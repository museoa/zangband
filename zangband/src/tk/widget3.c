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

#define DEF_BEVEL_LIGHT ((char *) "Black")
#define DEF_BEVEL_DARK ((char *) "White")
#define DEF_BEVEL_OPACITY ((char *) "127")
	
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
