
/* File: maid-grf.c */

/* Purpose: Interface for graphical ports */

/*
 * Copyright (c) 2002 S. Fuerst
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "maid-grf.h"

#ifdef SUPPORT_GAMMA

/* Table of gamma values */
byte gamma_table[256];

/* Table of ln(x/256) * 256 for x going from 0 -> 255 */
static const s16b gamma_helper[256] =
{
	0, -1420, -1242, -1138, -1065, -1007, -961, -921, -887, -857, -830, -806,
	-783, -762, -744, -726,
	-710, -694, -679, -666, -652, -640, -628, -617, -606, -596, -586, -576,
	-567, -577, -549, -541,
	-532, -525, -517, -509, -502, -495, -488, -482, -475, -469, -463, -457,
	-451, -455, -439, -434,
	-429, -423, -418, -413, -408, -403, -398, -394, -389, -385, -380, -376,
	-371, -367, -363, -359,
	-355, -351, -347, -343, -339, -336, -332, -328, -325, -321, -318, -314,
	-311, -308, -304, -301,
	-298, -295, -291, -288, -285, -282, -279, -276, -273, -271, -268, -265,
	-262, -259, -257, -254,
	-251, -248, -246, -243, -241, -238, -236, -233, -231, -228, -226, -223,
	-221, -219, -216, -214,
	-212, -209, -207, -205, -203, -200, -198, -196, -194, -192, -190, -188,
	-186, -184, -182, -180,
	-178, -176, -174, -172, -170, -168, -166, -164, -162, -160, -158, -156,
	-155, -153, -151, -149,
	-147, -146, -144, -142, -140, -139, -137, -135, -134, -132, -130, -128,
	-127, -125, -124, -122,
	-120, -119, -117, -116, -114, -112, -111, -109, -108, -106, -105, -103,
	-102, -100, -99, -97,
	-96, -95, -93, -92, -90, -89, -87, -86, -85, -83, -82, -80, -79, -78, -76,
	-75,
	-74, -72, -71, -70, -68, -67, -66, -65, -63, -62, -61, -59, -58, -57, -56,
	-54,
	-53, -52, -51, -50, -48, -47, -46, -45, -44, -42, -41, -40, -39, -38, -37,
	-35,
	-34, -33, -32, -31, -30, -29, -27, -26, -25, -24, -23, -22, -21, -20, -19,
	-18,
	-17, -16, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1
};


/*
 * Build the gamma table so that floating point isn't needed.
 *
 *  ANGBAND_X11_GAMMA is
 * 256 * (1 / gamma), rounded to integer. A recommended value
 * is 183, which is an approximation of the Macintosh hardware
 * gamma of 1.4.
 *
 *   gamma	ANGBAND_X11_GAMMA
 *   -----	-----------------
 *   1.2	213
 *   1.25	205
 *   1.3	197
 *   1.35	190
 *   1.4	183
 *   1.45	177
 *   1.5	171
 *   1.6	160
 *   1.7	151
 *   ...
 *
 * XXX XXX The environment variable, or better,
 * the interact with colours command should allow users
 * to specify gamma values (or gamma value * 100).
 */
void build_gamma_table(int gamma)
{
	int i, n;

	/*
	 * value is the current sum.
	 * diff is the new term to add to the series.
	 */
	long value, diff;

	/* Paranoia */
	if (gamma < 0) gamma = 0;
	if (gamma > 255) gamma = 255;

	/* Hack - convergence is bad in these cases. */
	gamma_table[0] = 0;
	gamma_table[255] = 255;

	for (i = 1; i < 255; i++)
	{
		/*
		 * Initialise the Taylor series
		 *
		 * value and diff have been scaled by 256
		 */

		n = 1;
		value = 256 * 256;
		diff = ((long)gamma_helper[i]) * (gamma - 256);

		while (diff)
		{
			value += diff;
			n++;

			/*
			 * Use the following identiy to calculate the gamma table.
			 * exp(x) = 1 + x + x^2/2 + x^3/(2*3) + x^4/(2*3*4) +...
			 *
			 * n is the current term number.
			 *
			 * The gamma_helper array contains a table of
			 * ln(x/256) * 256
			 * This is used because a^b = exp(b*ln(a))
			 *
			 * In this case:
			 * a is i / 256
			 * b is gamma.
			 *
			 * Note that everything is scaled by 256 for accuracy,
			 * plus another factor of 256 for the final result to
			 * be from 0-255.  Thus gamma_helper[] * gamma must be
			 * divided by 256*256 each itteration, to get back to
			 * the original power series.
			 */
			diff = (((diff / 256) * gamma_helper[i]) *
					(gamma - 256)) / (256 * n);
		}

		/*
		 * Store the value in the table so that the
		 * floating point pow function isn't needed.
		 */
		gamma_table[i] = ((long)(value / 256) * i) / 256;
	}
}

#endif /* SUPPORT_GAMMA */

/*
 * Get the name of the default font to use for the term.
 */
cptr get_default_font(int term_num)
{
	cptr font;

	char buf[80];

	/* Window specific font name */
	sprintf(buf, "ANGBAND_X11_FONT_%d", term_num);

	/* Check environment for that font */
	font = getenv(buf);

	/* Check environment for "base" font */
	if (!font) font = getenv("ANGBAND_X11_FONT");

	/* No environment variables, use default font */
	if (!font)
	{
		switch (term_num)
		{
			case 0:
			{
				font = DEFAULT_X11_FONT_0;
				break;
			}
			case 1:
			{
				font = DEFAULT_X11_FONT_1;
				break;
			}
			case 2:
			{
				font = DEFAULT_X11_FONT_2;
				break;
			}
			case 3:
			{
				font = DEFAULT_X11_FONT_3;
				break;
			}
			case 4:
			{
				font = DEFAULT_X11_FONT_4;
				break;
			}
			case 5:
			{
				font = DEFAULT_X11_FONT_5;
				break;
			}
			case 6:
			{
				font = DEFAULT_X11_FONT_6;
				break;
			}
			case 7:
			{
				font = DEFAULT_X11_FONT_7;
				break;
			}
			default:
			{
				font = DEFAULT_X11_FONT;
			}
		}
	}

	return (font);
}

#ifdef USE_GRAPHICS
bool pick_graphics(int graphics, int *xsize, int *ysize, char *filename)
{
	int old_graphics = use_graphics;

	use_graphics = GRAPHICS_NONE;
	use_transparency = FALSE;

	if ((graphics == GRAPHICS_ANY)
		|| (graphics == GRAPHICS_ADAM_BOLT) || (graphics == GRAPHICS_HALF_3D))
	{
		/* Try the "16x16.bmp" file */
		path_build(filename, 1024, ANGBAND_DIR_XTRA, "graf/16x16.bmp");

		/* Use the "16x16.bmp" file if it exists */
		if (0 == fd_close(fd_open(filename, O_RDONLY)))
		{
			use_transparency = TRUE;

			*xsize = 16;
			*ysize = 16;

			/* Use graphics */
			if (graphics == GRAPHICS_HALF_3D)
			{
				use_graphics = GRAPHICS_HALF_3D;
			}
			else
			{
				use_graphics = GRAPHICS_ADAM_BOLT;
			}
		}
	}

	/* We failed, or we want 8x8 graphics */
	if (!use_graphics
		&& ((graphics == GRAPHICS_ANY) || (graphics == GRAPHICS_ORIGINAL)))
	{
		/* Try the "8x8.bmp" file */
		path_build(filename, 1024, ANGBAND_DIR_XTRA, "graf/8x8.bmp");

		/* Use the "8x8.bmp" file if it exists */
		if (0 == fd_close(fd_open(filename, O_RDONLY)))
		{
			/* Use graphics */
			use_graphics = GRAPHICS_ORIGINAL;

			*xsize = 8;
			*ysize = 8;
		}
	}

	/* Did we change the graphics? */
	if (old_graphics == use_graphics) return (FALSE);

	/* Success */
	return (TRUE);
}
#endif /* USE_GRAPHICS */

#ifdef TERM_USE_MAP

static bool map_init = FALSE;

/* List of 16x16 blocks for the overhead map */
map_blk_ptr_ptr *map_cache;

/* Refcount for map cache */
byte *map_cache_refcount;

/* Location of cache blocks */
int *map_cache_x;
int *map_cache_y;

/* The map itself - grid of 16x16 blocks*/
int **map_grid;

/* Player location */
static int player_x = 0;
static int player_y = 0;

/* Hooks to access overhead map */
static map_info_hook_type map_info_hook = NULL;
static map_erase_hook_type map_erase_hook = NULL;

/*
 * Set the map hook - returning the old hook
 *
 * You need to keep a copy of the old hook
 * to chain in on because multiple places
 * use the overhead map.
 */
map_info_hook_type set_map_hook(map_info_hook_type hook_func)
{
	/* Save the original hook */
	map_info_hook_type temp = map_info_hook;

	/* Set the hook */
	map_info_hook = hook_func;

	/* Return the old hook for chaining */
	return (temp);
}

/*
 * Set the erase hook - returning the old hook
 *
 * You need to keep a copy of the old hook
 * to chain in on because multiple places
 * use the overhead map.
 */
map_erase_hook_type set_erase_hook(map_erase_hook_type hook_func)
{
	/* Save the original hook */
	map_erase_hook_type temp = map_erase_hook;

	/* Set the hook */
	map_erase_hook = hook_func;

	/* Return the old hook for chaining */
	return (temp);
}



/*
 * Clear the map when changing a level.
 */
static void clear_map(void)
{
	int i, j;

	/* Erase the map */
	for (i = 0; i < WILD_SIZE; i++)
	{
		for (j = 0; j < WILD_SIZE; j++)
		{
			/* Set unused */
			map_grid[i][j] = -1;
		}
	}

	/* Erase the cache */
	for (i = 0; i < MAP_CACHE; i++)
	{
		map_cache_refcount[i] = 0;

		/* Flag that the block isn't used */
		map_cache_x[i] = -1;
	}

	/* Player doesn't have a position */
	player_x = 0;
	player_y = 0;
}

/*
 * Create the map information
 */
void init_overhead_map(void)
{
	int i, j;

	/* Do not initialize twice */
	if (map_init) return;

	/* Make the list of pointers to blocks */
	C_MAKE(map_cache, MAP_CACHE, map_blk_ptr_ptr);

	/* Refcount for cache blocks */
	C_MAKE(map_cache_refcount, MAP_CACHE, byte);

	/* Cache block locations */
	C_MAKE(map_cache_x, MAP_CACHE, int);
	C_MAKE(map_cache_y, MAP_CACHE, int);

	/* Allocate each block */
	for (i = 0; i < MAP_CACHE; i++)
	{
		/* Allocate block */
		C_MAKE(map_cache[i], WILD_BLOCK_SIZE, map_blk_ptr);

		/* Allocate rows of a block */
		for (j = 0; j < WILD_BLOCK_SIZE; j++)
		{
			C_MAKE(map_cache[i][j], WILD_BLOCK_SIZE, map_block);
		}
	}

	/* Allocate the overhead map itself */
	C_MAKE(map_grid, WILD_SIZE, int *);

	for (i = 0; i < WILD_SIZE; i++)
	{
		/* Allocate one row of the wilderness */
		C_MAKE(map_grid[i], WILD_SIZE, int);
	}

	/* The map exists */
	map_init = TRUE;

	/* Initialize */
	clear_map();
}

/*
 * Delete the overhead map
 */
void del_overhead_map(void)
{
	int i, j;

	/* Do not remove twice */
	if (!map_init) return;

	/* Free refcount for cache blocks */
	FREE(map_cache_refcount);

	/* Cache block locations */
	FREE(map_cache_x);
	FREE(map_cache_y);

	/* Delete each block */
	for (i = 0; i < MAP_CACHE; i++)
	{
		/* Allocate rows of a block */
		for (j = 0; j < WILD_BLOCK_SIZE; j++)
		{
			FREE(map_cache[i][j]);
		}

		/* Free block */
		FREE(map_cache[i]);
	}

	/* Free the list of pointers to blocks */
	FREE(map_cache);

	for (i = 0; i < WILD_SIZE; i++)
	{
		/* Free one row of the wilderness */
		FREE(map_grid[i]);
	}

	/* Free the overhead map itself */
	FREE(map_grid);

	/* The map no longer exists */
	map_init = FALSE;
}


/*
 * Erase a block
 */
static void clear_block(int block)
{
	int i, j;

	map_block *mb_ptr;

	/* Wipe each square */
	for (i = 0; i < WILD_BLOCK_SIZE; i++)
	{
		for (j = 0; j < WILD_BLOCK_SIZE; j++)
		{
			mb_ptr = &map_cache[block][i][j];

			(void)WIPE(mb_ptr, map_block);
		}
	}

	/* Was this used? */
	if (map_cache_x[block] != -1)
	{
		/* Mark map block as unused */
		map_grid[map_cache_y[block]][map_cache_x[block]] = -1;

		/* Set "unused block" flag */
		map_cache_x[block] = -1;
	}
}


/*
 * Find an empty block to use
 */
static int get_empty_block(void)
{
	int i;
	int dist, best_dist = 0;
	int best_block = 0;

	int px, py;

	/* Get player block location */
	px = player_x / 16;
	py = player_y / 16;

	/* Scan for a used but out of los block */
	for (i = 0; i < MAP_CACHE; i++)
	{
		/* Get block out of los */
		if (map_cache_refcount[i]) continue;

		/* Check to see if unused */
		if (map_cache_x[i] < 0)
		{
			best_block = i;
			break;
		}

		/* Get rough dist from player */
		dist = ABS(map_cache_x[i] - px) + ABS(map_cache_y[i] - py);

		/* Save furthest block */
		if (dist > best_dist)
		{
			best_dist = dist;
			best_block = i;
		}
	}

	/* Erase the block */
	clear_block(best_block);

	/* Return the furthest unused block from the player */
	return (best_block);
}


/*
 * Is the location in bounds on the map?
 */
bool map_in_bounds(int x, int y)
{
	if (x < 0 || x >= WILD_BLOCK_SIZE * WILD_SIZE) return (FALSE);
	if (y < 0 || y >= WILD_BLOCK_SIZE * WILD_SIZE) return (FALSE);
	return (map_grid[y >> 4][x >> 4] != -1);
}


/*
 * Save information into a block location
 */
static void save_map_location(int x, int y, term_map *map)
{
	map_blk_ptr_ptr mbp_ptr;
	map_block *mb_ptr;

	int x1 = x / WILD_BLOCK_SIZE;
	int y1 = y / WILD_BLOCK_SIZE;

	int block_num;

	/* Does the location exist? */
	if (!map_in_bounds(x, y))
	{
		/* Create a new block there */
		block_num = get_empty_block();

		/* Set this block up */
		mbp_ptr = map_cache[block_num];

		/* Link to the map */
		map_grid[y1][x1] = block_num;

		/* Save block coordinates */
		map_cache_x[block_num] = x1;
		map_cache_y[block_num] = y1;
	}
	else
	{
		block_num = map_grid[y1][x1];
		mbp_ptr = map_cache[block_num];
	}

	mb_ptr = &mbp_ptr[y & 15][x & 15];

	/* Increment refcount depending on visibility */
	if (map->flags & MAP_ONCE)
	{
		/* Wasn't seen, and now is */
		if (!(mb_ptr->flags & (MAP_ONCE)))
		{
			map_cache_refcount[block_num]++;
		}
	}
	else if (!(mb_ptr->flags & MAP_ONCE))
	{
		/* Was seen, and now is not */
		if (mb_ptr->flags & MAP_ONCE)
		{
			/* Paranoia */
			if (!map_cache_refcount[block_num])
				quit("Decrementing invalid overhead map loc");

			map_cache_refcount[block_num]--;
		}
	}

	/* Remember info by calling hook */
	if (map_info_hook)
	{
		map_info_hook(mb_ptr, map);
	}

	/* Save the flags */
	mb_ptr->flags = map->flags;

#ifdef TERM_CAVE_MAP

	/* Save the information */
	mb_ptr->terrain = map->terrain;
	mb_ptr->field = map->field;
	
	mb_ptr->object = map->object;
	mb_ptr->unknown = map->unknown;
	
	mb_ptr->monster = map->monster;
	mb_ptr->m_flags = map->m_flags;
	mb_ptr->m_hp = map->m_hp;

#endif /* TERM_CAVE_MAP */

	/* XXX XXX Hack */
	player_x = p_ptr->px;
	player_y = p_ptr->py;
}


/*
 * Get the information in the map
 */
map_block *map_loc(int x, int y)
{
	return (&map_cache[map_grid[y / WILD_BLOCK_SIZE][x / WILD_BLOCK_SIZE]]
			[y & 15][x & 15]);
}


/*
 * Angband-specific code designed to allow the map to be sent
 * to the port as required.  This allows the main-???.c file
 * not to access internal game data, which may or may not
 * be accessable.
 */
void Term_write_map(int x, int y, cave_type *c_ptr, pcave_type *pc_ptr)
{
	term_map map;

	int fld_idx, next_f_idx, o_idx, next_o_idx;

	monster_type *m_ptr;
	object_type *o_ptr;
	field_type *fld_ptr;

	monster_race *r_ptr;
	object_kind *k_ptr;

	bool visible = pc_ptr->player & GRID_SEEN;
	bool glow = c_ptr->info & CAVE_GLOW;
	bool lite = (c_ptr->info & CAVE_MNLT) || (pc_ptr->player & GRID_LITE);

	/* Paranoia */
	if (!map_init) return;

	/* clear map info */
	(void)WIPE(&map, term_map);

	/* Save known data */
	map.terrain = pc_ptr->feat;

	/* Visible, and not hallucinating */
	if (visible && !p_ptr->image)
	{
		map.flags = MAP_SEEN | MAP_ONCE;

		if (glow) map.flags |= MAP_GLOW;
		if (lite) map.flags |= MAP_LITE;
	}

	/* Not hallucinating */
	if (!p_ptr->image)
	{
		/* Save known monsters */
		if (c_ptr->m_idx)
		{
			m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml)
			{
				map.monster = m_ptr->r_idx;

				/* Keep this grid */
				map.flags |= MAP_ONCE;
				
				/* Get monster information */
				if (!m_ptr->csleep) map.m_flags |= MONST_AWAKE;
				if (is_friendly(m_ptr)) map.m_flags |= MONST_FRIEND;
				if (is_pet(m_ptr)) map.m_flags |= MONST_PET;
				if (m_ptr->confused) map.m_flags |= MONST_CONFUSED;
				if (m_ptr->monfear) map.m_flags |= MONST_FEAR;
				if (m_ptr->stunned) map.m_flags |= MONST_STUN;
				if (m_ptr->invulner) map.m_flags |= MONST_INVULN;
				
				/* Get scaled monster hp */
				map.m_hp = m_ptr->hp * 10 / m_ptr->maxhp;
			}

			/* Mimic in los? */
			else if (visible)
			{
				r_ptr = &r_info[m_ptr->r_idx];

				if (r_ptr->flags1 & RF1_CHAR_MIMIC)
				{
					/* Keep this grid */
					map.flags |= MAP_ONCE;

					/* Save mimic character */
					map.unknown = r_ptr->d_char;
				}
			}
		}

		/* Fields */
		for (fld_idx = c_ptr->fld_idx; fld_idx; fld_idx = next_f_idx)
		{
			/* Acquire field */
			fld_ptr = &fld_list[fld_idx];

			/* Acquire next field */
			next_f_idx = fld_ptr->next_f_idx;

			/* Memorized, visible fields */
			if ((fld_ptr->info & (FIELD_INFO_MARK | FIELD_INFO_VIS)) ==
				(FIELD_INFO_MARK | FIELD_INFO_VIS))
			{
				map.field = fld_ptr->t_idx;

				/* Keep this grid */
				map.flags |= MAP_ONCE;

				/* Stop looking */
				break;
			}
		}

		for (o_idx = c_ptr->o_idx; o_idx; o_idx = next_o_idx)
		{
			/* Acquire object */
			o_ptr = &o_list[o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Memorized objects */
			if (o_ptr->marked)
			{
				k_ptr = &k_info[o_ptr->k_idx];

				/* Flavoured object */
				if (k_ptr->flavor)
				{
					/* Save flavor character */
					map.unknown = k_ptr->d_char;
				}
				else
				{
					/* Save object */
					map.object = o_ptr->k_idx;
				}

				/* Keep this grid */
				map.flags |= MAP_ONCE;

				/* Stop looking */
				break;
			}
		}
	}
	else
	{
		map.flags = glow ? MAP_GLOW : 0;
	}

	/* Save location */
	map.x = x;
	map.y = y;

	/* Save information in map */
	save_map_location(x, y, &map);
}

/*
 * Erase the map
 */
void Term_erase_map(void)
{
	/* Paranoia */
	if (!map_init) return;

	/* Notify erasure of the map */
	if (map_erase_hook) map_erase_hook();

	/* Actually clear the map */
	clear_map();
}

#endif /* TERM_USE_MAP */
