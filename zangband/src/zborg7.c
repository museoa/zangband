/* File: borg7.c */
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
	object_kind *k_ptr = &k_info[l_ptr->k_idx];

	/* if its average, dump it if you want to. */
	if (strstr(l_ptr->o_name, "{average")) return (TRUE);

	/* Mega-Hack -- allow "icky" items */
	if (borg_class == CLASS_PRIEST ||
		borg_class == CLASS_RANGER ||
		borg_class == CLASS_MAGE || borg_skill[BI_CLEVEL] < 20)
	{
		/* things that are good/excelent/special */
		if (strstr(l_ptr->o_name, "{special") ||
			strstr(l_ptr->o_name, "{terrible") ||
			strstr(l_ptr->o_name, "{excellent"))
			/* not icky */
			return (FALSE);

		/* Broken dagger/sword, Filthy rag */
		if (((l_ptr->tval == TV_SWORD) && (k_ptr->sval == SV_BROKEN_DAGGER)) ||
			((l_ptr->tval == TV_SWORD) && (k_ptr->sval == SV_BROKEN_SWORD)) ||
			((l_ptr->tval == TV_SOFT_ARMOR) && (k_ptr->sval == SV_FILTHY_RAG)))
		{
			return (TRUE);
		}

		/* Dagger, Sling */
		if (((l_ptr->tval == TV_SWORD) && (k_ptr->sval == SV_DAGGER)) ||
			((l_ptr->tval == TV_BOW) && (k_ptr->sval == SV_SLING)))
		{
			return (TRUE);
		}

		/* Cloak, Robe */
		if (((l_ptr->tval == TV_CLOAK) && (k_ptr->sval == SV_CLOAK)) ||
			((l_ptr->tval == TV_SOFT_ARMOR) && (k_ptr->sval == SV_ROBE)))
		{
			return (TRUE);
		}

		/* Leather Gloves */
		if ((l_ptr->tval == TV_GLOVES) &&
			(k_ptr->sval == SV_SET_OF_LEATHER_GLOVES))
		{
			return (TRUE);
		}

		/* Assume the item is not icky */
		return (FALSE);
	}

	/*
	 * Process other classes which do get pseudo ID
	 * things that are good/excelent/special/no P-ID
	 */
	if (strstr(l_ptr->o_name, "{special") || strstr(l_ptr->o_name, "{terrible")
		|| strstr(l_ptr->o_name, "{excellent"))
		return (FALSE);


		/*** {Good} items in inven, But I have {excellent} in equip ***/

	if (strstr(l_ptr->o_name, "{good"))
	{
		int slot;

		/* Obtain the slot of the suspect item */
		slot = borg_wield_slot(l_ptr);

		/* Obtain my equipped item in the slot */
		l_ptr = &equipment[slot];

		/* Is my item an ego or artifact? */
		if (l_ptr->xtra_name) return (TRUE);
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
	if (borg_skill[BI_ISFIXEXP] &&
		(borg_spell(REALM_LIFE, 3, 3) ||
		 borg_spell(REALM_DEATH, 1, 7) ||
		 borg_activate_artifact(ART_LUTHIEN, FALSE) ||
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
	if ((borg_skill[BI_ISFIXSTR] &&
		 (borg_quaff_potion(SV_POTION_RES_STR) ||
		  borg_quaff_potion(SV_POTION_INC_STR) ||
		  borg_eat_food(SV_FOOD_RESTORE_STR) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(borg_skill[BI_ISFIXINT] &&
		 (borg_quaff_potion(SV_POTION_RES_INT) ||
		  borg_quaff_potion(SV_POTION_INC_INT) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(borg_skill[BI_ISFIXWIS] &&
		 (borg_quaff_potion(SV_POTION_RES_WIS) ||
		  borg_quaff_potion(SV_POTION_INC_WIS) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(borg_skill[BI_ISFIXDEX] &&
		 (borg_quaff_potion(SV_POTION_RES_DEX) ||
		  borg_quaff_potion(SV_POTION_INC_DEX) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		(borg_skill[BI_ISFIXCON] &&
		 (borg_quaff_potion(SV_POTION_RES_CON) ||
		  borg_quaff_potion(SV_POTION_INC_CON) ||
		  borg_eat_food(SV_FOOD_RESTORE_CON) ||
		  borg_eat_food(SV_FOOD_RESTORING))) ||
		((borg_skill[BI_ISFIXCHR]) &&
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
						if (!borg_skill[BI_CDEPTH]) break;

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
				if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) break;

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
						if (!borg_skill[BI_CDEPTH]) break;

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
	if (borg_skill[BI_ISHUNGRY])
	{
		/* Attempt to satisfy hunger */
		if (borg_eat_food(SV_FOOD_BISCUIT) ||
			borg_eat_food(SV_FOOD_JERKY) ||
			borg_eat_food(SV_FOOD_WAYBREAD) ||
			borg_eat_food(SV_FOOD_RATION) ||
			borg_spell(REALM_SORCERY, 2, 0) ||
			borg_spell(REALM_LIFE, 0, 7) ||
			borg_spell(REALM_ARCANE, 2, 7) || borg_spell(REALM_NATURE, 0, 3))
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

	bool do_lite_aux = FALSE;


	/* Never in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);

	/* Never when comprimised, save your mana */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE] || borg_skill[BI_ISPOISONED] ||
		borg_skill[BI_ISCUT] || borg_skill[BI_ISWEAK]) return (FALSE);

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
		 183 - (20 - borg_skill[BI_MAXCLEVEL]))) do_evil = TRUE;

	/* Dont bother if I have ESP */
	if (borg_skill[BI_ESP]) do_evil = FALSE;

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
			borg_spell_fail(REALM_ARCANE, 3, 5, 20) ||
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
		if (borg_spell_fail(REALM_LIFE, 0, 5, 20) ||
			borg_spell_fail(REALM_SORCERY, 0, 2, 20) ||
			borg_spell_fail(REALM_ARCANE, 1, 0, 20) ||
			borg_spell_fail(REALM_NATURE, 1, 2, 20) ||
			borg_racial(RACE_DWARF) || borg_racial(RACE_NIBELUNG))
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
		/* Check for traps */
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
			borg_zap_rod(SV_ROD_MAPPING) || borg_spell(REALM_NATURE, 1, 2))
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
		if (borg_spell_fail(REALM_LIFE, 0, 0, 20) ||
			borg_spell_fail(REALM_ARCANE, 0, 3, 20) ||
			borg_spell_fail(REALM_NATURE, 0, 0, 20) ||
			borg_spell_fail(REALM_SORCERY, 0, 0, 20) ||
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
	if (do_lite && (borg_skill[BI_CUR_LITE] >= 2) && (randint0(100) < 90))
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
	if (floors <= 11) do_lite = do_lite_aux = FALSE;

	/* Vampires need to be careful for Light */
	if (borg_skill[BI_FEAR_LITE])
		do_lite = do_lite_aux = FALSE;

	/* Hack -- call lite */
	if (do_lite && (!when_call_lite || (borg_t - when_call_lite >= 7)))
	{
		/* Call light */
		if (borg_activate_artifact(ART_GALADRIEL, FALSE) ||
			borg_zap_rod(SV_ROD_ILLUMINATION) ||
			borg_use_staff(SV_STAFF_LITE) ||
			borg_read_scroll(SV_SCROLL_LIGHT) ||
			borg_spell(REALM_ARCANE, 0, 5) ||
			borg_spell(REALM_CHAOS, 0, 2) ||
			borg_spell(REALM_NATURE, 0, 4) ||
			borg_spell(REALM_SORCERY, 0, 3) || borg_spell(REALM_LIFE, 0, 4))
		{
			borg_note("# Illuminating the room");

			borg_react("SELF:lite", "SELF:lite");

			when_call_lite = borg_t;

			return (TRUE);
		}
	}


	/* Hack -- Wizard Lite */
	if (TRUE && (!when_wizard_lite || (borg_t - when_wizard_lite >= 1000)))
	{
		/* Wizard lite */
		if (borg_activate_artifact(ART_THRAIN, FALSE) ||
			borg_spell(REALM_ARCANE, 3, 7) ||
			borg_spell(REALM_SORCERY, 3, 3) || borg_spell(REALM_NATURE, 3, 5))
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

	int q_x, q_y;

	map_block *mb_ptr;


	bool do_lite;

	bool do_lite_aux = FALSE;


	/* Never in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);

	/* Never when blind or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISIMAGE]) return (FALSE);

	/* Vampires need to be careful for Light */
	if (borg_skill[BI_FEAR_LITE])
		return (FALSE);

	/* XXX XXX XXX Dark */


	/* Extract the panel */
	q_x = w_x / 33;
	q_y = w_y / 11;

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
	if (do_lite && (borg_skill[BI_CUR_LITE] >= 2) && (randint0(100) < 90))
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
	if (floors <= 11) do_lite = do_lite_aux = FALSE;

	/* Hack -- call lite */
	if (do_lite && (!when_call_lite || (borg_t - when_call_lite >= 7)))
	{
		/* Call light */
		if (borg_activate_artifact(ART_GALADRIEL, FALSE) ||
			borg_zap_rod(SV_ROD_ILLUMINATION) ||
			borg_use_staff(SV_STAFF_LITE) ||
			borg_read_scroll(SV_SCROLL_LIGHT) ||
			borg_spell(REALM_ARCANE, 0, 5) ||
			borg_spell(REALM_CHAOS, 0, 2) ||
			borg_spell(REALM_NATURE, 0, 4) ||
			borg_spell(REALM_SORCERY, 0, 3) || borg_spell(REALM_LIFE, 0, 4))
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
			borg_spell(REALM_SORCERY, 3, 3) || borg_spell(REALM_NATURE, 3, 5))
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

/* Check to see if the borg is standing on a nasty grid.
 * Lava can hurt the borg unless he is IFire.
 * Water can hurt if it is deep and encumbered.
 * Levetation item can reduce the effect of nasty grids.
 */
bool borg_on_safe_grid(void)
{
	/* Get the grid under the borg */
	map_block *mb_ptr = map_loc(c_x, c_y);

	/* Lava */
	if (mb_ptr->feat == FEAT_SHAL_LAVA)
	{
		/* Immunity helps */
		if (borg_skill[BI_IFIRE]) return (TRUE);

		/* Invulnerability helps */
		if (borg_goi) return (TRUE);

		/* Everything else hurts */
		return (FALSE);
	}

	/* Water */
	if (mb_ptr->feat == FEAT_SHAL_WATER)
	{
		/* Levatation helps */
		if (borg_skill[BI_FEATH]) return (TRUE);

		/* Invulnerability helps */
		if (borg_goi) return (TRUE);

		/* Being non-encumbered helps */
		if (!borg_skill[BI_ENCUMBERD]) return (TRUE);

		/* Everything else hurts */
		return (FALSE);
	}

	/* Generally ok */
	return (TRUE);
}

/*
 * Enchant armor, not including my swap armour
 */
static bool borg_enchant_to_a(void)
{
	int i, b_i = -1;
	int a, b_a = 99;

	/* Nothing to enchant */
	if (!my_need_enchant_to_a) return (FALSE);

	/* Need "enchantment" ability */
	if ((!amt_enchant_to_a) && (!amt_enchant_armor)) return (FALSE);


	/* Look for armor that needs enchanting */
	for (i = EQUIP_BODY; i < equip_num; i++)
	{
		list_item *l_ptr = &equipment[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Skip non-identified items */
		if (!(l_ptr->info & OB_KNOWN)) continue;

		/* Obtain the bonus */
		a = l_ptr->to_a;

		/* Skip "boring" items */
		if (borg_spell_okay_fail(REALM_SORCERY, 3, 5, 40) ||
			amt_enchant_armor >= 1)
		{
			if (a >= 15) continue;
		}
		else
		{
			if (a >= 8) continue;
		}

		/* Find the least enchanted item */
		if ((b_i >= 0) && (b_a < a)) continue;

		/* Save the info */
		b_i = i;
		b_a = a;
	}

	/* Nothing */
	if (b_i < 0) return (FALSE);

	/* Enchant it */
	if (borg_spell_fail(REALM_SORCERY, 3, 5, 40) ||
		borg_read_scroll(SV_SCROLL_STAR_ENCHANT_ARMOR) ||
		borg_read_scroll(SV_SCROLL_ENCHANT_ARMOR))
	{
		borg_keypress('/');

		/* Choose that item */
		borg_keypress(I2A(b_i));

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
	int i, b_i = -1;
	int a, b_a = 99;

	bool inven = FALSE;

	/* Nothing to enchant */
	if (!my_need_enchant_to_h) return (FALSE);

	/* Need "enchantment" ability */
	if ((!amt_enchant_to_h) && (!amt_enchant_weapon)) return (FALSE);


	/* Look for a weapon that needs enchanting */
	for (i = EQUIP_WIELD; i <= EQUIP_BOW; i++)
	{
		list_item *l_ptr = &equipment[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Skip non-identified items */
		if (!(l_ptr->info & OB_KNOWN)) continue;

		/* Obtain the bonus */
		a = l_ptr->to_h;

		/* Skip "boring" items */
		if (borg_spell_okay_fail(REALM_SORCERY, 3, 4, 40) ||
			amt_enchant_weapon >= 1)
		{
			if (a >= 15) continue;
		}
		else
		{
			if (a >= 8) continue;
		}

		/* Find the least enchanted item */
		if ((b_i >= 0) && (b_a < a)) continue;

		/* Save the info */
		b_i = i;
		b_a = a;
	}

	/* Nothing, check ammo */
	if (b_i < 0)
	{
		/* look through inventory for ammo */
		for (i = 0; i < inven_num; i++)
		{
			list_item *l_ptr = &inventory[i];

			/* Only enchant if qty >= 5 */
			if (l_ptr->number < 5) continue;

			/* Skip non-identified items  */
			if (!(l_ptr->info & OB_KNOWN)) continue;

			/* Make sure it is the right type if missile */
			if (l_ptr->tval != my_ammo_tval) continue;

			/* Obtain the bonus  */
			a = l_ptr->to_h;

			/* Skip items that are already enchanted */
			if (borg_spell_okay_fail(REALM_SORCERY, 3, 4, 40) ||
				amt_enchant_weapon >= 1)
			{
				if (a >= 10) continue;
			}
			else
			{
				if (a >= 8) continue;
			}

			/* Find the least enchanted item */
			if ((b_i >= 0) && (b_a < a)) continue;

			/* Save the info  */
			b_i = i;
			b_a = a;

			/* Item is in inventory */
			inven = TRUE;
		}
	}

	/* Nothing */
	if (b_i < 0) return (FALSE);

	/* Enchant it */
	if (borg_spell_fail(REALM_SORCERY, 3, 4, 40) ||
		borg_read_scroll(SV_SCROLL_STAR_ENCHANT_WEAPON) ||
		borg_read_scroll(SV_SCROLL_ENCHANT_WEAPON_TO_HIT))
	{

		if (inven)
		{
			/* choose the swap or ammo */
			borg_keypress(I2A(b_i));
		}
		else
		{
			/* Choose from equipment */
			borg_keypress('/');

			/* Choose that item */
			borg_keypress(I2A(b_i));
		}

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
	int i, b_i = -1;
	int a, b_a = 99;

	bool inven = FALSE;

	/* Nothing to enchant */
	if (!my_need_enchant_to_d) return (FALSE);

	/* Need "enchantment" ability */
	if ((!amt_enchant_to_d) && (!amt_enchant_weapon)) return (FALSE);


	/* Look for a weapon that needs enchanting */
	for (i = EQUIP_WIELD; i <= EQUIP_BOW; i++)
	{
		list_item *l_ptr = &equipment[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Skip non-identified items */
		if (!(l_ptr->info & OB_KNOWN)) continue;

		/* Obtain the bonus */
		a = l_ptr->to_d;

		/* Skip "boring" items */
		if (borg_spell_okay_fail(REALM_SORCERY, 3, 4, 40) ||
			amt_enchant_weapon >= 1)
		{
			if (a >= 15) continue;
		}
		else
		{
			if (a >= 8) continue;
		}

		/* Find the least enchanted item */
		if ((b_i >= 0) && (b_a < a)) continue;

		/* Save the info */
		b_i = i;
		b_a = a;
	}

	/* Nothing, check ammo */
	if (b_i < 0)
	{
		/* look through inventory for ammo */
		for (i = 0; i < inven_num; i++)
		{
			list_item *l_ptr = &inventory[i];

			/* Only enchant if qty >= 5 */
			if (l_ptr->number < 5) continue;

			/* Skip non-identified items  */
			if (!(l_ptr->info & OB_KNOWN)) continue;

			/* Make sure it is the right type if missile */
			if (l_ptr->tval != my_ammo_tval) continue;

			/* Obtain the bonus  */
			a = l_ptr->to_d;

			/* Skip items that are already enchanted */
			if (borg_spell_okay_fail(REALM_SORCERY, 3, 4, 40) ||
				amt_enchant_weapon >= 1)
			{
				if (a >= 10) continue;
			}
			else
			{
				if (a >= 8) continue;
			}

			/* Find the least enchanted item */
			if ((b_i >= 0) && (b_a < a)) continue;

			/* Save the info  */
			b_i = i;
			b_a = a;

			/* Item is in the inventory */
			inven = TRUE;
		}
	}

	/* Nothing */
	if (b_i < 0) return (FALSE);

	/* Enchant it */
	if (borg_spell_fail(REALM_SORCERY, 3, 4, 40) ||
		borg_read_scroll(SV_SCROLL_STAR_ENCHANT_WEAPON) ||
		borg_read_scroll(SV_SCROLL_ENCHANT_WEAPON_TO_DAM))
	{
		if (inven)
		{
			/* choose the swap or ammo */
			borg_keypress(I2A(b_i));
		}
		else
		{
			/* Choose from equipment */
			borg_keypress('/');

			/* Choose that item */
			borg_keypress(I2A(b_i));
		}

		/* Success */
		return (TRUE);
	}

	/* Nothing to do */
	return (FALSE);
}

/*
 * Brand Bolts
 */
static bool borg_brand_weapon(void)
{
#if 0

	int i, b_i = -1;
	int a, b_a = 0;

	/* Nothing to brand */
	if (!my_need_brand_weapon) return (FALSE);

	/* Need "brand" ability */
	if (!amt_brand_weapon) return (FALSE);

	/* look through inventory for ammo */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Only enchant if qty >= 5 */
		if (l_ptr->number < 5) continue;

		/* Skip non-identified items  */
		if (!(l_ptr->info & OB_KNOWN)) continue;

		/* Make sure it is the right type if missile */
		if (l_ptr->tval != TV_BOLT) continue;

		/* Obtain the bonus  */
		a = l_ptr->to_h;

		/* Skip branded items */
		if (l_ptr->xtra_name) continue;

		/* Find the most enchanted item */
		if ((b_i >= 0) && (b_a > a)) continue;

		/* Save the info  */
		b_i = i;
		b_a = a;
	}

	/* Enchant it */
	if (borg_activate_artifact(ART_BRAND, FALSE))
	{


		/* 291 would like a location of the brand */
		/* choose the or ammo */
		{
			borg_keypress(I2A(b_i));
		}

		/* Success */
		return (TRUE);
	}
#endif /* 0 */

	/* Nothing to do */
	return (FALSE);
}

/*
 * Remove Curse
 */
static bool borg_decurse(void)
{
	/* Nothing to decurse */
	if (!borg_wearing_cursed) return (FALSE);

	/* Ability for light curse */
	if (borg_wearing_cursed)
	{
		if (!borg_slot(TV_SCROLL, SV_SCROLL_REMOVE_CURSE) &&
			(!borg_slot(TV_STAFF, SV_STAFF_REMOVE_CURSE) &&
			 !borg_slot(TV_STAFF, SV_STAFF_REMOVE_CURSE)->pval)
			&& !borg_spell_okay_fail(REALM_LIFE, 2, 1, 40))
		{
			return (FALSE);
		}

		/* remove the curse */
		if (borg_read_scroll(SV_SCROLL_REMOVE_CURSE) ||
			borg_use_staff(SV_STAFF_REMOVE_CURSE) ||
			borg_spell(REALM_LIFE, 2, 1))
		{
			/* Shekockazol! */
			borg_wearing_cursed = FALSE;
			return (TRUE);
		}
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
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (FALSE);

	/*apw Forbid if been sitting on level forever */
	/*    Just come back and finish the job later */
	if ((borg_t - borg_began > 150 && borg_skill[BI_CDEPTH]) ||
		(borg_t - borg_began > 350 && !borg_skill[BI_CDEPTH])) return (FALSE);

	/* Remove Curses */
	if (borg_decurse()) return (TRUE);

	/* Enchant things */
	if (borg_enchant_to_d()) return (TRUE);
	if (borg_enchant_to_a()) return (TRUE);
	if (borg_enchant_to_h()) return (TRUE);
	if (borg_brand_weapon()) return (TRUE);

	/* Nope */
	return (FALSE);
}


/*
 * Recharge things
 *
 * XXX XXX XXX Prioritize available items
 */
bool borg_recharging(void)
{
	int i = -1;
	bool charge = FALSE;

	/* Forbid blind/confused */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (FALSE);

	/* XXX XXX XXX Dark */

	/* Look for an item to recharge */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip empty /unaware items */
		if (!l_ptr->k_idx) continue;

		/* Skip non-identified items */
		if (!(l_ptr->info & OB_KNOWN)) continue;

		/* assume we can't charge it. */
		charge = FALSE;

		/* Wands with no charges can be charged */
		if ((l_ptr->tval == TV_WAND) && (l_ptr->pval < 1))
			charge = TRUE;

		/* recharge staves sometimes */
		if (l_ptr->tval == TV_STAFF)
		{
			/*
			 * Allow staves to be recharged at 2 charges if
			 * the borg has the big recharge spell. And its not a *Dest*
			 */
			if ((l_ptr->pval < 3) &&
				(borg_spell_okay(REALM_SORCERY, 0, 7) ||
				 borg_spell_okay(REALM_CHAOS, 2, 2) ||
				 borg_spell_okay(REALM_ARCANE, 3, 0)) &&
				k_info[l_ptr->k_idx].sval < SV_STAFF_POWER)
				charge = TRUE;

			/* recharge any staff at 0 charges */
			if (l_ptr->pval < 1)
				charge = TRUE;
		}

		/* recharge rods that are 'charging' if we have the big recharge */
		/* spell */
		if ((l_ptr->tval == TV_ROD) && l_ptr->timeout &&
			(borg_spell_okay(REALM_SORCERY, 0, 7) ||
			 borg_spell_okay(REALM_CHAOS, 2, 2) ||
			 borg_spell_okay(REALM_ARCANE, 3, 0)))
		{
			charge = TRUE;
		}

		/* get the next item if we are not to charge this one */
		if (!charge) continue;

		/* Attempt to recharge */
		if (borg_spell(REALM_SORCERY, 0, 7) ||
			borg_spell(REALM_ARCANE, 3, 0) ||
			borg_spell(REALM_CHAOS, 2, 2) ||
			borg_read_scroll(SV_SCROLL_RECHARGING) ||
			borg_activate_artifact(ART_THINGOL, FALSE))
		{
			/* Message */
			borg_note(format("Recharging %s", l_ptr->o_name));

			/* Recharge the item */
			borg_keypress(I2A(i));

			/* Success */
			return (TRUE);
		}
		else
			/* if we fail once, no need to try again. */
			break;
	}

	/* Nope */
	return (FALSE);
}


/*
 * Attempt to consume an item
 */
static bool borg_consume(list_item *l_ptr)
{
	/* Special destruction */
	switch (l_ptr->tval)
	{
		case TV_POTION:
		{
			/* Check the potion */
			switch (k_info[l_ptr->k_idx].sval)
			{
				case SV_POTION_WATER:
				case SV_POTION_APPLE_JUICE:
				case SV_POTION_SLIME_MOLD:
				case SV_POTION_CURE_LIGHT:
				case SV_POTION_CURE_SERIOUS:
				case SV_POTION_CURE_CRITICAL:
				case SV_POTION_HEALING:
				case SV_POTION_STAR_HEALING:
				case SV_POTION_LIFE:
				case SV_POTION_RES_STR:
				case SV_POTION_RES_INT:
				case SV_POTION_RES_WIS:
				case SV_POTION_RES_DEX:
				case SV_POTION_RES_CON:
				case SV_POTION_RES_CHR:
				case SV_POTION_RESTORE_EXP:
				case SV_POTION_RESTORE_MANA:
				case SV_POTION_HEROISM:
				case SV_POTION_BERSERK_STRENGTH:
				case SV_POTION_RESIST_HEAT:
				case SV_POTION_RESIST_COLD:
				case SV_POTION_INFRAVISION:
				case SV_POTION_DETECT_INVIS:
				case SV_POTION_SLOW_POISON:
				case SV_POTION_CURE_POISON:
				case SV_POTION_SPEED:
				{

					/* Try quaffing the potion */
					if (borg_quaff_potion(k_info[l_ptr->k_idx].sval))
							return (TRUE);
				}
			}

			break;
		}

		case TV_SCROLL:
		{

			/* Check the scroll */
			switch (k_info[l_ptr->k_idx].sval)
			{
				case SV_SCROLL_REMOVE_CURSE:
				case SV_SCROLL_LIGHT:
				case SV_SCROLL_MONSTER_CONFUSION:
				case SV_SCROLL_STAR_REMOVE_CURSE:
				case SV_SCROLL_DETECT_GOLD:
				case SV_SCROLL_DETECT_ITEM:
				case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
				case SV_SCROLL_SATISFY_HUNGER:
				case SV_SCROLL_DISPEL_UNDEAD:
				case SV_SCROLL_BLESSING:
				case SV_SCROLL_HOLY_CHANT:
				case SV_SCROLL_HOLY_PRAYER:
				{
					/* XXX maybe consume Enchant scrolls on items */

					/* Try reading the scroll */
					if (borg_read_scroll(k_info[l_ptr->k_idx].sval))
							return (TRUE);
				}
			}

			break;
		}

		case TV_FOOD:
		{
			/* Check the grub */
			switch (k_info[l_ptr->k_idx].sval)
			{
				case SV_FOOD_CURE_POISON:
				case SV_FOOD_CURE_BLINDNESS:
				case SV_FOOD_CURE_PARANOIA:
				case SV_FOOD_CURE_CONFUSION:
				case SV_FOOD_CURE_SERIOUS:
				case SV_FOOD_RESTORE_STR:
				case SV_FOOD_RESTORE_CON:
				case SV_FOOD_RESTORING:
				case SV_FOOD_BISCUIT:
				case SV_FOOD_JERKY:
				case SV_FOOD_RATION:
				case SV_FOOD_SLIME_MOLD:
				case SV_FOOD_WAYBREAD:
				case SV_FOOD_PINT_OF_ALE:
				case SV_FOOD_PINT_OF_WINE:
				{
					/* Try eating the food (unless Bloated) */
					if (!borg_skill[BI_ISFULL] &&
						borg_eat_food(k_info[l_ptr->k_idx].sval)) return (TRUE);
				}
			}

			break;
		}
	}


	/* Nope */
	return (FALSE);
}



/*
 * Destroy "junk" items
 */
bool borg_crush_junk(void)
{
	int i;
	bool fix = FALSE;
	s32b p;
	s32b value;

	/* Hack -- no need */
	if (!borg_do_crush_junk) return (FALSE);

	/* No crush if even slightly dangerous */
	if (borg_danger(c_x, c_y, 1, TRUE) >
		borg_skill[BI_CURHP] / 10) return (FALSE);

	/* Destroy actual "junk" items */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* dont crush our spell books */
		if (l_ptr->tval == mp_ptr->spell_book) continue;

		/* save the items value */
		/* value = item->value; */

		/* Hack - we need to work this out properly */
		value = 0;

		/* Try not to crush ammo */
		if (l_ptr->tval == my_ammo_tval) value = 100L;

		/* Skip non "worthless" items */
		if (l_ptr->tval >= TV_FIGURINE)
		{
			/* unknown? */
			if (!(l_ptr->info & OB_KNOWN) &&
				!strstr(l_ptr->o_name, "{average")) continue;

#if 0

			/* skip items that are 'valuable'.  This is level dependent */
			/* try to make the borg junk +1,+1 dagger at level 40 */

			/* if the item gives a bonus to a stat, boost its value */
			if ((l_ptr->flags1 & TR1_STR) ||
				(l_ptr->flags1 & TR1_INT) ||
				(l_ptr->flags1 & TR1_WIS) ||
				(l_ptr->flags1 & TR1_DEX) || (l_ptr->flags1 & TR1_CON))
			{
				value += 2000L;
			}

			/* Get sval */
			sval = k_info[l_ptr->k_idx].sval;

			/* Keep some stuff */
			if (((l_ptr->tval == TV_POTION &&
				  sval == SV_POTION_RESTORE_MANA) &&
				 (borg_skill[BI_MAXSP] >= 1)) || (l_ptr->tval == TV_POTION &&
												  (sval ==
												   SV_POTION_HEALING)) ||
				(l_ptr->tval == TV_POTION &&
				 sval == SV_POTION_STAR_HEALING) ||
				(l_ptr->tval == TV_POTION && sval == SV_POTION_LIFE) ||
				(l_ptr->tval == TV_POTION && sval == SV_POTION_SPEED) ||
				(l_ptr->tval == TV_ROD && sval == SV_ROD_DRAIN_LIFE) ||
				(l_ptr->tval == TV_ROD &&
				 (sval == SV_ROD_HEALING || sval == SV_ROD_MAPPING)
				 && borg_class == CLASS_WARRIOR) || (l_ptr->tval == TV_STAFF &&
													 sval ==
													 SV_STAFF_DISPEL_EVIL) ||
				(l_ptr->tval == TV_STAFF && sval == SV_STAFF_POWER) ||
				(l_ptr->tval == TV_STAFF && sval == SV_STAFF_HOLINESS) ||
				(l_ptr->tval == TV_WAND && sval == SV_WAND_DRAIN_LIFE) ||
				(l_ptr->tval == TV_WAND && sval == SV_WAND_ANNIHILATION) ||
				(l_ptr->tval == TV_SCROLL &&
				 sval == SV_SCROLL_TELEPORT_LEVEL &&
				 borg_skill[BI_ATELEPORTLVL] < 1000) ||
				(l_ptr->tval == TV_SCROLL &&
				 sval == SV_SCROLL_PROTECTION_FROM_EVIL))
			{
				value += 5000L;
			}


#endif /* 0 */

			/* Pretend item isn't there */
			l_ptr->treat_as = TREAT_AS_SWAP;

			/* Fix later */
			fix = TRUE;

			/* Examine the inventory */
			borg_notice();

			/* Evaluate the inventory */
			p = borg_power();

			/* Restore item */
			l_ptr->treat_as = TREAT_AS_NORM;

			/* Hack - set value */
			value = my_power - p;

			/* up to level 5, keep anything of any value */
			if (borg_skill[BI_CDEPTH] < 5 && value > 0)
				continue;
			/* up to level 15, keep anything of value 100 or better */
			if (borg_skill[BI_CDEPTH] < 15 && value > 100)
				continue;
			/* up to level 30, keep anything of value 500 or better */
			if (borg_skill[BI_CDEPTH] < 30 && value > 500)
				continue;
			/* up to level 40, keep anything of value 1000 or better */
			if (borg_skill[BI_CDEPTH] < 40 && value > 1000)
				continue;
			/* up to level 60, keep anything of value 1200 or better */
			if (borg_skill[BI_CDEPTH] < 60 && value > 1200)
				continue;
			/* up to level 80, keep anything of value 1400 or better */
			if (borg_skill[BI_CDEPTH] < 80 && value > 1400)
				continue;
			/* up to level 90, keep anything of value 1600 or better */
			if (borg_skill[BI_CDEPTH] < 90 && value > 1600)
				continue;
			/* up to level 95, keep anything of value 1800 or better */
			if (borg_skill[BI_CDEPTH] < 95 && value > 1800)
				continue;
			/* below level 127, keep anything of value 2000 or better */
			if (borg_skill[BI_CDEPTH] < 127 && value > 2000)
				continue;
		}

		/* re-examine the inventory */
		if (fix) borg_notice();

		/* Hack -- skip good un-id'd "artifacts" */
		if (strstr(l_ptr->o_name, "{special")) continue;
		if (strstr(l_ptr->o_name, "{terrible")) continue;

		/* hack check anything interesting */
		if (l_ptr->xtra_name && !(l_ptr->info & OB_MENTAL)) continue;

		/* Message */
		borg_note(format("# Junking junk (valued at %d)", value));

		/* Message */
		borg_note(format("# Destroying %s.", l_ptr->o_name));

		/* Destroy all items */
		borg_keypresses("099");

		/* Destroy that item */
		if (!(l_ptr->kn_flags3 & TR3_INSTA_ART))
			borg_keypress('k');
		else
		{
			int a;

			/* worthless artifacts are dropped. */
			borg_keypress('d');

			/* mark the spot that the object was dropped so that  */
			/* it will not be picked up again. */
			for (a = 0; a < 50; a++)
			{
				if (bad_obj_x[a] != -1) continue;
				if (bad_obj_y[a] != -1) continue;

				bad_obj_x[a] = c_x;
				bad_obj_y[a] = c_y;
				borg_note(format
						  ("# Crappy artifact at %d,%d", bad_obj_x[a],
						   bad_obj_y[a]));
				break;
			}
		}
		borg_keypress(I2A(i));

		/* Verify destruction */
		borg_keypress('y');

		/* Success */
		return (TRUE);
	}

	/* re-examine the inventory */
	if (fix) borg_notice();

	/* Hack -- no need */
	borg_do_crush_junk = FALSE;


	/* Nothing to destroy */
	return (FALSE);
}



/*
 * Destroy something to make a free inventory slot.
 *
 * This function evaluates the possible worlds that result from
 * the destruction of each inventory slot, and attempts to destroy
 * that slot which causes the best possible resulting world.
 *
 * We attempt to avoid destroying unknown items by "rewarding" the
 * presence of unknown items by a massively heuristic value.
 *
 * If the Borg cannot find something to destroy, which should only
 * happen if he fills up with artifacts, then he will probably act
 * rather twitchy for a while.
 *
 * This function does not have to be very efficient.
 */
bool borg_crush_hole(void)
{
	int i, b_i = -1;
	s32b p, b_p = 0L;

	bool fix = FALSE;

	byte sval;

	/* Do not destroy items unless we need the space */
	if (inven_num < INVEN_PACK) return (FALSE);

	/* Scan the inventory */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Hack -- skip "artifacts" */
		if (l_ptr->kn_flags3 & TR3_INSTA_ART) continue;

		/* dont crush our spell books */
		if (l_ptr->tval == mp_ptr->spell_book) continue;

		/* Hack -- skip artifacts and ego items not fully identified */
		if (l_ptr->xtra_name && !(l_ptr->info & OB_MENTAL)) continue;
		if (strstr(l_ptr->o_name, "{special")) continue;
		if (strstr(l_ptr->o_name, "{terrible")) continue;

		/* Get sval */
		sval = k_info[l_ptr->k_idx].sval;

		/* never crush cool stuff that we might be needing later */
		if ((l_ptr->tval == TV_POTION && sval == SV_POTION_RESTORE_MANA) &&
			(borg_skill[BI_MAXSP] >= 1)) continue;
		if ((l_ptr->tval == TV_POTION) && (sval == SV_POTION_HEALING)) continue;
		if ((l_ptr->tval == TV_POTION) &&
			(sval == SV_POTION_STAR_HEALING)) continue;
		if (l_ptr->tval == TV_POTION && sval == SV_POTION_LIFE) continue;
		if (l_ptr->tval == TV_POTION && sval == SV_POTION_SPEED) continue;
		if (l_ptr->tval == TV_SCROLL &&
			sval == SV_SCROLL_PROTECTION_FROM_EVIL) continue;
		if (l_ptr->tval == TV_SCROLL &&
			sval == SV_SCROLL_RUNE_OF_PROTECTION) continue;
		if (l_ptr->tval == TV_SCROLL && sval == SV_SCROLL_TELEPORT_LEVEL &&
			borg_skill[BI_ATELEPORTLVL] < 1000) continue;
		if (l_ptr->tval == TV_ROD && (sval == SV_ROD_HEALING ||
									  sval == SV_ROD_MAPPING) &&
			borg_class == CLASS_WARRIOR && l_ptr->number <= 5) continue;


		/* Pretend item isn't there */
		l_ptr->treat_as = TREAT_AS_SWAP;

		/* Fix later */
		fix = TRUE;

		/* Examine the inventory */
		borg_notice();

		/* Evaluate the inventory */
		p = borg_power();

		/* Restore item */
		l_ptr->treat_as = TREAT_AS_NORM;

		/* Ignore "bad" swaps */
		if ((b_i >= 0) && (p < b_p)) continue;

		/* Maintain the "best" */
		b_i = i;
		b_p = p;
	}

	/* Examine the inventory */
	if (fix) borg_notice();

	/* Attempt to destroy it */
	if (b_i >= 0)
	{
		list_item *l_ptr = &inventory[b_i];

		/* Try to consume the junk */
		if (borg_consume(l_ptr)) return (TRUE);

		/* Message */
		borg_note(format("# Destroying %s.", l_ptr->o_name));

		/* Destroy all items */
		borg_keypresses("099");

		/* Destroy that item */
		borg_keypress('k');
		borg_keypress(I2A(b_i));

		/* Verify destruction */
		borg_keypress('y');

		/* Success */
		return (TRUE);
	}

	/* Hack -- no need */
	borg_do_crush_hole = FALSE;


	/* Paranoia */
	return (FALSE);
}



/*
 * Destroy "junk" when slow (in the dungeon).
 *
 * We penalize the loss of power, and reward
 * the loss of weight that may be slowing us down.
 */
bool borg_crush_slow(void)
{
	int i, b_i = -1;
	s32b p, b_p = 0L;

	bool fix = FALSE;

	/* No crush if even slightly dangerous */
	if (borg_danger(c_x, c_y, 1, TRUE) >
		borg_skill[BI_CURHP] / 20) return (FALSE);

	/* Hack -- never in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);

	/* Do not crush items unless we are slow */
	if (borg_skill[BI_SPEED] >= 110) return (FALSE);

	/* Scan for junk */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Skip "good" unknown items (unless "icky") */
		if (!(l_ptr->info & OB_KNOWN) && !borg_item_icky(l_ptr)) continue;

		/* Hack -- Skip artifacts */
		if (l_ptr->xtra_name && !(l_ptr->info & OB_MENTAL)) continue;
		if (strstr(l_ptr->o_name, "{special")) continue;
		if (strstr(l_ptr->o_name, "{terrible")) continue;

		if (l_ptr->number > 1)
		{
			/* Pretend item is less */
			l_ptr->treat_as = TREAT_AS_LESS;
		}
		else
		{
			/* Pretend item isn't there */
			l_ptr->treat_as = TREAT_AS_GONE;
		}

		/* Fix later */
		fix = TRUE;

		/* Examine the inventory */
		borg_notice();

		/* Evaluate the inventory */
		p = borg_power();

		/* Restore item */
		l_ptr->treat_as = TREAT_AS_NORM;

		/* Reward getting rid of heavy items */
		p += (l_ptr->weight * 100);

		/* Ignore "bad" swaps */
		if (p < b_p) continue;

		/* Maintain the "best" */
		b_i = i;
		b_p = p;
	}

	/* Examine the inventory */
	if (fix) borg_notice();

	/* Destroy "useless" things */
	if ((b_i >= 0) && (b_p >= (my_power)))
	{
		list_item *l_ptr = &inventory[b_i];

		/* Attempt to consume it */
		if (borg_consume(l_ptr)) return (TRUE);

		/* Message */
		borg_note(format("# Destroying %s.", l_ptr->o_name));

		/* Destroy one item */
		borg_keypress('0');
		borg_keypress('1');

		/* Destroy that item */
		borg_keypress('k');
		borg_keypress(I2A(b_i));

		/* Verify destruction */
		borg_keypress('y');
	}

	/* Hack -- no need */
	borg_do_crush_slow = FALSE;


	/* Nothing to destroy */
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
 *
 * This function is also repeated to *ID* objects.  Right now only objects
 * with random high resist or random powers are *ID*'d
 */
bool borg_test_stuff(bool star_id)
{
	int i, b_i = -1;
	s32b v, b_v = -1;

	bool inven = FALSE;

	/* don't ID stuff when you can't recover spent spell point immediately */
	if ((!star_id) &&
		((borg_skill[BI_CURSP] < 50 && borg_spell_legal(REALM_ARCANE, 3, 2)) ||
		 (borg_skill[BI_CURSP] < 50 && borg_spell_legal(REALM_SORCERY, 1, 1)))
		&& !borg_check_rest())
		return (FALSE);

	/* No ID if in danger */
	if (borg_danger(c_x, c_y, 1, TRUE) > 1) return (FALSE);

	/* Look for an item to identify (equipment) */
	for (i = 0; i < equip_num; i++)
	{
		list_item *l_ptr = &equipment[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Skip known items */
		if (!star_id)
		{
			if (l_ptr->info & OB_KNOWN) continue;
		}
		else
		{
			/* go ahead and check egos and artifacts */
			if (l_ptr->info & OB_MENTAL) continue;
			if (!l_ptr->xtra_name) continue;
		}

		/* Track it */
		b_i = i;
		b_v = 50000L;

		break;
	}

	/* Look for an ego or artifact item to identify (inventory) */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip known items */
		if (!star_id)
		{
			if (l_ptr->info & OB_KNOWN) continue;
		}
		else
		{
			if (l_ptr->info & OB_MENTAL) continue;
			if (l_ptr->xtra_name) break;
		}

		/* Assume nothing */
		v = 0;

		/* Identify "good" (and "terrible") items */
		/* weak pseudo id */
		if (strstr(l_ptr->o_name, "{good") && (borg_class == CLASS_MAGE ||
											   borg_class == CLASS_PRIEST ||
											   borg_class == CLASS_RANGER)) v =
				10000L;
		/* heavy pseudo id */
		else if (strstr(l_ptr->o_name, "{good") && borg_gold < 10000) v = 1000L;
		else if (strstr(l_ptr->o_name, "{excellent")) v = 20000L;
		else if (strstr(l_ptr->o_name, "{special")) v = 50000L;
		else if (strstr(l_ptr->o_name, "{terrible")) v = 50000L;

		/* Hack -- reward "unaware" items */
		if (!l_ptr->k_idx && !star_id)
		{
			/* Analyze the type */
			switch (l_ptr->tval)
			{
				case TV_RING:
				case TV_AMULET:
				{

					/* Hack -- reward depth */
					v += (borg_skill[BI_MAXDEPTH] * 5000L);

					break;
				}

				case TV_ROD:
				{

					/* Hack -- reward depth */
					v += (borg_skill[BI_MAXDEPTH] * 3000L);

					break;
				}

				case TV_WAND:
				case TV_STAFF:
				{

					/* Hack -- reward depth */
					v += (borg_skill[BI_MAXDEPTH] * 2000L);

					break;
				}

				case TV_POTION:
				case TV_SCROLL:
				{
					/* Hack -- reward depth */
					v += (borg_skill[BI_MAXDEPTH] * 500L);

					break;
				}

				case TV_FOOD:
				{

					/* Hack -- reward depth */
					v += (borg_skill[BI_MAXDEPTH] * 10L);

					break;
				}
			}
		}

		/* Nothing */
		if (!v) continue;

		/* Track the best */
		if (v < b_v) continue;

		/* Track it */
		b_i = i;
		b_v = v;

		inven = TRUE;
	}

	/* Found something */
	if (b_i >= 0)
	{
		if (inven)
		{
			list_item *l_ptr = &inventory[b_i];

			if (star_id)
			{
				if (borg_spell(REALM_SORCERY, 1, 7) ||
					borg_spell(REALM_NATURE, 2, 5) ||
					borg_spell(REALM_DEATH, 3, 2) ||
					borg_read_scroll(SV_SCROLL_STAR_IDENTIFY))
				{
					/* Log -- may be cancelled */
					borg_note(format("# *IDENTIFY*ing %s.", l_ptr->o_name));


					/* Select the item */
					borg_keypress(I2A(b_i));

					/* press enter a few time (get rid of display) */
					borg_keypress('\r');
					borg_keypress('\r');
					borg_keypress('\r');
					borg_keypress('\r');
					borg_keypress(ESCAPE);

					/* Success */
					return (TRUE);
				}

			}
			else
			{
				/* Use a Spell/Prayer/Rod/Staff/Scroll of Identify */
				if (borg_spell(REALM_SORCERY, 1, 1) ||
					borg_spell(REALM_ARCANE, 3, 2) ||
					borg_mindcr(MIND_PSYCHOMETRY, 40) ||
					borg_zap_rod(SV_ROD_IDENTIFY) ||
					borg_use_staff(SV_STAFF_IDENTIFY) ||
					borg_activate_artifact(ART_ERIRIL, FALSE) ||
					borg_read_scroll(SV_SCROLL_IDENTIFY))
				{
					/* Log -- may be cancelled */
					borg_note(format("# Identifying %s.", l_ptr->o_name));

					/* Select the item */
					borg_keypress(I2A(b_i));

					borg_keypress(ESCAPE);

					/* Success */
					return (TRUE);
				}
			}
		}
		else
		{
			list_item *l_ptr = &equipment[b_i];

			if (star_id)
			{
				if (borg_spell(REALM_SORCERY, 1, 7) ||
					borg_spell(REALM_NATURE, 2, 5) ||
					borg_spell(REALM_DEATH, 3, 2) ||
					borg_read_scroll(SV_SCROLL_STAR_IDENTIFY))
				{
					/* Log -- may be cancelled */
					borg_note(format("# *IDENTIFY*ing %s.", l_ptr->o_name));

					/* Select the equipment */
					borg_keypress('/');

					/* Select the item */
					borg_keypress(I2A(b_i));

					/* HACK need to recheck stats if we id something on us. */
					for (i = 0; i < 6; i++)
					{
						my_need_stat_check[i] = TRUE;
						my_stat_max[i] = 0;
					}

					/* press enter a few time (get rid of display) */
					borg_keypress('\r');
					borg_keypress('\r');
					borg_keypress('\r');
					borg_keypress('\r');
					borg_keypress(ESCAPE);

					/* Success */
					return (TRUE);
				}

			}
			else
			{
				/* Use a Spell/Prayer/Rod/Staff/Scroll of Identify */
				if (borg_spell(REALM_SORCERY, 1, 1) ||
					borg_spell(REALM_ARCANE, 3, 2) ||
					borg_mindcr(MIND_PSYCHOMETRY, 40) ||
					borg_zap_rod(SV_ROD_IDENTIFY) ||
					borg_use_staff(SV_STAFF_IDENTIFY) ||
					borg_activate_artifact(ART_ERIRIL, FALSE) ||
					borg_read_scroll(SV_SCROLL_IDENTIFY))
				{
					/* Log -- may be cancelled */
					borg_note(format("# Identifying %s.", l_ptr->o_name));

					/* Select the equipment */
					borg_keypress('/');

					/* Select the item */
					borg_keypress(I2A(b_i));

					/* HACK need to recheck stats if we id something on us. */
					for (i = 0; i < 6; i++)
					{
						my_need_stat_check[i] = TRUE;
						my_stat_max[i] = 0;
					}

					borg_keypress(ESCAPE);

					/* Success */
					return (TRUE);
				}
			}
		}
	}

	/* Nothing to do */
	return (FALSE);
}


/*
 * Place our "best" ring on the "tight" finger if needed
 *
 * This function is adopted from "borg_wear_stuff()"
 *
 * Basically, we evaluate the world in which each ring is added
 * to the current set of equipment, and we wear the ring, if any,
 * that gives us the most "power".
 *
 * The "borg_swap_rings()" code above occasionally allows us to remove
 * both rings, at which point this function will place the "best" ring
 * on the "tight" finger, and then the "borg_best_stuff()" function will
 * allow us to put on our second "best" ring on the "loose" finger.
 */
static bool borg_wear_rings(void)
{
	int slot;

	s32b p, b_p = my_power;

	int i, b_i = -1;
	
	bool hand = FALSE;

	list_item *l_ptr;

	bool fix = FALSE;


	/* Require no rings */
	if (equipment[EQUIP_LEFT].number) return (FALSE);
	if (equipment[EQUIP_RIGHT].number) return (FALSE);

	/* Require two empty slots */
	if (inven_num >= INVEN_PACK - 2) return (FALSE);

	/* Scan inventory */
	for (i = 0; i < inven_num; i++)
	{
		l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Not cursed items */
		if (l_ptr->kn_flags3 & TR3_CURSED) continue;

		/* skip artifact rings not star id'd  */
		if ((l_ptr->kn_flags3 & TR3_INSTA_ART) &&
			!(l_ptr->info & OB_MENTAL)) continue;

		/* Where does it go */
		slot = borg_wield_slot(l_ptr);

		/* Only process "rings" */
		if (slot != EQUIP_LEFT) continue;

		/* Test both fingers */
		for (; slot <= EQUIP_RIGHT; slot++)
		{
			/* Pretend to move item into equipment slot */
			equipment[slot].treat_as = TREAT_AS_SWAP;
			l_ptr->treat_as = TREAT_AS_SWAP;

			/* Fix later */
			fix = TRUE;

			/* Examine the inventory */
			borg_notice();

			/* Evaluate the inventory */
			p = borg_power();

			/* Restore the old items */
			equipment[slot].treat_as = TREAT_AS_NORM;
			l_ptr->treat_as = TREAT_AS_NORM;

			/* Ignore "bad" swaps */
			if ((b_i >= 0) && (p < b_p)) continue;

			/* Maintain the "best" */
			b_i = i;
			b_p = p;
			
			/* On right hand? */
			if (slot == EQUIP_RIGHT)
			{
				hand = TRUE;
			}
			else
			{
				hand = FALSE;
			}
		}
	}

	/* Restore bonuses */
	if (fix) borg_notice();

	/* No item */
	if ((b_i >= 0) && (b_p > my_power))
	{
		/* Get the item */
		l_ptr = &inventory[b_i];

		/* Log */
		borg_note("# Putting on ring.");

		/* Log */
		borg_note(format("# Wearing %s.", l_ptr->o_name));
		
		/* Wear it */
		borg_keypress('w');
		borg_keypress(I2A(b_i));
		
		/* Check for two rings */
		if (equipment[EQUIP_LEFT].number && equipment[EQUIP_RIGHT].number)
		{
			/* On right hand? */
			if (hand)
			{
				borg_keypress('y');
			}
			else
			{
				borg_keypress('n');
			}
		}

		return (TRUE);
	}

	/* Nope */
	return (FALSE);
}

/*
 * Remove useless equipment.
 *
 * Look through the inventory for equipment that is reducing power.
 *
 * Basically, we evaluate the world both with the current set of
 * equipment, and in the alternate world in which various items
 * are removed, and we take
 * one step towards the world in which we have the most "power".
 */
bool borg_remove_stuff(void)
{
	s32b p, b_p = 0L;

	int i, b_i = -1;

	list_item *l_ptr;

	bool fix = FALSE;

	/* Require an empty slot */
	if (inven_num >= INVEN_PACK - 1) return (FALSE);

	/* Start with good power */
	b_p = borg_power();

	/* Scan equip */
	for (i = 0; i < equip_num; i++)
	{
		l_ptr = &equipment[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Require "known" (or average, good, etc) */
		if (!(l_ptr->info & OB_KNOWN) &&
			!strstr(l_ptr->o_name, "{average") &&
			!strstr(l_ptr->o_name, "{good") &&
			!strstr(l_ptr->o_name, "{excellent") &&
			!strstr(l_ptr->o_name, "{special")) continue;

		/* skip it if it has not been decursed */
		if ((l_ptr->kn_flags3 & TR3_CURSED) ||
			(l_ptr->kn_flags3 & TR3_HEAVY_CURSE)) continue;
		
		/* Take it off */
		l_ptr->treat_as = TREAT_AS_SWAP;

		/* Fix later */
		fix = TRUE;

		/* Examine the inventory */
		borg_notice();

		/* Evaluate the inventory */
		p = borg_power();
		
		/* Put it back on */
		l_ptr->treat_as = TREAT_AS_NORM;

		/* Track the crappy items */
		if (p >= b_p)
		{
			b_i = i;
		}
	}

	/* Restore bonuses */
	if (fix) borg_notice();

	/* No item */
	if (b_i >= 0)
	{
		/* Get the item */
		l_ptr = &equipment[b_i];

		/* Log */
		borg_note(format("# Removing %s.", l_ptr->o_name));

		/* Wear it */
		borg_keypress('t');
		borg_keypress(I2A(b_i));

		return (TRUE);
	}

	/* Nope */
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
 *
 * The "borg_swap_rings()" code above occasionally allows us to remove
 * both rings, at which point this function will replace the "best" ring
 * on the "tight" finger, and the second "best" ring on the "loose" finger.
 */
bool borg_wear_stuff(void)
{
	int slot;
	int d;

	s32b p, b_p = 0L;

	int i, b_i = -1;
	int danger;

	list_item *l_ptr;

	bool fix = FALSE;

	/* Require an empty slot */
	if (inven_num >= INVEN_PACK - 1) return (FALSE);

	/* Wear new rings if better */
	if (borg_wear_rings()) return (TRUE);

	/* Scan inventory */
	for (i = 0; i < inven_num; i++)
	{
		l_ptr = &inventory[i];

		/* Skip empty / unaware items */
		if (!l_ptr->k_idx) continue;

		/* Require "known" (or average, good, etc) */
		if (!(l_ptr->info & OB_KNOWN) &&
			!strstr(l_ptr->o_name, "{average") &&
			!strstr(l_ptr->o_name, "{good") &&
			!strstr(l_ptr->o_name, "{excellent") &&
			!strstr(l_ptr->o_name, "{special")) continue;

		/* apw do not wear not *idd* artifacts */
		if (!(l_ptr->info & OB_MENTAL) && l_ptr->xtra_name) continue;

		/* skip it if it has not been decursed */
		if ((l_ptr->kn_flags3 & TR3_CURSED) || (l_ptr->kn_flags3 & TR3_HEAVY_CURSE)) continue;

		/* Where does it go */
		slot = borg_wield_slot(l_ptr);

		/* Cannot wear this item */
		if (slot < 0) continue;

		/* skip it if it this slot has been decursed */
		if (equipment[slot].kn_flags3 & TR3_CURSED) continue;

		/* Obtain danger */
		danger = borg_danger(c_x, c_y, 1, TRUE);

		/*** Process non-rings ***/
		if (slot != EQUIP_LEFT)
		{
			/* Swap items */
			equipment[slot].treat_as = TREAT_AS_SWAP;
			l_ptr->treat_as = TREAT_AS_SWAP;

			/* Fix later */
			fix = TRUE;

			/* Examine the inventory */
			borg_notice();

			/* Evaluate the inventory */
			p = borg_power();

			/* Evaluate local danger */
			d = borg_danger(c_x, c_y, 1, TRUE);
			
			/* Restore items */
			equipment[slot].treat_as = TREAT_AS_NORM;
			l_ptr->treat_as = TREAT_AS_NORM;

			/* Ignore "bad" swaps */
			if ((b_i >= 0) && (p < b_p)) continue;

			/* Ignore if more dangerous */
			if (danger < d) continue;

			/* Hack -- Ignore "equal" swaps */
			if ((b_i >= 0) && (p == b_p)) continue;

			/* Maintain the "best" */
			b_i = i;
			b_p = p;
		}
	}
	
	/* Restore bonuses */
	if (fix) borg_notice();

	/* No item */
	if ((b_i >= 0) && (b_p > my_power))
	{
		/* Get the item */
		l_ptr = &inventory[b_i];

		/* Log */
		borg_note(format("# Wearing %s.  Old Power (%ld) New Power (%ld)",
						 l_ptr->o_name, my_power, b_p));

		/* Wear it */
		borg_keypress('w');
		borg_keypress(I2A(b_i));

		return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * Hack -- order of the slots
 *
 * XXX XXX XXX Note that we ignore the "tight" ring, and we
 * assume that we will always be wearing our "best" ring on
 * our "right" (tight) finger, and if we are not, then the
 * "borg_swap_rings()" function will remove both the rings,
 * which will induce the "borg_best_stuff()" function to put
 * the rings back on in the "optimal" order.
 */
static byte borg_best_stuff_order[] =
{
	EQUIP_BOW,
	EQUIP_WIELD,
	EQUIP_BODY,
	EQUIP_OUTER,
	EQUIP_ARM,
	EQUIP_HEAD,
	EQUIP_HANDS,
	EQUIP_FEET,
	EQUIP_LEFT,
	EQUIP_LITE,
	EQUIP_NECK,

	255
};


/*
 * Helper function (see below)
 */
static void borg_best_stuff_aux(int n, byte *test, byte *best, s32b *vp)
{
	int i;

	int slot;


	/* Extract the slot */
	slot = borg_best_stuff_order[n];

	/* All done */
	if (slot == 255)
	{
		s32b p;

		/* Examine */
		borg_notice();

		/* Evaluate */
		p = borg_power();

		/* Track best */
		if (p > *vp)
		{

#if 0
			/* dump list and power...  for debugging */
			borg_note(format("Trying Combo (best power %ld)", *vp));
			borg_note(format("             (borg_power %ld)", p));
			for (i = 0; i < INVEN_PACK; i++)
				borg_note(format("inv %d %s.", i, borg_items[i].desc));
			for (i = 0; borg_best_stuff_order[i] != 255; i++)
				borg_note(format("stuff %s.",
								 borg_items[borg_best_stuff_order[i]].desc));
#endif
			/* Save the results */
			for (i = 0; i < n; i++) best[i] = test[i];

			/* Use it */
			*vp = p;
		}

		/* Success */
		return;
	}


	/* Note the attempt */
	test[n] = slot;

	/* Evaluate the default item */
	borg_best_stuff_aux(n + 1, test, best, vp);


	/* Try other possible objects */
	for (i = 0; i <
#if 0
		 ((shop_num == BORG_HOME) ? (INVEN_PACK + STORE_INVEN_MAX) : INVEN_PACK)
#endif
		 INVEN_PACK; i++)
	{
		borg_item *item;
#if 0
		if (i < INVEN_PACK)
			item = &borg_items[i];
		else
			item = &borg_shops[BORG_HOME].ware[i - INVEN_PACK];
#endif
		item = &borg_items[i];

		/* Skip empty items */
		if (!item->iqty) continue;

		/* Require "aware" */
		if (!item->kind) continue;

		/* Require "known" (or average, good, etc) */
		if (!item->able &&
			!strstr(item->desc, "{average") &&
			!strstr(item->desc, "{good") &&
			!strstr(item->desc, "{excellent") &&
			!strstr(item->desc, "{special")) continue;

		/* Hack -- ignore "worthless" items */
		if (!item->value) continue;

		/* Skip it if it has not been decursed */
		if ((item->cursed) || (item->flags3 & TR3_HEAVY_CURSE)) continue;

		/* Make sure it goes in this slot, special consideration
		 * for checking rings
		 */
		if (slot != borg_wield_slot(l_ptr)) continue;

		/* Make sure that slot does not have a cursed item */
		if ((borg_items[slot].cursed) ||
			(borg_items[slot].flags3 & TR3_HEAVY_CURSE)) continue;

		/* Wear the new item */
		COPY(&borg_items[slot], item, borg_item);

		/* Note the attempt */
		if (i < INVEN_PACK)
			test[n] = i;
		else
			/* if in home, note by adding 100 to item number. */
			test[n] = (i - INVEN_PACK) + 100;


		/* Evaluate the possible item */
		borg_best_stuff_aux(n + 1, test, best, vp);

		/* Restore equipment */
		COPY(&borg_items[slot], &safe_items[slot], borg_item);
	}
}


/*
 * Attempt to instantiate the *best* possible equipment.
 */
bool borg_best_stuff(void)
{
#if 0
	int hole = INVEN_PACK - 1;
#endif

	int k;

	s32b value;

	int i;

	byte test[12];
	byte best[12];

	/* Hack -- Initialize */
	for (k = 0; k < 12; k++)
	{
		/* Initialize */
		best[k] = test[k] = 255;
	}

	/* Hack -- Copy all the slots */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		/* Save the item */
		COPY(&safe_items[i], &borg_items[i], borg_item);
	}

#if 0
	if (shop_num == BORG_HOME)
	{
		/* Hack -- Copy all the store slots */
		for (i = 0; i < STORE_INVEN_MAX; i++)
		{
			/* Save the item */
			COPY(&safe_home[i], &borg_shops[BORG_HOME].ware[i], borg_item);
		}
	}
#endif

	/* Evaluate the inventory */
	value = my_power;

	/* Determine the best possible equipment */
	(void)borg_best_stuff_aux(0, test, best, &value);

	/* Restore bonuses */
	borg_notice();

	/* Make first change. */
	for (k = 0; k < 12; k++)
	{
		/* Get choice */
		i = best[k];

		/* Ignore non-changes */
		if (i == borg_best_stuff_order[k] || 255 == i) continue;

		if (i < 100)
		{
			/* weild the item */
			borg_item *item = &borg_items[i];
			borg_note(format("# Best Combo %s.", item->desc));

			borg_keypress('w');
			borg_keypress(I2A(i));
			return (TRUE);
		}
		else
		{
#if 0
			borg_item *item;

			/* can't get an item if full. */
			if (borg_items[hole].iqty) return (FALSE);

			i -= 100;

			item = &borg_shops[BORG_HOME].ware[i];

			/* Dont do it if you just sold this item */
			if (sold_item_tval == item->tval && sold_item_sval == item->sval &&
				sold_item_pval == item->pval &&
				sold_item_store == BORG_HOME) return (FALSE);

			/* Get the item */
			borg_note(format("# Getting (Best Fit) %s.", item->desc));

			/* Minor Hack -- Go to the correct page */
			if ((i / 12) != borg_shops[BORG_HOME].page)
			{
				borg_keypress(' ');
			}

			borg_keypress('p');
			borg_keypress(I2A(i % 12));

			/* press enter a few time (mulitple objects) */
			borg_keypress('\r');
			borg_keypress('\r');
			borg_keypress('\r');
			borg_keypress('\r');

			return (TRUE);
#endif /* 0 */
		}
	}

	/* Nope */
	return (FALSE);
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
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (FALSE);

	/* Dark */
	if (!borg_skill[BI_CUR_LITE]) return (FALSE);

	/* Check each realm, backwards */
	for (realm = MAX_REALM; realm > 0; realm--)
	{
		/* skip non my realms */
		if (realm != borg_skill[BI_REALM1] &&
			realm != borg_skill[BI_REALM2]) continue;


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

				/* Skip "worse" spells/prayers */
				if (rate <= b_rate) continue;

				/* Track it */
				b_inven = inven;
				b_rate = rate;
				b_realm = realm;
				b_book = book;
				b_spell = spell;
			}
		}						/* book */
	}							/* realm  */

	/* Study */
	if (borg_skill[BI_ISSTUDY] && (b_rate > 0))
	{

		/* Realm */
		borg_magic *as = &borg_magics[b_realm][b_book][b_spell];

		/* Debugging Info */
		borg_note(format("# Studying %s spell %s.", as->realm_name, as->name));

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
	if (borg_skill[BI_CDEPTH]) return (FALSE);

	/* Hack -- only when bored */
	if (!bored) return (FALSE);


	/* Check each realm backwards */
	for (realm = MAX_REALM; realm > 0; realm--)
	{
		/* Check each book (backwards) */
		for (book = 3; book >= 0; book--)
		{

			/* Only my realms */
			if (realm != borg_skill[BI_REALM1] &&
				realm != borg_skill[BI_REALM2]) continue;

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
				if (borg_skill[BI_CURSP] < as->power) continue;

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
						/* Hack -- target self */
						borg_keypress('t');
					}

					/* Success */
					return (TRUE);
				}
			}					/* spells */
		}						/* books */
	}							/* realm */

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
	for (i = 0; i < INVEN_PACK; i++)
	{
		borg_item *item = &borg_items[i];

		/* Skip empty items */
		if (!item->iqty) continue;
		if (!item->kind) continue;

		/* Skip "crappy" items */
		if (item->value <= 0) continue;

		/* Obtain the base price */
		price = ((item->value < 30000L) ? item->value : 30000L);

		/* Skip cheap "known" (or "average") items */
		if ((price * item->iqty < greed) &&
			(item->able || strstr(item->desc, "{average"))) continue;

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
	if (borg_skill[BI_ISWEAK]) return (FALSE);

	/* Look for an (wearable- non rod) item to recharge */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		borg_item *item = &borg_items[i];
		object_type *o_ptr;	/* cheat */
		o_ptr = &inventory[i];	/* cheat */

		/* Skip empty items */
		if (!item->iqty) continue;
		if (!item->kind) continue;

		/* skip items that are charged */
		if (!item->timeout) continue;

		/* skip lites */
		if (item->tval == TV_LITE) continue;

		/* Cheat-- the borg is misreading his equip.
		 * So this is pulling the info right from the game.
		 */
		if (!o_ptr->timeout) continue;

		/* Where does this belong? */
		slot = borg_wield_slot(l_ptr);

		/* Skip stuff that can't be worn */
		if (slot < INVEN_WIELD) continue;

		/* note this one */
		b_i = i;
	}

	if (b_i >= INVEN_WIELD)
	{
		/* Item is worn, no swap is nec. */
		borg_note(format
				  ("# Waiting for '%s' to Recharge.", borg_items[b_i].desc));

		/* Rest for a while */
		borg_keypress('R');
		borg_keypress('7');
		borg_keypress('5');
		borg_keypress('\n');

		/* done */
		return (TRUE);
	}
	/* Item must be worn to be recharged
	 * But, none if some equip is cursed
	 */
	else if (b_i >= INVEN_WIELD && b_i <= INVEN_PACK && !borg_wearing_cursed)
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
	if (!borg_skill[BI_CDEPTH])
	{
		/* Cancel rising */
		goal_rising = FALSE;

		/* Wait until bored */
		if (!bored) return (FALSE);

		/* Case for those who cannot Teleport Level */
		if (borg_skill[BI_MAXDEPTH] == 100 && !borg_plays_risky)
		{
			if (borg_skill[BI_ATELEPORTLVL] == 0)
			{
				/* These pple must crawl down to 100, Sorry */
				goal_fleeing = TRUE;
				goal_leaving = TRUE;
				stair_more = TRUE;

				/* Note */
				borg_note
					("# Borg must crawl to deep dungeon- no recall to 100.");

				/* Attempt to use those stairs */
				if (borg_flow_stair_more(GOAL_BORE)) return (TRUE);

				/* Oops */
				return (FALSE);
			}
		}


		/* Hack -- Recall into dungeon */
		if ((borg_skill[BI_MAXDEPTH] >= 5) &&
			(borg_skill[BI_RECALL] >= 6) && borg_recall())
		{
			/* Note */
			borg_note("# Recalling into dungeon.");

			/* Give it a shot */
			return (TRUE);
		}
		else
		{
			/* note why we didn't recall. */
			if (borg_skill[BI_MAXDEPTH] < 5)
				borg_note("# Not deep enough to recall");
			else if (borg_skill[BI_RECALL] <= 2)
				borg_note("# Not enough recalls to recall");
			else
			{
				/* recall unless way out of our league */
				if ((cptr)NULL !=
					borg_prepared(borg_skill[BI_MAXDEPTH] * 6 / 10))
				{
					cptr reason = borg_prepared(borg_skill[BI_MAXDEPTH]);
					borg_slow_return = TRUE;
					borg_note(format
							  ("# Way too scary to recall down there!   %s",
							   reason));
					borg_slow_return = FALSE;
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
	if ((cptr)NULL == borg_prepared(borg_skill[BI_CDEPTH] + 1)) g = 1;

	/* Count sellable items */
	k = borg_count_sell();

	/* Do not dive when "full" of items */
	if (g && (k >= 12)) g = 0;

	/* Do not dive when drained */
	if (g && borg_skill[BI_ISFIXEXP]) g = 0;


	/* Hack -- Stay on each level for a minimal amount of time */
	if (borg_skill[BI_CLEVEL] > 10 &&
		g != 0 && (borg_t - borg_began < value_feeling[borg_feeling]))
	{
		g = 0;
	}

	/* Rise a level if bored and unable to dive. */
	if (bored && ((cptr)NULL != borg_prepared(borg_skill[BI_CDEPTH] + 1)))
	{
		cptr reason = borg_prepared(borg_skill[BI_CDEPTH] + 1);
		g = -1;
		borg_slow_return = TRUE;
		borg_note(format("# heading up (bored and unable to dive: %s)",
						 reason));
		borg_slow_return = FALSE;
	}

	/* Power dive if I am playing too shallow */
	if ((cptr)NULL == borg_prepared(borg_skill[BI_CDEPTH] + 5)) g = 1;

	/* Hack -- Power-climb upwards when needed */
	if ((cptr)NULL != borg_prepared(borg_skill[BI_CDEPTH]) && !unique_on_level)
	{
		cptr reason = borg_prepared(borg_skill[BI_CDEPTH]);

		borg_slow_return = TRUE;
		borg_note(format("# heading up (too deep: %s)", reason));
		borg_slow_return = FALSE;
		g = -1;

		/* if I must restock go to town */
		if ((cptr)NULL != borg_restock(borg_skill[BI_CDEPTH]))
		{
			cptr reason = borg_prepared(borg_skill[BI_CDEPTH]);

			borg_note(format("# returning to town to restock(too deep: %s)",
							 reason));
			goal_rising = TRUE;
		}

		/* if I am really out of depth go to town */
		if ((cptr)NULL != borg_prepared(borg_skill[BI_MAXDEPTH] * 5 / 10))
		{
			cptr reason = borg_prepared(borg_skill[BI_CDEPTH]);
			borg_slow_return = TRUE;
			borg_note(format("# returning to town (too deep: %s)", reason));
			goal_rising = TRUE;
			borg_slow_return = FALSE;
		}
	}

	/* Hack -- if I am playing way too shallow return to town */
	if ((cptr)NULL == borg_prepared(borg_skill[BI_CDEPTH] + 20) &&
		(cptr)NULL == borg_prepared(borg_skill[BI_MAXDEPTH] * 6 / 10) &&
		borg_skill[BI_MAXDEPTH] > borg_skill[BI_CDEPTH] + 10)
	{
		borg_note("# returning to town to recall back down (too shallow)");
		goal_rising = TRUE;
	}

	/* Hack -- It is much safer to scum for items on 98
	 * Check to see if depth 99, if Sauron is dead and Im not read to fight
	 * the final battle
	 */
	if (borg_skill[BI_CDEPTH] == 99 && borg_race_death[860] == 1 &&
		borg_ready_morgoth != 1)
	{
		borg_note("# Returning to level 98 to scum for items.");
		g = -1;
	}

	/* Power dive too 100 if ready */
	if ((cptr)NULL == borg_prepared(100)) g = 1;

	/* Power dive if Morgoth is dead */
	if (borg_skill[BI_KING]) g = 1;

	/* Return to town to sell stuff */
	if (bored && (k >= 12))
	{
		borg_note("# Going to town (Sell Stuff).");
		goal_rising = TRUE;
	}

	/* Return to town when level drained */
	if (borg_skill[BI_ISFIXLEV])
	{
		borg_note("# Going to town (Fix Level).");
		goal_rising = TRUE;
	}

	/* Return to town to restore experience */
	if (bored && borg_skill[BI_ISFIXEXP] && borg_skill[BI_CLEVEL] != 50)
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
	if (borg_skill[BI_MAXDEPTH] >= borg_skill[BI_CDEPTH] + 25 &&
		borg_skill[BI_CDEPTH] < 9 &&
		borg_time_town + borg_t - borg_began > 3500)
	{
		borg_note("# Going to town (scumming check).");
		goal_rising = TRUE;
	}

	/* if returning to town, try to go upstairs */
	if (goal_rising) g = -1;

	/* Mega-Hack -- spend time on the first level to rotate shops */
	if (borg_skill[BI_CLEVEL] > 10 &&
		(borg_skill[BI_CDEPTH] == 1) && (borg_t - borg_began < 100) && (g < 0))
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
			(borg_skill[BI_CDEPTH] >= 5) && borg_recall())
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
