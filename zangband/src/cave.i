%module cave

%{

#include "angband.h"

%}

typedef const char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;

/*
 * Number of grids in each block (vertically)
 * Probably hard-coded to 11, see "generate.c"
 */
#define BLOCK_HGT	11

/*
 * Number of grids in each block (horizontally)
 * Probably hard-coded to 11, see "generate.c"
 */
#define BLOCK_WID	11


/*
 * Number of grids in each panel (vertically)
 * Must be a multiple of BLOCK_HGT
 */
#define PANEL_HGT	11

/*
 * Number of grids in each panel (horizontally)
 * Must be a multiple of BLOCK_WID
 */
#define PANEL_WID	33


/*
 * Number of grids used to display the dungeon (vertically).
 * Must be a multiple of 11, probably hard-coded to 22.
 */
#define SCREEN_HGT      22

/*
 * Number of grids used to display the dungeon (horizontally).
 * Must be a multiple of 33, probably hard-coded to 66.
 */
#define SCREEN_WID      66


/*
 * Maximum dungeon height in grids, must be a multiple of SCREEN_HGT,
 * probably hard-coded to SCREEN_HGT * 3.
 */
#define MAX_HGT         66

/*
 * Maximum dungeon width in grids, must be a multiple of SCREEN_WID,
 * probably hard-coded to SCREEN_WID * 3.
 */
#define MAX_WID         198


%name (set_feat) void cave_set_feat(int y, int x, int feat);


struct cave_type
{
	byte info;		/* Hack -- cave flags */

	byte feat;		/* Hack -- feature type */

	s16b o_idx;		/* Object in this grid */

	s16b m_idx;		/* Monster in this grid */

#ifdef MONSTER_FLOW

	byte cost;		/* Hack -- cost of flowing */
	byte when;		/* Hack -- when cost was computed */

#endif /* MONSTER_FLOW */

}

%{
	cave_type *tile(int y, int x)
	{
		return &cave[y][x];
	}
%}

cave_type *tile(int y, int x);

s16b object_level;		/* Current object creation level */
s16b monster_level;		/* Current monster creation level */
s16b base_level;        /* Base dungeon level */


/*
 * Information about "vault generation"
 */

typedef struct vault_type vault_type;

struct vault_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte typ;			/* Vault type */

	byte rat;			/* Vault rating */

	byte hgt;			/* Vault height */
	byte wid;			/* Vault width */
};


extern void map_area(void);
extern void wiz_lite(void);
extern void wiz_dark(void);
