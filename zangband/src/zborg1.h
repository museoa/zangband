
/* File: zborg1.h */
/* Purpose: Header file for "borg1.c" -BEN- */

#ifndef INCLUDED_BORG1_H
#define INCLUDED_BORG1_H

#include "angband.h"



#ifdef ALLOW_BORG


/*
 * Number of grids in each screen (vertically)
 * Must be a multiple of PANEL_HGT (at least 2x)
 */
#define SCREEN_HGT	22

/*
 * Number of grids in each screen (horizontally)
 * Must be a multiple of PANEL_WID (at least 2x)
 */
#define SCREEN_WID	66


/* WARNING: If you mess with the objects.txt or the monster.txt and change
 * the locations of things, then you must make those changes in zborg.txt
 * as well as in this borg code.  The borg is very concerned about item
 * index locations.  ie: borgs_has[242] is looking for a Potion of Healing.
 * The borg is concerned over several items, broken swords, and several
 * monster locations (Tarresque, Sauron, Morgoth).
 */

/*
 * This file provides support for "borg1.c".
 */



/*** Some constants ***/


/*
 * Maximum possible dungeon size
 */
#define AUTO_MAX_X  MAX_WID
#define AUTO_MAX_Y  MAX_HGT



/*
 * Flags for the "info" field of grids
 *
 * Note that some of the flags below are not "perfect", in particular,
 * several of the flags should be treated as "best guesses", see below.
 *
 * The "BORG_MARK" flag means that the grid has been "observed", though
 * the terrain feature may or may not be memorized.  Note the use of the
 * "FEAT_NONE", "FEAT_FLOOR", and "FEAT_INVIS" feature codes below.
 *
 * The "BORG_GLOW" flag means that a grid is probably "perma-lit", but
 * sometimes it is really only "recently" perma-lit, but was then made
 * dark with a darkness attack, and it is now torch-lit or off-screen.
 *
 * The "BORG_DARK" flag means that a grid is probably not "perma-lit",
 * but sometimes it is really only "recently" dark, but was then made
 * "lite" with a "call lite" spell, and it is now torch-lit or off-screen.
 *
 * The "BORG_LITE" flag means that a grid is probably lit by the player
 * torch, but this may not be true if the nearby "BORG_VIEW" flags are
 * not correct, or if the "lite radius" has changed recently.
 *
 * The "BORG_VIEW" flag means that a grid is probably in line of sight
 * of the player, but this may not be true if some of the grids between
 * the player and the grid contain previously unseen walls/doors/etc.
 *
 * The "BORG_TEMP" flag means that a grid has been added to the array
 * "borg_temp_x"/"borg_temp_y", though normally we ignore this flag.
 *
 * The "BORG_XTRA" flag is used for various "extra" purposes, primarily
 * to assist with the "update_view()" code.
 */
#define BORG_MARK   0x01    /* observed grid */
#define BORG_GLOW   0x02    /* probably perma-lit */
#define BORG_DARK   0x04    /* probably not perma-lit */
#define BORG_OKAY   0x08    /* on the current panel */
#define BORG_LITE   0x10    /* lit by the torch */
#define BORG_VIEW   0x20    /* in line of sight */
#define BORG_TEMP   0x40    /* temporary flag */
#define BORG_XTRA   0x80    /* extra flag */


/*
 * Maximum size of the "lite" array
 */
#define AUTO_LITE_MAX 1536

/*
 * Some assistance with the borg_attack and magic arrows
 */
#define GF_ARROW_SEEKER  92
#define GF_ARROW_FLAME   93
#define GF_ARROW_FROST   94
#define GF_ARROW_ANIMAL  95
#define GF_ARROW_DRAGON  96
#define GF_ARROW_EVIL    97
#define GF_ARROW_WOUNDING 98
#define GF_HOLY_WORD      99
#define GF_DISP_UNDEAD_DEMON 100  /* effect both */
#define GF_ELEMENTS       101 /* all elements could be cast */
#define GF_DEATHRAY      102

/* the Z randarts are considered #127 by the borg */
#define ART_RANDART  127

/* Magic Numbers are Evil */
#define BORG_GSTORE  	0
#define BORG_TEMPLE  	3
#define BORG_ALCHEMIST  4
#define BORG_BM  		6
#define BORG_HOME  		7



/*
 * Maximum size of the "view" array
 */
/*#define AUTO_VIEW_MAX 1536*/
#define AUTO_VIEW_MAX 9000


/*
 * Number of grids in the "temp" array
 */
#define AUTO_TEMP_MAX 1536


/*
 * Number of grids in the "flow" array
 */
#define AUTO_FLOW_MAX 1536



/*
 * Enable the "borg_note()" usage of the Recall Window
 * Also specify the number of "rolling rows" to use
 */
#define BORG_NOTE_ROWS      12



/*
 * Size of Keypress buffer
 */
#define KEY_SIZE 8192




/*
 * Object information
 */

typedef struct borg_take borg_take;

struct borg_take
{
    s16b    k_idx;      /* Kind index */

    bool    known;      /* Verified kind */

    bool    seen;       /* Assigned motion */

    bool    extra;      /* Unused */

    byte    x, y;       /* Location */

    s16b    when;       /* When last seen */
};


/*
 * Monster information
 */

typedef struct borg_kill borg_kill;

struct borg_kill
{
    s16b    r_idx;      /* Race index */

    bool    known;      /* Verified race */
    bool    awake;      /* Probably awake */

    bool    confused;   /* Probably confused */
    bool    afraid;     /* Probably afraid */
    bool    quiver;     /* Probably quivering */
    bool    stunned;
    bool    poisoned;   /* Probably poisoned */
	bool    invulner;	/* Probably Invulnerable */

    bool    seen;       /* Assigned motion */
    bool    used;       /* Assigned message */

    byte    x, y;       /* Location */

    byte    ox, oy;     /* Old location */

    byte    speed;      /* Estimated speed */
    byte    moves;      /* Estimates moves */
    bool    ranged_attack; /* can attack from a dx */
    s16b    power;      /* Estimated hit-points */
    s16b    other;      /* Estimated something */
    s16b    level;      /* Monsters Level */

	bool	friendly;	/* friend/pet */
    s16b    when;       /* When last seen */
};



/*
 * Maximum number of rooms.  This may be too small.
 * But if AUTO_ROOMS * sizeof(borg_room) > 64K then some
 * machines may not be able to allocate the room array.
 */
#define AUTO_ROOMS  (AUTO_MAX_X * AUTO_MAX_Y / 8)

/*
 * Forward declare
 */
typedef struct borg_grid borg_grid;

/*
 * A grid in the dungeon.  Several bytes.
 *
 * There is a set of eight bit flags (see below) called "info".
 *
 * There is a terrain feature type, which may be incorrect.  It is
 * more or less based on the standard "feature" values, but some of
 * the legal values are never used, such as "secret door", and others
 * are used in bizarre ways, such as "invisible trap".
 *
 * There is an object index into the "object tracking" array.
 *
 * There is a monster index into the "monster tracking" array.
 *
 * There is a byte "hmmm" which is currently unused.
 *
 * There is a byte "xtra" which tracks how much "searching" has been done
 * in the grid or in any grid next to the grid.
 *
 * To perform "navigation" from one place to another, the "flow" routines
 * are used, which place "cost" information into the "cost" fields.  Then,
 * if the path is clear, the "cost" information is copied into the "flow"
 * fields, which are used for the actual navigation.  This allows multiple
 * routines to check for "possible" flowing, without hurting the current
 * flow, which may have taken a long time to construct.  We also assume
 * that the Borg never needs to follow a path longer than 250 grids long.
 * Note that the "cost" fields have been moved into external arrays.
 *
 * Hack -- note that the "char" zero will often crash the system!
 */
struct borg_grid
{
    byte feat;      /* Grid type */
    byte info;      /* Grid flags */

    byte take;      /* Object index */
    byte kill;      /* Monster index */

    byte hmmm;      /* Extra field (unused) */

    byte xtra;      /* Extra field (search count) */
};


/*
 * Forward declare
 */
typedef struct borg_data borg_data;

/*
 * Hack -- one byte of info per grid
 *
 * We use a structure to encapsulate the data into a "typed" form.
 */
struct borg_data
{
    byte data[AUTO_MAX_Y][AUTO_MAX_X];
};




/*** Some macros ***/


/*
 * Determine "twice" the distance between two points
 * This results in "diagonals" being "correctly" ranged,
 * that is, a diagonal appears "furthur" than an adjacent.
 */
#define double_distance(Y1,X1,Y2,X2) \
    (distance(((int)(Y1))<<1,((int)(X1))<<1,((int)(Y2))<<1,((int)(X2))<<1))



/*** Some variables ***/


/*
 * Some variables
 */
extern bool borg_active;        /* Actually active */
extern bool borg_resurrect;     /* Continous play mode */
extern bool borg_cancel;        /* Being cancelled */

extern char genocide_target;    /* Identity of the poor unsuspecting soul */
extern int zap_slot;            /* to avoid a nasty game bug with amnesia */
extern bool borg_casted_glyph;  /* we dont have a launch messages anymore */
extern int borg_stop_dlevel;
extern int borg_stop_clevel;
extern bool borg_stop_king;
extern bool borg_dont_react;
extern int successful_target;
extern int sold_item_tval;
extern int sold_item_sval;
extern int sold_item_pval;
extern int sold_item_store;

/* options from the zborg.txt file */
extern int borg_respawn_race;
extern int borg_respawn_class;
extern int borg_respawn_str;
extern int borg_respawn_int;
extern int borg_respawn_wis;
extern int borg_respawn_dex;
extern int borg_respawn_con;
extern int borg_respawn_chr;
extern int borg_dump_level;
extern bool borg_respawn_winners;
extern int borg_delay_factor;

extern bool borg_worships_damage;
extern bool borg_worships_speed;
extern bool borg_worships_hp;
extern bool borg_worships_mana;
extern bool borg_worships_ac;
extern bool borg_worships_gold;
extern bool borg_plays_risky;
extern bool borg_uses_swaps;
extern bool borg_uses_calcs;
extern bool borg_slow_optimizehome;
extern bool borg_scums_uniques;
extern bool borg_engage_cloak;
extern int borg_chest_fail_tolerance;
extern int borg_money_scum_amount;
extern bool borg_scums_money;

/* HACK... this should really be a parm into borg_prepared */
/*         I am just being lazy */
extern bool borg_slow_return;

/* dynamic required items */
/* dynamic required items */
typedef struct req_item
{
   int depth;
   int item;
   int number;

} req_item;

extern req_item *borg_required_item[MAX_CLASS];
extern int n_req[MAX_CLASS];

typedef struct power_item
{
   int depth;
   int cnd;
   int item;
   int from;
   int to;
   int power;
   bool each;
} power_item;


extern power_item *borg_power_item[MAX_CLASS];
extern int n_pwr[MAX_CLASS];
extern int *borg_has;
extern int *borg_has_on;
extern int *borg_artifact;
extern int *borg_skill;
extern int size_depth;
extern int size_obj;

/* NOTE: This must exactly match the prefix_pref enums in BORG1.c */
enum
{
    BI_STR,
    BI_INT,
    BI_WIS,
    BI_DEX,
    BI_CON,
    BI_CHR,
    BI_CSTR,
    BI_CINT,
    BI_CWIS,
    BI_CDEX,
    BI_CCON,
    BI_CCHR,
    BI_SSTR,
    BI_SINT,
    BI_SWIS,
    BI_SDEX,
    BI_SCON,
    BI_SCHR,
    BI_INTMANA,
    BI_WISMANA,
    BI_LITE,
    BI_CURHP,
    BI_MAXHP,
    BI_HP_ADJ,
    BI_OLDCHP,
    BI_CURSP,
    BI_MAXSP,
    BI_SP_ADJ,
    BI_OLDCSP,
    BI_FAIL1,
    BI_FAIL2,
    BI_REALM1,
    BI_REALM2,
    BI_CLEVEL,
    BI_MAXCLEVEL,
    BI_ESP,
    BI_CUR_LITE,
    BI_RECALL,
    BI_FOOD,
    BI_SPEED,
    BI_SDIG,
    BI_FEATH,
    BI_REG,
    BI_SINV,
    BI_INFRA,
    BI_DIS,
    BI_DEV,
    BI_SAV,
    BI_STL,
    BI_SRCH,
    BI_SRCHFREQ,
    BI_THN,
    BI_THB,
    BI_THT,
    BI_DIG,
    BI_IFIRE,
    BI_IACID,
    BI_ICOLD,
    BI_IELEC,
    BI_RFIRE,
    BI_RCOLD,
    BI_RELEC,
    BI_RACID,
    BI_RPOIS,
    BI_RFEAR,
    BI_RLITE,
    BI_RDARK,
    BI_RBLIND,
    BI_RCONF,
    BI_RSND,
    BI_RSHRD,
    BI_RNXUS,
    BI_RNTHR,
    BI_RKAOS,
    BI_RDIS,
    BI_REFLECT,
    BI_HLIFE,
    BI_FRACT,
    BI_SRFIRE,
    BI_SRCOLD,
    BI_SRELEC,
    BI_SRACID,
    BI_SRPOIS,
    BI_SRFEAR,
    BI_SRLITE,
    BI_SRDARK,
    BI_SRBLIND,
    BI_SRCONF,
    BI_SRSND,
    BI_SRSHRD,
    BI_SRNXUS,
    BI_SRNTHR,
    BI_SRKAOS,
    BI_SRDIS,
    BI_SHLIFE,
    BI_SFRACT,
    BI_DEPTH,
    BI_CDEPTH,
    BI_MAXDEPTH,
    BI_KING,

    BI_ISWEAK,
    BI_ISHUNGRY,
    BI_ISFULL,
    BI_ISGORGED,
    BI_ISBLIND,
    BI_ISAFRAID,
    BI_ISCONFUSED,
    BI_ISPOISONED,
    BI_ISCUT,
    BI_ISSTUN,
    BI_ISHEAVYSTUN,
    BI_ISIMAGE,
    BI_ISSTUDY,
    BI_ISSEARCHING,
    BI_ISFIXLEV,
    BI_ISFIXEXP,
    BI_ISFIXSTR,
    BI_ISFIXINT,
    BI_ISFIXWIS,
    BI_ISFIXDEX,
    BI_ISFIXCON,
    BI_ISFIXCHR,
    BI_ISFIXALL,
	BI_HRTIME,
	BI_MNTIME,

    BI_ARMOR,
    BI_TOHIT,
    BI_TODAM,
    BI_WTOHIT,
    BI_WTODAM,
    BI_BTOHIT,
    BI_BTODAM,
    BI_BLOWS,
    BI_SHOTS,
    BI_WMAXDAM,
    BI_WBASEDAM,
    BI_BMAXDAM,
    BI_HEAVYWEPON,
    BI_HEAVYBOW,
    BI_CRSTELE,
    BI_CRSAGRV,
    BI_CRSTY,
    BI_CRSNOTELE,  /* no teleport */
    BI_CRSNOMAGIC, /* no magic */
	BI_ENCUMBERD,
	BI_FEAR_LITE,
    BI_WS_ANIMAL,
    BI_WS_EVIL,
    BI_WS_UNDEAD,
    BI_WS_DEMON,
    BI_WS_ORC,
    BI_WS_TROLL,
    BI_WS_GIANT,
    BI_WS_DRAGON,
    BI_WK_DRAGON,
    BI_W_IMPACT,
    BI_WB_ACID,
    BI_WB_ELEC,
    BI_WB_FIRE,
    BI_WB_COLD,
    BI_WB_POIS,
    BI_WB_VORPAL,
    BI_WB_VAMPIRIC,
    BI_WB_CHAOTIC,
    BI_ATELEPORT,
    BI_AESCAPE,
    BI_AFUEL,
    BI_AHEAL,
    BI_AEZHEAL,
    BI_AID,
    BI_ASPEED,
    BI_ASTFMAGI,
    BI_ASTFDEST,
    BI_AMISSILES,
    BI_ACUREPOIS,
    BI_ADETTRAP,
    BI_ADETDOOR,
    BI_ADETEVIL,
    BI_AMAGICMAP,
    BI_ARECHARGE,
    BI_APFE,
    BI_AGLYPH,
    BI_ACCW,
    BI_ACSW,
    BI_ARESHEAT,
    BI_ARESCOLD,
    BI_ATELEPORTLVL,  /* scroll of teleport level */
    BI_AXGOI,            /* reliable GOI spell */
    BI_AGOI,            /* GOI spell */
    BI_AHWORD,            /* Holy Word prayer */

	BI_TOWN_NUM,			/* Current town number */
	BI_ARENA_NUM,		/* monster number in arena -KMW- */
	BI_INSIDEARENA,		/* Is character inside arena? */
	BI_INSIDEQUEST,		/* Inside quest level */
	BI_X_WILD,      	/* Coordinates in the wilderness */
	BI_Y_WILD,

    BI_MAX
};

#define MAX_FORMULA_ELEMENTS 60
enum
{
    BFO_DONE, /* just to make sure we end fast if there is no formula */
    BFO_NUMBER,
    BFO_VARIABLE,
    BFO_EQ,
    BFO_NEQ,
    BFO_NOT,
    BFO_LT,
    BFO_LTE,
    BFO_GT,
    BFO_GTE,
    BFO_AND,
    BFO_OR,
    BFO_PLUS,
    BFO_MINUS,
    BFO_DIVIDE,
    BFO_MULT
};

extern int *formula[1000];
extern cptr prefix_pref[];

/*
 * Hack -- optional cheating flags
 */

extern bool borg_do_star_id;


/*
 * Various silly flags
 */

extern bool borg_flag_save;     /* Save savefile at each level */

extern bool borg_flag_dump;     /* Save savefile at each death */

extern bool borg_save; /* do a save next time we get to press a key! */

extern bool borg_borg_message;      /* List borg messages or not */
extern bool borg_graphics;          /* List borg messages or not */
extern bool borg_confirm_target;

extern char borg_engine_date[];       /* last update */

/*
 * Use a simple internal random number generator
 */

extern bool borg_rand_quick;        /* Save system setting */

extern u32b borg_rand_value;        /* Save system setting */

extern u32b borg_rand_local;        /* Save personal setting */


/*
 * Hack -- time variables
 */

extern s16b borg_t;        /* Current "time" */
extern s16b need_see_inviso;        /* To tell me to cast it */
extern s16b borg_see_inv;
extern bool need_shift_panel;        /* to spot offscreeners */
extern s16b when_shift_panel;
extern s16b time_this_panel;        /* Current "time" for current panel*/
extern bool vault_on_level;     /* borg will search for a vault */
extern bool unique_on_level;
extern bool scaryguy_on_level;
extern s16b borg_quest_level;      /* On a level with quest */

extern bool breeder_level;      /* Borg will shut doors */
extern s16b old_depth;
extern s16b borg_respawning;       /* to prevent certain crashes */
extern bool genocide_level_hounds; /* used against hounds */
extern s16b borg_hound_count;
extern s16b borg_no_retreat;

/*
 * Hack -- Other time variables
 */

extern s16b when_call_lite; /* When we last did call light */
extern s16b when_wizard_lite;   /* When we last did wizard light */

extern s16b when_detect_traps;  /* When we last detected traps */
extern s16b when_detect_doors;  /* When we last detected doors */
extern s16b when_detect_walls;  /* When we last detected walls */
extern s16b when_detect_evil;
extern s16b when_last_kill_mult;   /* When a multiplier was last killed */

extern bool my_need_alter;     /* incase of walls/doors */
extern bool my_no_alter;     /* incase of walls/doors */
extern bool my_need_redraw;     /* incase of walls/doors */
extern bool borg_attempting_refresh;  /* for the goi spell */

/*
 * Some information
 */

extern s16b goal;       /* Flowing (goal type) */

extern bool goal_rising;    /* Currently returning to town */

extern bool goal_leaving;   /* Currently leaving the level */

extern bool goal_fleeing;   /* Currently fleeing the level */

extern bool goal_ignoring;  /* Currently ignoring monsters */

extern int goal_recalling;  /* Currently waiting for recall, guessing turns left */
extern bool goal_less;      /* return to, but dont use, the next up stairs */

extern s16b borg_times_twitch; /* how often twitchy on this level */
extern s16b borg_escapes; /* how often teleported on this level */

extern bool stair_less;     /* Use the next "up" staircase */
extern bool stair_more;     /* Use the next "down" staircase */

extern s32b borg_began;     /* When this level began */
extern s32b borg_time_town; /* how long it has been since I was in town */

extern s16b avoidance;      /* Current danger thresh-hold */

extern bool borg_failure;   /* Notice failure */

extern bool borg_simulate;  /* Simulation flag */
extern bool borg_attacking; /* Are we attacking a monster? */
extern bool borg_offsetting; /* Are we attacking a monster? with offsett balls*/

extern bool borg_completed; /* Completed the level */
extern bool borg_on_upstairs;      /* used when leaving a level */
extern bool borg_on_dnstairs;      /* used when leaving a level */
extern bool borg_needs_searching;  /* borg will search with each step */
extern bool borg_full_damage;  /* make danger = full possible damage. */

/* defence flags */
extern bool borg_prot_from_evil;
extern bool borg_speed;
extern bool borg_bless;
extern bool borg_hero;
extern bool borg_berserk;
extern bool my_oppose_fire;
extern bool my_oppose_cold;
extern bool my_oppose_acid;
extern bool my_oppose_pois;
extern bool my_oppose_elec;
extern s16b borg_goi;
extern s16b borg_inviso;
extern bool borg_esp;
extern s16b borg_game_ratio;
extern bool borg_shield;
extern bool borg_on_glyph; /* borg is standing on a glyph of warding */
extern bool borg_create_door; /* borg is going to create doors */
extern bool borg_sleep_spell;
extern bool borg_sleep_spell_ii;
extern bool borg_slow_spell;
extern bool borg_confuse_spell;
extern bool borg_fear_mon_spell;


/*
 * Shop goals
 */

extern s16b goal_shop;      /* Next shop to visit */
extern s16b goal_ware;      /* Next item to buy there */
extern s16b goal_item;      /* Next item to sell there */
extern int borg_food_onsale;      /* Are shops selling food? */

/*
 * Other variables
 */

extern int w_x;         /* Current panel offset (X) */
extern int w_y;         /* Current panel offset (Y) */

extern int c_x;         /* Current location (X) */
extern int c_y;         /* Current location (Y) */

extern int g_x;         /* Goal location (X) */
extern int g_y;         /* Goal location (Y) */

extern int dim_door_y;		/* Safe landing zone for DDoor */
extern int dim_door_x;

extern int bad_obj_x[50];   /* Dropped cursed artifact at location (X) */
extern int bad_obj_y[50];   /* Dropped cursed artifact at location (Y) */

/*
 * Some estimated state variables
 */

extern s16b my_stat_max[6]; /* Current "maximal" stat values    */
extern s16b my_stat_cur[6]; /* Current "natural" stat values    */
extern s16b my_stat_use[6]; /* Current "resulting" stat values  */
extern s16b my_stat_ind[6]; /* Current "additions" to stat values   */
extern bool my_need_stat_check[6];  /* do I need to check my stats */

extern s16b my_stat_add[6];  /* aditions to stats */

extern s16b home_stat_add[6];

extern int  weapon_swap;   /* location of my swap weapon   */
extern s32b weapon_swap_value;   /* value of my swap weapon   */
extern int  armour_swap;   /* location of my swap weapon   */
extern s32b armour_swap_value;   /* value of my swap weapon   */

/* a 3 state boolean */
/*-1 = not cursed, no help needed for it */
/* 0 = light curse, needs light remove curse spell */
/* 1 = heavy curse, needs heavy remove curse spell */
extern int decurse_weapon_swap;  /* my swap is great, except its cursed */
extern int enchant_weapon_swap_to_h;  /* my swap is great, except its cursed */
extern int enchant_weapon_swap_to_d;  /* my swap is great, except its cursed */
extern int decurse_armour_swap;  /* my swap is great, except its cursed */
extern int enchant_armour_swap_to_a;  /* my swap is great, except its cursed */
extern bool borg_wearing_cursed;

extern s16b weapon_swap_digger;
extern int  weapon_swap_slay_animal;
extern int  weapon_swap_slay_evil;
extern int  weapon_swap_slay_undead;
extern int  weapon_swap_slay_demon;
extern int  weapon_swap_slay_orc;
extern int  weapon_swap_slay_troll;
extern int  weapon_swap_slay_giant;
extern int  weapon_swap_slay_dragon;
extern int  weapon_swap_kill_dragon;
extern int  weapon_swap_impact;
extern int  weapon_swap_brand_acid;
extern int  weapon_swap_brand_elec;
extern int  weapon_swap_brand_fire;
extern int  weapon_swap_brand_cold;
extern int  weapon_swap_see_infra;
extern int  weapon_swap_slow_digest;
extern int  weapon_swap_aggravate;
extern int  weapon_swap_teleport;
extern int  weapon_swap_regenerate;
extern int  weapon_swap_telepathy;
extern int  weapon_swap_lite;
extern int  weapon_swap_see_invis;
extern int  weapon_swap_ffall;
extern int  weapon_swap_free_act;
extern int  weapon_swap_hold_life;
extern int  weapon_swap_immune_fire;
extern int  weapon_swap_immune_acid;
extern int  weapon_swap_immune_cold;
extern int  weapon_swap_immune_elec;
extern int  weapon_swap_resist_acid;
extern int  weapon_swap_resist_elec;
extern int  weapon_swap_resist_fire;
extern int  weapon_swap_resist_cold;
extern int  weapon_swap_resist_pois;
extern int  weapon_swap_resist_conf;
extern int  weapon_swap_resist_sound;
extern int  weapon_swap_resist_lite;
extern int  weapon_swap_resist_dark;
extern int  weapon_swap_resist_chaos;
extern int  weapon_swap_resist_disen;
extern int  weapon_swap_resist_shard;
extern int  weapon_swap_resist_nexus;
extern int  weapon_swap_resist_blind;
extern int  weapon_swap_resist_neth;
extern int  weapon_swap_resist_fear;
extern int  armour_swap_slay_animal;
extern int  armour_swap_slay_evil;
extern int  armour_swap_slay_undead;
extern int  armour_swap_slay_demon;
extern int  armour_swap_slay_orc;
extern int  armour_swap_slay_troll;
extern int  armour_swap_slay_giant;
extern int  armour_swap_slay_dragon;
extern int  armour_swap_kill_dragon;
extern int  armour_swap_impact;
extern int  armour_swap_brand_acid;
extern int  armour_swap_brand_elec;
extern int  armour_swap_brand_fire;
extern int  armour_swap_brand_cold;
extern int  armour_swap_see_infra;
extern int  armour_swap_slow_digest;
extern int  armour_swap_aggravate;
extern int  armour_swap_teleport;
extern int  armour_swap_regenerate;
extern int  armour_swap_telepathy;
extern int  armour_swap_lite;
extern int  armour_swap_see_invis;
extern int  armour_swap_ffall;
extern int  armour_swap_free_act;
extern int  armour_swap_hold_life;
extern int  armour_swap_immune_fire;
extern int  armour_swap_immune_acid;
extern int  armour_swap_immune_cold;
extern int  armour_swap_immune_elec;
extern int  armour_swap_resist_acid;
extern int  armour_swap_resist_elec;
extern int  armour_swap_resist_fire;
extern int  armour_swap_resist_cold;
extern int  armour_swap_resist_pois;
extern int  armour_swap_resist_conf;
extern int  armour_swap_resist_sound;
extern int  armour_swap_resist_lite;
extern int  armour_swap_resist_dark;
extern int  armour_swap_resist_chaos;
extern int  armour_swap_resist_disen;
extern int  armour_swap_resist_shard;
extern int  armour_swap_resist_nexus;
extern int  armour_swap_resist_blind;
extern int  armour_swap_resist_neth;
extern int  armour_swap_resist_fear;

extern int my_ammo_tval;   /* Ammo -- "tval"   */
extern int my_ammo_sides;  /* Ammo -- "sides"  */
extern s16b my_ammo_power;  /* Shooting multipler   */
extern s16b my_ammo_range;  /* Shooting range   */

extern s16b my_need_enchant_to_a;   /* Need some enchantment */
extern s16b my_need_enchant_to_h;   /* Need some enchantment */
extern s16b my_need_enchant_to_d;   /* Need some enchantment */
extern s16b my_need_brand_weapon;  /* apw brand bolts */


/*
 * Hack -- basic "power"
 */

extern s32b my_power;


/*
 * Various "amounts" (for the player)
 */

extern s16b amt_phase;
extern s16b amt_food_lowcal;
extern s16b amt_food_hical;

extern s16b amt_slow_poison;
extern s16b amt_cure_confusion;
extern s16b amt_cure_blind;

extern s16b amt_cool_staff;  /* holiness-power staff */

extern s16b amt_book[7][4]; /* [realm][sval] */

extern s16b amt_add_stat[6];
extern s16b amt_fix_stat[7];

extern s16b amt_fix_exp;

extern s16b amt_enchant_to_a;
extern s16b amt_enchant_to_d;
extern s16b amt_enchant_to_h;
extern s16b amt_brand_weapon;  /* cubragol and bolts */
extern s16b amt_enchant_weapon;
extern s16b amt_enchant_armor;
extern s16b amt_digger;

/*
 * Various "amounts" (for the home)
 */

extern int num_food;
extern int num_mold;
extern int num_ident;
extern int num_star_ident;
extern int num_recall;
extern int num_phase;
extern int num_escape;
extern int num_teleport;
extern int num_berserk;
extern int num_teleport_level;

extern int num_cure_critical;
extern int num_cure_serious;

extern int num_pot_rheat;
extern int num_pot_rcold;

extern int num_missile;

extern int num_book[7][4];

extern int num_fix_stat[7];

extern int num_fix_exp;
extern int num_mana;
extern int num_heal;
extern int num_heal_true;
extern int num_ez_heal;
extern int num_ez_heal_true;
extern int num_pfe;
extern int num_glyph;
extern int num_speed;
extern int num_goi_pot;
extern int num_resist_pot;

extern int num_enchant_to_a;
extern int num_enchant_to_d;
extern int num_enchant_to_h;
extern int num_brand_weapon;  /* apw crubragol and bolts */
extern int num_genocide;
extern int num_mass_genocide;

extern int num_artifact;

extern s16b home_slot_free;
extern s16b home_damage;
extern s16b num_duplicate_items;
extern int num_slow_digest;
extern int num_regenerate;
extern int num_telepathy;
extern int num_lite;
extern int num_see_inv;

extern int num_invisible; /*apw*/

extern int num_ffall;
extern int num_free_act;
extern int num_hold_life;
extern int num_immune_acid;
extern int num_immune_elec;
extern int num_immune_fire;
extern int num_immune_cold;
extern int num_resist_acid;
extern int num_resist_elec;
extern int num_resist_fire;
extern int num_resist_cold;
extern int num_resist_pois;
extern int num_resist_conf;
extern int num_resist_sound;
extern int num_resist_lite;
extern int num_resist_dark;
extern int num_resist_chaos;
extern int num_resist_disen;
extern int num_resist_shard;
extern int num_resist_nexus;
extern int num_resist_blind;
extern int num_resist_neth;
extern int num_sustain_str;
extern int num_sustain_int;
extern int num_sustain_wis;
extern int num_sustain_dex;
extern int num_sustain_con;
extern int num_sustain_all;

extern int num_speed;
extern int num_edged_weapon;
extern int num_bad_gloves;
extern int num_weapons;
extern int num_bow;
extern int num_rings;
extern int num_neck;
extern int num_armor;
extern int num_cloaks;
extern int num_shields;
extern int num_hats;
extern int num_gloves;
extern int num_boots;


/*
 * Hack -- extra state variables
 */

extern int borg_feeling;    /* Current level "feeling" */

/*
 * Hack -- current shop index
 */

extern s16b shop_num;       /* Current shop index */



/*
 * State variables extracted from the screen
 */

extern s32b borg_exp;       /* Current experience */

extern s32b borg_gold;      /* Current gold */

extern int borg_stat[6];    /* Current stats */

extern int borg_book[8][4];   /* Current book slots, Realm,sval */


/*
 * State variables extracted from the inventory/equipment
 */

extern int borg_cur_wgt;    /* Current weight */


/*
 * Constant state variables
 */

extern int borg_race;       /* Current race */
extern int borg_class;      /* Current class */



/*
 * Constant state structures
 */

extern player_race *rb_ptr; /* Player race info */
extern player_class *cb_ptr;    /* Player class info */
extern player_magic *mb_ptr;    /* Player magic info */


/*
 * Number of turns to step for (zero means forever)
 */
extern u16b borg_step;      /* Step count (if any) */

extern void borgmove2(int *y, int *x, int y1, int x1, int y2, int x2);


/*
 * Status message search string
 */
extern char borg_match[128];    /* Search string */


/*
 * Log file
 */
extern FILE *borg_fff;      /* Log file */


/*
 * Hack -- single character constants
 */

extern const char p1, p2, c1, c2, b1, b2;


/*
 * Hack -- the detection arrays
 */

extern bool borg_detect_wall[6][6];

extern bool borg_detect_trap[6][6];

extern bool borg_detect_door[6][6];

extern bool borg_detect_evil[6][6];

/*
 * Locate the store doors
 */

extern byte *track_shop_x;
extern byte *track_shop_y;

extern byte *track_quest_x;
extern byte *track_quest_y;


/*
 * Track "stairs up"
 */

extern s16b track_less_num;
extern s16b track_less_size;
extern byte *track_less_x;
extern byte *track_less_y;


/*
 * Track "stairs down"
 */

extern s16b track_more_num;
extern s16b track_more_size;
extern byte *track_more_x;
extern byte *track_more_y;

/*
 * Track glyphs
 */
extern s16b track_glyph_num;
extern s16b track_glyph_size;
extern byte *track_glyph_x;
extern byte *track_glyph_y;

/*
 * Track steps
 */
extern s16b track_step_num;
extern s16b track_step_size;
extern byte *track_step_x;
extern byte *track_step_y;

/*
 * Track closed doors
 */
extern s16b track_door_num;
extern s16b track_door_size;
extern byte *track_door_x;
extern byte *track_door_y;

/*
 * The object list.  This list is used to "track" objects.
 */

extern s16b borg_takes_cnt;

extern s16b borg_takes_nxt;

extern borg_take *borg_takes;


/*
 * The monster list.  This list is used to "track" monsters.
 */

extern s16b borg_kills_cnt;
extern s16b borg_kills_summoner;   /* index of a summoning guy */
extern s16b borg_kills_nxt;

extern borg_kill *borg_kills;


/*
 * Hack -- depth readiness
 */
extern int borg_ready_morgoth;

/*
 * Hack -- extra fear per "region"
 */

extern u16b borg_fear_region[6][18];


/*
 * Hack -- count racial appearances per level
 */

extern s16b *borg_race_count;


/*
 * Hack -- count racial kills (for uniques)
 */

extern s16b *borg_race_death;


/*
 * Classification of map symbols
 */

extern bool borg_is_take[256];      /* Symbol may be an object */

extern bool borg_is_kill[256];      /* Symbol may be a monster */


/*
 * Current "grid" list
 */

extern borg_grid *borg_grids[AUTO_MAX_Y];   /* Current "grid list" */



/*
 * Maintain a set of grids (liteable grids)
 */

extern s16b borg_lite_n;
extern byte borg_lite_y[AUTO_LITE_MAX];
extern byte borg_lite_x[AUTO_LITE_MAX];

/*
 * Maintain a set of glow grids (liteable grids)
 */

extern s16b borg_glow_n;
extern byte borg_glow_y[AUTO_LITE_MAX];
extern byte borg_glow_x[AUTO_LITE_MAX];


/*
 * Maintain a set of grids (viewable grids)
 */

extern s16b borg_view_n;
extern byte borg_view_y[AUTO_VIEW_MAX];
extern byte borg_view_x[AUTO_VIEW_MAX];


/*
 * Maintain a set of grids (scanning arrays)
 */

extern s16b borg_temp_n;
extern byte borg_temp_y[AUTO_TEMP_MAX];
extern byte borg_temp_x[AUTO_TEMP_MAX];

extern byte offset_y;
extern byte offset_x;


/*
 * Maintain a set of grids (flow calculations)
 */

extern s16b borg_flow_n;
extern byte borg_flow_y[AUTO_FLOW_MAX];
extern byte borg_flow_x[AUTO_FLOW_MAX];


/*
 * Hack -- use "flow" array as a queue
 */

extern int flow_head;
extern int flow_tail;


/*
 * Some variables
 */

extern borg_data *borg_data_flow;   /* Current "flow" data */

extern borg_data *borg_data_cost;   /* Current "cost" data */

extern borg_data *borg_data_hard;   /* Constant "hard" data */

extern borg_data *borg_data_know;   /* Current "know" flags */

extern borg_data *borg_data_icky;   /* Current "icky" flags */


/*
 * Strategy flags -- recalculate things
 */

extern bool borg_danger_wipe;       /* Recalculate danger */

extern bool borg_do_update_view;       /* Recalculate view */

extern bool borg_do_update_lite;       /* Recalculate lite */


/*
 * Strategy flags -- examine the world
 */

extern bool borg_do_inven;      /* Acquire "inven" info */

extern bool borg_do_equip;      /* Acquire "equip" info */

extern bool borg_do_panel;      /* Acquire "panel" info */

extern bool borg_do_frame;      /* Acquire "frame" info */

extern bool borg_do_spell;      /* Acquire "spell" info */

extern bool borg_do_browse;     /* Acquire "store" info */

extern byte borg_do_browse_what;    /* Hack -- store for "borg_do_browse" */

extern byte borg_do_browse_more;    /* Hack -- pages for "borg_do_browse" */


/*
 * Strategy flags -- run certain functions
 */

extern bool borg_do_crush_junk;

extern bool borg_do_crush_hole;

extern bool borg_do_crush_slow;

/* am I fighting a unique */
extern int borg_fighting_unique;
extern bool borg_fighting_evil_unique;

/* am I fighting a summoner */
extern bool borg_fighting_summoner;



/*** Some functions ***/

/*
 * Queue a keypress
 */
extern errr borg_keypress(char k);

/*
 * Queue several keypresses
 */
extern errr borg_keypresses(cptr str);

/*
 * Dequeue a keypress
 */
extern char borg_inkey(bool take);

/*
 * Flush the keypresses
 */
extern void borg_flush(void);


/*
 * Obtain some text from the screen (single character)
 */
extern errr borg_what_char(int x, int y, byte *a, char *c);

/*
 * Obtain some text from the screen (multiple characters)
 */
extern errr borg_what_text(int x, int y, int n, byte *a, char *s);


/*
 * Log a message to a file
 */
extern void borg_info(cptr what);

/*
 * Log a message, Search it, and Show/Memorize it in pieces
 */
extern void borg_note(cptr what);


/*
 * Abort the Borg, noting the reason
 */
extern void borg_oops(cptr what);


/*
 * Take a "memory note"
 */
extern bool borg_tell(cptr what);

/*
 * Change the player name
 */
extern bool borg_change_name(cptr str);

/*
 * Dump a character description
 */
extern bool borg_dump_character(cptr str);

/*
 * Save the game (but do not quit)
 */
extern bool borg_save_game(void);


/*
 * Update the "frame" info from the screen
 */
extern void borg_update_frame(void);

/*
 * Calc a formula out in RPN
 */
extern int borg_calc_formula(int *);
/*
 * check out a formula in RPN
 */
extern int borg_check_formula(int *);
/*
 * return a string for the formula
 */
extern cptr borg_prt_formula(int *formula);

/*
 * Print the string for an item
 */
extern cptr borg_prt_item(int item);

/*
 * Initialize this file
 */
extern void borg_init_1(void);

#ifdef ALLOW_BORG_GRAPHICS

typedef struct glyph
{
   byte d_attr;        /* Attribute */
   char d_char;        /* Character */
} glyph;


extern glyph translate_visuals[255][255];

#endif /* ALLOW_BORG_GRAPHICS */

#endif

#endif
