/* File: maid-grf.h */

/* Purpose: Common header file for graphics ports */

/*
 * Copyright (c) 2002 Steven Fuerst
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/*
 * Number of blocks in the overhead map cache.
 * 
 * This number must be bigger than the number of blocks
 * required to remember the biggest dungeon level.
 */
#define MAP_CACHE	(WILD_VIEW * WILD_VIEW * 2)

/*
 * This is used by the Borg and by ports that like to
 * draw a "graphical" small-scale map
 */
#ifdef TERM_USE_MAP

/*
 * Constants used to pass information to users
 * of the overhead map hooks.
 */
#define MAP_SEEN	0x01		/* GRID_SEEN equivalent */
#define MAP_GLOW	0x02		/* CAVE_GLOW equivalent */
#define MAP_LITE	0x04		/* GRID_LITE equivalent */
#define MAP_ONCE    0x08		/* This block has ever been seen */

/*
 * Constants used to describe the state of monsters
 * to users of the overhead map
 */
#define MONST_ASLEEP	0x01
#define MONST_FRIEND	0x02
#define MONST_PET		0x04
#define MONST_CONFUSED	0x08
#define MONST_FEAR		0x10
#define MONST_STUN		0x20
#define MONST_INVULN	0x40


/*
 * Make an iterator, so we can scan the map quickly
 */
#define MAP_ITT_START(M) \
	{ \
		int _map_count;\
		\
		for (_map_count = 0; _map_count < MAP_CACHE; _map_count++) \
		{ \
			int _map_i, _map_j; \
			\
			if (map_cache_x[_map_count] == -1) continue; \
			\
			if (map_grid[map_cache_y[_map_count]][map_cache_x[_map_count]] == -1)\
				 continue; \
			\
			for (_map_i = 0; _map_i < WILD_BLOCK_SIZE; _map_i++) \
			{ \
				for (_map_j = 0; _map_j < WILD_BLOCK_SIZE; _map_j++) \
				{ \
					(M) = &map_cache[_map_count][_map_j][_map_i];


#define MAP_ITT_END \
				} \
			} \
		} \
	}

/* Macro to extract location during iteration */
#define MAP_GET_LOC(X, Y)\
	(((X) = _map_i + map_cache_x[_map_count] * WILD_BLOCK_SIZE), \
	 ((Y) = _map_j + map_cache_y[_map_count] * WILD_BLOCK_SIZE))

/*
 * Map data structure
 */
typedef struct term_map term_map;

struct term_map
{
	/* Information about what is at the grid */
	s16b object;
	s16b monster;
	s16b field;
	byte terrain;
	char unknown;

	/* Location */
	u16b x;
	u16b y;

	/* Player-known flags */
	byte flags;

	/* Monster flags */
	byte m_flags;

	/* Rough measure of monster hp */
	byte m_hp;
};

typedef struct map_block map_block;

struct map_block
{
	/* Save what it looks like */

#ifdef TERM_OVERHEAD_MAP
	u16b feature_code;
#endif /* TERM_OVERHEAD_MAP */

	/* Save the cave info itself - used by the borg */
#ifdef TERM_CAVE_MAP
	s16b object;
	s16b monster;
	s16b field;
	byte terrain;

	/* unknown mimics or flavoured objects */
	char unknown;

	/* Monster flags */
	byte m_flags;

	/* Monster hp (scaled) */
	byte m_hp;
#endif /* TERM_CAVE_MAP */

	/* Borg-specific stuff */
#ifdef ALLOW_BORG
	u16b fear;	/* fear value */

	u16b kill;	/* Entry into "kill" list */
	u16b take;	/* Entry into "take" list */

	byte feat;	/* Terrain feature */
	byte info;	/* info flags */

	byte flow;	/* "flow" data */
	byte cost;	/* "cost" data */

	byte detect;	/* Detection flags */
	byte xtra;	/* search count */
#endif /* ALLOW_BORG */

	/* We need to save the flags to get the refcounting right. */
	byte flags;
};

typedef map_block *map_blk_ptr;
typedef map_block **map_blk_ptr_ptr;

typedef void (*map_info_hook_type) (map_block *mb_ptr, term_map *map);
typedef void (*map_erase_hook_type) (void);

#endif /* TERM_USE_MAP */

/* Extern Variables */
extern byte gamma_table[256];

#ifdef TERM_USE_MAP
extern map_blk_ptr_ptr *map_cache;
extern byte *map_cache_refcount;
extern int *map_cache_x;
extern int *map_cache_y;
extern int **map_grid;
#endif /* TERM_USE_MAP */


/* Extern Functions */
extern void build_gamma_table(int gamma);
extern cptr get_default_font(int term_num);
extern bool pick_graphics(int graphics, int *xsize, int *ysize, char *filename);
#ifdef TERM_USE_MAP
extern map_info_hook_type set_map_hook(map_info_hook_type hook_func);
extern map_erase_hook_type set_erase_hook(map_erase_hook_type hook_func);
extern void init_overhead_map(void);
extern void del_overhead_map(void);
extern bool map_in_bounds(int x, int y);
extern map_block *map_loc(int dx, int dy);
#endif /* TERM_USE_MAP */
