/* File: term.c */

/* Purpose: Term emulation stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
/* #include "icon.h" */

typedef struct Widget Widget;

struct Widget
{
    Tk_Window tkwin;
    Display *display;
    Tcl_Interp *interp;
    Tcl_Command widgetCmd;
	GC gc;						/* XCopyArea() context */
	BitmapPtr bitmap;			/* Offscreen bitmap */
    int width;					/* # of columns */
    int height;					/* # of rows */
	
	Tk_Font font;
	int font_y;
	
	BitmapPtr tiles;			/* The graphical tiles */

    int oldWidth, oldHeight;	/* To notice changes */

	int y, x;					/* Cave location (center of widget) */

	int rc, cc;					/* Rows, columns */
	int bh, bw;					/* Bitmap width & height */
	int by, bx;					/* Offset of window from bitmap */
	int dy, dx, dw, dh;			/* Dirty rect for copying */

	int y_min, y_max;           /* Limits of displayed info */
	int x_min, x_max;           /* Limits of displayed info */
    int gwidth;					/* Source column width */
    int gheight;				/* Source row height */
    int oldGWidth, oldGHeight;	/* To notice changes */
	
	#define WIDGET_REDRAW 0x01
	#define WIDGET_DELETED 0x02
	#define WIDGET_EXPOSE 0x04
	#define WIDGET_WIPE 0x08
	byte flags;                  /* Misc flags */

};

/* Predeclare */
static void widget_draw_all(Widget *widgetPtr);

/* Hack - have one widget for the current term */
static Widget *tnb_term;

/* Colour depth */
static int term_icon_depth;

/*
 * Actually draw stuff into the Widget's display. This routine is
 * usually passed to Tcl_DoWhenIdle().
 */
static void Widget_Display(ClientData clientData)
{
	Widget *widgetPtr = (Widget *) clientData;
	Tk_Window tkwin = widgetPtr->tkwin;

	/* We want to draw all grids */
	if (widgetPtr->flags & WIDGET_WIPE)
	{
		/* Forget that a wipe (and redraw) is needed */
		widgetPtr->flags &= ~(WIDGET_WIPE);

		/* Draw all grids */
		widget_draw_all(widgetPtr);
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
		widgetPtr->bitmap->pixmap, /* source drawable */
		Tk_WindowId(tkwin), /* dest drawable */
		widgetPtr->gc, /* graphics context */
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
 * Draw a blank square at this 'unkown' location
 */
static void DrawBlank(int x, int y, Widget *widgetPtr)
{
	BitmapPtr bitmapPtr = widgetPtr->bitmap;
	byte *dstPtr;
	int y2;
	int length = widgetPtr->gwidth * bitmapPtr->pixelSize;
	
	int xp = (x - widgetPtr->x_min) * widgetPtr->gwidth;
	int yp = (y - widgetPtr->y_min) * widgetPtr->gheight;
	
	/* Get the address of where to write the data in the bitmap */
	dstPtr = bitmapPtr->pixelPtr + xp * bitmapPtr->pixelSize + yp * bitmapPtr->pitch;
	
	/* Clear the area */
	for (y2 = 0; y2 < widgetPtr->gheight; y2++)
	{
		C_WIPE(dstPtr, length, byte);
		dstPtr += bitmapPtr->pitch;
	}
}

/*
 * Wipe n characters at position x, y
 */
errr Term_wipe_tnb(int x, int y, int n)
{
	int i;
	
	/* Paranoia */
	if (!tnb_term) return (1);
	
	for (i = 0; (i < n) && (x + i < 80); i++)
	{
		DrawBlank(x + i, y, tnb_term);
	}
	
	/* Set dirty bounds to entire window */
	tnb_term->dx = tnb_term->bx;
	tnb_term->dy = tnb_term->by;
	tnb_term->dw = tnb_term->width;
	tnb_term->dh = tnb_term->height;
	
	/* Redraw later */
	Widget_EventuallyRedraw(tnb_term);

	return (0);
}


errr Term_pict_tnb(int x, int y, int n, const byte *ap, const char *cp, const byte *tap, const char *tcp)
{
	/* Hack - ignore parameters for now */
	(void) ap;
	(void) cp;
	(void) tap;
	(void) tcp;
	
	/* Hack - we don't support icons here (yet) */
	return (Term_wipe_tnb(x, y, n));
}

/* Colours for the text */
static u32b colors[16];

static u16b r16mask = 0, r16shift = 0;
static u16b g16mask = 0, g16shift = 0;
static u16b b16mask = 0, b16shift = 0;

static int count_ones(u16b mask)
{
	int n;

	for (n = 0; mask != 0; mask &= mask - 1)
	{
		n++;
	}
	
	return (n);
}


/* Initialise the 16bit colour masks and shifts */
static void init_masks(Tcl_Interp *interp)
{
	Tk_Window tkwin = Tk_MainWindow(interp);
	Visual *visual = Tk_Visual(tkwin);
	
	int redcount, greencount, bluecount;
	
	r16mask = visual->red_mask;
	g16mask = visual->green_mask;
	b16mask = visual->blue_mask;

#ifdef PLATFORM_WIN
	/* XXX Always 5-5-5 */
	r16mask = 0x7C00;
	g16mask = 0x03E0;
	b16mask = 0x001F;
#endif
	
	/* Get red mask and shift */
	redcount = count_ones(r16mask);
	greencount = count_ones(g16mask);
	bluecount = count_ones(b16mask);
	
	r16shift = redcount + greencount + bluecount - 8;
	g16shift = greencount + bluecount - 8;
	b16shift = 8 - bluecount;
}


static void set_colours(Widget *widgetPtr)
{
	int i;
	
	int depth = widgetPtr->bitmap->depth;
	
	for (i = 0; i < 16; i++)
	{
		u32b r, g, b;
		
		r = angband_color_table[i][1];
		g = angband_color_table[i][2];
		b = angband_color_table[i][3];
		
		switch (depth)
		{
			case 8:
			{
				colors[i] = Palette_RGB2Index(r, g, b);
		
				break;
			}
		
			case 16:
			{
				u16b p;
				
				/* Convert to 16bit colour */
				
				p = (r << r16shift) & r16mask;
				p += (g << g16shift) & g16mask;
				p += (b >> b16shift) & b16mask;
				
				colors[i] = p;
				
				break;
			}
		
			case 24:
			{
				colors[i] = (r << 16) + (g << 8) + b;
				
				break;
			}
		}
	}
}


errr Term_xtra_tnb_react(void)
{
	/* Paranoia */
	if (!tnb_term) return (1);

	/* React to colours */
	set_colours(tnb_term);

	/* Redraw later */
	Widget_EventuallyRedraw(tnb_term);

	return 0;
}


/*
 * Draw stuff at this location
 */
static void DrawIconSpec(int x, int y, byte attr, char c, Widget *widgetPtr)
{
	int x2, y2, width;
	
	int xp, yp;
	
	char buf[2];
	
    XGCValues gcValues;
	
	/* Draw a blank square first */
	DrawBlank(x, y, widgetPtr);
	
	/* Only draw characters */
	if (attr & 0x80) return;
	
	/* Make our one-character long string */
	buf[0] = c;
	buf[1] = '\0';
	
	/* Change the colour */
	gcValues.foreground = colors[attr];
	widgetPtr->gc = Tk_GetGC(widgetPtr->tkwin, GCForeground, &gcValues);
	
	/* Calculate the width of the character */
	width = Tk_TextWidth(widgetPtr->font, buf, 1);
	
	/* Calculate the position of the character in the bitmap */
	x2 = (widgetPtr->gwidth - width) / 2;
	y2 = widgetPtr->font_y;
	
	xp = (x - widgetPtr->x_min) * widgetPtr->gwidth;
	yp = (y - widgetPtr->y_min) * widgetPtr->gheight;
	
	/* Draw the character into the bitmap */
	Tk_DrawChars(widgetPtr->display, widgetPtr->bitmap->pixmap, widgetPtr->gc,
				widgetPtr->font, buf, 1,
				x2 + xp, y2 + yp);
}

/*
 * Draw a string of characters
 *
 * (This could be made more efficient by drawing
 * more than one character at a time.)
 */
errr Term_text_tnb(int x, int y, int n, byte a, const char *s)
{
	int i;
		
	/* Paranoia */
	if (!tnb_term) return (1);
	
	for (i = 0; (i < n) && (x + i < 80); i++)
	{
		DrawIconSpec(x + i, y, a, s[i], tnb_term);
	}
	
	/* Set dirty bounds to entire window */
	tnb_term->dx = tnb_term->bx;
	tnb_term->dy = tnb_term->by;
	tnb_term->dw = tnb_term->width;
	tnb_term->dh = tnb_term->height;
	
	/* Redraw later */
	Widget_EventuallyRedraw(tnb_term);

	return 0;
}


/*
 * Redraw everything.
 */
static void widget_draw_all(Widget *widgetPtr)
{
	int y, x;
	
	byte a;
	char c;
	
	/* Paranoia: make sure the bitmap exists */
	if (!widgetPtr->bitmap) return;

	for (x = widgetPtr->x_min; x < widgetPtr->x_max; x++)
	{
		for (y = widgetPtr->y_min; y < widgetPtr->y_max; y++)
		{
			if ((x < 0) || (x >= 80) || (y < 0) || (y >= 24))
			{
				/* Just "erase" this spot */
				DrawBlank(x, y, widgetPtr);
			}
			
			/* Hack - Get attr/char of this location */
			tnb_get_term(x, y, &a, &c);
			
			/* Draw stuff at that location */
			DrawIconSpec(x, y, a, c, widgetPtr);
		}
	}

	/* Set dirty bounds to entire window */
	widgetPtr->dx = widgetPtr->bx;
	widgetPtr->dy = widgetPtr->by;
	widgetPtr->dw = widgetPtr->width;
	widgetPtr->dh = widgetPtr->height;
}


/*
 * Create a bitmap as big as the given Widget. We get the address of
 * the bits so we can write directly into the bitmap. The bitmap is
 * 8-bit depth with a 256-color palette.
 */
static void Widget_CreateBitmap(Widget *widgetPtr)
{
	/* Create the bitmap structure */
	MAKE(widgetPtr->bitmap, BitmapType);

	/* Calculate the bitmap dimensions in pixels */
	widgetPtr->bitmap->width = widgetPtr->bw;
	widgetPtr->bitmap->height = widgetPtr->bh;

	widgetPtr->bitmap->depth = term_icon_depth;
	
	/* Create the bitmap */
	Bitmap_New(widgetPtr->interp, widgetPtr->bitmap);
}

/* Free the bitmap for this Widget */
static void Widget_DeleteBitmap(Widget *widgetPtr)
{
	Bitmap_Delete(widgetPtr->bitmap);
	
	/* Free the bitmap structure */
	FREE(widgetPtr->bitmap);
}


static void Widget_Calc(Widget *widgetPtr)
{
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
	
	/* Calculate the limits of visibility (only for term) */
	widgetPtr->y_min = 0;
	widgetPtr->y_max = widgetPtr->y_min + widgetPtr->height / widgetPtr->gheight;
	widgetPtr->x_min = 0;
	widgetPtr->x_max = widgetPtr->x_min + widgetPtr->width / widgetPtr->gwidth;

	widgetPtr->rc = rc;
	widgetPtr->cc = cc;

	widgetPtr->bw = cc * widgetPtr->gwidth;
	widgetPtr->bh = rc * widgetPtr->gheight;

	widgetPtr->bx = cLeft * widgetPtr->gwidth - dLeft;
	widgetPtr->by = rTop * widgetPtr->gheight - dTop;
}

static void Widget_Wipe(Widget *widgetPtr)
{
	/* Remember to redraw all grids later */
	widgetPtr->flags |= WIDGET_WIPE;

	/* Redraw later */
	Widget_EventuallyRedraw(widgetPtr);
}

/*
 * Wipe the term
 */
errr Term_xtra_tnb_clear(void)
{
	/* Paranoia */
	if (!tnb_term) return (1);

	Widget_Wipe(tnb_term);

	return 0;
}

static void Widget_Center(Widget *widgetPtr, int cx, int cy)
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
 * This procedure is called when the world has changed in some
 * way and the widget needs to recompute all its graphics contexts
 * and determine its new geometry.
 */
static void Widget_WorldChanged(ClientData instanceData)
{
    Widget *widgetPtr = (Widget *) instanceData;
	Tk_Window tkwin = widgetPtr->tkwin;
    XGCValues gcValues;

	/* Allocate  GC */
    if (widgetPtr->gc == None)
    {
		gcValues.foreground = 0xFFFFFF;
		gcValues.function = GXcopy;
		gcValues.graphics_exposures = False;
		gcValues.font = Tk_FontId(widgetPtr->font);
		widgetPtr->gc = Tk_GetGC(tkwin, GCForeground | GCFunction |
								 GCGraphicsExposures | GCFont, &gcValues);
    }
	
	if (!widgetPtr->font_y)
	{
		Tk_FontMetrics fm;

		/* Get info about the font */
		Tk_GetFontMetrics(widgetPtr->font, &fm);
		
		widgetPtr->font_y = /* (widgetPtr->gheight - fm.linespace) / 2*/ + fm.ascent;
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
	if ((widgetPtr->bitmap) &&
		((widgetPtr->bw != widgetPtr->bitmap->width) ||
		(widgetPtr->bh != widgetPtr->bitmap->height)))
	{
		/* Delete the bitmap */
		Widget_DeleteBitmap(widgetPtr);

		/* Forget the bitmap */
		widgetPtr->bitmap = NULL;
	}

	/* No bitmap yet */
	if (!widgetPtr->bitmap)
	{
		/* Allocate bitmap */
		Widget_CreateBitmap(widgetPtr);
	}

	/* The widget is of non-zero size */
    if ((widgetPtr->width > 0) && (widgetPtr->height > 0))
	{
		/* Request geometry */
		Tk_GeometryRequest(tkwin, widgetPtr->width, widgetPtr->height);
	}

	/* Remember the current info */
	widgetPtr->oldWidth = widgetPtr->width;
	widgetPtr->oldHeight = widgetPtr->height;
	widgetPtr->oldGWidth = widgetPtr->gwidth;
	widgetPtr->oldGHeight = widgetPtr->gheight;

	/* Redraw the window (later) */
	Widget_Wipe(widgetPtr);
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


/*
 * This is the window-specific command created for each new Widget.
 */
static int Widget_WidgetObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	static cptr commandNames[] = {"caveyx", "center", "configure",
		"wipe", "bounds", "visible", "hittest", NULL};
	enum {IDX_CAVEYX, IDX_CENTER, IDX_CONFIGURE,
		IDX_WIPE, IDX_BOUNDS, IDX_VISIBLE, IDX_HITTEST} option;
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
				
			strnfmt(buffer, 20, "%d %d", yc, xc);
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
				Widget_Center(widgetPtr, x, y);
			}
			else
			{
				/* Print the current center */
				strnfmt(buffer, 20, "%d %d", widgetPtr->y, widgetPtr->x);
		
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
			strnfmt(buf, 32, "%d %d %d %d",
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

			strnfmt(buffer, 20, "%d %d %d", yc, xc, layer);
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
    if (widgetPtr->gc != None)
    {
		Tk_FreeGC(widgetPtr->display, widgetPtr->gc);
    }

	/* Free the bitmap */
	if (widgetPtr->bitmap)
	{
		Widget_DeleteBitmap(widgetPtr);
	}
	
	/* Free the options table */
	Tk_FreeConfigOptions((char *) widgetPtr, optionTable,
		widgetPtr->tkwin);

	widgetPtr->tkwin = NULL;
	
	/* Free the font, if any */
	if (widgetPtr->font) Tk_FreeFont(widgetPtr->font);
	
	/* Free the tiles */
	if (widgetPtr->tiles) Bitmap_Delete(widgetPtr->tiles);

	/* Free the Widget struct */
    Tcl_EventuallyFree((ClientData) widgetPtr, Tcl_Free);
	
	/* We no longer have a term */
	tnb_term = NULL;
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
static Tk_ClassProcs widgetProcs = {
	sizeof(Tk_ClassProcs),
	Widget_WorldChanged,			/* geometryProc. */
	NULL,							/* createProc. */
    NULL							/* modalProc. */ 
};


/*
 * Table specifying legal configuration options for a Widget.
 */
static Tk_OptionSpec optionSpecs[] = {
    {TK_OPTION_INT, (char *) "-height", (char *) "height", (char *) "Height",
    (char *) "100", -1, Tk_Offset(Widget, height), 0, 0, 0},
    {TK_OPTION_INT, (char *) "-width", (char *) "width", (char *) "Width",
    (char *) "100", -1, Tk_Offset(Widget, width), 0, 0, 0},
    {TK_OPTION_INT, (char *) "-gheight", (char *) "gheight", (char *) "Height",
    (char *) "32", -1, Tk_Offset(Widget, gheight), 0, 0, 0},
    {TK_OPTION_INT, (char *) "-gwidth", (char *) "gwidth", (char *) "Width",
    (char *) "32", -1, Tk_Offset(Widget, gwidth), 0, 0, 0},
	{TK_OPTION_FONT, (char *) "-font", NULL, NULL,
	(char *) "{MS Sans Serif} 8", -1, Tk_Offset(Widget, font), 0, 0, 0},
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
    Tk_SetClass(tkwin, "Term");
	
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
	widgetPtr->gc = None;
    widgetPtr->width = 0;
    widgetPtr->height = 0;
    widgetPtr->gwidth = tnb_tile_x;
    widgetPtr->gheight = tnb_tile_y;
    widgetPtr->oldWidth = widgetPtr->oldHeight = 0;
    widgetPtr->oldGWidth = widgetPtr->oldGHeight = 0;
    widgetPtr->flags = 0;
	widgetPtr->y = widgetPtr->x = 0;
	widgetPtr->y_min = widgetPtr->y_max = 0;
	widgetPtr->x_min = widgetPtr->x_max = 0;
	widgetPtr->dx = widgetPtr->dy = 0;
	widgetPtr->dw = widgetPtr->dh = 0;

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
	
	/* Load the tiles */
	widgetPtr->tiles = Bitmap_Load(g_interp, tnb_tile_file);

	/* We now have a term */
	tnb_term = widgetPtr;

	
	/* Return the window pathname */
    Tcl_SetStringObj(Tcl_GetObjResult(interp), Tk_PathName(widgetPtr->tkwin),
    	-1);

	/* Success */
    return TCL_OK;
}

/*
 * Initialize the Term package
 */
int init_term(Tcl_Interp *interp, int g_icon_depth)
{
	/* Create the "term" interpreter command */
	Tcl_CreateObjCommand(interp, "term", Widget_ObjCmd, NULL, NULL);
	
	/* Initialise palette stuff */
	if (g_icon_depth == 16) init_masks(interp);
	
	/* Save for later */
	term_icon_depth = g_icon_depth;

	/* Success */
    return TCL_OK;
}



