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
 * Constants used to pass lighting information to users
 * of the overhead map hooks.
 */
#define MAP_SEEN	0x01
#define MAP_GLOW	0x02
#define MAP_LITE	0x04


/*
 * Map data structure
 */
typedef struct term_map term_map;

struct term_map
{
	s16b object;
	s16b monster;
	s16b field;
	byte terrain;
	byte flags;
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
#endif /* TERM_CAVE_MAP */

	/* Borg-specific stuff */
#ifdef ALLOW_BORG
	byte info;		/* info flags */
	byte xtra;		/* search count */
	
	byte flow;		/* "flow" data */
	byte cost;		/* "cost" data */
#endif /* ALLOW_BORG */

	/* We need to save the flags to get the refcounting right. */
	byte flags;
};

typedef map_block **map_blk_ptr;

typedef void (*map_info_hook_type)(map_block *mb_ptr, term_map *map);
typedef errr (*map_erase_hook_type) (void);

#endif /* TERM_USE_MAP */


extern byte gamma_table[256];
extern void build_gamma_table(int gamma);
extern cptr get_default_font(int term_num);
extern bool pick_graphics(int graphics, int *xsize, int *ysize, char *filename);
extern map_info_hook_type set_map_hook(map_info_hook_type hook_func);
extern map_erase_hook_type set_erase_hook(map_erase_hook_type hook_func);
extern void init_overhead_map(void);
extern void del_overhead_map(void);
extern bool map_in_bounds(int x, int y);
extern map_block *map_loc(int dx, int dy);
