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

#include "tnb.h" /* TNB */


/*
 * Helper for plasma generation.
 */
static void perturb_point_mid(int x1, int x2, int x3, int x4, int xmid,
	int ymid, int rough, int depth_max)
{
	/*
	 * Average the four corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough * 2 + 1;
	int tmp = randint(tmp2) - (rough + 1);

	int avg = ((x1 + x2 + x3 + x4) / 4) + tmp;

	/* Division always rounds down, so we round up again */
	if (((x1 + x2 + x3 + x4) % 4) > 1)
		avg++;

	/* Normalize */
	if (avg < 0)
		avg = 0;
	if (avg > depth_max)
		avg = depth_max;

	/* Set the new value. */
	cave[ymid][xmid].feat = avg;
}


static void perturb_point_end(int x1, int x2, int x3, int xmid, int ymid,
	int rough, int depth_max)
{
	/*
	 * Average the three corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough * 2 + 1;
	int tmp = rand_int(tmp2) - rough;

	int avg = ((x1 + x2 + x3) / 3) + tmp;

	/* Division always rounds down, so we round up again */
	if ((x1 + x2 + x3) % 3)
		avg++;

	/* Normalize */
	if (avg < 0)
		avg = 0;
	if (avg > depth_max)
		avg = depth_max;

	/* Set the new value. */
	cave[ymid][xmid].feat = avg;
}


/*
 * A generic function to generate the plasma fractal.
 * Note that it uses ``cave_feat'' as temporary storage.
 * The values in ``cave_feat'' after this function
 * are NOT actual features; They are raw heights which
 * need to be converted to features.
 */
static void plasma_recursive(int x1, int y1, int x2, int y2, int depth_max,
	int rough)
{
	/* Find middle */
	int xmid = (x2 - x1) / 2 + x1;
	int ymid = (y2 - y1) / 2 + y1;

	/* Are we done? */
	if (x1 + 1 == x2)
		return;

	perturb_point_mid(cave[y1][x1].feat, cave[y2][x1].feat,
		cave[y1][x2].feat, cave[y2][x2].feat, xmid, ymid, rough,
		depth_max);

	perturb_point_end(cave[y1][x1].feat, cave[y1][x2].feat,
		cave[ymid][xmid].feat, xmid, y1, rough, depth_max);

	perturb_point_end(cave[y1][x2].feat, cave[y2][x2].feat,
		cave[ymid][xmid].feat, x2, ymid, rough, depth_max);

	perturb_point_end(cave[y2][x2].feat, cave[y2][x1].feat,
		cave[ymid][xmid].feat, xmid, y2, rough, depth_max);

	perturb_point_end(cave[y2][x1].feat, cave[y1][x1].feat,
		cave[ymid][xmid].feat, x1, ymid, rough, depth_max);


	/* Recurse the four quadrants */
	plasma_recursive(x1, y1, xmid, ymid, depth_max, rough);
	plasma_recursive(xmid, y1, x2, ymid, depth_max, rough);
	plasma_recursive(x1, ymid, xmid, y2, depth_max, rough);
	plasma_recursive(xmid, ymid, x2, y2, depth_max, rough);
}


/*
 * The default table in terrain level generation.
 */
static int terrain_table[MAX_WILDERNESS][18] = {
	/* TERRAIN_EDGE */
	{
			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,

			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,

			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,

			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,

			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,

			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,
			FEAT_PERM_SOLID,
		},
	/* TERRAIN_TOWN */
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,
		},
	/* TERRAIN_DEEP_WATER */
	{
			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,

			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,

			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,

			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,

			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,

			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,
		},
	/* TERRAIN_SHALLOW_WATER */
	{
			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,
			FEAT_DEEP_WATER,

			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,

			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,

			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,

			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,
			FEAT_SHAL_WATER,

			FEAT_FLOOR,
			FEAT_DIRT,
			FEAT_GRASS,
		},
	/* TERRAIN_SWAMP */
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_GRASS,
			FEAT_GRASS,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_TREES,
			FEAT_TREES,
		},
	/* TERRAIN_DIRT */
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_GRASS,
			FEAT_TREES,
			FEAT_TREES,
		},
	/* TERRAIN_GRASS */
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_GRASS,
			FEAT_GRASS,

			FEAT_GRASS,
			FEAT_GRASS,
			FEAT_GRASS,

			FEAT_GRASS,
			FEAT_GRASS,
			FEAT_GRASS,

			FEAT_GRASS,
			FEAT_GRASS,
			FEAT_GRASS,

			FEAT_GRASS,
			FEAT_TREES,
			FEAT_TREES,
		},
	/* TERRAIN_TREES */
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_DIRT,

			FEAT_TREES,
			FEAT_TREES,
			FEAT_TREES,

			FEAT_TREES,
			FEAT_TREES,
			FEAT_TREES,

			FEAT_TREES,
			FEAT_TREES,
			FEAT_TREES,

			FEAT_TREES,
			FEAT_TREES,
			FEAT_TREES,

			FEAT_GRASS,
			FEAT_GRASS,
			FEAT_GRASS,
		},
	/* TERRAIN_DESERT */
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_GRASS,
			FEAT_GRASS,
			FEAT_GRASS,
		},
	/* TERRAIN_SHALLOW_LAVA */
	{
			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,

			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,

			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,

			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,

			FEAT_DEEP_LAVA,
			FEAT_DEEP_LAVA,
			FEAT_DEEP_LAVA,

			FEAT_DEEP_LAVA,
			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,
		},
	/* TERRAIN_DEEP_LAVA */
	{
			FEAT_DIRT,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,
			FEAT_SHAL_LAVA,

			FEAT_DEEP_LAVA,
			FEAT_DEEP_LAVA,
			FEAT_DEEP_LAVA,

			FEAT_DEEP_LAVA,
			FEAT_DEEP_LAVA,
			FEAT_DEEP_LAVA,

			FEAT_DEEP_LAVA,
			FEAT_DEEP_LAVA,
			FEAT_DEEP_LAVA,

			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,
		},
	/* TERRAIN_MOUNTAIN */
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_GRASS,

			FEAT_GRASS,
			FEAT_DIRT,
			FEAT_DIRT,

			FEAT_TREES,
			FEAT_TREES,
			FEAT_MOUNTAIN,

			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,

			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,

			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,
			FEAT_MOUNTAIN,
		},
};


void generate_wilderness_area(int terrain, u32b seed, bool border,
	bool corner)
{
	int x1, y1;
	int table_size = sizeof(terrain_table[0]) / sizeof(int);
	int roughness = 1; /* The roughness of the level. */

	/* The outer wall is easy */
	if (terrain == TERRAIN_EDGE)
	{
		/* Create level background */
		for (y1 = 0; y1 < MAX_HGT; y1++)
		{
			for (x1 = 0; x1 < MAX_WID; x1++)
			{
				cave[y1][x1].feat = FEAT_PERM_SOLID;
			}
		}

		/* We are done already */
		return;
	}


	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant town layout */
	Rand_value = seed;

	if (!corner)
	{
		/* Create level background */
		for (y1 = 0; y1 < MAX_HGT; y1++)
		{
			for (x1 = 0; x1 < MAX_WID; x1++)
			{
				cave[y1][x1].feat = table_size / 2;
			}
		}
	}

	/*
	 * Initialize the four corners
	 * ToDo: calculate the medium height of the adjacent
	 * terrains for every corner.
	 */
	cave[1][1].feat = (byte) rand_int(table_size);
	cave[MAX_HGT - 2][1].feat = (byte) rand_int(table_size);
	cave[1][MAX_WID - 2].feat = (byte) rand_int(table_size);
	cave[MAX_HGT - 2][MAX_WID - 2].feat = (byte) rand_int(table_size);

	if (!corner)
	{
		/* x1, y1, x2, y2, num_depths, roughness */
		plasma_recursive(1, 1, MAX_WID - 2, MAX_HGT - 2, table_size - 1,
			roughness);
	}

	/* Use the complex RNG */
	Rand_quick = FALSE;

	for (y1 = 1; y1 < MAX_HGT - 1; y1++)
	{
		for (x1 = 1; x1 < MAX_WID - 1; x1++)
		{
			cave[y1][x1].feat = terrain_table[terrain][cave[y1][x1].feat];
		}
	}
}



/*
 * Load a town or generate a terrain level using "plasma" fractals.
 *
 * x and y are the coordinates of the area in the wilderness.
 * Border and corner are optimization flags to speed up the
 * generation of the fractal terrain.
 * If border is set then only the border of the terrain should
 * be generated (for initializing the border structure).
 * If corner is set then only the corners of the area are needed.
 */
static void generate_area(int y, int x, bool border, bool corner)
{
	int road;
	int x1, y1;

	/* Number of the town (if any) */
	p_ptr->town_num = wilderness[y][x].town;

	/* Set the base level */
	base_level = wilderness[y][x].level;

	/* Set the dungeon level */
	dun_level = 0;

	/* Set the monster generation level */
	monster_level = base_level;

	/* Set the object generation level */
	object_level = base_level;

	/* Create the town */
	if (p_ptr->town_num)
	{
		/* Reset the buildings */
		init_buildings();

		/* Initialize the town */
		if (border | corner)
			init_flags = INIT_CREATE_DUNGEON | INIT_ONLY_FEATURES;
		else
			init_flags = INIT_CREATE_DUNGEON;

		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);
	}
	else
	{
		int terrain = wilderness[y][x].terrain;
		u32b seed = wilderness[y][x].seed;

		generate_wilderness_area(terrain, seed, border, corner);
	}

	if (!corner)
	{
		/*
		 * Place roads in the wilderness
		 * ToDo: make the road a bit more interresting
		 */
		road = wilderness[y][x].road;

		if (road & ROAD_NORTH)
		{
			/* North road */
			for (y1 = 1; y1 < MAX_HGT / 2; y1++)
			{
				x1 = MAX_WID / 2;
				cave[y1][x1].feat = FEAT_FLOOR;
			}
		}

		if (road & ROAD_SOUTH)
		{
			/* North road */
			for (y1 = MAX_HGT / 2; y1 < MAX_HGT - 1; y1++)
			{
				x1 = MAX_WID / 2;
				cave[y1][x1].feat = FEAT_FLOOR;
			}
		}

		if (road & ROAD_EAST)
		{
			/* East road */
			for (x1 = MAX_WID / 2; x1 < MAX_WID - 1; x1++)
			{
				y1 = MAX_HGT / 2;
				cave[y1][x1].feat = FEAT_FLOOR;
			}
		}

		if (road & ROAD_WEST)
		{
			/* West road */
			for (x1 = 1; x1 < MAX_WID / 2; x1++)
			{
				y1 = MAX_HGT / 2;
				cave[y1][x1].feat = FEAT_FLOOR;
			}
		}
	}
}


/*
 * Border of the wilderness area
 */

static border_type border;


typedef struct road_type road_type;
struct road_type
{
	/* location of point */
	s16b x;
	s16b y;

	/* Number of connections */
	byte connect;

	coord con_pts[4];
};

/*
 * Build the wilderness area outside of the town.
 */
void wilderness_gen(void)
{
	int i, y, x;
	bool daytime;
	cave_type *c_ptr;


	/* Big town */
	cur_hgt = MAX_HGT;
	cur_wid = MAX_WID;

	/* Determine number of panels */
	max_panel_rows = (cur_hgt / SCREEN_HGT) * 2 - 2;
	max_panel_cols = (cur_wid / SCREEN_WID) * 2 - 2;

	/* Assume illegal panel */
	panel_row = max_panel_rows;
	panel_col = max_panel_cols;

	/* Init the wilderness */
	process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);

	x = p_ptr->wilderness_x;
	y = p_ptr->wilderness_y;

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), NULL);

	/* North border */
	generate_area(y - 1, x, TRUE, FALSE);

	for (i = 1; i < MAX_WID - 1; i++)
	{
		border.north[i] = cave[MAX_HGT - 2][i].feat;
	}

	/* South border */
	generate_area(y + 1, x, TRUE, FALSE);

	for (i = 1; i < MAX_WID - 1; i++)
	{
		border.south[i] = cave[1][i].feat;
	}

	/* West border */
	generate_area(y, x - 1, TRUE, FALSE);

	for (i = 1; i < MAX_HGT - 1; i++)
	{
		border.west[i] = cave[i][MAX_WID - 2].feat;
	}

	/* East border */
	generate_area(y, x + 1, TRUE, FALSE);


	for (i = 1; i < MAX_HGT - 1; i++)
	{
		border.east[i] = cave[i][1].feat;
	}

	/* North west corner */
	generate_area(y - 1, x - 1, FALSE, TRUE);
	border.north_west = cave[MAX_HGT - 2][MAX_WID - 2].feat;

	/* North east corner */
	generate_area(y - 1, x + 1, FALSE, TRUE);
	border.north_east = cave[MAX_HGT - 2][1].feat;

	/* South west corner */
	generate_area(y + 1, x - 1, FALSE, TRUE);
	border.south_west = cave[1][MAX_WID - 2].feat;

	/* South east corner */
	generate_area(y + 1, x + 1, FALSE, TRUE);
	border.south_east = cave[1][1].feat;


	/* Leaving the dungeon by stairs */
	/* (needed before the town is loaded) */
	if (p_ptr->leaving_dungeon)
	{
		p_ptr->oldpy = 0;
		p_ptr->oldpx = 0;
	}



	/* Create terrain of the current area */
	generate_area(y, x, FALSE, FALSE);

	/* Special boundary walls -- North */
	for (i = 0; i < MAX_WID; i++)
	{
		cave[0][i].feat = FEAT_PERM_SOLID;
		cave[0][i].mimic = border.north[i];
	}

	/* Special boundary walls -- South */
	for (i = 0; i < MAX_WID; i++)
	{
		cave[MAX_HGT - 1][i].feat = FEAT_PERM_SOLID;
		cave[MAX_HGT - 1][i].mimic = border.south[i];
	}

	/* Special boundary walls -- West */
	for (i = 0; i < MAX_HGT; i++)
	{
		cave[i][0].feat = FEAT_PERM_SOLID;
		cave[i][0].mimic = border.west[i];
	}

	/* Special boundary walls -- East */
	for (i = 0; i < MAX_HGT; i++)
	{
		cave[i][MAX_WID - 1].feat = FEAT_PERM_SOLID;
		cave[i][MAX_WID - 1].mimic = border.east[i];
	}

	/* North west corner */
	cave[0][0].mimic = border.north_west;

	/* North east corner */
	cave[0][MAX_WID - 1].mimic = border.north_east;

	/* South west corner */
	cave[MAX_HGT - 1][0].mimic = border.south_west;

	/* South east corner */
	cave[MAX_HGT - 1][MAX_WID - 1].mimic = border.south_east;

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
				if (view_perma_grids)
					c_ptr->info |= (CAVE_MARK);
			}
			else
			{
				/* Darken "boring" features */
#if 1 /* TNB */
				if (g_feat_flag[c_ptr->feat] & FEAT_FLAG_BORING)
#else /* TNB */
				if ((c_ptr->feat <= FEAT_INVIS) ||
					((c_ptr->feat >= FEAT_DEEP_WATER) &&
						(c_ptr->feat <= FEAT_TREES)))
#endif /* TNB */
				{
					/* Forget the grid */
					c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);
				}
			}
		}
	}

	player_place(p_ptr->oldpy, p_ptr->oldpx);
	p_ptr->leftbldg = FALSE;
	p_ptr->leaving_dungeon = FALSE;

	/* Make some residents */
	for (i = 0; i < MIN_M_ALLOC_TN; i++)
	{
		/* Make a resident */
		(void) alloc_monster(3, TRUE);
	}

	/* Set rewarded quests to finished */
	for (i = 0; i < max_quests; i++)
	{
		if (quest[i].status == QUEST_STATUS_REWARDED)
			quest[i].status = QUEST_STATUS_FINISHED;
	}
}


typedef struct wilderness_grid wilderness_grid;

	/* Fix location of grid */

struct wilderness_grid
{
	int terrain; /* Terrain type */
	int town; /* Town number */
	byte level;	/* Level of the wilderness */
	byte road; /* Road */
	char name[32]; /* Name of the town/wilderness */
};



static wilderness_grid w_letter[255];

/*
 * Parse a sub-file of the "extra info"
 */
errr parse_line_wilderness(char *buf, int ymin, int xmin, int ymax,
	int xmax, int *y, int *x)
{

	int i, num;
	char *zz[33];

	/* Paranoia */
	if (!(buf[0] == 'W'))
		return (PARSE_ERROR_GENERIC);

	switch (buf[2])

	{
			/* Process "W:F:<letter>:<terrain>:<town>:<road>:<name> */
		case 'F':
		{
			if ((num = tokenize(buf + 4, 6, zz, 0)) > 1)
			{

				int index = zz[0][0];

				if (num > 1)
					w_letter[index].terrain = atoi(zz[1]);
				else
					w_letter[index].terrain = 0;

				if (num > 2)
					w_letter[index].level = atoi(zz[2]);
				else
					w_letter[index].level = 0;

				if (num > 3)
					w_letter[index].town = atoi(zz[3]);
				else
					w_letter[index].town = 0;

				if (num > 4)
					w_letter[index].road = atoi(zz[4]);
				else
					w_letter[index].road = 0;

				if (num > 5)
					strcpy(w_letter[index].name, zz[5]);
				else
					w_letter[index].name[0] = 0;
			}
			else
			{
				/* Failure */
				return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
			}

			break;
		}

			/* Process "W:D:<layout> */
			/* Layout of the wilderness */
		case 'D':
		{
			/* Acquire the text */
			char *s = buf + 4;

			/* Length of the text */
			int len = strlen(s);

			for (*x = xmin, i = 0; ((*x < xmax) && (i < len));
				(*x)++, s++, i++)
			{
				int idx = s[0];

				wilderness[*y][*x].terrain = w_letter[idx].terrain;

				wilderness[*y][*x].level = w_letter[idx].level;

				wilderness[*y][*x].town = w_letter[idx].town;

				wilderness[*y][*x].road = w_letter[idx].road;

				strcpy(town[w_letter[idx].town].name, w_letter[idx].name);
			}

			(*y)++;

			break;
		}

			/* Process "W:P:<x>:<y> - starting position in the wilderness */
		case 'P':
		{
			if ((p_ptr->wilderness_x == 0) && (p_ptr->wilderness_y == 0))
			{
				if (tokenize(buf + 4, 2, zz, 0) == 2)
				{
					p_ptr->wilderness_x = atoi(zz[0]);
					p_ptr->wilderness_y = atoi(zz[1]);

					if ((p_ptr->wilderness_x < 1) ||
						(p_ptr->wilderness_x > max_wild_x) ||
						(p_ptr->wilderness_y < 1) ||
						(p_ptr->wilderness_y > max_wild_y))
					{
						return (PARSE_ERROR_OUT_OF_BOUNDS);
					}
				}
				else
				{
					return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
				}
			}

			break;
		}

		default:
			/* Failure */
			return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Generate the random seeds for the wilderness
 */
void seed_wilderness(void)
{
	int x, y;
	{
		/* Init wilderness seeds */
		for (x = 0; x < max_wild_x; x++)
		{
			for (y = 0; y < max_wild_y; y++)
			{
				wilderness[y][x].seed = rand_int(0x10000000);
			}
		}
	}
}


/*
 * Pointer to wilderness_type
 */
typedef wilderness_type *wilderness_type_ptr;

/*
 * Initialize wilderness array
 */
errr init_wilderness(void)
{
	int i;

	/* Allocate the wilderness (two-dimension array) */
	C_MAKE(wilderness, max_wild_y, wilderness_type_ptr);
	C_MAKE(wilderness[0], max_wild_x * max_wild_y, wilderness_type);

	/* Init the other pointers */
	for (i = 1; i < max_wild_y; i++)
		wilderness[i] = wilderness[0] + i * max_wild_x;

	return 0;
}
