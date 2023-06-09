/* CVS: Last edit by $Author$ on $Date$ */
/* File: mind.c */

/* Purpose: Mindcrafter code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


#include "tnb.h" /* TNB */


mindcraft_power mindcraft_powers[MAX_MINDCRAFT_POWERS] = {
	/* Level gained,  cost,  %fail,  name */
	{1, 1, 15, "Precognition"}
	, /* Det. monsters/traps */
	{2, 1, 20, "Neural Blast"}
	, /* ~MM */
	{3, 2, 25, "Minor Displacement"}
	, /* Phase door */
	{7, 6, 35, "Major Displacement"}
	, /* Tele. Self / All */
	{9, 7, 50, "Domination"}
	,
	{11, 7, 30, "Pulverise"}
	, /* Telekinetic "bolt" */
	{13, 12, 50, "Character Armour"}
	, /* Psychic/physical defenses */
	{15, 12, 60, "Psychometry"}
	,
	{18, 10, 45, "Mind Wave"}
	, /* Ball -> LOS */
	{23, 15, 50, "Adrenaline Channeling"}
	,
	{25, 10, 40, "Psychic Drain"}
	, /* Convert enemy HP to mana */
	{28, 20, 45, "Telekinetic Wave"}
	, /* Ball -> LOS */
};


void mindcraft_info(char *p, int power)
{
	int plev = p_ptr->lev;

	strcpy(p, "");

	switch (power)
	{
		case 0:
			break;
		case 1:
			sprintf(p, " dam %dd%d", 3 + ((plev - 1) / 4), 3 + plev / 15);
			break;
		case 2:
			sprintf(p, " range %d", (plev < 25 ? 10 : plev + 2));
			break;
		case 3:
			sprintf(p, " range %d", plev * 5);
			break;
		case 4:
			break;
		case 5:
			sprintf(p, " dam %dd8", 8 + ((plev - 5) / 4));
			break;
		case 6:
			sprintf(p, " dur %d", plev);
			break;
		case 7:
			break;
		case 8:
			sprintf(p, " dam %d", plev * ((plev - 5) / 10 + 1));
			break;
		case 9:
			sprintf(p, " dur 11-%d", plev + plev / 2);
			break;
		case 10:
			sprintf(p, " dam %dd6", plev / 2);
			break;
		case 11:
			sprintf(p, " dam %d", plev * (plev > 39 ? 4 : 3));
			break;
	}
}


/*
 * Allow user to choose a mindcrafter power.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 *
 * nb: This function has a (trivial) display bug which will be obvious
 * when you run it. It's probably easy to fix but I haven't tried,
 * sorry.
 */
static int get_mindcraft_power(int *sn)
{
	int i;
	int num = 0;
#if 0 /* TNB */
	int y = 1;
	int x = 20;
	int minfail = 0;
#endif /* TNB */
	int plev = p_ptr->lev;
/*	int chance = 0; -- TNB */
	int ask;
	char choice;
	char out_val[160];
/*	char comment[80]; -- TNB */
	cptr p = "power";
	mindcraft_power spell;
	bool flag, redraw;

	/* Assume cancelled */
	*sn = (-1);

#ifdef ALLOW_REPEAT	/* TNB */

	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Verify the spell */
		if (mindcraft_powers[*sn].min_lev <= plev)
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	for (i = 0; i < MAX_MINDCRAFT_POWERS; i++)
	{
		if (mindcraft_powers[i].min_lev <= plev)
		{
			num++;
		}
	}

	/* Build a prompt (accept all spells) */
	(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
		p, I2A(0), I2A(num - 1), p);

#if 1 /* TNB */

	Bind_Choose(KEYWORD_CHOOSE_MINDCRAFT + 1, 0, TRUE);

	while (!flag)
	{
		bool okay = FALSE;

		inkey_flags = (INKEY_MINDCRAFT);

		okay = get_com(out_val, &choice);

		inkey_flags = 0;
		if (!okay)
			break;

#else /* TNB */

	/* Get a spell from the user */
	while (!flag && get_com(out_val, &choice))
	{

#endif /* TNB */

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			/* Show the list */
			if (!redraw)
			{
#if 1 /* TNB */

				/* Show list */
				redraw = TRUE;

				angtk_eval("angband_display", "mindcraft", "show", NULL);

#else /* TNB */

				char psi_desc[80];

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str("Name", y, x + 5);
				put_str("Lv Mana Fail Info", y, x + 35);

				/* Dump the spells */
				for (i = 0; i < MAX_MINDCRAFT_POWERS; i++)
				{
					/* Access the spell */
					spell = mindcraft_powers[i];
					if (spell.min_lev > plev)
						break;

					chance = spell.fail;

					/* Reduce failure rate by "effective" level adjustment */
					chance -= 3 * (plev - spell.min_lev);

					/* Reduce failure rate by INT/WIS adjustment */
					chance -=
						3 *
						(adj_mag_stat[p_ptr->stat_ind[mp_ptr->
					  spell_stat]] - 1);

					/* Not enough mana to cast */
					if (spell.mana_cost > p_ptr->csp)
					{
						chance += 5 * (spell.mana_cost - p_ptr->csp);
					}

					/* Extract the minimum failure rate */
					minfail =
						adj_mag_fail[p_ptr->stat_ind[mp_ptr->spell_stat]];

					/* Minimum failure rate */
					if (chance < minfail)
						chance = minfail;

					/* Stunning makes spells harder */
					if (p_ptr->stun > 50)
						chance += 25;
					else if (p_ptr->stun)
						chance += 15;

					/* Always a 5 percent chance of working */
					if (chance > 95)
						chance = 95;

					/* Get info */
					mindcraft_info(comment, i);

					/* Dump the spell --(-- */
					sprintf(psi_desc, "  %c) %-30s%2d %4d %3d%%%s", I2A(i),
						spell.name, spell.min_lev, spell.mana_cost, chance,
						comment);
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);

#endif /* TNB */
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

#if 1 /* TNB */

				angtk_eval("angband_display", "mindcraft", "hide", NULL); /* TNB */

#else /* TNB */

				/* Restore the screen */
				screen_load();

#endif /* TNB */
			}

			/* Redo asking */
			continue;
		}

		/* Note verify */
		ask = isupper(choice);

		/* Lowercase */
		if (ask)
			choice = tolower(choice);

		/* Extract request */
		i = (islower(choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = mindcraft_powers[i];

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
			(void) strnfmt(tmp_val, 78, "Use %s? ",
				mindcraft_powers[i].name);

			/* Belay that order */
			if (!get_check(tmp_val))
				continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	Bind_Choose(KEYWORD_CHOOSE_MINDCRAFT + 1, 0, FALSE); /* TNB */

#if 1 /* TNB */

	if (redraw)
		angtk_eval("angband_display", "mindcraft", "hide", NULL); /* TNB */

#else /* TNB */

	/* Restore the screen */
	if (redraw)
		screen_load();

	/* Show choices */
	if (show_choices)
	{
		/* Update */
		p_ptr->window |= (PW_SPELL);

		/* Window stuff */
		window_stuff();
	}

#endif /* TNB */

	/* Abort if needed */
	if (!flag)
		return (FALSE);

	/* Save the choice */
	(*sn) = i;

#ifdef ALLOW_REPEAT	/* TNB */

	repeat_push(*sn);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
static bool cast_mindcrafter_spell(int spell)
{
	int b = 0;
	int dir;
	int plev = p_ptr->lev;


	/* spell code */
	switch (spell)
	{
		case 0: /* Precog */
			if (plev > 44)
				wiz_lite();
			else if (plev > 19)
				map_area();

			if (plev < 30)
			{
				b = detect_monsters_normal();
				if (plev > 14)
					b |= detect_monsters_invis();
				if (plev > 4)
				{
					b |= detect_traps();
					b |= detect_doors();
				}
			}
			else
			{
				b = detect_all();
			}

			if ((plev > 24) && (plev < 40))
				set_tim_esp(p_ptr->tim_esp + plev);

			if (!b)
				msg_print("You feel safe.");
			break;
		case 1:
			/* Mindblast */
			if (!get_aim_dir(&dir))
				return FALSE;

			if (randint(100) < plev * 2)
				fire_beam(GF_PSI, dir, damroll(3 + ((plev - 1) / 4),
						(3 + plev / 15)));
			else
				fire_ball(GF_PSI, dir, damroll(3 + ((plev - 1) / 4),
						(3 + plev / 15)), 0);
			break;
		case 2:
			/* Minor displace */
			if (plev < 40)
			{
				teleport_player(10);
			}
			else
			{
				msg_print
					("You open a dimensional gate. Choose a destination.");
				return dimension_door();
			}
			break;
		case 3:
			/* Major displace */
			if (plev > 29)
				banish_monsters(plev);
			teleport_player(plev * 5);
			break;
		case 4:
			/* Domination */
			if (plev < 30)
			{
				if (!get_aim_dir(&dir))
					return FALSE;

				fire_ball(GF_DOMINATION, dir, plev, 0);
			}
			else
			{
				charm_monsters(plev * 2);
			}
			break;
		case 5:
			/* Fist of Force  ---  not 'true' TK  */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_SOUND, dir, damroll(8 + ((plev - 5) / 4), 8),
				(plev > 20 ? (plev - 20) / 8 + 1 : 0));
			break;
		case 6:
			/* Character Armour */
			set_shield(p_ptr->shield + plev);
			if (plev > 14)
				set_oppose_acid(p_ptr->oppose_acid + plev);
			if (plev > 19)
				set_oppose_fire(p_ptr->oppose_fire + plev);
			if (plev > 24)
				set_oppose_cold(p_ptr->oppose_cold + plev);
			if (plev > 29)
				set_oppose_elec(p_ptr->oppose_elec + plev);
			if (plev > 34)
				set_oppose_pois(p_ptr->oppose_pois + plev);
			break;
		case 7:
			/* Psychometry */
			if (plev < 25)
				return psychometry();
			else
				return ident_spell();
			break;
		case 8:
			/* Mindwave */
			msg_print("Mind-warping forces emanate from your brain!");
			if (plev < 25)
				project(0, 2 + plev / 10, py, px, (plev * 3) / 2, GF_PSI,
					PROJECT_KILL);
			else
				(void) mindblast_monsters(plev * ((plev - 5) / 10 + 1));
			break;
		case 9:
			/* Adrenaline */
			set_afraid(0);
			set_stun(0);

			/*
			 * Only heal when Adrenalin Channeling is not active. We check
			 * that by checking if the player isn't fast and 'heroed' atm.
			 */
			if (!p_ptr->fast || !(p_ptr->hero || p_ptr->shero))
			{
				hp_player(plev);
			}

			b = 10 + randint((plev * 3) / 2);
			if (plev < 35)
				set_hero(p_ptr->hero + b);
			else
				set_shero(p_ptr->shero + b);

			if (!p_ptr->fast)
			{
				/* Haste */
				(void) set_fast(b);
			}
			else
			{
				(void) set_fast(p_ptr->fast + b);
			}
			break;
		case 10:
			/* Psychic Drain */
			if (!get_aim_dir(&dir))
				return FALSE;

			b = damroll(plev / 2, 6);

			/* This is always a radius-0 ball now */
			if (fire_ball(GF_PSI_DRAIN, dir, b, 0))
				p_ptr->energy -= randint(150);
			break;
		case 11:
			/* Telekinesis */
			msg_print
				("A wave of pure physical force radiates out from your body!");
			project(0, 3 + plev / 10, py, px, plev * (plev > 39 ? 4 : 3),
				GF_TELEKINESIS,
				PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID);
			break;
		default:
			msg_print("Zap?");
	}

	return TRUE;
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
void do_cmd_mindcraft(void)
{
	int n = 0;
	int chance;
	int minfail = 0;
	int plev = p_ptr->lev;
	int old_csp = p_ptr->csp;
	mindcraft_power spell;
	bool cast;


	/* not if confused */
	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		return;
	}

	/* get power */
	if (!get_mindcraft_power(&n))
		return;

	spell = mindcraft_powers[n];

	/* Verify "dangerous" spells */
	if (spell.mana_cost > p_ptr->csp)
	{
		/* Warning */
		msg_print("You do not have enough mana to use this power.");

		/* Verify */
		if (!get_check("Attempt it anyway? "))
			return;
	}

	/* Spell failure chance */
	chance = spell.fail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (plev - spell.min_lev);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[mp_ptr->spell_stat]] - 1);

	/* Not enough mana to cast */
	if (spell.mana_cost > p_ptr->csp)
	{
		chance += 5 * (spell.mana_cost - p_ptr->csp);
	}

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[p_ptr->stat_ind[mp_ptr->spell_stat]];

	/* Minimum failure rate */
	if (chance < minfail)
		chance = minfail;

	/* Stunning makes spells harder */
	if (p_ptr->stun > 50)
		chance += 25;
	else if (p_ptr->stun)
		chance += 15;

	/* Always a 5 percent chance of working */
	if (chance > 95)
		chance = 95;

	/* Failed spell */
	if (rand_int(100) < chance)
	{
		if (flush_failure)
			flush();
		msg_format("You failed to concentrate hard enough!");

		if (randint(100) < (chance / 2))
		{
			/* Backfire */
			int b = randint(100);
			if (b < 5)
			{
				msg_print("Oh, no! Your mind has gone blank!");
				lose_all_info();
			}
			else if (b < 15)
			{
				msg_print
					("Weird visions seem to dance before your eyes...");
				set_image(p_ptr->image + 5 + randint(10));
			}
			else if (b < 45)
			{
				msg_print("Your brain is addled!");
				set_confused(p_ptr->confused + randint(8));
			}
			else if (b < 90)
			{
				set_stun(p_ptr->stun + randint(8));
			}
			else
			{
				/* Mana storm */
				msg_print
					("Your mind unleashes its power in an uncontrollable storm!");
				project(1, 2 + plev / 10, py, px, plev * 2, GF_MANA,
					PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID |
					PROJECT_ITEM);
				p_ptr->csp = MAX(0, p_ptr->csp - plev * MAX(1, plev / 10));
			}
		}
	}
	else
	{
		/* Cast the spell */
		cast = cast_mindcrafter_spell(n);

		if (!cast)
			return;
	}

	/* Take a turn */
	energy_use = 100;

	/* Sufficient mana */
	if (spell.mana_cost <= old_csp)
	{
		/* Use some mana */
		p_ptr->csp -= spell.mana_cost;

		/* Limit */
		if (p_ptr->csp < 0)
			p_ptr->csp = 0;
	}

	/* Over-exert the player */
	else
	{
		int oops = spell.mana_cost - old_csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
		msg_print("You faint from the effort!");

		/* Hack -- Bypass free action */
		(void) set_paralyzed(p_ptr->paralyzed + randint(5 * oops + 1));

		/* Damage WIS (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
			msg_print("You have damaged your mind!");

			/* Reduce constitution */
			(void) dec_stat(A_WIS, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);
}
