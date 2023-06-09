/* File: map-dll.h */

/* Purpose: micro-map backend for Widget */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef _INCLUDE_MAPDLL_H_
#define _INCLUDE_MAPDLL_H_

#if !defined(PLATFORM_MAC) && !defined(PLATFORM_WIN) && !defined(PLATFORM_X11)
#error "you must define one of PLATFORM_MAC, PLATFORM_WIN or PLATFORM_X11"
#endif /* */

#define USE_MAP_DATA 1
#define USE_MAP_MIMIC 1

typedef struct t_symbol t_symbol;

struct t_symbol
{
	char *name;
	int inner;
	int outer;
	int light;
	IconPtr bits[5];
	IconValue bits4[16 * 4]; /* 4 is max pixel size */
	IconValue bits5[25 * 4];
	IconValue bits6[36 * 4];
	IconValue bits7[49 * 4];
	IconValue bits8[64 * 4];
#if USE_MAP_MIMIC
	int mimic; /* Get info from another symbol */
#endif
};

t_symbol **g_symbol;
int g_symbol_count;

IconPtr *g_bits[5];
int g_bits_count[5];

Tcl_HashTable g_symbol_hash;

typedef void (*DrawSymbolProc)(long *srcPtr, long *dstPtr, long pitch);
DrawSymbolProc symbolProcTable[4][5];

int Map_Init(Tcl_Interp *interp);
void DrawMapSymbol(Widget *widgetPtr, int y, int x, int symbol);
int symbol_find(Tcl_Interp *interp, Tcl_Obj *objName, char *strName,
	t_symbol **symbolPtrPtr, int *symbolIndex);

#endif /* _INCLUDE_MAPDLL_H_ */
