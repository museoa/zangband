/*
 * File: script.h
 * Purpose: Header file for scripting support
 */

/*
 * Copyright (c) 2001 Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


#include "angband.h"


#ifdef USE_SCRIPT

/* Wrappers */
extern void script_init_io(void);

/* script.c */
extern errr script_execute(char *name);
extern errr init_script(void);

#endif /* USE_SCRIPT */

