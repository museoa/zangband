/* File: borg5.c */
/* Purpose: Medium level stuff for the Borg -BEN- */

#include "angband.h"

#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"
#include "zborg4.h"
#include "zborg5.h"



/*
 * This file is responsible for the "borg_update" routine, which is used
 * to notice changes in the world, and to keep track of terrain features,
 * objects, monsters, both from visual changes, and from world messages.
 *
 * One big thing this file does is "object/monster tracking", which
 * attempts to gather information about the objects and monsters in
 * the dungeon, including their identity, location, and state, and
 * to "follow" them if they "move", and to delete them when they die.
 *
 * Information about terrain is used to help plan "flow" paths.  Info
 * about objects and monsters is used to optimize planning paths to
 * those objects and monsters.  Info about monsters is also used for
 * the "danger" functions, which help avoid dangerous situations.
 *
 * Notes:
 *   We assume that monsters/objects can never appear in walls/doors
 *   We count the occurance of invisible or offscreen monsters
 *   We treat "mimics" and "trappers" as "invisible" monsters
 *
 * To Do:
 *
 * Bugs:
 *   The timestamps are not quite in sync properly (?)
 */


/*
 * Old values
 */

static int o_w_x = -1;	/* Old panel */
static int o_w_y = -1;	/* Old panel */

static int o_c_x = -1;	/* Old location */
static int o_c_y = -1;	/* Old location */


/*
 * Hack -- message memory
 */

static s16b borg_msg_len;

static s16b borg_msg_siz;

static char *borg_msg_buf;

static s16b borg_msg_num;

static s16b borg_msg_max;

static s16b *borg_msg_pos;

static s16b *borg_msg_use;


/*
 * Hack -- help identify "unique" monster names
 */

static int borg_unique_size;	/* Number of uniques */
static s16b *borg_unique_what;	/* Indexes of uniques */
static cptr *borg_unique_text;	/* Names of uniques */

/*
 * Hack -- help identify "normal" monster names
 */

static int borg_normal_size;	/* Number of normals */
static s16b *borg_normal_what;	/* Indexes of normals */
static cptr *borg_normal_text;	/* Names of normals */


/*
 * Hack -- object/monster tracking array
 */

static int borg_wank_num = 0;

static borg_wank *borg_wanks;


/*
 * Delete an old "object" record
 */
static void borg_delete_take(int i)
{
	borg_take *take = &borg_takes[i];

	/* Paranoia -- Already wiped */
	if (!take->k_idx) return;

	/* Note */
	borg_note(format("# Forgetting an object '%s' at (%d,%d)",
					 (k_name + k_info[take->k_idx].name), take->y, take->x));

	/* Kill the object */
	WIPE(take, borg_take);

	/* One less object */
	borg_takes_cnt--;

	/* Wipe goals */
	goal = 0;
}

/*
 * Attempt to "follow" a missing object
 *
 * This routine is not called when the player is blind or hallucinating.
 *
 * This function just deletes objects which have disappeared.
 *
 * We assume that a monster walking onto an object destroys the object.
 */
static void borg_follow_take(int i)
{
	borg_take *take = &borg_takes[i];

	/* Paranoia */
	if (!take->k_idx) return;

	/* Note */
	borg_note(format("# There was an object '%s' at (%d,%d)",
					 (k_name + k_info[take->k_idx].name), take->y, take->x));

	/* Kill the object */
	borg_delete_take(i);
}


/*
 * Guess the kidx for an unknown item.
 */
static int borg_guess_kidx(int unknown)
{
    int k;

    for (k = 1; k < z_info->k_max; k++)
    {
        object_kind *k_ptr = &k_info[k];

        /* Skip "empty" items */
        if (!k_ptr->name) continue;

        /* Skip identified items */
        if (k_ptr->aware) continue;

        /* Skip items with the wrong symbol */
        if (unknown != k_ptr->d_char) continue;

        /* Valueless items are boring */
        if (k_ptr->cost <= 0) continue;

        /* Return the result */
        return k;
    }

    /* Didn't find anything */
    return (0);
}

/*
 * Obtain a new "take" index
 */
static int borg_new_take(int k_idx, int y, int x)
{
	int i, n = 0;

	borg_take *take;

	/* Look for a "dead" object */
	for (i = 1; i < borg_takes_nxt; i++)
	{
		/* Reuse "dead" objects */
		if (!borg_takes[i].k_idx)
		{
			n = i;
			break;
		}
	}

	/* Allocate a new object */
	if ((n == 0) && (borg_takes_nxt < 256))
	{
		/* Acquire the entry, advance */
		n = borg_takes_nxt++;
	}

	/* Hack -- steal an old object */
	if (n == 0)
	{
		/* Note */
		borg_note("# Too many objects");

		/* Hack -- Pick a random object */
		n = randint1(borg_takes_nxt - 1);

		/* Delete it */
		borg_delete_take(n);
	}

	/* Count new object */
	borg_takes_cnt++;

	/* Obtain the object */
    take = &borg_takes[n];

	/* Save the kind */
	take->k_idx = k_idx;

	/* Save the location */
	take->x = x;
	take->y = y;

	/* Timestamp */
	take->when = borg_t;

	/* Note */
	borg_note(format("# Creating an object '%s' at (%d,%d)",
					 (k_name + k_info[take->k_idx].name), y, x));

	/* Wipe goals */
	goal = 0;

	/* Result */
	return (n);
}



/*
 * Attempt to notice a changing "take"
 */
static bool observe_take_diff(int y, int x)
{
	int i, k_idx;

	borg_take *take;
	map_block *mb_ptr;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (FALSE);

	/* Get grid */
	mb_ptr = map_loc(x, y);

    if (mb_ptr->object)
    {
        /* Get the kind */
        k_idx = mb_ptr->object;
    }
    else
    {
        k_idx = borg_guess_kidx(mb_ptr->unknown);
    }

	/* Oops */
	if (!k_idx) return (FALSE);

	/* Make a new object */
	i = borg_new_take(k_idx, y, x);

	/* Get the object */
	take = &borg_takes[i];

	/* Timestamp */
	take->when = borg_t;

	/* Okay */
	return (TRUE);
}


/*
 * Attempt to "track" a "take" at the given location
 * Assume that the object has not moved more than "d" grids
 * Note that, of course, objects are never supposed to move,
 * but we may want to take account of "falling" missiles later.
 */
static bool observe_take_move(int y, int x, int d)
{
	int i, z, ox, oy;

	object_kind *k_ptr;
	map_block *mb_ptr;

	/* Scan the objects */
	for (i = 1; i < borg_takes_nxt; i++)
	{
		borg_take *take = &borg_takes[i];

		/* Skip dead objects */
		if (!take->k_idx) continue;

		/* Skip assigned objects */
		if (take->seen) continue;

		/* Extract old location */
		ox = take->x;
		oy = take->y;

		/* Calculate distance */
		z = distance(oy, ox, y, x);

		/* Too far? */
		if (z > d) continue;

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		mb_ptr = map_loc(x, y);

		/* Match type here */
		if (mb_ptr->object != take->k_idx) continue;

		/* Actual movement (?) */
		if (z)
		{
			/* Track it */
			take->x = x;
			take->y = y;

			k_ptr = &k_info[take->k_idx];

			/* Note */
			borg_note(format
					  ("# Tracking an object '%s' at (%d,%d) from (%d,%d)",
					   (k_name + k_ptr->name), take->y, take->x, ox, oy));

			/* Clear goals if really critical to follow movement */
			if (!borg_skill[BI_ESP] && goal == GOAL_TAKE) goal = 0;
		}

		/* Timestamp */
		take->when = borg_t;

		/* Mark as seen */
		take->seen = TRUE;

		/* Done */
		return (TRUE);
	}

	/* Oops */
	return (FALSE);
}


/*
 * Attempt to convert a monster name into a race index
 *
 * First we check for all possible "unique" monsters, including
 * ones we have killed, and even if the monster name is "prefixed"
 * (as in "The Tarrasque" and "The Lernean Hydra").  Since we use
 * a fast binary search, this is acceptable.
 *
 * Otherwise, if the monster is NOT named "The xxx", we assume it
 * must be a "player ghost" (which is impossible).
 *
 * Then, we do a binary search on all "normal" monster names, using
 * a search which is known to find the last matching entry, if one
 * exists, and otherwise to find an entry which would follow the
 * matching entry if there was one, unless the matching entry would
 * follow all the existing entries, in which case it will find the
 * final entry in the list.  Thus, we can search *backwards* from
 * the result of the search, and know that we will access all of
 * the matching entries, as long as we stop once we find an entry
 * which does not match, since this will catch all cases above.
 *
 * Finally, we assume the monster must be a "player ghost" (which
 * as noted above is impossible), which is a hack, but may prevent
 * crashes, even if it does induce strange behavior.
 */
static int borg_guess_race_name(cptr who)
{
	int k, m, n;

	int i, b_i = 0;
	int s, b_s = 0;

	monster_race *r_ptr;

	char partial[160];

	int len = strlen(who);

	/* Start the search */
	m = 0;
	n = borg_unique_size;

	/* Binary search */
	while (m < n - 1)
	{
		/* Pick a "middle" entry */
		i = (m + n) / 2;

		/* Search to the right (or here) */
		if (strcmp(borg_unique_text[i], who) <= 0)
		{
			m = i;
		}

		/* Search to the left */
		else
		{
			n = i;
		}
	}

	/* Check for equality */
	if (streq(who, borg_unique_text[m]))
	{
		/* Use this monster */
		return (borg_unique_what[m]);
	}


	/* Assume player ghost */
	if (!prefix(who, "The "))
	{
		/* Message */
		borg_note(format("# Assuming player ghost (%s)", who));

		/* Oops */
		return (z_info->r_max);
	}

	/* Hack -- handle "offscreen" */
	if (suffix(who, " (offscreen)"))
	{
		/* Remove the suffix */
		strcpy(partial, who);
		partial[len - 12] = '\0';
		who = partial;

		/* Message */
		borg_note(format("# Handling offscreen monster (%s)", who));
	}

	/* Skip the prefix */
	who += 4;


	/* Start the search */
	m = 0;
	n = borg_normal_size;

	/* Binary search */
	while (m < n - 1)
	{
		/* Pick a "middle" entry */
		i = (m + n) / 2;
		/* Search to the right (or here) */
		if (strcmp(borg_normal_text[i], who) <= 0)
		{
			m = i;
		}

		/* Search to the left */
		else
		{
			n = i;
		}
	}

	/* Scan possibilities */
	for (k = m; k >= 0; k--)
	{
		/* Stop when done */
		if (!streq(who, borg_normal_text[k])) break;

		/* Extract the monster */
		i = borg_normal_what[k];

		/* Access the monster */
		r_ptr = &r_info[i];

		/* Basic score */
		s = 1000;

		/* Penalize "depth miss" */
		s = s - ABS(r_ptr->level - borg_skill[BI_CDEPTH]);

		/* Track best */
		if (b_i && (s < b_s)) continue;

		/* Track it */
		b_i = i;
		b_s = s;
	}

	/* Success */


	if (b_i) return (b_i);


	/* Message */
	borg_note(format("# Assuming player ghost (%s)", who));

	/* Oops */
	return (z_info->r_max);
}

/*
 * Increase the "region danger"
 */
static void borg_fear_grid(cptr who, int y, int x, uint k, bool seen_guy)
{
	int i, j;
	
	map_block *mb_ptr;

	/* Reduce fear if GOI is on */
	if (borg_goi)
	{
		k = k / 3;
	}

	/* Messages */
	if (seen_guy)
	{
		borg_note(format("#   Fearing region value %d.", k));
	}
	else
	{
		borg_note(format
				  ("# Fearing grid (%d,%d) value %d because of a non-LOS %s", y,
				   x, k, who));
	}

	/* Current region */
	
	for (i = -15; i < 15; i++)
	{
		for (j = -15; j < 15; j++)
		{
			if (!map_in_bounds(i + x, j + y)) continue;
			
			mb_ptr = map_loc(i + x, j + y);
			
			/* Add some fear */
			mb_ptr->fear += 2 * k /(8 + ABS(i) + ABS(j));
		}
	}


	/* there is some problems here, when the death of a monster decreases the fear,
	 * it ends up making about 2000 danger pts.  It needs to be fixed.
	 */
}


/*
 * Hack -- Update a "new" monster
 */
static void borg_update_kill_new(int i)
{
	int k = 0;
	int num = 0;

	byte spell[96];
	borg_kill *kill = &borg_kills[i];

	monster_race *r_ptr = &r_info[kill->r_idx];

	/* Extract the monster speed */
	kill->speed = (r_ptr->speed);

#if 0
	/* Hack -- assume optimal racial variety */
	if (!(r_ptr->flags1 & RF1_UNIQUE))
	{
		/* Hack -- Assume full speed bonus */
		kill->speed += (extract_energy[kill->speed] / 10);
	}
#endif


	/* Extract max hitpoints */
	/* This is a cheat.  Borg does not look
	 * at the bar at the bottom and frankly that would take a lot of code.
	 * It would involve targeting every monster to read their individual bar.
	 * then keeping track of it.  When the borg has telepathy this would
	 * cripple him down and be tremendously slow.
	 *
	 * This cheat is not too bad.  A human could draw the same info from
	 * from the screen.
	 *
	 * Basically the borg is cheating the real hit points of the monster then
	 * using that information to calculate the estimated hp of the monster.
	 * Its the same basic tactict that we would use.
	 *
	 * Kill->power is used a lot in borg_danger,
	 * for calculating damage from breath attacks.
	 */
#if 0
	if (m_ptr->maxhp)
	{
		/* Cheat the "percent" of health */
		pct = 100L * m_ptr->hp / (m_ptr->maxhp > 1) ? m_ptr->maxhp : 1;
	}
	else
	{
		pct = 100;
	}

	/* Compute estimated HP based on number of * in monster health bar */
	kill->power = (m_ptr->maxhp * pct) / 100;
#endif
	kill->power = /* m_ptr->hp; */ r_ptr->hdice * r_ptr->hside;

	/* Extract the Level */
	kill->level = r_ptr->level;

	/* Extract the Invuln */
	kill->invulner = /* m_ptr->invulner; */ FALSE;

	/* Some monsters never move */
	if (r_ptr->flags1 & RF1_NEVER_MOVE) kill->awake = TRUE;
#if 0
	/* Is it sleeping apw */
	if (m_ptr->csleep == 0) kill->awake = TRUE;
	else
		kill->awake = FALSE;

	/* Is it afraid apw */
	if (m_ptr->monfear == 0) kill->afraid = FALSE;
	else
		kill->afraid = TRUE;

	/* Is it confused apw */
	if (m_ptr->confused == 0) kill->confused = FALSE;
	else
		kill->confused = TRUE;

	/* Is it stunned */
	if (m_ptr->stunned == 0) kill->stunned = FALSE;
	else
		kill->stunned = TRUE;

	/* Friendly or pet */
	if (is_pet(m_ptr)) kill->friendly = TRUE;
	if (is_friendly(m_ptr)) kill->friendly = TRUE;
#endif /* 0 */

	/* hack - we cannot access m_ptr */
	kill->awake = TRUE;
	kill->afraid = FALSE;
	kill->confused = FALSE;
	kill->stunned = FALSE;
	kill->friendly = FALSE;

	/* Can it attack from a distance? */
	/* Extract the "inate" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags4 & (1L << k)) spell[num++] = k + 32 * 3;
	}

	/* Extract the "normal" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags5 & (1L << k)) spell[num++] = k + 32 * 4;
	}

	/* Extract the "bizarre" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags6 & (1L << k)) spell[num++] = k + 32 * 5;
	}

	/* to cast or not to cast, that is the question */
	if (num)
	{
		kill->ranged_attack = TRUE;
	}
	else
		kill->ranged_attack = FALSE;

}


/*
 * Hack -- Update a "old" monster
 *
 * We round the player speed down, and the monster speed up,
 * and we assume maximum racial speed for each monster.
 */
static void borg_update_kill_old(int i)
{
	int t, e;
	int k = 0;
	int num = 0;

	byte spell[96];
	borg_kill *kill = &borg_kills[i];

	monster_race *r_ptr = &r_info[kill->r_idx];

	/* Extract max hitpoints */
	/* Extract actual Hitpoints, this is a cheat.  Borg does not look
	 * at the bar at the bottom and frankly that would take a lot of code.
	 * It would involve targeting every monster to read their individual bar.
	 * then keeping track of it.  When the borg has telepathy this would
	 * cripple him down and be tremendously slow.
	 *
	 * This cheat is not too bad.  A human could draw the same info from
	 * from the screen.
	 *
	 * Basically the borg is cheating the real hit points of the monster then
	 * using that information to calculate the estimated hp of the monster.
	 * Its the same basic tactict that we would use.
	 *
	 * Kill->power is used a lot in borg_danger,
	 * for calculating damage from breath attacks.
	 */

#if 0
	if (m_ptr->maxhp)
	{
		/* Cheat the "percent" of health */
		pct = 100L * m_ptr->hp / (m_ptr->maxhp > 1) ? m_ptr->maxhp : 1;
	}
	else
	{
		pct = 100;
	}

	/* Compute estimated HP based on number of * in monster health bar */
	kill->power = (m_ptr->maxhp * pct) / 100;
#endif
	kill->power = /* m_ptr->hp; */ r_ptr->hdice * r_ptr->hside;

#if 0
	/* Is it sleeping apw */
	if (m_ptr->csleep == 0) kill->awake = TRUE;
	else
		kill->awake = FALSE;

	/* Is it afraid apw */
	if (m_ptr->monfear == 0) kill->afraid = FALSE;
	else
		kill->afraid = TRUE;

	/* Is it confused apw */
	if (m_ptr->confused == 0) kill->confused = FALSE;
	else
		kill->confused = TRUE;

	/* Is it stunned */
	if (m_ptr->stunned == 0) kill->stunned = FALSE;
	else
		kill->stunned = TRUE;

	/* Extract the Invuln */
	kill->invulner = m_ptr->invulner;
#endif /* 0 */

	/* Hack - we cannot access m_ptr */
	kill->awake = TRUE;
	kill->afraid = FALSE;
	kill->confused = FALSE;
	kill->stunned = FALSE;

	/* Extract the monster speed */
	kill->speed = r_ptr->speed;

	/* Player energy per game turn */
	e = extract_energy[borg_skill[BI_SPEED]];

	/* Game turns per player move */
	t = (100 + (e - 1)) / e;

	/* Monster energy per game turn */
	e = extract_energy[kill->speed];

	/* Monster moves (times ten) */
	kill->moves = (t * e) / 10;

#if 0
	/* Friendly or pet */
	if (is_pet(m_ptr)) kill->friendly = TRUE;
	if (is_friendly(m_ptr)) kill->friendly = TRUE;
#endif /* 0 */

	kill->friendly = FALSE;


	/* Can it attack from a distance? */
	/* Extract the "inate" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags4 & (1L << k)) spell[num++] = k + 32 * 3;
	}

	/* Extract the "normal" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags5 & (1L << k)) spell[num++] = k + 32 * 4;
	}

	/* Extract the "bizarre" spells */
	for (k = 0; k < 32; k++)
	{
		if (r_ptr->flags6 & (1L << k)) spell[num++] = k + 32 * 5;
	}

	/* to cast or not to cast, that is the question */
	if (num)
	{
		kill->ranged_attack = TRUE;
	}
	else
	{
		kill->ranged_attack = FALSE;
	}

	/* Special check on uniques, sometimes the death
	 * is not caught so he thinks they are still running
	 * around
	 */
	if ((r_ptr->flags1 & RF1_UNIQUE) &&
		r_ptr->max_num == 0) borg_race_death[i] = 1;
}


/*
 * Delete an old "kill" record
 */
void borg_delete_kill(int i)
{
	borg_kill *kill = &borg_kills[i];

	map_block *mb_ptr;

	/* Paranoia -- Already wiped */
	if (!kill->r_idx) return;

	/* Note */
	borg_note(format("# Forgetting a monster '%s' at (%d,%d)",
					 (r_name + r_info[kill->r_idx].name), kill->y, kill->x));
#if 0
	/* Reduce the regional fear with this guy dead */
	borg_fear_grid(NULL, kill->y, kill->x,
				   -(borg_danger(kill->y, kill->x, 1, TRUE)), TRUE);
#endif

	/* Bounds checking */
	if (map_in_bounds(kill->x, kill->y))
	{
		mb_ptr = map_loc(kill->x, kill->y);

		/* Update the grids */
		mb_ptr->kill = 0;
	}

	/* save a time stamp of when the last multiplier was killed */
	if (r_info[kill->r_idx].flags2 & RF2_MULTIPLY)
		when_last_kill_mult = borg_t;

	/* Kill the monster */
	WIPE(kill, borg_kill);

	/* One less monster */
	borg_kills_cnt--;

	/* Recalculate danger */
	borg_danger_wipe = TRUE;


	/* Wipe goals */
	goal = 0;

	/* After killing a monster, check for being in a quest */
	/* borg_quest_level = quest_number(borg_skill[BI_CDEPTH]); */

}


/*
 * Force sleep onto a "kill" record
 * ??? Since this check is done at update_kill should I have it here?
 */
static void borg_sleep_kill(int i)
{
	borg_kill *kill = &borg_kills[i];

	/* Paranoia -- Already wiped */
	if (!kill->r_idx) return;

	/* Note */
	borg_note(format("# Noting sleep on a monster '%s' at (%d,%d)",
					 (r_name + r_info[kill->r_idx].name), kill->y, kill->x));

	/* note sleep */
	kill->awake = FALSE;

	/* Recalculate danger */
	borg_danger_wipe = TRUE;
}


/*
 * Determine if a monster should be "viewable"
 */
static bool borg_follow_kill_aux(int i, int y, int x)
{
	int d;

	map_block *mb_ptr;

	borg_kill *kill = &borg_kills[i];

	monster_race *r_ptr = &r_info[kill->r_idx];


	/* Distance to player */
	d = distance(c_y, c_x, y, x);

	/* Too far away */
	if (d > MAX_SIGHT) return (FALSE);

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (FALSE);

	/* Access the grid */
	mb_ptr = map_loc(x, y);

	/* Line of sight */
	if (mb_ptr->info & BORG_MAP_VIEW)
	{
		/* Use "illumination" */
		if (mb_ptr->flags & MAP_SEEN)
		{
			/* We can see invisible */
			if (borg_skill[BI_SINV] || borg_see_inv) return (TRUE);

			/* Monster is not invisible */
			if (!(r_ptr->flags2 & RF2_INVISIBLE)) return (TRUE);
		}

		/* Use "infravision" */
		if (d <= borg_skill[BI_INFRA])
		{
			/* Infravision works on "warm" creatures */
			if (!(r_ptr->flags2 & RF2_COLD_BLOOD)) return (TRUE);
		}
	}


	/* Telepathy requires "telepathy" */
	if (borg_skill[BI_ESP])
	{
		/* Telepathy fails on "strange" monsters */
		if (r_ptr->flags2 & RF2_EMPTY_MIND) return (FALSE);
		if (r_ptr->flags2 & RF2_WEIRD_MIND) return (FALSE);

		/* Success */
		return (TRUE);
	}


	/* Nope */
	return (FALSE);
}


/*
 * Attempt to "follow" a missing monster
 *
 * This routine is not called when the player is blind or hallucinating.
 *
 * Currently this function is a total hack, but handles the case of only
 * one possible location (taking it), two adjacent possible locations
 * (taking the diagonal one), and three adjacent locations with the
 * central one being a diagonal (taking the diagonal one).
 *
 * We should perhaps handle the case of three adjacent locations with
 * the central one being a non-diagonal (taking the non-diagonal one).
 *
 * We should perhaps attempt to take into account "last known direction",
 * which would allow us to "predict" motion up to walls, and we should
 * perhaps attempt to take into account the "standard" flee algorithm,
 * though that feels a little like cheating.
 */
static void borg_follow_kill(int i)
{
	int j;
	int x, y;
	int ox, oy;

	int dx, b_dx = 0;
	int dy, b_dy = 0;

	map_block *mb_ptr;

	borg_kill *kill = &borg_kills[i];

	/* Paranoia */
	if (!kill->r_idx) return;

	/* Old location */
	ox = kill->x;
	oy = kill->y;

	/* Out of sight */
	if (!borg_follow_kill_aux(i, oy, ox)) return;

	mb_ptr = map_loc(ox, oy);

	/* Note */
	borg_note(format("# There was a monster '%s' at (%d,%d)",
					 (r_name + r_info[kill->r_idx].name), oy, ox));


	/* Prevent silliness */
	if (!borg_cave_floor_grid(mb_ptr))
	{
		/* Delete the monster */
		borg_delete_kill(i);

		/* Done */
		return;
	}

	/* Prevent loops */
	if (one_in_(100))
	{
		/* Just delete the monster */
		borg_delete_kill(i);

		/* Done */
		return;
	}

	/* prevent overflows */
	if (borg_t > 20000)
	{
		/* Just delete the monster */
		borg_delete_kill(i);

		/* Done */
		return;
	}

	/* Some never move, no reason to follow them */
	if ((r_info[kill->r_idx].flags1 & RF1_NEVER_MOVE) ||
		/* Some are sleeping and don't move, no reason to follow them */
		(kill->awake == FALSE))
	{
		/* delete them if they are under me */
		if (kill->y == c_y && kill->x == c_x)
		{
			borg_delete_kill(i);
		}
		/* Dont 'forget' certain ones */
		return;
	}

	/* Scan locations */
	for (j = 0; j < 8; j++)
	{
		/* Access offset */
		dx = ddx_ddd[j];
		dy = ddy_ddd[j];

		/* Access location */
		x = ox + dx;
		y = oy + dy;

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Access the grid */
		mb_ptr = map_loc(x, y);

		/* Skip known walls and doors */
		if (!borg_cave_floor_grid(mb_ptr)) continue;

		/* Skip known monsters */
		if (mb_ptr->kill) continue;

		/* Skip visible grids */
		if (borg_follow_kill_aux(i, y, x)) continue;

		/* Collect the offsets */
		b_dx += dx;
		b_dy += dy;
	}


	/* Don't go too far */
	if (b_dx < -1) b_dx = -1;
	else if (b_dx > 1) b_dx = 1;

	/* Don't go too far */
	if (b_dy < -1) b_dy = -1;
	else if (b_dy > 1) b_dy = 1;


	/* Access location */
	x = ox + b_dx;
	y = oy + b_dy;

	/* Access the grid */
	mb_ptr = map_loc(x, y);

	/* Avoid walls and doors */
	if (!borg_cave_floor_grid(mb_ptr))
	{
		/* Just delete the monster */
		borg_delete_kill(i);

		/* Done */
		return;
	}

	/* Avoid monsters */
	if (mb_ptr->kill)
	{
		/* Just delete the monster */
		borg_delete_kill(i);

		/* Done */
		return;
	}

	mb_ptr = map_loc(kill->x, kill->y);

	/* Update the grids */
	mb_ptr->kill = 0;

	/* Save the old Location */
	kill->ox = ox;
	kill->oy = oy;

	/* Save the Location */
	kill->x = ox + b_dx;
	kill->y = oy + b_dy;

	mb_ptr = map_loc(kill->x, kill->y);

	/* Update the grids */
	mb_ptr->kill = i;

	/* Note */
	borg_note(format("# Following a monster '%s' to (%d,%d) from (%d,%d)",
					 (r_name + r_info[kill->r_idx].name),
					 kill->y, kill->x, oy, ox));

	/* Recalculate danger */
	borg_danger_wipe = TRUE;

	/* Clear goals */
	goal = 0;
}


/*
 * Obtain a new "kill" index
 */
static int borg_new_kill(int r_idx, int y, int x)
{
	int i, n = -1;
	int p = 0;

	map_block *mb_ptr;

	borg_kill *kill;

	monster_race *r_ptr;

	/* Look for a "dead" monster */
	for (i = 1; (n < 0) && (i < borg_kills_nxt); i++)
	{
		/* Skip real entries */
		if (!borg_kills[i].r_idx) n = i;
	}

	/* Allocate a new monster */
	if ((n < 0) && (borg_kills_nxt < 256))
	{
		/* Acquire the entry, advance */
		n = borg_kills_nxt++;
	}

	/* Hack -- steal an old monster */
	if (n < 0)
	{
		/* Note */
		borg_note("# Too many monsters");

		/* Hack -- Pick a random monster */
		n = randint1(borg_kills_nxt - 1);

		/* Kill it */
		borg_delete_kill(n);
	}


	/* Count the monsters */
	borg_kills_cnt++;

	/* Access the monster */
	kill = &borg_kills[n];
	r_ptr = &r_info[kill->r_idx];

	/* Save the race */
	kill->r_idx = r_idx;

	/* Location */
	kill->ox = kill->x = x;
	kill->oy = kill->y = y;

	mb_ptr = map_loc(kill->x, kill->y);

	/* Update the grids */
	mb_ptr->kill = n;

	/* Timestamp */
	kill->when = borg_t;

	/* Update the monster */
	borg_update_kill_new(n);

	/* Update the monster */
	borg_update_kill_old(n);

	/* Danger of this monster to its grid (used later) */
	p = borg_danger(kill->y, kill->x, 1, FALSE);

	/* Note */
	borg_note(format("# Creating a monster '%s' at (%d,%d) danger %d",
					 (r_name + r_info[kill->r_idx].name), kill->y, kill->x, p));

#if 0
	/* Add some regional fear (2%) due to this monster */
	borg_fear_grid(NULL, kill->y, kill->x, p * 2 / 100, TRUE);
#endif

	/* Recalculate danger */
	borg_danger_wipe = TRUE;

	/* Wipe goals */
	goal = 0;

	/* Count Hounds */
	if (r_ptr->d_char == 'Z') borg_hound_count++;

	/* Return the monster */
	return (n);
}



/*
 * Attempt to notice a changing "kill"
 */
static bool observe_kill_diff(int y, int x)
{
	int i, r_idx;

	borg_kill *kill;
	map_block *mb_ptr;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (FALSE);

	/* Get grid */
	mb_ptr = map_loc(x, y);

	/* Guess the race */
	r_idx = mb_ptr->monster;

	/* Oops */
	if (!r_idx) return (FALSE);

	/* Create a new monster */
	i = borg_new_kill(r_idx, y, x);

	/* Get the object */
	kill = &borg_kills[i];

	/* Timestamp */
	kill->when = borg_t;

	/* Done */
	return (TRUE);
}


/*
 * Attempt to "track" a "kill" at the given location
 * Assume that the monster moved at most 'd' grids.
 * If "flag" is TRUE, allow monster "conversion"
 */
static bool observe_kill_move(int y, int x, int d, bool flag)
{
	int i, z, ox, oy;
	borg_kill *kill;
	monster_race *r_ptr;

	map_block *mb_ptr;

	/* Bounds checking */
	if (!map_in_bounds(x, y)) return (FALSE);

	/* Look at the monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		kill = &borg_kills[i];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Skip assigned monsters */
		if (kill->seen) continue;

		/* Old location */
		ox = kill->x;
		oy = kill->y;

		/* Bounds checking */
		if (!map_in_bounds(ox, oy)) continue;

		/* Calculate distance */
		z = distance(oy, ox, y, x);

		/* Verify distance */
		if (z > d) continue;

		/* Verify "reasonable" motion, if allowed */
		if (!flag && (z > (kill->moves / 10) + 1)) continue;

		/* Access the monster race */
		r_ptr = &r_info[kill->r_idx];

		/* Actual movement */
		if (z)
		{
			mb_ptr = map_loc(kill->x, kill->y);

			/* Update the grids */
			mb_ptr->kill = 0;

			/* Save the old Location */
			kill->ox = kill->x;
			kill->oy = kill->y;

			/* Save the Location */
			kill->x = x;
			kill->y = y;

			mb_ptr = map_loc(kill->x, kill->y);

			/* Update the grids */
			mb_ptr->kill = i;

			/* Note */
			borg_note(format
					  ("# Tracking a monster '%s' at (%d,%d) from (%d,%d)",
					   (r_name + r_ptr->name), kill->y, kill->x, ox, oy));

			/* Recalculate danger */
			borg_danger_wipe = TRUE;

			/* Clear goals */
			if (!borg_skill[BI_ESP] && goal == GOAL_TAKE) goal = 0;
		}

		/* Note when last seen */
		kill->when = borg_t;

		/* Update the monster */
		borg_update_kill_old(i);

		/* Mark as seen */
		kill->seen = TRUE;

		/* Done */
		return (TRUE);
	}

	/* Oops */
	return (FALSE);
}



/*
 * Calculate base danger from a spell attack by an invisible monster
 *
 * We attempt to take account of various resistances, both in
 * terms of actual damage, and special effects, as appropriate.
 */
static int borg_fear_spell(int i)
{
	int z = 0;
	int p = 0;

	int ouch = 0;


	/* Damage taken */
	if (borg_skill[BI_OLDCHP] > borg_skill[BI_CURHP]) ouch =
			(borg_skill[BI_OLDCHP] - borg_skill[BI_CURHP]) * 2;


	/* Check the spell */
	switch (i)
	{
		case 0:
		{
			/* RF4_SHRIEK */
			p += 10;
			break;
		}

		case 1:
		{
			/* RF4_FAILED spell by monster.  Fear it! */
			/* It could be a unique like Azriel */
			p += borg_skill[BI_CDEPTH];
			break;
		}

		case 2:
		{
			/* RF4_XXX3X4 */
			break;
		}

		case 3:
		{
			/* RF4_XXX4X4 */
			break;
		}

		case 4:
		{
			/* RF4_ARROW_1 */
			z = (1 * 6);
			break;
		}

		case 5:
		{
			/* RF4_ARROW_2 */
			z = (3 * 6);
			break;
		}

		case 6:
		{
			/* RF4_ARROW_3 */
			z = (5 * 6);
			break;
		}

		case 7:
		{
			/* RF4_ARROW_4 */
			z = (7 * 6);
			break;
		}

		case 8:
		{
			/* RF4_BR_ACID */
			if (borg_skill[BI_IACID]) break;
			z = ouch;
			p += 40;
			break;
		}

		case 9:
		{
			/* RF4_BR_ELEC */
			if (borg_skill[BI_IELEC]) break;
			z = ouch;
			p += 20;
			break;
		}

		case 10:
		{
			/* RF4_BR_FIRE */
			if (borg_skill[BI_IFIRE]) break;
			z = ouch;
			p += 40;
			break;
		}

		case 11:
		{
			/* RF4_BR_COLD */
			if (borg_skill[BI_ICOLD]) break;
			z = ouch;
			p += 20;
			break;
		}

		case 12:
		{
			/* RF4_BR_POIS */
			z = ouch;
			if (borg_skill[BI_RPOIS]) break;
			if (my_oppose_pois) break;
			p += 20;
			break;
		}

		case 13:
		{
			/* RF4_BR_NETH */
			z = ouch + 100;
			if (borg_skill[BI_RNTHR]) break;
			p += 50;
			if (borg_skill[BI_HLIFE]) break;
			/* do not worry about drain exp after level 50 */
			if (borg_skill[BI_CLEVEL] >= 50) break;
			p += 150;
			break;
		}

		case 14:
		{
			/* RF4_BR_LITE */
			z = ouch;
			if (borg_skill[BI_RLITE]) break;
			if (borg_skill[BI_RBLIND]) break;
			p += 20;
			break;
		}

		case 15:
		{
			/* RF4_BR_DARK */
			z = ouch;
			if (borg_skill[BI_RDARK]) break;
			if (borg_skill[BI_RBLIND]) break;
			p += 20;
			break;
		}

		case 16:
		{
			/* RF4_BR_CONF */
			z = ouch;
			if (borg_skill[BI_RCONF]) break;
			p += 100;
			break;
		}

		case 17:
		{
			/* RF4_BR_SOUN */
			z = ouch;
			if (borg_skill[BI_RSND]) break;
			p += 50;
			break;
		}

		case 18:
		{
			/* RF4_BR_CHAO */
			z = ouch;
			if (borg_skill[BI_RKAOS]) break;
			p += 200;
			if (!borg_skill[BI_RNTHR]) p += 50;
			if (!borg_skill[BI_HLIFE]) p += 50;
			if (!borg_skill[BI_RCONF]) p += 50;
			if (borg_skill[BI_CLEVEL] == 50) break;
			p += 100;
			break;
		}

		case 19:
		{
			/* RF4_BR_DISE */
			z = ouch;
			if (borg_skill[BI_RDIS]) break;
			p += 500;
			break;
		}

		case 20:
		{
			/* RF4_BR_NEXU */
			z = ouch;
			if (borg_skill[BI_RNXUS]) break;
			p += 100;
			break;
		}

		case 21:
		{
			/* RF4_BR_TIME */
			z = ouch;
			p += 200;
			break;
		}

		case 22:
		{
			/* RF4_BR_INER */
			z = ouch;
			p += 50;
			break;
		}

		case 23:
		{
			/* RF4_BR_GRAV */
			z = ouch;
			p += 50;
			if (borg_skill[BI_RSND]) break;
			p += 50;
			break;
		}

		case 24:
		{
			/* RF4_BR_SHAR */
			z = ouch;
			if (borg_skill[BI_RSHRD]) break;
			p += 50;
			break;
		}

		case 25:
		{
			/* RF4_BR_PLAS */
			z = ouch;
			if (borg_skill[BI_RSND]) break;
			p += 50;
			break;
		}

		case 26:
		{
			/* RF4_BR_WALL */
			z = ouch;
			if (borg_skill[BI_RSND]) break;
			p += 50;
			break;
		}

		case 27:
		{
			/* RF4_BR_MANA */
			/* XXX XXX XXX */
			break;
		}

		case 28:
		{
			/* RF4_XXX5X4 */
			break;
		}

		case 29:
		{
			/* RF4_XXX6X4 */
			break;
		}

		case 30:
		{
			/* RF4_XXX7X4 */
			break;
		}

		case 31:
		{
			/* RF4_XXX8X4 */
			break;
		}

		case 32:
		{
			/* RF5_BA_ACID */
			if (borg_skill[BI_IACID]) break;
			z = ouch;
			p += 40;
			break;
		}

		case 33:
		{
			/* RF5_BA_ELEC */
			if (borg_skill[BI_IELEC]) break;
			z = ouch;
			p += 20;
			break;
		}

		case 34:
		{
			/* RF5_BA_FIRE */
			if (borg_skill[BI_IFIRE]) break;
			z = ouch;
			p += 40;
			break;
		}

		case 35:
		{
			/* RF5_BA_COLD */
			if (borg_skill[BI_ICOLD]) break;
			z = ouch;
			p += 20;
			break;
		}

		case 36:
		{
			/* RF5_BA_POIS */
			z = ouch;
			if (borg_skill[BI_RPOIS]) break;
			p += 20;
			break;
		}

		case 37:
		{
			/* RF5_BA_NETH */
			z = ouch + 100;
			if (borg_skill[BI_RNTHR]) break;
			p += 300;
			break;
		}

		case 38:
		{
			/* RF5_BA_WATE */
			z = ouch;
			p += 50;
			break;
		}

		case 39:
		{
			/* RF5_BA_MANA */
			z = ouch;
			break;
		}

		case 40:
		{
			/* RF5_BA_DARK */
			z = ouch;
			if (borg_skill[BI_RDARK]) break;
			if (borg_skill[BI_RBLIND]) break;
			p += 20;
			break;
		}

		case 41:
		{
			/* RF5_DRAIN_MANA */
			if (borg_skill[BI_MAXSP]) p += 10;
			break;
		}

		case 42:
		{
			/* RF5_MIND_BLAST */
			z = 20;
			break;
		}

		case 43:
		{
			/* RF5_BRAIN_SMASH */
			z = (12 * 15);
			p += 100;
			break;
		}

		case 44:
		{
			/* RF5_CAUSE_1 */
			z = (3 * 8);
			break;
		}

		case 45:
		{
			/* RF5_CAUSE_2 */
			z = (8 * 8);
			break;
		}

		case 46:
		{
			/* RF5_CAUSE_3 */
			z = (10 * 15);
			break;
		}

		case 47:
		{
			/* RF5_CAUSE_4 */
			z = (15 * 15);
			p += 50;
			break;
		}

		case 48:
		{
			/* RF5_BO_ACID */
			if (borg_skill[BI_IACID]) break;
			z = ouch;
			p += 40;
			break;
		}

		case 49:
		{
			/* RF5_BO_ELEC */
			if (borg_skill[BI_IELEC]) break;
			z = ouch;
			p += 20;
			break;
		}

		case 50:
		{
			/* RF5_BO_FIRE */
			if (borg_skill[BI_IFIRE]) break;
			z = ouch;
			p += 40;
			break;
		}

		case 51:
		{
			/* RF5_BO_COLD */
			if (borg_skill[BI_ICOLD]) break;
			z = ouch;
			p += 20;
			break;
		}

		case 52:
		{
			/* RF5_BO_POIS */
			/* XXX XXX XXX */
			break;
		}

		case 53:
		{
			/* RF5_BO_NETH */
			z = ouch + 100;
			if (borg_skill[BI_RNTHR]) break;
			p += 200;
			break;
		}

		case 54:
		{
			/* RF5_BO_WATE */
			z = ouch;
			p += 20;
			break;
		}

		case 55:
		{
			/* RF5_BO_MANA */
			z = ouch;
			break;
		}

		case 56:
		{
			/* RF5_BO_PLAS */
			z = ouch;
			p += 20;
			break;
		}

		case 57:
		{
			/* RF5_BO_ICEE */
			z = ouch;
			p += 20;
			break;
		}

		case 58:
		{
			/* RF5_MISSILE */
			z = ouch;
			break;
		}

		case 59:
		{
			/* RF5_SCARE */
			p += 10;
			break;
		}

		case 60:
		{
			/* RF5_BLIND */
			p += 10;
			break;
		}

		case 61:
		{
			/* RF5_CONF */
			p += 10;
			break;
		}

		case 62:
		{
			/* RF5_SLOW */
			p += 5;
			break;
		}

		case 63:
		{
			/* RF5_HOLD */
			p += 20;
			break;
		}

		case 64:
		{
			/* RF6_HASTE */
			p += 10 + borg_skill[BI_CDEPTH];
			break;
		}

		case 65:
		{
			/* RF6_XXX1X6 */
			break;
		}

		case 66:
		{
			/* RF6_HEAL */
			p += 10;
			break;
		}

		case 67:
		{
			/* RF6_XXX2X6 */
			break;
		}

		case 68:
		{
			/* RF6_XXX3X6 */
			break;
		}

		case 69:
		{
			/* RF6_XXX4X6 */
			break;
		}

		case 70:
		{
			/* RF6_TELE_TO */
			p += 20 + borg_skill[BI_CDEPTH];
			break;
		}

		case 71:
		{
			/* RF6_TELE_AWAY */
			p += 10;
			break;
		}

		case 72:
		{
			/* RF6_TELE_LEVEL */
			p += 50;
			break;
		}

		case 73:
		{
			/* RF6_XXX5 */
			break;
		}

		case 74:
		{
			/* RF6_DARKNESS */
			break;
		}

		case 75:
		{
			/* RF6_TRAPS */
			p += 50;
			break;
		}

		case 76:
		{
			/* RF6_FORGET */
			/* if you have lots of cash this is not very scary... just re-ID. */
			if (borg_skill[BI_CLEVEL] < 35)
				p += 500;
			else
				p += 50;
			break;
		}

		case 77:
		{
			/* RF6_XXX6X6 */
			break;
		}

		case 78:
		{
			/* RF6_XXX7X6 */
			break;
		}

		case 79:
		{
			/* RF6_XXX8X6 */
			break;
		}

		case 80:
		{
			/* RF6_S_MONSTER */
			p += 55;
			break;
		}

		case 81:
		{
			/* RF6_S_MONSTERS */
			p += 30;
			break;
		}

		case 82:
		{
			/* RF6_S_ANT */
			p += 15;
			break;
		}

		case 83:
		{
			/* RF6_S_SPIDER */
			p += 25;
			break;
		}

		case 84:
		{
			/* RF6_S_HOUND */
			p += 45;
			break;
		}

		case 85:
		{
			/* RF6_S_HYDRA */
			p += 70;
			break;
		}

		case 86:
		{
			/* RF6_S_ANGEL */
			p += 80;
			break;
		}

		case 87:
		{
			/* RF6_S_DEMON */
			p += 80;
			break;
		}

		case 88:
		{
			/* RF6_S_UNDEAD */
			p += 80;
			break;
		}

		case 89:
		{
			/* RF6_S_DRAGON */
			p += 80;
			break;
		}

		case 90:
		{
			/* RF6_S_HI_UNDEAD */
			p += 95;
			break;
		}

		case 91:
		{
			/* RF6_S_HI_DRAGON */
			p += 95;
			break;
		}

		case 92:
		{
			/* RF6_S_WRAITH */
			p += 95;
			break;
		}

		case 93:
		{
			/* RF6_S_UNIQUE */
			p += 50;
			break;
		}
	}

	/* Things which hurt us alot need to be a concern */
	if (ouch >= borg_skill[BI_CURHP] / 2) ouch = ouch * 2;

	/* Notice damage */
	return (p + z);
}




/*
 * Attempt to locate a monster which could explain a message involving
 * the given monster name, near the given location, up to the given
 * distance from the given location.
 *
 * Invisible monsters, bizarre monsters, and unexplainable monsters are
 * all treated the same way, and should eventually contribute some amount
 * of basic "fear" to the current region.
 *
 * First, we attempt to convert "similar" objects into the desired monster,
 * then we attempt to convert "similar" monsters into the desired monster,
 * then we attempt to match an existing monster, and finally, we give up.
 *
 * XXX XXX XXX Hack -- To prevent fatal situations, every time we think
 * there may be a monster nearby, we look for a nearby object which could
 * be the indicated monster, and convert it into that monster.  This allows
 * us to correctly handle a room full of multiplying clear mushrooms.
 *
 * XXX XXX XXX When surrounded by multiple monsters of the same type,
 * we will ascribe most messages to one of those monsters, and ignore
 * the existance of all the other similar monsters.
 *
 * XXX XXX XXX Currently, confusion may cause messages to be ignored.
 */
static int borg_locate_kill(cptr who, int y, int x, int r)
{
	int i, d, r_idx;

	int b_i, b_d;

	borg_kill *kill;

	monster_race *r_ptr;

	/* Handle invisible monsters */
	if (streq(who, "It") || streq(who, "Someone") || streq(who, "Something"))
	{
		/* Note */
		borg_note("# Possible Invisible monster nearby.");
		borg_note("# Possible Offscreen monster nearby");

		/* Shift the panel */
		need_shift_panel = TRUE;

		/* if I can, cast detect inviso--time stamp it
		 * We stamp it now if we can, or later if we just did the spell
		 * That way we dont loop casting the spell.    APW
		 */
		/* detect invis spell not working right, for now just shift panel
		 * and cast a light beam if in a hallway and we have see_inv*/
		if (need_see_inviso < (borg_t))
		{
			need_see_inviso = (borg_t);
		}

		/* Ignore */
		return (0);
	}

	/* Handle offsreen monsters */
	if (suffix(who, " (offscreen)"))
	{
		/* Note */
		borg_note("# Offscreen monster nearby");

		/* Shift the panel */
		need_shift_panel = TRUE;

		/* Ignore */
		return (0);
	}

	/* Guess the monster race */
	r_idx = borg_guess_race_name(who);

	/* Access the monster race */
	r_ptr = &r_info[r_idx];

	/* Note */
	borg_note(format("# There is a monster '%s' within %d grids of %d,%d",
					 (r_name + r_ptr->name), r, y, x));

	/* Hack -- count racial appearances */
	if (borg_race_count[r_idx] < MAX_SHORT) borg_race_count[r_idx]++;


	/* Handle trappers and lurkers and mimics */
	if (r_ptr->flags1 & (RF1_CHAR_CLEAR | RF1_CHAR_MIMIC))
	{
		/* Note */
		borg_note("# Bizarre monster nearby");
	}

#if 0
	/*** Hack -- Find a similar object ***/

	/* Nothing yet */
	b_i = -1;
	b_d = 999;

	/* Scan the objects */
	for (i = 1; i < borg_takes_nxt; i++)
	{
		take = &borg_takes[i];

		/* Skip "dead" objects */
		if (!take->k_idx) continue;

		/* Access kind */
		k_ptr = &k_info[take->k_idx];

		/* Verify char */
		if (k_ptr->d_char != r_ptr->d_char) continue;

		/* Verify attr (unless clear or multi-hued) */
		if (!(r_ptr->flags1 & (RF1_ATTR_MULTI | RF1_ATTR_CLEAR)))
		{
			/* Verify attr (unless flavored) */
			if (!(k_ptr->flavor))
			{
				/* Verify attr */
				if (k_ptr->d_attr != r_ptr->d_attr) continue;
			}
		}

		/* Bounds checking */
		if (!map_in_bounds(take->x, take->y)) continue;

		/* Calculate distance */
		d = distance(take->y, take->x, y, x);

		/* Skip "wrong" objects */
		if (d > r) continue;

		/* Track closest one */
		if (d > b_d) continue;

		/* Track it */
		b_i = i;
		b_d = d;
	}

	/* Found one */
	if (b_i >= 0)
	{
		take = &borg_takes[b_i];

		/* Access kind */
		k_ptr = &k_info[take->k_idx];

		/* Note */
		borg_note(format("# Converting an object '%s' at (%d,%d)",
						 (k_name + k_ptr->name), take->y, take->x));

		/* Save location */
		x = take->x;
		y = take->y;

		/* Delete the object */
		borg_delete_take(b_i);

		/* Make a new monster */
		b_i = borg_new_kill(r_idx, y, x);

		/* Get the monster */
		kill = &borg_kills[b_i];

		/* Timestamp */
		kill->when = borg_t;

		/* Return the index */
		return (b_i);
	}

#endif /* 0 */

#if 0
	/*** Hack -- Find a similar monster ***/

	/* Nothing yet */
	b_i = -1;
	b_d = 999;

	/* Scan the monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		kill = &borg_kills[i];

		/* Skip "dead" monsters */
		if (!kill->r_idx) continue;

		/* Skip "matching" monsters */
		if (kill->r_idx == r_idx) continue;

		/* Verify char */
		if (r_info[kill->r_idx].d_char != r_ptr->d_char) continue;

		/* Verify attr (unless clear or multi-hued) */
		if (!(r_ptr->flags1 & (RF1_ATTR_MULTI | RF1_ATTR_CLEAR)))
		{
			/* Verify attr */
			if (r_info[kill->r_idx].d_attr != r_ptr->d_attr) continue;
		}

		/* Distance away */
		d = distance(kill->y, kill->x, y, x);

		/* Check distance */
		if (d > r) continue;

		/* Track closest one */
		if (d > b_d) continue;

		/* Track it */
		b_i = i;
		b_d = d;
	}

	/* Found one */
	if (b_i >= 0)
	{
		kill = &borg_kills[b_i];

		/* Note */
		borg_note(format("# Converting a monster '%s' at (%d,%d)",
						 (r_name + r_info[kill->r_idx].name),
						 kill->y, kill->x));

		/* Change the race */
		kill->r_idx = r_idx;

		/* Update the monster */
		borg_update_kill_new(b_i);

		/* Update the monster */
		borg_update_kill_old(b_i);

		/* Recalculate danger */
		borg_danger_wipe = TRUE;

		/* Clear goals */
		if (!borg_skill[BI_ESP] && goal == GOAL_TAKE) goal = 0;

		/* Index */
		return (b_i);
	}

#endif /* 0 */

	/*** Hack -- Find an existing monster ***/

	/* Nothing yet */
	b_i = -1;
	b_d = 999;

	/* Scan the monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		kill = &borg_kills[i];

		/* Skip "dead" monsters */
		if (!kill->r_idx) continue;

		/* Skip "different" monsters */
		if (kill->r_idx != r_idx) continue;

		/* Distance away */
		d = distance(kill->y, kill->x, y, x);

		/* In a darkened room with ESP we can get hit and ignore it */
		/* Check distance */
		if (d > r) continue;

		/* Hopefully this will add fear to our grid */
		if (!borg_projectable(kill->y, kill->x, y, x)) continue;

		/* Track closest one */
		if (d > b_d) continue;

		/* Track it */
		b_i = i;
		b_d = d;
	}

	/* Found one */
	if (b_i >= 0)
	{
		kill = &borg_kills[b_i];

		/* Note */
		borg_note(format("# Matched a monster '%s' at (%d,%d)",
						 (r_name + r_info[kill->r_idx].name),
						 kill->y, kill->x));


		/* Index */
		return (b_i);
	}


	/*** Oops ***/

	/* Note */
	borg_note(format("# Ignoring a monster '%s' near (%d,%d)",
					 (r_name + r_ptr->name), y, x));

	/* Oops */
	/* this is the case where we know the name of the monster */
	/* but cannot locate it on the monster list. */
	return (-1);
}




/*
 * Notice the "death" of a monster
 */
static void borg_count_death(int i)
{
	int r_idx;

	borg_kill *kill = &borg_kills[i];

	/* Access race */
	r_idx = kill->r_idx;

	/* Hack -- count racial deaths */
	if (borg_race_death[r_idx] < MAX_SHORT) borg_race_death[r_idx]++;

	/* if it was a unique then remove the unique_on_level flag */
	if (r_info[kill->r_idx].flags1 & RF1_UNIQUE) unique_on_level = FALSE;

}


/* Mark detected region */
static void detect_region(byte flag)
{
	int x, y;

	map_block *mb_ptr;
	
	for (x = c_x - BORG_MAX_DETECT; x <= c_x + BORG_MAX_DETECT; x++)
	{
		for (y = c_y - BORG_MAX_DETECT; y <= c_y + BORG_MAX_DETECT; y++)
		{
			/* bounds checking */
			if (!map_in_bounds(x, y)) continue;
			
			/* Check distance */
			if (distance(c_x, c_y, x, y) > BORG_MAX_DETECT) continue;
			
			mb_ptr = map_loc(x, y);
			
			/* Set flag */
			mb_ptr->detect |= flag;
		}
	}
}

/*
 * Handle "detection" spells and "call lite"
 *
 * Note that we must use the "old" player location
 */
static bool borg_handle_self(cptr str)
{
	/* Handle failure */
	if (borg_failure)
	{
		borg_note("# Something failed");
	}

	/* Handle "call lite" */
	else if (prefix(str, "lite"))
	{
		/* Message */
		borg_note(format("# Called lite at (%d,%d)", o_c_y, o_c_x));
	}


	/* Handle "detect walls" */
	else if (prefix(str, "wall"))
	{
		/* Message */
		borg_note("# Detected walls");
		
		/* Mark detected walls */
		detect_region(BORG_DETECT_WALL);
	}

	/* Handle "detect traps" */
	else if (prefix(str, "trap"))
	{
		/* Message */
		borg_note("# Detected traps");

		/* Mark detected traps */
		detect_region(BORG_DETECT_TRAP);
	}

	/* Handle "detect doors" */
	else if (prefix(str, "door"))
	{
		/* Message */
		borg_note("# Detected doors");

		/* Mark detected doors */
		detect_region(BORG_DETECT_DOOR);
	}

	/* Handle "detect traps and doors" */
	else if (prefix(str, "both"))
	{
		/* Message */
		borg_note("# Detected traps and doors");

		/* Mark detected traps + doors */
		detect_region(BORG_DETECT_TRAP | BORG_DETECT_DOOR);
	}

	/* Handle "detect traps and doors and evil" */
	else if (prefix(str, "TDE"))
	{
		/* Message */
		borg_note("# Detected traps, doors & evil");
		
		/* Mark detected traps + doors + evil */
		detect_region(BORG_DETECT_TRAP | BORG_DETECT_DOOR | BORG_DETECT_EVIL);
	}

	/* Handle "detect evil" */
	else if (prefix(str, "evil"))
	{
		/* Message */
		borg_note("# Detected evil");

		/* Mark detected evil */
		detect_region(BORG_DETECT_EVIL);
	}

	/* Done */
	return (TRUE);
}

/*
 * Hack - save old hook to overhead map.
 *
 * We chain into this after storing our information.
 * (This is so multiple sub-systems can hook into
 * map changes.)
 */
map_info_hook_type old_info_hook = NULL;

/*
 * Save the borg information into the overhead map
 */
void borg_map_info(map_block *mb_ptr, term_map *map)
{
	int i;

	int x = map->x;
	int y = map->y;

	bool old_wall;
    bool new_wall;

	/* Save the old "wall" or "door" */
	old_wall = !borg_cave_floor_grid(mb_ptr);

    /* Don't overwrite known info with unknown */
    /* XXX This needs fixing */
    if (map->terrain != FEAT_NONE)
    {
        mb_ptr->terrain = map->terrain;
    }

	/*
	 * Examine monsters
	 */
	if (map->monster)
	{
		borg_wank *wank;

		/* Check for memory overflow */
        if (borg_wank_num == AUTO_VIEW_MAX)
            borg_oops("too many objects or monsters");

		/* Access next wank, advance */
		wank = &borg_wanks[borg_wank_num++];

		/* Save some information */
		wank->x = x;
		wank->y = y;
		wank->is_kill = TRUE;
	}

	/*
	 * Examine objects
	 */
	if (map->object || map->unknown)
	{
		borg_wank *wank;

		/* Check for memory overflow */
        if (borg_wank_num == AUTO_VIEW_MAX)
            borg_oops("too many objects or monsters");

		/* Access next wank, advance */
		wank = &borg_wanks[borg_wank_num++];

		/* Save some information */
		wank->x = x;
		wank->y = y;
		wank->is_take = TRUE;
	}

	/* Analyze terrain */
	switch (map->terrain)
	{
			/* Up stairs */
		case FEAT_LESS:
		{
			/* Check for an existing "up stairs" */
			for (i = 0; i < track_less_num; i++)
			{
				/* Stop if we already new about these stairs */
				if ((track_less_x[i] == x) && (track_less_y[i] == y)) break;
			}

			/* Track the newly discovered "up stairs" */
			if ((i == track_less_num) && (i < track_less_size))
			{
				track_less_x[i] = x;
				track_less_y[i] = y;
				track_less_num++;
			}
			/* Done */
			break;
		}

			/* Down stairs */
		case FEAT_MORE:
		{
			/* Check for an existing "down stairs" */
			for (i = 0; i < track_more_num; i++)
			{
				/* We already knew about that one */
				if ((track_more_x[i] == x) && (track_more_y[i] == y)) break;
			}

			/* Track the newly discovered "down stairs" */
			if ((i == track_more_num) && (i < track_more_size))
			{
				track_more_x[i] = x;
				track_more_y[i] = y;
				track_more_num++;
			}

			/* Done */
			break;
		}

#if 0							/* Need to use fields */
			/* Traps */
		case FEAT_TRAP_TRAPDOOR:
		case FEAT_TRAP_PIT:
		case FEAT_TRAP_SPIKED_PIT:
		case FEAT_TRAP_POISON_PIT:
		case FEAT_TRAP_TY_CURSE:
		case FEAT_TRAP_TELEPORT:
		case FEAT_TRAP_FIRE:
		case FEAT_TRAP_ACID:
		case FEAT_TRAP_SLOW:
		case FEAT_TRAP_LOSE_STR:
		case FEAT_TRAP_LOSE_DEX:
		case FEAT_TRAP_LOSE_CON:
		case FEAT_TRAP_BLIND:
		case FEAT_TRAP_CONFUSE:
		case FEAT_TRAP_POISON:
		case FEAT_TRAP_SLEEP:
		case FEAT_TRAP_TRAPS:
		{

			/* Assume trap door */
			ag->feat = FEAT_TRAP_TRAPDOOR;

			/* Done */
			break;
		}
#endif /* 0 */



#if 0							/* Need to parse fields */
			/* glyph of warding stuff here */
		case FEAT_MINOR_GLYPH:
		case FEAT_GLYPH:
		{
			ag->feat = FEAT_GLYPH;

			/* Check for an existing glyph */
			for (i = 0; i < track_glyph_num; i++)
			{
				/* Stop if we already new about this glyph */
				if ((track_glyph_x[i] == x) && (track_glyph_y[i] == y)) break;
			}

			/* Track the newly discovered glyph */
			if ((i == track_glyph_num) && (i < track_glyph_size))
			{
				track_glyph_x[i] = x;
				track_glyph_y[i] = y;
				track_glyph_num++;
			}

			/* done */
			break;
		}
#endif /* 0 */
	}

    /* Save the new "wall" or "door" */
    /*
     * XXX This depends in a very unhealthy way on borg_cave_floor grid
     * being a macro.
     */
	new_wall = !borg_cave_floor_grid(map);

	/* Notice wall changes */
	if (old_wall != new_wall)
	{
		/* Remove this grid from any flow */
		if (new_wall) mb_ptr->flow = 255;

		/* Remove this grid from any flow */
		mb_ptr->info &= ~(BORG_MAP_ICKY | BORG_MAP_KNOW);

		/* Recalculate the view (if needed) */
		if (mb_ptr->info & BORG_MAP_VIEW) borg_do_update_view = TRUE;
	}

	/* Finally - chain into the old hook, if it exists */
	if (old_info_hook) old_info_hook(mb_ptr, map);
}



/*
 * Update the Borg based on the current "map"
 */
static void borg_forget_map(void)
{
	map_block *mb_ptr;

	/* Itterate over the map */
	MAP_ITT_START (mb_ptr)
	{
		/* Clear flow information */
		mb_ptr->cost = 255;
		mb_ptr->flow = 255;

		/* Clear icky + know flag */
		mb_ptr->info &= ~(BORG_MAP_ICKY | BORG_MAP_KNOW);
	}
	MAP_ITT_END;

	/* Forget the view */
	borg_forget_view();
}

static byte Get_f_info_number[256];

/*
 * Update the "map" based on visual info on the screen
 *
 * Note that we make assumptions about the grid under the player,
 * to prevent painful situations such as seeming to be standing
 * in a wall, or on a trap, etc.
 *
 * In general, we use the same "feat" codes as the game itself, but
 * sometimes we are just guessing (as with "visible traps"), and we
 * use some special codes, explained below.
 *
 * Note that we use the "feat" code of "FEAT_NONE" for grids which
 * have never been seen, or which, when seen, have always contained
 * an object or monster.  These grids are probably walls, unless
 * they contain a monster or object, in which case they are probably
 * floors, unless they contain a monster which passes through walls,
 * in which case they are probably walls.
 *
 * Note that we use the "feat" code of "FEAT_FLOOR" for grids which
 * were a normal floor last time we checked.  These grids may have
 * changed into non-floor grids recently (via earthquake?), unless
 * the grid is on the current panel, and is currently "lit" in some
 * manner, and does not contain a monster.
 *
 * Note that we use the "feat" code of "FEAT_INVIS" for grids which
 * once contained a wall/door, but then contained a monster or object.
 * These grids are probably floors, unless the grid contains a monster
 * which can pass through walls, in which case note that missiles and
 * spells may not affect a monster in the grid.
 *
 * Note that we use the other "feat" codes for grids which probably
 * contain the given feature type, unless several feature types use
 * the same symbol, in which case we use some "default" code, changing
 * our guess when messages provide us with more information.  This is
 * especially necessary for distinguishing magma from quartz, and for
 * distinguishing normal doors from locked doors from jammed doors.
 * Note that most "feat" codes, even if they are not "guesses", may
 * not be valid unless the grid is on the current panel.
 *
 * Note the "interesting" code used to learn which floor grids are "dark"
 * and which are "perma-lit", by tracking those floor grids which appear
 * to be "lit", and then marking all of these grids which do not appear
 * to be lit by the torch as "known" to be illuminated, and by marking
 * any grids which "disappear" or which are displayed as "dark floors"
 * as "known" to be "dark".  This leaves many grids, especially those
 * lit by the torch, as being neither lit nor dark.
 *
 * The basic problem is that, especially with no special options set,
 * the player has very little direct information about which grids
 * are perma-lit, since all non-floor grids are memorized when they
 * are seen, and torch-lit floor grids look just like perma-lit
 * floor grids.  Also, monsters hide any object or feature in their
 * grid, and objects hide any feature in their grid, and objects are
 * memorized when they are seen, and monsters can be detected by a
 * variety of methods, including infravision and telepathy.
 *
 * So we ignore most non-floor grids, and we mark any floor grids which
 * are "known" to be perma-lit as "BORG_GLOW", and any which are "known"
 * to be dark as "BORG_DARK".  These flags are used for many purposes,
 * most importantly, to determine when "call lite" would be useful, and
 * to help determine when a monster is standing in a viewable perma-lit
 * grid, and should thus be "visible", and to determine when the player
 * has "lite", even though his torch has gone out.
 *
 * When a "call lite" spell succeeds, we mark the grids around the
 * player as "BORG_GLOW" and not "BORG_DARK", but we do not attempt
 * to "spread" the lite to the entire room, since, in general, it is
 * not possible to know what grids are in the "room".
 *
 * Note that we assume that normally, when the player steps onto
 * something, it disappears, and turns into a normal floor, unless
 * the player is stepping onto a grid which is normally "permanent"
 * (floors, stairs, store doors), in which case it does not change.
 *
 * Note that when we encounter a grid which blocks motion, but which
 * was previously thought to not block motion, we must be sure to
 * remove it from any "flow" which might be in progress, to prevent
 * nasty situations in which we attempt to flow into a wall grid
 * which was thought to be something else, like an unknown grid.
 *
 * In Zborg, the wilderness level shops and stairs are
 * handled in borg_update() under new levels.
 */
static void borg_update_map(void)
{
	int i;

	/* Hack - simply read location of player from game... */

	/* Memorize player location */
	c_x = p_ptr->px;
	c_y = p_ptr->py;

	/* Mark this grid as having been stepped on */
	track_step_x[track_step_num] = c_x;
	track_step_y[track_step_num] = c_y;
	track_step_num++;

	/* Hack - Clean the steps every so often */
	if (track_step_num > 75)
	{
		for (i = 0; i < 75; i++)
		{
			/* Move each step down one position */
			track_step_x[i] = track_step_x[i + 1];
			track_step_y[i] = track_step_y[i + 1];
		}
		/* reset the count */
		track_step_num = 75;
	}
}

/* Cheat the feature codes into memory.  Used on Wilderness
 * Levels mostly.
 */
static void borg_cheat_feats(void)
{
	int x, y;
	int i;

	map_block *mb_ptr;

	/* Currently only cheat towns and wilderness */
	if (borg_skill[BI_CDEPTH] == 0)
	{
		MAP_ITT_START (mb_ptr)
		{
			/* Dungeon Stair Location */
			if (mb_ptr->terrain == FEAT_MORE)
			{
				/* Get location */
				MAP_GET_LOC(x, y);

				/* Check for an existing "down stairs" */
				for (i = 0; i < track_more_num; i++)
				{
					/* We already knew about that one */
					if ((track_more_x[i] == x) && (track_more_y[i] == y))
					{
						break;
					}
				}

				/* Track the newly discovered "down stairs" */
				if ((i == track_more_num) && (i < track_more_size))
				{
					track_more_x[i] = x;
					track_more_y[i] = y;
					track_more_num++;
				}
			}


#if 0
			/* Shop Entry */
			if (cave[y][x].feat >= FEAT_SHOP_HEAD &&
				cave[y][x].feat <= FEAT_SHOP_TAIL)
			{
				for (i = 0; i < MAX_STORES; i++)
				{
					if (cave[y][x].feat == FEAT_SHOP_HEAD + i)
					{
						/* Shop Location */
						track_shop_x[i] = x;
						track_shop_y[i] = y;
					}
				}
			}
#endif /* 0 */

		}
		MAP_ITT_END;
	}
}

/*
 * Look at the screen and update the borg
 *
 * Uses the "panel" info (w_x, w_y) obtained earlier
 *
 * Note that all the "important" messages that occured after our last
 * action have been "queued" in a usable form.  We must attempt to use
 * these messages to update our knowledge about the world, keeping in
 * mind that the world may have changed in drastic ways.
 *
 * Note that the "borg_t" variable corresponds *roughly* to player turns,
 * except that resting and "repeated" commands count as a single turn,
 * and "free" moves (including "illegal" moves, such as attempted moves
 * into walls, or tunneling into monsters) are counted as turns.
 *
 * Also note that "borg_t" is not incremented until the Borg is about to
 * do something, so nothing ever has a time-stamp of the current time.
 *
 * We rely on the fact that all "perma-lit" grids are memorized when
 * they are seen, so any grid on the current panel that appears "dark"
 * must not be perma-lit.
 *
 * We rely on the fact that all "objects" are memorized when they are
 * seen, so any grid on the current panel that appears "dark" must not
 * have an object in it.  But it could have a monster which looks like
 * an object, but this is very rare.  XXX XXX XXX
 *
 * XXX XXX XXX The basic problem with timestamping the monsters
 * and objects is that we often get a message about a monster, and so
 * we want to timestamp it, but then we cannot use the timestamp to
 * indicate that the monster has not been "checked" yet.  Perhaps
 * we need to do something like give each monster a "moved" flag,
 * and clear all the flags to FALSE each turn before tracking. (?)
 *
 * Note that when two monsters of the same race are standing next to
 * each other, and they both move, such that the second monster ends
 * up where the first one began, we will incorrectly assume that the
 * first monster stayed still, and either the second monster moved
 * two spaces, or the second monster disappeared and a third monster
 * appeared, which is technically possible, if the first monster ate
 * the second, and then cloned the third.
 *
 * There is a problem with monsters which look like objects, namely,
 * they are assumed to be objects, and then if they leave line of
 * sight, they disappear, and the Borg assumes that they are gone,
 * when really they should be identified as monsters.
 *
 * XXX XXX Hack -- note the fast direct access to the screen.
 */
void borg_update(void)
{
	int i, k;

	int hit_dist;

	cptr msg;

	cptr what;

	bool reset = FALSE;


	/*** Process objects/monsters ***/

	/* Scan monsters */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		borg_kill *kill = &borg_kills[i];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Clear flags */
		kill->seen = FALSE;
		kill->used = FALSE;

		/* Skip recently seen monsters */
		if (borg_t - kill->when < 2000) continue;

		/* Note */
		borg_note(format("# Expiring a monster '%s' (%d) at (%d,%d)",
						 (r_name + r_info[kill->r_idx].name), kill->r_idx,
						 kill->y, kill->x));

		/* Kill the monster */
		borg_delete_kill(i);
	}

	/* Scan objects */
	for (i = 1; i < borg_takes_nxt; i++)
	{
		borg_take *take = &borg_takes[i];

		/* Skip dead objects */
		if (!take->k_idx) continue;

		/* Clear flags */
		take->seen = FALSE;

		/* Skip recently seen objects */
		if (borg_t - take->when < 2000) continue;

		/* Note */
		borg_note(format("# Expiring an object '%s' (%d) at (%d,%d)",
						 (k_name + k_info[take->k_idx].name), take->k_idx,
						 take->y, take->x));

		/* Kill the object */
		borg_delete_take(i);
	}

	/*** Handle messages ***/

	/* Process messages */
	for (i = 0; i < borg_msg_num; i++)
	{
		/* Get the message */
		msg = borg_msg_buf + borg_msg_pos[i];

		/* Note the message */
		borg_note(format("# %s (+)", msg));
	}

	/* Process messages */
	for (i = 0; i < borg_msg_num; i++)
	{
		/* Skip parsed messages */
		if (borg_msg_use[i]) continue;

		/* Get the message */
		msg = borg_msg_buf + borg_msg_pos[i];

		/* Get the arguments */
		what = strchr(msg, ':') + 1;

		/* Hack -- Handle "SELF" info */
		if (prefix(msg, "SELF:"))
		{
			(void)borg_handle_self(what);
			borg_msg_use[i] = 1;
		}

		/* Handle "You feel..." */
		else if (prefix(msg, "FEELING:"))
		{
			borg_feeling = atoi(what);
			borg_msg_use[i] = 1;
		}
	}

	/* Process messages */
	for (i = 0; i < borg_msg_num; i++)
	{
		/* Skip parsed messages */
		if (borg_msg_use[i]) continue;

		/* Get the message */
		msg = borg_msg_buf + borg_msg_pos[i];

		/* Get the arguments */
		what = strchr(msg, ':') + 1;

		/* Handle "You hit xxx." */
		if (prefix(msg, "HIT:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 0)) > 0)
			{
				borg_msg_use[i] = 2;
			}
		}

		/* Handle "You miss xxx." */
		else if (prefix(msg, "MISS:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 0)) > 0)
			{
				borg_msg_use[i] = 2;
			}
		}

		/* Handle "You have killed xxx." */
		else if (prefix(msg, "KILL:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 0)) > 0)
			{
				borg_count_death(k);

				borg_delete_kill(k);
				borg_msg_use[i] = 2;
				/* reset the panel.  He's on a roll */
				time_this_panel = 1;
			}
			/* Shooting through darkness worked */
			if (successful_target < 0) successful_target = 2;

		}

		/* Handle "The xxx disappears!"  via teleport other, and blinks away */
		else if (prefix(msg, "BLINK:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 0)) > 0)
			{
				borg_delete_kill(k);
				borg_msg_use[i] = 2;
				/* reset the panel.  He's on a roll */
				time_this_panel = 1;
			}
			/* Shooting through darkness worked */
			if (successful_target < 0) successful_target = 2;
		}

		/* Handle "xxx dies." */
		else if (prefix(msg, "DIED:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 3)) > 0)
			{
				borg_count_death(k);
				borg_delete_kill(k);
				borg_msg_use[i] = 2;
				/* reset the panel.  He's on a roll */
				time_this_panel = 1;
			}
			/* Shooting through darkness worked */
			if (successful_target < 0) successful_target = 2;
		}

		/* Handle "xxx screams in pain." */
		else if (prefix(msg, "PAIN:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 3)) > 0)
			{
				borg_msg_use[i] = 2;
			}
			/* Shooting through darkness worked */
			if (successful_target < 0) successful_target = 2;
		}

		/* Handle "sleep" */
		else if (prefix(msg, "STATE__FEAR:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 0)) > 0)
			{
				borg_msg_use[i] = 2;
			}
		}

		/* Handle "sleep" */
		else if (prefix(msg, "STATE__BOLD:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 0)) > 0)
			{
				borg_msg_use[i] = 2;
			}
		}
		else if (prefix(msg, "STATE_SLEEP:"))
		{
			/* Shooting through darkness worked */
			if (successful_target < 0) successful_target = 2;
		}
		else if (prefix(msg, "STATE__FEAR:"))
		{
			/* Shooting through darkness worked */
			if (successful_target < 0) successful_target = 2;
		}
		else if (prefix(msg, "STATE_CONFUSED:"))
		{
			/* Shooting through darkness worked */
			if (successful_target < 0) successful_target = 2;
		}


	}

	/* Process messages */
	/* getting distance to allow for 'hit's */
	hit_dist = 1;
	for (i = 0; i < borg_msg_num; i++)
	{
		/* Skip parsed messages */
		if (borg_msg_use[i]) continue;

		/* Get the message */
		msg = borg_msg_buf + borg_msg_pos[i];

		/* if you have moved than do not count the monsters as unknown */
		/* unless they are very far away */
		if (prefix(msg, "SPELL_70") || prefix(msg, "SPELL_71"))
		{
			hit_dist = 100;
			break;
		}

		/* monsters move from earthquake */
		if (prefix(msg, "QUAKE"))
		{
			hit_dist = 3;
			break;
		}
	}

	/* Process messages */
	for (i = 0; i < borg_msg_num; i++)
	{
		/* Skip parsed messages */
		if (borg_msg_use[i]) continue;

		/* Get the message */
		msg = borg_msg_buf + borg_msg_pos[i];

		/* Get the arguments */
		what = strchr(msg, ':') + 1;

		/* Handle "You hit xxx." */
		if (prefix(msg, "HIT:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, hit_dist)) > 0)
			{
				borg_msg_use[i] = 3;
			}
		}

		/* Handle "You miss xxx." */
		else if (prefix(msg, "MISS:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, hit_dist)) > 0)
			{
				borg_msg_use[i] = 3;
			}
		}

		/* Handle "You have killed xxx." */
		else if (prefix(msg, "KILL:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 1)) > 0)
			{
				borg_count_death(k);
				borg_delete_kill(k);
				borg_msg_use[i] = 3;
				/* reset the panel.  He's on a roll */
				time_this_panel = 1;
			}
			/* Shooting through darkness worked */
			if (successful_target == -1) successful_target = 2;
		}

		/* Handle "The xxx disappears!"  via teleport other, and blinks away */
		else if (prefix(msg, "BLINK:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, g_y, g_x, 1)) > 0)
			{
				borg_delete_kill(k);
				borg_msg_use[i] = 3;
				/* reset the panel.  He's on a roll */
				time_this_panel = 1;
			}
			/* Shooting through darkness worked */
			if (successful_target == -1) successful_target = 2;
		}


		/* Handle "xxx dies." */
		else if (prefix(msg, "DIED:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 20)) > 0)
			{
				borg_count_death(k);
				borg_delete_kill(k);
				borg_msg_use[i] = 3;
				/* reset the panel.  He's on a roll */
				time_this_panel = 1;
			}
			/* Shooting through darkness worked */
			if (successful_target == -1) successful_target = 2;
		}

		/* Handle "xxx screams in pain." */
		else if (prefix(msg, "PAIN:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 20)) > 0)
			{
				borg_msg_use[i] = 3;
			}
			/* Shooting through darkness worked */
			if (successful_target == -1) successful_target = 2;
		}

		/* Handle "xxx hits you." */
		else if (prefix(msg, "HIT_BY:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 1)) > 0)
			{
				borg_msg_use[i] = 3;
			}
		}

		/* Handle "xxx misses you." */
		else if (prefix(msg, "MISS_BY:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 1)) > 0)
			{
				borg_msg_use[i] = 3;
			}
		}

		/* Handle "sleep" */
		else if (prefix(msg, "STATE_SLEEP:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 20)) > 0)
			{
				borg_sleep_kill(k);
				borg_msg_use[i] = 3;
			}
		}

		/* Handle "awake" */
		else if (prefix(msg, "STATE_AWAKE:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 20)) > 0)
			{
				borg_msg_use[i] = 3;
			}
		}

		/* Handle "sleep" */
		else if (prefix(msg, "STATE__FEAR:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 20)) > 0)
			{
				borg_msg_use[i] = 3;
			}
		}

		/* Handle "sleep" */
		else if (prefix(msg, "STATE__BOLD:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 20)) > 0)
			{
				borg_msg_use[i] = 3;
			}
		}

		/* Hack -- Handle "spell" */
		else if (prefix(msg, "SPELL_"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, o_c_y, o_c_x, 20)) > 0)
			{
				borg_msg_use[i] = 3;
			}
		}
	}

	/* if we didn't successfully target,
	   mark the first unknown in the path as a wall
	   If we mark a wall, let the borg shoot again */
	if (successful_target < 0)
	{
		if (successful_target > -10)
		{
			successful_target -= 10;
			if (borg_target_unknown_wall(g_y, g_x))
				successful_target = 2;
		}
	}

	/*** Handle new levels ***/

	/* Hack -- note new levels */
	if (old_depth != borg_skill[BI_CDEPTH] || borg_respawning >= 3)
	{
		/* if we are not leaving town increment time since town clock */
		if (!old_depth)
			borg_time_town = 0;
		else
			borg_time_town += borg_t - borg_began;

		/* Hack -- Restart the clock */
		borg_t = 1000;

		/* reset our panel clock */
		time_this_panel = 1;

		/* reset our vault/unique check */
		vault_on_level = FALSE;
		unique_on_level = FALSE;
		scaryguy_on_level = FALSE;

		/* reset our breeder flag */
		breeder_level = FALSE;

		/* reset our need to see inviso clock */
		need_see_inviso = 1;

		/* reset our 'shoot in the dark' flag */
		successful_target = 0;

		/* When level was begun */
		borg_began = borg_t;

		/* Not completed */
		borg_completed = FALSE;

		/* New danger thresh-hold */
		avoidance = borg_skill[BI_CURHP];

		/* Wipe the danger */
		borg_danger_wipe = TRUE;

		/* Clear our sold flags */
		sold_item_tval = 0;
		sold_item_pval = 0;
		sold_item_tval = 0;
		sold_item_sval = 0;
		sold_item_pval = 0;
		sold_item_store = 0;

		/* Update some stuff */
		borg_do_update_view = TRUE;

		/* Examine the world */
		borg_do_inven = TRUE;
		borg_do_equip = TRUE;
		borg_do_spell = TRUE;
		borg_do_panel = TRUE;
		borg_do_frame = TRUE;

		/* Enable some functions */
		borg_do_crush_junk = TRUE;
		borg_do_crush_hole = TRUE;
		borg_do_crush_slow = TRUE;

		/* Allow Pets to stick close */
		p_ptr->pet_follow_distance = PET_FOLLOW_ME;

		/* Mega-Hack -- Clear "call lite" stamp */
		when_call_lite = 0;

		/* Mega-Hack -- Clear "wizard lite" stamp */
		when_wizard_lite = 0;

		/* Mega-Hack -- Clear "detect traps" stamp */
		when_detect_traps = 0;

		/* Mega-Hack -- Clear "detect doors" stamp */
		when_detect_doors = 0;

		/* Mega-Hack -- Clear "detect walls" stamp */
		when_detect_walls = 0;

		/* Mega-Hack -- Clear "detect evil" stamp */
		when_detect_evil = 0;

		/* Hack -- Clear "shop visit" stamps */
		for (i = 0; i < (MAX_STORES); i++) borg_shops[i].when = 0;


		/* No goal yet */
		goal = 0;

		/* Hack -- Clear "shop" goals */
		goal_shop = goal_ware = goal_item = -1;

		/* Reset food in store */
		borg_food_onsale = -1;

		/* Do not use any stairs */
		stair_less = stair_more = FALSE;

		/* Hack -- cannot rise past town */
		if (!borg_skill[BI_CDEPTH]) goal_rising = FALSE;

		/* Assume not leaving the level */
		goal_leaving = FALSE;

		/* Assume not fleeing the level */
		goal_fleeing = FALSE;

		/* Assume not ignoring monsters */
		goal_ignoring = FALSE;

		/* No known stairs */
		track_less_num = 0;
		track_more_num = 0;

		/* No known glyph */
		track_glyph_num = 0;

		/* No known steps */
		track_step_num = 0;

		/* No known doors */
		track_door_num = 0;

		/* No objects here */
		borg_takes_cnt = 0;
		borg_takes_nxt = 1;

		/* Forget old objects */
		C_WIPE(borg_takes, 256, borg_take);

		/* No monsters here */
		borg_kills_cnt = 0;
		borg_kills_nxt = 1;

		/* Reset the hound Genocide */
		genocide_level_hounds = FALSE;

		/* Forget old monsters */
		C_WIPE(borg_kills, 256, borg_kill);

		/* Hack -- Forget race counters */
		C_WIPE(borg_race_count, z_info->r_max, s16b);

		/* Hack -- Rarely, a Unique can die off screen and the borg will miss it.
		 * This check will cheat to see if uniques are dead.
		 */
		/*Extract dead uniques */
		for (i = 1; i < z_info->r_max; i++)
		{
			monster_race *r_ptr = &r_info[i];

			/* Skip non-monsters */
			if (!r_ptr->name) continue;

			/* Skip non-uniques */
			if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

			/* Mega-Hack -- Access "dead unique" list */
			if (r_ptr->max_num == 0) borg_race_death[i] = 1;
		}


		/* Forget the map */
		borg_forget_map();

		/* Reset */
		reset = TRUE;

		/* wipe out bad artifacts list */
		for (i = 0; i < 50; i++)
		{
			bad_obj_x[i] = -1;
			bad_obj_y[i] = -1;
		}

		/* save once per level */
		if (borg_flag_save) borg_save = TRUE;

		/* Save new depth */
		old_depth = borg_skill[BI_CDEPTH];

		borg_times_twitch = 0;
		borg_escapes = 0;

		/* Cheat Town(s) / Wilderness / Shop info */
		borg_cheat_feats();

		/* Check for being in a quest */
		/* borg_quest_level = quest_number(borg_skill[BI_CDEPTH]); */
	}

	/* Handle old level */
	else
	{
		/* reduce GOI count. NOTE: do not reduce below 1.  That is done */
		/* when the spell is cast. */
		if (borg_goi > 1)
		{
			borg_goi -= borg_game_ratio;
		}

		/* Count down to blast off */
		if (goal_recalling > 1)
		{
			goal_recalling -= borg_game_ratio;

			/* dont let it get to 0 or borg will recast the spell */
			if (goal_recalling <= 0) goal_recalling = 1;
		}

		/* when we need to cast this spell again */
		if (borg_see_inv > 1)
		{
			borg_see_inv -= borg_game_ratio;
		}

		/* Reduce fear over time */
		if (!(borg_t % 10))
		{
			map_block *mb_ptr;
		
			MAP_ITT_START(mb_ptr)
			{
				if (mb_ptr->fear) mb_ptr->fear--;
			}
			MAP_ITT_END;
		}

		/* Handle changing map panel */
		if ((o_w_x != w_x) || (o_w_y != w_y))
		{
			/* Time stamp this new panel-- to avoid a repeated motion bug */
			time_this_panel = 1;

		}

		/* Examine the world while in town. */
		if (!borg_skill[BI_CDEPTH]) borg_do_inven = TRUE;
		if (!borg_skill[BI_CDEPTH]) borg_do_equip = TRUE;


	}


	/*** Update the map ***/

	/* Update the map */
	borg_update_map();

	/* Reset */
	if (reset)
	{
		/* Fake old panel */
		o_w_x = w_x;
		o_w_y = w_y;

		/* Fake old location */
		o_c_x = c_x;
		o_c_y = c_y;

		/* Fake goal location */
		g_x = c_x;
		g_y = c_y;
	}

	/* Player moved */
	if ((o_c_x != c_x) || (o_c_y != c_y))
	{
		/* Update view */
		borg_do_update_view = TRUE;

		/* Assume I can shoot here */
		successful_target = 0;
	}

	/* Update the view */
	if (borg_do_update_view)
	{
		/* Update the view */
		borg_update_view();

		/* Take note */
		borg_do_update_view = FALSE;
	}

	/*** Track objects and monsters ***/

	/* Pass 1 -- stationary monsters */
	for (i = borg_wank_num - 1; i >= 0; i--)
	{
		borg_wank *wank = &borg_wanks[i];

		/* Track stationary monsters */
		if (wank->is_kill && observe_kill_move(wank->y, wank->x, 0, FALSE))
		{
			/* Hack -- excise the entry */
			borg_wanks[i] = borg_wanks[--borg_wank_num];
		}
	}
	/* Pass 2 -- stationary objects */
	for (i = borg_wank_num - 1; i >= 0; i--)
	{
		borg_wank *wank = &borg_wanks[i];

		/* Track stationary objects */
		if (wank->is_take && observe_take_move(wank->y, wank->x, 0))
		{
			/* Hack -- excise the entry */
			borg_wanks[i] = borg_wanks[--borg_wank_num];
		}
	}
	/* Pass 3a -- moving monsters (distance 1) */
	for (i = borg_wank_num - 1; i >= 0; i--)
	{
		borg_wank *wank = &borg_wanks[i];

		/* Track moving monsters */
		if (wank->is_kill && observe_kill_move(wank->y, wank->x, 1, FALSE))
		{
			/* Hack -- excise the entry */
			borg_wanks[i] = borg_wanks[--borg_wank_num];
		}
	}
	/* Pass 3b -- moving monsters (distance 2) */
	for (i = borg_wank_num - 1; i >= 0; i--)
	{
		borg_wank *wank = &borg_wanks[i];

		/* Track moving monsters */
		if (wank->is_kill && observe_kill_move(wank->y, wank->x, 2, FALSE))
		{
			/* Hack -- excise the entry */
			borg_wanks[i] = borg_wanks[--borg_wank_num];
		}
	}
	/* Pass 3c -- moving monsters (distance 3) */
	for (i = borg_wank_num - 1; i >= 0; i--)
	{
		borg_wank *wank = &borg_wanks[i];

		/* Track moving monsters */
		if (wank->is_kill && observe_kill_move(wank->y, wank->x, 3, FALSE))
		{
			/* Hack -- excise the entry */
			borg_wanks[i] = borg_wanks[--borg_wank_num];
		}
	}
	/* Pass 3d -- moving monsters (distance 3, allow changes) */
	for (i = borg_wank_num - 1; i >= 0; i--)
	{
		borg_wank *wank = &borg_wanks[i];

		/* Track moving monsters */
		if (wank->is_kill && observe_kill_move(wank->y, wank->x, 3, TRUE))
		{
			/* Hack -- excise the entry */
			borg_wanks[i] = borg_wanks[--borg_wank_num];
		}
	}
	/* Pass 4 -- new objects */
	for (i = borg_wank_num - 1; i >= 0; i--)
	{
		borg_wank *wank = &borg_wanks[i];

		/* Track new objects */
		if (wank->is_take && observe_take_diff(wank->y, wank->x))
		{
			/* Hack -- excise the entry */
			borg_wanks[i] = borg_wanks[--borg_wank_num];
		}
	}
	/* Pass 5 -- new monsters */
	for (i = borg_wank_num - 1; i >= 0; i--)
	{
		borg_wank *wank = &borg_wanks[i];

		/* Track new monsters */
		if (wank->is_kill && observe_kill_diff(wank->y, wank->x))
		{
			/* Hack -- excise the entry */
			borg_wanks[i] = borg_wanks[--borg_wank_num];
		}
	}

	/* Ignore certain Wanks */
	if (borg_wank_num)
	{
#if 0
		int i;
		for (i = 0; i < borg_wank_num; i++)
		{
			borg_wank *wank = &borg_wanks[i];
			borg_note(format("# Unresolved Wank at %d,%d. #%d of %d",
							 wank->y, wank->x, i, borg_wank_num));
		}
#endif

		/* Clear Unresolved Wanks */
		borg_wank_num = 0;
	}

	/*** Handle messages ***/

	/* Process messages */
	for (i = 0; i < borg_msg_num; i++)
	{
		/* Skip parsed messages */
		if (borg_msg_use[i]) continue;

		/* Get the message */
		msg = borg_msg_buf + borg_msg_pos[i];

		/* Get the arguments */
		what = strchr(msg, ':') + 1;

		/* Handle "xxx dies." */
		if (prefix(msg, "DIED:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, c_y, c_x, 20)) > 0)
			{
				borg_count_death(k);
				borg_delete_kill(k);
				borg_msg_use[i] = 4;
			}
		}

		/* Handle "xxx screams in pain." */
		else if (prefix(msg, "PAIN:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, c_y, c_x, 20)) > 0)
			{
				borg_msg_use[i] = 4;
			}
		}

		/* Handle "xxx hits you." */
		else if (prefix(msg, "HIT_BY:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, c_y, c_x, hit_dist)) > 0)
			{
				borg_msg_use[i] = 4;
			}
		}

		/* Handle "xxx misses you." */
		else if (prefix(msg, "MISS_BY:"))
		{
			/* Attempt to find the monster */

			if ((k = borg_locate_kill(what, c_y, c_x, hit_dist)) > 0)
			{
				borg_msg_use[i] = 4;
			}
		}

		/* Handle "sleep" */
		else if (prefix(msg, "STATE_SLEEP:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, c_y, c_x, 20)) > 0)
			{
				borg_sleep_kill(k);
				borg_msg_use[i] = 4;
			}
		}

		/* Handle "awake" */
		else if (prefix(msg, "STATE_AWAKE:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, c_y, c_x, 20)) > 0)
			{
				borg_msg_use[i] = 4;
			}
		}

		/* Handle "sleep" */
		else if (prefix(msg, "STATE__FEAR:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, c_y, c_x, 20)) > 0)
			{
				borg_msg_use[i] = 4;
			}
		}

		/* Handle "sleep" */
		else if (prefix(msg, "STATE__BOLD:"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, c_y, c_x, 20)) > 0)
			{
				borg_msg_use[i] = 4;
			}
		}

		/* Hack -- Handle "spell" */
		else if (prefix(msg, "SPELL_"))
		{
			/* Attempt to find the monster */
			if ((k = borg_locate_kill(what, c_y, c_x, 20)) > 0)
			{
				borg_msg_use[i] = 4;
			}
		}
	}
	/* Process messages */
	for (i = 0; i < borg_msg_num; i++)
	{
		/* Skip parsed messages */
		if (borg_msg_use[i]) continue;

		/* Get the message */
		msg = borg_msg_buf + borg_msg_pos[i];

		/* Get the arguments */
		what = strchr(msg, ':') + 1;

		/* Handle "xxx hits you." */
		if (prefix(msg, "HIT_BY:"))
		{
			borg_fear_grid(what, c_y, c_x,
						   4 * ((borg_skill[BI_CDEPTH] / 5) + 1), FALSE);
			borg_msg_use[i] = 5;
		}

		/* Handle "xxx misses you." */
		else if (prefix(msg, "MISS_BY:"))
		{
			borg_fear_grid(what, c_y, c_x,
						   2 * ((borg_skill[BI_CDEPTH] / 5) + 1), FALSE);
			borg_msg_use[i] = 5;
		}

		/* Hack -- Handle "spell" */
		else if (prefix(msg, "SPELL_"))
		{
			borg_fear_grid(what, c_y, c_x, borg_fear_spell(atoi(msg + 6)),
						   FALSE);
			borg_msg_use[i] = 5;
		}
	}
	/* Display messages */
	for (i = 0; i < borg_msg_num; i++)
	{
		/* Get the message */
		msg = borg_msg_buf + borg_msg_pos[i];

		/* Final message */
		borg_note(format("# %s (%d)", msg, borg_msg_use[i]));
	}


	/*** Notice missing monsters ***/
	/* Scan the monster list */
	for (i = 1; i < borg_kills_nxt; i++)
	{
		borg_kill *kill = &borg_kills[i];

		/* Skip dead monsters */
		if (!kill->r_idx) continue;

		/* Skip seen monsters */
		if (kill->when == borg_t) continue;

		/* Hack -- blind or hallucinating */
		if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISIMAGE]) continue;

		/* Predict the monster */
		borg_follow_kill(i);
	}

	/*** Notice missing objects ***/

	/* Scan the object list */
	for (i = 1; i < borg_takes_nxt; i++)
	{
		borg_take *take = &borg_takes[i];

		/* Skip dead objects */
		if (!take->k_idx) continue;

		/* Skip seen objects */
		if (take->when == borg_t) continue;

		/* Hack -- blind or hallucinating */
		if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISIMAGE]) continue;

		/* Follow the object */
		borg_follow_take(i);
	}

	/*** Various things ***/

	/* Forget goals while "impaired" in any way */
	if (borg_skill[BI_ISBLIND] || borg_skill[BI_ISCONFUSED] ||
		borg_skill[BI_ISAFRAID] || borg_skill[BI_ISIMAGE]) goal = 0;

	/* Forget goals while "bleeding" in any way */
	if (borg_skill[BI_ISWEAK] || borg_skill[BI_ISPOISONED] ||
		borg_skill[BI_ISCUT] || borg_skill[BI_ISSTUN] ||
		borg_skill[BI_ISHEAVYSTUN]) goal = 0;

	/* Forget goals when HP changes */
	if (borg_skill[BI_CURHP] < borg_skill[BI_OLDCHP]) goal = 0;

	/* Save the hit points */
	borg_skill[BI_OLDCHP] = borg_skill[BI_CURHP];

	/* Save the spell points */
	borg_skill[BI_OLDCSP] = borg_skill[BI_CURSP];

	/* Forget failure */
	borg_failure = FALSE;

	/* Forget the messages */
	borg_msg_len = 0;
	borg_msg_num = 0;


	/*** Save old info ***/

	/* Save the old "location" */
	o_c_x = c_x;
	o_c_y = c_y;

	/* Save the old "panel" */
	o_w_x = w_x;
	o_w_y = w_y;


	/*** Defaults ***/

	/* Default "goal" location */
	g_x = c_x;
	g_y = c_y;
}


/*
 * Handle various "important" messages
 *
 * Actually, we simply "queue" them for later analysis
 */
void borg_react(cptr msg, cptr buf)
{
	int len;

	if (borg_dont_react)
		return;

	/* Note actual message */
	borg_note(format("> %s", msg));

	/* Extract length of parsed message */
	len = strlen(buf);

	/* Verify space */
	if (borg_msg_num + 1 > borg_msg_max)
	{
		borg_oops("too many messages");
		return;
	}

	/* Verify space */
	if (borg_msg_len + len + 1 > borg_msg_siz)
	{
		borg_oops("too much messages");
		return;
	}

	/* Assume not used yet */
	borg_msg_use[borg_msg_num] = 0;

	/* Save the message position */
	borg_msg_pos[borg_msg_num] = borg_msg_len;

	/* Save the message text */
	strcpy(borg_msg_buf + borg_msg_len, buf);

	/* Advance the buf */
	borg_msg_len += len + 1;

	/* Advance the pos */
	borg_msg_num++;
}



/*
 * Sorting hook -- comp function -- see below
 *
 * We use "u" to point to an array of strings, and "v" to point to
 * an array of indexes, and we sort them together by the strings.
 */
static bool ang_sort_comp_hook_strings(vptr u, vptr v, int a, int b)
{
	cptr *text = (cptr *)(u);
	s16b *what = (s16b *)(v);

	int cmp;

	/* Compare the two strings */
	cmp = (strcmp(text[a], text[b]));

	/* Strictly less */
	if (cmp < 0) return (TRUE);

	/* Strictly more */
	if (cmp > 0) return (FALSE);

	/* Enforce "stable" sort */
	return (what[a] <= what[b]);
}


/*
 * Sorting hook -- swap function -- see below
 *
 * We use "u" to point to an array of strings, and "v" to point to
 * an array of indexes, and we sort them together by the strings.
 */
static void ang_sort_swap_hook_strings(vptr u, vptr v, int a, int b)
{
	cptr *text = (cptr *)(u);
	s16b *what = (s16b *)(v);

	cptr texttmp;
	s16b whattmp;

	/* Swap "text" */
	texttmp = text[a];
	text[a] = text[b];
	text[b] = texttmp;

	/* Swap "what" */
	whattmp = what[a];
	what[a] = what[b];
	what[b] = whattmp;
}



/*
 * Initialize this file
 */
void borg_init_5(void)
{
	int i;

	int size;

	s16b *what;
	cptr *text;

	/* Allocate temp arrays */
	C_MAKE(what, z_info->r_max, s16b);
	C_MAKE(text, z_info->r_max, cptr);

	/*** Message tracking ***/

	/* No chars saved yet */
	borg_msg_len = 0;

	/* Maximum buffer size */
	borg_msg_siz = 4096;

	/* Allocate a buffer */
	C_MAKE(borg_msg_buf, borg_msg_siz, char);

	/* No msg's saved yet */
	borg_msg_num = 0;

	/* Maximum number of messages */
	borg_msg_max = 256;

	/* Allocate array of positions */
	C_MAKE(borg_msg_pos, borg_msg_max, s16b);

	/* Allocate array of use-types */
	C_MAKE(borg_msg_use, borg_msg_max, s16b);


	/*** Object/Monster tracking ***/

	/* Array of "wanks" */
	C_MAKE(borg_wanks, AUTO_VIEW_MAX, borg_wank);


	/*** Reset the map ***/

	/* Forget the map */
	borg_forget_map();


	/*** Parse "unique" monster names ***/

	/* Start over */
	size = 0;

	/* Collect "unique" monsters */
	for (i = 1; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Skip non-monsters */
		if (!r_ptr->name) continue;

		/* Skip non-unique monsters */
		if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

		/* Use it */
		text[size] = r_name + r_ptr->name;
		what[size] = i;
		size++;
	}

	/* Set the sort hooks */
	ang_sort_comp = ang_sort_comp_hook_strings;
	ang_sort_swap = ang_sort_swap_hook_strings;

	/* Sort */
	ang_sort(text, what, size);

	/* Save the size */
	borg_unique_size = size;

	/* Allocate the arrays */
	C_MAKE(borg_unique_text, borg_unique_size, cptr);
	C_MAKE(borg_unique_what, borg_unique_size, s16b);

	/* Save the entries */
	for (i = 0; i < size; i++) borg_unique_text[i] = text[i];
	for (i = 0; i < size; i++) borg_unique_what[i] = what[i];


	/*** Parse "normal" monster names ***/

	/* Start over */
	size = 0;

	/* Collect "normal" monsters */
	for (i = 1; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Skip non-monsters */
		if (!r_ptr->name) continue;

		/* Skip unique monsters */
		if (r_ptr->flags1 & RF1_UNIQUE) continue;

		/* Use it */
		text[size] = r_name + r_ptr->name;
		what[size] = i;
		size++;
	}

	/* Set the sort hooks */
	ang_sort_comp = ang_sort_comp_hook_strings;
	ang_sort_swap = ang_sort_swap_hook_strings;

	/* Sort */
	ang_sort(text, what, size);

	/* Save the size */
	borg_normal_size = size;

	/* Allocate the arrays */
	C_MAKE(borg_normal_text, borg_normal_size, cptr);
	C_MAKE(borg_normal_what, borg_normal_size, s16b);

	/* Save the entries */
	for (i = 0; i < size; i++) borg_normal_text[i] = text[i];
	for (i = 0; i < size; i++) borg_normal_what[i] = what[i];

	/* Initialize */
	for (i = 0; i < 256; i++) Get_f_info_number[i] = -1;

	for (i = z_info->f_max - 1; i >= 0; i--)
	{
		if (i == FEAT_SECRET || i == FEAT_INVIS)
			continue;

		Get_f_info_number[(int)f_info[i].d_char] = i;
	}

	/* Free the arrays */
	FREE(what);
	FREE(text);
}




#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
