/* File: wild2.c */

/* Purpose: Wilderness generation */

/*
 * Copyright (c) 1989, 2003 James E. Wilson, Robert A. Koeneke,
 *                          Robert Ruehlmann, Steven Fuerst
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#include "wild.h"


/* The starting position of the player */
static int wild_stairs_x = 0;
static int wild_stairs_y = 0;

/*
 * Building information
 *
 * Number currently created in this town
 * Field to place, if applicable.
 * Type of building
 * Pop, magic, law levels
 * Rarity
 */
wild_building_type wild_build[MAX_CITY_BUILD] =
{
	{0, FT_STORE_GENERAL, BT_STORE, 100, 150, 150, 2},
	{0, FT_STORE_ARMOURY, BT_STORE, 150, 150, 100, 1},
	{0, FT_STORE_WEAPON, BT_STORE, 150, 150, 100, 1},
	{0, FT_STORE_TEMPLE, BT_STORE, 150, 150, 200, 1},
	{0, FT_STORE_ALCHEMIST, BT_STORE, 100, 150, 200, 21},
	{0, FT_STORE_MAGIC, BT_STORE, 200, 150, 200, 1},
	{0, FT_STORE_BLACK, BT_STORE, 250, 150, 50, 5},
	{0, FT_STORE_HOME, BT_STORE, 150, 150, 150, 2},
	{0, FT_STORE_BOOK, BT_STORE, 250, 150, 150, 2},
	{0, 0, BT_GENERAL, 150, 150, 150, 10},
	{0, FT_BUILD_WEAPON, BT_BUILD, 100, 150, 150, 5},
	{0, FT_BUILD_RECHARGE, BT_BUILD, 200, 150, 150, 10},
	{0, FT_BUILD_PLUS_WEAPON, BT_BUILD, 200, 150, 200, 10},
	{0, FT_BUILD_PLUS_ARMOUR, BT_BUILD, 200, 150, 200, 10},
	{0, FT_BUILD_MUTATE, BT_BUILD, 200, 150, 50, 20},
	{0, 0, BT_GENERAL, 150, 150, 150, 1},
	{0, 0, BT_GENERAL, 150, 150, 150, 1},
	{0, FT_BUILD_MAP, BT_BUILD, 150, 150, 150, 5},
	{0, FT_STORE_WEAPON1, BT_STORE, 100, 100, 100, 10},
	{0, FT_STORE_WEAPON2, BT_STORE, 100, 150, 100, 20},
	{0, FT_STORE_WEAPON3, BT_STORE, 100, 50, 100, 50},
	{0, FT_STORE_WEAPON4, BT_STORE, 150, 200, 100, 100},
	{0, FT_STORE_WEAPON5, BT_STORE, 200, 200, 50, 200},
	{0, FT_STORE_ARMOUR1, BT_STORE, 100, 100, 100, 10},
	{0, FT_STORE_ARMOUR2, BT_STORE, 100, 150, 100, 20},
	{0, FT_STORE_ARMOUR3, BT_STORE, 100, 150, 100, 50},
	{0, FT_STORE_ARMOUR4, BT_STORE, 150, 200, 100, 100},
	{0, FT_STORE_ARMOUR5, BT_STORE, 200, 250, 50, 200},
	{0, FT_STORE_SWORD0, BT_STORE, 100, 50, 100, 5},
	{0, FT_STORE_SWORD1, BT_STORE, 100, 50, 100, 10},
	{0, FT_STORE_SWORD2, BT_STORE, 100, 100, 100, 25},
	{0, FT_STORE_SWORD3, BT_STORE, 150, 150, 100, 50},
	{0, FT_STORE_SWORD4, BT_STORE, 200, 150, 100, 100},
	{0, FT_STORE_SWORD5, BT_STORE, 200, 200, 50, 200},
	{0, FT_STORE_SHIELD0, BT_STORE, 100, 100, 100, 5},
	{0, FT_STORE_SHIELD1, BT_STORE, 100, 100, 100, 10},
	{0, FT_STORE_SHIELD2, BT_STORE, 100, 150, 100, 25},
	{0, FT_STORE_SHIELD3, BT_STORE, 150, 150, 100, 50},
	{0, FT_STORE_SHIELD4, BT_STORE, 200, 200, 50, 200},
	{0, FT_STORE_SHIELD5, BT_STORE, 200, 250, 50, 400},
	{0, FT_STORE_AXE0, BT_STORE, 150, 50, 100, 5},
	{0, FT_STORE_AXE1, BT_STORE, 150, 50, 100, 10},
	{0, FT_STORE_AXE2, BT_STORE, 150, 100, 100, 25},
	{0, FT_STORE_AXE3, BT_STORE, 150, 100, 100, 50},
	{0, FT_STORE_AXE4, BT_STORE, 200, 150, 100, 100},
	{0, FT_STORE_AXE5, BT_STORE, 200, 150, 50, 200},
	{0, FT_STORE_AMMO0, BT_STORE, 150, 100, 100, 5},
	{0, FT_STORE_AMMO1, BT_STORE, 200, 200, 150, 10},
	{0, FT_STORE_AMMO2, BT_STORE, 250, 250, 150, 100},
	{0, FT_STORE_FLET0, BT_STORE, 100, 50, 100, 15},
	{0, FT_STORE_FLET1, BT_STORE, 100, 100, 100, 25},
	{0, FT_STORE_FLET2, BT_STORE, 150, 150, 150, 100},
	{0, FT_STORE_FLET3, BT_STORE, 150, 200, 150, 400},
	{0, FT_STORE_WARHALL0, BT_STORE, 50, 50, 50, 15},
	{0, FT_STORE_WARHALL1, BT_STORE, 50, 50, 50, 50},
	{0, FT_STORE_WARHALL2, BT_STORE, 100, 50, 100, 100},
	{0, FT_STORE_WARHALL3, BT_STORE, 100, 100, 100, 150},
	{0, FT_STORE_WARHALL4, BT_STORE, 150, 100, 200, 200},
	{0, FT_STORE_WARHALL5, BT_STORE, 150, 150, 250, 250},
	{0, FT_STORE_CLOTH0, BT_STORE, 200, 100, 150, 15},
	{0, FT_STORE_CLOTH1, BT_STORE, 150, 150, 150, 25},
	{0, FT_STORE_HARMOUR0, BT_STORE, 150, 100, 100, 25},
	{0, FT_STORE_HARMOUR1, BT_STORE, 150, 100, 100, 25},
	{0, FT_STORE_HARMOUR2, BT_STORE, 200, 150, 150, 50},
	{0, FT_STORE_HARMOUR3, BT_STORE, 200, 150, 150, 100},
	{0, FT_STORE_HARMOUR4, BT_STORE, 250, 200, 200, 200},
	{0, FT_STORE_HARMOUR5, BT_STORE, 250, 250, 200, 400},
	{0, FT_STORE_HAT0, BT_STORE, 200, 50, 150, 15},
	{0, FT_STORE_HAT1, BT_STORE, 200, 150, 150, 25},
	{0, FT_STORE_HAT2, BT_STORE, 200, 150, 200, 50},
	{0, FT_STORE_HAT3, BT_STORE, 250, 200, 200, 400},
	{0, FT_STORE_JEWEL0, BT_STORE, 150, 150, 150, 25},
	{0, FT_STORE_JEWEL1, BT_STORE, 150, 200, 150, 50},
	{0, FT_STORE_JEWEL2, BT_STORE, 200, 200, 200, 100},
	{0, FT_STORE_JEWEL3, BT_STORE, 200, 250, 200, 200},
	{0, FT_STORE_JEWEL4, BT_STORE, 200, 250, 250, 400},
	{0, FT_STORE_STATUE0, BT_STORE, 250, 150, 150, 50},
	{0, FT_STORE_STATUE1, BT_STORE, 250, 150, 150, 50},
	{0, FT_STORE_FIGUR0, BT_STORE, 200, 200, 150, 50},
	{0, FT_STORE_FIGUR1, BT_STORE, 200, 200, 200, 50},
	{0, FT_STORE_POTION0, BT_STORE, 150, 150, 150, 15},
	{0, FT_STORE_POTION1, BT_STORE, 150, 150, 150, 50},
	{0, FT_STORE_POTION2, BT_STORE, 200, 200, 200, 100},
	{0, FT_STORE_POTION3, BT_STORE, 200, 200, 200, 200},
	{0, FT_STORE_POTION4, BT_STORE, 200, 200, 200, 400},
	{0, FT_STORE_SCROLL0, BT_STORE, 150, 150, 150, 15},
	{0, FT_STORE_SCROLL1, BT_STORE, 150, 150, 150, 50},
	{0, FT_STORE_SCROLL2, BT_STORE, 200, 200, 200, 100},
	{0, FT_STORE_SCROLL3, BT_STORE, 200, 200, 200, 200},
	{0, FT_STORE_SCROLL4, BT_STORE, 200, 200, 200, 400},
	{0, FT_STORE_MAGIC0, BT_STORE, 50, 150, 200, 15},
	{0, FT_STORE_MAGIC1, BT_STORE, 100, 200, 200, 25},
	{0, FT_STORE_MAGIC2, BT_STORE, 100, 200, 200, 50},
	{0, FT_STORE_MAGIC3, BT_STORE, 150, 250, 250, 100},
	{0, FT_STORE_MAGIC4, BT_STORE, 200, 250, 250, 150},
	{0, FT_STORE_BOOK1, BT_STORE, 200, 250, 250, 50},
	{0, FT_STORE_TEMPLE1, BT_STORE, 50, 100, 150, 25},
	{0, FT_STORE_TEMPLE2, BT_STORE, 100, 150, 150, 50},
	{0, FT_STORE_TEMPLE3, BT_STORE, 150, 200, 200, 200},
	{0, FT_STORE_SUPPLIES0, BT_STORE, 150, 50, 150, 50},
	{0, FT_STORE_SUPPLIES1, BT_STORE, 100, 100, 150, 20},
	{0, FT_STORE_BLACK1, BT_STORE, 200, 150, 50, 75},
	{0, FT_STORE_BLACK2, BT_STORE, 200, 200, 50, 200},
	{0, FT_STORE_ALCHEMY1, BT_STORE, 100, 150, 150, 25},
	{0, FT_STORE_ALCHEMY2, BT_STORE, 150, 200, 150, 100},
	{0, FT_STORE_JUNK, BT_STORE, 200, 50, 150, 10},
	{0, FT_STORE_FOOD, BT_STORE, 200, 100, 150, 10},
	{0, FT_BUILD_LIBRARY, BT_BUILD, 200, 200, 200, 20},
	{0, FT_BUILD_CASINO, BT_BUILD, 100, 200, 200, 20},
	{0, FT_BUILD_INN, BT_BUILD, 100, 100, 200, 1},
	{0, FT_BUILD_HEALER, BT_BUILD, 250, 250, 200, 20},
	{0, FT_STORE_BLACK0, BT_STORE, 100, 100, 100, 10},
	{0, FT_BUILD_MAGETOWER0, BT_BUILD, 100, 150, 100, 6},
	{0, FT_BUILD_MAGETOWER1, BT_BUILD, 150, 250, 150, 20},
	{0, FT_BUILD_CASTLE0, BT_BUILD, 100, 150, 150, 10},
	{0, FT_BUILD_CASTLE1, BT_BUILD, 200, 150, 250, 20},
};

/* The stores in the starting town */
static int wild_first_town[START_STORE_NUM] =
{
	BUILD_STAIRS,
	BUILD_STORE_HOME,
	BUILD_SUPPLIES0,
	BUILD_WARHALL0,
	BUILD_STORE_TEMPLE,
	BUILD_STORE_MAGIC,
	BUILD_BLACK0
};


/*
 * Return the building name given a building "type"
 */
cptr building_name(byte build_type)
{
	u16b field_num;

	/* Look up the field type */
	field_num = wild_build[build_type].field;

	/* Return the name of the building */
	return (t_info[field_num].name);
}


/* Find a place for the player */
static void place_player_start(s32b *x, s32b *y, u16b this_town)
{
	int tempx, tempy;

	tempx = (int)place[this_town].x + wild_stairs_x / 16;
	tempy = (int)place[this_town].y + wild_stairs_y / 16;

	/* Get corner of visible region */
	shift_in_bounds(&tempx, &tempy);

	/* Set corner of visible region */
	p_ptr->old_wild_x = tempx;
	p_ptr->old_wild_y = tempy;

	/* Hack - Reset player position to be on the stairs in town */
	*x = place[this_town].x * 16 + wild_stairs_x;
	*y = place[this_town].y * 16 + wild_stairs_y;

	/* Set current town */
	p_ptr->place_num = this_town;
}


/* Pick a name for the town based on population */
void select_town_name(char *name, int pop)
{
	char buf[T_NAME_LEN + 1];
	int len;

	/* Get a normal 'elvish' name */
	get_table_name(buf, FALSE);

	/* Get length */
	len = strlen(buf) - 1;

	if (pop < T_SIZE_SMALL)
	{
		/* Hamlet */
		if ((len < T_NAME_LEN - 5) && one_in_(2))
		{
			strnfmt(name, T_NAME_LEN + 1, "%sville", buf);
		}
		else
		{
			/* Simply copy it */
			strnfmt(name, T_NAME_LEN + 1, "%s", buf);
		}
	}
	else if (pop < T_SIZE_TOWN)
	{
		/* Tiny town */
		if ((len < T_NAME_LEN - 4) && one_in_(2))
		{
			strnfmt(name, T_NAME_LEN + 1, "%s Dun", buf);
		}
				else
		{
			/* Simply copy it */
			strnfmt(name, T_NAME_LEN + 1, "%s", buf);
		}
	}
	else if (pop < T_SIZE_CITY)
	{
		/* Large Town */
		if ((len < T_NAME_LEN - 3) && one_in_(2))
		{
			strnfmt(name, T_NAME_LEN + 1, "%ston", buf);
		}
				else
		{
			/* Simply copy it */
			strnfmt(name, T_NAME_LEN + 1, "%s", buf);
		}
	}
	else if (pop < T_SIZE_CASTLE)
	{
		/* City */
		if ((len < T_NAME_LEN - 4) && one_in_(4))
		{
			strnfmt(name, T_NAME_LEN + 1, "%sford", buf);
		}
		else if ((len < T_NAME_LEN - 5) && one_in_(3))
		{
			strnfmt(name, T_NAME_LEN + 1, "%s City", buf);
		}
		else if ((len < T_NAME_LEN - 5) && one_in_(2))
		{
			strnfmt(name, T_NAME_LEN + 1, "%s View", buf);
		}
		else if ((len < T_NAME_LEN - 5) && one_in_(2))
		{
			strnfmt(name, T_NAME_LEN + 1, "%s Fort", buf);
		}
		else
		{
			/* Simply copy it */
			strnfmt(name, T_NAME_LEN + 1, "%s", buf);
		}
	}
	else
	{
		/* Castle */
		if ((len < T_NAME_LEN - 7) && one_in_(2))
		{
			strnfmt(name, T_NAME_LEN + 1, "%s Castle", buf);
		}
		else if ((len < T_NAME_LEN - 5) && one_in_(2))
		{
			strnfmt(name, T_NAME_LEN + 1, "%s Keep", buf);
		}
		else
		{
			/* Simply copy it */
			strnfmt(name, T_NAME_LEN + 1, "%s", buf);
		}
	}
}


/* Select a store or building "appropriate" for a given position */
static u16b select_building(byte pop, byte magic, byte law, u16b *build,
                            int build_num)
{
	int i;

	s32b total = 0;

	/* Draw stairs first for small towns */
	if ((build_num < 11) && (!build[BUILD_STAIRS])) return (BUILD_STAIRS);


	for (i = 0; i < MAX_CITY_BUILD; i++)
	{
		/* Work out total effects due to location */
		total = (ABS(pop - wild_build[i].pop) +
				 ABS(magic - wild_build[i].magic) +
				 ABS(law - wild_build[i].law)) + 1;

		/* Effect due to rarity */
		total *= wild_build[i].rarity;

		/* Effect due to total count */
		total += build[i] * 200;

		/* calculate probability based on location */
		wild_build[i].gen = (u16b)(MAX_SHORT / total);
	}

	/* Note that cities of size 11 have a small chance to have stairs. */

	/* Effects for cities */
	if (build_num > 11)
	{
		/* Hack - Dungeons are not in large cities */
		wild_build[BUILD_STAIRS].gen = 0;

		/* Hack - Increase possibility of 'general' features */
		for (i = 0; i < MAX_CITY_BUILD; i++)
		{
			if (build_is_general(i))
			{
				wild_build[i].gen *= ((build_num - 5) / 6);
			}
		}
	}
	/* Some buildings don't exist for small towns */
	else
	{
		for (i = 0; i < MAX_CITY_BUILD; i++)
		{
			/* No 'filler' buildings in small towns. */
			if (build_is_general(i))
			{
				wild_build[i].gen = 0;
			}
		}
	}

	/* Hack - Not more than one home per city */
	if (build[BUILD_STORE_HOME])
	{
		wild_build[BUILD_STORE_HOME].gen = 0;
	}

	/* Hack - Not more than one magetower per city */
	if (build[BUILD_MAGETOWER0] || build[BUILD_MAGETOWER1])
	{
		wild_build[BUILD_MAGETOWER0].gen = 0;
		wild_build[BUILD_MAGETOWER1].gen = 0;
	}

	total = 0;

	/* Calculate total */
	for (i = 0; i < MAX_CITY_BUILD; i++)
	{
		total += wild_build[i].gen;
	}

	/* Pick a building */
	total = randint0(total);

	/* Later add checks for silliness */
	/* (A small town with 5 "homes" would be silly) */


	/* Find which building we've got */
	for (i = 0; i < MAX_CITY_BUILD; i++)
	{
		total -= wild_build[i].gen;

		if (total < 0) return (i);
	}


	/* paranoia - we didn't find it */
	msgf("FAILED to generate building!");

	return (0);
}


static void general_init(int town_num, int store_num, byte general_type)
{
	/* Activate that feature */
	store_type *st_ptr = &place[town_num].store[store_num];

	/* Set the type */
	st_ptr->type = general_type;

	/* Initialize */
	st_ptr->data = 0;
	st_ptr->last_visit = 0;
}


static byte build_x[WILD_BLOCK_SIZE * WILD_BLOCK_SIZE];
static byte build_y[WILD_BLOCK_SIZE * WILD_BLOCK_SIZE];
static byte build_pop[WILD_BLOCK_SIZE * WILD_BLOCK_SIZE];
static byte build_count;


/*
 * Recursive function used to generate towns with no islands
 */
static void fill_town(byte x, byte y)
{
	byte i;

	/* Hack - deliberate braces to lower memory cost of recursion */
	{
		u16b *block_data = &temp_block[y][x];

		/* Do not continue if hit a previously done area. */
		if (*block_data == 1) return;

		/* Do not redo a building */
		if (*block_data == 2) return;

		/* Save the square */
		build_pop[build_count] = *block_data / WILD_BLOCK_SIZE;

		/* Do not redo this square */
		*block_data = 2;
	}

	build_x[build_count] = x;
	build_y[build_count] = y;

	/* Increment store counter */
	build_count++;

	/* Look at adjacent squares */
	for (i = 0; i < 8; i++)
	{
		/* Recurse */
		fill_town(x + ddx_ddd[i], y + ddy_ddd[i]);
	}
}


/* Work out where the walls are */
static void find_walls(void)
{
	int i, j, k, l;

	/* Copy the temp block to the town block */
	for (i = 0; i < WILD_BLOCK_SIZE + 1; i++)
	{
		for (j = 0; j < WILD_BLOCK_SIZE + 1; j++)
		{
			if (temp_block[j][i] < WILD_BLOCK_SIZE * 128)
			{
				/* Outside the town */
				temp_block[j][i] = 0;
			}
		}
	}

	/* Find walls */
	for (i = 0; i < WILD_BLOCK_SIZE; i++)
	{
		for (j = 0; j < WILD_BLOCK_SIZE; j++)
		{
			/* Is a "city block" */
			if (temp_block[j][i])
			{
				/* Scan around */
				for (k = -1; k <= 1; k++)
				{
					for (l = -1; l <= 1; l++)
					{
						/* In bounds? */
						if ((i + k >= 0) && (i + k < WILD_BLOCK_SIZE) &&
							(j + l >= 0) && (j + l < WILD_BLOCK_SIZE))
						{
							/* Is it outside? */
							if (!temp_block[j + l][i + k])
							{
								/* Make a wall */
								temp_block[j][i] = 1;
							}
						}
						else
						{
							/* Make a wall */
							temp_block[j][i] = 1;
						}
					}
				}
			}
		}
	}

}

/*
 * Driver function for the fill_town() routine
 */
static byte fill_town_driver(void)
{
	/* Paranoia - middle square must be in the town */
	if (!temp_block[WILD_BLOCK_SIZE / 2][WILD_BLOCK_SIZE / 2]) return (0);

	build_count = 0;

	/* 'Fill' the town with buildings, stopping at the walls */
	fill_town(WILD_BLOCK_SIZE / 2, WILD_BLOCK_SIZE / 2);

	/* Return number of buildings allocated */
	return (build_count);
}

/*
 * Remove "islands" from cities.
 *
 * Check that the city is fully connected...
 */
static void remove_islands(void)
{
	int i, j, k, l;
	bool city_block;

	/* Rescan walls to avoid "islands" */
	for (i = 0; i < WILD_BLOCK_SIZE; i++)
	{
		for (j = 0; j < WILD_BLOCK_SIZE; j++)
		{
			/* Is a "wall block" */
			if (temp_block[j][i] == 1)
			{
				city_block = FALSE;

				/* Scan around */
				for (k = -1; k <= 1; k++)
				{
					for (l = -1; l <= 1; l++)
					{
						/* In bounds? */
						if ((i + k >= 0) && (i + k < WILD_BLOCK_SIZE) &&
							(j + l >= 0) && (j + l < WILD_BLOCK_SIZE))
						{
							/* Is it a city block? */
							if (temp_block[j + l][i + k] == 2)
							{
								/* We are next to a city */
								city_block = TRUE;
							}
						}
					}
				}

				/* No islands */
				if (!city_block) temp_block[j][i] = 0;
			}
		}
	}
}


/*
 * Create a city + contained stores and buildings
 */
static bool create_city(int x, int y, int town_num)
{
	int i, j;

	/* Hack - fix this XXX XXX */

	/* int pop = wild[y][x].trans.pop_map; */
	int pop = ((wild[y][x].trans.pop_map + wild[y][x].trans.law_map) /
			   rand_range(4, 32)) + 128;
	int law = wild[y][x].trans.law_map;
	int magic;
	int build_num = 0, build_tot;
	byte building;
	byte count;
	byte gate_value[MAX_GATES];
	byte gate_num[MAX_GATES];

	u32b rng_seed_save;

	wild_gen2_type *w_ptr;
	place_type *pl_ptr = &place[town_num];

	u16b build[MAX_CITY_BUILD];
	u16b build_list[WILD_BLOCK_SIZE * WILD_BLOCK_SIZE];

	/* Hack - the first town is special */
	if (town_num == 1)
	{
		/* Use a low pop - we don't want too many blank buildings */
		pop = 64 + 128;
	}

	/* Wipe the list of allocated buildings */
	(void)C_WIPE(build, MAX_CITY_BUILD, u16b);
	(void)C_WIPE(build_list, (WILD_BLOCK_SIZE * WILD_BLOCK_SIZE), u16b);

	/* Add town */
	select_town_name(pl_ptr->name, pop);
	pl_ptr->seed = randint0(0x10000000);

	pl_ptr->type = TOWN_FRACT;
	pl_ptr->monst_type = TOWN_MONST_VILLAGER;
	pl_ptr->x = x;
	pl_ptr->y = y;

	/* Save the population value in the 'data' value */
	pl_ptr->data = pop;

	/* Hack - the size is constant... */
	pl_ptr->xsize = 8;
	pl_ptr->ysize = 8;

	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant town layout */
	Rand_value = pl_ptr->seed;

	/* We don't have to save this in the town structure */
	magic = randint0(256);

	/* Generate plasma factal */
	clear_temp_block();
	set_temp_corner_val(WILD_BLOCK_SIZE * 64);
	set_temp_mid(WILD_BLOCK_SIZE * pop);
	frac_block();

	/* Locate the walls */
	find_walls();

	/* 'Fill' the town with buildings */
	count = fill_town_driver();

	/* Too few squares??? */
	if (count < 7) return (FALSE);

	/* Make sure the city is self-connected properly */
	remove_islands();

	/* Clear the gates locations */
	(void)C_WIPE(pl_ptr->gates_x, MAX_GATES, byte);
	(void)C_WIPE(pl_ptr->gates_y, MAX_GATES, byte);
	(void)C_WIPE(gate_num, MAX_GATES, byte);


	/* Initialise min and max values */
	gate_value[0] = 0;
	gate_value[1] = 255;
	gate_value[2] = 0;
	gate_value[3] = 255;

	/* Hack - save seed of rng */
	rng_seed_save = Rand_value;

	/*
	 * Link wilderness to the new city
	 * and find position of town gates.
	 */
	for (i = 0; i < WILD_BLOCK_SIZE; i++)
	{
		for (j = 0; j < WILD_BLOCK_SIZE; j++)
		{
			/* Is it a city block? */
			if (temp_block[j][i])
			{
				w_ptr = &wild[y + j / 2][x + i / 2].trans;

				/*
				 * Add city to wilderness
				 * Note: only 255 towns can be stored currently.
				 */
				w_ptr->place = (byte)town_num;

				/* Hack - make a flat area around the town */
				w_ptr->info |= WILD_INFO_ROAD;

				/* Right gate */
				if (gate_value[0] < i)
				{
					/* save it */
					gate_value[0] = i;
					gate_num[0] = 2;
					pl_ptr->gates_x[0] = i;
					pl_ptr->gates_y[0] = j;
				}
				else if ((gate_value[0] == i) && one_in_(gate_num[0]))
				{
					/* save it */
					gate_value[0] = i;
					gate_num[0]++;
					pl_ptr->gates_x[0] = i;
					pl_ptr->gates_y[0] = j;
				}

				/* Left gate */
				if (gate_value[1] > i)
				{
					/* save it */
					gate_value[1] = i;
					gate_num[1] = 2;
					pl_ptr->gates_x[1] = i;
					pl_ptr->gates_y[1] = j;
				}
				else if ((gate_value[1] == i) && one_in_(gate_num[1]))
				{
					/* save it */
					gate_value[1] = i;
					gate_num[1]++;
					pl_ptr->gates_x[1] = i;
					pl_ptr->gates_y[1] = j;
				}

				/* Bottom gate */
				if (gate_value[2] < j)
				{
					/* save it */
					gate_value[2] = j;
					gate_num[2] = 2;
					pl_ptr->gates_x[2] = i;
					pl_ptr->gates_y[2] = j;
				}
				else if ((gate_value[2] == j) && one_in_(gate_num[2]))
				{
					/* save it */
					gate_value[2] = j;
					gate_num[2]++;
					pl_ptr->gates_x[2] = i;
					pl_ptr->gates_y[2] = j;
				}

				/* Top gate */
				if (gate_value[3] > j)
				{
					/* save it */
					gate_value[3] = j;
					gate_num[3] = 2;
					pl_ptr->gates_x[3] = i;
					pl_ptr->gates_y[3] = j;
				}
				else if ((gate_value[3] == j) && one_in_(gate_num[3]))
				{
					/* save it */
					gate_value[3] = j;
					gate_num[3]++;
					pl_ptr->gates_x[3] = i;
					pl_ptr->gates_y[3] = j;
				}
			}
		}
	}

	/*
	 * Generate second fractal
	 */
	clear_temp_block();
	set_temp_corner_val(WILD_BLOCK_SIZE * 64);
	set_temp_mid(WILD_BLOCK_SIZE * law);
	frac_block();

	/* Restore the old seed */
	Rand_value = rng_seed_save;

	/* Save the total number of buildings */
	build_tot = count;

	/* Scan blocks in a random order */
	while (count)
	{
		/* Pick a square */
		i = randint0(count);

		/* Get parameters for the 8x8 section the building is on */
		pop = build_pop[i];
		law = temp_block[build_y[i]][build_x[i]] / WILD_BLOCK_SIZE;

		/*
		 * "place" building, and then record in the
		 * list of allocated buildings.
		 */
		building = select_building(pop, magic, law, build, build_tot);

		/* Count number of this type */
		build[building]++;

		/* Record list of created buildings */
		build_list[build_num++] = building;

		/*
		 * Decrement free space in city
		 * Note deliberate use of count-- in initialiser
		 */
		for (count--; i < count; i++)
		{
			/* Shift unallocated buildings down */
			build_pop[i] = build_pop[i + 1];
			build_x[i] = build_x[i + 1];
			build_y[i] = build_y[i + 1];
		}
	}

	/*
	 * Generate store and building data structures
	 *
	 * We need to do this second, because we need to
	 * know exactly how many stores we have - and realloc
	 * is silly, unless you need to use it.
	 */

	/* Allocate the stores */
	C_MAKE(pl_ptr->store, build_num, store_type);
	pl_ptr->numstores = build_num;

	/* Initialise the stores */
	for (i = 0; i < build_num; i++)
	{
		building = build_list[i];

		if (build_is_store(building))
		{
			/* Initialise the store */
			store_init(town_num, i, building);
		}
		else if (build_is_general(building))
		{
			/* Initialise general feature */
			general_init(town_num, i, building);
		}
		else
		{
			/* Initialise the building */
			build_init(town_num, i, building);
		}
	}

	/* Success */
	return (TRUE);
}


/*
 * Draw the gates to the city
 */
static void draw_gates(byte i, byte j, place_type *pl_ptr)
{
	int k;
	int x = i * 8, y = j * 8;
	int xx = x, yy = y;

	cave_type *c_ptr;

	/* Draw gates if visible */
	for (k = 0; k < MAX_GATES; k++)
	{
		if ((pl_ptr->gates_x[k] == i) && (pl_ptr->gates_y[k] == j))
		{
			/* Add doors (hack) */

			switch (k)
			{
				case 0:
				{
					/* Hack - shift gate if next to walls */
					if (cave_perma_grid(cave_p(x + 3, y + 2))) yy -= 3;
					if (cave_perma_grid(cave_p(x + 3, y + 5))) yy += 3;

					y = yy;

					/* Draw an empty square */
					generate_fill(x + 3, y + 3, x + 4, y + 4, FEAT_FLOOR);

					/* Right gate */
					c_ptr = cave_p(x + 4, y + 3);
					c_ptr->fld_idx = FT_LOCK_DOOR;
					set_feat_grid(c_ptr, FEAT_CLOSED);

					c_ptr = cave_p(x + 4, y + 4);
					c_ptr->fld_idx = FT_LOCK_DOOR;
					set_feat_grid(c_ptr, FEAT_CLOSED);

					return;
				}

				case 1:
				{
					/* Hack - shift gate if next to walls */
					if (cave_perma_grid(cave_p(x + 3, y + 2))) yy -= 3;
					if (cave_perma_grid(cave_p(x + 3, y + 5))) yy += 3;

					y = yy;

					/* Draw an empty square */
					generate_fill(x + 3, y + 3, x + 4, y + 4, FEAT_FLOOR);

					/* Left gate */
					c_ptr = cave_p(x + 3, y + 3);
					c_ptr->fld_idx = FT_LOCK_DOOR;
					set_feat_grid(c_ptr, FEAT_CLOSED);

					c_ptr = cave_p(x + 3, y + 4);
					c_ptr->fld_idx = FT_LOCK_DOOR;
					set_feat_grid(c_ptr, FEAT_CLOSED);

					return;
				}

				case 2:
				{
					/* Hack - shift gate if next to walls */
					if (cave_perma_grid(cave_p(x + 2, y + 3))) xx -= 3;
					if (cave_perma_grid(cave_p(x + 5, y + 3))) xx += 3;

					x = xx;

					/* Draw an empty square */
					generate_fill(x + 3, y + 3, x + 4, y + 4, FEAT_FLOOR);

					/* Bottom gate */
					c_ptr = cave_p(x + 3, y + 4);
					c_ptr->fld_idx = FT_LOCK_DOOR;
					set_feat_grid(c_ptr, FEAT_CLOSED);

					c_ptr = cave_p(x + 4, y + 4);
					c_ptr->fld_idx = FT_LOCK_DOOR;
					set_feat_grid(c_ptr, FEAT_CLOSED);

					return;
				}

				case 3:
				{
					/* Hack - shift gate if next to walls */
					if (cave_perma_grid(cave_p(x + 2, y + 3))) xx -= 3;
					if (cave_perma_grid(cave_p(x + 5, y + 3))) xx += 3;

					x = xx;

					/* Draw an empty square */
					generate_fill(x + 3, y + 3, x + 4, y + 4, FEAT_FLOOR);

					/* Top gate */
					c_ptr = cave_p(x + 3, y + 3);
					c_ptr->fld_idx = FT_LOCK_DOOR;
					set_feat_grid(c_ptr, FEAT_CLOSED);

					c_ptr = cave_p(x + 4, y + 3);
					c_ptr->fld_idx = FT_LOCK_DOOR;
					set_feat_grid(c_ptr, FEAT_CLOSED);

					return;
				}
			}
		}
	}
}


static void draw_store(int x0, int y0, store_type *st_ptr, int x, int y)
{
	int x1, y1, x2, y2;
	int i, j;
	int tmp;

	cave_type *c_ptr;

	/* Determine the store boundaries */
	y1 = y0 - randint1(3);
	y2 = y0 + randint1(2);
	x1 = x0 - randint1(3);
	x2 = x0 + randint1(3);

	/* Build an invulnerable rectangular building */
	generate_fill(x1, y1, x2, y2, FEAT_PERM_EXTRA);

	/* Pick a door direction (S,N,E,W) */
	tmp = randint0(4);

	/* Extract a "door location" */
	switch (tmp)
	{
		case 0:
		{
			/* Bottom side */
			i = rand_range(x1, x2);
			j = y2;
			break;
		}

		case 1:
		{
			/* Top side */
			i = rand_range(x1, x2);
			j = y1;
			break;
		}

		case 2:
		{
			/* Right side */
			i = x2;
			j = rand_range(y1, y2);
			break;
		}

		default:
		{
			/* Left side */
			i = x1;
			j = rand_range(y1, y2);
			break;
		}
	}

	c_ptr = cave_p(i, j);

	/* Clear previous contents, add a store door */
	set_feat_grid(c_ptr, FEAT_FLOOR);

	c_ptr->fld_idx = wild_build[st_ptr->type].field;

	/* Save location of store door */
	st_ptr->x = x * 8 + i % 8;
	st_ptr->y = y * 8 + j % 8;
}


static void draw_general(int x0, int y0, store_type *st_ptr, int x, int y)
{
	int i, j;

	/* Ignore currently unused parameters */
	(void)x;
	(void)y;

	switch (st_ptr->type)
	{
		case BUILD_STAIRS:
		{
			/* Put dungeon floor next to stairs so they are easy to find. */
			for (i = -1; i <= 1; i++)
			{
				for (j = -1; j <= 1; j++)
				{
					/* Convert square to dungeon floor */
					set_feat_bold(x0 + i, y0 + j, FEAT_FLOOR);
				}
			}

			/* Clear previous contents, add down stairs */
			set_feat_bold(x0, y0, FEAT_MORE);

			break;
		}

		case BUILD_NONE:
		{
			int x1, y1, x2, y2;

			/* Determine the store boundaries */
			y1 = y0 - randint1(3);
			y2 = y0 + randint1(2);
			x1 = x0 - randint1(3);
			x2 = x0 + randint1(3);

			/* Build an invulnerable rectangular building */
			generate_fill(x1, y1, x2, y2, FEAT_PERM_EXTRA);

			/* No doors */

			/* break; Restore this when we do something for BUILD_BLANK */
		}

		case BUILD_BLANK:
		{
			/* Do Nothing */

			break;
		}
	}
}


/*
 * Draw a building / store of a given type at a given position
 */
static void draw_building(byte type, byte x, byte y, u16b store, u16b town_num)
{
	/* Really dodgy - just a rectangle, independent of type, for now */
	int xx, yy;

	/* Hack - save the rng seed */
	u32b rng_save_seed = Rand_value;

	store_type *st_ptr = &place[town_num].store[store];

	/* Hack, ignore building draw type for now */
	(void)type;

	/* Get location in region */
	xx = x * 8;
	yy = y * 8;

	/* Hack - set location of stairs so we can start on them. */
	if (st_ptr->type == BUILD_STAIRS)
	{
		wild_stairs_x = xx + 4;
		wild_stairs_y = yy + 4;
	}

	/* What are we drawing? */
	if (build_is_store(st_ptr->type))
	{
		/* Draw the store */
		draw_store(xx + 4, yy + 4, st_ptr, x, y);
	}
	else if (build_is_general(st_ptr->type))
	{
		/* Draw the general feature */
		draw_general(xx + 4, yy + 4, st_ptr, x, y);
	}
	else
	{
		/* Hack - Draw the "normal" building */
		draw_store(xx + 4, yy + 4, st_ptr, x, y);
	}

	/* Hack - restore the rng seed */
	Rand_value = rng_save_seed;
}


/* Actually draw the city in the region */
void draw_city(u16b town_num)
{
	int x, y;
	int count = 0;
	byte i, j;
	byte magic;
	u16b build;


	place_type *pl_ptr = &place[town_num];

	/* Paranoia */
	if (pl_ptr->region) quit("Town already has region during creation.");

	/* Get region */
	create_region(pl_ptr, pl_ptr->xsize * WILD_BLOCK_SIZE,
						pl_ptr->ysize * WILD_BLOCK_SIZE,
						REGION_NULL);

	/* Hack - do not increment refcount here - let allocate_block do that */

	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant town layout */
	Rand_value = pl_ptr->seed;

	/* Get value of "magic" level of buildings */
	magic = (byte)randint0(256);

	/* Generate plasma factal */
	clear_temp_block();
	set_temp_corner_val(WILD_BLOCK_SIZE * 64);

	/* Use population value saved in data. */
	set_temp_mid((u16b)(WILD_BLOCK_SIZE * place[town_num].data));
	frac_block();

	/* Locate the walls */
	find_walls();

	/* 'Fill' the town with buildings */
	count = fill_town_driver();

	/* Make sure the city is self-connected properly */
	remove_islands();

	/* Draw walls */
	for (i = 0; i < WILD_BLOCK_SIZE; i++)
	{
		for (j = 0; j < WILD_BLOCK_SIZE; j++)
		{
			/* Are we a wall? */
			if (temp_block[j][i] == 1)
			{
				/* Get coords in region */
				y = j * 8;
				x = i * 8;


				/* Wall goes up */
				if ((j > 0) && (temp_block[j - 1][i] == 1))
				{
					generate_fill(x + 3, y, x + 4, y + 4, FEAT_PERM_SOLID);
				}

				/* Wall goes left */
				if ((i > 0) && (temp_block[j][i - 1] == 1))
				{
					generate_fill(x, y + 3, x + 4, y + 4, FEAT_PERM_SOLID);
				}

				/* Wall goes right */
				if ((i < WILD_BLOCK_SIZE - 1) && (temp_block[j][i + 1] == 1))
				{
					generate_fill(x + 3, y + 3, x + 7, y + 4, FEAT_PERM_SOLID);
				}

				/* Wall goes down */
				if ((j < WILD_BLOCK_SIZE - 1) && (temp_block[j + 1][i] == 1))
				{
					generate_fill(x + 3, y + 3, x + 4, y + 7, FEAT_PERM_SOLID);
				}

				/* Draw the gates */
				draw_gates(i, j, pl_ptr);
			}
		}
	}

	/* Scan blocks in a random order */
	for (build = 0; count; build++)
	{
		/* Pick a square */
		i = (byte)randint0(count);

		/* Draw the building */
		draw_building(0, build_x[i], build_y[i], build, town_num);

		/*
		 * Decrement free space in city
		 * Note deliberate use of count-- in initialiser
		 */
		for (count--; i < count; i++)
		{
			/* Shift unallocated buildings down */
			build_x[i] = build_x[i + 1];
			build_y[i] = build_y[i + 1];
		}
	}

	/* Hack -- use the "complex" RNG */
	Rand_quick = FALSE;
}


/*
 * Draw a (generic) dungeon entrance on a region
 *
  * This is really crappy... but will be improved soonish.
 */
void draw_dungeon(u16b place_num)
{
	int x, y;
	int i, j;

	place_type *pl_ptr = &place[place_num];

	/* Paranoia */
	if (pl_ptr->region) quit("Dungeon entrance already has region during creation.");

	/* Get region */
	create_region(pl_ptr, WILD_BLOCK_SIZE, WILD_BLOCK_SIZE, REGION_NULL);

	/* Hack - do not increment refcount here - let allocate_block do that */

	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant layout */
	Rand_value = pl_ptr->seed;
	
	/* Get location of stairs */
	x = randint1(14);
	y = randint1(14);
	
	/* Put dungeon floor next to stairs so they are easy to find. */
	for (i = -1; i <= 1; i++)
	{
		for (j = -1; j <= 1; j++)
		{
			/* Convert square to dungeon floor */
			set_feat_bold(x + i, y + j, FEAT_FLOOR);
		}
	}

	/* Add down stairs */
	set_feat_bold(x, y, FEAT_MORE);

	/* Hack -- use the "complex" RNG */
	Rand_quick = FALSE;
}


/*
 * Look to see if a wilderness block is able to have
 * a town/dungeon overlayed on top.
 */
static bool blank_spot(int x, int y, int xsize, int ysize, int town_num, bool town)
{
	int i, j;
	wild_gen2_type *w_ptr;
	
	int dist;

	/* Hack - Population check */
	if (randint0(256) > wild[y][x].trans.pop_map) return (FALSE);

	for (i = x - 1; i < x + xsize + 2; i++)
	{
		for (j = y - 1; j < y + ysize + 2; j++)
		{
			/* Hack - Not next to boundary */
			if ((i <= 0) || (i >= max_wild - 1) ||
				(j <= 0) || (j >= max_wild - 1))
			{
				return (FALSE);
			}

			w_ptr = &wild[j][i].trans;

			/* No place already */
			if (w_ptr->place) return (FALSE);

			/* No water or lava or acid */
			if (w_ptr->info & (WILD_INFO_WATER | WILD_INFO_LAVA | WILD_INFO_ACID))
				return (FALSE);

			/* No Ocean */
			if (w_ptr->hgt_map < (256 / SEA_FRACTION)) return (FALSE);
		}
	}

	if (town)
	{
		dist = MIN_DIST_TOWN;
	}
	else
	{
		dist = MIN_DIST_DUNGEON;
	}
	
	/* Look to see if another place is too close */
	for (i = 1; i < town_num; i++)
	{ 
		if (distance(place[i].x, place[i].y, x, y) < dist)
		{
			/* Too close? */
			return (FALSE);
		}
	}

	/* Ok then */
	return (TRUE);
}

#define DUN_LIST_NUM		12

/*
 * A few dungeon types.
 *
 */
static const dun_gen_type dungeons[] =
{
	{{0, 10, 0, 40}, RF7_DUN_DARKWATER, 0, 15, 0, 1,
		100, 0,
		RT_SIMPLE | RT_NATURAL | RT_ANIMAL | RT_STRANGE,
		FEAT_DRY_MUD,
		LQ_WATER | LQ_SWAMP,
		DF_TRACK | DF_ROAD},

	{{50, 10, 10, 0}, RF7_DUN_LAIR, 10, 50, 10, 1,
		100, 100,
		RT_NATURAL | RT_COMPLEX | RT_RUIN,
		FEAT_DIRT,
		LQ_WATER | LQ_ACID | LQ_SWAMP,
		DF_NONE},

	{{10, 30, 30, 30}, RF7_DUN_TEMPLE, 20, 60, 20, 1,
		250, 250,
		RT_SIMPLE | RT_COMPLEX | RT_DENSE | RT_FANCY | RT_BUILDING | RT_CRYPT,
		FEAT_FLOOR_TILE,
		LQ_WATER | LQ_LAVA,
		DF_ROAD},

	{{20, 0, 80, 0}, RF7_DUN_TOWER, 20, 60, 10, 1,
		250, 200,
		RT_SIMPLE | RT_COMPLEX | RT_BUILDING | RT_RVAULT,
		FEAT_FLOOR_WOOD,
		LQ_ACID | LQ_LAVA,
		DF_TRACK},

	{{10, 20, 20, 0}, RF7_DUN_RUIN, 20, 80, 10, 1,
		0, 150,
		RT_RUIN,
		FEAT_PEBBLES,
		LQ_WATER | LQ_LAVA | LQ_SWAMP,
		DF_TRACK | DF_ROAD},

	{{50, 20, 20, 0}, RF7_DUN_GRAVE, 30, 100, 25, 1,
		50, 150,
		RT_COMPLEX | RT_FANCY | RT_CRYPT,
		FEAT_FLOOR_TILE,
		LQ_WATER | LQ_SWAMP,
		DF_TRACK | DF_ROAD},

	{{30, 30, 30, 10}, RF7_DUN_CAVERN, 40, 80, 0, 1,
		50, 200,
		RT_SIMPLE | RT_ANIMAL | RT_DENSE | RT_RUIN | RT_RVAULT,
		FEAT_DIRT,
		LQ_WATER | LQ_ACID | LQ_LAVA,
		DF_TRACK},

	{{30, 30, 40, 0}, RF7_DUN_PLANAR, 40, 127, 0, 1,
		0, 250,
		RT_COMPLEX | RT_DENSE | RT_FANCY | RT_RVAULT,
		FEAT_SAND,
		LQ_ACID | LQ_LAVA,
		DF_TRACK},

	{{20, 40, 40, 0}, RF7_DUN_HELL, 60, 127, 0, 1,
		0, 0,
		RT_SIMPLE | RT_NATURAL | RT_ANIMAL | RT_DENSE | RT_RUIN |
		RT_FANCY | RT_RVAULT | RT_STRANGE,
		FEAT_SOLID_LAVA,
		LQ_LAVA,
		DF_TRACK},

	{{0, 20, 20, 0}, RF7_DUN_HORROR, 80, 127, 0, 1,
		0, 150,
		RT_SIMPLE | RT_NATURAL | RT_ANIMAL | RT_DENSE | RT_RUIN | RT_STRANGE,
		FEAT_SALT,
		LQ_ACID,
		DF_TRACK},

	{{10, 20, 10, 40}, RF7_DUN_MINE, 0, 40, 10, 1,
		200, 200,
		RT_SIMPLE | RT_NATURAL | RT_ANIMAL | RT_RUIN | RT_STRANGE,
		FEAT_DIRT,
		LQ_WATER | LQ_LAVA,
		DF_ROAD},

	{{30, 30, 10, 10}, RF7_DUN_CITY, 20, 60, 10, 1,
		200, 200,
		RT_SIMPLE | RT_COMPLEX | RT_DENSE | RT_FANCY | RT_BUILDING |
		RT_CRYPT | RT_RVAULT | RT_STRANGE,
		FEAT_FLOOR_TILE,
		LQ_WATER,
		DF_TRACK | DF_ROAD},

	{{0, 0, 0, 0}, 0, 0, 0, 0, 0,
		0, 0,
		0,
		FEAT_NONE,
		LQ_NONE,
		DF_NONE},
};


/*
 * Pick a type of dungeon from the above list
 */
const dun_gen_type *pick_dungeon_type(void)
{
	int tmp, total;

	const dun_gen_type *d_ptr;

	/* Calculate the total possibilities */
	for (d_ptr = dungeons, total = 0; d_ptr->habitat; d_ptr++)
	{
		/* Count this possibility */
		if (d_ptr->min_level > p_ptr->depth) continue;

		/* Normal selection */
		total += d_ptr->chance * MAX_DEPTH * 10 /
				(p_ptr->depth - d_ptr->min_level + 5);
	}

	/* Pick a random type */
	tmp = randint0(total);

	/* Find this type */
	for (d_ptr = dungeons, total = 0; d_ptr->habitat; d_ptr++)
	{
		/* Count this possibility */
		if (d_ptr->min_level > p_ptr->depth) continue;
		
		total += d_ptr->chance * MAX_DEPTH * 10 /
			(p_ptr->depth - d_ptr->min_level + 5);

		/* Found the type */
		if (tmp < total) break;
	}

	/* Return the index of the chosen dungeon */
	return (d_ptr);
}


/* Save dungeon information so we know what to build later */
static void init_dungeon(place_type *pl_ptr, const dun_gen_type *d_ptr)
{
	dun_type *dt_ptr;

	/* Create it */
	MAKE(pl_ptr->dungeon, dun_type);

	dt_ptr = pl_ptr->dungeon;

	/* Set the object theme (structure copy) */
	dt_ptr->theme = d_ptr->theme;
	
	/* Hack - Reset the dungeon habitat to be everything */
	dt_ptr->habitat = d_ptr->habitat;
	
	/* Dungeon is full range of levels? */
	if (!d_ptr->dif_level)
	{
		/* Save level bounds */
		dt_ptr->min_level = d_ptr->min_level;
		dt_ptr->max_level = d_ptr->max_level;
	}
	else
	{
		/* Get range of levels */
		int range = Rand_normal(d_ptr->dif_level, d_ptr->dif_level / 2);
	
		/* Make sure fits inside bounds */
		if (range > d_ptr->max_level - d_ptr->min_level)
		{
			/* Save level bounds */
			dt_ptr->min_level = d_ptr->min_level;
			dt_ptr->max_level = d_ptr->max_level;
		}
		else
		{
			/* Fit dungeon within level bounds (randomly) */
			dt_ptr->min_level = rand_range(d_ptr->min_level, d_ptr->max_level - range);
			dt_ptr->max_level = dt_ptr->min_level + range;
		}
	}
	
	/* Copy dungeon creation info */
	dt_ptr->rooms = d_ptr->rooms;
	dt_ptr->floor = d_ptr->floor;
	dt_ptr->liquid = d_ptr->liquid;
	
	/* Extra flags */
	dt_ptr->flags = d_ptr->flags;
}


/* Hack - return the current type of "floor" */
byte the_floor(void)
{
	/* In the wilderness */
	if (!p_ptr->depth) return (FEAT_DIRT);
	
	/* In the dungeon */
	return (place[p_ptr->place_num].dungeon->floor);
}


static bool create_towns(int xx, int yy)
{
	int x, y, i;
	bool first_try = TRUE;

	wild_gen2_type *w_ptr;

	place_type *pl_ptr;

	/* Variables to pick "easiest" town. */
	u16b best_town = 0, town_value = 0;

	/*
	 * Try to add z_info->wp_max towns.
	 */
	while (place_count < NUM_TOWNS)
	{
		if (first_try)
		{
			/* Try the "easiest" spot in the wilderness */
			x = xx;
			y = yy;

			/* Only try once here */
			first_try = FALSE;
		}
		else
		{
			/* Get a random position */
			x = randint0(max_wild);
			y = randint0(max_wild);
		}

		/*
		 * See if a city will fit.
		 * (Need a 8x8 block free.)
		 */
		if (!blank_spot(x, y, 8, 8, place_count, TRUE)) continue;

		/* Generate it */
		if (!create_city(x, y, place_count)) continue;
		
		/* get wildernesss + place pointers */
		w_ptr = &wild[y][x].trans;
		pl_ptr = &place[place_count];
				
		/* Check to see if the town has stairs */
		for (i = 0; i < pl_ptr->numstores; i++)
		{
			if (pl_ptr->store[i].type == BUILD_STAIRS)
			{
				/* Create dungeon information */
				if (!pl_ptr->dungeon)
				{
					/* Use sewer */
					init_dungeon(pl_ptr, &dungeons[0]);
				}
				
				/* Select easiest town */
				if (w_ptr->law_map > town_value)
				{
					/* Save this town */
					town_value = w_ptr->law_map;
					best_town = place_count;

					/* Done */
					continue;
				}
			}
		}
		
		/* Increment number of places */
		place_count++;		
	}
	
	/* Paranoia */
	if (!best_town) return (FALSE);
	
	/* Hack - the starting town uses pre-defined stores */
	for (i = 0; i < place[best_town].numstores; i++)
	{
		if (i == 0)
		{
			/* Hack - make stairs */
			store_init(best_town, i, wild_first_town[i]);
		}
		else if (i < START_STORE_NUM)
		{
			if (build_is_store(wild_first_town[i]))
			{
				/* Hack - use the pre-defined stores */
				store_init(best_town, i, wild_first_town[i]);
			}
			else
			{
				build_init(best_town, i, wild_first_town[i]);
			}
		}
		else
		{
			/* Blank spot */
			general_init(best_town, i, BUILD_NONE);
		}
	}

	/* Build starting city / town */
	draw_city(best_town);

	place_player_start(&p_ptr->wilderness_x, &p_ptr->wilderness_y, best_town);

	/* Hack - No current region */
	set_region(0);

	/* Success */
	return (TRUE);
}


/*
 * What is the "score" for a dungeon of the given type
 * at this location in the wilderness?
 * The lower the score, the better the match.
 */
static long score_dungeon(const wild_gen2_type *w_ptr, const dun_gen_type *d_ptr)
{
	long score = 0, value;
	
	/* Height */
	value = w_ptr->hgt_map - d_ptr->height;
	score += value * value;
	
	/* Population */
	value = w_ptr->pop_map - d_ptr->pop;
	score += value * value;
	
	/* Lawless level */
	value = w_ptr->law_map - d_ptr->min_level;
	score += value * value;
	
	return (score);
}



/* Add in dungeons into the wilderness */
static void create_dungeons(void)
{
	int i, j;
	
	int x, y;
	
	int best;
	
	long best_val, score;
	
	place_type *pl_ptr;
	
	wild_gen2_type *w_ptr;
	
	int dungeon_list[NUM_DUNGEON];
	
	/*
	 * Scan for places to add dungeons.
	 */
	while (place_count < NUM_TOWNS + NUM_DUNGEON)
	{
		/* Get a random position */
		x = randint0(max_wild);
		y = randint0(max_wild);
		
		pl_ptr = &place[place_count];

		/*
		 * See if a dungeon will fit.
		 * (Need a 1x1 block free.)
		 */
		if (!blank_spot(x, y, 1, 1, place_count, FALSE)) continue;
		
		/* Get location */
		w_ptr = &wild[y][x].trans;

		/*
		 * Add city to wilderness
		 * Note: only 255 towns can be stored currently.
		 */
		w_ptr->place = (byte)place_count;
				
		pl_ptr->x = x;
		pl_ptr->y = y;
		pl_ptr->xsize = 1;
		pl_ptr->ysize = 1;
		
		/* We are a dugneon */
		pl_ptr->type = TOWN_DUNGEON;
		
		/* We have monsters */
		pl_ptr->monst_type = TOWN_MONST_ABANDONED;
		
		/* Hack - A really crap name */
		strcpy(pl_ptr->name, "Dungeon");

		/* Increment number of places */
		place_count++;
	}

	/* Select list of dungeon types to use */
	for (i = 0; i <	NUM_DUNGEON; i++)
	{
		/* Make sure we have at least one of each */
		if (i < DUN_LIST_NUM)
		{
			dungeon_list[i] = i;
		}
		else
		{
			dungeon_list[i] = randint0(DUN_LIST_NUM);
		}	
	}
	
	/* Match available dungeon types to locations */
	for (i = 0; i < NUM_DUNGEON; i++)
	{
		/* Score each available location, and pick the best one. */
		
		best = -1;
		best_val = -1;
		
		for (j = NUM_TOWNS; j < NUM_TOWNS + NUM_DUNGEON; j++)
		{
			pl_ptr = &place[j];
			
			/* Skip already created dungeons */
			if (pl_ptr->dungeon) continue;
			
			/* Get location */
			w_ptr = &wild[pl_ptr->y][pl_ptr->x].trans;
			
			score = score_dungeon(w_ptr, &dungeons[dungeon_list[i]]);
			
			/* Better dungeon? */
			if ((best == -1) || (score < best_val))
			{
				best = j;
				best_val = score;
			}
		}
		
		/* Initialise best dungeon */
		init_dungeon(&place[best], &dungeons[dungeon_list[i]]);
	}
}


/*
 * Place the quests on the wilderness
 */
static void create_quests(void)
{
	int x, y;

	int xsize, ysize;
	byte flags;

	/*
	 * Try to add z_info->wp_max towns.
	 */
	while (place_count < z_info->wp_max)
	{
		/* Get a random position */
		x = randint0(max_wild);
		y = randint0(max_wild);
	
		/* Pick quest size / type */
		pick_wild_quest(&xsize, &ysize, &flags);

		/* See if a quest will fit */
		if (!quest_blank(x, y, xsize, ysize, place_count, flags)) continue;

		/* Build it */
		if (!create_quest(x, y, place_count)) continue;

		/* Increment number of places */
		place_count++;
	}
}


/*
 * Initialise the place structures
 *
 * There are currently, cities and quests.
 *
 * Soon there will be:
 * Ruins, barracks, towers etc.
 */
bool init_places(int xx, int yy)
{
	/* No towns yet */
	place_count = 1;
	
	/* Create towns */
	if (!create_towns(xx, yy)) return (FALSE);

	/* Create dungeons */
	create_dungeons();
	
	/* Create quests */
	create_quests();

	/* Done */
	return (TRUE);
}


/*
 * Builds a store at a given pseudo-location
 *
 * As of Z 2.5.0 the town is moved back to (0,0) - and is overlayed
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
static void build_store(int xx, int yy, store_type *st_ptr)
{
	int y, x, y0, x0, y1, x1, y2, x2, tmp;

	cave_type *c_ptr;

	/* Find the "center" of the store */
	y0 = yy * 6 + 4;
	x0 = xx * 16 + 8;

	/* Determine the store boundaries */
	y1 = y0 - randint1(2);
	y2 = y0 + randint1(2);
	x1 = x0 - randint1(5);
	x2 = x0 + randint1(5);

	/* Build an invulnerable rectangular building */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			/* Create the building */
			set_feat_bold(x, y, FEAT_PERM_EXTRA);
		}
	}

	/* Pick a door direction (S,N,E,W) */
	tmp = randint0(4);

	/* Re-roll "annoying" doors */
	if (((tmp == 0) && (yy == 2)) ||
		((tmp == 1) && (yy == 0)) ||
		((tmp == 2) && (xx == 2)) || ((tmp == 3) && (xx == 0)))
	{
		/* Pick a new direction */
		tmp = randint0(4);
	}

	/* Extract a "door location" */
	switch (tmp)
	{
		case 0:
		{
			/* Bottom side */
			y = y2;
			x = rand_range(x1, x2);
			break;
		}

		case 1:
		{
			/* Top side */
			y = y1;
			x = rand_range(x1, x2);
			break;
		}

		case 2:
		{
			/* Right side */
			y = rand_range(y1, y2);
			x = x2;
			break;
		}

		default:
		{
			/* Left side */
			y = rand_range(y1, y2);
			x = x1;
			break;
		}
	}

	c_ptr = cave_p(x, y);

	/* Clear previous contents, add a store door */
	set_feat_grid(c_ptr, FEAT_FLOOR);
	c_ptr->fld_idx = wild_build[st_ptr->type].field;

	/* Save location of store door */
	st_ptr->x = x;
	st_ptr->y = y;
}


/*
 * Generate the "consistent" town features, and place the player
 *
 * Hack -- play with the R.N.G. to always yield the same town
 * layout, including the size and shape of the buildings, the
 * locations of the doorways, and the location of the stairs.
 *
 * This simple routine does not check the type of stores town_num wants.
 */
static void town_gen_hack(u16b town_num)
{
	int y, x, k, n, xx, yy;

	/* Add an extra column to make it symmetrical */
	int rooms[3 * 4];

	cave_type *c_ptr;

	/* Prepare an array of "remaining stores", and count them */
	for (n = 0; n < 3 * 4; n++) rooms[n] = n;

	/* Place three rows of stores */
	for (y = 0; y < 3; y++)
	{
		/* Place four stores per row */
		for (x = 0; x < 4; x++)
		{
			/* Pick a random unplaced store */
			k = ((n <= 1) ? 0 : randint0(n));

			/* Only build real stores */
			if (rooms[k] < MAX_STORES)
			{
				/* Build that store at the proper location */
				build_store(x, y, &place[town_num].store[rooms[k]]);
			}

			/* Shift the stores down, remove one store */
			rooms[k] = rooms[--n];
		}
	}


	/* Place the stairs */
	while (TRUE)
	{
		/* Pick a location at least "three" from the outer walls */
		yy = rand_range(3, TOWN_HGT - 4);
		xx = rand_range(3, TOWN_WID - 4);

		c_ptr = cave_p(xx, yy);

		/* If square is a shop then try again */
		if (!cave_naked_grid(c_ptr)) continue;

		/* Blank square */
		break;
	}

	/* Put dungeon floor next to stairs so they are easy to find. */
	for (y = -1; y <= 1; y++)
	{
		for (x = -1; x <= 1; x++)
		{
			c_ptr = cave_p(xx + x, yy + y);

			if (!cave_naked_grid(c_ptr)) continue;

			/* Convert square to dungeon floor */
			set_feat_grid(c_ptr, FEAT_FLOOR);
		}
	}

	/* Clear previous contents, add down stairs */
	set_feat_bold(xx, yy, FEAT_MORE);

	wild_stairs_x = xx;
	wild_stairs_y = yy;
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
 * xx and yy point to the location of the stairs (So the player can
 * start there.)
 *
 * (Vanilla town only now.)
 */
void van_town_gen(u16b town_num)
{
	int y, x;

	cave_type *c_ptr;

	place_type *pl_ptr = &place[town_num];

	/* Paranoia */
	if (pl_ptr->region) quit("Town already has region during creation.");

	/* Get region */
	create_region(pl_ptr, V_TOWN_BLOCK_WID, V_TOWN_BLOCK_HGT, REGION_NULL);

	/* Hack - do not increment refcount here - let allocate_block do that */

	/* Place transparent area */
	for (y = 0; y < V_TOWN_BLOCK_HGT; y++)
	{
		for (x = 0; x < V_TOWN_BLOCK_WID; x++)
		{
			c_ptr = cave_p(x, y);

			/* Create empty area */
			set_feat_grid(c_ptr, FEAT_PERM_EXTRA);
		}
	}

	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant town layout */
	Rand_value = place[town_num].seed;

	/* Place some floors */
	for (y = 1; y < TOWN_HGT - 1; y++)
	{
		for (x = 1; x < TOWN_WID - 1; x++)
		{
			/* Create see-through terrain */
			set_feat_bold(x, y, FEAT_FLOOR);
		}
	}

	/* Build stuff */
	town_gen_hack(town_num);

	/* Hack -- use the "complex" RNG */
	Rand_quick = FALSE;
}


/*
 * Place a single town in the middle of the tiny wilderness
 */
void init_vanilla_town(void)
{
	int i, j;

	place_type *pl_ptr = &place[1];

	dun_type *d_ptr;

	/* Only one town */
	strcpy(pl_ptr->name, "Town");
	pl_ptr->seed = randint0(0x10000000);
	pl_ptr->numstores = 9;
	pl_ptr->type = TOWN_OLD;
	pl_ptr->x = (max_wild / 2) - TOWN_WID / (WILD_BLOCK_SIZE * 2) - 1;
	pl_ptr->y = (max_wild / 2) - TOWN_HGT / (WILD_BLOCK_SIZE * 2) - 1;
	pl_ptr->xsize = V_TOWN_BLOCK_WID / WILD_BLOCK_SIZE;
	pl_ptr->ysize = V_TOWN_BLOCK_HGT / WILD_BLOCK_SIZE;

	/* Allocate the stores */
	C_MAKE(place[1].store, MAX_STORES, store_type);

	/* Init the stores */
	for (i = 0; i < MAX_STORES; i++)
	{
		/* Initialize */
		store_init(1, i, (byte)i);
	}

	/* Place town on wilderness */
	for (i = pl_ptr->x; i < pl_ptr->x + pl_ptr->xsize; i++)
	{
		for (j = pl_ptr->y; j < pl_ptr->y + pl_ptr->ysize; j++)
		{
			wild[j][i].done.place = 1;
		}
	}
	
	/* Create dungeon */
	MAKE(pl_ptr->dungeon, dun_type);
	
	d_ptr = pl_ptr->dungeon;
	
	/* Set dungeon depths */
	d_ptr->max_level = MAX_DEPTH - 1;
	d_ptr->min_level = 1;

	/* Make the town - and get the location of the stairs */
	van_town_gen(1);

	place_player_start(&p_ptr->wilderness_x, &p_ptr->wilderness_y, 1);

	/* One town + 1 for bounds */
	place_count = 2;

	/* Hack - set global region back to wilderness value */
	set_region(0);
}




