/* File: icon-dll.h */

/* Purpose: icon stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef _INCLUDE_ICONDLL_H_
#define _INCLUDE_ICONDLL_H_

#include <tk.h>

typedef struct IconSpec {
	int type;
	int index;
	int ascii;
	int dark;
} IconSpec;

long g_icon_length; /* size x size x pixel_size */
int g_icon_size; /* 16, 24 or 32 */
int g_icon_depth; /* 8, 16 or 24 */
int g_icon_pixels; /* size x size */
int g_pixel_size; /* 1, 2, 3 or 4 */

#define ICON_LENGTH_16 256L
#define ICON_LENGTH_24 576L
#define ICON_LENGTH_32 1024L
#define ICON_LENGTH_MAX (ICON_LENGTH_32 * 4)
#define ICON_LENGTH g_icon_length
#define ICON_PIXELS g_icon_pixels

typedef unsigned char IconValue;
typedef unsigned char IconData[ICON_LENGTH_MAX];
typedef unsigned char *IconPtr;

typedef union PixelPtr
{
	unsigned char *pix8;
	unsigned short *pix16;
	unsigned long *pix24;
} PixelPtr;

typedef struct t_icon_data {
	char *desc; /* type name */
	IconPtr icon_data; /* Address of icon data */
	int icon_count; /* Number of icons */
	Tk_Font font; /* Font for ascii icons */
	int *char_table; /* Table of chars for ascii icons */
	int dynamic; /* rle_data is array of ptrs to malloc()'d data */
	IconSpec *dynamic_spec; /* List of dynamic components */
	IconPtr *dark_data; /* Darkened copies */
	unsigned char *gamma[2]; /* Per-icon gamma for dark_data. Value 0-200 */

#define ICON_FLAG_DARK 0x0001 /* Recalculate dark_data before display */
#define ICON_FLAG_LEFT 0x0002 /* back2front */
#define ICON_FLAG_RIGHT 0x0004 /* back2front */
	short *flags; /* Per-icon flags */

	IconPtr rle_data; /* Address of RLE data */
	long *rle_offset; /* Per-icon offset into RLE data */
	int *rle_len; /* Per-icon length of RLE data */
	unsigned char *rle_bounds; /* Per-icon x, y, width, height */
	int rle_pixel; /* Transparent pixel (must be 4 bytes) */
	int depth; /* Bits per pixel (8, 16, 24) */
	int bypp; /* Bytes per pixel (1, 2, 3 or 4) */
	int width; /* Pixels per column */
	int height; /* Pixels per row */
	int pitch; /* Convenience: width * bypp */
	int length; /* Convenience: width * height * bypp */
	int pixels; /* Convenience: width * height */
} t_icon_data;

t_icon_data *g_icon_data; /* Array of icon types */
int g_icon_data_count; /* Number of icon types */
Tcl_HashTable g_icon_table; /* Hash table for icon types */

#define ASCII_NORMAL 0 /* Colored char */
#define ASCII_ATTR_MULTI 1 /* Animate color */
#define ASCII_SHAPECHANGER 2 /* Animate character */

typedef struct t_ascii {
	int color[2]; /* Foreground/Background palette index */
	int mode; /* Special behaviour, ASCII_XXX flags */
} t_ascii;

t_ascii *g_ascii; /* Array of "ascii" icon info */
int g_ascii_count; /* Number of elements in g_ascii[] array */
int g_ascii_delay, g_ascii_multi, g_ascii_ticks, g_ascii_char;

#define ICON_STYLE_ICON 0
int g_icon_style;

void PixelSet_RGB(IconPtr dst, int r, int g, int b, int bypp);
void RL_Bounds(int w, int h, int bypp, IconPtr srcbuf, int key, unsigned char *bounds);
int RL_Len(int w, int h, int bypp, IconPtr srcbuf, int pitch, int key);
int RL_Encode(int w, int h, int bypp, IconPtr srcbuf, int pitch, int key, IconPtr rlebuf);
int RL_Decode(int w, int h, int bypp, IconPtr rlebuf, IconPtr dst, int pitch);
void Icon_MakeRLEBounds(t_icon_data *iconDataPtr, int i);
void Icon_MakeRLE(t_icon_data *iconDataPtr);

int Image2Bits(Tcl_Interp *interp, t_icon_data *iconDataPtr,
	Tk_PhotoHandle photoH, int imageW, int imageH, XColor *xColorPtr);

void Icon_AddType(t_icon_data *data);
int Icon_Init(Tcl_Interp *interp, int size, int depth);
void Icon_Exit(Tcl_Interp *interp);
int Icon_Validate(Tcl_Interp *interp, char *typeName, int index, int ascii,
	IconSpec *specPtr);
int Icon_ParseArgs(Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[],
	int offset, IconSpec *spectr);
IconPtr Icon_GetAsciiData(IconSpec *specPtr, IconPtr dstPtr);
int Icon_FindTypeByName(Tcl_Interp *interp, int *typeIndexPtr,
	char *typeName);
int Icon_GetTypeFromObj(Tcl_Interp *interp,
	t_icon_data **typePtrPtr, Tcl_Obj *objPtr);
int Icon_GetIndexFromObj(Tcl_Interp *interp,
	int *indexPtr, Tcl_Obj *objPtr, t_icon_data *iconDataPtr);
void Icon_MakeDark(t_icon_data *iconDataPtr, int index);

#endif /* _INCLUDE_ICONDLL_H_ */

