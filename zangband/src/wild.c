/* CVS: Last edit by $Author$ on $Date$ */
/* File: wild.c */

/* Purpose: Wilderness generation */

/*
 * Copyright (c) 1989, 1999 James E. Wilson, Robert A. Koeneke,
 * Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"




/* Code fragment that lights / darkens old wilderness */

#if 0

	/* Day time */
	if ((turn % (10L * TOWN_DAWN)) < ((10L * TOWN_DAWN) / 2))
		daytime = TRUE;
	else
		daytime = FALSE;

	/* Light up or darken the area */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Get the cave grid */
			c_ptr = &cave[y][x];

			if (daytime)
			{
				/* Assume lit */
				c_ptr->info |= (CAVE_GLOW);

				/* Hack -- Memorize lit grids if allowed */
				if (view_perma_grids) c_ptr->info |= (CAVE_MARK);
			}
			else
			{
				/* Darken "boring" features */
				if ((c_ptr->feat <= FEAT_INVIS) ||
				    ((c_ptr->feat >= FEAT_DEEP_WATER) &&
					(c_ptr->feat <= FEAT_TREES)))
				{
					/* Forget the grid */
					c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);
				}
			}
		}
	}
#endif


/* Town currently stored in cave[][] */
static u16b cur_town;


/* Old in_bounds(2) macros are now functions */


/*
 * Determines if a map location is fully inside the outer walls
 */
static bool in_bounds_cave(int y, int x)
{
	return ((y > 0) && (x > 0) && (y < cur_hgt-1) && (x < cur_wid-1));
}

/*
 * Determines if a map location is on or inside the outer walls
 */
static bool in_bounds2_cave(int y, int x)
{
	return ((y >= 0) && (x >= 0) && (y < cur_hgt) && (x < cur_wid));
}

/*
 * Wilderness bounds funcitons.
 * These functions check to see if a square is accessable on the grid
 * of wilderness blocks in the cache.
 */

/*
 * Determines if a map location is fully inside the outer boundary
 */
static bool in_bounds_wild(int y, int x)
{
	/* Multiply block bounds by 16 to get bounds in normal coords. */
	return ((y > wild_grid.y_min) &&
	        (x > wild_grid.x_min) &&
	        (y < wild_grid.y_max - 1) &&
	        (x < wild_grid.x_max - 1));
}

/*
 * Determines if a map location is on or inside the outer boundary
 */
static bool in_bounds2_wild(int y, int x)
{
	return ((y >= wild_grid.y_min) && (x >= wild_grid.x_min) &&
	        (y < wild_grid.y_max) && (x < wild_grid.x_max));
}


/* Access the old cave array. */
static cave_type *access_cave(int y, int x)
{
	return &cave[y][x];
}

/* Access wilderness */
static cave_type *access_wild(int y, int x)
{
	/*
	 * Divide by 16 to get block.
	 * Logical AND with 15 to get location within block.
	 */

	return &wild_grid.block_ptr[((u16b) y>>4) - wild_grid.y]
		[((u16b) x>>4) - wild_grid.x][y & 15][x & 15];
}


/*
 * This function _must_ be called whenever the dungeon level changes.
 * It makes sure the bounds and access functions point to the correct
 * functions.  If this is not done - bad things happen.
 */

void change_level(int level)
{
	if (!level)
	{
		/* In the wilderness */
		in_bounds = in_bounds_wild;
		in_bounds2 = in_bounds2_wild;
		area = access_wild;

		/*
		 * Allocate blocks around player - only has effect if
		 * old game is loaded in the dungeon.
		 */
		if (!dun_level)
		{
			move_wild();
		}
	}
	else
	{
		/* In the dungeon */
		in_bounds = in_bounds_cave;
		in_bounds2 = in_bounds2_cave;
		area = access_cave;

		/* No town stored in cave[][] */
		cur_town = 0;
	}
}



/*
 * Builds a store at a given pseudo-location
 *
 * As of Z 2.3.6 the town is moved back to (0,0) - and is overlayed
 * on top of the wilderness.
 *
 * As of 2.8.1 (?) the town is actually centered in the middle of a
 * complete level, and thus the top left corner of the town itself
 * is no longer at (0,0), but rather, at (qy,qx), so the constants
 * in the comments below should be mentally modified accordingly.
 *
 * As of 2.7.4 (?) the stores are placed in a more "user friendly"
 * configuration, such that the four "center" buildings always
 * have at least four grids between them, to allow easy running,
 * and the store doors tend to face the middle of town.
 *
 * The stores now lie inside boxes from 3-9 and 12-18 vertically,
 * and from 7-17, 21-31, 35-45, 49-59.  Note that there are thus
 * always at least 2 open grids between any disconnected walls.
 *
 * Note the use of "town_illuminate()" to handle all "illumination"
 * and "memorization" issues.
 */
static void build_store(int n, int yy, int xx)
{
	int y, x, y0, x0, y1, x1, y2, x2, tmp;

	/* Find the "center" of the store */
	y0 = yy * 6 + 4;
	x0 = xx * 17 + 8;

	/* Determine the store boundaries */
	y1 = y0 - randint(2);
	y2 = y0 + randint(2);
	x1 = x0 - randint(5);
	x2 = x0 + randint(5);

	/* Build an invulnerable rectangular building */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			/* Create the building */
			cave[y][x].feat = FEAT_PERM_EXTRA;
		}
	}

	/* Pick a door direction (S,N,E,W) */
	tmp = rand_int(4);

	/* Re-roll "annoying" doors */
	if (((tmp == 0) && (yy == 1)) ||
	    ((tmp == 1) && (yy == 0)) ||
	    ((tmp == 2) && (xx == 3)) ||
	    ((tmp == 3) && (xx == 0)))
	{
		/* Pick a new direction */
		tmp = rand_int(4);
	}

	/* Extract a "door location" */
	switch (tmp)
	{
		/* Bottom side */
		case 0:
		{
			y = y2;
			x = rand_range(x1, x2);
			break;
		}

		/* Top side */
		case 1:
		{
			y = y1;
			x = rand_range(x1, x2);
			break;
		}

		/* Right side */
		case 2:
		{
			y = rand_range(y1, y2);
			x = x2;
			break;
		}

		/* Left side */
		default:
		{
			y = rand_range(y1, y2);
			x = x1;
			break;
		}
	}

	/* Clear previous contents, add a store door */
	cave[y][x].feat = FEAT_SHOP_HEAD + n;
}




/*
 * Generate the "consistent" town features, and place the player
 *
 * Hack -- play with the R.N.G. to always yield the same town
 * layout, including the size and shape of the buildings, the
 * locations of the doorways, and the location of the stairs.
 */
static void town_gen_hack(u16b town_num)
{
	int y, x, k, n;

	/* This simple routine does not check the type of
	 * stores town_num wants.
	 */

	int rooms[MAX_STORES];


	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant town layout */
	Rand_value = town[town_num].seed;


	/* Prepare an Array of "remaining stores", and count them */
	for (n = 0; n < MAX_STORES; n++) rooms[n] = n;

	/* Place three rows of stores */
	for (y = 0; y < 3; y++)
	{
		/* Place three stores per row */
		for (x = 0; x < 3; x++)
		{
			/* Pick a random unplaced store */
			k = ((n <= 1) ? 0 : rand_int(n));

			/* Build that store at the proper location */
			build_store(rooms[k], y, x);

			/* Shift the stores down, remove one store */
			rooms[k] = rooms[--n];
		}
	}


	/* Place the stairs */
	while (TRUE)
	{
		/* Pick a location at least "three" from the outer walls */
		y = rand_range(3, SCREEN_HGT - 4);
		x = rand_range(3, SCREEN_WID - 4);

		/* Require a "naked" floor grid */
		if (cave[y][x].feat == FEAT_FLOOR) break;
	}

	/* Clear previous contents, add down stairs */
	cave[y][x].feat = FEAT_MORE;

	/* Hack -- use the "complex" RNG */
	Rand_quick = FALSE;
}




/*
 * Town logic flow for generation of new town
 *
 * We start with a fully wiped cave of normal floors.
 *
 * Note that town_gen_hack() plays games with the R.N.G.
 *
 * This function does NOT do anything about the owners of the stores,
 * nor the contents thereof.  It only handles the physical layout.
 *
 */
static void town_gen(u16b town_num)
{
	int y, x;

	/* Place transparent area */
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			/* Create empty area */
			cave[y][x].feat = FEAT_NONE;
		}
	}

	/* Place some floors */
	for (y = 1; y < SCREEN_HGT-1; y++)
	{
		for (x = 1; x < SCREEN_WID-1; x++)
		{
			/* Create empty floor */
			cave[y][x].feat = FEAT_FLOOR;
		}
	}

	/* Build stuff */
	town_gen_hack(town_num);

	/* Town is now built */
	cur_town = town_num;
}

/*
 * Overlay a section of a town onto the wilderness
 */
static void overlay_town(byte y, byte x, u16b w_town, blk_ptr block_ptr)
{
	int i, j, xx, yy;
	byte feat;

	/* Find block to copy */
	xx = (x - town[w_town].x) << 4;
	yy = (y - town[w_town].y) << 4;

	/* copy 16x16 block from cave[][] */
	for (j = 0; j < WILD_BLOCK_SIZE; j++)
	{
		for (i = 0; i < WILD_BLOCK_SIZE; i++)
		{
			feat = cave[yy + j][xx + i].feat;

			if (feat != FEAT_NONE)
			{
				/* Only copy if there is something there. */
				block_ptr[j][i].feat = feat;
			}
		}
	}
}


/*
 * Initialise the town structures
 * At the moment there is only one town generator
 * and four towns.
 *
 * The four towns are right next to each other. This is
 * a test of the speed of the switching mechanism.
 * Hopefully it will be fast enough so that towns can be placed
 * anywhere in the wilderness.  It will also be possible to
 * have *HUGE* towns (cities) composed of smaller parts.
 */
static void init_towns(void)
{
	int i, j;
	u16b town_num;


	for (town_num = 1; town_num <= 4; town_num++)
	{
		strcpy(town[town_num].name, "town");
		town[town_num].seed = rand_int(0x10000000);
		town[town_num].numstores = 9;
		town[town_num].type = 1;
		town[town_num].x = 32 + ((town_num - 1) % 2) * (SCREEN_WID / 16 + 1);
		town[town_num].y = 32 + ((town_num - 1) / 2) * (SCREEN_HGT / 16 + 1);

		/* Place town on wilderness */
		for (j = 0; j < (SCREEN_HGT / 16 + 1); j++)
		{
			for (i = 0; i < (SCREEN_WID / 16 +1); i++)
			{
				wild[town[town_num].y + j][town[town_num].x + i]
					.done.town = town_num;
			}
		}
	}

	/* No current town in cave[][] */
	cur_town = 0;
}


/*
 * Set cur_town = 0
 * This function exists so that this variable remains hidden.
 * This is only used after a game is loaded - when cave[][]
 * is blank.
 */
void set_no_town(void)
{
	cur_town = 0;
}

/* Delete a wilderness block */
static void del_block(blk_ptr block_ptr)
{
	int x, y;
	int m_idx;
	for (x = 0; x < WILD_BLOCK_SIZE; x++)
	{
		for (y = 0; y < WILD_BLOCK_SIZE; y++)
		{
			/* Clear old terrain data */
			block_ptr[y][x].info = 0;
			block_ptr[y][x].feat = 0;

			/* Delete monster on the square */
			m_idx = block_ptr[y][x].m_idx;

			/* Only delete if one exists */
			if (m_idx)
			{
				delete_monster_idx(m_idx);
				block_ptr[y][x].m_idx = 0;
			}

			/* Delete objects on the square */
			delete_object_location(&block_ptr[y][x]);
		}
	}
}


/* Store routine for the fractal cave generator */
/* this routine probably should be an inline function or a macro. */
static void store_height(int x, int y, int val)
{
	/* only write to points that are "blank" */
	if (temp_block[y][x] != MAX_SHORT) return;

	/* store the value in height-map format */
	temp_block[y][x] = val;

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
 */

static void frac_block(void)
{
	int cutoff, grd;

	/* fixed point variables- these are stored as 256 x normal value
	* this gives 8 binary places of fractional part + 8 places of normal part*/

	u16b lstep, hstep, i, j, diagsize, size;

	/* Size is one bigger than normal blocks for speed of algorithm with 2^n + 1 */
	size = WILD_BLOCK_SIZE;


	/*
	 * Scale factor for middle points:
	 * About sqrt(2) * 256 / 2 - correct for a square lattice.
	 */
	diagsize = 181;

	/* Clear the section */
	for (i = 0; i <= size; i++)
	{
		for (j = 0; j <= size; j++)
		{
			/* MAX_SHORT is a flag for "not done yet" */
			temp_block[j][i] = MAX_SHORT;
		}
	}

	/* Hack - set boundary value midway. */
	cutoff = WILD_BLOCK_SIZE * 128;

	grd = 4 * 256;

	/* Set the corner values just in case grd > size. */
	store_height(0, 0, cutoff);
	store_height(0, size, cutoff);
	store_height(size, 0, cutoff);
	store_height(size, size, cutoff);

	/* Set the middle square to be an open area. */
	store_height(size / 2, size / 2, cutoff);

	/* Initialize the step sizes */
	lstep = hstep = size * 256;
	size = size * 256;

	/*
	 * Fill in the square with fractal height data -
	 * like the 'plasma fractal' in fractint.
	 */
	while (lstep > 256)
	{
		/* Halve the step sizes */
		lstep = hstep;
		hstep /= 2;

		/* middle top to bottom.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = 0; j <= size; j += lstep)
			{
				if (hstep > grd)
				{
					/* If greater than 'grid' level then is random */
					store_height(i >> 8, j >> 8, randint(WILD_BLOCK_SIZE * 256));
				}
			   	else
				{
					/* Average of left and right points +random bit */
					store_height(i >> 8, j >> 8,
					((temp_block[j >> 8][(i - hstep) >> 8] +
					temp_block[j >> 8][(i + hstep) >>8]) >> 1) +
					((randint(lstep) - hstep) >> 1));
				}
			}
		}


		/* middle left to right.*/
		for (j = hstep; j <= size - hstep; j += lstep)
		{
			for (i = 0; i <= size; i += lstep)
		   	{
				if (hstep > grd)
				{
					/* If greater than 'grid' level then is random */
					store_height(i >> 8, j >> 8, randint(WILD_BLOCK_SIZE * 256));
				}
		   		else
				{
					/* Average of up and down points +random bit */
					store_height(i >> 8, j >> 8,
					((temp_block[(j - hstep) >> 8][i >> 8]
					+ temp_block[(j + hstep) >> 8][i >> 8]) >> 1)
					+ ((randint(lstep) - hstep) >> 1));
				}
			}
		}

		/* center.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = hstep; j <= size - hstep; j += lstep)
			{
			   	if (hstep > grd)
				{
					/* If greater than 'grid' level then is random */
					store_height(i >> 8, j >> 8, randint(WILD_BLOCK_SIZE * 256));
				}
		   		else
				{
					/* average over all four corners + scale by diagsize to
					 * reduce the effect of the square grid on the shape of the fractal */
					store_height(i >> 8, j >> 8,
					((temp_block[(j - hstep) >> 8][(i - hstep) >> 8]
					+ temp_block[(j + hstep) >> 8][(i - hstep) >> 8]
					+ temp_block[(j - hstep) >> 8][(i + hstep) >> 8]
					+ temp_block[(j + hstep) >> 8][(i + hstep) >> 8]) >> 2)
					+ (((randint(lstep) - hstep) * diagsize) >> 8));
				}
			}
		}
	}
}

/*
 * This function copies the fractal height map in temp_block
 * to a normal wilderness block.  The height map is modified
 * via a simple function to make terrain. XXX XXX XXX
 */

static void copy_block(blk_ptr block_ptr)
{
	int i, j, element;

	for (j = 0; j < WILD_BLOCK_SIZE; j++)
	{
		for (i = 0; i < WILD_BLOCK_SIZE; i++)
		{
			element = temp_block[j][i];
			if (element < WILD_BLOCK_SIZE * 128)
			{
				/* Grass */
				block_ptr[j][i].info = CAVE_GLOW|CAVE_MARK;
				block_ptr[j][i].feat = FEAT_GRASS;
			}
			else
			{
				/* Trees */
				block_ptr[j][i].info = CAVE_GLOW|CAVE_MARK;
				block_ptr[j][i].feat = FEAT_TREES;
			}
		}
	}
}



/* Make a new block based on the terrain type */
static void gen_block(int x, int y, blk_ptr block_ptr)
{
	u16b w_town;

	/*
	 * Since only grass has been "turned on", this function
	 * is rather simple at the moment.
	 * Eventually there will be a switch statement for each
	 * terrain type.
	 * Even later - most of this will be table driven.
	 */

	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant wilderness blocks */
	Rand_value = wild_grid.wild_seed + x + y * WILD_SIZE;

	/* Generate a terrain block*/

	/* Test fractal terrain */
	/* Note that this is very dodgy at the moment.
	 * There is no wilderness */
	frac_block();
	copy_block(block_ptr);

	/* Add roads / river / lava (Not done)*/


	/* Hack -- Use the "complex" RNG */
	Rand_quick = FALSE;

	/* Overlay town */
	w_town = wild[y][x].done.town;

	/* Is there a town? */
	if (w_town)
	{
		/* Is it the right town? */
		if (cur_town != w_town)
		{
			/* Make the town */
			town_gen(w_town);

			init_buildings();
		}

		/* overlay town on wilderness */
		overlay_town(y, x, w_town, block_ptr);
	}

	/* Set the monster generation level */

	/*Hack - just a number. (No themes yet.) */
	monster_level = wild[y][x].done.mon_gen;

	/* Set the object generation level */

	/* Hack - set object level to monster level */
	object_level = wild[y][x].done.mon_gen;

	/* Add monsters. (Not done) */

#ifdef USE_SCRIPT
	if (generate_wilderness_callback(y, x)) return;
#endif /* USE_SCRIPT */
}



/* Allocate all grids around player */
void allocate_all(void)
{
	int x, y;
	blk_ptr block_ptr;

	/* Allocate blocks around player */
	for (x = 0; x < WILD_GRID_SIZE; x++)
	{
		for (y = 0; y < WILD_GRID_SIZE; y++)
		{
			/* The block to use */
			block_ptr = wild_cache[x  + WILD_GRID_SIZE * y];

			/* Delete the block */
			del_block(block_ptr);

			/* Link to the grid */
			wild_grid.block_ptr[y][x] = block_ptr;

			/* Make the new block */
			gen_block(x + wild_grid.x, y + wild_grid.y, block_ptr);
		}
	}
}


/*
 * The following four functions shift the visible
 * section of the wilderness by 16 units.  This is
 * done by scrolling the grid of pointers.
 */

void shift_down(void)
{
	int i, j;
	blk_ptr block_ptr;

	for (i = 0; i < WILD_GRID_SIZE; i++)
	{
		/* The block on the edge */
		block_ptr = wild_grid.block_ptr[0][i];

		/* Delete the block */
		del_block(block_ptr);

		/* Scroll pointers */
		for (j = 1; j < WILD_GRID_SIZE; j++)
		{
			wild_grid.block_ptr[j - 1][i] =
				wild_grid.block_ptr[j][i];
		}

		/* Connect new grid to wilderness */
		wild_grid.block_ptr[WILD_GRID_SIZE - 1][i] = block_ptr;

		/* Make the new block */
		gen_block(i + wild_grid.x,
			WILD_GRID_SIZE - 1 + wild_grid.y, block_ptr);
	}
}

void shift_up(void)
{
	int i, j;
	blk_ptr block_ptr;

	for (i = 0; i < WILD_GRID_SIZE; i++)
	{
		/* The block on the edge */
		block_ptr = wild_grid.block_ptr[WILD_GRID_SIZE - 1][i];

		/* Delete the block */
		del_block(block_ptr);

		/* Scroll pointers */
		for (j = WILD_GRID_SIZE - 1; j > 0; j--)
		{
			wild_grid.block_ptr[j][i] =
				wild_grid.block_ptr[j - 1][i];
		}

		/* Connect new grid to wilderness */
		wild_grid.block_ptr[0][i] = block_ptr;

		/* Make the new block */
		gen_block(i + wild_grid.x, wild_grid.y, block_ptr);
	}
}

void shift_right(void)
{
	int i, j;
	blk_ptr block_ptr;

	for (j = 0; j < WILD_GRID_SIZE; j++)
	{
		/* The block on the edge */
		block_ptr = wild_grid.block_ptr[j][0];

		/* Delete the block */
		del_block(block_ptr);

		/* Scroll pointers */
		for (i = 1; i < WILD_GRID_SIZE; i++)
		{
			wild_grid.block_ptr[j][i - 1] =
				wild_grid.block_ptr[j][i];
		}

		/* Connect new grid to wilderness */
		wild_grid.block_ptr[j][WILD_GRID_SIZE - 1] = block_ptr;

		/* Make the new block */
		gen_block(WILD_GRID_SIZE - 1 + wild_grid.x,
			j + wild_grid.y, block_ptr);
	}
}


void shift_left(void)
{
	int i, j;
	blk_ptr block_ptr;

	for (j = 0; j < WILD_GRID_SIZE; j++)
	{
		/* The block on the edge */
		block_ptr = wild_grid.block_ptr[j][WILD_GRID_SIZE - 1];

		/* Delete the block */
		del_block(block_ptr);

		/* Scroll pointers */
		for (i = WILD_GRID_SIZE - 1; i > 0; i--)
		{
			wild_grid.block_ptr[j][i] =
				wild_grid.block_ptr[j][i - 1];
		}

		/* Connect new grid to wilderness */
		wild_grid.block_ptr[j][0] = block_ptr;

		/* Make the new block */
		gen_block(wild_grid.x, j + wild_grid.y, block_ptr);
	}
}

/*
 * Centre grid of wilderness blocks around player.
 * This must be called after the player moves in the wilderness.
 * If the player is just walking around, all that needs to be done is
 * to scroll the grid of pointers - not recalculate them all.
 * However, when the player teleports, things have to stay as is.
 */
void move_wild(void)
{
	int x, y, dx, dy;

	/* Get upper left hand block in grid. */

	/* Divide by 16 to get block from (x,y) coord + shift it.*/
	x = ((u16b) p_ptr->wilderness_x>>4) - WILD_GRID_SIZE / 2;
	y = ((u16b) p_ptr->wilderness_y>>4) - WILD_GRID_SIZE / 2;

	/* Move if out of bounds */
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x + WILD_GRID_SIZE > WILD_SIZE) x = WILD_SIZE - WILD_GRID_SIZE;
	if (y + WILD_GRID_SIZE > WILD_SIZE) y = WILD_SIZE - WILD_GRID_SIZE;

	/* Hack - set town */
	p_ptr->town_num = wild[p_ptr->wilderness_y >> 4]
		[p_ptr->wilderness_x >> 4].done.town;

	/*
	 * Hack - check to see if first block is the same.
	 * If so, the grid doesn't need to move.
	 */
	if ((x == wild_grid.x) && (y == wild_grid.y)) return;

	dx = x - wild_grid.x;
	dy = y - wild_grid.y;

	/* Store in upper left hand corner. */
	wild_grid.y = y;

	/* Recalculate boundaries */
	wild_grid.y_max = (y + WILD_GRID_SIZE) << 4;
	wild_grid.y_min = y << 4;

	/* Shift in only a small discrepency */
	if (abs(dy) == 1)
	{
		if (dy == 1) shift_down();
		else shift_up();
	}
	else if (dy)
	{
		/* Too large of a shift */

		/* Store in upper left hand corner. */
		wild_grid.x = x;

		/* Recalculate boundaries */
		wild_grid.x_max = (x + WILD_GRID_SIZE) << 4;
		wild_grid.x_min = x << 4;

		allocate_all();
		return;
	}

	/* Store in upper left hand corner. */
	wild_grid.x = x;

	/* Recalculate boundaries */
	wild_grid.x_max = (x + WILD_GRID_SIZE) << 4;
	wild_grid.x_min = x << 4;

	/* Shift in only a small discrepency */
	if (abs(dx) == 1)
	{
		if (dx == 1) shift_right();
		else shift_left();

		/* Done */
		return;
	}

	if (dx)
	{
		/* Too big of a jump */
		allocate_all();
	}
}





/*
 * Create the wilderness - dodgy function now.  Much to be done.
 * Later this will use a fractal method to make the wilderness.
 * Towns / dungeons yet.
 * No monsters yet.
 * No roads/ rivers yet.
 */

void create_wilderness(void)
{
	int i,j;

	/* Fill wilderness with grass */
	/* This will be replaced with a more inteligent routine later */
	for (i = 0; i < WILD_SIZE; i++)
	{
		for (j = 0; j < WILD_SIZE; j++)
		{
			/* All grass */
			wild[j][i].done.wild = TERRAIN_GRASS;

			/* No town yet */
			wild[j][i].done.town = 0;

			/* No rivers / roads / all unknown */
			wild[j][i].done.info = 0;

			/* No monsters */
			wild[j][i].done.mon_gen = 0;
		}
	}

	/* Hack - Reset player position to centre. */
	p_ptr->wilderness_x = 32 * 16;
	p_ptr->wilderness_y = 32 * 16;

	if (!dun_level)
	{
		px = p_ptr->wilderness_x;
		py = p_ptr->wilderness_y;

		/* Determine number of panels */
		max_panel_rows = WILD_SIZE * 16 * 2 - 2;
		max_panel_cols = WILD_SIZE * 16 * 2 - 2;

		/* Assume illegal panel */
		panel_row = max_panel_rows;
		panel_col = max_panel_cols;

		/* Hack - delete all items / monsters in wilderness */
		wipe_o_list();
		wipe_m_list();
	}

	max_wild_y = WILD_SIZE * 16;
	max_wild_x = WILD_SIZE * 16;

	/* Clear cache */
	wild_grid.cache_count = 0;

	/* Fix location of grid */

	/* Hack - set the coords to crazy values so move_wild() works. */
	wild_grid.x = -1;
	wild_grid.y = -1;

	/* A dodgy town */
	init_towns();

	/* Refresh random number seed */
	wild_grid.wild_seed = rand_int(0x10000000);
}
