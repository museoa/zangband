/* CVS: Last edit by $Author$ on $Date$ */
/* File:field.c */

/* Purpose: Field code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

/*
 * Excise a field from a stacks
 */
void excise_field_idx(int fld_idx)
{
	s16b this_f_idx, next_f_idx = 0;

	s16b prev_f_idx = 0;

	/* Dungeon */
	cave_type *c_ptr;

	/* Field */
	field_type *j_ptr = &fld_list[fld_idx];

	int y = j_ptr->fy;
	int x = j_ptr->fx;


	/* Exit if is a "dummy" object */
	if ((x == 0) && (y == 0)) return;

	/* Grid */
	c_ptr = area(y, x);

	/* Scan all fields in the grid */
	for (this_f_idx = c_ptr->fld_idx; this_f_idx; this_f_idx = next_f_idx)
	{
		field_type *f_ptr;

		/* Acquire field */
		f_ptr = &fld_list[this_f_idx];

		/* Acquire next field */
		next_f_idx = f_ptr->next_f_idx;

		/* Done */
		if (this_f_idx == fld_idx)
		{
			/* No previous */
			if (prev_f_idx == 0)
			{
				/* Remove from list */
				c_ptr->fld_idx = next_f_idx;
			}

			/* Real previous */
			else
			{
				field_type *k_ptr;

				/* Previous field */
				k_ptr = &fld_list[prev_f_idx];

				/* Remove from list */
				k_ptr->next_f_idx = next_f_idx;
			}

			/* Forget next pointer */
			f_ptr->next_f_idx = 0;

			/* Done */
			break;
		}

		/* Save prev_f_idx */
		prev_f_idx = this_f_idx;
	}
}


/*
 * Delete a dungeon field
 *
 * Handle "lists" of fields correctly.
 */
void delete_field_idx(int fld_idx)
{
	/* Field */
	field_type *j_ptr = &fld_list[fld_idx];

	/* Dungeon floor */
	int y, x;

	/* Location */
	y = j_ptr->fy;
	x = j_ptr->fx;

	/* Excise */
	excise_field_idx(fld_idx);

	/* Refuse "illegal" locations */
	if (in_bounds(y, x))
	{
		/* Note the spot */
		note_spot(y, x);
		
		/* Visual update */
		lite_spot(y, x);
	}

#ifdef USE_SCRIPT
	field_delete_callback(j_ptr);
#endif /* USE_SCRIPT */

	/* Wipe the field */
	field_wipe(j_ptr);

	/* Count fields */
	fld_cnt--;
}

/*
 * Deletes the list of fields attached to something.
 */
void delete_field_aux(s16b *fld_idx_ptr)
{
	s16b this_f_idx, next_f_idx = 0;

	/* Scan all fields in the grid */
	for (this_f_idx = *fld_idx_ptr; this_f_idx; this_f_idx = next_f_idx)
	{
		field_type *f_ptr;

		/* Acquire field */
		f_ptr = &fld_list[this_f_idx];

		/* Acquire next field */
		next_f_idx = f_ptr->next_f_idx;

#ifdef USE_SCRIPT
		field_delete_callback(f_ptr);
#endif /* USE_SCRIPT */

		/* Wipe the field */
		field_wipe(f_ptr);

		/* Count fields */
		fld_cnt--;
	}

	/* Nothing left */
	*fld_idx_ptr = 0;
}


/*
 * Deletes all fields at given location
 */
void delete_field(int y, int x)
{
	cave_type *c_ptr;

	/* Refuse "illegal" locations */
	if (!in_bounds(y, x)) return;

	/* Grid */
	c_ptr = area(y,x);

	delete_field_aux(&(c_ptr->fld_idx));

	/* Note the spot */
	note_spot(y, x);
	
	/* Visual update */
	lite_spot(y, x);
}

/*
 * Deletes all fields at given location
 */
void delete_field_location(cave_type *c_ptr)
{
	delete_field_aux(&c_ptr->fld_idx);
}


/*
 * Move a field from index i1 to index i2 in the field list
 */
static void compact_fields_aux(int i1, int i2)
{
	int i;

	cave_type *c_ptr;

	field_type *f_ptr;

	int y, x;


	/* Do nothing */
	if (i1 == i2) return;


	/* Repair fields */
	for (i = 1; i < fld_max; i++)
	{
		/* Acquire field */
		f_ptr = &fld_list[i];

		/* Skip "dead" fields */
		if (!f_ptr->t_idx) continue;

		/* Repair "next" pointers */
		if (f_ptr->next_f_idx == i1)
		{
			/* Repair */
			f_ptr->next_f_idx = i2;
		}
	}


	/* Acquire object */
	f_ptr = &fld_list[i1];

	/* Acquire location */
	y = f_ptr->fy;
	x = f_ptr->fx;

	/* If the square exists */
	if ((y) && (x))
	{
		/* Acquire grid */
		c_ptr = area(y, x);

		/* Repair grid */
		if (c_ptr->fld_idx == i1)
		{
			/* Repair */
			c_ptr->fld_idx = i2;
		}
	}


	/* Structure copy */
	fld_list[i2] = fld_list[i1];

#ifdef USE_SCRIPT
	field_delete_callback(f_ptr);
#endif /* USE_SCRIPT */

	/* Wipe the hole */
	field_wipe(f_ptr);
}


/*
 * Compact and Reorder the field list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" field, we base the saving throw on a
 * combination of field level, distance from player, and current
 * "desperation".
 *
 * After "compacting" (if needed), we "reorder" the fields into a more
 * compact order, and we reset the allocation info, and the "live" array.
 */
void compact_fields(int size)
{
	int y, x, num, cnt;
	int cur_lev, cur_dis, chance;
	s16b i;
	
	s16b *fld_ptr;


	/* Compact */
	if (size)
	{
		/* Message */
		msg_print("Compacting fields...");

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
	}


	/* Compact at least 'size' fields */
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Examine the fields */
		for (i = 1; i < fld_max; i++)
		{
			field_type *f_ptr = &fld_list[i];

			byte fld_level = 100;

			/* Skip dead fields */
			if (!f_ptr->t_idx) continue;

			/* Get pointer to field index */
			fld_ptr = field_find(i);

			/* Get compaction "value" */
			(void) field_hook_single(fld_ptr, FIELD_ACT_COMPACT,
				 (void *) &fld_level);

			/* Hack -- High level fields start out "immune" */
			if (fld_level > cur_lev) continue;

			/* Get the location */
			y = f_ptr->fy;
			x = f_ptr->fx;

			/* Nearby fields start out "immune" */
			if ((cur_dis > 0) && (distance(py, px, y, x) < cur_dis)) continue;

			/* Saving throw */
			chance = 90;

			/* Only compact permanent fields in an emergency. */
			/* Add code here. ##### */

			/* Apply the saving throw */
			if (rand_int(100) < chance) continue;

			/* Call completion routine */
			if (field_hook_single(fld_ptr, FIELD_ACT_EXIT, NULL))
			{
				/* It didn't delete itself (naughty) so we do it now */
				delete_field_aux(fld_ptr);
			}

			/* Count it */
			num++;
		}
	}


	/* Excise dead fields (backwards!) */
	for (i = fld_max - 1; i >= 1; i--)
	{
		field_type *f_ptr = &fld_list[i];

		/* Skip real fields */
		if (f_ptr->t_idx) continue;

		/* Move last object into open hole */
		compact_fields_aux(fld_max - 1, i);

		/* Compress "fld_max" */
		fld_max--;
	}
}


/*
 * Delete all the fields when player leaves the level
 *
 * Note -- we do NOT visually reflect these (irrelevant) changes
 *
 * Hack -- we clear the "c_ptr->fld_idx" field for every grid
 * since we know we are clearing every field.  Technically,
 * we only clear those fields for grids/monsters containing
 * fields, and we clear it once for every such field.
 */
void wipe_f_list(void)
{
	int i;

	cave_type *c_ptr;

	int y, x;

	/* Delete the existing fields */
	for (i = 1; i < fld_max; i++)
	{
		field_type *f_ptr = &fld_list[i];

		/* Skip dead objects */
		if (!f_ptr->t_idx) continue;

		/* Access location */
		y = f_ptr->fy;
		x = f_ptr->fx;

		/* If exists at a square */
		if ((y) && (x))
		{
			/* Access grid */
			c_ptr = area(y,x);

			/* Hack -- see above */
			c_ptr->fld_idx = 0;
		}

#ifdef USE_SCRIPT
		field_delete_callback(f_ptr);
#endif /* USE_SCRIPT */

		/* Wipe the field */
		field_wipe(f_ptr);
	}

	/* Reset "fld_max" */
	fld_max = 1;

	/* Reset "fld_cnt" */
	fld_cnt = 0;
}


/*
 * Acquires and returns the index of a "free" field.
 *
 * This routine should almost never fail, but in case it does,
 * we must be sure to handle "failure" of this routine.
 */
s16b f_pop(void)
{
	int i;


	/* Initial allocation */
	if (fld_max < max_fld_idx)
	{
		/* Get next space */
		i = fld_max;

		/* Expand field array */
		fld_max++;

		/* Count fields */
		fld_cnt++;

		/* Use this field */
		return (i);
	}


	/* Recycle dead fields */
	for (i = 1; i < fld_max; i++)
	{
		field_type *f_ptr;

		/* Acquire field */
		f_ptr = &fld_list[i];

		/* Skip live fields */
		if (f_ptr->t_idx) continue;

		/* Count fields */
		fld_cnt++;

		/* Use this field */
		return (i);
	}


	/* Warn the player (except during dungeon creation) */
	if (character_dungeon) msg_print("Too many fields!");

	/* Oops */
	return (0);
}

/*
 * Wipe an field clean.
 */
void field_wipe(field_type *f_ptr)
{
	/* Wipe the structure */
	(void)WIPE(f_ptr, field_type);
}

/*
 * Prepare a field based on an existing one
 */
void field_copy(field_type *f_ptr, field_type *j_ptr)
{
	/* Copy the structure */
	COPY(f_ptr, j_ptr, field_type);

#ifdef USE_SCRIPT
	j_ptr->python = field_copy_callback(f_ptr, j_ptr);
#endif /* USE_SCRIPT */
}



/*
 * Add a field to a list of fields.
 * The list is sorted so that the field with the highest
 * priority is on top.
 * f_ptr is the field to add.
 * *fld_idx2 is a pointer to the head of the list of fields.
 */
s16b field_add(field_type *f_ptr, s16b *fld_idx2)
{
	s16b fld_idx = *fld_idx2;
	s16b new_idx;
	field_type *j_ptr = &fld_list[*fld_idx2];
	
	bool merge = FALSE;
	
	s32b counter;

	/* Add to a list of fields */
	while ((*fld_idx2) && (j_ptr->priority > f_ptr->priority))
	{
		/* Look for fields that can be merged */
		if ((f_ptr->info & FIELD_INFO_MERGE) && (f_ptr->t_idx == j_ptr->t_idx))
		{
			/* Set merging flag */
			merge = TRUE;
			break;
		}
		
		/* Save old field number */
		fld_idx = *fld_idx2;

		/* Get next field in the list */
		fld_idx2 = &(j_ptr->next_f_idx);
		
		/* Update the pointer */
		j_ptr = &fld_list[*fld_idx2];
	}

	/* An Empty list is easy */
	if (!(*fld_idx2))
	{
		new_idx = f_pop();

		if (!new_idx) return (0);

		/* Store into location */
		*fld_idx2 = new_idx;
		
		/* Zero end link */
		f_ptr->next_f_idx = 0;

		/* Move field to location */
		field_copy(&fld_list[new_idx], f_ptr);
		
		/* Hack - save the location */
		hack_fld_ptr = fld_idx2;

		return (new_idx);
	}
	
	if (merge)
	{
		/* Merge the two together */
		counter = j_ptr->counter + f_ptr->counter;
		
		/* Bounds checking */
		if (counter > MAX_SHORT) counter = MAX_SHORT;
		
		/* Store in new counter */
		j_ptr->counter = (s16b)counter;
		
		return (*fld_idx2);
	}

	/* Bump a node out of the way */
	else
	{
		/*
		 * fld_idx points to node before this one.
		 * *fld_idx2 points to the node after this one.
		 */

		/* The next node */
		f_ptr->next_f_idx = *fld_idx2;

		/* Get new node in list */
		new_idx = f_pop();

		if (!new_idx) return (0);
		
		/* Move field to location */
		field_copy(&fld_list[new_idx], f_ptr);

		/* Make node before this one, point to this one. */
		fld_list[fld_idx].next_f_idx = new_idx;
		
		/* Hack - save the location */
		hack_fld_ptr = fld_idx2;

		return (new_idx);
	}
}

/*
 * Sort a list of fields so that they are in priority order.
 *
 * Since the linked list is one-way. A simple bubble sort is
 * used.  If this is too slow, perhaps hooks to the quicksort
 * routine should be made.
 *
 * This routine is so slow - it should really never be used.
 * If at all possible - use a merge sort based on the above code.
 */
void field_sort_priority(s16b *fld_idx_ptr)
{
	s16b *i_ptr, *j_ptr;

	s16b temp;
	bool swapped = TRUE;

	/* Paranoia - exit if list is empty */
	if (!(*fld_idx_ptr)) return;

	/* Keep scanning until no more changes */
	while (swapped)
	{
		/* Initialize */
		i_ptr = fld_idx_ptr;
		j_ptr = &(fld_list[*i_ptr].next_f_idx);

		swapped = FALSE;

		/* Scan the list until the end */
		while (*j_ptr)
		{
			/* Check to see if in order */
			if (fld_list[*i_ptr].priority < fld_list[*j_ptr].priority)
			{
				/* swap the links */
				temp = *i_ptr;
				*i_ptr = *j_ptr;
				*j_ptr = temp;
				swapped = TRUE;
			}

			/* Advance the pointers */
			i_ptr = j_ptr;
			j_ptr = &(fld_list[*j_ptr].next_f_idx);
		}
	}
}


/*
 * Prepare a field based on kind.
 */
void field_prep(field_type *f_ptr, int t_idx)
{
	field_thaum *t_ptr = &t_info[t_idx];
	int i;

	/* Clear the record */
	field_wipe(f_ptr);

	/* Save the kind index */
	f_ptr->t_idx = t_idx;

	/* Get pointer to thaum type. */
	t_ptr = &t_info[t_idx];

	/* What it looks like */
	f_ptr->f_attr = t_ptr->f_attr;
	f_ptr->f_char = t_ptr->f_char;

	f_ptr->priority = t_ptr->priority;
	f_ptr->counter = t_ptr->count_init;
	f_ptr->info = t_ptr->info;

	for (i = 0; i < 8; i++)
	{
		/* Store the value */
		f_ptr->data[i] = t_ptr->data_init[i];

		/* Randomize some of them */
		if ((i < 4) && (t_ptr->info & FIELD_INFO_RAND))
		{
			f_ptr->data[i] = (byte)rand_int(f_ptr->data[i]);
		}
	}


	/* create actions */
	for (i = 0; i < FIELD_ACTION_MAX; i++)
	{
		/* copy function pointers */
		f_ptr->action[i] = t_ptr->action[i];
	}
	
}

/*
 * See if a field of a particular type is on a square.
 * (eg. call with (c_ptr->fld_idx, FTYPE_TRAP) to get traps)
 */
bool field_is_type(s16b fld_idx, byte typ)
{
	field_type *f_ptr;

	/* While the field exists */
	while (fld_idx)
	{
		/* Get field */
		f_ptr = &fld_list[fld_idx];

		/* Is it the correct type? */
		if (t_info[f_ptr->t_idx].type == typ) return TRUE;

		/* If not, get next one. */
		fld_idx = f_ptr->next_f_idx;
	}

	/* Didn't find anything */
	return FALSE;
}

/*
 * See if flags are set in a list of fields
 *
 * This is used to see of a grid blocks LOS or magic
 */
u16b fields_have_flags(s16b fld_idx, u16b info)
{
	field_type *f_ptr;
	
	u16b flags = 0;

	/* While the field exists */
	while (fld_idx)
	{
		/* Get field */
		f_ptr = &fld_list[fld_idx];

		/* Or the flags together */
		flags |= f_ptr->info;

		/* Get next field. */
		fld_idx = f_ptr->next_f_idx;
	}
	
	return (flags & info);
}



/*
 * Place a field of a given type on a square
 */
s16b place_field(int y, int x, s16b t_idx)
{
	field_type *f_ptr;

	s16b fld_idx;
	
	s16b *fld_ptr;

	field_type temp_field;
	field_type *ft_ptr = &temp_field;

	/* Make the field */
	field_prep(ft_ptr, t_idx);
	
	/* Get pointer to field list */
	fld_ptr = &(area(y, x)->fld_idx);

	/* Place it */
	fld_idx = field_add(ft_ptr, fld_ptr);

	/* Paranoia */
	if (!fld_idx) return (0);

	/* Get new field */
	f_ptr = &fld_list[fld_idx];

	/* Connect to ground */
	f_ptr->fy = y;
	f_ptr->fx = x;	

	return (fld_idx);
}

/*
 * Find the connection to the cave array for the field
 * fld_list[fld_idx].
 *
 * This is used so that an arbitrary field can be deleted.
 * This routine is fairly fast if there are not too many fields
 * on a square at one time.
 *
 * Note that this routine should not be merged with field_hook
 * because most of the time you know where a field is.  Only
 * if you are looking through the fld_list[] one by one will
 * this be needed.
 */
s16b *field_find(s16b fld_idx)
{
	field_type *f_ptr;

	/* pointer to a field index in a list. */
	s16b *location;

	/* Point to the field */
	f_ptr = &fld_list[fld_idx];

	location = &(area(f_ptr->fy, f_ptr->fx)->fld_idx);

	while (*location != fld_idx)
	{
		/* Paranoia: Is the list broken? */
		if (!(*location)) return (location);

		/* Get the next field in the chain */
		location = &(fld_list[*location].next_f_idx);
	}

	/* Found a pointer to our field */
	return (location);
}

/*
 * Call the action function for the field pointed to by *field_ptr.
 *
 * This function does not do a list of fields like the one below.
 *
 * It returns FALSE if the field deleted itself, FALSE otherwise.
 */
bool field_hook_single(s16b *field_ptr, int action, void *action_struct)
{
	/* Point to the field */
	field_type *f_ptr = &fld_list[*field_ptr];

	
	/* Paranoia - Is there a function to call? */
	if (f_ptr->action[action])
	{
		/* Call the action function */
		f_ptr->action[action](field_ptr, action_struct);
	}
	
	/* Check for deletion */
	if (&fld_list[*field_ptr] == f_ptr)
	{
		/* The field is still there */
		return TRUE;
	}
	else
	{
		/* The field has moved - assume it has deleted itself */
		return FALSE;
	}
}


/*
 * Call the specified action routine for each field
 * in the list specified by *field_ptr.
 *
 * Note the code must take into account fields deleting
 * themselves.
 */
void field_hook(s16b *field_ptr, int action, void *action_struct)
{
	field_type *f_ptr;
	
	while (*field_ptr)
	{
		/* Point to the field */
		f_ptr = &fld_list[*field_ptr];

		/* Paranoia - Is there a function to call? */
		if (f_ptr->action[action])
		{
			/* Call the action function */
			f_ptr->action[action](field_ptr, action_struct);
		}
		else
		{
			/* Paranoia - Next field in the list */
			field_ptr = &f_ptr->next_f_idx;
		}

		/*
		 * Hack - the action function must change *field_ptr
		 * to point to the next field.
		 *
		 * This is much simpler than trying to work what the
		 * next one is in the case where the field deletes itself.
		 */
	}
}


void process_fields(void)
{
	s16b fld_idx;
	field_type *f_ptr;
	
	s16b *fld_ptr;

	for (fld_idx = 0; fld_idx < fld_max; fld_idx++)
	{
		/* Point to field */
		f_ptr = &fld_list[fld_idx];

		/* No dead fields */
		if (!f_ptr->t_idx) continue;
		
		/* Get pointer to field index */
		fld_ptr = field_find(fld_idx);

		/* If it is a temporary field, count down every 10 turns */
		if ((f_ptr->info & FIELD_INFO_TEMP) && !(turn % 10))
		{
			/* Decrement counter */
			f_ptr->counter--;

			/* If at bottom */
			if (!f_ptr->counter)
			{
				/* Call completion routine */
				if (field_hook_single(fld_ptr, FIELD_ACT_EXIT, NULL));
				{
					/* It didn't delete itself - do it now */
					delete_field_aux(fld_ptr);
				}

				/* Nothing else to do now */
				continue;
			}
		}

		/* If acts every turn */
		(void) field_hook_single(fld_ptr, FIELD_ACT_ALWAYS, NULL);
	}
}

/*
 * "Testing" function, used to find bugs.
 * It will test the monster-field,
 * object-field, and cave-field data structures.
 * (A similar function was used to detect the
 *  "invisible monster" bug.
 */
void test_field_data_integtrity(void)
{
	int i, j;
	cave_type *c_ptr;
	field_type *f_ptr;

	s16b fld_idx;

	int xmin, xmax, ymin, ymax;

	/* Test cave data structure first */
	/* In dungeon? */
	if (dun_level)
	{
		xmin = 0;
		ymin = 0;
		xmax = cur_wid;
		ymax = cur_hgt;
	}
	else
	/* In wilderness */
	{
		xmin = wild_grid.x_min;
		ymin = wild_grid.y_min;
		xmax = wild_grid.x_max;
		ymax = wild_grid.y_max;
	}

	/* Test cave data structure */
	for (i = xmin; i < xmax; i++)
	{
		for (j = ymin; j < ymax; j++)
		{
			/* Point to location */
			c_ptr = area(j, i);

			fld_idx = c_ptr->fld_idx;

			/* Want a field */
			while (fld_idx)
			{
				f_ptr = &fld_list[fld_idx];

				/* Dead field? */
				if (!f_ptr->t_idx)
				{
					msg_print("Dead Field");
				}

				if (fld_idx > fld_max)
				{
					msg_print("Field index inconsistancy.");
				}

				if ((f_ptr->fy != j) || (f_ptr->fx != i))
				{
					msg_print("Field location inconsistancy.");
					msg_format("Field x, cave x,%d,%d",f_ptr->fx, i);
					msg_format("Field y, cave y,%d,%d",f_ptr->fy, j);
				}

				fld_idx = f_ptr->next_f_idx;
			}
		}
	}
}

/* Field action functions - later will be implemented in python */


/*
 * The type of the void pointer is:
 *
 * FIELD_ACT_INIT			Function dependent.  (Be careful)
 * FIELD_ACT_ALWAYS			NULL
 * FIELD_ACT_PLAYER_ENTER	player_type*	(p_ptr)
 * FIELD_ACT_PLAYER_ON		player_type*	(p_ptr)
 * FIELD_ACT_PLAYER_LEAVE	player_type*	(p_ptr)
 * FIELD_ACT_MONSTER_ENTER	monster_type*	(m_ptr)
 * FIELD_ACT_MONSTER_ON		monster_type*	(m_ptr)
 * FIELD_ACT_MONSTER_LEAVE	monster_type*	(m_ptr)
 * FIELD_ACT_OBJECT_DROP	object_type*	(o_ptr)	 
 * FIELD_ACT_OBJECT_ON		object_type*	(o_ptr)	 
 * FIELD_ACT_TUNNEL			Not implemented yet.
 * FIELD_ACT_MAGIC_TARGET	field_magic_target*
 * FIELD_ACT_COMPACT		byte*
 * FIELD_ACT_EXIT			NULL
 * FIELD_ACT_MONSTER_AI		Not implemented yet.
 * FIELD_ACT_OPEN			Not implemented yet.
 * FIELD_ACT_CLOSE			Not implemented yet.
 * FIELD_ACT_MON_ENTER_TEST field_mon_test*
 */	


/* Simple function that does nothing */
void field_action_nothing(s16b *field_ptr, void *nothing)
{
	field_type *f_ptr = &fld_list[*field_ptr];

	/* Action: Do nothing at all */

	/* Update *field_ptr to point to the next field in the list */
	field_ptr = &(f_ptr->next_f_idx);
	return;
}

/* Simple function that deletes the field */
void field_action_delete(s16b *field_ptr, void *nothing)
{
	/* Action: Do nothing at all */

	/* Delete the field */
	delete_field_aux(field_ptr);

	/* Note that *field_ptr does not need to be updated */
	return;
}

/* 
 * A function that gives "typical" values for field compaction.
 *
 * You may want to make a new function for strange types of fields.
 *
 * Alternatively, this function can be modified.
 *
 * Eventually, if there is only one function, it can be merged into
 * the rest of the compaction code.  This will remove an action type.
 */
void field_action_compact_basic(s16b *field_ptr, void *compact_val)
{
	field_type *f_ptr;
	field_thaum *t_ptr;

	byte *compact_value = (byte *) compact_val;

	/* Point to the field */
	f_ptr = &fld_list[*field_ptr];
	
	/* Point to the field type */
	t_ptr = &t_info[f_ptr->t_idx];
	
	switch (t_ptr->type)
	{
		case FTYPE_NOTHING:
		{
			/* This field is dead - get rid of it */
			*compact_value = 0;
		
			break;
		}
		case FTYPE_TRAP:
		{
			/* Traps can be gotten rid of safely */
			*compact_value = 100;
			
			
			break;
		}
		case FTYPE_DOOR:
		{
			/* Spikes / locked doors can be turned off if required */
			*compact_value = 80;
		
			break;
		}
		case FTYPE_BUILD:
		{
			/* Buildings should not be removed */
			*compact_value = 255;
		
			break;
		}
		case FTYPE_FEAT:
		{
			/* General features are important */
			*compact_value = 250;
			
			break;
		}
		case FTYPE_QUEST:
		{
			/* Quest events can be important */
			*compact_value = 150;
		
			break;
		}
		case FTYPE_FIELD:
		{
			/* Does the field have a counter? */
			if (t_ptr->count_init)
			{
				/* Compact fields that are nearly done */
				*compact_value = 20 + 40 * f_ptr->counter / t_ptr->count_init;
			}
			else
			{
				/* Permanent magical fields can be gotten rid of */
				*compact_value = 50;
			}
		
			break;
		}
	}


	/* Update *field_ptr to point to the next field in the list */
	field_ptr = &(f_ptr->next_f_idx);
	
	return;
}


/* The function that now controls the glyph of warding rune. */
void field_action_glyph_warding(s16b *field_ptr, void *mon_enter_test)
{
	field_type *f_ptr = &fld_list[*field_ptr];
	
	/* Look at input data */
	field_mon_test *mon_enter = (field_mon_test *) mon_enter_test;

	monster_type *m_ptr = mon_enter->m_ptr;
	
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	
	bool do_move = mon_enter->do_move;
	
	/* Hack: No monster - just test for existance of glyph */
	if (!m_ptr)
	{
		/* Monsters cannot be generated / teleported onto glyph */
		mon_enter->do_move = FALSE;
		
		/* Update *field_ptr to point to the next field in the list */
		field_ptr = &(f_ptr->next_f_idx);
		
		/* Done */
		return;
	}
	
	if (do_move && !(r_ptr->flags1 & RF1_NEVER_BLOW) && 
		(randint(BREAK_GLYPH) < r_ptr->level)) 
	{
		/* Describe observable breakage */
		if (area(f_ptr->fy, f_ptr->fx)->info & CAVE_MARK)
		{
			msg_print("The rune of protection is broken!");
		}

		/* Delete the field */
		delete_field_aux(field_ptr);
			
		/* Note that *field_ptr does not need to be updated */

		/* Allow movement */
		do_move = TRUE;
	}
	else
	{
		/* No move allowed */
		do_move = FALSE;
			
		/* Update *field_ptr to point to the next field in the list */
		field_ptr = &(f_ptr->next_f_idx);
	}

	/* Save result */
	mon_enter->do_move = do_move;
	
	/* Done */
	return;
}

/* The function that now controls the exploding rune spell . */
void field_action_glyph_explode(s16b *field_ptr, void *mon_enter_test)
{
	field_type *f_ptr = &fld_list[*field_ptr];
	
	/* Look at input data */
	field_mon_test *mon_enter = (field_mon_test *) mon_enter_test;

	monster_type *m_ptr = mon_enter->m_ptr;
	
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	
	bool do_move = mon_enter->do_move;
	
	/* Hack: No monster - just test for existance of glyph */
	if (!m_ptr)
	{
		/* Monsters cannot be generated / teleported onto glyph */
		mon_enter->do_move = FALSE;
		
		/* Update *field_ptr to point to the next field in the list */
		field_ptr = &(f_ptr->next_f_idx);
		
		/* Done */
		return;
	}
	
	if (do_move && !(r_ptr->flags1 & RF1_NEVER_BLOW) && 
		(randint(BREAK_GLYPH) < r_ptr->level)) 
	{
		if (f_ptr->fy == py && f_ptr->fx == px)
		{
			msg_print("The rune explodes!");
			fire_ball(GF_MANA, 0, 2 * ((p_ptr->lev / 2) + damroll(7, 7)), 2);
		}
		else
			msg_print("An explosive rune was disarmed.");
		
		/* Delete the field */
		delete_field_aux(field_ptr);
			
		/* Note that *field_ptr does not need to be updated */

		/* Allow movement */
		do_move = TRUE;
	}
	else
	{
		/* No move allowed */
		do_move = FALSE;
			
		/* Update *field_ptr to point to the next field in the list */
		field_ptr = &(f_ptr->next_f_idx);
	}

	/* Save result */
	mon_enter->do_move = do_move;
	
	/* Done */
	return;
}


/* 
 * Corpses disappear after the 100 turns...
 *
 * In nightmare mode, they reappear as monsters.
 */
void field_action_corpse_decay(s16b *field_ptr, void *nothing)
{
	field_type *f_ptr = &fld_list[*field_ptr];

	field_thaum *t_ptr = &t_info[f_ptr->t_idx];
	
	/*
	 * Data[1] * 256 + Data[2] = r_idx of monster.
	 */
	
	/* Monster race */
	u16b r_idx = ((u16b) f_ptr->data[1]) * 256 + f_ptr->data[2];
	
	if (ironman_nightmare)
	{
		/* Make a monster nearby if possible */
		if (summon_named_creature(f_ptr->fy, f_ptr->fx,
				 r_idx, FALSE, FALSE, FALSE))
		{
			msg_format("The %s rises.", t_ptr->name);

				/* Set the cloned flag, so no treasure is dropped */
				m_list[hack_m_idx_ii].smart |= SM_CLONED;
		}
	}
	else
	{
		/* Let player know what happened. */
		msg_format("The %s decays.", t_ptr->name);
	}


	/* Delete the field */
	delete_field_aux(field_ptr);

	/* Note that *field_ptr does not need to be updated */
	return;
}

/* Initialise corpse / skeletons */
void field_action_corpse_init(s16b *field_ptr, void *input)
{
	field_type *f_ptr = &fld_list[*field_ptr];

	monster_type *m_ptr = (monster_type *) input;
	
	/*
	 * Data[1] * 256 + Data[2] = r_idx of monster.
	 */
	
	/* Store the r_idx in the data fields so that the corpse can be raised */
	f_ptr->data[1] = m_ptr->r_idx / 256;
	f_ptr->data[2] = m_ptr->r_idx % 256;
	
	/* Initialise the name here? */
	
	/* Initialise the graphic here? */
	
	/* Redraw the square if visible */
	if (area(f_ptr->fy, f_ptr->fx)->info & CAVE_VIEW)
	{
		/* Note the spot */
		note_spot(f_ptr->fy, f_ptr->fx);
		
		lite_spot(f_ptr->fy, f_ptr->fx);
	}
	
	/* Init action functions do not need to change the field_ptr. */
	return;
}
