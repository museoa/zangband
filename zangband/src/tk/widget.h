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
typedef int Widget_ConfigureProc(Tcl_Interp *interp, Widget *widgetPtr);
typedef int Widget_CreateProc(Tcl_Interp *interp, Widget **widgetPtr);
typedef void Widget_DestroyProc(Widget *widgetPtr);
typedef void Widget_DrawAllProc(Widget *widgetPtr);
typedef void Widget_DrawInvalidProc(Widget *widgetPtr);
typedef int Widget_HitTestProc(Widget *widgetPtr, int x, int y, int col, int row, int *xc, int *yc);
typedef void Widget_InvalidateProc(Widget *widgetPtr, int row, int col);
typedef void Widget_InvalidateAreaProc(Widget *widgetPtr, int top, int left, int bottom, int right);
typedef void Widget_WipeProc(Widget *widgetPtr);

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
	Widget_ConfigureProc *configureProc; /* Per-widget config routine */
	Widget_DestroyProc *destroyProc; /* Per-widget destroy routine */
	Widget_DrawAllProc *drawAllProc;  /* Per-widget drawing routine */
	Widget_DrawInvalidProc *drawInvalidProc;  /* Per-widget drawing routine */
	Widget_InvalidateProc *invalidateProc;
	Widget_InvalidateAreaProc *invalidateAreaProc;
	Widget_WipeProc *wipeProc;  /* Per-widget fresh routine */
	DoubleLink link;			/* Linked list of Widget's */
	DoubleLink linkMap;			/* Linked list of mapped Widget's */
	DoubleLinker linkerItem;	/* List of items */
	DoubleLinker linkerItemVis;	/* List of visible items */
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
	DoubleLink link; /* Linked list of items */
	DoubleLink linkVis; /* Linked list of visible items */
} WidgetItem;

typedef int	WidgetItem_CreateProc _ANSI_ARGS_((Tcl_Interp *interp,
	Widget *widgetPtr, WidgetItem *itemPtr,
	int objc, Tcl_Obj *CONST objv[]));
typedef int	WidgetItem_ConfigProc _ANSI_ARGS_((Tcl_Interp *interp,
	Widget *widgetPtr, WidgetItem *itemPtr,
	int objc, Tcl_Obj *CONST objv[]));
typedef int	WidgetItem_DisplayProc _ANSI_ARGS_((Tcl_Interp *interp,
	Widget *widgetPtr, WidgetItem *itemPtr));
typedef int	WidgetItem_ChangedProc _ANSI_ARGS_((Tcl_Interp *interp,
	Widget *widgetPtr, WidgetItem *itemPtr));
typedef void WidgetItem_DeleteProc _ANSI_ARGS_((Widget *widgetPtr,
	WidgetItem *itemPtr));

/*
 * Records of the following type are used to describe a type of
 * extension (e.g. monster bar, status, etc.) that can modify the
 * appearance of a Widget.
 */

typedef struct WidgetItemType
{
	cptr name;
	int itemSize;
	Tk_OptionSpec *optionSpecs;
	Tk_OptionTable optionTable;
	WidgetItem_CreateProc *createProc;
	WidgetItem_ConfigProc *configProc;
	WidgetItem_DisplayProc *displayProc;
	WidgetItem_ChangedProc *changedProc;
	WidgetItem_DeleteProc *deleteProc;
	struct WidgetItemType *nextPtr;
} WidgetItemType;


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
extern void Widget_DrawAll(Widget *widgetPtr);
extern void Widget_EventuallyRedraw(Widget *widgetPtr);

extern int init_widget(Tcl_Interp *interp);

/* Extended Widget record */
typedef struct ExWidget {
	Widget widget;
	IconSpec *effect; /* Per-tile effect icons */

	void (*whatToDrawProc)(Widget *widgetPtr, int y, int x, t_display *wtd);
} ExWidget;

