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
	switch (randint1(37))
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

static void apply_activation_power(object_type *o_ptr, int type, int level)
{
	cptr text = "";
	cptr desc = "";
	cptr effect = "";
	int charge_min = 10;
	int charge_max = 0;
	int dice = 0;
	int sides = 1;
	int radius = 0;
	bool aimed = TRUE;
	
	int len;

	char text_buf[256];
	char buf[1024];

	text = activation_text[randint0(NUM_ELEMENTS(activation_text))];

	switch (type)
	{
		case ACT_SUNLIGHT:
			text = "A line of sunlight appears.";
			desc = "beam of sunlight";
			effect = "lite_line(dir)";
			charge_min = rand_range(50, 100) / level;
			if (charge_min > 10) charge_min = 10;
			if (charge_min < 2) charge_min = 2;
			charge_max = charge_min;
			break;

		case ACT_BO_MISS_1:
			text = "The %v glows extremely brightly...";
			desc = "magic missile";
			effect = "fire_bolt(GF_MISSILE, dir, dam)";
			charge_max = charge_min = 2;
			dice = randint1(3) + level / 10;
			sides = 6;
			break;

		case ACT_BA_POIS_1:
			text = "The %v throbs deep green...";
			desc = "stinking cloud";
			effect = "fire_ball(GF_POIS, dir, dam, rad)";
			radius = (level >= 15 ? 3 : 2);
			charge_min = rand_range(4, 8);
			dice = 10 + 5 * (level / rand_range(6, 12));
			break;

		case ACT_BO_ELEC_1:
			text = "The %v is covered in sparks...";
			desc = "lightning bolt";
			effect = "fire_bolt(GF_ELEC, dir, dam)";
			charge_min = rand_range(4, 8);
			dice = 1 + level / rand_range(4, 6);
			sides = 8;
			break;

		case ACT_BO_ACID_1:
			text = "The %v is covered in acid...";
			desc = "acid bolt";
			effect = "fire_bolt(GF_ACID, dir, dam)";
			charge_min = rand_range(4, 8);
			dice = 1 + level / rand_range(4, 6);
			sides = 8;
			break;

		case ACT_BO_COLD_1:
			text = "The %v is covered in frost...";
			desc = "frost bolt";
			effect = "fire_bolt(GF_COLD, dir, dam)";
			charge_min = rand_range(4, 8);
			dice = 1 + level / rand_range(4, 6);
			sides = 8;
			break;

		case ACT_BO_FIRE_1:
			text = "The %v is covered in fire...";
			desc = "fire bolt";
			effect = "fire_bolt(GF_FIRE, dir, dam)";
			charge_min = rand_range(4, 8);
			dice = 1 + level / rand_range(4, 6);
			sides = 8;
			break;

		case ACT_BA_COLD_1:
		case ACT_BA_COLD_2:
		case ACT_BA_COLD_3:
			text = "The %v glows an intense blue...";
			desc = "ball of cold";
			effect = "fire_ball(GF_COLD, dir, dam, rad)";
			radius = (level > 70 ? 3 : 2);
			charge_min = 200 + 50 * randint0(4);
			dice = 10 * (1 + level / rand_range(2, 5));
			break;

		case ACT_BA_FIRE_1:
		case ACT_BA_FIRE_2:
			text = "The %v glows an intense red...";
			desc = "ball of fire";
			effect = "fire_ball(GF_FIRE, dir, dam, rad)";
			radius = (level >= 60 ? 3 : 2);
			charge_min = 200 + 50 * randint0(4);
			dice = 10 * (1 + level / rand_range(2, 5));
			break;

		case ACT_BA_ELEC_2:
		case ACT_BA_ELEC_3:
			text = "The %v crackles with electricity...";
			desc = "ball of lightning";
			effect = "fire_ball(GF_ELEC, dir, dam, rad)";
			radius = (level >= 40 ? 3 : 2);
			charge_min = 200 + 50 * randint0(4);
			dice = 10 * (1 + level / rand_range(2, 5));
			break;

		case ACT_DRAIN_1:
		case ACT_DRAIN_2:
			text = "The %v glows black...";
			desc = "drain life";
			effect = "drain_life(dir, dam)";
			charge_min = 50 + 25 * randint0(5);
			dice = level * 5;
			break;

		case ACT_VAMPIRE_1:
		case ACT_VAMPIRE_2:
			/* XXX Is there a reason to do 3 seperate bolts? */
			text = "The %v throbs red...";
			desc = "vampiric drain";
			effect = "drain_gain_life(dir, dam)";
			charge_min = 200 + 100 * randint0(4);
			dice = level * 10;
			break;

		case ACT_BO_MISS_2:
			text = "The %v grows magical spikes...";
			desc = "arrows";
			effect = "fire_bolt(GF_ARROW, dir, dam)";
			charge_min = 60 + 5 * randint0(10);
			dice = (1 + level / 5);
			if (randint0(100) < 40)
				dice *= 25;
			else
				sides = 25 + 5 * randint0(6);
			break;

		case ACT_WHIRLWIND:
			text = "The %v emits a blast of air...";
			desc = "whirlwind attack";
			effect = "whirlwind_attack()";
			aimed = FALSE;
			charge_min = 200 + 50 * randint0(4);
			break;

		case ACT_CALL_CHAOS:
			text = "The %v glows in scintillating colours...";
			desc = "call chaos";
			effect = "call_chaos()";
			aimed = FALSE;
			charge_min = 300 + 50 * randint0(4);
			break;

		case ACT_ROCKET:
			text = "You launch a rocket!";
			desc = "launch rocket";
			effect = "fire_ball(GF_ROCKET, dir, dam, 2)";
			dice = 300 + level;
			charge_min = 200 + 100 * randint0(4);
			break;

		case ACT_DISP_EVIL:
			text = "The %v floods the area with goodness...";
			desc = "dispel evil";
			effect = "dispel_evil(dam)";
			aimed = FALSE;
			dice = 10 * (1 + level / rand_range(2, 5));
			charge_min = 200 + 100 * randint0(5);
			break;

		case ACT_DISP_GOOD:
			text = "The %v floods the area with evil...";
			desc = "dispel good";
			effect = "dispel_good(dam)";
			aimed = FALSE;
			dice = 10 * (1 + level / rand_range(2, 5));
			charge_min = 200 + 100 * randint0(5);
			break;

		case ACT_BA_MISS_3:
			text = "You breathe the elements.";
			desc = "breathe the elements";
			effect = "fire_ball(GF_MISSILE, dir, dam, rad)";
			radius = 2 + level / rand_range(25, 50);
			dice = 10 * (level + randint1(10));
			charge_min = 300 + 100 * randint0(4);
			break;

		case ACT_CONFUSE:
			desc = "confuse monster";
			effect = "confuse_monster(dir, 50)";
			charge_min = rand_range(250, 500) / level;
			if (charge_min > 50) charge_min = 50;
			break;

		case ACT_SLEEP:
			desc = "sleep nearby monsters";
			effect = "sleep_monsters_touch()";
			aimed = FALSE;
			charge_min = 5 * ((100 - level) / rand_range(5, 10)
					+ randint1(5));
			break;

		case ACT_QUAKE:
			text = "The %v vibrates...";
			desc = "earthquake";
			effect = "earthquake(px, py, rad)";
			aimed = FALSE;
			radius = 5 + level / rand_range(4, 8);
			charge_min = 40 + 10 * randint0(4);
			break;

		case ACT_TERROR:
			text = "The %v emits a loud blast...";
			desc = "terror";
			effect = "turn_monsters(40 + player.lev)";
			aimed = FALSE;
			charge_min = 5 * (rand_range(500, 1000) / level);
			if (charge_min > 200) charge_min = 200;
			break;

		case ACT_TELE_AWAY:
			desc = "teleport away";
			effect = "fire_beam(GF_AWAY_ALL, dir, plev)";
			charge_min = 10 * (rand_range(500, 1000) / level);
			if (charge_min > 500) charge_min = 500;
			break;
			
		case ACT_BANISH_EVIL:
			text = "The power of the artifact banishes evil!";
			desc = "banish evil";
			effect = "banish_evil(200)";
			aimed = FALSE;
			charge_min = 10 * (rand_range(1000, 2000) / level);
			break;

		case ACT_GENOCIDE:
			desc = "genocide";
			effect = "genocide(TRUE)";
			aimed = FALSE;
			charge_min = 25 * (rand_range(500, 1000) / level);
			break;

		case ACT_MASS_GENO:
			desc = "mass genocide";
			effect = "mass_genocide(TRUE)";
			aimed = FALSE;
			charge_min = 50 * (rand_range(500, 1000) / level);
			break;

		case ACT_CHARM_ANIMAL:
			desc = "charm animal";
			effect = "charm_animal(dir, plev)";
			charge_min = 10 * (rand_range(1000, 2000) / level);
			break;

		case ACT_CHARM_UNDEAD:
			desc = "enslave undead";
			effect = "control_one_undead(dir, plev)";
			charge_min = 3 * (rand_range(2500, 5000) / level);
			if (charge_min > 666) charge_min = 666;
			break;

		case ACT_CHARM_OTHER:
			desc = "charm monster";
			effect = "charm_monster(dir, plev)";
			charge_min = 25 * (rand_range(500, 1000) / level);
			break;

		case ACT_CHARM_ANIMALS:
			desc = "animal friendship";
			effect = "charm_animals(plev * 2)";
			aimed = FALSE;
			charge_min = 25 * (rand_range(500, 1000) / level);
			break;

		case ACT_CHARM_OTHERS:
			desc = "mass charm";
			effect = "charm_monsters(plev * 2)";
			aimed = FALSE;
			charge_min = 25 * (rand_range(1000, 2000) / level);
			break;

		case ACT_SUMMON_ANIMAL:
			text = "You summon a beast.";
			desc = "summon animal";
			effect = "summon_specific(-1, px, py, plev, SUMMON_ANIMAL_RANGER, TRUE, TRUE, TRUE)";
			aimed = FALSE;
			charge_min = 10 * (rand_range(500, 1000) / level);
			break;

		case ACT_SUMMON_PHANTOM:
			text = "You summon a phantasmal servant.";
			desc = "summon phantasmal servant";
			effect = "summon_specific(-1, px, py, plev, SUMMON_PHANTOM, TRUE, TRUE, TRUE)";
			aimed = FALSE;
			charge_min = 10 * (rand_range(500, 1000) / level);
			break;

		case ACT_SUMMON_ELEMENTAL:
			text = "You summon an elemental.";
			desc = "summon elemental";
			effect = "summon_unsafe(SUMMON_ELEMENTAL, \"You fail to control it!\")";
			aimed = FALSE;
			charge_min = 50 * (rand_range(500, 1000) / level);
			break;

		case ACT_SUMMON_DEMON:
			text = "The area fills with the stench of sulphur and brimstone.";
			desc = "summon demon";
			effect = "summon_unsafe(SUMMON_DEMON, \"'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'\")";
			aimed = FALSE;
			charge_min = 666;
			charge_max = 333 * rand_range(2, 5);
			break;

		case ACT_SUMMON_UNDEAD:
			text = "Ancient, long-dead forms rise from the ground.";
			if (level < rand_range(60, 100))
			{
				desc = "summon undead";
				effect = "summon_unsafe(SUMMON_UNDEAD, \"'The dead arise... to punish you for disturbing them!'\")";
			}
			else
			{
				desc = "summon greater undead";
				effect = "summon_unsafe(SUMMON_HI_UNDEAD, \"'The dead arise... to punish you for disturbing them!'\")";
			}
			aimed = FALSE;
			charge_min = 666;
			charge_max = 333 * rand_range(2, 5);
			break;

		case ACT_CURE_LW:
			desc = "remove fear & heal";
			effect = "clear_afraid(); hp_player(dam)";
			aimed = FALSE;
			dice = 5 * (1 + level / rand_range(2, 5));
			if (dice > 50) dice = 50;
			charge_min = rand_range(5, 10);
			break;

		case ACT_CURE_MW:
			desc = "cure wounds & heal";
			effect = "hp_player(dam); inc_cut(-50)";
			aimed = FALSE;
			dice = 15 * (1 + level / rand_range(5, 10));
			if (dice > 150) dice = 150;
			charge_min = rand_range(2, 5);
			break;

		case ACT_CURE_POISON:
			desc = "remove fear & cure poison";
			effect = "clear_afraid(); clear_poisoned()";
			aimed = FALSE;
			charge_min = rand_range(4, 8);
			break;

		case ACT_REST_LIFE:
			desc = "restore life levels";
			effect = "restore_level()";
			aimed = FALSE;
			charge_min = 50 * (rand_range(100, 500) / level);
			break;

		case ACT_REST_ALL:
			desc = "restore stats and life levels";
			effect = "do_res_stat(A_STR); do_res_stat(A_INT); "
				"do_res_stat(A_WIS); do_res_stat(A_DEX); "
				"do_res_stat(A_CON); do_res_stat(A_CHR); "
				"restore_level()";
			aimed = FALSE;
			charge_min = 50 * (rand_range(500, 1000) / level);
			break;

		case ACT_CURE_700:
			desc = "cure wounds & heal";
			effect = "hp_player(dam); clear_cut()";
			aimed = FALSE;
			dice = 700;
			charge_min = 10 * (rand_range(1000, 2000) / level);
			break;

		case ACT_CURE_1000:
			desc = "cure wounds & heal";
			effect = "hp_player(dam); clear_cut()";
			aimed = FALSE;
			dice = 1000;
			charge_min = 111 * (rand_range(500, 1000) / level);
			break;

		case ACT_ESP:
			text = "The %v enters your thoughts...";
			desc = "telepathy";
			effect = "inc_tim_esp(dam)";
			aimed = FALSE;
			dice = 5 * (1 + level / rand_range(10, 20));
			sides = -(5 * randint1(6));
			charge_min = 100 + 50 * randint0(4);
			break;

		case ACT_BERSERK:
			text = "The %v drives you into a berserk rage...";
			desc = "heroism and berserk";
			effect = "inc_shero(dam); inc_blessed(dam)";
			aimed = FALSE;
			dice = 5 * (1 + level / rand_range(5, 10));
			sides = -dice;
			charge_min = 100 + 50 * randint0(4);
			break;
			
		case ACT_PROT_EVIL:
			desc = "protection from evil";
			effect = "inc_protevil(dam)";
			aimed = FALSE;
			dice = 10 * (1 + level / rand_range(5, 10));
			sides = -25;
			charge_min = 200 + 25 * randint0(5);
			break;

		case ACT_RESIST_ALL:
			text = "The %v glows many colours...";
			desc = "resist elements";
			effect = "inc_oppose_acid(dam); inc_oppose_elec(dam); "
				"inc_oppose_fire(dam); inc_oppose_cold(dam); "
				"inc_oppose_pois(dam)";
			aimed = FALSE;
			dice = 5 * (1 + level / rand_range(5, 10));
			sides = -dice;
			charge_min = 100 + 50 * randint0(4);
			break;

		case ACT_SPEED:
		case ACT_XTRA_SPEED:
			desc = "speed";
			effect = "inc_fast(dam)";
			aimed = FALSE;
			dice = 5 * (1 + level / rand_range(5, 10));
			sides = -dice;
			charge_min = 100 + 50 * randint0(4);
			break;

		case ACT_WRAITH:
			text = "The %v fades out...";
			desc = "wraith form";
			effect = "inc_wraith_form(dam)";
			aimed = FALSE;
			dice = 1 + level / rand_range(3, 6);
			sides = -dice;
			charge_min = 1000 * 500 * randint0(4);
			break;

		case ACT_INVULN:
			text = "The %v fires a beam of bright light at you...";
			desc = "invulnerability";
			effect = "inc_invuln(dam)";
			aimed = FALSE;
			dice = 1 + level / rand_range(10, 20);
			sides = -dice;
			charge_min = 1000 + 500 * randint0(4);
			break;

		case ACT_TELEPORT_1:
		case ACT_TELEPORT_2:
			text = "The %v twists space around you...";
			desc = "teleport";
			effect = "teleport_player(dam)";
			aimed = FALSE;
			dice = 100;
			charge_min = 5 * (rand_range(250, 500) / level);
			break;

		case ACT_LIGHT:
			text = "The %v wells with clear light...";
			desc = "light area";
			effect = "lite_area(dam, 3)";
			aimed = FALSE;
			dice = 1 + level / rand_range(5, 10);
			if (dice > 5) dice = 5;
			sides = 5 * rand_range(2, 5);
			charge_min = rand_range(6, 12);
			break;

		case ACT_MAP_LIGHT:
			text = "The %v shines brightly...";
			desc = "map & light area";
			effect = "map_area(); lite_area(dam, 3)";
			aimed = FALSE;
			dice = 1 + level / rand_range(5, 10);
			if (dice > 5) dice = 5;
			sides = 5 * rand_range(2, 5);
			charge_min = 5 * rand_range(6, 12);
			break;
			
		case ACT_DETECT_ALL:
			text = "An image forms in your mind...";
			desc = "detection";
			effect = "detect_all()";
			aimed = FALSE;
			charge_min = 5 * (rand_range(250, 500) / level);
			break;

		case ACT_DETECT_XTRA:
			desc = "detection, probing and identify true";
			effect = "detect_all(); probing(); identify_fully()";
			aimed = FALSE;
			charge_min = 100 * (rand_range(500, 1000) / level);
			break;

		case ACT_ID_FULL:
			desc = "identify true";
			effect = "identify_fully()";
			aimed = FALSE;
			charge_min = 10 * (rand_range(2500, 5000) / level);
			break;

		case ACT_ID_PLAIN:
			desc = "identify";
			effect = "if not ident_spell() then return end";
			aimed = FALSE;
			charge_min = rand_range(250, 500) / level;
			break;

		case ACT_RUNE_EXPLO:
			text = "The %v glows bright red...";
			desc = "explosive rune";
			effect = "explosive_rune()";
			aimed = FALSE;
			charge_min = 10 * (rand_range(500, 1000) / level);
			break;

		case ACT_RUNE_PROT:
			desc = "rune of protection";
			effect = "warding_glyph()";
			aimed = FALSE;
			charge_min = 10 * (rand_range(1000, 2000) / level);
			break;

		case ACT_SATIATE:
			desc = "satisfy hunger";
			effect = "set_food(PY_FOOD_MAX - 1)";
			aimed = FALSE;
			charge_min = 5 * (rand_range(500, 1000) / level);
			break;

		case ACT_DEST_DOOR:
			desc = "destroy doors";
			effect = "destroy_doors_touch()";
			aimed = FALSE;
			charge_min = (rand_range(250, 500) / level);
			if (charge_min > 50) charge_min = 50;
			break;

		case ACT_STONE_MUD:
			desc = "stone to mud";
			effect = "wall_to_mud(dir)";
			charge_min = (rand_range(100, 200) / level);
			if (charge_min > 25) charge_min = 25;
			break;

		case ACT_RECHARGE:
			desc = "recharging";
			effect = "recharge(130)";
			aimed = FALSE;
			charge_min = 5 * (rand_range(250, 500) / level);
			break;

		case ACT_ALCHEMY:
			desc = "alchemy";
			effect = "alchemy()";
			aimed = FALSE;
			charge_min = 10 * (rand_range(1000, 2000) / level);
			break;

		case ACT_DIM_DOOR:
			text = "You open a dimensional gate. Choose a destination.";
			desc = "dimension door";
			effect = "if not dimension_door() return end";
			aimed = FALSE;
			charge_min = 5 * (rand_range(250, 500) / level);
			break;

		case ACT_RECALL:
			desc = "word of recall";
			effect = "word_of_recall()";
			aimed = FALSE;
			charge_min = 10 * (rand_range(500, 1000) / level);
			break;
	}

	if (charge_min < 1) charge_min = 1;
	if (charge_min > 5000) charge_min = 5000;
	if (!charge_max) charge_max = charge_min * randint1(2);

	if (sides > 1 && dice < 1) dice = 1;
	
	/* Get the basic name of the object in the description */
	strnfmt(text_buf, 256, text, OBJECT_FMT(o_ptr, FALSE, 0));
	
	/* Construct the lua script */
	len = strnfmt(buf, 1024, "msgf(\"%s\"); ", text_buf);
	
	if (aimed) strnfcat(buf, 1024, &len, 
				"local success; local dir; "
				"success, dir = get_aim_dir(); "
				"if not success then return; end; ");

	if (sides > 1)
		strnfcat(buf, 1024, &len, "local dam = damroll(%i, %i); ", dice, sides);
	else if (sides < 0)
		strnfcat(buf, 1024, &len, "local dam = rand_range(%i, %i); ", dice, dice + -sides);
	else if (dice > 0)
		strnfcat(buf, 1024, &len, "local dam = %i; ", dice);

	if (radius > 0 && radius != 2)
		strnfcat(buf, 1024, &len, "local rad = %i; ", radius);
	
	strnfcat(buf, 1024, &len, "%s; object.timeout = ", effect);

	if (charge_min != charge_max)
		strnfcat(buf, 1024, &len, "rand_range(%i, %i)", charge_min, charge_max);
	else
		strnfcat(buf, 1024, &len, "%i", charge_min);
	
	/* Usage script */
	o_ptr->trigger[TRIGGER_USE] = quark_add(buf);
	
	
	len = strnfmt(buf, 1024, "return \"%s", desc);
	
	if (sides > 1)
		strnfcat(buf, 1024, &len, " (%id%i)", dice, sides);
	else if (sides < 0)
		strnfcat(buf, 1024, &len, " (%i-%i)", dice, dice + -sides);
	else if (dice > 0)
		strnfcat(buf, 1024, &len, " (%i)", dice);

	if (radius > 0) strnfcat(buf, 1024, &len, ", rad. %i,", radius);
	
	if (charge_min != charge_max)
		strnfcat(buf, 1024, &len, " every %i-%i turns \"", charge_min, charge_max);
	else
		strnfcat(buf, 1024, &len, " every %i turns \"", charge_min);
	
	/* Description script */
	o_ptr->trigger[TRIGGER_DESC] = quark_fmt(buf);

	o_ptr->flags3 |= TR3_ACTIVATE;
	o_ptr->timeout = 0;
}


static int choose_activation_power(object_type *o_ptr, int level)
{
	int type = 0, chance = 0;
	
	/* Hack - ignore parameters */
	(void) o_ptr;
	(void) level;

	while (!type || (randint1(100) >= chance))
	{
		type = randint1(255);
		switch (type)
		{
			case ACT_SUNLIGHT:
			case ACT_BO_MISS_1:
			case ACT_BA_POIS_1:
			case ACT_BO_ELEC_1:
			case ACT_BO_ACID_1:
			case ACT_BO_COLD_1:
			case ACT_BO_FIRE_1:
			case ACT_CONFUSE:
			case ACT_SLEEP:
			case ACT_QUAKE:
			case ACT_CURE_LW:
			case ACT_CURE_MW:
			case ACT_CURE_POISON:
			case ACT_BERSERK:
			case ACT_LIGHT:
			case ACT_MAP_LIGHT:
			case ACT_DEST_DOOR:
			case ACT_STONE_MUD:
			case ACT_TELEPORT_1:
			case ACT_TELEPORT_2:
				chance = 101;
				break;
			case ACT_BA_COLD_1:
			case ACT_BA_FIRE_1:
			case ACT_DRAIN_1:
			case ACT_TELE_AWAY:
			case ACT_ESP:
			case ACT_RESIST_ALL:
			case ACT_DETECT_ALL:
			case ACT_RECALL:
			case ACT_SATIATE:
			case ACT_RECHARGE:
				chance = 85;
				break;
			case ACT_TERROR:
			case ACT_PROT_EVIL:
			case ACT_ID_PLAIN:
				chance = 75;
				break;
			case ACT_DRAIN_2:
			case ACT_VAMPIRE_1:
			case ACT_BO_MISS_2:
			case ACT_BA_FIRE_2:
			case ACT_REST_LIFE:
				chance = 66;
				break;
			case ACT_BA_COLD_3:
			case ACT_BA_ELEC_3:
			case ACT_WHIRLWIND:
			case ACT_VAMPIRE_2:
			case ACT_CHARM_ANIMAL:
				chance = 50;
				break;
			case ACT_SUMMON_ANIMAL:
				chance = 40;
				break;
			case ACT_DISP_EVIL:
			case ACT_BA_MISS_3:
			case ACT_DISP_GOOD:
			case ACT_BANISH_EVIL:
			case ACT_GENOCIDE:
			case ACT_MASS_GENO:
			case ACT_CHARM_UNDEAD:
			case ACT_CHARM_OTHER:
			case ACT_SUMMON_PHANTOM:
			case ACT_REST_ALL:
			case ACT_RUNE_EXPLO:
				chance = 33;
				break;
			case ACT_CALL_CHAOS:
			case ACT_ROCKET:
			case ACT_CHARM_ANIMALS:
			case ACT_CHARM_OTHERS:
			case ACT_SUMMON_ELEMENTAL:
			case ACT_CURE_700:
			case ACT_SPEED:
			case ACT_ID_FULL:
			case ACT_RUNE_PROT:
				chance = 25;
				break;
			case ACT_CURE_1000:
			case ACT_XTRA_SPEED:
			case ACT_DETECT_XTRA:
			case ACT_DIM_DOOR:
				chance = 10;
				break;
			case ACT_SUMMON_UNDEAD:
			case ACT_SUMMON_DEMON:
			case ACT_WRAITH:
			case ACT_INVULN:
			case ACT_ALCHEMY:
				chance = 5;
				break;
			default:
				chance = 0;
		}
	}

	return type;
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

			if (one_in_(2))
				activate = ACT_BO_ACID_1;
			
			break;

		case 6:
		case 7:
			o_ptr->flags1 |= TR1_BRAND_ELEC;
			o_ptr->flags2 |= TR2_RES_ELEC;

			if (one_in_(4))
				activate = ACT_BO_ELEC_1;
			else if (one_in_(5))
				activate = ACT_BA_ELEC_2;
			else if (one_in_(5))
				activate = ACT_BA_ELEC_3;

			break;

		case 8:
		case 9:
		case 10:
			o_ptr->flags1 |= TR1_BRAND_FIRE;
			o_ptr->flags2 |= TR2_RES_FIRE;
			o_ptr->flags3 |= TR3_LITE;

			if (one_in_(5))
				activate = ACT_BO_FIRE_1;
			else if (one_in_(5))
				activate = ACT_BA_FIRE_1;
			else if (one_in_(5))
				activate = ACT_BA_FIRE_2;

			break;

		case 11:
		case 12:
		case 13:
			o_ptr->flags1 |= TR1_BRAND_COLD;
			o_ptr->flags2 |= TR2_RES_COLD;

			if (one_in_(6))
				activate = ACT_BO_COLD_1;
			else if (one_in_(6))
				activate = ACT_BA_COLD_1;
			else if (one_in_(6))
				activate = ACT_BA_COLD_2;

			break;

		case 14:
		case 15:	
			o_ptr->flags1 |= TR1_BRAND_POIS;
			o_ptr->flags2 |= TR2_RES_POIS;

			if (one_in_(3))
				activate = ACT_BA_POIS_1;

			break;

		case 16:
		case 17:
			o_ptr->flags1 |= TR1_CHAOTIC;
			o_ptr->flags2 |= TR2_RES_CHAOS;
			if (one_in_(3))
				o_ptr->flags4 |= TR4_PATRON;

			if (one_in_(11))
				activate = ACT_CALL_CHAOS;
			
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

			if (one_in_(6))
				activate = ACT_VAMPIRE_1;
			else if (one_in_(9))
				activate = ACT_VAMPIRE_2;

			break;

		case 36:
			o_ptr->flags2 |= TR2_HOLD_LIFE;

			if (!one_in_(3))
				activate = ACT_DRAIN_1;
			else
				activate = ACT_DRAIN_2;

			break;

		case 37:
			o_ptr->to_h += rand_range(5, 15);
			o_ptr->to_d += rand_range(5, 15);

			activate = ACT_WHIRLWIND;

			break;

		case 38:
			o_ptr->flags1 |= TR1_SLAY_ANIMAL;

			if (!one_in_(3))
				activate = ACT_CHARM_ANIMAL;
			else
				activate = ACT_CHARM_ANIMALS;

			break;

		case 39:
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;

			activate = ACT_CHARM_UNDEAD;

			break;

		case 40:
			if (o_ptr->tval == TV_SWORD)
				o_ptr->flags1 |= TR1_TUNNEL;

			activate = ACT_STONE_MUD;

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

		if (one_in_(4))
			activate = ACT_DISP_EVIL;
		else if (one_in_(7))
			activate = ACT_CURE_700;
		
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

		if (one_in_(8))
			activate = ACT_RESIST_ALL;

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

		if (one_in_(3))
			activate = ACT_TELEPORT_2;
		else if (one_in_(3))
			activate = ACT_TELEPORT_1;

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

			if (one_in_(8))
				activate = ACT_BERSERK;
			
			break;

		case 6:
		case 7:
			o_ptr->flags1 |= TR1_INT;
			o_ptr->flags2 |= TR2_SUST_INT;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_FEATHER;
			
			if (one_in_(8))
				activate = ACT_ID_PLAIN;
			
			break;

		case 8:
		case 9:
			o_ptr->flags1 |= TR1_WIS;
			o_ptr->flags2 |= TR2_SUST_WIS;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_SEE_INVIS;

			if (one_in_(8))
				activate = ACT_DETECT_ALL;
			
			break;

		case 10:
		case 11:
			o_ptr->flags1 |= TR1_DEX;
			o_ptr->flags2 |= TR2_SUST_DEX;
			if (one_in_(3))
				o_ptr->flags2 |= TR2_FREE_ACT;

			if (one_in_(8))
				activate = ACT_SPEED;
			
			break;

		case 12:
		case 13:
			o_ptr->flags1 |= TR1_CON;
			o_ptr->flags2 |= TR2_SUST_CON;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_REGEN;

			if (one_in_(8))
				activate = ACT_SATIATE;
			
			break;

		case 14:
		case 15:
			o_ptr->flags1 |= TR1_CHR;
			o_ptr->flags2 |= TR2_SUST_CHR;
			if (one_in_(3))
				o_ptr->flags3 |= TR3_LITE;

			if (one_in_(8))
				activate = ACT_RECALL;
			
			break;

		case 16:
			o_ptr->flags3 |= TR3_LITE;
			o_ptr->flags2 |= TR2_RES_LITE;

			if (one_in_(3))
				activate = ACT_LIGHT;

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

			if (one_in_(3))
				activate = ACT_DISP_EVIL;
			else if (one_in_(2))
				activate = ACT_BANISH_EVIL;
			else
				activate = ACT_PROT_EVIL;

			break;

		case 24:
			o_ptr->flags1 |= TR1_STEALTH;

			activate = ACT_SLEEP;

			break;

		case 25:
			o_ptr->flags1 |= TR1_WIS;

			activate = ACT_ESP;

			break;

		case 26:
			o_ptr->flags2 |= TR2_RES_DARK;

			activate = ACT_SUNLIGHT;

			break;

		case 27:
			o_ptr->flags2 |= TR2_RES_CHAOS;
			o_ptr->flags2 |= TR2_RES_CONF;

			break;

		case 28:
			o_ptr->flags2 |= TR2_RES_NETHER;
			o_ptr->flags2 |= TR2_HOLD_LIFE;

			if (one_in_(6))
				activate = ACT_REST_LIFE;

			break;

		case 29:
			o_ptr->flags2 |= TR2_RES_SOUND;
			o_ptr->flags2 |= TR2_RES_SHARDS;

			break;

		case 30:
			o_ptr->flags2 |= TR2_RES_FEAR;

			activate = ACT_TERROR;

			break;

		case 31:
			o_ptr->flags1 |= TR1_SLAY_ANIMAL;

			if (one_in_(3))
				activate = ACT_CHARM_ANIMAL;
			else if (one_in_(2))
				activate = ACT_CHARM_ANIMALS;
			else
				activate = ACT_SUMMON_ANIMAL;

			break;

		case 32:
			o_ptr->flags1 |= TR1_SLAY_UNDEAD;

			if (one_in_(2))
				activate = ACT_CHARM_UNDEAD;
			else
				activate = ACT_SUMMON_UNDEAD;

			break;

		case 33:
			o_ptr->flags1 |= TR1_SLAY_DEMON;

			activate = ACT_SUMMON_DEMON;

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
	int activate = 0;

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

		if (!activate) activate = act;
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

		if (!activate) activate = act;
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
		 * If an appropriate activation isn't already chosen, get
		 * a random activation
		 */
		if (!activate)
			activate = choose_activation_power(o_ptr, activation_level);

		apply_activation_power(o_ptr, activate, activation_level);
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


void random_artifact_resistance(object_type *o_ptr)
{
	bool give_resistance = FALSE, give_power = FALSE;

	/* Terror Mask is for warriors... */
	if (o_ptr->activate == ART_TERROR)
	{
		if (p_ptr->rp.pclass == CLASS_WARRIOR)
		{
			give_power = TRUE;
			give_resistance = TRUE;
		}
		else
		{
			o_ptr->flags3 |=
				(TR3_CURSED | TR3_HEAVY_CURSE | TR3_AGGRAVATE | TR3_TY_CURSE);
			return;
		}
	}

	switch (o_ptr->activate)
	{
		case ART_CELEBORN:
		case ART_ARVEDUI:
		case ART_CASPANION:
		case ART_HITHLOMIR:
		case ART_ROHIRRIM:
		case ART_CELEGORM:
		case ART_ANARION:
		case ART_THRANDUIL:
		case ART_LUTHIEN:
		case ART_THROR:
		case ART_THORIN:
		case ART_NIMTHANC:
		case ART_DETHANC:
		case ART_NARTHANC:
		case ART_STING:
		case ART_TURMIL:
		case ART_THALKETTOTH:
		{
			/* Give a resistance */
			give_resistance = TRUE;
		}
			break;
		case ART_MAEDHROS:
		case ART_GLAMDRING:
		case ART_ORCRIST:
		case ART_ANDURIL:
		case ART_ZARCUTHRA:
		case ART_GURTHANG:
		case ART_HARADEKKET:
		case ART_BRAND:
		case ART_DAWN:
		{
			/* Give a resistance OR a power */
			if (one_in_(2)) give_resistance = TRUE;
			else
				give_power = TRUE;
		}
			break;
		case ART_NENYA:
		case ART_VILYA:
		case ART_BERUTHIEL:
		case ART_FINGOLFIN:
		case ART_THINGOL:
		case ART_ULMO:
		case ART_OLORIN:
		{
			/* Give a power */
			give_power = TRUE;
		}
			break;
		case ART_POWER:
		case ART_GONDOR:
		case ART_AULE:
		{
			/* Give both */
			give_power = TRUE;
			give_resistance = TRUE;
		}
			break;
	}

	if (give_power)
	{
		add_ego_power(EGO_XTRA_ABILITY, o_ptr);
	}

	if (give_resistance)
	{
		add_ego_power(EGO_XTRA_HI_RESIST, o_ptr);
	}
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

	random_artifact_resistance(q_ptr);

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
