/* File: quest.c */

/* Purpose: Quest code */

/*
 * Copyright (c) 1989, 1999 James E. Wilson, Robert A. Koeneke,
 * Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

/*
 * Maximum number of tries for selection of a proper quest monster
 */
#define MAX_TRIES 100


/*
 * Acquires and returns the index of a "free" quest.
 *
 * This routine should almost never fail, but in case it does,
 * we must be sure to handle "failure" of this routine.
 */
static u16b q_pop(void)
{
	int i;

	/* Initial allocation */
	if (q_max < z_info->q_max)
	{
		/* Get next space */
		i = q_max;

		/* Count quests */
		q_max++;

		/* Use this quest */
		return (i);
	}

	/* Warn the player */
	msg_print("Too many quests!");

	/* Oops */
	return (0);
}

/*
 * Wipe a quest
 */
static void quest_wipe(int q_idx)
{
	quest_type *q_ptr = &quest[q_idx];

	q_ptr->status = QUEST_STATUS_UNTAKEN;
	q_ptr->flags = 0x00;
	q_ptr->type = QUEST_TYPE_UNKNOWN;

	/* No artificial quest item */
	q_ptr->item = 0;

	/* No quest-giver */
	q_ptr->town = 0;
	q_ptr->shop = 0;
		
	/* No reward */
	q_ptr->reward = 0;

	/* Types of creation and trigger hooks */
	q_ptr->c_type = QC_NONE;
	q_ptr->x_type = QX_NONE;
		
	/* Timeout */
	q_ptr->timeout = 0;
		
	/* No name */
	q_ptr->name[0] = '\0';
		
	/*
	 * Do not need to clear the extra data
	 * - it is ignored since q_ptr->type = UNKNOWN
	 */
}

/* The quest list will be dynamic soon... */
#if 0

/*
 * Copy a quest from one slot to another
 */
static void quest_copy(int src, int dst)
{
	/* Copy the structure */
	COPY(&quest[src], &quest[dst], quest_type);
	
	/* Insert other needed copy code here - ie fixing info / item array */
}


/*
 * Remove a quest from the list
 */
static void quest_remove(int q_idx)
{
	int i;
	
	/* Paranoia */
	if ((q_idx < 0) || (q_idx >= q_max)) return;
	
	/* Shift quests down */
	for (i = q_idx + 1; i < q_max; i++)
	{
		quest_copy(i, i - 1);
	}
	
	/* Paranoia */
	if (!q_max) return;
	
	/* Wipe empty slot */
	quest_wipe(q_max);
	
	/* Decrease total number of quests */
	q_max--;
}


/*
 * Clean the list of quests, removing 'completed' ones
 */
static void prune_quests(void)
{
	int i;
	
	quest_type *q_ptr;
	
	for (i = 0; i < q_max; i++)
	{
		q_ptr = &quest[i];
		
		/*
		 * Remove 'completed' quests.
		 *
		 * Set a quest to be 'finished' if you want to
		 * keep the player memory of it.
		 */
		if (q_ptr->status == QUEST_STATUS_COMPLETED)
		{
			quest_remove(i);
		}
	}
}

#endif /* 0 */


/*
 * Make a quest for killing n monsters of a certain type on a certain level
 */
u16b insert_dungeon_monster_quest(u16b r_idx, u16b num, u16b level)
{
	int q_num;
	quest_type *q_ptr;
	monster_race *r_ptr = &r_info[r_idx];
	
	/* get a new quest */
	q_num = q_pop();

	/* Paranoia */
	if (!q_num) return (0);
	
	q_ptr = &quest[q_num];
	
	/* Store in information */
	q_ptr->type =  QUEST_TYPE_DUNGEON;

	/* We need to place the monster(s) when the dungeon is made */
	q_ptr->c_type = QC_DUN_MONST;
	
	/* We need to trigger when the monsters are killed */
	if (r_ptr->flags1 & RF1_UNIQUE)
	{
		q_ptr->x_type = QX_KILL_UNIQUE;
	}
	else
	{
		q_ptr->x_type = QX_KILL_MONST;
	}

	if (num != 1)
	{

		/* XXX XXX Create quest name */
		(void)strnfmt(q_ptr->name, 60, "Kill %d %s on level %d.",
			 (int) num, r_name + r_ptr->name, (int) level);
	}
	else
	{
		/* XXX XXX Create quest name */
		(void)strnfmt(q_ptr->name, 60, "Kill %s on level %d.",
			 (int) r_name + r_ptr->name, (int) level);
	}
	
	/* Save the quest data */
	q_ptr->data.dun.r_idx = r_idx;
	q_ptr->data.dun.level = level;
	q_ptr->data.dun.cur_num = 0;
	q_ptr->data.dun.max_num = num;
	q_ptr->data.dun.num_mon = 0;
	
	/* Return number of quest */
	return (q_num);
}


/*
 * Initialise the quests
 */
errr init_quests(void)
{
	int i;
	
	/* Make the quest array */
	C_MAKE(quest, z_info->q_max, quest_type);
	
	/* Reset number of quests */
	q_max = 0;
	
	/* Wipe the quests */
	for (i = 0; i < z_info->q_max; i++)
	{
		quest_wipe(i);
	}

	/* Add the winner quests */
	
	/* Hack XXX XXX Oberon, hard coded */
	quest[insert_dungeon_monster_quest(QW_OBERON, 1, 98)].x_type
		 = QX_KILL_WINNER;
	
	/* Hack XXX XXX Serpent, hard coded */
	quest[insert_dungeon_monster_quest(QW_SERPENT, 1, 100)].x_type
		 = QX_KILL_WINNER;
	
	/* Add some 'random quests' at birth. */
	
	return (0);
}


/*
 * Quests
 */
void get_player_quests(void)
{
	char inp[80];

	monster_race *r_ptr;

	int	r_idx;
	int	i, j, v, level;
	
	int best_level, best_r_idx;
	int num;

	/* Extra info */
	Term_putstr(5, 15, -1, TERM_WHITE,
		"You can enter the number of quests you'd like to perform in addition");
	Term_putstr(5, 16, -1, TERM_WHITE,
		"to the two obligatory ones ( Oberon and the Serpent of Chaos )");
	Term_putstr(5, 17, -1, TERM_WHITE,
		"In case you do not want any additional quests, just enter 0");

	Term_putstr(5, 18, -1, TERM_WHITE,
		"If you want a random number of random quests, just enter *");

	/* Ask the number of additional quests */
	while (TRUE)
	{
		put_str("Number of additional quests? (<50) ", 2, 20);

		/* Get a the number of additional quest */
		while (TRUE)
		{
			/* Move the cursor */
			put_str("", 37, 20);

			/* Default */
			strcpy(inp, "20");

			/* Get a response (or escape) */
			if (!askfor_aux(inp, 3)) inp[0] = '\0';

			/* Check for random number of quests */
			if (inp[0] == '*')
			{
				/* 0 to 49 random quests */
				v = randint0(50);
			}
			else
			{
				v = atoi(inp);
			}

			/* Break on valid input */
			if ((v <= 50) && (v >= 0)) break;
		}
		break;
	}

	/* Prepare allocation table */
	get_mon_num_prep(monster_quest, NULL);

	/* Generate quests */
	for (i = 0; i < v; i++)
	{
		level = (i * 97 + 48) / v;
		
		best_r_idx = 1;
		best_level = 1;
		
		/* Get monster */
		for (j = 0; j < MAX_TRIES; j++)
		{
			/*
			 * Random monster out of depth
			 * (depending on level + number of quests)
			 */
			r_idx = get_mon_num(level + 6 + 
					randint1(level * v / 200 + 1) +
			        randint1(level * v / 200 + 1));

			r_ptr = &r_info[r_idx];

			/* Look at the monster - only "hard" monsters for quests */
			if (r_ptr->flags1 & (RF1_NEVER_MOVE | RF1_FRIENDS)) continue;

			/* Save the index if the monster is deeper than current monster */
			if (!best_r_idx || (r_info[r_idx].level > best_level))
			{
				best_r_idx = r_idx;
				best_level = r_info[r_idx].level;
			}

			/* Accept monsters that are a few levels out of depth */
			if (best_level > (level + (level / 20) + 1)) break;
		}
		
		r_ptr = &r_info[best_r_idx];
		
		/* Get the number of monsters */
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			/* Mark uniques */
			r_ptr->flags1 |= RF1_QUESTOR;

			num = 1;
		}
		else if (r_ptr->flags3 & RF3_UNIQUE_7)
		{
			/* Mark uniques */
			r_ptr->flags1 |= RF1_QUESTOR;

			num = randint1(r_ptr->max_num);
		}
		else
		{
			num = 5 + (s16b)randint0(level / 3 + 5) / r_ptr->rarity;
		}
		
		/* Create the quest */
		insert_dungeon_monster_quest(best_r_idx, num, level);
	}
}


/* Array of places to find an inscription */
static cptr find_quest[] =
{
	"You find the following inscription in the floor",
	"You see a message inscribed in the wall",
	"There is a sign saying",
	"Something is written on the staircase",
	"You find a scroll with the following message",
	"You hear",
};


/*
 * Discover quests on this level
 */
void quest_discovery(void)
{
	int i;
	
	quest_type *q_ptr;
	
	char name[80];
	
	for (i = 0; i < q_max; i++)
	{
	 	q_ptr = &quest[i];
		
		/* Quest needs to be taken. */
		if (q_ptr->status != QUEST_STATUS_TAKEN) continue;

		/* Is the quest active? */
		if (!(q_ptr->flags & QUEST_FLAG_ACTIVE)) continue;
		
		/* Is the quest known already? */
		if (q_ptr->flags & QUEST_FLAG_KNOWN) continue;
		
		/* Hack - The quest is now known */
		q_ptr->flags |= QUEST_FLAG_KNOWN;
	
		/* See what type of quest it is */
		switch (q_ptr->type)
		{
			case QUEST_TYPE_UNKNOWN:
			{
				/* Paranoia */
				continue;
			}
		
			case QUEST_TYPE_GENERAL:
			{
			
				/* Paranoia */
				continue;
			}
		
			case QUEST_TYPE_DUNGEON:
			{
				monster_race *r_ptr = &r_info[q_ptr->data.dun.r_idx];
				int q_num = q_ptr->data.dun.max_num - q_ptr->data.dun.cur_num;
			
				/* Assume the quest is a 'kill n monsters quest' for now. */
				strcpy(name, (r_name + r_ptr->name));

				if (r_ptr->flags1 & RF1_UNIQUE)
				{
					/* Unique */
					msg_format("%s: Beware, this level is protected by %s!",
						 find_quest[rand_range(0, 5)], name);
				}
				else
				{
					/* Normal monsters */
					if (q_num > 1) plural_aux(name);

					msg_format("%s: Be warned, this level is guarded by %d %s!",
						 find_quest[rand_range(0, 5)], q_num, name);
				}
			}
		
			case QUEST_TYPE_WILD:
			{
		
				/* Paranoia */
				continue;
			}
		
			default:
			{
				/* Paranoia */
				continue;
			}
		}
	}
}


/*
 * Hack -- Check if a level is a "quest" level
 */
int quest_number(void)
{
	int i;
	
	int count = 0;
	
	quest_type *q_ptr;

	for (i = 0; i < q_max; i++)
	{
		q_ptr = &quest[i];
		
		/* Quest needs to be taken. */
		if (q_ptr->status != QUEST_STATUS_TAKEN) continue;

		/* Is the quest active? */
		if (q_ptr->flags & QUEST_FLAG_ACTIVE) count++;
	}

	/* Return number of active quests */
	return (count);
}

/*
 * Activate quests valid for this level
 */
void activate_quests(int level)
{
	int i;
	
	quest_type *q_ptr;
	
	for (i = 0; i < q_max; i++)
	{
		q_ptr = &quest[i];
		
		/* Is the quest relevant? */
		if ((q_ptr->type == QUEST_TYPE_GENERAL)
			|| ((q_ptr->type == QUEST_TYPE_DUNGEON) && (level))
			|| ((q_ptr->type == QUEST_TYPE_WILD) && (!level)))
		{
			q_ptr->flags |= QUEST_FLAG_ACTIVE;
			
			/* Hack - toggle QUESTOR flag */
			if (q_ptr->type == QUEST_TYPE_DUNGEON)
			{
				r_info[q_ptr->data.dun.r_idx].flags1 |= RF1_QUESTOR;
			}
		}
		else
		{
			q_ptr->flags &= ~(QUEST_FLAG_ACTIVE);
		}
	}
}


/*
 * Count the number of quests chosen so far
 */
int number_of_quests(void)
{
	int i;
	
	int count = 0;

	for (i = 0; i < q_max; i++)
	{
		if (quest[i].status != QUEST_STATUS_UNTAKEN)
		{
			/* Increment count of quests taken. */
			count++;
		}
	}

	/* Return the number of quests taken */
	return (count);
}


/*
 * Create a magical staircase
 */
static void create_stairs(int x, int y)
{
	int i = 0;
	
	int ny, nx;
	
	cave_type *c_ptr = area(y, x);
	
	/* Stagger around */
	while ((cave_perma_grid(c_ptr) || c_ptr->o_idx) && !(i > 100))
	{
		/* Pick a location */
		scatter(&nx, &ny, x, y, 1);

		/* Stagger */
		y = ny; x = nx;

		/* paranoia - increment counter */
		i++;

		/* paranoia */
		if (!in_bounds(y, x)) continue;

		c_ptr = area(y, x);
	}

	/* Explain the staircase */
	msg_print("A magical staircase appears...");

	/* Create stairs down */
	cave_set_feat(x, y, FEAT_MORE);
	
	/* Notice the change */
	note_spot(x, y);

	/* Remember to update everything */
	p_ptr->update |= (PU_VIEW | PU_FLOW | PU_MONSTERS);
}

/*
 * Give the reward for the indicated quest
 */
static void quest_reward(int num, int x, int y)
{
	object_type forge, *o_ptr;
	
	/* Ignore num for now */
	(void) num;
	
	while (TRUE)
	{
		/* Get local object */
		o_ptr = &forge;

		/* Wipe the object */
		object_wipe(o_ptr);

		/* Average of 20 great objects per game */
		if (randint0(number_of_quests()) < 20)
		{
			/* Make a great object */
			(void)make_object(o_ptr, 30, dun_theme);
		}
		else
		{
			/* Make a good object */
			(void)make_object(o_ptr, 15, dun_theme);
		}
		
		/* We need a 'good' item - so check the price */
		if (object_value_real(o_ptr) > 100 * p_ptr->depth) break;
	}
	
	/* We need to be given a location... */
	
	
	/* Drop it in the dungeon */
	(void)drop_near(o_ptr, -1, x, y);
}


/*
 * Test each quest to see which ones are created
 */
void trigger_quest_create(byte c_type, void *data)
{
	int i, j, k;
	quest_type *q_ptr;
	cave_type *c_ptr;
	
	/* Ignore data - it may be unused */
	(void) data;

	for (i = 0; i < q_max; i++)
	{
		q_ptr = &quest[i];
		
		/* Quest must be chosen */
		if (q_ptr->status != QUEST_STATUS_TAKEN) continue;
		
		/* Quest must be active */
		if (!(q_ptr->flags & QUEST_FLAG_ACTIVE)) continue;
		
		/* Must be relevant */
		if (q_ptr->c_type != c_type) continue;
		
		/* Handle the trigger */
		switch (c_type)
		{
			case QC_NONE:
			{
				/* Paranoia */
				continue;
			}
			
			case QC_DUN_MONST:
			{
				int x, y;
				
				monster_race *r_ptr = &r_info[q_ptr->data.dun.r_idx];
				
				/* Hack -- "unique" monsters must be "unique" */
				if ((r_ptr->flags1 & RF1_UNIQUE) &&
					(r_ptr->cur_num >= r_ptr->max_num))
				{
					/* The unique is already dead */
					q_ptr->status = QUEST_STATUS_FINISHED;
				}
				else
				{
					bool group;

					/* Hard quests -> revive all monsters */
					if (ironman_hard_quests)
					{
						q_ptr->data.dun.cur_num = 0;
					}

					for (j = 0; j < (q_ptr->data.dun.max_num - q_ptr->data.dun.cur_num); j++)
					{
						for (k = 0; k < 5000; k++)
						{
							/* Find an empty grid */
							while (TRUE)
							{
								y = rand_range(p_ptr->min_hgt + 1, p_ptr->max_hgt - 2);
								x = rand_range(p_ptr->min_wid + 1, p_ptr->max_wid - 2);

								/* Access the grid */
								c_ptr = &cave[y][x];

								if (!cave_naked_grid(c_ptr)) continue;
								if (distance(y, x, p_ptr->py, p_ptr->px) < 10) continue;
								else break;
							}

							if (r_ptr->flags1 & RF1_FRIENDS)
								group = FALSE;
							else
								group = TRUE;

							/* Try to place the monster */
							if (place_monster_aux(x, y, q_ptr->data.dun.r_idx, FALSE, group, FALSE, FALSE))
							{
								/* Success */
								break;
							}
							else
							{
								/* Failure - Try again */
								continue;
							}
						}
					}
				}
			}
		}
	}
}


/*
 * Test each quest to see if they are completed
 */
void trigger_quest_complete(byte x_type, void *data)
{
	int i;
	quest_type *q_ptr;
	
	
	for (i = 0; i < q_max; i++)
	{
		q_ptr = &quest[i];
		
		/* Quest must be chosen */
		if (q_ptr->status != QUEST_STATUS_TAKEN) continue;
		
		/* Quest must be active */
		if (!(q_ptr->flags & QUEST_FLAG_ACTIVE)) continue;
		
		/* Must be relevant */
		if (q_ptr->x_type != x_type) continue;
		
		/* Handle the trigger */
		switch (x_type)
		{
			case QX_NONE:
			{
				/* Paranoia */
				continue;
			}
			
			case QX_KILL_MONST:
			case QX_KILL_UNIQUE:
			{
				monster_type *m_ptr = ((monster_type *)data);
				
				if (q_ptr->data.dun.r_idx == m_ptr->r_idx)
				{
					/* Don't count clones */
					if (m_ptr->smart & SM_CLONED) break;
					
					/* Increment number killed */
					q_ptr->data.dun.cur_num++;
					
					if (q_ptr->data.dun.cur_num >= q_ptr->data.dun.max_num)
					{
						/* Complete the quest */
						q_ptr->status = QUEST_STATUS_COMPLETED;
						
						/* Create some stairs */
						create_stairs(m_ptr->fx, m_ptr->fy);
						
						/* Drop the reward */
						quest_reward(q_ptr->reward, m_ptr->fx, m_ptr->fy);
						
						/* Monster is no longer 'QUESTOR' */
						r_info[q_ptr->data.dun.r_idx].flags1 &= ~(RF1_QUESTOR);
					}
				}
				
				continue;
			}
			
			case QX_KILL_WINNER:
			{
				monster_type *m_ptr = ((monster_type *)data);
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				
				if (q_ptr->data.dun.r_idx == m_ptr->r_idx)
				{
					/* Winner? */
					if (strstr((r_name + r_ptr->name), "Serpent of Chaos"))
					{
						/* Total winner */
						p_ptr->total_winner = TRUE;

						/* Redraw the "title" */
						p_ptr->redraw |= (PR_TITLE);
			
						/* Congratulations */
						msg_print("*** CONGRATULATIONS ***");
						msg_print("You have won the game!");
						msg_print("You may retire (commit suicide) when you are ready.");
					}
					else
					{
						/* Oberon */
						
						/* A message */
						msg_print("Well done!");
						msg_print("You have beaten Oberon.");
						msg_print("You now can meet the final challenge of the Serpent of Chaos.");
					}
					
					/* Complete the quest */
					q_ptr->status = QUEST_STATUS_FINISHED;
					
					/* Mega-hack */
					create_stairs(m_ptr->fx, m_ptr->fy);
				}
				
				continue;
			}
		}
#if 0		
		/* Hack - Give a message for 'completed' quests. */
		if (q_ptr->status == QUEST_STATUS_COMPLETED)
		{
			/* Take note */
			if (auto_notes)
			{
				char note[80];
				
				sprintf(note, "Finished quest: %d %s",
					 quest[i].max_num, (r_name + r_info[quest[i].r_idx].name));
				
				add_note(note, 'Q');
			}

			msg_print("You just completed your quest!");
			message_flush();
		}
#endif /* 0 */
	}
}

#if 0
/*
 * Display quest information
 */
static void get_questinfo(int questnum)
{
	int     i;
	int     old_quest;
	char    tmp_str[80];


	/* Clear the text */
	for (i = 0; i < 10; i++)
	{
		quest_text[i][0] = '\0';
	}


	/* Print the quest info */
	sprintf(tmp_str, "Quest Information (Danger level: %d)", quest[questnum].level);
	prt(tmp_str, 5, 0);

	prt(quest[questnum].name, 7, 0);

	for (i = 0; i < 10; i++)
	{
		c_put_str(TERM_YELLOW, quest_text[i], 0, i + 8);
	}
}


/*
 * Request a quest from the Lord.
 */
static void castle_quest(void)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	int             q_index = 0;
	monster_race    *r_ptr;
	quest_type      *q_ptr;
	cptr            name;


	clear_bldg(7, 18);

	/* Current quest of the building */
	q_index = cave[py][px].special;

	/* Is there a quest available at the building? */
	if (!q_index)
	{
		put_str("I don't have a quest for you at the moment.", 0, 8);
		return;
	}

	q_ptr = &quest[q_index];

	/* Quest is completed */
	if (q_ptr->status == QUEST_STATUS_COMPLETED)
	{
		/* Rewarded quest */
		q_ptr->status = QUEST_STATUS_REWARDED;

		get_questinfo(q_index);
	}
	/* Failed quest */
	else if (q_ptr->status == QUEST_STATUS_FAILED)
	{
		get_questinfo(q_index);

		/* Mark quest as done (but failed) */
		q_ptr->status = QUEST_STATUS_FAILED_DONE;
	}
	/* Quest is still unfinished */
	else if (q_ptr->status == QUEST_STATUS_TAKEN)
	{
		put_str("You have not completed your current quest yet!", 0, 8);
		put_str("Use CTRL-Q to check the status of your quest.", 0, 9);
		put_str("Return when you have completed your quest.", 0, 12);
	}
	/* No quest yet */
	else if (q_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		q_ptr->status = QUEST_STATUS_TAKEN;

		/* Assign a new quest */
		if (q_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL)
		{
			if (q_ptr->r_idx == 0)
			{
				/* Random monster at least 5 - 10 levels out of deep */
				q_ptr->r_idx = get_mon_num(q_ptr->level + rand_range(5, 10));
			}

			r_ptr = &r_info[q_ptr->r_idx];

			while ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->rarity != 1))
			{
				q_ptr->r_idx = get_mon_num(q_ptr->level) + rand_range(5, 10);
				r_ptr = &r_info[q_ptr->r_idx];
			}

			if (q_ptr->max_num == 0)
			{
				/* Random monster number */
				if (randint1(10) > 7)
					q_ptr->max_num = 1;
				else
					q_ptr->max_num = rand_range(2, 4);
			}

			q_ptr->cur_num = 0;
			name = (r_name + r_ptr->name);
			msg_format("Your quest: kill %d %s", q_ptr->max_num, name);
			message_flush();
		}
		else
		{
			get_questinfo(q_index);
		}
	}
}

#endif /* 0 */


/*
 * Print quest status of all active quests
 */
void do_cmd_knowledge_quests(void)
{
	FILE *fff;
	char file_name[1024];
	char name[80];
	char tmp_str[256];
	
	quest_type *q_ptr;
	int i;
	
	/* Open a temporary file */
	fff = my_fopen_temp(file_name, 1024);

	/* Failure */
	if (!fff) return;

	for (i = 0; i < q_max; i++)
	{

		q_ptr = &quest[i];
		
		/* Do we know about it? */
		if (q_ptr->status == QUEST_STATUS_UNTAKEN) continue;
		
		/* See what type of quest it is */
		switch (q_ptr->type)
		{
			case QUEST_TYPE_UNKNOWN:
			{
				/* Paranoia */
				continue;
			}
		
			case QUEST_TYPE_GENERAL:
			{
				
				/* Paranoia */
				continue;
			}
		
			case QUEST_TYPE_DUNGEON:
			{
				monster_race *r_ptr = &r_info[q_ptr->data.dun.r_idx];
								
				strncpy(name, r_name + r_ptr->name, 80);
				
				if (q_ptr->status == QUEST_STATUS_TAKEN)
				{
					/* Hack - assume kill n monsters of type m */
					if (q_ptr->data.dun.max_num > 1)
					{
						plural_aux(name);

						strnfmt(tmp_str, 256, "%s (Dungeon level: %d)\n  Kill %d %s, have killed %d.\n",
							q_ptr->name, (int) q_ptr->data.dun.level,
							(int) q_ptr->data.dun.max_num, name, (int) q_ptr->data.dun.cur_num);
					}
					else
					{
						strnfmt(tmp_str, 256, "%s (Dungeon level: %d)\n  Kill %s.\n",
							q_ptr->name, (int) q_ptr->data.dun.level, name);
					}
				}
				else
				{
					/* Assume we've completed it for now */
					strnfmt(tmp_str, 256, "%s (Completed on dungeon level %d). \n",
						q_ptr->name, (int) q_ptr->data.dun.level, name);
				
				}
			}
		
			case QUEST_TYPE_WILD:
			{
		
				/* Paranoia */
				continue;
			}
		
			default:
			{
				/* Paranoia */
				return;
			}
		
		}	
		
		/* Copy to the file */
		fprintf(fff, "%s", tmp_str);
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
	(void)show_file(file_name, "Quest status", 0, 0);

	/* Remove the file */
	(void)fd_kill(file_name);
}
