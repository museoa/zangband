/* File: tcltk.h */

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

Tcl_Interp *TclTk_Init(int argc, char **argv);
void TclTk_Exit(Tcl_Interp *interp);

#endif /* INCLUDED_TCLTK_H */
