/* File: map2.c */

/* Purpose: micro-map backend for Widget */

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
#include "map.h"

#include <limits.h>
#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif

t_symbol **g_symbol;
int g_symbol_count;

IconPtr *g_bits[5];
int g_bits_count[5];

Tcl_HashTable g_symbol_hash;


int symbol_find(Tcl_Interp *interp, Tcl_Obj *objName, char *strName,
	t_symbol **symbolPtrPtr, int *symbolIndex)
{
	char *t;
	Tcl_HashEntry *hPtr;
	int index;

	/* Symbol name */
	if (objName)
	{
		t = Tcl_GetString(objName);
	}
	else
	{
		t = strName;
	}

	/* Find the symbol by name our hash table */
	hPtr = Tcl_FindHashEntry(&g_symbol_hash, t);

	/* No such symbol */
	if (hPtr == NULL)
	{
		/* Set the error */
		Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "symbol \"",
			t, "\" doesn't exist", NULL);

		/* Failure */
		return TCL_ERROR;
	}

	index = (int) Tcl_GetHashValue(hPtr);
	if (symbolIndex) (*symbolIndex) = index;
	if (symbolPtrPtr) (*symbolPtrPtr) = g_symbol[index];

	return TCL_OK;
}



#define JUMP(p,d) \
	((byte *) p + (d))
#define INCR(p,d) \
	p = (void *) ((byte *) p + (d));
#define MOVE2(d,s) \
	*((u16b *) d) = *((u16b *) s); INCR(s,2)
#define MOVE4(d,s) \
	*((u32b *) d) = *((u32b *) s); INCR(s,4)
#define MOVE6(d,s) \
	MOVE4(d,s) \
	MOVE2(JUMP(d,4),s)
#define MOVE8(d,s) \
	MOVE4(d,s) \
	MOVE4(JUMP(d,4),s)
#define MOVE12(d,s) \
	MOVE8(d,s) \
	MOVE4(JUMP(d,8),s)
#define MOVE16(d,s) \
	MOVE12(d,s) \
	MOVE4(JUMP(d,12),s)
#define MOVE18(d,s) \
	MOVE16(d,s) \
	MOVE2(JUMP(d,16),s)
#define MOVE24(d,s) \
	MOVE16(d,s) \
	MOVE8(JUMP(d,16),s)
#define MOVE32(d,s) \
	MOVE16(d,s) \
	MOVE16(JUMP(d,16),s) \

/*
 * Draw a 4x4 map grid
 */
static void Widget_DrawSymbol4_8bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE4(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE4(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE4(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE4(dstPtr, srcPtr)
}

/*
 * Draw a 6x6 map grid
 */
static void Widget_DrawSymbol6_8bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE6(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE6(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE6(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE6(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE6(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE6(dstPtr, srcPtr)
}

/*
 * Draw an 8x8 map grid
 */
static void Widget_DrawSymbol8_8bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
}

/*
 * Draw a 4x4 map grid
 */
static void Widget_DrawSymbol4_16bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE8(dstPtr, srcPtr)
}

/*
 * Draw a 6x6 map grid
 */
static void Widget_DrawSymbol6_16bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE12(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE12(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE12(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE12(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE12(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE12(dstPtr, srcPtr)
}

/*
 * Draw an 8x8 map grid
 */
static void Widget_DrawSymbol8_16bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
}

/*
 * Draw a 4x4 map grid
 */
static void Widget_DrawSymbol4_24bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE12(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE12(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE12(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE12(dstPtr, srcPtr)
}

/*
 * Draw a 6x6 map grid
 */
static void Widget_DrawSymbol6_24bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE18(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE18(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE18(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE18(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE18(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE18(dstPtr, srcPtr)
}

/*
 * Draw an 8x8 map grid
 */
static void Widget_DrawSymbol8_24bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
}

/*
 * Draw a 4x4 map grid
 */
static void Widget_DrawSymbol4_32bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE16(dstPtr, srcPtr)
}

/*
 * Draw a 6x6 map grid
 */
static void Widget_DrawSymbol6_32bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE24(dstPtr, srcPtr)
}

/*
 * Draw an 8x8 map grid
 */
static void Widget_DrawSymbol8_32bit(long *srcPtr, long *dstPtr, long pitch)
{
	MOVE32(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE32(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE32(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE32(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE32(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE32(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE32(dstPtr, srcPtr)
	INCR(dstPtr, pitch)
	MOVE32(dstPtr, srcPtr)
}

DrawSymbolProc symbolProcTable[4][5] = {
	{
	Widget_DrawSymbol4_8bit,
	NULL, /* Widget_DrawSymbol5_8bit, */
	Widget_DrawSymbol6_8bit,
	NULL, /*Widget_DrawSymbol7_8bit, */
	Widget_DrawSymbol8_8bit
	},
	{
	Widget_DrawSymbol4_16bit,
	NULL,
	Widget_DrawSymbol6_16bit,
	NULL,
	Widget_DrawSymbol8_16bit
	},
	{
	Widget_DrawSymbol4_24bit,
	NULL,
	Widget_DrawSymbol6_24bit,
	NULL,
	Widget_DrawSymbol8_24bit
	},
	{
	Widget_DrawSymbol4_32bit,
	NULL,
	Widget_DrawSymbol6_32bit,
	NULL,
	Widget_DrawSymbol8_32bit
	}
};


int Map_Init(Tcl_Interp *interp)
{
	int i;

	g_symbol = Array_New(1, sizeof(t_symbol *));
	g_symbol_count = 0;

	for (i = 0; i < 5; i++)
	{
		g_bits[i] = Array_New(1, sizeof(IconPtr *));
		g_bits_count[i] = 0;
	}

	Tcl_InitHashTable(&g_symbol_hash, TCL_STRING_KEYS);

	return TCL_OK;
}
