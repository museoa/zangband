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
	int dark;
} IconSpec;

extern long g_icon_length; /* size x size x pixel_size */
extern int g_icon_size; /* 16, 24 or 32 */
extern int g_icon_depth; /* 8, 16 or 24 */
extern int g_icon_pixels; /* size x size */
extern int g_pixel_size; /* 1, 2, 3 or 4 */

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
	cptr desc; /* type name */
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

extern t_icon_data *g_icon_data; /* Array of icon types */
extern int g_icon_data_count; /* Number of icon types */
extern Tcl_HashTable g_icon_table; /* Hash table for icon types */

typedef struct t_ascii {
	int color[2]; /* Foreground/Background palette index */
} t_ascii;

extern t_ascii *g_ascii; /* Array of "ascii" icon info */
extern int g_ascii_count; /* Number of elements in g_ascii[] array */

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

/*
 * Constants for t_assign.assignType.
 */
#define ASSIGN_TYPE_ALTERNATE 0
#define ASSIGN_TYPE_FLAVOR 1
#define ASSIGN_TYPE_ICON 2
#define ASSIGN_TYPE_MAX 3

/* One assigned alternate */
typedef struct t_assign_alternate {
	int assignType; /* Required field */
	int index;
} t_assign_alternate;

/* One assigned flavor */
typedef struct t_assign_flavor {
	int assignType; /* Required field */
	int group;
	int index;
} t_assign_flavor;

/* One assigned icon */
typedef struct t_assign_icon {
	int assignType; /* Required field */
	int type;
	int index;
	int ascii;
} t_assign_icon;

/* One assignment */
typedef union t_assign {
	int assignType;
	t_assign_alternate alternate;
	t_assign_flavor flavor;
	t_assign_icon icon;
} t_assign;

/*
 * Icon assignment for each member of each group (monster,
 * object, features, etc)
 */
typedef struct t_assign_group {
	int count; /* Number of elements in array */
	t_assign *assign; /* Array of iassignments */
} t_assign_group;

extern t_assign_group g_assign[ASSIGN_MAX];
extern t_assign g_assign_none;

/* Constants for g_feat_lite[] */
#define FT_LIGHT_NONE 0 /* Always use 1 icon, no tinting */
#define FT_LIGHT_ICON 1 /* Use 3 icons, no tinting */
#define FT_LIGHT_TINT 2 /* Use 1 icon, with tinting as needed */

extern int *g_feat_lite;

/*
 * When TRUE, uses 4 levels of grid light. The icon set must
 * support 4 levels for this to work.
 */
extern int g_torchlite;

extern int *g_image_object;

/* Feature index for masked features */
extern int *g_background;

/* One icon for each feature, for each shape */
extern cptr keyword_wall[];

/* Layers of feature icons */
enum {
ICON_LAYER_1,
ICON_LAYER_2,
ICON_LAYER_3,
ICON_LAYER_4,
ICON_LAYER_MAX
};

/* Per-layer assignment for each cave location */
extern t_assign *g_icon_map[ICON_LAYER_MAX][MAX_HGT];

/*
 * Information about what to display.
 */
typedef struct t_display {
	bool blank; /* Totally uninteresting grid */
	TintPtr tint; /* Tint to apply, or NULL */
	IconSpec fg; /* Foreground */
	IconSpec bg[ICON_LAYER_MAX]; /* Background (when typeFG is masked) */
} t_display;

/*
 * TYPE_FLAVOR refers to one of the predefined flavor types, which
 * are themselves defined by the structure below.
 */
typedef struct t_flavor {
	char *desc; /* Keyword for this flavor (potion, ring, mushroom) */
	int tval; /* Object tval flavor applies to */
	int count; /* Number of assignments */
	int *sorted; /* Map unsorted index to assignment index */
	IconSpec *icon; /* Assignments */
	byte *color; /* TERM_XXX colors */
} t_flavor;

extern t_flavor *g_flavor; /* Array of flavor types */
extern int g_flavor_count; /* Number of flavors */
extern Tcl_HashTable g_flavor_table; /* Map flavor name to g_flavor[] index */


/*
 * Constants for t_alternate.reason.
 */
#define REASON_NONE			0	/* No reason */
#define REASON_NUMBER		1	/* Use "second" if only one object in stack */
#define REASON_IDENT		2	/* Use "second" if object identified */
#define REASON_FEATURE 3 /* Use "second" icon for door/pillar */

typedef struct t_alternate {
	int reason; /* Reason code */
	int count; /* Number of icons */
	IconSpec *icon; /* The icons */
} t_alternate;

extern t_alternate *g_alternate; /* Array of "alternate" info */
extern int g_alternate_count; /* Number of elements in g_alternate[] array */

/* g_effect[EFFECT_SPELL_BALL/BOLT].icon index */
enum {
EFFECT_SPELL_ARROW,
EFFECT_SPELL_MISSILE,
EFFECT_SPELL_MANA,
EFFECT_SPELL_LITE_WEAK,
EFFECT_SPELL_DARK_WEAK,
EFFECT_SPELL_WATER,
EFFECT_SPELL_PLASMA,
EFFECT_SPELL_METEOR,
EFFECT_SPELL_ICE,
EFFECT_SPELL_GRAVITY,
EFFECT_SPELL_INERTIA,
EFFECT_SPELL_FORCE,
EFFECT_SPELL_TIME,
EFFECT_SPELL_ACID,
EFFECT_SPELL_ELEC,
EFFECT_SPELL_FIRE,
EFFECT_SPELL_COLD,
EFFECT_SPELL_POIS,
EFFECT_SPELL_LITE,
EFFECT_SPELL_DARK,
EFFECT_SPELL_CONFUSION,
EFFECT_SPELL_SOUND,
EFFECT_SPELL_SHARD,
EFFECT_SPELL_NEXUS,
EFFECT_SPELL_NETHER,
EFFECT_SPELL_CHAOS,
EFFECT_SPELL_DISENCHANT,
EFFECT_SPELL_ROCKET,
EFFECT_SPELL_NUKE,
EFFECT_SPELL_DEATH_RAY,
EFFECT_SPELL_HOLY_FIRE,
EFFECT_SPELL_HELL_FIRE,
EFFECT_SPELL_DISINTEGRATE,
EFFECT_SPELL_PSI,
EFFECT_SPELL_PSI_DRAIN,
EFFECT_SPELL_TELEKINESIS,
EFFECT_SPELL_DOMINATION,
EFFECT_SPELL_MAX
};

/* g_effect[EFFECT_AMMO].icon index */
enum {
EFFECT_AMMO_ARROW,
EFFECT_AMMO_BOLT,
EFFECT_AMMO_MAX
};

/* g_effect[] index */
#define EFFECT_SPELL_BALL 0
#define EFFECT_SPELL_BOLT 1
#define EFFECT_AMMO 2
#define EFFECT_MAX 3

typedef struct t_effect {
	char **name; /* Keyword for each icon */
	IconSpec *icon; /*  */
} t_effect;

extern t_effect *g_effect; /* Array of "effect" info */
extern cptr keyword_effect_spell[];
extern cptr keyword_effect_ammo[];

typedef struct t_darken {
	TintTable table; /* Tint table for darkening feature icons */
	int brightness;
	int contrast;
	double gamma;
} t_darken;
extern t_darken g_darken[3];

/* Recalculate g_icon_map[] */
extern bool g_icon_map_changed;

extern void init_icons(int size, int depth);

/* t_grid -> t_display */
extern void get_display_info(int y, int x, t_display *displayPtr);

extern IconPtr SetIconBits(IconPtr bg, IconPtr fg, IconPtr mk, TintTable t,
	IconPtr b);

extern unsigned char *g_palette_rgb;

extern int *g_image_monster;

extern void FinalIcon(IconSpec *iconOut, t_assign *assignPtr, int hack, object_type *o_ptr);
extern int assign_parse(Tcl_Interp *interp, t_assign *assignPtr, char *desc);
extern char *assign_print(char *buf, t_assign *assignPtr);
extern char *assign_print2(char *buf, int assignType, int assignIndex);
extern char *assign_print_object(char *buf, object_type *o_ptr);
extern void get_object_assign(t_assign *assignPtr, object_type *o_ptr);

extern int objcmd_vault _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));
extern int vault_icon(int y, int x, bool test_feat, t_assign assign[ICON_LAYER_MAX]);

#endif /* _INCLUDE_ICON_H_ */
