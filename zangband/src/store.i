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
	
	field_type *f_ptr;		/* Field corresponding to this store */
	byte	info_flags;		/* Info flags - obtained from the field data[] */
};

typedef store_type *store_ptr;
