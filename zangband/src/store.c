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


#define MAX_COMMENT_1	6

static cptr comment_1[MAX_COMMENT_1] =
{
	"Okay.",
	"Fine.",
	"Accepted!",
	"Agreed!",
	"Done!",
	"Taken!"
};

/*
 * Successful haggle.
 */
static void say_comment_1(void)
{
	msg_print(comment_1[randint0(MAX_COMMENT_1)]);
}


/*
 * Messages for reacting to purchase prices.
 */

#define MAX_COMMENT_7A	4

static cptr comment_7a[MAX_COMMENT_7A] =
{
	"Arrgghh!",
	"You bastard!",
	"You hear someone sobbing...",
	"The shopkeeper howls in agony!"
};

#define MAX_COMMENT_7B	4

static cptr comment_7b[MAX_COMMENT_7B] =
{
	"Damn!",
	"You fiend!",
	"The shopkeeper curses at you.",
	"The shopkeeper glares at you."
};

#define MAX_COMMENT_7C	4

static cptr comment_7c[MAX_COMMENT_7C] =
{
	"Cool!",
	"You've made my day!",
	"The shopkeeper giggles.",
	"The shopkeeper laughs loudly."
};

#define MAX_COMMENT_7D	4

static cptr comment_7d[MAX_COMMENT_7D] =
{
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
		message(MSG_STORE1, 0, comment_7a[randint0(MAX_COMMENT_7A)]);

		chg_virtue(V_HONOUR, -1);
		chg_virtue(V_JUSTICE, -1);

		/* Sound */
		sound(SOUND_STORE1);
	}

	/* Item was cheaper than we thought, and we paid more than necessary */
	else if ((value < guess) && (price > value))
	{
		/* Comment */
		message(MSG_STORE2, 0, comment_7b[randint0(MAX_COMMENT_7B)]);

		chg_virtue(V_JUSTICE, -1);
		if (one_in_(4)) chg_virtue(V_HONOUR, -1);

		/* Sound */
		sound(SOUND_STORE2);
	}

	/* Item was a good bargain, and we got away with it */
	else if ((value > guess) && (value < (4 * guess)) && (price < value))
	{
		/* Comment */
		message(MSG_STORE3, 0, comment_7c[randint0(MAX_COMMENT_7C)]);

		if (one_in_(4))
			chg_virtue(V_HONOUR, -1);
		else if (one_in_(4))
			chg_virtue(V_HONOUR, 1);

		/* Sound */
		sound(SOUND_STORE3);
	}

	/* Item was a great bargain, and we got away with it */
	else if ((value > guess) && (price < value))
	{
		/* Comment */
		message(MSG_STORE4, 0, comment_7d[randint0(MAX_COMMENT_7D)]);

		if (one_in_(2))
			chg_virtue(V_HONOUR, -1);
		if (one_in_(4))
			chg_virtue(V_HONOUR, 1);

		if (10 * price < value)
			chg_virtue(V_SACRIFICE, 1);

		/* Sound */
		sound(SOUND_STORE4);
	}
}


/*
 * We store the current "store pointer" here so everyone can access it
 */
static store_type *st_ptr = NULL;

/*
 * We store the current field here so that it can be accessed everywhere
 */
static field_type *f_ptr = NULL;

/* Save info flags for store */
static byte info_flags;



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
static s32b price_item(object_type *o_ptr, int greed, bool flip)
{
	int factor;
	int adjust;
	s32b price;

	const owner_type *ot_ptr = &owners[f_ptr->data[0]][st_ptr->owner];

	/* Get the value of one of the items */
	price = object_value(o_ptr);

	/* Worthless items */
	if (price <= 0) return (0L);


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
		if (adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if ((info_flags & ST_GREED) || (info_flags & ST_ULTRA_GREED))
		{
			price = price / 2;
		}
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if (adjust < 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (info_flags & ST_GREED)
			price = price * 2;

		if (info_flags & ST_ULTRA_GREED)
			price = price * 4;
	}

	/* Compute the final price (with rounding) */
	price = (price * adjust + 50L) / 100L;

	/* Note -- Never become "free" */
	if (price <= 0L) return (1L);

	/* Return the price */
	return (price);
}


/*
 * Certain "cheap" objects should be created in "piles"
 * Some objects can be sold at a "discount" (in small piles)
 */
static void mass_produce(object_type *o_ptr)
{
	int size = 1;
	int discount = 0;

	s32b cost = object_value(o_ptr);

	/* Analyze the type */
	switch (o_ptr->tval)
	{
		case TV_FOOD:
		case TV_FLASK:
		case TV_LITE:
		{
			/* Food, Flasks, and Lites */
			if (cost <= 5L) size += damroll(3, 5);
			if (cost <= 20L) size += damroll(3, 5);
			break;
		}

		case TV_POTION:
		case TV_SCROLL:
		{
			if (cost <= 60L) size += damroll(3, 5);
			if (cost <= 240L) size += damroll(1, 5);
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
			if (cost <= 50L) size += damroll(2, 3);
			if (cost <= 500L) size += damroll(1, 3);
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
			if (o_ptr->xtra_name) break;
			if (cost <= 10L) size += damroll(3, 5);
			if (cost <= 100L) size += damroll(3, 5);
			break;
		}

		case TV_SPIKE:
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (cost <= 5L) size += damroll(5, 5);
			if (cost <= 50L) size += damroll(5, 5);
			if (cost <= 500L) size += damroll(5, 5);
			break;
		}

		case TV_FIGURINE:
		case TV_STATUE:
		{
			if (cost <= 100L) size += damroll(2, 2);
			if (cost <= 1000L) size += damroll(2, 2);
			break;
		}

		case TV_ROD:
		case TV_WAND:
		case TV_STAFF:
		{
			if (one_in_(3))
			{
				if (cost < 1601L) size += damroll(1, 5);
				else if (cost < 3201L) size += damroll(1, 3);
			}

			/* 
			 * Ensure that mass-produced rods and wands
			 * get the correct pvals.
			 */
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
	else if (one_in_(25))
	{
		discount = 25;
	}
	else if (one_in_(150))
	{
		discount = 50;
	}
	else if (one_in_(300))
	{
		discount = 75;
	}
	else if (one_in_(500))
	{
		discount = 90;
	}


	if (o_ptr->xtra_name)
	{
		if (cheat_peek && discount)
		{
			msg_print("No discount on powerful items.");
		}
		discount = 0;
	}

	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = size - (size * discount / 100);
}



/*
 * Determine if a store item can "absorb" another item
 *
 * See "object_similar()" for the same function for the "player"
 */
static bool store_object_similar(object_type *o_ptr, object_type *j_ptr)
{
	/* Hack -- Identical items cannot be stacked */
	if (o_ptr == j_ptr) return (FALSE);

	/* Different objects cannot be stacked */
	if (o_ptr->k_idx != j_ptr->k_idx) return (FALSE);

	/* Different charges (etc) cannot be stacked, unless wands or rods. */
	if ((o_ptr->pval != j_ptr->pval) &&
		(o_ptr->tval != TV_WAND) && (o_ptr->tval != TV_ROD)) return (FALSE);

	/* Require many identical values */
	if (o_ptr->to_h != j_ptr->to_h) return (FALSE);
	if (o_ptr->to_d != j_ptr->to_d) return (FALSE);
	if (o_ptr->to_a != j_ptr->to_a) return (FALSE);

	/* Artifacts and ego items don't stack ! */
	if (o_ptr->xtra_name || j_ptr->xtra_name) return (FALSE);

	/* Hack -- Identical flags! */
	if ((o_ptr->flags1 != j_ptr->flags1) ||
		(o_ptr->flags2 != j_ptr->flags2) || (o_ptr->flags3 != j_ptr->flags3))
		return (FALSE);

	/* Require identical recharge times / fuel level */
	if (o_ptr->timeout != j_ptr->timeout) return (FALSE);

	/* Require many identical values */
	if (o_ptr->ac != j_ptr->ac) return (FALSE);
	if (o_ptr->dd != j_ptr->dd) return (FALSE);
	if (o_ptr->ds != j_ptr->ds) return (FALSE);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST) return (FALSE);

	/* Require matching discounts */
	if (o_ptr->discount != j_ptr->discount) return (FALSE);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store item to absorb another item
 */
static void store_object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > 99) ? 99 : total;

	/* 
	 * Hack -- if rods are stacking, add the pvals
	 * (maximum timeouts) together. -LM-
	 */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval += j_ptr->pval;
	}

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if (o_ptr->tval == TV_WAND)
	{
		o_ptr->pval += j_ptr->pval;

		/* No "used" charges in store stock */
		o_ptr->ac = 0;
	}
}


/*
 * Check to see if the shop will be carrying too many objects
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.	Before, one could "nuke" potions this way.
 */
static bool store_check_num(object_type *o_ptr)
{
	object_type *j_ptr;

	/* Free space is always usable */
	if (get_list_length(st_ptr->stock) < st_ptr->max_stock) return TRUE;

	/* The "home" acts like the player */
	if (st_ptr->type == BUILD_STORE_HOME)
	{
		/* Check all the items */
		OBJ_ITT_START(st_ptr->stock, j_ptr)
		{
			/* Can the new object be combined with the old one? */
			if (object_similar(j_ptr, o_ptr)) return (TRUE);
		}
		OBJ_ITT_END;
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the items */
		OBJ_ITT_START(st_ptr->stock, j_ptr)
		{
			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr)) return (TRUE);
		}
		OBJ_ITT_END;
	}

	/* But there was no room at the inn... */
	return (FALSE);
}


/*
 * Determine if the current store will purchase the given item
 * (Check restriction flags, and object theme)
 */
static bool store_will_buy(const object_type *o_ptr)
{
	obj_theme theme;

	/* Check restriction flags */

	/* Blessed items only */
	if (info_flags & ST_REST_BLESSED)
	{
		if (!item_tester_hook_is_blessed(o_ptr)) return (FALSE);
	}

	/* Good items only */
	if ((info_flags & ST_REST_GOOD) || (info_flags & ST_REST_GREAT))
	{
		if (!item_tester_hook_is_good(o_ptr)) return (FALSE);
	}

	/* Ignore "worthless" items XXX XXX XXX */
	if (object_value(o_ptr) <= 0) return (FALSE);

	/* Final stage, check the theme */

	/* Set theme */
	theme.treasure = f_ptr->data[3];
	theme.combat = f_ptr->data[4];
	theme.magic = f_ptr->data[5];
	theme.tools = f_ptr->data[6];

	/* Initialise the theme tester */
	init_match_theme(theme);

	/*
	 * Final check: 
	 * Does the object have a chance of being made?
	 */
	return (kind_is_theme(o_ptr->k_idx));
}


/*
 * The player wants to sell something to the store.
 * This is a much more restrictive case than the store_will_buy()
 * function below.
 * Only objects that pass the field hooks will be accepted.
 * (As well as only selecting store_will_buy() objects.
 *
 * Two field action functions are called for a store.
 * The first checks to see if a store will not buy something.
 * The second checks the opposite.
 * By combining different action functions, lots of different
 * types of store can be made.
 */
static bool store_will_stock(const object_type *o_ptr)
{
	/* Thing to pass to the action functions */
	field_obj_test f_o_t;

	/*
	 * Save information to pass to the field action function
	 *
	 * Hack - remove const specifier...
	 * (store_will_stock needs const due to the item_hook
	 * which is really annoying.)
	 */
	f_o_t.o_ptr = (object_type *)o_ptr;

	/* Default is to reject this rejection */
	f_o_t.result = FALSE;

	/* Will the store !not! buy this item? */
	field_hook(&area(p_ptr->px, p_ptr->py)->fld_idx,
			   FIELD_ACT_STORE_ACT1, (vptr)&f_o_t);

	/* We don't want this item type? */
	if (f_o_t.result == TRUE) return (FALSE);

	/* Change the default to acceptance */
	f_o_t.result = TRUE;

	/* Will the store buy this item? */
	field_hook(&area(p_ptr->px, p_ptr->py)->fld_idx,
			   FIELD_ACT_STORE_ACT2, (vptr)&f_o_t);

	/* Finally check to see if we will buy the item */
	return (f_o_t.result && store_will_buy(o_ptr));
}


/*
 * Compare two items to see if they are in store-order.
 */
static bool reorder_store_comp(object_type *o1_ptr, object_type *o2_ptr)
{
	/* Hack -- readable books always come first */
	if ((o1_ptr->tval == mp_ptr->spell_book) &&
		(o2_ptr->tval != mp_ptr->spell_book)) return (TRUE);
	if ((o1_ptr->tval == mp_ptr->spell_book) &&
		(o2_ptr->tval != mp_ptr->spell_book)) return (FALSE);

	/* Objects sort by decreasing type */
	if (o1_ptr->tval > o2_ptr->tval) return (TRUE);
	if (o1_ptr->tval < o2_ptr->tval) return (FALSE);

	/* Can happen in the home */
	if (!object_aware_p(o2_ptr)) return (TRUE);
	if (!object_aware_p(o1_ptr)) return (FALSE);

	/* Objects sort by increasing sval */
	if (o1_ptr->sval < o2_ptr->sval) return (TRUE);
	if (o1_ptr->sval > o2_ptr->sval) return (FALSE);

	/* Objects in the home can be unknown */
	if (!object_known_p(o2_ptr)) return (TRUE);
	if (!object_known_p(o1_ptr)) return (FALSE);

	/* Objects sort by decreasing value */
	if (object_value(o1_ptr) > object_value(o2_ptr)) return (TRUE);
	
	return (FALSE);
}


/*
 * Add the item "o_ptr" to the inventory of the "Home"
 *
 * In all cases, return the slot (or -1) where the object was placed
 *
 * Note that this is a hacked up version of "inven_carry()".
 *
 * Also note that it may not correctly "adapt" to "knowledge" becoming
 * known, the player may have to pick stuff up and drop it again.
 */
static object_type *home_carry(object_type *o_ptr)
{
	object_type *j_ptr;


	/* Check each existing item (try to combine) */
	OBJ_ITT_START(st_ptr->stock, j_ptr)
	{
		/* The home acts just like the player */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, o_ptr);

			/* All done */
			return (j_ptr);
		}
	}
	OBJ_ITT_END;

	/* No space? */
	if (get_list_length(st_ptr->stock) >= st_ptr->max_stock) return (NULL);

	/* Add the item to the store */
	o_ptr = add_object_list(&st_ptr->stock, o_ptr);
	
	/* Forget location */
	o_ptr->iy = o_ptr->ix = 0;

	/* Forget Region */
	o_ptr->region = 0;

	/* No longer marked */
	o_ptr->info &= ~(OB_SEEN);

	/* Reorder the items */
	o_ptr = reorder_objects_aux(o_ptr, reorder_store_comp, st_ptr->stock);

	chg_virtue(V_SACRIFICE, -1);

	/* Return the location */
	return (o_ptr);
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
 * In all cases, return the slot (or NULL) where the object was placed
 */
static object_type *store_carry(object_type *o_ptr)
{
	object_type *j_ptr;

	/* Evaluate the object */
	s32b value = object_value(o_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	if (value <= 0) return (NULL);

	/* Identify it fully */
	object_known(o_ptr);

	/* Save all the known flags */
	o_ptr->kn_flags1 = o_ptr->flags1;
	o_ptr->kn_flags2 = o_ptr->flags2;
	o_ptr->kn_flags3 = o_ptr->flags3;

	/* Erase the inscription */
	o_ptr->inscription = 0;

	/* Erase the "feeling" */
	o_ptr->feeling = FEEL_NONE;

	/* Check each existing item (try to combine) */
	OBJ_ITT_START(st_ptr->stock, j_ptr)
	{
		/* Can the existing items be incremented? */
		if (store_object_similar(j_ptr, o_ptr))
		{
			/* Hack -- extra items disappear */
			store_object_absorb(j_ptr, o_ptr);

			/* All done */
			return (j_ptr);
		}
	}
	OBJ_ITT_END;

	/* No space? */
	if (get_list_length(st_ptr->stock) >= st_ptr->max_stock) return (NULL);
	
	/* Add the item to the store */
	o_ptr = add_object_list(&st_ptr->stock, o_ptr);
	
	/* Forget location */
	o_ptr->iy = o_ptr->ix = 0;

	/* Forget Region */
	o_ptr->region = 0;

	/* No longer marked */
	o_ptr->info &= ~(OB_SEEN);

	/* Reorder the items */
	o_ptr = reorder_objects_aux(o_ptr, reorder_store_comp, st_ptr->stock);

	/* Return the location */
	return (o_ptr);
}


/*
 * Attempt to delete (some of) a random item from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
static void store_delete(void)
{
	int what, num;
	object_type *o_ptr;

	/* Pick a random slot */
	what = randint0(get_list_length(st_ptr->stock));
	
	/* Get the item */
	o_ptr = get_list_item(st_ptr->stock, what);

	/* Determine how many items are here */
	num = o_ptr->number;

	/* Hack -- sometimes, only destroy half the items */
	if (one_in_(2)) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single item */
	if (one_in_(2)) num = 1;

	/* 
	 * Hack -- decrement the maximum timeouts and
	 * total charges of rods and wands. -LM-
	 */
	if ((o_ptr->tval == TV_ROD) ||
		(o_ptr->tval == TV_WAND))
	{
		o_ptr->pval -= num * o_ptr->pval / o_ptr->number;
	}

	/* Actually destroy (part of) the item */
	item_increase(o_ptr, -num);
}


/*
 * Creates a random item and gives it to a store
 */
static void store_create(void)
{
	int kind, tries, level;

	object_type *q_ptr;

	obj_theme theme;

	byte restricted = f_ptr->data[7];

	/* Paranoia -- no room left */
	if (get_list_length(st_ptr->stock) >= st_ptr->max_stock) return;

	/* Set theme */
	theme.treasure = f_ptr->data[3];
	theme.combat = f_ptr->data[4];
	theme.magic = f_ptr->data[5];
	theme.tools = f_ptr->data[6];

	/* Select items based on "theme" */
	init_match_theme(theme);

	/* Activate restriction */
	get_obj_num_hook = kind_is_theme;

	/* Prepare allocation table */
	get_obj_num_prep();

	/* Limit table with store-only items */
	get_obj_store_prep();

	/* Hack -- consider up to fifty items */
	for (tries = 0; tries < 50; tries++)
	{
		/* Get level to use */
		level = rand_range(f_ptr->data[1], f_ptr->data[2]);

		/* Get an item */
		kind = get_obj_num(level, 0);

		/* Handle failure */
		if (!kind) continue;

		/* Create a new object of the chosen kind */
		q_ptr = object_prep(kind);

		/* Create object based on restrictions */
		if (restricted & ST_REST_GREAT)
		{
			/* Apply some "low-level" magic (great) */
			apply_magic(q_ptr, level, 30, OC_FORCE_GOOD);
		}
		else if (restricted & ST_REST_GOOD)
		{
			/* Apply some "low-level" magic (good) */
			apply_magic(q_ptr, level, 15, OC_NONE);
		}
		else
		{
			/* Apply some "low-level" magic (normal) */
			apply_magic(q_ptr, level, 0, OC_NONE);
		}

		/* Mega-Hack -- no chests in stores */
		if (q_ptr->tval == TV_CHEST) continue;

		/* The item is "known" */
		object_known(q_ptr);

		/* Mark it storebought */
		q_ptr->info |= OB_STOREB;

		/* Require valid object */
		if (!store_will_stock(q_ptr)) continue;

		/* Hack -- Charge lite's */
		if (q_ptr->tval == TV_LITE)
		{
			if (q_ptr->sval == SV_LITE_TORCH) q_ptr->timeout = FUEL_TORCH / 2;
			if (q_ptr->sval == SV_LITE_LANTERN) q_ptr->timeout = FUEL_LAMP / 2;
		}

		/* Mass produce and/or Apply discount */
		mass_produce(q_ptr);

		/* Attempt to carry the (known) item */
		(void)store_carry(q_ptr);

		/* Definitely done */
		break;
	}

	/* Clear restriction */
	get_obj_num_hook = NULL;
}

/*
 * Re-displays a single store entry
 */
static void display_entry(int pos)
{
	int i;
	
	/* Get the object */
	object_type *o_ptr = get_list_item(st_ptr->stock, pos);
	
	s32b x;

	char o_name[256];
	char out_val[160];

	byte a;
	char c;

	int maxwid;

	const owner_type *ot_ptr = &owners[f_ptr->data[0]][st_ptr->owner];

	int wid, hgt;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Get the "offset" */
	i = (pos % 12);

	/* Label it, clear the line --(-- */
	(void)sprintf(out_val, "%c) ", I2A(i));
	prt(out_val, 0, i + 6);

	/* Show_store_graph perm on. */
	a = object_attr(o_ptr);
	c = object_char(o_ptr);

#ifdef AMIGA
	if (a & 0x80) a |= 0x40;
#endif

	/* Hack -- fake monochrome */
	if (!use_color || ironman_moria) a = TERM_WHITE;

	Term_draw(3, i + 6, a, c);

	/* Describe an item in the home */
	if (st_ptr->type == BUILD_STORE_HOME)
	{
		maxwid = wid - 4;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 10;

		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3, maxwid);
		c_put_str(tval_to_attr[o_ptr->tval], o_name, 5, i + 6);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
			(void)sprintf(out_val, "%3d.%d lb", wgt / 10, wgt % 10);
			put_str(out_val, wid - 12, i + 6);
		}
	}

	/* Describe an item (fully) in a store */
	else
	{
		/* Must leave room for the "price" */
		maxwid = wid - 14;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 7;

		/* Describe the object (fully) */
		object_desc_store(o_name, o_ptr, TRUE, 3, maxwid);
		c_put_str(tval_to_attr[o_ptr->tval], o_name, 5, i + 6);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
			(void)sprintf(out_val, "%3d.%d", wgt / 10, wgt % 10);
			put_str(out_val, wid - 19, i + 6);
		}

		/* Extract the "minimum" price */
		x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

		/* Actually draw the price (with tax) */
		(void)sprintf(out_val, "%9ld  ", (long)x);
		put_str(out_val, wid - 12, i + 6);

	}
}


/*
 * Displays a store's inventory
 * All prices are listed as "per individual object".  -BEN-
 */
static void display_inventory(int store_top)
{
	int i, k;
	
	int stocknum = get_list_length(st_ptr->stock);
	
	/* Display the next 12 items */
	for (k = 0; k < 12; k++)
	{
		/* Do not display "dead" items */
		if (store_top + k >= stocknum) break;

		/* Display that line */
		display_entry(store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < 13; i++) prt("", 0, i + 6);

	/* Assume "no current page" */
	put_str("        ", 20, 5);

	/* Visual reminder of "more items" */
	if (stocknum > 12)
	{
		/* Show "more" reminder (after the last item) */
		prt("-more-", 3, k + 6);

		/* Indicate the "current page" */
		put_str(format("(Page %d)", store_top / 12 + 1), 20, 5);
	}
}


/*
 * Displays players gold					-RAK-
 */
static void store_prt_gold(void)
{
	char out_val[64];

	prt("Gold Remaining: ", 53, 19);

	sprintf(out_val, "%9ld", (long)p_ptr->au);
	prt(out_val, 68, 19);
}


/*
 * Displays store (after clearing screen)		-RAK-
 */
static void display_store(int store_top)
{
	char buf[80];

	const owner_type *ot_ptr = &owners[f_ptr->data[0]][st_ptr->owner];

	/* Clear screen */
	Term_clear();

	/* The "Home" is special */
	if (st_ptr->type == BUILD_STORE_HOME)
	{
		/* Put the owner name */
		put_str("Your Home", 30, 3);

		/* Label the item descriptions */
		put_str("Item Description", 3, 5);

		/* If showing weights, show label */
		if (show_weights)
		{
			put_str("Weight", 70, 5);
		}
	}

	/* Normal stores */
	else
	{
		cptr store_name = t_info[f_ptr->t_idx].name;
		cptr owner_name = ot_ptr->owner_name;
		cptr race_name = race_info[ot_ptr->owner_race].title;

		/* Put the owner name and race */
		sprintf(buf, "%s (%s)", owner_name, race_name);
		put_str(buf, 5, 3);

		/* Show the max price in the store (above prices) */
		sprintf(buf, "%s (%ld)", store_name, (long)(ot_ptr->max_cost) * 100);
		prt(buf, 45, 3);

		/* Label the item descriptions */
		put_str("Item Description", 3, 5);

		/* If showing weights, show label */
		if (show_weights)
		{
			put_str("Weight", 60, 5);
		}

		/* Label the asking price (in stores) */
		put_str("Price", 72, 5);
	}

	/* Display the current gold */
	store_prt_gold();

	/* Draw in the inventory */
	display_inventory(store_top);
}


/*
 * Maintain the inventory at the stores.
 */
static void store_maint(void)
{
	int i = 0, j;

	int old_rating = dun_ptr->rating;

	/* Ignore home + locker */
	if (st_ptr->type == BUILD_STORE_HOME) return;

	/* Choose the number of slots to keep */
	j = get_list_length(st_ptr->stock);

	/* Sell a few items */
	j = j - randint1(STORE_TURNOVER);

	if (st_ptr->max_stock == STORE_INVEN_MAX)
	{
		/* Never keep more than "STORE_MAX_KEEP" slots */
		if (j > STORE_MAX_KEEP1) j = STORE_MAX_KEEP1;

		/* Always "keep" at least "STORE_MIN_KEEP" items */
		if (j < STORE_MIN_KEEP1) j = STORE_MIN_KEEP1;
	}
	else
	{
		/* The store has half the normal inventory space */

		/* Never keep more than "STORE_MAX_KEEP" slots */
		if (j > STORE_MAX_KEEP2) j = STORE_MAX_KEEP2;

		/* Always "keep" at least "STORE_MIN_KEEP" items */
		if (j < STORE_MIN_KEEP2) j = STORE_MIN_KEEP2;
	}

	/* Hack -- prevent "underflow" (This should never happen anyway.) */
	if (j < 0) j = 0;

	/* Destroy objects until only "j" slots are left */
	while (get_list_length(st_ptr->stock) > j) store_delete();


	/* Choose the number of slots to fill */
	j = get_list_length(st_ptr->stock);

	/* Buy some more items */
	j = j + randint1(STORE_TURNOVER);

	if (st_ptr->max_stock == STORE_INVEN_MAX)
	{
		/* Never keep more than "STORE_MAX_KEEP" slots */
		if (j > STORE_MAX_KEEP1) j = STORE_MAX_KEEP1;

		/* Always "keep" at least "STORE_MIN_KEEP" items */
		if (j < STORE_MIN_KEEP1) j = STORE_MIN_KEEP1;
	}
	else
	{
		/* The store has half the normal inventory space */

		/* Never keep more than "STORE_MAX_KEEP" slots */
		if (j > STORE_MAX_KEEP2) j = STORE_MAX_KEEP2;

		/* Always "keep" at least "STORE_MIN_KEEP" items */
		if (j < STORE_MIN_KEEP2) j = STORE_MIN_KEEP2;
	}

	/* Hack -- prevent "overflow" (This shouldn't do anything) */
	if (j >= st_ptr->max_stock) j = st_ptr->max_stock - 1;

	/* Acquire some new items */
	while ((get_list_length(st_ptr->stock) < j) && (i < 30))
	{
		/* Increment counter so we avoid taking too long */
		i++;

		/* Try to allocate some items */
		store_create();
	}


	/* Hack -- Restore the rating */
	dun_ptr->rating = old_rating;
}


/*
 * Shuffle one of the stores.
 */
static void store_shuffle(store_type *st_ptr)
{
	int j;
	
	object_type *o_ptr;

	/* Ignore home + locker */
	if (st_ptr->type == BUILD_STORE_HOME) return;

	/* Pick a new owner */
	for (j = st_ptr->owner; j == st_ptr->owner;)
	{
		st_ptr->owner = (byte)randint0(MAX_OWNERS);
	}

	/* Reset the owner data */
	st_ptr->data = 0;

	/* Hack -- discount all the items */
	OBJ_ITT_START(st_ptr->stock, o_ptr)
	{
		/* Hack -- Sell all old items for "half price" */
		if (!(o_ptr->xtra_name))
		{
			o_ptr->discount = 50;
		}

		/* Mega-Hack -- Note that the item is "on sale" */
		o_ptr->inscription = quark_add("on sale");
	}
	OBJ_ITT_END;
}


/*
 * Get the ID of a store item and return its value
 */
static int get_stock(int *com_val, cptr pmt, int maxobj)
{
	char command;

	char out_val[160];

	/* Get the item index */
	if (repeat_pull(com_val))
	{
		/* Verify the item */
		if ((*com_val >= 0) && (*com_val < maxobj))
		{
			/* Success */
			return (TRUE);
		}
	}
	else
	{
		/* Invalid repeat - reset it */
		repeat_clear();
	}

	/* Paranoia XXX XXX XXX */
	message_flush();


	/* Assume failure */
	*com_val = (-1);

	/* Build the prompt */
	(void)sprintf(out_val, "(Items a-%c, ESC to exit) %s",
			 I2A(maxobj - 1), pmt);

	/* Ask until done */
	while (TRUE)
	{
		int k;

		/* Escape */
		if (!get_com(out_val, &command)) break;

		/* Convert */
		k = (islower(command) ? A2I(command) : -1);

		/* Legal responses */
		if ((k >= 0) && (k < maxobj))
		{
			*com_val = k;
			break;
		}

		/* Oops */
		bell("Illegal store object choice!");
	}

	/* Clear the prompt */
	prt("", 0, 0);

	/* Cancel */
	if (command == ESCAPE) return (FALSE);

	repeat_push(*com_val);

	/* Success */
	return (TRUE);
}

static bool store_access_item(object_type *o_ptr, s32b price, bool buy)
{
	char out_val[160];
	char o_name[256];

	/* Describe the object (fully) */
	object_desc_store(o_name, o_ptr, TRUE, 3, 256);
	
	(void)sprintf(out_val, "%s %s, offer :  %ld",
				  (buy) ? "Buying" : "Selling", o_name, (long) price);
	put_str(out_val, 0, 1);
	
	(void)sprintf(out_val, "Do you want to %s it? ", (buy) ? "buy" : "sell");
	
	/* Ask the user for a response */
	if (!get_check(out_val)) return (FALSE);
	

	/* Chose to make transaction */
	return (TRUE);
}

/*
 * Buy an item from a store
 */
static void store_purchase(int *store_top)
{
	int i, amt;
	int item, item_new;

	s32b price, best;

	object_type forge;
	object_type *j_ptr;

	object_type *o_ptr;

	char o_name[256];

	char out_val[160];

	const owner_type *ot_ptr = &owners[f_ptr->data[0]][st_ptr->owner];

	/* Empty? */
	if (!st_ptr->stock)
	{
		if (st_ptr->type == BUILD_STORE_HOME)
			msg_print("Your home is empty.");
		else
			msg_print("I am currently out of stock.");
		return;
	}

	/* Find the number of objects on this and following pages */
	i = (get_list_length(st_ptr->stock) - *store_top);

	/* And then restrict it to the current page */
	if (i > 12) i = 12;

	/* Prompt */
	if (st_ptr->type == BUILD_STORE_HOME)
	{
		sprintf(out_val, "Which item do you want to take? ");
	}
	else
	{
		sprintf(out_val, "Which item are you interested in? ");
	}

	/* Get the item number to be bought */
	if (!get_stock(&item, out_val, i)) return;

	/* Get the actual index */
	item = item + *store_top;

	/* Get the actual item */
	o_ptr = get_list_item(st_ptr->stock, item);

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
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
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
	if (!(st_ptr->type == BUILD_STORE_HOME))
	{
		/* Get price */
		price = price_item(j_ptr, ot_ptr->min_inflate, FALSE) * amt;
		
		/* Player can afford it */
		if (p_ptr->au < price)
		{
			/* Simple message (no insult) */
			msg_print("You do not have enough gold.");

		}

		/* Player wants it? */
		else if (store_access_item(j_ptr, price, TRUE))
		{
			/* Say "okay" */
			say_comment_1();

			/* Make a sound */
			sound(SOUND_BUY);

			/* Spend the money */
			p_ptr->au -= price;

			/* Update the display */
			store_prt_gold();

			/* Hack -- buying an item makes you aware of it */
			object_aware(j_ptr);

			/* Describe the transaction */
			object_desc(o_name, j_ptr, TRUE, 3, 256);

			/* Message */
			msg_format("You bought %s for %ld gold.", o_name, (long)price);

			/* Erase the inscription */
			j_ptr->inscription = 0;

			/* Erase the "feeling" */
			j_ptr->feeling = FEEL_NONE;

			/* Give it to the player */
			j_ptr = inven_carry(j_ptr);

			/* Describe the final result */
			object_desc(o_name, j_ptr, TRUE, 3, 256);

			/* Get slot */
			item_new = get_item_position(p_ptr->inventory, j_ptr);

			/* Message */
			msg_format("You have %s (%c).", o_name, I2A(item_new));

			/* Now, reduce the original stack's pval. */
			if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
			{
				o_ptr->pval -= j_ptr->pval;

				/* No used charges in store stock */
				o_ptr->ac = 0;
			}

			/* Handle stuff */
			handle_stuff();

			/* Note how many slots the store used to have */
			i = get_list_length(st_ptr->stock);

			/* Remove the bought items from the store */
			item_increase(o_ptr, -amt);

			/* Store is empty */
			if (!st_ptr->stock)
			{
				/* Shuffle */
				if (one_in_(STORE_SHUFFLE))
				{
					/* Message */
					msg_print("The shopkeeper retires.");

					/* Shuffle the store */
					store_shuffle(st_ptr);
				}

				/* Maintain */
				else
				{
					/* Message */
					msg_print("The shopkeeper brings out some new stock.");
				}

				/* New inventory */
				for (i = 0; i < 10; i++)
				{
					/* Maintain the store */
					store_maint();
				}

				/* Start over */
				*store_top = 0;

				/* Redraw everything */
				display_inventory(*store_top);
			}

			/* The item is gone */
			else if (get_list_length(st_ptr->stock) != i)
			{
				/* Pick the correct screen */
				if (*store_top >= get_list_length(st_ptr->stock))
				{
					*store_top -= 12;
				}
				
				/* Redraw everything */
				display_inventory(*store_top);
			}

			/* Item is still here */
			else
			{
				/* Redraw the item */
				display_entry(item);
			}
		}
	}

	/* Home is much easier */
	else
	{
		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, j_ptr, amt);

		/* Give it to the player */
		j_ptr = inven_carry(j_ptr);

		/* Describe just the result */
		object_desc(o_name, j_ptr, TRUE, 3, 256);

		/* Get slot */
		item_new = get_item_position(p_ptr->inventory, j_ptr);

		/* Message */
		msg_format("You have %s (%c).", o_name, I2A(item_new));

		/* Handle stuff */
		handle_stuff();

		/* Take note if we take the last one */
		i = get_list_length(st_ptr->stock);

		/* Remove the items from the home */
		item_increase(o_ptr, -amt);

		/* Hack -- Item is still here */
		if (i == get_list_length(st_ptr->stock))
		{
			/* Redraw the item */
			display_entry(item);
		}

		/* The item is gone */
		else
		{
			/* Nothing left */
			if (!st_ptr->stock) *store_top = 0;

			/* Nothing left on that screen */
			else if (*store_top >= get_list_length(st_ptr->stock))
			{
				*store_top -= 12;
			}
			
			/* Redraw everything */
			display_inventory(*store_top);

			chg_virtue(V_SACRIFICE, 1);
		}
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an item to the store (or home)
 */
static void store_sell(int *store_top)
{
	int item_pos;
	int amt;

	const owner_type *ot_ptr = &owners[f_ptr->data[0]][st_ptr->owner];
	s32b price, value, dummy;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr q, s;

	char o_name[256];


	/* Prepare a prompt */
	if (st_ptr->type == BUILD_STORE_HOME)
	{
		q = "Drop which item? ";

		/* Home takes anything */
		item_tester_hook = NULL;
	}
	else
	{
		q = "Sell which item? ";

		/* Only allow items the store will buy */
		item_tester_hook = store_will_stock;
	}


	/* Get an item */
	s = "You have nothing that I want.";

	o_ptr = get_item(q, s, (USE_EQUIP | USE_INVEN));

	/* Not a valid item */
	if (!o_ptr) return;

	/* Hack -- Cannot remove cursed items */
	if ((!o_ptr->allocated) && cursed_p(o_ptr))
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
		if (amt <= 0) return;
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
	if (o_ptr->tval == TV_WAND)
	{
		q_ptr->pval = (o_ptr->pval + o_ptr->ac) * amt / o_ptr->number;

		/* Remove "used" charges */
		if (q_ptr->pval < o_ptr->ac)
		{
			q_ptr->pval = 0;
		}
		else
		{
			q_ptr->pval -= o_ptr->ac;
		}
	}

	if (o_ptr->tval == TV_ROD)
	{
		q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

	/* Get a full description */
	object_desc(o_name, q_ptr, TRUE, 3, 256);

	/* Remove any inscription, feeling for stores */
	if (!(st_ptr->type == BUILD_STORE_HOME))
	{
		q_ptr->inscription = 0;
		q_ptr->feeling = FEEL_NONE;
	}

	/* Is there room in the store (or the home?) */
	if (!store_check_num(q_ptr))
	{
		if (st_ptr->type == BUILD_STORE_HOME)
			msg_print("Your home is full.");
		else
			msg_print("I have not the room in my store to keep it.");
		return;
	}


	/* Real store */
	if (!(st_ptr->type == BUILD_STORE_HOME))
	{
		/* Get price */
		price = price_item(q_ptr, ot_ptr->min_inflate, FALSE) * amt;
	
		/* Sold... */
		if (store_access_item(q_ptr, price, FALSE))
		{
			/* Say "okay" */
			say_comment_1();

			/* Make a sound */
			sound(SOUND_SELL);

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
			 * Hack -- Allocate charges between those wands or rods sold
			 * and retained, unless all are being sold. -LM-
			 */
			distribute_charges(o_ptr, q_ptr, amt);

			/* Get the "actual" value */
			value = object_value(q_ptr) * q_ptr->number;

			/* Get the description all over again */
			object_desc(o_name, q_ptr, TRUE, 3, 256);

			/* Describe the result (in message buffer) */
			msg_format("You sold %s for %ld gold.", o_name, (long)price);

			if (!((q_ptr->tval == TV_FIGURINE) && (value > 0)))
			{
				/* 
				 * Analyze the prices (and comment verbally)
				 * unless object is a figurine
				 */
				purchase_analyze(price, value, dummy);
			}

			if (q_ptr->tval != TV_LITE)
			{
				/* Reset timeouts of the sold items */
				q_ptr->timeout = 0;
			}

			if (q_ptr->tval == TV_WAND)
			{
				/* Reset the "used" charges. */
				q_ptr->ac = 0;
			}

			/* Take the item from the player, describe the result */
			item_increase(o_ptr, -amt);

			/* Handle stuff */
			handle_stuff();

			/* The store gets that (known) item */
			q_ptr = store_carry(q_ptr);

			/* Get position */
			item_pos = get_item_position(st_ptr->stock, q_ptr);

			/* Re-display if item is now in store */
			if (item_pos >= 0)
			{
				*store_top = (item_pos / 12) * 12;
				display_inventory(*store_top);
			}
		}
	}

	/* Player is at home */
	else
	{
		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, q_ptr, amt);

		/* Describe */
		msg_format("You drop %s.", o_name);

		/* Take it from the players inventory */
		item_increase(o_ptr, -amt);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		q_ptr = home_carry(q_ptr);
		
		/* Get position */
		item_pos = get_item_position(st_ptr->stock, q_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			*store_top = (item_pos / 12) * 12;
			display_inventory(*store_top);
		}
	}
}


/*
 * Examine an item in a store			   -JDL-
 */
static void store_examine(int store_top)
{
	int i;
	int item;
	object_type *o_ptr;
	char o_name[256];
	char out_val[160];


	/* Empty? */
	if (!st_ptr->stock)
	{
		if (st_ptr->type == BUILD_STORE_HOME)
			msg_print("Your home is empty.");
		else
			msg_print("I am currently out of stock.");
		return;
	}


	/* Find the number of objects on this and following pages */
	i = (get_list_length(st_ptr->stock) - store_top);

	/* And then restrict it to the current page */
	if (i > 12) i = 12;

	/* Prompt */
	sprintf(out_val, "Which item do you want to examine? ");

	/* Get the item number to be examined */
	if (!get_stock(&item, out_val, i)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = get_list_item(st_ptr->stock, item);

	/* Require full knowledge */
	if (!object_known_full(o_ptr))
	{
		/* This can only happen in the home */
		msg_print("You have no special knowledge about that item.");
		return;
	}

	/* Description */
	object_desc(o_name, o_ptr, TRUE, 3, 256);

	/* Describe */
	msg_format("Examining %s...", o_name);

	/* Describe it fully */
	if (!identify_fully_aux(o_ptr))
		msg_print("You see nothing special.");

	return;
}


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
static void store_process_command(int *store_top)
{
	int stocknum = get_list_length(st_ptr->stock);

	/* Handle repeating the last command */
	repeat_check();

	if (rogue_like_commands && p_ptr->command_cmd == 'l')
	{
		p_ptr->command_cmd = 'x';	/* hack! */
	}

	/* Parse the command */
	switch (p_ptr->command_cmd)
	{
		case ESCAPE:
		{
			/* Leave */
			leave_store = TRUE;
			break;
		}

		case ' ':
		{
			/* Browse */
			if (stocknum <= 12)
			{
				msg_print("Entire inventory is shown.");
			}
			else
			{
				*store_top += 12;
				if (*store_top >= stocknum) *store_top = 0;
				display_inventory(*store_top);
			}
			break;
		}

		case KTRL('R'):
		{
			/* Redraw */
			do_cmd_redraw();
			display_store(*store_top);
			break;
		}

		case 'g':
		{
			/* Get (purchase) */
			store_purchase(store_top);
			break;
		}

		case 'd':
		{
			/* Drop (Sell) */
			store_sell(store_top);
			break;
		}

		case 'x':
		{
			/* Examine */
			store_examine(*store_top);
			break;
		}

		case '\r':
		{
			/* Ignore return */
			break;
		}

			/*** Inventory Commands ***/

		case 'w':
		{
			/* Wear/wield equipment */
			do_cmd_wield();
			break;
		}

		case 't':
		{
			/* Take off equipment */
			do_cmd_takeoff();
			break;
		}

		case 'k':
		{
			/* Destroy an item */
			do_cmd_destroy();
			break;
		}

		case 'e':
		{
			/* Equipment list */
			do_cmd_equip();
			break;
		}

		case 'i':
		{
			/* Inventory list */
			do_cmd_inven();
			break;
		}


			/*** Various commands ***/

		case 'I':
		{
			/* Identify an object */
			do_cmd_observe();
			break;
		}

		case KTRL('I'):
		{
			/* Hack -- toggle windows */
			toggle_inven_equip();
			break;
		}


			/*** Use various objects ***/

		case 'b':
		{
			/* Browse a book */
			do_cmd_browse();
			break;
		}

		case '{':
		{
			/* Inscribe an object */
			do_cmd_inscribe();
			break;
		}

		case '}':
		{
			/* Uninscribe an object */
			do_cmd_uninscribe();
			break;
		}



		/*** Help and Such ***/

		case '?':
		{
			/* Help */
			do_cmd_help();
			break;
		}

		case '/':
		{
			/* Identify symbol */
			do_cmd_query_symbol();
			break;
		}

		case 'C':
		{
			/* Character description */
			do_cmd_character();
			display_store(*store_top);
			break;
		}


		/*** System Commands ***/

		case '!':
		{
			/* Hack -- User interface */
			(void)Term_user(0);
			break;
		}

		case '"':
		{
			/* Single line from a pref file */
			do_cmd_pref();
			break;
		}

		case '@':
		{
			/* Interact with macros */
			do_cmd_macros();
			break;
		}

		case '%':
		{
			/* Interact with visuals */
			do_cmd_visuals();
			break;
		}

		case '&':
		{
			/* Interact with colors */
			do_cmd_colors();
			break;
		}

		case '=':
		{
			/* Interact with options */
			do_cmd_options(OPT_FLAG_SERVER | OPT_FLAG_PLAYER);
			break;
		}

			/*** Misc Commands ***/

		case ':':
		{
			/* Take notes */
			do_cmd_note();
			break;
		}

		case 'V':
		{
			/* Version info */
			do_cmd_version();
			break;
		}

		case KTRL('F'):
		{
			/* Repeat level feeling */
			do_cmd_feeling();
			break;
		}

		case KTRL('O'):
		{
			/* Show previous message */
			do_cmd_message_one();
			break;
		}

		case KTRL('P'):
		{
			/* Show previous messages */
			do_cmd_messages();
			break;
		}

		case '~':
		case '|':
		{
			/* Check artifacts, uniques, quests etc. */
			do_cmd_knowledge();
			break;
		}

		case '(':
		{
			/* Load "screen dump" */
			do_cmd_load_screen();
			break;
		}

		case ')':
		{
			/* Save "screen dump" */
			do_cmd_save_screen();
			break;
		}

		default:
		{
			/* Hack -- Unknown command */
			msg_print("That command does not work in stores.");
			break;
		}
	}
}


/*
 * Deallocate stores stock.
 *
 * This routine is used to deallocate the first store in the
 * store stock cache.  This is done to save memory.
 */
static void deallocate_store(void)
{
	int i;
	store_type *home;

	/* Return if there are no stores with stock */
	if (store_cache_num == 0) return;

	/* Do not deallocate homes or lockers */
	while (store_cache[0]->type == BUILD_STORE_HOME)
	{
		/* Hack - move home to end of cache */

		/* Keep track of stuff in home */
		home = store_cache[0];

		/* Resort the rest of the stores */
		for (i = 1; i < store_cache_num; i++)
		{
			store_cache[i - 1] = store_cache[i];
		}

		/* Move home to the end */
		store_cache[store_cache_num - 1] = home;
	}

	/* Delete store least used. */
	delete_object_list(&store_cache[0]->stock);

	/* Shift all other stores down the cache to fill the gap */
	for (i = 1; i < store_cache_num; i++)
	{
		store_cache[i - 1] = store_cache[i];
	}

	/* Decrease number of stores with stock */
	store_cache_num--;
}


/*
 * Allocate memory for a stores stock.
 *
 * This routine is used to save memory.  It is a waste to record
 * what is in every store in every town in the wilderness.  This
 * allocates the required array if the stockpointer is NULL.
 */
bool allocate_store(store_type *st_ptr)
{
	int i, n = 0;

	/* See if store has stock. */
	if (st_ptr->stock)
	{
		/* Find the location in the cache */
		for (i = 0; i < store_cache_num; i++)
		{
			/* See if cache location matches */
			if (st_ptr == store_cache[i])
			{
				/* note location */
				n = i;
				break;
			}
		}

		/* Resort order based on last_visit */
		for (i = n + 1; i < store_cache_num; i++)
		{
			store_cache[i - 1] = store_cache[i];
		}

		/* Move current one to end */
		store_cache[store_cache_num - 1] = st_ptr;

		/* (No need to maintain store) */
		return FALSE;
	}

	/* Store does not have stock - so need to allocate. */

	/* See if cache is full */
	if (store_cache_num == STORE_CACHE_AMNT)
	{
		/* Delete least used store */
		deallocate_store();
	}

	/* Add store to end of cache */
	store_cache[store_cache_num] = st_ptr;

	/* The number in the cache has increased */
	store_cache_num++;

	/* (Need to maintain stores) */
	return TRUE;
}

store_type *get_current_store(void)
{
	place_type *pl_ptr = &place[p_ptr->place_num];

	int i, which = -1;

	/* Get the building the player is on */
	for (i = 0; i < pl_ptr->numstores; i++)
	{
		if ((p_ptr->py - pl_ptr->y * 16 == pl_ptr->store[i].y) &&
			(p_ptr->px - pl_ptr->x * 16 == pl_ptr->store[i].x))
		{
			which = i;
		}
	}

	/* Paranoia */
	if (which == -1)
	{
		msg_print("Could not locate building!");
		return (NULL);
	}

	/* Return a pointer to the store */
	return (&pl_ptr->store[which]);
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
void do_cmd_store(field_type *f1_ptr)
{
	int maintain_num;
	int tmp_chr;
	int i;
	int store_top;

	/* Hack - save f1_ptr for later */
	f_ptr = f1_ptr;

	/* Save the store pointer */
	st_ptr = get_current_store();
	
	/* Paranoia */
	if (!st_ptr) return;

	/* Hack - save interesting flags for later */
	info_flags = f_ptr->data[7];


	/* Hack -- Check the "locked doors" */
	if (ironman_shops)
	{
		msg_print("The doors are locked.");
		return;
	}

	/* Calculate the number of store maintainances since the last visit */
	maintain_num = (turn - st_ptr->last_visit) / (10L * STORE_TURNS);


	/* Recalculate maximum number of items in store */
	if (f_ptr->data[7] & ST_HALF_INVEN)
	{
		st_ptr->max_stock = STORE_INVEN_MAX / 2;
	}
	else
	{
		st_ptr->max_stock = STORE_INVEN_MAX;
	}

	/* Allocate object storage if required */
	if (allocate_store(st_ptr))
	{
		/* Hack - Maintain store if it is just allocated. */
		maintain_num++;
	}

	/* Maintain the store max. 20 times */
	if (maintain_num > 20) maintain_num = 20;

	if (maintain_num)
	{
		/* Maintain the store */
		for (i = 0; i < maintain_num; i++)
		{
			store_maint();
		}

		/* Save the visit */
		st_ptr->last_visit = turn;
	}

	/* Forget the view */
	forget_view();


	/* Hack -- Character is in "icky" mode */
	character_icky = TRUE;


	/* No command argument */
	p_ptr->command_arg = 0;

	/* No repeated command */
	p_ptr->command_rep = 0;

	/* No automatic command */
	p_ptr->command_new = 0;


	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store(store_top);

	/* Do not leave */
	leave_store = FALSE;

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 0, 1);

		/* Hack -- Check the charisma */
		tmp_chr = p_ptr->stat_use[A_CHR];

		/* Clear */
		clear_from(21);


		/* Basic commands */
		prt(" ESC) Exit from Building.", 0, 22);

		/* Browse if necessary */
		if (get_list_length(st_ptr->stock) > 12)
		{
			prt(" SPACE) Next page of stock", 0, 23);
		}

		/* Home commands */
		if (st_ptr->type == BUILD_STORE_HOME)
		{
			prt(" g) Get an item.", 31, 22);
			prt(" d) Drop an item.", 31, 23);
		}

		/* Shop commands XXX XXX XXX */
		else
		{
			prt(" p) Purchase an item.", 31, 22);
			prt(" s) Sell an item.", 31, 23);
		}

		/* Add in the eXamine option */
		prt(" x) eXamine an item.", 56, 22);

		/* Prompt */
		prt("You may: ", 0, 21);

		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		store_process_command(&store_top);

		/* Hack -- Character is still in "icky" mode */
		character_icky = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/* XXX XXX XXX Pack Overflow */
		if (get_list_length(p_ptr->inventory) > INVEN_PACK)
		{
			/* Message */
			msg_print("Your pack is so full that you flee outside...");

			/* Leave */
			leave_store = TRUE;
		}

		/* Hack -- Redisplay store prices if charisma changes */
		if (tmp_chr != p_ptr->stat_use[A_CHR])
		{
			display_inventory(store_top);
		}
	}


	/* Free turn XXX XXX XXX */
	p_ptr->energy_use = 0;


	/* Hack -- Character is no longer in "icky" mode */
	character_icky = FALSE;


	/* Hack -- Cancel automatic command */
	p_ptr->command_new = 0;

	/* Hack -- Cancel "see" mode */
	p_ptr->command_see = FALSE;


	/* Flush messages XXX XXX XXX */
	message_flush();


	/* Clear the screen */
	Term_clear();


	/* Update everything */
	p_ptr->update |= (PU_VIEW);
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}


/*
 * Initialize a store
 */
void store_init(int town_num, int store_num, byte store_type)
{
	/* Activate that store */
	st_ptr = &place[town_num].store[store_num];

	/* Pick an owner */
	st_ptr->owner = (byte)randint0(MAX_OWNERS);

	/* Do not allocate the stock yet. */
	st_ptr->stock = 0;

	/* Set the store type */
	st_ptr->type = store_type;

	/* Initialize the store */
	st_ptr->data = 0;

	/*
	 * Hack - maximum items in stock
	 * (This number may be changed when the store
	 *  is actually created.)
	 * The reason we do it later, is because we do not
	 * have the field data at this stage.  This line
	 * perhaps can be removed, but beware of strange bugs
	 * popping up in other code.
	 */
	st_ptr->max_stock = STORE_INVEN_MAX;

	/*
	 * MEGA-HACK - Last visit to store is
	 * BEFORE player birth to enable store restocking
	 */
	st_ptr->last_visit = -100L * STORE_TURNS;
}
