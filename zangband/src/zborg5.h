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

/* Types of monster list */
#define BORG_MON_USED	1
#define BORG_MON_NEW	2
#define BORG_MON_OLD	3
#define BORG_MON_MOVE	4

/* Recalculate danger */
extern bool borg_danger_wipe;


/* Old overhead map hooks to chain into */
extern map_info_hook_type old_info_hook;
extern map_erase_hook_type old_erase_hook;

/*
 * Save the borg information into the overhead map
 */
extern void borg_map_info(map_block *mb_ptr, term_map *map);

/*
 * Notice when we change level
 */
extern void borg_map_erase(void);

/*
 * Update state based on current "map"
 */
extern void borg_update(void);


/*
 * React to various "important" messages
 */
extern void borg_react(cptr msg, cptr buf);

/*
 * Monster kill list interface
 */
extern void borg_delete_kill(int i, cptr reason);


/*
 * Initialize this file
 */
extern void borg_init_5(void);




#endif

#endif
