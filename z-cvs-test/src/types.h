/* CVS: Last edit by $Author$ on $Date$ */
/* File: types.h */

/* Purpose: global type declarations */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/*
 * This file should ONLY be included by "angband.h"
 */

/*
 * Note that "char" may or may not be signed, and that "signed char"
 * may or may not work on all machines.  So always use "s16b" or "s32b"
 * for signed values.  Also, note that unsigned values cause math problems
 * in many cases, so try to only use "u16b" and "u32b" for "bit flags",
 * unless you really need the extra bit of information, or you really
 * need to restrict yourself to a single byte for storage reasons.
 *
 * Also, if possible, attempt to restrict yourself to sub-fields of
 * known size (use "s16b" or "s32b" instead of "int", and "byte" instead
 * of "bool"), and attempt to align all fields along four-byte words, to
 * optimize storage issues on 32-bit machines.  Also, avoid "bit flags"
 * since these increase the code size and slow down execution.  When
 * you need to store bit flags, use one byte per flag, or, where space
 * is an issue, use a "byte" or "u16b" or "u32b", and add special code
 * to access the various bit flags.
 *
 * Many of these structures were developed to reduce the number of global
 * variables, facilitate structured program design, allow the use of ascii
 * template files, simplify access to indexed data, or facilitate efficient
 * clearing of many variables at once.
 *
 * Certain data is saved in multiple places for efficient access, currently,
 * this includes the tval/sval/weight fields in "object_type", various fields
 * in "header_type", and the "m_idx" and "o_idx" fields in "cave_type".  All
 * of these could be removed, but this would, in general, slow down the game
 * and increase the complexity of the code.
 */





/*
 * Template file header information (see "init.c").  16 bytes.
 *
 * Note that the sizes of many of the "arrays" are between 32768 and
 * 65535, and so we must use "unsigned" values to hold the "sizes" of
 * these arrays below.  Normally, I try to avoid using unsigned values,
 * since they can cause all sorts of bizarre problems, but I have no
 * choice here, at least, until the "race" array is split into "normal"
 * and "unique" monsters, which may or may not actually help.
 *
 * Note that, on some machines, for example, the Macintosh, the standard
 * "read()" and "write()" functions cannot handle more than 32767 bytes
 * at one time, so we need replacement functions, see "util.c" for details.
 *
 * Note that, on some machines, for example, the Macintosh, the standard
 * "malloc()" function cannot handle more than 32767 bytes at one time,
 * but we may assume that the "ralloc()" function can handle up to 65535
 * butes at one time.  We should not, however, assume that the "ralloc()"
 * function can handle more than 65536 bytes at a time, since this might
 * result in segmentation problems on certain older machines, and in fact,
 * we should not assume that it can handle exactly 65536 bytes at a time,
 * since the internal functions may use an unsigned short to specify size.
 *
 * In general, these problems occur only on machines (such as most personal
 * computers) which use 2 byte "int" values, and which use "int" for the
 * arguments to the relevent functions.
 */

typedef struct header header;

struct header
{
	byte v_major; /* Version -- major */
	byte v_minor; /* Version -- minor */
	byte v_patch; /* Version -- patch */
	byte v_extra; /* Version -- extra */


	u16b info_num; /* Number of "info" records */

	u16b info_len; /* Size of each "info" record */


	u32b head_size;	/* Size of the "header" in bytes */

	u32b info_size;	/* Size of the "info" array in bytes */

	u32b name_size;	/* Size of the "name" array in bytes */

	u32b text_size;	/* Size of the "text" array in bytes */
};



/*
 * Information about terrain "features"
 */

typedef struct feature_type feature_type;

struct feature_type
{
	u32b name; /* Name (offset) */
	u32b text; /* Text (offset) */

	byte mimic;	/* Feature to mimic */

	byte extra;	/* Extra byte (unused) */

	s16b unused; /* Extra bytes (unused) */

	byte d_attr; /* Default feature attribute */
	char d_char; /* Default feature character */


	byte x_attr; /* Desired feature attribute */
	char x_char; /* Desired feature character */
};


/*
 * Information about object "kinds", including player knowledge.
 *
 * Only "aware" and "tried" are saved in the savefile
 */

typedef struct object_kind object_kind;

struct object_kind
{
	u32b name; /* Name (offset) */
	u32b text; /* Text (offset) */

	byte tval; /* Object type */
	byte sval; /* Object sub type */

	s16b pval; /* Object extra info */

	s16b to_h; /* Bonus to hit */
	s16b to_d; /* Bonus to damage */
	s16b to_a; /* Bonus to armor */

	s16b ac; /* Base armor */

	byte dd, ds; /* Damage dice/sides */

	s16b weight; /* Weight */

	s32b cost; /* Object "base cost" */

	u32b flags1; /* Flags, set 1 */
	u32b flags2; /* Flags, set 2 */
	u32b flags3; /* Flags, set 3 */

	byte locale[4];	/* Allocation level(s) */
	byte chance[4];	/* Allocation chance(s) */

	byte level;	/* Level */
	byte extra;	/* Something */


	byte d_attr; /* Default object attribute */
	char d_char; /* Default object character */


	byte x_attr; /* Desired object attribute */
	char x_char; /* Desired object character */


	byte flavor; /* Special object flavor (or zero) */

	bool easy_know;	/* This object is always known (if aware) */


	bool aware;	/* The player is "aware" of the item's effects */

	bool tried;	/* The player has "tried" one of the items */
};



/*
 * Information about "artifacts".
 *
 * Note that the save-file only writes "cur_num" to the savefile.
 *
 * Note that "max_num" is always "1" (if that artifact "exists")
 */

typedef struct artifact_type artifact_type;

struct artifact_type
{
	u32b name; /* Name (offset) */
	u32b text; /* Text (offset) */

	byte tval; /* Artifact type */
	byte sval; /* Artifact sub type */

	s16b pval; /* Artifact extra info */

	s16b to_h; /* Bonus to hit */
	s16b to_d; /* Bonus to damage */
	s16b to_a; /* Bonus to armor */

	s16b ac; /* Base armor */

	byte dd, ds; /* Damage when hits */

	s16b weight; /* Weight */

	s32b cost; /* Artifact "cost" */

	u32b flags1; /* Artifact Flags, set 1 */
	u32b flags2; /* Artifact Flags, set 2 */
	u32b flags3; /* Artifact Flags, set 3 */

	byte level;	/* Artifact level */
	byte rarity; /* Artifact rarity */

	byte cur_num; /* Number created (0 or 1) */
	byte max_num; /* Unused (should be "1") */
};


/*
 * Information about "ego-items".
 */

typedef struct ego_item_type ego_item_type;

struct ego_item_type
{
	u32b name; /* Name (offset) */
	u32b text; /* Text (offset) */

	byte slot; /* Standard slot value */
	byte rating; /* Rating boost */

	byte level;	/* Minimum level */
	byte rarity; /* Object rarity */

	byte max_to_h; /* Maximum to-hit bonus */
	byte max_to_d; /* Maximum to-dam bonus */
	byte max_to_a; /* Maximum to-ac bonus */

	byte max_pval; /* Maximum pval */

	s32b cost; /* Ego-item "cost" */

	u32b flags1; /* Ego-Item Flags, set 1 */
	u32b flags2; /* Ego-Item Flags, set 2 */
	u32b flags3; /* Ego-Item Flags, set 3 */
};




/*
 * Monster blow structure
 *
 *	- Method (RBM_*)
 *	- Effect (RBE_*)
 *	- Damage Dice
 *	- Damage Sides
 */

typedef struct monster_blow monster_blow;

struct monster_blow
{
	byte method;
	byte effect;
	byte d_dice;
	byte d_side;
};



/*
 * Monster "race" information, including racial memories
 *
 * Note that "d_attr" and "d_char" are used for MORE than "visual" stuff.
 *
 * Note that "x_attr" and "x_char" are used ONLY for "visual" stuff.
 *
 * Note that "cur_num" (and "max_num") represent the number of monsters
 * of the given race currently on (and allowed on) the current level.
 * This information yields the "dead" flag for Unique monsters.
 *
 * Note that "max_num" is reset when a new player is created.
 * Note that "cur_num" is reset when a new level is created.
 *
 * Note that several of these fields, related to "recall", can be
 * scrapped if space becomes an issue, resulting in less "complete"
 * monster recall (no knowledge of spells, etc).  All of the "recall"
 * fields have a special prefix to aid in searching for them.
 */


typedef struct monster_race monster_race;

struct monster_race
{
	u32b name; /* Name (offset) */
	u32b text; /* Text (offset) */

	byte hdice;	/* Creatures hit dice count */
	byte hside;	/* Creatures hit dice sides */

	s16b ac; /* Armour Class */

	s16b sleep;	/* Inactive counter (base) */
	byte aaf; /* Area affect radius (1-100) */
	byte speed;	/* Speed (normally 110) */

	s32b mexp; /* Exp value for kill */

	s16b extra;	/* Unused (for now) */

	byte freq_inate; /* Inate spell frequency */
	byte freq_spell; /* Other spell frequency */

	u32b flags1; /* Flags 1 (general) */
	u32b flags2; /* Flags 2 (abilities) */
	u32b flags3; /* Flags 3 (race/resist) */
	u32b flags4; /* Flags 4 (inate/breath) */
	u32b flags5; /* Flags 5 (normal spells) */
	u32b flags6; /* Flags 6 (special spells) */
	u32b flags7; /* Flags 7 (movement related abilities) */
	u32b flags8; /* Flags 8 (wilderness info) */
	u32b flags9; /* Flags 9 (drops info) */

	monster_blow blow[4]; /* Up to four blows per round */


	byte level;	/* Level of creature */
	byte rarity; /* Rarity of creature */


	byte d_attr; /* Default monster attribute */
	char d_char; /* Default monster character */


	byte x_attr; /* Desired monster attribute */
	char x_char; /* Desired monster character */


	byte max_num; /* Maximum population allowed per level */

	byte cur_num; /* Monster population on current level */


	s16b r_sights; /* Count sightings of this monster */
	s16b r_deaths; /* Count deaths from this monster */

	s16b r_pkills; /* Count monsters killed in this life */
	s16b r_tkills; /* Count monsters killed in all lives */

	byte r_wake; /* Number of times woken up (?) */
	byte r_ignore; /* Number of times ignored (?) */

	byte r_xtra1; /* Something (unused) */
	byte r_xtra2; /* Something (unused) */

	byte r_drop_gold; /* Max number of gold dropped at once */
	byte r_drop_item; /* Max number of item dropped at once */

	byte r_cast_inate; /* Max number of inate spells seen */
	byte r_cast_spell; /* Max number of other spells seen */

	byte r_blows[4]; /* Number of times each blow type was seen */

	u32b r_flags1; /* Observed racial flags */
	u32b r_flags2; /* Observed racial flags */
	u32b r_flags3; /* Observed racial flags */
	u32b r_flags4; /* Observed racial flags */
	u32b r_flags5; /* Observed racial flags */
	u32b r_flags6; /* Observed racial flags */
	u32b r_flags7; /* Observed racial flags */
};



/*
 * Information about "vault generation"
 */

typedef struct vault_type vault_type;

struct vault_type
{
	u32b name; /* Name (offset) */
	u32b text; /* Text (offset) */

	byte typ; /* Vault type */

	byte rat; /* Vault rating */

	byte hgt; /* Vault height */
	byte wid; /* Vault width */
};





/*
 * A single "grid" in a Cave
 *
 * Note that several aspects of the code restrict the actual cave
 * to a max size of 256 by 256.  In partcular, locations are often
 * saved as bytes, limiting each coordinate to the 0-255 range.
 *
 * The "o_idx" and "m_idx" fields are very interesting.  There are
 * many places in the code where we need quick access to the actual
 * monster or object(s) in a given cave grid.  The easiest way to
 * do this is to simply keep the index of the monster and object
 * (if any) with the grid, but this takes 198*66*4 bytes of memory.
 * Several other methods come to mind, which require only half this
 * amound of memory, but they all seem rather complicated, and would
 * probably add enough code that the savings would be lost.  So for
 * these reasons, we simply store an index into the "o_list" and
 * "m_list" arrays, using "zero" when no monster/object is present.
 *
 * Note that "o_idx" is the index of the top object in a stack of
 * objects, using the "next_o_idx" field of objects (see below) to
 * create the singly linked list of objects.  If "o_idx" is zero
 * then there are no objects in the grid.
 *
 * Note the special fields for the "MONSTER_FLOW" code.
 */

typedef struct cave_type cave_type;

struct cave_type
{
	byte info; /* Hack -- cave flags */

	byte feat; /* Hack -- feature type */

	s16b o_idx;	/* Object in this grid */

	s16b m_idx;	/* Monster in this grid */

	s16b special; /* Special cave info */

	byte mimic;	/* Feature to mimic */

#ifdef MONSTER_FLOW

	byte cost; /* Hack -- cost of flowing */
	byte when; /* Hack -- when cost was computed */

#endif /* MONSTER_FLOW */
};

/*
 * Simple structure to hold a map location
 */
typedef struct coord coord;

struct coord
{
	u16b y;
	u16b x;
};


/*
 * Object information, for a specific object.
 *
 * Note that a "discount" on an item is permanent and never goes away.
 *
 * Note that inscriptions are now handled via the "quark_str()" function
 * applied to the "note" field, which will return NULL if "note" is zero.
 *
 * Note that "object" records are "copied" on a fairly regular basis,
 * and care must be taken when handling such objects.
 *
 * Note that "object flags" must now be derived from the object kind,
 * the artifact and ego-item indexes, and the two "xtra" fields.
 *
 * Each cave grid points to one (or zero) objects via the "o_idx"
 * field (above).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a "stack" of objects in the same grid.
 *
 * Each monster points to one (or zero) objects via the "hold_o_idx"
 * field (below).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a pile of objects held by the monster.
 *
 * The "held_m_idx" field is used to indicate which monster, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */

typedef struct object_type object_type;

struct object_type
{
	s16b k_idx;	/* Kind index (zero if "dead") */

	byte iy; /* Y-position on map, or zero */
	byte ix; /* X-position on map, or zero */

	byte tval; /* Item type (from kind) */
	byte sval; /* Item sub-type (from kind) */

	s16b pval; /* Item extra-parameter */

	byte discount; /* Discount (if any) */

	byte number; /* Number of items */

	s16b weight; /* Item weight */

	byte name1;	/* Artifact type, if any */
	byte name2;	/* Ego-Item type, if any */

	byte xtra1;	/* Extra info type */
	byte xtra2;	/* Extra info index */

	s16b to_h; /* Plusses to hit */
	s16b to_d; /* Plusses to damage */
	s16b to_a; /* Plusses to AC */

	s16b ac; /* Normal AC */

	byte dd, ds; /* Damage dice/sides */

	s16b timeout; /* Timeout Counter */

	byte ident;	/* Special flags  */

	byte marked; /* Object is marked */

	u16b inscription; /* Inscription index */
	u16b art_name; /* Artifact name (random artifacts) */

	byte feeling; /* Game generated inscription number (eg, pseudo-id) */

	u32b art_flags1; /* Flags, set 1  Alas, these were necessary */
	u32b art_flags2; /* Flags, set 2  for the random artifacts of */
	u32b art_flags3; /* Flags, set 3  Zangband */

	s16b next_o_idx; /* Next object in stack (if any) */

	s16b held_m_idx; /* Monster holding us (if any) */

#ifdef SCRIPT_OBJ_KIND
	char *name;

	byte d_attr; /* Default object attribute */
	char d_char; /* Default object character */


	byte x_attr; /* Desired object attribute */
	char x_char; /* Desired object character */


	byte flavor; /* Special object flavor (or zero) */

	bool easy_know;	/* This object is always known (if aware) */


	bool aware;	/* The player is "aware" of the item's effects */

	bool tried;	/* The player has "tried" one of the items */
#endif /* SCRIPT_OBJ_KIND */
};



/*
 * Monster information, for a specific monster.
 *
 * Note: fy, fx constrain dungeon size to 256x256
 *
 * The "hold_o_idx" field points to the first object of a stack
 * of objects (if any) being carried by the monster (see above).
 */

typedef struct monster_type monster_type;

struct monster_type
{
	s16b r_idx;	/* Monster race index */

	byte fy; /* Y location on map */
	byte fx; /* X location on map */

	s16b hp; /* Current Hit points */
	s16b maxhp;	/* Max Hit points */

	s16b csleep; /* Inactive counter */

	byte mspeed; /* Monster "speed" */
	byte energy; /* Monster "energy" */

	byte stunned; /* Monster is stunned */
	byte confused; /* Monster is confused */
	byte monfear; /* Monster is afraid */
	byte invulner; /* Monster is temporarily invulnerable */

	byte cdis; /* Current dis from player */

	byte mflag;	/* Extra monster flags */

	bool ml; /* Monster is "visible" */

	s16b hold_o_idx; /* Object being held (if any) */

#ifdef WDT_TRACK_OPTIONS

	byte ty; /* Y location of target */
	byte tx; /* X location of target */

	byte t_dur;	/* How long are we tracking */

	byte t_bit;	/* Up to eight bit flags */

#endif /* WDT_TRACK_OPTIONS */

#ifdef DRS_SMART_OPTIONS

	u32b smart;	/* Field for "smart_learn" */

#endif /* DRS_SMART_OPTIONS */

};




/*
 * An entry for the object/monster allocation functions
 *
 * Pass 1 is determined from allocation information
 * Pass 2 is determined from allocation restriction
 * Pass 3 is determined from allocation calculation
 */

typedef struct alloc_entry alloc_entry;

struct alloc_entry
{
	s16b index;	/* The actual index */

	byte level;	/* Base dungeon level */
	byte prob1;	/* Probability, pass 1 */
	byte prob2;	/* Probability, pass 2 */
	byte prob3;	/* Probability, pass 3 */

	u16b total;	/* Unused for now */
};



/*
 * Available "options"
 *
 *	- Address of actual option variable (or NULL)
 *
 *	- Normal Value (TRUE or FALSE)
 *
 *	- Option Page Number (or zero)
 *
 *	- Savefile Set (or zero)
 *	- Savefile Bit in that set
 *
 *	- Textual name (or NULL)
 *	- Textual description
 */

typedef struct option_type option_type;

struct option_type
{
	bool *o_var;

	byte o_norm;

	byte o_page;

	byte o_set;
	byte o_bit;

	cptr o_text;
	cptr o_desc;
};


/*
 * Structure for the "quests"
 */
typedef struct quest_type quest_type;

struct quest_type
{
	s16b status; /* Is the quest taken, completed, finished? */

	s16b type; /* The quest type */

	char name[60]; /* Quest name */
	s16b level;	/* Dungeon level */
	s16b r_idx;	/* Monster race */

	s16b cur_num; /* Number killed */
	s16b max_num; /* Number required */

	s16b k_idx;	/* object index */
	s16b num_mon; /* number of monsters on level */

	byte flags;	/* quest flags */
};


/*
 * A store owner
 */
typedef struct owner_type owner_type;

struct owner_type
{
	cptr owner_name; /* Name */

	s16b max_cost; /* Purse limit */

	byte max_inflate; /* Inflation (max) */
	byte min_inflate; /* Inflation (min) */

	byte haggle_per; /* Haggle unit */

	byte insult_max; /* Insult limit */

	byte owner_race; /* Owner race */
};




/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
typedef struct store_type store_type;

struct store_type
{
	byte type; /* Store type */

	byte owner;	/* Owner index */
	byte extra;	/* Unused for now */

	s16b insult_cur; /* Insult counter */

	s16b good_buy; /* Number of "good" buys */
	s16b bad_buy; /* Number of "bad" buys */

	s32b store_open; /* Closed until this turn */

	s32b last_visit; /* Last visited on this turn */

	s16b table_num;	/* Table -- Number of entries */
	s16b table_size; /* Table -- Total Size of Array */
	s16b *table; /* Table -- Legal item kinds */

	byte stock_num;	/* Stock -- Number of entries */
	s16b stock_size; /* Stock -- Total Size of Array */
	object_type *stock;	/* Stock -- Actual stock items */
};


/*
 * The "name" of spell 'N' is stored as spell_names[X][N],
 * where X is 0 for mage-spells and 1 for priest-spells.
 */
typedef struct magic_type magic_type;

struct magic_type
{
	byte slevel; /* Required level (to learn) */
	byte smana;	/* Required mana (to cast) */
	byte sfail;	/* Minimum chance of failure */
	byte sexp; /* Encoded experience bonus */
};


/*
 * Information about the player's "magic"
 *
 * Note that a player with a "spell_book" of "zero" is illiterate.
 */

typedef struct player_magic player_magic;

struct player_magic
{
	int spell_book;	/* Tval of spell books (if any) */
	int spell_xtra;	/* Something for later */

	int spell_stat;	/* Stat for spells (if any)  */
	int spell_type;	/* Spell type (mage/priest) */

	int spell_first; /* Level of first spell */
	int spell_weight; /* Weight that hurts spells */

	magic_type info[MAX_REALM][32];	/* The available spells */
};



/*
 * Player sex info
 */

typedef struct player_sex player_sex;

struct player_sex
{
	cptr title;	/* Type of sex */

	cptr winner; /* Name of winner */
};


/*
 * Player racial info
 */

typedef struct player_race player_race;

struct player_race
{
	cptr title;	/* Type of race */

	s16b r_adj[6]; /* Racial stat bonuses */

	s16b r_dis;	/* disarming */
	s16b r_dev;	/* magic devices */
	s16b r_sav;	/* saving throw */
	s16b r_stl;	/* stealth */
	s16b r_srh;	/* search ability */
	s16b r_fos;	/* search frequency */
	s16b r_thn;	/* combat (normal) */
	s16b r_thb;	/* combat (shooting) */

	byte r_mhp;	/* Race hit-dice modifier */
	byte r_exp;	/* Race experience factor */

	byte b_age;	/* base age */
	byte m_age;	/* mod age */

	byte m_b_ht; /* base height (males) */
	byte m_m_ht; /* mod height (males) */
	byte m_b_wt; /* base weight (males) */
	byte m_m_wt; /* mod weight (males) */

	byte f_b_ht; /* base height (females) */
	byte f_m_ht; /* mod height (females)   */
	byte f_b_wt; /* base weight (females) */
	byte f_m_wt; /* mod weight (females) */

	byte infra;	/* Infra-vision range */

	u16b choice; /* Legal class choices */
/*    byte choice_xtra;   */
};


/*
 * Player class info
 */

typedef struct player_class player_class;

struct player_class
{
	cptr title;	/* Type of class */

	s16b c_adj[6]; /* Class stat modifier */

	s16b c_dis;	/* class disarming */
	s16b c_dev;	/* class magic devices */
	s16b c_sav;	/* class saving throws */
	s16b c_stl;	/* class stealth */
	s16b c_srh;	/* class searching ability */
	s16b c_fos;	/* class searching frequency */
	s16b c_thn;	/* class to hit (normal) */
	s16b c_thb;	/* class to hit (bows) */

	s16b x_dis;	/* extra disarming */
	s16b x_dev;	/* extra magic devices */
	s16b x_sav;	/* extra saving throws */
	s16b x_stl;	/* extra stealth */
	s16b x_srh;	/* extra searching ability */
	s16b x_fos;	/* extra searching frequency */
	s16b x_thn;	/* extra to hit (normal) */
	s16b x_thb;	/* extra to hit (bows) */

	s16b c_mhp;	/* Class hit-dice adjustment */
	s16b c_exp;	/* Class experience factor */

	byte pet_upkeep_div; /* Pet upkeep divider */
};




/*
 * Most of the "player" information goes here.
 *
 * This stucture gives us a large collection of player variables.
 *
 * This structure contains several "blocks" of information.
 *   (1) the "permanent" info
 *   (2) the "variable" info
 *   (3) the "transient" info
 *
 * All of the "permanent" info, and most of the "variable" info,
 * is saved in the savefile.  The "transient" info is recomputed
 * whenever anything important changes.
 */

typedef struct player_type player_type;

struct player_type
{
	s16b oldpy;	/* Previous player location -KMW- */
	s16b oldpx;	/* Previous player location -KMW- */

	byte psex; /* Sex index */
	byte prace;	/* Race index */
	byte pclass; /* Class index */
	byte realm1; /* First magic realm */
	byte realm2; /* Second magic realm */
	byte oops; /* Unused */

	byte hitdie; /* Hit dice (sides) */
	u16b expfact; /* Experience factor
				   * Note: was byte, causing overflow for Amberite
				   * characters (such as Amberite Paladins)
				   */

	s16b age; /* Characters age */
	s16b ht; /* Height */
	s16b wt; /* Weight */
	s16b sc; /* Social Class */


	s32b au; /* Current Gold */

	s32b max_exp; /* Max experience */
	s32b exp; /* Cur experience */
	u16b exp_frac; /* Cur exp frac (times 2^16) */

	s16b lev; /* Level */

	s16b town_num; /* Current town number */
	s16b arena_number; /* monster number in arena -KMW- */
	s16b inside_arena; /* Is character inside arena? */
	s16b inside_quest; /* Inside quest level */

	s16b rewards[MAX_BACT];	/* Status of rewards in town */

	s32b wilderness_x; /* Coordinates in the wilderness */
	s32b wilderness_y;

	s16b mhp; /* Max hit pts */
	s16b chp; /* Cur hit pts */
	u16b chp_frac; /* Cur hit frac (times 2^16) */

	s16b msp; /* Max mana pts */
	s16b csp; /* Cur mana pts */
	u16b csp_frac; /* Cur mana frac (times 2^16) */

	s16b max_plv; /* Max Player Level */
	s16b max_dlv; /* Max level explored */

	s16b stat_max[6]; /* Current "maximal" stat values */
	s16b stat_cur[6]; /* Current "natural" stat values */

	s16b fast; /* Timed -- Fast */
	s16b slow; /* Timed -- Slow */
	s16b blind;	/* Timed -- Blindness */
	s16b paralyzed;	/* Timed -- Paralysis */
	s16b confused; /* Timed -- Confusion */
	s16b afraid; /* Timed -- Fear */
	s16b image;	/* Timed -- Hallucination */
	s16b poisoned; /* Timed -- Poisoned */
	s16b cut; /* Timed -- Cut */
	s16b stun; /* Timed -- Stun */

	s16b protevil; /* Timed -- Protection */
	s16b invuln; /* Timed -- Invulnerable */
	s16b hero; /* Timed -- Heroism */
	s16b shero;	/* Timed -- Super Heroism */
	s16b shield; /* Timed -- Shield Spell */
	s16b blessed; /* Timed -- Blessed */
	s16b tim_invis;	/* Timed -- See Invisible */
	s16b tim_infra;	/* Timed -- Infra Vision */

	s16b oppose_acid; /* Timed -- oppose acid */
	s16b oppose_elec; /* Timed -- oppose lightning */
	s16b oppose_fire; /* Timed -- oppose heat */
	s16b oppose_cold; /* Timed -- oppose cold */
	s16b oppose_pois; /* Timed -- oppose poison */


	s16b tim_esp; /* Timed ESP */
	s16b wraith_form; /* Timed wraithform */

	s16b resist_magic; /* Timed Resist Magic (later) */
	s16b tim_xtra1;	/* Later */
	s16b tim_xtra2;	/* Later */
	s16b tim_xtra3;	/* Later */
	s16b tim_xtra4;	/* Later */
	s16b tim_xtra5;	/* Later */
	s16b tim_xtra6;	/* Later */
	s16b tim_xtra7;	/* Later */
	s16b tim_xtra8;	/* Later */

	s16b chaos_patron;
	u32b muta1;
	u32b muta2;
	u32b muta3;

	s16b virtues[MAX_PLAYER_VIRTUES];
	s16b vir_types[MAX_PLAYER_VIRTUES];

	s16b word_recall; /* Word of recall counter */

	s16b energy; /* Current energy */

	s16b food; /* Current nutrition */

	s16b total_weight; /* Total weight being carried */

	byte confusing;	/* Glowing hands */
	byte searching;	/* Currently searching */

	s16b health_who; /* Health bar trackee */

	s16b monster_race_idx; /* Monster race trackee */

	s16b object_kind_idx; /* Object kind trackee */

	s16b new_spells; /* Number of spells available */

	s16b old_spells;

	bool old_cumber_armor;
	bool old_cumber_glove;
	bool old_heavy_wield;
	bool old_heavy_shoot;
	bool old_icky_wield;

	s16b old_lite; /* Old radius of lite (if any) */
	s16b old_view; /* Old radius of view (if any) */

	s16b old_food_aux; /* Old value of food */


	bool cumber_armor; /* Mana draining armor */
	bool cumber_glove; /* Mana draining gloves */
	bool heavy_wield; /* Heavy weapon */
	bool heavy_shoot; /* Heavy shooter */
	bool icky_wield; /* Icky weapon */

	s16b cur_lite; /* Radius of lite (if any) */


	u32b notice; /* Special Updates (bit flags) */
	u32b update; /* Pending Updates (bit flags) */
	u32b redraw; /* Normal Redraws (bit flags) */
	u32b window; /* Window Redraws (bit flags) */

	s16b stat_use[6]; /* Current modified stats */
	s16b stat_top[6]; /* Maximal modified stats */

	s16b stat_add[6]; /* Modifiers to stat values */
	s16b stat_ind[6]; /* Indexes into stat tables */

	bool immune_acid; /* Immunity to acid */
	bool immune_elec; /* Immunity to lightning */
	bool immune_fire; /* Immunity to fire */
	bool immune_cold; /* Immunity to cold */

	bool resist_acid; /* Resist acid */
	bool resist_elec; /* Resist lightning */
	bool resist_fire; /* Resist fire */
	bool resist_cold; /* Resist cold */
	bool resist_pois; /* Resist poison */

	bool resist_conf; /* Resist confusion */
	bool resist_sound; /* Resist sound */
	bool resist_lite; /* Resist light */
	bool resist_dark; /* Resist darkness */
	bool resist_chaos; /* Resist chaos */
	bool resist_disen; /* Resist disenchant */
	bool resist_shard; /* Resist shards */
	bool resist_nexus; /* Resist nexus */
	bool resist_blind; /* Resist blindness */
	bool resist_neth; /* Resist nether */
	bool resist_fear; /* Resist fear */

	bool reflect; /* Reflect 'bolt' attacks */
	bool sh_fire; /* Fiery 'immolation' effect */
	bool sh_elec; /* Electric 'immolation' effect */

	bool anti_magic; /* Anti-magic */
	bool anti_tele;	/* Prevent teleportation */

	bool sustain_str; /* Keep strength */
	bool sustain_int; /* Keep intelligence */
	bool sustain_wis; /* Keep wisdom */
	bool sustain_dex; /* Keep dexterity */
	bool sustain_con; /* Keep constitution */
	bool sustain_chr; /* Keep charisma */

	bool aggravate;	/* Aggravate monsters */
	bool teleport; /* Random teleporting */

	bool exp_drain;	/* Experience draining */

	bool ffall;	/* No damage falling */
	bool lite; /* Permanent light */
	bool free_act; /* Never paralyzed */
	bool see_inv; /* Can see invisible */
	bool regenerate; /* Regenerate hit pts */
	bool hold_life;	/* Resist life draining */
	bool telepathy;	/* Telepathy */
	bool slow_digest; /* Slower digestion */
	bool bless_blade; /* Blessed blade */
	bool xtra_might; /* Extra might bow */
	bool impact; /* Earthquake blows */
	bool pass_wall;	/* Permanent wraithform */

	s16b dis_to_h; /* Known bonus to hit */
	s16b dis_to_d; /* Known bonus to dam */
	s16b dis_to_a; /* Known bonus to ac */

	s16b dis_ac; /* Known base ac */

	s16b to_h; /* Bonus to hit */
	s16b to_d; /* Bonus to dam */
	s16b to_a; /* Bonus to ac */

	s16b ac; /* Base ac */

	s16b see_infra;	/* Infravision range */

	s16b skill_dis;	/* Skill: Disarming */
	s16b skill_dev;	/* Skill: Magic Devices */
	s16b skill_sav;	/* Skill: Saving throw */
	s16b skill_stl;	/* Skill: Stealth factor */
	s16b skill_srh;	/* Skill: Searching ability */
	s16b skill_fos;	/* Skill: Searching frequency */
	s16b skill_thn;	/* Skill: To hit (normal) */
	s16b skill_thb;	/* Skill: To hit (shooting) */
	s16b skill_tht;	/* Skill: To hit (throwing) */
	s16b skill_dig;	/* Skill: Digging */

	s16b num_blow; /* Number of blows */
	s16b num_fire; /* Number of shots */

	byte tval_xtra;	/* Correct xtra tval */

	byte tval_ammo;	/* Correct ammo tval */

	s16b pspeed; /* Current speed */

	/*** Pet commands ***/
	s16b pet_follow_distance; /* Length of the imaginary "leash" for pets */
	byte pet_open_doors; /* flag - allow pets to open doors */
	byte pet_pickup_items; /* flag - allow pets to pickup items */

	/*** Temporary fields ***/
	byte exit_bldg;	/* Goal obtained in arena? -KMW- */
	byte leftbldg; /* did we just leave a special area? -KMW- */
	bool leaving; /* True if player is leaving */

	bool leaving_dungeon; /* True if player is leaving the dungeon */

	s32b align;	/* Good/evil/neutral */
};


/* For Monk martial arts */

typedef struct martial_arts martial_arts;

struct martial_arts
{
	cptr desc; /* A verbose attack description */
	int min_level; /* Minimum level to use */
	int chance;	/* Chance of 'success' */
	int dd;	/* Damage dice */
	int ds;	/* Damage sides */
	int effect;	/* Special effects */
};



/* Mindcrafters */

typedef struct mindcraft_power mindcraft_power;
struct mindcraft_power
{
	int min_lev;
	int mana_cost;
	int fail;
	cptr name;
};


/*
 * A structure to describe a building.
 * From Kamband
 */
typedef struct building_type building_type;

struct building_type
{
	char name[20]; /* proprietor name */
	char owner_name[20]; /* proprietor name */
	char owner_race[20]; /* proprietor race */

	char act_names[6][30]; /* action names */
	s16b member_costs[6]; /* Costs for class members of building */
	s16b other_costs[6]; /* Costs for nonguild members */
	char letters[6]; /* action letters */
	s16b actions[6]; /* action codes */
	s16b action_restr[6]; /* action restrictions */

	s16b member_class[MAX_CLASS]; /* which classes are part of guild */
	s16b member_race[MAX_RACES]; /* which classes are part of guild */
	s16b member_realm[MAX_REALM + 1]; /* which realms are part of guild */
};


/* Border */
typedef struct border_type border_type;
struct border_type
{
	byte north[MAX_WID];
	byte south[MAX_WID];
	byte east[MAX_HGT];
	byte west[MAX_HGT];
	byte north_west;
	byte north_east;
	byte south_west;
	byte south_east;
};


/*
 * A structure describing a wilderness area
 * with a terrain or a town
 */
typedef struct wilderness_type wilderness_type;
struct wilderness_type
{
	int terrain;
	int town;
	int road;
	u32b seed;
	byte level;
};


/*
 * A structure describing a town with
 * stores and buildings
 */
typedef struct town_type town_type;
struct town_type
{
	char name[32];
	u32b seed; /* Seed for RNG */
	store_type *store; /* The stores [MAX_STORES] */
	byte numstores;
};

/* Dungeons */
typedef struct dun_type dun_type;
struct dun_type
{
	byte min_level;	/* Minimum level in the dungeon */
	byte max_level;	/* Maximum dungeon level allowed */

	cptr name; /* The name of the dungeon */
};

/*
 * Sort-array element
 */
typedef struct tag_type tag_type;

struct tag_type
{
	int tag;
	void *pointer;
};

typedef bool(*monster_hook_type) (int r_idx);


/*
 * This seems like a pretty standard "typedef"
 */
typedef int (*inven_func) (object_type *);


/*
 * Semi-Portable High Score List Entry (128 bytes) -- BEN
 *
 * All fields listed below are null terminated ascii strings.
 *
 * In addition, the "number" fields are right justified, and
 * space padded, to the full available length (minus the "null").
 *
 * Note that "string comparisons" are thus valid on "pts".
 */

typedef struct high_score high_score;

struct high_score
{
	char what[8]; /* Version info (string) */

	char pts[10]; /* Total Score (number) */

	char gold[10]; /* Total Gold (number) */

	char turns[10];	/* Turns Taken (number) */

	char day[10]; /* Time stamp (string) */

	char who[16]; /* Player Name (string) */

	char uid[8]; /* Player UID (number) */

	char sex[2]; /* Player Sex (string) */
	char p_r[3]; /* Player Race (number) */
	char p_c[3]; /* Player Class (number) */

	char cur_lev[4]; /* Current Player Level (number) */
	char cur_dun[4]; /* Current Dungeon Level (number) */
	char max_lev[4]; /* Max Player Level (number) */
	char max_dun[4]; /* Max Dungeon Level (number) */

	char how[32]; /* Method of death (string) */
};
