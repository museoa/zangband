/* File: run.c */

/* Purpose: running code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "script.h"

/*
 * Hack -- Check for a "known wall" (see below)
 */
static int see_wall(int dir, int x, int y)
{
	pcave_type *pc_ptr;

	feature_type *f_ptr;

	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are "walls" */
	if (!in_boundsp(x, y)) return (TRUE);

	pc_ptr = parea(x, y);

	f_ptr = &f_info[pc_ptr->feat];

	/* Return block-los status */
	return (f_ptr->flags & FF_BLOCK);
}


/*
 * Hack -- Check for an "unknown corner" (see below)
 */
static int see_nothing(int dir, int x, int y)
{
	cave_type *c_ptr;
	pcave_type *pc_ptr;

	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are unknown */
	if (!in_boundsp(x, y)) return (FALSE);

	c_ptr = area(x, y);
	pc_ptr = parea(x, y);

	/* Memorized grids are always known */
	if (pc_ptr->feat) return (FALSE);

	/* Non-floor grids are unknown */
	if (cave_wall_grid(c_ptr)) return (TRUE);

	/* Viewable door/wall grids are known */
	if (player_can_see_grid(pc_ptr)) return (FALSE);

	/* Default */
	return (TRUE);
}

/*
 * The running algorithm:                       -CJS-
 *
 * In the diagrams below, the player has just arrived in the
 * grid marked as '@', and he has just come from a grid marked
 * as 'o', and he is about to enter the grid marked as 'x'.
 *
 * Of course, if the "requested" move was impossible, then you
 * will of course be blocked, and will stop.
 *
 * Overview: You keep moving until something interesting happens.
 * If you are in an enclosed space, you follow corners. This is
 * the usual corridor scheme. If you are in an open space, you go
 * straight, but stop before entering enclosed space. This is
 * analogous to reaching doorways. If you have enclosed space on
 * one side only (that is, running along side a wall) stop if
 * your wall opens out, or your open space closes in. Either case
 * corresponds to a doorway.
 *
 * What happens depends on what you can really SEE. (i.e. if you
 * have no light, then running along a dark corridor is JUST like
 * running in a dark room.) The algorithm works equally well in
 * corridors, rooms, mine tailings, earthquake rubble, etc, etc.
 *
 * These conditions are kept in static memory:
 * find_openarea         You are in the open on at least one
 * side.
 * find_breakleft        You have a wall on the left, and will
 * stop if it opens
 * find_breakright       You have a wall on the right, and will
 * stop if it opens
 *
 * To initialize these conditions, we examine the grids adjacent
 * to the grid marked 'x', two on each side (marked 'L' and 'R').
 * If either one of the two grids on a given side is seen to be
 * closed, then that side is considered to be closed. If both
 * sides are closed, then it is an enclosed (corridor) run.
 *
 * LL           L
 * @x          LxR
 * RR          @R
 *
 * Looking at more than just the immediate squares is
 * significant. Consider the following case. A run along the
 * corridor will stop just before entering the center point,
 * because a choice is clearly established. Running in any of
 * three available directions will be defined as a corridor run.
 * Note that a minor hack is inserted to make the angled corridor
 * entry (with one side blocked near and the other side blocked
 * further away from the runner) work correctly. The runner moves
 * diagonally, but then saves the previous direction as being
 * straight into the gap. Otherwise, the tail end of the other
 * entry would be perceived as an alternative on the next move.
 *
 * #.#
 * ##.##
 * .@x..
 * ##.##
 * #.#
 *
 * Likewise, a run along a wall, and then into a doorway (two
 * runs) will work correctly. A single run rightwards from @ will
 * stop at 1. Another run right and down will enter the corridor
 * and make the corner, stopping at the 2.
 *
 * #@x       1
 * ########### ######
 * 2           #
 * #############
 * #
 *
 * After any move, the function area_affect is called to
 * determine the new surroundings, and the direction of
 * subsequent moves. It examines the current player location
 * (at which the runner has just arrived) and the previous
 * direction (from which the runner is considered to have come).
 *
 * Moving one square in some direction places you adjacent to
 * three or five new squares (for straight and diagonal moves
 * respectively) to which you were not previously adjacent,
 * marked as '!' in the diagrams below.
 *
 * ...!   ...
 * .o@!   .o.!
 * ...!   ..@!
 * !!!
 *
 * You STOP if any of the new squares are interesting in any way:
 * for example, if they contain visible monsters or treasure.
 *
 * You STOP if any of the newly adjacent squares seem to be open,
 * and you are also looking for a break on that side. (that is,
 * find_openarea AND find_break).
 *
 * You STOP if any of the newly adjacent squares do NOT seem to be
 * open and you are in an open area, and that side was previously
 * entirely open.
 *
 * Corners: If you are not in the open (i.e. you are in a corridor)
 * and there is only one way to go in the new squares, then turn in
 * that direction. If there are more than two new ways to go, STOP.
 * If there are two ways to go, and those ways are separated by a
 * square which does not seem to be open, then STOP.
 *
 * Otherwise, we have a potential corner. There are two new open
 * squares, which are also adjacent. One of the new squares is
 * diagonally located, the other is straight on (as in the diagram).
 * We consider two more squares further out (marked below as ?).
 *
 * We assign "option" to the straight-on grid, and "option2" to the
 * diagonal grid, and "check_dir" to the grid marked 's'.
 *
 * .s
 * @x?
 * #?
 *
 * If they are both seen to be closed, then it is seen that no
 * benefit is gained from moving straight. It is a known corner.
 * To cut the corner, go diagonally, otherwise go straight, but
 * pretend you stepped diagonally into that next location for a
 * full view next time. Conversely, if one of the ? squares is
 * not seen to be closed, then there is a potential choice. We check
 * to see whether it is a potential corner or an intersection/room entrance.
 * If the square two spaces straight ahead, and the space marked with 's'
 * are both blank, then it is a potential corner and enter if find_examine
 * is set, otherwise must stop because it is not a corner.
 */




/*
 * Hack -- allow quick "cycling" through the legal directions
 */
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

/*
 * Hack -- map each direction into the "middle" of the "cycle[]" array
 */
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };


/*
 * Initialize the running algorithm for a new direction.
 *
 * Diagonal Corridor -- allow diaginal entry into corridors.
 *
 * Blunt Corridor -- If there is a wall two spaces ahead and
 * we seem to be in a corridor, then force a turn into the side
 * corridor, must be moving straight into a corridor here. ???
 *
 * Diagonal Corridor    Blunt Corridor (?)
 *       # #                  #
 *       #x#                 @x#
 *       @p.                  p
 */
static void run_init(int dir)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int row, col, deepleft, deepright;
	int i, shortleft, shortright;


	if (!p_ptr->depth)
	{
		/* If in the wilderness - run max 32 squares at a time */
		p_ptr->state.running = 32;
	}
	else
	{
		/* Large number to prevent infinite loops */
		p_ptr->state.running = 1000;
	}

	/* Save the direction */
	p_ptr->run.cur_dir = dir;

	/* Assume running straight */
	p_ptr->run.old_dir = dir;

	/* Assume looking for open area */
	p_ptr->run.open_area = TRUE;

	/* Assume not looking for breaks */
	p_ptr->run.break_right = FALSE;
	p_ptr->run.break_left = FALSE;

	/* Assume no nearby walls */
	deepleft = deepright = FALSE;
	shortright = shortleft = FALSE;

	/* Find the destination grid */
	row = py + ddy[dir];
	col = px + ddx[dir];

	/* Extract cycle index */
	i = chome[dir];

	/* Check for nearby wall */
	if (see_wall(cycle[i + 1], px, py))
	{
		p_ptr->run.break_left = TRUE;
		shortleft = TRUE;
	}
	else if (see_wall(cycle[i + 1], col, row))
	{
		p_ptr->run.break_left = TRUE;
		deepleft = TRUE;
	}

	/* Check for nearby wall */
	if (see_wall(cycle[i - 1], px, py))
	{
		p_ptr->run.break_right = TRUE;
		shortright = TRUE;
	}
	else if (see_wall(cycle[i - 1], col, row))
	{
		p_ptr->run.break_right = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if (p_ptr->run.break_left && p_ptr->run.break_right)
	{
		/* Not looking for open area */
		p_ptr->run.open_area = FALSE;

		/* Hack -- allow angled corridor entry */
		if (dir & 0x01)
		{
			if (deepleft && !deepright)
			{
				p_ptr->run.old_dir = cycle[i - 1];
			}
			else if (deepright && !deepleft)
			{
				p_ptr->run.old_dir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if (see_wall(cycle[i], col, row))
		{
			if (shortleft && !shortright)
			{
				p_ptr->run.old_dir = cycle[i - 2];
			}
			else if (shortright && !shortleft)
			{
				p_ptr->run.old_dir = cycle[i + 2];
			}
		}
	}
}



/*
 * Update the current "run" path
 *
 * Return TRUE if the running should be stopped
 */
static bool run_test(void)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	int prev_dir, new_dir, check_dir = 0;
	int row, col;
	int i, max, inv;
	int option = 0, option2 = 0;

	object_type *o_ptr;

	cave_type *c_ptr;
	pcave_type *pc_ptr;

	/* Where we came from */
	prev_dir = p_ptr->run.old_dir;

	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;


	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = py + ddy[new_dir];
		col = px + ddx[new_dir];

		if (!in_boundsp(col, row)) continue;

		/* Access grid */
		c_ptr = area(col, row);
		pc_ptr = parea(col, row);


		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}

		/* Visible objects abort running */
		OBJ_ITT_START (c_ptr->o_idx, o_ptr)
		{
			/* Visible object */
			if (o_ptr->info & (OB_SEEN)) return (TRUE);
		}
		OBJ_ITT_END;

		/* Visible traps abort running */
		if (is_visible_trap(c_ptr)) return TRUE;

		/* Assume unknown */
		inv = TRUE;

		/* Check memorized grids */
		if (pc_ptr->feat)
		{
			bool notice = TRUE;

			/* Examine the terrain */
			switch (c_ptr->feat)
			{
				case FEAT_FLOOR:
				case FEAT_SECRET:
				case FEAT_MAGMA:
				case FEAT_QUARTZ:

				case FEAT_WALL_EXTRA:
				case FEAT_WALL_INNER:
				case FEAT_WALL_OUTER:
				case FEAT_WALL_SOLID:
				case FEAT_PERM_EXTRA:
				case FEAT_PERM_INNER:
				case FEAT_PERM_OUTER:
				case FEAT_PERM_SOLID:

				case FEAT_SAND:
				case FEAT_SALT:
				case FEAT_WET_MUD:
				case FEAT_DRY_MUD:
				case FEAT_FLOOR_TILE:
				case FEAT_FLOOR_WOOD:
				case FEAT_PEBBLES:
				case FEAT_SOLID_LAVA:
				case FEAT_SHAL_WATER:
				case FEAT_DIRT:
				case FEAT_GRASS:
				case FEAT_TREE_WATER:

				case FEAT_TREES:
				case FEAT_MOUNTAIN:
				case FEAT_SNOW_MOUNTAIN:
				case FEAT_PINE_TREE:
				case FEAT_SNOW_TREE:
				case FEAT_OBELISK:
				case FEAT_PILLAR:

				case FEAT_BUSH:
				case FEAT_DEAD_BUSH:
				case FEAT_GRASS_LONG:
				case FEAT_ROCK_GEN:
				case FEAT_ROCK_SNOW:
				case FEAT_TREE_GEN:
				case FEAT_TREE_SNOW:
				case FEAT_SNOW:
				case FEAT_JUNGLE:
				{
					/* Ignore */
					notice = FALSE;

					/* Done */
					break;
				}

				case FEAT_DEEP_LAVA:
				case FEAT_SHAL_LAVA:
				{
					/* Ignore */
					if (p_ptr->tim.invuln || (FLAG(p_ptr, TR_IM_FIRE)))
						 notice = FALSE;

					/* Done */
					break;
				}

				case FEAT_DEEP_ACID:
				case FEAT_SHAL_ACID:
				{
					/* Ignore */
					if (p_ptr->tim.invuln || (FLAG(p_ptr, TR_IM_ACID)))
						 notice = FALSE;

					/* Done */
					break;
				}

				case FEAT_DEEP_SWAMP:
				case FEAT_SHAL_SWAMP:
				{
					/* Ignore */
					if (p_ptr->tim.invuln || (FLAG(p_ptr, TR_IM_POIS)))
						notice = FALSE;

					/* Done */
					break;
				}


				case FEAT_DEEP_WATER:
				case FEAT_OCEAN_WATER:
				{
					/* Ignore */
					if (FLAG(p_ptr, TR_FEATHER)) notice = FALSE;

					/* Done */
					break;
				}

					/* Open doors */
				case FEAT_OPEN:
				case FEAT_BROKEN:
				{
					/* Option -- ignore */
					if (find_ignore_doors) notice = FALSE;

					/* Done */
					break;
				}

					/* Stairs */
				case FEAT_LESS:
				case FEAT_MORE:
				{
					/* Option -- ignore */
					if (find_ignore_stairs) notice = FALSE;

					/* Done */
					break;
				}
			}

			/* Interesting feature */
			if (notice) return (TRUE);

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors */
		if (inv || cave_floor_grid(c_ptr))
		{
			/* Looking for open area */
			if (p_ptr->run.open_area)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if (!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if (option2)
			{
				return (TRUE);
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if (option != cycle[chome[prev_dir] + i - 1])
			{
				return (TRUE);
			}

			/* Two new (adjacent) directions (case 1) */
			else if (new_dir & 0x01)
			{
				check_dir = cycle[chome[prev_dir] + i - 2];
				option2 = new_dir;
			}

			/* Two new (adjacent) directions (case 2) */
			else
			{
				check_dir = cycle[chome[prev_dir] + i + 1];
				option2 = option;
				option = new_dir;
			}
		}

		/* Obstacle, while looking for open area */
		else
		{
			if (p_ptr->run.open_area)
			{
				if (i < 0)
				{
					/* Break to the right */
					p_ptr->run.break_right = TRUE;
				}

				else if (i > 0)
				{
					/* Break to the left */
					p_ptr->run.break_left = TRUE;
				}
			}
		}
	}


	/* Looking for open area */
	if (p_ptr->run.open_area)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			new_dir = cycle[chome[prev_dir] + i];

			row = py + ddy[new_dir];
			col = px + ddx[new_dir];

			if (!in_bounds2(col, row)) continue;

			/* Unknown grid or non-wall XXX XXX XXX cave_floor_grid(c_ptr)) */
			if (!see_wall(new_dir, px, py))

			{
				/* Looking to break right */
				if (p_ptr->run.break_right)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if (p_ptr->run.break_left)
				{
					return (TRUE);
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			new_dir = cycle[chome[prev_dir] + i];

			row = py + ddy[new_dir];
			col = px + ddx[new_dir];

			if (!in_bounds2(col, row)) continue;

			/* Access grid */
			c_ptr = area(col, row);

			/* Unknown grid or non-wall XXX XXX XXX cave_floor_grid(c_ptr)) */
			if (!see_wall(new_dir, px, py))

			{
				/* Looking to break left */
				if (p_ptr->run.break_left)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if (p_ptr->run.break_right)
				{
					return (TRUE);
				}
			}
		}
	}


	/* Not looking for open area */
	else
	{
		/* No options */
		if (!option)
		{
			return (TRUE);
		}

		/* One option */
		else if (!option2)
		{
			/* Primary option */
			p_ptr->run.cur_dir = option;

			/* No other options */
			p_ptr->run.old_dir = option;
		}

		/* Two options, examining corners */
		else if (find_examine && !find_cut)
		{
			/* Primary option */
			p_ptr->run.cur_dir = option;

			/* Hack -- allow curving */
			p_ptr->run.old_dir = option2;
		}

		/* Two options, pick one */
		else
		{
			/* Get next location */
			row = py + ddy[option];
			col = px + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if (!see_wall(option, col, row) || !see_wall(check_dir, col, row))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if (find_examine &&
					see_nothing(option, col, row) &&
					see_nothing(option2, col, row))
				{
					p_ptr->run.cur_dir = option;
					p_ptr->run.old_dir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return (TRUE);
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if (find_cut)
			{
				p_ptr->run.cur_dir = option2;
				p_ptr->run.old_dir = option2;
			}

			/* This corner is seen to be enclosed, and we */
			/* deliberately go the long way. */
			else
			{
				p_ptr->run.cur_dir = option;
				p_ptr->run.old_dir = option2;
			}
		}
	}


	/* About to hit a known wall, stop */
	if (see_wall(p_ptr->run.cur_dir, px, py))
	{
		return (TRUE);
	}


	/* Failure */
	return (FALSE);
}


/*
 * Take one step along the current "run" path
 */
void run_step(int dir)
{
	/* Start running */
	if (dir)
	{
		/* Hack -- do not start silly run */
		if (see_wall(dir, p_ptr->px, p_ptr->py))
		{
			/* Message */
			msgf("You cannot run in that direction.");

			/* Disturb */
			disturb(FALSE);

			/* Done */
			return;
		}

		/* Calculate torch radius */
		p_ptr->update |= (PU_TORCH);

		/* Initialize */
		run_init(dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if (run_test())
		{
			/* Disturb */
			disturb(FALSE);

			/* Done */
			return;
		}
	}

	/* Decrease the run counter */
	p_ptr->state.running--;

	/* Take time */
	p_ptr->state.energy_use = 100;

	/* Move the player, using the "pickup" flag */
	move_player(p_ptr->run.cur_dir, FALSE);
}
