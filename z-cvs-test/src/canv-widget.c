/* canv-widget.c */

/* Purpose: canvas item for displaying one icon */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifdef PLATFORM_WIN
#include <windows.h>
#include <tkWinInt.h>
#endif /* PLATFORM_WIN */

#ifdef PLATFORM_X11
#define HAVE_LIMITS_H
#define HAVE_UNISTD_H
#include <sys/param.h> /* Warning about NBBY being redefined */
#include <tkInt.h>
#endif /* PLATFORM_X11 */

#include <tkCanvas.h>
#include "angband.h"
#include "tnb.h"
#include "interp.h"
#include "util-dll.h"
#include "icon.h"

static BitmapType CanvWidgetBitmap;
static int g_256color;

typedef struct WidgetItem WidgetItem;
struct WidgetItem  {
    Tk_Item header;
    Tcl_Interp *interp;
    Tk_Canvas canvas;
	double x, y;
	Tk_Anchor anchor;
	t_assign assign;
	t_assign assignbg;
	int gwidth, gheight;
	XColor *borderColor;
	int borderWidth;
	int borderDist;
	DoubleLink link; /* Linked list of animated WidgetItems */
};

/*
 * Linked list of all Widget-type Canvas items in all Canvases which
 * display animated icons.
 */
static DoubleLinker WidgetItemList;

static int Assign_ParseProc _ANSI_ARGS_((
			    ClientData clientData, Tcl_Interp *interp,
			    Tk_Window tkwin, char *value, char *widgRec,
			    int offset));
static char *Assign_PrintProc _ANSI_ARGS_((
			    ClientData clientData, Tk_Window tkwin,
			    char *widgRec, int offset,
			    Tcl_FreeProc **freeProcPtr));

static Tk_CustomOption stateOption = {
    (Tk_OptionParseProc *) TkStateParseProc,
    TkStatePrintProc,
    (ClientData) 2
};

static Tk_CustomOption tagsOption = {
	Tk_CanvasTagsParseProc,
    Tk_CanvasTagsPrintProc,
	NULL
};

static Tk_CustomOption assignOption = {
	Assign_ParseProc,
    Assign_PrintProc,
	NULL
};

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_ANCHOR, "-anchor", NULL, NULL,
	 "nw", Tk_Offset(WidgetItem, anchor),
	 TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_COLOR, "-bordercolor", NULL, NULL,
	 "Yellow", Tk_Offset(WidgetItem, borderColor),
	 TK_CONFIG_DONT_SET_DEFAULT | TK_CONFIG_NULL_OK},
    {TK_CONFIG_INT, "-borderdistance", NULL, NULL,
	 "1", Tk_Offset(WidgetItem, borderDist), 0},
    {TK_CONFIG_INT, "-borderwidth", NULL, NULL,
	 "2", Tk_Offset(WidgetItem, borderWidth), 0},
    {TK_CONFIG_CUSTOM, "-assign", NULL, NULL,
	 NULL, Tk_Offset(WidgetItem, assign), TK_CONFIG_USER_BIT,
	 &assignOption},
    {TK_CONFIG_CUSTOM, "-assignbg", NULL, NULL,
	 NULL, Tk_Offset(WidgetItem, assignbg), TK_CONFIG_USER_BIT,
	 &assignOption},
    {TK_CONFIG_CUSTOM, "-state", NULL, NULL,
	 NULL, Tk_Offset(Tk_Item, state), TK_CONFIG_NULL_OK,
	 &stateOption},
    {TK_CONFIG_CUSTOM, "-tags", NULL, NULL,
	 NULL, 0, TK_CONFIG_NULL_OK, &tagsOption},
    {TK_CONFIG_END, NULL, NULL, NULL,
	 NULL, 0, 0}
};

static int		WidgetToArea _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, double *rectPtr));
static double		WidgetToPoint _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, double *coordPtr));
static void		ComputeWidgetBbox _ANSI_ARGS_((Tk_Canvas canvas,
			    WidgetItem *widgetPtr));
static int WidgetCoords _ANSI_ARGS_((Tcl_Interp *interp,
	Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
	Tcl_Obj *CONST argv[]));
static int CreateWidget _ANSI_ARGS_((Tcl_Interp *interp,
	Tk_Canvas canvas, struct Tk_Item *itemPtr,
	int argc, Tcl_Obj *CONST argv[]));
static int ConfigureWidget _ANSI_ARGS_((Tcl_Interp *interp,
	Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
	Tcl_Obj *CONST argv[], int flags));
static void		DeleteWidget _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, Display *display));
static void		DisplayWidget _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, Display *display, Drawable dst,
			    int x, int y, int width, int height));
static void		ScaleWidget _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, double originX, double originY,
			    double scaleX, double scaleY));
static void		TranslateWidget _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, double deltaX, double deltaY));

Tk_ItemType WidgetType = {
    "widget",						/* name */
    sizeof(WidgetItem),				/* itemSize */
    CreateWidget,					/* createProc */
    configSpecs,					/* configSpecs */
    ConfigureWidget,				/* configureProc */
    WidgetCoords,					/* coordProc */
    DeleteWidget,					/* deleteProc */
    DisplayWidget,					/* displayProc */
    TK_CONFIG_OBJS,					/* flags */
    WidgetToPoint,					/* pointProc */
    WidgetToArea,					/* areaProc */
    NULL,							/* postscriptProc */
    ScaleWidget,					/* scaleProc */
    TranslateWidget,				/* translateProc */
    NULL,	   						/* indexProc */
    NULL,							/* icursorProc */
    NULL,							/* selectionProc */
    NULL,	  						/* insertProc */
    NULL,	 						/* dTextProc */
    NULL							/* nextPtr */
};

static int
CreateWidget(
    Tcl_Interp *interp,			/* Interpreter for error reporting. */
    Tk_Canvas canvas,			/* Canvas to hold new item. */
    Tk_Item *itemPtr,			/* Record to hold new item;  header
								 * has been initialized by caller. */
    int argc,					/* Number of arguments in argv. */
	Tcl_Obj *CONST argv[]		/* Arguments describing item. */
)
{
    WidgetItem *widgetPtr = (WidgetItem *) itemPtr;
    t_assign none;

    if (argc < 2)
    {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			Tk_PathName(Tk_CanvasTkwin(canvas)), " create ",
			itemPtr->typePtr->name, " x y ?options?\"",
			NULL);
		return TCL_ERROR;
    }

    /*
     * Initialize item's record.
     */

	widgetPtr->interp = interp;
    widgetPtr->canvas = canvas;
    none.assignType = ASSIGN_TYPE_ICON;
    none.icon.type = ICON_TYPE_NONE;
    none.icon.index = 0;
    none.icon.ascii = -1;
	widgetPtr->assign = none;
	widgetPtr->assignbg = none;
	widgetPtr->gwidth = widgetPtr->gheight = g_icon_size;
	widgetPtr->anchor = TK_ANCHOR_NW;
	widgetPtr->borderColor = NULL;
	widgetPtr->borderWidth = 0;
	widgetPtr->borderDist = 0;
	DoubleLink_Init(&WidgetItemList, &widgetPtr->link, widgetPtr);

    /*
     * Process the arguments to fill in the item record.
     */

    if ((Tk_CanvasGetCoordFromObj(interp, canvas, argv[0], &widgetPtr->x) != TCL_OK)
	    || (Tk_CanvasGetCoordFromObj(interp, canvas, argv[1], &widgetPtr->y)
		!= TCL_OK))
	{
		return TCL_ERROR;
    }

    if (ConfigureWidget(interp, canvas, itemPtr, argc-2, argv+2, 0) != TCL_OK)
    {
		DeleteWidget(canvas, itemPtr, Tk_Display(Tk_CanvasTkwin(canvas)));
		return TCL_ERROR;
    }

    return TCL_OK;
}

static int
WidgetCoords(
    Tcl_Interp *interp,			/* Used for error reporting. */
    Tk_Canvas canvas,			/* Canvas containing item. */
    Tk_Item *itemPtr,			/* Item whose coordinates are to be
								 * read or modified. */
    int argc,					/* Number of coordinates supplied in
								 * argv. */
	Tcl_Obj *CONST argv[] 		/* Array of coordinates: x1, y1,
								 * x2, y2, ... */
)
{
    WidgetItem *widgetPtr = (WidgetItem *) itemPtr;
    char x[TCL_DOUBLE_SPACE], y[TCL_DOUBLE_SPACE];

    if (argc == 0)
    {
		Tcl_PrintDouble(interp, widgetPtr->x, x);
		Tcl_PrintDouble(interp, widgetPtr->y, y);
		Tcl_AppendResult(interp, x, " ", y, NULL);
    }
    else if (argc == 2)
    {
		if ((Tk_CanvasGetCoordFromObj(interp, canvas, argv[0], &widgetPtr->x)
			!= TCL_OK)
			|| (Tk_CanvasGetCoordFromObj(interp, canvas, argv[1],
			&widgetPtr->y) != TCL_OK))
		{
		    return TCL_ERROR;
		}
		ComputeWidgetBbox(canvas, widgetPtr);
    }
    else
    {
		sprintf(interp->result,
			"wrong # coordinates: expected 0 or 2, got %d", argc);
		return TCL_ERROR;
    }
    return TCL_OK;
}

static int
ConfigureWidget(
    Tcl_Interp *interp,		/* Used for error reporting. */
    Tk_Canvas canvas,		/* Canvas containing itemPtr. */
    Tk_Item *itemPtr,		/* Widget item to reconfigure. */
    int argc,				/* Number of elements in argv.  */
    Tcl_Obj *CONST argv[],	/* Arguments describing things to configure. */
    int flags 				/* Flags to pass to Tk_ConfigureWidget. */
)
{
    WidgetItem *widgetPtr = (WidgetItem *) itemPtr;
    Tk_Window tkwin;
    t_icon_data *iconDataPtr;
    IconSpec iconSpec;

    tkwin = Tk_CanvasTkwin(canvas);
    if (Tk_ConfigureWidget(interp, tkwin, configSpecs, argc,
	    (char **) argv, (char *) widgetPtr, flags | TK_CONFIG_OBJS) != TCL_OK)
	{
		return TCL_ERROR;
    }

	FinalIcon(&iconSpec, &widgetPtr->assign, 0, NULL);
	iconDataPtr = &g_icon_data[iconSpec.type];
	widgetPtr->gwidth = iconDataPtr->width;
	widgetPtr->gheight = iconDataPtr->height;
	
    ComputeWidgetBbox(canvas, widgetPtr);

	/* Keep a linked list of WidgetItem's displaying sprites */
	if (widgetPtr->link.isLinked)
	{
		if (!is_sprite(&widgetPtr->assign) && !is_sprite(&widgetPtr->assignbg))
		{
			DoubleLink_Unlink(&widgetPtr->link);
		}
	}
	else
	{
		if (is_sprite(&widgetPtr->assign) || is_sprite(&widgetPtr->assignbg))
		{
			DoubleLink_Link(&widgetPtr->link);
		}
	}

    return TCL_OK;
}

static void
DeleteWidget(
    Tk_Canvas canvas,			/* Info about overall canvas widget. */
    Tk_Item *itemPtr,			/* Item that is being deleted. */
    Display *display 			/* Display containing window for
					 * canvas. */
)
{
	WidgetItem *widgetPtr = (WidgetItem *) itemPtr;

	DoubleLink_Unlink(&widgetPtr->link);
}

static void
ComputeWidgetBbox(
    Tk_Canvas canvas,				/* Canvas that contains item. */
    WidgetItem *widgetPtr 			/* Item whose bbox is to be
									 * recomputed. */
)
{
	int x, y;
	int borderSize = 0;
    Tk_State state = widgetPtr->header.state;

	if(state == TK_STATE_NULL)
	{
		state = ((TkCanvas *) canvas)->canvas_state;
	}

    x = (int) (widgetPtr->x + ((widgetPtr->x >= 0) ? 0.5 : - 0.5));
    y = (int) (widgetPtr->y + ((widgetPtr->y >= 0) ? 0.5 : - 0.5));

	if (state == TK_STATE_HIDDEN)
	{
		widgetPtr->header.x1 = widgetPtr->header.x2 = x;
		widgetPtr->header.y1 = widgetPtr->header.y2 = y;
		return;
	}

	switch (widgetPtr->anchor)
	{
		case TK_ANCHOR_NW:
		case TK_ANCHOR_N:
		case TK_ANCHOR_NE:
			break;
	
		case TK_ANCHOR_W:
		case TK_ANCHOR_CENTER:
		case TK_ANCHOR_E:
			y -=  widgetPtr->gheight / 2;
			break;
	
		case TK_ANCHOR_SW:
		case TK_ANCHOR_S:
		case TK_ANCHOR_SE:
			y -=  widgetPtr->gheight;
			break;
	}
	switch (widgetPtr->anchor)
	{
		case TK_ANCHOR_NW:
		case TK_ANCHOR_W:
		case TK_ANCHOR_SW:
			break;
	
		case TK_ANCHOR_N:
		case TK_ANCHOR_CENTER:
		case TK_ANCHOR_S:
			x -=  widgetPtr->gwidth / 2;
			break;
	
		case TK_ANCHOR_NE:
		case TK_ANCHOR_E:
		case TK_ANCHOR_SE:
			x -= widgetPtr->gwidth;
			break;
	}

	if (widgetPtr->borderColor && widgetPtr->borderWidth)
	{
		borderSize = widgetPtr->borderWidth + widgetPtr->borderDist;
	}

    widgetPtr->header.x1 = x - borderSize;
    widgetPtr->header.y1 = y - borderSize;
    widgetPtr->header.x2 = x + widgetPtr->gwidth + borderSize;
    widgetPtr->header.y2 = y + widgetPtr->gheight + borderSize;
}

/*
 * Draw one icon into our bitmap.
 */
static void DrawIconSpec(IconSpec *iconSpecPtr)
{
	IconPtr srcPtr, dstPtr;
	int y;
	t_icon_data *iconDataPtr;

	iconDataPtr = &g_icon_data[iconSpecPtr->type];

	dstPtr = CanvWidgetBitmap.pixelPtr;

	/*
	 * If this is an ascii-type icon, then we must call a routine to
	 * give us the colorized icon data.
	 */
	if (iconSpecPtr->ascii != -1)
	{
		IconData iconData;
		srcPtr = Icon_GetAsciiData(iconSpecPtr, iconData);
		for (y = 0; y < iconDataPtr->height; y++)
		{
			memcpy(dstPtr, srcPtr, iconDataPtr->pitch);
			srcPtr += iconDataPtr->pitch;
			dstPtr += CanvWidgetBitmap.pitch;
		}
	}

	/* Not an ascii-type icon */
	else
	{
		srcPtr = iconDataPtr->icon_data + iconSpecPtr->index * ICON_LENGTH;
	
		/* Transparent */
		if (iconDataPtr->rle_data)
		{	
			int col = 0;
			IconPtr rlebuf;
			unsigned char *bounds = iconDataPtr->rle_bounds +
				iconSpecPtr->index * 4;
			int bypp = iconDataPtr->bypp;
			IconPtr dst = dstPtr + bounds[0] * bypp +
				bounds[1] * CanvWidgetBitmap.pitch;
			int w = bounds[2], h = bounds[3];

			if (iconDataPtr->dynamic)
			{
				IconValue empty[2] = { 0, 0 };
				rlebuf = ((IconPtr *) iconDataPtr->rle_data)[iconSpecPtr->index];
				if (!rlebuf)
					rlebuf = empty;
			}
			else
			{
				rlebuf = iconDataPtr->rle_data +
					iconDataPtr->rle_offset[iconSpecPtr->index];
			}

			while (1)
			{
				unsigned int trans, opaq;
		
				trans = rlebuf[0];
				opaq = rlebuf[1];
				rlebuf += 2;
		
				col += trans;
		
				if (opaq)
				{
					memcpy(dst + col * bypp, rlebuf, opaq * bypp);
					rlebuf += opaq * bypp;
					col += opaq;
				}
				else if (!col)
					break;

				if (col == w)
				{
					if (!--h)
						break;
					col = 0;
					dst += CanvWidgetBitmap.pitch;
				}
			}
		}
	
		/* Not transparent */
		else
		{
			for (y = 0; y < iconDataPtr->height; y++)
			{
				memcpy(dstPtr, srcPtr, iconDataPtr->pitch);
				srcPtr += iconDataPtr->pitch;
				dstPtr += CanvWidgetBitmap.pitch;
			}
		}
	}
}

static void
DisplayWidget(
    Tk_Canvas canvas,			/* Canvas that contains item. */
    Tk_Item *itemPtr,			/* Item to be displayed. */
    Display *display,			/* Display on which to draw item. */
    Drawable drawable,			/* Pixmap or window in which to draw
								 * item. */
    int x, int y,				/* Describes region of canvas that */
    int width, int height 		/* must be redisplayed (not used). */
)
{
#ifdef PLATFORM_WIN

    WidgetItem *widgetPtr = (WidgetItem *) itemPtr;
    short drawableX, drawableY;
    HDC dc, dc2;
    TkWinDCState state, state2;
	IconSpec iconSpecFG, iconSpecBG;
	int borderSize = 0;

    /*
     * Translate the coordinates to those of the item, then redisplay it.
     */

    Tk_CanvasDrawableCoords(canvas,
		(double) widgetPtr->header.x1,
		(double) widgetPtr->header.y1,
		&drawableX, &drawableY);

	FinalIcon(&iconSpecFG, &widgetPtr->assign, 0, NULL);
	FinalIcon(&iconSpecBG, &widgetPtr->assignbg, 0, NULL);

	if (widgetPtr->borderColor && widgetPtr->borderWidth)
	{
		borderSize = widgetPtr->borderWidth + widgetPtr->borderDist;
	}

	if ((iconSpecFG.type != ICON_TYPE_NONE) || (iconSpecBG.type != ICON_TYPE_NONE))
	{
		/* This code is like TkPutImage() */
		dc = TkWinGetDrawableDC(display, drawable, &state);
		dc2 = TkWinGetDrawableDC(display, CanvWidgetBitmap.pixmap, &state2);

		/* Transparent */
		if (g_icon_data[iconSpecFG.type].rle_data ||
			g_icon_data[iconSpecBG.type].rle_data)
		{
			/*
			 * Ignorance alert! I want to copy the background from where
			 * the masked icon should be drawn to the CanvWidgetBitmap
			 * and draw the masked icon on top of that. So I figure I
			 * will BitBlt() from the canvas HDC to CanvWidgetBitmap HDC.
			 * But on 256-color monitors it is way too slow. So I find
			 * what color is under the canvas item and use that.
			 */
			if (!g_256color)
			{
			    BitBlt(dc2, 0, 0, widgetPtr->gwidth,
					widgetPtr->gheight, dc, drawableX + borderSize,
					drawableY + borderSize, SRCCOPY);
			}
			else
			{
				TkWinFillRect(dc2, 0, 0,
					widgetPtr->gwidth, widgetPtr->gheight,
					GetPixel(dc, drawableX + borderSize,
					drawableY + borderSize));
			}
		}
		
		/* Draw background icon */
		if (iconSpecBG.type != ICON_TYPE_NONE)
		{
			DrawIconSpec(&iconSpecBG);
		}
	
		/* Draw foreground icon */
		if (iconSpecFG.type != ICON_TYPE_NONE)
		{
			DrawIconSpec(&iconSpecFG);
		}

		BitBlt(dc, drawableX + borderSize, drawableY + borderSize,
			widgetPtr->gwidth, widgetPtr->gheight, dc2, 0, 0, SRCCOPY);
	
		TkWinReleaseDrawableDC(CanvWidgetBitmap.pixmap, dc2, &state2);
		TkWinReleaseDrawableDC(drawable, dc, &state);
	}

	if (borderSize)
	{
		XGCValues gcValues;
		GC gc;
		int lineWidth = widgetPtr->borderWidth;

		gcValues.foreground = widgetPtr->borderColor->pixel;
		gcValues.line_width = lineWidth;
		gc = Tk_GetGC(Tk_CanvasTkwin(canvas), GCForeground | GCLineWidth,
			&gcValues);

	    XDrawRectangle(display, drawable, gc,
			drawableX + lineWidth / 2,
			drawableY + lineWidth / 2,
			(widgetPtr->header.x2 - widgetPtr->header.x1) - lineWidth / 2 - 1,
			(widgetPtr->header.y2 - widgetPtr->header.y1) - lineWidth / 2 - 1);

		Tk_FreeGC(display, gc);
	}
	
#endif /* PLATFORM_WIN */

	/*
	 * This code works on Win32 too, last time I checked.
	 * EXCEPT for the special case of 256-colors handled
	 * by the above code.
	 */

#ifdef PLATFORM_X11

    WidgetItem *widgetPtr = (WidgetItem *) itemPtr;
    short drawableX, drawableY;
	IconSpec iconSpecFG, iconSpecBG;
	int borderSize = 0;
	Tk_Window tkwin = ((TkCanvas *) canvas)->tkwin;
	XGCValues gcValues;
	GC gc;

    /*
     * Translate the coordinates to those of the image, then redisplay it.
     */

    Tk_CanvasDrawableCoords(canvas,
		(double) widgetPtr->header.x1,
		(double) widgetPtr->header.y1,
		&drawableX, &drawableY);

	FinalIcon(&iconSpecFG, &widgetPtr->assign, 0, NULL);
	FinalIcon(&iconSpecBG, &widgetPtr->assignbg, 0, NULL);

	if (widgetPtr->borderColor && widgetPtr->borderWidth)
	{
		borderSize = widgetPtr->borderWidth + widgetPtr->borderDist;
	}

	if ((iconSpecFG.type != ICON_TYPE_NONE) || (iconSpecBG.type != ICON_TYPE_NONE))
	{
		gcValues.function = GXcopy;
		gcValues.graphics_exposures = False;
		gc = Tk_GetGC(tkwin, GCFunction | GCGraphicsExposures, &gcValues);

		/* Transparent */
		if (g_icon_data[iconSpecFG.type].rle_data ||
			g_icon_data[iconSpecBG.type].rle_data)
		{
			XCopyArea(display,
				drawable, /* source drawable */
				CanvWidgetBitmap.pixmap, /* dest drawable */
				gc, /* graphics context */
				drawableX + borderSize, drawableY + borderSize, /* source top-left */
				(unsigned int) widgetPtr->gwidth, /* width */
				(unsigned int) widgetPtr->gheight, /* height */
				0, 0 /* dest top-left */
			);

			Plat_SyncDisplay(display);
		}

		/* Draw background icon */
		if (iconSpecBG.type != ICON_TYPE_NONE)
		{
			DrawIconSpec(&iconSpecBG);
		}
	
		/* Draw foreground icon */
		if (iconSpecFG.type != ICON_TYPE_NONE)
		{
			DrawIconSpec(&iconSpecFG);
		}

		XCopyArea(display,
			CanvWidgetBitmap.pixmap, /* source drawable */
			drawable, /* dest drawable */
			gc, /* graphics context */
			0, 0, /* source top-left */
			(unsigned int) widgetPtr->gwidth, /* width */
			(unsigned int) widgetPtr->gheight, /* height */
			drawableX + borderSize, /* dest top-left */
			drawableY + borderSize
		);
	
		Tk_FreeGC(display, gc);
	}

	if (borderSize)
	{
		XGCValues gcValues;
		GC gc;
		int lineWidth = widgetPtr->borderWidth;

		gcValues.foreground = widgetPtr->borderColor->pixel;
		gcValues.line_width = lineWidth;
		gc = Tk_GetGC(Tk_CanvasTkwin(canvas), GCForeground | GCLineWidth,
			&gcValues);

	    XDrawRectangle(display, drawable, gc,
			drawableX + lineWidth / 2,
			drawableY + lineWidth / 2,
			(widgetPtr->header.x2 - widgetPtr->header.x1) - lineWidth / 2 - 1,
			(widgetPtr->header.y2 - widgetPtr->header.y1) - lineWidth / 2 - 1);

		Tk_FreeGC(display, gc);
	}

	/* Since multiple items may be drawn into CanvWidgetBitmap */
	Plat_SyncDisplay(display);

#endif /* PLATFORM_X11 */
}

static double
WidgetToPoint(
    Tk_Canvas canvas,		/* Canvas containing item. */
    Tk_Item *itemPtr,		/* Item to check against point. */
    double *coordPtr 		/* Pointer to x and y coordinates. */
)
{
	WidgetItem *widgetPtr = (WidgetItem *) itemPtr;
	double x1, x2, y1, y2, xDiff, yDiff;

	x1 = widgetPtr->header.x1;
	y1 = widgetPtr->header.y1;
	x2 = widgetPtr->header.x2;
	y2 = widgetPtr->header.y2;

	/*
	 * Point is outside rectangle.
	 */

	if (coordPtr[0] < x1)
	{
		xDiff = x1 - coordPtr[0];
	}
	else if (coordPtr[0] > x2)
	{
		xDiff = coordPtr[0] - x2;
	}
	else
	{
		xDiff = 0;
	}

	if (coordPtr[1] < y1)
	{
		yDiff = y1 - coordPtr[1];
	}
	else if (coordPtr[1] > y2) 
	{
		yDiff = coordPtr[1] - y2;
	}
	else
	{
		yDiff = 0;
	}

	return hypot(xDiff, yDiff);
}

static int
WidgetToArea(
    Tk_Canvas canvas,		/* Canvas containing item. */
    Tk_Item *itemPtr,		/* Item to check against rectangle. */
    double *rectPtr 		/* Pointer to array of four coordinates
				 * (x1, y1, x2, y2) describing rectangular
				 * area.  */
)
{
    WidgetItem *widgetPtr = (WidgetItem *) itemPtr;

    if ((rectPtr[2] <= widgetPtr->header.x1)
	    || (rectPtr[0] >= widgetPtr->header.x2)
	    || (rectPtr[3] <= widgetPtr->header.y1)
	    || (rectPtr[1] >= widgetPtr->header.y2))
	{
		return -1;
    }
    if ((rectPtr[0] <= widgetPtr->header.x1)
	    && (rectPtr[1] <= widgetPtr->header.y1)
	    && (rectPtr[2] >= widgetPtr->header.x2)
	    && (rectPtr[3] >= widgetPtr->header.y2))
	{
		return 1;
    }
    return 0;
}

static void
ScaleWidget(
    Tk_Canvas canvas,			/* Canvas containing rectangle. */
    Tk_Item *itemPtr,			/* Rectangle to be scaled. */
    double originX,
    double originY,			/* Origin about which to scale rect. */
    double scaleX,			/* Amount to scale in X direction. */
    double scaleY 			/* Amount to scale in Y direction. */
)
{
    WidgetItem *widgetPtr = (WidgetItem *) itemPtr;

    widgetPtr->x = originX + scaleX*(widgetPtr->x - originX);
    widgetPtr->y = originY + scaleY*(widgetPtr->y - originY);
    ComputeWidgetBbox(canvas, widgetPtr);
}

static void
TranslateWidget(
    Tk_Canvas canvas,			/* Canvas containing item. */
    Tk_Item *itemPtr,			/* Item that is being moved. */
    double deltaX,				/* Amount by which item is to be */
    double deltaY				/* moved. */
)
{
    WidgetItem *widgetPtr = (WidgetItem *) itemPtr;

    widgetPtr->x += deltaX;
    widgetPtr->y += deltaY;
    ComputeWidgetBbox(canvas, widgetPtr);
}

static int
Assign_ParseProc(ClientData clientData, Tcl_Interp *interp,
	Tk_Window tkwin, char *value, char *widgRec, int offset)
{
/*	WidgetItem *itemPtr = (WidgetItem *) widgRec; */

	if (assign_parse(interp, (t_assign *) (widgRec + offset), value) != TCL_OK)
	{
		return TCL_ERROR;
	}

    return TCL_OK;
}

static char *
Assign_PrintProc(ClientData clientData, Tk_Window tkwin, char *widgRec,
	int offset, Tcl_FreeProc **freeProcPtr)
{
/*	WidgetItem *itemPtr = (WidgetItem *) widgRec; */
	char *buf = Tcl_Alloc(128);

	*freeProcPtr = (Tcl_FreeProc *) TCL_DYNAMIC;
	return assign_print(buf, (t_assign *) (widgRec + offset));
}

void CanvasWidget_Idle(void)
{
	DoubleLink *link;
	WidgetItem *widgetPtr;

	for (link = WidgetItemList.head; link; link = link->next)
	{
		widgetPtr = DoubleLink_Data(link, WidgetItem);
	    Tk_CanvasEventuallyRedraw(widgetPtr->canvas, widgetPtr->header.x1,
		    widgetPtr->header.y1, widgetPtr->header.x2, widgetPtr->header.y2);
	}
}

int CanvasWidget_Init(Tcl_Interp *interp)
{
	DoubleLink_Init(&WidgetItemList, NULL, NULL);

	CanvWidgetBitmap.height = 100;
	CanvWidgetBitmap.width = 100;
	CanvWidgetBitmap.depth = g_icon_depth;
	Bitmap_New(interp, &CanvWidgetBitmap);

#ifdef PLATFORM_WIN
	{
		HDC monDC = GetDC(NULL);
		g_256color = (GetDeviceCaps(monDC, BITSPIXEL) == 8);
		ReleaseDC(NULL, monDC);
	}
#endif /* PLATFORM_WIN */

	Tk_CreateItemType(&WidgetType);

	return TCL_OK;
}

