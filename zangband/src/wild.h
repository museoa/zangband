/* File: wild1.h */

/* Purpose: Wilderness generation header file */

/*
 * Copyright (c) 1989, 1999 James E. Wilson, Robert A. Koeneke,
 * Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/* 1/4 of the wilderness is sea */
#define	SEA_FRACTION	4

/* Number of lakes to try and make */
#define LAKE_NUM		4

/* Constant^2 that determines number of rivers */
#define RIVER_NUM		4

/* Maximum distance a road can connect */
#define ROAD_DIST		30

/* Minimum fractional distance a road can approach a non-connecting town */
#define ROAD_MIN		3



/* List of building types */
#define	BUILD_STORE_GENERAL		0
#define BUILD_STORE_ARMOURY		1
#define BUILD_STORE_WEAPON		2
#define BUILD_STORE_TEMPLE		3
#define BUILD_STORE_ALCHEMIST	4
#define BUILD_STORE_MAGIC		5
#define BUILD_STORE_BLACK		6
#define BUILD_STORE_HOME		7
#define BUILD_STORE_BOOK		8
#define BUILD_STAIRS			9

/* Maximum number of "building" types in a city */
#define MAX_CITY_BUILD			10

/* Dodgy replacement for SCREEN_WID and SCREEN_HGT */

/* This will be removed later. */
#define TOWN_WID	66
#define TOWN_HGT	22


/* Road constants used to define with of the path */
#define ROAD_LEVEL		(WILD_BLOCK_SIZE * 150)
#define TRACK_LEVEL		(WILD_BLOCK_SIZE * 140)
#define ROAD_BORDER		(WILD_BLOCK_SIZE * 120)
#define GROUND_LEVEL	(WILD_BLOCK_SIZE * 100)

/* Externs */
extern bool build_is_general(byte type);
extern void clear_temp_block(void);
extern void set_temp_corner_val(u16b val);
extern void set_temp_mid(u16b val);
extern void frac_block(void);
extern bool town_blank(int x, int y, int xsize, int ysize);
extern void draw_city(u16b town_num);
extern void van_town_gen(u16b town_num);
extern void init_wild_cache(void);



