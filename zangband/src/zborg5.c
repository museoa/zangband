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
 * Calculate base danger from a monster's physical attacks
 *
 * We attempt to take account of various resistances, both in
 * terms of actual damage, and special effects, as appropriate.
 *
 * We reduce the danger from distant "sleeping" monsters.
 * apw. PFE reduces my fear of an area.
 */
static int borg_danger_aux1(int r_idx)
{
	int k, n = 0;
	int pfe = 0;
	int power, chance;

	s16b ac = borg_skill[BI_ARMOR];

	monster_race *r_ptr = &r_info[r_idx];

	/* goi gives +100 to ac and deflects almost all missiles and balls */
	if (borg_goi)
		ac += 100;

	/* shields gives +50 to ac and deflects some missiles and balls */
	if (borg_shield)
		ac += 50;

	/* apw PFE gives a protection.  */
	/* Hack -- Apply "protection from evil" */
	if ((borg_prot_from_evil) &&
		(r_ptr->flags3 & RF3_EVIL) && ((borg_skill[BI_CLEVEL]) >= r_ptr->level))
	{
		pfe = 1;
	}

	/* Analyze each physical attack */
	for (k = 0; k < 4; k++)
	{
		int z = 0;

		monster_blow *b_ptr = &r_ptr->blow[k];

		power = 0;

		/* Done */
		if (!b_ptr->method) break;

		/* Analyze the attack */
		switch (b_ptr->effect)
		{
			case RBE_HURT:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				z -= (z * ((ac < 150) ? ac : 150) / 250);
				/* if invulnurable (or PFE), no damage (carried through) */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if ((pfe) && !borg_attacking)
					z /= 2;
				/* stun */
				if ((b_ptr->d_side < 3) && (z > b_ptr->d_dice * b_ptr->d_side))
					n += 200;
				/* fudge- only mystics kick and they tend to KO.  Avoid close */
				/* combat like the plauge */
				if (b_ptr->method == RBM_KICK)
				{
					/* If GOI is on, take that into account */
					n += borg_goi ? 400 * 20 : 400;
				}
				power = 60;
				break;
			}

			case RBE_POISON:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 5;
				if (borg_skill[BI_RPOIS]) break;
				if (my_oppose_pois) break;
				if (!borg_full_damage)
					z += 10;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_UN_BONUS:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 20;
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_RDIS]) break;
				/* if invulnurable, no damage */
				if (!borg_full_damage)
					z += 500;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_UN_POWER:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (!borg_full_damage)
					z += 20;
				if ((pfe) && !borg_attacking)
					z /= 2;
				power = 15;
				break;
			}

			case RBE_EAT_GOLD:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if in town and low level avoid them stupid urchins */
				if (borg_skill[BI_CLEVEL] < 5) z += 50;
				/* if invulnurable, no damage */
				power = 5;
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (100 <=
					adj_dex_safe[my_stat_ind[A_DEX]] +
					borg_skill[BI_CLEVEL]) break;
				if (borg_gold < 100) break;
				if (borg_gold > 100000) break;
				if (!borg_full_damage)
					z += 5;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_EAT_ITEM:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				power = 5;
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (100 <=
					adj_dex_safe[my_stat_ind[A_DEX]] +
					borg_skill[BI_CLEVEL]) break;
				if (!borg_full_damage)
					z += 20;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_EAT_FOOD:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 5;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_FOOD] > 5) break;
				if (!borg_full_damage)
					z += 5;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_EAT_LITE:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 5;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_CUR_LITE] == 0) break;
				if (borg_skill[BI_AFUEL] > 5) break;
				if (!borg_full_damage)
					z += 20;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_ACID:
			{
				if (borg_skill[BI_IACID]) break;
				z = (b_ptr->d_dice * b_ptr->d_side);
				if (borg_skill[BI_RACID]) z = (z + 2) / 3;
				if (my_oppose_acid) z = (z + 2) / 3;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (!borg_full_damage)
					z += 200;	/* We dont want our armour corroded. */
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_ELEC:
			{
				if (borg_skill[BI_IELEC]) break;
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 10;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_RELEC]) z = (z + 2) / 3;
				if (my_oppose_elec) z = (z + 2) / 3;
				if (!borg_full_damage)
					z += 10;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_FIRE:
			{
				if (borg_skill[BI_IFIRE]) break;
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 10;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_RFIRE]) z = (z + 2) / 3;
				if (my_oppose_fire) z = (z + 2) / 3;
				if (!borg_full_damage)
					z += 20;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_COLD:
			{
				if (borg_skill[BI_ICOLD]) break;
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 10;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_RCOLD]) z = (z + 2) / 3;
				if (my_oppose_cold) z = (z + 2) / 3;
				if (!borg_full_damage)
					z += 15;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_BLIND:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 2;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_RBLIND]) break;
				if (!borg_full_damage)
					z += 10;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_CONFUSE:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 10;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_RCONF]) break;
				if (!borg_full_damage)
					z += 200;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_TERRIFY:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 10;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_RFEAR]) break;
				if (!borg_full_damage)
					z += 10;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_PARALYZE:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 2;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_FRACT]) break;
				z += 200;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_LOSE_STR:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z /= 25;
				if (borg_skill[BI_SSTR]) break;
				if (borg_stat[A_STR] <= 3) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3)) break;
				z += 150;
				/* extra scary to have str drain below 10 */
				if (borg_stat[A_STR] < 10)
					z += 350;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_LOSE_DEX:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z /= 25;
				if (borg_skill[BI_SDEX]) break;
				if (borg_stat[A_DEX] <= 3) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3)) break;
				z += 150;
				/* extra scary to have drain below 10 */
				if (borg_stat[A_DEX] < 10)
					z += 350;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_LOSE_CON:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z /= 25;
				if (borg_skill[BI_SCON]) break;
				if (borg_stat[A_CON] <= 3) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3)) break;
				if (!borg_full_damage)
					z += 150;
				/* extra scary to have con drain below 8 */
				if (borg_stat[A_STR] < 8)
					z += 350;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_LOSE_INT:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z /= 25;
				if (borg_skill[BI_SINT]) break;
				if (borg_stat[A_INT] <= 3) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3)) break;
				z += 150;
				/* extra scary for spell caster */
				if (borg_skill[BI_INTMANA])
					z += 350;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_LOSE_WIS:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z /= 25;
				if (borg_skill[BI_SWIS]) break;
				if (borg_stat[A_WIS] <= 3) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3)) break;
				z += 150;
				/* extra scary for pray'er */
				if (borg_skill[BI_WISMANA])
					z += 350;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_LOSE_CHR:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z /= 25;
				if (borg_skill[BI_SCHR]) break;
				if (borg_stat[A_CHR] <= 3) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3)) break;
				z += 50;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_LOSE_ALL:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				power = 2;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z /= 25;
				/* only morgoth. HACK to make it easier to fight him */
				break;
			}

			case RBE_SHATTER:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				z -= (z * ((ac < 150) ? ac : 150) / 250);
				power = 60;
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (!borg_full_damage)
					z += 150;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_EXP_10:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_HLIFE]) break;
				/* do not worry about drain exp after level 50 */
				if (borg_skill[BI_CLEVEL] == 50) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3) ||
					borg_spell_legal(REALM_DEATH, 1, 7)) break;
				if (!borg_full_damage)
					z += 100;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_EXP_20:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_HLIFE]) break;
				/* do not worry about drain exp after level 50 */
				if (borg_skill[BI_CLEVEL] >= 50) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3) ||
					borg_spell_legal(REALM_DEATH, 1, 7)) break;
				if (!borg_full_damage)
					z += 150;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_EXP_40:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_HLIFE]) break;
				/* do not worry about drain exp after level 50 */
				if (borg_skill[BI_CLEVEL] >= 50) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3) ||
					borg_spell_legal(REALM_DEATH, 1, 7)) break;
				if (!borg_full_damage)
					z += 200;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_EXP_80:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (borg_skill[BI_HLIFE]) break;
				/* do not worry about drain exp after level 50 */
				if (borg_skill[BI_CLEVEL] >= 50) break;
				if (borg_spell_legal(REALM_LIFE, 3, 3) ||
					borg_spell_legal(REALM_DEATH, 1, 7)) break;
				if (!borg_full_damage)
					z += 250;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_DISEASE:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (!borg_skill[BI_RPOIS] && !my_oppose_pois) z += 50;
				/* there is a 10% chance to suffer CON loss */
				if (!borg_skill[BI_SCON]) z += 50;
				if (!borg_full_damage)
					z += 50;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_TIME:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (!borg_skill[BI_HLIFE] &&
					borg_skill[BI_CLEVEL] != 50) z += 25;
				if (!borg_spell_legal(REALM_LIFE, 3, 3) &&
					!borg_spell_legal(REALM_DEATH, 1, 7)) z += 25;
				/* Some fear for non sustaining stats ? */
				if (!borg_full_damage)
					z += 100;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}

			case RBE_EXP_VAMP:
			{
				z = (b_ptr->d_dice * b_ptr->d_side);
				/* if invulnurable, no damage */
				if ((borg_goi) && !borg_attacking)
					z = 0;
				if (!borg_skill[BI_HLIFE] &&
					borg_skill[BI_CLEVEL] != 50) z += 25;
				if (!borg_spell_legal(REALM_LIFE, 3, 3) &&
					!borg_spell_legal(REALM_DEATH, 1, 7)) z += 25;
				if (!borg_full_damage)
					z += 200;
				if ((pfe) && !borg_attacking)
					z /= 2;
				break;
			}
		}

		/* if we are doing partial damage reduce for % chance that it will */
		/* hit you. */
		if (!borg_full_damage)
		{
			/* figure out chance that monster will hit you. */
			/* add a 30% bonus in to account for bad luck. */
			if ((r_ptr->level + power) > 0)
				chance =
					130 - (((ac * 300) / 4) / ((r_ptr->level + power) * 3));
			else
				chance = -1;

			/* always have a 5% chance of hitting. */
			if (chance < 0)
				z = (z * 5) / 100;

			if (chance < 100)
				z = (z * chance) / 100;
		}

		/* Add in damage */
		n += z;
	}

	/* if invulnurable, very little damage 5% */
	if (borg_goi) n = (n * 5 / 100);

	/* Danger */
	return (n);
}


/*
 * Calculate base danger from a monster's spell attacks
 *
 * We attempt to take account of various resistances, both in
 * terms of actual damage, and special effects, as appropriate.
 *
 * We reduce the danger from distant "sleeping" monsters.
 *
 * We reduce the danger if the monster is immobile or not LOS
 */
static int borg_danger_aux2(int i, bool average)
{
	int q, k, n = 0, pfe = 0;
	bool glyph = FALSE;

	int true_borg_goi = borg_goi;

	int spot_x, spot_y, spot_safe = 1;

	int x, y;

	int lev, hp, total_dam = 0, av;

	byte spell[96], num = 0;

	borg_kill *kill = &borg_kills[i];

	map_block *mb_ptr;

	monster_race *r_ptr = &r_info[kill->r_idx];

	/* apw PFE gives a protection.  */
	/* Hack -- Apply "protection from evil" */
	if ((borg_prot_from_evil) &&
		(r_ptr->flags3 & RF3_EVIL) && ((borg_skill[BI_CLEVEL]) >= r_ptr->level))
	{
		pfe = 1;
	}

	/*
	 * Glyph of warding rune of protection provides some small
	 * protection with some ranged atacks; mainly summon attacks.
	 * We should reduce the danger commensurate to the probability of the
	 * monster breaking the glyph as defined by melee2.c
	 */
	if (borg_on_glyph)
	{
		glyph = 1;
	}

	/* This is used to calculate the free squares next to us.
	 * This is important when dealing with summoners.
	 */
	for (spot_x = -1; spot_x <= 1; spot_x++)
	{
		for (spot_y = -1; spot_y <= 1; spot_y++)
		{
			/* Acquire location */
			x = spot_x + c_x;
			y = spot_y + c_y;

			/* Bounds checking */
			if (!map_in_bounds(x, y)) continue;

			mb_ptr = map_loc(x, y);

			/* skip our own spot */
			if (x == c_x && y == c_y) continue;

			/* Track spaces already protected */
			if (mb_ptr->monster ||
				((mb_ptr->feat >= FEAT_CLOSED)
				 && (mb_ptr->feat <= FEAT_PERM_SOLID)))
			{
				/* Track the safe areas for calculating danger */
				spot_safe++;

				/* Just in case */
				if (spot_safe == 0) spot_safe = 1;
			}

		}
	}
	/* HACK- to accomdate for GOI and Create_Door */
	if (borg_create_door)
	{
		borg_goi = 0;
	}

	/* Extract the "inate" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags4 & (1L << k)) spell[num++] = k + 32 * 3;
	}

	/* Extract the "normal" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags5 & (1L << k)) spell[num++] = k + 32 * 4;
	}

	/* Extract the "bizarre" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags6 & (1L << k)) spell[num++] = k + 32 * 5;
	}

	/* Paranoia -- Nothing to cast */
	if (!num) return (0);


	/* Extract the level */
	lev = r_ptr->level;

	/* Extract hit-points */
	hp = kill->power;


	/* Analyze the spells */
	for (q = 0; q < num; q++)
	{
		int p = 0;

		int z = 0;

		/* Cast the spell. */
		switch (spell[q])
		{
			case 96 + 0:
			{
				/* RF4_SHRIEK */
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 10;
				break;
			}

			case 96 + 1:
			{
				/* RF4_XXX2X4 */
				/* this is now a failed spell attempt for monsters */
				/* used to recognize invisible/ hidden monsters */
				p += 10;
				break;
			}

			case 96 + 2:
			{
				/* RF4_XXX3X4 */
				break;
			}

			case 96 + 3:
			{
				/* RF4_ROCKETS */
				z = (hp / 4);
				/* max damage */
				if (z > 600)
					z = 600;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RSHRD]) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
				break;
			}

			case 96 + 4:
			{
				/* RF4_ARROW_1 */
				z = (1 * 6);
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_REFLECT]) z = (z + 2) / 3;
				break;
			}

			case 96 + 5:
			{
				/* RF4_ARROW_2 */
				z = (3 * 6);
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_REFLECT]) z = (z + 2) / 3;
				break;
			}

			case 96 + 6:
			{
				/* RF4_ARROW_3 */
				z = (5 * 6);
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_REFLECT]) z = (z + 2) / 3;
				break;
			}

			case 96 + 7:
			{
				/* RF4_ARROW_4 */
				z = (7 * 6);
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_REFLECT]) z = (z + 2) / 3;
				break;
			}

			case 96 + 8:
			{
				/* RF4_BR_ACID */
				if (borg_skill[BI_IACID]) break;
				z = (hp / 3);
				/* max damage */
				if (z > 1200)
					z = 1200;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RACID]) z = (z + 2) / 3;
				if (my_oppose_acid) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
				break;
			}

			case 96 + 9:
			{
				/* RF4_BR_ELEC */
				if (borg_skill[BI_IELEC]) break;
				z = (hp / 3);
				/* max damage */
				if (z > 1200)
					z = 1200;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RELEC]) z = (z + 2) / 3;
				if (my_oppose_elec) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 96 + 10:
			{
				/* RF4_BR_FIRE */
				if (borg_skill[BI_IFIRE]) break;
				z = (hp / 3);
				/* max damage */
				if (z > 1200)
					z = 1200;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RFIRE]) z = (z + 2) / 3;
				if (my_oppose_fire) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
				break;
			}

			case 96 + 11:
			{
				/* RF4_BR_COLD */
				if (borg_skill[BI_ICOLD]) break;
				z = (hp / 3);
				/* max damage */
				if (z > 1200)
					z = 1200;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RCOLD]) z = (z + 2) / 3;
				if (my_oppose_cold) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 96 + 12:
			{
				/* RF4_BR_POIS */
				z = (hp / 3);
				/* max damage */
				if (z > 600)
					z = 600;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RPOIS]) z = (z + 2) / 3;
				if (my_oppose_pois) z = (z + 2) / 3;
				if (my_oppose_pois) break;
				if (borg_skill[BI_RPOIS]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 96 + 13:
			{
				/* RF4_BR_NETH */
				z = (hp / 6);
				/* max damage */
				if (z > 450)
					z = 450;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RNTHR])
				{
					z = (z * 6) / 9;
					break;
				}
				if (!borg_full_damage)
					p += 125;
				break;
			}

			case 96 + 14:
			{
				/* RF4_BR_LITE */
				z = (hp / 4);
				/* max damage */
				if (z > 350)
					z = 350;
				/* Vamps hurt more */
				if (borg_race == RACE_VAMPIRE)
					z *= 2;
				if (borg_goi)
				{
					z /= 25;
				}
				if (borg_skill[BI_RLITE])
				{
					z = (z * 2) / 3;
					break;
				}
				if (borg_skill[BI_RBLIND]) break;
				p += 20;
				break;
			}

			case 96 + 15:
			{
				/* RF4_BR_DARK */
				z = (hp / 4);
				/* max damage */
				if (z > 350)
					z = 350;
				if (borg_goi)
				{
					z /= 25;
				}
				if (borg_skill[BI_RDARK]) z = (z * 2) / 3;
				if (borg_skill[BI_RDARK]) break;
				if (borg_skill[BI_RBLIND]) break;
				p += 20;
				break;
			}

			case 96 + 16:
			{
				/* RF4_BR_CONF */
				z = (hp / 4);
				/* max damage */
				if (z > 350)
					z = 350;
				if (borg_goi)
				{
					z /= 25;
				}
				if (borg_skill[BI_RCONF]) z = z / 2;
				if (borg_skill[BI_RCONF]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 300;
				break;
			}

			case 96 + 17:
			{
				/* RF4_BR_SOUN */
				z = (hp / 4);
				/* max damage */
				if (z > 350)
					z = 350;
				if (borg_goi)
				{
					z /= 25;
				}
				if (borg_skill[BI_RSND]) z = (z * 5) / 9;
				if (borg_skill[BI_RSND]) break;
				/* if already stunned be REALLY nervous dangerousabout this */
				if (borg_skill[BI_ISSTUN])
					p += 500;
				if (borg_skill[BI_ISHEAVYSTUN])
					p += 1000;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 50;
				break;
			}

			case 96 + 18:
			{
				/* RF4_BR_CHAO */
				z = (hp / 4);
				/* max damage */
				if (z > 500)
					z = 500;
				if (borg_goi)
				{
					z /= 25;
				}
				if (borg_skill[BI_RKAOS]) z = (z * 6) / 9;
				if (!borg_full_damage)
					p += 100;
				if (borg_skill[BI_RKAOS]) break;
				p += 200;
				break;
			}

			case 96 + 19:
			{
				/* RF4_BR_DISE */
				z = (hp / 4);
				/* max damage */
				if (z > 400)
					z = 400;
				if (borg_goi)
				{
					z /= 25;
				}
				if (borg_skill[BI_RDIS]) z = (z * 6) / 10;
				if (borg_skill[BI_RDIS]) break;
				p += 500;
				break;
			}

			case 96 + 20:
			{
				/* RF4_BR_NEXU */
				z = (hp / 3);
				/* max damage */
				if (z > 250)
					z = 250;
				if (borg_goi)
				{
					z /= 25;
				}
				if (borg_skill[BI_RNXUS]) z = (z * 6) / 10;
				if (borg_skill[BI_RNXUS]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 100;
				break;
			}

			case 96 + 21:
			{
				/* RF4_BR_TIME */
				z = (hp / 3);
				/* max damage */
				if (z > 150)
					z = 150;
				if (borg_goi)
				{
					z /= 25;
				}
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 250;
				break;
			}

			case 96 + 22:
			{
				/* RF4_BR_INER */
				z = (hp / 4);
				/* max damage */
				if (z > 200)
					z = 200;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 100;
				break;
			}

			case 96 + 23:
			{
				/* RF4_BR_GRAV */
				z = (hp / 3);
				/* max damage */
				if (z > 200)
					z = 200;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 100;
				if (borg_skill[BI_RSND]) break;
				/* Pump this up if you have goi so that the borg is sure */
				/* to be made nervous */
				if (borg_goi)
					p += 100;
				else
					p += 75;
				/* if already stunned be REALLY nervous about this */
				if (borg_skill[BI_ISSTUN])
					p += 500;
				if (borg_skill[BI_ISHEAVYSTUN])
					p += 1000;
				break;
			}

			case 96 + 24:
			{
				/* RF4_BR_SHAR */
				z = (hp / 4);
				/* max damage */
				if (z > 400)
					z = 400;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RSHRD]) z = (z * 6) / 9;
				if (borg_skill[BI_RSHRD]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 50;
				break;
			}

			case 96 + 25:
			{
				/* RF4_BR_PLAS */
				z = (hp / 4);
				/* max damage */
				if (z > 200)
					z = 200;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RSND]) break;
				/* Pump this up if you have goi so that the borg is sure */
				/* to be made nervous */
				if (borg_goi)
					p += 200;
				else
					p += 100;
				/* if already stunned be REALLY nervous about this */
				if (borg_skill[BI_ISSTUN])
					p += 500;
				if (borg_skill[BI_ISHEAVYSTUN])
					p += 1000;
				break;
			}

			case 96 + 26:
			{
				/* RF4_BR_WALL */
				z = (hp / 4);
				/* max damage */
				if (z > 200)
					z = 200;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RSND]) break;
				/* if already stunned be REALLY nervous about this */
				if (borg_skill[BI_ISSTUN])
					p += 100;
				if (borg_skill[BI_ISHEAVYSTUN])
					p += 500;
				if (!borg_full_damage)
					p += 50;
				break;
			}

			case 96 + 27:
			{
				/* RF4_MANA */
				z = (hp / 3);
				/* max damage */
				if (z > 250)
					z = 250;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 50;
				break;
			}

			case 96 + 28:
			{
				/* RF4_BA_NUKE */
				z = (lev + (10 * 6));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (!borg_skill[BI_RPOIS]) p += 25;
				if (!borg_full_damage)
					p += 50;
				break;
			}

			case 96 + 29:
			{
				/* RF4_BR_NUKE */
				z = (hp / 2);
				/* max damage */
				if (z > 600)
					z = 600;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (!borg_skill[BI_RPOIS]) p += 25;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 75;
				break;
			}

			case 96 + 30:
			{
				/* RF4_BA_CHAOS */
				z = ((lev * 2) + 75);
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RKAOS]) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
				break;
			}

			case 96 + 31:
			{
				/* RF4_BA_DISI */
				z = (hp / 3);
				if (z >= 300) z = 300;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				/*     if (borg_skill[BI_Rdisintegration?]) z = (z + 2) / 3; */
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
			}

			case 128 + 0:
			{
				/* RF5_BA_ACID */
				if (borg_skill[BI_IACID]) break;
				z = (lev * 3) / 2 + 15;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RACID]) z = (z + 2) / 3;
				if (my_oppose_acid) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
				break;
			}

			case 128 + 1:
			{
				/* RF5_BA_ELEC */
				if (borg_skill[BI_IELEC]) break;
				z = (lev * 3) / 2 + 8;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RELEC]) z = (z + 2) / 3;
				if (my_oppose_elec) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 128 + 2:
			{
				/* RF5_BA_FIRE */
				if (borg_skill[BI_IFIRE]) break;
				z = (lev * 7) / 2 + 10;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RFIRE]) z = (z + 2) / 3;
				if (my_oppose_fire) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
				break;
			}

			case 128 + 3:
			{
				/* RF5_BA_COLD */
				if (borg_skill[BI_ICOLD]) break;
				z = (lev * 3) / 2 + 10;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RCOLD]) z = (z + 2) / 3;
				if (my_oppose_cold) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 128 + 4:
			{
				/* RF5_BA_POIS */
				z = (12 * 2);
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RPOIS]) z = (z + 2) / 3;
				if (my_oppose_pois) z = (z + 2) / 3;
				if (my_oppose_pois) break;
				if (borg_skill[BI_RPOIS]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 128 + 5:
			{
				/* RF5_BA_NETH */
				z = (lev + (75) + 50);
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RNTHR]) z = (z * 6) / 8;
				if (borg_skill[BI_RNTHR]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 250;
				break;
			}

			case 128 + 6:
			{
				/* RF5_BA_WATE */
				z = ((lev * 5) / 2) + 50;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RSND]) break;
				/* if already stunned be REALLY nervous about this */
				if (borg_skill[BI_ISSTUN])
					p += 500;
				if (borg_skill[BI_ISHEAVYSTUN])
					p += 1000;
				if (borg_skill[BI_RCONF]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 50;
				break;
			}

			case 128 + 7:
			{
				/* RF5_BA_MANA */
				z = ((lev * 4) + 75);
				if (!borg_full_damage)
					p += 50;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				break;
			}

			case 128 + 8:
			{
				/* RF5_BA_DARK */
				z = (((lev * 4)) + (75));
				if (borg_goi)
				{
					z /= 25;
				}
				if (borg_skill[BI_RDARK]) z = (z * 6) / 9;
				if (borg_skill[BI_RDARK]) break;
				if (borg_skill[BI_RBLIND]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 128 + 9:
			{
				/* RF5_DRAIN_MANA */
				if (borg_skill[BI_MAXSP]) p += 10;
				break;
			}

			case 128 + 10:
			{
				/* RF5_MIND_BLAST */
				if (borg_skill[BI_SAV] < 100)
					z = 35;
				break;
			}

			case 128 + 11:
			{
				/* RF5_BRAIN_SMASH */
				z = (12 * 15);
				p += 200 - 2 * borg_skill[BI_SAV];
				if (p < 0) p = 0;
				break;
			}

			case 128 + 12:
			{
				/* RF5_CAUSE_1 */
				if (borg_skill[BI_SAV] >= 100) break;
				z = (3 * 8);
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					/* reduce by % chance of save  (add 20% for fudge) */
					z = z * (120 - borg_skill[BI_SAV]) / 100;
				break;
			}

			case 128 + 13:
			{
				/* RF5_CAUSE_2 */
				if (borg_skill[BI_SAV] >= 100) break;
				z = (8 * 8);
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					/* reduce by % chance of save  (add 20% for fudge) */
					z = z * (120 - borg_skill[BI_SAV]) / 100;
				break;
			}

			case 128 + 14:
			{
				/* RF5_CAUSE_3 */
				if (borg_skill[BI_SAV] >= 100) break;
				z = (10 * 15);
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					/* reduce by % chance of save  (add 20% for fudge) */
					z = z * (120 - borg_skill[BI_SAV]) / 100;
				break;
			}

			case 128 + 15:
			{
				/* RF5_CAUSE_4 */
				if (borg_skill[BI_SAV] >= 100) break;
				z = (15 * 15);
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					/* reduce by % chance of save  (add 40% for fudge) */
					z = z * (120 - borg_skill[BI_SAV]) / 100;
				break;
			}

			case 128 + 16:
			{
				/* RF5_BO_ACID */
				if (borg_skill[BI_IACID]) break;
				z = ((7 * 8) + (lev / 3));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RACID]) z = (z + 2) / 3;
				if (my_oppose_acid) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
				break;
			}

			case 128 + 17:
			{
				/* RF5_BO_ELEC */
				if (borg_skill[BI_IELEC]) break;
				z = ((4 * 8) + (lev / 3));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RELEC]) z = (z + 2) / 3;
				if (my_oppose_elec) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 128 + 18:
			{
				/* RF5_BO_FIRE */
				if (borg_skill[BI_IFIRE]) break;
				z = ((9 * 8) + (lev / 3));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RFIRE]) z = (z + 2) / 3;
				if (my_oppose_fire) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 40;
				break;
			}

			case 128 + 19:
			{
				/* RF5_BO_COLD */
				if (borg_skill[BI_ICOLD]) break;
				z = ((6 * 8) + (lev / 3));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RCOLD]) z = (z + 2) / 3;
				if (my_oppose_cold) z = (z + 2) / 3;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 128 + 20:
			{
				/* RF5_BO_POIS */
				/* XXX XXX XXX */
				break;
			}

			case 128 + 21:
			{
				/* RF5_BO_NETH */
				z = (50 + 30 + (5 * 5) + (lev * 3) / 2);
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RNTHR]) z = (z * 6) / 8;
				if (borg_skill[BI_RNTHR]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 200;
				break;
			}

			case 128 + 22:
			{
				/* RF5_BO_WATE */
				z = ((10 * 10) + (lev));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RSND]) break;
				/* if already stunned be REALLY nervous about this */
				if (borg_skill[BI_ISSTUN])
					p += 500;
				if (borg_skill[BI_ISHEAVYSTUN])
					p += 1000;
				if (borg_skill[BI_RCONF]) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 128 + 23:
			{
				/* RF5_BO_MANA */
				z = ((lev * 7) / 2) + 50;
				if (!borg_full_damage)
					p += 50;
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				break;
			}

			case 128 + 24:
			{
				/* RF5_BO_PLAS */
				z = (10 + (8 * 7) + (lev));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				if (borg_skill[BI_RSND]) break;
				/* if already stunned be REALLY nervous about this */
				if (borg_skill[BI_ISSTUN])
					p += 500;
				if (borg_skill[BI_ISHEAVYSTUN])
					p += 1000;
				break;
			}

			case 128 + 25:
			{
				/* RF5_BO_ICEE */
				z = ((6 * 6) + (lev));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				if (borg_skill[BI_RSND]) break;
				/* if already stunned be REALLY nervous about this */
				if (borg_skill[BI_ISSTUN])
					p += 50;
				if (borg_skill[BI_ISHEAVYSTUN])
					p += 1000;
				break;
			}

			case 128 + 26:
			{
				/* RF5_MISSILE */
				z = ((2 * 6) + (lev / 3));
				if (borg_goi)
				{
					z /= 25;
					break;
				}
				break;
			}

			case 128 + 27:
			{
				/* RF5_SCARE */
				if (borg_skill[BI_SAV] >= 100) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 10;
				break;
			}

			case 128 + 28:
			{
				/* RF5_BLIND */
				if (borg_skill[BI_SAV] >= 100) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 10;
				break;
			}

			case 128 + 29:
			{
				/* RF5_CONF */
				if (borg_skill[BI_SAV] >= 100) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 10;
				break;
			}

			case 128 + 30:
			{
				/* RF5_SLOW */
				if (borg_skill[BI_FRACT]) break;
				if (borg_skill[BI_SAV] >= 100) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 5;
				break;
			}

			case 128 + 31:
			{
				/* RF5_HOLD */
				if (borg_skill[BI_FRACT]) break;
				if (borg_skill[BI_SAV] >= 100) break;
				p += 150;
				break;
			}

			case 160 + 0:
			{
				/* RF6_HASTE */
				if (borg_skill[BI_SAV] >= 100) break;
				z += (90 * borg_skill[BI_CURHP] / 100);
				p += 150;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 10;
				break;
			}

			case 160 + 1:
			{
				/* RF6_HAND_OF_DOOM */
				break;
			}

			case 160 + 2:
			{
				/* RF6_HEAL */
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 10;
				break;
			}

			case 160 + 3:
			{
				/* RF6_INVULNER */
				break;
			}

			case 160 + 4:
			{
				/* RF6_BLINK */
				break;
			}

			case 160 + 5:
			{
				/* RF6_TPORT */
				break;
			}

			case 160 + 6:
			{
				/* RF6_XXX3X6 */
				break;
			}

			case 160 + 7:
			{
				/* RF6_XXX4X6 */
				break;
			}

			case 160 + 8:
			{
				/* RF6_TELE_TO */
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 20;
				break;
			}

			case 160 + 9:
			{
				/* RF6_TELE_AWAY */
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 10;
				break;
			}

			case 160 + 10:
			{
				/* RF6_TELE_LEVEL */
				if (borg_skill[BI_SAV] >= 100) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 50;
				break;
			}

			case 160 + 11:
			{
				/* RF6_XXX5 */
				break;
			}

			case 160 + 12:
			{
				/* RF6_DARKNESS */
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 5;
				break;
			}

			case 160 + 13:
			{
				/* RF6_TRAPS */
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
					p += 50;
				break;
			}

			case 160 + 14:
			{
				/* RF6_FORGET */
				if (borg_skill[BI_SAV] >= 100) break;
				/* if looking at full damage, things that are just annoying */
				/* do not count. */
				if (!borg_full_damage)
				{
					/* if you have lots of cash (like you will at level 35) */
					/* this is not very scary... just re-ID. */
					if (borg_skill[BI_CLEVEL] < 35)
					{
						p += 500;
					}
					else
					{
						p += 50;
					}
				}
				break;
			}

			case 160 + 15:
			{
				/* RF6_RAISE_DEAD */
				break;
			}

			case 160 + 16:
			{
				/* Summoning is only as dangerous as the monster that is
				 * actually summoned but the monsters that summon are a priority
				 * to kill.  PFE reduces danger from some evil summoned monsters
				 * One Problem with GOI and Create Door is that the GOI reduces
				 * the fear so much that the borg won't cast the Create Door,
				 * eventhough it would be a good idea.
				 */

				/* S_KIN */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 3;
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else
				{
					p += (lev) * 7;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;

				break;
			}

			case 160 + 17:
			{
				/* S_HI_DEMON */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 6;
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else
				{
					p += (lev) * 12;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}


			case 160 + 18:
			{
				/* RF6_S_MONSTER */
				if (borg_goi || pfe || glyph || borg_create_door ||
					borg_fighting_unique)
					p += 0;
				else
				{
					p += (lev) * 5;
					p = p / spot_safe;
				}
				break;
			}

			case 160 + 19:
			{
				/* RF6_S_MONSTERS */
				if (borg_goi || pfe || glyph || borg_create_door ||
					borg_fighting_unique)
					p += 0;
				else
				{
					p += (lev) * 7;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 20:
			{
				/* RF6_S_ANT */
				if (borg_goi || pfe || glyph || borg_create_door ||
					borg_fighting_unique)
					p += 0;
				else
				{
					p += (lev) * 5;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 21:
			{
				/* RF6_S_SPIDER */
				if (borg_goi || pfe || glyph || borg_create_door ||
					borg_fighting_unique)
					p += 0;
				else
				{
					p += (lev) * 5;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 22:
			{
				/* RF6_S_HOUND */
				if (borg_goi || pfe || glyph || borg_create_door ||
					borg_fighting_unique)
					p += 0;
				else
				{
					p += (lev) * 5;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 23:
			{
				/* RF6_S_HYDRA */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 2;
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 5;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 24:
			{
				/* RF6_S_ANGEL */
				if (pfe || borg_fighting_unique)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 3;
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 7;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 25:
			{
				/* RF6_S_DEMON */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 3;
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 7;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 26:
			{
				/* RF6_S_UNDEAD */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 3;
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 7;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 27:
			{
				/* RF6_S_DRAGON */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 3;
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 7;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 28:
			{
				/* RF6_S_HI_UNDEAD */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 6;
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 12;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 29:
			{
				/* RF6_S_HI_DRAGON */
				if (pfe)
				{
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 6;
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 12;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 30:
			{
				/* RF6_S_AMBERITES */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door || borg_fighting_unique)
				{
					p += (lev) * 6;
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 12;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}

			case 160 + 31:
			{
				/* RF6_S_UNIQUE */
				if (pfe)
				{
					p += (lev);
					p = p / spot_safe;
				}
				else if (borg_goi)
				{
					p = 0;
				}
				else if (glyph || borg_create_door)
				{
					p += (lev) * 3;	/* slightly reduced danger for unique */
					p = p / spot_safe;
				}
				else
				{
					p += (lev) * 6;
					p = p / spot_safe;
				}
				/* reduce the fear if it is a unique */
				if (r_info->flags1 & RF1_UNIQUE) p = p * 75 / 100;
				break;
			}
		}

		/* Notice damage */
		p += z;

		/* Track the most dangerous spell */
		if (p > n) n = p;

		/* Track the damage of all the spells, used in averaging */
		total_dam += p;
	}

	/* Average damage of all the spells & compare to most dangerous spell */
	av = total_dam / num;

	/* HACK- to accomdate for GOI and Create_Door */
	borg_goi = true_borg_goi;

	/*
	 * If the most dangerous spell is alot bigger than the average,
	 * then return the dangerous one
	 *
	 * There is a problem when dealing with defence manuevers.
	 * If the borg is considering casting a spell like Resistance
	 * and the monster also has a non-resistable attack
	 * (like Disenchant) then the damage returned will be for that
	 * spell, since the danger of the others (like fire, cold) will
	 * be greatly reduced by the proposed defence spell.  The result
	 * will be the borg will not cast the resistance spell even though
	 * it may be a very good idea.
	 *
	 * Example: a monster has three breath attacks
	 * (Fire, Ice, Disenchant) and each hits for 500 pts of damage.
	 * The borg currently resists all three, so the danger would be 500.
	 * If the borg were to use a Res Heat Potion that would decrease
	 * the danger to:
	 *
	 * Fire:  333
	 * Ice:   500
	 * Disen: 500
	 * Now the Average is 444.  Not really worth it, nominal change.
	 * But if the resistance spell was both Fire and Ice, then it would be:
	 * Fire:  333
	 * Ice:   333
	 * Disen: 500
	 * With an average of 388. Probably worth it, but the borg would
	 * see that the Disen attack is a quite dangerous and would return
	 * the result of 500.
	 *
	 * To fix this, the flag 'average' is added to the borg_danger()
	 * to skip this check and return the average damage.  If the flag
	 * is FALSE then the formula below is SKIPPED and the value returned
	 * with be the average.  If the flag is TRUE, then the formula below
	 * will be used to determine the returned value.  Currently the
	 * elemental resistance spells and PFE have the flag set as FALSE.
	 */
	if (!average) return (av);
	if ((n >= av * 15 / 10) || (n > borg_skill[BI_CURHP] * 8 / 10))
	{
		return (n);
	}
	else
	{
		/* Average Danger */
		return (av);
	}
}


/*
 * Calculate the danger to a grid from a monster  XXX XXX XXX
 *
 * Note that we are paranoid, especially about "monster speed",
 * since even if a monster is slower than us, it will occasionally
 * get one full turn to attack us.
 *
 * Note that we assume that monsters can walk through walls and
 * other monsters to get to the player.  XXX XXX XXX
 *
 * This function ignores possibilities such as movement plus
 * spell attacks, physical attacks and spell attacks together,
 * and other similar situations.  XXX XXX XXX
 *
 * Currently we assume that "sleeping" monsters are less dangerous
 * unless you get near them, which may wake them up.
 *
 * We attempt to take into account things like monsters which sometimes
 * "stumble", and monsters which only "sometimes" use powerful spells.
 */
int borg_danger_aux(int x, int y, int c, int i, bool average)
{
	borg_kill *kill = &borg_kills[i];

	monster_race *r_ptr = &r_info[kill->r_idx];

	int x9 = kill->x;
	int y9 = kill->y;

	int ax, ay, d;

	int q = 0, r, p, v1 = 0, v2 = 0;

	int glyph = 0;

	int fake_speed = borg_skill[BI_SPEED];
	int monster_speed = r_ptr->speed;
	int t, e;

	/* Paranoia */
	if (!kill->r_idx) return (0);


	/* Distance components */
	ax = (x9 > x) ? (x9 - x) : (x - x9);
	ay = (y9 > y) ? (y9 - y) : (y - y9);

	/* Distance */
	d = MAX(ax, ay);

	/* Minimal distance */
	if (d < 1) d = 1;

	/* Minimal distance */
	if (d > 20) return (0);

	/* A very speedy borg will miscalculate danger of some monsters */
	if (borg_skill[BI_SPEED] >= 135) fake_speed =
			(borg_fighting_unique ? 120 : 125);

	/* Consider the character haste and slow monster spells */
	if (borg_speed)
		fake_speed += 10;
	if (borg_slow_spell)
		monster_speed -= 10;

	/* Assume monsters are a little fast when you are low level */
	if (borg_skill[BI_MAXHP] < 20)
		monster_speed += 7;


	/* Player energy per game turn  */
	e = extract_energy[(fake_speed)];

	/* Game turns per player move  */
	t = (100 + (e - 1)) / e;

	/*  Monster energy per game turn  */
	e = extract_energy[monster_speed];

	/* Monster moves */
	q = c * ((t * e) / 10);

	/* Minimal energy */
	/* allow partial hits when not caculating full possible damage */
	if (borg_full_damage)
		q = (int)((q + 9) / 10) * 10;


	/** Danger from physical attacks **/

	/* Physical attacks */
	v1 = borg_danger_aux1(kill->r_idx);

	/* No attacks for some monsters */
	if (r_ptr->flags1 & RF1_NEVER_BLOW)
	{
		v1 = 0;
	}

	/* No movement for some monsters */
	if ((r_ptr->flags1 & RF1_NEVER_MOVE) && (d > 1))
	{
		v1 = 0;
	}

	/* Hack -- Physical attacks require proximity */
	/* If the monster is next to us and gets a partial hit, count it. */
	if (q > 10 || d != 1)
	{
		if (q < (d * 10) && borg_skill[BI_CLEVEL] > 20)
		{
			v1 = 0;
		}
		else if (q < (d * 10) && borg_skill[BI_CLEVEL] <= 20)
		{						/* reduce damage to 20% if we are weak */
			v1 = (v1 * 2 / 10);
		}
	}

	/* multipliers yeild some trouble when I am weak */
	if ((r_ptr->flags2 & RF2_MULTIPLY) && (borg_skill[BI_CLEVEL] < 20))
	{							/* extra 50% */
		v1 = v1 + (v1 * 15 / 10);
	}

	/* Friends yeild some trouble when I am weak */
	if ((r_ptr->flags1 & RF1_FRIENDS || r_ptr->flags1 & RF1_ESCORTS) &&
		(borg_skill[BI_CLEVEL] < 20))
	{
		if (borg_skill[BI_CLEVEL] < 15)
		{
			/* extra 80% */
			v1 = v1 + (v1 * 18 / 10);
		}
		else
		{
			/* extra 30% */
			v1 = v1 + (v1 * 13 / 10);
		}

	}

	/* glyph of warding rune of protection reduction here
	 * We should reduce the danger commensurate to the probability of the
	 * monster breaking the glyph as defined by melee2.c
	 */
	if (borg_on_glyph)
	{
		v1 = 0;
	}
	if (track_glyph_num)
	{
		/* Check all existing glyphs */
		for (glyph = 0; glyph < track_glyph_num; glyph++)
		{
			if ((track_glyph_y[glyph] == y) && (track_glyph_x[glyph] == x))
			{
				/* Reduce the danger */
				v1 = 0;
			}
		}
	}



	/* Reduce danger from sleeping monsters */
	if ((kill->m_flags & MONST_ASLEEP) && (d > 1))
	{
		/* Normal reduction of fear */
		if (borg_skill[BI_CLEVEL] >= 10)
		{
			v1 = v1 / d;
		}
		else
		{
			/* low clevel weaklings should still fear alot */
			v1 = v1 * 8 / 10;
		}
	}

	/* Reduce danger from sleeping monsters with the sleep 2 spell */
	if (borg_sleep_spell_ii)
	{
		if ((d == 1) &&
			(!(kill->m_flags & MONST_ASLEEP)) &&
			(!(r_ptr->flags3 & RF3_NO_SLEEP)) &&
			(!(r_ptr->flags1 & RF1_UNIQUE)) &&
			(r_ptr->level <= (borg_skill[BI_CLEVEL] - 15)))
		{
			v1 = v1 / 3;
		}
	}
	/* Reduce danger from sleeping monsters with the sleep 1,3 spell */
	if (borg_sleep_spell)
	{
		v1 = v1 / (d + 2);
	}
	/* Reduce danger from confused monsters */
	if (kill->m_flags & MONST_CONFUSED)
	{
		v1 = v1 / 2;
	}
	if (kill->m_flags & MONST_STUN)
	{
		v1 = v1 * 10 / 13;
	}
	if (borg_confuse_spell)
	{
		v1 = v1 / 6;
	}
	/* Perceive a reduce danger from scared monsters */
	if (borg_fear_mon_spell)
	{
		v1 = 0;
	}

	/* Tweak danger based on the "alertness" of the monster */
	if (kill->m_flags & MONST_ASLEEP)
	{
		/* increase the danger for light sleepers */
		int inc = r_ptr->sleep + 5;

		v1 = v1 + (v1 * inc / 100);
	}

	/* Danger */
	if (v1)
	{
		/* Attacks after movement */
		r = (q - ((d - 1) * 10));

		/* XXX XXX XXX */
		if (c > 1)
		{
			/* Hack -- stumble sometimes XXX XXX XXX */
			if (r_ptr->flags1 & (RF1_RAND_25 | RF1_RAND_50)) r -= (r / 4);
		}
		/* Total danger */
		v1 = v1 * r / 10;

	}

	/** Ranged Attacks **/

	/* Never cast spells */
	if (!r_ptr->freq_inate && !r_ptr->freq_spell)
	{
		v2 = 0;
	}

	/* Hack -- verify distance */
	else if (distance(y9, x9, y, x) > MAX_RANGE)
	{
		v2 = 0;
	}

	/* Hack -- verify line of sight (both ways) */
	else if (!borg_projectable(x9, y9, x, y) && !borg_projectable(x, y, x9, y9))
	{
		v2 = 0;
	}

	/* Danger from spell attacks */
	else
	{
		int chance;

		/* Spell attacks */
		v2 = borg_danger_aux2(i, average);

		/* multipliers yeild some trouble when I am weak */
		if ((r_ptr->flags2 & RF2_MULTIPLY) && (borg_skill[BI_CLEVEL] < 20))
		{
			v2 = v2 + (v2 * 12 / 10);
		}

		/* Friends yeild some trouble when I am weak */
		if ((r_ptr->flags1 & RF1_FRIENDS || r_ptr->flags1 & RF1_ESCORTS) &&
			(borg_skill[BI_CLEVEL] < 20))
		{
			v2 = v2 + (v2 * 12 / 10);
		}

		/* Reduce danger from sleeping monsters */
		if ((kill->m_flags & MONST_ASLEEP) && (d > 1))
		{
			/* weaklings and should still fear */
			if (borg_skill[BI_CLEVEL] >= 10)
			{
				v2 = v2 / d;
			}
			else
			{
				/* only subract 10% of the danger */
				v2 = v2 * 9 / 10;
			}
		}

		/* Reduce danger from sleeping monsters with the sleep 2 spell */
		if (borg_sleep_spell_ii)
		{

			if ((d == 1) &&
				(!(kill->m_flags & MONST_ASLEEP)) &&
				(!(r_ptr->flags3 & RF3_NO_SLEEP)) &&
				(!(r_ptr->flags1 & RF1_UNIQUE)) &&
				(r_ptr->level <=
				 ((borg_skill[BI_CLEVEL] <
				   15) ? borg_skill[BI_CLEVEL] : (((borg_skill[BI_CLEVEL] - 10)
												   / 4) * 3) + 10)))
			{
				v2 = v2 / 3;
			}
		}

		/* Reduce danger from sleeping monsters with the sleep 1,3 spell */
		if (borg_sleep_spell)
		{
			v2 = v2 / (d + 2);
		}
		/* Reduce danger from confused monsters */
		if (kill->m_flags & MONST_CONFUSED)
		{
			v2 = v2 / 2;
		}
		/* Reduce danger from stunnned monsters  */
		if (kill->m_flags & MONST_STUN)
		{
			v2 = v2 * 10 / 13;
		}
		if (borg_confuse_spell)
		{
			v2 = v2 / 6;
		}

		/* Tweak danger based on the "alertness" of the monster */
		if (kill->m_flags & MONST_ASLEEP)
		{
			/* increase the danger for light sleepers */
			int inc = r_ptr->sleep + 5;

			v2 = v2 + (v2 * inc / 100);
		}

		if (!borg_full_damage)
		{
			/* reduce for frequency. */
			chance = (r_ptr->freq_inate + r_ptr->freq_spell) / 2;
			if (chance < 11)
				v2 = ((v2 * 4) / 10);
			else if (chance < 26)
				v2 = ((v2 * 6) / 10);
			else if (chance < 51)
				v2 = ((v2 * 8) / 10);
		}

		/* Danger */
		if (v2)
		{
			/* Full power */
			r = q;

			/* Total danger */
			v2 = v2 * r / 10;
		}
	}


	/* Maximal danger */
	p = MAX(v1, v2);

	/* No danger from friends or pets */
	if (kill->m_flags & (MONST_FRIEND | MONST_PET)) p = 0;

	/* Result */
	return (p);
}


/*
 * Hack -- Calculate the "danger" of the given grid.
 *
 * Currently based on the physical power of nearby monsters, as well
 * as the spell power of monsters which can target the given grid.
 *
 * This function is extremely expensive, mostly due to the number of
 * times it is called, and also to the fact that it calls its helper
 * functions about thirty times each per call.
 *
 * We need to do more intelligent processing with the "c" parameter,
 * since currently the Borg does not realize that backing into a
 * hallway is a good idea, since as far as he can tell, many of
 * the nearby monsters can "squeeze" into a single grid.
 *
 * Note that we also take account of the danger of the "region" in
 * which the grid is located, which allows us to apply some "fear"
 * of invisible monsters and things of that nature.
 *
 * Generally bool Average is TRUE.
 */
int borg_danger(int x, int y, int c, bool average)
{
	int i, p = 1000;
	int grid_fear = 1000;

	map_block *mb_ptr;

	/* do twice.  Once to get full damage and once to get partial. */
	/* !FIX this is very slow.  I need to find a better way of doing this */
	/* perhaps I should calc both at the same time and pass back */
	/* the right one.  AJG */

	/* Bounds checking */
	if (map_in_bounds(x, y))
	{
		mb_ptr = map_loc(x, y);

		/* Base danger (from fear) */
		grid_fear = mb_ptr->fear * c;
		p = grid_fear;
	}

	/* Reduce this fear if GOI is up */
	if (borg_goi)
	{
		p = p / 4;
	}


	borg_full_damage = TRUE;

	/* Examine all the monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		borg_kill *kill = &borg_kills[i];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Collect danger from monster */
		p += borg_danger_aux(x, y, c, i, average);
	}

	borg_full_damage = FALSE;

	/*
	 * If I can't be killed in one round
	 * (or severely wounded) use probablilities
	 */
	if (p < (avoidance * 85 / 100) && p != 0)
	{
		/* Base danger (from fear) */
		p = grid_fear;

		/* Reduce this fear if GOI is up */
		if (borg_goi)
		{
			p = p / 4;
		}

		/* Examine all the monsters */
		for (i = 1; i < borg_kills_nxt; i++)
		{
			borg_kill *kill = &borg_kills[i];

			/* Skip dead monsters */
			if (!kill->r_idx) continue;

			/* Collect danger from monster */
			p += borg_danger_aux(x, y, c, i, average);
		}
	}

	/* Return the danger */
	return (p > 2000 ? 2000 : p);
}



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
		value += (borg_skill[BI_BMAXDAM] * 20L);

		/* Reward "bonus to hit" */
		if (l_ptr->to_h > 8 || borg_skill[BI_CLEVEL] < 25)
			value += ((borg_skill[BI_TOHIT] + l_ptr->to_h) * 7L);
		else
			value += ((borg_skill[BI_TOHIT] + 8) * 7L);

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
	value -= borg_skill[BI_WEIGHT] / adj_str_wgt[my_stat_ind[A_STR]];


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
	value += borg_power_aux3();

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
