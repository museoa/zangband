/* File: borg4.c */
/*  Purpose: Notice and Power code for the Borg -BEN- */

#include "angband.h"

#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"
#include "zborg4.h"


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

int num_book[8][4];	/* [realm][book] */

int num_fix_stat[7];	/* #7 is to fix all stats */

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
int num_brand_weapon;	/*apw brand bolts */
int num_genocide;

s16b home_slot_free;
s16b home_damage;
s16b num_duplicate_items;
int num_slow_digest;
int num_regenerate;
int num_telepathy;
int num_lite;
int num_see_inv;
int num_invisible;	/* apw */

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
 * Remember items in the home.  (Only one home at a time)
 */
list_item *borg_home;
int home_num;


/* The shop that corresponds to the current home */
int home_shop = -1;

/* Include shop items in power calculation */
int use_shop;

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
	int i;

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
			/* Inventory changed so goals must change. */
			goal_shop = -1;

			/* Note changed inventory */
			borg_do_crush_junk = TRUE;
			borg_do_crush_hole = TRUE;
			borg_do_crush_slow = TRUE;
			break;
		}

		case LIST_EQUIP:
		{
			/* Equipment changed so goals must change. */
			goal_shop = -1;

			/* Note changed inventory */
			borg_do_crush_junk = TRUE;
			borg_do_crush_hole = TRUE;
			borg_do_crush_slow = TRUE;

			break;
		}

		case LIST_FLOOR:
		{

			break;
		}

		case LIST_STORE:
		{
			/* Notice store inventory changes */
			
			/* Mega-hack - update position */
			c_x = p_ptr->px;
			c_y = p_ptr->py;

			/* Silly value */
			shop_num = -1;

			/* Scan for the right shop */
			for (i = 0; i < track_shop_num; i++)
			{
				if ((borg_shops[i].x == c_x) && (borg_shops[i].y == c_y))
				{
					shop_num = i;
					break;
				}
			}
			
			/* Paranoia */
			if (shop_num == -1) quit("Could not find store!");

			/* Clear the goal */
			goal = 0;

			break;
		}

		case LIST_HOME:
		{
			/* Notice home inventory changes */
			
			/* Mega-hack - update position */
			c_x = p_ptr->px;
			c_y = p_ptr->py;

			/* Silly value */
			shop_num = -1;

			/* Scan for the home */
			for (i = 0; i < track_shop_num; i++)
			{
				if ((borg_shops[i].x == c_x) && (borg_shops[i].y == c_y))
				{
					shop_num = i;
					home_shop = i;
					break;
				}
			}
			
			/* Paranoia */
			if (shop_num == -1) quit("Could not find home!");

			/* Clear the goal */
			goal = 0;

			/* Save items for later... */

			/* Number of items */
			home_num = cur_num;

			/* Save items for later... */
			C_COPY(borg_home, cur_list, cur_num, list_item);

			break;
		}
		default:
		{
			/* Paranoia */
			quit(format("Unrecognised list type %d", list_type));
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
list_item *look_up_equip_slot(int slot)
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
	
	/* Look at current shop */
	if (use_shop)
	{
		for (i = 0; i < cur_num; i++)
		{
			l_ptr = &cur_list[i];

			/* Does it exist and are we aware? */
			if (l_ptr->k_idx)
			{
				/* The item to swap with */
				if (l_ptr->treat_as == TREAT_AS_SWAP) return (l_ptr);
			}
		}
	}
	else	
	{
		/* Otherwise, scan the inventory */
		for (i = 0; i < inven_num; i++)
		{
			l_ptr = &inventory[i];

			/* Does it exist and are we aware? */
			if (l_ptr->k_idx)
			{
				/* The item to swap with */
				if (l_ptr->treat_as != TREAT_AS_NORM) return (l_ptr);
			}
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
		borg_skill[BI_WEIGHT] += l_ptr->weight;

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
	borg_skill[BI_WEIGHT] += l_ptr->weight * number;

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
			if (borg_skill[BI_CUR_LITE] == 2) borg_skill[BI_AFUEL] += number;

			/* Count as Missiles */
			if (borg_skill[BI_CLEVEL] < 15) borg_skill[BI_AMISSILES] += number;

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
				if (borg_spell_okay_fail(REALM_LIFE, 7, 3, 40) && number >= 5)
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
		if ((l_ptr->treat_as == TREAT_AS_LESS) && (l_ptr->number == 1))
		{
			 continue;
		}

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
	
	/* Scan home for swapped items */
	for (i = 0; i < home_num; i++)
	{
		l_ptr = &borg_home[i];
		
		/* Hack - only 'LESS' items are treated as going into inven */
		if (l_ptr->treat_as == TREAT_AS_LESS)
		{
			int num = l_ptr->number;
				
			/* Hack - assume we get one item */
			l_ptr->number = 1;
				
			/* Examine the item */
			borg_notice_inven_item(l_ptr);

			/* Restore number */
			l_ptr->number = num;

			/* Done (Only one extra item) */
			return;
		}
		
		/* Sometimes want to grab a whole pile */
		if (l_ptr->treat_as == TREAT_AS_SWAP)
		{
			/* Examine the item */
			borg_notice_inven_item(l_ptr);
			
			/* Done (Only one extra item) */
			return;
		}
	}
	
	/* Scan current shop? */
	if (use_shop)
	{
		for (i = 0; i < cur_num; i++)
		{
			l_ptr = &cur_list[i];

			/* A known item? */
			if (l_ptr->k_idx)
			{
				/* Hack - only 'LESS' items are treated as going into inven */
				if (l_ptr->treat_as == TREAT_AS_LESS)
				{
					int num = l_ptr->number;
				
					/* Hack - assume we get one item */
					l_ptr->number = 1;
				
					/* Examine the item */
					borg_notice_inven_item(l_ptr);

					/* Restore number */
					l_ptr->number = num;

					/* Done (Only one extra item) */
					return;
				}
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

	/*
	 * Correct BI_ENCUMBERD from total weight to the degree
	 * of being overweight.
	 */
	/* Extract the "weight limit" (in tenth pounds) */
	carry_capacity = (adj_str_wgt[my_stat_ind[A_STR]] * 100) / 2;

	/* over or under the limit */
	if (borg_skill[BI_WEIGHT] > carry_capacity)
	{
		borg_skill[BI_ENCUMBERD] = (borg_skill[BI_WEIGHT] - carry_capacity);
	}
	else
	{
		borg_skill[BI_ENCUMBERD] = 0;
	}
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


/*
 * Helper function -- clear home values
 */
static void borg_notice_home_clear(void)
{
	int i, ii;

	/*** Reset counters ***/

	/* Reset basic */
	num_food = 0;
	num_ident = 0;
	num_star_ident = 0;
	num_recall = 0;
	num_phase = 0;
	num_escape = 0;
	num_teleport = 0;
	num_teleport_level = 0;

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
	num_ez_heal_true = 0;
	num_heal_true = 0;


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


/*
 * Notice flags on item
 */
static void borg_notice_home_flags(list_item *l_ptr)
{
	if (l_ptr->kn_flags3 & TR3_SLOW_DIGEST) num_slow_digest += l_ptr->number;
	if (l_ptr->kn_flags3 & TR3_REGEN) num_regenerate += l_ptr->number;
	if (l_ptr->kn_flags3 & TR3_TELEPATHY) num_telepathy += l_ptr->number;
	if (l_ptr->kn_flags3 & TR3_SEE_INVIS) num_see_inv += l_ptr->number;
	if (l_ptr->kn_flags3 & TR3_FEATHER) num_ffall += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_FREE_ACT) num_free_act += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_HOLD_LIFE) num_hold_life += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_IM_FIRE)
	{
		num_immune_fire += l_ptr->number;
		num_resist_fire += l_ptr->number;
	}
	if (l_ptr->kn_flags2 & TR2_IM_ACID)
	{
		num_immune_acid += l_ptr->number;
		num_resist_acid += l_ptr->number;
	}
	if (l_ptr->kn_flags2 & TR2_IM_COLD)
	{
		num_immune_cold += l_ptr->number;
		num_resist_cold += l_ptr->number;
	}
	if (l_ptr->kn_flags2 & TR2_IM_ELEC)
	{
		num_immune_elec += l_ptr->number;
		num_resist_elec += l_ptr->number;
	}
	if (l_ptr->kn_flags2 & TR2_RES_ACID) num_resist_acid += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_ELEC) num_resist_elec += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_FIRE) num_resist_fire += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_COLD) num_resist_cold += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_POIS) num_resist_pois += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_SOUND) num_resist_sound += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_LITE) num_resist_lite += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_DARK) num_resist_dark += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_CHAOS) num_resist_chaos += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_CONF) num_resist_conf += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_DISEN) num_resist_disen += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_SHARDS) num_resist_shard += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_NEXUS) num_resist_nexus += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_BLIND) num_resist_blind += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_RES_NETHER) num_resist_neth += l_ptr->number;

	/* Count Sustains */
	if (l_ptr->kn_flags2 & TR2_SUST_STR) num_sustain_str += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_SUST_INT) num_sustain_str += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_SUST_WIS) num_sustain_str += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_SUST_DEX) num_sustain_str += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_SUST_CON) num_sustain_str += l_ptr->number;
	if (l_ptr->kn_flags2 & TR2_SUST_STR &&
		l_ptr->kn_flags2 & TR2_SUST_INT &&
		l_ptr->kn_flags2 & TR2_SUST_WIS &&
		l_ptr->kn_flags2 & TR2_SUST_DEX &&
		l_ptr->kn_flags2 & TR2_SUST_CON) num_sustain_all += l_ptr->number;

	/* count up bonus to stats */
	if (l_ptr->kn_flags1 & TR1_STR)
	{
		if (l_ptr->tval != TV_RING)
			home_stat_add[A_STR] += l_ptr->pval * l_ptr->number;
	}
	if (l_ptr->kn_flags1 & TR1_INT)
	{
		if (l_ptr->tval != TV_RING)
			home_stat_add[A_INT] += l_ptr->pval * l_ptr->number;
	}
	if (l_ptr->kn_flags1 & TR1_WIS)
	{
		if (l_ptr->tval != TV_RING)
			home_stat_add[A_WIS] += l_ptr->pval * l_ptr->number;
	}
	if (l_ptr->kn_flags1 & TR1_DEX)
	{
		if (l_ptr->tval != TV_RING)
			home_stat_add[A_DEX] += l_ptr->pval * l_ptr->number;
	}
	if (l_ptr->kn_flags1 & TR1_CON)
	{
		if (l_ptr->tval != TV_RING)
			home_stat_add[A_CON] += l_ptr->pval * l_ptr->number;
	}
	if (l_ptr->kn_flags1 & TR1_CHR)
	{
		if (l_ptr->tval != TV_RING)
			home_stat_add[A_CHR] += l_ptr->pval * l_ptr->number;
	}

	/* count up bonus to speed */
	if (l_ptr->kn_flags1 & TR1_SPEED) num_speed += l_ptr->pval * l_ptr->number;
}


/*
 * This checks for duplicate items in the home
 */
static void borg_notice_home_dupe(list_item *l_ptr, bool check_sval, int i)
{
	int dupe_count, x;
	list_item *w_ptr;

	dupe_count = l_ptr->number - 1;

	/* Look for other items before this one that are the same */
	for (x = 0; x < i; x++)
	{
		if (x < home_num)
			w_ptr = &borg_home[x];
		else
			/* Check what the borg has on as well. */
			w_ptr = &equipment[x - home_num];
		
		/* Don't count items we are swapping */
		if (w_ptr->treat_as == TREAT_AS_SWAP) continue;

		/*
		 * If everything matches it is a duplicate item
		 * Note that we only check sval on certain items.  This
		 * is because, for example, two pairs of dragon armor
		 * are not the same unless their subtype (color) matches
		 * but a defender is a defender even if one is a dagger and
		 * one is a mace
		 */
		if (l_ptr->tval == w_ptr->tval)
		{
			if (check_sval &&
				(k_info[l_ptr->k_idx].sval != k_info[w_ptr->k_idx].sval))
			{
				/* Svals don't match when required */
				continue;
			}
			
			/* Does only one have an xtra name? */
			if ((l_ptr->xtra_name == NULL) !=
				(w_ptr->xtra_name == NULL)) continue;
			
			/* Do the xtra names match? */
			if ((l_ptr->xtra_name == w_ptr->xtra_name) ||
				(streq(l_ptr->xtra_name, w_ptr->xtra_name)))
			{
				/* Count duplicate items */
				dupe_count++;
			}
		}
	}

	/* There can be one dupe of rings because there are two ring slots. */
	if (l_ptr->tval == TV_RING && dupe_count) dupe_count--;

	/* Add this items count to the total duplicate count */
	num_duplicate_items += dupe_count;
}

/*
 * Examine weapons in the home
 */
static void borg_notice_home_weapon(list_item *l_ptr)
{
	s16b num_blow;

	int str_index, dex_index;
	int num = 0, wgt = 0, mul = 0, div = 0;

	num_weapons += l_ptr->number;

	/* apw most edged weapons hurt magic for priests */
	if (borg_class == CLASS_PRIEST)
	{
		/* Penalize non-blessed edged weapons */
		if (((l_ptr->tval == TV_SWORD) || (l_ptr->tval == TV_POLEARM))
			&& (!(l_ptr->kn_flags3 & TR3_BLESSED)))
		{
			num_edged_weapon += l_ptr->number;
		}
	}


	/*
	 * NOTE:  This damage does not take slays into account.
	 * It is just a rough estimate to make sure the glave of pain
	 * is kept if it is found.
	 * It is hard to hold a heavy weapon.
	 */
	num_blow = 1;

	if (adj_str_hold[my_stat_ind[A_STR]] >= l_ptr->weight / 10)
	{

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
		div = ((l_ptr->weight < wgt) ? wgt : l_ptr->weight);

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

	if (l_ptr->kn_flags1 & TR1_BLOWS) num_blow += l_ptr->pval;

	num_blow *= l_ptr->number;

	if (l_ptr->to_d > 8 || borg_skill[BI_CLEVEL] < 15)
	{
		home_damage += num_blow * (l_ptr->dd * l_ptr->ds +
								   (borg_skill[BI_TODAM] + l_ptr->to_d));
	}
	else
	{
		home_damage += num_blow * (l_ptr->dd * l_ptr->ds +
								   (borg_skill[BI_TODAM] + 8));
	}
}

/*
 * Examine potions in the home
 */
static void borg_notice_home_potion(list_item *l_ptr)
{
	/* Analyze */
	switch (k_info[l_ptr->k_idx].sval)
	{
		case SV_POTION_CURE_CRITICAL:
		{
			num_cure_critical += l_ptr->number;
			break;
		}

		case SV_POTION_CURE_SERIOUS:
		{
			num_cure_serious += l_ptr->number;
			break;
		}

		case SV_POTION_RESIST_HEAT:
		{
			num_pot_rheat += l_ptr->number;
			break;
		}

		case SV_POTION_RESIST_COLD:
		{
			num_pot_rcold += l_ptr->number;
			break;
		}

		case SV_POTION_RES_STR:
		{
			num_fix_stat[A_STR] += l_ptr->number;
			break;
		}

		case SV_POTION_RES_INT:
		{
			num_fix_stat[A_INT] += l_ptr->number;
			break;
		}

		case SV_POTION_RES_WIS:
		{
			num_fix_stat[A_WIS] += l_ptr->number;
			break;
		}

		case SV_POTION_RES_DEX:
		{
			num_fix_stat[A_DEX] += l_ptr->number;
			break;
		}

		case SV_POTION_RES_CON:
		{
			num_fix_stat[A_CON] += l_ptr->number;
			break;
		}

		case SV_POTION_RES_CHR:
		{
			num_fix_stat[A_CHR] += l_ptr->number;
			break;
		}

		case SV_POTION_RESTORE_EXP:
		{
			num_fix_exp += l_ptr->number;
			break;
		}

		case SV_POTION_RESTORE_MANA:
		{
			num_mana += l_ptr->number;
			break;
		}

		case SV_POTION_HEALING:
		{
			num_heal += l_ptr->number;
			num_heal_true += l_ptr->number;
			break;
		}

		case SV_POTION_STAR_HEALING:
		{
			num_ez_heal += l_ptr->number;
			num_ez_heal_true += l_ptr->number;
			break;
		}

		case SV_POTION_LIFE:
		{
			num_ez_heal += l_ptr->number;
			num_ez_heal_true += l_ptr->number;
			break;
		}

		case SV_POTION_BERSERK_STRENGTH:
		{
			num_berserk += l_ptr->number;
			break;
		}

		case SV_POTION_SPEED:
		{
			num_speed += l_ptr->number;
			break;
		}

		case SV_POTION_INVULNERABILITY:
		{
			num_goi_pot += l_ptr->number;
			break;
		}

		case SV_POTION_RESISTANCE:
		{
			num_resist_pot += l_ptr->number;
			break;
		}
	}
}

/*
 * Examine scrolls in the home
 */
static void borg_notice_home_scroll(list_item *l_ptr)
{
	/* Analyze the scroll */
	switch (k_info[l_ptr->k_idx].sval)
	{
		case SV_SCROLL_IDENTIFY:
		{
			num_ident += l_ptr->number;
			break;
		}

		case SV_SCROLL_STAR_IDENTIFY:
		{
			num_star_ident += l_ptr->number;
			break;
		}

		case SV_SCROLL_PHASE_DOOR:
		{
			num_phase += l_ptr->number;
			break;
		}

		case SV_SCROLL_TELEPORT:
		{
			num_escape += l_ptr->number;
			break;
		}

		case SV_SCROLL_WORD_OF_RECALL:
		{
			num_recall += l_ptr->number;
			break;
		}

		case SV_SCROLL_ENCHANT_ARMOR:
		{
			num_enchant_to_a += l_ptr->number;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		{
			num_enchant_to_h += l_ptr->number;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			num_enchant_to_d += l_ptr->number;
			break;
		}

		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			num_pfe += l_ptr->number;
			break;
		}

		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			num_glyph += l_ptr->number;
			break;
		}

		case SV_SCROLL_TELEPORT_LEVEL:
		{
			num_teleport_level += l_ptr->number;
			break;
		}

		case SV_SCROLL_SATISFY_HUNGER:
		{
			num_food += l_ptr->number;
			break;
		}
	}
}

/*
 * Include effects of spells on home items
 */
static void borg_notice_home_spells(void)
{
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
		(borg_skill[BI_CDEPTH] == 100 &&
		 (borg_spell_legal(REALM_LIFE, 3, 6) ||
		  borg_spell_legal(REALM_SORCERY, 2, 7) ||
		  borg_spell_legal(REALM_TRUMP, 1, 6))))
	{
		num_recall += 1000;
	}

	/* Handle teleport_level */
	if (borg_spell_legal_fail(REALM_SORCERY, 2, 6, 40) ||
		borg_spell_legal_fail(REALM_TRUMP, 1, 5, 40))
	{
		num_teleport_level += 1000;
	}
}


/*
 * Innate abilities of the player can affect home item choice
 */
static void borg_notice_home_player(void)
{
	u32b f1, f2, f3;

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


/*
 * Notice a particular item
 */
static void borg_notice_home_item(list_item *l_ptr, int i)
{
	/* Analyze the item */
	switch (l_ptr->tval)
	{
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		{
			num_armor += l_ptr->number;

			/* see if this item is duplicated */
			borg_notice_home_dupe(l_ptr, FALSE, i);
			break;
		}

		case TV_DRAG_ARMOR:
		{
			num_armor += l_ptr->number;

			/* see if this item is duplicated */
			borg_notice_home_dupe(l_ptr, TRUE, i);
			break;
		}

		case TV_CLOAK:
		{
			num_cloaks += l_ptr->number;

			/* see if this item is duplicated */
			borg_notice_home_dupe(l_ptr, FALSE, i);

			break;
		}

		case TV_SHIELD:
		{
			num_shields += l_ptr->number;

			/* see if this item is duplicated */
			borg_notice_home_dupe(l_ptr, FALSE, i);
			break;
		}

		case TV_HELM:
		case TV_CROWN:
		{
			num_hats += l_ptr->number;

			/* see if this item is duplicated */
			borg_notice_home_dupe(l_ptr, FALSE, i);

			break;
		}

		case TV_GLOVES:
		{
			num_gloves += l_ptr->number;

			/* most gloves hurt magic for spell-casters */
			if (borg_skill[BI_INTMANA] && borg_skill[BI_MAXSP] > 3)
			{
				/* Penalize non-usable gloves */
				if (l_ptr->number &&
					(!(l_ptr->kn_flags2 & TR2_FREE_ACT)) &&
					(!((l_ptr->kn_flags1 & TR1_DEX) && (l_ptr->pval > 0))))
				{
					num_bad_gloves += l_ptr->number;
				}
			}

			/* gloves of slaying give a damage bonus */
			home_damage += l_ptr->to_d * 3;

			/* see if this item is duplicated */
			borg_notice_home_dupe(l_ptr, FALSE, i);

			break;
		}

		case TV_LITE:
		{
			if (l_ptr->kn_flags3 & TR3_INSTA_ART)
			{
				num_lite += l_ptr->number;
			}
			break;
		}

		case TV_BOOTS:
		{
			num_boots += l_ptr->number;

			/* see if this item is duplicated */
			borg_notice_home_dupe(l_ptr, FALSE, i);
			break;
		}

		case TV_SWORD:
		case TV_POLEARM:
		case TV_HAFTED:
		case TV_DIGGING:
		{
			/* Look at weapon information */
			borg_notice_home_weapon(l_ptr);

			/* see if this item is a duplicate */
			borg_notice_home_dupe(l_ptr, FALSE, i);
			break;
		}

		case TV_BOW:
		{
			num_bow += l_ptr->number;

			/* see if this item is a duplicate */
			borg_notice_home_dupe(l_ptr, FALSE, i);
			break;
		}

		case TV_RING:
		{
			num_rings += l_ptr->number;

			/* see if this item is a duplicate */
			borg_notice_home_dupe(l_ptr, TRUE, i);

			break;
		}

		case TV_AMULET:
		{
			num_neck += l_ptr->number;

			/* see if this item is a duplicate */
			borg_notice_home_dupe(l_ptr, TRUE, i);
			break;
		}

		case TV_LIFE_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_LIFE ||
				borg_skill[BI_REALM2] == REALM_LIFE)
				num_book[REALM_LIFE][k_info[l_ptr->k_idx].sval] +=
					l_ptr->number;
			break;
		}

		case TV_SORCERY_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_SORCERY ||
				borg_skill[BI_REALM2] == REALM_SORCERY)
				num_book[REALM_SORCERY][k_info[l_ptr->k_idx].sval] +=
					l_ptr->number;
			break;
		}

		case TV_NATURE_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_NATURE ||
				borg_skill[BI_REALM2] == REALM_NATURE)
				num_book[REALM_NATURE][k_info[l_ptr->k_idx].sval] +=
					l_ptr->number;
			break;
		}
		case TV_CHAOS_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_CHAOS ||
				borg_skill[BI_REALM2] == REALM_CHAOS)
				num_book[REALM_CHAOS][k_info[l_ptr->k_idx].sval] +=
					l_ptr->number;
			break;
		}
		case TV_DEATH_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_DEATH ||
				borg_skill[BI_REALM2] == REALM_DEATH)
				num_book[REALM_DEATH][k_info[l_ptr->k_idx].sval] +=
					l_ptr->number;
			break;
		}
		case TV_TRUMP_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_TRUMP ||
				borg_skill[BI_REALM2] == REALM_TRUMP)
				num_book[REALM_TRUMP][k_info[l_ptr->k_idx].sval] +=
					l_ptr->number;
			break;
		}
		case TV_ARCANE_BOOK:
		{
			/* Count good books */
			if (borg_skill[BI_REALM1] == REALM_ARCANE ||
				borg_skill[BI_REALM2] == REALM_ARCANE)
				num_book[REALM_ARCANE][k_info[l_ptr->k_idx].sval] +=
					l_ptr->number;
			break;
		}

		case TV_FOOD:
		{
			/* Food */

			/* Analyze */
			switch (k_info[l_ptr->k_idx].sval)
			{
				case SV_FOOD_WAYBREAD:
				case SV_FOOD_RATION:
				{
					if (borg_race >= RACE_SPRITE && borg_race <= RACE_IMP)
					{
						num_food += l_ptr->number;
					}
					break;
				}

				case SV_FOOD_RESTORE_STR:
				{
					num_fix_stat[A_STR] += l_ptr->number;
					break;
				}

				case SV_FOOD_RESTORE_CON:
				{
					num_fix_stat[A_CON] += l_ptr->number;
					break;
				}

				case SV_FOOD_RESTORING:
				{
					num_fix_stat[A_STR] += l_ptr->number;
					num_fix_stat[A_INT] += l_ptr->number;
					num_fix_stat[A_WIS] += l_ptr->number;
					num_fix_stat[A_DEX] += l_ptr->number;
					num_fix_stat[A_CON] += l_ptr->number;
					num_fix_stat[A_CHR] += l_ptr->number;
					num_fix_stat[6] += l_ptr->number;
					break;
				}
			}

			break;
		}


		case TV_POTION:
		{
			/* Potions */
			borg_notice_home_potion(l_ptr);

			break;
		}


		case TV_SCROLL:
		{
			/* Scrolls */
			borg_notice_home_scroll(l_ptr);

			break;
		}


		case TV_ROD:
		{
			/* Rods */

			/* Analyze */
			switch (k_info[l_ptr->k_idx].sval)
			{
				case SV_ROD_IDENTIFY:
				{
					num_ident += l_ptr->number * 100;
					break;
				}

				case SV_ROD_RECALL:
				{
					num_recall += l_ptr->number * 100;
					break;
				}
			}

			break;
		}

		case TV_STAFF:
		{
			/* Staffs */

			/* Only collect staves with more than 3 charges at high level */
			if (l_ptr->pval <= 3 && borg_skill[BI_CLEVEL] > 30)
				break;

			/* Analyze */
			switch (k_info[l_ptr->k_idx].sval)
			{
				case SV_STAFF_IDENTIFY:
				{
					num_ident += l_ptr->number * l_ptr->pval;
					break;
				}

				case SV_STAFF_TELEPORTATION:
				{
					/*
					 * Don't use them deep in the dungeon because the
					 * charges will get drained and he wont have any
					 * scrolls left to read
					 */
					if (borg_skill[BI_MAXDEPTH] < 97)
					{
						num_teleport += l_ptr->number * l_ptr->pval;
					}
					break;
				}
			}

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
			num_missile += l_ptr->number;

			break;
		}
	}
}


/*
 * Helper function -- notice the home inventory
 */
static void borg_notice_home_aux(void)
{
	int i;
	int num;

	list_item *l_ptr;

	/*** Process the inventory ***/

	/* Scan the home */
	for (i = 0; i < (home_num + EQUIP_MAX); i++)
	{
		if (i < STORE_INVEN_MAX)
			l_ptr = &borg_home[i];
		else
			l_ptr = &equipment[i - home_num];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;
		
		/* Don't count items we are swapping */
		if (l_ptr->treat_as == TREAT_AS_SWAP) continue;
		
		/* Save number of items */
		num = l_ptr->number;
		
		/* Hack - simulate change in number of items */
		if (l_ptr->treat_as == TREAT_AS_LESS) l_ptr->number--;
		if (l_ptr->treat_as == TREAT_AS_MORE) l_ptr->number++;

		/* Notice item flags */
		borg_notice_home_flags(l_ptr);

		/* Notice the item itself */
		borg_notice_home_item(l_ptr, i);
		
		/* Hack - revert change in number of items */
		l_ptr->number = num;
	}
	
	/* Scan the inventory for virtual home items */
	for (i = 0; i < inven_num; i++)
	{
		l_ptr = &inventory[i];

		/* Ignore normal items */
		if (l_ptr->treat_as == TREAT_AS_NORM) continue;
		
		/* Save number of items */
		num = l_ptr->number;
		
		/* Hack - simulate change in number of items */
		if (l_ptr->treat_as == TREAT_AS_LESS) l_ptr->number = 1;
		if (l_ptr->treat_as == TREAT_AS_MORE) l_ptr->number++;
		
		/* Notice item flags */
		borg_notice_home_flags(l_ptr);

		/* Notice the item itself */
		borg_notice_home_item(l_ptr, home_num + EQUIP_MAX - 1);
		
		/* Hack - revert change in number of items */
		l_ptr->number = num;
	
		/* Hack - assume only one swap at a time */
		break;
	}
	
	/* Scan for virtual home items from stores */
	if (use_shop)
	{
		for (i = 0; i < cur_num; i++)
		{
			l_ptr = &cur_list[i];

			/* Ignore normal items */
			if (l_ptr->treat_as == TREAT_AS_NORM) continue;
		
			/* Save number of items */
			num = l_ptr->number;
		
			/* Hack - simulate change in number of items */
			if (l_ptr->treat_as == TREAT_AS_LESS) l_ptr->number = 1;
			if (l_ptr->treat_as == TREAT_AS_MORE) l_ptr->number++;
		
			/* Notice item flags */
			borg_notice_home_flags(l_ptr);

			/* Notice the item itself */
			borg_notice_home_item(l_ptr, home_num + EQUIP_MAX - 1);
		
			/* Hack - revert change in number of items */
			l_ptr->number = num;
		}
	}

	/*** Process the Spells and Prayers ***/
	borg_notice_home_spells();

	/*** Process the player abilities ***/
	borg_notice_home_player();
}


/*
 * Extract the bonuses for items in the home.
 */
void borg_notice_home(void)
{
	/* Notice the home equipment */
	borg_notice_home_clear();

	/* Notice the home inventory */
	borg_notice_home_aux();
}





/*
 * Helper function -- calculate power of equipment in the home
 */
static s32b borg_power_home_aux1(void)
{
	s32b value = 0L;

	/*
	 * This would be better seperated by item type
	 * (so 1 bonus for resist cold armor
	 *   1 bonus for resist cold shield...
	 * but that would take a bunch more code.
	 *
	 * Try to collect at least 2 of each resist/power (for swapping)
	 * This can be used to get rid of extra artifacts...
	 */

	/* Spare lite sources.  Artifacts only */
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

	/*
	 * Stat gain items as well...
	 * (good to carry ring of dex +6 in
	 * house even if I don't need it right now)
	 */
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

	/*
	 * Do a minus for too many duplicates.
	 * This way we do not store useless items
	 * and spread out types of items.
	 */
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

	/* If edged and priest, dump it   */
	value -= num_edged_weapon * 3000L;

	/* If gloves and mage or ranger and not FA/Dex, dump it. */
	value -= num_bad_gloves * 3000L;

	/* Do not allow duplication of items. */
	value -= num_duplicate_items * 5000L;

	/* Return the value */
	return (value);
}


/*
 * Helper function -- calculate power of items in the home
 *
 * The wierd calculations help spread out the purchase order
 */
static s32b borg_power_home_aux2(void)
{
	int k, book = 0, realm = 0;

	s32b value = 0L;


	/*** Basic abilities ***/

	/* Collect food */
	for (k = 0; k < 50 && k < num_food; k++) value += 8000L - k * 10L;

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

	/* Collect heal/mana/ */
	for (k = 0; k < 99 && k < num_heal; k++) value += 3000L - k * 8L;
	for (k = 0; k < 99 && k < num_ez_heal; k++) value += 8000L - k * 8L;
	if (borg_skill[BI_MAXSP] > 1)
	{
		for (k = 0; k < 99 && k < num_mana; k++) value += 6000L - k * 8L;
	}

	/*** Healing ***/

	/* Collect cure critical */
	for (k = 0; k < 99 && k < num_cure_critical; k++) value += 1500L - k * 10L;

	/* Collect cure serious - but they aren't as good */
	for (k = 0; k < 99 && k < num_cure_serious; k++) value += 750L - k * 100L;

	/*** Various ***/

	/* Fixing Stats */
	if (borg_skill[BI_CLEVEL] == 50) value += 500L * num_fix_exp;
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
		/* Only my realms */
		if (realm != borg_skill[BI_REALM1] &&
			realm != borg_skill[BI_REALM2]) continue;

		/* Scan Books */
		for (book = 0; book < 4; book++)
		{
			if (borg_skill[BI_CLEVEL] > 35)
			{
				/* Collect up to 20 copies of each normal book */
				for (k = 0; k < 20 && k < num_book[realm][book]; k++)
				{
					/* Hack -- only stockpile useful books */
					if (num_book[realm][book]) value += 5000L - k * 10L;
				}
			}
			else
			{
				/* Collect up to 5 copies of each normal book */
				for (k = 0; k < 5 && k < num_book[realm][book]; k++)
				{
					/* Hack -- only stockpile useful books */
					if (num_book[realm][book]) value += 5000L - k * 10L;
				}
			}
		}
	}

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
 * Initialize this file
 */
void borg_init_4(void)
{
	/* Make the home inventory array */
	C_MAKE(borg_home, STORE_INVEN_MAX, list_item);
}



#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
