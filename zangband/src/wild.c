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
#endif 0




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
	return ((y > wild_grid.y_min) && (x > wild_grid.x_min)
		 && (y < wild_grid.y_max - 1)
		  && (x < wild_grid.x_max - 1));
}

/*
 * Determines if a map location is on or inside the outer boundary
 */
static bool in_bounds2_wild(int y, int x)
{
	return ((y >= wild_grid.y_min) && (x >= wild_grid.x_min)
		 && (y < wild_grid.y_max) && (x < wild_grid.x_max));
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
	
	return &wild_grid.block_ptr[((u16b) y>>4)-wild_grid.y]
		[((u16b) x>>4)-wild_grid.x][y&15][x&15];
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
	}
	else
	{
		/* In the dungeon */
		in_bounds = in_bounds_cave;
		in_bounds2 = in_bounds2_cave;
		area = access_cave;
	}	
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

static void place_building(int x, int y, blk_ptr block_ptr)
{
	block_ptr[y][x].info = CAVE_GLOW|CAVE_MARK;
	block_ptr[y][x].feat = FEAT_PERM_SOLID;
}

	
/* Make a new block based on the terrain type */
static void gen_block(int x, int y, blk_ptr block_ptr)
{
	int i, j;
	
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
	Rand_value = wild_grid.wild_seed + x + y*WILD_SIZE;
		
	/* Generate a terrain block*/
	
	for (i = 0; i < WILD_BLOCK_SIZE; i++)
	{
		for (j = 0; j < WILD_BLOCK_SIZE; j++)
		{
			/* Make Grass */
			block_ptr[j][i].info = CAVE_GLOW|CAVE_MARK;
			block_ptr[j][i].feat = FEAT_GRASS;
			
			/* Note: Fix info flags later. */
		}	
	}
	
	/* Add a tree so know if movement works properly. */
	block_ptr[rand_int(WILD_BLOCK_SIZE)]
	 [rand_int(WILD_BLOCK_SIZE)].feat = FEAT_TREES;
	 
	 
	/* Add roads / river / lava (Not done)*/
	 
	 
	/* Overlay town  (Not done) */
	 
	/* Hack - one town at the moment */
	p_ptr->town_num = 1;
	
	if (wild[y][x].done.town)
	{
		/* Make a dodgy town */
		for(i = 0; i < 9; i++)
		{
			/* Make a building */
			place_building(1+5*(i/3), 1+5*(i%3), block_ptr);
			place_building(1+5*(i/3), 2+5*(i%3), block_ptr);
			place_building(1+5*(i/3), 3+5*(i%3), block_ptr);
			place_building(2+5*(i/3), 1+5*(i%3), block_ptr);
			place_building(2+5*(i/3), 2+5*(i%3), block_ptr);
			place_building(2+5*(i/3), 3+5*(i%3), block_ptr);
			place_building(3+5*(i/3), 1+5*(i%3), block_ptr);
			place_building(3+5*(i/3), 3+5*(i%3), block_ptr);
			block_ptr[2+5*(i%3)][3+5*(i/3)].info = CAVE_GLOW|CAVE_MARK;
			block_ptr[2+5*(i%3)][3+5*(i/3)].feat = FEAT_SHOP_HEAD + i;
		
		}
		
		/* Stairway down */
		block_ptr[0][0].info = CAVE_GLOW|CAVE_MARK;
		block_ptr[0][0].feat = FEAT_MORE;	
	}
	
	/* if town changes... */
	/* Reset the buildings */
	/* init_buildings(); */
	
	 
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
	
	/* Hack -- Use the "complex" RNG */
	Rand_quick = FALSE;
}

/* Allocate all grids around player */
void allocate_all(void)
{
	int x, y;
	blk_ptr block_ptr;
	
	/* Allocate blocks around player */
	for(x = 0; x < WILD_GRID_SIZE; x++)
	{
		for(y = 0; y < WILD_GRID_SIZE; y++)
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
			wild_grid.block_ptr[j-1][i] =
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
				wild_grid.block_ptr[j-1][i];		
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
			wild_grid.block_ptr[j][i-1] =
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
				wild_grid.block_ptr[j][i-1];		
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
	x = ((u16b) p_ptr->wilderness_x>>4) - WILD_GRID_SIZE/2;
	y = ((u16b) p_ptr->wilderness_y>>4) - WILD_GRID_SIZE/2;
	
	/* Move if out of bounds */
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x + WILD_GRID_SIZE > WILD_SIZE) x = WILD_SIZE - WILD_GRID_SIZE;
	if (y + WILD_GRID_SIZE > WILD_SIZE) y = WILD_SIZE - WILD_GRID_SIZE;
		
	/* 
	 * Hack - check to see if first block is the same.
	 * If so, the grid doesn't need to move.
	 */
	if ((x == wild_grid.x) && (y == wild_grid.y)) return;
	
	dx = x - wild_grid.x;
	dy = y - wild_grid.y;
	 
	/* Store in upper left hand corner. */
	wild_grid.x = x;
	wild_grid.y = y;
	 
	/* Recalculate boundaries */
	wild_grid.y_max = (y+WILD_GRID_SIZE)<<4;
	wild_grid.y_min = y<<4;
	
	wild_grid.x_max = (x+WILD_GRID_SIZE)<<4;
	wild_grid.x_min = x<<4;
	
	
	/* Shift in only a small discrepency */
	if (abs(dy) == 1)
	{
		if (dy == 1) shift_down();
		else shift_up();	
	}
	else if (dy)
	{
		/* Too large of a shift */
		allocate_all();
		return;
	}
	
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
	
	/*Fill wilderness with grass*/
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
	p_ptr->wilderness_x = 32*16;
	p_ptr->wilderness_y = 32*16;
	
	if(!dun_level)
	{
		px = p_ptr->wilderness_x;
		py = p_ptr->wilderness_y;
	}
	
	max_wild_y = WILD_SIZE*16;
	max_wild_x = WILD_SIZE*16;
	
	/* Determine number of panels */
	max_panel_rows = WILD_SIZE*16 * 2 - 2;
	max_panel_cols = WILD_SIZE*16 * 2 - 2;

	/* Assume illegal panel */
	panel_row = max_panel_rows;
	panel_col = max_panel_cols;
	
	/* Clear cache */
	wild_grid.cache_count = 0;
	
	/* Fix location of grid */
	
	/* Hack - set the coords to crazy values so move_wild() works. */
	wild_grid.x = -1;
	wild_grid.y = -1;
	
	/* A dodgy town */
	wild[32][32].done.town = 1;
	
	/* Refresh random number seed */
	wild_grid.wild_seed = rand_int(0x10000000);
	
	/* Allocate blocks around player */
	move_wild();
}







