/* File: borg5.c */
/* Purpose: Medium level stuff for the Borg -BEN- */

#include "angband.h"

#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"
#include "zborg4.h"
#include "zborg5.h"


/*
 * Helper function -- calculate "power" of equipment
 * Dynamic Calcs off
 */
static s32b borg_power_aux3(void)
{
	int hold = 0;
	int damage = 0, dam = 0;

	int i;

	int cur_wgt = 0;
	int max_wgt = 0;

	int realm, book, k;

	s32b value = 0L;

	list_item *l_ptr;


	/* Obtain the "hold" value (weight limit for weapons) */
	hold = adj_str_hold[my_stat_ind[A_STR]];

	/*** Analyze weapon ***/
	l_ptr = look_up_equip_slot(EQUIP_WIELD);

	/* Examine current weapon */
	if (l_ptr)
	{
		/* Calculate "average" damage per "normal" blow  */
		/* and assume we can enchant up to +8 if borg_skill[BI_CLEVEL] > 25 */
		damage = (l_ptr->dd * l_ptr->ds * 20L);


		/* Reward "damage" and increased blows per round */
		value += damage * (borg_skill[BI_BLOWS] + 1);

		/* Reward "bonus to hit" */
		if (l_ptr->to_h > 8 || borg_skill[BI_CLEVEL] < 25)
			value += ((borg_skill[BI_TOHIT] + l_ptr->to_h) * 30L);
		else
			value += ((borg_skill[BI_TOHIT] + 8) * 30L);

		/* Reward "bonus to dam" */
		value += ((borg_skill[BI_TODAM] + l_ptr->to_d) * 30L);

		/* extra boost for deep dungeon */
		if (borg_skill[BI_MAXDEPTH] >= 75)
		{
			value += ((borg_skill[BI_TOHIT] + l_ptr->to_h) * 15L);

			value += l_ptr->dd * l_ptr->ds * 20L * 2 * borg_skill[BI_BLOWS];
		}

		/* assume 2x base damage for x% of creatures */
		dam = damage * 2 * borg_skill[BI_BLOWS];
		if (borg_skill[BI_WS_ANIMAL]) value += (dam * 2) / 2;
		if (borg_skill[BI_WB_POIS]) value += (dam * 2) / 2;
		if (borg_skill[BI_WS_EVIL]) value += (dam * 7) / 2;

		/* assume 3x base damage for x% of creatures */
		dam = damage * 3 * borg_skill[BI_BLOWS];
		if (borg_skill[BI_WS_UNDEAD]) value += (dam * 5) / 2;
		if (borg_skill[BI_WS_DEMON]) value += (dam * 3) / 2;
		if (borg_skill[BI_WS_DRAGON] &&
			(!borg_skill[BI_WK_DRAGON])) value += (dam * 6) / 2;
		if (borg_skill[BI_WS_GIANT]) value += (dam * 4) / 2;
		if (borg_skill[BI_WB_ACID]) value += (dam * 4) / 2;
		if (borg_skill[BI_WB_ELEC]) value += (dam * 5) / 2;
		if (borg_skill[BI_WB_FIRE]) value += (dam * 3) / 2;
		if (borg_skill[BI_WB_COLD]) value += (dam * 3) / 2;
		if (borg_skill[BI_WB_VAMPIRIC]) value += (dam * 3) / 2;
		if (borg_skill[BI_WB_VORPAL]) value += (dam * 3) / 2;
		if (borg_skill[BI_WB_CHAOTIC]) value += (dam * 12) / 10;
		/* SOrc and STroll get 1/2 of reward now */
		if (borg_skill[BI_WS_ORC]) value += (dam * 1) / 2;
		if (borg_skill[BI_WS_TROLL]) value += (dam * 2) / 2;
		/* and the other 2/2 if SEvil not possesed */
		if (borg_skill[BI_WS_ORC] &&
			!borg_skill[BI_WS_EVIL]) value += (dam * 1) / 2;
		if (borg_skill[BI_WS_TROLL] &&
			!borg_skill[BI_WS_EVIL]) value += (dam * 1) / 2;

		/* assume 5x base damage for x% of creatures */
		dam = damage * 5 * borg_skill[BI_BLOWS];
		if (borg_skill[BI_WK_DRAGON]) value += (dam * 5) / 2;
	}
	else
	{
		/* Martial Artists */
		int ma = MAX_MA - 1;

		const martial_arts *ma_ptr = &ma_blows[MAX_MA];

		/* Calculate best Monk Attacks */
		while (ma != 0)
		{
			ma_ptr = &ma_blows[ma];

			/* Can do this attack */
			if (borg_skill[BI_CLEVEL] >= ma_ptr->min_level)
				break;

			/* Reduce the ma level and try again */
			ma--;
		}

		/* Calculate "average" damage per "normal" blow  */
		/* and assume we can enchant up to +8 if borg_skill[BI_CLEVEL] > 25 */
		damage = (ma_ptr->dd * ma_ptr->ds * 20L);


		/* Reward "damage" and increased blows per round */
		value += damage * (borg_skill[BI_BLOWS] + 1);

		/* Reward "bonus to hit" */
		value += ((borg_skill[BI_TOHIT] + 8) * 30L);

		/* Reward "bonus to dam" */
		value += ((borg_skill[BI_TODAM]) * 30L);

		/* extra boost for deep dungeon */
		if (borg_skill[BI_MAXDEPTH] >= 75)
		{
			value += ((borg_skill[BI_TOHIT]) * 15L);

			value += ma_ptr->dd * ma_ptr->ds * 20L * 2 * borg_skill[BI_BLOWS];
		}

	}
	/* It is only on Grond */
	if (borg_skill[BI_W_IMPACT]) value += 5000L;

	/* Hack -- It is hard to hold a heavy weapon */
	if (borg_skill[BI_HEAVYWEPON]) value -= 500000L;

	/*** Analyze bow ***/
	l_ptr = look_up_equip_slot(EQUIP_BOW);

	/* Examine current bow */
	if (l_ptr)
	{
		/* Calculate "average" damage per "normal" shot (times 2) */
		if (l_ptr->to_d > 8 || borg_skill[BI_CLEVEL] < 25)
			damage = ((my_ammo_sides) + (l_ptr->to_d)) * my_ammo_power;
		else
			damage = (my_ammo_sides + 8) * my_ammo_power;

		/* Reward "damage" */
		value += (borg_skill[BI_SHOTS] * damage * 9L);

		/*
		 * AJG - slings force you to carry heavy ammo.
		 * Penalty for that unles you have lots of str
		 */
		if (k_info[l_ptr->k_idx].sval == SV_SLING &&
			!(l_ptr->xtra_name && *l_ptr->xtra_name) && my_stat_ind[A_STR] < 14)
		{
			value -= 5000L;
		}

		/* Reward "bonus to hit" */
		if (l_ptr->to_h > 8 || borg_skill[BI_CLEVEL] < 25)
			value += ((borg_skill[BI_TOHIT] + l_ptr->to_h) * 7L);
		else
			value += ((borg_skill[BI_TOHIT] + 8) * 7L);

		/* Prefer bows */
		if (borg_class == CLASS_RANGER &&
			my_ammo_tval == TV_ARROW) value += 30000L;

		/* Hack -- It is hard to hold a heavy weapon */
		if (hold < l_ptr->weight / 10) value -= 500000L;
	}

	/*** apw Analyze dragon armour  ***/
	l_ptr = look_up_equip_slot(EQUIP_BODY);

	/* Examine current armor */
	if (l_ptr && (l_ptr->tval == TV_DRAG_ARMOR))
	{
		switch (k_info[l_ptr->k_idx].sval)
		{
			case SV_DRAGON_BLACK:
			case SV_DRAGON_BLUE:
			case SV_DRAGON_WHITE:
			case SV_DRAGON_RED:
			{
				value += 1100;
				break;
			}
			case SV_DRAGON_GREEN:
			{
				value += 2750;
				break;
			}
			case SV_DRAGON_MULTIHUED:
			{
				value += 3250;
				break;
			}
			case SV_DRAGON_SHINING:
			case SV_DRAGON_LAW:
			case SV_DRAGON_BRONZE:
			case SV_DRAGON_GOLD:
			case SV_DRAGON_CHAOS:
			case SV_DRAGON_BALANCE:
			case SV_DRAGON_POWER:
			{
				value += 5150;
			}
		}
	}


	/*** Reward various things ***/

	/* Hack -- Reward light radius */
	value += (borg_skill[BI_CUR_LITE] * 1000000L);

	/* Hack -- Reward speed */

	if (borg_skill[BI_SPEED] >= 150)
		value += (((borg_skill[BI_SPEED] - 120) * 1000L) + 185000L);

	if (borg_skill[BI_SPEED] >= 145 && borg_skill[BI_SPEED] <= 149)
		value += (((borg_skill[BI_SPEED] - 120) * 1000L) + 180000L);

	if (borg_skill[BI_SPEED] >= 140 && borg_skill[BI_SPEED] <= 144)
		value += (((borg_skill[BI_SPEED] - 120) * 1000L) + 175000L);

	if (borg_skill[BI_SPEED] >= 135 && borg_skill[BI_SPEED] <= 139)
		value += (((borg_skill[BI_SPEED] - 120) * 1000L) + 165000L);

	if (borg_skill[BI_SPEED] >= 130 && borg_skill[BI_SPEED] <= 134)
		value += (((borg_skill[BI_SPEED] - 120) * 1000L) + 150000L);

	if (borg_skill[BI_SPEED] >= 125 && borg_skill[BI_SPEED] <= 129)
		value += (((borg_skill[BI_SPEED] - 110) * 1000L) + 125000L);

	if (borg_skill[BI_SPEED] >= 120 && borg_skill[BI_SPEED] <= 124)
		value += (((borg_skill[BI_SPEED] - 110) * 1000L) + 100000L);

	if (borg_skill[BI_SPEED] >= 115 && borg_skill[BI_SPEED] <= 119)
		value += (((borg_skill[BI_SPEED] - 110) * 1000L) + 75000L);

	if (borg_skill[BI_SPEED] >= 110 && borg_skill[BI_SPEED] <= 114)
		value += (((borg_skill[BI_SPEED] - 110) * 1000L) + 55000L);
	else
		value += (((borg_skill[BI_SPEED] - 110) * 2500L));


	/* Hack -- Reward strength bonus */
	value += (my_stat_ind[A_STR] * 100L);

	/* Hack -- Reward intelligence bonus */
	if ((borg_skill[BI_INTMANA]) && (my_stat_ind[A_INT] <= 37))
	{
		value += (my_stat_ind[A_INT] * 500L);

		/* Bonus for sp. */
		value += ((adj_mag_mana[my_stat_ind[A_INT]] *
				   borg_skill[BI_CLEVEL]) / 2) * 155L;

		/* bonus for fail rate */
		value += adj_mag_stat[my_stat_ind[A_INT]] * 5010L;

		/* mage should try to get min fail to 0 */
		if (borg_class == CLASS_MAGE)
		{
			/* Bonus for mages to in order to keep GOI fail rate down */
			if (borg_spell_legal(REALM_SORCERY, 3, 7) ||
				borg_spell_legal(REALM_LIFE, 3, 7))
			{
				value += my_stat_ind[A_INT] * 35000L;
			}

			/* other fail rates */
			if (adj_mag_fail[my_stat_ind[A_INT]] < 1)
				value += 90000L;

		}
	}

	/* Hack -- Reward wisdom bonus */
	if ((borg_skill[BI_WISMANA]) && (my_stat_ind[A_WIS] <= 37))
	{
		value += (my_stat_ind[A_WIS] * 200L);

		/* Bonus for sp. */
		value +=
			((adj_mag_mana[my_stat_ind[A_WIS]] * borg_skill[BI_CLEVEL]) / 2) *
			150L;

		/* bonus for fail rate */
		value += adj_mag_stat[my_stat_ind[A_WIS]] * 3000L;

		/* priest should try to get min fail to 0 */
		if (borg_class == CLASS_PRIEST)
		{
			/* Bonus for priests to in order to keep Holy Word fail rate down */
			if (borg_spell_legal(REALM_LIFE, 2, 6)) value +=
					my_stat_ind[A_WIS] * 35000L;

			if (adj_mag_fail[my_stat_ind[A_WIS]] < 1)
				value += 70000L;
		}

	}

	/* Dexterity Bonus --good for attacking and ac */
	if (my_stat_ind[A_DEX] <= 37)
	{
		/* Hack -- Reward bonus */
		value += (my_stat_ind[A_DEX] * 120L);
	}

	/* Constitution Bonus */
	if (my_stat_ind[A_CON] <= 37)
	{
		int bonus_hp =
			(((adj_con_mhp[my_stat_ind[A_CON]] -
			   128) * borg_skill[BI_MAXCLEVEL]) / 2);

		value += (my_stat_ind[A_CON] * 150L);
		/* Hack -- Reward hp bonus */
		/*   This is a bit wierd because we are not really giving a bonus for */
		/*   what hp you have, but for the 'bonus' hp you get */
		/*   getting over 500hp is very important. */
		if (bonus_hp < 500)
			value += bonus_hp * 350L;
		else
			value += (bonus_hp - 500) * 100L + (350L * 500);
	}


	/* Hack -- Reward charisma bonus up to level 25 */
	if (borg_skill[BI_CLEVEL] < 25)
		value += (my_stat_ind[A_CHR] * 2L);



	/* HACK - a small bonus for adding to stats even above max. */
	/*        This will allow us to swap a ring of int +6 for */
	/*        our ring of int +2 even though we are at max int because */
	/*        we are wielding a weapon that has +4 int */
	/*        later it might be nice to swap to a weapon that does not */
	/*        have an int bonus */
	for (i = 0; i < 6; i++) value += my_stat_add[i];


	/*** Reward current skills ***/

	/* Hack -- tiny rewards */
	value += (borg_skill[BI_DIS] * 2L);
	value += (borg_skill[BI_DEV] * 25L);
	value += (borg_skill[BI_SAV] * 25L);
	/* perfect saves are very nice */
	if (borg_skill[BI_SAV] > 99)
		value += 10000;
	value += (borg_skill[BI_STL] * 2L);
	value += (borg_skill[BI_SRCH] * 1L);
	value += (borg_skill[BI_SRCHFREQ] * 1L);
	value += (borg_skill[BI_THN] * 5L);
	value += (borg_skill[BI_THB] * 35L);
	value += (borg_skill[BI_THT] * 2L);
	value += (borg_skill[BI_DIG] * 2L);


	/*** Reward current flags ***/

	/* Various flags */
	if (borg_skill[BI_SDIG]) value += 10L;

	/* Feather Fall if low level is nice */
	if (borg_skill[BI_MAXDEPTH] < 20)
	{
		if (borg_skill[BI_FEATH]) value += 500L;
	}
	else
	{
		if (borg_skill[BI_FEATH]) value += 50;
	}
	if (borg_skill[BI_LITE]) value += 2000L;
	if (borg_skill[BI_ESP])
	{
		if (borg_skill[BI_SINV]) value += 500L;
	}
	else if (borg_skill[BI_SINV]) value += 5000L;

	if (borg_skill[BI_FRACT]) value += 10000L;

	/* after you max out you are pretty safe from drainers. */
	if (borg_skill[BI_MAXCLEVEL] < 50)
	{
		if (borg_skill[BI_HLIFE]) value += 2000L;
	}
	else
	{
		if (borg_skill[BI_HLIFE]) value += 200L;
	}
	if (borg_skill[BI_REG]) value += 2000L;
	if (borg_skill[BI_ESP]) value += 80000L;

	/* Immunity flags */
	if (borg_skill[BI_ICOLD]) value += 25000L;
	if (borg_skill[BI_IELEC]) value += 40000L;
	if (borg_skill[BI_IFIRE]) value += 60000L;
	if (borg_skill[BI_IACID]) value += 80000L;
	/* Warriors need a slight boost for this */
	if ((borg_class == CLASS_WARRIOR ||
		 borg_class == CLASS_CHAOS_WARRIOR) &&
		borg_skill[BI_RFEAR]) value += 2000L;
	if (borg_skill[BI_RFEAR]) value += 2000L;

	/* Resistance flags */
	if (borg_skill[BI_RCOLD]) value += 3000L;
	if (borg_skill[BI_RELEC]) value += 4000L;
	if (borg_skill[BI_RACID]) value += 6000L;
	if (borg_skill[BI_RFIRE]) value += 8000L;
	/* extra bonus for getting all basic resist */
	if (borg_skill[BI_RFIRE] &&
		borg_skill[BI_RACID] &&
		borg_skill[BI_RELEC] && borg_skill[BI_RCOLD]) value += 10000L;
	if (borg_skill[BI_RPOIS]) value += 20000L;
	if (borg_skill[BI_RSND]) value += 3500L;
	if (borg_skill[BI_RLITE]) value += 800L;
	if (borg_skill[BI_RDARK]) value += 800L;
	if (borg_skill[BI_RKAOS]) value += 5000L;

	/* this is way boosted to avoid carrying stuff you don't need */
	if (borg_skill[BI_RCONF]) value += 80000L;

	/* mages need a slight boost for this */
	if (borg_class == CLASS_MAGE && borg_skill[BI_RCONF]) value += 2000L;

	if (borg_skill[BI_RDIS]) value += 5000L;
	if (borg_skill[BI_RSHRD]) value += 100L;
	if (borg_skill[BI_RNXUS]) value += 100L;
	if (borg_skill[BI_RBLIND]) value += 5000L;
	if (borg_skill[BI_RNTHR]) value += 5500L;
	if (borg_skill[BI_REFLECT]) value += 2000L;

	/* Sustain flags */
	if (borg_skill[BI_SSTR]) value += 50L;
	if (borg_skill[BI_SINT]) value += 50L;
	if (borg_skill[BI_SWIS]) value += 50L;
	if (borg_skill[BI_SCON]) value += 50L;
	if (borg_skill[BI_SDEX]) value += 50L;
	/* boost for getting them all */
	if (borg_skill[BI_SSTR] &&
		borg_skill[BI_SINT] &&
		borg_skill[BI_SWIS] &&
		borg_skill[BI_SDEX] && borg_skill[BI_SCON]) value += 1000L;


	/*** XXX XXX XXX Reward "necessary" flags ***/

	/* Mega-Hack -- See invisible (level 10) */
	if ((borg_skill[BI_SINV] || borg_skill[BI_ESP]) &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 10)) value += 100000L;


	/* Mega-Hack -- Free action (level 20) */
	if (borg_skill[BI_FRACT] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 20)) value += 100000L;


	/*  Mega-Hack -- resists (level 25) */
	if (borg_skill[BI_RFIRE] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 25)) value += 100000L;


	/*  Mega-Hack -- resists (level 40) */
	if (borg_skill[BI_RPOIS] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 40)) value += 100000L;
	if (borg_skill[BI_RELEC] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 40)) value += 100000L;
	if (borg_skill[BI_RACID] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 40)) value += 100000L;
	if (borg_skill[BI_RCOLD] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 40)) value += 100000L;


	/* APW Mega-Hack -- Speed / Hold Life (level 46) and maxed out */
	if ((borg_skill[BI_HLIFE] && (borg_skill[BI_MAXDEPTH] + 1 >= 46) &&
		 (borg_skill[BI_MAXCLEVEL] < 50))) value += 100000L;
	if ((borg_skill[BI_SPEED] >= 115) &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 46)) value += 100000L;
	if (borg_skill[BI_RCONF] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 46)) value += 100000L;

	/*  Mega-Hack -- resist Nether is -very- nice to have at level 50 */
	if (borg_skill[BI_RNTHR] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 50)) value += 55000L;

	/*  Mega-Hack -- resist Sound to avoid being KO'd */
	if (borg_skill[BI_RSND] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 50)) value += 100000L;

	/*  Mega-Hack -- resists & Telepathy (level 55) */
	if (borg_skill[BI_RBLIND] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 55)) value += 100000L;
	if (borg_skill[BI_ESP] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 55)) value += 100000L;
	if (borg_skill[BI_RNTHR] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 60)) value += 55000L;


	/*  Mega-Hack -- resists & +10 speed (level 60) */
	if (borg_skill[BI_RKAOS] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 60)) value += 104000L;
	if (borg_skill[BI_RDIS] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 60)) value += 90000L;
	if ((borg_skill[BI_SPEED] >= 120) &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 60)) value += 100000L;

	/*  Must have +20 speed (level 80) */
	if ((borg_skill[BI_SPEED] >= 130) &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 80)) value += 100000L;

	/* Not Req, but a good idea:
	 * Extra boost to Nether deeper down
	 * RDark for deeper uniques
	 * Good to have +30 speed
	 */
	if (borg_skill[BI_RNTHR] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 80)) value += 15000L;
	if (borg_skill[BI_RDARK] &&
		(borg_skill[BI_MAXDEPTH] + 1 >= 80)) value += 25000L;
	if ((borg_skill[BI_SPEED] >= 140) && (borg_skill[BI_MAXDEPTH] + 1 >= 80) &&
		borg_class == CLASS_WARRIOR) value += 100000L;


	/*** Reward powerful armor ***/
	if (borg_skill[BI_ARMOR] < 15) value += borg_skill[BI_ARMOR] * 2000L;
	if ((borg_skill[BI_ARMOR] >= 15) && (borg_skill[BI_ARMOR] < 75))
	{
		value += borg_skill[BI_ARMOR] * 1500L + 28350L;
	}
	if (borg_skill[BI_ARMOR] >= 75) value +=
			borg_skill[BI_ARMOR] * 500L + 73750L;

	/*** Penalize various things ***/

	/* Penalize various flags */
	if (borg_skill[BI_CRSTELE]) value -= 10000L;
	if (borg_skill[BI_CRSAGRV]) value -= 8000L;
	if (borg_skill[BI_CRSNOTELE]) value -= 10000L;
	if (borg_skill[BI_CRSNOMAGIC]) value -= 100000L;

	/*** Penalize armor weight ***/
	if (my_stat_ind[A_STR] < 15)
	{
		l_ptr = look_up_equip_slot(EQUIP_BODY);
		if (l_ptr && (l_ptr->weight > 200)) value -= (l_ptr->weight - 200) * 15;

		l_ptr = look_up_equip_slot(EQUIP_HEAD);
		if (l_ptr && (l_ptr->weight > 30)) value -= 250;

		l_ptr = look_up_equip_slot(EQUIP_ARM);
		if (l_ptr && (l_ptr->weight > 10)) value -= 250;

		l_ptr = look_up_equip_slot(EQUIP_FEET);
		if (l_ptr && (l_ptr->weight > 50)) value -= 250;
	}

	/* Compute the total armor weight */
	for (i = EQUIP_BODY; i <= EQUIP_FEET; i++)
	{
		l_ptr = look_up_equip_slot(i);
		if (l_ptr) cur_wgt += l_ptr->weight;
	}

	/* Determine the weight allowance */
	max_wgt = mp_ptr->spell_weight;

	/* Hack -- heavy armor hurts magic */
	if (borg_skill[BI_INTMANA] &&
		(((cur_wgt - max_wgt) / 10) > 0) &&
		((adj_mag_mana[my_stat_ind[A_INT]] * borg_skill[BI_CLEVEL]) / 2) < 150)
	{
		/* Mega-Hack -- Penalize heavy armor which hurts mana */
		value -= (((cur_wgt - max_wgt) / 10) * 3600L);
	}

	/*** Penalize bad magic ***/

	/* Hack -- most gloves hurt magic for spell-casters */
	if (borg_class == CLASS_MAGE)
	{
		l_ptr = look_up_equip_slot(EQUIP_HANDS);

		/* Penalize non-usable gloves */
		if (l_ptr && (!(l_ptr->kn_flags2 & TR2_FREE_ACT)) &&
			(!((l_ptr->kn_flags1 & TR1_DEX) && (l_ptr->pval > 0))))
		{
			/* Hack -- Major penalty */
			value -= 275000L;
		}
	}

	/* apw Hack -- most edged weapons hurt magic for priests */
	if (borg_class == CLASS_PRIEST)
	{
		l_ptr = look_up_equip_slot(EQUIP_WIELD);

		/* Penalize non-blessed edged weapons */
		if (l_ptr && (((l_ptr->tval == TV_SWORD) ||
					   (l_ptr->tval == TV_POLEARM)) &&
					  !(l_ptr->kn_flags3 & TR3_BLESSED)))
		{
			/* Hack -- Major penalty */
			value -= 75000L;
		}
	}

	/*** Hack -- books ***/

	/* Reward books */
	for (realm = 0; realm < MAX_REALM; realm++)
	{
		/* My realm only */
		if (realm != borg_skill[BI_REALM1] &&
			realm != borg_skill[BI_REALM2]) continue;

		for (book = 0; book < 4; book++)
		{
			/* No copies */
			if (!amt_book[realm][book]) continue;

			/* The "hard" books */
			if (book >= 2)
			{
				/* Reward the book */
				k = 0;
				for (; k < 1 && k < amt_book[realm][book];
					 k++) value += 300000L;
			}

			/* The "easy" books */
			else
			{
				int what, when = 99;

				/* Scan the spells */
				for (what = 0; what < 9; what++)
				{
					borg_magic *as = &borg_magics[realm][book][what];

					/* Track minimum level */
					if (as->level < when) when = as->level;
				}

				/* Hack -- Ignore "difficult" normal books */
				if ((when > 5) &&
					(when >= borg_skill[BI_MAXCLEVEL] + 2)) continue;

				/* Reward the book */
				k = 0;
				for (; k < 1 && k < amt_book[realm][book];
					 k++) value += 500000L;
				if (borg_skill[BI_MAXDEPTH] > 5)
					for (; k < 2 && k < amt_book[realm][book];
						 k++) value += 10000L;
				if (borg_skill[BI_MAXDEPTH] > 50)
					for (; k < 3 && k < amt_book[realm][book];
						 k++) value += 2500L;
			}
		}
	}

	/* Reward for wielded artifacts with multiple high resists */
	for (i = 0; i < equip_num; i++)
	{
		int multibonus = 0;

		l_ptr = look_up_equip_slot(i);

		/* Skip empty items */
		if (!l_ptr) continue;

		/* Good to have one item with multiple high resists */
		multibonus = (((l_ptr->kn_flags2 & TR2_RES_POIS) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_LITE) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_DARK) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_BLIND) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_CONF) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_SOUND) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_SHARDS) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_NEXUS) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_NETHER) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_CHAOS) != 0) +
					  ((l_ptr->kn_flags2 & TR2_RES_DISEN) != 0));

		if (multibonus >= 2) value += 15000 * multibonus;
	}

	/* Result */
	return (value);
}


/*
 * Helper function -- calculate power of inventory
 * Dynamic Calcs off
 */
static s32b borg_power_aux4(void)
{
	int k, book, realm;

	s32b value = 0L;

	/*** Basic abilities ***/

	/* Reward fuel */
	for (k = 0; k < 5 && k < borg_skill[BI_AFUEL]; k++) value += 60000L;
	for (; k < 10 && k < borg_skill[BI_AFUEL]; k++) value += 6000L;

	/* Reward Food */
	/* if hungry, food is THE top priority */
	if ((borg_skill[BI_ISHUNGRY] || borg_skill[BI_ISWEAK]) &&
		borg_skill[BI_FOOD]) value += 100000;

	for (k = 0; k < 5 && k < borg_skill[BI_FOOD]; k++) value += 50000L;
	for (; k < 10 && k < borg_skill[BI_FOOD]; k++) value += 200L;
	if (borg_skill[BI_REG] && !borg_skill[BI_SDIG])
	{
		for (k = 0; k < 10 && k < borg_skill[BI_FOOD]; k++) value += 500L;
	}
	/* Prefere to buy HiCalorie foods over LowCalorie */
	if (amt_food_hical <= 5) value += amt_food_hical * 50;

	/* Reward Cure Poison and Cuts */
	if ((borg_skill[BI_ISCUT] || borg_skill[BI_ISPOISONED]) &&
		borg_skill[BI_ACCW]) value += 100000;
	if ((borg_skill[BI_ISCUT] || borg_skill[BI_ISPOISONED]) &&
		borg_skill[BI_AHEAL]) value += 50000;
	if ((borg_skill[BI_ISCUT] || borg_skill[BI_ISPOISONED]) &&
		borg_skill[BI_ACSW])
	{
		for (k = 0; k < 5 && k < borg_skill[BI_ACSW]; k++) value += 25000L;
	}
	if (borg_skill[BI_ISPOISONED] && borg_skill[BI_ACUREPOIS]) value += 15000;
	if (borg_skill[BI_ISPOISONED] && amt_slow_poison) value += 5000;

	/* Reward Resistance Potions for Warriors */
	if (borg_class == CLASS_WARRIOR)
	{
		for (k = 0; k < 4 && k < borg_skill[BI_ARESHEAT]; k++) value += 500L;
		for (k = 0; k < 4 && k < borg_skill[BI_ARESCOLD]; k++) value += 500L;
	}

	/* Reward ident */
	for (k = 0; k < 10 && k < borg_skill[BI_AID]; k++) value += 6000L;
	for (; k < 15 && k < borg_skill[BI_AID]; k++) value += 600L;

	/*  Reward *id* apw carry lots of these */
	for (k = 0; k < 8 && k < borg_has[177]; k++) value += 10000L;
	for (; k < 15 && k < borg_has[177]; k++) value += 2000L;

	/*  Reward PFE  carry lots of these */
	for (k = 0; k < 10 && k < borg_skill[BI_APFE]; k++) value += 10000L;
	for (; k < 25 && k < borg_skill[BI_APFE]; k++) value += 2000L;

	/*  apw Reward Glyph- Rune of Protection-  carry lots of these */
	for (k = 0; k < 10 && k < borg_skill[BI_AGLYPH]; k++) value += 10000L;
	for (; k < 25 && k < borg_skill[BI_AGLYPH]; k++) value += 2000L;

	/* Reward recall */
	for (k = 0; k < 3 && k < borg_skill[BI_RECALL]; k++) value += 50000L;
	for (; k < 7 && k < borg_skill[BI_RECALL]; k++) value += 5000L;

	/* first phase door is very important */
	if (amt_phase) value += 50000;
	for (k = 1; k < 15 && k < amt_phase; k++) value += 500L;

	/* Reward escape */
	for (k = 0; k < 5 && k < borg_skill[BI_AESCAPE]; k++) value += 10000L;
	if (borg_skill[BI_CDEPTH] > 90)
	{
		for (; k < 15 && k < borg_skill[BI_AESCAPE]; k++) value += 10000L;
	}

	/* Reward teleport */
	for (k = 0; k < 10 && k < borg_skill[BI_ATELEPORT]; k++) value += 10000L;

	/* Reward Teleport Level scrolls */
	if (borg_skill[BI_MAXDEPTH] >= 99)
	{
		for (k = 0; k < 5 && k < borg_skill[BI_ATELEPORTLVL];
			 k++) value += 5000L;
	}


	/*** Healing ***/
	if (borg_class == CLASS_WARRIOR || borg_class == CLASS_ROGUE)
	{
		for (k = 0; k < 15 && k < borg_skill[BI_AHEAL]; k++) value += 8000L;

		for (k = 0; k < 2 && k < borg_has[419]; k++) value += 10000L;
		if (borg_has[419] == 0)
		{
			for (k = 0; k < 2 && k < borg_has[420]; k++) value += 10000L;
		}

		for (k = 0; k < 4 && k < borg_has[374]; k++) value += 20000L;
	}
	else if (borg_class == CLASS_RANGER || borg_class == CLASS_PALADIN ||
			 borg_class == CLASS_MAGE)
	{
		for (k = 0; k < 10 && k < borg_skill[BI_AHEAL]; k++) value += 4000L;

		for (k = 0; k < 2 && k < borg_has[419]; k++) value += 9000L;
		if (borg_has[419] == 0)
		{
			for (k = 0; k < 2 && k < borg_has[420]; k++) value += 9000L;
		}

		if (borg_class == CLASS_PALADIN)
		{
			/* Reward heal potions */
			for (k = 0; k < 3 && k < borg_has[242]; k++) value += 5000L;
		}

	}
	else if (borg_class == CLASS_PRIEST)
	{
		/* Reward heal potions */
		for (k = 0; k < 5 && k < borg_has[242]; k++) value += 2000L;

		for (k = 0; k < 2 && k < borg_has[419]; k++) value += 9000L;
		if (borg_has[419] == 0)
		{
			for (k = 0; k < 2 && k < borg_has[420]; k++) value += 9000L;
		}
	}

	/* Restore Mana */
	if (borg_skill[BI_MAXSP] > 100)
	{
		for (k = 0; k < 10 && k < borg_has[266]; k++) value += 4000L;
		for (k = 0; k < 100 && k < borg_skill[BI_ASTFMAGI]; k++) value += 4000L;
	}

	/* Reward cure critical.  Heavy reward on first 5 */
	if (borg_skill[BI_CLEVEL] < 35 || !borg_skill[BI_RCONF])
	{
		for (k = 0; k < 10 && k < borg_skill[BI_ACCW]; k++) value += 5000L;
		for (; k < 15 && k < borg_skill[BI_ACCW]; k++) value += 500L;
	}
	else
	{
		/* Reward cure critical.  Later on in game. */
		for (k = 0; k < 10 && k < borg_skill[BI_ACCW]; k++) value += 5000L;
	}

	/* Reward cure serious -- only reward serious if low on crits */
	if (borg_skill[BI_ACCW] < 10)
	{
		for (k = 0; k < 5 && k < borg_skill[BI_ACSW]; k++) value += 50L;
		for (; k < 10 && k < borg_skill[BI_ACSW]; k++) value += 5L;
	}

	/* Reward cure serious -- Low Level Characters */
	if (borg_skill[BI_CLEVEL] < 15)
	{
		for (k = 0; k < 5 && k < borg_skill[BI_ACSW]; k++) value += 250L;
		for (; k < 10 && k < borg_skill[BI_ACSW]; k++) value += 55L;
	}

	/* Reward Cures */
	if (!borg_skill[BI_RCONF])
	{
		for (k = 0; k < 10 && k < amt_cure_confusion; k++) value += 400L;
	}
	if (!borg_skill[BI_RBLIND])
	{
		for (k = 0; k < 5 && k < amt_cure_blind; k++) value += 300L;
	}
	if (!borg_skill[BI_RPOIS])
	{
		for (k = 0; k < 5 && k < borg_skill[BI_ACUREPOIS]; k++) value += 250L;
	}

	/*** Detection ***/

	/* Reward detect trap */
	for (k = 0; k < 1 && k < borg_skill[BI_ADETTRAP]; k++) value += 4000L;

	/* Reward detect door */
	for (k = 0; k < 1 && k < borg_skill[BI_ADETDOOR]; k++) value += 2000L;

	/* Reward detect evil */
	if (!borg_skill[BI_ESP])
	{
		for (k = 0; k < 1 && k < borg_skill[BI_ADETEVIL]; k++) value += 1000L;
	}

	/* Reward magic mapping */
	for (k = 0; k < 1 && k < borg_skill[BI_AMAGICMAP]; k++) value += 4000L;

	/* Genocide scrolls. Just scrolls, mainly used for Morgoth */
	if (borg_skill[BI_MAXDEPTH] >= 98)
	{
		for (k = 0; k < 10 && k < borg_has[207]; k++) value += 10000L;
		for (; k < 25 && k < borg_has[207]; k++) value += 2000L;
	}

	/* Mass Genocide scrolls. Just scrolls, mainly used for Morgoth */
	if (borg_skill[BI_MAXDEPTH] >= 98)
	{
		for (k = 0; k < 10 && k < borg_has[200]; k++) value += 10000L;
		for (; k < 25 && k < borg_has[200]; k++) value += 2000L;
	}

	/* Reward speed potions/staves */
	if (borg_skill[BI_MAXDEPTH] <= 98)
	{
		for (k = 0; k < 20 && k < borg_skill[BI_ASPEED]; k++) value += 5000L;
	}

	/* Invuln Potions, mainly used for Morgoth */
	if (borg_skill[BI_MAXDEPTH] >= 98)
	{
		for (k = 0; k < 15 && k < borg_has[238]; k++) value += 10000L;
		for (; k < 99 && k < borg_has[238]; k++) value += 2000L;
	}

	/* Reward Recharge ability */
	for (k = 0; k < 5 && k < borg_skill[BI_ARECHARGE]; k++) value += 2000L;

	/*** Missiles ***/

	/* Reward missiles */
	if (borg_class == CLASS_RANGER)
	{
		for (k = 0; k < 30 && k < borg_skill[BI_AMISSILES]; k++) value += 1000L;
		for (; k < 80 && k < borg_skill[BI_AMISSILES]; k++) value += 100L;
	}
	else
	{
		for (k = 0; k < 20 && k < borg_skill[BI_AMISSILES]; k++) value += 1000L;
		for (; k < 50 && k < borg_skill[BI_AMISSILES]; k++) value += 100L;
	}

	/*** Various ***/

	/* These staves are great but do not clutter inven with them */
	/*  -- Reward carrying a staff of holiness/power */
	if (amt_cool_staff) value += 2500L;
	value += amt_cool_staff > 3 ? 1500 : 500 * amt_cool_staff;

	/*  -- Reward carrying a staff of destruction. */
	if (borg_has[307]) value += 5000L;
	value += borg_has[307] > 3 ? 600 : 200 * borg_has[307];

	/* Hack -- Reward add stat */
	if (amt_add_stat[A_STR]) value += 50000;
	if (amt_add_stat[A_INT]) value += 20000;
	if (borg_skill[BI_INTMANA])
	{
		if (amt_add_stat[A_INT]) value += 50000;
	}

	if (amt_add_stat[A_WIS]) value += 20000;
	if (borg_skill[BI_WISMANA])
	{
		if (amt_add_stat[A_WIS]) value += 50000;
	}
	if (amt_add_stat[A_DEX]) value += 50000;
	if (amt_add_stat[A_CON]) value += 50000;
	if (amt_add_stat[A_CHR]) value += 10000;

	/* Hack -- Reward fix stat */
	if (amt_fix_stat[A_STR]) value += 10000;
	if (amt_fix_stat[A_INT]) value += 10000;
	if (amt_fix_stat[A_WIS]) value += 10000;
	if (amt_fix_stat[A_DEX]) value += 10000;
	if (amt_fix_stat[A_CON]) value += 10000;
	if (amt_fix_stat[A_CHR]) value += 10000;

	/* Reward Remove Curse */
	if (borg_wearing_cursed)
	{
		if (borg_has[191]) value += 90000;
		if (borg_has[180]) value += 90000;
	}

	/* Hack -- Restore experience */
	if (amt_fix_exp) value += 500000;

	/*** Enchantment ***/

	/* Reward enchant armor */
	if (amt_enchant_to_a && my_need_enchant_to_a) value += 14L;

	/* Reward enchant weapon to hit */
	if (amt_enchant_to_h && my_need_enchant_to_h) value += 24L;

	/* Reward enchant weapon to damage */
	if (amt_enchant_to_d && my_need_enchant_to_d) value += 109L;

	/* Reward *enchant weapon* to damage */
	if (amt_enchant_weapon) value += 5000L;

	/* Reward *enchant armour*  */
	if (amt_enchant_armor) value += 5000L;

	/*** Hack -- books ***/

	/* Reward books */
	for (realm = 0; realm < MAX_REALM; realm++)
	{
		/* My realm only */
		if (realm != borg_skill[BI_REALM1] &&
			realm != borg_skill[BI_REALM2]) continue;

		for (book = 0; book < 4; book++)
		{
			/* No copies */
			if (!amt_book[realm][book]) continue;

			/* The "hard" books */
			if (book >= 2)
			{
				/* Reward the book */
				for (k = 0; k < 1 && k < amt_book[realm][book]; k++)
				{
					value += 300000L;
				}
			}

			/* The "easy" books */
			else
			{
				int what, when = 99;

				/* Scan the spells */
				for (what = 0; what < 9; what++)
				{
					borg_magic *as = &borg_magics[realm][book][what];

					/* Track minimum level */
					if (as->level < when) when = as->level;
				}

				/* Hack -- Ignore "difficult" normal books */
				if ((when > 5) &&
					(when >= borg_skill[BI_MAXCLEVEL] + 2)) continue;

				/* Reward the book */
				for (k = 0; k < 1 && k < amt_book[realm][book]; k++)
				{
					value += 500000L;
				}
				if (borg_skill[BI_MAXDEPTH] > 5)
				{
					for (; k < 2 && k < amt_book[realm][book]; k++)
					{
						value += 10000L;
					}
				}
				if (borg_skill[BI_MAXDEPTH] > 50)
				{
					for (; k < 3 && k < amt_book[realm][book]; k++)
					{
						value += 2500L;
					}
				}
			}
		}
	}

	/* Hack -- Apply "encumbrance" from weight */
	if (borg_skill[BI_ENCUMBERD])
	{
		value -= (borg_skill[BI_ENCUMBERD] * 500L);
	}

	/* Being too heavy is really bad */
	value -= borg_skill[BI_WEIGHT] * borg_skill[BI_WEIGHT]
		/ (adj_str_wgt[my_stat_ind[A_STR]] * adj_str_wgt[my_stat_ind[A_STR]]);


	/* Reward empty slots */
	if (INVEN_PACK - inven_num < 5)
	{
		value += 400L * (INVEN_PACK - inven_num);
	}
	else
	{
		value += 400L * 5;
	}

	/* Return the value */
	return (value);
}


/*
 * Calculate the "power" of the Borg
 */
s32b borg_power(void)
{
	int i = 1;
	s32b value = 0L;

	/* Process the equipment */
	value  += borg_power_aux3();
	
	/* Process the inventory */
	value += borg_power_aux4();

	/* Add a bonus for deep level prep */
	/* Dump prep codes */
	for (i = 1; i <= borg_skill[BI_MAXDEPTH] + 10; i++)
	{
		/* Dump fear code */
		if ((cptr)NULL != borg_prepared(i)) break;
	}
	value += ((i - 1) * 20000L);

	/* Return the value */
	return (value);
}



/*
 * Determine if the Borg is out of "crucial" supplies.
 *
 * Note that we ignore "restock" issues for the first several turns
 * on each level, to prevent repeated "level bouncing".
 */
cptr borg_restock(int depth)
{
	/* Always ready for the town */
	if (!depth) return ((cptr)NULL);

	/* Always spend time on a level unless 100 */
	if (borg_t - borg_began < 100 &&
		borg_skill[BI_CDEPTH] != 100) return ((cptr)NULL);


	/*** Level 1 ***/

	/* Must have some lite */
	if (borg_skill[BI_CUR_LITE] < 1) return ("rs my_cur_lite");

	/* Must have "fuel" */
	if (borg_skill[BI_AFUEL] < 1) return ("rs amt_fuel");

	/* Must have "food" */
	if (borg_skill[BI_FOOD] < 1) return ("rs amt_food");

	/* Assume happy at level 1 */
	if (depth <= 1) return ((cptr)NULL);

	/*** Level 2 and 3 ***/

	/* Must have good lite */
	if (borg_skill[BI_CUR_LITE] < 2) return ("rs lite+1");

	/* Must have "fuel" */
	if (borg_skill[BI_AFUEL] < 3) return ("rs fuel+2");

	/* Must have "food" */
	if (borg_skill[BI_FOOD] < 3) return ("rs food+2");

	/* Must have "recall" */
	if (borg_skill[BI_RECALL] < 2) return ("rs recall");

	/* Assume happy at level 3 */
	if (depth <= 3) return ((cptr)NULL);

	/*** Level 3 to 5 ***/

	if (depth <= 5) return ((cptr)NULL);

	/*** Level 6 to 9 ***/

	/* Potions of Critical Wounds */
	if (borg_skill[BI_ACCW] < 1 &&
		(!borg_skill[BI_RBLIND] ||
		 !borg_skill[BI_RCONF])) return ("rs cure crit");

	/* Assume happy at level 9 */
	if (depth <= 9) return ((cptr)NULL);

	/*** Level 10 - 19  ***/

	/* Must have "phase" */
	if (amt_phase < 1) return ("rs phase");

	/* Must have "cure" */
	if ((borg_skill[BI_MAXCLEVEL] < 30) &&
		borg_skill[BI_ACSW] + borg_skill[BI_ACCW] < 4) return ("rs cure");

	/* Must have "teleport" */
	if (borg_skill[BI_ATELEPORT] < 2) return ("rs teleport");

	/* Assume happy at level 19 */
	if (depth <= 19) return ((cptr)NULL);

	/*** Level 20 - 45  ***/

	/* Must have "cure" */
	if ((borg_skill[BI_MAXCLEVEL] < 30) &&
		borg_skill[BI_ACSW] + borg_skill[BI_ACCW] < 6) return ("rs cure");

	/* Must have "teleport" */
	if (borg_skill[BI_ATELEPORT] + borg_skill[BI_AESCAPE] <
		4) return ("rs teleport");

	/* Assume happy at level 44 */
	if (depth <= 44) return ((cptr)NULL);

	/*** Level 46 - 99  ***/

	/* Must have "Heal" */
	if (borg_skill[BI_AHEAL] + borg_has[374] + borg_skill[BI_AEZHEAL] <
		1) return ("rs heal");

	/* Assume happy at level 99 */
	if (depth <= 99) return ((cptr)NULL);

	/*** Level 100  ***/

	/* Must have "Heal" */
	/* If I just got to dlevel 100 and low on heals, get out now. */
	if (borg_t - borg_began < 10 &&
		borg_skill[BI_AEZHEAL] < 15) return ("rs *heal*");

	/* Assume happy */
	return ((cptr)NULL);
}


/*
 * Determine if the Borg meets the "minimum" requirements for a level
 */
static cptr borg_prepared_aux2(int depth)
{
	/* Always ready for the town */
	if (!depth) return ((cptr)NULL);


	/*** Essential Items for Level 1 ***/

	/* Require lite (any) */
	if (borg_skill[BI_CUR_LITE] < 1) return ("1 Lite");

	/* Require food */
	if (borg_skill[BI_FOOD] < 5) return ("5 Food");

	/* Usually ready for level 1 */
	if (depth <= 1) return ((cptr)NULL);


	/*** Essential Items for Level 2 ***/

	/* Require lite (radius two) */
	if (borg_skill[BI_CUR_LITE] < 2) return ("2 Lite");

	/* Require fuel */
	if (borg_skill[BI_AFUEL] < 5) return ("5 Fuel");

	/* Require recall */
	if (borg_skill[BI_RECALL] < 1) return ("1 recall");

	if (!borg_plays_risky)
	{
		/* Require 30 hp */
		if (borg_skill[BI_MAXHP] < 30) return ("30 hp");
	}

	/* Usually ready for level 2 */
	if (depth <= 2) return ((cptr)NULL);

	/*** Essential Items for Level 3 and 4 ***/

	if (!borg_plays_risky)
	{
		/* class specific requirement */
		switch (borg_class)
		{
			case CLASS_WARRIOR:
			{
				if (borg_skill[BI_MAXHP] < 50) return ("50 hp");
				if (borg_skill[BI_MAXCLEVEL] < 4) return ("4 clevel");
				break;
			}
			case CLASS_ROGUE:
			{
				if (borg_skill[BI_MAXHP] < 50) return ("50 hp");
				if (borg_skill[BI_MAXCLEVEL] < 8) return ("8 clevel");
				break;
			}
			case CLASS_PRIEST:
			{
				if (borg_skill[BI_MAXHP] < 40) return ("40 hp");
				if (borg_skill[BI_MAXCLEVEL] < 9) return ("9 level");
				break;
			}
			case CLASS_PALADIN:
			{
				if (borg_skill[BI_MAXHP] < 50) return ("50 hp");
				if (borg_skill[BI_MAXCLEVEL] < 4) return ("4 clevel");
				break;
			}
			case CLASS_RANGER:
			{
				if (borg_skill[BI_MAXHP] < 50) return ("50 hp");
				if (borg_skill[BI_MAXCLEVEL] < 4) return ("4 clevel");
				break;
			}
			case CLASS_MAGE:
			{
				if (borg_skill[BI_MAXHP] < 60) return ("60 hp");
				if (borg_skill[BI_MAXCLEVEL] < 11) return ("11 clevel");
				break;
			}
			default:
			{
				if (borg_skill[BI_MAXHP] < 50) return ("50 hp");
				if (borg_skill[BI_MAXCLEVEL] < 8) return ("8 clevel");
				break;
			}
		}
	}

	/* Scrolls of Word of Recall */
	if (borg_skill[BI_RECALL] < 3) return ("3 recall");

	/* Potions of Cure Serious Wounds */
	if ((borg_skill[BI_MAXCLEVEL] < 30) &&
		borg_skill[BI_ACSW] + borg_skill[BI_ACCW] < 2) return ("2 cure");

	/* Usually ready for level 3 and 4 */
	if (depth <= 4) return ((cptr)NULL);


	/*** Essential Items for Level 5 to 9 ***/

	if (!borg_plays_risky)
	{
		/* class specific requirement */
		if (borg_skill[BI_CDEPTH])
		{
			switch (borg_class)
			{
				case CLASS_WARRIOR:
				{
					if (borg_skill[BI_MAXHP] < 60) return ("60 hp");
					if (borg_skill[BI_MAXCLEVEL] < 6) return ("6 clevel");
					break;
				}
				case CLASS_ROGUE:
				{
					if (borg_skill[BI_MAXHP] < 60) return ("60 hp");
					if (borg_skill[BI_MAXCLEVEL] < 10) return ("10 clevel");
					break;
				}
				case CLASS_PRIEST:
				{
					if (borg_skill[BI_MAXHP] < 60) return ("60 hp");
					if (borg_skill[BI_MAXCLEVEL] < 15) return ("15 clevel");
					break;
				}
				case CLASS_PALADIN:
				{
					if (borg_skill[BI_MAXHP] < 60) return ("60 hp");
					if (borg_skill[BI_MAXCLEVEL] < 6) return ("6 clevel");
					break;
				}
				case CLASS_RANGER:
				{
					if (borg_skill[BI_MAXHP] < 60) return ("60 hp");
					if (borg_skill[BI_MAXCLEVEL] < 6) return ("6 clevel");
					break;
				}
				case CLASS_MAGE:
				{
					if (borg_skill[BI_MAXHP] < 80) return ("80 hp");
					if (borg_skill[BI_MAXCLEVEL] < 15) return ("15 level");
					break;
				}
				default:
				{
					if (borg_skill[BI_MAXHP] < 60) return ("60 hp");
					if (borg_skill[BI_MAXCLEVEL] < 10) return ("10 clevel");
					break;
				}
			}
		}
	}
	/* Scrolls of Word of Recall */
	if (borg_skill[BI_RECALL] < 4) return ("4 recalls");

	/* Potions of Cure Serious/Critical Wounds */
	if ((borg_skill[BI_MAXCLEVEL] < 30) &&
		borg_skill[BI_ACSW] + borg_skill[BI_ACCW] < 5) return ("5 cures");

	/* Usually ready for level 5 to 9 */
	if (depth <= 9) return ((cptr)NULL);

	/*** Essential Items for Level 10 to 19 ***/


	/* Escape or Teleport */
	if (borg_skill[BI_ATELEPORT] + borg_skill[BI_AESCAPE] <
		2) return ("2 teleports");

	if (!borg_plays_risky)
	{
		/* class specific requirement */
		switch (borg_class)
		{
			case CLASS_WARRIOR:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth - 4) && depth <= 19)
					return ("dlevel - 4 >= clevel");
				break;
			}
			case CLASS_ROGUE:
			{
				if (borg_skill[BI_MAXCLEVEL] < depth &&
					depth <= 19) return ("dlevel >= clevel");
				break;
			}
			case CLASS_PRIEST:
			{
				if (borg_skill[BI_MAXCLEVEL] < depth &&
					depth <= 19) return ("dlevel >= clevel");
				break;
			}
			case CLASS_PALADIN:
			{
				if (borg_skill[BI_MAXCLEVEL] < depth &&
					depth <= 19) return ("dlevel >= clevel");
				break;
			}
			case CLASS_RANGER:
			{
				if (borg_skill[BI_MAXCLEVEL] < depth &&
					depth <= 19) return ("dlevel >= clevel");
				break;
			}
			case CLASS_MAGE:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth + 5) &&
					borg_skill[BI_MAXCLEVEL] <= 28)
					return ("dlevel + 5 > = clevel");
				break;
			}
			default:
			{
				if (borg_skill[BI_MAXCLEVEL] < depth &&
					depth <= 19) return ("dlevel >= clevel");
				break;
			}
		}
	}

	/* Potions of Cure Critical Wounds */
	if ((borg_skill[BI_MAXCLEVEL] < 30) &&
		borg_skill[BI_ACCW] < 5) return ("cure crit5");

	/* See invisible */
	/* or telepathy */
	if ((!borg_skill[BI_SINV] &&
		 !borg_skill[BI_ESP])) return ("See Invis : ESP");

	/* Usually ready for level 10 to 19 */
	if (depth <= 19) return ((cptr)NULL);


	/*** Essential Items for Level 20 ***/


	/* Free action */
	if (!borg_skill[BI_FRACT]) return ("FA");

	/* ready for level 20 */
	if (depth <= 20) return ((cptr)NULL);


	/*** Essential Items for Level 25 ***/

	/* must have fire + 2 other basic resists */
	if (!borg_skill[BI_SRFIRE]) return ("RF");
	{
		int basics =
			borg_skill[BI_RACID] + borg_skill[BI_RCOLD] + borg_skill[BI_RELEC];

		if (basics < 2) return ("basic resist2");
	}
	/* have some minimal stats */
	if (borg_stat[A_STR] < 7) return ("low STR");

	if (borg_skill[BI_INTMANA])
	{
		if (borg_stat[A_INT] < 7) return ("low INT");
	}
	if (borg_skill[BI_WISMANA])
	{
		if (borg_stat[A_WIS] < 7) return ("low WIS");
	}
	if (borg_stat[A_DEX] < 7) return ("low DEX");
	if (borg_stat[A_CON] < 7) return ("low CON");

	if (!borg_plays_risky)
	{
		/* class specific requirement */
		switch (borg_class)
		{
			case CLASS_WARRIOR:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth + 5) &&
					borg_skill[BI_MAXCLEVEL] <= 38)
					return ("dlevel + 5 >= clevel");
				break;
			}
			case CLASS_ROGUE:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth + 10) &&
					borg_skill[BI_MAXCLEVEL] <= 43)
					return ("dlevel + 10 >= clevel");
				break;
			}
			case CLASS_PRIEST:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth + 13) &&
					borg_skill[BI_MAXCLEVEL] <= 46)
					return ("dlevel + 13 >= clevel");
				break;
			}
			case CLASS_PALADIN:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth + 7) &&
					borg_skill[BI_MAXCLEVEL] <= 40)
					return ("dlevel + 7 >= clevel");
				break;
			}
			case CLASS_RANGER:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth + 8) &&
					borg_skill[BI_MAXCLEVEL] <= 41 &&
					borg_skill[BI_MAXCLEVEL] > 28)
					return ("dlevel + 8 >= clevel");
				break;
			}
			case CLASS_MAGE:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth + 8) &&
					borg_skill[BI_MAXCLEVEL] <= 38)
					return ("dlevel + 8 >= clevel");
				if (((borg_skill[BI_MAXCLEVEL] - 38) * 2 + 30) < depth &&
					borg_skill[BI_MAXCLEVEL] <= 44 &&
					borg_skill[BI_MAXCLEVEL] > 38)
					return ("(clevel-38)*2+30 < dlevel");
				break;
			}
			default:
			{
				if (borg_skill[BI_MAXCLEVEL] < (depth + 10) &&
					borg_skill[BI_MAXCLEVEL] <= 43)
					return ("dlevel + 10 >= clevel");
				break;
			}
		}
	}

	/* Ready for level 25 */
	if (depth <= 25) return ((cptr)NULL);


	/*** Essential Items for Level 25 to 39 ***/


	/* Escape and Teleport */
	if (borg_skill[BI_ATELEPORT] < 2) return ("teleport2");
	if (borg_skill[BI_ATELEPORT] + borg_skill[BI_AESCAPE] <
		6) return ("tell&esc6");

	/* Cure Critical Wounds */
	if ((borg_skill[BI_MAXCLEVEL] < 30) &&
		(borg_skill[BI_ACCW] + borg_skill[BI_ACSW]) < 10) return ("cure10");

	/* Ready for level 33 */
	if (depth <= 33) return ((cptr)NULL);

	/* Minimal level */
	if (borg_skill[BI_MAXCLEVEL] < 40) return ("level 40");

	/* Usually ready for level 20 to 39 */
	if (depth <= 39) return ((cptr)NULL);



	/*** Essential Items for Level 40 to 45 ***/

	/* All Basic resistance & poison */
	if (!borg_skill[BI_SRCOLD]) return ("RC");
	if (!borg_skill[BI_SRELEC]) return ("RE");
	if (!borg_skill[BI_SRACID]) return ("RA");
	if (!borg_skill[BI_SRPOIS]) return ("RPois");

	if (borg_stat[A_STR] < 16) return ("low STR");

	if ((borg_skill[BI_REALM1] >= REALM_SORCERY ||
		 borg_skill[BI_REALM2] >= REALM_SORCERY))
	{
		if (borg_stat[A_INT] < 16) return ("low INT");
	}
	if ((borg_skill[BI_REALM1] == REALM_LIFE ||
		 borg_skill[BI_REALM2] == REALM_LIFE))
	{
		if (borg_stat[A_WIS] < 16) return ("low WIS");
	}
	if (borg_stat[A_DEX] < 16) return ("low DEX");
	if (borg_stat[A_CON] < 16) return ("low CON");

	if (!borg_plays_risky)
	{
		/* Minimal level */
		if (borg_skill[BI_MAXCLEVEL] < 45) return ("level45");
	}

	if (depth <= 45) return ((cptr)NULL);


	/*** Essential Items for Level 46 to 55 ***/

	/*  Must have +5 speed after level 46 */
	if (borg_skill[BI_SPEED] < 115) return ("+5 speed");

	/* Potions of heal */
	if (borg_skill[BI_AHEAL] < 1 &&
		(borg_skill[BI_AEZHEAL] < 1)) return ("1heal");

	if (!borg_skill[BI_SRCONF]) return ("RConf");

	if (!borg_plays_risky)
	{
		/* Minimal hitpoints */
		if (borg_skill[BI_MAXHP] < 500) return ("HP 500");
	}

	/* High stats XXX XXX XXX */
	if (borg_stat[A_STR] < 18 + 40) return ("low STR");

	if ((borg_skill[BI_REALM1] >= REALM_SORCERY ||
		 borg_skill[BI_REALM2] >= REALM_SORCERY))
	{
		if (borg_stat[A_INT] < 18 + 100) return ("low INT");
	}
	if ((borg_skill[BI_REALM1] == REALM_LIFE ||
		 borg_skill[BI_REALM2] == REALM_LIFE))
	{
		if (borg_stat[A_WIS] < 18 + 100) return ("low WIS");
	}
	if (borg_stat[A_DEX] < 18 + 60) return ("low DEX");
	if (borg_stat[A_CON] < 18 + 60) return ("low CON");

	/* Hold Life */
	if (!borg_skill[BI_HLIFE] &&
		(borg_skill[BI_MAXCLEVEL] < 50)) return ("hold life");

	/* Usually ready for level 46 to 55 */
	if (depth <= 55) return ((cptr)NULL);

	/*** Essential Items for Level 55 to 59 ***/

	/* Potions of heal */
	if (borg_skill[BI_AHEAL] < 2 &&
		borg_skill[BI_AEZHEAL] < 1) return ("2heal");

	/* Resists */
	if (!borg_skill[BI_SRBLIND]) return ("RBlind");

	/* Must have resist nether */
	if (borg_skill[BI_AXGOI])
	{
		/* this mage will not require RNethr to dive */
	}
	else
	{
		if (!borg_skill[BI_SRNTHR]) return ("RNeth");
	}

	/* Telepathy, better have it by now */
	if (!borg_skill[BI_ESP]) return ("ESP");

	/* Usually ready for level 55 to 59 */
	if (depth <= 59) return ((cptr)NULL);



	/*** Essential Items for Level 61 to 80 ***/

	/* Must have +10 speed */
	if (borg_skill[BI_SPEED] < 120) return ("+10 speed");


	/* Resists */
	if (borg_skill[BI_AXGOI])
	{
		/* This mage will not require Chaos to dive */
	}
	else
	{
		if (!borg_skill[BI_SRKAOS]) return ("RChaos");
	}
	if (!borg_skill[BI_SRDIS]) return ("RDisen");

	/* Usually ready for level 61 to 80 */
	if (depth <= 80) return ((cptr)NULL);

	/*** Essential Items for Level 81-85 ***/
	/* Minimal Speed */
	if (borg_skill[BI_SPEED] < 130) return ("+20 Speed");

	/* Usually ready for level 81 to 85 */
	if (depth <= 85) return ((cptr)NULL);


	/*** Essential Items for Level 86-99 ***/


	/* Usually ready for level 86 to 99 */
	if (depth <= 99) return ((cptr)NULL);

	/*** Essential Items for Level 100 ***/

	/* must have lots of restore mana to go after MORGOTH */
	if (!borg_skill[BI_KING])
	{
		if ((borg_skill[BI_MAXSP] > 100) &&
			(borg_has[266] < 15)) return ("15ResMana");

		/* must have lots of heal */
		if (borg_has[242] < 15 &&
			(borg_class == CLASS_MAGE ||
			 borg_class == CLASS_PRIEST)) return ("15Heal");
		else if (borg_has[242] < 25) return ("25Heal");

		/* must have lots of ez-heal */
		if (borg_skill[BI_AEZHEAL] < 15) return ("15EZHeal");

		/* must have lots of speed */
		if (borg_skill[BI_ASPEED] < 15) return ("15Speed");

	}

	/* Its good to be the king */
	if (depth <= 127) return ((cptr)NULL);

	/* all bases covered */
	return ((cptr)NULL);
}





/*
 * Determine if the Borg is "prepared" for the given level
 *
 * This routine does not help him decide how to get ready for the
 * given level, so it must work closely with "borg_power()".
 *
 * Note that we ignore any "town fear", and we allow fear of one
 * level up to and including the relevant depth.
 *
 * This now returns a string with the reason you are not prepared.
 *
 */
cptr borg_prepared(int depth)
{
	cptr reason;

	/* Town and First level */
	if (depth == 1) return (NULL);

	/* Not prepared if I need to restock */
	if ((reason = borg_restock(depth))) return (reason);

	/* Must meet minimal requirements */
	if ((reason = borg_prepared_aux2(depth))) return (reason);

	/* Always okay */
	return (NULL);
}

/*
 * Initialize this file
 */
void borg_init_5(void)
{
	/* Do nothing? */
}




#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
