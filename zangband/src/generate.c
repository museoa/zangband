/* CVS: Last edit by $Author$ on $Date$ */
/* File: generate.c */

/* Purpose: Dungeon generation */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/*
 * Note that Level generation is *not* an important bottleneck,
 * though it can be annoyingly slow on older machines...  Thus
 * we emphasize "simplicity" and "correctness" over "speed".
 *
 * This entire file is only needed for generating levels.
 * This may allow smart compilers to only load it when needed.
 *
 * Consider the "v_info.txt" file for vault generation.
 *
 * In this file, we use the "special" granite and perma-wall sub-types,
 * where "basic" is normal, "inner" is inside a room, "outer" is the
 * outer wall of a room, and "solid" is the outer wall of the dungeon
 * or any walls that may not be pierced by corridors.  Thus the only
 * wall type that may be pierced by a corridor is the "outer granite"
 * type.  The "basic granite" type yields the "actual" corridors.
 *
 * Note that we use the special "solid" granite wall type to prevent
 * multiple corridors from piercing a wall in two adjacent locations,
 * which would be messy, and we use the special "outer" granite wall
 * to indicate which walls "surround" rooms, and may thus be "pierced"
 * by corridors entering or leaving the room.
 *
 * Note that a tunnel which attempts to leave a room near the "edge"
 * of the dungeon in a direction toward that edge will cause "silly"
 * wall piercings, but will have no permanently incorrect effects,
 * as long as the tunnel can *eventually* exit from another side.
 * And note that the wall may not come back into the room by the
 * hole it left through, so it must bend to the left or right and
 * then optionally re-enter the room (at least 2 grids away).  This
 * is not a problem since every room that is large enough to block
 * the passage of tunnels is also large enough to allow the tunnel
 * to pierce the room itself several times.
 *
 * Note that no two corridors may enter a room through adjacent grids,
 * they must either share an entryway or else use entryways at least
 * two grids apart.  This prevents "large" (or "silly") doorways.
 *
 * To create rooms in the dungeon, we first divide the dungeon up
 * into "blocks" of 11x11 grids each, and require that all rooms
 * occupy a rectangular group of blocks.  As long as each room type
 * reserves a sufficient number of blocks, the room building routines
 * will not need to check bounds.  Note that most of the normal rooms
 * actually only use 23x11 grids, and so reserve 33x11 grids.
 *
 * Note that the use of 11x11 blocks (instead of the old 33x11 blocks)
 * allows more variability in the horizontal placement of rooms, and
 * at the same time has the disadvantage that some rooms (two thirds
 * of the normal rooms) may be "split" by panel boundaries.  This can
 * induce a situation where a player is in a room and part of the room
 * is off the screen.  It may be annoying enough to go back to 33x11
 * blocks to prevent this visual situation.
 *
 * Note that the dungeon generation routines are much different (2.7.5)
 * and perhaps "DUN_ROOMS" should be less than 50.
 *
 * XXX XXX XXX Note that it is possible to create a room which is only
 * connected to itself, because the "tunnel generation" code allows a
 * tunnel to leave a room, wander around, and then re-enter the room.
 *
 * XXX XXX XXX Note that it is possible to create a set of rooms which
 * are only connected to other rooms in that set, since there is nothing
 * explicit in the code to prevent this from happening.  But this is less
 * likely than the "isolated room" problem, because each room attempts to
 * connect to another room, in a giant cycle, thus requiring at least two
 * bizarre occurances to create an isolated section of the dungeon.
 *
 * Note that (2.7.9) monster pits have been split into monster "nests"
 * and monster "pits".  The "nests" have a collection of monsters of a
 * given type strewn randomly around the room (jelly, animal, or undead),
 * while the "pits" have a collection of monsters of a given type placed
 * around the room in an organized manner (orc, troll, giant, dragon, or
 * demon).  Note that both "nests" and "pits" are now "level dependant",
 * and both make 16 "expensive" calls to the "get_mon_num()" function.
 *
 * Note that the cave grid flags changed in a rather drastic manner
 * for Angband 2.8.0 (and 2.7.9+), in particular, dungeon terrain
 * features, such as doors and stairs and traps and rubble and walls,
 * are all handled as a set of 64 possible "terrain features", and
 * not as "fake" objects (440-479) as in pre-2.8.0 versions.
 *
 * The 64 new "dungeon features" will also be used for "visual display"
 * but we must be careful not to allow, for example, the user to display
 * hidden traps in a different way from floors, or secret doors in a way
 * different from granite walls, or even permanent granite in a different
 * way from granite.  XXX XXX XXX
 */

#include "angband.h"
#include "generate.h"
#include "grid.h"
#include "rooms.h"
#include "streams.h"

int dun_rooms;

int dun_tun_rnd;
int dun_tun_chg;
int dun_tun_con;
int dun_tun_pen;
int dun_tun_jct;


/*
 * Dungeon generation data -- see "cave_gen()"
 */
dun_data *dun;

/*
 * Places some staircases near walls
 */
static bool alloc_stairs(int feat, int num, int walls)
{
	int         y, x, i, j, flag;
	cave_type   *c_ptr;

	if (feat == FEAT_LESS)
	{
		/* No up stairs in town or in ironman mode */
		if (ironman_downward || !dun_level) return TRUE;
	}
	else if (feat == FEAT_MORE)
	{
		/* No downstairs on quest levels */
		if ((dun_level > 1) && quest_number(dun_level)) return TRUE;

		/* No downstairs at the bottom */
		if (dun_level >= MAX_DEPTH - 1) return TRUE;
	}

	/* Place "num" stairs */
	for (i = 0; i < num; i++)
	{
		/* Place some stairs */
		for (flag = FALSE; !flag; )
		{
			/* Try several times, then decrease "walls" */
			for (j = 0; !flag && j <= 10000; j++)
			{
				/* Pick a random grid */
				y = rand_int(cur_hgt);
				x = rand_int(cur_wid);

				/* Access the grid */
				c_ptr = &cave[y][x];

				/* Require "naked" floor grid */
				if (!cave_naked_grid(c_ptr)) continue;

				/* Require floor */
				if (c_ptr->feat != FEAT_FLOOR) continue;

				/* Require a certain number of adjacent walls */
				if (next_to_walls(y, x) < walls) continue;

				/* Clear previous contents, add stairs */
				c_ptr->feat = feat;

				/* All done */
				flag = TRUE;
			}

			/* If cannot find a blank spot - exit */
			if (!walls)
			{
				/* Placed at least one. */
				if(i > 0) return TRUE;

				/* Couldn't place any stairs */
				return FALSE;
			}

			/* Require fewer walls */
			walls--;
		}
	}

	/* Done */
	return TRUE;
}


/*
 * Allocates some objects (using "place" and "type")
 */
static void alloc_object(int set, int typ, int num)
{
	int y, x, k;
	int dummy = 0;
	cave_type *c_ptr;

	/* Place some objects */
	for (k = 0; k < num; k++)
	{
		/* Pick a "legal" spot */
		while (dummy < SAFE_MAX_ATTEMPTS)
		{
			bool room;

			dummy++;

			/* Location */
			y = rand_int(cur_hgt);
			x = rand_int(cur_wid);

			c_ptr = &cave[y][x];

			/* Require "naked" floor grid */
			if (!cave_naked_grid(c_ptr)) continue;

			/* Check for "room" */
			room = (cave[y][x].info & CAVE_ROOM) ? TRUE : FALSE;

			/* Require corridor? */
			if ((set == ALLOC_SET_CORR) && room) continue;

			/* Require room? */
			if ((set == ALLOC_SET_ROOM) && !room) continue;

			/* Accept it */
			break;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
				msg_print("Warning! Could not place object!");
			}
			return;
		}


		/* Place something */
		switch (typ)
		{
			case ALLOC_TYP_RUBBLE:
			{
				place_rubble(y, x);
				break;
			}

			case ALLOC_TYP_TRAP:
			{
				place_trap(y, x);
				break;
			}

			case ALLOC_TYP_GOLD:
			{
				place_gold(y, x);
				break;
			}

			case ALLOC_TYP_OBJECT:
			{
				place_object(y, x, FALSE, FALSE);
				break;
			}

			case ALLOC_TYP_INVIS:
			{
				place_invis_wall(y, x);
				break;
			}
		}
	}
}


/*
 * Count the number of "corridor" grids adjacent to the given grid.
 *
 * Note -- Assumes "in_bounds(y1, x1)"
 *
 * XXX XXX This routine currently only counts actual "empty floor"
 * grids which are not in rooms.  We might want to also count stairs,
 * open doors, closed doors, etc.
 */
static int next_to_corr(int y1, int x1)
{
	int i, y, x, k = 0;

	cave_type *c_ptr;

	/* Scan adjacent grids */
	for (i = 0; i < 4; i++)
	{
		/* Extract the location */
		y = y1 + ddy_ddd[i];
		x = x1 + ddx_ddd[i];

		/* Access the grid */
		c_ptr = &cave[y][x];

		/* Skip non floors */
		if (!cave_floor_grid(c_ptr)) continue;

		/* Skip non "empty floor" grids */
		if (c_ptr->feat != FEAT_FLOOR) continue;

		/* Skip grids inside rooms */
		if (c_ptr->info & (CAVE_ROOM)) continue;

		/* Count these grids */
		k++;
	}

	/* Return the number of corridors */
	return (k);
}


/*
 * Determine if the given location is "between" two walls,
 * and "next to" two corridor spaces.  XXX XXX XXX
 *
 * Assumes "in_bounds(y, x)"
 */
static bool possible_doorway(int y, int x)
{
	/* Count the adjacent corridors */
	if (next_to_corr(y, x) >= 2)
	{
		/* Check Vertical */
		if ((cave[y-1][x].feat >= FEAT_MAGMA) &&
		    (cave[y+1][x].feat >= FEAT_MAGMA))
		{
			return (TRUE);
		}

		/* Check Horizontal */
		if ((cave[y][x-1].feat >= FEAT_MAGMA) &&
		    (cave[y][x+1].feat >= FEAT_MAGMA))
		{
			return (TRUE);
		}
	}

	/* No doorway */
	return (FALSE);
}


/*
 * Places door at y, x position if at least 2 walls found
 */
static void try_door(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Ignore walls */
	if (cave[y][x].feat >= FEAT_MAGMA) return;

	/* Ignore room grids */
	if (cave[y][x].info & (CAVE_ROOM)) return;

	/* Occasional door (if allowed) */
	if ((rand_int(100) < dun_tun_jct) && possible_doorway(y, x))
	{
		/* Place a door */
		place_random_door(y, x);
	}
}



/*
 * Generate a new dungeon level
 *
 * Note that "dun_body" adds about 4000 bytes of memory to the stack.
 */
static bool cave_gen(void)
{
	int i, j, k, y, x, y1, x1;

	int max_vault_ok = 2;

	int feat1, feat2;

	cave_type *c_ptr;

	bool destroyed = FALSE;
	bool empty_level = FALSE;
	bool cavern = FALSE;
	int laketype = 0;


	dun_data dun_body;

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), NULL);

	/* Global data */
	dun = &dun_body;

	if (max_panel_rows < 67) max_vault_ok--;
	if (max_panel_cols < 34) max_vault_ok--;

	/* Randomize the dungeon creation values */
	dun_rooms = rand_range(DUN_ROOMS_MIN, DUN_ROOMS_MAX);
	dun_tun_rnd = rand_range(DUN_TUN_RND_MIN, DUN_TUN_RND_MAX);
	dun_tun_chg = rand_range(DUN_TUN_CHG_MIN, DUN_TUN_CHG_MAX);
	dun_tun_con = rand_range(DUN_TUN_CON_MIN, DUN_TUN_CON_MAX);
	dun_tun_pen = rand_range(DUN_TUN_PEN_MIN, DUN_TUN_PEN_MAX);
	dun_tun_jct = rand_range(DUN_TUN_JCT_MIN, DUN_TUN_JCT_MAX);

	/* Empty arena levels */
	if (ironman_empty_levels || (empty_levels && (randint(EMPTY_LEVEL) == 1)))
	{
		empty_level = TRUE;

		if (cheat_room)
			msg_print("Arena level.");
	}


	/* Hack -- Start with basic granite */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave_type *c_ptr = &cave[y][x];

			if (empty_level)
				c_ptr->feat = FEAT_FLOOR;
			else
			  /* Create granite wall */
				c_ptr->feat = FEAT_WALL_EXTRA;
		}
	}

#ifdef ALLOW_CAVERNS_AND_LAKES
	/* Possible "destroyed" level */
	if ((dun_level > 15) && (rand_int(DUN_DEST) == 0) && (small_levels))
	{
		destroyed = TRUE;

		/* extra rubble around the place looks cool */
		build_lake(3);
	}

	/* Make a lake some of the time */
	if ((rand_int(LAKE_LEVEL) == 0) && !empty_level && !destroyed && terrain_streams)
	{
		/* Lake of Water */
		if (dun_level > 52) laketype = 2;

		/* Lake of Lava */
		if (dun_level > 90) laketype = 1;

		if (laketype != 0)
		{
			if (cheat_room)
				msg_print("Lake on the level.");
			build_lake(laketype);
		}
	}

	if ((rand_int(DUN_CAV1/(dun_level + DUN_CAV2)) == 0) && !empty_level &&
	    (laketype == 0) && !destroyed && (dun_level >= MIN_CAVERN))
	{
		cavern = TRUE;

		/* make a large fractal cave in the middle of the dungeon */

		if (cheat_room)
			msg_print("Cavern on level.");

		build_cavern();
	}
#endif /* ALLOW_CAVERNS_AND_LAKES */

	/* Hack -- No destroyed "quest" levels */
	if (quest_number(dun_level)) destroyed = FALSE;

	/* Actual maximum number of rooms on this level */
	dun->row_rooms = cur_hgt / BLOCK_HGT;
	dun->col_rooms = cur_wid / BLOCK_WID;

	/* Initialize the room table */
	for (y = 0; y < dun->row_rooms; y++)
	{
		for (x = 0; x < dun->col_rooms; x++)
		{
			dun->room_map[y][x] = FALSE;
		}
	}


	/* No "crowded" rooms yet */
	dun->crowded = 0;


	/* No rooms yet */
	dun->cent_n = 0;

	/* Build some rooms */
	for (i = 0; i < dun_rooms; i++)
	{
		/* Pick a block for the room */
		y = rand_int(dun->row_rooms);
		x = rand_int(dun->col_rooms);

		/* Align dungeon rooms */
		if (dungeon_align)
		{
			/* Slide some rooms right */
			if ((x % 3) == 0) x++;

			/* Slide some rooms left */
			if ((x % 3) == 2) x--;
		}

		/* Attempt an "unusual" room */
		if (ironman_rooms || (rand_int(DUN_UNUSUAL) < dun_level))
		{
			/* Roll for room type */
			k = rand_int(100);

			/* Attempt a very unusual room */
			if ((ironman_rooms && (rand_int(DUN_UNUSUAL) < dun_level * 2)) ||
				 (rand_int(DUN_UNUSUAL) < dun_level))
			{
#ifdef FORCE_V_IDX
				if (room_build(y, x, 8)) continue;
#else
				/* Type 8 -- Greater vault (4%) */
				if (k < 4)
				{
					if (max_vault_ok > 1)
					{
						if (room_build(y, x, 8)) continue;
					}
					else
					{
						if (cheat_room) msg_print("Refusing a greater vault.");
					}
				}

				/* Type 7 -- Lesser vault (6%) */
				if (k < 10)
				{
					if (max_vault_ok > 0)
					{
						if (room_build(y, x, 7)) continue;
					}
					else
					{
						if (cheat_room) msg_print("Refusing a lesser vault.");
					}
				}


				/* Type 5 -- Monster nest (8%) */
				if ((k < 18) && room_build(y, x, 5)) continue;

				/* Type 6 -- Monster pit (10%) */
				if ((k < 28) && room_build(y, x, 6)) continue;

				/* Type 10 -- Random vault (4%) */
				if ((k < 32) && room_build(y, x, 10)) continue;
#endif

			}

			/* Type 4 -- Large room (25%) */
			if ((k < 25) && room_build(y, x, 4)) continue;

			/* Type 3 -- Cross room (25%) */
			if ((k < 50) && room_build(y, x, 3)) continue;

			/* Type 2 -- Overlapping (25%) */
			if ((k < 75) && room_build(y, x, 2)) continue;

			/* Type 11 -- Circular (10%) */
			if ((k < 85) && room_build(y, x, 11)) continue;

			/* Type 12 -- Crypt (15%) */
			if ((k < 100) && room_build(y, x, 12)) continue;
		}

		/* The deeper you are, the more cavelike the rooms are */
		k = randint(100);

		/* No caves when a cavern exists: they look bad */
		if ((k < dun_level) && (!cavern) && (!empty_level) && (laketype == 0))
		{
			/* Type 9 -- Fractal cave */
			if (room_build(y, x, 9)) continue;
		}
		else
			/* Attempt a "trivial" room */
		if (room_build(y, x, 1)) continue;
		continue;
	}

	/* Make a hole in the dungeon roof sometimes at level 1 */
	if ((dun_level == 1) && terrain_streams)
	{
		while (randint(DUN_MOS_DEN) == 1)
		{
			place_trees(randint(cur_wid - 2), randint(cur_hgt - 2));
		}
	}

	/* Destroy the level if necessary */
	if (destroyed) destroy_level();

	/* Hack -- Add some rivers */
	if ((randint(3) == 1) && (randint(dun_level) > 5) && terrain_streams)
	{
	 	/* Choose water or lava */
		if (randint(MAX_DEPTH * 2) - 1 > dun_level)
		{
			feat1 = FEAT_DEEP_WATER;
			feat2 = FEAT_SHAL_WATER;
		}
		else
		{
			feat1 = FEAT_DEEP_LAVA;
			feat2 = FEAT_SHAL_LAVA;
		}


	 	/* Only add river if matches lake type or if have no lake at all */
	 	if (((laketype == 1) && (feat1 == FEAT_DEEP_LAVA)) ||
	 	    ((laketype == 2) && (feat1 == FEAT_DEEP_WATER)) ||
		     (laketype == 0))
	 	{
			add_river(feat1, feat2);
		}
	}

	/* Special boundary walls -- Top */
	for (x = 0; x < cur_wid; x++)
	{
		cave_type *c_ptr = &cave[0][x];

		/* Clear previous contents, add "solid" perma-wall */
		c_ptr->feat = FEAT_PERM_SOLID;
	}

	/* Special boundary walls -- Bottom */
	for (x = 0; x < cur_wid; x++)
	{
		cave_type *c_ptr = &cave[cur_hgt-1][x];

		/* Clear previous contents, add "solid" perma-wall */
		c_ptr->feat = FEAT_PERM_SOLID;
	}

	/* Special boundary walls -- Left */
	for (y = 0; y < cur_hgt; y++)
	{
		cave_type *c_ptr = &cave[y][0];

		/* Clear previous contents, add "solid" perma-wall */
		c_ptr->feat = FEAT_PERM_SOLID;
	}

	/* Special boundary walls -- Right */
	for (y = 0; y < cur_hgt; y++)
	{
		cave_type *c_ptr = &cave[y][cur_wid-1];

		/* Clear previous contents, add "solid" perma-wall */
		c_ptr->feat = FEAT_PERM_SOLID;
	}


	/* Hack -- Scramble the room order */
	for (i = 0; i < dun->cent_n; i++)
	{
		int pick1 = rand_int(dun->cent_n);
		int pick2 = rand_int(dun->cent_n);
		y1 = dun->cent[pick1].y;
		x1 = dun->cent[pick1].x;
		dun->cent[pick1].y = dun->cent[pick2].y;
		dun->cent[pick1].x = dun->cent[pick2].x;
		dun->cent[pick2].y = y1;
		dun->cent[pick2].x = x1;
	}

	/* Start with no tunnel doors */
	dun->door_n = 0;

	/* Hack -- connect the first room to the last room */
	y = dun->cent[dun->cent_n-1].y;
	x = dun->cent[dun->cent_n-1].x;

	/* Connect all the rooms together */
	for (i = 0; i < dun->cent_n; i++)
	{

		/* Reset the arrays */
		dun->tunn_n = 0;
		dun->wall_n = 0;

		/* Connect the room to the previous room */
#if PILLAR_TUNNELS

		if ((randint(20) > dun_level) && (randint(100) < 25))
		{
			/* make catacomb-like tunnel */
			build_tunnel2(dun->cent[i].x, dun->cent[i].y, x, y, 3, 30);
		}
		else if (randint(dun_level) > 50)
#else
		if (randint(dun_level) > 50)
#endif /* PILLAR_TUNNELS */
		{
			/* make cave-like tunnel */
			build_tunnel2(dun->cent[i].x, dun->cent[i].y, x, y, 2, 2);
		}
		else
		{
			/* make normal tunnel */
			build_tunnel(dun->cent[i].y, dun->cent[i].x, y, x);
		}

		/* Turn the tunnel into corridor */
		for (j = 0; j < dun->tunn_n; j++)
		{
			/* Access the grid */
			y = dun->tunn[j].y;
			x = dun->tunn[j].x;

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Clear previous contents (if not a lake), add a floor */
			if ((c_ptr->feat < FEAT_DEEP_WATER) ||
			    (c_ptr->feat > FEAT_SHAL_LAVA))
			{
				c_ptr->feat = FEAT_FLOOR;
			}
		}

		/* Apply the piercings that we found */
		for (j = 0; j < dun->wall_n; j++)
		{
			/* Access the grid */
			y = dun->wall[j].y;
			x = dun->wall[j].x;

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Clear previous contents, add up floor */
			c_ptr->feat = FEAT_FLOOR;

			/* Occasional doorway */
			if (rand_int(100) < dun_tun_pen)
			{
				/* Place a random door */
				place_random_door(y, x);
			}
		}

		/* Remember the "previous" room */
		y = dun->cent[i].y;
		x = dun->cent[i].x;
	}

	/* Place intersection doors	 */
	for (i = 0; i < dun->door_n; i++)
	{
		/* Extract junction location */
		y = dun->door[i].y;
		x = dun->door[i].x;

		/* Try placing doors */
		try_door(y, x - 1);
		try_door(y, x + 1);
		try_door(y - 1, x);
		try_door(y + 1, x);
	}


	/* Hack -- Add some magma streamers */
	for (i = 0; i < DUN_STR_MAG; i++)
	{
		build_streamer(FEAT_MAGMA, DUN_STR_MC);
	}

	/* Hack -- Add some quartz streamers */
	for (i = 0; i < DUN_STR_QUA; i++)
	{
		build_streamer(FEAT_QUARTZ, DUN_STR_QC);
	}

	/* Place 3 or 4 down stairs near some walls */
	if (!alloc_stairs(FEAT_MORE, rand_range(3, 4), 3)) return FALSE;

	/* Place 1 or 2 up stairs near some walls */
	if (!alloc_stairs(FEAT_LESS, rand_range(1, 2), 3)) return FALSE;

	/* Handle the quest monster placements */
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].status == QUEST_STATUS_TAKEN) &&
		    ((quest[i].type == QUEST_TYPE_KILL_LEVEL) ||
		    (quest[i].type == QUEST_TYPE_RANDOM)) &&
		    (quest[i].level == dun_level) &&
			!(quest[i].flags & QUEST_FLAG_PRESET))
		{
			monster_race *r_ptr = &r_info[quest[i].r_idx];

			/* Hack -- "unique" monsters must be "unique" */
			if ((r_ptr->flags1 & RF1_UNIQUE) &&
			    (r_ptr->cur_num >= r_ptr->max_num))
			{
				/* The unique is already dead */
				quest[i].status = QUEST_STATUS_FINISHED;
			}
			else
			{
				bool group;

				/* Hard quests -> revive all monsters */
				if (ironman_hard_quests)
				{
					quest[i].cur_num = 0;
				}

				for (j = 0; j < (quest[i].max_num - quest[i].cur_num); j++)
				{
					for (k = 0; k < SAFE_MAX_ATTEMPTS; k++)
					{
						/* Find an empty grid */
						while (TRUE)
						{
							y = rand_int(cur_hgt);
							x = rand_int(cur_wid);

							/* Access the grid */
							c_ptr = &cave[y][x];

							if (!cave_naked_grid(c_ptr)) continue;
							if (distance(y, x, py, px) < 10) continue;
							else break;
						}

						if (r_ptr->flags1 & RF1_FRIENDS)
							group = FALSE;
						else
							group = TRUE;

						/* Try to place the monster */
						if (place_monster_aux(y, x, quest[i].r_idx, FALSE, group, FALSE, FALSE))
						{
							/* Success */
							break;
						}
						else
						{
							/* Failure - Try again */
							continue;
						}
					}
				}
			}
		}
	}


	/* Basic "amount" */
	k = (dun_level / 3);
	if (k > 10) k = 10;
	if (k < 2) k = 2;

	/* Pick a base number of monsters */
	i = MIN_M_ALLOC_LEVEL;

	/* To make small levels a bit more playable */
	if (cur_hgt < MAX_HGT || cur_wid < MAX_WID)
	{
		int small_tester = i;

		i = (i * cur_hgt) / MAX_HGT;
		i = (i * cur_wid) / MAX_WID;
		i += 1;

		if (i > small_tester) i = small_tester;
		else if (cheat_hear)
		{
			msg_format("Reduced monsters base from %d to %d", small_tester, i);
		}
	}

	i += randint(8);

	/* Put some monsters in the dungeon */
	for (i = i + k; i > 0; i--)
	{
		(void)alloc_monster(0, TRUE);
	}

	/* Place some traps in the dungeon */
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_TRAP, randint(k));

	/* Put some rubble in corridors */
	alloc_object(ALLOC_SET_CORR, ALLOC_TYP_RUBBLE, randint(k));

	/* Put some objects in rooms */
	alloc_object(ALLOC_SET_ROOM, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ROOM, 3));

	/* Put some objects/gold in the dungeon */
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ITEM, 3));
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_GOLD, randnor(DUN_AMT_GOLD, 3));

	/* Put some invisible walls in the dungeon for nightmare mode */
	if (ironman_nightmare)
	{
		alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_INVIS, randnor(DUN_AMT_INVIS, 3));
	}

	if (empty_level && ((randint(DARK_EMPTY) != 1) || (randint(100) > dun_level)))
	{
		/* Lite the cave */
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				cave[y][x].info |= (CAVE_GLOW);
			}
		}
	}

	/* Determine the character location */
	if (!new_player_spot())
		return FALSE;

	return TRUE;
}


#if 0

/*
 * Generate a quest level
 */
static void quest_gen(void)
{
	int x, y;


	/* Start with perm walls */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave[y][x].feat = FEAT_PERM_SOLID;
		}
	}

	/* Set the quest level */
	base_level = quest[p_ptr->inside_quest].level;
	dun_level = base_level;
	object_level = base_level;
	monster_level = base_level;

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), NULL);

	init_flags = INIT_CREATE_DUNGEON | INIT_ASSIGN;
	process_dungeon_file("q_info.txt", 0, 0, MAX_HGT, MAX_WID);
}

#endif

static int map_wid_old = 66;


void map_panel_size(void)
{
	int wid, hgt;


	/* Only if the map exists */
	if (!character_dungeon) return;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Offset */
	wid -= COL_MAP + 1;

	/* reset panels */
	if (dun_level)
	{
		/* Determine number of panels (dungeon) */
		max_panel_rows = cur_hgt;
		max_panel_cols = cur_wid;
	}
	else
	{
		/* Determine number of panels (wilderness) */
		max_panel_rows = max_wild * 16;
		max_panel_cols = max_wild * 16;
	}

	/* Assume illegal panel */
	panel_row_min = max_panel_rows;
	panel_col_min = max_panel_cols;
	
	/* Kill previous size of line */
	
#ifdef USE_TRANSPARENCY
	/* String of terrain characters along one row of the map */
	if (mp_ta) C_KILL(mp_ta, map_wid_old, byte);
	if (mp_tc) C_KILL(mp_tc, map_wid_old, char);
	
#endif /* USE_TRANSPARENCY */	
	
	/* String of characters along one row of the map */
	if (mp_a) C_KILL(mp_a, map_wid_old, byte);
	if (mp_c) C_KILL(mp_c, map_wid_old, char);
	
	/* Save size */
	map_wid_old = wid;
	
	/* Make the new lines */

#ifdef USE_TRANSPARENCY
	/* String of terrain characters along one row of the map */
	C_MAKE(mp_ta, wid, byte);
	C_MAKE(mp_tc, wid, char);
	
#endif /* USE_TRANSPARENCY */	
	
	/* String of characters along one row of the map */
	C_MAKE(mp_a, wid, byte);
	C_MAKE(mp_c, wid, char);
}

/* Make a real level */
static bool level_gen(cptr *why)
{
	int level_height, level_width;

	if (ironman_small_levels ||
		((randint(SMALL_LEVEL) == 1) && small_levels))
	{
		if (cheat_room)
		  msg_print("A 'small' dungeon level.");

		while (TRUE)
		{
			level_height = randint(MAX_HGT / BLOCK_HGT);
			level_width = randint(MAX_WID / BLOCK_WID);
			
			/* Exit if larger than one screen, but less than normal dungeon */
			if ((level_height < (MAX_HGT / BLOCK_HGT)) &&
				(level_height >= (22 / BLOCK_HGT)) &&
				(level_width < (MAX_WID / BLOCK_WID)) &&
				(level_width >= (66 / BLOCK_WID))) break;
		}

		cur_hgt = level_height * BLOCK_HGT;
		cur_wid = level_width * BLOCK_WID;

		if (cheat_room)
		  msg_format("X:%d, Y:%d.", max_panel_cols, max_panel_rows);
	}
	else
	{
		/* Big dungeon */
		cur_hgt = MAX_HGT;
		cur_wid = MAX_WID;
	}

	/* Make a dungeon */
	if (!cave_gen())
	{
		*why = "could not place player";
		return FALSE;
	}
	else return TRUE;
}

static byte extract_feeling(void)
{
	/* Hack -- no feeling in the town */
	if (!dun_level) return 0;

	/* Hack -- Have a special feeling sometimes */
	if (good_item_flag && !preserve_mode) return 1;

 	if (rating > 100) return 2;
	if (rating > 80) return 3;
	if (rating > 60) return 4;
	if (rating > 40) return 5;
	if (rating > 30) return 6;
	if (rating > 20) return 7;
	if (rating > 10) return 8;
	if (rating > 0) return 9;

	if ((turn - old_turn) > 50000L)
		chg_virtue(V_PATIENCE, 1);

	return 10;
}


/*
 * Generates a random dungeon level			-RAK-
 *
 * Hack -- regenerate any "overflow" levels
 *
 * Hack -- allow auto-scumming via a gameplay option.
 */
void generate_cave(void)
{
	int y, x, num;


	/* Build the wilderness */
	if (!dun_level)
	{
		/* Hack XXX XXX */
		/* Exit, information is already in other data type. */

		px = (s16b)p_ptr->wilderness_x;
		py = (s16b)p_ptr->wilderness_y;

		/* The "dungeon" is ready */
		character_dungeon = TRUE;
		
		/* Reset map panels */
		map_panel_size();

		/* Add monsters to the wilderness */
		repopulate_wilderness();

		return;
	}

	/* The dungeon is not ready */
	character_dungeon = FALSE;

	/* Generate */
	for (num = 0; TRUE; num++)
	{
		bool okay = TRUE;

		cptr why = NULL;


		/* XXX XXX XXX XXX */
		o_max = 1;
		m_max = 1;

		/* Start with a blank cave */
		for (y = 0; y < MAX_HGT; y++)
		{
			for (x = 0; x < MAX_WID; x++)
			{
				/* No flags */
				cave[y][x].info = 0;

				/* No features */
				cave[y][x].feat = 0;

				/* No objects */
				cave[y][x].o_idx = 0;

				/* No monsters */
				cave[y][x].m_idx = 0;

				/* No mimic */
				cave[y][x].mimic = 0;

#ifdef MONSTER_FLOW
				/* No flow */
				cave[y][x].cost = 0;
				cave[y][x].when = 0;
#endif /* MONSTER_FLOW */
			}
		}

		/* Set the base level */
		base_level = dun_level;

		/* Reset the monster generation level */
		monster_level = base_level;

		/* Reset the object generation level */
		object_level = base_level;

		/* Nothing special here yet */
		good_item_flag = FALSE;

		/* Nothing good here yet */
		rating = 0;

#ifdef USE_SCRIPT
		if (!generate_level_callback(dun_level))
#endif /* USE_SCRIPT */
		{

#if 0
			/* Build the arena -KMW- */
			if (p_ptr->inside_arena)
			{
				/* Small arena */
				arena_gen();
			}

			/* Quest levels -KMW- */
			/* Hack - disabled.
			else if (p_ptr->inside_quest)
			{
				quest_gen();
			}*/
#endif

			okay = level_gen(&why);

		}

		/* Extract the feeling */
		feeling = extract_feeling();

		/* Prevent object over-flow */
		if (o_max >= max_o_idx)
		{
			/* Message */
			why = "too many objects";

			/* Message */
			okay = FALSE;
		}
		/* Prevent monster over-flow */
		else if (m_max >= max_m_idx)
		{
			/* Message */
			why = "too many monsters";

			/* Message */
			okay = FALSE;
		}

		/* Mega-Hack -- "auto-scum" */
		else if ((auto_scum || ironman_autoscum) && (num < 100) &&
				 !p_ptr->inside_quest)
		{
			/* Require "goodness" */
			if ((feeling > 9) ||
			    ((dun_level >= 7) && (feeling > 8)) ||
			    ((dun_level >= 15) && (feeling > 7)) ||
			    ((dun_level >= 35) && (feeling > 6)) ||
			    ((dun_level >= 70) && (feeling > 5)))
			{
				/* Give message to cheaters */
				if (cheat_room || cheat_hear ||
				    cheat_peek || cheat_xtra)
				{
					/* Message */
					why = "boring level";
				}

				/* Try again */
				okay = FALSE;
			}
		}

		/* Accept */
		if (okay) break;

		/* Message */
		if (why) msg_format("Generation restarted (%s)", why);

		/* Wipe the objects */
		wipe_o_list();

		/* Wipe the monsters */
		wipe_m_list();

		/* Wipe the fields */
		wipe_f_list();
	}

	/* The dungeon is ready */
	character_dungeon = TRUE;
	
	/* Reset map panels */
	map_panel_size();
	
	/* Verify the panel */
	verify_panel();

	/* Remember when this level was "created" */
	old_turn = turn;
}
