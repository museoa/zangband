/* File: maid-grf.h */

/* Purpose: Common header file for graphics ports */

/*
 * Copyright (c) 2002 Steven Fuerst
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/* maid-graf.c */
extern byte gamma_table[256];
extern void build_gamma_table(int gamma);
extern cptr get_default_font(int term_num);
extern bool pick_graphics(int graphics, int *xsize, int *ysize, char *filename);
