/* File: tcltk-dll.h */

/* Purpose: embedded Tcl */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef INCLUDED_TCLTK_H
#define INCLUDED_TCLTK_H

#if !defined(PLATFORM_MAC) && !defined(PLATFORM_WIN) && !defined(PLATFORM_X11)
#error "you must define one of PLATFORM_MAC, PLATFORM_WIN or PLATFORM_X11"
#endif /* */

Tcl_Interp *TclTk_Init(int argc, char **argv);
void TclTk_Exit(Tcl_Interp *interp);
Tcl_Interp *TclTk_Interp(void);

#endif /* INCLUDED_TCLTK_H */
