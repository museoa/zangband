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

/* Hack - prevent warnings from tk headers */
#ifdef errno
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
 * Hack - prevent a huge number of compiler warnings when <tclInt.h>
 * is included indirectly below.
 */
#	define _TCLINTDECLS
#	include <tkInt.h>
#endif /* PLATFORM_X11 */


/* main-tnb.c */
extern bool game_in_progress;
extern cptr ANGBAND_DIR_TK;

extern Tcl_Interp *g_interp;

/* canv-widget.c */
extern int CanvasWidget_Init(Tcl_Interp *interp);


/* cmdinfo.c */
typedef struct CommandInfo CommandInfo;
typedef struct CommandInit CommandInit;
typedef struct SubCommandInfo SubCommandInfo;

struct SubCommandInfo {
	int alloc;
	int count;
	cptr *name;
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

extern void CommandInfo_Add(CommandInfo *infoCmd, CommandInfo *infoSubCmd);
extern CommandInfo *CommandInfo_GetInfo(Tcl_Interp *interp, cptr names[]);
extern int CommandInfo_InitAux(Tcl_Interp *interp, CommandInit *init,
	int index, CommandInfo *parent);
extern int CommandInfo_Init(Tcl_Interp *interp, CommandInit *init,
	CommandInfo *parent);
extern CommandInfo *CommandInfo_New(CommandInit *init);
extern int CommandInfo_ObjCmd(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

/* icon1.c */

extern int g_icon_size; /* 16, 24 or 32 */
extern void init_icons(int size, int depth);


/*
 * Memory of what is seen at a cave location for the entire cave.
 */
typedef struct t_grid {
	int f_idx; /* Feature */
	object_type *o_ptr; /* Object */
	int m_idx; /* Character/Monster */
} t_grid;

extern t_grid *g_grid[MAX_HGT];

/* Cave location -> t_grid */
extern void get_grid_info(int y, int x, t_grid *gridPtr);

extern void angtk_image_reset(void);
extern void set_grid_assign(int y, int x);
extern void Icon_Exit(void);
extern void init_palette(void);

/* widget.c */
extern void (*angtk_lite_spot)(int y, int x);
extern void angtk_lite_spot_real(int y, int x);
extern void angtk_wipe_spot(int y, int x);

extern int exit_skip_save;
extern bool command_repeating;
extern void angtk_angband_initialized(void);
extern void angtk_display_info_init(void);
extern void angtk_display_info_append(cptr s);
extern void angtk_display_info_done(cptr title);
extern void angtk_display_info(char *title, char **info, int count);
extern void angtk_display_info_aux(cptr title, Tcl_Obj *listObjPtr);
extern void angtk_eval(cptr command, ...);
extern int angtk_eval_file(cptr extFileName);
extern void angtk_cave_generated(void);
extern void angtk_health(char *buf);
extern void angtk_init(void);
extern cptr player_status(int status, int *value);


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
#define INKEY_POWER 9
#define INKEY_CMD_PET 10

extern int inkey_book;
extern int inkey_flags;

/* describe.c */
extern cptr keyword_slot[];
extern int strcpy_len(char *s1, const char *s2);
extern long angtk_describe_object(object_type *o_ptr, char *buf, bool in_store);
extern int SetArrayValueChar(cptr varName, cptr field, char value);
extern int SetArrayValueLong(cptr varName, cptr field, long value);
extern int SetArrayValueString(cptr varName, cptr field, cptr value);

/* tcltk.c */
extern Tcl_Interp *TclTk_Init(cptr *argv);

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

extern int ExtToUtf_SetArrayValueString(cptr varName, cptr field, cptr value);

/* util-dll.c */
extern int g_palette_white, g_palette_black;
#define PALETTE_WHITE g_palette_white
#define PALETTE_BLACK g_palette_black

extern int g_colormap_white, g_colormap_black;
#define COLORMAP_WHITE g_colormap_white
#define COLORMAP_BLACK g_colormap_black

extern unsigned char g_palette2colormap[256];

extern int Palette_Init(Tcl_Interp *interp);
#ifdef PLATFORM_WIN
/* HPALETTE */ extern void *Palette_GetHPal(void);
#endif /* PLATFORM_WIN */
extern unsigned char *Palette_GetRGB(void);
extern void Palette_ResetHash(void);
extern int Palette_RGB2Index(unsigned char r, unsigned char g, unsigned char b);

extern int Colormap_Init(Tcl_Interp *interp);
extern unsigned char *Colormap_GetRGB(void);
extern int Colormap_RGB2Index(unsigned char r, unsigned char g, unsigned char b);

typedef struct RGBInfo RGBInfo;
struct RGBInfo
{
	int red_count, green_count, blue_count;
	int red_mask, green_mask, blue_mask;
	int red_shift, green_shift, blue_shift;
	int extra;
};

extern RGBInfo g_rgbi;

extern void SetPix16(unsigned char *p, int r, int g, int b);

extern void *Array_Append(void *array_ptr, int *count, int elem_size,
	void *elem_ptr);
extern void *Array_Insert(void *array_ptr, int *count, int elem_size,
	int index);
extern void *Array_Delete(void *array_ptr, int *count, int elem_size,
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

extern void Bitmap_New(Tcl_Interp *interp, BitmapPtr bitmapPtr);
extern void Bitmap_Delete(BitmapPtr bitmapPtr);
extern BitmapPtr Bitmap_Load(Tcl_Interp *interp, cptr name);

#ifdef PLATFORM_WINxxx
extern HFONT TkToWinFont(Tk_Font tkFont);
#endif /* PLATFORM_WIN */

extern Tcl_Obj *ExtToUtf_NewStringObj(CONST char *bytes, int length);
extern void ExtToUtf_SetResult(Tcl_Interp *interp, cptr string);
extern char *UtfToExt_TranslateFileName(Tcl_Interp *interp, char *utfPath,
	Tcl_DString *extDStringPtr);


/* plat.c */
extern void *Plat_PaletteInit(unsigned char *rgb);
extern int Plat_XColor2Pixel(XColor *xColorPtr);
extern void Plat_SyncDisplay(Display *display);



/* Make a tk hook function called 'objcmd_name' */
#define DECLARE_TK_HOOK(N) \
	extern int objcmd_##N (ClientData clientData, \
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])

/* DECLARE_TK_HOOK(angband); */
DECLARE_TK_HOOK(player);
DECLARE_TK_HOOK(cave);
DECLARE_TK_HOOK(floor);
DECLARE_TK_HOOK(game);
DECLARE_TK_HOOK(init_icons);
DECLARE_TK_HOOK(inkey_flags);
DECLARE_TK_HOOK(inventory);
DECLARE_TK_HOOK(keycount);
DECLARE_TK_HOOK(keypress);
DECLARE_TK_HOOK(message);
DECLARE_TK_HOOK(fontdesc);
DECLARE_TK_HOOK(equipinfo);
DECLARE_TK_HOOK(inveninfo);
DECLARE_TK_HOOK(term_char);
DECLARE_TK_HOOK(term_attr);
#endif /* _INCLUDE_TNB_H_ */


extern int init_widget(Tcl_Interp *interp);
