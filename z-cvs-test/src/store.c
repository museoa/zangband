/* CVS: Last edit by $Author$ on $Date$ */
/* File: store.c */

/* Purpose: Store commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


#include "tnb.h" /* TNB */


#define RUMOR_CHANCE 8

#define MAX_COMMENT_1	6

static cptr comment_1[MAX_COMMENT_1] = {
	"Okay.",
	"Fine.",
	"Accepted!",
	"Agreed!",
	"Done!",
	"Taken!"
};

#define MAX_COMMENT_2A	2

static cptr comment_2a[MAX_COMMENT_2A] = {
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
};

#define MAX_COMMENT_2B	12

static cptr comment_2b[MAX_COMMENT_2B] = {
	"I can take no less than %s gold pieces.",
	"I will accept no less than %s gold pieces.",
	"Ha!  No less than %s gold pieces.",
	"You knave!  No less than %s gold pieces.",
	"That's a pittance!  I want %s gold pieces.",
	"That's an insult!  I want %s gold pieces.",
	"As if!  How about %s gold pieces?",
	"My arse!  How about %s gold pieces?",
	"May the fleas of 1000 orcs molest you!  Try %s gold pieces.",
	"May your most favourite parts go moldy!  Try %s gold pieces.",
	"May Morgoth find you tasty!  Perhaps %s gold pieces?",
	"Your mother was an Ogre!  Perhaps %s gold pieces?"
};

#define MAX_COMMENT_3A	2

static cptr comment_3a[MAX_COMMENT_3A] = {
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
};


#define MAX_COMMENT_3B	12

static cptr comment_3b[MAX_COMMENT_3B] = {
	"Perhaps %s gold pieces?",
	"How about %s gold pieces?",
	"I will pay no more than %s gold pieces.",
	"I can afford no more than %s gold pieces.",
	"Be reasonable.  How about %s gold pieces?",
	"I'll buy it as scrap for %s gold pieces.",
	"That is too much!  How about %s gold pieces?",
	"That looks war surplus!  Say %s gold pieces?",
	"Never!  %s is more like it.",
	"That's an insult!  %s is more like it.",
	"%s gold pieces and be thankful for it!",
	"%s gold pieces and not a copper more!"
};

#define MAX_COMMENT_4A	4

static cptr comment_4a[MAX_COMMENT_4A] = {
	"Enough!  You have abused me once too often!",
	"Arghhh!  I have had enough abuse for one day!",
	"That does it!  You shall waste my time no more!",
	"This is getting nowhere!  I'm going to Londis!"
};

#define MAX_COMMENT_4B	4

static cptr comment_4b[MAX_COMMENT_4B] = {
	"Leave my store!",
	"Get out of my sight!",
	"Begone, you scoundrel!",
	"Out, out, out!"
};

#define MAX_COMMENT_5	8

static cptr comment_5[MAX_COMMENT_5] = {
	"Try again.",
	"Ridiculous!",
	"You will have to do better than that!",
	"Do you wish to do business or not?",
	"You've got to be kidding!",
	"You'd better be kidding!",
	"You try my patience.",
	"Hmmm, nice weather we're having."
};

#define MAX_COMMENT_6	4

static cptr comment_6[MAX_COMMENT_6] = {
	"I must have heard you wrong.",
	"I'm sorry, I missed that.",
	"I'm sorry, what was that?",
	"Sorry, what was that again?"
};



/*
 * Successful haggle.
 */
static void say_comment_1(void)
{
	char rumour[1024];

	msg_print(comment_1[rand_int(MAX_COMMENT_1)]);

	if ((randint(RUMOR_CHANCE) == 1) && speak_unique)
	{
		msg_print("The shopkeeper whispers something into your ear:");

		if (!get_rnd_line("rumors.txt", 0, rumour))
			msg_print(rumour);
	}
}


/*
 * Continue haggling (player is buying)
 */
static void say_comment_2(s32b value, int annoyed)
{
	char tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long) value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_2a[rand_int(MAX_COMMENT_2A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
		msg_format(comment_2b[rand_int(MAX_COMMENT_2B)], tmp_val);
	}
}


/*
 * Continue haggling (player is selling)
 */
static void say_comment_3(s32b value, int annoyed)
{
	char tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long) value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_3a[rand_int(MAX_COMMENT_3A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
		msg_format(comment_3b[rand_int(MAX_COMMENT_3B)], tmp_val);
	}
}


/*
 * Kick 'da bum out.					-RAK-
 */
static void say_comment_4(void)
{
	msg_print(comment_4a[rand_int(MAX_COMMENT_4A)]);
	msg_print(comment_4b[rand_int(MAX_COMMENT_4B)]);
}


/*
 * You are insulting me
 */
static void say_comment_5(void)
{
	msg_print(comment_5[rand_int(MAX_COMMENT_5)]);
}


/*
 * That makes no sense.
 */
static void say_comment_6(void)
{
	msg_print(comment_6[rand_int(MAX_COMMENT_6)]);
}



/*
 * Messages for reacting to purchase prices.
 */

#define MAX_COMMENT_7A	4

static cptr comment_7a[MAX_COMMENT_7A] = {
	"Arrgghh!",
	"You bastard!",
	"You hear someone sobbing...",
	"The shopkeeper howls in agony!"
};

#define MAX_COMMENT_7B	4

static cptr comment_7b[MAX_COMMENT_7B] = {
	"Damn!",
	"You fiend!",
	"The shopkeeper curses at you.",
	"The shopkeeper glares at you."
};

#define MAX_COMMENT_7C	4

static cptr comment_7c[MAX_COMMENT_7C] = {
	"Cool!",
	"You've made my day!",
	"The shopkeeper giggles.",
	"The shopkeeper laughs loudly."
};

#define MAX_COMMENT_7D	4

static cptr comment_7d[MAX_COMMENT_7D] = {
	"Yipee!",
	"I think I'll retire!",
	"The shopkeeper jumps for joy.",
	"The shopkeeper smiles gleefully."
};


/*
 * Let a shop-keeper React to a purchase
 *
 * We paid "price", it was worth "value", and we thought it was worth "guess"
 */
static void purchase_analyze(s32b price, s32b value, s32b guess)
{
	/* Item was worthless, but we bought it */
	if ((value <= 0) && (price > value))
	{
		/* Comment */
		msg_print(comment_7a[rand_int(MAX_COMMENT_7A)]);

		chg_virtue(V_HONOUR, -1);
		chg_virtue(V_JUSTICE, -1);
	}

	/* Item was cheaper than we thought, and we paid more than necessary */
	else if ((value < guess) && (price > value))
	{
		/* Comment */
		msg_print(comment_7b[rand_int(MAX_COMMENT_7B)]);

		chg_virtue(V_JUSTICE, -1);
		if (randint(4) == 1)
			chg_virtue(V_HONOUR, -1);
	}

	/* Item was a good bargain, and we got away with it */
	else if ((value > guess) && (value < (4 * guess)) && (price < value))
	{
		/* Comment */
		msg_print(comment_7c[rand_int(MAX_COMMENT_7C)]);

		if (randint(4) == 1)
			chg_virtue(V_HONOUR, -1);
		else if (randint(4) == 1)
			chg_virtue(V_HONOUR, 1);
	}

	/* Item was a great bargain, and we got away with it */
	else if ((value > guess) && (price < value))
	{
		/* Comment */
		msg_print(comment_7d[rand_int(MAX_COMMENT_7D)]);

		if (randint(2) == 1)
			chg_virtue(V_HONOUR, -1);
		if (randint(4) == 1)
			chg_virtue(V_HONOUR, 1);

		if (10 * price < value)
			chg_virtue(V_SACRIFICE, 1);
	}
}





/*
 * We store the current "store number" here so everyone can access it
 */
static int cur_store_num = 7;

/*
 * We store the current "store page" here so everyone can access it
 */
static int store_top = 0;

/*
 * We store the current "store pointer" here so everyone can access it
 */
static store_type *st_ptr = NULL;

/*
 * We store the current "owner type" here so everyone can access it
 */
static owner_type *ot_ptr = NULL;






/*
 * Buying and selling adjustments for race combinations.
 * Entry[owner][player] gives the basic "cost inflation".
 */
static byte rgold_adj[MAX_RACES][MAX_RACES] = {
	/*Hum, HfE, Elf,  Hal, Gno, Dwa, HfO, HfT, Dun, HiE, Barbarian,
	 * HfOg, HGn, HTn, Cyc, Yek, Klc, Kbd, Nbl, DkE, Drc, Mind Flayer,
	 * Imp,  Glm, Skl, Zombie, Vampire, Spectre */

	/* Human */
	{100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
			124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
		115, 105, 125, 125, 125, 125, 105, 120},

	/* Half-Elf */
	{110, 100, 100, 105, 110, 120, 125, 130, 110, 100, 110,
			120, 115, 108, 115, 110, 110, 120, 120, 115, 115, 110,
		120, 110, 110, 110, 120, 110, 100, 125},

	/* Elf */
	{110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
			120, 120, 105, 120, 110, 105, 125, 125, 110, 115, 108,
		120, 115, 110, 110, 120, 110, 100, 125},

	/* Halfling */
	{115, 110, 105, 95, 105, 110, 115, 130, 115, 105, 115,
			125, 120, 120, 125, 115, 110, 120, 120, 120, 115, 115,
		120, 110, 120, 120, 130, 110, 110, 130},

	/* Gnome */
	{115, 115, 110, 105, 95, 110, 115, 130, 115, 110, 115,
			120, 125, 110, 120, 110, 105, 120, 110, 110, 105, 110,
		120, 101, 110, 110, 120, 120, 115, 130},

	/* Dwarf */
	{115, 120, 120, 110, 110, 95, 125, 135, 115, 120, 115,
			125, 140, 130, 130, 120, 115, 115, 115, 135, 125, 120,
		120, 105, 115, 115, 115, 115, 120, 130},

	/* Half-Orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 115, 120, 125, 115},

	/* Half-Troll */
	{110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
			110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
		110, 115, 112, 112, 115, 112, 120, 110},

	/* Amberite (Dunedain)  */
	{100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
			120, 120, 105, 120, 115, 105, 115, 120, 110, 105, 105,
		120, 105, 120, 120, 125, 120, 105, 135},

	/* High_Elf */
	{110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
			125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
		125, 115, 120, 120, 125, 120, 100, 125},

	/* Human / Barbarian (copied from human) */
	{100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
			124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
		115, 105, 125, 125, 130, 125, 115, 120},

	/* Half-Ogre: theoretical, copied from half-troll */
	{110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
			110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
		110, 115, 112, 112, 115, 112, 120, 110},

	/* Half-Giant: theoretical, copied from half-troll */
	{110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
			110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
		110, 115, 112, 112, 115, 112, 130, 120},

	/* Half-Titan: theoretical, copied from High_Elf */
	{110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
			125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
		125, 115, 120, 120, 120, 120, 130, 130},

	/* Cyclops: theoretical, copied from half-troll */
	{110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
			110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
		110, 115, 112, 112, 115, 112, 130, 130},

	/* Yeek: theoretical, copied from Half-Orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 120, 120, 130, 130},

	/* Klackon: theoretical, copied from Gnome */
	{115, 115, 110, 105, 95, 110, 115, 130, 115, 110, 115,
			120, 125, 110, 120, 110, 105, 120, 110, 110, 105, 110,
		120, 101, 110, 110, 120, 120, 130, 130},

	/* Kobold: theoretical, copied from Half-Orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 120, 120, 130, 130},

	/* Nibelung: theoretical, copied from Dwarf */
	{115, 120, 120, 110, 110, 95, 125, 135, 115, 120, 115,
			125, 140, 130, 130, 120, 115, 115, 115, 135, 125, 120,
		120, 105, 115, 115, 120, 120, 130, 130},

	/* Dark Elf */
	{110, 110, 110, 115, 120, 130, 115, 115, 120, 110, 115,
			115, 115, 116, 115, 120, 120, 115, 115, 101, 110, 110,
		110, 110, 112, 122, 110, 110, 110, 115},

	/* Draconian: theoretical, copied from High_Elf */
	{110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
			125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
		125, 115, 120, 120, 120, 120, 130, 130},

	/* Mind Flayer: theoretical, copied from High_Elf */
	{110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
			125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
		125, 115, 120, 120, 120, 120, 130, 130},

	/* Imp: theoretical, copied from High_Elf */
	{110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
			125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
		125, 115, 120, 120, 120, 120, 130, 130},

	/* Golem: theoretical, copied from High_Elf */
	{110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
			125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
		125, 115, 120, 120, 120, 120, 130, 130},

	/* Skeleton: theoretical, copied from half-orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 120, 120, 130, 130},

	/* Zombie: Theoretical, copied from half-orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 120, 120, 130, 130},

	/* Vampire: Theoretical, copied from half-orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 120, 120, 130, 130},

	/* Spectre: Theoretical, copied from half-orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 120, 120, 130, 130},

	/* Sprite: Theoretical, copied from half-orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 120, 120, 130, 130},

	/* Beastman: Theoretical, copied from half-orc */
	{115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
			110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
		115, 125, 120, 120, 120, 120, 130, 130},

};




/*
 * Determine the price of an item (qty one) in a store.
 *
 * This function takes into account the player's charisma, and the
 * shop-keepers friendliness, and the shop-keeper's base greed, but
 * never lets a shop-keeper lose money in a transaction.
 *
 * The "greed" value should exceed 100 when the player is "buying" the
 * item, and should be less than 100 when the player is "selling" it.
 *
 * Hack -- the black market always charges twice as much as it should.
 *
 * Charisma adjustment runs from 80 to 130
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 */
static s32b price_item(object_type * o_ptr, int greed, bool flip)
{
	int factor;
	int adjust;
	s32b price;


	/* Get the value of one of the items */
	price = object_value(o_ptr);

	/* Worthless items */
	if (price <= 0)
		return (0L);


	/* Compute the racial factor */
	factor = rgold_adj[ot_ptr->owner_race][p_ptr->prace];

	/* Add in the charisma factor */
	factor += adj_chr_gold[p_ptr->stat_ind[A_CHR]];


	/* Shop is buying */
	if (flip)
	{
		/* Adjust for greed */
		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if (adjust > 100)
			adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (cur_store_num == STORE_BLACK)
			price = price / 2;
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if (adjust < 100)
			adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (cur_store_num == STORE_BLACK)
			price = price * 2;
	}

	/* Compute the final price (with rounding) */
	price = (price * adjust + 50L) / 100L;

	/* Note -- Never become "free" */
	if (price <= 0L)
		return (1L);

	/* Return the price */
	return (price);
}


/*
 * Certain "cheap" objects should be created in "piles"
 * Some objects can be sold at a "discount" (in small piles)
 */
static void mass_produce(object_type * o_ptr)
{
	int size = 1;
	int discount = 0;

	s32b cost = object_value(o_ptr);


	/* Analyze the type */
	switch (o_ptr->tval)
	{
			/* Food, Flasks, and Lites */
		case TV_FOOD:
		case TV_FLASK:
		case TV_LITE:
		{
			if (cost <= 5L)
				size += damroll(3, 5);
			if (cost <= 20L)
				size += damroll(3, 5);
			break;
		}

		case TV_POTION:
		case TV_SCROLL:
		{
			if (cost <= 60L)
				size += damroll(3, 5);
			if (cost <= 240L)
				size += damroll(1, 5);
			break;
		}

		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		{
			if (cost <= 50L)
				size += damroll(2, 3);
			if (cost <= 500L)
				size += damroll(1, 3);
			break;
		}

		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SHIELD:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_CLOAK:
		case TV_HELM:
		case TV_CROWN:
		case TV_SWORD:
		case TV_POLEARM:
		case TV_HAFTED:
		case TV_DIGGING:
		case TV_BOW:
		{
			if (o_ptr->name2)
				break;
			if (cost <= 10L)
				size += damroll(3, 5);
			if (cost <= 100L)
				size += damroll(3, 5);
			break;
		}

		case TV_SPIKE:
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (cost <= 5L)
				size += damroll(5, 5);
			if (cost <= 50L)
				size += damroll(5, 5);
			if (cost <= 500L)
				size += damroll(5, 5);
			break;
		}

		case TV_FIGURINE:
		case TV_STATUE:
		{
			if (cost <= 100L)
				size += damroll(2, 2);
			if (cost <= 1000L)
				size += damroll(2, 2);
			break;
		}

			/*
			 * Because many rods (and a few wands and staffs) are useful mainly
			 * in quantity, the Black Market will occasionally have a bunch of
			 * one kind. -LM-
			 */
		case TV_ROD:
		case TV_WAND:
		case TV_STAFF:
		{
			if ((cur_store_num == STORE_BLACK) && (randint(3) == 1))
			{
				if (cost < 1601L)
					size += damroll(1, 5);
				else if (cost < 3201L)
					size += damroll(1, 3);
			}

			/* Ensure that mass-produced rods and wands get the correct pvals. */
			if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
			{
				o_ptr->pval *= size;
			}
			break;
		}
	}


	/* Pick a discount */
	if (cost < 5)
	{
		discount = 0;
	}
	else if (rand_int(25) == 0)
	{
		discount = 25;
	}
	else if (rand_int(150) == 0)
	{
		discount = 50;
	}
	else if (rand_int(300) == 0)
	{
		discount = 75;
	}
	else if (rand_int(500) == 0)
	{
		discount = 90;
	}


	if (o_ptr->art_name)
	{
		if (cheat_peek && discount)
		{
			msg_print("No discount on random artifacts.");
		}
		discount = 0;
	}

	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = size - (size * discount / 100);

#if 1 /* BUG -- TNB */
	if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
	{
		o_ptr->pval = o_ptr->number * k_info[o_ptr->k_idx].pval;
	}
#endif /* BUG -- TNB */
}



/*
 * Determine if a store item can "absorb" another item
 *
 * See "object_similar()" for the same function for the "player"
 */
static bool store_object_similar(object_type * o_ptr, object_type * j_ptr)
{
	/* Hack -- Identical items cannot be stacked */
	if (o_ptr == j_ptr)
		return (0);

	/* Different objects cannot be stacked */
	if (o_ptr->k_idx != j_ptr->k_idx)
		return (0);

	/* Different charges (etc) cannot be stacked, unless wands or rods. */
	if ((o_ptr->pval != j_ptr->pval) && (o_ptr->tval != TV_WAND) &&
		(o_ptr->tval != TV_ROD))
		return (0);

	/* Require many identical values */
	if (o_ptr->to_h != j_ptr->to_h)
		return (0);
	if (o_ptr->to_d != j_ptr->to_d)
		return (0);
	if (o_ptr->to_a != j_ptr->to_a)
		return (0);

	/* Require identical "artifact" names */
	if (o_ptr->name1 != j_ptr->name1)
		return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->name2 != j_ptr->name2)
		return (0);

	/* Random artifacts don't stack ! */
	if (o_ptr->art_name || j_ptr->art_name)
		return (0);

	/* Hack -- Identical art_flags! */
	if ((o_ptr->art_flags1 != j_ptr->art_flags1) ||
		(o_ptr->art_flags2 != j_ptr->art_flags2) ||
		(o_ptr->art_flags3 != j_ptr->art_flags3))
		return (0);

	/* Hack -- Never stack "powerful" items */
	if (o_ptr->xtra1 || j_ptr->xtra1)
		return (0);

	/* Hack -- Never stack recharging items */
	if (o_ptr->timeout || j_ptr->timeout)
		return (0);

	/* Require many identical values */
	if (o_ptr->ac != j_ptr->ac)
		return (0);
	if (o_ptr->dd != j_ptr->dd)
		return (0);
	if (o_ptr->ds != j_ptr->ds)
		return (0);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST)
		return (0);

	/* Require matching discounts */
	if (o_ptr->discount != j_ptr->discount)
		return (0);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store item to absorb another item
 */
static void store_object_absorb(object_type * o_ptr, object_type * j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > 99) ? 99 : total;

	/* Hack -- if rods are stacking, add the pvals (maximum timeouts) together. -LM- */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval += j_ptr->pval;
	}

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if (o_ptr->tval == TV_WAND)
	{
		o_ptr->pval += j_ptr->pval;
	}
}


/*
 * Check to see if the shop will be carrying too many objects	-RAK-
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.	Before, one could "nuke" potions this way.
 */
static bool store_check_num(object_type * o_ptr)
{
	int i;
	object_type *j_ptr;

	/* Free space is always usable */
	if (st_ptr->stock_num < st_ptr->stock_size)
		return TRUE;

	/* The "home" acts like the player */
	if (cur_store_num == STORE_HOME)
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (object_similar(j_ptr, o_ptr))
				return (TRUE);
		}
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr))
				return (TRUE);
		}
	}

	/* But there was no room at the inn... */
	return (FALSE);
}


static bool is_blessed(object_type * o_ptr)
{
	u32b f1, f2, f3;
	object_flags(o_ptr, &f1, &f2, &f3);
	if (f3 & TR3_BLESSED)
		return (TRUE);
	else
		return (FALSE);
}



/*
 * Determine if the current store will purchase the given item
 *
 * Note that a shop-keeper must refuse to buy "worthless" items
 */
#if 1 /* TNB */
bool store_will_buy(const object_type * o_ptr)
#else /* TNB */
static bool store_will_buy(object_type * o_ptr)
#endif /* TNB */
{
	/* Hack -- The Home is simple */
	if (cur_store_num == STORE_HOME)
		return (TRUE);

	/* Switch on the store */
	switch (cur_store_num)
	{
			/* General Store */
		case STORE_GENERAL:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_FOOD:
				case TV_LITE:
				case TV_FLASK:
				case TV_SPIKE:
				case TV_SHOT:
				case TV_ARROW:
				case TV_BOLT:
				case TV_DIGGING:
				case TV_CLOAK:
				case TV_BOTTLE: /* 'Green', recycling Angband */
				case TV_FIGURINE:
				case TV_STATUE:
					break;
				default:
					return (FALSE);
			}
			break;
		}

			/* Armoury */
		case STORE_ARMOURY:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_BOOTS:
				case TV_GLOVES:
				case TV_CROWN:
				case TV_HELM:
				case TV_SHIELD:
				case TV_CLOAK:
				case TV_SOFT_ARMOR:
				case TV_HARD_ARMOR:
				case TV_DRAG_ARMOR:
					break;
				default:
					return (FALSE);
			}
			break;
		}

			/* Weapon Shop */
		case STORE_WEAPON:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SHOT:
				case TV_BOLT:
				case TV_ARROW:
				case TV_BOW:
				case TV_DIGGING:
				case TV_HAFTED:
				case TV_POLEARM:
				case TV_SWORD:
					break;
				default:
					return (FALSE);
			}
			break;
		}

			/* Temple */
		case STORE_TEMPLE:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_LIFE_BOOK:
				case TV_SCROLL:
				case TV_POTION:
				case TV_HAFTED:
				{
					break;
				}
				case TV_FIGURINE:
				case TV_STATUE:
				{
					monster_race *r_ptr = &r_info[o_ptr->pval];

					/* Decline evil */
					if (!(r_ptr->flags3 & RF3_EVIL))
					{
						/* Accept good */
						if (r_ptr->flags3 & RF3_GOOD)
							break;

						/* Accept animals */
						if (r_ptr->flags3 & RF3_ANIMAL)
							break;

						/* Accept mimics */
						if (strchr("?!", r_ptr->d_char))
							break;
					}
				}
				case TV_POLEARM:
				case TV_SWORD:
				{
					if (is_blessed(o_ptr))
						break;
				}
				default:
					return (FALSE);
			}
			break;
		}

			/* Alchemist */
		case STORE_ALCHEMIST:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SCROLL:
				case TV_POTION:
					break;
				default:
					return (FALSE);
			}
			break;
		}

			/* Magic Shop */
		case STORE_MAGIC:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SORCERY_BOOK:
				case TV_NATURE_BOOK:
				case TV_CHAOS_BOOK:
				case TV_DEATH_BOOK:
				case TV_TRUMP_BOOK:
				case TV_ARCANE_BOOK:
				case TV_AMULET:
				case TV_RING:
				case TV_STAFF:
				case TV_WAND:
				case TV_ROD:
				case TV_SCROLL:
				case TV_POTION:
				case TV_FIGURINE:
					break;
				default:
					return (FALSE);
			}
			break;
		}
			/* Bookstore Shop */
		case STORE_BOOK:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SORCERY_BOOK:
				case TV_NATURE_BOOK:
				case TV_CHAOS_BOOK:
				case TV_DEATH_BOOK:
				case TV_LIFE_BOOK:
				case TV_TRUMP_BOOK:
				case TV_ARCANE_BOOK:
					break;
				default:
					return (FALSE);
			}
			break;
		}
	}

	/* XXX XXX XXX Ignore "worthless" items */
	if (object_value(o_ptr) <= 0)
		return (FALSE);

	/* Assume okay */
	return (TRUE);
}



/*
 * Add the item "o_ptr" to the inventory of the "Home"
 *
 * In all cases, return the slot (or -1) where the object was placed
 *
 * Note that this is a hacked up version of "inven_carry()".
 *
 * Also note that it may not correctly "adapt" to "knowledge" bacoming
 * known, the player may have to pick stuff up and drop it again.
 */
static int home_carry(object_type * o_ptr)
{
	int slot;
	s32b value, j_value;
	int i;
	object_type *j_ptr;


	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* The home acts just like the player */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size)
		return (-1);


	/* Determine the "value" of the item */
	value = object_value(o_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that item */
		j_ptr = &st_ptr->stock[slot];

		/* Hack -- readable books always come first */
		if ((o_ptr->tval == mp_ptr->spell_book) &&
			(j_ptr->tval != mp_ptr->spell_book))
			break;
		if ((j_ptr->tval == mp_ptr->spell_book) &&
			(o_ptr->tval != mp_ptr->spell_book))
			continue;

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval)
			break;
		if (o_ptr->tval < j_ptr->tval)
			continue;

		/* Can happen in the home */
		if (!object_aware_p(o_ptr))
			continue;
		if (!object_aware_p(j_ptr))
			break;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval)
			break;
		if (o_ptr->sval > j_ptr->sval)
			continue;

		/* Objects in the home can be unknown */
		if (!object_known_p(o_ptr))
			continue;
		if (!object_known_p(j_ptr))
			break;

		/*
		 * Hack:  otherwise identical rods sort by
		 * increasing recharge time --dsb
		 */
		if (o_ptr->tval == TV_ROD)
		{
			if (o_ptr->pval < j_ptr->pval)
				break;
			if (o_ptr->pval > j_ptr->pval)
				continue;
		}

		/* Objects sort by decreasing value */
		j_value = object_value(j_ptr);
		if (value > j_value)
			break;
		if (value < j_value)
			continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i - 1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	chg_virtue(V_SACRIFICE, -1);

	/* Return the location */
	return (slot);
}


/*
 * Add the item "o_ptr" to a real stores inventory.
 *
 * If the item is "worthless", it is thrown away (except in the home).
 *
 * If the item cannot be combined with an object already in the inventory,
 * make a new slot for it, and calculate its "per item" price.	Note that
 * this price will be negative, since the price will not be "fixed" yet.
 * Adding an item to a "fixed" price stack will not change the fixed price.
 *
 * In all cases, return the slot (or -1) where the object was placed
 */
static int store_carry(object_type * o_ptr)
{
	int i, slot;
	s32b value, j_value;
	object_type *j_ptr;


	/* Evaluate the object */
	value = object_value(o_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	if (value <= 0)
		return (-1);

	/* All store items are fully *identified* */
	o_ptr->ident |= IDENT_MENTAL;

	/* Erase the inscription */
	o_ptr->inscription = 0;

	/* Erase the "feeling" */
	o_ptr->feeling = FEEL_NONE;

	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* Can the existing items be incremented? */
		if (store_object_similar(j_ptr, o_ptr))
		{
			/* Hack -- extra items disappear */
			store_object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size)
		return (-1);


	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that item */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval)
			break;
		if (o_ptr->tval < j_ptr->tval)
			continue;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval)
			break;
		if (o_ptr->sval > j_ptr->sval)
			continue;

		/*
		 * Hack:  otherwise identical rods sort by
		 * increasing recharge time --dsb 
		 */
		if (o_ptr->tval == TV_ROD)
		{
			if (o_ptr->pval < j_ptr->pval)
				break;
			if (o_ptr->pval > j_ptr->pval)
				continue;
		}

		/* Evaluate that slot */
		j_value = object_value(j_ptr);

		/* Objects sort by decreasing value */
		if (value > j_value)
			break;
		if (value < j_value)
			continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i - 1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	/* Return the location */
	return (slot);
}


/*
 * Increase, by a given amount, the number of a certain item
 * in a certain store.	This can result in zero items.
 */
static void store_item_increase(int item, int num)
{
	int cnt;
	object_type *o_ptr;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Verify the number */
	cnt = o_ptr->number + num;
	if (cnt > 255)
		cnt = 255;
	else if (cnt < 0)
		cnt = 0;
	num = cnt - o_ptr->number;

	/* Save the new number */
	o_ptr->number += num;
}


/*
 * Remove a slot if it is empty
 */
static void store_item_optimize(int item)
{
	int j;
	object_type *o_ptr;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Must exist */
	if (!o_ptr->k_idx)
		return;

	/* Must have no items */
	if (o_ptr->number)
		return;

	/* One less item */
	st_ptr->stock_num--;

	/* Slide everyone */
	for (j = item; j < st_ptr->stock_num; j++)
	{
		st_ptr->stock[j] = st_ptr->stock[j + 1];
	}

	/* Nuke the final slot */
	object_wipe(&st_ptr->stock[j]);
}


/*
 * This function will keep 'crap' out of the black market.
 * Crap is defined as any item that is "available" elsewhere
 * Based on a suggestion by "Lee Vogt" <lvogt@cig.mcel.mot.com>
 */
static bool black_market_crap(object_type * o_ptr)
{
	int i, j;

	/* Ego items are never crap */
	if (o_ptr->name2)
		return (FALSE);

	/* Good items are never crap */
	if (o_ptr->to_a > 0)
		return (FALSE);
	if (o_ptr->to_h > 0)
		return (FALSE);
	if (o_ptr->to_d > 0)
		return (FALSE);

	/* Check all stores */
	for (i = 0; i < MAX_STORES; i++)
	{
		if (i == STORE_HOME)
			continue;

		/* Check every item in the store */
		for (j = 0; j < town[p_ptr->town_num].store[i].stock_num; j++)
		{
			object_type *j_ptr = &town[p_ptr->town_num].store[i].stock[j];

			/* Duplicate item "type", assume crappy */
			if (o_ptr->k_idx == j_ptr->k_idx)
				return (TRUE);
		}
	}

	/* Assume okay */
	return (FALSE);
}


/*
 * Attempt to delete (some of) a random item from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
static void store_delete(void)
{
	int what, num;

	/* Pick a random slot */
	what = rand_int(st_ptr->stock_num);

	/* Determine how many items are here */
	num = st_ptr->stock[what].number;

	/* Hack -- sometimes, only destroy half the items */
	if (rand_int(100) < 50)
		num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single item */
	if (rand_int(100) < 50)
		num = 1;

	/* Hack -- decrement the maximum timeouts and total charges of rods and wands. -LM- */
	if ((st_ptr->stock[what].tval == TV_ROD) ||
		(st_ptr->stock[what].tval == TV_WAND))
	{
		st_ptr->stock[what].pval -=
			num * st_ptr->stock[what].pval / st_ptr->stock[what].number;
	}

	/* Actually destroy (part of) the item */
	store_item_increase(what, -num);
	store_item_optimize(what);
}


/*
 * Creates a random item and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 * Currently, "normal" stores use a pre-built array.
 *
 * Note -- the "level" given to "obj_get_num()" is a "favored"
 * level, that is, there is a much higher chance of getting
 * items with a level approaching that of the given level...
 *
 * Should we check for "permission" to have the given item?
 */
static void store_create(void)
{
	int i, tries, level;

	object_type forge;
	object_type *q_ptr;


	/* Paranoia -- no room left */
	if (st_ptr->stock_num >= st_ptr->stock_size)
		return;


	/* Hack -- consider up to four items */
	for (tries = 0; tries < 4; tries++)
	{
		/* Black Market */
		if (cur_store_num == STORE_BLACK)
		{
			/* Pick a level for object/magic */
			level = 25 + rand_int(25);

			/* Random item (usually of given level) */
			i = get_obj_num(level);

			/* Handle failure */
			if (!i)
				continue;
		}

		/* Normal Store */
		else
		{
			/* Hack -- Pick an item to sell */
			i = st_ptr->table[rand_int(st_ptr->table_num)];

			/* Hack -- fake level for apply_magic() */
			level = rand_range(1, STORE_OBJ_LEVEL);
		}


		/* Get local object */
		q_ptr = &forge;

		/* Create a new object of the chosen kind */
		object_prep(q_ptr, i);

		/* Apply some "low-level" magic (no artifacts) */
		apply_magic(q_ptr, level, FALSE, FALSE, FALSE, FALSE);

		/* Require valid object */
		if (!store_will_buy(q_ptr))
			continue;

		/* Hack -- Charge lite's */
		if (q_ptr->tval == TV_LITE)
		{
			if (q_ptr->sval == SV_LITE_TORCH)
				q_ptr->pval = FUEL_TORCH / 2;
			if (q_ptr->sval == SV_LITE_LANTERN)
				q_ptr->pval = FUEL_LAMP / 2;
		}


		/* The item is "known" */
		object_known(q_ptr);

		/* Mark it storebought */
		q_ptr->ident |= IDENT_STOREB;

		/* Mega-Hack -- no chests in stores */
		if (q_ptr->tval == TV_CHEST)
			continue;

		/* Prune the black market */
		if (cur_store_num == STORE_BLACK)
		{
			/* Hack -- No "crappy" items */
			if (black_market_crap(q_ptr))
				continue;

			/* Hack -- No "cheap" items */
			if (object_value(q_ptr) < 10)
				continue;

			/* No "worthless" items */
			/* if (object_value(q_ptr) <= 0) continue; */
		}

		/* Prune normal stores */
		else
		{
			/* No "worthless" items */
			if (object_value(q_ptr) <= 0)
				continue;
		}


		/* Mass produce and/or Apply discount */
		mass_produce(q_ptr);

		/* Attempt to carry the (known) item */
		(void) store_carry(q_ptr);

		/* Definitely done */
		break;
	}
}



/*
 * Eliminate need to bargain if player has haggled well in the past
 */
static bool noneedtobargain(s32b minprice)
{
	s32b good = st_ptr->good_buy;
	s32b bad = st_ptr->bad_buy;

	/* Cheap items are "boring" */
	if (minprice < 10L)
		return (TRUE);

	/* Perfect haggling */
	if (good == MAX_SHORT)
		return (TRUE);

	/* Reward good haggles, punish bad haggles, notice price */
	if (good > ((3 * bad) + (5 + (minprice / 50))))
		return (TRUE);

	/* Return the flag */
	return (FALSE);
}


/*
 * Update the bargain info
 */
static void updatebargain(s32b price, s32b minprice)
{
	/* Hack -- auto-haggle */
	if (auto_haggle)
		return;

	/* Cheap items are "boring" */
	if (minprice < 10L)
		return;

	/* Count the successful haggles */
	if (price == minprice)
	{
		/* Just count the good haggles */
		if (st_ptr->good_buy < MAX_SHORT)
		{
			st_ptr->good_buy++;
		}
	}

	/* Count the failed haggles */
	else
	{
		/* Just count the bad haggles */
		if (st_ptr->bad_buy < MAX_SHORT)
		{
			st_ptr->bad_buy++;
		}
	}
}



/*
 * Re-displays a single store entry
 */
static void display_entry(int pos)
{
#if 0 /* TNB */

	int i;
	object_type *o_ptr;
	s32b x;

	char o_name[80];
	char out_val[160];


	int maxwid = 75;

	/* Get the item */
	o_ptr = &st_ptr->stock[pos];

	/* Get the "offset" */
	i = (pos % 12);

	/* Label it, clear the line --(-- */
	(void) sprintf(out_val, "%c) ", I2A(i));
	prt(out_val, i + 6, 0);

	if (show_store_graph)
	{
		byte a = object_attr(o_ptr);
		char c = object_char(o_ptr);

#ifdef AMIGA
		if (a & 0x80)
			a |= 0x40;
#endif

		Term_draw(3, i + 6, a, c);
	}

	/* Describe an item in the home */
	if (cur_store_num == STORE_HOME)
	{
		maxwid = 75;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights)
			maxwid -= 10;

		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';
		c_put_str(tval_to_attr[o_ptr->tval], o_name, i + 6,
			show_store_graph ? 5 : 3);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
			(void) sprintf(out_val, "%3d.%d lb", wgt / 10, wgt % 10);
			put_str(out_val, i + 6, 68);
		}
	}

	/* Describe an item (fully) in a store */
	else
	{
		/* Must leave room for the "price" */
		maxwid = 65;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights)
			maxwid -= 7;

		/* Describe the object (fully) */
		object_desc_store(o_name, o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';
		c_put_str(tval_to_attr[o_ptr->tval], o_name, i + 6,
			show_store_graph ? 5 : 3);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
			(void) sprintf(out_val, "%3d.%d", wgt / 10, wgt % 10);
			put_str(out_val, i + 6, 61);
		}

		/* Display a "fixed" cost */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Actually draw the price (not fixed) */
			(void) sprintf(out_val, "%9ld F", (long) x);
			put_str(out_val, i + 6, 68);
		}

		/* Display a "taxed" cost */
		else if (auto_haggle)
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Hack -- Apply Sales Tax if needed */
			if (!noneedtobargain(x))
				x += x / 10;

			/* Actually draw the price (with tax) */
			(void) sprintf(out_val, "%9ld  ", (long) x);
			put_str(out_val, i + 6, 68);
		}

		/* Display a "haggle" cost */
		else
		{
			/* Extrect the "maximum" price */
			x = price_item(o_ptr, ot_ptr->max_inflate, FALSE);

			/* Actually draw the price (not fixed) */
			(void) sprintf(out_val, "%9ld  ", (long) x);
			put_str(out_val, i + 6, 68);
		}
	}

#endif /* TNB */
}


/*
 * Displays a store's inventory 		-RAK-
 * All prices are listed as "per individual object".  -BEN-
 */
static void display_inventory(void)
{
#if 0 /* TNB */

	int i, k;

	/* Display the next 12 items */
	for (k = 0; k < 12; k++)
	{
		/* Do not display "dead" items */
		if (store_top + k >= st_ptr->stock_num)
			break;

		/* Display that line */
		display_entry(store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < 13; i++)
		prt("", i + 6, 0);

	/* Assume "no current page" */
	put_str("        ", 5, 20);

	/* Visual reminder of "more items" */
	if (st_ptr->stock_num > 12)
	{
		/* Show "more" reminder (after the last item) */
		prt("-more-", k + 6, 3);

		/* Indicate the "current page" */
		put_str(format("(Page %d)", store_top / 12 + 1), 5, 20);
	}

#endif /* TNB */
}


/*
 * Displays players gold					-RAK-
 */
static void store_prt_gold(void)
{
#if 1 /* TNB */

	Bind_Generic(EVENT_PY, KEYWORD_PY_GOLD + 1);

#else /* TNB */

	char out_val[64];

	prt("Gold Remaining: ", 19, 53);

	sprintf(out_val, "%9ld", (long) p_ptr->au);
	prt(out_val, 19, 68);

#endif /* TNB */
}


/*
 * Displays store (after clearing screen)		-RAK-
 */
static void display_store(void)
{
#if 1 /* TNB */

	/* Show or update the store display */
	angtk_eval("angband_display", "store", "show", NULL);

	/* Display the current gold */
	store_prt_gold();

#else /* TNB */

	char buf[80];


	/* Clear screen */
	Term_clear();

	/* The "Home" is special */
	if (cur_store_num == STORE_HOME)
	{
		/* Put the owner name */
		put_str("Your Home", 3, 30);

		/* Label the item descriptions */
		put_str("Item Description", 5, 3);

		/* If showing weights, show label */
		if (show_weights)
		{
			put_str("Weight", 5, 70);
		}
	}

	/* Normal stores */
	else
	{
		cptr store_name =
			(f_name + f_info[FEAT_SHOP_HEAD + cur_store_num].name);
		cptr owner_name = (ot_ptr->owner_name);
		cptr race_name = race_info[ot_ptr->owner_race].title;

		/* Put the owner name and race */
		sprintf(buf, "%s (%s)", owner_name, race_name);
		put_str(buf, 3, 10);

		/* Show the max price in the store (above prices) */
		sprintf(buf, "%s (%ld)", store_name, (long) (ot_ptr->max_cost));
		prt(buf, 3, 50);

		/* Label the item descriptions */
		put_str("Item Description", 5, 3);

		/* If showing weights, show label */
		if (show_weights)
		{
			put_str("Weight", 5, 60);
		}

		/* Label the asking price (in stores) */
		put_str("Price", 5, 72);
	}

	/* Display the current gold */
	store_prt_gold();

	/* Draw in the inventory */
	display_inventory();

#endif /* TNB */
}



/*
 * Get the ID of a store item and return its value	-RAK-
 */
static int get_stock(int *com_val, cptr pmt, int i, int j)
{
	char command;

	char out_val[160];

#ifdef ALLOW_REPEAT	/* TNB */

	/* Get the item index */
	if (repeat_pull(com_val))
	{
		/* Verify the item */
		if ((*com_val >= i) && (*com_val <= j))
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Assume failure */
	*com_val = (-1);

	/* Build the prompt */
	(void) sprintf(out_val, "(Items %c-%c, ESC to exit) %s", I2A(i),
		I2A(j), pmt);

	/* Ask until done */
	while (TRUE)
	{
		int k;

#if 1 /* TNB */

		int flag;

		inkey_flags = (INKEY_ITEM_STORE);
		flag = get_com(out_val, &command);
		inkey_flags = 0;

		/* Escape */
		if (!flag)
			return (FALSE);

#else /* TNB */

		/* Escape */
		if (!get_com(out_val, &command))
			break;

#endif /* TNB */

		/* Convert */
		k = (islower(command) ? A2I(command) : -1);

		/* Legal responses */
		if ((k >= i) && (k <= j))
		{
			*com_val = k;
			break;
		}

		/* Oops */
		bell();
	}

	/* Clear the prompt */
	prt("", 0, 0);

	/* Cancel */
	if (command == ESCAPE)
		return (FALSE);

#ifdef ALLOW_REPEAT	/* TNB */

	repeat_push(*com_val);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


/*
 * Increase the insult counter and get angry if too many -RAK-
 */
static int increase_insults(void)
{
	/* Increase insults */
	st_ptr->insult_cur++;

	/* Become insulted */
	if (st_ptr->insult_cur > ot_ptr->insult_max)
	{
		/* Complain */
		say_comment_4();

		/* Reset insults */
		st_ptr->insult_cur = 0;
		st_ptr->good_buy = 0;
		st_ptr->bad_buy = 0;

		/* Open tomorrow */
		st_ptr->store_open = turn + 25000 + randint(25000);

		/* Closed */
		return (TRUE);
	}

	/* Not closed */
	return (FALSE);
}


/*
 * Decrease insults 				-RAK-
 */
static void decrease_insults(void)
{
	/* Decrease insults */
	if (st_ptr->insult_cur)
		st_ptr->insult_cur--;
}


/*
 * Have insulted while haggling 			-RAK-
 */
static int haggle_insults(void)
{
	/* Increase insults */
	if (increase_insults())
		return (TRUE);

	/* Display and flush insult */
	say_comment_5();

	/* Still okay */
	return (FALSE);
}


/*
 * Mega-Hack -- Enable "increments"
 */
static bool allow_inc = FALSE;

/*
 * Mega-Hack -- Last "increment" during haggling
 */
static s32b last_inc = 0L;


/*
 * Get a haggle
 */
static int get_haggle(cptr pmt, s32b * poffer, s32b price, int final)
{
	s32b i;

	cptr p;

	char buf[128];
	char out_val[160];


	/* Clear old increment if necessary */
	if (!allow_inc)
		last_inc = 0L;


	/* Final offer */
	if (final)
	{
		sprintf(buf, "%s [accept] ", pmt);
	}

	/* Old (negative) increment, and not final */
	else if (last_inc < 0)
	{
		sprintf(buf, "%s [-%ld] ", pmt, (long) (ABS(last_inc)));
	}

	/* Old (positive) increment, and not final */
	else if (last_inc > 0)
	{
		sprintf(buf, "%s [+%ld] ", pmt, (long) (ABS(last_inc)));
	}

	/* Normal haggle */
	else
	{
		sprintf(buf, "%s ", pmt);
	}


	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Ask until done */
	while (TRUE)
	{
		/* Default */
		strcpy(out_val, "");

		/* Ask the user for a response */
		if (!get_string(buf, out_val, 32))
			return (FALSE);

		/* Skip leading spaces */
		for (p = out_val; *p == ' '; p++) /* loop */ ;

		/* Empty response */
		if (*p == '\0')
		{
			/* Accept current price */
			if (final)
			{
				*poffer = price;
				last_inc = 0L;
				break;
			}

			/* Use previous increment */
			if (allow_inc && last_inc)
			{
				*poffer += last_inc;
				break;
			}
		}

		/* Normal response */
		else
		{
			/* Extract a number */
			i = atol(p);

			/* Handle "incremental" number */
			if ((*p == '+' || *p == '-'))
			{
				/* Allow increments */
				if (allow_inc)
				{
					/* Use the given "increment" */
					*poffer += i;
					last_inc = i;
					break;
				}
			}

			/* Handle normal number */
			else
			{
				/* Use the given "number" */
				*poffer = i;
				last_inc = 0L;
				break;
			}
		}

		/* Warning */
		msg_print("Invalid response.");
		msg_print(NULL);
	}

	/* Success */
	return (TRUE);
}


/*
 * Receive an offer (from the player)
 *
 * Return TRUE if offer is NOT okay
 */
static bool receive_offer(cptr pmt, s32b * poffer, s32b last_offer,
	int factor, s32b price, int final)
{
	/* Haggle till done */
	while (TRUE)
	{
		/* Get a haggle (or cancel) */
		if (!get_haggle(pmt, poffer, price, final))
			return (TRUE);

		/* Acceptable offer */
		if (((*poffer) * factor) >= (last_offer * factor))
			break;

		/* Insult, and check for kicked out */
		if (haggle_insults())
			return (TRUE);

		/* Reject offer (correctly) */
		(*poffer) = last_offer;
	}

	/* Success */
	return (FALSE);
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool purchase_haggle(object_type * o_ptr, s32b * price)
{
	s32b cur_ask, final_ask;
	s32b last_offer, offer;
	s32b x1, x2, x3;
	s32b min_per, max_per;
	int flag, loop_flag, noneed;
	int annoyed = 0, final = FALSE;

	bool cancel = FALSE;

	cptr pmt = "Asking";

	char out_val[160];


	*price = 0;


	/* Extract the starting offer and the final offer */
	cur_ask = price_item(o_ptr, ot_ptr->max_inflate, FALSE);
	final_ask = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* No need to haggle */
	if (noneed || auto_haggle)
	{
		/* No need to haggle */
		if (noneed)
		{
			/* Message summary */
			msg_print("You eventually agree upon the price.");
			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
			msg_print("You quickly agree upon the price.");
			msg_print(NULL);

			/* Apply Sales Tax */
			final_ask += final_ask / 10;
		}

		/* Final price */
		cur_ask = final_ask;

		/* Go to final offer */
		pmt = "Final Offer";
		final = TRUE;
	}


	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* Haggle parameters */
	min_per = ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	last_offer = last_offer * (200 - (int) (ot_ptr->max_inflate)) / 100L;
	if (last_offer <= 0)
		last_offer = 1;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	angtk_eval("angband_store", "haggle_open", NULL); /* TNB */

	/* Haggle until done */
	for (flag = FALSE; !flag;)
	{
		loop_flag = TRUE;

		while (!flag && loop_flag)
		{
#if 1 /* TNB */
			(void) sprintf(out_val, "%s: %ld", pmt, (long)cur_ask);
#else /* TNB */
			(void) sprintf(out_val, "%s :  %ld", pmt, (long) cur_ask);
			put_str(out_val, 1, 0);
#endif /* TNB */
			angtk_eval("angband_store", "price_owner", out_val, NULL); /* TNB */
			cancel =
				receive_offer("What do you offer? ", &offer, last_offer, 1,
				cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer > cur_ask)
			{
				say_comment_6();
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}
		}

		if (!flag)
		{
			x1 = 100 * (offer - last_offer) / (cur_ask - last_offer);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per)
					x1 = max_per;
			}
			x2 = rand_range(x1 - 2, x1 + 2);
			x3 = ((cur_ask - offer) * x2 / 100L) + 1;
			/* don't let the price go up */
			if (x3 < 0)
				x3 = 0;
			cur_ask -= x3;

			/* Too little */
			if (cur_ask < final_ask)
			{
				final = TRUE;
				cur_ask = final_ask;
				pmt = "Final Offer";
				annoyed++;
				if (annoyed > 3)
				{
					(void) (increase_insults());
					cancel = TRUE;
					flag = TRUE;
				}
			}
			else if (offer >= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
				angtk_eval("angband_store", "price_owner", "", NULL); /* TNB */
				(void) sprintf(out_val, "Your last offer: %ld",
					(long) last_offer);
				put_str(out_val, 1, 39);
				angtk_eval("angband_store", "price_character", out_val, NULL); /* TNB */
				say_comment_2(cur_ask, annoyed);
			}
		}
	}

	angtk_eval("angband_store", "haggle_close", NULL); /* TNB */

	/* Cancel */
	if (cancel)
		return (TRUE);

	/* Update bargaining info */
	updatebargain(*price, final_ask);

	/* Do not cancel */
	return (FALSE);
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool sell_haggle(object_type * o_ptr, s32b * price)
{
	s32b purse, cur_ask, final_ask;
	s32b last_offer = 0, offer = 0;
	s32b x1, x2, x3;
	s32b min_per, max_per;
	int flag, loop_flag, noneed;
	int annoyed = 0, final = FALSE;
	bool cancel = FALSE;
	cptr pmt = "Offer";
	char out_val[160];


	*price = 0;


	/* Obtain the starting offer and the final offer */
	cur_ask = price_item(o_ptr, ot_ptr->max_inflate, TRUE);
	final_ask = price_item(o_ptr, ot_ptr->min_inflate, TRUE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* Get the owner's payout limit */
	purse = (s32b) (ot_ptr->max_cost);

	/* No need to haggle */
	if (noneed || auto_haggle || (final_ask >= purse))
	{
		/* No reason to haggle */
		if (final_ask >= purse)
		{
			/* Message */
			msg_print("You instantly agree upon the price.");
			msg_print(NULL);

			/* Offer full purse */
			final_ask = purse;
		}

		/* No need to haggle */
		else if (noneed)
		{
			/* Message */
			msg_print("You eventually agree upon the price.");
			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
			msg_print("You quickly agree upon the price.");
			msg_print(NULL);

			/* Apply Sales Tax */
			final_ask -= final_ask / 10;
		}

		/* Final price */
		cur_ask = final_ask;

		/* Final offer */
		final = TRUE;
		pmt = "Final Offer";
	}

	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* XXX XXX XXX Display commands */

	/* Haggling parameters */
	min_per = ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	last_offer = last_offer * ot_ptr->max_inflate / 100L;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	angtk_eval("angband_store", "haggle_open", NULL); /* TNB */

	/* Haggle */
	for (flag = FALSE; !flag;)
	{
		while (1)
		{
			loop_flag = TRUE;

#if 1 /* TNB */
			(void) sprintf(out_val, "%s: %ld", pmt, (long )cur_ask);
#else /* TNB */
			(void) sprintf(out_val, "%s :  %ld", pmt, (long) cur_ask);
			put_str(out_val, 1, 0);
#endif /* TNB */
			angtk_eval("angband_store", "price_owner", out_val, NULL); /* TNB */

			cancel =
				receive_offer("What price do you ask? ", &offer,
				last_offer, -1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer < cur_ask)
			{
				say_comment_6();
				/* rejected, reset offer for incremental haggling */
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}

			/* Stop */
			if (flag || !loop_flag)
				break;
		}

		if (!flag)
		{
			x1 = 100 * (last_offer - offer) / (last_offer - cur_ask);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per)
					x1 = max_per;
			}
			x2 = rand_range(x1 - 2, x1 + 2);
			x3 = ((offer - cur_ask) * x2 / 100L) + 1;
			/* don't let the price go down */
			if (x3 < 0)
				x3 = 0;
			cur_ask += x3;

			if (cur_ask > final_ask)
			{
				cur_ask = final_ask;
				final = TRUE;
				pmt = "Final Offer";
				annoyed++;
				if (annoyed > 3)
				{
					flag = TRUE;
					(void) (increase_insults());
				}
			}
			else if (offer <= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
				angtk_eval("angband_store", "price_owner", "", NULL); /* TNB */
#if 1 /* TNB */
				(void) sprintf(out_val, "Your last bid: %ld",
					(long) last_offer);
#else /* TNB */
				(void) sprintf(out_val, "Your last bid %ld",
					(long) last_offer);
				put_str(out_val, 1, 39);
#endif /* TNB */
				angtk_eval("angband_store", "price_character", out_val, NULL); /* TNB */
				say_comment_3(cur_ask, annoyed);
			}
		}
	}

	angtk_eval("angband_store", "haggle_close", NULL); /* TNB */

	/* Cancel */
	if (cancel)
		return (TRUE);

	/* Update bargaining info */
	updatebargain(*price, final_ask);

	/* Do not cancel */
	return (FALSE);
}


/*
 * Buy an item from a store 			-RAK-
 */
static void store_purchase(void)
{
	int i, amt, choice;
	int item, item_new;

	s32b price, best;

	object_type forge;
	object_type *j_ptr;

	object_type *o_ptr;

	char o_name[O_NAME_MAX];

	char out_val[160];


	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (cur_store_num == STORE_HOME)
			msg_print("Your home is empty.");
		else
			msg_print("I am currently out of stock.");
		return;
	}


	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > 12)
		i = 12;

	/* Prompt */
	if (cur_store_num == STORE_HOME)
	{
		sprintf(out_val, "Which item do you want to take? ");
	}
	else
	{
		sprintf(out_val, "Which item are you interested in? ");
	}

	/* Get the item number to be bought */
	if (!get_stock(&item, out_val, 0, st_ptr->stock_num - 1	/* i-1 -- TNB */
		))
		return;

	/* Get the actual index */
/*	item = item + store_top; -- TNB */

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Assume the player wants just one of them */
	amt = 1;

	/* Get local object */
	j_ptr = &forge;

	/* Get a copy of the object */
	object_copy(j_ptr, o_ptr);

	/* Recalculate charges for a single wand/rod */
	reduce_charges(j_ptr, j_ptr->number - 1);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
		msg_print("You cannot carry that many different items.");
		return;
	}

	/* Determine the "best" price (per item) */
	best = price_item(j_ptr, ot_ptr->min_inflate, FALSE);

	/* Find out how many the player wants */
	if (o_ptr->number > 1)
	{
		/* Hack -- note cost of "fixed" items */
		if ((cur_store_num != STORE_HOME) && (o_ptr->ident & IDENT_FIXED))
		{
			msg_format("That costs %ld gold per item.", (long) (best));
		}

		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0)
			return;
	}

	/* Get local object */
	j_ptr = &forge;

	/* Get desired object */
	object_copy(j_ptr, o_ptr);

	/*
	 * If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf.
	 */
	reduce_charges(j_ptr, j_ptr->number - amt);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
		msg_print("You cannot carry that many items.");
		return;
	}

	/* Attempt to buy it */
	if (cur_store_num != STORE_HOME)
	{
		/* Fixed price, quick buy */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Assume accept */
			choice = 0;

			/* Go directly to the "best" deal */
			price = (best * j_ptr->number);
		}

		/* Haggle for it */
		else
		{
			/* Describe the object (fully) */
			object_desc_store(o_name, j_ptr, TRUE, 3);

			/* Message */
			msg_format("Buying %s (%c).", o_name, I2A(item));
			msg_print(NULL);

			/* Haggle for a final price */
			choice = purchase_haggle(j_ptr, &price);

			/* Hack -- Got kicked out */
			if (st_ptr->store_open >= turn)
				return;
		}


		/* Player wants it */
		if (choice == 0)
		{
			/* Fix the item price (if "correctly" haggled) */
			if (price == (best * j_ptr->number))
				o_ptr->ident |= (IDENT_FIXED);

			/* Player can afford it */
			if (p_ptr->au >= price)
			{
				/* Say "okay" */
				say_comment_1();

				if (cur_store_num == STORE_BLACK) /* The black market is illegal! */
					chg_virtue(V_JUSTICE, -1);
				if ((o_ptr->tval == TV_BOTTLE) &&
					(cur_store_num != STORE_HOME))
					chg_virtue(V_NATURE, -1);

				/* Be happy */
				decrease_insults();

				/* Spend the money */
				p_ptr->au -= price;

				/* Update the display */
				store_prt_gold();

				/* Hack -- buying an item makes you aware of it */
				object_aware(j_ptr);

				/* Hack -- clear the "fixed" flag from the item */
				j_ptr->ident &= ~(IDENT_FIXED);

				/* Describe the transaction */
				object_desc(o_name, j_ptr, TRUE, 3);

				/* Message */
				msg_format("You bought %s for %ld gold.", o_name,
					(long) price);

				/* Erase the inscription */
				j_ptr->inscription = 0;

				/* Erase the "feeling" */
				j_ptr->feeling = FEEL_NONE;
				/* Give it to the player */
				item_new = inven_carry(j_ptr);

				/* Describe the final result */
				object_desc(o_name, &inventory[item_new], TRUE, 3);

				/* Message */
				msg_format("You have %s (%c).", o_name,
					index_to_label(item_new));

				/* Now, reduce the original stack's pval. */
				if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
				{
					o_ptr->pval -= j_ptr->pval;
				}

				/* Handle stuff */
				handle_stuff();

				/* Note how many slots the store used to have */
				i = st_ptr->stock_num;

				/* Remove the bought items from the store */
				store_item_increase(item, -amt);
				store_item_optimize(item);

				/* Store is empty */
				if (st_ptr->stock_num == 0)
				{
					/* Shuffle */
					if (rand_int(STORE_SHUFFLE) == 0)
					{
						/* Message */
						msg_print("The shopkeeper retires.");

						/* Shuffle the store */
						store_shuffle(cur_store_num);
					}

					/* Maintain */
					else
					{
						/* Message */
						msg_print
							("The shopkeeper brings out some new stock.");
					}

					/* New inventory */
					for (i = 0; i < 10; i++)
					{
						/* Maintain the store */
						store_maint(p_ptr->town_num, cur_store_num);
					}

					/* Start over */
					store_top = 0;

					/* Redraw everything */
					display_inventory();
				}

				/* The item is gone */
				else if (st_ptr->stock_num != i)
				{
					/* Pick the correct screen */
					if (store_top >= st_ptr->stock_num)
						store_top -= 12;

					/* Redraw everything */
					display_inventory();
				}

				/* Item is still here */
				else
				{
					/* Redraw the item */
					display_entry(item);
				}

				angtk_eval("angband_display", "store", "show", NULL);
			}

			/* Player cannot afford it */
			else
			{
				/* Simple message (no insult) */
				msg_print("You do not have enough gold.");
			}
		}
	}

	/* Home is much easier */
	else
	{
		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, j_ptr, amt);

		/* Give it to the player */
		item_new = inven_carry(j_ptr);

		/* Describe just the result */
		object_desc(o_name, &inventory[item_new], TRUE, 3);

		/* Message */
		msg_format("You have %s (%c).", o_name, index_to_label(item_new));

		/* Handle stuff */
		handle_stuff();

		/* Take note if we take the last one */
		i = st_ptr->stock_num;

		/* Remove the items from the home */
		store_item_increase(item, -amt);
		store_item_optimize(item);

		/* Hack -- Item is still here */
		if (i == st_ptr->stock_num)
		{
			/* Redraw the item */
			display_entry(item);
		}

		/* The item is gone */
		else
		{
			/* Nothing left */
			if (st_ptr->stock_num == 0)
				store_top = 0;

			/* Nothing left on that screen */
			else if (store_top >= st_ptr->stock_num)
				store_top -= 12;

			/* Redraw everything */
			display_inventory();

			chg_virtue(V_SACRIFICE, 1);
		}

		angtk_eval("angband_display", "store", "show", NULL); /* TNB */
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an item to the store (or home)
 */
static void store_sell(void)
{
	int choice;
	int item, item_pos;
	int amt;

	s32b price, value, dummy;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr q, s;

	char o_name[O_NAME_MAX];


	/* Prepare a prompt */
	if (cur_store_num == STORE_HOME)
		q = "Drop which item? ";
	else
		q = "Sell which item? ";

	/* Only allow items the store will buy */
	item_tester_hook = store_will_buy;

	/* Get an item */
	s = "You have nothing that I want.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR)))
		return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Hack -- Cannot remove cursed items */
	if ((item >= INVEN_WIELD) && cursed_p(o_ptr))
	{
		/* Oops */
		msg_print("Hmmm, it seems to be cursed.");

		/* Nope */
		return;
	}


	/* Assume one item */
	amt = 1;

	/* Find out how many the player wants (letter means "all") */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0)
			return;
	}

	/* Get local object */
	q_ptr = &forge;

	/* Get a copy of the object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = amt;

	/*
	 * Hack -- If a rod or wand, allocate total maximum
	 * timeouts or charges to those being sold. -LM-
	 */
	if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
	{
		q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

	/* Get a full description */
	object_desc(o_name, q_ptr, TRUE, 3);

	/* Remove any inscription, feeling for stores */
	if (cur_store_num != STORE_HOME)
	{
		q_ptr->inscription = 0;
		q_ptr->feeling = FEEL_NONE;
	}

	/* Is there room in the store (or the home?) */
	if (!store_check_num(q_ptr))
	{
		if (cur_store_num == STORE_HOME)
			msg_print("Your home is full.");
		else
			msg_print("I have not the room in my store to keep it.");
		return;
	}


	/* Real store */
	if (cur_store_num != STORE_HOME)
	{
		/* Describe the transaction */
		msg_format("Selling %s (%c).", o_name, index_to_label(item));
		msg_print(NULL);

		/* Haggle for it */
		choice = sell_haggle(q_ptr, &price);

		/* Kicked out */
		if (st_ptr->store_open >= turn)
			return;

		/* Sold... */
		if (choice == 0)
		{
			/* Say "okay" */
			say_comment_1();

			if (cur_store_num == STORE_BLACK) /* The black market is illegal! */
				chg_virtue(V_JUSTICE, -1);

			if ((o_ptr->tval == TV_BOTTLE) &&
				(cur_store_num != STORE_HOME))
				chg_virtue(V_NATURE, 1);

			/* Be happy */
			decrease_insults();

			/* Get some money */
			p_ptr->au += price;

			/* Update the display */
			store_prt_gold();

			/* Get the "apparent" value */
			dummy = object_value(q_ptr) * q_ptr->number;

			/* Identify it */
			identify_item(o_ptr);

			/* Get local object */
			q_ptr = &forge;

			/* Get a copy of the object */
			object_copy(q_ptr, o_ptr);

			/* Modify quantity */
			q_ptr->number = amt;

			/*
			 * Hack -- If a rod or wand, let the shopkeeper know just 
			 * how many charges he really paid for. -LM-
			 */
			if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
			{
				q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
			}

			/* Get the "actual" value */
			value = object_value(q_ptr) * q_ptr->number;

			/* Get the description all over again */
			object_desc(o_name, q_ptr, TRUE, 3);

			/* Describe the result (in message buffer) */
			msg_format("You sold %s for %ld gold.", o_name, (long) price);

			if (!((o_ptr->tval == TV_FIGURINE) && (value > 0)))
			{
				/* Analyze the prices (and comment verbally) unless a figurine */
				purchase_analyze(price, value, dummy);
			}

			/*
			 * Hack -- Allocate charges between those wands or rods sold 
			 * and retained, unless all are being sold. -LM-
			 */
			distribute_charges(o_ptr, q_ptr, amt);

			/* Reset timeouts of the sold items */
			q_ptr->timeout = 0;

			/* Take the item from the player, describe the result */
			inven_item_increase(item, -amt);
			inven_item_describe(item);
			inven_item_optimize(item);

			/* Handle stuff */
			handle_stuff();

			/* The store gets that (known) item */
			item_pos = store_carry(q_ptr);

			/* Re-display if item is now in store */
			if (item_pos >= 0)
			{
				store_top = (item_pos / 12) * 12;
				display_inventory();

				angtk_eval("angband_display", "store", "show", NULL); /* TNB */
			}
		}
	}

	/* Player is at home */
	else
	{
		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, q_ptr, amt);

		/* Describe */
		msg_format("You drop %s (%c).", o_name, index_to_label(item));

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(q_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			store_top = (item_pos / 12) * 12;
			display_inventory();

			angtk_eval("angband_display", "store", "show", NULL); /* TNB */
		}
	}
}


/*
 * Examine an item in a store			   -JDL-
 */
static void store_examine(void)
{
	int i;
	int item;
	object_type *o_ptr;
	char o_name[O_NAME_MAX];
	char out_val[160];


	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (cur_store_num == STORE_HOME)
			msg_print("Your home is empty.");
		else
			msg_print("I am currently out of stock.");
		return;
	}


	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > 12)
		i = 12;

	/* Prompt */
	sprintf(out_val, "Which item do you want to examine? ");

	/* Get the item number to be examined */
	if (!get_stock(&item, out_val, 0, st_ptr->stock_num	/* i-1 -- TNB */
		))
		return;

	/* Get the actual index */
/*	item = item + store_top; -- TNB */

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Require full knowledge */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
		/* This can only happen in the home */
		msg_print("You have no special knowledge about that item.");
		return;
	}

	/* Description */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Describe */
	msg_format("Examining %s...", o_name);

	/* Describe it fully */
	if (!identify_fully_aux(o_ptr))
		msg_print("You see nothing special.");

	return;
}


#if 1 /* TNB */

extern bool high_level_book(object_type * o_ptr);

/*
 * Let the user choose an object to destroy in the Home.
 */
static void home_destroy(void)
{
	int amt = 1, item, old_number;

	object_type *o_ptr;

	char o_name[O_NAME_MAX];
	char out_val[160];

	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		msg_print("Your home is empty.");
		return;
	}

	/* Get the object number to be bought */
	if (!get_stock(&item, "Destroy which item? ", 0,
			st_ptr->stock_num - 1)) return;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0)
			return;
	}

	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, TRUE, 3);
	o_ptr->number = old_number;

	/* Verify destruction */
	if (!(auto_destroy && (object_value(o_ptr) < 1)))
	{
		sprintf(out_val, "Really destroy %s? ", o_name);
		if (!get_check(out_val))
			return;
	}

	/* Artifacts cannot be destroyed */
	if (artifact_p(o_ptr) || o_ptr->art_name)
	{
		/* Message */
		msg_format("You cannot destroy %s.", o_name);

		/* Don't mark id'ed objects */
		if (!object_known_p(o_ptr))
		{
			if (cursed_p(o_ptr) || broken_p(o_ptr))
				o_ptr->feeling = FEEL_TERRIBLE;
			else
				o_ptr->feeling = FEEL_SPECIAL;

			/* We have "felt" it (again) */
			o_ptr->ident |= (IDENT_SENSE);

			/* Redraw everything */
			angtk_eval("angband_display", "store", "show", NULL); /* TNB */
		}

		/* Done */
		return;
	}

	/* Message */
	msg_format("You destroy %s.", o_name);

	if (high_level_book(o_ptr))
	{
		bool gain_expr = FALSE;

		if (p_ptr->pclass == CLASS_WARRIOR)
		{
			gain_expr = TRUE;
		}
		else if (p_ptr->pclass == CLASS_PALADIN)
		{
			if (p_ptr->realm1 == REALM_LIFE)
			{
				if (o_ptr->tval != TV_LIFE_BOOK)
					gain_expr = TRUE;
			}
			else
			{
				if (o_ptr->tval == TV_LIFE_BOOK)
					gain_expr = TRUE;
			}
		}

		if (gain_expr && (p_ptr->exp < PY_MAX_EXP))
		{
			s32b tester_exp = p_ptr->max_exp / 20;
			if (tester_exp > 10000)
				tester_exp = 10000;
			if (o_ptr->sval < 3)
				tester_exp /= 4;
			if (tester_exp < 1)
				tester_exp = 1;

			msg_print("You feel more experienced.");
			gain_exp(tester_exp * amt);
		}

		if (high_level_book(o_ptr) && o_ptr->tval == TV_LIFE_BOOK)
		{
			chg_virtue(V_UNLIFE, 1);
			chg_virtue(V_VITALITY, -1);
		}
		else if (high_level_book(o_ptr) && o_ptr->tval == TV_DEATH_BOOK)
		{
			chg_virtue(V_UNLIFE, -1);
			chg_virtue(V_VITALITY, 1);
		}

		if (o_ptr->to_a || o_ptr->to_h || o_ptr->to_d)
			chg_virtue(V_ENCHANT, -1);

		if (object_value_real(o_ptr) > 30000)
			chg_virtue(V_SACRIFICE, 2);

		else if (object_value_real(o_ptr) > 10000)
			chg_virtue(V_SACRIFICE, 1);
	}

	if (o_ptr->to_a != 0 || o_ptr->to_d != 0 || o_ptr->to_h != 0)
		chg_virtue(V_HARMONY, 1);

	/* Reduce the charges of rods/wands */
	reduce_charges(o_ptr, amt);

	/* Remove the items from the home */
	store_item_increase(item, -amt);
	store_item_optimize(item);

	/* Redraw everything */
	angtk_eval("angband_display", "store", "show", NULL); /* TNB */
}

#endif /* 1 -- TNB */


/*
 * Hack -- set this to leave the store
 */
static bool leave_store = FALSE;


/*
 * Process a command in a store
 *
 * Note that we must allow the use of a few "special" commands
 * in the stores which are not allowed in the dungeon, and we
 * must disable some commands which are allowed in the dungeon
 * but not in the stores, to prevent chaos.
 */
static void store_process_command(void)
{
#ifdef ALLOW_REPEAT	/* TNB */

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	if (rogue_like_commands && command_cmd == 'l')
	{
		command_cmd = 'x'; /* hack! */
	}

	/* Parse the command */
	switch (command_cmd)
	{
			/* Leave */
		case ESCAPE:
		{
			leave_store = TRUE;
			break;
		}

#if 0 /* TNB */

			/* Browse */
		case ' ':
		{
			if (st_ptr->stock_num <= 12)
			{
				msg_print("Entire inventory is shown.");
			}
			else
			{
				store_top += 12;
				if (store_top >= st_ptr->stock_num)
					store_top = 0;
				display_inventory();
			}
			break;
		}

#endif /* TNB */

			/* Redraw */
		case KTRL('R'):
		{
			do_cmd_redraw();
			display_store();
			break;
		}

			/* Get (purchase) */
		case 'g':
		{
			store_purchase();
			break;
		}

			/* Drop (Sell) */
		case 'd':
		{
			store_sell();
			break;
		}

			/* Examine */
		case 'x':
		{
			store_examine();
			break;
		}

			/* Ignore return */
		case '\r':
		{
			break;
		}

		/*** Inventory Commands ***/

			/* Wear/wield equipment */
		case 'w':
		{
			do_cmd_wield();
			break;
		}

			/* Take off equipment */
		case 't':
		{
			do_cmd_takeoff();
			break;
		}

#if 0

			/* Drop an item */
		case 'd':
		{
			do_cmd_drop();
			break;
		}

#endif

			/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break;
		}

			/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

			/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

			/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
			break;
		}

			/* Hack -- toggle windows */
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}



		/*** Use various objects ***/

			/* Browse a book */
		case 'b':
		{
			do_cmd_browse();
			break;
		}

			/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

			/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}



		/*** Help and Such ***/

			/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

			/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

			/* Character description */
		case 'C':
		{
			do_cmd_character();
			display_store();
			break;
		}


		/*** System Commands ***/

			/* Hack -- User interface */
		case '!':
		{
			(void) Term_user(0);
			break;
		}

			/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

			/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

			/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			break;
		}

			/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			break;
		}

			/* Interact with options */
		case '=':
		{
			do_cmd_options();
			break;
		}

		/*** Misc Commands ***/

			/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

			/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

			/* Repeat level feeling */
		case KTRL('F'):
		{
			do_cmd_feeling();
			break;
		}

			/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

			/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages();
			break;
		}

			/* Check artifacts, uniques etc. */
		case '~':
		case '|':
		{
			do_cmd_knowledge();
			break;
		}

			/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

			/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

#if 1 /* TNB */

			/* Destroy item in the Home */
		case 'K':
		{
			if (cur_store_num != STORE_HOME)
				msg_print("That command only works in the Home.");
			else
				home_destroy();
			break;
		}

#endif /* TNB */

			/* Hack -- Unknown command */
		default:
		{
			msg_print("That command does not work in stores.");
			break;
		}
	}
}


/*
 * Enter a store, and interact with it.
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 */
void do_cmd_store(void)
{
	int which;
	int maintain_num;
	int tmp_chr;
	int i;
	cave_type *c_ptr;


	/* Access the player grid */
	c_ptr = &cave[py][px];

	/* Verify a store */
	if (!((c_ptr->feat >= FEAT_SHOP_HEAD) &&
			(c_ptr->feat <= FEAT_SHOP_TAIL)))
	{
		msg_print("You see no store here.");
		return;
	}

	/* Extract the store code */
	which = (c_ptr->feat - FEAT_SHOP_HEAD);

	/* Hack -- Check the "locked doors" */
	if ((town[p_ptr->town_num].store[which].store_open >= turn) ||
		(ironman_shops))
	{
		msg_print("The doors are locked.");
		return;
	}

	/* Calculate the number of store maintainances since the last visit */
	maintain_num =
		(turn -
		town[p_ptr->town_num].store[which].last_visit) / (10L *
		STORE_TURNS);

	/* Maintain the store max. 10 times */
	if (maintain_num > 10)
		maintain_num = 10;

	if (maintain_num)
	{
		/* Maintain the store */
		for (i = 0; i < maintain_num; i++)
			store_maint(p_ptr->town_num, which);

		/* Save the visit */
		town[p_ptr->town_num].store[which].last_visit = turn;
	}

	angtk_widget_lock(TRUE); /* TNB */

	/* Forget the lite */
	forget_lite();

	/* Forget the view */
	forget_view();


	/* Hack -- Character is in "icky" mode */
	character_icky = TRUE;


	/* No command argument */
	command_arg = 0;

	/* No repeated command */
	command_rep = 0;

	/* No automatic command */
	command_new = 0;


	/* Save the store number */
	cur_store_num = which;

	/* Save the store and owner pointers */
	st_ptr = &town[p_ptr->town_num].store[cur_store_num];
	ot_ptr = &owners[cur_store_num][st_ptr->owner];

#if 1 /* TNB */

	/* Information used by Tcl "store" command */
	storedata.shopping = TRUE;
	storedata.store_num = which;
	storedata.st_ptr = st_ptr;
	storedata.ot_ptr = ot_ptr;

#endif /* TNB */

	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store();

	/* Do not leave */
	leave_store = FALSE;

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Hack -- Check the charisma */
		tmp_chr = p_ptr->stat_use[A_CHR];

		/* Clear */
		clear_from(21);


		/* Basic commands */
		prt(" ESC) Exit from Building.", 22, 0);

		/* Browse if necessary */
		if (st_ptr->stock_num > 12)
		{
			prt(" SPACE) Next page of stock", 23, 0);
		}

		/* Home commands */
		if (cur_store_num == STORE_HOME)
		{
			prt(" g) Get an item.", 22, 31);
			prt(" d) Drop an item.", 23, 31);
		}

		/* Shop commands XXX XXX XXX */
		else
		{
			prt(" p) Purchase an item.", 22, 31);
			prt(" s) Sell an item.", 23, 31);
		}

		/* Add in the eXamine option */
		prt(" x) eXamine an item.", 22, 56);

		/* Prompt */
		prt("You may: ", 21, 0);

		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		store_process_command();

		/* Hack -- Character is still in "icky" mode */
		character_icky = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/* XXX XXX XXX Pack Overflow */
		if (inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			object_type *o_ptr = &inventory[item];

			/* Hack -- Flee from the store */
			if (cur_store_num != STORE_HOME)
			{
				/* Message */
				msg_print
					("Your pack is so full that you flee the store...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if (!store_check_num(o_ptr))
			{
				/* Message */
				msg_print
					("Your pack is so full that you flee your home...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type forge;
				object_type *q_ptr;

				char o_name[O_NAME_MAX];

				/* Give a message */
				msg_print("Your pack overflows!");

				/* Get local object */
				q_ptr = &forge;

				/* Grab a copy of the item */
				object_copy(q_ptr, o_ptr);

				/* Describe it */
				object_desc(o_name, q_ptr, TRUE, 3);

				/* Message */
				msg_format("You drop %s (%c).", o_name,
					index_to_label(item));

				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

				/* Let the home carry it */
				item_pos = home_carry(q_ptr);

				/* Redraw the home */
				if (item_pos >= 0)
				{
					store_top = (item_pos / 12) * 12;
					display_inventory();
				}
			}
		}

		/* Hack -- Redisplay store prices if charisma changes */
		if (tmp_chr != p_ptr->stat_use[A_CHR])
			display_inventory();

		/* Hack -- get kicked out of the store */
		if (st_ptr->store_open >= turn)
			leave_store = TRUE;
	}


	/* Free turn XXX XXX XXX */
	energy_use = 0;


	/* Hack -- Character is no longer in "icky" mode */
	character_icky = FALSE;


	/* Hack -- Cancel automatic command */
	command_new = 0;

	/* Hack -- Cancel "see" mode */
	command_see = FALSE;


	/* Flush messages XXX XXX XXX */
	msg_print(NULL);


	/* Clear the screen */
	Term_clear();


	/* Update everything */
	p_ptr->update |= (PU_VIEW | PU_LITE);
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

#if 1 /* TNB */

	storedata.shopping = FALSE;
	angtk_eval("angband_display", "store", "hide", NULL);

	angtk_widget_lock(FALSE);

#endif /* TNB */
}



/*
 * Shuffle one of the stores.
 */
void store_shuffle(int which)
{
	int i, j;


	/* Ignore home */
	if (which == STORE_HOME)
		return;


	/* Save the store index */
	cur_store_num = which;

	/* Activate that store */
	st_ptr = &town[p_ptr->town_num].store[cur_store_num];

	/* Pick a new owner */
	for (j = st_ptr->owner; j == st_ptr->owner;)
	{
		st_ptr->owner = (byte) rand_int(MAX_OWNERS);
	}

	/* Activate the new owner */
	ot_ptr = &owners[cur_store_num][st_ptr->owner];


	/* Reset the owner data */
	st_ptr->insult_cur = 0;
	st_ptr->store_open = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;


	/* Hack -- discount all the items */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		object_type *o_ptr;

		/* Get the item */
		o_ptr = &st_ptr->stock[i];

		/* Hack -- Sell all old items for "half price" */
		if (!(o_ptr->art_name))
			o_ptr->discount = 50;

		/* Hack -- Items are no longer "fixed price" */
		o_ptr->ident &= ~(IDENT_FIXED);

		/* Mega-Hack -- Note that the item is "on sale" */
		o_ptr->inscription = quark_add("on sale");
	}
}


/*
 * Maintain the inventory at the stores.
 */
void store_maint(int town_num, int store_num)
{
	int j;

	int old_rating = rating;

	cur_store_num = store_num;

	/* Ignore home */
	if (store_num == STORE_HOME)
		return;

	/* Activate that store */
	st_ptr = &town[town_num].store[store_num];

	/* Activate the owner */
	ot_ptr = &owners[store_num][st_ptr->owner];

	/* Store keeper forgives the player */
	st_ptr->insult_cur = 0;

	/* Mega-Hack -- prune the black market */
	if (store_num == STORE_BLACK)
	{
		/* Destroy crappy black market items */
		for (j = st_ptr->stock_num - 1; j >= 0; j--)
		{
			object_type *o_ptr = &st_ptr->stock[j];

			/* Destroy crappy items */
			if (black_market_crap(o_ptr))
			{
				/* Destroy the item */
				store_item_increase(j, 0 - o_ptr->number);
				store_item_optimize(j);
			}
		}
	}


	/* Choose the number of slots to keep */
	j = st_ptr->stock_num;

	/* Sell a few items */
	j = j - randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP)
		j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP)
		j = STORE_MIN_KEEP;

	/* Hack -- prevent "underflow" */
	if (j < 0)
		j = 0;

	/* Destroy objects until only "j" slots are left */
	while (st_ptr->stock_num > j)
		store_delete();


	/* Choose the number of slots to fill */
	j = st_ptr->stock_num;

	/* Buy some more items */
	j = j + randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP)
		j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP)
		j = STORE_MIN_KEEP;

	/* Hack -- prevent "overflow" */
	if (j >= st_ptr->stock_size)
		j = st_ptr->stock_size - 1;

	/* Acquire some new items */
	while (st_ptr->stock_num < j)
		store_create();


	/* Hack -- Restore the rating */
	rating = old_rating;
}


/*
 * Initialize the stores
 */
void store_init(int town_num, int store_num)
{
	int k;

	cur_store_num = store_num;

	/* Activate that store */
	st_ptr = &town[town_num].store[store_num];


	/* Pick an owner */
	st_ptr->owner = (byte) rand_int(MAX_OWNERS);

	/* Activate the new owner */
	ot_ptr = &owners[store_num][st_ptr->owner];


	/* Initialize the store */
	st_ptr->store_open = 0;
	st_ptr->insult_cur = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* Nothing in stock */
	st_ptr->stock_num = 0;

	/*
	 * MEGA-HACK - Last visit to store is
	 * BEFORE player birth to enable store restocking
	 */
	st_ptr->last_visit = -100L * STORE_TURNS;

	/* Clear any old items */
	for (k = 0; k < st_ptr->stock_size; k++)
	{
		object_wipe(&st_ptr->stock[k]);
	}
}


void move_to_black_market(object_type * o_ptr)
{
	/* Not in town */
	if (!p_ptr->town_num)
		return;

	st_ptr = &town[p_ptr->town_num].store[STORE_BLACK];

	o_ptr->ident |= IDENT_STOREB;

	(void) store_carry(o_ptr);
	object_wipe(o_ptr);	/* Don't leave a bogus object behind... */
}

#if 1 /* TNB */

char *store_cost(object_type *o_ptr)
{
	s32b x;

	/* No cost in home */
	if (storedata.store_num == STORE_HOME)
	{
		/* Sanity: zero cost */
		return "0";
	}

	/* Display a "fixed" cost */
	else if (o_ptr->ident & (IDENT_FIXED))
	{
		/* Extract the "minimum" price */
		x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

		/* Set the price (fixed) */
		return format("%ld F", (long) x);
	}

	/* Display a "taxed" cost */
	else if (auto_haggle)
	{
		/* Extract the "minimum" price */
		x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

		/* Hack -- Apply Sales Tax if needed */
		if (!noneedtobargain(x))
			x += x / 10;

		/* Set the price (with tax) */
		return format("%ld", (long) x);
	}

	/* Display a "haggle" cost */
	else
	{
		/* Extrect the "maximum" price */
		x = price_item(o_ptr, ot_ptr->max_inflate, FALSE);

		/* Set the price (not fixed) */
		return format("%ld", (long) x);
	}
}

#endif /* TNB */
