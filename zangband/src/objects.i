%module objects

%{

#include "angband.h"
#include "Python.h"

%}

typedef const char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;

%typemap(memberin) byte[4]
{
	$target[0] = $source[0];
	$target[1] = $source[1];
	$target[2] = $source[2];
	$target[3] = $source[3];
}

%typemap(memberout) byte[4]
{
	$target[0] = $source[0];
	$target[1] = $source[1];
	$target[2] = $source[2];
	$target[3] = $source[3];
}

%typemap(python,in) PyObject*
{
	$target = $source;
}

%typemap(python,out) PyObject*
{
	$target = $source;
}


typedef struct object_kind
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Object type */
	byte sval;			/* Object sub type */

	s16b pval;			/* Object extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage dice/sides */

	s16b weight;		/* Weight */

	s32b cost;			/* Object "base cost" */

	u32b flags1;		/* Flags, set 1 */
	u32b flags2;		/* Flags, set 2 */
	u32b flags3;		/* Flags, set 3 */

	byte locale[4];		/* Allocation level(s) */
	byte chance[4];		/* Allocation chance(s) */

	byte level;			/* Level */
	byte extra;			/* Something */


	byte d_attr;		/* Default object attribute */
	char d_char;		/* Default object character */


	byte x_attr;		/* Desired object attribute */
	char x_char;		/* Desired object character */


	byte flavor;		/* Special object flavor (or zero) */

	bool easy_know;		/* This object is always known (if aware) */


	bool aware;			/* The player is "aware" of the item's effects */

	bool tried;			/* The player has "tried" one of the items */
} object_kind;


typedef struct object_type
{
	s16b k_idx;			/* Kind index (zero if "dead") */

	byte iy;			/* Y-position on map, or zero */
	byte ix;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	s16b pval;			/* Item extra-parameter */

	byte discount;		/* Discount (if any) */

	byte number;		/* Number of items */

	s16b weight;		/* Item weight */

	byte name1;			/* Artifact type, if any */
	byte name2;			/* Ego-Item type, if any */

	byte xtra1;			/* Extra info type */
	byte xtra2;			/* Extra info index */

	s16b to_h;			/* Plusses to hit */
	s16b to_d;			/* Plusses to damage */
	s16b to_a;			/* Plusses to AC */

	s16b ac;			/* Normal AC */

	byte dd, ds;		/* Damage dice/sides */

	s16b timeout;		/* Timeout Counter */

	byte ident;			/* Special flags  */

	byte marked;		/* Object is marked */

	u16b inscription;	/* Inscription index */
	u16b art_name;      /* Artifact name (random artifacts) */

	u32b art_flags1;    /* Flags, set 1  Alas, these were necessary */
	u32b art_flags2;    /* Flags, set 2  for the random artifacts of*/
	u32b art_flags3;    /* Flags, set 3  Zangband */

	s16b next_o_idx;	/* Next object in stack (if any) */

	s16b held_m_idx;	/* Monster holding us (if any) */

	%addmethods
	{
		object_type(void)
		{
			object_type *o_ptr = (object_type*) malloc(sizeof(object_type));
			object_wipe(o_ptr);

#if 0
			/* Add a reference to new callback */
			Py_XINCREF(self);

			o_ptr->python = self;
#endif
			return (o_ptr);
		}

		~object_type(void)
		{
			free(self);
		}

		void object_wipe(void)
		{
			object_wipe(self);
		}

		void object_copy(object_type *j_ptr)
		{
			object_copy(j_ptr, self);
		}

		void object_prep(int k_idx)
		{
			object_prep(self, k_idx);

			self->python = object_create_callback(self);
		}

		s16b drop_near(int chance, int y, int x)
		{
			return drop_near(self, chance, y, x);
		}

		cptr activation(void)
		{
			return item_activation(self);
		}

		bool identify_fully_aux(void)
		{
			return identify_fully_aux(self);
		}

		void object_known(void)
		{
			object_known(self);
		}

		void object_aware(void)
		{
			object_aware(self);
		}

		void set_python(PyObject *python)
		{
			self->python = python;
		}
	}
} object_type;


/*** Object "tval" and "sval" codes ***/


/*
 * The values for the "tval" field of various objects.
 *
 * This value is the primary means by which items are sorted in the
 * player inventory, followed by "sval" and "cost".
 *
 * Note that a "BOW" with tval = 19 and sval S = 10*N+P takes a missile
 * weapon with tval = 16+N, and does (xP) damage when so combined.  This
 * fact is not actually used in the source, but it kind of interesting.
 *
 * Note that as of 2.7.8, the "item flags" apply to all items, though
 * only armor and weapons and a few other items use any of these flags.
 */

#define TV_SKELETON      1      /* Skeletons ('s') */
#define TV_BOTTLE        2      /* Empty bottles ('!') */
#define TV_JUNK          3      /* Sticks, Pottery, etc ('~') */
#define TV_SPIKE         5      /* Spikes ('~') */
#define TV_CHEST         7      /* Chests ('~') */
#define TV_SHOT         16      /* Ammo for slings */
#define TV_ARROW        17      /* Ammo for bows */
#define TV_BOLT         18      /* Ammo for x-bows */
#define TV_BOW          19      /* Slings/Bows/Xbows */
#define TV_DIGGING      20      /* Shovels/Picks */
#define TV_HAFTED       21      /* Priest Weapons */
#define TV_POLEARM      22      /* Axes and Pikes */
#define TV_SWORD        23      /* Edged Weapons */
#define TV_BOOTS        30      /* Boots */
#define TV_GLOVES       31      /* Gloves */
#define TV_HELM         32      /* Helms */
#define TV_CROWN        33      /* Crowns */
#define TV_SHIELD       34      /* Shields */
#define TV_CLOAK        35      /* Cloaks */
#define TV_SOFT_ARMOR   36      /* Soft Armor */
#define TV_HARD_ARMOR   37      /* Hard Armor */
#define TV_DRAG_ARMOR   38      /* Dragon Scale Mail */
#define TV_LITE         39      /* Lites (including Specials) */
#define TV_AMULET       40      /* Amulets (including Specials) */
#define TV_RING         45      /* Rings (including Specials) */
#define TV_STAFF        55
#define TV_WAND         65
#define TV_ROD          66
#define TV_SCROLL       70
#define TV_POTION       75
#define TV_FLASK        77
#define TV_FOOD         80
#define TV_LIFE_BOOK    90
#define TV_SORCERY_BOOK 91
#define TV_NATURE_BOOK  92
#define TV_CHAOS_BOOK   93
#define TV_DEATH_BOOK   94
#define TV_TRUMP_BOOK   95
#define TV_ARCANE_BOOK  96
#define TV_GOLD         100     /* Gold can only be picked up by players */



/* The "sval" codes for TV_SHOT/TV_ARROW/TV_BOLT */
#define SV_AMMO_LIGHT                    0	/* pebbles */
#define SV_AMMO_NORMAL                   1	/* shots, arrows, bolts */
#define SV_AMMO_HEAVY                    2	/* seeker arrows and bolts, mithril shots */

/* The "sval" codes for TV_BOW (note information in "sval") */
#define SV_SLING                         2	/* (x2) */
#define SV_SHORT_BOW                    12	/* (x2) */
#define SV_LONG_BOW                     13	/* (x3) */
#define SV_LIGHT_XBOW                   23	/* (x3) */
#define SV_HEAVY_XBOW                   24	/* (x4) */

/* The "sval" codes for TV_DIGGING */
#define SV_SHOVEL                        1
#define SV_GNOMISH_SHOVEL                2
#define SV_DWARVEN_SHOVEL                3
#define SV_PICK                          4
#define SV_ORCISH_PICK                   5
#define SV_DWARVEN_PICK                  6
#define SV_MATTOCK                       7

/* The "sval" values for TV_HAFTED */
#define SV_CLUB                          1	/* 1d4  */
#define SV_WHIP                          2	/* 1d6  */
#define SV_QUARTERSTAFF                  3	/* 1d9  */
#define SV_NUNCHAKU                      4	/* 2d3  */
#define SV_MACE                          5	/* 2d4  */
#define SV_BALL_AND_CHAIN                6	/* 2d4  */
#define SV_JO_STAFF                      7	/* 1d7  */
#define SV_WAR_HAMMER                    8	/* 3d3  */
#define SV_LUCERN_HAMMER                10	/* 2d5  */
#define SV_THREE_PIECE_ROD              11	/* 3d3  */
#define SV_MORNING_STAR                 12	/* 2d6  */
#define SV_FLAIL                        13	/* 2d6  */
#define SV_BO_STAFF                     14	/* 1d11 */
#define SV_LEAD_FILLED_MACE             15	/* 3d4  */
#define SV_TETSUBO                      16	/* 2d7  */
#define SV_TWO_HANDED_FLAIL             18	/* 3d6  */
#define SV_GREAT_HAMMER                 19	/* 4d6  */
#define SV_MACE_OF_DISRUPTION           20	/* 5d8  */
#define SV_GROND                        50	/* 3d4  */

/* The "sval" values for TV_POLEARM */
#define SV_HATCHET                       1	/* 1d5 */
#define SV_SPEAR                         2	/* 1d6 */
#define SV_SICKLE                        3	/* 2d3 */
#define SV_AWL_PIKE                      4	/* 1d8 */
#define SV_TRIDENT                       5	/* 1d9 */
#define SV_FAUCHARD                      6  /* 1d10 */
#define SV_BROAD_SPEAR                   7	/* 1d9 */
#define SV_PIKE                          8	/* 2d5 */
#define SV_NAGINATA                      9  /* 2d6 */
#define SV_BEAKED_AXE                   10	/* 2d6 */
#define SV_BROAD_AXE                    11	/* 2d6 */
#define SV_GLAIVE                       13	/* 2d6 */
#define SV_LAJATANG                     14	/* 2d7 */
#define SV_HALBERD                      15	/* 3d4 */
#define SV_GUISARME                     16  /* 2d5 */
#define SV_SCYTHE                       17	/* 5d3 */
#define SV_LANCE                        20	/* 2d8 */
#define SV_BATTLE_AXE                   22	/* 2d8 */
#define SV_GREAT_AXE                    25	/* 4d4 */
#define SV_TRIFURCATE_SPEAR             26	/* 2d9 */
#define SV_LOCHABER_AXE                 28	/* 3d8 */
#define SV_HEAVY_LANCE                  29  /* 4d8 */
#define SV_SCYTHE_OF_SLICING            30	/* 8d4 */

/* The "sval" codes for TV_SWORD */
#define SV_BROKEN_DAGGER                 1  /* 1d1 */
#define SV_BROKEN_SWORD                  2  /* 1d2 */
#define SV_DAGGER                        4  /* 1d4 */
#define SV_MAIN_GAUCHE                   5  /* 1d5 */
#define SV_TANTO                         6  /* 1d5 */
#define SV_RAPIER                        7  /* 1d6 */
#define SV_SMALL_SWORD                   8  /* 1d6 */
#define SV_BASILLARD                     9  /* 1d8 */
#define SV_SHORT_SWORD                  10  /* 1d7 */
#define SV_SABRE                        11  /* 1d7 */
#define SV_CUTLASS                      12  /* 1d7 */
#define SV_WAKIZASHI                    13  /* 2d4 */
#define SV_KHOPESH                      14  /* 2d4 */
#define SV_TULWAR                       15  /* 2d4 */
#define SV_BROAD_SWORD                  16  /* 2d5 */
#define SV_LONG_SWORD                   17  /* 2d5 */
#define SV_SCIMITAR                     18  /* 2d5 */
#define SV_NINJATO                      19  /* 1d9 */
#define SV_KATANA                       20  /* 3d4 */
#define SV_BASTARD_SWORD                21  /* 3d4 */
#define SV_GREAT_SCIMITAR               22  /* 4d5 */
#define SV_CLAYMORE                     23  /* 2d8 */
#define SV_ESPADON                      24  /* 2d9 */
#define SV_TWO_HANDED_SWORD             25  /* 3d6 */
#define SV_FLAMBERGE                    26  /* 3d7 */
#define SV_NO_DACHI                     27  /* 5d4 */
#define SV_EXECUTIONERS_SWORD           28  /* 4d5 */
#define SV_ZWEIHANDER                   29  /* 4d6 */
#define SV_BLADE_OF_CHAOS               30  /* 6d5 */

/* The "sval" codes for TV_SHIELD */
#define SV_SMALL_LEATHER_SHIELD          2
#define SV_SMALL_METAL_SHIELD            3
#define SV_LARGE_LEATHER_SHIELD          4
#define SV_LARGE_METAL_SHIELD            5
#define SV_DRAGON_SHIELD                 6
#define SV_SHIELD_OF_DEFLECTION         10

/* The "sval" codes for TV_HELM */
#define SV_HARD_LEATHER_CAP              2
#define SV_METAL_CAP                     3
#define SV_JINGASA                       4  /* 4 */
#define SV_IRON_HELM                     5
#define SV_STEEL_HELM                    6
#define SV_DRAGON_HELM                   7
#define SV_KABUTO                        8  /* 7 */
#define SV_IRON_CROWN                   10
#define SV_GOLDEN_CROWN                 11
#define SV_JEWELED_CROWN                12
#define SV_MORGOTH                      50

/* The "sval" codes for TV_BOOTS */
#define SV_PAIR_OF_SOFT_LEATHER_BOOTS    2
#define SV_PAIR_OF_HARD_LEATHER_BOOTS    3
#define SV_PAIR_OF_METAL_SHOD_BOOTS      6

/* The "sval" codes for TV_CLOAK */
#define SV_CLOAK                         1
#define SV_ELVEN_CLOAK                   2
#define SV_FUR_CLOAK                     3
#define SV_SHADOW_CLOAK                  6

/* The "sval" codes for TV_GLOVES */
#define SV_SET_OF_LEATHER_GLOVES         1
#define SV_SET_OF_GAUNTLETS              2
#define SV_SET_OF_CESTI                  5

/* The "sval" codes for TV_SOFT_ARMOR */
#define SV_FILTHY_RAG                    1
#define SV_ROBE                          2
#define SV_PAPER_ARMOR                   3  /* 4 */
#define SV_SOFT_LEATHER_ARMOR            4
#define SV_SOFT_STUDDED_LEATHER          5
#define SV_HARD_LEATHER_ARMOR            6
#define SV_HARD_STUDDED_LEATHER          7
#define SV_RHINO_HIDE_ARMOR              8
#define SV_CORD_ARMOR                    9  /*  6 */
#define SV_PADDED_ARMOR                 10  /*  4 */
#define SV_LEATHER_SCALE_MAIL           11
#define SV_LEATHER_JACK                 12
#define SV_STONE_AND_HIDE_ARMOR         15  /* 15 */

/* The "sval" codes for TV_HARD_ARMOR */
#define SV_RUSTY_CHAIN_MAIL              1  /* 14- */
#define SV_RING_MAIL                     2  /* 12  */
#define SV_METAL_SCALE_MAIL              3  /* 13  */
#define SV_CHAIN_MAIL                    4  /* 14  */
#define SV_DOUBLE_RING_MAIL              5  /* 15  */
#define SV_AUGMENTED_CHAIN_MAIL          6  /* 16  */
#define SV_DOUBLE_CHAIN_MAIL             7  /* 16  */
#define SV_BAR_CHAIN_MAIL                8  /* 18  */
#define SV_METAL_BRIGANDINE_ARMOUR       9  /* 19  */
#define SV_SPLINT_MAIL                  10  /* 19  */
#define SV_DO_MARU                      11  /* 20  */
#define SV_PARTIAL_PLATE_ARMOUR         12  /* 22  */
#define SV_METAL_LAMELLAR_ARMOUR        13  /* 23  */
#define SV_HARAMAKIDO                   14  /* 17  */
#define SV_FULL_PLATE_ARMOUR            15  /* 25  */
#define SV_O_YOROI                      16  /* 24  */
#define SV_RIBBED_PLATE_ARMOUR          18  /* 28  */
#define SV_MITHRIL_CHAIN_MAIL           20  /* 28+ */
#define SV_MITHRIL_PLATE_MAIL           25  /* 35+ */
#define SV_ADAMANTITE_PLATE_MAIL        30  /* 40+ */

/* The "sval" codes for TV_DRAG_ARMOR */
#define SV_DRAGON_BLACK                  1
#define SV_DRAGON_BLUE                   2
#define SV_DRAGON_WHITE                  3
#define SV_DRAGON_RED                    4
#define SV_DRAGON_GREEN                  5
#define SV_DRAGON_MULTIHUED              6
#define SV_DRAGON_SHINING               10
#define SV_DRAGON_LAW                   12
#define SV_DRAGON_BRONZE                14
#define SV_DRAGON_GOLD                  16
#define SV_DRAGON_CHAOS                 18
#define SV_DRAGON_BALANCE               20
#define SV_DRAGON_POWER                 30

/* The sval codes for TV_LITE */
#define SV_LITE_TORCH                    0
#define SV_LITE_LANTERN                  1
#define SV_LITE_GALADRIEL                4
#define SV_LITE_ELENDIL                  5
#define SV_LITE_THRAIN                   6

/* The "sval" codes for TV_AMULET */
#define SV_AMULET_DOOM                   0
#define SV_AMULET_TELEPORT               1
#define SV_AMULET_ADORNMENT              2
#define SV_AMULET_SLOW_DIGEST            3
#define SV_AMULET_RESIST_ACID            4
#define SV_AMULET_SEARCHING              5
#define SV_AMULET_WISDOM                 6
#define SV_AMULET_CHARISMA               7
#define SV_AMULET_THE_MAGI               8
#define SV_AMULET_REFLECTION             9
#define SV_AMULET_CARLAMMAS             10
#define SV_AMULET_INGWE                 11
#define SV_AMULET_DWARVES               12
#define SV_AMULET_NO_MAGIC              13
#define SV_AMULET_NO_TELE               14
#define SV_AMULET_RESISTANCE            15

/* The sval codes for TV_RING */
#define SV_RING_WOE                      0
#define SV_RING_AGGRAVATION              1
#define SV_RING_WEAKNESS                 2
#define SV_RING_STUPIDITY                3
#define SV_RING_TELEPORTATION            4
#define SV_RING_SLOW_DIGESTION           6
#define SV_RING_FEATHER_FALL             7
#define SV_RING_RESIST_FIRE              8
#define SV_RING_RESIST_COLD              9
#define SV_RING_SUSTAIN_STR             10
#define SV_RING_SUSTAIN_INT             11
#define SV_RING_SUSTAIN_WIS             12
#define SV_RING_SUSTAIN_DEX             13
#define SV_RING_SUSTAIN_CON             14
#define SV_RING_SUSTAIN_CHR             15
#define SV_RING_PROTECTION              16
#define SV_RING_ACID                    17
#define SV_RING_FLAMES                  18
#define SV_RING_ICE                     19
#define SV_RING_RESIST_POIS             20
#define SV_RING_FREE_ACTION             21
#define SV_RING_SEE_INVIS               22
#define SV_RING_SEARCHING               23
#define SV_RING_STR                     24
#define SV_RING_INT                     25
#define SV_RING_DEX                     26
#define SV_RING_CON                     27
#define SV_RING_ACCURACY                28
#define SV_RING_DAMAGE                  29
#define SV_RING_SLAYING                 30
#define SV_RING_SPEED                   31
#define SV_RING_BARAHIR                 32
#define SV_RING_TULKAS                  33
#define SV_RING_NARYA                   34
#define SV_RING_NENYA                   35
#define SV_RING_VILYA                   36
#define SV_RING_POWER                   37
#define SV_RING_RES_FEAR                38
#define SV_RING_RES_LD                  39
#define SV_RING_RES_NETHER              40
#define SV_RING_RES_NEXUS               41
#define SV_RING_RES_SOUND               42
#define SV_RING_RES_CONFUSION           43
#define SV_RING_RES_SHARDS              44
#define SV_RING_RES_DISENCHANT          45
#define SV_RING_RES_CHAOS               46
#define SV_RING_RES_BLINDNESS           47
#define SV_RING_LORDLY                  48
#define SV_RING_ATTACKS                 49

/* The "sval" codes for TV_STAFF */
#define SV_STAFF_DARKNESS                0
#define SV_STAFF_SLOWNESS                1
#define SV_STAFF_HASTE_MONSTERS          2
#define SV_STAFF_SUMMONING               3
#define SV_STAFF_TELEPORTATION           4
#define SV_STAFF_IDENTIFY                5
#define SV_STAFF_REMOVE_CURSE            6
#define SV_STAFF_STARLITE                7
#define SV_STAFF_LITE                    8
#define SV_STAFF_MAPPING                 9
#define SV_STAFF_DETECT_GOLD            10
#define SV_STAFF_DETECT_ITEM            11
#define SV_STAFF_DETECT_TRAP            12
#define SV_STAFF_DETECT_DOOR            13
#define SV_STAFF_DETECT_INVIS           14
#define SV_STAFF_DETECT_EVIL            15
#define SV_STAFF_CURE_LIGHT             16
#define SV_STAFF_CURING                 17
#define SV_STAFF_HEALING                18
#define SV_STAFF_THE_MAGI               19
#define SV_STAFF_SLEEP_MONSTERS         20
#define SV_STAFF_SLOW_MONSTERS          21
#define SV_STAFF_SPEED                  22
#define SV_STAFF_PROBING                23
#define SV_STAFF_DISPEL_EVIL            24
#define SV_STAFF_POWER                  25
#define SV_STAFF_HOLINESS               26
#define SV_STAFF_GENOCIDE               27
#define SV_STAFF_EARTHQUAKES            28
#define SV_STAFF_DESTRUCTION            29


/* The "sval" codes for TV_WAND */
#define SV_WAND_HEAL_MONSTER             0
#define SV_WAND_HASTE_MONSTER            1
#define SV_WAND_CLONE_MONSTER            2
#define SV_WAND_TELEPORT_AWAY            3
#define SV_WAND_DISARMING                4
#define SV_WAND_TRAP_DOOR_DEST           5
#define SV_WAND_STONE_TO_MUD             6
#define SV_WAND_LITE                     7
#define SV_WAND_SLEEP_MONSTER            8
#define SV_WAND_SLOW_MONSTER             9
#define SV_WAND_CONFUSE_MONSTER         10
#define SV_WAND_FEAR_MONSTER            11
#define SV_WAND_DRAIN_LIFE              12
#define SV_WAND_POLYMORPH               13
#define SV_WAND_STINKING_CLOUD          14
#define SV_WAND_MAGIC_MISSILE           15
#define SV_WAND_ACID_BOLT               16
#define SV_WAND_CHARM_MONSTER           17
#define SV_WAND_FIRE_BOLT               18
#define SV_WAND_COLD_BOLT               19
#define SV_WAND_ACID_BALL               20
#define SV_WAND_ELEC_BALL               21
#define SV_WAND_FIRE_BALL               22
#define SV_WAND_COLD_BALL               23
#define SV_WAND_WONDER                  24
#define SV_WAND_ANNIHILATION            25
#define SV_WAND_DRAGON_FIRE             26
#define SV_WAND_DRAGON_COLD             27
#define SV_WAND_DRAGON_BREATH           28
#define SV_WAND_ROCKETS                 29

/* The "sval" codes for TV_ROD */
#define SV_ROD_DETECT_TRAP               0
#define SV_ROD_DETECT_DOOR               1
#define SV_ROD_IDENTIFY                  2
#define SV_ROD_RECALL                    3
#define SV_ROD_ILLUMINATION              4
#define SV_ROD_MAPPING                   5
#define SV_ROD_DETECTION                 6
#define SV_ROD_PROBING                   7
#define SV_ROD_CURING                    8
#define SV_ROD_HEALING                   9
#define SV_ROD_RESTORATION              10
#define SV_ROD_SPEED                    11
#define SV_ROD_PESTICIDE                12
#define SV_ROD_TELEPORT_AWAY            13
#define SV_ROD_DISARMING                14
#define SV_ROD_LITE                     15
#define SV_ROD_SLEEP_MONSTER            16
#define SV_ROD_SLOW_MONSTER             17
#define SV_ROD_DRAIN_LIFE               18
#define SV_ROD_POLYMORPH                19
#define SV_ROD_ACID_BOLT                20
#define SV_ROD_ELEC_BOLT                21
#define SV_ROD_FIRE_BOLT                22
#define SV_ROD_COLD_BOLT                23
#define SV_ROD_ACID_BALL                24
#define SV_ROD_ELEC_BALL                25
#define SV_ROD_FIRE_BALL                26
#define SV_ROD_COLD_BALL                27
#define SV_ROD_HAVOC                    28


/* The "sval" codes for TV_SCROLL */

#define SV_SCROLL_DARKNESS               0
#define SV_SCROLL_AGGRAVATE_MONSTER      1
#define SV_SCROLL_CURSE_ARMOR            2
#define SV_SCROLL_CURSE_WEAPON           3
#define SV_SCROLL_SUMMON_MONSTER         4
#define SV_SCROLL_SUMMON_UNDEAD          5
/* xxx (summon?) */
#define SV_SCROLL_TRAP_CREATION          7
#define SV_SCROLL_PHASE_DOOR             8
#define SV_SCROLL_TELEPORT               9
#define SV_SCROLL_TELEPORT_LEVEL        10
#define SV_SCROLL_WORD_OF_RECALL        11
#define SV_SCROLL_IDENTIFY              12
#define SV_SCROLL_STAR_IDENTIFY         13
#define SV_SCROLL_REMOVE_CURSE          14
#define SV_SCROLL_STAR_REMOVE_CURSE     15
#define SV_SCROLL_ENCHANT_ARMOR         16
#define SV_SCROLL_ENCHANT_WEAPON_TO_HIT 17
#define SV_SCROLL_ENCHANT_WEAPON_TO_DAM 18
/* xxx enchant missile? */
#define SV_SCROLL_STAR_ENCHANT_ARMOR    20
#define SV_SCROLL_STAR_ENCHANT_WEAPON   21
#define SV_SCROLL_RECHARGING            22
/* xxx */
#define SV_SCROLL_LIGHT                 24
#define SV_SCROLL_MAPPING               25
#define SV_SCROLL_DETECT_GOLD           26
#define SV_SCROLL_DETECT_ITEM           27
#define SV_SCROLL_DETECT_TRAP           28
#define SV_SCROLL_DETECT_DOOR           29
#define SV_SCROLL_DETECT_INVIS          30
/* xxx (detect evil?) */
#define SV_SCROLL_SATISFY_HUNGER        32
#define SV_SCROLL_BLESSING              33
#define SV_SCROLL_HOLY_CHANT            34
#define SV_SCROLL_HOLY_PRAYER           35
#define SV_SCROLL_MONSTER_CONFUSION     36
#define SV_SCROLL_PROTECTION_FROM_EVIL  37
#define SV_SCROLL_RUNE_OF_PROTECTION    38
#define SV_SCROLL_TRAP_DOOR_DESTRUCTION 39
/* xxx */
#define SV_SCROLL_STAR_DESTRUCTION      41
#define SV_SCROLL_DISPEL_UNDEAD         42
/* xxx */
#define SV_SCROLL_GENOCIDE              44
#define SV_SCROLL_MASS_GENOCIDE         45
#define SV_SCROLL_ACQUIREMENT           46
#define SV_SCROLL_STAR_ACQUIREMENT      47
#define SV_SCROLL_FIRE                  48
#define SV_SCROLL_ICE                   49
#define SV_SCROLL_CHAOS                 50
#define SV_SCROLL_RUMOR                 51
#define SV_SCROLL_ARTIFACT              52

/* The "sval" codes for TV_POTION */
#define SV_POTION_WATER                  0
#define SV_POTION_APPLE_JUICE            1
#define SV_POTION_SLIME_MOLD             2
/* xxx (fixed color) */
#define SV_POTION_SLOWNESS               4
#define SV_POTION_SALT_WATER             5
#define SV_POTION_POISON                 6
#define SV_POTION_BLINDNESS              7
/* xxx */
#define SV_POTION_CONFUSION              9
/* xxx */
#define SV_POTION_SLEEP                 11
/* xxx */
#define SV_POTION_LOSE_MEMORIES         13
/* xxx */
#define SV_POTION_RUINATION             15
#define SV_POTION_DEC_STR               16
#define SV_POTION_DEC_INT               17
#define SV_POTION_DEC_WIS               18
#define SV_POTION_DEC_DEX               19
#define SV_POTION_DEC_CON               20
#define SV_POTION_DEC_CHR               21
#define SV_POTION_DETONATIONS           22
#define SV_POTION_DEATH                 23
#define SV_POTION_INFRAVISION           24
#define SV_POTION_DETECT_INVIS          25
#define SV_POTION_SLOW_POISON           26
#define SV_POTION_CURE_POISON           27
#define SV_POTION_BOLDNESS              28
#define SV_POTION_SPEED                 29
#define SV_POTION_RESIST_HEAT           30
#define SV_POTION_RESIST_COLD           31
#define SV_POTION_HEROISM               32
#define SV_POTION_BESERK_STRENGTH       33
#define SV_POTION_CURE_LIGHT            34
#define SV_POTION_CURE_SERIOUS          35
#define SV_POTION_CURE_CRITICAL         36
#define SV_POTION_HEALING               37
#define SV_POTION_STAR_HEALING          38
#define SV_POTION_LIFE                  39
#define SV_POTION_RESTORE_MANA          40
#define SV_POTION_RESTORE_EXP           41
#define SV_POTION_RES_STR               42
#define SV_POTION_RES_INT               43
#define SV_POTION_RES_WIS               44
#define SV_POTION_RES_DEX               45
#define SV_POTION_RES_CON               46
#define SV_POTION_RES_CHR               47
#define SV_POTION_INC_STR               48
#define SV_POTION_INC_INT               49
#define SV_POTION_INC_WIS               50
#define SV_POTION_INC_DEX               51
#define SV_POTION_INC_CON               52
#define SV_POTION_INC_CHR               53
/* xxx */
#define SV_POTION_AUGMENTATION          55
#define SV_POTION_ENLIGHTENMENT         56
#define SV_POTION_STAR_ENLIGHTENMENT    57
#define SV_POTION_SELF_KNOWLEDGE        58
#define SV_POTION_EXPERIENCE            59
#define SV_POTION_RESISTANCE            60
#define SV_POTION_CURING                61
#define SV_POTION_INVULNERABILITY       62
#define SV_POTION_NEW_LIFE              63

/* The "sval" codes for TV_FOOD */
#define SV_FOOD_POISON                   0
#define SV_FOOD_BLINDNESS                1
#define SV_FOOD_PARANOIA                 2
#define SV_FOOD_CONFUSION                3
#define SV_FOOD_HALLUCINATION            4
#define SV_FOOD_PARALYSIS                5
#define SV_FOOD_WEAKNESS                 6
#define SV_FOOD_SICKNESS                 7
#define SV_FOOD_STUPIDITY                8
#define SV_FOOD_NAIVETY                  9
#define SV_FOOD_UNHEALTH                10
#define SV_FOOD_DISEASE                 11
#define SV_FOOD_CURE_POISON             12
#define SV_FOOD_CURE_BLINDNESS          13
#define SV_FOOD_CURE_PARANOIA           14
#define SV_FOOD_CURE_CONFUSION          15
#define SV_FOOD_CURE_SERIOUS            16
#define SV_FOOD_RESTORE_STR             17
#define SV_FOOD_RESTORE_CON             18
#define SV_FOOD_RESTORING               19
/* many missing mushrooms */
#define SV_FOOD_BISCUIT                 32
#define SV_FOOD_JERKY                   33
#define SV_FOOD_RATION                  35
#define SV_FOOD_SLIME_MOLD              36
#define SV_FOOD_WAYBREAD                37
#define SV_FOOD_PINT_OF_ALE             38
#define SV_FOOD_PINT_OF_WINE            39


/*
 * Special "sval" limit -- first "normal" food
 */
#define SV_FOOD_MIN_FOOD                32

/*
 * Special "sval" limit -- first "aimed" rod
 */
#define SV_ROD_MIN_DIRECTION    12

/*
 * Special "sval" limit -- first "large" chest
 */
#define SV_CHEST_MIN_LARGE      4

/*
 * Special "sval" limit -- first "good" magic/prayer book
 */
#define SV_BOOK_MIN_GOOD    2


#define OBJ_GOLD_LIST   480     /* First "gold" entry */
#define MAX_GOLD        18      /* Number of "gold" entries */



struct artifact_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Artifact type */
	byte sval;			/* Artifact sub type */

	s16b pval;			/* Artifact extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage when hits */

	s16b weight;		/* Weight */

	s32b cost;			/* Artifact "cost" */

	u32b flags1;		/* Artifact Flags, set 1 */
	u32b flags2;		/* Artifact Flags, set 2 */
	u32b flags3;		/* Artifact Flags, set 3 */

	byte level;			/* Artifact level */
	byte rarity;		/* Artifact rarity */

	byte cur_num;		/* Number created (0 or 1) */
	byte max_num;		/* Unused (should be "1") */
};


/*
 * Information about "ego-items".
 */

struct ego_item_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte slot;			/* Standard slot value */
	byte rating;		/* Rating boost */

	byte level;			/* Minimum level */
	byte rarity;		/* Object rarity */

	byte max_to_h;		/* Maximum to-hit bonus */
	byte max_to_d;		/* Maximum to-dam bonus */
	byte max_to_a;		/* Maximum to-ac bonus */

	byte max_pval;		/* Maximum pval */

	s32b cost;			/* Ego-item "cost" */

	u32b flags1;		/* Ego-Item Flags, set 1 */
	u32b flags2;		/* Ego-Item Flags, set 2 */
	u32b flags3;		/* Ego-Item Flags, set 3 */
};


/*
 * Special Object Flags
 */
#define IDENT_SENSE     0x01    /* Item has been "sensed" */
#define IDENT_FIXED     0x02    /* Item has been "haggled" */
#define IDENT_EMPTY     0x04    /* Item charges are known */
#define IDENT_KNOWN     0x08    /* Item abilities are known */
#define IDENT_STOREB    0x10    /* Item is storebought !!!! */
#define IDENT_MENTAL    0x20    /* Item information is known */
#define IDENT_CURSED    0x40    /* Item is temporarily cursed */
#define IDENT_BROKEN    0x80    /* Item is permanently worthless */


s16b lookup_kind(int tval, int sval);
