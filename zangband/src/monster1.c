/* File: monster1.c */

/* Purpose: describe monsters (using monster memory) */

/*
 * Copyright (c) 1989 James E. Wilson, Christopher J. Stuart
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Pronoun arrays, by gender.
 */
static cptr wd_he[3] =
{ "it", "he", "she" };
static cptr wd_his[3] =
{ "its", "his", "her" };


/*
 * Pluralizer.  Args(count, singular, plural)
 */
#define plural(c,s,p) \
    (((c) == 1) ? (s) : (p))


/*
 * Determine if the "armor" is known
 * The higher the level, the fewer kills needed.
 */
static bool know_armour(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	s32b level = r_ptr->level;

	s32b kills = r_ptr->r_tkills;

	/* Normal monsters */
	if (kills > 304 / (4 + level)) return (TRUE);

	/* Skip non-uniques */
	if (!(MON_FLAG(r_ptr, 0, UNIQUE))) return (FALSE);

	/* Unique monsters */
	if (kills > 304 / (38 + (5 * level) / 4)) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Determine if the "damage" of the given attack is known
 * the higher the level of the monster, the fewer the attacks you need,
 * the more damage an attack does, the more attacks you need
 */
static bool know_damage(int r_idx, int i)
{
	monster_race *r_ptr = &r_info[r_idx];

	s32b level = r_ptr->level;

	s32b a = r_ptr->r_blows[i];

	s32b d1 = r_ptr->blow[i].d_dice;
	s32b d2 = r_ptr->blow[i].d_side;

	s32b d = d1 * d2;

	/* Normal monsters */
	if ((4 + level) * a > 80 * d) return (TRUE);

	/* Skip non-uniques */
	if (!(MON_FLAG(r_ptr, 0, UNIQUE))) return (FALSE);

	/* Unique monsters */
	if ((4 + level) * (2 * a) > 80 * d) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Hack -- display monster information using "roff()"
 *
 * Note that there is now a compiler option to only read the monster
 * descriptions from the raw file when they are actually needed, which
 * saves about 60K of memory at the cost of disk access during monster
 * recall, which is optional to the user.
 *
 * This function should only be called with the cursor placed at the
 * left edge of the screen, on a cleared line, in which the recall is
 * to take place.  One extra blank line is left after the recall.
 */
static void roff_mon_aux(int r_idx, int remem)
{
	monster_race *r_ptr = &r_info[r_idx];

	bool old = FALSE;
	bool sin = FALSE;

	int m, n, r;
	cptr p, q;

	int msex = 0;
	int speed = (ironman_nightmare) ? r_ptr->speed + 5 : r_ptr->speed;

	bool breath = FALSE;
	bool magic = FALSE;

	u32b ff[7];

	int vn = 0;
	cptr vp[80];

	monster_race save_mem;

	/* Descriptions */
	char buf[2048];

	/* Cheat -- Know everything */
	if (cheat_know)
	{
		/* XXX XXX XXX */

		/* Save the "old" memory (structure copy) */
		save_mem = *r_ptr;

		/* Hack -- Maximal info */
		r_ptr->r_wake = r_ptr->r_ignore = MAX_UCHAR;

		/* Observe "maximal" attacks */
		for (m = 0; m < 4; m++)
		{
			/* Examine "actual" blows */
			if (r_ptr->blow[m].effect || r_ptr->blow[m].method)
			{
				/* Hack -- maximal observations */
				r_ptr->r_blows[m] = MAX_UCHAR;
			}
		}

		/* Hack -- maximal drops */
		r_ptr->r_drop_gold = r_ptr->r_drop_item =
			(((MON_FLAG(r_ptr, 0, DROP_4D2)) ? 8 : 0) +
			 ((MON_FLAG(r_ptr, 0, DROP_3D2)) ? 6 : 0) +
			 ((MON_FLAG(r_ptr, 0, DROP_2D2)) ? 4 : 0) +
			 ((MON_FLAG(r_ptr, 0, DROP_1D2)) ? 2 : 0) +
			 ((MON_FLAG(r_ptr, 0, DROP_90)) ? 1 : 0) +
			 ((MON_FLAG(r_ptr, 0, DROP_60)) ? 1 : 0));

		/* Hack -- but only "valid" drops */
		if (MON_FLAG(r_ptr, 0, ONLY_GOLD)) r_ptr->r_drop_item = 0;
		if (MON_FLAG(r_ptr, 0, ONLY_ITEM)) r_ptr->r_drop_gold = 0;

		/* Hack -- observe many spells */
		r_ptr->r_cast_inate = MAX_UCHAR;
		r_ptr->r_cast_spell = MAX_UCHAR;

		/* Hack -- know all the flags */
		r_ptr->r_flags[0] = r_ptr->flags[0];
		r_ptr->r_flags[1] = r_ptr->flags[1];
		r_ptr->r_flags[2] = r_ptr->flags[2];
		r_ptr->r_flags[3] = r_ptr->flags[3];
		r_ptr->r_flags[4] = r_ptr->flags[4];
		r_ptr->r_flags[5] = r_ptr->flags[5];
	}


	/* Extract a gender (if applicable) */
	if (MON_FLAG(r_ptr, 0, FEMALE)) msex = 2;
	else if (MON_FLAG(r_ptr, 0, MALE)) msex = 1;


	/* Obtain a copy of the "known" flags */
	ff[0] = (r_ptr->flags[0] & r_ptr->r_flags[0]);
	ff[1] = (r_ptr->flags[1] & r_ptr->r_flags[1]);
	ff[2] = (r_ptr->flags[2] & r_ptr->r_flags[2]);
	ff[3] = (r_ptr->flags[3] & r_ptr->r_flags[3]);
	ff[4] = (r_ptr->flags[4] & r_ptr->r_flags[4]);
	ff[5] = (r_ptr->flags[5] & r_ptr->r_flags[5]);
	ff[6] = (r_ptr->flags[6]);


	/* Assume some "obvious" flags */
	if (MON_FLAG(r_ptr, 0, UNIQUE)) SET_FLAG(ff, 0, RF0_UNIQUE);
	if (MON_FLAG(r_ptr, 0, QUESTOR)) SET_FLAG(ff, 0, RF0_QUESTOR);
	if (MON_FLAG(r_ptr, 0, MALE)) SET_FLAG(ff, 0, RF0_MALE);
	if (MON_FLAG(r_ptr, 0, FEMALE)) SET_FLAG(ff, 0, RF0_FEMALE);

	/* Assume some "creation" flags */
	if (MON_FLAG(r_ptr, 0, CHAR_MIMIC)) SET_FLAG(ff, 0, RF0_CHAR_MIMIC);
	if (MON_FLAG(r_ptr, 0, FRIENDS)) SET_FLAG(ff, 0, RF0_FRIENDS);
	if (MON_FLAG(r_ptr, 0, ESCORT)) SET_FLAG(ff, 0, RF0_ESCORT);
	if (MON_FLAG(r_ptr, 0, ESCORTS)) SET_FLAG(ff, 0, RF0_ESCORTS);

	/* Killing a monster reveals some properties */
	if (r_ptr->r_tkills || cheat_know)
	{
		/* Know "race" flags */
		if (MON_FLAG(r_ptr, 2, ORC)) SET_FLAG(ff, 2, RF2_ORC);
		if (MON_FLAG(r_ptr, 2, TROLL)) SET_FLAG(ff, 2, RF2_TROLL);
		if (MON_FLAG(r_ptr, 2, GIANT)) SET_FLAG(ff, 2, RF2_GIANT);
		if (MON_FLAG(r_ptr, 2, DRAGON)) SET_FLAG(ff, 2, RF2_DRAGON);
		if (MON_FLAG(r_ptr, 2, DEMON)) SET_FLAG(ff, 2, RF2_DEMON);
		if (MON_FLAG(r_ptr, 2, UNDEAD)) SET_FLAG(ff, 2, RF2_UNDEAD);
		if (MON_FLAG(r_ptr, 2, EVIL)) SET_FLAG(ff, 2, RF2_EVIL);
		if (MON_FLAG(r_ptr, 2, GOOD)) SET_FLAG(ff, 2, RF2_GOOD);
		if (MON_FLAG(r_ptr, 2, ANIMAL)) SET_FLAG(ff, 2, RF2_ANIMAL);
		if (MON_FLAG(r_ptr, 2, AMBERITE)) SET_FLAG(ff, 2, RF2_AMBERITE);

		/* Know 'quantum' flag */
		if (MON_FLAG(r_ptr, 1, QUANTUM)) SET_FLAG(ff, 1, RF1_QUANTUM);

		/* Know "forced" flags */
		if (MON_FLAG(r_ptr, 0, FORCE_DEPTH)) SET_FLAG(ff, 0, RF0_FORCE_DEPTH);
		if (MON_FLAG(r_ptr, 0, FORCE_MAXHP)) SET_FLAG(ff, 0, RF0_FORCE_MAXHP);
	}

	/* Treat uniques differently */
	if (RF_FLAG(ff, 0, UNIQUE))
	{
		/* Hack -- Determine if the unique is "dead" */
		bool dead = (r_ptr->max_num == 0) ? TRUE : FALSE;

		/* We've been killed... */
		if (r_ptr->r_deaths)
		{
			/* Killed ancestors */
			roff("%^s has slain %d of your ancestors",
									  wd_he[msex], r_ptr->r_deaths);

			/* But we've also killed it */
			if (dead)
			{
				roff(", but you have avenged %s!  ",
										  plural(r_ptr->r_deaths, "him",
												 "them"));
			}

			/* Unavenged (ever) */
			else
			{
				roff(", who %s unavenged.  ",
										  plural(r_ptr->r_deaths, "remains",
												 "remain"));
			}
		}

		/* Dead unique who never hurt us */
		else if (dead)
		{
			roff(CLR_L_DARK "You have slain this foe.  ");
		}
	}

	/* Not unique, but killed us */
	else if (r_ptr->r_deaths)
	{
		/* Dead ancestors */
		roff("%d of your ancestors %s been killed by this creature, ",
					  r_ptr->r_deaths, plural(r_ptr->r_deaths, "has", "have"));

		/* Some kills this life */
		if (r_ptr->r_pkills)
		{
			roff("and you have exterminated at least %d of the creatures.  ",
					r_ptr->r_pkills);
		}

		/* Some kills past lives */
		else if (r_ptr->r_tkills)
		{
			roff("and %s have exterminated at least %d of the creatures.  ",
					"your ancestors", r_ptr->r_tkills);
		}

		/* No kills */
		else
		{
			roff(CLR_RED "and %s is not ever known to have been defeated.  ",
						  wd_he[msex]);
		}
	}

	/* Normal monsters */
	else
	{
		/* Killed some this life */
		if (r_ptr->r_pkills)
		{
			roff("You have killed at least %d of these creatures.  ",
						  r_ptr->r_pkills);
		}

		/* Killed some last life */
		else if (r_ptr->r_tkills)
		{
			roff("Your ancestors have killed at least %d of these creatures.  ",
					r_ptr->r_tkills);
		}

		/* Killed none */
		else
		{
			roff("No battles to the death are recalled.  ");
		}
	}


#ifdef DELAY_LOAD_R_TEXT

	int fd;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "r_info.raw");

	/* Open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Use file */
	if (fd >= 0)
	{
		huge pos;

		/* Starting position */
		pos = r_ptr->text;

		/* Additional offsets */
		pos += r_head->head_size;
		pos += r_head->info_size;
		pos += r_head->name_size;

		/* Seek */
		(void)fd_seek(fd, pos);

		/* Read a chunk of data */
		(void)fd_read(fd, buf, 2048);

		/* Close it */
		(void)fd_close(fd);
	}

#else

	/* Simple method */
	strcpy(buf, r_text + r_ptr->text);

#endif

	/* Dump it */
	roff(buf);
	roff("  ");


	/* Nothing yet */
	old = FALSE;

	/* Describe location */
	if (r_ptr->level == 0)
	{
		roff("%^s lives in the town", wd_he[msex]);
		old = TRUE;
	}
	else if (r_ptr->r_tkills || cheat_know)
	{
		roff(CLR_SLATE "%^s is ", wd_he[msex]);

		if (r_ptr->r_tkills * r_ptr->rarity >= 30 || cheat_know)
		{
			if (r_ptr->rarity < 2)
				roff(CLR_SLATE "very common");
			else if (r_ptr->rarity < 4)
				roff(CLR_SLATE "common");
			else if (r_ptr->rarity < 8)
				roff(CLR_SLATE "uncommon");
			else if (r_ptr->rarity < 16)
				roff(CLR_SLATE "rare");
			else
				roff(CLR_SLATE "very rare");
		}
		else
		{
			roff(CLR_SLATE "normally found");
		}
		
		if (depth_in_feet)
		{
			roff(CLR_SLATE " at depths of %d feet", r_ptr->level * 50);
		}
		else
		{
			roff(CLR_SLATE " on dungeon level %d", r_ptr->level);
		}
		old = TRUE;
	}


	/* Describe movement */
	if (TRUE)
	{
		/* Introduction */
		if (old)
		{
			roff(", and ");
		}
		else
		{
			roff("%^s ", wd_he[msex]);
			old = TRUE;
		}
		roff("moves");

		/* Random-ness */
		if ((RF_FLAG(ff, 0, RAND_50)) || (RF_FLAG(ff, 0, RAND_25)))
		{
			/* Adverb */
			if ((RF_FLAG(ff, 0, RAND_50)) && (RF_FLAG(ff, 0, RAND_25)))
			{
				roff(" extremely");
			}
			else if (RF_FLAG(ff, 0, RAND_50))
			{
				roff(" somewhat");
			}
			else if (RF_FLAG(ff, 0, RAND_25))
			{
				roff(" a bit");
			}

			/* Adjective */
			roff(" erratically");

			/* Hack -- Occasional conjunction */
			if (speed != 110) roff(", and");
		}

		/* Speed */
		if (speed > 110)
		{
			if (speed > 130) roff(CLR_GREEN " incredibly");
			else if (speed > 120) roff(CLR_GREEN " very");
			roff(CLR_GREEN " quickly");
		}
		else if (speed < 110)
		{
			if (speed < 90) roff(CLR_GREEN " incredibly");
			else if (speed < 100) roff(CLR_GREEN " very");
			roff(CLR_GREEN " slowly");
		}
		else
		{
			roff(CLR_GREEN " at normal speed");
		}
	}

	/* The code above includes "attack speed" */
	if (RF_FLAG(ff, 0, NEVER_MOVE))
	{
		/* Introduce */
		if (old)
		{
			roff(", but ");
		}
		else
		{
			roff("%^s ", wd_he[msex]);
			old = TRUE;
		}

		/* Describe */
		roff("does not deign to chase intruders");
	}

	/* End this sentence */
	if (old)
	{
		roff(".  ");
		old = FALSE;
	}


	/* Describe experience if known */
	if (r_ptr->r_tkills || cheat_know)
	{
		/* Introduction */
		if (RF_FLAG(ff, 0, UNIQUE))
		{
			roff("Killing this");
		}
		else
		{
			roff("A kill of this");
		}

		/* Describe the "quality" */
		if (RF_FLAG(ff, 1, XXX_1)) roff(CLR_L_BLUE " some property");
		if (RF_FLAG(ff, 2, ANIMAL)) roff(CLR_L_BLUE " natural");
		if (RF_FLAG(ff, 2, EVIL)) roff(CLR_L_BLUE " evil");
		if (RF_FLAG(ff, 2, GOOD)) roff(CLR_L_BLUE " good");
		if (RF_FLAG(ff, 2, UNDEAD)) roff(CLR_L_BLUE " undead");

		/* Describe the "race" */
		if (RF_FLAG(ff, 2, DRAGON)) roff(CLR_L_BLUE " dragon");
		else if (RF_FLAG(ff, 2, DEMON)) roff(CLR_L_BLUE " demon");
		else if (RF_FLAG(ff, 2, GIANT)) roff(CLR_L_BLUE " giant");
		else if (RF_FLAG(ff, 2, TROLL)) roff(CLR_L_BLUE " troll");
		else if (RF_FLAG(ff, 2, ORC)) roff(CLR_L_BLUE " orc");
		else if (RF_FLAG(ff, 2, AMBERITE)) roff(CLR_L_BLUE " Amberite");
		else if (RF_FLAG(ff, 1, QUANTUM)) roff(CLR_L_BLUE " quantum creature");
		else
			roff(" creature");

		/* Group some variables */
		if (TRUE)
		{
			s32b new_exp, new_exp_frac;
			int i;

			/* Get the xp for a kill */
			exp_for_kill(r_ptr, &new_exp, &new_exp_frac);

			/* calculate the fractional exp part scaled by 100, */
			/* must use long arithmetic to avoid overflow */
			new_exp_frac =
				(((long)new_exp_frac + 0x10000L / 500) * 100) / 0x10000L;

			/* Mention the experience */
			roff(" is worth %ld.%02ld point%s",
					(long)new_exp, (long)new_exp_frac,
					(((new_exp == 1) && (new_exp_frac == 0)) ? "" : "s"));

			/* Take account of annoying English */
			p = "th";
			i = p_ptr->lev % 10;
			if ((p_ptr->lev / 10) == 1) /* nothing */ ;
			else if (i == 1) p = "st";
			else if (i == 2) p = "nd";
			else if (i == 3) p = "rd";

			/* Take account of "leading vowels" in numbers */
			q = "";
			i = p_ptr->lev;
			if ((i == 8) || (i == 11) || (i == 18)) q = "n";

			/* Mention the dependance on the player's level */
			roff(" for a%s %lu%s level character.  ", q, (long)i, p);
		}
	}

	if ((RF_FLAG(ff, 1, AURA_FIRE)) && (RF_FLAG(ff, 1, AURA_ELEC)))
	{
		roff(CLR_YELLOW "%^s is surrounded by flames and electricity.  ",
					  wd_he[msex]);
	}
	else if ((RF_FLAG(ff, 2, AURA_COLD)) && (RF_FLAG(ff, 1, AURA_ELEC)))
	{
		roff(CLR_YELLOW "%^s is surrounded by ice and electricity.  ",
					  wd_he[msex]);
	}
	else if (RF_FLAG(ff, 1, AURA_FIRE))
	{
		roff(CLR_YELLOW "%^s is surrounded by flames.  ", wd_he[msex]);
	}
	else if (RF_FLAG(ff, 2, AURA_COLD))
	{
		roff(CLR_YELLOW "%^s is surrounded by ice.  ", wd_he[msex]);
	}
	else if (RF_FLAG(ff, 1, AURA_ELEC))
	{
		roff(CLR_YELLOW "%^s is surrounded by electricity.  ", wd_he[msex]);
	}

	if (RF_FLAG(ff, 1, REFLECTING))
	{
		roff(CLR_YELLOW "%^s reflects bolt spells.  ", wd_he[msex]);
	}

	/* Describe escorts */
	if ((RF_FLAG(ff, 0, ESCORT)) || (RF_FLAG(ff, 0, ESCORTS)))
	{
		roff("%^s usually appears with escorts.  ", wd_he[msex]);
	}

	/* Describe friends */
	else if (RF_FLAG(ff, 0, FRIENDS))
	{
		roff("%^s usually appears in groups.  ", wd_he[msex]);
	}

	else if (RF_FLAG(ff, 0, CHAR_MIMIC))
	{
		roff("%^s is a mimic.  ", wd_he[msex]);
	}

	/* Collect inate attacks */
	if (RF_FLAG(ff, 3, SHRIEK)) vp[vn++] = "shriek for help";
	if (RF_FLAG(ff, 3, ELDRITCH_HORROR)) vp[vn++] = "blast your sanity";
	if (RF_FLAG(ff, 3, ROCKET)) vp[vn++] = "shoot a rocket";
	if (RF_FLAG(ff, 3, ARROW_1)) vp[vn++] = "fire an arrow";
	if (RF_FLAG(ff, 3, ARROW_2)) vp[vn++] = "fire arrows";
	if (RF_FLAG(ff, 3, ARROW_3)) vp[vn++] = "fire a missile";
	if (RF_FLAG(ff, 3, ARROW_4)) vp[vn++] = "fire missiles";

	/* Describe inate attacks */
	if (vn)
	{
		/* Intro */
		roff("%^s", wd_he[msex]);

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
			if (n == 0) roff(" may ");
			else if (n < vn - 1) roff(", ");
			else
				roff(" or ");

			/* Dump */
			roff(CLR_L_RED "%s", vp[n]);
		}

		/* End */
		roff(".  ");
	}


	/* Collect breaths */
	vn = 0;
	if (RF_FLAG(ff, 3, BR_ACID)) vp[vn++] = "acid";
	if (RF_FLAG(ff, 3, BR_ELEC)) vp[vn++] = "lightning";
	if (RF_FLAG(ff, 3, BR_FIRE)) vp[vn++] = "fire";
	if (RF_FLAG(ff, 3, BR_COLD)) vp[vn++] = "frost";
	if (RF_FLAG(ff, 3, BR_POIS)) vp[vn++] = "poison";
	if (RF_FLAG(ff, 3, BR_NETH)) vp[vn++] = "nether";
	if (RF_FLAG(ff, 3, BR_LITE)) vp[vn++] = "light";
	if (RF_FLAG(ff, 3, BR_DARK)) vp[vn++] = "darkness";
	if (RF_FLAG(ff, 3, BR_CONF)) vp[vn++] = "confusion";
	if (RF_FLAG(ff, 3, BR_SOUN)) vp[vn++] = "sound";
	if (RF_FLAG(ff, 3, BR_CHAO)) vp[vn++] = "chaos";
	if (RF_FLAG(ff, 3, BR_DISE)) vp[vn++] = "disenchantment";
	if (RF_FLAG(ff, 3, BR_NEXU)) vp[vn++] = "nexus";
	if (RF_FLAG(ff, 3, BR_TIME)) vp[vn++] = "time";
	if (RF_FLAG(ff, 3, BR_INER)) vp[vn++] = "inertia";
	if (RF_FLAG(ff, 3, BR_GRAV)) vp[vn++] = "gravity";
	if (RF_FLAG(ff, 3, BR_SHAR)) vp[vn++] = "shards";
	if (RF_FLAG(ff, 3, BR_PLAS)) vp[vn++] = "plasma";
	if (RF_FLAG(ff, 3, BR_WALL)) vp[vn++] = "force";
	if (RF_FLAG(ff, 3, BR_MANA)) vp[vn++] = "mana";
	if (RF_FLAG(ff, 3, BR_NUKE)) vp[vn++] = "toxic waste";
	if (RF_FLAG(ff, 3, BR_DISI)) vp[vn++] = "disintegration";

	/* Describe breaths */
	if (vn)
	{
		/* Note breath */
		breath = TRUE;

		/* Intro */
		roff("%^s", wd_he[msex]);

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
			if (n == 0) roff(" may breathe ");
			else if (n < vn - 1) roff(", ");
			else
				roff(" or ");

			/* Dump */
			roff(CLR_L_RED "%s", vp[n]);
		}
	}


	/* Collect spells */
	vn = 0;
	if (RF_FLAG(ff, 4, BA_ACID)) vp[vn++] = "produce acid balls";
	if (RF_FLAG(ff, 4, BA_ELEC)) vp[vn++] = "produce lightning balls";
	if (RF_FLAG(ff, 4, BA_FIRE)) vp[vn++] = "produce fire balls";
	if (RF_FLAG(ff, 4, BA_COLD)) vp[vn++] = "produce frost balls";
	if (RF_FLAG(ff, 4, BA_POIS)) vp[vn++] = "produce poison balls";
	if (RF_FLAG(ff, 4, BA_NETH)) vp[vn++] = "produce nether balls";
	if (RF_FLAG(ff, 4, BA_WATE)) vp[vn++] = "produce water balls";
	if (RF_FLAG(ff, 3, BA_NUKE)) vp[vn++] = "produce balls of radiation";
	if (RF_FLAG(ff, 4, BA_MANA)) vp[vn++] = "invoke mana storms";
	if (RF_FLAG(ff, 4, BA_DARK)) vp[vn++] = "invoke darkness storms";
	if (RF_FLAG(ff, 3, BA_CHAO)) vp[vn++] = "invoke raw Logrus";
	if (RF_FLAG(ff, 5, HAND_DOOM)) vp[vn++] = "invoke the Hand of Doom";
	if (RF_FLAG(ff, 4, DRAIN_MANA)) vp[vn++] = "drain mana";
	if (RF_FLAG(ff, 4, MIND_BLAST)) vp[vn++] = "cause mind blasting";
	if (RF_FLAG(ff, 4, BRAIN_SMASH)) vp[vn++] = "cause brain smashing";
	if (RF_FLAG(ff, 4, CAUSE_1)) vp[vn++] = "cause light wounds and cursing";
	if (RF_FLAG(ff, 4, CAUSE_2)) vp[vn++] = "cause serious wounds and cursing";
	if (RF_FLAG(ff, 4, CAUSE_3)) vp[vn++] = "cause critical wounds and cursing";
	if (RF_FLAG(ff, 4, CAUSE_4)) vp[vn++] = "cause mortal wounds";
	if (RF_FLAG(ff, 4, BO_ACID)) vp[vn++] = "produce acid bolts";
	if (RF_FLAG(ff, 4, BO_ELEC)) vp[vn++] = "produce lightning bolts";
	if (RF_FLAG(ff, 4, BO_FIRE)) vp[vn++] = "produce fire bolts";
	if (RF_FLAG(ff, 4, BO_COLD)) vp[vn++] = "produce frost bolts";
	if (RF_FLAG(ff, 4, BO_POIS)) vp[vn++] = "produce poison bolts";
	if (RF_FLAG(ff, 4, BO_NETH)) vp[vn++] = "produce nether bolts";
	if (RF_FLAG(ff, 4, BO_WATE)) vp[vn++] = "produce water bolts";
	if (RF_FLAG(ff, 4, BO_MANA)) vp[vn++] = "produce mana bolts";
	if (RF_FLAG(ff, 4, BO_PLAS)) vp[vn++] = "produce plasma bolts";
	if (RF_FLAG(ff, 4, BO_ICEE)) vp[vn++] = "produce ice bolts";
	if (RF_FLAG(ff, 4, MISSILE)) vp[vn++] = "produce magic missiles";
	if (RF_FLAG(ff, 4, SCARE)) vp[vn++] = "terrify";
	if (RF_FLAG(ff, 4, BLIND)) vp[vn++] = "blind";
	if (RF_FLAG(ff, 4, CONF)) vp[vn++] = "confuse";
	if (RF_FLAG(ff, 4, SLOW)) vp[vn++] = "slow";
	if (RF_FLAG(ff, 4, HOLD)) vp[vn++] = "paralyze";
	if (RF_FLAG(ff, 5, HASTE)) vp[vn++] = "haste-self";
	if (RF_FLAG(ff, 5, HEAL)) vp[vn++] = "heal-self";
	if (RF_FLAG(ff, 5, INVULNER)) vp[vn++] = "make invulnerable";
	if (RF_FLAG(ff, 5, BLINK)) vp[vn++] = "blink-self";
	if (RF_FLAG(ff, 5, TPORT)) vp[vn++] = "teleport-self";
	if (RF_FLAG(ff, 5, XXX3)) vp[vn++] = "do something";
	if (RF_FLAG(ff, 5, XXX4)) vp[vn++] = "do something";
	if (RF_FLAG(ff, 5, TELE_TO)) vp[vn++] = "teleport to";
	if (RF_FLAG(ff, 5, TELE_AWAY)) vp[vn++] = "teleport away";
	if (RF_FLAG(ff, 5, TELE_LEVEL)) vp[vn++] = "teleport level";
	if (RF_FLAG(ff, 5, XXX5)) vp[vn++] = "do something";
	if (RF_FLAG(ff, 5, DARKNESS)) vp[vn++] = "create darkness";
	if (RF_FLAG(ff, 5, TRAPS)) vp[vn++] = "create traps";
	if (RF_FLAG(ff, 5, FORGET)) vp[vn++] = "cause amnesia";
	if (RF_FLAG(ff, 5, RAISE_DEAD)) vp[vn++] = "raise dead";
	if (RF_FLAG(ff, 5, S_MONSTER)) vp[vn++] = "summon a monster";
	if (RF_FLAG(ff, 5, S_MONSTERS)) vp[vn++] = "summon monsters";
	if (RF_FLAG(ff, 5, S_KIN)) vp[vn++] = "summon aid";
	if (RF_FLAG(ff, 5, S_ANT)) vp[vn++] = "summon ants";
	if (RF_FLAG(ff, 5, S_SPIDER)) vp[vn++] = "summon spiders";
	if (RF_FLAG(ff, 5, S_HOUND)) vp[vn++] = "summon hounds";
	if (RF_FLAG(ff, 5, S_HYDRA)) vp[vn++] = "summon hydras";
	if (RF_FLAG(ff, 5, S_ANGEL)) vp[vn++] = "summon an angel";
	if (RF_FLAG(ff, 5, S_DEMON)) vp[vn++] = "summon a demon";
	if (RF_FLAG(ff, 5, S_UNDEAD)) vp[vn++] = "summon an undead";
	if (RF_FLAG(ff, 5, S_DRAGON)) vp[vn++] = "summon a dragon";
	if (RF_FLAG(ff, 5, S_HI_UNDEAD)) vp[vn++] = "summon Greater Undead";
	if (RF_FLAG(ff, 5, S_HI_DRAGON)) vp[vn++] = "summon Ancient Dragons";
	if (RF_FLAG(ff, 5, S_CYBER)) vp[vn++] = "summon Cyberdemons";
	if (RF_FLAG(ff, 5, S_AMBERITES)) vp[vn++] = "summon Lords of Amber";
	if (RF_FLAG(ff, 5, S_UNIQUE)) vp[vn++] = "summon Unique Monsters";

	/* Describe spells */
	if (vn)
	{
		/* Note magic */
		magic = TRUE;

		/* Intro */
		if (breath)
		{
			roff(", and is also");
		}
		else
		{
			roff("%^s is", wd_he[msex]);
		}

		/* Verb Phrase */
		roff(" magical, casting spells");

		/* Adverb */
		if (RF_FLAG(ff, 1, SMART)) roff(CLR_ORANGE " intelligently");

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
			if (n == 0) roff(" which ");
			else if (n < vn - 1) roff(", ");
			else
				roff(" or ");

			/* Dump */
			roff(CLR_L_RED "%s", vp[n]);
		}
	}


	/* End the sentence about inate/other spells */
	if (breath || magic)
	{
		/* Total casting */
		m = r_ptr->r_cast_inate + r_ptr->r_cast_spell;

		/* Average frequency */
		n = (r_ptr->freq_inate + r_ptr->freq_spell) / 2;

		/* Describe the spell frequency */
		if (m > 100)
		{
			roff("; 1 time in %d", 100 / n);
		}

		/* Guess at the frequency */
		else if (m)
		{
			n = ((n + 9) / 10) * 10;
			roff("; about 1 time in %d", 100 / n);
		}

		/* End this sentence */
		roff(".  ");
	}


	/* Describe monster "toughness" */
	if (know_armour(r_idx))
	{
		/* Armor */
		roff("%^s has an armor rating of %d", wd_he[msex], r_ptr->ac);

		/* Maximized hitpoints */
		if (RF_FLAG(ff, 0, FORCE_MAXHP))
		{
			roff(" and a life rating of %d.  ",
									  r_ptr->hdice * r_ptr->hside);
		}

		/* Variable hitpoints */
		else
		{
			roff(" and a life rating of %dd%d.  ",
									  r_ptr->hdice, r_ptr->hside);
		}
	}



	/* Collect special abilities. */
	vn = 0;
	if (RF_FLAG(ff, 1, OPEN_DOOR)) vp[vn++] = "open doors";
	if (RF_FLAG(ff, 1, BASH_DOOR)) vp[vn++] = "bash down doors";
	if (RF_FLAG(ff, 1, PASS_WALL)) vp[vn++] = "pass through walls";
	if (RF_FLAG(ff, 1, KILL_WALL)) vp[vn++] = "bore through walls";
	if (RF_FLAG(ff, 1, MOVE_BODY)) vp[vn++] = "push past weaker monsters";
	if (RF_FLAG(ff, 1, KILL_BODY)) vp[vn++] = "destroy weaker monsters";
	if (RF_FLAG(ff, 1, TAKE_ITEM)) vp[vn++] = "pick up objects";
	if (RF_FLAG(ff, 1, KILL_ITEM)) vp[vn++] = "destroy objects";
	if (RF_FLAG(ff, 6, LITE_1) || RF_FLAG(ff, 6, LITE_2)) vp[vn++] = "light the dungeon";

	/* Describe special abilities. */
	if (vn)
	{
		/* Intro */
		roff("%^s", wd_he[msex]);

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
			if (n == 0) roff(" can ");
			else if (n < vn - 1) roff(", ");
			else
				roff(" and ");

			/* Dump */
			roff(CLR_L_UMBER "%s", vp[n]);
		}

		/* End */
		roff(".  ");
	}


	/* Describe special abilities. */
	if (RF_FLAG(ff, 1, INVISIBLE))
	{
		roff(CLR_L_BLUE "%^s is invisible.  ", wd_he[msex]);
	}
	if (RF_FLAG(ff, 1, COLD_BLOOD))
	{
		roff("%^s is cold blooded.  ", wd_he[msex]);
	}
	if (RF_FLAG(ff, 1, EMPTY_MIND))
	{
		roff("%^s is not detected by telepathy.  ", wd_he[msex]);
	}
	if (RF_FLAG(ff, 1, WEIRD_MIND))
	{
		roff("%^s is rarely detected by telepathy.  ", wd_he[msex]);
	}
	if (RF_FLAG(ff, 1, MULTIPLY))
	{
		roff(CLR_L_UMBER "%^s breeds explosively.  ", wd_he[msex]);
	}
	if (RF_FLAG(ff, 1, REGENERATE))
	{
		roff("%^s regenerates quickly.  ", wd_he[msex]);
	}


	/* Collect susceptibilities */
	vn = 0;
	if (RF_FLAG(ff, 2, HURT_ROCK)) vp[vn++] = "rock remover";
	if (RF_FLAG(ff, 2, HURT_LITE)) vp[vn++] = "bright light";
	if (RF_FLAG(ff, 2, HURT_FIRE)) vp[vn++] = "fire";
	if (RF_FLAG(ff, 2, HURT_COLD)) vp[vn++] = "cold";

	/* Describe susceptibilities */
	if (vn)
	{
		/* Intro */
		roff("%^s", wd_he[msex]);

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
			if (n == 0) roff(" is hurt by ");
			else if (n < vn - 1) roff(", ");
			else
				roff(" and ");

			/* Dump */
			roff(CLR_YELLOW "%s", vp[n]);
		}

		/* End */
		roff(".  ");
	}


	/* Collect immunities */
	vn = 0;
	if (RF_FLAG(ff, 2, IM_ACID)) vp[vn++] = "acid";
	if (RF_FLAG(ff, 2, IM_ELEC)) vp[vn++] = "lightning";
	if (RF_FLAG(ff, 2, IM_FIRE)) vp[vn++] = "fire";
	if (RF_FLAG(ff, 2, IM_COLD)) vp[vn++] = "cold";
	if (RF_FLAG(ff, 2, IM_POIS)) vp[vn++] = "poison";

	/* Describe immunities */
	if (vn)
	{
		/* Intro */
		roff("%^s", wd_he[msex]);

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
			if (n == 0) roff(" resists ");
			else if (n < vn - 1) roff(", ");
			else
				roff(" and ");

			/* Dump */
			roff(CLR_ORANGE "%s", vp[n]);
		}

		/* End */
		roff(".  ");
	}


	/* Collect resistances */
	vn = 0;
	if (RF_FLAG(ff, 2, RES_NETH)) vp[vn++] = "nether";
	if (RF_FLAG(ff, 2, RES_WATE)) vp[vn++] = "water";
	if (RF_FLAG(ff, 2, RES_PLAS)) vp[vn++] = "plasma";
	if (RF_FLAG(ff, 2, RES_NEXU)) vp[vn++] = "nexus";
	if (RF_FLAG(ff, 2, RES_DISE)) vp[vn++] = "disenchantment";
	if ((RF_FLAG(ff, 2, RES_TELE))
		&& !(MON_FLAG(r_ptr, 0, UNIQUE))) vp[vn++] = "teleportation";

	/* Describe resistances */
	if (vn)
	{
		/* Intro */
		roff("%^s", wd_he[msex]);

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
			if (n == 0) roff(" resists ");
			else if (n < vn - 1) roff(", ");
			else
				roff(" and ");

			/* Dump */
			roff(CLR_ORANGE "%s", vp[n]);
		}

		/* End */
		roff(".  ");
	}


	/* Collect non-effects */
	vn = 0;
	if (RF_FLAG(ff, 2, NO_STUN)) vp[vn++] = "stunned";
	if (RF_FLAG(ff, 2, NO_FEAR)) vp[vn++] = "frightened";
	if (RF_FLAG(ff, 2, NO_CONF)) vp[vn++] = "confused";
	if (RF_FLAG(ff, 2, NO_SLEEP)) vp[vn++] = "slept";
	if ((RF_FLAG(ff, 2, RES_TELE))
		&& (MON_FLAG(r_ptr, 0, UNIQUE))) vp[vn++] = "teleported";

	/* Describe non-effects */
	if (vn)
	{
		/* Intro */
		roff("%^s", wd_he[msex]);

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
			if (n == 0) roff(" cannot be ");
			else if (n < vn - 1) roff(", ");
			else
				roff(" or ");

			/* Dump */
			roff(CLR_YELLOW "%s", vp[n]);
		}

		/* End */
		roff(".  ");
	}


	/* Do we know how aware it is? */
	if ((((int)r_ptr->r_wake * (int)r_ptr->r_wake) > r_ptr->sleep) ||
		(r_ptr->r_ignore == MAX_UCHAR) ||
		((r_ptr->sleep == 0) && ((r_ptr->r_tkills >= 10) || cheat_know)))
	{
		cptr act;

		if (r_ptr->sleep > 200)
		{
			act = "prefers to ignore";
		}
		else if (r_ptr->sleep > 95)
		{
			act = "pays very little attention to";
		}
		else if (r_ptr->sleep > 75)
		{
			act = "pays little attention to";
		}
		else if (r_ptr->sleep > 45)
		{
			act = "tends to overlook";
		}
		else if (r_ptr->sleep > 25)
		{
			act = "takes quite a while to see";
		}
		else if (r_ptr->sleep > 10)
		{
			act = "takes a while to see";
		}
		else if (r_ptr->sleep > 5)
		{
			act = "is fairly observant of";
		}
		else if (r_ptr->sleep > 3)
		{
			act = "is observant of";
		}
		else if (r_ptr->sleep > 1)
		{
			act = "is very observant of";
		}
		else if (r_ptr->sleep > 0)
		{
			act = "is vigilant for";
		}
		else
		{
			act = "is ever vigilant for";
		}

		roff("%^s %s intruders, which %s may notice from %d feet.  ",
					  wd_he[msex], act, wd_he[msex], 10 * r_ptr->aaf);
	}


	/* Drops gold and/or items */
	if (r_ptr->r_drop_gold || r_ptr->r_drop_item)
	{
		/* No "n" needed */
		sin = FALSE;

		/* Intro */
		roff("%^s may carry", wd_he[msex]);

		/* Count maximum drop */
		n = MAX(r_ptr->r_drop_gold, r_ptr->r_drop_item);

		/* One drop (may need an "n") */
		if (n == 1)
		{
			roff(" a");
			sin = TRUE;
		}

		/* Two drops */
		else if (n == 2)
		{
			roff(" one or two");
		}

		/* Many drops */
		else
		{
			roff(" up to %d", n);
		}


		/* Great */
		if (RF_FLAG(ff, 0, DROP_GREAT))
		{
			p = " exceptional";
		}

		/* Good (no "n" needed) */
		else if (RF_FLAG(ff, 0, DROP_GOOD))
		{
			p = " good";
			sin = FALSE;
		}

		/* Okay */
		else
		{
			p = NULL;
		}


		/* Objects */
		if (r_ptr->r_drop_item)
		{
			/* Handle singular "an" */
			if (sin) roff("n");
			sin = FALSE;

			/* Dump "object(s)" */
			if (p) roff(p);
			roff(" object");
			if (n != 1) roff("s");

			/* Conjunction replaces variety, if needed for "gold" below */
			p = " or";
		}

		/* Treasures */
		if (r_ptr->r_drop_gold)
		{
			/* Cancel prefix */
			if (!p) sin = FALSE;

			/* Handle singular "an" */
			if (sin) roff("n");
			sin = FALSE;

			/* Dump "treasure(s)" */
			if (p) roff(p);
			roff(" treasure");
			if (n != 1) roff("s");
		}

		/* End this sentence */
		roff(".  ");
	}


	/* Count the number of "known" attacks */
	for (n = 0, m = 0; m < 4; m++)
	{
		/* Skip non-attacks */
		if (!r_ptr->blow[m].method) continue;

		/* Count known attacks */
		if (r_ptr->r_blows[m]) n++;
	}

	/* Examine (and count) the actual attacks */
	for (r = 0, m = 0; m < 4; m++)
	{
		int method, effect, d1, d2;

		/* Skip non-attacks */
		if (!r_ptr->blow[m].method) continue;

		/* Skip unknown attacks */
		if (!r_ptr->r_blows[m]) continue;


		/* Extract the attack info */
		method = r_ptr->blow[m].method;
		effect = r_ptr->blow[m].effect;
		d1 = r_ptr->blow[m].d_dice;
		d2 = r_ptr->blow[m].d_side;

		/* Acquire the method */
		p = rbm_info[method].name;

		/* Default effect */
		q = NULL;

		/* Acquire the effect */
		switch (effect)
		{
			case RBE_HURT:
			{
				q = "attack";
				break;
			}
			case RBE_POISON:
			{
				q = "poison";
				break;
			}
			case RBE_UN_BONUS:
			{
				q = "disenchant";
				break;
			}
			case RBE_UN_POWER:
			{
				q = "drain charges";
				break;
			}
			case RBE_EAT_GOLD:
			{
				q = "steal gold";
				break;
			}
			case RBE_EAT_ITEM:
			{
				q = "steal items";
				break;
			}
			case RBE_EAT_FOOD:
			{
				q = "eat your food";
				break;
			}
			case RBE_EAT_LITE:
			{
				q = "absorb light";
				break;
			}
			case RBE_ACID:
			{
				q = "shoot acid";
				break;
			}
			case RBE_ELEC:
			{
				q = "electrocute";
				break;
			}
			case RBE_FIRE:
			{
				q = "burn";
				break;
			}
			case RBE_COLD:
			{
				q = "freeze";
				break;
			}
			case RBE_BLIND:
			{
				q = "blind";
				break;
			}
			case RBE_CONFUSE:
			{
				q = "confuse";
				break;
			}
			case RBE_TERRIFY:
			{
				q = "terrify";
				break;
			}
			case RBE_PARALYZE:
			{
				q = "paralyze";
				break;
			}
			case RBE_LOSE_STR:
			{
				q = "reduce strength";
				break;
			}
			case RBE_LOSE_INT:
			{
				q = "reduce intelligence";
				break;
			}
			case RBE_LOSE_WIS:
			{
				q = "reduce wisdom";
				break;
			}
			case RBE_LOSE_DEX:
			{
				q = "reduce dexterity";
				break;
			}
			case RBE_LOSE_CON:
			{
				q = "reduce constitution";
				break;
			}
			case RBE_LOSE_CHR:
			{
				q = "reduce charisma";
				break;
			}
			case RBE_LOSE_ALL:
			{
				q = "reduce all stats";
				break;
			}
			case RBE_SHATTER:
			{
				q = "shatter";
				break;
			}
			case RBE_EXP_10:
			{
				q = "lower experience (by 10d6+)";
				break;
			}
			case RBE_EXP_20:
			{
				q = "lower experience (by 20d6+)";
				break;
			}
			case RBE_EXP_40:
			{
				q = "lower experience (by 40d6+)";
				break;
			}
			case RBE_EXP_80:
			{
				q = "lower experience (by 80d6+)";
				break;
			}
			case RBE_DISEASE:
			{
				q = "disease";
				break;
			}
			case RBE_TIME:
			{
				q = "distrupt the time continuum";
				break;
			}
			case RBE_EXP_VAMP:
			{
				q = "drain life force";
				break;
			}
		}


		/* Introduce the attack description */
		if (!r)
		{
			roff("%^s can ", wd_he[msex]);
		}
		else if (r < n - 1)
		{
			roff(", ");
		}
		else
		{
			roff(", and ");
		}


		/* Hack -- force a method */
		if (!p) p = "do something weird";

		/* Describe the method */
		roff(p);


		/* Describe the effect (if any) */
		if (q)
		{
			/* Describe the attack type */
			roff(" to ");
			roff(CLR_L_RED "%s", q);

			/* Describe damage (if known) */
			if (d1 && d2 && know_damage(r_idx, m))
			{
				/* Display the damage */
				roff(" with damage");
				roff(" %dd%d", d1, d2);
			}
		}


		/* Count the attacks as printed */
		r++;
	}

	/* Finish sentence above */
	if (r)
	{
		roff(".  ");
	}

	/* Notice lack of attacks */
	else if (RF_FLAG(ff, 0, NEVER_BLOW))
	{
		roff("%^s has no physical attacks.  ", wd_he[msex]);
	}

	/* Or describe the lack of knowledge */
	else
	{
		roff("Nothing is known about %s attack.  ", wd_his[msex]);
	}


	/*
	 * Notice "Quest" monsters, but only if you
	 * already encountered the monster.
	 */
	if ((RF_FLAG(ff, 0, QUESTOR)) && (r_ptr->r_sights))
	{
		roff("You feel an intense desire to kill this monster...  ");
	}


	/* All done */
	roff("\n");

	/* Cheat -- know everything */
	if ((cheat_know) && (remem == 0))
	{
		/* Hack -- restore memory */
		COPY(r_ptr, &save_mem, monster_race);
	}
}



/*
 * Hack -- Display the "name" and "attr/chars" of a monster race
 */
void roff_mon_top(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	byte a1, a2;
	char c1, c2;


	/* Access the chars */
	c1 = r_ptr->d_char;
	c2 = r_ptr->x_char;

	/* Access the attrs */
	a1 = r_ptr->d_attr;
	a2 = r_ptr->x_attr;

	/* Hack -- fake monochrome */
	if (!use_color)
	{
		a1 = TERM_WHITE;
		a2 = TERM_WHITE;
	}


	/* Clear the top line */
	clear_msg();
    
	/* Reset the cursor */
	Term_gotoxy(0, 0);

	/* A title (use "The" for non-uniques) */
	if (!(MON_FLAG(r_ptr, 0, UNIQUE)))
	{
		roff("The ");
	}

	/* Dump the name */
	roff(r_name + r_ptr->name);

	/* Append the "standard" attr/char info */
	roff(" ('");
	Term_addch(a1, c1);
	roff("')");

	/* Append the "optional" attr/char info */
	roff("/('");
	Term_addch(a2, c2);
	roff("'):");

	/* Wizards get extra info */
	if (p_ptr->state.wizard)
	{
		roff(" (" CLR_L_BLUE "%d)", r_idx);
	}
}



/*
 * Hack -- describe the given monster race at the top of the screen
 */
void screen_roff_mon(int r_idx, int remember)
{
	/* Flush messages */
	message_flush();

	/* Begin recall */
	clear_row(1);

	/* Recall monster */
	roff_mon_aux(r_idx, remember);

	/* Describe monster */
	roff_mon_top(r_idx);
}




/*
 * Hack -- describe the given monster race in the current "term" window
 */
void display_roff_mon(int r_idx)
{
	/* Erase the window */
    clear_from(0);

	/* Begin recall */
	Term_gotoxy(0, 1);

	/* Recall monster */
	roff_mon_aux(r_idx, 0);

	/* Describe monster */
	roff_mon_top(r_idx);
}


/*
 * Hack -- show a list of the visible monsters in the current "term" window
 */
void display_visible(void)
{
	int i, y;
	char c1, c2;
	byte a1, a2;
	monster_race *r_ptr;

	/* Erase the window */
    clear_from(0);

	/* Are we hallucinating? */
	if (p_ptr->tim.image)
	{
		put_fstr(0, 10, CLR_VIOLET "Hallucinations");

		return;
	}

	i = p_ptr->max_seen_r_idx;

	/* Show the list */
	for (y = 0; y < Term->hgt; y++)
	{
		/* No more to display */
		if (!i) return;

		/* Go to left of screen */
		Term_gotoxy(0, y);

		/* Note we have assumed that r_info.txt has been sorted */

		/* Access monster */
		r_ptr = &r_info[i];

		/* Access the chars */
		c1 = r_ptr->d_char;
		c2 = r_ptr->x_char;

		/* Access the attrs */
		a1 = r_ptr->d_attr;
		a2 = r_ptr->x_attr;

		/* Hack -- fake monochrome */
		if (!use_color)
		{
			a1 = TERM_WHITE;
			a2 = TERM_WHITE;
		}

		/* Dump the name */
		if (MON_FLAG(r_ptr, 0, UNIQUE))
		{
			roff(CLR_L_BLUE "%s", (r_name + r_ptr->name));
		}
		else if (MON_FLAG(r_ptr, 0, QUESTOR))
		{
			roff(CLR_L_RED "%s", (r_name + r_ptr->name));
		}
		else
		{
			roff("%s", r_name + r_ptr->name);
		}

		/* Append the "standard" attr/char info */
		roff(" ('");
		Term_addch(a1, c1);
		roff("')");

		/* Append the "optional" attr/char info */
		roff("/('");
		Term_addch(a2, c2);
		roff("'):");

		/* Wizards get extra info */
		if (p_ptr->state.wizard)
		{
			roff(" (" CLR_L_BLUE "%d)", i);
		}

		/* Append count */
		roff("[%d]", r_ptr->r_see);

		/* Look for the next one */
		while (i > 0)
		{
			i--;

			if (r_info[i].r_see)
			{
				break;
			}
		}
	}
}


void set_friendly(monster_type *m_ptr)
{
	m_ptr->smart |= SM_FRIENDLY;
}


void set_pet(monster_type *m_ptr)
{
	m_ptr->smart |= SM_PET;
}


/*
 * Makes the monster hostile towards the player
 */
void set_hostile(monster_type *m_ptr)
{
	m_ptr->smart &= ~SM_PET;
	m_ptr->smart &= ~SM_FRIENDLY;
}


/*
 * Anger the monster
 */
void anger_monster(monster_type *m_ptr)
{
	if (!is_hostile(m_ptr))
	{
		msgf("%^v gets angry!", MONSTER_FMT(m_ptr, 0));
		set_hostile(m_ptr);

		chg_virtue(V_INDIVIDUALISM, 1);
		chg_virtue(V_HONOUR, -1);
		chg_virtue(V_JUSTICE, -1);
		chg_virtue(V_COMPASSION, -1);
	}
}


/*
 * Check if two monsters are enemies
 */
bool are_enemies(const monster_type *m_ptr, const monster_type *n_ptr)
{
	const monster_race *r_ptr = &r_info[m_ptr->r_idx];
	const monster_race *s_ptr = &r_info[n_ptr->r_idx];

	/* Friendly vs. opposite aligned normal or pet */
	if (((MON_FLAG(r_ptr, 2, EVIL)) &&
		 (MON_FLAG(s_ptr, 2, GOOD))) ||
		((MON_FLAG(r_ptr, 2, GOOD)) && (MON_FLAG(s_ptr, 2, EVIL))))
	{
		return TRUE;
	}

	/* Hostile vs. non-hostile */
	if (is_hostile(m_ptr) != is_hostile(n_ptr))
	{
		return TRUE;
	}

	/* Default */
	return FALSE;
}


/*
 * Is the monster "alive"?
 *
 * Used to determine the message to print for a killed monster.
 * ("dies", "destroyed")
 */
bool monster_living(const monster_race *r_ptr)
{
	/* Non-living, undead, or demon */
	if (MON_FLAG(r_ptr, 2, DEMON) || MON_FLAG(r_ptr, 2, UNDEAD) ||
		MON_FLAG(r_ptr, 2, NONLIVING))
		return FALSE;
	else
		return TRUE;
}

