%module player

%{

#include "angband.h"

%}

typedef const char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;

extern player_type *p_ptr;

%typemap(memberin) char name[32]
{
	strncpy(player_name, $source, 32);
}

%typemap(memberout) char name[32]
{
	$target = player_name;
}

%typemap(memberin) char player_base[32]
{
	strncpy(player_base, $source, 32);
}

%typemap(memberout) char player_base[32]
{
	$target = player_base;
}

%typemap(memberin) char died_from[80]
{
	strncpy(died_from, $source, 80);
}

%typemap(memberout) char died_from[80]
{
	$target = died_from;
}

%typemap(memberin) char history[4][60]
{
	strncpy(history, $source, 60);
}

%typemap(memberout) char history[4][60]
{
	$target = history;
}

%typemap(memberin) int dun_level
{
	dun_level = $source;
}

%typemap(memberout) int dun_level
{
	$target = dun_level;
}

%typemap(memberin) s16b py
{
	py = $source;
}

%typemap(memberout) s16b py
{
	$target = py;
}

%typemap(memberin) s16b px
{
	px = $source;
}

%typemap(memberout) s16b px
{
	$target = px;
}

%typemap(memberin) s16b csp
{
	$target = $source;

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);
	
	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);
}

%typemap(memberin) s16b msp
{
	$target = $source;

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);
	
	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);
}


%include typemap.i


/*
 * Maximum number of "normal" pack slots, and the index of the "overflow"
 * slot, which can hold an item, but only temporarily, since it causes the
 * pack to "overflow", dropping the "last" item onto the ground.  Since this
 * value is used as an actual slot, it must be less than "INVEN_WIELD" (below).
 * Note that "INVEN_PACK" is probably hard-coded by its use in savefiles, and
 * by the fact that the screen can only show 23 items plus a one-line prompt.
 */
#define INVEN_PACK              23

/*
 * Indexes used for various "equipment" slots (hard-coded by savefiles, etc).
 */
#define INVEN_WIELD     24
#define INVEN_BOW       25
#define INVEN_LEFT      26
#define INVEN_RIGHT     27
#define INVEN_NECK      28
#define INVEN_LITE      29
#define INVEN_BODY      30
#define INVEN_OUTER     31
#define INVEN_ARM       32
#define INVEN_HEAD      33
#define INVEN_HANDS     34
#define INVEN_FEET      35

/*
 * Total number of inventory slots (hard-coded).
 */
#define INVEN_TOTAL     36



typedef struct player_type
{
	s16b oldpy;		/* Previous player location */
	s16b oldpx;		/* Previous player location */

	byte psex;			/* Sex index */
	byte prace;			/* Race index */
	byte pclass;		/* Class index */
	byte realm1;        /* First magic realm */
	byte realm2;        /* Second magic realm */
	byte oops;			/* Unused */

	byte hitdie;		/* Hit dice (sides) */
	u16b expfact;       /* Experience factor
						 * Note: was byte, causing overflow for Amberite
						 * characters (such as Amberite Paladins)
						 */

	s16b age;			/* Characters age */
	s16b ht;			/* Height */
	s16b wt;			/* Weight */
	s16b sc;			/* Social Class */


	s32b au;			/* Current Gold */

	s32b max_exp;		/* Max experience */
	s32b exp;			/* Cur experience */
	u16b exp_frac;		/* Cur exp frac (times 2^16) */

	%name(level) s16b lev;			/* Level */

	s16b town_num;			/* Current town number */
	s16b arena_number;		/* monster number in arena */
	s16b inside_arena;		/* Is character inside arena? */
	s16b inside_quest;		/* Inside quest level */

	s16b rewards[MAX_BACT];	/* Status of rewards in town */

	s32b wilderness_x;	/* Coordinates in the wilderness */
	s32b wilderness_y;

	s16b mhp;			/* Max hit pts */
	s16b chp;			/* Cur hit pts */
	u16b chp_frac;		/* Cur hit frac (times 2^16) */

	s16b msp;			/* Max mana pts */
	s16b csp;			/* Cur mana pts */
	u16b csp_frac;		/* Cur mana frac (times 2^16) */

	s16b max_plv;		/* Max Player Level */
	s16b max_dlv;		/* Max level explored */

	s16b stat_max[6];	/* Current "maximal" stat values */
	//s16b stat_cur[6];	/* Current "natural" stat values */

	s16b fast;			/* Timed -- Fast */
	s16b slow;			/* Timed -- Slow */
	s16b blind;			/* Timed -- Blindness */
	s16b paralyzed;		/* Timed -- Paralysis */
	s16b confused;		/* Timed -- Confusion */
	s16b afraid;		/* Timed -- Fear */
	s16b image;			/* Timed -- Hallucination */
	s16b poisoned;		/* Timed -- Poisoned */
	s16b cut;			/* Timed -- Cut */
	s16b stun;			/* Timed -- Stun */

	s16b protevil;		/* Timed -- Protection */
	s16b invuln;		/* Timed -- Invulnerable */
	s16b hero;			/* Timed -- Heroism */
	s16b shero;			/* Timed -- Super Heroism */
	s16b shield;		/* Timed -- Shield Spell */
	s16b blessed;		/* Timed -- Blessed */
	s16b tim_invis;		/* Timed -- See Invisible */
	s16b tim_infra;		/* Timed -- Infra Vision */

	s16b oppose_acid;	/* Timed -- oppose acid */
	s16b oppose_elec;	/* Timed -- oppose lightning */
	s16b oppose_fire;	/* Timed -- oppose heat */
	s16b oppose_cold;	/* Timed -- oppose cold */
	s16b oppose_pois;	/* Timed -- oppose poison */


	s16b tim_esp;       /* Timed ESP */
	s16b wraith_form;   /* Timed wraithform */

	s16b resist_magic;  /* Timed Resist Magic (later) */
	s16b tim_xtra1;     /* Later */
	s16b tim_xtra2;     /* Later */
	s16b tim_xtra3;     /* Later */
	s16b tim_xtra4;     /* Later */
	s16b tim_xtra5;     /* Later */
	s16b tim_xtra6;     /* Later */
	s16b tim_xtra7;     /* Later */
	s16b tim_xtra8;     /* Later */

	s16b chaos_patron;
	u32b muta1;
	u32b muta2;
	u32b muta3;

	s16b word_recall;	/* Word of recall counter */

	s16b energy;		/* Current energy */

	s16b food;			/* Current nutrition */

	byte confusing;		/* Glowing hands */
	byte searching;		/* Currently searching */

	s16b new_spells;	/* Number of spells available */

	s16b old_spells;

	bool old_cumber_armor;
	bool old_cumber_glove;
	bool old_heavy_wield;
	bool old_heavy_shoot;
	bool old_icky_wield;

	s16b old_lite;		/* Old radius of lite (if any) */
	s16b old_view;		/* Old radius of view (if any) */

	s16b old_food_aux;	/* Old value of food */


	bool cumber_armor;	/* Mana draining armor */
	bool cumber_glove;	/* Mana draining gloves */
	bool heavy_wield;	/* Heavy weapon */
	bool heavy_shoot;	/* Heavy shooter */
	bool icky_wield;	/* Icky weapon */

	s16b cur_lite;		/* Radius of lite (if any) */


	u32b notice;		/* Special Updates (bit flags) */
	u32b update;		/* Pending Updates (bit flags) */
	u32b redraw;		/* Normal Redraws (bit flags) */
	u32b window;		/* Window Redraws (bit flags) */

	s16b stat_use[6];	/* Current modified stats */
	s16b stat_top[6];	/* Maximal modified stats */

	s16b stat_add[6];	/* Modifiers to stat values */
	s16b stat_ind[6];	/* Indexes into stat tables */

	bool immune_acid;	/* Immunity to acid */
	bool immune_elec;	/* Immunity to lightning */
	bool immune_fire;	/* Immunity to fire */
	bool immune_cold;	/* Immunity to cold */

	bool resist_acid;	/* Resist acid */
	bool resist_elec;	/* Resist lightning */
	bool resist_fire;	/* Resist fire */
	bool resist_cold;	/* Resist cold */
	bool resist_pois;	/* Resist poison */

	bool resist_conf;	/* Resist confusion */
	bool resist_sound;	/* Resist sound */
	bool resist_lite;	/* Resist light */
	bool resist_dark;	/* Resist darkness */
	bool resist_chaos;	/* Resist chaos */
	bool resist_disen;	/* Resist disenchant */
	bool resist_shard;	/* Resist shards */
	bool resist_nexus;	/* Resist nexus */
	bool resist_blind;	/* Resist blindness */
	bool resist_neth;	/* Resist nether */
	bool resist_fear;	/* Resist fear */

	bool reflect;       /* Reflect 'bolt' attacks */
	bool sh_fire;       /* Fiery 'immolation' effect */
	bool sh_elec;       /* Electric 'immolation' effect */

	bool anti_magic;    /* Anti-magic */
	bool anti_tele;     /* Prevent teleportation */

	bool sustain_str;	/* Keep strength */
	bool sustain_int;	/* Keep intelligence */
	bool sustain_wis;	/* Keep wisdom */
	bool sustain_dex;	/* Keep dexterity */
	bool sustain_con;	/* Keep constitution */
	bool sustain_chr;	/* Keep charisma */

	bool aggravate;		/* Aggravate monsters */
	bool teleport;		/* Random teleporting */

	bool exp_drain;		/* Experience draining */

	bool ffall;			/* No damage falling */
	bool lite;			/* Permanent light */
	bool free_act;		/* Never paralyzed */
	bool see_inv;		/* Can see invisible */
	bool regenerate;	/* Regenerate hit pts */
	bool hold_life;		/* Resist life draining */
	bool telepathy;		/* Telepathy */
	bool slow_digest;	/* Slower digestion */
	bool bless_blade;	/* Blessed blade */
	bool xtra_might;	/* Extra might bow */
	bool impact;		/* Earthquake blows */

	s16b dis_to_h;		/* Known bonus to hit */
	s16b dis_to_d;		/* Known bonus to dam */
	s16b dis_to_a;		/* Known bonus to ac */

	s16b dis_ac;		/* Known base ac */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to dam */
	s16b to_a;			/* Bonus to ac */

	s16b ac;			/* Base ac */

	s16b see_infra;		/* Infravision range */

	s16b skill_dis;		/* Skill: Disarming */
	s16b skill_dev;		/* Skill: Magic Devices */
	s16b skill_sav;		/* Skill: Saving throw */
	s16b skill_stl;		/* Skill: Stealth factor */
	s16b skill_srh;		/* Skill: Searching ability */
	s16b skill_fos;		/* Skill: Searching frequency */
	s16b skill_thn;		/* Skill: To hit (normal) */
	s16b skill_thb;		/* Skill: To hit (shooting) */
	s16b skill_tht;		/* Skill: To hit (throwing) */
	s16b skill_dig;		/* Skill: Digging */

	s16b num_blow;		/* Number of blows */
	s16b num_fire;		/* Number of shots */

	byte tval_xtra;		/* Correct xtra tval */

	byte tval_ammo;		/* Correct ammo tval */

	s16b pspeed;		/* Current speed */

	/*** Pet commands ***/
	byte pet_follow_distance; /* Length of the imaginary "leash" for pets */
	byte pet_open_doors;      /* flag - allow pets to open doors */
	byte pet_pickup_items;    /* flag - allow pets to pickup items */

	/*** Temporary fields ***/
	byte exit_bldg;			/* Goal obtained in arena? */
	byte leftbldg;			/* did we just leave a special area? */
	bool leaving;			/* True if player is leaving */

	/* Virtual fields */
#if 0
	char name[32];

	int dun_level;

	s16b py;
	s16b px;

	char player_base[32];
	char died_from[80];
	char history[4][60];
#endif
} player_type;


%addmethods player_type
{
	int stat_cur_get(int index)
	{
		return p_ptr->stat_cur[index];
	}

	object_type *inventory(int index)
	{
		return &inventory[index];
	}

	void inven_item_increase(int item, int num)
	{
		inven_item_increase(item, num);
	}

	void inven_item_optimize(int item)
	{
		inven_item_optimize(item);
	}

	void carry(object_type *o_ptr)
	{
		inven_carry(o_ptr);
	}

	/* Places the player in the dungeon */
	/* Returns -1 on success and 0 on failure */
	s16b place(int y, int x)
	{
		return player_place(y, x);
	}

	void teleport(int dis)
	{
		teleport_player(dis);
	}

	void teleport_to(int ny, int nx)
	{
		teleport_player_to(ny, nx);
	}

	void teleport_level(void)
	{
		teleport_player_level();
	}

	void recall(int turns)
	{
		recall_player(turns);
	}

	void word_of_recall(void)
	{
		word_of_recall();
	}

	bool disenchant(int mode)
	{
		return apply_disenchant(mode);
	}

	void mutate_player(void)
	{
		mutate_player();
	}

	void nexus(monster_type *m_ptr)
	{
		apply_nexus(m_ptr);
	}

	void phlogiston(void)
	{
		phlogiston();
	}

	void brand_weapon(int brand_type)
	{
		brand_weapon(brand_type);
	}

	void call_the_void(void)
	{
		call_the_();
	}

	void fetch(int dir, int wgt, bool require_los)
	{
		fetch(dir, wgt, require_los);
	}

	void alter_reality(void)
	{
		alter_reality();
	}

	bool set_afraid(int v)
	{
		return set_afraid(v);
	}

	bool set_shero(int v)
	{
		return set_shero(v);
	}

	bool hp_player(int num)
	{
		return hp_player(num);
	}

	bool set_blind(int v)
	{
		return set_blind(v);
	}

	bool set_confused(int v)
	{
		return set_confused(v);
	}

	bool set_poisoned(int v)
	{
		return set_poisoned(v);
	}

	bool set_paralyzed(int v)
	{
		return set_paralyzed(v);
	}

	bool set_image(int v)
	{
		return set_image(v);
	}

	bool set_fast(int v)
	{
		return set_fast(v);
	}

	bool set_slow(int v)
	{
		return set_slow(v);
	}

	bool set_shield(int v)
	{
		return set_shield(v);
	}

	bool set_blessed(int v)
	{
		return set_blessed(v);
	}

	bool set_hero(int v)
	{
		return set_hero(v);
	}

	bool set_protevil(int v)
	{
		return set_protevil(v);
	}

	bool set_invuln(int v)
	{
		return set_invuln(v);
	}

	bool set_tim_invis(int v)
	{
		return set_tim_invis(v);
	}

	bool set_tim_infra(int v)
	{
		return set_tim_infra(v);
	}

	bool set_oppose_acid(int v)
	{
		return set_oppose_acid(v);
	}

	bool set_oppose_elec(int v)
	{
		return set_oppose_elec(v);
	}

	bool set_oppose_fire(int v)
	{
		return set_oppose_fire(v);
	}

	bool set_oppose_cold(int v)
	{
		return set_oppose_cold(v);
	}

	bool set_oppose_pois(int v)
	{
		return set_oppose_pois(v);
	}

	bool set_stun(int v)
	{
		return set_stun(v);
	}

	bool set_cut(int v)
	{
		return set_cut(v);
	}

	bool set_food(int v)
	{
		return set_food(v);
	}

	bool inc_stat(int stat)
	{
		return inc_stat(stat);
	}

	bool dec_stat(int stat, int amount, int permanent)
	{
		return dec_stat(stat, amount, permanent);
	}

	bool res_stat(int stat)
	{
		return res_stat(stat);
	}

	bool do_dec_stat(int stat)
	{
		return do_dec_stat(stat);
	}

	bool do_res_stat(int stat)
	{
		return do_res_stat(stat);
	}

	bool do_inc_stat(int stat)
	{
		return do_inc_stat(stat);
	}

	bool restore_level(void)
	{
		return restore_level();
	}

	bool lose_all_info(void)
	{
		return lose_all_info();
	}

	void gain_exp(s32b amount)
	{
		gain_exp(amount);
	}

	void lose_exp(s32b amount)
	{
		lose_exp(amount);
	}

	void do_poly_self(void)
	{
		do_poly_self();
	}
}


/*
 * Player constants
 */
#define PY_MAX_EXP      99999999L       /* Maximum exp */
#define PY_MAX_GOLD     999999999L      /* Maximum gold */
#define PY_MAX_LEVEL    50              /* Maximum level */

/*
 * Player "food" crucial values
 */
#define PY_FOOD_MAX             15000   /* Food value (Bloated) */
#define PY_FOOD_FULL    10000   /* Food value (Normal) */
#define PY_FOOD_ALERT   2000    /* Food value (Hungry) */
#define PY_FOOD_WEAK    1000    /* Food value (Weak) */
#define PY_FOOD_FAINT   500             /* Food value (Fainting) */
#define PY_FOOD_STARVE  100             /* Food value (Starving) */

/*
 * Player regeneration constants
 */
#define PY_REGEN_NORMAL         197             /* Regen factor*2^16 when full */
#define PY_REGEN_WEAK           98              /* Regen factor*2^16 when weak */
#define PY_REGEN_FAINT          33              /* Regen factor*2^16 when fainting */
#define PY_REGEN_HPBASE         1442    /* Min amount hp regen*2^16 */
#define PY_REGEN_MNBASE         524             /* Min amount mana regen*2^16 */

/*
 * Indexes of the various "stats" (hard-coded by savefiles, etc).
 */
#define A_STR   0
#define A_INT   1
#define A_WIS   2
#define A_DEX   3
#define A_CON   4
#define A_CHR   5

/*
 * Player sex constants (hard-coded by save-files, arrays, etc)
 */
#define SEX_FEMALE              0
#define SEX_MALE                1
