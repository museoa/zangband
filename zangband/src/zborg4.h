
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

/* Borg functions */
extern void borg_notice(bool notice_swap);
extern void borg_notice_home(list_item *l_ptr, bool no_items);
extern s32b borg_power(void);
extern s32b borg_power_home(void);
extern int borg_danger_aux(int x, int y, int c, int i, bool average);
extern int borg_danger(int x, int y, int c, bool average);
extern cptr borg_restock(int depth);
extern cptr borg_prepared(int depth);


/*
 * Initialize this file
 */
extern void borg_init_4(void);


#endif

#endif
