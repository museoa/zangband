/* CVS: Last edit by $Author$ on $Date$
 * 
 * File: grid.h
 * Purpose: header file for grid.c, used only in dungeon generation
 * files (generate.c, rooms.c)
 */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/* Macros */

#define set_cave_feat(X,Y,Z)    (cave[(X)][(Y)].feat = (Z))
#define set_cave_info(X,Y,Z)    (cave[(X)][(Y)].info = (Z))

#define place_rubble(X,Y)       set_cave_feat(X,Y,FEAT_RUBBLE)
#define place_up_stairs(X,Y)    set_cave_feat(X,Y,FEAT_LESS)
#define place_down_stairs(X,Y)  set_cave_feat(X,Y,FEAT_MORE)
#define place_locked_door(X,Y)  set_cave_feat(X,Y,FEAT_DOOR_HEAD+randint(7))
#define place_secret_door(X,Y)  set_cave_feat(X,Y,FEAT_SECRET)
#define place_inner_wall(X,Y)   set_cave_feat(X,Y,FEAT_WALL_INNER)
#define place_outer_wall(X,Y)   set_cave_feat(X,Y,FEAT_WALL_OUTER)

/* Externs */

extern bool new_player_spot(void);

extern void place_random_stairs(int y, int x);
extern void place_random_door(int y, int x);
extern void place_closed_door(int y, int x);
extern void place_floor(int x1, int x2, int y1, int y2, bool light);
extern void place_room(int x1, int x2, int y1, int y2, bool light);
extern void vault_monsters(int y1, int x1, int num);
extern void vault_objects(int y, int x, int num);
extern void vault_trap_aux(int y, int x, int yd, int xd);
extern void vault_traps(int y, int x, int yd, int xd, int num);

extern int next_to_walls(int y, int x);
extern void correct_dir(int *rdir, int *cdir, int y1, int x1, int y2, int x2);

extern void rand_dir(int *rdir, int *cdir);
extern void build_tunnel(int row1, int col1, int row2, int col2);
