/* File: zborg7.c */
/* Purpose: High level functions for the Borg -BEN- */

#include "angband.h"

#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"
#include "zborg4.h"
#include "zborg5.h"
#include "zborg6.h"
#include "zborg7.h"


/*
 * This file handles various high level inventory related goals.
 *
 * Problems:
 *   Use "time stamps" (and not "random" checks) for several routines,
 *   including "kill junk" and "wear stuff", and maybe even "free space".
 *   But be careful with the "free space" routine, wear stuff first.
 *   Make sure nothing is "destroyed" if we do not do them every turn.
 *   Consider some special routines in stores (and in the home).
 *
 * Hack -- We should perhaps consider wearing "harmless" items into empty
 * slots when in the dungeon, to allow rings/amulets to be brought back up
 * to town to be sold.
 *
 * We should take account of possible combinations of equipment.  This may
 * be a potentially expensive computation, but could be done occasionally.
 * It is important to use a "state-less" formula to allow the exchange to
 * be spread over multiple turns.
 *
 * Hack -- We should attempt to only collect non-discounted items, at least
 * for the "expensive" slots, such as books, since we do not want to lose
 * value due to stacking.  We seem to sell the non-discounted items first,
 * and to buy the discounted items first, since they are cheap.  Oh well,
 * we may just be stuck with using discounted books.  Unless we actually
 * do correct "combining" in the simulations, and reward free slots.  Ick!
 *
 * XXX XXX XXX We really need a better "twitchy" function.
 *
 * XXX XXX XXX We need a better "flee this level" function
 *
 * XXX XXX XXX We need to stockpile possible useful items at home.
 *
 * XXX XXX XXX Perhaps we could simply maintain a list of abilities
 * that we might need at some point, such as the ability to identify, and
 * simply allow the Borg to "sell" items to the home which satisfy this
 * desire for "abilities".
 *
 * XXX XXX XXX Also, we should probably attempt to track the "best" item
 * in the home for each equipment slot, using some form of heuristic, and
 * reward that item based on its power, so that the Borg would always
 * have a "backup" item in case of disenchantment.
 *
 * XXX XXX XXX Also, we could reward equipment based on possible enchantment,
 * up to the maximal amount available in the home, which would induce item
 * switching when the item could be enchanted sufficiently.
 *
 * Fleeing from fast spell-casters is probably not a very smart idea, nor is
 * fleeing from fast monsters, nor is attempting to clear a room full of fast
 * moving breeding monsters, such as lice.
 */



/*
 * Hack -- importance of the various "level feelings"
 * Try to explore the level for at least this many turns
 */
static s16b value_feeling[] =
{
	500,
	8000,
	8000,
	6000,
	4000,
	2000,
	1000,
	800,
	600,
	400,
	200,
	0
};




/* This function (copied from dungeon.c) delivers the chance for pseudo-id. */
static long borg_calc_pseudo(void)
{
	long difficulty;

	/* Based on race get the basic feel factor. */
	switch (borg_class)
	{
		case CLASS_WARRIOR:
		{
			/* Good (heavy) sensing */
			difficulty = 9000L;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		{
			/* Very bad (light) sensing */
			difficulty = 240000L;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		{
			/* Good (light) sensing */
			difficulty = 10000L;

			/* Done */
			break;
		}

		case CLASS_ROGUE:
		{
			/* Okay sensing */
			difficulty = 20000L;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		{
			/* Bad (heavy) sensing */
			difficulty = 95000L;

			/* Done */
			break;
		}

		case CLASS_PALADIN:
		{
			/* Bad (heavy) sensing */
			difficulty = 77777L;

			/* Done */
			break;
		}

		case CLASS_WARRIOR_MAGE:
		{
			/* Bad sensing */
			difficulty = 75000L;

			/* Done */
			break;
		}

		case CLASS_MINDCRAFTER:
		{
			/* Bad sensing */
			difficulty = 55000L;
	
			/* Done */
			break;
		}

		case CLASS_CHAOS_WARRIOR:
		{
			/* Bad (heavy) sensing */
			difficulty = 80000L;

			/* Done */
			break;
		}

		case CLASS_MONK:
		{
			/* Okay sensing */
			difficulty = 20000L;

			/* Done */
			break;
		}

		default:
		{
			/* Paranoia */
			difficulty = 0;
		}
	}

	/* Factor in the sensing ability */
	difficulty /= MAX(bp_ptr->skill_sns, 1);

	/* Rescale larger by a facter of 25 */
	difficulty *= 25;

	/* Sensing gets better as you get more experienced */
	difficulty /= p_ptr->lev * p_ptr->lev + 40;

	/* Give the answer */
	return (difficulty);
}

/*
 * Determine if an item is "probably" worthless
 *
 * This (very heuristic) function is a total hack, designed only to prevent
 * a very specific annoying situation described below.
 *
 * Note that a "cautious" priest (or low level mage/ranger) will leave town
 * with a few identify scrolls, wander around dungeon level 1 for a few turns,
 * and use all of the scrolls on leather gloves and broken daggers, and must
 * then return to town for more scrolls.  This may repeat indefinitely.
 *
 * The problem is that some characters (priests, mages, rangers) never get an
 * "average" feeling about items, and have no way to keep track of how long
 * they have been holding a given item for, so they cannot even attempt to
 * gain knowledge from the lack of "good" or "cursed" feelings.  But they
 * cannot afford to just identify everything they find by using scrolls of
 * identify, because, in general, some items are, on average, "icky", and
 * not even worth the price of a new scroll of identify.
 *
 * Even worse, the current algorithm refuses to sell un-identified items, so
 * the poor character will throw out all his good stuff to make room for crap.
 *
 * This function simply examines the item and assumes that certain items are
 * "icky", which is probably a total hack.  Perhaps we could do something like
 * compare the item to the item we are currently wearing, or perhaps we could
 * analyze the expected value of the item, or guess at the likelihood that the
 * item might be a blessed, or something.
 *
 */
bool borg_item_icky(list_item *l_ptr)
{
	int sval = k_info[l_ptr->k_idx].sval;

	/* if its average, dump it if you want to. */
	if (strstr(l_ptr->o_name, "{average")) return (TRUE);

	/* items that are terrible/excellent/special/tainted need ID */
	if (strstr(l_ptr->o_name, "{special") ||
		strstr(l_ptr->o_name, "{terrible") ||
		strstr(l_ptr->o_name, "{excellent") ||
		strstr(l_ptr->o_name, "{tainted")) return (FALSE);

	/* If your pseudo capabilities are non-existent */
	if (borg_calc_pseudo() > 100)
	{

		/* Swords */
		if (l_ptr->tval == TV_SWORD)
				return (sval == SV_BROKEN_DAGGER ||
						sval == SV_BROKEN_SWORD ||
						sval == SV_DAGGER);

		/* Hafted */
		if (l_ptr->tval == TV_HAFTED)
			return (sval == SV_CLUB ||
					sval == SV_WHIP);

		/* Sling */
		if (l_ptr->tval == TV_BOW) return (sval == SV_SLING);

		/* Rags and Robes */
		if (l_ptr->tval == TV_SOFT_ARMOR)
			return (sval == SV_FILTHY_RAG ||
					sval == SV_SOFT_LEATHER_ARMOR ||
					sval == SV_SOFT_STUDDED_LEATHER ||
					sval == SV_ROBE);

		/* Cloak */
		if (l_ptr->tval == TV_CLOAK) return (sval == SV_CLOAK);

		/* Leather Gloves */
		if (l_ptr->tval == TV_GLOVES)
			return (sval == SV_SET_OF_LEATHER_GLOVES);

		/* Helmet */
		if (l_ptr->tval == TV_HELM) return (sval == SV_HARD_LEATHER_CAP);

		/* Assume the item is not icky */
		return (FALSE);
	}


	/*** {Good} items in inven, But I have {excellent} in equip ***/

	if (strstr(l_ptr->o_name, "{good"))
	{
		int slot;

		/* Obtain the slot of the suspect item */
		slot = borg_wield_slot(l_ptr);

		/* Obtain my equipped item in the slot */
		l_ptr = look_up_equip_slot(slot);

		/* Is the equipped item an ego or artifact? */
		if (l_ptr &&
			(borg_obj_is_ego_art(l_ptr) ||
			strstr(l_ptr->o_name, "{special") ||
			strstr(l_ptr->o_name, "{terrible") ||
			strstr(l_ptr->o_name, "{excellent") ||
			strstr(l_ptr->o_name, "{tainted"))) return (TRUE);
	}

	/* Assume not icky, I should have extra ID for the item */
	return (FALSE);
}


/*
 * Use things in a useful, but non-essential, manner
 */
bool borg_use_things(void)
{
	int i;

	/* Quaff experience restoration potion */
	if (bp_ptr->status.fixexp &&
		(borg_activate_artifact(ART_LUTHIEN, FALSE) ||
		 borg_spell(REALM_LIFE, 3, 3) ||
		 borg_spell(REALM_DEATH, 1, 7) ||
		 borg_quaff_potion(SV_POTION_RESTORE_EXP)))
	{
		return (TRUE);
	}

	/* just drink the stat gains, at this dlevel we wont need cash */
	if (borg_quaff_potion(SV_POTION_INC_STR) ||
		borg_quaff_potion(SV_POTION_INC_INT) ||
		borg_quaff_potion(SV_POTION_INC_WIS) ||
		borg_quaff_potion(SV_POTION_INC_DEX) ||
		borg_quaff_potion(SV_POTION_INC_CON) ||
		borg_quaff_potion(SV_POTION_INC_CHR))
	{
		return (TRUE);
	}

	/* Quaff potions of "restore" stat if needed */
	if ((bp_ptr->status.fixstat[A_STR] &&
		 (borg_quaff_potion(SV_POTION_RES_STR) ||
		  borg_quaff_potion(SV_POTION_INC_STR) ||
		  borg_eat_food(SV_FOOD_RESTORE_STR) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(bp_ptr->status.fixstat[A_INT] &&
		 (borg_quaff_potion(SV_POTION_RES_INT) ||
		  borg_quaff_potion(SV_POTION_INC_INT) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(bp_ptr->status.fixstat[A_WIS] &&
		 (borg_quaff_potion(SV_POTION_RES_WIS) ||
		  borg_quaff_potion(SV_POTION_INC_WIS) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(bp_ptr->status.fixstat[A_DEX] &&
		 (borg_quaff_potion(SV_POTION_RES_DEX) ||
		  borg_quaff_potion(SV_POTION_INC_DEX) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(bp_ptr->status.fixstat[A_CON] &&
		 (borg_quaff_potion(SV_POTION_RES_CON) ||
		  borg_quaff_potion(SV_POTION_INC_CON) ||
		  borg_eat_food(SV_FOOD_RESTORE_CON) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(bp_ptr->status.fixstat[A_CHR] &&
		 (borg_quaff_potion(SV_POTION_RES_CHR) ||
		  borg_quaff_potion(SV_POTION_INC_CHR) ||
		  borg_eat_food(SV_FOOD_RESTORING))))
	{
		return (TRUE);
	}


	/* Use some items right away */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Process "force" items */
		switch (l_ptr->tval)
		{
			case TV_POTION:
			{
				/* Check the scroll */
				switch (k_info[l_ptr->k_idx].sval)
				{
					case SV_POTION_ENLIGHTENMENT:
					{

						/* Never quaff these in town */
						if (!bp_ptr->depth) break;

						/* fall through */
					}

					case SV_POTION_AUGMENTATION:
					case SV_POTION_EXPERIENCE:
					{

						/* Try quaffing the potion */
						if (borg_quaff_potion(k_info[l_ptr->k_idx].sval))
							return (TRUE);

						break;
					}
				}

				break;
			}
			case TV_SCROLL:
			{
				/* Hack -- check Blind/Confused */
				if (bp_ptr->status.blind || bp_ptr->status.confused) break;

				/* XXX XXX XXX Dark */

				/* Check the scroll */
				switch (k_info[l_ptr->k_idx].sval)
				{
					case SV_SCROLL_MAPPING:
					case SV_SCROLL_DETECT_TRAP:
					case SV_SCROLL_DETECT_DOOR:
					case SV_SCROLL_ACQUIREMENT:
					case SV_SCROLL_STAR_ACQUIREMENT:
					{
						/* Never read these in town */
						if (!bp_ptr->depth) break;

						/* Try reading the scroll */
						if (borg_read_scroll(k_info[l_ptr->k_idx].sval))
							return (TRUE);
						break;
					}
				}

				break;
			}
		}
	}

	/* Eat food */
	if (bp_ptr->status.hungry)
	{
		/* Attempt to satisfy hunger */
		if (borg_spell_fail(REALM_LIFE, 0, 7, 40) ||
			borg_spell_fail(REALM_ARCANE, 2, 6, 40) ||
			borg_spell_fail(REALM_NATURE, 0, 3, 40) ||
			borg_eat_food(SV_FOOD_BISCUIT) ||
			borg_eat_food(SV_FOOD_JERKY) ||
			borg_eat_food(SV_FOOD_RATION) ||
			borg_read_scroll(SV_SCROLL_SATISFY_HUNGER) ||
			borg_eat_food(SV_FOOD_WAYBREAD))
		{
			return (TRUE);
		}
	}

	/* Nothing to do */
	return (FALSE);
}



/*
 * Refuel, call lite, detect traps/doors/walls/evil, etc
 *
 * Note that we refuel whenever our lite starts to get low.
 *
 * Note that we detect traps/doors/walls/evil at least once in each
 * panel, as soon as possible after entering a new panel.
 *
 * Note that we call lite whenever the current grid is dark, and
 * all the grids touching the current grid diagonally are known
 * floors, which catches all rooms, including "checkerboard" rooms,
 * and only occasionally calls lite in corridors, and then only once.
 *
 * Note that we also sometimes call lite whenever we are using a
 * lantern or artifact lite, and when all of the grids in the box
 * of grids containing the maximal torch-lit region (the 5x5 or 7x7
 * region centered at the player) are non-glowing floor grids, and
 * when at least one of them is known to be "dark".  This catches
 * most of the "probable rooms" before the standard check succeeds.
 *
 * We use the special "SELF" messages to "borg_react()" to delay the
 * processing of "detection" and "call lite" until we know if it has
 * worked or not.
 *
 * The matching function borg_check_lite_only is used only with resting
 * to heal.  I don't want him teleporting into a room, resting to heal while
 * there is a dragon sitting in a dark corner waiting to breathe on him.
 * So now he will check for lite.
 *
 */
bool borg_check_lite(void)
{
	int i, x, y;
	int corners, floors;

	map_block *mb_ptr = map_loc(c_x, c_y);


	bool do_lite;

	bool do_trap;
	bool do_door;
	bool do_wall;
	bool do_evil;



	/* Never in town */
	if (!bp_ptr->depth) return (FALSE);

	/* Never when comprimised, save your mana */
	if (bp_ptr->status.blind || bp_ptr->status.confused ||
		bp_ptr->status.image || bp_ptr->status.poisoned ||
		bp_ptr->status.cut || bp_ptr->status.weak) return (FALSE);

	/* XXX XXX XXX Dark */


	/* Start */
	do_trap = FALSE;

	/* Determine if we need to detect traps */
	if (!(mb_ptr->detect & BORG_DETECT_TRAP)) do_trap = TRUE;

	/* Hack -- check traps every few turns anyway */
	if (!when_detect_traps ||
		(borg_t - when_detect_traps >= 183)) do_trap = TRUE;


	/* Start */
	do_door = FALSE;

	/* Determine if we need to detect doors */
	if (!(mb_ptr->detect & BORG_DETECT_DOOR)) do_door = TRUE;

	/* Hack -- check doors every few turns anyway */
	if (!when_detect_doors ||
		(borg_t - when_detect_doors >= 731)) do_door = TRUE;


	/* Start */
	do_wall = FALSE;

	/* Determine if we need to detect walls */
	if (!(mb_ptr->detect & BORG_DETECT_WALL)) do_wall = TRUE;

	/* Hack -- check walls every few turns anyway */
	if (!when_detect_walls ||
		(borg_t - when_detect_walls >= 937)) do_wall = TRUE;


	/* Start */
	do_evil = FALSE;

	/* Determine if we need to detect evil */
	if (!(mb_ptr->detect & BORG_DETECT_EVIL)) do_evil = TRUE;

	/* Hack -- check evil every few turns anyway- more fq if low level */
	if (!when_detect_evil ||
		(borg_t - when_detect_evil >=
		 183 - (20 - bp_ptr->max_lev))) do_evil = TRUE;

	/* Dont bother if I have ESP */
	if (FLAG(bp_ptr, TR_TELEPATHY)) do_evil = FALSE;

	/* Do not do these if monsters near.  Save mana */
	if (!borg_check_rest())
	{
		do_trap = FALSE;
		do_door = FALSE;
		do_wall = FALSE;
		do_evil = FALSE;
	}

	/*** Do Things ***/

	/* Hack -- find traps and doors and evil */
	if ((do_trap || do_door || do_evil) &&
		((!when_detect_traps || (borg_t - when_detect_traps >= 5)) ||
		 (!when_detect_evil || (borg_t - when_detect_evil >= 5)) ||
		 (!when_detect_doors || (borg_t - when_detect_doors >= 5))))
	{
		/* Check for traps and doors and evil */
		if (borg_zap_rod(SV_ROD_DETECTION) ||
			borg_activate_artifact(ART_HOLHENNETH, FALSE) ||
			borg_activate_artifact(ART_OLORIN, FALSE) ||
			borg_spell_fail(REALM_SORCERY, 1, 6, 20) ||
			borg_spell_fail(REALM_ARCANE, 3, 5, 20) ||
			borg_spell_fail(REALM_TRUMP, 3, 0, 20) ||
			borg_spell_fail(REALM_NATURE, 1, 2, 20))
		{
			borg_note("# Checking for traps, doors, and evil.");

			borg_react("SELF:TDE", "SELF:TDE");

			when_detect_traps = borg_t;
			when_detect_doors = borg_t;
			when_detect_evil = borg_t;

			return (TRUE);
		}
	}

	/* Hack -- find traps and doors */
	if ((do_trap || do_door) &&
		((!when_detect_traps || (borg_t - when_detect_traps >= 5)) ||
		 (!when_detect_doors || (borg_t - when_detect_doors >= 5))))
	{
		/* Check for traps and doors */
		if (borg_activate_artifact(ART_THRAIN, FALSE) ||
			borg_spell_fail(REALM_LIFE, 0, 5, 20) ||
			borg_spell_fail(REALM_SORCERY, 0, 2, 20) ||
			borg_spell_fail(REALM_ARCANE, 1, 0, 20) ||
			borg_spell_fail(REALM_NATURE, 0, 2, 20) ||
			borg_racial(RACE_DWARF) ||
			borg_racial(RACE_NIBELUNG))
		{
			borg_note("# Checking for traps and doors.");

			borg_react("SELF:both", "SELF:both");

			when_detect_traps = borg_t;
			when_detect_doors = borg_t;

			return (TRUE);
		}
	}


	/* Hack -- find traps */
	if (do_trap && (!when_detect_traps || (borg_t - when_detect_traps >= 7)))
	{
		/* Check for traps */
		if (borg_read_scroll(SV_SCROLL_DETECT_TRAP) ||
			borg_use_staff(SV_STAFF_DETECT_TRAP) ||
			borg_zap_rod(SV_ROD_DETECT_TRAP))
		{
			borg_note("# Checking for traps.");

			borg_react("SELF:trap", "SELF:trap");

			when_detect_traps = borg_t;

			return (TRUE);
		}
	}


	/* Hack -- find doors */
	if (do_door && (!when_detect_doors || (borg_t - when_detect_doors >= 9)))
	{
		/* Check for doors */
		if (borg_read_scroll(SV_SCROLL_DETECT_DOOR) ||
			borg_use_staff(SV_STAFF_DETECT_DOOR) ||
			borg_zap_rod(SV_ROD_DETECT_DOOR))
		{
			borg_note("# Checking for doors.");

			borg_react("SELF:door", "SELF:door");

			when_detect_doors = borg_t;

			return (TRUE);
		}
	}


	/* Hack -- find walls */
	if (do_wall && (!when_detect_walls || (borg_t - when_detect_walls >= 15)))
	{
		/* Check for walls */
		if (borg_activate_artifact(ART_ELENDIL, FALSE) ||
			borg_read_scroll(SV_SCROLL_MAPPING) ||
			borg_use_staff(SV_STAFF_MAPPING) ||
			borg_zap_rod(SV_ROD_MAPPING) ||
			borg_spell(REALM_SORCERY, 1, 0) ||
			borg_spell(REALM_NATURE, 1, 2) ||
			borg_mindcr(MIND_PRECOGNIT, 20))
		{
			borg_note("# Checking for walls.");

			borg_react("SELF:wall", "SELF:wall");

			when_detect_walls = borg_t;

			return (TRUE);
		}
	}

	/* Hack -- find evil */
	if (do_evil && (!when_detect_evil || (borg_t - when_detect_evil >= 9)))
	{
		/* Check for traps */
		if (borg_spell_fail(REALM_NATURE, 0, 0, 20) ||
			borg_spell_fail(REALM_ARCANE, 0, 3, 20) ||
			borg_spell_fail(REALM_SORCERY, 0, 0, 20) ||
			borg_spell_fail(REALM_DEATH, 0, 2, 20) ||
			borg_spell_fail(REALM_DEATH, 0, 0, 20) ||
			borg_spell_fail(REALM_LIFE, 0, 0, 20) ||
			borg_spell_fail(REALM_DEATH, 0, 0, 20))
		{
			borg_note("# Checking for monsters.");

			borg_react("SELF:evil", "SELF:evil");

			when_detect_evil = borg_t;

			return (TRUE);
		}
	}


	/* Start */
	do_lite = FALSE;

	corners = 0;
	floors = 0;

	/* Scan diagonal neighbors */
	for (i = 4; i < 8; i++)
	{
		/* Get location */
		x = c_x + ddx_ddd[i];
		y = c_y + ddy_ddd[i];

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Get grid */
		mb_ptr = map_loc(x, y);

		/* Location must be known */
		if (!mb_ptr->feat) corners++;

		/* Location must not be a wall/door */
		if (borg_cave_wall_grid(mb_ptr)) corners++;

	}
	/* Add them up */
	if (corners <= 2) do_lite = TRUE;

	/* Hack */
	if (do_lite && (bp_ptr->cur_lite >= 2) && (randint0(100) < 90))
	{
		floors = 0;

		/* Scan the "local" grids (5x5) 2 same as torch grid */
		for (y = c_y - 2; y <= c_y + 2; y++)
		{
			/* Scan the "local" grids (5x5) */
			for (x = c_x - 2; x <= c_x + 2; x++)
			{
				if (!map_in_bounds(x, y)) continue;

				/* Get grid */
				mb_ptr = map_loc(x, y);

				/* Location must be a lit floor */
				if (mb_ptr->flags & MAP_SEEN) floors++;

				/* Location must not be glowing */
				if (mb_ptr->flags & MAP_GLOW) floors--;

				/* Location must not be a wall/door */
				if (borg_cave_wall_grid(mb_ptr)) floors--;

			}
		}
	}
	/* add them up */
	if (floors <= 11) do_lite = FALSE;

	/* Vampires need to be careful for Light */
	if (FLAG(bp_ptr, TR_HURT_LITE) && !FLAG(bp_ptr, TR_RES_LITE))
		do_lite = FALSE;

	/* Hack -- call lite */
	if (do_lite && (!when_call_lite || (borg_t - when_call_lite >= 7)))
	{
		/* Call light */
		if (borg_activate_artifact(ART_GALADRIEL, FALSE) ||
			borg_activate_artifact(ART_ELENDIL, FALSE) ||
			borg_zap_rod(SV_ROD_ILLUMINATION) ||
			borg_use_staff(SV_STAFF_LITE) ||
			borg_read_scroll(SV_SCROLL_LIGHT) ||
			borg_spell(REALM_ARCANE, 0, 5) ||
			borg_spell(REALM_CHAOS, 0, 2) ||
			borg_spell(REALM_NATURE, 0, 4) ||
			borg_spell(REALM_SORCERY, 0, 3) ||
			borg_spell(REALM_LIFE, 0, 4) ||
			borg_mutation(MUT1_ILLUMINE))
		{
			borg_note("# Illuminating the room");

			borg_react("SELF:lite", "SELF:lite");

			when_call_lite = borg_t;

			return (TRUE);
		}
	}

	/* What to do when the borg has no light */
	if (!bp_ptr->cur_lite)
	{
		/* Set to default */
		do_lite = FALSE;

		/* Check all the surrounding spots */
		for (x = c_x - 1; x <= c_x + 1; x++)
		{
			for (y = c_y - 1; y <= c_y + 1; y++)
			{
				/* Check if it is on the map */
				if (!map_in_bounds(x, y)) continue;

				/* Is it a dark spot */
				if (!(map_loc(x, y)->flags & MAP_GLOW))
				{
					/* Think about lighting up */
					do_lite = TRUE;

					/* Remember where */
					g_x = x;
					g_y = y;
				}
			}
		}

		if (do_lite)
		{
			/* Can the borg cast a light area? */
			if (borg_zap_rod(SV_ROD_ILLUMINATION) ||
				borg_use_staff(SV_STAFF_LITE) ||
				borg_read_scroll(SV_SCROLL_LIGHT) ||
				borg_spell(REALM_ARCANE, 0, 5) ||
				borg_spell(REALM_CHAOS, 0, 2) ||
				borg_spell(REALM_NATURE, 0, 4) ||
				borg_spell(REALM_SORCERY, 0, 3) ||
				borg_spell(REALM_LIFE, 0, 4) ||
				borg_mutation(MUT1_ILLUMINE))
			{
				/* making lite */
				borg_note("# Trying to light the whole dungeon");

				return (TRUE);
			}

			/* Can the borg cast a beam of light */
			if (borg_spell_okay(REALM_NATURE, 1, 4) ||
				borg_spell_okay(REALM_ARCANE, 2, 5) ||
				borg_equips_rod(SV_ROD_LITE) ||
				borg_equips_wand(SV_WAND_LITE))
			{	
				/* Release target */
				borg_keypress('*');
				borg_keypress(ESCAPE);

				/* Can the borg cast a beam of light */
				if (borg_spell(REALM_NATURE, 1, 4) ||
					borg_spell(REALM_ARCANE, 2, 5) ||
					borg_zap_rod(SV_ROD_LITE) ||
					borg_aim_wand(SV_WAND_LITE))
				{	
					/* making lite */
					borg_note("# Trying to light the whole dungeon");

					/* show me the way */
					borg_keypress(I2D(borg_extract_dir(c_x, c_y, g_x, g_y)));

					return (TRUE);
				}
			}
		}
	}

	/* Hack -- Wizard Lite */
	if (TRUE && (!when_wizard_lite || (borg_t - when_wizard_lite >= 1000)))
	{
		/* Wizard lite */
		if (borg_activate_artifact(ART_THRAIN, FALSE) ||
			borg_spell(REALM_ARCANE, 3, 7) ||
			borg_spell(REALM_SORCERY, 3, 3) ||
			borg_spell(REALM_NATURE, 3, 5))
		{
			borg_note("# Illuminating the dungeon");

			/* borg_react("SELF:wizard lite", "SELF:wizard lite"); */

			when_wizard_lite = borg_t;

			return (TRUE);
		}
	}


	/* Oops */
	return (FALSE);
}

bool borg_check_lite_only(void)
{
	int i, x, y;
	int corners, floors;

	map_block *mb_ptr;

	bool do_lite;

	/* Never in town */
	if (!bp_ptr->depth) return (FALSE);

	/* Never when blind or hallucinating */
	if (bp_ptr->status.blind || bp_ptr->status.image) return (FALSE);

	/* XXX XXX XXX Dark */

	/* Start */
	do_lite = FALSE;

	corners = 0;
	floors = 0;

	/* Scan diagonal neighbors */
	for (i = 4; i < 8; i++)
	{
		/* Get location */
		x = c_x + ddx_ddd[i];
		y = c_y + ddy_ddd[i];

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Get grid */
		mb_ptr = map_loc(x, y);

		/* Location must be known */
		if (!mb_ptr->feat) corners++;

		/* Location must not be a wall/door */
		if (borg_cave_wall_grid(mb_ptr)) corners++;

	}
	/* Add them up ..2 */
	if (corners <= 2) do_lite = TRUE;

	/* Hack */
	if (do_lite && (bp_ptr->cur_lite >= 2) && (randint0(100) < 90))
	{

		floors = 0;
		/* Scan the "local" grids (5x5) 2 same as torch grid */
		for (y = c_y - 2; y <= c_y + 2; y++)
		{
			/* Scan the "local" grids (5x5) */
			for (x = c_x - 2; x <= c_x + 2; x++)
			{
				if (!map_in_bounds(x, y)) continue;

				/* Get grid */
				mb_ptr = map_loc(x, y);

				/* Location must be a lit floor */
				if (mb_ptr->flags & MAP_SEEN) floors++;

				/* Location must not be glowing */
				if (mb_ptr->flags & MAP_GLOW) floors--;

				/* Location must not be a wall/door */
				if (borg_cave_wall_grid(mb_ptr)) floors--;

			}
		}
	}
	/* add them up */
	if (floors <= 11) do_lite = FALSE;

	/* Hack -- call lite */
	if (do_lite && (!when_call_lite || (borg_t - when_call_lite >= 7)))
	{
		/* Call light */
		if (borg_activate_artifact(ART_GALADRIEL, FALSE) ||
			borg_activate_artifact(ART_ELENDIL, FALSE) ||
			borg_zap_rod(SV_ROD_ILLUMINATION) ||
			borg_use_staff(SV_STAFF_LITE) ||
			borg_read_scroll(SV_SCROLL_LIGHT) ||
			borg_spell(REALM_ARCANE, 0, 5) ||
			borg_spell(REALM_CHAOS, 0, 2) ||
			borg_spell(REALM_NATURE, 0, 4) ||
			borg_spell(REALM_SORCERY, 0, 3) ||
			borg_spell(REALM_LIFE, 0, 4) ||
			borg_mutation(MUT1_ILLUMINE))
		{
			borg_note("# Illuminating the room prior to resting");

			borg_react("SELF:lite", "SELF:lite");

			when_call_lite = borg_t;

			/* dont rest. call light instead */
			return (TRUE);
		}
	}


	/* Hack -- Wizard Lite */
	if (TRUE && (!when_wizard_lite || (borg_t - when_wizard_lite >= 1000)))
	{
		/* Wizard lite */
		if (borg_activate_artifact(ART_THRAIN, FALSE) ||
			borg_spell(REALM_ARCANE, 3, 7) ||
			borg_spell(REALM_SORCERY, 3, 3) ||
			borg_spell(REALM_NATURE, 3, 5))
		{
			borg_note("# Illuminating the dungeon prior to resting");

			/* borg_react("SELF:wizard lite", "SELF:wizard lite"); */

			when_wizard_lite = borg_t;

			return (TRUE);
		}
	}


	/* nothing to light up.  OK to rest. */
	return (FALSE);
}


/*
 * Enchant armor with the lowest AC.  This routine doesn't use the spell when
 * the borg is in the dungeon because down there he has better things to do.
 */
static bool borg_enchant_to_a(void)
{
	int i, slot;

	/* What is the best item? */
	slot = borg_notice_enchant_ac();

	/* No suitable item */
	if (slot == -1) return (FALSE);

	/* Enchant it */
	if (borg_read_scroll(SV_SCROLL_STAR_ENCHANT_ARMOR) ||
		borg_read_scroll(SV_SCROLL_ENCHANT_ARMOR) ||
		(!bp_ptr->depth && borg_spell_fail(REALM_SORCERY, 3, 5, 40)))
	{
		/*
		 * Find out if the prompt is at Inven or Equip by checking if
		 * there is armour in the inventory.  If there is then the prompt
		 * is at Inven and has to be moved to Equip.
		 */
		for (i = 0; i < inven_num; i++)
		{
			list_item *l_ptr = &inventory[i];
	
			/* Is this item is enchantable? */
			if (l_ptr->tval >= TV_BOOTS && l_ptr->tval <= TV_DRAG_ARMOR)
			{
				/* Goto the equipment */
				borg_keypress('/');
				break;
			}
		}

		/* Tell the world */
		borg_note_fmt("# Enchanting %s (%c)",
					  equipment[slot].o_name, I2A(slot));

		/* Choose that item */
		borg_keypress(I2A(slot));

		/* Success */
		return (TRUE);
	}

	/* Nothing to do */
	return (FALSE);
}


/*
 * Enchant weapons to hit
 */
static bool borg_enchant_to_h(void)
{
	int i, slot;

	bool inven = FALSE;

	/* What is the best item? */
	slot = borg_notice_enchant_hit(&inven);

	/* No suitable item */
	if (slot == -1) return (FALSE);

	/* Enchant it */
	if (borg_read_scroll(SV_SCROLL_ENCHANT_WEAPON_TO_HIT))
	{
		if (inven)
		{
			/* Tell the world */
			borg_note_fmt("# Enchanting %s (%c)",
						  inventory[slot].o_name, I2A(slot));
		}
		else
		{
			/* Tell the world */
			borg_note_fmt("# Enchanting %s (%c)",
						  equipment[slot].o_name, I2A(slot));

			/*
			 * Find out if the prompt is at Inven or Equip by checking if
			 * there is a weapon or ammo in the inventory.  If there is
			 * then the prompt is at Inven and has to be moved to Equip.
			 */
			for (i = 0; i < inven_num; i++)
			{
				list_item *l_ptr = &inventory[i];

				/* Is this item is enchantable? */
				if (l_ptr->tval >= TV_SHOT && l_ptr->tval <= TV_SWORD)
				{
					/* Goto the equipment */
					borg_keypress('/');
					break;
				}
			}
		}

		/* Choose that item */
		borg_keypress(I2A(slot));

		/* Success */
		return (TRUE);
	}

	/* Nothing to do */
	return (FALSE);
}


/*
 * Enchant weapons to dam
 */
static bool borg_enchant_to_d(void)
{
	int i, slot;

	bool inven = FALSE;

	/* What is the best item? */
	slot = borg_notice_enchant_dam(&inven);

	/* No suitable item */
	if (slot == -1) return (FALSE);

	/* Enchant it */
	if (borg_read_scroll(SV_SCROLL_ENCHANT_WEAPON_TO_DAM))
	{
		if (inven)
		{
			/* Tell the world */
			borg_note_fmt("# Enchanting %s (%c)",
						  inventory[slot].o_name, I2A(slot));
		}
		else
		{
			/* Tell the world */
			borg_note_fmt("# Enchanting %s (%c)",
						  equipment[slot].o_name, I2A(slot));

			/*
			 * Find out if the prompt is at Inven or Equip by checking if
			 * there is a weapon or ammo in the inventory.  If there is
			 * then the prompt is at Inven and has to be moved to Equip.
			 */
			for (i = 0; i < inven_num; i++)
			{
				list_item *l_ptr = &inventory[i];

				/* Is this item is enchantable? */
				if (l_ptr->tval >= TV_SHOT && l_ptr->tval <= TV_SWORD)
				{
					/* Goto the equipment */
					borg_keypress('/');
					break;
				}
			}
		}

		/* Choose that item */
		borg_keypress(I2A(slot));

		/* Success */
		return (TRUE);
	}

	/* Nothing to do */
	return (FALSE);
}

/*
 * Enchant weapons to dam and to hit.
 * Target a wielded weapon if it has a bonus < 10
 * Otherwise target arrows.
 * Forget about enchanting arrows with the spell if its bonuses are high
 */
static bool borg_enchant_to_w(void)
{
	int i, slot, slot_d, slot_h;

	bool inven, inven_d, inven_h;
	bool scroll = borg_read_scroll_fail(SV_SCROLL_STAR_ENCHANT_WEAPON);


	/* Can we enchant at all */
	if (!scroll &&
		!borg_spell_fail(REALM_SORCERY, 3, 4, 40)) return (FALSE);

	/* What is the item with the lowest dam? */
	slot_d = borg_notice_enchant_dam(&inven_d);

	/* What is the item with the lowest hit? */
	slot_h = borg_notice_enchant_dam(&inven_h);

	/* Is the item with the lowest dam bonus in the inventory? */
	if (inven_d)
	{
		/* Is the item with the lowest hit bonus in the inventory? */
		if (inven_h)
		{
			/* Both hit and dam items are in the inventory */
			inven = TRUE;

			/* Which has the lower bonus? */
			if (inventory[slot_d].to_d < inventory[slot_h].to_h)
				slot = slot_d;
			else
				slot = slot_h;
		}
		/* Equipment goes first */
		else
		{
			inven = FALSE;
			slot = slot_h;
		}
	}
	/* The item with the lowest dam bonus is in the equipment */
	else
	{
		/* Target the equipment */
		inven = FALSE;

		/* Or maybe the hit bonus? */
		if (!inven_h &&
			equipment[slot_d].to_h < equipment[slot_h].to_d)
		{
			/* So it is the hit bonus */
			slot = slot_h;
		}
		else
		{
			/* The dam bonus is the lowest */
			slot = slot_d;
		}
	}

	/* No suitable item */
	if (slot == -1) return (FALSE);

	/* If you are using the spell */
	if (!scroll)
	{
		/* Don't bother enchanting arrows up to the max with the spell */
		if (inven &&
			inventory[slot].to_h > 10 &&
			inventory[slot].to_d > 15) return (FALSE);

		/* Don't bother enchanting equipped items all the way */
		if (!inven &&
			equipment[slot].to_h > 13 &&
			equipment[slot].to_d > 20) return (FALSE);
	}

	/* Enchant it */
	if (borg_read_scroll(SV_SCROLL_STAR_ENCHANT_WEAPON) ||
		borg_spell(REALM_SORCERY, 3, 4))
	{
		if (inven)
		{
			/* Tell the world */
			borg_note_fmt("# Enchanting %s (%c)",
						  inventory[slot].o_name, I2A(slot));
		}
		else
		{
			/* Tell the world */
			borg_note_fmt("# Enchanting %s (%c)",
						  equipment[slot].o_name, I2A(slot));

			/*
			 * Find out if the prompt is at Inven or Equip by checking if
			 * there is a weapon or ammo in the inventory.  If there is
			 * then the prompt is at Inven and has to be moved to Equip.
			 */
			for (i = 0; i < inven_num; i++)
			{
				list_item *l_ptr = &inventory[i];

				/* Is this item is enchantable? */
				if (l_ptr->tval >= TV_SHOT && l_ptr->tval <= TV_SWORD)
				{
					/* Goto the equipment */
					borg_keypress('/');
					break;
				}
			}
		}

		/* Choose that item */
		borg_keypress(I2A(slot));

		/* Success */
		return (TRUE);
	}

	/* Flow can't get here because of the availability check at the start */
	borg_oops("Marooned code was reached.");

	return (FALSE);
}


static bool borg_mundane(void)
{
	int i, slot = -1;
	list_item *l_ptr;

	if (!borg_read_scroll_fail(SV_SCROLL_MUNDANITY)) return (FALSE);

	/* Check the equipment */
	for (i = 0; i < equip_num; i++)
	{
		l_ptr = look_up_equip_slot(i);

		/* Not for empty slots */
		if (!l_ptr) continue;

		/* If there is a nasty curse */
		if (KN_FLAG(l_ptr, TR_NO_TELE) ||
			(KN_FLAG(l_ptr, TR_NO_MAGIC) && borg_class != CLASS_WARRIOR) ||
			KN_FLAG(l_ptr, TR_TY_CURSE))
		{
			/* Remember the target for the mundanity scroll */
			slot = i;
		}
	}

	/* I guess not */
	if (slot == -1) return (FALSE);

	if (borg_read_scroll(SV_SCROLL_MUNDANITY))
	{
		/* switch to equipment */
		borg_keypress('/');

		/* Kazam! */
		borg_keypress(I2A(slot));

		/* Getaway */
		return (TRUE);
	}

	/* Should be unreachable */
	return (FALSE);
}


/* Handle the use of a scroll of artifact creation */
static bool borg_enchant_artifact(void)
{
	int i, slot = -1;
	bool inven;
	list_item *l_ptr;

	slot = borg_notice_create_artifact(&inven);

	/* Do we have a winner? */
	if (slot == -1) return (FALSE);

	/* Tell the world */
	if (inven)
		borg_note_fmt("# Creating an artifact from %s (%c)",
					  inventory[slot].o_name, I2A(slot));
	else
		borg_note_fmt("# Creating an artifact from %s (%c)",
					  equipment[slot].o_name, I2A(slot));

	/* Read the scroll */
	(void)borg_read_scroll(SV_SCROLL_ARTIFACT);

	/* if the item is in the equipment */
	if (!inven)
	{
		/* Check if the scrolls heads towards the inventory */
		for (i = 0; i < inven_num; i++)
		{
			l_ptr = &inventory[i];
		
			/* Is this item is artifactable? */
			if (l_ptr->tval < TV_BOW ||
				l_ptr->tval > TV_DRAG_ARMOR) continue;

			/* switch to equipment */
			borg_keypress('/');

			break;
		}
	}

	/* Choose that item */
	borg_keypress(I2A(slot));

	/* Complete sequence */
	borg_keypresses(" Borg Artifact\r");

	/* Success */
	return (TRUE);
}


/* Find out if the borg wears a cursed item */
bool borg_wears_cursed(bool heavy)
{
	int i;
	bool result = FALSE;

	for (i = 0; i < equip_num; i++)
	{
		list_item *l_ptr = look_up_equip_slot(i);

		/* If this slot is not empty */
		if (!l_ptr) continue;

		/* And the item in this slot has a name */
		if (streq(l_ptr->o_name,"")) continue;

		/* And the name is cursed then the borg wears a cursed item */
		if (strstr(l_ptr->o_name, "cursed"))
		{
			result = TRUE;
			if (heavy)
			{
				l_ptr->kn_flags[2] |= TR2_HEAVY_CURSE;
			}
			else
			{
				l_ptr->kn_flags[2] |= TR2_CURSED;
			}
		}
	}

	return (result);
}


/*
 * Remove Curse
 */
static bool borg_decurse(void)
{
	/* Nothing to decurse */
	if (!borg_wearing_cursed) return (FALSE);

	/* remove the curse */
	if (borg_spell_fail(REALM_LIFE, 1, 0, 40) ||
		borg_use_staff_fail(SV_STAFF_REMOVE_CURSE) ||
		borg_read_scroll(SV_SCROLL_REMOVE_CURSE))
	{
		/* Shekockazol! */
		borg_wearing_cursed = FALSE;
		borg_heavy_curse = borg_wears_cursed(TRUE);
		return (TRUE);
	}

	/* Try *remove curse* if unlimited available */
	if (bp_ptr->able.star_remove_curse >= 1000)
	{
		/* pretend there is a heavy curse */
		borg_heavy_curse = TRUE;
	}

	/* Nothing to do */
	return (FALSE);
}

/*
 * Remove Heavy Curse
 */
static bool borg_star_decurse(void)
{
	/* Nothing to *decurse* */
	if (!borg_heavy_curse) return (FALSE);

	/* remove the curse */
	if (borg_spell_fail(REALM_LIFE, 2, 1, 40) ||
		borg_read_scroll(SV_SCROLL_STAR_REMOVE_CURSE))
	{
		/* Shekockazol! */
		borg_wearing_cursed = FALSE;
		borg_heavy_curse = FALSE;
		return (TRUE);
	}

	/* Nothing to do */
	return (FALSE);
}

/*
 * Enchant things
 */
bool borg_enchanting(void)
{
	/* Forbid blind/confused */
	if (bp_ptr->status.blind || bp_ptr->status.confused) return (FALSE);

	/* One time only */
	if (borg_decurse()) return (TRUE);
	if (borg_star_decurse()) return (TRUE);
	if (borg_enchant_artifact()) return (TRUE);
	if (borg_mundane()) return (TRUE);

	/* Prevent casting a spell over and over */
	if ((borg_t - borg_began > 150 && bp_ptr->depth) ||
		(borg_t - borg_began > 350 && !bp_ptr->depth)) return (FALSE);

	/* Maybe with spell */
	if (borg_enchant_to_a()) return (TRUE);
	if (borg_enchant_to_d()) return (TRUE);
	if (borg_enchant_to_h()) return (TRUE);
	if (borg_enchant_to_w()) return (TRUE);

	/* Nope */
	return (FALSE);
}


/* Recharge things.  Rods go first, then staffs and wands last. */
bool borg_recharging(void)
{
	int i, charge = -1;

	/* Forbid blind/confused */
	if (bp_ptr->status.blind || bp_ptr->status.confused) return (FALSE);

	/* Look for an item to recharge */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip empty /unaware items */
		if (!l_ptr->k_idx) continue;

		/* Wands or staffs with no charges can be charged */
		if ((l_ptr->tval == TV_WAND || l_ptr->tval == TV_STAFF) &&
			((borg_obj_known_p(l_ptr) && l_ptr->pval < 1) ||
			strstr(l_ptr->o_name, "{empty")))
		{
			/* Settle for this wand/staff. */
			charge = i;
		}

		/*
		 * Recharging rods is not a smart idea as it is a real pain if your 
		 * precious rod of recall is consumed by wild magic.
		 * So only allow a Rod of Healing to be recharged when the borg is
		 * low on HP and presumably will use the rod next turn.
		 */
		if (l_ptr->tval == TV_ROD && l_ptr->timeout &&
			k_info[l_ptr->k_idx].sval == SV_ROD_HEALING && bp_ptr->chp < 100)
		{
			/* Settle for this rod */
			charge = i;

			/* Rods go first so leave the loop */
			break;
		}
	}

	/* Don't try to recharge if there is nothing to recharge */
	if (charge == -1) return (FALSE);

	/* Attempt to recharge */
	if (borg_activate_artifact(ART_THINGOL, FALSE) ||
		borg_spell(REALM_ARCANE, 3, 0) ||
		borg_spell(REALM_CHAOS, 2, 2) ||
		borg_spell(REALM_SORCERY, 0, 7) ||
		borg_read_scroll(SV_SCROLL_RECHARGING))
	{
		/* Message */
		borg_note_fmt("Recharging %s", inventory[charge].o_name);

		/* Recharge the item */
		borg_keypress(I2A(charge));

		/* Success */
		return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * Sometimes the borg will want to destroy vaguely usefull items.
 * This procedure use those items instead of destroying them.
 */
static bool borg_consume(list_item *l_ptr)
{
	int sval;
	
	/* must have an item */
	if (!l_ptr) return (FALSE);

	/* get the sval */
	sval = k_info[l_ptr->k_idx].sval;

	/* Special destruction */
	switch (l_ptr->tval)
	{
		case TV_POTION:
		{
			/* Check the potion */
			if (bp_ptr->status.gorged ||
			    ((sval >= SV_POTION_SLOWNESS) &&
			     (sval <= SV_POTION_DEATH)))
			{
				/* probably bad to quaff this */
				return (FALSE);
			}

			/* Try quaffing the potion */
			if (borg_quaff_potion(sval)) return (TRUE);

			break;
		}

		case TV_SCROLL:
		{
			/* Check the scroll */
			switch (sval)
			{
				/* only good for scrolls without a target */
				case SV_SCROLL_REMOVE_CURSE:
				case SV_SCROLL_LIGHT:
				case SV_SCROLL_MAPPING:
				case SV_SCROLL_STAR_REMOVE_CURSE:
				case SV_SCROLL_DETECT_GOLD:
				case SV_SCROLL_DETECT_ITEM:
				case SV_SCROLL_DETECT_TRAP:
				case SV_SCROLL_DETECT_DOOR:
				case SV_SCROLL_DETECT_INVIS:
				case SV_SCROLL_SATISFY_HUNGER:
				case SV_SCROLL_BLESSING:
				case SV_SCROLL_HOLY_CHANT:
				case SV_SCROLL_HOLY_PRAYER:
				case SV_SCROLL_MONSTER_CONFUSION:
				case SV_SCROLL_PROTECTION_FROM_EVIL:
				case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
				case SV_SCROLL_DISPEL_UNDEAD:
				case SV_SCROLL_ACQUIREMENT:
				case SV_SCROLL_STAR_ACQUIREMENT:
				case SV_SCROLL_RUMOR:
				{
					/* Try reading the scroll */
					return (borg_read_scroll(sval));
				}
				case SV_SCROLL_STAR_IDENTIFY:
				case SV_SCROLL_IDENTIFY:
				{
					int i;

					/* Try to find something to id, backwards to get weapons */
					for (i = inven_num - 1; i >= 0; i--)
					{
						/* If this is an unid'd object */
						if (!borg_obj_known_p(&inventory[i]))
						{
							/* read the scroll on it */
							(void)borg_read_scroll(sval);
							borg_keypress(I2A(i));

							return (TRUE);
						}
					}
				}
			}

			break;
		}

		case TV_FOOD:
		{
			/* Check if the food is bad or the borg is full */
			if (bp_ptr->status.full ||
				bp_ptr->status.gorged ||
				sval < SV_FOOD_MIN_FOOD ||
				FLAG(bp_ptr, TR_CANT_EAT))
			{
				/* Probably not a good idea */
				return (FALSE);
			}

			/* Try eating the food */
			if (borg_eat_food(sval)) return (TRUE);;
		}

		case TV_LITE:
		{
			/* What is the light source of the borg? */
			list_item *q_ptr = look_up_equip_slot(EQUIP_LITE);

			/* If the item and the light source are nor the same type */
			if (!q_ptr || 
				k_info[q_ptr->k_idx].sval != k_info[l_ptr->k_idx].sval)
			{
				/* No refueling possible */
				return (FALSE);
			}

			/* If the item is an empty lantern */
			if (k_info[l_ptr->k_idx].sval == SV_LITE_LANTERN &&
				 l_ptr->timeout == 0)
			{
				/* No refueling possible */
				return (FALSE);
			}

			/* If the item is an artifact */
			if (KN_FLAG(l_ptr, TR_INSTA_ART))
			{
				/* No refueling possible */
				return (FALSE);
			}

			/* Then it can be used as fuel */
			borg_keypress('F');
			borg_keypress(I2A(look_up_index(l_ptr)));

			return (TRUE);
		}

		case TV_FLASK:
		{
			/* What is the light source of the borg? */
			list_item *q_ptr = look_up_equip_slot(EQUIP_LITE);

			/* Is that a lantern? */
			if (q_ptr && k_info[q_ptr->k_idx].sval == SV_LITE_LANTERN)
			{
				/* Fuel it with the flask */
				borg_keypress('F');
				borg_keypress(I2A(look_up_index(l_ptr)));

				return (TRUE);
			}
		}
	}


	/* Nope */
	return (FALSE);
}

/*
 * Should we *id* this item?
 */
bool borg_obj_star_id_able(list_item *l_ptr)
{
	/* Is there an object at all? */
	if (!l_ptr) return (FALSE);

	/* Demand that the item is identified */
	if (!borg_obj_known_p(l_ptr)) return (FALSE);
	
	/* Some non-ego items should be *id'ed too */
	if (l_ptr->tval == TV_SHIELD &&
	 	k_info[l_ptr->k_idx].sval == SV_DRAGON_SHIELD) return (TRUE);
	if (l_ptr->tval == TV_HELM &&
	 	k_info[l_ptr->k_idx].sval == SV_DRAGON_HELM) return (TRUE);
	if (l_ptr->tval == TV_CLOAK &&
	 	k_info[l_ptr->k_idx].sval == SV_SHADOW_CLOAK) return (TRUE);
	if (l_ptr->tval == TV_RING &&
	 	k_info[l_ptr->k_idx].sval == SV_RING_LORDLY) return (TRUE);

	/* not an ego object */
	if (!borg_obj_is_ego_art(l_ptr)) return (FALSE);

	/* Artifacts */
	if (KN_FLAG(l_ptr, TR_INSTA_ART)) return (TRUE);

	/* Weapons */
	if (streq(l_ptr->xtra_name, "(Holy Avenger)")) return (TRUE);
	if (streq(l_ptr->xtra_name, "(Defender)")) return (TRUE);
	if (streq(l_ptr->xtra_name, "(Blessed)")) return (TRUE);
	if (streq(l_ptr->xtra_name, "of Westernesse")) return (TRUE);
	if (streq(l_ptr->xtra_name, "of Slay Dragon")) return (TRUE);
	if (streq(l_ptr->xtra_name, "of *Slay* Dragon")) return (TRUE);
	if (streq(l_ptr->xtra_name, "(Chaotic)")) return (TRUE);
	if (streq(l_ptr->xtra_name, "of Slaying")) return (TRUE);
	if (streq(l_ptr->xtra_name, "(Vampiric)")) return (TRUE);
	if (streq(l_ptr->xtra_name, "(Trump Weapon)")) return (TRUE);
	if (streq(l_ptr->xtra_name, "(Pattern Weapon)")) return (TRUE);

	/* Bow */
	if (streq(l_ptr->xtra_name, "of Might")) return (TRUE);

	/* Armour */
	if (streq(l_ptr->xtra_name, "of Permanence")) return (TRUE);
	if (streq(l_ptr->xtra_name, "of Resistance")) return (TRUE);
	if (streq(l_ptr->xtra_name, "of Elvenkind")) return (TRUE);

	/* Hat */
	if (streq(l_ptr->xtra_name, "of the Magi")) return (TRUE);
	if (streq(l_ptr->xtra_name, "of Lordliness")) return (TRUE);
	if (streq(l_ptr->xtra_name, "of Seeing")) return (TRUE);

	/* Cloak */
	if (streq(l_ptr->xtra_name, "of Aman")) return (TRUE);

	/* Any object that reaches here has nothing interesting to *id* */
	return (FALSE);
}


static bool borg_heavy_sense(void)
{
	switch(borg_class)
	{
		case CLASS_WARRIOR:
		case CLASS_ROGUE:
		case CLASS_RANGER:
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR: return (TRUE);

		case CLASS_MAGE:
		case CLASS_PRIEST:
		case CLASS_WARRIOR_MAGE:
		case CLASS_MONK:
		case CLASS_MINDCRAFTER:
		case CLASS_HIGH_MAGE: return (FALSE);

		default:
		{
			borg_oops("Unknown heavy sense for unknown class.");
			return (FALSE);
		}
	}
}


/*
 * Destroy 'number' items
 */
static void borg_destroy_item(list_item *l_ptr, int slot, int number)
{
	char buf[4];

	/* Message */
	borg_note_fmt("# Destroying %s.", l_ptr->o_name);

	borg_keypress('0');

	/* Get string corresponding to number */
	(void)strnfmt(buf, 4, "%d", number);
	borg_keypresses(buf);

	/* Destroy that item */
	if (!KN_FLAG(l_ptr, TR_INSTA_ART))
	{
		/* Convert the object to money! */
		if (!borg_spell(REALM_SORCERY, 3, 6))
		{
			/* Try the mutation to gain money */
			if (!borg_mutation(MUT1_MIDAS_TCH))
			{
				/* Allright then, press the letter */
				borg_keypress('k');
			}
		}
	}
	else
	{
		/* worthless artifacts are dropped. */
		borg_keypress('d');

		/*
		 * Mark the spot that the object was dropped so that it will not be
		 * picked up again.  The wrap around is ok because the dropped object
		 * is not always right at the players feet, because of doors, stairs
		 * or other objects.  As one bad object can spawn multiple entries in
		 * this list, it is ok to start over.
		 */
		bad_obj_n = (bad_obj_n + 1) % 50;

		bad_obj_x[bad_obj_n] = c_x;
		bad_obj_y[bad_obj_n] = c_y;
		borg_note_fmt("# Crappy artifact at %d,%d", c_x, c_y);
	}

	borg_keypress(I2A(slot));
}

/*
 * Which items can you destroy without identifying
 * despite their unidentified status.
 */
static bool borg_destroy_unidentified(list_item *l_ptr)
{
	int sval;

	/* This proc is there for unid'd items */
	if (!l_ptr || borg_obj_known_p(l_ptr)) return (FALSE);

	/* If this item has been pseudo id'd with boring results */
	if (strstr(l_ptr->o_name, "{average") ||
		strstr(l_ptr->o_name, "{cursed") ||
		strstr(l_ptr->o_name, "{bad") ||
		strstr(l_ptr->o_name, "{broken") ||
		strstr(l_ptr->o_name, "{dubious") ||
		strstr(l_ptr->o_name, "{worthless")) return (TRUE);

	sval = k_info[l_ptr->k_idx].sval;

	switch (l_ptr->tval)
	{
		case TV_RING:
		{
			if (sval <= SV_RING_TELEPORTATION) return (TRUE);
			break;
		}
		case TV_AMULET:
		{
			if (sval <= SV_AMULET_TELEPORT) return (TRUE);
			break;
		}
		case TV_STAFF:
		{
			if (sval == SV_STAFF_DARKNESS &&
				!FLAG(bp_ptr, TR_HURT_LITE)) return (TRUE);
			if (sval >= SV_STAFF_SLOWNESS &&
				sval <= SV_STAFF_SUMMONING) return (TRUE);
			break;
		}
		case TV_WAND:
		{
			if (sval == SV_WAND_CLONE_MONSTER) return (TRUE);
			if (sval == SV_WAND_HASTE_MONSTER) return (TRUE);
			if (sval == SV_WAND_HEAL_MONSTER) return (TRUE);
			break;
		}
	}

	/* This item needs to be identified */
	return (FALSE);
}


/*
 * This is an effort to keep the borg from destroying useless items that
 * he should sell in order to get more money
 * The returned value should be less than 1000 so the borg can destroy it
 * anyway in real need.
 * You can use this procedure also to get the borg to collect items in the
 * dungeon that he shouldn't buy, like a potion of restore foo when he is
 * not drained.
 * It won't work too well on items that you want the borg to collect for his
 * home because if the borg meets a shop before the home he will sell your
 * precious item.
 */
static s32b borg_values_money(list_item *l_ptr)
{
	int tval = l_ptr->tval;
	int sval = k_info[l_ptr->k_idx].sval;

	/* Always try to collect fancy books */
	if (tval >= TV_BOOKS_MIN && tval <= TV_BOOKS_MAX && tval != TV_ARCANE_BOOK)
	{
		/* Dungeon books */
		if (sval == 3) return (800);
		if (sval == 2) return (700);
	}

	/* Reward getting stat restore potions when not needed */
	if (tval == TV_POTION &&
		sval >= SV_POTION_RES_STR &&
		sval <= SV_POTION_RES_CON &&
		!bp_ptr->status.fixstat[sval - SV_POTION_RES_STR]) return (100);

	/* Less valuable items */
	if (borg_gold > 100000) return (0);

	/* The deepest Arcane book is worth 2000 or so */
	if (tval >= TV_ARCANE_BOOK && sval == 3) return (400);

	/* Less valuable items */
	if (borg_gold > 10000) return (0);

	/* The second book is worth maybe 500 gold */
	if (tval >= TV_BOOKS_MIN && tval <= TV_BOOKS_MAX && sval > 0) return (200);

	/* Not so valuable items */
	if (borg_gold > 1000) return (0);

	/* The first book is worth about 50 gold */
	if (tval >= TV_BOOKS_MIN && tval <= TV_BOOKS_MAX) return (50);

	/* Lanterns are worth about 100 and stack when empty */
	if (tval == TV_LITE && sval == SV_LITE_LANTERN && !l_ptr->timeout)
		return (100);

	/* Collect spikes if there is really little gold */
	if (tval == TV_SPIKE) return (l_ptr->number);

	/* Truly worthless */
	return (0);
}


/*
 * This proc may or may not destroy one item, depending on must_destroy.
 * If the value loss is greater than 1000 then the borg will return home.
 * So if you want the borg to keep items make sure they are valued > 1000.
 * If the borg is slowed because of his weight then the borg will attempt
 * to destroy enough items to fix that.
 */
static bool borg_destroy_aux(bool must_destroy)
{
	int i, b_i = -1;
	int my_encumber, extra, number = 1;
	bool destroy_weight;
	s16b b_w = 0;
	s32b value = -1, b_v = 1000L, my_power, my_home_power;
	list_item *l_ptr;

	/* Get the starting power and encumberment */
	my_power = borg_power();
	my_home_power = borg_power_home();
	my_encumber = bp_ptr->encumber;

	/* if the carry capacity is used for more than 120% */
	destroy_weight = 2 * bp_ptr->weight / adj_str_wgt[my_stat_ind[A_STR]] >= 120;

	/* How much extra weight can the borg have before being slowed */
	extra = (adj_str_wgt[my_stat_ind[A_STR]] * 100) / 10;

	/* Scan for junk */
	for (i = 0; i < inven_num; i++)
	{
		l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* unknown? */
		if (borg_destroy_unidentified(l_ptr))
		{
			/* Some unknown items are valueless */
			value = 0;
		}
		else
		{
			/* Skip "good" unknown items (unless "icky") */
			if (!must_destroy && !destroy_weight &&
				!borg_obj_known_p(l_ptr) &&
				!borg_item_icky(l_ptr)) continue;

			/* Skip items that need to be *id*'d */
			if (strstr(l_ptr->o_name, "{special") ||
				strstr(l_ptr->o_name, "{terrible") ||
				(!borg_obj_known_full(l_ptr) &&
				borg_obj_star_id_able(l_ptr))) continue;

			if (!destroy_weight)
			{
				/* Pretend the whole pile is gone */
				l_ptr->treat_as = TREAT_AS_GONE;
				number = l_ptr->number;
			}
			else
			{
				/* Pretend the pile is one smaller */
				l_ptr->treat_as = TREAT_AS_LESS;
				number = 1;
			}

			/* Calculate the value of this item */
			value = my_power - borg_power();

			/* Useless for now.  Maybe take it home? */
			if (!value)
			{
				/* Find out the difference when this item goes home */
				value = borg_power_home() - my_home_power;

				/* If the home value decreases then nullify value */
				if (value < 0) value = 0;
			}
	
			/* Restore item */
			l_ptr->treat_as = TREAT_AS_NORM;

			/* If the borg is overweight */
			if (destroy_weight)
			{
				/* Void the punishment for the borg dropping speed */
				value += MIN(my_encumber - extra, l_ptr->weight * number) * 500L;
			}
		}

		/* Keeps some items for their gold value */
		if (value == 0) value = borg_values_money(l_ptr);

		/* if the borg wants to destroy junk then allow items with value = 0 */
		if (!must_destroy && !destroy_weight && value != 0) continue;

		/* Ignore "bad" swaps */
		if (value > b_v) continue;

		/* Hehe, someone can not count */
		if (value < 0) borg_oops_fmt("Destroying %s with value = %d ???", l_ptr->o_name, value);

		/* do not allow too large values because that item is too interesting */
		if (value > 1000)
		{
			/* If the borg was destroying for room in the inv but found nothing */
			if (must_destroy)
			{
				/* Go back to town to fix the inv */
				borg_note("# Going to town:  Full inventory.");
				goal_rising = TRUE;
			}
			else
			{
				/* Allow speed loss for neat items */
				continue;
			}
		}

		/* If the value is the same, take the one with the greater weight */
		if ((value == b_v) && (l_ptr->weight <= b_w)) continue;

		/* Maintain the "best" */
		b_i = i;
		b_v = value;
		b_w = l_ptr->weight;
	}

	/* Nothing to destroy */
	if (b_i < 0) return (FALSE);

	/* Reassign the item */
	l_ptr = &inventory[b_i];

	/* Attempt to consume it */
	if (borg_consume(l_ptr)) return (TRUE);

	/* Destroy the item */
	if (destroy_weight)
	{
		/* Destroy just one item */
		borg_destroy_item(l_ptr, b_i, 1);
	}
	else
	{
		/* Destroy all the items */
		borg_destroy_item(l_ptr, b_i, l_ptr->number);
	}

	return (TRUE);
}


/*
 * Catchall procedure for destroying items.  The borg will try to destroy an
 * item after he has picked up something.  If the inventory is full he must
 * make an empty slot, Otherwise he will just rid himself of low value items.
 */
bool borg_destroy(void)
{
	/* No destroying if even slightly dangerous */
	if (borg_danger(c_x, c_y, 1, TRUE) > bp_ptr->chp / 20) return (FALSE);

	/* Destroy items when the borg needs the space */
	if (inven_num == INVEN_PACK) return (borg_destroy_aux(TRUE));;

	/* Check if the borg carries junk */
	if (borg_do_destroy)
	{
		/* Until the next inv change */
		borg_do_destroy = FALSE;

		/* Try to destroy */
		return (borg_destroy_aux(FALSE));
	}

	/* No reason to destroy anything */
	return (FALSE);
}


/*
 * Identify items if possible
 *
 * Note that "borg_parse()" will "cancel" the identification if it
 * detects a "You failed..." message.  This is VERY important!!!
 * Otherwise the "identify" might induce bizarre actions by sending
 * the "index" of an item as a command.
 *
 * Hack -- recover from mind blanking by re-identifying the equipment.
 *
 * We instantly identify items known to be "good" (or "terrible").
 *
 * We identify most un-aware items as soon as possible.
 *
 * We identify most un-known items as soon as possible.
 *
 * We play games with items that get "feelings" to try and wait for
 * "sensing" to take place if possible.
 *
 * XXX XXX XXX Make sure not to sell "non-aware" items, unless
 * we are really sure we want to lose them.  For example, we should
 * wait for feelings on (non-icky) wearable items or else make sure
 * that we identify them before we try and sell them.
 *
 * Mega-Hack -- the whole "sometimes identify things" code is a total
 * hack.  Slightly less bizarre would be some form of "occasionally,
 * pick a random item and identify it if necessary", which might lower
 * the preference for identifying items that appear early in the pack.
 * Also, preventing inventory motion would allow proper time-stamping.
 */
static bool borg_test_stuff(void)
{
	int i, b_i = -1;
	s32b v, b_v = -1;

	list_item *l_ptr;

	bool inven = FALSE;

	/* Is there a way to identify things? */
	if (!borg_equips_rod_fail(SV_ROD_IDENTIFY) &&
		!borg_spell_legal_fail(REALM_ARCANE, 3, 2, 60) &&
		!borg_spell_legal_fail(REALM_SORCERY, 1, 1, 60) &&
		!borg_mindcr_legal_fail(MIND_PSYCHOMETRY, 25, 60) &&
		!borg_equips_staff_fail(SV_STAFF_IDENTIFY) &&
		!borg_read_scroll_fail(SV_SCROLL_IDENTIFY)) return (FALSE);

	/* Look for an item to identify (equipment) */
	for (i = 0; i < equip_num; i++)
	{
		l_ptr = look_up_equip_slot(i);

		/* Skip empty / unaware items */
		if (!l_ptr) continue;

		if (borg_obj_known_p(l_ptr)) continue;

		/* Track it */
		b_i = i;

		break;
	}

	/* Only bother with the inventory if you found nothing in the equipment */
	if (b_i < 0)
	{
		/* Look for an ego or artifact item to identify (inventory) */
		for (i = 0; i < inven_num; i++)
		{
			l_ptr = &inventory[i];

			if (borg_obj_known_p(l_ptr)) continue;

			/* Assume nothing */
			v = -1;

			/* With unlimited identify the borg should identify everything */
			if (bp_ptr->able.id >= 100) v = 1;

			/* Ignore items that are very likely worth nothing */
			if (borg_item_icky(l_ptr)) continue;

			/* Identify "good" items for a borg with light pseudo id */
			if (strstr(l_ptr->o_name, "{good") &&
				!borg_heavy_sense())
			{
				v = 10000L;
			}
			else if (strstr(l_ptr->o_name, "{good") &&
				bp_ptr->able.id > 10)
			{
				v = 1000L;
			}
			else if (strstr(l_ptr->o_name, "{excellent")) v = 20000L;
			else if (strstr(l_ptr->o_name, "{special")) v = 50000L;
			else if (strstr(l_ptr->o_name, "{terrible")) v = 50000L;

			/* Hack -- reward "unaware" items */
			if (!l_ptr->k_idx)
			{
				/* Analyze the type */
				switch (l_ptr->tval)
				{
					case TV_RING:
					case TV_AMULET:
					{
						/* Hack -- reward depth */
						v += (bp_ptr->max_depth * 5000L);

						break;
					}

					case TV_ROD:
					{
						/* Hack -- reward depth */
						v += (bp_ptr->max_depth * 3000L);

						break;
					}

					case TV_WAND:
					case TV_STAFF:
					{
						/* Hack -- reward depth */
						v += (bp_ptr->max_depth * 2000L);

						break;
					}

					case TV_POTION:
					case TV_SCROLL:
					{
						/* Hack -- reward depth */
						v += (bp_ptr->max_depth * 500L);

						break;
					}

					case TV_FOOD:
					{
						/* Hack -- reward depth */
						v += (bp_ptr->max_depth * 10L);

						break;
					}
				}
			}

			/* Track the best */
			if (v <= b_v) continue;

			/* Track it */
			b_i = i;
			b_v = v;

			inven = TRUE;
		}
	}

	/* Found something */
	if (b_i >= 0)
	{
		/* Use a Spell/Prayer/Rod/Staff/Scroll of Identify */
		if (borg_activate_artifact(ART_ERIRIL, FALSE) ||
			borg_zap_rod(SV_ROD_IDENTIFY) ||
			borg_spell(REALM_ARCANE, 3, 2) ||
			borg_spell(REALM_SORCERY, 1, 1) ||
			borg_mindcr(MIND_PSYCHOMETRY, 25) ||
			borg_use_staff(SV_STAFF_IDENTIFY) ||
			borg_read_scroll(SV_SCROLL_IDENTIFY))
		{
			if (inven)
			{
				l_ptr = &inventory[b_i];
			}
			else
			{
				l_ptr = &equipment[b_i];

				/* Switch to equipment but not in case you go there immediately */
				for (i = 0; i < inven_num; i++)
				{
					if (!borg_obj_known_p(&inventory[i]))
					{
						borg_keypress('/');
						break;
					}
				}
			}

			/* Log -- may be cancelled */
			borg_note_fmt("# Identifying %s.", l_ptr->o_name);

			/* Select the item */
			borg_keypress(I2A(b_i));

			borg_keypress(ESCAPE);

			/* HACK need to recheck stats if we id something on us. */
			for (i = 0; i < 6; i++)
			{
				my_need_stat_check[i] = TRUE;
				my_stat_max[i] = 0;
			}

			/* Success */
			return (TRUE);
		}
	}

	/* Nothing to do */
	return (FALSE);
}


/*
 * The basic method of *id*ing is that the first *id*-worthy item is *id*ed
 * There are not so many items that have to be *id*ed thanks to
 * borg_item_star_id_able.  Also, if there is a *id*-spell available but not
 * an id-spell (or rod) then all identifying is done with *id*.  This way the
 * borg does not have to worry about id-ing so much.
 */
static bool borg_test_stuff_star(void)
{
	int i, b_i = -1;

	list_item *l_ptr;

	/* Do we have the ability? */
	if (!borg_spell_legal_fail(REALM_SORCERY, 1, 7, 60) &&
		!borg_spell_legal_fail(REALM_NATURE, 2, 5, 60) &&
		!borg_spell_legal_fail(REALM_DEATH, 3, 2, 60) &&
		!borg_spell_legal_fail(REALM_TRUMP, 3, 1, 60) &&
		!borg_spell_legal_fail(REALM_LIFE, 3, 5, 60) &&
		!borg_read_scroll_fail(SV_SCROLL_STAR_IDENTIFY)) return (FALSE);

	/* Look for an item to identify (equipment) */
	for (i = 0; i < equip_num + inven_num; i++)
	{
		if (i >= equip_num)
		{
			l_ptr = &inventory[i - equip_num];
		}
		else
		{
			l_ptr = look_up_equip_slot(i);

			/* Ignore empty slots */
			if (!l_ptr) continue;
		}

		/* Ignore items that were *id'd* before */
		if (borg_obj_known_full(l_ptr)) continue;

		/*
		 * Accept unknown items if there are unlimited *id*s
		 * Ignore items that are known and have no hidden flags.
		 */
		if ((bp_ptr->able.star_id < 100 ||
			borg_obj_known_p(l_ptr)) &&
			!borg_obj_star_id_able(l_ptr))
			continue;

		/* Track it */
		b_i = i;

		break;
	}

	/* Found something */
	if (b_i >= 0)
	{
		if (borg_spell(REALM_SORCERY, 1, 7) ||
			borg_spell(REALM_NATURE, 2, 5) ||
			borg_spell(REALM_DEATH, 3, 2) ||
			borg_spell(REALM_TRUMP, 3, 1) ||
			borg_spell(REALM_LIFE, 3, 5) ||
			borg_read_scroll(SV_SCROLL_STAR_IDENTIFY))
		{
			if (b_i >= equip_num)
			{
				/* Adapt to the inventory */
				b_i -= equip_num;

				l_ptr = &inventory[b_i];
			}
			else
			{
				l_ptr = &equipment[b_i];

				/* Switch to equipment but not in case you go there immediately */
				for (i = 0; i < inven_num; i++)
				{
					if (!borg_obj_known_full(&inventory[i]))
					{
						borg_keypress('/');
						break;
					}
				}
			}

			/* Log -- may be cancelled */
			borg_note_fmt("# *IDENTIFY*ing %s.", l_ptr->o_name);

			/* Select the item */
			borg_keypress(I2A(b_i));

			/* press enter a few time (get rid of display) */
			borg_keypress('\r');
			borg_keypress('\r');
			borg_keypress('\r');
			borg_keypress('\r');
			borg_keypress(ESCAPE);

			/* HACK need to recheck stats if we id something on us. */
			for (i = 0; i < 6; i++)
			{
				my_need_stat_check[i] = TRUE;
				my_stat_max[i] = 0;
			}

			/* Success */
			return (TRUE);
		}
	}

	/* Nothing to do */
	return (FALSE);
}


/* 
 * Use the Mindcrafter Psychometry power to pseudo-id items
 * or determine if it is smart to wait it out
 */
static bool borg_test_stuff_pseudo(void)
{
	int i, b_i = -1;

	list_item *l_ptr;

	/* Can the borg cast the spell? */
	if (!borg_mindcr_legal_fail(MIND_PSYCHOMETRY, 15, 60) ||
		bp_ptr->lev > 24)
	{
		/* Is it heavy pseudo id and likely to kick in by itself? */
		if (!borg_heavy_sense() ||
			borg_calc_pseudo() > 100) return (FALSE);
	}

	/* Look for an item to pseudo identify */
	for (i = 0; i < equip_num + inven_num; i++)
	{
		if (i >= equip_num)
		{
			l_ptr = &inventory[i - equip_num];
		}
		else
		{
			l_ptr = look_up_equip_slot(i);

			/* Ignore empty slots */
			if (!l_ptr) continue;
		}

		/* Ignore items that were id'd before */
		if (borg_obj_known_p(l_ptr)) continue;

		/* Item has to be weapon, armor or ammo */
		if (l_ptr->tval < TV_SHOT || l_ptr->tval > TV_DRAG_ARMOR) continue;

		/* Item does not have a pseudo-id already (or comment) */
		if (strstr(l_ptr->o_name, "{")) continue;

		/* Track it */
		b_i = i;

		break;
	}

	/* Found nothing */
	if (b_i < 0) return (FALSE);

	/* If you can cast the spell */
	if (borg_mindcr(MIND_PSYCHOMETRY, 15))
	{
		if (b_i >= equip_num)
		{
			/* Adapt to the inventory */
			b_i -= equip_num;

			l_ptr = &inventory[b_i];
		}
		else
		{
			l_ptr = &equipment[b_i];

			/* Switch to equipment but not in case you go there immediately */
			for (i = 0; i < inven_num; i++)
			{
				if (inventory[i].tval >= TV_SHOT &&
					inventory[i].tval <= TV_DRAG_ARMOR &&
					!borg_obj_known_p(&inventory[i]))
				{
					borg_keypress('/');
					break;
				}
			}
		}

		/* Log -- may be cancelled */
		borg_note_fmt("# pseudo identifying %s.", l_ptr->o_name);

		/* Select the item */
		borg_keypress(I2A(b_i));

		/* press enter a few time (get rid of display) */
		borg_keypress(ESCAPE);

		/* Success */
		return (TRUE);
	}

	/* Make a note */
	borg_note("# Waiting for pseudo id to kick in");

	/* Wait a bit for the pseudo-id to kick in */
	borg_keypress('0');
	borg_keypress('9');
	borg_keypress('R');

	/* Ready */
	return (TRUE);
}


/* Catch all function that does the common part for the id routines */
bool borg_id_meta(void)
{
	/* No ID if in danger */
	if (borg_danger(c_x, c_y, 1, TRUE) > 1) return (FALSE);

	/* don't ID stuff when you can't recover spent spell point immediately */
	if (bp_ptr->csp < 50 &&	!borg_check_rest()) return (FALSE);

	/* Identify unknown things */
	if (borg_test_stuff()) return (TRUE);

	/* *Id* unknown things */
	if (borg_test_stuff_star()) return (TRUE);

	/* Pseudo identify unknown things */
	if (borg_test_stuff_pseudo()) return (TRUE);

	/* nothing */
	return (FALSE);
}


/*
 * Wear useful equipment.
 *
 * Look through the inventory for equipment that is better than
 * the current equipment, and wear it, in an optimal order.
 *
 * Basically, we evaluate the world both with the current set of
 * equipment, and in the alternate world in which various items
 * are used instead of the items they would replace, and we take
 * one step towards the world in which we have the most "power".
 */
bool borg_wear_stuff(void)
{
	int slot, b_slot = -1;
	int ii, i, b_i = -1;
	int danger, d;

	bool ring_repeat = FALSE;

	s32b p, b_p = borg_power();

	list_item *l_ptr;

	/* Scan inventory */
	for (i = 0; i < inven_num; i++)
	{
		l_ptr = &inventory[i];

		ii = i;

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Require "known" (or average, good, etc) */
		if (!borg_obj_known_p(l_ptr) &&
			!strstr(l_ptr->o_name, "{average") &&
			!strstr(l_ptr->o_name, "{good") &&
			!strstr(l_ptr->o_name, "{excellent") &&
			!strstr(l_ptr->o_name, "{special")) continue;

		/* apw do not wear not *id* artifacts */
		if (!borg_obj_known_full(l_ptr) &&
			borg_obj_star_id_able(l_ptr)) continue;

		/* Where does it go */
		slot = borg_wield_slot(l_ptr);

		/* Cannot wear this item */
		if (slot < 0) continue;

		/* Process non-rings */
		if (slot == EQUIP_LEFT)
		{
			/* if this is the second time you see this ring */
			if (ring_repeat)
			{
				/* use the other ring slot */
				slot = EQUIP_RIGHT;
			}
			/* So it is the first time this ring is valued */
			else
			{
				/* Pull back the counter to force a repetition of this item */
				i--;
			}

			/* Flip value */
			ring_repeat = !ring_repeat;
		}

		/* skip it if it this slot has not been decursed */
		if (KN_FLAG(&equipment[slot], TR_CURSED) ||
			KN_FLAG(&equipment[slot], TR_HEAVY_CURSE) ) continue;

		/* Obtain danger */
		danger = borg_danger(c_x, c_y, 1, TRUE);

		/* Swap items */
		equipment[slot].treat_as = TREAT_AS_SWAP;
		l_ptr->treat_as = TREAT_AS_LESS;

		/* Evaluate the inventory */
		p = borg_power();

		/* Evaluate local danger */
		d = borg_danger(c_x, c_y, 1, TRUE);

		/* Restore items */
		equipment[slot].treat_as = TREAT_AS_NORM;
		l_ptr->treat_as = TREAT_AS_NORM;

		/* Ignore "bad" swaps */
		if (p <= b_p) continue;

		/* Ignore if more dangerous */
		if (danger < d) continue;

		/* Maintain the "best" */
		b_i = ii;
		b_p = p;
		b_slot = slot;
	}

	/* No item so give up */
	if (b_i < 0) return (FALSE);

	/* Get the item */
	l_ptr = &inventory[b_i];

	/* Log */
	borg_note_fmt("# Wearing %s. (%c)", l_ptr->o_name, b_i);

	/* Wear it */
	borg_keypress('w');
	borg_keypress(I2A(b_i));

	/* Check for two rings */
	if ((b_slot == EQUIP_LEFT || b_slot == EQUIP_RIGHT) &&
		equipment[EQUIP_LEFT].number && equipment[EQUIP_RIGHT].number)
	{
		/* On right hand? */
		if (b_slot == EQUIP_RIGHT)
		{
			borg_keypress('y');
		}
		else
		{
			borg_keypress('n');
		}
	}

	/* Is the item cursed? */
	if ((KN_FLAG(l_ptr, TR_CURSED) || KN_FLAG(l_ptr, TR_HEAVY_CURSE)) &&
		confirm_wear)
	{
		/* Handle wearing cursed items correctly */
		borg_keypress('y');
	}

	/* Okidoki */
	return (TRUE);
}


/*
 * Take off equipment that has become useless
 * This can happen through mutations, too much acid.
 */
bool borg_unwear_stuff(void)
{
	int slot, b_slot = -1;
	int p, b_p, d, b_d;
	list_item *l_ptr;
	
	/* Get the original power */
	b_p = borg_power();

	/* Get the original danger */
	b_d = borg_danger(c_x, c_y, 1, TRUE);

	/* Loop through the equipment */
	for (slot = 0; slot < equip_num; slot++)
	{
		/* Get the object from the current slot */
		l_ptr = &equipment[slot];

		/* Skip empty slots */
		if (!l_ptr->k_idx) continue;

		/* skip it if it has not been decursed */
		if (KN_FLAG(l_ptr, TR_CURSED) ||
			KN_FLAG(l_ptr, TR_HEAVY_CURSE)) continue;

		/* Pretend it is not there */
		l_ptr->treat_as = TREAT_AS_SWAP;

		/* Calculate new power */
		p = borg_power();

		/* Calculate the new danger */
		d = borg_danger(c_x, c_y, 1, TRUE);

		/* Stop pretending */
		l_ptr->treat_as = TREAT_AS_NORM;

		/* Is the borg better off without? */
		if (p <= b_p) continue;

		/* Is it more dangerous now */
		if (d > b_d) continue;

		/* Track this item */
		b_slot = slot;
		b_p = p;
	}

	/* All the equipment is fine */
	if (b_slot == -1) return (FALSE);

	/* Say you take it off */
	borg_note_fmt("# Taking off a %s (%c)", equipment[b_slot].o_name, I2A(b_slot));

	/* Take it off */
	borg_keypress('t');
	borg_keypress(I2A(b_slot));

	/* Success */
	return (TRUE);
}


/* 
 * Prevent spells that appear in two realms to be learned in both.
 * This is to make more different spells available at low levels.
 * Only when the borg loses all his books containing the spell that he did learn
 * he will learn the second spell.
 *
 * Light area is in 5 realms.
 * Detect trap/door is in 4 realms.
 * Detect monster is in 3 realms.
 * Phase door is in 3 realms.
 * Cure light wounds is in 3 realms.
 * Detect evil is in 2 realms.
 * Trap/door destruction is in 2 realms.
 */
static void borg_spell_prevent_learn(void)
{
	/* Find out which realms the borg has. */
	int r_life = (borg_has_realm(REALM_LIFE)) ? 1 : 0,
		r_sorcery = (borg_has_realm(REALM_SORCERY)) ? 1 : 0,
		r_nature = (borg_has_realm(REALM_NATURE)) ? 1 : 0,
		r_chaos = (borg_has_realm(REALM_CHAOS)) ? 1 : 0,
		r_death = (borg_has_realm(REALM_DEATH)) ? 1 : 0,
		r_trump = (borg_has_realm(REALM_TRUMP)) ? 1 : 0,
		r_arcane = (borg_has_realm(REALM_ARCANE)) ? 1 : 0;


	/* Does the borg have two realms containing light? */
	if (r_life + r_sorcery + r_nature + r_chaos + r_arcane == 2)
	{
		/* Can the borg cast a light already? */
		if (borg_spell_legal(REALM_LIFE, 0, 4) ||
			borg_spell_legal(REALM_SORCERY, 0, 3) ||
			borg_spell_legal(REALM_NATURE, 0, 4) ||
			borg_spell_legal(REALM_CHAOS, 0, 2) ||
			borg_spell_legal(REALM_ARCANE, 0, 5))
		{
			/* Prevent the learning of the second light spell */
			if (borg_magics[REALM_LIFE][0][4].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_LIFE][0][4].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_SORCERY][0][3].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_SORCERY][0][3].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_NATURE][0][4].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_NATURE][0][4].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_CHAOS][0][2].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_CHAOS][0][2].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_ARCANE][0][5].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_ARCANE][0][5].status =  BORG_MAGIC_HIGH;
		}
	}

	/* Does the borg have two realms containing detect trap/door? */
	if (r_life + r_sorcery + r_nature + r_arcane == 2)
	{
		/* Can the borg detect trap/door already? */
		if (borg_spell_legal(REALM_LIFE, 0, 5) ||
			borg_spell_legal(REALM_SORCERY, 0, 2) ||
			borg_spell_legal(REALM_NATURE, 0, 2) ||
			borg_spell_legal(REALM_ARCANE, 1, 0))
		{
			/* Prevent the learning of the second detect trap/door spell */
			if (borg_magics[REALM_LIFE][0][5].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_LIFE][0][5].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_SORCERY][0][2].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_SORCERY][0][2].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_NATURE][0][2].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_NATURE][0][2].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_ARCANE][1][0].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_ARCANE][1][0].status =  BORG_MAGIC_HIGH;
		}
	}

	/* Does the borg have two realms containing detect monster? */
	if (r_sorcery + r_nature + r_arcane == 2)
	{
		/* Can the borg cast a detect monster? */
		if (borg_spell_legal(REALM_SORCERY, 0, 0) ||
			borg_spell_legal(REALM_NATURE, 0, 0) ||
			borg_spell_legal(REALM_ARCANE, 0, 3))
		{
			/* Prevent the learning of the second detect monster */
			if (borg_magics[REALM_SORCERY][0][0].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_SORCERY][0][0].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_NATURE][0][0].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_NATURE][0][0].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_ARCANE][0][3].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_ARCANE][0][3].status =  BORG_MAGIC_HIGH;
		}
	}

	/* Does the borg have two realms containing phase door? */
	if (r_sorcery + r_trump + r_arcane == 2)
	{
		/* Can the borg phase door already? */
		if (borg_spell_legal(REALM_SORCERY, 0, 1) ||
			borg_spell_legal(REALM_TRUMP, 0, 0) ||
			borg_spell_legal(REALM_ARCANE, 0, 4))
		{
			/* Prevent the learning of the second phase door spell */
			if (borg_magics[REALM_SORCERY][0][1].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_SORCERY][0][1].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_TRUMP][0][0].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_TRUMP][0][0].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_ARCANE][0][4].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_ARCANE][0][4].status =  BORG_MAGIC_HIGH;
		}
	}

	/* Does the borg have two realms containing cure light wounds? */
	if (r_life + r_nature + r_arcane == 2)
	{
		/* Can the borg cure light wounds already? */
		if (borg_spell_legal(REALM_LIFE, 0, 1) ||
			borg_spell_legal(REALM_NATURE, 0, 1) ||
			borg_spell_legal(REALM_ARCANE, 0, 7))
		{
			/* Prevent the learning of the second cure light wounds spell */
			if (borg_magics[REALM_LIFE][0][1].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_LIFE][0][1].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_NATURE][0][1].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_NATURE][0][1].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_ARCANE][0][7].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_ARCANE][0][7].status =  BORG_MAGIC_HIGH;
		}
	}

	/* Does the borg have both realms containing detect evil? */
	if (r_life + r_death == 2)
	{
		/* Can the borg detect evil already? */
		if (borg_spell_legal(REALM_LIFE, 0, 0) ||
			borg_spell_legal(REALM_DEATH, 0, 2))
		{
			/* Prevent the learning of the second detect evil */
			if (borg_magics[REALM_LIFE][0][0].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_LIFE][0][0].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_DEATH][0][2].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_DEATH][0][2].status =  BORG_MAGIC_HIGH;
		}
	}

	/* Does the borg have both realms containing trap/door destruction? */
	if (r_chaos + r_arcane == 2)
	{
		/* Can the borg detect evil already? */
		if (borg_spell_legal(REALM_CHAOS, 0, 1) ||
			borg_spell_legal(REALM_ARCANE, 0, 6))
		{
			/* Prevent the learning of the second detect evil */
			if (borg_magics[REALM_CHAOS][0][1].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_CHAOS][0][1].status =  BORG_MAGIC_HIGH;
			if (borg_magics[REALM_ARCANE][0][6].status == BORG_MAGIC_OKAY)
				borg_magics[REALM_ARCANE][0][6].status =  BORG_MAGIC_HIGH;
		}
	}
}


/*
 * Study and/or Test spells/prayers
 */
bool borg_play_magic(bool bored)
{

	int rate, b_rate = -1;
	int realm, b_realm = -1;
	int book, b_book = -1;
	int spell, b_spell = -1;
	int inven, b_inven = -1;

	/* Hack -- must use magic or prayers */
	if (!mp_ptr->spell_book) return (FALSE);

	/* Hack -- blind/confused */
	if (bp_ptr->status.blind || bp_ptr->status.confused) return (FALSE);

	/* Dark */
	if (!bp_ptr->cur_lite) return (FALSE);

	/* Low level borgs shouldn't learn the same spell twice */
	if (bp_ptr->lev < 30) borg_spell_prevent_learn();

	/* Check each realm, backwards */
	for (realm = MAX_REALM; realm > 0; realm--)
	{
		/* skip non my realms */
		if (!borg_has_realm(realm)) continue;

		/* Check each book (backwards) */
		for (book = 3; book >= 0; book--)
		{
			/* Look for the book */
			inven = borg_book[realm][book];

			/* No such book */
			if (inven < 0) continue;

			/* Check each spells */
			for (spell = 7; spell >= 0; spell--)
			{
				borg_magic *as = &borg_magics[realm][book][spell];

				/* Require "learnable" status */
				if (as->status != BORG_MAGIC_OKAY) continue;

				/* Obtain "rating" */
				rate = as->rating;

				/* Skip "boring" spells/prayers */
				if (!bored && (rate <= 50)) continue;

				/* Skip "icky" spells/prayers */
				if (rate <= 0) continue;
				
				/*
				 * Spells with higher casting cost than max mana
				 * should be learned last.
				 */
				if (borg_spell_mana(realm, book, spell) > bp_ptr->msp) rate = 1;


				/* Skip "worse" spells/prayers */
				if (rate <= b_rate) continue;

				/* Track it */
				b_inven = inven;
				b_rate = rate;
				b_realm = realm;
				b_book = book;
				b_spell = spell;
			}
		}
	}

	/* Study */
	if (bp_ptr->status.study && (b_rate > 0))
	{

		/* Realm */
		borg_magic *as = &borg_magics[b_realm][b_book][b_spell];

		/* Debugging Info */
		borg_note_fmt("# Studying %s spell %s.", as->realm_name, as->name);

		/* Learn the spell */
		borg_keypress('G');

		/* Specify the book */
		borg_keypress(I2A(b_inven));

		/* Some Classes can not choose */
		if (borg_class != CLASS_PRIEST &&
			borg_class != CLASS_PALADIN && borg_class != CLASS_MONK)
		{
			/* Specify the spell */
			borg_keypress(I2A(b_spell));
		}

		/* Success */
		return (TRUE);
	}

	/* Hack -- only in town */
	if (bp_ptr->depth) return (FALSE);

	/* Hack -- only when bored */
	if (!bored) return (FALSE);

	/* Check each realm backwards */
	for (realm = MAX_REALM; realm > 0; realm--)
	{
		/* Check each book (backwards) */
		for (book = 3; book >= 0; book--)
		{
			/* Only my realms */
			if (!borg_has_realm(realm)) continue;

			/* Look for the book */
			inven = borg_book[realm][book];

			/* No such book */
			if (inven < 0) continue;

			/* Check every spell (backwards) */
			for (spell = 8 - 1; spell >= 0; spell--)
			{
				borg_magic *as = &borg_magics[realm][book][spell];

				/* Only try "untried" spells/prayers */
				if (as->status != BORG_MAGIC_TEST) continue;

				/* Ignore "bizarre" spells/prayers */
				if (as->method == BORG_MAGIC_OBJ) continue;

				/* Make sure I have enough mana */
				if (bp_ptr->csp < borg_spell_mana(realm, book, spell)) continue;

				/* Note */
				borg_note("# Testing untried spell/prayer");

				/* Hack -- Use spell or prayer */
				if (borg_spell(realm, book, spell))
				{
					/* Hack -- Allow attack spells */
					if (as->method == BORG_MAGIC_AIM)
					{
						/* Hack -- target self */
						borg_keypress('*');
						borg_keypress('p');
						borg_keypress('t');
					}

					/* Hack -- Allow dimension Door */
					if (as->method == BORG_MAGIC_EXT)
					{
						/* Hack -- target self */
						borg_keypress(' ');
					}

					/* Hack -- Allow genocide spells */
					if (as->method == BORG_MAGIC_WHO)
					{
						/* Hack -- target townies */
						borg_keypress('t');
					}

					/* Success */
					return (TRUE);
				}
			}
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * Count the number of items worth "selling"
 *
 * This determines the choice of stairs.
 *
 * XXX XXX XXX Total hack, by the way...
 */
static int borg_count_sell(void)
{
	int i, k = 0;

	s32b price;
	s32b greed;


	/* Calculate "greed" factor */
	greed = (borg_gold / 100L) + 100L;

	/* Minimal greed */
	if (greed < 1000L) greed = 1000L;
	if (greed > 25000L) greed = 25000L;


	/* Count "sellable" items */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Skip "crappy" items */
		if (l_ptr->cost <= 0) continue;

		/* Obtain the base price */
		price = ((l_ptr->cost < 30000L) ? l_ptr->cost : 30000L);

		/* Skip cheap "known" (or "average") items */
		if ((price * l_ptr->number < greed) &&
			(borg_obj_known_p(l_ptr) ||
			 strstr(l_ptr->o_name, "{average"))) continue;

		/* Count remaining items */
		k++;
	}

	/* Result */
	return (k);
}


/*
 * Scan the item list and recharge items before leaving the
 * level.  Right now rod are not recharged from this.
 */
bool borg_wear_recharge(void)
{
	int i, b_i = -1;
	int slot = -1;

	/* No resting in danger */
	if (!borg_check_rest()) return (FALSE);

	/* Not if hungry */
	if (bp_ptr->status.weak) return (FALSE);

	/* Look for an (wearable- non rod) item to recharge */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* skip items that are charged */
		if (!l_ptr->timeout) continue;

		/* skip lites */
		if (l_ptr->tval == TV_LITE) continue;

		/* Where does this belong? */
		slot = borg_wield_slot(l_ptr);

		/* Skip stuff that can't be worn */
		if (slot < 0) continue;

		/* note this one */
		b_i = i;

		break;
	}

	/*
	 *Item must be worn to be recharged
	 * But, none if some equip is cursed
	 */
	if ((b_i >= 0) && !borg_wearing_cursed)
	{

		/* wear the item */
		borg_note("# Swapping Item for Recharge.");
		borg_keypress(ESCAPE);
		borg_keypress('w');
		borg_keypress(I2A(b_i));
		borg_keypress(' ');
		borg_keypress(' ');

		/* rest for a while */
		borg_keypress('R');
		borg_keypress('7');
		borg_keypress('5');
		borg_keypress('\n');

		/* done */
		return (TRUE);

	}

	/* nothing to recharge */
	return (FALSE);
}


/*
 * Leave the level if necessary (or bored)
 * Scumming defined in borg_prepared.
 */
bool borg_leave_level(bool bored)
{
	int k, g = 0;

	/* Hack -- waiting for "recall" */
	if (goal_recalling) return (FALSE);

	/* There is a great concern about recalling back to level 100.
	 * Often the borg will fall down a trap door to level 100 when he is not
	 * prepared to be there.  Some classes can use Teleport Level to get
	 * back up to 99,  But Warriors cannot.  Realistically the borg needs
	 * be be able to scum deep in the dungeon.  But he cannot risk being
	 * on 100 and using the few *Healing* pots that he managed to collect.
	 * It is better for warriors to walk all the way down to 98 and scum.
	 * It seems like a long and nasty crawl, but it is the best way to
	 * make sure the borg survives.  Along the way he will collect the
	 * Healing, Life and *Healing* that he needs.
	 *
	 * The other classes (or at least those who can use the Teleport Level
	 * spell) will not need to do this nasty crawl.  Risky Borgs will
	 * not crawl either.
	 */

	/* Town */
	if (!bp_ptr->depth)
	{
		/* Cancel rising */
		goal_rising = FALSE;

		/* Wait until bored */
		if (!bored) return (FALSE);

		/* Hack -- Recall into dungeon */
		if ((bp_ptr->max_depth >= 5) && (bp_ptr->recall >= 4) && borg_recall())
		{
			/* Note */
			borg_note("# Recalling into dungeon.");

			/* Give it a shot */
			return (TRUE);
		}
		else
		{
			/* note why we didn't recall. */
			if (bp_ptr->max_depth < 5)
				borg_note("# Not deep enough to recall");
			else if (bp_ptr->recall <= 4)
				borg_note("# Not enough recalls to recall");
			else
			{
				/* recall unless way out of our league */
				if (borg_prepared(bp_ptr->max_depth * 6 / 10))
				{
					cptr reason = borg_prepared(bp_ptr->max_depth);
					borg_note_fmt
						("# Way too scary to recall down there!   %s", reason);
				}
				else
					borg_note("# failed to recall when I wanted to");
			}

			goal_fleeing = TRUE;
			goal_leaving = TRUE;
		}

		stair_more = TRUE;

		/* Try to get to town location (town gate for now) */
		if (borg_flow_town_exit(GOAL_TOWN)) return (TRUE);

		/* Attempt to use those stairs */
		if (borg_flow_stair_more(GOAL_BORE)) return (TRUE);

		/* Oops */
		return (FALSE);
	}

	/** In the Dungeon **/

	/* do not hangout on boring levels for *too* long */
	if (!borg_prepared(bp_ptr->depth + 1)) g = 1;

	/* Count sellable items */
	k = borg_count_sell();

	/* Do not dive when "full" of items */
	if (g && (k >= 12)) g = 0;

	/* Do not dive when drained */
	if (g && bp_ptr->status.fixexp) g = 0;


	/* Hack -- Stay on each level for a minimal amount of time */
	if ((bp_ptr->lev > 10) &&
		(g != 0) && (borg_t - borg_began < value_feeling[borg_feeling]))
	{
		g = 0;
	}

	/* Rise a level if bored and unable to dive. */
	if (bored && borg_prepared(bp_ptr->depth + 1))
	{
		cptr reason = borg_prepared(bp_ptr->depth + 1);
		g = -1;
		borg_note_fmt("# heading up (bored and unable to dive: %s)", reason);
	}

	/* Power dive if I am playing too shallow */
	if (!borg_prepared(bp_ptr->depth + 5)) g = 1;

	/* Hack -- Power-climb upwards when needed */
	if (borg_prepared(bp_ptr->depth) && !unique_on_level)
	{
		cptr reason = borg_prepared(bp_ptr->depth);

		borg_note_fmt("# heading up (too deep: %s)", reason);
		g = -1;

		/* if I must restock go to town */
		if (borg_restock(bp_ptr->depth))
		{
			cptr reason = borg_prepared(bp_ptr->depth);

			borg_note_fmt("# returning to town to restock(too deep: %s)",
						  reason);
			goal_rising = TRUE;
		}

		/* if I am really out of depth go to town */
		if (borg_prepared(bp_ptr->max_depth * 5 / 10))
		{
			cptr reason = borg_prepared(bp_ptr->depth);
			borg_note_fmt("# returning to town (too deep: %s)", reason);
			goal_rising = TRUE;
		}
	}

	/* Hack -- if I am playing way too shallow return to town */
	if (!borg_prepared(bp_ptr->depth + 20) &&
		!borg_prepared(bp_ptr->max_depth * 6 / 10) &&
		bp_ptr->max_depth > bp_ptr->depth + 10)
	{
		borg_note("# returning to town to recall back down (too shallow)");
		goal_rising = TRUE;
	}

	/* Power dive too 100 if ready */
	if (!borg_prepared(100)) g = 1;

	/* Power dive if the Serpent is dead */
	if (bp_ptr->winner) g = 1;

	/* Return to town to sell stuff */
	if (bored && (k >= 12))
	{
		borg_note("# Going to town (Sell Stuff).");
		goal_rising = TRUE;
	}

	/* Return to town when level drained */
	if (bp_ptr->status.fixlvl)
	{
		borg_note("# Going to town (Fix Level).");
		goal_rising = TRUE;
	}

	/* Return to town to restore experience */
	if (bored && bp_ptr->status.fixexp && (bp_ptr->lev != 50))
	{
		borg_note("# Going to town (Fix Experience).");
		goal_rising = TRUE;
	}

	/* return to town if it has been a while */
	if ((!goal_rising && bored && !vault_on_level &&
		 !borg_fighting_unique &&
		 borg_time_town + borg_t - borg_began > 8000) ||
		(borg_time_town + borg_t - borg_began > 12000))
	{
		borg_note("# Going to town (I miss my home).");
		goal_rising = TRUE;
	}

	/* return to town if been scumming for a bit */
	if (bp_ptr->max_depth >= bp_ptr->depth + 25 &&
		bp_ptr->depth < 9 && borg_time_town + borg_t - borg_began > 3500)
	{
		borg_note("# Going to town (scumming check).");
		goal_rising = TRUE;
	}

	/* if returning to town, try to go upstairs */
	if (goal_rising) g = -1;

	/* Mega-Hack -- spend time on the first level to rotate shops */
	if ((bp_ptr->lev > 10) &&
		(bp_ptr->depth == 1) && (borg_t - borg_began < 100) && (g < 0))
	{
		g = 0;
	}

	/* Use random stairs when really bored */
	if (bored && (borg_t - borg_began >= 5000))
	{
		/* Note */
		borg_note("# Choosing random stairs.");

		/* Use random stairs */
		g = ((randint0(100) < 50) ? -1 : 1);
	}

	/* Go Up */
	if (g < 0)
	{
		/* Take next stairs */
		stair_less = TRUE;

		/* Hack -- recall if going to town */
		if (goal_rising &&
			((borg_time_town + (borg_t - borg_began)) > 200) &&
			(bp_ptr->depth >= 5) && borg_recall())
		{
			borg_note("# Recalling to town (goal rising)");
			return (TRUE);
		}

		/* Attempt to use stairs */
		if (borg_flow_stair_less(GOAL_BORE)) return (TRUE);

		/* Cannot find any stairs */
		if (goal_rising && bored && (borg_t - borg_began) >= 1000)
		{
			if (borg_recall())
			{
				borg_note("# Recalling to town (no stairs)");
				return (TRUE);
			}
		}
	}


	/* Go Down */
	if (g > 0)
	{
		/* Take next stairs */
		stair_more = TRUE;

		/* Attempt to use those stairs */
		if (borg_flow_stair_more(GOAL_BORE)) return (TRUE);
	}


	/* Failure */
	return (FALSE);
}





/*
 * Initialize this file
 */
void borg_init_7(void)
{
	/* Nothing */
}



#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
