/* File: term.c */

/* Purpose: term view */

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
static void CalcLimits(Widget *widgetPtr, WidgetItem *itemPtr)
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


#define DEF_TERM_FILL ((char *) "White")

/*
 * The structure below defines the record for each term item.
 */
typedef struct TermItem {
	WidgetItem header; /* Required header info */
	int x, y; /* Pixel offsets in Widget bitmap */
    Tk_Font tkfont;		/* Font for drawing text. */
    char *text; /* Text for item (malloc'd). */
	int numChars; /* Number of non-NULL characters in text. */
	int width, height; /* Size of bounding rect in pixels */
	XColor *fill; /* Color info */
	int clipX, clipY; /* Boolean: Clip displayed area to size of text */
	int expandX, expandY; /* Boolean: Expand to text size if needed */
	int textHeight, textWidth, textLeft; /* Height & width of text, left of text */
	int textAscent;
	GC textGC;
	int dirty[4], hasDirty;
} TermItem;

#define TERM_SIZE 0x0001
#define TERM_MOVE 0x0002
#define TERM_DRAW 0x0004
#define TERM_DISPLAY 0x0008
#define TERM_TEXT 0x0010
#define TERM_FONT 0x0020
#define TERM_FILL 0x0040

#define TERM_EXPAND 0x0400
	
static Tk_OptionSpec optionSpecTerm[] = {
    {TK_OPTION_PIXELS, (char *) "-x", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TermItem, x), 0, 0, 0},
    {TK_OPTION_PIXELS, (char *) "-y", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TermItem, y), 0, 0, 0},
    {TK_OPTION_FONT, (char *) "-font", NULL, NULL,
	 (char *) "{MS Sans Serif} 8", -1, Tk_Offset(TermItem, tkfont), 0, 0,
	 TERM_FONT | TERM_TEXT | TERM_DRAW | TERM_DISPLAY},
    {TK_OPTION_STRING, (char *) "-text", NULL, NULL,
	 (char *) "", -1, Tk_Offset(TermItem, text), 0, 0,
	 TERM_TEXT | TERM_DRAW | TERM_DISPLAY},
    {TK_OPTION_PIXELS, (char *) "-width", NULL, NULL,
     (char *) "160", -1, Tk_Offset(TermItem, width), 0, 0,
     TERM_SIZE | TERM_DRAW | TERM_DISPLAY},
    {TK_OPTION_PIXELS, (char *) "-height", NULL, NULL,
     (char *) "7", -1, Tk_Offset(TermItem, height), 0, 0,
     TERM_SIZE | TERM_DRAW | TERM_DISPLAY},
    {TK_OPTION_COLOR, (char *) "-fill", NULL, NULL,
     DEF_TERM_FILL, -1, Tk_Offset(TermItem, fill), 0, 0,
     TERM_FILL | TERM_DISPLAY},
    {TK_OPTION_BOOLEAN, (char *) "-clipx", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TermItem, clipX), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-clipy", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TermItem, clipY), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-expandx", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TermItem, expandX), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-expandy", NULL, NULL,
     (char *) "0", -1, Tk_Offset(TermItem, expandY), 0, 0, 0},
    {TK_OPTION_BOOLEAN, (char *) "-visible", NULL, NULL,
     (char *) "0", -1, Tk_Offset(WidgetItem, visible), 0, 0,
     TERM_DISPLAY},
    {TK_OPTION_END, NULL, NULL, NULL,
     NULL, 0, -1, 0, 0, 0}
};

static int	CreateTerm _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr,
		    int objc, Tcl_Obj *CONST objv[]));
static int	ConfigureTerm _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr,
		    int objc, Tcl_Obj *CONST objv[]));
static int	DisplayTerm _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr));
static int	ChangedTerm _ANSI_ARGS_((Tcl_Interp *interp,
		    Widget *widgetPtr, WidgetItem *itemPtr));
static void	DeleteTerm _ANSI_ARGS_((Widget *widgetPtr,
			WidgetItem *itemPtr));
static int SanityCheckTerm(Widget *widgetPtr, TermItem *TermPtr);
static void ComputeTermLayout(TermItem *TermPtr, int *maskPtr);

WidgetItemType TermType = {
	"term",
	sizeof(TermItem),
	optionSpecTerm,
	NULL, /* optionTable */
	CreateTerm,
	ConfigureTerm,
	DisplayTerm,
	ChangedTerm,
	DeleteTerm,
	NULL
};

/*
 * Item creation callback
 */
static int CreateTerm(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr, int objc, Tcl_Obj *CONST objv[])
{
	TermItem *termPtr = (TermItem *) itemPtr;

	termPtr->header.visible = FALSE;
	termPtr->x = 0;
	termPtr->y = 0;
    termPtr->tkfont	= NULL;
    termPtr->text = NULL;
	termPtr->numChars = 0;
	termPtr->width = 0;
	termPtr->height = 0;
	termPtr->fill = NULL;
	termPtr->clipX = FALSE;
	termPtr->clipY = FALSE;
	termPtr->expandX = FALSE;
	termPtr->expandY = FALSE;
	termPtr->textHeight = 0;
	termPtr->textWidth = 0;
	termPtr->textLeft = 0;
	termPtr->textAscent = 0;
	termPtr->textGC = None;
	termPtr->hasDirty = FALSE;

	/* Set the default options for the new widget */
	if (Tk_InitOptions(interp, (char *) itemPtr,
		itemPtr->typePtr->optionTable, widgetPtr->tkwin) != TCL_OK)
	{
		DeleteTerm(widgetPtr, itemPtr);
		return TCL_ERROR;
	}

	if (ConfigureTerm(interp, widgetPtr, itemPtr, objc, objv)
		!= TCL_OK)
	{
		DeleteTerm(widgetPtr, itemPtr);
		return TCL_ERROR;
	}

	return TCL_OK;
}

/*
 * Item configuration callback
 */
static int ConfigureTerm(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr, int objc, Tcl_Obj *CONST objv[])
{
    Tk_SavedOptions savedOptions;
	Tcl_Obj *errorResult = NULL;
	int error, mask = 0;

	TermItem *termPtr = (TermItem *) itemPtr;
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

		redraw = (mask & TERM_DRAW) != 0;
		redisplay = (mask & TERM_DISPLAY) != 0;
		
		/* We are creating the item, so don't bother optimizing */
		if (termPtr->textGC == None)
		{
			termPtr->numChars = strlen(termPtr->text);
			termPtr->textWidth = Tk_TextWidth(termPtr->tkfont, termPtr->text,
				termPtr->numChars);

			Tk_GetFontMetrics(termPtr->tkfont, &fm);
			termPtr->textHeight = fm.linespace;
			termPtr->textAscent = fm.ascent;

			gcValues.foreground = termPtr->fill->pixel;
			gcValues.graphics_exposures = False;
			gcValues.font = Tk_FontId(termPtr->tkfont);
			gcMask = GCForeground | GCFont | GCGraphicsExposures;
			termPtr->textGC = Tk_GetGC(widgetPtr->tkwin, gcMask, &gcValues);

			ComputeTermLayout(termPtr, &layout);
	
			if (SanityCheckTerm(widgetPtr, termPtr) != TCL_OK)
			{
				continue;
			}
	
			/* Calculate which grids are covered by us */
			CalcLimits(widgetPtr, itemPtr);

		}
		else
		{
			resize = (mask & TERM_SIZE) != 0;
			text = (mask & TERM_TEXT) != 0;

			/* Text or font changed */
			if (text)
			{
				/* Calculate the length of the text */
				termPtr->numChars = strlen(termPtr->text);
	
				/* Calculate the width of the text in pixels */
				termPtr->textWidth = Tk_TextWidth(termPtr->tkfont,
					termPtr->text, termPtr->numChars);
			}

			if (mask & TERM_FONT)
			{
				Tk_GetFontMetrics(termPtr->tkfont, &fm);
				termPtr->textHeight = fm.linespace;
				termPtr->textAscent = fm.ascent;
			}

			if (mask & (TERM_FONT | TERM_FILL))
			{
				if (termPtr->textGC)
				{
					Tk_FreeGC(Tk_Display(widgetPtr->tkwin), termPtr->textGC);
				}
				gcValues.foreground = termPtr->fill->pixel;
				gcValues.graphics_exposures = False;
				gcValues.font = Tk_FontId(termPtr->tkfont);
				gcMask = GCForeground | GCFont | GCGraphicsExposures;
				termPtr->textGC = Tk_GetGC(widgetPtr->tkwin, gcMask, &gcValues);
			}

			/* Calculate the size and position on the display */
			ComputeTermLayout(termPtr, &layout);
	
			if (SanityCheckTerm(widgetPtr, termPtr) != TCL_OK)
			{
				continue;
			}
	
			/* The display size or position changed */
			if (layout)
			{
				/* Calculate which grids are covered by us */
				CalcLimits(widgetPtr, itemPtr);
	
				/* The display size changed */
				if (layout & TERM_SIZE)
				{
					/* Handle automatic expansion of bitmap */
					if (layout & TERM_EXPAND)
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
		if (termPtr->hasDirty && layout)
		{
			Widget_InvalidateArea(widgetPtr,
				termPtr->dirty[0], termPtr->dirty[1],
				termPtr->dirty[2], termPtr->dirty[3]);
			termPtr->hasDirty = FALSE;
			
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
static void	DeleteTerm(Widget *widgetPtr, WidgetItem *itemPtr)
{
	TermItem *termPtr = (TermItem *) itemPtr;

	if (termPtr->textGC)
	{
		Tk_FreeGC(Tk_Display(widgetPtr->tkwin), termPtr->textGC);
	}
}

/*
 * Calculates the coordinates of the term item
 */
static void ComputeTermLayout(TermItem *termPtr, int *maskPtr)
{
	WidgetItem *itemPtr = (WidgetItem *) termPtr;
	int leftX, topY, width, height;

	(*maskPtr) = 0;
	
	/* Get the requested width */
	width = termPtr->width;

	/* Get the requested height */
	height = termPtr->height;

	/* Clip vertically */
	if (termPtr->clipY)
	{
		/* Calculate height of text */
		height = termPtr->textHeight;
	}

	/* Clip horizontally */
	if (termPtr->clipX)
	{
		/* Calculate width of text */
		width = termPtr->textWidth;
	}

	/* Expand vertically */
	if (height > termPtr->height)
	{
		if (termPtr->expandY)
		{
			termPtr->height = height;
			(*maskPtr) |= TERM_EXPAND;
		}
		else
			height = termPtr->height;
	}

	/* Expand horizontally */
	if (width > termPtr->width)
	{
		if (termPtr->expandX)
		{
			termPtr->width = width;
			(*maskPtr) |= TERM_EXPAND;
		}
		else
			width = termPtr->width;
	}

	/*
	 * Compute the top-left corner
	 * of the bounding box for the term item.
	 */
	leftX = termPtr->x - width / 2;
	topY = termPtr->y - height / 2;

	/* The position changed */
	if ((itemPtr->x1 != leftX) || (itemPtr->y1 != topY))
	{
		(*maskPtr) |= TERM_MOVE;
	}

	/* The size changed */
	if (width != (itemPtr->x2 - itemPtr->x1) ||
		height != (itemPtr->y2 - itemPtr->y1))
	{
		(*maskPtr) |= TERM_SIZE;
	}

	itemPtr->x1 = leftX;
	itemPtr->y1 = topY;
	itemPtr->x2 = leftX + width;
	itemPtr->y2 = topY + height;

	termPtr->textLeft = 0;
}

/*
 * Return an error if there are problems with a term item's options
 */
static int SanityCheckTerm(Widget *widgetPtr, TermItem *termPtr)
{
	WidgetItem *itemPtr = (WidgetItem *) termPtr;

	/* Prevent illegal x size or position */
	if ((itemPtr->x1 < 0) || (termPtr->width < 0) ||
		(itemPtr->x2 > widgetPtr->width) ||
		((itemPtr->x2 - itemPtr->x1) > termPtr->width))
	{
		Tcl_AppendResult(widgetPtr->interp, "illegal x size or position",
			NULL);
		return TCL_ERROR;
	}

	/* Prevent illegal y size or position */
	if ((itemPtr->y1 < 0) || (termPtr->height < 0) ||
		(itemPtr->y2 > widgetPtr->height) ||
		((itemPtr->y2 - itemPtr->y1) > termPtr->height))
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
static int DisplayTerm_Solid(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	TermItem *termPtr = (TermItem *) itemPtr;
	Pixmap pixmap = widgetPtr->bitmap.pixmap;

	/* Hack - ignore unused parameter */
	(void) interp;

	/* Draw the text */
	Tk_DrawChars(widgetPtr->display, pixmap,
		termPtr->textGC, termPtr->tkfont, termPtr->text, termPtr->numChars,
		widgetPtr->bx + itemPtr->x1 + termPtr->textLeft,
		widgetPtr->by + itemPtr->y1 + termPtr->textAscent);
		
termPtr->dirty[0] = itemPtr->minY;
termPtr->dirty[1] = itemPtr->minX;
termPtr->dirty[2] = itemPtr->maxY;
termPtr->dirty[3] = itemPtr->maxX;
termPtr->hasDirty = TRUE;

	return TCL_OK;
}

/*
 * Item display callback
 */
static int DisplayTerm(Tcl_Interp *interp, Widget *widgetPtr,
	WidgetItem *itemPtr)
{
	/* Not transparent */
	return DisplayTerm_Solid(interp, widgetPtr, itemPtr);
}


/*
 * Item changed callback
 */
static int ChangedTerm(Tcl_Interp *interp, Widget *widgetPtr,
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
