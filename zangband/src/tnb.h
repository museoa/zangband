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
#include "angband.h"

#if !defined(PLATFORM_WIN) && !defined(PLATFORM_X11)
#error "You must define one of PLATFORM_WIN or PLATFORM_X11"
#endif /* */

#if (TK_MINOR_VERSION != 3)
#error "(1) You must use tcl/tk version 8.3.3"
#endif /* */
#if (TK_RELEASE_SERIAL < 3)
#error "(2) You must use tcl/tk version 8.3.3"
#endif /* */


/* main-tnb.c */
extern bool g_initialized;
extern bool game_in_progress;
extern cptr ANGBAND_DIR_ROOT;
extern cptr ANGBAND_DIR_TK;
extern bool check_dir(cptr s);
extern void validate_file(cptr s, cptr fmt);

extern Tcl_Interp *g_interp;

/* canv-widget.c */
extern void CanvasWidget_Idle(void);
extern int CanvasWidget_Init(Tcl_Interp *interp);

/* const.c */
extern int init_const(Tcl_Interp *interp);


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

/* Constants for g_feat_flag[] */
#define FEAT_FLAG_BORING 0x01 /* Feature is optionally memorized */
#define FEAT_FLAG_TOWN 0x02 /* Feature uses lighting in the town */

extern byte *g_feat_flag;

/* Constants for t_grid.dark */
#define GRID_LITE_TORCH 0
#define GRID_LITE_NORMAL 1
#define GRID_LITE_DARK 2

/* Constants for t_grid.xtra */
#define GRID_XTRA_ISVERT 0x0008 /* Door is vertical */
#define GRID_XTRA_WALL 0x0010 /* Is a wall or secret door */
#define GRID_XTRA_DOOR 0x0020 /* Is a door */


/* Constants for t_grid.shape */
enum {
GRID_SHAPE_NOT,
GRID_SHAPE_SINGLE,
GRID_SHAPE_NS,
GRID_SHAPE_WE,
GRID_SHAPE_CORNER_NW,
GRID_SHAPE_CORNER_NE,
GRID_SHAPE_CORNER_SW,
GRID_SHAPE_CORNER_SE,
GRID_SHAPE_TRI_N,
GRID_SHAPE_TRI_S,
GRID_SHAPE_TRI_W,
GRID_SHAPE_TRI_E,
GRID_SHAPE_QUAD,
GRID_SHAPE_MAX
};

extern bool is_door(int y, int x);
extern bool is_wall(int y, int x);

/*
 * Memory of what is seen at a cave location for the entire cave.
 */
typedef struct t_grid {
	int dark; /* GRID_LITE_XXX flags */
	int f_idx; /* Feature */
	object_type *o_ptr; /* Object */
	int m_idx; /* Character/Monster */
	int xtra; /* GRID_XTRA_XXX flags */
	int shape; /* GRID_SHAPE_XXX enum */
} t_grid;

extern t_grid *g_grid[MAX_HGT];

/* TRUE if g_grid[].xtra was initialized */
extern int g_grid_xtra_init;

extern bool g_daytime; /* Day or night */

/* Cave location -> t_grid */
extern void get_grid_info(int y, int x, t_grid *gridPtr);

extern void angtk_image_reset(void);
extern void angtk_cave_changed(void);
extern void angtk_feat_known(int y, int x);
extern void angtk_feat_changed(int y, int x);
extern void angtk_view_floor(int y, int x, int info, int torch);
extern void angtk_view_wall(int y, int x, int info, int torch);
extern void set_grid_assign(int y, int x);
extern bool door_vertical(int y, int x);
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
extern void angtk_character_generated(void);
extern void angtk_display_info_init(void);
extern void angtk_display_info_append(cptr s);
extern void angtk_display_info_done(cptr title);
extern void angtk_display_info(char *title, char **info, int count);
extern void angtk_display_info_aux(char *title, Tcl_Obj *listObjPtr);
extern void angtk_eval(cptr command, ...);
extern int angtk_eval_file(cptr extFileName);
extern int angtk_generate(void);
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

/* Event types */
#define EVENT_POSITION 1
#define EVENT_STAT 2
#define EVENT_TARGET 3
#define EVENT_STATUS 4
#define EVENT_INKEY 5
#define EVENT_CURSOR 6
#define EVENT_ASSIGN 7
#define EVENT_TERM 8
#define EVENT_CHOOSE 9
#define EVENT_TRACK 10
#define EVENT_PY 11
#define EVENT_SETTING 12
#define EVENT_DUNGEON 13
#define EVENT_KEYMAP 14

#define KEYWORD_TERM_FRESH 0
#define KEYWORD_TERM_INKEY 1

#define KEYWORD_CHOOSE_ITEM 0
#define KEYWORD_CHOOSE_SPELL 1

#define KEYWORD_CHOOSE_POWER 2
#define KEYWORD_CHOOSE_MINDCRAFT 3
#define KEYWORD_CHOOSE_CMD_PET 4

#define KEYWORD_PY_HP 0
#define KEYWORD_PY_SP 1
#define KEYWORD_PY_FOOD 2
#define KEYWORD_PY_DEPTH 3
#define KEYWORD_PY_EXP 4
#define KEYWORD_PY_LEVEL 5
#define KEYWORD_PY_AC 6
#define KEYWORD_PY_NAME 7
#define KEYWORD_PY_TITLE 8
#define KEYWORD_PY_GOLD 9
#define KEYWORD_PY_RACE 10

#define KEYWORD_DUNGEON_ENTER 0
#define KEYWORD_DUNGEON_LEAVE 1
#define KEYWORD_DUNGEON_GEN 2

/* bind.c */
extern cptr keyword_assign[];
extern cptr keyword_stat[];
extern cptr keyword_status[];
extern cptr keyword_target[];
extern cptr *keyword_setting;
extern void init_bindings(void);
extern void Bind_Choose(int detail, int other, int show);
extern void Bind_Cursor(int detail, int y, int x);
extern void Bind_Generic(int eventType, int eventDetail);
extern void Bind_Keymap(int ch);
extern void Bind_Position(int who, int y1, int x1, int y2, int x2);
extern void Bind_Setting(int detail, int value);
extern void Bind_Status(int detail);
extern void Bind_Track(int detail, int who, int y, int x);
extern void Bind_Option(const char *name, int value);

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


/* setting.c */
extern void init_settings(void);
extern struct SettingGroup_ *g_setting;

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

/* Make a tk hook function called 'objcmd_name' */
#define DECLARE_TK_HOOK(N) \
	extern int objcmd_##N _ANSI_ARGS_((ClientData clientData, \
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]))

DECLARE_TK_HOOK(angband);
DECLARE_TK_HOOK(player);
DECLARE_TK_HOOK(power);
DECLARE_TK_HOOK(setting);
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

#endif /* _INCLUDE_TNB_H_ */
