/* File: icon.h */

/* Purpose: icon environment definitions */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef _INCLUDE_ICON_H_
#define _INCLUDE_ICON_H_

typedef struct IconSpec {
	int type;
	int index;
	int ascii;
} IconSpec;

extern int g_icon_depth; /* 8, 16 or 24 */
extern long g_icon_length;

#define ICON_LENGTH_16 256L
#define ICON_LENGTH_24 576L
#define ICON_LENGTH_32 1024L
#define ICON_LENGTH_MAX (ICON_LENGTH_32 * 4)
#define ICON_LENGTH g_icon_length

typedef byte IconData[ICON_LENGTH_MAX];
typedef byte *IconPtr;

typedef union PixelPtr
{
	byte *pix8;
	u16b *pix16;
	u32b *pix24;
} PixelPtr;

typedef struct t_icon_data {
	cptr desc; /* type name */
	IconPtr icon_data; /* Address of icon data */
	int icon_count; /* Number of icons */
	Tk_Font font; /* Font for ascii icons */
	int *char_table; /* Table of chars for ascii icons */

	IconPtr rle_data; /* Address of RLE data */
	long *rle_offset; /* Per-icon offset into RLE data */
	int *rle_len; /* Per-icon length of RLE data */
	byte *rle_bounds; /* Per-icon x, y, width, height */
	int rle_pixel; /* Transparent pixel (must be 4 bytes) */
	int depth; /* Bits per pixel (8, 16, 24) */
	int bypp; /* Bytes per pixel (1, 2, 3 or 4) */
	int width; /* Pixels per column */
	int height; /* Pixels per row */
	int pitch; /* Convenience: width * bypp */
	int length; /* Convenience: width * height * bypp */
	int pixels; /* Convenience: width * height */
} t_icon_data;

extern t_icon_data *g_icon_data; /* Array of icon types */
extern int g_icon_data_count; /* Number of icon types */

typedef struct t_ascii {
	int color[2]; /* Foreground/Background palette index */
} t_ascii;

extern int g_ascii_count; /* Number of elements in g_ascii[] array */

extern void PixelSet_RGB(IconPtr dst, int r, int g, int b, int bypp);
extern void RL_Bounds(int w, int h, int bypp, IconPtr srcbuf, int key, byte *bounds);
extern int RL_Len(int w, int h, int bypp, IconPtr srcbuf, int pitch, int key);
extern int RL_Encode(int w, int h, int bypp, IconPtr srcbuf, int pitch, int key, IconPtr rlebuf);
extern int RL_Decode(int w, int h, int bypp, IconPtr rlebuf, IconPtr dst, int pitch);
extern void Icon_MakeRLEBounds(t_icon_data *iconDataPtr, int i);
extern void Icon_MakeRLE(t_icon_data *iconDataPtr);

extern int Image2Bits(Tcl_Interp *interp, t_icon_data *iconDataPtr,
	Tk_PhotoHandle photoH, int imageW, int imageH, XColor *xColorPtr);

extern void Icon_AddType(t_icon_data *data);
extern int Icon_Init(Tcl_Interp *interp, int size, int depth);
extern int Icon_Validate(Tcl_Interp *interp, char *typeName, int index, int ascii,
	IconSpec *specPtr);
extern int Icon_ParseArgs(Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[],
	int offset, IconSpec *spectr);
extern IconPtr Icon_GetAsciiData(IconSpec *specPtr, IconPtr dstPtr);
extern int Icon_FindTypeByName(Tcl_Interp *interp, int *typeIndexPtr,
	char *typeName);
extern int Icon_GetTypeFromObj(Tcl_Interp *interp,
	t_icon_data **typePtrPtr, Tcl_Obj *objPtr);
extern int Icon_GetIndexFromObj(Tcl_Interp *interp,
	int *indexPtr, Tcl_Obj *objPtr, t_icon_data *iconDataPtr);
extern void Icon_MakeDark(t_icon_data *iconDataPtr, int index);


/*
 * Constants for g_icon_data[] index.
 * icon_type.type constants.
 */

/* Special type "none". It's a transparent icon. */
#define ICON_TYPE_NONE 0

#define ICON_TYPE_BLANK 1
#define ICON_TYPE_DEFAULT 2

/*
 * Constants for g_assign[] index.
 */
#define ASSIGN_MONSTER 0
#define ASSIGN_OBJECT 1
#define ASSIGN_CHARACTER 2
#define ASSIGN_FEATURE 3
#define ASSIGN_SHOPKEEPER 4
#define ASSIGN_ARTIFACT 5
#define ASSIGN_MAX 6

/* One assigned icon */
typedef struct t_assign_icon {
	int type;
	int index;
	int ascii;
} t_assign_icon;


/*
 * Icon assignment for each member of each group (monster,
 * object, features, etc)
 */
typedef struct t_assign_group {
	int count; /* Number of elements in array */
	t_assign_icon *assign; /* Array of iassignments */
} t_assign_group;

/* Layers of feature icons */
enum {
ICON_LAYER_1,
ICON_LAYER_2,
ICON_LAYER_3,
ICON_LAYER_4,
ICON_LAYER_MAX
};

/* Per-layer assignment for each cave location */
extern t_assign_icon *g_icon_map[ICON_LAYER_MAX][MAX_HGT];

/*
 * Information about what to display.
 */
typedef struct t_display {
	bool blank; /* Totally uninteresting grid */
	IconSpec fg; /* Foreground */
	IconSpec bg[ICON_LAYER_MAX]; /* Background (when typeFG is masked) */
} t_display;


/* Recalculate g_icon_map[] */
extern bool g_icon_map_changed;

/* t_grid -> t_display */
extern void get_display_info(int y, int x, t_display *displayPtr);

extern IconPtr SetIconBits(IconPtr bg, IconPtr fg, IconPtr mk, TintTable t,
	IconPtr b);

extern byte *g_palette_rgb;

extern void FinalIcon(IconSpec *iconOut, t_assign_icon *assignPtr, int hack, object_type *o_ptr);
extern int assign_parse(Tcl_Interp *interp, t_assign_icon *assignPtr, cptr desc);
extern char *AssignToString_Icon(char *buf, t_assign_icon *assign);
extern char *assign_print2(char *buf, int assignType);
extern char *assign_print_object(char *buf, object_type *o_ptr);

#endif /* _INCLUDE_ICON_H_ */
