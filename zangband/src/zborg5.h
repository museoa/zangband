/* File: zborg5.h */
/* Purpose: Header file for "borg5.c" -BEN- */

#ifndef INCLUDED_BORG5_H
#define INCLUDED_BORG5_H

#include "angband.h"

#ifdef ALLOW_BORG

/*
 * This file provides support for "borg5.c".
 */

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"
#include "zborg6.h"

/* Hack - this is a field, so define it here */
#define FEAT_INVIS	0x02


/* Old overhead map info hook to chain into */
extern map_info_hook_type old_info_hook;

/*
 * Save the borg information into the overhead map
 */
extern void borg_map_info(map_block *mb_ptr, term_map *map);


#define WANK_KILL	2



/*
 * Hack -- monster/object tracking grids
 */

typedef struct borg_wank borg_wank;

struct borg_wank
{
	/* Linked list of wanks */
	u16b next_wank;

	/* Location */
	byte x;
	byte y;

	byte type;
};


/*
 * Update state based on current "map"
 */
extern void borg_update(void);


/*
 * React to various "important" messages
 */
extern void borg_react(cptr msg, cptr buf);
extern void borg_delete_kill(int i);


/*
 * Initialize this file
 */
extern void borg_init_5(void);




#endif

#endif
