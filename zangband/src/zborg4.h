
/* File: zborg4.h */

/* Purpose: Header file for "borg4.c" -BEN- */

#ifndef INCLUDED_BORG4_H
#define INCLUDED_BORG4_H

#include "angband.h"

#ifdef ALLOW_BORG

/*
 * This file provides support for "borg4.c".
 */

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"

/* Object list interface */
extern list_notice_hook_type old_list_hook;
extern void borg_list_info(byte list_type);

/*
 * Extract various bonuses
 */
extern void borg_notice(bool notice_swap);

/*
 * Extract the bonuses for items in the home.
 *
 * in_item is passed in if you want to pretent that in_item is
 *          the only item in the home.
 * no_items is passed in as TRUE if you want to pretend that the
 *          home is empty.
 */
extern void borg_notice_home(borg_item *in_item, bool no_items);


/*
 * Calculate the basic "power"
 */
extern s32b borg_power(void);

/*
 * Calculate the basic "power" of the home
 */
extern s32b borg_power_home(void);



/*
 * Calculate danger to a grid from a monster
 */
extern int borg_danger_aux(int x, int y, int c, int i, bool average);

/*
 * Hack -- Calculate the "danger" of the given grid.
 */
extern int borg_danger(int x, int y, int c, bool average);


/*
 * Determine if the Borg is out of "crucial" supplies.
 */
extern cptr borg_restock(int depth);

/*
 * Determine if the Borg is "prepared" for the given level
 */
extern cptr borg_prepared(int depth);


/*
 * Initialize this file
 */
extern void borg_init_4(void);


#endif

#endif
