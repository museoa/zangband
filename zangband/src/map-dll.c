/* File: map-dll.c */

/* Purpose: micro-map backend for Widget */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include "cmdinfo-dll.h"
#include "util-dll.h"
#include "icon-dll.h"
#include "widget-dll.h"
#include "map-dll.h"

#include <limits.h>
#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif

t_symbol **g_symbol;
int g_symbol_count;

IconPtr *g_bits[5];
int g_bits_count[5];

Tcl_HashTable g_symbol_hash;

/*
 * Wipe map widgets when symbols change.
 */
static void symbol_changed(void)
{
	DoubleLink *link;
	Widget *widgetPtr;

	for (link = WidgetListMap.head; link; link = link->next)
	{
		widgetPtr = DoubleLink_Data(link, Widget);
		if (widgetPtr->gwidth != g_icon_size)
		{
			Widget_Wipe(widgetPtr);
		}
	}
}

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

#define SetPix24(p,r,g,b) \
	(p)[0] = b; (p)[1] = g; (p)[2] = r;

/* ?-option value...? */
static int symbol_configure(Tcl_Interp *interp, t_symbol *symbolPtr, int objc,
	Tcl_Obj *CONST objv[], int objOffset)
{
	Tcl_Obj *CONST *objPtr;
	int objC;
	int index;
	t_symbol symbol;

	symbol.inner = -1;
	symbol.outer = -1;
	symbol.light = -1;

	objPtr = objv + objOffset;
	objC = objc - objOffset;

	while (objC > 1)
	{
		static cptr symbolOption[] = {"-inner", "-light", "-outer",
			"-data",
			NULL};

	    if (Tcl_GetIndexFromObj(interp, objPtr[0], (char **) symbolOption,
			(char *) "option", 0, &index) != TCL_OK)
		{
			return TCL_ERROR;
	    }

		switch (index)
		{
			case 0: /* inner */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &symbol.inner)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				/* VERIFY inner */
				break;

			case 1: /* light */
				if (Tcl_GetBooleanFromObj(interp, objPtr[1], &symbol.light)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				break;

			case 2: /* -outer */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &symbol.outer)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				/* VERIFY inner */
				break;

			case 3: /* -data */
			{
				Tk_Window tkwin = Tk_MainWindow(interp);
				Tcl_Obj *listObjPtr;
				int row, col, dataHeight, dataWidth;
				IconPtr dstPtr;

				listObjPtr = objPtr[1];
				if (Tcl_ListObjLength(interp, listObjPtr, &dataHeight)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				if ((dataHeight < 4) || (dataHeight > 8))
				{
					/* "illegal height" */
					return TCL_ERROR;
				}
				dstPtr = symbolPtr->bits[dataHeight - 4];
				for (row = 0; row < dataHeight; row++)
				{
					Tcl_Obj *rowPtr;

					if (Tcl_ListObjIndex(interp, listObjPtr, row,
						&rowPtr) != TCL_OK)
					{
						return TCL_ERROR;
					}
					if (Tcl_ListObjLength(interp, rowPtr, &dataWidth)
						!= TCL_OK)
					{
						return TCL_ERROR;
					}
					if (dataWidth != dataHeight)
					{
						/* "illegal width" */
						return TCL_ERROR;
					}
					for (col = 0; col < dataWidth; col++)
					{
						Tcl_Obj *colorPtr;
						XColor *xColorPtr;
						int r, g, b;

						if (Tcl_ListObjIndex(interp, rowPtr, col, &colorPtr)
							!= TCL_OK)
						{
							return TCL_ERROR;
						}
						if ((xColorPtr = Tk_AllocColorFromObj(interp, tkwin,
							colorPtr)) == NULL)
						{
							return TCL_ERROR;
						}
						r = ((double) xColorPtr->red / USHRT_MAX) * 255,
						g = ((double) xColorPtr->green / USHRT_MAX) * 255,
						b = ((double) xColorPtr->blue / USHRT_MAX) * 255;
						switch (g_pixel_size)
						{
							case 1:
#ifdef PLATFORM_X11
								*dstPtr++ = xColorPtr->pixel; /* only on X (?) */
#endif
#ifdef PLATFORM_WIN
								*dstPtr++ = Colormap_RGB2Index(r, g, b);
#endif
								break;
							case 2:
								SetPix16(dstPtr, r, g, b); /* use pixel directly (?) */
								dstPtr += 2;
								break;
							case 3:
							case 4:
								SetPix24(dstPtr, r, g, b);
								dstPtr += g_pixel_size;
								break;
						}
						Tk_FreeColor(xColorPtr);
					}
				}
				break;
			}
		}
		objPtr += 2;
		objC -= 2;
	}

	/* Required number of arguments */
	if (objC != 0)
	{
		/* Set the error */
		Tcl_WrongNumArgs(interp, objOffset, objv,
			(char *) "-inner inner -outer outer");

		/* Failure */
		return TCL_ERROR;
	}

	if (symbol.inner != -1) symbolPtr->inner = symbol.inner;
	if (symbol.outer != -1) symbolPtr->outer = symbol.outer;
	if (symbol.light != -1) symbolPtr->light = symbol.light;

	symbol_changed();

	return TCL_OK;
}

/* cget $name $option */
static int objcmd_symbol_cget(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	static cptr symbolOption[] = {"-inner", "-light", "-outer",
		NULL};
	t_symbol *symbolPtr;
	int index;

	/* Hack - ignore parameter */
	(void) objc;

	if (symbol_find(interp, objV[1], NULL, &symbolPtr, NULL) != TCL_OK)
	{
		return TCL_ERROR;
	}

    if (Tcl_GetIndexFromObj(interp, objV[2], (char **) symbolOption,
		(char *) "option", 0, &index) != TCL_OK)
	{
		return TCL_ERROR;
    }
	switch (index)
	{
		case 0: /* inner */
			Tcl_SetObjResult(interp, Tcl_NewIntObj(symbolPtr->inner));
			break;

		case 1: /* light */
			Tcl_SetObjResult(interp, Tcl_NewBooleanObj(symbolPtr->light));
			break;

		case 2: /* -outer */
			Tcl_SetObjResult(interp, Tcl_NewIntObj(symbolPtr->outer));
			break;
	}

	return TCL_OK;
}

/* configure $name -inner $inner -outer $outer */
static int objcmd_symbol_configure(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	t_symbol *symbolPtr;

	if (symbol_find(interp, objV[1], NULL, &symbolPtr, NULL) != TCL_OK)
	{
		return TCL_ERROR;
	}

	return (symbol_configure(interp, symbolPtr, objc, objv,
		infoCmd->depth + 2));
}

static void Pixel2RGB(unsigned char *p, int size, int *r, int *g, int *b)
{
	switch (size)
	{
		case 1:
		{
			unsigned char *rgb = Colormap_GetRGB();
			*r = rgb[*p * 3 + 0];
			*g = rgb[*p * 3 + 1];
			*b = rgb[*p * 3 + 2];
			break;
		}
		case 2:
		{
			GetPix16(p, r, g, b);
			break;
		}
		case 3:
		case 4:
		{
			*b = *p++;
			*g = *p++;
			*r = *p;
			break;
		}
	}
}

/* data $name $size */
static int objcmd_symbol_data(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	t_symbol *symbolPtr;
	int size, row, col;
	Tcl_Obj *listObjPtr, *rowPtr;
	IconPtr bits;
	
	/* Hack - ignore parameter */
	(void) objc;

	if (symbol_find(interp, objV[1], NULL, &symbolPtr, NULL) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj(interp, objV[2], &size) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if ((size < 4) || (size > 8))
	{
		/* REPORT */
		return TCL_ERROR;
	}
	bits = symbolPtr->bits[size - 4];

	listObjPtr = Tcl_NewListObj(0, NULL);
	for (row = 0; row < size; row++)
	{
		rowPtr = Tcl_NewListObj(0, NULL);
		for (col = 0; col < size; col++)
		{
			int r, g, b;
			char buf[24];
			Pixel2RGB(bits, g_pixel_size, &r, &g, &b);
			(void) sprintf(buf, "#%02x%02x%02x", r, g, b);
			Tcl_ListObjAppendElement(interp, rowPtr,
				Tcl_NewStringObj(buf, -1));
			bits += g_pixel_size;
		}
		Tcl_ListObjAppendElement(interp, listObjPtr, rowPtr);
	}
	Tcl_SetObjResult(interp, listObjPtr);

	return TCL_OK;
}

/* image2bits $imageFile $size $symbolList */
static int objcmd_symbol_image2bits(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	char *imageFile;
	cptr imageName = "MakeIconImage";
	char buf[1024];
	int i, length, numSymbols, size;
	int result = TCL_OK;
	Tcl_Obj *listObjPtr, *namePtr;
	Tk_PhotoHandle photoH;
	t_symbol *symbolPtr;
	t_icon_data iconData;
	
	/* Hack - ignore parameter */
	(void) objc;

	imageFile = Tcl_GetStringFromObj(objV[1], NULL);

	if (Tcl_GetIntFromObj(interp, objV[2], &size) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if ((size < 4) || (size > 8))
	{
		/* REPORT */
		return TCL_ERROR;
	}

	listObjPtr = objV[3];

	/* Read the image file */
	length = sprintf(buf, "image create photo %s -file \"%s\"",
		imageName, imageFile);
	if (Tcl_EvalEx(interp, buf, length, TCL_EVAL_GLOBAL) != TCL_OK)
	{
		return TCL_ERROR;
	}
	Tcl_ResetResult(interp);

	/* Lookup the photo by name */
	photoH = Tk_FindPhoto(interp, (char *) imageName);

	/* The photo was not found */
	if (photoH == NULL)
	{
		return TCL_ERROR;
	}

	/* Number of bytes-per-symbol */
	iconData.icon_data = NULL;
	iconData.depth = g_icon_depth;
	iconData.bypp = g_pixel_size;
	iconData.width = size;
	iconData.height = size;
	iconData.length = iconData.width * iconData.height * iconData.bypp;

	/* Convert 24-bit image to g_icon_depth-bit data */
	if (Image2Bits(interp, &iconData, photoH, size, size, NULL) != TCL_OK)
	{
		result = TCL_ERROR;
		goto cleanup;
	}

	/* Get the number of symbols names passed by the caller */
	if (Tcl_ListObjLength(interp, listObjPtr, &numSymbols) != TCL_OK)
	{
		result = TCL_ERROR;
		goto cleanup;
	}
	if (numSymbols > iconData.icon_count)
	{
		/* REPORT */
		result = TCL_ERROR;
		goto cleanup;
	}
	for (i = 0; i < numSymbols; i++)
	{
		IconPtr iconPtr = iconData.icon_data + i * iconData.length;

		if (Tcl_ListObjIndex(interp, listObjPtr, i, &namePtr) != TCL_OK)
		{
			result = TCL_ERROR;
			goto cleanup;
		}
		if (symbol_find(interp, namePtr, NULL, &symbolPtr, NULL) != TCL_OK)
		{
			/* Skip non-existent symbols, since .gif is shared by variants */
			Tcl_ResetResult(interp);
			continue;
		}
		if (g_pixel_size == 1)
		{
			int j;
			for (j = 0; j < iconData.length; j++)
			{
				symbolPtr->bits[size - 4][j] = g_palette2colormap[iconPtr[j]];
			}
		}
		else
		{
			(void) memcpy(symbolPtr->bits[size - 4], iconPtr, iconData.length);
		}
	}

cleanup:

	if (iconData.icon_data != NULL)
		Tcl_FreeDebug((char *) iconData.icon_data);

	/* Delete the image */
	length = sprintf(buf, "image delete %s", imageName);
	(void) Tcl_EvalEx(interp, buf, length, TCL_EVAL_GLOBAL);
	
	return result;
}

/* names */
static int objcmd_symbol_names(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
/*	CommandInfo *infoCmd = (CommandInfo *) clientData; */
/*	Tcl_Obj *CONST *objV = objv + infoCmd->depth; */
	int i;
	Tcl_Obj *listObjPtr;

	/* Hack - ignore parameters */
	(void) objc;
	(void) objv;
	(void) clientData;

	listObjPtr = Tcl_NewListObj(0, NULL);
	
	for (i = 0; i < g_symbol_count; i++)
	{
		Tcl_ListObjAppendElement(interp, listObjPtr,
			Tcl_NewStringObj(g_symbol[i]->name, -1));
	}

	Tcl_SetObjResult(interp, listObjPtr);

	return TCL_OK;
}

/* new $name -inner $inner -outer $outer */
static int objcmd_symbol_new(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	t_symbol symbol, *symbolPtr;
	Tcl_HashEntry *hPtr;
	int i, new;

	/* Get the name */
	symbol.name = Tcl_GetString(objV[1]);

	/* Add it to our name hash-table */
	hPtr = Tcl_CreateHashEntry(&g_symbol_hash, symbol.name, &new);

	/* The symbol already exists */
	if (!new)
	{
		/* Set the error */
		Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "symbol \"",
			symbol.name, "\" already exists", NULL);

		/* Failure */
		return TCL_ERROR;
	}

	/* Some defaults */
	symbol.inner = 255;
	symbol.outer = 255;
	symbol.light = 0;

	/* Add it to our list of symbols */
	g_symbol = Array_Insert(g_symbol, &g_symbol_count,
		sizeof(t_symbol *), g_symbol_count);
	symbolPtr = (t_symbol *) Tcl_AllocDebug(sizeof(t_symbol));
	g_symbol[g_symbol_count - 1] = symbolPtr;

	symbolPtr->name = (char *) string_make(symbol.name);
	symbolPtr->inner = symbol.inner;
	symbolPtr->outer = symbol.outer;
	symbolPtr->light = symbol.light;

	symbolPtr->bits[0] = symbolPtr->bits4;
	symbolPtr->bits[1] = symbolPtr->bits5;
	symbolPtr->bits[2] = symbolPtr->bits6;
	symbolPtr->bits[3] = symbolPtr->bits7;
	symbolPtr->bits[4] = symbolPtr->bits8;

	Tcl_SetHashValue(hPtr, (ClientData) (g_symbol_count - 1));

	for (i = 0; i < 5; i++)
	{
		g_bits[i] = Array_Insert(g_bits[i], &g_bits_count[i],
			sizeof(IconPtr *), g_bits_count[i]);
	}
	g_bits[0][g_bits_count[0] - 1] = symbolPtr->bits4;
	g_bits[1][g_bits_count[1] - 1] = symbolPtr->bits5;
	g_bits[2][g_bits_count[2] - 1] = symbolPtr->bits6;
	g_bits[3][g_bits_count[3] - 1] = symbolPtr->bits7;
	g_bits[4][g_bits_count[4] - 1] = symbolPtr->bits8;

	if (symbol_configure(interp, symbolPtr, objc, objv, infoCmd->depth + 2)
		!= TCL_OK)
	{
		/* Overkill: Free resources */
		Tcl_DeleteHashEntry(hPtr);
		for (i = 0; i < 5; i++)
		{
			g_bits[i] = Array_Delete(g_bits[i], &g_bits_count[i],
				sizeof(IconPtr *), g_bits_count[i] - 1);
		}
		g_symbol = Array_Delete(g_symbol, &g_symbol_count,
			sizeof(t_symbol *), g_symbol_count - 1);
		return TCL_ERROR;
	}

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

void DrawMapSymbol(Widget *widgetPtr, int y, int x, int symbol)
{
	int size = widgetPtr->gwidth;
	IconPtr *tilePtr;
	long *srcPtr, *dstPtr, pitch;

	tilePtr = g_bits[size - 4];
	pitch = widgetPtr->bitmap.pitch;
	srcPtr = (long *) tilePtr[symbol];
	dstPtr = (long *) (widgetPtr->bitmap.pixelPtr + x * size * g_pixel_size +
		y * size * pitch);

	symbolProcTable[g_pixel_size - 1][size - 4](srcPtr, dstPtr, pitch);
}

/* write $path */ 
static int objcmd_symbol_write(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	char *fileName, *extFileName;
	FILE *fp;
	s16b tmp16b;
	int i;
	Tcl_DString extDString;

	/* Hack - ignore parameter */
	(void) objc;

	/* Get the name of the file to read */
	fileName = Tcl_GetStringFromObj(objV[1], NULL);

	/* Convert to system format */
	extFileName = UtfToExt_TranslateFileName(interp, fileName, &extDString);

	/* Bad path */
	if (extFileName == NULL)
	{
		/* Note: Tcl_DStringFree() is called for us */
		return TCL_ERROR;
	}

	/* Try to open the file */
	if ((fp = fopen(extFileName, "wb")) == NULL)
	{
		Tcl_AppendResult(interp, "couldn't open file \"", fileName,
			"\": ", Tcl_PosixError(interp), NULL);
		return TCL_ERROR;
	}

	/* Write number of symbols */
	tmp16b = g_symbol_count;
	(void) fwrite(&tmp16b, 1, 2, fp);

	/* Write each symbol data */
	for (i = 0; i < g_symbol_count; i++)
	{
		t_symbol *symbolPtr = g_symbol[i];
		int size;
		char *t = symbolPtr->name;

		(void) fwrite(t, 1, strlen(t) + 1, fp);
		for (size = 4; size <= 8; size++)
		{
			if (g_pixel_size == 1)
			{
				IconValue data[64];
				int j;
				for (j = 0; j < size * size; j++)
				{
					data[j] = g_colormap2palette[symbolPtr->bits[size-4][j]];
				}
				(void) fwrite(data, 1, size * size, fp);
			}
			else
			{
				(void) fwrite(symbolPtr->bits[size - 4], 1,
					size * size * g_pixel_size, fp);
			}
		}
	}

	fclose(fp);

	/* Clean up */
	Tcl_DStringFree(&extDString);

	return TCL_OK;
}

/* read $path */ 
static int objcmd_symbol_read(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	char *fileName, *extFileName;
	FILE *fp;
	s16b count;
	int i;
	Tcl_DString extDString;

	/* Hack - ignore parameter */
	(void) objc;

	/* Get the name of the file to read */
	fileName = Tcl_GetStringFromObj(objV[1], NULL);

	/* Convert to system format */
	extFileName = UtfToExt_TranslateFileName(interp, fileName, &extDString);

	/* Bad path */
	if (extFileName == NULL)
	{
		/* Note: Tcl_DStringFree() is called for us */
		return TCL_ERROR;
	}

	/* Try to open the file */
	if ((fp = fopen(extFileName, "rb")) == NULL)
	{
		Tcl_AppendResult(interp, "couldn't open file \"", fileName,
			"\": ", Tcl_PosixError(interp), NULL);
		return TCL_ERROR;
	}

	/* Read number of symbols */
	(void) fread(&count, 1, 2, fp);

	for (i = 0; i < count; i++)
	{
		char name[80];
		int c, n = 0, size;
		t_symbol *symbolPtr;

		while ((c = fgetc(fp)) != 0) name[n++] = c;
		name[n++] = '\0';
		if (symbol_find(interp, NULL, name, &symbolPtr, NULL) != TCL_OK)
		{
			goto error;
		}
		for (size = 4; size <= 8; size++)
		{
			/* Read the symbol data */
			(void) fread(symbolPtr->bits[size - 4], 1,
				size * size * g_pixel_size, fp);

			/* Convert from palette to colormap colors on 8-bit display */
			if (g_pixel_size == 1)
			{
				int j;
				for (j = 0; j < size * size; j++)
				{
					symbolPtr->bits[size - 4][j] =
						g_palette2colormap[symbolPtr->bits[size - 4][j]];
				}
			}
		}
	}

	fclose(fp);

	return TCL_OK;

error:
	fclose(fp);

	return TCL_ERROR;
}

static CommandInit commandInit[] = {
	{0, "symbol", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "cget", 3, 3, "name option", objcmd_symbol_cget, (ClientData) 0},
		{1, "configure", 4, 0, "symbol option value ?option value ...?", objcmd_symbol_configure, (ClientData) 0},
		{1, "data", 3, 3, "name size", objcmd_symbol_data, (ClientData) 0},
		{1, "image2bits", 4, 4, "imageName size symbolList", objcmd_symbol_image2bits, (ClientData) 0},
		{1, "names", 1, 1, NULL, objcmd_symbol_names, (ClientData) 0},
		{1, "new", 2, 0, "name ?option value ...?", objcmd_symbol_new, (ClientData) 0},
		{1, "read", 2, 2, "path", objcmd_symbol_read, (ClientData) 0},
		{1, "write", 2, 2, "path", objcmd_symbol_write, (ClientData) 0},
	{0, NULL, 0, 0, NULL, NULL, (ClientData) 0}
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

	(void) CommandInfo_Init(interp, commandInit, NULL);

	return TCL_OK;
}
