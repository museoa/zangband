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

#if !defined(PLATFORM_MAC) && !defined(PLATFORM_WIN) && !defined(PLATFORM_X11)
#error "You must define one of PLATFORM_MAC, PLATFORM_WIN or PLATFORM_X11"
#endif /* */

#if (TK_MINOR_VERSION != 3)
#error "(1) You must use tcl/tk version 8.3.3"
#endif /* */
#if (TK_RELEASE_SERIAL < 3)
#error "(2) You must use tcl/tk version 8.3.3"
#endif /* */


#define PU_MAP_INFO 0x04000000L	/* Update g_grid[] */

/* cmd4.c */

#define CHEAT_MAX 6
extern option_type cheat_info[];

/* init1.c */
extern char **r_info_flags[10];
extern int r_info_flags_max;
extern cptr k_info_flags1[], k_info_flags2[], k_info_flags3[];
extern cptr r_info_flags1[], r_info_flags2[], r_info_flags3[],
	r_info_flags4[], r_info_flags5[], r_info_flags6[], r_info_flags7[],
	r_info_flags8[], r_info_flags9[];

/* racial.c */
extern int racial_chance(s16b min_level, int use_stat, int difficulty);

/* spells3.c */
extern void spell_info(char *p, int spell, int realm);



/* main-tnb.c */
extern bool g_initialized;
extern bool game_in_progress;
extern cptr ANGBAND_DIR_ROOT;
extern cptr ANGBAND_DIR_TK;

/* canv-widget.c */
extern void CanvasWidget_Idle(void);

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
 * array in interp2.c, and keyword_inkey[] in bind.c, and the
 * KEYWORD_INKEY_XXX constants below.
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

extern bool store_will_buy(const object_type *o_ptr);

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

#define KEYWORD_INKEY_CMD 0
#define KEYWORD_INKEY_DIR 1
#define KEYWORD_INKEY_DISPLAY 2
#define KEYWORD_INKEY_ITEM 3
#define KEYWORD_INKEY_ITEM_STORE 4
#define KEYWORD_INKEY_MORE 5
#define KEYWORD_INKEY_SPELL 6
#define KEYWORD_INKEY_TARGET 7

enum {
KEYWORD_STATUS_CUT,
KEYWORD_STATUS_STUN,
KEYWORD_STATUS_HUNGER,
KEYWORD_STATUS_BLIND,
KEYWORD_STATUS_CONFUSED,
KEYWORD_STATUS_AFRAID,
KEYWORD_STATUS_POISONED,
KEYWORD_STATUS_STATE,
KEYWORD_STATUS_SPEED,
KEYWORD_STATUS_STUDY,
KEYWORD_STATUS_WINNER,
KEYWORD_STATUS_MAX
};

#define KEYWORD_TARGET_SET 0
#define KEYWORD_TARGET_UNSET 1
#define KEYWORD_TARGET_VISIBILITY 2

#define KEYWORD_CURSOR_SHOW 0
#define KEYWORD_CURSOR_HIDE 1

#define KEYWORD_ASSIGN_MONSTER 0
#define KEYWORD_ASSIGN_OBJECT 1
#define KEYWORD_ASSIGN_CHARACTER 2
#define KEYWORD_ASSIGN_FEATURE 3
#define KEYWORD_ASSIGN_SHOPKEEPER 4
#define KEYWORD_ASSIGN_ARTIFACT 5

#define KEYWORD_TERM_FRESH 0
#define KEYWORD_TERM_INKEY 1

#define KEYWORD_CHOOSE_ITEM 0
#define KEYWORD_CHOOSE_SPELL 1

#define KEYWORD_CHOOSE_POWER 2
#define KEYWORD_CHOOSE_MINDCRAFT 3
#define KEYWORD_CHOOSE_CMD_PET 4

#define KEYWORD_TRACK_HEALTH 0
#define KEYWORD_TRACK_RACE 1
#define KEYWORD_TRACK_OBJECT 2
#define KEYWORD_TRACK_INVENTORY 3
#define KEYWORD_TRACK_EQUIPMENT 4
#define KEYWORD_TRACK_GRID 5
#define KEYWORD_TRACK_MESSAGE 6

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

/* savefile.c */
extern errr angtk_savefile_info(char *fileName, char *varName);

/* setting.c */
extern void init_settings(void);

/* struct.c */
extern void init_struct(void);

/* town.c */
extern void init_vault(void);
extern int vault_apply_icon(int y, int x);
extern int vault_apply_feat(int y, int x);
extern bool angtk_build_town(void);
extern void angtk_rebuild_town(void);

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

/*
 * Option: Use a fast file buffer when writing/reading savefiles.
 */
#define ALLOW_SAVEFILE_BUFFER

/* Size of temp savefile buffer */
#define SAVEFILE_BUFFER_MAX (30L * 1024L)

/* Version number for savefile extension */
#define VERSION_MAJOR_TNB 1
#define VERSION_MINOR_TNB 0

#define VERSION_PATCH_TNB 0

/* Constants for racial and mutation powers */
enum {

/* Racial power */
POWER_DWARF,
POWER_HOBBIT,
POWER_GNOME,
POWER_HALF_ORC,
POWER_HALF_TROLL,
POWER_AMBERITE_A,
POWER_AMBERITE_B,
POWER_BARBARIAN,
POWER_HALF_OGRE,
POWER_HALF_GIANT,
POWER_HALF_TITAN,
POWER_CYCLOPS,
POWER_YEEK,
POWER_KLACKON,
POWER_KOBOLD,
POWER_NIBELUNG,
POWER_DARK_ELF,
POWER_DRACONIAN,
POWER_MIND_FLAYER,
POWER_IMP,
POWER_GOLEM,
POWER_SKELETON,
POWER_ZOMBIE,
POWER_VAMPIRE,
POWER_SPECTRE,
POWER_SPRITE,

/* Mutation power */
POWER_SPIT_ACID,
POWER_BR_FIRE,
POWER_HYPN_GAZE,
POWER_TELEKINES,
POWER_VTELEPORT,
POWER_MIND_BLST,
POWER_RADIATION,
POWER_VAMPIRISM,
POWER_SMELL_MET,
POWER_SMELL_MON,
POWER_BLINK,
POWER_EAT_ROCK,
POWER_SWAP_POS,
POWER_SHRIEK,
POWER_ILLUMINE,
POWER_DET_CURSE,
POWER_BERSERK,
POWER_POLYMORPH,
POWER_MIDAS_TCH,
POWER_GROW_MOLD,
POWER_RESIST,
POWER_EARTHQUAKE,
POWER_EAT_MAGIC,
POWER_WEIGH_MAG,
POWER_STERILITY,
POWER_PANIC_HIT,
POWER_DAZZLE,
POWER_LASER_EYE,
POWER_RECALL,
POWER_BANISH,
POWER_COLD_TOUCH,
POWER_LAUNCHER,

/* Max number of power's */
MAX_POWER
};

extern cptr power_desc[MAX_POWER];
extern int get_powers(int *power);

#ifdef ALLOW_EASY_FLOOR
extern int floor_y, floor_x;
#endif /* ALLOW_EASY_FLOOR */

#endif /* _INCLUDE_TNB_H_ */
