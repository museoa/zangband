/* File: widget.h */

/* Purpose: Widget definitions */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

typedef struct Widget Widget;
typedef void Widget_CenterProc(Widget *widgetPtr, int y, int x);
typedef void Widget_ChangedProc(Widget *widgetPtr);
typedef void Widget_DestroyProc(Widget *widgetPtr);
typedef int Widget_HitTestProc(Widget *widgetPtr, int x, int y, int col, int row, int *xc, int *yc);
typedef void Widget_InvalidateProc(Widget *widgetPtr, int row, int col);
typedef void Widget_InvalidateAreaProc(Widget *widgetPtr, int top, int left, int bottom, int right);

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
	Widget_CenterProc *centerProc; /* Per-widget centering routine */
	Widget_ChangedProc *changedProc; /* Per-widget config routine */
	Widget_DestroyProc *destroyProc; /* Per-widget destroy routine */
	Widget_InvalidateProc *invalidateProc;
	Widget_InvalidateAreaProc *invalidateAreaProc;
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

/*
 * For each extension in a Widget there exists one record with
 * the following structure.  Each actual item is represented by
 * a record with the following stuff at its beginning, plus additional
 * type-specific stuff after that.
 */

typedef struct WidgetItem
{
	int visible; /* TRUE if the item should be drawn */
    struct WidgetItemType *typePtr;	/* Table of procedures that
					 * implement this type of item. */
    int x1, y1, x2, y2;			/* Bounding box for item, in pixels.
					 * Set by item-specific code and guaranteed to
					 * contain every pixel drawn in item. Item area
					 * includes x1 and y1 but not x2 and y2. */
	int minX, minY, maxX, maxY; /* Rows/Columns clobbered in widget */
} WidgetItem;


/*
 * Widget items use tint tables for transparency. Since each tint table
 * takes 256 bytes, and there may be many Widget items, we allow items
 * to share tint tables.
 */
typedef struct t_widget_color
{
	int ref_cnt; /* Number of allocations; zero means free'd */
	int color; /* Palette index for this table */
	int opacity; /* Opacity for this table */
	TintTable tint; /* The tint table */
} t_widget_color;

extern void Widget_InvalidateArea(Widget *widgetPtr, int top, int left, int right, int bottom);
extern void Widget_EventuallyRedraw(Widget *widgetPtr);

/* Extended Widget record */
typedef struct ExWidget {
	Widget widget;
	IconSpec *effect; /* Per-tile effect icons */

	void (*whatToDrawProc)(Widget *widgetPtr, int y, int x, t_display *wtd);
} ExWidget;

