/* File: zborg2.h */
/* Purpose: Header file for "borg2.c" -BEN- */

#ifndef INCLUDED_BORG2_H
#define INCLUDED_BORG2_H

#include "angband.h"

#ifdef ALLOW_BORG

/*
 * This file provides support for "borg2.c".
 */

#include "zborg1.h"

/*
 * Determine if a "legal" grid is a "floor" grid
 */
#define borg_cave_floor_grid(C) \
    (!(f_info[(C)->feat].flags & FF_BLOCK))
/*
 * Determine if a "legal" grid is a "wall" grid
 */
#define borg_cave_wall_grid(C) \
    (f_info[(C)->feat].flags & FF_BLOCK)

/*
 * True half the time for trees. (Block line of sight half the time.)
 */
#define borg_cave_half_grid(C) \
    ((f_info[(C)->feat].flags & FF_HALF_LOS) && (quick_rand()))

/*
 * Grid will block LOS.
 */
#define borg_cave_los_grid(C) \
   ((borg_cave_floor_grid(C)) || (borg_cave_half_grid(C)))


/*
 * Bold version of borg_cave_floor_grid
 * (with bounds checking)
 */
#define borg_cave_floor_bold(Y, X) \
    (map_in_bounds((X),(Y)) && \
	 borg_cave_floor_grid(map_loc((X),(Y))))

/* Useful typedef for los_general() */
typedef bool (*map_hook_type) (map_block *mb_ptr);


extern void borg_mmove_init(int x1, int y1, int x2, int y2);
extern void borg_mmove(int *x, int *y, int x1, int y1);
extern void borgmove2(int *y, int *x, int y1, int x1, int y2, int x2);
extern bool borg_los(int y1, int x1, int y2, int x2);
extern bool borg_projectable(int y1, int x1, int y2, int x2);
extern bool borg_offset_projectable(int y1, int x1, int y2, int x2);
extern bool borg_projectable_pure(int y1, int x1, int y2, int x2);
extern void borg_forget_view(void);
extern void borg_update_view(void);


/*
 * Initialize this file
 */
extern void borg_init_2(void);


#endif


#endif
