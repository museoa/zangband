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
	switch (randint1(evil ? 26 : 16))
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
	"acid", "elec", "fire", "cold", "poison",
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

static void apply_activation_power(object_type *o_ptr, int level)
{
	cptr text = "";
	int charge_min = 10;
	int charge_max = 0;
	int dice = 0;
	int sides = 1;
	int radius = 0;
	int pp = 0;	/* Charge time * level */
	bool aimed = TRUE;
	
	int len;

	char text_buf[256];
	char buf[1024];
	char effect[256] = "";
	char desc[256] = "";

	int rlev = level * rand_range(50, 150) / 100;
	if (rlev < 1) rlev = 1;

	text = activation_text[randint0(NUM_ELEMENTS(activation_text))];

	if (randint0(100) < 40)
	{
		static char buf1[40];
		int element;

		/* Pick a low or high element */
		if (level <= randint1(60))
			element = randint0(5);
		else
			element = randint0(NUM_ELEMENTS(element_list));

		/* Describe the visual */
		strnfmt(buf1, 256, "The %%v %s %s...", glow_desc[randint0(NUM_ELEMENTS(glow_desc))], element_colors[element]);
		text = buf1;

		switch (randint1(10))
		{
		/* Breathe */
		case 1:
			strnfmt(buf1, 256, "You breathe %s.", element_names[element]);

			/* Dice, radius, and charge time */
			dice = rlev * 5;
			radius = 2 + dice / rand_range(100, 200);
			pp = dice * 5 * radius;

			/* Create the lua */
			strnfmt(desc, 256, "breathe %s (%i, rad. %i)", element_names[element], dice, radius);
			strnfmt(effect, 256, "fire_ball(\"%s\", dir, %i, %i)", element_list[element], dice, radius);

			break;

		/* Emit a blast */
		case 2:
			strnfmt(buf1, 256, "The %%v emits a blast of %s...", element_names[element]);

			/* Dice, radius, and charge time */
			dice = rlev * 10;
			radius = 2 + dice / rand_range(50, 100);
			pp = dice * radius;
			
			aimed = FALSE;

			/* Create the lua */
			strnfmt(desc, 256, "%s blast (%i, rad. %i)", element_names[element], dice, radius);
			strnfmt(effect, 256, "fire_ball(\"%s\", 0, %i, %i)", element_list[element], dice, radius);

			break;

		/* Fire a ball */
		case 3:
		case 4:
		case 5:
			/* Dice, radius, and charge time */
			dice = rlev * 5;
			radius = (level >= rand_range(40, 80) ? 3 : 2);
			pp = dice * 5 * radius;

			/* Create the lua */
			strnfmt(desc, 256, "%s%s ball (%i)", radius > 2 ? "large " : "", element_names[element], dice);
			strnfmt(effect, 256, "fire_ball(\"%s\", dir, %i, %i)", element_list[element], dice, radius);

			break;

		/* Fire a bolt */
		default:
			/* Dice and charge time */
			dice = rlev / 2;
			sides = rand_range(5, 8);
			pp = dice * sides * 2;

			/* Create the lua */
			strnfmt(desc, 256, "%s bolt (%id%i)", element_names[element], dice, sides);
			strnfmt(effect, 256, "fire_bolt(\"%s\", dir, damroll(%i, %i))", element_list[element], dice, sides);

			break;
		}
	}
	else switch (randint1(84))
	{
		case 1: case 2: case 3:
			text = "A line of sunlight appears.";
			strcpy(desc, "beam of sunlight");
			strcpy(effect, "lite_line(dir)");
			charge_min = rand_range(50, 100) / level;
			if (charge_min > 10) charge_min = 10;
			if (charge_min < 2) charge_min = 2;
			charge_max = charge_min;
			break;

		case 4: case 5: case 6:
			text = "The %v glows extremely brightly...";
			charge_max = charge_min = 2;
			dice = randint1(3) + level / 10;
			sides = 6;
			strnfmt(desc, 256, "magic missile (%id%i)", dice, sides);
			strnfmt(effect, 256, "fire_bolt(GF_MISSILE, dir, damroll(%i, %i))", dice, sides);
			break;

		case 7: case 8:
			text = "The %v throbs deep green...";
			dice = 10 + rlev;
			pp = dice * 5;
			strnfmt(desc, 256, "stinking cloud (%i)", dice);
			strnfmt(effect, 256, "fire_ball(GF_POIS, dir, %i, 2)", dice);
			break;

		case 9:
			text = "The %v glows black...";
			dice = 5 * rlev;
			pp = dice * 10;
			strnfmt(desc, 256, "drain life (%i)", dice);
			strnfmt(effect, 256, "drain_life(dir, %i)", dice);
			break;

		case 10:
			text = "The %v throbs red...";
			dice = 10 * rlev;
			pp = dice * 20;
			strnfmt(desc, 256, "vampiric drain (%i)", dice);
			strnfmt(desc, 256, "drain_gain_life(dir, %i)", dice);
			break;

		case 11:
			text = "The %v grows magical spikes...";
			if (randint0(100) < 40)
			{
				dice = 5 * rlev;
				pp = dice * 3;
				strnfmt(desc, 256, "arrows (%i)", dice);
				strnfmt(effect, 256, "fire_bolt(GF_ARROW, dir, %i)", dice);
			}
			else
			{
				dice = 1 + rlev / 3;
				sides = 5 * rand_range(5, 10);
				pp = dice * sides * 2;
				strnfmt(desc, 256, "arrows (%id%i)", dice, sides);
				strnfmt(effect, 256, "fire_bolt(GF_ARROW, dir, damroll(%i, %i))", dice, sides);
			}
			break;

		case 12:
			text = "The %v emits a blast of air...";
			strcpy(desc, "whirlwind attack");
			strcpy(effect, "whirlwind_attack()");
			aimed = FALSE;
			pp = 20000;
			break;

		case 13:
			text = "The %v glows in scintillating colours...";
			strcpy(desc, "call chaos");
			strcpy(effect, "call_chaos()");
			aimed = FALSE;
			pp = 25000;
			break;

		case 14:
			text = "You launch a rocket!";
			dice = 100 + 5 * rlev;
			pp = dice * 20;
			strnfmt(desc, 256, "launch rocket (%i)", dice);
			strnfmt(effect, 256, "fire_ball(GF_ROCKET, dir, %i, 2)", dice);
			break;

		case 15: case 16:
			text = "The %v floods the area with goodness...";
			aimed = FALSE;
			dice = 5 * rlev;
			pp = dice * 50;
			strnfmt(desc, 256, "dispel evil (%i)", dice);
			strnfmt(effect, 256, "dispel_evil(%i)", dice);
			break;

		case 17:
			text = "The %v floods the area with evil...";
			aimed = FALSE;
			dice = 5 * rlev;
			pp = dice * 50;
			strnfmt(desc, 256, "dispel good (%i)", dice);
			strnfmt(effect, 256, "dispel_good(%i)", dice);
			break;

		case 18:
			/* Note that this is more powerful than a normal breath activation */
			text = "You breathe the elements.";
			dice = 10 * rlev;
			radius = 2 + dice / rand_range(100, 200);
			pp = dice * 5 * radius;
			strnfmt(desc, 256, "breathe the elements (%i, rad. %i)", dice, radius);
			strnfmt(effect, 256, "fire_ball(GF_MISSILE, dir, %i, %i)", dice, radius);
			break;

		case 19: case 20:
			strcpy(desc, "confuse monster");
			strcpy(effect, "confuse_monster(dir, 50)");
			pp = 250;
			break;

		case 21:
			strcpy(desc, "sleep nearby monsters");
			strcpy(effect, "sleep_monsters_touch()");
			aimed = FALSE;
			pp = 1000;
			break;

		case 22:
			text = "The %v vibrates...";
			aimed = FALSE;
			radius = 5 + rlev / 4;
			pp = radius * 100;
			strnfmt(desc, 256, "earthquake (rad. %i)", radius);
			strnfmt(effect, 256, "earthquake(px, py, %i)", radius);
			break;

		case 23:
			text = "The %v emits a loud blast...";
			dice = rlev / 2;
			pp = 1500 + 25 * rlev;
			strcpy(desc, "terror");
			strnfmt(effect, 256, "turn_monsters(%i + player.lev)", dice);
			aimed = FALSE;
			break;

		case 24: case 25:
			strcpy(desc, "teleport away");
			strcpy(effect, "fire_beam(GF_AWAY_ALL, dir, plev)");
			pp = 5000;
			break;
			
		case 26:
			text = "The power of the artifact banishes evil!";
			strcpy(desc, "banish evil");
			strcpy(effect, "banish_evil(200)");
			aimed = FALSE;
			pp = 10000;
			break;

		case 27:
			strcpy(desc, "genocide");
			strcpy(effect, "genocide(TRUE)");
			aimed = FALSE;
			pp = 10000;
			break;

		case 28:
			strcpy(desc, "mass genocide");
			strcpy(effect, "mass_genocide(TRUE)");
			aimed = FALSE;
			pp = 25000;
			break;

		case 29:
			strcpy(desc, "charm animal");
			strcpy(effect, "charm_animal(dir, plev)");
			pp = 5000;
			break;

		case 30:
			strcpy(desc, "enslave undead");
			strcpy(effect, "control_one_undead(dir, plev)");
			pp = 7500;
			break;

		case 31:
			strcpy(desc, "charm monster");
			strcpy(effect, "charm_monster(dir, plev)");
			pp = 10000;
			break;

		case 32:
			strcpy(desc, "animal friendship");
			strcpy(effect, "charm_animals(plev * 2)");
			aimed = FALSE;
			pp = 10000;
			break;

		case 33:
			strcpy(desc, "mass charm");
			strcpy(effect, "charm_monsters(plev * 2)");
			aimed = FALSE;
			pp = 25000;
			break;

		case 34:
			text = "You summon a beast.";
			strcpy(desc, "summon animal");
			strcpy(effect, "summon_specific(-1, px, py, plev, SUMMON_ANIMAL_RANGER, TRUE, TRUE, TRUE)");
			aimed = FALSE;
			pp = 10000;
			break;

		case 35:
			text = "You summon a phantasmal servant.";
			strcpy(desc, "summon phantasmal servant");
			strcpy(effect, "summon_specific(-1, px, py, plev, SUMMON_PHANTOM, TRUE, TRUE, TRUE)");
			aimed = FALSE;
			pp = 10000;
			break;

		case 36:
			text = "You summon an elemental.";
			strcpy(desc, "summon elemental");
			strcpy(effect, "summon_unsafe(SUMMON_ELEMENTAL, \"You fail to control it!\")");
			aimed = FALSE;
			pp = 25000;
			break;

		case 37:
			/* XXX XXX XXX summon_unsafe() doesn't seem to be implemented anywhere */
			text = "The area fills with the stench of sulphur and brimstone.";
			strcpy(desc, "summon demon");
			strcpy(effect, "summon_unsafe(SUMMON_DEMON, \"'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'\")");
			aimed = FALSE;
			charge_min = 666;
			charge_max = 333 * rand_range(2, 5);
			break;

		case 38:
			text = "Ancient, long-dead forms rise from the ground.";
			if (level < rand_range(60, 100))
			{
				strcpy(desc, "summon undead");
				strcpy(effect, "summon_unsafe(SUMMON_UNDEAD, \"'The dead arise... to punish you for disturbing them!'\")");
			}
			else
			{
				strcpy(desc, "summon greater undead");
				strcpy(effect, "summon_unsafe(SUMMON_HI_UNDEAD, \"'The dead arise... to punish you for disturbing them!'\")");
			}
			aimed = FALSE;
			charge_min = 666;
			charge_max = 333 * rand_range(2, 5);
			break;

		case 39:
			aimed = FALSE;
			dice = 1 + rlev / 3;
			sides = rand_range(4, 8);
			pp = dice * sides * 5;
			strnfmt(desc, 256, "remove fear & heal (%id%i)", dice, sides);
			strnfmt(effect, 256, "clear_afraid(); hp_player(damroll(%i, %i))", dice, sides);
			break;

		case 40: case 41: case 42:
			aimed = FALSE;
			dice = rlev;
			sides = rand_range(4, 8);
			pp = dice * sides * 5;
			strnfmt(desc, 256, "cure wounds & heal (%id%i)", dice, sides);
			strnfmt(effect, 256, "hp_player(damroll(%i, %i)); inc_cut(-%i)", dice, sides, 5 * rlev);
			break;

		case 43:
			strcpy(desc, "remove fear & cure poison");
			strcpy(effect, "clear_afraid(); clear_poisoned()");
			aimed = FALSE;
			pp = 50;
			break;

		case 44:
			strcpy(desc, "restore life levels");
			strcpy(effect, "restore_level()");
			aimed = FALSE;
			pp = 5000;
			break;

		case 45:
			strcpy(desc, "restore stats");
			strcpy(effect, "do_res_stat(A_STR); do_res_stat(A_INT); "
				"do_res_stat(A_WIS); do_res_stat(A_DEX); "
				"do_res_stat(A_CON); do_res_stat(A_CHR); ");
			aimed = FALSE;
			pp = 15000;
			break;

		case 46:
			strcpy(desc, "restore stats and life levels");
			strcpy(effect, "do_res_stat(A_STR); do_res_stat(A_INT); "
				"do_res_stat(A_WIS); do_res_stat(A_DEX); "
				"do_res_stat(A_CON); do_res_stat(A_CHR); "
				"restore_level()");
			aimed = FALSE;
			pp = 25000;
			break;

		case 47: case 48:
			aimed = FALSE;
			dice = 10 * rlev;
			pp = dice * 10;
			strnfmt(desc, 256, "cure wounds & heal (%i)", dice);
			strnfmt(effect, 256, "hp_player(%i); clear_cut()", dice);
			break;

		case 49:
			text = "The %v enters your thoughts...";
			aimed = FALSE;
			dice = rlev;
			pp = dice * 200;
			strnfmt(desc, 256, "telepathy (%i-%i turns)", dice, dice * 2);
			strnfmt(effect, 256, "inc_time_esp(rand_range(%i, %i))", dice, dice * 2);
			break;

		case 50:
			text = "The %v drives you into a berserk rage...";
			aimed = FALSE;
			dice = rlev;
			pp = dice * 200;
			strnfmt(desc, 256, "heroism and berserk (%i-%i turns)", dice, dice * 2);
			strnfmt(effect, 256, "inc_shero(rand_range(%i, %i)); inc_blessed(rand_range(%i, %i))",
				dice, dice * 2, dice, dice * 2);
			break;
			
		case 51:
			aimed = FALSE;
			dice = rlev * 2;
			pp = dice * 100;
			strnfmt(desc, 256, "protection from evil (%i-%i turns)", dice, dice * 2);
			strnfmt(effect, 256, "inc_protevil(rand_range(%i, %i))", dice, dice * 2);
			break;

		case 52:
			text = "The %v glows many colours...";
			aimed = FALSE;
			dice = rlev;
			pp = dice * 200;
			strnfmt(desc, 256, "resist elements (%i-%i turns)", dice, dice * 2);
			strnfmt(effect, 256, "local turns = rand_range(%i, %i); "
				"inc_oppose_acid(turns); inc_oppose_elec(turns); "
				"inc_oppose_fire(turns); inc_oppose_cold(turns); "
				"inc_oppose_pois(turns)", dice, dice * 2);
			break;

		case 53: case 54:
			aimed = FALSE;
			dice = rlev;
			pp = dice * 200;
			strnfmt(desc, 256, "speed (%i-%i turns)", dice, dice * 2);
			strnfmt(effect, 256, "inc_fast(rand_range(%i, %i))", dice, dice * 2);
			break;

		case 55:
			text = "The %v fades out...";
			aimed = FALSE;
			dice = 1 + rlev / 3;
			pp = dice * 2000;
			strnfmt(desc, 256, "wraith form (%i-%i turns)", dice, dice * 2);
			strnfmt(effect, 256, "inc_wraith_form(rand_range(%i, %i))", dice, dice * 2);
			break;

		case 56:
			text = "The %v fires a beam of bright light at you...";
			aimed = FALSE;
			dice = 1 + rlev / 3;
			pp = dice * 2000;
			strnfmt(desc, 256, "invulnerability (%i-%i turns)", dice, dice * 2);
			strnfmt(effect, 256, "inc_invuln(rand_range(%i, %i))", dice, dice * 2);
			break;

		case 57: case 58: case 59: case 60:
			text = "The %v twists space around you...";
			strcpy(desc, "teleport (100)");
			strcpy(effect, "teleport_player(100)");
			aimed = FALSE;
			pp = 1000;
			break;

		case 61: case 62: case 63:
			text = "The %v wells with clear light...";
			aimed = FALSE;
			dice = 1 + rlev / 5;
			if (dice > 5) dice = 5;
			sides = 5 * rand_range(2, 5);
			pp = dice * sides * 2 + 20;
			strnfmt(desc, 256, "light area (%id%i)", dice, sides);
			strnfmt(effect, 256, "lite_area(damroll(%i, %i), 3)", dice, sides);
			break;

		case 64:
			text = "The %v shines brightly...";
			aimed = FALSE;
			dice = 1 + rlev / 5;
			if (dice > 5) dice = 5;
			sides = 5 * rand_range(2, 5);
			pp = dice * sides * 2 + 200;
			strnfmt(desc, 256, "map & light area (%id%i)", dice, sides);
			strnfmt(effect, 256, "map_area(); lite_area(damroll(%i, %i), 3)", dice, sides);
			break;
			
		case 65:
			text = "An image forms in your mind...";
			strcpy(desc, "detection");
			strcpy(effect, "detect_all()");
			aimed = FALSE;
			pp = 1000;
			break;

		case 66:
			strcpy(desc, "detection, probing and identify true");
			strcpy(effect, "detect_all(); probing(); identify_fully()");
			aimed = FALSE;
			pp = 50000;
			break;

		case 67:
			strcpy(desc, "identify true");
			strcpy(effect, "identify_fully()");
			aimed = FALSE;
			pp = 25000;
			break;

		case 68: case 69: case 70: case 71: case 72:
			strcpy(desc, "identify");
			strcpy(effect, "if not ident_spell() then return end");
			aimed = FALSE;
			pp = 250;
			break;

		case 73:
			text = "The %v glows bright red...";
			strcpy(desc, "explosive rune");
			strcpy(effect, "explosive_rune()");
			aimed = FALSE;
			pp = 5000;
			break;

		case 74:
			strcpy(desc, "rune of protection");
			strcpy(effect, "warding_glyph()");
			aimed = FALSE;
			pp = 10000;
			break;

		case 75: case 76:
			strcpy(desc, "satisfy hunger");
			strcpy(effect, "set_food(PY_FOOD_MAX - 1)");
			aimed = FALSE;
			pp = 25000;
			break;

		case 77:
			strcpy(desc, "destroy doors");
			strcpy(effect, "destroy_doors_touch()");
			aimed = FALSE;
			pp = 250;
			break;

		case 78: case 79:
			strcpy(desc, "stone to mud");
			strcpy(effect, "wall_to_mud(dir)");
			pp = 100;
			break;

		case 80:
			strcpy(desc, "recharging");
			strcpy(effect, "recharge(130)");
			aimed = FALSE;
			pp = 1000;
			break;

		case 81:
			strcpy(desc, "alchemy");
			strcpy(effect, "alchemy()");
			aimed = FALSE;
			pp = 10000;
			break;

		case 82:
			text = "You open a dimensional gate. Choose a destination.";
			strcpy(desc, "dimension door");
			strcpy(effect, "if not dimension_door() return end");
			aimed = FALSE;
			pp = 1000;
			break;

		case 83: case 84:
			strcpy(desc, "word of recall");
			strcpy(effect, "word_of_recall()");
			aimed = FALSE;
			pp = 5000;
			break;
	}

	if (pp) 
	{
		charge_min = pp / level;

		/* Round to nice numbers */
		if (charge_min >= 1000)
			charge_min -= charge_min % 100;
		else if (charge_min >= 500)
			charge_min -= charge_min % 50;
		else if (charge_min >= 100)
			charge_min -= charge_min % 10;
		else if (charge_min >= 50)
			charge_min -= charge_min % 5;
	}

	if (charge_min < 1) charge_min = 1;
	if (charge_min > 5000) charge_min = 5000;
	if (!charge_max) charge_max = charge_min * 2;

	if (sides > 1 && dice < 1) dice = 1;
	
	/* Get the basic name of the object in the description */
	strnfmt(text_buf, 256, text, OBJECT_FMT(o_ptr, FALSE, 0));
	
	/* Construct the lua script */
	len = strnfmt(buf, 1024, "msgf(\"%s\"); ", text_buf);
	
	if (aimed) strnfcat(buf, 1024, &len, 
				"local success; local dir; "
				"success, dir = get_aim_dir(); "
				"if not success then return; end; ");

	strnfcat(buf, 1024, &len, "%s; object.timeout = ", effect);

	if (charge_min != charge_max)
		strnfcat(buf, 1024, &len, "rand_range(%i, %i)", charge_min, charge_max);
	else
		strnfcat(buf, 1024, &len, "%i", charge_min);
	
	/* Usage script */
	o_ptr->trigger[TRIGGER_USE] = quark_add(buf);
	
	
	len = strnfmt(buf, 1024, "return \"%s", desc);
	
	if (charge_min != charge_max)
		strnfcat(buf, 1024, &len, " every %i-%i turns \"", charge_min, charge_max);
	else
		strnfcat(buf, 1024, &len, " every %i turns \"", charge_min);
	
	/* Description script */
	o_ptr->trigger[TRIGGER_DESC] = quark_fmt(buf);

	o_ptr->flags3 |= TR3_ACTIVATE;
	o_ptr->timeout = 0;
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


static int random_minor_theme_weapon(object_type *o_ptr)
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

#if 0
			if (one_in_(2))
				activate = ACT_BO_ACID_1;
#endif
			
			break;

		case 6:
		case 7:
			o_ptr->flags1 |= TR1_BRAND_ELEC;
			o_ptr->flags2 |= TR2_RES_ELEC;

#if 0
			if (one_in_(4))
				activate = ACT_BO_ELEC_1;
			else if (one_in_(5))
				activate = ACT_BA_ELEC_2;
			else if (one_in_(5))
				activate = ACT_BA_ELEC_3;
#endif

			break;

		case 8:
		case 9:
		case 10:
			o_ptr->flags1 |= TR1_BRAND_FIRE;
			o_ptr->flags2 |= TR2_RES_FIRE;
			o_ptr->flags3 |= TR3_LITE;

#if 0
			if (one_in_(5))
				activate = ACT_BO_FIRE_1;
			else if (one_in_(5))
				activate = ACT_BA_FIRE_1;
			else if (one_in_(5))
				activate = ACT_BA_FIRE_2;
#endif

			break;

		case 11:
		case 12:
		case 13:
			o_ptr->flags1 |= TR1_BRAND_COLD;
			o_ptr->flags2 |= TR2_RES_COLD;

#if 0
			if (one_in_(6))
				activate = ACT_BO_COLD_1;
			else if (one_in_(6))
				activate = ACT_BA_COLD_1;
			else if (one_in_(6))
				activate = ACT_BA_COLD_2;
#endif

			break;

		case 14:
		case 15:	
			o_ptr->flags1 |= TR1_BRAND_POIS;
			o_ptr->flags2 |= TR2_RES_POIS;

#if 0
			if (one_in_(3))
				activate = ACT_BA_POIS_1;
#endif

			break;

		case 16:
		case 17:
			o_ptr->flags1 |= TR1_CHAOTIC;
			o_ptr->flags2 |= TR2_RES_CHAOS;
			if (one_in_(3))
				o_ptr->flags4 |= TR4_PATRON;

#if 0
			if (one_in_(11))
				activate = ACT_CALL_CHAOS;
#endif
			
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

#if 0
			if (one_in_(6))
				activate = ACT_VAMPIRE_1;
			else if (one_in_(9))
				activate = ACT_VAMPIRE_2;
#endif

			break;

		case 36:
			o_ptr->flags2 |= TR2_HOLD_LIFE;

#if 0
			if (!one_in_(3))
				activate = ACT_DRAIN_1;
			else
				activate = ACT_DRAIN_2;
#endif

			break;

		case 37:
			o_ptr->to_h += rand_range(5, 15);
			o_ptr->to_d += rand_range(5, 15);

#if 0
			activate = ACT_WHIRLWIND;
#endif

			break;

		case 38:
			o_ptr->flags1 |= TR1_SLAY_ANIMAL;

#if 0
			if (!one_in_(3))
				activate = ACT_CHARM_ANIMAL;
			else
				activate = ACT_CHARM_ANIMALS;
#endif

			break;

		case 39:
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;

#if 0
			activate = ACT_CHARM_UNDEAD;
#endif

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

static int random_major_theme_weapon(object_type *o_ptr)
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

#if 0
		if (one_in_(4))
			activate = ACT_DISP_EVIL;
		else if (one_in_(7))
			activate = ACT_CURE_700;
#endif
		
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

#if 0
		if (one_in_(3))
			activate = ACT_TELEPORT_2;
		else if (one_in_(3))
			activate = ACT_TELEPORT_1;
#endif

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

static int random_minor_theme_armor(object_type *o_ptr)
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

#if 0
			if (one_in_(8))
				activate = ACT_BERSERK;
#endif
			
			break;

		case 6:
		case 7:
			o_ptr->flags1 |= TR1_INT;
			o_ptr->flags2 |= TR2_SUST_INT;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_FEATHER;

#if 0			
			if (one_in_(8))
				activate = ACT_ID_PLAIN;
#endif
			
			break;

		case 8:
		case 9:
			o_ptr->flags1 |= TR1_WIS;
			o_ptr->flags2 |= TR2_SUST_WIS;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_SEE_INVIS;

#if 0
			if (one_in_(8))
				activate = ACT_DETECT_ALL;
#endif
			
			break;

		case 10:
		case 11:
			o_ptr->flags1 |= TR1_DEX;
			o_ptr->flags2 |= TR2_SUST_DEX;
			if (one_in_(3))
				o_ptr->flags2 |= TR2_FREE_ACT;

#if 0
			if (one_in_(8))
				activate = ACT_SPEED;
#endif
			
			break;

		case 12:
		case 13:
			o_ptr->flags1 |= TR1_CON;
			o_ptr->flags2 |= TR2_SUST_CON;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_REGEN;

#if 0
			if (one_in_(8))
				activate = ACT_SATIATE;
#endif
			
			break;

		case 14:
		case 15:
			o_ptr->flags1 |= TR1_CHR;
			o_ptr->flags2 |= TR2_SUST_CHR;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_LITE;

#if 0
			if (one_in_(8))
				activate = ACT_RECALL;
#endif
			
			break;

		case 16:
			o_ptr->flags3 |= TR3_LITE;
			o_ptr->flags2 |= TR2_RES_LITE;

#if 0
			if (one_in_(3))
				activate = ACT_LIGHT;
#endif

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

#if 0
			if (one_in_(3))
				activate = ACT_DISP_EVIL;
			else if (one_in_(2))
				activate = ACT_BANISH_EVIL;
			else
				activate = ACT_PROT_EVIL;
#endif

			break;

		case 24:
			o_ptr->flags1 |= TR1_STEALTH;

#if 0
			activate = ACT_SLEEP;
#endif

			break;

		case 25:
			o_ptr->flags1 |= TR1_WIS;

#if 0
			activate = ACT_ESP;
#endif

			break;

		case 26:
			o_ptr->flags2 |= TR2_RES_DARK;

#if 0
			activate = ACT_SUNLIGHT;
#endif

			break;

		case 27:
			o_ptr->flags2 |= TR2_RES_CHAOS;
			o_ptr->flags2 |= TR2_RES_CONF;

			break;

		case 28:
			o_ptr->flags2 |= TR2_RES_NETHER;
			o_ptr->flags2 |= TR2_HOLD_LIFE;

#if 0
			if (one_in_(6))
				activate = ACT_REST_LIFE;
#endif

			break;

		case 29:
			o_ptr->flags2 |= TR2_RES_SOUND;
			o_ptr->flags2 |= TR2_RES_SHARDS;

			break;

		case 30:
			o_ptr->flags2 |= TR2_RES_FEAR;

#if 0
			activate = ACT_TERROR;
#endif

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

static int random_major_theme_armor(object_type *o_ptr)
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

	o_ptr->flags3 |= (TR3_HEAVY_CURSE | TR3_CURSED);

	for (i = rand_range(2, 5); i > 0; --i)
		random_curse(o_ptr, TRUE);

	if ((p_ptr->rp.pclass != CLASS_WARRIOR) && one_in_(3))
		o_ptr->flags3 |= TR3_NO_MAGIC;
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
			act = random_major_theme_weapon(o_ptr);
			o_ptr->to_h += rand_range(5, 15);
			o_ptr->to_d += rand_range(5, 15);
		}
		else
		{
			act = random_major_theme_armor(o_ptr);
			o_ptr->to_a += rand_range(5, 15);
		}
		powers -= 3;
	}

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
				act = random_minor_theme_armor(o_ptr);
				break;
			case 8:  case 9:
				random_slay(o_ptr);
				break;
			case 10:  case 11:
				act = random_minor_theme_weapon(o_ptr);
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
			random_curse(o_ptr, (one_in_(3) ? TRUE : FALSE));
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
		apply_activation_power(o_ptr, activation_level);
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
