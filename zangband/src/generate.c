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

#include "angband.h"
#define SAFE_MAX_ATTEMPTS 5000

int template_race;

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


/*
 * Dungeon generation values
 */
#define DUN_UNUSUAL 194 /* Level/chance of unusual room (was 200) */
#define DUN_DEST    18  /* 1/chance of having a destroyed level */
#define SMALL_LEVEL 3   /* 1/chance of smaller size (3)*/
#define EMPTY_LEVEL 15  /* 1/chance of being 'empty' (15)*/
#define DARK_EMPTY  5   /* 1/chance of arena level NOT being lit (2)*/

/* Number of rooms to attempt (was 50) */
#define DUN_ROOMS_MIN	10
#define DUN_ROOMS_MAX	50

static int dun_rooms;

/*
 * Dungeon tunnel generation values
 */
#define DUN_TUN_RND_MIN	 5 /* Chance of random direction (was 10) */
#define DUN_TUN_RND_MAX	20
#define DUN_TUN_CHG_MIN	20 /* Chance of changing direction (was 30) */
#define DUN_TUN_CHG_MAX	60
#define DUN_TUN_CON_MIN 10 /* Chance of extra tunneling (was 15) */
#define DUN_TUN_CON_MAX	40
#define DUN_TUN_PEN_MIN 30 /* Chance of doors at room entrances (was 25) */
#define DUN_TUN_PEN_MAX 70
#define DUN_TUN_JCT_MIN 60 /* Chance of doors at tunnel junctions (was 90) */
#define DUN_TUN_JCT_MAX 90

static int dun_tun_rnd;
static int dun_tun_chg;
static int dun_tun_con;
static int dun_tun_pen;
static int dun_tun_jct;

/*
 * Dungeon streamer generation values
 */
#define DUN_STR_DEN     5	/* Density of streamers */
#define DUN_STR_RNG     2	/* Width of streamers */
#define DUN_STR_MAG     3	/* Number of magma streamers */
#define DUN_STR_MC     90	/* 1/chance of treasure per magma */
#define DUN_STR_QUA	    2	/* Number of quartz streamers */
#define DUN_STR_QC     40	/* 1/chance of treasure per quartz */
#define DUN_STR_WLW     1	/* Width of lava & water streamers -KMW- */
#define DUN_STR_DWLW    8	/* Density of water & lava streams -KMW- */

#define DUN_MOS_DEN	2	/* Density of moss streamers */
#define DUN_MOS_RNG	10	/* Width of moss streamers */
#define DUN_STR_MOS	2	/* Number of moss streamers */
#define DUN_WAT_DEN	15	/* Density of rivers */
#define DUN_WAT_RNG	3	/* Width of rivers */
#define DUN_STR_WAT	3	/* Max number of rivers */


/*
 * Dungeon treausre allocation values
 */
#define DUN_AMT_ROOM	9	/* Amount of objects for rooms */
#define DUN_AMT_ITEM	3	/* Amount of objects for rooms/corridors */
#define DUN_AMT_GOLD	3	/* Amount of treasure for rooms/corridors */

/*
 * Hack -- Dungeon allocation "places"
 */
#define ALLOC_SET_CORR		1	/* Hallway */
#define ALLOC_SET_ROOM		2	/* Room */
#define ALLOC_SET_BOTH		3	/* Anywhere */

/*
 * Hack -- Dungeon allocation "types"
 */
#define ALLOC_TYP_RUBBLE	1	/* Rubble */
#define ALLOC_TYP_TRAP		3	/* Trap */
#define ALLOC_TYP_GOLD		4	/* Gold */
#define ALLOC_TYP_OBJECT	5	/* Object */



/*
 * The "size" of a "generation block" in grids
 */
#define BLOCK_HGT	11
#define BLOCK_WID	11

/*
 * Maximum numbers of rooms along each axis (currently 6x6)
 */
#define MAX_ROOMS_ROW	(MAX_HGT / BLOCK_HGT)
#define MAX_ROOMS_COL	(MAX_WID / BLOCK_WID)


/*
 * Bounds on some arrays used in the "dun_data" structure.
 * These bounds are checked, though usually this is a formality.
 */
#define CENT_MAX	100
#define DOOR_MAX	200
#define WALL_MAX	500
#define TUNN_MAX	900


/*
 * Simple structure to hold a map location
 */
typedef struct coord coord;

struct coord
{
	byte y;
	byte x;
};


/*
 * Room type information
 */

typedef struct room_data room_data;

struct room_data
{
	/* Required size in blocks */
	s16b dy1, dy2, dx1, dx2;

	/* Hack -- minimum level */
	s16b level;
};


/*
 * Structure to hold all "dungeon generation" data
 */

typedef struct dun_data dun_data;

struct dun_data
{
	/* Array of centers of rooms */
	int cent_n;
	coord cent[CENT_MAX];

	/* Array of possible door locations */
	int door_n;
	coord door[DOOR_MAX];

	/* Array of wall piercing locations */
	int wall_n;
	coord wall[WALL_MAX];

	/* Array of tunnel grids */
	int tunn_n;
	coord tunn[TUNN_MAX];

	/* Number of blocks along each axis */
	int row_rooms;
	int col_rooms;

	/* Array of which blocks are used */
	bool room_map[MAX_ROOMS_ROW][MAX_ROOMS_COL];

	/* Hack -- there is a pit/nest on this level */
	bool crowded;
};


/*
 * Dungeon generation data -- see "cave_gen()"
 */
static dun_data *dun;


/*
 * Array of room types (assumes 11x11 blocks)
 */
static room_data room[] =
{
	{  0, 0,  0, 0,  0 },		/*  0 = Nothing */
	{  0, 0, -1, 1,  1 },		/*  1 = Simple (33x11) */
	{  0, 0, -1, 1,  1 },		/*  2 = Overlapping (33x11) */
	{  0, 0, -1, 1,  3 },		/*  3 = Crossed (33x11) */
	{  0, 0, -1, 1,  3 },		/*  4 = Large (33x11) */
	{  0, 0, -1, 1,  5 },		/*  5 = Monster nest (33x11) */
	{  0, 0, -1, 1,  5 },		/*  6 = Monster pit (33x11) */
	{  0, 1, -1, 1,  5 },		/*  7 = Lesser vault (33x22) */
	{ -1, 2, -2, 3, 10 },		/*  8 = Greater vault (66x44) */
	{  0, 1,  0, 1,  1 },		/*  9 = Circular (22x22) (Dag Arneson) */
	{  0, 1, -1, 1,  3 },		/* 10 = Labyrinth (11x11) to (33x22) */
};



/*
 * Always picks a correct direction
 */
static void correct_dir(int *rdir, int *cdir, int y1, int x1, int y2, int x2)
{
	/* Extract vertical and horizontal directions */
	*rdir = (y1 == y2) ? 0 : (y1 < y2) ? 1 : -1;
	*cdir = (x1 == x2) ? 0 : (x1 < x2) ? 1 : -1;

	/* Never move diagonally */
	if (*rdir && *cdir)
	{
		if (rand_int(100) < 50)
		{
			*rdir = 0;
		}
		else
		{
			*cdir = 0;
		}
	}
}


/*
 * Pick a random direction
 */
static void rand_dir(int *rdir, int *cdir)
{
	/* Pick a random direction */
	int i = rand_int(4);

	/* Extract the dy/dx components */
	*rdir = ddy_ddd[i];
	*cdir = ddx_ddd[i];
}


/*
 * Returns random co-ordinates for player/monster/object
 */
static bool new_player_spot(void)
{
	int	y, x;
	int max_attempts = 5000;

	/* Place the player */
	while (max_attempts--)
	{
		/* Pick a legal spot */
		y = rand_range(1, cur_hgt - 2);
		x = rand_range(1, cur_wid - 2);

		/* Must be a "naked" floor grid */
		if (!cave_naked_bold(y, x)) continue;

		/* Refuse to start on anti-teleport grids */
		if (cave[y][x].info & (CAVE_ICKY)) continue;

		/* Done */
		break;
	}

	if (max_attempts < 1) /* Should be -1, actually if we failed... */
		return FALSE;


	/* Save the new player grid */
	py = y;
	px = x;

	return TRUE;
}



/*
 * Count the number of walls adjacent to the given grid.
 *
 * Note -- Assumes "in_bounds(y, x)"
 *
 * We count only granite walls and permanent walls.
 */
static int next_to_walls(int y, int x)
{
	int	k = 0;

	if (cave[y+1][x].feat >= FEAT_WALL_EXTRA) k++;
	if (cave[y-1][x].feat >= FEAT_WALL_EXTRA) k++;
	if (cave[y][x+1].feat >= FEAT_WALL_EXTRA) k++;
	if (cave[y][x-1].feat >= FEAT_WALL_EXTRA) k++;

	return (k);
}



/*
 * Convert existing terrain type to rubble
 */
static void place_rubble(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Create rubble */
	c_ptr->feat = FEAT_RUBBLE;
}



/*
 * Convert existing terrain type to "up stairs"
 */
static void place_up_stairs(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Create up stairs */
	c_ptr->feat = FEAT_LESS;
}


/*
 * Convert existing terrain type to "down stairs"
 */
static void place_down_stairs(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Create down stairs */
	c_ptr->feat = FEAT_MORE;
}


/*
 * Place an up/down staircase at given location
 */
static void place_random_stairs(int y, int x)
{
	bool up_stairs = TRUE;
	bool down_stairs = TRUE;


	/* Paranoia */
	if (!cave_clean_bold(y, x)) return;

	/* Town */
	if (!dun_level)
		up_stairs = FALSE;

	/* Ironman */
	if (ironman_downward)
		up_stairs = FALSE;

	/* Bottom */
	if (dun_level >= MAX_DEPTH - 1)
		down_stairs = FALSE;

	/* Quest-level */
	if (quest_number(dun_level) && (dun_level > 1))
		down_stairs = FALSE;

	/* We can't place both */
	if (down_stairs && up_stairs)
	{
		/* Choose a staircase randomly */
		if (rand_int(100) < 50)
			up_stairs = FALSE;
		else
			down_stairs = FALSE;
	}

	/* Place the stairs */
	if (up_stairs)
		place_up_stairs(y, x);
	else if (down_stairs)
		place_down_stairs(y, x);
}


/*
 * Place a locked door at the given location
 */
static void place_locked_door(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Create locked door */
	c_ptr->feat = FEAT_DOOR_HEAD + randint(7);
}


/*
 * Place a secret door at the given location
 */
static void place_secret_door(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Create secret door */
	c_ptr->feat = FEAT_SECRET;
}


/*
 * Place a random type of door at the given location
 */
static void place_random_door(int y, int x)
{
	int tmp;

	/* Choose an object */
	tmp = rand_int(1000);

	/* Open doors (300/1000) */
	if (tmp < 300)
	{
		/* Create open door */
		cave_set_feat(y, x, FEAT_OPEN);
	}

	/* Broken doors (100/1000) */
	else if (tmp < 400)
	{
		/* Create broken door */
		cave_set_feat(y, x, FEAT_BROKEN);
	}

	/* Secret doors (200/1000) */
	else if (tmp < 600)
	{
		/* Create secret door */
		cave_set_feat(y, x, FEAT_SECRET);
	}

	/* Closed, locked, or stuck doors (400/1000) */
	else place_closed_door(y, x);
}


/*
 * Place a random type of normal door at the given location.
 */
void place_closed_door(int y, int x)
{
	int tmp;

	/* Choose an object */
	tmp = rand_int(400);

	/* Closed doors (300/400) */
	if (tmp < 300)
	{
		/* Create closed door */
		cave_set_feat(y, x, FEAT_DOOR_HEAD + 0x00);
	}

	/* Locked doors (99/400) */
	else if (tmp < 399)
	{
		/* Create locked door */
		cave_set_feat(y, x, FEAT_DOOR_HEAD + randint(7));
	}

	/* Stuck doors (1/400) */
	else
	{
		/* Create jammed door */
		cave_set_feat(y, x, FEAT_DOOR_HEAD + 0x08 + rand_int(8));
	}
}


/*
 * Places some staircases near walls
 */
static void alloc_stairs(int feat, int num, int walls)
{
	int         y, x, i, j, flag;
	cave_type   *c_ptr;


	if (feat == FEAT_LESS)
	{
		/* No up stairs in town or in ironman mode */
		if (ironman_downward || !dun_level) return;
	}
	else if (feat == FEAT_MORE)
	{
		/* No downstairs on quest levels */
		if ((dun_level > 1) && quest_number(dun_level)) return;

		/* No downstairs at the bottom */
		if (dun_level >= MAX_DEPTH - 1) return;
	}

	/* Place "num" stairs */
	for (i = 0; i < num; i++)
	{
		/* Place some stairs */
		for (flag = FALSE; !flag; )
		{
			/* Try several times, then decrease "walls" */
			for (j = 0; !flag && j <= 3000; j++)
			{
				/* Pick a random grid */
				y = rand_int(cur_hgt);
				x = rand_int(cur_wid);

				/* Require "naked" floor grid */
				if (!cave_naked_bold(y, x)) continue;

				/* Require a certain number of adjacent walls */
				if (next_to_walls(y, x) < walls) continue;

				/* Access the grid */
				c_ptr = &cave[y][x];

				/* Clear previous contents, add stairs */
				c_ptr->feat = feat;

				/* All done */
				flag = TRUE;
			}

			/* Require fewer walls */
			if (walls) walls--;
		}
	}
}


/*
 * Allocates some objects (using "place" and "type")
 */
static void alloc_object(int set, int typ, int num)
{
	int y, x, k;
	int dummy = 0;

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

			/* Require "naked" floor grid */
			if (!cave_naked_bold(y, x) &&
			    !((y == py) && (x == px))) continue;

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
		}
	}
}


#if 0
/* Taken from Eric Bock's RAngband */
/*
 * Places moss through dungeon.
 */
static void add_moss(void)
{
	int i, tx, ty;
	int y, x, dir;


	/* Hack -- Choose starting point */
	y = rand_spread(cur_hgt / 2, cur_hgt / 2 - 10);
	x = rand_spread(cur_wid / 2, cur_wid / 2 - 10);

	/* Choose a random compass direction */
	dir = ddd[rand_int(8)];

	/* Place streamer into dungeon */
	while (TRUE)
	{
		/* One grid per density */
		for (i = 0; i < DUN_MOS_DEN; i++)
		{
			int d = DUN_MOS_RNG;

			/* Pick a nearby grid */
			while (1)
			{
				ty = rand_spread(y, d);
				tx = rand_spread(x, d);
				if (!in_bounds(ty, tx)) continue;
				break;
			}

			/* Only convert "outer" and "solid" walls and floors */
			if ((cave[ty][tx].feat != FEAT_WALL_OUTER) &&
				 (cave[ty][tx].feat != FEAT_WALL_SOLID) &&
				 (cave[ty][tx].feat != FEAT_FLOOR))
			{
				continue;
			}

			/* Clear previous contents, add proper moss type */
			if (cave[ty][tx].feat == FEAT_FLOOR)
				cave[ty][tx].feat = FEAT_FLOOR_M;
			else
				cave[ty][tx].feat = FEAT_WALL_M;
		}

		/* Advance the streamer */
		y += ddy[dir];
		x += ddx[dir];

		/* Stop at dungeon edge */
		if (!in_bounds(y, x)) break;
	}
}
#endif /* 0 */


/*
 * Places water through dungeon.
 */
static void add_river(int feat1, int feat2, int depth)
{
	int tx, ty;
	int y, x, dir, dd, len, wid;
	int d;

	int ly, lx;
	int ny, nx;

	/* Hack -- Choose starting point */
	ly = y = rand_spread(cur_hgt / 2, cur_hgt / 2 - 10);
	lx = x = rand_spread(cur_wid / 2, cur_wid / 2 - 10);

	/* Choose a random direction */
	dd = rand_int(16);
	dir = cdd[dd / 2];

	wid = randint(DUN_WAT_RNG);

	/* Choose a length */
	len = rand_int(10) + 5;

	/* Find endpoint */
	if (dd % 2 == 0)
	{
		ny = y + len * ddy[dir];
		nx = x + len * ddx[dir];
	}
	/* Acquire the intermediate direction */
	else
	{
		ny = y + len * (ddy[dir] + ddy_cdd[((dd + 1) / 2) % 8]);
		nx = x + len * (ddx[dir] + ddx_cdd[((dd + 1) / 2) % 8]);
	}

	/* Place streamer into dungeon */
	while (TRUE)
	{
		for (ty = y - wid - 1; ty <= y + wid + 1; ty++)
		{
			for (tx = x - wid - 1; tx <= x + wid + 1; tx++)
			{
				if (!in_bounds(ty, tx)) continue;

				if (distance(ty, tx, y, x) > rand_spread(wid, 1)) continue;

				/* Do not convert permanent features */
				if (cave_perma_bold(ty, tx)) continue;

				/* Find the distance from the center */
				if (distance(ty, tx, ny, nx) > distance(ly, lx, ny, nx))
				{
					/* Beginning of river */
					d = distance(ty, tx, ly, lx);
				}
				else
				{
					/* Middle of river */
					d = dist_to_line(ty, tx, ly, lx, ny, nx);
				}

				/*
				 * Clear previous contents, add feature
				 * The border mainly gets feat2, while the center gets feat1
				 */
				if (!wid || ((d > wid * rand_spread(depth, 10) / 100) &&
					 (rand_int(d * 100 / wid) >= depth)))
				{
					cave[ty][tx].feat = feat2;
				}
				else
				{
					cave[ty][tx].feat = feat1;
				}

				/* Lava terrain glows */
				if ((feat1 == FEAT_DEEP_LAVA) ||
				    (feat1 == FEAT_SHAL_LAVA))
				{
					cave[ty][tx].info |= CAVE_GLOW;
				}

				/* Hack -- don't teleport here */
				cave[ty][tx].info |= CAVE_ICKY;
			}
		}

		/* Advance the streamer */
		if (dd % 2 == 0)
		{
			y += ddy[dir];
			x += ddx[dir];
		}
		/* Acquire the intermediate direction */
		else
		{
			y += ddy[dir] + ddy_cdd[((dd + 1) / 2) % 8];
			x += ddx[dir] + ddx_cdd[((dd + 1) / 2) % 8];
		}

		if (len-- == 0)
		{
			/* Change direction slowly */
			dd = rand_spread(dd, 1) % 16;
			dir = cdd[dd / 2];

			wid = rand_spread(wid, 1);

			depth = rand_spread(depth, 10);

			if (wid < 0) wid = 0;
			if (wid > DUN_WAT_RNG) wid = DUN_WAT_RNG;

			if (depth < 0) depth = 0;
			if (depth > 100) depth = 100;

			len = rand_int(10) + 5;

			/* Find starting point */
			ly = y;
			lx = x;

			/* Find endpoint */
			if (dd % 2 == 0)
			{
				ny = y + len * ddy[dir];
				nx = x + len * ddx[dir];
			}
			/* Acquire the intermediate direction */
			else
			{
				ny = y + len * (ddy[dir] + ddy_cdd[((dd + 1) / 2) % 8]);
				nx = x + len * (ddx[dir] + ddx_cdd[((dd + 1) / 2) % 8]);
			}
		}

		/* Stop at dungeon edge */
		if (!in_bounds(y, x)) break;
	}
}


/*
 * Places "streamers" of rock through dungeon
 *
 * Note that their are actually six different terrain features used
 * to represent streamers.  Three each of magma and quartz, one for
 * basic vein, one with hidden gold, and one with known gold.  The
 * hidden gold types are currently unused.
 */
static void build_streamer(int feat, int chance)
{
	int		i, tx, ty;
	int		y, x, dir;
	int dummy = 0;

	cave_type *c_ptr;

	/* Hack -- Choose starting point */
	y = rand_spread(cur_hgt / 2, 10);
	x = rand_spread(cur_wid / 2, 15);

	/* Choose a random compass direction */
	dir = ddd[rand_int(8)];

	/* Place streamer into dungeon */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* One grid per density */
		for (i = 0; i < DUN_STR_DEN; i++)
		{
			int d = DUN_STR_RNG;

			/* Pick a nearby grid */
			while (1)
			{
				ty = rand_spread(y, d);
				tx = rand_spread(x, d);
				if (!in_bounds2(ty, tx)) continue;
				break;
			}

			/* Access the grid */
			c_ptr = &cave[ty][tx];

			/* Only convert "granite" walls */
			if (c_ptr->feat < FEAT_WALL_EXTRA) continue;
			if (c_ptr->feat > FEAT_WALL_SOLID) continue;

			/* Clear previous contents, add proper vein type */
			c_ptr->feat = feat;

			/* Hack -- Add some (known) treasure */
			if (rand_int(chance) == 0) c_ptr->feat += 0x04;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
				msg_print("Warning! Could not place streamer!");
			}
			return;
		}


		/* Advance the streamer */
		y += ddy[dir];
		x += ddx[dir];

		/* Quit before leaving the dungeon */
		if (!in_bounds(y, x)) break;
	}
}



/*
 * Place streams of water, lava, & trees -KMW-
 * This routine varies the placement based on dungeon level
 * otherwise is similar to build_streamer
 */
static void build_streamer2(int feat, bool killwall, bool pool)
{
	int i, j, mid, tx, ty;
	int y, x, dir;
	int poolsize;


	/* Hack -- Choose starting point */
	y = rand_spread(cur_hgt / 2, 10);
	x = rand_spread(cur_wid / 2, 15);

	/* Choose a random compass direction */
	dir = ddd[rand_int(8)];

	if (!pool)
	{
		/* Place streamer into dungeon */
		while (TRUE)
		{
			/* One grid per density */
			for (i = 0; i < (DUN_STR_DWLW + 1); i++)
			{
				int d = DUN_STR_WLW;

				/* Pick a nearby grid */
				while (1)
				{
					ty = rand_spread(y, d);
					tx = rand_spread(x, d);
					if (!in_bounds(ty, tx)) continue;
					break;
				}

				/* Only convert non-permanent features */
				if (killwall)
				{
					if (cave[ty][tx].feat >= FEAT_PERM_EXTRA) continue;
					if (cave[ty][tx].feat == FEAT_LESS) continue;
					if (cave[ty][tx].feat == FEAT_MORE) continue;
				}
				else
				{
					if (cave[ty][tx].feat >= FEAT_MAGMA) continue;
					if (cave[ty][tx].feat == FEAT_LESS) continue;
					if (cave[ty][tx].feat == FEAT_MORE) continue;
				}

				/* Clear previous contents, add proper vein type */
				cave[ty][tx].feat = feat;
			}

			/* Advance the streamer */
			y += ddy[dir];
			x += ddx[dir];

			if (randint(20) == 1)
				dir = ddd[rand_int(8)]; /* change direction */

			/* Stop at dungeon edge */
			if (!in_bounds(y, x)) break;
		}
	}
	else
	{
		/* create pool */
		poolsize = 5 + randint(10);
		mid = poolsize / 2;

		/* One grid per density */
		for (i = 0; i < poolsize; i++)
		{
			for (j = 0; j < poolsize; j++)
			{
				tx = x + j;
				ty = y + i;

				if (!in_bounds(ty, tx)) continue;

				if (i < mid)
				{
					if (j < mid)
					{
						if ((i + j + 1) < mid)
							continue;
					}
					else if (j > (mid+ i))
						continue;
				}
				else if (j < mid)
				{
					if (i > (mid + j))
						continue;
				}
				else if ((i + j) > ((mid * 3)-1))
					continue;

				/* Only convert non-permanent features */
				if (killwall)
				{
					if (cave[ty][tx].feat >= FEAT_PERM_EXTRA) continue;
					if (cave[ty][tx].feat == FEAT_LESS) continue;
					if (cave[ty][tx].feat == FEAT_MORE) continue;
				}
				else
				{
					if (cave[ty][tx].feat >= FEAT_MAGMA) continue;
					if (cave[ty][tx].feat == FEAT_LESS) continue;
					if (cave[ty][tx].feat == FEAT_MORE) continue;
				}

				/* Clear previous contents, add proper vein type */
				cave[ty][tx].feat = feat;
			}
		}
	}
}



/*
 * Build a destroyed level
 */
static void destroy_level(void)
{
	int y1, x1, y, x, k, t, n;

	cave_type *c_ptr;

	/* Note destroyed levels */
	if (cheat_room) msg_print("Destroyed Level");

	/* Drop a few epi-centers (usually about two) */
	for (n = 0; n < randint(5); n++)
	{
		/* Pick an epi-center */
		x1 = rand_range(5, cur_wid-1 - 5);
		y1 = rand_range(5, cur_hgt-1 - 5);

		/* Big area of affect */
		for (y = (y1 - 15); y <= (y1 + 15); y++)
		{
			for (x = (x1 - 15); x <= (x1 + 15); x++)
			{
				/* Skip illegal grids */
				if (!in_bounds(y, x)) continue;

				/* Extract the distance */
				k = distance(y1, x1, y, x);

				/* Stay in the circle of death */
				if (k >= 16) continue;

				/* Delete the monster (if any) */
				delete_monster(y, x);

				/* Destroy valid grids */
				if (cave_valid_bold(y, x))
				{
					/* Delete objects */
					delete_object(y, x);

					/* Access the grid */
					c_ptr = &cave[y][x];

					/* Wall (or floor) type */
					t = rand_int(200);

					/* Granite */
					if (t < 20)
					{
						/* Create granite wall */
						c_ptr->feat = FEAT_WALL_EXTRA;
					}

					/* Quartz */
					else if (t < 70)
					{
						/* Create quartz vein */
						c_ptr->feat = FEAT_QUARTZ;
					}

					/* Magma */
					else if (t < 100)
					{
						/* Create magma vein */
						c_ptr->feat = FEAT_MAGMA;
					}

					/* Floor */
					else
					{
						/* Create floor */
						c_ptr->feat = FEAT_FLOOR;
					}

					/* No longer part of a room or vault */
					c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

					/* No longer illuminated or known */
					c_ptr->info &= ~(CAVE_MARK | CAVE_GLOW);
				}
			}
		}
	}
}



/*
 * Create up to "num" objects near the given coordinates
 * Only really called by some of the "vault" routines.
 */
static void vault_objects(int y, int x, int num)
{
	int dummy = 0;
	int i = 0, j = y, k = x;


	/* Attempt to place 'num' objects */
	for (; num > 0; --num)
	{
		/* Try up to 11 spots looking for empty space */
		for (i = 0; i < 11; ++i)
		{
			/* Pick a random location */
			while (dummy < SAFE_MAX_ATTEMPTS)
			{
				j = rand_spread(y, 2);
				k = rand_spread(x, 3);
				dummy++;
				if (!in_bounds(j, k)) continue;
				break;
			}


			if (dummy >= SAFE_MAX_ATTEMPTS)
			{
				if (cheat_room)
				{
					msg_print("Warning! Could not place vault object!");
				}
			}


			/* Require "clean" floor space */
			if (!cave_clean_bold(j, k)) continue;

			/* Place an item */
			if (rand_int(100) < 75)
			{
				place_object(j, k, FALSE, FALSE);
			}

			/* Place gold */
			else
			{
				place_gold(j, k);
			}

			/* Placement accomplished */
			break;
		}
	}
}


/*
 * Place a trap with a given displacement of point
 */
static void vault_trap_aux(int y, int x, int yd, int xd)
{
	int count = 0, y1 = y, x1 = x;
	int dummy = 0;

	/* Place traps */
	for (count = 0; count <= 5; count++)
	{
		/* Get a location */
		while (dummy < SAFE_MAX_ATTEMPTS)
		{
			y1 = rand_spread(y, yd);
			x1 = rand_spread(x, xd);
			dummy++;
			if (!in_bounds(y1, x1)) continue;
			break;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
				msg_print("Warning! Could not place vault trap!");
			}
		}


		/* Require "naked" floor grids */
		if (!cave_naked_bold(y1, x1)) continue;

		/* Place the trap */
		place_trap(y1, x1);

		/* Done */
		break;
	}
}


/*
 * Place some traps with a given displacement of given location
 */
static void vault_traps(int y, int x, int yd, int xd, int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		vault_trap_aux(y, x, yd, xd);
	}
}


/*
 * Hack -- Place some sleeping monsters near the given location
 */
static void vault_monsters(int y1, int x1, int num)
{
	int k, i, y, x;

	/* Try to summon "num" monsters "near" the given location */
	for (k = 0; k < num; k++)
	{
		/* Try nine locations */
		for (i = 0; i < 9; i++)
		{
			int d = 1;

			/* Pick a nearby location */
			scatter(&y, &x, y1, x1, d, 0);

			/* Require "empty" floor grids */
			if (!cave_empty_bold(y, x)) continue;

			/* Place the monster (allow groups) */
			monster_level = base_level + 2;
			(void)place_monster(y, x, TRUE, TRUE);
			monster_level = base_level;
		}
	}
}




/*
 * Room building routines.
 *
 * Room types:
 *   1 -- normal
 *   2 -- overlapping
 *   3 -- cross shaped
 *   4 -- large room with features
 *   5 -- monster nests
 *   6 -- monster pits
 *   7 -- simple vaults
 *   8 -- greater vaults
 *   9 -- circular rooms
 *  10 -- labyrinthine rooms
 */


/*
 * Type 1 -- normal rectangular rooms
 */
static void build_type1(int yval, int xval)
{
	int y, x, y2, x2;
	int y1, x1;

	bool light;

	cave_type *c_ptr;


	/* Choose lite or dark */
	light = (dun_level <= randint(25));


	/* Pick a room size */
	y1 = yval - randint(4);
	y2 = yval + randint(3);
	x1 = xval - randint(11);
	x2 = xval + randint(11);


	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Walls around the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}


	/* Hack -- Occasional pillar room */
	if (rand_int(20) == 0)
	{
		for (y = y1; y <= y2; y += 2)
		{
			for (x = x1; x <= x2; x += 2)
			{
				c_ptr = &cave[y][x];
				c_ptr->feat = FEAT_WALL_INNER;
			}
		}
	}

	/* Hack -- Occasional room with four pillars */
	else if (rand_int(20) == 0)
	{
		if ((y1 + 4 < y2) && (x1 + 4 < x2))
		{
			c_ptr = &cave[y1 + 1][x1 + 1];
			c_ptr->feat = FEAT_WALL_INNER;

			c_ptr = &cave[y1 + 1][x2 - 1];
			c_ptr->feat = FEAT_WALL_INNER;

			c_ptr = &cave[y2 - 1][x1 + 1];
			c_ptr->feat = FEAT_WALL_INNER;

			c_ptr = &cave[y2 - 1][x2 - 1];
			c_ptr->feat = FEAT_WALL_INNER;
		}
	}

	/* Hack -- Occasional ragged-edge room */
	else if (rand_int(50) == 0)
	{
		for (y = y1 + 2; y <= y2 - 2; y += 2)
		{
			c_ptr = &cave[y][x1];
			c_ptr->feat = FEAT_WALL_INNER;
			c_ptr = &cave[y][x2];
			c_ptr->feat = FEAT_WALL_INNER;
		}
		for (x = x1 + 2; x <= x2 - 2; x += 2)
		{
			c_ptr = &cave[y1][x];
			c_ptr->feat = FEAT_WALL_INNER;
			c_ptr = &cave[y2][x];
			c_ptr->feat = FEAT_WALL_INNER;
		}
	}
}


/*
 * Type 2 -- Overlapping rectangular rooms
 */
static void build_type2(int yval, int xval)
{
	int			y, x;
	int			y1a, x1a, y2a, x2a;
	int			y1b, x1b, y2b, x2b;

	bool		light;

	cave_type *c_ptr;



	/* Choose lite or dark */
	light = (dun_level <= randint(25));


	/* Determine extents of the first room */
	y1a = yval - randint(4);
	y2a = yval + randint(3);
	x1a = xval - randint(11);
	x2a = xval + randint(10);

	/* Determine extents of the second room */
	y1b = yval - randint(3);
	y2b = yval + randint(4);
	x1b = xval - randint(10);
	x2b = xval + randint(11);


	/* Place a full floor for room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		for (x = x1a - 1; x <= x2a + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Place a full floor for room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		for (x = x1b - 1; x <= x2b + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}


	/* Place the walls around room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		c_ptr = &cave[y][x1a-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2a+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1a - 1; x <= x2a + 1; x++)
	{
		c_ptr = &cave[y1a-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2a+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}

	/* Place the walls around room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		c_ptr = &cave[y][x1b-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2b+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1b - 1; x <= x2b + 1; x++)
	{
		c_ptr = &cave[y1b-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2b+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}



	/* Replace the floor for room "a" */
	for (y = y1a; y <= y2a; y++)
	{
		for (x = x1a; x <= x2a; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
		}
	}

	/* Replace the floor for room "b" */
	for (y = y1b; y <= y2b; y++)
	{
		for (x = x1b; x <= x2b; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
		}
	}
}



/*
 * Type 3 -- Cross shaped rooms
 *
 * Builds a room at a row, column coordinate
 *
 * Room "a" runs north/south, and Room "b" runs east/east
 * So the "central pillar" runs from x1a,y1b to x2a,y2b.
 *
 * Note that currently, the "center" is always 3x3, but I think that
 * the code below will work (with "bounds checking") for 5x5, or even
 * for unsymetric values like 4x3 or 5x3 or 3x4 or 3x5, or even larger.
 */
static void build_type3(int yval, int xval)
{
	int			y, x, dy, dx, wy, wx;
	int			y1a, x1a, y2a, x2a;
	int			y1b, x1b, y2b, x2b;

	bool		light;

	cave_type *c_ptr;



	/* Choose lite or dark */
	light = (dun_level <= randint(25));


	/* For now, always 3x3 */
	wx = wy = 1;

	/* Pick max vertical size (at most 4) */
	dy = rand_range(3, 4);

	/* Pick max horizontal size (at most 15) */
	dx = rand_range(3, 11);


	/* Determine extents of the north/south room */
	y1a = yval - dy;
	y2a = yval + dy;
	x1a = xval - wx;
	x2a = xval + wx;

	/* Determine extents of the east/west room */
	y1b = yval - wy;
	y2b = yval + wy;
	x1b = xval - dx;
	x2b = xval + dx;


	/* Place a full floor for room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		for (x = x1a - 1; x <= x2a + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Place a full floor for room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		for (x = x1b - 1; x <= x2b + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}


	/* Place the walls around room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		c_ptr = &cave[y][x1a-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2a+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1a - 1; x <= x2a + 1; x++)
	{
		c_ptr = &cave[y1a-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2a+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}

	/* Place the walls around room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		c_ptr = &cave[y][x1b-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2b+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1b - 1; x <= x2b + 1; x++)
	{
		c_ptr = &cave[y1b-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2b+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}


	/* Replace the floor for room "a" */
	for (y = y1a; y <= y2a; y++)
	{
		for (x = x1a; x <= x2a; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
		}
	}

	/* Replace the floor for room "b" */
	for (y = y1b; y <= y2b; y++)
	{
		for (x = x1b; x <= x2b; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
		}
	}



	/* Special features (3/4) */
	switch (rand_int(4))
	{
		/* Large solid middle pillar */
		case 1:
		{
			for (y = y1b; y <= y2b; y++)
			{
				for (x = x1a; x <= x2a; x++)
				{
					c_ptr = &cave[y][x];
					c_ptr->feat = FEAT_WALL_INNER;
				}
			}
			break;
		}

		/* Inner treasure vault */
		case 2:
		{
			/* Build the vault */
			for (y = y1b; y <= y2b; y++)
			{
				c_ptr = &cave[y][x1a];
				c_ptr->feat = FEAT_WALL_INNER;
				c_ptr = &cave[y][x2a];
				c_ptr->feat = FEAT_WALL_INNER;
			}
			for (x = x1a; x <= x2a; x++)
			{
				c_ptr = &cave[y1b][x];
				c_ptr->feat = FEAT_WALL_INNER;
				c_ptr = &cave[y2b][x];
				c_ptr->feat = FEAT_WALL_INNER;
			}

			/* Place a secret door on the inner room */
			switch (rand_int(4))
			{
				case 0: place_secret_door(y1b, xval); break;
				case 1: place_secret_door(y2b, xval); break;
				case 2: place_secret_door(yval, x1a); break;
				case 3: place_secret_door(yval, x2a); break;
			}

			/* Place a treasure in the vault */
			place_object(yval, xval, FALSE, FALSE);

			/* Let's guard the treasure well */
			vault_monsters(yval, xval, rand_int(2) + 3);

			/* Traps naturally */
			vault_traps(yval, xval, 4, 4, rand_int(3) + 2);

			break;
		}

		/* Something else */
		case 3:
		{
			/* Occasionally pinch the center shut */
			if (rand_int(3) == 0)
			{
				/* Pinch the east/west sides */
				for (y = y1b; y <= y2b; y++)
				{
					if (y == yval) continue;
					c_ptr = &cave[y][x1a - 1];
					c_ptr->feat = FEAT_WALL_INNER;
					c_ptr = &cave[y][x2a + 1];
					c_ptr->feat = FEAT_WALL_INNER;
				}

				/* Pinch the north/south sides */
				for (x = x1a; x <= x2a; x++)
				{
					if (x == xval) continue;
					c_ptr = &cave[y1b - 1][x];
					c_ptr->feat = FEAT_WALL_INNER;
					c_ptr = &cave[y2b + 1][x];
					c_ptr->feat = FEAT_WALL_INNER;
				}

				/* Sometimes shut using secret doors */
				if (rand_int(3) == 0)
				{
					place_secret_door(yval, x1a - 1);
					place_secret_door(yval, x2a + 1);
					place_secret_door(y1b - 1, xval);
					place_secret_door(y2b + 1, xval);
				}
			}

			/* Occasionally put a "plus" in the center */
			else if (rand_int(3) == 0)
			{
				c_ptr = &cave[yval][xval];
				c_ptr->feat = FEAT_WALL_INNER;
				c_ptr = &cave[y1b][xval];
				c_ptr->feat = FEAT_WALL_INNER;
				c_ptr = &cave[y2b][xval];
				c_ptr->feat = FEAT_WALL_INNER;
				c_ptr = &cave[yval][x1a];
				c_ptr->feat = FEAT_WALL_INNER;
				c_ptr = &cave[yval][x2a];
				c_ptr->feat = FEAT_WALL_INNER;
			}

			/* Occasionally put a pillar in the center */
			else if (rand_int(3) == 0)
			{
				c_ptr = &cave[yval][xval];
				c_ptr->feat = FEAT_WALL_INNER;
			}

			break;
		}
	}
}


/*
 * Type 4 -- Large room with inner features
 *
 * Possible sub-types:
 *	1 - Just an inner room with one door
 *	2 - An inner room within an inner room
 *	3 - An inner room with pillar(s)
 *	4 - Inner room has a maze
 *	5 - A set of four inner rooms
 */
static void build_type4(int yval, int xval)
{
	int y, x, y1, x1;
	int y2, x2, tmp;

	bool light;

	cave_type *c_ptr;

	/* Choose lite or dark */
	light = (dun_level <= randint(25));

	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Outer Walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}


	/* The inner room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1-1];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y][x2+1];
		c_ptr->feat = FEAT_WALL_INNER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1-1][x];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y2+1][x];
		c_ptr->feat = FEAT_WALL_INNER;
	}


	/* Inner room variations */
	switch (randint(5))
	{
		/* Just an inner room with a monster */
		case 1:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Place a monster in the room */
		vault_monsters(yval, xval, 1);

		break;


		/* Treasure Vault (with a door) */
		case 2:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Place another inner room */
		for (y = yval - 1; y <= yval + 1; y++)
		{
			for (x = xval -  1; x <= xval + 1; x++)
			{
				if ((x == xval) && (y == yval)) continue;
				c_ptr = &cave[y][x];
				c_ptr->feat = FEAT_WALL_INNER;
			}
		}

		/* Place a locked door on the inner room */
		switch (randint(4))
		{
			case 1: place_locked_door(yval - 1, xval); break;
			case 2: place_locked_door(yval + 1, xval); break;
			case 3: place_locked_door(yval, xval - 1); break;
			case 4: place_locked_door(yval, xval + 1); break;
		}

		/* Monsters to guard the "treasure" */
		vault_monsters(yval, xval, randint(3) + 2);

		/* Object (80%) */
		if (rand_int(100) < 80)
		{
			place_object(yval, xval, FALSE, FALSE);
		}

		/* Stairs (20%) */
		else
		{
			place_random_stairs(yval, xval);
		}

		/* Traps to protect the treasure */
		vault_traps(yval, xval, 4, 10, 2 + randint(3));

		break;


		/* Inner pillar(s). */
		case 3:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Large Inner Pillar */
		for (y = yval - 1; y <= yval + 1; y++)
		{
			for (x = xval - 1; x <= xval + 1; x++)
			{
				c_ptr = &cave[y][x];
				c_ptr->feat = FEAT_WALL_INNER;
			}
		}

		/* Occasionally, two more Large Inner Pillars */
		if (rand_int(2) == 0)
		{
			tmp = randint(2);
			for (y = yval - 1; y <= yval + 1; y++)
			{
				for (x = xval - 5 - tmp; x <= xval - 3 - tmp; x++)
				{
					c_ptr = &cave[y][x];
					c_ptr->feat = FEAT_WALL_INNER;
				}
				for (x = xval + 3 + tmp; x <= xval + 5 + tmp; x++)
				{
					c_ptr = &cave[y][x];
					c_ptr->feat = FEAT_WALL_INNER;
				}
			}
		}

		/* Occasionally, some Inner rooms */
		if (rand_int(3) == 0)
		{
			/* Long horizontal walls */
			for (x = xval - 5; x <= xval + 5; x++)
			{
				c_ptr = &cave[yval-1][x];
				c_ptr->feat = FEAT_WALL_INNER;
				c_ptr = &cave[yval+1][x];
				c_ptr->feat = FEAT_WALL_INNER;
			}

			/* Close off the left/right edges */
			c_ptr = &cave[yval][xval-5];
			c_ptr->feat = FEAT_WALL_INNER;
			c_ptr = &cave[yval][xval+5];
			c_ptr->feat = FEAT_WALL_INNER;

			/* Secret doors (random top/bottom) */
			place_secret_door(yval - 3 + (randint(2) * 2), xval - 3);
			place_secret_door(yval - 3 + (randint(2) * 2), xval + 3);

			/* Monsters */
			vault_monsters(yval, xval - 2, randint(2));
			vault_monsters(yval, xval + 2, randint(2));

			/* Objects */
			if (rand_int(3) == 0) place_object(yval, xval - 2, FALSE, FALSE);
			if (rand_int(3) == 0) place_object(yval, xval + 2, FALSE, FALSE);
		}

		break;


		/* Maze inside. */
		case 4:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Maze (really a checkerboard) */
		for (y = y1; y <= y2; y++)
		{
			for (x = x1; x <= x2; x++)
			{
				if (0x1 & (x + y))
				{
					c_ptr = &cave[y][x];
					c_ptr->feat = FEAT_WALL_INNER;
				}
			}
		}

		/* Monsters just love mazes. */
		vault_monsters(yval, xval - 5, randint(3));
		vault_monsters(yval, xval + 5, randint(3));

		/* Traps make them entertaining. */
		vault_traps(yval, xval - 3, 2, 8, randint(3));
		vault_traps(yval, xval + 3, 2, 8, randint(3));

		/* Mazes should have some treasure too. */
		vault_objects(yval, xval, 3);

		break;


		/* Four small rooms. */
		case 5:

		/* Inner "cross" */
		for (y = y1; y <= y2; y++)
		{
			c_ptr = &cave[y][xval];
			c_ptr->feat = FEAT_WALL_INNER;
		}
		for (x = x1; x <= x2; x++)
		{
			c_ptr = &cave[yval][x];
			c_ptr->feat = FEAT_WALL_INNER;
		}

		/* Doors into the rooms */
		if (rand_int(100) < 50)
		{
			int i = randint(10);
			place_secret_door(y1 - 1, xval - i);
			place_secret_door(y1 - 1, xval + i);
			place_secret_door(y2 + 1, xval - i);
			place_secret_door(y2 + 1, xval + i);
		}
		else
		{
			int i = randint(3);
			place_secret_door(yval + i, x1 - 1);
			place_secret_door(yval - i, x1 - 1);
			place_secret_door(yval + i, x2 + 1);
			place_secret_door(yval - i, x2 + 1);
		}

		/* Treasure, centered at the center of the cross */
		vault_objects(yval, xval, 2 + randint(2));

		/* Gotta have some monsters. */
		vault_monsters(yval + 1, xval - 4, randint(4));
		vault_monsters(yval + 1, xval + 4, randint(4));
		vault_monsters(yval - 1, xval - 4, randint(4));
		vault_monsters(yval - 1, xval + 4, randint(4));

		break;
	}
}


/*
 * The following functions are used to determine if the given monster
 * is appropriate for inclusion in a monster nest or monster pit or
 * the given type.
 *
 * None of the pits/nests are allowed to include "unique" monsters,
 * or monsters which can "multiply".
 *
 * Some of the pits/nests are asked to avoid monsters which can blink
 * away or which are invisible.  This is probably a hack.
 *
 * The old method made direct use of monster "names", which is bad.
 *
 * Note the use of Angband 2.7.9 monster race pictures in various places.
 */


/*
 * Helper function for "monster nest (jelly)"
 */
static bool vault_aux_jelly(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Also decline evil jellies (like death molds and shoggoths) */
	if (r_ptr->flags3 & (RF3_EVIL)) return (FALSE);

	/* Require icky thing, jelly, mold, or mushroom */
	if (!strchr("ijm,", r_ptr->d_char)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (animal)"
 */
static bool vault_aux_animal(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require "animal" flag */
	if (!(r_ptr->flags3 & (RF3_ANIMAL))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (undead)"
 */
static bool vault_aux_undead(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require Undead */
	if (!(r_ptr->flags3 & (RF3_UNDEAD))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (chapel)"
 */
static bool vault_aux_chapel(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require "priest" or Angel */
	if (!((r_ptr->d_char == 'A') ||
		strstr((r_name + r_ptr->name),"riest")))
	{
		return (FALSE);
	}

	/* Okay */
	return (TRUE);
}

/*
 * Helper function for "monster nest (kennel)"
 */
static bool vault_aux_kennel(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require a Zephyr Hound or a dog */
	return ((r_ptr->d_char == 'Z') || (r_ptr->d_char == 'C'));

}

/*
 * Helper function for "monster nest (treasure)"
 */
static bool vault_aux_treasure(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require "priest" or Angel */
	if (!((r_ptr->d_char == '!') || (r_ptr->d_char == '|') ||
		(r_ptr->d_char == '$') || (r_ptr->d_char == '?') ||
		(r_ptr->d_char == '=')))
	{
		return (FALSE);
	}

	/* Okay */
	return (TRUE);
}

/*
 * Helper function for "monster nest (clone)"
 */
static bool vault_aux_clone(int r_idx)
{
	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	return (r_idx == template_race);
}


/*
 * Helper function for "monster nest (symbol clone)"
 */
static bool vault_aux_symbol(int r_idx)
{
	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	return ((r_info[r_idx].d_char == (r_info[template_race].d_char))
		&& !(r_info[r_idx].flags1 & RF1_UNIQUE));
}


/*
 * Helper function for "monster pit (orc)"
 */
static bool vault_aux_orc(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require orc */
	if (!(r_ptr->flags3 & RF3_ORC)) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}



/*
 * Helper function for "monster pit (troll)"
 */
static bool vault_aux_troll(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline aquatic monsters */
	if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & RF1_UNIQUE) return (FALSE);

	/* Require troll */
	if (!(r_ptr->flags3 & RF3_TROLL)) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (giant)"
 */
static bool vault_aux_giant(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require giant */
	if (!(r_ptr->flags3 & RF3_GIANT)) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Hack -- breath type for "vault_aux_dragon()"
 */
static u32b vault_aux_dragon_mask4;


/*
 * Helper function for "monster pit (dragon)"
 */
static bool vault_aux_dragon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require dragon */
	if (!(r_ptr->flags3 & RF3_DRAGON)) return (FALSE);

	/* Hack -- Require correct "breath attack" */
	if (r_ptr->flags4 != vault_aux_dragon_mask4) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (demon)"
 */
static bool vault_aux_demon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
	if (!monster_dungeon(r_idx)) return FALSE;

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require demon */
	if (!(r_ptr->flags3 & RF3_DEMON)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Type 5 -- Monster nests
 *
 * A monster nest is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type strewn about the room.
 *
 * The monsters are chosen from a set of 64 randomly selected monster
 * races, to allow the nest creation to fail instead of having "holes".
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the nest.
 *
 * Currently, a monster nest is one of
 *   a nest of "jelly" monsters   (Dungeon level 5 and deeper)
 *   a nest of "animal" monsters  (Dungeon level 30 and deeper)
 *   a nest of "undead" monsters  (Dungeon level 50 and deeper)
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which
 * case the nest will be empty, and will not affect the level rating.
 *
 * Note that "monster nests" will never contain "unique" monsters.
 */
static void build_type5(int yval, int xval)
{
	int         y, x, y1, x1, y2, x2;
	int         tmp, i;
	s16b        what[64];
	cave_type   *c_ptr;
	cptr        name;
	bool        empty = FALSE;
	monster_hook_type vault_monster_hook;


	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;


	/* Place the floor area */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
		}
	}

	/* Place the outer walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}


	/* Advance to the center room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1-1];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y][x2+1];
		c_ptr->feat = FEAT_WALL_INNER;
	}

	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1-1][x];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y2+1][x];
		c_ptr->feat = FEAT_WALL_INNER;
	}


	/* Place a secret door */
	switch (randint(4))
	{
		case 1: place_secret_door(y1 - 1, xval); break;
		case 2: place_secret_door(y2 + 1, xval); break;
		case 3: place_secret_door(yval, x1 - 1); break;
		case 4: place_secret_door(yval, x2 + 1); break;
	}


	/* Hack -- Choose a nest type */
	tmp = randint(dun_level);

	if ((tmp < 25) && (randint(2) != 1))
	{
		do
		{
			template_race = randint(max_r_idx - 1);
		}
		while ((r_info[template_race].flags1 & RF1_UNIQUE) ||
		       ((r_info[template_race].level + randint(5)) > (dun_level + randint(5))));

		if ((randint(2) != 1) && (dun_level >= (25 + randint(15))))
		{
			name = "symbol clone";
			vault_monster_hook = vault_aux_symbol;
		}
		else
		{
			name = "clone";
			vault_monster_hook = vault_aux_clone;
		}
	}

	/* Monster nest (jelly) */
	else if (tmp < 25)
	{
		/* Describe */
		name = "jelly";

		/* Restrict to jelly */
		vault_monster_hook = vault_aux_jelly;
	}

	else if (tmp < 50)
	{
		name = "treasure";
		vault_monster_hook = vault_aux_treasure;
	}

	/* Monster nest (animal) */
	else if (tmp < 65)
	{
		if (randint(3) == 1)
		{
			name = "kennel";
			vault_monster_hook = vault_aux_kennel;
		}
		else
		{
			/* Describe */
			name = "animal";

			/* Restrict to animal */
			vault_monster_hook = vault_aux_animal;
		}
	}

	/* Monster nest (undead) */
	else
	{
		if (randint(3) == 1)
		{
			name = "chapel";
			vault_monster_hook = vault_aux_chapel;
		}
		else
		{
			/* Describe */
			name = "undead";

			/* Restrict to undead */
			vault_monster_hook = vault_aux_undead;
		}
	}


	/* Prepare allocation table */
	get_mon_num_prep(vault_monster_hook, NULL);


	/* Pick some monster types */
	for (i = 0; i < 64; i++)
	{
		/* Get a (hard) monster type */
		what[i] = get_mon_num(dun_level + 10);

		/* Notice failure */
		if (!what[i]) empty = TRUE;
	}


	/* Oops */
	if (empty) return;


	/* Describe */
	if (cheat_room)
	{
		/* Room type */
		msg_format("Monster nest (%s)", name);
	}


	/* Increase the level rating */
	rating += 10;

	/* (Sometimes) Cause a "special feeling" (for "Monster Nests") */
	if ((dun_level <= 40) && (randint(dun_level * dun_level + 50) < 300))
	{
		good_item_flag = TRUE;
	}


	/* Place some monsters */
	for (y = yval - 2; y <= yval + 2; y++)
	{
		for (x = xval - 9; x <= xval + 9; x++)
		{
			int r_idx = what[rand_int(64)];

			/* Place that "random" monster (no groups) */
			(void)place_monster_aux(y, x, r_idx, FALSE, FALSE, FALSE, FALSE);
		}
	}
}



/*
 * Type 6 -- Monster pits
 *
 * A monster pit is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type organized in the room.
 *
 * Monster types in the pit  (list out of date...)
 *   orc pit	(Dungeon Level 5 and deeper)
 *   troll pit	(Dungeon Level 20 and deeper)
 *   giant pit	(Dungeon Level 40 and deeper)
 *   dragon pit	(Dungeon Level 60 and deeper)
 *   demon pit	(Dungeon Level 80 and deeper)
 *
 * The inside room in a monster pit appears as shown below, where the
 * actual monsters in each location depend on the type of the pit
 *
 *   #####################
 *   #0000000000000000000#
 *   #0112233455543322110#
 *   #0112233467643322110#
 *   #0112233455543322110#
 *   #0000000000000000000#
 *   #####################
 *
 * Note that the monsters in the pit are now chosen by using "get_mon_num()"
 * to request 16 "appropriate" monsters, sorting them by level, and using
 * the "even" entries in this sorted list for the contents of the pit.
 *
 * Hack -- all of the "dragons" in a "dragon" pit must be the same "color",
 * which is handled by requiring a specific "breath" attack for all of the
 * dragons.  This may include "multi-hued" breath.  Note that "wyrms" may
 * be present in many of the dragon pits, if they have the proper breath.
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the pit.
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which case
 * the pit will be empty, and will not effect the level rating.
 *
 * Note that "monster pits" will never contain "unique" monsters.
 */
static void build_type6(int yval, int xval)
{
	int         tmp, what[16];
	int         i, j, y, x, y1, x1, y2, x2;
	bool        empty = FALSE;
	cave_type   *c_ptr;
	cptr        name;
	monster_hook_type vault_monster_hook;


	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Place the floor area */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
		}
	}

	/* Place the outer walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}


	/* Advance to the center room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1-1];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y][x2+1];
		c_ptr->feat = FEAT_WALL_INNER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1-1][x];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y2+1][x];
		c_ptr->feat = FEAT_WALL_INNER;
	}


	/* Place a secret door */
	switch (randint(4))
	{
		case 1: place_secret_door(y1 - 1, xval); break;
		case 2: place_secret_door(y2 + 1, xval); break;
		case 3: place_secret_door(yval, x1 - 1); break;
		case 4: place_secret_door(yval, x2 + 1); break;
	}


	/* Choose a pit type */
	tmp = randint(dun_level);

	/* Orc pit */
	if (tmp < 20)
	{
		/* Message */
		name = "orc";

		/* Restrict monster selection */
		vault_monster_hook = vault_aux_orc;
	}

	/* Troll pit */
	else if (tmp < 40)
	{
		/* Message */
		name = "troll";

		/* Restrict monster selection */
		vault_monster_hook = vault_aux_troll;
	}

	/* Giant pit */
	else if (tmp < 55)
	{
		/* Message */
		name = "giant";

		/* Restrict monster selection */
		vault_monster_hook = vault_aux_giant;
	}

	else if (tmp < 70)
	{
		if (randint(4) != 1)
		{
			/* Message */
			name = "ordered clones";

			do
			{
				template_race = randint(max_r_idx - 1);
			}
			while ((r_info[template_race].flags1 & RF1_UNIQUE) ||
			  (((r_info[template_race].level) + randint(5)) > (dun_level + randint(5))));

			/* Restrict selection */
			vault_monster_hook = vault_aux_symbol;
		}
		else
		{
			name = "ordered chapel";
			vault_monster_hook = vault_aux_chapel;
		}
	}


	/* Dragon pit */
	else if (tmp < 80)
	{
		/* Pick dragon type */
		switch (rand_int(6))
		{
			/* Black */
			case 0:
			{
				/* Message */
				name = "acid dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_ACID;

				/* Done */
				break;
			}

			/* Blue */
			case 1:
			{
				/* Message */
				name = "electric dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_ELEC;

				/* Done */
				break;
			}

			/* Red */
			case 2:
			{
				/* Message */
				name = "fire dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_FIRE;

				/* Done */
				break;
			}

			/* White */
			case 3:
			{
				/* Message */
				name = "cold dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_COLD;

				/* Done */
				break;
			}

			/* Green */
			case 4:
			{
				/* Message */
				name = "poison dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_POIS;

				/* Done */
				break;
			}

			/* Multi-hued */
			default:
			{
				/* Message */
				name = "multi-hued dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = (RF4_BR_ACID | RF4_BR_ELEC |
				                          RF4_BR_FIRE | RF4_BR_COLD |
				                          RF4_BR_POIS);

				/* Done */
				break;
			}

		}

		/* Restrict monster selection */
		vault_monster_hook = vault_aux_dragon;
	}

	/* Demon pit */
	else
	{
		/* Message */
		name = "demon";

		/* Restrict monster selection */
		vault_monster_hook = vault_aux_demon;
	}

	/* Prepare allocation table */
	get_mon_num_prep(vault_monster_hook, NULL);


	/* Pick some monster types */
	for (i = 0; i < 16; i++)
	{
		/* Get a (hard) monster type */
		what[i] = get_mon_num(dun_level + 10);

		/* Notice failure */
		if (!what[i]) empty = TRUE;
	}


	/* Oops */
	if (empty) return;


	/* XXX XXX XXX */
	/* Sort the entries */
	for (i = 0; i < 16 - 1; i++)
	{
		/* Sort the entries */
		for (j = 0; j < 16 - 1; j++)
		{
			int i1 = j;
			int i2 = j + 1;

			int p1 = r_info[what[i1]].level;
			int p2 = r_info[what[i2]].level;

			/* Bubble */
			if (p1 > p2)
			{
				int tmp = what[i1];
				what[i1] = what[i2];
				what[i2] = tmp;
			}
		}
	}

	/* Select the entries */
	for (i = 0; i < 8; i++)
	{
		/* Every other entry */
		what[i] = what[i * 2];
	}


	/* Message */
	if (cheat_room)
	{
		/* Room type */
		msg_format("Monster pit (%s)", name);

		if (cheat_hear)
		{
			/* Contents */
			for (i = 0; i < 8; i++)
			{
				/* Message */
				msg_print(r_name + r_info[what[i]].name);
			}
		}
	}


	/* Increase the level rating */
	rating += 10;

	/* (Sometimes) Cause a "special feeling" (for "Monster Pits") */
	if ((dun_level <= 40) && (randint(dun_level*dun_level + 50) < 300))
	{
		good_item_flag = TRUE;
	}


	/* Top and bottom rows */
	for (x = xval - 9; x <= xval + 9; x++)
	{
		place_monster_aux(yval - 2, x, what[0], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(yval + 2, x, what[0], FALSE, FALSE, FALSE, FALSE);
	}

	/* Middle columns */
	for (y = yval - 1; y <= yval + 1; y++)
	{
		place_monster_aux(y, xval - 9, what[0], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 9, what[0], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 8, what[1], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 8, what[1], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 7, what[1], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 7, what[1], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 6, what[2], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 6, what[2], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 5, what[2], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 5, what[2], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 4, what[3], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 4, what[3], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 3, what[3], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 3, what[3], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 2, what[4], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 2, what[4], FALSE, FALSE, FALSE, FALSE);
	}

	/* Above/Below the center monster */
	for (x = xval - 1; x <= xval + 1; x++)
	{
		place_monster_aux(yval + 1, x, what[5], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(yval - 1, x, what[5], FALSE, FALSE, FALSE, FALSE);
	}

	/* Next to the center monster */
	place_monster_aux(yval, xval + 1, what[6], FALSE, FALSE, FALSE, FALSE);
	place_monster_aux(yval, xval - 1, what[6], FALSE, FALSE, FALSE, FALSE);

	/* Center monster */
	place_monster_aux(yval, xval, what[7], FALSE, FALSE, FALSE, FALSE);
}



/*
 * Hack -- fill in "vault" rooms
 */
static void build_vault(int yval, int xval, int ymax, int xmax, cptr data)
{
	int dx, dy, x, y;

	cptr t;

	cave_type *c_ptr;


	/* Place dungeon features and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; dx++, t++)
		{
			/* Extract the location */
			x = xval - (xmax / 2) + dx;
			y = yval - (ymax / 2) + dy;

			/* Hack -- skip "non-grids" */
			if (*t == ' ') continue;

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Lay down a floor */
			c_ptr->feat = FEAT_FLOOR;

			/* Part of a vault */
			c_ptr->info |= (CAVE_ROOM | CAVE_ICKY);

			/* Analyze the grid */
			switch (*t)
			{
				/* Granite wall (outer) */
			case '%':
				c_ptr->feat = FEAT_WALL_OUTER;
				break;

				/* Granite wall (inner) */
			case '#':
				c_ptr->feat = FEAT_WALL_INNER;
				break;

				/* Permanent wall (inner) */
			case 'X':
				c_ptr->feat = FEAT_PERM_INNER;
				break;

				/* Treasure/trap */
			case '*':
				if (rand_int(100) < 75)
				{
					place_object(y, x, FALSE, FALSE);
				}
				else
				{
					place_trap(y, x);
				}
				break;

				/* Secret doors */
			case '+':
				place_secret_door(y, x);
				break;

				/* Trap */
			case '^':
				place_trap(y, x);
				break;
			}
		}
	}


	/* Place dungeon monsters and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; dx++, t++)
		{
			/* Extract the grid */
			x = xval - (xmax/2) + dx;
			y = yval - (ymax/2) + dy;

			/* Hack -- skip "non-grids" */
			if (*t == ' ') continue;

			/* Analyze the symbol */
			switch (*t)
			{
				/* Monster */
				case '&':
				{
					monster_level = base_level + 5;
					place_monster(y, x, TRUE, TRUE);
					monster_level = base_level;
					break;
				}

				/* Meaner monster */
				case '@':
				{
					monster_level = base_level + 11;
					place_monster(y, x, TRUE, TRUE);
					monster_level = base_level;
					break;
				}

				/* Meaner monster, plus treasure */
				case '9':
				{
					monster_level = base_level + 9;
					place_monster(y, x, TRUE, TRUE);
					monster_level = base_level;
					object_level = base_level + 7;
					place_object(y, x, TRUE, FALSE);
					object_level = base_level;
					break;
				}

				/* Nasty monster and treasure */
				case '8':
				{
					monster_level = base_level + 40;
					place_monster(y, x, TRUE, TRUE);
					monster_level = base_level;
					object_level = base_level + 20;
					place_object(y, x, TRUE, TRUE);
					object_level = base_level;
					break;
				}

				/* Monster and/or object */
				case ',':
				{
					if (rand_int(100) < 50)
					{
						monster_level = base_level + 3;
						place_monster(y, x, TRUE, TRUE);
						monster_level = base_level;
					}
					if (rand_int(100) < 50)
					{
						object_level = base_level + 7;
						place_object(y, x, FALSE, FALSE);
						object_level = base_level;
					}
					break;
				}

				case 'p':
					cave_set_feat(y, x, FEAT_PATTERN_START);
					break;

				case 'a':
					cave_set_feat(y, x, FEAT_PATTERN_1);
					break;

				case 'b':
					cave_set_feat(y, x, FEAT_PATTERN_2);
					break;

				case 'c':
					cave_set_feat(y, x, FEAT_PATTERN_3);
					break;

				case 'd':
					cave_set_feat(y, x, FEAT_PATTERN_4);
					break;

				case 'P':
					cave_set_feat(y, x, FEAT_PATTERN_END);
					break;

				case 'B':
					cave_set_feat(y, x, FEAT_PATTERN_XTRA1);
					break;

				case 'A':
				{
					object_level = base_level + 12;
					place_object(y, x, TRUE, FALSE);
					object_level = base_level;
				}
				break;
			}
		}
	}
}


/*
 * Type 7 -- simple vaults (see "v_info.txt")
 */
static void build_type7(int yval, int xval)
{
	vault_type	*v_ptr;
	int dummy = 0;

	/* Pick a lesser vault */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* Access a random vault record */
		v_ptr = &v_info[rand_int(max_v_idx)];

		/* Accept the first lesser vault */
		if (v_ptr->typ == 7) break;
	}

	if (dummy >= SAFE_MAX_ATTEMPTS)
	{
		if (cheat_room)
		{
			msg_print("Warning! Could not place lesser vault!");
		}
		return;
	}


#ifdef FORCE_V_IDX
	v_ptr = &v_info[2];
#endif

	/* Message */
	if (cheat_room) msg_print("Lesser Vault");

	/* Boost the rating */
	rating += v_ptr->rat;

	/* (Sometimes) Cause a special feeling */
	if ((dun_level <= 50) ||
		(randint((dun_level-40) * (dun_level-40) + 50) < 400))
	{
		good_item_flag = TRUE;
	}

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr->hgt, v_ptr->wid, v_text + v_ptr->text);
}



/*
 * Type 8 -- greater vaults (see "v_info.txt")
 */
static void build_type8(int yval, int xval)
{
	vault_type	*v_ptr;
	int dummy = 0;

	/* Pick a lesser vault */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* Access a random vault record */
		v_ptr = &v_info[rand_int(max_v_idx)];

		/* Accept the first greater vault */
		if (v_ptr->typ == 8) break;
	}

	if (dummy >= SAFE_MAX_ATTEMPTS)
	{
		if (cheat_room)
		{
			msg_print("Warning! Could not place greater vault!");
		}
		return;
	}


#ifdef FORCE_V_IDX
	v_ptr = &v_info[76 + randint(3)];
#endif

	/* Message */
	if (cheat_room) msg_print("Greater Vault");

	/* Boost the rating */
	rating += v_ptr->rat;

	/* (Sometimes) Cause a special feeling */
	if ((dun_level <= 50) ||
	    (randint((dun_level - 40) * (dun_level - 40) + 50) < 400))
	{
		good_item_flag = TRUE;
	}

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr->hgt, v_ptr->wid, v_text + v_ptr->text);
}


/*
 * DAG:
 * Build an vertical oval room.
 * For every grid in the possible square, check the distance.
 * If it's less than or == than the radius, make it a room square.
 * If its less, make it a normal grid. If it's == make it an outer
 * wall.
 */
static void build_type9(int y0, int x0)
{
	int rad, x, y;

	int light = FALSE;

	/* Occasional light */
	if (randint(dun_level) <= 5) light = TRUE;

	rad = rand_int(10);
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		for (y = y0 - rad; y <= y0 + rad; y++)
		{
			if (distance(y0, x0, y, x) == rad)
			{
				cave[y][x].info |= (CAVE_ROOM);
				if (light) cave[y][x].info |= (CAVE_GLOW);

				cave[y][x].feat = FEAT_WALL_OUTER;
				/* cave_set_feat(y, x, FEAT_WALL_OUTER); */
			}

			if (distance(y0, x0, y, x) < rad)
			{
				cave[y][x].info |= (CAVE_ROOM);
				if (light) cave[y][x].info |= (CAVE_GLOW);

				cave[y][x].feat = FEAT_FLOOR;
				/* cave_set_feat(y, x, FEAT_FLOOR); */
			}
		}
	}
}


/*
 * Type 10 -- rectangular labyrinthine rooms
 *
 * Type 10 uses two routines:
 *    r_visit - a recursive routine that builds the labyrinth
 *    build_type10 - a driver routine that calls r_visit and adds
 *                   monsters, traps and treasure
 *
 * The labyrinth is built by creating a spanning tree of a graph.
 * The graph vertices are at
 *    (x,y) = (2j+x1,2k+y1)   j = 0,...,m-1    k = 0,...,n-1
 * and the edges are the vertical and horizontal nearest neighbors.
 *
 * The spanning tree is created by performing a suitably randomized
 * depth-first traversal of the graph. The only adjustable parameter
 * is the rand_int(3) below; it governs the relative density of
 * twists and turns in the labyrinth: smaller number, more twists.
 */
static void r_visit(int y1, int x1, int y2, int x2,
                    int node, int dir, int *visited)
{
	int i, j, m, n, temp, x, y, adj[4];

	/* dimensions of vertex array */
	m = (x2 - x1) / 2 + 1;
	n = (y2 - y1) / 2 + 1;

	/* mark node visited and set it to a floor */
	visited[node] = 1;
	x = 2 * (node % m) + x1;
	y = 2 * (node / m) + y1;
	cave_set_feat(y, x, FEAT_FLOOR);

	/* setup order of adjacent node visits */
	if (rand_int(3) == 0)
	{
		/* pick a random ordering */
		for (i = 0; i < 4; i++)
			adj[i] = i;

		for (i = 0; i < 4; i++)
		{
			j = rand_int(4);
			temp = adj[i];
			adj[i] = adj[j];
			adj[j] = temp;
		}

		dir = adj[0];
	}
	else
	{
		/* pick a random ordering with dir first */
		adj[0] = dir;

		for (i = 1; i < 4; i++)
			adj[i] = i;

		for (i = 1; i < 4; i++)
		{
			j = 1 + rand_int(3);
			temp = adj[i];
			adj[i] = adj[j];
			adj[j] = temp;
		}
	}

	for (i = 0; i < 4; i++)
	{
		switch (adj[i])
		{
			/* (0,+) - check for bottom boundary */
			case 0:
				if ((node / m < n - 1) && (visited[node + m] == 0))
				{
					cave_set_feat(y + 1, x, FEAT_FLOOR);
					r_visit(y1, x1, y2, x2, node + m, dir, visited);
				}
				break;
			/* (0,-) - check for top boundary */
			case 1:
				if ((node / m > 0) && (visited[node - m] == 0))
				{
					cave_set_feat(y - 1, x, FEAT_FLOOR);
					r_visit(y1, x1, y2, x2, node - m, dir, visited);
				}
				break;
			/* (+,0) - check for right boundary */
			case 2:
				if ((node % m < m - 1) && (visited[node + 1] == 0))
				{
					cave_set_feat(y, x + 1, FEAT_FLOOR);
					r_visit(y1, x1, y2, x2, node + 1, dir, visited);
				}
				break;
			/* (-,0) - check for left boundary */
			case 3:
				if ((node % m > 0) && (visited[node - 1] == 0))
				{
					cave_set_feat(y, x - 1, FEAT_FLOOR);
					r_visit(y1, x1, y2, x2, node - 1, dir, visited);
				}
				break;
		} /* end switch */
	}
}


static void build_type10(int y0, int x0)
{
	int y, x, dy, dx;
	int y1, x1, y2, x2;
	int i, m, n, num_vertices, *visited;
	bool light;
	cave_type *c_ptr;

	/* Choose lite or dark */
	light = (dun_level <= randint(25));

	/* Pick a random room size */
	dy = rand_range(5, 9);
	dx = rand_range(5, 15);
	y1 = y0 - dy;
	x1 = x0 - dx;
	y2 = y0 + dy;
	x2 = x0 + dx;

	/* generate the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->info |= (CAVE_ROOM);
			c_ptr->feat = FEAT_WALL_INNER;
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* generate outer walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1-1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2+1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}

	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1-1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2+1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}

	/* dimensions of vertex array */
	m = dx + 1;
	n = dy + 1;
	num_vertices = m * n;

	/* initialize array of visited vertices */
	/* use ralloc here ? */
	visited = (int *) malloc(num_vertices * sizeof(int));

	for (i = 0; i < num_vertices; i++)
		visited[i] = 0;

	/* traverse the graph to create a spannng tree, pick a random root */
	r_visit(y1, x1, y2, x2, rand_int(num_vertices), 0, visited);

	/* Monsters just love labyrinths. */
	vault_monsters(rand_range(y1, y2), rand_range(x1, x2), randint(3));
	vault_monsters(rand_range(y1, y2), rand_range(x1, x2), randint(3));
	vault_monsters(rand_range(y1, y2), rand_range(x1, x2), randint(3));

	/* Traps make them entertaining. */
	vault_traps(rand_range(y1, y2), rand_range(x1, x2), 3, 3, 1);
	vault_traps(rand_range(y1, y2), rand_range(x1, x2), 3, 3, 1);
	vault_traps(rand_range(y1, y2), rand_range(x1, x2), 3, 3, 1);

	/* Labyrinths should have some treasure too. */
	vault_objects(rand_range(y1, y2), rand_range(x1, x2), 2);
	vault_objects(rand_range(y1, y2), rand_range(x1, x2), 2);
	vault_objects(rand_range(y1, y2), rand_range(x1, x2), 1);

	free(visited);
}


/*
 * Constructs a tunnel between two points
 *
 * This function must be called BEFORE any streamers are created,
 * since we use the special "granite wall" sub-types to keep track
 * of legal places for corridors to pierce rooms.
 *
 * We use "door_flag" to prevent excessive construction of doors
 * along overlapping corridors.
 *
 * We queue the tunnel grids to prevent door creation along a corridor
 * which intersects itself.
 *
 * We queue the wall piercing grids to prevent a corridor from leaving
 * a room and then coming back in through the same entrance.
 *
 * We "pierce" grids which are "outer" walls of rooms, and when we
 * do so, we change all adjacent "outer" walls of rooms into "solid"
 * walls so that no two corridors may use adjacent grids for exits.
 *
 * The "solid" wall check prevents corridors from "chopping" the
 * corners of rooms off, as well as "silly" door placement, and
 * "excessively wide" room entrances.
 *
 * Useful "feat" values:
 *   FEAT_WALL_EXTRA -- granite walls
 *   FEAT_WALL_INNER -- inner room walls
 *   FEAT_WALL_OUTER -- outer room walls
 *   FEAT_WALL_SOLID -- solid room walls
 *   FEAT_PERM_EXTRA -- shop walls (perma)
 *   FEAT_PERM_INNER -- inner room walls (perma)
 *   FEAT_PERM_OUTER -- outer room walls (perma)
 *   FEAT_PERM_SOLID -- dungeon border (perma)
 */
static void build_tunnel(int row1, int col1, int row2, int col2)
{
	int i, y, x;
	int tmp_row, tmp_col;
	int row_dir, col_dir;
	int start_row, start_col;
	int main_loop_count = 0;

	bool door_flag = FALSE;

	cave_type *c_ptr;


	/* Reset the arrays */
	dun->tunn_n = 0;
	dun->wall_n = 0;

	/* Save the starting location */
	start_row = row1;
	start_col = col1;

	/* Start out in the correct direction */
	correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

	/* Keep going until done (or bored) */
	while ((row1 != row2) || (col1 != col2))
	{
		/* Mega-Hack -- Paranoia -- prevent infinite loops */
		if (main_loop_count++ > 2000) break;

		/* Allow bends in the tunnel */
		if (rand_int(100) < dun_tun_chg)
		{
			/* Acquire the correct direction */
			correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

			/* Random direction */
			if (rand_int(100) < dun_tun_rnd)
			{
				rand_dir(&row_dir, &col_dir);
			}
		}

		/* Get the next location */
		tmp_row = row1 + row_dir;
		tmp_col = col1 + col_dir;


		/* Extremely Important -- do not leave the dungeon */
		while (!in_bounds(tmp_row, tmp_col))
		{
			/* Acquire the correct direction */
			correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

			/* Random direction */
			if (rand_int(100) < dun_tun_rnd)
			{
				rand_dir(&row_dir, &col_dir);
			}

			/* Get the next location */
			tmp_row = row1 + row_dir;
			tmp_col = col1 + col_dir;
		}


		/* Access the location */
		c_ptr = &cave[tmp_row][tmp_col];


		/* Avoid the edge of the dungeon */
		if (c_ptr->feat == FEAT_PERM_SOLID) continue;

		/* Avoid the edge of vaults */
		if (c_ptr->feat == FEAT_PERM_OUTER) continue;

		/* Avoid "solid" granite walls */
		if (c_ptr->feat == FEAT_WALL_SOLID) continue;

		/* Pierce "outer" walls of rooms */
		if (c_ptr->feat == FEAT_WALL_OUTER)
		{
			/* Acquire the "next" location */
			y = tmp_row + row_dir;
			x = tmp_col + col_dir;

			/* Hack -- Avoid outer/solid permanent walls */
			if (cave[y][x].feat == FEAT_PERM_SOLID) continue;
			if (cave[y][x].feat == FEAT_PERM_OUTER) continue;

			/* Hack -- Avoid outer/solid granite walls */
			if (cave[y][x].feat == FEAT_WALL_OUTER) continue;
			if (cave[y][x].feat == FEAT_WALL_SOLID) continue;

			/* Accept this location */
			row1 = tmp_row;
			col1 = tmp_col;

			/* Save the wall location */
			if (dun->wall_n < WALL_MAX)
			{
				dun->wall[dun->wall_n].y = row1;
				dun->wall[dun->wall_n].x = col1;
				dun->wall_n++;
			}

			/* Forbid re-entry near this piercing */
			for (y = row1 - 1; y <= row1 + 1; y++)
			{
				for (x = col1 - 1; x <= col1 + 1; x++)
				{
					/* Convert adjacent "outer" walls as "solid" walls */
					if (cave[y][x].feat == FEAT_WALL_OUTER)
					{
						/* Change the wall to a "solid" wall */
						cave[y][x].feat = FEAT_WALL_SOLID;
					}
				}
			}
		}

		/* Travel quickly through rooms */
		else if (c_ptr->info & (CAVE_ROOM))
		{
			/* Accept the location */
			row1 = tmp_row;
			col1 = tmp_col;
		}

		/* Tunnel through all other walls */
		else if (c_ptr->feat >= FEAT_WALL_EXTRA)
		{
			/* Accept this location */
			row1 = tmp_row;
			col1 = tmp_col;

			/* Save the tunnel location */
			if (dun->tunn_n < TUNN_MAX)
			{
				dun->tunn[dun->tunn_n].y = row1;
				dun->tunn[dun->tunn_n].x = col1;
				dun->tunn_n++;
			}

			/* Allow door in next grid */
			door_flag = FALSE;
		}

		/* Handle corridor intersections or overlaps */
		else
		{
			/* Accept the location */
			row1 = tmp_row;
			col1 = tmp_col;

			/* Collect legal door locations */
			if (!door_flag)
			{
				/* Save the door location */
				if (dun->door_n < DOOR_MAX)
				{
					dun->door[dun->door_n].y = row1;
					dun->door[dun->door_n].x = col1;
					dun->door_n++;
				}

				/* No door in next grid */
				door_flag = TRUE;
			}

			/* Hack -- allow pre-emptive tunnel termination */
			if (rand_int(100) >= dun_tun_con)
			{
				/* Distance between row1 and start_row */
				tmp_row = row1 - start_row;
				if (tmp_row < 0) tmp_row = (-tmp_row);

				/* Distance between col1 and start_col */
				tmp_col = col1 - start_col;
				if (tmp_col < 0) tmp_col = (-tmp_col);

				/* Terminate the tunnel */
				if ((tmp_row > 10) || (tmp_col > 10)) break;
			}
		}
	}


	/* Turn the tunnel into corridor */
	for (i = 0; i < dun->tunn_n; i++)
	{
		/* Access the grid */
		y = dun->tunn[i].y;
		x = dun->tunn[i].x;

		/* Access the grid */
		c_ptr = &cave[y][x];

		/* Clear previous contents, add a floor */
		c_ptr->feat = FEAT_FLOOR;
	}


	/* Apply the piercings that we found */
	for (i = 0; i < dun->wall_n; i++)
	{
		/* Access the grid */
		y = dun->wall[i].y;
		x = dun->wall[i].x;

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

		/* Skip non floors */
		if (!cave_floor_bold(y, x)) continue;

		/* Access the grid */
		c_ptr = &cave[y][x];

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
 * Assumes "in_bounds(y,x)"
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
 * Attempt to build a room of the given type at the given block
 *
 * Note that we restrict the number of "crowded" rooms to reduce
 * the chance of overflowing the monster list during level creation.
 */
static bool room_build(int y0, int x0, int typ)
{
	int y, x, y1, x1, y2, x2;


	/* Restrict level */
	if ((dun_level < room[typ].level) && !ironman_rooms) return (FALSE);

	/* Restrict "crowded" rooms */
	if (dun->crowded && ((typ == 5) || (typ == 6))) return (FALSE);

	/* Extract blocks */
	y1 = y0 + room[typ].dy1;
	y2 = y0 + room[typ].dy2;
	x1 = x0 + room[typ].dx1;
	x2 = x0 + room[typ].dx2;

	/* Never run off the screen */
	if ((y1 < 0) || (y2 >= dun->row_rooms)) return (FALSE);
	if ((x1 < 0) || (x2 >= dun->col_rooms)) return (FALSE);

	/* Verify open space */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			if (dun->room_map[y][x]) return (FALSE);
		}
	}

	/* XXX XXX XXX It is *extremely* important that the following */
	/* calculation is *exactly* correct to prevent memory errors */

	/* Acquire the location of the room */
	y = ((y1 + y2 + 1) * BLOCK_HGT) / 2;
	x = ((x1 + x2 + 1) * BLOCK_WID) / 2;

	/* Build a room */
	switch (typ)
	{
		/* Build an appropriate room */
		case 10: build_type10(y, x); break;
		case  9: build_type9 (y, x); break;
		case  8: build_type8 (y, x); break;
		case  7: build_type7 (y, x); break;
		case  6: build_type6 (y, x); break;
		case  5: build_type5 (y, x); break;
		case  4: build_type4 (y, x); break;
		case  3: build_type3 (y, x); break;
		case  2: build_type2 (y, x); break;
		case  1: build_type1 (y, x); break;

		/* Paranoia */
		default: return (FALSE);
	}

	/* Save the room location */
	if (dun->cent_n < CENT_MAX)
	{
		dun->cent[dun->cent_n].y = y;
		dun->cent[dun->cent_n].x = x;
		dun->cent_n++;
	}

	/* Reserve some blocks */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			dun->room_map[y][x] = TRUE;
		}
	}

	/* Count "crowded" rooms */
	if ((typ == 5) || (typ == 6)) dun->crowded = TRUE;

	/* Success */
	return (TRUE);
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

	bool destroyed = FALSE;
	bool empty_level = FALSE;

	dun_data dun_body;

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), NULL);

	/* Global data */
	dun = &dun_body;

	if (!max_panel_rows) max_vault_ok--;
	if (!max_panel_cols) max_vault_ok--;

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


	/* Possible "destroyed" level */
	if ((dun_level > 10) && (rand_int(DUN_DEST) == 0) && (small_levels))
		destroyed = TRUE;

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
	dun->crowded = FALSE;


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
			k = (ironman_rooms ? 0 : rand_int(100));

			/* Attempt a very unusual room */
			if (ironman_rooms || (rand_int(DUN_UNUSUAL) < dun_level))
			{
#ifdef FORCE_V_IDX
				if (room_build(y, x, 8)) continue;
#else
				/* Type 8 -- Greater vault (10%) */
				if (k < 10)
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

				/* Type 7 -- Lesser vault (15%) */
				if (k < 25)
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


				/* Type 5 -- Monster nest (15%) */
				if ((k < 40) && room_build(y, x, 5)) continue;

				/* Type 6 -- Monster pit (15%) */
				if ((k < 55) && room_build(y, x, 6)) continue;
#endif

			}

			/* Type 4 -- Large room (20%) */
			if ((k < 20) && room_build(y, x, 4)) continue;

			/* Type 3 -- Cross room (20%) */
			if ((k < 40) && room_build(y, x, 3)) continue;

			/* Type 2 -- Overlapping (20%) */
			if ((k < 60) && room_build(y, x, 2)) continue;

			/* Type 10 -- Labyrinthine (20%)*/
			if ((k < 80) && room_build(y, x, 10)) continue;

			/* Type 9 -- Circular (20%) */
			if ((k < 100) && room_build(y, x, 9)) continue;
		}

		/* Attempt a trivial room */
		if (room_build(y, x, 1)) continue;
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
		/* Connect the room to the previous room */
		build_tunnel(dun->cent[i].y, dun->cent[i].x, y, x);

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

	/* Add other terrain types */
	if (terrain_streams)
	{
		int feat1, feat2;

		/* Add streamers of trees, water, or lava */
		if ((dun_level == 1) && (randint(20) > 10))
			for (i = 0; i < randint(DUN_STR_QUA); i++)
				build_streamer2(FEAT_TREES, FALSE, TRUE);

		/* Choose water or lava */
		if (rand_int(MAX_DEPTH) > dun_level)
		{
			feat1 = FEAT_DEEP_WATER;
			feat2 = FEAT_SHAL_WATER;
		}
		else
		{
			feat1 = FEAT_DEEP_LAVA;
			feat2 = FEAT_SHAL_LAVA;
		}

		/* Hack -- Add some rivers */
		while(!rand_int(3)) add_river(feat1, feat2, randnor(40, 15));
	}

	/* Destroy the level if necessary */
	if (destroyed) destroy_level();

	/* Place 3 or 4 down stairs near some walls */
	alloc_stairs(FEAT_MORE, rand_range(3, 4), 3);

	/* Place 1 or 2 up stairs near some walls */
	alloc_stairs(FEAT_LESS, rand_range(1, 2), 3);


	/* Determine the character location */
	if (!new_player_spot())
		return FALSE;

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
							if (!cave_naked_bold(y, x)) continue;
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

	return TRUE;
}


/*
 * Builds the arena after it is entered -KMW-
 */
static void build_arena(void)
{
	int yval, y_height, y_depth, xval, x_left, x_right;
	register int i, j;

	yval = SCREEN_HGT / 2;
	xval = SCREEN_WID / 2;
	y_height = yval - 10 + SCREEN_HGT;
	y_depth = yval + 10 + SCREEN_HGT;
	x_left = xval - 32 + SCREEN_WID;
	x_right = xval + 32 + SCREEN_WID;

	for (i = y_height; i <= y_height + 5; i++)
		for (j = x_left; j <= x_right; j++)
		{
			cave[i][j].feat = FEAT_PERM_EXTRA;
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (i = y_depth; i >= y_depth - 5; i--)
		for (j = x_left; j <= x_right; j++)
		{
			cave[i][j].feat = FEAT_PERM_EXTRA;
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_left; j <= x_left + 17; j++)
		for (i = y_height; i <= y_depth; i++)
		{
			cave[i][j].feat = FEAT_PERM_EXTRA;
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_right; j >= x_right - 17; j--)
		for (i = y_height; i <= y_depth; i++)
		{
			cave[i][j].feat = FEAT_PERM_EXTRA;
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}

	cave[y_height+6][x_left+18].feat = FEAT_PERM_EXTRA;
	cave[y_height+6][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	cave[y_depth-6][x_left+18].feat = FEAT_PERM_EXTRA;
	cave[y_depth-6][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	cave[y_height+6][x_right-18].feat = FEAT_PERM_EXTRA;
	cave[y_height+6][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);
	cave[y_depth-6][x_right-18].feat = FEAT_PERM_EXTRA;
	cave[y_depth-6][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);

	i = y_height + 5;
	j = xval + SCREEN_WID;
	cave[i][j].feat = FEAT_BLDG_HEAD + 2;
	cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
	player_place(i + 1, j);
}


/*
 * Town logic flow for generation of arena -KMW-
 */
static void arena_gen(void)
{
	int y, x;
	int qy = SCREEN_HGT;
	int qx = SCREEN_WID;
	bool daytime;

	/* Day time */
	if ((turn % (10L * TOWN_DAWN)) < ((10L * TOWN_DAWN) / 2))
		daytime = TRUE;

	/* Night time */
	else
		daytime = FALSE;

	/* Start with solid walls */
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			/* Create "solid" perma-wall */
			cave[y][x].feat = FEAT_PERM_SOLID;

			/* Illuminate and memorize the walls */
			cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
		}
	}

	/* Then place some floors */
	for (y = qy + 1; y < qy + SCREEN_HGT - 1; y++)
	{
		for (x = qx + 1; x < qx + SCREEN_WID - 1; x++)
		{
			/* Create empty floor */
			cave[y][x].feat = FEAT_FLOOR;

			/* Darken and forget the floors */
			cave[y][x].info &= ~(CAVE_GLOW | CAVE_MARK);

			/* Day time */
			if (daytime)
			{
				/* Perma-Lite */
				cave[y][x].info |= (CAVE_GLOW);

				/* Memorize */
				if (view_perma_grids) cave[y][x].info |= (CAVE_MARK);
			}
		}
	}

	build_arena();

	place_monster_aux(py + 5, px, arena_monsters[p_ptr->arena_number],
	    FALSE, FALSE, FALSE, FALSE);
}


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


/*
 * Generates a random dungeon level			-RAK-
 *
 * Hack -- regenerate any "overflow" levels
 *
 * Hack -- allow auto-scumming via a gameplay option.
 */
void generate_cave(void)
{
	int tester_1, tester_2;
	int y, x, num;


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


		/* Mega-Hack -- no player yet */
		px = py = 0;


		/* Mega-Hack -- no panel yet */
		panel_row_min = 0;
		panel_row_max = 0;
		panel_col_min = 0;
		panel_col_max = 0;


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
			/* Build the arena -KMW- */
			if (p_ptr->inside_arena)
			{
				/* Small arena */
				arena_gen();
			}

			/* Quest levels -KMW- */
			else if (p_ptr->inside_quest)
			{
				quest_gen();
			}

			/* Build the town */
			else if (!dun_level)
			{
				/* Big town */
				cur_hgt = MAX_HGT;
				cur_wid = MAX_WID;

				/* Determine number of panels */
				max_panel_rows = (cur_hgt / SCREEN_HGT) * 2 - 2;
				max_panel_cols = (cur_wid / SCREEN_WID) * 2 - 2;

				/* Assume illegal panel */
				panel_row = max_panel_rows;
				panel_col = max_panel_cols;

				/* Make the wilderness */
				wilderness_gen();
			}

			/* Build a real level */
			else
			{
				if (always_small_levels || ironman_small_levels ||
					((randint(SMALL_LEVEL) == 1) && small_levels))
				{
					if (cheat_room)
						msg_print("A 'small' dungeon level.");

					do
					{
						tester_1 = randint(MAX_HGT/SCREEN_HGT);
						tester_2 = randint(MAX_WID/SCREEN_WID);
					}
					while ((tester_1 == MAX_HGT/SCREEN_HGT) &&
						   (tester_2 == MAX_WID/SCREEN_WID));

					cur_hgt = tester_1 * SCREEN_HGT;
					cur_wid = tester_2 * SCREEN_WID;

					/* Determine number of panels */
					max_panel_rows = (cur_hgt / SCREEN_HGT) * 2 - 2;
					max_panel_cols = (cur_wid / SCREEN_WID) * 2 - 2;

					/* Assume illegal panel */
					panel_row = max_panel_rows;
					panel_col = max_panel_cols;

					if (cheat_room)
						msg_format("X:%d, Y:%d.", max_panel_cols, max_panel_rows);
				}
				else
				{
					/* Big dungeon */
					cur_hgt = MAX_HGT;
					cur_wid = MAX_WID;

					/* Determine number of panels */
					max_panel_rows = (cur_hgt / SCREEN_HGT) * 2 - 2;
					max_panel_cols = (cur_wid / SCREEN_WID) * 2 - 2;

					/* Assume illegal panel */
					panel_row = max_panel_rows;
					panel_col = max_panel_cols;
				}

				/* Make a dungeon */
				if (!cave_gen())
				{
					why = "could not place player";
					okay = FALSE;
				}
			}
		}

		/* Extract the feeling */
		if (rating > 100) feeling = 2;
		else if (rating > 80) feeling = 3;
		else if (rating > 60) feeling = 4;
		else if (rating > 40) feeling = 5;
		else if (rating > 30) feeling = 6;
		else if (rating > 20) feeling = 7;
		else if (rating > 10) feeling = 8;
		else if (rating > 0) feeling = 9;
		else feeling = 10;

		/* Hack -- Have a special feeling sometimes */
		if (good_item_flag && !preserve_mode) feeling = 1;

		/* It takes 1000 game turns for "feelings" to recharge */
		if ((turn - old_turn) < 1000) feeling = 0;

		/* Hack -- no feeling in the town */
		if (!dun_level) feeling = 0;


		/* Prevent object over-flow */
		if (o_max >= max_o_idx)
		{
			/* Message */
			why = "too many objects";

			/* Message */
			okay = FALSE;
		}

		/* Prevent monster over-flow */
		if (m_max >= max_m_idx)
		{
			/* Message */
			why = "too many monsters";

			/* Message */
			okay = FALSE;
		}

		/* Mega-Hack -- "auto-scum" */
		if ((auto_scum || ironman_autoscum) && (num < 100) && !p_ptr->inside_quest)
		{
			/* Require "goodness" */
			if ((feeling > 9) ||
			    ((dun_level >= 5) && (feeling > 8)) ||
			    ((dun_level >= 10) && (feeling > 7)) ||
			    ((dun_level >= 20) && (feeling > 6)) ||
			    ((dun_level >= 40) && (feeling > 5)))
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
	}


	/* The dungeon is ready */
	character_dungeon = TRUE;

	/* Remember when this level was "created" */
	old_turn = turn;
}
