/* CVS: Last edit by $Author$ on $Date$ */
/* File: wild.c */

/* Purpose: Wilderness generation */

/*
 * Copyright (c) 1989, 1999 James E. Wilson, Robert A. Koeneke,
 * Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"




/* Code fragment that lights / darkens old wilderness */

#if 0

	/* Day time */
	if ((turn % (10L * TOWN_DAWN)) < ((10L * TOWN_DAWN) / 2))
		daytime = TRUE;
	else
		daytime = FALSE;

	/* Light up or darken the area */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Get the cave grid */
			c_ptr = &cave[y][x];

			if (daytime)
			{
				/* Assume lit */
				c_ptr->info |= (CAVE_GLOW);

				/* Hack -- Memorize lit grids if allowed */
				if (view_perma_grids) c_ptr->info |= (CAVE_MARK);
			}
			else
			{
				/* Darken "boring" features */
				if ((c_ptr->feat <= FEAT_INVIS) ||
				    ((c_ptr->feat >= FEAT_DEEP_WATER) &&
					(c_ptr->feat <= FEAT_TREES)))
				{
					/* Forget the grid */
					c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);
				}
			}
		}
	}
#endif


/* Town currently stored in cave[][] */
static u16b cur_town;


/* Old in_bounds(2) macros are now functions */


/*
 * Determines if a map location is fully inside the outer walls
 */
static bool in_bounds_cave(int y, int x)
{
	return ((y > 0) && (x > 0) && (y < cur_hgt-1) && (x < cur_wid-1));
}

/*
 * Determines if a map location is on or inside the outer walls
 */
static bool in_bounds2_cave(int y, int x)
{
	return ((y >= 0) && (x >= 0) && (y < cur_hgt) && (x < cur_wid));
}

/*
 * Wilderness bounds funcitons.
 * These functions check to see if a square is accessable on the grid
 * of wilderness blocks in the cache.
 */

/*
 * Determines if a map location is fully inside the outer boundary
 */
static bool in_bounds_wild(int y, int x)
{
	/* Multiply block bounds by 16 to get bounds in normal coords. */
	return ((y > wild_grid.y_min) &&
	        (x > wild_grid.x_min) &&
	        (y < wild_grid.y_max - 1) &&
	        (x < wild_grid.x_max - 1));
}

/*
 * Determines if a map location is on or inside the outer boundary
 */
static bool in_bounds2_wild(int y, int x)
{
	return ((y >= wild_grid.y_min) && (x >= wild_grid.x_min) &&
	        (y < wild_grid.y_max) && (x < wild_grid.x_max));
}


/* Access the old cave array. */
static cave_type *access_cave(int y, int x)
{
	return &cave[y][x];
}

/* Access wilderness */
static cave_type *access_wild(int y, int x)
{
	/*
	 * Divide by 16 to get block.
	 * Logical AND with 15 to get location within block.
	 */

	return &wild_grid.block_ptr[((u16b) y>>4) - wild_grid.y]
		[((u16b) x>>4) - wild_grid.x][y & 15][x & 15];
}


/*
 * This function _must_ be called whenever the dungeon level changes.
 * It makes sure the bounds and access functions point to the correct
 * functions.  If this is not done - bad things happen.
 */

void change_level(int level)
{
	if (!level)
	{
		/* In the wilderness */
		in_bounds = in_bounds_wild;
		in_bounds2 = in_bounds2_wild;
		area = access_wild;

		/*
		 * Allocate blocks around player - only has effect if
		 * old game is loaded in the dungeon.
		 */
		if (!dun_level)
		{
			move_wild();
		}
	}
	else
	{
		/* In the dungeon */
		in_bounds = in_bounds_cave;
		in_bounds2 = in_bounds2_cave;
		area = access_cave;

		/* No town stored in cave[][] */
		cur_town = 0;
	}
}



/*
 * Builds a store at a given pseudo-location
 *
 * As of Z 2.3.6 the town is moved back to (0,0) - and is overlayed
 * on top of the wilderness.
 *
 * As of 2.8.1 (?) the town is actually centered in the middle of a
 * complete level, and thus the top left corner of the town itself
 * is no longer at (0,0), but rather, at (qy,qx), so the constants
 * in the comments below should be mentally modified accordingly.
 *
 * As of 2.7.4 (?) the stores are placed in a more "user friendly"
 * configuration, such that the four "center" buildings always
 * have at least four grids between them, to allow easy running,
 * and the store doors tend to face the middle of town.
 *
 * The stores now lie inside boxes from 3-9 and 12-18 vertically,
 * and from 7-17, 21-31, 35-45, 49-59.  Note that there are thus
 * always at least 2 open grids between any disconnected walls.
 *
 * Note the use of "town_illuminate()" to handle all "illumination"
 * and "memorization" issues.
 */
static void build_store(int n, int yy, int xx)
{
	int y, x, y0, x0, y1, x1, y2, x2, tmp;

	/* Find the "center" of the store */
	y0 = yy * 6 + 4;
	x0 = xx * 17 + 8;

	/* Determine the store boundaries */
	y1 = y0 - randint(2);
	y2 = y0 + randint(2);
	x1 = x0 - randint(5);
	x2 = x0 + randint(5);

	/* Build an invulnerable rectangular building */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			/* Create the building */
			cave[y][x].feat = FEAT_PERM_EXTRA;
		}
	}

	/* Pick a door direction (S,N,E,W) */
	tmp = rand_int(4);

	/* Re-roll "annoying" doors */
	if (((tmp == 0) && (yy == 1)) ||
	    ((tmp == 1) && (yy == 0)) ||
	    ((tmp == 2) && (xx == 3)) ||
	    ((tmp == 3) && (xx == 0)))
	{
		/* Pick a new direction */
		tmp = rand_int(4);
	}

	/* Extract a "door location" */
	switch (tmp)
	{
		/* Bottom side */
		case 0:
		{
			y = y2;
			x = rand_range(x1, x2);
			break;
		}

		/* Top side */
		case 1:
		{
			y = y1;
			x = rand_range(x1, x2);
			break;
		}

		/* Right side */
		case 2:
		{
			y = rand_range(y1, y2);
			x = x2;
			break;
		}

		/* Left side */
		default:
		{
			y = rand_range(y1, y2);
			x = x1;
			break;
		}
	}

	/* Clear previous contents, add a store door */
	cave[y][x].feat = FEAT_SHOP_HEAD + n;
}


/*
 * Generate the "consistent" town features, and place the player
 *
 * Hack -- play with the R.N.G. to always yield the same town
 * layout, including the size and shape of the buildings, the
 * locations of the doorways, and the location of the stairs.
 */
static void town_gen_hack(u16b town_num)
{
	int y, x, k, n;

	/* This simple routine does not check the type of
	 * stores town_num wants.
	 */

	int rooms[MAX_STORES];


	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant town layout */
	Rand_value = town[town_num].seed;


	/* Prepare an Array of "remaining stores", and count them */
	for (n = 0; n < MAX_STORES; n++) rooms[n] = n;

	/* Place three rows of stores */
	for (y = 0; y < 3; y++)
	{
		/* Place three stores per row */
		for (x = 0; x < 3; x++)
		{
			/* Pick a random unplaced store */
			k = ((n <= 1) ? 0 : rand_int(n));

			/* Build that store at the proper location */
			build_store(rooms[k], y, x);

			/* Shift the stores down, remove one store */
			rooms[k] = rooms[--n];
		}
	}


	/* Place the stairs */
	while (TRUE)
	{
		/* Pick a location at least "three" from the outer walls */
		y = rand_range(3, SCREEN_HGT - 4);
		x = rand_range(3, SCREEN_WID - 4);

		/* Require a "naked" floor grid */
		if (cave[y][x].feat == FEAT_FLOOR) break;
	}

	/* Clear previous contents, add down stairs */
	cave[y][x].feat = FEAT_MORE;

	/* Hack -- use the "complex" RNG */
	Rand_quick = FALSE;
}




/*
 * Town logic flow for generation of new town
 *
 * We start with a fully wiped cave of normal floors.
 *
 * Note that town_gen_hack() plays games with the R.N.G.
 *
 * This function does NOT do anything about the owners of the stores,
 * nor the contents thereof.  It only handles the physical layout.
 *
 */
static void town_gen(u16b town_num)
{
	int y, x;

	/* Place transparent area */
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			/* Create empty area */
			cave[y][x].feat = FEAT_NONE;
		}
	}

	/* Place some floors */
	for (y = 1; y < SCREEN_HGT-1; y++)
	{
		for (x = 1; x < SCREEN_WID-1; x++)
		{
			/* Create empty floor */
			cave[y][x].feat = FEAT_FLOOR;
		}
	}

	/* Build stuff */
	town_gen_hack(town_num);

	/* Town is now built */
	cur_town = town_num;
}

/*
 * Overlay a section of a town onto the wilderness
 */
static void overlay_town(byte y, byte x, u16b w_town, blk_ptr block_ptr)
{
	int i, j, xx, yy;
	byte feat;

	/* Find block to copy */
	xx = (x - town[w_town].x) << 4;
	yy = (y - town[w_town].y) << 4;

	/* copy 16x16 block from cave[][] */
	for (j = 0; j < WILD_BLOCK_SIZE; j++)
	{
		for (i = 0; i < WILD_BLOCK_SIZE; i++)
		{
			feat = cave[yy + j][xx + i].feat;

			if (feat != FEAT_NONE)
			{
				/* Only copy if there is something there. */
				block_ptr[j][i].feat = feat;
			}
		}
	}
}


/*
 * Initialise the town structures
 * At the moment there is only one town generator
 * and four towns.
 *
 * The four towns are right next to each other. This is
 * a test of the speed of the switching mechanism.
 * Hopefully it will be fast enough so that towns can be placed
 * anywhere in the wilderness.  It will also be possible to
 * have *HUGE* towns (cities) composed of smaller parts.
 */
static void init_towns(void)
{
	int i, j;
	u16b town_num;


	for (town_num = 1; town_num <= 4; town_num++)
	{
		strcpy(town[town_num].name, "town");
		town[town_num].seed = rand_int(0x10000000);
		town[town_num].numstores = 9;
		town[town_num].type = 1;
		town[town_num].x = 32 + ((town_num - 1) % 2) * (SCREEN_WID / 16 + 1);
		town[town_num].y = 32 + ((town_num - 1) / 2) * (SCREEN_HGT / 16 + 1);

		/* Place town on wilderness */
		for (j = 0; j < (SCREEN_HGT / 16 + 1); j++)
		{
			for (i = 0; i < (SCREEN_WID / 16 +1); i++)
			{
				wild[town[town_num].y + j][town[town_num].x + i]
					.done.town = town_num;
			}
		}
	}

	/* No current town in cave[][] */
	cur_town = 0;
}


/*
 * Set cur_town = 0
 * This function exists so that this variable remains hidden.
 * This is only used after a game is loaded - when cave[][]
 * is blank.
 */
void set_no_town(void)
{
	cur_town = 0;
}


/*
 * This section deals with wilderness generation
 * - both at the start of the game, and sorting
 * out what routines are used to make things as
 * the player moves around.
 *
 * Note that these routines return zero as a fail
 * indicator.  They return a non zero value for
 * a success - the value being the node last added
 * to the decision tree.  (This usually can be
 * ignored.)
 *
 */



/*
 * This function returns a wilderness block type that fits
 * the required parameters.
 *
 * The set of generation types is stored in a "decision tree"
 * - so the required time to get the wilderness type from the
 * three parameters (hgt,pop,law) is proportional to log(n).
 * This speeds up wilderness generation alot.  (Note the
 * "obvious" method of using a linear search to find matching
 * wilderness creation functions is too slow.
 *
 * The "type" value has two different uses.  One is to specify
 * which axis of the parameter space is being split.  The other
 * is to show whether or not a node is a terminal "leaf" node.
 * If it is a leaf node - the value returned is the number of
 * the type of wilderness generation function.
 */
static u16b get_gen_type(byte hgt, byte pop, byte law)
{
	/* Current node in choice tree - node zero is the "trunk" */
	int node = 0;
	
	/* branch of tree to go down */
	bool branch = TRUE;
	
	wild_choice_tree_type *tree_ptr;
	
	/* Find matching generation type */
	
	/* The while loop is used instead of the "obvious" recursion */
	while(1)
	{
		/* Access Node */
		tree_ptr = &wild_choice_tree[node];
		
		/* If are near end - look at leaves of tree		
		 * 
		 * (cutoff == 0) is used as a flag since it doesn't
		 * split the possibility tree in any useful way.
		 */
		if (tree_ptr->cutoff == 0)
		{
			/* randomly choose branch */
			if (randint(tree_ptr->chance1+tree_ptr->chance2) 
				> tree_ptr->chance2)
			{
				/* Chance1 of going "left" */
				branch = TRUE;
			}
			else
			{
				/* Chance2 of going "right" */
				branch = FALSE;
			}
		}
		else
		{
			/* 
			 * Get lower two bits of type to decribe which of
			 * (hgt,pop,law) cutoff refers to.
			 */
			
			switch(tree_ptr->info & 3)
			{
				case 1: 
				{
					/* Look at height */
					if (tree_ptr->cutoff >= hgt)
					{
						branch = TRUE;
					}
					else
					{
						branch = FALSE;
					}
					
					break;
				}
				case 2:
				{
					/* Look at population */
					if (tree_ptr->cutoff >= pop)
					{
						branch = TRUE;
					}
					else
					{
						branch = FALSE;
					}
				
					break;
				}
				case 3:
				{
					/* Look at lawfulness */
					if (tree_ptr->cutoff >= law)
					{
						branch = TRUE;
					}
					else
					{
						branch = FALSE;
					}
					
					break;
				}
				default:
				{
					msg_print("Invalid stat chosen!");
					
					break;
				}
			}
		}
		
		
		/* Look at the proper branch of the tree */ 
		if (branch)
		{
			/* Go "left" */
			
			/* See if references leaf node */
			if (tree_ptr->info & 4)
			{
				/* If the bit is set - leaf */
				return (tree_ptr->ptrnode1);			
			}
			else
			{
				/* use the while loop to recurse */
				node = tree_ptr->ptrnode1;
			}
		}
		else
		{
			/* Go "right" */
			
			/* See if references leaf node */
			if (tree_ptr->info & 8)
			{
				/* If the bit is set - leaf */
				return (tree_ptr->ptrnode2);			
			}
			else
			{
				/* use the while loop to recurse */
				node = tree_ptr->ptrnode2;
			}
		}	
	}
}

/* The number of allocated nodes in the decsion tree */
static u16b d_tree_count;

/*
 * This function creates a new node on the decision tree.
 * It then connects that node to the node referenced by
 * the variable "node".  The process of making the link
 * stomps on any information about the old link.
 *
 * branch == TRUE is "left" , FALSE is "right" 
 *
 * This function returns the location of the new node in
 * the decision tree array.
 */ 
static u16b create_node(u16b node, bool branch)
{
	u16b new_node;
	
	wild_choice_tree_type	*tree_ptr;
	
	if (d_tree_count >= max_w_node)
	{
		/* 
		 * Return zero (known as the location of the tree's
		 * "root" - so can act as a flag) if all of the
		 * memory allocated has been used.
		 *
		 * Always check the return value - and report the error
		 *
		 * The number of nodes required is roughly proportional
		 * to nlog(n) for a random input of ranges.
		 */
		 return(0);	
	}
	
	/* Get location of new node */
	new_node = d_tree_count;
	
	/* Access old node */
	tree_ptr = &wild_choice_tree[node];
	
	if (branch)
	{
		/* Link new node to left of old */
		tree_ptr->ptrnode1 = new_node;
		
		/* Link is not to a leaf */
		tree_ptr->info &= ~4;
	}
	else
	{
		/* Link new node to right of old */
		tree_ptr->ptrnode2 = new_node;
		
		/* Link is not to a leaf */
		tree_ptr->info &= ~8;	
	}
	
	/* Increase count of allocated nodes */
	d_tree_count++;
	
	return(new_node);	
}

/*
 * This function deletes the last node on the decision tree.
 * It is needed for a major hack when something is being added
 * to a "null" region.
 *
 * This routine, and the above routine are the only ones that can
 * modify the number of nodes in the array.  (This makes checking
 * for the array-full case much easier.)
 */
static void delete_last_node(void)
{
	d_tree_count--;
}

/*
 * This function adds a node to the tree between a "leaf"
 * and the rest of the tree.  As nodes are added to the "leaf"
 * the chance of each wilderness generation type is collated.
 *
 * Note - this function used so that several different wilderness
 * generation types can exist within the same region in parameter
 * space.  Each possibility has a "chance" associated with it.
 * At generation time - the RNG is used to determine which node
 * in the "leaf" of several different generation types is used.
 * The wilderness generation type of that node is then used.
 *
 * This function also takes care of the case when something is
 * being added to a "null" node.  "Null" nodes of type zero
 * describe areas of parameter space that are outside the currently
 * used area.  This is needed because the decision tree starts out
 * empty.  As wilderness generation types are added, the null area
 * is slowly clipped away.  If at the end of creating the decision
 * tree and there is any "null" area left, the types do not fill
 * parameter space.  This will flag an error. 
 */ 

static u16b add_node_chance(u16b type, u16b node, bool branch)
{
	/* Is the "leaf" a tree of nodes - or a single node. */
	bool is_tree;
	
	/* The node inserted into the decision tree */
	u16b new_node;
	wild_choice_tree_type	*tree_ptr;
	
	/* The old connection */
	u16b old_node;
	
	tree_ptr = &wild_choice_tree[node];
	
	if (branch)
	{		
		old_node = tree_ptr->ptrnode1;
		
		/* Check for null case. */
		if (old_node == 0)
		{
			/* Easy - just replace with data */
		 	tree_ptr->ptrnode1 = type;
			
			/* Return current node. */
			return(node);
		}
		
		/* Get left leaf status */
		is_tree = (wild_choice_tree[old_node].info & 4);
		
	}
	else
	{		
		old_node = tree_ptr->ptrnode2;
		
		/* Check for null case. */
		if (old_node == 0)
		{
			/* Easy - just replace with data */
		 	tree_ptr->ptrnode2 = type;
			
			/* Return current node. */
			return(node);
		}
		
		/* Get right leaf status */
		is_tree = (wild_choice_tree[old_node].info & 8);		
	}

	/* Insert new node */
	new_node = create_node(node, branch);
	
	/* Error if array is full */	
	if (new_node==0)
	{
		/* Return zero as error code */
		return(0);
	}	
	
	/* Access node */
	tree_ptr = &wild_choice_tree[new_node];
	
	/* Cutoff = 0 since is a leaf node */
	tree_ptr->cutoff = 0;
	
	/* Connect to old leaf */
	tree_ptr->ptrnode1 = old_node;
	
	/* Connect to new type */
	tree_ptr->ptrnode2 = type;
	
	
	if (is_tree)
	{
		/* Set "info" bit-flag */
		/* Only new node is a pointer to gen. type */
		tree_ptr->info = 8;
		
		/* Calculate the chance fields */		
		tree_ptr->chance1 = wild_choice_tree[old_node].chance1 +
			wild_choice_tree[old_node].chance2;
		
		tree_ptr->chance2 = wild_gen_data[type].chance;
	}
	else
	{
		/* Set "info" bit-flag */
		/* Both links are to wild. gen. types. */
		tree_ptr->info = 8 + 4;
		
		/* Calculate the chance fields */
		tree_ptr->chance1 = wild_gen_data[old_node].chance;
		tree_ptr->chance2 = wild_gen_data[type].chance;
	}
	
	/* Return location of new node if required */
	return(new_node);
}


/*
 * This function copies the contents of one "leaf" (specified by
 * node1 + branch1) to the side of another node.
 *
 * This is needed because as the tree splits the parameter space
 * the leaves occupy regions.  When new wild. gen. types are added
 * to the decision tree - the "leaves" may not match their size.
 * This means that the leaves need to be split - in other words
 * copied.
 */
static u16b copy_branch(u16b node1, bool branch1, u16b node2, bool branch2)
{
	/* This function assumes that the "leaves" are of this form:
	*
	*StartNode
	* /  \
	*x  Node
	*    / \
	* type Node
	*       / \
	*    type Node
	*          / \
	*       type type
	*
	* (Where one pointer connects to a node, and one to a wild. gen. type)
	*/
	
	/* 
	 * The complexity of this function is due to the large number of
	 * possibilities:  both branches can be left of right, and the node
	 * can be terminal or not.  This gives a set of nested "if's" resulting
	 * in eight small sections of code.
	 */
	u16b new_node;
	u16b temp_node;
	
	wild_choice_tree_type	*tree_ptr1;
	wild_choice_tree_type	*tree_ptr2;
		
	/* point to node to be copied from */
	tree_ptr1 = &wild_choice_tree[node1];
	
	/* work out what has to be copied. */
	if (branch1)
	{
		if(tree_ptr1->info & 4)
		{
			/* need to copy tree of nodes */
			
			/* make new node */
			new_node = create_node(node2, branch2);
			
			/* Exit on failure */
			if(new_node == 0) return(0);
			
			/* Point to block to copy */
			temp_node = tree_ptr1->ptrnode1;
			tree_ptr1 = &wild_choice_tree[temp_node];
			
			/* Point to new block */
			tree_ptr2 = &wild_choice_tree[new_node];
			
			/* Copy data to new node */
			tree_ptr2->info = tree_ptr1->info;
			tree_ptr2->cutoff = tree_ptr1->cutoff;
			tree_ptr2->chance1 = tree_ptr1->chance1;
			tree_ptr2->chance2 = tree_ptr1->chance2;
			tree_ptr2->ptrnode1 = tree_ptr1->ptrnode1;
			tree_ptr2->ptrnode2 = tree_ptr1->ptrnode2;
			
			/* Recurse along branches to this node */
			if(!(tree_ptr2->info & 4))
			{
				/* Recurse along "left" branch */
				if(copy_branch(temp_node, TRUE, new_node, TRUE) == 0)
					return(0);		
			}
			
			if(!(tree_ptr2->info & 8))
			{
				/* Recurse along "right" branch */
				if(copy_branch(temp_node, TRUE, new_node, TRUE) == 0)
					return(0);
			}
			
			/* Done */
			return(new_node);
		}
		else
		{
			/* point to node to be copied to */
			tree_ptr2 = &wild_choice_tree[node2];
			
			/* only need to copy a single wild. gen. type */
			if(branch2)
			{
				/* terminal branch */
				tree_ptr2->info |= 4;
				
				/* Copy information */
				tree_ptr2->ptrnode1 = tree_ptr1->ptrnode1;
				tree_ptr2->chance1 = tree_ptr1->chance1;
			}
			else
			{
				/* terminal branch */
				tree_ptr2->info |= 8;
				
				/* Copy information */
				tree_ptr2->ptrnode2 = tree_ptr1->ptrnode1;
				tree_ptr2->chance2 = tree_ptr1->chance1;
			}
			
			/* done */
			return(node2);
		}
	}
	else
	{
		if(tree_ptr1->info & 8)
		{
			/* need to copy tree of nodes */
			
			/* make new node */
			new_node = create_node(node2, branch2);
			
			/* Exit on failure */
			if(new_node == 0) return(0);
			
			/* Point to block to copy */
			temp_node = tree_ptr1->ptrnode2;
			tree_ptr1 = &wild_choice_tree[temp_node];
			
			/* Point to new block */
			tree_ptr2 = &wild_choice_tree[new_node];
			
			/* Copy data to new node */
			tree_ptr2->info = tree_ptr1->info;
			tree_ptr2->cutoff = tree_ptr1->cutoff;
			tree_ptr2->chance1 = tree_ptr1->chance1;
			tree_ptr2->chance2 = tree_ptr1->chance2;
			tree_ptr2->ptrnode1 = tree_ptr1->ptrnode1;
			tree_ptr2->ptrnode2 = tree_ptr1->ptrnode2;
			
			/* Recurse along branches to this node */
			if(!(tree_ptr2->info & 4))
			{
				/* Recurse along "left" branch */
				if(copy_branch(temp_node, TRUE, new_node, TRUE) == 0)
					return(0);		
			}
			
			if(!(tree_ptr2->info & 8))
			{
				/* Recurse along "right" branch */
				if(copy_branch(temp_node, TRUE, new_node, TRUE) == 0)
					return(0);
			}
			
			/* Done */
			return(new_node);
		}
		else
		{
			/* point to node to be copied to */
			tree_ptr2 = &wild_choice_tree[node2];
			
			/* only need to copy a single wild. gen. type */
			if(branch2)
			{
				/* terminal branch */
				tree_ptr2->info |= 4;
				
				/* Copy information */
				tree_ptr2->ptrnode1 = tree_ptr1->ptrnode2;
				tree_ptr2->chance1 = tree_ptr1->chance2;
			}
			else
			{
				/* terminal branch */
				tree_ptr2->info |= 8;
				
				/* Copy information */
				tree_ptr2->ptrnode2 = tree_ptr1->ptrnode2;
				tree_ptr2->chance2 = tree_ptr1->chance2;
			}
			
			/* done */
			return(node2);
		}
	}
}


/*
 * This function is used to add a wilderness generation type within another
 * typed region of parameter space described by the decision tree.  This is
 * the only function that actually extends the decision tree itself.  (The
 * add_node_chance() function increases the size of the "leaves" though.)
 *
 * The bounding box of the bigger region (number 1) is repeatedly clipped onto
 * the sides of the smaller region2.  This can result with up to 6 nodes being
 * used.  Finally, when the two regions are the same size, the add_node_chance()
 * function is called to extend the "leaves" of the decision tree.
 *
 * This function must be called with a new empty node.  The node must be connected
 * to the tree by the calling routine.  (This "feature" is so that this routine
 * can be used to initialise an empty decision tree.)  This means that the calling
 * routine must check for the "null" node + completely filled case. XXX XXX
 */

static u16b add_node_inside(u16b node, u16b type1, wild_bound_box_type *bound1,
	 u16b type2, wild_bound_box_type *bound2)
{
	/* The node inserted into the decision tree */
	u16b new_node;
	wild_choice_tree_type	*tree_ptr;
	
	tree_ptr = &wild_choice_tree[node];
	
	if(bound1->hgtmin != bound2->hgtmin)
	{
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->hgtmin;
		
		/* Excess is smaller than cutoff */
		tree_ptr->ptrnode1 = type1;
		
		/* Cutoff = hgt , ptrnode1 = wild. gen. type. */
		tree_ptr->info = 1 + 4;
		
		/* Wipe chance values  (this probably isn't needed) */
		tree_ptr->chance1 = 0;
		tree_ptr->chance2 = 0;
		
		/* Add new node to decision tree */
		new_node = create_node(node, FALSE);
		
		/* Exit if out of space */
		if (new_node == 0) return(0);
	
		/* reset node to current end of tree */
		node = new_node;
		tree_ptr = &wild_choice_tree[node];
	}
	
	if(bound1->hgtmax != bound2->hgtmax)
	{
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->hgtmax;
		
		/* Excess is larger than cutoff */
		tree_ptr->ptrnode2 = type1;
		
		/* Cutoff = hgt , ptrnode2 = wild. gen. type. */
		tree_ptr->info = 1 + 8;
		
		/* Wipe chance values  (this probably isn't needed) */
		tree_ptr->chance1 = 0;
		tree_ptr->chance2 = 0;
		
		/* Add new node to decision tree */
		new_node = create_node(node, TRUE);
		
		/* Exit if out of space */
		if (new_node == 0) return(0);
	
		/* reset node to current end of tree */
		node = new_node;
		tree_ptr = &wild_choice_tree[node];
	}
	
	if(bound1->popmin != bound2->popmin)
	{
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->popmin;
		
		/* Excess is smaller than cutoff */
		tree_ptr->ptrnode1 = type1;
		
		/* Cutoff = pop , ptrnode1 = wild. gen. type. */
		tree_ptr->info = 2 + 4;
		
		/* Wipe chance values  (this probably isn't needed) */
		tree_ptr->chance1 = 0;
		tree_ptr->chance2 = 0;
		
		/* Add new node to decision tree */
		new_node = create_node(node, FALSE);
		
		/* Exit if out of space */
		if (new_node == 0) return(0);
	
		/* reset node to current end of tree */
		node = new_node;
		tree_ptr = &wild_choice_tree[node];
	}
	
	if(bound1->popmax != bound2->popmax)
	{
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->popmax;
		
		/* Excess is larger than cutoff */
		tree_ptr->ptrnode2 = type1;
		
		/* Cutoff = pop , ptrnode2 = wild. gen. type. */
		tree_ptr->info = 2 + 8;
		
		/* Wipe chance values  (this probably isn't needed) */
		tree_ptr->chance1 = 0;
		tree_ptr->chance2 = 0;
		
		/* Add new node to decision tree */
		new_node = create_node(node, TRUE);
		
		/* Exit if out of space */
		if (new_node == 0) return(0);
	
		/* reset node to current end of tree */
		node = new_node;
		tree_ptr = &wild_choice_tree[node];
	}
	
	if(bound1->lawmin != bound2->lawmin)
	{
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->lawmin;
		
		/* Excess is smaller than cutoff */
		tree_ptr->ptrnode1 = type1;
		
		/* Cutoff = law , ptrnode1 = wild. gen. type. */
		tree_ptr->info = 3 + 4;
		
		/* Wipe chance values  (this probably isn't needed) */
		tree_ptr->chance1 = 0;
		tree_ptr->chance2 = 0;
		
		/* Add new node to decision tree */
		new_node = create_node(node, FALSE);
		
		/* Exit if out of space */
		if (new_node == 0) return(0);
	
		/* reset node to current end of tree */
		node = new_node;
		tree_ptr = &wild_choice_tree[node];
	}
	
	if(bound1->lawmax != bound2->lawmax)
	{
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->lawmax;
		
		/* Excess is larger than cutoff */
		tree_ptr->ptrnode2 = type1;
		
		/* Cutoff = law , ptrnode2 = wild. gen. type. */
		tree_ptr->info = 3 + 8;
		
		/* Wipe chance values  (this probably isn't needed) */
		tree_ptr->chance1 = 0;
		tree_ptr->chance2 = 0;
		
		/* Add new node to decision tree */
		new_node = create_node(node, TRUE);
		
		/* Exit if out of space */
		if (new_node == 0) return(0);
	
		/* reset node to current end of tree */
		node = new_node;
		tree_ptr = &wild_choice_tree[node];
	}
	
	/*
	 * "null" case - don't need the extra node.
	 * Hack - delete extra node and go up one (should be the last node on the
	 * array.)  XXX XXX XXX
	 * Once there - look for "null" type on _one_ branch.
	 * The other branch was previously a link to the now deleted node.
	 * Replace that link with the wilderness gen. type.
	 *
	 * This only works because we know that at least one node was added
	 * to the bottom of the array.  This is why this routine must never
	 * be called with a "null" region the same size as the region to be
	 * added.
	 */
	if (type1 == 0)
	{
		/* Delete last node on array - and move back one. */
		delete_last_node();
		node--;
		tree_ptr = &wild_choice_tree[node];
		
		/* look "left" for null */
		if(tree_ptr->ptrnode1 == 0)
		{
			/* Paranoia - check for both branches null */
			if(tree_ptr->ptrnode2 == 0) return(0);
			
			/* link to wild. gen. type. */
			tree_ptr->ptrnode2 = type2;
			
			/* right branch is to a wild. gen. type - not a node. */
			tree_ptr->info |= 8;
		
			/* Done */
			return(node);
		}
		
		/* look "right" for null */
		if(tree_ptr->ptrnode2 == 0)
		{
			/* Paranoia - check for both branches null */
			if(tree_ptr->ptrnode1 == 0) return(0);
			
			/* link to wild. gen. type. */
			tree_ptr->ptrnode1 = type2;
			
			/* left branch is to a wild. gen. type - not a node. */
			tree_ptr->info |= 4;
		
			/* Done */
			return(node);
		}		
	}
	
	/*
	 * Have two wild. gen. types that want to be in the same region of
	 * parameter space.  This is accomedated by using the "chance" fields.
	 * chance1 of going "left", and chance2 of going "right".  This state
	 * is flagged by having cutoff == 0.
	 */

	/* Set flag for existance of "chance" fields. */
	tree_ptr->cutoff = 0;
	
	/* connect to wild. gen. types */
	tree_ptr->ptrnode1 = type1;
	tree_ptr->ptrnode2 = type2;
	
	/* Set info flag to show both branches are "leaves"*/
	tree_ptr->info = 8 + 4;
	
	/* Look up chances and add to node. */
	tree_ptr->chance1 = wild_gen_data[type1].chance;
	tree_ptr->chance2 = wild_gen_data[type2].chance;

	/* Done */
	return(node);
}

/*
 * This routine compares two bounding boxes and returns true if they are the same.
 */

static bool compare_bounds(wild_bound_box_type *bound1, wild_bound_box_type *bound2)
{
	return((bound2->hgtmin == bound1->hgtmin) &&
		(bound2->hgtmax == bound1->hgtmax) &&
	
		(bound2->popmin == bound1->popmin) &&
		(bound2->popmax == bound1->popmax) &&
	
		(bound2->lawmin == bound1->lawmin) &&
		(bound2->lawmax == bound1->lawmax));
}

/*
 * This function adds a type within a leaf that has a bigger bounding box.
 * This means that the nodes containing the leaf are copied several times
 * until the bounding boxes match - and the node can be added to the leaf.
 *
 * This function is similar to the above one - except that the input is
 * node + branch rather than just node. (This is to simplify the copying
 * function.) 
 */
static u16b inside_leaf(u16b node, u16b type, wild_bound_box_type *bound1,
	 wild_bound_box_type *bound2, bool branch)
{
	
	
	/* The node inserted into the decision tree */
	u16b new_node;
	u16b branch_node;
	wild_choice_tree_type	*tree_ptr;
	
	tree_ptr = &wild_choice_tree[node];
		
	if(bound1->hgtmin != bound2->hgtmin)
	{
		/* Record branch node */
		if(branch)
		{
			branch_node = tree_ptr->ptrnode1;
		}
		else
		{
			branch_node = tree_ptr->ptrnode2;
		
		}
		
		/* Make empty node connected along branch */
		new_node = create_node(node, branch);
		if(new_node == 0) return(0);
		
		/* Reconnect to new node */
		tree_ptr = &wild_choice_tree[new_node];
		tree_ptr->ptrnode1 = branch_node;
		
		/* Copy so that leaf is duplicated */
		if(copy_branch(new_node, TRUE, new_node, FALSE) == 0)
			return(0);
		
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->hgtmin;
		
		/* Cutoff = hgt */
		tree_ptr->info = 1;
	
		/* work out branch to follow */
		branch = FALSE;
	}

	if(bound1->hgtmax != bound2->hgtmax)
	{
		
		/* Record branch node */
		if(branch)
		{
			branch_node = tree_ptr->ptrnode1;
		}
		else
		{
			branch_node = tree_ptr->ptrnode2;
		
		}
		
		/* Make empty node connected along branch */
		new_node = create_node(node, branch);
		if(new_node == 0) return(0);
		
		/* Reconnect to new node */
		tree_ptr = &wild_choice_tree[new_node];
		tree_ptr->ptrnode1 = branch_node;
		
		/* Copy so that leaf is duplicated */
		if(copy_branch(new_node, TRUE, new_node, FALSE) == 0)
			return(0);
		
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->hgtmax;
		
		/* Cutoff = hgt */
		tree_ptr->info = 1;
	
		/* work out branch to follow */
		branch = TRUE;
	}

	if(bound1->popmin != bound2->popmin)
	{
		
		/* Record branch node */
		if(branch)
		{
			branch_node = tree_ptr->ptrnode1;
		}
		else
		{
			branch_node = tree_ptr->ptrnode2;
		
		}
		
		/* Make empty node connected along branch */
		new_node = create_node(node, branch);
		if(new_node == 0) return(0);
		
		/* Reconnect to new node */
		tree_ptr = &wild_choice_tree[new_node];
		tree_ptr->ptrnode1 = branch_node;
		
		/* Copy so that leaf is duplicated */
		if(copy_branch(new_node, TRUE, new_node, FALSE) == 0)
			return(0);
		
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->popmin;
		
		/* Cutoff = pop */
		tree_ptr->info = 2;
	
		/* work out branch to follow */
		branch = FALSE;
	}

	if(bound1->popmax != bound2->popmax)
	{
		
		/* Record branch node */
		if(branch)
		{
			branch_node = tree_ptr->ptrnode1;
		}
		else
		{
			branch_node = tree_ptr->ptrnode2;
		
		}
		
		/* Make empty node connected along branch */
		new_node = create_node(node, branch);
		if(new_node == 0) return(0);
		
		/* Reconnect to new node */
		tree_ptr = &wild_choice_tree[new_node];
		tree_ptr->ptrnode1 = branch_node;
		
		/* Copy so that leaf is duplicated */
		if(copy_branch(new_node, TRUE, new_node, FALSE) == 0)
			return(0);
		
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->popmax;
		
		/* Cutoff = pop */
		tree_ptr->info = 2;
	
		/* work out branch to follow */
		branch = TRUE;
	}
	
	if(bound1->lawmin != bound2->lawmin)
	{
		
		/* Record branch node */
		if(branch)
		{
			branch_node = tree_ptr->ptrnode1;
		}
		else
		{
			branch_node = tree_ptr->ptrnode2;
		
		}
		
		/* Make empty node connected along branch */
		new_node = create_node(node, branch);
		if(new_node == 0) return(0);
		
		/* Reconnect to new node */
		tree_ptr = &wild_choice_tree[new_node];
		tree_ptr->ptrnode1 = branch_node;
		
		/* Copy so that leaf is duplicated */
		if(copy_branch(new_node, TRUE, new_node, FALSE) == 0)
			return(0);
		
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->lawmin;
		
		/* Cutoff = law */
		tree_ptr->info = 3;
	
		/* work out branch to follow */
		branch = FALSE;
	}

	if(bound1->lawmax != bound2->lawmax)
	{
		
		/* Record branch node */
		if(branch)
		{
			branch_node = tree_ptr->ptrnode1;
		}
		else
		{
			branch_node = tree_ptr->ptrnode2;
		
		}
		
		/* Make empty node connected along branch */
		new_node = create_node(node, branch);
		if(new_node == 0) return(0);
		
		/* Reconnect to new node */
		tree_ptr = &wild_choice_tree[new_node];
		tree_ptr->ptrnode1 = branch_node;
		
		/* Copy so that leaf is duplicated */
		if(copy_branch(new_node, TRUE, new_node, FALSE) == 0)
			return(0);
		
		/* Split node along face of region */
		tree_ptr->cutoff = bound2->lawmax;
		
		/* Cutoff = law */
		tree_ptr->info = 3;
	
		/* work out branch to follow */
		branch = TRUE;
	}
	
	/* Finally - merge wild. gen. type with leaf of the same size */
	return(add_node_chance(type, node, branch));
}


/*
 * This function copies the parameter bounds from one variable to another.
 */
static void copy_bounds(wild_bound_box_type *bound1,
	 wild_bound_box_type *bound2)
{
	bound2->hgtmin = bound1->hgtmin;
	bound2->hgtmax = bound1->hgtmax;
	
	bound2->popmin = bound1->popmin;
	bound2->popmax = bound1->popmax;
	
	bound2->lawmin = bound1->lawmin;
	bound2->lawmax = bound1->lawmax;
}


/* 
 * Add a wilderness generation function to the decision tree.
 *
 * There are many special cases to take care of here.  First the
 * current tree is followed until the required region either
 * 1) Is split
 * 2) Is subsumed inside a "leaf" node.
 * 3) Takes over a "null" node.
 *
 * Note: Null nodes exist because no generation routine covers the
 * whole parameter space.  This means that the inital state of the
 * decision tree does not cover every case.  Therefore, as nodes are
 * added - checks are made to see if the region falls ouside of the
 * current "reach" of the tree.
 */
static u16b add_node(wild_bound_box_type *bound,
	 wild_bound_box_type *cur_bound, u16b type, u16b node)
{
	
	/* 
	 * Temp storage of the current bounds and current type bounds 
	 * (Used in splitting a region that overlaps a cutoff)
	 */
	wild_bound_box_type temp_bound1;
	wild_bound_box_type temp_bound2;
	
	u16b oldnode = node;
	
	bool branch = FALSE;
	
	wild_choice_tree_type	*tree_ptr;
	/* Scan tree until hit a leaf or split required region */
	
	/* Use a while loop instead of recursion to follow tree */
	while(1) 
	{
		/* Access Node */
		tree_ptr = &wild_choice_tree[node];
		
		/* If are near end - look at leaves of tree		
		 * 
		 * (cutoff == 0) is used as a flag since it doesn't
		 * split the possibility tree in any useful way.
		 */
		if (tree_ptr->cutoff == 0)
		{
			/* leaf node */	
			return(inside_leaf(oldnode, type, bound, cur_bound, branch));
		}
		else
		{
			/* 
			 * Get lower two bits of type to decribe which of
			 * (hgt,pop,law) cutoff refers to.
			 */
			
			switch(tree_ptr->info & 3)
			{
				case 1: 
				{
					/* Look at height */
					if (tree_ptr->cutoff >= bound->hgtmax)
					{
						branch = TRUE;
						
						cur_bound->hgtmax = tree_ptr->cutoff;
					}
					else if (tree_ptr->cutoff <= bound->hgtmin)
					{
						branch = FALSE;
						
						cur_bound->hgtmin = tree_ptr->cutoff;
					}
					else
					{
						/* make backups before recursion */
						copy_bounds(bound, &temp_bound1);
						copy_bounds(cur_bound, &temp_bound2);
						
						/* upper bound = cutoff */
						temp_bound1.hgtmax = tree_ptr->cutoff;
						temp_bound2.hgtmax = tree_ptr->cutoff;
						
						/* rescan with smaller domain */
						if (!add_node(&temp_bound1, &temp_bound2, type, node))
							return(0);
						
						/* make backups before recursion */
						copy_bounds(bound, &temp_bound1);
						copy_bounds(cur_bound, &temp_bound2);
						
						/* lower bound = cutoff */
						temp_bound1.hgtmin = tree_ptr->cutoff;
						temp_bound2.hgtmin = tree_ptr->cutoff;
						
						/* rescan with smaller domain */
						return(add_node(&temp_bound1, &temp_bound2, type, node));
					}
					break;
				}
				case 2:
				{
					/* Look at population */
					if (tree_ptr->cutoff >= bound->popmax)
					{
						branch = TRUE;
						
						cur_bound->popmax = tree_ptr->cutoff;
					}
					else if (tree_ptr->cutoff <= bound->popmin)
					{
						branch = FALSE;
						
						cur_bound->popmin = tree_ptr->cutoff;
					}
					else
					{
						/* make backups before recursion */
						copy_bounds(bound, &temp_bound1);
						copy_bounds(cur_bound, &temp_bound2);
						
						/* upper bound = cutoff */
						temp_bound1.popmax = tree_ptr->cutoff;
						temp_bound2.popmax = tree_ptr->cutoff;
						
						/* rescan with smaller domain */
						if (!add_node(&temp_bound1, &temp_bound2, type, node))
							return(0);
						
						/* make backups before recursion */
						copy_bounds(bound, &temp_bound1);
						copy_bounds(cur_bound, &temp_bound2);
						
						/* lower bound = cutoff */
						temp_bound1.popmin = tree_ptr->cutoff;
						temp_bound2.popmin = tree_ptr->cutoff;
						
						/* rescan with smaller domain */
						return(add_node(&temp_bound1, &temp_bound2, type, node));
					}
					break;
				}
				case 3:
				{
					/* Look at lawfulness */
					if (tree_ptr->cutoff >= bound->lawmax)
					{
						branch = TRUE;
						
						cur_bound->lawmax = tree_ptr->cutoff;
					}
					else if (tree_ptr->cutoff <= bound->lawmin)
					{
						branch = FALSE;
						
						cur_bound->lawmin = tree_ptr->cutoff;
					}
					else
					{
						/* make backups before recursion */
						copy_bounds(bound, &temp_bound1);
						copy_bounds(cur_bound, &temp_bound2);
						
						/* upper bound = cutoff */
						temp_bound1.lawmax = tree_ptr->cutoff;
						temp_bound2.lawmax = tree_ptr->cutoff;
						
						/* rescan with smaller domain */
						if (!add_node(&temp_bound1, &temp_bound2, type, node))
							return(0);
						
						/* make backups before recursion */
						copy_bounds(bound, &temp_bound1);
						copy_bounds(cur_bound, &temp_bound2);
						
						/* lower bound = cutoff */
						temp_bound1.lawmin = tree_ptr->cutoff;
						temp_bound2.lawmin = tree_ptr->cutoff;
						
						/* rescan with smaller domain */
						return(add_node(&temp_bound1, &temp_bound2, type, node));
					}
					break;
				}
				default:
				{
					msg_format("Info - %d", tree_ptr->info);
					msg_print("Invalid stat chosen!");
					
					break;
				}
			}
		}
		
		
		/* Look at the proper branch of the tree */ 
		if (branch)
		{
			/* Go "left" */
			
			/* See if references leaf node */
			if (tree_ptr->info & 4)
			{
				/* Hit leaf node */
				
				/* store connection */
				oldnode = tree_ptr->ptrnode1;
				
				/* Take care of null case */
				if ((oldnode == 0) && compare_bounds(cur_bound, bound))
				{
					/* simply set the branch to point to the wild. gen. type */
					tree_ptr->ptrnode1 = type;
					
					/* done - don't return zero as can happen with the root node */
					return(1);
				}
				
				/* Make new node */
				node = create_node(node, TRUE);
				if (node == 0) return (0);
				
				return(add_node_inside(node, oldnode, cur_bound, type, bound));
			}
			else
			{
				/* use the while loop to recurse */
				
				oldnode = node;
				node = tree_ptr->ptrnode1;
			}
		}
		else
		{
			/* Go "right" */
			
			/* See if references leaf node */
			if (tree_ptr->info & 8)
			{
				/* Hit leaf node */
				
				/* store connection */
				oldnode = tree_ptr->ptrnode2;
				
				/* Take care of null case */
				if ((oldnode == 0) && compare_bounds(cur_bound, bound))
				{
					/* simply set the branch to point to the wild. gen. type */
					tree_ptr->ptrnode2 = type;
					
					/* done - don't return zero as can happen with the root node */
					return(1);
				}
				
				/* Make new node */
				node = create_node(node, FALSE);
				if (node == 0) return (0);
				
				return(add_node_inside(node, oldnode, cur_bound, type, bound));		
			}
			else
			{
				/* use the while loop to recurse */
				oldnode = node;
				node = tree_ptr->ptrnode2;
			}
		}	
	}
}

/*
 * Initialise the decision tree with the first wilderness generation type.
 */

u16b init_choice_tree(wild_bound_box_type *bound, u16b type)
{
	wild_bound_box_type start_bounds;
	
	/* The decision tree has one (empty) node */
	d_tree_count = 1;
	
	/* 
	 * Set the starting bounds of the decision tree - this covers
	 * the whole parameter space used by the wilderness generation
	 * types.
	 */
	start_bounds.hgtmin = 0;
	start_bounds.hgtmax = 255;
	
	start_bounds.popmin = 0;
	start_bounds.popmax = 255;
	
	start_bounds.lawmin = 0;
	start_bounds.lawmax = 255;
	
	/* Assume first node is cleared by C_MAKE */
	
	/* 
	 * Start the tree off by adding the type within a "null" region covering
	 * the whole parameter space.  (Note this routine requires one empty node.
	 * - that is why d_tree_count starts out as one.)
	 */
	return(add_node_inside(0, 0, &start_bounds, type, bound));
}


u16b add_node_tree_root(wild_bound_box_type *bound, u16b type)
{
	/* default bounds */
	wild_bound_box_type start_bounds;
	
	start_bounds.hgtmin = 0;
	start_bounds.hgtmax = 255;
	
	start_bounds.popmin = 0;
	start_bounds.popmax = 255;
	
	start_bounds.lawmin = 0;
	start_bounds.lawmax = 255;

	/* Add to root of tree */
	return(add_node(bound, &start_bounds, type, 0));
}


/* Testing code - remove later. */
void test_decision_tree(void)
{
	u16b hgt, pop, law;
	
	u16b type;
	
	/* get parameters */
	msg_print("Type in hgt");
	
	hgt = get_quantity(NULL, 255);
	
	msg_print("Type in pop");
	
	pop = get_quantity(NULL, 255);
	
	msg_print("Type in law");
	
	law = get_quantity(NULL, 255);
	
	/* Get value from decision tree */
	type = get_gen_type(hgt, pop, law);

	msg_format("Type returned: %d .", type);
}


/* Delete a wilderness block */
static void del_block(blk_ptr block_ptr)
{
	int x, y;
	int m_idx;
	for (x = 0; x < WILD_BLOCK_SIZE; x++)
	{
		for (y = 0; y < WILD_BLOCK_SIZE; y++)
		{
			/* Clear old terrain data */
			block_ptr[y][x].info = 0;
			block_ptr[y][x].feat = 0;

			/* Delete monster on the square */
			m_idx = block_ptr[y][x].m_idx;

			/* Only delete if one exists */
			if (m_idx)
			{
				delete_monster_idx(m_idx);
				block_ptr[y][x].m_idx = 0;
			}

			/* Delete objects on the square */
			delete_object_location(&block_ptr[y][x]);
		}
	}
}


/*
* Explanation of the plasma fractal algorithm:
*
* A grid of points is created with the properties of a 'height-map'
* This is done by making the corners of the grid have a random value.
* The grid is then subdivided into one with twice the resolution.
* The new points midway between two 'known' points can be calculated
* by taking the average value of the 'known' ones and randomly adding
* or subtracting an amount proportional to the distance between those
* points.  The final 'middle' points of the grid are then calculated
* by averaging all four of the originally 'known' corner points.  An
* random amount is added or subtracted from this to get a value of the
* height at that point.  The scaling factor here is adjusted to the
* slightly larger distance diagonally as compared to orthogonally.
*
* This is then repeated recursively to fill an entire 'height-map'
* A rectangular map is done the same way, except there are different
* scaling factors along the x and y directions.
*
 */

static void frac_block(void)
{
	int cutoff, grd;

	/* fixed point variables- these are stored as 256 x normal value
	* this gives 8 binary places of fractional part + 8 places of normal part*/

	u16b lstep, hstep, i, j, ii, jj, size;

	/* Size is one bigger than normal blocks for speed of algorithm with 2^n + 1 */
	size = WILD_BLOCK_SIZE;

	/* Clear the section */
	for (i = 0; i <= size; i++)
	{
		for (j = 0; j <= size; j++)
		{
			/* MAX_SHORT is a flag for "not done yet" */
			temp_block[j][i] = MAX_SHORT;
		}
	}

	/* Hack - set boundary value midway. */
	cutoff = WILD_BLOCK_SIZE * 128;

	grd = 4 * 256;

	/* Set the corner values just in case grd > size. */
	temp_block[0][0] = cutoff;
	temp_block[0][size] = cutoff;
	temp_block[size][0] = cutoff;
	temp_block[0][size] = cutoff;

	/* Set the middle square to be an open area. */
	temp_block[size / 2][size / 2] = cutoff;

	/* Initialize the step sizes */
	lstep = hstep = size * 256;
	size = size * 256;

	/*
	 * Fill in the square with fractal height data -
	 * like the 'plasma fractal' in fractint.
	 */
	while (lstep > 256)
	{
		/* Halve the step sizes */
		lstep = hstep;
		hstep /= 2;

		/* middle top to bottom.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = 0; j <= size; j += lstep)
			{
				/* cache values of i,j divided by 256 */
				ii = i >> 8;
				jj = j >> 8;
				
				/* only write to points that are "blank" */
				if (temp_block[jj][ii] == MAX_SHORT)
				{
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						temp_block[jj][ii] = randint(WILD_BLOCK_SIZE * 256);
					}
			   		else
					{
						/* Average of left and right points +random bit */
						temp_block[jj][ii] =
						((temp_block[jj][(i - hstep) >> 8] +
						temp_block[jj][(i + hstep) >>8]) >> 1) +
						((randint(lstep) - hstep) >> 1);
					}
				}
			}
		}


		/* middle left to right.*/
		for (j = hstep; j <= size - hstep; j += lstep)
		{
			for (i = 0; i <= size; i += lstep)
		   	{
				/* cache values of i,j / 256 */
				ii = i >> 8;
				jj = j >> 8;
				
				/* only write to points that are "blank" */
				if (temp_block[jj][ii] == MAX_SHORT)
				{				
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						temp_block[jj][ii] = randint(WILD_BLOCK_SIZE * 256);
					}
		   			else
					{
						/* Average of up and down points +random bit */
						temp_block[jj][ii] =
						((temp_block[(j - hstep) >> 8][ii]
						+ temp_block[(j + hstep) >> 8][ii]) >> 1)
						+ ((randint(lstep) - hstep) >> 1);
					}
				}
			}
		}

		/* center.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = hstep; j <= size - hstep; j += lstep)
			{
			   	/* cache values of i,j / 256 */
				ii = i >> 8;
				jj = j >> 8;
				
				/* only write to points that are "blank" */
				if (temp_block[jj][ii] == MAX_SHORT)		
				{
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						temp_block[jj][ii] = randint(WILD_BLOCK_SIZE * 256);
					}
		   			else
					{
						/* average over all four corners + scale by 181 to
						 * reduce the effect of the square grid on the shape of the fractal */
						temp_block[jj][ii] =
						((temp_block[(j - hstep) >> 8][(i - hstep) >> 8]
						+ temp_block[(j + hstep) >> 8][(i - hstep) >> 8]
						+ temp_block[(j - hstep) >> 8][(i + hstep) >> 8]
						+ temp_block[(j + hstep) >> 8][(i + hstep) >> 8]) >> 2)
						+ (((randint(lstep) - hstep) * 181) >> 8);
					}
				}
			}
		}
	}
}

/*
 * This function copies the fractal height map in temp_block
 * to a normal wilderness block.  The height map is modified
 * via a simple function to make terrain. XXX XXX XXX
 */

static void copy_block(blk_ptr block_ptr)
{
	int i, j, element;

	for (j = 0; j < WILD_BLOCK_SIZE; j++)
	{
		for (i = 0; i < WILD_BLOCK_SIZE; i++)
		{
			element = temp_block[j][i];
			if (element < WILD_BLOCK_SIZE * 128)
			{
				/* Grass */
				block_ptr[j][i].info = CAVE_GLOW|CAVE_MARK;
				block_ptr[j][i].feat = FEAT_GRASS;
			}
			else
			{
				/* Trees */
				block_ptr[j][i].info = CAVE_GLOW|CAVE_MARK;
				block_ptr[j][i].feat = FEAT_TREES;
			}
		}
	}
}



/* Make a new block based on the terrain type */
static void gen_block(int x, int y, blk_ptr block_ptr)
{
	u16b w_town;

	/*
	 * Since only grass has been "turned on", this function
	 * is rather simple at the moment.
	 * Eventually there will be a switch statement for each
	 * terrain type.
	 * Even later - most of this will be table driven.
	 */

	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant wilderness blocks */
	Rand_value = wild_grid.wild_seed + x + y * max_wild;

	/* Generate a terrain block*/

	/* Test fractal terrain */
	/* Note that this is very dodgy at the moment.
	 * There is no wilderness */
	frac_block();
	copy_block(block_ptr);

	/* Add roads / river / lava (Not done)*/


	/* Hack -- Use the "complex" RNG */
	Rand_quick = FALSE;

	/* Overlay town */
	w_town = wild[y][x].done.town;

	/* Is there a town? */
	if (w_town)
	{
		/* Is it the right town? */
		if (cur_town != w_town)
		{
			/* Make the town */
			town_gen(w_town);

			init_buildings();
		}

		/* overlay town on wilderness */
		overlay_town(y, x, w_town, block_ptr);
	}

	/* Set the monster generation level */

	/*Hack - just a number. (No themes yet.) */
	monster_level = wild[y][x].done.mon_gen;

	/* Set the object generation level */

	/* Hack - set object level to monster level */
	object_level = wild[y][x].done.mon_gen;

	/* Add monsters. (Not done) */

#ifdef USE_SCRIPT
	if (generate_wilderness_callback(y, x)) return;
#endif /* USE_SCRIPT */
}



/* Allocate all grids around player */
void allocate_all(void)
{
	int x, y;
	blk_ptr block_ptr;

	/* Allocate blocks around player */
	for (x = 0; x < WILD_GRID_SIZE; x++)
	{
		for (y = 0; y < WILD_GRID_SIZE; y++)
		{
			/* The block to use */
			block_ptr = wild_cache[x  + WILD_GRID_SIZE * y];

			/* Delete the block */
			del_block(block_ptr);

			/* Link to the grid */
			wild_grid.block_ptr[y][x] = block_ptr;

			/* Make the new block */
			gen_block(x + wild_grid.x, y + wild_grid.y, block_ptr);
		}
	}
}


/*
 * The following four functions shift the visible
 * section of the wilderness by 16 units.  This is
 * done by scrolling the grid of pointers.
 */

void shift_down(void)
{
	int i, j;
	blk_ptr block_ptr;

	for (i = 0; i < WILD_GRID_SIZE; i++)
	{
		/* The block on the edge */
		block_ptr = wild_grid.block_ptr[0][i];

		/* Delete the block */
		del_block(block_ptr);

		/* Scroll pointers */
		for (j = 1; j < WILD_GRID_SIZE; j++)
		{
			wild_grid.block_ptr[j - 1][i] =
				wild_grid.block_ptr[j][i];
		}

		/* Connect new grid to wilderness */
		wild_grid.block_ptr[WILD_GRID_SIZE - 1][i] = block_ptr;

		/* Make the new block */
		gen_block(i + wild_grid.x,
			WILD_GRID_SIZE - 1 + wild_grid.y, block_ptr);
	}
}

void shift_up(void)
{
	int i, j;
	blk_ptr block_ptr;

	for (i = 0; i < WILD_GRID_SIZE; i++)
	{
		/* The block on the edge */
		block_ptr = wild_grid.block_ptr[WILD_GRID_SIZE - 1][i];

		/* Delete the block */
		del_block(block_ptr);

		/* Scroll pointers */
		for (j = WILD_GRID_SIZE - 1; j > 0; j--)
		{
			wild_grid.block_ptr[j][i] =
				wild_grid.block_ptr[j - 1][i];
		}

		/* Connect new grid to wilderness */
		wild_grid.block_ptr[0][i] = block_ptr;

		/* Make the new block */
		gen_block(i + wild_grid.x, wild_grid.y, block_ptr);
	}
}

void shift_right(void)
{
	int i, j;
	blk_ptr block_ptr;

	for (j = 0; j < WILD_GRID_SIZE; j++)
	{
		/* The block on the edge */
		block_ptr = wild_grid.block_ptr[j][0];

		/* Delete the block */
		del_block(block_ptr);

		/* Scroll pointers */
		for (i = 1; i < WILD_GRID_SIZE; i++)
		{
			wild_grid.block_ptr[j][i - 1] =
				wild_grid.block_ptr[j][i];
		}

		/* Connect new grid to wilderness */
		wild_grid.block_ptr[j][WILD_GRID_SIZE - 1] = block_ptr;

		/* Make the new block */
		gen_block(WILD_GRID_SIZE - 1 + wild_grid.x,
			j + wild_grid.y, block_ptr);
	}
}


void shift_left(void)
{
	int i, j;
	blk_ptr block_ptr;

	for (j = 0; j < WILD_GRID_SIZE; j++)
	{
		/* The block on the edge */
		block_ptr = wild_grid.block_ptr[j][WILD_GRID_SIZE - 1];

		/* Delete the block */
		del_block(block_ptr);

		/* Scroll pointers */
		for (i = WILD_GRID_SIZE - 1; i > 0; i--)
		{
			wild_grid.block_ptr[j][i] =
				wild_grid.block_ptr[j][i - 1];
		}

		/* Connect new grid to wilderness */
		wild_grid.block_ptr[j][0] = block_ptr;

		/* Make the new block */
		gen_block(wild_grid.x, j + wild_grid.y, block_ptr);
	}
}

/*
 * Centre grid of wilderness blocks around player.
 * This must be called after the player moves in the wilderness.
 * If the player is just walking around, all that needs to be done is
 * to scroll the grid of pointers - not recalculate them all.
 * However, when the player teleports, things have to stay as is.
 */
void move_wild(void)
{
	int x, y, dx, dy;

	/* Get upper left hand block in grid. */

	/* Divide by 16 to get block from (x,y) coord + shift it.*/
	x = ((u16b) p_ptr->wilderness_x>>4) - WILD_GRID_SIZE / 2;
	y = ((u16b) p_ptr->wilderness_y>>4) - WILD_GRID_SIZE / 2;

	/* Move if out of bounds */
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x + WILD_GRID_SIZE > max_wild) x = max_wild - WILD_GRID_SIZE;
	if (y + WILD_GRID_SIZE > max_wild) y = max_wild - WILD_GRID_SIZE;

	/* Hack - set town */
	p_ptr->town_num = wild[p_ptr->wilderness_y >> 4]
		[p_ptr->wilderness_x >> 4].done.town;

	/*
	 * Hack - check to see if first block is the same.
	 * If so, the grid doesn't need to move.
	 */
	if ((x == wild_grid.x) && (y == wild_grid.y)) return;

	dx = x - wild_grid.x;
	dy = y - wild_grid.y;

	/* Store in upper left hand corner. */
	wild_grid.y = y;

	/* Recalculate boundaries */
	wild_grid.y_max = (y + WILD_GRID_SIZE) << 4;
	wild_grid.y_min = y << 4;

	/* Shift in only a small discrepency */
	if (abs(dy) == 1)
	{
		if (dy == 1) shift_down();
		else shift_up();
	}
	else if (dy)
	{
		/* Too large of a shift */

		/* Store in upper left hand corner. */
		wild_grid.x = x;

		/* Recalculate boundaries */
		wild_grid.x_max = (x + WILD_GRID_SIZE) << 4;
		wild_grid.x_min = x << 4;

		allocate_all();
		return;
	}

	/* Store in upper left hand corner. */
	wild_grid.x = x;

	/* Recalculate boundaries */
	wild_grid.x_max = (x + WILD_GRID_SIZE) << 4;
	wild_grid.x_min = x << 4;

	/* Shift in only a small discrepency */
	if (abs(dx) == 1)
	{
		if (dx == 1) shift_right();
		else shift_left();

		/* Done */
		return;
	}

	if (dx)
	{
		/* Too big of a jump */
		allocate_all();
	}
}



/* this routine probably should be an inline function or a macro. */
static void store_hgtmap(int x, int y, int val)
{
	/* bounds checking */
	if (val < 0) val = 0;
	if ((val >> 4) >= max_wild) val = (max_wild << 4) - 1;
	
	/* Save distribution information */	
	wild_temp_dist[val >> 4] = 1;
	
	/* store the value in height-map format */
	wild[y][x].gen.hgt_map = val;

	return;
}

/*
 * This function creates the first of the three parameters used to generate
 * the wilderness.  This is done by making a plasma fractal.  The distribution
 * of the values in the height map is stored so that they can be scaled to
 * generate a wilderness with an even distribution of terrain.
 */
static void create_hgt_map(void)
{
	int grd;

	/* fixed point variables- these are stored as 16 x normal value
	* this gives 4 binary places of fractional part + 12 places of normal part*/

	int lstep, hstep, i, j, ii, jj, size;
	
	/* Size is one bigger than normal blocks for speed of algorithm with 2^n + 1 */
	size = max_wild - 1;

	/* Clear the section */
	for (i = 0; i <= size; i++)
	{
		for (j = 0; j <= size; j++)
		{
			/* MAX_SHORT is a flag for "not done yet" */
			wild[j][i].gen.hgt_map = MAX_SHORT;
		}
		
		/* Clear distribution information */
		wild_temp_dist[i] = 0;
	}

	grd = 4 * 16;

	/* Set the corner values just in case grd > size. */
	store_hgtmap(0, 0, rand_int(size));
	store_hgtmap(size, 0, rand_int(size));
	store_hgtmap(0, size, rand_int(size));
	store_hgtmap(size, size, rand_int(size));

	/* Initialize the step sizes */
	lstep = hstep = size * 16;
	size = size * 16;

	/*
	 * Fill in the square with fractal height data -
	 * like the 'plasma fractal' in fractint.
	 */
	while (lstep > 16)
	{
		/* Halve the step sizes */
		lstep = hstep;
		hstep /= 2;

		/* middle top to bottom.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = 0; j <= size; j += lstep)
			{
				/* cache values of i,j divided by 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.hgt_map == MAX_SHORT)
				{
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_hgtmap(ii, jj, randint(max_wild * 16));
					}
			   		else
					{
						/* Average of left and right points +random bit */
						store_hgtmap(ii, jj,
						((wild[jj][(i - hstep) >> 4].gen.hgt_map +
						wild[jj][(i + hstep) >> 4].gen.hgt_map) >> 1) +
						((randint(lstep) - hstep) >> 1));
					}
				}
			}
		}


		/* middle left to right.*/
		for (j = hstep; j <= size - hstep; j += lstep)
		{
			for (i = 0; i <= size; i += lstep)
		   	{
				/* cache values of i,j / 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.hgt_map == MAX_SHORT)
				{				
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_hgtmap(ii, jj, randint(max_wild * 16));
					}
		   			else
					{
						/* Average of up and down points +random bit */
						store_hgtmap(ii, jj,
						((wild[(j - hstep) >> 4][ii].gen.hgt_map
						+ wild[(j + hstep) >> 4][ii].gen.hgt_map) >> 1)
						+ ((randint(lstep) - hstep) >> 1));
					}
				}
			}
		}

		/* center.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = hstep; j <= size - hstep; j += lstep)
			{
			   	/* cache values of i,j / 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.hgt_map == MAX_SHORT)		
				{
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_hgtmap(ii, jj, randint(max_wild * 16));
					}
		   			else
					{
						/* average over all four corners + scale by 181 to
						 * reduce the effect of the square grid on the shape of the fractal */
						store_hgtmap(ii, jj,
						((wild[(j - hstep) >> 4][(i - hstep) >> 4].gen.hgt_map
						+ wild[(j + hstep) >> 4][(i - hstep) >> 4].gen.hgt_map
						+ wild[(j - hstep) >> 4][(i + hstep) >> 4].gen.hgt_map
						+ wild[(j + hstep) >> 4][(i + hstep) >> 4].gen.hgt_map) >> 2)
						+ (((randint(lstep) - hstep) * 181) >> 8));
					}
				}
			}
		}
	}
}


/* this routine probably should be an inline function or a macro. */
static void store_popmap(int x, int y, int val, u16b sea)
{
	/* bounds checking */
	if (val < 0) val = 0;
	if ((val >> 4) >= max_wild) val = (max_wild << 4) - 1;
	
	/* Save distribution information (only if not below sea level)*/	
	if (wild[y][x].gen.hgt_map > sea) wild_temp_dist[val >> 4] = 1;
	
	/* store the value in height-map format */
	wild[y][x].gen.pop_map = val;

	return;
}

/*
 * This function creates the second of the three parameters used to generate
 * the wilderness.  This is done by making a plasma fractal.
 */
static void create_pop_map(u16b sea)
{
	int grd;

	/* fixed point variables- these are stored as 16 x normal value
	* this gives 4 binary places of fractional part + 12 places of normal part*/

	int lstep, hstep, i, j, ii, jj, size;

	/* Size is one bigger than normal blocks for speed of algorithm with 2^n + 1 */
	size = max_wild - 1;

	/* Clear the section */
	for (i = 0; i <= size; i++)
	{
		for (j = 0; j <= size; j++)
		{
			/* MAX_SHORT is a flag for "not done yet" */
			wild[j][i].gen.pop_map = MAX_SHORT;
		}
		
		/* Clear distribution information */
		wild_temp_dist[i] = 0;
	}

	grd = 4 * 16;

	/* Set the corner values just in case grd > size. */
	store_popmap(0, 0, rand_int(size), sea);
	store_popmap(size, 0, rand_int(size), sea);
	store_popmap(0, size, rand_int(size), sea);
	store_popmap(size, size, rand_int(size), sea);

	/* Initialize the step sizes */
	lstep = hstep = size * 16;
	size = size * 16;

	/*
	 * Fill in the square with fractal height data -
	 * like the 'plasma fractal' in fractint.
	 */
	while (lstep > 16)
	{
		/* Halve the step sizes */
		lstep = hstep;
		hstep /= 2;

		/* middle top to bottom.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = 0; j <= size; j += lstep)
			{
				/* cache values of i,j divided by 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.pop_map == MAX_SHORT)
				{
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_popmap(ii, jj, randint(max_wild * 16), sea);
					}
			   		else
					{
						/* Average of left and right points +random bit */
						store_popmap(ii, jj,
						((wild[jj][(i - hstep) >> 4].gen.pop_map +
						wild[jj][(i + hstep) >> 4].gen.pop_map) >> 1) +
						((randint(lstep) - hstep) >> 1), sea);
					}
				}
			}
		}


		/* middle left to right.*/
		for (j = hstep; j <= size - hstep; j += lstep)
		{
			for (i = 0; i <= size; i += lstep)
		   	{
				/* cache values of i,j / 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.pop_map == MAX_SHORT)
				{				
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_popmap(ii, jj, randint(max_wild * 16), sea);
					}
		   			else
					{
						/* Average of up and down points +random bit */
						store_popmap(ii, jj,
						((wild[(j - hstep) >> 4][ii].gen.pop_map
						+ wild[(j + hstep) >> 4][ii].gen.pop_map) >> 1)
						+ ((randint(lstep) - hstep) >> 1), sea);
					}
				}
			}
		}

		/* center.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = hstep; j <= size - hstep; j += lstep)
			{
			   	/* cache values of i,j / 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.pop_map == MAX_SHORT)		
				{
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_popmap(ii, jj, randint(max_wild * 16), sea);
					}
		   			else
					{
						/* average over all four corners + scale by 181 to
						 * reduce the effect of the square grid on the shape of the fractal */
						store_popmap(ii, jj,
						((wild[(j - hstep) >> 4][(i - hstep) >> 4].gen.pop_map
						+ wild[(j + hstep) >> 4][(i - hstep) >> 4].gen.pop_map
						+ wild[(j - hstep) >> 4][(i + hstep) >> 4].gen.pop_map
						+ wild[(j + hstep) >> 4][(i + hstep) >> 4].gen.pop_map) >> 2)
						+ (((randint(lstep) - hstep) * 181) >> 8), sea);
					}
				}
			}
		}
	}
}


/* this routine probably should be an inline function or a macro. */
static void store_lawmap(int x, int y, int val, u16b sea)
{	
	/* bounds checking */
	if (val < 0) val = 0;
	if ((val >> 4) >= max_wild) val = (max_wild << 4) - 1;
	
	/* Save distribution information (only if not below sea level)*/	
	if (wild[y][x].gen.hgt_map > sea) wild_temp_dist[val >> 4] = 1;
	
	/* store the value in height-map format */
	wild[y][x].gen.law_map = val;

	return;
}

/*
 * This function creates the third of the three parameters used to generate
 * the wilderness.  This is done by making a plasma fractal.
 */
static void create_law_map(u16b sea)
{
	int grd;

	/* fixed point variables- these are stored as 16 x normal value
	* this gives 4 binary places of fractional part + 12 places of normal part*/

	int lstep, hstep, i, j, ii, jj, size;

	/* Size is one bigger than normal blocks for speed of algorithm with 2^n + 1 */
	size = max_wild - 1;

	/* Clear the section */
	for (i = 0; i <= size; i++)
	{
		for (j = 0; j <= size; j++)
		{
			/* MAX_SHORT is a flag for "not done yet" */
			wild[j][i].gen.law_map = MAX_SHORT;
		}
		
		/* Clear distribution information */
		wild_temp_dist[i] = 0;
	}

	grd = 4 * 16;

	/* Set the corner values just in case grd > size. */
	store_lawmap(0, 0, rand_int(size), sea);
	store_lawmap(size, 0, rand_int(size), sea);
	store_lawmap(0, size, rand_int(size), sea);
	store_lawmap(size, size, rand_int(size), sea);

	/* Initialize the step sizes */
	lstep = hstep = size * 16;
	size = size * 16;

	/*
	 * Fill in the square with fractal height data -
	 * like the 'plasma fractal' in fractint.
	 */
	while (lstep > 16)
	{
		/* Halve the step sizes */
		lstep = hstep;
		hstep /= 2;

		/* middle top to bottom.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = 0; j <= size; j += lstep)
			{
				/* cache values of i,j divided by 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.law_map == MAX_SHORT)
				{
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_lawmap(ii, jj, randint(max_wild * 16), sea);
					}
			   		else
					{
						/* Average of left and right points +random bit */
						store_lawmap(ii, jj,
						((wild[jj][(i - hstep) >> 4].gen.law_map +
						wild[jj][(i + hstep) >> 4].gen.law_map) >> 1) +
						((randint(lstep) - hstep) >> 1), sea);
					}
				}
			}
		}


		/* middle left to right.*/
		for (j = hstep; j <= size - hstep; j += lstep)
		{
			for (i = 0; i <= size; i += lstep)
		   	{
				/* cache values of i,j / 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.law_map == MAX_SHORT)
				{				
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_lawmap(ii, jj, randint(max_wild * 16), sea);
					}
		   			else
					{
						/* Average of up and down points +random bit */
						store_lawmap(ii, jj,
						((wild[(j - hstep) >> 4][ii].gen.law_map
						+ wild[(j + hstep) >> 4][ii].gen.law_map) >> 1)
						+ ((randint(lstep) - hstep) >> 1), sea);
					}
				}
			}
		}

		/* center.*/
		for (i = hstep; i <= size - hstep; i += lstep)
		{
			for (j = hstep; j <= size - hstep; j += lstep)
			{
			   	/* cache values of i,j / 16 */
				ii = i >> 4;
				jj = j >> 4;
				
				/* only write to points that are "blank" */
				if (wild[jj][ii].gen.law_map == MAX_SHORT)
				{
					if (hstep > grd)
					{
						/* If greater than 'grid' level then is random */
						store_lawmap(ii, jj, randint(max_wild * 16), sea);
					}
		   			else
					{
						/* average over all four corners + scale by 181 to
						 * reduce the effect of the square grid on the shape of the fractal */
						store_lawmap(ii, jj,
						((wild[(j - hstep) >> 4][(i - hstep) >> 4].gen.law_map
						+ wild[(j + hstep) >> 4][(i - hstep) >> 4].gen.law_map
						+ wild[(j - hstep) >> 4][(i + hstep) >> 4].gen.law_map
						+ wild[(j + hstep) >> 4][(i + hstep) >> 4].gen.law_map) >> 2)
						+ (((randint(lstep) - hstep) * 181) >> 8), sea);
					}
				}
			}
		}
	}
}
/*
 * Create the wilderness - dodgy function now.  Much to be done.
 * Later this will use a fractal method to make the wilderness.
 * Towns / dungeons yet.
 * No monsters yet.
 * No roads/ rivers yet.
 */

void create_wilderness(void)
{
	int i,j;
	
	u16b hgt_min, hgt_max, pop_min, pop_max, law_min, law_max;
	u16b sea_level;
	
	/* Create "height" information of wilderness */
	create_hgt_map();
	
	/* work out extremes of height so it can be scaled. */
	hgt_min = hgt_max = pop_min = pop_max = law_min = law_max = 0;
	
	/* minimum height */
	for (i = 0; i < max_wild; i++)
	{
		if (wild_temp_dist[i] != 0)
		{
			hgt_min = i;
			break;
		}
	}
	
	/* maximum height */
	for (i = max_wild - 1; i >= 0; i--)
	{
		if (wild_temp_dist[i] != 0)
		{
			hgt_max = i;
			break;
		}
	}
	
	/* The sea covers 1/3 of the wilderness */
	sea_level = hgt_min + (hgt_max - hgt_min) / 3;
	
	/* create "population density" information */
	create_pop_map(sea_level);
	
	/* work out extremes of population so it can be scaled. */
	
	/* minimum population */
	for (i = 0; i < max_wild; i++)
	{
		if (wild_temp_dist[i] != 0)
		{
			pop_min = i;
			break;
		}
	}
	
	/* maximum population */
	for (i = max_wild - 1; i >= 0; i--)
	{
		if (wild_temp_dist[i] != 0)
		{
			pop_max = i;
			break;
		}
	}
	
	create_law_map(sea_level);
	
	/* work out extremes of "lawfulness" so it can be scaled. */
	
	/* minimum lawfulness */
	for (i = 0; i < max_wild; i++)
	{
		if (wild_temp_dist[i] != 0)
		{
			law_min = i;
			break;
		}
	}
	
	/* maximum lawfulness */
	for (i = max_wild - 1; i >= 0; i--)
	{
		if (wild_temp_dist[i] != 0)
		{
			law_max = i;
			break;
		}
	}
	
	/* Fill wilderness with grass */
	/* This will be replaced with a more inteligent routine later */
	for (i = 0; i < max_wild; i++)
	{
		for (j = 0; j < max_wild; j++)
		{
			/* All grass */
			wild[j][i].done.wild = 1;

			/* No town yet */
			wild[j][i].done.town = 0;

			/* No rivers / roads / all unknown */
			wild[j][i].done.info = 0;

			/* No monsters */
			wild[j][i].done.mon_gen = 0;
		}
	}

	/* Hack - Reset player position to centre. */
	p_ptr->wilderness_x = 32 * 16;
	p_ptr->wilderness_y = 32 * 16;

	if (!dun_level)
	{
		px = p_ptr->wilderness_x;
		py = p_ptr->wilderness_y;

		/* Determine number of panels */
		max_panel_rows = max_wild * 16 * 2 - 2;
		max_panel_cols = max_wild * 16 * 2 - 2;

		/* Assume illegal panel */
		panel_row = max_panel_rows;
		panel_col = max_panel_cols;

		/* Hack - delete all items / monsters in wilderness */
		wipe_o_list();
		wipe_m_list();
	}

	/* Clear cache */
	wild_grid.cache_count = 0;

	/* Fix location of grid */

	/* Hack - set the coords to crazy values so move_wild() works. */
	wild_grid.x = -1;
	wild_grid.y = -1;

	/* A dodgy town */
	init_towns();

	/* Refresh random number seed */
	wild_grid.wild_seed = rand_int(0x10000000);
	
	/* Free up memory used to create the wilderness */
	#if 0
	C_FREE(wild_choice_tree, max_w_node, wild_choice_tree_type);
	C_FREE(wild_temp_dist, max_wild, byte);
	#endif
}
