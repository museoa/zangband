%module store

%{

#include "angband.h"

%}

typedef const char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;



typedef struct object_type object_type;

struct object_type
{
	s16b k_idx;			/* Kind index (zero if "dead") */

	s16b iy;			/* Y-position on map, or zero */
	s16b ix;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	s16b pval;			/* Item extra-parameter */

	byte discount;		/* Discount (if any) */

	byte number;		/* Number of items */

	s16b weight;		/* Item weight */

	s16b to_h;			/* Plusses to hit */
	s16b to_d;			/* Plusses to damage */
	s16b to_a;			/* Plusses to AC */

	s16b ac;			/* Normal AC */

	s16b timeout;		/* Timeout Counter */

	byte dd, ds;		/* Damage dice/sides */	

	byte ident;			/* Special flags  */

	byte marked;		/* Object is marked */

	u16b inscription;	/* Inscription index */
	u16b xtra_name;      /* Extra Name (Artifacts and ego items) */

	u32b flags1;        /* Flags, set 1 */
	u32b flags2;        /* Flags, set 2 */
	u32b flags3;        /* Flags, set 3 */
	
	u32b kn_flags1;     /* Known Flags, set 1 */
	u32b kn_flags2;     /* Known Flags, set 2 */
	u32b kn_flags3;     /* Known Flags, set 3 */

	s16b next_o_idx;	/* Next object in stack (if any) */

	s16b held_m_idx;	/* Monster holding us (if any) */

	s32b cost;			/* Object "base cost" */
	
	byte feeling;       /* Game generated inscription number (eg, pseudo-id) */

	byte activate;		/* Activation type */

#ifdef USE_SCRIPT
	PyObject *python;
#endif /* USE_SCRIPT */

#ifdef SCRIPT_OBJ_KIND
	char *name;

	byte d_attr;		/* Default object attribute */
	char d_char;		/* Default object character */


	byte x_attr;		/* Desired object attribute */
	char x_char;		/* Desired object character */


	byte flavor;		/* Special object flavor (or zero) */

	bool easy_know;		/* This object is always known (if aware) */


	bool aware;			/* The player is "aware" of the item's effects */

	bool tried;			/* The player has "tried" one of the items */
#endif /* SCRIPT_OBJ_KIND */
};



/*
 * A store owner
 */
typedef struct owner_type owner_type;

struct owner_type
{
	cptr owner_name;	/* Name */

	s16b max_cost;		/* Purse limit */

	byte max_inflate;	/* Inflation (max) */
	byte min_inflate;	/* Inflation (min) */

	byte haggle_per;	/* Haggle unit */

	byte insult_max;	/* Insult limit */

	byte owner_race;	/* Owner race */
};




/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
typedef struct store_type store_type;

struct store_type
{
	byte type;				/* Store type */
	byte owner;				/* Owner index */

	s16b insult_cur;		/* Insult counter */

	s16b good_buy;			/* Number of "good" buys */
	s16b bad_buy;			/* Number of "bad" buys */

	s32b store_open;		/* Closed until this turn */

	s32b last_visit;		/* Last visited on this turn */

	byte max_stock;			/* Stock -- Max number of entries */
	byte stock_num;			/* Stock -- Number of entries */
	object_type *stock;		/* Stock -- Actual stock items */
	
	u16b x;					/* Location x coord. */
	u16b y;					/* Location y coord. */
};

typedef store_type *store_ptr;
