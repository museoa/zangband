/* File: widget1-dll.c */

/* Purpose: graphical cave-displaying widget */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/* Why did I call it "Widget"? Why? WHY?! Aargh! */

#define HAVE_LIMITS_H
#define HAVE_UNISTD_H
#include <tclInt.h>
#include <tkInt.h>
#include "util-dll.h"
#include "plat-dll.h"
#include "widget-dll.h"

char *keyword_widget_style[] = {
	"icon", "map", "iso",
	NULL
};

/*
 * Table specifying legal configuration options for a Widget.
 */
static Tk_OptionSpec optionSpecs[20] = {
    {TK_OPTION_INT, "-height", "height", "Height",
     "100", -1, Tk_Offset(Widget, height), 0, 0, 0},
    {TK_OPTION_INT, "-width", "width", "Width",
     "100", -1, Tk_Offset(Widget, width), 0, 0, 0},
    {TK_OPTION_INT, "-gheight", "gheight", "Height",
     "32", -1, Tk_Offset(Widget, gheight), 0, 0, 0},
    {TK_OPTION_INT, "-gwidth", "gwidth", "Width",
     "32", -1, Tk_Offset(Widget, gwidth), 0, 0, 0},
    {TK_OPTION_CURSOR, "-cursor", "cursor", "Cursor",
     "", -1, Tk_Offset(Widget, cursor), TK_OPTION_NULL_OK, 0, 0},
    {TK_OPTION_BOOLEAN, "-setgrid", "setGrid", "SetGrid",
	 "no", -1, Tk_Offset(Widget, setGrid), 0, 0, 0},
    {TK_OPTION_BOOLEAN, "-noupdate", "noUpdate", "NoUpdate",
	 "no", -1, Tk_Offset(Widget, noUpdate), 0, 0, 0},
   {TK_OPTION_STRING_TABLE, "-style", "style", "Style",
	 "icon", -1, Tk_Offset(Widget, style), 0, keyword_widget_style, 0},
    {TK_OPTION_INT, "-hit", "", "",
     "-1", -1, Tk_Offset(Widget, hit), 0, 0, 0},
    {TK_OPTION_INT, "-hitx", "", "",
     "-1", -1, Tk_Offset(Widget, hitx), 0, 0, 0},
    {TK_OPTION_INT, "-hity", "", "",
     "-1", -1, Tk_Offset(Widget, hity), 0, 0, 0},
    {TK_OPTION_END, NULL, NULL, NULL,
     NULL, 0, -1, 0, 0, 0}
};

static Tk_OptionTable optionTable = None;

/* forward declarations */

int Widget_ObjCmd _ANSI_ARGS_((ClientData clientData,
  Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));
int Widget_Configure _ANSI_ARGS_((Tcl_Interp *interp,
  Widget *widgetPtr, int objc, Tcl_Obj *CONST objv[]));
void Widget_WorldChanged(ClientData instanceData);
void Widget_CmdDeletedProc _ANSI_ARGS_((ClientData clientData));
void Widget_EventProc _ANSI_ARGS_((ClientData clientData,
  XEvent *eventPtr));
int Widget_WidgetObjCmd _ANSI_ARGS_((ClientData clientData,
  Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));
int Widget_Photo(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);
void Widget_Calc(Widget *widgetPtr);

static void Widget_CreateBitmap(Widget *widgetPtr);
static void Widget_DeleteBitmap(Widget *widgetPtr);

/* Table of procedures for the "Widget" class */
TkClassProcs widgetProcs = { 
	NULL,						/* createProc. */
	Widget_WorldChanged,				/* geometryProc. */
    NULL							/* modalProc. */ 
};

int debug_widgets = 0;

/* List of existing widgets let us iterate over them */
DoubleLinker WidgetList;

/* List of mapped widgets */
DoubleLinker WidgetListMap;

static Widget_CreateProc *g_create_proc;

static void WindowToBitmap(Widget *widgetPtr, int *y, int *x)
{
	*y += widgetPtr->by;
	*x += widgetPtr->bx;
}

#define BAD_COLOR(c) (((c) < 0) || ((c) > 255))

/* Array of all allocated Widget item colors */
t_widget_color **g_widget_color;

/* Number of allocated Widget item colors */
int g_widget_color_count;

/*
 * Initialize the Widget item color package
 */
int WidgetColor_Init(Tcl_Interp *interp)
{
	g_widget_color = Array_New(1, sizeof(t_widget_color *));
	g_widget_color_count = 0;
	return TCL_OK;
}

/*
 * Search for an existing Widget item color of the given properties.
 * Return NULL if no such color was allocated.
 */
t_widget_color *WidgetColor_Find(int color, int opacity)
{
	int i;

	/* Check each color */
	for (i = 0; i < g_widget_color_count; i++)
	{
		/* Skip free'd colors */
		if (g_widget_color[i]->ref_cnt == 0) continue;
		
		/* This is a match */
		if ((g_widget_color[i]->color == color) &&
			(g_widget_color[i]->opacity == opacity))
		{
			/* Return address of color */
			return g_widget_color[i];
		}
	}

	/* Not found */
	return NULL;
}

/*
 * Search for an already allocated but unused Widget item color.
 */
t_widget_color *WidgetColor_FindFree(void)
{
	int i;

	/* Check each color */
	for (i = 0; i < g_widget_color_count; i++)
	{
		/* This is free */
		if (g_widget_color[i]->ref_cnt == 0)
		{
			/* Return the address */
			return g_widget_color[i];
		}
	}

	/* None are free */
	return NULL;
}

/*
 * Allocates a new Widget item color of the given properties. If such
 * a color already exists, its references count is incremented and
 * the color is returned. Otherwise a new struct is allocated.
 */
t_widget_color *WidgetColor_Alloc(int color, int opacity)
{
	t_widget_color *color_ptr;

	/* Look for an existing color */
	if ((color_ptr = WidgetColor_Find(color, opacity)))
	{
		/* Increase the reference count */
		color_ptr->ref_cnt++;

		/* Return the address */
		return color_ptr;
	}

	/* Look for a free struct */
	if (!(color_ptr = WidgetColor_FindFree()))
	{
		/* Allocate a struct */
		color_ptr = (t_widget_color *) Tcl_AllocDebug(sizeof(t_widget_color));

		/* Append pointer to the global array */
		g_widget_color = Array_Insert(g_widget_color,
			&g_widget_color_count, sizeof(t_widget_color *),
			g_widget_color_count);

		/* Remember the new struct */
		g_widget_color[g_widget_color_count - 1] = color_ptr;
	}

	/* Set the fields */
	color_ptr->ref_cnt = 1;
	color_ptr->color = color;
	color_ptr->opacity = opacity;

	/* Calculate the tint table */
	Colormap_TintTable(g_palette2colormap[color], opacity, color_ptr->tint);

	/* Return the address */
	return color_ptr;
}

/*
 * Decrement the reference count for a Widget item color. We never
 * actually free the memory for a struct with zero references, since
 * we allow them to be reused.
 */
void WidgetColor_Deref(t_widget_color *color_ptr)
{
	/* Allow NULL color_ptr */
	if (!color_ptr) return;

	/* Decrement the reference count */
	if (--color_ptr->ref_cnt <= 0)
	{
		/* Mark the color as unused */
		color_ptr->ref_cnt = 0;
		color_ptr->color = 0;
		color_ptr->opacity = 0;
	}
}

/*
 *--------------------------------------------------------------
 *
 * Widget_ObjCmd --
 *
 *	Called by Tcl to implement the "widget" command. This is
 *	the procedure passed to Tcl_CreateCommand() inside
 *	Widget_Init().
 *
 *--------------------------------------------------------------
 */

int Widget_ObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Widget *widgetPtr;
	Tk_Window tkwin;

	optionTable = (Tk_OptionTable) clientData;
	if (optionTable == NULL)
	{
		Tcl_CmdInfo info;
		char *name;

		/*
		* We haven't created the option table for this widget class
		* yet.  Do it now and save the table as the clientData for
		* the command, so we'll have access to it in future
		* invocations of the command.
		*/

		optionTable = Tk_CreateOptionTable(interp, optionSpecs);
		name = Tcl_GetString(objv[0]);
		Tcl_GetCommandInfo(interp, name, &info);
		info.objClientData = (ClientData) optionTable;
		Tcl_SetCommandInfo(interp, name, &info);
	}

	/* Required number of arguments */
    if (objc < 2)
    {
		/* Set the error */
		Tcl_WrongNumArgs(interp, 1, objv, "pathName ?options?");

		/* Failure */
		return(TCL_ERROR);
    }

	/* Create a new Tk window with the given name */
    tkwin = Tk_CreateWindowFromPath(interp, Tk_MainWindow(interp),
	    Tcl_GetString(objv[1]), NULL);

	/* The window could not be created */
	if (tkwin == NULL)
	{
		return TCL_ERROR;
	}

	/*
	 * Set the window class. By convention all class names start with
	 * a capital letter, and there exists a Tcl command with the same
	 * name as each class.
	 */
    Tk_SetClass(tkwin, "Widget");

    /* Allocate a new Widget struct */
    if ((*g_create_proc)(interp, &widgetPtr) != TCL_OK)
    {
    	return TCL_ERROR;
    }

	/* Set the class callbacks for the new Widget */
    TkSetClassProcs(tkwin, &widgetProcs, (ClientData) widgetPtr);

	/* Set some fields */
	widgetPtr->tkwin = tkwin;
	widgetPtr->display = Tk_Display(tkwin);
	widgetPtr->interp = interp;

	/*
	 * Note here we are creating a new Tcl command that has the same
	 * name as the full pathname of the new Tk window (this Widget).
	 */
	widgetPtr->widgetCmd = Tcl_CreateObjCommand(interp,
		Tk_PathName(tkwin), Widget_WidgetObjCmd, (ClientData) widgetPtr,
		Widget_CmdDeletedProc);

	/* Set more fields */
	widgetPtr->copyGC = None;
	widgetPtr->bitmap.pixelPtr = NULL;
	widgetPtr->bitmap.pixmap = None;
    widgetPtr->width = 0;
    widgetPtr->height = 0;
    widgetPtr->gwidth = 0;
    widgetPtr->gheight = 0;
    widgetPtr->oldWidth = widgetPtr->oldHeight = 0;
    widgetPtr->oldGWidth = widgetPtr->oldGHeight = 0;
    widgetPtr->oldStyle = -1;
    widgetPtr->cursor = None;
	widgetPtr->setGrid = FALSE;
    widgetPtr->takeFocus = NULL;
    widgetPtr->flags = 0;
	widgetPtr->y = widgetPtr->x = 0;
	widgetPtr->y_min = widgetPtr->y_max = 0;
	widgetPtr->x_min = widgetPtr->x_max = 0;
	DoubleLink_Init(&WidgetList, &widgetPtr->link, widgetPtr);
	DoubleLink_Init(&WidgetListMap, &widgetPtr->linkMap, widgetPtr);
	DoubleLink_Init(&widgetPtr->linkerItem, NULL, NULL);
widgetPtr->linkerItem.what = "item";
	DoubleLink_Init(&widgetPtr->linkerItemVis, NULL, NULL);
widgetPtr->linkerItemVis.what = "itemVis";
	widgetPtr->noUpdate = FALSE;
	widgetPtr->dx = widgetPtr->dy = 0;
	widgetPtr->dw = widgetPtr->dh = 0;
	widgetPtr->info = NULL;
	widgetPtr->invalid = NULL;
	widgetPtr->anim = NULL;
	widgetPtr->yp = widgetPtr->xp = NULL;

	/*
	 * Arrange for our routine to be called when any of the specified
	 * events occur to our new Widget.
	 */
    Tk_CreateEventHandler(widgetPtr->tkwin,
		ExposureMask | StructureNotifyMask | FocusChangeMask,
		Widget_EventProc, (ClientData) widgetPtr);

	/* Set the default options for the new widget */
	if (Tk_InitOptions(interp, (char *) widgetPtr, optionTable, tkwin)
		!= TCL_OK)
	{
		/* Destroy the Tk window */
		Tk_DestroyWindow(widgetPtr->tkwin);

		/* Failure */
		return TCL_ERROR;
	}
	
	/* Parse the rest of the arguments for option/value pairs */
	if (Widget_Configure(interp, widgetPtr, objc - 2, objv + 2) != TCL_OK)
	{
		/* Destroy the Tk window */
		Tk_DestroyWindow(widgetPtr->tkwin);

		/* Failure */
		return TCL_ERROR;
	}

	/* Add another widget to the list */
	DoubleLink_Link(&widgetPtr->link);
	
	/* Return the window pathname */
    Tcl_SetStringObj(Tcl_GetObjResult(interp), Tk_PathName(widgetPtr->tkwin),
    	-1);

	/* Success */
    return TCL_OK;
}

/*
 * This is the window-specific command created for each new Widget.
 */
int Widget_WidgetObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	static char *commandNames[] = {"caveyx", "center", "cget", "configure",
		"coloralloc", "colorderef", "create", "itemcget", "itemconfigure",
		"photo", "wipe", "bounds", "visible", "wipespot",
		"hittest", NULL};
	enum {IDX_CAVEYX, IDX_CENTER, IDX_CGET, IDX_CONFIGURE,
		IDX_COLORALLOC, IDX_COLORDEREF, IDX_CREATE, IDX_ITEMCGET, IDX_ITEMCONFIGURE,
		IDX_PHOTO, IDX_WIPE, IDX_BOUNDS, IDX_VISIBLE,
		IDX_WIPESPOT, IDX_HITTEST} option;
	Widget *widgetPtr = (Widget *) clientData;
	int result;
    Tcl_Obj *objPtr;

	/* Required number of arguments */
	if (objc < 2)
	{
		/* Set the error */
		Tcl_WrongNumArgs(interp, 1, objv, "option ?arg arg ...?");

		/* Failure */
		return TCL_ERROR;
	}

	result = Tcl_GetIndexFromObj(interp, objv[1], commandNames,
		"option", 0, (int *) &option);
	if (result != TCL_OK)
	{
		return result;
	}

	/*
	 * Since this command could possibly destroy the window, and
	 * the window may be in use somewhere up the calling stack,
	 * we increase a reference count to prevent the memory being
	 * freed too soon.
	 */
    Tcl_Preserve((ClientData) widgetPtr);

    switch (option)
    {
		case IDX_CAVEYX: /* caveyx */
		{
			int y, x, yc, xc;
			int row, col;
			char buffer[20];

			/* Get x coord */
			if (Tcl_GetIntFromObj(interp, objv[2], &x) != TCL_OK)
			{
				goto error;
			}

			/* Get y coord */
			if (Tcl_GetIntFromObj(interp, objv[3], &y) != TCL_OK)
			{
				goto error;
			}

			if (x < 0 || x >= widgetPtr->width)
				break;
			if (y < 0 || y >= widgetPtr->height)
				break;

			WindowToBitmap(widgetPtr, &y, &x);
	
			row = y / widgetPtr->gheight;
			col = x / widgetPtr->gwidth;
	
			yc = widgetPtr->y_min + row;
			xc = widgetPtr->x_min + col;
				
			(void) sprintf(buffer, "%d %d", yc, xc);
			Tcl_SetStringObj(Tcl_GetObjResult(interp), buffer, -1);

			break;
		}

		case IDX_CENTER: /* center */
		{
			char buffer[20];
	
			/* New coordinates were given */
			if (objc == 4)
			{
				int y, x;
	
				/* Get y location */
				if (Tcl_GetIntFromObj(interp, objv[2], &y) != TCL_OK)
				{
					goto error;
				}
	
				/* Get x location */
				if (Tcl_GetIntFromObj(interp, objv[3], &x) != TCL_OK)
				{
					goto error;
				}
	
				/* Center the widget */
				Widget_Center(widgetPtr, y, x);
			}
			else
			{
				/* Print the current center */
				(void) sprintf(buffer, "%d %d", widgetPtr->y, widgetPtr->x);
		
				/* Return the center */
				Tcl_SetStringObj(Tcl_GetObjResult(interp), buffer, -1);
			}
			break;
		}

		case IDX_CGET: /* cget */
		{
			/* Required number of arguments */
			if (objc != 3)
			{
				/* Set the error */
				Tcl_WrongNumArgs(interp, 2, objv, "option");
	
				/* Failure */
				goto error;
			}
		
			objPtr = Tk_GetOptionValue(interp, (char *) widgetPtr,
				optionTable, objv[2], widgetPtr->tkwin);
			if (objPtr == NULL)
			{
				goto error;
			}
			else
			{
				Tcl_SetObjResult(interp, objPtr);
			}
			break;
		}

		case IDX_CONFIGURE: /* configure */
		{
			if (objc <= 3)
			{
				objPtr = Tk_GetOptionInfo(interp, (char *) widgetPtr,
					optionTable, (objc == 3) ? objv[2] : NULL,
					widgetPtr->tkwin);
				if (objPtr == NULL)
				{
					goto error;
				}
				else
				{
					Tcl_SetObjResult(interp, objPtr);
				}
			}
			else
			{
				result = Widget_Configure(interp, widgetPtr, objc - 2,
					objv + 2);
			}
			break;
		}

		/*
		 * Each Widget item may have a number of Widget item colors. Sometimes
		 * it is desireable for a single item to have more than one set of
		 * colors (for example, a progress bar that has different colors for
		 * a friendly versus non-friendly monster). Since it would be inefficent
		 * to calculate the new tint table every time the item colors change,
		 * we implement reference counts for item colors. The following two
		 * options allow us to artificially increase or decrease the reference
		 * count for an arbitrary item color. Note that the item color is not
		 * item-specific.
		 */
		 
		case IDX_COLORALLOC: /* coloralloc */
		{
			int color, opacity;
	
			/* Required number of arguments */
			if (objc != 4)
			{
				/* Set the error */
				Tcl_WrongNumArgs(interp, 2, objv,
					"paletteIndex opacity");
	
				/* Failure */
				goto error;
			}
	
			/* Get the palette index */
			if (Tcl_GetIntFromObj(interp, objv[2], &color) != TCL_OK)
			{
				goto error;
			}
	
			/* Verify the palette index */
			if (BAD_COLOR(color))
			{
				/* Set the error */
				Tcl_SetStringObj(Tcl_GetObjResult(interp),
					format("bad color \"%d\"", color), -1);
	
				/* Failure */
				goto error;
			}
	
			/* Get the opacity */
			if (Tcl_GetIntFromObj(interp, objv[3], &opacity) != TCL_OK)
			{
				goto error;
			}
	
			/* Verify the opacity */
			if (BAD_COLOR(opacity))
			{
				/* Set the error */
				Tcl_SetStringObj(Tcl_GetObjResult(interp),
					format("bad opacity \"%d\"", opacity), -1);
	
				/* Failure */
				goto error;
			}
	
			/* Allocate a Widget item color */
			(void) WidgetColor_Alloc(color, opacity);

			break;
		}

		case IDX_COLORDEREF: /* colorderef */
		{
			int color, opacity;
			t_widget_color *color_ptr;
			
			/* Required number of arguments */
			if (objc != 4)
			{
				/* Set the error */
				Tcl_WrongNumArgs(interp, 2, objv, "paletteIndex opacity");
	
				/* Failure */
				goto error;
			}
	
			/* Get the palette index */
			if (Tcl_GetIntFromObj(interp, objv[2], &color) != TCL_OK)
			{
				goto error;
			}
	
			/* Verify the palette index */
			if (BAD_COLOR(color))
			{
				/* Set the error */
				Tcl_SetStringObj(Tcl_GetObjResult(interp),
					format("bad color \"%d\"", color), -1);
	
				/* Failure */
				goto error;
			}
	
			/* Get the opacity */
			if (Tcl_GetIntFromObj(interp, objv[3], &opacity) != TCL_OK)
			{
				goto error;
			}
	
			/* Verify the opacity */
			if (BAD_COLOR(opacity))
			{
				/* Set the error */
				Tcl_SetStringObj(Tcl_GetObjResult(interp),
					format("bad opacity \"%d\"", opacity), -1);
	
				/* Failure */
				goto error;
			}
	
			/* Look for an existing color */
			color_ptr = WidgetColor_Find(color, opacity);
	
			/* We found the color */
			if (color_ptr)
			{
				/* Dereference */
				WidgetColor_Deref(color_ptr);
			}
			break;
		}

		case IDX_CREATE: /* create */
		{
			/* Create a new Widget item */
			result = WidgetItem_Create(interp, widgetPtr, objc, objv);
			break;
		}

		case IDX_ITEMCGET: /* itemcget */
		{
			/* Query a Widget item option */
			result = WidgetItem_Cget(interp, widgetPtr, objc, objv);
			break;
		}

		case IDX_ITEMCONFIGURE: /* itemconfigure */
		{
			/* Configure a Widget item */
			result = WidgetItem_Configure(interp, widgetPtr, objc, objv);
			break;
		}

		case IDX_PHOTO: /* photo */
		{
			/* Set a photo image with the widget contents */
			result = Widget_Photo(clientData, interp, objc, objv);
			break;
		}

		case IDX_WIPE: /* wipe */
		{
			/* Schedule a complete redraw */
			Widget_Wipe(widgetPtr);
			break;
		}

		case IDX_BOUNDS: /* bounds */
		{
			char buf[32];
			(void) sprintf(buf, "%d %d %d %d",
				widgetPtr->y_min, widgetPtr->x_min,
				widgetPtr->y_max - 1, widgetPtr->x_max - 1);
			Tcl_SetStringObj(Tcl_GetObjResult(interp), buf, -1);
			break;
		}

		case IDX_VISIBLE: /* visible */
		{
			int y, x, r, c, vis;

			/* Required number of arguments */
			if (objc != 4)
			{
				/* Set the error */
				Tcl_WrongNumArgs(interp, 2, objv, "y x");
	
				/* Failure */
				goto error;
			}

			/* Get y location */
			if (Tcl_GetIntFromObj(interp, objv[2], &y) != TCL_OK)
			{
				goto error;
			}

			/* Get x location */
			if (Tcl_GetIntFromObj(interp, objv[3], &x) != TCL_OK)
			{
				goto error;
			}

			vis = Widget_CaveToView(widgetPtr, y, x, &r, &c);
			if (vis)
			{
				int yp, xp, h, w;

				yp = r * widgetPtr->gheight - widgetPtr->by;
				xp = c * widgetPtr->gwidth - widgetPtr->bx;
				h = widgetPtr->gheight;
				w = widgetPtr->gwidth;
				
				if (yp < 0 || yp + h > widgetPtr->height ||
					xp < 0 || xp + w > widgetPtr->width)
				{
					vis = FALSE;
				}
			}
			
			Tcl_SetBooleanObj(Tcl_GetObjResult(interp), vis);
			break;
		}

		/* For debugging */
		case IDX_WIPESPOT: /* wipespot */
		{
			int y, x, r, c;

			/* Get y location */
			if (Tcl_GetIntFromObj(interp, objv[2], &y) != TCL_OK)
			{
				goto error;
			}

			/* Get x location */
			if (Tcl_GetIntFromObj(interp, objv[3], &x) != TCL_OK)
			{
				goto error;
			}

			if (!Widget_CaveToView(widgetPtr, y, x, &r, &c))
				break;
			Widget_Invalidate(widgetPtr, r, c);
			widgetPtr->flags |= WIDGET_DRAW_INVALID;
			Widget_EventuallyRedraw(widgetPtr);
			break;
		}

		case IDX_HITTEST: /* hittest */
		{
			int y, x, row, col, yc, xc;
			int layer = -1;
			char buffer[20];

			/* Get x coord */
			if (Tcl_GetIntFromObj(interp, objv[2], &x) != TCL_OK)
			{
				goto error;
			}

			/* Get y coord */
			if (Tcl_GetIntFromObj(interp, objv[3], &y) != TCL_OK)
			{
				goto error;
			}

			if (x < 0 || x >= widgetPtr->width)
				break;
			if (y < 0 || y >= widgetPtr->height)
				break;

			WindowToBitmap(widgetPtr, &y, &x);
	
			row = y / widgetPtr->gheight;
			col = x / widgetPtr->gwidth;
	
			yc = widgetPtr->y_min + row;
			xc = widgetPtr->x_min + col;

			/* Refine the hit */
			if (widgetPtr->hitTestProc)
				layer = (*widgetPtr->hitTestProc)(widgetPtr, x, y, col, row, &xc, &yc);

			(void) sprintf(buffer, "%d %d %d", yc, xc, layer);
			Tcl_SetStringObj(Tcl_GetObjResult(interp), buffer, -1);
			
			break;
		}
	}
	
	/* Decrease the reference count */
	Tcl_Release((ClientData) widgetPtr);

	/* Result */
	return result;

error:

	/* Decrease the reference count */
	Tcl_Release((ClientData) widgetPtr);

	/* Failure */
	return TCL_ERROR;
}

static void Tcl_FreeDebugHack(char *ptr)
{
	Tcl_FreeDebug(ptr);
}

/*
 * This procedure is invoked by Tcl_EventuallyFree() or Tcl_Release()
 * to clean up the internal structure of a Widget at a safe time
 * (when no-one is using it anymore).
 */
void Widget_Destroy(Widget *widgetPtr)
{
    widgetPtr->flags |= WIDGET_DELETED;

	if (widgetPtr->flags & WIDGET_REDRAW)
	{
		Tcl_CancelIdleCall(Widget_Display, (ClientData) widgetPtr);
	}

    /*
     * Free up all the stuff that requires special handling, then
     * let Tk_FreeOptions handle all the standard option-related
     * stuff.
     */

    Tcl_DeleteCommandFromToken(widgetPtr->interp, widgetPtr->widgetCmd);

	/* Client command */
	if (widgetPtr->destroyProc)
		(*widgetPtr->destroyProc)(widgetPtr);

	/* Free a GC */ 
    if (widgetPtr->copyGC != None)
    {
		Tk_FreeGC(widgetPtr->display, widgetPtr->copyGC);
    }

	/* Free the bitmap */
	if (widgetPtr->bitmap.pixelPtr != NULL)
	{
		Widget_DeleteBitmap(widgetPtr);
	}

	/*
	 * Free each Widget item. It is very important that all Tk fonts
	 * are freed.
	 */
	while (widgetPtr->linkerItem.head)
	{
		WidgetItem_Delete(widgetPtr,
			DoubleLink_Data(widgetPtr->linkerItem.head, WidgetItem));
	}

	/*
	 * Removes the deleted Widget from the global list of Widgets.
	 */
	DoubleLink_Unlink(&widgetPtr->link);

	/* Remove from the list of mapped Widgets */
	DoubleLink_Unlink(&widgetPtr->linkMap);
	
	/* Free the options table */
	Tk_FreeConfigOptions((char *) widgetPtr, optionTable,
		widgetPtr->tkwin);

	if (widgetPtr->anim)
		Tcl_FreeDebug(widgetPtr->anim);
	if (widgetPtr->info)
		Tcl_FreeDebug(widgetPtr->info);
	if (widgetPtr->invalid)
		Tcl_FreeDebug(widgetPtr->invalid);
	if (widgetPtr->yp)
		Tcl_FreeDebug(widgetPtr->yp);
	if (widgetPtr->xp)
		Tcl_FreeDebug(widgetPtr->xp);

	widgetPtr->tkwin = NULL;

	/* Free the Widget struct */
    Tcl_EventuallyFree((ClientData) widgetPtr, Tcl_FreeDebugHack);
}

/*
 * Fiddle with configuration options for a Widget
 */
int Widget_Configure(Tcl_Interp *interp, Widget *widgetPtr, int objc, Tcl_Obj *CONST objv[])
{
	Tk_SavedOptions savedOptions;
	Tcl_Obj *errorResult = NULL;
	int error;

    /*
     * The following loop is potentially executed twice.  During the
     * first pass configuration options get set to their new values.
     * If there is an error in this pass, we execute a second pass
     * to restore all the options to their previous values.
     */

    for (error = 0; error <= 1; error++)
    {
    	if (!error)
    	{
		    /*
		     * First pass: set options to new values.
		     */
			if (Tk_SetOptions(interp, (char *) widgetPtr, optionTable, objc, objv,
				widgetPtr->tkwin, &savedOptions, NULL) != TCL_OK)
			{
				continue;
			}
		}
		else
		{
		    /*
		     * Second pass: restore options to old values.
		     */
		    errorResult = Tcl_GetObjResult(interp);
		    Tcl_IncrRefCount(errorResult);
		    Tk_RestoreSavedOptions(&savedOptions);
		}

		/* Require gwidth == gheight. Why not have just one option? */
		if (widgetPtr->gwidth != widgetPtr->gheight)
		{
			/* Set the error */
			Tcl_AppendResult(interp, "expected gwidth equal to gheight", NULL);
	
			/* Failure */
			continue;
		}

		/* Client command */
		if (widgetPtr->configureProc)
		{
			if ((*widgetPtr->configureProc)(interp, widgetPtr) != TCL_OK)
				continue;
		}

		break;
	}
	if (!error)
	{
		Tk_FreeSavedOptions(&savedOptions);
	}

	/* Recompute geometry, etc */
    Widget_WorldChanged((ClientData) widgetPtr);

	if (error)
	{
		Tcl_SetObjResult(interp, errorResult);
		Tcl_DecrRefCount(errorResult);

		/* Failure */
		return TCL_ERROR;
	}

	/* Success */
	return TCL_OK;
}

/*
 * This procedure is called when the world has changed in some
 * way and the widget needs to recompute all its graphics contexts
 * and determine its new geometry.
 */
void Widget_WorldChanged(ClientData instanceData)
{
    Widget *widgetPtr = (Widget *) instanceData;
	Tk_Window tkwin = widgetPtr->tkwin;
    XGCValues gcValues;
	WidgetItem *itemPtr;

	/* Allocate GC */
    if (widgetPtr->copyGC == None)
    {
		gcValues.function = GXcopy;
		gcValues.graphics_exposures = False;
		widgetPtr->copyGC = Tk_GetGC(tkwin, GCFunction | GCGraphicsExposures,
			&gcValues);
    }

	/* Free old style data */
	if (widgetPtr->style != widgetPtr->oldStyle)
	{
		if (widgetPtr->anim)
		{
			Tcl_FreeDebug((char *) widgetPtr->anim);
			widgetPtr->anim = NULL;
		}
		if (widgetPtr->info)
		{
			Tcl_FreeDebug((char *) widgetPtr->info);
			widgetPtr->info = NULL;
		}
		if (widgetPtr->invalid)
		{
			Tcl_FreeDebug((char *) widgetPtr->invalid);
			widgetPtr->invalid = NULL;
		}
		if (widgetPtr->yp)
		{
			Tcl_FreeDebug((char *) widgetPtr->yp);
			widgetPtr->yp = NULL;
		}
		if (widgetPtr->xp)
		{
			Tcl_FreeDebug((char *) widgetPtr->xp);
			widgetPtr->xp = NULL;
		}
	}

	/* Style or size changed */
	if ((widgetPtr->style != widgetPtr->oldStyle) ||
		(widgetPtr->width != widgetPtr->oldWidth) ||
		(widgetPtr->height != widgetPtr->oldHeight) ||
		(widgetPtr->gwidth != widgetPtr->oldGWidth) ||
		(widgetPtr->gheight != widgetPtr->oldGHeight))
	{
		Widget_Calc(widgetPtr);
	}

	/* The bitmap is not the right size */
	if ((widgetPtr->bitmap.pixelPtr != NULL) &&
		((widgetPtr->bw != widgetPtr->bitmap.width) ||
		(widgetPtr->bh != widgetPtr->bitmap.height)))
	{
		/* Delete the bitmap */
		Widget_DeleteBitmap(widgetPtr);

		/* Forget the bitmap */
		widgetPtr->bitmap.pixelPtr = NULL;
	}

	/* No bitmap yet */
	if (widgetPtr->bitmap.pixelPtr == NULL)
	{
		DoubleLink *link;

		/* Allocate bitmap */
		Widget_CreateBitmap(widgetPtr);

		/* Check each Widget item */
		for (link = widgetPtr->linkerItem.head; link; link = link->next)
		{
			itemPtr = DoubleLink_Data(link, WidgetItem);
			
			/*
			 * Some Widget items are not allowed to be out of bounds. In such
			 * cases, we call a item-type-specific callback to allow the
			 * item to move itself to a safe position.
			 */
			if (itemPtr->typePtr->changedProc)
			{
				(void) (*itemPtr->typePtr->changedProc)(widgetPtr->interp,
					widgetPtr, itemPtr);
			}
		}
	}

	if (widgetPtr->noUpdate)
	{
		widgetPtr->flags |= WIDGET_NO_UPDATE;
	}
	else
	{
		widgetPtr->flags &= ~WIDGET_NO_UPDATE;
	}

	/* The widget is of non-zero size */
    if ((widgetPtr->width > 0) && (widgetPtr->height > 0))
	{
		/* Request geometry */
		Tk_GeometryRequest(tkwin, widgetPtr->width, widgetPtr->height);
	}

	/* We want to control gridded geometry of its toplevel */
	if (widgetPtr->setGrid)
	{
		/* Turn on gridded geometry management for the toplevel */
		Tk_SetGrid(tkwin, widgetPtr->cc, widgetPtr->rc,
			widgetPtr->gwidth, widgetPtr->gheight);
	}

	/* We do not want to control gridded geometry of its toplevel */
	else
	{
		/* Cancel gridded geometry management for the toplevel */
		Tk_UnsetGrid(widgetPtr->tkwin);
	}

	/* Client command */
	if (widgetPtr->changedProc)
		(*widgetPtr->changedProc)(widgetPtr);

	/* Remember the current info */
	widgetPtr->oldTileCnt = widgetPtr->tc;
	widgetPtr->oldWidth = widgetPtr->width;
	widgetPtr->oldHeight = widgetPtr->height;
	widgetPtr->oldGWidth = widgetPtr->gwidth;
	widgetPtr->oldGHeight = widgetPtr->gheight;
	widgetPtr->oldStyle = widgetPtr->style;

	/* Redraw the window (later) */
	Widget_EventuallyRedraw(widgetPtr);
}

/*
 * This procedure is invoked by the Tk dispatcher for various
 * events on a Widget.
 */
void Widget_EventProc(ClientData clientData, XEvent *eventPtr)
{
    Widget *widgetPtr = (Widget *) clientData;

	/* A region of the window became newly visible */
	if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0))
	{
		/* Redraw the window */
		goto redraw;
	}

	/* The window was resized */
	else if (eventPtr->type == ConfigureNotify)
	{
		/* Redraw the window */
		goto redraw;
	}

	/* The window is about to be destroyed */
	else if (eventPtr->type == DestroyNotify)
	{
		/* Destroy the window */
		Widget_Destroy(widgetPtr);
	}

	/* The window was mapped */
	else if (eventPtr->type == MapNotify)
	{
		DoubleLink_Link(&widgetPtr->linkMap);
	}

	/* The window was unmapped */
	else if (eventPtr->type == UnmapNotify)
	{
		DoubleLink_Unlink(&widgetPtr->linkMap);
	}

	/* Done */
	return;

redraw:

	/* Redraw later */
	widgetPtr->flags |= WIDGET_EXPOSE;
	Widget_EventuallyRedraw(widgetPtr);
}

/*
 * This procedure is invoked when a Widget command is deleted. If
 * the Widget isn't already in the process of being destroyed,
 * this command destroys it.
 */
void Widget_CmdDeletedProc(ClientData clientData)
{
    Widget *widgetPtr = (Widget *) clientData;

    /*
     * This procedure could be invoked either because the window was
     * destroyed and the command was then deleted or because the command
     * was deleted, and then this procedure destroys the widget.  The
     * WIDGET_DELETED flag distinguishes these cases.
     */

	if (!(widgetPtr->flags & WIDGET_DELETED))
	{
		Tk_DestroyWindow(widgetPtr->tkwin);
	}
}

/*
 * Actually draw stuff into the Widget's display. This routine is
 * usually passed to Tcl_DoWhenIdle().
 */
void Widget_Display(ClientData clientData)
{
	Widget *widgetPtr = (Widget *) clientData;
	Tk_Window tkwin = widgetPtr->tkwin;

	/* We want to draw all grids */
	if ((widgetPtr->flags & WIDGET_WIPE) != 0)
	{
		/* Forget that a wipe (and redraw) is needed */
		widgetPtr->flags &= ~(WIDGET_WIPE | WIDGET_DRAW_INVALID);

		/* Draw all grids */
		Widget_DrawAll(widgetPtr);
	}

	/* We want to draw outdated grids */
	if ((widgetPtr->flags & WIDGET_DRAW_INVALID) != 0)
	{
		/* Forget that a redraw is needed */
		widgetPtr->flags &= ~WIDGET_DRAW_INVALID;

		/* Draw outdated grids (offscreen) */
		Widget_DrawInvalid(widgetPtr);
	}

	/* Forget that a redraw is scheduled */
	widgetPtr->flags &= ~WIDGET_REDRAW;

	/* The window doesn't exist, or it is not mapped */
	if ((tkwin == NULL) || (!Tk_IsMapped(tkwin)))
	{		
		/* Done */
		return;
	}

	if (widgetPtr->flags & WIDGET_EXPOSE)
	{
		/* Reset dirty bounds to entire window */
		widgetPtr->dx = widgetPtr->bx;
		widgetPtr->dy = widgetPtr->by;
		widgetPtr->dw = widgetPtr->width;
		widgetPtr->dh = widgetPtr->height;

		/* Forget expose flag */
		widgetPtr->flags &= ~WIDGET_EXPOSE;
	}

#if 0
	/* Use Tk_SetWindowBackgroundPixmap() for more speed */
	XClearWindow(widgetPtr->display, Tk_WindowId(tkwin));
#endif

	XCopyArea(widgetPtr->display,
		widgetPtr->bitmap.pixmap, /* source drawable */
		Tk_WindowId(tkwin), /* dest drawable */
		widgetPtr->copyGC, /* graphics context */
		widgetPtr->dx, widgetPtr->dy, /* source top-left */
		(unsigned int) widgetPtr->dw, /* width */
		(unsigned int) widgetPtr->dh, /* height */
		widgetPtr->dx - widgetPtr->bx,
		widgetPtr->dy - widgetPtr->by /* dest top-left */
	);

	Plat_SyncDisplay(widgetPtr->display);

	/* Reset dirty bounds to entire window */
	widgetPtr->dx = widgetPtr->bx;
	widgetPtr->dy = widgetPtr->by;
	widgetPtr->dw = widgetPtr->width;
	widgetPtr->dh = widgetPtr->height;
}

void Widget_Calc(Widget *widgetPtr)
{
	int i;
	int rc, cc;
	int dLeft, cLeft, dRight, cRight;
	int dTop, rTop, dBottom, rBottom;

	dLeft = (widgetPtr->width - widgetPtr->gwidth) / 2;
	cLeft = dLeft / widgetPtr->gwidth;
	if (dLeft % widgetPtr->gwidth)
		++cLeft;

	dRight = widgetPtr->width - dLeft - widgetPtr->gwidth;
	cRight = dRight / widgetPtr->gwidth;
	if (dRight % widgetPtr->gwidth)
		++cRight;

	dTop = (widgetPtr->height - widgetPtr->gheight) / 2;
	rTop = dTop / widgetPtr->gheight;
	if (dTop % widgetPtr->gheight)
		++rTop;

	dBottom = widgetPtr->height - dTop - widgetPtr->gheight;
	rBottom = dBottom / widgetPtr->gheight;
	if (dBottom % widgetPtr->gheight)
		++rBottom;

	cc = cLeft + 1 + cRight;
	rc = rTop + 1 + rBottom;

	widgetPtr->tc = rc * cc;
	widgetPtr->rc = rc;
	widgetPtr->cc = cc;

	widgetPtr->bw = cc * widgetPtr->gwidth;
	widgetPtr->bh = rc * widgetPtr->gheight;

	widgetPtr->bx = cLeft * widgetPtr->gwidth - dLeft;
	widgetPtr->by = rTop * widgetPtr->gheight - dTop;

	if (widgetPtr->anim)
		Tcl_FreeDebug((char *) widgetPtr->anim);
	if (widgetPtr->info)
		Tcl_FreeDebug((char *) widgetPtr->info);
	if (widgetPtr->invalid)
		Tcl_FreeDebug((char *) widgetPtr->invalid);

	widgetPtr->anim = (int *) Tcl_AllocDebug(sizeof(int) *
		widgetPtr->rc * widgetPtr->cc);
	widgetPtr->animCnt = 0;
	widgetPtr->info = (short *) Tcl_AllocDebug(sizeof(short) *
		widgetPtr->rc * widgetPtr->cc);
	widgetPtr->invalid = (int *) Tcl_AllocDebug(sizeof(int) *
		widgetPtr->rc * widgetPtr->cc);
	widgetPtr->invalidCnt = 0;

	for (i = 0; i < widgetPtr->rc * widgetPtr->cc; i++)
	{
		widgetPtr->info[i] = 0;
	}
}

/*
 * Create a bitmap as big as the given Widget. We get the address of
 * the bits so we can write directly into the bitmap. The bitmap is
 * 8-bit depth with a 256-color palette.
 */
static void Widget_CreateBitmap(Widget *widgetPtr)
{
	extern int g_icon_size, g_icon_depth;

	/* Calculate the bitmap dimensions in pixels */
	widgetPtr->bitmap.width = widgetPtr->bw;
	widgetPtr->bitmap.height = widgetPtr->bh;

	widgetPtr->bitmap.depth = ((widgetPtr->gwidth == g_icon_size) ? g_icon_depth : 8);
	widgetPtr->bitmap.depth = g_icon_depth;

	/* Create the bitmap */
	Bitmap_New(widgetPtr->interp, &widgetPtr->bitmap);
}

/* Free the bitmap for this Widget */
static void Widget_DeleteBitmap(Widget *widgetPtr)
{
	Bitmap_Delete(&widgetPtr->bitmap);
}

/* List of all Widget item types */
WidgetItemType *typeList = NULL;

/* Forward delcarations (static?) */
extern WidgetItemType ProgressType;
extern WidgetItemType TextType;
extern WidgetItemType CursorType;
extern WidgetItemType RectType;

/*
 * Initialize the Widget item package
 */
static int WidgetItem_Init(Tcl_Interp *interp)
{
	/* Add each Widget item type to the global list */
	typeList = &ProgressType;
	ProgressType.nextPtr = &TextType;
	TextType.nextPtr = &CursorType;
	CursorType.nextPtr = &RectType;
	RectType.nextPtr = NULL;

	return TCL_OK;
}

void Widget_Center(Widget *widgetPtr, int cy, int cx)
{
	/* Remember new center */
	widgetPtr->y = cy, widgetPtr->x = cx;

	/* Calculate the limits of visibility */
	widgetPtr->y_min = cy - widgetPtr->rc / 2;
	widgetPtr->y_max = widgetPtr->y_min + widgetPtr->rc;
	widgetPtr->x_min = cx - widgetPtr->cc / 2;
	widgetPtr->x_max = widgetPtr->x_min + widgetPtr->cc;

	Widget_Wipe(widgetPtr);
}

void Widget_DrawAll(Widget *widgetPtr)
{
	if (widgetPtr->drawAllProc)
		(*widgetPtr->drawAllProc)(widgetPtr);
}

void Widget_DrawInvalid(Widget *widgetPtr)
{
	if (widgetPtr->drawInvalidProc)
		(*widgetPtr->drawInvalidProc)(widgetPtr);
}

void Widget_Invalidate(Widget *widgetPtr, int row, int col)
{
	int rc = widgetPtr->rc;
	int cc = widgetPtr->cc;
	int tile;

if (widgetPtr->flags & WIDGET_WIPE) return;

	if (row < 0 || row >= rc)
		return;
	if (col < 0 || col >= cc)
		return;

	if (widgetPtr->style != WIDGET_STYLE_MAP)
	{
		tile = row * cc + col;
		if (widgetPtr->info[tile] & (WIDGET_INFO_DIRTY | WIDGET_INFO_IGNORE))
			return;
		widgetPtr->info[tile] |= WIDGET_INFO_DIRTY;
		widgetPtr->invalid[widgetPtr->invalidCnt++] = tile;
	}
	else
	{
		if (row < widgetPtr->dirty[1])
			widgetPtr->dirty[1] = row;
		if (row > widgetPtr->dirty[3])
			widgetPtr->dirty[3] = row;
		if (col < widgetPtr->dirty[0])
			widgetPtr->dirty[0] = col;
		if (col > widgetPtr->dirty[2])
			widgetPtr->dirty[2] = col;
	}
}

void Widget_InvalidateArea(Widget *widgetPtr, int top, int left, int bottom, int right)
{
	int row, col;

if (widgetPtr->flags & WIDGET_WIPE) return;

	if (widgetPtr->style != WIDGET_STYLE_MAP)
	{
		for (row = top; row <= bottom; row++)
			for (col = left; col <= right; col++)
				Widget_Invalidate(widgetPtr, row, col);
	}
	else
	{
		Widget_Invalidate(widgetPtr, top, left);
		Widget_Invalidate(widgetPtr, bottom, right);
	}
}

void Widget_Wipe(Widget *widgetPtr)
{
	/* Remember to redraw all grids later */
	widgetPtr->flags |= WIDGET_WIPE;

	/* Don't bother drawing invalid grids */
	widgetPtr->flags &= ~WIDGET_DRAW_INVALID;
/*
	if (widgetPtr->wipeProc)
		(*widgetPtr->wipeProc)(widgetPtr);
*/
	/* Redraw later */
	Widget_EventuallyRedraw(widgetPtr);
}

void Widget_EventuallyRedraw(Widget *widgetPtr)
{
	if (widgetPtr->tkwin == NULL)
		return;

	/* A redraw is already scheduled */
	if (widgetPtr->flags & WIDGET_REDRAW)
		return;

	/* Schedule a redraw */
	Tcl_DoWhenIdle(Widget_Display, (ClientData) widgetPtr);

	/* Remember a redraw is scheduled */
	widgetPtr->flags |= WIDGET_REDRAW;
}

int Widget_CaveToView(Widget *widgetPtr, int y, int x, int *rowPtr, int *colPtr)
{
	if ((y < widgetPtr->y_min) || (y >= widgetPtr->y_max))
		return FALSE;
	if ((x < widgetPtr->x_min) || (x >= widgetPtr->x_max))
		return FALSE;
	*rowPtr = y - widgetPtr->y_min;
	*colPtr = x - widgetPtr->x_min;
	return TRUE;
}

/*
 * Scan arguments to create a new Widget item. Widget items are similar
 * to Canvas items.
 *
 * $widget create $itemType $arg $arg ...
 */
int WidgetItem_Create(Tcl_Interp *interp, Widget *widgetPtr,
	int objc, Tcl_Obj *CONST objv[])
{
	WidgetItemType *typePtr;
	WidgetItemType *matchPtr = NULL;
	WidgetItem *itemPtr;
	char *t;
	int length;

	/* Required number of arguments */
	if (objc < 3)
	{
		/* Set the error */
		Tcl_WrongNumArgs(interp, 2, objv, "type ?arg arg ...?");

		/* Failure */
		goto error;
	}

	/* Get the item type to create */
	t = Tcl_GetStringFromObj(objv[2], NULL);
	length = strlen(t);
	
	/* Check each known item type */
	for (typePtr = typeList; typePtr != NULL; typePtr = typePtr->nextPtr)
	{
		/* Compare the name */
		if (t[0] != typePtr->name[0]) continue;
		if (strncmp(t, typePtr->name, length) != 0) continue;

		/* We already found an item with this name */
		if (matchPtr != NULL) goto badType;

		/* Remember the matching item type */
		matchPtr = typePtr;
	}

	/* We didn't find an item of that type */
	if (matchPtr == NULL) goto badType;

	typePtr = matchPtr;

	/*
	 * Create the option table for this item type if needed.
	 */
	if (typePtr->optionTable == NULL)
	{
		typePtr->optionTable = Tk_CreateOptionTable(interp,
			typePtr->optionSpecs);
	}
	
	/* Allocate item memory */
	itemPtr = (WidgetItem *) Tcl_AllocDebug(typePtr->itemSize);

	/* Set fields */
	itemPtr->typePtr = typePtr;
	DoubleLink_Init(&widgetPtr->linkerItem, &itemPtr->link, itemPtr);
	DoubleLink_Init(&widgetPtr->linkerItemVis, &itemPtr->linkVis, itemPtr);

	/* Call the item creation callback */
	if ((*typePtr->createProc)(interp, widgetPtr, itemPtr, objc - 3,
		objv + 3) != TCL_OK)
	{
		Tk_FreeConfigOptions((char *) itemPtr, itemPtr->typePtr->optionTable,
			widgetPtr->tkwin);

		/* Free item memory */
		Tcl_FreeDebug((char *) itemPtr);

		/* Failure */
		goto error;
	}

	/*
	 * Add the item to the Widget's list of items
	 *
	 * Y'know, a linked list of items is not very efficient in this
	 * case. Especially when repeatedly configuring an item.
	 */
	DoubleLink_Link(&itemPtr->link);

	if (itemPtr->visible)
		DoubleLink_Link(&itemPtr->linkVis);

	/* Return the index of the new item */
	Tcl_SetIntObj(Tcl_GetObjResult(interp), widgetPtr->linkerItem.count - 1);

	/* Success */
	return TCL_OK;

badType:

	/* Set the error */
    Tcl_AppendResult(interp, "unknown or ambiguous item type \"",
		t, "\"", NULL);
error:

	/* Failure */
	return TCL_ERROR;
}

/*
 * Set the interpreter result with the value of an item's configuration
 * option.
 *
 * $widget itemcget $index $option
 */
int WidgetItem_Cget(Tcl_Interp *interp, Widget *widgetPtr,
	int objc, Tcl_Obj *CONST objv[])
{
	WidgetItem *itemPtr;
	int index;
	Tcl_Obj *objPtr;
	DoubleLink *link;

	/* Required number of arguments */
	if (objc != 4)
	{
		/* Set the error */
		Tcl_WrongNumArgs(interp, 2, objv, "index option");

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the item index */
	if (Tcl_GetIntFromObj(interp, objv[2], &index) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Verify the item index */
	if ((index < 0) || (index >= widgetPtr->linkerItem.count))
	{
		/* Set the error */
		Tcl_AppendResult(interp, format("bad item index \"%d\"", index),
			NULL);

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the first item */
	link = widgetPtr->linkerItem.head;

	/* Walk through the items to the desired item */
	while (index--) link = link->next;
	itemPtr = DoubleLink_Data(link, WidgetItem);

	/* Get the value of the configuration option */
	objPtr = Tk_GetOptionValue(interp, (char *) itemPtr,
		itemPtr->typePtr->optionTable, objv[3], widgetPtr->tkwin);
	if (objPtr == NULL)
	{
		return TCL_ERROR;
	}
	
	Tcl_SetObjResult(interp, objPtr);

	/* Success */
	return TCL_OK;
}

/*
 * Fiddle with configuration options for a Widget item
 *
 * $widget itemconfigure $index ?option? ?value? ?option value ...?
 */
int WidgetItem_Configure(Tcl_Interp *interp, Widget *widgetPtr,
	int objc, Tcl_Obj *CONST objv[])
{
	WidgetItem *itemPtr;
	int index, result = TCL_OK;
	Tcl_Obj *objPtr;
	DoubleLink *link;

	/* Required number of arguments */
	if (objc < 3)
	{
		/* Set the error */
		Tcl_WrongNumArgs(interp, 2, objv, "index ?option? ?value? ?option value ...?");
		
		/* Failure */
		return TCL_ERROR;
	}

	/* Get the item index */
	if (Tcl_GetIntFromObj(interp, objv[2], &index) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Verify the item index */
	if ((index < 0) || (index >= widgetPtr->linkerItem.count))
	{
		/* Set the error */
		Tcl_AppendResult(interp, format("bad item index \"%d\"", index),
			NULL);

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the first item */
	link = widgetPtr->linkerItem.head;

	/* Walk through the items to the desired item */
	while (index--) link = link->next;
	itemPtr = DoubleLink_Data(link, WidgetItem);

	if (objc <= 4)
	{
		objPtr = Tk_GetOptionInfo(interp, (char *) itemPtr,
			itemPtr->typePtr->optionTable,
			(objc == 4) ? objv[3] : NULL,
			widgetPtr->tkwin);
		if (objPtr == NULL)
		{
			result = TCL_ERROR;
		}
		else
		{
			Tcl_SetObjResult(interp, objPtr);
		}
	}
	else
	{
		result = (*itemPtr->typePtr->configProc)(interp, widgetPtr, itemPtr,
			objc - 3, objv + 3);

		if (itemPtr->visible != itemPtr->linkVis.isLinked)
		{
			if (itemPtr->linkVis.isLinked)
				DoubleLink_Unlink(&itemPtr->linkVis);
			else
				DoubleLink_Link(&itemPtr->linkVis);
		}
	}

	/* Result */
	return result;
}

/*
 * Delete a Widget item. This is currently only called when a Widget
 * is deleted (ie, you can't delete an individual item. It would be
 * best to replace the linked list of items with another direct-lookup
 * method, and implement unique item ids like a Canvas.
 */
void WidgetItem_Delete(Widget *widgetPtr, WidgetItem *itemPtr)
{
	/* Call the item delete callback */
	(*itemPtr->typePtr->deleteProc)(widgetPtr, itemPtr);

	Tk_FreeConfigOptions((char *) itemPtr, itemPtr->typePtr->optionTable,
		widgetPtr->tkwin);

	/* Remove the item from the linked list of items */
	DoubleLink_Unlink(&itemPtr->link);

	/* Free the item memory */
	Tcl_FreeDebug((char *) itemPtr);
}

/*
 * Dumps the current contents of the offscreen bitmap for the given
 * Widget into a photo image.
 */
int Widget_Photo(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Widget *widgetPtr = (Widget *) clientData;
	Tk_PhotoHandle photoH;
	Tk_PhotoImageBlock photoBlock;
	char *imageName;
	int x, y;
	unsigned char *srcPtr, *dstPtr, *rgbPtr;
	unsigned char *rgb = Colormap_GetRGB();

	/* Requred number of arguments */
	if (objc < 3)
	{
		/* Set the error */
		Tcl_WrongNumArgs(interp, 2, objv, "imageName ?x1 y1 x2 y2?");

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the name of the Tk photo image. It must already exist */
	imageName = Tcl_GetStringFromObj(objv[2], NULL);

	/* Lookup the photo by name */
	photoH = Tk_FindPhoto(interp, imageName);

	/* The photo was not found */
	if (photoH == NULL)
	{
		return TCL_ERROR;
	}

	/*
	 * Tk_PhotoPutBlock() grows the image as needed as image data
	 * is added. Since I already know how large the image must be,
	 * I set the image dimensions here.
	 */
	Tk_PhotoSetSize(photoH, widgetPtr->width, widgetPtr->height);

	/*
	 * Set the fields for the Tk_PhotoPutBlock() call. We allocate
	 * a single row of 3 bytes per RGB pixel.
	 */
	photoBlock.pixelPtr = (unsigned char *) Tcl_Alloc(widgetPtr->width * 3);
	photoBlock.width = widgetPtr->width;
	photoBlock.height = 1;
	photoBlock.pitch = photoBlock.width * 3;
	photoBlock.pixelSize = 3;
	photoBlock.offset[0] = 0;
	photoBlock.offset[1] = 1;
	photoBlock.offset[2] = 2;

	/* Check each row */
	for (y = 0; y < widgetPtr->height; y++)
	{
		/* Read from the row */
		srcPtr = widgetPtr->bitmap.pixelPtr +
			widgetPtr->bx * widgetPtr->bitmap.pixelSize +
			(widgetPtr->by + y) * widgetPtr->bitmap.pitch;

		/* Write to the photoBlock */
		dstPtr = photoBlock.pixelPtr;

		/* Check each column */
		for (x = 0; x < widgetPtr->width; x++)
		{
			switch (widgetPtr->bitmap.depth)
			{
				case 8:
				{
					/* Get the rgb color for this pixel */
					rgbPtr = &rgb[*srcPtr * 3];
					srcPtr++;
					*dstPtr++ = *rgbPtr++; /* r */
					*dstPtr++ = *rgbPtr++; /* g */
					*dstPtr++ = *rgbPtr++; /* b */
					break;
				}
				case 16:
				{
					int r, g, b;
					GetPix16(srcPtr, &r, &g, &b);
					srcPtr += 2;
					*dstPtr++ = r;
					*dstPtr++ = g;
					*dstPtr++ = b;
					break;
				}
				case 24:
				{
					*dstPtr++ = srcPtr[2]; /* r */
					*dstPtr++ = srcPtr[1]; /* g */
					*dstPtr++ = srcPtr[0]; /* b */
					srcPtr += widgetPtr->bitmap.pixelSize;
					break;
				}
			}
		}

		/* Add the data to the photo image */
		Tk_PhotoPutBlock(photoH, &photoBlock, 0, y, photoBlock.width,
			photoBlock.height);
	}

	/* Clean up */
	Tcl_Free((char *) photoBlock.pixelPtr);

	/* Sucess */
	return TCL_OK;
}

int Widget_AddOptions(Tcl_Interp *interp, Tk_OptionSpec *option)
{
	int i, j;

	for (i = 0; optionSpecs[i].type != TK_OPTION_END; i++) ;
	
	for (j = 0; option[j].type != TK_OPTION_END; j++)
		optionSpecs[i++] = option[j];
	optionSpecs[i++] = option[j];

	return TCL_OK;
}

/*
 * Initialize the Widget package
 */
int Widget_Init(Tcl_Interp *interp, Widget_CreateProc *proc)
{
	/*  */
	g_create_proc = proc;

	/* Linked lists of Widgets */
	DoubleLink_Init(&WidgetList, NULL, NULL);
WidgetList.what = "widget";
	DoubleLink_Init(&WidgetListMap, NULL, NULL);
WidgetListMap.what = "widgetMap";

	/* Initialize Widget item colors */
	if (WidgetColor_Init(interp) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Initialize the Widget item stuff */
	if (WidgetItem_Init(interp) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Create the "widget" interpreter command */
	Tcl_CreateObjCommand(interp, "widget", Widget_ObjCmd, NULL, NULL);

	/* Success */
    return TCL_OK;
}

