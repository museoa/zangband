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

/* Extended Widget record */
typedef struct ExWidget {
	Widget widget;
	int spriteCnt;
	int vaultNum;
	void *vaultPtr;
	IconSpec *effect; /* Per-tile effect icons */

	/* WIDGET_STYLE_ICON */
	void (*whatToDrawProc)(Widget *widgetPtr, int y, int x, t_display *wtd);

	/* WIDGET_STYLE_MAP */
	int (*symbolProc)(Widget *widgetPtr, int y, int x); /* For micro-map */
} ExWidget;

extern void Widget_SetVault(Widget *widgetPtr);

