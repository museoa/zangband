/* File: widget.c */

/* Purpose: Widget stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include "icon.h"

typedef struct Widget Widget;

struct Widget
{
    Tk_Window tkwin;
    Display *display;
    Tcl_Interp *interp;
    Tcl_Command widgetCmd;
	GC copyGC;					/* XCopyArea() context */
	BitmapType bitmap;			/* Offscreen bitmap */
    int width;					/* # of columns */
    int height;					/* # of rows */

#define WIDGET_REDRAW 0x0001
#define WIDGET_DRAW_INVALID 0x0002
#define WIDGET_DELETED 0x0008
#define WIDGET_EXPOSE 0x0010
#define WIDGET_NO_UPDATE 0x0020
#define WIDGET_WIPE 0x0040
	int flags;                  /* Misc flags */

    int oldWidth, oldHeight;	/* To notice changes */
    int oldTileCnt;				/* To notice changes */
    Tk_Cursor cursor;           /* Cursor? */
	int setGrid;                /* Use gridded geometry */

	int y, x;					/* Cave location (center of widget) */
	int noUpdate;				/* Drawing is disabled */

	int tc;						/* rc * cc */
	int rc, cc;					/* Rows, columns */
	int bh, bw;					/* Bitmap width & height */
	int by, bx;					/* Offset of window from bitmap */
	int dy, dx, dw, dh;			/* Dirty rect for copying */

#define WIDGET_INFO_IGNORE 0x0001	/* This tile isn't visible */
#define WIDGET_INFO_DIRTY 0x0002	/* This tile needs redraw */
	short *info;					/* Flags for each tile */

	int *invalid, invalidCnt;	/* List of invalid grids */

	int y_min, y_max;           /* Limits of displayed info */
	int x_min, x_max;           /* Limits of displayed info */
    int gwidth;					/* Source column width */
    int gheight;				/* Source row height */
    int oldGWidth, oldGHeight;	/* To notice changes */
};


static void DrawIconSpec(int y, int x, IconSpec iconSpec, BitmapPtr bitmapPtr)
{
	int pitch = bitmapPtr->pitch;
	int bypp = bitmapPtr->pixelSize;
	IconPtr srcPtr, dstPtr;
	int y2;
	int length;
	t_icon_data *iconDataPtr;

	/* Ignore NONE icon */
	if (iconSpec.type == ICON_TYPE_NONE)
		return;

	/* Special handling of BLANK */
	if (iconSpec.type == ICON_TYPE_BLANK)
	{
		/* Access the "blank" icon data */
		iconDataPtr = &g_icon_data[ICON_TYPE_BLANK];
		length = iconDataPtr->width * bypp;
		srcPtr = iconDataPtr->icon_data;
		
		/* Get the address of where to write the data in the bitmap */
		dstPtr = bitmapPtr->pixelPtr +
			x * bypp +
			y * pitch;
	
		/* Write the icon data */
		for (y2 = 0; y2 < g_icon_size; y2++)
		{
			memcpy(dstPtr, srcPtr, length);
			srcPtr += length;
			dstPtr += pitch;
		}

		/* Done */
		return;
	}

	/* Sanity check icon type */
	if ((iconSpec.type) < 0 || (iconSpec.type >= g_icon_data_count))
	{	
		/* Use "default" icon */
		iconSpec.type = ICON_TYPE_DEFAULT;
		iconSpec.index = 0;
		iconSpec.ascii = -1;
	}

	/* Access the icon type */
	iconDataPtr = &g_icon_data[iconSpec.type];

	/* Sanity check icon index */
	if ((iconSpec.index < 0) || (iconSpec.index >= iconDataPtr->icon_count))
	{
		/* Use "default" icon */
		iconSpec.type = ICON_TYPE_DEFAULT;
		iconSpec.index = 0;
		iconSpec.ascii = -1;

		/* Access the DEFAULT icon type */
		iconDataPtr = &g_icon_data[iconSpec.type];
	}

	if (iconSpec.ascii != -1)
	{
		if (iconSpec.ascii >= g_ascii_count)
		{
			/* Use "default" icon */
			iconSpec.type = ICON_TYPE_DEFAULT;
			iconSpec.index = 0;
			iconSpec.ascii = -1;

			/* Access the DEFAULT icon type */
			iconDataPtr = &g_icon_data[iconSpec.type];
		}
		else
		{
			IconData iconData;
/*			iconDataPtr = &g_icon_data[ICON_TYPE_BLANK];*/
			length = iconDataPtr->width * bypp;
			srcPtr = Icon_GetAsciiData(&iconSpec, iconData);
			dstPtr = bitmapPtr->pixelPtr +
				x * bypp +
				y * pitch;
			for (y2 = 0; y2 < iconDataPtr->height; y2++)
			{
				memcpy(dstPtr, srcPtr, length);
				srcPtr += length;
				dstPtr += pitch;
			}
			return;
		}
	}

	/* FIXME: tint */
	srcPtr = iconDataPtr->icon_data + iconSpec.index * iconDataPtr->length;

	/* Get the address of where to write the data in the bitmap */
	dstPtr = bitmapPtr->pixelPtr +
		x * bypp +
		y * pitch;

	length = iconDataPtr->width * bypp;

	/* Write the icon data */
	for (y2 = 0; y2 < iconDataPtr->height; y2++)
	{
		memcpy(dstPtr, srcPtr, length);
		srcPtr += length;
		dstPtr += pitch;
	}
}

/*
 * Get what to draw
 */
static void widget_wtd(int x, int y, t_display *wtd)
{
	/* If this is a valid cave location, get the display info. */
	if (in_bounds2 && in_bounds2(x, y))
		get_display_info(y, x, wtd);

	/* This isn't a valid cave location, so draw a "blank" icon */
	else
		wtd->blank = TRUE;
}

/*
 * Redraw everything.
 */
static void widget_draw_all(Widget *widgetPtr)
{
	int tile, layer;
	int rc = widgetPtr->rc;
	int cc = widgetPtr->cc;
	int y, x, yp, xp;
	t_display wtd;
	IconSpec iconSpec;
	BitmapPtr bitmapPtr = &widgetPtr->bitmap;

	/* Paranoia: make sure the bitmap exists */
	if (widgetPtr->bitmap.pixelPtr == NULL) return;

	/* Drawing is disabled */
	if (widgetPtr->flags & WIDGET_NO_UPDATE) return;

	for (tile = 0; tile < cc * rc; tile++)
	{
		/* This tile does not need to be redrawn */
		widgetPtr->info[tile] &= ~(WIDGET_INFO_DIRTY);

		y = widgetPtr->y_min + tile / cc;
		x = widgetPtr->x_min + tile % cc;

		widget_wtd(x, y, &wtd);

		yp = tile / cc * widgetPtr->gheight;
		xp = tile % cc * widgetPtr->gwidth;

		/* Just "erase" this spot */
		if (wtd.blank)
		{
			iconSpec.type = ICON_TYPE_BLANK;
			DrawIconSpec(yp, xp, iconSpec, bitmapPtr);
			continue;
		}

		/*
		 * Draw 1-4 background layers.
		 */
		for (layer = 0; layer < ICON_LAYER_MAX; layer++)
		{
			iconSpec = wtd.bg[layer];
	
			/* Stop at NONE icon */
			if (iconSpec.type == ICON_TYPE_NONE)
				break;
	
			/* Draw background icon */
			DrawIconSpec(yp, xp, iconSpec, bitmapPtr);
	
			/* Stop at BLANK icon */
			if (iconSpec.type == ICON_TYPE_BLANK)
				break;
		}
	
		/* Draw foreground icon */
		if (wtd.fg.type != ICON_TYPE_NONE)
			DrawIconSpec(yp, xp, wtd.fg, bitmapPtr);
	
	}

	/* There are no invalid tiles */
	widgetPtr->invalidCnt = 0;

	/* Set dirty bounds to entire window */
	widgetPtr->dx = widgetPtr->bx;
	widgetPtr->dy = widgetPtr->by;
	widgetPtr->dw = widgetPtr->width;
	widgetPtr->dh = widgetPtr->height;
}

/*
 * Redraws only those grids that were specifically marked as invalid
 * Any affected widget items are also redrawn.
 */
static void widget_draw_invalid(Widget *widgetPtr)
{
	int i, layer;
	int cc = widgetPtr->cc;
	int y, x, yp, xp;
	t_display wtd;
	IconSpec iconSpec;
	BitmapPtr bitmapPtr = &widgetPtr->bitmap;
	short *pinfo = widgetPtr->info;
	int dl, dt, dr, db;

	/* Paranoia: make sure the bitmap exists */
	if (bitmapPtr->pixelPtr == NULL) return;

	/* Drawing is disabled */
	if (widgetPtr->flags & WIDGET_NO_UPDATE) return;

	/* Keep track of dirty area */
	dl = bitmapPtr->width;
	dt = bitmapPtr->height;
	dr = 0;
	db = 0;

	for (i = 0; i < widgetPtr->invalidCnt; i++)
	{
		int tile = widgetPtr->invalid[i];

		/* This tile does not need to be redrawn */
		pinfo[tile] &= ~WIDGET_INFO_DIRTY;

		/* Cave coords */
		y = widgetPtr->y_min + tile / cc;
		x = widgetPtr->x_min + tile % cc;

		widget_wtd(x, y, &wtd);

		/* Bitmap coords */
		yp = (tile / cc) * widgetPtr->gheight;
		xp = (tile % cc) * widgetPtr->gwidth;

		/* Dirty bounds */
		if (xp < dl)
			dl = xp;
		if (yp < dt)
			dt = yp;
		if (xp + widgetPtr->gwidth - 1 > dr)
			dr = xp + widgetPtr->gwidth - 1;
		if (yp + widgetPtr->gheight - 1 > db)
			db = yp + widgetPtr->gheight - 1;

		/* Just "erase" this spot */
		if (wtd.blank)
		{
			iconSpec.type = ICON_TYPE_BLANK;
			DrawIconSpec(yp, xp, iconSpec, bitmapPtr);
			continue;
		}

		/*
		 * Draw 1-4 background layers.
		 */
		for (layer = 0; layer < ICON_LAYER_MAX; layer++)
		{
			iconSpec = wtd.bg[layer];
	
			/* Stop at NONE icon */
			if (iconSpec.type == ICON_TYPE_NONE)
				break;
	
			/* Draw background icon */
			DrawIconSpec(yp, xp, iconSpec, bitmapPtr);
	
			/* Stop at BLANK icon */
			if (iconSpec.type == ICON_TYPE_BLANK)
				break;
		}
	
		/* Draw foreground icon */
		if (wtd.fg.type != ICON_TYPE_NONE)
			DrawIconSpec(yp, xp, wtd.fg, bitmapPtr);
	}

	widgetPtr->invalidCnt = 0;

	widgetPtr->dx = dl;
	widgetPtr->dy = dt;
	widgetPtr->dw = dr - dl + 1;
	widgetPtr->dh = db - dt + 1;
}


/*
 * Create a bitmap as big as the given Widget. We get the address of
 * the bits so we can write directly into the bitmap. The bitmap is
 * 8-bit depth with a 256-color palette.
 */
static void Widget_CreateBitmap(Widget *widgetPtr)
{
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


static void Widget_Calc(Widget *widgetPtr)
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

	if (widgetPtr->info)
		FREE(widgetPtr->info);
	if (widgetPtr->invalid)
		FREE(widgetPtr->invalid);

	C_MAKE(widgetPtr->info, widgetPtr->rc * widgetPtr->cc, short);
	C_MAKE(widgetPtr->invalid, widgetPtr->rc * widgetPtr->cc, int);
	widgetPtr->invalidCnt = 0;

	for (i = 0; i < widgetPtr->rc * widgetPtr->cc; i++)
	{
		widgetPtr->info[i] = 0;
	}
}


/*
 * Actually draw stuff into the Widget's display. This routine is
 * usually passed to Tcl_DoWhenIdle().
 */
static void Widget_Display(ClientData clientData)
{
	Widget *widgetPtr = (Widget *) clientData;
	Tk_Window tkwin = widgetPtr->tkwin;

	/* We want to draw all grids */
	if ((widgetPtr->flags & WIDGET_WIPE) != 0)
	{
		/* Forget that a wipe (and redraw) is needed */
		widgetPtr->flags &= ~(WIDGET_WIPE | WIDGET_DRAW_INVALID);

		/* Draw all grids */
		widget_draw_all(widgetPtr);
	}

	/* We want to draw outdated grids */
	if ((widgetPtr->flags & WIDGET_DRAW_INVALID) != 0)
	{
		/* Forget that a redraw is needed */
		widgetPtr->flags &= ~WIDGET_DRAW_INVALID;

		/* Draw outdated grids (offscreen) */
		widget_draw_invalid(widgetPtr);
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


static void Widget_EventuallyRedraw(Widget *widgetPtr)
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


/*
 * This procedure is called when the world has changed in some
 * way and the widget needs to recompute all its graphics contexts
 * and determine its new geometry.
 */
static void Widget_WorldChanged(ClientData instanceData)
{
    Widget *widgetPtr = (Widget *) instanceData;
	Tk_Window tkwin = widgetPtr->tkwin;
    XGCValues gcValues;

	/* Allocate GC */
    if (widgetPtr->copyGC == None)
    {
		gcValues.function = GXcopy;
		gcValues.graphics_exposures = False;
		widgetPtr->copyGC = Tk_GetGC(tkwin, GCFunction | GCGraphicsExposures,
			&gcValues);
    }

	/* Size changed */
	if ((widgetPtr->width != widgetPtr->oldWidth) ||
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
		/* Allocate bitmap */
		Widget_CreateBitmap(widgetPtr);
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

	/* Remember the current info */
	widgetPtr->oldTileCnt = widgetPtr->tc;
	widgetPtr->oldWidth = widgetPtr->width;
	widgetPtr->oldHeight = widgetPtr->height;
	widgetPtr->oldGWidth = widgetPtr->gwidth;
	widgetPtr->oldGHeight = widgetPtr->gheight;

	/* Redraw the window (later) */
	Widget_EventuallyRedraw(widgetPtr);
}

/*
 * This procedure is invoked when a Widget command is deleted. If
 * the Widget isn't already in the process of being destroyed,
 * this command destroys it.
 */
static void Widget_CmdDeletedProc(ClientData clientData)
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

static void WindowToBitmap(Widget *widgetPtr, int *y, int *x)
{
	*y += widgetPtr->by;
	*x += widgetPtr->bx;
}

static Tk_OptionTable optionTable = None;

/*
 * Fiddle with configuration options for a Widget
 */
static int Widget_Configure(Tcl_Interp *interp, Widget *widgetPtr, int objc, Tcl_Obj *CONST objv[])
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


static int Widget_CaveToView(Widget *widgetPtr, int y, int x, int *rowPtr, int *colPtr)
{
	if ((y < widgetPtr->y_min) || (y >= widgetPtr->y_max))
		return FALSE;
	if ((x < widgetPtr->x_min) || (x >= widgetPtr->x_max))
		return FALSE;
	*rowPtr = y - widgetPtr->y_min;
	*colPtr = x - widgetPtr->x_min;
	return TRUE;
}

static void Widget_Wipe(Widget *widgetPtr)
{
	/* Remember to redraw all grids later */
	widgetPtr->flags |= WIDGET_WIPE;

	/* Don't bother drawing invalid grids */
	widgetPtr->flags &= ~WIDGET_DRAW_INVALID;

	/* Redraw later */
	Widget_EventuallyRedraw(widgetPtr);
}

static void Widget_Invalidate(Widget *widgetPtr, int row, int col)
{
	int rc = widgetPtr->rc;
	int cc = widgetPtr->cc;
	int tile;

	if (widgetPtr->flags & WIDGET_WIPE) return;

	if (row < 0 || row >= rc)
		return;
	if (col < 0 || col >= cc)
		return;

	tile = row * cc + col;
	if (widgetPtr->info[tile] & (WIDGET_INFO_DIRTY | WIDGET_INFO_IGNORE))
		return;
	widgetPtr->info[tile] |= WIDGET_INFO_DIRTY;
	widgetPtr->invalid[widgetPtr->invalidCnt++] = tile;
}


static void Widget_Center(Widget *widgetPtr, int cy, int cx)
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


/*
 * This is the window-specific command created for each new Widget.
 */
static int Widget_WidgetObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	static cptr commandNames[] = {"caveyx", "center", "configure",
		"wipe", "bounds", "visible", "wipespot", "hittest", NULL};
	enum {IDX_CAVEYX, IDX_CENTER, IDX_CONFIGURE,
		IDX_WIPE, IDX_BOUNDS, IDX_VISIBLE, IDX_WIPESPOT, IDX_HITTEST} option;
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


/*
 * This procedure is invoked by Tcl_EventuallyFree() or Tcl_Release()
 * to clean up the internal structure of a Widget at a safe time
 * (when no-one is using it anymore).
 */
static void Widget_Destroy(Widget *widgetPtr)
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
	
	/* Free the options table */
	Tk_FreeConfigOptions((char *) widgetPtr, optionTable,
		widgetPtr->tkwin);

	if (widgetPtr->info)
		FREE(widgetPtr->info);
	if (widgetPtr->invalid)
		FREE(widgetPtr->invalid);

	widgetPtr->tkwin = NULL;

	/* Free the Widget struct */
    Tcl_EventuallyFree((ClientData) widgetPtr, Tcl_Free);
}


/*
 * This procedure is invoked by the Tk dispatcher for various
 * events on a Widget.
 */
static void Widget_EventProc(ClientData clientData, XEvent *eventPtr)
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

	/* Done */
	return;

redraw:

	/* Redraw later */
	widgetPtr->flags |= WIDGET_EXPOSE;
	Widget_EventuallyRedraw(widgetPtr);
}


/* Table of procedures for the "Widget" class */
Tk_ClassProcs widgetProcs = {
	sizeof(Tk_ClassProcs),
	Widget_WorldChanged,			/* geometryProc. */
	NULL,							/* createProc. */
    NULL							/* modalProc. */ 
};


/*
 * Table specifying legal configuration options for a Widget.
 */
static Tk_OptionSpec optionSpecs[20] = {
    {TK_OPTION_INT, (char *) "-height", (char *) "height", (char *) "Height",
    (char *) "100", -1, Tk_Offset(Widget, height), 0, 0, 0},
    {TK_OPTION_INT, (char *) "-width", (char *) "width", (char *) "Width",
    (char *) "100", -1, Tk_Offset(Widget, width), 0, 0, 0},
    {TK_OPTION_INT, (char *) "-gheight", (char *) "gheight", (char *) "Height",
    (char *) "32", -1, Tk_Offset(Widget, gheight), 0, 0, 0},
    {TK_OPTION_INT, (char *) "-gwidth", (char *) "gwidth", (char *) "Width",
    (char *) "32", -1, Tk_Offset(Widget, gwidth), 0, 0, 0},
    {TK_OPTION_CURSOR, (char *) "-cursor", (char *) "cursor", (char *) "Cursor",
    (char *) "", -1, Tk_Offset(Widget, cursor), TK_OPTION_NULL_OK, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-setgrid", (char *) "setGrid", (char *) "SetGrid",
	(char *) "no", -1, Tk_Offset(Widget, setGrid), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-noupdate", (char *) "noUpdate", (char *) "NoUpdate",
	(char *) "no", -1, Tk_Offset(Widget, noUpdate), 0, 0, 0},
   {TK_OPTION_END, NULL, NULL, NULL,
     NULL, 0, -1, 0, 0, 0}
};


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
static int Widget_ObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
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
	
	/* Create the pointer */
	MAKE(widgetPtr, Widget);

	/* Set the class callbacks for the new Widget */
    Tk_SetClassProcs(tkwin, &widgetProcs, (ClientData) widgetPtr);

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
    widgetPtr->cursor = None;
	widgetPtr->setGrid = FALSE;
    widgetPtr->flags = 0;
	widgetPtr->y = widgetPtr->x = 0;
	widgetPtr->y_min = widgetPtr->y_max = 0;
	widgetPtr->x_min = widgetPtr->x_max = 0;
	widgetPtr->noUpdate = FALSE;
	widgetPtr->dx = widgetPtr->dy = 0;
	widgetPtr->dw = widgetPtr->dh = 0;
	widgetPtr->info = NULL;
	widgetPtr->invalid = NULL;

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
	
	/* Return the window pathname */
    Tcl_SetStringObj(Tcl_GetObjResult(interp), Tk_PathName(widgetPtr->tkwin),
    	-1);

	/* Success */
    return TCL_OK;
}


/*
 * This is a dummy lite_spot() routine that may get called before
 * the icons have been initialized.
 */
static void angtk_lite_spot_dummy(int y, int x)
{
	/* Ignore parameters and do nothing */
	(void) x;
	(void) y;
}

void (*angtk_lite_spot)(int y, int x) = angtk_lite_spot_dummy;

/*
 * This is called whenever the game thinks a grid needs to be redrawn,
 * via lite_spot(). If the grid actually changed then a redraw is
 * scheduled.
 */
void angtk_lite_spot_real(int y, int x)
{
	/* Get knowledge about location */
	get_grid_info(y, x, &g_grid[y][x]);

}

static void Widget_InvalidateArea(Widget *widgetPtr, int top, int left, int bottom, int right)
{
	int row, col;

	if (widgetPtr->flags & WIDGET_WIPE) return;

	for (row = top; row <= bottom; row++)
	{
		for (col = left; col <= right; col++)
		{
			Widget_Invalidate(widgetPtr, row, col);
		}
	}
}

/*
 * Initialize the Widget package
 */
int init_widget(Tcl_Interp *interp)
{
	/* Create the "widget" interpreter command */
	Tcl_CreateObjCommand(interp, "widget", Widget_ObjCmd, NULL, NULL);

	/* Success */
    return TCL_OK;
}




