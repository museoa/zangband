
/* File: zborg1.h */
/* Purpose: Header file for "borg1.c" -BEN- */

#ifndef INCLUDED_BORG1_H
#define INCLUDED_BORG1_H

#include "angband.h"
#include "maid-grf.h"



#ifdef ALLOW_BORG


/* WARNING: If you mess with the objects.txt or the monster.txt and change
 * the locations of things, then you must make those changes in zborg.txt
 * as well as in this borg code.  The borg is very concerned about item
 * index locations.  ie: borgs_has[242] is looking for a Potion of Healing.
 * The borg is concerned over several items, broken swords, and several
 * monster locations (Tarresque, Sauron, Morgoth).
 *
 * This is a massive hack and must be fixed -SF-
 */

/*
 * This file provides support for "borg1.c".
 */



/*** Some constants ***/

/*
 * Possible values of "goal"
 */
#define GOAL_KILL   1			/* Monsters */
#define GOAL_TAKE   2			/* Objects */
#define GOAL_MISC   3			/* Stores */
#define GOAL_DARK   4			/* Exploring */
#define GOAL_XTRA   5			/* Searching */
#define GOAL_BORE   6			/* Leaving */
#define GOAL_FLEE   7			/* Fleeing */
#define GOAL_TOWN	8			/* Town Special Grid */


/*
 * Flags for the "info" field of grids
 *
 * "BORG_MAP_VIEW"  This is the equivalent of GRID_VIEW,
 * calculated with the best information available to the
 * borg.
 *
 * "BORG_MAP_ICKY"  These are grids not to flow over.
 *
 * "BORG_MAP_KNOW"  This marks grids already in the flow,
 * and already tested for 'ickyness'.  This is done for
 * efficiency.
 */
#define BORG_MAP_VIEW   0x01	/* in line of sight */
#define BORG_MAP_ICKY	0x02	/* grids to avoid */
#define BORG_MAP_KNOW	0x04	/* 'know' grids */



/* Flags used to mark detection */
#define BORG_DETECT_TRAP	0x01
#define BORG_DETECT_DOOR	0x02
#define BORG_DETECT_WALL	0x04
#define BORG_DETECT_EVIL	0x08


/*
 * Borg detection radius.
 *
 * This is smaller than the actual detection radius because
 * we don't want the borg to walk into undetected regions.
 */
#define BORG_MAX_DETECT		(MAX_DETECT - 2)

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
#define GF_DISP_UNDEAD_DEMON 100	/* effect both */
#define GF_ELEMENTS       101	/* all elements could be cast */
#define GF_DEATHRAY      102

/* the Z randarts are considered #127 by the borg */
#define ART_RANDART  127



/*
 * Maximum size of the "view" array
 */
#define AUTO_VIEW_MAX 1536


/*
 * Number of grids in the "temp" array
 */
#define AUTO_TEMP_MAX 1536


/*
 * Number of grids in the "flow" array
 */
#define AUTO_FLOW_MAX 2000



/*
 * Enable the "borg_note()" usage of the Recall Window
 * Also specify the number of "rolling rows" to use
 */
#define BORG_NOTE_ROWS      12



/*
 * Size of Keypress buffer
 */
#define KEY_SIZE 8192

/* Maximum number of 'takes' */
#define BORG_TAKES_MAX	1024
#define BORG_KILLS_MAX	1024



/*
 * Object information
 */

typedef struct borg_take borg_take;

struct borg_take
{
	s16b k_idx;	/* Kind index */

	byte x, y;	/* Location */

	char unknown;	/* Unknown type */
};


/*
 * Monster information
 */

typedef struct borg_kill borg_kill;

struct borg_kill
{
	s16b r_idx;	/* Race index */

	s16b power;	/* Estimated hit-points */
	s32b when;	/* When last seen */

	byte x, y;	/* Location */

	bool ranged_attack;	/* can attack from a dx */

	byte m_flags;

	byte type;	/* Type of kill */
};


/*
 * A store
 */

typedef struct borg_shop borg_shop;

struct borg_shop
{
	/* Location */
	int x;
	int y;

	/* Time stamp */
	s32b when;

	/* Is this shop useful? */
	s16b b_count;
	s16b u_count;
};


/*** Some variables ***/


/*
 * Some variables
 */
extern bool borg_active;	/* Actually active */
extern bool borg_resurrect;	/* Continous play mode */
extern bool borg_cancel;	/* Being cancelled */

extern bool borg_stop_king;
extern bool borg_dont_react;
extern int successful_target;

extern bool borg_plays_risky;
extern bool borg_scums_uniques;

/* HACK... this should really be a parm into borg_prepared */
/*         I am just being lazy */
extern bool borg_slow_return;

/* Borg has is intrinsically broken */
extern int *borg_has;
extern int *borg_has_on;
extern int *borg_artifact;
extern int *borg_skill;
extern int size_depth;
extern int size_obj;



#define BI_STR		0
#define BI_INT		1
#define BI_WIS		2
#define BI_DEX		3
#define BI_CON		4
#define BI_CHR		5
#define BI_CSTR		6
#define BI_CINT		7
#define BI_CWIS		8
#define BI_CDEX		9
#define BI_CCON		10
#define BI_CCHR		11
#define BI_SSTR		12
#define BI_SINT		13
#define BI_SWIS		14
#define BI_SDEX		15
#define BI_SCON		16
#define BI_SCHR		17
#define BI_INTMANA	18
#define BI_WISMANA	19
#define BI_LITE		20
#define BI_CURHP	21
#define BI_MAXHP	22
#define BI_HP_ADJ	23
#define BI_OLDCHP	24
#define BI_CURSP	25
#define BI_MAXSP	26
#define BI_SP_ADJ	27
#define BI_OLDCSP	28
#define BI_FAIL1	29
#define BI_FAIL2	30
#define BI_REALM1	31
#define BI_REALM2	32
#define BI_CLEVEL	33
#define BI_MAXCLEVEL	34
#define BI_ESP		35
#define BI_CUR_LITE	36
#define BI_RECALL	37
#define BI_FOOD		38
#define BI_SPEED	39
#define BI_SDIG		40
#define BI_FEATH	41
#define BI_REG		42
#define BI_SINV		43
#define BI_INFRA	44
#define BI_DIS		45
#define BI_DEV		46
#define BI_SAV		47
#define BI_STL		48
#define BI_SRCH		49
#define BI_SRCHFREQ	50
#define BI_THN		51
#define BI_THB		52
#define BI_THT		53
#define BI_DIG		54
#define BI_IFIRE	55
#define BI_IACID	56
#define BI_ICOLD	57
#define BI_IELEC	58
#define BI_RFIRE	59
#define BI_RCOLD	60
#define BI_RELEC	61
#define BI_RACID	62
#define BI_RPOIS	63
#define BI_RFEAR	64
#define BI_RLITE	65
#define BI_RDARK	66
#define BI_RBLIND	67
#define BI_RCONF	68
#define BI_RSND	 	69
#define BI_RSHRD	70
#define BI_RNXUS	71
#define BI_RNTHR	72
#define BI_RKAOS	73
#define BI_RDIS		74
#define BI_REFLECT	75
#define BI_HLIFE	76
#define BI_FRACT	77
#define BI_SRFIRE	78
#define BI_SRCOLD	79
#define BI_SRELEC	80
#define BI_SRACID	81
#define BI_SRPOIS	82
#define BI_SRFEAR	83
#define BI_SRLITE	84
#define BI_SRDARK	85
#define BI_SRBLIND	86
#define BI_SRCONF	87
#define BI_SRSND	88
#define BI_SRSHRD	89
#define BI_SRNXUS	90
#define BI_SRNTHR	91
#define BI_SRKAOS	92
#define BI_SRDIS	93
#define BI_SHLIFE	94
#define BI_SFRACT	95
#define BI_DEPTH	96
#define BI_CDEPTH	97
#define BI_MAXDEPTH	98
#define BI_KING		99

#define BI_ISWEAK 	100
#define BI_ISHUNGRY	101
#define BI_ISFULL	102
#define BI_ISGORGED	103
#define BI_ISBLIND	104
#define BI_ISAFRAID	105
#define BI_ISCONFUSED	106
#define BI_ISPOISONED	107
#define BI_ISCUT	108
#define BI_ISSTUN	109
#define BI_ISHEAVYSTUN	110
#define BI_ISIMAGE	111
#define BI_ISSTUDY	112
#define BI_ISSEARCHING	113
#define BI_ISFIXLEV	114
#define BI_ISFIXEXP	115
#define BI_ISFIXSTR	116
#define BI_ISFIXINT	117
#define BI_ISFIXWIS	118
#define BI_ISFIXDEX	119
#define BI_ISFIXCON	120
#define BI_ISFIXCHR	121
#define BI_ISFIXALL	122
#define BI_HRTIME	123
#define BI_MNTIME	124

#define BI_ARMOR	125
#define BI_TOHIT	126
#define BI_TODAM	127
#define BI_WTOHIT	128
#define BI_WTODAM	129
#define BI_BTOHIT	130
#define BI_BTODAM	131
#define BI_BLOWS	132
#define BI_SHOTS	133
#define BI_WMAXDAM	134
#define BI_WBASEDAM	135
#define BI_BMAXDAM	136
#define BI_HEAVYWEPON	137
#define BI_HEAVYBOW	138
#define BI_CRSTELE	139
#define BI_CRSAGRV	140
#define BI_CRSTY	141
#define BI_CRSNOTELE	142		/* no teleport */
#define BI_CRSNOMAGIC	143		/* no magic */
#define BI_ENCUMBERD	144
#define BI_FEAR_LITE	145
#define BI_WS_ANIMAL	146
#define BI_WS_EVIL	147
#define BI_WS_UNDEAD	148
#define BI_WS_DEMON	149
#define BI_WS_ORC	150
#define BI_WS_TROLL	151
#define BI_WS_GIANT	152
#define BI_WS_DRAGON	153
#define BI_WK_DRAGON	154
#define BI_W_IMPACT	155
#define BI_WB_ACID	156
#define BI_WB_ELEC	157
#define BI_WB_FIRE	158
#define BI_WB_COLD	159
#define BI_WB_POIS	160
#define BI_WB_VORPAL	161
#define BI_WB_VAMPIRIC	162
#define BI_WB_CHAOTIC	163
#define BI_ATELEPORT	164
#define BI_AESCAPE	165
#define BI_AFUEL	166
#define BI_AHEAL	167
#define BI_AEZHEAL	168
#define BI_AID		169
#define BI_ASPEED	170
#define BI_ASTFMAGI	171
#define BI_ASTFDEST	172
#define BI_AMISSILES	173
#define BI_ACUREPOIS	174
#define BI_ADETTRAP	175
#define BI_ADETDOOR	176
#define BI_ADETEVIL	177
#define BI_AMAGICMAP	178
#define BI_ARECHARGE	179
#define BI_APFE		180
#define BI_AGLYPH	181
#define BI_ACCW		182
#define BI_ACSW		183
#define BI_ARESHEAT	184
#define BI_ARESCOLD	185
#define BI_ATELEPORTLVL	186		/* scroll of teleport level */
#define BI_AXGOI	187			/* reliable GOI spell */
#define BI_AGOI		188			/* GOI spell */
#define BI_AHWORD	189			/* Holy Word prayer */

#define BI_WEIGHT	190
#define BI_MAX		191


/*
 * Various silly flags
 */

extern bool borg_flag_save;	/* Save savefile at each level */

extern bool borg_flag_dump;	/* Save savefile at each death */

extern bool borg_save;	/* do a save next time we get to press a key! */

extern bool borg_borg_message;	/* List borg messages or not */
extern bool borg_confirm_target;

/*
 * Use a simple internal random number generator
 */
extern u32b borg_rand_local;	/* Save personal setting */


/*
 * Hack -- time variables
 */

extern s32b borg_t;	/* Current "time" */
extern s32b need_see_inviso;	/* To tell me to cast it */
extern s32b borg_see_inv;
extern bool vault_on_level;	/* borg will search for a vault */
extern bool unique_on_level;
extern bool scaryguy_on_level;

extern bool breeder_level;	/* Borg will shut doors */
extern s16b old_depth;
extern s16b borg_no_retreat;

/*
 * Hack -- Other time variables
 */

extern s32b when_call_lite;	/* When we last did call light */
extern s32b when_wizard_lite;	/* When we last did wizard light */

extern s32b when_detect_traps;	/* When we last detected traps */
extern s32b when_detect_doors;	/* When we last detected doors */
extern s32b when_detect_walls;	/* When we last detected walls */
extern s32b when_detect_evil;

extern bool my_need_alter;	/* incase of walls/doors */
extern bool my_no_alter;	/* incase of walls/doors */
extern bool borg_attempting_refresh;	/* for the goi spell */

/*
 * Some information
 */

extern s16b goal;	/* Flowing (goal type) */

extern bool goal_rising;	/* Currently returning to town */

extern bool goal_leaving;	/* Currently leaving the level */

extern bool goal_fleeing;	/* Currently fleeing the level */

extern bool goal_ignoring;	/* Currently ignoring monsters */

extern int goal_recalling;	/* Currently waiting for recall, guessing turns left */
extern bool goal_less;	/* return to, but dont use, the next up stairs */

extern s16b borg_times_twitch;	/* how often twitchy on this level */
extern s16b borg_escapes;	/* how often teleported on this level */

extern bool stair_less;	/* Use the next "up" staircase */
extern bool stair_more;	/* Use the next "down" staircase */

extern s32b borg_began;	/* When this level began */
extern s32b borg_time_town;	/* how long it has been since I was in town */

extern s16b avoidance;	/* Current danger thresh-hold */

extern bool borg_failure;	/* Notice failure */

extern bool borg_simulate;	/* Simulation flag */
extern bool borg_attacking;	/* Are we attacking a monster? */
extern bool borg_offsetting;	/* Are we attacking a monster? with offsett balls */

extern bool borg_completed;	/* Completed the level */
extern bool borg_needs_searching;	/* borg will search with each step */
extern bool borg_full_damage;	/* make danger = full possible damage. */

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
extern bool borg_on_glyph;	/* borg is standing on a glyph of warding */
extern bool borg_create_door;	/* borg is going to create doors */
extern bool borg_sleep_spell;
extern bool borg_sleep_spell_ii;
extern bool borg_slow_spell;
extern bool borg_confuse_spell;
extern bool borg_fear_mon_spell;


/*
 * Shop goals
 */

extern s16b goal_shop;	/* Next shop to visit */

/*
 * Hack -- current shop index
 */
extern s16b shop_num;

/* Current "shops" */
extern borg_shop *borg_shops;

/*
 * Number of allocated stores...
 */
extern s16b track_shop_num;
extern s16b track_shop_size;


/*
 * Other variables
 */

extern int c_x;	/* Current location (X) */
extern int c_y;	/* Current location (Y) */

extern int g_x;	/* Goal location (X) */
extern int g_y;	/* Goal location (Y) */

extern int dim_door_y;	/* Safe landing zone for DDoor */
extern int dim_door_x;

extern int bad_obj_x[50];	/* Dropped cursed artifact at location (X) */
extern int bad_obj_y[50];	/* Dropped cursed artifact at location (Y) */

/*
 * Some estimated state variables
 */

extern s16b my_stat_max[6];	/* Current "maximal" stat values    */
extern s16b my_stat_cur[6];	/* Current "natural" stat values    */
extern s16b my_stat_use[6];	/* Current "resulting" stat values  */
extern s16b my_stat_ind[6];	/* Current "additions" to stat values   */
extern bool my_need_stat_check[6];	/* do I need to check my stats */

extern s16b my_stat_add[6];	/* aditions to stats */

extern s16b home_stat_add[6];

extern bool borg_wearing_cursed;

extern s16b weapon_swap_digger;

extern int my_ammo_tval;	/* Ammo -- "tval"   */
extern int my_ammo_sides;	/* Ammo -- "sides"  */
extern s16b my_ammo_power;	/* Shooting multipler   */
extern s16b my_ammo_range;	/* Shooting range   */

extern s16b my_need_enchant_to_a;	/* Need some enchantment */
extern s16b my_need_enchant_to_h;	/* Need some enchantment */
extern s16b my_need_enchant_to_d;	/* Need some enchantment */


/*
 * Various "amounts" (for the player)
 */

extern s16b amt_phase;
extern s16b amt_food_lowcal;
extern s16b amt_food_hical;

extern s16b amt_slow_poison;
extern s16b amt_cure_confusion;
extern s16b amt_cure_blind;

extern s16b amt_cool_staff;	/* holiness-power staff */

extern s16b amt_book[8][4];	/* [realm][sval] */

extern s16b amt_add_stat[6];
extern s16b amt_fix_stat[7];

extern s16b amt_fix_exp;

extern s16b amt_enchant_to_a;
extern s16b amt_enchant_to_d;
extern s16b amt_enchant_to_h;
extern s16b amt_brand_weapon;	/* cubragol and bolts */
extern s16b amt_enchant_weapon;
extern s16b amt_enchant_armor;
extern s16b amt_digger;


/*
 * Hack -- extra state variables
 */

extern int borg_feeling;	/* Current level "feeling" */




/*
 * State variables extracted from the screen
 */

extern s32b borg_exp;	/* Current experience */

extern s32b borg_gold;	/* Current gold */

extern int borg_stat[6];	/* Current stats */

extern int borg_book[8][4];	/* Current book slots, Realm,sval */



/*
 * Constant state variables
 */

extern int borg_race;	/* Current race */
extern int borg_class;	/* Current class */



/*
 * Constant state structures
 */

extern player_race *rb_ptr;	/* Player race info */
extern player_class *cb_ptr;	/* Player class info */
extern player_magic *pmb_ptr;	/* Player magic info */


/*
 * Number of turns to step for (zero means forever)
 */
extern u16b borg_step;	/* Step count (if any) */

/*
 * Status message search string
 */
extern char borg_match[128];	/* Search string */


/*
 * Log file
 */
extern FILE *borg_fff;	/* Log file */


/*
 * Track "stairs up"
 */
extern s16b track_less_num;
extern s16b track_less_size;
extern int *track_less_x;
extern int *track_less_y;


/*
 * Track "stairs down"
 */
extern s16b track_more_num;
extern s16b track_more_size;
extern int *track_more_x;
extern int *track_more_y;

/*
 * Track glyphs
 */
extern s16b track_glyph_num;
extern s16b track_glyph_size;
extern int *track_glyph_x;
extern int *track_glyph_y;

/*
 * Track steps
 */
extern s16b track_step_num;
extern s16b track_step_size;
extern int *track_step_x;
extern int *track_step_y;

/*
 * Track closed doors
 */
extern s16b track_door_num;
extern s16b track_door_size;
extern int *track_door_x;
extern int *track_door_y;

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
extern s16b borg_kills_nxt;

extern borg_kill *borg_kills;

/*
 * Hack -- count racial appearances per level
 */

extern s16b *borg_race_count;


/*
 * Maintain a set of glow grids (liteable grids)
 */
extern s16b borg_glow_n;
extern s16b borg_glow_y[AUTO_LITE_MAX];
extern s16b borg_glow_x[AUTO_LITE_MAX];


/*
 * Maintain a set of grids (viewable grids)
 */

extern s16b borg_view_n;
extern s16b borg_view_y[AUTO_VIEW_MAX];
extern s16b borg_view_x[AUTO_VIEW_MAX];


/*
 * Maintain a set of grids (scanning arrays)
 */
extern s16b borg_temp_n;
extern s16b borg_temp_y[AUTO_TEMP_MAX];
extern s16b borg_temp_x[AUTO_TEMP_MAX];


/*
 * Maintain a set of grids (flow calculations)
 */
extern s16b borg_flow_n;
extern s16b borg_flow_y[AUTO_FLOW_MAX];
extern s16b borg_flow_x[AUTO_FLOW_MAX];


/*
 * Hack -- use "flow" array as a queue
 */
extern int flow_head;
extern int flow_tail;


/*
 * Strategy flags -- examine the world
 */
extern bool borg_do_frame;	/* Acquire "frame" info */

extern bool borg_do_spell;	/* Acquire "spell" info */


/*
 * Strategy flags -- run certain functions
 */

extern bool borg_do_crush_junk;

extern bool borg_do_crush_hole;

extern bool borg_do_crush_slow;

/* am I fighting a unique */
extern int borg_fighting_unique;
extern bool borg_fighting_evil_unique;


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
 * Initialize this file
 */
extern void borg_init_1(void);

#endif

#endif
