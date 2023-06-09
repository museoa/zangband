/* CVS: Last edit by $Author$ on $Date$ */
/* File: flavor.c */

/* Purpose: Object flavor code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


#include "tnb.h" /* TNB */


/*
 * Hack -- note that "TERM_MULTI" is now just "TERM_VIOLET".
 * We will have to find a cleaner method for "MULTI_HUED" later.
 * There were only two multi-hued "flavors" (one potion, one food).
 * Plus five multi-hued "base-objects" (3 dragon scales, one blade
 * of chaos, and one something else).  See the SHIMMER_OBJECTS code
 * in "dungeon.c" and the object color extractor in "cave.c".
 */
#define TERM_MULTI      TERM_VIOLET


/*
 * Max sizes of the following arrays
 */
#define MAX_ROCKS      56 /* Used with rings (min 38) */
#define MAX_AMULETS    17 /* Used with amulets (min 13) */
#define MAX_WOODS      32 /* Used with staffs (min 30) */
#define MAX_METALS     39 /* Used with wands/rods (min 29/28) */
#define MAX_COLORS     66 /* Used with potions (min 60) */
#define MAX_SHROOM     20 /* Used with mushrooms (min 20) */
#define MAX_TITLES     54 /* Used with scrolls (min 48) */
#define MAX_SYLLABLES 164 /* Used with scrolls (see below) */


/*
 * Rings (adjectives and colors)
 */

static cptr ring_adj[MAX_ROCKS] = {
	"Alexandrite", "Amethyst", "Aquamarine", "Azurite", "Beryl",
	"Bloodstone", "Calcite", "Carnelian", "Corundum", "Diamond",
	"Emerald", "Fluorite", "Garnet", "Granite", "Jade",
	"Jasper", "Lapis Lazuli", "Malachite", "Marble", "Moonstone",
	"Onyx", "Opal", "Pearl", "Quartz", "Quartzite",
	"Rhodonite", "Ruby", "Sapphire", "Tiger Eye", "Topaz",
	"Turquoise", "Zircon", "Platinum", "Bronze", "Gold",
	"Obsidian", "Silver", "Tortoise Shell", "Mithril", "Jet",
	"Engagement", "Adamantite",
	"Wire", "Dilithium", "Bone", "Wooden",
	"Spikard", "Serpent", "Wedding", "Double",
	"Plain", "Brass", "Scarab", "Shining",
	"Rusty", "Transparent"
};

static byte ring_col[MAX_ROCKS] = {
	TERM_GREEN, TERM_VIOLET, TERM_L_BLUE, TERM_L_BLUE, TERM_L_GREEN,
	TERM_RED, TERM_WHITE, TERM_RED, TERM_SLATE, TERM_WHITE,
	TERM_GREEN, TERM_L_GREEN, TERM_RED, TERM_L_DARK, TERM_L_GREEN,
	TERM_UMBER, TERM_BLUE, TERM_GREEN, TERM_WHITE, TERM_L_WHITE,
	TERM_L_RED, TERM_L_WHITE, TERM_WHITE, TERM_L_WHITE, TERM_L_WHITE,
	TERM_L_RED, TERM_RED, TERM_BLUE, TERM_YELLOW, TERM_YELLOW,
	TERM_L_BLUE, TERM_L_UMBER, TERM_WHITE, TERM_L_UMBER, TERM_YELLOW,
	TERM_L_DARK, TERM_L_WHITE, TERM_GREEN, TERM_L_BLUE, TERM_L_DARK,
	TERM_YELLOW, TERM_VIOLET,
	TERM_UMBER, TERM_L_WHITE, TERM_WHITE, TERM_UMBER,
	TERM_BLUE, TERM_GREEN, TERM_YELLOW, TERM_ORANGE,
	TERM_YELLOW, TERM_ORANGE, TERM_L_GREEN, TERM_YELLOW,
	TERM_RED, TERM_WHITE
};


/*
 * Amulets (adjectives and colors)
 */
static cptr amulet_adj[MAX_AMULETS] = {
	"Amber", "Driftwood", "Coral", "Agate", "Ivory",
	"Obsidian", "Bone", "Brass", "Bronze", "Pewter",
	"Tortoise Shell", "Golden", "Azure", "Crystal", "Silver",
	"Copper", "Swastika"
};

static byte amulet_col[MAX_AMULETS] = {
	TERM_YELLOW, TERM_L_UMBER, TERM_WHITE, TERM_L_WHITE, TERM_WHITE,
	TERM_L_DARK, TERM_WHITE, TERM_L_UMBER, TERM_L_UMBER, TERM_SLATE,
	TERM_GREEN, TERM_YELLOW, TERM_L_BLUE, TERM_L_BLUE, TERM_L_WHITE,
	TERM_L_UMBER, TERM_VIOLET /* Hack */
};


/*
 * Staffs (adjectives and colors)
 */
static cptr staff_adj[MAX_WOODS] = {
	"Aspen", "Balsa", "Banyan", "Birch", "Cedar",
	"Cottonwood", "Cypress", "Dogwood", "Elm", "Willow",
	"Hemlock", "Hickory", "Ironwood", "Locust", "Mahogany",
	"Maple", "Mulberry", "Oak", "Pine", "Redwood",
	"Rosewood", "Spruce", "Sycamore", "Teak", "Walnut",
	"Mistletoe", "Hawthorn", "Bamboo", "Silver", "Runed",
	"Golden", "Ashen" /*,"Gnarled","Ivory","Willow" */
};

static byte staff_col[MAX_WOODS] = {
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_UMBER, TERM_L_UMBER, TERM_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_RED,
	TERM_RED, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_UMBER,
	TERM_GREEN, TERM_L_UMBER, TERM_L_UMBER, TERM_L_WHITE, TERM_UMBER,
	TERM_YELLOW, TERM_SLATE, /*???,???,??? */
};


/*
 * Wands (adjectives and colors)
 */
static cptr wand_adj[MAX_METALS] = {
	"Aluminum", "Cast Iron", "Chromium", "Copper", "Gold",
	"Iron", "Magnesium", "Molybdenum", "Nickel", "Rusty",
	"Silver", "Steel", "Tin", "Titanium", "Tungsten",
	"Zirconium", "Zinc", "Aluminum-Plated", "Copper-Plated", "Gold-Plated",
	"Nickel-Plated", "Silver-Plated", "Steel-Plated", "Tin-Plated",
	"Zinc-Plated",
	"Mithril-Plated", "Mithril", "Runed", "Bronze", "Brass",
	"Platinum", "Lead", "Lead-Plated", "Ivory", "Adamantite",
	"Uridium", "Long", "Short", "Hexagonal"
};

static byte wand_col[MAX_METALS] = {
	TERM_L_BLUE, TERM_L_DARK, TERM_WHITE, TERM_L_UMBER, TERM_YELLOW,
	TERM_SLATE, TERM_L_WHITE, TERM_L_WHITE, TERM_L_UMBER, TERM_RED,
	TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE, TERM_WHITE, TERM_WHITE,
	TERM_L_WHITE, TERM_L_WHITE, TERM_L_BLUE, TERM_L_UMBER, TERM_YELLOW,
	TERM_L_UMBER, TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE,
	TERM_L_BLUE, TERM_L_BLUE, TERM_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_WHITE, TERM_SLATE, TERM_SLATE, TERM_WHITE, TERM_VIOLET,
	TERM_L_RED, TERM_L_BLUE, TERM_BLUE, TERM_RED
};


/*
 * Rods (adjectives and colors).
 * Efficiency -- copied from wand arrays
 */

static cptr rod_adj[MAX_METALS];

static byte rod_col[MAX_METALS];


/*
 * Mushrooms (adjectives and colors)
 */

static cptr food_adj[MAX_SHROOM] = {
	"Blue", "Black", "Black Spotted", "Brown", "Dark Blue",
	"Dark Green", "Dark Red", "Yellow", "Furry", "Green",
	"Grey", "Light Blue", "Light Green", "Violet", "Red",
	"Slimy", "Tan", "White", "White Spotted", "Wrinkled",
};

static byte food_col[MAX_SHROOM] = {
	TERM_BLUE, TERM_L_DARK, TERM_L_DARK, TERM_UMBER, TERM_BLUE,
	TERM_GREEN, TERM_RED, TERM_YELLOW, TERM_L_WHITE, TERM_GREEN,
	TERM_SLATE, TERM_L_BLUE, TERM_L_GREEN, TERM_VIOLET, TERM_RED,
	TERM_SLATE, TERM_L_UMBER, TERM_WHITE, TERM_WHITE, TERM_UMBER
};


/*
 * Color adjectives and colors, for potions.
 * Hack -- The first four entries are hard-coded.
 * (water, apple juice, slime mold juice, something)
 */

static cptr potion_adj[MAX_COLORS] = {
	"Clear", "Light Brown", "Icky Green", "xxx",
	"Azure", "Blue", "Blue Speckled", "Black", "Brown", "Brown Speckled",
	"Bubbling", "Chartreuse", "Cloudy", "Copper Speckled", "Crimson",
	"Cyan",
	"Dark Blue", "Dark Green", "Dark Red", "Gold Speckled", "Green",
	"Green Speckled", "Grey", "Grey Speckled", "Hazy", "Indigo",
	"Light Blue", "Light Green", "Magenta", "Metallic Blue",
	"Metallic Red",
	"Metallic Green", "Metallic Purple", "Misty", "Orange",
	"Orange Speckled",
	"Pink", "Pink Speckled", "Puce", "Purple", "Purple Speckled",
	"Red", "Red Speckled", "Silver Speckled", "Smoky", "Tangerine",
	"Violet", "Vermilion", "White", "Yellow", "Violet Speckled",
	"Pungent", "Clotted Red", "Viscous Pink", "Oily Yellow",
	"Gloopy Green",
	"Shimmering", "Coagulated Crimson", "Yellow Speckled", "Gold",
	"Manly", "Stinking", "Oily Black", "Ichor", "Ivory White", "Sky Blue",
};

static byte potion_col[MAX_COLORS] = {
	TERM_WHITE, TERM_L_UMBER, TERM_GREEN, 0,
	TERM_L_BLUE, TERM_BLUE, TERM_BLUE, TERM_L_DARK, TERM_UMBER, TERM_UMBER,
	TERM_L_WHITE, TERM_L_GREEN, TERM_WHITE, TERM_L_UMBER, TERM_RED,
	TERM_L_BLUE,
	TERM_BLUE, TERM_GREEN, TERM_RED, TERM_YELLOW, TERM_GREEN,
	TERM_GREEN, TERM_SLATE, TERM_SLATE, TERM_L_WHITE, TERM_VIOLET,
	TERM_L_BLUE, TERM_L_GREEN, TERM_RED, TERM_BLUE, TERM_RED,
	TERM_GREEN, TERM_VIOLET, TERM_L_WHITE, TERM_ORANGE, TERM_ORANGE,
	TERM_L_RED, TERM_L_RED, TERM_VIOLET, TERM_VIOLET, TERM_VIOLET,
	TERM_RED, TERM_RED, TERM_L_WHITE, TERM_L_DARK, TERM_ORANGE,
	TERM_VIOLET, TERM_RED, TERM_WHITE, TERM_YELLOW, TERM_VIOLET,
	TERM_L_RED, TERM_RED, TERM_L_RED, TERM_YELLOW, TERM_GREEN,
	TERM_MULTI, TERM_RED, TERM_YELLOW, TERM_YELLOW,
	TERM_L_UMBER, TERM_UMBER, TERM_L_DARK, TERM_RED, TERM_WHITE,
	TERM_L_BLUE
};


/*
 * Syllables for scrolls (must be 1-4 letters each)
 */

static cptr syllables[MAX_SYLLABLES] = {
	"a", "ab", "ag", "aks", "ala", "an", "ankh", "app",
	"arg", "arze", "ash", "aus", "ban", "bar", "bat", "bek",
	"bie", "bin", "bit", "bjor", "blu", "bot", "bu",
	"byt", "comp", "con", "cos", "cre", "dalf", "dan",
	"den", "der", "doe", "dok", "eep", "el", "eng", "er", "ere", "erk",
	"esh", "evs", "fa", "fid", "flit", "for", "fri", "fu", "gan",
	"gar", "glen", "gop", "gre", "ha", "he", "hyd", "i",
	"ing", "ion", "ip", "ish", "it", "ite", "iv", "jo",
	"kho", "kli", "klis", "la", "lech", "man", "mar",
	"me", "mi", "mic", "mik", "mon", "mung", "mur", "nag", "nej",
	"nelg", "nep", "ner", "nes", "nis", "nih", "nin", "o",
	"od", "ood", "org", "orn", "ox", "oxy", "pay", "pet",
	"ple", "plu", "po", "pot", "prok", "re", "rea", "rhov",
	"ri", "ro", "rog", "rok", "rol", "sa", "san", "sat",
	"see", "sef", "seh", "shu", "ski", "sna", "sne", "snik",
	"sno", "so", "sol", "sri", "sta", "sun", "ta", "tab",
	"tem", "ther", "ti", "tox", "trol", "tue", "turs", "u",
	"ulk", "um", "un", "uni", "ur", "val", "viv", "vly",
	"vom", "wah", "wed", "werg", "wex", "whon", "wun", "x",
	"yerg", "yp", "zun", "tri", "blaa", "jah", "bul", "on",
	"foo", "ju", "xuxu"
};


/*
 * Hold the titles of scrolls, 6 to 14 characters each
 * Also keep an array of scroll colors (always WHITE for now)
 */
static char scroll_adj[MAX_TITLES][16];

static byte scroll_col[MAX_TITLES];


/*
 * Certain items, if aware, are known instantly
 * This function is used only by "flavor_init()"
 *
 * XXX XXX XXX Add "EASY_KNOW" flag to "k_info.txt" file
 */
static bool object_easy_know(int i)
{
	object_kind *k_ptr = &k_info[i];

	/* Analyze the "tval" */
	switch (k_ptr->tval)
	{
			/* Spellbooks */
		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		{
			return (TRUE);
		}

			/* Simple items */
		case TV_FLASK:
		case TV_JUNK:
		case TV_BOTTLE:
		case TV_SKELETON:
		case TV_SPIKE:
		{
			return (TRUE);
		}

			/* All Food, Potions, Scrolls, Rods */
		case TV_FOOD:
		case TV_POTION:
		case TV_SCROLL:
		case TV_ROD:
		{
			return (TRUE);
		}

			/* Some Rings, Amulets, Lites */
		case TV_RING:
		case TV_AMULET:
		case TV_LITE:
		{
			if (k_ptr->flags3 & (TR3_EASY_KNOW))
				return (TRUE);
			return (FALSE);
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * Certain items have a flavor
 * This function is used only by "flavor_init()"
 */
static bool object_flavor(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item */
	switch (k_ptr->tval)
	{
		case TV_AMULET:
		{
			return (0x80 + amulet_col[k_ptr->sval]);
		}

		case TV_RING:
		{
			return (0x90 + ring_col[k_ptr->sval]);
		}

		case TV_STAFF:
		{
			return (0xA0 + staff_col[k_ptr->sval]);
		}

		case TV_WAND:
		{
			return (0xB0 + wand_col[k_ptr->sval]);
		}

		case TV_ROD:
		{
			return (0xC0 + rod_col[k_ptr->sval]);
		}

		case TV_SCROLL:
		{
			return (0xD0 + scroll_col[k_ptr->sval]);
		}

		case TV_POTION:
		{
			return (0xE0 + potion_col[k_ptr->sval]);
		}

		case TV_FOOD:
		{
			if (k_ptr->sval < SV_FOOD_MIN_FOOD)
			{
				return (0xF0 + food_col[k_ptr->sval]);
			}

			break;
		}
	}

	/* No flavor */
	return (0);
}


void get_table_name(char *out_string)
{
	int testcounter = randint(3) + 1;

	strcpy(out_string, "'");

	if (randint(3) == 2)
	{
		while (testcounter--)
			strcat(out_string, syllables[rand_int(MAX_SYLLABLES)]);
	}
	else
	{
		char Syllable[80];
		testcounter = randint(2) + 1;
		while (testcounter--)
		{
			(void) get_rnd_line("elvish.txt", 0, Syllable);
			strcat(out_string, Syllable);
		}
	}

	out_string[1] = toupper(out_string[1]);

	strcat(out_string, "'");

	out_string[18] = '\0';

	return;
}

#if 1 /* TNB */

#define STRINGIFY2(x) (sizeof(x) ? #x : "")

cptr keyword_term_color[16] = {
	STRINGIFY2(TERM_DARK),
	STRINGIFY2(TERM_WHITE),
	STRINGIFY2(TERM_SLATE),
	STRINGIFY2(TERM_ORANGE),
	STRINGIFY2(TERM_RED),
	STRINGIFY2(TERM_GREEN),
	STRINGIFY2(TERM_BLUE),
	STRINGIFY2(TERM_UMBER),
	STRINGIFY2(TERM_L_DARK),
	STRINGIFY2(TERM_L_WHITE),
	STRINGIFY2(TERM_VIOLET),
	STRINGIFY2(TERM_YELLOW),
	STRINGIFY2(TERM_L_RED),
	STRINGIFY2(TERM_L_GREEN),
	STRINGIFY2(TERM_L_BLUE),
	STRINGIFY2(TERM_L_UMBER)
};

#if 0 /* Use "angband info potion_col" command instead */

/*
 * The file lib/data/flavor_col contains a list of TERM_XXX color
 * names for each flavor of object. It is created if it does not
 * exist.
 */

static errr init_flavor_txt_aux(FILE * fff, cptr title, byte * col,
	int max)
{
	int i;

	/* Dump the title */
	fprintf(fff, "\"%s\"\n", title);

	/* Dump each color */
	for (i = 0; i < max; i++)
	{
		fprintf(fff, "%s\n", term_col_name[col[i]]);
	}

	/* Terminate */
	fprintf(fff, "\n");

	/* Success */
	return (0);
}

static errr init_flavor_txt(void)
{
	char buf[1024];

	FILE *fff;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "flavor_col");

	/* Try to open the file for reading */
	fff = my_fopen(buf, "r");

	/* If the file exists, do nothing */
	if (fff)
	{
		/* Close */
		my_fclose(fff);

		/* Success */
		return (0);
	}

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Write to the file */
	fff = my_fopen(buf, "w");

	/* Failure */
	if (!fff)
		return (-1);

	/* Start dumping */
	fprintf(fff, "# Automatically generated. Do not edit.\n\n");

	(void) init_flavor_txt_aux(fff, "amulet", amulet_col, MAX_AMULETS);
	(void) init_flavor_txt_aux(fff, "mushroom", food_col, MAX_SHROOM);
	(void) init_flavor_txt_aux(fff, "potion", potion_col, MAX_COLORS);
	(void) init_flavor_txt_aux(fff, "ring", ring_col, MAX_ROCKS);
	(void) init_flavor_txt_aux(fff, "rod", wand_col, MAX_METALS);
	(void) init_flavor_txt_aux(fff, "staff", staff_col, MAX_WOODS);
	(void) init_flavor_txt_aux(fff, "wand", wand_col, MAX_METALS);

	/* Close */
	my_fclose(fff);

	/* Success */
	return (0);
}

#endif /* 0 */

#if 1
static void init_flavor(void)
{
	int n = 0;
	int max[7];
	byte *attr[7];

	max[n] = MAX_AMULETS;
	attr[n++] = amulet_col;

	max[n] = MAX_SHROOM;
	attr[n++] = food_col;

	max[n] = MAX_COLORS;
	attr[n++] = potion_col;

	max[n] = MAX_ROCKS;
	attr[n++] = ring_col;

	max[n] = MAX_METALS;
	attr[n++] = rod_col;

	max[n] = MAX_WOODS;
	attr[n++] = staff_col;

	max[n] = MAX_METALS;
	attr[n++] = wand_col;

	angtk_flavor_init(max, attr);
}
#else
static int init_flavor_aux(int n, char *desc, int tval, int count,
	byte * color)
{
	Tcl_HashEntry *hPtr;
	t_flavor flavor;
	int i, dummy;
	IconSpec defaultIcon = {TYPE_DEFAULT, 0, -1};

	flavor.desc = (char *) string_make(desc);
	flavor.tval = tval;
	flavor.count = count;
	C_MAKE(flavor.sorted, count, int);
	C_MAKE(flavor.icon, count, IconSpec);
	C_MAKE(flavor.color, count, byte);

	for (i = 0; i < count; i++)
	{
		flavor.sorted[i] = i;
		flavor.color[i] = color[i];
		flavor.icon[i] = defaultIcon;
	}

	hPtr = Tcl_CreateHashEntry(&g_flavor_table, desc, &dummy);
	Tcl_SetHashValue(hPtr, (ClientData) n);
	g_flavor[n++] = flavor;

	return n;
}

static void init_flavor(void)
{
	int n = 0;

#if 0
	if (init_flavor_txt())
		quit("error creating lib/data/flavor_col");
#endif

	C_MAKE(g_flavor, FLAVOR_MAX, t_flavor);
	g_flavor_count = FLAVOR_MAX;

	/*
	 * This table maps a flavor name to an index in the g_flavor[] array.
	 */
	Tcl_InitHashTable(&g_flavor_table, TCL_STRING_KEYS);

	/* Note: indexes are hard-coded by flavor_init() */
	n = init_flavor_aux(n, "amulet", TV_AMULET, MAX_AMULETS, amulet_col);
	n = init_flavor_aux(n, "mushroom", TV_FOOD, MAX_SHROOM, food_col);
	n = init_flavor_aux(n, "potion", TV_POTION, MAX_COLORS, potion_col);
	n = init_flavor_aux(n, "ring", TV_RING, MAX_ROCKS, ring_col);
	n = init_flavor_aux(n, "rod", TV_ROD, MAX_METALS, wand_col); /* !!! */
	n = init_flavor_aux(n, "staff", TV_STAFF, MAX_WOODS, staff_col);
	n = init_flavor_aux(n, "wand", TV_WAND, MAX_METALS, wand_col);
}
#endif /* 0 */

#endif /* TNB */


/*
 * Prepare the "variable" part of the "k_info" array.
 *
 * The "color"/"metal"/"type" of an item is its "flavor".
 * For the most part, flavors are assigned randomly each game.
 *
 * Initialize descriptions for the "colored" objects, including:
 * Rings, Amulets, Staffs, Wands, Rods, Food, Potions, Scrolls.
 *
 * The first 4 entries for potions are fixed (Water, Apple Juice,
 * Slime Mold Juice, Unused Potion).
 *
 * Scroll titles are always between 6 and 14 letters long.  This is
 * ensured because every title is composed of whole words, where every
 * word is from 1 to 8 letters long (one or two syllables of 1 to 4
 * letters each), and that no scroll is finished until it attempts to
 * grow beyond 15 letters.  The first time this can happen is when the
 * current title has 6 letters and the new word has 8 letters, which
 * would result in a 6 letter scroll title.
 *
 * Duplicate titles are avoided by requiring that no two scrolls share
 * the same first four letters (not the most efficient method, and not
 * the least efficient method, but it will always work).
 *
 * Hack -- make sure everything stays the same for each saved game
 * This is accomplished by the use of a saved "random seed", as in
 * "town_gen()".  Since no other functions are called while the special
 * seed is in effect, so this function is pretty "safe".
 *
 * Note that the "hacked seed" may provide an RNG with alternating parity!
 */
void flavor_init(void)
{
	int i, j;

	byte temp_col;

	cptr temp_adj;


	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant flavors */
	Rand_value = seed_flavor;


	/* Efficiency -- Rods/Wands share initial array */
	for (i = 0; i < MAX_METALS; i++)
	{
		rod_adj[i] = wand_adj[i];
		rod_col[i] = wand_col[i];
	}

	init_flavor(); /* TNB */

	/* Rings have "ring colors" */
	for (i = 0; i < MAX_ROCKS; i++)
	{
		j = rand_int(MAX_ROCKS);
		temp_adj = ring_adj[i];
		ring_adj[i] = ring_adj[j];
		ring_adj[j] = temp_adj;
		temp_col = ring_col[i];
		ring_col[i] = ring_col[j];
		ring_col[j] = temp_col;
#if 1 /* TNB */
		angtk_flavor_swap(FLAVOR_RING, i, j);
#endif
	}

	/* Amulets have "amulet colors" */
	for (i = 0; i < MAX_AMULETS; i++)
	{
		j = rand_int(MAX_AMULETS);
		temp_adj = amulet_adj[i];
		amulet_adj[i] = amulet_adj[j];
		amulet_adj[j] = temp_adj;
		temp_col = amulet_col[i];
		amulet_col[i] = amulet_col[j];
		amulet_col[j] = temp_col;
#if 1 /* TNB */
		angtk_flavor_swap(FLAVOR_AMULET, i, j);
#endif
	}

	/* Staffs */
	for (i = 0; i < MAX_WOODS; i++)
	{
		j = rand_int(MAX_WOODS);
		temp_adj = staff_adj[i];
		staff_adj[i] = staff_adj[j];
		staff_adj[j] = temp_adj;
		temp_col = staff_col[i];
		staff_col[i] = staff_col[j];
		staff_col[j] = temp_col;
#if 1 /* TNB */
		angtk_flavor_swap(FLAVOR_STAFF, i, j);
#endif
	}

	/* Wands */
	for (i = 0; i < MAX_METALS; i++)
	{
		j = rand_int(MAX_METALS);
		temp_adj = wand_adj[i];
		wand_adj[i] = wand_adj[j];
		wand_adj[j] = temp_adj;
		temp_col = wand_col[i];
		wand_col[i] = wand_col[j];
		wand_col[j] = temp_col;
#if 1 /* TNB */
		angtk_flavor_swap(FLAVOR_WAND, i, j);
#endif
	}

	/* Rods */
	for (i = 0; i < MAX_METALS; i++)
	{
		j = rand_int(MAX_METALS);
		temp_adj = rod_adj[i];
		rod_adj[i] = rod_adj[j];
		rod_adj[j] = temp_adj;
		temp_col = rod_col[i];
		rod_col[i] = rod_col[j];
		rod_col[j] = temp_col;
#if 1 /* TNB */
		angtk_flavor_swap(FLAVOR_ROD, i, j);
#endif
	}

	/* Foods (Mushrooms) */
	for (i = 0; i < MAX_SHROOM; i++)
	{
		j = rand_int(MAX_SHROOM);
		temp_adj = food_adj[i];
		food_adj[i] = food_adj[j];
		food_adj[j] = temp_adj;
		temp_col = food_col[i];
		food_col[i] = food_col[j];
		food_col[j] = temp_col;
#if 1 /* TNB */
		angtk_flavor_swap(FLAVOR_MUSHROOM, i, j);
#endif
	}

	/* Potions */
	for (i = 4; i < MAX_COLORS; i++)
	{
		j = rand_int(MAX_COLORS - 4) + 4;
		temp_adj = potion_adj[i];
		potion_adj[i] = potion_adj[j];
		potion_adj[j] = temp_adj;
		temp_col = potion_col[i];
		potion_col[i] = potion_col[j];
		potion_col[j] = temp_col;
#if 1 /* TNB */
		angtk_flavor_swap(FLAVOR_POTION, i, j);
#endif
	}

	/* Scrolls (random titles, always white) */
	for (i = 0; i < MAX_TITLES; i++)
	{
		/* Get a new title */
		while (TRUE)
		{
			char buf[80];

			bool okay;

			/* Start a new title */
			buf[0] = '\0';

			/* Collect words until done */
			while (1)
			{
				int q, s;

				char tmp[80];

				/* Start a new word */
				tmp[0] = '\0';

				/* Choose one or two syllables */
				s = ((rand_int(100) < 30) ? 1 : 2);

				/* Add a one or two syllable word */
				for (q = 0; q < s; q++)
				{
					/* Add the syllable */
					strcat(tmp, syllables[rand_int(MAX_SYLLABLES)]);
				}

				/* Stop before getting too long */
				if (strlen(buf) + 1 + strlen(tmp) > 15)
					break;

				/* Add a space */
				strcat(buf, " ");

				/* Add the word */
				strcat(buf, tmp);
			}

			/* Save the title */
			strcpy(scroll_adj[i], buf + 1);

			/* Assume okay */
			okay = TRUE;

			/* Check for "duplicate" scroll titles */
			for (j = 0; j < i; j++)
			{
				cptr hack1 = scroll_adj[j];
				cptr hack2 = scroll_adj[i];

				/* Compare first four characters */
				if (*hack1++ != *hack2++)
					continue;
				if (*hack1++ != *hack2++)
					continue;
				if (*hack1++ != *hack2++)
					continue;
				if (*hack1++ != *hack2++)
					continue;

				/* Not okay */
				okay = FALSE;

				/* Stop looking */
				break;
			}

			/* Break when done */
			if (okay)
				break;
		}

		/* All scrolls are white */
		scroll_col[i] = TERM_WHITE;
	}


	/* Hack -- Use the "complex" RNG */
	Rand_quick = FALSE;

	/* Analyze every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip "empty" objects */
		if (!k_ptr->name)
			continue;

		/* Extract "flavor" (if any) */
		k_ptr->flavor = object_flavor(i);

		/* No flavor yields aware */
		if (!k_ptr->flavor)
			k_ptr->aware = TRUE;

		/* Check for "easily known" */
		k_ptr->easy_know = object_easy_know(i);
	}
}


/*
 * Print a char "c" into a string "t", as if by sprintf(t, "%c", c),
 * and return a pointer to the terminator (t + 1).
 */
static char *object_desc_chr(char *t, char c)
{
	/* Copy the char */
	*t++ = c;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}


/*
 * Print a string "s" into a string "t", as if by strcpy(t, s),
 * and return a pointer to the terminator.
 */
static char *object_desc_str(char *t, cptr s)
{
	/* Copy the string */
	while (*s)
		*t++ = *s++;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}



/*
 * Print an unsigned number "n" into a string "t", as if by
 * sprintf(t, "%u", n), and return a pointer to the terminator.
 */
static char *object_desc_num(char *t, uint n)
{
	uint p;

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */ ;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}




/*
 * Print an signed number "v" into a string "t", as if by
 * sprintf(t, "%+d", n), and return a pointer to the terminator.
 * Note that we always print a sign, either "+" or "-".
 */
static char *object_desc_int(char *t, sint v)
{
	uint p, n;

	/* Negative */
	if (v < 0)
	{
		/* Take the absolute value */
		n = 0 - v;

		/* Use a "minus" sign */
		*t++ = '-';
	}

	/* Positive (or zero) */
	else
	{
		/* Use the actual number */
		n = v;

		/* Use a "plus" sign */
		*t++ = '+';
	}

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */ ;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}

#if 1 /* TNB */

static char *object_desc_inscrip(object_type *o_ptr, char *buf)
{
	/* See if the object is "aware" */
	bool aware = (object_aware_p(o_ptr) ? TRUE : FALSE);

	/* See if the object is "known" */
	bool known = (object_known_p(o_ptr) ? TRUE : FALSE);

	char *t = buf, *v = NULL;

	/* Append the user's inscription if available */
	if (o_ptr->inscription)
	{
		char *u = (char *) quark_str(o_ptr->inscription);
		while (*u && (*u != '#')) *t++ = *u++;
	}

	/* Append the game-generated "feeling" if available */
	if (o_ptr->feeling)
	{
		v = (char *) game_inscriptions[o_ptr->feeling];
	}

	/* Note "cursed" if the item is known to be cursed */
	else if (cursed_p(o_ptr) && (known || (o_ptr->ident & (IDENT_SENSE))))
	{
		v = "cursed";
	}

	/* Mega-Hack -- note empty wands/staffs */
	else if (!known && (o_ptr->ident & (IDENT_EMPTY)))
	{
		v = "empty";
	}

	/* Note "tried" if the object has been tested unsuccessfully */
	else if (!aware && object_tried_p(o_ptr))
	{
		v = "tried";
	}

	/* Note the discount, if any */
	else if (o_ptr->discount)
	{
		char discount_buf[80], *q = discount_buf;
		q = object_desc_num(q, o_ptr->discount);
		q = object_desc_str(q, "% off");
		v = discount_buf;
	}

	if (v)
	{
		if (t > buf)
		{
			*t++ = ',';
			*t++ = ' ';
		}
		while (*v) *t++ = *v++;
	}

	/* Terminate */
	*t = '\0';

	return buf;
}

#endif /* TNB */

/*
 * Creates a description of the item "o_ptr", and stores it in "out_val".
 *
 * One can choose the "verbosity" of the description, including whether
 * or not the "number" of items should be described, and how much detail
 * should be used when describing the item.
 *
 * The given "buf" must be 80 chars long to hold the longest possible
 * description, which can get pretty long, including incriptions, such as:
 * "no more Maces of Disruption (Defender) (+10,+10) [+5] (+3 to stealth)".
 * Note that the inscription will be clipped to keep the total description
 * under 79 chars (plus a terminator).
 *
 * Note the use of "object_desc_num()" and "object_desc_int()" as hyper-efficient,
 * portable, versions of some common "sprintf()" commands.
 *
 * Note that all ego-items (when known) append an "Ego-Item Name", unless
 * the item is also an artifact, which should NEVER happen.
 *
 * Note that all artifacts (when known) append an "Artifact Name", so we
 * have special processing for "Specials" (artifact Lites, Rings, Amulets).
 * The "Specials" never use "modifiers" if they are "known", since they
 * have special "descriptions", such as "The Necklace of the Dwarves".
 *
 * Special Lite's use the "k_info" base-name (Phial, Star, or Arkenstone),
 * plus the artifact name, just like any other artifact, if known.
 *
 * Special Ring's and Amulet's, if not "aware", use the same code as normal
 * rings and amulets, and if "aware", use the "k_info" base-name (Ring or
 * Amulet or Necklace).  They will NEVER "append" the "k_info" name.  But,
 * they will append the artifact name, just like any artifact, if known.
 *
 * None of the Special Rings/Amulets are "EASY_KNOW", though they could be,
 * at least, those which have no "pluses", such as the three artifact lites.
 *
 * Hack -- Display "The One Ring" as "a Plain Gold Ring" until aware.
 *
 * If "pref" then a "numeric" prefix will be pre-pended.
 *
 * Mode:
 *   0 -- The Cloak of Death
 *   1 -- The Cloak of Death [1,+3]
 *   2 -- The Cloak of Death [1,+3] (+2 to Stealth)
 *   3 -- The Cloak of Death [1,+3] (+2 to Stealth) {nifty}
 */
void object_desc(char *buf, object_type * o_ptr, int pref, int mode)
{
	cptr basenm, modstr;
	int power, indexx;

	bool aware = FALSE;
	bool known = FALSE;

	bool append_name = FALSE;

	bool show_weapon = FALSE;
	bool show_armour = FALSE;

	cptr s, u;
	char *t;

	char p1 = '(', p2 = ')';
	char b1 = '[', b2 = ']';
	char c1 = '{', c2 = '}';

	char tmp_val[160];
	char tmp_val2[90];

	u32b f1, f2, f3;

	object_type *bow_ptr;

	/* describe what type of ammo item is. (0=none) */
	byte ammotype = 0;

	/* damage dice, damage sides, damage bonus, energy */
	int dd, ds, db, energy_use;
	int tmul;
	long avgdam;


	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	monster_race *r_ptr = &r_info[o_ptr->pval];

	/* Extract some flags */
	object_flags(o_ptr, &f1, &f2, &f3);


	/* See if the object is "aware" */
	if (object_aware_p(o_ptr))
		aware = TRUE;

	/* See if the object is "known" */
	if (object_known_p(o_ptr))
		known = TRUE;

	/* Hack -- Extract the sub-type "indexx" */
	indexx = o_ptr->sval;

	/* Extract default "base" string */
	basenm = get_object_name(o_ptr);

	/* Assume no "modifier" string */
	modstr = "";


	/* Analyze the object */
	switch (o_ptr->tval)
	{
			/* Some objects are easy to describe */
		case TV_SKELETON:
		case TV_BOTTLE:
		case TV_JUNK:
		case TV_SPIKE:
		case TV_FLASK:
		case TV_CHEST:
		{
			break;
		}

			/* Figurines/Statues */
		case TV_FIGURINE:
		case TV_STATUE:
		{
			cptr t = r_name + r_ptr->name;

			if (!(r_ptr->flags1 & RF1_UNIQUE))
			{
				sprintf(tmp_val2, "%s%s", (is_a_vowel(*t) ? "an " : "a "),
					t);

				modstr = tmp_val2;
			}
			else
			{
				modstr = t;
			}

			break;
		}

			/* Corpses */
		case TV_CORPSE:
		{
			modstr = r_name + r_ptr->name;

			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				sprintf(tmp_val2, "%s %s", basenm, "of #");
			}
			else
			{
				sprintf(tmp_val2, "& # %s", basenm + 2);
			}

			basenm = tmp_val2;
			break;
		}

			/* Missiles/ Bows/ Weapons */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_BOW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			show_weapon = TRUE;
			break;
		}


			/* Armour */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		{
			show_armour = TRUE;
			break;
		}


			/* Lites (including a few "Specials") */
		case TV_LITE:
		{
			break;
		}

			/* Amulets (including a few "Specials") */
		case TV_AMULET:
		{
			/* Known artifacts */
			if (artifact_p(o_ptr) && aware)
				break;

			/* Color the object */
			modstr = amulet_adj[indexx];
			if (aware)
				append_name = TRUE;

			if (((plain_descriptions) && (aware)) ||
				o_ptr->ident & IDENT_STOREB)
				basenm = "& Amulet~";
			else
				basenm = aware ? "& # Amulet~" : "& # Amulet~";
			break;
		}

			/* Rings (including a few "Specials") */
		case TV_RING:
		{
			/* Known artifacts */
			if (artifact_p(o_ptr) && aware)
				break;

			/* Color the object */
			modstr = ring_adj[indexx];
			if (aware)
				append_name = TRUE;

			if (((plain_descriptions) && (aware)) ||
				o_ptr->ident & IDENT_STOREB)
				basenm = "& Ring~";
			else
				basenm = aware ? "& # Ring~" : "& # Ring~";

			/* Hack -- The One Ring */
			if (!aware && (o_ptr->sval == SV_RING_POWER))
				modstr = "Plain Gold";

			break;
		}

		case TV_STAFF:
		{
			/* Color the object */
			modstr = staff_adj[indexx];
			if (aware)
				append_name = TRUE;
			if (((plain_descriptions) && (aware)) ||
				o_ptr->ident & IDENT_STOREB)
				basenm = "& Staff~";
			else
				basenm = aware ? "& # Staff~" : "& # Staff~";
			break;
		}

		case TV_WAND:
		{
			/* Color the object */
			modstr = wand_adj[indexx];
			if (aware)
				append_name = TRUE;
			if (((plain_descriptions) && (aware)) ||
				o_ptr->ident & IDENT_STOREB)
				basenm = "& Wand~";
			else
				basenm = aware ? "& # Wand~" : "& # Wand~";
			break;
		}

		case TV_ROD:
		{
			/* Color the object */
			modstr = rod_adj[indexx];
			if (aware)
				append_name = TRUE;
			if (((plain_descriptions) && (aware)) ||
				o_ptr->ident & IDENT_STOREB)
				basenm = "& Rod~";
			else
				basenm = aware ? "& # Rod~" : "& # Rod~";
			break;
		}

		case TV_SCROLL:
		{
			/* Color the object */
			modstr = scroll_adj[indexx];
			if (aware)
				append_name = TRUE;
			if (((plain_descriptions) && (aware)) ||
				o_ptr->ident & IDENT_STOREB)
				basenm = "& Scroll~";
			else
				basenm =
					aware ? "& Scroll~ titled \"#\"" :
					"& Scroll~ titled \"#\"";
			break;
		}

		case TV_POTION:
		{
			/* Color the object */
			modstr = potion_adj[indexx];
			if (aware)
				append_name = TRUE;
			if (((plain_descriptions) && (aware)) ||
				o_ptr->ident & IDENT_STOREB)
				basenm = "& Potion~";
			else
				basenm = aware ? "& # Potion~" : "& # Potion~";
			break;
		}

		case TV_FOOD:
		{
			/* Ordinary food is "boring" */
			if (o_ptr->sval >= SV_FOOD_MIN_FOOD)
				break;

			/* Color the object */
			modstr = food_adj[indexx];
			if (aware)
				append_name = TRUE;
			if (((plain_descriptions) && (aware)) ||
				o_ptr->ident & IDENT_STOREB)
				basenm = "& Mushroom~";
			else
				basenm = aware ? "& # Mushroom~" : "& # Mushroom~";
			break;
		}

			/* Magic Books */
		case TV_LIFE_BOOK:
		{
			modstr = basenm;
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Life Magic #";
			else
				basenm = "& Life Spellbook~ #";
			break;
		}

		case TV_SORCERY_BOOK:
		{
			modstr = basenm;
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Sorcery #";
			else
				basenm = "& Sorcery Spellbook~ #";
			break;
		}

		case TV_NATURE_BOOK:
		{
			modstr = basenm;
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Nature Magic #";
			else
				basenm = "& Nature Spellbook~ #";
			break;
		}

		case TV_CHAOS_BOOK:
		{
			modstr = basenm;
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Chaos Magic #";
			else
				basenm = "& Chaos Spellbook~ #";
			break;
		}

		case TV_DEATH_BOOK:
		{
			modstr = basenm;
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Death Magic #";
			else
				basenm = "& Death Spellbook~ #";
			break;
		}

		case TV_TRUMP_BOOK:
		{
			modstr = basenm;
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Trump Magic #";
			else
				basenm = "& Trump Spellbook~ #";
			break;
		}

		case TV_ARCANE_BOOK:
		{
			modstr = basenm;
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Arcane Magic #";
			else
				basenm = "& Arcane Spellbook~ #";
			break;
		}


			/* Hack -- Gold/Gems */
		case TV_GOLD:
		{
			strcpy(buf, basenm);
			return;
		}

			/* Used in the "inventory" routine */
		default:
		{
			strcpy(buf, "(nothing)");
			return;
		}
	}


	/* Start dumping the result */
	t = tmp_val;

	/* The object "expects" a "number" */
	if (basenm[0] == '&')
	{
		/* Skip the ampersand (and space) */
		s = basenm + 2;

		/* No prefix */
		if (!pref)
		{
			/* Nothing */
		}

		/* Hack -- None left */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Extract the number */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && (artifact_p(o_ptr) || o_ptr->art_name))
		{
			t = object_desc_str(t, "The ");
		}

		/* Unique corpses are unique */
		else if ((o_ptr->tval == TV_CORPSE) &&
			(r_ptr->flags1 & RF1_UNIQUE))
		{
			t = object_desc_str(t, "The ");
		}

		/* A single one, with a vowel in the modifier */
		else if ((*s == '#') && (is_a_vowel(modstr[0])))
		{
			t = object_desc_str(t, "an ");
		}

		/* A single one, with a vowel */
		else if (is_a_vowel(*s))
		{
			t = object_desc_str(t, "an ");
		}

		/* A single one, without a vowel */
		else
		{
			t = object_desc_str(t, "a ");
		}
	}

	/* Hack -- objects that "never" take an article */
	else
	{
		/* No ampersand */
		s = basenm;

		/* No pref */
		if (!pref)
		{
			/* Nothing */
		}

		/* Hack -- all gone */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Prefix a number if required */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && (artifact_p(o_ptr) || o_ptr->art_name))
		{
			t = object_desc_str(t, "The ");
		}

		/* Hack -- single items get no prefix */
		else
		{
			/* Nothing */
		}
	}

	/* Paranoia -- skip illegal tildes */
	/* while (*s == '~') s++; */

	/* Copy the string */
	for (; *s; s++)
	{
		/* Pluralizer */
		if (*s == '~')
		{
			/* Add a plural if needed */
			if (o_ptr->number != 1)
			{
				char k = t[-1];

				/* XXX XXX XXX Mega-Hack */

				/* Hack -- "Cutlass-es" and "Torch-es" */
				if ((k == 's') || (k == 'h'))
					*t++ = 'e';

				/* Add an 's' */
				*t++ = 's';
			}
		}

		/* Modifier */
		else if (*s == '#')
		{
			/* Insert the modifier */
			for (u = modstr; *u; u++)
				*t++ = *u;
		}

		/* Normal */
		else
		{
			/* Copy */
			*t++ = *s;
		}
	}

	/* Terminate */
	*t = '\0';


	/* Append the "kind name" to the "base name" */
	if (append_name)
	{
		t = object_desc_str(t, " of ");
		t = object_desc_str(t, get_object_name(o_ptr));
	}


	/* Hack -- Append "Artifact" or "Special" names */
	if (known)
	{
		if (o_ptr->inscription &&
			strchr(quark_str(o_ptr->inscription), '#'))
		{
			/* Find the '#' */
			cptr str = strchr(quark_str(o_ptr->inscription), '#');

			/* Add the false name */
			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, &str[1]);
		}

		/* Is it a new random artifact ? */
		else if (o_ptr->art_name)
		{
#if 0
			if (o_ptr->ident & IDENT_STOREB)
				t = object_desc_str(t, " called '");
			else
#endif
				t = object_desc_chr(t, ' ');

			t = object_desc_str(t, quark_str(o_ptr->art_name));

#if 0
			if (o_ptr->ident & IDENT_STOREB)
				t = object_desc_chr(t, '\'');
#endif
		}

		/* Grab any artifact name */
		else if (o_ptr->name1)
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, (a_name + a_ptr->name));
		}

		/* Grab any ego-item name */
		else if (o_ptr->name2)
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, (e_name + e_ptr->name));
		}
	}


	/* No more details wanted */
	if (mode < 1)
		goto copyback;

	/* Hack -- Chests must be described in detail */
	if (o_ptr->tval == TV_CHEST)
	{
		/* Not searched yet */
		if (!known)
		{
			/* Nothing */
		}

		/* May be "empty" */
		else if (!o_ptr->pval)
		{
			t = object_desc_str(t, " (empty)");
		}

		/* May be "disarmed" */
		else if (o_ptr->pval < 0)
		{
			if (chest_traps[0 - o_ptr->pval])
			{
				t = object_desc_str(t, " (disarmed)");
			}
			else
			{
				t = object_desc_str(t, " (unlocked)");
			}
		}

		/* Describe the traps, if any */
		else
		{
			/* Describe the traps */
			switch (chest_traps[o_ptr->pval])
			{
				case 0:
				{
					t = object_desc_str(t, " (Locked)");
					break;
				}
				case CHEST_LOSE_STR:
				{
					t = object_desc_str(t, " (Poison Needle)");
					break;
				}
				case CHEST_LOSE_CON:
				{
					t = object_desc_str(t, " (Poison Needle)");
					break;
				}
				case CHEST_POISON:
				{
					t = object_desc_str(t, " (Gas Trap)");
					break;
				}
				case CHEST_PARALYZE:
				{
					t = object_desc_str(t, " (Gas Trap)");
					break;
				}
				case CHEST_EXPLODE:
				{
					t = object_desc_str(t, " (Explosion Device)");
					break;
				}
				case CHEST_SUMMON:
				{
					t = object_desc_str(t, " (Summoning Runes)");
					break;
				}
				default:
				{
					t = object_desc_str(t, " (Multiple Traps)");
					break;
				}
			}
		}
	}


	/* Display the item like a weapon */
	if (f3 & (TR3_SHOW_MODS))
		show_weapon = TRUE;

	/* Display the item like a weapon */
	if (o_ptr->to_h && o_ptr->to_d)
		show_weapon = TRUE;

	/* Display the item like armour */
	if (o_ptr->ac)
		show_armour = TRUE;

	/* Dump base weapon info */
	switch (o_ptr->tval)
	{
			/* Missiles and Weapons */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:

			/* Append a "damage" string */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_num(t, o_ptr->dd);
			t = object_desc_chr(t, 'd');
			t = object_desc_num(t, o_ptr->ds);
			t = object_desc_chr(t, p2);

			/* Set ammotype - used later to show avg damages */
			if (o_ptr->tval == TV_SHOT)
				ammotype = 1;
			if (o_ptr->tval == TV_ARROW)
				ammotype = 2;
			if (o_ptr->tval == TV_BOLT)
				ammotype = 3;

			/* All done */
			break;


			/* Bows get a special "damage string" */
		case TV_BOW:

			/* Extract the "base power" */
			switch (o_ptr->sval)
			{
				case SV_SLING:
					power = 2;
					break;

				case SV_SHORT_BOW:
					power = 2;
					break;

				case SV_LONG_BOW:
					if (p_ptr->stat_use[A_STR] >= 16)
					{
						power = 3;
					}
					else
					{
						/* hack- weak players cannot use a longbow well */
						power = 2;
					}
					break;

				case SV_LIGHT_XBOW:
					power = 4;
					break;

				case SV_HEAVY_XBOW:
					power = 5;
					break;

				default:
					msg_print("Unknown firing multiplier.");
					power = 0;
			}

			/* Apply the "Extra Might" flag */
			if (f3 & (TR3_XTRA_MIGHT))
				power++;

			/* Append a special "damage" string */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_chr(t, 'x');
			t = object_desc_num(t, power);
			t = object_desc_chr(t, p2);

			/* All done */
			break;
	}


	/* Add the weapon bonuses */
	if (known)
	{
		/* Show the tohit/todam on request */
		if (show_weapon)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}

		/* Show the tohit if needed */
		else if (o_ptr->to_h)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, p2);
		}

		/* Show the todam if needed */
		else if (o_ptr->to_d)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}
	}

	bow_ptr = &inventory[INVEN_BOW];

	/* if have a firing weapon + ammo matches bow */
	if (bow_ptr->k_idx && (((bow_ptr->sval == SV_SLING) && (ammotype == 1))
			|| (((bow_ptr->sval == SV_SHORT_BOW) ||
					(bow_ptr->sval == SV_LONG_BOW)) && (ammotype == 2)) ||
			(((bow_ptr->sval == SV_LIGHT_XBOW) ||
					(bow_ptr->sval == SV_HEAVY_XBOW)) && (ammotype == 3))))
	{
		/* See if the bow is "known" - then set damage bonus */
		if (object_known_p(bow_ptr))
		{
			db = bow_ptr->to_d;
		}
		else
		{
			db = 0;
		}

		/* effect of player */
		db += p_ptr->dis_to_d;

		/* effect of ammo */
		if (known)
			db += o_ptr->to_d;

		dd = o_ptr->dd;
		ds = o_ptr->ds;

		if (db > 0)
			avgdam = (100 + deadliness_conversion[db]);
		else if (db > -31)
			avgdam = (100 - deadliness_conversion[ABS(db)]);
		else
			avgdam = 0;

		/* effect of damage dice x2 */
		avgdam *= dd * (ds + 1);

		/* Stop compiler warnings */
		energy_use = 100;
		tmul = 1;

		/* Analyze the launcher */
		switch (bow_ptr->sval)
		{
				/* Sling and ammo */
			case SV_SLING:
			{
				tmul = 2;
				energy_use = 50;
				break;
			}

				/* Short Bow and Arrow */
			case SV_SHORT_BOW:
			{
				tmul = 2;
				energy_use = 100;
				break;
			}

				/* Long Bow and Arrow */
			case SV_LONG_BOW:
			{
				if (p_ptr->stat_use[A_STR] >= 16)
				{
					tmul = 3;
				}
				else
				{
					/* weak players cannot use a longbow well */
					tmul = 2;
				}
				energy_use = 100;
				break;
			}

				/* Light Crossbow and Bolt */
			case SV_LIGHT_XBOW:
			{
				tmul = 4;
				energy_use = 120;
				break;
			}

				/* Heavy Crossbow and Bolt */
			case SV_HEAVY_XBOW:
			{
				tmul = 5;
				if (p_ptr->stat_use[A_DEX] >= 16)
				{
					energy_use = 150;
				}
				else
				{
					/* players with low dex will take longer to load */
					energy_use = 200;
				}
				break;
			}
		}

		/* Get extra "power" from "extra might" */
		if (p_ptr->xtra_might)
			tmul++;

		/* launcher multiplier */
		avgdam *= tmul;

		/* display (shot damage/ avg damage) */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam / 200);
		t = object_desc_chr(t, '/');

		tmul = p_ptr->num_fire;
		if (tmul == 0)
		{
			t = object_desc_chr(t, '0');
		}
		else
		{
			/* calc effects of energy */
			avgdam *= p_ptr->num_fire;

			/* rescale */
			avgdam /= 2 * energy_use;
			t = object_desc_num(t, avgdam);
		}

		t = object_desc_chr(t, p2);
	}

	/* Add the armor bonuses */
	if (known)
	{
		/* Show the armor class info */
		if (show_armour)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_num(t, o_ptr->ac);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}

		/* No base armor, but does increase armor */
		else if (o_ptr->to_a)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}
	}

	/* Hack -- always show base armor */
	else if (show_armour)
	{
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, b1);
		t = object_desc_num(t, o_ptr->ac);
		t = object_desc_chr(t, b2);
	}


	/* No more details wanted */
	if (mode < 2)
		goto copyback;


	/*
	 * Hack -- Wands and Staffs have charges.  Make certain how many charges
	 * a stack of staffs really has is clear. -LM-
	 */
	if (known && ((o_ptr->tval == TV_STAFF) || (o_ptr->tval == TV_WAND)))
	{
		/* Dump " (N charges)" */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);

		/* Clear explaination for staffs. */
		if ((o_ptr->tval == TV_STAFF) && (o_ptr->number > 1))
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_str(t, "x ");
		}
		t = object_desc_num(t, o_ptr->pval);
		t = object_desc_str(t, " charge");

		if (o_ptr->pval != 1)
		{
			t = object_desc_chr(t, 's');
		}

		t = object_desc_chr(t, p2);
	}
	/* Hack -- Rods have a "charging" indicator.  Now that stacks of rods may
	 * be in any state of charge or discharge, this now includes a number. -LM-
	 */
	else if (known && (o_ptr->tval == TV_ROD))
	{
		/* Hack -- Dump " (# charging)" if relevant */
		if (o_ptr->timeout)
		{
			/* Stacks of rods display an exact count of charging rods. */
			if (o_ptr->number > 1)
			{
				/* Paranoia. */
				if (k_ptr->pval == 0)
					k_ptr->pval = 1;

				/* Find out how many rods are charging, by dividing
				 * current timeout by each rod's maximum timeout.
				 * Ensure that any remainder is rounded up.  Display
				 * very discharged stacks as merely fully discharged.
				 */
				power = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
				if (power > o_ptr->number)
					power = o_ptr->number;

				/* Display prettily. */
				t = object_desc_str(t, " (");
				t = object_desc_num(t, power);
				t = object_desc_str(t, " charging)");
			}

			/* "one Rod of Perception (1 charging)" would look tacky. */
			else
			{
				t = object_desc_str(t, " (charging)");
			}
		}
	}

	/* Hack -- Process Lanterns/Torches */
	else if ((o_ptr->tval == TV_LITE) && (!artifact_p(o_ptr)))
	{
		/* Hack -- Turns of light for normal lites */
		t = object_desc_str(t, " (with ");
		t = object_desc_num(t, o_ptr->pval);
		t = object_desc_str(t, " turns of light)");
	}


	/* Dump "pval" flags for wearable items */
	if (known && (f1 & (TR1_PVAL_MASK)))
	{
		/* Start the display */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);

		/* Dump the "pval" itself */
		t = object_desc_int(t, o_ptr->pval);

		/* Do not display the "pval" flags */
		if (f3 & (TR3_HIDE_TYPE))
		{
			/* Nothing */
		}

		/* Speed */
		else if (f1 & (TR1_SPEED))
		{
			/* Dump " to speed" */
			t = object_desc_str(t, " to speed");
		}

		/* Attack speed */
		else if (f1 & (TR1_BLOWS))
		{
			/* Add " attack" */
			t = object_desc_str(t, " attack");

			/* Add "attacks" */
			if (ABS(o_ptr->pval) != 1)
				t = object_desc_chr(t, 's');
		}

		/* Stealth */
		else if (f1 & (TR1_STEALTH))
		{
			/* Dump " to stealth" */
			t = object_desc_str(t, " to stealth");
		}

		/* Search */
		else if (f1 & (TR1_SEARCH))
		{
			/* Dump " to searching" */
			t = object_desc_str(t, " to searching");
		}

		/* Infravision */
		else if (f1 & (TR1_INFRA))
		{
			/* Dump " to infravision" */
			t = object_desc_str(t, " to infravision");
		}

		/* Tunneling */
		else if (f1 & (TR1_TUNNEL))
		{
			/* Nothing */
		}

		/* Finish the display */
		t = object_desc_chr(t, p2);
	}

	/* Indicate charging objects, but not rods. */
	if (known && o_ptr->timeout && o_ptr->tval != TV_ROD)
	{
		/* Hack -- Dump " (charging)" if relevant */
		t = object_desc_str(t, " (charging)");
	}


	/* No more details wanted */
	if (mode < 3)
		goto copyback;

#if 1 /* TNB */

	(void) object_desc_inscrip(o_ptr, tmp_val2);
	if (tmp_val2[0])
	{
		/* Append the inscription */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, c1);
		t = object_desc_str(t, tmp_val2);
		t = object_desc_chr(t, c2);
	}

#else /* not 1 -- TNB */

	/* No inscription yet */
	tmp_val2[0] = '\0';

	/* Use the standard inscription if available */
	if (o_ptr->inscription)
	{
		char *u = tmp_val2;

		strcpy(tmp_val2, quark_str(o_ptr->inscription));

		for (; *u && (*u != '#'); u++);

		*u = '\0';
	}

	/* Use the game-generated "feeling" otherwise, if available */
	else if (o_ptr->feeling)
	{
		strcpy(tmp_val2, game_inscriptions[o_ptr->feeling]);
	}

	/* Note "cursed" if the item is known to be cursed */
	else if (cursed_p(o_ptr) && (known || (o_ptr->ident & (IDENT_SENSE))))
	{
		strcpy(tmp_val2, "cursed");
	}

	/* Mega-Hack -- note empty wands/staffs */
	else if (!known && (o_ptr->ident & (IDENT_EMPTY)))
	{
		strcpy(tmp_val2, "empty");
	}

	/* Note "tried" if the object has been tested unsuccessfully */
	else if (!aware && object_tried_p(o_ptr))
	{
		strcpy(tmp_val2, "tried");
	}

	/* Note the discount, if any */
	else if (o_ptr->discount)
	{
		(void) object_desc_num(tmp_val2, o_ptr->discount);
		strcat(tmp_val2, "% off");
	}

	/* Append the inscription, if any */
	if (tmp_val2[0])
	{
		int n;

		/* Hack -- How much so far */
		n = (t - tmp_val);

		/* Paranoia -- do not be stupid */
		if (n > 75)
			n = 75;

		/* Hack -- shrink the inscription */
		tmp_val2[75 - n] = '\0';

		/* Append the inscription */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, c1);
		t = object_desc_str(t, tmp_val2);
		t = object_desc_chr(t, c2);
	}

#endif /* not 1 -- TNB */

  copyback:
	/* Here's where we dump the built string into buf. */
#if 0 /* TNB */
	tmp_val[79] = '\0';
#endif /* TNB */
	t = tmp_val;
	while ((*(buf++) = *(t++))); /* copy the string over */
}
