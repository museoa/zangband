/* File: borg6.c */
/* Purpose: Medium level stuff for the Borg -BEN- */

#include "angband.h"

#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"
#include "zborg4.h"
#include "zborg5.h"
#include "zborg6.h"

static bool borg_desperate = FALSE;



/*
 * This file is responsible for the low level dungeon goals.
 *
 * This includes calculating the danger from monsters, determining
 * how and when to attack monsters, and calculating "flow" paths
 * from place to place for various reasons.
 *
 * Notes:
 *   We assume that invisible/offscreen monsters are dangerous
 *   We consider physical attacks, missile attacks, spell attacks,
 *     wand attacks, etc, as variations on a single theme.
 *   We take account of monster resistances and susceptibilities
 *   We try not to wake up sleeping monsters by throwing things
 *
 *
 * Bugs:
 *   Currently the "twitchy()" function is not very smart
 *   We get "twitchy" when we are afraid of the monsters
 *   Annoyance and Danger are very different things (!)
 */



/*
 * Given a "source" and "target" locations, extract a "direction",
 * which will move one step from the "source" towards the "target".
 *
 * Note that we use "diagonal" motion whenever possible.
 *
 * We return "5" if no motion is needed.
 */
static int borg_extract_dir(int y1, int x1, int y2, int x2)
{
	/* No movement required */
	if ((y1 == y2) && (x1 == x2)) return (5);

	/* South or North */
	if (x1 == x2) return ((y1 < y2) ? 2 : 8);

	/* East or West */
	if (y1 == y2) return ((x1 < x2) ? 6 : 4);

	/* South-east or South-west */
	if (y1 < y2) return ((x1 < x2) ? 3 : 1);

	/* North-east or North-west */
	if (y1 > y2) return ((x1 < x2) ? 9 : 7);

	/* Paranoia */
	return (5);
}


/*
 * Given a "source" and "target" locations, extract a "direction",
 * which will move one step from the "source" towards the "target".
 *
 * We prefer "non-diagonal" motion, which allows us to save the
 * "diagonal" moves for avoiding pillars and other obstacles.
 *
 * If no "obvious" path is available, we use "borg_extract_dir()".
 *
 * We return "5" if no motion is needed.
 */
static int borg_goto_dir(int y1, int x1, int y2, int x2)
{
	int d, e;

	int ay = (y2 > y1) ? (y2 - y1) : (y1 - y2);
	int ax = (x2 > x1) ? (x2 - x1) : (x1 - x2);

	map_block *mb_ptr;


	/* Default direction */
	e = borg_extract_dir(y1, x1, y2, x2);


	/* Adjacent location, use default */
	if ((ay <= 1) && (ay <= 1)) return (e);


	/* Try south/north (primary) */
	if (ay > ax)
	{
		d = (y1 < y2) ? 2 : 8;

		/* Bounds checking */
		if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
		{
			mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

			if (borg_cave_floor_grid(mb_ptr)) return (d);
		}
	}

	/* Try east/west (primary) */
	if (ay < ax)
	{
		d = (x1 < x2) ? 6 : 4;

		/* Bounds checking */
		if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
		{
			mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

			if (borg_cave_floor_grid(mb_ptr)) return (d);
		}
	}


	/* Try diagonal */
	d = borg_extract_dir(y1, x1, y2, x2);

	/* Check for walls */

	/* Bounds checking */
	if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
	{
		mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

		if (borg_cave_floor_grid(mb_ptr)) return (d);
	}

	/* Try south/north (secondary) */
	if (ay <= ax)
	{
		d = (y1 < y2) ? 2 : 8;

		/* Bounds checking */
		if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
		{
			mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

			if (borg_cave_floor_grid(mb_ptr)) return (d);
		}
	}

	/* Try east/west (secondary) */
	if (ay >= ax)
	{
		d = (x1 < x2) ? 6 : 4;

		/* Bounds checking */
		if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
		{
			mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

			if (borg_cave_floor_grid(mb_ptr)) return (d);
		}
	}


	/* Circle obstacles */
	if (!ay)
	{
		/* Circle to the south */
		d = (x1 < x2) ? 3 : 1;

		/* Bounds checking */
		if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
		{
			mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

			if (borg_cave_floor_grid(mb_ptr)) return (d);
		}

		/* Circle to the north */
		d = (x1 < x2) ? 9 : 7;

		/* Bounds checking */
		if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
		{
			mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

			if (borg_cave_floor_grid(mb_ptr)) return (d);
		}
	}

	/* Circle obstacles */
	if (!ax)
	{
		/* Circle to the east */
		d = (y1 < y2) ? 3 : 9;

		/* Bounds checking */
		if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
		{
			mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

			if (borg_cave_floor_grid(mb_ptr)) return (d);
		}

		/* Circle to the west */
		d = (y1 < y2) ? 1 : 7;

		/* Bounds checking */
		if (map_in_bounds(x1 + ddx[d], y1 + ddy[d]))
		{
			mb_ptr = map_loc(x1 + ddx[d], y1 + ddy[d]);

			if (borg_cave_floor_grid(mb_ptr)) return (d);
		}
	}


	/* Oops */
	return (e);
}



/*
 * Clear the "flow" information
 *
 * This function was once a major bottleneck, so we now use several
 * slightly bizarre, but highly optimized, memory copying methods.
 */
static void borg_flow_clear(void)
{
	map_block *mb_ptr;

	/* Itterate over the map */
	MAP_ITT_START (mb_ptr)
	{
		mb_ptr->cost = 255;

		if (borg_danger_wipe)
		{
			/* Clear the "icky" + "know" flags */
			mb_ptr->info &= ~(BORG_MAP_ICKY | BORG_MAP_KNOW);
		}
	}
	MAP_ITT_END;

	/* Wipe complete */
	borg_danger_wipe = FALSE;

	/* Start over */
	flow_head = 0;
	flow_tail = 0;
}




/*
 * Spread a "flow" from the "destination" grids outwards
 *
 * We fill in the "cost" field of every grid that the player can
 * "reach" with the number of steps needed to reach that grid,
 * if the grid is "reachable", and otherwise, with "255", which
 * is the largest possible value that can be stored in a byte.
 *
 * Thus, certain grids which are actually "reachable" but only by
 * a path which is at least 255 steps in length will thus appear
 * to be "unreachable", but this is not a major concern.
 *
 * We use the "flow" array as a "circular queue", and thus we must
 * be careful not to allow the "queue" to "overflow".  This could
 * only happen with a large number of distinct destination points,
 * each several units away from every other destination point, and
 * in a dungeon with no walls and no dangerous monsters.  But this
 * is technically possible, so we must check for it just in case.
 *
 * We do not need a "priority queue" because the cost from grid to
 * grid is always "one" and we process them in order.  If we did
 * use a priority queue, this function might become unusably slow,
 * unless we reactivated the "room building" code.
 *
 * We handle both "walls" and "danger" by marking every grid which
 * is "impassible", due to either walls, or danger, as "ICKY", and
 * marking every grid which has been "checked" as "KNOW", allowing
 * us to only check the wall/danger status of any grid once.  This
 * provides some important optimization, since many "flows" can be
 * done before the "ICKY" and "KNOW" flags must be reset.
 *
 * Note that the "borg_enqueue_grid()" function should refuse to
 * enqueue "dangeous" destination grids, but does not need to set
 * the "KNOW" or "ICKY" flags, since having a "cost" field of zero
 * means that these grids will never be queued again.  In fact,
 * the "borg_enqueue_grid()" function can be used to enqueue grids
 * which are "walls", such as "doors" or "rubble".
 *
 * This function is extremely expensive, and is a major bottleneck
 * in the code, due more to internal processing than to the use of
 * the "borg_danger()" function, especially now that the use of the
 * "borg_danger()" function has been optimized several times.
 *
 * The "optimize" flag allows this function to stop as soon as it
 * finds any path which reaches the player, since in general we are
 * looking for paths to destination grids which the player can take,
 * and we can stop this function as soon as we find any usable path,
 * since it will always be as short a path as possible.
 *
 * We queue the "children" in reverse order, to allow any "diagonal"
 * neighbors to be processed first, since this may boost efficiency.
 *
 * Note that we should recalculate "danger", and reset all "flows"
 * if we notice that a wall has disappeared, and if one appears, we
 * must give it a maximal cost, and mark it as "icky", in case it
 * was currently included in any flow.
 *
 * If a "depth" is given, then the flow will only be spread to that
 * depth, note that the maximum legal value of "depth" is 250.
 */
static void borg_flow_spread(int depth, bool optimize, bool avoid,
                             bool tunneling)
{
	int i;
	int n, o = 0;
	int x1, y1;
	int x, y;

	map_block *mb_ptr = map_loc(c_x, c_y);

	int player_cost = mb_ptr->cost;


	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		x1 = borg_flow_x[flow_tail];
		y1 = borg_flow_y[flow_tail];

		/* Circular queue -- dequeue the next entry */
		if (++flow_tail == AUTO_FLOW_MAX) flow_tail = 0;

		/* Bounds checking */
		if (!map_in_bounds(x1, y1)) continue;

		mb_ptr = map_loc(x1, y1);

		/* Cost (one per movement grid) */
		n = mb_ptr->cost + 1;

		/* New depth */
		if (n > o)
		{
			/* Optimize (if requested) */
			if (optimize && (n > player_cost)) break;

			/* Limit depth */
			if (n > depth) break;

			/* Save */
			o = n;
		}

		/* Queue the "children" */
		for (i = 0; i < 8; i++)
		{
			int old_head;

			map_block *mb_ptr;


			/* Neighbor grid */
			x = x1 + ddx_ddd[i];
			y = y1 + ddy_ddd[i];

			/* only on legal grids */
			if (!map_in_bounds(x, y)) continue;

			/* Access the grid */
			mb_ptr = map_loc(x, y);

			/* Skip "reached" grids */
			if (mb_ptr->cost <= n) continue;

			/* Avoid "wall" grids (not doors) unless tunneling */
			if (!tunneling &&
				(mb_ptr->feat >= FEAT_SECRET &&
				 mb_ptr->feat <= FEAT_WALL_SOLID)) continue;

			/* Avoid pillars */
			if (!tunneling && mb_ptr->feat == FEAT_PILLAR) continue;

			/* Avoid "perma-wall" grids */
			if (mb_ptr->feat >= FEAT_PERM_EXTRA &&
				mb_ptr->feat <= FEAT_PERM_SOLID) continue;

			/* Avoid Lava */
			if ((mb_ptr->feat == FEAT_DEEP_LAVA ||
				 mb_ptr->feat == FEAT_SHAL_LAVA)
				&& !borg_skill[BI_IFIRE]) continue;

			/* Avoid Water if dangerous */
			if (mb_ptr->feat == FEAT_SHAL_WATER &&
				(borg_skill[BI_ENCUMBERD] && !borg_skill[BI_FEATH])) continue;


			/* Avoid Mountains */
			if (mb_ptr->feat == FEAT_MOUNTAIN) continue;

			/* Avoid some other Zang Terrains */

			/* Avoid unknown grids (if requested or retreating) */
			if ((avoid || borg_desperate) && !mb_ptr->feat) continue;

			/* Avoid Monsters if Desprerate */
			if (borg_desperate && (mb_ptr->monster)) continue;

#if 0
			/* Avoid Traps if low level-- unless brave or scaryguy. */
			if (mb_ptr->feat >= FEAT_TRAP_TRAPDOOR &&
				mb_ptr->feat <= FEAT_TRAP_SLEEP &&
				avoidance <= borg_skill[BI_CURHP] && !scaryguy_on_level)
			{
				/* Do not disarm when you could end up dead */
				if (borg_skill[BI_CURHP] < 60) continue;

				/* Do not disarm when clumsy */
				if (borg_skill[BI_DIS] < 30 &&
					borg_skill[BI_CLEVEL] < 20) continue;
				if (borg_skill[BI_DIS] < 45 &&
					borg_skill[BI_CLEVEL] < 10) continue;
			}
#endif /* 0 */


			/* Ignore "icky" grids */
			if (mb_ptr->info & BORG_MAP_ICKY) continue;


			/* Analyze every grid once */
			if (!(mb_ptr->info & BORG_MAP_KNOW))
			{
				int p;

				/* Mark as known */
				mb_ptr->info |= BORG_MAP_KNOW;

				if (!borg_desperate)
				{
					/* Get the danger */
					p = borg_danger(y, x, 1, TRUE);

					/* Dangerous grid */
					if (p > avoidance / 3)
					{
						/* Mark as icky */
						mb_ptr->info |= BORG_MAP_ICKY;

						/* Ignore this grid */
						continue;
					}
				}
			}


			/* Save the flow cost */
			mb_ptr->cost = n;

			/* Enqueue that entry */
			borg_flow_x[flow_head] = x;
			borg_flow_y[flow_head] = y;

			/* Circular queue -- memorize head */
			old_head = flow_head;

			/* Circular queue -- insert with wrap */
			if (++flow_head == AUTO_FLOW_MAX)
				flow_head = 0;

			/* Circular queue -- handle overflow (badly) */
			if (flow_head == flow_tail)
				flow_head = old_head;
		}
	}

	/* Forget the flow info */
	flow_head = flow_tail = 0;
}



/*
 * Enqueue a fresh (legal) starting grid, if it is safe
 */
static void borg_flow_enqueue_grid(int y, int x)
{
	int old_head;

	map_block *mb_ptr;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return;

	mb_ptr = map_loc(x, y);

	/* Avoid icky grids */
	if (mb_ptr->info & BORG_MAP_ICKY) return;

	/* Unknown */
	if (!(mb_ptr->info & BORG_MAP_KNOW))
	{
		/* Mark as known */
		mb_ptr->info |= BORG_MAP_KNOW;

		/* Mark dangerous grids as icky */
		if ((borg_danger(y, x, 1, TRUE) > avoidance / 3) && !borg_desperate)
		{
			/* Icky */
			mb_ptr->info |= BORG_MAP_ICKY;

			/* Avoid */
			return;
		}
	}


	/* Only enqueue a grid once */
	if (mb_ptr->cost == 1) return;


	/* Save the flow cost (zero) */
	mb_ptr->cost = 1;

	/* Enqueue that entry */
	borg_flow_y[flow_head] = y;
	borg_flow_x[flow_head] = x;


	/* Circular queue -- memorize head */
	old_head = flow_head;

	/* Circular queue -- insert with wrap */
	if (++flow_head == AUTO_FLOW_MAX) flow_head = 0;

	/* Circular queue -- handle overflow */
	if (flow_head == flow_tail) flow_head = old_head;

}



/*
 * Do a "reverse" flow from the player outwards
 */
static void borg_flow_reverse(void)
{
	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue the player's grid */
	borg_flow_enqueue_grid(c_y, c_x);

	/* Spread, but do NOT optimize */
	borg_flow_spread(250, FALSE, FALSE, FALSE);
}





/*
 * Attempt to induce "word of recall"
 * artifact activations added throughout this code
 */
bool borg_recall(void)
{

	/* Vampires dont like to Recall up in Daytime */
	if (borg_skill[BI_FEAR_LITE] && borg_skill[BI_CDEPTH] >= 1)
	{
		/* If day time, Recall could be bad */
		if ((borg_skill[BI_HRTIME] >= 5) && (borg_skill[BI_HRTIME] <= 18))
		{
			/* Do not Recall */
			return (FALSE);
		}
	}

	/* Multiple "recall" fails */
	if (!goal_recalling)
	{
		/* Try to "recall" */
		if (borg_zap_rod(SV_ROD_RECALL) ||
			borg_activate_artifact(ART_AVAVIR, FALSE) ||
			borg_activate_artifact(ART_THRAIN, TRUE) ||
			borg_spell_fail(REALM_SORCERY, 2, 7, 60) ||
			borg_spell_fail(REALM_ARCANE, 3, 6, 60) ||
			borg_spell_fail(REALM_TRUMP, 1, 6, 60) ||
			borg_read_scroll(SV_SCROLL_WORD_OF_RECALL))
		{
			/* Success */
			return (TRUE);
		}
	}

	/* Nothing */
	return (FALSE);
}



/*
 * Prevent starvation by any means possible
 */
static bool borg_eat_food_any(void)
{
	int i;

	/* No help for some */
	if (borg_race >= RACE_GOLEM && borg_race <= RACE_SPECTRE)
	{
		return (FALSE);
	}

	/* Scan the inventory for "normal" food */
	for (i = 0; i < INVEN_PACK; i++)
	{
		borg_item *item = &borg_items[i];

		/* Skip empty items */
		if (!item->iqty) continue;

		/* Skip unknown food */
		if (!item->kind) continue;

		/* Skip non-food */
		if (item->tval != TV_FOOD) continue;

		/* Skip "flavored" food */
		if (item->sval < SV_FOOD_MIN_FOOD) continue;

		/* Eat something of that type */
		if (borg_eat_food(item->sval)) return (TRUE);
	}

	/* Scan the inventory for "okay" food */
	for (i = 0; i < INVEN_PACK; i++)
	{
		borg_item *item = &borg_items[i];

		/* Skip empty items */
		if (!item->iqty) continue;

		/* Skip unknown food */
		if (!item->kind) continue;

		/* Skip non-food */
		if (item->tval != TV_FOOD) continue;

		/* Skip "icky" food */
		if (item->sval < SV_FOOD_MIN_FOOD) continue;

		/* Eat something of that type */
		if (borg_eat_food(item->sval)) return (TRUE);
	}

	/* Scan the inventory for "potions" food */
	for (i = 0; i < INVEN_PACK; i++)
	{
		borg_item *item = &borg_items[i];

		/* Skip empty items */
		if (!item->iqty) continue;

		/* Skip unknown food */
		if (!item->kind) continue;

		/* Skip non-food */
		if (item->tval != TV_POTION) continue;

		/* Consume in order, when hurting */
		if (borg_skill[BI_CURHP] < 4 &&
			(borg_quaff_potion(SV_POTION_CURE_LIGHT) ||
			 borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			 borg_quaff_potion(SV_POTION_CURE_CRITICAL) ||
			 borg_quaff_potion(SV_POTION_RESTORE_MANA) ||
			 borg_quaff_potion(SV_POTION_HEALING) ||
			 borg_quaff_potion(SV_POTION_STAR_HEALING) ||
			 borg_quaff_potion(SV_POTION_LIFE)))
		{
			return (TRUE);
		}
	}

	/* Nothing */
	return (FALSE);
}

/*
 * Hack -- evaluate the likelihood of the borg getting surrounded
 * by a bunch of monsters.  This is called from borg_danger() when
 * he looking for a strategic retreat.  It is hopeful that the borg
 * will see that several monsters are approaching him and he may
 * become surrouned then die.  This routine looks at near by monsters
 * as determines the likelyhood of him getting surrouned.
 */
static bool borg_surrounded(void)
{
	borg_kill *kill;
	monster_race *r_ptr;

	int safe_grids = 8;
	int non_safe_grids = 0;
	int monsters = 0;
	int adjacent_monsters = 0;

	int x9, y9, ax, ay, d;
	int i;

	/* Evaluate the local monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		kill = &borg_kills[i];
		r_ptr = &r_info[kill->r_idx];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		x9 = kill->x;
		y9 = kill->y;

		/* Distance components */
		ax = (x9 > c_x) ? (x9 - c_x) : (c_x - x9);
		ay = (y9 > c_y) ? (y9 - c_y) : (c_y - y9);

		/* Distance */
		d = MAX(ax, ay);

		/* if the monster is too far then skip it. */
		if (d > 3) continue;

		/* if he cant see me then forget it. */
		if (!borg_los(c_y, c_x, y9, x9)) continue;

		/* if asleep, don't consider this one */
		if (!kill->awake) continue;

		/* Monsters with Pass Wall are dangerous, no escape from them */
		if (r_ptr->flags2 & RF2_PASS_WALL) continue;
		if (r_ptr->flags2 & RF2_KILL_WALL) continue;

		/* Monsters who never move cant surround */
		if (r_ptr->flags1 & RF1_NEVER_MOVE) continue;

		/* keep track of monsters touching me */
		if (d == 1) adjacent_monsters++;

		/* Add them up. */
		monsters++;

	}

	/* Evaluate the Non Safe Grids, (walls, closed doors, traps, monsters) */
	for (i = 0; i < 8; i++)
	{
		int x = c_x + ddx_ddd[i];
		int y = c_y + ddy_ddd[i];

		map_block *mb_ptr;

		if (!map_in_bounds(x, y)) continue;

		/* Access the grid */
		mb_ptr = map_loc(x, y);

		/* Non Safe grids */
		if (borg_cave_wall_grid(mb_ptr)) non_safe_grids++;

		/* Skip unknown grids */
		if (!mb_ptr->feat) non_safe_grids++;

		/* Skip monster grids */
		if (mb_ptr->monster) non_safe_grids++;
#if 0
		/* Mega-Hack -- skip stores XXX XXX XXX */
		if ((mb_ptr->feat >= FEAT_SHOP_HEAD) &&
			(mb_ptr->feat <= FEAT_SHOP_TAIL)) non_safe_grids++;

		/* Mega-Hack -- skip traps XXX XXX XXX */
		if ((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) &&
			(mb_ptr->feat <= FEAT_TRAP_SLEEP)) non_safe_grids++;
#endif /* 0 */
	}

	/* Safe grids are decreased */
	safe_grids = safe_grids - non_safe_grids;

	/* Am I in hallway? If so don't worry about it */
	if (safe_grids == 1 && adjacent_monsters == 1) return (FALSE);

	/* I am likely to get surrouned */
	if (monsters > safe_grids)
	{
		borg_note(format("# Possibility of being surrounded (%d/%d)",
						 monsters, safe_grids));

		/* The borg can get trapped by breeders by continueing to flee
		 * into a dead-end.  So he needs to be able to trump this
		 * routine.
		 */
		if (goal_ignoring) return (FALSE);
		else
			return (TRUE);
	}

	/* Probably will not be surrouned */
	return (FALSE);
}

/*
 * Mega-Hack -- evaluate the "freedom" of the given location
 *
 * The theory is that often, two grids will have equal "danger",
 * but one will be "safer" than the other, perhaps because it
 * is closer to stairs, or because it is in a corridor, or has
 * some other characteristic that makes it "safer".
 *
 * Then, if the Borg is in danger, say, from a normal speed monster
 * which is standing next to him, he will know that walking away from
 * the monster is "pointless", because the monster will follow him,
 * but if the resulting location is "safer" for some reason, then
 * he will consider it.  This will allow him to flee towards stairs
 * in the town, and perhaps towards corridors in the dungeon.
 *
 * This method is used in town to chase the stairs.
 *
 * XXX XXX XXX We should attempt to walk "around" buildings.
 */
static int borg_freedom(int y, int x)
{
	int d, f = 0;

	/* Hack -- chase down stairs in town */
	if (!borg_skill[BI_CDEPTH] && track_more_num)
	{
		/* Love the stairs! */
		d = double_distance(y, x, track_more_y[0], track_more_x[0]);

		/* Proximity is good */
		f += (1000 - d);

		/* Close proximity is great */
		if (d < 4) f += (2000 - (d * 500));
	}

	/* Hack -- chase Up Stairs in dungeon */
	if (borg_skill[BI_CDEPTH] && track_less_num)
	{
		/* Love the stairs! */
		d = double_distance(y, x, track_less_y[0], track_less_x[0]);

		/* Proximity is good */
		f += (1000 - d);

		/* Close proximity is great */
		if (d < 4) f += (2000 - (d * 500));
	}

	/* Freedom */
	return (f);
}


/*
 * Check a floor grid for "happy" status
 *
 * These grids are floor grids which contain stairs, or which
 * are non-corners in corridors, or which are directly adjacent
 * to pillars, or grids which we have stepped on before.
 *  Stairs are good because they can be used to leave
 * the level.  Corridors are good because you can back into them
 * to avoid groups of monsters and because they can be used for
 * escaping.  Pillars are good because while standing next to a
 * pillar, you can walk "around" it in two different directions,
 * allowing you to retreat from a single normal monster forever.
 * Stepped on grids are good because they likely stem from an area
 * which has been cleared of monsters.
 */
static bool borg_happy_grid_bold(int y, int x)
{
	int i;

	map_block *mb_ptr;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (FALSE);

	mb_ptr = map_loc(x, y);


	/* Accept stairs */
	if (mb_ptr->feat == FEAT_LESS) return (TRUE);
	if (mb_ptr->feat == FEAT_MORE) return (TRUE);
#if 0
	if (mb_ptr->feat == FEAT_MINOR_GLYPH) return (TRUE);
	if (mb_ptr->feat == FEAT_GLYPH) return (TRUE);
#endif /* 0 */

	/* Hack -- weak/dark is very unhappy */
	if (borg_skill[BI_ISWEAK] || borg_skill[BI_CUR_LITE] == 0) return (FALSE);

	/* Apply a control effect so that he does not get stuck in a loop */
	if ((borg_t - borg_began) >= 2000) return (FALSE);

	/* Case 1a: north-south corridor */
	if (borg_cave_floor_bold(y - 1, x) && borg_cave_floor_bold(y + 1, x) &&
		!borg_cave_floor_bold(y, x - 1) && !borg_cave_floor_bold(y, x + 1) &&
		!borg_cave_floor_bold(y + 1, x - 1) &&
		!borg_cave_floor_bold(y + 1, x + 1) &&
		!borg_cave_floor_bold(y - 1, x - 1) &&
		!borg_cave_floor_bold(y - 1, x + 1))
	{
		/* Happy */
		return (TRUE);
	}

	/* Case 1b: east-west corridor */
	if (borg_cave_floor_bold(y, x - 1) && borg_cave_floor_bold(y, x + 1) &&
		!borg_cave_floor_bold(y - 1, x) && !borg_cave_floor_bold(y + 1, x) &&
		!borg_cave_floor_bold(y + 1, x - 1) &&
		!borg_cave_floor_bold(y + 1, x + 1) &&
		!borg_cave_floor_bold(y - 1, x - 1) &&
		!borg_cave_floor_bold(y - 1, x + 1))
	{
		/* Happy */
		return (TRUE);
	}

	/* Case 1aa: north-south doorway */
	if (borg_cave_floor_bold(y - 1, x) && borg_cave_floor_bold(y + 1, x) &&
		!borg_cave_floor_bold(y, x - 1) && !borg_cave_floor_bold(y, x + 1))
	{
		/* Happy */
		return (TRUE);
	}

	/* Case 1ba: east-west doorway */
	if (borg_cave_floor_bold(y, x - 1) && borg_cave_floor_bold(y, x + 1) &&
		!borg_cave_floor_bold(y - 1, x) && !borg_cave_floor_bold(y + 1, x))
	{
		/* Happy */
		return (TRUE);
	}


	/* Case 2a: north pillar */
	if (!borg_cave_floor_bold(y - 1, x) &&
		borg_cave_floor_bold(y - 1, x - 1) &&
		borg_cave_floor_bold(y - 1, x + 1) && borg_cave_floor_bold(y - 2, x))
	{
		/* Happy */
		return (TRUE);
	}

	/* Case 2b: south pillar */
	if (!borg_cave_floor_bold(y + 1, x) &&
		borg_cave_floor_bold(y + 1, x - 1) &&
		borg_cave_floor_bold(y + 1, x + 1) && borg_cave_floor_bold(y + 2, x))
	{
		/* Happy */
		return (TRUE);
	}

	/* Case 2c: east pillar */
	if (!borg_cave_floor_bold(y, x + 1) &&
		borg_cave_floor_bold(y - 1, x + 1) &&
		borg_cave_floor_bold(y + 1, x + 1) && borg_cave_floor_bold(y, x + 2))
	{
		/* Happy */
		return (TRUE);
	}

	/* Case 2d: west pillar */
	if (!borg_cave_floor_bold(y, x - 1) &&
		borg_cave_floor_bold(y - 1, x - 1) &&
		borg_cave_floor_bold(y + 1, x - 1) && borg_cave_floor_bold(y, x - 2))
	{
		/* Happy */
		return (TRUE);
	}

	/* check for grids that have been stepped on before */
	for (i = 0; i < track_step_num; i++)
	{
		/* Enqueue the grid */
		if ((track_step_y[i] == y) && (track_step_x[i] == x))
		{
			/* Recent step is good */
			if (i < 25)
			{
				return (TRUE);
			}
		}
	}

	/* Not happy */
	return (FALSE);
}

static bool test_borg_lite_beam(byte dir, byte radius)
{
	int x = c_x, y = c_y;
	int dx = 0, dy = 0;
	int i;

	map_block *mb_ptr;

	switch (dir)
	{
		case 8:
		{
			/* North */
			dx = 0;
			dy = -1;
		}

		case 6:
		{
			/* East */
			dx = 1;
			dy = 0;
		}

		case 2:
		{
			/* South */
			dx = 0;
			dy = 1;
		}

		case 4:
		{
			/* West */
			dx = -1;
			dy = 0;
		}
	}

	for (i = 0; i <= radius; i++)
	{
		x += dx;
		y += dy;

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		mb_ptr = map_loc(x, y);

		/* Walls aren't interesting to light */
		if (borg_cave_wall_grid(mb_ptr)) return (FALSE);

		/* Unlit square just out of los? */
		if (!mb_ptr->feat) return (TRUE);
	}

	/* Nothing interesting just out of los */
	return (FALSE);
}

/*
 * This will look down a hallway and possibly light it up using
 * the Light Beam mage spell.  This spell is mostly used when
 * the borg is moving through the dungeon under boosted bravery.
 * This will allow him to "see" if anyone is there.
 *
 * It might also come in handy if he's in a hallway and gets shot, or
 * if resting in a hallway.  He may want to cast it to make
 * sure no previously unknown monsters are in the hall.
 * NOTE:  ESP will alter the value of this spell.
 *
 * Borg has a problem when not on map centering mode and casting the beam
 * repeatedly, down or up when at the edge of a panel.
 */
bool borg_lite_beam(bool simulation)
{
	int dir = 5;
	bool spell_ok = FALSE;

	/* Hack -- weak/dark is very unhappy */
	if (borg_skill[BI_ISWEAK] || borg_skill[BI_CUR_LITE] == 0) return (FALSE);

	/* Apply a control effect so that he does not get stuck in a loop */
	if ((borg_t - borg_began) >= 2000) return (FALSE);

	/* Require the abilitdy */
	if (borg_spell_okay_fail(REALM_NATURE, 1, 4, 20) ||
		(-1 != borg_slot(TV_WAND, SV_WAND_LITE) &&
		 borg_items[borg_slot(TV_WAND, SV_WAND_LITE)].pval) ||
		borg_equips_rod(SV_ROD_LITE))
		spell_ok = TRUE;

	/* No need to waste time calculating */
	if (!simulation && !spell_ok) return (FALSE);

	/* North */
	if (test_borg_lite_beam(8, borg_skill[BI_CUR_LITE])) dir = 8;

	/* East */
	else if (test_borg_lite_beam(6, borg_skill[BI_CUR_LITE])) dir = 6;

	/* West */
	else if (test_borg_lite_beam(4, borg_skill[BI_CUR_LITE])) dir = 4;

	/* South */
	else if (test_borg_lite_beam(2, borg_skill[BI_CUR_LITE])) dir = 2;

	/* Failure? */
	if (dir == 5 || spell_ok == FALSE) return (FALSE);

	/* simulation */
	if (simulation) return (TRUE);

	/* cast the light beam */
	if (borg_spell_fail(REALM_NATURE, 1, 4, 20) ||
		borg_zap_rod(SV_ROD_LITE) || borg_aim_wand(SV_WAND_LITE))
	{							/* apply the direction */
		borg_keypress(I2D(dir));
		borg_note("# Illuminating this hallway");
		return (TRUE);
	}

	/* cant do it */
	return (FALSE);
}

/*
 * Scan the monster lists for certain types of monster that we
 * should be concerned over.
 * This only works for monsters we know about.  If one of the
 * monsters around is misidentified then it may be a unique
 * and we wouldn't know.  Special consideration is given to Morgoth
 */
static void borg_near_monster_type(int dist)
{
	borg_kill *kill;
	monster_race *r_ptr;

	int x9, y9, ax, ay, d;
	int i;

	/* reset the borg flags */
	borg_fighting_unique = 0;
	borg_fighting_evil_unique = FALSE;

	/* Scan the monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		kill = &borg_kills[i];
		r_ptr = &r_info[kill->r_idx];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;


		x9 = kill->x;
		y9 = kill->y;

		/* Distance components */
		ax = (x9 > c_x) ? (x9 - c_x) : (c_x - x9);
		ay = (y9 > c_y) ? (y9 - c_y) : (c_y - y9);

		/* Distance */
		d = MAX(ax, ay);

		/* if the guy is too far then skip it. */
		if (d > dist) continue;


		/*** Scan for Uniques ***/

		/* this is a unique. */
		if ((r_ptr->flags1 & RF1_UNIQUE))
		{
			/* Set a flag for use with certain types of spells */
			unique_on_level = TRUE;

			/* return 1 if not Morgy, +10 if it is Morgy or Sauron */
			if (r_ptr->flags1 & RF1_QUESTOR)
			{
				/* keep a battle log */
				if (!borg_fff) borg_log_battle(TRUE);

				borg_fighting_unique += 10;
			}

			/* regular unique */
			borg_fighting_unique++;

			/* Note that fighting a Questor would result in a 11 value */
			if (r_ptr->flags3 & RF3_EVIL) borg_fighting_evil_unique = TRUE;

		}

#if 0
		/*** Scan for Scary Guys ***/

		/* run from certain scaries */
		if (borg_skill[BI_CLEVEL] < 3 &&
			(strstr(r_name + r_ptr->name, "Squint"))) scaryguy_on_level = TRUE;

		/* run from certain dungeon scaries */
		if (borg_skill[BI_CLEVEL] <= 3 &&
			(strstr(r_name + r_ptr->name, "Grip") ||
			 strstr(r_name + r_ptr->name, "Agent") ||
			 strstr(r_name + r_ptr->name, "Fang"))) scaryguy_on_level = TRUE;

		/* run from certain scaries */
		if (borg_skill[BI_CLEVEL] <= 5 &&
			(strstr(r_name + r_ptr->name, "Small kobold") ||
			 strstr(r_name + r_ptr->name, "Kobold") ||
			 strstr(r_name + r_ptr->name, "Jackal") ||
			 strstr(r_name + r_ptr->name, "Filthy street urchin") ||
			 strstr(r_name + r_ptr->name, "Battle scarred veteran") ||
			 strstr(r_name + r_ptr->name,
					"Mean looking mercenary"))) scaryguy_on_level = TRUE;

		if (borg_skill[BI_CLEVEL] <= 8 &&
			(strstr(r_name + r_ptr->name, "Giant white mouse") ||
			 strstr(r_name + r_ptr->name, "White worm mass") ||
			 strstr(r_name + r_ptr->name,
					"Green worm mass"))) scaryguy_on_level = TRUE;

		if (borg_skill[BI_CLEVEL] <= 10 &&
			(strstr(r_name + r_ptr->name, "Cave spider") ||
			 strstr(r_name + r_ptr->name, "Yellow worm mass") ||
			 strstr(r_name + r_ptr->name, "Pink naga") ||
			 strstr(r_name + r_ptr->name, "Giant pink frog") ||
			 strstr(r_name + r_ptr->name, "Radiation eye"))) scaryguy_on_level =
TRUE;


		if (borg_skill[BI_CLEVEL] <= 40 &&
			(strstr(r_name + r_ptr->name, "Greater hell"))) scaryguy_on_level =
TRUE;

		/* Nether breath is bad */
		if ((!borg_skill[BI_SRNTHR] && !borg_skill[BI_AXGOI]) &&
			(strstr(r_name + r_ptr->name, "Azriel") ||
			 strstr(r_name + r_ptr->name, "Dracolich") ||
			 strstr(r_name + r_ptr->name, "Dracolisk"))) scaryguy_on_level =
TRUE;

		/* Blindness is really bad */
		if ((!borg_skill[BI_SRBLIND]) &&
			(strstr(r_name + r_ptr->name, "Light hound") ||
			 strstr(r_name + r_ptr->name, "Dark hound"))) scaryguy_on_level =
TRUE;

		/* Chaos and Confusion are really bad */
		if ((!borg_skill[BI_SRKAOS] && !borg_skill[BI_SRCONF]) &&
			(strstr(r_name + r_ptr->name, "Chaos"))) scaryguy_on_level = TRUE;

		/*** Scan for Summoners ***/

		if ((r_ptr->flags6 & RF6_S_KIN) ||
			(r_ptr->flags6 & RF6_S_CYBER) ||
			(r_ptr->flags6 & RF6_S_MONSTER) ||
			(r_ptr->flags6 & RF6_S_MONSTERS) ||
			(r_ptr->flags6 & RF6_S_ANT) ||
			(r_ptr->flags6 & RF6_S_SPIDER) ||
			(r_ptr->flags6 & RF6_S_HOUND) ||
			(r_ptr->flags6 & RF6_S_HYDRA) ||
			(r_ptr->flags6 & RF6_S_ANGEL) ||
			(r_ptr->flags6 & RF6_S_DEMON) ||
			(r_ptr->flags6 & RF6_S_UNDEAD) ||
			(r_ptr->flags6 & RF6_S_DRAGON) ||
			(r_ptr->flags6 & RF6_S_HI_UNDEAD) ||
			(r_ptr->flags6 & RF6_S_HI_DRAGON) ||
			(r_ptr->flags6 & RF6_S_AMBERITES) ||
			(r_ptr->flags6 & RF6_S_UNIQUE) || (r_ptr->flags1 & RF1_QUESTOR))
		{
			/* mark the flag */
			borg_fighting_summoner = TRUE;

			/* recheck the distance to see if close
			 * and mark the index for as-corridor
			 */
			if (d < 8)
			{
				borg_kills_summoner = i;
			}
		}
#endif /* 0 */
	}
}

/*
 * Help determine if "phase door" seems like a good idea
 */
bool borg_caution_phase(int emergency, int turns)
{
	int n, k, i, d, x, y, p;

	int dis = 10;
	int min;

	map_block *mb_ptr = map_loc(c_x, c_y);

	/* Define minimal distance */
	min = dis / 2;

	/* Simulate 100 attempts */
	for (n = k = 0; k < 100; k++)
	{
		/* Pick a location */
		for (i = 0; i < 100; i++)
		{
			/* Pick a (possibly illegal) location */
			while (1)
			{
				y = rand_spread(c_y, dis);
				x = rand_spread(c_x, dis);
				d = distance(c_y, c_x, y, x);
				if ((d >= min) && (d <= dis)) break;
			}

			if (!map_in_bounds(x, y)) continue;

			/* Access */
			mb_ptr = map_loc(x, y);

			/* If low level, unknown squares are scary */
			if (!mb_ptr->feat && borg_skill[BI_MAXHP] < 30) break;

			/* Skip unknown grids */
			if (!mb_ptr->feat) continue;

			/* Skip weird grids */
			if (mb_ptr->feat == FEAT_INVIS) continue;

			/* Skip walls */
			if (borg_cave_wall_grid(mb_ptr)) continue;

			/* Skip monsters */
			if (mb_ptr->monster) continue;

			/* Stop looking */
			break;
		}

		/* If low level, unknown squares are scary */
		if (!mb_ptr->feat && borg_skill[BI_MAXHP] < 30)
		{
			n++;
			continue;
		}

		/* No location */
		/* in the real code it would keep trying but here we should */
		/* assume that there is unknown spots that you would be able */
		/* to go but may be dangerious. */
		if (i >= 100)
		{
			n++;
			continue;
		}

		/* Examine */
		p = borg_danger(y, x, turns, TRUE);

		/* if *very* scary, do not allow jumps at all */
		if (p > borg_skill[BI_CURHP]) n++;

	}

	/* Too much danger */
	/* in an emergency try with extra danger allowed */
	if (n > emergency)
	{
		borg_note(format("# No Phase. scary squares: %d", n));
		return (FALSE);
	}
	else
		borg_note(format("# Safe to Phase. scary squares: %d", n));

	/* Okay */
	return (TRUE);
}

/*
 * Help determine if "dimension door" seems like a good idea
 */
static bool borg_dim_door(int emergency, int p1)
{
	int x, y, p;

	int b_y = -1, b_x = -1, b_p = p1;
	int dis = borg_skill[BI_CLEVEL] + 2;

	map_block *mb_ptr = map_loc(c_x, c_y);

	/* Scan every grid in landing zone */
	for (y = c_y - dis; y < c_y + dis; y++)
	{
		/* Pick a location */
		for (x = c_x - dis; x < c_x + dis; x++)
		{
			if (!map_in_bounds(x, y)) continue;

			if ((x == c_x) && (y == c_y)) continue;

			/* Access */
			mb_ptr = map_loc(x, y);

			/* Verify distance again */
			if (distance(y, x, c_y, c_x) > borg_skill[BI_CLEVEL] + 2) continue;

			/* Skip unknown grids */
			if (!mb_ptr->feat) continue;

			/* Skip weird grids */
			if (mb_ptr->feat == FEAT_INVIS) continue;

			/* Skip walls, trees, water, lava */
			if (borg_cave_wall_grid(mb_ptr)) continue;

			/* Skip monsters */
			if (mb_ptr->monster) continue;

			/* Examine */
			p = borg_danger(y, x, 1, TRUE);

			/* if *very* scary, do not allow jumps at all */
			if (!emergency && p > borg_skill[BI_CURHP]) continue;

			/* Track the grid with the least danger */
			if (p > b_p) continue;

			/* note good landing zones */
			b_p = p;
			b_y = y;
			b_x = x;
		}
	}


	/* Dimension Door report */
	borg_note(format
			  ("# Dim Door: Safest grid: (%d, %d) with %d Danger", b_y, b_x,
			   b_p));
	dim_door_y = b_y;
	dim_door_x = b_x;

	/* No good landing zone */
	if (b_p >= p1) return (FALSE);

	/* Okay */
	return (TRUE);
}

/*
 * Try to phase door or teleport
 * b_q is the danger of the least dangerious square around us.
 */
static bool borg_escape(int b_q)
{

	int risky_boost = 0;

	bool amt_dim_door = FALSE;

	/* only escape with spell if fail is low */
	int allow_fail = 25;
	int sv_mana;

	/* Not if locked down */
	if (borg_skill[BI_CRSNOTELE]) return (FALSE);

	/* if we have Dim Door spell */
	amt_dim_door = (borg_spell_okay_fail(REALM_SORCERY, 2, 3, allow_fail) ||
					borg_spell_okay_fail(REALM_TRUMP, 0, 5, allow_fail) ||
					borg_mindcr_okay_fail(MIND_MINOR_DISP, 40, allow_fail));

	/* if very healthy, allow extra fail */
	if (((borg_skill[BI_CURHP] * 100) / borg_skill[BI_MAXHP]) > 70)
		allow_fail = 10;

	/* comprimised, get out of the fight */
	if (borg_skill[BI_ISHEAVYSTUN])
		allow_fail = 35;

	/* for emergencies */
	sv_mana = borg_skill[BI_CURSP];

	/* Borgs who are bleeding to death or dying of poison may sometimes
	 * phase around the last two hit points right before they enter a
	 * shop.  He knows to make a bee-line for the temple but the danger
	 * trips this routine.  So we must bypass this routine for some
	 * particular circumstances.
	 */
	if (!borg_skill[BI_CDEPTH] &&
		(borg_skill[BI_ISPOISONED] || borg_skill[BI_ISWEAK] ||
		 borg_skill[BI_ISCUT])) return (FALSE);

	/* Borgs with GOI should not escape until the GOI falls */
	if (borg_goi) return (FALSE);

	/* Risky borgs are more likely to stay in a fight */
	if (borg_plays_risky) risky_boost = 3;

	/* 1. really scary, I'm about to die */
	/* Try an emergency teleport, or phase door as last resort */
	if (borg_skill[BI_ISHEAVYSTUN] ||
		(b_q >= avoidance * (45 + risky_boost) / 10) ||
		((b_q >= avoidance * (40 + risky_boost) / 10) &&
		 borg_fighting_unique >= 10 && borg_skill[BI_CDEPTH] == 100 &&
		 borg_skill[BI_CURHP] < 600) ||
		((b_q >= avoidance * (30 + risky_boost) / 10) &&
		 borg_fighting_unique >= 10 && borg_skill[BI_CDEPTH] == 99 &&
		 borg_skill[BI_CURHP] < 600) ||
		((b_q >= avoidance * (25 + risky_boost) / 10) &&
		 borg_fighting_unique >= 1 && borg_fighting_unique <= 8 &&
		 borg_skill[BI_CDEPTH] >= 95 && borg_skill[BI_CURHP] < 550) ||
		((b_q >= avoidance * (17 + risky_boost) / 10) &&
		 borg_fighting_unique >= 1 && borg_fighting_unique <= 8 &&
		 borg_skill[BI_CDEPTH] < 95) ||
		((b_q >= avoidance * (15 + risky_boost) / 10) && !borg_fighting_unique))
	{

		int allow_fail = 11;

		if (borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail - 10) ||
			borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail - 10) ||
			borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail - 10) ||
			borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail - 10) ||
			borg_mindcr_fail(MIND_MAJOR_DISP, 7, allow_fail - 10) ||
			borg_read_scroll(SV_SCROLL_TELEPORT) ||
			borg_use_staff_fail(SV_STAFF_TELEPORTATION) ||
			borg_activate_artifact(ART_COLANNON, FALSE) ||
			/* revisit spells, increased fail rate */
			borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail + 9) ||
			borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail + 9) ||
			borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail + 9) ||
			borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail + 9) ||
			borg_mindcr_fail(MIND_MAJOR_DISP, 7, allow_fail + 9) ||
			borg_racial(RACE_GNOME) ||
			/* Attempt Teleport Level */
			borg_spell_fail(REALM_SORCERY, 2, 6, allow_fail + 9) ||
			borg_spell_fail(REALM_TRUMP, 1, 5, allow_fail + 9) ||
			borg_spell_fail(REALM_ARCANE, 3, 1, allow_fail + 9) ||
			borg_read_scroll(SV_SCROLL_TELEPORT_LEVEL) ||
			/* try Dimension Door */
			(amt_dim_door && borg_dim_door(TRUE, b_q) &&
			 (borg_spell_fail(REALM_SORCERY, 2, 3, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 5, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 40, allow_fail))) ||
			/* try phase at least */
			borg_read_scroll(SV_SCROLL_PHASE_DOOR) ||
			borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			borg_spell_fail(REALM_ARCANE, 0, 4, allow_fail) ||
			borg_spell_fail(REALM_SORCERY, 0, 1, allow_fail) ||
			borg_spell_fail(REALM_TRUMP, 0, 0, allow_fail) ||
			borg_mindcr_fail(MIND_MINOR_DISP, 3, allow_fail))
		{
			/* Flee! */
			borg_note("# Danger Level 1.");
			return (TRUE);
		}

		borg_skill[BI_CURSP] = borg_skill[BI_MAXSP];

		/* try to teleport, get far away from here */
		if (borg_use_staff_fail(SV_STAFF_TELEPORTATION) ||
			borg_activate_artifact(ART_COLANNON, FALSE) ||
			borg_read_scroll(SV_SCROLL_TELEPORT) ||
			borg_spell(REALM_ARCANE, 2, 3) ||
			borg_spell(REALM_TRUMP, 0, 4) ||
			borg_spell(REALM_CHAOS, 0, 7) || borg_spell(REALM_SORCERY, 0, 5))
		{
			/* verify use of spell */
			/* borg_keypress('y');  */

			/* Flee! */
			borg_note("# Danger Level 1.1  Critical Attempt");
			return (TRUE);
		}

		/* emergency phase spell */
		if (borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			(amt_phase && borg_caution_phase(80, 5) &&
			 (borg_read_scroll(SV_SCROLL_PHASE_DOOR))) ||
			borg_read_scroll(SV_SCROLL_TELEPORT_LEVEL))
		{
			/* Flee! */
			borg_escapes--;		/* a phase isn't really an escape */
			borg_note("# Danger Level 1.2  Critical Phase");
			return (TRUE);
		}

		/* emergency phase spell */
		if (borg_caution_phase(80, 5) &&
			(borg_spell_fail(REALM_ARCANE, 0, 4, 15) ||
			 borg_spell_fail(REALM_SORCERY, 0, 1, 15) ||
			 borg_spell_fail(REALM_TRUMP, 0, 0, 15)))
		{
			/* verify use of spell */
			borg_keypress('y');

			/* Flee! */
			borg_note("# Danger Level 1.3  Critical Attempt");
			return (TRUE);
		}
		borg_skill[BI_CURSP] = sv_mana;
	}

	/* If fighting a unique and at the end of the game try to stay and
	 * finish the fight.  Only bail out in extreme danger as above.
	 */
	if ((b_q < avoidance * (25 + risky_boost) / 10 && borg_fighting_unique >= 1
		 && borg_fighting_unique <= 3 && borg_skill[BI_CDEPTH] >= 97) ||
		borg_skill[BI_CURHP] > 550) return (FALSE);


	/* 2 - a bit more scary */
	/* Attempt to teleport (usually) */
	/* do not escape from uniques so quick */
	if (borg_skill[BI_ISHEAVYSTUN] ||
		((b_q >= avoidance * (15 + risky_boost) / 10) &&
		 borg_fighting_unique >= 1 && borg_fighting_unique <= 8 &&
		 borg_skill[BI_CDEPTH] != 99) ||
		((b_q >= avoidance * (13 + risky_boost) / 10) && !borg_fighting_unique))
	{

		/* Try teleportation */
		if (borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail - 10) ||
			borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail - 10) ||
			borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail - 10) ||
			borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail - 10) ||
			borg_mindcr_fail(MIND_MAJOR_DISP, 7, allow_fail - 10) ||
			borg_use_staff_fail(SV_STAFF_TELEPORTATION) ||
			borg_activate_artifact(ART_COLANNON, FALSE) ||
			borg_read_scroll(SV_SCROLL_TELEPORT) ||
			borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail) ||
			borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail) ||
			borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail) ||
			borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail) ||
			borg_mindcr_fail(MIND_MAJOR_DISP, 7, allow_fail) ||
			borg_racial(RACE_GNOME) ||
			/* try Dimension Door */
			(amt_dim_door && borg_dim_door(TRUE, b_q) &&
			 (borg_spell_fail(REALM_SORCERY, 2, 3, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 5, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 40, allow_fail))))
		{
			/* Flee! */
			borg_note("# Danger Level 2.1");

			/* Success */
			return (TRUE);
		}
		/* Phase door, if useful */
		if (amt_phase && borg_caution_phase(50, 2) &&
			(borg_read_scroll(SV_SCROLL_PHASE_DOOR) ||
			 borg_spell(REALM_ARCANE, 0, 4) ||
			 borg_spell(REALM_SORCERY, 0, 1) ||
			 borg_spell(REALM_TRUMP, 0, 0) ||
			 borg_mindcr(MIND_MINOR_DISP, 3) ||
			 borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			 borg_activate_artifact(ART_COLANNON, FALSE)))
		{
			/* Flee! */
			borg_note("# Danger Level 2.2");
			/* Success */
			return (TRUE);
		}

	}

	/* 3- not too bad */
	/* also run if stunned or it is scary here */
	if (borg_skill[BI_ISHEAVYSTUN] ||
		((b_q >= avoidance * (13 + risky_boost) / 10) &&
		 borg_fighting_unique >= 2 && borg_fighting_unique <= 8) ||
		((b_q >= avoidance * (10 + risky_boost) / 10) && !borg_fighting_unique)
		|| ((b_q >= avoidance * (10 + risky_boost) / 10) &&
			borg_skill[BI_ISAFRAID] && (borg_skill[BI_AMISSILES] <= 0 &&
										borg_class == CLASS_WARRIOR)))
	{
		/* try Dimension Door */
		if ((amt_dim_door && borg_dim_door(TRUE, b_q) &&
			 (borg_spell_fail(REALM_SORCERY, 2, 3, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 5, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 40, allow_fail))) ||
			/* Phase door, if useful */
			(amt_phase && borg_caution_phase(25, 2) &&
			 (borg_spell_fail(REALM_ARCANE, 0, 4, allow_fail) ||
			  borg_spell_fail(REALM_SORCERY, 0, 1, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 0, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 3, allow_fail) ||
			  borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			  borg_activate_artifact(ART_COLANNON, FALSE) ||
			  borg_read_scroll(SV_SCROLL_PHASE_DOOR))))
		{
			/* Flee! */
			borg_escapes--;		/* a phase isn't really an escape */
			borg_note("# Danger Level 3.1");

			/* Success */
			return (TRUE);
		}

		/* Teleport via spell */
		if (borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail) ||
			borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail) ||
			borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail) ||
			borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail) ||
			borg_mindcr_fail(MIND_MAJOR_DISP, 7, allow_fail) ||
			borg_activate_artifact(ART_COLANNON, FALSE) ||
			borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			borg_use_staff_fail(SV_STAFF_TELEPORTATION) ||
			borg_read_scroll(SV_SCROLL_TELEPORT) || borg_racial(RACE_GNOME))
		{
			/* Flee! */
			borg_note("# Danger Level 3.2");

			/* Success */
			return (TRUE);
		}
		/* Phase door, if useful */
		if (amt_phase && borg_caution_phase(65, 2) &&
			(borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail) ||
			 borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail) ||
			 borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail) ||
			 borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail) ||
			 borg_mindcr_fail(MIND_MINOR_DISP, 3, allow_fail) ||
			 borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			 borg_activate_artifact(ART_COLANNON, FALSE) ||
			 borg_read_scroll(SV_SCROLL_PHASE_DOOR)))
		{
			/* Flee! */
			borg_escapes--;		/* a phase isn't really an escape */
			borg_note("# Danger Level 3.3");

			/* Success */
			return (TRUE);
		}

		/* if we got this far we tried to escape but couldn't... */
		/* time to flee */
		if (!goal_fleeing &&
			(!borg_fighting_unique || borg_skill[BI_CLEVEL] < 35) &&
			!vault_on_level && !borg_quest_level)
		{
			/* Note */
			borg_note("# Fleeing (failed to teleport)");

			/* Start fleeing */
			goal_fleeing = TRUE;
		}

		/* Flee now */
		if (!goal_leaving &&
			(!borg_fighting_unique || borg_skill[BI_CLEVEL] < 35) &&
			!vault_on_level && !borg_quest_level)
		{
			/* Flee! */
			borg_note("# Leaving (failed to teleport)");

			/* Start leaving */
			goal_leaving = TRUE;
		}

	}
	/* 4- not too scary but I'm comprimized */
	if ((b_q >= avoidance * (8 + risky_boost) / 10 &&
		 (borg_skill[BI_CLEVEL] < 35 ||
		  borg_skill[BI_CURHP] <= borg_skill[BI_MAXHP] / 3)) ||
		((b_q >= avoidance * (12 + risky_boost) / 10) &&
		 borg_fighting_unique >= 1 && borg_fighting_unique <= 8 &&
		 (borg_skill[BI_CLEVEL] < 35 ||
		  borg_skill[BI_CURHP] <= borg_skill[BI_MAXHP] / 3)) ||
		((b_q >= avoidance * (6 + risky_boost) / 10) &&
		 borg_skill[BI_CLEVEL] <= 20 && !borg_fighting_unique) ||
		((b_q >= avoidance * (6 + risky_boost) / 10) && borg_class == CLASS_MAGE
		 && borg_skill[BI_CLEVEL] <= 35))
	{
		/* Phase door, if useful */
		if ((amt_dim_door && borg_dim_door(TRUE, b_q) &&
			 (borg_spell_fail(REALM_SORCERY, 2, 3, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 5, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 40, allow_fail))) ||
			(amt_phase && borg_caution_phase(20, 2) &&
			 (borg_spell_fail(REALM_ARCANE, 0, 4, allow_fail) ||
			  borg_spell_fail(REALM_SORCERY, 0, 1, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 0, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 3, allow_fail) ||
			  borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			  borg_activate_artifact(ART_COLANNON, FALSE) ||
			  borg_read_scroll(SV_SCROLL_PHASE_DOOR))))
		{
			/* Flee! */
			borg_escapes--;		/* a phase isn't really an escape */
			borg_note("# Danger Level 4.1");
			/* Success */
			return (TRUE);
		}

		/* Teleport via spell */
		if (borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail) ||
			borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail) ||
			borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail) ||
			borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail) ||
			borg_mindcr_fail(MIND_MAJOR_DISP, 7, allow_fail) ||
			borg_activate_artifact(ART_COLANNON, FALSE) ||
			borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			borg_read_scroll(SV_SCROLL_TELEPORT) ||
			borg_use_staff_fail(SV_STAFF_TELEPORTATION) ||
			borg_racial(RACE_GNOME))
		{
			/* Flee! */
			borg_note("# Danger Level 4.2");

			/* Success */
			return (TRUE);
		}

		/* if we got this far we tried to escape but couldn't... */
		/* time to flee */
		if (!goal_fleeing && !borg_fighting_unique && borg_skill[BI_CLEVEL] < 25
			&& !vault_on_level && !borg_quest_level)
		{
			/* Note */
			borg_note("# Fleeing (failed to teleport)");

			/* Start fleeing */
			goal_fleeing = TRUE;
		}

		/* Flee now */
		if (!goal_leaving && !borg_fighting_unique &&
			!vault_on_level && !borg_quest_level)
		{
			/* Flee! */
			borg_note("# Leaving (failed to teleport)");

			/* Start leaving */
			goal_leaving = TRUE;
		}
		/* Emergency Phase door if a weak mage */
		if ((borg_class == CLASS_MAGE && borg_skill[BI_CLEVEL] <= 35) &&
			amt_phase && borg_caution_phase(65, 2) &&
			(borg_spell_fail(REALM_ARCANE, 0, 4, allow_fail) ||
			 borg_spell_fail(REALM_SORCERY, 0, 1, allow_fail) ||
			 borg_spell_fail(REALM_TRUMP, 0, 0, allow_fail) ||
			 borg_mindcr_fail(MIND_MINOR_DISP, 3, allow_fail) ||
			 borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			 borg_activate_artifact(ART_COLANNON, FALSE) ||
			 borg_read_scroll(SV_SCROLL_PHASE_DOOR)))
		{
			/* Flee! */
			borg_escapes--;		/* a phase isn't really an escape */
			borg_note("# Danger Level 4.3");
			/* Success */
			return (TRUE);
		}

	}

	/* 5- not too scary but I'm very low level  */
	if (borg_skill[BI_CLEVEL] < 10 &&
		(b_q >= avoidance * (6 + risky_boost) / 10 ||
		 (b_q >= avoidance * (8 + risky_boost) / 10 && borg_fighting_unique >= 1
		  && borg_fighting_unique <= 8)))
	{
		/* Dimension Door, if useful */
		if ((amt_dim_door && borg_dim_door(TRUE, b_q) &&
			 (borg_spell_fail(REALM_SORCERY, 2, 3, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 5, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 40, allow_fail))) ||
			/* Phase Door */
			(amt_phase && borg_caution_phase(20, 2) &&
			 (borg_spell_fail(REALM_ARCANE, 0, 4, allow_fail) ||
			  borg_spell_fail(REALM_SORCERY, 0, 1, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 0, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 3, allow_fail) ||
			  borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			  borg_activate_artifact(ART_COLANNON, FALSE) ||
			  borg_read_scroll(SV_SCROLL_PHASE_DOOR))))
		{
			/* Flee! */
			borg_note("# Danger Level 5.1");
			/* Success */
			return (TRUE);
		}

		/* Teleport via spell */
		if (borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail) ||
			borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail) ||
			borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail) ||
			borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail) ||
			borg_mindcr_fail(MIND_MAJOR_DISP, 7, allow_fail) ||
			borg_activate_artifact(ART_COLANNON, FALSE) ||
			borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			borg_read_scroll(SV_SCROLL_TELEPORT) ||
			borg_use_staff_fail(SV_STAFF_TELEPORTATION) ||
			borg_racial(RACE_GNOME))
		{
			/* Flee! */
			borg_note("# Danger Level 5.2");

			/* Success */
			return (TRUE);
		}

		/* if we got this far we tried to escape but couldn't... */
		/* time to flee */
		if (!goal_fleeing && !borg_fighting_unique)
		{
			/* Note */
			borg_note("# Fleeing (failed to teleport)");

			/* Start fleeing */
			goal_fleeing = TRUE;
		}

		/* Flee now */
		if (!goal_leaving && !borg_fighting_unique)
		{
			/* Flee! */
			borg_note("# Leaving (failed to teleport)");

			/* Start leaving */
			goal_leaving = TRUE;
		}
		/* Emergency Phase door if a weak mage */
		if ((borg_class == CLASS_MAGE && borg_skill[BI_CLEVEL] <= 8) &&
			amt_phase && borg_caution_phase(65, 2) &&
			(borg_spell_fail(REALM_ARCANE, 0, 4, allow_fail) ||
			 borg_spell_fail(REALM_SORCERY, 0, 1, allow_fail) ||
			 borg_spell_fail(REALM_TRUMP, 0, 0, allow_fail) ||
			 borg_mindcr_fail(MIND_MINOR_DISP, 3, allow_fail) ||
			 borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			 borg_activate_artifact(ART_COLANNON, FALSE) ||
			 borg_read_scroll(SV_SCROLL_PHASE_DOOR)))
		{
			/* Flee! */
			borg_escapes--;		/* a phase isn't really an escape */
			borg_note("# Danger Level 5.3");
			/* Success */
			return (TRUE);
		}

	}

	/* 6- not too scary but I'm out of mana  */
	if ((borg_class == CLASS_MAGE || borg_class == CLASS_PRIEST) &&
		(b_q >= avoidance * (6 + risky_boost) / 10 ||
		 (b_q >= avoidance * (8 + risky_boost) / 10 && borg_fighting_unique >= 1
		  && borg_fighting_unique <= 8)) &&
		(borg_skill[BI_CURSP] <= (borg_skill[BI_MAXSP] * 1 / 10) &&
		 borg_skill[BI_MAXSP] >= 100))
	{
		/* Dimension Door, if useful */
		if ((amt_dim_door && borg_dim_door(TRUE, b_q) &&
			 (borg_spell_fail(REALM_SORCERY, 2, 3, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 5, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 40, allow_fail))) ||
			/* Phase Door */
			(amt_phase && borg_caution_phase(20, 2) &&
			 (borg_spell_fail(REALM_ARCANE, 0, 4, allow_fail) ||
			  borg_spell_fail(REALM_SORCERY, 0, 1, allow_fail) ||
			  borg_spell_fail(REALM_TRUMP, 0, 0, allow_fail) ||
			  borg_mindcr_fail(MIND_MINOR_DISP, 3, allow_fail) ||
			  borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			  borg_activate_artifact(ART_COLANNON, FALSE) ||
			  borg_read_scroll(SV_SCROLL_PHASE_DOOR))))
		{
			/* Flee! */
			borg_note("# Danger Level 6.1");
			/* Success */
			return (TRUE);
		}

		/* Teleport via spell */
		if (borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail) ||
			borg_spell_fail(REALM_TRUMP, 0, 4, allow_fail) ||
			borg_spell_fail(REALM_CHAOS, 0, 7, allow_fail) ||
			borg_spell_fail(REALM_SORCERY, 0, 5, allow_fail) ||
			borg_mindcr_fail(MIND_MAJOR_DISP, 7, allow_fail) ||
			borg_activate_artifact(ART_COLANNON, FALSE) ||
			borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			borg_read_scroll(SV_SCROLL_TELEPORT) ||
			borg_use_staff_fail(SV_STAFF_TELEPORTATION) ||
			borg_racial(RACE_GNOME))
		{
			/* Flee! */
			borg_note("# Danger Level 6.2");

			/* Success */
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * ** Try healing **
 * this function tries to heal the borg before trying to flee.
 * The ez_heal items (*Heal* and Life) are reserved for Morgoth.
 * In severe emergencies the borg can drink an ez_heal item but that is
 * checked in borg_caution().  He should bail out of the fight before
 * using an ez_heal.
 */
static bool borg_heal(int danger)
{
	int hp_down;
	int allow_fail = 20;
	int chance;

	int stats_needing_fix = 0;

	map_block *mb_ptr = map_loc(c_x, c_y);

	hp_down = borg_skill[BI_MAXHP] - borg_skill[BI_CURHP];


	/* when fighting Morgoth, we want the borg to use Life potion to fix his
	 * stats.  So we need to add up the ones that are dropped.
	 */
	if (borg_skill[BI_ISFIXSTR]) stats_needing_fix++;
	if (borg_skill[BI_ISFIXINT]) stats_needing_fix++;
	if (borg_skill[BI_ISFIXWIS]) stats_needing_fix++;
	if (borg_skill[BI_ISFIXDEX]) stats_needing_fix++;
	if (borg_skill[BI_ISFIXCON]) stats_needing_fix++;

	/* Special cases get a second vote */
	if (borg_class == CLASS_MAGE &&
		borg_skill[BI_ISFIXINT]) stats_needing_fix++;
	if (borg_class == CLASS_PRIEST &&
		borg_skill[BI_ISFIXWIS]) stats_needing_fix++;
	if (borg_class == CLASS_WARRIOR &&
		borg_skill[BI_ISFIXCON]) stats_needing_fix++;
	if (borg_skill[BI_MAXHP] <= 850 &&
		borg_skill[BI_ISFIXCON]) stats_needing_fix++;
	if (borg_skill[BI_MAXHP] <= 700 &&
		borg_skill[BI_ISFIXCON]) stats_needing_fix += 3;
	if (borg_class == CLASS_PRIEST && borg_skill[BI_MAXSP] < 100 &&
		borg_skill[BI_ISFIXWIS])
		stats_needing_fix += 5;
	if (borg_class == CLASS_MAGE && borg_skill[BI_MAXSP] < 100 &&
		borg_skill[BI_ISFIXINT])
		stats_needing_fix += 5;


	/*  Hack -- heal when confused. This is deadly. */
	/* This is checked twice, once, here, to see if he is in low danger
	 * and again at the end of borg_caution, when all other avenues have failed */
	if (borg_skill[BI_ISCONFUSED] && (randint0(100) < 85))
	{
		if ((hp_down >= 300) && danger - 300 < borg_skill[BI_CURHP] &&
			borg_quaff_potion(SV_POTION_HEALING))
		{
			borg_note("# Fixing Confusion. Level 1");
			return (TRUE);
		}
		if (danger - 20 < borg_skill[BI_CURHP] &&
			(borg_eat_food(SV_FOOD_CURE_CONFUSION) ||
			 borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			 borg_quaff_crit(FALSE) ||
			 borg_quaff_potion(SV_POTION_HEALING) ||
			 borg_use_staff_fail(SV_STAFF_HEALING) ||
			 borg_use_staff_fail(SV_STAFF_CURING)))
		{
			borg_note("# Fixing Confusion. Level 2");
			return (TRUE);
		}

		/* If my ability to use a teleport staff is really
		 * bad, then I should heal up then use the staff.
		 */
		/* Check for a charged teleport staff */
		if (borg_equips_staff_fail(SV_STAFF_TELEPORTATION))
		{
			/* check my skill, drink a potion */
			if ((borg_skill[BI_DEV] -
				 borg_items[borg_slot(TV_STAFF, SV_STAFF_TELEPORTATION)].level >
				 7) && (danger < (avoidance + 35) * 15 / 10) &&
				(borg_quaff_crit(FALSE) ||
				 borg_quaff_potion(SV_POTION_HEALING)))
			{
				borg_note("# Fixing Confusion. Level 3");
				return (TRUE);
			}
			/* However, if I am in really big trouble and there is no way I am going to be able to
			 * survive another round, take my chances on the staff.
			 */
			else if (danger >= avoidance * 15 / 10)
			{
				borg_note("# Too scary to fix Confusion. Level 4");
				return (FALSE);
			}

		}

	}
	/*  Hack -- heal when blind. This is deadly. */
	if (borg_skill[BI_ISBLIND] && (randint0(100) < 85))
	{
		/* if in extreme danger, use teleport then fix the
		 * blindness later.
		 */
		if (danger > avoidance * 25 / 10)
		{
			/* Check for a charged teleport staff */
			if (borg_equips_staff_fail(SV_STAFF_TELEPORTATION)) return (0);
		}
		if ((hp_down >= 300) && borg_quaff_potion(SV_POTION_HEALING))
		{
			return (TRUE);
		}
		/* Warriors with ESP won't need it so quickly */
		if (!
			(borg_class == CLASS_WARRIOR &&
			 borg_skill[BI_CURHP] > borg_skill[BI_MAXHP] / 4 &&
			 borg_skill[BI_ESP]))
		{
			if (borg_eat_food(SV_FOOD_CURE_BLINDNESS) ||
				borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
				borg_quaff_crit(TRUE) ||
				borg_quaff_potion(SV_POTION_HEALING) ||
				borg_use_staff_fail(SV_STAFF_HEALING) ||
				borg_use_staff_fail(SV_STAFF_CURING))
			{
				borg_note("# Fixing Blindness.");
				return (TRUE);
			}
		}
	}


	/* We generally try to conserve ez-heal pots */
	if ((borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) &&
		((hp_down >= 400) ||
		 (danger > borg_skill[BI_CURHP] * 5 && hp_down > 100)) &&
		borg_quaff_potion(SV_POTION_STAR_HEALING))
	{
		borg_note("# Fixing Confusion/Blind.");
		return (TRUE);
	}

	/*  Hack -- rest until healed */
	if ((!borg_skill[BI_ISBLIND] && !borg_skill[BI_ISPOISONED] &&
		 !borg_skill[BI_ISCUT] && !borg_goi && !borg_see_inv && !borg_shield &&
		 (borg_skill
		  [BI_CDEPTH] !=
		  100) &&
		 !borg_skill[BI_ISWEAK] && !borg_skill[BI_ISHUNGRY] &&
		 danger < avoidance / 5) && (borg_skill[BI_ISCONFUSED] ||
									 borg_skill[BI_ISIMAGE] ||
									 borg_skill[BI_ISAFRAID] ||
									 borg_skill[BI_ISSTUN] ||
									 borg_skill[BI_ISHEAVYSTUN] ||
									 borg_skill[BI_CURHP] < borg_skill[BI_MAXHP]
									 || (borg_skill[BI_CURSP] <
										 borg_skill[BI_MAXSP] * 6 / 10)) &&
		borg_check_rest() && !scaryguy_on_level &&
		(danger <= mb_ptr->fear) && !goal_fleeing)
	{
		/* check for then call lite in dark room before resting */
		if (!borg_check_lite_only())
		{
			/* Take note */
			borg_note(format("# Resting to restore HP/SP..."));

			/* Rest until done */
			borg_keypress('R');
			borg_keypress('&');
			borg_keypress('\n');

			/* Reset our panel clock, we need to be here */
			time_this_panel = 0;

			/* reset the inviso clock to avoid loops */
			need_see_inviso = borg_t - 50;

			/* Done */
			return (TRUE);
		}
		else
		{
			/* Must have been a dark room */
			borg_note(format
					  ("# Lighted the darkened room instead of resting."));
			return (TRUE);
		}
	}


	/* Healing and fighting Morgoth. */
	if (borg_fighting_unique >= 10)
	{
		if (borg_skill[BI_CURHP] <= 625 && borg_fighting_unique >= 10 && ((borg_skill[BI_CURHP] > 250 && borg_spell_fail(REALM_LIFE, 2, 6, 14)) ||	/* Holy Word */
																		  borg_use_staff_fail(SV_STAFF_HOLINESS) ||	/* Choose Life over *Healing* to fix stats */
																		  (stats_needing_fix >= 5 &&
																		   borg_quaff_potion
																		   (SV_POTION_LIFE))
																		  ||
																		  /* Choose Life over Healing if way down on pts */
																		  (hp_down > 500 && -1 == borg_slot(TV_POTION, SV_POTION_STAR_HEALING) && borg_quaff_potion(SV_POTION_LIFE)) || borg_quaff_potion(SV_POTION_STAR_HEALING) || borg_quaff_potion(SV_POTION_HEALING) || borg_spell_fail(REALM_LIFE, 2, 6, 17) ||	/* Holy Word */
																		  borg_spell_fail(REALM_LIFE, 3, 4, 15) ||	/* 2000 pts */
																		  borg_spell_fail(REALM_NATURE, 1, 7, allow_fail + 9) || borg_spell_fail(REALM_LIFE, 1, 7, 15) ||	/* 300 pts */
/* True Vampirism ? */
																		  borg_quaff_potion
																		  (SV_POTION_LIFE)))
		{
			borg_note("# Healing in Questor Combat.");
			return (TRUE);
		}
	}

	/* restore Mana */
	/* note, blow the staff charges easy because the staff will not last. */
	if (borg_skill[BI_CURSP] < (borg_skill[BI_MAXSP] / 5) &&
		(randint0(100) < 50))
	{
		if (borg_use_staff_fail(SV_STAFF_THE_MAGI))
		{
			borg_note("# Use Magi Staff");
			return (TRUE);
		}
	}
	/* blowing potions is harder */
	/* NOTE: must have enough mana to keep up GOI or do a HEAL */
	if (borg_skill[BI_CURSP] < (borg_skill[BI_MAXSP] / 10) ||
		((borg_skill[BI_CURSP] < 70 && borg_skill[BI_MAXSP] > 200) &&
		 (borg_goi <= borg_game_ratio * 3)))
	{
		/*  use the potion if battling a unique and not too dangerous */
		if (borg_fighting_unique >= 11 ||
			(borg_fighting_unique && danger < avoidance * 2) ||
			(borg_skill[BI_ATELEPORT] == 0 && danger > avoidance))
		{
			if (borg_use_staff_fail(SV_STAFF_THE_MAGI) ||
				borg_quaff_potion(SV_POTION_RESTORE_MANA))
			{
				borg_note("# Restored My Mana");
				return (TRUE);
			}
		}
	}

	/* if unhurt no healing needed */
	if (hp_down == 0)
		return FALSE;

	/* Don't bother healing if not in danger */
	if (danger == 0 && !borg_skill[BI_ISPOISONED] && !borg_skill[BI_ISCUT])
		return (FALSE);

	/* Restoring while fighting Morgoth */
	if (stats_needing_fix >= 5 && borg_fighting_unique >= 10 &&
		borg_skill[BI_CURHP] > 650 && borg_eat_food(SV_FOOD_RESTORING))
	{
		borg_note("# Trying to fix stats in combat.");
		return (TRUE);
	}

	/* No further Healing considerations if fighting Questors */
	if (borg_fighting_unique >= 10)
	{
		/* No further healing considerations right now */
		return (FALSE);
	}


	/* Hack -- heal when wounded a percent of the time */
	/* down 4/5 hp 0%                      */
	/* 3/4 hp 2%                           */
	/* 2/3 hp 20%                          */
	/* 1/2 hp 50%                          */
	/* 1/3 hp 75%                          */
	/* 1/4 hp 100%                         */

	chance = randint0(100);

	/* if we are fighting a unique increase the odds of healing */
	if (borg_fighting_unique) chance -= 10;

	/* if danger is close to the hp and healing will help, do it */
	if (danger >= borg_skill[BI_CURHP] && danger < borg_skill[BI_MAXHP])
		chance -= 75;
	else
	{
		if (borg_class != CLASS_PRIEST && borg_class != CLASS_PALADIN)
			chance -= 25;
	}


	/* Risky Borgs are less likely to heal in the fight */
	if (borg_plays_risky) chance += 3;

	if (!
		(((borg_skill[BI_CURHP] <= ((borg_skill[BI_MAXHP] * 4) / 5)) &&
		  (chance < 0)) ||
		 ((borg_skill[BI_CURHP] <= ((borg_skill[BI_MAXHP] * 3) / 4)) &&
		  (chance < 2)) ||
		 ((borg_skill[BI_CURHP] <= ((borg_skill[BI_MAXHP] * 2) / 3)) &&
		  (chance < 20)) ||
		 ((borg_skill[BI_CURHP] <= (borg_skill[BI_MAXHP] / 2)) && (chance < 50))
		 || ((borg_skill[BI_CURHP] <= (borg_skill[BI_MAXHP] / 3)) &&
			 (chance < 75)) ||
		 (borg_skill[BI_CURHP] <= (borg_skill[BI_MAXHP] / 4)) ||
		 borg_skill[BI_ISHEAVYSTUN] || borg_skill[BI_ISSTUN] ||
		 borg_skill[BI_ISPOISONED] || borg_skill[BI_ISCUT]))
		return FALSE;


	/* Cure light Wounds (2d10) */
	if (hp_down < 10 &&
		((danger / 2) < borg_skill[BI_CURHP] + 6) &&
		(borg_spell_fail(REALM_LIFE, 0, 1, allow_fail) ||
		 borg_spell_fail(REALM_ARCANE, 0, 7, allow_fail) ||
		 borg_spell_fail(REALM_NATURE, 0, 1, allow_fail) ||
		 borg_quaff_potion(SV_POTION_CURE_LIGHT) ||
		 borg_activate_artifact(ART_LOTHARANG, FALSE)))
	{
		borg_note("# Healing Level 1.");
		return (TRUE);
	}
	/* Cure Serious Wounds (4d10) */
	if (hp_down < 20 &&
		((danger / 2) < borg_skill[BI_CURHP] + 18) &&
		(borg_spell_fail(REALM_LIFE, 0, 6, allow_fail) ||
		 borg_spell_fail(REALM_ARCANE, 2, 3, allow_fail) ||
		 borg_mindcr_fail(MIND_ADRENALINE, 23, allow_fail) ||
		 borg_quaff_potion(SV_POTION_CURE_SERIOUS)))
	{
		borg_note("# Healing Level 2.");
		return (TRUE);
	}

	/* Cure Critical Wounds (6d10) */
	if (hp_down < 50 &&
		((danger / 2) < borg_skill[BI_CURHP] + 35) &&
		(borg_spell_fail(REALM_LIFE, 1, 2, allow_fail) ||
		 borg_mindcr_fail(MIND_ADRENALINE, 35, allow_fail) ||
		 borg_quaff_crit(FALSE)))
	{
		borg_note("# Healing Level 3.");
		return (TRUE);
	}

	/* Cure Mortal Wounds (8d10) */
#if 0							/* These spells are not in Z */
	if (hp_down < 120 &&
		((danger / 2) < borg_skill[BI_CURHP] + 55) &&
		(borg_spell_fail(REALM_LIFE, 2, 7, allow_fail) ||
		 borg_spell_fail(REALM_LIFE, 6, 1, allow_fail) ||
		 borg_mindcr_fail(MIND_ADRENALINE, 50, allow_fail) ||
		 /* ||
		    borg_quaff_crit(FALSE) don't want to CCW here, it would not help enough */
		))
	{
		borg_note("# Healing Level 4.");
		return (TRUE);
	}
#endif
	/* If in danger try  one more Cure Critical if it will help */
	if (danger >= borg_skill[BI_CURHP] &&
		danger < borg_skill[BI_MAXHP] &&
		borg_skill[BI_CURHP] < 20 && danger < 30 && borg_quaff_crit(TRUE))
	{
		borg_note("# Healing Level 5.");
		return (TRUE);
	}



	/* Generally continue to heal.  But if we are preparing for the end
	 * game uniques, then bail out here in order to save our heal pots.
	 * (unless morgoth is dead)
	 * Priests wont need to bail, they have good heal spells.
	 */
	if (borg_skill[BI_MAXDEPTH] >= 98 && !borg_skill[BI_KING] &&
		!borg_fighting_unique && borg_class != CLASS_PRIEST)
	{
		/* Bail out to save the heal pots for Morgoth */
		return (FALSE);
	}

	/* Heal step one (200hp) */
	if (hp_down < 250 &&
		danger / 2 < borg_skill[BI_CURHP] + 200 &&
		(((!borg_skill[BI_ATELEPORT] ||
		   borg_skill[BI_DEV] -
		   borg_items[borg_slot(TV_ROD, SV_ROD_HEALING)].level > 7) &&
		  borg_zap_rod(SV_ROD_HEALING)) ||
		 borg_activate_artifact(ART_SOULKEEPER, FALSE) ||
		 borg_activate_artifact(ART_GONDOR, FALSE) ||
		 borg_use_staff_fail(SV_STAFF_HEALING) ||
		 borg_spell_fail(REALM_LIFE, 1, 6, allow_fail) ||
		 borg_quaff_potion(SV_POTION_HEALING)))
	{
		borg_note("# Healing Level 6.");
		return (TRUE);
	}

	/* Heal step two (300hp) */
	if (hp_down < 350 && danger / 2 < borg_skill[BI_CURHP] + 300 && (borg_use_staff_fail(SV_STAFF_HEALING) || (borg_fighting_evil_unique && borg_spell_fail(REALM_LIFE, 2, 6, allow_fail)) ||	/* holy word */
/* Vamp Drain ? */
																	 borg_use_staff_fail
																	 (SV_STAFF_HOLINESS)
																	 ||
																	 borg_spell_fail
																	 (REALM_LIFE,
																	  1, 6,
																	  allow_fail)
																	 ||
																	 ((!borg_skill[BI_ATELEPORT] || borg_skill[BI_DEV] - borg_items[borg_slot(TV_ROD, SV_ROD_HEALING)].level > 7) && borg_zap_rod(SV_ROD_HEALING)) || borg_zap_rod(SV_ROD_HEALING) || borg_quaff_potion(SV_POTION_HEALING)))
	{
		borg_note("# Healing Level 7.");
		return (TRUE);
	}

	/* Healing step three (300hp).  */
	if (hp_down < 650 && danger / 2 < borg_skill[BI_CURHP] + 300 && ((borg_fighting_evil_unique && borg_spell_fail(REALM_LIFE, 2, 6, allow_fail)) ||	/* holy word */
/* Vamp Drain ? */
																	 ((!borg_skill[BI_ATELEPORT] || borg_skill[BI_DEV] - borg_items[borg_slot(TV_ROD, SV_ROD_HEALING)].level > 7) && borg_zap_rod(SV_ROD_HEALING)) || borg_spell_fail(REALM_LIFE, 1, 6, allow_fail) || borg_spell_fail(REALM_NATURE, 1, 7, allow_fail) || borg_use_staff_fail(SV_STAFF_HOLINESS) || borg_use_staff_fail(SV_STAFF_HEALING) || borg_quaff_potion(SV_POTION_HEALING) || borg_activate_artifact(ART_SOULKEEPER, FALSE) || borg_activate_artifact(ART_GONDOR, FALSE)))
	{
		borg_note("# Healing Level 8.");
		return (TRUE);
	}

	/* Healing final check.  Note that *heal* and Life potions are not
	 * wasted.  They are saved for Morgoth and emergencies.  The
	 * Emergency check is at the end of borg_caution().
	 */
	if (hp_down >= 650 && (danger / 2 < borg_skill[BI_CURHP] + 500) && ((borg_fighting_evil_unique && borg_spell_fail(REALM_LIFE, 2, 6, allow_fail)) ||	/* holy word */
/* Vamp Drain ? */
																		borg_spell_fail(REALM_LIFE, 3, 4, allow_fail) ||	/* 2000 */
																		borg_spell_fail(REALM_NATURE, 1, 7, allow_fail) ||	/* 1000 */
																		borg_use_staff_fail
																		(SV_STAFF_HOLINESS)
																		||
																		borg_use_staff_fail
																		(SV_STAFF_HEALING)
																		||
																		((!borg_skill[BI_ATELEPORT] || borg_skill[BI_DEV] - borg_items[borg_slot(TV_ROD, SV_ROD_HEALING)].level > 7) && borg_zap_rod(SV_ROD_HEALING)) || borg_quaff_potion(SV_POTION_HEALING) || borg_activate_artifact(ART_SOULKEEPER, FALSE) || borg_activate_artifact(ART_GONDOR, FALSE) || (borg_fighting_unique && (borg_quaff_potion(SV_POTION_HEALING) || borg_quaff_potion(SV_POTION_LIFE)))))
	{
		borg_note("# Healing Level 9.");
		return (TRUE);
	}

	/*** Cures ***/

	/* Dont do these in the middle of a fight, teleport out then try it */
	if (danger > avoidance * 2 / 10) return (FALSE);

	/* Hack -- cure poison when poisoned
	 * This was moved from borg_caution.
	 */
	if (borg_skill[BI_ISPOISONED] &&
		(borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2))
	{
		if (borg_spell_fail(REALM_LIFE, 1, 1, 60) ||
			borg_spell_fail(REALM_ARCANE, 1, 5, 60) ||
			borg_spell_fail(REALM_NATURE, 0, 7, 60) ||
			borg_quaff_potion(SV_POTION_CURE_POISON) ||
			borg_activate_artifact(ART_DAL, FALSE) ||
			borg_use_staff(SV_STAFF_CURING) ||
			borg_eat_food(SV_FOOD_CURE_POISON) ||
			borg_quaff_potion(SV_POTION_CURING) ||
			/* buy time */
			borg_quaff_crit(TRUE) ||
			borg_spell_fail(REALM_LIFE, 0, 6, 40) ||
			borg_spell_fail(REALM_LIFE, 0, 1, 40) ||
			borg_spell_fail(REALM_ARCANE, 0, 7, 40) ||
			borg_spell_fail(REALM_NATURE, 0, 1, 40) ||
			borg_use_staff_fail(SV_STAFF_HEALING))
		{
			borg_note("# Curing.");
			return (TRUE);
		}

		/* attempt to fix mana then poison on next round */
		if ((borg_spell_legal(REALM_LIFE, 1, 1) ||
			 borg_spell_legal(REALM_ARCANE, 1, 5) ||
			 borg_spell_legal(REALM_NATURE, 0, 7)) &&
			(borg_quaff_potion(SV_POTION_RESTORE_MANA)))
		{
			borg_note("# Curing next round.");
			return (TRUE);
		}
	}


	/* Hack -- cure poison when poisoned CRITICAL CHECK
	 */
	if (borg_skill[BI_ISPOISONED] &&
		(borg_skill[BI_CURHP] < 2 ||
		 borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 20))
	{
		int sv_mana = borg_skill[BI_CURSP];

		borg_skill[BI_CURSP] = borg_skill[BI_MAXSP];

		if (borg_spell(REALM_LIFE, 1, 1) ||
			borg_spell(REALM_ARCANE, 1, 5) || borg_spell(REALM_NATURE, 0, 7))
		{
			/* verify use of spell */
			/* borg_keypress('y'); */

			/* Flee! */
			borg_note("# Emergency Cure Poison! Gasp!!!....");

			return (TRUE);
		}
		borg_skill[BI_CURSP] = sv_mana;

		/* Quaff healing pots to buy some time- in this emergency.  */
		if (borg_quaff_potion(SV_POTION_CURE_LIGHT) ||
			borg_quaff_potion(SV_POTION_CURE_SERIOUS)) return (TRUE);

		/* Try to Restore Mana */
		if (borg_quaff_potion(SV_POTION_RESTORE_MANA)) return (TRUE);

		/* Emergency check on healing.  Borg_heal has already been checked but
		 * but we did not use our ez_heal potions.  All other attempts to save
		 * ourself have failed.  Use the ez_heal if I have it.
		 */
		if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 20 &&
			(borg_quaff_potion(SV_POTION_STAR_HEALING) ||
			 borg_quaff_potion(SV_POTION_LIFE) ||
			 borg_quaff_potion(SV_POTION_HEALING)))
		{
			return (TRUE);
		}

		/* Quaff unknown potions in this emergency.  We might get luck */
		if (borg_quaff_unknown()) return (TRUE);

		/* Eat unknown mushroom in this emergency.  We might get luck */
		if (borg_eat_unknown()) return (TRUE);

		/* Use unknown Staff in this emergency.  We might get luck */
		if (borg_use_unknown()) return (TRUE);

	}

	/* Hack -- cure wounds when bleeding, also critical check */
	if (borg_skill[BI_ISCUT] &&
		(borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 3 || randint0(100) < 20))
	{
		if (borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			borg_quaff_potion(SV_POTION_CURE_LIGHT) ||
			borg_quaff_crit(borg_skill[BI_CURHP] < 10) ||
			borg_spell_fail(REALM_LIFE, 1, 1, 100) ||
			borg_spell_fail(REALM_LIFE, 0, 6, 100) ||
			borg_spell_fail(REALM_LIFE, 0, 1, 100) ||
			borg_spell_fail(REALM_ARCANE, 2, 2, 100) ||
			borg_spell_fail(REALM_ARCANE, 0, 7, 100) ||
			borg_spell_fail(REALM_NATURE, 0, 7, 100) ||
			borg_spell_fail(REALM_NATURE, 0, 1, 100))
		{
			return (TRUE);
		}
	}
	/* bleeding and about to die CRITICAL CHECK */
	if (borg_skill[BI_ISCUT] &&
		((borg_skill[BI_CURHP] < 2) ||
		 borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 20))
	{
		int sv_mana = borg_skill[BI_CURSP];


		/* Quaff healing pots to buy some time- in this emergency.  */
		if (borg_quaff_potion(SV_POTION_CURE_LIGHT) ||
			borg_quaff_potion(SV_POTION_CURE_SERIOUS)) return (TRUE);

		/* Try to Restore Mana */
		if (borg_quaff_potion(SV_POTION_RESTORE_MANA)) return (TRUE);

		/* Emergency check on healing.  Borg_heal has already been checked but
		 * but we did not use our ez_heal potions.  All other attempts to save
		 * ourself have failed.  Use the ez_heal if I have it.
		 */
		if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 20 &&
			(borg_quaff_potion(SV_POTION_HEALING) ||
			 borg_quaff_potion(SV_POTION_STAR_HEALING) ||
			 borg_quaff_potion(SV_POTION_LIFE)))
		{
			return (TRUE);
		}

		borg_skill[BI_CURSP] = borg_skill[BI_MAXSP];

		/* Emergency use of spell */
		if (borg_spell_fail(REALM_LIFE, 1, 1, 100) ||
			borg_spell_fail(REALM_LIFE, 0, 6, 100) ||
			borg_spell_fail(REALM_LIFE, 0, 1, 100) ||
			borg_spell_fail(REALM_ARCANE, 2, 2, 100) ||
			borg_spell_fail(REALM_ARCANE, 0, 7, 100) ||
			borg_spell_fail(REALM_NATURE, 0, 7, 100) ||
			borg_spell_fail(REALM_NATURE, 0, 1, 100))

		{
			/* verify use of spell */
			/* borg_keypress('y'); */

			/* Flee! */
			borg_note("# Emergency Wound Patch! Gasp!!!....");

			return (TRUE);
		}
		borg_skill[BI_CURSP] = sv_mana;

		/* Quaff unknown potions in this emergency.  We might get luck */
		if (borg_quaff_unknown()) return (TRUE);

		/* Eat unknown mushroom in this emergency.  We might get luck */
		if (borg_eat_unknown()) return (TRUE);

		/* Use unknown Staff in this emergency.  We might get luck */
		if (borg_use_unknown()) return (TRUE);
	}

	/* nothing to do */
	return (FALSE);

}

/*
 * Be "cautious" and attempt to prevent death or dishonor.
 *
 * Strategy:
 *
 *   (1) Caution
 *   (1a) Analyze the situation
 *   (1a1) try to heal
 *   (1a2) try a defence
 *   (1b) Teleport from danger
 *   (1c) Handle critical stuff
 *   (1d) Retreat to happy grids
 *   (1e) Back away from danger
 *   (1f) Heal various conditions
 *
 *   (2) Attack
 *   (2a) Simulate possible attacks
 *   (2b) Perform optimal attack
 *
 *   (3) Recover
 *   (3a) Recover by spells/prayers
 *   (3b) Recover by items/etc
 *   (3c) Recover by resting
 *
 * XXX XXX XXX
 * In certain situations, the "proper" course of action is to simply
 * attack a nearby monster, since often most of the danger is due to
 * a single monster which can sometimes be killed in a single blow.
 *
 * Actually, both "borg_caution()" and "borg_recover()" need to
 * be more intelligent, and should probably take into account
 * such things as nearby monsters, and/or the relative advantage
 * of simply pummeling nearby monsters instead of recovering.
 *
 * Note that invisible/offscreen monsters contribute to the danger
 * of an extended "region" surrounding the observation, so we will
 * no longer rest near invisible monsters if they are dangerous.
 *
 * XXX XXX XXX
 * We should perhaps reduce the "fear" values of each region over
 * time, to take account of obsolete invisible monsters.
 *
 * Note that walking away from a fast monster is counter-productive,
 * since the monster will often just follow us, so we use a special
 * method which allows us to factor in the speed of the monster and
 * predict the state of the world after we move one step.  Of course,
 * walking away from a spell casting monster is even worse, since the
 * monster will just get to use the spell attack multiple times.  But,
 * if we are trying to get to known safety, then fleeing in such a way
 * might make sense.  Actually, this has been done too well, note that
 * it makes sense to flee some monsters, if they "stumble", or if we
 * are trying to get to stairs.  XXX XXX XXX
 *
 * Note that the "flow" routines attempt to avoid entering into
 * situations that are dangerous, but sometimes we do not see the
 * danger coming, and then we must attempt to survive by any means.
 *
 * We will attempt to "teleport" if the danger in the current situation,
 * as well as that resulting from attempting to "back away" from danger,
 * are sufficient to kill us in one or two blows.  This allows us to
 * avoid teleportation in situations where simply backing away is the
 * proper course of action, for example, when standing next to a nasty
 * stationary monster, but also to teleport when backing away will not
 * reduce the danger sufficiently.
 *
 * But note that in "nasty" situations (when we are running out of light,
 * or when we are starving, blind, confused, or hallucinating), we will
 * ignore the possibility of "backing away" from danger, when considering
 * the possibility of using "teleport" to escape.  But if the teleport
 * fails, we will still attempt to "retreat" or "back away" if possible.
 *
 * XXX XXX XXX Note that it should be possible to do some kind of nasty
 * "flow" algorithm which would use a priority queue, or some reasonably
 * efficient normal queue stuff, to determine the path which incurs the
 * smallest "cumulative danger", and minimizes the total path length.
 * It may even be sufficient to treat each step as having a cost equal
 * to the danger of the destination grid, plus one for the actual step.
 * This would allow the Borg to prefer a ten step path passing through
 * one grid with danger 10, to a five step path, where each step has
 * danger 9.  Currently, he often chooses paths of constant danger over
 * paths with small amounts of high danger.  However, the current method
 * is very fast, which is certainly a point in its favor...
 *
 * When in danger, attempt to "flee" by "teleport" or "recall", and if
 * this is not possible, attempt to "heal" damage, if needed, and else
 * attempt to "flee" by "running".
 *
 * XXX XXX XXX Both "borg_caution()" and "borg_recover()" should only
 * perform the "healing" tasks if they will cure more "damage"/"stuff"
 * than may be re-applied in the next turn, this should prevent using
 * wimpy healing spells next to dangerous monsters, and resting to regain
 * mana near a mana-drainer.
 *
 * Whenever we are in a situation in which, even when fully healed, we
 * could die in a single round, we set the "goal_fleeing" flag, and if
 * we could die in two rounds, we set the "goal_leaving" flag.
 *
 * In town, whenever we could die in two rounds if we were to stay still,
 * we set the "goal_leaving" flag.  In combination with the "retreat" and
 * the "back away" code, this should allow us to leave town before getting
 * into situations which might be fatal.
 *
 * Flag "goal_fleeing" means get off this level right now, using recall
 * if possible when we get a chance, and otherwise, take stairs, even if
 * it is very dangerous to do so.
 *
 * Flag "goal_leaving" means get off this level when possible, using
 * stairs if possible when we get a chance.
 *
 * We will also take stairs if we happen to be standing on them, and we
 * could die in two rounds.  This is often "safer" than teleportation,
 * and allows the "retreat" code to retreat towards stairs, knowing that
 * once there, we will leave the level.
 *
 * If we can, we should try to hit a monster with an offset  spell.
 * A Druj can not move but they are really dangerous.  So we should retreat
 * to a happy grid (meaning we have los and it does not), we should target
 * one space away from the bad guy then blast away with ball spells.
 */
bool borg_caution(void)
{
	int j, p;
	bool borg_surround = FALSE;
	bool nasty = FALSE;

	map_block *mb_ptr = map_loc(c_x, c_y);

	/*** Notice "nasty" situations ***/

	/* About to run out of light is extremely nasty */
	if (!borg_skill[BI_LITE] &&
		borg_items[INVEN_LITE].timeout < 250) nasty = TRUE;

	/* Starvation is nasty */
	if (borg_skill[BI_ISWEAK]) nasty = TRUE;

	/* Blind-ness is nasty */
	if (borg_skill[BI_ISBLIND]) nasty = TRUE;

	/* Confusion is nasty */
	if (borg_skill[BI_ISCONFUSED]) nasty = TRUE;

	/* Hallucination is nasty */
	if (borg_skill[BI_ISIMAGE]) nasty = TRUE;

	/* if on level 100 and not ready for Morgoth, run */
	if (borg_skill[BI_CDEPTH] == 100 && borg_t - borg_began < 10)
	{
		if (borg_ready_morgoth == 0 && !borg_skill[BI_KING])
		{
			/* teleport level up to 99 to finish uniques */
			if (borg_spell_fail(REALM_SORCERY, 2, 6, 100) ||
				borg_spell_fail(REALM_TRUMP, 1, 5, 100) ||
				borg_spell_fail(REALM_ARCANE, 3, 1, 100) ||
				borg_read_scroll(SV_SCROLL_TELEPORT_LEVEL))
			{
				borg_note("# Rising one dlevel (Not ready for Morgoth)");
				return (TRUE);
			}

			/* Start leaving */
			if (!goal_leaving)
			{
				/* Note */
				borg_note("# Leaving (Not ready for Morgoth now)");

				/* Start leaving */
				goal_leaving = TRUE;
			}
		}
	}

	/*** Evaluate local danger ***/

	/* am I fighting a unique or a summoner, or scaryguy? */
	borg_near_monster_type(borg_skill[BI_MAXCLEVEL] < 15 ? MAX_SIGHT : 12);
	borg_surround = borg_surrounded();


	/* Only allow three 'escapes' per level unless heading for morogoth
	   or fighting a unique, then allow 7. */
	if ((borg_escapes > 3 && !unique_on_level && !borg_ready_morgoth &&
		 !borg_quest_level) || borg_escapes > 7)
	{
		/* No leaving if going after questors */
		if (borg_skill[BI_CDEPTH] <= 98)
		{
			/* Start leaving */
			if (!goal_leaving)
			{
				/* Note */
				borg_note("# Leaving (Too many escapes)");

				/* Start leaving */
				goal_leaving = TRUE;
			}

			/* Start fleeing */
			if (!goal_fleeing && borg_escapes > 3)
			{
				/* Note */
				borg_note("# Fleeing (Too many escapes)");

				/* Start fleeing */
				goal_fleeing = TRUE;
			}
		}
	}

	/* No hanging around if nasty here. */
	if (scaryguy_on_level)
	{
		/* Start leaving */
		if (!goal_leaving)
		{
			/* Note */
			borg_note("# Leaving (Scary guy on level)");

			/* Start leaving */
			goal_leaving = TRUE;
		}

		/* Start fleeing */
		if (!goal_fleeing)
		{
			/* Note */
			borg_note("# Fleeing (Scary guy on level)");

			/* Start fleeing */
			goal_fleeing = TRUE;
		}
	}

	/* Look around */
	p = borg_danger(c_y, c_x, 1, TRUE);

	/* Describe (briefly) the current situation */
	/* Danger (ignore stupid "fear" danger) */
	if (borg_goi || (p > avoidance / 10) || (p > mb_ptr->fear))
	{
		/* Describe (briefly) the current situation */
		borg_note(format
				  ("# Loc:%d,%d Dep:%d Lev:%d HP:%d/%d SP:%d/%d Danger:p=%d",
				   c_y, c_x, borg_skill[BI_CDEPTH], borg_skill[BI_CLEVEL],
				   borg_skill[BI_CURHP], borg_skill[BI_MAXHP],
				   borg_skill[BI_CURSP], borg_skill[BI_MAXSP], p));
		if (borg_goi)
		{
			borg_note(format
					  ("# Protected by GOI (borg turns:%d; game turns:%d)",
					   borg_goi / borg_game_ratio, p_ptr->invuln));
		}
		if (borg_shield)
		{
			borg_note(format("# Protected by Mystic Shield"));
		}
		if (borg_prot_from_evil)
		{
			borg_note(format("# Protected by PFE"));
		}
	}
	/* Comment on glyph */
	if (track_glyph_num)
	{
		int i;
		for (i = 0; i < track_glyph_num; i++)
		{
			/* Enqueue the grid */
			if ((track_glyph_y[i] == c_y) && (track_glyph_x[i] == c_x))
			{
				/* if standing on one */
				borg_note(format("# Standing on Glyph"));
			}
		}
	}
	/* Comment on stair */
	if (track_less_num)
	{
		int i;
		for (i = 0; i < track_less_num; i++)
		{
			/* Enqueue the grid */
			if ((track_less_y[i] == c_y) && (track_less_x[i] == c_x))
			{
				/* if standing on one */
				borg_note(format("# Standing on up-stairs"));
			}
		}
	}
	/* Comment on stair */
	if (track_more_num)
	{
		int i;
		for (i = 0; i < track_more_num; i++)
		{
			/* Enqueue the grid */
			if ((track_more_y[i] == c_y) && (track_more_x[i] == c_x))
			{
				/* if standing on one */
				borg_note(format("# Standing on dn-stairs"));
			}
		}
	}

	if (borg_class == CLASS_MAGE)
	{
		/* do some defence before running away */
		if (borg_defend(p))
			return TRUE;

		/* try healing before running away */
		if (borg_heal(p))
			return TRUE;
	}
	else
	{
		/* try healing before running away */
		if (borg_heal(p))
			return TRUE;

		/* do some defence before running away! */
		if (borg_defend(p))
			return TRUE;
	}


	if (borg_uses_swaps)
	{
		/* do some swapping before running away! */
		if (p > (avoidance / 3) || borg_goi)
		{
			if (borg_backup_swap(p))
				return TRUE;
		}
	}

	/* If I am waiting for recall,  & safe, then stay put. */
	if (goal_recalling && borg_check_rest() && borg_skill[BI_CDEPTH])
	{
		/* note the resting */
		borg_note("# Resting here, waiting for Recall.");

		/* rest here until lift off */
		borg_keypress('R');
		borg_keypress('5');
		borg_keypress('0');
		borg_keypress('0');
		borg_keypress('\n');

		return (TRUE);
	}

	/* If I am waiting for recall in town */
	if (goal_recalling && goal_recalling <= (borg_game_ratio * 2) &&
		!borg_skill[BI_CDEPTH])
	{
		/* Cast GOI just before returning to dungeon */
		if (!borg_goi &&
			(borg_spell_fail(REALM_LIFE, 3, 7, 15) ||
			 borg_spell_fail(REALM_SORCERY, 3, 7, 15)))
		{
			borg_note("# Casting GOI before Recall activates.");
			return (TRUE);
		}

		/* Cast PFE just before returning to dungeon */
		if (!borg_prot_from_evil && borg_spell_fail(REALM_LIFE, 1, 5, 15))
		{
			borg_note("# Casting PFE before Recall activates.");
			return (TRUE);
		}

		/* Cast other good prep things */
		if ((!borg_speed && borg_spell_fail(REALM_SORCERY, 1, 5, 15)) ||
			(my_oppose_fire + my_oppose_cold + my_oppose_acid +
			 my_oppose_elec + my_oppose_pois < 3 &&
			 (borg_spell_fail(REALM_NATURE, 2, 3, 15) ||
			  borg_spell_fail(REALM_NATURE, 0, 6, 15))) ||
			(my_oppose_fire + my_oppose_cold + my_oppose_elec < 2 &&
			 borg_spell_fail(REALM_NATURE, 0, 6, 15)) ||
			(!borg_shield && !borg_goi &&
			 borg_spell_fail(REALM_NATURE, 2, 2, 15)) || (!borg_hero &&
														  borg_spell_fail
														  (REALM_SORCERY, 7, 0,
														   15)) ||
			(!borg_berserk && borg_spell_fail(REALM_DEATH, 2, 0, 15)) ||
			(!borg_bless && borg_spell_fail(REALM_LIFE, 0, 2, 15)) ||
			(!borg_speed && borg_mindcr_fail(MIND_ADRENALINE, 35, 15)) ||
			(!borg_hero && borg_mindcr_fail(MIND_ADRENALINE, 35, 15)))
		{
			borg_note("# Casting preparatory spell before Recall activates.");
			return (TRUE);
		}

	}

	/*** Danger ***/

	/* Impending doom */
	/* Don't take off in the middle of a fight */
	/* just to restock and it is useless to restock */
	/* if you have just left town. */
	if (borg_restock(borg_skill[BI_CDEPTH]) &&
		!borg_fighting_unique && (borg_time_town + (borg_t - borg_began)) > 200)
	{
		/* Start leaving */
		if (!goal_leaving)
		{
			/* Note */
			borg_note(format
					  ("# Leaving (restock) %s",
					   borg_restock(borg_skill[BI_CDEPTH])));

			/* Start leaving */
			goal_leaving = TRUE;
		}
		/* Start fleeing */
		if (!goal_fleeing && borg_skill[BI_ACCW] < 2)
		{
			/* Flee */
			borg_note(format
					  ("# Fleeing (restock) %s",
					   borg_restock(borg_skill[BI_CDEPTH])));

			/* Start fleeing */
			goal_fleeing = TRUE;
		}
	}
	/* Excessive danger */
	else if (p > (borg_skill[BI_CURHP] * 2))
	{
		/* Start fleeing */
		/* do not flee level if going after Morgoth or fighting a unique */
		if (!goal_fleeing && !borg_fighting_unique &&
			(borg_skill[BI_CLEVEL] < 50) && !vault_on_level && !borg_quest_level
			&& (borg_skill[BI_CDEPTH] < 100 && borg_ready_morgoth == 1))
		{
			/* Note */
			borg_note("# Fleeing (excessive danger)");

			/* Start fleeing */
			goal_fleeing = TRUE;
		}
	}
	/* Potential danger (near death) in town */
	else if (!borg_skill[BI_CDEPTH] && (p > borg_skill[BI_CURHP]) &&
			 (borg_skill[BI_CLEVEL] < 50))
	{
		/* Flee now */
		if (!goal_leaving)
		{
			/* Flee! */
			borg_note("# Leaving (potential danger)");

			/* Start leaving */
			goal_leaving = TRUE;
		}
	}


	/*** Stairs ***/

	/* Leaving or Fleeing, take stairs */
	if (goal_leaving || goal_fleeing || scaryguy_on_level)
	{
		/* Take next stairs */
		stair_less = goal_fleeing;

		if (borg_ready_morgoth == 0 && !borg_skill[BI_KING])
			stair_less = TRUE;

		if (scaryguy_on_level) stair_less = TRUE;

		/* Only go down if fleeing or prepared, but not when starving.
		 * or lacking on food */
		stair_more = goal_fleeing;
		if ((cptr)NULL == borg_prepared(borg_skill[BI_CDEPTH] + 1))
			stair_more = TRUE;

		/* Its ok to go one level deep if evading scary guy */
		if (scaryguy_on_level) stair_more = TRUE;

		if (borg_skill[BI_CUR_LITE] == 0 || borg_skill[BI_ISHUNGRY] ||
			borg_skill[BI_ISWEAK] || borg_skill[BI_FOOD] < 2)
			stair_more = FALSE;

		/* if fleeing town, then dive */
		if (!borg_skill[BI_CDEPTH]) stair_more = TRUE;


	}

	/* Take stairs up */
	if (stair_less)
	{
		/* Current grid */
		map_block *mb_ptr = map_loc(c_x, c_y);

		/* Usable stairs */
		if (mb_ptr->feat == FEAT_LESS)
		{
			borg_keypress('<');

			/* Success */
			return (TRUE);
		}
	}


	/* Take stairs down */
	if (stair_more && !goal_recalling)
	{
		/* Current grid */
		map_block *mb_ptr = map_loc(c_x, c_y);

		/* Usable stairs */
		if (mb_ptr->feat == FEAT_MORE)
		{
			/* Cast GOI just before returning to dungeon */
			if (borg_skill[BI_CURSP] > borg_skill[BI_MAXSP] * 6 / 10 &&
				!borg_goi &&
				(borg_spell_fail(REALM_LIFE, 3, 7, 15) ||
				 borg_spell_fail(REALM_SORCERY, 3, 7, 15)))
			{
				borg_note("# Casting GOI before taking stairs.");
				return (TRUE);
			}

			/* Cast PFE just before returning to dungeon */
			if (borg_skill[BI_CURSP] > borg_skill[BI_MAXSP] * 6 / 10 &&
				!borg_prot_from_evil && borg_spell_fail(REALM_LIFE, 1, 5, 15))
			{
				borg_note("# Casting PFE before taking stairs.");
				return (TRUE);
			}

			/* Cast other good prep things */
			if ((borg_skill[BI_CURSP] > borg_skill[BI_MAXSP] * 6 / 10) &&
				((!borg_speed && borg_spell_fail(REALM_SORCERY, 1, 5, 15)) ||
				 (my_oppose_fire + my_oppose_cold + my_oppose_acid +
				  my_oppose_elec + my_oppose_pois < 3 &&
				  (borg_spell_fail(REALM_NATURE, 2, 3, 15) ||
				   borg_spell_fail(REALM_NATURE, 0, 6, 15))) ||
				 (my_oppose_fire + my_oppose_cold + my_oppose_elec < 2 &&
				  borg_spell_fail(REALM_NATURE, 0, 6, 15)) ||
				 (!borg_shield && !borg_goi &&
				  borg_spell_fail(REALM_NATURE, 2, 2, 15)) || (!borg_hero &&
															   borg_spell_fail
															   (REALM_SORCERY,
																7, 0, 15)) ||
				 (!borg_berserk && borg_spell_fail(REALM_DEATH, 2, 0, 15)) ||
				 (!borg_bless && borg_spell_fail(REALM_LIFE, 0, 2, 15)) ||
				 (!borg_speed && borg_mindcr_fail(MIND_ADRENALINE, 35, 15)) ||
				 (!borg_hero && borg_mindcr_fail(MIND_ADRENALINE, 35, 15))))

			{
				borg_note("# Casting preparatory spell before taking stairs.");
				return (TRUE);
			}

			/* Take the stairs */
			borg_keypress('>');

			/* Success */
			return (TRUE);
		}
	}


	/*** Deal with critical situations ***/

	/* Hack -- require light */
	if (!borg_skill[BI_LITE])
	{
		borg_item *item = &borg_items[INVEN_LITE];

		/* Must have light -- Refuel current torch */
		if ((item->tval == TV_LITE) && (item->sval == SV_LITE_TORCH))
		{
			/* Try to refuel the torch */
			if ((item->timeout < 500) && borg_refuel_torch()) return (TRUE);
		}

		/* Must have light -- Refuel current lantern */
		if ((item->tval == TV_LITE) && (item->sval == SV_LITE_LANTERN))
		{
			/* Try to refill the lantern */
			if ((item->timeout < 1000) && borg_refuel_lantern()) return (TRUE);
		}

		/* Flee for fuel */
		if (borg_skill[BI_CDEPTH] && (item->timeout < 250))
		{
			/* Start leaving */
			if (!goal_leaving)
			{
				/* Flee */
				borg_note("# Leaving (need fuel)");

				/* Start leaving */
				goal_leaving = TRUE;
			}
		}
	}

	/* Hack -- prevent starvation */
	if (borg_skill[BI_ISWEAK])
	{
		/* Attempt to satisfy hunger */
		if (borg_eat_food_any() ||
			borg_spell_fail(REALM_SORCERY, 2, 0, 45) ||
			borg_spell_fail(REALM_LIFE, 0, 7, 45) ||
			borg_spell_fail(REALM_ARCANE, 2, 7, 45) ||
			borg_spell_fail(REALM_NATURE, 0, 3, 45))
		{
			/* Success */
			return (TRUE);
		}

		/* Try to restore mana then cast the spell next round */
		if (borg_quaff_potion(SV_POTION_RESTORE_MANA)) return (TRUE);

		/* Flee for food */
		if (borg_skill[BI_CDEPTH])
		{
			/* Start leaving */
			if (!goal_leaving)
			{
				/* Flee */
				borg_note("# Leaving (need food)");

				/* Start leaving */
				goal_leaving = TRUE;
			}

			/* Start fleeing */
			if (!goal_fleeing)
			{
				/* Flee */
				borg_note("# Fleeing (need food)");

				/* Start fleeing */
				goal_fleeing = TRUE;
			}
		}
	}

	/* Prevent breeder explosions when low level */
	if (breeder_level && borg_skill[BI_CLEVEL] < 15)
	{
		/* Start leaving */
		if (!goal_leaving)
		{
			/* Flee */
			borg_note("# Leaving (breeder level)");

			/* Start leaving */
			goal_leaving = TRUE;
		}

	}

	/*** Flee on foot ***/

	/* Desperation Head for stairs */
	/* If you are low level and near the stairs and you can */
	/* hop onto them in very few steps, try to head to them */
	/* out of desperation */
	if (track_less_num &&
		(goal_fleeing || (p > avoidance && borg_skill[BI_CLEVEL] < 35)))
	{
		int y, x, i;
		int b_j = -1;

		/* Check for an existing "up stairs" */
		for (i = 0; i < track_less_num; i++)
		{
			x = track_less_x[i];
			y = track_less_y[i];

			/* How far is the nearest up stairs */
			j = distance(c_y, c_x, y, x);

			/* skip the closer ones */
			if (b_j >= j) continue;

			/* track it */
			b_j = j;
		}
		/* If you are within a few (3) steps of the stairs */
		/* and you can take some damage to get there */
		/* go for it */
		if (b_j < 3 && p < borg_skill[BI_CURHP])
		{
			borg_desperate = TRUE;
			if (borg_flow_stair_less(GOAL_FLEE))
			{
				/* Note */
				borg_note("# Desperate for Stairs (one)");

				borg_desperate = FALSE;
				return (TRUE);
			}
			borg_desperate = FALSE;
		}

		/* If you are next to steps of the stairs go for it */
		if (b_j <= 2)
		{
			borg_desperate = TRUE;
			if (borg_flow_stair_less(GOAL_FLEE))
			{
				/* Note */
				borg_note("# Desperate for Stairs (two)");

				borg_desperate = FALSE;
				return (TRUE);
			}
			borg_desperate = FALSE;
		}

		/* Low level guys tend to waste money reading the recall scrolls */
		if (b_j < 15 && scaryguy_on_level && borg_skill[BI_CLEVEL] < 20)
		{
			borg_desperate = TRUE;
			if (borg_flow_stair_less(GOAL_FLEE))
			{
				/* Note */
				borg_note("# Desperate for Stairs (three)");

				borg_desperate = FALSE;
				return (TRUE);
			}
			borg_desperate = FALSE;
		}
	}


	/* Strategic retreat */
	/* Do not retreat if */
	/* 1) we are icky (poisoned, blind, confused etc */
	/* 2) we are boosting our avoidance because we are stuck */
	if ((p > avoidance / 3 && !nasty && !borg_no_retreat) ||
		(borg_surround && p != 0))
	{
		int d, b_d = -1;
		int r, b_r = -1;

		int b_x = c_x;
		int b_y = c_y;

		/* Scan the useful viewable grids */
		for (j = 1; j < borg_view_n; j++)
		{
			int x1 = c_x;
			int y1 = c_y;

			int x2 = borg_view_x[j];
			int y2 = borg_view_y[j];

			/* Cant if confused: no way to predict motion */
			if (borg_skill[BI_ISCONFUSED]) continue;

			/* Require "floor" grids */
			if (!borg_cave_floor_bold(y2, x2)) continue;

			/* XXX -- Borgs in an unexplored hall (& with only a torch
			 * will always return FALSE for Happy Grids:
			 *
			 *  222222      Where 2 = unknown grid.  Borg has a torch.
			 *  2221.#      Borg will consider both the . and the 1
			 *     #@#      for a retreat from the C. But the . will be
			 *     #C#      false d/t adjacent wall to the east.  1 will
			 *     #'#      will be false d/t unknown grid to the west.
			 *              So he makes no attempt to retreat.
			 * However, the next function (backing away), allows him
			 * to back up to 1 safely.
			 *
			 * To play safer, the borg should not retreat to grids where
			 * he has not previously been.  This tends to run him into
			 * more monsters.  It is better for him to retreat to grids
			 * previously travelled, where the monsters are most likely
			 * dead, and the path is clear.  However, there is not (yet)
			 * tag for those grids.  Something like BORG_BEEN would work.
			 */

			/* Require "happy" grids (most of the time) */
			if (!borg_happy_grid_bold(y2, x2)) continue;

			/* Track "nearest" grid */
			if (b_r >= 0)
			{
				int ay = ((y2 > y1) ? (y2 - y1) : (y1 - y2));
				int ax = ((x2 > x1) ? (x2 - x1) : (x1 - x2));

				/* Ignore "distant" locations */
				if ((ax > b_r) || (ay > b_r)) continue;
			}

			/* Reset */
			r = 0;

			/* Simulate movement */
			while (1)
			{
				map_block *mb_ptr;

				/* Obtain direction */
				d = borg_goto_dir(y1, x1, y2, x2);

				/* Verify direction */
				if ((d == 0) || (d == 5)) break;

				/* Track distance */
				r++;

				/* Simulate the step */
				y1 += ddy[d];
				x1 += ddx[d];

				/* Bounds checking */
				if (!map_in_bounds(x1, y1)) break;

				/* Obtain the grid */
				mb_ptr = map_loc(x1, y1);

				/* Require floor */
				if (borg_cave_wall_grid(mb_ptr)) break;

				/* Require line of sight */
				if (!borg_los(y1, x1, y2, x2)) break;

				/* Check danger of that spot (over time) */
				if (!borg_surround &&
					borg_danger(y1, x1, r + 1, TRUE) >= p) break;

				/* make sure it is not dangerous to take the first step; unless surrounded. */
				if (r == 1)
				{
					/* Not surrounded */
					if (!borg_surround)
					{
						if (borg_danger(y2, x2, 1, TRUE) >= avoidance * 6 / 10)
							break;
					}
					else
						/* Surrounded, try to back-up */
					{
						if (borg_danger(y2, x2, 1, TRUE) >=
							(b_r <= 3 ? avoidance * 15 / 10 : avoidance))
							break;
					}
				}

				/* Skip monsters */
				if (mb_ptr->monster) break;
#if 0
				/* Skip traps */
				if ((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) &&
					(mb_ptr->feat <= FEAT_TRAP_SLEEP)) break;
#endif /* 0 */
				/* Safe arrival */
				if ((x1 == x2) && (y1 == y2))
				{
					/* Save distance */
					b_r = r;

					/* Save location */
					b_x = x2;
					b_y = y2;

					/* Done */
					break;
				}
			}
		}

		/* Retreat */
		if (b_r >= 0)
		{
			/* Save direction */
			b_d = borg_goto_dir(c_y, c_x, b_y, b_x);

			/* Hack -- set goal */
			g_x = c_x + ddx[b_d];
			g_y = c_y + ddy[b_d];

			/* Note */
			borg_note(format
					  ("# Retreating to %d,%d (distance %d) via %d,%d (%d > %d)",
					   b_y, b_x, b_r, g_y, g_x, p, borg_danger(g_y, g_x, 2,
															   TRUE)));

			/* Strategic retreat */
			borg_keypress(I2D(b_d));

			/* Success */
			return (TRUE);
		}
	}

	/*** Escape if possible ***/

	/* Attempt to escape via spells */
	if (borg_escape(p))
	{
		/* increment the escapes this level counter */
		borg_escapes++;

		/* Success */
		return (TRUE);
	}

	/*** Back away ***/
	/* Do not back up if */
	/* 1) we are icky (poisoned, blind, confused etc */
	/* 2) we are boosting our avoidance because we are stuck */
	if ((p > avoidance / 3 && !nasty && !borg_no_retreat) ||
		(borg_surround && p != 0))
	{
		int i = -1, b_i = -1;
		int k = -1, b_k = -1;
		int f = -1, b_f = -1;

		/* Current danger */
		b_k = p;

		/* Fake the danger down if surounded so that he can move. */
		if (borg_surround) b_k = (b_k * 6 / 10);

		/* Check the freedom */
		b_f = borg_freedom(c_y, c_x);

		/* Attempt to find a better grid */
		for (i = 0; i < 8; i++)
		{
			int x = c_x + ddx_ddd[i];
			int y = c_y + ddy_ddd[i];

			map_block *mb_ptr;

			/* Bounds checking */
			if (!map_in_bounds(x, y)) continue;

			/* Access the grid */
			mb_ptr = map_loc(x, y);

			/* Cant if confused: no way to predict motion */
			if (borg_skill[BI_ISCONFUSED]) continue;

			/* Skip walls/doors */
			if (borg_cave_wall_grid(mb_ptr)) continue;

			/* Skip monster grids */
			if (mb_ptr->monster) continue;
#if 0
			/* Mega-Hack -- skip stores XXX XXX XXX */
			if ((mb_ptr->feat >= FEAT_SHOP_HEAD) &&
				(mb_ptr->feat <= FEAT_SHOP_TAIL)) continue;

			/* Mega-Hack -- skip traps XXX XXX XXX */
			if ((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) &&
				(mb_ptr->feat <= FEAT_TRAP_SLEEP)) continue;
#endif /* 0 */

			/* Extract the danger there */
			k = borg_danger(y, x, 2, TRUE);

			/* Skip higher danger */
			/* note: if surrounded, then b_k has been lowered. */
			if (b_k < k) continue;

			/* Check the freedom there */
			f = borg_freedom(y, x);

			/* Danger is the same */
			if (b_k == k)
			{
				/* If I am low level, reward backing-up if safe */
				if (borg_skill[BI_CLEVEL] <= 3 &&
					(borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] ||
					 borg_skill[BI_CURSP] < borg_skill[BI_CURSP]))
				{
					/* do consider the retreat */
				}
				/* Freedom of my grid is better than the next grid
				 * so stay put and fight.
				 */
				else if (b_f > f) continue;
				else
					continue;
			}

			/* Save the info */
			b_i = i;
			b_k = k;
			b_f = f;
		}

		/* Back away */
		if (b_i >= 0)
		{
			/* Hack -- set goal */
			g_x = c_x + ddx_ddd[b_i];
			g_y = c_y + ddy_ddd[b_i];

			/* Note */
			borg_note(format("# Backing up to %d,%d (%d > %d)",
							 g_x, g_y, p, borg_danger(g_y, g_x, 2, TRUE)));

			/* Back away from danger */
			borg_keypress(I2D(ddd[b_i]));

			/* Success */
			return (TRUE);
		}

	}


	/*** Cures ***/

	/* cure confusion, second check, first (slightly different) in borg_heal */
	if (borg_skill[BI_ISCONFUSED])
	{
		if (borg_skill[BI_MAXHP] - borg_skill[BI_CURHP] >= 300 &&
			(borg_quaff_potion(SV_POTION_HEALING) ||
			 borg_quaff_potion(SV_POTION_STAR_HEALING) ||
			 borg_quaff_potion(SV_POTION_LIFE)))
		{
			return (TRUE);
		}
		if (borg_eat_food(SV_FOOD_CURE_CONFUSION) ||
			borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			borg_quaff_crit(FALSE) ||
			borg_quaff_potion(SV_POTION_HEALING) ||
			borg_use_staff_fail(SV_STAFF_HEALING))
		{
			return (TRUE);
		}
	}

	/* Hack -- cure fear when afraid */
	if (borg_skill[BI_ISAFRAID] &&
		(randint0(100) < 70 ||
		 (borg_class == CLASS_WARRIOR && borg_skill[BI_AMISSILES] <= 0)))
	{
		if (borg_spell_fail(REALM_LIFE, 0, 3, 100) ||
			borg_mindcr_fail(MIND_ADRENALINE, 23, 100) ||
			borg_quaff_potion(SV_POTION_BOLDNESS) ||
			borg_quaff_potion(SV_POTION_HEROISM) ||
			borg_quaff_potion(SV_POTION_BERSERK_STRENGTH) ||
			borg_activate_artifact(ART_DAL, FALSE) ||
			borg_racial(RACE_HALF_ORC) || borg_racial(RACE_HALF_TROLL))
		{
			return (TRUE);
		}
	}


	/*** Note impending death XXX XXX XXX ***/

	/* Flee from low hit-points */
	if (((borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 3) ||
		 ((borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2) &&
		  borg_skill[BI_CURHP] < (borg_skill[BI_CLEVEL] * 3))) &&
		(borg_skill[BI_ACCW] < 3) && (borg_skill[BI_AHEAL] < 1))
	{
		/* Flee from low hit-points */
		if (borg_skill[BI_CDEPTH] && (randint0(100) < 25))
		{
			/* Start leaving */
			if (!goal_leaving)
			{
				/* Flee */
				borg_note("# Leaving (low hit-points)");

				/* Start leaving */
				goal_leaving = TRUE;

			}
			/* Start fleeing */
			if (!goal_fleeing)
			{
				/* Flee */
				borg_note("# Fleeing (low hit-points)");

				/* Start fleeing */
				goal_fleeing = TRUE;
			}

		}
	}

	/* Flee from bleeding wounds or poison and no heals */
	if ((borg_skill[BI_ISCUT] || borg_skill[BI_ISPOISONED]) &&
		(borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2))
	{
		/* Flee from bleeding wounds */
		if (borg_skill[BI_CDEPTH] && (randint0(100) < 25))
		{
			/* Start leaving */
			if (!goal_leaving)
			{
				/* Flee */
				borg_note("# Leaving (bleeding/posion)");

				/* Start leaving */
				goal_leaving = TRUE;
			}

			/* Start fleeing */
			if (!goal_fleeing)
			{
				/* Flee */
				borg_note("# Fleeing (bleeding/poison)");

				/* Start fleeing */
				goal_fleeing = TRUE;
			}
		}
	}

	/* Emergency check on healing.  Borg_heal has already been checked but
	 * but we did not use our ez_heal potions.  All other attempts to save
	 * ourself have failed.  Use the ez_heal if I have it.
	 */
	if ((borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 10 ||	/* dangerously low HP */
		 (borg_skill[BI_ATELEPORT] + borg_skill[BI_AESCAPE] == 0 && borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 4)) &&	/* low on escapes */
		(p > borg_skill[BI_CURHP] * 2 ||	/* extreme danger */
		 (p > borg_skill[BI_CURHP] && borg_skill[BI_AEZHEAL] > 5) ||	/* moderate danger, lots of heals */
		 (p > borg_skill[BI_CURHP] * 12 / 10 && borg_skill[BI_MAXHP] - borg_skill[BI_CURHP] >= 400 && borg_fighting_unique && borg_skill[BI_CDEPTH] >= 85)) &&	/* moderate danger, unique, deep */
		(borg_quaff_potion(SV_POTION_HEALING) ||
		 borg_quaff_potion(SV_POTION_STAR_HEALING) ||
		 borg_quaff_potion(SV_POTION_LIFE)))
	{
		borg_note("# Using reserve EZ_Heal.");
		return (TRUE);
	}

	/* Hack -- use "recall" to flee if possible */
	if (goal_fleeing && borg_skill[BI_CDEPTH] && (borg_recall()))
	{
		/* Note */
		borg_note("# Fleeing the level (recall)");

		/* Success */
		return (TRUE);
	}

	/* If I am waiting for recall,and in danger, buy time with
	 * phase and cure_anythings.
	 */
	if (goal_recalling && (p > avoidance * 2))
	{
		if (!borg_skill[BI_ISCONFUSED] && !borg_skill[BI_ISBLIND] &&
			borg_skill[BI_MAXSP] > 60 &&
			borg_skill[BI_CURSP] < (borg_skill[BI_CURSP] / 4) &&
			borg_quaff_potion(SV_POTION_RESTORE_MANA))
		{
			borg_note("# Buying time waiting for Recall.  Step 1.");
			return (TRUE);
		}

		if (borg_read_scroll(SV_SCROLL_PHASE_DOOR) ||
			borg_spell_fail(REALM_ARCANE, 0, 4, 25) ||
			borg_spell_fail(REALM_SORCERY, 0, 1, 25) ||
			borg_spell_fail(REALM_TRUMP, 0, 0, 25) ||
			borg_mindcr_fail(MIND_MINOR_DISP, 3, 35) ||
			borg_activate_artifact(ART_BELEGENNON, FALSE) ||
			borg_activate_artifact(ART_COLANNON, FALSE) ||
			borg_zap_rod(SV_ROD_HEALING))
		{
			borg_note("# Buying time waiting for Recall.  Step 2.");
			return (TRUE);
		}

		if ((borg_skill[BI_MAXHP] - borg_skill[BI_CURHP] < 100) &&
			(borg_quaff_crit(TRUE) ||
			 borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			 borg_quaff_potion(SV_POTION_CURE_LIGHT)))
		{
			borg_note("# Buying time waiting for Recall.  Step 3.");
			return (TRUE);
		}
		if ((borg_skill[BI_MAXHP] - borg_skill[BI_CURHP] > 150) &&
			(borg_quaff_potion(SV_POTION_HEALING) ||
			 borg_quaff_potion(SV_POTION_STAR_HEALING) ||
			 borg_quaff_potion(SV_POTION_LIFE) ||
			 borg_quaff_crit(TRUE) ||
			 borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			 borg_quaff_potion(SV_POTION_CURE_LIGHT)))
		{
			borg_note("# Buying time waiting for Recall.  Step 4.");
			return (TRUE);
		}

	}



	/* if I am gonna die next round, and I have no way to escape
	 * use the unknown stuff (if I am low level).
	 */
	if (p > (borg_skill[BI_CURHP] * 4) && borg_skill[BI_CLEVEL] < 20 &&
		!borg_skill[BI_MAXSP])
	{
		if (borg_use_unknown()) return (TRUE);
		if (borg_quaff_unknown()) return (TRUE);
		if (borg_read_unknown()) return (TRUE);
		if (borg_eat_unknown()) return (TRUE);

	}



	/* Nothing */
	return (FALSE);
}


/*
 * New method for handling attacks, missiles, and spells
 *
 * Every turn, we evaluate every known method of causing damage
 * to monsters, and evaluate the "reward" inherent in each of
 * the known methods which is usable at that time, and then
 * we actually use whichever method, if any, scores highest.
 *
 * For each attack, we need a function which will determine the best
 * possible result of using that attack, and return its value.  Also,
 * if requested, the function should actually perform the action.
 *
 * Note that the functions should return zero if the action is not
 * usable, or if the action is not useful.
 *
 * These functions need to apply some form of "cost" evaluation, to
 * prevent the use of expensive spells with minimal reward.  Also,
 * we should always prefer attacking by hand to using spells if the
 * damage difference is "small", since there is no "cost" in making
 * a physical attack.
 *
 * We should take account of "spell failure", as well as "missile
 * missing" and "blow missing" probabilities.
 *
 * Note that the functions may store local state information when
 * doing a "simulation" and then they can use this information if
 * they are asked to implement their strategy.
 *
 * There are several types of damage inducers:
 *
 *   Attacking physically
 *   Launching missiles
 *   Throwing objects
 *   Casting spells
 *   Praying prayers
 *   Using wands
 *   Using rods
 *   Using staffs
 *   Using scrolls
 *   Activating Artifacts
 *   Activate Dragon Armour
 */
enum
{
	BF_LAUNCH_NORMAL,
	BF_LAUNCH_SEEKER,
	BF_LAUNCH_FLAME,
	BF_LAUNCH_FROST,
	BF_LAUNCH_ANIMAL,
	BF_LAUNCH_EVIL,
	BF_LAUNCH_DRAGON,
	BF_LAUNCH_WOUNDING,
	BF_OBJECT,
	BF_THRUST,

	BF_LIFE_CALL_LIGHT,
	BF_LIFE_HOLY_ORB,
	BF_LIFE_EXORCISM,
	BF_LIFE_DISP_UNDEAD,
	BF_LIFE_DISP_EVIL,
	BF_LIFE_HOLY_WORD,
	BF_LIFE_DIVINE_INT,

	BF_ARCANE_ZAP,
	BF_ARCANE_ZAP_RESERVE,
	BF_ARCANE_LAREA,
	BF_ARCANE_STONEMUD,
	BF_ARCANE_LBEAM,
	BF_ARCANE_ELEM_BALL,

	BF_SORC_LAREA,
	BF_SORC_CONF_MON,
	BF_SORC_SLEEP_I,
	BF_SORC_SLOW_MON,
	BF_SORC_SLEEP_III,
	BF_SORC_STASIS,

	BF_NATURE_DAYL,
	BF_NATURE_STONEMUD,
	BF_NATURE_ELECBOLT,
	BF_NATURE_FROSTBOLT,
	BF_NATURE_SUNL,
	BF_NATURE_ENTANGLE,
	BF_NATURE_WHIRLWIND,
	BF_NATURE_BLIZZARD,
	BF_NATURE_ELECSTORM,
	BF_NATURE_WHIRLPOOL,
	BF_NATURE_SUNLIGHT,
	BF_NATURE_NATWRATH,

	BF_TRUMP_MINDBLAST,
	BF_TRUMP_MINDBLAST_RESERVE,

	BF_CHAOS_MMISSILE,
	BF_CHAOS_MMISSILE_RESERVE,
	BF_CHAOS_FLASHLIGHT,
	BF_CHAOS_MANABURST,
	BF_CHAOS_FIREBOLT,
	BF_CHAOS_FISTFORCE,
	BF_CHAOS_CHAOSBOLT,
	BF_CHAOS_SONICBOOM,
	BF_CHAOS_DOOMBOLT,
	BF_CHAOS_FIREBALL,
	BF_CHAOS_INVOKELOGRUS,
	BF_CHAOS_POLYMORPH,
	BF_CHAOS_CHAINLIGHT,
	BF_CHAOS_DISINTEG,
	BF_CHAOS_GRAVITY,
	BF_CHAOS_METEORSWARM,
	BF_CHAOS_FLAMESTRIKE,
	BF_CHAOS_ROCKET,
	BF_CHAOS_MANASTORM,
	BF_CHAOS_BRLOGRUS,
	BF_CHAOS_CALLVOID,

	BF_DEATH_MALEDICTION,
	BF_DEATH_MALEDICTION_RESERVE,
	BF_DEATH_STINKCLOUD,
	BF_DEATH_SLEEP_I,
	BF_DEATH_HORRIFY,
	BF_DEATH_ENTROPY,
	BF_DEATH_NETHERBOLT,
	BF_DEATH_TERROR,
	BF_DEATH_VAMPDRAIN,
	BF_DEATH_DISPELGOOD,
	BF_DEATH_DARKBOLT,
	BF_DEATH_VAMPIRISM,
	BF_DEATH_DARKNESS,
	BF_DEATH_DEATHRAY,
	BF_DEATH_WORDOFDEATH,
	BF_DEATH_EVOCATION,
	BF_DEATH_HELLFIRE,

	BF_MIND_NEURAL,
	BF_MIND_PULVERISE,
	BF_MIND_MINDWAVE,
	BF_MIND_PSYCH_DRAIN,
	BF_MIND_TELE_WAVE,

	BF_ROD_ELEC_BOLT,
	BF_ROD_COLD_BOLT,
	BF_ROD_ACID_BOLT,
	BF_ROD_FIRE_BOLT,
	BF_ROD_LITE_BEAM,
	BF_ROD_DRAIN_LIFE,
	BF_ROD_ELEC_BALL,
	BF_ROD_COLD_BALL,
	BF_ROD_ACID_BALL,
	BF_ROD_FIRE_BALL,
	BF_ROD_SLOW_MONSTER,
	BF_ROD_SLEEP_MONSTER,
	BF_ROD_PESTICIDE,

	BF_STAFF_SLEEP_MONSTERS,
	BF_STAFF_SLOW_MONSTERS,
	BF_STAFF_DISPEL_EVIL,
	BF_STAFF_POWER,
	BF_STAFF_HOLINESS,

	BF_WAND_MAGIC_MISSILE,
/*    BF_WAND_CHARM_MONSTERS, */
	BF_WAND_COLD_BOLT,
	BF_WAND_ACID_BOLT,
	BF_WAND_FIRE_BOLT,
	BF_WAND_SLOW_MONSTER,
	BF_WAND_SLEEP_MONSTER,
	BF_WAND_CONFUSE_MONSTER,
	BF_WAND_FEAR_MONSTER,
	BF_WAND_ANNIHILATION,
	BF_WAND_DRAIN_LIFE,
	BF_WAND_LITE_BEAM,
	BF_WAND_STINKING_CLOUD,
	BF_WAND_ELEC_BALL,
	BF_WAND_COLD_BALL,
	BF_WAND_ACID_BALL,
	BF_WAND_FIRE_BALL,
	BF_WAND_WONDER,
	BF_WAND_DRAGON_COLD,
	BF_WAND_DRAGON_FIRE,
	BF_WAND_ROCKETS,

	BF_ART_NARTHANC,
	BF_ART_NIMTHANC,
	BF_ART_DETHANC,
	BF_ART_RILIA,
	BF_ART_BELANGIL,
	BF_ART_ARUNRUTH,
	BF_ART_RINGIL,
	BF_ART_ANDURIL,
	BF_ART_THEODEN,
	BF_ART_AEGLOS,
	BF_ART_TOTILA,
	BF_ART_FIRESTAR,
	BF_ART_TURMIL,
	BF_ART_RAZORBACK,
	BF_ART_CAMMITHRIM,
	BF_ART_PAURHACH,
	BF_ART_PAURNIMMEN,
	BF_ART_HOLCOLLETH,
	BF_ART_PAURAEGEN,
	BF_ART_PAURNEN,
	BF_ART_FINGOLFIN,
	BF_ART_INGWE,
	BF_ART_NARYA,
	BF_ART_NENYA,
	BF_ART_VILYA,

	BF_DRAGON_BLUE,
	BF_DRAGON_WHITE,
	BF_DRAGON_BLACK,
	BF_DRAGON_GREEN,
	BF_DRAGON_RED,
	BF_DRAGON_MULTIHUED,
	BF_DRAGON_BRONZE,
	BF_DRAGON_GOLD,
	BF_DRAGON_CHAOS,
	BF_DRAGON_LAW,
	BF_DRAGON_BALANCE,
	BF_DRAGON_SHINING,
	BF_DRAGON_POWER,

	BF_RACIAL_VAMP,
	BF_RACIAL_CYCLOPS,
	BF_RACIAL_DARK_ELF,
	BF_RACIAL_DRACONIAN,
	BF_RACIAL_IMP,
	BF_RACIAL_KLACKON,
	BF_RACIAL_KOBOLD,
	BF_RACIAL_MINDFLAYER,
	BF_RACIAL_SPRITE,
	BF_RACIAL_YEEK,

	BF_MAX
};



/*
 * Guess how much damage a physical attack will do to a monster
 */
static int borg_thrust_damage_one(int i)
{
	int dam;
	int mult;

	borg_kill *kill;

	monster_race *r_ptr;

	borg_item *item;

	int chance;

	/* Examine current weapon */
	item = &borg_items[INVEN_WIELD];

	/* Monster record */
	kill = &borg_kills[i];

	/* Monster race */
	r_ptr = &r_info[kill->r_idx];

	/* Damage */
	dam = (item->dd * (item->ds + 1) / 2);

	/* here is the place for slays and such */
	mult = 1;

	if (((borg_skill[BI_WS_ANIMAL]) && (r_ptr->flags3 & RF3_ANIMAL)) ||
		((borg_skill[BI_WS_EVIL]) && (r_ptr->flags3 & RF3_EVIL)))
		mult = 2;
	if (((borg_skill[BI_WS_UNDEAD]) && (r_ptr->flags3 & RF3_ANIMAL)) ||
		((borg_skill[BI_WS_DEMON]) && (r_ptr->flags3 & RF3_DEMON)) ||
		((borg_skill[BI_WS_ORC]) && (r_ptr->flags3 & RF3_ORC)) ||
		((borg_skill[BI_WS_TROLL]) && (r_ptr->flags3 & RF3_TROLL)) ||
		((borg_skill[BI_WS_GIANT]) && (r_ptr->flags3 & RF3_GIANT)) ||
		((borg_skill[BI_WS_DRAGON]) && (r_ptr->flags3 & RF3_DRAGON)) ||
		((borg_skill[BI_WB_ACID]) && !(r_ptr->flags3 & RF3_IM_ACID)) ||
		((borg_skill[BI_WB_FIRE]) && !(r_ptr->flags3 & RF3_IM_FIRE)) ||
		((borg_skill[BI_WB_COLD]) && !(r_ptr->flags3 & RF3_IM_COLD)) ||
		((borg_skill[BI_WB_ELEC]) && !(r_ptr->flags3 & RF3_IM_ELEC)))
		mult = 3;
	if ((borg_skill[BI_WK_DRAGON]) && (r_ptr->flags3 & RF3_DRAGON))
		mult = 5;

	/* add the multiplier */
	dam *= mult;

	/* add weapon bonuses */
	dam += item->to_d;

	/* add player bonuses */
	dam += borg_skill[BI_TODAM];

	/* multiply the damage for the whole round of attacks */
	dam *= borg_skill[BI_BLOWS];

	/* reduce for % chance to hit (AC) */
	chance = (borg_skill[BI_THN] + ((borg_skill[BI_TOHIT] + item->to_h) * 3));
	if ((r_ptr->ac * 3 / 4) > 0)
		chance = (chance * 100) / (r_ptr->ac * 3 / 4);

	/* 5% automatic success/fail */
	if (chance > 95) chance = 95;
	if (chance < 5) chance = 5;

	/* add 20% to chance to give a bit more wieght to weapons */
	if (borg_skill[BI_CLEVEL] > 15 && borg_class != CLASS_MAGE) chance += 20;

	dam = (dam * chance) / 100;

	/* Limit damage to twice maximal hitpoints */
	if (dam > kill->power * 2) dam = kill->power * 2;

	/* Reduce the damage if a mage, they should not melee if they can avoid it */
	if (borg_class == CLASS_MAGE &&
		borg_skill[BI_MAXCLEVEL] < 40) dam = dam * 6 / 10;

	/*
	 * Enhance the preceived damage on Uniques.  This way we target them
	 * Keep in mind that he should hit the uniques but if he has a
	 * x5 great bane of dragons, he will tend attack the dragon since the
	 * precieved (and actual) damage is higher.  But don't select
	 * the town uniques (maggot does no damage)
	 *
	 */
	if ((r_ptr->flags1 & RF1_UNIQUE) &&
		borg_skill[BI_CDEPTH] >= 1) dam += (dam * 5);

	/* Hack -- ignore Maggot until later.  Player will chase Maggot
	 * down all accross the screen waking up all the monsters.  Then
	 * he is stuck in a comprimised situation.
	 */
	if ((r_ptr->flags1 & RF1_UNIQUE) && borg_skill[BI_CDEPTH] == 0)
	{
		dam = dam * 2 / 3;

		/* Dont hunt maggot until later */
		if (borg_skill[BI_CLEVEL] < 5) dam = 0;
	}

	/* give a small bonus for whacking a breeder */
	if (r_ptr->flags2 & RF2_MULTIPLY)
		dam = (dam * 3 / 2);

	/* Enhance the preceived damgage to summoner in order to influence the
	 * choice of targets.
	 */
	if ((r_ptr->flags6 & RF6_S_KIN) ||
		(r_ptr->flags6 & RF6_S_CYBER) ||
		(r_ptr->flags6 & RF6_S_MONSTER) ||
		(r_ptr->flags6 & RF6_S_MONSTERS) ||
		(r_ptr->flags6 & RF6_S_ANT) ||
		(r_ptr->flags6 & RF6_S_SPIDER) ||
		(r_ptr->flags6 & RF6_S_HOUND) ||
		(r_ptr->flags6 & RF6_S_HYDRA) ||
		(r_ptr->flags6 & RF6_S_ANGEL) ||
		(r_ptr->flags6 & RF6_S_DEMON) ||
		(r_ptr->flags6 & RF6_S_UNDEAD) ||
		(r_ptr->flags6 & RF6_S_DRAGON) ||
		(r_ptr->flags6 & RF6_S_HI_UNDEAD) ||
		(r_ptr->flags6 & RF6_S_HI_DRAGON) ||
		(r_ptr->flags6 & RF6_S_AMBERITES) ||
		(r_ptr->flags6 & RF6_S_UNIQUE) || (r_ptr->flags1 & RF1_QUESTOR))
		dam += ((dam * 3) / 2);

	/* To conserve mana, for keeping GOI up, increase the value of melee */
	if (borg_goi)
	{
		dam += (dam * 15 / 10);
	}

	/* dont hurt friends or pets */
	if (kill->friendly) dam = -10;

	/* Invuln monsters take no dam */
	if (kill->invulner) dam = 0;

	/* Damage */
	return (dam);
}



/*
 * Simulate/Apply the optimal result of making a physical attack
 */
extern int borg_attack_aux_thrust(void)
{
	int p, dir;

	int i, b_i = -1;
	int d, b_d = -1;

	map_block *mb_ptr;

	borg_kill *kill;

	/* Too afraid to attack */
	if (borg_skill[BI_ISAFRAID]) return (0);


	/* Examine possible destinations */
	for (i = 0; i < borg_temp_n; i++)
	{
		int x = borg_temp_x[i];
		int y = borg_temp_y[i];

		/* Require "adjacent" */
		if (distance(c_y, c_x, y, x) > 1) continue;

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Acquire grid */
		mb_ptr = map_loc(x, y);

		/* Calculate "average" damage */
		d = borg_thrust_damage_one(mb_ptr->kill);

		/* No damage */
		if (d <= 0) continue;

		/* Obtain the monster */
		kill = &borg_kills[mb_ptr->kill];

		/* Hack -- avoid waking most "hard" sleeping monsters */
		if (!kill->awake && (d <= kill->power))
		{
			/* Calculate danger */
			borg_full_damage = TRUE;
			p = borg_danger_aux(y, x, 1, mb_ptr->kill, TRUE);
			borg_full_damage = FALSE;

			if (p > avoidance / 2)
				continue;
		}

		/* Hack -- ignore sleeping town monsters */
		if (!borg_skill[BI_CDEPTH] && !kill->awake) continue;


		/* Calculate "danger" to player */
		borg_full_damage = TRUE;
		p = borg_danger_aux(c_y, c_x, 2, mb_ptr->kill, TRUE);
		borg_full_damage = FALSE;

		/* Reduce "bonus" of partial kills */
		if (d <= kill->power) p = p / 10;

		/* Add the danger to the damage */
		d += p;

		/* Ignore lower damage */
		if ((b_i >= 0) && (d < b_d)) continue;

		/* Save the info */
		b_i = i;
		b_d = d;
	}

	/* Nothing to attack */
	if (b_i < 0) return (0);

	/* Simulation */
	if (borg_simulate) return (b_d);

	/* Save the location */
	g_x = borg_temp_x[b_i];
	g_y = borg_temp_y[b_i];

	mb_ptr = map_loc(g_x, g_y);
	kill = &borg_kills[mb_ptr->kill];

	/* Note */
	borg_note(format
			  ("# Facing %s at (%d,%d) who has %d Hit Points.",
			   (r_name + r_info[kill->r_idx].name), g_y, g_x, kill->power));
	borg_note(format
			  ("# Attacking with weapon '%s'", borg_items[INVEN_WIELD].desc));

	/* Get a direction for attacking */
	dir = borg_extract_dir(c_y, c_x, g_y, g_x);

	/* Attack the grid */
	borg_keypress('+');
	borg_keypress(I2D(dir));

	/* Success */
	return (b_d);
}




/*
 * Target a location.  Can be used alone or at "Direction?" prompt.
 *
 * Warning -- This will only work for locations on the current panel
 */
bool borg_target(int y, int x)
{
	int x1, y1, x2, y2;

	map_block *mb_ptr;

	borg_kill *kill;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (FALSE);

	mb_ptr = map_loc(x, y);

	kill = &borg_kills[mb_ptr->kill];


	/* Log */
	/* Report a little bit */
	if (mb_ptr->kill)
	{
		borg_note(format
				  ("# Targeting %s who has %d Hit Points.",
				   (r_name + r_info[kill->r_idx].name), kill->power));
	}
	else
	{
		borg_note(format("# Targetting location (%d,%d)", y, x));
	}

	/* Target mode */
	borg_keypress('*');

	/* Target a location */
	borg_keypress('p');

	/* Determine "path" */
	x1 = c_x;
	y1 = c_y;
	x2 = x;
	y2 = y;

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
	borg_keypress('5');

	/* Success */
	return (TRUE);
}

/*
 * Mark spot along the target path a wall.
 * This will mark the unknown squares as a wall.  This might not be
 * the wall we ran into but also might be.
 *
 * Warning -- This will only work for locations on the current panel
 */
bool borg_target_unknown_wall(int y, int x)
{
	int n_x, n_y;
	bool found = FALSE;
	bool y_hall = FALSE;
	bool x_hall = FALSE;

	borg_note(format("# Perhaps wall near targetted location (%d,%d)", y, x));

	/* Determine "path" */
	n_x = c_x;
	n_y = c_y;

	/* check for 'in a hall' x axis */
	/* This check is for this: */
	/*
	 *      x
	 *    ..@..
	 *      x
	 *
	 * 'x' being 'not a floor' and '.' being a floor.
	 */

	if (borg_cave_floor_bold(c_y + 1, c_x) &&
		borg_cave_floor_bold(c_y + 2, c_x) &&
		borg_cave_floor_bold(c_y - 1, c_x) &&
		borg_cave_floor_bold(c_y - 2, c_x) &&
		!borg_cave_floor_bold(c_y, c_x + 1) &&
		!borg_cave_floor_bold(c_y, c_x - 1))
		x_hall = TRUE;

	/* check for 'in a hall' y axis */
	if (borg_cave_floor_bold(c_y, c_x + 1) &&
		borg_cave_floor_bold(c_y, c_x + 2) &&
		borg_cave_floor_bold(c_y, c_x - 1) &&
		borg_cave_floor_bold(c_y, c_x - 2) &&
		!borg_cave_floor_bold(c_y + 1, c_x) &&
		!borg_cave_floor_bold(c_y - 1, c_x))
		y_hall = TRUE;

	while (n_x != x && n_y != y)
	{
		map_block *mb_ptr;

		/* Bounds checking */
		if (map_in_bounds(n_x, n_y))
		{
			mb_ptr = map_loc(n_x, n_y);

			if (!mb_ptr->feat &&
				((n_y != c_y) || !y_hall) && ((n_x != c_x) || !x_hall))
			{
				borg_note(format
						  ("# Guessing wall (%d,%d) near target (%d,%d)",
						   n_y, n_x, y, x));
				mb_ptr->feat = FEAT_WALL_SOLID;
				found = TRUE;
			}
		}

		/* Calculate the new location */
		borgmove2(&n_y, &n_x, c_y, c_x, y, x);
	}

	return found;
}


/*
 * Guess how much damage a spell attack will do to a monster
 *
 * We only handle the "standard" damage types.
 *
 * We are paranoid about monster resistances
 *
 * He tends to waste all of his arrows on a monsters immediately adjacent
 * to him.  Then he has no arrows for the rest of the level.  We will
 * decrease the damage if the monster is adjacent and we are getting low
 * on missiles.
 *
 * We will also decrease the value of the missile attack on breeders or
 * high clevel borgs town scumming.
 */
int borg_launch_damage_one(int i, int dam, int typ)
{
	int p1, p2 = 0;
	bool borg_use_missile = FALSE;

	borg_kill *kill;

	monster_race *r_ptr;

	/* Monster record */
	kill = &borg_kills[i];

	/* all danger checks are with maximal damage */
	borg_full_damage = TRUE;

	/* Monster race */
	r_ptr = &r_info[kill->r_idx];

	/* Analyze the damage type */
	switch (typ)
	{
		case GF_MISSILE:
		{
			/* Magic Missile */
			break;
		}

		case GF_ARROW:
		{
			/* Standard Arrow */
			if (distance(c_y, c_x, kill->y, kill->x) == 1 &&
				!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 5;
			break;
		}

		case GF_ARROW_SEEKER:
		{
			/* Seeker Arrow/Bolt */
			if (!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 10;
			if (distance(c_y, c_x, kill->y, kill->x) == 1 &&
				!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 5;
			break;
		}

		case GF_ARROW_FLAME:
		{
			/* Arrow of Flame */
			if (!(r_ptr->flags3 & RF3_IM_FIRE)) dam *= 3;
			if (distance(c_y, c_x, kill->y, kill->x) == 1 &&
				!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 5;
			break;
		}

		case GF_ARROW_FROST:
		{
			/* Arrow of Frost */
			if (!(r_ptr->flags3 & RF3_IM_COLD)) dam *= 3;
			if (distance(c_y, c_x, kill->y, kill->x) == 1 &&
				!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 5;
			break;
		}

		case GF_ARROW_ANIMAL:
		{
			/* Arrow of Hurt Animal */
			if (r_ptr->flags3 & RF3_ANIMAL) dam *= 2;
			if (distance(c_y, c_x, kill->y, kill->x) == 1 &&
				!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 5;
			break;
		}

		case GF_ARROW_EVIL:
		{
			/* Arrow of hurt evil */
			if (r_ptr->flags3 & RF3_EVIL) dam *= 2;
			if (distance(c_y, c_x, kill->y, kill->x) == 1 &&
				!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 5;
			break;
		}

		case GF_ARROW_DRAGON:
		{
			/* Arrow of slay dragon */
			if (r_ptr->flags3 & RF3_DRAGON) dam *= 3;
			if (distance(c_y, c_x, kill->y, kill->x) == 1 &&
				!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 5;
			break;
		}

		case GF_ARROW_WOUNDING:
		{
			/* Arrow of Wounding */
			if (distance(c_y, c_x, kill->y, kill->x) == 1 &&
				!(r_ptr->flags1 & RF1_UNIQUE)) dam /= 5;
			break;
		}

		case GF_MANA:
		{
			/* Pure damage */

			/* only use mana storm against uniques... this */
			/* should cut down on some mana use. */
			if (!borg_fighting_unique || borg_has[266] < 3)
				dam /= 2;
			if (borg_fighting_unique && borg_has[266] > 7)
				dam *= 2;
			break;
		}


		case GF_ACID:
		{
			/* Acid */
			if (r_ptr->flags3 & RF3_IM_ACID) dam /= 9;
			break;
		}

		case GF_ELEC:
		{
			/* Electricity */
			if (r_ptr->flags3 & RF3_IM_ELEC) dam /= 9;
			break;
		}

		case GF_FIRE:
		{
			/* Fire damage */
			if (r_ptr->flags3 & RF3_IM_FIRE) dam /= 9;
			break;
		}

		case GF_COLD:
		{
			/* Cold */
			if (r_ptr->flags3 & RF3_IM_COLD) dam /= 9;
			break;
		}

		case GF_ELEMENTS:
		{
			/* Hack -- Equal chance of all elements to be cast */
			if (r_ptr->flags3 & RF3_IM_COLD) dam /= 4;
			if (r_ptr->flags3 & RF3_IM_ELEC) dam /= 4;
			if (r_ptr->flags3 & RF3_IM_FIRE) dam /= 4;
			if (r_ptr->flags3 & RF3_IM_ACID) dam /= 4;
			break;
		}

		case GF_POIS:
		{
			/* Poison */
			if (r_ptr->flags3 & RF3_IM_POIS) dam /= 9;
			break;
		}

		case GF_NUKE:
		{
			/* Nuke */
			if (r_ptr->flags3 & RF3_IM_POIS) dam = (dam * 3) / 9;
			break;
		}

		case GF_ICE:
		{
			/* Ice */
			if (r_ptr->flags3 & RF3_IM_COLD) dam /= 9;
			break;
		}

		case GF_HELL_FIRE:
		{
			/* Holy Orb */
			if (r_ptr->flags3 & RF3_EVIL) dam *= 2;
			break;
		}

		case GF_HOLY_FIRE:
		{
			/* Holy Orb */
			if (r_ptr->flags3 & RF3_GOOD) dam = 0;
			else if (r_ptr->flags3 & RF3_EVIL) dam *= 2;
			else
				dam = (dam * 3) / 9;
			break;
		}

		case GF_DISP_UNDEAD:
		{
			/* dispel undead */
			if (!(r_ptr->flags3 & RF3_UNDEAD)) dam = 0;
			break;
		}

		case GF_DISP_DEMON:
		{
			/* Dispel Demon */
			if (!(r_ptr->flags3 & RF3_DEMON)) dam = 0;
			break;
		}

		case GF_DISP_UNDEAD_DEMON:
		{
			/* Dispel Demons and Undead (Exorcism Spell) */
			if (!(r_ptr->flags3 & RF3_UNDEAD)) dam = 0;
			if (!(r_ptr->flags3 & RF3_DEMON)) dam = 0;
			break;
		}

		case GF_DISP_EVIL:
		{
			/*  Dispel Evil */
			if (!(r_ptr->flags3 & RF3_EVIL)) dam = 0;
			break;
		}

		case GF_HOLY_WORD:
		{
			/*  Holy Word */
			if (!(r_ptr->flags3 & RF3_EVIL)) dam = 0;
			break;
		}

		case GF_LITE_WEAK:
		{
			/* Weak Lite */
			if (!(r_ptr->flags3 & RF3_HURT_LITE)) dam = 0;
			if (borg_skill[BI_FEAR_LITE]) dam = 0;
			break;
		}

		case GF_LITE:
		{
			/* Regular Lite */
			if (borg_skill[BI_FEAR_LITE]) dam = 0;
			break;
		}

		case GF_OLD_DRAIN:
		case GF_DEATH_RAY:
		{
			/* Drain Life / Psi / Vamp. */
			if (!monster_living(r_ptr))
			{
				dam = 0;
			}
			break;
		}

		case GF_PSI:
		case GF_PSI_DRAIN:
		{
			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				dam = 0;
			}
			else if ((r_ptr->flags2 & RF2_STUPID) ||
					 (r_ptr->flags2 & RF2_WEIRD_MIND) ||
					 (r_ptr->flags3 & RF3_ANIMAL) ||
					 (r_ptr->level > (3 * dam / 2)))
			{
				dam /= 3;
			}
			else if (((r_ptr->flags3 & RF3_UNDEAD) ||
					  (r_ptr->flags3 & RF3_DEMON)) &&
					 (r_ptr->level > borg_skill[BI_CLEVEL] / 2))
			{
				dam = 0;
			}
			break;
		}

		case GF_KILL_WALL:
		{
			/* Stone to Mud */
			if (!(r_ptr->flags3 & RF3_HURT_ROCK)) dam = 0;
			break;
		}

		case GF_NETHER:
		{
			/* Nether */

			if (r_ptr->flags3 & RF3_UNDEAD)
			{
				dam = 0;
			}
			else if (r_ptr->flags4 & RF4_BR_NETH)
			{
				dam *= 3;
				dam /= 9;
			}
			else if (r_ptr->flags3 & RF3_EVIL)
			{
				dam /= 2;
			}
			break;
		}

		case GF_CHAOS:
		{
			/* Chaos */

			if ((r_ptr->flags4 & RF4_BR_CHAO) || (r_ptr->flags3 & RF3_DEMON))
			{
				dam *= 3;
				dam /= 9;
			}
			break;
		}

		case GF_GRAVITY:
		{
			/* Gravity */

			if (r_ptr->flags4 & RF4_BR_GRAV)
			{
				dam *= 2;
				dam /= 9;
			}
			break;
		}

		case GF_SHARDS:
		{
			/* Shards */
			if (r_ptr->flags4 & RF4_BR_SHAR)
			{
				dam *= 3;
				dam /= 9;
			}
			break;
		}

		case GF_ROCKET:
		{
			/* Rockets */
			if (r_ptr->flags4 & RF4_BR_SHAR)
			{
				dam /= 2;
			}
			break;
		}

		case GF_SOUND:
		{
			/* Sound */
			if (r_ptr->flags4 & RF4_BR_SOUN)
			{
				dam *= 2;
				dam /= 9;
			}
			break;
		}

		case GF_PLASMA:
		{
			/* Plasma */
			if ((r_ptr->flags4 & RF4_BR_PLAS) || (r_ptr->flags3 & RF3_RES_PLAS))
			{
				dam *= 2;
				dam /= 9;
			}
			break;
		}

		case GF_FORCE:
		{
			/* Force */
			if (r_ptr->flags4 & RF4_BR_WALL)
			{
				dam *= 2;
				dam /= 9;
			}
			break;
		}

		case GF_DARK:
		{
			/* Dark */
			if (r_ptr->flags4 & RF4_BR_DARK)
			{
				dam *= 2;
				dam /= 9;
			}
			break;
		}

		case GF_WATER:
		{
			/* Water */
			if (r_ptr->flags3 & RF3_RES_WATE)
			{
				dam *= 2;
				dam /= 9;
			}
			break;
		}

		case GF_DISINTEGRATE:
		{
			/* Disintegrate */
			if (r_ptr->flags3 & RF3_RES_DISE)
			{
				dam *= 2;
				dam /= 9;
			}
			break;
		}
		case GF_TELEKINESIS:
		{
			if (r_ptr->flags1 & RF1_UNIQUE) dam /= 3;
			break;
		}

		case GF_METEOR:
		{
			/* Meteor */
			break;
		}

		case GF_DISP_GOOD:
		{
			/* Dispel Good */
			if (!(r_ptr->flags3 & RF3_GOOD)) dam = 0;
			break;
		}

		case GF_DISP_LIVING:
		{
			/* Dispel Living */
			if (!monster_living(r_ptr)) dam = 0;
			break;
		}

		case GF_CONFUSION:
		case GF_DISENCHANT:
		case GF_NEXUS:
		case GF_INERTIA:
		case GF_TIME:
		{
			/* Weird attacks */
			dam /= 2;
			break;
		}

		case GF_DOMINATION:
		case GF_CHARM:
		case GF_CONTROL_UNDEAD:
		case GF_CONTROL_ANIMAL:
		{
			/* Really weird attacks */
			dam = 0;
			break;
		}

		case GF_OLD_HEAL:
		case GF_OLD_CLONE:
		case GF_OLD_SPEED:
		case GF_DARK_WEAK:
		case GF_KILL_DOOR:
		case GF_KILL_TRAP:
		case GF_MAKE_WALL:
		case GF_MAKE_DOOR:
		case GF_MAKE_TRAP:
		case GF_AWAY_UNDEAD:
		case GF_TURN_EVIL:
		{
			/* Various */
			dam = 0;
			break;
		}

		case GF_AWAY_ALL:
		{
			/* These spells which put the monster out of commission, we
			 * look at the danger of the monster prior to and after being
			 * put out of commission.  The difference is the damage.
			 * The following factors are considered when we
			 * consider the spell:
			 *
			 * 1. Is it already comprised by that spell?
			 * 2. Is it comprimised by another spell?
			 * 3. Does it resist the modality?
			 * 4. Will it make it's savings throw better than half the time?
			 * 5. We generally ignore these spells for breeders.
			 *
			 * The spell sleep II and sanctuary have a special consideration
			 * since the monsters must be adjacent to the player.
			 */


			dam = borg_danger_aux(c_y, c_x, 1, i, TRUE);

			/* try not to teleport away uniques.   These are the guys you are trying */
			/* to kill! */
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				/* If this unique is causing the danger, get rid of it */
				if (dam > avoidance * 3 && borg_skill[BI_CDEPTH] <= 95)
				{
					/* get rid of this unique */
				}
				else
					dam = -999;
			}
			break;
		}

		case GF_DISP_ALL:
		{
			/* In Z this does hurt Uniques but not in V */
			break;
		}

		case GF_OLD_CONF:
		{
			dam = 0;
			if (r_ptr->flags3 & RF3_NO_CONF) break;
			if (r_ptr->flags2 & RF2_MULTIPLY) break;
			if (kill->speed < r_ptr->speed) break;
			if (kill->afraid) break;
			if (kill->confused) break;
			if (!kill->awake) break;
			if ((r_ptr->level >=
				 (borg_skill[BI_CLEVEL] <
				  13) ? borg_skill[BI_CLEVEL] : (((borg_skill[BI_CLEVEL] - 10) /
												  4) * 3) + 10)) break;
			dam = -999;
			if (r_ptr->flags1 & RF1_UNIQUE) break;
			borg_confuse_spell = FALSE;
			p1 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
			borg_confuse_spell = TRUE;
			p2 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
			borg_confuse_spell = FALSE;
			dam = (p1 - p2);
			break;
		}

		case GF_TURN_ALL:
		{
			dam = 0;
			if (kill->speed < r_ptr->speed) break;
			if (r_ptr->flags3 & RF3_NO_FEAR) break;
			if (kill->afraid) break;
			if (kill->confused) break;
			if (!kill->awake) break;
			if ((r_ptr->level >=
				 (borg_skill[BI_CLEVEL] <
				  13) ? borg_skill[BI_CLEVEL] : (((borg_skill[BI_CLEVEL] - 10) /
												  4) * 3) + 10)) break;
			dam = -999;
			if (r_ptr->flags1 & RF1_UNIQUE) break;
			borg_fear_mon_spell = FALSE;
			p1 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
			borg_fear_mon_spell = TRUE;
			p2 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
			borg_fear_mon_spell = FALSE;
			dam = (p1 - p2);
			break;
		}

		case GF_OLD_SLOW:
		{
			dam = 0;
			if (kill->speed < r_ptr->speed) break;
			if (kill->afraid) break;
			if (kill->confused) break;
			if (!kill->awake) break;
			if ((r_ptr->level >=
				 (borg_skill[BI_CLEVEL] <
				  13) ? borg_skill[BI_CLEVEL] : (((borg_skill[BI_CLEVEL] - 10) /
												  4) * 3) + 10)) break;
			dam = -999;
			if (r_ptr->flags1 & RF1_UNIQUE) break;
			borg_slow_spell = FALSE;
			p1 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
			borg_slow_spell = TRUE;
			p2 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
			borg_slow_spell = FALSE;
			dam = (p1 - p2);
			break;
		}

		case GF_OLD_SLEEP:
		case GF_STASIS:
		{
			dam = 0;
			if (r_ptr->flags3 & RF3_NO_SLEEP) break;
			if (kill->speed < r_ptr->speed) break;
			if (kill->afraid) break;
			if (kill->confused) break;
			if (!kill->awake) break;
			if ((r_ptr->level >=
				 (borg_skill[BI_CLEVEL] <
				  13) ? borg_skill[BI_CLEVEL] : (((borg_skill[BI_CLEVEL] - 10) /
												  4) * 3) + 10)) break;
			dam = -999;
			if (r_ptr->flags1 & RF1_UNIQUE) break;
			borg_sleep_spell = FALSE;
			p1 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
			borg_sleep_spell = TRUE;
			p2 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
			borg_sleep_spell = FALSE;
			dam = (p1 - p2);
			break;
		}

		case GF_OLD_POLY:
		{
			dam = 0;
			if ((r_ptr->level >=
				 (borg_skill[BI_CLEVEL] <
				  13) ? borg_skill[BI_CLEVEL] : (((borg_skill[BI_CLEVEL] - 10) /
												  4) * 3) + 10)) break;
			dam = -999;
			if (r_ptr->flags1 & RF1_UNIQUE) break;
			dam = borg_danger_aux(c_y, c_x, 2, i, TRUE);
			/* dont bother unless he is a scary monster */
			if (dam < avoidance * 2) dam = 0;
			break;
		}

		case GF_TURN_UNDEAD:
		{
			if (r_ptr->flags3 & RF3_UNDEAD)
			{
				dam = 0;
				if (kill->confused) break;
				if (kill->afraid) break;
				if (kill->speed < r_ptr->speed) break;
				if (!kill->awake) break;
				if (r_ptr->level > borg_skill[BI_CLEVEL] - 5) break;
				borg_fear_mon_spell = FALSE;
				p1 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
				borg_fear_mon_spell = TRUE;
				p2 = borg_danger_aux(c_y, c_x, 1, i, TRUE);
				borg_fear_mon_spell = FALSE;
				dam = (p1 - p2);
			}
			else
			{
				dam = 0;
			}
			break;
		}

		case GF_AWAY_EVIL:
		{
			/* Banishment-- cast when in extreme danger (checked in borg_defense). */
			if (r_ptr->flags3 & RF3_EVIL)
			{
				/* try not teleport away uniques. */
				if (r_ptr->flags1 & RF1_UNIQUE)
				{
					/* Banish ones with escorts */
					if (r_ptr->flags1 & RF1_ESCORT)
					{
						dam = 0;
					}
					else
					{
						/* try not Banish non escorted uniques */
						dam = -500;
					}

				}
				else
				{
					/* damage is the danger of the baddie */
					dam = borg_danger_aux(c_y, c_x, 1, i, TRUE);
				}
			}
			else
			{
				dam = 0;
			}
			break;
		}

	}

	/* use Missiles on certain types of monsters */
	if ((borg_danger_aux(kill->y, kill->x, 1, i, TRUE) >= avoidance * 3 / 10) ||
		(r_ptr->flags1 & RF1_FRIENDS /* monster has friends */  &&
		 r_ptr->level >= borg_skill[BI_CLEVEL] - 5 /* close levels */ ) ||
		(kill->ranged_attack /* monster has a ranged attack */ ) ||
		(r_ptr->flags1 & RF1_UNIQUE) ||
		(r_ptr->flags2 & RF2_MULTIPLY) ||
		(borg_skill[BI_CLEVEL] <= 5 /* stil very weak */ ))
	{
		borg_use_missile = TRUE;
	}

	/* Restore normal calcs of danger */
	borg_full_damage = FALSE;

	/* dont hurt friends or pets */
	if (kill->friendly) dam = -10;

	/* Invuln monsters take no dam */
	if (kill->invulner) dam = 0;

	/* Return Damage as pure danger of the monster */
	if (typ == GF_AWAY_ALL || typ == GF_AWAY_EVIL) return (dam);

	/* Limit damage to twice maximal hitpoints */
	if (dam > kill->power * 2) dam = kill->power * 2;

	/* give a small bonus for whacking a unique */
	/* this should be just enough to give prefrence to wacking uniques */
	if ((r_ptr->flags1 & RF1_UNIQUE) && borg_skill[BI_CDEPTH] >= 1)
		dam = (dam * 5);

	/* Hack -- ignore Maggot until later.  Player will chase Maggot
	 * down all accross the screen waking up all the monsters.  Then
	 * he is stuck in a comprimised situation.
	 */
	if ((r_ptr->flags1 & RF1_UNIQUE) && borg_skill[BI_CDEPTH] == 0)
	{
		dam = dam * 2 / 3;

		/* Dont hunt maggot until later */
		if (borg_skill[BI_CLEVEL] < 5) dam = 0;
	}

	/* give a small bonus for whacking a breeder */
	if (r_ptr->flags2 & RF2_MULTIPLY)
		dam = (dam * 3 / 2);

	/* Enhance the preceived damage to summoner in order to influence the
	 * choice of targets.
	 */
	if ((r_ptr->flags6 & RF6_S_KIN) ||
		(r_ptr->flags6 & RF6_S_CYBER) ||
		(r_ptr->flags6 & RF6_S_MONSTER) ||
		(r_ptr->flags6 & RF6_S_MONSTERS) ||
		(r_ptr->flags6 & RF6_S_ANT) ||
		(r_ptr->flags6 & RF6_S_SPIDER) ||
		(r_ptr->flags6 & RF6_S_HOUND) ||
		(r_ptr->flags6 & RF6_S_HYDRA) ||
		(r_ptr->flags6 & RF6_S_ANGEL) ||
		(r_ptr->flags6 & RF6_S_DEMON) ||
		(r_ptr->flags6 & RF6_S_UNDEAD) ||
		(r_ptr->flags6 & RF6_S_DRAGON) ||
		(r_ptr->flags6 & RF6_S_HI_UNDEAD) ||
		(r_ptr->flags6 & RF6_S_HI_DRAGON) ||
		(r_ptr->flags6 & RF6_S_AMBERITES) ||
		(r_ptr->flags6 & RF6_S_UNIQUE) || (r_ptr->flags1 & RF1_QUESTOR))
		dam += ((dam * 3) / 2);

	/*  Try to conserve missiles.
	 */
	if (typ < BF_THRUST)
	{
		if (!borg_use_missile)
			/* set damage to zero, force borg to melee attack */
			dam = 0;
	}

	/* Damage */
	return (dam);
}

/*
 * Simulate / Invoke the launching of a bolt at a monster
 */
static int borg_launch_bolt_aux_hack(int i, int dam, int typ)
{
	int d, p, x, y;
	int o_y = 0;
	int o_x = 0;
	int walls = 0;
	int unknown = 0;

	map_block *mb_ptr;

	borg_kill *kill;

	monster_race *r_ptr;

	/* Monster */
	kill = &borg_kills[i];

	/* monster race */
	r_ptr = &r_info[kill->r_idx];


	/* Skip dead monsters */
	if (!kill->r_idx) return (0);

	/* Require current knowledge */
	if (kill->when < borg_t) return (0);

	/* Acquire location */
	x = kill->x;
	y = kill->y;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (0);

	/* Acquire the grid */
	mb_ptr = map_loc(x, y);

	/* Never shoot walls/doors */
	if (borg_cave_wall_grid(mb_ptr)) return (0);

#if 0
	/* Hack -- Unknown grids should be avoided some of the time */
	if (!mb_ptr->feat && ((borg_t % 8) == 0)) return (0);

	/* Hack -- Weird grids should be avoided some of the time */
	if ((mb_ptr->feat == FEAT_INVIS) && ((borg_t % 8) == 0)) return (0);
#endif

	/* dont shoot at ghosts in walls, not perfect */
	if (r_ptr->flags2 & RF2_PASS_WALL)
	{
		/* if 2 walls and 1 unknown skip this monster */
		/* Acquire location */
		x = kill->x;
		y = kill->y;

		/* Get grid */
		for (o_x = -1; o_x <= 1; o_x++)
		{
			for (o_y = -1; o_y <= 1; o_y++)
			{
				/* Acquire location */
				x = kill->x + o_x;
				y = kill->y + o_y;

				/* Bounds checking */
				if (!map_in_bounds(x, y)) continue;

				mb_ptr = map_loc(x, y);

				if (mb_ptr->feat >= FEAT_MAGMA &&
					mb_ptr->feat <= FEAT_PERM_SOLID) walls++;

				if (mb_ptr->feat & FEAT_INVIS) unknown++;
			}
		}
		/* Is the ghost likely in a wall? */
		if (walls >= 2 && unknown >= 1) return (0);
	}



	/* Calculate damage */
	d = borg_launch_damage_one(i, dam, typ);

	/* Calculate danger */
	borg_full_damage = TRUE;
	p = borg_danger_aux(y, x, 1, i, TRUE);
	borg_full_damage = FALSE;

	/* Return Damage as pure danger of the monster */
	if (typ == GF_AWAY_ALL || typ == GF_AWAY_EVIL) return (d);

	/* Return 0 if the true damge (w/o the danger bonus) is 0 */
	if (d <= 0) return (d);

	/* Hack -- avoid waking most "hard" sleeping monsters */
	if (!kill->awake && (p > avoidance / 2) && (d < kill->power))
	{
		return (-999);
	}

	/* Hack -- ignore sleeping town monsters */
	if (!borg_skill[BI_CDEPTH] && !kill->awake)
	{
		return (0);
	}

	/* Calculate "danger" to player */
	borg_full_damage = TRUE;
	p = borg_danger_aux(c_y, c_x, 2, i, TRUE);
	borg_full_damage = FALSE;

	/* Reduce "bonus" of partial kills */
	if (d < kill->power) p = p / 10;

	/* Add in power */
	d += p;

	/* Result */
	return (d);
}


/*
 * Determine the "reward" of launching a beam/bolt/ball at a location
 *
 * An "unreachable" location always has zero reward.
 *
 * Basically, we sum the "rewards" of doing the appropriate amount of
 * damage to each of the "affected" monsters.
 *
 * We will attempt to apply the offset-ball attack here
 */
static int borg_launch_bolt_aux(int y, int x, int rad, int dam, int typ,
                                int max)
{
	int i;

	int x1, y1;
	int x2, y2;

	int dist;

	int r, n;

	map_block *mb_ptr;

	monster_race *r_ptr;

	/* Reset damage */
	n = 0;

	/* Initial location */
	x1 = c_x;
	y1 = c_y;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (0);

	/* Final location */
	x2 = x;
	y2 = y;

	/* Start over */
	x = x1;
	y = y1;

	/* Get the grid */
	mb_ptr = map_loc(x2, y2);

	r_ptr = &r_info[mb_ptr->monster];

	/* Simulate the spell/missile path */
	for (dist = 0; dist < max; dist++)
	{
		mb_ptr = map_loc(x, y);

		/* Bounds checking */
		if (!map_in_bounds(x, y)) return (0);


		/* Stop at walls */
		/* note: beams end at walls.  */
		if (dist)
		{
			/* Stop at walls */
			/* note if beam, this is the end of the beam */
			/* dispel spells act like beams (sort of) */
			if (borg_cave_wall_grid(mb_ptr))
			{
				if (rad != -1)
					return (0);
				else
					return (n);
			}
		}

		/* Collect damage (bolts/beams) */
		if (rad <= 0) n += borg_launch_bolt_aux_hack(mb_ptr->kill, dam, typ);

		/* Check for arrival at "final target" */
		/* except beams, which keep going. */
		if (rad != -1 && x == x2 && y == y2) break;

		/* Stop bolts at monsters  */
		if (!rad && mb_ptr->monster) return (0);

		/* The missile path can be complicated.  There are several checks
		 * which need to be made.  First we assume that we targetting
		 * a monster.  That monster could be known from either sight or
		 * ESP.  If the entire pathway from us to the monster is known,
		 * then there is no concern.  But if the borg is shooting through
		 * unknown grids, then there is a concern when he has ESP; without
		 * ESP he would not see that monster if the unknown grids
		 * contained walls or closed doors.
		 *
		 * 1.  ESP Inactive
		 *   A.  No Infravision
		 *       -Then the monster must be in a lit grid.
		 *   B.  Yes Infravision
		 *       -Then the monster must be projectable()
		 * 2.  ESP Active
		 *   A. No Infravision
		 *       -Then the monster could be in a lit grid.
		 *       -Or I detect it with ESP and it's not projectable().
		 *   B.  Yes Infravision
		 *       -Then the monster could be projectable()
		 *       -Or I detect it with ESP and it's not projectable().
		 *   -In the cases of ESP Active, the borg will test fire a missile.
		 *    Then wait for a 'painful ouch' from the monster.
		 */

		/* dont do the check if esp */
		if (!borg_skill[BI_ESP])
		{
			/* Check the missile path--no Infra, no HAS_LITE */
			if (dist && (borg_skill[BI_INFRA] <= 0)
#ifdef MONSTER_LITE
				&& !(r_ptr->flags2 & RF2_HAS_LITE)
#endif /* has_lite */
				)
			{
				/* Stop at unknown grids (see above) */
				/* note if beam, dispel, this is the end of the beam */
				if (!mb_ptr->feat)
				{
					if (rad != -1)
						return (0);
					else
						return (n);
				}
				/* Stop at weird grids (see above) */
				/* note if beam, this is the end of the beam */
				if (mb_ptr->feat == FEAT_INVIS)
				{
					if (rad != -1)
						return (0);
					else
						return (n);
				}
				/* Stop at unseen walls */
				/* We just shot and missed, this is our next shot */
				if (successful_target < 0)
				{
					/* When throwing things, it is common to just 'miss' */
					/* Skip only one round in this case */
					if (successful_target == -12)
						successful_target = 0;
					if (rad != -1)
						return (0);
					else
						return (n);
				}
			}
			else				/* I do have infravision or it's a lite monster */
			{
				/* Stop at unseen walls */
				/* We just shot and missed, this is our next shot */
				if (successful_target < 0)
				{
					/* When throwing things, it is common to just 'miss' */
					/* Skip only one round in this case */
					if (successful_target == -12)
						successful_target = 0;
					if (rad != -1)
						return (0);
					else
						return (n);
				}
			}
		}
		else					/* I do have ESP */
		{
			/* Check the missile path */
			if (dist)
			{
				/* if this area has been magic mapped,
				   * ok to shoot in the dark
				 */
				if (mb_ptr->detect & BORG_DETECT_WALL)
				{
					/* Stop at unknown grids (see above) */
					/* note if beam, dispel, this is the end of the beam */
					if (!mb_ptr->feat)
					{
						if (rad != -1)
							return (0);
						else
							return (n);
					}
					/* Stop at unseen walls */
					/* We just shot and missed, this is our next shot */
					if (successful_target < 0)
					{
						/* When throwing things, it is common to just 'miss' */
						/* Skip only one round in this case */
						if (successful_target == -12)
							successful_target = 0;
						if (rad != -1)
							return (0);
						else
							return (n);
					}
				}

				/* Stop at weird grids (see above) */
				/* note if beam, this is the end of the beam */
				if (mb_ptr->feat == FEAT_INVIS)
				{
					if (rad != -1)
						return (0);
					else
						return (n);
				}
				/* Stop at unseen walls */
				/* We just shot and missed, this is our next shot */
				if (successful_target < 0)
				{
					/* When throwing things, it is common to just 'miss' */
					/* Skip only one round in this case */
					if (successful_target == -12)
						successful_target = 0;

					if (rad != -1)
						return (0);
					else
						return (n);
				}
			}
		}

		/* Calculate the new location */
		borgmove2(&y, &x, y1, x1, y2, x2);
	}

	/* Bolt/Beam attack */
	if (rad <= 0) return (n);

	/* Excessive distance */
	if (dist >= MAX_RANGE) return (0);

	/* Check monsters in blast radius  And apply Dispel type
	 * spells or spells which center on the player with a
	 * particular radius
	 */
	for (i = 0; i < borg_temp_n; i++)
	{
		/* Acquire location */
		x = borg_temp_x[i];
		y = borg_temp_y[i];

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Get the grid */
		mb_ptr = map_loc(x, y);

		/* Check distance */
		r = distance(y2, x2, y, x);

		/* Maximal distance */
		if (r > rad) continue;

		/* Never pass through walls */
		if (!borg_los(y2, x2, y, x)) continue;

		/*  dispel spells should hurt the same no matter the rad: make r= y  and x */
		if (rad == MAX_SIGHT) r = 0;

		/* Collect damage, lowered by distance */
		n += borg_launch_bolt_aux_hack(mb_ptr->kill, dam / (r + 1), typ);

		/* probable damage int was just changed by b_l_b_a_h */

		/* check destroyed stuff. */
		if (mb_ptr->object)
		{
			object_kind *k_ptr = &k_info[mb_ptr->object];

			switch (typ)
			{
				case GF_ACID:
				{
					/* rings/boots cost extra (might be speed!) */
					if (k_ptr->tval == TV_BOOTS)
					{
						n -= 200;
					}
					break;
				}
				case GF_ELEC:
				{
					/* rings/boots cost extra (might be speed!) */
					if (k_ptr->tval == TV_RING)
					{
						n -= 200;
					}
					break;
				}

				case GF_FIRE:
				{
					/* rings/boots cost extra (might be speed!) */
					if (k_ptr->tval == TV_BOOTS)
					{
						n -= 200;
					}
					break;
				}
				case GF_COLD:
				{
					if (k_ptr->tval == TV_POTION)
					{
						n -= 200;
					}
					break;
				}
				case GF_MANA:
				{
					/* Used against uniques, allow the stuff to burn */
					break;
				}
			}
		}
	}
	/* Result */
	return (n);
}


/*
 * Simulate/Apply the optimal result of launching a beam/bolt/ball
 *
 * Note that "beams" have a "rad" of "-1", "bolts" have a "rad" of "0",
 * and "balls" have a "rad" of "2" or "3", depending on "blast radius".
 *  dispel spells have a rad  of 10 or greater
 */
static int borg_launch_bolt(int rad, int dam, int typ, int max)
{
	int num = 0;

	int i, b_i = -1;
	int n = 0, b_n = 0;
	int b_o_y = 0, b_o_x = 0;
	int o_y = 0, o_x = 0;


	/* some Dispel-Type Spells have limted range */
	if (rad >= 10)
	{
		max = rad - 10;
	}

	/** Examine possible destinations **/

	/* Dispel Spells and Special Ball Spells centered on Player */
	if (rad >= 10)
	{
		/* Consider it centered on Player */
		return (borg_launch_bolt_aux(c_y, c_x, rad - 10, dam, typ, max));
	}

	/* This will allow the borg to target places adjacent to a monster
	 * in order to exploit and abuse a feature of the game.  Whereas,
	 * the borg, while targeting a monster will not score d/t walls, he
	 * could land a success hit by targeting adjacent to the monster.
	 * For example:
	 * ######################
	 * #####@..........######
	 * ############P..x......
	 * ######################
	 * In order to hit the P, the borg must target the x and not the P.
	 *
	 * This is a massive exploitation.  But it ranks with Farming, Scumming,
	 * Savefile abuse.
	 */
	for (i = 0; i < borg_temp_n; i++)
	{
		int x = borg_temp_x[i];
		int y = borg_temp_y[i];


		/* Consider each adjacent spot to and on top of the monster */
		for (o_x = -1; o_x <= 1; o_x++)
		{
			for (o_y = -1; o_y <= 1; o_y++)
			{
				/* Acquire location */
				x = borg_temp_x[i] + o_x;
				y = borg_temp_y[i] + o_y;

				/* Reset N */
				n = 0;

				/* Consider it if its a ball spell or right on top of it */
				if (rad >= 2 || (y == borg_temp_y[i] && x == borg_temp_x[i]))
				{
					n = borg_launch_bolt_aux(y, x, rad, dam, typ, max);
				}

				/* Skip useless attacks */
				if (n <= 0) continue;
#if 0
				/* The game forbids targetting the outside walls */
				if (x == 0 || y == 0 || x == MAX_WID - 1 || y == MAX_HGT - 1)
					continue;
#endif /* 0 */
				/* Collect best attack */
				if ((b_i >= 0) && (n < b_n)) continue;

				/* Hack -- reset chooser */
				if ((b_i >= 0) && (n > b_n)) num = 0;

				/* Apply the randomizer */
				if ((num > 1) && (randint0(num))) continue;

				/* Track it */
				b_i = i;
				b_n = n;
				b_o_y = o_y;
				b_o_x = o_x;
			}
		}
	}

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Save the location */
	g_x = borg_temp_x[b_i] + b_o_x;
	g_y = borg_temp_y[b_i] + b_o_y;

	/* Target the location */
	(void)borg_target(g_y, g_x);

	/* Result */
	return (b_n);
}




/*
 * Simulate/Apply the optimal result of launching a normal missile
 *
 * First, pick the "optimal" ammo, then pick the optimal target
 */
static int borg_attack_aux_launch(void)
{
	int b_n = 0;

	int k, b_k = -1;
	int d, b_d = -1;


	borg_item *bow = &borg_items[INVEN_BOW];

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{

		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		/* Skip Ego branded items--they are looked at later */
		if (item->xtra_name) continue;

		/* Skip bad missiles */
		if (item->tval != my_ammo_tval) continue;

		/* Skip worthless missiles */
		if (item->value <= 0) continue;

		/* Skip un-identified, non-average, missiles */
		if (!item->able && !strstr(item->desc, "{average") &&
			!strstr(item->desc, "{good") &&
			!strstr(item->desc, "{excellent")) continue;

		/* Determine average damage */
		d = (item->dd * (item->ds + 1) / 2);
		d = d + item->to_d + bow->to_d;
		d = d * my_ammo_power * borg_skill[BI_SHOTS];


		/* Paranoia */
		if (d <= 0) continue;

		if ((b_k >= 0) && (d <= b_d)) continue;

		b_k = k;
		b_d = d;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Choose optimal type of bolt */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Do it */
	borg_note(format("# Firing standard missile '%s'", borg_items[b_k].desc));

	/* Fire */
	borg_keypress('f');

	/* Use the missile */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -2;

	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of launching a SEEKER missile
 *
 * First, pick the "optimal" ammo, then pick the optimal target
 */
static int borg_attack_aux_launch_seeker(void)
{
	int b_n = 0;

	int k, b_k = -1;
	int d, b_d = -1;


	borg_item *bow = &borg_items[INVEN_BOW];

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{

		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		/* Skip non-seekers items--they are looked at later */
		if (item->sval != SV_AMMO_HEAVY) continue;

		/* Skip bad missiles */
		if (item->tval != my_ammo_tval) continue;

		/* Skip worthless missiles */
		if (item->value <= 0) continue;

		/* Skip un-identified, non-average, missiles */
		if (!item->able && !strstr(item->desc, "{average")) continue;

		/* Determine average damage */
		d = (item->dd * (item->ds + 1) / 2);
		d = d + item->to_d + bow->to_d;
		d = d * my_ammo_power * borg_skill[BI_SHOTS];


		/* Paranoia */
		if (d <= 0) continue;

		if ((b_k >= 0) && (d <= b_d)) continue;

		b_k = k;
		b_d = d;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Choose optimal type of bolt */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW_SEEKER, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);

	/* Do it */
	borg_note(format("# Firing seeker missile '%s'", borg_items[b_k].desc));

	/* Fire */
	borg_keypress('f');

	/* Use the missile */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -2;

	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of launching a branded missile
 *
 * First, pick the "optimal" ammo, then pick the optimal target
 */
static int borg_attack_aux_launch_flame(void)
{
	int b_n = 0;

	int k, b_k = -1;
	int d, b_d = -1;

	borg_item *bow = &borg_items[INVEN_BOW];

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{
		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		if (item->sval == SV_AMMO_HEAVY) continue;

		if (item->xtra_name) continue;

		/* Skip bad missiles */
		if (item->tval != my_ammo_tval) continue;

		/* Skip worthless missiles */
		if (item->value <= 0) continue;

		/* Skip un-identified, non-average, missiles */
		if (!item->able && !strstr(item->desc, "{average")) continue;

		/* Determine average damage */
		d = (item->dd * (item->ds + 1) / 2);
		d = d + item->to_d + bow->to_d;
		d = d * my_ammo_power * borg_skill[BI_SHOTS];


		/* Paranoia */
		if (d <= 0) continue;

		if ((b_k >= 0) && (d <= b_d)) continue;

		b_k = k;
		b_d = d;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Choose optimal type of bolt */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW_FLAME, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Do it */
	borg_note(format("# Firing flame branded missile '%s'",
					 borg_items[b_k].desc));

	/* Fire */
	borg_keypress('f');

	/* Use the missile */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -1;

	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of launching a branded missile
 *
 * First, pick the "optimal" ammo, then pick the optimal target
 */
static int borg_attack_aux_launch_frost(void)
{
	int b_n = 0;

	int k, b_k = -1;
	int d, b_d = -1;

	borg_item *bow = &borg_items[INVEN_BOW];

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{
		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		if (item->sval == SV_AMMO_HEAVY) continue;
		if (item->xtra_name) continue;

		/* Skip bad missiles */
		if (item->tval != my_ammo_tval) continue;

		/* Skip worthless missiles */
		if (item->value <= 0) continue;

		/* Skip un-identified, non-average, missiles */
		if (!item->able && !strstr(item->desc, "{average")) continue;

		/* Determine average damage */
		d = (item->dd * (item->ds + 1) / 2);
		d = d + item->to_d + bow->to_d;
		d = d * my_ammo_power * borg_skill[BI_SHOTS];

		/* Paranoia */
		if (d <= 0) continue;

		if ((b_k >= 0) && (d <= b_d)) continue;

		b_k = k;
		b_d = d;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Choose optimal type of bolt */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW_FROST, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Do it */
	borg_note(format("# Firing frost branded missile '%s'",
					 borg_items[b_k].desc));

	/* Fire */
	borg_keypress('f');

	/* Use the missile */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -2;

	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of launching a branded missile
 *
 * First, pick the "optimal" ammo, then pick the optimal target
 */
static int borg_attack_aux_launch_animal(void)
{
	int b_n = 0;

	int k, b_k = -1;
	int d, b_d = -1;

	borg_item *bow = &borg_items[INVEN_BOW];

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{
		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		if (item->sval == SV_AMMO_HEAVY) continue;
		if (item->xtra_name) continue;

		/* Skip bad missiles */
		if (item->tval != my_ammo_tval) continue;

		/* Skip worthless missiles */
		if (item->value <= 0) continue;

		/* Skip un-identified, non-average, missiles */
		if (!item->able && !strstr(item->desc, "{average")) continue;

		/* Determine average damage */
		d = (item->dd * (item->ds + 1) / 2);
		d = d + item->to_d + bow->to_d;
		d = d * my_ammo_power * borg_skill[BI_SHOTS];


		/* Paranoia */
		if (d <= 0) continue;

		if ((b_k >= 0) && (d <= b_d)) continue;

		b_k = k;
		b_d = d;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Choose optimal type of bolt */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW_ANIMAL, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Do it */
	borg_note(format("# Firing animal missile '%s'", borg_items[b_k].desc));

	/* Fire */
	borg_keypress('f');

	/* Use the missile */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -2;

	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of launching a branded missile
 *
 * First, pick the "optimal" ammo, then pick the optimal target
 */
static int borg_attack_aux_launch_evil(void)
{
	int b_n = 0;

	int k, b_k = -1;
	int d, b_d = -1;

	borg_item *bow = &borg_items[INVEN_BOW];

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{
		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		if (item->sval == SV_AMMO_HEAVY) continue;
		if (item->xtra_name) continue;

		/* Skip bad missiles */
		if (item->tval != my_ammo_tval) continue;

		/* Skip worthless missiles */
		if (item->value <= 0) continue;

		/* Skip un-identified, non-average, missiles */
		if (!item->able && !strstr(item->desc, "{average")) continue;

		/* Determine average damage */
		d = (item->dd * (item->ds + 1) / 2);
		d = d + item->to_d + bow->to_d;
		d = d * my_ammo_power * borg_skill[BI_SHOTS];


		/* Paranoia */
		if (d <= 0) continue;

		if ((b_k >= 0) && (d <= b_d)) continue;

		b_k = k;
		b_d = d;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Choose optimal type of bolt */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW_EVIL, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Do it */
	borg_note(format("# Firing evil branded missile '%s'",
					 borg_items[b_k].desc));

	/* Fire */
	borg_keypress('f');

	/* Use the missile */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -2;

	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of launching a branded missile
 *
 * First, pick the "optimal" ammo, then pick the optimal target
 */
static int borg_attack_aux_launch_dragon(void)
{
	int b_n = 0;

	int k, b_k = -1;
	int d, b_d = -1;

	borg_item *bow = &borg_items[INVEN_BOW];

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{
		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		if (item->sval == SV_AMMO_HEAVY) continue;
		if (item->xtra_name) continue;

		/* Skip bad missiles */
		if (item->tval != my_ammo_tval) continue;

		/* Skip worthless missiles */
		if (item->value <= 0) continue;

		/* Skip un-identified, non-average, missiles */
		if (!item->able && !strstr(item->desc, "{average")) continue;

		/* Determine average damage */
		d = (item->dd * (item->ds + 1) / 2);
		d = d + item->to_d + bow->to_d;
		d = d * my_ammo_power * borg_skill[BI_SHOTS];


		/* Paranoia */
		if (d <= 0) continue;

		if ((b_k >= 0) && (d <= b_d)) continue;

		b_k = k;
		b_d = d;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Choose optimal type of bolt */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW_DRAGON, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Do it */
	borg_note(format("# Firing dragon branded missile '%s'",
					 borg_items[b_k].desc));

	/* Fire */
	borg_keypress('f');

	/* Use the missile */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -2;

	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of launching a branded missile
 *
 * First, pick the "optimal" ammo, then pick the optimal target
 */
static int borg_attack_aux_launch_wounding(void)
{
	int b_n = 0;

	int k, b_k = -1;
	int d, b_d = -1;

	borg_item *bow = &borg_items[INVEN_BOW];

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{
		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		if (item->sval == SV_AMMO_HEAVY) continue;
		if (item->xtra_name) continue;

		/* Skip bad missiles */
		if (item->tval != my_ammo_tval) continue;

		/* Skip worthless missiles */
		if (item->value <= 0) continue;

		/* Skip un-identified, non-average, missiles */
		if (!item->able && !strstr(item->desc, "{average")) continue;

		/* Determine average damage */
		d = (item->dd * (item->ds + 1) / 2);
		d = d + item->to_d + bow->to_d;
		d = d * my_ammo_power * borg_skill[BI_SHOTS];


		/* Paranoia */
		if (d <= 0) continue;

		if ((b_k >= 0) && (d <= b_d)) continue;

		b_k = k;
		b_d = d;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Choose optimal type of bolt */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW_WOUNDING, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Do it */
	borg_note(format("# Firing wounding branded missile '%s'",
					 borg_items[b_k].desc));

	/* Fire */
	borg_keypress('f');

	/* Use the missile */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -2;

	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of throwing an object
 *
 * First choose the "best" object to throw, then check targets.
 */
static int borg_attack_aux_object(void)
{
	int b_n;

	int b_r = 0;

	int k, b_k = -1;
	int d, b_d = -1;

	int div, mul;

	/* Scan the pack */
	for (k = 0; k < INVEN_PACK; k++)
	{
		borg_item *item = &borg_items[k];

		/* Skip empty items */
		if (!item->iqty) continue;

		/* Skip un-identified, non-average, objects */
		if (!item->able && !strstr(item->desc, "{average")) continue;

		/* Skip "equipment" items (not ammo) */
		if (borg_wield_slot(item) >= 0) continue;

		/* Determine average damage from object */
		d = (k_info[item->kind].dd * (k_info[item->kind].ds + 1) / 2);

		/* Skip useless stuff */
		if (d <= 0) continue;

		/* Skip "expensive" stuff */
		if (d < item->value) continue;

		/* Hack -- Save last seven flasks for fuel, if needed */
		if (item->tval == TV_FLASK &&
			(borg_skill[BI_AFUEL] <= 7 && !borg_fighting_unique)) continue;

		/* Ignore worse damage */
		if ((b_k >= 0) && (d <= b_d)) continue;

		/* Track */
		b_k = k;
		b_d = d;

		/* Extract a "distance multiplier" */
		mul = 10;

		/* Enforce a minimum "weight" of one pound */
		div = ((item->weight > 10) ? item->weight : 10);

		/* Hack -- Distance -- Reward strength, penalize weight */
		b_r = (adj_str_blow[my_stat_ind[A_STR]] + 20) * mul / div;

		/* Max distance of 10 */
		if (b_r > 10) b_r = 10;
	}

	/* Nothing to use */
	if (b_k < 0) return (0);


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);


	/* Choose optimal location */
	b_n = borg_launch_bolt(0, b_d, GF_ARROW, b_r);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Do it */
	borg_note(format("# Throwing painful object '%s'", borg_items[b_k].desc));

	/* Fire */
	borg_keypress('v');

	/* Use the object */
	borg_keypress(I2A(b_k));

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -2;

	/* Value */
	return (b_n);
}




/*
 * Simulate/Apply the optimal result of using a "normal" attack spell
 *
 * Take into account the failure rate of spells/objects/etc.  XXX XXX XXX
 */
static int borg_attack_aux_spell_bolt(int realm, int book, int what, int rad,
                                      int dam, int typ)
{
	int b_n;
	int penalty = 0;

	borg_magic *as = &borg_magics[realm][book][what];


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* make sure I am powerfull enough to do another goi if this one falls */
	if (borg_goi && ((borg_skill[BI_CURSP] - as->power) < 70)) return (0);

	/* Paranoia */
	if (borg_simulate && (randint0(100) < 5)) return (0);

	/* Require ability (right now) */
	if (!borg_spell_okay_fail
		(realm, book, what, (borg_fighting_unique ? 40 : 25))) return (0);


	/* Choose optimal location */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);

	/* weaklings need that spell, they dont get penalized */
	if (book == 0 && what == 0 && borg_skill[BI_MAXCLEVEL] <= 30)
	{
		if (borg_simulate) return (b_n);
	}

	/* Penalize mana usage */
	b_n = b_n - as->power;

	/* Penalize use of reserve mana */
	if (borg_skill[BI_CURSP] - as->power < borg_skill[BI_MAXSP] / 2) b_n =
			b_n - (as->power * 10);

	/* Penalize use of deep reserve mana */
	if (borg_skill[BI_CURSP] - as->power < borg_skill[BI_MAXSP] / 3) b_n =
			b_n - (as->power * 20);

	/* Really penalize use of mana needed for final teleport */
	if (borg_class == CLASS_MAGE) penalty = 6;
	if (borg_class == CLASS_RANGER) penalty = 22;
	if (borg_class == CLASS_ROGUE) penalty = 20;
	if (borg_class == CLASS_PRIEST) penalty = 8;
	if (borg_class == CLASS_PALADIN) penalty = 20;
	if ((borg_skill[BI_MAXSP] > 30) &&
		(borg_skill[BI_CURSP] - as->power) < penalty)
		b_n = b_n - (as->power * 750);


	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Cast the spell */
	(void)borg_spell(realm, book, what);

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -1;

	/* Value */
	return (b_n);
}

/* This routine is the same as the one above only in an emergency case.
 * The borg will enter negative mana casting this
 */
static int borg_attack_aux_spell_bolt_reserve(int realm, int book, int what,
                                              int rad, int dam, int typ)
{
	int b_n;
	int i;

	/* Fake our Mana */
	int sv_mana = borg_skill[BI_CURSP];

	/* Only Weak guys should try this */
	if (borg_skill[BI_CLEVEL] >= 15) return (0);

	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Must not have enough mana right now */
	if (borg_spell_okay_fail(realm, book, what, 25)) return (0);

	/* If there is more than one close monster, don't risk fainting */
	if (borg_temp_n > 1) return (0);

	/* Must be dangerous */
	if (borg_danger(c_y, c_x, 1, TRUE) < avoidance * 2) return (0);

	/* Find the monster */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		borg_kill *kill;

		/* Monster */
		kill = &borg_kills[i];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Require current knowledge */
		if (kill->when < borg_t) continue;

		/* check the location */
		if (borg_temp_x[0] != kill->x || borg_temp_y[0] != kill->y)
			continue;

		/* If it has too many hp to be taken out with this */
		/* spell, don't bother trying */
		/* NOTE: the +4 is because the damage is toned down
		   as an 'average damage' */
		if (kill->power > (dam + 4))
			return (0);

		break;
	}

	/* Require ability (with faked mana) */
	borg_skill[BI_CURSP] = borg_skill[BI_MAXSP];
	if (!borg_spell_okay_fail(realm, book, what, 25))
	{
		/* Restore Mana */
		borg_skill[BI_CURSP] = sv_mana;
		return (0);
	}

	/* Choose optimal location */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);

	/* return the value */
	if (borg_simulate)
	{
		/* Restore Mana */
		borg_skill[BI_CURSP] = sv_mana;
		return (b_n);
	}

	/* Cast the spell with fake mana */
	borg_skill[BI_CURSP] = borg_skill[BI_MAXSP];
	if (borg_spell_fail(realm, book, what, 25))
	{
		/* Note the use of the emergency spell */
		borg_note("# Emergency use of an Attack Spell.");

		/* verify use of spell */
		/* borg_keypress('y'); */
	}

	/* Use target */
	/* borg_keypress('5'); */
	borg_confirm_target = TRUE;


	/* Set our shooting flag */
	successful_target = -1;

	/* restore true mana */
	borg_skill[BI_CURSP] = 0;

	/* Value */
	return (b_n);
}


/*
 *  Simulate/Apply the optimal result of using a "dispel" attack prayer
 */
static int borg_attack_aux_spell_dispel(int realm, int book, int what, int rad,
                                        int dam, int typ)
{
	int b_n;
	int penalty = 0;

	borg_magic *as = &borg_magics[realm][book][what];

	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);


	/* Paranoia */
	if (borg_simulate && (randint0(100) < 5)) return (0);


	/* Require ability */
	if (!borg_spell_okay_fail(realm, book, what, 25)) return (0);

	/* Choose optimal location-- */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);

	/* Penalize mana usage */
	b_n = b_n - as->power;

	/* Penalize use of reserve mana */
	if (borg_skill[BI_CURSP] - as->power < borg_skill[BI_MAXSP] / 2) b_n =
			b_n - (as->power * 10);

	/* Penalize use of deep reserve mana */
	if (borg_skill[BI_CURSP] - as->power < borg_skill[BI_MAXSP] / 3) b_n =
			b_n - (as->power * 20);

	/* Really penalize use of mana needed for final teleport */
	if (borg_class == CLASS_MAGE) penalty = 6;
	if (borg_class == CLASS_RANGER) penalty = 22;
	if (borg_class == CLASS_ROGUE) penalty = 20;
	if (borg_class == CLASS_PRIEST) penalty = 8;
	if (borg_class == CLASS_PALADIN) penalty = 20;
	if ((borg_skill[BI_MAXSP] > 30) &&
		(borg_skill[BI_CURSP] - as->power) < penalty)
		b_n = b_n - (as->power * 750);

	/* Simulation */
	if (borg_simulate) return (b_n);

	/* Cast the prayer */
	(void)borg_spell(realm, book, what);


	/* Value */
	return (b_n);
}

/*
 * Simulate/Apply the optimal result of using a "normal" attack spell
 *
 * Take into account the failure rate of spells/objects/etc.  XXX XXX XXX
 */
static int borg_attack_aux_mind_bolt(int spell, int level, int rad, int dam,
                                     int typ)
{
	int b_n = -1;
	int penalty = 0;

	borg_mind *as = &borg_minds[spell];

	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);

	/* Paranoia */
	if (borg_simulate && (randint0(100) < 5)) return (0);

	/* Require ability (right now) */
	if (!borg_mindcr_okay_fail(spell, level, (borg_fighting_unique ? 40 : 25)))
		return (0);


	/* Choose optimal location */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);

	/* weaklings need that spell, they dont get penalized */
	if (spell == MIND_NEURAL_BL && borg_skill[BI_MAXCLEVEL] <= 30)
	{
		if (borg_simulate) return (b_n);
	}

	/* Penalize mana usage */
	b_n = b_n - as->power;

	/* Penalize use of reserve mana */
	if (borg_skill[BI_CURSP] - as->power < borg_skill[BI_MAXSP] / 2) b_n =
			b_n - (as->power * 10);

	/* Penalize use of deep reserve mana */
	if (borg_skill[BI_CURSP] - as->power < borg_skill[BI_MAXSP] / 3) b_n =
			b_n - (as->power * 20);

	/* Really penalize use of mana needed for final teleport */
	penalty = 6;
	if ((borg_skill[BI_MAXSP] > 30) &&
		(borg_skill[BI_CURSP] - as->power) < penalty)
		b_n = b_n - (as->power * 750);

	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Cast the spell */
	(void)borg_mindcr(spell, level);

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -1;

	/* Value */
	return (b_n);
}


/*
 *  Simulate/Apply the optimal result of using a "dispel" staff
 * Which would be dispel evil, power, holiness.  Genocide handeled later.
 */
static int borg_attack_aux_staff_dispel(int sval, int rad, int dam, int typ)
{
	int i, b_n;

	/* hack - ignore parameter */
	(void)rad;

	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);


	/* Paranoia */
	if (borg_simulate && (randint0(100) < 5)) return (0);

	/* look for the staff */
	if (!borg_equips_staff_fail(sval)) return (0);
	i = borg_slot(TV_STAFF, sval);

	/* Choose optimal location--radius defined as 10 */
	b_n = borg_launch_bolt(MAX_SIGHT + 10, dam, typ, MAX_RANGE);

	/* Big Penalize charge usage */
	b_n = b_n - 50;

	/* Simulation */
	if (borg_simulate) return (b_n);

	/* Cast the prayer */
	(void)borg_use_staff(sval);


	/* Value */
	return (b_n);
}



/*
 * Simulate/Apply the optimal result of using a "normal" attack rod
 */
static int borg_attack_aux_rod_bolt(int sval, int rad, int dam, int typ)
{
	int b_n;


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);


	/* Paranoia */
	if (borg_simulate && (randint0(100) < 5)) return (0);


	/* Look for that rod */
	if (!borg_equips_rod(sval)) return (0);

	/* Choose optimal location */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);

	/* Zap the rod */
	(void)borg_zap_rod(sval);

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -1;

	/* Value */
	return (b_n);
}



/*
 * Simulate/Apply the optimal result of using a "normal" attack wand
 */
static int borg_attack_aux_wand_bolt(int sval, int rad, int dam, int typ)
{
	int i;

	int b_n;


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);


	/* Paranoia */
	if (borg_simulate && (randint0(100) < 5)) return (0);


	/* Look for that wand */
	i = borg_slot(TV_WAND, sval);

	/* None available */
	if (i < 0) return (0);

	/* No charges */
	if (!borg_items[i].pval) return (0);


	/* Choose optimal location */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);

	/* Penalize charge usage */
	b_n = b_n - 5;

	/* Wands of wonder are used in last ditch efforts.  They behave
	 * randomly, so the best use of them is an emergency.  I have seen
	 * borgs die from hill orcs with fully charged wonder wands.  Odds
	 * are he could have taken the orcs with the wand.  So use them in
	 * an emergency after all the borg_caution() steps have failed
	 */
	if (sval == SV_WAND_WONDER)
	{
		/* check the danger */
		if (b_n > 0 && borg_danger(c_y, c_x, 1, TRUE) >= (avoidance * 2))
		{
			/* make the wand appear deadly */
			b_n = 999;

			/* note the use of the wand in the emergency */
			borg_note(format("# Emergency use of a Wand of Wonder."));
		}
		else
		{
			b_n = 0;
		}
	}

	/* Simulation */
	if (borg_simulate) return (b_n);

	/* Aim the wand */
	(void)borg_aim_wand(sval);

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -1;

	/* Value */
	return (b_n);
}

#if 0
/*
 * Simulate/Apply the optimal result of ACTIVATING an attack artifact
 *
 */
static int borg_attack_aux_artifact(int art_name, int art_loc, int rad, int dam,
                                    int typ)
{
	int b_n;

	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);


	/* Paranoia */
	if (borg_simulate && (randint0(100) < 5)) return (0);


	/* Look for that artifact and to see if it is charged */
	if (!borg_equips_artifact(art_name, art_loc)) return (0);

	/* Choose optimal location */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);

	/* Activate the artifact */
	(void)borg_activate_artifact(art_name, FALSE);

	/* Use target */
	if (art_name != ART_INGWE || art_name != ART_RAZORBACK)
	{
		borg_keypress('5');

		/* Set our shooting flag */
		successful_target = -1;
	}

	/* Value */
	return (b_n);
}

#endif /* 0 */

/*
 * Simulate/Apply the optimal result of ACTIVATING a DRAGON ARMOUR
 *
 */
static int borg_attack_aux_dragon(int sval, int rad, int dam, int typ)
{
	int b_n;


	/* No firing while blind, confused, or hallucinating */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE]) return (0);


	/* Paranoia */
	if (borg_simulate && (randint0(100) < 5)) return (0);


	/* Look for that scale mail and charged */
	if (!borg_equips_dragon(sval)) return (0);

	/* Choose optimal location */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);

	/* Simulation */
	if (borg_simulate) return (b_n);

	/* Activate the scale mail */
	(void)borg_activate_dragon(sval);

	/* Use target */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -1;

	/* Value */
	return (b_n);
}

/* Whirlwind--
 * Attacks adjacent monsters
 */
static int borg_attack_aux_nature_whirlwind(void)
{
	int y = 0, x = 0;
	int dir;

	int dam = 0;

	map_block *mb_ptr;

	if (!borg_spell_fail(REALM_NATURE, 3, 1, 20)) return 0;

	/* Scan neighboring grids */
	for (dir = 0; dir <= 9; dir++)
	{
		y = c_y + ddy[dir];
		x = c_x + ddx[dir];

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		mb_ptr = map_loc(x, y);

		/* is ther ea kill next to me */
		if (mb_ptr->kill)
		{
			/* Calculate "average" damage */
			dam += borg_thrust_damage_one(mb_ptr->kill);
		}

	}

	/* Return the damage for consideration */
	if (borg_simulate) return (dam);

	/* Cast the spell, perform the attack */
	if (borg_spell_fail(REALM_NATURE, 3, 1, 20)) return (dam);

	/* */
	return (0);
}

/* Call Void--
 * Multiple Attacks, careful for walls
 */
static int borg_attack_aux_spell_callvoid(void)
{
	int y = 0, x = 0;
	int dir;

	int dam = 0;

	map_block *mb_ptr;

	if (!borg_spell_fail(REALM_CHAOS, 3, 7, 20)) return (0);
	/* Scan neighboring grids */
	for (dir = 0; dir <= 9; dir++)
	{
		y = c_y + ddy[dir];
		x = c_x + ddx[dir];

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		mb_ptr = map_loc(x, y);

		/* is there a kill next to me */
		if (mb_ptr->feat >= FEAT_MAGMA && mb_ptr->feat <= FEAT_PERM_SOLID)
		{
			return (0);

		}
		else
		{
			/* Calculate "average" damage */
			dam += borg_launch_bolt(10 + 2, 175, GF_SHARDS, MAX_RANGE);
			dam += borg_launch_bolt(10 + 2, 175, GF_MANA, MAX_RANGE);
			dam += borg_launch_bolt(10 + 4, 175, GF_NUKE, MAX_RANGE);
		}

	}

	/* Return the damage for consideration */
	if (borg_simulate) return (dam);

	/* Cast the spell, perform the attack */
	if (borg_spell_fail(REALM_CHAOS, 3, 7, 20)) return (dam);

	/* */
	return (0);
}

#if 0

/* Holcolleth -- sleep adjacent monsters */
static int borg_attack_aux_artifact_holcolleth(void)
{
	int p1 = 0;
	int p2 = 0;
	int d = 0;

	/* Obtain initial danger */
	borg_sleep_spell = FALSE;
	p1 = borg_danger(c_y, c_x, 4, TRUE);

	if (!borg_equips_artifact(ART_HOLCOLLETH, INVEN_OUTER))
		return (0);

	/* What effect is there? */
	borg_sleep_spell_ii = TRUE;
	p2 = borg_danger(c_y, c_x, 4, TRUE);
	borg_sleep_spell_ii = FALSE;

	/* value is d, enhance the value for rogues and rangers so that
	 * they can use their critical hits.
	 */
	d = (p1 - p2);

	/* Simulation */
	if (borg_simulate) return (d);

	/* Cast the spell */
	if (borg_activate_artifact(ART_HOLCOLLETH, FALSE))
		/* Value */
	{
		return (d);
	}
	else
		return (0);
}

#endif /* 0 */

/*
 * Simulate/Apply the optimal result of making a racial physical attack
 */
static int borg_attack_aux_racial_thrust(int race, int level, int dam)
{
	int p, dir;

	int i, b_i = -1;
	int d, b_d = -1;

	map_block *mb_ptr;

	borg_kill *kill;

	monster_race *r_ptr;

	/* Too afraid to attack */
	if (borg_skill[BI_ISAFRAID]) return (0);

	/* must be right race */
	if (borg_race != race) return (0);

	/* must be right level */
	if (borg_skill[BI_CLEVEL] < level) return (0);

	/* Examine possible destinations */
	for (i = 0; i < borg_temp_n; i++)
	{
		int x = borg_temp_x[i];
		int y = borg_temp_y[i];

		/* Require "adjacent" */
		if (distance(c_y, c_x, y, x) > 1) continue;

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Acquire grid */
		mb_ptr = map_loc(x, y);

		/* Obtain the monster */
		kill = &borg_kills[mb_ptr->kill];

		/* monster race */
		r_ptr = &r_info[mb_ptr->monster];

		/* Dont attack our buddies */
		if (kill->friendly) continue;

		/* Base Dam */
		d = dam;

		/* Vampire Drain */
		if (race == RACE_VAMPIRE)
		{
			if (!borg_skill[BI_ISFULL]) d = d * 13 / 10;
			if (borg_skill[BI_ISHUNGRY]) d = d * 13 / 10;

			/* Drain gives food */
			if (!monster_living(r_ptr)) continue;

		}

		/* Hack -- avoid waking most "hard" sleeping monsters */
		if (!kill->awake && (d <= kill->power))
		{
			/* Calculate danger */
			borg_full_damage = TRUE;
			p = borg_danger_aux(y, x, 1, mb_ptr->kill, TRUE);
			borg_full_damage = FALSE;

			if (p > avoidance / 2)
				continue;
		}

		/* Hack -- ignore sleeping town monsters */
		if (!borg_skill[BI_CDEPTH] && !kill->awake) continue;

		/* Calculate "danger" to player */
		borg_full_damage = TRUE;
		p = borg_danger_aux(c_y, c_x, 2, mb_ptr->kill, TRUE);
		borg_full_damage = FALSE;

		/* Reduce "bonus" of partial kills */
		if (d <= kill->power) p = p / 10;

		/* Add the danger to the damage */
		d += p;

		/* Ignore lower damage */
		if ((b_i >= 0) && (d < b_d)) continue;

		/* Save the info */
		b_i = i;
		b_d = d;
	}

	/* Nothing to attack */
	if (b_i < 0) return (0);

	/* Simulation */
	if (borg_simulate) return (b_d);

	/* Save the location */
	g_x = borg_temp_x[b_i];
	g_y = borg_temp_y[b_i];

	mb_ptr = map_loc(g_x, g_y);

	kill = &borg_kills[mb_ptr->kill];

	/* Note */
	borg_note(format
			  ("# Facing %s at (%d,%d) who has %d Hit Points.",
			   (r_name + r_info[mb_ptr->monster].name), g_y, g_x, kill->power));
	borg_note(format("# Attacking with Racial Attack '%d'", b_d));

	/* Get a direction for attacking */
	dir = borg_extract_dir(c_y, c_x, g_y, g_x);

	/* Activate */
	borg_keypress('U');

	/* Racial is always 'a' */
	borg_keypress('a');

	/* Attack the grid */
	borg_keypress(I2D(dir));

	/* Success */
	return (b_d);
}

/*
 * Simulate/Apply the optimal result of making a racial attack.
 */
static int borg_attack_aux_racial_bolt(int race, int rad, int dam, int typ)
{
	int b_n = -1;

	/* Cost of ability, affordable */

	/* Check for ability */
	if (!borg_racial_check(race, TRUE)) return (FALSE);

	/* Choose optimal location */
	b_n = borg_launch_bolt(rad, dam, typ, MAX_RANGE);


	/* Simulation */
	if (borg_simulate) return (b_n);


	/* Note */
	borg_note(format("# Racial Attack -- for '%d'", b_n));

	/* Activate */
	borg_keypress('U');

	/* Racial is always 'a' */
	borg_keypress('a');

	/* Attack the previously targeted grid */
	borg_keypress('5');

	/* Set our shooting flag */
	successful_target = -1;

	/* Success */
	return (b_n);
}


/*
 * Simulate/Apply the optimal result of using the given "type" of attack
 */
static int borg_attack_aux(int what)
{
	int dam = 0, chance, rad = 0;

	/* Analyze */
	switch (what)
	{
		case BF_THRUST:
		{
			/* Physical attack */
			return (borg_attack_aux_thrust());
		}

		case BF_LAUNCH_NORMAL:
		{
			/* Missile attack */
			return (borg_attack_aux_launch());
		}

		case BF_LAUNCH_SEEKER:
		{
			/* Missile attack */
			return (borg_attack_aux_launch_seeker());
		}

		case BF_LAUNCH_FLAME:
		{
			/* Missile attack */
			return (borg_attack_aux_launch_flame());
		}

		case BF_LAUNCH_FROST:
		{
			/* Missile attack */
			return (borg_attack_aux_launch_frost());
		}

		case BF_LAUNCH_ANIMAL:
		{
			/* Missile attack */
			return (borg_attack_aux_launch_animal());
		}

		case BF_LAUNCH_EVIL:
		{
			/* Missile attack */
			return (borg_attack_aux_launch_evil());
		}

		case BF_LAUNCH_DRAGON:
		{
			/* Missile attack */
			return (borg_attack_aux_launch_dragon());
		}

		case BF_LAUNCH_WOUNDING:
		{
			/* Missile attack */
			return (borg_attack_aux_launch_wounding());
		}

		case BF_OBJECT:
		{
			/* Object attack */
			return (borg_attack_aux_object());
		}

		case BF_LIFE_CALL_LIGHT:
		{
			/* Spell -- Call Light */
			dam = (2 * borg_skill[BI_CLEVEL]) / 2;
			rad = 10 + (borg_skill[BI_CLEVEL] / 10) + 1;
			return (borg_attack_aux_spell_dispel
					(REALM_LIFE, 0, 4, rad, dam, GF_LITE_WEAK));
		}

		case BF_LIFE_HOLY_ORB:
		{
			/* Spell -- Holy Orb */
			dam =
				((3 * 3) + borg_skill[BI_CLEVEL] + (borg_skill[BI_CLEVEL] / 3));
			rad = 2;
			return (borg_attack_aux_spell_bolt
					(REALM_LIFE, 1, 4, rad, dam, GF_HOLY_FIRE));
		}

		case BF_LIFE_EXORCISM:
		{
			/* Spell -- Exorcism */
			dam = borg_skill[BI_CLEVEL] / 2;
			rad = MAX_SIGHT + 10;
			return (borg_attack_aux_spell_dispel
					(REALM_LIFE, 2, 0, rad, dam, GF_DISP_UNDEAD_DEMON));
		}

		case BF_LIFE_DISP_UNDEAD:
		{
			/* Spell -- Disp Undead & Demon */
			dam = (borg_skill[BI_CLEVEL] * 3) / 2;
			rad = MAX_SIGHT + 10;
			return (borg_attack_aux_spell_dispel
					(REALM_LIFE, 2, 2, rad, dam, GF_DISP_UNDEAD_DEMON));
		}

		case BF_LIFE_DISP_EVIL:
		{
			/* Spell -- Disp Evil */
			dam = (borg_skill[BI_CLEVEL] * 4) / 2;
			rad = MAX_SIGHT + 10;
			return (borg_attack_aux_spell_dispel
					(REALM_LIFE, 2, 4, rad, dam, GF_DISP_EVIL));
		}

		case BF_LIFE_HOLY_WORD:
		{
			/* Holy Word also has heal effect and is considered in borg_heal */
			rad = MAX_SIGHT + 10;
			if (borg_skill[BI_MAXHP] - borg_skill[BI_CURHP] >= 300)
				/*  force him to think the spell is more deadly to get him to
				 * cast it.  This will provide some healing for him.
				 */
			{
				dam = ((borg_skill[BI_CLEVEL] * 10));
				return (borg_attack_aux_spell_dispel
						(REALM_LIFE, 2, 6, rad, dam, GF_DISP_EVIL));
			}
			else				/* If he is not wounded dont cast this, use Disp Evil instead. */
			{
				dam = ((borg_skill[BI_CLEVEL] * 3) / 2) - 50;
				return (borg_attack_aux_spell_dispel
						(REALM_LIFE, 2, 6, rad, dam, GF_DISP_EVIL));
			}
		}

		case BF_LIFE_DIVINE_INT:
		{
			/* Spell -- Divine Intervention */
			dam = borg_skill[BI_CLEVEL] * 4 / 2;
			rad = MAX_SIGHT + 10;
			/* if hurting, add bonus */
			if (borg_skill[BI_MAXHP] - borg_skill[BI_CURHP] >= 200) dam =
					(dam * 12) / 10;
			/* if no speedy, add bonus */
			if (!borg_speed) dam = (dam * 13) / 10;
			/* bonus for refreshing the speedy */
			if (borg_speed) dam = (dam * 11) / 10;
			return (borg_attack_aux_spell_dispel
					(REALM_LIFE, 3, 6, rad, dam, GF_HOLY_FIRE));
		}

		case BF_ARCANE_ZAP:
		{
			/* Spell -- Zap */
			dam = (3 + ((borg_skill[BI_CLEVEL] - 1) / 5)) / 2;
			return (borg_attack_aux_spell_bolt
					(REALM_ARCANE, 0, 0, rad, dam, GF_ELEC));
		}

		case BF_ARCANE_ZAP_RESERVE:
		{
			/* Emergency cast of Zap */
			dam = (3 + ((borg_skill[BI_CLEVEL] - 1) / 5)) / 2;
			return (borg_attack_aux_spell_bolt_reserve
					(REALM_ARCANE, 0, 0, rad, dam, GF_ELEC));
		}

		case BF_ARCANE_LAREA:
		{
			/* Spell -- Light Area */
			dam = (2 * (borg_skill[BI_CLEVEL]) / 2) / 2;
			rad = borg_skill[BI_CLEVEL] + 1 + 10;
			return (borg_attack_aux_spell_dispel
					(REALM_ARCANE, 0, 5, rad, dam, GF_LITE_WEAK));
		}

		case BF_ARCANE_STONEMUD:
		{
			/* Spell -- Stone to Mud */
			dam = 45;
			return (borg_attack_aux_spell_bolt
					(REALM_ARCANE, 2, 4, rad, dam, GF_KILL_WALL));
		}

		case BF_ARCANE_LBEAM:
		{
			/* Spell -- Light Beam */
			dam = 30;
			rad = -1;
			return (borg_attack_aux_spell_bolt
					(REALM_ARCANE, 2, 5, rad, dam, GF_LITE_WEAK));
		}

		case BF_ARCANE_ELEM_BALL:
		{
			/* Spell -- Elem Ball */
			dam = (75 + borg_skill[BI_CLEVEL]);
			return (borg_attack_aux_spell_bolt
					(REALM_ARCANE, 2, 6, rad, dam, GF_ELEMENTS));
		}

		case BF_SORC_LAREA:
		{
			/* Spell -- Light area */
			dam = (2 * (borg_skill[BI_CLEVEL]) / 2) / 2;
			rad = borg_skill[BI_CLEVEL] + 1 + 10;
			return (borg_attack_aux_spell_dispel
					(REALM_SORCERY, 0, 3, rad, dam, GF_LITE_WEAK));
		}

		case BF_SORC_CONF_MON:
		{
			/* Spell -- Confuse Monster */
			dam = 10;
			return (borg_attack_aux_spell_bolt
					(REALM_SORCERY, 0, 4, rad, dam, GF_OLD_CONF));
		}

		case BF_SORC_SLEEP_I:
		{
			/* Spell -- Sleep I */
			dam = 10;
			return (borg_attack_aux_spell_bolt
					(REALM_SORCERY, 0, 6, rad, dam, GF_OLD_SLEEP));
		}

		case BF_SORC_SLOW_MON:
		{
			/* Spell -- Slow I */
			dam = 10;
			return (borg_attack_aux_spell_bolt
					(REALM_SORCERY, 1, 2, rad, dam, GF_OLD_SLOW));
		}

		case BF_SORC_SLEEP_III:
		{
			/* Spell -- Sleep III */
			rad = MAX_SIGHT + 10;
			dam = 10;
			return (borg_attack_aux_spell_dispel
					(REALM_SORCERY, 1, 3, rad, dam, GF_OLD_SLEEP));
		}

		case BF_SORC_STASIS:
		{
			/* Spell -- Stasis */
			dam = 30;
			return (borg_attack_aux_spell_bolt
					(REALM_SORCERY, 3, 0, rad, dam, GF_STASIS));
		}

		case BF_TRUMP_MINDBLAST:
		{
			/* Spell -- Mind Blast */
			dam = 3 * ((borg_skill[BI_CLEVEL] - 1) / 5);
			return (borg_attack_aux_spell_bolt
					(REALM_TRUMP, 0, 1, rad, dam, GF_PSI));
		}

		case BF_TRUMP_MINDBLAST_RESERVE:
		{
			/* Spell -- Mind Blast Reserve */
			dam = 3 * ((borg_skill[BI_CLEVEL] - 1) / 5);
			return (borg_attack_aux_spell_bolt_reserve
					(REALM_TRUMP, 0, 1, rad, dam, GF_PSI));
		}

		case BF_NATURE_DAYL:
		{
			/* Spell -- Day Light */
			rad = (borg_skill[BI_CLEVEL] / 10) + 1 + 10;
			dam = (borg_skill[BI_CLEVEL] / 2);
			return (borg_attack_aux_spell_dispel
					(REALM_NATURE, 0, 4, rad, dam, GF_LITE_WEAK));
		}

		case BF_NATURE_STONEMUD:
		{
			/* Spell -- Stone to Mud */
			dam = (45);
			return (borg_attack_aux_spell_bolt
					(REALM_NATURE, 1, 0, rad, dam, GF_KILL_WALL));
		}

		case BF_NATURE_ELECBOLT:
		{
			/* Spell -- lightning bolt */
			dam = 3 + ((borg_skill[BI_CLEVEL] - 5) / 4);
			return (borg_attack_aux_spell_bolt
					(REALM_NATURE, 1, 1, rad, dam, GF_ELEC));
		}

		case BF_NATURE_FROSTBOLT:
		{
			/* Spell -- frost bolt */
			dam = 5 + ((borg_skill[BI_CLEVEL] - 5) / 4);
			return (borg_attack_aux_spell_bolt
					(REALM_NATURE, 1, 3, rad, dam, GF_COLD));
		}

		case BF_NATURE_SUNL:
		{
			/* Spell -- Sunlight */
			dam = (6 * 8) / 2;
			rad = -1;
			return (borg_attack_aux_spell_bolt
					(REALM_NATURE, 1, 4, rad, dam, GF_LITE_WEAK));
		}

		case BF_NATURE_ENTANGLE:
		{
			/* Spell -- Entangle */
			rad = 10 + MAX_SIGHT;
			dam = (10);
			return (borg_attack_aux_spell_dispel
					(REALM_NATURE, 1, 5, rad, dam, GF_OLD_SLOW));
		}

		case BF_NATURE_WHIRLWIND:
		{
			/* Whirlwind */
			return (borg_attack_aux_nature_whirlwind());
		}

		case BF_NATURE_BLIZZARD:
		{
			/* Blizzard */
			dam = 70 + borg_skill[BI_CLEVEL];
			rad = (borg_skill[BI_CLEVEL] / 12) + 1;
			return (borg_attack_aux_spell_bolt
					(REALM_NATURE, 3, 2, rad, dam, GF_OLD_CONF));
		}

		case BF_NATURE_ELECSTORM:
		{
			/* Elec Storm */
			dam = 90 + borg_skill[BI_CLEVEL];
			rad = (borg_skill[BI_CLEVEL] / 12) + 1;
			return (borg_attack_aux_spell_bolt
					(REALM_NATURE, 3, 3, rad, dam, GF_ELEC));
		}

		case BF_NATURE_WHIRLPOOL:
		{
			/* Whirlpool */
			dam = 100 + borg_skill[BI_CLEVEL];
			rad = (borg_skill[BI_CLEVEL] / 12) + 1;
			return (borg_attack_aux_spell_bolt
					(REALM_NATURE, 3, 4, rad, dam, GF_WATER));
		}

		case BF_NATURE_SUNLIGHT:
		{
			/* Call Sunlight */
			dam = 150;
			rad = 8 + 10;
			if (borg_skill[BI_FEAR_LITE] && borg_skill[BI_CURHP] < 150) dam = 0;
			return (borg_attack_aux_spell_dispel
					(REALM_NATURE, 3, 5, rad, dam, GF_LITE_WEAK));
		}

		case BF_NATURE_NATWRATH:
		{
			/* Natures Wrath */
			dam = ((borg_skill[BI_CLEVEL] + 100));
			rad = (borg_skill[BI_CLEVEL] / 12) + 1 + 10;
			return (borg_attack_aux_spell_dispel
					(REALM_NATURE, 3, 7, rad, dam, GF_DISINTEGRATE));
		}

		case BF_CHAOS_MMISSILE:
		{
			/* Magic Missile */
			dam = (3 + ((borg_skill[BI_CLEVEL] - 1) / 5)) * 2;
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 0, 0, rad, dam, GF_MISSILE));
		}

		case BF_CHAOS_MMISSILE_RESERVE:
		{
			/* Magic Missile */
			dam = (3 + ((borg_skill[BI_CLEVEL] - 1) / 5)) * 2;
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 0, 0, rad, dam, GF_MISSILE));
		}

		case BF_CHAOS_FLASHLIGHT:
		{
			/* Flash of Light */
			rad = (borg_skill[BI_CLEVEL] / 10) + 1 + 10;
			dam = (borg_skill[BI_CLEVEL] / 2);
			return (borg_attack_aux_spell_dispel
					(REALM_CHAOS, 0, 2, rad, dam, GF_LITE_WEAK));
		}

		case BF_CHAOS_MANABURST:
		{
			/* Mana Burst */
			rad = borg_skill[BI_CLEVEL] + (borg_skill[BI_CLEVEL] / 3);
			dam = ((borg_skill[BI_CLEVEL] < 30) ? 2 : 3);
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 0, 4, rad, dam, GF_MANA));
		}

		case BF_CHAOS_FIREBOLT:
		{
			/* Fire Bolt */
			dam = (8 + ((borg_skill[BI_CLEVEL] - 5) / 4)) * 4;
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 0, 2, rad, dam, GF_FIRE));
		}

		case BF_CHAOS_FISTFORCE:
		{
			/* Fist of Force */
			dam = (8 + ((borg_skill[BI_CLEVEL] - 5) / 4)) * 4;
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 0, 6, rad, dam, GF_DISINTEGRATE));
		}

		case BF_CHAOS_CHAOSBOLT:
		{
			/* Chaos Bolt */
			dam = (10 + ((borg_skill[BI_CLEVEL] - 5) / 4)) * 4;
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 1, 1, rad, dam, GF_CHAOS));
		}

		case BF_CHAOS_SONICBOOM:
		{
			/* Sonic Boom */
			dam = (borg_skill[BI_CLEVEL] + 45);
			rad = (borg_skill[BI_CLEVEL] / 10) + 2 + 10;
			return (borg_attack_aux_spell_dispel
					(REALM_CHAOS, 1, 2, rad, dam, GF_SOUND));
		}

		case BF_CHAOS_DOOMBOLT:
		{
			/* Doom Bolt */
			rad = -1;
			dam = (11 + ((borg_skill[BI_CLEVEL] - 5) / 4)) * 4;
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 1, 3, rad, dam, GF_MANA));
		}

		case BF_CHAOS_FIREBALL:
		{
			/* Fire ball */
			rad = 2;
			dam = (borg_skill[BI_CLEVEL] + 55);
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 1, 4, rad, dam, GF_LITE_WEAK));
		}

		case BF_CHAOS_INVOKELOGRUS:
		{
			/* Invoke Lorgrus */
			rad = (borg_skill[BI_CLEVEL] / 5);
			dam = (borg_skill[BI_CLEVEL] + 66);
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 1, 7, rad, dam, GF_CHAOS));
		}

		case BF_CHAOS_POLYMORPH:
		{
			/* Polymorph */
			dam = 10;
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 2, 0, rad, dam, GF_OLD_POLY));
		}

		case BF_CHAOS_CHAINLIGHT:
		{
			/* Chain Lightning */
			rad = 13 + 10;
			dam = (5 + (borg_skill[BI_CLEVEL] / 10)) * 4;
			return (borg_attack_aux_spell_dispel
					(REALM_CHAOS, 2, 1, rad, dam, GF_ELEC));
		}

		case BF_CHAOS_DISINTEG:
		{
			/* Disintegration */
			rad = 3;
			dam = (borg_skill[BI_CLEVEL] + 80);
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 2, 3, rad, dam, GF_DISINTEGRATE));
		}

		case BF_CHAOS_GRAVITY:
		{
			/* Gravity */
			rad = -1;
			dam = (9 + ((borg_skill[BI_CLEVEL] - 5) / 4)) * 4;
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 3, 0, rad, dam, GF_GRAVITY));
		}

		case BF_CHAOS_METEORSWARM:
		{
			/* Meteor Swarm */
			rad = 3;
			dam = (borg_skill[BI_CLEVEL] + 65);
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 3, 1, rad, dam, GF_METEOR));
		}

		case BF_CHAOS_FLAMESTRIKE:
		{
			/* Flamestrike */
			rad = 8 + 10;
			dam = 150 + (borg_skill[BI_CLEVEL] * 2);
			return (borg_attack_aux_spell_dispel
					(REALM_CHAOS, 3, 2, rad, dam, GF_FIRE));
		}

		case BF_CHAOS_ROCKET:
		{
			/* Rocket */
			rad = 2;
			dam = 120 + (borg_skill[BI_CLEVEL]);
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 3, 4, rad, dam, GF_SHARDS));
		}

		case BF_CHAOS_MANASTORM:
		{
			/* Mana Storm */
			rad = 4;
			dam = 300 + (borg_skill[BI_CLEVEL] * 2);
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 3, 5, rad, dam, GF_MANA));
		}

		case BF_CHAOS_BRLOGRUS:
		{
			rad = 2;
			dam = (borg_skill[BI_CURHP]);
			return (borg_attack_aux_spell_bolt
					(REALM_CHAOS, 3, 6, rad, dam, GF_CHAOS));
		}

			/* Call Void */
		case BF_CHAOS_CALLVOID:
		{
			/* Breath Logrus */
			return (borg_attack_aux_spell_callvoid());
		}

		case BF_DEATH_MALEDICTION:
		{
			/* Malediction */
			rad = 0;
			dam = (3 + ((borg_skill[BI_CLEVEL] - 1) / 5)) / 2;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 0, 1, rad, dam, GF_HELL_FIRE));
		}

		case BF_DEATH_MALEDICTION_RESERVE:
		{
			/* Malediction RESERVE */
			rad = 0;
			dam = (3 + ((borg_skill[BI_CLEVEL] - 1) / 5)) / 2;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 0, 1, rad, dam, GF_HELL_FIRE));
		}

		case BF_DEATH_STINKCLOUD:
		{
			/* Poison Ball */
			rad = 2;
			dam = (10 + borg_skill[BI_CLEVEL] / 2);
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 0, 3, rad, dam, GF_POIS));
		}

		case BF_DEATH_SLEEP_I:
		{
			/* Black Sleep */
			dam = 10;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 0, 4, rad, dam, GF_OLD_SLEEP));
		}

		case BF_DEATH_HORRIFY:
		{
			/* Horrify */
			dam = 10;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 0, 6, rad, dam, GF_TURN_ALL));
		}

		case BF_DEATH_ENTROPY:
		{
			/* Entropy */
			rad = 2;
			dam = 10 + borg_skill[BI_CLEVEL] + (borg_skill[BI_CLEVEL] / 2);
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 1, 0, rad, dam, GF_OLD_DRAIN));
		}

		case BF_DEATH_NETHERBOLT:
		{
			/* Nether Bolt */
			dam = (6 + ((borg_skill[BI_CLEVEL] - 5)) / 4);
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 1, 1, rad, dam, GF_HELL_FIRE));
		}

		case BF_DEATH_TERROR:
		{
			/* Terror */
			dam = borg_skill[BI_CLEVEL] + 30;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 1, 2, rad, dam, GF_TURN_ALL));
		}

		case BF_DEATH_VAMPDRAIN:
		{
			/* Vamp Drain */
			dam =
				(borg_skill[BI_CLEVEL] +
				 (borg_skill[BI_CLEVEL] / 2 *
				  (MAX(1, borg_skill[BI_CLEVEL] / 10))));
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 1, 3, rad, dam, GF_OLD_DRAIN));
		}

		case BF_DEATH_DISPELGOOD:
		{
			/* Dispel Good */
			rad = MAX_SIGHT + 10;
			dam = (borg_skill[BI_CLEVEL] * 4) / 2;
			return (borg_attack_aux_spell_dispel
					(REALM_DEATH, 1, 5, rad, dam, GF_DISP_GOOD));
		}

		case BF_DEATH_DARKBOLT:
		{
			/* Dark Bolt */
			dam = (4 + ((borg_skill[BI_CLEVEL] - 5)) / 4);
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 2, 2, rad, dam, GF_DARK));
		}

		case BF_DEATH_VAMPIRISM:
		{
			/* Vampirism True */
			dam = 300;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 2, 4, rad, dam, GF_OLD_DRAIN));
		}

		case BF_DEATH_DARKNESS:
		{
			/* DarknessStorm */
			dam = 120;
			rad = 4;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 2, 6, rad, dam, GF_DARK));
		}

		case BF_DEATH_DEATHRAY:
		{
			/* Death Ray */
			dam = borg_skill[BI_CLEVEL] * 200;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 3, 0, rad, dam, GF_DEATH_RAY));
		}

		case BF_DEATH_WORDOFDEATH:
		{
			/* Word of Death */
			rad = MAX_SIGHT + 10;
			dam = (3 * borg_skill[BI_CLEVEL]) / 2;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 3, 3, rad, dam, GF_OLD_DRAIN));
		}

		case BF_DEATH_EVOCATION:
		{
			/* Evocation */
			dam = borg_skill[BI_CLEVEL] * 2;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 3, 4, rad, dam, GF_OLD_DRAIN));
		}

		case BF_DEATH_HELLFIRE:
		{
			/* HellFire */
			dam = 666;
			if (borg_skill[BI_CURHP] < 200) dam = 0;
			return (borg_attack_aux_spell_bolt
					(REALM_DEATH, 3, 5, rad, dam, GF_OLD_DRAIN));
		}

		case BF_MIND_NEURAL:
		{
			/* Mind-- Neural Blast */
			dam =
				3 + ((borg_skill[BI_CLEVEL] - 1) / 4) * (3 +
														 (borg_skill[BI_CLEVEL]
														  / 15)) / 2;
			rad = -1;
			return (borg_attack_aux_mind_bolt
					(MIND_NEURAL_BL, 2, rad, dam, GF_PSI));
		}

		case BF_MIND_PULVERISE:
		{
			/* Mind-- Pulverise */
			dam = ((8 + ((borg_skill[BI_CLEVEL] - 5) / 4) * 8)) / 2;
			rad = -1;
			return (borg_attack_aux_mind_bolt
					(MIND_PULVERISE, 11, rad, dam, GF_SOUND));
		}

		case BF_MIND_MINDWAVE:
		{
			/* Mind-- Mind Wave */
			if (borg_skill[BI_CLEVEL] < 25)
			{
				dam = (borg_skill[BI_CLEVEL] * 3) / 2;
				rad = 10 + (2 + borg_skill[BI_CLEVEL] / 10);
			}
			else
			{
				dam =
					borg_skill[BI_CLEVEL] * ((borg_skill[BI_CLEVEL] - 5) / 11);
				rad = 10 + MAX_SIGHT;
			}
			return (borg_attack_aux_mind_bolt
					(MIND_MIND_WAVE, 18, rad, dam, GF_PSI));
		}

		case BF_MIND_PSYCH_DRAIN:
		{
			/* Mind-- Psychic Drain */
			dam = ((borg_skill[BI_CLEVEL] / 2) * 6) / 2;
			rad = (borg_skill[BI_CLEVEL] - 25) / 10;
			return (borg_attack_aux_mind_bolt
					(MIND_PSYCHIC_DR, 25, rad, dam, GF_PSI_DRAIN));
		}

		case BF_MIND_TELE_WAVE:
		{
			/* Mind-- Telekinetic Wave */
			if (borg_skill[BI_CLEVEL] < 40)
			{
				dam = (borg_skill[BI_CLEVEL] * 3);
				rad = 10 + 2 + (borg_skill[BI_CLEVEL] / 10);
			}
			else
			{
				dam = borg_skill[BI_CLEVEL] * 4;
				rad = 10 + 2 + (borg_skill[BI_CLEVEL] / 10);
			}
			return (borg_attack_aux_mind_bolt
					(MIND_TELE_WAVE, 28, rad, dam, GF_TELEKINESIS));
		}

		case BF_ROD_SLOW_MONSTER:
		{
			/* Rod -- slow monster */
			dam = 10;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_SLOW_MONSTER, rad, dam, GF_OLD_SLOW));
		}

		case BF_ROD_SLEEP_MONSTER:
		{
			/* Rod -- sleep monster */
			dam = 10;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_SLEEP_MONSTER, rad, dam, GF_OLD_SLEEP));
		}

		case BF_ROD_ELEC_BOLT:
		{
			/* Rod -- elec bolt */
			dam = 3 * (8 + 1) / 2;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_ELEC_BOLT, rad, dam, GF_ELEC));
		}

		case BF_ROD_COLD_BOLT:
		{
			/* Rod -- cold bolt */
			dam = 5 * (8 + 1) / 2;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_COLD_BOLT, rad, dam, GF_COLD));
		}

		case BF_ROD_ACID_BOLT:
		{
			/* Rod -- acid bolt */
			dam = 6 * (8 + 1) / 2;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_ACID_BOLT, rad, dam, GF_ACID));
		}

		case BF_ROD_FIRE_BOLT:
		{
			/* Rod -- fire bolt */
			dam = 8 * (8 + 1) / 2;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_FIRE_BOLT, rad, dam, GF_FIRE));
		}

		case BF_ROD_LITE_BEAM:
		{
			/* Spell -- light beam */
			rad = -1;
			dam = (6 * (8 + 1) / 2);
			return (borg_attack_aux_rod_bolt
					(SV_ROD_LITE, rad, dam, GF_LITE_WEAK));
		}

		case BF_ROD_DRAIN_LIFE:
		{
			/* Spell -- drain life */
			dam = (75);
			return (borg_attack_aux_rod_bolt
					(SV_ROD_DRAIN_LIFE, rad, dam, GF_OLD_DRAIN));
		}

		case BF_ROD_ELEC_BALL:
		{
			/* Rod -- elec ball */
			rad = 2;
			dam = 32;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_ELEC_BALL, rad, dam, GF_ELEC));
		}

		case BF_ROD_COLD_BALL:
		{
			/* Rod -- acid ball */
			rad = 2;
			dam = 48;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_COLD_BALL, rad, dam, GF_COLD));
		}

		case BF_ROD_ACID_BALL:
		{
			/* Rod -- acid ball */
			rad = 2;
			dam = 60;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_ACID_BALL, rad, dam, GF_ACID));
		}

		case BF_ROD_FIRE_BALL:
		{
			/* Rod -- fire ball */
			rad = 2;
			dam = 72;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_FIRE_BALL, rad, dam, GF_FIRE));
		}

		case BF_ROD_PESTICIDE:
		{
			/* Rod -- Pesticide */
			rad = 3;
			dam = 8;
			return (borg_attack_aux_rod_bolt
					(SV_ROD_PESTICIDE, rad, dam, GF_POIS));
		}

		case BF_WAND_ROCKETS:
		{
			/* Wand -- Rocket */
			dam = 250;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_ROCKETS, rad, dam, GF_ROCKET));
		}

		case BF_WAND_MAGIC_MISSILE:
		{
			/* Wand -- magic missile */
			dam = 2 * (6 + 1) / 2;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_MAGIC_MISSILE, rad, dam, GF_MISSILE));
		}

		case BF_WAND_SLOW_MONSTER:
		{
			/* Wand -- slow monster */
			dam = 10;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_SLOW_MONSTER, rad, dam, GF_OLD_SLOW));
		}

		case BF_WAND_SLEEP_MONSTER:
		{
			/* Wand -- sleep monster */
			dam = 10;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_SLEEP_MONSTER, rad, dam, GF_OLD_SLEEP));
		}

		case BF_WAND_FEAR_MONSTER:
		{
			/* Wand -- fear monster */
			dam = 2 * (6 + 1) / 2;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_FEAR_MONSTER, rad, dam, GF_TURN_ALL));
		}

		case BF_WAND_CONFUSE_MONSTER:
		{
			/* Wand -- conf monster */
			dam = 2 * (6 + 1) / 2;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_CONFUSE_MONSTER, rad, dam, GF_OLD_CONF));
		}

		case BF_WAND_COLD_BOLT:
		{
			/* Wand -- cold bolt */
			dam = 3 * (8 + 1) / 2;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_COLD_BOLT, rad, dam, GF_COLD));
		}

		case BF_WAND_ACID_BOLT:
		{
			/* Wand -- acid bolt */
			dam = 5 * (8 + 1) / 2;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_ACID_BOLT, rad, dam, GF_ACID));
		}

		case BF_WAND_FIRE_BOLT:
		{
			/* Wand -- fire bolt */
			dam = 6 * (8 + 1) / 2;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_FIRE_BOLT, rad, dam, GF_FIRE));
		}

		case BF_WAND_LITE_BEAM:
		{
			/* Spell -- light beam */
			rad = -1;
			dam = (6 * (8 + 1) / 2);
			return (borg_attack_aux_wand_bolt
					(SV_WAND_LITE, rad, dam, GF_LITE_WEAK));
		}

		case BF_WAND_STINKING_CLOUD:
		{
			/* Wand -- stinking cloud */
			rad = 2;
			dam = 12;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_STINKING_CLOUD, rad, dam, GF_POIS));
		}

		case BF_WAND_ELEC_BALL:
		{
			/* Wand -- elec ball */
			rad = 2;
			dam = 32;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_ELEC_BALL, rad, dam, GF_ELEC));
		}

		case BF_WAND_COLD_BALL:
		{
			/* Wand -- acid ball */
			rad = 2;
			dam = 48;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_COLD_BALL, rad, dam, GF_COLD));
		}

		case BF_WAND_ACID_BALL:
		{
			/* Wand -- acid ball */
			rad = 2;
			dam = 60;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_ACID_BALL, rad, dam, GF_ACID));
		}

		case BF_WAND_FIRE_BALL:
		{
			/* Wand -- fire ball */
			rad = 2;
			dam = 72;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_FIRE_BALL, rad, dam, GF_FIRE));
		}

		case BF_WAND_DRAGON_COLD:
		{
			/* Wand -- dragon cold */
			rad = 3;
			dam = 80;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_DRAGON_COLD, rad, dam, GF_COLD));
		}

		case BF_WAND_DRAGON_FIRE:
		{
			/* Wand -- dragon fire */
			rad = 3;
			dam = 100;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_DRAGON_FIRE, rad, dam, GF_FIRE));
		}

		case BF_WAND_ANNIHILATION:
		{
			/* Wand -- annihilation */
			dam = 125;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_ANNIHILATION, rad, dam, GF_OLD_DRAIN));
		}

		case BF_WAND_DRAIN_LIFE:
		{
			/* Wand -- drain life */
			dam = 75;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_DRAIN_LIFE, rad, dam, GF_OLD_DRAIN));
		}

		case BF_WAND_WONDER:
		{
			/* Wand -- wand of wonder */
			dam = 35;
			return (borg_attack_aux_wand_bolt
					(SV_WAND_WONDER, rad, dam, GF_MISSILE));
		}

		case BF_STAFF_SLEEP_MONSTERS:
		{
			/* Staff -- Sleep Monsters */
			dam = 60;
			rad = MAX_SIGHT + 10;
			return (borg_attack_aux_staff_dispel
					(SV_STAFF_SLEEP_MONSTERS, rad, dam, GF_OLD_SLEEP));
		}

		case BF_STAFF_SLOW_MONSTERS:
		{
			/* Staff -- Slow Monsters */
			dam = 60;
			rad = MAX_SIGHT + 10;
			return (borg_attack_aux_staff_dispel
					(SV_STAFF_SLOW_MONSTERS, rad, dam, GF_OLD_SLOW));
		}

		case BF_STAFF_DISPEL_EVIL:
		{
			/* Staff -- Dispel Evil */
			dam = 60;
			rad = MAX_SIGHT + 10;
			return (borg_attack_aux_staff_dispel
					(SV_STAFF_DISPEL_EVIL, rad, dam, GF_DISP_EVIL));
		}

		case BF_STAFF_POWER:
		{
			/* Staff -- Power */
			dam = 120;
			rad = MAX_SIGHT + 10;
			return (borg_attack_aux_staff_dispel
					(SV_STAFF_POWER, rad, dam, GF_TURN_ALL));
		}

		case BF_STAFF_HOLINESS:
		{
			/* Staff -- holiness */
			rad = MAX_SIGHT + 10;
			if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2) dam = 500;
			else
				dam = 120;
			return (borg_attack_aux_staff_dispel
					(SV_STAFF_HOLINESS, rad, dam, GF_DISP_EVIL));
		}


		case BF_DRAGON_BLUE:
		{
			rad = 2;
			dam = 100;
			return (borg_attack_aux_dragon(SV_DRAGON_BLUE, rad, dam, GF_ELEC));
		}

		case BF_DRAGON_WHITE:
		{
			rad = 2;
			dam = 110;
			return (borg_attack_aux_dragon(SV_DRAGON_WHITE, rad, dam, GF_COLD));
		}

		case BF_DRAGON_BLACK:
		{
			rad = 2;
			dam = 130;
			return (borg_attack_aux_dragon(SV_DRAGON_BLACK, rad, dam, GF_ACID));
		}

		case BF_DRAGON_GREEN:
		{
			rad = 2;
			dam = 150;
			return (borg_attack_aux_dragon(SV_DRAGON_GREEN, rad, dam, GF_POIS));
		}

		case BF_DRAGON_RED:
		{
			rad = 2;
			dam = 200;
			return (borg_attack_aux_dragon(SV_DRAGON_RED, rad, dam, GF_FIRE));
		}

		case BF_DRAGON_MULTIHUED:
		{
			chance = randint0(5);
			rad = 2;
			dam = 200;
			return (borg_attack_aux_dragon(SV_DRAGON_MULTIHUED, rad, dam,
										   (((chance == 1) ? GF_ELEC :
											 ((chance == 2) ? GF_COLD :
											  ((chance == 3) ? GF_ACID :
											   ((chance ==
												 4) ? GF_POIS : GF_FIRE)))))));
		}

		case BF_DRAGON_BRONZE:
		{
			rad = 2;
			dam = 120;
			return (borg_attack_aux_dragon
					(SV_DRAGON_BRONZE, rad, dam, GF_CONFUSION));
		}

		case BF_DRAGON_GOLD:
		{
			rad = 2;
			dam = 150;
			return (borg_attack_aux_dragon(SV_DRAGON_GOLD, rad, dam, GF_SOUND));
		}

		case BF_DRAGON_CHAOS:
		{
			chance = randint0(2);
			rad = 2;
			dam = 220;
			return (borg_attack_aux_dragon(SV_DRAGON_CHAOS, rad, dam,
										   (chance ==
											1 ? GF_CHAOS : GF_DISENCHANT)));
		}

		case BF_DRAGON_LAW:
		{
			chance = randint0(2);
			rad = 2;
			dam = 230;
			return (borg_attack_aux_dragon(SV_DRAGON_LAW, rad, dam,
										   (chance ==
											1 ? GF_SOUND : GF_SHARDS)));
		}

		case BF_DRAGON_BALANCE:
		{
			chance = randint0(4);
			rad = 2;
			dam = 230;
			return (borg_attack_aux_dragon(SV_DRAGON_BALANCE, rad, dam,
										   (((chance == 1) ? GF_CHAOS :
											 ((chance == 2) ? GF_DISENCHANT :
											  ((chance ==
												3) ? GF_SOUND : GF_SHARDS))))));
		}

		case BF_DRAGON_SHINING:
		{
			chance = randint0(2);
			rad = 2;
			dam = 200;
			return (borg_attack_aux_dragon(SV_DRAGON_SHINING, rad, dam,
										   (chance == 0 ? GF_LITE : GF_DARK)));
		}

		case BF_DRAGON_POWER:
		{
			rad = 2;
			dam = 300;
			return (borg_attack_aux_dragon
					(SV_DRAGON_POWER, rad, dam, GF_MISSILE));
		}

		case BF_RACIAL_VAMP:
		{
			/* Suck Blood */
			dam = borg_skill[BI_CLEVEL] + ((borg_skill[BI_CLEVEL] / 2) * MAX(1, borg_skill[BI_CLEVEL] / 10));	/* Dmg */
			return (borg_attack_aux_racial_thrust(RACE_VAMPIRE, 2, dam));
		}

		case BF_RACIAL_CYCLOPS:
		{
			/* Throw Boulder */
			dam = borg_skill[BI_CLEVEL] * 3 / 2;	/* Dmg */
			return (borg_attack_aux_racial_bolt
					(RACE_CYCLOPS, rad, dam, GF_MISSILE));
		}

		case BF_RACIAL_DARK_ELF:
		{
			/* Magic Missile */
			dam = (3 + ((borg_skill[BI_CLEVEL] - 1)) / 4) * 5;
			return (borg_attack_aux_racial_bolt
					(RACE_DARK_ELF, rad, dam, GF_MISSILE));
		}

		case BF_RACIAL_DRACONIAN:
		{
			/* Draconian Breath */
			dam = 2 * borg_skill[BI_CLEVEL];
			rad = 1 + borg_skill[BI_CLEVEL] / 15;
			return (borg_attack_aux_racial_bolt
					(RACE_DRACONIAN, rad, dam, GF_FIRE));
		}

		case BF_RACIAL_IMP:
		{
			/* Fireball */
			dam = borg_skill[BI_CLEVEL];
			rad = (borg_skill[BI_CLEVEL] >= 30) ? 2 : 0;
			return (borg_attack_aux_racial_bolt(RACE_IMP, rad, dam, GF_FIRE));
		}

		case BF_RACIAL_KLACKON:
		{
			/* Acidball */
			dam = borg_skill[BI_CLEVEL];
			rad = (borg_skill[BI_CLEVEL] >= 25) ? 2 : 0;
			return (borg_attack_aux_racial_bolt
					(RACE_KLACKON, rad, dam, GF_ACID));
		}

		case BF_RACIAL_KOBOLD:
		{
			/* Poison bolt */
			dam = borg_skill[BI_CLEVEL];
			return (borg_attack_aux_racial_bolt
					(RACE_KOBOLD, rad, dam, GF_POIS));
		}

		case BF_RACIAL_MINDFLAYER:
		{
			/* Mindblast bolt */
			dam = borg_skill[BI_CLEVEL];
			return (borg_attack_aux_racial_bolt
					(RACE_MIND_FLAYER, rad, dam, GF_PSI));
		}

		case BF_RACIAL_SPRITE:
		{
			/* Sleep III */
			dam = borg_skill[BI_CLEVEL];
			rad = 10 + MAX_RANGE;
			return (borg_attack_aux_racial_bolt
					(RACE_SPRITE, rad, dam, GF_OLD_SLEEP));
		}

		case BF_RACIAL_YEEK:
		{
			/* Scare Mon */
			dam = borg_skill[BI_CLEVEL];
			return (borg_attack_aux_racial_bolt
					(RACE_YEEK, rad, dam, GF_OLD_SLEEP));
		}

	}


	/* Oops */
	return (0);
}


/*
 * Attack nearby monsters, in the best possible way, if any.
 *
 * We consider a variety of possible attacks, including physical attacks
 * on adjacent monsters, missile attacks on nearby monsters, spell/prayer
 * attacks on nearby monsters, and wand/rod attacks on nearby monsters.
 *
 * Basically, for each of the known "types" of attack, we "simulate" the
 * "optimal" result of using that attack, and then we "apply" the "type"
 * of attack which appears to have the "optimal" result.
 *
 * When calculating the "result" of using an attack, we only consider the
 * effect of the attack on visible, on-screen, known monsters, which are
 * within 16 grids of the player.  This prevents most "spurious" attacks,
 * but we can still be fooled by situations like creeping coins which die
 * while out of sight, leaving behind a pile of coins, which we then find
 * again, and attack with distance attacks, which have no effect.  Perhaps
 * we should "expect" certain results, and take note of failure to observe
 * those effects.  XXX XXX XXX
 *
 * See above for the "semantics" of each "type" of attack.
 */
bool borg_attack(bool boosted_bravery)
{
	int i, x, y;

	int n, b_n = 0;
	int g, b_g = -1;

	map_block *mb_ptr;

	/* Nobody around */
	if (!borg_kills_cnt) return (FALSE);

	/* Set the attacking flag so that danger is boosted for monsters */
	/* we want to attack first. */
	borg_attacking = TRUE;

	/* Reset list */
	borg_temp_n = 0;

	/* Find "nearby" monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{

		borg_kill *kill;

		/* Monster */
		kill = &borg_kills[i];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Require current knowledge */
		if (kill->when < borg_t) continue;

		/* Ignore multiplying monsters and when fleeing from scaries */
		if (goal_ignoring && !borg_skill[BI_ISAFRAID] &&
			(r_info[kill->r_idx].flags2 & RF2_MULTIPLY)) continue;

		/* no attacking most scaryguys, try to get off the level */
		if (scaryguy_on_level)
		{

			/* probably Grip or Fang. */
			if (borg_skill[BI_CDEPTH] <= 5 && borg_skill[BI_CDEPTH] != 0 &&
				borg_fighting_unique)
			{
				/* Try to fight Grip and Fang. */
			}
			else if (boosted_bravery)
			{
				/* Try to fight if being Boosted */
			}
			else
			{
				/* Flee from other scary guys */
				continue;
			}

		}

		/* Acquire location */
		x = kill->x;
		y = kill->y;

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Get grid */
		mb_ptr = map_loc(x, y);

		/* Never shoot through walls */
		if (!(mb_ptr->info & BORG_MAP_VIEW)) continue;

		/* Check the distance XXX XXX XXX */
		if (distance(c_y, c_x, y, x) > 16) continue;

		/* Save the location (careful) */
		borg_temp_x[borg_temp_n] = x;
		borg_temp_y[borg_temp_n] = y;
		borg_temp_n++;
	}

	/* No destinations */
	if (!borg_temp_n)
	{
		borg_attacking = FALSE;
		return (FALSE);
	}

	/* Simulate */
	borg_simulate = TRUE;

	/* Analyze the possible attacks */
	for (g = 0; g < BF_MAX; g++)
	{
		/* Simulate */
		n = borg_attack_aux(g);

		/* Track "best" attack  <= */
		if (n <= b_n) continue;

		/* Track best */
		b_g = g;
		b_n = n;
	}

	/* Nothing good */
	if (b_n <= 0)
	{
		borg_attacking = FALSE;
		return (FALSE);
	}


	/* Note */
	borg_note(format("# Performing attack type %d with value %d.", b_g, b_n));

	/* Instantiate */
	borg_simulate = FALSE;

	/* Instantiate */
	(void)borg_attack_aux(b_g);

	borg_attacking = FALSE;

	/* Success */
	return (TRUE);
}

/*
 *
 * There are several types of setup moves:
 *
 *   Temporary speed
 *   Protect From Evil
 *   Bless\Prayer
 *   Berserk\Heroism
 *   Temp Resist (either all or just cold/fire?)
 *   Shield
 *   Teleport away
 *   Glyph of Warding
 *   See inviso
 *
 * * and many others
 *
 */
enum
{
	BD_SPEED,
	BD_PROT_FROM_EVIL,
	BD_BLESS,
	BD_BERSERK,
	BD_HERO,
	BD_RESIST_FCE,
	BD_RESIST_FECAP,
	BD_RESIST_F,
	BD_RESIST_C,
	BD_RESIST_A,
	BD_RESIST_E,
	BD_RESIST_P,
	BD_SHIELD,
	BD_GOI,
	BD_GOI_POT,
	BD_GLYPH,
	BD_WARDING,
	BD_TELL_AWAY,
	BD_CREATE_WALLS,
	BD_MASS_GENOCIDE,
	BD_GENOCIDE,
	BD_GENOCIDE_HOUNDS,
	BD_EARTHQUAKE,
	BD_DESTRUCTION,
	BD_BANISHMENT,
	BD_DETECT_INVISO,
	BD_LIGHT_BEAM,
	BD_SHIFT_PANEL,
	BD_TRUMP_SERVANT,

	BD_MAX
};

/*
 * Bless/Prayer to prepare for battle
 */
static int borg_defend_aux_bless(int p1)
{
	int fail_allowed = 10;
	map_block *mb_ptr = map_loc(c_x, c_y);

	/* already blessed */
	if (borg_bless)
		return (0);

	/* Cant when Blind */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (0);

	/* Dark */
	if (!(mb_ptr->flags & MAP_GLOW) && borg_skill[BI_CUR_LITE] == 0) return (0);


	/* no spell */
	if (!borg_spell_okay_fail(REALM_LIFE, 0, 2, fail_allowed) &&
		!borg_spell_okay_fail(REALM_LIFE, 3, 1, fail_allowed) &&
		-1 == borg_slot(TV_SCROLL, SV_SCROLL_BLESSING) &&
		-1 == borg_slot(TV_SCROLL, SV_SCROLL_HOLY_CHANT) &&
		-1 == borg_slot(TV_SCROLL, SV_SCROLL_HOLY_PRAYER))
		return (0);

	/* if we are in some danger but not much, go for a quick bless */
	if (p1 > avoidance / 12 && p1 < avoidance / 2)
	{
		/* Simulation */
		/* bless is a low priority */
		if (borg_simulate) return (1);

		/* do it! */
		if (borg_spell(REALM_LIFE, 0, 2) || borg_spell(REALM_LIFE, 3, 1) ||
			borg_read_scroll(SV_SCROLL_BLESSING) ||
			borg_read_scroll(SV_SCROLL_HOLY_CHANT) ||
			borg_read_scroll(SV_SCROLL_HOLY_PRAYER))
			return 1;
	}

	return (0);
}

/*
 * Speed to prepare for battle
 */
static int borg_defend_aux_speed(int p1)
{
	int p2 = 0;
	bool good_speed = FALSE;
	bool speed_spell = FALSE;
	bool speed_staff = FALSE;
	bool speed_rod = FALSE;
	int fail_allowed = 39;

	/* already fast */
	if (borg_speed)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;

	/* only cast defence spells if fail rate is not too high */
	if (borg_spell_okay_fail(REALM_SORCERY, 1, 5, fail_allowed) ||
		borg_spell_okay_fail(REALM_DEATH, 2, 3, fail_allowed) ||
		borg_mindcr_okay_fail(MIND_ADRENALINE, 35, fail_allowed))
		speed_spell = TRUE;

	/* staff must have charges */
	if (borg_equips_staff_fail(SV_STAFF_SPEED))
		speed_staff = TRUE;

	/* rod can't be charging */
	if (borg_equips_rod(SV_ROD_SPEED))
		speed_rod = TRUE;

	if (0 > borg_slot(TV_POTION, SV_POTION_SPEED) && !speed_staff && !speed_rod && !speed_spell	/*&&
																								   !borg_equips_artifact(ART_FEANOR, INVEN_FEET) &&
																								   !borg_equips_artifact(ART_TARATOL, INVEN_WIELD) &&
																								   !borg_equips_artifact(ART_TULKAS, INVEN_LEFT) */ )
		return (0);

	/* if we have an infinite/large suppy of speed we can */
	/* be generious with our use */
	if (speed_rod || speed_spell || speed_staff	/*||
												   borg_equips_artifact(ART_TULKAS, INVEN_RIGHT) ||
												   borg_equips_artifact(ART_FEANOR, INVEN_FEET) ||
												   borg_equips_artifact(ART_TARATOL, INVEN_WIELD) */ )
		good_speed = TRUE;

	/* pretend we are protected and look again */
	borg_speed = TRUE;
	p2 = borg_danger(c_y, c_x, 1, TRUE);
	borg_speed = FALSE;

	/* if we are fighting a unique cast it. */
	if (good_speed && borg_fighting_unique)
	{
		/* HACK pretend that it was scary and will be safer */
		p2 = p2 * 7 / 10;
	}
	/* if the unique is Sauron cast it */
	if (borg_skill[BI_CDEPTH] == 99 && borg_fighting_unique >= 10)
	{
		p2 = p2 * 6 / 10;
	}

	/* if the unique is Morgoth cast it */
	if (borg_skill[BI_CDEPTH] == 100 && borg_fighting_unique >= 10)
	{
		p2 = p2 * 5 / 10;
	}

	/* Attempt to conserve Speed at end of game */
	if (borg_skill[BI_CDEPTH] >= 97 && !borg_fighting_unique &&
		!good_speed) p2 = 9999;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (((p1 > p2) &&
		 p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		 && (p1 > (avoidance / 5)) && good_speed) || ((p1 > p2) &&
													  p2 <=
													  (borg_fighting_unique
													   ? ((avoidance * 2) /
														  3) : (avoidance / 3))
													  && (p1 >
														  (avoidance / 7))))
	{
		/* if we just did GOI do a speed right after. */
		if (good_speed && borg_goi)
		{
			/* HACK pretend that it was scary and will be very safe */
			/* This is done because GOI messes up our calculations */
			p1 = 10000;
			p2 = 1;
		}

		/* Simulation */
		if (borg_simulate) return (p1 - p2 + (borg_goi / 100) * 50);

		/* do it! */
		if (borg_spell_fail(REALM_SORCERY, 1, 5, fail_allowed) ||
			borg_spell_fail(REALM_DEATH, 2, 3, fail_allowed) ||
			borg_mindcr_fail(MIND_ADRENALINE, 35, fail_allowed))
			return (p1 - p2);

		if (borg_zap_rod(SV_ROD_SPEED)	/*||
										   borg_activate_artifact(ART_FEANOR, FALSE) ||
										   borg_activate_artifact(ART_TARATOL, FALSE) ||
										   borg_activate_artifact(ART_TULKAS, FALSE) */  ||
			borg_use_staff(SV_STAFF_SPEED) ||
			borg_quaff_potion(SV_POTION_SPEED))

			/* Value */
			return (p1 - p2 + borg_goi * 50);
	}
	/* default to can't do it. */
	return (0);
}

/*
 * Globe of Invulnurability
 */
static int borg_defend_aux_goi(int p1)
{
	int p2 = 0;
	int fail_allowed = 39;

	if (borg_goi)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* a bit scary */
	if (p1 > (avoidance / 2))
		fail_allowed -= 5;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 4)
		fail_allowed += 10;

	/* If fighting regular unique boost the fail rate */
	if (borg_fighting_unique >= 1)
		fail_allowed = 25;

	/* If fighting Questor boost the fail rate */
	if (borg_fighting_unique >= 11 || borg_skill[BI_DEPTH] == 100)
		fail_allowed = 33;

	if (!borg_spell_okay_fail(REALM_SORCERY, 3, 7, fail_allowed) &&
		!borg_spell_okay_fail(REALM_LIFE, 3, 7, fail_allowed))
		return (0);

	/* pretend we are protected and look again */
	borg_goi = 100;
	p2 = borg_danger(c_y, c_x, 1, TRUE);
	borg_goi = 0;

	/*  if we are fighting a unique enhance the value by reducing p2 */
	if (borg_fighting_unique)
	{
		p2 = p2 / 2;
	}

	/* if the unique is Sauron cast it */
	if (borg_skill[BI_CDEPTH] == 99 && borg_fighting_unique >= 10)
	{
		p2 = p2 * 4 / 10;
	}

	/* if the unique is Morgoth cast it */
	if (borg_skill[BI_CDEPTH] == 100 && borg_fighting_unique >= 10)
	{
		p2 = 0;
	}

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_spell_fail(REALM_SORCERY, 3, 7, fail_allowed) ||
			borg_spell_fail(REALM_LIFE, 3, 7, fail_allowed))
			return (p1 - p2);

	}

	/* default to can't do it. */
	return (0);
}

/*
 * Globe of Invulnurability Potion
 */
static int borg_defend_aux_goi_pot(int p1)
{
	int p2 = 0;

	if (borg_goi)
		return (0);

	/* Save for fighting uniques */
	if (!borg_fighting_unique)
		return (0);

	/* have some in inven? */
	if (borg_has[238] == 0) return (0);

	/* pretend we are protected and look again */
	borg_goi = 100;
	p2 = borg_danger(c_y, c_x, 1, TRUE);
	borg_goi = 0;

	/*  Fighting a unique, enhance the value by reducing p2 */
	p2 = p2 / 2;

	/* if the unique is Sauron cast it */
	if (borg_skill[BI_CDEPTH] == 99 && borg_fighting_unique >= 10)
	{
		p2 = p2 * 4 / 10;
	}

	/* if the unique is Morgoth cast it */
	if (borg_skill[BI_CDEPTH] == 100 && borg_fighting_unique >= 10)
	{
		p2 = 0;
	}

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_quaff_potion(SV_POTION_INVULNERABILITY))
			return (p1 - p2);

	}

	/* default to can't do it. */
	return (0);
}

/* cold/fire */
static int borg_defend_aux_resist_fce(int p1)
{
	int p2 = 0;
	int fail_allowed = 39;
	bool save_fire, save_cold, save_elec;

	if (my_oppose_fire && my_oppose_cold && my_oppose_elec)
		return (0);

#if 0
	if (borg_skill[BI_RFIRE] && borg_skill[BI_RCOLD])
		return (0);
#endif

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;

	if (!borg_spell_okay_fail(REALM_NATURE, 0, 6, fail_allowed) &&
		!borg_equips_artifact(ART_COLLUIN, INVEN_OUTER))
		return (0);

	/* pretend we are protected and look again */
	save_fire = my_oppose_fire;
	save_cold = my_oppose_cold;
	save_elec = my_oppose_elec;

	my_oppose_fire = TRUE;
	my_oppose_cold = TRUE;
	my_oppose_elec = TRUE;
	p2 = borg_danger(c_y, c_x, 1, FALSE);
	my_oppose_fire = save_fire;
	my_oppose_cold = save_cold;
	my_oppose_elec = save_elec;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_activate_artifact(ART_COLLUIN, FALSE) ||
			borg_spell_fail(REALM_NATURE, 0, 6, fail_allowed))

			/* Value */
			return (p1 - p2);
	}

	/* default to can't do it. */
	return (0);
}

/* all resists */
static int borg_defend_aux_resist_fecap(int p1)
{
	int p2 = 0;
	int fail_allowed = 39;
	bool save_fire, save_acid, save_poison, save_elec, save_cold;

	if (my_oppose_fire &&
		my_oppose_acid && my_oppose_pois && my_oppose_elec && my_oppose_cold)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;
/* How about adding the potion of Resistance?
 * Won't want to cast it though if only one element is
 * down.  Ought to at least wait until 3 of the 4 are down.
 */
	if (!borg_spell_okay_fail(REALM_NATURE, 2, 3, fail_allowed) &&
		!borg_equips_artifact(ART_COLLUIN, INVEN_OUTER) &&
		!borg_mindcr_okay_fail(MIND_CHAR_ARMOUR, 35, fail_allowed))
		return (0);

	/* pretend we are protected and look again */
	save_fire = my_oppose_fire;
	save_acid = my_oppose_acid;
	save_poison = my_oppose_pois;
	save_elec = my_oppose_elec;
	save_cold = my_oppose_cold;
	my_oppose_fire = TRUE;
	my_oppose_cold = TRUE;
	my_oppose_acid = TRUE;
	my_oppose_pois = TRUE;
	my_oppose_elec = TRUE;
	p2 = borg_danger(c_y, c_x, 1, FALSE);
	my_oppose_fire = save_fire;
	my_oppose_cold = save_cold;
	my_oppose_acid = save_acid;
	my_oppose_pois = save_poison;
	my_oppose_elec = save_elec;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return ((p1 - p2) - 1);

		/* do it! */
		if (borg_activate_artifact(ART_COLLUIN, FALSE) ||
			borg_spell_fail(REALM_NATURE, 2, 3, fail_allowed) ||
			borg_mindcr_fail(MIND_CHAR_ARMOUR, 35, fail_allowed))

			/* Value */
			return ((p1 - p2) - 1);
	}

	/* default to can't do it. */
	return (0);
}

/* fire */
static int borg_defend_aux_resist_f(int p1)
{

	int p2 = 0;
	int fail_allowed = 39;
	bool save_fire;

	if (my_oppose_fire)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;

	if (!borg_spell_okay_fail(REALM_ARCANE, 1, 6, fail_allowed) &&
		!borg_mindcr_okay_fail(MIND_CHAR_ARMOUR, 20, fail_allowed) &&
		!borg_equips_artifact(ART_COLLUIN, INVEN_OUTER) &&
		-1 == borg_slot(TV_POTION, SV_POTION_RESIST_HEAT))
		return (0);

	save_fire = my_oppose_fire;
	/* pretend we are protected and look again */
	my_oppose_fire = TRUE;
	p2 = borg_danger(c_y, c_x, 1, FALSE);
	my_oppose_fire = save_fire;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_activate_artifact(ART_COLLUIN, FALSE) ||
			borg_spell_fail(REALM_SORCERY, 1, 6, fail_allowed) ||
			borg_mindcr_fail(MIND_CHAR_ARMOUR, 20, fail_allowed) ||
			borg_quaff_potion(SV_POTION_RESIST_HEAT))

			/* Value */
			return (p1 - p2);
	}

	/* default to can't do it. */
	return (0);
}

 /* cold */
static int borg_defend_aux_resist_c(int p1)
{

	int p2 = 0;
	int fail_allowed = 39;
	bool save_cold;

	if (my_oppose_cold)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;

	if (!borg_spell_okay_fail(REALM_NATURE, 1, 7, fail_allowed) &&
		!borg_equips_artifact(ART_COLLUIN, INVEN_OUTER) &&
		!borg_mindcr_okay_fail(MIND_CHAR_ARMOUR, 25, fail_allowed) &&
		-1 == borg_slot(TV_POTION, SV_POTION_RESIST_COLD))
		return (0);

	save_cold = my_oppose_cold;
	/* pretend we are protected and look again */
	my_oppose_cold = TRUE;
	p2 = borg_danger(c_y, c_x, 1, FALSE);
	my_oppose_cold = save_cold;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_activate_artifact(ART_COLLUIN, FALSE) ||
			borg_spell_fail(REALM_NATURE, 1, 7, fail_allowed) ||
			borg_mindcr_fail(MIND_CHAR_ARMOUR, 25, fail_allowed) ||
			borg_quaff_potion(SV_POTION_RESIST_COLD))

			/* Value */
			return (p1 - p2);
	}

	/* default to can't do it. */
	return (0);
}

/* acid */
static int borg_defend_aux_resist_a(int p1)
{

	int p2 = 0;
	int fail_allowed = 39;
	bool save_acid;

	if (my_oppose_acid)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;

	if (!borg_spell_okay_fail(REALM_NATURE, 2, 1, fail_allowed) &&
		!borg_mindcr_okay_fail(MIND_CHAR_ARMOUR, 15, fail_allowed) &&
		!borg_equips_artifact(ART_COLLUIN, INVEN_OUTER))
		return (0);

	save_acid = my_oppose_acid;
	/* pretend we are protected and look again */
	my_oppose_acid = TRUE;
	p2 = borg_danger(c_y, c_x, 1, FALSE);
	my_oppose_acid = save_acid;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_activate_artifact(ART_COLLUIN, FALSE) ||
			borg_mindcr_fail(MIND_CHAR_ARMOUR, 15, fail_allowed) ||
			borg_spell_fail(REALM_NATURE, 2, 1, fail_allowed))

			/* Value */
			return (p1 - p2);
	}
	/* default to can't do it. */
	return (0);
}

/* poison */
static int borg_defend_aux_resist_p(int p1)
{
	int p2 = 0;
	int fail_allowed = 39;
	bool save_poison;

	if (my_oppose_pois)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;

	if (!borg_spell_okay_fail(REALM_DEATH, 0, 5, fail_allowed) &&
		!borg_equips_artifact(ART_COLLUIN, INVEN_OUTER))
		return (0);

	save_poison = my_oppose_pois;
	/* pretend we are protected and look again */
	my_oppose_pois = TRUE;
	p2 = borg_danger(c_y, c_x, 1, FALSE);
	my_oppose_pois = save_poison;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_activate_artifact(ART_COLLUIN, FALSE) ||
			borg_spell_fail(REALM_DEATH, 0, 5, fail_allowed))

			/* Value */
			return (p1 - p2);
	}

	/* default to can't do it. */
	return (0);
}

static int borg_defend_aux_prot_evil(int p1)
{
	int p2 = 0;
	int fail_allowed = 39;
	bool pfe_spell = FALSE;

	map_block *mb_ptr = map_loc(c_x, c_y);


	/* if already protected */
	if (borg_prot_from_evil)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 5;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;

	if (borg_spell_okay_fail(REALM_LIFE, 1, 5, fail_allowed)) pfe_spell = TRUE;

	if (0 <= borg_slot(TV_SCROLL, SV_SCROLL_PROTECTION_FROM_EVIL)) pfe_spell =
			TRUE;

	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISIMAGE])
		pfe_spell = FALSE;
	if (!(mb_ptr->flags & MAP_GLOW) &&
		borg_skill[BI_CUR_LITE] == 0) pfe_spell = FALSE;

	if (borg_equips_artifact(ART_CARLAMMAS, INVEN_NECK)) pfe_spell = TRUE;

	if (!pfe_spell) return (0);


	/* pretend we are protected and look again */
	borg_prot_from_evil = TRUE;
	p2 = borg_danger(c_y, c_x, 1, FALSE);
	borg_prot_from_evil = FALSE;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */

	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_spell_fail(REALM_LIFE, 1, 5, fail_allowed) ||
			borg_activate_artifact(ART_CARLAMMAS, FALSE) ||
			borg_read_scroll(SV_SCROLL_PROTECTION_FROM_EVIL))

			/* Value */
			return (p1 - p2);
	}

	/* default to can't do it. */
	return (0);
}

static int borg_defend_aux_shield(int p1)
{
	int p2 = 0;
	int fail_allowed = 39;

	/* if already protected */
	if (borg_shield || borg_goi)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 5;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 5;

	if (!borg_spell_okay_fail(REALM_NATURE, 2, 2, fail_allowed) &&
		!borg_mindcr_okay_fail(MIND_CHAR_ARMOUR, 13, fail_allowed) &&
		!borg_racial_check(RACE_GOLEM, TRUE))
		return (0);

	/* pretend we are protected and look again */
	borg_shield = TRUE;
	p2 = borg_danger(c_y, c_x, 1, TRUE);
	borg_shield = FALSE;

	/* slightly enhance the value if fighting a unique */
	if (borg_fighting_unique) p2 = (p2 * 7 / 10);


	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_spell_fail(REALM_NATURE, 2, 2, fail_allowed) ||
			borg_mindcr_fail(MIND_CHAR_ARMOUR, 13, fail_allowed) ||
			borg_racial(RACE_GOLEM))
			return (p1 - p2);
	}

	/* default to can't do it. */
	return (0);
}

/*
 * Try to get rid of all of the non-uniques around so you can go at it
 * 'mano-e-mano' with the unique.
 */
static int borg_defend_aux_tell_away(int p1)
{
	int p2 = 0, b_n = 0;
	int fail_allowed = 30;
	bool spell_ok;

	/* Only tell away if scared */
	if (p1 < avoidance)
		return (0);

	spell_ok = FALSE;

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance * 4)
		fail_allowed -= 18;
	else
		/* scary */
	if (p1 > avoidance * 3)
		fail_allowed -= 12;
	else
		/* a little scary */
	if (p1 > (avoidance * 5) / 2)
		fail_allowed += 5;

	if (borg_spell_okay_fail(REALM_ARCANE, 3, 3, fail_allowed) ||
		borg_spell_okay_fail(REALM_SORCERY, 1, 4, fail_allowed) ||
		borg_spell_okay_fail(REALM_CHAOS, 1, 5, fail_allowed) ||
		borg_equips_artifact(ART_ULMO, INVEN_WIELD) ||
		(-1 != borg_slot(TV_WAND, SV_WAND_TELEPORT_AWAY) &&
		 borg_items[borg_slot(TV_WAND, SV_WAND_TELEPORT_AWAY)].pval))
		spell_ok = TRUE;

	if (!spell_ok) return (0);

	/* chose then target a bad guy */
	/* Hack, its actually a beam but we leave it as bolt for calculations */
	b_n = borg_launch_bolt(0, 50, GF_AWAY_ALL, MAX_RANGE);

	/* normalize the value */
	p2 = (p1 - b_n);
	if (p2 < 0) p2 = 0;

	/* check to see if I am left better off */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (b_n);

		/* Cast the spell */
		if (borg_spell(REALM_SORCERY, 1, 4) ||
			borg_spell(REALM_ARCANE, 3, 3) ||
			borg_spell(REALM_CHAOS, 1, 5) ||
			borg_activate_artifact(ART_ULMO, FALSE) ||
			borg_aim_wand(SV_WAND_TELEPORT_AWAY))
		{
			/* Use target */
			borg_keypress('5');

			/* Set our shooting flag */
			successful_target = -1;

			/* Value */
			return (b_n);
		}
	}
	return (0);
}

/*
 * Hero to prepare for battle
 */
static int borg_defend_aux_hero(int p1)
{
	int fail_allowed = 10;

	/* already hero */
	if (borg_hero || borg_berserk)
		return (0);

	if (!borg_spell_okay_fail(REALM_LIFE, 3, 0, fail_allowed) &&
		!borg_spell_okay_fail(REALM_DEATH, 2, 0, fail_allowed) &&
		!borg_racial_check(RACE_HALF_TROLL, TRUE) &&
		!borg_racial_check(RACE_BARBARIAN, TRUE) &&
		!borg_mindcr_okay_fail(MIND_ADRENALINE, 23, fail_allowed) &&
		-1 == borg_slot(TV_POTION, SV_POTION_HEROISM))
		return (0);

	/* if we are in some danger but not much, go for a quick bless */
	if (borg_goi || (p1 > avoidance / 12 && p1 < avoidance / 2) ||
		(borg_fighting_unique && p1 < avoidance * 13 / 10))
	{
		/* Simulation */
		/* hero is a low priority */
		if (borg_simulate) return (1);

		/* do it! */
		if (borg_spell(REALM_LIFE, 2, 0) ||
			borg_spell(REALM_DEATH, 3, 0) ||
			borg_mindcr(MIND_ADRENALINE, 23) ||
			borg_racial(RACE_HALF_TROLL) ||
			borg_racial(RACE_BARBARIAN) || borg_quaff_potion(SV_POTION_HEROISM))
			return 1;
	}

	return (0);
}

/*
 * Hero to prepare for battle
 */
static int borg_defend_aux_berserk(int p1)
{
	/* already hero */
	if (borg_hero || borg_berserk)
		return (0);

	if (-1 == borg_slot(TV_POTION, SV_POTION_BERSERK_STRENGTH))
		return (0);

	/* if we are in some danger but not much, go for a quick bless */
	if (borg_goi || (p1 > avoidance / 12 && p1 < avoidance / 2) ||
		(borg_fighting_unique && p1 < avoidance * 13 / 10))
	{
		/* Simulation */
		/* berserk is a low priority */
		if (borg_simulate) return (5);

		/* do it! */
		if (borg_quaff_potion(SV_POTION_BERSERK_STRENGTH))
			return 2;
	}

	return (0);
}

/* Glyph of Warding and Rune of Protection */
static int borg_defend_aux_glyph(int p1)
{
	int p2 = 0, i;
	int fail_allowed = 30;
	bool glyph_spell = FALSE;

	map_block *mb_ptr = map_loc(c_x, c_y);

	/* He should not cast it while on an object.
	 * I have addressed this inadequately in borg9.c when dealing with
	 * messages.  The message "the object resists" will delete the glyph
	 * from the array.  Then I set a broken door on that spot, the borg ignores
	 * broken doors, so he won't loop.
	 */

	if ((mb_ptr->object)		/* ||
								   (mb_ptr->feat == FEAT_MINOR_GLYPH) ||
								   (mb_ptr->feat == FEAT_GLYPH) ||
								   ((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) && (mb_ptr->feat <= FEAT_TRAP_SLEEP)) */
		||
		(mb_ptr->feat == FEAT_CLOSED) || (mb_ptr->feat == FEAT_LESS) ||
		(mb_ptr->feat == FEAT_MORE) || (mb_ptr->feat == FEAT_OPEN) ||
		(mb_ptr->feat == FEAT_BROKEN))
	{
		return (0);
	}

	/* Morgoth breaks these in one try so its a waste of mana against him */
	if (borg_fighting_unique >= 10) return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 5;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 20;

	if (borg_spell_okay_fail(REALM_LIFE, 1, 7, fail_allowed)) glyph_spell =
			TRUE;

	if (0 <= borg_slot(TV_SCROLL, SV_SCROLL_RUNE_OF_PROTECTION)) glyph_spell =
			TRUE;

	if ((borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		 borg_skill[BI_ISIMAGE]) && glyph_spell)
		glyph_spell = FALSE;
	if (!(mb_ptr->flags & MAP_GLOW) &&
		borg_skill[BI_CUR_LITE] == 0) glyph_spell = FALSE;


	if (!glyph_spell) return (0);

	/* pretend we are protected and look again */
	borg_on_glyph = TRUE;
	p2 = borg_danger(c_y, c_x, 1, TRUE);
	borg_on_glyph = FALSE;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_spell_fail(REALM_LIFE, 1, 7, fail_allowed) ||
			borg_read_scroll(SV_SCROLL_RUNE_OF_PROTECTION))
		{
			/* Check for an existing glyph */
			for (i = 0; i < track_glyph_num; i++)
			{
				/* Stop if we already new about this glyph */
				if ((track_glyph_x[i] == c_x) &&
					(track_glyph_y[i] == c_y)) return (p1 - p2);
			}

			/* Track the newly discovered glyph */
			if ((i == track_glyph_num) && (track_glyph_size))
			{
				borg_note("# Noting the creation of a glyph.");
				track_glyph_num++;
				track_glyph_x[i] = c_x;
				track_glyph_y[i] = c_y;
			}
			return (p1 - p2);
		}

	}

	/* default to can't do it. */
	return (0);
}

/* True Warding */
static int borg_defend_aux_true_warding(int p1)
{
	/* Ignore parameter */
	(void) p1;
#if 0
	int p2 = 0;
	int fail_allowed = 30;
	int glyph_bad = 0;
	int glyph_x, glyph_y, x, y;

	map_block *mb_ptr;

	/* any summoners near? */
	if (!borg_fighting_summoner) return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 5;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 20;

	if (!borg_spell_okay_fail(REALM_LIFE, 2, 7, fail_allowed))
		return (0);

	/* Do not cast if surounded by doors or something */
	/* Get grid */
	for (glyph_x = -1; glyph_x <= 1; glyph_x++)
	{
		for (glyph_y = -1; glyph_y <= 1; glyph_y++)
		{
			/* Acquire location */
			x = glyph_x + c_x;
			y = glyph_y + c_y;

			/* Bounds checking */
			if (!map_in_bounds(x, y)) continue;

			mb_ptr = map_loc(x, y);

			/* track spaces already protected */
			if ((mb_ptr->feat >= FEAT_CLOSED) &&
				(mb_ptr->feat <= FEAT_PERM_SOLID))
			{
				glyph_bad++;
			}

			/* track spaces that cannot be protected */
			if ((mb_ptr->object)	/* ||
									   ((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) && (mb_ptr->feat <= FEAT_TRAP_SLEEP)) */
				||
				(mb_ptr->feat == FEAT_LESS) || (mb_ptr->feat == FEAT_MORE)
				|| (mb_ptr->feat == FEAT_OPEN) ||
				(mb_ptr->feat == FEAT_BROKEN) || (mb_ptr->monster))
			{
				glyph_bad++;
			}
		}
	}


	/* Track it */
	/* lets make sure that we going to be benifited */
	if (glyph_bad >= 6)
	{
		/* not really worth it.  Only 2 spaces protected */
		return (0);
	}

	/* pretend we are protected and look again (use the door code) */
	borg_create_door = TRUE;
	p2 = borg_danger(c_y, c_x, 1, TRUE);
	borg_create_door = FALSE;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_spell_fail(REALM_LIFE, 2, 7, fail_allowed))
		{
			/* Set the breeder flag to keep doors closed. Avoid summons */
			breeder_level = TRUE;

			/* Value */
			return (p1 - p2);
		}
	}
#endif /* 0 */

	/* default to can't do it. */
	return (0);
}


/* Create Granite Walls-- Nature spell */
static int borg_defend_aux_create_walls(int p1)
{
	/* Ignore parameter */
	(void) p1;
#if 0
	int p2 = 0;
	int fail_allowed = 30;
	int wall_bad = 0;
	int wall_x, wall_y, x, y;

	map_block *mb_ptr;

	/* any summoners near? */
	if (!borg_fighting_summoner) return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 5;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 20;

	if (!borg_spell_okay_fail(REALM_NATURE, 2, 6, fail_allowed) &&
		!borg_spell_okay_fail(REALM_NATURE, 2, 0, fail_allowed))
		return (0);

	/* Do not cast if surounded by doors or something */
	/* Get grid */
	for (wall_x = -1; wall_x <= 1; wall_x++)
	{
		for (wall_y = -1; wall_y <= 1; wall_y++)
		{
			/* Acquire location */
			x = wall_x + c_x;
			y = wall_y + c_y;

			/* Bounds checking */
			if (!map_in_bounds(x, y)) continue;

			mb_ptr = map_loc(x, y);

			/* track spaces already protected */
			if (				/* (mb_ptr->feat == FEAT_GLYPH) ||
								   (mb_ptr->feat == FEAT_MINOR_GLYPH) || */ mb_ptr->
				   monster ||
				   ((mb_ptr->feat >= FEAT_CLOSED) &&
					(mb_ptr->feat <= FEAT_PERM_SOLID)))
			{
				wall_bad++;
			}

			/* track spaces that cannot be protected */
			if ((mb_ptr->object)	/*||
									   ((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) && (mb_ptr->feat <= FEAT_TRAP_SLEEP)) */
				||
				(mb_ptr->feat == FEAT_LESS) || (mb_ptr->feat == FEAT_MORE)
				|| (mb_ptr->feat == FEAT_OPEN) ||
				(mb_ptr->feat == FEAT_BROKEN) || (mb_ptr->monster))
			{
				wall_bad++;
			}
		}
	}


	/* Track it */
	/* lets make sure that we going to be benifited */
	if (wall_bad >= 6)
	{
		/* not really worth it.  Only 2 spaces protected */
		return (0);
	}

	/* pretend we are protected and look again */
	borg_create_door = TRUE;
	p2 = borg_danger(c_y, c_x, 1, TRUE);
	borg_create_door = FALSE;

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 7))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* do it! */
		if (borg_spell_fail(REALM_NATURE, 2, 0, fail_allowed) ||
			borg_spell_fail(REALM_NATURE, 2, 6, fail_allowed))
		{
			/* Set the breeder flag to keep doors closed. Avoid summons */
			breeder_level = TRUE;

			/* Value */
			return (p1 - p2);
		}
	}
#endif /* 0 */

	/* default to can't do it. */
	return (0);
}



/* This will simulate and cast the mass genocide spell.
 */
static int borg_defend_aux_mass_genocide(void)
{
	int p1 = 0, hit = 0, i = 0, p2;
	int b_p = 0, p;

	borg_kill *kill;
	monster_race *r_ptr;

	/* see if prayer is legal */
	if (!borg_spell_okay_fail(REALM_DEATH, 2, 7, 40) &&
		!borg_spell_okay_fail(REALM_DEATH, 3, 6, 40) &&
		!borg_equips_artifact(ART_EONWE, INVEN_WIELD))
		return (0);

	/* Obtain initial danger, measured over time */
	p1 = borg_danger(c_y, c_x, 1, TRUE);

	/* See if he is in real danger */
	if (p1 < avoidance * 12 / 10)
		return (0);

	/* Find a monster and calculate its danger */
	for (i = 0; i < borg_kills_nxt; i++)
	{

		/* Monster */
		kill = &borg_kills[i];
		r_ptr = &r_info[kill->r_idx];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Check the distance */
		if (distance(c_y, c_x, kill->y, kill->x) > 20) continue;

		/* we try not to genocide uniques */
		if (r_ptr->flags1 & RF1_UNIQUE) continue;

		/* Calculate danger */
		borg_full_damage = TRUE;
		p = borg_danger_aux(c_y, c_x, 1, i, TRUE);
		borg_full_damage = FALSE;

		/* store the danger for this type of monster */
		b_p = b_p + p;
		hit = hit + 4;
	}

	/* normalize the value */
	p2 = (p1 - b_p);
	if (p2 < 0) p2 = 0;

	/* if strain (plus a pad incase we did not know about some monsters)
	 * is greater than hp, don't cast it
	 */
	if ((hit * 11 / 10) >= borg_skill[BI_CURHP]) return (0);

	/* Penalize the strain from casting the spell */
	p2 = p2 + hit;

	/* Be more likely to use this if fighting Morgoth */
	if (borg_fighting_unique >= 10 && (hit / 3 > 8))
	{
		p2 = p2 * 6 / 10;
	}

	/* if this is an improvement and we may not avoid monster now and */
	/* we may have before */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? (avoidance * 2 / 3) : (avoidance / 2)))
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		/* Cast the spell */
		if (borg_spell(REALM_SORCERY, 2, 7) ||
			borg_spell(REALM_SORCERY, 3, 6) ||
			borg_activate_artifact(ART_EONWE, FALSE))
		{
			/* Value */
			return (p1 - p2);
		}
	}
	/* Not worth it */
	return (0);

}

/* This will simulate and cast the genocide spell.
 * There are two seperate functions happening here.
 * 1. will genocide the race which is immediately threatening me.
 * 2. will genocide the race which is most dangerous on the level.  Though it may not be
 *    threatening the borg right now.  It was considered to nuke the escorts of a unique.
 *    But it could also be used to nuke a race if it becomes too dangerous, for example
 *    a summoner called up 15-20 hounds, and they must be dealt with.
 * The first option may be called at any time.  While the 2nd option is only called when the
 * borg is in relatively good health.
 */
static int borg_defend_aux_genocide(void)
{
	int p1 = 0, i, p, u, b_i = 0;
	int p2 = 0;
	int threat = 0;
	int max = 1;

	int b_p[256];
	int b_num[256];
	int b_threat[256];
	int b_threat_num[256];

	char genocide_target = (char)0;
	int b_threat_id = (char)0;

	bool genocide_spell = FALSE;
	int fail_allowed = 39;

	/* Obtain initial danger, measured over time */
	p1 = borg_danger(c_y, c_x, 1, TRUE);


	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance)
		fail_allowed -= 19;
	else
		/* a little scary */
	if (p1 > (avoidance * 2) / 3)
		fail_allowed -= 10;
	else
		/* not very scary, allow lots of fail */
	if (p1 < avoidance / 3)
		fail_allowed += 10;

	if (borg_spell_okay_fail(REALM_DEATH, 1, 6, fail_allowed) ||
		borg_equips_artifact(ART_CELEBORN, INVEN_BODY) ||
		borg_equips_staff_fail(SV_STAFF_GENOCIDE) ||
		(-1 != borg_slot(TV_SCROLL, SV_SCROLL_GENOCIDE)))
	{
		genocide_spell = TRUE;
	}

	if (genocide_spell == FALSE) return (0);


	/* Don't try it if really weak */
	if (borg_skill[BI_CURHP] <= 75) return (0);

	/* two methods to calculate the threat:
	 *1. cycle each character of monsters on screen
	 *   collect collective threat of each char
	 *2 select race of most dangerous guy, and choose him.
	 * Method 2 is cheaper and faster.
	 *
	 * The borg uses method #1
	 */

	/* Clear previous dangers */
	for (i = 0; i < 256; i++)
	{
		b_p[i] = 0;
		b_num[i] = 0;
		b_threat[i] = 0;
		b_threat_num[i] = 0;
	}

	/* Find a monster and calculate its danger */
	for (i = 0; i < borg_kills_nxt; i++)
	{
		borg_kill *kill;
		monster_race *r_ptr;

		/* Monster */
		kill = &borg_kills[i];
		r_ptr = &r_info[kill->r_idx];

		/* Our char of the monster */
		u = r_ptr->d_char;

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* we try not to genocide uniques */
		if (r_ptr->flags1 & RF1_UNIQUE) continue;

		/* Calculate danger */
		borg_full_damage = TRUE;
		p = borg_danger_aux(c_y, c_x, 1, i, TRUE);
		threat = borg_danger_aux(kill->y, kill->x, 1, i, TRUE);
		borg_full_damage = FALSE;

		/* store the danger for this type of monster */
		b_p[u] = b_p[u] + p;
		b_threat[u] = b_threat[u] + threat;

		/* Store the number of this type of monster */
		b_num[u]++;
		b_threat_num[u]++;
	}

	/* Now, see which race contributes the most danger */
	for (i = 0; i < 256; i++)
	{

		/* Skip this one if empty */
		if (!b_p[i]) continue;

		/* for the race threatening me right now */
		if (b_p[i] > max)
		{
			/* track the race */
			max = b_p[i];
			b_i = i;

			/* note the danger with this race gone */
			p2 = p1 - b_p[b_i];
		}

		/* for this race on the whole level */
		if (b_threat[i] > max)
		{
			/* track the race */
			max = b_threat[i];
			b_threat_id = i;
		}

	}

	/* This will track and decide if it is worth genociding this dangerous race for the level */
	if (b_threat_id)
	{
		/* Not if I am weak (should have 400 HP really in case of a Pit) */
		if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] ||
			borg_skill[BI_CURHP] < 375) b_threat_id = 0;

		/* Do not perform in Danger */
		if (borg_danger(c_y, c_x, 1, TRUE) > avoidance / 5) b_threat_id = 0;

		/* The threat must be real */
		if (b_threat[b_threat_id] < borg_skill[BI_MAXHP] * 10) b_threat_id = 0;

		/* Too painful to cast it (padded to be safe incase of unknown monsters) */
		if ((b_num[b_threat_id] * 4) * 11 / 10 >=
			borg_skill[BI_CURHP]) b_threat_id = 0;

		/* report the danger and most dangerous race */
		if (b_threat_id)
		{
			borg_note(format
					  ("# Race '%c' is a real threat with total danger %d from %d individuals.",
					   b_threat_id, b_threat[b_threat_id],
					   b_threat_num[b_threat_id]));
		}

		/* Genociding this race would reduce the danger of the level */
		genocide_target = b_threat_id;

	}

	/* Consider the immediate threat genocide */
	if (b_i)
	{

		/* Too painful to cast it (padded to be safe incase of unknown monsters) */
		if ((b_num[b_i] * 4) * 11 / 10 >= borg_skill[BI_CURHP]) b_i = 0;

		/* See if he is in real danger, generally,
		 * or deeper in the dungeon, conservatively,
		 */
		if (p1 < avoidance * 12 / 10 ||
			(borg_skill[BI_CDEPTH] > 75 && p1 < avoidance * 7 / 10)) b_i = 0;

		/* Did this help improve my situation? */
		if (p1 < p2 && p2 >= (avoidance / 2)) b_i = 0;

		/* Genociding this race would help me immediately */
		genocide_target = b_i;

	}

	/* Complete the genocide routine */
	if (genocide_target)
	{
		/* Simulation */
		if (borg_simulate) return (p1 - p2);

		borg_note(format
				  ("# Genociding race '%c' (%d)", genocide_target,
				   genocide_target));

		/* do it! ---use scrolls first since they clutter inventory */
		if (borg_read_scroll(SV_SCROLL_GENOCIDE) ||
			borg_spell(REALM_DEATH, 1, 6) ||
			borg_activate_artifact(ART_CELEBORN, FALSE) ||
			borg_use_staff(SV_STAFF_GENOCIDE))
		{
			/* and the winner is..... */
			borg_keypress((genocide_target));
		}

		/* Remove this race from the borg_kill */
		for (i = 0; i < borg_kills_nxt; i++)
		{
			borg_kill *kill;
			monster_race *r_ptr;

			/* Monster */
			kill = &borg_kills[i];
			r_ptr = &r_info[kill->r_idx];

			/* Our char of the monster */
			if (r_ptr->d_char != genocide_target) continue;

			/* remove this monster */
			borg_delete_kill(i);
		}

		return (p1 - p2);

	}
	/* default to can't do it. */
	return (0);
}

/* This will cast the genocide spell on Hounds at the beginning of each level.
 */
static int borg_defend_aux_genocide_hounds(void)
{
	int i = 0;

	char genocide_target = 'Z';

	bool genocide_spell = FALSE;

	/* Not if level is already cleared */
	if (genocide_level_hounds && borg_hound_count < 30) return (0);

	/* Not if I am weak */
	if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] ||
		borg_skill[BI_CURHP] < 350) return (0);

	/* only do it when deep, */
	if (borg_skill[BI_CDEPTH] < 50) return (0);

	/* Do not perform in Danger */
	if (borg_danger(c_y, c_x, 1, TRUE) > avoidance / 3)
		return (0);

	if (borg_spell_okay_fail(REALM_DEATH, 1, 6, 35) ||
		borg_equips_artifact(ART_CELEBORN, INVEN_BODY) ||
		borg_equips_staff_fail(SV_STAFF_GENOCIDE))
	{
		genocide_spell = TRUE;
	}

	if (genocide_spell == FALSE) return (0);

	if (borg_simulate) return (1);

	borg_note("# Genociding Hounds at Start of DLevel");

	if (borg_spell(REALM_DEATH, 1, 6) ||
		borg_activate_artifact(ART_CELEBORN, FALSE) ||
		borg_use_staff(SV_STAFF_GENOCIDE))
	{
		/* and the winner is..... */
		borg_keypress((genocide_target));

		/* set the flag to not do it again */
		genocide_level_hounds = TRUE;

		/* Remove this race from the borg_kill */
		for (i = 0; i < borg_kills_nxt; i++)
		{
			borg_kill *kill;
			monster_race *r_ptr;

			/* Monster */
			kill = &borg_kills[i];
			r_ptr = &r_info[kill->r_idx];

			/* Our char of the monster */
			if (r_ptr->d_char != genocide_target) continue;

			/* remove this monster */
			borg_delete_kill(i);

			/* reset the count */
			borg_hound_count = 0;
		}

		return (1);
	}
	/* default to can't do it. */
	return (0);
}

/* Earthquake, priest and mage spells.
 */
static int borg_defend_aux_earthquake(void)
{
	int p1 = 0;
	int p2 = 0;
	int door_bad = 0;
	int door_x, door_y, x, y;

	map_block *mb_ptr;

	/* Obtain initial danger */
	p1 = borg_danger(c_y, c_x, 1, TRUE);

	if (!borg_spell_okay_fail(REALM_NATURE, 3, 0, 35))
		return (0);

	/* See if he is in real danger or fighting summoner */
	if (p1 < avoidance)
		return (0);

	/* Do not cast if surounded by doors or something */
	/* Get grid */
	for (door_x = -1; door_x <= 1; door_x++)
	{
		for (door_y = -1; door_y <= 1; door_y++)
		{
			/* Acquire location */
			x = door_x + c_x;
			y = door_y + c_y;

			/* Bounds checking */
			if (!map_in_bounds(x, y)) continue;

			mb_ptr = map_loc(x, y);

			/* track spaces already protected */
			if ( /*(mb_ptr->feat == FEAT_GLYPH) || */ mb_ptr->monster ||
				((mb_ptr->feat >= FEAT_CLOSED) &&
				 (mb_ptr->feat <= FEAT_PERM_SOLID)))
			{
				door_bad++;
			}

			/* track spaces that cannot be protected */
			if ((mb_ptr->object) ||
				/*((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) && (mb_ptr->feat <= FEAT_TRAP_SLEEP)) || */
				(mb_ptr->feat == FEAT_LESS) ||
				(mb_ptr->feat == FEAT_MORE) ||
				(mb_ptr->feat == FEAT_OPEN) ||
				(mb_ptr->feat == FEAT_BROKEN) || (mb_ptr->monster))
			{
				door_bad++;
			}
		}
	}


	/* Track it */
	/* lets make sure that we going to be benifited */
	if (door_bad >= 6)
	{
		/* not really worth it.  Only 2 spaces protected */
		return (0);
	}

	/* What effect is there? */
	borg_create_door = TRUE;
	p2 = borg_danger(c_y, c_x, 1, TRUE);
	borg_create_door = FALSE;

	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance / 5))
	{
		/* Simulation */
		if (borg_simulate) return (p2);

		/* Cast the spell */
		if (borg_spell(REALM_NATURE, 3, 0))
		{
			return (p2);
		}
	}
	return (0);
}

/* Word of Destruction, priest and mage spells.  Death is right around the
 *  corner, so kill everything.
 */
static int borg_defend_aux_destruction(void)
{
	int p1 = 0;
	int p2 = 0;
	int d = 0;
	bool spell = FALSE;

	/* Borg_defend() is called before borg_escape().  He may have some
	 * easy ways to escape (teleport scroll) but he may attempt this spell
	 * instead of using the scrolls.
	 */
	/* Use teleport scrolls instead of WoD */
	if (borg_skill[BI_AESCAPE] && !borg_skill[BI_ISBLIND] &&
		!borg_skill[BI_ISCONFUSED]) return (0);

	/* Obtain initial danger */
	p1 = borg_danger(c_y, c_x, 1, TRUE);

	if (borg_spell_okay_fail(REALM_CHAOS, 1, 6, 55) ||
		borg_equips_staff_fail(SV_STAFF_DESTRUCTION))
		spell = TRUE;

	/* Special check for super danger--no fail check */
	if (p1 > (avoidance * 4) && borg_equips_staff_fail(SV_STAFF_DESTRUCTION))
		spell = TRUE;

	if (spell == FALSE) return (0);

	/* See if he is in real danger */
	if (p1 < avoidance * 2)
		return (0);

	/* What effect is there? */
	p2 = 0;

	/* value is d */
	d = (p1 - p2);

	/* Try not to cast this against uniques */
	if (borg_fighting_unique && p1 < avoidance * 5) d = 0;
	if (borg_fighting_unique >= 10) d = 0;

	/* Simulation */
	if (borg_simulate) return (d);

	/* Cast the spell */
	if (borg_spell(REALM_CHAOS, 1, 6) || borg_use_staff(SV_STAFF_DESTRUCTION))
	{
		return (d);
	}

	/* oops it did not work */
	return (0);
}

static int borg_defend_aux_banishment(int p1)
{
	int p2 = 0;
	int fail_allowed = 15;
	int i;

	/* Only tell away if scared */
	if (p1 < avoidance * 12 / 10)
		return (0);

	/* if very scary, do not allow for much chance of fail */
	if (p1 > avoidance * 4)
		fail_allowed -= 10;

	if (!borg_spell_okay_fail(REALM_LIFE, 2, 5, fail_allowed) &&
		!borg_spell_okay_fail(REALM_TRUMP, 1, 7, fail_allowed))
		return (0);

	/* reset initial danger */
	p1 = 1;

	/* Two passes to determine exact danger */
	for (i = 0; i < borg_kills_nxt; i++)
	{
		borg_kill *kill;
		monster_race *r_ptr;

		/* Monster */
		kill = &borg_kills[i];
		r_ptr = &r_info[kill->r_idx];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Check the LOS */
		if (!borg_projectable(c_y, c_x, kill->y, kill->x)) continue;

		/* Calculate danger of who is left over */
		borg_full_damage = TRUE;
		p1 += borg_danger_aux(c_y, c_x, 1, i, TRUE);
		borg_full_damage = FALSE;

	}

	/* Pass two -- Find a monster and calculate its danger */
	for (i = 0; i < borg_kills_nxt; i++)
	{
		borg_kill *kill;
		monster_race *r_ptr;

		/* Monster */
		kill = &borg_kills[i];
		r_ptr = &r_info[kill->r_idx];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Check the LOS */
		if (!borg_projectable(c_y, c_x, kill->y, kill->x)) continue;

		/* get rid of evil monsters */
		if (r_ptr->flags3 & RF3_EVIL) continue;

		/* Calculate danger of who is left over */
		borg_full_damage = TRUE;
		p2 += borg_danger_aux(c_y, c_x, 1, i, TRUE);
		borg_full_damage = FALSE;

	}

	/* no negatives */
	if (p2 <= 0) p2 = 0;

	/* Try not to cast this against Morgy/Sauron */
	if (borg_fighting_unique >= 10 && borg_skill[BI_CURHP] > 250 &&
		borg_skill[BI_CDEPTH] == 99) p2 = 9999;
	if (borg_fighting_unique >= 10 && borg_skill[BI_CURHP] > 350 &&
		borg_skill[BI_CDEPTH] == 100) p2 = 9999;

	/* check to see if I am left better off */
	if (p1 > p2 &&
		p2 <= (borg_fighting_unique ? ((avoidance * 2) / 3) : (avoidance / 2))
		&& p1 > (avoidance * 2))
	{
		/* Simulation */
		if (borg_simulate) return (p2);

		/* Cast the spell */
		if (borg_spell_fail(REALM_LIFE, 2, 5, fail_allowed) ||
			borg_spell_fail(REALM_TRUMP, 1, 7, fail_allowed))
		{
			/* Value */
			return (p2);
		}
	}
	return (0);
}



/*
 * Detect Inviso/Monsters
 * Used only if I am hit by an unseen guy.
 * Casts detect invis.
 */
static int borg_defend_aux_inviso(int p1)
{
	int fail_allowed = 25;
	map_block *mb_ptr = map_loc(c_x, c_y);


	/* no need */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_SINV] || borg_see_inv)
		return (0);

	/* not recent */
	if (borg_t > need_see_inviso + 5) return (0);


	/* too dangerous to cast */
	if (p1 > avoidance * 7) return (0);

	/* Do I have anything that will work? */
	if (-1 == borg_slot(TV_POTION, SV_POTION_DETECT_INVIS) &&
		-1 == borg_slot(TV_SCROLL, SV_SCROLL_DETECT_INVIS) &&
		!borg_equips_staff_fail(SV_STAFF_DETECT_INVIS) &&
		!borg_equips_staff_fail(SV_STAFF_DETECT_EVIL) &&
		!borg_spell_okay_fail(REALM_LIFE, 1, 3, fail_allowed) &&
		!borg_spell_okay_fail(REALM_ARCANE, 0, 2, fail_allowed))
		return (0);

	/* Darkness */
	if (!(mb_ptr->flags & MAP_GLOW) && !borg_skill[BI_CUR_LITE]) return (0);

	/* No real value known, but lets cast it to find the bad guys. */
	if (borg_simulate) return (10);


	/* smoke em if you got em */
	/* short time */
	if (borg_quaff_potion(SV_POTION_DETECT_INVIS))
	{
		borg_see_inv = 18000;
		return (10);
	}
	/* long time */
	if (borg_spell_fail(REALM_LIFE, 1, 3, fail_allowed) ||
		borg_spell_fail(REALM_ARCANE, 0, 2, fail_allowed))
	{
		borg_see_inv = 20000;
		return (10);
	}
	/* snap shot */
	if (borg_read_scroll(SV_SCROLL_DETECT_INVIS) ||
		borg_use_staff(SV_STAFF_DETECT_INVIS) ||
		borg_use_staff(SV_STAFF_DETECT_EVIL))
	{
		borg_see_inv = 3000;	/* hack, actually a snap shot, no ignition message */
		return (10);
	}

	/* ah crap, I guess I wont be able to see them */
	return (0);

}

/*
 * Light Beam to spot lurkers
 * Used only if I am hit by an unseen guy.
 * Lights up a hallway.
 */
static int borg_defend_aux_lbeam(void)
{
	bool hallway = FALSE;
	int x = c_x;
	int y = c_y;


	/* no need */
	if (borg_skill[BI_ISBLIND])
		return (0);

	/* Light Beam section to spot non seen guys */
	/* not recent, dont bother */
	if (borg_t > (need_see_inviso + 2))
		return (0);

	/* Check to see if I am in a hallway */
	/* Case 1a: north-south corridor */
	if (borg_cave_floor_bold(y - 1, x) && borg_cave_floor_bold(y + 1, x) &&
		!borg_cave_floor_bold(y, x - 1) && !borg_cave_floor_bold(y, x + 1) &&
		!borg_cave_floor_bold(y + 1, x - 1) &&
		!borg_cave_floor_bold(y + 1, x + 1) &&
		!borg_cave_floor_bold(y - 1, x - 1) &&
		!borg_cave_floor_bold(y - 1, x + 1))
	{
		/* ok to light up */
		hallway = TRUE;
	}

	/* Case 1b: east-west corridor */
	if (borg_cave_floor_bold(y, x - 1) && borg_cave_floor_bold(y, x + 1) &&
		!borg_cave_floor_bold(y - 1, x) && !borg_cave_floor_bold(y + 1, x) &&
		!borg_cave_floor_bold(y + 1, x - 1) &&
		!borg_cave_floor_bold(y + 1, x + 1) &&
		!borg_cave_floor_bold(y - 1, x - 1) &&
		!borg_cave_floor_bold(y - 1, x + 1))
	{
		/* ok to light up */
		hallway = TRUE;
	}

	/* Case 1aa: north-south doorway */
	if (borg_cave_floor_bold(y - 1, x) && borg_cave_floor_bold(y + 1, x) &&
		!borg_cave_floor_bold(y, x - 1) && !borg_cave_floor_bold(y, x + 1))
	{
		/* ok to light up */
		hallway = TRUE;
	}

	/* Case 1ba: east-west doorway */
	if (borg_cave_floor_bold(y, x - 1) && borg_cave_floor_bold(y, x + 1) &&
		!borg_cave_floor_bold(y - 1, x) && !borg_cave_floor_bold(y + 1, x))
	{
		/* ok to light up */
		hallway = TRUE;
	}


	/* not in a hallway */
	if (!hallway) return (0);

	/* Make sure I am not in too much danger */
	/* if (borg_simulate && p1 > avoidance * 3 / 4) return (0); */

	/* test the beam function */
	if (!borg_lite_beam(TRUE)) return (0);

	/* return some value */
	if (borg_simulate) return (10);


	/* if in a hallway call the Light Beam routine */
	if (borg_lite_beam(FALSE))
	{
		return (10);
	}
	return (0);
}

/* Shift the panel to locate offscreen monsters */
static int borg_defend_aux_panel_shift(void)
{
	int dir = 0;

	int x, y;

	int wx, wy;

	/* Get size */
	Term_get_size(&x, &y);

	/* Remove map offset */
	x -= COL_MAP + 1;
	y -= ROW_MAP + 1;

	/* Get panel */
	wx = ((c_x - x / 4) / (x / 2));
	wy = ((c_y - y / 4) / (y / 2));

	/* no need */
	if (!need_shift_panel && borg_skill[BI_CDEPTH] < 70)
		return (0);

	/* Determine our current panel */

	/* Which direction do we need to move? */
	/* Shift panel to the right */
	if (c_x >= 52 && c_x <= 60 && wx == 0) dir = 6;
	if (c_x >= 84 && c_x <= 94 && wx == 1) dir = 6;
	if (c_x >= 116 && c_x <= 123 && wx == 2) dir = 6;
	if (c_x >= 148 && c_x <= 159 && wx == 3) dir = 6;
	/* Shift panel to the left */
	if (c_x <= 142 && c_x >= 136 && wx == 4) dir = 4;
	if (c_x <= 110 && c_x >= 103 && wx == 3) dir = 4;
	if (c_x <= 78 && c_x >= 70 && wx == 2) dir = 4;
	if (c_x <= 46 && c_x >= 37 && wx == 1) dir = 4;

	/* Shift panel down */
	if (c_y >= 15 && c_y <= 19 && wy == 0) dir = 2;
	if (c_y >= 25 && c_y <= 30 && wy == 1) dir = 2;
	if (c_y >= 36 && c_y <= 41 && wy == 2) dir = 2;
	if (c_y >= 48 && c_y <= 52 && wy == 3) dir = 2;
	/* Shift panel up */
	if (c_y <= 51 && c_y >= 47 && wy == 4) dir = 8;
	if (c_y <= 39 && c_y >= 35 && wy == 3) dir = 8;
	if (c_y <= 28 && c_y >= 24 && wy == 2) dir = 8;
	if (c_y <= 17 && c_y >= 13 && wy == 1) dir = 8;

	/* Do the Shift if needed, then note it,  reset the flag */
	if (need_shift_panel == TRUE)
	{
		/* Send action (view panel info) */
		borg_keypress('L');

		if (dir) borg_keypress(I2D(dir));
		borg_keypress(ESCAPE);

		borg_note("# Shifted panel to locate offscreen monster.");
		need_shift_panel = FALSE;
	}
	else
		/* check to make sure its appropriate */
	{

		/* Hack Not if I just did one */
		if (when_shift_panel && (borg_t - when_shift_panel) <= 7)
		{
			/* do nothing */
		}
		else
			/* shift up? only if a north corridor */
		if (dir == 8 && borg_projectable_pure(c_y, c_x, c_y - 2, c_x) &&
				track_step_y[track_step_num - 1] != c_y - 1)
		{
			/* Send action (view panel info) */
			borg_keypress('L');
			if (dir) borg_keypress(I2D(dir));
			borg_note("# Shifted panel as a precaution.");
			/* Mark the time to avoid loops */
			when_shift_panel = borg_t;
		}
		else /* shift down? only if a south corridor */ if (dir == 2 &&
															borg_projectable_pure
															(c_y, c_x, c_y + 2,
															 c_x) &&
															track_step_y
															[track_step_num -
															 1] != c_y + 1)
		{
			/* Send action (view panel info) */
			borg_keypress('L');
			borg_keypress(I2D(dir));
			borg_note("# Shifted panel as a precaution.");
			/* Mark the time to avoid loops */
			when_shift_panel = borg_t;
		}
		else /* shift Left? only if a west corridor */ if (dir == 4 &&
														   borg_projectable_pure
														   (c_y, c_x, c_y,
															c_x - 2) &&
														   track_step_x
														   [track_step_num -
															1] != c_x - 1)
		{
			/* Send action (view panel info) */
			borg_keypress('L');
			if (dir) borg_keypress(I2D(dir));
			borg_note("# Shifted panel as a precaution.");
			/* Mark the time to avoid loops */
			when_shift_panel = borg_t;
		}
		else /* shift Right? only if a east corridor */ if (dir == 6 &&
															borg_projectable_pure
															(c_y, c_x, c_y,
															 c_x + 2) &&
															track_step_x
															[track_step_num -
															 1] != c_x + 1)
		{
			/* Send action (view panel info) */
			borg_keypress('L');
			if (dir) borg_keypress(I2D(dir));
			borg_note("# Shifted panel as a precaution.");
			/* Mark the time to avoid loops */
			when_shift_panel = borg_t;
		}

		/* Leave the panel shift mode */
		borg_keypress(ESCAPE);

	}
	/* This uses no energy */
	return (0);
}

/*
 * Phantasmal Servant.
 * If I dont have one, then get one.
 */
static int borg_defend_aux_servant(int p1)
{
	int fail_allowed = 15;
	int i;
	int friendlies = 0;

	/* must have the ability */
	if (!borg_spell_okay_fail(REALM_TRUMP, 1, 2, fail_allowed))
		return (0);

	/* reset initial danger */
	p1 = 1;

	/* Two passes to determine exact danger */
	for (i = 0; i < borg_kills_nxt; i++)
	{
		borg_kill *kill;

		/* Monster */
		kill = &borg_kills[i];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Skip non Friendly */
		if (!kill->friendly) continue;

		/* Count Friendly */
		friendlies++;
	}

	/* check to see if I am left better off */
	if (friendlies < 5 && p1 < (avoidance / 3))
	{
		/* Simulation */
		if (borg_simulate) return (5);

		/* Cast the spell */
		if (borg_spell_fail(REALM_TRUMP, 1, 2, fail_allowed))
		{
			/* Value */
			return (5);
		}
	}
	return (0);
}


/*
 * Simulate/Apply the optimal result of using the given "type" of defence
 * p1 is the current danger level (passed in for effiency)
 */
static int borg_defend_aux(int what, int p1)
{
	/* Analyze */
	switch (what)
	{
		case BD_SPEED:
		{
			return (borg_defend_aux_speed(p1));
		}

		case BD_PROT_FROM_EVIL:
		{
			return (borg_defend_aux_prot_evil(p1));
		}
		case BD_RESIST_FCE:
		{
			return (borg_defend_aux_resist_fce(p1));
		}
		case BD_RESIST_FECAP:
		{
			return (borg_defend_aux_resist_fecap(p1));
		}
		case BD_RESIST_F:
		{
			return (borg_defend_aux_resist_f(p1));
		}
		case BD_RESIST_C:
		{
			return (borg_defend_aux_resist_c(p1));
		}
		case BD_RESIST_A:
		{
			return (borg_defend_aux_resist_a(p1));
		}
		case BD_RESIST_P:
		{
			return (borg_defend_aux_resist_p(p1));
		}
		case BD_BLESS:
		{
			return (borg_defend_aux_bless(p1));
		}

		case BD_HERO:
		{
			return (borg_defend_aux_hero(p1));
		}
		case BD_BERSERK:
		{
			return (borg_defend_aux_berserk(p1));
		}
		case BD_SHIELD:
		{
			return (borg_defend_aux_shield(p1));
		}
		case BD_GOI:
		{
			return (borg_defend_aux_goi(p1));
		}
		case BD_GOI_POT:
		{
			return (borg_defend_aux_goi_pot(p1));
		}

		case BD_TELL_AWAY:
		{
			return (borg_defend_aux_tell_away(p1));
		}
		case BD_GLYPH:
		{
			return (borg_defend_aux_glyph(p1));
		}
		case BD_WARDING:
		{
			return (borg_defend_aux_true_warding(p1));
		}
		case BD_CREATE_WALLS:
		{
			return (borg_defend_aux_create_walls(p1));
		}
		case BD_MASS_GENOCIDE:
		{
			return (borg_defend_aux_mass_genocide());
		}
		case BD_GENOCIDE:
		{
			return (borg_defend_aux_genocide());
		}
		case BD_GENOCIDE_HOUNDS:
		{
			return (borg_defend_aux_genocide_hounds());
		}
		case BD_EARTHQUAKE:
		{
			return (borg_defend_aux_earthquake());
		}
		case BD_DESTRUCTION:
		{
			return (borg_defend_aux_destruction());
		}
		case BD_BANISHMENT:
		{
			return (borg_defend_aux_banishment(p1));
		}
		case BD_DETECT_INVISO:
		{
			return (borg_defend_aux_inviso(p1));
		}
		case BD_LIGHT_BEAM:
		{
			return (borg_defend_aux_lbeam());
		}
		case BD_SHIFT_PANEL:
		{
			return (borg_defend_aux_panel_shift());
		}
		case BD_TRUMP_SERVANT:
		{
			return (borg_defend_aux_servant(p1));
		}
	}
	return (0);
}

/*
 * prepare to attack... this is setup for a battle.
 */
bool borg_defend(int p1)
{
	int n, b_n = 0;
	int g, b_g = -1;

	map_block *mb_ptr = map_loc(c_x, c_y);

	/* Simulate */
	borg_simulate = TRUE;

	/* if you have a globe up and it is about to drop, */
	/* refresh it (if you can) */
	if (borg_goi && borg_goi < (borg_game_ratio * 2))
	{
		int p;

		/* check 'true' danger. This will make sure we do not */
		/* refresh our GOI if no-one is around */
		borg_attacking = TRUE;
		p = borg_danger(c_y, c_x, 1, TRUE);
		borg_attacking = FALSE;
		if ((p > mb_ptr->fear) || borg_fighting_unique)
		{
			if (borg_spell(REALM_ARCANE, 3, 7) || borg_spell(REALM_LIFE, 3, 7))
			{
				borg_note(format
						  ("# refreshing GOI.  borg_goi=%d, p_ptr->invuln=%d, (ratio=%d)",
						   borg_goi, p_ptr->invuln, borg_game_ratio));
				borg_attempting_refresh = TRUE;
				borg_goi = 12000;
				return (TRUE);
			}
		}
	}

	/* Analyze the possible setup moves */
	for (g = 0; g < BD_MAX; g++)
	{
		/* Simulate */
		n = borg_defend_aux(g, p1);

		/* Track "best" attack */
		if (n <= b_n) continue;

		/* Track best */
		b_g = g;
		b_n = n;
	}

	/* Nothing good */
	if (b_n <= 0)
	{
		return (FALSE);
	}

	/* Note */
	borg_note(format("# Performing defence type %d with value %d", b_g, b_n));

	/* Instantiate */
	borg_simulate = FALSE;

	/* Instantiate */
	(void)borg_defend_aux(b_g, p1);
	/* Success */
	return (TRUE);

}

/*
 * Perma spells.  Some are cool to have on all the time, so long as their
 * mana cost is not too much.
 * There are several types of setup moves:
 *
 *   Temporary speed
 *   Protect From Evil
 *   Prayer
 *   Temp Resist (either all or just cold/fire?)
 *   Shield
 *
 */
enum
{
	BP_SPEED,
	BP_PROT_FROM_EVIL,
	BP_BLESS,
	BP_TELEPATHY,

	BP_RESIST_ALL,
	BP_RESIST_ALL_COLLUIN,
	BP_RESIST_F,
	BP_RESIST_C,
	BP_RESIST_A,
	BP_RESIST_P,
	BP_RESIST_FCE,

	BP_GOI,
	BP_SHIELD,
	BP_HERO,
	BP_BERSERK,
	BP_BERSERK_POTION,

	BP_GLYPH,

	BP_MAX
};

/*
 * Bless/Prayer to prepare for battle
 */
static int borg_perma_aux_bless(void)
{
	int fail_allowed = 5, cost;

	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	/* already blessed */
	if (borg_bless)
		return (0);

	/* Cant when Blind */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (0);

	/* XXX Dark */

	if (!borg_spell_okay_fail(REALM_LIFE, 3, 1, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_LIFE][3][1];
	cost = as->power;
	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	/* bless is a low priority */
	if (borg_simulate) return (1);

	/* do it! */
	if (borg_spell(REALM_LIFE, 3, 0))
		return 1;


	return (0);
}

/* all resists */
static int borg_perma_aux_resist(void)
{
	int cost = 0;
	int fail_allowed = 5;
	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	if (my_oppose_fire + my_oppose_acid + my_oppose_pois +
		my_oppose_elec + my_oppose_cold >= 3)
		return (0);

	/* Not needed if GOI is on */
	if (borg_goi) return (0);

	if (!borg_spell_okay_fail(REALM_NATURE, 2, 3, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_NATURE][2][3];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	if (borg_simulate) return (2);

	/* do it! */
	if (borg_spell_fail(REALM_NATURE, 2, 3, fail_allowed))

		/* Value */
		return (2);


	/* default to can't do it. */
	return (0);
}

/* all resists from the cloak*/
static int borg_perma_aux_resist_colluin(void)
{
	if (my_oppose_fire + my_oppose_acid + my_oppose_pois +
		my_oppose_elec + my_oppose_cold >= 3)
		return (0);

	/* Only use it when Unique is close */
	if (!borg_fighting_unique) return (0);

	/* Not needed if GOI is on */
	if (borg_goi) return (0);


	if (!borg_equips_artifact(ART_COLLUIN, INVEN_OUTER))
		return (0);


	/* Simulation */
	if (borg_simulate) return (2);

	/* do it! */
	if (borg_activate_artifact(ART_COLLUIN, FALSE))

		/* Value */
		return (2);


	/* default to can't do it. */
	return (0);
}

/* resists--- Only bother if a Unique is on the level.*/
static int borg_perma_aux_resist_f(void)
{
	int cost = 0;
	int fail_allowed = 5;
	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	if (my_oppose_fire || !unique_on_level)
		return (0);

	if (borg_skill[BI_IFIRE]) return (0);

	if (!borg_spell_okay_fail(REALM_ARCANE, 1, 6, fail_allowed))
		return (0);

	/* Not needed if GOI is on */
	if (borg_goi) return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_ARCANE][1][6];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >= borg_skill[BI_CURSP] / 20) return (0);

	/* Simulation */
	if (borg_simulate) return (1);

	/* do it! */
	if (borg_spell_fail(REALM_ARCANE, 1, 6, fail_allowed))
	{
		/* Value */
		return (1);
	}

	/* default to can't do it. */
	return (0);
}

/* resists--- Only bother if a Unique is on the level.*/
static int borg_perma_aux_resist_c(void)
{
	int cost = 0;
	int fail_allowed = 5;
	borg_magic *as;


	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	if (my_oppose_cold || !unique_on_level)
		return (0);

	if (borg_skill[BI_ICOLD]) return (0);

	/* Not needed if GOI is on */
	if (borg_goi) return (0);

	if (!borg_spell_okay_fail(REALM_ARCANE, 1, 7, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_ARCANE][1][7];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >= borg_skill[BI_CURSP] / 20) return (0);

	/* Simulation */
	if (borg_simulate) return (1);

	/* do it! */
	if (borg_spell_fail(REALM_ARCANE, 1, 7, fail_allowed))
	{
		/* Value */
		return (1);
	}


	/* default to can't do it. */
	return (0);
}

/* resists--- Only bother if a Unique is on the level.*/
static int borg_perma_aux_resist_a(void)
{
	int cost = 0;
	int fail_allowed = 5;
	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	if (my_oppose_acid || !unique_on_level)
		return (0);

	/* Not needed if GOI is on */
	if (borg_goi) return (0);

	if (!borg_spell_okay_fail(REALM_ARCANE, 2, 1, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_ARCANE][2][1];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >= borg_skill[BI_CURSP] / 20) return (0);

	/* Simulation */
	if (borg_simulate) return (1);

	/* do it! */
	if (borg_spell_fail(REALM_ARCANE, 2, 1, fail_allowed))
	{

		/* Value */
		return (1);
	}


	/* default to can't do it. */
	return (0);
}

/* resists--- Only bother if a Unique is on the level.*/
static int borg_perma_aux_resist_p(void)
{
	int cost = 0;
	int fail_allowed = 5;
	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;


	if (my_oppose_pois || !unique_on_level)
		return (0);

	/* Not needed if GOI is on */
	if (borg_goi) return (0);

	if (!borg_spell_okay_fail(REALM_DEATH, 0, 5, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_DEATH][0][5];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >= borg_skill[BI_CURSP] / 20) return (0);

	/* Simulation */
	if (borg_simulate) return (1);

	/* do it! */
	if (borg_spell_fail(REALM_DEATH, 0, 5, fail_allowed))

		/* Value */
		return (1);


	/* default to can't do it. */
	return (0);
}

/* resist fire and cold for priests */
static int borg_perma_aux_resist_fce(void)
{
	int cost = 0;
	int fail_allowed = 5;
	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	/* cast if one drops and unique is near */
	if (borg_fighting_unique &&
		((my_oppose_fire || borg_skill[BI_IFIRE]) &&
		 (my_oppose_elec || borg_skill[BI_IELEC]) &
		 (my_oppose_cold || borg_skill[BI_ICOLD]))) return (0);


	/* cast if both drop and no unique is near */
	if (!borg_fighting_unique && (my_oppose_fire || my_oppose_cold)) return (0);

	/* no need if immune */
	if (borg_skill[BI_IFIRE] && borg_skill[BI_ICOLD] &&
		borg_skill[BI_IELEC]) return (0);

	/* Not needed if GOI is on */
	if (borg_goi) return (0);

	if (!borg_spell_okay_fail(REALM_NATURE, 0, 6, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_NATURE][0][6];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	if (borg_simulate) return (2);

	/* do it! */
	if (borg_spell_fail(REALM_NATURE, 0, 6, fail_allowed))

		/* Value */
		return (2);


	/* default to can't do it. */
	return (0);
}



/*
 * Speed to prepare for battle
 */
static int borg_perma_aux_speed(void)
{
	int fail_allowed = 7;
	int cost;
	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;


	/* already fast */
	if (borg_speed)
		return (0);

	/* only cast defence spells if fail rate is not too high */
	if (!borg_spell_okay_fail(REALM_SORCERY, 1, 4, fail_allowed) &&
		!borg_spell_okay_fail(REALM_DEATH, 2, 3, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	if (borg_spell_okay_fail(REALM_SORCERY, 1, 4, fail_allowed))
	{
		as = &borg_magics[REALM_SORCERY][1][4];
		cost = as->power;
	}
	else
	{
		as = &borg_magics[REALM_DEATH][2][3];
		cost = as->power;
	}

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	if (borg_simulate) return (5);

	/* do it! */
	if (borg_spell_fail(REALM_SORCERY, 1, 4, fail_allowed) ||
		borg_spell_fail(REALM_DEATH, 2, 3, fail_allowed))
		return (5);

	/* default to can't do it. */
	return (0);
}
static int borg_perma_aux_goi(void)
{
	int fail_allowed = 5;
	int cost;
	borg_magic *as = &borg_magics[0][0][0];

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	/* if already protected */
	if (borg_shield || borg_goi)
		return (0);

	/* only on 100 and when Morgy is near */
	if (borg_skill[BI_CDEPTH] != 100 || borg_fighting_unique <= 9)
		return (0);

	if (!borg_spell_okay_fail(REALM_LIFE, 3, 7, fail_allowed) &&
		!borg_spell_okay_fail(REALM_SORCERY, 3, 7, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	if (borg_spell_okay_fail(REALM_LIFE, 3, 7, fail_allowed))
	{
		as = &borg_magics[REALM_LIFE][3][7];
	}
	else if (borg_spell_okay_fail(REALM_SORCERY, 3, 7, fail_allowed))
	{
		as = &borg_magics[REALM_LIFE][3][7];
	}
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	if (borg_simulate) return (3);

	/* do it! */
	if (borg_spell_fail(REALM_SORCERY, 3, 7, fail_allowed) ||
		borg_spell_fail(REALM_LIFE, 3, 7, fail_allowed))
		return (2);

	/* default to can't do it. */
	return (0);
}

/*
 * Telepathy
 */
static int borg_perma_aux_telepathy(void)
{
	int fail_allowed = 5, cost;

	borg_magic *as = &borg_magics[0][0][0];

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	/* already blessed */
	if (borg_esp || borg_skill[BI_ESP])
		return (0);

	/* must be able to */
	if (!borg_spell_okay_fail(REALM_ARCANE, 3, 7, fail_allowed) &&
		!borg_spell_okay_fail(REALM_SORCERY, 3, 3, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	if (borg_spell_legal_fail(REALM_ARCANE, 3, 7, fail_allowed))
	{
		as = &borg_magics[REALM_ARCANE][3][7];
	}
	else if (borg_spell_legal_fail(REALM_SORCERY, 3, 3, fail_allowed))
	{
		as = &borg_magics[REALM_SORCERY][3][3];
	}

	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	if (borg_simulate) return (1);

	/* do it! */
	if (borg_spell(REALM_ARCANE, 3, 7) || borg_spell(REALM_SORCERY, 3, 3))
		return 1;


	return (0);
}

static int borg_perma_aux_shield(void)
{
	int fail_allowed = 5;
	int cost;
	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	/* if already protected */
	if (borg_shield || borg_goi)
		return (0);

	if (!borg_spell_okay_fail(REALM_NATURE, 2, 2, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_NATURE][2][2];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	if (borg_simulate) return (2);

	/* do it! */
	if (borg_spell_fail(REALM_NATURE, 2, 2, fail_allowed))
		return (2);

	/* default to can't do it. */
	return (0);
}
static int borg_perma_aux_prot_evil(void)
{
	int cost = 0;
	int fail_allowed = 5;
	borg_magic *as = &borg_magics[0][0][0];

	/* if already protected */
	if (borg_prot_from_evil)
		return (0);

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;


	if (!borg_spell_okay_fail(REALM_LIFE, 1, 5, fail_allowed)) return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_LIFE][1][5];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	if (borg_simulate) return (3);

	/* do it! */
	if (borg_spell_fail(REALM_LIFE, 1, 5, fail_allowed))

		/* Value */
		return (3);

	/* default to can't do it. */
	return (0);
}

/*
 * Hero to prepare for battle
 */
static int borg_perma_aux_hero(void)
{
	int fail_allowed = 5, cost;

	borg_magic *as = &borg_magics[0][0][0];

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	/* already blessed */
	if (borg_hero || borg_berserk)
		return (0);

	/* Cant when Blind */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (0);

	/* XXX Dark */

	if (!borg_spell_okay_fail(REALM_LIFE, 3, 0, fail_allowed) &&
		!borg_spell_okay_fail(REALM_DEATH, 2, 0, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	if (borg_spell_legal_fail(REALM_LIFE, 3, 0, fail_allowed))
	{
		as = &borg_magics[REALM_LIFE][3][0];
	}
	else if (borg_spell_legal_fail(REALM_DEATH, 2, 0, fail_allowed))
	{
		as = &borg_magics[REALM_DEATH][2][0];
	}

	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	/* hero is a low priority */
	if (borg_simulate) return (1);

	/* do it! */
	if (borg_spell(REALM_LIFE, 7, 0) || borg_spell(REALM_DEATH, 7, 0))
		return 1;


	return (0);
}

/*
 * Berserk to prepare for battle
 */
static int borg_perma_aux_berserk(void)
{
	int fail_allowed = 5, cost;

	borg_magic *as;

	/* increase the threshold */
	if (unique_on_level) fail_allowed = 10;
	if (borg_fighting_unique) fail_allowed = 15;

	/* already blessed */
	if (borg_hero || borg_berserk)
		return (0);

	/* Cant when Blind */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED]) return (0);

	/* XXX Dark */

	if (!borg_spell_okay_fail(REALM_DEATH, 2, 2, fail_allowed))
		return (0);

	/* Obtain the cost of the spell */
	as = &borg_magics[REALM_DEATH][2][2];
	cost = as->power;

	/* If its cheap, go ahead */
	if (cost >=
		((unique_on_level) ? borg_skill[BI_CURSP] / 7 : borg_skill[BI_CURSP] /
		 10)) return (0);

	/* Simulation */
	/* Berserk is a low priority */
	if (borg_simulate) return (2);

	/* do it! */
	if (borg_spell(REALM_DEATH, 2, 2))
		return 2;


	return (0);
}

/*
 * Berserk to prepare for battle
 */
static int borg_perma_aux_berserk_potion(void)
{

	/* Saver the potions */
	if (!borg_fighting_unique) return (0);

	/* already blessed */
	if (borg_hero || borg_berserk)
		return (0);

	/* do I have any? */
	if (-1 == borg_slot(TV_POTION, SV_POTION_BERSERK_STRENGTH))
		return (0);

	/* Simulation */
	/* Berserk is a low priority */
	if (borg_simulate) return (2);

	/* do it! */
	if (borg_quaff_potion(SV_POTION_BERSERK_STRENGTH))
		return (2);


	return (0);
}

/* Glyph of Warding in a a-s corridor */
static int borg_perma_aux_glyph(void)
{
#if 0
	int i, wall_y, wall_x, wall_count = 0, y, x;
	int fail_allowed = 20;

	map_block *mb_ptr = map_loc(c_x, c_y);

	/* check to make sure a summoner is near */
	if (borg_kills_summoner == -1) return (0);

	/* make sure I have the spell */
	if (!borg_spell_okay_fail(REALM_LIFE, 1, 7, fail_allowed)) return (0);


	/* He should not cast it while on an object.
	 * I have addressed this inadequately in borg9.c when dealing with
	 * messages.  The message "the object resists" will delete the glyph
	 * from the array.  Then I set a broken door on that spot, the borg ignores
	 * broken doors, so he won't loop.
	 */
	if ((mb_ptr->object)		/*||
								   (mb_ptr->feat == FEAT_GLYPH) ||
								   ((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) && (mb_ptr->feat <= FEAT_TRAP_SLEEP)) */
		||
		(mb_ptr->feat == FEAT_CLOSED) || (mb_ptr->feat == FEAT_LESS) ||
		(mb_ptr->feat == FEAT_MORE) || (mb_ptr->feat == FEAT_OPEN) ||
		(mb_ptr->feat == FEAT_BROKEN))
	{
		return (0);
	}

	/* This spell is cast while he is digging and AS Corridor */
	/* Get grid */
	for (wall_x = -1; wall_x <= 1; wall_x++)
	{
		for (wall_y = -1; wall_y <= 1; wall_y++)
		{
			/* Acquire location */
			x = wall_x + c_x;
			y = wall_y + c_y;

			/* Bounds checking */
			if (!map_in_bounds(x, y)) continue;

			mb_ptr = map_loc(x, y);

			/* track adjacent walls */
			if (				/* (mb_ptr->feat == FEAT_GLYPH) || */
				   (mb_ptr->feat == FEAT_PILLAR) ||
				   ((mb_ptr->feat >= FEAT_MAGMA) &&
					(mb_ptr->feat <= FEAT_WALL_SOLID)))
			{
				wall_count++;
			}
		}
	}

	/* must be in a corridor */
	if (wall_count < 7) return (0);

	/* Simulation */
	if (borg_simulate) return (10);

	/* do it! */
	if (borg_spell_fail(REALM_LIFE, 1, 7, fail_allowed) ||
		borg_read_scroll(SV_SCROLL_RUNE_OF_PROTECTION))
	{
		/* Check for an existing glyph */
		for (i = 0; i < track_glyph_num; i++)
		{
			/* Stop if we already new about this glyph */
			if ((track_glyph_x[i] == c_x) &&
				(track_glyph_y[i] == c_y)) return (2);
		}

		/* Track the newly discovered glyph */
		if ((i == track_glyph_num) && (track_glyph_size))
		{
			borg_note("# Noting the creation of a corridor glyph.");
			track_glyph_num++;
			track_glyph_x[i] = c_x;
			track_glyph_y[i] = c_y;
		}
		return (2);
	}
#endif /* 0 */

	/* default to can't do it. */
	return (0);
}



/*
 * Simulate/Apply the optimal result of using the given "type" of set-up
 */
static int borg_perma_aux(int what)
{

	/* Analyze */
	switch (what)
	{
		case BP_SPEED:
		{
			return (borg_perma_aux_speed());
		}
		case BP_TELEPATHY:
		{
			return (borg_perma_aux_telepathy());
		}

		case BP_PROT_FROM_EVIL:
		{
			return (borg_perma_aux_prot_evil());
		}
		case BP_RESIST_ALL:
		{
			return (borg_perma_aux_resist());
		}
		case BP_RESIST_ALL_COLLUIN:
		{
			return (borg_perma_aux_resist_colluin());
		}
		case BP_RESIST_F:
		{
			return (borg_perma_aux_resist_f());
		}
		case BP_RESIST_C:
		{
			return (borg_perma_aux_resist_c());
		}
		case BP_RESIST_A:
		{
			return (borg_perma_aux_resist_a());
		}
		case BP_RESIST_P:
		{
			return (borg_perma_aux_resist_p());
		}
		case BP_RESIST_FCE:
		{
			return (borg_perma_aux_resist_fce());
		}
		case BP_BLESS:
		{
			return (borg_perma_aux_bless());
		}
		case BP_HERO:
		{
			return (borg_perma_aux_hero());
		}
		case BP_BERSERK:
		{
			return (borg_perma_aux_berserk());
		}
		case BP_BERSERK_POTION:
		{
			return (borg_perma_aux_berserk_potion());
		}
		case BP_GOI:
		{
			return (borg_perma_aux_goi());
		}

		case BP_SHIELD:
		{
			return (borg_perma_aux_shield());
		}
		case BP_GLYPH:
		{
			return (borg_perma_aux_glyph());
		}
	}
	return (0);
}


/*
 * prepare to attack... this is setup for a battle.
 */
bool borg_perma_spell()
{
	int n, b_n = 0;
	int g, b_g = -1;


	/* Simulate */
	borg_simulate = TRUE;

	/* Not in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);

	/* No perma-spells until clevel 30 to avoid nasty loops with resting for
	 * mana regain
	 */
	if (borg_skill[BI_CLEVEL] < 30) return (FALSE);

	/* Analyze the possible setup moves */
	for (g = 0; g < BP_MAX; g++)
	{
		/* Simulate */
		n = borg_perma_aux(g);

		/* Track "best" move */
		if (n <= b_n) continue;

		/* Track best */
		b_g = g;
		b_n = n;
	}

	/* Nothing good */
	if (b_n <= 0)
	{
		return (FALSE);
	}

	/* Note */
	borg_note(format
			  ("# Performing perma-spell type %d with value %d", b_g, b_n));

	/* Instantiate */
	borg_simulate = FALSE;

	/* Instantiate */
	(void)borg_perma_aux(b_g);
	/* Success */
	return (TRUE);

}

/*
 * check to make sure there are no monsters around
 * that should prevent resting also make sure the ground
 * is safe for us.
 */
bool borg_check_rest(void)
{
	int i;


	/* Do not rest in Sunlight */
	if (borg_skill[BI_FEAR_LITE] && borg_skill[BI_CDEPTH] == 0)
	{
		/* day time */
		if ((borg_skill[BI_HRTIME] >= 5) && (borg_skill[BI_HRTIME] <= 18))
		{
			return (FALSE);
		}
	}

	/* Do not rest with Phial or Star if it hurts */
	if (borg_skill[BI_FEAR_LITE] && borg_items[INVEN_LITE].xtra_name)
	{
		return (FALSE);
	}

	/* now check the ground to see if safe. */
	if (borg_on_safe_grid() == FALSE) return (FALSE);

	/* Do not rest recently after killing a multiplier */
	/* This will avoid the problem of resting next to */
	/* an unkown area full of breeders */
	if (when_last_kill_mult > (borg_t - 4) && when_last_kill_mult <= borg_t)
		return (FALSE);

	when_last_kill_mult = 0;

	/* Generally disturb_move is off */
	disturb_move = FALSE;

	/* Examine all the monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		borg_kill *kill = &borg_kills[i];
		monster_race *r_ptr = &r_info[kill->r_idx];

		int x9 = kill->x;
		int y9 = kill->y;
		int ax, ay, d;
		int p = 0;

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Distance components */
		ax = (x9 > c_x) ? (x9 - c_x) : (c_x - x9);
		ay = (y9 > c_y) ? (y9 - c_y) : (c_y - y9);

		/* Distance */
		d = MAX(ax, ay);

		/* Minimal distance */
		if (d > 16) continue;

		/* if too close, don't rest */
		if (d < 2) return (FALSE);

		/* if too close, don't rest */
		if (d < 3 && !(r_ptr->flags1 & RF1_NEVER_MOVE)) return (FALSE);

		/* one call for dangers */
		borg_full_damage = TRUE;
		p = borg_danger_aux(y9, x9, 1, i, TRUE);
		borg_full_damage = FALSE;


		/* Real scary guys pretty close */
		if (d < 5 && (p > avoidance / 3)) return (FALSE);

		/* Scary guys kinda close, tinker with disturb near.
		 * We do not want a borg with ESP stopping his rest
		 * each round, having only rested one turn. So we set
		 * disturb_move to true only if some scary guys are
		 * somewhat close to us.
		 */
		if (d < 13 && (p > avoidance) && avoidance < borg_skill[BI_MAXHP] / 2)
			disturb_move = TRUE;
		if (d < 8 && (p > avoidance / 2))
			disturb_move = TRUE;


		/* should check LOS... monster to me */
		if (borg_los(y9, x9, c_y, c_x)) return FALSE;

		/* should check LOS... me to monster */
		if (borg_los(c_y, c_x, y9, x9)) return FALSE;

		/* Perhaps borg should check and see if the previous grid was los */

		/* if absorbs mana, not safe */
		if ((r_ptr->flags5 & RF5_DRAIN_MANA) &&
			(borg_skill[BI_MAXSP] > 1)) return FALSE;

		/* if it walks through walls, not safe */
		if (r_ptr->flags2 & RF2_PASS_WALL) return FALSE;
		if (r_ptr->flags2 & RF2_KILL_WALL) return FALSE;
	}


	/* Otherwise ok */
	return TRUE;
}

/*
 * Attempt to recover from damage and such after a battle
 *
 * Note that resting while in danger is counter-productive, unless
 * the danger is low, in which case it may induce "farming".
 *
 * Note that resting while recall is active will often cause you
 * to lose hard-won treasure from nasty monsters, so we disable
 * resting when waiting for recall in the dungeon near objects.
 *
 * First we try spells/prayers, which are "free", and then we
 * try food, potions, scrolls, staffs, rods, artifacts, etc.
 *
 * XXX XXX XXX
 * Currently, we use healing spells as if they were "free", but really,
 * this is only true if the "danger" is less than the "reward" of doing
 * the healing spell, and if there are no monsters which might soon step
 * around the corner and attack.
 */
bool borg_recover(void)
{
	int p = 0;
	int q;

	map_block *mb_ptr = map_loc(c_x, c_y);

	/*** Handle annoying situations ***/

	/* Refuel current torch */
	if ((borg_items[INVEN_LITE].tval == TV_LITE) &&
		(borg_items[INVEN_LITE].sval == SV_LITE_TORCH))
	{
		/* Refuel the torch if needed */
		if (borg_items[INVEN_LITE].timeout < 250)
		{
			if (borg_refuel_torch()) return (TRUE);

			/* Take note */
			borg_note(format("# Need to refuel but cant!", p));

			/* Allow Pets to Roam so we dont hit them in the dark. */
			p_ptr->pet_follow_distance = PET_STAY_AWAY;

		}
	}

	/* Refuel current lantern */
	if ((borg_items[INVEN_LITE].tval == TV_LITE) &&
		(borg_items[INVEN_LITE].sval == SV_LITE_LANTERN))
	{
		/* Refuel the lantern if needed */
		if (borg_items[INVEN_LITE].timeout < 500)
		{
			if (borg_refuel_lantern()) return (TRUE);

			/* Take note */
			borg_note(format("# Need to refuel but cant!", p));
		}
	}


	/*** Do not recover when in danger ***/

	/* Look around for danger */
	p = borg_danger(c_y, c_x, 1, TRUE);

	/* Never recover in dangerous situations */
	if (p > avoidance / 4) return (FALSE);


	/*** Roll for "paranoia" ***/

	/* Base roll */
	q = randint0(100);

	/* Half dead */
	if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2) q = q - 10;

	/* Almost dead */
	if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 4) q = q - 10;


	/*** Use "cheap" cures ***/

	/* Hack -- cure stun */
	if (borg_skill[BI_ISSTUN] && (q < 75))
	{
		if (borg_activate_artifact(ART_LOTHARANG, FALSE) ||
			borg_spell(REALM_LIFE, 0, 1) ||
			borg_spell(REALM_LIFE, 0, 6) || borg_spell(REALM_ARCANE, 0, 7))

		{
			/* Take note */
			borg_note(format("# Cure Stun", p));

			return (TRUE);
		}
	}

	/* Hack -- cure stun */
	if (borg_skill[BI_ISHEAVYSTUN])
	{
		if (borg_activate_artifact(ART_LOTHARANG, FALSE) ||
			borg_spell(REALM_LIFE, 1, 2))
		{
			/* Take note */
			borg_note(format("# Cure Heavy Stun", p));

			return (TRUE);
		}
	}

	/* Hack -- cure cuts */
	if (borg_skill[BI_ISCUT] && (q < 75))
	{
		if (borg_activate_artifact(ART_LOTHARANG, FALSE) ||
			borg_spell(REALM_LIFE, 1, 2) ||
			borg_spell(REALM_NATURE, 0, 7) || borg_spell(REALM_LIFE, 0, 6))
		{
			/* Take note */
			borg_note(format("# Cure Cuts", p));

			return (TRUE);
		}
	}

	/* Hack -- cure poison */
	if (borg_skill[BI_ISPOISONED] && (q < 75))
	{
		if (borg_activate_artifact(ART_DAL, FALSE) ||
			borg_spell(REALM_ARCANE, 1, 7) ||
			borg_spell(REALM_NATURE, 0, 7) || borg_spell(REALM_LIFE, 1, 2))
		{
			/* Take note */
			borg_note(format("# Cure poison", p));

			return (TRUE);
		}
	}

	/* Hack -- cure fear */
	if (borg_skill[BI_ISAFRAID] && (q < 75))
	{
		if (borg_activate_artifact(ART_DAL, FALSE) ||
			borg_spell(REALM_LIFE, 0, 3))
		{
			/* Take note */
			borg_note(format("# Cure fear", p));

			return (TRUE);
		}
	}

	/* Hack -- satisfy hunger */
	if ((borg_skill[BI_ISHUNGRY] || borg_skill[BI_ISWEAK]) && (q < 75))
	{
		if (borg_spell_fail(REALM_SORCERY, 2, 0, 65) ||
			borg_spell_fail(REALM_LIFE, 0, 7, 65) ||
			borg_spell_fail(REALM_ARCANE, 2, 7, 65) ||
			borg_spell_fail(REALM_NATURE, 0, 3, 65) ||
			borg_racial(RACE_HOBBIT) ||
			borg_read_scroll(SV_SCROLL_SATISFY_HUNGER))
		{
			return (TRUE);
		}
	}

	/* Hack -- heal damage */
	if ((borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2) && (q < 75) && p == 0
		&& (borg_skill[BI_CURSP] > borg_skill[BI_MAXSP] / 4))
	{
		if (borg_activate_artifact(ART_SOULKEEPER, FALSE) ||
			borg_spell(REALM_LIFE, 1, 6) || borg_spell(REALM_NATURE, 1, 7))
		{
			/* Take note */
			borg_note(format("# heal damage (recovering)"));

			return (TRUE);
		}
	}

	/* cure experience loss with prayer */
	if (borg_skill[BI_ISFIXEXP] &&
		(borg_activate_artifact(ART_LUTHIEN, FALSE) ||
		 borg_spell(REALM_LIFE, 3, 3) ||
		 borg_spell(REALM_DEATH, 1, 7) ||
		 borg_racial(RACE_SKELETON) || borg_racial(RACE_ZOMBIE)))
	{
		return (TRUE);
	}

	/* cure stat drain with prayer */
	if ((borg_skill[BI_ISFIXSTR] ||
		 borg_skill[BI_ISFIXINT] ||
		 borg_skill[BI_ISFIXWIS] ||
		 borg_skill[BI_ISFIXDEX] ||
		 borg_skill[BI_ISFIXCON] ||
		 borg_skill[BI_ISFIXCHR] ||
		 borg_skill[BI_ISFIXALL]) && borg_spell(REALM_LIFE, 3, 3))
	{
		return (TRUE);
	}

	/*** Use "expensive" cures ***/

	/* Hack -- cure stun */
	if (borg_skill[BI_ISSTUN] && (q < 25))
	{
		if (borg_use_staff_fail(SV_STAFF_CURING) ||
			borg_zap_rod(SV_ROD_CURING) ||
			borg_zap_rod(SV_ROD_HEALING) ||
			borg_activate_artifact(ART_SOULKEEPER, FALSE) ||
			borg_activate_artifact(ART_GONDOR, FALSE) ||
			borg_quaff_crit(FALSE) || borg_quaff_potion(SV_POTION_CURING))
		{
			return (TRUE);
		}
	}

	/* Hack -- cure heavy stun */
	if (borg_skill[BI_ISHEAVYSTUN] && (q < 95))
	{
		if (borg_quaff_crit(TRUE) ||
			borg_quaff_potion(SV_POTION_CURING) ||
			borg_use_staff_fail(SV_STAFF_CURING) ||
			borg_zap_rod(SV_ROD_CURING) ||
			borg_zap_rod(SV_ROD_HEALING) ||
			borg_activate_artifact(ART_SOULKEEPER, FALSE) ||
			borg_activate_artifact(ART_GONDOR, FALSE))
		{
			return (TRUE);
		}
	}

	/* Hack -- cure cuts */
	if (borg_skill[BI_ISCUT] && (q < 25))
	{
		if (borg_use_staff_fail(SV_STAFF_CURING) ||
			borg_zap_rod(SV_ROD_CURING) ||
			borg_zap_rod(SV_ROD_HEALING) ||
			borg_quaff_potion(SV_POTION_CURING) ||
			borg_activate_artifact(ART_SOULKEEPER, FALSE) ||
			borg_activate_artifact(ART_GONDOR, FALSE) ||
			borg_quaff_crit(borg_skill[BI_CURHP] < 10))
		{
			return (TRUE);
		}
	}

	/* Hack -- cure poison */
	if (borg_skill[BI_ISPOISONED] && (q < 25))
	{
		if (borg_quaff_potion(SV_POTION_CURE_POISON) ||
			borg_quaff_potion(SV_POTION_SLOW_POISON) ||
			borg_eat_food(SV_FOOD_WAYBREAD) ||
			borg_eat_food(SV_FOOD_CURE_POISON) ||
			borg_quaff_crit(borg_skill[BI_CURHP] < 10) ||
			borg_use_staff_fail(SV_STAFF_CURING) ||
			borg_zap_rod(SV_ROD_CURING) ||
			borg_quaff_potion(SV_POTION_CURING) ||
			borg_activate_artifact(ART_DAL, FALSE))
		{
			return (TRUE);
		}
	}

	/* Hack -- cure blindness */
	if (borg_skill[BI_ISBLIND] && (q < 25))
	{
		if (borg_eat_food(SV_FOOD_CURE_BLINDNESS) ||
			borg_quaff_potion(SV_POTION_CURE_LIGHT) ||
			borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			borg_quaff_crit(FALSE) ||
			borg_use_staff_fail(SV_STAFF_CURING) ||
			borg_quaff_potion(SV_POTION_CURING) || borg_zap_rod(SV_ROD_CURING))
		{
			return (TRUE);
		}
	}

	/* Hack -- cure confusion */
	if (borg_skill[BI_ISCONFUSED] && (q < 25))
	{
		if (borg_eat_food(SV_FOOD_CURE_CONFUSION) ||
			borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			borg_quaff_crit(FALSE) ||
			borg_use_staff_fail(SV_STAFF_CURING) ||
			borg_zap_rod(SV_ROD_CURING) || borg_quaff_potion(SV_POTION_CURING))
		{
			return (TRUE);
		}
	}

	/* Hack -- cure fear */
	if (borg_skill[BI_ISAFRAID] && (q < 25))
	{
		if (borg_eat_food(SV_FOOD_CURE_PARANOIA) ||
			borg_quaff_potion(SV_POTION_BOLDNESS) ||
			borg_quaff_potion(SV_POTION_HEROISM) ||
			borg_quaff_potion(SV_POTION_BERSERK_STRENGTH) ||
			borg_activate_artifact(ART_DAL, FALSE) ||
			borg_racial(RACE_HALF_ORC) || borg_racial(RACE_HALF_TROLL))
		{
			return (TRUE);
		}
	}

	/* Hack -- satisfy hunger */
	if ((borg_skill[BI_ISHUNGRY] || borg_skill[BI_ISWEAK]) && (q < 25))
	{
		if (borg_read_scroll(SV_SCROLL_SATISFY_HUNGER))
		{
			return (TRUE);
		}
	}

	/* Hack -- heal damage */
	if ((borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2) && (q < 25))
	{
		if (borg_zap_rod(SV_ROD_HEALING) ||
			borg_quaff_potion(SV_POTION_CURE_SERIOUS) ||
			borg_quaff_crit(FALSE) ||
			borg_activate_artifact(ART_LOTHARANG, FALSE))
		{
			return (TRUE);
		}
	}

	/* If Fleeing, then do not rest */
	if (goal_fleeing) return (FALSE);

	/* Hack -- Rest to recharge Rods of Healing or Recall */
	if (borg_has[374] || borg_has[354])
	{
		/* Step 1.  Recharge just 1 rod. */
		if ((borg_has[374] &&
			 !borg_items[borg_slot(TV_ROD, SV_ROD_HEALING)].pval) ||
			(borg_has[354] &&
			 !borg_items[borg_slot(TV_ROD, SV_ROD_RECALL)].pval))
		{
			/* Mages can cast the recharge spell */



			/* Rest until at least one recharges */
			if (!borg_skill[BI_ISWEAK] && !borg_skill[BI_ISCUT] &&
				!borg_skill[BI_ISHUNGRY] && !borg_skill[BI_ISPOISONED] &&
				borg_check_rest() && !borg_spell_okay(REALM_SORCERY, 6, 2))
			{
				/* Take note */
				borg_note("# Resting to recharge a rod...");

				/* Reset the Bouncing-borg Timer */
				time_this_panel = 0;

				/* Rest until done */
				borg_keypress('R');
				borg_keypress('1');
				borg_keypress('0');
				borg_keypress('0');
				borg_keypress('\n');

				/* Done */
				return (TRUE);
			}
		}
	}

	/*** Just Rest ***/

	/* Hack -- rest until healed */
	if ((borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		 borg_skill[BI_ISIMAGE] || borg_skill[BI_ISAFRAID] ||
		 borg_skill[BI_ISSTUN] || borg_skill[BI_ISHEAVYSTUN] ||
		 (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP]) ||
		 (borg_skill[BI_CURSP] < borg_skill[BI_MAXSP] * 6 / 10)) &&
		(!borg_takes_cnt || !goal_recalling) && !borg_goi && !borg_shield &&
		(borg_skill
		 [BI_CDEPTH] !=
		 100) &&
		!scaryguy_on_level && (randint0(100) < 90) && borg_check_rest() &&
		(p <= mb_ptr->fear) && !goal_fleeing)
	{
		/* XXX XXX XXX */
		if (!borg_skill[BI_ISWEAK] && !borg_skill[BI_ISCUT] &&
			!borg_skill[BI_ISHUNGRY] && !borg_skill[BI_ISPOISONED])
		{
			/* Take note */
			borg_note(format("# Resting (danger %d)...", p));

			/* Rest until done */
			borg_keypress('R');
			borg_keypress('&');
			borg_keypress('\n');

			/* Reset our panel clock */
			time_this_panel = 0;

			/* Done */
			return (TRUE);
		}
	}

	/* Hack to recharge mana if a low level mage or priest */
	if (borg_skill[BI_MAXSP] && borg_skill[BI_CLEVEL] < 25 &&
		borg_skill[BI_CURSP] < borg_skill[BI_MAXSP] && p == 0)
	{
		if (!borg_skill[BI_ISWEAK] && !borg_skill[BI_ISCUT] &&
			!borg_skill[BI_ISHUNGRY] && !borg_skill[BI_ISPOISONED])
		{
			/* Take note */
			borg_note(format("# Resting to gain Mana. (danger %d)...", p));

			/* Rest until done */
			borg_keypress('R');
			borg_keypress('*');
			borg_keypress('\n');

			/* Done */
			return (TRUE);
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * Take one "step" towards the given location, return TRUE if possible
 */
static bool borg_play_step(int y2, int x2)
{
	map_block *mb_ptr;

	int dir, x, y, ox, oy, i;

	int o_y = 0, o_x = 0, door_found = 0;

	/* Breeder levels, close all doors */
	if (breeder_level)
	{
		/* scan the adjacent grids */
		for (ox = -1; ox <= 1; ox++)
		{
			for (oy = -1; oy <= 1; oy++)
			{
				/* skip our own spot */
				if ((oy + c_y == c_y) && (ox + c_x == c_x)) continue;

				/* skip our orignal goal */
				if ((oy + c_y == y2) && (ox + c_x == x2)) continue;

				/* Bounds checking */
				if (!map_in_bounds(ox + c_x, oy + c_y)) continue;

				/* Acquire location */
				mb_ptr = map_loc(ox + c_x, oy + c_y);

				/* skip non open doors */
				if (mb_ptr->feat != FEAT_OPEN) continue;

				/* skip monster on door */
				if (mb_ptr->monster) continue;

				/* Skip repeatedly closed doors */
				if (track_door_num >= 255) continue;

				/* save this spot */
				o_y = oy;
				o_x = ox;
				door_found = 1;
			}
		}

		/* Is there a door to close? */
		if (door_found)
		{
			/* Get a direction, if possible */
			dir = borg_goto_dir(c_y, c_x, c_y + o_y, c_x + o_x);

			/* Obtain the destination */
			x = c_x + ddx[dir];
			y = c_y + ddy[dir];

			/* Hack -- set goal */
			g_x = x;
			g_y = y;

			/* Close */
			borg_note("# Closing a door");
			borg_keypress('c');
			borg_keypress(I2D(dir));

			/* Check for an existing flag */
			for (i = 0; i < track_door_num; i++)
			{
				/* Stop if we already new about this door */
				if ((track_door_x[i] == x) &&
					(track_door_y[i] == y)) return (TRUE);
			}

			/* Track the newly closed door */
			if (i == track_door_num && i < track_door_size)
			{

				borg_note("# Noting the closing of a door.");
				track_door_num++;
				track_door_x[i] = x;
				track_door_y[i] = y;
			}
			return (TRUE);

		}
	}

	/* Get a direction, if possible */
	dir = borg_goto_dir(c_y, c_x, y2, x2);

	/* We have arrived */
	if (dir == 5) return (FALSE);

	/* Obtain the destination */
	x = c_x + ddx[dir];
	y = c_y + ddy[dir];

	/* Access the grid we are stepping on (Assume this is in bounds) */
	mb_ptr = map_loc(x, y);

	/* Hack -- set goal */
	g_x = x;
	g_y = y;

	/* Monsters -- Attack */
	if (mb_ptr->kill)
	{
		borg_kill *kill = &borg_kills[mb_ptr->kill];

		/* can't attack someone if afraid! */
		if (borg_skill[BI_ISAFRAID])
			return (FALSE);

		/* Hack -- ignore Maggot until later.  */
		if ((r_info[kill->r_idx].flags1 & RF1_UNIQUE) &&
			borg_skill[BI_CDEPTH] == 0 && borg_skill[BI_CLEVEL] < 5)
			return (FALSE);

		/* Message */
		borg_note(format("# Walking into a '%s' at (%d,%d)",
						 r_name + r_info[kill->r_idx].name, kill->y, kill->x));

		/* Walk into it */
		if (my_no_alter)
		{
			borg_keypress(';');
			my_no_alter = FALSE;
		}
		else
		{
			borg_keypress('+');
		}
		borg_keypress(I2D(dir));
		return (TRUE);
	}


	/* Objects -- Take */
	if (mb_ptr->object)
	{
#if 0
		/*** Handle Chests ***/
		/* The borg will cheat when it comes to chests.
		 * He does not have to but it makes him faster and
		 * it does not give him any information that a
		 * person would not know by looking at the trap.
		 * So there is no advantage to the borg.
		 */
		if (strstr(k_name + k_info[mb_ptr->object].name, "chest") &&
			!strstr(k_name + k_info[mb_ptr->object].name, "Ruined"))
		{
			cave_type *c_ptr = &cave[y2][x2];
			object_type *o_ptr = &o_list[c_ptr->o_idx];

			/* Unknown, Search it */
			if (!object_known_p(o_ptr) && chest_traps[o_ptr->pval])
			{
				borg_note(format("# Searching a '%s' at (%d,%d)",
								 k_name + k_info[mb_ptr->object].name, y, x));

				/* Walk onto it */
				borg_keypress('0');
				borg_keypress('5');
				borg_keypress('s');
				return (TRUE);
			}

			/* Traps. Disarm it w/ fail check */
			if (o_ptr->pval >= 1 && object_known_p(o_ptr) &&
				borg_skill[BI_DEV] - o_ptr->pval >= 13)
			{
				borg_note(format("# Disarming a '%s' at (%d,%d)",
								 k_name + k_info[mb_ptr->object].name, y, x));

				/* Open it */
				borg_keypress('D');
				borg_keypress(I2D(dir));
				return (TRUE);
			}

			/* No trap, or unknown trap that passed above checks - Open it */
			/* if (o_ptr->pval < 0 || !object_known_p(o_ptr)) */
			{
				borg_note(format("# Opening a '%s' at (%d,%d)",
								 k_name + k_info[mb_ptr->object].name, y, x));

				/* Open it */
				borg_keypress('o');
				borg_keypress(I2D(dir));
				return (TRUE);
			}

			/* Empty chest */
			/* continue in routine and pick it up */
		}
#endif /* 0 */

		/*** Handle other takes ***/
		/* Message */
		borg_note(format("# Walking onto a '%s' at (%d,%d)",
						 k_name + k_info[mb_ptr->object].name, y, x));

		/* Walk onto it */
		borg_keypress(I2D(dir));
		return (TRUE);
	}

#if 0
	/* Glyph of Warding */
	if (mb_ptr->feat == FEAT_MINOR_GLYPH || mb_ptr->feat == FEAT_GLYPH)
	{
		/* Message */
		borg_note(format("# Walking onto a glyph of warding."));

		/* Walk onto it */
		borg_keypress(I2D(dir));
		return (TRUE);
	}
#endif /* 0 */

#if 0
	/* Traps -- disarm -- */
	if (borg_skill[BI_CUR_LITE] && !borg_skill[BI_ISBLIND] &&
		!borg_skill[BI_ISCONFUSED] && !scaryguy_on_level &&
		(mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) &&
		(mb_ptr->feat <= FEAT_TRAP_SLEEP))
	{

		/* NOTE: If a scary guy is on the level, we allow the borg to run over the
		 * trap in order to escape this level.
		 */

		/* allow "destroy doors" */
		if (borg_spell(REALM_ARCANE, 0, 6))
		{
			borg_note("# Unbarring ways");
			borg_keypress(I2D(dir));
			return (TRUE);
		}
		/* allow "destroy doors" */
		if (borg_spell(REALM_CHAOS, 0, 1))
		{
			borg_note("# Unbarring ways");
			return (TRUE);
		}

		/* Disarm */
		borg_note("# Disarming a trap");
		borg_keypress('D');
		borg_keypress(I2D(dir));

		/* We are not sure if the trap will get 'untrapped'. pretend it will */
		mb_ptr->feat = FEAT_NONE;
		return (TRUE);
	}
#endif /* 0 */

	/* Closed Doors -- Open */
	if (mb_ptr->feat == FEAT_CLOSED)
	{
		/* Paranoia XXX XXX XXX */
		if (one_in_(100)) return (FALSE);

		/* Open */
		if (my_need_alter)
		{
			borg_keypress('+');
			my_need_alter = FALSE;
		}
		else
		{
			borg_note("# Opening a door");
			borg_keypress('0');
			borg_keypress('9');
			borg_keypress('o');
		}
		borg_keypress(I2D(dir));
		return (TRUE);
	}


#if 0							/* Need to use fields */
	/* Jammed Doors -- Bash or destroy */
	if ((mb_ptr->feat >= FEAT_DOOR_HEAD + 0x08) &&
		(mb_ptr->feat <= FEAT_DOOR_TAIL))
	{
		/* Paranoia XXX XXX XXX */
		if (one_in_(100)) return (FALSE);

		/* Not if hungry */
		if (borg_skill[BI_ISWEAK]) return (FALSE);

		if (borg_spell(REALM_ARCANE, 0, 6) | borg_spell(REALM_CHAOS, 0, 1))
		{
			borg_note("# Destroying doors");
			return (TRUE);
		}

		/* Mega-Hack -- allow "stone to mud" */
		if (borg_spell(REALM_SORCERY, 1, 8) ||
			borg_spell(REALM_ARCANE, 2, 4) ||
			borg_spell(REALM_NATURE, 1, 0) ||
			borg_spell(REALM_CHAOS, 2, 3) || borg_racial(RACE_HALF_GIANT))
		{
			borg_note("# Melting a door");
			borg_keypress(I2D(dir));
			return (TRUE);
		}

		/* Bash */
		borg_note("# Bashing a door");
		borg_keypress('B');
		borg_keypress(I2D(dir));
		return (TRUE);
	}
#endif /* 0 */

	/* Rubble, Treasure, Seams, Walls -- Tunnel or Melt */
	if (mb_ptr->feat >= FEAT_PILLAR && mb_ptr->feat <= FEAT_WALL_SOLID)
	{

		/* Mega-Hack -- prevent infinite loops */
		if (randint0(100) < 10) return (FALSE);

		/* Not if hungry */
		if (borg_skill[BI_ISWEAK]) return (FALSE);

		/* Mega-Hack -- allow "stone to mud" */
		if (borg_spell(REALM_SORCERY, 1, 8) ||
			borg_spell(REALM_ARCANE, 2, 4) ||
			borg_spell(REALM_NATURE, 1, 0) ||
			borg_spell(REALM_CHAOS, 2, 3) || borg_racial(RACE_HALF_GIANT))
		{
			borg_note("# Melting a wall");
			borg_keypress(I2D(dir));
			return (TRUE);
		}

		/* No tunneling if in danger */
		if (borg_danger(c_y, c_x, 1, TRUE) >=
			borg_skill[BI_CURHP] / 4) return (FALSE);

		/* Tunnel */
		/* If I have a shovel then use it */
		if (borg_items[weapon_swap].tval == TV_DIGGING &&
			!(borg_items[INVEN_WIELD].cursed))
		{
			borg_note("# Swapping Digger");
			borg_keypress(ESCAPE);
			borg_keypress('w');
			borg_keypress(I2A(weapon_swap));
			borg_keypress(' ');
			borg_keypress(' ');
		}
		borg_note("# Digging through wall/etc");
		borg_keypress('0');
		borg_keypress('9');
		borg_keypress('9');

		borg_keypress('T');
		borg_keypress(I2D(dir));
		return (TRUE);
	}

#if 0							/* Need to use fields */
	/* Shops -- Enter */
	if ((mb_ptr->feat >= FEAT_SHOP_HEAD) && (mb_ptr->feat <= FEAT_SHOP_TAIL))
	{
		/* Message */
		borg_note(format
				  ("# Entering a '%d' shop",
				   (mb_ptr->feat - FEAT_SHOP_HEAD) + 1));

		/* Enter the shop */
		borg_keypress(I2D(dir));
		return (TRUE);
	}
#endif /* 0 */

	/* Perhaps the borg could search for traps as he walks around level one. */
	if (borg_skill[BI_MAXCLEVEL] <= 3 && borg_skill[BI_CDEPTH] &&
		!borg_skill[BI_ISSEARCHING] && borg_needs_searching)
	{
		borg_keypress('S');
	}

	/* Turn off the searching if needed */
	if (!borg_needs_searching && borg_skill[BI_ISSEARCHING])
	{
		borg_keypress('S');
	}

	/* Walk in that direction */
	if (my_need_alter)
	{
		borg_keypress('+');
		my_need_alter = FALSE;
	}
	else
	{
		/* nothing */
	}
	borg_keypress(I2D(dir));


	/* Stand stairs up */
	if (goal_less)
	{
		/* Up stairs */
		if (mb_ptr->feat == FEAT_LESS)
		{
			/* Stand on stairs */
			goal_less = FALSE;

			/* Success */
			return (TRUE);
		}
	}



	/* Did something */
	return (TRUE);
}




/*
 * Act twitchy
 */
bool borg_twitchy(void)
{
	int dir;

	/* This is a bad thing */
	borg_note("# Twitchy!");

	/* try to phase out of it */
	if (amt_phase && borg_caution_phase(15, 2) &&
		(borg_spell_fail(REALM_ARCANE, 0, 4, 40) ||
		 borg_spell_fail(REALM_SORCERY, 0, 1, 40) ||
		 borg_spell_fail(REALM_TRUMP, 0, 0, 40) ||
		 borg_activate_artifact(ART_BELEGENNON, FALSE) ||
		 borg_activate_artifact(ART_COLANNON, FALSE) ||
		 borg_read_scroll(SV_SCROLL_PHASE_DOOR)))
	{
		/* We did something */
		return (TRUE);
	}
	/* Pick a random direction */
	dir = randint1(9);

	/* Hack -- set goal */
	g_x = c_x + ddx[dir];
	g_y = c_y + ddy[dir];

	/* Maybe alter */
	if (randint0(100) < 10 && dir != 5)
	{
		/* Send action (alter) */
		borg_keypress('+');

		/* Send direction */
		borg_keypress(I2D(dir));
	}

	/* Normally move */
	else
	{
		/* Send direction */
		borg_keypress(I2D(dir));
	}

	/* We did something */
	return (TRUE);
}





/*
 * Commit the current "flow"
 */
static bool borg_flow_commit(cptr who, int why)
{
	int cost;

	map_block *mb_ptr = map_loc(c_x, c_y);

	/* Cost of current grid */
	cost = mb_ptr->cost;

	/* Verify the total "cost" */
	if (cost >= 250) return (FALSE);

	/* Message */
	if (who) borg_note(format("# Flowing toward %s at cost %d", who, cost));

	/* Itterate over all grids */
	MAP_ITT_START (mb_ptr)
	{
		/* Obtain the "flow" information */
		mb_ptr->flow = mb_ptr->cost;
	}
	MAP_ITT_END;

	/* Save the goal type */
	goal = why;

	/* Success */
	return (TRUE);
}





/*
 * Attempt to take an optimal step towards the current goal location
 *
 * Note that the "borg_update()" routine notices new monsters and objects,
 * and movement of monsters and objects, and cancels any flow in progress.
 *
 * Note that the "borg_update()" routine notices when a grid which was
 * not thought to block motion is discovered to in fact be a grid which
 * blocks motion, and removes that grid from any flow in progress.
 *
 * When given multiple alternative steps, this function attempts to choose
 * the "safest" path, by penalizing grids containing embedded gold, monsters,
 * rubble, doors, traps, store doors, and even floors.  This allows the Borg
 * to "step around" dangerous grids, even if this means extending the path by
 * a step or two, and encourages him to prefer grids such as objects and stairs
 * which are not only interesting but which are known not to be invisible traps.
 *
 * XXX XXX XXX XXX This function needs some work.  It should attempt to
 * analyze the "local region" around the player and determine the optimal
 * choice of locations based on some useful computations.
 *
 * If it works, return TRUE, otherwise, cancel the goal and return FALSE.
 */
bool borg_flow_old(int why)
{
	int x, y;

	map_block *mb_ptr;

	/* Continue */
	if (goal == why)
	{
		int b_n = 0;

		int i, b_i = -1;

		int c, b_c;

		mb_ptr = map_loc(c_x, c_y);

		/* Flow cost of current grid */
		b_c = mb_ptr->flow * 10;

		/* Prevent loops */
		b_c = b_c - 5;


		/* Look around */
		for (i = 0; i < 8; i++)
		{
			/* Grid in that direction */
			x = c_x + ddx_ddd[i];
			y = c_y + ddy_ddd[i];

			/* Bounds checking */
			if (!map_in_bounds(x, y)) continue;

			/* Access the grid */
			mb_ptr = map_loc(x, y);

			/* Flow cost at that grid */
			c = mb_ptr->flow * 10;

			/* Never backtrack */
			if (c > b_c) continue;


			/* Notice new best value */
			if (c < b_c) b_n = 0;

			/* Apply the randomizer to equivalent values */
			if ((++b_n >= 2) && (randint0(b_n)))
			{
				continue;
			}
			/* Track it */
			b_i = i;
			b_c = c;
		}


		/* Try it */
		if (b_i >= 0)
		{
			/* Access the location */
			x = c_x + ddx_ddd[b_i];
			y = c_y + ddy_ddd[b_i];

			/* Attempt motion */
			if (borg_play_step(y, x)) return (TRUE);
		}

		/* Cancel goal */
		goal = 0;
	}

	/* Nothing to do */
	return (FALSE);
}




/*
 * Prepare to flee the level via stairs
 */
bool borg_flow_stair_both(int why)
{
	int i;

	/* None to flow to */
	if (!track_less_num && !track_more_num) return (FALSE);

	/* dont go down if hungry or low on food, unless fleeing a scary town */
	if ((!goal_fleeing && !borg_skill[BI_CDEPTH]) &&
		(borg_skill[BI_CUR_LITE] == 0 || borg_skill[BI_ISWEAK] ||
		 borg_skill[BI_ISHUNGRY] || borg_skill[BI_FOOD] < 2))
		return (FALSE);

	/* clear the possible searching flag */
	borg_needs_searching = FALSE;

	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue useful grids */
	for (i = 0; i < track_less_num; i++)
	{
		/* Enqueue the grid */
		borg_flow_enqueue_grid(track_less_y[i], track_less_x[i]);
	}

	/* Enqueue useful grids */
	for (i = 0; i < track_more_num; i++)
	{
		/* Enqueue the grid */
		borg_flow_enqueue_grid(track_more_y[i], track_more_x[i]);
	}

	/* Spread the flow */
	borg_flow_spread(250, TRUE, FALSE, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("stairs", why)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(why)) return (FALSE);

	/* Success */
	return (TRUE);
}




/*
 * Prepare to flow towards "up" stairs
 */
bool borg_flow_stair_less(int why)
{
	int i;

	/* None to flow to */
	if (!track_less_num) return (FALSE);

	/* Clear the flow codes */
	borg_flow_clear();

	/* clear the possible searching flag */
	borg_needs_searching = FALSE;

	/* Enqueue useful grids */
	for (i = 0; i < track_less_num; i++)
	{
		/* Enqueue the grid */
		borg_flow_enqueue_grid(track_less_y[i], track_less_x[i]);
	}

	if (borg_skill[BI_CLEVEL] > 35 || borg_skill[BI_CUR_LITE] == 0)
	{
		/* Spread the flow */
		borg_flow_spread(250, TRUE, FALSE, FALSE);
	}
	else
	{
		/* Spread the flow, No Optimize, Avoid */
		borg_flow_spread(250, FALSE, !borg_desperate, FALSE);
	}

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("up-stairs", why)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(why)) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Prepare to flow towards "down" stairs
 */
bool borg_flow_stair_more(int why)
{
	int i;

	/* None to flow to */
	if (!track_more_num) return (FALSE);

	/* if not fleeing do not go down unless safe */
	if (!goal_fleeing && (cptr)NULL != borg_prepared(borg_skill[BI_CDEPTH] + 1))
		return (FALSE);

	/* dont go down if hungry or low on food, unless fleeing a scary town */
	if (borg_skill[BI_CDEPTH] &&
		(borg_skill[BI_ISWEAK] || borg_skill[BI_ISHUNGRY] ||
		 borg_skill[BI_FOOD] < 2))
		return (FALSE);

	/* No diving if no light */
	if (borg_skill[BI_CUR_LITE] == 0) return (FALSE);

	/* don't head for the stairs if you are recalling,  */
	/* even if you are fleeing. */
	if (goal_recalling)
		return (FALSE);

	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue useful grids */
	for (i = 0; i < track_more_num; i++)
	{
		/* Enqueue the grid */
		borg_flow_enqueue_grid(track_more_y[i], track_more_x[i]);
	}

	/* Spread the flow */
	borg_flow_spread(250, TRUE, FALSE, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("down-stairs", why)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(why)) return (FALSE);

	/* Success */
	return (TRUE);
}

/*
 * Prepare to flow towards glyph of warding
 */
bool borg_flow_glyph(int why)
{
	int i;

	/* None to flow to */
	if (!track_glyph_num) return (FALSE);

	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue useful grids */
	for (i = 0; i < track_glyph_num; i++)
	{
		/* Enqueue the grid */
		borg_flow_enqueue_grid(track_glyph_y[i], track_glyph_x[i]);
	}

	/* Spread the flow */
	borg_flow_spread(250, TRUE, FALSE, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("glyph of warding", why)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(why)) return (FALSE);

	/* Success */
	return (TRUE);
}

/*
 * Prepare to flow towards Town Gates
 */
bool borg_flow_town_exit(int why)
{
	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue the grid */
	if (borg_skill[BI_TOWN_NUM] == 1)
	{
		/* Main town gate is 33, 130 */
		borg_flow_enqueue_grid(33, 130);
	}

	/* Enqueue the grid */
	if (borg_skill[BI_TOWN_NUM] == 2)
	{
		/* Main town gate, just outside of town */
		borg_flow_enqueue_grid(33, 112);
	}

	if (borg_skill[BI_TOWN_NUM] == 3)
	{
		/* Main town gate is */
		borg_flow_enqueue_grid(39, 77);
	}


/* This routine can be used to flow to any town special
 * such as the mayors office or the Whitehorse Inn or even
 * special town quests.
 */
	/* Spread the flow */
	borg_flow_spread(250, TRUE, FALSE, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("Town Gates", why)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(why)) return (FALSE);

	/* Success */
	return (TRUE);
}

/*
 * Prepare to flow towards light
 */
bool borg_flow_light(int why)
{
	int y, x, i;

	map_block *mb_ptr;

	/* reset counters */
	borg_glow_n = 0;
	i = 0;

	/* build the glow array */
	/* Scan map */
	MAP_ITT_START (mb_ptr)
	{
		/* Not a perma-lit, and not our spot. */
		if (!(mb_ptr->flags & MAP_GLOW)) continue;

		/* Get location */
		MAP_GET_LOC(x, y);

		/* keep count */
		borg_glow_y[borg_glow_n] = y;
		borg_glow_x[borg_glow_n] = x;
		borg_glow_n++;
	}
	MAP_ITT_END;

	/* None to flow to */
	if (!borg_glow_n) return (FALSE);

	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue useful grids */
	for (i = 0; i < borg_glow_n; i++)
	{
		/* Enqueue the grid */
		borg_flow_enqueue_grid(borg_glow_y[i], borg_glow_x[i]);
	}

	/* Spread the flow */
	borg_flow_spread(250, TRUE, FALSE, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("a lighted area", why)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(why)) return (FALSE);

	/* Success */
	return (TRUE);
}

/*
 * Prepare to "flow" towards any non-visited shop
 */
bool borg_flow_shop_visit(void)
{
	int i, x, y;

	/* Must be in town */
	if (borg_skill[BI_CDEPTH]) return (FALSE);

	/* Clear the flow codes */
	borg_flow_clear();

	/* Visit the shops */
	for (i = 0; i < track_shop_num; i++)
	{
		/* Must not be visited */
		if (borg_shops[i].when) continue;

#if 0
		/* if poisoned or bleeding skip non temples */
		if ((borg_skill[BI_ISCUT] || borg_skill[BI_ISPOISONED]) &&
			(i != BORG_TEMPLE && i != BORG_HOME)) continue;

		/* if starving--skip non food places */
		if (borg_skill[BI_ISWEAK] &&
			(i != BORG_GSTORE && i != BORG_HOME)) continue;

		/* if dark--skip non food places */
		if (borg_skill[BI_CUR_LITE] == 0 && (i != 0) &&
			borg_skill[BI_CLEVEL] >= 2) continue;
#endif

		/* Obtain the location */
		x = track_shop_x[i];
		y = track_shop_y[i];

		/* Hack -- Must be known and not under the player */
		if (!x || !y || ((c_x == x) && (c_y == y))) continue;

		/* Enqueue the grid */
		borg_flow_enqueue_grid(y, x);
	}

	/* Spread the flow */
	borg_flow_spread(250, TRUE, FALSE, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("all shops", GOAL_MISC)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_MISC)) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Prepare to "flow" towards a specific shop entry
 */
bool borg_flow_shop_entry(int i)
{
	int x, y;

	/* cptr name = (f_name + f_info[FEAT_SHOP_HEAD+i].name); */

	/* Must be in town */
	if (borg_skill[BI_CDEPTH]) return (FALSE);

	/* Obtain the location */
	x = track_shop_x[i];
	y = track_shop_y[i];

	/* Hack -- Must be known */
	if (!x || !y) return (FALSE);

	/* Hack -- re-enter a shop if needed */
	if ((x == c_x) && (y == c_y))
	{
		/* Note */
		borg_note("# Re-entering a shop");

		/* Enter the store */
		borg_keypress('5');

		/* Success */
		return (TRUE);
	}

	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue the grid */
	borg_flow_enqueue_grid(y, x);

	/* Spread the flow */
	borg_flow_spread(250, TRUE, FALSE, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("shop", GOAL_MISC)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_MISC)) return (FALSE);

	/* Success */
	return (TRUE);
}

/*
 * The borg can take a shot from a distance
 *
 */
static bool borg_has_distance_attack(void)
{
	int rad;
	int dam;

	borg_simulate = TRUE;

	/* XXX For now only line up Magic Missle shots */
	rad = 0;
	dam = (3 + ((borg_skill[BI_CLEVEL]) / 4)) * (4 + 1) / 2;
	if (borg_attack_aux_spell_bolt(REALM_CHAOS, 0, 0, rad, dam, GF_MISSILE) > 0)
		return TRUE;

	return FALSE;
}


/*
 * Take a couple of steps to line up a shot
 *
 */
bool borg_flow_kill_aim(bool viewable)
{
	int o_y, o_x;
	int s_c_y = c_y;
	int s_c_x = c_x;
	int i;

	/* Efficiency -- Nothing to kill */
	if (!borg_kills_cnt) return (FALSE);

	/* Sometimes we loop on this if we back  up to a point where */
	/* the monster is out of site */
	if (time_this_panel > 500) return (FALSE);

	/* If you can shoot from where you are, don't bother reaiming */
	if (borg_has_distance_attack()) return (FALSE);

	/* Consider each adjacent spot */
	for (o_x = -2; o_x <= 2; o_x++)
	{
		for (o_y = -2; o_y <= 2; o_y++)
		{
			/* borg_attack would have already checked
			   for a shot from where I currently am */
			if (o_x == 0 && o_y == 0)
				continue;

			/* XXX  Mess with where the program thinks the
			   player is */
			c_x = s_c_x + o_x;
			c_y = s_c_y + o_y;

			/* avoid screen edgeds */
			if (!map_in_bounds(c_x + 2, c_y + 2)) continue;
			if (!map_in_bounds(c_x - 2, c_y - 2)) continue;

			/* Make sure we do not end up next to a monster */
			for (i = 0; i < borg_temp_n; i++)
			{
				if (distance(c_y, c_x, borg_temp_y[i], borg_temp_x[i]) == 1)
					break;
			}
			if (i != borg_temp_n)
				continue;

			/* Check for a distance attack from here */
			if (borg_has_distance_attack())
			{
				/* Clear the flow codes */
				borg_flow_clear();

				/* Enqueue the grid */
				borg_flow_enqueue_grid(c_y, c_x);

				/* restore the saved player position */
				c_x = s_c_x;
				c_y = s_c_y;

				/* Spread the flow */
				borg_flow_spread(5, TRUE, !viewable, FALSE);

				/* Attempt to Commit the flow */
				if (!borg_flow_commit("targetable position", GOAL_KILL))
					return (FALSE);

				/* Take one step */
				if (!borg_flow_old(GOAL_KILL)) return (FALSE);

				return (TRUE);
			}
		}
	}

	/* restore the saved player position */
	c_x = s_c_x;
	c_y = s_c_y;

	return FALSE;
}

/*
 * Dig an anti-summon corridor
 *
 *            ############## We want the borg to not dig #1
 *            #............# but to dig #2, and hopefully shoot from the
 *      #######............# last #2 and try to avoid standing on #3.
 *      #222223............# This is great for offset ball attacks but
 *      #2#####..s.........# not for melee.  Warriors need to dig a wall
 * ######2###########+###### adjacent to the monsters so he can swing on them.
 * #            1     #
 * # ################ #
 *   #              # #
 * ###              # #
 *
 */
bool borg_flow_kill_corridor(bool viewable)
{
#if 0
	int o_y, o_x;
	int m_x, m_y;
	int f_y, f_x;
	int floors = 0;
	int b_y = 0, b_x = 0;
	int perma_grids = 0;

	borg_kill *kill;

#endif /* 0 */

	/* Hack - ignore usused parameter */
	(void)viewable;

	/* Efficiency -- Nothing to kill */
	if (!borg_kills_cnt) return (FALSE);
	
	/* Need to do this properly */
#if 0

	/* Only do this to summoners when they are close */
	if (borg_kills_summoner == -1) return (FALSE);

	/* Do not dig when weak. It takes too long */
	if (borg_skill[BI_STR] < 17) return (FALSE);

	/* Sometimes we loop on this */
	if (time_this_panel > 500) return (FALSE);

	/* Do not dig when confused */
	if (borg_skill[BI_ISCONFUSED]) return (FALSE);

	/* Not when darkened */
	if (borg_skill[BI_CUR_LITE] == 0) return (FALSE);

	/* get the summoning monster */
	kill = &borg_kills[borg_kills_summoner];

	/* Consider each adjacent spot to monster */
	for (o_x = -1; o_x <= 1; o_x++)
	{
		for (o_y = -1; o_y <= 1; o_y++)
		{
			map_block *mb_ptr;

			/* Check grids near monster */
			m_x = kill->x + o_x;
			m_y = kill->y + o_y;

			/* avoid screen edges */
			if (!map_in_bounds(m_x + 2, m_y + 2)) continue;
			if (!map_in_bounds(m_x - 2, m_y - 2)) continue;

			/* Bounds checking */
			if (!map_in_bounds(m_x, m_y)) continue;

			/* get the grid */
			mb_ptr = map_loc(m_x, m_y);

			/* Can't tunnel a non wall or permawall */
			if (mb_ptr->feat >= FEAT_FLOOR &&
				mb_ptr->feat < FEAT_MAGMA) continue;
			if (mb_ptr->feat >= FEAT_PERM_EXTRA &&
				mb_ptr->feat <= FEAT_PERM_SOLID)
			{
				perma_grids++;
				continue;
			}

			/* Do not dig unless we appear strong enough to succeed or we have a digger */
			if (borg_spell_legal(REALM_SORCERY, 1, 8) ||
				borg_spell_legal(REALM_ARCANE, 2, 4) ||
				borg_spell_legal(REALM_NATURE, 1, 0) ||
				borg_spell_legal(REALM_CHAOS, 2, 3) ||
				borg_racial_check(RACE_HALF_GIANT, TRUE) ||
				borg_skill[BI_DIG] > (borg_skill[BI_CDEPTH] > 80 ? 30 : 40))
			{
				/* digging ought to work */
			}
			else
			{
				/* do not try digging */
				continue;
			}

			/* reset floors counter */
			floors = 0;

			/* That grid must not have too many floors adjacent */
			for (f_x = -1; f_x <= 1; f_x++)
			{
				for (f_y = -1; f_y <= 1; f_y++)
				{
					/* Bounds checking */
					if (!map_in_bounds(m_x + f_x, m_y + f_y)) continue;

					/* grid the grid */
					mb_ptr = map_loc(m_x + f_x, m_y + f_y);

					/* check if this neighbor is a floor */
					if (borg_cave_floor_grid(mb_ptr)) floors++;
				}
			}

			/* Do not dig if too many floors near. */
			if (floors >= 5) continue;

			/* Track the good location */
			b_y = m_y;
			b_x = m_x;
		}
	}
	/* NOTE: Perma_grids count the number of grids which contain permawalls.
	 * The borg may try to flow to an unknown grid but may get stuck on a perma
	 * wall.  This will keep him from flowing to a summoner if the summoner is
	 * near a perma grid.  The real fix out to be in the flow_spread so that
	 * he will not flow through perma_grids.  I will work on that next.
	 */
	if (b_y != 0 && b_x != 0 && perma_grids == 0)
	{
		/* Clear the flow codes */
		borg_flow_clear();

		/* Enqueue the grid */
		borg_flow_enqueue_grid(m_y, m_x);

		/* Spread the flow */
		borg_flow_spread(15, TRUE, FALSE, TRUE);

		/* Attempt to Commit the flow */
		if (!borg_flow_commit("anti-summon corridor", GOAL_KILL))
			return (FALSE);

		/* Take one step */
		if (!borg_flow_old(GOAL_KILL)) return (FALSE);

		return (TRUE);
	}
#endif /* 0 */
	return FALSE;
}



/*
 * Prepare to "flow" towards monsters to "kill"
 * But in a few phases, viewable, near and far.
 * Note that monsters under the player are always deleted
 */
bool borg_flow_kill(bool viewable, int nearness)
{
	int i, x, y, p, j, b_j = -1;
	int b_stair = -1;

	bool borg_in_hall = FALSE;
	int hall_y, hall_x, hall_walls = 0;
	bool skip_monster = FALSE;

	map_block *mb_ptr;

	/* Efficiency -- Nothing to kill */
	if (!borg_kills_cnt) return (FALSE);

	/* Don't chase down town monsters when you are just starting out */
	if (borg_skill[BI_CDEPTH] == 0 && borg_skill[BI_CLEVEL] < 7) return (FALSE);

	/* YOU ARE NOT A WARRIOR!! DON'T ACT LIKE ONE!! */
	if (borg_class == CLASS_MAGE &&
		borg_skill[BI_CLEVEL] <
		(borg_skill[BI_CDEPTH] ? 35 : 5)) return (FALSE);


	/* Nothing found */
	borg_temp_n = 0;

	/* check to see if in a hall, used later */
	for (hall_x = -1; hall_x <= 1; hall_x++)
	{
		for (hall_y = -1; hall_y <= 1; hall_y++)
		{
			/* Acquire location */
			x = hall_x + c_x;
			y = hall_y + c_y;

			/* Bounds checking */
			if (!map_in_bounds(x, y)) continue;

			mb_ptr = map_loc(x, y);

			/* track walls */
			if (				/* (mb_ptr->feat == FEAT_GLYPH) ||
								   (mb_ptr->feat == FEAT_MINOR_GLYPH) || */
				   ((mb_ptr->feat >= FEAT_MAGMA) &&
					(mb_ptr->feat <= FEAT_PERM_SOLID)))
			{
				hall_walls++;
			}

			/* addem up */
			if (hall_walls >= 5) borg_in_hall = TRUE;
		}
	}


	/* Check distance away from stairs, used later */

	/* Check for an existing "up stairs" */
	for (i = 0; i < track_less_num; i++)
	{
		x = track_less_x[i];
		y = track_less_y[i];

		/* How far is the nearest up stairs */
		j = distance(c_y, c_x, y, x);

		/* skip the closer ones */
		if (b_j >= j) continue;

		/* track it */
		b_j = j;
		b_stair = i;
	}

	/* Scan the monster list */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		borg_kill *kill = &borg_kills[i];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Ignore multiplying monsters */
		if (goal_ignoring && !borg_skill[BI_ISAFRAID] &&
			(r_info[kill->r_idx].flags2 & RF2_MULTIPLY)) continue;

		/* Avoid fighting if a scary guy is on the level */
		if (scaryguy_on_level) continue;

		/* Don't chase our friends or pets */
		if (kill->friendly) continue;

		/* Avoid multiplying monsters when low level */
		if (borg_skill[BI_CLEVEL] < 10 &&
			(r_info[kill->r_idx].flags2 & RF2_MULTIPLY)) continue;

		/* Hack -- ignore Maggot until later.  Player will chase Maggot
		 * down all accross the screen waking up all the monsters.  Then
		 * he is stuck in a comprimised situation.
		 */
		if ((r_info[kill->r_idx].flags1 & RF1_UNIQUE) &&
			borg_skill[BI_CDEPTH] == 0 && borg_skill[BI_CLEVEL] < 5) continue;

		/* Access the location */
		x = kill->x;
		y = kill->y;

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Get the grid */
		mb_ptr = map_loc(x, y);

		/* Require line of sight if requested */
		if (viewable && !(mb_ptr->info & BORG_MAP_VIEW)) continue;

		/* Calculate danger */
		borg_full_damage = FALSE;
		p = borg_danger_aux(y, x, 1, i, TRUE);
		borg_full_damage = FALSE;


		/* Hack -- Skip "deadly" monsters unless uniques */
		if (borg_skill[BI_CLEVEL] > 15 && (!r_info->flags1 & RF1_UNIQUE) &&
			p > avoidance / 2) continue;
		if (borg_skill[BI_CLEVEL] <= 15 && p > avoidance / 3) continue;

		/* Skip ones that make me wander too far */
		if (b_stair != -1 && borg_skill[BI_CLEVEL < 10])
		{
			/* Check the distance of this monster to the stair */
			j = distance(track_less_y[b_stair], track_less_x[b_stair], y, x);
			/* skip far away monsters while I am close to stair */
			if (b_j <= borg_skill[BI_CLEVEL] * 5 + 9 &&
				j >= borg_skill[BI_CLEVEL] * 5 + 9) continue;
		}

		/* Hack -- Avoid getting surrounded */
		if (borg_in_hall && (r_info[kill->r_idx].flags1 & RF1_FRIENDS))
		{
			/* check to see if monster is in a hall, */
			for (hall_x = -1; hall_x <= 1; hall_x++)
			{
				for (hall_y = -1; hall_y <= 1; hall_y++)
				{
					/* Bounds checking */
					if (!map_in_bounds(hall_x + x, hall_y + y)) continue;

					mb_ptr = map_loc(hall_x + x, hall_y + y);

					/* track walls */
					if (		/* (mb_ptr->feat == FEAT_GLYPH) ||
								   (mb_ptr->feat == FEAT_MINOR_GLYPH) || */
						   ((mb_ptr->feat >= FEAT_MAGMA) &&
							(mb_ptr->feat <= FEAT_PERM_SOLID)))
					{
						hall_walls++;
					}

					/* we want the monster to be in a hall also
					 *
					 *  ########################
					 *  ############      S  ###
					 *  #         @'     SSS ###
					 *  # ##########       SS###
					 *  # #        #       Ss###
					 *  # #        ###### ######
					 *  # #             # #
					 * Currently, we would like the borg to avoid
					 * flowing to a situation like the one above.
					 * We would like him to stay in the hall and
					 * attack from a distance.  One problem is the
					 * lower case 's' in the corner, He will show
					 * up as being in a corner, and the borg may
					 * flow to it.  Let's hope that is a rare case.
					 *
					 * The borg might flow to the 'dark' south exit
					 * of the room.  This would be dangerous for
					 * him as well.
					 */
					/* add 'em up */
					if (hall_walls < 4)
					{
						/* This monster is not in a hallway.
						 * It may not be safe to fight.
						 */
						skip_monster = TRUE;
					}
				}
			}
		}
		/* skip certain ones */
		if (skip_monster) continue;

		/* Careful -- Remember it */
		borg_temp_x[borg_temp_n] = x;
		borg_temp_y[borg_temp_n] = y;
		borg_temp_n++;
	}

	/* Nothing to kill */
	if (!borg_temp_n) return (FALSE);


	/* Clear the flow codes */
	borg_flow_clear();

	/* Look for something to kill */
	for (i = 0; i < borg_temp_n; i++)
	{
		/* Enqueue the grid */
		borg_flow_enqueue_grid(borg_temp_y[i], borg_temp_x[i]);
	}

	/* Spread the flow */
	/* if we are not flowing toward monsters that we can see, make sure they */
	/* are at least easily reachable.  The second flag is whether or not */
	/* to avoid unknown squares.  This was for performance when we have ESP. */
	borg_flow_spread(nearness, TRUE, !viewable, FALSE);


	/* Attempt to Commit the flow */
	if (!borg_flow_commit("kill", GOAL_KILL)) return (FALSE);


	/* Take one step */
	if (!borg_flow_old(GOAL_KILL)) return (FALSE);


	/* Success */
	return (TRUE);
}



/*
 * Prepare to "flow" towards objects to "take"
 *
 * Note that objects under the player are always deleted
 */
bool borg_flow_take(bool viewable, int nearness)
{
	int i, x, y;
	int b_stair = -1, j, b_j = -1;

	map_block *mb_ptr;

	/* Efficiency -- Nothing to take */
	if (!borg_takes_cnt) return (FALSE);

	/* Require one empty slot */
	if (borg_items[INVEN_PACK - 1].iqty) return (FALSE);

	/* Nothing yet */
	borg_temp_n = 0;

	/* Set the searching flag for low level borgs */
	borg_needs_searching = TRUE;

	/* Check distance away from stairs, used later */
	/* Check for an existing "up stairs" */
	for (i = 0; i < track_less_num; i++)
	{
		x = track_less_x[i];
		y = track_less_y[i];

		/* How far is the nearest up stairs */
		j = distance(c_y, c_x, y, x);

		/* skip the closer ones */
		if (b_j >= j) continue;

		/* track it */
		b_j = j;
		b_stair = i;
	}

	/* Scan the object list */
	for (i = 1; i < borg_takes_nxt; i++)
	{
		borg_take *take = &borg_takes[i];

		int a;
		bool item_bad;

		/* Skip dead objects */
		if (!take->k_idx) continue;

		/* Access the location */
		x = take->x;
		y = take->y;

		/* Skip ones that make me wander too far */
		if (b_stair != -1 && borg_skill[BI_CLEVEL < 10])
		{
			/* Check the distance of this 'take' to the stair */
			j = distance(track_less_y[b_stair], track_less_x[b_stair], y, x);

			/* skip far away takes while I am close to stair */
			if (b_j <= borg_skill[BI_CLEVEL] * 5 + 9 &&
				j >= borg_skill[BI_CLEVEL] * 5 + 9) continue;
		}

		/* look to see if this is on the bad items list */
		item_bad = FALSE;
		for (a = 0; a < 50; a++)
		{
			if (x == bad_obj_x[a] && y == bad_obj_y[a])
				item_bad = TRUE;
		}

		/* it is a bad item, do not track it */
		if (item_bad) continue;

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Get the grid */
		mb_ptr = map_loc(x, y);

		/* Require line of sight if requested */
		if (viewable && !(mb_ptr->info & BORG_MAP_VIEW)) continue;

		/* Careful -- Remember it */
		borg_temp_x[borg_temp_n] = x;
		borg_temp_y[borg_temp_n] = y;
		borg_temp_n++;
	}

	/* Nothing to take */
	if (!borg_temp_n) return (FALSE);


	/* Clear the flow codes */
	borg_flow_clear();

	/* Look for something to take */
	for (i = 0; i < borg_temp_n; i++)
	{
		/* Enqueue the grid */
		borg_flow_enqueue_grid(borg_temp_y[i], borg_temp_x[i]);
	}

	/* Spread the flow */
	/* if we are not flowing toward items that we can see, make sure they */
	/* are at least easily reachable.  The second flag is weather or not  */
	/* to avoid unkown squares.  This was for performance. */
	borg_flow_spread(nearness, TRUE, !viewable, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("item", GOAL_TAKE)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_TAKE)) return (FALSE);

	/* Success */
	return (TRUE);
}



/*
 * Determine if a grid is "interesting" (and should be explored)
 *
 * A grid is "interesting" if it is a closed door, rubble, hidden treasure,
 * or a visible trap, or an "unknown" grid.
 * or a non-perma-wall adjacent to a perma-wall. (GCV)
 *
 * b_stair is the index to the closest upstairs.
 */
static bool borg_flow_dark_interesting(int y, int x, int b_stair)
{
	int oy;
	int ox, i;

	map_block *mb_ptr;

	/* Hack ignore parameter */
	(void)b_stair;

	/* Have the borg so some Searching */
	borg_needs_searching = TRUE;

#if 0
	/* Skip ones that make me wander too far */
	if (b_stair != -1 && borg_skill[BI_CLEVEL < 10])
	{
		/* Check the distance of this grid to the stair */
		j = distance(track_less_y[b_stair], track_less_x[b_stair], y, x);
		/* Distance of me to the stairs */
		b_j = distance(c_y, c_x, track_less_y[b_stair], track_less_x[b_stair]);

		/* skip far away grids while I am close to stair */
		if (b_j <= borg_skill[BI_CLEVEL] * 5 + 9 &&
			j >= borg_skill[BI_CLEVEL] * 5 + 9) return (FALSE);
	}
#endif /* 0 */

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (TRUE);

	/* Get the grid */
	mb_ptr = map_loc(x, y);

	/* Explore unknown grids */
	if (!mb_ptr->feat) return (TRUE);

	/* Efficiency -- Ignore "boring" grids */
	if (mb_ptr->feat < FEAT_CLOSED) return (FALSE);

	/* Explore "known treasure" */
	if ((mb_ptr->feat == FEAT_MAGMA_K) || (mb_ptr->feat == FEAT_QUARTZ_K))
	{
		/* Do not disarm when confused */
		if (borg_skill[BI_ISCONFUSED]) return (FALSE);

		/* Do not bother if super rich */
		if (borg_gold >= 1000000) return (FALSE);

		/* Not when darkened */
		if (borg_skill[BI_CUR_LITE] == 0) return (FALSE);

		/* Allow "stone to mud" ability */
		if (borg_spell_legal(REALM_SORCERY, 1, 8) ||
			borg_spell_legal(REALM_ARCANE, 2, 4) ||
			borg_spell_legal(REALM_NATURE, 1, 0) ||
			borg_spell_legal(REALM_CHAOS, 2, 3) ||
			borg_racial(RACE_HALF_GIANT)) return (TRUE);

		/* Do not dig unless we appear strong enough to succeed or we have a digger */
		if (borg_skill[BI_DIG] > 40)
		{
			/* digging ought to work */
		}
		else
		{
			return (FALSE);
		}

		/* Okay */
		return (TRUE);
	}

#if 0
	/* "Vaults" Explore non perma-walls adjacent to a perma wall */
	if (mb_ptr->feat == FEAT_WALL_EXTRA || mb_ptr->feat == FEAT_MAGMA ||
		mb_ptr->feat == FEAT_QUARTZ)
	{
		/* Do not attempt when confused */
		if (borg_skill[BI_ISCONFUSED]) return (FALSE);

		/* hack and cheat.  No vaults  on this level */
		if (!vault_on_level) return (FALSE);

		/* AJG Do not attempt on the edge */
		if (map_in_bounds(x, y))
		{
			/* scan the adjacent grids */
			for (ox = -1; ox <= 1; ox++)
			{
				for (oy = -1; oy <= 1; oy++)
				{
					/* Bounds checking */
					if (!map_in_bounds(ox + x, oy + y)) continue;

					/* Acquire location */
					mb_ptr = map_loc(ox + x, oy + y);

					/* skip non perma grids wall */
					if (mb_ptr->feat != FEAT_PERM_INNER) continue;

					/* Allow "stone to mud" ability */
					if (borg_spell_legal(REALM_SORCERY, 1, 8) ||
						borg_spell_legal(REALM_ARCANE, 2, 4) ||
						borg_spell_legal(REALM_NATURE, 1, 0) ||
						borg_spell_legal(REALM_CHAOS, 0, 6) ||
						borg_equips_artifact(ART_OROME, INVEN_WIELD) ||
						borg_racial_check(RACE_HALF_GIANT, TRUE)) return (TRUE);

					/* Do not dig unless we appear strong enough to succeed or we have a digger */
					if (borg_skill[BI_DIG] > 40)
					{
						/* digging ought to work, proceed */
					}
					else
					{
						continue;
					}
					if (borg_skill[BI_DIG] < 40) return (FALSE);

					/* Glove up and dig in */
					return (TRUE);
				}
			}
		}

		/* not adjacent to a GCV,  Restore Grid */
		mb_ptr = map_loc(x, y);
	}
#endif /* 0 */

	/* Explore "rubble" */
	if (mb_ptr->feat == FEAT_RUBBLE)
	{
		return (TRUE);
	}

	/* Explore "Trees" somewhat */
	if (mb_ptr->feat == FEAT_TREES)
	{
		/* Scan near trees for unknown grids */

		/* AJG Do not attempt on the edge */
		if (map_in_bounds(x, y))
		{
			/* scan the adjacent grids */
			for (ox = -1; ox <= 1; ox++)
			{
				for (oy = -1; oy <= 1; oy++)
				{
					/* Bounds checking */
					if (!map_in_bounds(ox + x, oy + y)) continue;

					/* Acquire location */
					mb_ptr = map_loc(ox + x, oy + y);

					/* look for Unknown grid */
					if (!mb_ptr->feat) return (TRUE);
				}
			}
		}

		/* this forest is already explored */
		return (FALSE);
	}


	/* Explore "closed doors" */
	if (mb_ptr->feat == FEAT_CLOSED)
	{
		/* some closed doors leave alone */
		if (breeder_level)
		{
			/* Did I close this one */
			for (i = 0; i < track_door_num; i++)
			{
				/* mark as icky if I closed this one */
				if ((track_door_x[i] == x) && (track_door_y[i] == y))
				{
					/* not interesting */
					return (FALSE);
				}
			}

		}
		/* this door should be ok to open */
		return (TRUE);
	}

#if 0
	/* Explore "visible traps" */
	if ((mb_ptr->feat >= FEAT_TRAP_TRAPDOOR) &&
		(mb_ptr->feat <= FEAT_TRAP_SLEEP))
	{
		/* Do not disarm when blind */
		if (borg_skill[BI_ISBLIND]) return (FALSE);

		/* Do not disarm when confused */
		if (borg_skill[BI_ISCONFUSED]) return (FALSE);

		/* Do not disarm when hallucinating */
		if (borg_skill[BI_ISIMAGE]) return (FALSE);

		/* Do not flow without lite */
		if (borg_skill[BI_CUR_LITE] == 0) return (FALSE);

		/* Do not disarm trap doors on level 99 */
		if (borg_skill[BI_CDEPTH] == 99 &&
			mb_ptr->feat == FEAT_TRAP_TRAPDOOR) return (FALSE);

		/* Do not disarm when you could end up dead */
		if (borg_skill[BI_CURHP] < 60) return (FALSE);

		/* Do not disarm when clumsy */
		if (borg_skill[BI_DIS] < 30 &&
			borg_skill[BI_CLEVEL] < 20) return (FALSE);
		if (borg_skill[BI_DIS] < 45 &&
			borg_skill[BI_CLEVEL] < 10) return (FALSE);

		/* NOTE: the flow code allows a borg to flow through a trap and so he may
		 * still try to disarm one on his way to the other interesting grid.  If mods
		 * are made to the above criteria for disarming traps, then mods must also be
		 * made to borg_flow_spread()
		 */

		/* Okay */
		return (TRUE);
	}
#endif /* 0 */

	/* Ignore other grids */
	return (FALSE);
}


/*
 * Determine if a grid is "reachable" (and can be explored)
 */
static bool borg_flow_dark_reachable(int y, int x)
{
	int j;

	map_block *mb_ptr;

	/* Scan neighbors */
	for (j = 0; j < 8; j++)
	{
		int y2 = y + ddy_ddd[j];
		int x2 = x + ddx_ddd[j];

		/* Bounds checking */
		if (!map_in_bounds(x2, y2)) continue;

		/* Get the grid */
		mb_ptr = map_loc(x2, y2);


		/* Skip unknown grids (important) */
		if (!mb_ptr->feat) continue;

		/* Accept known floor grids */
		if (borg_cave_floor_grid(mb_ptr)) return (TRUE);

		/* Accept Trees too */
		if (mb_ptr->feat == FEAT_TREES) return (TRUE);

		/* Accept Lava if immune */
		if (mb_ptr->feat == FEAT_SHAL_LAVA &&
			borg_skill[BI_IFIRE]) return (TRUE);

		/* Accept Water if not drowning */
		if (mb_ptr->feat == FEAT_SHAL_WATER &&
			(!borg_skill[BI_ENCUMBERD] || borg_skill[BI_FEATH])) return (TRUE);

		/* I can push pass friendly monsters */
		if (mb_ptr->kill && borg_kills[mb_ptr->kill].friendly)
			return (TRUE);
	}

	/* Failure */
	return (FALSE);
}


/*
 * Place a "direct path" into the flow array, checking danger
 *
 * Modify the "cost" array in such a way that from any point on
 * one "direct" path from the player to the given grid, as long
 * as the rest of the path is "safe" and "clear", the Borg will
 * walk along the path to the given grid.
 *
 * This function is used by "borg_flow_dark_1()" to provide an
 * optimized "flow" during the initial exploration of a level.
 *
 * It is also used to move around town without looking like a drunk.
 */
extern void borg_flow_direct(int y, int x)
{
	int n = 0;

	int x1, y1, x2, y2;

	int ay, ax;

	int shift;

	map_block *mb_ptr;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return;

	mb_ptr = map_loc(x, y);

	/* Avoid icky grids */
	if (mb_ptr->info & BORG_MAP_ICKY) return;

	/* Unknown */
	if (!(mb_ptr->info & BORG_MAP_KNOW))
	{
		/* Mark as known */
		mb_ptr->info |= BORG_MAP_KNOW;

		/* Mark dangerous grids as icky */
		if (borg_danger(y, x, 1, TRUE) > avoidance / 3)
		{
			/* Icky */
			mb_ptr->info |= BORG_MAP_ICKY;


			/* Avoid */
			return;
		}
	}


	/* Save the flow cost (zero) */
	mb_ptr->cost = 1;


	/* Save "origin" */
	y1 = y;
	x1 = x;

	/* Save "destination" */
	y2 = c_y;
	x2 = c_x;

	/* Calculate distance components */
	ay = (y2 < y1) ? (y1 - y2) : (y2 - y1);
	ax = (x2 < x1) ? (x1 - x2) : (x2 - x1);

	/* Path */
	while (1)
	{
		/* Check for arrival at player */
		if ((x == x2) && (y == y2)) return;

		/* Next */
		n++;

		/* Move mostly vertically */
		if (ay > ax)
		{
			/* Extract a shift factor XXX */
			shift = (n * ax + (ay - 1) / 2) / ay;

			/* Sometimes move along the minor axis */
			x = (x2 < x1) ? (x1 - shift) : (x1 + shift);

			/* Always move along major axis */
			y = (y2 < y1) ? (y1 - n) : (y1 + n);
		}

		/* Move mostly horizontally */
		else
		{
			/* Extract a shift factor XXX */
			shift = (n * ay + (ax - 1) / 2) / ax;

			/* Sometimes move along the minor axis */
			y = (y2 < y1) ? (y1 - shift) : (y1 + shift);

			/* Always move along major axis */
			x = (x2 < x1) ? (x1 - n) : (x1 + n);
		}

		/* Bounds checking */
		if (!map_in_bounds(x, y)) return;

		/* Access the grid */
		mb_ptr = map_loc(x, y);

		if (borg_cave_wall_grid(mb_ptr))
		{
			/* Only like 'diggable' things */
			if (!((mb_ptr->feat >= FEAT_CLOSED) &&
				  (mb_ptr->feat <= FEAT_QUARTZ))) return;
		}

		/* Ignore certain "non-wall" grids */
		if ((mb_ptr->feat == FEAT_SHAL_WATER &&
			 (!borg_skill[BI_ENCUMBERD] &&
			  !borg_skill[BI_FEATH])) ||
			(mb_ptr->feat == FEAT_SHAL_LAVA && !borg_skill[BI_IFIRE])) return;

		/* Abort at "icky" grids */
		if (mb_ptr->info & BORG_MAP_ICKY) return;

		/* Analyze every grid once */
		if (!(mb_ptr->info & BORG_MAP_KNOW))
		{
			/* Mark as known */
			mb_ptr->info |= BORG_MAP_KNOW;

			/* Avoid dangerous grids (forever) */
			if (borg_danger(y, x, 1, TRUE) > avoidance / 3)
			{
				/* Mark as icky */
				mb_ptr->info |= BORG_MAP_ICKY;

				/* Abort */
				return;
			}
		}

		/* Abort "pointless" paths if possible */
		if (mb_ptr->cost <= n) break;

		/* Save the new flow cost */
		mb_ptr->cost = n;
	}

}

/* Currently not used, I thought I might need it for anti-summoning */
extern void borg_flow_direct_dig(int y, int x)
{
	int n = 0;

	int x1, y1, x2, y2;

	int ay, ax;

	int shift;

	map_block *mb_ptr;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return;

	mb_ptr = map_loc(x, y);

	/* Save the flow cost (zero) */
	mb_ptr->cost = 0;


	/* Save "origin" */
	y1 = y;
	x1 = x;

	/* Save "destination" */
	y2 = c_y;
	x2 = c_x;

	/* Calculate distance components */
	ay = (y2 < y1) ? (y1 - y2) : (y2 - y1);
	ax = (x2 < x1) ? (x1 - x2) : (x2 - x1);

	/* Path */
	while (1)
	{
		/* Check for arrival at player */
		if ((x == x2) && (y == y2)) return;

		/* Next */
		n++;

		/* Move mostly vertically */
		if (ay > ax)
		{
			/* Extract a shift factor XXX */
			shift = (n * ax + (ay - 1) / 2) / ay;

			/* Sometimes move along the minor axis */
			x = (x2 < x1) ? (x1 - shift) : (x1 + shift);

			/* Always move along major axis */
			y = (y2 < y1) ? (y1 - n) : (y1 + n);
		}

		/* Move mostly horizontally */
		else
		{
			/* Extract a shift factor XXX */
			shift = (n * ay + (ax - 1) / 2) / ax;

			/* Sometimes move along the minor axis */
			y = (y2 < y1) ? (y1 - shift) : (y1 + shift);

			/* Always move along major axis */
			x = (x2 < x1) ? (x1 - n) : (x1 + n);
		}

		/* Bounds checking */
		if (!map_in_bounds(x, y)) return;

		/* Access the grid */
		mb_ptr = map_loc(x, y);


		/* Abort at "icky" grids */
		if (mb_ptr->info & BORG_MAP_ICKY) return;

		/* Analyze every grid once */
		if (!(mb_ptr->info & BORG_MAP_KNOW))
		{
			/* Mark as known */
			mb_ptr->info |= BORG_MAP_KNOW;

			/* Avoid dangerous grids (forever) */
			if (borg_danger(y, x, 1, TRUE) > avoidance / 3)
			{
				/* Mark as icky */
				mb_ptr->info |= BORG_MAP_ICKY;

				/* Abort */
				return;
			}
		}

		/* Abort "pointless" paths if possible */
		if (mb_ptr->cost <= n) break;

		/* Save the new flow cost */
		mb_ptr->cost = n;
	}
}



/*
 * Hack -- mark off the edges of a rectangle as "avoid" or "clear"
 */
static void borg_flow_border(int y1, int x1, int y2, int x2, bool stop)
{
	int x, y;

	map_block *mb_ptr;

	if (stop)
	{
		/* Scan west/east edges */
		for (y = y1; y <= y2; y++)
		{
			/* Avoid/Clear west edge */
			if (!map_in_bounds(x1, y)) continue;
			mb_ptr = map_loc(x1, y);
			mb_ptr->info |= (BORG_MAP_ICKY | BORG_MAP_KNOW);

			/* Avoid/Clear east edge */
			if (!map_in_bounds(x2, y)) continue;
			mb_ptr = map_loc(x2, y);
			mb_ptr->info |= (BORG_MAP_ICKY | BORG_MAP_KNOW);
		}

		/* Scan north/south edges */
		for (x = x1; x <= x2; x++)
		{
			/* Avoid/Clear north edge */
			if (!map_in_bounds(x, y1)) continue;
			mb_ptr = map_loc(x, y1);
			mb_ptr->info |= (BORG_MAP_ICKY | BORG_MAP_KNOW);

			/* Avoid/Clear south edge */
			if (!map_in_bounds(x, y2)) continue;
			mb_ptr = map_loc(x, y2);
			mb_ptr->info |= (BORG_MAP_ICKY | BORG_MAP_KNOW);
		}
	}
	else
	{
		/* Scan west/east edges */
		for (y = y1; y <= y2; y++)
		{
			/* Avoid/Clear west edge */
			if (!map_in_bounds(x1, y)) continue;
			mb_ptr = map_loc(x1, y);
			mb_ptr->info &= ~(BORG_MAP_ICKY | BORG_MAP_KNOW);

			/* Avoid/Clear east edge */
			if (!map_in_bounds(x2, y)) continue;
			mb_ptr = map_loc(x2, y);
			mb_ptr->info &= ~(BORG_MAP_ICKY | BORG_MAP_KNOW);
		}

		/* Scan north/south edges */
		for (x = x1; x <= x2; x++)
		{
			/* Avoid/Clear north edge */
			if (!map_in_bounds(x, y1)) continue;
			mb_ptr = map_loc(x, y1);
			mb_ptr->info &= ~(BORG_MAP_ICKY | BORG_MAP_KNOW);

			/* Avoid/Clear south edge */
			if (!map_in_bounds(x, y2)) continue;
			mb_ptr = map_loc(x, y2);
			mb_ptr->info &= ~(BORG_MAP_ICKY | BORG_MAP_KNOW);
		}
	}
}


/*
 * Prepare to "flow" towards "interesting" grids (method 1)
 *
 * This function examines the torch-lit grids for "interesting" grids.
 */
static bool borg_flow_dark_1(int b_stair)
{
	int i;

	int x, y;

	/* Ignore parameter */
	(void)b_stair;


	/* Hack -- not in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);

	/* Reset */
	borg_temp_n = 0;


	/* This is broken borg_lite_ no longer exists */
#if 0
	/* Scan torch-lit grids */
	for (i = 0; i < borg_lite_n; i++)
	{
		y = borg_lite_y[i];
		x = borg_lite_x[i];

		/* Skip "boring" grids (assume reachable) */
		if (!borg_flow_dark_interesting(y, x, b_stair)) continue;

		/* Careful -- Remember it */
		borg_temp_x[borg_temp_n] = x;
		borg_temp_y[borg_temp_n] = y;
		borg_temp_n++;
	}
#endif /* 0 */

	/* Nothing */
	if (!borg_temp_n) return (FALSE);


	/* Clear the flow codes */
	borg_flow_clear();

	/* Create paths to useful grids */
	for (i = 0; i < borg_temp_n; i++)
	{
		y = borg_temp_y[i];
		x = borg_temp_x[i];
#if 0
		/* Create a path */
		borg_flow_direct(y, x);
#endif /* 0 */

		borg_flow_enqueue_grid(y, x);

	}

	/* Spread the flow */
	borg_flow_spread(5, TRUE, FALSE, FALSE);


	/* Attempt to Commit the flow */
	/* Note was NULL */
	if (!borg_flow_commit("dark-1", GOAL_DARK)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_DARK)) return (FALSE);

	/* Forget goal */
	goal = 0;

	/* Success */
	return (TRUE);
}


/*
 * Prepare to "flow" towards "interesting" grids (method 2)
 *
 * This function is only used when the player is at least 4 grids away
 * from the outer dungeon wall, to prevent any nasty memory errors.
 *
 * This function examines the grids just outside the torch-lit grids
 * for "unknown" grids, and flows directly towards them (one step).
 */
static bool borg_flow_dark_2(void)
{
	int i, r;

	int x, y;

	map_block *mb_ptr;

	/* Hack -- not in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);

	/* Set the searching flag for low level borgs */
	borg_needs_searching = TRUE;

	/* Maximal radius */
	r = borg_skill[BI_CUR_LITE] + 1;


	/* Reset */
	borg_temp_n = 0;

	/* Four directions */
	for (i = 0; i < 4; i++)
	{
		y = c_y + ddy_ddd[i] * r;
		x = c_x + ddx_ddd[i] * r;

		/* Check legality */
		if (!map_in_bounds(x, y)) continue;

		/* Acquire grid */
		mb_ptr = map_loc(x, y);

		/* Require unknown */
		if (mb_ptr->feat) continue;

		/* Require viewable */
		if (!(mb_ptr->info & BORG_MAP_VIEW)) continue;

		/* if it makes me wander, skip it */

		/* Careful -- Remember it */
		borg_temp_x[borg_temp_n] = x;
		borg_temp_y[borg_temp_n] = y;
		borg_temp_n++;
	}

	/* Nothing */
	if (!borg_temp_n) return (FALSE);


	/* Clear the flow codes */
	borg_flow_clear();

	/* Create paths to useful grids */
	for (i = 0; i < borg_temp_n; i++)
	{
		y = borg_temp_y[i];
		x = borg_temp_x[i];
#if 0
		/* Create a path */
		borg_flow_direct(y, x);
#endif /* 0 */
		borg_flow_enqueue_grid(y, x);
	}

	/* Spread the flow */
	borg_flow_spread(5, TRUE, FALSE, FALSE);


	/* Attempt to Commit the flow */
	/* Note was NULL */
	if (!borg_flow_commit("dark-2", GOAL_DARK)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_DARK)) return (FALSE);

	/* Forget goal */
	goal = 0;

	/* Success */
	return (TRUE);
}


/*
 * Prepare to "flow" towards "interesting" grids (method 3)
 *
 * Note the use of a limit on the "depth" of the flow, and of the flag
 * which avoids "unknown" grids when calculating the flow, both of which
 * help optimize this function to only handle "easily reachable" grids.
 *
 * The "borg_temp" array is much larger than any "local region".
 */
static bool borg_flow_dark_3(int b_stair)
{
	int i;

	int x, y;

	int x1, y1, x2, y2;


	/* Hack -- not in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);


	/* Local region */
	y1 = c_y - 4;
	x1 = c_x - 4;
	y2 = c_y + 4;
	x2 = c_x + 4;

	/* Reset */
	borg_temp_n = 0;

	/* Examine the region */
	for (y = y1; y <= y2; y++)
	{
		/* Examine the region */
		for (x = x1; x <= x2; x++)
		{
			if (!map_in_bounds(x, y)) continue;

			/* Skip "boring" grids */
			if (!borg_flow_dark_interesting(y, x, b_stair)) continue;

			/* Skip "unreachable" grids */
			if (!borg_flow_dark_reachable(y, x)) continue;

			/* Careful -- Remember it */
			borg_temp_x[borg_temp_n] = x;
			borg_temp_y[borg_temp_n] = y;
			borg_temp_n++;
		}
	}

	/* Nothing interesting */
	if (!borg_temp_n) return (FALSE);


	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue useful grids */
	for (i = 0; i < borg_temp_n; i++)
	{
		y = borg_temp_y[i];
		x = borg_temp_x[i];

		/* Enqueue the grid */
		borg_flow_enqueue_grid(y, x);
	}

	/* Spread the flow (limit depth) */
	borg_flow_spread(5, TRUE, TRUE, FALSE);


	/* Attempt to Commit the flow */
	/* Note was NULL */
	if (!borg_flow_commit("dark-3", GOAL_DARK)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_DARK)) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Prepare to "flow" towards "interesting" grids (method 4)
 *
 * Note that we avoid grids close to the edge of the panel, since they
 * induce panel scrolling, which is "expensive" in terms of CPU usage,
 * and because this allows us to "expand" the border by several grids
 * to lay down the "avoidance" border in known legal grids.
 *
 * We avoid paths that would take us into different panels by setting
 * the "icky" flag for the "border" grids to prevent path construction,
 * and then clearing them when done, to prevent confusion elsewhere.
 *
 * The "borg_temp" array is large enough to hold one panel full of grids.
 */
static bool borg_flow_dark_4(int b_stair)
{
	int i, x, y;

	int x1, y1, x2, y2;


	/* Hack -- not in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);


	/* Local region */
	y1 = c_y - 11;
	x1 = c_x - 11;
	y2 = c_y + 11;
	x2 = c_x + 11;

	/* Nothing yet */
	borg_temp_n = 0;

	/* Examine the panel */
	for (y = y1; y <= y2; y++)
	{
		/* Examine the panel */
		for (x = x1; x <= x2; x++)
		{
			if (!map_in_bounds(x, y)) continue;

			/* Skip "boring" grids */
			if (!borg_flow_dark_interesting(y, x, b_stair)) continue;

			/* Skip "unreachable" grids */
			if (!borg_flow_dark_reachable(y, x)) continue;

			/* Careful -- Remember it */
			borg_temp_x[borg_temp_n] = x;
			borg_temp_y[borg_temp_n] = y;
			borg_temp_n++;
		}
	}

	/* Nothing useful */
	if (!borg_temp_n) return (FALSE);


	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue useful grids */
	for (i = 0; i < borg_temp_n; i++)
	{
		y = borg_temp_y[i];
		x = borg_temp_x[i];

		/* Enqueue the grid */
		borg_flow_enqueue_grid(y, x);
	}


	/* Expand borders */
	y1--;
	x1--;
	y2++;
	x2++;

	/* Avoid the edges */
	borg_flow_border(y1, x1, y2, x2, TRUE);

	/* Spread the flow (limit depth) */
	borg_flow_spread(32, TRUE, TRUE, FALSE);

	/* Clear the edges */
	borg_flow_border(y1, x1, y2, x2, FALSE);


	/* Attempt to Commit the flow */
	if (!borg_flow_commit("dark-4", GOAL_DARK)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_DARK)) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Prepare to "flow" towards "interesting" grids (method 5)
 */
static bool borg_flow_dark_5(int b_stair)
{
	int i, x, y;

	map_block *mb_ptr;

	/* Hack -- not in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);

	/* Nothing yet */
	borg_temp_n = 0;

	/* Examine every "legal" grid */
	MAP_ITT_START (mb_ptr)
	{
		/* Paranoia -- Check for overflow */
		if (borg_temp_n == AUTO_TEMP_MAX) continue;

		/* Get location */
		MAP_GET_LOC(x, y);

		/* Skip "boring" grids */
		if (!borg_flow_dark_interesting(y, x, b_stair)) continue;

		/* Skip "unreachable" grids */
		if (!borg_flow_dark_reachable(y, x)) continue;

		/* Careful -- Remember it */
		borg_temp_x[borg_temp_n] = x;
		borg_temp_y[borg_temp_n] = y;
		borg_temp_n++;
	}
	MAP_ITT_END;

	/* Nothing useful */
	if (!borg_temp_n) return (FALSE);


	/* Clear the flow codes */
	borg_flow_clear();

	/* Enqueue useful grids */
	for (i = 0; i < borg_temp_n; i++)
	{
		y = borg_temp_y[i];
		x = borg_temp_x[i];

		/* Enqueue the grid */
		borg_flow_enqueue_grid(y, x);
	}

	/* Spread the flow */
	borg_flow_spread(250, TRUE, TRUE, FALSE);


	/* Attempt to Commit the flow */
	if (!borg_flow_commit("dark-5", GOAL_DARK)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_DARK)) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Prepare to "flow" towards "interesting" grids
 *
 * The "exploration" routines are broken into "near" and "far"
 * exploration, and each set is chosen via the flag below.
 */
bool borg_flow_dark(bool close)
{
	int i;
	int x, y, j, b_j = -1;
	int b_stair = -1;

	/* Paranoia */
	if (borg_flow_dark_interesting(c_y, c_x, -1))
	{
		return (FALSE);
	}

	/* Check distance away from stairs, used later */
	/* Check for an existing "up stairs" */
	for (i = 0; i < track_less_num; i++)
	{
		x = track_less_x[i];
		y = track_less_y[i];

		/* How far is the nearest up stairs */
		j = distance(c_y, c_x, y, x);

		/* skip the closer ones */
		if (b_j >= j) continue;

		/* track it */
		b_j = j;
		b_stair = i;
	}

	/* Near */
	if (close)
	{
		/* Method 1 */
		if (borg_flow_dark_1(b_stair)) return (TRUE);

		/* Method 2 */
		if (borg_flow_dark_2()) return (TRUE);

		/* Method 3 */
		if (borg_flow_dark_3(b_stair)) return (TRUE);
	}

	/* Far */
	else
	{
		/* Method 4 */
		if (borg_flow_dark_4(b_stair)) return (TRUE);

		/* Method 5 */
		if (borg_flow_dark_5(b_stair)) return (TRUE);
	}

	/* Fail */
	return (FALSE);
}



/*
 * Hack -- spastic searching
 */

static byte spastic_x;
static byte spastic_y;



/*
 * Search carefully for secret doors and such
 */
bool borg_flow_spastic(bool bored)
{
	int cost;

	int i, x, y, v;

	int b_x = c_x;
	int b_y = c_y;
	int b_v = -1;
	int j, b_j = 1;
	int b_stair = -1;

	map_block *mb_ptr;

	/* Hack -- not in town */
	if (!borg_skill[BI_CDEPTH]) return (FALSE);


	/* Not bored */
	if (!bored)
	{
		/* Look around for danger */
		int p = borg_danger(c_y, c_x, 1, TRUE);

		/* Avoid searching when in danger */
		if (p > avoidance / 4) return (FALSE);
	}

	/* Check distance away from stairs, used later */
	/* Check for an existing "up stairs" */
	for (i = 0; i < track_less_num; i++)
	{
		x = track_less_x[i];
		y = track_less_y[i];

		/* How far is the nearest up stairs */
		j = distance(c_y, c_x, y, x);

		/* skip the closer ones */
		if (b_j >= j) continue;

		/* track it */
		b_j = j;
		b_stair = i;
	}

	/* We have arrived */
	if ((spastic_x == c_x) && (spastic_y == c_y))
	{
		/* Cancel */
		spastic_x = 0;
		spastic_y = 0;

		/* Take note */
		borg_note(format("# Spastic Searching at (%d,%d)...", c_x, c_y));

		/* Count searching */
		for (i = 0; i < 9; i++)
		{
			/* Extract the location */
			int xx = c_x + ddx_ddd[i];
			int yy = c_y + ddy_ddd[i];

			/* Bounds checking */
			if (!map_in_bounds(xx, yy)) continue;

			/* Current grid */
			mb_ptr = map_loc(xx, yy);

			/* Tweak -- Remember the search */
			if (mb_ptr->xtra < 100) mb_ptr->xtra += 5;
		}

		/* Tweak -- Search a little */
		borg_keypress('0');
		borg_keypress('5');
		borg_keypress('s');

		/* Success */
		return (TRUE);
	}


	/* Reverse flow */
	borg_flow_reverse();

	/* Scan the entire map */
	MAP_ITT_START (mb_ptr)
	{
		map_block *mb_array[8];

		int wall = 0;
		int supp = 0;
		int diag = 0;

		byte xtra_val;

		/* Skip unknown grids */
		if (!mb_ptr->feat) continue;

		/* Skip walls/doors */
		if (borg_cave_wall_grid(mb_ptr)) continue;

		/* Acquire the cost */
		cost = mb_ptr->cost;

		/* Skip "unreachable" grids */
		if (cost >= 250) continue;

		xtra_val = mb_ptr->xtra;

		/* Tweak -- Limit total searches */
		if (xtra_val >= 50) continue;
		if (xtra_val >= borg_skill[BI_CLEVEL] * 5) continue;

		/* Limit initial searches until bored */
		if (!bored && (xtra_val > 5)) continue;

		/* Acquire the location */
		MAP_GET_LOC(x, y);

		/* Avoid searching detected sectors */
		if (mb_ptr->detect & BORG_DETECT_DOOR) continue;

		/* Skip ones that make me wander too far */
		if (b_stair != -1 && borg_skill[BI_CLEVEL < 10])
		{
			/* Check the distance of this grid to the stair */
			j = distance(track_less_y[b_stair], track_less_x[b_stair], y, x);
			/* Distance of me to the stairs */
			b_j =
				distance(c_y, c_x, track_less_y[b_stair],
						 track_less_x[b_stair]);

			/* skip far away grids while I am close to stair */
			if (b_j <= borg_skill[BI_CLEVEL] * 5 + 9 &&
				j >= borg_skill[BI_CLEVEL] * 5 + 9) continue;
		}

		/* Extract adjacent locations */
		for (i = 0; i < 8; i++)
		{
			/* Extract the location */
			int xx = x + ddx_ddd[i];
			int yy = y + ddy_ddd[i];

			/* Bounds checking */
			if (map_in_bounds(xx, yy))
			{
				/* Get the grid contents */
				mb_array[i] = map_loc(xx, yy);
			}
			else
			{
				mb_array[i] = NULL;
			}
		}


		/* Count possible door locations */
		for (i = 0; i < 4; i++)
		{
			mb_ptr = mb_array[i];
			if (mb_ptr && mb_ptr->feat >= FEAT_WALL_EXTRA) wall++;
		}

		/* No possible secret doors */
		if (wall < 1) continue;


		/* Count supporting evidence for secret doors */
		for (i = 0; i < 4; i++)
		{
			mb_ptr = mb_array[i];

			/* Rubble */
			if (!mb_ptr || mb_ptr->feat == FEAT_RUBBLE) continue;

			/* Walls, Doors */
			if (((mb_ptr->feat >= FEAT_SECRET) &&
				 (mb_ptr->feat <= FEAT_PERM_SOLID)) ||
				((mb_ptr->feat == FEAT_OPEN) ||
				 (mb_ptr->feat == FEAT_BROKEN)) ||
				(mb_ptr->feat == FEAT_CLOSED))
			{
				supp++;
			}
		}

		/* Count supporting evidence for secret doors */
		for (i = 4; i < 8; i++)
		{
			mb_ptr = mb_array[i];

			/* Rubble */
			if (!mb_ptr || mb_ptr->feat == FEAT_RUBBLE) continue;

			/* Walls */
			if (mb_ptr->feat >= FEAT_SECRET)
			{
				diag++;
			}
		}

		/* No possible secret doors */
		if (diag < 2) continue;

		/* Tweak -- Reward walls, punish visitation and distance */
		v = (supp * 500) + (diag * 100) - (xtra_val * 20) - (cost * 1);

		/* The grid is not searchable */
		if (v <= 0) continue;


		/* Tweak -- Minimal interest until bored */
		if (!bored && (v < 1500)) continue;


		/* Track "best" grid */
		if ((b_v >= 0) && (v < b_v)) continue;

		/* Save the data */
		b_v = v;
		b_x = x;
		b_y = y;

	}
	MAP_ITT_END;

	/* Clear the flow codes */
	borg_flow_clear();

	/* Hack -- Nothing found */
	if (b_v < 0) return (FALSE);

	/* Memorize */
	spastic_x = b_x;
	spastic_y = b_y;


	/* Enqueue the grid */
	borg_flow_enqueue_grid(b_y, b_x);

	/* Spread the flow */
	borg_flow_spread(250, TRUE, FALSE, FALSE);

	/* Attempt to Commit the flow */
	if (!borg_flow_commit("spastic", GOAL_XTRA)) return (FALSE);

	/* Take one step */
	if (!borg_flow_old(GOAL_XTRA)) return (FALSE);

	/* Success */
	return (TRUE);
}




/*
 * Initialize this file
 */
void borg_init_6(void)
{
	/* Nothing */
}



#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
