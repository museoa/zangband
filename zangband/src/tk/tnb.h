/* File: tnb.h */

/* Purpose: AngbandTk header */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef _INCLUDE_TNB_H_
#define _INCLUDE_TNB_H_

#include <tk.h>
#include <tcl.h>
#include "../angband.h"

#if !defined(PLATFORM_WIN) && !defined(PLATFORM_X11)
#error "You must define one of PLATFORM_WIN or PLATFORM_X11"
#endif /* */

#if (TK_MINOR_VERSION != 3)
#error "(1) You must use tcl/tk version 8.3.3"
#endif /* */
#if (TK_RELEASE_SERIAL < 3)
#error "(2) You must use tcl/tk version 8.3.3"
#endif /* */


/* Hack - prevent warnings from tk headers */
#if defined errno
#	undef errno
#	define errno errno_hack
#endif /* errno */

#ifdef PLATFORM_WIN
#	include <windows.h>
#	include <tkWinInt.h>
#endif /* PLATFORM_WIN */

#ifdef PLATFORM_X11
#	define HAVE_LIMITS_H
#	define HAVE_UNISTD_H

/*
 * Hack - prevent a huge number of compiler warnings when <tclInt.h
 * is included indirectly below.
 */
#	define _TCLINTDECLS
#	include <tkInt.h>
#endif /* PLATFORM_X11 */




/* main-tnb.c */
extern bool g_initialized;
extern bool game_in_progress;
extern cptr ANGBAND_DIR_TK;

extern Tcl_Interp *g_interp;

/* canv-widget.c */
extern void CanvasWidget_Idle(void);
extern int CanvasWidget_Init(Tcl_Interp *interp);

/* const.c */
extern int init_const(Tcl_Interp *interp);


/* cmdinfo.c */
typedef struct CommandInfo CommandInfo;
typedef struct CommandInit CommandInit;
typedef struct SubCommandInfo SubCommandInfo;

struct SubCommandInfo {
	int alloc;
	int count;
	char **name;
	CommandInfo **info;
};

struct CommandInfo {
	int depth;
	cptr name;
	int minArgs, maxArgs;
	cptr errorMsg;
	Tcl_ObjCmdProc *proc;
	ClientData clientData;
	SubCommandInfo subCmd;
};

struct CommandInit {
	int depth;
	cptr name;
	int minArgs, maxArgs;
	cptr errorMsg;
	Tcl_ObjCmdProc *proc;
	ClientData clientData;
};

void CommandInfo_Add(CommandInfo *infoCmd, CommandInfo *infoSubCmd);
CommandInfo *CommandInfo_GetInfo(Tcl_Interp *interp, char *names[]);
extern int CommandInfo_InitAux(Tcl_Interp *interp, CommandInit *init,
	int index, CommandInfo *parent);
int CommandInfo_Init(Tcl_Interp *interp, CommandInit *init,
	CommandInfo *parent);
CommandInfo *CommandInfo_New(CommandInit *init);
extern int CommandInfo_ObjCmd(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

/* icon1.c */

#define FLAVOR_AMULET 0
#define FLAVOR_MUSHROOM 1
#define FLAVOR_POTION 2
#define FLAVOR_RING 3
#define FLAVOR_ROD 4
#define FLAVOR_STAFF 5
#define FLAVOR_WAND 6
#define FLAVOR_MAX 7

extern void angtk_flavor_init(int *max, byte **attr);
extern void angtk_flavor_swap(int n, int a, int b);

/* Constants for t_grid.dark */
#define GRID_LITE_TORCH 0
#define GRID_LITE_NORMAL 1
#define GRID_LITE_DARK 2


/*
 * Memory of what is seen at a cave location for the entire cave.
 */
typedef struct t_grid {
	int dark; /* GRID_LITE_XXX flags */
	int f_idx; /* Feature */
	object_type *o_ptr; /* Object */
	int m_idx; /* Character/Monster */
} t_grid;

extern t_grid *g_grid[MAX_HGT];

extern bool g_daytime; /* Day or night */

/* Cave location -> t_grid */
extern void get_grid_info(int y, int x, t_grid *gridPtr);

extern void angtk_image_reset(void);
extern void angtk_view_floor(int y, int x, int info, int torch);
extern void angtk_view_wall(int y, int x, int info, int torch);
extern void set_grid_assign(int y, int x);
extern void free_icons(void);
extern void init_palette(void);

/* widget.c */
extern void angtk_widget_lock(bool lock);
extern void angtk_effect_clear(int y, int x);
extern bool angtk_effect_spell(int y, int x, int typ, int bolt);
extern bool angtk_effect_ammo(int y, int x, object_type *o_ptr, int dir);
extern bool angtk_effect_object(int y, int x, object_type *o_ptr);
extern void (*angtk_lite_spot)(int y, int x);
extern void angtk_lite_spot_real(int y, int x);
extern void angtk_wipe_spot(int y, int x);
extern void angtk_idle(void);

/* interp1.c */
typedef struct {
	cptr key; /* Textual name of TVAL_XXX macro */
	int value; /* TVAL_XXX constant */
} t_tval;
extern t_tval g_tval[];
extern Tcl_HashTable *g_tval_str;
extern Tcl_HashTable *g_tval_const;

extern cptr *keyword_gender;
extern cptr *keyword_race;
extern cptr *keyword_class;

/* Like object_attr, by k_info[] index */
#define kind_attr(k_idx) \
	((k_info[k_idx].flavor) ? \
	 (misc_to_attr[k_info[k_idx].flavor]) : \
	 (k_info[k_idx].x_attr))

extern cptr g_attr_str;
extern int exit_skip_save;
extern bool command_repeating;
extern void angtk_angband_initialized(void);
extern void angtk_display_info_init(void);
extern void angtk_display_info_append(cptr s);
extern void angtk_display_info_done(cptr title);
extern void angtk_display_info(char *title, char **info, int count);
extern void angtk_display_info_aux(char *title, Tcl_Obj *listObjPtr);
extern void angtk_eval(cptr command, ...);
extern int angtk_eval_file(cptr extFileName);
extern void angtk_cave_generated(void);
extern void angtk_health(char *buf);
extern int angtk_tval_string(cptr *str, int tval);
extern int angtk_tval_const(int *tval, cptr str);
extern void angtk_init(void);
extern bool player_test_feature(int y, int x, int mode);
extern cptr player_status(int status, int *value);

extern void blows_per_round(int *_blows, int *_muta_att);
extern void shots_per_round(int *shots, int *shots_frac);

#define TARGET_STATE_SET 0x0001 /* Target is set */
#define TARGET_STATE_VIS 0x0002 /* Target is visible */
#define TARGET_STATE_CHANGE 0x0004 /* Target changed */
extern int target_state;
extern bool target_vis;

#define PR_POSITION 0x20000000L /* p_ptr->redraw: player position changed */
#define PR_TARGET 0x40000000L /* p_ptr->redraw: target visibility changed */

/*
 * XXXXX Important!
 * If you add INKEY_XXX flags here, you must update the inkey_to_str[]
 * array in interp2.c, and keyword_inkey[] in bind.c
 */
 
#define INKEY_CMD 1
#define INKEY_DIR 2
#define INKEY_DISTURB 3
#define INKEY_ITEM 4
#define INKEY_ITEM_STORE 5
#define INKEY_MORE 6
#define INKEY_SPELL 7
#define INKEY_TARGET 8
#define INKEY_MINDCRAFT 9
#define INKEY_POWER 10
#define INKEY_CMD_PET 11

extern int inkey_book;
extern int inkey_flags;

/* r_info.c */
extern long angtk_roff(int r_idx, char *buffer);

/* birth-tnb.c */
extern void init_birth(void);

/* describe.c */
extern cptr keyword_slot[];
extern int strcpy_len(char *s1, const char *s2);
extern long angtk_describe_object(object_type *o_ptr, char *buf, bool in_store);
extern int SetArrayValueChar(cptr varName, cptr field, char value);
extern int SetArrayValueLong(cptr varName, cptr field, long value);
extern int SetArrayValueString(cptr varName, cptr field, cptr value);

/* map.c */
extern void init_map(void);
extern void map_symbol_set(int y, int x);
extern char *map_symbol_name(int symbolIndex);
extern int map_symbol_feature(int f_idx);
extern int map_symbol_find(Tcl_Interp *interp, Tcl_Obj *objName,
	int *symbolIndex);

/* struct.c */
extern void init_struct(void);

/* tcltk.c */
Tcl_Interp *TclTk_Init(int argc, char **argv);
void TclTk_Exit(Tcl_Interp *interp);

/* util-tnb.c */
extern cptr keyword_term_color[];
extern byte g_prompt_attr;
extern void prompt_print(cptr str);
extern void prompt_erase(void);
extern void prompt_format(cptr fmt, ...);
extern void prompt_append(cptr str);
extern void prompt_open(cptr str);
extern void prompt_update(cptr str);
extern void any_more(cptr prompt);

extern int ExtToUtf_SetArrayValueString(char *varName, char *field, char *value);

/* util-dll.c */
extern int g_palette_white, g_palette_black;
#define PALETTE_WHITE g_palette_white
#define PALETTE_BLACK g_palette_black

extern int g_colormap_white, g_colormap_black;
#define COLORMAP_WHITE g_colormap_white
#define COLORMAP_BLACK g_colormap_black

extern unsigned char g_palette2colormap[256];
extern unsigned char g_colormap2palette[256];

extern int gamma_correct(int value, double gamma);

typedef unsigned char TintValue, TintTable[256], *TintPtr;
int Palette_Init(Tcl_Interp *interp, char *fileName);
#ifdef PLATFORM_WIN
/* HPALETTE */ void *Palette_GetHPal(void);
#endif /* PLATFORM_WIN */
unsigned char *Palette_GetRGB(void);
void Palette_ResetHash(void);
int Palette_RGB2Index(unsigned char r, unsigned char g, unsigned char b);
void Palette_GammaTable(double gamma, TintTable table);
void Palette_TintTable(int tint, int opacity, TintTable table);

int Colormap_Init(Tcl_Interp *interp);
unsigned char *Colormap_GetRGB(void);
int Colormap_RGB2Index(unsigned char r, unsigned char g, unsigned char b);
void Colormap_GammaTable(double gamma, TintTable table);
void Colormap_TintTable(int tint, int opacity, TintTable table);
void Colormap_BrightnessTable(int intensity, TintTable table);
void Colormap_ContrastTable(int intensity, TintTable table);
void Colormap_One2OneTable(TintTable table);

typedef struct RGBInfo RGBInfo;
struct RGBInfo
{
	int red_count, green_count, blue_count;
	int red_mask, green_mask, blue_mask;
	int red_shift, green_shift, blue_shift;
	int extra;
};
extern RGBInfo g_rgbi;

void GetPix16(unsigned char *p, int *r, int *g, int *b);
void SetPix16(unsigned char *p, int r, int g, int b);

void *Array_New(int count, int elem_size);
void *Array_Append(void *array_ptr, int *count, int elem_size,
	void *elem_ptr);
void *Array_Insert(void *array_ptr, int *count, int elem_size,
	int index);
void *Array_Delete(void *array_ptr, int *count, int elem_size,
	int index);

typedef struct BitmapType {
	unsigned char *pixelPtr; /* Address of top-left pixel */
	int width; /* Width in pixels */
	int height; /* Height in pixels */
	int depth; /* 8, 16 or 24 */
	int pitch; /* Address difference between vertically adjacent pixels */
	int pixelSize; /* Address difference between horizontally adjacent pixels */
    Pixmap pixmap;
    void *platData; /* Platform-specific info */
} BitmapType, *BitmapPtr;

void Bitmap_New(Tcl_Interp *interp, BitmapPtr bitmapPtr);
void Bitmap_Delete(BitmapPtr bitmapPtr);

#ifdef PLATFORM_WINxxx
HFONT TkToWinFont(Tk_Font tkFont);
#endif /* PLATFORM_WIN */

typedef struct DoubleLink DoubleLink;
typedef struct DoubleLinker DoubleLinker;

struct DoubleLink
{
	int isLinked;
	DoubleLinker *linker;
	DoubleLink *prev;
	DoubleLink *next;
	void *data;
};

struct DoubleLinker
{
	int count;
	cptr what;
	DoubleLink *head;
	DoubleLink *tail;
};

#define DoubleLink_Data(link,type) ((type *) link->data)
void DoubleLink_Init(DoubleLinker *linker, DoubleLink *link, void *data);
void DoubleLink_Link(DoubleLink *link);
void DoubleLink_Unlink(DoubleLink *link);
/* void DoubleLink_Move(DoubleLink *old, DoubleLink *new); */

Tcl_Obj *ExtToUtf_NewStringObj(CONST char *bytes, int length);
void ExtToUtf_SetResult(Tcl_Interp *interp, char *string);
char *UtfToExt_TranslateFileName(Tcl_Interp *interp, char *utfPath,
	Tcl_DString *extDStringPtr);


/* plat.c */
extern void *Plat_PaletteInit(unsigned char *rgb);
extern int Plat_XColor2Pixel(XColor *xColorPtr);
extern void Plat_SyncDisplay(Display *display);



/* Make a tk hook function called 'objcmd_name' */
#define DECLARE_TK_HOOK(N) \
	extern int objcmd_##N _ANSI_ARGS_((ClientData clientData, \
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]))

/* DECLARE_TK_HOOK(angband); */
DECLARE_TK_HOOK(player);
DECLARE_TK_HOOK(power);
DECLARE_TK_HOOK(spell);
DECLARE_TK_HOOK(cave);
DECLARE_TK_HOOK(equipment);
DECLARE_TK_HOOK(floor);
DECLARE_TK_HOOK(game);
DECLARE_TK_HOOK(info);
DECLARE_TK_HOOK(init_icons);
DECLARE_TK_HOOK(inkey_flags);
DECLARE_TK_HOOK(inventory);
DECLARE_TK_HOOK(keycount);
DECLARE_TK_HOOK(keypress);
DECLARE_TK_HOOK(macro);
DECLARE_TK_HOOK(message);
DECLARE_TK_HOOK(mindcraft);
DECLARE_TK_HOOK(ARRAY_find);
DECLARE_TK_HOOK(ARRAY_info);
DECLARE_TK_HOOK(ARRAY_max);
DECLARE_TK_HOOK(ARRAY_set);
DECLARE_TK_HOOK(fontdesc);
DECLARE_TK_HOOK(menuentrystate);
DECLARE_TK_HOOK(photo_get);
DECLARE_TK_HOOK(photo_mask);
DECLARE_TK_HOOK(equipinfo);
DECLARE_TK_HOOK(inveninfo);
DECLARE_TK_HOOK(term_char);
DECLARE_TK_HOOK(term_attr);
#endif /* _INCLUDE_TNB_H_ */
