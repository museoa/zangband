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


extern byte gamma_table[256];
extern void build_gamma_table(int gamma);
extern cptr get_default_font(int term_num);
extern bool pick_graphics(int graphics, int *xsize, int *ysize, char *filename);

#ifdef TERM_USE_MAP
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

	/* We need to save the flags to get the refcounting right. */
	byte flags;
};

typdef map_block **map_blk_ptr;

typedef (*map_info_hook_type)(map_block *mb_ptr, term_map *map);

#endif /* TERM_USE_MAP */
