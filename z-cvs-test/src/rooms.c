/* CVS: Last edit by $Author$ on $Date$
 *
 * File: rooms.c
 * Purpose: make rooms. Used by generate.c when creating dungeons.
 */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "generate.h"
#include "grid.h"
#include "rooms.h"


#include "tnb.h" /* TNB */


/*
 * Array of minimum room depths
 */
static s16b roomdep[] = {
	0, /* 0  = Nothing */
	1, /* 1  = Simple (33x11) */
	1, /* 2  = Overlapping (33x11) */
	3, /* 3  = Crossed (33x11) */
	3, /* 4  = Large (33x11) */
	5, /* 5  = Monster nest (33x11) */
	5, /* 6  = Monster pit (33x11) */
	5, /* 7  = Lesser vault (33x22) */
	10,	/* 8  = Greater vault (66x44) */
	3, /* 9  = Fractal cave (42x24) */
	5, /* 10 = Random vault (44x22) */
	1, /* 11 = Circular rooms (22x22) */
	3, /* 12 = Crypts (22x22) */
};


/*
 * This funtion makes a very small room centred at (x0, y0)
 * This is used in crypts, and random elemental vaults.
 *
 * Note - this should be used only on allocated regions
 * within another room.
 */
static void build_small_room(int x0, int y0)
{
	int x, y;

	for (y = y0 - 1; y <= y0 + 1; y++)
	{
		cave[y][x0 - 1].feat = FEAT_WALL_INNER;
		cave[y][x0 + 1].feat = FEAT_WALL_INNER;
	}

	for (x = x0 - 1; x <= x0 + 1; x++)
	{
		cave[y0 - 1][x].feat = FEAT_WALL_INNER;
		cave[y0 + 1][x].feat = FEAT_WALL_INNER;
	}

	/* Place a secret door on one side */
	switch (rand_int(4))
	{
		case 0:
			place_secret_door(y0, x0 - 1);
			break;
		case 1:
			place_secret_door(y0, x0 + 1);
			break;
		case 2:
			place_secret_door(y0 - 1, x0);
			break;
		case 3:
			place_secret_door(y0 + 1, x0);
			break;
	}

	/* Add inner open space */
	cave[y0][x0].feat = FEAT_FLOOR;
}


/*
 * This function tunnels around a room if
 * it will cut off part of a cave system.
 */
static void check_room_boundary(int x1, int y1, int x2, int y2)
{
	int count, x, y;
	bool old_is_floor, new_is_floor;


	/* Initialize */
	count = 0;

	old_is_floor = get_is_floor(x1 - 1, y1);

	/*
	 * Count the number of floor-wall boundaries around the room
	 * Note: diagonal squares are ignored since the player can move diagonally
	 * to bypass these if needed.
	 */

	/* Above the top boundary */
	for (x = x1; x <= x2; x++)
	{
		new_is_floor = get_is_floor(x, y1 - 1);

		/* Increment counter if they are different */
		if (new_is_floor != old_is_floor)
			count++;

		old_is_floor = new_is_floor;
	}

	/* Right boundary */
	for (y = y1; y <= y2; y++)
	{
		new_is_floor = get_is_floor(x2 + 1, y);

		/* increment counter if they are different */
		if (new_is_floor != old_is_floor)
			count++;

		old_is_floor = new_is_floor;
	}

	/* Bottom boundary */
	for (x = x2; x >= x1; x--)
	{
		new_is_floor = get_is_floor(x, y2 + 1);

		/* increment counter if they are different */
		if (new_is_floor != old_is_floor)
			count++;

		old_is_floor = new_is_floor;
	}

	/* Left boundary */
	for (y = y2; y >= y1; y--)
	{
		new_is_floor = get_is_floor(x1 - 1, y);

		/* increment counter if they are different */
		if (new_is_floor != old_is_floor)
			count++;

		old_is_floor = new_is_floor;
	}

	/* If all the same, or only one connection exit. */
	if (count <= 2)
		return;


	/* Tunnel around the room so to prevent problems with caves */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			set_floor(x, y);
		}
	}
}


/*
 * This function is used to allocate the space needed by a room in the room_map
 * array.
 * x, y represent the size of the room (0...x-1) by (0...y-1).
 * crowded is used to denote a monset nest.
 * by0, bx0 are the positions in the room_map array given to the build_type'x'
 * function.
 * xx, yy are the returned center of the allocated room in coordinates for
 * cave.feat and cave.info etc.
 */
static bool room_alloc(int x, int y, bool crowded, int by0, int bx0,
	int *xx, int *yy)
{
	int temp, bx1, bx2, by1, by2, by, bx;

	/* Calculate number of room_map squares to allocate */

	/* temp is total number along width */
	temp = ((x - 1) / BLOCK_WID) + 1;

	/* bx2 = ending block */
	bx2 = temp / 2 + bx0;

	/* bx1 = starting block (Note: rounding taken care of here.) */
	bx1 = bx2 + 1 - temp;

	/* temp is total number along height */
	temp = ((y - 1) / BLOCK_HGT) + 1;

	/* by2 = ending block */
	by2 = temp / 2 + by0;

	/* by1 = starting block */
	by1 = by2 + 1 - temp;


	/* Never run off the screen */
	if ((by1 < 0) || (by2 >= dun->row_rooms))
		return (FALSE);
	if ((bx1 < 0) || (bx2 >= dun->col_rooms))
		return (FALSE);

	/* Verify open space */
	for (by = by1; by <= by2; by++)
	{
		for (bx = bx1; bx <= bx2; bx++)
		{
			if (dun->room_map[by][bx])
				return (FALSE);
		}
	}

	/* It is *extremely* important that the following calculation */
	/* be *exactly* correct to prevent memory errors XXX XXX XXX */

	/* Acquire the location of the room */
	*yy = ((by1 + by2 + 1) * BLOCK_HGT) / 2;
	*xx = ((bx1 + bx2 + 1) * BLOCK_WID) / 2;


	/* Save the room location */
	if (dun->cent_n < CENT_MAX)
	{
		dun->cent[dun->cent_n].y = *yy;
		dun->cent[dun->cent_n].x = *xx;
		dun->cent_n++;
	}

	/* Reserve some blocks */
	for (by = by1; by <= by2; by++)
	{
		for (bx = bx1; bx <= bx2; bx++)
		{
			dun->room_map[by][bx] = TRUE;
		}
	}

	/* Count "crowded" rooms */
	if (crowded)
		dun->crowded++;

	/*
	 * Hack- See if room will cut off a cavern.
	 * If so, fix by tunneling outside the room in such a way as to connect the caves.
	 */
	check_room_boundary(*xx - x / 2 - 1, *yy - y / 2 - 1,
		*xx + (x - 1) / 2 + 1, *yy + (y - 1) / 2 + 1);

	/* Success */
	return (TRUE);
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
 *   9 -- fractal caves
 *  10 -- random vaults
 *  11 -- circular rooms
 *  12 -- crypts
 */


/*
 * Type 1 -- normal rectangular rooms
 */
static void build_type1(int by0, int bx0)
{
	int y, x, y2, x2, yval, xval;
	int y1, x1, xsize, ysize;

	bool light;

	cave_type *c_ptr;

	/* Pick a room size */
	y1 = randint(4);
	x1 = randint(11);
	y2 = randint(3);
	x2 = randint(11);

	xsize = x1 + x2 + 1;
	ysize = y1 + y2 + 1;

	/* Try to allocate space for room.  If fails, exit */
	if (!room_alloc(xsize + 2, ysize + 2, FALSE, by0, bx0, &xval, &yval))
		return;

	/* Choose lite or dark */
	light = (dun_level <= randint(25));


	/* Get corner values */
	y1 = yval - ysize / 2;
	x1 = xval - xsize / 2;
	y2 = yval + (ysize - 1) / 2;
	x2 = xval + (xsize - 1) / 2;


	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->feat = FEAT_FLOOR;
			c_ptr->info |= (CAVE_ROOM);
			if (light)
				c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Walls around the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2 + 1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2 + 1][x];
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
	/* Hack -- Occasional divided room */
	else if (rand_int(50) == 0)
	{
		if (randint(100) < 50)
		{
			/* Horizontal wall */
			for (x = x1; x <= x2; x++)
			{
				cave[yval][x].feat = FEAT_WALL_INNER;
			}

			/* Prevent edge of wall from being tunneled */
			cave[yval][x1 - 1].feat = FEAT_WALL_SOLID;
			cave[yval][x2 + 1].feat = FEAT_WALL_SOLID;
		}
		else
		{
			/* Vertical wall */
			for (y = y1; y <= y2; y++)
			{
				cave[y][xval].feat = FEAT_WALL_INNER;
			}

			/* Prevent edge of wall from being tunneled */
			cave[y1 - 1][xval].feat = FEAT_WALL_SOLID;
			cave[y2 + 1][xval].feat = FEAT_WALL_SOLID;
		}

		place_random_door(yval, xval);
	}
}


/*
 * Type 2 -- Overlapping rectangular rooms
 */
static void build_type2(int by0, int bx0)
{
	int y, x, xval, yval;
	int y1a, x1a, y2a, x2a;
	int y1b, x1b, y2b, x2b;
	bool light;
	cave_type *c_ptr;


	/* Try to allocate space for room. If fails, exit */
	if (!room_alloc(25, 11, FALSE, by0, bx0, &xval, &yval))
		return;

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
			if (light)
				c_ptr->info |= (CAVE_GLOW);
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
			if (light)
				c_ptr->info |= (CAVE_GLOW);
		}
	}


	/* Place the walls around room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		c_ptr = &cave[y][x1a - 1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2a + 1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1a - 1; x <= x2a + 1; x++)
	{
		c_ptr = &cave[y1a - 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2a + 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}

	/* Place the walls around room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		c_ptr = &cave[y][x1b - 1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2b + 1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1b - 1; x <= x2b + 1; x++)
	{
		c_ptr = &cave[y1b - 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2b + 1][x];
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
 * So the "central pillar" runs from x1a, y1b to x2a, y2b.
 *
 * Note that currently, the "center" is always 3x3, but I think that
 * the code below will work (with "bounds checking") for 5x5, or even
 * for unsymetric values like 4x3 or 5x3 or 3x4 or 3x5, or even larger.
 */
static void build_type3(int by0, int bx0)
{
	int y, x, dy, dx, wy, wx;
	int y1a, x1a, y2a, x2a;
	int y1b, x1b, y2b, x2b;
	int yval, xval;
	bool light;
	cave_type *c_ptr;


	/* Try to allocate space for room. */
	if (!room_alloc(25, 11, FALSE, by0, bx0, &xval, &yval))
		return;

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
			if (light)
				c_ptr->info |= (CAVE_GLOW);
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
			if (light)
				c_ptr->info |= (CAVE_GLOW);
		}
	}


	/* Place the walls around room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		c_ptr = &cave[y][x1a - 1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2a + 1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1a - 1; x <= x2a + 1; x++)
	{
		c_ptr = &cave[y1a - 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2a + 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
	}

	/* Place the walls around room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		c_ptr = &cave[y][x1b - 1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2b + 1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1b - 1; x <= x2b + 1; x++)
	{
		c_ptr = &cave[y1b - 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2b + 1][x];
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
				case 0:
					place_secret_door(y1b, xval);
					break;
				case 1:
					place_secret_door(y2b, xval);
					break;
				case 2:
					place_secret_door(yval, x1a);
					break;
				case 3:
					place_secret_door(yval, x2a);
					break;
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
					if (y == yval)
						continue;
					c_ptr = &cave[y][x1a - 1];
					c_ptr->feat = FEAT_WALL_INNER;
					c_ptr = &cave[y][x2a + 1];
					c_ptr->feat = FEAT_WALL_INNER;
				}

				/* Pinch the north/south sides */
				for (x = x1a; x <= x2a; x++)
				{
					if (x == xval)
						continue;
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
static void build_type4(int by0, int bx0)
{
	int y, x, y1, x1;
	int y2, x2, tmp, yval, xval;
	bool light;
	cave_type *c_ptr;


	/* Try to allocate space for room. */
	if (!room_alloc(25, 11, FALSE, by0, bx0, &xval, &yval))
		return;

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
			if (light)
				c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Outer Walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2 + 1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2 + 1][x];
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
		c_ptr = &cave[y][x1 - 1];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y][x2 + 1];
		c_ptr->feat = FEAT_WALL_INNER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y2 + 1][x];
		c_ptr->feat = FEAT_WALL_INNER;
	}


	/* Inner room variations */
	switch (randint(5))
	{
			/* Just an inner room with a monster */
		case 1:
		{
			/* Place a secret door */
			switch (randint(4))
			{
				case 1:
					place_secret_door(y1 - 1, xval);
					break;
				case 2:
					place_secret_door(y2 + 1, xval);
					break;
				case 3:
					place_secret_door(yval, x1 - 1);
					break;
				case 4:
					place_secret_door(yval, x2 + 1);
					break;
			}

			/* Place a monster in the room */
			vault_monsters(yval, xval, 1);

			break;
		}

			/* Treasure Vault (with a door) */
		case 2:
		{
			/* Place a secret door */
			switch (randint(4))
			{
				case 1:
					place_secret_door(y1 - 1, xval);
					break;
				case 2:
					place_secret_door(y2 + 1, xval);
					break;
				case 3:
					place_secret_door(yval, x1 - 1);
					break;
				case 4:
					place_secret_door(yval, x2 + 1);
					break;
			}

			/* Place another inner room */
			for (y = yval - 1; y <= yval + 1; y++)
			{
				for (x = xval - 1; x <= xval + 1; x++)
				{
					if ((x == xval) && (y == yval))
						continue;
					c_ptr = &cave[y][x];
					c_ptr->feat = FEAT_WALL_INNER;
				}
			}

			/* Place a locked door on the inner room */
			switch (randint(4))
			{
				case 1:
					place_locked_door(yval - 1, xval);
					break;
				case 2:
					place_locked_door(yval + 1, xval);
					break;
				case 3:
					place_locked_door(yval, xval - 1);
					break;
				case 4:
					place_locked_door(yval, xval + 1);
					break;
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
		}

			/* Inner pillar(s). */
		case 3:
		{
			/* Place a secret door */
			switch (randint(4))
			{
				case 1:
					place_secret_door(y1 - 1, xval);
					break;
				case 2:
					place_secret_door(y2 + 1, xval);
					break;
				case 3:
					place_secret_door(yval, x1 - 1);
					break;
				case 4:
					place_secret_door(yval, x2 + 1);
					break;
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
					c_ptr = &cave[yval - 1][x];
					c_ptr->feat = FEAT_WALL_INNER;
					c_ptr = &cave[yval + 1][x];
					c_ptr->feat = FEAT_WALL_INNER;
				}

				/* Close off the left/right edges */
				c_ptr = &cave[yval][xval - 5];
				c_ptr->feat = FEAT_WALL_INNER;
				c_ptr = &cave[yval][xval + 5];
				c_ptr->feat = FEAT_WALL_INNER;

				/* Secret doors (random top/bottom) */
				place_secret_door(yval - 3 + (randint(2) * 2), xval - 3);
				place_secret_door(yval - 3 + (randint(2) * 2), xval + 3);

				/* Monsters */
				vault_monsters(yval, xval - 2, randint(2));
				vault_monsters(yval, xval + 2, randint(2));

				/* Objects */
				if (rand_int(3) == 0)
					place_object(yval, xval - 2, FALSE, FALSE);
				if (rand_int(3) == 0)
					place_object(yval, xval + 2, FALSE, FALSE);
			}

			break;
		}

			/* Maze inside. */
		case 4:
		{
			/* Place a secret door */
			switch (randint(4))
			{
				case 1:
					place_secret_door(y1 - 1, xval);
					break;
				case 2:
					place_secret_door(y2 + 1, xval);
					break;
				case 3:
					place_secret_door(yval, x1 - 1);
					break;
				case 4:
					place_secret_door(yval, x2 + 1);
					break;
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
		}

			/* Four small rooms. */
		case 5:
		{
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
}


/*
 * The following functions are used to determine if the given monster
 * is appropriate for inclusion in a monster nest or monster pit or
 * the given type.
 *
 * None of the pits/nests are allowed to include "unique" monsters.
 */


/*
 * Monster validation macro
 *
 * Line 1 -- forbid town monsters
 * Line 2 -- forbid uniques
 * Line 3 -- forbid aquatic monsters
 */
#define vault_monster_okay(I) \
	(monster_dungeon(I) && \
	 !(r_info[I].flags1 & RF1_UNIQUE) && \
	 !(r_info[I].flags7 & RF7_AQUATIC))


/* Race index for "monster pit (clone)" */
static int vault_aux_race;

/* Race index for "monster pit (symbol clone)" */
static char vault_aux_char;

/* Breath mask for "monster pit (dragon)" */
static u32b vault_aux_dragon_mask4;


/*
 * Helper monster selection function
 */
static bool vault_aux_simple(int r_idx)
{
	/* Okay */
	return (vault_monster_okay(r_idx));
}


/*
 * Helper function for "monster nest (jelly)"
 */
static bool vault_aux_jelly(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Also decline evil jellies (like death molds and shoggoths) */
	if (r_ptr->flags3 & (RF3_EVIL))
		return (FALSE);

	/* Require icky thing, jelly, mold, or mushroom */
	if (!strchr("ijm,", r_ptr->d_char))
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (animal)"
 */
static bool vault_aux_animal(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE))
		return (FALSE);

	/* Require "animal" flag */
	if (!(r_ptr->flags3 & (RF3_ANIMAL)))
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (undead)"
 */
static bool vault_aux_undead(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require Undead */
	if (!(r_ptr->flags3 & (RF3_UNDEAD)))
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (chapel)"
 */
static bool vault_aux_chapel(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require "priest" or Angel */
	if ((r_ptr->d_char != 'A') && !strstr((r_name + r_ptr->name), "riest"))
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

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require a Zephyr Hound or a dog */
	if (!strchr("CZ", r_ptr->d_char))
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (mimic)"
 */
static bool vault_aux_mimic(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require mimic */
	if (!strchr("!|$?=", r_ptr->d_char))
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (clone)"
 */
static bool vault_aux_clone(int r_idx)
{
	return (r_idx == vault_aux_race);
}


/*
 * Helper function for "monster nest (symbol clone)"
 */
static bool vault_aux_symbol(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Decline incorrect symbol */
	if (r_ptr->d_char != vault_aux_char)
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (orc)"
 */
static bool vault_aux_orc(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require orc */
	if (!(r_ptr->flags3 & RF3_ORC))
		return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD)
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (troll)"
 */
static bool vault_aux_troll(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require troll */
	if (!(r_ptr->flags3 & RF3_TROLL))
		return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD)
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (giant)"
 */
static bool vault_aux_giant(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require giant */
	if (!(r_ptr->flags3 & RF3_GIANT))
		return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD)
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (dragon)"
 */
static bool vault_aux_dragon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require dragon */
	if (!(r_ptr->flags3 & RF3_DRAGON))
		return (FALSE);

	/* Hack -- Require correct "breath attack" */
	if (r_ptr->flags4 != vault_aux_dragon_mask4)
		return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD)
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (demon)"
 */
static bool vault_aux_demon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require demon */
	if (!(r_ptr->flags3 & RF3_DEMON))
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (lovecraftian)"
 */
static bool vault_aux_cthulhu(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx))
		return (FALSE);

	/* Require eldritch horror */
	if (!(r_ptr->flags2 & RF2_ELDRITCH_HORROR))
		return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (clone)"
 */
static void vault_prep_clone(void)
{
	/* Apply the monster restriction */
	get_mon_num_prep(vault_aux_simple, NULL);

	/* Pick a race to clone */
	vault_aux_race = get_mon_num(dun_level + 10);

	/* Remove the monster restriction */
	get_mon_num_prep(NULL, NULL);
}


/*
 * Helper function for "monster pit (symbol clone)"
 */
static void vault_prep_symbol(void)
{
	int r_idx;

	/* Apply the monster restriction */
	get_mon_num_prep(vault_aux_simple, NULL);

	/* Pick a race to clone */
	r_idx = get_mon_num(dun_level + 10);

	/* Remove the monster restriction */
	get_mon_num_prep(NULL, NULL);

	/* Extract the symbol */
	vault_aux_char = r_info[r_idx].d_char;
}


/*
 * Helper function for "monster pit (dragon)"
 */
static void vault_prep_dragon(void)
{
	/* Pick dragon type */
	switch (rand_int(6))
	{
			/* Black */
		case 0:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_ACID;

			/* Done */
			break;
		}

			/* Blue */
		case 1:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_ELEC;

			/* Done */
			break;
		}

			/* Red */
		case 2:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_FIRE;

			/* Done */
			break;
		}

			/* White */
		case 3:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_COLD;

			/* Done */
			break;
		}

			/* Green */
		case 4:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_POIS;

			/* Done */
			break;
		}

			/* Multi-hued */
		default:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 =
				(RF4_BR_ACID | RF4_BR_ELEC | RF4_BR_FIRE | RF4_BR_COLD |
				RF4_BR_POIS);

			/* Done */
			break;
		}
	}
}


typedef struct vault_aux_type vault_aux_type;


struct vault_aux_type
{
	cptr name;
	 bool(*hook_func) (int r_idx);
	void (*prep_func) (void);
	int level;
	int chance;
};


static vault_aux_type *pick_vault_type(vault_aux_type * l_ptr)
{
	int tmp, total;

	vault_aux_type *n_ptr;

	/* Calculate the total possibilities */
	for (n_ptr = l_ptr, total = 0; TRUE; n_ptr++)
	{
		/* Note end */
		if (!n_ptr->name)
			break;

		/* Ignore excessive depth */
		if (n_ptr->level > dun_level)
			continue;

		/* Count this possibility */
		total +=
			n_ptr->chance * MAX_DEPTH / (dun_level - n_ptr->level + 5);
	}

	/* Pick a random type */
	tmp = rand_int(total);

	/* Find this type */
	for (n_ptr = l_ptr, total = 0; TRUE; n_ptr++)
	{
		/* Note end */
		if (!n_ptr->name)
			break;

		/* Ignore excessive depth */
		if (n_ptr->level > dun_level)
			continue;

		/* Count this possibility */
		total +=
			n_ptr->chance * MAX_DEPTH / (dun_level - n_ptr->level + 5);

		/* Found the type */
		if (tmp < total)
			break;
	}

	return (n_ptr->name ? n_ptr : NULL);
}


static vault_aux_type nest_types[] = {
	{"clone", vault_aux_clone, vault_prep_clone, 5, 3},
	{"jelly", vault_aux_jelly, NULL, 5, 6},
	{"symbol clone", vault_aux_symbol, vault_prep_symbol, 25, 3},
	{"mimic", vault_aux_mimic, NULL, 25, 6},
	{"lovecraftian", vault_aux_cthulhu, NULL, 40, 2},
	{"kennel", vault_aux_kennel, NULL, 50, 2},
	{"animal", vault_aux_animal, NULL, 50, 4},
	{"chapel", vault_aux_chapel, NULL, 65, 2},
	{"undead", vault_aux_undead, NULL, 65, 4},
	{NULL, NULL, NULL, 0, 0},
};


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
 * Note that the "get_mon_num()" function may (rarely) fail, in which
 * case the nest will be empty, and will not affect the level rating.
 *
 * Note that "monster nests" will never contain "unique" monsters.
 */
static void build_type5(int by0, int bx0)
{
	int y, x, y1, x1, y2, x2, xval, yval;
	int i;
	int what[64];

	int align = 0;

	cave_type *c_ptr;

	vault_aux_type *n_ptr = pick_vault_type(nest_types);

	/* Try to allocate space for room. */
	if (!room_alloc(25, 11, TRUE, by0, bx0, &xval, &yval))
		return;

	/* No type available */
	if (!n_ptr)
		return;

	/* Process a preparation function if necessary */
	if (n_ptr->prep_func)
		(*(n_ptr->prep_func)) ();

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
		c_ptr = &cave[y][x1 - 1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2 + 1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2 + 1][x];
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
		c_ptr = &cave[y][x1 - 1];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y][x2 + 1];
		c_ptr->feat = FEAT_WALL_INNER;
	}

	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y2 + 1][x];
		c_ptr->feat = FEAT_WALL_INNER;
	}


	/* Place a secret door */
	switch (randint(4))
	{
		case 1:
			place_secret_door(y1 - 1, xval);
			break;
		case 2:
			place_secret_door(y2 + 1, xval);
			break;
		case 3:
			place_secret_door(yval, x1 - 1);
			break;
		case 4:
			place_secret_door(yval, x2 + 1);
			break;
	}

	/* Prepare allocation table */
	get_mon_num_prep(n_ptr->hook_func, NULL);

	/* Pick some monster types */
	for (i = 0; i < 64; i++)
	{
		int r_idx = 0, attempts = 100;

		while (attempts--)
		{
			/* Get a (hard) monster type */
			r_idx = get_mon_num(dun_level + 10);

			/* Decline incorrect alignment */
			if (((align < 0) && (r_info[r_idx].flags3 & RF3_GOOD)) ||
				((align > 0) && (r_info[r_idx].flags3 & RF3_EVIL)))
			{
				continue;
			}

			/* Accept this monster */
			break;
		}

		/* Notice failure */
		if (!r_idx || !attempts)
			return;

		/* Note the alignment */
		if (r_info[r_idx].flags3 & RF3_GOOD)
			align++;
		else if (r_info[r_idx].flags3 & RF3_EVIL)
			align--;

		what[i] = r_idx;
	}

	/* Describe */
	if (cheat_room)
	{
		/* Room type */
		msg_format("Monster nest (%s)", n_ptr->name);
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
			(void) place_monster_aux(y, x, r_idx, FALSE, FALSE, FALSE,
				FALSE);
		}
	}
}


static vault_aux_type pit_types[] = {
	{"orc", vault_aux_orc, NULL, 5, 4},
	{"troll", vault_aux_troll, NULL, 20, 4},
	{"giant", vault_aux_giant, NULL, 40, 4},
	{"lovecraftian", vault_aux_cthulhu, NULL, 40, 4},
	{"clone", vault_aux_symbol, vault_prep_symbol, 55, 3},
	{"chapel", vault_aux_chapel, NULL, 55, 1},
	{"dragon", vault_aux_dragon, vault_prep_dragon, 70, 4},
	{"demon", vault_aux_demon, NULL, 80, 4},
	{NULL, NULL, NULL, 0, 0},
};


/*
 * Type 6 -- Monster pits
 *
 * A monster pit is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type organized in the room.
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
static void build_type6(int by0, int bx0)
{
	int y, x, y1, x1, y2, x2, xval, yval;
	int i, j;

	int what[16];

	int align = 0;

	cave_type *c_ptr;

	vault_aux_type *n_ptr = pick_vault_type(pit_types);

	/* Try to allocate space for room. */
	if (!room_alloc(25, 11, TRUE, by0, bx0, &xval, &yval))
		return;

	/* No type available */
	if (!n_ptr)
		return;

	/* Process a preparation function if necessary */
	if (n_ptr->prep_func)
		(*(n_ptr->prep_func)) ();

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
		c_ptr = &cave[y][x1 - 1];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y][x2 + 1];
		c_ptr->feat = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		c_ptr->feat = FEAT_WALL_OUTER;
		c_ptr = &cave[y2 + 1][x];
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
		c_ptr = &cave[y][x1 - 1];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y][x2 + 1];
		c_ptr->feat = FEAT_WALL_INNER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		c_ptr->feat = FEAT_WALL_INNER;
		c_ptr = &cave[y2 + 1][x];
		c_ptr->feat = FEAT_WALL_INNER;
	}

	/* Place a secret door */
	switch (randint(4))
	{
		case 1:
			place_secret_door(y1 - 1, xval);
			break;
		case 2:
			place_secret_door(y2 + 1, xval);
			break;
		case 3:
			place_secret_door(yval, x1 - 1);
			break;
		case 4:
			place_secret_door(yval, x2 + 1);
			break;
	}

	/* Prepare allocation table */
	get_mon_num_prep(n_ptr->hook_func, NULL);

	/* Pick some monster types */
	for (i = 0; i < 16; i++)
	{
		int r_idx = 0, attempts = 100;

		while (attempts--)
		{
			/* Get a (hard) monster type */
			r_idx = get_mon_num(dun_level + 10);

			/* Decline incorrect alignment */
			if (((align < 0) && (r_info[r_idx].flags3 & RF3_GOOD)) ||
				((align > 0) && (r_info[r_idx].flags3 & RF3_EVIL)))
			{
				continue;
			}

			/* Accept this monster */
			break;
		}

		/* Notice failure */
		if (!r_idx || !attempts)
			return;

		/* Note the alignment */
		if (r_info[r_idx].flags3 & RF3_GOOD)
			align++;
		else if (r_info[r_idx].flags3 & RF3_EVIL)
			align--;

		what[i] = r_idx;
	}

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

	/* Message */
	if (cheat_room)
	{
		/* Room type */
		msg_format("Monster pit (%s)", n_ptr->name);
	}

	/* Select the entries */
	for (i = 0; i < 8; i++)
	{
		/* Every other entry */
		what[i] = what[i * 2];

		if (cheat_hear)
		{
			/* Message */
			msg_print(r_name + r_info[what[i]].name);
		}
	}

	/* Increase the level rating */
	rating += 10;

	/* (Sometimes) Cause a "special feeling" (for "Monster Pits") */
	if ((dun_level <= 40) && (randint(dun_level * dun_level + 50) < 300))
	{
		good_item_flag = TRUE;
	}

	/* Top and bottom rows */
	for (x = xval - 9; x <= xval + 9; x++)
	{
		place_monster_aux(yval - 2, x, what[0], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(yval + 2, x, what[0], FALSE, FALSE, FALSE,
			FALSE);
	}

	/* Middle columns */
	for (y = yval - 1; y <= yval + 1; y++)
	{
		place_monster_aux(y, xval - 9, what[0], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(y, xval + 9, what[0], FALSE, FALSE, FALSE,
			FALSE);

		place_monster_aux(y, xval - 8, what[1], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(y, xval + 8, what[1], FALSE, FALSE, FALSE,
			FALSE);

		place_monster_aux(y, xval - 7, what[1], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(y, xval + 7, what[1], FALSE, FALSE, FALSE,
			FALSE);

		place_monster_aux(y, xval - 6, what[2], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(y, xval + 6, what[2], FALSE, FALSE, FALSE,
			FALSE);

		place_monster_aux(y, xval - 5, what[2], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(y, xval + 5, what[2], FALSE, FALSE, FALSE,
			FALSE);

		place_monster_aux(y, xval - 4, what[3], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(y, xval + 4, what[3], FALSE, FALSE, FALSE,
			FALSE);

		place_monster_aux(y, xval - 3, what[3], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(y, xval + 3, what[3], FALSE, FALSE, FALSE,
			FALSE);

		place_monster_aux(y, xval - 2, what[4], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(y, xval + 2, what[4], FALSE, FALSE, FALSE,
			FALSE);
	}

	/* Above/Below the center monster */
	for (x = xval - 1; x <= xval + 1; x++)
	{
		place_monster_aux(yval + 1, x, what[5], FALSE, FALSE, FALSE,
			FALSE);
		place_monster_aux(yval - 1, x, what[5], FALSE, FALSE, FALSE,
			FALSE);
	}

	/* Next to the center monster */
	place_monster_aux(yval, xval + 1, what[6], FALSE, FALSE, FALSE, FALSE);
	place_monster_aux(yval, xval - 1, what[6], FALSE, FALSE, FALSE, FALSE);

	/* Center monster */
	place_monster_aux(yval, xval, what[7], FALSE, FALSE, FALSE, FALSE);
}


/* coordinate translation code */
static void coord_trans(int *x, int *y, int xoffset, int yoffset,
	int transno)
{
	int i;
	int temp;

	/*
	 * transno specifies what transformation is required. (0-7)
	 * The lower two bits indicate by how much the vault is rotated,
	 * and the upper bit indicates a reflection.
	 * This is done by using rotation matrices... however since
	 * these are mostly zeros for rotations by 90 degrees this can
	 * be expressed simply in terms of swapping and inverting the
	 * x and y coordinates.
	 */
	for (i = 0; i <= transno % 4; i++)
	{
		/* rotate by 90 degrees */
		temp = *x;
		*x = -(*y);
		*y = temp;
	}

	if (transno / 4)
	{
		/* Reflect depending on status of 3rd bit. */
		*x = -(*x);
	}

	/* Add offsets so vault stays in the first quadrant */
	*x += xoffset;
	*y += yoffset;
}


/*
 * Hack -- fill in "vault" rooms
 */
static void build_vault(int yval, int xval, int ymax, int xmax, cptr data,
	int xoffset, int yoffset, int transno)
{
	int dx, dy, x, y, i, j;

	cptr t;

	cave_type *c_ptr;


	/* Place dungeon features and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; dx++, t++)
		{
			/* prevent loop counter from being overwritten */
			i = dx;
			j = dy;

			/* Flip / rotate */
			coord_trans(&i, &j, xoffset, yoffset, transno);

			/* Extract the location */
			if (transno % 2)
			{
				/* no swap of x/y */
				x = xval - (xmax / 2) + i;
				y = yval - (ymax / 2) + j;
			}
			else
			{
				/* swap of x/y */
				x = xval - (ymax / 2) + i;
				y = yval - (xmax / 2) + j;
			}

			/* Hack -- skip "non-grids" */
			if (*t == ' ')
				continue;

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
			/* prevent loop counter from being overwritten */
			i = dx;
			j = dy;

			/* Flip / rotate */
			coord_trans(&i, &j, xoffset, yoffset, transno);

			/* Extract the location */
			if (transno % 2)
			{
				/* no swap of x/y */
				x = xval - (xmax / 2) + i;
				y = yval - (ymax / 2) + j;
			}
			else
			{
				/* swap of x/y */
				x = xval - (ymax / 2) + i;
				y = yval - (xmax / 2) + j;
			}

			/* Hack -- skip "non-grids" */
			if (*t == ' ')
				continue;

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
static void build_type7(int by0, int bx0)
{
	vault_type *v_ptr = NULL;
	int dummy = 0;
	int x, y;
	int xval, yval;
	int xoffset, yoffset;
	int transno;

	/* Pick a lesser vault */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* Access a random vault record */
		v_ptr = &v_info[rand_int(max_v_idx)];

		/* Accept the first lesser vault */
		if (v_ptr->typ == 7)
			break;
	}

	/* No lesser vault found */
	if (!v_ptr)
		return;

	/* pick type of transformation (0-7) */
	transno = rand_int(8);

	/* calculate offsets */
	x = v_ptr->wid;
	y = v_ptr->hgt;

	coord_trans(&x, &y, 0, 0, transno);

	if (x < 0)
	{
		xoffset = -x - 1;
	}
	else
	{
		xoffset = 0;
	}

	if (y < 0)
	{
		yoffset = -y - 1;
	}
	else
	{
		yoffset = 0;
	}

	/* Try to allocate space for room. */
	if (!room_alloc(abs(x), abs(y), FALSE, by0, bx0, &xval, &yval))
		return;

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
	if (cheat_room)
		msg_format("%s", v_name + v_ptr->name);

	/* Boost the rating */
	rating += v_ptr->rat;

	/* (Sometimes) Cause a special feeling */
	if ((dun_level <= 50) ||
		(randint((dun_level - 40) * (dun_level - 40) + 50) < 400))
	{
		good_item_flag = TRUE;
	}

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr->hgt, v_ptr->wid, v_text + v_ptr->text,
		xoffset, yoffset, transno);
}


/*
 * Type 8 -- greater vaults (see "v_info.txt")
 */
static void build_type8(int by0, int bx0)
{
	vault_type *v_ptr = NULL;
	int dummy = 0;
	int xval, yval;
	int x, y;
	int transno;
	int xoffset, yoffset;

	/* Pick a greater vault */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* Access a random vault record */
		v_ptr = &v_info[rand_int(max_v_idx)];

		/* Accept the first greater vault */
		if (v_ptr->typ == 8)
			break;
	}

	/* No greater vault found */
	if (!v_ptr)
		return;

	/* pick type of transformation (0-7) */
	transno = rand_int(8);

	/* calculate offsets */
	x = v_ptr->wid;
	y = v_ptr->hgt;

	coord_trans(&x, &y, 0, 0, transno);

	if (x < 0)
	{
		xoffset = -x - 1;
	}
	else
	{
		xoffset = 0;
	}

	if (y < 0)
	{
		yoffset = -y - 1;
	}
	else
	{
		yoffset = 0;
	}

	/* Try to allocate space for room.  If fails, exit */
	if (!room_alloc(abs(x), abs(y), FALSE, by0, bx0, &xval, &yval))
		return;

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
	if (cheat_room)
		msg_format("%s", v_name + v_ptr->name);

	/* Boost the rating */
	rating += v_ptr->rat;

	/* (Sometimes) Cause a special feeling */
	if ((dun_level <= 50) ||
		(randint((dun_level - 40) * (dun_level - 40) + 50) < 400))
	{
		good_item_flag = TRUE;
	}

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr->hgt, v_ptr->wid, v_text + v_ptr->text,
		xoffset, yoffset, transno);
}


/*
 * Structure to hold all "fill" data
 */

typedef struct fill_data_type fill_data_type;

struct fill_data_type
{
	/* area size */
	int xmin;
	int ymin;
	int xmax;
	int ymax;

	/* cutoffs */
	int c1;
	int c2;
	int c3;

	/* features to fill with */
	int feat1;
	int feat2;
	int feat3;

	/* number of filled squares */
	int amount;
};

static fill_data_type fill_data;


/* Store routine for the fractal cave generator */
/* this routine probably should be an inline function or a macro. */
static void store_height(int x, int y, int val)
{
	/* if on boundary set val > cutoff so walls are not as square */
	if (((x == fill_data.xmin) || (y == fill_data.ymin) ||
			(x == fill_data.xmax) || (y == fill_data.ymax)) &&
		(val <= fill_data.c1)) val = fill_data.c1 + 1;

	/* store the value in height-map format */
	cave[y][x].feat = val;

	return;
}


/*
* Explanation of the plasma fractal algorithm:
*
* A grid of points is created with the properties of a 'height-map'
* This is done by making the corners of the grid have a random value.
* The grid is then subdivided into one with twice the resolution.
* The new points midway between two 'known' points can be calculated
* by taking the average value of the 'known' ones and randomly adding
* or subtracting an amount proportional to the distance between those
* points.  The final 'middle' points of the grid are then calculated
* by averaging all four of the originally 'known' corner points.  An
* random amount is added or subtracted from this to get a value of the
* height at that point.  The scaling factor here is adjusted to the
* slightly larger distance diagonally as compared to orthogonally.
*
* This is then repeated recursively to fill an entire 'height-map'
* A rectangular map is done the same way, except there are different
* scaling factors along the x and y directions.
*
* A hack to change the amount of correlation between points is done using
* the grd variable.  If the current step size is greater than grd then
* the point will be random, otherwise it will be calculated by the
* above algorithm.  This makes a maximum distance at which two points on
* the height map can affect each other.
*
* How fractal caves are made:
*
* When the map is complete, a cut-off value is used to create a cave.
* Heights below this value are "floor", and heights above are "wall".
* This also can be used to create lakes, by adding more height levels
* representing shallow and deep water/ lava etc.
*
* The grd variable affects the width of passages.
* The roug variable affects the roughness of those passages
*
* The tricky part is making sure the created cave is connected.  This
* is done by 'filling' from the inside and only keeping the 'filled'
* floor.  Walls bounding the 'filled' floor are also kept.  Everything
* else is converted to the normal granite FEAT_WALL_EXTRA.
 */


/*
 *  Note that this uses the cave.feat array in a very hackish way
 *  the values are first set to zero, and then each array location
 *  is used as a "heightmap"
 *  The heightmap then needs to be converted back into the "feat" format.
 *
 *  grd=level at which fractal turns on.  smaller gives more mazelike caves
 *  roug=roughness level.  16=normal.  higher values make things more convoluted
 *    small values are good for smooth walls.
 *  size=length of the side of the square cave system.
 */
static void generate_hmap(int y0, int x0, int xsiz, int ysiz, int grd,
	int roug, int cutoff)
{
	int xhsize, yhsize, xsize, ysize, maxsize;

	/*
	 * fixed point variables- these are stored as 256 x normal value
	 * this gives 8 binary places of fractional part + 8 places of normal part
	 */

	u16b xstep, xhstep, ystep, yhstep;
	u16b xstep2, xhstep2, ystep2, yhstep2;
	u16b i, j, ii, jj, diagsize, xxsize, yysize;

	/* Cache for speed */
	u16b xm, xp, ym, yp;

	/* redefine size so can change the value if out of range */
	xsize = xsiz;
	ysize = ysiz;

	/* Paranoia about size of the system of caves */
	if (xsize > 254)
		xsize = 254;
	if (xsize < 4)
		xsize = 4;
	if (ysize > 254)
		ysize = 254;
	if (ysize < 4)
		ysize = 4;

	/* get offsets to middle of array */
	xhsize = xsize / 2;
	yhsize = ysize / 2;

	/* fix rounding problem */
	xsize = xhsize * 2;
	ysize = yhsize * 2;

	/* get limits of region */
	fill_data.xmin = x0 - xhsize;
	fill_data.ymin = y0 - yhsize;
	fill_data.xmax = x0 + xhsize;
	fill_data.ymax = y0 + yhsize;

	/* Store cutoff in global for quick access */
	fill_data.c1 = cutoff;

	/*
	 * * Scale factor for middle points:
	 * * About sqrt(2) * 256 - correct for a square lattice
	 * * approximately correct for everything else.
	 */
	diagsize = 362;

	/* maximum of xsize and ysize */
	maxsize = (xsize > ysize) ? xsize : ysize;

	/* Clear the section */
	for (i = 0; i <= xsize; i++)
	{
		for (j = 0; j <= ysize; j++)
		{
			/* 255 is a flag for "not done yet" */
			cave[(int) (fill_data.ymin + j)][(int) (fill_data.xmin +
					i)].feat = 255;
			/* Clear icky flag because may be redoing the cave */
			cave[(int) (fill_data.ymin + j)][(int) (fill_data.xmin +
					i)].info &= ~(CAVE_ICKY);
		}
	}

	/* Boundaries are walls */
	cave[fill_data.ymin][fill_data.xmin].feat = maxsize;
	cave[fill_data.ymax][fill_data.xmin].feat = maxsize;
	cave[fill_data.ymin][fill_data.xmax].feat = maxsize;
	cave[fill_data.ymax][fill_data.xmax].feat = maxsize;

	/* Set the middle square to be an open area. */
	cave[y0][x0].feat = 0;

	/* Initialize the step sizes */
	xstep = xhstep = xsize * 256;
	ystep = yhstep = ysize * 256;
	xxsize = xsize * 256;
	yysize = ysize * 256;

	/*
	 * Fill in the rectangle with fractal height data -
	 * like the 'plasma fractal' in fractint.
	 */
	while ((xhstep > 256) || (yhstep > 256))
	{
		/* Halve the step sizes */
		xstep = xhstep;
		xhstep /= 2;
		ystep = yhstep;
		yhstep /= 2;

		/* cache well used values */
		xstep2 = xstep / 256;
		ystep2 = ystep / 256;

		xhstep2 = xhstep / 256;
		yhstep2 = yhstep / 256;

		/* middle top to bottom. */
		for (i = xhstep; i <= xxsize - xhstep; i += xstep)
		{
			for (j = 0; j <= yysize; j += ystep)
			{
				/* cache often used values */
				ii = i / 256 + fill_data.xmin;
				jj = j / 256 + fill_data.ymin;

				/* Test square */
				if (cave[jj][ii].feat == 255)
				{
					if (xhstep2 > grd)
					{
						/* If greater than 'grid' level then is random */
						store_height(ii, jj, randint(maxsize));
					}
					else
					{
						/* Average of left and right points +random bit */
						store_height(ii, jj,
							(cave[jj][fill_data.xmin + (i -
					   xhstep) / 256].feat + cave[jj][fill_data.xmin + (i +
										xhstep) / 256].feat) / 2 +
							(randint(xstep2) - xhstep2) * roug / 16);
					}
				}
			}
		}


		/* middle left to right. */
		for (j = yhstep; j <= yysize - yhstep; j += ystep)
		{
			for (i = 0; i <= xxsize; i += xstep)
			{
				/* cache often used values */
				ii = i / 256 + fill_data.xmin;
				jj = j / 256 + fill_data.ymin;

				/* Test square */
				if (cave[jj][ii].feat == 255)
				{
					if (xhstep2 > grd)
					{
						/* If greater than 'grid' level then is random */
						store_height(ii, jj, randint(maxsize));
					}
					else
					{
						/* Average of up and down points +random bit */
						store_height(ii, jj,
							(cave[fill_data.ymin + (j -
					   yhstep) / 256][ii].feat + cave[fill_data.ymin + (j +
										yhstep) / 256][ii].feat) / 2 +
							(randint(ystep2) - yhstep2) * roug / 16);
					}
				}
			}
		}

		/* center. */
		for (i = xhstep; i <= xxsize - xhstep; i += xstep)
		{
			for (j = yhstep; j <= yysize - yhstep; j += ystep)
			{
				/* cache often used values */
				ii = i / 256 + fill_data.xmin;
				jj = j / 256 + fill_data.ymin;

				/* Test square */
				if (cave[jj][ii].feat == 255)
				{
					if (xhstep2 > grd)
					{
						/* If greater than 'grid' level then is random */
						store_height(ii, jj, randint(maxsize));
					}
					else
					{
						/* Cache reused values. */
						xm = fill_data.xmin + (i - xhstep) / 256;
						xp = fill_data.xmin + (i + xhstep) / 256;
						ym = fill_data.ymin + (j - yhstep) / 256;
						yp = fill_data.ymin + (j + yhstep) / 256;

						/* 
						 * Average over all four corners + scale by diagsize to
						 * reduce the effect of the square grid on the shape of the fractal
						 */
						store_height(ii, jj,
							(cave[ym][xm].feat + cave[yp][xm].feat +
								cave[ym][xp].feat +
								cave[yp][xp].feat) / 4 + (randint(xstep2) -
								xhstep2) * (diagsize / 16) / 256 * roug);
					}
				}
			}
		}
	}
}


static bool hack_isnt_wall(int y, int x, int c1, int c2, int c3, int feat1,
	int feat2, int feat3)
{
	/*
	 * function used to convert from height-map back to the
	 *  normal angband cave format
	 */
	if (cave[y][x].info & CAVE_ICKY)
	{
		/* already done */
		return FALSE;
	}
	else
	{
		/* Show that have looked at this square */
		cave[y][x].info |= (CAVE_ICKY);

		/* Use cutoffs c1-c3 to allocate regions of floor /water/ lava etc. */
		if (cave[y][x].feat <= c1)
		{
			/* 25% of the time use the other tile : it looks better this way */
			if (randint(100) < 75)
			{
				cave[y][x].feat = feat1;
				return TRUE;
			}
			else
			{
				cave[y][x].feat = feat2;
				return TRUE;
			}
		}
		else if (cave[y][x].feat <= c2)
		{
			/* 25% of the time use the other tile : it looks better this way */
			if (randint(100) < 75)
			{
				cave[y][x].feat = feat2;
				return TRUE;
			}
			else
			{
				cave[y][x].feat = feat1;
				return TRUE;
			}
		}
		else if (cave[y][x].feat <= c3)
		{
			cave[y][x].feat = feat3;
			return TRUE;
		}
		/* if greater than cutoff then is a wall */
		else
		{
			cave[y][x].feat = FEAT_WALL_OUTER;
			return FALSE;
		}
	}
}




/*
 * Fill the fractal height-map using the features specified in in fill_data.
 *
 * This routine is similar to the method used to update the monster flow
 * information.  It uses the temp grids as a circular queue.
 */
static void cave_fill(byte y, byte x)
{

	int i, j, d;
	int ty, tx;

	int flow_tail = 1;
	int flow_head = 0;


	/*** Start Grid ***/

	/* Enqueue that entry */
	temp_y[0] = y;
	temp_x[0] = x;


	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		ty = temp_y[flow_head];
		tx = temp_x[flow_head];

		/* Forget that entry */
		if (++flow_head == TEMP_MAX)
			flow_head = 0;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			int old_head = flow_tail;

			/* Child location */
			j = ty + ddy_ddd[d];
			i = tx + ddx_ddd[d];

			/* Paranoia Don't leave the cave */
			if (!in_bounds(j, i))
				return;

			/* If within bounds */
			if ((i > fill_data.xmin) && (i < fill_data.xmax) &&
				(j > fill_data.ymin) && (j < fill_data.ymax))
			{
				/* If not a wall or floor done before */
				if (hack_isnt_wall(j, i, fill_data.c1, fill_data.c2,
						fill_data.c3, fill_data.feat1, fill_data.feat2,
						fill_data.feat3))
				{

					/* Enqueue that entry */
					temp_y[flow_tail] = j;
					temp_x[flow_tail] = i;

					/* Advance the queue */
					if (++flow_tail == TEMP_MAX)
						flow_tail = 0;

					/* Hack -- Overflow by forgetting new entry */
					if (flow_tail == flow_head)
					{
						flow_tail = old_head;
					}
					else
					{
						/* keep tally of size of cave system */
						(fill_data.amount)++;
					}
				}
			}
			else
			{
				/* affect boundary */
				cave[j][i].info |= CAVE_ICKY;
			}
		}
	}
}


static bool generate_fracave(int y0, int x0, int xsize, int ysize,
	int cutoff, bool light, bool room)
{
	int x, y, i, xhsize, yhsize;


	/* offsets to middle from corner */
	xhsize = xsize / 2;
	yhsize = ysize / 2;


	/*
	 * select region connected to center of cave system
	 * this gets rid of alot of isolated one-sqaures that
	 * can make teleport traps instadeaths...
	 */

	/* cutoffs */
	fill_data.c1 = cutoff;
	fill_data.c2 = 0;
	fill_data.c3 = 0;

	/* features to fill with */
	fill_data.feat1 = FEAT_FLOOR;
	fill_data.feat2 = FEAT_FLOOR;
	fill_data.feat3 = FEAT_FLOOR;

	/* number of filled squares */
	fill_data.amount = 0;

	cave_fill((byte) y0, (byte) x0);

	/* if tally too small, try again */
	if (fill_data.amount < 10)
	{
		/* too small - clear area and try again later */
		for (x = 0; x <= xsize; ++x)
		{
			for (y = 0; y <= ysize; ++y)
			{
				cave[y0 + y - yhsize][x0 + x - xhsize].feat =
					FEAT_WALL_EXTRA;
				cave[y0 + y - yhsize][x0 + x - xhsize].info &=
					~(CAVE_ICKY | CAVE_ROOM);
			}
		}
		return FALSE;
	}

	/*
	 * Do boundarys-check to see if they are next to a filled region
	 * If not then they are set to normal granite
	 * If so then they are marked as room walls.
	 */
	for (i = 0; i <= xsize; ++i)
	{
		/* top boundary */
		if ((cave[0 + y0 - yhsize][i + x0 - xhsize].info & CAVE_ICKY) &&
			(room))
		{
			/* Next to a 'filled' region? - set to be room walls */
			cave[y0 + 0 - yhsize][x0 + i - xhsize].feat = FEAT_WALL_OUTER;
			if (light)
				cave[y0 + 0 - yhsize][x0 + i - xhsize].info |= (CAVE_GLOW);
			cave[y0 + 0 - yhsize][x0 + i - xhsize].info |= (CAVE_ROOM);
			cave[y0 + 0 - yhsize][x0 + i - xhsize].feat = FEAT_WALL_OUTER;
		}
		else
		{
			/* set to be normal granite */
			cave[y0 + 0 - yhsize][x0 + i - xhsize].feat = FEAT_WALL_EXTRA;
		}

		/* bottom boundary */
		if ((cave[ysize + y0 - yhsize][i + x0 - xhsize].info & CAVE_ICKY)
			&& (room))
		{
			/* Next to a 'filled' region? - set to be room walls */
			cave[y0 + ysize - yhsize][x0 + i - xhsize].feat =
				FEAT_WALL_OUTER;
			if (light)
				cave[y0 + ysize - yhsize][x0 + i - xhsize].info |=
					(CAVE_GLOW);
			cave[y0 + ysize - yhsize][x0 + i - xhsize].info |= (CAVE_ROOM);
			cave[y0 + ysize - yhsize][x0 + i - xhsize].feat =
				FEAT_WALL_OUTER;
		}
		else
		{
			/* set to be normal granite */
			cave[y0 + ysize - yhsize][x0 + i - xhsize].feat =
				FEAT_WALL_EXTRA;
		}

		/* clear the icky flag-don't need it any more */
		cave[y0 + 0 - yhsize][x0 + i - xhsize].info &= ~(CAVE_ICKY);
		cave[y0 + ysize - yhsize][x0 + i - xhsize].info &= ~(CAVE_ICKY);
	}

	/* Do the left and right boundaries minus the corners (done above) */
	for (i = 1; i < ysize; ++i)
	{
		/* left boundary */
		if ((cave[i + y0 - yhsize][0 + x0 - xhsize].info & CAVE_ICKY) &&
			room)
		{
			/* room boundary */
			cave[y0 + i - yhsize][x0 + 0 - xhsize].feat = FEAT_WALL_OUTER;
			if (light)
				cave[y0 + i - yhsize][x0 + 0 - xhsize].info |= (CAVE_GLOW);
			cave[y0 + i - yhsize][x0 + 0 - xhsize].info |= (CAVE_ROOM);
			cave[y0 + i - yhsize][x0 + 0 - xhsize].feat = FEAT_WALL_OUTER;
		}
		else
		{
			/* outside room */
			cave[y0 + i - yhsize][x0 + 0 - xhsize].feat = FEAT_WALL_EXTRA;
		}
		/* right boundary */
		if ((cave[i + y0 - yhsize][xsize + x0 - xhsize].info & CAVE_ICKY)
			&& room)
		{
			/* room boundary */
			cave[y0 + i - yhsize][x0 + xsize - xhsize].feat =
				FEAT_WALL_OUTER;
			if (light)
				cave[y0 + i - yhsize][x0 + xsize - xhsize].info |=
					(CAVE_GLOW);
			cave[y0 + i - yhsize][x0 + xsize - xhsize].info |= (CAVE_ROOM);
			cave[y0 + i - yhsize][x0 + xsize - xhsize].feat =
				FEAT_WALL_OUTER;
		}
		else
		{
			/* outside room */
			cave[y0 + i - yhsize][x0 + xsize - xhsize].feat =
				FEAT_WALL_EXTRA;
		}

		/* clear icky flag -done with it */
		cave[y0 + i - yhsize][x0 + 0 - xhsize].info &= ~(CAVE_ICKY);
		cave[y0 + i - yhsize][x0 + xsize - xhsize].info &= ~(CAVE_ICKY);
	}


	/* Do the rest: convert back to the normal format */
	for (x = 1; x < xsize; ++x)
	{
		for (y = 1; y < ysize; ++y)
		{
			if ((cave[y0 + y - yhsize][x0 + x - xhsize].feat == FEAT_FLOOR)
				&& (cave[y0 + y - yhsize][x0 + x -
						xhsize].info & CAVE_ICKY))
			{
				/* Clear the icky flag in the filled region */
				cave[y0 + y - yhsize][x0 + x - xhsize].info &= ~CAVE_ICKY;

				/* Set appropriate flags */
				if (light)
					cave[y0 + y - yhsize][x0 + x - xhsize].info |=
						(CAVE_GLOW);
				if (room)
					cave[y0 + y - yhsize][x0 + x - xhsize].info |=
						(CAVE_ROOM);
			}
			else if ((cave[y0 + y - yhsize][x0 + x - xhsize].feat ==
					FEAT_WALL_OUTER) &&
				(cave[y0 + y - yhsize][x0 + x - xhsize].info & CAVE_ICKY))
			{
				/* Walls */
				cave[y0 + y - yhsize][x0 + x - xhsize].info &=
					~(CAVE_ICKY);
				if (light)
					cave[y0 + y - yhsize][x0 + x - xhsize].info |=
						(CAVE_GLOW);
				if (room)
				{
					cave[y0 + y - yhsize][x0 + x - xhsize].info |=
						(CAVE_ROOM);
				}
				else
				{

					cave[y0 + y - yhsize][x0 + x - xhsize].feat =
						FEAT_WALL_EXTRA;
					cave[y0 + y - yhsize][x0 + x - xhsize].info &=
						~(CAVE_ROOM);
				}
			}
			else
			{
				/* Clear the unconnected regions */
				cave[y0 + y - yhsize][x0 + x - xhsize].feat =
					FEAT_WALL_EXTRA;
				cave[y0 + y - yhsize][x0 + x - xhsize].info &=
					~(CAVE_ICKY | CAVE_ROOM);
			}
		}
	}

	/*
	 * XXX XXX XXX There is a slight problem when tunnels pierce the caves:
	 * Extra doors appear inside the system.  (Its not very noticeable though.)
	 * This can be removed by "filling" from the outside in.  This allows a separation
	 * from FEAT_WALL_OUTER with FEAT_WALL_INNER.  (Internal walls are  F.W.OUTER instead.)
	 * The extra effort for what seems to be only a minor thing (even non-existant if you
	 * think of the caves not as normal rooms, but as holes in the dungeon), doesn't seem
	 * worth it.
	 */

	return TRUE;
}



/*
 * Driver routine to create fractal cave system
 */
static void build_type9(int by0, int bx0)
{
	int grd, roug, cutoff, xsize, ysize, y0, x0;

	bool done, light, room;

	/* get size: note 'Evenness' */
	xsize = randint(22) * 2 + 6;
	ysize = randint(15) * 2 + 6;

	/* Try to allocate space for room.  If fails, exit */
	if (!room_alloc(xsize + 1, ysize + 1, FALSE, by0, bx0, &x0, &y0))
		return;

	light = done = FALSE;
	room = TRUE;

	if (dun_level <= randint(25))
		light = TRUE;

	while (!done)
	{
		/* Note: size must be even or there are rounding problems
		 * * This causes the tunnels not to connect properly to the room */

		/* testing values for these parameters feel free to adjust */
		grd = 1 << (rand_int(4));

		/* want average of about 16 */
		roug = randint(8) * randint(4);

		/* about size/2 */
		cutoff =
			randint(xsize / 4) + randint(ysize / 4) + randint(xsize / 4) +
			randint(ysize / 4);

		/* make it */
		generate_hmap(y0, x0, xsize, ysize, grd, roug, cutoff);

		/* Convert to normal format + clean up */
		done = generate_fracave(y0, x0, xsize, ysize, cutoff, light, room);
	}
}

#ifdef ALLOW_CAVERNS_AND_LAKES
/*
 * Builds a cave system in the center of the dungeon.
 */
void build_cavern(void)
{
	int grd, roug, cutoff, xsize, ysize, x0, y0;
	bool done, light;

	light = done = FALSE;
	if (dun_level <= randint(50))
		light = TRUE;

	/* Make a cave the size of the dungeon */
	xsize = cur_wid - 1;
	ysize = cur_hgt - 1;
	x0 = xsize / 2;
	y0 = ysize / 2;

	/* Paranoia: make size even */
	xsize = x0 * 2;
	ysize = y0 * 2;

	while (!done)
	{
		/* testing values for these parameters: feel free to adjust */
		grd = randint(4) + 4;

		/* want average of about 16 */
		roug = randint(8) * randint(4);

		/* about size/2 */
		cutoff = xsize / 2;

		/* make it */
		generate_hmap(y0 + 1, x0 + 1, xsize, ysize, grd, roug, cutoff);

		/* Convert to normal format+ clean up */
		done =
			generate_fracave(y0 + 1, x0 + 1, xsize, ysize, cutoff, light,
			FALSE);
	}
}

static bool generate_lake(int y0, int x0, int xsize, int ysize, int c1,
	int c2, int c3, int type)
{
	int x, y, i, xhsize, yhsize;
	int feat1, feat2, feat3;

	/* offsets to middle from corner */
	xhsize = xsize / 2;
	yhsize = ysize / 2;

	/* Get features based on type */
	switch (type)
	{
		case 1:
		{
			/* Lava */
			feat1 = FEAT_DEEP_LAVA;
			feat2 = FEAT_SHAL_LAVA;
			feat3 = FEAT_FLOOR;
		};
			break;
		case 2:
		{
			/* Water */
			feat1 = FEAT_DEEP_WATER;
			feat2 = FEAT_SHAL_WATER;
			feat3 = FEAT_FLOOR;
		};
			break;
		case 3:
		{
			/* Collapsed cave */
			feat1 = FEAT_FLOOR;
			feat2 = FEAT_FLOOR;
			feat3 = FEAT_RUBBLE;
		};
			break;
		case 4:
		{
			/* Earth vault */
			feat1 = FEAT_RUBBLE;
			feat2 = FEAT_FLOOR;
			feat3 = FEAT_RUBBLE;
		};
			break;
		case 5:
		{
			/* Air vault */
			feat1 = FEAT_FLOOR;
			feat2 = FEAT_TREES;
			feat3 = FEAT_FLOOR;
		};
			break;
		case 6:
		{
			/* Water vault */
			feat1 = FEAT_SHAL_WATER;
			feat2 = FEAT_DEEP_WATER;
			feat3 = FEAT_SHAL_WATER;
		};
			break;
		case 7:
		{
			/* Fire Vault */
			feat1 = FEAT_SHAL_LAVA;
			feat2 = FEAT_DEEP_LAVA;
			feat3 = FEAT_SHAL_LAVA;
		};
			break;

			/* Paranoia */
		default:
			return FALSE;
	}

	/*
	 * select region connected to center of cave system
	 * this gets rid of alot of isolated one-sqaures that
	 * can make teleport traps instadeaths...
	 */

	/* cutoffs */
	fill_data.c1 = c1;
	fill_data.c2 = c2;
	fill_data.c3 = c3;

	/* features to fill with */
	fill_data.feat1 = feat1;
	fill_data.feat2 = feat2;
	fill_data.feat3 = feat3;

	/* number of filled squares */
	fill_data.amount = 0;

	/* select region connected to center of cave system
	 * * this gets rid of alot of isolated one-sqaures that
	 * * can make teleport traps instadeaths... */
	cave_fill((byte) y0, (byte) x0);

	/* if tally too small, try again */
	if (fill_data.amount < 10)
	{
		/* too small -clear area and try again later */
		for (x = 0; x <= xsize; ++x)
		{
			for (y = 0; y <= ysize; ++y)
			{
				cave[y0 + y - yhsize][x0 + x - xhsize].feat =
					FEAT_WALL_EXTRA;
				cave[y0 + y - yhsize][x0 + x - xhsize].info &=
					~(CAVE_ICKY);
			}
		}
		return FALSE;
	}

	/* Do boundarys- set to normal granite */
	for (i = 0; i <= xsize; ++i)
	{
		cave[y0 + 0 - yhsize][x0 + i - xhsize].feat = FEAT_WALL_EXTRA;
		cave[y0 + ysize - yhsize][x0 + i - xhsize].feat = FEAT_WALL_EXTRA;

		/* clear the icky flag-don't need it any more */
		cave[y0 + 0 - yhsize][x0 + i - xhsize].info &= ~(CAVE_ICKY);
		cave[y0 + ysize - yhsize][x0 + i - xhsize].info &= ~(CAVE_ICKY);
	}

	/* Do the left and right boundaries minus the corners (done above) */

	for (i = 1; i < ysize; ++i)
	{
		cave[y0 + i - yhsize][x0 + 0 - xhsize].feat = FEAT_WALL_EXTRA;
		cave[y0 + i - yhsize][x0 + xsize - xhsize].feat = FEAT_WALL_EXTRA;

		/* clear icky flag -done with it */
		cave[y0 + i - yhsize][x0 + 0 - xhsize].info &= ~(CAVE_ICKY);
		cave[y0 + i - yhsize][x0 + xsize - xhsize].info &= ~(CAVE_ICKY);
	}


	/* Do the rest: convert back to the normal format */
	for (x = 1; x < xsize; ++x)
	{
		for (y = 1; y < ysize; ++y)
		{
			/* Fill unconnected regions with granite */
			if ((!(cave[y0 + y - yhsize][x0 + x -
							xhsize].info & CAVE_ICKY)) ||
				(cave[y0 + y - yhsize][x0 + x - xhsize].feat ==
					FEAT_WALL_OUTER))
				cave[y0 + y - yhsize][x0 + x - xhsize].feat =
					FEAT_WALL_EXTRA;

			/* turn off icky flag (no longer needed.) */
			cave[y0 + y - yhsize][x0 + x - xhsize].info &=
				~(CAVE_ICKY | CAVE_ROOM);

			/* Light lava and trees */
			if ((cave[y0 + y - yhsize][x0 + x - xhsize].feat ==
					FEAT_DEEP_LAVA) ||
				(cave[y0 + y - yhsize][x0 + x - xhsize].feat ==
					FEAT_SHAL_LAVA) ||
				(cave[y0 + y - yhsize][x0 + x - xhsize].feat ==
					FEAT_TREES))
			{
				cave[y0 + y - yhsize][x0 + x - xhsize].info |= CAVE_GLOW;
			}
		}
	}

	return TRUE;
}


/*
 * makes a lake/collapsed cave system in the center of the dungeon
 */
void build_lake(int type)
{
	int grd, roug, xsize, ysize, x0, y0;
	bool done = FALSE;
	int c1, c2, c3;

	/* paranoia - exit if lake type out of range. */
	if ((type < 1) || (type > 7))
	{
		msg_format("Invalid lake type (%d)", type);
		return;
	}

	/* Make the size of the dungeon */
	xsize = cur_wid - 1;
	ysize = cur_hgt - 1;
	x0 = xsize / 2;
	y0 = ysize / 2;

	/* Paranoia: make size even */
	xsize = x0 * 2;
	ysize = y0 * 2;

	while (!done)
	{
		/* testing values for these parameters: feel free to adjust */
		grd = randint(3) + 4;

		/* want average of about 16 */
		roug = randint(8) * randint(4);

		/* Make up size of various componants */
		/* Floor */
		c3 = 3 * xsize / 4;

		/* Deep water/lava */
		c1 = rand_int(c3 / 2) + rand_int(c3 / 2) - 5;

		/* Shallow boundary */
		c2 = (c1 + c3) / 2;

		/* make it */
		generate_hmap(y0 + 1, x0 + 1, xsize, ysize, grd, roug, c3);

		/* Convert to normal format+ clean up */
		done =
			generate_lake(y0 + 1, x0 + 1, xsize, ysize, c1, c2, c3, type);
	}
}
#endif /* ALLOW_CAVERNS_AND_LAKES */


/*
 * Routine used by the random vault creators to add a door to a location
 * Note that range checking has to be done in the calling routine.
 *
 * The doors must be INSIDE the allocated region.
 */
static void add_door(int x, int y)
{
	/* Need to have a wall in the center square */
	if (cave[y][x].feat != FEAT_WALL_OUTER)
		return;

	/* look at:
	 *  x#x
	 *  .#.
	 *  x#x
	 *
	 *  where x=don't care
	 *  .=floor, #=wall
	 */

	if ((cave[y - 1][x].feat == FEAT_FLOOR) &&
		(cave[y + 1][x].feat == FEAT_FLOOR) &&
		(cave[y][x - 1].feat == FEAT_WALL_OUTER) &&
		(cave[y][x + 1].feat == FEAT_WALL_OUTER))
	{
		/* secret door */
		place_secret_door(y, x);

		/* set boundarys so don't get wide doors */
		cave[y][x - 1].feat = FEAT_WALL_SOLID;
		cave[y][x + 1].feat = FEAT_WALL_SOLID;
	}


	/* look at:
	 *  x#x
	 *  .#.
	 *  x#x
	 *
	 *  where x = don't care
	 *  .=floor, #=wall
	 */
	if ((cave[y - 1][x].feat == FEAT_WALL_OUTER) &&
		(cave[y + 1][x].feat == FEAT_WALL_OUTER) &&
		(cave[y][x - 1].feat == FEAT_FLOOR) &&
		(cave[y][x + 1].feat == FEAT_FLOOR))
	{
		/* secret door */
		place_secret_door(y, x);

		/* set boundarys so don't get wide doors */
		cave[y - 1][x].feat = FEAT_WALL_SOLID;
		cave[y + 1][x].feat = FEAT_WALL_SOLID;
	}
}


/*
 * Routine that fills the empty areas of a room with treasure and monsters.
 */
static void fill_treasure(int x1, int x2, int y1, int y2, int difficulty)
{
	int x, y, cx, cy, size;
	s32b value;

	/* center of room: */
	cx = (x1 + x2) / 2;
	cy = (y1 + y2) / 2;

	/* Rough measure of size of vault= sum of lengths of sides */
	size = abs(x2 - x1) + abs(y2 - y1);

	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
		{
			/* Thing added based on distance to center of vault
			 * Difficulty is 1-easy to 10-hard */
			value =
				((((s32b) (distance(cx, cy, x,
							y))) * 100) / size) + randint(10) - difficulty;

			/* hack- empty square part of the time */
			if ((randint(100) - difficulty * 3) > 50)
				value = 20;

			/* if floor, shallow water and lava */
			if ((cave[y][x].feat == FEAT_FLOOR) ||
				(cave[y][x].feat == FEAT_SHAL_WATER) ||
				(cave[y][x].feat == FEAT_SHAL_LAVA))
			{
				/* The smaller 'value' is, the better the stuff */
				if (value < 0)
				{
					/* Meanest monster + treasure */
					monster_level = base_level + 40;
					place_monster(y, x, TRUE, TRUE);
					monster_level = base_level;
					object_level = base_level + 20;
					place_object(y, x, TRUE, FALSE);
					object_level = base_level;
				}
				else if (value < 5)
				{
					/* Mean monster +treasure */
					monster_level = base_level + 20;
					place_monster(y, x, TRUE, TRUE);
					monster_level = base_level;
					object_level = base_level + 10;
					place_object(y, x, TRUE, FALSE);
					object_level = base_level;
				}
				else if (value < 10)
				{
					/* Monster */
					monster_level = base_level + 9;
					place_monster(y, x, TRUE, TRUE);
					monster_level = base_level;
				}
				else if (value < 17)
				{
					/* Intentional Blank space */

					/*(Want some of the vault to be empty
					 * * so have room for group monsters
					 * * +this is used in the hack above to lower
					 * * the density of stuff in the vault) */
				}
				else if (value < 23)
				{
					/* Object or trap */
					if (rand_int(100) < 25)
					{
						place_object(y, x, FALSE, FALSE);
					}
					else
					{
						place_trap(y, x);
					}
				}
				else if (value < 30)
				{
					/* Monster and trap */
					monster_level = base_level + 5;
					place_monster(y, x, TRUE, TRUE);
					monster_level = base_level;
					place_trap(y, x);
				}
				else if (value < 40)
				{
					/* Monster or object */
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
				}
				else if (value < 50)
				{
					/* Trap */
					place_trap(y, x);
				}
				else
				{
					/* Various Stuff */

					/* 20% monster, 40% trap, 20% object, 20% blank space */
					if (rand_int(100) < 20)
					{
						place_monster(y, x, TRUE, TRUE);
					}
					else if (rand_int(100) < 50)
					{
						place_trap(y, x);
					}
					else if (rand_int(100) < 50)
					{
						place_object(y, x, FALSE, FALSE);
					}
				}

			}
		}
	}
}


/*
 * This function creates a random vault that looks like a collection of bubbles.
 * It works by getting a set of coordinates that represent the center of each
 * bubble.  The entire room is made by seeing which bubble center is closest. If
 * two centers are equidistant then the square is a wall, otherwise it is a floor.
 * The only exception is for squares really near a center, these are always floor.
 * (It looks better than without this check.)
 *
 * Note: If two centers are on the same point then this algorithm will create a
 *       blank bubble filled with walls. - This is prevented from happening.
 */
static void build_bubble_vault(int x0, int y0, int xsize, int ysize)
{
#define BUBBLENUM 10 /* number of bubbles */

	/* array of center points of bubbles */
	coord center[BUBBLENUM];

	int i, x, y;
	u16b min1, min2, temp;

	/* Offset from center to top left hand corner */
	int xhsize = xsize / 2;
	int yhsize = ysize / 2;


	if (cheat_room)
		msg_print("Bubble Vault");

	/* Allocate center of bubbles */
	for (i = 0; i < BUBBLENUM; i++)
	{
		/* get center and (Uniqueness check removed) */

		center[i].x = randint(xsize - 3) + 1;
		center[i].y = randint(ysize - 3) + 1;
	}


	/* Top and bottom boundaries */
	for (i = 0; i <= xsize; i++)
	{
		int x = x0 - xhsize + i;

		cave[y0 - yhsize + 0][x].feat = FEAT_WALL_OUTER;
		cave[y0 - yhsize + 0][x].info |= (CAVE_ROOM | CAVE_ICKY);
		cave[y0 - yhsize + ysize][x].feat = FEAT_WALL_OUTER;
		cave[y0 - yhsize + ysize][x].info |= (CAVE_ROOM | CAVE_ICKY);
	}

	/* Left and right boundaries */
	for (i = 1; i < ysize; i++)
	{
		int y = y0 - yhsize + i;

		cave[y][x0 - xhsize + 0].feat = FEAT_WALL_OUTER;
		cave[y][x0 - xhsize + 0].info |= (CAVE_ROOM | CAVE_ICKY);
		cave[y][x0 - xhsize + xsize].feat = FEAT_WALL_OUTER;
		cave[y][x0 - xhsize + xsize].info |= (CAVE_ROOM | CAVE_ICKY);
	}

	/* Fill in middle with bubbles */
	for (x = 1; x < xsize; x++)
	{
		for (y = 1; y < ysize; y++)
		{
			/* Get distances to two closest centers */

			/* initialize */
			min1 = distance(x, y, center[0].x, center[0].y);
			min2 = distance(x, y, center[1].x, center[1].y);

			if (min1 > min2)
			{
				/* swap if in wrong order */
				temp = min1;
				min1 = min2;
				min2 = temp;
			}

			/* Scan the rest */
			for (i = 2; i < BUBBLENUM; i++)
			{
				temp = distance(x, y, center[i].x, center[i].y);

				if (temp < min1)
				{
					/* smallest */
					min2 = min1;
					min1 = temp;
				}
				else if (temp < min2)
				{
					/* second smallest */
					min2 = temp;
				}
			}
			if (((min2 - min1) <= 2) && (!(min1 < 3)))
			{
				/* Boundary at midpoint+ not at inner region of bubble */
				cave[y0 - yhsize + y][x0 - xhsize + x].feat =
					FEAT_WALL_OUTER;
			}
			else
			{
				/* middle of a bubble */
				cave[y0 - yhsize + y][x0 - xhsize + x].feat = FEAT_FLOOR;
			}

			/* clean up rest of flags */
			cave[y0 - yhsize + y][x0 - xhsize + x].info |=
				(CAVE_ROOM | CAVE_ICKY);
		}
	}

	/* Try to add some random doors */
	for (i = 0; i < 500; i++)
	{
		x = randint(xsize - 3) - xhsize + x0 + 1;
		y = randint(ysize - 3) - yhsize + y0 + 1;
		add_door(x, y);
	}

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x0 - xhsize + 1, x0 - xhsize + xsize - 2,
		y0 - yhsize + 1, y0 - yhsize + ysize - 2, randint(5));
}


/*
 * Overlay a rectangular room given its bounds
 * This routine is used by build_room_vault
 * The area inside the walls is not touched:
 * only granite is removed- normal walls stay
 */
static void build_room(int x1, int x2, int y1, int y2)
{
	int x, y, i, xsize, ysize, temp;

	/* Check if rectangle has no width */
	if ((x1 == x2) || (y1 == y2))
		return;

	/* initialize */
	if (x1 > x2)
	{
		/* Swap boundaries if in wrong order */
		temp = x1;
		x1 = x2;
		x2 = temp;
	}

	if (y1 > y2)
	{
		/* Swap boundaries if in wrong order */
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	/* get total widths */
	xsize = x2 - x1;
	ysize = y2 - y1;


	/* Top and bottom boundaries */
	for (i = 0; i <= xsize; i++)
	{
		cave[y1][x1 + i].feat = FEAT_WALL_OUTER;
		cave[y1][x1 + i].info |= (CAVE_ROOM | CAVE_ICKY);
		cave[y2][x1 + i].feat = FEAT_WALL_OUTER;
		cave[y2][x1 + i].info |= (CAVE_ROOM | CAVE_ICKY);
	}

	/* Left and right boundaries */
	for (i = 1; i < ysize; i++)
	{
		cave[y1 + i][x1].feat = FEAT_WALL_OUTER;
		cave[y1 + i][x1].info |= (CAVE_ROOM | CAVE_ICKY);
		cave[y1 + i][x2].feat = FEAT_WALL_OUTER;
		cave[y1 + i][x2].info |= (CAVE_ROOM | CAVE_ICKY);
	}

	/* Middle */
	for (x = 1; x < xsize; x++)
	{
		for (y = 1; y < ysize; y++)
		{
			if (cave[y1 + y][x1 + x].feat == FEAT_WALL_EXTRA)
			{
				/* clear the untouched region */
				cave[y1 + y][x1 + x].feat = FEAT_FLOOR;
				cave[y1 + y][x1 + x].info |= (CAVE_ROOM | CAVE_ICKY);
			}
			else
			{
				/* make it a room- but don't touch */
				cave[y1 + y][x1 + x].info |= (CAVE_ROOM | CAVE_ICKY);
			}
		}
	}
}


/* Create a random vault that looks like a collection of overlapping rooms */

static void build_room_vault(int x0, int y0, int xsize, int ysize)
{
	int i, x1, x2, y1, y2, xhsize, yhsize;

	/* get offset from center */
	xhsize = xsize / 2;
	yhsize = ysize / 2;

	if (cheat_room)
		msg_print("Room Vault");

	/* fill area so don't get problems with arena levels */
	for (x1 = 0; x1 < xsize; x1++)
	{
		int x = x0 - xhsize + x1;

		for (y1 = 0; y1 < ysize; y1++)
		{
			int y = y0 - yhsize + y1;

			cave[y][x].feat = FEAT_WALL_EXTRA;
			cave[y][x].info &= (~CAVE_ICKY);
		}
	}

	/* add ten random rooms */
	for (i = 0; i < 10; i++)
	{
		x1 = randint(xhsize) * 2 + x0 - xhsize;
		x2 = randint(xhsize) * 2 + x0 - xhsize;
		y1 = randint(yhsize) * 2 + y0 - yhsize;
		y2 = randint(yhsize) * 2 + y0 - yhsize;
		build_room(x1, x2, y1, y2);
	}

	/* Add some random doors */
	for (i = 0; i < 500; i++)
	{
		x1 = randint(xsize - 2) - xhsize + x0 + 1;
		y1 = randint(ysize - 2) - yhsize + y0 + 1;
		add_door(x1, y1);
	}

	/* Fill with monsters and treasure, high difficulty */
	fill_treasure(x0 - xhsize + 1, x0 - xhsize + xsize - 1,
		y0 - yhsize + 1, y0 - yhsize + ysize - 1, randint(5) + 5);
}


/* Create a random vault out of a fractal cave */
static void build_cave_vault(int x0, int y0, int xsiz, int ysiz)
{
	int grd, roug, cutoff, xhsize, yhsize, xsize, ysize, x, y;
	bool done, light, room;

	/* round to make sizes even */
	xhsize = (xsiz - 1) / 2;
	yhsize = (ysiz - 1) / 2;
	xsize = xhsize * 2;
	ysize = yhsize * 2;

	if (cheat_room)
		msg_print("Cave Vault");

	light = done = FALSE;
	room = TRUE;

	while (!done)
	{
		/* testing values for these parameters feel free to adjust */
		grd = 1 << rand_int(4);

		/* want average of about 16 */
		roug = randint(8) * randint(4);

		/* about size/2 */
		cutoff =
			randint(xsize / 4) + randint(ysize / 4) + randint(xsize / 4) +
			randint(ysize / 4);

		/* make it */
		generate_hmap(y0, x0, xsize, ysize, grd, roug, cutoff);

		/* Convert to normal format+ clean up */
		done = generate_fracave(y0, x0, xsize, ysize, cutoff, light, room);
	}

	/* Set icky flag because is a vault */
	for (x = 0; x <= xsize; x++)
	{
		for (y = 0; y <= ysize; y++)
		{
			cave[y0 - yhsize + y][x0 - xhsize + x].info |= CAVE_ICKY;
		}
	}

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x0 - xhsize + 1, x0 - xhsize + xsize - 1,
		y0 - yhsize + 1, y0 - yhsize + ysize - 1, randint(5));
}

/*
 * maze vault -- rectangular labyrinthine rooms
 *
 * maze vault uses two routines:
 *    r_visit - a recursive routine that builds the labyrinth
 *    build_maze_vault - a driver routine that calls r_visit and adds
 *                   monsters, traps and treasure
 *
 * The labyrinth is built by creating a spanning tree of a graph.
 * The graph vertices are at
 *    (x, y) = (2j + x1, 2k + y1)   j = 0,...,m-1    k = 0,...,n-1
 * and the edges are the vertical and horizontal nearest neighbors.
 *
 * The spanning tree is created by performing a suitably randomized
 * depth-first traversal of the graph. The only adjustable parameter
 * is the rand_int(3) below; it governs the relative density of
 * twists and turns in the labyrinth: smaller number, more twists.
 */
static void r_visit(int y1, int x1, int y2, int x2, int node, int dir,
	int *visited)
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
			case 0:
				/* (0,+) - check for bottom boundary */
				if ((node / m < n - 1) && (visited[node + m] == 0))
				{
					cave_set_feat(y + 1, x, FEAT_FLOOR);
					r_visit(y1, x1, y2, x2, node + m, dir, visited);
				}
				break;
			case 1:
				/* (0,-) - check for top boundary */
				if ((node / m > 0) && (visited[node - m] == 0))
				{
					cave_set_feat(y - 1, x, FEAT_FLOOR);
					r_visit(y1, x1, y2, x2, node - m, dir, visited);
				}
				break;
			case 2:
				/* (+,0) - check for right boundary */
				if ((node % m < m - 1) && (visited[node + 1] == 0))
				{
					cave_set_feat(y, x + 1, FEAT_FLOOR);
					r_visit(y1, x1, y2, x2, node + 1, dir, visited);
				}
				break;
			case 3:
				/* (-,0) - check for left boundary */
				if ((node % m > 0) && (visited[node - 1] == 0))
				{
					cave_set_feat(y, x - 1, FEAT_FLOOR);
					r_visit(y1, x1, y2, x2, node - 1, dir, visited);
				}
		} /* end switch */
	}
}


static void build_maze_vault(int x0, int y0, int xsize, int ysize)
{
	int y, x, dy, dx;
	int y1, x1, y2, x2;
	int i, m, n, num_vertices, *visited;
	bool light;
	cave_type *c_ptr;


	if (cheat_room)
		msg_print("Maze Vault");

	/* Choose lite or dark */
	light = (dun_level <= randint(25));

	/* Pick a random room size- randomized by calling routine */
	dy = ysize / 2 - 1;
	dx = xsize / 2 - 1;

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
			c_ptr->info |= (CAVE_ROOM | CAVE_ICKY);
			if ((x == x1 - 1) || (x == x2 + 1) || (y == y1 - 1) ||
				(y == y2 + 1))
				c_ptr->feat = FEAT_WALL_OUTER;
			else
				c_ptr->feat = FEAT_WALL_INNER;
			if (light)
				c_ptr->info |= (CAVE_GLOW);
		}
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

	/* traverse the graph to create a spaning tree, pick a random root */
	r_visit(y1, x1, y2, x2, rand_int(num_vertices), 0, visited);

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x1, x2, y1, y2, randint(5));

	free(visited);
}


/* Build a "mini" checkerboard vault
 *
 * This is done by making a permanent wall maze and setting
 * the diagonal sqaures of the checker board to be granite.
 * The vault has two entrances on opposite sides to guarantee
 * a way to get in even if the vault abuts a side of the dungeon.
 */
static void build_mini_c_vault(int x0, int y0, int xsize, int ysize)
{
	int dy, dx;
	int y1, x1, y2, x2, y, x, total;
	int i, m, n, num_vertices;
	int *visited;

	if (cheat_room)
		msg_print("Mini Checker Board Vault");

	/* Pick a random room size */
	dy = ysize / 2 - 1;
	dx = xsize / 2 - 1;

	y1 = y0 - dy;
	x1 = x0 - dx;
	y2 = y0 + dy;
	x2 = x0 + dx;


	/* generate the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			cave[y][x].info |= (CAVE_ROOM | CAVE_ICKY);

			/* Permanent walls */
			cave[y][x].feat = FEAT_PERM_INNER;
		}
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

	/* Make it look like a checker board vault */
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
		{
			total = x - x1 + y - y1;
			/* If total is odd- and is a floor then make a wall */
			if ((total % 2 == 1) && (cave[y][x].feat == FEAT_FLOOR))
			{
				cave[y][x].feat = FEAT_WALL_INNER;
			}
		}
	}

	/* Make a couple of entrances */
	if (randint(2) == 1)
	{
		/* left and right */
		y = randint(dy) + dy / 2;
		cave[y1 + y][x1 - 1].feat = FEAT_WALL_OUTER;
		cave[y1 + y][x2 + 1].feat = FEAT_WALL_OUTER;
	}
	else
	{
		/* top and bottom */
		x = randint(dx) + dx / 2;
		cave[y1 - 1][x1 + x].feat = FEAT_WALL_OUTER;
		cave[y2 + 1][x1 + x].feat = FEAT_WALL_OUTER;
	}

	/* Fill with monsters and treasure, highest difficulty */
	fill_treasure(x1, x2, y1, y2, 10);

	/* rnfree(visited, num_vertices * sizeof(int)); */
	free(visited);
}


/* Build a town/ castle by using a recursive algorithm.
 * Basically divide each region in a probalistic way to create
 * smaller regions.  When the regions get too small stop.
 *
 * The power variable is a measure of how well defended a region is.
 * This alters the possible choices.
 */
static void build_recursive_room(int x1, int y1, int x2, int y2, int power)
{
	int xsize, ysize;
	int x, y;
	int choice;

	/* Temp variables */
	int t1, t2, t3, t4;

	xsize = x2 - x1;
	ysize = y2 - y1;

	if ((power < 3) && (xsize > 12) && (ysize > 12))
	{
		/* Need outside wall +keep */
		choice = 1;
	}
	else
	{
		if (power < 10)
		{
			/* Make rooms + subdivide */
			if ((randint(10) > 2) && (xsize < 8) && (ysize < 8))
			{
				choice = 4;
			}
			else
			{
				choice = randint(2) + 1;
			}
		}
		else
		{
			/* Mostly subdivide */
			choice = randint(3) + 1;
		}
	}

	/* Based on the choice made above, do something */

	switch (choice)
	{
		case 1:
		{
			/* Outer walls */

			/* top and bottom */
			for (x = x1; x <= x2; x++)
			{
				cave[y1][x].feat = FEAT_WALL_OUTER;
				cave[y2][x].feat = FEAT_WALL_OUTER;
			}

			/* left and right */
			for (y = y1 + 1; y < y2; y++)
			{
				cave[y][x1].feat = FEAT_WALL_OUTER;
				cave[y][x2].feat = FEAT_WALL_OUTER;
			}

			/* Make a couple of entrances */
			if (randint(2) == 1)
			{
				/* left and right */
				y = randint(ysize) + y1;
				cave[y][x1].feat = FEAT_FLOOR;
				cave[y][x2].feat = FEAT_FLOOR;
			}
			else
			{
				/* top and bottom */
				x = randint(xsize) + x1;
				cave[y1][x].feat = FEAT_FLOOR;
				cave[y2][x].feat = FEAT_FLOOR;
			}

			/* Select size of keep */
			t1 = randint(ysize / 3) + y1;
			t2 = y2 - randint(ysize / 3);
			t3 = randint(xsize / 3) + x1;
			t4 = x2 - randint(xsize / 3);

			/* Do outside areas */

			/* Above and below keep */
			build_recursive_room(x1 + 1, y1 + 1, x2 - 1, t1, power + 1);
			build_recursive_room(x1 + 1, t2, x2 - 1, y2, power + 1);

			/* Left and right of keep */
			build_recursive_room(x1 + 1, t1 + 1, t3, t2 - 1, power + 3);
			build_recursive_room(t4, t1 + 1, x2 - 1, t2 - 1, power + 3);

			/* Make the keep itself: */
			x1 = t3;
			x2 = t4;
			y1 = t1;
			y2 = t2;
			xsize = x2 - x1;
			ysize = y2 - y1;
			power += 2;

			/* Fall through */
		}
		case 4:
		{
			/* Try to build a room */
			if ((xsize < 3) || (ysize < 3))
			{
				for (y = y1; y < y2; y++)
				{
					for (x = x1; x < x2; x++)
					{
						cave[y][x].feat = FEAT_WALL_INNER;
					}
				}

				/* Too small */
				return;
			}

			/* Make outside walls */
			/* top and bottom */
			for (x = x1 + 1; x <= x2 - 1; x++)
			{
				cave[y1 + 1][x].feat = FEAT_WALL_INNER;
				cave[y2 - 1][x].feat = FEAT_WALL_INNER;
			}

			/* left and right */
			for (y = y1 + 1; y <= y2 - 1; y++)
			{
				cave[y][x1 + 1].feat = FEAT_WALL_INNER;
				cave[y][x2 - 1].feat = FEAT_WALL_INNER;
			}

			/* Make a door */
			y = randint(ysize - 3) + y1 + 1;

			if (randint(2) == 1)
			{
				/* left  */
				cave[y][x1 + 1].feat = FEAT_FLOOR;
			}
			else
			{
				/* right */
				cave[y][x2 - 1].feat = FEAT_FLOOR;
			}

			/* Build the room */
			build_recursive_room(x1 + 2, y1 + 2, x2 - 2, y2 - 2,
				power + 3);
			break;
		}
		case 2:
		{
			/* Try and divide vertically */

			if (xsize < 3)
			{
				/* Too small */
				for (y = y1; y < y2; y++)
				{
					for (x = x1; x < x2; x++)
					{
						cave[y][x].feat = FEAT_WALL_INNER;
					}
				}
				return;
			}

			t1 = randint(xsize - 2) + x1 + 1;
			build_recursive_room(x1, y1, t1, y2, power - 2);
			build_recursive_room(t1 + 1, y1, x2, y2, power - 2);
			break;
		}
		case 3:
		{
			/* Try and divide horizontally */
			if (ysize < 3)
			{
				/* Too small */
				for (y = y1; y < y2; y++)
				{
					for (x = x1; x < x2; x++)
					{
						cave[y][x].feat = FEAT_WALL_INNER;
					}
				}
				return;
			}

			t1 = randint(ysize - 2) + y1 + 1;
			build_recursive_room(x1, y1, x2, t1, power - 2);
			build_recursive_room(x1, t1 + 1, x2, y2, power - 2);
			break;
		}
	}
}


/* Build a castle */

/* Driver routine: clear the region and call the recursive
* room routine.
*
*This makes a vault that looks like a castle/ city in the dungeon.
*/
static void build_castle_vault(int x0, int y0, int xsize, int ysize)
{
	int dy, dx;
	int y1, x1, y2, x2;
	int y, x;

	/* Pick a random room size */
	dy = ysize / 2 - 1;
	dx = xsize / 2 - 1;

	y1 = y0 - dy;
	x1 = x0 - dx;
	y2 = y0 + dy;
	x2 = x0 + dx;

	if (cheat_room)
		msg_print("Castle Vault");

	/* generate the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			cave[y][x].info |= (CAVE_ROOM | CAVE_ICKY);
			/* Make everything a floor */
			cave[y][x].feat = FEAT_FLOOR;
		}
	}

	/* Make the castle */
	build_recursive_room(x1, y1, x2, y2, randint(5));

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x1, x2, y1, y2, randint(3));
}


/*
 * Add outer wall to a floored region
 * Note: no range checking is done so must be inside dungeon
 * This routine also stomps on doors
 */
static void add_outer_wall(int x, int y, int light, int x1, int y1, int x2,
	int y2)
{
	int i, j;

	if (!in_bounds(y, x))
		return;

	/* hack- check to see if square has been visited before
	 * * if so, then exit (use room flag to do this) */
	if (cave[y][x].info & CAVE_ROOM)
		return;

	/* set room flag */
	cave[y][x].info |= CAVE_ROOM;

	if (cave[y][x].feat == FEAT_FLOOR)
	{
		for (i = -1; i <= 1; i++)
		{
			for (j = -1; j <= 1; j++)
			{
				if ((x + i >= x1) && (x + i <= x2) && (y + j >= y1) &&
					(y + j <= y2))
				{
					add_outer_wall(x + i, y + j, light, x1, y1, x2, y2);
					if (light)
						cave[y][x].info |= CAVE_GLOW;
				}
			}
		}
	}
	else if (cave[y][x].feat == FEAT_WALL_EXTRA)
	{
		/* Set bounding walls */
		cave[y][x].feat = FEAT_WALL_OUTER;
		if (light == TRUE)
			cave[y][x].info |= CAVE_GLOW;
	}
	else if (cave[y][x].feat == FEAT_PERM_OUTER)
	{
		/* Set bounding walls */
		if (light == TRUE)
			cave[y][x].info |= CAVE_GLOW;
	}
}


/*
 * Hacked distance formula - gives the 'wrong' answer.
 * Used to build crypts
 */
static int dist2(int x1, int y1, int x2, int y2, int h1, int h2, int h3,
	int h4)
{
	int dx, dy;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	/* Basically this works by taking the normal pythagorean formula
	 * and using an expansion to express this in a way without the
	 * square root.  This approximate formula is then perturbed to give
	 * the distorted results.  (I found this by making a mistake when I was
	 * trying to fix the circular rooms.)
	 */

	/* h1-h4 are constants that describe the metric */
	if (dx >= 2 * dy)
		return (dx + (dy * h1) / h2);
	if (dy >= 2 * dx)
		return (dy + (dx * h1) / h2);
	return (((dx + dy) * 128) / 181 + (dx * dx / (dy * h3) +
			dy * dy / (dx * h3)) * h4);
	/* 128/181 is approx. 1/sqrt(2) */
}


/*
 * Build target vault.
 * This is made by two concentric "crypts" with perpendicular
 * walls creating the cross-hairs.
 */
static void build_target_vault(int x0, int y0, int xsize, int ysize)
{
	int rad, x, y;

	/* Make a random metric */
	int h1, h2, h3, h4;
	h1 = randint(32) - 16;
	h2 = randint(16);
	h3 = randint(32);
	h4 = randint(32) - 16;

	if (cheat_room)
		msg_print("Target Vault");

	/* work out outer radius */
	if (xsize > ysize)
	{
		rad = ysize / 2;
	}
	else
	{
		rad = xsize / 2;
	}

	/* Make floor */
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		for (y = y0 - rad; y <= y0 + rad; y++)
		{
			/* clear room flag */
			cave[y][x].info &= ~(CAVE_ROOM);

			/* Vault - so is "icky" */
			cave[y][x].info |= CAVE_ICKY;

			if (dist2(y0, x0, y, x, h1, h2, h3, h4) <= rad - 1)
			{
				/* inside- so is floor */
				cave[y][x].feat = FEAT_FLOOR;
			}
			else
			{
				/* make granite outside so arena works */
				cave[y][x].feat = FEAT_WALL_EXTRA;
			}

			/* proper boundary for arena */
			if (((y + rad) == y0) || ((y - rad) == y0) || ((x + rad) == x0)
				|| ((x - rad) == x0))
			{
				cave[y][x].feat = FEAT_WALL_EXTRA;
			}
		}
	}

	/* Find visible outer walls and set to be FEAT_OUTER */
	add_outer_wall(x0, y0, FALSE, x0 - rad - 1, y0 - rad - 1, x0 + rad + 1,
		y0 + rad + 1);

	/* Add inner wall */
	for (x = x0 - rad / 2; x <= x0 + rad / 2; x++)
	{
		for (y = y0 - rad / 2; y <= y0 + rad / 2; y++)
		{
			if (dist2(y0, x0, y, x, h1, h2, h3, h4) == rad / 2)
			{
				/* Make an internal wall */
				cave[y][x].feat = FEAT_WALL_INNER;
			}
		}
	}

	/* Add perpendicular walls */
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		cave[y0][x].feat = FEAT_WALL_INNER;
	}

	for (y = y0 - rad; y <= y0 + rad; y++)
	{
		cave[y][x0].feat = FEAT_WALL_INNER;
	}

	/* Make inner vault */
	for (y = y0 - 1; y <= y0 + 1; y++)
	{
		cave[y][x0 - 1].feat = FEAT_WALL_INNER;
		cave[y][x0 + 1].feat = FEAT_WALL_INNER;
	}
	for (x = x0 - 1; x <= x0 + 1; x++)
	{
		cave[y0 - 1][x].feat = FEAT_WALL_INNER;
		cave[y0 + 1][x].feat = FEAT_WALL_INNER;
	}

	cave[y0][x0].feat = FEAT_FLOOR;


	/* Add doors to vault */
	/* get two distances so can place doors relative to centre */
	x = (rad - 2) / 4 + 1;
	y = rad / 2 + x;

	add_door(x0 + x, y0);
	add_door(x0 + y, y0);
	add_door(x0 - x, y0);
	add_door(x0 - y, y0);
	add_door(x0, y0 + x);
	add_door(x0, y0 + y);
	add_door(x0, y0 - x);
	add_door(x0, y0 - y);

	/* Fill with stuff - medium difficulty */
	fill_treasure(x0 - rad, x0 + rad, y0 - rad, y0 + rad, randint(3) + 3);
}


#ifdef ALLOW_CAVERNS_AND_LAKES
/*
 * This routine uses a modified version of the lake code to make a
 * distribution of some terrain type over the vault.  This type
 * depends on the dungeon depth.
 *
 * Miniture rooms are then scattered across the vault.
 */
static void build_elemental_vault(int x0, int y0, int xsiz, int ysiz)
{
	int grd, roug;
	int c1, c2, c3;
	bool done = FALSE;
	int xsize, ysize, xhsize, yhsize, x, y, i;
	int type;


	if (cheat_room)
		msg_print("Elemental Vault");

	/* round to make sizes even */
	xhsize = xsiz / 2;
	yhsize = ysiz / 2;
	xsize = xhsize * 2;
	ysize = yhsize * 2;

	if (dun_level < 25)
	{
		/* Earth vault  (Rubble) */
		type = 4;
	}
	else if (dun_level < 50)
	{
		/* Air vault (Trees) */
		type = 5;
	}
	else if (dun_level < 75)
	{
		/* Water vault (shallow water) */
		type = 6;
	}
	else
	{
		/* Fire vault (shallow lava) */
		type = 7;
	}

	while (!done)
	{
		/* testing values for these parameters: feel free to adjust */
		grd = 1 << (rand_int(3));

		/* want average of about 16 */
		roug = randint(8) * randint(4);

		/* Make up size of various componants */
		/* Floor */
		c3 = 2 * xsize / 3;

		/* Deep water/lava */
		c1 = rand_int(c3 / 2) + rand_int(c3 / 2) - 5;

		/* Shallow boundary */
		c2 = (c1 + c3) / 2;

		/* make it */
		generate_hmap(y0, x0, xsize, ysize, grd, roug, c3);

		/* Convert to normal format+ clean up */
		done = generate_lake(y0, x0, xsize, ysize, c1, c2, c3, type);
	}

	/* Set icky flag because is a vault */
	for (x = 0; x <= xsize; x++)
	{
		for (y = 0; y <= ysize; y++)
		{
			cave[y0 - yhsize + y][x0 - xhsize + x].info |= CAVE_ICKY;
		}
	}

	/* make a few rooms in the vault */
	for (i = 1; i <= (xsize * ysize) / 50; i++)
	{
		build_small_room(x0 + rand_int(xsize - 4) - xsize / 2 + 2,
			y0 + rand_int(ysize - 4) - ysize / 2 + 2);
	}

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x0 - xhsize + 1, x0 - xhsize + xsize - 1,
		y0 - yhsize + 1, y0 - yhsize + ysize - 1, randint(5));
}
#endif /* ALLOW_CAVERNS_AND_LAKES */


/*
 * Random vaults
 */
static void build_type10(int by0, int bx0)
{
	int y0, x0, xsize, ysize, vtype;

	/* Get size */
	/* big enough to look good, small enough to be fairly common. */
	xsize = randint(22) + 22;
	ysize = randint(11) + 11;

	/* Allocate in room_map.  If will not fit, exit */
	if (!room_alloc(xsize + 1, ysize + 1, FALSE, by0, bx0, &x0, &y0))
		return;

	/* Boost the rating- higher than lesser vaults and lower than greater vaults */
	rating += 10;

	/* (Sometimes) Cause a special feeling */
	if ((dun_level <= 50) ||
		(randint((dun_level - 40) * (dun_level - 40) + 1) < 400))
	{
		good_item_flag = TRUE;
	}

	/* Select type of vault */
#ifdef ALLOW_CAVERNS_AND_LAKES
	vtype = randint(8);
#else /* ALLOW_CAVERNS_AND_LAKES */
	vtype = randint(7);
#endif /* ALLOW_CAVERNS_AND_LAKES */

	switch (vtype)
	{
			/* Build an appropriate room */
		case 1:
			build_bubble_vault(x0, y0, xsize, ysize);
			break;
		case 2:
			build_room_vault(x0, y0, xsize, ysize);
			break;
		case 3:
			build_cave_vault(x0, y0, xsize, ysize);
			break;
		case 4:
			build_maze_vault(x0, y0, xsize, ysize);
			break;
		case 5:
			build_mini_c_vault(x0, y0, xsize, ysize);
			break;
		case 6:
			build_castle_vault(x0, y0, xsize, ysize);
			break;
		case 7:
			build_target_vault(x0, y0, xsize, ysize);
			break;
#ifdef ALLOW_CAVERNS_AND_LAKES
		case 8:
			build_elemental_vault(x0, y0, xsize, ysize);
			break;
#endif /* ALLOW_CAVERNS_AND_LAKES */
			/* I know how to add a few more... give me some time. */

			/* Paranoia */
		default:
			return;
	}
}


/*
 * Build an vertical oval room.
 * For every grid in the possible square, check the distance.
 * If it's less than the radius, make it a room square.
 *
 * When done fill from the inside to find the walls,
 */
static void build_type11(int by0, int bx0)
{
	int rad, x, y, x0, y0;
	int light = FALSE;

	/* Occasional light */
	if (randint(dun_level) <= 15)
		light = TRUE;

	rad = rand_int(9);

	/* Allocate in room_map.  If will not fit, exit */
	if (!room_alloc(rad * 2 + 1, rad * 2 + 1, FALSE, by0, bx0, &x0, &y0))
		return;

	/* Make circular floor */
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		for (y = y0 - rad; y <= y0 + rad; y++)
		{
			if (distance(y0, x0, y, x) <= rad - 1)
			{
				/* inside- so is floor */
				cave[y][x].feat = FEAT_FLOOR;
			}
			else if (distance(y0, x0, y, x) <= rad + 1)
			{
				/* make granite outside so arena works */
				cave[y][x].feat = FEAT_WALL_EXTRA;
			}
		}
	}

	/* Find visible outer walls and set to be FEAT_OUTER */
	add_outer_wall(x0, y0, light, x0 - rad, y0 - rad, x0 + rad, y0 + rad);
}


/*
 * Build crypt room.
 * For every grid in the possible square, check the (fake) distance.
 * If it's less than the radius, make it a room square.
 *
 * When done fill from the inside to find the walls,
 */
static void build_type12(int by0, int bx0)
{
	int rad, x, y, x0, y0;
	int light = FALSE;
	bool emptyflag = TRUE;

	/* Make a random metric */
	int h1, h2, h3, h4;
	h1 = randint(32) - 16;
	h2 = randint(16);
	h3 = randint(32);
	h4 = randint(32) - 16;

	/* Occasional light */
	if (randint(dun_level) <= 5)
		light = TRUE;

	rad = randint(9);

	/* Allocate in room_map.  If will not fit, exit */
	if (!room_alloc(rad * 2 + 3, rad * 2 + 3, FALSE, by0, bx0, &x0, &y0))
		return;

	/* Make floor */
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		for (y = y0 - rad; y <= y0 + rad; y++)
		{
			/* clear room flag */
			cave[y][x].info &= ~(CAVE_ROOM);

			if (dist2(y0, x0, y, x, h1, h2, h3, h4) <= rad - 1)
			{
				/* inside- so is floor */
				cave[y][x].feat = FEAT_FLOOR;
			}
			else if (distance(y0, x0, y, x) < 3)
			{
				cave[y][x].feat = FEAT_FLOOR;
			}
			else
			{
				/* make granite outside so arena works */
				cave[y][x].feat = FEAT_WALL_EXTRA;
			}

			/* proper boundary for arena */
			if (((y + rad) == y0) || ((y - rad) == y0) || ((x + rad) == x0)
				|| ((x - rad) == x0))
			{
				cave[y][x].feat = FEAT_WALL_EXTRA;
			}
		}
	}

	/* Find visible outer walls and set to be FEAT_OUTER */
	add_outer_wall(x0, y0, light, x0 - rad - 1, y0 - rad - 1, x0 + rad + 1,
		y0 + rad + 1);

	/* Check to see if there is room for an inner vault */
	for (x = x0 - 2; x <= x0 + 2; x++)
	{
		for (y = y0 - 2; y <= y0 + 2; y++)
		{
			if (cave[y][x].feat != FEAT_FLOOR)
			{
				/* Wall in the way */
				emptyflag = FALSE;
			}
		}
	}

	if (emptyflag && (randint(2) == 1))
	{
		/* Build the vault */
		build_small_room(x0, y0);

		/* Place a treasure in the vault */
		place_object(y0, x0, FALSE, FALSE);

		/* Let's guard the treasure well */
		vault_monsters(y0, x0, rand_int(2) + 3);

		/* Traps naturally */
		vault_traps(y0, x0, 4, 4, rand_int(3) + 2);
	}
}


/*
 * Attempt to build a room of the given type at the given block
 *
 * Note that we restrict the number of "crowded" rooms to reduce
 * the chance of overflowing the monster list during level creation.
 */
bool room_build(int by0, int bx0, int typ)
{
	/* Restrict level */
	if ((dun_level < roomdep[typ]) && !ironman_rooms)
		return (FALSE);

	/* Restrict "crowded" rooms */
	if ((dun->crowded >= 2) && ((typ == 5) || (typ == 6)))
		return (FALSE);

	/* Build a room */
	switch (typ)
	{
			/* Build an appropriate room */
		case 12:
			build_type12(by0, bx0);
			break;
		case 11:
			build_type11(by0, bx0);
			break;
		case 10:
			build_type10(by0, bx0);
			break;
		case 9:
			build_type9(by0, bx0);
			break;
		case 8:
			build_type8(by0, bx0);
			break;
		case 7:
			build_type7(by0, bx0);
			break;
		case 6:
			build_type6(by0, bx0);
			break;
		case 5:
			build_type5(by0, bx0);
			break;
		case 4:
			build_type4(by0, bx0);
			break;
		case 3:
			build_type3(by0, bx0);
			break;
		case 2:
			build_type2(by0, bx0);
			break;
		case 1:
			build_type1(by0, bx0);
			break;

			/* Paranoia */
		default:
			return (FALSE);
	}

	return (TRUE);
}
