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
#include "zborg4.h"



extern s32b borg_power(void);
extern cptr borg_restock(int depth);
extern cptr borg_prepared(int depth);

/*
 * Initialize this file
 */
extern void borg_init_5(void);


#endif

#endif
