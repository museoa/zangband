/* File: zborg3.h */
/* Purpose: Header file for "borg3.c" -BEN- */

#ifndef INCLUDED_BORG3_H
#define INCLUDED_BORG3_H

#include "angband.h"

#ifdef ALLOW_BORG

/*
 * This file provides support for "borg3.c".
 */

#include "zborg1.h"


/*
 * Hack -- location of the "Lv Mana Fail" prompt
 */
#define ROW_SPELL   1
#define COL_SPELL   20+35




/*
 * Forward declare
 */
typedef struct borg_item borg_item;
typedef struct borg_shop borg_shop;
typedef struct borg_mind borg_mind;


/*
 * A structure holding information about an object.  120 bytes.
 *
 * The "iqty" is zero if the object is "missing"
 * The "kind" is zero if the object is "unaware" (or missing)
 * The "able" is zero if the object is "unknown" (or unaware or missing)
 *
 * Note that unaware items will have a "tval" but an invalid "sval".
 */
struct borg_item
{
	char desc[80];	/* Actual Description */

	cptr note;	/* Pointer to tail of 'desc' */

	s16b kind;	/* Kind index */

	bool able;	/* True if item is identified */

	bool fully_identified;	/* True if item is fully identified (AJG) */

	byte tval;	/* Item type */
	byte sval;	/* Item sub-type */
	s16b pval;	/* Item extra-info */

	byte discount;	/* Discount */

	byte iqty;	/* Number of items */

	s16b weight;	/* Probable weight */

	u16b xtra_name;	/* Extra Name (Artifacts and ego items) */
#if 0
	/*
	 * These do not exist any more...
	 * we have to think everything is a randart.
	 * (Since most ego items have random abilities - this
	 * isn't too much of a problem.
	 */
	int name1;	/* Artifact index (if any) */
	int name2;	/* Ego-item index (if any) */
#endif /* 0 */
	s16b timeout;	/* Timeout counter */

	s16b to_h;	/* Bonus to hit */
	s16b to_d;	/* Bonus to dam */
	s16b to_a;	/* Bonus to ac */
	s16b ac;	/* Armor class */
	byte dd;	/* Damage dice */
	byte ds;	/* Damage sides */

	byte level;	/* Level apw */

	s32b cost;	/* Cost (in stores) */

	s32b value;	/* Value (estimated) */

	bool cursed;	/* Item is cursed */
	u32b flags1;	/* Extracted item flags (set 1) */
	u32b flags2;	/* Extracted item flags (set 2) */
	u32b flags3;	/* Extracted item flags (set 3) */
};


/*
 * A store
 */
struct borg_shop
{
	s16b when;	/* Time stamp */

	s16b xtra;	/* Something unused */

	s16b page;	/* Current page */
	s16b more;	/* Number of pages */

	borg_item ware[24];	/* Store contents */
};



/*
 * Spell method values
 */

#define BORG_MAGIC_ICK      0	/* Spell is illegible */
#define BORG_MAGIC_NOP      1	/* Spell takes no arguments */
#define BORG_MAGIC_EXT      2	/* Spell needs 'space' after cast */
#define BORG_MAGIC_AIM      3	/* Spell requires a direction */
#define BORG_MAGIC_OBJ      4	/* Spell requires a pack object */
#define BORG_MAGIC_WHO      5	/* Spell requires a monster symbol */


/*
 * Spell status values
 */

#define BORG_MAGIC_ICKY     0	/* Spell is illegible */
#define BORG_MAGIC_LOST     1	/* Spell is forgotten */
#define BORG_MAGIC_HIGH     2	/* Spell is high level */
#define BORG_MAGIC_OKAY     3	/* Spell is learnable */
#define BORG_MAGIC_TEST     4	/* Spell is untried */
#define BORG_MAGIC_KNOW     5	/* Spell is known */

/*
 * Define some MindCraft Spells
 */
#define MIND_PRECOGNIT 	 0
#define MIND_NEURAL_BL	 1
#define MIND_MINOR_DISP  2
#define MIND_MAJOR_DISP  3
#define MIND_DOMINATION  4
#define MIND_PULVERISE   5
#define MIND_CHAR_ARMOUR 6
#define MIND_PSYCHOMETRY 7
#define MIND_MIND_WAVE   8
#define MIND_ADRENALINE  9
#define MIND_PSYCHIC_DR	10
#define MIND_TELE_WAVE	11


/*
 * Forward declare
 */
typedef struct borg_magic borg_magic;


/*
 * A spell/prayer in a book
 */
struct borg_magic
{
	cptr name;	/* Textual name */

	cptr realm_name;	/* Text name of realm */

	byte realm;	/* number Realm, see defines.h */

	byte status;	/* Status (see above) */

	byte method;	/* Method (see above) */

	byte rating;	/* Usefulness */

	byte level;	/* Required level */

	byte power;	/* Required power */

	byte sfail;	/* Minimum chance of failure */

	byte cheat;	/* Actual "spell index" (or 99) */

	s32b times;	/* Times this spell was cast */
};

/*
 * A spell/prayer in a book
 */
struct borg_mind
{
	cptr name;	/* Textual name */

	byte level;	/* Required level */

	byte power;	/* Required power --mana cost */

	byte sfail;	/* Minimum chance of failure */

	char letter;	/* Actual "spell index" (a,b,c...) */

	s32b times;	/* Times this spell was cast */
};


/*
 * Some variables
 */

extern borg_shop *borg_shops;	/* Current "shops" */


/*
 * Safety arrays for simulating possible worlds
 */

extern borg_item *safe_home;	/* Safety "home" */

extern borg_shop *safe_shops;	/* Safety "shops" */


/*
 * Spell casting information
 */

extern borg_magic borg_magics[8][4][8];	/* Spell info, including realm */
extern borg_mind borg_minds[MINDCRAFT_MAX];



/*
 * Determine which slot an item could be wielded into
 */
extern int borg_wield_slot(borg_item *item);

/*
 * Count the items of a given tval/sval
 */
extern int borg_count(int tval, int sval);

/*
 * Find an item with a given tval/sval
 */
extern list_item *borg_slot(int tval, int sval);

/*
 * Find corresponding index of item
 */
extern int look_up_index(list_item *l_ptr);

/*
 * Item usage functions
 */
extern bool borg_refuel_torch(void);
extern bool borg_refuel_lantern(void);

/*
 * Item usage functions (by sval)
 */
extern bool borg_eat_food(int sval);
extern bool borg_quaff_crit(bool no_check);
extern bool borg_quaff_potion(int sval);
extern bool borg_eat_unknown(void);
extern bool borg_use_unknown(void);
extern bool borg_quaff_unknown(void);
extern bool borg_read_unknown(void);
extern bool borg_read_scroll(int sval);
extern bool borg_equips_rod(int sval);
extern bool borg_zap_rod(int sval);
extern bool borg_aim_wand(int sval);
extern bool borg_use_staff(int sval);
extern bool borg_use_staff_fail(int sval);
extern bool borg_equips_staff_fail(int sval);

/*
 * Artifact usage function (by index)
 */
extern bool borg_activate_artifact(int name1, bool secondary);	/* apw */
extern bool borg_equips_artifact(int name1, int location);	/* apw */
extern bool borg_activate_dragon(int drag_sval);	/* apw */
extern bool borg_equips_dragon(int drag_sval);	/* apw */


/*
 * Spell functions
 */
extern bool borg_spell_legal(int realm, int book, int what);
extern bool borg_spell_okay(int realm, int book, int what);
extern bool borg_spell(int realm, int book, int what);
extern bool borg_spell_fail(int realm, int book, int what, int allow_fail);
extern bool borg_spell_okay_fail(int realm, int book, int what, int allow_fail);
extern bool borg_spell_legal_fail(int realm, int book, int what,
								  int allow_fail);

/*
 * Mindcraft Spell functions
 */
extern bool borg_mindcr_legal(int spell, int level);
extern bool borg_mindcr_okay(int spell, int level);
extern bool borg_mindcr(int spell, int level);
extern bool borg_mindcr_fail(int spell, int level, int allow_fail);
extern bool borg_mindcr_okay_fail(int spell, int level, int allow_fail);
extern bool borg_mindcr_legal_fail(int spell, int level, int allow_fail);

/*
 * Racial abilities
 */
extern bool borg_racial_check(int race, bool check_fail);
extern bool borg_racial(int race);

/*
 * Cheat/Parse the "equip" and "inven" screens.
 */
extern void borg_cheat_equip(void);

/*
 * Cheat/Parse the "spell" screen
 */
extern void borg_cheat_spell(int realm);

/*
 * Hack -- prepare stuff based on the race/class
 */
extern void prepare_race_class_info(void);

extern void borg_clear_3(void);

/*
 * Initialize this file
 */
extern void borg_init_3(void);


#endif

#endif
