/* File: widget.h */

/* Purpose: Widget definitions */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "widget-dll.h"

extern void map_draw_all(Widget *widgetPtr);
extern void map_draw_invalid(Widget *widgetPtr);
extern int map_symbol_proc(Widget *widgetPtr, int y, int x);

/* Extended Widget record */
typedef struct ExWidget {
	Widget widget;
	IconSpec *effect; /* Per-tile effect icons */

	/* WIDGET_STYLE_ICON */
	void (*whatToDrawProc)(Widget *widgetPtr, int y, int x, t_display *wtd);

	/* WIDGET_STYLE_MAP */
	int (*symbolProc)(Widget *widgetPtr, int y, int x); /* For micro-map */
} ExWidget;


