/* File: widget3.c */

/* Purpose: widget extensions */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include "icon.h"
#include "widget.h"

#define XSetRect(r,x1,y1,w,h) (*r).x=x1,(*r).y=y1,(*r).width=w,(*r).height=h

#define BAD_COLOR(c) (((c) < 0) || ((c) > 255))


/*
 * Calculate rows/columns covered in parent Widget.
 */
void CalcLimits(Widget *widgetPtr, WidgetItem *itemPtr)
{
	int height, width, dy, dx, my;

	/* WindowToBitmap */
	dy = widgetPtr->by;
	dx = widgetPtr->bx;

	height = widgetPtr->gheight;
	width = widgetPtr->gwidth;
	my = 1;
	
	itemPtr->minY = (dy + itemPtr->y1) / height * my;
	itemPtr->maxY = (dy + itemPtr->y2) / height * my;
	itemPtr->minX = (dx + itemPtr->x1) / width;
	itemPtr->maxX = (dx + itemPtr->x2) / width;
	if (((dy + itemPtr->y2) % height) == 0)
		itemPtr->maxY -= my;
	if (((dx + itemPtr->x2) % width) == 0)
		--itemPtr->maxX;
}

#define DEF_BEVEL_LIGHT ((char *) "Black")
#define DEF_BEVEL_DARK ((char *) "White")
#define DEF_BEVEL_OPACITY ((char *) "127")

/*
 * The structure below defines the record for each progress bar item.
 */
typedef struct ProgressItem {
	WidgetItem header; /* Required header info */
    Tk_Anchor anchor;	/* Where to anchor rect relative to (x,y). */
	int x, y; /* Pixel offsets in Widget bitmap */
	int width, height; /* Size of bar in pixels */
	XColor *done, *todo; /* Color info */
	int bevel; /* Boolean: Draw a bevel or not */
	XColor *bevelLight, *bevelDark; /* Color info for bevel */
	int cur, max; /* Progress current, maximum values */
	long length; /* Length in pixels of "done" part */
/* WIDGET_TRANSPARENCY */
	int done1, todo1, done2, todo2; /* */
	int bevelLight1, bevelDark1, bevelLight2, bevelDark2; /* */
} ProgressItem;

/*
 * Constants for Tk_OptionSpec.typeMask.
 */
#define PROGRESS_SIZE 0x0001
#define PROGRESS_MOVE 0x0002
#define PROGRESS_DONE 0x0004
#define PROGRESS_TODO 0x0008
#define PROGRESS_BEVEL_L 0x0010
#define PROGRESS_BEVEL_D 0x0020
#define PROGRESS_VAL 0x0040
#define PROGRESS_DRAW 0x4000
#define PROGRESS_DISPLAY 0x8000

static Tk_OptionSpec optionSpecProgress[] = {
    {TK_OPTION_INT, (char *) "-x", NULL, NULL,
     (char *) "0", -1, Tk_Offset(ProgressItem, x), 0, 0,
     PROGRESS_MOVE | PROGRESS_DISPLAY},
    {TK_OPTION_INT, (char *) "-y", NULL, NULL,
     (char *) "0", -1, Tk_Offset(ProgressItem, y), 0, 0,
     PROGRESS_MOVE | PROGRESS_DISPLAY},
    {TK_OPTION_INT, (char *) "-width", NULL, NULL,
     (char *) "160", -1, Tk_Offset(ProgressItem, width), 0, 0,
     PROGRESS_SIZE | PROGRESS_DRAW | PROGRESS_DISPLAY},
    {TK_OPTION_INT, (char *) "-height", NULL, NULL,
     (char *) "7", -1, Tk_Offset(ProgressItem, height), 0, 0,
     PROGRESS_SIZE | PROGRESS_DRAW | PROGRESS_DISPLAY},
    {TK_OPTION_COLOR, (char *) "-done", NULL, NULL,
     (char *) "Black", -1, Tk_Offset(ProgressItem, done), 0, 0,
     PROGRESS_DONE | PROGRESS_DISPLAY},
    {TK_OPTION_COLOR, (char *) "-todo", NULL, NULL,
     (char *) "White", -1, Tk_Offset(ProgressItem, todo), 0, 0,
     PROGRESS_TODO | PROGRESS_DISPLAY},
    {TK_OPTION_BOOLEAN, (char *) "-bevel", NULL, NULL,
     (char *) "1", -1, Tk_Offset(ProgressItem, bevel), 0, 0, 0},
    {TK_OPTION_COLOR, (char *) "-bevellight", NULL, NULL,
     DEF_BEVEL_LIGHT, -1, Tk_Offset(ProgressItem, bevelLight), 0, 0,
     PROGRESS_BEVEL_L | PROGRESS_DISPLAY},
    {TK_OPTION_COLOR, (char *) "-beveldark", NULL, NULL,
     DEF_BEVEL_DARK, -1, Tk_Offset(ProgressItem, bevelDark), 0, 0,
     PROGRESS_BEVEL_D | PROGRESS_DISPLAY},
/* WIDGET_TRANSPARENCY */
    {TK_OPTION_INT, (char *) "-done2", NULL, NULL,
     (char *) "0", -1, Tk_Offset(ProgressItem, done2), 0, 0,
     PROGRESS_DONE | PROGRESS_DISPLAY},
    {TK_OPTION_INT, (char *) "-todo2", NULL, NULL,
     (char *) "0", -1, Tk_Offset(ProgressItem, todo2), 0, 0,
     PROGRESS_TODO | PROGRESS_DISPLAY},
    {TK_OPTION_INT, (char *) "-bevellight2", NULL, NULL,
     DEF_BEVEL_OPACITY, -1, Tk_Offset(ProgressItem, bevelLight2), 0, 0,
     PROGRESS_BEVEL_L | PROGRESS_DISPLAY},
    {TK_OPTION_INT, (char *) "-beveldark2", NULL, NULL,
     DEF_BEVEL_OPACITY, -1, Tk_Offset(ProgressItem, bevelDark2), 0, 0,
     PROGRESS_BEVEL_D | PROGRESS_DISPLAY},
/* */
    {TK_OPTION_INT, (char *) "-current", NULL, NULL,
     (char *) "0", -1, Tk_Offset(ProgressItem, cur), 0, 0,
     PROGRESS_VAL | PROGRESS_DRAW | PROGRESS_DISPLAY},
    {TK_OPTION_INT, (char *) "-maximum", NULL, NULL,
     (char *) "1", -1, Tk_Offset(ProgressItem, max), 0, 0, 
     PROGRESS_VAL | PROGRESS_DRAW | PROGRESS_DISPLAY},
    {TK_OPTION_BOOLEAN, (char *) "-visible", NULL, NULL,
     (char *) "1", -1, Tk_Offset(WidgetItem, visible), 0, 0,
     PROGRESS_DISPLAY},
    {TK_OPTION_ANCHOR, (char *) "-anchor", NULL, NULL,
	 (char *) "center", -1, Tk_Offset(ProgressItem, anchor), 0, 0,
	 PROGRESS_MOVE | PROGRESS_DISPLAY},
    {TK_OPTION_END, NULL, NULL, NULL,
     NULL, 0, -1, 0, 0, 0}
};

static int	CreateProgress _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr,
		    int objc, Tcl_Obj *CONST objv[]));
static int	ConfigureProgress _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr,
		    int objc, Tcl_Obj *CONST objv[]));
static int	DisplayProgress _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr));
static int	ChangedProgress _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr));
static void	DeleteProgress _ANSI_ARGS_((Widget *widgetPtr,
			WidgetItem *itemPtr));
static void ComputeProgressBbox(ProgressItem *barPtr);

WidgetItemType ProgressType = {
	"progressbar",
	sizeof(ProgressItem),
	optionSpecProgress,
	NULL, /* optionTable */
	CreateProgress,
	ConfigureProgress,
	DisplayProgress,
	ChangedProgress,
	DeleteProgress,
	NULL
};

/* 
 * Item creation callback
 */
static int CreateProgress(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr, int objc, Tcl_Obj *CONST objv[])
{
	ProgressItem *barPtr = (ProgressItem *) itemPtr;

	barPtr->header.visible = FALSE;
	barPtr->anchor = TK_ANCHOR_CENTER;
	barPtr->x = 0;
	barPtr->y = 0;
	barPtr->width = 0;
	barPtr->height = 0;
	barPtr->done = NULL;
	barPtr->todo = NULL;
	barPtr->bevel = TRUE;
	barPtr->bevelLight = NULL;
	barPtr->bevelDark = NULL;
/* WIDGET_TRANSPARENCY */
	barPtr->done1 = 0;
	barPtr->todo1 = 0;
	barPtr->done2 = 0;
	barPtr->todo2 = 0;
	barPtr->bevelLight2 = 0;
	barPtr->bevelDark2 = 0;
	barPtr->cur = 0;
	barPtr->max = 0;
	barPtr->length = 0;

	/* Set the default options for the new widget */
	if (Tk_InitOptions(interp, (char *) itemPtr,
		itemPtr->typePtr->optionTable, widgetPtr->tkwin) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (ConfigureProgress(interp, widgetPtr, itemPtr, objc, objv) != TCL_OK)
	{
		return TCL_ERROR;
	}

	return TCL_OK;
}

/*
 * Item configuration callback
 */
static int ConfigureProgress(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr, int objc, Tcl_Obj *CONST objv[])
{
    Tk_SavedOptions savedOptions;
	Tcl_Obj *errorResult = NULL;
	int error, mask = 0;

	ProgressItem *barPtr = (ProgressItem *) itemPtr;
	int redraw = 0, redisplay = 0, resize = 0, move = 0;
	int wasVisible = itemPtr->visible;

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
			if (Tk_SetOptions(interp, (char *) itemPtr,
				itemPtr->typePtr->optionTable, objc, objv,
				widgetPtr->tkwin, &savedOptions, &mask) != TCL_OK)
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

		redisplay = (mask & PROGRESS_DISPLAY) != 0;
		redraw = (mask & PROGRESS_DRAW) != 0;
		
		/* The item is being created, don't bother optimizing */
		if (barPtr->done == NULL)
		{
			ComputeProgressBbox(barPtr);
			
			if ((barPtr->header.x1 < 0) || (barPtr->width < 0) ||
				(barPtr->header.x2 >= widgetPtr->width))
			{
				Tcl_AppendResult(interp, "illegal x size or position", NULL);
				continue;
			}
			if ((barPtr->header.y1 < 0) || (barPtr->height < 0) ||
				(barPtr->header.y2 >= widgetPtr->height))
			{
				Tcl_AppendResult(interp, "illegal y size or position", NULL);
				continue;
			}
		
			/* Calculate which grids are covered by us */
			CalcLimits(widgetPtr, itemPtr);
		
			if ((barPtr->cur < 0) || (barPtr->cur > barPtr->max) ||
				(barPtr->max < 1))
			{
				Tcl_AppendResult(interp, "bad min/max value", NULL);
				continue;
			}
			barPtr->length = (barPtr->width - 2) * ((long) barPtr->cur) / barPtr->max;
		}

		/* Post-creation configure, so optimize */
		else
		{
			/* Resize if height or width changed */
			resize = (mask & PROGRESS_SIZE);
		
			/* Move if x, y, or anchor changed */
			move = (mask & PROGRESS_MOVE);
		
			/* Recalculate the bound box if it moved or resized */
			if (resize || move)
			{
				ComputeProgressBbox(barPtr);
		
				if ((barPtr->header.x1 < 0) || (barPtr->width < 0) ||
					(barPtr->header.x2 >= widgetPtr->width))
				{
					Tcl_AppendResult(interp, "illegal x size or position",
						 NULL);
					continue;
				}
				if ((barPtr->header.y1 < 0) || (barPtr->height < 0) ||
					(barPtr->header.y2 >= widgetPtr->height))
				{
					Tcl_AppendResult(interp, "illegal y size or position",
						NULL);
					continue;
				}
			
				/* Calculate which grids are covered by us */
				CalcLimits(widgetPtr, itemPtr);
			}
		
			/* Current/Maximum */
			if (resize || (mask & PROGRESS_VAL))
			{
				if ((barPtr->cur < 0) || (barPtr->cur > barPtr->max) ||
					(barPtr->max < 1))
				{
					Tcl_AppendResult(interp, "bad min/max value", NULL);
					continue;
				}
				barPtr->length = (barPtr->width - 2) * ((long) barPtr->cur) / barPtr->max;
			}
		}
		break;
	}

	if (error)
	{
		Tcl_SetObjResult(interp, errorResult);
		Tcl_DecrRefCount(errorResult);

		/* Failure */
		return TCL_ERROR;
	}

	Tk_FreeSavedOptions(&savedOptions);

	if (redisplay && (itemPtr->visible || wasVisible))
	{
		Widget_InvalidateArea(widgetPtr, itemPtr->minY, itemPtr->minX,
			itemPtr->maxY, itemPtr->maxX);
		widgetPtr->flags |= WIDGET_DRAW_INVALID;
		Widget_EventuallyRedraw(widgetPtr);
	}

	/* Success */
	return TCL_OK;
}

/*
 * Item delete callback
 */
static void	DeleteProgress(Widget *widgetPtr, WidgetItem *itemPtr)
{
	/* Do nothing? */
	(void) widgetPtr;
	(void) itemPtr;
}

/*
 * Calculates the coordinates of the Widget item, using the given
 * position and anchor.
 */
static void ComputeProgressBbox(ProgressItem *barPtr)
{
	int leftX, topY, width, height;

	width = barPtr->width;
	height = barPtr->height;

	leftX = barPtr->x;
	topY = barPtr->y;

	switch (barPtr->anchor)
	{
		case TK_ANCHOR_NW:
		case TK_ANCHOR_N:
		case TK_ANCHOR_NE:
			break;
	
		case TK_ANCHOR_W:
		case TK_ANCHOR_CENTER:
		case TK_ANCHOR_E:
			topY -= height / 2;
			break;
	
		case TK_ANCHOR_SW:
		case TK_ANCHOR_S:
		case TK_ANCHOR_SE:
			topY -= height;
			break;
	}
	switch (barPtr->anchor)
	{
		case TK_ANCHOR_NW:
		case TK_ANCHOR_W:
		case TK_ANCHOR_SW:
			break;
	
		case TK_ANCHOR_N:
		case TK_ANCHOR_CENTER:
		case TK_ANCHOR_S:
			leftX -= width / 2;
			break;
	
		case TK_ANCHOR_NE:
		case TK_ANCHOR_E:
		case TK_ANCHOR_SE:
			leftX -= width;
			break;
	}

	barPtr->header.x1 = leftX;
	barPtr->header.y1 = topY;
	barPtr->header.x2 = leftX + width;
	barPtr->header.y2 = topY + height;
}


/*
 * Item display callback
 */
static int DisplayProgress_Solid(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	ProgressItem *barPtr = (ProgressItem *) itemPtr;
	int width = barPtr->width;
	int height = barPtr->height;
	GC gc;
	Display *display = widgetPtr->display;
	Pixmap pixmap = widgetPtr->bitmap.pixmap;

	/* Hack - ignore unused parameter */
	(void) interp;

	gc = Tk_GCForColor(barPtr->done, pixmap);
	XFillRectangle(display, pixmap, gc,
		widgetPtr->bx + itemPtr->x1 + 1, widgetPtr->by + itemPtr->y1 + 1,
		barPtr->length, height - 2);

	gc = Tk_GCForColor(barPtr->todo, pixmap);
	XFillRectangle(display, pixmap, gc,
		widgetPtr->bx + itemPtr->x1 + 1 + barPtr->length,
		widgetPtr->by + itemPtr->y1 + 1,
		width - barPtr->length - 2, height - 2);

	gc = Tk_GCForColor(barPtr->bevelLight, pixmap);
	XDrawLine(display, pixmap, gc, widgetPtr->bx + itemPtr->x1,
		widgetPtr->by + itemPtr->y2 - 1,
		widgetPtr->bx + itemPtr->x1, widgetPtr->by + itemPtr->y1);
	XDrawLine(display, pixmap, gc, widgetPtr->bx + itemPtr->x1,
		widgetPtr->by + itemPtr->y1,
		widgetPtr->bx + itemPtr->x2 - 1, widgetPtr->by + itemPtr->y1);

	gc = Tk_GCForColor(barPtr->bevelDark, pixmap);
	XDrawLine(display, pixmap, gc, widgetPtr->bx + itemPtr->x1,
		widgetPtr->by + itemPtr->y2 - 1,
		widgetPtr->bx + itemPtr->x2 - 1, widgetPtr->by + itemPtr->y2 - 1);
	XDrawLine(display, pixmap, gc, widgetPtr->bx + itemPtr->x2 - 1,
		widgetPtr->by + itemPtr->y2 - 1,
		widgetPtr->bx + itemPtr->x2 - 1, widgetPtr->by + itemPtr->y1);

	return TCL_OK;
}

/*
 * Item display callback
 */
static int DisplayProgress(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	/* Not transparent */
	return DisplayProgress_Solid(interp, widgetPtr, itemPtr);
}

/*
 * Item changed callback
 */
static int ChangedProgress(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	int delta;

	/* Hack - ignore unused parameter */
	(void) interp;

	if (itemPtr->x2 >= widgetPtr->width)
	{
		delta = itemPtr->x2 - widgetPtr->width;
		itemPtr->x1 -= delta, itemPtr->x2 -= delta;
	}
	if (itemPtr->y2 >= widgetPtr->height)
	{
		delta = itemPtr->y2 - widgetPtr->height;
		itemPtr->y1 -= delta, itemPtr->y2 -= delta;
	}

	/* Calculate which grids are covered by us */
	CalcLimits(widgetPtr, itemPtr);

	return TCL_OK;
}

#define DEF_TEXT_FILL ((char *) "Black")
#define DEF_TEXT_BACK ((char *) "White")
#define DEF_TEXT_OPACITY ((char *) "127")

/*
 * The structure below defines the record for each text item.
 */
typedef struct TextItem {
	WidgetItem header; /* Required header info */
	int x, y; /* Pixel offsets in Widget bitmap */
    Tk_Anchor anchor;	/* Where to anchor rect relative to (x,y). */
    Tk_Font tkfont;		/* Font for drawing text. */
    Tk_Justify justify;		/* Justification mode for text. */
    char *text; /* Text for item (malloc'd). */
	int numChars; /* Number of non-NULL characters in text. */
	int width, height; /* Size of bounding rect in pixels */
	XColor *fill, *background; /* Color info */
	int bevel; /* Boolean: Draw a bevel or not */
	XColor *bevelLight, *bevelDark; /* Color info for bevel */
	int clipX, clipY; /* Boolean: Clip displayed area to size of text + padding */
	int expandX, expandY; /* Boolean: Expand to text size if needed */
	int padLeft, padRight, padTop, padBottom; /* Text padding */
	int fill1, background1, fill2, background2; /* */
	int bevelLight1, bevelDark1, bevelLight2, bevelDark2; /* */
	int textHeight, textWidth, textLeft; /* Height & width of text, left of text */
	int textAscent;
	GC textGC;
	int dirty[4], hasDirty;
} TextItem;

#define TEXT_SIZE 0x0001
#define TEXT_MOVE 0x0002
#define TEXT_DRAW 0x0004
#define TEXT_DISPLAY 0x0008
#define TEXT_TEXT 0x0010
#define TEXT_FONT 0x0020
#define TEXT_FILL 0x0040
#define TEXT_BACKGROUND 0x0080
#define TEXT_BEVEL_L 0x0100
#define TEXT_BEVEL_D 0x0200

#define TEXT_EXPAND 0x0400
	
static Tk_OptionSpec optionSpecText[] = {
    {TK_OPTION_PIXELS, (char *) "-x", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TextItem, x), 0, 0, 0},
    {TK_OPTION_PIXELS, (char *) "-y", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TextItem, y), 0, 0, 0},
    {TK_OPTION_ANCHOR, (char *) "-anchor", NULL, NULL,
	 (char *) "center", -1, Tk_Offset(TextItem, anchor), 0, 0, 0},
    {TK_OPTION_FONT, (char *) "-font", NULL, NULL,
	 (char *) "{MS Sans Serif} 8", -1, Tk_Offset(TextItem, tkfont), 0, 0,
	 TEXT_FONT | TEXT_TEXT | TEXT_DRAW | TEXT_DISPLAY},
    {TK_OPTION_JUSTIFY, (char *) "-justify", NULL, NULL,
	 (char *) "left", -1, Tk_Offset(TextItem, justify), 0, 0,
	 TEXT_TEXT | TEXT_DRAW | TEXT_DISPLAY},
    {TK_OPTION_STRING, (char *) "-text", NULL, NULL,
	 (char *) "", -1, Tk_Offset(TextItem, text), 0, 0,
	 TEXT_TEXT | TEXT_DRAW | TEXT_DISPLAY},
    {TK_OPTION_PIXELS, (char *) "-width", NULL, NULL,
     (char *) "160", -1, Tk_Offset(TextItem, width), 0, 0,
     TEXT_SIZE | TEXT_DRAW | TEXT_DISPLAY},
    {TK_OPTION_PIXELS, (char *) "-height", NULL, NULL,
     (char *) "7", -1, Tk_Offset(TextItem, height), 0, 0,
     TEXT_SIZE | TEXT_DRAW | TEXT_DISPLAY},
    {TK_OPTION_COLOR, (char *) "-fill", NULL, NULL,
     DEF_TEXT_FILL, -1, Tk_Offset(TextItem, fill), 0, 0,
     TEXT_FILL | TEXT_DISPLAY},
    {TK_OPTION_COLOR, (char *) "-background", NULL, NULL,
     DEF_TEXT_BACK, -1, Tk_Offset(TextItem, background),
     TK_OPTION_NULL_OK, 0,
     TEXT_BACKGROUND | TEXT_DISPLAY},
    {TK_OPTION_BOOLEAN, (char *) "-bevel", NULL, NULL,
     (char *) "1", -1, Tk_Offset(TextItem, bevel), 0, 0, 0},
    {TK_OPTION_COLOR, (char *) "-bevellight", NULL, NULL,
     DEF_BEVEL_LIGHT, -1, Tk_Offset(TextItem, bevelLight), 0, 0,
     TEXT_BEVEL_L | TEXT_DISPLAY},
    {TK_OPTION_COLOR, (char *) "-beveldark", NULL, NULL,
     DEF_BEVEL_DARK, -1, Tk_Offset(TextItem, bevelDark), 0, 0,
     TEXT_BEVEL_D | TEXT_DISPLAY},
/* WIDGET_TRANSPARENCY */
    {TK_OPTION_INT, (char *) "-fill2", NULL, NULL,
     DEF_TEXT_OPACITY, -1, Tk_Offset(TextItem, fill2), 0, 0,
     TEXT_FILL | TEXT_DISPLAY},
    {TK_OPTION_INT, (char *) "-background2", NULL, NULL,
     DEF_TEXT_OPACITY, -1, Tk_Offset(TextItem, background2), 0, 0,
     TEXT_BACKGROUND | TEXT_DISPLAY},
    {TK_OPTION_INT, (char *) "-bevellight2", NULL, NULL,
     DEF_BEVEL_OPACITY, -1, Tk_Offset(TextItem, bevelLight2), 0, 0,
     TEXT_BEVEL_L | TEXT_DISPLAY},
    {TK_OPTION_INT, (char *) "-beveldark2", NULL, NULL,
     DEF_BEVEL_OPACITY, -1, Tk_Offset(TextItem, bevelDark2), 0, 0,
     TEXT_BEVEL_D | TEXT_DISPLAY},
/* */
    {TK_OPTION_BOOLEAN, (char *) "-clipx", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TextItem, clipX), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-clipy", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TextItem, clipY), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-expandx", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TextItem, expandX), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-expandy", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TextItem, expandY), 0, 0, 0},
    {TK_OPTION_PIXELS, (char *) "-padleft", NULL, NULL,
     (char *) "2", -1, Tk_Offset(TextItem, padLeft), 0, 0, 0},
    {TK_OPTION_PIXELS, (char *) "-padright", NULL, NULL,
     (char *) "2", -1, Tk_Offset(TextItem, padRight), 0, 0, 0},
    {TK_OPTION_PIXELS, (char *) "-padtop", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TextItem, padTop), 0, 0, 0},
    {TK_OPTION_PIXELS, (char *) "-padbottom", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TextItem, padBottom), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-visible", NULL, NULL,
     (char *) "0", -1, Tk_Offset(WidgetItem, visible), 0, 0,
     TEXT_DISPLAY},
    {TK_OPTION_END, NULL, NULL, NULL,
     NULL, 0, -1, 0, 0, 0}
};

static int	CreateText _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr,
		    int objc, Tcl_Obj *CONST objv[]));
static int	ConfigureText _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr,
		    int objc, Tcl_Obj *CONST objv[]));
static int	DisplayText _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr));
static int	ChangedText _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr));
static void	DeleteText _ANSI_ARGS_((Widget *widgetPtr,
			WidgetItem *itemPtr));
static int SanityCheckText(Widget *widgetPtr, TextItem *textPtr);
static void ComputeTextLayout(TextItem *textPtr, int *maskPtr);

WidgetItemType TextType = {
	"text",
	sizeof(TextItem),
	optionSpecText,
	NULL, /* optionTable */
	CreateText,
	ConfigureText,
	DisplayText,
	ChangedText,
	DeleteText,
	NULL
};

/*
 * Item creation callback
 */
static int CreateText(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr, int objc, Tcl_Obj *CONST objv[])
{
	TextItem *textPtr = (TextItem *) itemPtr;

	textPtr->header.visible = FALSE;
	textPtr->x = 0;
	textPtr->y = 0;
    textPtr->anchor	= TK_ANCHOR_CENTER;	
    textPtr->tkfont	= NULL;
    textPtr->justify = TK_JUSTIFY_LEFT;
    textPtr->text = NULL;
	textPtr->numChars = 0;
	textPtr->width = 0;
	textPtr->height = 0;
	textPtr->fill = NULL;
	textPtr->background = NULL;
	textPtr->bevel = FALSE;
	textPtr->bevelLight = NULL;
	textPtr->bevelDark = NULL;
/* WIDGET_TRANSPARENCY */
	textPtr->fill1 = 0;
	textPtr->background1 = 0;
	textPtr->fill2 = 0;
	textPtr->background2 = 0;
	textPtr->bevelLight1 = 0;
	textPtr->bevelDark1 = 0;
	textPtr->bevelLight2 = 0;
	textPtr->bevelDark2 = 0;
	textPtr->clipX = FALSE;
	textPtr->clipY = FALSE;
	textPtr->expandX = FALSE;
	textPtr->expandY = FALSE;
	textPtr->padLeft = 2;
	textPtr->padRight = 2;
	textPtr->padTop = 0;
	textPtr->padBottom = 0;
	textPtr->textHeight = 0;
	textPtr->textWidth = 0;
	textPtr->textLeft = 0;
	textPtr->textAscent = 0;
	textPtr->textGC = None;
	textPtr->hasDirty = FALSE;

	/* Set the default options for the new widget */
	if (Tk_InitOptions(interp, (char *) itemPtr,
		itemPtr->typePtr->optionTable, widgetPtr->tkwin) != TCL_OK)
	{
		DeleteText(widgetPtr, itemPtr);
		return TCL_ERROR;
	}

	if (ConfigureText(interp, widgetPtr, itemPtr, objc, objv)
		!= TCL_OK)
	{
		DeleteText(widgetPtr, itemPtr);
		return TCL_ERROR;
	}

	return TCL_OK;
}

/*
 * Item configuration callback
 */
static int ConfigureText(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr, int objc, Tcl_Obj *CONST objv[])
{
    Tk_SavedOptions savedOptions;
	Tcl_Obj *errorResult = NULL;
	int error, mask = 0;

	TextItem *textPtr = (TextItem *) itemPtr;
	int wasVisible = itemPtr->visible;
	int redraw = 0, redisplay = 0, resize = 0, text, layout = 0;
	Tk_FontMetrics fm;
	XGCValues gcValues;
	unsigned long gcMask;

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
			if (Tk_SetOptions(interp, (char *) itemPtr,
				itemPtr->typePtr->optionTable, objc, objv,
				widgetPtr->tkwin, &savedOptions, &mask) != TCL_OK)
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

		redraw = (mask & TEXT_DRAW) != 0;
		redisplay = (mask & TEXT_DISPLAY) != 0;
		
		/* We are creating the item, so don't bother optimizing */
		if (textPtr->textGC == None)
		{
			textPtr->numChars = strlen(textPtr->text);
			textPtr->textWidth = Tk_TextWidth(textPtr->tkfont, textPtr->text,
				textPtr->numChars);

			Tk_GetFontMetrics(textPtr->tkfont, &fm);
			textPtr->textHeight = fm.linespace;
			textPtr->textAscent = fm.ascent;

			gcValues.foreground = textPtr->fill->pixel;
			gcValues.graphics_exposures = False;
			gcValues.font = Tk_FontId(textPtr->tkfont);
			gcMask = GCForeground | GCFont | GCGraphicsExposures;
			textPtr->textGC = Tk_GetGC(widgetPtr->tkwin, gcMask, &gcValues);

			ComputeTextLayout(textPtr, &layout);
	
			if (SanityCheckText(widgetPtr, textPtr) != TCL_OK)
			{
				continue;
			}
	
			/* Calculate which grids are covered by us */
			CalcLimits(widgetPtr, itemPtr);

		}
		else
		{
			resize = (mask & TEXT_SIZE) != 0;
			text = (mask & TEXT_TEXT) != 0;

			/* Text, font or justify changed */
			if (text)
			{
				/* Calculate the length of the text */
				textPtr->numChars = strlen(textPtr->text);
	
				/* Calculate the width of the text in pixels */
				textPtr->textWidth = Tk_TextWidth(textPtr->tkfont,
					textPtr->text, textPtr->numChars);
			}

			if (mask & TEXT_FONT)
			{
				Tk_GetFontMetrics(textPtr->tkfont, &fm);
				textPtr->textHeight = fm.linespace;
				textPtr->textAscent = fm.ascent;
			}

			if (mask & (TEXT_FONT | TEXT_FILL))
			{
				if (textPtr->textGC)
				{
					Tk_FreeGC(Tk_Display(widgetPtr->tkwin), textPtr->textGC);
				}
				gcValues.foreground = textPtr->fill->pixel;
				gcValues.graphics_exposures = False;
				gcValues.font = Tk_FontId(textPtr->tkfont);
				gcMask = GCForeground | GCFont | GCGraphicsExposures;
				textPtr->textGC = Tk_GetGC(widgetPtr->tkwin, gcMask, &gcValues);
			}

			/* Calculate the size and position on the display */
			ComputeTextLayout(textPtr, &layout);
	
			if (SanityCheckText(widgetPtr, textPtr) != TCL_OK)
			{
				continue;
			}
	
			/* The display size or position changed */
			if (layout)
			{
				/* Calculate which grids are covered by us */
				CalcLimits(widgetPtr, itemPtr);
	
				/* The display size changed */
				if (layout & TEXT_SIZE)
				{
					/* Handle automatic expansion of bitmap */
					if (layout & TEXT_EXPAND)
					{
						resize = TRUE;
					}
						
					/* Redraw the bitmap */
					redraw = TRUE;
				}

				/* Update display if visible */
				redisplay = TRUE;
			}
		}
		break;
	}

	if (error)
	{
		Tcl_SetObjResult(interp, errorResult);
		Tcl_DecrRefCount(errorResult);

		/* Failure */
		return TCL_ERROR;
	}

	Tk_FreeSavedOptions(&savedOptions);

	/* Update display (later) */
	if (redisplay && (itemPtr->visible || wasVisible))
	{
		/* Was drawn, and size or position changed */
		if (textPtr->hasDirty && layout)
		{
			Widget_InvalidateArea(widgetPtr,
				textPtr->dirty[0], textPtr->dirty[1],
				textPtr->dirty[2], textPtr->dirty[3]);
			textPtr->hasDirty = FALSE;
			
		}
		Widget_InvalidateArea(widgetPtr,
			itemPtr->minY, itemPtr->minX,
			itemPtr->maxY, itemPtr->maxX);
		widgetPtr->flags |= WIDGET_DRAW_INVALID;
		Widget_EventuallyRedraw(widgetPtr);
	}

	/* Success */
	return TCL_OK;
}

/*
 * Item deletion callback
 */
static void	DeleteText(Widget *widgetPtr, WidgetItem *itemPtr)
{
	TextItem *textPtr = (TextItem *) itemPtr;

	if (textPtr->textGC)
	{
		Tk_FreeGC(Tk_Display(widgetPtr->tkwin), textPtr->textGC);
	}
}

/*
 * Calculates the coordinates of the text item
 */
static void ComputeTextLayout(TextItem *textPtr, int *maskPtr)
{
	WidgetItem *itemPtr = (WidgetItem *) textPtr;
	int leftX, topY, width, height;

	(*maskPtr) = 0;
	
	/* Get the requested width */
	width = textPtr->width;

	/* Get the requested height */
	height = textPtr->height;

	/* Clip vertically */
	if (textPtr->clipY)
	{
		/* Calculate height of text plus vertical padding */
		height = (textPtr->bevel != 0) * 2 + textPtr->padTop +
			textPtr->textHeight + textPtr->padBottom;
	}

	/* Clip horizontally */
	if (textPtr->clipX)
	{
		/* Calculate width of text plus horizontal padding */
		width = (textPtr->bevel != 0) * 2 + textPtr->padLeft +
			textPtr->textWidth + textPtr->padRight;
	}

	/* Expand vertically */
	if (height > textPtr->height)
	{
		if (textPtr->expandY)
		{
			textPtr->height = height;
			(*maskPtr) |= TEXT_EXPAND;
		}
		else
			height = textPtr->height;
	}

	/* Expand horizontally */
	if (width > textPtr->width)
	{
		if (textPtr->expandX)
		{
			textPtr->width = width;
			(*maskPtr) |= TEXT_EXPAND;
		}
		else
			width = textPtr->width;
	}

	/*
	 * Use overall geometry information to compute the top-left corner
	 * of the bounding box for the text item.
	 */

	leftX = textPtr->x;
	topY = textPtr->y;
	switch (textPtr->anchor)
	{
		case TK_ANCHOR_NW:
		case TK_ANCHOR_N:
		case TK_ANCHOR_NE:
			break;
	
		case TK_ANCHOR_W:
		case TK_ANCHOR_CENTER:
		case TK_ANCHOR_E:
			topY -= height / 2;
			break;
	
		case TK_ANCHOR_SW:
		case TK_ANCHOR_S:
		case TK_ANCHOR_SE:
			topY -= height;
			break;
	}
	switch (textPtr->anchor)
	{
		case TK_ANCHOR_NW:
		case TK_ANCHOR_W:
		case TK_ANCHOR_SW:
			break;
	
		case TK_ANCHOR_N:
		case TK_ANCHOR_CENTER:
		case TK_ANCHOR_S:
			leftX -= width / 2;
			break;
	
		case TK_ANCHOR_NE:
		case TK_ANCHOR_E:
		case TK_ANCHOR_SE:
			leftX -= width;
			break;
	}

	/* The position changed */
	if ((itemPtr->x1 != leftX) || (itemPtr->y1 != topY))
	{
		(*maskPtr) |= TEXT_MOVE;
	}

	/* The size changed */
	if (width != (itemPtr->x2 - itemPtr->x1) ||
		height != (itemPtr->y2 - itemPtr->y1))
	{
		(*maskPtr) |= TEXT_SIZE;
	}

	itemPtr->x1 = leftX;
	itemPtr->y1 = topY;
	itemPtr->x2 = leftX + width;
	itemPtr->y2 = topY + height;

	switch (textPtr->justify)
	{
		case TK_JUSTIFY_LEFT:
			textPtr->textLeft = textPtr->padLeft;
			break;
		case TK_JUSTIFY_CENTER:
			textPtr->textLeft = width / 2 - textPtr->textWidth / 2;
			break;
		case TK_JUSTIFY_RIGHT:
			textPtr->textLeft = width - textPtr->textWidth -
				textPtr->padRight;
			break;
	}
}

/*
 * Return an error if there are problems with a text item's options
 */
static int SanityCheckText(Widget *widgetPtr, TextItem *textPtr)
{
	WidgetItem *itemPtr = (WidgetItem *) textPtr;

	/* Prevent illegal x size or position */
	if ((itemPtr->x1 < 0) || (textPtr->width < 0) ||
		(itemPtr->x2 > widgetPtr->width) ||
		((itemPtr->x2 - itemPtr->x1) > textPtr->width))
	{
		Tcl_AppendResult(widgetPtr->interp, "illegal x size or position",
			NULL);
		return TCL_ERROR;
	}

	/* Prevent illegal y size or position */
	if ((itemPtr->y1 < 0) || (textPtr->height < 0) ||
		(itemPtr->y2 > widgetPtr->height) ||
		((itemPtr->y2 - itemPtr->y1) > textPtr->height))
	{
		Tcl_AppendResult(widgetPtr->interp, "illegal y size or position",
			NULL);
		return TCL_ERROR;
	}

	/* Success */
	return TCL_OK;
}


/*
 * Item display callback
 */
static int DisplayText_Solid(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	TextItem *textPtr = (TextItem *) itemPtr;
	int width = textPtr->header.x2 - textPtr->header.x1;
	int height = textPtr->header.y2 - textPtr->header.y1;
	Display *display = widgetPtr->display;
	GC gc;
	Pixmap pixmap = widgetPtr->bitmap.pixmap;

	/* Hack - ignore unused parameter */
	(void) interp;

	if (textPtr->background != NULL)
	{
		gc = Tk_GCForColor(textPtr->background, pixmap);
		XFillRectangle(display, pixmap, gc,
			widgetPtr->bx + itemPtr->x1, widgetPtr->by + itemPtr->y1,
			width, height);
	}

	/* Draw the text */
	Tk_DrawChars(widgetPtr->display, pixmap,
		textPtr->textGC, textPtr->tkfont, textPtr->text, textPtr->numChars,
		widgetPtr->bx + itemPtr->x1 + textPtr->textLeft,
		widgetPtr->by + itemPtr->y1 + (textPtr->bevel != 0) +
		textPtr->padTop + textPtr->textAscent);

	if (textPtr->bevel)
	{
		gc = Tk_GCForColor(textPtr->bevelLight, pixmap);
		XDrawLine(display, pixmap, gc, widgetPtr->bx + itemPtr->x1,
			widgetPtr->by + itemPtr->y2 - 1,
			widgetPtr->bx + itemPtr->x1, widgetPtr->by + itemPtr->y1);
		XDrawLine(display, pixmap, gc, widgetPtr->bx + itemPtr->x1,
			widgetPtr->by + itemPtr->y1,
			widgetPtr->bx + itemPtr->x2 - 1, widgetPtr->by + itemPtr->y1);
	
		gc = Tk_GCForColor(textPtr->bevelDark, pixmap);
		XDrawLine(display, pixmap, gc, widgetPtr->bx + itemPtr->x1,
			widgetPtr->by + itemPtr->y2 - 1,
			widgetPtr->bx + itemPtr->x2 - 1, widgetPtr->by + itemPtr->y2 - 1);
		XDrawLine(display, pixmap, gc, widgetPtr->bx + itemPtr->x2 - 1,
			widgetPtr->by + itemPtr->y2 - 1,
			widgetPtr->bx + itemPtr->x2 - 1, widgetPtr->by + itemPtr->y1);
	}

textPtr->dirty[0] = itemPtr->minY;
textPtr->dirty[1] = itemPtr->minX;
textPtr->dirty[2] = itemPtr->maxY;
textPtr->dirty[3] = itemPtr->maxX;
textPtr->hasDirty = TRUE;

	return TCL_OK;
}

/*
 * Item display callback
 */
static int DisplayText(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	/* Not transparent */
	return DisplayText_Solid(interp, widgetPtr, itemPtr);
}


/*
 * Item changed callback
 */
static int ChangedText(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	int delta;

	/* Hack - ignore unused parameter */
	(void) interp;

	if (itemPtr->x2 >= widgetPtr->width)
	{
		delta = itemPtr->x2 - widgetPtr->width;
		itemPtr->x1 -= delta, itemPtr->x2 -= delta;
	}
	if (itemPtr->y2 >= widgetPtr->height)
	{
		delta = itemPtr->y2 - widgetPtr->height;
		itemPtr->y1 -= delta, itemPtr->y2 -= delta;
	}

	/* Calculate which grids are covered by us */
	CalcLimits(widgetPtr, itemPtr);

	return TCL_OK;
}

/*
 * The structure below defines the record for each cursor item.
 */
typedef struct CursorItem {
	WidgetItem header; /* Required header info */
	int x, y; /* Cave location to highlight */
	XColor *color; /* Color info */
	int lineWidth; /* Width of rectangle */
	int col, row; /* Displayed row/col */
	int dirty[2], hasDirty; /* Previous row/col */
} CursorItem;

static Tk_OptionSpec optionSpecCursor[] = {
    {TK_OPTION_BOOLEAN, (char *) "-visible", NULL, NULL,
     (char *) "1", -1, Tk_Offset(WidgetItem, visible), 0, 0, 0},
    {TK_OPTION_PIXELS, (char *) "-x", NULL, NULL,
     (char *) "0", -1, Tk_Offset(CursorItem, x), 0, 0, 0},
    {TK_OPTION_PIXELS, (char *) "-y", NULL, NULL,
     (char *) "0", -1, Tk_Offset(CursorItem, y), 0, 0, 0},
    {TK_OPTION_COLOR, (char *) "-color", NULL, NULL,
     (char *) "yellow", -1, Tk_Offset(CursorItem, color), 0, 0, 0},
    {TK_OPTION_INT, (char *) "-linewidth", NULL, NULL,
     (char *) "2", -1, Tk_Offset(CursorItem, lineWidth), 0, 0, 0},
    {TK_OPTION_END, NULL, NULL, NULL,
     NULL, 0, -1, 0, 0, 0}
};

static int	KreateCursor _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr,
		    int objc, Tcl_Obj *CONST objv[]));
static int	ConfigureCursor _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr,
		    int objc, Tcl_Obj *CONST objv[]));
static int	DisplayCursor _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr));
static void	DeleteCursor _ANSI_ARGS_((Widget *widgetPtr,
			WidgetItem *itemPtr));
static int CalcCursorPosition(Widget *widgetPtr, CursorItem *cursorPtr);

WidgetItemType CursorType = {
	"cursor",
	sizeof(CursorItem),
	optionSpecCursor,
	NULL, /* optionTable */
	KreateCursor,
	ConfigureCursor,
	DisplayCursor,
	NULL,
	DeleteCursor,
	NULL
};

/*
 * Item creation callback
 */
static int KreateCursor(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr, int objc, Tcl_Obj *CONST objv[])
{
	CursorItem *cursorPtr = (CursorItem *) itemPtr;

	cursorPtr->header.visible = TRUE;
	cursorPtr->x = 0;
	cursorPtr->y = 0;
	cursorPtr->color = NULL;
	cursorPtr->lineWidth = 2;
	cursorPtr->hasDirty = FALSE;

	/* Set the default options for the new widget */
	if (Tk_InitOptions(interp, (char *) itemPtr,
		itemPtr->typePtr->optionTable, widgetPtr->tkwin) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (ConfigureCursor(interp, widgetPtr, itemPtr, objc, objv)
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	return TCL_OK;
}

/*
 * Item configuration callback
 */
static int ConfigureCursor(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr, int objc, Tcl_Obj *CONST objv[])
{
    Tk_SavedOptions savedOptions;
	Tcl_Obj *errorResult = NULL;
	int error, mask = 0;
	int wasVisible = itemPtr->visible;

	CursorItem *cursorPtr = (CursorItem *) itemPtr;

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
			if (Tk_SetOptions(interp, (char *) itemPtr,
				itemPtr->typePtr->optionTable, objc, objv,
				widgetPtr->tkwin, &savedOptions, &mask) != TCL_OK)
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

		if (!CalcCursorPosition(widgetPtr, cursorPtr))
		{
			itemPtr->minY = 0, itemPtr->maxY = -1;
			itemPtr->minX = 0, itemPtr->maxX = -1;
		}
		
		break;
	}

	if (error)
	{
		Tcl_SetObjResult(interp, errorResult);
		Tcl_DecrRefCount(errorResult);

		/* Failure */
		return TCL_ERROR;
	}

	Tk_FreeSavedOptions(&savedOptions);

	if (itemPtr->visible || wasVisible)
	{
		if (cursorPtr->hasDirty)
		{
			Widget_Invalidate(widgetPtr,
				cursorPtr->dirty[0], cursorPtr->dirty[1]);
			cursorPtr->hasDirty = FALSE;
		}
		Widget_Invalidate(widgetPtr, cursorPtr->row, cursorPtr->col);
	}
		
	widgetPtr->flags |= WIDGET_DRAW_INVALID;
	Widget_EventuallyRedraw(widgetPtr);

	return TCL_OK;
}

/*
 * Item deletion callback
 */
static void	DeleteCursor(Widget *widgetPtr, WidgetItem *itemPtr)
{
	/* Do nothing? */
	(void) widgetPtr;
	(void) itemPtr;
}

/*
 * Item display callback
 */
static int DisplayCursor(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	CursorItem *cursorPtr = (CursorItem *) itemPtr;
	int col, row;
	XGCValues gcValues;
	GC gc;
	
	/* Hack - ignore parameter */
	(void) interp;

	if (CalcCursorPosition(widgetPtr, cursorPtr))
	{
		int lineWidth = cursorPtr->lineWidth;
		int gHeight = widgetPtr->gheight;
		int gWidth = widgetPtr->gwidth;
		
		col = cursorPtr->col, row = cursorPtr->row;
	
		gcValues.foreground = cursorPtr->color->pixel;
		gcValues.line_width = lineWidth;
		gc = Tk_GetGC(widgetPtr->tkwin, GCForeground | GCLineWidth, &gcValues);

		XDrawRectangle(widgetPtr->display,
			widgetPtr->bitmap.pixmap, gc,
			col * gWidth + lineWidth / 2,
			row * gHeight + lineWidth / 2,
			gWidth - lineWidth / 2 - 1,
			gHeight - lineWidth / 2 - 1);
		
		
		Tk_FreeGC(widgetPtr->display, gc);

		cursorPtr->dirty[0] = cursorPtr->row;
		cursorPtr->dirty[1] = cursorPtr->col;
		cursorPtr->hasDirty = TRUE;
	}
	else
	{
		itemPtr->minY = 0, itemPtr->maxY = -1;
		itemPtr->minX = 0, itemPtr->maxX = -1;
	}

	return TCL_OK;
}

/*
 * Calculates the row and column in the parent Widget of the cursor.
 * Return 1 if the row/column is in bounds, 0 otherwise.
 */
static int CalcCursorPosition(Widget *widgetPtr, CursorItem *cursorPtr)
{
	WidgetItem *itemPtr = (WidgetItem *) cursorPtr;
	int row, col;
	
	int gHeight = widgetPtr->gheight;
	int gWidth = widgetPtr->gwidth;

	if (!Widget_CaveToView(widgetPtr, cursorPtr->y, cursorPtr->x, &row, &col))
		return 0;

	cursorPtr->row = row;
	cursorPtr->col = col;
	itemPtr->minX = itemPtr->maxX = col;
	itemPtr->minY = itemPtr->maxY = row;

	itemPtr->x1 = col * gWidth - widgetPtr->bx;
	itemPtr->x2 = itemPtr->x1 + gWidth;
	itemPtr->y1 = row * gHeight - widgetPtr->by;
	itemPtr->y2 = itemPtr->y1 + gHeight;

	return 1;
}
