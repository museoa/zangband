/* File: borg3.c */

/* Purpose: Object and Spell routines for the Borg -BEN- */

#include "angband.h"


#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg3.h"



/*
 * This file helps the Borg analyze "objects" and "shops", and to
 * deal with objects and spells.
 */



/*
 * Some variables
 */

borg_item *borg_items;      /* Current "inventory" */

borg_shop *borg_shops;      /* Current "shops" */



/*
 * Safety arrays for simulating possible worlds
 */

borg_item *safe_items;      /* Safety "inventory" */
borg_item *safe_home;       /* Safety "home stuff" */

borg_shop *safe_shops;      /* Safety "shops" */


/*
 * Spell info
 */

borg_magic borg_magics[8][4][8];   /* Spell info, by realm/book/what*/
borg_mind borg_minds[MINDCRAFT_MAX];


/* Food Names */
static char *food_syllable1[] =
{
    "BBQ ", "Boiled ", "Fresh ", "Frozen ", "Burned ", "Rotten ", "Raw ", "Toasted ", "Broiled ", "Baked ", "Fried ", "Buttered ", "Steamed ", "Gramma's ",
};

/* Food Names */
static char *food_syllable2[] =
{
    "Pizza", "Eggs", "Spam", "Oatmeal", "Chicken", "Bacon", "Peanutbutter", "Roast Beef", "Cheese", "Toast", "Hamburger", "Carrots", "Corn", "Potato", "Pork Chops", "Chinese Takeout", "Cookies",
};

/* Slime Molds */
static char *mold_syllable1[] =
{
    "Ab", "Ac", "Ad", "Af", "Agr", "Ast", "As", "Al", "Adw", "Adr", "Ar", "B", "Br", "C", "Cr", "Ch", "Cad", "D", "Dr", "Dw", "Ed", "Eth", "Et", "Er", "El", "Eow", "F", "Fr", "G", "Gr", "Gw", "Gal", "Gl", "H", "Ha", "Ib", "Jer", "K", "Ka", "Ked", "L", "Loth"
, "Lar", "Leg", "M", "Mir", "N", "Nyd", "Ol", "Oc", "On", "P", "Pr", "R", "Rh", "S", "Sev", "T", "Tr", "Th", "V", "Y", "Z", "W", "Wic",
};

static char *mold_syllable2[] =
{
    "a", "adrie", "ara", "e", "ebri", "ele", "ere", "i", "io", "ithra", "ilma", "il-Ga", "ili", "o", "orfi", "u", "y",
};

static char *mold_syllable3[] =
{
    "bur", "fur", "gan", "gnus", "gnar", "li", "lin", "lir", "mli", "nar", "nus", "rin", "ran", "sin", "sil", "sur",
};


/*
 * Hack -- help analyze the magic
 *
 * The comments yield the "name" of the spell or prayer.
 *
 * Also, the leading letter in the comment indicates how we use the
 * spell or prayer, if at all, using "A" for "attack", "D" for "call
 * light" and "detection", "E" for "escape", "H" for healing, "O" for
 * "object manipulation", and "F" for "terrain feature manipulation",
 * plus "!" for entries that can soon be handled.
 */

static byte borg_magic_method[8][4][8] =
{
    {/* 0 Realm 0 -- Non spell caster*/
        {/* Book0... (sval 0) */
            BORG_MAGIC_ICK  /* ! "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /* ! "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */},
        {/* Book1... (sval 1) */
            BORG_MAGIC_ICK  /* ! "(blank)" */,
            BORG_MAGIC_ICK  /* ! "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */},
        {/* Book0... (sval 2) */
            BORG_MAGIC_ICK  /* ! "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /* ! "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */},
        {/* Book3... (sval 3) */
            BORG_MAGIC_ICK  /* ! "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /* ! "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */,
            BORG_MAGIC_ICK  /*   "(blank)" */}
	}, /* end of realm 0 */

    {/* 1 Life Realm */
        {/* Common Prayers (sval 0) */
            BORG_MAGIC_NOP  /*   "Detect Evil" */,
            BORG_MAGIC_NOP  /*   "Cure Light Wounds" */,
            BORG_MAGIC_NOP  /*   "Bless" */,
            BORG_MAGIC_NOP  /* H "Remove Fear" */,
            BORG_MAGIC_NOP  /* D "Call Light" */,
            BORG_MAGIC_NOP  /* D "Find Traps" */,
            BORG_MAGIC_NOP  /* D "Cure Medium Wounds" */,
            BORG_MAGIC_NOP  /*   "Satisfy Hunger" */,},

        {/* High Mass (sval 1) */
            BORG_MAGIC_NOP  /*   "Remove Curse" */,
            BORG_MAGIC_NOP  /* E "Cure Poison" */,
            BORG_MAGIC_NOP  /* H "Cure Crit Wounds" */,
            BORG_MAGIC_NOP  /*   "See Inv" */,
            BORG_MAGIC_AIM  /*   "Holy Orb" */,
            BORG_MAGIC_NOP  /* H "PFE" */,
            BORG_MAGIC_NOP  /*   "Healing" */,
            BORG_MAGIC_NOP  /*   "Rune of Protection" */,},

        {/* Book of the Unicorn (sval 2) */
            BORG_MAGIC_NOP  /* H "Exorcism" */,
            BORG_MAGIC_NOP  /* A "Dispel Curse" */,
            BORG_MAGIC_NOP  /* H "Disp Undead and Demon" */,
            BORG_MAGIC_NOP  /*   "Day of Dove" */,
            BORG_MAGIC_NOP  /*   "Dispel Evil" */,
            BORG_MAGIC_NOP  /* D "Banishment" */,
            BORG_MAGIC_NOP  /* H "Holy Word" */,
            BORG_MAGIC_NOP  /*   "Warding True" */},

        {/* Blessings of the Grail (sval 3) */
            BORG_MAGIC_NOP  /*   "Heroism" */,
            BORG_MAGIC_NOP  /* ! "Prayer" */,
            BORG_MAGIC_NOP  /* H "Bless Weapon" */,
            BORG_MAGIC_NOP  /* ! "Restoration" */,
            BORG_MAGIC_NOP  /*   "Healing True" */,
            BORG_MAGIC_OBJ  /* ! "Holy Vision" */,
            BORG_MAGIC_NOP  /*   "Divine Intervent" */,
            BORG_MAGIC_NOP  /*   "Holy Invuln" */}

    }, /* endof Life Realm */

    {/*2. Sorcery Realm*/
        {/* Beginners Handbook (sval 0) */
            BORG_MAGIC_NOP  /*   "Detect Monster" */,
            BORG_MAGIC_NOP  /*   "Phase Door" */,
            BORG_MAGIC_NOP  /*   "Detect Doors & Traps" */,
            BORG_MAGIC_NOP  /*   "Light Area" */,
            BORG_MAGIC_AIM  /*   "Confuse Monster" */,
            BORG_MAGIC_NOP  /*   "Teleport Self" */,
            BORG_MAGIC_NOP  /*   "Sleep Monster" */,
            BORG_MAGIC_OBJ  /*   "Recharging" */},

        {/* Master Sorc (sval 1) */
            BORG_MAGIC_NOP  /*   "Magic Map" */,
            BORG_MAGIC_OBJ  /*   "Ident" */,
            BORG_MAGIC_AIM  /*   "Slow Monster" */,
            BORG_MAGIC_NOP  /*   "Mass Sleep " */,
            BORG_MAGIC_AIM  /*   "Teleport Away" */,
            BORG_MAGIC_NOP  /*   "Haste Self" */,
            BORG_MAGIC_NOP  /*   "Detection True" */,
            BORG_MAGIC_OBJ  /*   "*ID*" */},

        {/* Pattern Sorc (sval 2) */
            BORG_MAGIC_NOP  /*   "Detect Obj" */,
            BORG_MAGIC_NOP  /*   "Detect Enchant" */,
            BORG_MAGIC_ICK  /*   "Charm Mon" */,
            BORG_MAGIC_AIM  /*   "Dimension Door" */,
            BORG_MAGIC_NOP  /*   "Sense Minds" */,
            BORG_MAGIC_NOP  /*   "Self Knowledge" */,
            BORG_MAGIC_NOP  /*   "Teleport Level" */,
            BORG_MAGIC_NOP  /*   "Word of Recall" */},

        {/* Grimoir of Power (sval 3) */
            BORG_MAGIC_AIM  /*   "Stasis" */,
            BORG_MAGIC_ICK  /*   "Telekinesis" */,
            BORG_MAGIC_ICK  /*   "Explosive Rune" */,
            BORG_MAGIC_NOP  /*   "Clairvoyance" */,
            BORG_MAGIC_OBJ  /*   "*Enchant Weap" */,
            BORG_MAGIC_OBJ  /*   "*Enchant Armor" */,
            BORG_MAGIC_ICK  /*   "Alchemy" */,
            BORG_MAGIC_NOP  /*   "GOI" */}
    },/* End of Sorcery Realm */

    {/* 3 Nature Realm */
        {/* Call of the Wild (sval 0) */
            BORG_MAGIC_NOP  /*   "Detect Creature" */,
            BORG_MAGIC_NOP  /*   "First Aid" */,
            BORG_MAGIC_NOP  /* ! "Detect Door" */,
            BORG_MAGIC_NOP  /*   "Foraging" */,
            BORG_MAGIC_NOP  /*   "Daylight" */,
            BORG_MAGIC_AIM  /*   "Animal Taming" */,
            BORG_MAGIC_NOP  /*   "Resist Environment" */,
            BORG_MAGIC_NOP  /*   "Cure Wound&Poison" */},
        {/* Nature Mastery (sval 1) */
            BORG_MAGIC_AIM  /* ! "Stone to Mud" */,
            BORG_MAGIC_AIM  /* ! "Lightning Bolt" */,
            BORG_MAGIC_NOP  /*   "Nature Awareness" */,
            BORG_MAGIC_AIM  /*   "Frost Bolt" */,
            BORG_MAGIC_AIM  /*   "Ray of Sunlight" */,
            BORG_MAGIC_NOP  /*   "Entangle" */,
            BORG_MAGIC_ICK  /*   "Summon Animals" */,
            BORG_MAGIC_NOP  /*   "Herbal Healing" */},
        {/* Nature Gifts (sval 2) */
            BORG_MAGIC_NOP  /* ! "Door Building" */,
            BORG_MAGIC_NOP  /*   "Stair Building" */,
            BORG_MAGIC_NOP  /* ! "Stone Skin" */,
            BORG_MAGIC_NOP  /*   "Resistance True" */,
            BORG_MAGIC_NOP  /*   "Animal Friend" */,
            BORG_MAGIC_OBJ  /*   "Stone Tell" */,
            BORG_MAGIC_NOP  /*   "Wall of Stone" */,
            BORG_MAGIC_OBJ  /*   "Protect From Corros." */},
        {/* Natures Wrath (sval 3) */
            BORG_MAGIC_NOP  /* ! "Earthquake" */,
            BORG_MAGIC_NOP  /*   "Whirlwind" */,
            BORG_MAGIC_AIM  /* ! "Blizzard" */,
            BORG_MAGIC_AIM  /*   "Lightning" */,
            BORG_MAGIC_AIM  /*   "Whirpool" */,
            BORG_MAGIC_NOP  /*   "Call Sunlight" */,
            BORG_MAGIC_OBJ  /*   "Elemental Brand" */,
            BORG_MAGIC_NOP  /*   "Natures Wrath" */}
	}, /* end of Natural realm  */

    {/* 4.Chaos Realm */
        {/* Sign of Chaos... (sval 0) */
            BORG_MAGIC_AIM  /* "Magic Missile" */,
            BORG_MAGIC_NOP  /* "Trap/Door Dest" */,
            BORG_MAGIC_NOP  /* "Flash of Light" */,
            BORG_MAGIC_NOP  /* "Touch of Conf" */,
            BORG_MAGIC_NOP  /* "ManaBurst" */,
            BORG_MAGIC_AIM  /* "Fire Bolt" */,
            BORG_MAGIC_AIM  /* "Fist of Force" */,
            BORG_MAGIC_NOP  /* "Teleport" */},
        {/* Chaos Mastery... (sval 1) */
            BORG_MAGIC_ICK  /*   "Wonder" */,
            BORG_MAGIC_AIM  /*   "Chaos Bolt" */,
            BORG_MAGIC_NOP  /*   "Sonic Boom" */,
            BORG_MAGIC_AIM  /*   "Doom Beam" */,
            BORG_MAGIC_AIM  /*   "Fire Ball" */,
            BORG_MAGIC_AIM  /*   "Teleport Other" */,
            BORG_MAGIC_NOP  /*   "Word of Dest" */,
            BORG_MAGIC_NOP  /*   "Invoke Logrus" */},
        {/* Chaos Channels (sval 2) */
            BORG_MAGIC_AIM  /*   "Polymorph Other" */,
            BORG_MAGIC_NOP  /*   "Chain Lightn" */,
            BORG_MAGIC_OBJ  /*   "Arcane Binding" */,
            BORG_MAGIC_AIM  /*   "Disintegration" */,
            BORG_MAGIC_NOP  /*   "Alter Reality" */,
            BORG_MAGIC_ICK  /*   "Polymorph Self" */,
            BORG_MAGIC_ICK  /*   "Chaos Brinding" */,
            BORG_MAGIC_ICK  /*   "Summon Demon" */},
        {/* Armageddon Tome (sval 3) */
            BORG_MAGIC_AIM  /*   "Gravity Beam" */,
            BORG_MAGIC_AIM  /*   "Meteor Swarm" */,
            BORG_MAGIC_NOP  /*   "Flame Strike" */,
            BORG_MAGIC_NOP  /*   "Call Chaos" */,
            BORG_MAGIC_AIM  /*   "Magic Rocket" */,
            BORG_MAGIC_AIM  /*   "Mana Storm" */,
            BORG_MAGIC_AIM  /*   "Breath Logrus" */,
            BORG_MAGIC_NOP  /*   "Call Void" */}
	}, /* end of Chaos Realm*/

    {/* 5. Death Realm */
        {/* Black Prayers (sval 0) */
            BORG_MAGIC_NOP  /* ! "Detect Unlife" */,
            BORG_MAGIC_AIM  /*   "Maledition" */,
            BORG_MAGIC_NOP  /* ! "Detect Evil" */,
            BORG_MAGIC_AIM  /*   "Stinking Cloud" */,
            BORG_MAGIC_AIM  /*   "Black Sleep" */,
            BORG_MAGIC_NOP  /*   "Resist Poison" */,
            BORG_MAGIC_AIM  /*   "Horrify" */,
            BORG_MAGIC_AIM  /*   "Enslave Undead" */},
        {/* Black Mass (sval 1) */
            BORG_MAGIC_AIM  /* ! "Orb of Entropy" */,
            BORG_MAGIC_AIM  /*   "Nether Bolt" */,
            BORG_MAGIC_NOP  /*   "Terror" */,
            BORG_MAGIC_AIM  /*   "Vamp Drain" */,
            BORG_MAGIC_OBJ  /*   "Poison Brand" */,
            BORG_MAGIC_NOP  /*   "Disp Good" */,
            BORG_MAGIC_WHO  /*   "Genocide" */,
            BORG_MAGIC_NOP  /*   "Restore Life" */},
        {/* Black Channels (sval 2) */
            BORG_MAGIC_NOP  /* ! "Berserk" */,
            BORG_MAGIC_NOP  /*   "Invoke Spirits" */,
            BORG_MAGIC_AIM  /* ! "Dark Bolt" */,
            BORG_MAGIC_NOP  /*   "Battle Frenzy" */,
            BORG_MAGIC_AIM  /*   "Vamp True" */,
            BORG_MAGIC_OBJ  /*   "Vamp Brand" */,
            BORG_MAGIC_AIM  /*   "Dark Storm" */,
            BORG_MAGIC_NOP  /*   "Mass Genocide" */},
        {/* Necronomicon (sval 3) */
            BORG_MAGIC_AIM  /* ! "Death Ray" */,
            BORG_MAGIC_ICK  /*   "Raise the Dead" */,
            BORG_MAGIC_OBJ  /* ! "Esoteria" */,
            BORG_MAGIC_NOP  /*   "Word of Death" */,
            BORG_MAGIC_NOP  /*   "Evocation" */,
            BORG_MAGIC_AIM  /*   "Hellfire" */,
            BORG_MAGIC_NOP  /*   "Omnicide" */,
            BORG_MAGIC_NOP  /*   "Wraithform" */}
	}, /* end of Death Realm */

    {/* 6 Trump Realm */
        {/* Conjuring and Tricks (sval 0) */
            BORG_MAGIC_NOP  /* ! "Phase Door" */,
            BORG_MAGIC_AIM  /*   "Mind Blast" */,
            BORG_MAGIC_ICK  /*   "Shuffle" */,
            BORG_MAGIC_ICK  /*   "Reset Recall" */,
            BORG_MAGIC_NOP  /*   "Teleport Self" */,
            BORG_MAGIC_AIM  /*   "Dimension Door" */,
            BORG_MAGIC_NOP  /*   "Trump Spying" */,
            BORG_MAGIC_AIM  /*   "Teleport Away" */},
        {/* Deck of Many Things (sval 1) */
            BORG_MAGIC_ICK  /* ! "Trump Object" */,
            BORG_MAGIC_ICK  /* ! "Trump Animal" */,
            BORG_MAGIC_NOP  /*   "Phantasmal Servant" */,
            BORG_MAGIC_ICK  /*   "Trump Monster" */,
            BORG_MAGIC_ICK  /*   "Conjure Elemental" */,
            BORG_MAGIC_NOP  /*   "Teleport Level" */,
            BORG_MAGIC_NOP  /*   "Word of Recall" */,
            BORG_MAGIC_NOP  /*   "Banish" */},
        {/* Trumps of Doom (sval 2) */
            BORG_MAGIC_ICK  /* ! "Joker Card" */,
            BORG_MAGIC_ICK  /*   "Trump Spiders" */,
            BORG_MAGIC_ICK  /*   "T. Reptiles" */,
            BORG_MAGIC_ICK  /*   "T. Houdns" */,
            BORG_MAGIC_ICK  /*   "T. Branding" */,
            BORG_MAGIC_ICK  /*   "Living Trump" */,
            BORG_MAGIC_ICK  /*   "Death Dealing" */,
            BORG_MAGIC_ICK  /*   "T. Cyberdemon" */},
        {/* Five Aces (sval 3) */
            BORG_MAGIC_NOP  /* ! "T. Divination" */,
            BORG_MAGIC_OBJ  /*   "T. Lore" */,
            BORG_MAGIC_ICK  /*   "T. Undead" */,
            BORG_MAGIC_ICK  /*   "T. Dragon" */,
            BORG_MAGIC_ICK  /*   "Mass Trump" */,
            BORG_MAGIC_ICK  /*   "T. Demon" */,
            BORG_MAGIC_ICK  /*   "T. Ancient Dragon " */,
            BORG_MAGIC_ICK  /*   "T. Greater Undead" */}
	}, /* end of Trump Realm */

    {/* 7 Arcane Realm */
        {/* Cantrips (sval 0) */
            BORG_MAGIC_AIM  /* ! "Zap" */,
            BORG_MAGIC_AIM  /*   "Wiz Lock" */,
            BORG_MAGIC_NOP  /*   "Det Invis" */,
            BORG_MAGIC_NOP  /*   "Det Mon" */,
            BORG_MAGIC_NOP  /*   "Blink" */,
            BORG_MAGIC_NOP  /*   "Light Area" */,
            BORG_MAGIC_AIM  /*   "Trap/Door Dest" */,
            BORG_MAGIC_NOP  /*   "Cure Light Wounds" */},
        {/* Minor Arcana (sval 1) */
            BORG_MAGIC_NOP  /* ! "Det Door/Trap" */,
            BORG_MAGIC_NOP  /* ! "Phlogiston" */,
            BORG_MAGIC_NOP  /*   "Det Treasure" */,
            BORG_MAGIC_NOP  /*   "Det Enchant" */,
            BORG_MAGIC_NOP  /*   "Det Object" */,
            BORG_MAGIC_NOP  /*   "Cure Poison" */,
            BORG_MAGIC_NOP  /*   "Resist Cold" */,
            BORG_MAGIC_NOP  /*   "Resist Fre" */},
        {/* Major Arcana (sval 2) */
            BORG_MAGIC_NOP  /* ! "Resist Elec" */,
            BORG_MAGIC_NOP  /*   "Resist Acid" */,
            BORG_MAGIC_NOP  /* ! "Cure Med Wounds" */,
            BORG_MAGIC_NOP  /*   "Teleport" */,
            BORG_MAGIC_AIM  /*   "Stone to Mud" */,
            BORG_MAGIC_AIM  /*   "Ray of Light" */,
            BORG_MAGIC_NOP  /*   "Satisfy Hunger" */,
            BORG_MAGIC_NOP  /*   "See Invis" */},
        {/* Manual of Mastery (sval 3) */
            BORG_MAGIC_OBJ  /* ! "Recharge" */,
            BORG_MAGIC_NOP  /*   "Teleport Level" */,
            BORG_MAGIC_OBJ  /* ! "Ident" */,
            BORG_MAGIC_AIM  /*   "Teleport Away" */,
            BORG_MAGIC_AIM  /*   "Elemental Ball" */,
            BORG_MAGIC_NOP  /*   "Detection" */,
            BORG_MAGIC_NOP  /*   "Word of Recall" */,
            BORG_MAGIC_NOP  /*   "Clairvoyance" */}
	} /* end of Arcane Realm */


};



/*
 * Hack -- help analyze the magic
 *
 * The comments yield the "name" of the spell or prayer.
 *
 * Also, the leading letter in the comment indicates how we use the
 * spell or prayer, if at all, using "A" for "attack", "D" for "call
 * light" and "detection", "E" for "escape", "H" for healing, "O" for
 * "object manipulation", "F" for "terrain feature manipulation",
 * "X" for "never use this", and "!" for "soon to be handled".
 *
 * The value indicates how much we want to know the spell/prayer.  A
 * rating of zero indicates that the spell/prayer is useless, and should
 * never be learned or used.  A rating from 1 to 49 indicates that the
 * spell/prayer is worth some experience to use once, so we should study
 * (and use) it when we get bored in town.  A rating from 50 to 99 means
 * that the spell/prayer should be learned as soon as possible (and used
 * when bored).
 *
 * XXX XXX XXX Verify ratings.
 */

static byte borg_magic_rating[8][4][8] =
{
    {/* Null Realm */
        {/* Book0... (sval 0) */
            0  /* ! "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */},
        {/* Book1... (sval 1) */
            0  /*   "(blank)" */,
            0  /* ! "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */},
        {/* Book2... (sval 2) */
            0  /*   "(blank)" */,
            0  /* ! "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */},
        {/* Book3... (sval 3) */
            0  /* ! "(blank)" */,
            0  /*   "(blank)" */,
            0  /* ! "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */,
            0  /*   "(blank)" */}
	}, /* end of Null Realm */

    {/* Life Realm */
        {/* Beginners Handbook (sval 0) */
            85          /*   "Detect Evil" */,
            55          /*   "Cure Light Wounds" */,
            85          /*   "Bless" */,
            35          /*   "Remove Fear" */,
            35          /*   "Call Light" */,
            75          /*   "Find Traps & Doors" */,
            65          /*   "Cure Medium Wounds" */,
            85          /*   "Satisfy Hunger" */},
        {/* Words of Wisdom (sval 1) */
            65           /*   "Remove Curse" */,
            65           /*   "Cure Poison" */,
            85           /*   "Cure Crit Wounds" */,
            55           /*   "See Invis" */,
            95           /*   "Holy Orb" */,
            85           /*   "Prot/Evil" */,
            65           /*   "Heal 300" */,
            55           /*   "Glyph" */},
        {/* Chants and Blessings (sval 2) */
            65          /*   "Exorcism" */,
            65          /*   "Dispel Curse" */,
            55          /*   "Dispel Demon" */,
            0           /*   "Day of Dove" */,
            65          /*   "Dispel Evil" */,
            55          /*   "Banishment" */,
            65          /*   "Holy Word" */,
            55          /*   "Warding True" */},
        {/* Exorcism and Dispelling (sval 3) */
            55          /*   "Heroism" */,
            65          /*   "Prayer" */,
            45          /*   "Bless Weapon" */,
            55          /*   "Restoration" */,
            65          /*   "Healing 2000" */,
            55          /*   "Holy Vision" */,
            55          /*   "Divine Intervent" */,
            55          /*   "Holy Invuln" */}
    }, /* end of Life Magic */

    {/* Sorcery Realm */
        {/* Magic for Beginners (sval 0) */
            95          /*   "Detect Monsters" */,
            85          /*   "Phase Door" */,
            65          /*   "Detect Door" */,
            85          /*   "Light Area" */,
            75          /*   "Confuse Monster" */,
            75          /*   "Teleport Selft" */,
            65          /*   "Sleep Monster" */,
            65          /*   "Recharging" */},
        {/* Conjurings and Tricks (sval 1) */
            55          /*   "Magic Map" */,
            85          /*   "Identify" */,
            55          /*   "Slow Monster" */,
            65          /*   "Mass Sleep" */,
            95          /*   "Teleport Away" */,
            55          /*   "Haste Self" */,
            85          /*   "Detection True" */,
            75          /*   "*Identify*" */},
        {/* Incantations and Illusions (sval 2) */
            55          /*   "Detect Obj/Treasure" */,
            55          /*   "Detect Enchantment" */,
            75          /*   "Charm Monster" */,
            65          /*   "Dimension Door" */,
            65          /*   "Sense Minds" */,
            0           /*   "Self Knowledge" */,
            65          /*   "Teleport Level" */,
            65          /*   "Word of Recall" */},
        {/* Sorcery and Evocations (sval 3) */
            55          /*   "Stasis" */,
            0           /*   "Telekinesis" */,
            0           /*   "Explosive Rune" */,
            65          /*   "Clairvoyance" */,
            55          /*   "Enchant Weap" */,
            55          /*   "Enchant Armour" */,
            0           /*   "Alchemy" */,
            95          /*   "GOI" */}
    }, /* end of Sorcery Realm */

    {/* 3 Nature Realm */
        {/* Call of the Wild (sval 0) */
            65  /*   "Detect Creature" */,
            65  /*   "First Aid" */,
            55  /* ! "Detect Door" */,
            75  /*   "Foraging" */,
            75  /*   "Daylight" */,
            55  /*   "Animal Taming" */,
            75  /*   "Resist Environment" */,
            65  /*   "Cure Wound&Poison" */},
        {/* Nature Mastery (sval 1) */
            55  /* ! "Stone to Mud" */,
            65  /* ! "Lightning Bolt" */,
            65  /*   "Nature Awareness" */,
            65  /*   "Frost Bolt" */,
            65  /*   "Ray of Sunlight" */,
            65  /*   "Entangle" */,
            65  /*   "Summon Animals" */,
            65  /*   "Herbal Healing" */},
        {/* Nature Gifts (sval 2) */
            65  /* ! "Door Building" */,
            45  /*   "Stair Building" */,
            65  /* ! "Stone Skin" */,
            65  /*   "Resistance True" */,
            55  /*   "Animal Friend" */,
            65  /*   "Stone Tell" */,
            45  /*   "Wall of Stone" */,
            45  /*   "Protect From Corros." */},
        {/* Natures Wrath (sval 3) */
            65  /* ! "Earthquake" */,
            65  /*   "Whirlwind" */,
            65  /* ! "Blizzard" */,
            65  /*   "Lightning" */,
            65  /*   "Whirpool" */,
            65  /*   "Call Sunlight" */,
            45  /*   "Elemental Brand" */,
            65  /*   "Natures Wrath" */}
	}, /* end of Natural realm  */

    {/* 4.Chaos Realm */
        {/* Sign of Chaos... (sval 0) */
            95  /* "Magic Missile" */,
            65  /* "Trap/Door Dest" */,
            75  /* "Flash of Light" */,
            55  /* "Touch of Conf" */,
            65  /* "ManaBurst" */,
            65  /* "Fire Bolt" */,
            65  /* "Fist of Force" */,
            75  /* "Teleport" */},
        {/* Chaos Mastery... (sval 1) */
            5   /*   "Wonder" */,
            65  /*   "Chaos Bolt" */,
            65  /*   "Sonic Boom" */,
            65  /*   "Doom Beam" */,
            65  /*   "Fire Ball" */,
            65  /*   "Teleport Other" */,
            65  /*   "Word of Dest" */,
            55  /*   "Invoke Logrus" */},
        {/* Chaos Channels (sval 2) */
            45  /*   "Polymorph Other" */,
            65  /*   "Chain Lightn" */,
            65  /*   "Arcane Binding" */,
            65  /*   "Disintegration" */,
            55  /*   "Alter Reality" */,
            5   /*   "Polymorph Self" */,
            55  /*   "Chaos Binding" */,
            55  /*   "Summon Demon" */},
        {/* Armageddon Tome (sval 3) */
            65  /*   "Gravity Beam" */,
            65  /*   "Meteor Swarm" */,
            65  /*   "Flame Strike" */,
            65  /*   "Call Chaos" */,
            75  /*   "Magic Rocket" */,
            75  /*   "Mana Storm" */,
            65  /*   "Breath Logrus" */,
            65  /*   "Call Void" */}
	}, /* end of Chaos Realm*/

    {/* 5. Death Realm */
        {/* Black Prayers (sval 0) */
            65  /* ! "Detect Unlife" */,
            75  /*   "Maledition" */,
            75  /* ! "Detect Evil" */,
            75  /*   "Stinking Cloud" */,
            65  /*   "Black Sleep" */,
            65  /*   "Resist Poison" */,
            65  /*   "Horrify" */,
            65  /*   "Enslave Undead" */},
        {/* Black Mass (sval 1) */
            70  /* ! "Orb of Entropy" */,
            65  /*   "Nether Bolt" */,
            50  /*   "Terror" */,
            65  /*   "Vamp Drain" */,
            55  /*   "Poison Brand" */,
            65  /*   "Disp Good" */,
            65  /*   "Genocide" */,
            65  /*   "Restore Life" */},
        {/* Black Channels (sval 2) */
            65  /* ! "Berserk" */,
            65  /*   "Invoke Spirits" */,
            65  /* ! "Dark Bolt" */,
            85  /*   "Battle Frenzy" */,
            65  /*   "Vamp True" */,
            65  /*   "Vamp Brand" */,
            65  /*   "Dark Storm" */,
            65  /*   "Mass Genocide" */},
        {/* Necronomicon (sval 3) */
            65  /* ! "Death Ray" */,
            65  /*   "Raise the Dead" */,
            75  /* ! "Esoteria" */,
            65  /*   "Word of Death" */,
            65  /*   "Evocation" */,
            65  /*   "Hellfire" */,
            65  /*   "Omnicide" */,
            55  /*   "Wraithform" */}
	}, /* end of Death Realm */


    {/* Trump Realm */
        {/* Trump Magic (sval 0) */
            95  /* ! "Phase Door" */,
            85  /* ! "Mind Blast " */,
            0   /*   "Shuffle" */,
            0   /*   "Reset Recall" */,
            75  /*   "Tlelport Self" */,
            65  /*   "Dimension Door " */,
            65   /*   "Trump Spying " */,
            70   /*   "Teleport Away " */},
        {/* Deck of Many Things (sval 1) */
            0    /* ! "Trump Object " */,
            0    /* ! "Trump animal " */,
            85   /*   "Phantasmal Servant " */,
            0    /*   "Trump Monster " */,
            0    /*   "Conjure Elemental " */,
            50   /*   "Teleport Level " */,
            65   /*   "Word of recall " */,
            65   /*   "Banishment" */},
        {/* Trump of Doom (sval 2) */
            0  /* ! "Joker Card " */,
            0  /*   "Trump Spiders " */,
            0  /*   "Trump Reptiles " */,
            0  /*   "Trump Hounds " */,
            0  /*   "Trump Branding " */,
            0  /*   "Living Trump " */,
            0  /*   "Death Dealing " */,
            0  /*   "Trump Cyberdemon " */},
        {/* Five Aces (sval 3) */
            0  /* ! "Trump Divination " */,
            0  /*   "Trump Lore " */,
            0  /*   "Trump Undead " */,
            0  /*   "Trump Dragon " */,
            0  /*   "Mass Trump " */,
            0  /*   "Trump Demon " */,
            0  /*   "Trump Ancient Dragon " */,
            0  /*   "Trump Greater Undead " */}
	}, /* end of Trump Realm */

    {/* 7 Arcane Realm */
        {/* Cantrips (sval 0) */
            85  /* ! "Zap" */,
            85  /*   "Wiz Lock" */,
            75  /*   "Det Invis" */,
            75  /*   "Det Mon" */,
            75  /*   "Blink" */,
            75  /*   "Light Area" */,
            85  /*   "Trap/Door Dest" */,
            75  /*   "Cure Light Wounds" */},
        {/* Minor Arcana (sval 1) */
            75  /* ! "Det Door/Trap" */,
            75  /* ! "Phlogiston" */,
            75  /*   "Det Treasure" */,
            75  /*   "Det Enchant" */,
            75  /*   "Det Object" */,
            75  /*   "Cure Poison" */,
            75  /*   "Resist Cold" */,
            75  /*   "Resist Fre" */},
        {/* Major Arcana (sval 2) */
            75  /* ! "Resist Elec" */,
            75  /*   "Resist Acid" */,
            75  /* ! "Cure Med Wounds" */,
            75  /*   "Teleport" */,
            85  /*   "Stone to Mud" */,
            85  /*   "Ray of Light" */,
            75  /*   "Satisfy Hunger" */,
            75  /*   "See Invis" */},
        {/* Manual of Mastery (sval 3) */
            75  /* ! "Recharge" */,
            75  /*   "Teleport Level" */,
            85  /* ! "Ident" */,
            85  /*   "Teleport Away" */,
            70  /*   "Elemental Ball" */,
            75  /*   "Detection" */,
            75  /*   "Word of Recall" */,
            75  /*   "Clairvoyance" */}
	} /* end of Arcane Realm */

};




/*
 * Constant "item description parsers" (singles)
 */
static int borg_single_size;        /* Number of "singles" */
static s16b *borg_single_what;      /* Kind indexes for "singles" */
static cptr *borg_single_text;      /* Textual prefixes for "singles" */

/*
 * Constant "item description parsers" (plurals)
 */
static int borg_plural_size;        /* Number of "plurals" */
static s16b *borg_plural_what;      /* Kind index for "plurals" */
static cptr *borg_plural_text;      /* Textual prefixes for "plurals" */
static cptr *borg_sv_plural_text;   /* Save Textual prefixes for "plurals" (in kind order) */

/*
 * Constant "item description parsers" (suffixes)
 */
static int borg_artego_size;        /* Number of "artegos" */
static s16b *borg_artego_what;      /* Indexes for "artegos" */
static cptr *borg_artego_text;      /* Textual prefixes for "artegos" */
static cptr *borg_sv_art_text;      /* Save textual prefixes for "artifacts" (in kind order) */


/*
 * Return the slot that items of the given type are wielded into
 *
 * Note that "rings" are tough because there are two slots
 *
 * Returns "-1" if the item cannot (or should not) be wielded
 */
int borg_wield_slot(borg_item *item)
{
	if ((item->tval == TV_SWORD) ||
	(item->tval == TV_POLEARM) ||
	(item->tval == TV_HAFTED) ||
	(item->tval == TV_DIGGING) ) return (INVEN_WIELD);

	if ((item->tval == TV_DRAG_ARMOR) ||
	(item->tval == TV_HARD_ARMOR) ||
	(item->tval == TV_SOFT_ARMOR) ) return (INVEN_BODY);

	if (item->tval == TV_SHIELD) return (INVEN_ARM);

	if ((item->tval == TV_CROWN) ||
	(item->tval == TV_HELM) ) return (INVEN_HEAD);

	if (item->tval == TV_BOW) return (INVEN_BOW);

	if (item->tval == TV_RING) return (INVEN_LEFT);

	if (item->tval == TV_AMULET) return (INVEN_NECK);

	if (item->tval == TV_LITE) return (INVEN_LITE);

	if (item->tval == TV_CLOAK) return (INVEN_OUTER);

	if (item->tval == TV_GLOVES) return (INVEN_HANDS);

	if (item->tval == TV_BOOTS) return (INVEN_FEET);

    /* No slot available */
    return (-1);
}

/*
 * Get the *ID information
 *
 * cheats information from the screen if it is not passed
 * a *real* item.  It is only passed in *real* items if the borg is allowed
 * to 'cheat' for inventory.
 * This function returns TRUE if space needs to be pressed
 */
bool borg_object_star_id_aux(borg_item *borg_item, object_type *real_item)
{
    u32b f1, f2, f3;

    /* If a real item pointer is passed in then we are cheating to get */
    /* the data directly from the real item    */
    if (real_item)
    {
        object_flags(real_item, &f1, &f2, &f3);
    }

    borg_item->flags1 = f1;
    borg_item->flags2 = f2;
    borg_item->flags3 = f3;

    borg_item->needs_I_exam = FALSE;




    return (FALSE);
}

/*
 * Look for an item that needs to be analysed because it has been *ID*d
 *
 * This will go through inventory and look for items that were just*ID*'d
 * and examine them for their bonuses.
 */
bool borg_object_star_id( void )
{
    int i;

    /* look in inventory and equiptment for something to *id* */
    for (i = 0; i < INVEN_TOTAL; i++)
    {

        borg_item *item = &borg_items[i];

        if (borg_items[i].needs_I_exam)
        {
            if ((borg_cheat_equip && i >= INVEN_WIELD) ||
                (borg_cheat_inven && i < INVEN_WIELD) )
            {
                /* cheat to get the information. */
                borg_object_star_id_aux( &borg_items[i], &inventory[i]);
            }


            /* inscribe certain objects */
            if (!borg_skill[BI_CDEPTH] &&
                (item->xtra_name) &&
                (item->note == NULL || streq(item->note, " ")
				|| streq(item->note, "") 
				|| strstr(item->desc, "{uncursed")))
            {
                /* make the inscription */
                borg_keypress('{');

                if (i >= INVEN_WIELD)
                {
                borg_keypress('/');
                borg_keypress(I2A(i - INVEN_WIELD));
                }
                else
                {
                borg_keypress(I2A(i));
                }

                if (item->flags1 & TR1_SPEED)
                {
                    borg_keypresses("Spd");
                }
                /* slays and immunities */
                if (item->flags2 & TR2_RES_POIS)
                {
                borg_keypresses("Poisn");
                }
                if (item->flags2 & TR2_IM_FIRE)
                {
                borg_keypresses("IFir");
                }
                if (item->flags2 & TR2_IM_COLD)
                {
                borg_keypresses("ICld");
                }
                if (item->flags2 & TR2_IM_ACID)
                {
                borg_keypresses("IAcd");
                }
                if (item->flags2 & TR2_IM_ELEC)
                {
                borg_keypresses("IElc");
                }
                if (item->flags2 & TR2_RES_LITE)
                {
                    borg_keypresses("Lite");
                }
                if (item->flags2 & TR2_RES_DARK)
                {
                    borg_keypresses("Dark");
                }
                if (item->flags2 & TR2_RES_BLIND)
                {
                    borg_keypresses("Blnd");
                }
                if (item->flags2 & TR2_RES_CONF)
                {
                    borg_keypresses("Conf");
                }
                if (item->flags2 & TR2_RES_SOUND)
                {
                    borg_keypresses("Sound");
                }
                if (item->flags2 & TR2_RES_SHARDS)
                {
                    borg_keypresses("Shrd");
                }
                if (item->flags2 & TR2_RES_NETHER)
                {
                    borg_keypresses("Nthr");
                }
                if (item->flags2 & TR2_RES_NEXUS)
                {
                    borg_keypresses("Nxs");
                }
                if (item->flags2 & TR2_RES_CHAOS)
                {
                    borg_keypresses("Chaos");
                }
                if (item->flags2 & TR2_RES_DISEN)
                {
                    borg_keypresses("Disn");
                }
                if (item->flags3 & TR3_ACTIVATE)
                {
                    borg_keypresses("Actv");
                }
                if (item->flags3 & TR3_TELEPATHY)
                {
                    borg_keypresses("ESP");
                }
                if (item->flags2 & TR2_HOLD_LIFE)
                {
                    borg_keypresses("HL");
                }
                if (item->flags2 & TR2_FREE_ACT)
                {
                    borg_keypresses("FA");
                }
                if (item->flags3 & TR3_SEE_INVIS)
                {
                    borg_keypresses("SInv");
                }

                /* end the inscription */
                borg_keypress('\n');

            }

        }

    }
    return (FALSE);
}



/*
 * Determine the "base price" of a known item (see below)
 *
 * This function is adapted from "object_value_known()".
 *
 * This routine is called only by "borg_item_analyze()", which
 * uses this function to guess at the "value" of an item, if it
 * was to be sold to a store, with perfect "charisma" modifiers.
 */
static s32b borg_object_value_known(borg_item *item)
{
    s32b value;


    object_kind *k_ptr = &k_info[item->kind];

    /* Worthless items */
    if (!k_ptr->cost) return (0L);

    /* Extract the base value */
    value = k_ptr->cost;

#if 0
    /* Hack -- use artifact base costs */
    if (item->name1)
    {
        if (item->name1 != ART_RANDART)
        {
			artifact_type *a_ptr = &a_info[item->name1];

        	/* Worthless artifacts */
        	if (!a_ptr->cost) return (0L);

        	/* Hack -- use the artifact cost */
        	value = a_ptr->cost;
		}
    }

    /* Hack -- add in ego-item bonus cost */
    if (item->name2)
    {
        ego_item_type *e_ptr = &e_info[item->name2];

        /* Worthless ego-items */
        if (!e_ptr->cost) return (0L);

        /* Hack -- reward the ego-item cost */
        value += e_ptr->cost;
    }

#endif /* 0 */

    /* Analyze pval bonus */
    switch (item->tval)
    {
        /* Wands/Staffs */
        case TV_WAND:
        case TV_STAFF:
        {
            /* Pay extra for charges */
            value += ((value / 20) * item->pval);

            break;
        }

        /* Wearable items */
        case TV_SHOT:
        case TV_ARROW:
        case TV_BOLT:
        case TV_BOW:
        case TV_DIGGING:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_HELM:
        case TV_CROWN:
        case TV_SHIELD:
        case TV_CLOAK:
        case TV_SOFT_ARMOR:
        case TV_HARD_ARMOR:
        case TV_DRAG_ARMOR:
        case TV_LITE:
        case TV_AMULET:
        case TV_RING:
        {
            /* Hack -- Negative "pval" is always bad */
            if (item->pval < 0) return (0L);

            /* No pval */
            if (!item->pval) break;

            /* Give credit for stat bonuses */
            if (item->flags1 & TR1_STR) value += (item->pval * 200L);
            if (item->flags1 & TR1_INT) value += (item->pval * 200L);
            if (item->flags1 & TR1_WIS) value += (item->pval * 200L);
            if (item->flags1 & TR1_DEX) value += (item->pval * 200L);
            if (item->flags1 & TR1_CON) value += (item->pval * 200L);
            if (item->flags1 & TR1_CHR) value += (item->pval * 200L);

            /* Give credit for stealth and searching */
            if (item->flags1 & TR1_STEALTH) value += (item->pval * 100L);
            if (item->flags1 & TR1_SEARCH) value += (item->pval * 100L);

            /* Give credit for infra-vision and tunneling */
            if (item->flags1 & TR1_INFRA) value += (item->pval * 50L);
            if (item->flags1 & TR1_TUNNEL) value += (item->pval * 50L);

            /* Give credit for extra attacks */
            if (item->flags1 & TR1_BLOWS) value += (item->pval * 2000L);

            /* Give credit for speed bonus */
            if (item->flags1 & TR1_SPEED) value += (item->pval * 30000L);

            break;
        }
    }


    /* Analyze the item */
    switch (item->tval)
    {
        /* Rings/Amulets */
        case TV_RING:
        case TV_AMULET:
        {
            /* Hack -- negative bonuses are bad */
            if (item->to_a < 0) return (0L);
            if (item->to_h < 0) return (0L);
            if (item->to_d < 0) return (0L);

            /* Give credit for bonuses */
            value += ((item->to_h + item->to_d + item->to_a) * 100L);

            break;
        }

        /* Armor */
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_CLOAK:
        case TV_CROWN:
        case TV_HELM:
        case TV_SHIELD:
        case TV_SOFT_ARMOR:
        case TV_HARD_ARMOR:
        case TV_DRAG_ARMOR:
        {
            /* Hack -- negative armor bonus */
            if (item->to_a < 0) return (0L);

            /* Give credit for bonuses */
            value += ((item->to_h + item->to_d + item->to_a) * 100L);

            break;
        }

        /* Bows/Weapons */
        case TV_BOW:
        case TV_DIGGING:
        case TV_HAFTED:
        case TV_SWORD:
        case TV_POLEARM:
        {
            /* Hack -- negative hit/damage bonuses */
            if (item->to_h + item->to_d < 0) return (0L);

            /* Factor in the bonuses */
            value += ((item->to_h + item->to_d + item->to_a) * 100L);

            /* Hack -- Factor in extra damage dice */
            if ((item->dd > k_ptr->dd) && (item->ds == k_ptr->ds))
            {
                value += (item->dd - k_ptr->dd) * item->ds * 200L;
            }

            break;
        }

        /* Ammo */
        case TV_SHOT:
        case TV_ARROW:
        case TV_BOLT:
        {
            /* Hack -- negative hit/damage bonuses */
            if (item->to_h + item->to_d < 0) return (0L);

            /* Factor in the bonuses */
            value += ((item->to_h + item->to_d) * 5L);

            /* Hack -- Factor in extra damage dice */
            if ((item->dd > k_ptr->dd) && (item->ds == k_ptr->ds))
            {
                value += (item->dd - k_ptr->dd) * item->ds * 5L;
            }

            break;
        }
    }


    /* Return the value */
    return (value);
}


/*
 * Analyze an item given a description and (optional) cost
 *
 * From the description, extract the item identity, and the various
 * bonuses, plus the "aware" and "known" flags (in an encoded state).
 *
 * Note the use of a "prefix binary search" on the arrays of object
 * base names, and on the arrays of artifact/ego-item special names.
 *
 * The Vanilla borg does not cheat here, this borg does cheat.
 */
void borg_item_analyze(borg_item *item, object_type *real_item, cptr desc)
{
	object_kind *k_ptr;

    /* Wipe the item */
    WIPE(item, borg_item);

    /* Save the item description */
    strcpy(item->desc, desc);

    /* Save a pointer to the inscription */
    item->note = quark_str(real_item->inscription);

	/* Need the pseudo-id inscript too */
	/* psuedo-id is checked by strstr(item->desc */

    /* Quantity of item */
    item->iqty = real_item->number;

	/* empty item, leave here */
	if (item->iqty == 0) return;

	/* TVal and SVal */
	item->tval = real_item->tval;

	/* This is only known on some items if not id'd/known */
	item->sval = real_item->sval;

	/* Some Sense but not necessarily real ID
	 * some (Easy Know + Aware) items might sneak in here.
	 */
    item->able = object_known_p(real_item);
	if (!item->able)
	{
		if (object_aware_p(real_item)) item->kind = real_item->k_idx;
	}

	/* Item has been ID'd (store, scroll, spell) */
  	if ((real_item->ident & IDENT_KNOWN) ||
  	    (real_item->ident & IDENT_STOREB) ||
  	    (real_item->ident & IDENT_MENTAL)) item->able = TRUE;

	/* Item has been *ID*'d (store, scroll, spell) */
  	if ((real_item->ident & IDENT_STOREB) ||
  	    (real_item->ident & IDENT_MENTAL)) item->fully_identified = TRUE;

    /* Kind index -- Only if partially ID*/
    if (item->able) item->kind = real_item->k_idx;

	/* power value -- Only if ID'd */
  	if (item->able) item->pval = real_item->pval;

    /* Rods are considered pval 1 if charged */
  	if (item->tval == TV_ROD)
  	{
		k_ptr = &k_info[real_item->k_idx];

		if ((item->iqty > 1) && (real_item->timeout > real_item->pval - k_ptr->pval)) item->pval = 0;
  		else if (item->iqty == 1 && real_item->timeout) item->pval = 0;
  		else item->pval = 1;
	}

	/* Staves and Wands are considered charged unless
	 * they are known to be empty or are {empty}
	 */
	if (item->tval == TV_STAFF || item->tval == TV_WAND)
	{
		/* assume good */
		item->pval =1;

		/* if Known, get correct pval */
		if (item->able) item->pval = real_item->pval;

		/* if seen {empty} assume pval 0 */
		if (real_item->ident & IDENT_EMPTY) item->pval = 0;
	}

  	/* Weight of item */
  	item->weight = real_item->weight;


#if 0
	/* Index known if ID'd */
	if (item->able)
	{
  		/* Artifact Index --Only known if ID'd*/
  		item->name1 = real_item->name1;

  		/* Ego Index --Only known if ID'd*/
  		item->name2 = real_item->name2;

  		/* Artifact Index --Only known if ID'd*/
  		if (real_item->art_name) item->name1 = ART_RANDART;
	}
#endif /* 0 */

	/* Timeout, must wait for recharge */
  	item->timeout = real_item->timeout;

    /* Modifiers -- Only known if ID'd */
    if (item->able)
    {
		item->to_h = real_item->to_h;/* Bonus to hit */
    	item->to_d = real_item->to_d;/* Bonus to dam */
    	item->to_a = real_item->to_a;/* Bonus to ac */
	}
    item->ac = real_item->ac;    /* Armor class */
    item->dd = real_item->dd;    /* Damage dice */
    item->ds = real_item->ds;    /* Damage sides */

    /* Level of item */
    item->level = k_info[item->kind].level;

    /* Extract the base flags -- Kind only given if 'able' */
    item->flags1 = k_info[item->kind].flags1;
    item->flags2 = k_info[item->kind].flags2;
    item->flags3 = k_info[item->kind].flags3;

    /* Base Cost -- Guess */

    /* Known items */
    if (item->able)
    {
        /* Process various fields */
        item->value = borg_object_value_known(item);
    }
    /* Aware items */
    else
    {
        /* Aware items can assume template cost */
        item->value = k_info[item->kind].cost;
    }

	/* No known price on non-aware  item */
    if (!item->kind && !item->value)
    {
        /* Guess at weight and cost */
        switch (item->tval)
        {
			case TV_SKELETON:
				item->value = 0L;
				break;
            case TV_FOOD:
            {
                item->value = 5L;
                break;
            }
            case TV_POTION:
            {
                item->value = 20L;
                break;
            }
            case TV_SCROLL:
            {
                item->value = 20L;
                break;
            }
            case TV_STAFF:
            {
                item->value = 70L;
                break;
            }
            case TV_WAND:
            {
                item->value = 50L;
                break;
            }
            case TV_ROD:
            {
                item->value = 90L;
                break;
            }
            case TV_RING:
            {
                item->value = 45L;
                break;
            }
            case TV_AMULET:
            {
                item->value = 45L;
                break;
            }
        }
	}

    /* Item is cursed */
    item->cursed = cursed_p(real_item);

#if 0
	/* name3 items really screw up the borg */
    if (real_item->name3 && (real_item->ident & (IDENT_KNOWN)))
    {
         artifact_type *a_ptr;

         item->name1 = real_item->name1;
         item->name3 = real_item->name3;
         a_ptr = randart_make(real_item);
         item->value = a_ptr->cost;
         item->able = TRUE;
    }
#endif

#if 0
	/* Hack -- examine artifacts */
    if (item->name1 && item->name1 != ART_RANDART)
    {
        /* XXX XXX Hack -- fix "weird" artifacts */
        if ((item->tval != a_info[item->name1].tval) ||
            (item->sval != a_info[item->name1].sval))
        {
            /* Save the kind */
            item->kind = lookup_kind(item->tval, item->sval);

            /* Save the tval/sval */
            item->tval = k_info[item->kind].tval;
            item->sval = k_info[item->kind].sval;
        }

        /* Extract the weight */
        item->weight = a_info[item->name1].weight;

        /* Extract the artifact flags */
        item->flags1 = a_info[item->name1].flags1;
        item->flags2 = a_info[item->name1].flags2;
        item->flags3 = a_info[item->name1].flags3;
    }
#endif /* 0 */

#if 0
    /* Hack -- examine ego-items */
    if (item->name2)
    {
        /* XXX Extract the weight */

        /* Extract the ego-item flags */
        item->flags1 |= e_info[item->name2].flags1;
        item->flags2 |= e_info[item->name2].flags2;
        item->flags3 |= e_info[item->name2].flags3;
    }
#endif /* 0 */

    /* Special "discount" */
    if (strstr(item->desc, "{on sale")) item->discount = 50;

    /* Standard "discounts" */
    else if (strstr(item->desc, "{25% off")) item->discount = 25;
    else if (strstr(item->desc, "{50% off")) item->discount = 50;
    else if (strstr(item->desc, "{75% off")) item->discount = 75;
    else if (strstr(item->desc, "{90% off")) item->discount = 90;

    /* Cursed indicators */
    else if (strstr(item->desc, "{cursed")) item->value = 0L;
    else if (strstr(item->desc, "{broken")) item->value = 0L;
    else if (strstr(item->desc, "{terrible")) item->value = 0L;
    else if (strstr(item->desc, "{worthless")) item->value = 0L;


    /* Ignore certain feelings */
    /* "{average}" */
    /* "{blessed}" */
    /* "{good}" */
    /* "{excellent}" */
    /* "{special}" */

    /* Ignore special inscriptions */
    /* "{empty}", "{tried}" */

}

/*
 * Send a command to inscribe item number "i" with the inscription "str".
 */
void borg_send_inscribe(int i, cptr str)
{
    cptr s;

    /* Label it */
    borg_keypress(c1);

    /* Choose from inventory */
    if (i < INVEN_WIELD)
    {
        /* Choose the item */
        borg_keypress(I2A(i));
    }

    /* Choose from equipment */
    else
    {
        /* Go to equipment (if necessary) */
        if (borg_items[0].iqty) borg_keypress('/');

        /* Choose the item */
        borg_keypress(I2A(i - INVEN_WIELD));
    }

    /* Send the label */
    for (s = str; *s; s++) borg_keypress(*s);

    /* End the inscription */
    borg_keypress('\n');

}




/*
 * Find the slot of an item with the given tval/sval, if available.
 * Given multiple choices, choose the item with the largest "pval".
 * Given multiple choices, choose the smallest available pile.
 */
int borg_slot(int tval, int sval)
{
    int i, n = -1;

    /* Scan the pack */
    for (i = 0; i < INVEN_PACK; i++)
    {
        borg_item *item = &borg_items[i];

        /* Skip empty items */
        if (!item->iqty) continue;

        /* Skip un-aware items */
        if (!item->kind) continue;

        /* Require correct tval */
        if (item->tval != tval) continue;

        /* Require correct sval */
        if (item->sval != sval) continue;

        /* Prefer largest "pval" */
        if ((n >= 0) && (item->pval < borg_items[n].pval)) continue;

        /* Prefer smallest pile */
        if ((n >= 0) && (item->iqty > borg_items[n].iqty)) continue;

        /* Save this item */
        n = i;
    }

    /* Done */
    return (n);
}



/*
 * Hack -- refuel a torch
 */
bool borg_refuel_torch(void)
{
    int i;

    /* Look for a torch */
    i = borg_slot(TV_LITE, SV_LITE_TORCH);

    /* None available */
    if (i < 0) return (FALSE);

    /* must first wield before one can refuel */
    if (borg_items[INVEN_LITE].sval != SV_LITE_TORCH)
        {
            return (FALSE);
        }

    /* Dont bother with empty */
    if (borg_items[i].pval == 0)
    {
        return (FALSE);
    }

    /* Cant refuel nothing */
    if (borg_items[INVEN_LITE].iqty == 0)
    {
        return (FALSE);
    }

    /* Log the message */
    borg_note(format("# Refueling with %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress('F');
    borg_keypress(I2A(i));

    /* Success */
    return (TRUE);
}


/*
 * Hack -- refuel a lantern
 */
bool borg_refuel_lantern(void)
{
    int i;

    /* Look for a torch */
    i = borg_slot(TV_FLASK, 0);

    /* None available */
    if (i < 0) return (FALSE);

    /* Cant refuel a torch with oil */
    if (borg_items[INVEN_LITE].sval != SV_LITE_LANTERN)
    {
        return (FALSE);
    }

    /* Log the message */
    borg_note(format("# Refueling with %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress('F');
    borg_keypress(I2A(i));

    /* Success */
    return (TRUE);
}




/*
 * Hack -- attempt to eat the given food (by sval)
 */
bool borg_eat_food(int sval)
{
    int i;

    /* Look for that food */
    i = borg_slot(TV_FOOD, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* Log the message */
    borg_note(format("# Eating %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress('E');
    borg_keypress(I2A(i));

    /* Success */
    return (TRUE);
}

/*
 * Quaff a potion of cure critical wounds.  This is a special case
 *   for several reasons.
 *   1) it is usually the only healing potion we have on us
 *   2) we should try to conserve a couple for when we really need them
 *   3) if we are burning through them fast we should probably teleport out of
 *      the fight.
 *   4) When it is the only/best way out of danger, drink away
  */
bool borg_quaff_crit( bool no_check )
{
    static s16b when_last_quaff = 0;

    if (no_check)
    {
        if (borg_quaff_potion(SV_POTION_CURE_CRITICAL))
        {
            when_last_quaff = borg_t;
            return (TRUE);
        }
        return (FALSE);
    }

    /* Save the last two for when we really need them */
    if (borg_skill[BI_ACCW] < 2)
        return FALSE;

    /* Avoid drinking CCW twice in a row */
    if (when_last_quaff > (borg_t-4) &&
        when_last_quaff <= borg_t  &&
        (randint0(100) < 75))
        return FALSE;

    if (borg_quaff_potion(SV_POTION_CURE_CRITICAL))
    {
        when_last_quaff = borg_t;
        return (TRUE);
    }
    return (FALSE);
}


/*
 * Hack -- attempt to quaff the given potion (by sval)
 */
bool borg_quaff_potion(int sval)
{
    int i;

    /* Look for that potion */
    i = borg_slot(TV_POTION, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* Log the message */
    borg_note(format("# Quaffing %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress('q');
    borg_keypress(I2A(i));

    /* Success */
    return (TRUE);
}
/*
 * Hack -- attempt to quaff an unknown potion
 */
bool borg_quaff_unknown(void)
{
    int i, n = -1;

    /* Scan the pack */
    for (i = 0; i < INVEN_PACK; i++)
    {
        borg_item *item = &borg_items[i];

        /* Skip empty items */
        if (!item->iqty) continue;

        /* Require correct tval */
        if (item->tval != TV_POTION) continue;

        /* Skip aware items */
        if (item->kind) continue;

        /* Save this item */
        n = i;
    }


    /* None available */
    if (n < 0) return (FALSE);

    /* Log the message */
    borg_note(format("# Quaffing unknown potion %s.", borg_items[n].desc));

    /* Perform the action */
    borg_keypress('q');
    borg_keypress(I2A(n));

    /* Success */
    return (TRUE);
}

/*
 * Hack -- attempt to read an unknown scroll
 */
bool borg_read_unknown(void)
{
    int i, n = -1;
    borg_grid *ag = &borg_grids[c_y][c_x];

    /* Scan the pack */
    for (i = 0; i < INVEN_PACK; i++)
    {
        borg_item *item = &borg_items[i];

        /* Skip empty items */
        if (!item->iqty) continue;

        /* Require correct tval */
        if (item->tval != TV_SCROLL) continue;

        /* Skip aware items */
        if (item->kind) continue;

        /* Save this item */
        n = i;
    }


    /* None available */
    if (n < 0) return (FALSE);

    /* Not when dark */
    if (!(ag->info & BORG_GLOW) && !borg_skill[BI_CUR_LITE]) return (FALSE);

    /* Blind or Confused */
    if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (FALSE);

    /* Log the message */
    borg_note(format("# Reading unknown scroll %s.", borg_items[n].desc));

    /* Perform the action */
    borg_keypress('r');
    borg_keypress(I2A(n));

    /* Success */
    return (TRUE);
}


/*
 * Hack -- attempt to eat an unknown potion.  This is done in emergencies.
 */
bool borg_eat_unknown(void)
{
    int i, n = -1;

    /* Scan the pack */
    for (i = 0; i < INVEN_PACK; i++)
    {
        borg_item *item = &borg_items[i];

        /* Skip empty items */
        if (!item->iqty) continue;

        /* Require correct tval */
        if (item->tval != TV_FOOD) continue;

        /* Skip aware items */
        if (item->kind) continue;

        /* Save this item */
        n = i;
    }


    /* None available */
    if (n < 0) return (FALSE);

    /* Log the message */
    borg_note(format("# Eating unknown mushroom %s.", borg_items[n].desc));

    /* Perform the action */
    borg_keypress('E');
    borg_keypress(I2A(n));

    /* Success */
    return (TRUE);
}

/*
 * Hack -- attempt to use an unknown staff.  This is done in emergencies.
 */
bool borg_use_unknown(void)
{
    int i, n = -1;

    /* Scan the pack */
    for (i = 0; i < INVEN_PACK; i++)
    {
        borg_item *item = &borg_items[i];

        /* Skip empty items */
        if (!item->iqty) continue;

        /* Require correct tval */
        if (item->tval != TV_STAFF) continue;

        /* Skip aware items */
        if (item->kind) continue;

        /* Save this item */
        n = i;
    }


    /* None available */
    if (n < 0) return (FALSE);

    /* Log the message */
    borg_note(format("# Using unknown Staff %s.", borg_items[n].desc));

    /* Perform the action */
    borg_keypress('u');
    borg_keypress(I2A(n));

    /* Success */
    return (TRUE);
}


/*
 * Hack -- attempt to read the given scroll (by sval)
 */
bool borg_read_scroll(int sval)
{
    int i;
    borg_grid *ag = &borg_grids[c_y][c_x];

    /* Dark */
    if (!(ag->info & BORG_GLOW) && !borg_skill[BI_CUR_LITE]) return (FALSE);

    /* Blind or Confused */
    if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (FALSE);

    /* Look for that scroll */
    i = borg_slot(TV_SCROLL, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* Log the message */
    borg_note(format("# Reading %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress(ESCAPE);
    borg_keypress(ESCAPE);
    borg_keypress('r');
    borg_keypress(I2A(i));

	/* reset recall depth in dungeon? */
	if (sval == SV_SCROLL_WORD_OF_RECALL &&
		borg_skill[BI_CDEPTH] < borg_skill[BI_MAXDEPTH] &&
		borg_skill[BI_CDEPTH] != 0)
    {
		/* Do not reset Depth */
	    borg_keypress('n');
	}

	/* Success */
    return (TRUE);
}

/*
 * Hack -- checks rod (by sval) and
 * make a fail check on it.
 */
bool borg_equips_rod(int sval)
{
    int i, chance, lev;

    /* Look for that staff */
    i = borg_slot(TV_ROD, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* No charges */
    if (!borg_items[i].pval) return (FALSE);

    /* Extract the item level */
    lev = (borg_items[i].level);

    /* Base chance of success */
    chance = borg_skill[BI_DEV];

    /* Confusion hurts skill */
    if (borg_skill[BI_ISCONFUSED]) chance = chance / 2;

    /* High level objects are harder */
    chance = chance - ((lev > 50) ? 50 : lev);

    /* Roll for usage */
    if (chance < USE_DEVICE*2) return (FALSE);

    /* Yep we got one */
    return (TRUE);
}



/*
 * Hack -- attempt to zap the given (charged) rod (by sval)
 */
bool borg_zap_rod(int sval)
{
    int i, lev, chance;

    /* Look for that rod */
    i = borg_slot(TV_ROD, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* Hack -- Still charging */
    if (!borg_items[i].pval) return (FALSE);

    /* Extract the item level */
    lev = (borg_items[i].level);

    /* Base chance of success */
    chance = borg_skill[BI_DEV];

    /* Confusion hurts skill */
    if (borg_skill[BI_ISCONFUSED]) chance = chance / 2;

    /* High level objects are harder */
    chance = chance - ((lev > 50) ? 50 : lev);

    /* Roll for usage */
    if (chance < USE_DEVICE+2) return (FALSE);

    /* Log the message */
    borg_note(format("# Zapping %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress('z');
    borg_keypress(I2A(i));

    /* Success */
    return (TRUE);
}


/*
 * Hack -- attempt to aim the given (charged) wand (by sval)
 */
bool borg_aim_wand(int sval)
{
    int i;

    /* Look for that wand */
    i = borg_slot(TV_WAND, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* No charges */
    if (!borg_items[i].pval) return (FALSE);

    /* record the address to avoid certain bugs with inscriptions&amnesia */
    zap_slot = i;

    /* Log the message */
    borg_note(format("# Aiming %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress('a');
    borg_keypress(I2A(i));

    /* Success */
    return (TRUE);
}


/*
 * Hack -- attempt to use the given (charged) staff (by sval)
 */
bool borg_use_staff(int sval)
{
    int i;

    /* Look for that staff */
    i = borg_slot(TV_STAFF, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* No charges */
    if (!borg_items[i].pval) return (FALSE);

    /* record the address to avoid certain bugs with inscriptions&amnesia */
    zap_slot = i;

    /* Log the message */
    borg_note(format("# Using %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress('u');
    borg_keypress(I2A(i));

    /* Success */
    return (TRUE);
}

/*
 * Hack -- attempt to use the given (charged) staff (by sval) and
 * make a fail check on it.
 */
bool borg_use_staff_fail(int sval)
{
    int i, chance, lev;

    /* Look for that staff */
    i = borg_slot(TV_STAFF, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* No charges */
    if (!borg_items[i].pval) return (FALSE);

    /* Extract the item level */
    lev = (borg_items[i].level);

    /* Base chance of success */
    chance = borg_skill[BI_DEV];

    /* Confusion hurts skill */
    if (borg_skill[BI_ISCONFUSED]) chance = chance / 2;

    /* High level objects are harder */
    chance = chance - ((lev > 50) ? 50 : lev);

    /* Roll for usage, but if its a Teleport be generous. */
    if (chance < USE_DEVICE*2)
    {
        if (sval != SV_STAFF_TELEPORTATION)
        {
            return (FALSE);
        }

        /* We need to give some "desparation attempt to teleport staff" */
        if (!borg_skill[BI_ISCONFUSED] && !borg_skill[BI_ISBLIND]) /* Dark? */
        {
            /* We really have no chance, return false, attempt the scroll */
            if (chance < USE_DEVICE) return (FALSE);
        }
        /* We might have a slight chance, or we cannot not read */
    }


    /* record the address to avoid certain bugs with inscriptions&amnesia */
    zap_slot = i;

    /* Log the message */
    borg_note(format("# Using %s.", borg_items[i].desc));

    /* Perform the action */
    borg_keypress('u');
    borg_keypress(I2A(i));

    /* Success */
    return (TRUE);
}
/*
 * Hack -- checks staff (by sval) and
 * make a fail check on it.
 */
bool borg_equips_staff_fail(int sval)
{
    int i, chance, lev;

    /* Look for that staff */
    i = borg_slot(TV_STAFF, sval);

    /* None available */
    if (i < 0) return (FALSE);

    /* No charges */
    if (!borg_items[i].pval) return (FALSE);

    /* Extract the item level */
    lev = (borg_items[i].level);

    /* Base chance of success */
    chance = borg_skill[BI_DEV];

    /* Confusion hurts skill */
    if (borg_skill[BI_ISCONFUSED]) chance = chance / 2;

    /* High level objects are harder */
    chance = chance - ((lev > 50) ? 50 : lev);

    /* Roll for usage, but if its a Teleport be generous. */
    if (chance < USE_DEVICE*2)
    {
        if (sval != SV_STAFF_TELEPORTATION && sval !=SV_STAFF_DESTRUCTION)
        {
            return (FALSE);
        }

        /* We need to give some "desparation attempt to teleport staff" */
        if (!borg_skill[BI_ISCONFUSED])
        {
            /* We really have no chance, return false, attempt the scroll */
            if (chance < USE_DEVICE) return (FALSE);
        }

        /* We might have a slight chance, continue on */
    }

    /* Yep we got one */
    return (TRUE);
}



/*
 * Hack -- attempt to use the given artifact (by index)
 */
bool borg_activate_artifact(int name1, bool secondary)
{
    int i;

    /* Hack - ignore unused parameter */
	(void) name1;
	(void) secondary;
	
	/* Check the equipment */
    for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
    {
        borg_item *item = &borg_items[i];

        /* Skip incorrect artifacts */
        /* if (item->name1 != name1) continue; */

        /* Check charge */
        if (item->timeout) return (FALSE);

        /*
         * Random Artifact must be *ID* to know the activation power.
         * The borg will cheat with random artifacts to know if the
         * artifact number is activatable, but artifact names and
         * types will be scrambled.  So he must first *ID* the artifact
         * he must play with the artifact to learn its power, just as
         * he plays with magic to gain experience.  But I am not about
         * to undertake that coding.  He needs to *ID* it anyway to learn
         * of the resists that go with the artifact.
         * Lights dont need *id* just regular id.
         */
        if  ((item->tval != TV_LITE) && (!item->fully_identified))
        {
            borg_note(format("# %s must be *ID*'d before activation.", item->desc));
            return (FALSE);
        }


        /* Log the message */
        borg_note(format("# Activating artifact %s.", item->desc));

        /* Perform the action */
        borg_keypress('A');
        borg_keypress(I2A(i - INVEN_WIELD));
#if 0
		/* Jewel aslo gives Recall */
		if (item->name1 == ART_THRAIN)
		{
			if (secondary == FALSE)
			{
	    	    borg_keypress('n');
			}
			else
			{
			    borg_keypress('y');
			}
		}
#endif /* 0 */
        /* Success */
        return (TRUE);
    }

    /* Oops */
    return (FALSE);
}


/*
 * apw Hack -- check and see if borg is wielding an artifact
 */
bool borg_equips_artifact(int name1, int location)
{
	/* Hack - ignore parameters */
	(void) location;
	(void) name1;
	
	/* Hack for [Z] */
	return (FALSE);

#if 0
    int i;
    int lev, chance;
	
	

    /* Check the equipment-- */
    for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
    {
        borg_item *item = &borg_items[i];

        /* Skip incorrect artifacts */
        if (item->name1 != name1) continue;

        /* Check charge.  But not on certain ones  Wor, ID, phase, TELEPORT.*/
        /* this is to ensure that his borg_prep code is working ok */
        if ((name1 != ART_AVAVIR &&
             name1 != ART_ERIRIL &&
             name1 != ART_BELEGENNON &&
             name1 != ART_COLANNON) &&
           (item->timeout) ) continue;
        /*
         * Random Artifact must be *ID* to know the activation power.
         * The borg will cheat with random artifacts to know if the
         * artifact number is activatable, but artifact names and
         * types will be scrambled.  So he must first *ID* the artifact
         * he must play with the artifact to learn its power, just as
         * he plays with magic to gain experience.  But I am not about
         * to undertake that coding.  He needs to *ID* it anyway to learn
         * of the resists that go with the artifact.
         * Lights dont need *id* just regular id.
         */
        if  (/* adult_rand_artifacts */ (item->name1 != ART_GALADRIEL &&
              item->name1 != ART_ELENDIL &&
              item->name1 != ART_THRAIN) &&
             (!item->fully_identified))
        {
            borg_note(format("# %s must be *ID*'d before activation.", item->desc));
            continue;
        }

       /* Extract the item level for fail rate check*/
       lev = item->level;

        /* Base chance of success */
        chance = borg_skill[BI_DEV];

        /* Confusion hurts skill */
        if (borg_skill[BI_ISCONFUSED]) chance = chance / 2;

        /* High level objects are harder */
        chance = chance - ((lev > 50) ? 50 : lev);

        /* Roll for usage.  Return Fail if greater than 50% fail */
        if (chance < (USE_DEVICE*2)) continue;

        /* Success */
        return (TRUE);

    }

    /* I guess I dont have it, or it is not ready */
    return (FALSE);

#endif /* 0 */
}
/*
 * Hack -- check and see if borg is wielding a dragon armor and if
 * he will pass a fail check.
 */
bool borg_equips_dragon(int drag_sval)
{
    int lev, chance;

       /* Check the equipment */
       borg_item *item = &borg_items[INVEN_BODY];

        /* Skip incorrect armours */
        if (item->tval !=TV_DRAG_ARMOR) return (FALSE);
        if (item->sval != drag_sval) return (FALSE);

        /* Check charge */
        if (item->timeout) return (FALSE);

        /* Make Sure Mail is IDed */
        if (!item->able) return (FALSE);

       /* check on fail rate
        * The fail check is automatic for dragon armor.  It is an attack
        * item.  He should not sit around failing 5 or 6 times in a row.
        * he should attempt to activate it, and if he is likely to fail, then
        * eh should look at a different attack option.  We are assuming
        * that the fail rate is about 50%.  So He may still try to activate it
        * and fail.  But he will not even try if he has negative chance or
        * less than twice the USE_DEVICE variable
        */
       /* Extract the item level */
       lev = borg_items[INVEN_BODY].level;

        /* Base chance of success */
        chance = borg_skill[BI_DEV];

        /* Confusion hurts skill */
        if (borg_skill[BI_ISCONFUSED]) chance = chance / 2;

        /* High level objects are harder */
        chance = chance - ((lev > 50) ? 50 : lev);

        /* Roll for usage */
        if (chance < (USE_DEVICE*2)) return (FALSE);

        /* Success */
        return (TRUE);

}

/*
 * apw Hack -- attempt to use the given dragon armour
 */
bool borg_activate_dragon(int drag_sval)
{

    /* Check the equipment */

      borg_item *item = &borg_items[INVEN_BODY];

        /* Skip incorrect mails */
        if (item->tval != TV_DRAG_ARMOR) return (FALSE);
        if (item->sval != drag_sval) return (FALSE);

        /* Check charge */
        if (item->timeout) return (FALSE);

        /* apw Make Sure Mail is IDed */
        if (!item->able) return (FALSE);

        /* Log the message */
        borg_note(format("# Activating dragon scale %s.", item->desc));

        /* Perform the action */
        borg_keypress('A');
        borg_keypress(I2A(INVEN_BODY - INVEN_WIELD));

        /* Success */
        return (TRUE);
}


/*
 * Determine if borg can cast a given spell (when fully rested)
 */
bool borg_spell_legal(int realm, int book, int what)
{
    borg_magic *as = &borg_magics[realm][book][what];

    /* The borg must be able to "cast" spells this realm*/
	if (borg_skill[BI_REALM1] != realm &&
	    borg_skill[BI_REALM2] != realm) return (FALSE);

	/* Make sure we have this realm book */
    if (amt_book[realm][book] <= 0) return (FALSE);

	/* The spell must be "known" */
	if (as->status < BORG_MAGIC_TEST) return (FALSE);

	/* The spell must be affordable (when rested) */
	if (as->power > borg_skill[BI_MAXSP]) return (FALSE);

    /* Success */
    return (TRUE);
}

/*
 * Determine if borg can cast a given spell (right now)
 */
bool borg_spell_okay(int realm, int book, int what)
{
    int reserve_mana = 0;

    borg_magic *as = &borg_magics[realm][book][what];

    borg_grid *ag = &borg_grids[c_y][c_x];

    /* Dark */
    if (!(ag->info & BORG_GLOW) && !borg_skill[BI_CUR_LITE]) return (FALSE);

    /* Define reserve_mana for each class */
    if (borg_skill[BI_REALM1] == REALM_SORCERY) reserve_mana = 6;
    if (borg_skill[BI_REALM1] == REALM_TRUMP) reserve_mana = 6;
    if (borg_skill[BI_REALM1] == REALM_ARCANE) reserve_mana = 15;
    if (borg_skill[BI_REALM1] == REALM_CHAOS) reserve_mana = 15;

    /* Low level spell casters should not worry about this */
    if (borg_skill[BI_CLEVEL] < 35) reserve_mana = 0;

    /* Require ability (when rested) */
    if (!borg_spell_legal(realm, book, what)) return (FALSE);

    /* Hack -- blind/confused */
    if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (FALSE);


    /* The spell must be affordable (now) */
    if (as->power > borg_skill[BI_CURSP]) return (FALSE);

    /* Do not cut into reserve mana (for final teleport) */
    if (borg_skill[BI_CURSP] - as->power < reserve_mana &&
        realm == REALM_SORCERY)
    {
        /* Phase spells ok */
        if (book == 0 && what == 2) return (TRUE);

        /* Teleport spells ok */
        if (book == 1 && what == 5) return (TRUE);

        /* Satisfy Hunger OK */
        if (book == 2 && what == 0) return (TRUE);

        /* others are rejected */
        return (FALSE);
    }

    /* Success */
    return (TRUE);
}

/*
 * fail rate on a spell
 */
static int borg_spell_fail_rate(int realm, int book, int what)
{
    int     chance, minfail;
    borg_magic *as = &borg_magics[realm][book][what];

    /* Access the spell  */
    chance = as->sfail;

    /* Reduce failure rate by "effective" level adjustment */
    chance -= 3 * (borg_skill[BI_CLEVEL] - as->level);

    /* Reduce failure rate by INT/WIS adjustment */
    chance -= 3 * (adj_mag_stat[my_stat_ind[A_INT]] - 1);

    /* Extract the minimum failure rate */
    minfail = adj_mag_fail[my_stat_ind[A_INT]];

    /* Non mage characters never get too good */
    if (borg_class != CLASS_MAGE)
    {
        if (minfail < 5) minfail = 5;
    }

    /* Minimum failure rate */
    if (chance < minfail) chance = minfail;

    /* Stunning makes spells harder */
    if (borg_skill[BI_ISHEAVYSTUN]) chance += 25;
    if (borg_skill[BI_ISSTUN]) chance += 15;

    /* Always a 5 percent chance of working */
    if (chance > 95) chance = 95;

    /* Return the chance */
    return (chance);


}

/*
 * same as borg_spell_okay with a fail % check
 */
bool borg_spell_okay_fail(int realm, int book, int what, int allow_fail )
{
    if (borg_spell_fail_rate(realm, book, what) > allow_fail)
        return FALSE;
    return borg_spell_okay(realm, book, what );
}

/*
 * Same as borg_spell with a fail % check
 */
bool borg_spell_fail(int realm, int book, int what, int allow_fail)
{
    if (borg_spell_fail_rate(realm, book, what) > allow_fail)
        return FALSE;
    return borg_spell(realm, book, what );
}

/*
 * Same as borg_spell_legal with a fail % check
 */
bool borg_spell_legal_fail(int realm, int book, int what, int allow_fail)
{
    if (borg_spell_fail_rate(realm, book, what) > allow_fail)
        return FALSE;
    return borg_spell_legal(realm, book, what );
}

/*
 * Attempt to cast a spell
 */
bool borg_spell(int realm, int book, int what)
{
    int i;

    borg_magic *as = &borg_magics[realm][book][what];

    /* Require ability (right now) */
    if (!borg_spell_okay(realm, book, what)) return (FALSE);

	/* Not if locked down */
	if (borg_skill[BI_CRSNOMAGIC]) return (FALSE);

    /* Look for the book */
    i = borg_book[realm][book];

    /* Paranoia */
    if (i < 0) return (FALSE);

    /* Debugging Info */
    borg_note(format("# Casting %s (%d,%d).", as->name, book, what));

    /* Cast a spell */
    borg_keypress('m');
    borg_keypress(I2A(i));
    borg_keypress(I2A(what));

    /* Because we have no launch message to indicate failure */
    if (realm == 1 && book ==3 && what ==4)
    {
        borg_casted_glyph = TRUE;
    }
    else
    {
        borg_casted_glyph = FALSE;
    }

    /* increment the spell counter */
    as->times ++;

	/* Dimension Door -- Must target the landing zone */
    if ((realm == REALM_SORCERY && book == 2 && what == 3) ||
    	(realm == REALM_TRUMP && book == 0 && what == 5))
    {
	    int x1, y1, x2, y2;

	    /* Report a little bit */
        borg_note(format("# Targetting Landing Zone (%d,%d)", dim_door_y, dim_door_x));

	    /* Determine "path" */
	    x1 = c_x;
	    y1 = c_y;
	    x2 = dim_door_x;
	    y2 = dim_door_y;

	    /* Move to the location (diagonals) */
	    for (; (y1 < y2) && (x1 < x2); y1++, x1++) borg_keypress('3');
	    for (; (y1 < y2) && (x1 > x2); y1++, x1--) borg_keypress('1');
	    for (; (y1 > y2) && (x1 < x2); y1--, x1++) borg_keypress('9');
	    for (; (y1 > y2) && (x1 > x2); y1--, x1--) borg_keypress('7');

	    /* Move to the location */
	    for (; y1 < y2; y1++) borg_keypress('2');
	    for (; y1 > y2; y1--) borg_keypress('8');
	    for (; x1 < x2; x1++) borg_keypress('6');
	    for (; x1 > x2; x1--) borg_keypress('4');

	    /* Select the target */
	    borg_keypress(' ');
	}

    /* Success */
    return (TRUE);
}

/*** Mindcrafter spells are much like realm spells ***/

/*
 * Determine if borg can cast a given Mindcraft spell (when fully rested)
 */
bool borg_mindcr_legal(int spell, int level)
{
    borg_mind *as = &borg_minds[spell];

    /* The borg must be able to "cast" spells this realm*/
	if (borg_class != CLASS_MINDCRAFTER) return (FALSE);

	/* The spell must be "known" */
	if (borg_skill[BI_CLEVEL] < level) return (FALSE);

	/* The spell must be affordable (when rested) */
	if (as->power > borg_skill[BI_MAXSP]) return (FALSE);

    /* Success */
    return (TRUE);
}

/*
 * Determine if borg can cast a given spell (right now)
 */
bool borg_mindcr_okay(int spell, int level)
{
    int reserve_mana = 0;

    borg_mind *as = &borg_minds[spell];

    borg_grid *ag = &borg_grids[c_y][c_x];

    /* Dark */
    if (!(ag->info & BORG_GLOW) && !borg_skill[BI_CUR_LITE]) return (FALSE);

    /* Define reserve_mana for Displacement */
    if (borg_skill[BI_CLEVEL] >= 3) reserve_mana = 2;
    if (borg_skill[BI_CLEVEL] >= 7) reserve_mana = 6;

    /* Low level spell casters should not worry about this */
    if (borg_skill[BI_CLEVEL] < 35) reserve_mana = 0;

    /* Require ability (when rested) */
    if (!borg_mindcr_legal(spell, level)) return (FALSE);

    /* Hack -- blind/confused */
    if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (FALSE);


    /* The spell must be affordable (now) */
    if (as->power > borg_skill[BI_CURSP]) return (FALSE);

    /* Do not cut into reserve mana (for final teleport) */
    if (borg_skill[BI_CURSP] - as->power < reserve_mana)
    {
        /* Minor Displacement spells ok */
        if (spell == 2) return (TRUE);

        /* Major Displacement ok */
        if (spell == 3) return (TRUE);

        /* others are rejected */
        return (FALSE);
    }
    /* Success */
    return (TRUE);
}

/*
 * fail rate on a mindcrafter spell
 */
static int borg_mindcr_fail_rate(int spell, int level)
{
    int     chance, minfail;
	borg_mind	*as = &borg_minds[spell];

    /* Hack - ignore parameter */
	(void) level;
	
	/* Access the spell  */
    chance = as->sfail;

    /* Reduce failure rate by "effective" level adjustment */
    chance -= 3 * (borg_skill[BI_CLEVEL] - as->level);

    /* Reduce failure rate by INT/WIS adjustment */
    chance -= 3 * (adj_mag_stat[my_stat_ind[A_WIS]] - 1);

    /* Extract the minimum failure rate */
    minfail = adj_mag_fail[my_stat_ind[A_WIS]];


    /* Minimum failure rate */
    if (chance < minfail) chance = minfail;

    /* Stunning makes spells harder */
    if (borg_skill[BI_ISHEAVYSTUN]) chance += 25;
    if (borg_skill[BI_ISSTUN]) chance += 15;

    /* Always a 5 percent chance of working */
    if (chance > 95) chance = 95;

    /* Return the chance */
    return (chance);
}

/*
 * same as borg_mind_okay with a fail % check
 */
bool borg_mindcr_okay_fail(int spell, int level, int allow_fail )
{
    if (borg_mindcr_fail_rate(spell, level) > allow_fail)
        return FALSE;
    return borg_mindcr_okay(spell, level );
}

/*
 * Same as borg_mind with a fail % check
 */
bool borg_mindcr_fail(int spell, int level, int allow_fail)
{
    if (borg_mindcr_fail_rate(spell, level) > allow_fail)
        return FALSE;
    return borg_mindcr(spell, level);
}

/*
 * Same as borg_mind_legal with a fail % check
 */
bool borg_mindcr_legal_fail(int spell, int level, int allow_fail)
{
    if (borg_mindcr_fail_rate(spell, level) > allow_fail)
        return FALSE;
    return borg_mindcr_legal(spell, level );
}

/*
 * Attempt to cast a mindcrafter spell
 */
bool borg_mindcr(int spell, int level)
{
	borg_mind	*as = &borg_minds[spell];

    /* Require ability (right now) */
    if (!borg_mindcr_okay(spell, level)) return (FALSE);

	/* Not if locked down */
	if (borg_skill[BI_CRSNOMAGIC]) return (FALSE);

    /* Debugging Info */
    borg_note(format("# Casting %s (spell: %d, level: %d).", as->name, spell, level));

    /* Cast a spell */
    borg_keypress('m');
	borg_keypress(as->letter);

    /* increment the spell counter */
    as->times ++;

	/* Dimension Door -- need a landing Zone */
	if (spell == MIND_MINOR_DISP && level >= 40)
	{
	    int x1, y1, x2, y2;

	    /* Report a little bit */
        borg_note(format("# Targetting Landing Zone (%d,%d)", dim_door_y, dim_door_x));

	    /* Determine "path" */
	    x1 = c_x;
	    y1 = c_y;
	    x2 = dim_door_x;
	    y2 = dim_door_y;

	    /* Move to the location (diagonals) */
	    for (; (y1 < y2) && (x1 < x2); y1++, x1++) borg_keypress('3');
	    for (; (y1 < y2) && (x1 > x2); y1++, x1--) borg_keypress('1');
	    for (; (y1 > y2) && (x1 < x2); y1--, x1++) borg_keypress('9');
	    for (; (y1 > y2) && (x1 > x2); y1--, x1--) borg_keypress('7');

	    /* Move to the location */
	    for (; y1 < y2; y1++) borg_keypress('2');
	    for (; y1 > y2; y1--) borg_keypress('8');
	    for (; x1 < x2; x1++) borg_keypress('6');
	    for (; x1 > x2; x1--) borg_keypress('4');

	    /* Select the target */
	    borg_keypress(' ');
	}

    /* Success */
    return (TRUE);
}


/*** Racial abilities are much like magic spells ***/

/*
 * Determine if borg can cast a given Racial spell
 * (when fully rested).
 * -or-
 * with a reasonable degree of difficulty with Check_fail
 */
bool borg_racial_check(int race, bool check_fail)
{

	int i;
	int val;
	int sum = 0;

    int lev_req = 99;
    int cost = 0;
    int stat = 0;
    int diff = 0;
	int use_stat = 0;
	int difficulty = 0;

	bool use_hp = FALSE;

    /* The borg must be able to "cast" spells this race */
	if (borg_race != race) return (FALSE);

	/* The spell must be "known" */
	switch (borg_race)
	{
 		case RACE_HUMAN:
 		case RACE_HALF_ELF:
 		case RACE_ELF:
 			lev_req = 99;
 			break;
 		case RACE_HOBBIT:
 			lev_req = 15;
			cost = 10;
			use_stat = A_INT;
			diff = 10;
 			break;
		case RACE_GNOME:
 			lev_req = 5;
			cost = 5 + (borg_skill[BI_CLEVEL] / 5);
			use_stat = A_INT;
			diff = 12;
 			break;
		case RACE_DWARF:
 			lev_req = 5;
			cost = 5;
			use_stat = A_WIS;
			diff = 12;
 			break;
		case RACE_HALF_ORC:
 			lev_req = 3;
			cost = 5;
			use_stat = A_WIS;
			diff = ((borg_class == CLASS_WARRIOR) ? 5: 10);
 			break;
		case RACE_HALF_TROLL:
 			lev_req = 10;
			cost = 12;
			use_stat = A_WIS;
			diff = ((borg_class == CLASS_WARRIOR) ? 6: 12);
 			break;
		case RACE_AMBERITE: /* not coded yet */
 			lev_req = 99;
			cost = 5;
			use_stat = A_WIS;
			diff = 50;
 			break;
		case RACE_HIGH_ELF:
  			lev_req = 99;
			cost = 0;
			use_stat = A_WIS;
			diff = 0;
 			break;
		case RACE_BARBARIAN:
 			lev_req = 8;
			cost = 10;
			use_stat = A_WIS;
			diff = ((borg_class == CLASS_WARRIOR) ? 6: 12);
 			break;
		case RACE_HALF_OGRE:
  			lev_req = 25;
			cost = 35;
			use_stat = A_STR;
			diff = 12;
 			break;
		case RACE_HALF_GIANT:
  			lev_req =99; /* no support */
			cost = 35;
			use_stat = A_INT;
			diff = 15;
 			break;
		case RACE_HALF_TITAN:
  			lev_req = 99; /* no support */
			cost = 20;
			use_stat = A_INT;
			diff = 12;
 			break;
		case RACE_CYCLOPS:
 			lev_req = 20;
			cost = 15;
			use_stat = A_STR;
			diff = 12;
 			break;
		case RACE_YEEK:
 			lev_req = 15;
			cost = 15;
			use_stat = A_WIS;
			diff = 10;
 			break;
		case RACE_KLACKON:
 			lev_req = 9;
			cost = 9;
			use_stat = A_DEX;
			diff = 14;
 			break;
		case RACE_KOBOLD:
 			lev_req = 12;
			cost = 8;
			use_stat = A_DEX;
			diff = 14;
 			break;
 		case RACE_NIBELUNG:
 			lev_req = 10;
			cost = 5;
			use_stat = A_WIS;
			diff = 10;
 			break;
		case RACE_DARK_ELF:
 			lev_req = 2;
			cost = 2;
			use_stat = A_INT;
			diff = 9;
 			break;
 		case RACE_DRACONIAN:
  			lev_req = 1;
			cost = borg_skill[BI_CLEVEL];
			use_stat = A_CON;
			diff = 12;
 			break;
		case RACE_MIND_FLAYER:
 			lev_req = 15;
			cost = 12;
			use_stat = A_STR;
			diff = 15;
 			break;
 		case RACE_IMP:
  			lev_req = 9;
			cost = 15;
			use_stat = A_WIS;
			diff = 15;
 			break;
		case RACE_GOLEM:
 			lev_req = 20;
			cost = 15;
			use_stat = A_CON;
			diff = 8;
 			break;
		case RACE_SKELETON:
 		case RACE_ZOMBIE:
 			lev_req = 30;
			cost = 30;
			use_stat = A_WIS;
			diff = 18;
 			break;
		case RACE_VAMPIRE:
 			lev_req = 2;
			cost = 1 + (borg_skill[BI_CLEVEL] /3);
			use_stat = A_WIS;
			diff = 18;
 			break;
		case RACE_SPECTRE:
  			lev_req = 4;
			cost = 6;
			use_stat = A_INT;
			diff = 3;
 			break;
		case RACE_SPRITE:
 			lev_req = 12;
			cost = 12;
			use_stat = A_INT;
			diff = 15;
 			break;
		case RACE_BEASTMAN:
 			lev_req = 99;  /* No ability */
			cost = 30;
			use_stat = A_WIS;
			diff = 18;
 			break;
	}

	/* Power is not available yet */
	if (borg_skill[BI_CLEVEL] < lev_req) return (FALSE);

	/* Not enough mana - use hp */
	if (borg_skill[BI_MAXSP] < cost) use_hp = TRUE;

	/* Too confused */
	if (borg_skill[BI_ISCONFUSED]) return FALSE;

	/* Cost -- dont go into debt */
	if (use_hp && (cost > borg_skill[BI_CURHP] * 7/10)) return (FALSE);

	/* Legal check ends here */
	if (!check_fail) return (TRUE);

	/* Otherwise continue on to a fail check */

	/* Reasonable chance of success */
	stat = my_stat_cur[use_stat];

	/* Stun makes it more difficult */
	if (borg_skill[BI_ISSTUN])
	{
		difficulty += 10;
	}
	else
	{
		int lev_adj = ((borg_skill[BI_CLEVEL] - lev_req) / 3);
		if (lev_adj > 10) lev_adj = 10;
		difficulty -= lev_adj;
	}

	if (difficulty < 5) difficulty = 5;

	/* We only need halfs of the difficulty */
	difficulty = difficulty / 2;

	for (i = 1; i <= stat; i++)
	{
		val = i - difficulty;
		if (val > 0)
			sum += (val <= difficulty) ? val : difficulty;
	}

	if (difficulty >= 40)
		return (FALSE);
	else
	    /* Success */
	    return (TRUE);
}


/*
 * Attempt to cast a racial spell
 */
bool borg_racial(int race)
{

    /* Require ability (right now) */
    if (!borg_racial_check(race, TRUE)) return (FALSE);

	/* Cost -- dont go into debt */
	if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] * 5/10 &&
		borg_race != RACE_GNOME) return (FALSE);

	/* Gnomes can go into emergency zone (mostly) */
	if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] * 3/10 &&
		borg_race == RACE_GNOME) return (FALSE);

    /* Debugging Info */
    borg_note("# Racial Power.");

    /* Cast a spell */
    borg_keypress('U');
	borg_keypress('a');

    /* Success */
    return (TRUE);
}


/*
 * Inscribe food and Slime Molds
 */
extern bool borg_inscribe_food(void)
{
    int ii;
    char name[80];

    for (ii=0; ii < INVEN_TOTAL; ii++)
    {
        borg_item *item = &borg_items[ii];

        /* Skip empty items */
        if (!item->iqty) continue;

        /* Require correct tval */
        if (item->tval != TV_FOOD) continue;

        /* skip things already inscribed */
        if (item->note != NULL && (!(streq(item->note, "")) &&
            !(streq(item->note, " ")))) continue;

        /* inscribe foods and molds */
        if (item->sval == SV_FOOD_SLIME_MOLD || item->sval == SV_FOOD_RATION)
        {

            if (item->sval == SV_FOOD_RATION)
            {
                /* get a name */
                strcpy(name, food_syllable1[randint0(sizeof(food_syllable1) / sizeof(char*))]);
                strcat(name, food_syllable2[randint0(sizeof(food_syllable2) / sizeof(char*))]);

                borg_send_inscribe(ii, name);
                return (TRUE);
            }

            if (item->sval == SV_FOOD_SLIME_MOLD)
            {
                /* get a name */
                strcpy(name, mold_syllable1[randint0(sizeof(mold_syllable1) / sizeof(char*))]);
                strcat(name, mold_syllable2[randint0(sizeof(mold_syllable2) / sizeof(char*))]);
                strcat(name, mold_syllable3[randint0(sizeof(mold_syllable3) / sizeof(char*))]);

                borg_send_inscribe(ii, name);
                return (TRUE);
            }

        }
    }

    /* all done */
    return (FALSE);
}
/*
 * Send a command to de-inscribe item number "i" .
 */
void borg_send_deinscribe(int i)
{

    /* Ok to inscribe Slime Molds */
    if (borg_items[i].tval == TV_FOOD &&
        borg_items[i].sval == SV_FOOD_SLIME_MOLD) return;

    /* Label it */
    borg_keypress('}');

    /* Choose from inventory */
    if (i < INVEN_WIELD)
    {
        /* Choose the item */
        borg_keypress(I2A(i));
    }

    /* Choose from equipment */
    else
    {
        /* Go to equipment (if necessary) */
        if (borg_items[0].iqty) borg_keypress('/');

        /* Choose the item */
        borg_keypress(I2A(i - INVEN_WIELD));
    }

}

/*
 * Cheat the "equip" screen
 */
void borg_cheat_equip(void)
{
    int i;

    char buf[256];

    /* Extract the equipment */
    for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
    {
        /* Default to "nothing" */
        buf[0] = '\0';

        /* Describe a real item */
        if (inventory[i].k_idx)
        {
            /* Describe it */
            object_desc(buf, &inventory[i], TRUE, 3, 256);
        }

#if 0
        /* Ignore "unchanged" items */
        /* !FIX do I still need this?  I forget...  AJG */
        if (borg_items[i].needs_I_exam)
        {
           if (streq(buf, borg_items[i].desc)) continue;
        }
#endif
        /* Analyze the item (no price) */
        borg_item_analyze(&borg_items[i], &inventory[i], buf);

        /* get the fully id stuff */
        if ((inventory[i].ident & IDENT_MENTAL) ||
        	(inventory[i].ident & IDENT_STOREB))
        {
            borg_items[i].fully_identified = TRUE;
            borg_items[i].needs_I_exam = TRUE;
            borg_do_star_id = TRUE;
        }

    }
}


/*
 * Cheat the "inven" screen
 */
void borg_cheat_inven(void)
{
    int i;

    char buf[256];

    /* Extract the current weight */
    borg_cur_wgt = p_ptr->total_weight;

    /* Extract the inventory */
    for (i = 0; i < INVEN_PACK; i++)
    {
        /* Default to "nothing" */
        buf[0] = '\0';

        /* Describe a real item */
        if (inventory[i].k_idx)
        {
            /* Describe it */
            object_desc(buf, &inventory[i], TRUE, 3, 256);
        }

        /* Ignore "unchanged" items */
        if (streq(buf, borg_items[i].desc)) continue;

        /* inventory changed so goals must change. */
        goal_shop = goal_ware = goal_item = -1;

        /* Analyze the item (no price) */
        borg_item_analyze(&borg_items[i], &inventory[i], buf);

        /* get the fully id stuff */
        if ((inventory[i].ident & IDENT_MENTAL) ||
        	(inventory[i].ident & IDENT_STOREB))
        {
            borg_items[i].fully_identified = TRUE;
            borg_items[i].needs_I_exam = TRUE;
            borg_do_star_id = TRUE;
        }

        /* Note changed inventory */
        borg_do_crush_junk = TRUE;
        borg_do_crush_hole = TRUE;
        borg_do_crush_slow = TRUE;
    }
}


/*
 * Hack -- Cheat the "spell" info
 *
 * Hack -- note the use of the "cheat" field for efficiency
 */
void borg_cheat_spell(int realm)
{
    int j, what;
	int book;

    /* Can we use spells/prayers? */
    if (realm == 0) return;


	/* process books */
    for (book = 0; book < 4; book++)
    {
	    /* Process the spells */
	    for (what = 0; what < 8; what++)
	    {
	        /* Access the spell */
	        borg_magic *as = &borg_magics[realm][book][what];

	        /* Skip illegible spells */
	        if (as->status == BORG_MAGIC_ICKY) continue;

	        /* Access the index */
	        j = as->cheat;

	        /* Note "forgotten" spells */
	        if ((realm == borg_skill[BI_REALM1]) ?
	            ((p_ptr->spell_forgotten1 & (1L << j))) :
	            ((p_ptr->spell_forgotten2 & (1L << j))))
	        {
	            /* Forgotten */
	            as->status = BORG_MAGIC_LOST;
	        }

	        /* Note "difficult" spells */
	        else if (borg_skill[BI_CLEVEL] < as->level)
	        {
	            /* Unknown */
	            as->status = BORG_MAGIC_HIGH;
	        }

	        /* Note "unknown" spells */
	        else if (!((realm == borg_skill[BI_REALM1]) ?
	                   (p_ptr->spell_learned1 & (1L << j)) :
	                   (p_ptr->spell_learned2 & (1L << j))))
	        {
	            /* Unknown */
	            as->status = BORG_MAGIC_OKAY;
	        }

	        /* Note "untried" spells */
	        else if (!((realm == borg_skill[BI_REALM1]) ?
	                   (p_ptr->spell_learned1 & (1L << j)) :
	                   (p_ptr->spell_learned2 & (1L << j))))
	        {
	            /* Untried */
	            as->status = BORG_MAGIC_TEST;
	        }

	        /* Note "known" spells */
	        else
	        {
	            /* Known */
	            as->status = BORG_MAGIC_KNOW;
	        }
	    } /* book */
	} /* Realm */
}


/*
 * Prepare a book
 */
static void prepare_book_info(int realm, int book)
{
    int i, what;

    int spell[64], num = 0;

    /* Reset each spell entry */
    for (what = 0; what < 8; what++)
    {
        borg_magic *as = &borg_magics[realm][book][what];

        /* Assume no name */
        as->name = NULL;

		/* Know the Realm, if any */
		as->realm = realm;

        /* Assume illegible */
        as->status = BORG_MAGIC_ICKY;

        /* Assume illegible */
        as->method = BORG_MAGIC_ICK;

        /* Impossible values */
        as->level = 99;
        as->power = 99;

        /* Impossible value */
        as->cheat = 99;

        /* Delete the text name */
        as->realm_name = NULL;
    }


    /* Can we use spells/prayers? */
    if (borg_skill[BI_REALM1] == -1) return;


	/* Extract spells */
    for (i = 0; i < 32; i++)
    {
        /* Check for this spell */
        if ((fake_spell_flags[book] & (1L << i)))
        {
            /* Collect this spell */
            spell[num++] = i;
        }
    }

    /* Process each existing spell */
    for (what = 0; what < num; what++)
    {
        borg_magic *as = &borg_magics[realm][book][what];

        magic_type *s_ptr = &mb_ptr->info[realm][spell[what]];

        /* Skip "illegible" spells */
        if (s_ptr->slevel == 99) continue;

        /* Save the spell name */
        as->name = spell_names[realm][spell[what]];

		/* Realm Name */
		if (realm == 1) as->realm_name = "Life";
		if (realm == 2) as->realm_name = "Sorcery";
		if (realm == 3) as->realm_name = "Nature";
		if (realm == 4) as->realm_name = "Chaos";
		if (realm == 5) as->realm_name = "Death";
		if (realm == 6) as->realm_name = "Trump";
		if (realm == 7) as->realm_name = "Arcane";

		/* Save the Realm, if any */
		as->realm = realm;

        /* Save the spell index */
        as->cheat = spell[what];

        /* Hack -- assume excessive level */
        as->status = BORG_MAGIC_HIGH;

        /* Access the correct "method" */
        as->method = borg_magic_method[realm][book][what];

        /* Access the correct "rating" */
        as->rating = borg_magic_rating[realm][book][what];

        /* Extract the level and power */
        as->level = s_ptr->slevel;
        as->power = s_ptr->smana;

        /* extract fail rate. */
        as->sfail = s_ptr->sfail;
    }
}
/*
 * Prepare a Mindcrafter Array
 */
static void prepare_mind_info(void)
{
    int spell;

    /* Reset each spell entry */
    for (spell = 0; spell < MINDCRAFT_MAX; spell++)
    {
        borg_mind *as = &borg_minds[spell];
        mindcraft_power *s_ptr = &mindcraft_powers[spell];

        /* name */
        as->name = s_ptr->name;

        /* values */
        as->level = s_ptr->min_lev;
        as->power = s_ptr->mana_cost;

		/* Fail Rate */
		as->sfail = s_ptr->fail;

        /* Delete the text letter address */
        as->letter = 'a' + spell;

    }
}


/*
 * Hack -- prepare some stuff based on the player race and class
 */
void prepare_race_class_info(void)
{
    int book;

	/* Hack -- Realms */
	borg_skill[BI_REALM1] = p_ptr->realm1;
	borg_skill[BI_REALM2] = p_ptr->realm2;

 	/* Initialize the various spell arrays by book */
	for (book = 0; book < 4; book++)
	{
		prepare_book_info(borg_skill[BI_REALM1], book);
	}
	
	for (book = 0; book < 4; book++)
	{
		prepare_book_info(borg_skill[BI_REALM2], book);
	}
	
	/* MindCrafters */
	if (borg_class == CLASS_MINDCRAFTER)
	{
		prepare_mind_info();
	}
}



/*
 * Sorting hook -- comp function -- see below
 *
 * We use "u" to point to an array of strings, and "v" to point to
 * an array of indexes, and we sort them together by the strings.
 */
static bool ang_sort_comp_hook_string(vptr u, vptr v, int a, int b)
{
    cptr *text = (cptr*)(u);
    s16b *what = (s16b*)(v);

    int cmp;

    /* Compare the two strings */
    cmp = (strcmp(text[a], text[b]));

    /* Strictly less */
    if (cmp < 0) return (TRUE);

    /* Strictly more */
    if (cmp > 0) return (FALSE);

    /* Enforce "stable" sort */
    return (what[a] <= what[b]);
}


/*
 * Sorting hook -- swap function -- see below
 *
 * We use "u" to point to an array of strings, and "v" to point to
 * an array of indexes, and we sort them together by the strings.
 */
static void ang_sort_swap_hook_string(vptr u, vptr v, int a, int b)
{
    cptr *text = (cptr*)(u);
    s16b *what = (s16b*)(v);

    cptr texttmp;
    s16b whattmp;

    /* Swap "text" */
    texttmp = text[a];
    text[a] = text[b];
    text[b] = texttmp;

    /* Swap "what" */
    whattmp = what[a];
    what[a] = what[b];
    what[b] = whattmp;
}


void borg_clear_3(void)
{
    KILL(borg_items);
    KILL(borg_shops);
    KILL(safe_items);
    KILL(safe_home);
    KILL(safe_shops);
    KILL(borg_plural_text);
    KILL(borg_sv_plural_text);
    KILL(borg_plural_what);
    KILL(borg_single_text);
    KILL(borg_single_what);
    KILL(borg_artego_text);
    KILL(borg_sv_art_text);
    KILL(borg_artego_what);
}

/*
 * Initialize this file
 *
 * Note that all six artifact "Rings" will parse as "kind 506"
 * (the first artifact ring) and both artifact "Amulets" will
 * parse as "kind 503" (the first of the two artifact amulets),
 * but as long as we use the "name1" field (and not the "kind"
 * or "sval" fields) we should be okay.
 *
 * We sort the two arrays of items names in reverse order, so that
 * we will catch "mace of disruption" before "mace", "Scythe of
 * Slicing" before "Scythe", and for "Ring of XXX" before "Ring".
 *
 * Note that we do not have to parse "plural artifacts" (!)
 *
 * Hack -- This entire routine is a giant hack, but it works
 */
void borg_init_3(void)
{
    int i, k, n;

    int size = 0;

    s16b *what;
    cptr *text;

    char buf[256];

	/* Create buffers */
	C_MAKE(what, z_info->k_max + z_info->a_max, s16b);
	C_MAKE(text, z_info->k_max + z_info->a_max, cptr);


    /*** Item/Ware arrays ***/

    /* Make the inventory array */
    C_MAKE(borg_items, INVEN_TOTAL, borg_item);

    /* Make the stores in the town */
    C_MAKE(borg_shops, MAX_STORES, borg_shop);


    /*** Item/Ware arrays (simulation) ***/

    /* Make the "safe" inventory array */
    C_MAKE(safe_items, INVEN_TOTAL, borg_item);
    C_MAKE(safe_home,  STORE_INVEN_MAX, borg_item);

    /* Make the "safe" stores in the town */
    C_MAKE(safe_shops, MAX_STORES, borg_shop);

    /*** Plural Object Templates ***/

    /* Start with no objects */
    size = 0;

    /* Analyze some "item kinds" */
    for (k = 1; k < z_info->k_max; k++)
    {
        object_type hack;

        /* Get the kind */
        object_kind *k_ptr = &k_info[k];

        /* Skip "empty" items */
        if (!k_ptr->name) continue;

        /* Skip "gold" objects */
        if (k_ptr->tval == TV_GOLD) continue;

        /* Skip "artifacts" */
        if (k_ptr->flags3 & TR3_INSTA_ART) continue;

        /* Hack -- make an item */
        object_prep(&hack, k);

        /* Describe a "plural" object */
        hack.number = 2;
        object_desc_store(buf, &hack, FALSE, 0, 256);

        /* Save an entry */
        text[size] = string_make(buf);
        what[size] = k;
        size++;
    }

    /* Set the sort hooks */
    ang_sort_comp = ang_sort_comp_hook_string;
    ang_sort_swap = ang_sort_swap_hook_string;

    C_MAKE(borg_sv_plural_text, z_info->k_max, cptr);
    for (i = 0; i < size; i++)
    {
		borg_sv_plural_text[what[i]] = text[i];
	}
    /* Sort */
    ang_sort(text, what, size);

    /* Save the size */
    borg_plural_size = size;

    /* Allocate the "item parsing arrays" (plurals) */
    C_MAKE(borg_plural_text, borg_plural_size, cptr);
    C_MAKE(borg_plural_what, borg_plural_size, s16b);

    /* Save the entries */
    for (i = 0; i < size; i++) borg_plural_text[i] = text[i];
    for (i = 0; i < size; i++) borg_plural_what[i] = what[i];


    /*** Singular Object Templates ***/

    /* Start with no objects */
    size = 0;

    /* Analyze some "item kinds" */
    for (k = 1; k < z_info->k_max; k++)
    {
        object_type hack;

        /* Get the kind */
        object_kind *k_ptr = &k_info[k];

        /* Skip "empty" items */
        if (!k_ptr->name) continue;

        /* Skip "dungeon terrain" objects */
        if (k_ptr->tval == TV_GOLD) continue;

        /* Skip "artifacts" */
        if (k_ptr->flags3 & TR3_INSTA_ART) continue;

        /* Hack -- make an item */
        object_prep(&hack, k);

        /* Describe a "singular" object */
        hack.number = 1;
        object_desc_store(buf, &hack, FALSE, 0, 256);

        /* Save an entry */
        text[size] = string_make(buf);
        what[size] = k;
        size++;
    }

    /* Analyze the "INSTA_ART" items */
    for (i = 1; i < z_info->a_max; i++)
    {
        object_type hack;

        artifact_type *a_ptr = &a_info[i];

        cptr name = (a_name + a_ptr->name);

        /* Skip "empty" items */
        if (!a_ptr->name) continue;

        /* Skip non INSTA_ART things */
        if (!(a_ptr->flags3 & TR3_INSTA_ART)) continue;

        /* Extract the "kind" */
        k = lookup_kind(a_ptr->tval, a_ptr->sval);

        /* Hack -- make an item */
        object_prep(&hack, k);

        /* Save the index */
/*         hack.name1 = i; */

        /* Describe a "singular" object */
        hack.number = 1;
        object_desc_store(buf, &hack, FALSE, 0, 256);

        /* Extract the "suffix" length */
        n = strlen(name) + 1;

        /* Remove the "suffix" */
        buf[strlen(buf) - n] = '\0';

        /* Save an entry */
        text[size] = string_make(buf);
        what[size] = k;
        size++;
    }

    /* Set the sort hooks */
    ang_sort_comp = ang_sort_comp_hook_string;
    ang_sort_swap = ang_sort_swap_hook_string;

    /* Sort */
    ang_sort(text, what, size);

    /* Save the size */
    borg_single_size = size;

    /* Allocate the "item parsing arrays" (plurals) */
    C_MAKE(borg_single_text, borg_single_size, cptr);
    C_MAKE(borg_single_what, borg_single_size, s16b);

    /* Save the entries */
    for (i = 0; i < size; i++) borg_single_text[i] = text[i];
    for (i = 0; i < size; i++) borg_single_what[i] = what[i];


    /*** Artifact and Ego-Item Parsers ***/

    /* No entries yet */
    size = 0;

    /* Collect the "artifact names" */
    for (k = 1; k < z_info->a_max; k++)
    {
        artifact_type *a_ptr = &a_info[k];

        /* Skip non-items */
        if (!a_ptr->name) continue;

        /* Extract a string */
        sprintf(buf, " %s", (a_name + a_ptr->name));

        /* Save an entry */
        text[size] = string_make(buf);
        what[size] = k;
        size++;
    }

    C_MAKE(borg_sv_art_text, z_info->a_max, cptr);
    for (i = 0; i < size; i++) borg_sv_art_text[what[i]] = text[i];

    /* Collect the "ego-item names" */
    for (k = 1; k < z_info->e_max; k++)
    {
        ego_item_type *e_ptr = &e_info[k];

        /* Skip non-items */
        if (!e_ptr->name) continue;

        /* Extract a string */
        sprintf(buf, " %s", (e_name + e_ptr->name));

        /* Save an entry */
        text[size] = string_make(buf);
        what[size] = k + 256;
        size++;
    }

    /* Set the sort hooks */
    ang_sort_comp = ang_sort_comp_hook_string;
    ang_sort_swap = ang_sort_swap_hook_string;


    /* Sort */
    ang_sort(text, what, size);

    /* Save the size */
    borg_artego_size = size;

    /* Allocate the "item parsing arrays" (plurals) */
    C_MAKE(borg_artego_text, borg_artego_size, cptr);
    C_MAKE(borg_artego_what, borg_artego_size, s16b);

    /* Save the entries */
    for (i = 0; i < size; i++) borg_artego_text[i] = text[i];
    for (i = 0; i < size; i++) borg_artego_what[i] = what[i];

	/* Clean up */
	FREE(what);
	FREE(text);
}

cptr borg_prt_item(int item)
{
            if (item < z_info->k_max)
            {
                return borg_sv_plural_text[item];
            }
            if (item < z_info->k_max * 2)
                return borg_sv_plural_text[item - z_info->k_max];
            if (item < z_info->k_max * 2 + z_info->a_max)
                return borg_sv_art_text[item - z_info->k_max * 2];
            return (prefix_pref[item - z_info->k_max * 2
                                 - z_info->a_max]);

}


#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
