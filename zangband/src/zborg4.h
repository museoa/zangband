
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

/* Treat items differently depending on flags */
#define TREAT_AS_NORM	0		/* Normal item */
#define TREAT_AS_GONE	1		/* Pretend item doesn't exist */
#define TREAT_AS_LESS	2		/* Pretend one less item */
#define TREAT_AS_MORE	3		/* Pretend one more item */
#define TREAT_AS_SWAP	4		/* Pretend other item is here */

/* The current home */
extern int home_shop;

/* Borg functions */
extern void borg_notice(void);
extern void borg_notice_home(void);
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
