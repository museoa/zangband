/* File: borg1.c */
/* Purpose: Low level stuff for the Borg -BEN- */
#include "angband.h"


#ifdef ALLOW_BORG

#include "zborg1.h"

/*
 * using 2 rings of Light Res.
 * does not know anti-teleport.
 * if a borg has no light and no money to buy fuel, sell some
 *    in order to raise cash.
 * Zborg.txt needs support for quest numbers on respawn
 *    and verify the quest numbers on from respawn.
 *    I dont know if the same random Quests are generated
 *	  for each respawned borg, if they are reset, or even
 *    marked as done.  Might need to watch it.
 * Inscription problem with long art names
 * learning - of prayers.
 *
 * Mimic Doors.
 *
 * mindcrafters need to be able to sell non ps-id items if they
 *   are low level and do not have ID or ps-id.
 * getting to a wilderness grid that has a town.
 *   using waypoints?
 * Using dimdoor to get accross lava and water.
 * Mutations can be activated.
 */

/* Needs code:
 *    _QUESTITEM
 * TR3_NO_TELE
 * TR3_NO_MAGIC
 * TR3_TY_CURSE
 */

/* Things that would be nice for the borg to know but would
 * require a bit of programming and may not be worth the time.
 *
 * - Aquatic animals stay in aquatic realms.
 * - Wraithform
 * - Summoning
 * - Fixed Quests
 */

/*
 * This file contains various low level variables and routines.
 */

/* Date of the last change */
char borg_engine_date[] = __DATE__;

/*
 * Borg information, ScreenSaver or continual play mode;
 */
int borg_respawn_race;
int borg_respawn_class;
int borg_respawn_str;
int borg_respawn_int;
int borg_respawn_wis;
int borg_respawn_dex;
int borg_respawn_con;
int borg_respawn_chr;
int borg_dump_level;
bool borg_respawn_winners;
int borg_delay_factor;

/* dynamic borg stuff */
bool borg_uses_swaps;
bool borg_uses_calcs = TRUE;
bool borg_worships_damage;
bool borg_worships_speed;
bool borg_worships_hp;
bool borg_worships_mana;
bool borg_worships_ac;
bool borg_worships_gold;
bool borg_plays_risky;
bool borg_slow_optimizehome;
bool borg_scums_uniques;
bool borg_engage_cloak;
int borg_chest_fail_tolerance;
int borg_money_scum_amount;
bool borg_scums_money;

/* HACK... this should really be a parm into borg_prepared */
/*         I am just being lazy */
bool borg_slow_return = FALSE;

req_item *borg_required_item[MAX_CLASS];
int n_req[MAX_CLASS];
power_item *borg_power_item[MAX_CLASS];
int n_pwr[MAX_CLASS];

int *borg_has;
int *borg_has_on;
int *borg_artifact;
int *borg_skill;
int size_class;
int size_depth;
int size_obj;
int *formula[1000];

/* k_max for Z is now 582,
 * a_max for Z is now 128.
 */
cptr prefix_pref[] =
{
/* personal attributes */
    "_STR", /* ,has[1292] */
    "_INT",
    "_WIS",
    "_DEX",
    "_CON",
    "_CHR",
    "_CSTR",
    "_CINT",
    "_CWIS",
    "_CDEX",
    "_CCON",
    "_CCHR",
    "_SSTR",
    "_SINT",
    "_SWIS",
    "_SDEX",
    "_SCON",
    "_SCHR",
    "_INTMANA",
    "_WISMANA", /* 20 */
    "_LITE",
    "_CURHP",
    "_MAXHP",
    "_ADJHP",
    "_OLDCHP",
    "_CURSP",
    "_MAXSP",
    "_ADJSP",
    "_OLDCSP",
    "_SFAIL1", /* 30, */
    "_SFAIL2",
    "_REALM1",
    "_REALM2",
    "_CLEVEL",
    "_MAXCLEVEL",
    "_ESP",
    "_LITRAD",
    "_RECALL",
    "_FOOD",  /* 29 */
    "_SPEED",
    "_SDIG",
    "_FEATH",
    "_REG",
    "_SINV",
    "_INFRA",
    "_DIS",
    "_DEV",
    "_SAV",
    "_STL",
    "_SRCH",
    "_SERCHFREQ",
    "_THN",
    "_THB",
    "_THT",
    "_DIG",
    "_IFIRE",
    "_IACID",
    "_ICOLD",
    "_IELEC",
    "_RFIRE",
    "_RCOLD",
    "_RELEC",
    "_RACID",
    "_RPOIS",
    "_RFEAR",
    "_RLITE",
    "_RDARK",
    "_RBLIND",
    "_RCONF",
    "_RSND",
    "_RSHRD",
    "_RNXUS",
    "_RNTHR",
    "_RKAOS",
    "_RDIS",
    "_REFLECT",
    "_HLIFE",
    "_FRACT",
    "_SRFIRE", /* same as without S but includes swap */
    "_SRCOLD",
    "_SRELEC",
    "_SRACID",
    "_SRPOIS",
    "_SRFEAR",
    "_SRLITE",
    "_SRDARK",
    "_SRBLIND",
    "_SRCONF",
    "_SRSND",
    "_SRSHRD",
    "_SRNXUS",
    "_SRNTHR",
    "_SRKAOS",
    "_SRDIS",
    "_SHLIFE",
    "_SFRACT",

/* random extra variable */
    "_DEPTH",  /* current depth being tested */
    "_CDEPTH", /* borgs current depth */
    "_MAXDEPTH", /* recall depth */
    "_KING",    /* borg has won */

/* player state things */
    "_ISWEAK",
    "_ISHUNGRY",
    "_ISFULL",
    "_ISGORGED",
    "_ISBLIND",
    "_ISAFRAID",
    "_ISCONFUSED",
    "_ISPOISONED",
    "_ISCUT",
    "_ISSTUN",
    "_ISHEAVYSTUN",
    "_ISIMAGE",
    "_ISSTUDY",
    "_ISSEARCHING",
    "_ISFIXLEV",
    "_ISFIXEXP",
    "_ISFIXSTR",
    "_ISFIXINT",
    "_ISFIXWIS",
    "_ISFIXDEX",
    "_ISFIXCON",
    "_ISFIXCHR",
    "_ISFIXALL",
	"_HRTIME",
	"_MNTIME",

/* some combat stuff */
    "_ARMOR",
    "_TOHIT",   /* base to hit, does not include weapon */
    "_TODAM",   /* base to damage, does not include weapon */
    "_WTOHIT",  /* weapon to hit */
    "_WTODAM",  /* weapon to damage */
    "_BTOHIT",  /* bow to hit */
    "_BTODAM",  /* bow to damage */
    "_BLOWS",
    "_SHOTS",
    "_WMAXDAM", /* max damage per round with weapon (normal blow) */
                /* Assumes you can enchant to +8 if you are level 25+ */
    "_WBASEDAM",/* max damage per round with weapon (normal blow) */
                /* Assumes you have no enchantment */
    "_BMAXDAM", /* max damage per round with bow (normal hit) */
                /* Assumes you can enchant to +8 if you are level 25+ */
    "_HEAVYWEPON",
    "_HEAVYBOW",

/* curses */
    "_CRSTELE",
    "_CRSAGRV",
	"_CRSTY",
    "_CRSNOTELE",    /* Anti teleport */
    "_CRSNOMAGIC",   /* Anti Magic */
	"_ENCUMBERD", /* Current total weight carrying */
	"_FEAR_LITE",	/* Vampire or Fearful of Lite */

/* weapon attributes */
    "_WSANIMAL",  /* WS = weapon slays */
    "_WSEVIL",
    "_WSUNDEAD",
    "_WSDEMON",
    "_WSORC",
    "_WSTROLL",
    "_WSGIANT",
    "_WSDRAGON",
    "_WKDRAGON",   /* WK = Weapon Kills */
    "_WIMPACT",
    "_WBACID",     /* WB = Weapon Branded With */
    "_WBELEC",
    "_WBFIRE",
    "_WBCOLD",
	"_WBPOIS",
	"_WBVORPAL",
	"_WBVAMPIRIC",
	"_WBCHAOTIC",

/* amounts */
    "_ATELEPORT",
    "_AESCAPE",
    "_FUEL",
    "_HEAL",
    "_EZHEAL",
    "_ID",
    "_ASPEED",
    "_ASTFMAGI",  /* Amount Staff Charges */
    "_ASTFDEST",
    "_AMISSLES",  /* only ones for your current bow count */
    "_ACUREPOIS",
    "_ADETTRAP",
    "_ADETDOOR",
    "_ADETEVIL",
    "_AMAGICMAP",
    "_ARECHARGE",
    "_APFE",      /* Protection from Evil */
    "_AGLYPH",    /* Rune Protection */
    "_ACCW",     /* CCW potions (just because we use it so often) */
    "_ACSW",     /* CSW potions (+ CLW if cut) */
    "_ARESHEAT", /* potions of res heat */
    "_ARESCOLD", /* pot of res cold */
    "_ATELEPORTLVL", /* scroll of teleport level */
    "_AXGOI",        /* Reliable GOI spell */
    "_AGOI",        /* GOI spell Legal*/
    "_AHWORD",      /* Holy Word prayer Legal*/

	"_TOWN_NUM",			/* Current town number */
	"_ARENA_NUM",		/* monster number in arena -KMW- */
	"_INSIDEARENA",		/* Is character inside arena? */
	"_INSIDEQUEST",		/* Inside quest level */
	"_X_WILD",      	/* Coordinates in the wilderness */
	"_Y_WILD",

    NULL
};

/*
 * Some variables
 */

bool borg_active;       /* Actually active */
bool borg_resurrect = FALSE;    /* continous play mode */

bool borg_cancel;       /* Being cancelled */

char genocide_target;   /* identity of the poor unsuspecting soul */
int zap_slot;                  /* slot of a wand/staff---to avoid a game bug*/
bool borg_casted_glyph;        /* because we dont have a launch anymore */
int borg_stop_dlevel = -1;
int borg_stop_clevel = -1;
bool borg_stop_king = TRUE;
bool borg_dont_react = FALSE;
int successful_target = 0;
int sold_item_tval;
int sold_item_sval;
int sold_item_pval;
int sold_item_store;

/*
 * Various silly flags
 */

bool borg_flag_save = TRUE;    /* Save savefile at each level */
bool borg_flag_dump = FALSE;    /* Save savefile at each death */
bool borg_save = FALSE;        /* do a save next level */
bool borg_graphics = FALSE;    /* rr9's graphics */
bool borg_confirm_target = FALSE; /* emergency spell use */

/*
 * Use a simple internal random number generator
 */

bool borg_rand_quick;       /* Save system setting */

u32b borg_rand_value;       /* Save system setting */

u32b borg_rand_local;       /* Save personal setting */


bool borg_do_star_id;


/*
 * Hack -- Time variables
 */

s16b borg_t = 0L;          /* Current "time" */
s16b need_see_inviso = 0;    /* cast this when required */
s16b borg_see_inv = 0;
bool need_shift_panel = FALSE;    /* to spot offscreens */
s16b when_shift_panel = 0L;
s16b time_this_panel = 0L;   /* Current "time" on current panel*/
bool vault_on_level;         /* Borg will search for a vault */
bool unique_on_level;
bool scaryguy_on_level;     /* flee from certain guys */
s16b borg_quest_level;      /* On a level with quest */

bool breeder_level = FALSE;          /* Borg will shut door */
s16b old_depth = 128;
s16b borg_respawning = 0;
bool genocide_level_hounds = FALSE;       /* Used against Hounds */
s16b borg_hound_count = 0;
s16b borg_no_retreat= 0;

/*
 * Hack -- Other time variables
 */

s16b when_call_lite;        /* When we last did call light */
s16b when_wizard_lite;      /* When we last did wizard light */

s16b when_detect_traps;     /* When we last detected traps */
s16b when_detect_doors;     /* When we last detected doors */
s16b when_detect_walls;     /* When we last detected walls */
s16b when_detect_evil;      /* When we last detected monsters or evil */
s16b when_last_kill_mult = 0;   /* When a multiplier was last killed */

bool my_need_alter;        /* incase i hit a wall or door */
bool my_no_alter;          /*  */
bool my_need_redraw;        /* incase i hit a wall or door */
bool borg_attempting_refresh = FALSE;  /* for the goi spell */

/*
 * Some information
 */

s16b goal;          /* Goal type */

bool goal_rising;       /* Currently returning to town */

bool goal_leaving;      /* Currently leaving the level */

bool goal_fleeing;      /* Currently fleeing the level */

bool goal_ignoring;     /* Currently ignoring monsters */

int goal_recalling;     /* Currently waiting for recall, guessing the turns left */

bool goal_less;         /* return to, but dont use, the next up stairs */

s16b borg_times_twitch; /* how often twitchy on this level */
s16b borg_escapes;      /* how often teleported on this level */

bool stair_less;        /* Use the next "up" staircase */
bool stair_more;        /* Use the next "down" staircase */

s32b borg_began;        /* When this level began */
s32b borg_time_town;    /* how long it has been since I was in town */

s16b avoidance = 0;     /* Current danger thresh-hold */

bool borg_failure;      /* Notice failure */

bool borg_simulate;     /* Simulation flag */
bool borg_attacking;        /* Simulation flag */
bool borg_offsetting;    /* offset ball attacks */

bool borg_completed;        /* Completed the level */
bool borg_on_upstairs;      /* used when leaving a level */
bool borg_on_dnstairs;      /* used when leaving a level */

bool borg_needs_searching;  /* borg will search with each step */
bool borg_full_damage;  /* make danger = full possible damage. */

/* defence flags */
bool borg_prot_from_evil;
bool borg_speed;
bool borg_bless;
bool borg_hero;
bool borg_berserk;
bool my_oppose_fire;
bool my_oppose_cold;
bool my_oppose_acid;
bool my_oppose_pois;
bool my_oppose_elec;
s16b borg_goi;
s16b borg_inviso;
bool borg_esp;

s16b borg_game_ratio;  /* the ratio of borg time to game time */

bool borg_shield;
bool borg_on_glyph;    /* borg is standing on a glyph of warding */
bool borg_create_door;    /* borg is going to create doors */
bool borg_sleep_spell;
bool borg_sleep_spell_ii;
bool borg_slow_spell;  /* borg is about to cast the spell */
bool borg_confuse_spell;
bool borg_fear_mon_spell;

/*
 * Current shopping information
 */

s16b goal_shop = -1;        /* Next shop to visit */
s16b goal_ware = -1;        /* Next item to buy there */
s16b goal_item = -1;        /* Next item to sell there */
int borg_food_onsale = -1;      /* Are shops selling food? */

/*
 * Location variables
 */

int w_x;            /* Current panel offset (X) */
int w_y;            /* Current panel offset (Y) */

int c_x;            /* Current location (X) */
int c_y;            /* Current location (Y) */

int g_x;            /* Goal location (X) */
int g_y;            /* Goal location (Y) */

int dim_door_y;		/* Safe landing zone for DDoor */
int dim_door_x;

/* BIG HACK! Assume only 10 cursed artifacts */
int bad_obj_x[50];  /* Dropped cursed artifact at location (X) */
int bad_obj_y[50];  /* Dropped cursed artifact at location (Y) */

/* Some important town information */
int town_x_wild[5];	/* Wilderness location of Towns */
int town_y_wild[5];	/* Wilderness location of Towns */

/*
 * Some estimated state variables
 */

s16b my_stat_max[6];    /* Current "maximal" stat values */
s16b my_stat_cur[6];    /* Current "natural" stat values */
s16b my_stat_use[6];    /* Current "resulting" stat values */
s16b my_stat_ind[6];    /* Current "additions" to stat values */
bool my_need_stat_check[6];  /* do I need to check my stats? */

s16b my_stat_add[6];  /* additions to stats  This will allow upgrading of */
                      /* equiptment to allow a ring of int +4 to be traded */
                      /* for a ring of int +6 even if maximized to allow a */
                      /* later swap to be better. */

s16b home_stat_add[6];

int weapon_swap;    /* location of my swap weapon */
int armour_swap;    /* my swap of armour */

/* a 3 state boolean */
/*-1 = not cursed, no help needed for it */
/* 0 = light curse, needs light remove curse spell */
/* 1 = heavy curse, needs heavy remove curse spell */
int decurse_weapon_swap;  /* my swap is great, except its cursed */
int enchant_weapon_swap_to_h;  /* my swap is great, except its cursed */
int enchant_weapon_swap_to_d;  /* my swap is great, except its cursed */
int decurse_armour_swap;  /* my swap is great, except its cursed */
int enchant_armour_swap_to_a;  /* my swap is great, except its cursed */
bool borg_wearing_cursed;

s32b weapon_swap_value;
s32b armour_swap_value;

s16b weapon_swap_digger;
int weapon_swap_slay_animal;
int weapon_swap_slay_evil;
int weapon_swap_slay_undead;
int weapon_swap_slay_demon;
int weapon_swap_slay_orc;
int weapon_swap_slay_troll;
int weapon_swap_slay_giant;
int weapon_swap_slay_dragon;
int weapon_swap_kill_dragon;
int weapon_swap_impact;
int weapon_swap_brand_acid;
int weapon_swap_brand_elec;
int weapon_swap_brand_fire;
int weapon_swap_brand_cold;
int weapon_swap_see_infra;
int weapon_swap_slow_digest;
int weapon_swap_aggravate;
int weapon_swap_teleport;
int weapon_swap_regenerate;
int weapon_swap_telepathy;
int weapon_swap_lite;
int weapon_swap_see_invis;
int weapon_swap_ffall;
int weapon_swap_free_act;
int weapon_swap_hold_life;
int weapon_swap_immune_fire;
int weapon_swap_immune_acid;
int weapon_swap_immune_cold;
int weapon_swap_immune_elec;
int weapon_swap_resist_acid;
int weapon_swap_resist_elec;
int weapon_swap_resist_fire;
int weapon_swap_resist_cold;
int weapon_swap_resist_pois;
int weapon_swap_resist_conf;
int weapon_swap_resist_sound;
int weapon_swap_resist_lite;
int weapon_swap_resist_dark;
int weapon_swap_resist_chaos;
int weapon_swap_resist_disen;
int weapon_swap_resist_shard;
int weapon_swap_resist_nexus;
int weapon_swap_resist_blind;
int weapon_swap_resist_neth;
int weapon_swap_resist_fear;
int armour_swap_slay_animal;
int armour_swap_slay_evil;
int armour_swap_slay_undead;
int armour_swap_slay_demon;
int armour_swap_slay_orc;
int armour_swap_slay_troll;
int armour_swap_slay_giant;
int armour_swap_slay_dragon;
int armour_swap_kill_dragon;
int armour_swap_impact;
int armour_swap_brand_acid;
int armour_swap_brand_elec;
int armour_swap_brand_fire;
int armour_swap_brand_cold;
int armour_swap_see_infra;
int armour_swap_slow_digest;
int armour_swap_aggravate;
int armour_swap_teleport;
int armour_swap_regenerate;
int armour_swap_telepathy;
int armour_swap_lite;
int armour_swap_see_invis;
int armour_swap_ffall;
int armour_swap_free_act;
int armour_swap_hold_life;
int armour_swap_immune_fire;
int armour_swap_immune_acid;
int armour_swap_immune_cold;
int armour_swap_immune_elec;
int armour_swap_resist_acid;
int armour_swap_resist_elec;
int armour_swap_resist_fire;
int armour_swap_resist_cold;
int armour_swap_resist_pois;
int armour_swap_resist_conf;
int armour_swap_resist_sound;
int armour_swap_resist_lite;
int armour_swap_resist_dark;
int armour_swap_resist_chaos;
int armour_swap_resist_disen;
int armour_swap_resist_shard;
int armour_swap_resist_nexus;
int armour_swap_resist_blind;
int armour_swap_resist_neth;
int armour_swap_resist_fear;

int my_ammo_tval;  /* Ammo -- "tval" */
int my_ammo_sides; /* Ammo -- "sides" */
s16b my_ammo_power; /* Shooting multipler */
s16b my_ammo_range; /* Shooting range */

s16b my_need_enchant_to_a;  /* Need some enchantment */
s16b my_need_enchant_to_h;  /* Need some enchantment */
s16b my_need_enchant_to_d;  /* Need some enchantment */
s16b my_need_brand_weapon;  /* apw actually brand bolts */

/*
 * Hack -- basic "power"
 */

s32b my_power;


/*
 * Various "amounts" (for the player)
 */

s16b amt_phase;
s16b amt_food_hical;
s16b amt_food_lowcal;

s16b amt_slow_poison;
s16b amt_cure_confusion;
s16b amt_cure_blind;

s16b amt_book[7][4]; /* [realm][sval] */

s16b amt_add_stat[6];
s16b amt_fix_stat[7];  /* #7 is to fix all stats */
s16b amt_fix_exp;

s16b amt_cool_staff;   /* holiness - power staff */

s16b amt_enchant_to_a;
s16b amt_enchant_to_d;
s16b amt_enchant_to_h;
s16b amt_brand_weapon;  /* apw brand bolts */
s16b amt_enchant_weapon;
s16b amt_enchant_armor;
s16b amt_digger;

/*
 * Various "amounts" (for the home)
 */

int num_food;
int num_mold;
int num_ident;
int num_star_ident;
int num_recall;
int num_phase;
int num_escape;
int num_teleport;
int num_berserk;
int num_teleport_level;

int num_cure_critical;
int num_cure_serious;

int num_pot_rheat;
int num_pot_rcold;

int num_missile;

int num_book[7][4]; /* [realm][book] */

int num_fix_stat[7]; /* #7 is to fix all stats */

int num_fix_exp;
int num_mana;
int num_heal;
int num_heal_true;
int num_ez_heal;
int num_ez_heal_true;
int num_pfe;
int num_glyph;
int num_mass_genocide;
int num_speed;
int num_goi_pot;
int num_resist_pot;

int num_enchant_to_a;
int num_enchant_to_d;
int num_enchant_to_h;
int num_brand_weapon;  /*apw brand bolts */
int num_genocide;

int num_artifact;

s16b home_slot_free;
s16b home_damage;
s16b num_duplicate_items;
int num_slow_digest;
int num_regenerate;
int num_telepathy;
int num_lite;
int num_see_inv;
int num_invisible;   /* apw */

int num_ffall;
int num_free_act;
int num_hold_life;
int num_immune_acid;
int num_immune_elec;
int num_immune_fire;
int num_immune_cold;
int num_resist_acid;
int num_resist_elec;
int num_resist_fire;
int num_resist_cold;
int num_resist_pois;
int num_resist_conf;
int num_resist_sound;
int num_resist_lite;
int num_resist_dark;
int num_resist_chaos;
int num_resist_disen;
int num_resist_shard;
int num_resist_nexus;
int num_resist_blind;
int num_resist_neth;
int num_sustain_str;
int num_sustain_int;
int num_sustain_wis;
int num_sustain_dex;
int num_sustain_con;
int num_sustain_all;

int num_speed;
int num_edged_weapon;
int num_bad_gloves;
int num_weapons;
int num_bow;
int num_rings;
int num_neck;
int num_armor;
int num_cloaks;
int num_shields;
int num_hats;
int num_gloves;
int num_boots;

/*
 * Hack -- extra state variables
 */

int borg_feeling = 0;   /* Current level "feeling" */

/*
 * Hack -- current shop index
 */

s16b shop_num = -1;     /* Current shop index */



/*
 * State variables extracted from the screen
 */

s32b borg_exp;      /* Current experience */

s32b borg_gold;     /* Current gold */

int borg_stat[6];   /* Current stat values */

int borg_book[8][4];   /* Current book slots, [realm][sval] */


/*
 * State variables extracted from the inventory/equipment
 */

int borg_cur_wgt;   /* Current weight */


/*
 * Constant state variables
 */

int borg_race;      /* Player race */
int borg_class;     /* Player class */


/*
 * Hack -- access the class/race records
 */

player_race *rb_ptr;    /* Player race info */
player_class *cb_ptr;   /* Player class info */

player_magic *mb_ptr;   /* Player magic info */



/*
 * Number of turns to step for (zero means forever)
 */
u16b borg_step = 0;     /* Step count (if any) */


/*
 * Status message search string
 */
char borg_match[128] = "plain gold ring";  /* Search string */


/*
 * Log file
 */
FILE *borg_fff = NULL;      /* Log file */


/*
 * Hack -- single character constants
 */

const char p1 = '(', p2 = ')';
const char c1 = '{', c2 = '}';
const char b1 = '[', b2 = ']';


/*
 * Hack -- the detection arrays
 */

bool borg_detect_wall[6][6];

bool borg_detect_trap[6][6];

bool borg_detect_door[6][6];

bool borg_detect_evil[6][6];

/*
 * Locate the store doors
 */

byte *track_shop_x;
byte *track_shop_y;

byte *track_quest_x;
byte *track_quest_y;

/*
 * Track "stairs up"
 */

s16b track_less_num;
s16b track_less_size;
byte *track_less_x;
byte *track_less_y;


/*
 * Track "stairs down"
 */

s16b track_more_num;
s16b track_more_size;
byte *track_more_x;
byte *track_more_y;

/*
 * Track glyphs
 */
s16b track_glyph_num;
s16b track_glyph_size;
byte *track_glyph_x;
byte *track_glyph_y;

/*
 * Track Steps
 */
s16b track_step_num;
s16b track_step_size;
byte *track_step_x;
byte *track_step_y;

/*
 * Track closed doors
 */
s16b track_door_num;
s16b track_door_size;
byte *track_door_x;
byte *track_door_y;

/*
 * The object list.  This list is used to "track" objects.
 */

s16b borg_takes_cnt;

s16b borg_takes_nxt;

borg_take *borg_takes;


/*
 * The monster list.  This list is used to "track" monsters.
 */

s16b borg_kills_cnt;
s16b borg_kills_summoner;    /* index of a summoner */
s16b borg_kills_nxt;

borg_kill *borg_kills;


/* a 3 state boolean */
/*-1 = not checked yet */
/* 0 = not ready */
/* 1 = ready */
int borg_ready_morgoth;


/*
 * Hack -- extra fear per "region"
 */
u16b borg_fear_region[6][18];


/*
 * Hack -- count racial appearances per level
 */
s16b *borg_race_count;


/*
 * Hack -- count racial kills (for uniques)
 */

s16b *borg_race_death;


/*
 * Classification of map symbols
 */

bool borg_is_take[256];     /* Symbol may be an object */

bool borg_is_kill[256];     /* Symbol may be a monster */


/*
 * The current map
 */

borg_grid *borg_grids[AUTO_MAX_Y];  /* The grids */


/*
 * Maintain a set of grids marked as "BORG_LITE"
 */

s16b borg_lite_n = 0;

byte borg_lite_x[AUTO_LITE_MAX];
byte borg_lite_y[AUTO_LITE_MAX];

/*
 * Maintain a set of grids marked as "BORG_GLOW"
 */

s16b borg_glow_n = 0;

byte borg_glow_x[AUTO_LITE_MAX];
byte borg_glow_y[AUTO_LITE_MAX];


/*
 * Maintain a set of grids marked as "BORG_VIEW"
 */

s16b borg_view_n = 0;

byte borg_view_x[AUTO_VIEW_MAX];
byte borg_view_y[AUTO_VIEW_MAX];


/*
 * Maintain a temporary set of grids
 */

s16b borg_temp_n = 0;

byte borg_temp_x[AUTO_TEMP_MAX];
byte borg_temp_y[AUTO_TEMP_MAX];

byte offset_x;
byte offset_y;


/*
 * Maintain a circular queue of grids
 */

s16b borg_flow_n = 0;

byte borg_flow_x[AUTO_FLOW_MAX];
byte borg_flow_y[AUTO_FLOW_MAX];


/*
 * Hack -- use "flow" array as a queue
 */

int flow_head = 0;
int flow_tail = 0;



/*
 * Some variables
 */

borg_data *borg_data_flow;  /* Current "flow" data */

borg_data *borg_data_cost;  /* Current "cost" data */

borg_data *borg_data_hard;  /* Constant "hard" data */

borg_data *borg_data_know;  /* Current "know" flags */

borg_data *borg_data_icky;  /* Current "icky" flags */



/*
 * Strategy flags -- recalculate things
 */

bool borg_danger_wipe = FALSE;  /* Recalculate danger */

bool borg_do_update_view = FALSE;  /* Recalculate view */

bool borg_do_update_lite = FALSE;  /* Recalculate lite */


/*
 * Strategy flags -- examine the world
 */

bool borg_do_inven = TRUE;  /* Acquire "inven" info */

bool borg_do_equip = TRUE;  /* Acquire "equip" info */

bool borg_do_panel = TRUE;  /* Acquire "panel" info */

bool borg_do_frame = TRUE;  /* Acquire "frame" info */

bool borg_do_spell = TRUE;  /* Acquire "spell" info */

bool borg_do_browse = 0;    /* Acquire "store" info */

byte borg_do_browse_what = 0;   /* Hack -- store for "borg_do_browse" */

byte borg_do_browse_more = 0;   /* Hack -- pages for "borg_do_browse" */


/*
 * Strategy flags -- run certain functions
 */

bool borg_do_crush_junk = FALSE;

bool borg_do_crush_hole = FALSE;

bool borg_do_crush_slow = FALSE;

/* am I fighting a unique? */
int borg_fighting_unique;
bool borg_fighting_evil_unique;

/* am I fighting a summoner? */
bool borg_fighting_summoner;


#ifndef BORG_TK
/*
 * Calculate "incremental motion". Used by project() and shoot().
 * Assumes that (*y,*x) lies on the path from (y1,x1) to (y2,x2).
 */
/* changing this to be more like project_path */
/* note that this is much slower but much more accurate */
void borgmove2(int *py, int *px, int y1, int x1, int y2, int x2)
{
    int dy, dx;
    int sy, sx;
    int y, x;

    /* Scale factors */
    int full, half;

    /* Fractions */
    int frac;

    /* Slope */
    int m;

    /* Distance */
    int k = 0;

    /* Extract the distance travelled */
    /* Analyze "dy" */
    if (y2 < y1)
    {
        dy = (y1 - y2);
        sy = -1;
    }
    else
    {
        dy = (y2 - y1);
        sy = 1;
    }

    /* Analyze "dx" */
    if (x2 < x1)
    {
        dx = (x1 - x2);
        sx = -1;
    }
    else
    {
        dx = (x2 - x1);
        sx = 1;
    }

    /* Paranoia -- Hack -- no motion */
    if (!dy && !dx) return;

    /* Number of "units" in one "half" grid */
    half = (dy * dx);

    /* Number of "units" in one "full" grid */
    full = half << 1;

    /* First step is fixed */
    if (*px == x1 && *py == y1)
    {
        if (dy > dx)
        {
            *py+=sy;
            return;
        }
        else if (dx > dy)
        {
            *px+=sx;
            return;
        }
        else
        {
            *px+=sx;
            *py+=sy;
            return;
        }
    }

    /* Move mostly vertically */
    if (dy > dx)
    {
        k = dy;

        /* Start at tile edge */
        frac = dx * dx;

        /* Let m = ((dx/dy) * full) = (dx * dx * 2) = (frac * 2) */
        m = frac << 1;

        /* Start */
        y = y1 + sy;
        x = x1;

        /* Create the projection path */
        while (1)
        {
            if (x == *px && y == *py)
                k = 1;

            /* Slant */
            if (m)
            {
                /* Advance (X) part 1 */
                frac += m;

                /* Horizontal change */
                if (frac >= half)
                {
                    /* Advance (X) part 2 */
                    x += sx;

                    /* Advance (X) part 3 */
                    frac -= full;
                }
            }

            /* Advance (Y) */
            y += sy;

            /* Track distance */
            k--;

            if (!k)
            {
                *px = x;
                *py = y;
                return;
            }
        }
    }
    /* Move mostly horizontally */
    else if (dx > dy)
    {
        /* Start at tile edge */
        frac = dy * dy;

        /* Let m = ((dy/dx) * full) = (dy * dy * 2) = (frac * 2) */
        m = frac << 1;

        /* Start */
        y = y1;
        x = x1 + sx;
        k = dx;

        /* Create the projection path */
        while (1)
        {
            if (x == *px && y == *py)
                k = 1;

            /* Slant */
            if (m)
            {
                /* Advance (Y) part 1 */
                frac += m;

                /* Vertical change */
                if (frac >= half)
                {
                    /* Advance (Y) part 2 */
                    y += sy;

                    /* Advance (Y) part 3 */
                    frac -= full;
                }
            }

            /* Advance (X) */
            x += sx;

            /* Track distance */
            k--;

            if (!k)
            {
                *px = x;
                *py = y;
                return;
            }
        }
    }
    /* Diagonal */
    else
    {
        /* Start */
        k = dy;
        y = y1 + sy;
        x = x1 + sx;

        /* Create the projection path */
        while (1)
        {
            if (x == *px && y == *py)
                k=1;

            /* Advance (Y) */
            y += sy;

            /* Advance (X) */
            x += sx;

            /* Track distance */
            k--;

            if (!k)
            {
                *px = x;
                *py = y;
                return;
            }
        }
    }
}


/*
 * Query the "attr/char" at a given location on the screen
 * We return "zero" if the given location was legal
 *
 * XXX XXX XXX We assume the given location is legal
 */
errr borg_what_char(int x, int y, byte *a, char *c)
{
    /* Direct access XXX XXX XXX */
    (*a) = (Term->scr->a[y][x]);
    (*c) = (Term->scr->c[y][x]);

    /* Success */
    return (0);
}


/*
 * Query the "attr/chars" at a given location on the screen
 *
 * Note that "a" points to a single "attr", and "s" to an array
 * of "chars", into which the attribute and text at the given
 * location are stored.
 *
 * We will not grab more than "ABS(n)" characters for the string.
 * If "n" is "positive", we will grab exactly "n" chars, or fail.
 * If "n" is "negative", we will grab until the attribute changes.
 *
 * We automatically convert all "blanks" and "invisible text" into
 * spaces, and we ignore the attribute of such characters.
 *
 * We do not strip final spaces, so this function will very often
 * read characters all the way to the end of the line.
 *
 * We succeed only if a string of some form existed, and all of
 * the non-space characters in the string have the same attribute,
 * and the string was long enough.
 *
 * XXX XXX XXX We assume the given location is legal
 */
errr borg_what_text(int x, int y, int n, byte *a, char *s)
{
    int i;

    byte t_a;
    char t_c;

    byte *aa;
    char *cc;

    /* Current attribute */
    byte d_a = 0;

    /* Max length to scan for */
    int m = ABS(n);

    /* Hack -- Do not run off the screen */
    if (x + m > 80) m = 80 - x;

    /* Direct access XXX XXX XXX */
    aa = &(Term->scr->a[y][x]);
    cc = &(Term->scr->c[y][x]);

    /* Grab the string */
    for (i = 0; i < m; i++)
    {
        /* Access */
        t_a = *aa++;
        t_c = *cc++;

        /* Handle spaces */
        if ((t_c == ' ') || !t_a)
        {
            /* Save space */
            s[i] = ' ';
        }

        /* Handle real text */
        else
        {
            /* Attribute ready */
            if (d_a)
            {
                /* Verify the "attribute" (or stop) */
                if (t_a != d_a) break;
            }

            /* Acquire attribute */
            else
            {
                /* Save it */
                d_a = t_a;
            }

            /* Save char */
            s[i] = t_c;
        }
    }

    /* Terminate the string */
    s[i] = '\0';

    /* Save the attribute */
    (*a) = d_a;

    /* Too short */
    if ((n > 0) && (i != n)) return (1);

    /* Success */
    return (0);
}

#endif /* not BORG_TK */


/*
 * Log a message to a file
 */
void borg_info(cptr what)
{
    /* Dump a log file message */
    if (borg_fff) fprintf(borg_fff, "%s\n", what);

}



/*
 * Memorize a message, Log it, Search it, and Display it in pieces
 */
void borg_note(cptr what)
{
    int j, n, i, k;

    int w, h, x, y;


#ifndef BORG_TK
    term *old = Term;
#endif

    /* Memorize it */
    message_add(what, MSG_GENERIC);


    /* Log the message */
    borg_info(what);


    /* Mega-Hack -- Check against the search string */
    if (borg_match[0] && strstr(what, borg_match))
    {
        /* Clean cancel */
        borg_cancel = TRUE;
    }

    /* Mega-Hack -- Check against the swap loops */
    if (strstr(what, "Best Combo"))
    {
        /* Tick the anti loop clock */
        time_this_panel += 10;
        borg_note(format("# Anti-loop variable tick (%d).", time_this_panel));
    }

#ifndef BORG_TK
    /* Scan windows */
    for (j = 0; j < 8; j++)
    {
        if (!angband_term[j]) continue;

        /* Check flag */
        if (!(window_flag[j] & PW_BORG_1)) continue;

        /* Activate */
        Term_activate(angband_term[j]);

        /* Access size */
        Term_get_size(&w, &h);

        /* Access cursor */
        Term_locate(&x, &y);

        /* Erase current line */
        Term_erase(0, y, 255);


        /* Total length */
        n = strlen(what);

        /* Too long */
        if (n > w - 2)
        {
            char buf[1024];

            /* Split */
            while (n > w - 2)
            {
                /* Default */
                k = w - 2;

                /* Find a split point */
                for (i = w / 2; i < w - 2; i++)
                {
                    /* Pre-emptive split point */
                    if (isspace(what[i])) k = i;
                }

                /* Copy over the split message */
                for (i = 0; i < k; i++)
                {
                    /* Copy */
                    buf[i] = what[i];
                }

                /* Indicate split */
                buf[i++] = '\\';

                /* Terminate */
                buf[i] = '\0';

                /* Show message */
                Term_addstr(-1, TERM_WHITE, buf);

                /* Advance (wrap) */
                if (++y >= h) y = 0;

                /* Erase next line */
                Term_erase(0, y, 255);

                /* Advance */
                what += k;

                /* Reduce */
                n -= k;
            }

            /* Show message tail */
            Term_addstr(-1, TERM_WHITE, what);

            /* Advance (wrap) */
            if (++y >= h) y = 0;

            /* Erase next line */
            Term_erase(0, y, 255);
        }

        /* Normal */
        else
        {
            /* Show message */
            Term_addstr(-1, TERM_WHITE, what);

            /* Advance (wrap) */
            if (++y >= h) y = 0;

            /* Erase next line */
            Term_erase(0, y, 255);
        }


        /* Flush output */
        Term_fresh();

        /* Use correct window */
        Term_activate(old);
    }
#endif /* not BORG_TK */
 }




/*
 * Abort the Borg, noting the reason
 */
void borg_oops(cptr what)
{
    /* Stop processing */
    borg_active = FALSE;

    /* Give a warning */
    borg_note(format("# Aborting (%s).", what));

    /* Forget borg keys */
    borg_flush();
}

/*
 * A Queue of keypresses to be sent
 */
static char *borg_key_queue;
static s16b borg_key_head;
static s16b borg_key_tail;


/*
 * Add a keypress to the "queue" (fake event)
 */
errr borg_keypress(char k)
{
    /* Hack -- Refuse to enqueue "nul" */
    if (!k) return (-1);

    /* Hack -- note the keypress */
    if (borg_fff) borg_info(format("& Key <%c>", k));

    /* Store the char, advance the queue */
    borg_key_queue[borg_key_head++] = k;

    /* Circular queue, handle wrap */
    if (borg_key_head == KEY_SIZE) borg_key_head = 0;

    /* Hack -- Catch overflow (forget oldest) */
    if (borg_key_head == borg_key_tail) borg_oops("overflow");

    /* Hack -- Overflow may induce circular queue */
    if (borg_key_tail == KEY_SIZE) borg_key_tail = 0;

    /* Success */
    return (0);
}


/*
 * Add a keypress to the "queue" (fake event)
 */
errr borg_keypresses(cptr str)
{
    cptr s;

    /* Enqueue them */
    for (s = str; *s; s++) borg_keypress(*s);

    /* Success */
    return (0);
}


/*
 * Get the next Borg keypress
 */
char borg_inkey(bool take)
{
    int i;

    /* Nothing ready */
    if (borg_key_head == borg_key_tail)
        return (0);

    /* Extract the keypress */
    i = borg_key_queue[borg_key_tail];

    /* Do not advance */
    if (!take) return (i);

    /* Advance the queue */
    borg_key_tail++;

    /* Circular queue requires wrap-around */
    if (borg_key_tail == KEY_SIZE) borg_key_tail = 0;

    /* Return the key */
    return (i);
}



/*
 * Get the next Borg keypress
 */
void borg_flush(void)
{
    /* Simply forget old keys */
    borg_key_tail = borg_key_head;
}






/*
 * Hack -- take a note later
 */
bool borg_tell(cptr what)
{
    cptr s;

    /* Hack -- self note */
    borg_keypress(':');
    for (s = what; *s; s++) borg_keypress(*s);
    borg_keypress('\n');

    /* Success */
    return (TRUE);
}



/*
 * Attempt to change the borg's name
 */
bool borg_change_name(cptr str)
{
    cptr s;

    /* Cancel everything */
    borg_keypress(ESCAPE);
    borg_keypress(ESCAPE);

    /* Character description */
    borg_keypress('C');

    /* Change the name */
    borg_keypress('c');

    /* Enter the new name */
    for (s = str; *s; s++) borg_keypress(*s);

    /* End the name */
    borg_keypress('\r');

    /* Cancel everything */
    borg_keypress(ESCAPE);
    borg_keypress(ESCAPE);

    /* Success */
    return (TRUE);
}


/*
 * Attempt to dump a character description file
 */
bool borg_dump_character(cptr str)
{
    cptr s;

    /* Cancel everything */
    borg_keypress(ESCAPE);
    borg_keypress(ESCAPE);

    /* Character description */
    borg_keypress('C');

    /* Dump character file */
    borg_keypress('f');

    /* Enter the new name */
    for (s = str; *s; s++) borg_keypress(*s);

    /* End the file name */
    borg_keypress('\r');

    /* Cancel everything */
    borg_keypress(ESCAPE);
    borg_keypress(ESCAPE);

    /* Success */
    return (TRUE);
}




/*
 * Attempt to save the game
 */
bool borg_save_game(void)
{
    /* Cancel everything */
    borg_keypress(ESCAPE);
    borg_keypress(ESCAPE);

    /* Save the game */
    borg_keypress('^');
    borg_keypress('S');

    /* Cancel everything */
    borg_keypress(ESCAPE);
    borg_keypress(ESCAPE);

    /* Success */
    return (TRUE);
}




/*
 * Update the Borg based on the current "frame"
 *
 * Assumes the Borg is actually in the dungeon.
 */
void borg_update_frame(void)
{
    int i;

	s32b len = 10L * TOWN_DAWN;
	s32b tick = turn % len + len / 4;

	int hour = (24 * tick / len) % 24;
	int min = (1440 * tick / len) % 60;

	borg_skill[BI_HRTIME] = hour;
	borg_skill[BI_MNTIME] = min;

    /* Assume level is fine */
    borg_skill[BI_ISFIXLEV] = FALSE;

    /* Note "Lev" vs "LEV" */
    if (p_ptr->lev < p_ptr->max_lev) borg_skill[BI_ISFIXLEV] = TRUE;

    /* Extract "LEVEL xxxxxx" */
    borg_skill[BI_CLEVEL] = borg_skill[BI_CLEVEL] = p_ptr->lev;

    /* cheat the max clevel */
    borg_skill[BI_MAXCLEVEL] = p_ptr->max_lev;

    /* Note "Winner" */
    borg_skill[BI_KING] = p_ptr->total_winner;

    /* Assume experience is fine */
    borg_skill[BI_ISFIXEXP] = FALSE;

    /* Note "Exp" vs "EXP" and am I lower than level 50*/
    if ((p_ptr->exp < p_ptr->max_exp) &&
        (borg_skill[BI_CLEVEL] != 50)) borg_skill[BI_ISFIXEXP] = TRUE;

    /* Extract "EXP xxxxxxxx" */
    borg_exp = p_ptr->exp;


    /* Extract "AU xxxxxxxxx" */
    borg_gold = p_ptr->au;


    /* Extract "Fast (+x)" or "Slow (-x)" */
    borg_skill[BI_SPEED] = p_ptr->pspeed;

    /* Check my float for decrementing variables */
    if (borg_skill[BI_SPEED] >110)
    {
        borg_game_ratio = 100000/(((borg_skill[BI_SPEED]-110)*10)+100);
    }
    else
    {
        borg_game_ratio = 1000;
    }


    /* if hasting, it doesn't count as 'borg_speed'.  The speed */
    /* gained from hasting is counted seperately. */
    if (borg_speed)
        borg_skill[BI_SPEED] -= 10;

    /* Extract "Cur AC xxxxx" */
    borg_skill[BI_ARMOR] = p_ptr->dis_ac + p_ptr->dis_to_a;

    /* Extract "Cur HP xxxxx" */
    borg_skill[BI_CURHP] = p_ptr->chp;

    /* Extract "Max HP xxxxx" */
    borg_skill[BI_MAXHP] = p_ptr->mhp;

    /* Extract "Cur SP xxxxx" (or zero) */
    borg_skill[BI_CURSP] = p_ptr->csp;

    /* Extract "Max SP xxxxx" (or zero) */
    borg_skill[BI_MAXSP] = p_ptr->msp;

    /* Clear all the "state flags" */
    borg_skill[BI_ISWEAK] = borg_skill[BI_ISHUNGRY] = borg_skill[BI_ISFULL] = borg_skill[BI_ISGORGED] = FALSE;
    borg_skill[BI_ISBLIND] = borg_skill[BI_ISCONFUSED] = borg_skill[BI_ISAFRAID] = borg_skill[BI_ISPOISONED] = FALSE;
    borg_skill[BI_ISCUT] = borg_skill[BI_ISSTUN] = borg_skill[BI_ISHEAVYSTUN] = borg_skill[BI_ISIMAGE] = borg_skill[BI_ISSTUDY] = FALSE;
    borg_skill[BI_ISSEARCHING] = FALSE;

    /* Check for "Weak" */
    if (p_ptr->food < PY_FOOD_WEAK) borg_skill[BI_ISWEAK] = borg_skill[BI_ISHUNGRY] = TRUE;

    /* Check for "Hungry" */
    else if (p_ptr->food < PY_FOOD_ALERT) borg_skill[BI_ISHUNGRY] = TRUE;

    /* Check for "Normal" */
    else if (p_ptr->food < PY_FOOD_FULL) /* Nothing */;

    /* Check for "Full" */
    else if (p_ptr->food < PY_FOOD_MAX) borg_skill[BI_ISFULL] = TRUE;

    /* Check for "Gorged" */
    else borg_skill[BI_ISGORGED] = borg_skill[BI_ISFULL] = TRUE;

    /* Check for "Blind" */
    if (p_ptr->blind) borg_skill[BI_ISBLIND] = TRUE;

    /* Check for "Confused" */
    if (p_ptr->confused) borg_skill[BI_ISCONFUSED] = TRUE;

    /* Check for "Afraid" */
    if (p_ptr->afraid) borg_skill[BI_ISAFRAID] = TRUE;

    /* Check for "Poisoned" */
    if (p_ptr->poisoned) borg_skill[BI_ISPOISONED] = TRUE;

    /* Check for any text */
    if (p_ptr->cut) borg_skill[BI_ISCUT] = TRUE;

    /* Check for Stun */
    if (p_ptr->stun && (p_ptr->stun <= 50)) borg_skill[BI_ISSTUN] = TRUE;

    /* Check for Heavy Stun */
    if (p_ptr->stun > 50) borg_skill[BI_ISHEAVYSTUN] = TRUE;

    /* XXX XXX XXX Parse "State" */
    if (p_ptr->searching) borg_skill[BI_ISSEARCHING] = TRUE;

    /* Check for "Study" */
    if (p_ptr->new_spells) borg_skill[BI_ISSTUDY] = TRUE;

    /* Parse stats */
    for (i = 0; i < 6; i++)
    {
        borg_skill[BI_ISFIXSTR+i] = p_ptr->stat_cur[A_STR+i] < p_ptr->stat_max[A_STR+i];
        borg_skill[BI_CSTR+i] = p_ptr->stat_cur[A_STR+i];
        borg_stat[i] = p_ptr->stat_cur[i];

    }

    /* Hack -- Access max depth */
    borg_skill[BI_CDEPTH] = p_ptr->depth;

    /* Hack -- Access max depth */
    borg_skill[BI_MAXDEPTH] = p_ptr->max_depth;

	/* Hack -- Realms */
	borg_skill[BI_REALM1] = p_ptr->realm1;
	borg_skill[BI_REALM2] = p_ptr->realm2;

	/* Hack -- Mana increases */
	switch (borg_class)
	{
		case CLASS_PRIEST:
		case CLASS_PALADIN:
		case CLASS_MONK:
		case CLASS_MINDCRAFTER:
		   borg_skill[BI_WISMANA] = 1;
		   break;

		case CLASS_MAGE:
		case CLASS_ROGUE:
		case CLASS_RANGER:
		case CLASS_WARRIOR_MAGE:
		case CLASS_CHAOS_WARRIOR:
		case CLASS_HIGH_MAGE:
			borg_skill[BI_INTMANA] = 1;
			break;
		default:
			borg_skill[BI_WISMANA] = 0;
			borg_skill[BI_INTMANA] = 0;
	}

	/* Misc Zang stuff */
#if 0
	borg_skill[BI_TOWN_NUM] = p_ptr->town_num;
	borg_skill[BI_ARENA_NUM] = p_ptr->arena_number;
	borg_skill[BI_INSIDEARENA] = p_ptr->inside_arena;
	borg_skill[BI_INSIDEQUEST] = p_ptr->inside_quest;
#endif /* 0 */
	borg_skill[BI_X_WILD] = p_ptr->wilderness_x;
	borg_skill[BI_Y_WILD] = p_ptr->wilderness_y;

	/* Time issues for Vampires */
}


int
borg_check_formula(int *formula)
{
    int     oper1;          /* operand #1 */
    int     oper2;          /* operand #2 */
    int     stack[256];     /* stack */
    int     *stackptr;      /* stack pointer */

    /* loop until we hit BFO_DONE */
    for (stackptr = stack; *formula; formula++)
    {
        if (stackptr < stack)
            return 0;
        switch (*formula)
        {
            /* Number */
            case BFO_NUMBER:
                *stackptr++ = *++formula;
                break;

            /* Variable */
            case BFO_VARIABLE:
                *stackptr++ = borg_has[*++formula];
                if ((*formula) > (z_info->k_max + z_info->k_max + z_info->a_max + BI_MAX))
                    return 0;
                break;

            /* Equal */
            case BFO_EQ:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 == oper2);
                break;

            /* Not Equal */
            case BFO_NEQ:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 != oper2);
                break;

            /* Less Than */
            case BFO_LT:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 < oper2);
                break;

            /* Less Than Or Equal */
            case BFO_LTE:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 <= oper2);
                break;

            /* Greater Than */
            case BFO_GT:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 > oper2);
                break;

            /* Greater Than Or Equal */
            case BFO_GTE:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 >= oper2);
                break;

            /* Logical And */
            case BFO_AND:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 && oper2);
                break;

            /* Logical Or */
            case BFO_OR:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 || oper2);
                break;

            /* Plus */
            case BFO_PLUS:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 + oper2);
                break;

            /* Minus */
            case BFO_MINUS:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 - oper2);
                break;

            /* Divide */
            case BFO_DIVIDE:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 / (oper2 ? oper2 : 1));
                break;

            /* Multiply */
            case BFO_MULT:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 * oper2);
                break;

            /* Logical Not */
            case BFO_NOT:
                oper1 = *--stackptr;
                *stackptr++ = (!oper1);
                break;

            default:
                return 0;
        }
    }

    if (stackptr != (stack+1))
        return 0;
    return 1;
}

cptr
borg_prt_formula(int *formula)
{
    static char FormulaStr[2000];
    char tmpStr[50];

    memset(FormulaStr, 0, sizeof(FormulaStr));
    /* loop until we hit BFO_DONE */
    for (; *formula; formula++)
    {
        switch (*formula)
        {
            /* Number */
            case BFO_NUMBER:

                sprintf(tmpStr, "%d ", *++formula);
                strcat(FormulaStr, tmpStr);
                break;

            /* Variable */
            case BFO_VARIABLE:
                strcat(FormulaStr, "'");
                strcat(FormulaStr, borg_prt_item(*++formula));
                strcat(FormulaStr, "'");
                strcat(FormulaStr, " ");
                break;

            /* Equal */
            case BFO_EQ:
                strcat(FormulaStr, "== ");
                break;

            /* Not Equal */
            case BFO_NEQ:
                strcat(FormulaStr, "!= ");
                break;

            /* Less Than */
            case BFO_LT:
                strcat(FormulaStr, "< ");
                break;

            /* Less Than Or Equal */
            case BFO_LTE:
                strcat(FormulaStr, "<= ");
                break;

            /* Greater Than */
            case BFO_GT:
                strcat(FormulaStr, "> ");
                break;

            /* Greater Than Or Equal */
            case BFO_GTE:
                strcat(FormulaStr, ">= ");
                break;

            /* Logical And */
            case BFO_AND:
                strcat(FormulaStr, "&& ");
                break;

            /* Logical Or */
            case BFO_OR:
                strcat(FormulaStr, "|| ");
                break;

            /* Plus */
            case BFO_PLUS:
                strcat(FormulaStr, "+ ");
                break;

            /* Minus */
            case BFO_MINUS:
                strcat(FormulaStr, "- ");
                break;

            /* Divide */
            case BFO_DIVIDE:
                strcat(FormulaStr, "/ ");
                break;

            /* Multiply */
            case BFO_MULT:
                strcat(FormulaStr, "* ");
                break;

            /* Logical Not */
            case BFO_NOT:
                strcat(FormulaStr, "! ");
                break;
        }
    }

    /* BFO_DONE */
    return FormulaStr;
}

int
borg_calc_formula(int *formula)
{
    int     oper1;          /* operand #1 */
    int     oper2;          /* operand #2 */
    int     stack[256];     /* stack */
    int     *stackptr;      /* stack pointer */


    if (!formula)
        return 0;

    *stack = 0;
    /* loop until we hit BFO_DONE */
    for (stackptr = stack; *formula; formula++)
    {
        switch (*formula)
        {
            /* Number */
            case BFO_NUMBER:
                *stackptr++ = *++formula;
                break;

            /* Variable */
            case BFO_VARIABLE:
                *stackptr++ = borg_has[*++formula];
                break;

            /* Equal */
            case BFO_EQ:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 == oper2);
                break;

            /* Not Equal */
            case BFO_NEQ:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 != oper2);
                break;

            /* Less Than */
            case BFO_LT:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 < oper2);
                break;

            /* Less Than Or Equal */
            case BFO_LTE:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 <= oper2);
                break;

            /* Greater Than */
            case BFO_GT:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 > oper2);
                break;

            /* Greater Than Or Equal */
            case BFO_GTE:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 >= oper2);
                break;

            /* Logical And */
            case BFO_AND:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 && oper2);
                break;

            /* Logical Or */
            case BFO_OR:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 || oper2);
                break;

            /* Plus */
            case BFO_PLUS:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 + oper2);
                break;

            /* Minus */
            case BFO_MINUS:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 - oper2);
                break;

            /* Divide */
            case BFO_DIVIDE:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 / (oper2 ? oper2 : 1));
                break;

            /* Multiply */
            case BFO_MULT:
                oper2 = *--stackptr;
                oper1 = *--stackptr;
                *stackptr++ = (oper1 * oper2);
                break;

            /* Logical Not */
            case BFO_NOT:
                oper1 = *--stackptr;
                *stackptr++ = (!oper1);
                break;
        }
    }

    /* BFO_DONE */
    return *--stackptr;
}


/*
 * Initialize this file
 */
void borg_init_1(void)
{
    int i, x, y;


    /* Allocate the "keypress queue" */
    C_MAKE(borg_key_queue, KEY_SIZE, char);


    /* Prapare a local random number seed */
if (!borg_rand_local)
    borg_rand_local = rand_int(0x10000000);


    /*** Grids ***/

    /* Make each row of grids */
    for (y = 0; y < AUTO_MAX_Y; y++)
    {
        /* Make each row */
        C_MAKE(borg_grids[y], AUTO_MAX_X, borg_grid);
    }


    /*** Grid data ***/

    /* Allocate */
    MAKE(borg_data_flow, borg_data);

    /* Allocate */
    MAKE(borg_data_cost, borg_data);

    /* Allocate */
    MAKE(borg_data_hard, borg_data);

    /* Allocate */
    MAKE(borg_data_know, borg_data);

    /* Allocate */
    MAKE(borg_data_icky, borg_data);

    /* Prepare "borg_data_hard" */
    for (y = 0; y < AUTO_MAX_Y; y++)
    {
        for (x = 0; x < AUTO_MAX_X; x++)
        {
            /* Prepare "borg_data_hard" */
            borg_data_hard->data[y][x] = 255;
        }
    }


    /*** Very special "tracking" array ***/

    /* Track the shop locations */
    C_MAKE(track_shop_x, 10, byte);
    C_MAKE(track_shop_y, 10, byte);

    /* Track the quest locations */
    C_MAKE(track_quest_x, 40, byte);
    C_MAKE(track_quest_y, 40, byte);

    /*** Special "tracking" arrays ***/

    /* Track "up" stairs */
    track_less_num = 0;
    track_less_size = 16;
    C_MAKE(track_less_x, track_less_size, byte);
    C_MAKE(track_less_y, track_less_size, byte);

    /* Track "down" stairs */
    track_more_num = 0;
    track_more_size = 16;
    C_MAKE(track_more_x, track_more_size, byte);
    C_MAKE(track_more_y, track_more_size, byte);

    /* Track glyphs */
    track_glyph_num = 0;
    track_glyph_size = 256;
    C_MAKE(track_glyph_x, track_glyph_size, byte);
    C_MAKE(track_glyph_y, track_glyph_size, byte);

    /* Track Steps */
    track_step_num = 0;
    track_step_size = 256;
    C_MAKE(track_step_x, track_step_size, byte);
    C_MAKE(track_step_y, track_step_size, byte);

    /* Track closed doors */
    track_door_num = 0;
    track_door_size = 256;
    C_MAKE(track_door_x, track_door_size, byte);
    C_MAKE(track_door_y, track_door_size, byte);

    /*** Object tracking ***/

    /* No objects yet */
    borg_takes_cnt = 0;
    borg_takes_nxt = 1;

    /* Array of objects */
    C_MAKE(borg_takes, 256, borg_take);

    /* Scan the objects */
    for (i = 0; i < z_info->k_max; i++)
    {
        object_kind *k_ptr = &k_info[i];

        /* Skip non-items */
        if (!k_ptr->name) continue;

        /* Notice this object */
        borg_is_take[(byte)(k_ptr->d_char)] = TRUE;
    }

    /*** Monster tracking ***/

    /* No monsters yet */
    borg_kills_cnt = 0;
    borg_kills_nxt = 1;

    /* Array of monsters */
    C_MAKE(borg_kills, 256, borg_kill);

    /* Scan the monsters */
    for (i = 1; i < z_info->r_max - 1; i++)
    {
        monster_race *r_ptr = &r_info[i];

        /* Skip non-monsters */
        if (!r_ptr->name) continue;
#if 0
        /* Hack -- Skip "clear" monsters XXX XXX XXX */
        if (r_ptr->flags1 & RF1_CHAR_CLEAR) continue;

        /* Hack -- Skip "multi" monsters XXX XXX XXX */
        if (r_ptr->flags1 & RF1_CHAR_MULTI) continue;
#endif
        /* Notice this monster */
        borg_is_kill[(byte)(r_ptr->d_char)] = TRUE;
    }


    /*** Special counters ***/

    /* Count racial appearances */
    C_MAKE(borg_race_count, z_info->r_max, s16b);

    /* Count racial deaths */
    C_MAKE(borg_race_death, z_info->r_max, s16b);


    /*** XXX XXX XXX Hack -- Cheat ***/

    /* Hack -- Extract dead uniques */
    for (i = 1; i < z_info->r_max-1; i++)
    {
        monster_race *r_ptr = &r_info[i];

        /* Skip non-monsters */
        if (!r_ptr->name) continue;

        /* Skip non-uniques */
        if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

        /* Mega-Hack -- Access "dead unique" list */
        if (r_ptr->max_num == 0) borg_race_death[i] = 1;
    }

}



#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
