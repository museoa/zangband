/* File: melee2.c */

/* Purpose: Monster spells and movement */

/*
* Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research, and
* not for profit purposes provided that this copyright and statement are
* included in all such copies.
*/

/*
* This file has several additions to it by Keldon Jones (keldon@umr.edu)
* to improve the general quality of the AI (version 0.1.1).
*/

#include "angband.h"

#define SPEAK_CHANCE 8
#define GRINDNOISE 20
#define CYBERNOISE 20

/*
 * Convert the target a monster has
 * to a direction to move in.
 *
 * This uses the old direction encoding still.
 */
static void convert_target_dir(monster_type *m_ptr, int *mm)
{
	int x = m_ptr->tx;
	int y = m_ptr->ty;

	/* Paranoia */
	if (!x && !y) return;

	/* Extract the direction */
	x -= m_ptr->fx;
	y -= m_ptr->fy;

	/* North */
	if ((y < 0) && (x == 0))
	{
		mm[0] = 8;
		mm[1] = 7;
		mm[2] = 9;
	}
	/* South */
	else if ((y > 0) && (x == 0))
	{
		mm[0] = 2;
		mm[1] = 1;
		mm[2] = 3;
	}
	/* East */
	else if ((x > 0) && (y == 0))
	{
		mm[0] = 6;
		mm[1] = 9;
		mm[2] = 3;
	}
	/* West */
	else if ((x < 0) && (y == 0))
	{
		mm[0] = 4;
		mm[1] = 7;
		mm[2] = 1;
	}
	/* North-West */
	else if ((y < 0) && (x < 0))
	{
		mm[0] = 7;
		mm[1] = 4;
		mm[2] = 8;
	}
	/* North-East */
	else if ((y < 0) && (x > 0))
	{
		mm[0] = 9;
		mm[1] = 6;
		mm[2] = 8;
	}
	/* South-West */
	else if ((y > 0) && (x < 0))
	{
		mm[0] = 1;
		mm[1] = 4;
		mm[2] = 2;
	}
	/* South-East */
	else if ((y > 0) && (x > 0))
	{
		mm[0] = 3;
		mm[1] = 6;
		mm[2] = 2;
	}
}


/*
 * Is the monster worth targetting?
 */
static bool nice_target(monster_type *m_ptr, monster_race *r_ptr,
						monster_type *t_ptr)
{
	/* The monster itself isn't a target */
	if (t_ptr == m_ptr) return (FALSE);

	/* Paranoia -- Skip dead monsters */
	if (!t_ptr->r_idx) return (FALSE);

	/* Monster must be 'an enemy' */
	if (!are_enemies(m_ptr, t_ptr)) return (FALSE);

	/* Mega Hack - Monster must be close */
	if (ABS(m_ptr->fy - t_ptr->fy) + ABS(m_ptr->fx - t_ptr->fx) > 20)
	{
		return (FALSE);
	}

	if (is_pet(m_ptr))
	{
		/* Hack -- only fight away from player */
		if (p_ptr->pet_follow_distance < 0)
		{
			/* No fighting near player */
			if (t_ptr->cdis <= (0 - p_ptr->pet_follow_distance))
			{
				return (FALSE);
			}
		}
		/* Hack -- no fighting away from player */
		else if ((m_ptr->cdis < t_ptr->cdis) &&
				 (t_ptr->cdis > p_ptr->pet_follow_distance))
		{
			return (FALSE);
		}
	}

	/* Monster must be projectable if we can't pass through walls */
	if (!(r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL)) &&
		!projectable(m_ptr->fx, m_ptr->fy, t_ptr->fx, t_ptr->fy))
	{
		return (FALSE);
	}

	/* We have a target */
	return (TRUE);
}


/*
 * Find an enemy to target
 */
static bool get_enemy_target(monster_type *m_ptr)
{
	int i;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	monster_type *t_ptr;

	cave_type *c_ptr;


	/* Do we already have a nice enemy? */
	if (m_ptr->tx || m_ptr->ty)
	{
		c_ptr = area(m_ptr->tx, m_ptr->ty);

		/* Is there a monster on our target? */
		if (c_ptr->m_idx)
		{
			t_ptr = &m_list[c_ptr->m_idx];

			/* Is it a a good monster to target? */
			if (nice_target(m_ptr, r_ptr, t_ptr)) return (TRUE);
		}

		/* Forget target */
		m_ptr->tx = 0;
		m_ptr->ty = 0;
	}

	/*
	 * Scan through all monsters
	 *
	 * Scanning backwards seems to give the best results -
	 * We tend to find the "newer" monsters first.
	 * "Newer" monsters tend to be closer. 
	 */
	for (i = m_max; i > 0; i--)
	{
		t_ptr = &m_list[i];

		if (nice_target(m_ptr, r_ptr, t_ptr))
		{
			/* OK -- we've got a target */
			m_ptr->ty = t_ptr->fy;
			m_ptr->tx = t_ptr->fx;

			/* Pick the first such monster... */
			return (TRUE);
		}
	}

	/* No monster found */
	return (FALSE);
}

#define HURT_TERRAIN_CHANCE		10


/*
 * Can the monster enter this grid?  How easy is it for them to do so?
 *
 * The code that uses this function sometimes assumes that it will never 
 * return a value greater than 100.
 *
 * The usage of exp to determine whether one monster can kill another is 
 * a kludge.  Maybe use HPs, plus a big bonus for acidic monsters 
 * against monsters that don't like acid.
 *
 * The usage of exp to determine whether one monster can push past 
 * another is also a tad iffy, but ensures that black orcs can always 
 * push past other black orcs.
 */
static int cave_passable_mon(monster_type *m_ptr, cave_type *c_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Assume nothing in the grid other than the terrain hinders movement */
	int move_chance = 100;

	int feat = c_ptr->feat;

	/* The grid is occupied by the player. */
	if (character_dungeon && (area(p_ptr->px, p_ptr->py) == c_ptr))
	{
		/* Monster has no melee blows - character's grid is off-limits. */
		if (r_ptr->flags1 & (RF1_NEVER_BLOW)) return (0);
	}

	/* The grid is occupied by a monster. */
	else if (c_ptr->m_idx)
	{
		monster_type *n_ptr = &m_list[c_ptr->m_idx];
		monster_race *nr_ptr = &r_info[n_ptr->r_idx];

		/* Kill weaker monsters + attack 'enemies' */
		if (((r_ptr->flags2 & (RF2_KILL_BODY)) && (r_ptr->mexp > nr_ptr->mexp))
			|| are_enemies(m_ptr, n_ptr) || m_ptr->confused)
		{
			/* This is a null statement... */
			move_chance = 100;
		}

		/* Push past weaker or similar monsters */
		else if ((r_ptr->flags2 & (RF2_MOVE_BODY))
				 && (r_ptr->mexp >= nr_ptr->mexp))
		{
			/* It's easier to push past weaker monsters */
			if (r_ptr->mexp == nr_ptr->mexp)
			{
				move_chance = 50;
			}
			else
			{
				move_chance = 80;
			}
		}
		else
		{
			/* Cannot do anything to clear away the other monster */
			return (0);
		}
	}

	/* Insert field logic here */

	/*** Check passability of various features. ***/

	/* Feature is not a wall */
	if (floor_grid(feat))
	{
		/* Ocean */
		if (feat == FEAT_OCEAN_WATER)
		{
			if (!(r_ptr->flags8 & RF8_WILD_OCEAN)) return (HURT_TERRAIN_CHANCE);
		}

		/* Deep water */
		else if (feat == FEAT_DEEP_WATER)
		{
			if (!((r_ptr->flags7 & RF7_AQUATIC) ||
				  (r_ptr->flags7 & RF7_CAN_FLY) ||
				  (r_ptr->flags7 & RF7_CAN_SWIM)))
			{
				return (HURT_TERRAIN_CHANCE);
			}
		}

		/* Shallow water */
		else if (feat == FEAT_SHAL_WATER)
		{
			if (r_ptr->flags2 & RF2_AURA_FIRE) return (0);
		}

		/* Aquatic creatues need water */
		if ((r_ptr->flags7 & RF7_AQUATIC) && !(r_ptr->flags7 & RF7_CAN_FLY))
		{
			return (0);
		}

		/* Lava */
		if ((feat == FEAT_SHAL_LAVA) || (feat == FEAT_DEEP_LAVA))
		{
			if (!((r_ptr->flags3 & RF3_IM_FIRE) ||
				  (r_ptr->flags7 & RF7_CAN_FLY)))
			{
				return (HURT_TERRAIN_CHANCE);
			}
		}

		/* Acid */
		else if ((feat == FEAT_SHAL_ACID) || (feat == FEAT_DEEP_ACID))
		{
			if (!((r_ptr->flags3 & RF3_IM_ACID) ||
				  (r_ptr->flags7 & RF7_CAN_FLY)))
			{
				return (HURT_TERRAIN_CHANCE);
			}
		}

		/* Swamp */
		else if ((feat == FEAT_SHAL_SWAMP) || (feat == FEAT_DEEP_SWAMP))
		{
			if (!((r_ptr->flags3 & RF3_IM_POIS) ||
				  (r_ptr->flags7 & RF7_CAN_FLY)))
			{
				return (HURT_TERRAIN_CHANCE);
			}
		}

		/* Anything else that's not a wall we assume to be passable. */
		return (move_chance);
	}

	/* Semi-transparent terrains are no obstacle */
	if ((feat & 0x60) == 0x60)
	{
		return (move_chance);
	}

	/* Closed or secret doors can be opened */
	if ((feat == FEAT_CLOSED) || (feat == FEAT_SECRET))
	{
		if (((r_ptr->flags2 & RF2_OPEN_DOOR)
			 || (r_ptr->flags2 & RF2_BASH_DOOR))
			&& (!is_pet(m_ptr) || p_ptr->pet_open_doors))
		{
			/* Opening doors takes time */
			return (move_chance / 2);
		}
	}

	/* Permanent walls + the pattern block movement */
	else if ((feat >= FEAT_PERM_EXTRA) && (feat <= FEAT_PATTERN_XTRA2))
	{
		return (0);
	}

	/* Monster can burrow through walls */
	if ((r_ptr->flags2 & (RF2_PASS_WALL)) || (r_ptr->flags2 & (RF2_KILL_WALL)))
	{
		return (move_chance);
	}

	/* Otherwise cannot pass through */
	return (0);
}




/*
 * Hack, based on mon_take_hit... perhaps all monster attacks on
 * other monsters should use this?
 */
void mon_take_hit_mon(int m_idx, int dam, bool *fear, cptr note)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	char m_name[160];

	/* Can the player be aware of this attack? */
	bool known = (m_ptr->cdis <= MAX_SIGHT);

	/* Extract monster name */
	monster_desc(m_name, m_ptr, 0);

	/* Redraw (later) if needed */
	if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);

	/* Wake it up */
	m_ptr->csleep = 0;

	if (m_ptr->invulner && !one_in_(PENETRATE_INVULNERABILITY))
	{
		if (m_ptr->ml)
		{
			msg_format("%^s is unharmed.", m_name);
		}

		return;
	}

	/* Hurt it */
	m_ptr->hp -= dam;

	/* It is dead now... or is it? */
	if (m_ptr->hp < 0)
	{
		if ((r_ptr->flags1 & RF1_UNIQUE) ||
			(r_ptr->flags1 & RF1_QUESTOR) || (r_ptr->flags3 & RF3_UNIQUE_7))
		{
			m_ptr->hp = 1;
		}
		else
		{
			/* Make a sound */
			if (!monster_living(r_ptr))
			{
				sound(SOUND_N_KILL);
			}
			else
			{
				sound(SOUND_KILL);
			}

			if (known)
			{
				/* Unseen death by normal attack */
				if (!m_ptr->ml)
				{
					p_ptr->mon_fight = TRUE;
				}
				/* Death by special attack */
				else if (note)
				{
					msg_format("%^s%s", m_name, note);
				}
				/* Death by normal attack -- nonliving monster */
				else if (!monster_living(r_ptr))
				{
					msg_format("%^s is destroyed.", m_name);
				}
				/* Death by normal attack -- living monster */
				else
				{
					msg_format("%^s is killed.", m_name);
				}
			}

			/* Generate treasure */
			(void) monster_death(m_idx, TRUE);

			/* Delete the monster */
			delete_monster_idx(m_idx);

			/* Not afraid */
			(*fear) = FALSE;

			/* Monster is dead */
			return;
		}
	}

	/* Mega-Hack -- Pain cancels fear */
	if (m_ptr->monfear && (dam > 0))
	{
		int tmp = randint1(dam / 4);

		/* Cure a little fear */
		if (tmp < m_ptr->monfear)
		{
			/* Reduce fear */
			m_ptr->monfear -= tmp;
		}

		/* Cure all the fear */
		else
		{
			/* Cure fear */
			m_ptr->monfear = 0;

			/* No more fear */
			(*fear) = FALSE;
		}
	}

	/* Sometimes a monster gets scared by damage */
	if (!m_ptr->monfear && !(r_ptr->flags3 & RF3_NO_FEAR))
	{
		int percentage;

		/* Percentage of fully healthy */
		percentage = (100L * m_ptr->hp) / m_ptr->maxhp;

		/*
		   * Run (sometimes) if at 10% or less of max hit points,
		   * or (usually) when hit for half its current hit points
		 */
		if (((percentage <= 10) && (randint0(10) < percentage)) ||
			((dam >= m_ptr->hp) && (randint0(100) < 80)))
		{
			/* Hack -- note fear */
			(*fear) = TRUE;

			/* XXX XXX XXX Hack -- Add some timed fear */
			m_ptr->monfear += (randint1(10) +
							   (((dam >= m_ptr->hp) && (percentage > 7)) ?
								20 : ((11 - percentage) * 5)));
		}
	}

	/* Not dead yet */
	return;
}


/*
 * Returns whether a given monster will try to run from the player.
 *
 * Monsters will attempt to avoid very powerful players.  See below.
 *
 * Because this function is called so often, little details are important
 * for efficiency.  Like not using "mod" or "div" when possible.  And
 * attempting to check the conditions in an optimal order.  Note that
 * "(x << 2) == (x * 4)" if "x" has enough bits to hold the result.
 *
 * Note that this function is responsible for about one to five percent
 * of the processor use in normal conditions...
 */
static int mon_will_run(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	u16b p_lev, m_lev;
	u16b p_chp, p_mhp;
	u16b m_chp, m_mhp;
	u32b p_val, m_val;

	/* Friends can be commanded to avoid the player */
	if (is_pet(m_ptr))
	{
		/* Are we trying to avoid the player? */
		return ((p_ptr->pet_follow_distance < 0) &&
				(m_ptr->cdis <= (0 - p_ptr->pet_follow_distance)));
	}

	/* Keep monsters from running too far away */
	if (m_ptr->cdis > MAX_SIGHT + 5) return (FALSE);

	/* All "afraid" monsters will run away */
	if (m_ptr->monfear) return (TRUE);

	/* Nearby monsters will not become terrified */
	if (m_ptr->cdis <= 5) return (FALSE);

	/* Examine player power (level) */
	p_lev = p_ptr->lev;

	/* Examine monster power (level plus morale) */
	m_lev = r_ptr->level + 25;

	/* Optimize extreme cases below */
	if (m_lev > p_lev + 4) return (FALSE);
	if (m_lev + 4 <= p_lev) return (TRUE);

	/* Examine player health */
	p_chp = p_ptr->chp;
	p_mhp = p_ptr->mhp;

	/* Examine monster health */
	m_chp = m_ptr->hp;
	m_mhp = m_ptr->maxhp;

	/* Prepare to optimize the calculation */
	p_val = (p_lev * p_mhp) + (p_chp << 2);	/* div p_mhp */
	m_val = (m_lev * m_mhp) + (m_chp << 2);	/* div m_mhp */

	/* Strong players scare strong monsters */
	if (p_val * m_mhp > m_val * p_mhp) return (TRUE);

	/* Assume no terror */
	return (FALSE);
}

/*
 * Can the monster catch a whiff of the character?
 *
 * Many more monsters can smell, but they find it hard to smell and 
 * track down something at great range.
 *
 * XXX XXX Use SMELL_STRENGTH as a function of monster race...
 */
static bool monster_can_smell(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	cave_type *c_ptr = area(m_ptr->fx, m_ptr->fy);

	/* No information? */
	if (!c_ptr->when) return (FALSE);

#if 0

	/* Scent is too old */
	if (age > SMELL_STRENGTH) return (FALSE);

#endif /* 0 */

	/* Canines and Zephyer Hounds are amazing trackers */
	if (strchr("CZ", r_ptr->d_char)) return (TRUE);

	/* So are the Nazgul */
	else if ((strchr("W", r_ptr->d_char)) && (r_ptr->flags1 & (RF1_UNIQUE)))
	{
		/* Bloodscent! */
		return (TRUE);
	}
#if 0
	/* Other monsters can sometimes make good use of scent */
	else if (strchr("fkoqyHORTY", r_ptr->d_char))
	{
		if (age <= SMELL_STRENGTH - 10)
		{
			/* Something's in the air... */
			return (TRUE);
		}
	}
#endif /* 0 */

	/* You're imagining things. */
	return (FALSE);
}


/*
 * Hack -- Precompute a bunch of calls to distance() in find_safety() and
 * find_hiding().
 *
 * The pair of arrays dist_offsets_y[n] and dist_offsets_x[n] contain the
 * offsets of all the locations with a distance of n from a central point,
 * with an offset of (0,0) indicating no more offsets at this distance.
 *
 * This is, of course, fairly unreadable, but it eliminates multiple loops
 * from the previous version.
 *
 * It is probably better to replace these arrays with code to compute
 * the relevant arrays, even if the storage is pre-allocated in hard
 * coded sizes.  At the very least, code should be included which is
 * able to generate and dump these arrays (ala "los()").  XXX XXX XXX
 *
 * Also, the storage needs could be halved by using bytes.  XXX XXX XXX
 *
 * These arrays could be combined into two big arrays, using sub-arrays
 * to hold the offsets and lengths of each portion of the sub-arrays, and
 * this could perhaps also be used somehow in the "look" code.  XXX XXX XXX
 */


static sint d_off_y_0[] =
{ 0 };

static sint d_off_x_0[] =
{ 0 };


static sint d_off_y_1[] =
{ -1, -1, -1, 0, 0, 1, 1, 1, 0 };

static sint d_off_x_1[] =
{ -1, 0, 1, -1, 1, -1, 0, 1, 0 };


static sint d_off_y_2[] =
{ -1, -1, -2, -2, -2, 0, 0, 1, 1, 2, 2, 2, 0 };

static sint d_off_x_2[] =
{ -2, 2, -1, 0, 1, -2, 2, -2, 2, -1, 0, 1, 0 };


static sint d_off_y_3[] =
{ -1, -1, -2, -2, -3, -3, -3, 0, 0, 1, 1, 2, 2, 3, 3, 3, 0 };

static sint d_off_x_3[] =
{ -3, 3, -2, 2, -1, 0, 1, -3, 3, -3, 3, -2, 2, -1, 0, 1, 0 };


static sint d_off_y_4[] =
{
	-1, -1, -2, -2, -3, -3, -3, -3, -4, -4, -4, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 4,
		4, 4, 0
};

static sint d_off_x_4[] =
{
	-4, 4, -3, 3, -2, -3, 2, 3, -1, 0, 1, -4, 4, -4, 4, -3, 3, -2, -3, 2, 3, -1,
		0, 1, 0
};


static sint d_off_y_5[] =
{
	-1, -1, -2, -2, -3, -3, -4, -4, -4, -4, -5, -5, -5, 0, 0, 1, 1, 2, 2, 3, 3,
		4, 4, 4, 4, 5, 5, 5, 0
};

static sint d_off_x_5[] =
{
	-5, 5, -4, 4, -4, 4, -2, -3, 2, 3, -1, 0, 1, -5, 5, -5, 5, -4, 4, -4, 4, -2,
		-3, 2, 3, -1, 0, 1, 0
};


static sint d_off_y_6[] =
{
	-1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -5, -5, -6, -6, -6, 0, 0, 1, 1, 2,
		2, 3, 3, 4, 4, 5, 5, 5, 5, 6, 6, 6, 0
};

static sint d_off_x_6[] =
{
	-6, 6, -5, 5, -5, 5, -4, 4, -2, -3, 2, 3, -1, 0, 1, -6, 6, -6, 6, -5, 5, -5,
		5, -4, 4, -2, -3, 2, 3, -1, 0, 1, 0
};


static sint d_off_y_7[] =
{
	-1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -5, -5, -6, -6, -6, -6, -7, -7, -7,
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 0
};

static sint d_off_x_7[] =
{
	-7, 7, -6, 6, -6, 6, -5, 5, -4, -5, 4, 5, -2, -3, 2, 3, -1, 0, 1, -7, 7, -7,
		7, -6, 6, -6, 6, -5, 5, -4, -5, 4, 5, -2, -3, 2, 3, -1, 0, 1, 0
};


static sint d_off_y_8[] =
{
	-1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -6, -6, -7, -7, -7, -7, -8,
		-8, -8, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8,
		8, 8, 0
};

static sint d_off_x_8[] =
{
	-8, 8, -7, 7, -7, 7, -6, 6, -6, 6, -4, -5, 4, 5, -2, -3, 2, 3, -1, 0, 1, -8,
		8, -8, 8, -7, 7, -7, 7, -6, 6, -6, 6, -4, -5, 4, 5, -2, -3, 2, 3, -1, 0,
		1, 0
};


static sint d_off_y_9[] =
{
	-1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -7, -7, -8, -8, -8,
		-8, -9, -9, -9, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 7, 8,
		8, 8, 8, 9, 9, 9, 0
};

static sint d_off_x_9[] =
{
	-9, 9, -8, 8, -8, 8, -7, 7, -7, 7, -6, 6, -4, -5, 4, 5, -2, -3, 2, 3, -1, 0,
		1, -9, 9, -9, 9, -8, 8, -8, 8, -7, 7, -7, 7, -6, 6, -4, -5, 4, 5, -2,
		-3, 2, 3, -1, 0, 1, 0
};


static sint *dist_offsets_y[10] =
{
	d_off_y_0, d_off_y_1, d_off_y_2, d_off_y_3, d_off_y_4, d_off_y_5, d_off_y_6,
		d_off_y_7, d_off_y_8, d_off_y_9
};

static sint *dist_offsets_x[10] =
{
	d_off_x_0, d_off_x_1, d_off_x_2, d_off_x_3, d_off_x_4, d_off_x_5, d_off_x_6,
		d_off_x_7, d_off_x_8, d_off_x_9
};

/*
* Choose a "safe" location near a monster for it to run toward.
*
* A location is "safe" if it can be reached quickly and the player
* is not able to fire into it (it isn't a "clean shot").  So, this will
* cause monsters to "duck" behind walls.  Hopefully, monsters will also
* try to run towards corridor openings if they are in a room.
*
* This function may take lots of CPU time if lots of monsters are
* fleeing.
*/
static void find_safety(monster_type *m_ptr, int *xp, int *yp)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int fy = m_ptr->fy;
	int fx = m_ptr->fx;

	int y, x, d, dis, i;
	int gdis = 0;

	sint *y_offsets;
	sint *x_offsets;

	cave_type *c_ptr;

	/* Start with adjacent locations, spread further */
	for (d = 1; d < 10; d++)
	{
		/* Get the lists of points with a distance d from (fx, fy) */
		y_offsets = dist_offsets_y[d];
		x_offsets = dist_offsets_x[d];

		/* Check the locations */
		for (i = 0; x_offsets[i] != 0 || y_offsets[i] != 0; i++)
		{
			y = fy + y_offsets[i];
			x = fx + x_offsets[i];

			/* Skip illegal locations */
			if (!in_boundsp(x, y)) continue;

			c_ptr = area(x, y);

			/* Skip locations in a wall */
			if (!cave_floor_grid(c_ptr)) continue;

			/* Ignore grids very far from the player */
			if (c_ptr->when < area(px, py)->when) continue;

			/* Ignore too-distant grids */
			if (c_ptr->cost > area(fx, fy)->cost + 2 * d) continue;

			/* Check for absence of shot (more or less) */
			if (clean_shot(fx, fy, x, y, FALSE))
			{
				/* Calculate distance from player */
				dis = distance(x, y, px, py);

				/* Remember if further than previous */
				if (dis > gdis)
				{
					*yp = y;
					*xp = x;
					if (!player_has_los_grid(parea(x, y)))
					{
						gdis = dis * 5;
					}
					else
					{
						gdis = dis;
					}
				}
			}
		}

		/* Bail out when have a safe square */
		if (gdis > d * 2) return;
	}
}


/*
 * Choose a good hiding place near a monster for it to run toward.
 *
 * Pack monsters will use this to "ambush" the player and lure him out
 * of corridors into open space so they can swarm him.
 *
 * Return TRUE if a good location is available.
 */
static bool find_hiding(monster_type *m_ptr, int *xp, int *yp)
{
	int fy = m_ptr->fy;
	int fx = m_ptr->fx;

	int py = p_ptr->py;
	int px = p_ptr->px;

	int y, x, d, dis, i;
	int gdis = 999;

	sint *y_offsets, *x_offsets;

	cave_type *c_ptr;

	/* Start with adjacent locations, spread further */
	for (d = 1; d < 10; d++)
	{
		/* Get the lists of points with a distance d from (fx, fy) */
		y_offsets = dist_offsets_y[d];
		x_offsets = dist_offsets_x[d];

		/* Check the locations */
		for (i = 0; x_offsets[i] != 0 || y_offsets[i] != 0; i++)
		{
			y = fy + y_offsets[i];
			x = fx + x_offsets[i];

			/* Skip illegal locations */
			if (!in_boundsp(x, y)) continue;

			c_ptr = area(x, y);

			/* Skip occupied locations */
			if (!cave_empty_grid(c_ptr)) continue;

			/* Not on player */
			if ((y == py) && (x == px)) continue;

			/* Check for hidden, available grid */
			if (!player_has_los_grid(parea(x, y))
				&& clean_shot(fx, fy, x, y, FALSE))
			{
				/* Calculate distance from player */
				dis = distance(x, y, px, py);

				/* Remember if closer than previous */
				if (dis < gdis && dis >= 2)
				{
					*yp = y;
					*xp = x;
					gdis = dis;
				}
			}
		}

		/* Check for success */
		if (gdis < 999)
		{
			/* Found good place */
			return (TRUE);
		}
	}

	/* No good place */
	return (FALSE);
}



/*
 * Helper function for monsters that want to advance toward the character.
 * Assumes that the monster isn't frightened, and is not in LOS of the 
 * character.
 *
 * Ghosts and rock-eaters do not use flow information, because they 
 * can - in general - move directly towards the character.  We could make 
 * them look for a grid at their preferred range, but the character 
 * would then be able to avoid them better (it might also be a little 
 * hard on those poor warriors...).
 *
 * Other monsters will use target information, then their ears, then their
 * noses (if they can), and advance blindly if nothing else works.
 * 
 * When flowing, monsters prefer non-diagonal directions.
 *
 * XXX - At present, this function does not handle difficult terrain 
 * intelligently.  Monsters using flow may bang right into a door that 
 * they can't handle.  Fixing this may require code to set monster 
 * paths.
 */
static void get_move_advance(monster_type *m_ptr, int *tx, int *ty)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	int i, x, y;
	int mx = m_ptr->fx;
	int my = m_ptr->fy;

	int best_val = 0;

	cave_type *c_ptr;

	bool use_sound = FALSE;
	bool use_scent = FALSE;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];


	/* Hack - Monster can go through rocks - head straight for character */
	if (r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL))
	{
		*tx = px;
		*ty = py;
		return;
	}

	/* Can the player see us? - if so run towards him */
	if (in_boundsp(mx, my))
	{
		if (player_has_los_grid(parea(mx, my)))
		{
			*tx = px;
			*ty = py;

			return;
		}
	}

#if 0
	/* Use target information if available */
	if ((m_ptr->ty) || (m_ptr->tx))
	{
		*tx = m_ptr->tx;
		*ty = m_ptr->ty;
		return;
	}
#endif /* 0 */

	/* Monster location */
	c_ptr = area(mx, my);

	/* If we can hear noises, advance towards them */
	if (c_ptr->cost)
	{
		use_sound = TRUE;
	}

	/* Otherwise, try to follow a scent trail */
	else if (monster_can_smell(m_ptr))
	{
		use_scent = TRUE;
	}

	/* Otherwise, advance blindly */
	if ((!use_sound) && (!use_scent))
	{
		*ty = py;
		*tx = px;
		return;
	}

	/* Paranoia - nothing to do? */
	*tx = mx;
	*ty = my;

	/* Using flow information.  Check nearby grids, diagonals first. */
	for (i = 7; i >= 0; i--)
	{
		/* Get the location */
		x = mx + ddx_ddd[i];
		y = my + ddy_ddd[i];

		/* Check Bounds */
		if (!in_bounds(x, y)) continue;

		c_ptr = area(x, y);

		/* We're following a scent trail */
		if (use_scent)
		{
			byte when = c_ptr->when;

			/* Accept younger scent */
			if (best_val < when) continue;
			best_val = when;
		}

		/* We're using sound */
		else
		{
			byte cost = c_ptr->cost;

			/* Accept louder sounds */
			if ((!best_val) || (cost > best_val)) continue;
			best_val = cost;
		}

		/* Save the location */
		*ty = y;
		*tx = x;
	}
}


#define FLEE_RANGE 15

/*
 * Helper function for monsters that want to retreat from the character.
 * Used for any monster that is terrified, frightened, is looking for a 
 * temporary hiding spot, or just wants to open up some space between it 
 * and the character.
 *
 * If the monster is well away from danger, let it relax.
 * If the monster's current target is not in LOS, use it (+).
 * If the monster is not in LOS, and cannot pass through walls, try to 
 * use flow (noise) information.
 * If the monster is in LOS, even if it can pass through walls, 
 * search for a hiding place (helper function "find_safety()"):
 * search for a hiding place (helper function "find_safety()").
 * If no hiding place is found, and there seems no way out, go down
 * fighting.
 *
 * If none of the above solves the problem, run away blindly.
 *
 * (+) There is one exception to the automatic usage of a target.  If the 
 * target is only out of LOS because of "knight's move" rules (distance 
 * along one axis is 2, and along the other, 1), then the monster will try 
 * to find another adjacent grid that is out of sight.  What all this boils 
 * down to is that monsters can now run around corners properly!
 *
 * Return TRUE if the monster did actually want to do anything.
 */
static bool get_move_retreat(monster_type *m_ptr, int *tx, int *ty)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	cave_type *c_ptr;

	int i;
	int x, y;

	/* If the monster is well away from danger, let it relax. */
	if (m_ptr->cdis >= FLEE_RANGE)
	{
		return (FALSE);
	}

	/* Monster has a target */
	if ((m_ptr->tx) && (m_ptr->ty))
	{
		/* It's out of LOS; keep using it, except in "knight's move" cases */
		if (!player_has_los_grid(parea(m_ptr->tx, m_ptr->ty)))
		{
			/* Get axis distance from character to current target */
			int dist_x = ABS(p_ptr->px - m_ptr->tx);
			int dist_y = ABS(p_ptr->py - m_ptr->ty);

			/* It's only out of LOS because of "knight's move" rules */
			if (((dist_y == 2) && (dist_x == 1)) ||
				((dist_y == 1) && (dist_x == 2)))
			{
				/*
				 * If there is another grid adjacent to the monster that 
				 * the character cannot see into, and it isn't any harder 
				 * to enter, use it instead.  Prefer diagonals.
				 */
				for (i = 7; i >= 0; i--)
				{
					x = m_ptr->fx + ddx_ddd[i];
					y = m_ptr->fy + ddy_ddd[i];

					/* Check Bounds */
					if (!in_bounds(x, y)) continue;

					if (player_has_los_grid(parea(x, y))) continue;

					if ((x == m_ptr->tx) && (y == m_ptr->ty)) continue;

					if (cave_passable_mon(m_ptr, area(m_ptr->tx, m_ptr->ty)) >
						cave_passable_mon(m_ptr, area(x, y))) continue;

					m_ptr->ty = x;
					m_ptr->tx = y;
					break;
				}
			}

			/* Move towards the target */
			*tx = m_ptr->tx;
			*ty = m_ptr->ty;
			return (TRUE);
		}

		/* It's in LOS; cancel it. */
		else
		{
			m_ptr->tx = 0;
			m_ptr->ty = 0;
		}
	}

	c_ptr = area(m_ptr->fx, m_ptr->fy);

	/* The monster is not in LOS, but thinks it's still too close. */
	if (in_boundsp(m_ptr->fx, m_ptr->fy)
		&& !player_has_los_grid(parea(m_ptr->fx, m_ptr->fy)))
	{
		/* Monster cannot pass through walls */
		if (!(r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL)))
		{
			/* Run away from noise */
			if (c_ptr->cost)
			{
				int start_cost = c_ptr->cost;

				/* Look at adjacent grids, diagonals first */
				for (i = 7; i >= 0; i--)
				{
					x = m_ptr->fx + ddx_ddd[i];
					y = m_ptr->fy + ddy_ddd[i];

					/* Check Bounds */
					if (!in_bounds(y, x)) continue;

					c_ptr = area(x, y);

					/* Accept the first non-visible grid with a lower cost */
					if (c_ptr->cost < start_cost)
					{
						if (!player_has_los_grid(parea(x, y)))
						{
							*tx = x;
							*ty = y;

							/* Success */
							return (TRUE);
						}
					}
				}
			}
		}

		/* No flow info, or don't need it -- see bottom of function */
	}

	/* The monster is in line of sight. */
	else
	{
		int prev_cost = c_ptr->cost;
		int start = rand_int(8);

		/* Look for adjacent hiding places */
		for (i = start; i < 8 + start; i++)
		{
			x = m_ptr->fx + ddx_ddd[i % 8];
			y = m_ptr->fy + ddy_ddd[i % 8];

			/* Check Bounds */
			if (!in_bounds(x, y)) continue;

			c_ptr = area(x, y);

			/* No grids in LOS */
			if (in_boundsp(x, y) && player_has_los_grid(parea(x, y))) continue;

			/* Grid must be pretty easy to enter */
			if (cave_passable_mon(m_ptr, c_ptr) < 50) continue;

			/* Accept any grid that doesn't have a higher flow (noise) cost. */
			if (c_ptr->cost <= prev_cost)
			{
				*tx = x;
				*ty = y;

				/* Success */
				return (TRUE);
			}
		}

		/* Find a nearby grid not in LOS of the character. */

		find_safety(m_ptr, tx, ty);
#if 0
		/*
		 * No safe place found.  If monster is in LOS and close,
		 * it will turn to fight.
		 */
		if ((player_has_los_bold(m_ptr->fx, m_ptr->fy)) &&
			(m_ptr->cdis < TURN_RANGE))
		{
			/* Turn and fight */
			m_ptr->monfear = 0;

			/* Recalculate combat range (later) */
			m_ptr->min_range = 0;

			/* Visible */
			if (m_ptr->ml)
			{
				char m_name[80];

				/* Get the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
				msg_format("%^s turns to fight!", m_name);
			}

			/* Charge! */
			*tx = p_ptr->px;
			*ty = p_ptr->py;

		}
#endif /* 0 */
		return (TRUE);
	}

	/* Move directly away from character. */
	*tx = -(p_ptr->px - m_ptr->fx);
	*ty = -(p_ptr->py - m_ptr->fy);

	/* We want to run away */
	return (TRUE);
}


/*
 * Choose "logical" directions for monster movement
 */
static bool get_moves(int m_idx, int *mm)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	int xx, yy;
	int tx, ty;

	bool will_run = mon_will_run(m_ptr);
	bool done = FALSE;

	cave_type *c_ptr;

	if (will_run)
	{
		/* Get flow target */
		get_move_retreat(m_ptr, &tx, &ty);
	}
	else
	{
		/* Get flow target */
		get_move_advance(m_ptr, &tx, &ty);
	}

	if (!stupid_monsters && !will_run && is_hostile(m_ptr))
	{
		/*
		 * Animal packs try to get the player out of corridors
		 * (...unless they can move through walls -- TY)
		 */
		if ((r_ptr->flags1 & RF1_FRIENDS) &&
			(r_ptr->flags3 & RF3_ANIMAL) &&
			!(r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL)) && smart_packs)
		{
			int i, room = 0;

			/* Count room grids next to player */
			for (i = 0; i < 8; i++)
			{
				xx = px + ddx_ddd[i];
				yy = py + ddy_ddd[i];

				if (!in_bounds2(xx, yy)) continue;

				c_ptr = area(xx, yy);

				/* Check grid */
				if (((cave_floor_grid(c_ptr)) || ((c_ptr->feat & 0x60) == 0x60))
					&& monster_can_cross_terrain(c_ptr->feat, r_ptr))
				{
					/* One more room grid */
					room++;
				}
			}

			/* Not in a room and strong player */
			if (room <= (8 * (p_ptr->chp + p_ptr->csp)) /
				(p_ptr->mhp + p_ptr->msp))
			{
				/* Find hiding place */
				if (find_hiding(m_ptr, &tx, &ty)) done = TRUE;
			}
		}

		/* Monster groups try to surround the player */
		if (!done && (r_ptr->flags1 & RF1_FRIENDS))
		{
			int i;

			/* Find an empty square near the player to fill */
			for (i = 0; i < 8; i++)
			{
				/* Pick squares near player (semi-randomly) */
				xx = px + ddx_ddd[(m_idx + i) & 7];
				yy = py + ddy_ddd[(m_idx + i) & 7];


				/* Already there? */
				if ((m_ptr->fy == yy) && (m_ptr->fx == xx))
				{
					/* Attack the player */
					ty = py;
					tx = px;

					break;
				}

				if (!in_bounds2(xx, yy)) continue;

				/* Not on player */
				if ((yy == py) && (xx == px)) continue;

				/* Ignore filled grids */
				if (!cave_empty_grid(area(xx, yy))) continue;

				/* Try to fill this hole */
				tx = xx;
				ty = yy;

				break;
			}

			/* Done */
			done = TRUE;
		}
	}

	if (!stupid_monsters)
	{
		/* Check for no move */
		if (!tx && !ty) return (FALSE);
	}

	/* Set our target? */
	m_ptr->tx = tx;
	m_ptr->ty = ty;

	/* Move towards our target */
	convert_target_dir(m_ptr, mm);

	/* Wants to move... */
	return (TRUE);
}


static int check_hit2(int power, int level, int ac)
{
	int i, k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Always miss or hit */
	if (k < 10) return (k < 5);

	/* Calculate the "attack quality" */
	i = (power + (level * 3));

	/* Power and Level compete against Armor */
	if ((i > 0) && (randint1(i) > ((ac * 3) / 4))) return (TRUE);

	/* Assume miss */
	return (FALSE);
}


/* Monster attacks monster */
static bool monst_attack_monst(int m_idx, int t_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_type *t_ptr = &m_list[t_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_race *tr_ptr = &r_info[t_ptr->r_idx];

	int ap_cnt;
	int ac, rlev, pt;
	char m_name[80], t_name[80];
	char ddesc[80], temp[80];
	bool blinked, heal_effect;
	bool explode = FALSE, touched = FALSE, fear = FALSE;
	int y_saver = t_ptr->fy;
	int x_saver = t_ptr->fx;

	bool see_m = m_ptr->ml;
	bool see_t = t_ptr->ml;
	bool see_either = see_m || see_t;

	/* Can the player be aware of this attack? */
	bool known = (m_ptr->cdis <= MAX_SIGHT) || (t_ptr->cdis <= MAX_SIGHT);

	/* Cannot attack self */
	if (m_idx == t_idx) return FALSE;

	/* Not allowed to attack */
	if (r_ptr->flags1 & RF1_NEVER_BLOW) return FALSE;

	/* Wake it up */
	t_ptr->csleep = 0;

	/* Total armor */
	ac = tr_ptr->ac;

	/* Extract the effective monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/* Get the monster name (or "it") */
	monster_desc(t_name, t_ptr, 0);

	/* Get the "died from" information (i.e. "a kobold") */
	monster_desc(ddesc, m_ptr, 0x88);

	/* Assume no blink */
	blinked = FALSE;

	if (!see_either && known)
	{
		p_ptr->mon_fight = TRUE;
	}

	/* Scan through all four blows */
	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		bool obvious = FALSE;

		int power = 0;
		int damage = 0;

		cptr act = NULL;

		/* Extract the attack infomation */
		int effect = r_ptr->blow[ap_cnt].effect;
		int method = r_ptr->blow[ap_cnt].method;
		int d_dice = r_ptr->blow[ap_cnt].d_dice;
		int d_side = r_ptr->blow[ap_cnt].d_side;

		/* Stop attacking if the target teleports away */
		if (t_ptr->fx != x_saver || t_ptr->fy != y_saver)
			break;

		/* Stop attacking if the aggressor dies (fire sheath etc.) */
		if ((!m_ptr->r_idx) || (!t_ptr->r_idx)) return TRUE;

		/* Hack -- no more attacks */
		if (!method) break;

		if (blinked)			/* Stop! */
		{
			/* break; */
		}

		/* Extract the attack "power" */
		switch (effect)
		{
			case RBE_HURT:
			{
				power = 60;
				break;
			}
			case RBE_POISON:
			{
				power = 5;
				break;
			}
			case RBE_UN_BONUS:
			{
				power = 20;
				break;
			}
			case RBE_UN_POWER:
			{
				power = 15;
				break;
			}
			case RBE_EAT_GOLD:
			{
				power = 5;
				break;
			}
			case RBE_EAT_ITEM:
			{
				power = 5;
				break;
			}
			case RBE_EAT_FOOD:
			{
				power = 5;
				break;
			}
			case RBE_EAT_LITE:
			{
				power = 5;
				break;
			}
			case RBE_ACID:
			{
				power = 0;
				break;
			}
			case RBE_ELEC:
			{
				power = 10;
				break;
			}
			case RBE_FIRE:
			{
				power = 10;
				break;
			}
			case RBE_COLD:
			{
				power = 10;
				break;
			}
			case RBE_BLIND:
			{
				power = 2;
				break;
			}
			case RBE_CONFUSE:
			{
				power = 10;
				break;
			}
			case RBE_TERRIFY:
			{
				power = 10;
				break;
			}
			case RBE_PARALYZE:
			{
				power = 2;
				break;
			}
			case RBE_LOSE_STR:
			{
				power = 0;
				break;
			}
			case RBE_LOSE_DEX:
			{
				power = 0;
				break;
			}
			case RBE_LOSE_CON:
			{
				power = 0;
				break;
			}
			case RBE_LOSE_INT:
			{
				power = 0;
				break;
			}
			case RBE_LOSE_WIS:
			{
				power = 0;
				break;
			}
			case RBE_LOSE_CHR:
			{
				power = 0;
				break;
			}
			case RBE_LOSE_ALL:
			{
				power = 2;
				break;
			}
			case RBE_SHATTER:
			{
				power = 60;
				break;
			}
			case RBE_EXP_10:
			{
				power = 5;
				break;
			}
			case RBE_EXP_20:
			{
				power = 5;
				break;
			}
			case RBE_EXP_40:
			{
				power = 5;
				break;
			}
			case RBE_EXP_80:
			{
				power = 5;
				break;
			}
			case RBE_DISEASE:
			{
				power = 5;
				break;
			}
			case RBE_TIME:
			{
				power = 5;
				break;
			}
			case RBE_EXP_VAMP:
			{
				power = 5;
				break;
			}
		}


		/* Monster hits */
		if (!effect || check_hit2(power, rlev, ac))
		{
			/* Describe the attack method */
			switch (method)
			{
				case RBM_HIT:
				{
					act = "hits %s.";
					touched = TRUE;
					break;
				}

				case RBM_TOUCH:
				{
					act = "touches %s.";
					touched = TRUE;
					break;
				}

				case RBM_PUNCH:
				{
					act = "punches %s.";
					touched = TRUE;
					break;
				}

				case RBM_KICK:
				{
					act = "kicks %s.";
					touched = TRUE;
					break;
				}

				case RBM_CLAW:
				{
					act = "claws %s.";
					touched = TRUE;
					break;
				}

				case RBM_BITE:
				{
					act = "bites %s.";
					touched = TRUE;
					break;
				}

				case RBM_STING:
				{
					act = "stings %s.";
					touched = TRUE;
					break;
				}

				case RBM_XXX1:
				{
					act = "XXX1's %s.";
					break;
				}

				case RBM_BUTT:
				{
					act = "butts %s.";
					touched = TRUE;
					break;
				}

				case RBM_CRUSH:
				{
					act = "crushes %s.";
					touched = TRUE;
					break;
				}

				case RBM_ENGULF:
				{
					act = "engulfs %s.";
					touched = TRUE;
					break;
				}

				case RBM_CHARGE:
				{
					act = "charges %s.";
					touched = TRUE;
					break;
				}

				case RBM_CRAWL:
				{
					act = "crawls on %s.";
					touched = TRUE;
					break;
				}

				case RBM_DROOL:
				{
					act = "drools on %s.";
					touched = FALSE;
					break;
				}

				case RBM_SPIT:
				{
					act = "spits on %s.";
					touched = FALSE;
					break;
				}

				case RBM_EXPLODE:
				{
					if (see_either) disturb(TRUE);
					act = "explodes.";
					explode = TRUE;
					touched = FALSE;
					break;
				}

				case RBM_GAZE:
				{
					act = "gazes at %s.";
					touched = FALSE;
					break;
				}

				case RBM_WAIL:
				{
					act = "wails at %s.";
					touched = FALSE;
					break;
				}

				case RBM_SPORE:
				{
					act = "releases spores at %s.";
					touched = FALSE;
					break;
				}

				case RBM_XXX4:
				{
					act = "projects XXX4's at %s.";
					touched = FALSE;
					break;
				}

				case RBM_BEG:
				{
					act = "begs %s for money.";
					touched = FALSE;
					break;
				}

				case RBM_INSULT:
				{
					act = "insults %s.";
					touched = FALSE;
					break;
				}

				case RBM_MOAN:
				{
					act = "moans at %s.";
					touched = FALSE;
					break;
				}

				case RBM_SHOW:
				{
					act = "sings to %s.";
					touched = FALSE;
					break;
				}
			}

			/* Message */
			if (act && see_either)
			{
				/* Look to see if we've spotted a mimic */
				if ((m_ptr->smart & SM_MIMIC) && m_ptr->ml)
				{
					char m_name2[80];

					/* Get name */
					monster_desc(m_name2, m_ptr, 0x88);

					/* Toggle flag */
					m_ptr->smart &= ~(SM_MIMIC);

					/* It is in the monster list now */
					update_mon_vis(m_ptr->r_idx, 1);

					/* We've spotted it */
					msg_format("You see %s!", m_name2);
				}

				/* Look to see if we've spotted a mimic */
				if ((t_ptr->smart & SM_MIMIC) && t_ptr->ml)
				{
					char t_name2[80];

					/* Get name */
					monster_desc(t_name2, t_ptr, 0x88);

					/* Toggle flag */
					t_ptr->smart &= ~(SM_MIMIC);

					/* It is in the monster list now */
					update_mon_vis(t_ptr->r_idx, 1);

					/* We've spotted it */
					msg_format("You see %s!", t_name2);
				}

				if ((p_ptr->image) && one_in_(3))
				{
					(void) strfmt(temp, "%s %s.",
								  silly_attacks[randint0(MAX_SILLY_ATTACK)],
								  t_name);
				}
				else
					(void) strfmt(temp, act, t_name);

				msg_format("%^s %s", m_name, temp);
			}

			/* Hack -- assume all attacks are obvious */
			obvious = TRUE;

			/* Roll out the damage */
			damage = damroll(d_dice, d_side);

			/* Assume no healing effect */
			heal_effect = FALSE;

			pt = GF_MISSILE;

			/* Apply appropriate damage */
			switch (effect)
			{
				case 0:
				{
					damage = 0;
					pt = 0;
					break;
				}

				case RBE_HURT:
				{
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);
					break;
				}

				case RBE_POISON:
				case RBE_DISEASE:
				{
					pt = GF_POIS;
					break;
				}

				case RBE_UN_BONUS:
				case RBE_UN_POWER:
				{
					pt = GF_DISENCHANT;
					break;
				}

				case RBE_EAT_FOOD:
				case RBE_EAT_LITE:
				{
					pt = damage = 0;
					break;
				}

				case RBE_EAT_ITEM:
				case RBE_EAT_GOLD:
				{
					pt = damage = 0;
					if (one_in_(2)) blinked = TRUE;
					break;
				}

				case RBE_ACID:
				{
					pt = GF_ACID;
					break;
				}

				case RBE_ELEC:
				{
					pt = GF_ELEC;
					break;
				}

				case RBE_FIRE:
				{
					pt = GF_FIRE;
					break;
				}

				case RBE_COLD:
				{
					pt = GF_COLD;
					break;
				}

				case RBE_BLIND:
				{
					break;
				}

				case RBE_CONFUSE:
				{
					pt = GF_CONFUSION;
					break;
				}

				case RBE_TERRIFY:
				{
					pt = GF_TURN_ALL;
					break;
				}

				case RBE_PARALYZE:
				{
					pt = GF_OLD_SLEEP;	/* sort of close... */
					break;
				}

				case RBE_LOSE_STR:
				case RBE_LOSE_INT:
				case RBE_LOSE_WIS:
				case RBE_LOSE_DEX:
				case RBE_LOSE_CON:
				case RBE_LOSE_CHR:
				case RBE_LOSE_ALL:
				{
					break;
				}
				case RBE_SHATTER:
				{
					if (damage > 23)
					{
						(void) earthquake(m_ptr->fx, m_ptr->fy, 8);
					}
					break;
				}
				case RBE_EXP_10:
				case RBE_EXP_20:
				case RBE_EXP_40:
				case RBE_EXP_80:
				{
					pt = GF_NETHER;
					break;
				}
				case RBE_TIME:
				{
					pt = GF_TIME;
					break;
				}
				case RBE_EXP_VAMP:
				{
					pt = GF_OLD_DRAIN;
					heal_effect = TRUE;
					break;
				}

				default:
				{
					pt = 0;
					break;
				}
			}

			if (pt)
			{
				/* Do damage if not exploding */
				if (!explode)
				{
					(void) project(m_idx, 0, t_ptr->fx, t_ptr->fy,
								   (pt == GF_OLD_SLEEP ? r_ptr->level : damage),
								   pt, PROJECT_KILL | PROJECT_STOP);
				}

				if (heal_effect)
				{
					if ((monster_living(tr_ptr)) && (damage > 2))
					{
						bool did_heal = FALSE;

						if (m_ptr->hp < m_ptr->maxhp) did_heal = TRUE;

						/* Heal */
						m_ptr->hp += damroll(4, damage / 6);
						if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

						/* Redraw (later) if needed */
						if (p_ptr->health_who == m_idx) p_ptr->redraw |=
								(PR_HEALTH);

						/* Special message */
						if (see_m && did_heal)
						{
							msg_format("%^s appears healthier.", m_name);
						}
					}
				}

				if (touched)
				{
					/* Aura fire */
					if ((tr_ptr->flags2 & RF2_AURA_FIRE) &&
						!(r_ptr->flags3 & RF3_IM_FIRE))
					{
						if (see_either)
						{
							blinked = FALSE;
							msg_format("%^s is suddenly very hot!", m_name);
							if (see_t)
								tr_ptr->r_flags2 |= RF2_AURA_FIRE;
						}
						(void) project(t_idx, 0, m_ptr->fx, m_ptr->fy,
									   damroll(1 + ((tr_ptr->level) / 26),
											   1 + ((tr_ptr->level) / 17)),
									   GF_FIRE, PROJECT_KILL | PROJECT_STOP);
					}

					/* Aura cold */
					if ((tr_ptr->flags3 & RF3_AURA_COLD) &&
						!(r_ptr->flags3 & RF3_IM_COLD))
					{
						if (see_either)
						{
							blinked = FALSE;
							msg_format("%^s is suddenly very cold!", m_name);
							if (see_t)
								tr_ptr->r_flags3 |= RF3_AURA_COLD;
						}
						(void) project(t_idx, 0, m_ptr->fx, m_ptr->fy,
									   damroll(1 + ((tr_ptr->level) / 26),
											   1 + ((tr_ptr->level) / 17)),
									   GF_COLD, PROJECT_KILL | PROJECT_STOP);
					}

					/* Aura elec */
					if ((tr_ptr->flags2 & (RF2_AURA_ELEC))
						&& !(r_ptr->flags3 & (RF3_IM_ELEC)))
					{
						if (see_either)
						{
							blinked = FALSE;
							msg_format("%^s gets zapped!", m_name);
							if (see_t)
								tr_ptr->r_flags2 |= RF2_AURA_ELEC;
						}
						(void) project(t_idx, 0, m_ptr->fx, m_ptr->fy,
									   damroll(1 + ((tr_ptr->level) / 26),
											   1 + ((tr_ptr->level) / 17)),
									   GF_ELEC, PROJECT_KILL | PROJECT_STOP);
					}

				}
			}
		}

		/* Monster missed the monster */
		else
		{
			/* Analyze failed attacks */
			switch (method)
			{
				case RBM_HIT:
				case RBM_TOUCH:
				case RBM_PUNCH:
				case RBM_KICK:
				case RBM_CLAW:
				case RBM_BITE:
				case RBM_STING:
				case RBM_XXX1:
				case RBM_BUTT:
				case RBM_CRUSH:
				case RBM_ENGULF:
				case RBM_CHARGE:
				{
					/* Visible monsters */
					if (see_m)
					{
						/* Message */
						msg_format("%^s misses %s.", m_name, t_name);
					}

					break;
				}
			}
		}


		/* Analyze "visible" monsters only */
		if (see_m)
		{
			/* Count "obvious" attacks (and ones that cause damage) */
			if (obvious || damage || (r_ptr->r_blows[ap_cnt] > 10))
			{
				/* Count attacks of this type */
				if (r_ptr->r_blows[ap_cnt] < MAX_UCHAR)
				{
					r_ptr->r_blows[ap_cnt]++;
				}
			}
		}
	}

	if (explode)
	{
		sound(SOUND_EXPLODE);

		/* Cancel Invulnerability */
		if (m_ptr->invulner) m_ptr->invulner = 0;

		mon_take_hit_mon(m_idx, m_ptr->hp + 1, &fear,
						 " explodes into tiny shreds.");

		blinked = FALSE;
	}


	/* Blink away */
	if (blinked)
	{
		if (see_m)
		{
			msg_print("The thief flees laughing!");
		}
		else if (known)
		{
			p_ptr->mon_fight = TRUE;
		}

		(void) teleport_away(m_idx, MAX_SIGHT * 2 + 5);
	}

	return TRUE;
}

/*
 * Actually move the monster
 */
static void take_move(int m_idx, int *mm)
{
	int i;
	int d;
	int ny, nx;
	int oy, ox;

	cave_type *c_ptr;
	pcave_type *pc_ptr;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	monster_type *y_ptr;

	char m_name[80];

	field_mon_test mon_enter_test;

	/* Assume nothing */
	bool do_turn = FALSE;
	bool do_move = TRUE;

	bool did_open_door = FALSE;
	bool did_bash_door = FALSE;
	bool did_take_item = FALSE;
	bool did_kill_item = FALSE;
	bool did_move_body = FALSE;
	bool did_pass_wall = FALSE;
	bool did_kill_wall = FALSE;

	/* Get the origin */
	oy = m_ptr->fy;
	ox = m_ptr->fx;


	/* Take a zero-terminated array of "directions" */
	for (i = 0; mm[i]; i++)
	{
		/* Get the direction */
		d = mm[i];

		/* Hack -- allow "randomized" motion */
		if (d == 5) d = ddd[randint0(8)];

		/* Get the destination */
		ny = oy + ddy[d];
		nx = ox + ddx[d];

		/* Ignore locations off of edge */
		if (!in_boundsp(nx, ny)) continue;

		/* Access that cave grid */
		c_ptr = area(nx, ny);
		pc_ptr = parea(nx, ny);

		/* Access that cave grid's contents */
		y_ptr = &m_list[c_ptr->m_idx];

		/* Floor is open? */
		if (cave_floor_grid(c_ptr))
		{
			/* Go ahead and move */
			do_move = TRUE;
		}

		/* Hack -- player 'in' wall */
		else if ((ny == p_ptr->py) && (nx == p_ptr->px))
		{
			do_move = TRUE;
		}

		else if (c_ptr->m_idx)
		{
			/* Possibly a monster to attack */
			do_move = TRUE;
		}

		/* Permanent wall */
		else if ((c_ptr->feat >= FEAT_PERM_EXTRA) &&
				 (c_ptr->feat <= FEAT_PERM_SOLID))
		{
			do_move = FALSE;
		}

		/* Hack -- semi-transparent terrains are no obstacle */
		else if ((c_ptr->feat & 0x60) == 0x60)
		{
			do_move = TRUE;
		}

		/* Hack -- closed or secret doors are no obstacle */
		else if ((c_ptr->feat == FEAT_CLOSED) || (c_ptr->feat == FEAT_SECRET))
		{
			do_move = TRUE;
		}

		/* Monster moves through walls (and doors) */
		else if (r_ptr->flags2 & RF2_PASS_WALL)
		{
			/* Pass through walls/doors/rubble */
			do_move = TRUE;

			/* Monster went through a wall */
			did_pass_wall = TRUE;
		}

		/* Monster destroys walls (and doors) */
		else if (r_ptr->flags2 & RF2_KILL_WALL)
		{
			/* Eat through walls/doors/rubble */
			do_move = TRUE;

			/* Monster destroyed a wall */
			did_kill_wall = TRUE;

			if (one_in_(GRINDNOISE))
			{
				msg_print("There is a grinding sound.");
			}

			/* Notice */
			cave_set_feat(nx, ny, FEAT_FLOOR);
		}

		else if (!cave_floor_grid(c_ptr))
		{
			/* This monster cannot walk through walls */
			do_move = FALSE;
		}

		/* Require "empty" fields */
		if (fields_have_flags(c_ptr->fld_idx, FIELD_INFO_NO_ENTER))
		{
			do_move = FALSE;
		}

		/* 
		 * Test for fields that will not allow this
		 * specific monster to pass.  (i.e. Glyph of warding)
		 */

		/* Initialise information to pass to action functions */
		mon_enter_test.m_ptr = m_ptr;

		/* Set up flags */
		mon_enter_test.flags = 0x00;
		if (do_move) mon_enter_test.flags |= MEG_DO_MOVE;

		/* Call the hook */
		field_hook(&c_ptr->fld_idx, FIELD_ACT_MON_ENTER_TEST,
				   (vptr) &mon_enter_test);

		/* Get result */
		if (mon_enter_test.flags & (MEG_DO_MOVE)) do_move = TRUE;
		if (mon_enter_test.flags & (MEG_OPEN)) did_open_door = TRUE;
		if (mon_enter_test.flags & (MEG_BASH)) did_bash_door = TRUE;
		if (mon_enter_test.flags & (MEG_DO_TURN)) do_turn = TRUE;

		/* Some monsters never attack */
		if (do_move && (ny == p_ptr->py) && (nx == p_ptr->px) &&
			(r_ptr->flags1 & RF1_NEVER_BLOW))
		{
			/* Hack -- memorize lack of attacks */
			if (m_ptr->ml) r_ptr->r_flags1 |= (RF1_NEVER_BLOW);

			/* Do not move */
			do_move = FALSE;
		}

		/* Handle closed doors and secret doors */
		if (do_move && ((c_ptr->feat == FEAT_CLOSED)
						|| (c_ptr->feat == FEAT_SECRET)) &&
			(r_ptr->flags2 & RF2_OPEN_DOOR) &&
			(!is_pet(m_ptr) || p_ptr->pet_open_doors))
		{
			/* Open the door */
			cave_set_feat(nx, ny, FEAT_OPEN);

			/* Take a turn */
			do_turn = TRUE;

			/* Do not move in any case. */
			do_move = FALSE;

			/* The door was opened */
			did_open_door = TRUE;
		}
		else if (((c_ptr->feat == FEAT_CLOSED) || (c_ptr->feat == FEAT_SECRET))
				 && !did_pass_wall)
		{
			/* Monsters cannot walk through closed doors */
			do_move = FALSE;
		}

		/* The player is in the way.  Attack him. */
		if (do_move && (ny == p_ptr->py) && (nx == p_ptr->px))
		{
			/* Do the attack */
			(void) make_attack_normal(m_idx);

			/* Do not move */
			do_move = FALSE;

			/* Took a turn */
			do_turn = TRUE;
		}

		if ((c_ptr->feat >= FEAT_PATTERN_START) &&
			(c_ptr->feat <= FEAT_PATTERN_XTRA2) &&
			!do_turn && !(r_ptr->flags7 & RF7_CAN_FLY))
		{
			do_move = FALSE;
		}


		/* A monster is in the way */
		if (do_move && c_ptr->m_idx)
		{
			monster_race *z_ptr = &r_info[y_ptr->r_idx];
			monster_type *m2_ptr = &m_list[c_ptr->m_idx];

			/* Assume no movement */
			do_move = FALSE;

			/* Attack 'enemies' */
			if (((r_ptr->flags2 & (RF2_KILL_BODY)) &&
				 (r_ptr->mexp * r_ptr->level > z_ptr->mexp * z_ptr->level) &&
				 (cave_floor_grid(c_ptr))) ||
				are_enemies(m_ptr, m2_ptr) || m_ptr->confused)
			{
				do_move = FALSE;

				if (r_ptr->flags2 & RF2_KILL_BODY)
					r_ptr->r_flags2 |= (RF2_KILL_BODY);

				/* attack */
				if ((m2_ptr->r_idx) && (m2_ptr->hp >= 0))
				{
					if (monst_attack_monst(m_idx, area(nx, ny)->m_idx))
						return;
				}
			}

			/* Push past weaker monsters (unless leaving a wall) */
			else if ((r_ptr->flags2 & RF2_MOVE_BODY) &&
					 (r_ptr->mexp > z_ptr->mexp) && cave_floor_grid(c_ptr) &&
					 (cave_floor_grid(area(m_ptr->fx, m_ptr->fy))))
			{
				/* Allow movement */
				do_move = TRUE;

				/* Monster pushed past another monster */
				did_move_body = TRUE;

				/* XXX XXX XXX Message */
			}
		}

		/*
		 * Check if monster can cross terrain
		 * This is checked after the normal attacks
		 * to allow monsters to attack an enemy,
		 * even if it can't enter the terrain.
		 */
		if (do_move && !monster_can_cross_terrain(c_ptr->feat, r_ptr))
		{
			/* Assume no move allowed */
			do_move = FALSE;
		}

		/* Some monsters never move */
		if (do_move && (r_ptr->flags1 & RF1_NEVER_MOVE))
		{
			/* Hack -- memorize lack of attacks */
			if (m_ptr->ml) r_ptr->r_flags1 |= (RF1_NEVER_MOVE);

			/* Do not move */
			do_move = FALSE;
		}



		/* Creature has been allowed move */
		if (do_move)
		{
			s16b this_o_idx, next_o_idx;

			cave_type *old_ptr = area(ox, oy);

			/* Take a turn */
			do_turn = TRUE;

			/* Look to see if we've spotted a mimic */
			if ((m_ptr->smart & SM_MIMIC) && m_ptr->ml)
			{
				char m_name2[80];

				/* Get name */
				monster_desc(m_name2, m_ptr, 0x88);

				/* Toggle flag */
				m_ptr->smart &= ~(SM_MIMIC);

				/* It is in the monster list now */
				update_mon_vis(m_ptr->r_idx, 1);

				/* We've spotted it */
				msg_format("You see %s!", m_name2);
			}

			/* Process fields under the monster. */
			field_hook(&old_ptr->fld_idx,
					   FIELD_ACT_MONSTER_LEAVE, (vptr) m_ptr);

			/* Hack -- Update the old location */
			old_ptr->m_idx = c_ptr->m_idx;

			/* Mega-Hack -- move the old monster, if any */
			if (c_ptr->m_idx)
			{
				/* Move the old monster */
				y_ptr->fy = oy;
				y_ptr->fx = ox;

				/* Update the old monster */
				update_mon(c_ptr->m_idx, TRUE);

				/* Wake up the moved monster */
				m_list[c_ptr->m_idx].csleep = 0;
			}

			/* Hack -- Update the new location */
			c_ptr->m_idx = m_idx;

			/* Move the monster */
			m_ptr->fy = ny;
			m_ptr->fx = nx;

			/* Update the monster */
			update_mon(m_idx, TRUE);

			/* Process fields under the monster. */
			field_hook(&old_ptr->fld_idx,
					   FIELD_ACT_MONSTER_ENTER, (vptr) m_ptr);

			/* Redraw the old grid */
			lite_spot(ox, oy);

			/* Redraw the new grid */
			lite_spot(nx, ny);

			/* Possible disturb */
			if (m_ptr->ml && (disturb_move ||
							  ((m_ptr->mflag & MFLAG_VIEW) && disturb_near)))
			{
				/* Disturb */
				if (is_hostile(m_ptr))
					disturb(FALSE);
			}

			/* Scan all objects in the grid */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Skip gold */
				if (o_ptr->tval == TV_GOLD) continue;

				/*
				 * Skip statues, to avoid extreme silliness
				 * like a novice rogue with pockets full of them.
				 */
				if (o_ptr->tval == TV_STATUE) continue;

				/* Take or Kill objects on the floor */
				if ((r_ptr->flags2 & (RF2_TAKE_ITEM | RF2_KILL_ITEM)) &&
					(!is_pet(m_ptr) || p_ptr->pet_pickup_items))
				{
					u32b f1, f2, f3;

					u32b flg3 = 0L;

					char o_name[256];

					/* Extract some flags */
					object_flags(o_ptr, &f1, &f2, &f3);

					/* Acquire the object name */
					object_desc(o_name, o_ptr, TRUE, 3, 256);

					/* Acquire the monster name */
					monster_desc(m_name, m_ptr, 0x04);

					/* React to objects that hurt the monster */
					if (f1 & TR1_KILL_DRAGON) flg3 |= (RF3_DRAGON);
					if (f1 & TR1_SLAY_DRAGON) flg3 |= (RF3_DRAGON);
					if (f1 & TR1_SLAY_TROLL) flg3 |= (RF3_TROLL);
					if (f1 & TR1_SLAY_GIANT) flg3 |= (RF3_GIANT);
					if (f1 & TR1_SLAY_ORC) flg3 |= (RF3_ORC);
					if (f1 & TR1_SLAY_DEMON) flg3 |= (RF3_DEMON);
					if (f1 & TR1_SLAY_UNDEAD) flg3 |= (RF3_UNDEAD);
					if (f1 & TR1_SLAY_ANIMAL) flg3 |= (RF3_ANIMAL);
					if (f1 & TR1_SLAY_EVIL) flg3 |= (RF3_EVIL);

					/* The object cannot be picked up by the monster */
					if ((o_ptr->flags3 & TR3_INSTA_ART) ||
						(r_ptr->flags3 & flg3))
					{
						/* Only give a message for "take_item" */
						if ((r_ptr->flags2 & (RF2_TAKE_ITEM))
							&& (r_ptr->flags2 & (RF2_STUPID)))
						{
							/* Take note */
							did_take_item = TRUE;

							/* Describe observable situations */
							if (m_ptr->ml && player_can_see_grid(pc_ptr))
							{
								/* Dump a message */
								msg_format
									("%^s tries to pick up %s, but fails.",
									 m_name, o_name);
							}
						}
					}

					/* Pick up the item */
					else if (r_ptr->flags2 & RF2_TAKE_ITEM)
					{
						/* Take note */
						did_take_item = TRUE;

						/* Describe observable situations */
						if (player_can_see_grid(pc_ptr))
						{
							/* Dump a message */
							msg_format("%^s picks up %s.", m_name, o_name);
						}

						/* Excise the object */
						excise_object_idx(this_o_idx);

						/* Forget mark */
						o_ptr->marked = FALSE;

						/* Forget location */
						o_ptr->iy = o_ptr->ix = 0;

						/* XXX Hack - Forget region */
						o_ptr->region = 0;

						/* Memorize monster */
						o_ptr->held_m_idx = m_idx;

						/* Build a stack */
						o_ptr->next_o_idx = m_ptr->hold_o_idx;

						/* Carry object */
						m_ptr->hold_o_idx = this_o_idx;
					}

					/* Destroy the item if not a pet */
					else if (!is_pet(m_ptr))
					{
						/* Take note */
						did_kill_item = TRUE;

						/* Describe observable situations */
						if (player_can_see_grid(pc_ptr))
						{
							/* Dump a message */
							msg_format("%^s destroys %s.", m_name, o_name);
						}

						/* Delete the object */
						delete_object_idx(this_o_idx);
					}
				}
			}
		}

		/* Stop when done */
		if (do_turn) break;
	}

	/* If we haven't done anything, try casting a spell again */
	if (!do_turn && !do_move && !m_ptr->monfear && !stupid_monsters &&
		!make_attack_spell(m_idx))
	{
		/* Cast spell */
		if (make_attack_spell(m_idx)) return;
	}

	/* Notice changes in view */
	if (do_move && (r_ptr->flags7 & (RF7_LITE_1 | RF7_LITE_2)))
	{
		/* Update some things */
		p_ptr->update |= (PU_MON_LITE);
	}

	/* Learn things from observable monster */
	if (m_ptr->ml)
	{
		/* Monster opened a door */
		if (did_open_door) r_ptr->r_flags2 |= (RF2_OPEN_DOOR);

		/* Monster bashed a door */
		if (did_bash_door) r_ptr->r_flags2 |= (RF2_BASH_DOOR);

		/* Monster tried to pick something up */
		if (did_take_item) r_ptr->r_flags2 |= (RF2_TAKE_ITEM);

		/* Monster tried to crush something */
		if (did_kill_item) r_ptr->r_flags2 |= (RF2_KILL_ITEM);

		/* Monster pushed past another monster */
		if (did_move_body) r_ptr->r_flags2 |= (RF2_MOVE_BODY);

		/* Monster passed through a wall */
		if (did_pass_wall) r_ptr->r_flags2 |= (RF2_PASS_WALL);

		/* Monster destroyed a wall */
		if (did_kill_wall) r_ptr->r_flags2 |= (RF2_KILL_WALL);
	}


	/* Hack -- get "bold" if out of options */
	if (!do_turn && !do_move && m_ptr->monfear)
	{
		/* No longer afraid */
		m_ptr->monfear = 0;

		/* Message if seen */
		if (m_ptr->ml)
		{
			/* Acquire the monster name */
			monster_desc(m_name, m_ptr, 0);

			/* Dump a message */
			msg_format("%^s turns to fight!", m_name);

			chg_virtue(V_COMPASSION, -1);
		}

		/* XXX XXX XXX Actually do something now (?) */
	}
}


/*
 * Process a monster
 *
 * The monster is known to be within 100 grids of the player
 *
 * In several cases, we directly update the monster lore
 *
 * Note that a monster is only allowed to "reproduce" if there
 * are a limited number of "reproducing" monsters on the current
 * level.  This should prevent the level from being "swamped" by
 * reproducing monsters.  It also allows a large mass of mice to
 * prevent a louse from multiplying, but this is a small price to
 * pay for a simple multiplication method.
 *
 * XXX Monster fear is slightly odd, in particular, monsters will
 * fixate on opening a door even if they cannot open it.  Actually,
 * the same thing happens to normal monsters when they hit a door
 *
 * XXX XXX XXX In addition, monsters which *cannot* open or bash
 * down a door will still stand there trying to open it...
 *
 * XXX Technically, need to check for monster in the way
 * combined with that monster being in a wall (or door?)
 *
 * A "direction" of "5" means "pick a random direction".
 */
static void process_monster(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	char m_name[80];

	int d, oy, ox;
	int mm[8];

	cave_type *c_ptr;

	bool gets_angry = FALSE;


	/* Quantum monsters are odd */
	if (r_ptr->flags2 & (RF2_QUANTUM))
	{
		/* Sometimes skip move */
		if (one_in_(2)) return;

		/* Sometimes die */
		if (one_in_((m_idx % 100) + 10) && !(r_ptr->flags1 & RF1_QUESTOR))
		{
			bool sad = FALSE;

			if (is_pet(m_ptr) && !(m_ptr->ml))
				sad = TRUE;

			if (m_ptr->ml)
			{
				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Oops */
				msg_format("%^s disappears!", m_name);
			}

			/* Generate treasure, etc */
			(void) monster_death(m_idx, TRUE);

			/* Delete the monster */
			delete_monster_idx(m_idx);

			if (sad)
			{
				msg_print("You feel sad for a moment.");
			}

			return;
		}
	}

	/* Get the origin */
	oy = m_ptr->fy;
	ox = m_ptr->fx;

	/* Access that cave grid */
	c_ptr = area(ox, oy);

	/* Process fields under the monster. */
	field_hook(&c_ptr->fld_idx, FIELD_ACT_MONSTER_ON, (vptr) m_ptr);

	/* Handle "sleep" */
	if (m_ptr->csleep)
	{
		u32b notice = 0;

		/* Hack -- handle non-aggravation */
		if (!p_ptr->aggravate) notice = randint0(1024);

		/* Nightmare monsters are more alert */
		if (ironman_nightmare) notice /= 2;

		/* Hack -- See if monster "notices" player */
		if ((notice * notice * notice) <= p_ptr->noise)
		{
			/* Hack -- amount of "waking" */
			d = 1;

			/* Wake up faster near the player */
			if (m_ptr->cdis < 50) d = (100 / m_ptr->cdis);

			/* Hack -- handle aggravation */
			if (p_ptr->aggravate) d = m_ptr->csleep;

			/* Still asleep */
			if (m_ptr->csleep > d)
			{
				/* Monster wakes up "a little bit" */
				m_ptr->csleep -= d;

				/* Notice the "not waking up" */
				if (m_ptr->ml)
				{
					/* Hack -- Count the ignores */
					if (r_ptr->r_ignore < MAX_UCHAR)
					{
						r_ptr->r_ignore++;
					}
				}
			}

			/* Just woke up */
			else
			{
				/* Reset sleep counter */
				m_ptr->csleep = 0;

				/* Notice the "waking up" */
				if ((m_ptr->ml) && (!(m_ptr->smart & SM_MIMIC)))
				{
					/* Acquire the monster name */
					monster_desc(m_name, m_ptr, 0);

					/* Dump a message */
					msg_format("%^s wakes up.", m_name);

					/* Redraw the health bar */
					if (p_ptr->health_who == m_idx)
						p_ptr->redraw |= (PR_HEALTH);

					/* Hack -- Count the wakings */
					if (r_ptr->r_wake < MAX_UCHAR)
					{
						r_ptr->r_wake++;
					}
				}
			}
		}

		/* Still sleeping */
		if (m_ptr->csleep) return;
	}


	/* Handle "stun" */
	if (m_ptr->stunned)
	{
		d = 1;

		/* Make a "saving throw" against stun */
		if (randint0(10000) <= r_ptr->level * r_ptr->level)
		{
			/* Recover fully */
			d = m_ptr->stunned;
		}

		/* Hack -- Recover from stun */
		if (m_ptr->stunned > d)
		{
			/* Recover somewhat */
			m_ptr->stunned -= d;
		}

		/* Fully recover */
		else
		{
			/* Recover fully */
			m_ptr->stunned = 0;

			/* Message if visible */
			if (m_ptr->ml)
			{
				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
				msg_format("%^s is no longer stunned.", m_name);
			}
		}

		/* Still stunned */
		if (m_ptr->stunned) return;
	}


	/* Handle confusion */
	if (m_ptr->confused)
	{
		/* Amount of "boldness" */
		d = randint1(r_ptr->level / 20 + 1);

		/* Still confused */
		if (m_ptr->confused > d)
		{
			/* Reduce the confusion */
			m_ptr->confused -= d;
		}

		/* Recovered */
		else
		{
			/* No longer confused */
			m_ptr->confused = 0;

			/* Message if visible */
			if ((m_ptr->ml) && (!(m_ptr->smart & SM_MIMIC)))
			{
				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
				msg_format("%^s is no longer confused.", m_name);
			}
		}
	}

	/* Handle Invulnerability */
	if (m_ptr->invulner)
	{
		/* Reduce by one, note if expires */
		m_ptr->invulner--;

		if (!(m_ptr->invulner) && m_ptr->ml)
		{
			/* Acquire the monster name */
			monster_desc(m_name, m_ptr, 0);

			/* Dump a message */
			msg_format("%^s is no longer invulnerable.", m_name);
		}
	}

	/* No one wants to be your friend if you're aggravating */
	if (!is_hostile(m_ptr) && p_ptr->aggravate)
		gets_angry = TRUE;

	/* Acquire the monster name */
	monster_desc(m_name, m_ptr, 0);

	if (gets_angry)
	{
		msg_format("%^s suddenly becomes hostile!", m_name);
		set_hostile(m_ptr);
	}

	/* Handle "fear" */
	if (m_ptr->monfear)
	{
		/* Amount of "boldness" */
		d = randint1(r_ptr->level / 20 + 1);

		/* Still afraid */
		if (m_ptr->monfear > d)
		{
			/* Reduce the fear */
			m_ptr->monfear -= d;
		}

		/* Recover from fear, take note if seen */
		else
		{
			/* No longer afraid */
			m_ptr->monfear = 0;

			/* Visual note */
			if (m_ptr->ml)
			{
				char m_poss[80];

				/* Acquire the monster poss */
				monster_desc(m_poss, m_ptr, 0x22);

				/* Dump a message */
				msg_format("%^s recovers %s courage.", m_name, m_poss);
			}
		}
	}


	/* Attempt to "multiply" if able and allowed */
	if ((r_ptr->flags2 & RF2_MULTIPLY) && (num_repro < MAX_REPRO))
	{
		int k, y, x;

		/* Count the adjacent monsters */
		for (k = 0, y = oy - 1; y <= oy + 1; y++)
		{
			for (x = ox - 1; x <= ox + 1; x++)
			{
				/* Ignore locations off of edge */
				if (!in_bounds2(x, y)) continue;

				if (area(x, y)->m_idx) k++;
			}
		}

		/* Hack -- multiply slower in crowded areas */
		if ((k < 4) && (!k || one_in_(k * MON_MULT_ADJ)))
		{
			/* Try to multiply */
			if (multiply_monster
				(m_idx, FALSE, is_friendly(m_ptr), is_pet(m_ptr)))
			{
				/* Take note if visible */
				if (m_ptr->ml)
				{
					r_ptr->r_flags2 |= (RF2_MULTIPLY);
				}

				/* Multiplying takes energy */
				return;
			}
		}
	}


	/* Hack! "Cyber" monster makes noise... */
	if (strstr((r_name + r_ptr->name), "Cyber") && one_in_(CYBERNOISE) &&
		!m_ptr->ml && (m_ptr->cdis <= MAX_SIGHT))
	{
		msg_print("You hear heavy steps.");
	}

	/* Access that cave grid */
	c_ptr = area(ox, oy);

	/* Some monsters can speak */
	if (speak_unique &&
		(r_ptr->flags2 & RF2_CAN_SPEAK) && one_in_(SPEAK_CHANCE) &&
		player_has_los_grid(parea(ox, oy)))
	{
		char monmessage[1024];
		cptr filename;

		/* Acquire the monster name/poss */
		if (!m_ptr->ml) strcpy(m_name, "It");

		/* Select the file for monster quotes */
		if (m_ptr->monfear)
			filename = "monfear.txt";
		else if (is_friendly(m_ptr))
			filename = "monfrien.txt";
		else
			filename = "monspeak.txt";

		/* Get the monster line */
		if (get_rnd_line(filename, m_ptr->r_idx, monmessage) == 0)
		{
			/* Say something */
			msg_format("%^s %s", m_name, monmessage);
		}
	}


	/* Attempt to cast a spell */
	if (make_attack_spell(m_idx)) return;

	/*
	 * Attempt to cast a spell at an enemy other than the player
	 * (may slow the game a smidgeon, but I haven't noticed.)
	 */
	if (monst_spell_monst(m_idx)) return;


	/* Hack -- Assume no movement */
	mm[0] = mm[1] = mm[2] = mm[3] = 0;
	mm[4] = mm[5] = mm[6] = mm[7] = 0;

	/* Confused -- 100% random */
	if (m_ptr->confused)
	{
		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 75% random movement */
	else if ((r_ptr->flags1 & RF1_RAND_50) && (r_ptr->flags1 & RF1_RAND_25) &&
			 (randint0(100) < 75))
	{
		/* Memorize flags */
		if (m_ptr->ml) r_ptr->r_flags1 |= (RF1_RAND_50);
		if (m_ptr->ml) r_ptr->r_flags1 |= (RF1_RAND_25);

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 50% random movement */
	else if ((r_ptr->flags1 & RF1_RAND_50) && (randint0(100) < 50))
	{
		/* Memorize flags */
		if (m_ptr->ml) r_ptr->r_flags1 |= (RF1_RAND_50);

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 25% random movement */
	else if ((r_ptr->flags1 & RF1_RAND_25) && (randint0(100) < 25))
	{
		/* Memorize flags */
		if (m_ptr->ml) r_ptr->r_flags1 |= RF1_RAND_25;

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* Can't reach player - find something else to hit */
	else if ((r_ptr->flags1 & RF1_NEVER_MOVE) && (m_ptr->cdis > 1))
	{
		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* Pets will follow the player */
	else if (is_pet(m_ptr))
	{
		/* Are we trying to avoid the player? */
		bool avoid = ((p_ptr->pet_follow_distance < 0) &&
					  (m_ptr->cdis <= (0 - p_ptr->pet_follow_distance)));

		/* Do we want to find the player? */
		bool lonely = (!avoid && (m_ptr->cdis > p_ptr->pet_follow_distance));

		/* Should we find the player if we can't find a monster? */
		bool distant = (m_ptr->cdis > PET_SEEK_DIST);

		/* by default, move randomly */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;

		/* Look for an enemy */
		if (get_enemy_target(m_ptr))
		{
			convert_target_dir(m_ptr, mm);
		}
		else
		{
			/* Find the player if necessary */
			if (avoid || lonely || distant)
			{
				/* Remember the leash length */
				int dis = p_ptr->pet_follow_distance;

				/* Hack -- adjust follow distance temporarily */
				if (p_ptr->pet_follow_distance > PET_SEEK_DIST)
				{
					p_ptr->pet_follow_distance = PET_SEEK_DIST;
				}

				/* Find the player */
				(void) get_moves(m_idx, mm);

				/* Restore the leash */
				p_ptr->pet_follow_distance = dis;
			}
		}
	}

	/* Friendly monster movement */
	else if (!is_hostile(m_ptr))
	{
		/* by default, move randomly */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;

		/* Look for an enemy */
		if (get_enemy_target(m_ptr))
		{
			convert_target_dir(m_ptr, mm);
		}
		else
		{
			/* Failure... we need to do something else */
		}
	}
	/* Normal movement */
	else if (stupid_monsters)
	{
		/* Logical moves */
		(void) get_moves(m_idx, mm);
	}
	else
	{
		/* Logical moves, may do nothing */
		if (!get_moves(m_idx, mm)) return;
	}

	/* Make the move */
	take_move(m_idx, mm);
}




/*
 * Process all the "live" monsters, once per game turn.
 *
 * During each game turn, we scan through the list of all the "live" monsters,
 * (backwards, so we can excise any "freshly dead" monsters), energizing each
 * monster, and allowing fully energized monsters to move, attack, pass, etc.
 *
 * Note that monsters can never move in the monster array (except when the
 * "compact_monsters()" function is called by "dungeon()" or "save_player()").
 *
 * This function is responsible for at least half of the processor time
 * on a normal system with a "normal" amount of monsters and a player doing
 * normal things.
 *
 * When the player is resting, virtually 90% of the processor time is spent
 * in this function, and its children, "process_monster()" and "make_move()".
 *
 * Most of the rest of the time is spent in "update_view()" and "lite_spot()",
 * especially when the player is running.
 *
 * Note the special "MFLAG_BORN" flag, which allows us to ignore "fresh"
 * monsters while they are still being "born".  A monster is "fresh" only
 * during the turn in which it is created, and we use the "hack_m_idx" to
 * determine if the monster is yet to be processed during the current turn.
 *
 * Note the special "MFLAG_NICE" flag, which allows the player to get one
 * move before any "nasty" monsters get to use their spell attacks.
 *
 * Note that when the "knowledge" about the currently tracked monster
 * changes (flags, attacks, spells), we induce a redraw of the monster
 * recall window.
 */
void process_monsters(int min_energy)
{
	int i;
	int fx, fy;

	bool test;

	monster_type *m_ptr;
	monster_race *r_ptr;

	int old_monster_race_idx;

	u32b old_r_flags1 = 0L;
	u32b old_r_flags2 = 0L;
	u32b old_r_flags3 = 0L;
	u32b old_r_flags4 = 0L;
	u32b old_r_flags5 = 0L;
	u32b old_r_flags6 = 0L;

	byte old_r_blows0 = 0;
	byte old_r_blows1 = 0;
	byte old_r_blows2 = 0;
	byte old_r_blows3 = 0;

	byte old_r_cast_inate = 0;
	byte old_r_cast_spell = 0;

	int old_total_friends = total_friends;
	s32b old_friend_align = friend_align;

	cave_type *c_ptr;

	/* Clear some variables */
	total_friends = 0;
	total_friend_levels = 0;
	friend_align = 0;

	/* Clear monster fighting indicator */
	p_ptr->mon_fight = FALSE;

	/* Memorize old race */
	old_monster_race_idx = p_ptr->monster_race_idx;

	/* Acquire knowledge */
	if (p_ptr->monster_race_idx)
	{
		/* Acquire current monster */
		r_ptr = &r_info[p_ptr->monster_race_idx];

		/* Memorize flags */
		old_r_flags1 = r_ptr->r_flags1;
		old_r_flags2 = r_ptr->r_flags2;
		old_r_flags3 = r_ptr->r_flags3;
		old_r_flags4 = r_ptr->r_flags4;
		old_r_flags5 = r_ptr->r_flags5;
		old_r_flags6 = r_ptr->r_flags6;

		/* Memorize blows */
		old_r_blows0 = r_ptr->r_blows[0];
		old_r_blows1 = r_ptr->r_blows[1];
		old_r_blows2 = r_ptr->r_blows[2];
		old_r_blows3 = r_ptr->r_blows[3];

		/* Memorize castings */
		old_r_cast_inate = r_ptr->r_cast_inate;
		old_r_cast_spell = r_ptr->r_cast_spell;
	}


	/* Process the monsters (backwards) */
	for (i = m_max - 1; i >= 1; i--)
	{
		/* Access the monster */
		m_ptr = &m_list[i];
		r_ptr = &r_info[m_ptr->r_idx];

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Ignore "dead" monsters */
		if (!m_ptr->r_idx) continue;

		/* Calculate "upkeep" for pets */
		if (is_pet(m_ptr))
		{
			total_friends++;
			total_friend_levels += r_ptr->level;

			/* Determine pet alignment */
			if (r_ptr->flags3 & RF3_GOOD)
			{
				friend_align += r_ptr->level;
			}
			else if (r_ptr->flags3 & RF3_EVIL)
			{
				friend_align -= r_ptr->level;
			}
		}

		/* Has the monster already moved? */
		if (m_ptr->mflag & MFLAG_MOVE) continue;

		/* Make a move */
		m_ptr->mflag |= (MFLAG_MOVE);

		/* Not enough energy to move */
		if (m_ptr->energy < min_energy) continue;

		/* Use up "some" energy */
		m_ptr->energy -= 100;


		/* Hack -- Require proximity */
		if (m_ptr->cdis >= 100) continue;


		/* Access the location */
		fx = m_ptr->fx;
		fy = m_ptr->fy;

		c_ptr = area(fx, fy);

		/* Assume no move */
		test = FALSE;

		/* Allow more activity with pets around */
		if (old_total_friends)
		{
			test = TRUE;
		}

		/* Handle "sensing radius" */
		if (m_ptr->cdis <= r_ptr->aaf)
		{
			/* We can "sense" the player */
			test = TRUE;
		}

		/* Handle "sight" and "aggravation" */
		else if ((m_ptr->cdis <= MAX_SIGHT) &&
				 (player_has_los_grid(parea(fx, fy)) || p_ptr->aggravate))
		{
			/* We can "see" or "feel" the player */
			test = TRUE;
		}

		/* 
		 * Hack -- Monsters can "smell" the player from far away
		 * Note that most monsters have "aaf" of "20" or so
		 */
		else if ((area(p_ptr->px, p_ptr->py)->when == c_ptr->when) &&
				 (c_ptr->cost < MONSTER_FLOW_DEPTH) &&
				 (c_ptr->cost < r_ptr->aaf))
		{
			/* We can "smell" the player */
			test = TRUE;
		}

		/* Do nothing */
		if (!test) continue;

		/* Save global index */
		hack_m_idx = i;

		/* Process the monster */
		process_monster(i);

		/* Hack -- notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Notice leaving */
		if (p_ptr->leaving) break;
	}

	/* Reset global index */
	hack_m_idx = 0;


	/* Tracking a monster race (the same one we were before) */
	if (p_ptr->monster_race_idx
		&& (p_ptr->monster_race_idx == old_monster_race_idx))
	{
		/* Acquire monster race */
		r_ptr = &r_info[p_ptr->monster_race_idx];

		/* Check for knowledge change */
		if ((old_r_flags1 != r_ptr->r_flags1) ||
			(old_r_flags2 != r_ptr->r_flags2) ||
			(old_r_flags3 != r_ptr->r_flags3) ||
			(old_r_flags4 != r_ptr->r_flags4) ||
			(old_r_flags5 != r_ptr->r_flags5) ||
			(old_r_flags6 != r_ptr->r_flags6) ||
			(old_r_blows0 != r_ptr->r_blows[0]) ||
			(old_r_blows1 != r_ptr->r_blows[1]) ||
			(old_r_blows2 != r_ptr->r_blows[2]) ||
			(old_r_blows3 != r_ptr->r_blows[3]) ||
			(old_r_cast_inate != r_ptr->r_cast_inate) ||
			(old_r_cast_spell != r_ptr->r_cast_spell))
		{
			/* Window stuff */
			p_ptr->window |= (PW_MONSTER);
		}
	}

	if (old_friend_align != friend_align) p_ptr->update |= (PU_BONUS);
}

/*
 * Clear 'moved' status from all monsters.
 * 
 * Clear noise if appropriate???
 */
void reset_monsters(void)
{
	int i;
	monster_type *m_ptr;

	/* Process the monsters */
	for (i = 1; i < m_max; i++)
	{
		/* Access the monster */
		m_ptr = &m_list[i];

		/* Monster is ready to go again */
		m_ptr->mflag &= ~(MFLAG_MOVE);
	}

#if 0

	/* Clear the current noise after it is used to wake up monsters */
	if (turn % 10 == 0)
	{
		total_wakeup_chance = 0L;
		add_wakeup_chance = 0;
	}

#endif /* 0 */
}
