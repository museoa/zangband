/* File: artifact.c */

/* Purpose: Artifact code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "script.h"

/* Chance of using syllables to form the name instead of the "template" files */
#define TABLE_NAME      45
#define A_CURSED        13
#define WEIRD_LUCK      12
#define BIAS_LUCK       20

/*
 * Bias luck needs to be higher than weird luck,
 * since it is usually tested several times...
 */
#define ACTIVATION_CHANCE 3

static void random_plus(object_type *o_ptr)
{
	switch (randint1(o_ptr->tval < TV_BOOTS ? 24 : 20))
	{
		case 1:  case 2:
			o_ptr->flags1 |= TR1_STR;
			break;
		case 3:  case 4:
			o_ptr->flags1 |= TR1_INT;
			break;
		case 5:  case 6:
			o_ptr->flags1 |= TR1_WIS;
			break;
		case 7:  case 8:
			o_ptr->flags1 |= TR1_DEX;
			break;
		case 9:  case 10:
			o_ptr->flags1 |= TR1_CON;
			break;
		case 11:  case 12:
			o_ptr->flags1 |= TR1_CHR;
			break;
		case 13:  case 14:
			o_ptr->flags1 |= TR1_STEALTH;
			break;
		case 15:  case 16:
			o_ptr->flags1 |= TR1_SEARCH;
			break;
		case 17:  case 18:
			o_ptr->flags1 |= TR1_INFRA;
			break;
		case 19:
			o_ptr->flags1 |= TR1_SPEED;
			break;
		case 20:
			o_ptr->flags1 |= TR1_SP;
			break;
		case 21:  case 22:
			o_ptr->flags1 |= TR1_TUNNEL;
			break;
		case 23:  case 24:
			if (o_ptr->tval == TV_BOW)
			{
				o_ptr->flags1 |= TR1_DEX;
			}
			else
			{
				o_ptr->flags1 |= TR1_BLOWS;
			}
			break;
	}
}


static void random_resistance(object_type *o_ptr, int specific)
{
	switch (specific ? specific : randint1(42))
	{
		case 1:
			if (!one_in_(WEIRD_LUCK))
				o_ptr->flags2 |= TR2_RES_ACID;
			else
				o_ptr->flags2 |= TR2_IM_ACID;
			break;
		case 2:
			if (!one_in_(WEIRD_LUCK))
				o_ptr->flags2 |= TR2_RES_ELEC;
			else
				o_ptr->flags2 |= TR2_IM_ELEC;
			break;
		case 3:
			if (!one_in_(WEIRD_LUCK))
				o_ptr->flags2 |= TR2_RES_COLD;
			else
				o_ptr->flags2 |= TR2_IM_COLD;
			break;
		case 4:
			if (!one_in_(WEIRD_LUCK))
				o_ptr->flags2 |= TR2_RES_FIRE;
			else
				o_ptr->flags2 |= TR2_IM_FIRE;
			break;
		case 5:
		case 6:
		case 13:
			o_ptr->flags2 |= TR2_RES_ACID;
			break;
		case 7:
		case 8:
		case 14:
			o_ptr->flags2 |= TR2_RES_ELEC;
			break;
		case 9:
		case 10:
		case 15:
			o_ptr->flags2 |= TR2_RES_FIRE;
			break;
		case 11:
		case 12:
		case 16:
			o_ptr->flags2 |= TR2_RES_COLD;
			break;
		case 17:
		case 18:
			o_ptr->flags2 |= TR2_RES_POIS;
			break;
		case 19:
		case 20:
			o_ptr->flags2 |= TR2_RES_FEAR;
			break;
		case 21:
			if (!one_in_(WEIRD_LUCK))
				o_ptr->flags2 |= TR2_RES_LITE;
			else
				o_ptr->flags4 |= TR4_IM_LITE;
			break;
		case 22:
			if (!one_in_(WEIRD_LUCK))
				o_ptr->flags2 |= TR2_RES_DARK;
			else
				o_ptr->flags4 |= TR4_IM_DARK;
			break;
		case 23:
		case 24:
			o_ptr->flags2 |= TR2_RES_BLIND;
			break;
		case 25:
		case 26:
			o_ptr->flags2 |= TR2_RES_CONF;
			break;
		case 27:
		case 28:
			o_ptr->flags2 |= TR2_RES_SOUND;
			break;
		case 29:
		case 30:
			o_ptr->flags2 |= TR2_RES_SHARDS;
			break;
		case 31:
		case 32:
			o_ptr->flags2 |= TR2_RES_NETHER;
			break;
		case 33:
		case 34:
			o_ptr->flags2 |= TR2_RES_NEXUS;
			break;
		case 35:
		case 36:
			o_ptr->flags2 |= TR2_RES_CHAOS;
			break;
		case 37:
		case 38:
			o_ptr->flags2 |= TR2_RES_DISEN;
			break;
		case 39:
			if (o_ptr->tval >= TV_CLOAK && o_ptr->tval <= TV_HARD_ARMOR)
				o_ptr->flags3 |= TR3_SH_ELEC;
			else
				o_ptr->flags2 |= TR2_RES_ELEC;
			break;
		case 40:
			if (o_ptr->tval >= TV_CLOAK && o_ptr->tval <= TV_HARD_ARMOR)
				o_ptr->flags3 |= TR3_SH_FIRE;
			else
				o_ptr->flags2 |= TR2_RES_FIRE;
			break;
		case 41:
			if (o_ptr->tval >= TV_CLOAK && o_ptr->tval <= TV_HARD_ARMOR)
				o_ptr->flags4 |= TR4_SH_COLD;
			else
				o_ptr->flags2 |= TR2_RES_COLD;
			break;
		/* Note: SH_ACID is deliberately omitted here */
		case 42:
			if (o_ptr->tval == TV_SHIELD || o_ptr->tval == TV_CLOAK ||
				o_ptr->tval == TV_HELM || o_ptr->tval == TV_HARD_ARMOR)
				o_ptr->flags2 |= TR2_REFLECT;
			else
				o_ptr->flags2 |= TR2_RES_FEAR;
			break;
	}
}



static void random_misc(object_type *o_ptr)
{
	switch (randint1(38))
	{
		case 1:
			o_ptr->flags2 |= TR2_SUST_STR;
			break;
		case 2:
			o_ptr->flags2 |= TR2_SUST_INT;
			break;
		case 3:
			o_ptr->flags2 |= TR2_SUST_WIS;
			break;
		case 4:
			o_ptr->flags2 |= TR2_SUST_DEX;
			break;
		case 5:
			o_ptr->flags2 |= TR2_SUST_CON;
			break;
		case 6:
			o_ptr->flags2 |= TR2_SUST_CHR;
			break;
		case 7:
		case 8:
		case 14:
			o_ptr->flags2 |= TR2_FREE_ACT;
			break;
		case 9:
			o_ptr->flags2 |= TR2_HOLD_LIFE;
			break;
		case 10:
		case 11:
			o_ptr->flags3 |= TR3_LITE;
			break;
		case 12:
		case 13:
			o_ptr->flags3 |= TR3_FEATHER;
			break;
		case 15:
			if (o_ptr->tval == TV_GLOVES)
				o_ptr->flags4 |= TR4_GHOUL_TOUCH;
			else
				o_ptr->flags3 |= TR3_SEE_INVIS;

			break;
		case 16:
		case 17:
			o_ptr->flags3 |= TR3_SEE_INVIS;
			break;
		case 18:
			o_ptr->flags3 |= TR3_TELEPATHY;
			break;
		case 19:
		case 20:
			o_ptr->flags3 |= TR3_SLOW_DIGEST;
			break;
		case 21:
		case 22:
			o_ptr->flags3 |= TR3_REGEN;
			break;
		case 23:
			o_ptr->flags3 |= TR3_TELEPORT;
			break;
		case 24:
		case 25:
		case 26:
			if (o_ptr->tval >= TV_BOOTS && o_ptr->tval < TV_LITE)
			{
				o_ptr->flags3 |= TR3_SLOW_DIGEST;
			}
			else
			{
				o_ptr->flags3 |= TR3_SHOW_MODS;
				o_ptr->to_a = (s16b)rand_range(5, 15);
			}
			break;
		case 27:
		case 28:
		case 29:
			o_ptr->flags3 |= TR3_SHOW_MODS;
			o_ptr->to_h += (s16b)rand_range(5, 15);
			o_ptr->to_d += (s16b)rand_range(5, 15);
			break;
		case 30:
			o_ptr->flags3 |= TR3_NO_MAGIC;
			break;
		case 31:
			o_ptr->flags3 |= TR3_NO_TELE;
			break;
		case 32:
		case 33:
		case 34:
			/* A slay on a non-weapon gives protection */
			switch (randint1(8))
			{
				case 1: o_ptr->flags1 |= TR1_SLAY_ANIMAL; break;
				case 2: o_ptr->flags1 |= TR1_SLAY_EVIL; break;
				case 3: o_ptr->flags1 |= TR1_SLAY_UNDEAD; break;
				case 4: o_ptr->flags1 |= TR1_SLAY_DEMON; break;
				case 5: o_ptr->flags1 |= TR1_SLAY_ORC; break;
				case 6: o_ptr->flags1 |= TR1_SLAY_TROLL; break;
				case 7: o_ptr->flags1 |= TR1_SLAY_GIANT; break;
				case 8: o_ptr->flags1 |= TR1_SLAY_DRAGON; break;
			}
			break;
		case 35:
			o_ptr->flags4 |= TR4_MUTATE;
			break;
		case 36:
			o_ptr->flags4 |= TR4_PATRON;
			break;
		case 37:
			o_ptr->flags4 |= TR4_STRANGE_LUCK;
			break;
		case 38:
			o_ptr->flags4 |= TR4_LUCK_10;
			break;
	}
}

static void random_curse(object_type *o_ptr, bool evil)
{
	switch (randint1(evil ? 30 : 16))
	{
		case 1:
		case 17:
			o_ptr->flags4 |= TR4_HURT_ACID;
			break;
		case 2:
		case 18:
			o_ptr->flags4 |= TR4_HURT_ELEC;
			break;
		case 3:
		case 19:
			o_ptr->flags4 |= TR4_HURT_FIRE;
			break;
		case 4:
		case 20:
			o_ptr->flags4 |= TR4_HURT_COLD;
			break;
		case 5:
			o_ptr->flags4 |= TR4_HURT_LITE;
			break;
		case 6:
			o_ptr->flags4 |= TR4_HURT_DARK;
			break;
		case 7:
		case 8:
			o_ptr->flags3 |= TR3_AGGRAVATE;
			break;
		case 9:
			o_ptr->flags4 |= TR4_SLOW_HEAL;
			break;
		case 10:
		case 21:
			o_ptr->flags4 |= TR4_DRAIN_STATS;
			break;
		case 11:
		case 12:
			o_ptr->flags4 |= TR4_AUTO_CURSE;
			break;
		case 13:
		case 14:
			o_ptr->flags4 |= TR4_CANT_EAT;
			break;
		case 15:
		case 16:
			o_ptr->flags3 |= TR3_CURSED;
			break;
		case 22:
		case 23:
			o_ptr->flags3 |= TR3_TELEPORT;
			break;
		case 24:
			o_ptr->flags3 |= TR3_DRAIN_EXP;
			break;
		case 25:
		case 26:
			o_ptr->flags3 |= TR3_TY_CURSE;
			break;
		case 27:
		case 28:
		case 29:
			o_ptr->flags3 |= TR3_CURSED | TR3_HEAVY_CURSE;
			break;
		case 30:
			o_ptr->flags3 |= TR3_NO_MAGIC;
			break;
	}
}

static void random_slay(object_type *o_ptr)
{
	/* Bows get special treatment */
	if (o_ptr->tval == TV_BOW)
	{
		switch (randint1(6))
		{
			case 1:
			case 2:
			case 3:
				o_ptr->flags3 |= TR3_XTRA_MIGHT;
				break;
			default:
				o_ptr->flags3 |= TR3_XTRA_SHOTS;
				break;
		}
	}

	switch (randint1(36))
	{
		case 1:
		case 2:
			o_ptr->flags1 |= TR1_SLAY_ANIMAL;
			break;
		case 3:
		case 4:
			o_ptr->flags1 |= TR1_SLAY_EVIL;
			break;
		case 5:
		case 6:
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;
			break;
		case 7:
		case 8:
			o_ptr->flags1 |= TR1_SLAY_DEMON;
			break;
		case 9:
		case 10:
			o_ptr->flags1 |= TR1_SLAY_ORC;
			break;
		case 11:
		case 12:
			o_ptr->flags1 |= TR1_SLAY_TROLL;
			break;
		case 13:
		case 14:
			o_ptr->flags1 |= TR1_SLAY_GIANT;
			break;
		case 15:
		case 16:
			o_ptr->flags1 |= TR1_SLAY_DRAGON;
			break;
		case 17:
			o_ptr->flags1 |= TR1_KILL_DRAGON;
			break;
		case 18:
		case 19:
			if (o_ptr->tval == TV_SWORD)
			{
				o_ptr->flags1 |= TR1_VORPAL;
			}
			else
			{
				o_ptr->flags1 |= TR1_IMPACT;
			}
			break;
		case 20:
		case 21:
		case 22:
			o_ptr->flags1 |= TR1_BRAND_FIRE;
			break;
		case 23:
		case 24:
			o_ptr->flags1 |= TR1_BRAND_COLD;
			break;
		case 25:
		case 26:
			o_ptr->flags1 |= TR1_BRAND_ELEC;
			break;
		case 27:
		case 28:
			o_ptr->flags1 |= TR1_BRAND_ACID;
			break;
		case 29:
		case 30:
			o_ptr->flags1 |= TR1_BRAND_POIS;
			break;
		case 31:
		case 32:
			o_ptr->flags1 |= TR1_VAMPIRIC;
			break;
		case 33:
		case 34:
			o_ptr->flags4 |= TR4_PSI_CRIT;
			break;
		default:
			o_ptr->flags1 |= TR1_CHAOTIC;
			break;
	}
}


static cptr activation_text[] = {
	"The %v glows extremely brightly...",
	"The %v throbs deep green...",
	"The %v glows an intense red...",
	"The %v glows black...",
	"The %v glows an intense blue...",
	"The %v throbs red...",
	"The %v glows deep red...",
	"The %v glows bright white...",
	"The %v glows deep blue...",
	"The %v glows in scintillating colours...",
	"The %v vibrates...",
	"The %v glows violet...",
	"The %v lets out a long, shrill note...",
	"The %v twists in your hands...",
	"The %v shudders...",
	"The %v fades in and out...",
	"The %v hums softly...",
	"The %v blinks in and out...",
	"The %v radiates light blue...",
	"The %v radiates deep purple...",
	"The %v glows deep green...",
	"The %v lets out a shrill wail...",
	"The %v glows brightly...",
	"The %v shines brightly...",
	"The %v glows yellow...",
	"The %v glows light blue...",
	"The %v glows brown...",
	"The %v pulsates...",
	"The %v hums...",
	"The %v glows bright yellow..."
};

static cptr element_list[] =
{
	"GF_ACID", "GF_ELEC", "GF_FIRE", "GF_COLD", "GF_POIS",
	"GF_PLASMA", "GF_WATER", "GF_LITE", "GF_DARK", "GF_SHARDS",
	"GF_SOUND", "GF_CONFUSION", "GF_FORCE", "GF_INERTIA", "GF_MANA",
	"GF_ICE", "GF_CHAOS", "GF_NETHER", "GF_NEXUS", "GF_TIME",
	"GF_GRAVITY", "GF_NUKE", "GF_HOLY_FIRE", "GF_HELL_FIRE", "GF_MISSILE",
};
static cptr element_names[] =
{
	"acid", "lightning", "fire", "cold", "poison",
	"plasma", "water", "light", "darkness", "shards",
	"sound", "confusion", "force", "inertia", "mana",
	"ice", "chaos", "nether", "nexus", "time",
	"gravity", "toxic waste", "holy power", "unholy power", "elemental force",
};
static cptr element_colors[] =
{
	"dark grey", "light blue", "bright red", "pale white", "dark green",
	"bright orange", "dark blue", "bright white", "black", "dark red",
	"amber", "dark purple", "light grey", "silver", "many colors",
	"pale white", "many colors", "black", "yellow", "silver",
	"dark grey", "sickly yellow", "pure white", "dark red", "many colors",
};
static cptr glow_desc[] =
{
	"glows", "shines", "pulses", "throbs", "radiates", "sparks"
};

#define SPDICE_MM -1

/*
 * {
 * text,
 * desc,
 * effect,
 * freq, aimed, pp, dice, bonus
 * }
 */
static const struct randart_activation randart_activations[] =
{
	{
		NULL,
		"remove fear & cure poison",
		"clear_afraid(); clear_poisoned()",
		20, FALSE, 100, 0, 0
	},
	{
		NULL,
		"stone to mud",
		"wall_to_mud(dir)",
		100, TRUE, 1000, 0, 0
	},
	{
		NULL,
		"destroy doors",
		"destroy_doors_touch()",
		20, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"confuse monster",
		"confuse_monster(dir, 50)",
		50, TRUE, 1000, 0, 0
	},
	{
		NULL,
		"sleep monster",
		"sleep_monster(dir)",
		50, TRUE, 1000, 0, 0
	},
	{
		NULL,
		"slow monster",
		"slow_monster(dir)",
		50, TRUE, 1000, 0, 0
	},
	{
		NULL,
		"identify",
		"if not ident_spell() then return end",
		100, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"recharging",
		"recharge(130)",
		25, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"sleep nearby monsters",
		"sleep_monsters_touch()",
		25, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"sleep monsters",
		"sleep_monsters()",
		10, FALSE, 5000, 0, 0
	},
	{
		"You open a dimensional gate. Choose a destination.",
		"dimension door",
		"if not dimension_door() return end",
		25, FALSE, 1000, 0, 0
	},
	{
		"The %v twists space around you...",
		"teleport (100)",
		"teleport_player(100)",
		100, FALSE, 1000, 0, 0
	},
	{
		"An image forms in your mind...",
		"detection",
		"detect_all()",
		50, FALSE, 1000, 0, 0
	},
	{
		"The %v glows bright red...",
		"explosive rune",
		"explosive_rune()",
		5, FALSE, 5000, 0, 0
	},
	{
		NULL,
		"word of recall",
		"word_of_recall()",
		100, FALSE, 5000, 0, 0
	},
	{
		NULL,
		"restore life levels",
		"restore_level()",
		10, FALSE, 25000, 0, 0
	},
	{
		NULL,
		"teleport away",
		"fire_beam(GF_AWAY_ALL, dir, plev)",
		50, TRUE, 5000, 0, 0
	},
	{
		NULL,
		"charm animal",
		"charm_animal(dir, plev)",
		10, TRUE, 5000, 0, 0
	},
	{
		NULL,
		"charm animal (level %s)",
		"charm animal(dir, %s)",
		10, TRUE, 200, 100, 0	/* lev */
	},
	{
		NULL,
		"enslave undead",
		"control_one_undead(dir, plev)",
		10, TRUE, 7500, 0, 0
	},
	{
		NULL,
		"enslave undead (level %s)",
		"control_one_undead(dir, %s)",
		10, TRUE, 300, 100, 0	/* lev */
	},
	{
		NULL,
		"charm monster",
		"charm_monster(dir, plev)",
		10, TRUE, 10000, 0, 0
	},
	{
		NULL,
		"charm monster (level %s)",
		"charm_monster(dir, %s)",
		10, TRUE, 400, 100, 0	/* lev */
	},
	{
		NULL,
		"alchemy",
		"alchemy()",
		5, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"rune of protection",
		"warding_glyph()",
		10, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"animal friendship",
		"charm_animals(plev * 2)",
		5, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"animal friendship (level %s)",
		"charm_animals(%s * 2)",
		5, FALSE, 750, 100, 0	/* lev */
	},
	{
		"The power of the %v banishes evil!",
		"banish evil",
		"banish_evil(200)",
		25, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"genocide",
		"genocide(TRUE)",
		10, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"satisfy hunger",
		"set_food(PY_FOOD_MAX - 1)",
		100, FALSE, 10000, 0, 0
	},
	{
		"The %v emits a blast of air...",
		"whirlwind attack",
		"whirlwind_attack()",
		10, FALSE, 20000, 0, 0
	},
	{
		"The %v glows in scintillating colours...",
		"call chaos",
		"call_chaos()",
		10, FALSE, 25000, 0, 0
	},
	{
		NULL,
		"mass genocide",
		"mass_genocide(TRUE)",
		5, FALSE, 25000, 0, 0
	},
	{
		NULL,
		"mass charm",
		"charm_monsters(plev * 2)",
		5, FALSE, 25000, 0, 0
	},
	{
		NULL,
		"mass charm (level %s)",
		"charm_monsters(%s * 2)",
		5, FALSE, 2000, 100, 0	/* lev */
	},
	{
		NULL,
		"restore stats",
		"restore_all_stats()",
		10, FALSE, 25000, 0, 0
	},
	{
		NULL,
		"restore stats and life levels",
		"restore_all_stats(); restore_level()",
		5, FALSE, 50000, 0, 0
	},
	{
		NULL,
		"identify true",
		"identify_fully()",
		25, FALSE, 25000, 0, 0
	},
	{
		NULL,
		"detection, probing and identify true",
		"detect_all(); probing(); identify_fully()",
		5, FALSE, 50000, 0, 0
	},
	{
		"A line of sunlight appears.",
		"beam of sunlight",
		"lite_line(dir)",
		100, TRUE, 500, 0, 0
	},
	{
		"The %v glow extremely brightly...",
		"magic missile (%sd6)",
		"fire_bolt(GF_MISSILE, dir, damroll(%s, 6))",
		100, TRUE, 12, 10, 2	/* lev / 10 + 2 */
	},
	{
		"The %v throbs deep green...",
		"stinking cloud (%s)",
		"fire_ball(GF_POIS, dir, %s, 2)",
		50, TRUE, 5, 100, 10	/* lev + 10 */
	},
	{
		"The %v glows black...",
		"drain life (%s)",
		"drain_life(dir, %s)",
		25, TRUE, 10, 500, 0	/* lev * 5 */
	},
	{
		"The %v throbs red...",
		"vampiric drain (%s)",
		"drain_gain_life(dir, %s)",
		25, TRUE, 15, 500, 0	/* lev * 5 */
	},
	{
		"The %v grows magical spikes...",
		"arrows (%s)",
		"fire_bolt(GF_ARROW, dir, %s)",
		10, TRUE, 10, 500, 0	/* lev * 5 */
	},
	{
		"The %v grows magical spikes...",
		"arrows (%sd25)",
		"fire_bolt(GF_ARROW, dir, damroll(%s, 25))",
		5, TRUE, 125, 33, 1	/* lev / 3 + 1 */
	},
	{
		"The %v grows magical spikes...",
		"arrows (%sd50)",
		"fire_bolt(GF_ARROW, dir, damroll(%s, 50))",
		5, TRUE, 250, 33, 1	/* lev / 3 + 1 */
	},
	{
		"You launch a rocket!",
		"launch rocket (%s)",
		"fire_ball(GF_ROCKET, dir, %s, 2)",
		5, TRUE, 20, 500, 100	/* rlev * 5 + 100 */
	},
	{
		"The %v floods the area with goodness...",
		"dispel evil (%s)",
		"dispel_evil(%s)",
		50, FALSE, 100, 500, 0	/* rlev * 5 */
	},
	{
		"The %v floods the area with evil...",
		"dispel good (%s)",
		"dispel_good(%s)",
		25, FALSE, 100, 500, 0	/* rlev * 5 */
	},
	{
		/* Note that this is more powerful than a normal breath activation */
		"You breathe the elements.",
		"breathe the elements (%s, rad. 4)",
		"fire_ball(GF_MISSILE, dir, %s, 4)",
		5, FALSE, 40, 1000, 0	/* rlev * 10 */
	},
	{
		"The %v vibrates...",
		"earthquake (rad. %s)",
		"earthquake(px, py, %s)",
		10, FALSE, 250, 25, 5	/* lev / 4 + 5 */
	},
	{
		"The %v emits a loud blast...",
		"terror",
		"turn_monsters(40 + plev)",
		10, FALSE, 2500, 0, 0
	},
	{
		"You summon a beast.",
		"summon animal",
		"summon_controlled(SUMMON_ANIMAL_RANGER)",
		25, FALSE, 10000, 0, 0
	},
	{
		"You summon a phantasmal servant.",
		"summon phantasmal servant",
		"summon_controlled(SUMMON_PHANTOM)",
		25, FALSE, 10000, 0, 0
	},
	{
		"You summon an elemental.",
		"summon elemental",
		"summon_unsafe(SUMMON_ELEMENTAL)",
		25, FALSE, 25000, 0, 0
	},
	{
		"Ancient, long-dead forms rise from the ground.",
		"summon undead",
		"summon_unsafe(SUMMON_UNDEAD)",
		25, FALSE, 25000, 0, 0
	},
	{
		"Ancient, long-dead forms rise from the ground.",
		"summon greater undead",
		"summon_unsafe(SUMMON_HI_UNDEAD)",
		10, FALSE, 50000, 0, 0
	},
	{
		"The area fills with the stench of sulphur and brimstone.",
		"summon demon",
		"summon_unsafe(SUMMON_DEMON)",
		25, FALSE, 25000, 0, 0
	},
	{
		"The area fills with brilliant white light.",
		"summon angel",
		"summon_controlled(SUMMON_ANGEL)",
		10, FALSE, 50000, 0, 0
	},
	{
		NULL,
		"remove fear & heal (%sd10)",
		"clear_afraid(); hp_player(damroll(%s, 10))",
		25, FALSE, 100, 33, 1	/* lev / 3 + 1 */
	},
	{
		NULL,
		"cure wounds & heal (%sd10)",
		"hp_player(damroll(%s, 10)); clear_cut(); clear_stun()",
		100, FALSE, 100, 100, 0	/* lev */
	},
	{
		NULL,
		"cure wounds & heal (%s)",
		"hp_player(%s); clear_cut(); clear_stun()",
		100, FALSE, 20, 1000, 0	/* lev * 10 */
	},
	{
		"The %v enters your thoughts...",
		"telepathy (%s+ turns)",
		"inc_time_esp(rand_range2(%s))",
		25, FALSE, 200, 100, 0	/* lev */
	},
	{
		NULL,
		"heroism (%s+ turns)",
		"inc_hero(rand_range2(%s))",
		25, FALSE, 150, 100, 0	/* lev */
	},
	{
		NULL,
		"berserk (%s+ turns)",
		"inc_shero(rand_range2(%s))",
		25, FALSE, 150, 100, 0	/* lev */
	},
	{
		NULL,
		"bless (%s+ turns)",
		"inc_blessed(rand_range2(%s))",
		25, FALSE, 100, 100, 0	/* lev */
	},
	{
		NULL,
		"protection from evil (%s+ turns)",
		"inc_protevil(rand_range2(%s))",
		25, FALSE, 100, 200, 0	/* lev * 2 */
	},
	{
		"The %v glows many colours...",
		"resist elements (%s+ turns)",
		"inc_oppose_all(rand_range2(%s))",
		25, FALSE, 250, 100, 0	/* lev */
	},
	{
		NULL,
		"resist acid (%s+ turns)",
		"inc_oppose_acid(rand_range2(%s))",
		10, FALSE, 100, 100, 0	/* lev */
	},
	{
		NULL,
		"resist lightning (%s+ turns)",
		"inc_oppose_elec(rand_range2(%s))",
		10, FALSE, 100, 100, 0	/* lev */
	},
	{
		NULL,
		"resist fire (%s+ turns)",
		"inc_oppose_fire(rand_range2(%s))",
		10, FALSE, 100, 100, 0	/* lev */
	},
	{
		NULL,
		"resist cold (%s+ turns)",
		"inc_oppose_cold(rand_range2(%s))",
		10, FALSE, 100, 100, 0	/* lev */
	},
	{
		NULL,
		"resist poison (%s+ turns)",
		"inc_oppose_poison(rand_range2(%s))",
		5, FALSE, 100, 100, 0	/* lev */
	},
	{
		NULL,
		"speed (%s+ turns)",
		"inc_fast(rand_range2(%s))",
		25, FALSE, 200, 100, 0	/* lev */
	},
	{
		"The %v fades out...",
		"wraith form (%s+ turns)",
		"inc_wraith_form(rand_range2(%s))",
		5, FALSE, 2000, 33, 1	/* lev / 3 + 1 */
	},
	{
		"The %v fires a beam of bright light at you...",
		"invulnerability (%s+ turns)",
		"inc_invuln(rand_range2(%s))",
		5, FALSE, 2000, 10, 3	/* lev / 10 + 3 */
	},
	{
		"The %v wells with clear light...",
		"light area (2d20)",
		"lite_area(damroll(2, 20), 3)",
		100, FALSE, 100, 0, 0
	},
	{
		"The %v shines brightly...",
		"map & light area (5d20)",
		"lite_area(damroll(5, 20), 3)",
		50, FALSE, 400, 0, 0
	},
	{
		NULL,
		"detect evil",
		"detect_monsters_evil()",
		25, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"detect monsters",
		"detect_monsters()",
		50, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"detect traps and doors",
		"detect_traps(); detect_doors(); detect_stairs()",
		50, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"remove curse",
		"remove_curse()",
		10, FALSE, 25000, 0, 0
	},
	{
		NULL,
		"dispel curse",
		"remove_all_curse()",
		5, FALSE, 50000, 0, 0
	},
	{
		NULL,
		"dispel undead (%s)",
		"dispel_undead(%s)",
		25, FALSE, 50, 500, 0	/* rlev * 5 */
	},
	{
		NULL,
		"dispel demons (%s)",
		"dispel_demons(%s)",
		25, FALSE, 50, 500, 0	/* rlev * 5 */
	},
	{
		NULL,
		"dispel living (%s)",
		"dispel_living (%s)",
		5, FALSE, 200, 400, 0	/* lev * 4 */
	},
	{
		NULL,
		"dispel monsters (%s)",
		"dispel_monsters(%s)",
		5, FALSE, 250, 400, 0	/* lev * 4 */
	},
	{
		NULL,
		"slow monsters",
		"slow_monsters()",
		10, FALSE, 25000, 0, 0
	},
	{
		NULL,
		"detect objects",
		"detect_objects_normal()",
		25, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"detect treasure",
		"detect_treasure(); detect_objects_gold()",
		10, FALSE, 1000, 0, 0
	},
	{
		NULL,
		"detect enchantment",
		"detect_objects_magic()",
		10, FALSE, 2500, 0, 0
	},
	{
		NULL,
		"self knowledge",
		"self_knowledge()",
		10, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"teleport level",
		"teleport_player_level()",
		5, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"create doors",
		"door_creation()",
		5, FALSE, 2500, 0, 0
	},
	{
		NULL,
		"create stairs",
		"stair_creation()",
		5, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"alter reality",
		"alter_reality()",
		5, FALSE, 10000, 0, 0
	},
	{
		NULL,
		"polymorph self",
		"polymorph_self()",
		10, FALSE, 5000, 0, 0
	},
	{
		NULL,
		"phase door",
		"teleport_player(10)",
		100, FALSE, 100, 0, 0
	},
	{
		NULL,
		"banishment",
		"banish_monsters(200)",
		5, FALSE, 25000, 0, 0
	},
		
	/* XXX stun, confuse, turn, stasis */

};
	
static void apply_activation_power(object_type *o_ptr, cptr text, cptr desc, cptr effect, bool aimed, int pp, int level)
{
	char buf[1024];
	char text_buf[256];
	int len;
	int charge_min;

	/* Don't add a power if there already is one */
	if (o_ptr->flags3 & TR3_ACTIVATE)
		return;

	/* Calculate charge time */
	if (level > 0)
		charge_min = pp / level;
	else
		charge_min = pp;

	/* Round to nice numbers */
	if (charge_min >= 1000)
		charge_min -= charge_min % 100;
	else if (charge_min >= 250)
		charge_min -= charge_min % 50;
	else if (charge_min >= 100)
		charge_min -= charge_min % 10;
	else if (charge_min >= 25)
		charge_min -= charge_min % 5;

	/* Enforce minimum & maximum charge time */
	if (charge_min < 1) charge_min = 1;
	if (charge_min > 5000) charge_min = 5000;


	/* Get a description if needed */
	if (text == NULL)
		text = activation_text[randint0(NUM_ELEMENTS(activation_text))];

	/* Get the basic name of the object in the description */
	strnfmt(text_buf, 256, text, OBJECT_FMT(o_ptr, FALSE, 0));
	
	/* Construct the usage script */
	len = strnfmt(buf, 1024, "msgf(\"%s\"); local plev = player.lev; ", text_buf);
	
	if (aimed) strnfcat(buf, 1024, &len, 
				"local success; local dir; "
				"success, dir = get_aim_dir(); "
				"if not success then return; end; ");

	strnfcat(buf, 1024, &len, "%s; object.timeout = rand_range(%i, %i)", effect, charge_min, charge_min * 2);
	
	o_ptr->trigger[TRIGGER_USE] = quark_add(buf);
	
	
	/* Description script */
	len = strnfmt(buf, 1024, "return \"%s every %i-%i turns\"", desc, charge_min, charge_min * 2);
	o_ptr->trigger[TRIGGER_DESC] = quark_fmt(buf);

	o_ptr->flags3 |= TR3_ACTIVATE;
	o_ptr->timeout = 0;
}

static void attack_activation_power(object_type *o_ptr, int level, cptr fix_element)
{
	static char text[256];
	char effect[256] = "";
	char desc[256] = "";
	int element;
	int dice = 0;
	int sides = 1;
	int radius = 0;
	int pp = 0;	/* Charge time * level */
	bool aimed = TRUE;

	int rlev = level * rand_range(50, 150) / 100;
	if (rlev < 1) rlev = 1;

	if (fix_element == NULL)
	{
		/* Pick a low or high element */
		if (level <= randint1(60))
			element = randint0(5);
		else
			element = randint0(NUM_ELEMENTS(element_list));
	}
	else
	{
		for (element = 0; strcmp(fix_element, element_list[element]) != 0; element++)
			;
	}

	/* Describe the visual */
	strnfmt(text, 256, "The %%v %s %s...", glow_desc[randint0(NUM_ELEMENTS(glow_desc))], element_colors[element]);

	switch (randint1(10))
	{
	/* Breathe */
	case 1:
		strnfmt(text, 256, "You breathe %s.", element_names[element]);

		/* Dice, radius, and charge time */
		dice = rlev * 5;
		radius = 2 + dice / rand_range(100, 200);
		pp = dice * 10 * radius;

		/* Create the lua */
		strnfmt(desc, 256, "breathe %s (%i, rad. %i)", element_names[element], dice, radius);
		strnfmt(effect, 256, "fire_ball(%s, dir, %i, %i)", element_list[element], dice, radius);

		break;

	/* Emit a blast */
	case 2:
		strnfmt(text, 256, "The %%v emits a blast of %s...", element_names[element]);

		/* Dice, radius, and charge time */
		dice = rlev * 10;
		radius = 2 + dice / rand_range(50, 100);
		pp = dice * 2 * radius;
			
		aimed = FALSE;

		/* Create the lua */
		strnfmt(desc, 256, "%s blast (%i, rad. %i)", element_names[element], dice, radius);
		strnfmt(effect, 256, "fire_ball(%s, 0, %i, %i)", element_list[element], dice, radius);

		break;

	/* Fire a ball */
	case 3:
	case 4:
	case 5:
		/* Dice, radius, and charge time */
		dice = 5 * (1 + rlev / 2);
		radius = 2 + dice / rand_range(100, 200);
		pp = dice * 10 * radius;

		/* Create the lua */
		strnfmt(desc, 256, "%s%s ball (%i)", radius > 2 ? "large " : "", element_names[element], dice);
		strnfmt(effect, 256, "fire_ball(%s, dir, %i, %i)", element_list[element], dice, radius);

		break;

	/* Fire a beam */
	case 6:
		/* Dice and charge time */
		dice = 1 + rlev / 3;
		sides = rand_range(5, 8);
		pp = dice * sides * 10;

		/* Create the lua */
		strnfmt(desc, 256, "%s beam (%id%i)", element_names[element], dice, sides);
		strnfmt(effect, 256, "fire_beam(%s, dir, damroll(%i, %i))", element_list[element], dice, sides);

		break;

	/* Fire a bolt */
	default:
		/* Dice and charge time */
		dice = 1 + rlev / 2;
		sides = rand_range(5, 8);
		pp = dice * sides * 5;

		/* Create the lua */
		strnfmt(desc, 256, "%s bolt (%id%i)", element_names[element], dice, sides);
		strnfmt(effect, 256, "fire_bolt(%s, dir, damroll(%i, %i))", element_list[element], dice, sides);

		break;
	}

	apply_activation_power(o_ptr, text, desc, effect, aimed, pp, level);
}

static void misc_activation_power(object_type *o_ptr, int level, cptr fix_power)
{
	const struct randart_activation *act = NULL;
	char dice[32];
	char effect[256] = "";
	char desc[256] = "";
	int pp;

	int rlev = level * rand_range(50, 150) / 100;
	if (rlev < 1) rlev = 1;

	if (!fix_power)
	{
		do
		{
			act = &randart_activations[randint0(NUM_ELEMENTS(randart_activations))];
		}
		while (act->freq < randint1(100));
	}
	else
	{
		int i;
		for (i = 0; strcmp(randart_activations[i].desc, fix_power) != 0; i++)
			;
		act = &randart_activations[i];
	}
	
	/* Sometimes make the activation dependent on the player's level */
	if (one_in_(10) && act->dice >= 100 && act->bonus == 0)
	{
		int mult = act->dice * rand_range(50, 150) / 10000;
		if (mult < 1) mult = 1;

		if (mult > 1)
			strnfmt(dice, 32, "plev * %i", mult);
		else
			strcpy(dice, "plev");
		
		/* Fill in the dice */
		strnfmt(desc, 256, act->desc, dice);
		strnfmt(effect, 256, act->effect, dice);

		/* Assume plev 30 */
		pp = act->pp * mult * 30;
	}
	else
	{
		int d = rlev * act->dice / 100 + act->bonus;
		if (d < 1) d = 1;
		
		/* Fill in the dice */
		strnfmt(dice, 32, "%i", d);
		strnfmt(desc, 256, act->desc, dice);
		strnfmt(effect, 256, act->effect, dice);

		pp = act->pp * d;
	}
	
	apply_activation_power(o_ptr, act->text, desc, effect, act->aimed, pp, level);
}

static void random_activation_power(object_type *o_ptr, int level)
{
	int rlev = level * rand_range(50, 150) / 100;
	if (rlev < 1) rlev = 1;

	/* 60%/20% chance of a random attack */
	if (randint0(100) < (o_ptr->tval < TV_BOOTS ? 60 : 20))
		attack_activation_power(o_ptr, level, NULL);
	else
		misc_activation_power(o_ptr, level, NULL);
}

static void get_random_name(char *return_name, byte tval, int power)
{
	if ((randint1(100) <= TABLE_NAME) ||
		(tval == TV_AMULET) || (tval == TV_RING))
	{
		get_table_name(return_name, TRUE);
	}
	else
	{
		cptr filename;

		/* Armour or a Weapon? */
		if (tval >= TV_BOOTS)
		{
			switch (power)
			{
				case 0:
					filename = "a_cursed.txt";
					break;
				case 1:
					filename = "a_low.txt";
					break;
				case 2:
					filename = "a_med.txt";
					break;
				default:
					filename = "a_high.txt";
			}
		}
		else
		{
			switch (power)
			{
				case 0:
					filename = "w_cursed.txt";
					break;
				case 1:
					filename = "w_low.txt";
					break;
				case 2:
					filename = "w_med.txt";
					break;
				default:
					filename = "w_high.txt";
			}
		}

		(void)get_rnd_line(filename, 0, return_name);
	}
}


static int random_minor_theme_weapon(object_type *o_ptr, int level)
{
	int activate = 0;

	switch (randint1(39))
	{
		case 1:
		case 2:
		case 3:
			o_ptr->flags1 |= TR1_WIS;
			o_ptr->flags3 |= TR3_BLESSED;

			break;

		case 4:
		case 5:
			o_ptr->flags1 |= TR1_BRAND_ACID;
			o_ptr->flags2 |= TR2_RES_ACID;
			if (o_ptr->tval == TV_SWORD && one_in_(3))
				o_ptr->flags1 |= TR1_TUNNEL;

			if (one_in_(ACTIVATION_CHANCE))
				attack_activation_power(o_ptr, level, "GF_ACID");
			
			break;

		case 6:
		case 7:
			o_ptr->flags1 |= TR1_BRAND_ELEC;
			o_ptr->flags2 |= TR2_RES_ELEC;

			if (one_in_(ACTIVATION_CHANCE))
				attack_activation_power(o_ptr, level, "GF_ELEC");

			break;

		case 8:
		case 9:
		case 10:
			o_ptr->flags1 |= TR1_BRAND_FIRE;
			o_ptr->flags2 |= TR2_RES_FIRE;
			o_ptr->flags3 |= TR3_LITE;

			if (one_in_(ACTIVATION_CHANCE))
				attack_activation_power(o_ptr, level, "GF_FIRE");

			break;

		case 11:
		case 12:
		case 13:
			o_ptr->flags1 |= TR1_BRAND_COLD;
			o_ptr->flags2 |= TR2_RES_COLD;

			if (one_in_(ACTIVATION_CHANCE))
				attack_activation_power(o_ptr, level, "GF_COLD");

			break;

		case 14:
		case 15:	
			o_ptr->flags1 |= TR1_BRAND_POIS;
			o_ptr->flags2 |= TR2_RES_POIS;

			if (one_in_(ACTIVATION_CHANCE))
				attack_activation_power(o_ptr, level, "GF_POIS");

			break;

		case 16:
		case 17:
			o_ptr->flags1 |= TR1_CHAOTIC;
			o_ptr->flags2 |= TR2_RES_CHAOS;
			if (one_in_(3))
				o_ptr->flags4 |= TR4_PATRON;

			if (one_in_(ACTIVATION_CHANCE))
				attack_activation_power(o_ptr, level, "GF_CHAOS");
			
			break;

		case 18:
			if (o_ptr->tval == TV_SWORD)
			{
				o_ptr->flags1 |= TR1_VORPAL;
				o_ptr->flags1 |= TR1_TUNNEL;
			}
			else
			{
				o_ptr->flags1 |= TR1_BLOWS;
			}

			break;

		case 19:
		case 20:
			o_ptr->flags1 |= TR1_SLAY_ANIMAL;
			if (one_in_(2))
				o_ptr->flags1 |= TR1_INT;
			if (one_in_(2))
				o_ptr->flags3 |= TR3_REGEN;

			break;

		case 21:
		case 22:
		case 23:
			o_ptr->flags1 |= TR1_SLAY_EVIL;
			o_ptr->flags3 |= TR3_BLESSED;
			if (one_in_(2))
				o_ptr->flags1 |= TR1_WIS;
			if (one_in_(2))
				o_ptr->flags2 |= TR2_RES_FEAR;

			break;

		case 24:
		case 25:
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;
			if (one_in_(2))
				o_ptr->flags1 |= TR1_INT;
			if (one_in_(2))
				o_ptr->flags2 |= TR2_HOLD_LIFE;
			if (one_in_(2))
				o_ptr->flags3 |= TR3_SEE_INVIS;

			break;

		case 26:
		case 27:
			o_ptr->flags1 |= TR1_SLAY_DEMON;
			o_ptr->flags1 |= TR1_INT;
			
			break;

		case 28:
		case 29:
			o_ptr->flags1 |= TR1_SLAY_ORC;
			o_ptr->flags1 |= TR1_DEX;

			break;

		case 30:
		case 31:
			o_ptr->flags1 |= TR1_SLAY_GIANT;
			o_ptr->flags1 |= TR1_STR;

			break;

		case 32:
		case 33:
			o_ptr->flags1 |= TR1_SLAY_DRAGON;
			if (one_in_(3))
				o_ptr->flags1 |= TR1_KILL_DRAGON;
			o_ptr->flags1 |= TR1_CON;

			break;

		case 34:
		case 35:
			o_ptr->flags1 |= TR1_VAMPIRIC;
			o_ptr->flags2 |= TR2_HOLD_LIFE;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "vampiric drain (%s)");

			break;

		case 36:
			o_ptr->flags2 |= TR2_HOLD_LIFE;
			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "drain life (%s)");

			break;

		case 37:
			o_ptr->to_h += rand_range(5, 15);
			o_ptr->to_d += rand_range(5, 15);

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "whirlwind attack");

			break;

		case 38:
			o_ptr->flags1 |= TR1_SLAY_ANIMAL;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "charm animal");

			break;

		case 39:
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "enslave undead");

			break;

		case 40:
			if (o_ptr->tval == TV_SWORD)
				o_ptr->flags1 |= TR1_TUNNEL;

#if 0
			activate = ACT_STONE_MUD;
#endif

			break;
	}

	return activate;
}

static int random_major_theme_weapon(object_type *o_ptr, int level)
{
	int activate = 0;
	
	switch (randint1(7))
	{
	case 1:
		/* Holy Avenger */
		o_ptr->flags1 |= TR1_SLAY_EVIL;
		o_ptr->flags1 |= TR1_SLAY_UNDEAD;
		o_ptr->flags1 |= TR1_SLAY_DEMON;
		o_ptr->flags3 |= TR3_SEE_INVIS;
		o_ptr->flags3 |= TR3_BLESSED;

		if (one_in_(ACTIVATION_CHANCE))
			misc_activation_power(o_ptr, level, "dispel evil (%s)");
		
		break;

	case 2:
		/* Defender */
		o_ptr->flags2 |= TR2_RES_ACID;
		o_ptr->flags2 |= TR2_RES_ELEC;
		o_ptr->flags2 |= TR2_RES_FIRE;
		o_ptr->flags2 |= TR2_RES_COLD;
		if (one_in_(2))
			o_ptr->flags2 |= TR2_FREE_ACT;
		if (one_in_(2))
			o_ptr->flags3 |= TR3_SEE_INVIS;
		if (one_in_(2))
			o_ptr->flags3 |= TR3_FEATHER;
		if (one_in_(2))
			o_ptr->flags3 |= TR3_REGEN;
		if (one_in_(2))
			o_ptr->to_a += randint1(5);

#if 0
		if (one_in_(8))
			activate = ACT_RESIST_ALL;
#endif

		break;

	case 3:
		/* Westernesse */
		o_ptr->flags1 |= TR1_STR;
		o_ptr->flags1 |= TR1_DEX;
		o_ptr->flags1 |= TR1_CON;
		o_ptr->flags1 |= TR1_SLAY_ORC;
		o_ptr->flags1 |= TR1_SLAY_TROLL;
		o_ptr->flags1 |= TR1_SLAY_GIANT;

		break;

	case 4:
		/* Trump Weapon */
		o_ptr->flags1 |= TR1_SLAY_EVIL;
		o_ptr->flags3 |= TR3_TELEPORT;
		o_ptr->flags2 |= TR2_FREE_ACT;
		if (one_in_(2))
			o_ptr->flags1 |= TR1_SEARCH;
		if (one_in_(2))
			o_ptr->flags3 |= TR3_REGEN;
		if (one_in_(2))
			o_ptr->flags3 |= TR3_SLOW_DIGEST;

		if (one_in_(ACTIVATION_CHANCE))
			misc_activation_power(o_ptr, level, "teleport (100)");
		
		break;

	case 5:
		/* Pattern Weapon */
		o_ptr->flags1 |= TR1_STR;
		o_ptr->flags1 |= TR1_CON;
		o_ptr->flags2 |= TR2_FREE_ACT;
		o_ptr->flags3 |= TR3_SEE_INVIS;
		if (one_in_(2))
			o_ptr->flags1 |= TR1_SLAY_EVIL;
		if (one_in_(2))
			o_ptr->flags1 |= TR1_SLAY_DEMON;
		if (one_in_(2))
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;

		break;

	case 6:
		/* Mixed slays */
		if (one_in_(3))
			o_ptr->flags1 |= TR1_SLAY_ANIMAL;
		if (one_in_(3))
			o_ptr->flags1 |= TR1_SLAY_EVIL;
		if (one_in_(3))
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;
		if (one_in_(3))
			o_ptr->flags1 |= TR1_SLAY_DEMON;
		if (one_in_(3))
			o_ptr->flags1 |= TR1_SLAY_ORC;
		if (one_in_(3))
			o_ptr->flags1 |= TR1_SLAY_TROLL;
		if (one_in_(3))
			o_ptr->flags1 |= TR1_SLAY_GIANT;
		if (one_in_(3))
			o_ptr->flags1 |= TR1_SLAY_DRAGON;

		break;

	case 7:
		/* Assassin blade */
		o_ptr->flags1 |= TR1_STEALTH;
		o_ptr->flags1 |= TR1_BLOWS;
		o_ptr->flags2 |= TR2_FREE_ACT;
		
		if (one_in_(2))
			o_ptr->flags1 |= TR1_BRAND_POIS;
		else
			o_ptr->flags1 |= TR1_VAMPIRIC;
		
		if (o_ptr->tval == TV_SWORD)
			o_ptr->flags2 |= TR2_THROW;

		break;
	}

	return activate;
}

static int random_minor_theme_armor(object_type *o_ptr, int level)
{
	int activate = 0;

	switch (randint1(33))
	{
		case 1:
		case 2:
		case 3:
			o_ptr->flags3 |= TR3_SEE_INVIS;
			o_ptr->flags1 |= TR1_SEARCH;

			break;

		case 4:
		case 5:
			o_ptr->flags1 |= TR1_STR;
			o_ptr->flags2 |= TR2_SUST_STR;
			if (one_in_(3))
				o_ptr->flags2 |= TR2_RES_FEAR;

			break;

		case 6:
		case 7:
			o_ptr->flags1 |= TR1_INT;
			o_ptr->flags2 |= TR2_SUST_INT;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_FEATHER;

			break;

		case 8:
		case 9:
			o_ptr->flags1 |= TR1_WIS;
			o_ptr->flags2 |= TR2_SUST_WIS;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_SEE_INVIS;

			break;

		case 10:
		case 11:
			o_ptr->flags1 |= TR1_DEX;
			o_ptr->flags2 |= TR2_SUST_DEX;
			if (one_in_(3))
				o_ptr->flags2 |= TR2_FREE_ACT;

			break;

		case 12:
		case 13:
			o_ptr->flags1 |= TR1_CON;
			o_ptr->flags2 |= TR2_SUST_CON;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_REGEN;

			break;

		case 14:
		case 15:
			o_ptr->flags1 |= TR1_CHR;
			o_ptr->flags2 |= TR2_SUST_CHR;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_LITE;

			break;

		case 16:
			o_ptr->flags3 |= TR3_LITE;
			o_ptr->flags2 |= TR2_RES_LITE;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "light area (2d20)");

			break;

		case 17:
		case 18:
			o_ptr->flags2 |= TR2_RES_FIRE;
			o_ptr->flags3 |= TR3_SH_FIRE;

			break;

		case 19:
			o_ptr->flags2 |= TR2_RES_ELEC;
			o_ptr->flags3 |= TR3_SH_ELEC;

			break;

		case 20:
			o_ptr->flags2 |= TR2_RES_COLD;
			o_ptr->flags4 |= TR4_SH_COLD;

			break;

		case 21:
			o_ptr->flags1 |= TR1_INT;
			o_ptr->flags1 |= TR1_WIS;

			break;

		case 22:
			o_ptr->flags2 |= TR2_RES_LITE;
			o_ptr->flags2 |= TR2_RES_DARK;
			if (one_in_(2))
				o_ptr->flags3 |= TR3_LITE;

			break;

		case 23:
			o_ptr->flags1 |= TR1_SLAY_EVIL;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "banish evil");
			else if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "dispel evil (%s)");

			break;

		case 24:
			o_ptr->flags1 |= TR1_STEALTH;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "sleep nearby monsters");

			break;

		case 25:
			o_ptr->flags1 |= TR1_WIS;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "telepathy (%s+ turns)");

			break;

		case 26:
			o_ptr->flags2 |= TR2_RES_DARK;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "beam of sunlight");
			else if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "light area (2d20)");

			break;

		case 27:
			o_ptr->flags2 |= TR2_RES_CHAOS;
			o_ptr->flags2 |= TR2_RES_CONF;

			break;

		case 28:
			o_ptr->flags2 |= TR2_RES_NETHER;
			o_ptr->flags2 |= TR2_HOLD_LIFE;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "restore life levels");

			break;

		case 29:
			o_ptr->flags2 |= TR2_RES_SOUND;
			o_ptr->flags2 |= TR2_RES_SHARDS;

			break;

		case 30:
			o_ptr->flags2 |= TR2_RES_FEAR;

			if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "terror");
			else if (one_in_(ACTIVATION_CHANCE))
				misc_activation_power(o_ptr, level, "heroism (%s+ turns)");

			break;

		case 31:
			o_ptr->flags1 |= TR1_SLAY_ANIMAL;

#if 0
			if (one_in_(3))
				activate = ACT_CHARM_ANIMAL;
			else if (one_in_(2))
				activate = ACT_CHARM_ANIMALS;
			else
				activate = ACT_SUMMON_ANIMAL;
#endif

			break;

		case 32:
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;

#if 0
			if (one_in_(2))
				activate = ACT_CHARM_UNDEAD;
			else
				activate = ACT_SUMMON_UNDEAD;
#endif

			break;

		case 33:
			o_ptr->flags1 |= TR1_SLAY_DEMON;

#if 0
			activate = ACT_SUMMON_DEMON;
#endif

			break;
	}

	return activate;
}

static int random_major_theme_armor(object_type *o_ptr, int level)
{
	int activate = 0;

	int i;

	switch (randint1(10))
	{
		case 1:
			o_ptr->flags2 |= TR2_RES_ACID;
			o_ptr->flags2 |= TR2_RES_ELEC;
			o_ptr->flags2 |= TR2_RES_FIRE;
			o_ptr->flags2 |= TR2_RES_COLD;

			if (one_in_(3))
				o_ptr->flags2 |= TR2_RES_POIS;

			break;

		case 2:
			o_ptr->flags2 |= TR2_SUST_STR;
			o_ptr->flags2 |= TR2_SUST_INT;
			o_ptr->flags2 |= TR2_SUST_WIS;
			o_ptr->flags2 |= TR2_SUST_DEX;
			o_ptr->flags2 |= TR2_SUST_CON;
			o_ptr->flags2 |= TR2_SUST_CHR;

			break;

		case 3:
			/* Might */
			o_ptr->flags1 |= TR1_STR;
			o_ptr->flags2 |= TR2_SUST_STR;
			o_ptr->flags1 |= TR1_DEX;
			o_ptr->flags2 |= TR2_SUST_DEX;
			o_ptr->flags1 |= TR1_CON;
			o_ptr->flags2 |= TR2_SUST_CON;

			break;

		case 4:
			/* Mental */
			o_ptr->flags1 |= TR1_INT;
			o_ptr->flags2 |= TR2_SUST_INT;
			o_ptr->flags1 |= TR1_WIS;
			o_ptr->flags2 |= TR2_SUST_WIS;
			if (one_in_(3))
				o_ptr->flags1 |= TR1_SP;
			
			break;

		case 5:
			/* Lohengrin */
			o_ptr->flags1 |= TR1_STEALTH;
			o_ptr->flags1 |= TR1_INT;
			o_ptr->flags1 |= TR1_WIS;
			o_ptr->flags3 |= TR3_SEE_INVIS;

			break;

		case 6:
		case 7:
		case 8:
			/* Several high resists */
			for (i = randint1(3) + 1; i > 0; --i)
				random_resistance(o_ptr, rand_range(17, 38));

			break;

		case 9:
			/* Mixed stat boosts */
			for (i = 3; i > 0; --i)
			{
				switch (randint1(6))
				{
					case 1: o_ptr->flags1 |= TR1_STR; break;
					case 2: o_ptr->flags1 |= TR1_INT; break;
					case 3: o_ptr->flags1 |= TR1_WIS; break;
					case 4: o_ptr->flags1 |= TR1_DEX; break;
					case 5: o_ptr->flags1 |= TR1_CON; break;
					case 6: o_ptr->flags1 |= TR1_CHR; break;
				}
			}

			break;

		case 10:
			/* Thranduil */
			o_ptr->flags1 |= TR1_INT;
			o_ptr->flags1 |= TR1_WIS;
			o_ptr->flags2 |= TR2_RES_BLIND;
			if (o_ptr->tval == TV_HELM || o_ptr->tval == TV_CROWN)
				o_ptr->flags3 |= TR3_TELEPATHY;

			break;
	}

	return activate;
}

static void curse_artifact(object_type *o_ptr)
{
	int i;
	
	if (o_ptr->pval) o_ptr->pval = 0 - (o_ptr->pval + randint1(4));
	if (o_ptr->to_a) o_ptr->to_a = 0 - (o_ptr->to_a + randint1(4));
	if (o_ptr->to_h) o_ptr->to_h = 0 - (o_ptr->to_h + randint1(4));
	if (o_ptr->to_d) o_ptr->to_d = 0 - (o_ptr->to_d + randint1(4));

	o_ptr->flags3 |= (TR3_CURSED);

	for (i = rand_range(2, 5); i > 0; --i)
		random_curse(o_ptr, TRUE);
}


bool create_artifact(object_type *o_ptr, int level, bool a_scroll)
{
	char new_name[1024];
	int powers = rand_range(2, 6);
	int power_level;
	s32b total_flags, target_flags;
	bool a_cursed = FALSE;
	int i;
	int given = 0;

	/* No activation yet */
	o_ptr->activate = 0;

	new_name[0] = 0;

	if (!a_scroll && one_in_(A_CURSED))
		a_cursed = TRUE;

	while (one_in_(powers + 1))
		powers++;

#if 0
	if (!a_cursed && one_in_(WEIRD_LUCK))
		powers *= 2;
#endif

	if (a_cursed) powers /= 2;

	target_flags = 0;
	for (i = 0; i < powers; i++)
		target_flags += rand_range(10, 50) * (level + 5);

	/* Sometimes select a major theme - or two */
	while (o_ptr->tval < TV_LITE && randint1(powers) > 3)
	{
		int act;
		
		if (o_ptr->tval < TV_BOOTS)
		{
			act = random_major_theme_weapon(o_ptr, level);
			o_ptr->to_h += rand_range(5, 15);
			o_ptr->to_d += rand_range(5, 15);
		}
		else
		{
			act = random_major_theme_armor(o_ptr, level);
			o_ptr->to_a += rand_range(5, 15);
		}
		powers -= 3;
	}

	/* Possibly apply a power typical for the item's slot */
	if (one_in_(3))
	{
		switch (o_ptr->tval)
		{
			case TV_BOOTS:
				if (one_in_(WEIRD_LUCK))
					o_ptr->flags1 |= TR1_SPEED;
				else if (one_in_(2))
					o_ptr->flags2 |= TR2_FREE_ACT;
				else
					o_ptr->flags3 |= TR3_FEATHER;
				powers--;
				given++;
				break;

			case TV_GLOVES:
				if (one_in_(2))
					o_ptr->flags2 |= TR2_FREE_ACT;
				else
					o_ptr->flags1 |= TR1_DEX;
				powers--;
				given++;
				break;

			case TV_HELM:
			case TV_CROWN:
				if (one_in_(3))
					o_ptr->flags3 |= TR3_TELEPATHY;
				else if (one_in_(2))
					o_ptr->flags3 |= TR3_SEE_INVIS;
				else
					o_ptr->flags1 |= TR1_INFRA;
				powers--;
				given++;
				break;
		}
	}

	/* Lights already have permanent light */
	if (o_ptr->tval == TV_LITE)
		given++;

	total_flags = flag_cost(o_ptr, 1);

	/* Main loop */
	while (total_flags < target_flags || given < 2)
	{
		int act = 0;
		
		switch (randint1(o_ptr->tval < TV_BOOTS ? 11 : 7))
		{
			case 1:  case 2:
				random_plus(o_ptr);
				break;
			case 3:  case 4:
				random_resistance(o_ptr, 0);
				break;
			case 5:
				random_misc(o_ptr);
				break;
			case 6:  case 7:
				act = random_minor_theme_armor(o_ptr, level);
				break;
			case 8:  case 9:
				random_slay(o_ptr);
				break;
			case 10:  case 11:
				act = random_minor_theme_weapon(o_ptr, level);
				break;
		}
		given++;
		
		total_flags = flag_cost(o_ptr, 1);
	}

	if (o_ptr->flags1 & (TR1_PVAL_MASK))
	{
		if (o_ptr->flags1 & TR1_BLOWS)
		{
			if (one_in_(100))
			{
				o_ptr->pval = 2;
			}
			else
			{
				o_ptr->pval = 1;
			}
		}
		else
		{
			i = randint1(100);

			if (i <= 35)
				o_ptr->pval = 1;
			else if (i <= 65)
				o_ptr->pval = 2;
			else if (i <= 85)
				o_ptr->pval = 3;
			else if (i <= 99)
				o_ptr->pval = 4;
			else
				o_ptr->pval = 5;
		}
	}
	else
		o_ptr->pval = 0;

	/* give it some plusses... */
	if (o_ptr->tval >= TV_BOOTS && o_ptr->tval < TV_LITE)
		o_ptr->to_a += randint1(o_ptr->to_a > 19 ? 1 : 20 - o_ptr->to_a);
	else if (o_ptr->tval < TV_BOOTS)
	{
		o_ptr->to_h += randint1(o_ptr->to_h > 19 ? 1 : 20 - o_ptr->to_h);
		o_ptr->to_d += randint1(o_ptr->to_d > 19 ? 1 : 20 - o_ptr->to_d);
	}

	/* Just to be sure */
	o_ptr->flags3 |= (TR3_IGNORE_ACID | TR3_IGNORE_ELEC |
					  TR3_IGNORE_FIRE | TR3_IGNORE_COLD);

	/* Possibly add some curses ... */
	total_flags = flag_cost(o_ptr, o_ptr->pval);
	if (one_in_(13))
	{
		random_curse(o_ptr, FALSE);
		total_flags = flag_cost(o_ptr, o_ptr->pval);
	}

	/* Penalize too-good artifacts */
	if (!a_scroll)
	{
		if (total_flags >= target_flags * 2 && total_flags >= 5000 &&
				one_in_(2))
		{
			random_curse(o_ptr, FALSE);
			total_flags = flag_cost(o_ptr, o_ptr->pval);
		}
		if (total_flags >= target_flags * 3 && total_flags >= 10000 &&
				!one_in_(WEIRD_LUCK))
		{
			random_curse(o_ptr, TRUE);
			total_flags = flag_cost(o_ptr, o_ptr->pval);
		}
	}

	if (cheat_peek) msgf("%ld", total_flags);

	if (a_cursed) curse_artifact(o_ptr);

	/* Check if it has an activation */
	if (!a_cursed && one_in_((o_ptr->tval >= TV_BOOTS)
							 ? ACTIVATION_CHANCE * 2 : ACTIVATION_CHANCE))
	{
		int activation_level = level + randint0(10);

		if (one_in_(5))
			activation_level = activation_level * 3 / 2;

		if (activation_level > 100) activation_level = 100;

		/* 
		 * Get a random activation
		 */
		random_activation_power(o_ptr, activation_level);
	}

	if (o_ptr->dd && o_ptr->ds)
	{
		if (one_in_(10L * o_ptr->dd * o_ptr->ds))
		{
			o_ptr->ds += (o_ptr->ds * randint1(5)) / 5;
		}
	}

	if (o_ptr->tval >= TV_BOOTS)
	{
		if (a_cursed) power_level = 0;
		else if (total_flags < 10000) power_level = 1;
		else if (total_flags < 20000) power_level = 2;
		else
			power_level = 3;
	}

	else
	{
		if (a_cursed) power_level = 0;
		else if (total_flags < 15000) power_level = 1;
		else if (total_flags < 30000) power_level = 2;
		else
			power_level = 3;
	}

	if (a_scroll)
	{
		char dummy_name[80];
		dummy_name[0] = 0;
		(void)identify_fully_aux(o_ptr);
		o_ptr->info |= OB_STOREB;

		if (!(get_string(dummy_name, 80,
        				 "What do you want to call the artifact? ")))
		{
			get_random_name(new_name, o_ptr->tval, power_level);
		}
		else
		{
			strnfmt(new_name, 1024, "'%s'", dummy_name);
		}
		/* Identify it fully */
		object_aware(o_ptr);
		object_known(o_ptr);
		object_mental(o_ptr);

		/* Save all the known flags */
		o_ptr->kn_flags1 = o_ptr->flags1;
		o_ptr->kn_flags2 = o_ptr->flags2;
		o_ptr->kn_flags3 = o_ptr->flags3;
		o_ptr->kn_flags4 = o_ptr->flags4;
	}
	else
	{
		get_random_name(new_name, o_ptr->tval, power_level);
	}

	chg_virtue(V_INDIVIDUALISM, 2);
	chg_virtue(V_ENCHANT, 5);

	/* Save the inscription */
	o_ptr->xtra_name = quark_add(new_name);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Make the object an artifact */
	o_ptr->flags3 |= TR3_INSTA_ART;

	/* Set the cost */
	o_ptr->cost = k_info[o_ptr->k_idx].cost + flag_cost(o_ptr, o_ptr->pval);

	return TRUE;
}


/*
 * Create the artifact of the specified number
 */
void create_named_art(int a_idx, int x, int y)
{
	object_type *q_ptr;
	int i;

	artifact_type *a_ptr = &a_info[a_idx];

	/* Ignore "empty" artifacts */
	if (!a_ptr->name) return;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	if (!i) return;

	/* Create the artifact */
	q_ptr = object_prep(i);

	/* Set the activation */
	q_ptr->activate = a_idx;
	
	/* Add any special scripts */
	for (i = 0; i < MAX_TRIGGER; i++)
	{
		if (a_ptr->trigger[i])
			q_ptr->trigger[i] = quark_add(a_text + a_ptr->trigger[i]);
	}

	/* Do not make another one */
	a_ptr->cur_num = 1;

	/* Save the artifact flags */
	q_ptr->flags1 |= a_ptr->flags1;
	q_ptr->flags2 |= a_ptr->flags2;
	q_ptr->flags3 |= a_ptr->flags3;
	q_ptr->flags4 |= a_ptr->flags4;

	/* Extract the fields */
	q_ptr->pval = a_ptr->pval;
	q_ptr->ac = a_ptr->ac;
	q_ptr->dd = a_ptr->dd;
	q_ptr->ds = a_ptr->ds;
	q_ptr->to_a = a_ptr->to_a;
	q_ptr->to_h = a_ptr->to_h;
	q_ptr->to_d = a_ptr->to_d;
	q_ptr->weight = a_ptr->weight;

	/* Save the inscription */
	q_ptr->xtra_name = quark_add(a_name + a_ptr->name);

	/* Apply special scripts */
	apply_object_trigger(TRIGGER_MAKE, q_ptr, "i", "lev", a_ptr->level);

	if (!a_ptr->cost)
	{
		/* Hack -- "worthless" artifacts */
		q_ptr->cost = 0L;
	}
	else
	{
		/* Hack - use the artifact price */
		q_ptr->cost = k_info[q_ptr->k_idx].cost + a_ptr->cost;
	}

	/* Drop the artifact from heaven */
	drop_near(q_ptr, -1, x, y);
}
