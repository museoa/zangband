/* File: borg4.c */
/*  Purpose: Notice and Power code for the Borg -BEN- */

#include "angband.h"

#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"
#include "zborg4.h"


/*
 * Hack - save old hook for object lists.
 *
 * We chain into this after storing our information.
 * (This is so multiple sub-systems can hook into
 * object changes.)
 */
list_notice_hook_type old_list_hook = NULL;

void borg_list_info(byte list_type)
{
	/* Don't do anything if the borg is inactive */
	if (!borg_active)
	{
		/* Chain into the old hook, if it exists */
		if (old_list_hook) old_list_hook(list_type);

		/* Done */
		return;
	}

	/* Notice changes */
	switch (list_type)
	{
		case LIST_INVEN:
		{
			/* Notice changes */
			borg_notice();

			/* inventory changed so goals must change. */
			goal_shop = goal_ware = goal_item = -1;

			/* Note changed inventory */
			borg_do_crush_junk = TRUE;
			borg_do_crush_hole = TRUE;
			borg_do_crush_slow = TRUE;
			break;
		}

		case LIST_EQUIP:
		{
			/* Notice changes */
			borg_notice();

			goal_shop = goal_ware = goal_item = -1;

			/* Note changed inventory */
			borg_do_crush_junk = TRUE;
			borg_do_crush_hole = TRUE;
			borg_do_crush_slow = TRUE;

			break;
		}

		case LIST_FLOOR:
		{
			/* Notice changes */
			borg_notice();

			break;
		}

		case LIST_STORE_BUY:
		{

			break;
		}

		case LIST_STORE_SELL:
		{

			break;
		}
	}

	/* Finally - chain into the old hook, if it exists */
	if (old_list_hook) old_list_hook(list_type);
}


/*
 * Note that we assume that any item with quantity zero does not exist,
 * thus, when simulating possible worlds, we do not actually have to
 * "optimize" empty slots.
 *
 * XXX XXX XXX Also, we could reward equipment based on possible enchantment,
 * up to the maximal amount available in the home, which would induce item
 * switching when the item could be enchanted sufficiently.
 */


/*
 * The "notice" functions examine various aspects of the player inventory,
 * the player equipment, or the home contents, and extract various numerical
 * quantities based on those aspects, adjusting them for various "abilities",
 * such as the ability to cast certain spells, etc.
 *
 * The "power" functions use the numerical quantities described above, and
 * use them to do two different things:  (1) rank the "value" of having
 * various abilities relative to the possible "money" reward of carrying
 * sellable items instead, and (2) rank the value of various abilities
 * relative to each other, which is used to determine what to wear/buy,
 * and in what order to wear/buy those items.
 *
 * These functions use some very heuristic values, by the way...
 *
 * We should probably take account of things like possible enchanting
 * (especially when in town), and items which may be found soon.
 *
 * We consider several things:
 *   (1) the actual "power" of the current weapon and bow
 *   (2) the various "flags" imparted by the equipment
 *   (3) the various abilities imparted by the equipment
 *   (4) the penalties induced by heavy armor or gloves or edged weapons
 *   (5) the abilities required to enter the "max_depth" dungeon level
 *   (6) the various abilities of some useful inventory items
 *
 * Note the use of special "item counters" for evaluating the value of
 * a collection of items of the given type.  Basically, the first item
 * of the given type is always the most valuable, with subsequent items
 * being worth less, until the "limit" is reached, after which point any
 * extra items are only worth as much as they can be sold for.
 */


/*
 * Notice player flags
 */
static void borg_notice_player(void)
{
	u32b f1, f2, f3;

	/* Recalc some Variables */
	borg_skill[BI_ARMOR] = 0;
	borg_skill[BI_SPEED] = 110;

	/* Start with a single blow per turn */
	borg_skill[BI_BLOWS] = 1;

	/* Base infravision (purely racial) */
	borg_skill[BI_INFRA] = rb_ptr->infra;

	/* Base skill -- disarming */
	borg_skill[BI_DIS] = rb_ptr->r_dis + cb_ptr->c_dis;

	/* Base skill -- magic devices */
	borg_skill[BI_DEV] = rb_ptr->r_dev + cb_ptr->c_dev;

	/* Base skill -- saving throw */
	borg_skill[BI_SAV] = rb_ptr->r_sav + cb_ptr->c_sav;

	/* Base skill -- stealth */
	borg_skill[BI_STL] = rb_ptr->r_stl + cb_ptr->c_stl;

	/* Base skill -- searching ability */
	borg_skill[BI_SRCH] = rb_ptr->r_sns + cb_ptr->c_sns;

	/* Base skill -- searching frequency */
	borg_skill[BI_SRCHFREQ] = rb_ptr->r_fos + cb_ptr->c_fos;

	/* Base skill -- combat (normal) */
	borg_skill[BI_THN] = rb_ptr->r_thn + cb_ptr->c_thn;

	/* Base skill -- combat (shooting) */
	borg_skill[BI_THB] = rb_ptr->r_thb + cb_ptr->c_thb;

	/* Base skill -- combat (throwing) */
	borg_skill[BI_THT] = rb_ptr->r_thb + cb_ptr->c_thb;

	/* Racial Skills */

	/* Extract the player flags */
	player_flags(&f1, &f2, &f3);

	/* Good flags */
	if (f3 & (TR3_SLOW_DIGEST)) borg_skill[BI_SDIG] = TRUE;
	if (f3 & (TR3_FEATHER)) borg_skill[BI_FEATH] = TRUE;
	if (f3 & (TR3_LITE)) borg_skill[BI_LITE] = TRUE;
	if (f3 & (TR3_REGEN)) borg_skill[BI_REG] = TRUE;
	if (f3 & (TR3_TELEPATHY)) borg_skill[BI_ESP] = TRUE;
	if (f3 & (TR3_SEE_INVIS)) borg_skill[BI_SINV] = TRUE;
	if (f2 & (TR2_FREE_ACT)) borg_skill[BI_FRACT] = TRUE;
	if (f2 & (TR2_HOLD_LIFE)) borg_skill[BI_HLIFE] = TRUE;

	/* Weird flags */

	/* Bad flags */
	if (f1 & (TR1_IMPACT)) borg_skill[BI_W_IMPACT] = TRUE;
	if (f3 & (TR3_AGGRAVATE)) borg_skill[BI_CRSAGRV] = TRUE;
	if (f3 & (TR3_TELEPORT)) borg_skill[BI_CRSTELE] = TRUE;
	if (f3 & (TR3_NO_TELE)) borg_skill[BI_CRSNOTELE] = TRUE;
	if (f3 & (TR3_NO_MAGIC)) borg_skill[BI_CRSNOMAGIC] = TRUE;

	/* Immunity flags */
	if (f2 & (TR2_IM_FIRE)) borg_skill[BI_IFIRE] = TRUE;
	if (f2 & (TR2_IM_ACID)) borg_skill[BI_IACID] = TRUE;
	if (f2 & (TR2_IM_COLD)) borg_skill[BI_ICOLD] = TRUE;
	if (f2 & (TR2_IM_ELEC)) borg_skill[BI_IELEC] = TRUE;

	/* Resistance flags */
	if (f2 & (TR2_RES_ACID)) borg_skill[BI_RACID] = TRUE;
	if (f2 & (TR2_RES_ELEC)) borg_skill[BI_RELEC] = TRUE;
	if (f2 & (TR2_RES_FIRE)) borg_skill[BI_RFIRE] = TRUE;
	if (f2 & (TR2_RES_COLD)) borg_skill[BI_RCOLD] = TRUE;
	if (f2 & (TR2_RES_POIS)) borg_skill[BI_RPOIS] = TRUE;
	if (f2 & (TR2_RES_FEAR)) borg_skill[BI_RFEAR] = TRUE;
	if (f2 & (TR2_RES_LITE)) borg_skill[BI_RLITE] = TRUE;
	if (f2 & (TR2_RES_DARK)) borg_skill[BI_RDARK] = TRUE;
	if (f2 & (TR2_RES_BLIND)) borg_skill[BI_RBLIND] = TRUE;
	if (f2 & (TR2_RES_CONF)) borg_skill[BI_RCONF] = TRUE;
	if (f2 & (TR2_RES_SOUND)) borg_skill[BI_RSND] = TRUE;
	if (f2 & (TR2_RES_SHARDS)) borg_skill[BI_RSHRD] = TRUE;
	if (f2 & (TR2_RES_NEXUS)) borg_skill[BI_RNXUS] = TRUE;
	if (f2 & (TR2_RES_NETHER)) borg_skill[BI_RNTHR] = TRUE;
	if (f2 & (TR2_RES_CHAOS)) borg_skill[BI_RKAOS] = TRUE;
	if (f2 & (TR2_RES_DISEN)) borg_skill[BI_RDIS] = TRUE;
	if (f2 & (TR2_REFLECT)) borg_skill[BI_REFLECT] = TRUE;

	/* Sustain flags */
	if (f2 & (TR2_SUST_STR)) borg_skill[BI_SSTR] = TRUE;
	if (f2 & (TR2_SUST_INT)) borg_skill[BI_SINT] = TRUE;
	if (f2 & (TR2_SUST_WIS)) borg_skill[BI_SWIS] = TRUE;
	if (f2 & (TR2_SUST_DEX)) borg_skill[BI_SDEX] = TRUE;
	if (f2 & (TR2_SUST_CON)) borg_skill[BI_SCON] = TRUE;
	if (f2 & (TR2_SUST_CHR)) borg_skill[BI_SCHR] = TRUE;

	/* Hack -- Reward High Level Warriors with Res Fear */
	if (borg_class == CLASS_WARRIOR)
	{
		/* Resist fear at level 30 */
		if (borg_skill[BI_CLEVEL] >= 30) borg_skill[BI_RFEAR] = TRUE;
	}

	/* Bloating slows the player down (a little) */
	if (borg_skill[BI_ISGORGED]) borg_skill[BI_SPEED] -= 10;
}

/*
 * Find which item goes in an equipment slot.
 *
 * Normally, this is just the item already there,
 * however, sometimes we want to simulate another
 * item being in that location.
 *
 * Note: there must only be one TREAT_AS_SWAP
 * item in the inventory at any time.
 */
static list_item *look_up_equip_slot(int slot)
{
	list_item *l_ptr;
	
	int i;
	
	/* Look in equipment */
	l_ptr = &equipment[slot];
	
	/* Does it exist and are we aware? */
	if (l_ptr->k_idx)
	{
		/* Normal item? */
		if (l_ptr->treat_as == TREAT_AS_NORM) return (l_ptr);
		
		/* Missing item? */
		if (l_ptr->treat_as == TREAT_AS_GONE) return (NULL);
		
		/* Assume TREAT_AS_SWAP */
	}
	else
	{
		/* Optimise common case of empty slot */
		if (l_ptr->treat_as != TREAT_AS_SWAP) return (NULL);
	}
	
	/* Otherwise, scan the inventory */
	for (i = 0; i < inven_num; i++)
	{
		l_ptr = &inventory[i];
		
		/* Does it exist and are we aware? */
		if (l_ptr->k_idx)
		{
			/* The item to swap with */
			if (l_ptr->treat_as == TREAT_AS_SWAP) return (l_ptr);
		}
	}
	
	/* No match! */
	return (NULL);
}

/*
 * Notice the effects of equipment
 */
static void borg_notice_equip(int *extra_blows, int *extra_shots,
                              int *extra_might)
{
	int i;

	list_item *l_ptr;

	/* Scan the equipment */
	for (i = 0; i < equip_num; i++)
	{
		l_ptr = look_up_equip_slot(i);

		/* Pretend item isn't there */
		if (!l_ptr) continue;

		/* Check for cursed items */
		if (l_ptr->kn_flags3 & TR3_CURSED) borg_wearing_cursed = TRUE;

		if (l_ptr->treat_as == TREAT_AS_LESS)
		{
			/* Track number of items the borg has on him less one. */
			borg_has_on[l_ptr->k_idx] += l_ptr->number - 1;
		}
		else
		{
			/* Track number of items the borg has on him */
			borg_has_on[l_ptr->k_idx] += l_ptr->number;
		}

		/* Affect stats */
		if (l_ptr->kn_flags1 & TR1_STR) my_stat_add[A_STR] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_INT) my_stat_add[A_INT] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_WIS) my_stat_add[A_WIS] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_DEX) my_stat_add[A_DEX] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_CON) my_stat_add[A_CON] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_CHR) my_stat_add[A_CHR] += l_ptr->pval;

		/* Various slays */
		if (l_ptr->kn_flags1 & TR1_SLAY_ANIMAL) borg_skill[BI_WS_ANIMAL] = TRUE;
		if (l_ptr->kn_flags1 & TR1_SLAY_EVIL) borg_skill[BI_WS_EVIL] = TRUE;
		if (l_ptr->kn_flags1 & TR1_SLAY_UNDEAD) borg_skill[BI_WS_UNDEAD] = TRUE;
		if (l_ptr->kn_flags1 & TR1_SLAY_DEMON) borg_skill[BI_WS_DEMON] = TRUE;
		if (l_ptr->kn_flags1 & TR1_SLAY_ORC) borg_skill[BI_WS_ORC] = TRUE;
		if (l_ptr->kn_flags1 & TR1_SLAY_TROLL) borg_skill[BI_WS_TROLL] = TRUE;
		if (l_ptr->kn_flags1 & TR1_SLAY_GIANT) borg_skill[BI_WS_GIANT] = TRUE;
		if (l_ptr->kn_flags1 & TR1_SLAY_DRAGON) borg_skill[BI_WS_DRAGON] = TRUE;
		if (l_ptr->kn_flags1 & TR1_KILL_DRAGON) borg_skill[BI_WK_DRAGON] = TRUE;
		if (l_ptr->kn_flags1 & TR1_IMPACT) borg_skill[BI_W_IMPACT] = TRUE;
		if (l_ptr->kn_flags1 & TR1_BRAND_ACID) borg_skill[BI_WB_ACID] = TRUE;
		if (l_ptr->kn_flags1 & TR1_BRAND_ELEC) borg_skill[BI_WB_ELEC] = TRUE;
		if (l_ptr->kn_flags1 & TR1_BRAND_FIRE) borg_skill[BI_WB_FIRE] = TRUE;
		if (l_ptr->kn_flags1 & TR1_BRAND_COLD) borg_skill[BI_WB_COLD] = TRUE;
		if (l_ptr->kn_flags1 & TR1_BRAND_POIS) borg_skill[BI_WB_POIS] = TRUE;
		if (l_ptr->kn_flags1 & TR1_VORPAL) borg_skill[BI_WB_VORPAL] = TRUE;
		if (l_ptr->kn_flags1 & TR1_VAMPIRIC) borg_skill[BI_WB_VAMPIRIC] = TRUE;
		if (l_ptr->kn_flags1 & TR1_CHAOTIC) borg_skill[BI_WB_CHAOTIC] = TRUE;

		/* Affect infravision */
		if (l_ptr->kn_flags1 & TR1_INFRA) borg_skill[BI_INFRA] += l_ptr->pval;

		/* Affect stealth */
		if (l_ptr->kn_flags1 & TR1_STEALTH) borg_skill[BI_STL] += l_ptr->pval;

		/* Affect searching ability (factor of five) */
		if (l_ptr->kn_flags1 & TR1_SEARCH) borg_skill[BI_SRCH] +=
				l_ptr->pval * 5;

		/* Affect searching frequency (factor of five) */
		if (l_ptr->kn_flags1 & TR1_SEARCH) borg_skill[BI_SRCHFREQ] +=
				(l_ptr->pval * 5);

		/* Affect digging (factor of 20) */
		if (l_ptr->kn_flags1 & TR1_TUNNEL) borg_skill[BI_DIG] +=
				l_ptr->pval * 20;

		/* Affect speed */
		if (l_ptr->kn_flags1 & TR1_SPEED) borg_skill[BI_SPEED] += l_ptr->pval;

		/* Affect blows */
		if (l_ptr->kn_flags1 & TR1_BLOWS) *extra_blows += l_ptr->pval;

		/* Boost shots */
		if (l_ptr->kn_flags3 & TR3_XTRA_SHOTS) (*extra_shots)++;

		/* Boost might */
		if (l_ptr->kn_flags3 & TR3_XTRA_MIGHT) (*extra_might)++;

		/* Various flags */
		if (l_ptr->kn_flags3 & TR3_SLOW_DIGEST) borg_skill[BI_SDIG] = TRUE;
		if (l_ptr->kn_flags3 & TR3_AGGRAVATE) borg_skill[BI_CRSAGRV] = TRUE;
		if (l_ptr->kn_flags3 & TR3_TY_CURSE) borg_skill[BI_CRSTY] = TRUE;
		if (l_ptr->kn_flags3 & TR3_TELEPORT) borg_skill[BI_CRSTELE] = TRUE;
		if (l_ptr->kn_flags3 & TR3_NO_TELE) borg_skill[BI_CRSNOTELE] = TRUE;
		if (l_ptr->kn_flags3 & TR3_NO_MAGIC) borg_skill[BI_CRSNOMAGIC] = TRUE;

		if (l_ptr->kn_flags3 & TR3_REGEN) borg_skill[BI_REG] = TRUE;
		if (l_ptr->kn_flags3 & TR3_TELEPATHY) borg_skill[BI_ESP] = TRUE;
		if (l_ptr->kn_flags3 & TR3_LITE) borg_skill[BI_LITE] = TRUE;
		if (l_ptr->kn_flags3 & TR3_SEE_INVIS) borg_skill[BI_SINV] = TRUE;
		if (l_ptr->kn_flags3 & TR3_FEATHER) borg_skill[BI_FEATH] = TRUE;
		if (l_ptr->kn_flags2 & TR2_FREE_ACT) borg_skill[BI_FRACT] = TRUE;
		if (l_ptr->kn_flags2 & TR2_HOLD_LIFE) borg_skill[BI_HLIFE] = TRUE;

		/* Immunity flags */
		if (l_ptr->kn_flags2 & TR2_IM_FIRE)
		{
			borg_skill[BI_IFIRE] = TRUE;
			borg_skill[BI_RFIRE] = TRUE;
			my_oppose_fire = TRUE;
		}
		if (l_ptr->kn_flags2 & TR2_IM_ACID)
		{
			borg_skill[BI_IACID] = TRUE;
			borg_skill[BI_RACID] = TRUE;
			my_oppose_elec = TRUE;
		}
		if (l_ptr->kn_flags2 & TR2_IM_COLD)
		{
			borg_skill[BI_ICOLD] = TRUE;
			borg_skill[BI_RCOLD] = TRUE;
			my_oppose_elec = TRUE;
		}
		if (l_ptr->kn_flags2 & TR2_IM_ELEC)
		{
			borg_skill[BI_IELEC] = TRUE;
			borg_skill[BI_RELEC] = TRUE;
			my_oppose_elec = TRUE;
		}

		/* Resistance flags */
		if (l_ptr->kn_flags2 & TR2_RES_ACID) borg_skill[BI_RACID] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_ELEC) borg_skill[BI_RELEC] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_FIRE) borg_skill[BI_RFIRE] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_COLD) borg_skill[BI_RCOLD] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_POIS) borg_skill[BI_RPOIS] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_CONF) borg_skill[BI_RCONF] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_SOUND) borg_skill[BI_RSND] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_LITE) borg_skill[BI_RLITE] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_DARK) borg_skill[BI_RDARK] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_CHAOS) borg_skill[BI_RKAOS] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_DISEN) borg_skill[BI_RDIS] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_SHARDS) borg_skill[BI_RSHRD] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_NEXUS) borg_skill[BI_RNXUS] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_BLIND) borg_skill[BI_RBLIND] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_NETHER) borg_skill[BI_RNTHR] = TRUE;
		if (l_ptr->kn_flags2 & TR2_REFLECT) borg_skill[BI_REFLECT] = TRUE;
		if (l_ptr->kn_flags2 & TR2_RES_FEAR) borg_skill[BI_RFEAR] = TRUE;

		/* Sustain flags */
		if (l_ptr->kn_flags2 & TR2_SUST_STR) borg_skill[BI_SSTR] = TRUE;
		if (l_ptr->kn_flags2 & TR2_SUST_INT) borg_skill[BI_SINT] = TRUE;
		if (l_ptr->kn_flags2 & TR2_SUST_WIS) borg_skill[BI_SWIS] = TRUE;
		if (l_ptr->kn_flags2 & TR2_SUST_DEX) borg_skill[BI_SDEX] = TRUE;
		if (l_ptr->kn_flags2 & TR2_SUST_CON) borg_skill[BI_SCON] = TRUE;
		if (l_ptr->kn_flags2 & TR2_SUST_CHR) borg_skill[BI_SCHR] = TRUE;

		/* Modify the base armor class */
		borg_skill[BI_ARMOR] += l_ptr->ac;

		/* Apply the bonuses to armor class */
		borg_skill[BI_ARMOR] += l_ptr->to_a;

		/* Keep track of weight */
		borg_skill[BI_ENCUMBERD] += l_ptr->weight;

		/* Hack -- do not apply "weapon" bonuses */
		if (i == EQUIP_WIELD) continue;

		/* Hack -- do not apply "bow" bonuses */
		if (i == EQUIP_BOW) continue;

		/* Apply the bonuses to hit/damage */
		borg_skill[BI_TOHIT] += l_ptr->to_h;
		borg_skill[BI_TODAM] += l_ptr->to_d;
	}
}


/*
 * Recalculate player stats
 */
static void borg_notice_stats(void)
{
	int i;

	/* Update "stats" */
	for (i = 0; i < A_MAX; i++)
	{
		int use, ind, add;

		add = my_stat_add[i];

		/* Modify the stats for race/class */
		add += (rp_ptr->r_adj[i] + cp_ptr->c_adj[i]);

		/* Extract the new "use_stat" value for the stat */
		use = modify_stat_value(my_stat_cur[i], add);

		/* Save the stat */
		my_stat_use[i] = use;

		/* Values: 3, ..., 17 */
		if (use <= 18) ind = (use - 3);

		/* Ranges: 18/00-18/09, ..., 18/210-18/219 */
		else if (use <= 18 + 219) ind = (15 + (use - 18) / 10);

		/* Range: 18/220+ */
		else
			ind = (37);

		/* Save the index */
		if (ind > 37)
			my_stat_ind[i] = 37;
		else
			my_stat_ind[i] = p_ptr->stat_ind[i];
		borg_skill[BI_STR + i] = my_stat_ind[i];
		borg_skill[BI_CSTR + i] = borg_stat[i];
	}


	borg_skill[BI_HP_ADJ] =
		(((adj_con_mhp[my_stat_ind[A_CON]] - 128) * borg_skill[BI_CLEVEL]) / 2);

	/* 'Mana' is actually the 'mana adjustment' */
	if (borg_skill[BI_WISMANA])
	{
		borg_skill[BI_SP_ADJ] =
			((adj_mag_mana[my_stat_ind[A_WIS]] * borg_skill[BI_CLEVEL]) / 2);
		borg_skill[BI_FAIL1] = adj_mag_stat[my_stat_ind[A_WIS]];
		borg_skill[BI_FAIL2] = adj_mag_fail[my_stat_ind[A_WIS]];
	}
	if (borg_skill[BI_INTMANA])
	{
		borg_skill[BI_SP_ADJ] =
			((adj_mag_mana[my_stat_ind[A_INT]] * borg_skill[BI_CLEVEL]) / 2);
		borg_skill[BI_FAIL1] = adj_mag_stat[my_stat_ind[A_INT]];
		borg_skill[BI_FAIL2] = adj_mag_fail[my_stat_ind[A_INT]];
	}

	/* Actual Modifier Bonuses (Un-inflate stat bonuses) */
	borg_skill[BI_ARMOR] += ((int)(adj_dex_ta[my_stat_ind[A_DEX]]) - 128);
	borg_skill[BI_TODAM] += ((int)(adj_str_td[my_stat_ind[A_STR]]) - 128);
	borg_skill[BI_TOHIT] += ((int)(adj_dex_th[my_stat_ind[A_DEX]]) - 128);
}


/*
 * Examine bow
 */
static void borg_notice_shooter(int hold, int extra_might, int extra_shots)
{
	list_item *l_ptr;

	/* Start with a single shot per turn */
	int my_num_fire = 1;

	/* Reset the "ammo" tval to darts by default */
	my_ammo_tval = 0;

	/* Reset the "ammo" sides for darts */
	my_ammo_sides = 4;

	/* Reset the shooting power */
	my_ammo_power = 0;

	/* Reset the shooting range */
	my_ammo_range = 0;

	/* Examine the "current bow" */
	l_ptr = look_up_equip_slot(EQUIP_BOW);
	
	/* No bow? */
	if (!l_ptr) return;

	/* and assume we can enchant up to +8 if borg_skill[BI_CLEVEL] > 25 */
	borg_skill[BI_BTOHIT] = l_ptr->to_h;
	if (borg_skill[BI_BTOHIT] < 8 && borg_skill[BI_CLEVEL] >= 25)
		borg_skill[BI_BTOHIT] = 8;

	borg_skill[BI_BTODAM] = l_ptr->to_d;
	if (borg_skill[BI_BTODAM] < 8 && borg_skill[BI_CLEVEL] >= 25)
		borg_skill[BI_BTODAM] = 8;

	/* It is hard to carry a heavy bow */
	if (hold < l_ptr->weight / 10)
	{
		borg_skill[BI_HEAVYBOW] = TRUE;

		/* Hard to wield a heavy bow */
		borg_skill[BI_TOHIT] += 2 * (hold - l_ptr->weight / 10);
	}

	/* Compute "extra shots" if needed */
	if (l_ptr->k_idx && (hold >= l_ptr->weight / 10))
	{
		/* Take note of required "tval" for missiles */
		switch (k_info[l_ptr->k_idx].sval)
		{
			case SV_SLING:
			{
				my_ammo_tval = TV_SHOT;
				my_ammo_sides = 3;
				my_ammo_power = 2;
				break;
			}

			case SV_SHORT_BOW:
			{
				my_ammo_tval = TV_ARROW;
				my_ammo_sides = 4;
				my_ammo_power = 2;
				break;
			}

			case SV_LONG_BOW:
			{
				my_ammo_tval = TV_ARROW;
				my_ammo_sides = 4;
				my_ammo_power = 3;
				break;
			}

			case SV_LIGHT_XBOW:
			{
				my_ammo_tval = TV_BOLT;
				my_ammo_sides = 5;
				my_ammo_power = 3;
				break;
			}

			case SV_HEAVY_XBOW:
			{
				my_ammo_tval = TV_BOLT;
				my_ammo_sides = 5;
				my_ammo_power = 4;
				break;
			}
		}

		/* Add in extra power */
		my_ammo_power += extra_might;

		/* Calculate total range */
		my_ammo_range = 10 + my_ammo_power * 5;

		/* Hack -- Reward High Level Rangers using Bows */
		if ((borg_class == CLASS_RANGER) && (my_ammo_tval == TV_ARROW))
		{
			/* Extra shot at level 20 */
			if (borg_skill[BI_CLEVEL] >= 20) my_num_fire++;

			/* Extra shot at level 40 */
			if (borg_skill[BI_CLEVEL] >= 40) my_num_fire++;
		}

		/* Add in the "bonus shots" */
		my_num_fire += extra_shots;

		/* Require at least one shot */
		if (my_num_fire < 1) my_num_fire = 1;
	}
	borg_skill[BI_SHOTS] = my_num_fire;

	/* Calculate "average" damage per "normal" shot (times 2) */
	borg_skill[BI_BMAXDAM] =
		(my_ammo_sides + borg_skill[BI_BTODAM]) * my_ammo_power;
	borg_skill[BI_BMAXDAM] *= borg_skill[BI_SHOTS];
}


/*
 * Examine sword
 */
static void borg_notice_weapon(int hold, int extra_blows)
{
	list_item *l_ptr;

	/* Examine the "main weapon" */
	l_ptr = look_up_equip_slot(EQUIP_WIELD);
	
	/* No weapon? */
	if (!l_ptr) return;

	/* and assume we can enchant up to +8 if borg_skill[BI_CLEVEL] > 25 */
	borg_skill[BI_WTOHIT] = l_ptr->to_h;
	if (borg_skill[BI_WTOHIT] < 8 && borg_skill[BI_CLEVEL] >= 25)
		borg_skill[BI_WTOHIT] = 8;
	borg_skill[BI_WTODAM] = l_ptr->to_d;
	if (borg_skill[BI_WTODAM] < 8 && borg_skill[BI_CLEVEL] >= 25)
		borg_skill[BI_WTODAM] = 8;

	/* It is hard to hold a heavy weapon */
	if (hold < l_ptr->weight / 10)
	{
		borg_skill[BI_HEAVYWEPON] = TRUE;

		/* Hard to wield a heavy weapon */
		borg_skill[BI_TOHIT] += 2 * (hold - l_ptr->weight / 10);
	}

	/* Normal weapons */
	if (l_ptr->k_idx && (hold >= l_ptr->weight / 10))
	{
		int str_index, dex_index;
		int num = 0, wgt = 0, mul = 0, div = 0;

		/* Analyze the class */
		switch (borg_class)
		{
			case CLASS_WARRIOR:
			{
				/* Warrior */
				num = 5;
				wgt = 30;
				mul = 5;
				break;
			}

			case CLASS_MAGE:
			case CLASS_HIGH_MAGE:
			{
				/* Mage */
				num = 2;
				wgt = 40;
				mul = 2;
				break;
			}

			case CLASS_PRIEST:
			case CLASS_MINDCRAFTER:
			{
				/* Priest, Mindcrafter */
				num = 4;
				wgt = 35;
				mul = 3;
				break;
			}

			case CLASS_ROGUE:
			{
				/* Rogue */
				num = 4;
				wgt = 30;
				mul = 3;
				break;
			}

			case CLASS_RANGER:
			{
				/* Ranger */
				num = 4;
				wgt = 35;
				mul = 4;
				break;
			}

			case CLASS_PALADIN:
			{
				/* Paladin */
				num = 4;
				wgt = 30;
				mul = 4;
				break;
			}

			case CLASS_WARRIOR_MAGE:
			{
				/* Warrior-Mage */
				num = 4;
				wgt = 35;
				mul = 3;
				break;
			}

			case CLASS_CHAOS_WARRIOR:
			{
				/* Chaos Warrior */
				num = 4;
				wgt = 30;
				mul = 4;
				break;
			}

			case CLASS_MONK:
			{
				/* Monk */
				num = ((p_ptr->lev < 40) ? 2 : 3);
				wgt = 40;
				mul = 4;
				break;
			}
		}

		/* Enforce a minimum "weight" (tenth pounds) */
		div = ((l_ptr->weight < wgt) ? wgt : l_ptr->weight);

		/* Get the strength vs weight */
		str_index = (adj_str_blow[my_stat_ind[A_STR]] * mul / div);

		/* Maximal value */
		if (str_index > 11) str_index = 11;

		/* Index by dexterity */
		dex_index = (adj_dex_blow[my_stat_ind[A_DEX]]);

		/* Maximal value */
		if (dex_index > 11) dex_index = 11;

		/* Use the blows table */
		borg_skill[BI_BLOWS] = blows_table[str_index][dex_index];

		/* Maximal value */
		if (borg_skill[BI_BLOWS] > num) borg_skill[BI_BLOWS] = num;

		/* Add in the "bonus blows" */
		borg_skill[BI_BLOWS] += extra_blows;

		/* Require at least one blow */
		if (borg_skill[BI_BLOWS] < 1) borg_skill[BI_BLOWS] = 1;

		/* Boost digging skill by weapon weight */
		borg_skill[BI_DIG] += (l_ptr->weight / 10);

	}

	/* priest weapon penalty for non-blessed edged weapons */
	if ((borg_class == CLASS_PRIEST) &&
		((l_ptr->tval == TV_SWORD) || (l_ptr->tval == TV_POLEARM)) &&
		(!(l_ptr->kn_flags3 & TR3_BLESSED)))
	{
		/* Reduce the real bonuses */
		borg_skill[BI_TOHIT] -= 5;
		borg_skill[BI_TODAM] -= 5;
	}

	/* Calculate "max" damage per "normal" blow  */
	/* and assume we can enchant up to +8 if borg_skill[BI_CLEVEL] > 25 */
	borg_skill[BI_WMAXDAM] =
		(l_ptr->dd * l_ptr->ds + borg_skill[BI_TODAM] + borg_skill[BI_WTODAM]);

	/* Calculate base damage, used to calculating slays */
	borg_skill[BI_WBASEDAM] = (l_ptr->dd * l_ptr->ds);
}


/*
 * Notice skills
 */
static void borg_notice_skills(void)
{
	/* Affect Skill -- stealth (bonus one) */
	borg_skill[BI_STL] += 1;

	/* Affect Skill -- disarming (DEX and INT) */
	borg_skill[BI_DIS] += adj_dex_dis[my_stat_ind[A_DEX]];
	borg_skill[BI_DIS] += adj_int_dis[my_stat_ind[A_INT]];

	/* Affect Skill -- magic devices (INT) */
	borg_skill[BI_DEV] += adj_int_dev[my_stat_ind[A_INT]];

	/* Affect Skill -- saving throw (WIS) */
	borg_skill[BI_SAV] += adj_wis_sav[my_stat_ind[A_WIS]];

	/* Affect Skill -- digging (STR) */
	borg_skill[BI_DIG] += adj_str_dig[my_stat_ind[A_STR]];


	/* Affect Skill -- disarming (Level, by Class) */
	borg_skill[BI_DIS] += (cb_ptr->x_dis * borg_skill[BI_MAXCLEVEL] / 10);

	/* Affect Skill -- magic devices (Level, by Class) */
	borg_skill[BI_DEV] += (cb_ptr->x_dev * borg_skill[BI_MAXCLEVEL] / 10);

	/* Affect Skill -- saving throw (Level, by Class) */
	borg_skill[BI_SAV] += (cb_ptr->x_sav * borg_skill[BI_MAXCLEVEL] / 10);

	/* Affect Skill -- stealth (Level, by Class) */
	borg_skill[BI_STL] += (cb_ptr->x_stl * borg_skill[BI_MAXCLEVEL] / 10);

	/* Affect Skill -- search ability (Level, by Class) */
	borg_skill[BI_SRCH] += (cb_ptr->x_sns * borg_skill[BI_MAXCLEVEL] / 10);

	/* Affect Skill -- search frequency (Level, by Class) */
	borg_skill[BI_SRCHFREQ] += (cb_ptr->x_fos * borg_skill[BI_MAXCLEVEL] / 10);

	/* Affect Skill -- combat (normal) (Level, by Class) */
	borg_skill[BI_THN] += (cb_ptr->x_thn * borg_skill[BI_MAXCLEVEL] / 10);

	/* Affect Skill -- combat (shooting) (Level, by Class) */
	borg_skill[BI_THB] += (cb_ptr->x_thb * borg_skill[BI_MAXCLEVEL] / 10);

	/* Affect Skill -- combat (throwing) (Level, by Class) */
	borg_skill[BI_THT] += (cb_ptr->x_thb * borg_skill[BI_MAXCLEVEL] / 10);

	/* Limit Skill -- stealth from 0 to 30 */
	if (borg_skill[BI_STL] > 30) borg_skill[BI_STL] = 30;
	if (borg_skill[BI_STL] < 0) borg_skill[BI_STL] = 0;

	/* Limit Skill -- digging from 1 up */
	if (borg_skill[BI_DIG] < 1) borg_skill[BI_DIG] = 1;
}


/*
 * Monks are special
 */
static void borg_recalc_monk(int extra_blows)
{
	int monk_arm_wgt = 0;
	int ma = MAX_MA - 1;
	const martial_arts *ma_ptr = &ma_blows[MAX_MA];
	
	int i;
	
	list_item *l_ptr;

	/* Weigh the armor */
	for (i = EQUIP_BODY; i <= EQUIP_FEET; i++)
	{
		l_ptr = look_up_equip_slot(i);
		
		/* Add up the total */
		if (l_ptr) monk_arm_wgt += l_ptr->weight;
	}

	/* Consider the Martial Arts */
	if (!look_up_equip_slot(EQUIP_WIELD))
	{
		borg_skill[BI_BLOWS] = 2;

		if (borg_skill[BI_CLEVEL] > 9) borg_skill[BI_BLOWS]++;
		if (borg_skill[BI_CLEVEL] > 14) borg_skill[BI_BLOWS]++;
		if (borg_skill[BI_CLEVEL] > 24) borg_skill[BI_BLOWS]++;
		if (borg_skill[BI_CLEVEL] > 34) borg_skill[BI_BLOWS]++;
		if (borg_skill[BI_CLEVEL] > 44) borg_skill[BI_BLOWS]++;
		if (borg_skill[BI_CLEVEL] > 49) borg_skill[BI_BLOWS]++;

		if (monk_arm_wgt < (100 + (borg_skill[BI_CLEVEL] * 4)))
		{
			borg_skill[BI_TOHIT] += (borg_skill[BI_CLEVEL] / 3);
			borg_skill[BI_TODAM] += (borg_skill[BI_CLEVEL] / 3);
		}
		else
		{
			borg_skill[BI_BLOWS] /= 2;
		}

		borg_skill[BI_BLOWS] += extra_blows;

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

		/* Calculate "max" damage per monk blow  */
		borg_skill[BI_WMAXDAM] =
			(ma_ptr->dd * ma_ptr->ds + borg_skill[BI_TODAM]);

		/* Calculate base damage, used to calculating slays */
		borg_skill[BI_WBASEDAM] = (ma_ptr->dd * ma_ptr->ds);
	}

	/** Monk Armour **/

	/* Unencumbered Monks become faster every 10 levels */
	if (monk_arm_wgt < (100 + (borg_skill[BI_CLEVEL] * 4)))
	{
		borg_skill[BI_SPEED] += (borg_skill[BI_CLEVEL]) / 10;

		/* Free action if unencumbered at level 25 */
		if (borg_skill[BI_CLEVEL] > 24) borg_skill[BI_FRACT] = TRUE;

		if (!look_up_equip_slot(EQUIP_BODY))
		{
			borg_skill[BI_ARMOR] += (borg_skill[BI_CLEVEL] * 3) / 2;
		}
		if (!look_up_equip_slot(EQUIP_OUTER) && (borg_skill[BI_CLEVEL] > 15))
		{
			borg_skill[BI_ARMOR] += ((borg_skill[BI_CLEVEL] - 13) / 3);
		}
		if (!look_up_equip_slot(EQUIP_ARM) && (borg_skill[BI_CLEVEL] > 10))
		{
			borg_skill[BI_ARMOR] += ((borg_skill[BI_CLEVEL] - 8) / 3);
		}
		if (!look_up_equip_slot(EQUIP_HEAD) && (borg_skill[BI_CLEVEL] > 4))
		{
			borg_skill[BI_ARMOR] += (borg_skill[BI_CLEVEL] - 2) / 3;
		}
		if (!look_up_equip_slot(EQUIP_HANDS))
		{
			borg_skill[BI_ARMOR] += (borg_skill[BI_CLEVEL] / 2);
		}
		if (!look_up_equip_slot(EQUIP_FEET))
		{
			borg_skill[BI_ARMOR] += (borg_skill[BI_CLEVEL] / 3);
		}
	}
}


/*
 * Recalculate required enchantment levels
 */
static void borg_notice_enchant(void)
{
	list_item *l_ptr;

	int i;

	/* Assume no enchantment needed */
	my_need_enchant_to_a = 0;
	my_need_enchant_to_h = 0;
	my_need_enchant_to_d = 0;
	my_need_brand_weapon = 0;

	/* Hack -- enchant all the equipment (weapons) */
	for (i = 0; i <= EQUIP_BOW; i++)
	{
		l_ptr = look_up_equip_slot(i);
		
		/* Skip missing items */
		if (!l_ptr) continue;

		/* Skip "unknown" items */
		if (!(l_ptr->info & OB_KNOWN)) continue;

		/* Enchant all weapons (to hit) */
		if ((borg_spell_okay_fail(REALM_SORCERY, 3, 4, 40) ||
			 amt_enchant_weapon >= 1))
		{
			if (l_ptr->to_h < 15)
			{
				my_need_enchant_to_h += (15 - l_ptr->to_h);
			}

			/* Enchant all weapons (to damage) */
			if (l_ptr->to_d < 15)
			{
				my_need_enchant_to_d += (15 - l_ptr->to_d);
			}
		}
		else
		{
			if (l_ptr->to_h < 8)
			{
				my_need_enchant_to_h += (8 - l_ptr->to_h);
			}

			/* Enchant all weapons (to damage) */
			if (l_ptr->to_d < 8)
			{
				my_need_enchant_to_d += (8 - l_ptr->to_d);
			}
		}
	}

	/* Hack -- enchant all the equipment (armor) */
	for (i = EQUIP_BODY; i <= EQUIP_FEET; i++)
	{
		l_ptr = look_up_equip_slot(i);

		/* Skip empty items */
		if (!l_ptr) continue;

		/* Skip "unknown" items */
		if (!(l_ptr->info & OB_KNOWN)) continue;

		/* Note need for enchantment */
		if ((borg_spell_okay_fail(REALM_SORCERY, 3, 5, 40) ||
			 amt_enchant_armor >= 1))
		{
			if (l_ptr->to_a < 15)
			{
				my_need_enchant_to_a += (15 - l_ptr->to_a);
			}
		}
		else
		{
			if (l_ptr->to_a < 10)
			{
				my_need_enchant_to_a += (10 - l_ptr->to_a);
			}
		}
	}
}


/*
 * Notice changes in lighting
 */
static void borg_notice_lite(void)
{
	list_item *l_ptr;

	/* Assume normal lite radius */
	borg_skill[BI_CUR_LITE] = 0;

	/* Glowing player has light */
	if (borg_skill[BI_LITE]) borg_skill[BI_CUR_LITE] = 1;

	/* Vampires that do not Resist Light are in trouble */
	if (borg_race == RACE_VAMPIRE && !borg_skill[BI_RLITE])
		borg_skill[BI_FEAR_LITE] = TRUE;
	
	/* Examine the lite */
	l_ptr = look_up_equip_slot(EQUIP_LITE);
	
	/* Item missing? */
	if (!l_ptr) return;
	
	/* No need for fuel */
	if (l_ptr->kn_flags3 & TR3_LITE) borg_skill[BI_AFUEL] += 1000;

	/* Lite */
	if (l_ptr->tval == TV_LITE)
	{
		object_kind *k_ptr = &k_info[l_ptr->k_idx];
		
		/* No fuel means no radius */
		if (l_ptr->timeout)
		{
			/* Torches -- radius one */
			if (k_ptr->sval == SV_LITE_TORCH) borg_skill[BI_CUR_LITE] += 1;

			/* Lanterns -- radius two */
			if (k_ptr->sval == SV_LITE_LANTERN) borg_skill[BI_CUR_LITE] += 2;
		}

		/* Artifact lites -- radius three */
		if (l_ptr->kn_flags3 & TR3_INSTA_ART)
		{
			borg_skill[BI_CUR_LITE] += 3;

			/* Artifact lites -- assume glowing */
			borg_skill[BI_LITE] = TRUE;

			/* Vampires need to be concerned with Artifacts Lites */
			if (borg_skill[BI_FEAR_LITE])
			{
				borg_skill[BI_CUR_LITE] = 1;

				/* Artifact lites -- assume glowing */
				borg_skill[BI_LITE] = TRUE;
			}
		}
	}
}


/*
 * Helper function -- notice the player equipment
 */
static void borg_notice_aux1(void)
{
	int i, hold;

	int extra_blows = 0;

	int extra_shots = 0;
	int extra_might = 0;

	/* Notice player flags */
	borg_notice_player();

	/* Clear the stat modifiers */
	for (i = 0; i < 6; i++) my_stat_add[i] = 0;

	/* Notice equipment */
	borg_notice_equip(&extra_blows, &extra_shots, &extra_might);

	/* Recalculate the stats */
	borg_notice_stats();

	/* Obtain the "hold" value */
	hold = adj_str_hold[my_stat_ind[A_STR]];

	/* Examine ranged weapon */
	borg_notice_shooter(hold, extra_might, extra_shots);

	/* Examine melee weapon */
	borg_notice_weapon(hold, extra_blows);

	/* Recalculate skills */
	borg_notice_skills();

	/* Monks get bonus for not using weapon or armour */
	if (borg_class == CLASS_MONK)
	{
		borg_recalc_monk(extra_blows);
	}

	/* See if we need to enchant anything */
	borg_notice_enchant();

	/* Examine lite */
	borg_notice_lite();
}


/*
 * Notice food
 */
static void borg_notice_food(list_item *l_ptr, int number)
{
	int sval = k_info[l_ptr->k_idx].sval;

	/* Analyze */
	switch (sval)
	{
		case SV_FOOD_WAYBREAD:
		{
			amt_food_hical += number;
			break;
		}
		case SV_FOOD_RATION:
		{
			amt_food_hical += number;
			break;
		}
		case SV_FOOD_JERKY:
		{
			amt_food_lowcal += number;
			break;
		}
		case SV_FOOD_BISCUIT:
		{
			amt_food_lowcal += number;
			break;
		}
		case SV_FOOD_SLIME_MOLD:
		{
			amt_food_lowcal += number;
			break;
		}

		case SV_FOOD_RESTORE_STR:
		{
			amt_fix_stat[A_STR] += number;
			break;
		}
		case SV_FOOD_RESTORE_CON:
		{
			amt_fix_stat[A_CON] += number;
			break;
		}
		case SV_FOOD_RESTORING:
		{
			amt_fix_stat[A_STR] += number;
			amt_fix_stat[A_INT] += number;
			amt_fix_stat[A_WIS] += number;
			amt_fix_stat[A_DEX] += number;
			amt_fix_stat[A_CON] += number;
			amt_fix_stat[A_CHR] += number;
			amt_fix_stat[6] += number;
			break;
		}

		case SV_FOOD_CURE_CONFUSION:
		{
			amt_cure_confusion += number;
			break;
		}

		case SV_FOOD_CURE_BLINDNESS:
		{
			amt_cure_blind += number;
			break;
		}

		case SV_FOOD_CURE_POISON:
		{
			borg_skill[BI_ACUREPOIS] += number;
			break;
		}
	}
}


/*
 * Notice Potions
 */
static void borg_notice_potions(list_item *l_ptr, int number)
{
	int sval = k_info[l_ptr->k_idx].sval;

	/* Analyze */
	switch (sval)
	{
		case SV_POTION_HEALING:
		{
			borg_skill[BI_AHEAL] += number;
			break;
		}
		case SV_POTION_STAR_HEALING:
		case SV_POTION_LIFE:
		{
			borg_skill[BI_AEZHEAL] += number;
			break;
		}
		case SV_POTION_CURE_CRITICAL:
		{
			borg_skill[BI_ACCW] += number;
			break;
		}
		case SV_POTION_CURE_SERIOUS:
		{
			borg_skill[BI_ACSW] += number;
			break;
		}
		case SV_POTION_CURE_LIGHT:
		{
			if (borg_skill[BI_ISCUT]) borg_skill[BI_ACSW] += number;
			break;
		}
		case SV_POTION_CURE_POISON:
		{
			borg_skill[BI_ACUREPOIS] += number;
			break;
		}
		case SV_POTION_SLOW_POISON:
		{
			amt_slow_poison += number;
			break;
		}
		case SV_POTION_RESIST_HEAT:
		{
			borg_skill[BI_ARESHEAT] += number;
			break;
		}
		case SV_POTION_RESIST_COLD:
		{
			borg_skill[BI_ARESCOLD] += number;
			break;
		}
		case SV_POTION_INC_STR:
		{
			amt_add_stat[A_STR] += number;
			break;
		}
		case SV_POTION_INC_INT:
		{
			amt_add_stat[A_INT] += number;
			break;
		}
		case SV_POTION_INC_WIS:
		{
			amt_add_stat[A_WIS] += number;
			break;
		}
		case SV_POTION_INC_DEX:
		{
			amt_add_stat[A_DEX] += number;
			break;
		}
		case SV_POTION_INC_CON:
		{
			amt_add_stat[A_CON] += number;
			break;
		}
		case SV_POTION_INC_CHR:
		{
			amt_add_stat[A_CHR] += number;
			break;
		}
		case SV_POTION_RES_STR:
		{
			amt_fix_stat[A_STR] += number;
			break;
		}
		case SV_POTION_RES_INT:
		{
			amt_fix_stat[A_INT] += number;
			break;
		}
		case SV_POTION_RES_WIS:
		{
			amt_fix_stat[A_WIS] += number;
			break;
		}
		case SV_POTION_RES_DEX:
		{
			amt_fix_stat[A_DEX] += number;
			break;
		}
		case SV_POTION_RES_CON:
		{
			amt_fix_stat[A_CON] += number;
			break;
		}
		case SV_POTION_RES_CHR:
		{
			amt_fix_stat[A_CHR] += number;
			break;
		}
		case SV_POTION_RESTORE_EXP:
		{
			amt_fix_exp += number;
			break;
		}
		case SV_POTION_SPEED:
		{
			borg_skill[BI_ASPEED] += number;
			break;
		}
	}
}


/*
 * Notice scrolls
 */
static void borg_notice_scrolls(list_item *l_ptr, int number)
{
	int sval = k_info[l_ptr->k_idx].sval;

	/* Analyze the scroll */
	switch (sval)
	{
		case SV_SCROLL_IDENTIFY:
		{
			borg_skill[BI_AID] += number;
			break;
		}
		case SV_SCROLL_RECHARGING:
		{
			borg_skill[BI_ARECHARGE] += number;
			break;
		}
		case SV_SCROLL_PHASE_DOOR:
		{
			amt_phase += number;
			break;
		}
		case SV_SCROLL_TELEPORT:
		{
			borg_skill[BI_AESCAPE] += number;
			borg_skill[BI_ATELEPORT] += number;
			break;
		}
		case SV_SCROLL_WORD_OF_RECALL:
		{
			borg_skill[BI_RECALL] += number;
			break;
		}
		case SV_SCROLL_ENCHANT_ARMOR:
		{
			amt_enchant_to_a += number;
			break;
		}
		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		{
			amt_enchant_to_h += number;
			break;
		}
		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			amt_enchant_to_d += number;
			break;
		}
		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			amt_enchant_weapon += number;
			break;
		}
		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			borg_skill[BI_APFE] += number;
			break;
		}
		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			amt_enchant_armor += number;
			break;
		}
		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			borg_skill[BI_AGLYPH] += number;
			break;
		}
		case SV_SCROLL_TELEPORT_LEVEL:
		{
			borg_skill[BI_ATELEPORTLVL] += number;
			break;
		}
		case SV_SCROLL_SATISFY_HUNGER:
		{
			borg_skill[BI_FOOD] += number;
			break;
		}
	}
}


/*
 * Notice rods
 */
static void borg_notice_rods(list_item *l_ptr, int number)
{
	object_kind *k_ptr = &k_info[l_ptr->k_idx];
	int sval = k_ptr->sval;

	/* Analyze */
	switch (sval)
	{
		case SV_ROD_IDENTIFY:
		{
			if (borg_skill[BI_DEV] - k_ptr->level > 7)
			{
				borg_skill[BI_AID] += number * 100;
			}
			else
			{
				borg_skill[BI_AID] += number;
			}
			break;
		}

		case SV_ROD_RECALL:
		{
			/* Don't count on it if I suck at activations */
			if (borg_skill[BI_DEV] - k_ptr->level > 7)
			{
				borg_skill[BI_RECALL] += number * 100;
			}
			else
			{
				borg_skill[BI_RECALL] += number;
			}
			break;
		}

		case SV_ROD_DETECT_TRAP:
		{
			borg_skill[BI_ADETTRAP] += number * 100;
			break;
		}

		case SV_ROD_DETECT_DOOR:
		{
			borg_skill[BI_ADETDOOR] += number * 100;
			break;
		}

		case SV_ROD_DETECTION:
		{
			borg_skill[BI_ADETTRAP] += number * 100;
			borg_skill[BI_ADETDOOR] += number * 100;
			borg_skill[BI_ADETEVIL] += number * 100;
			break;
		}

		case SV_ROD_SPEED:
		{
			/* Don't count on it if I suck at activations */
			if (borg_skill[BI_DEV] - k_ptr->level > 7)
			{
				borg_skill[BI_ASPEED] += number * 100;
			}
			else
			{
				borg_skill[BI_ASPEED] += number;
			}
			break;
		}

		case SV_ROD_MAPPING:
		{
			borg_skill[BI_AMAGICMAP] += number * 100;
			break;
		}

		case SV_ROD_HEALING:
		{
			/* only +2 per rod because of long charge time. */
			/* Don't count on it if I suck at activations */
			if (borg_skill[BI_DEV] - k_ptr->level > 7)
			{
				borg_skill[BI_AHEAL] += number * 2;
			}
			else
			{
				borg_skill[BI_AHEAL] += number;
			}
			break;
		}
	}
}


/*
 * Notice staves
 */
static void borg_notice_staves(list_item *l_ptr, int number)
{
	int sval = k_info[l_ptr->k_idx].sval;

	/*
	 * Staves should not be carried to Morgoth, he drains
	 * them to heal himself- not good at all
	 */
	if (borg_skill[BI_MAXDEPTH] >= 99 && !borg_skill[BI_KING])
	{
		/* skip these */
		return;
	}

	/* Analyze */
	switch (sval)
	{
		case SV_STAFF_IDENTIFY:
		{
			borg_skill[BI_AID] += number * l_ptr->pval;
			break;
		}
		case SV_STAFF_TELEPORTATION:
		{
			borg_skill[BI_ATELEPORT] += number * l_ptr->pval;
			break;
		}
		case SV_STAFF_SPEED:
		{
			borg_skill[BI_ASPEED] += number * l_ptr->pval;
			break;
		}
		case SV_STAFF_HEALING:
		{
			borg_skill[BI_AHEAL] += number * l_ptr->pval;
			break;
		}
		case SV_STAFF_THE_MAGI:
		{
			borg_skill[BI_ASTFMAGI] += number * l_ptr->pval;
			break;
		}
		case SV_STAFF_DESTRUCTION:
		{
			borg_skill[BI_ASTFDEST] += number * l_ptr->pval;
			break;
		}
		case SV_STAFF_POWER:
		{
			amt_cool_staff += number;
			break;
		}
		case SV_STAFF_HOLINESS:
		{
			amt_cool_staff += number;
			borg_skill[BI_AHEAL] += number * l_ptr->pval;
			break;
		}
	}
}


/*
 * Examine an item in the inventory
 */
static void borg_notice_inven_item(list_item *l_ptr)
{
	int number;
	
	object_kind *k_ptr;
	
	if (l_ptr->treat_as == TREAT_AS_LESS)
	{
		/* Pretend we have less items */
		number = l_ptr->number - 1;
	}
	else
	{
		number = l_ptr->number;
	}

	/* count up the items on the borg */
	borg_has[l_ptr->k_idx] += number;

	/* Keep track of weight */
	borg_skill[BI_ENCUMBERD] += l_ptr->weight * number;

	/* Get item type */
	k_ptr = &k_info[l_ptr->k_idx];

	/* Analyze the item */
	switch (l_ptr->tval)
	{
		case TV_LIFE_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_LIFE ||
				borg_skill[BI_REALM2] == REALM_LIFE)
				amt_book[REALM_LIFE][k_ptr->sval] += number;
			break;
		}
		case TV_SORCERY_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_SORCERY ||
				borg_skill[BI_REALM2] == REALM_SORCERY)
				amt_book[REALM_SORCERY][k_ptr->sval] += number;
			break;
		}
		case TV_NATURE_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_NATURE ||
				borg_skill[BI_REALM2] == REALM_NATURE)
				amt_book[REALM_NATURE][k_ptr->sval] += number;
			break;
		}
		case TV_CHAOS_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_CHAOS ||
				borg_skill[BI_REALM2] == REALM_CHAOS)
				amt_book[REALM_CHAOS][k_ptr->sval] += number;
			break;
		}
		case TV_DEATH_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_DEATH ||
				borg_skill[BI_REALM2] == REALM_DEATH)
				amt_book[REALM_DEATH][k_ptr->sval] += number;
			break;
		}
		case TV_TRUMP_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_TRUMP ||
				borg_skill[BI_REALM2] == REALM_TRUMP)
				amt_book[REALM_TRUMP][k_ptr->sval] += number;
			break;
		}
		case TV_ARCANE_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_ARCANE ||
				borg_skill[BI_REALM2] == REALM_ARCANE)
				amt_book[REALM_ARCANE][k_ptr->sval] += number;
			break;
		}


		case TV_FOOD:
		{
			/* Food */
			borg_notice_food(l_ptr, number);
			break;
		}

		case TV_POTION:
		{
			/* Potions */
			borg_notice_potions(l_ptr, number);
			break;
		}

		case TV_SCROLL:
		{
			/* Scrolls */
			borg_notice_scrolls(l_ptr, number);
			break;
		}

		case TV_ROD:
		{
			/* Rods */
			borg_notice_rods(l_ptr, number);
			break;
		}

		case TV_STAFF:
		{
			/* Staffs */
			borg_notice_staves(l_ptr, number);
			break;
		}


		case TV_FLASK:
		{
			/* Flasks */

			/* Use as fuel if we equip a lantern */
			if (borg_skill[BI_CUR_LITE] == 2) borg_skill[BI_AFUEL] +=
					number;

			/* Count as Missiles */
			if (borg_skill[BI_CLEVEL] < 15) borg_skill[BI_AMISSILES] +=
					number;

			break;
		}

		case TV_LITE:

		{
			/* Torches */

			/* Use as fuel if it is a torch and we carry a torch */
			if ((k_ptr->sval == SV_LITE_TORCH) &&
				(borg_skill[BI_CUR_LITE] <= 1))
			{
				borg_skill[BI_AFUEL] += number;
			}
			break;
		}


		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		{
			/* Weapons */


			/* These items are checked a bit later in a sub routine
			 * to notice the flags.  It is done outside this switch.
			 */
			break;
		}

		case TV_DIGGING:
		{
			/* Shovels and such */

			/* Hack -- ignore worthless ones (including cursed) */
			if (l_ptr->kn_flags3 & TR3_CURSED) break;

			/* Do not carry if weak, won't be able to dig anyway */
			if (borg_skill[BI_DIG] < 30) break;

			amt_digger += number;
			break;
		}

		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			/* Missiles */

			/* Hack -- ignore invalid missiles */
			if (l_ptr->tval != my_ammo_tval) break;

			/* Count them */
			borg_skill[BI_AMISSILES] += number;

			/* Enchant missiles if have lots of cash */
			if (borg_skill[BI_CLEVEL] > 35)
			{
				if (borg_spell_okay_fail(REALM_LIFE, 7, 3, 40) &&
					number >= 5)
				{
					if (l_ptr->to_h < 8)
					{
						my_need_enchant_to_h += (10 - l_ptr->to_h);
					}

					if (l_ptr->to_d < 8)
					{
						my_need_enchant_to_d += (10 - l_ptr->to_d);
					}
				}
				else
				{
					if (l_ptr->to_h < 5)
					{
						my_need_enchant_to_h += (8 - l_ptr->to_h);
					}

					if (l_ptr->to_d < 5)
					{
						my_need_enchant_to_d += (8 - l_ptr->to_d);
					}
				}
			}

			break;
		}
	}
}


/*
 * Notice the inventory
 */
static void borg_notice_inven(void)
{
	list_item *l_ptr;

	int i;
	
	/* Scan the inventory */
	for (i = 0; i < inven_num; i++)
	{
		l_ptr = &inventory[i];

		/* Pretend item isn't there */
		if (l_ptr->treat_as == TREAT_AS_GONE) continue;
		
		/* Unaware item? */
		if (!l_ptr->k_idx) continue;
		
		/* Examine the item */
		borg_notice_inven_item(l_ptr);
	}
	
	/* Search equipment for swapped items */
	for (i = 0; i < equip_num; i++)
	{
		l_ptr = &equipment[i];
	
		/* A known item? */
		if (l_ptr->k_idx)
		{
			if (l_ptr->treat_as == TREAT_AS_SWAP)
			{
				/* Examine the item */
				borg_notice_inven_item(l_ptr);
				
				/* Done (Only one extra item) */
				return;
			}
		}
	}
}


/*
 * Helper function -- notice the player inventory
 */
static void borg_notice_aux2(void)
{
	int i, ii;
	int carry_capacity;

	/*** Reset counters ***/


	/* Reset basic */
	amt_phase = 0;
	amt_food_lowcal = 0;
	amt_food_hical = 0;

	/* Reset healing */
	amt_slow_poison = 0;
	amt_cure_confusion = 0;
	amt_cure_blind = 0;

	/* Reset books */
	for (i = 0; i < MAX_REALM; i++)
	{
		for (ii = 0; ii < 4; ii++)
		{
			amt_book[i][ii] = 0;
		}
	}

	/* Reset various */
	amt_add_stat[A_STR] = 0;
	amt_add_stat[A_INT] = 0;
	amt_add_stat[A_WIS] = 0;
	amt_add_stat[A_DEX] = 0;
	amt_add_stat[A_CON] = 0;
	amt_add_stat[A_CHR] = 0;
	amt_fix_stat[A_STR] = 0;
	amt_fix_stat[A_INT] = 0;
	amt_fix_stat[A_WIS] = 0;
	amt_fix_stat[A_DEX] = 0;
	amt_fix_stat[A_CON] = 0;
	amt_fix_stat[A_CHR] = 0;
	amt_fix_stat[6] = 0;

	amt_fix_exp = 0;
	amt_cool_staff = 0;
	amt_digger = 0;

	/* Reset enchantment */
	amt_enchant_to_a = 0;
	amt_enchant_to_d = 0;
	amt_enchant_to_h = 0;

	amt_brand_weapon = 0;
	amt_enchant_weapon = 0;
	amt_enchant_armor = 0;

	/*** Process the inventory ***/
	borg_notice_inven();


	/*** Process the Spells and Prayers ***/
	/*  apw  artifact activations are accounted here
	 *  But some artifacts are not counted for two reasons .
	 *  1.  Some spells-powers are needed instantly and are considered in
	 *  the borg preparation code.  An artifact maybe non-charged at the
	 *  moment he needes it.  Then he would need the spell and not be able
	 *  to cast it. (ie. teleport, phase)
	 *  2.  An artifact may grant a power then he assumes he has infinite
	 *  amounts.  He then sells off his scrolls with the duplicate power.
	 *  When it comes time to upgrade and swap out the artifact, he wont
	 *  because his power drops since he does not have the scrolls anymore.
	 *  and he does not buy items first.
	 *
	 *  A possible solution would be to have him keep a few scrolls as a
	 *  back-up, or to remove the bonus for level preparation from borg_power.
	 *  Right now I think it is better that he not consider the artifacts
	 *  Whose powers are considered in borg_prep.
	 */

	/* Handle "satisfy hunger" -> infinite food */
	if (borg_spell_legal_fail(REALM_SORCERY, 2, 0, 10) ||
		borg_spell_legal_fail(REALM_LIFE, 0, 7, 10) ||
		borg_spell_legal_fail(REALM_ARCANE, 2, 7, 10) ||
		borg_spell_legal_fail(REALM_NATURE, 0, 3, 10) ||
		borg_racial_check(RACE_HOBBIT, TRUE))
	{
		borg_skill[BI_FOOD] += 1000;
	}

	/* Handle "identify" -> infinite identifies */
	if (borg_spell_legal(REALM_SORCERY, 1, 1) ||
		borg_spell_legal(REALM_ARCANE, 3, 2) ||
		borg_mindcr_legal(MIND_PSYCHOMETRY, 40))
	{
		borg_skill[BI_AID] += 1000;
	}

	/* Handle "detect traps, doors, stairs" */
	if (borg_spell_legal(REALM_LIFE, 0, 5) ||
		borg_spell_legal(REALM_SORCERY, 0, 2) ||
		borg_spell_legal(REALM_ARCANE, 1, 0) ||
		borg_spell_legal(REALM_NATURE, 1, 2) ||
		borg_mindcr_legal(MIND_PRECOGNIT, 2) ||
		borg_racial_check(RACE_DWARF, TRUE) ||
		borg_racial_check(RACE_NIBELUNG, TRUE))
	{
		borg_skill[BI_ADETTRAP] += 1000;
		borg_skill[BI_ADETDOOR] += 1000;
	}

	/* Handle "detect evil & monsters" */
	if (borg_spell_legal(REALM_LIFE, 0, 0) ||
		borg_spell_legal(REALM_SORCERY, 0, 0) ||
		borg_spell_legal(REALM_NATURE, 0, 0) ||
		borg_spell_legal(REALM_DEATH, 0, 2) ||
		borg_mindcr_legal(MIND_PRECOGNIT, 1))
	{
		borg_skill[BI_ADETEVIL] += 1000;
	}

	/* Handle "detection" */
	if (borg_mindcr_legal(MIND_PRECOGNIT, 30))
	{
		borg_skill[BI_ADETDOOR] += 1000;
		borg_skill[BI_ADETTRAP] += 1000;
		borg_skill[BI_ADETEVIL] += 1000;
	}

	/* Handle "magic mapping" */
	if (borg_spell_legal(REALM_SORCERY, 1, 0) ||
		borg_spell_legal(REALM_NATURE, 1, 2) ||
		borg_mindcr_legal(MIND_PRECOGNIT, 20))
	{
		borg_skill[BI_ADETDOOR] += 1000;
		borg_skill[BI_ADETTRAP] += 1000;
		borg_skill[BI_AMAGICMAP] += 1000;
	}

	/* Handle "protection from evil" */
	if (borg_spell_legal(REALM_LIFE, 1, 5))
	{
		borg_skill[BI_APFE] += 1000;
	}

	/* Handle "rune of protection" glyph" */
	if (borg_spell_legal(REALM_LIFE, 1, 7) ||
		borg_spell_legal(REALM_LIFE, 2, 7))
	{
		borg_skill[BI_AGLYPH] += 1000;
	}

	/* Handle "enchant weapon" */
	if (borg_spell_legal_fail(REALM_SORCERY, 3, 4, 40))
	{
		amt_enchant_to_h += 1000;
		amt_enchant_to_d += 1000;
		amt_enchant_weapon += 1000;
	}

	/* Handle "enchant armor" */
	if (borg_spell_legal_fail(REALM_SORCERY, 3, 5, 40))
	{
		amt_enchant_to_a += 1000;
		amt_enchant_armor += 1000;
	}

	/* Handle Diggers */
	if (borg_spell_legal_fail(REALM_SORCERY, 1, 8, 40) ||
		borg_spell_legal_fail(REALM_NATURE, 1, 0, 40) ||
		borg_spell_legal_fail(REALM_CHAOS, 0, 6, 40) ||
		borg_racial_check(RACE_HALF_GIANT, TRUE))
	{
		amt_digger += 1;
	}

	/* Handle recall */
	if (borg_spell_legal_fail(REALM_ARCANE, 3, 6, 40) ||
		borg_spell_legal_fail(REALM_SORCERY, 2, 7, 40) ||
		borg_spell_legal_fail(REALM_TRUMP, 1, 6, 40) ||
		((borg_skill[BI_CDEPTH] == 100) &&
		 (borg_spell_legal(REALM_LIFE, 3, 6) ||
		  borg_spell_legal(REALM_SORCERY, 2, 7) ||
		  borg_spell_legal(REALM_TRUMP, 1, 6))))
	{
		borg_skill[BI_RECALL] += 1000;
	}

	/* Handle teleport_level */
	if (borg_spell_legal_fail(REALM_SORCERY, 2, 6, 40) ||
		borg_spell_legal_fail(REALM_ARCANE, 3, 1, 40) ||
		borg_spell_legal_fail(REALM_TRUMP, 1, 5, 40))
	{
		borg_skill[BI_ATELEPORTLVL] += 1000;
	}

	/* Handle teleport spell carefully */
	if ((borg_spell_okay_fail(REALM_ARCANE, 2, 3, 5) ||
		 borg_spell_okay_fail(REALM_LIFE, 4, 1, 5) ||
		 borg_spell_okay_fail(REALM_TRUMP, 0, 4, 5) ||
		 borg_spell_okay_fail(REALM_CHAOS, 0, 7, 5) ||
		 borg_mindcr_okay_fail(MIND_MAJOR_DISP, 7, 5)) &&
		borg_skill[BI_RBLIND] && borg_skill[BI_RCONF])
	{
		borg_skill[BI_ATELEPORT] += 1000;
	}

	/* Handle GOI spell carefully */
	if ((borg_spell_legal_fail(REALM_LIFE, 3, 7, 4) ||
		 borg_spell_legal_fail(REALM_SORCERY, 3, 7, 4)) &&
		borg_skill[BI_RBLIND] &&
		borg_skill[BI_RCONF] &&
		borg_skill[BI_ESP] && (borg_skill[BI_MAXHP] >= 650))
	{
		borg_skill[BI_AXGOI] += 1000;
	}

	/* Handle GOI spell just to see if legal */
	if (borg_spell_legal(REALM_SORCERY, 3, 7) ||
		borg_spell_legal(REALM_LIFE, 3, 7))
	{
		borg_skill[BI_AGOI] += 1000;
	}

	/* Handle Holy Word prayer just to see if legal */
	if (borg_spell_legal(REALM_LIFE, 2, 6))
	{
		borg_skill[BI_AHWORD] += 1000;
	}

	/* speed spells */
	if (borg_spell_legal(REALM_SORCERY, 1, 5) ||
		borg_spell_legal(REALM_DEATH, 2, 3) ||
		borg_mindcr_legal(MIND_ADRENALINE, 35))
	{
		borg_skill[BI_ASPEED] += 1000;
	}

	/* Handle "heal" */
	if (borg_spell_legal(REALM_LIFE, 1, 6) ||
		borg_spell_legal(REALM_NATURE, 1, 7))
	{
		borg_skill[BI_AHEAL] += 1000;
	}

	/* Handle "fix exp" */
	if (borg_spell_legal(REALM_LIFE, 3, 3) ||
		borg_spell_legal(REALM_DEATH, 1, 7) ||
		borg_racial_check(RACE_SKELETON, FALSE) ||
		borg_racial_check(RACE_ZOMBIE, FALSE))
	{
		amt_fix_exp += 1000;
	}

	/* Handle "recharge" */
	if (borg_spell_legal(REALM_ARCANE, 3, 0) ||
		borg_spell_legal(REALM_SORCERY, 0, 7))
	{
		borg_skill[BI_ARECHARGE] += 1000;
	}

	/*** Process the Needs ***/
	
	/* No need to *buy* stat increase potions */
	if (my_stat_cur[A_STR] >= (18 + 100) + 10 *
		(rp_ptr->r_adj[A_STR] + cp_ptr->c_adj[A_STR]))
		amt_add_stat[A_STR] += 1000;

	if (my_stat_cur[A_INT] >= (18 + 100) + 10 *
		(rp_ptr->r_adj[A_INT] + cp_ptr->c_adj[A_INT]))
		amt_add_stat[A_INT] += 1000;

	if (my_stat_cur[A_WIS] >= (18 + 100) + 10 *
		(rp_ptr->r_adj[A_WIS] + cp_ptr->c_adj[A_WIS]))
		amt_add_stat[A_WIS] += 1000;

	if (my_stat_cur[A_DEX] >= (18 + 100) + 10 *
		(rp_ptr->r_adj[A_DEX] + cp_ptr->c_adj[A_DEX]))
		amt_add_stat[A_DEX] += 1000;

	if (my_stat_cur[A_CON] >= (18 + 100) + 10 *
		(rp_ptr->r_adj[A_CON] + cp_ptr->c_adj[A_CON]))
		amt_add_stat[A_CON] += 1000;

	if (my_stat_cur[A_CHR] >= (18 + 100) + 10 *
		(rp_ptr->r_adj[A_CHR] + cp_ptr->c_adj[A_CHR]))
		amt_add_stat[A_CHR] += 1000;

	/* No need to *buy* stat repair potions */
	if (!borg_skill[BI_ISFIXSTR]) amt_fix_stat[A_STR] += 1000;
	if (!borg_skill[BI_ISFIXINT]) amt_fix_stat[A_INT] += 1000;
	if (!borg_skill[BI_ISFIXWIS]) amt_fix_stat[A_WIS] += 1000;
	if (!borg_skill[BI_ISFIXDEX]) amt_fix_stat[A_DEX] += 1000;
	if (!borg_skill[BI_ISFIXCON]) amt_fix_stat[A_CON] += 1000;
	if (!borg_skill[BI_ISFIXCHR]) amt_fix_stat[A_CHR] += 1000;


	/* No need for experience repair */
	if (!borg_skill[BI_ISFIXEXP]) amt_fix_exp += 1000;

	/* Correct the high and low calorie foods for the correct
	 * races.
	 */
	if (borg_race >= RACE_SPRITE && borg_race <= RACE_IMP)
	{
		borg_skill[BI_FOOD] += amt_food_hical;
		if (amt_food_hical <= 3) borg_skill[BI_FOOD] += amt_food_lowcal;
	}

	/* If weak, do not count food spells */
	if (borg_skill[BI_ISWEAK] && (borg_skill[BI_FOOD] >= 1000))
		borg_skill[BI_FOOD] -= 1000;

	/* Correct BI_ENCUMBERD from total weight to the degree
	 * of being overweight.
	 */
	/* Extract the "weight limit" (in tenth pounds) */
	carry_capacity = (adj_str_wgt[my_stat_ind[A_STR]] * 100) / 2;

	/* over or under the limit */
	if (borg_skill[BI_ENCUMBERD] > carry_capacity)
	{
		borg_skill[BI_ENCUMBERD] = (borg_skill[BI_ENCUMBERD] - carry_capacity);
	}
	else
		borg_skill[BI_ENCUMBERD] = 0;
}


/*
 * Analyze the equipment and inventory
 */
void borg_notice(void)
{
	/* Clear out 'has' array */
	memset(borg_has, 0, size_obj * sizeof(int));

	/* Many of our variables are tied to borg_skill[], which is erased at the
	 * the start of borg_notice().  So we must update the frame the cheat in
	 * all the non inventory skills.
	 */
	borg_update_frame();

	/* Notice the equipment */
	borg_notice_aux1();

	/* Notice the inventory */
	borg_notice_aux2();
}

#if 0
/*
 * Helper function -- notice the home equipment
 */
static void borg_notice_home_aux1(borg_item *in_item, bool no_items)
{

	int i, ii;

	/*** Reset counters ***/

	/* Reset basic */
	num_food = 0;
	num_mold = 0;
	num_ident = 0;
	num_star_ident = 0;
	num_recall = 0;
	num_phase = 0;
	num_escape = 0;
	num_teleport = 0;
	num_teleport_level = 0;

	num_artifact = 0;

	num_invisible = 0;
	num_pfe = 0;
	num_glyph = 0;
	num_genocide = 0;
	num_mass_genocide = 0;
	num_berserk = 0;
	num_pot_rheat = 0;
	num_pot_rcold = 0;
	num_speed = 0;
	num_goi_pot = 0;
	num_resist_pot = 0;

	num_slow_digest = 0;
	num_regenerate = 0;
	num_telepathy = 0;
	num_see_inv = 0;
	num_ffall = 0;
	num_free_act = 0;
	num_hold_life = 0;
	num_immune_acid = 0;
	num_immune_elec = 0;
	num_immune_fire = 0;
	num_immune_cold = 0;
	num_resist_acid = 0;
	num_resist_elec = 0;
	num_resist_fire = 0;
	num_resist_cold = 0;
	num_resist_pois = 0;
	num_resist_conf = 0;
	num_resist_sound = 0;
	num_resist_lite = 0;
	num_resist_dark = 0;
	num_resist_chaos = 0;
	num_resist_disen = 0;
	num_resist_shard = 0;
	num_resist_nexus = 0;
	num_resist_blind = 0;
	num_resist_neth = 0;
	num_sustain_str = 0;
	num_sustain_int = 0;
	num_sustain_wis = 0;
	num_sustain_dex = 0;
	num_sustain_con = 0;
	num_sustain_all = 0;

	home_stat_add[A_STR] = 0;
	home_stat_add[A_INT] = 0;
	home_stat_add[A_WIS] = 0;
	home_stat_add[A_DEX] = 0;
	home_stat_add[A_CON] = 0;
	home_stat_add[A_CHR] = 0;

	num_weapons = 0;

	num_bow = 0;
	num_rings = 0;
	num_neck = 0;
	num_armor = 0;
	num_cloaks = 0;
	num_shields = 0;
	num_hats = 0;
	num_gloves = 0;
	num_boots = 0;
	num_lite = 0;
	num_speed = 0;
	num_edged_weapon = 0;
	num_bad_gloves = 0;

	/* Reset healing */
	num_cure_critical = 0;
	num_cure_serious = 0;
	num_fix_exp = 0;
	num_mana = 0;
	num_heal = 0;
	num_ez_heal = 0;
	if (!in_item && !no_items) num_ez_heal_true = 0;
	if (!in_item && !no_items) num_heal_true = 0;


	/* Reset missiles */
	num_missile = 0;

	/* Reset books */
	for (i = 0; i < MAX_REALM; i++)
	{
		for (ii = 0; ii < 4; ii++)
		{
			num_book[i][ii] = 0;
		}
	}

	/* Reset various */
	num_fix_stat[A_STR] = 0;
	num_fix_stat[A_INT] = 0;
	num_fix_stat[A_WIS] = 0;
	num_fix_stat[A_DEX] = 0;
	num_fix_stat[A_CON] = 0;
	num_fix_stat[A_CHR] = 0;
	num_fix_stat[6] = 0;

	/* Reset enchantment */
	num_enchant_to_a = 0;
	num_enchant_to_d = 0;
	num_enchant_to_h = 0;

	home_slot_free = 0;
	home_damage = 0;

	num_duplicate_items = 0;
}
#endif /* 0 */

#if 0
/*
 * This checks for duplicate items in the home
 */
static void borg_notice_home_dupe(borg_item *item, bool check_sval, int i)
{
/* eventually check for power overlap... armor of resistence is same as weak elvenkind.*/
/*  two armors of elvenkind that resist poison is a dupe.  AJG*/

	int dupe_count, x;
	borg_item *item2;

	/* if this is a stack of items then all after the first are a */
	/* duplicate */
	dupe_count = item->iqty - 1;

	/* Look for other items before this one that are the same */
	for (x = 0; x < i; x++)
	{
		if (x < STORE_INVEN_MAX)
			item2 = &borg_shops[BORG_HOME].ware[x];
		else
			/* Check what the borg has on as well. */
			item2 = &borg_items[((x - STORE_INVEN_MAX) + INVEN_WIELD)];

		/* if everything matches it is a duplicate item */
		/* Note that we only check sval on certain items.  This */
		/* is because, for example, two pairs of dragon armor */
		/* are not the same unless thier subtype (color) matches */
		/* but a defender is a defender even if one is a dagger and */
		/* one is a mace */
		if ((item->tval == item2->tval) &&
			(check_sval ? (item->sval == item2->sval) : TRUE) &&
			(item->xtra_name == item2->xtra_name))
		{
			dupe_count++;
		}
	}

	/* there can be one dupe of rings because there are two ring slots. */
	if (item->tval == TV_RING && dupe_count)
		dupe_count--;

	/* Add this items count to the total duplicate count */
	num_duplicate_items += dupe_count;
}
#endif /* 0 */

#if 0
/*
 * Helper function -- notice the home inventory
 */
static void borg_notice_home_aux2(borg_item *in_item, bool no_items)
{
	int i;

	borg_item *item;

	borg_shop *shop = &borg_shops[BORG_HOME];

	u32b f1, f2, f3;

	/*** Process the inventory ***/

	/* Scan the home */
	for (i = 0; i < (STORE_INVEN_MAX + (INVEN_TOTAL - INVEN_WIELD)); i++)
	{
		if (no_items) break;

		if (!in_item)
			if (i < STORE_INVEN_MAX)
				item = &shop->ware[i];
			else
				item = &borg_items[((i - STORE_INVEN_MAX) + INVEN_WIELD)];
		else
			item = in_item;

		/* Skip empty items */
		if (!item->iqty)
		{
			home_slot_free++;
			continue;
		}

		/* Hack -- skip un-aware items */
		if (!item->kind)
		{
			home_slot_free++;
			continue;
		}

		if (item->flags3 & TR3_SLOW_DIGEST) num_slow_digest += item->iqty;
		if (item->flags3 & TR3_REGEN) num_regenerate += item->iqty;
		if (item->flags3 & TR3_TELEPATHY) num_telepathy += item->iqty;
		if (item->flags3 & TR3_SEE_INVIS) num_see_inv += item->iqty;
		if (item->flags3 & TR3_FEATHER) num_ffall += item->iqty;
		if (item->flags2 & TR2_FREE_ACT) num_free_act += item->iqty;
		if (item->flags2 & TR2_HOLD_LIFE) num_hold_life += item->iqty;
		if (item->flags2 & TR2_IM_FIRE)
		{
			num_immune_fire += item->iqty;
			num_resist_fire += item->iqty;
		}
		if (item->flags2 & TR2_IM_ACID)
		{
			num_immune_acid += item->iqty;
			num_resist_acid += item->iqty;
		}
		if (item->flags2 & TR2_IM_COLD)
		{
			num_immune_cold += item->iqty;
			num_resist_cold += item->iqty;
		}
		if (item->flags2 & TR2_IM_ELEC)
		{
			num_immune_elec += item->iqty;
			num_resist_elec += item->iqty;
		}
		if (item->flags2 & TR2_RES_ACID) num_resist_acid += item->iqty;
		if (item->flags2 & TR2_RES_ELEC) num_resist_elec += item->iqty;
		if (item->flags2 & TR2_RES_FIRE) num_resist_fire += item->iqty;
		if (item->flags2 & TR2_RES_COLD) num_resist_cold += item->iqty;
		if (item->flags2 & TR2_RES_POIS) num_resist_pois += item->iqty;
		if (item->flags2 & TR2_RES_SOUND) num_resist_sound += item->iqty;
		if (item->flags2 & TR2_RES_LITE) num_resist_lite += item->iqty;
		if (item->flags2 & TR2_RES_DARK) num_resist_dark += item->iqty;
		if (item->flags2 & TR2_RES_CHAOS) num_resist_chaos += item->iqty;
		if (item->flags2 & TR2_RES_CONF) num_resist_conf += item->iqty;
		if (item->flags2 & TR2_RES_DISEN) num_resist_disen += item->iqty;
		if (item->flags2 & TR2_RES_SHARDS) num_resist_shard += item->iqty;
		if (item->flags2 & TR2_RES_NEXUS) num_resist_nexus += item->iqty;
		if (item->flags2 & TR2_RES_BLIND) num_resist_blind += item->iqty;
		if (item->flags2 & TR2_RES_NETHER) num_resist_neth += item->iqty;

		/* Count Sustains */
		if (item->flags2 & TR2_SUST_STR) num_sustain_str += item->iqty;
		if (item->flags2 & TR2_SUST_INT) num_sustain_str += item->iqty;
		if (item->flags2 & TR2_SUST_WIS) num_sustain_str += item->iqty;
		if (item->flags2 & TR2_SUST_DEX) num_sustain_str += item->iqty;
		if (item->flags2 & TR2_SUST_CON) num_sustain_str += item->iqty;
		if (item->flags2 & TR2_SUST_STR &&
			item->flags2 & TR2_SUST_INT &&
			item->flags2 & TR2_SUST_WIS &&
			item->flags2 & TR2_SUST_DEX &&
			item->flags2 & TR2_SUST_CON) num_sustain_all += item->iqty;

		/* count up bonus to stats */
		/* HACK only collect stat rings above +3 */
		if (item->flags1 & TR1_STR)
		{
			if (item->tval != TV_RING || item->pval > 3)
				home_stat_add[A_STR] += item->pval * item->iqty;
		}
		if (item->flags1 & TR1_INT)
		{
			if (item->tval != TV_RING || item->pval > 3)
				home_stat_add[A_INT] += item->pval * item->iqty;
		}
		if (item->flags1 & TR1_WIS)
		{
			if (item->tval != TV_RING || item->pval > 3)
				home_stat_add[A_WIS] += item->pval * item->iqty;
		}
		if (item->flags1 & TR1_DEX)
		{
			if (item->tval != TV_RING || item->pval > 3)
				home_stat_add[A_DEX] += item->pval * item->iqty;
		}
		if (item->flags1 & TR1_CON)
		{
			if (item->tval != TV_RING || item->pval > 3)
				home_stat_add[A_CON] += item->pval * item->iqty;
		}
		if (item->flags1 & TR1_CHR)
		{
			if (item->tval != TV_RING || item->pval > 3)
				home_stat_add[A_CHR] += item->pval * item->iqty;
		}

		/* count up bonus to speed */
		if (item->flags1 & TR1_SPEED) num_speed += item->pval * item->iqty;

#if 0
		/* count artifacts */
		if (item->name1)
		{
			num_artifact += item->iqty;
		}
#endif /* 0 */

		/* Analyze the item */
		switch (item->tval)
		{
			case TV_SOFT_ARMOR:
			case TV_HARD_ARMOR:
			{
				num_armor += item->iqty;

				/* see if this item is duplicated */
				borg_notice_home_dupe(item, FALSE, i);
				break;
			}

			case TV_DRAG_ARMOR:
			{
				num_armor += item->iqty;

				/* see if this item is duplicated */
				borg_notice_home_dupe(item, TRUE, i);
				break;
			}

			case TV_CLOAK:
			{
				num_cloaks += item->iqty;

				/* see if this item is duplicated */
				borg_notice_home_dupe(item, FALSE, i);

				break;
			}

			case TV_SHIELD:
			{
				num_shields += item->iqty;

				/* see if this item is duplicated */
				borg_notice_home_dupe(item, FALSE, i);
				break;
			}

			case TV_HELM:
			case TV_CROWN:
			{
				num_hats += item->iqty;

				/* see if this item is duplicated */
				borg_notice_home_dupe(item, FALSE, i);

				break;
			}

			case TV_GLOVES:
			{
				num_gloves += item->iqty;

				/* most gloves hurt magic for spell-casters */
				if (borg_skill[BI_INTMANA] && borg_skill[BI_MAXSP] > 3)
				{
					/* Penalize non-usable gloves */
					if (item->iqty &&
						(!(item->flags2 & TR2_FREE_ACT)) &&
						(!((item->flags1 & TR1_DEX) && (item->pval > 0))))
					{
						num_bad_gloves += item->iqty;
					}
				}

				/* gloves of slaying give a damage bonus */
				home_damage += item->to_d * 3;

				/* see if this item is duplicated */
				borg_notice_home_dupe(item, FALSE, i);

				break;
			}

			case TV_LITE:
			{
				if (item->xtra_name)
				{
					num_lite += item->iqty;
				}
				break;
			}

			case TV_BOOTS:
			{
				num_boots += item->iqty;

				/* see if this item is duplicated */
				borg_notice_home_dupe(item, FALSE, i);
				break;
			}

			case TV_SWORD:
			case TV_POLEARM:
			case TV_HAFTED:
			case TV_DIGGING:
			{
				s16b num_blow;

				num_weapons += item->iqty;
				/* apw most edged weapons hurt magic for priests */
				if (borg_class == CLASS_PRIEST)
				{
					/* Penalize non-blessed edged weapons */
					if (((item->tval == TV_SWORD) || (item->tval == TV_POLEARM))
						&& (!(item->flags3 & TR3_BLESSED)))
					{
						num_edged_weapon += item->iqty;
					}
				}


				/* NOTE:  This damage does not take slays into account. */
				/* it is just a rough estimate to make sure the glave of pain */
				/* is kept if it is found */
				/* It is hard to hold a heavy weapon */
				num_blow = 1;
				if (adj_str_hold[my_stat_ind[A_STR]] >= item->weight / 10)
				{
					int str_index, dex_index;
					int num = 0, wgt = 0, mul = 0, div = 0;

					/* Analyze the class */
					switch (borg_class)
					{
						case CLASS_WARRIOR:
						{
							/* Warrior */
							num = 5;
							wgt = 30;
							mul = 5;
							break;
						}

						case CLASS_MAGE:
						case CLASS_HIGH_MAGE:
						{
							/* Mage */
							num = 2;
							wgt = 40;
							mul = 2;
							break;
						}

						case CLASS_PRIEST:
						case CLASS_MINDCRAFTER:
						{
							/* Priest, Mindcrafter */
							num = 4;
							wgt = 35;
							mul = 3;
							break;
						}

						case CLASS_ROGUE:
						{
							/* Rogue */
							num = 4;
							wgt = 30;
							mul = 3;
							break;
						}

						case CLASS_RANGER:
						{
							/* Ranger */
							num = 4;
							wgt = 35;
							mul = 4;
							break;
						}

						case CLASS_PALADIN:
						{
							/* Paladin */
							num = 4;
							wgt = 30;
							mul = 4;
							break;
						}

						case CLASS_WARRIOR_MAGE:
						{
							/* Warrior-Mage */
							num = 4;
							wgt = 35;
							mul = 3;
							break;
						}

						case CLASS_CHAOS_WARRIOR:
						{
							/* Chaos Warrior */
							num = 4;
							wgt = 30;
							mul = 4;
							break;
						}

						case CLASS_MONK:
						{
							/* Monk */
							num = ((p_ptr->lev < 40) ? 2 : 3);
							wgt = 40;
							mul = 4;
							break;
						}
					}

					/* Enforce a minimum "weight" */
					div = ((item->weight < wgt) ? wgt : item->weight);

					/* Access the strength vs weight */
					str_index = (adj_str_blow[my_stat_ind[A_STR]] * mul / div);

					/* Maximal value */
					if (str_index > 11) str_index = 11;

					/* Index by dexterity */
					dex_index = (adj_dex_blow[my_stat_ind[A_DEX]]);

					/* Maximal value */
					if (dex_index > 11) dex_index = 11;

					/* Use the blows table */
					num_blow = blows_table[str_index][dex_index];

					/* Maximal value */
					if (num_blow > num) num_blow = num;

				}

				/* Require at least one blow */
				if (num_blow < 1) num_blow = 1;

				if (item->flags1 & TR1_BLOWS)
					num_blow += item->pval;
				num_blow *= item->iqty;
				if (item->to_d > 8 || borg_skill[BI_CLEVEL] < 15)
				{
					home_damage += num_blow * (item->dd * (item->ds) +
											   (borg_skill[BI_TODAM] +
												item->to_d));
				}
				else
				{
					home_damage += num_blow * (item->dd * (item->ds) +
											   (borg_skill[BI_TODAM] + 8));
				}

				/* see if this item is a duplicate */
				borg_notice_home_dupe(item, FALSE, i);
				break;
			}

			case TV_BOW:
			{
				num_bow += item->iqty;

				/* see if this item is a duplicate */
				borg_notice_home_dupe(item, FALSE, i);
				break;
			}

			case TV_RING:
			{
				num_rings += item->iqty;

				/* see if this item is a duplicate */
				borg_notice_home_dupe(item, TRUE, i);

				break;
			}

			case TV_AMULET:
			{
				num_neck += item->iqty;

				/* see if this item is a duplicate */
				borg_notice_home_dupe(item, TRUE, i);
				break;
			}

			case TV_LIFE_BOOK:
			{
				/* Count good books */
				if (borg_skill[BI_REALM1] == REALM_LIFE ||
					borg_skill[BI_REALM2] == REALM_LIFE)
					num_book[REALM_LIFE][item->sval] += item->iqty;
				break;
			}

			case TV_SORCERY_BOOK:
			{
				/* Count good books */
				if (borg_skill[BI_REALM1] == REALM_SORCERY ||
					borg_skill[BI_REALM2] == REALM_SORCERY)
					num_book[REALM_SORCERY][item->sval] += item->iqty;
				break;
			}

			case TV_NATURE_BOOK:
			{
				/* Count good books */
				if (borg_skill[BI_REALM1] == REALM_NATURE ||
					borg_skill[BI_REALM2] == REALM_NATURE)
					num_book[REALM_NATURE][item->sval] += item->iqty;
				break;
			}
			case TV_CHAOS_BOOK:
			{
				/* Count good books */
				if (borg_skill[BI_REALM1] == REALM_CHAOS ||
					borg_skill[BI_REALM2] == REALM_CHAOS)
					num_book[REALM_CHAOS][item->sval] += item->iqty;
				break;
			}
			case TV_DEATH_BOOK:
			{
				/* Count good books */
				if (borg_skill[BI_REALM1] == REALM_DEATH ||
					borg_skill[BI_REALM2] == REALM_DEATH)
					num_book[REALM_DEATH][item->sval] += item->iqty;
				break;
			}
			case TV_TRUMP_BOOK:
			{
				/* Count good books */
				if (borg_skill[BI_REALM1] == REALM_TRUMP ||
					borg_skill[BI_REALM2] == REALM_TRUMP)
					num_book[REALM_TRUMP][item->sval] += item->iqty;
				break;
			}
			case TV_ARCANE_BOOK:
			{
				/* Count good books */
				if (borg_skill[BI_REALM1] == REALM_ARCANE ||
					borg_skill[BI_REALM2] == REALM_ARCANE)
					num_book[REALM_ARCANE][item->sval] += item->iqty;
				break;
			}



			case TV_FOOD:
			{
				/* Food */

				/* Analyze */
				switch (item->sval)
				{
					case SV_FOOD_WAYBREAD:
					{
						if (borg_race >= RACE_SPRITE && borg_race <= RACE_IMP)
						{
							num_food += item->iqty;
						}
						break;
					}

					case SV_FOOD_RATION:
					{
						if (borg_race >= RACE_SPRITE && borg_race <= RACE_IMP)
						{
							num_food += item->iqty;
						}
						break;
					}

					case SV_FOOD_SLIME_MOLD:
					{
						num_mold += item->iqty;
						break;
					}

#if 0

					case SV_FOOD_JERKY:
					{
						num_food += item->iqty;
						break;
					}

					case SV_FOOD_BISCUIT:
					{
						num_food += item->iqty;
						break;
					}
#endif

					case SV_FOOD_RESTORE_STR:
					{
						num_fix_stat[A_STR] += item->iqty;
						break;
					}

					case SV_FOOD_RESTORE_CON:
					{
						num_fix_stat[A_CON] += item->iqty;
						break;
					}

					case SV_FOOD_RESTORING:
					{
						num_fix_stat[A_STR] += item->iqty;
						num_fix_stat[A_INT] += item->iqty;
						num_fix_stat[A_WIS] += item->iqty;
						num_fix_stat[A_DEX] += item->iqty;
						num_fix_stat[A_CON] += item->iqty;
						num_fix_stat[A_CHR] += item->iqty;
						num_fix_stat[6] += item->iqty;
						break;
					}
				}

				break;
			}


			case TV_POTION:
			{
				/* Potions */

				/* Analyze */
				switch (item->sval)
				{
					case SV_POTION_CURE_CRITICAL:
					{
						num_cure_critical += item->iqty;
						break;
					}

					case SV_POTION_CURE_SERIOUS:
					{
						num_cure_serious += item->iqty;
						break;
					}

					case SV_POTION_RESIST_HEAT:
					{
						num_pot_rheat += item->iqty;
						break;
					}

					case SV_POTION_RESIST_COLD:
					{
						num_pot_rcold += item->iqty;
						break;
					}

					case SV_POTION_RES_STR:
					{
						num_fix_stat[A_STR] += item->iqty;
						break;
					}

					case SV_POTION_RES_INT:
					{
						num_fix_stat[A_INT] += item->iqty;
						break;
					}

					case SV_POTION_RES_WIS:
					{
						num_fix_stat[A_WIS] += item->iqty;
						break;
					}

					case SV_POTION_RES_DEX:
					{
						num_fix_stat[A_DEX] += item->iqty;
						break;
					}

					case SV_POTION_RES_CON:
					{
						num_fix_stat[A_CON] += item->iqty;
						break;
					}

					case SV_POTION_RES_CHR:
					{
						num_fix_stat[A_CHR] += item->iqty;
						break;
					}

					case SV_POTION_RESTORE_EXP:
					{
						num_fix_exp += item->iqty;
						break;
					}

					case SV_POTION_RESTORE_MANA:
					{
						num_mana += item->iqty;
						break;
					}

					case SV_POTION_HEALING:
					{
						num_heal += item->iqty;
						if (!in_item && !no_items) num_heal_true += item->iqty;
						break;
					}

					case SV_POTION_STAR_HEALING:
					{
						num_ez_heal += item->iqty;
						if (!in_item &&
							!no_items) num_ez_heal_true += item->iqty;
						break;
					}

					case SV_POTION_LIFE:
					{
						num_ez_heal += item->iqty;
						if (!in_item &&
							!no_items) num_ez_heal_true += item->iqty;
						break;
					}

					case SV_POTION_BERSERK_STRENGTH:
					{
						num_berserk += item->iqty;
						break;
					}

					case SV_POTION_SPEED:
					{
						num_speed += item->iqty;
						break;
					}

					case SV_POTION_INVULNERABILITY:
					{
						num_goi_pot += item->iqty;
						break;
					}

					case SV_POTION_RESISTANCE:
					{
						num_resist_pot += item->iqty;
						break;
					}
				}

				break;
			}


			case TV_SCROLL:
			{
				/* Scrolls */

				/* Analyze the scroll */
				switch (item->sval)
				{
					case SV_SCROLL_IDENTIFY:
					{
						num_ident += item->iqty;
						break;
					}

					case SV_SCROLL_STAR_IDENTIFY:
					{
						num_star_ident += item->iqty;
						break;
					}

					case SV_SCROLL_PHASE_DOOR:
					{
						num_phase += item->iqty;
						break;
					}

					case SV_SCROLL_TELEPORT:
					{
						num_escape += item->iqty;
						break;
					}

					case SV_SCROLL_WORD_OF_RECALL:
					{
						num_recall += item->iqty;
						break;
					}

					case SV_SCROLL_ENCHANT_ARMOR:
					{
						num_enchant_to_a += item->iqty;
						break;
					}

					case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
					{
						num_enchant_to_h += item->iqty;
						break;
					}

					case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
					{
						num_enchant_to_d += item->iqty;
						break;
					}

					case SV_SCROLL_PROTECTION_FROM_EVIL:
					{
						num_pfe += item->iqty;
						break;
					}

					case SV_SCROLL_RUNE_OF_PROTECTION:
					{
						num_glyph += item->iqty;
						break;
					}

					case SV_SCROLL_TELEPORT_LEVEL:
					{
						num_teleport_level += item->iqty;
						break;
					}

					case SV_SCROLL_SATISFY_HUNGER:
					{
						num_food += item->iqty;
						break;
					}
				}

				break;
			}


			case TV_ROD:
			{
				/* Rods */

				/* Analyze */
				switch (item->sval)
				{
					case SV_ROD_IDENTIFY:
					{
						num_ident += item->iqty * 100;
						break;
					}

					case SV_ROD_RECALL:
					{
						num_recall += item->iqty * 100;
						break;
					}
				}

				break;
			}



			case TV_STAFF:
			{
				/* Staffs */

				/* only collect staves with more than 3 charges at high level */
				if (item->pval <= 3 && borg_skill[BI_CLEVEL] > 30)
					break;

				/* Analyze */
				switch (item->sval)
				{
					case SV_STAFF_IDENTIFY:
					{
						num_ident += item->iqty * item->pval;
						break;
					}

					case SV_STAFF_TELEPORTATION:
					{
						/* Don't use them deep in the dungeon because the
						 * charges will get drained and he wont have any
						 * scrolls left to read
						 */
						if (borg_skill[BI_MAXDEPTH] < 97)
						{
							num_teleport += item->iqty * item->pval;
						}
						break;
					}
				}

				break;
			}

				/* Missiles */
			case TV_SHOT:
			case TV_ARROW:
			case TV_BOLT:
			{
				/* Hack -- ignore invalid missiles */
				if (item->tval != my_ammo_tval) break;

				/* Hack -- ignore worthless missiles */
				if (item->value <= 0) break;

				/* Count them */
				num_missile += item->iqty;

				break;
			}
		}

		/* if only doing one item, break. */
		if (in_item) break;
	}


	/*** Process the Spells and Prayers ***/
	/* apw, again.  Artifact activation included here */

	/* Handle "satisfy hunger" -> infinite food */
	if (borg_spell_legal_fail(REALM_SORCERY, 2, 0, 10) ||
		borg_spell_legal_fail(REALM_LIFE, 0, 7, 10) ||
		borg_spell_legal_fail(REALM_ARCANE, 2, 7, 10) ||
		borg_spell_legal_fail(REALM_NATURE, 0, 3, 10))
	{
		num_food += 1000;
	}

	/* Handle "identify" -> infinite identifies */
	if (borg_spell_legal(REALM_SORCERY, 1, 1) ||
		borg_spell_legal(REALM_ARCANE, 3, 2))
	{
		num_ident += 1000;
	}
	/* Handle "enchant weapon" */
	if (borg_spell_legal_fail(REALM_SORCERY, 3, 4, 40))
	{
		num_enchant_to_h += 1000;
		num_enchant_to_d += 1000;
	}

	/* apw Handle "protection from evil" */
	if (borg_spell_legal(REALM_LIFE, 1, 5))
	{
		num_pfe += 1000;
	}

	/* apw Handle "rune of protection" glyph */
	if (borg_spell_legal(REALM_LIFE, 1, 7) ||
		borg_spell_legal(REALM_LIFE, 2, 7))
	{
		num_glyph += 1000;
	}

	/* handle restore */

	/* Handle recall */
	if (borg_spell_legal_fail(REALM_ARCANE, 3, 6, 40) ||
		borg_spell_legal_fail(REALM_SORCERY, 2, 7, 40) ||
		borg_spell_legal_fail(REALM_TRUMP, 1, 6, 40) ||
		(borg_skill[BI_CDEPTH] == 100 && (borg_spell_legal(REALM_LIFE, 3, 6) ||
										  borg_spell_legal(REALM_SORCERY, 2, 7)
										  || borg_spell_legal(REALM_TRUMP, 1,
															  6))))
		/*  Avavir not counted because it may not be charged */
	{
		num_recall += 1000;
	}

	/* Handle teleport_level */
	if (borg_spell_legal_fail(REALM_SORCERY, 2, 6, 40) ||
		borg_spell_legal_fail(REALM_TRUMP, 1, 5, 40))
	{
		num_teleport_level += 1000;
	}


	/*** Process the Needs ***/

	/* Hack -- No need for stat repair */
	if (borg_skill[BI_SSTR]) num_fix_stat[A_STR] += 1000;
	if (borg_skill[BI_SINT]) num_fix_stat[A_INT] += 1000;
	if (borg_skill[BI_SWIS]) num_fix_stat[A_WIS] += 1000;
	if (borg_skill[BI_SDEX]) num_fix_stat[A_DEX] += 1000;
	if (borg_skill[BI_SCON]) num_fix_stat[A_CON] += 1000;
	if (borg_skill[BI_SCHR]) num_fix_stat[A_CHR] += 1000;

	/* Extract the player flags */
	player_flags(&f1, &f2, &f3);

	/* Good flags */
	if (f3 & (TR3_SLOW_DIGEST)) num_slow_digest = TRUE;
	if (f3 & (TR3_FEATHER)) num_ffall = TRUE;
	if (f3 & (TR3_LITE)) num_lite = TRUE;
	if (f3 & (TR3_REGEN)) num_regenerate = TRUE;
	if (f3 & (TR3_TELEPATHY)) num_telepathy = TRUE;
	if (f3 & (TR3_SEE_INVIS)) num_see_inv = TRUE;
	if (f2 & (TR2_FREE_ACT)) num_free_act = TRUE;
	if (f2 & (TR2_HOLD_LIFE)) num_hold_life = TRUE;

	/* Weird flags */

	/* Bad flags */

	/* Immunity flags */
	if (f2 & (TR2_IM_FIRE)) num_immune_fire = TRUE;
	if (f2 & (TR2_IM_ACID)) num_immune_acid = TRUE;
	if (f2 & (TR2_IM_COLD)) num_immune_cold = TRUE;
	if (f2 & (TR2_IM_ELEC)) num_immune_elec = TRUE;

	/* Resistance flags */
	if (f2 & (TR2_RES_ACID)) num_resist_acid = TRUE;
	if (f2 & (TR2_RES_ELEC)) num_resist_elec = TRUE;
	if (f2 & (TR2_RES_FIRE)) num_resist_fire = TRUE;
	if (f2 & (TR2_RES_COLD)) num_resist_cold = TRUE;
	if (f2 & (TR2_RES_POIS)) num_resist_pois = TRUE;
	if (f2 & (TR2_RES_LITE)) num_resist_lite = TRUE;
	if (f2 & (TR2_RES_DARK)) num_resist_dark = TRUE;
	if (f2 & (TR2_RES_BLIND)) num_resist_blind = TRUE;
	if (f2 & (TR2_RES_CONF)) num_resist_conf = TRUE;
	if (f2 & (TR2_RES_SOUND)) num_resist_sound = TRUE;
	if (f2 & (TR2_RES_SHARDS)) num_resist_shard = TRUE;
	if (f2 & (TR2_RES_NEXUS)) num_resist_nexus = TRUE;
	if (f2 & (TR2_RES_NETHER)) num_resist_neth = TRUE;
	if (f2 & (TR2_RES_CHAOS)) num_resist_chaos = TRUE;
	if (f2 & (TR2_RES_DISEN)) num_resist_disen = TRUE;

	/* Sustain flags */
	if (f2 & (TR2_SUST_STR)) num_sustain_str = TRUE;
	if (f2 & (TR2_SUST_INT)) num_sustain_int = TRUE;
	if (f2 & (TR2_SUST_WIS)) num_sustain_wis = TRUE;
	if (f2 & (TR2_SUST_DEX)) num_sustain_dex = TRUE;
	if (f2 & (TR2_SUST_CON)) num_sustain_con = TRUE;

}
#endif /* 0 */

/*
 * Extract the bonuses for items in the home.
 *
 * l_ptr is passed in if you want to pretend that l_ptr is
 *          the only item in the home.
 * no_items is passed in as TRUE if you want to pretend that the
 *          home is empty.
 */
void borg_notice_home(list_item *l_ptr, bool no_items)
{
	/* Hack - ignore parameters */
	(void)l_ptr;
	(void)no_items;

#if 0
	/* Notice the home equipment */
	borg_notice_home_aux1(l_ptr, no_items);

	/* Notice the home inventory */
	borg_notice_home_aux2(l_ptr, no_items);
#endif /* 0 */
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
		if (l_ptr->to_d > 8 || borg_skill[BI_CLEVEL] < 25)
			damage = ((my_ammo_sides) + (l_ptr->to_d)) * my_ammo_power;
		else
			damage = (my_ammo_sides + 8) * my_ammo_power;

		/* Reward "damage" */
		value += (borg_skill[BI_SHOTS] * damage * 9L);

		/* AJG - slings force you to carry heavy ammo.  Penalty for that unles you have lots of str  */
		if (k_info[l_ptr->k_idx].sval == SV_SLING && !l_ptr->xtra_name &&
			my_stat_ind[A_STR] < 14)
		{
			value -= 5000L;
		}

		/* Reward "bonus to hit" */
		if (l_ptr->to_h > 8 || borg_skill[BI_CLEVEL] < 25)
			value += ((borg_skill[BI_TOHIT] + l_ptr->to_h) * 7L);
		else
			value += ((borg_skill[BI_TOHIT] + 8) * 7L);

		/* Prefer bows */
		if (borg_class == CLASS_RANGER && my_ammo_tval == TV_ARROW) value += 30000L;

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
				borg_spell_legal(REALM_LIFE, 3, 7)) value +=
my_stat_ind[A_INT] * 35000L;

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
	if (borg_skill[BI_CRSTELE]) value -= 100000L;
	if (borg_skill[BI_CRSAGRV]) value -= 8000L;
	if (borg_skill[BI_CRSNOTELE]) value -= 100000L;
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
		if (l_ptr) continue;

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

	/* apw
	 * In here, we must subtract out the bonus granted from certain
	 * Artifacts.  They grant amt_x = 1000 then the power is increased
	 * by 1000 times whatever bonus.  In the case of Gondor.  This is
	 * 1000 heals times 4000 points per heal.
	 *
	 */

	/* Reward fuel */
	k = 0;
	for (; k < 5 && k < borg_skill[BI_AFUEL]; k++) value += 60000L;
	for (; k < 10 && k < borg_skill[BI_AFUEL]; k++) value += 6000L;

	/* Reward Food */
	/* if hungry, food is THE top priority */
	if ((borg_skill[BI_ISHUNGRY] || borg_skill[BI_ISWEAK]) &&
		borg_skill[BI_FOOD]) value += 100000;
	k = 0;
	for (; k < 5 && k < borg_skill[BI_FOOD]; k++) value += 50000L;
	for (; k < 10 && k < borg_skill[BI_FOOD]; k++) value += 200L;
	if (borg_skill[BI_REG] && !borg_skill[BI_SDIG])
	{
		k = 0;
		for (; k < 10 && k < borg_skill[BI_FOOD]; k++) value += 500L;
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
	{							/* usually takes more than one */
		k = 0;
		for (; k < 5 && k < borg_skill[BI_ACSW]; k++) value += 25000L;
	}
	if (borg_skill[BI_ISPOISONED] && borg_skill[BI_ACUREPOIS]) value += 15000;
	if (borg_skill[BI_ISPOISONED] && amt_slow_poison) value += 5000;

	/* Reward Resistance Potions for Warriors */
	if (borg_class == CLASS_WARRIOR)
	{
		k = 0;
		for (; k < 4 && k < borg_skill[BI_ARESHEAT]; k++) value += 500L;
		k = 0;
		for (; k < 4 && k < borg_skill[BI_ARESCOLD]; k++) value += 500L;
	}

	/* Reward ident */
	k = 0;
	for (; k < 10 && k < borg_skill[BI_AID]; k++) value += 6000L;
	for (; k < 15 && k < borg_skill[BI_AID]; k++) value += 600L;

	/*  Reward *id* apw carry lots of these */
	k = 0;
	for (; k < 8 && k < borg_has[177]; k++) value += 10000L;
	for (; k < 15 && k < borg_has[177]; k++) value += 2000L;

	/*  Reward PFE  carry lots of these */
	k = 0;
	for (; k < 10 && k < borg_skill[BI_APFE]; k++) value += 10000L;
	for (; k < 25 && k < borg_skill[BI_APFE]; k++) value += 2000L;

	/*  apw Reward Glyph- Rune of Protection-  carry lots of these */
	k = 0;
	for (; k < 10 && k < borg_skill[BI_AGLYPH]; k++) value += 10000L;
	for (; k < 25 && k < borg_skill[BI_AGLYPH]; k++) value += 2000L;

	/* Reward recall */
	k = 0;
	for (; k < 3 && k < borg_skill[BI_RECALL]; k++) value += 50000L;
	for (; k < 7 && k < borg_skill[BI_RECALL]; k++) value += 5000L;

	/* Reward phase */
	k = 1;
	/* first phase door is very important */
	if (amt_phase) value += 50000;
	for (; k < 15 && k < amt_phase; k++) value += 500L;

	/* Reward escape */
	k = 0;
	for (; k < 5 && k < borg_skill[BI_AESCAPE]; k++) value += 10000L;
	if (borg_skill[BI_CDEPTH] > 90)
	{
		for (; k < 15 && k < borg_skill[BI_AESCAPE]; k++) value += 10000L;
	}

	/* Reward teleport */
	k = 0;
	for (; k < 10 && k < borg_skill[BI_ATELEPORT]; k++) value += 10000L;

	/* Reward Teleport Level scrolls */
	if (borg_skill[BI_MAXDEPTH] >= 99)
	{
		k = 0;
		for (; k < 5 && k < borg_skill[BI_ATELEPORTLVL]; k++) value += 5000L;
	}


	/*** Healing ***/
	if (borg_class == CLASS_WARRIOR || borg_class == CLASS_ROGUE)
	{
		k = 0;
		for (; k < 15 && k < borg_skill[BI_AHEAL]; k++) value += 8000L;

		k = 0;					/* carry a couple for emergency. Store the rest. */
		for (; k < 2 && k < borg_has[419]; k++) value += 10000L;
		if (borg_has[419] == 0)
		{
			k = 0;				/* carry a couple for emergency. Store the rest. */
			for (; k < 2 && k < borg_has[420]; k++) value += 10000L;
		}

		/* These guys need to carry the rods more, town runs low on supply. */
		k = 0;
		for (; k < 4 && k < borg_has[374]; k++) value += 20000L;
	}
	else if (borg_class == CLASS_RANGER || borg_class == CLASS_PALADIN ||
			 borg_class == CLASS_MAGE)
	{
		k = 0;
		for (; k < 10 && k < borg_skill[BI_AHEAL]; k++) value += 4000L;

		k = 0;					/* carry a couple for emergency. Store the rest. */
		for (; k < 2 && k < borg_has[419]; k++) value += 9000L;
		if (borg_has[419] == 0)
		{
			k = 0;				/* carry a couple for emergency. Store the rest. */
			for (; k < 2 && k < borg_has[420]; k++) value += 9000L;
		}

		if (borg_class == CLASS_PALADIN)
		{
			/* Reward heal potions */
			k = 0;
			for (; k < 3 && k < borg_has[242]; k++) value += 5000L;
		}

	}
	else if (borg_class == CLASS_PRIEST)
	{
		/* Reward heal potions */
		k = 0;
		for (; k < 5 && k < borg_has[242]; k++) value += 2000L;

		k = 0;					/* carry a couple for emergency. Store the rest. */
		for (; k < 2 && k < borg_has[419]; k++) value += 9000L;
		if (borg_has[419] == 0)
		{
			k = 0;				/* carry a couple for emergency. Store the rest. */
			for (; k < 2 && k < borg_has[420]; k++) value += 9000L;
		}
	}

	/* Level 1 priests are given a Potion of Healing.  It is better
	 * for them to sell that potion and buy equipment or several
	 * Cure Crits with it.
	 */
	if (borg_skill[BI_CLEVEL] == 1)
	{
		k = 0;
		for (; k < 10 && k < borg_has[242]; k++) value -= 20000L;
	}

	/* Collecting Potions, prepping for Morgoth/Sauron fight */
	if (borg_skill[BI_MAXDEPTH] >= 99)
	{
		/* Sauron is alive -- carry them all */
		if (borg_race_death[860] == 0)
		{
			k = 0;
			for (; k < 99 && k < borg_has[242]; k++) value += 8000L;
			k = 0;
			for (; k < 99 && k < borg_skill[BI_AEZHEAL]; k++) value += 10000L;
			k = 0;
			for (; k < 99 && k < borg_skill[BI_ASPEED]; k++) value += 8000L;
		}
		/* Sauron is dead -- store them unless I have enough */
		if (borg_race_death[860] != 0)
		{
			/* Must scum for more pots */
			/* Must scum for more pots */
			if ((num_heal_true + borg_has[242] + num_ez_heal_true +
				 borg_skill[BI_AEZHEAL] < 45) ||
				(num_ez_heal_true + borg_skill[BI_AEZHEAL] < 20) ||
				(num_speed + borg_skill[BI_ASPEED] < 15))
			{
				/* leave pots at home so they dont shatter */
			}
			/* I have enough, carry all pots */
			else
			{
				k = 0;
				for (; k < 99 && k < borg_has[242]; k++) value += 8000L;
				k = 0;
				for (; k < 99 && k < borg_skill[BI_AEZHEAL];
					 k++) value += 10000L;
				k = 0;
				for (; k < 99 && k < borg_skill[BI_ASPEED]; k++) value += 8000L;
				/* Restore Mana */
				k = 0;
				for (; k < 99 && k < borg_has[266]; k++) value += 5000L;
				/* Shrooms of Restoring */
				k = 0;
				for (; k < 35 && k < amt_fix_stat[6]; k++) value += 5000L;
			}
		}
	}

	/* Restore Mana */
	if (borg_skill[BI_MAXSP] > 100)
	{
		k = 0;
		for (; k < 10 && k < borg_has[266]; k++) value += 4000L;
		k = 0;
		for (; k < 100 && k < borg_skill[BI_ASTFMAGI]; k++) value += 4000L;
	}

	/* Reward cure critical.  Heavy reward on first 5 */
	if (borg_skill[BI_CLEVEL] < 35 || !borg_skill[BI_RCONF])
	{
		k = 0;
		for (; k < 10 && k < borg_skill[BI_ACCW]; k++) value += 5000L;
		for (; k < 15 && k < borg_skill[BI_ACCW]; k++) value += 500L;
	}
	else
	{
		/* Reward cure critical.  Later on in game. */
		k = 0;
		for (; k < 10 && k < borg_skill[BI_ACCW]; k++) value += 5000L;
	}

	/* Reward cure serious -- only reward serious if low on crits */
	if (borg_skill[BI_ACCW] < 10 &&
		(borg_skill[BI_CLEVEL] < 35 || !borg_skill[BI_RCONF]))
	{
		k = 0;
		for (; k < 5 && k < borg_skill[BI_ACSW]; k++) value += 50L;
		for (; k < 10 && k < borg_skill[BI_ACSW]; k++) value += 5L;
	}

	/* Reward cure serious -- Low Level Characters */
	if (borg_skill[BI_CLEVEL] < 15)
	{
		k = 0;
		for (; k < 5 && k < borg_skill[BI_ACSW]; k++) value += 250L;
		for (; k < 10 && k < borg_skill[BI_ACSW]; k++) value += 55L;
	}

	/* Reward Cures */
	if (!borg_skill[BI_RCONF])
	{
		k = 0;
		for (; k < 10 && k < amt_cure_confusion; k++) value += 400L;
	}
	if (!borg_skill[BI_RBLIND])
	{
		k = 0;
		for (; k < 5 && k < amt_cure_blind; k++) value += 300L;
	}
	if (!borg_skill[BI_RPOIS])
	{
		k = 0;
		for (; k < 5 && k < borg_skill[BI_ACUREPOIS]; k++) value += 250L;
	}

	/*** Detection ***/

	/* Reward detect trap */
	k = 0;
	for (; k < 1 && k < borg_skill[BI_ADETTRAP]; k++) value += 4000L;

	/* Reward detect door */
	k = 0;
	for (; k < 1 && k < borg_skill[BI_ADETDOOR]; k++) value += 2000L;

	/* Reward detect evil */
	if (!borg_skill[BI_ESP])
	{
		k = 0;
		for (; k < 1 && k < borg_skill[BI_ADETEVIL]; k++) value += 1000L;
	}

	/* Reward magic mapping */
	k = 0;
	for (; k < 1 && k < borg_skill[BI_AMAGICMAP]; k++) value += 4000L;

	/* Genocide scrolls. Just scrolls, mainly used for Morgoth */
	if (borg_skill[BI_MAXDEPTH] >= 98)
	{
		k = 0;
		for (; k < 10 && k < borg_has[207]; k++) value += 10000L;
		for (; k < 25 && k < borg_has[207]; k++) value += 2000L;
	}

	/* Mass Genocide scrolls. Just scrolls, mainly used for Morgoth */
	if (borg_skill[BI_MAXDEPTH] >= 98)
	{
		k = 0;
		for (; k < 10 && k < borg_has[200]; k++) value += 10000L;
		for (; k < 25 && k < borg_has[200]; k++) value += 2000L;
	}

	/* Reward speed potions/staves */
	if (borg_skill[BI_MAXDEPTH] <= 98)
	{
		k = 0;
		for (; k < 20 && k < borg_skill[BI_ASPEED]; k++) value += 5000L;
	}

	/* Invuln Potions, mainly used for Morgoth */
	if (borg_skill[BI_MAXDEPTH] >= 98)
	{
		k = 0;
		for (; k < 15 && k < borg_has[238]; k++) value += 10000L;
		for (; k < 99 && k < borg_has[238]; k++) value += 2000L;
	}

	/* Reward Recharge ability */
	k = 0;
	for (; k < 5 && k < borg_skill[BI_ARECHARGE]; k++) value += 2000L;

	/*** Missiles ***/

	/* Reward missiles */
	if (borg_class == CLASS_RANGER)
	{
		k = 0;
		for (; k < 30 && k < borg_skill[BI_AMISSILES]; k++) value += 1000L;
		for (; k < 80 && k < borg_skill[BI_AMISSILES]; k++) value += 100L;
	}
	else
	{
		k = 0;
		for (; k < 20 && k < borg_skill[BI_AMISSILES]; k++) value += 1000L;
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
		if (amt_add_stat[A_INT]) value += 50000;

	if (amt_add_stat[A_WIS]) value += 20000;
	if (borg_skill[BI_WISMANA])
		if (amt_add_stat[A_WIS]) value += 50000;
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
		}						/* book */
	}							/* Realm */

	/* Hack -- Apply "encumbrance" from weight */
	if (borg_skill[BI_ENCUMBERD])
	{
		value -= (borg_skill[BI_ENCUMBERD] * 500L);
	}

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
 * Helper function -- calculate power of equipment in the home
 */
static s32b borg_power_home_aux1(void)
{
	s32b value = 0L;

	/* This would be better seperated by item type (so 1 bonus for resist cold armor */
	/*   1 bonus for resist cold shield... but that would take a bunch more code. */

	/* try to collect at least 2 of each resist/power (for swapping) */
	/* This can be used to get rid of extra artifacts... */

	/* spare lite sources.  Artifacts only */
	if (num_lite == 1)
		value += 150L;
	else if (num_lite == 2)
		value += 170L;
	else if (num_lite > 2)
		value += 170L + (num_lite - 2) * 5L;

	if (num_slow_digest == 1)
		value += 50L;
	else if (num_slow_digest == 2)
		value += 70L;
	else if (num_slow_digest > 2)
		value += 70L + (num_slow_digest - 2) * 5L;

	if (num_regenerate == 1)
		value += 75L;
	else if (num_regenerate == 2)
		value += 100L;
	else if (num_regenerate > 2)
		value += 100L + (num_regenerate - 2) * 10L;

	if (num_telepathy == 1)
		value += 1000L;
	else if (num_telepathy == 2)
		value += 1500L;
	else if (num_telepathy > 2)
		value += 1500L + (num_telepathy - 2) * 10L;

	if (num_see_inv == 1)
		value += 800L;
	else if (num_see_inv == 2)
		value += 1200L;
	else if (num_see_inv > 2)
		value += 1200L + (num_see_inv - 2) * 10L;

	if (num_ffall == 1)
		value += 10L;
	else if (num_ffall == 2)
		value += 15L;
	else if (num_ffall > 2)
		value += 15L + (num_ffall - 2) * 1L;


	if (num_free_act == 1)
		value += 1000L;
	else if (num_free_act == 2)
		value += 1500L;
	else if (num_free_act > 2)
		value += 1500L + (num_free_act - 2) * 10L;

	if (num_hold_life == 1)
		value += 1000L;
	else if (num_hold_life == 2)
		value += 1500L;
	else if (num_hold_life > 2)
		value += 1500L + (num_hold_life - 2) * 10L;

	if (num_resist_acid == 1)
		value += 1000L;
	else if (num_resist_acid == 2)
		value += 1500L;
	else if (num_resist_acid > 2)
		value += 1500L + (num_resist_acid - 2) * 1L;
	if (num_immune_acid == 1)
		value += 3000L;
	else if (num_immune_acid == 2)
		value += 5000L;
	else if (num_immune_acid > 2)
		value += 5000L + (num_immune_acid - 2) * 30L;

	if (num_resist_elec == 1)
		value += 1000L;
	else if (num_resist_elec == 2)
		value += 1500L;
	else if (num_resist_elec > 2)
		value += 1500L + (num_resist_elec - 2) * 1L;
	if (num_immune_elec == 1)
		value += 3000L;
	else if (num_immune_elec == 2)
		value += 5000L;
	else if (num_immune_elec > 2)
		value += 5000L + (num_immune_elec - 2) * 30L;

	if (num_resist_fire == 1)
		value += 1000L;
	else if (num_resist_fire == 2)
		value += 1500L;
	else if (num_resist_fire > 2)
		value += 1500L + (num_resist_fire - 2) * 1L;
	if (num_immune_fire == 1)
		value += 3000L;
	else if (num_immune_fire == 2)
		value += 5000L;
	else if (num_immune_fire > 2)
		value += 5000L + (num_immune_fire - 2) * 30L;

	if (num_resist_cold == 1)
		value += 1000L;
	else if (num_resist_cold == 2)
		value += 1500L;
	else if (num_resist_cold > 2)
		value += 1500L + (num_resist_cold - 2) * 1L;
	if (num_immune_cold == 1)
		value += 3000L;
	else if (num_immune_cold == 2)
		value += 5000L;
	else if (num_immune_cold > 2)
		value += 5000L + (num_immune_cold - 2) * 30L;

	if (num_resist_pois == 1)
		value += 5000L;
	else if (num_resist_pois == 2)
		value += 9000L;
	else if (num_resist_pois > 2)
		value += 9000L + (num_resist_pois - 2) * 40L;

	if (num_resist_conf == 1)
		value += 2000L;
	else if (num_resist_conf == 2)
		value += 3500L;
	else if (num_resist_conf > 2)
		value += 3500L + (num_resist_conf - 2) * 45L;

	if (num_resist_sound == 1)
		value += 500L;
	else if (num_resist_sound == 2)
		value += 700L;
	else if (num_resist_sound > 2)
		value += 700L + (num_resist_sound - 2) * 30L;

	if (num_resist_lite == 1)
		value += 100L;
	else if (num_resist_lite == 2)
		value += 150L;
	else if (num_resist_lite > 2)
		value += 150L + (num_resist_lite - 2) * 1L;

	if (num_resist_dark == 1)
		value += 100L;
	else if (num_resist_dark == 2)
		value += 150L;
	else if (num_resist_dark > 2)
		value += 150L + (num_resist_dark - 2) * 1L;

	if (num_resist_chaos == 1)
		value += 1000L;
	else if (num_resist_chaos == 2)
		value += 1500L;
	else if (num_resist_chaos > 2)
		value += 1500L + (num_resist_chaos - 2) * 10L;

	if (num_resist_disen == 1)
		value += 5000L;
	else if (num_resist_disen == 2)
		value += 7000L;
	else if (num_resist_disen > 2)
		value += 7000L + (num_resist_disen - 2) * 35L;

	if (num_resist_shard == 1)
		value += 100L;
	else if (num_resist_shard == 2)
		value += 150L;
	else if (num_resist_shard > 2)
		value += 150L + (num_resist_shard - 2) * 1L;

	if (num_resist_nexus == 1)
		value += 200L;
	else if (num_resist_nexus == 2)
		value += 300L;
	else if (num_resist_nexus > 2)
		value += 300L + (num_resist_nexus - 2) * 2L;

	if (num_resist_blind == 1)
		value += 500L;
	else if (num_resist_blind == 2)
		value += 700L;
	else if (num_resist_blind > 2)
		value += 700L + (num_resist_blind - 2) * 5L;

	if (num_resist_neth == 1)
		value += 5000L;
	else if (num_resist_neth == 2)
		value += 7000L;
	else if (num_resist_neth > 2)
		value += 7000L + (num_resist_neth - 2) * 45L;

	/* stat gain items as well...(good to carry ring of dex +6 in */
	/*                            house even if I don't need it right now) */
	if (home_stat_add[A_STR] < 9)
		value += home_stat_add[A_STR] * 300L;
	else if (home_stat_add[A_STR] < 15)
		value += 9 * 300L + (home_stat_add[A_STR] - 9) * 200L;
	else
		value += 9 * 300L + 6 * 200L + (home_stat_add[A_STR] - 15) * 1L;

	if (home_stat_add[A_DEX] < 9)
		value += home_stat_add[A_DEX] * 300L;
	else if (home_stat_add[A_DEX] < 15)
		value += 9 * 300L + (home_stat_add[A_DEX] - 9) * 200L;
	else
		value += 9 * 300L + 6 * 200L + (home_stat_add[A_DEX] - 15) * 1L;

	/* HACK extra con for thorin and other such things */
	if (home_stat_add[A_CON] < 15)
		value += home_stat_add[A_CON] * 300L;
	else if (home_stat_add[A_CON] < 21)
		value += 15 * 300L + (home_stat_add[A_CON] - 15) * 200L;
	else
		value += 15 * 300L + 6 * 200L + (home_stat_add[A_CON] - 21) * 1L;

	/* int and wis are only bonused for spell casters. */
	if (borg_skill[BI_INTMANA])
	{
		if (home_stat_add[A_INT] < 20)
			value += home_stat_add[A_INT] * 400L;
		else if (home_stat_add[A_INT] < 26)
			value += 20 * 400L + (home_stat_add[A_INT] - 20) * 300L;
		else
			value += 20 * 100L + 6 * 300L + (home_stat_add[A_INT] - 26) * 5L;
	}

	if (borg_skill[BI_WISMANA])
	{
		if (home_stat_add[A_WIS] < 20)
			value += home_stat_add[A_WIS] * 400L;
		else if (home_stat_add[A_WIS] < 26)
			value += 20 * 400L + (home_stat_add[A_WIS] - 20) * 300L;
		else
			value += 20 * 400L + 6 * 300L + (home_stat_add[A_WIS] - 26) * 3L;
	}

	/* Sustains */
	if (num_sustain_str == 1)
		value += 200L;
	else if (num_sustain_str == 2)
		value += 250L;
	else if (num_sustain_str > 2)
		value += 250L + (num_sustain_str - 2) * 1L;

	if (num_sustain_int == 1)
		value += 200L;
	else if (num_sustain_int == 2)
		value += 250L;
	else if (num_sustain_int > 2)
		value += 250L + (num_sustain_int - 2) * 1L;

	if (num_sustain_wis == 1)
		value += 200L;
	else if (num_sustain_wis == 2)
		value += 250L;
	else if (num_sustain_wis > 2)
		value += 250L + (num_sustain_wis - 2) * 1L;

	if (num_sustain_con == 1)
		value += 200L;
	else if (num_sustain_con == 2)
		value += 250L;
	else if (num_sustain_con > 2)
		value += 250L + (num_sustain_con - 2) * 1L;

	if (num_sustain_dex == 1)
		value += 200L;
	else if (num_sustain_dex == 2)
		value += 250L;
	else if (num_sustain_dex > 2)
		value += 250L + (num_sustain_dex - 2) * 1L;

	if (num_sustain_all == 1)
		value += 1000L;
	else if (num_sustain_all == 2)
		value += 1500L;
	else if (num_sustain_all > 2)
		value += 1500L + (num_sustain_all - 2) * 1L;

	/* do a minus for too many duplicates.  This way we do not store */
	/* useless items and spread out types of items. */
	if (num_weapons > 5)
		value -= (num_weapons - 5) * 2000L;
	else if (num_weapons > 1)
		value -= (num_weapons - 1) * 100L;
	if (num_bow > 2)
		value -= (num_bow - 2) * 1000L;
	if (num_rings > 6)
		value -= (num_rings - 6) * 4000L;
	else if (num_rings > 4)
		value -= (num_rings - 4) * 2000L;
	if (num_neck > 3)
		value -= (num_neck - 3) * 1500L;
	else if (num_neck > 2)
		value -= (num_neck - 2) * 700L;
	if (num_armor > 6)
		value -= (num_armor - 6) * 1000L;
	if (num_cloaks > 3)
		value -= (num_cloaks - 3) * 1000L;
	if (num_shields > 3)
		value -= (num_shields - 3) * 1000L;
	if (num_hats > 4)
		value -= (num_hats - 4) * 1000L;
	if (num_gloves > 3)
		value -= (num_gloves - 3) * 1000L;
	if (num_boots > 2)
		value -= (num_boots - 2) * 1000L;


	value += home_damage;

	/* if edged and priest, dump it   */
	value -= num_edged_weapon * 3000L;

	/* if gloves and mage or ranger and not FA/Dex, dump it. */
	value -= num_bad_gloves * 3000L;

	/* do not allow duplication of items. */
	value -= num_duplicate_items * 50000L;


	/* Return the value */
	return (value);
}


/*
 * Helper function -- calculate power of items in the home
 *
 * The weird calculations help spread out the purchase order
 */
static s32b borg_power_home_aux2(void)
{
	int k, book = 0, realm = 0;

	s32b value = 0L;


	/*** Basic abilities ***/

	/* Collect food */
	for (k = 0; k < 50 && k < num_food; k++) value += 8000L - k * 10L;

	/* Collect Molds as pets */
	for (k = 0; k < 10 && k < num_mold; k++) value += 10L - k;

	/* Collect ident */
	for (k = 0; k < 50 && k < num_ident; k++) value += 2000L - k * 10L;

	/* Collect *id*ent */
	for (k = 0; k < 50 && k < num_star_ident; k++) value += 5000L - k * 10L;

	/* apw Collect pfe */
	for (k = 0; k < 100 && k < num_pfe; k++) value += 5000L - k * 10L;

	/* apw Collect glyphs */
	for (k = 0; k < 100 && k < num_glyph; k++) value += 5000L - k * 10L;

	/* Reward Genocide scrolls. Just scrolls, mainly used for Morgoth */
	for (k = 0; k < 100 && k < num_genocide; k++) value += 5000L - k * 10L;

	/* Reward Mass Genocide scrolls. Just scrolls, mainly used for Morgoth */
	for (k = 0; k < 100 && k < num_mass_genocide; k++) value += 5000L - k * 10L;

	/* Reward Resistance Potions for Warriors */
	if (borg_class == CLASS_WARRIOR)
	{
		k = 0;
		for (; k < 99 && k < num_pot_rheat; k++) value += 1000L - k * 10L;
		for (; k < 99 && k < num_pot_rcold; k++) value += 1000L - k * 10L;
	}

	/* Collect recall */
	for (k = 0; k < 50 && k < num_recall; k++) value += 3000L - k * 10L;

	/* Collect escape */
	for (k = 0; k < 50 && k < num_escape; k++) value += 2000L - k * 10L;

	/* Collect teleport */
	for (k = 0; k < 50 && k < num_teleport; k++) value += 400L - k * 8L;

	/* Collect teleport level scrolls */
	for (k = 0; k < 99 && k < num_teleport_level; k++) value += 1000L - k * 8L;

	/* Collect Speed */
	for (k = 0; k < 99 && k < num_speed; k++) value += 5000L - k * 10L;

	/* Collect Invuln Potions */
	for (k = 0; k < 99 && k < num_goi_pot; k++) value += 5000L - k * 10L;

	/* collect heal/mana/ */
	for (k = 0; k < 99 && k < num_heal; k++) value += 3000L - k * 8L;
	for (k = 0; k < 99 && k < num_ez_heal; k++) value += 8000L - k * 8L;
	if (borg_skill[BI_MAXSP] > 1)
	{
		for (k = 0; k < 99 && k < num_mana; k++) value += 6000L - k * 8L;
	}

	/* Level 1 priests are given a Potion of Healing.  It is better
	 * for them to sell that potion and buy equipment or several
	 * Cure Crits with it.
	 */
	if (borg_skill[BI_CLEVEL] == 1)
	{
		k = 0;
		for (; k < 10 && k < num_heal; k++) value -= 5000L;
	}

	/*** Healing ***/

	/* Collect cure critical */
	for (k = 0; k < 99 && k < num_cure_critical; k++) value += 1500L - k * 10L;

	/* junk cure serious if we have some in the home */
	if (borg_skill[BI_CLEVEL] > 35)	/* dont bother keeping them if high level */
		for (k = 0; k < 99 && k < num_cure_serious;
			 k++) value -= 1500L - k * 10L;

	/*** Various ***/

	/* Fixing Stats */
	if (borg_skill[BI_CLEVEL] == 50) value -= 7500L * num_fix_exp;
	if (borg_skill[BI_CLEVEL] > 35)
		for (k = 0; k < 70 && k < num_fix_exp; k++) value += 5000L - k * 10L;
	else
		for (k = 0; k < 5 && k < num_fix_exp; k++) value += 5000L - k * 10L;

	/* Keep shrooms in the house */
	for (k = 0; k < 99 && k < num_fix_stat[6]; k++) value += 5000L;


	/*** Hack -- books ***/

	/* Scan Realms */
	for (realm = 0; realm < MAX_REALM; realm++)
	{
		/* only my realms */
		if (realm != borg_skill[BI_REALM1] &&
			realm != borg_skill[BI_REALM2]) continue;

		/* Scan Books */
		for (book = 0; book < 4; book++)
		{

			if (borg_skill[BI_CLEVEL] > 35)
				/* Collect up to 20 copies of each normal book */
				for (k = 0; k < 20 && k < num_book[realm][book]; k++)
				{
					/* Hack -- only stockpile useful books */
					if (num_book[realm][book]) value += 5000L - k * 10L;
				}
			else
				/* Collect up to 5 copies of each normal book */
				for (k = 0; k < 5 && k < num_book[realm][book]; k++)
				{
					/* Hack -- only stockpile useful books */
					if (num_book[realm][book]) value += 5000L - k * 10L;
				}
		}						/* book */
	}							/* realm */

	/* Reward artifacts in the home */
	value += num_artifact * 500L;

	/* Return the value */
	return (value);
}


/*
 * Calculate the "power" of the home
 */
s32b borg_power_home(void)
{
	s32b value = 0L;

	/* Process the home equipment */
	value += borg_power_home_aux1();

	/* Process the home inventory */
	value += borg_power_home_aux2();

	/* Return the value */
	return (value);
}


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
					n += borg_goi ? 400 * 20 : 400;	/* If GOI is on, take that into account */
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

	/* glyph of warding rune of protection provides some small
	 * protection with some ranged atacks; mainly summon attacks.
	 * We should reduce the danger commensurate to the probability of the
	 * monster breaking the glyph as defined by melee2.c
	 */
	if (borg_on_glyph)
	{
		glyph = 1;
	}
#if 0
	else if (track_glyph_num)
	{
		int glyph_check;

		/* Check all existing glyphs */
		for (glyph_check = 0; glyph_check < track_glyph_num; glyph_check++)
		{
			if ((track_glyph_y[glyph_check] == y) &&
				(track_glyph_x[glyph_check] == x))
			{
				/* Reduce the danger */
				glyph = 1;
			}
		}
	}
#endif /* 0 */

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

			/* track spaces already protected */
			if ( /* ag->feat == FEAT_GLYPH || */ mb_ptr->monster ||
				((mb_ptr->feat >= FEAT_CLOSED)
				 && (mb_ptr->feat <= FEAT_PERM_SOLID)))
			{					/* track the safe areas for calculating danger */
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

	/* If the most dangerous spell is alot bigger than the average,
	 * then return the dangerous one
	 *
	 * There is a problem when dealing with defence manuevers.  If the borg is
	 * considering casting a spell like Resistance and the monster also has a non
	 * resistable attack (like Disenchant) then the damage returned will be for
	 * that spell, since the danger of the others (like fire, cold) will be greatly
	 * reduced by the proposed defence spell.  The result will be the borg will not cast
	 * the resistance spell eventhough it may be a very good idea.
	 *
	 * Example: a monster has three breath attacks (Fire, Ice, Disenchant) and each hits for
	 * 500 pts of damage.  The borg currently resists all three, so the danger would be 500.
	 * if the borg were to use a Res Heat Potion that would decrease the danger to:
	 * Fire:  333
	 * Ice:   500
	 * Disen: 500
	 * Now the Average is 444.  Not really worth it, nominal change.  But if the resistance
	 * spell was both Fire and Ice, then it would be:
	 * Fire:  333
	 * Ice:   333
	 * Disen: 500
	 * With an average of 388. Probably worth it, but the borg would see that the Disen attack
	 * is a quite dangerous and would return the result of 500.
	 *
	 * To fix this, the flag 'average' is added to the borg_danger() to skip this check
	 * and return the average damage.  If the flag is FALSE then the formula below is SKIPPED
	 * and the value returned with be the average.  If the flag is TRUE, then the formula below
	 * will be used to determine the returned value.  Currently the elemental resistance spells
	 * and PFE have the flag set as FALSE.
	 *
	 */
	if (!average) return (av);
	if (n >= av * 15 / 10 || n > borg_skill[BI_CURHP] * 8 / 10) return (n);
	else
		/* Average Danger */
		return (av);
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

	/* if I can't be killed in one round (or severely wounded) use probablilities */
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
 * Determine if the Borg is out of "crucial" supplies.
 *
 * Note that we ignore "restock" issues for the first several turns
 * on each level, to prevent repeated "level bouncing".
 */
cptr borg_restock(int depth)
{

	/* We are now looking at our preparedness */
	if (-1 == borg_ready_morgoth)
		borg_ready_morgoth = 0;

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
	if (-1 == borg_ready_morgoth)
		borg_ready_morgoth = 0;
	if (borg_skill[BI_KING])
	{
		borg_ready_morgoth = 1;
		return ((cptr)NULL);
	}

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
#if 0
	/* Scrolls of Identify */
	if (amt_ident < 2 && (borg_skill[BI_CDEPTH])) return ("2 ident");
#endif
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

#if 0
	/* Scrolls of Identify */
	if (amt_ident < 5 && (borg_skill[BI_CDEPTH])) return ("5 idents");
#endif
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
#if 0
	/* Identify */
	if (amt_ident < 10) return ("ident10");
#endif
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
	if (!borg_skill[BI_HLIFE] && (borg_skill[BI_MAXCLEVEL] < 50)) return ("hold life");

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

/* Buffer for borg_prepared()
 */
#define MAX_REASON 1024

static char borg_prepared_buffer[MAX_REASON];

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

	/* -1 is unknown. */
	borg_ready_morgoth = -1;

	/* Town and First level */
	if (depth == 1) return ((cptr)NULL);

	/* Not prepared if I need to restock */
	if ((reason = borg_restock(depth))) return (reason);

	/* Must meet minimal requirements */
	if ((reason = borg_prepared_aux2(depth))) return (reason);


	/* Once Morgoth is dead */
	if (borg_skill[BI_KING])
	{
		return ((cptr)NULL);
	}

	/* Always okay from town */
	if (!borg_skill[BI_CDEPTH]) return ((cptr)NULL);

	/* check to make sure the borg does not go below where 2 living */
	/* uniques are. */

	if (borg_skill[BI_MAXDEPTH] <= 98)
	{
		int numb_live_unique = 0, i;
		int living_unique_index = 0;

		monster_race *r_ptr;

		/* BIG HACK, should check to make sure he has seen the unique. */
		/* !FIX change this to use the 'list of uniques (|) command AJG */
		for (i = 1; i < z_info->r_max; i++)
		{
			/* If any have been killed it is not a live unique */
			if (borg_race_death[i] != 0) continue;

			r_ptr = &r_info[i];

			/* Skip non-monsters */
			if (!r_ptr->name) continue;

			/* Skip non-uniques */
			if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

			/* Skip Friendly */
			if (r_ptr->flags7 & RF7_FRIENDLY) continue;

			/* Skip those that only live in the Wild */
			if (!(r_ptr->flags8 & RF8_DUNGEON)) continue;

			/* skip if deeper than dlevel */
			if (r_ptr->level > depth) break;

			numb_live_unique++;
			if (i < living_unique_index ||
				living_unique_index == 0) living_unique_index = i;
			continue;
		}

		if (numb_live_unique < 3 || borg_plays_risky)
		{
			return ((cptr)NULL);
		}
		/* scum for the uniques and report */
		r_ptr = &r_info[living_unique_index];
#if 0
		return ("Living uniques >= 3");
#endif

		/* To avoid double calls to format(); */
		strnfmt(borg_prepared_buffer, MAX_REASON, "Must kill %s.",
				r_name + r_ptr->name);

		return (borg_prepared_buffer);

	}
	else
		/* check to make sure the borg does not go to level 100 */
		/* unless all the uniques are dead. */
	{
		int numb_live_unique = 0, i;
		monster_race *r_ptr;
		int living_unique_index = 0;


		/* BIG HACK, should check to make sure he has seen the unique. */
		/* !FIX change this to use the 'list of uniques (|) command AJG */
		for (i = 1; i < z_info->r_max; i++)
		{
			/* If any have been killed it is not a live unique */
			if (borg_race_death[i] != 0) continue;

			r_ptr = &r_info[i];

			/* Skip non-monsters */
			if (!r_ptr->name) continue;

			/* Skip non-uniques */
			if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

			/* skip if deeper than dlevel */
			if (r_ptr->level >= depth) break;

			/* skip the check on Questors */
			if (r_ptr->flags1 & RF1_QUESTOR) continue;

			/* Skip Friendly */
			if (strstr(r_name + r_ptr->name, "Farmer Maggot")) continue;

			numb_live_unique++;
			living_unique_index = i;
			continue;
		}

		if (numb_live_unique < 1)
		{
			if (depth > 99) borg_ready_morgoth = 1;
			return ((cptr)NULL);
		}
		/* Under special cases allow the borg to dive to 99 then quickly
		 * get his butt to dlevel 98
		 */
		if (borg_skill[BI_MAXDEPTH] == 99 && depth <= 98 &&
			(randint0(100) < 3 ||
			 borg_spell_legal_fail(REALM_SORCERY, 2, 6, 40) ||
			 borg_spell_legal_fail(REALM_TRUMP, 1, 5, 40) || borg_has[187]))
		{
			return ((cptr)NULL);
		}
		/* Report */
		r_ptr = &r_info[living_unique_index];

		/* To avoid double calls to format(); */
		strnfmt(borg_prepared_buffer, MAX_REASON, "Must kill %s.",
				r_name + r_ptr->name);

		return (borg_prepared_buffer);
	}

}

/*
 * Initialize this file
 */
void borg_init_4(void)
{
	/* Do nothing? */
}



#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
