/* File: map.c */

/* Purpose: micro-map backend for Widget */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include <tcl.h>
#include "angband.h"
#include "tnb.h"
#include "interp.h"
#include "cmdinfo-dll.h"
#include "util-dll.h"
#include "icon.h"
#include "widget.h"
#include "map-dll.h"

#define USE_MAP_PET /* Friends and pets use different color */

/* Symbol for each cave grid */
byte *g_map_symbol[DUNGEON_HGT];

#define SYMBOL_ASSIGN_CHARACTER 0
#define SYMBOL_ASSIGN_FEATURE 1
#define SYMBOL_ASSIGN_MONSTER 2
#define SYMBOL_ASSIGN_OBJECT 3
#define SYMBOL_ASSIGN_MAX 4

typedef struct t_symbol_assign t_symbol_assign;

struct t_symbol_assign
{
	int count;
	int *assign;
};

t_symbol_assign g_symbol_assign[SYMBOL_ASSIGN_MAX];

#define SYMBOL_SPECIAL_BLANK 0
#define SYMBOL_SPECIAL_PET 1
#define SYMBOL_SPECIAL_MAX 2
int g_symbol_special[SYMBOL_SPECIAL_MAX];

int map_symbol_find(Tcl_Interp *interp, Tcl_Obj *objName, int *symbolIndex)
{
	return symbol_find(interp, objName, NULL, NULL, symbolIndex);
}

char *map_symbol_name(int symbolIndex)
{
	return g_symbol[symbolIndex]->name;
}

int map_symbol_feature(int f_idx)
{
	return g_symbol_assign[SYMBOL_ASSIGN_FEATURE].assign[f_idx];
}

/* special pet|dark ?symbol? */
int objcmd_symbol_special(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	static char *option[] = {"blank", "pet", NULL};
	int special, symbol;

	if (Tcl_GetIndexFromObj(interp, objV[1], option,
		"special", 0, &special) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (objC == 2)
	{
		symbol = g_symbol_special[special];
		Tcl_SetResult(interp, g_symbol[symbol]->name, TCL_STATIC);
		return TCL_OK;
	}

	if (symbol_find(interp, objV[2], NULL, NULL, &symbol) != TCL_OK)
	{
		return TCL_ERROR;
	}

	g_symbol_special[special] = symbol;

	return TCL_OK;
}

/*
 * assign $group $member ?$symbol?
 */
int objcmd_symbol_assign(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	static char *optionAssign[] = {"character", "feature", "monster",
		"object", NULL};
	int group, member;
	int symbol;

	if (Tcl_GetIndexFromObj(interp, objV[1], optionAssign,
		"group", 0, &group) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj(interp, objV[2], &member) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if ((member < 0) || (member >= g_symbol_assign[group].count))
	{
		Tcl_SetObjResult(interp,
			Tcl_NewStringObj(format("bad member \"%d\" : "
			"must be from 0 to %d", member,
			g_symbol_assign[group].count - 1), -1));
		return TCL_ERROR;
	}

	/* Return the current assignment */
	if (objC == 3)
	{
		symbol = g_symbol_assign[group].assign[member];
		Tcl_SetResult(interp, g_symbol[symbol]->name, TCL_STATIC);
		return TCL_OK;
	}

	if (symbol_find(interp, objV[3], NULL, NULL, &symbol) != TCL_OK)
	{
		return TCL_ERROR;
	}
	g_symbol_assign[group].assign[member] = symbol;

	return TCL_OK;
}

/*
 * Determine the symbol for given cave location
 */
void map_symbol_set(int y, int x)
{
	int m_idx, o_idx, f_idx;
	t_grid *gridPtr = &g_grid[y][x];
	int symbol;

	m_idx = gridPtr->m_idx;
	o_idx = gridPtr->o_idx;
	f_idx = gridPtr->f_idx;

	/* Character */
	if (m_idx == -1)
	{
		symbol = g_symbol_assign[SYMBOL_ASSIGN_CHARACTER].assign[0]; 
	}

	/* Monster */
	else if (m_idx > 0)
	{
		monster_type *m_ptr = &m_list[m_idx];
#ifdef USE_MAP_PET
		if (monster_is_friend(m_ptr))
			symbol = g_symbol_special[SYMBOL_SPECIAL_PET];
		else
#endif /* USE_MAP_PET */
			symbol = g_symbol_assign[SYMBOL_ASSIGN_MONSTER].assign[m_ptr->r_idx];
	}

	/* Object */
	else if (o_idx)
	{
		object_type *o_ptr = &o_list[o_idx];
		symbol = g_symbol_assign[SYMBOL_ASSIGN_OBJECT].assign[o_ptr->k_idx];
	}

	/* Feature */
	else if (f_idx)
	{
		symbol = g_symbol_assign[SYMBOL_ASSIGN_FEATURE].assign[f_idx];
	}

	/* Blank */
	else
	{
		symbol = g_symbol_special[SYMBOL_SPECIAL_BLANK];
	}

	g_map_symbol[y][x] = symbol;
}

#if 0

/* Is this ever called? */
void Widget_DrawMap(Widget *widgetPtr, int y, int x)
{
	DrawSymbolProc symbolProc;
	IconPtr *tilePtr;
	long *srcPtr, *dstPtr, pitch;
	int night, symbol, size = widgetPtr->gwidth;

	symbolProc = symbolProcTable[g_pixel_size - 1][size - 4];
	tilePtr = g_bits[size - 4];

	night = (p_ptr_depth || !g_daytime);

	if (in_bounds_test(widgetPtr->y_min + y, widgetPtr->x_min + x))
	{
		symbol = g_map_symbol[widgetPtr->y_min + y][widgetPtr->x_min + x];
#if USE_MAP_MIMIC
		symbol = g_symbol[symbol]->mimic;
#endif
		if (night && g_symbol[symbol]->light)
			symbol += g_grid[y][x].dark;
	}
	else
	{
		symbol = g_symbol_assign[SYMBOL_ASSIGN_FEATURE].assign[0];
	}

	pitch = widgetPtr->bitmap.pitch;
	srcPtr = (long *) (long *) tilePtr[symbol];
	dstPtr = (long *) (widgetPtr->bitmap.pixelPtr + x * size * g_pixel_size +
		y * size * pitch);

	(*symbolProc)(srcPtr, dstPtr, pitch);
}

#endif /* 0 */

#define INCR(p,d) \
	p = (void *) ((byte *) p + (d));

/* ExWidget.symbolProc() */
int map_symbol_proc(Widget *widgetPtr, int y, int x)
{
	int night = (p_ptr_depth || !g_daytime);
	int symbol = -1;

	if (in_bounds_test(y, x))
	{
		symbol = g_map_symbol[y][x];
#if USE_MAP_MIMIC
		symbol = g_symbol[symbol]->mimic;
#endif
		if (night && g_symbol[symbol]->light)
			symbol += g_grid[y][x].dark;
	}

	return symbol;
}

/* Widget.drawProc() */
void map_draw_all(Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;
	int y, x, y_min, y_max, x_min, x_max;
	DrawSymbolProc symbolProc;
	IconPtr *tilePtr;
	long *srcPtr, *dstPtr, *rowPtr, pitch;
	int size = widgetPtr->gwidth;
	int symbol, symbol_blank;
	DoubleLink *link;

	y_min = widgetPtr->y_min;
	y_max = widgetPtr->y_max;

	x_min = widgetPtr->x_min;
	x_max = widgetPtr->x_max;

	symbolProc = symbolProcTable[g_pixel_size - 1][size - 4];
	tilePtr = g_bits[size - 4];

	symbol_blank = g_symbol_special[SYMBOL_SPECIAL_BLANK];

	rowPtr = (long *) widgetPtr->bitmap.pixelPtr;
	pitch = widgetPtr->bitmap.pitch;

	for (y = y_min; y < y_max; y++)
	{
		dstPtr = rowPtr;
		for (x = x_min; x < x_max; x++)
		{
			symbol = (*exPtr->symbolProc)(widgetPtr, y, x);
			if (symbol < 0)
				symbol = symbol_blank;
			srcPtr = (long *) tilePtr[symbol];
			(*symbolProc)(srcPtr, dstPtr, pitch);
			INCR(dstPtr, size * g_pixel_size)
		}
		INCR(rowPtr, size * pitch)
	}

	/* Nothing is invalid */
	widgetPtr->dirty[0] = widgetPtr->cc - 1;
	widgetPtr->dirty[1] = widgetPtr->rc - 1;
	widgetPtr->dirty[2] = 0;
	widgetPtr->dirty[3] = 0;

	/* Now draw all of the items for this Widget */
	for (link = widgetPtr->linkerItemVis.head; link; link = link->next)
	{
		WidgetItem *itemPtr = DoubleLink_Data(link, WidgetItem);

		(*itemPtr->typePtr->displayProc)(widgetPtr->interp, widgetPtr,
			itemPtr);

		/* Invalidate the grids covered by the item */
		Widget_InvalidateArea(widgetPtr, itemPtr->minY, itemPtr->minX,
			itemPtr->maxY, itemPtr->maxX);
	}

	/* Set dirty bounds to entire window */
	widgetPtr->dx = widgetPtr->bx;
	widgetPtr->dy = widgetPtr->by;
	widgetPtr->dw = widgetPtr->width;
	widgetPtr->dh = widgetPtr->height;
}

/* Widget.invalidProc() */
void map_draw_invalid(Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;
	int y, x, y_min, y_max, x_min, x_max;
	DrawSymbolProc symbolProc;
	IconPtr *tilePtr;
	long *srcPtr, *dstPtr, *rowPtr, pitch;
	int size = widgetPtr->gwidth;
	int symbol, symbol_blank;
	DoubleLink *link;

	y_min = widgetPtr->dirty[1];
	y_max = widgetPtr->dirty[3];

	x_min = widgetPtr->dirty[0];
	x_max = widgetPtr->dirty[2];

	symbolProc = symbolProcTable[g_pixel_size - 1][size - 4];
	tilePtr = g_bits[size - 4];

	symbol_blank = g_symbol_special[SYMBOL_SPECIAL_BLANK];

	pitch = widgetPtr->bitmap.pitch;
	rowPtr = (long *) (widgetPtr->bitmap.pixelPtr
		+ x_min * size * g_pixel_size
		+ y_min * size * pitch);

	for (y = y_min; y <= y_max; y++)
	{
		dstPtr = rowPtr;
		for (x = x_min; x <= x_max; x++)
		{
			symbol = (*exPtr->symbolProc)(widgetPtr,
				widgetPtr->y_min + y, widgetPtr->x_min + x);
			if (symbol < 0)
				symbol = symbol_blank;
			srcPtr = (long *) tilePtr[symbol];
			(*symbolProc)(srcPtr, dstPtr, pitch);
			INCR(dstPtr, size * g_pixel_size)
		}
		INCR(rowPtr, size * pitch)
	}
	
	/* Nothing is invalid */
	widgetPtr->dirty[0] = widgetPtr->cc - 1;
	widgetPtr->dirty[1] = widgetPtr->rc - 1;
	widgetPtr->dirty[2] = 0;
	widgetPtr->dirty[3] = 0;

	/* Now draw all of the items for this Widget */
	for (link = widgetPtr->linkerItemVis.head; link; link = link->next)
	{
		WidgetItem *itemPtr = DoubleLink_Data(link, WidgetItem);

		(*itemPtr->typePtr->displayProc)(widgetPtr->interp, widgetPtr,
			itemPtr);

		/* Invalidate the grids covered by the item */
		Widget_InvalidateArea(widgetPtr, itemPtr->minY, itemPtr->minX,
			itemPtr->maxY, itemPtr->maxX);
	}

	/* Dirty bounds for display */
	widgetPtr->dx = x_min * size;
	widgetPtr->dy = y_min * size;
	widgetPtr->dw = (x_max - x_min + 1) * size;
	widgetPtr->dh = (y_max - y_min + 1) * size;
}

static CommandInit commandInit[] = {
	{0, "symbol", 0, 0, (char *) NULL, (Tcl_ObjCmdProc *) NULL, (ClientData) 0},
		{1, "assign", 3, 4, "group member ?symbol?", objcmd_symbol_assign, (ClientData) 0},
		{1, "special", 2, 3, "pet|dark ?symbol?", objcmd_symbol_special, (ClientData) 0},
	{0, (char *) NULL, 0, 0, (char *) NULL, (Tcl_ObjCmdProc *) NULL, (ClientData) 0}
};

static void assign_wipe(t_symbol_assign *groupPtr)
{
	int i;

	for (i = 0; i < groupPtr->count; i++)
	{
		groupPtr->assign[i] = 0;
	}
}

void init_map(void)
{
	int i;

	(void) Map_Init(g_interp);

	g_symbol_assign[SYMBOL_ASSIGN_CHARACTER].count = 1;
	g_symbol_assign[SYMBOL_ASSIGN_CHARACTER].assign =
		(int *) Tcl_Alloc(1 * sizeof(int));
	assign_wipe(&g_symbol_assign[SYMBOL_ASSIGN_CHARACTER]);

	g_symbol_assign[SYMBOL_ASSIGN_FEATURE].count = MAX_F_IDX;
	g_symbol_assign[SYMBOL_ASSIGN_FEATURE].assign =
		(int *) Tcl_Alloc(MAX_F_IDX * sizeof(int));
	assign_wipe(&g_symbol_assign[SYMBOL_ASSIGN_FEATURE]);

	g_symbol_assign[SYMBOL_ASSIGN_MONSTER].count = MAX_R_IDX;
	g_symbol_assign[SYMBOL_ASSIGN_MONSTER].assign =
		(int *) Tcl_Alloc(MAX_R_IDX * sizeof(int));
	assign_wipe(&g_symbol_assign[SYMBOL_ASSIGN_MONSTER]);

	g_symbol_assign[SYMBOL_ASSIGN_OBJECT].count = MAX_K_IDX;
	g_symbol_assign[SYMBOL_ASSIGN_OBJECT].assign =
		(int *) Tcl_Alloc(MAX_K_IDX * sizeof(int));
	assign_wipe(&g_symbol_assign[SYMBOL_ASSIGN_OBJECT]);

	for (i = 0; i < DUNGEON_HGT; i++)
	{
		/* Info about what feature/monster/object is known. */
		C_MAKE(g_map_symbol[i], DUNGEON_WID, byte);
	}

	for (i = 0; i < SYMBOL_SPECIAL_MAX; i++)
	{
		g_symbol_special[i] = 0;
	}

	(void) CommandInfo_Init(g_interp, commandInit, NULL);
}
