/* CVS: Last edit by $Author$ on $Date$ */
/* File: cmd5.c */

/* Purpose: Spell/Prayer commands */

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
 * Allow user to choose a spell/prayer from the given book.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 */
static int get_spell(int *sn, cptr prompt, int k_idx /* sval -- TNB */
	, bool known, bool realm_2)
{
	int i;
	int spell = -1;
	int num = 0;
	int ask;
	byte spells[64];
	bool flag, redraw, okay;
	char choice;
	magic_type *s_ptr;
	char out_val[160];
	int use_realm = (realm_2 ? p_ptr->realm2 : p_ptr->realm1);
	cptr p = ((mp_ptr->spell_book == TV_LIFE_BOOK) ? "prayer" : "spell");
	int sval = k_info[k_idx].sval; /* TNB */

#ifdef ALLOW_REPEAT	/* TNB */

	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Verify the spell is in this book */
		if ((fake_spell_flags[sval] & (1L << (*sn))))
		{
			/* Verify the spell is okay */
			if (spell_okay(*sn, known, use_realm - 1))
			{
				/* Success */
				return (TRUE);
			}
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Extract spells */
	for (spell = 0; spell < 32; spell++)
	{
		/* Check for this spell */
		if ((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}

	/* Assume no usable spells */
	okay = FALSE;

	/* Assume no spells available */
	(*sn) = -2;

	/* Check for "okay" spells */
	for (i = 0; i < num; i++)
	{
		/* Look for "okay" spells */
		if (spell_okay(spells[i], known, use_realm - 1))
			okay = TRUE;
	}

	/* No "okay" spells */
	if (!okay)
		return (FALSE);

	/* Assume cancelled */
	*sn = (-1);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Show choices */
	if (show_choices)
	{
		/* Update */
		p_ptr->window |= (PW_SPELL);

		/* Window stuff */
		window_stuff();
	}


	/* Build a prompt (accept all spells) */
	(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) %^s which %s? ",
		p, I2A(0), I2A(num - 1), prompt, p);

#if 1 /* TNB */

	Bind_Choose(KEYWORD_CHOOSE_SPELL + 1, k_idx, TRUE);

	while (!flag)
	{
		inkey_flags = (INKEY_SPELL);
		inkey_book = k_idx;

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
				/* Show list */
				redraw = TRUE;

#if 1 /* TNB */

				angtk_eval("angband_display", "book", "show", format("%d",
						inkey_book), NULL);	/* TNB */

#else /* TNB */

				/* Save the screen */
				screen_save();

				/* Display a list of spells */
				print_spells(spells, num, 1, 20, use_realm - 1);

#endif /* TNB */
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

#if 1 /* TNB */

				angtk_eval("angband_display", "book", "hide", NULL); /* TNB */

#else /* TNB */

				/* Restore the screen */
				screen_load();

#endif /* TNB */
			}

			/* Redo asking */
			continue;
		}


		/* Note verify */
		ask = (isupper(choice));

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
		spell = spells[i];

		/* Require "okay" spells */
		if (!spell_okay(spell, known, use_realm - 1))
		{
			bell();
			msg_format("You may not %s that %s.", prompt, p);
			continue;
		}

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Access the spell */
			s_ptr = &mp_ptr->info[use_realm - 1][spell % 32];

			/* Prompt */
			(void) strnfmt(tmp_val, 78, "%^s %s (%d mana, %d%% fail)? ",
				prompt, spell_names[use_realm - 1][spell % 32],
				s_ptr->smana, spell_chance(spell, use_realm - 1));

			/* Belay that order */
			if (!get_check(tmp_val))
				continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

#if 1 /* TNB */

	Bind_Choose(KEYWORD_CHOOSE_SPELL + 1, 0, FALSE);

	if (redraw)
		angtk_eval("angband_display", "book", "hide", NULL);

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
	(*sn) = spell;

#ifdef ALLOW_REPEAT	/* TNB */

	repeat_push(*sn);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


#if 1 /* TNB */

/*
 * Peruse all our books.
 */
void do_cmd_browse(void)
{
	/* Warriors are illiterate */
	if (!(p_ptr->realm1 || p_ptr->realm2))
	{
		msg_print("You cannot read books!");
		return;
	}

	msg_print(NULL);

	/* Prompt for a command */
	prompt_print("(Browsing) Command:");

	angtk_eval("angband_display", "book", "show", NULL);

    /* Hack -- Get a new command */
	inkey_book = 0;
	inkey_flags = (INKEY_CMD);
    command_new = inkey();
	inkey_flags = 0;

	angtk_eval("angband_display", "book", "hide", NULL);

	msg_print(NULL);

	/* Hack -- Process "Escape" */
	if (command_new == ESCAPE)
	{
		/* Reset stuff */
		command_new = 0;
	}
}

#else /* TNB */

/*
 * Peruse the spells/prayers in a book
 *
 * Note that *all* spells in the book are listed
 *
 * Note that browsing is allowed while confused or blind,
 * and in the dark, primarily to allow browsing in stores.
 */
void do_cmd_browse(void)
{
	int item, sval;
#if 0 /* TNB */
	int spell = -1;
	int num = 0;

	byte spells[64];
#endif /* TNB */

	object_type *o_ptr;

	cptr q, s;

	/* Warriors are illiterate */
	if (!(p_ptr->realm1 || p_ptr->realm2))
	{
		msg_print("You cannot read books!");
		return;
	}

#if 0

	/* No lite */
	if (p_ptr->blind || no_lite())
	{
		msg_print("You cannot see!");
		return;
	}

	/* Confused */
	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		return;
	}

#endif

	/* Restrict choices to "useful" books */
	item_tester_tval = mp_ptr->spell_book;

	/* Get an item */
	q = "Browse which book? ";
	s = "You have no books that you can read.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
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

	/* Access the item's sval */
	sval = o_ptr->sval;

	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();

#if 1 /* TNB */

	prompt_print("(Browsing) Command:");

	inkey_book = o_ptr->k_idx;
	angtk_eval("angband_display", "book", "show", format("%d", inkey_book),
		NULL);

	/* Hack -- Get a new command */
	inkey_flags = (INKEY_CMD);
	command_new = inkey();
	inkey_flags = 0;

	prompt_erase();
	angtk_eval("angband_display", "book", "hide", NULL);

	/* Hack -- Process "Escape" */
	if (command_new == ESCAPE)
	{
		/* Reset stuff */
		command_new = 0;
	}

#else /* TNB */

	/* Extract spells */
	for (spell = 0; spell < 32; spell++)
	{
		/* Check for this spell */
		if ((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}


	/* Save the screen */
	screen_save();

	/* Display the spells */
	print_spells(spells, num, 1, 20, (o_ptr->tval - 90));

	/* Clear the top line */
	prt("", 0, 0);

	/* Prompt user */
	put_str("[Press any key to continue]", 0, 23);

	/* Wait for key */
	(void) inkey();

	/* Restore the screen */
	screen_load();

#endif /* TNB */
}

#endif /* TNB */


#if 1 /* TNB */

/*
 * Item tester helper routine.
 */
static bool item_tester_hook_book(object_type * o_ptr, bool known)
{
	int realm, spell;

	/* Require the right kind of book */
	if ((o_ptr->tval != REALM1_BOOK) && (o_ptr->tval != REALM2_BOOK))
		return (FALSE);

	/* Extract the realm */
	realm = (o_ptr->tval == REALM1_BOOK) ? p_ptr->realm1 : p_ptr->realm2;

	/* Extract spells */
	for (spell = 0; spell < 32; spell++)
	{
		/* Check for this spell */
		if ((fake_spell_flags[o_ptr->sval] & (1L << spell)))
		{
			/* Look for "okay" spells */
			if (spell_okay(spell, known, realm - 1))
				return (TRUE);
		}
	}

	/* No "okay" spells */
	return (FALSE);
}

/*
 * Hook to determine if a book has cast-able spells.
 */
bool item_tester_hook_cast(object_type * o_ptr)
{
	return item_tester_hook_book(o_ptr, TRUE);
}

/*
 * Hook to determine if a book has study-able spells.
 */
bool item_tester_hook_study(object_type * o_ptr)
{
	return item_tester_hook_book(o_ptr, FALSE);
}

#endif /* TNB */

/*
 * Study a book to gain a new spell/prayer
 */
void do_cmd_study(void)
{
	int i, item, sval;
	int increment = 0;

	/* Spells of realm2 will have an increment of +32 */
	int spell = -1;

	cptr p =
		((mp_ptr->spell_book == TV_SORCERY_BOOK) ? "spell" : "prayer");

	object_type *o_ptr;

	cptr q, s;

	if (!p_ptr->realm1)
	{
		msg_print("You cannot read books!");
		return;
	}

	if (p_ptr->blind || no_lite())
	{
#if 1 /* TNB */
		if (flush_failure)
			flush();
#endif /* TNB */
		msg_print("You cannot see!");
		return;
	}

	if (p_ptr->confused)
	{
#if 1 /* TNB */
		if (flush_failure)
			flush();
#endif /* TNB */
		msg_print("You are too confused!");
		return;
	}

	if (!(p_ptr->new_spells))
	{
		msg_format("You cannot learn any new %ss!", p);
		return;
	}

	msg_format("You can learn %d new %s%s.", p_ptr->new_spells, p,
		(p_ptr->new_spells == 1 ? "" : "s"));
	msg_print(NULL);


	/* Restrict choices to "useful" books */
#if 1 /* TNB */
	item_tester_hook = item_tester_hook_study;
#else /* TNB */
	item_tester_tval = mp_ptr->spell_book;
#endif /* TNB */

	/* Get an item */
	q = "Study which book? ";
	s = "You have no books that you can read.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
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

	/* Access the item's sval */
	sval = o_ptr->sval;

	if (o_ptr->tval == REALM2_BOOK)
		increment = 32;

	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Mage -- Learn a selected spell */
	if (mp_ptr->spell_book != TV_LIFE_BOOK)
	{
		/* Ask for a spell, allow cancel */
#if 1 /* TNB */
		if (!get_spell(&spell, "study", o_ptr->k_idx, FALSE,
			(bool) (increment ? TRUE : FALSE)) && (spell == -1))
#else /* TNB */
		if (!get_spell(&spell, "study", sval, FALSE,
			(bool) (increment ? TRUE : FALSE)) && (spell == -1))
#endif /* TNB */
			return;
	}

	/* Priest -- Learn a random prayer */
	else
	{
		int k = 0;

		int gift = -1;

		/* Extract spells */
		for (spell = 0; spell < 32; spell++)
		{
			/* Check spells in the book */
			if ((fake_spell_flags[sval] & (1L << spell)))
			{
				/* Skip non "okay" prayers */
				if (!spell_okay(spell, FALSE,
						(increment ? p_ptr->realm2 - 1 : p_ptr->realm1 -
							1))) continue;

				/* Hack -- Prepare the randomizer */
				k++;

				/* Hack -- Apply the randomizer */
				if (rand_int(k) == 0)
					gift = spell;
			}
		}

		/* Accept gift */
		spell = gift;
	}

	/* Nothing to study */
	if (spell < 0)
	{
		/* Message */
		msg_format("You cannot learn any %ss in that book.", p);

		/* Abort */
		return;
	}


	/* Take a turn */
	energy_use = 100;

	if (increment)
		spell += increment;

	/* Learn the spell */
	if (spell < 32)
	{
		spell_learned1 |= (1L << spell);
	}
	else
	{
		spell_learned2 |= (1L << (spell - 32));
	}

	/* Find the next open entry in "spell_order[]" */
	for (i = 0; i < 64; i++)
	{
		/* Stop at the first empty space */
		if (spell_order[i] == 99)
			break;
	}

	/* Add the spell to the known list */
	spell_order[i++] = spell;

	/* Mention the result */
	msg_format("You have learned the %s of %s.", p,
		spell_names[(increment ? p_ptr->realm2 - 1 : p_ptr->realm1 -
				1)][spell % 32]);

	if (mp_ptr->spell_book == TV_LIFE_BOOK)
		chg_virtue(V_FAITH, 1);
	else
		chg_virtue(V_KNOWLEDGE, 1);

	/* One less spell available */
	p_ptr->new_spells--;

	/* Message if needed */
	if (p_ptr->new_spells)
	{
		/* Message */
		msg_format("You can learn %d more %s%s.", p_ptr->new_spells, p,
			(p_ptr->new_spells != 1) ? "s" : "");
	}

	/* Save the new_spells value */
	p_ptr->old_spells = p_ptr->new_spells;

	/* Redraw Study Status */
	p_ptr->redraw |= (PR_STUDY);
}


#define MAX_BIZARRE		6


static int bizarre_num[MAX_BIZARRE] = {
	SUMMON_BIZARRE1,
	SUMMON_BIZARRE2,
	SUMMON_BIZARRE3,
	SUMMON_BIZARRE4,
	SUMMON_BIZARRE5,
	SUMMON_BIZARRE6,
};


static void wild_magic(int spell)
{
	switch (randint(spell) + randint(8) + 1)
	{
		case 1:
		case 2:
		case 3:
		{
			teleport_player(10);
			break;
		}
		case 4:
		case 5:
		case 6:
		{
			teleport_player(100);
			break;
		}
		case 7:
		case 8:
		{
			teleport_player(200);
			break;
		}
		case 9:
		case 10:
		case 11:
		{
			unlite_area(10, 3);
			break;
		}
		case 12:
		case 13:
		case 14:
		{
			lite_area(damroll(2, 3), 2);
			break;
		}
		case 15:
		{
			destroy_doors_touch();
			break;
		}
		case 16:
		case 17:
		{
			wall_breaker();
			break;
		}
		case 18:
		{
			sleep_monsters_touch();
			break;
		}
		case 19:
		case 20:
		{
			trap_creation();
			break;
		}
		case 21:
		case 22:
		{
			door_creation();
			break;
		}
		case 23:
		case 24:
		case 25:
		{
			aggravate_monsters(0);
			break;
		}
		case 26:
		{
			earthquake(py, px, 5);
			break;
		}
		case 27:
		case 28:
		{
			(void) gain_random_mutation(0);
			break;
		}
		case 29:
		case 30:
		{
			apply_disenchant(0);
			break;
		}
		case 31:
		{
			lose_all_info();
			break;
		}
		case 32:
		{
			fire_ball(GF_CHAOS, 0, spell + 5, 1 + (spell / 10));
			break;
		}
		case 33:
		{
			wall_stone();
			break;
		}
		case 34:
		case 35:
		{
			int i;
			int type = bizarre_num[rand_int(6)];

			for (i = 0; i < 8; i++)
			{
				(void) summon_specific(0, py, px, (dun_level * 3) / 2,
					type, TRUE, FALSE, FALSE);
			}
			break;
		}
		case 36:
		case 37:
		{
			(void) activate_hi_summon();
			break;
		}
		case 38:
		{
			(void) summon_cyber(-1, py, px);
			break;
		}
		default:
		{
			int count = 0;

			(void) activate_ty_curse(FALSE, &count);

			break;
		}
	}

	return;
}


static bool cast_life_spell(int spell)
{
	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
		case 0: /* Detect Evil */
			(void) detect_monsters_evil();
			break;
		case 1: /* Cure Light Wounds */
			(void) hp_player(damroll(2, 10));
			(void) set_cut(p_ptr->cut - 10);
			break;
		case 2: /* Bless */
			(void) set_blessed(p_ptr->blessed + randint(12) + 12);
			break;
		case 3: /* Remove Fear */
			(void) set_afraid(0);
			break;
		case 4: /* Call Light */
			(void) lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
			break;
		case 5: /* Detect Traps + Secret Doors */
			(void) detect_traps();
			(void) detect_doors();
			(void) detect_stairs();
			break;
		case 6: /* Cure Medium Wounds */
			(void) hp_player(damroll(4, 10));
			(void) set_cut((p_ptr->cut / 2) - 20);
			break;
		case 7: /* Satisfy Hunger */
			(void) set_food(PY_FOOD_MAX - 1);
			break;
		case 8: /* Remove Curse */
			remove_curse();
			break;
		case 9: /* Cure Poison */
			(void) set_poisoned(0);
			break;
		case 10: /* Cure Critical Wounds */
			(void) hp_player(damroll(8, 10));
			(void) set_stun(0);
			(void) set_cut(0);
			break;
		case 11: /* Sense Unseen */
			(void) set_tim_invis(p_ptr->tim_invis + randint(24) + 24);
			break;
		case 12: /* Orb or Draining */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_HOLY_FIRE, dir, (damroll(3,
						6) + plev + (plev / ((p_ptr->pclass == CLASS_PRIEST
								|| p_ptr->pclass ==
								CLASS_HIGH_MAGE) ? 2 : 4))),
				((plev < 30) ? 2 : 3));

			break;
		case 13: /* Protection from Evil */
			(void) set_protevil(p_ptr->protevil + randint(25) +
				3 * p_ptr->lev);
			break;
		case 14: /* Healing */
			(void) hp_player(300);
			(void) set_stun(0);
			(void) set_cut(0);
			break;
		case 15: /* Glyph of Warding */
			warding_glyph();
			break;
		case 16: /* Exorcism */
			(void) dispel_undead(plev);
			(void) dispel_demons(plev);
			(void) turn_evil(plev);
			break;
		case 17: /* Dispel Curse */
			(void) remove_all_curse();
			break;
		case 18: /* Dispel Undead + Demons */
			(void) dispel_undead(plev * 3);
			(void) dispel_demons(plev * 3);
			break;
		case 19: /* 'Day of the Dove' */
			charm_monsters(plev * 2);
			break;
		case 20: /* Dispel Evil */
			(void) dispel_evil(plev * 4);
			break;
		case 21: /* Banishment */
			if (banish_evil(100))
			{
				msg_print("The power of your god banishes evil!");
			}
			break;
		case 22: /* Holy Word */
			(void) dispel_evil(plev * 4);
			(void) hp_player(1000);
			(void) set_afraid(0);
			(void) set_poisoned(0);
			(void) set_stun(0);
			(void) set_cut(0);
			break;
		case 23: /* Warding True */
			warding_glyph();
			glyph_creation();
			break;
		case 24: /* Heroism */
			(void) set_hero(p_ptr->hero + randint(25) + 25);
			(void) hp_player(10);
			(void) set_afraid(0);
			break;
		case 25: /* Prayer */
			(void) set_blessed(p_ptr->blessed + randint(48) + 48);
			break;
		case 26:
			return bless_weapon();
		case 27: /* Restoration */
			(void) do_res_stat(A_STR);
			(void) do_res_stat(A_INT);
			(void) do_res_stat(A_WIS);
			(void) do_res_stat(A_DEX);
			(void) do_res_stat(A_CON);
			(void) do_res_stat(A_CHR);
			(void) restore_level();
			break;
		case 28: /* Healing True */
			(void) hp_player(2000);
			(void) set_stun(0);
			(void) set_cut(0);
			break;
		case 29: /* Holy Vision */
			return identify_fully();
		case 30: /* Divine Intervention */
			project(0, 1, py, px, 777, GF_HOLY_FIRE, PROJECT_KILL);
			dispel_monsters(plev * 4);
			slow_monsters();
			stun_monsters(plev * 4);
			confuse_monsters(plev * 4);
			turn_monsters(plev * 4);
			stasis_monsters(plev * 4);
			summon_specific(-1, py, px, plev, SUMMON_ANGEL, TRUE, TRUE,
				TRUE);
			(void) set_shero(p_ptr->shero + randint(25) + 25);
			(void) hp_player(300);
			if (!p_ptr->fast)
			{ /* Haste */
				(void) set_fast(randint(20 + plev) + plev);
			}
			else
			{
				(void) set_fast(p_ptr->fast + randint(5));
			}
			(void) set_afraid(0);
			break;
		case 31: /* Holy Invulnerability */
			(void) set_invuln(p_ptr->invuln + randint(7) + 7);
			break;
		default:
			msg_format("You cast an unknown Life spell: %d.", spell);
			msg_print(NULL);
	}

	return TRUE;
}



static bool cast_sorcery_spell(int spell)
{
	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
		case 0: /* Detect Monsters */
			(void) detect_monsters_normal();
			break;
		case 1: /* Phase Door */
			teleport_player(10);
			break;
		case 2: /* Detect Doors and Traps */
			(void) detect_traps();
			(void) detect_doors();
			(void) detect_stairs();
			break;
		case 3: /* Light Area */
			(void) lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
			break;
		case 4: /* Confuse Monster */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) confuse_monster(dir, (plev * 3) / 2);
			break;
		case 5: /* Teleport Self */
			teleport_player(plev * 5);
			break;
		case 6: /* Sleep Monster */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) sleep_monster(dir);
			break;
		case 7: /* Recharging */
			return recharge(plev * 4);
		case 8: /* Magic Mapping */
			map_area();
			break;
		case 9: /* Identify */
			return ident_spell();
		case 10: /* Slow Monster */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) slow_monster(dir);
			break;
		case 11: /* Mass Sleep */
			(void) sleep_monsters();
			break;
		case 12: /* Teleport Away */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) fire_beam(GF_AWAY_ALL, dir, plev);
			break;
		case 13: /* Haste Self */
			if (!p_ptr->fast)
			{
				(void) set_fast(randint(20 + plev) + plev);
			}
			else
			{
				(void) set_fast(p_ptr->fast + randint(5));
			}
			break;
		case 14: /* Detection True */
			(void) detect_all();
			break;
		case 15: /* Identify True */
			return identify_fully();
		case 16: /* Detect Objects and Treasure */
			(void) detect_objects_normal();
			(void) detect_treasure();
			(void) detect_objects_gold();
			break;
		case 17: /* Detect Enchantment */
			(void) detect_objects_magic();
			break;
		case 18: /* Charm Monster */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) charm_monster(dir, plev);
			break;
		case 19: /* Dimension Door */
			msg_print
				("You open a dimensional gate. Choose a destination.");
			return dimension_door();
		case 20: /* Sense Minds */
			(void) set_tim_esp(p_ptr->tim_esp + randint(30) + 25);
			break;
		case 21: /* Self knowledge */
			(void) self_knowledge();
			break;
		case 22: /* Teleport Level */
			(void) teleport_player_level();
			break;
		case 23: /* Word of Recall */
			word_of_recall();
			break;
		case 24: /* Stasis */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) stasis_monster(dir);
			break;
		case 25: /* Telekinesis */
			if (!get_aim_dir(&dir))
				return FALSE;

			fetch(dir, plev * 15, FALSE);
			break;
		case 26: /* Explosive Rune */
			explosive_rune();
			break;
		case 27: /* Clairvoyance */
			wiz_lite();
			if (!(p_ptr->telepathy))
			{
				(void) set_tim_esp(p_ptr->tim_esp + randint(30) + 25);
			}
			break;
		case 28: /* Enchant Weapon */
			return enchant_spell(rand_int(4) + 1, rand_int(4) + 1, 0);
		case 29: /* Enchant Armour */
			return enchant_spell(0, 0, rand_int(3) + 2);
		case 30: /* Alchemy */
			return alchemy();
		case 31: /* Globe of Invulnerability */
			(void) set_invuln(p_ptr->invuln + randint(8) + 8);
			break;
		default:
			msg_format("You cast an unknown Sorcery spell: %d.", spell);
			msg_print(NULL);
	}

	return TRUE;
}


static bool cast_nature_spell(int spell)
{
	int dir;
	int beam;
	int plev = p_ptr->lev;
	bool no_trump = FALSE;

	if (p_ptr->pclass == CLASS_MAGE)
		beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE)
		beam = plev + 10;
	else
		beam = plev / 2;

	switch (spell)
	{
		case 0: /* Detect Creatures */
			(void) detect_monsters_normal();
			break;
		case 1: /* First Aid */
			(void) hp_player(damroll(2, 8));
			(void) set_cut(p_ptr->cut - 15);
			break;
		case 2: /* Detect Doors + Traps */
			(void) detect_traps();
			(void) detect_doors();
			(void) detect_stairs();
			break;
		case 3: /* Produce Food */
			(void) set_food(PY_FOOD_MAX - 1);
			break;
		case 4: /* Daylight */
			(void) lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
			if ((p_ptr->prace == RACE_VAMPIRE) && !p_ptr->resist_lite)
			{
				msg_print("The daylight scorches your flesh!");
				take_hit(damroll(2, 2), "daylight");
			}
			break;
		case 5: /* Animal Taming */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) charm_animal(dir, plev);
			break;
		case 6: /* Resist Environment */
			(void) set_oppose_cold(p_ptr->oppose_cold + randint(20) + 20);
			(void) set_oppose_fire(p_ptr->oppose_fire + randint(20) + 20);
			(void) set_oppose_elec(p_ptr->oppose_elec + randint(20) + 20);
			break;
		case 7: /* Cure Wounds + Poison */
			(void) set_cut(0);
			(void) set_poisoned(0);
			break;
		case 8: /* Stone to Mud */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) wall_to_mud(dir);
			break;
		case 9: /* Lightning Bolt */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_bolt_or_beam(beam - 10, GF_ELEC, dir,
				damroll(3 + ((plev - 5) / 4), 8));
			break;
		case 10: /* Nature Awareness -- downgraded */
			map_area();
			(void) detect_traps();
			(void) detect_doors();
			(void) detect_stairs();
			(void) detect_monsters_normal();
			break;
		case 11: /* Frost Bolt */
			if (!get_aim_dir(&dir))
				return FALSE;
			fire_bolt_or_beam(beam - 10, GF_COLD, dir,
				damroll(5 + ((plev - 5) / 4), 8));
			break;
		case 12: /* Ray of Sunlight */
			if (!get_aim_dir(&dir))
				return FALSE;
			msg_print("A line of sunlight appears.");
			(void) lite_line(dir);
			break;
		case 13: /* Entangle */
			slow_monsters();
			break;
		case 14: /* Summon Animals */
			if (!(summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER,
						TRUE, TRUE, TRUE)))
				no_trump = TRUE;
			break;
		case 15: /* Herbal Healing */
			(void) hp_player(1000);
			(void) set_stun(0);
			(void) set_cut(0);
			(void) set_poisoned(0);
			break;
		case 16: /* Door Building */
			(void) door_creation();
			break;
		case 17: /* Stair Building */
			(void) stair_creation();
			break;
		case 18: /* Stone Skin */
			(void) set_shield(p_ptr->shield + randint(20) + 30);
			break;
		case 19: /* Resistance True */
			(void) set_oppose_acid(p_ptr->oppose_acid + randint(20) + 20);
			(void) set_oppose_elec(p_ptr->oppose_elec + randint(20) + 20);
			(void) set_oppose_fire(p_ptr->oppose_fire + randint(20) + 20);
			(void) set_oppose_cold(p_ptr->oppose_cold + randint(20) + 20);
			(void) set_oppose_pois(p_ptr->oppose_pois + randint(20) + 20);
			break;
		case 20: /* Animal Friendship */
			(void) charm_animals(plev * 2);
			break;
		case 21: /* Stone Tell */
			return identify_fully();
		case 22: /* Wall of Stone */
			(void) wall_stone();
			break;
		case 23: /* Protection from Corrosion */
			return rustproof();
		case 24: /* Earthquake */
			earthquake(py, px, 10);
			break;
		case 25: /* Whirlwind Attack */
		{
			int y = 0, x = 0;
			cave_type *c_ptr;
			monster_type *m_ptr;

			for (dir = 0; dir <= 9; dir++)
			{
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				/* Get the monster */
				m_ptr = &m_list[c_ptr->m_idx];

				/* Hack -- attack monsters */
				if (c_ptr->m_idx && (m_ptr->ml || cave_floor_bold(y, x)))
					py_attack(y, x);
			}
		}
			break;
		case 26: /* Blizzard */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_COLD, dir, 70 + plev, (plev / 12) + 1);
			break;
		case 27: /* Lightning Storm */
			if (!get_aim_dir(&dir))
				return FALSE;
			fire_ball(GF_ELEC, dir, 90 + plev, (plev / 12) + 1);
			break;
		case 28: /* Whirlpool */
			if (!get_aim_dir(&dir))
				return FALSE;
			fire_ball(GF_WATER, dir, 100 + plev, (plev / 12) + 1);
			break;
		case 29: /* Call Sunlight */
			fire_ball(GF_LITE, 0, 150, 8);
			wiz_lite();
			if ((p_ptr->prace == RACE_VAMPIRE) && !p_ptr->resist_lite)
			{
				msg_print("The sunlight scorches your flesh!");
				take_hit(50, "sunlight");
			}
			break;
		case 30: /* Elemental Brand */
			brand_weapon(0);
			break;
		case 31: /* Nature's Wrath */
			(void) dispel_monsters(plev * 4);
			earthquake(py, px, 20 + (plev / 2));
			project(0, 1 + plev / 12, py, px, 100 + plev, GF_DISINTEGRATE,
				PROJECT_KILL | PROJECT_ITEM);
			break;
		default:
			msg_format("You cast an unknown Nature spell: %d.", spell);
			msg_print(NULL);
	}

	if (no_trump)
		msg_print("No animals arrive.");

	return TRUE;
}


static bool cast_chaos_spell(int spell)
{
	int dir, i, beam;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_MAGE)
		beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE)
		beam = plev + 10;
	else
		beam = plev / 2;

	switch (spell)
	{
		case 0: /* Magic Missile */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_bolt_or_beam(beam - 10, GF_MISSILE, dir,
				damroll(3 + ((plev - 1) / 5), 4));
			break;
		case 1: /* Trap / Door destruction */
			(void) destroy_doors_touch();
			break;
		case 2: /* Flash of Light == Light Area */
			(void) lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
			break;
		case 3: /* Touch of Confusion */
			if (!p_ptr->confusing)
			{
				msg_print("Your hands start glowing.");
				p_ptr->confusing = TRUE;
				p_ptr->redraw |= (PR_STATUS);
			}
			break;
		case 4: /* Manaburst */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_MISSILE, dir, (damroll(3,
						5) + plev + (plev / (((p_ptr->pclass == CLASS_MAGE)
								|| (p_ptr->pclass ==
									CLASS_HIGH_MAGE)) ? 2 : 4))),
				((plev < 30) ? 2 : 3));
			/* Shouldn't actually use GF_MANA, as it will destroy all
			 * items on the floor */
			break;
		case 5: /* Fire Bolt */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_bolt_or_beam(beam, GF_FIRE, dir,
				damroll(8 + ((plev - 5) / 4), 8));
			break;
		case 6: /* Fist of Force ("Fist of Fun") */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_DISINTEGRATE, dir, damroll(8 + ((plev - 5) / 4),
					8), 0);
			break;
		case 7: /* Teleport Self */
			teleport_player(plev * 5);
			break;
		case 8: /* Wonder */
		{
			/* This spell should become more useful (more
			 * controlled) as the player gains experience levels.
			 * Thus, add 1/5 of the player's level to the die roll.
			 * This eliminates the worst effects later on, while
			 * keeping the results quite random.  It also allows
			 * some potent effects only at high level. */

			int die = randint(100) + plev / 5;

			if (die < 26)
				chg_virtue(V_CHANCE, 1);

			if (!get_aim_dir(&dir))
				return FALSE;
			if (die > 100)
				msg_print("You feel a surge of power!");
			if (die < 8)
				clone_monster(dir);
			else if (die < 14)
				speed_monster(dir);
			else if (die < 26)
				heal_monster(dir);
			else if (die < 31)
				poly_monster(dir);
			else if (die < 36)
				fire_bolt_or_beam(beam - 10, GF_MISSILE, dir,
					damroll(3 + ((plev - 1) / 5), 4));
			else if (die < 41)
				confuse_monster(dir, plev);
			else if (die < 46)
				fire_ball(GF_POIS, dir, 20 + (plev / 2), 3);
			else if (die < 51)
				(void) lite_line(dir);
			else if (die < 56)
				fire_bolt_or_beam(beam - 10, GF_ELEC, dir,
					damroll(3 + ((plev - 5) / 4), 8));
			else if (die < 61)
				fire_bolt_or_beam(beam - 10, GF_COLD, dir,
					damroll(5 + ((plev - 5) / 4), 8));
			else if (die < 66)
				fire_bolt_or_beam(beam, GF_ACID, dir,
					damroll(6 + ((plev - 5) / 4), 8));
			else if (die < 71)
				fire_bolt_or_beam(beam, GF_FIRE, dir,
					damroll(8 + ((plev - 5) / 4), 8));
			else if (die < 76)
				drain_life(dir, 75);
			else if (die < 81)
				fire_ball(GF_ELEC, dir, 30 + plev / 2, 2);
			else if (die < 86)
				fire_ball(GF_ACID, dir, 40 + plev, 2);
			else if (die < 91)
				fire_ball(GF_ICE, dir, 70 + plev, 3);
			else if (die < 96)
				fire_ball(GF_FIRE, dir, 80 + plev, 3);
			else if (die < 101)
				drain_life(dir, 100 + plev);
			else if (die < 104)
			{
				earthquake(py, px, 12);
			}
			else if (die < 106)
			{
				destroy_area(py, px, 15, TRUE);
			}
			else if (die < 108)
			{
				genocide(TRUE);
			}
			else if (die < 110)
				dispel_monsters(120);
			else /* RARE */
			{
				dispel_monsters(150);
				slow_monsters();
				sleep_monsters();
				hp_player(300);
			}
			break;
		}
		case 9: /* Chaos Bolt */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_bolt_or_beam(beam, GF_CHAOS, dir,
				damroll(10 + ((plev - 5) / 4), 8));
			break;
		case 10: /* Sonic Boom */
			msg_print("BOOM! Shake the room!");
			project(0, plev / 10 + 2, py, px, 45 + plev, GF_SOUND,
				PROJECT_KILL | PROJECT_ITEM);
			break;
		case 11: /* Doom Bolt -- always beam in 2.0.7 or later */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_beam(GF_MANA, dir, damroll(11 + ((plev - 5) / 4), 8));
			break;
		case 12: /* Fire Ball */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_FIRE, dir, plev + 55, 2);
			break;
		case 13: /* Teleport Other */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) fire_beam(GF_AWAY_ALL, dir, plev);
			break;
		case 14: /* Word of Destruction */
			destroy_area(py, px, 15, TRUE);
			break;
		case 15: /* Invoke Logrus */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_CHAOS, dir, plev + 66, plev / 5);
			break;
		case 16: /* Polymorph Other */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) poly_monster(dir);
			break;
		case 17: /* Chain Lightning */
			for (dir = 0; dir <= 9; dir++)
				fire_beam(GF_ELEC, dir, damroll(5 + (plev / 10), 8));
			break;
		case 18: /* Arcane Binding == Charging */
			return recharge(90);
		case 19: /* Disintegration */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_DISINTEGRATE, dir, plev + 80, 3 + plev / 40);
			break;
		case 20: /* Alter Reality */
			alter_reality();
			break;
		case 21: /* Polymorph Self */
			do_poly_self();
			break;
		case 22: /* Chaos Branding */
			brand_weapon(1);
			break;
		case 23: /* Summon monster, demon */
		{
			bool pet = (randint(3) == 1);
			bool group = !(pet && (plev < 50));

			if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2,
					SUMMON_DEMON, group, FALSE, pet))
			{
				msg_print
					("The area fills with a stench of sulphur and brimstone.");

				if (pet)
					msg_print("'What is thy bidding... Master?'");
				else
					msg_print
						("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
			}
			break;
		}
		case 24: /* Beam of Gravity */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_beam(GF_GRAVITY, dir, damroll(9 + ((plev - 5) / 4), 8));
			break;
		case 25: /* Meteor Swarm  */
		{
			int x, y, dx, dy, d;
			int b = 10 + randint(10);
			for (i = 0; i < b; i++)
			{
				int count = 0;

				do
				{
					count++;
					if (count > 1000)
						break;
					x = px - 5 + randint(10);
					y = py - 5 + randint(10);

					dx = (px > x) ? (px - x) : (x - px);
					dy = (py > y) ? (py - y) : (y - py);

					/* Approximate distance */
					d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
				}
				while (in_bounds(y, x) && ((d > 5) ||
						!player_has_los_bold(y, x)));

				if (count > 1000)
					break;

				project(0, 2, y, x, (plev * 3) / 2, GF_METEOR,
					PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM);
			}
		}
			break;
		case 26: /* Flame Strike */
			fire_ball(GF_FIRE, 0, 150 + (2 * plev), 8);
			break;
		case 27: /* Call Chaos */
			call_chaos();
			break;
		case 28: /* Magic Rocket */
			if (!get_aim_dir(&dir))
				return FALSE;

			msg_print("You launch a rocket!");
			fire_ball(GF_ROCKET, dir, 120 + plev, 2);
			break;
		case 29: /* Mana Storm */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_MANA, dir, 300 + (plev * 2), 4);
			break;
		case 30: /* Breathe Logrus */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_CHAOS, dir, p_ptr->chp, 2);
			break;
		case 31: /* Call the Void */
			call_the_();
			break;
		default:
			msg_format("You cast an unknown Chaos spell: %d.", spell);
			msg_print(NULL);
	}

	return TRUE;
}


static bool cast_death_spell(int spell)
{
	int dir;
	int beam;
	int plev = p_ptr->lev;
	int dummy = 0;
	int i;

	if (p_ptr->pclass == CLASS_MAGE)
		beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE)
		beam = plev + 10;
	else
		beam = plev / 2;

	switch (spell)
	{
		case 0: /* Detect Undead & Demons -> Unlife */
			(void) detect_monsters_nonliving();
			break;
		case 1: /* Malediction */
			if (!get_aim_dir(&dir))
				return FALSE;
			/* A radius-0 ball may (1) be aimed at objects etc.,
			 * and will affect them; (2) may be aimed at ANY
			 * visible monster, unlike a 'bolt' which must travel
			 * to the monster. */

			fire_ball(GF_HELL_FIRE, dir, damroll(3 + ((plev - 1) / 5), 3),
				0);

			if (randint(5) == 1)
			{ /* Special effect first */
				dummy = randint(1000);
				if (dummy == 666)
					fire_bolt(GF_DEATH_RAY, dir, plev * 200);
				else if (dummy < 500)
					fire_bolt(GF_TURN_ALL, dir, plev);
				else if (dummy < 800)
					fire_bolt(GF_OLD_CONF, dir, plev);
				else
					fire_bolt(GF_STUN, dir, plev);
			}
			break;
		case 2: /* Detect Evil */
			(void) detect_monsters_evil();
			break;
		case 3: /* Stinking Cloud */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_POIS, dir, 10 + (plev / 2), 2);
			break;
		case 4: /* Black Sleep */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) sleep_monster(dir);
			break;
		case 5: /* Resist Poison */
			(void) set_oppose_pois(p_ptr->oppose_pois + randint(20) + 20);
			break;
		case 6: /* Horrify */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) fear_monster(dir, plev);
			(void) stun_monster(dir, plev);
			break;
		case 7: /* Enslave the Undead */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) control_one_undead(dir, plev);
			break;
		case 8: /* Orb of Entropy */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_OLD_DRAIN, dir, (damroll(3,
						6) + plev + (plev / (((p_ptr->pclass == CLASS_MAGE)
								|| (p_ptr->pclass ==
									CLASS_HIGH_MAGE)) ? 2 : 4))),
				((plev < 30) ? 2 : 3));
			break;
		case 9: /* Nether Bolt */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_bolt_or_beam(beam, GF_NETHER, dir,
				damroll(6 + ((plev - 5) / 4), 8));
			break;
		case 10: /* Terror */
			(void) turn_monsters(30 + plev);
			break;
		case 11: /* Vampiric Drain */
			if (!get_aim_dir(&dir))
				return FALSE;

			dummy = plev + randint(plev) * MAX(1, plev / 10); /* Dmg */
			if (drain_life(dir, dummy))
			{
				chg_virtue(V_SACRIFICE, -1);
				chg_virtue(V_VITALITY, -1);

				(void) hp_player(dummy);
				/* Gain nutritional sustenance: 150/hp drained */
				/* A Food ration gives 5000 food points (by contrast) */
				/* Don't ever get more than "Full" this way */
				/* But if we ARE Gorged,  it won't cure us */
				dummy = p_ptr->food + MIN(5000, 100 * dummy);
				if (p_ptr->food < PY_FOOD_MAX) /* Not gorged already */
					(void) set_food(dummy >=
						PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
			}
			break;
		case 12: /* Poison Branding */
			brand_weapon(2);
			break;
		case 13: /* Dispel Good */
			(void) dispel_good(plev * 4);
			break;
		case 14: /* Genocide */
			(void) genocide(TRUE);
			break;
		case 15: /* Restore Life */
			(void) restore_level();
			break;
		case 16: /* Berserk */
			(void) set_shero(p_ptr->shero + randint(25) + 25);
			(void) hp_player(30);
			(void) set_afraid(0);
			break;
		case 17: /* Invoke Spirits */
		{
			int die = randint(100) + plev / 5;
			if (!get_aim_dir(&dir))
				return FALSE;

			msg_print("You call on the power of the dead...");

			if (die < 26)
				chg_virtue(V_CHANCE, 1);

			if (die > 100)
				msg_print("You feel a surge of eldritch force!");

			if (die < 8)
			{
				msg_print
					("Oh no! Mouldering forms rise from the earth around you!");
				(void) summon_specific(0, py, px, dun_level, SUMMON_UNDEAD,
					TRUE, FALSE, FALSE);

				chg_virtue(V_UNLIFE, 1);
			}
			else if (die < 14)
			{
				msg_print
					("An unnamable evil brushes against your mind...");
				set_afraid(p_ptr->afraid + randint(4) + 4);
			}
			else if (die < 26)
			{
				msg_print
					("Your head is invaded by a horde of gibbering spectral voices...");
				set_confused(p_ptr->confused + randint(4) + 4);
			}
			else if (die < 31)
			{
				poly_monster(dir);
			}
			else if (die < 36)
			{
				fire_bolt_or_beam(beam - 10, GF_MISSILE, dir,
					damroll(3 + ((plev - 1) / 5), 4));
			}
			else if (die < 41)
			{
				confuse_monster(dir, plev);
			}
			else if (die < 46)
			{
				fire_ball(GF_POIS, dir, 20 + (plev / 2), 3);
			}
			else if (die < 51)
			{
				(void) lite_line(dir);
			}
			else if (die < 56)
			{
				fire_bolt_or_beam(beam - 10, GF_ELEC, dir,
					damroll(3 + ((plev - 5) / 4), 8));
			}
			else if (die < 61)
			{
				fire_bolt_or_beam(beam - 10, GF_COLD, dir,
					damroll(5 + ((plev - 5) / 4), 8));
			}
			else if (die < 66)
			{
				fire_bolt_or_beam(beam, GF_ACID, dir,
					damroll(6 + ((plev - 5) / 4), 8));
			}
			else if (die < 71)
			{
				fire_bolt_or_beam(beam, GF_FIRE, dir,
					damroll(8 + ((plev - 5) / 4), 8));
			}
			else if (die < 76)
			{
				drain_life(dir, 75);
			}
			else if (die < 81)
			{
				fire_ball(GF_ELEC, dir, 30 + plev / 2, 2);
			}
			else if (die < 86)
			{
				fire_ball(GF_ACID, dir, 40 + plev, 2);
			}
			else if (die < 91)
			{
				fire_ball(GF_ICE, dir, 70 + plev, 3);
			}
			else if (die < 96)
			{
				fire_ball(GF_FIRE, dir, 80 + plev, 3);
			}
			else if (die < 101)
			{
				drain_life(dir, 100 + plev);
			}
			else if (die < 104)
			{
				earthquake(py, px, 12);
			}
			else if (die < 106)
			{
				destroy_area(py, px, 15, TRUE);
			}
			else if (die < 108)
			{
				genocide(TRUE);
			}
			else if (die < 110)
			{
				dispel_monsters(120);
			}
			else
			{ /* RARE */
				dispel_monsters(150);
				slow_monsters();
				sleep_monsters();
				hp_player(300);
			}

			if (die < 31)
				msg_print
					("Sepulchral voices chuckle. 'Soon you will join us, mortal.'");
			break;
		}
		case 18: /* Dark Bolt */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_bolt_or_beam(beam, GF_DARK, dir,
				damroll(4 + ((plev - 5) / 4), 8));
			break;
		case 19: /* Battle Frenzy */
			(void) set_shero(p_ptr->shero + randint(25) + 25);
			(void) hp_player(30);
			(void) set_afraid(0);
			if (!p_ptr->fast)
			{
				(void) set_fast(randint(20 + (plev / 2)) + (plev / 2));
			}
			else
			{
				(void) set_fast(p_ptr->fast + randint(5));
			}
			break;
		case 20: /* Vampirism True */
			if (!get_aim_dir(&dir))
				return FALSE;

			chg_virtue(V_SACRIFICE, -1);
			chg_virtue(V_VITALITY, -1);

			for (dummy = 0; dummy < 3; dummy++)
			{
				if (drain_life(dir, 100))
					hp_player(100);
			}
			break;
		case 21: /* Vampiric Branding */
			brand_weapon(3);
			break;
		case 22: /* Darkness Storm */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_DARK, dir, 120, 4);
			break;
		case 23: /* Mass Genocide */
			(void) mass_genocide(TRUE);
			break;
		case 24: /* Death Ray */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) death_ray(dir, plev);
			break;
		case 25: /* Raise the Dead */
		{
			bool pet = (randint(3) == 1);
			bool group;
			int type;

			if (pet)
			{
				type =
					(plev >
					47 ? SUMMON_HI_UNDEAD_NO_UNIQUES : SUMMON_UNDEAD);
				group = (((plev > 24) &&
						(randint(3) == 1)) ? TRUE : FALSE);
			}
			else
			{
				type = (plev > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);
				group = TRUE;
			}

			if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2,
					type, group, FALSE, pet))
			{
				msg_print
					("Cold winds begin to blow around you, carrying with them the stench of decay...");

				if (pet)
					msg_print
						("Ancient, long-dead forms arise from the ground to serve you!");
				else
					msg_print
						("'The dead arise... to punish you for disturbing them!'");

				chg_virtue(V_UNLIFE, 1);
			}

			break;
		}
		case 26: /* Esoteria */
			if (randint(50) > plev)
				return ident_spell();
			else
				return identify_fully();
			break;
		case 27: /* Word of Death */
			(void) dispel_living(plev * 3);
			break;
		case 28: /* Evocation */
			(void) dispel_monsters(plev * 4);
			turn_monsters(plev * 4);
			banish_monsters(plev * 4);
			break;
		case 29: /* Hellfire */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_ball(GF_HELL_FIRE, dir, 666, 3);
			take_hit(50 + randint(50), "the strain of casting Hellfire");
			break;
		case 30: /* Omnicide */
			p_ptr->csp -= 100; /* Display doesn't show mana cost (100)
							    * as deleted until the spell has finished. This gives a
							    * false impression of how high your mana is climbing.
							    * Therefore, 'deduct' the cost temporarily before entering the
							    * loop, then add it back at the end so that the rest of the
							    * program can deduct it properly */
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				/* Paranoia -- Skip dead monsters */
				if (!m_ptr->r_idx)
					continue;

				/* Hack -- Skip Unique Monsters */
				if (r_ptr->flags1 & (RF1_UNIQUE))
					continue;

				/* Hack -- Skip Quest Monsters */
				if (r_ptr->flags1 & RF1_QUESTOR)
					continue;

				/* Delete the monster */
				delete_monster_idx(i);

				/* Take damage */
				take_hit(randint(4), "the strain of casting Omnicide");

				/* Absorb power of dead soul - up to twice max. mana */
				if (p_ptr->csp < (p_ptr->msp * 2))
					p_ptr->csp++;

				/* Visual feedback */
				move_cursor_relative(py, px);

				/* Redraw */
				p_ptr->redraw |= (PR_HP | PR_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);

				/* Handle */
				handle_stuff();

				/* Fresh */
				Term_fresh();

				/* Delay */
				Term_xtra(TERM_XTRA_DELAY,
					delay_factor * delay_factor * delay_factor);
			}
			p_ptr->csp += 100; /* Restore, ready to be deducted properly */

			break;
		case 31: /* Wraithform */
			set_wraith_form(p_ptr->wraith_form + randint(plev / 2) +
				(plev / 2));
			break;
		default:
			msg_format("You cast an unknown Death spell: %d.", spell);
			msg_print(NULL);
	}

	return TRUE;
}


static bool cast_trump_spell(int spell, bool success)
{
	int dir;
	int beam;
	int plev = p_ptr->lev;
	int dummy = 0;
	bool no_trump = FALSE;
	char ppp[80];
	char tmp_val[160];


	if (p_ptr->pclass == CLASS_MAGE)
		beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE)
		beam = plev + 10;
	else
		beam = plev / 2;

	switch (spell)
	{
		case 0: /* Phase Door */
			if (success)
			{
				teleport_player(10);
			}
			break;
		case 1: /* Mind Blast */
			if (success)
			{
				if (!get_aim_dir(&dir))
					return FALSE;
				fire_bolt_or_beam(beam - 10, GF_PSI, dir,
					damroll(3 + ((plev - 1) / 5), 3));
			}
			break;
		case 2: /* Shuffle */
			if (success)
			{
				/* A limited power 'wonder' spell */
				int die = randint(120);

				if ((p_ptr->pclass == CLASS_ROGUE) ||
					(p_ptr->pclass == CLASS_HIGH_MAGE))
					die = (randint(110)) + plev / 5;
				/* Card sharks and high mages get a level bonus */

				msg_print("You shuffle the deck and draw a card...");

				if (die < 30)
					chg_virtue(V_CHANCE, 1);

				if (die < 7)
				{
					msg_print("Oh no! It's Death!");
					for (dummy = 0; dummy < randint(3); dummy++)
						(void) activate_hi_summon();
				}
				else if (die < 14)
				{
					msg_print("Oh no! It's the Devil!");
					(void) summon_specific(0, py, px, dun_level,
						SUMMON_DEMON, TRUE, FALSE, FALSE);
				}
				else if (die < 18)
				{
					int count = 0;

					msg_print("Oh no! It's the Hanged Man.");
					(void) activate_ty_curse(FALSE, &count);
				}
				else if (die < 22)
				{
					msg_print("It's the swords of discord.");
					aggravate_monsters(0);
				}
				else if (die < 26)
				{
					msg_print("It's the Fool.");
					(void) do_dec_stat(A_INT);
					(void) do_dec_stat(A_WIS);
				}
				else if (die < 30)
				{
					msg_print("It's the picture of a strange monster.");
					if (!(summon_specific(0, py, px, (dun_level * 3) / 2,
								32 + randint(6), TRUE, FALSE, FALSE)))
						no_trump = TRUE;
				}
				else if (die < 33)
				{
					msg_print("It's the Moon.");
					unlite_area(10, 3);
				}
				else if (die < 38)
				{
					msg_print("It's the Wheel of Fortune.");
					wild_magic(rand_int(32));
				}
				else if (die < 40)
				{
					msg_print("It's a teleport trump card.");
					teleport_player(10);
				}
				else if (die < 42)
				{
					msg_print("It's Justice.");
					set_blessed(p_ptr->blessed + p_ptr->lev);
				}
				else if (die < 47)
				{
					msg_print("It's a teleport trump card.");
					teleport_player(100);
				}
				else if (die < 52)
				{
					msg_print("It's a teleport trump card.");
					teleport_player(200);
				}
				else if (die < 60)
				{
					msg_print("It's the Tower.");
					wall_breaker();
				}
				else if (die < 72)
				{
					msg_print("It's Temperance.");
					sleep_monsters_touch();
				}
				else if (die < 80)
				{
					msg_print("It's the Tower.");
					earthquake(py, px, 5);
				}
				else if (die < 82)
				{
					msg_print("It's the picture of a friendly monster.");
					if (!(summon_specific(-1, py, px, (dun_level * 3) / 2,
								SUMMON_BIZARRE1, FALSE, TRUE, TRUE)))
						no_trump = TRUE;
				}
				else if (die < 84)
				{
					msg_print("It's the picture of a friendly monster.");
					if (!(summon_specific(-1, py, px, (dun_level * 3) / 2,
								SUMMON_BIZARRE2, FALSE, TRUE, TRUE)))
						no_trump = TRUE;
				}
				else if (die < 86)
				{
					msg_print("It's the picture of a friendly monster.");
					if (!(summon_specific(-1, py, px, (dun_level * 3) / 2,
								SUMMON_BIZARRE4, FALSE, TRUE, TRUE)))
						no_trump = TRUE;
				}
				else if (die < 88)
				{
					msg_print("It's the picture of a friendly monster.");
					if (!(summon_specific(-1, py, px, (dun_level * 3) / 2,
								SUMMON_BIZARRE5, FALSE, TRUE, TRUE)))
						no_trump = TRUE;
				}
				else if (die < 96)
				{
					msg_print("It's the Lovers.");
					if (get_aim_dir(&dir))
						(void) charm_monster(dir, MIN(p_ptr->lev, 20));
				}
				else if (die < 101)
				{
					msg_print("It's the Hermit.");
					wall_stone();
				}
				else if (die < 111)
				{
					msg_print("It's the Judgement.");
					do_cmd_rerate();
					if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
					{
						msg_print("You are cured of all mutations.");
						p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = 0;
						p_ptr->update |= PU_BONUS;
						handle_stuff();
					}
				}
				else if (die < 120)
				{
					msg_print("It's the Sun.");
					wiz_lite();
				}
				else
				{
					msg_print("It's the World.");
					if (p_ptr->exp < PY_MAX_EXP)
					{
						s32b ee = (p_ptr->exp / 25) + 1;
						if (ee > 5000)
							ee = 5000;
						msg_print("You feel more experienced.");
						gain_exp(ee);
					}
				}
			}
			break;
		case 3: /* Reset Recall */
			if (success)
			{
				/* Prompt */
				sprintf(ppp, "Reset to which level (1-%d): ",
					p_ptr->max_dlv);

				/* Default */
				sprintf(tmp_val, "%d", MAX(dun_level, 1));

				/* Ask for a level */
				if (get_string(ppp, tmp_val, 10))
				{
					/* Extract request */
					dummy = atoi(tmp_val);

					/* Paranoia */
					if (dummy < 1)
						dummy = 1;

					/* Paranoia */
					if (dummy > p_ptr->max_dlv)
						dummy = p_ptr->max_dlv;

					p_ptr->max_dlv = dummy;

					/* Accept request */
					msg_format("Recall depth set to level %d (%d').",
						dummy, dummy * 50);
				}
				else
				{
					return FALSE;
				}
			}
			break;
		case 4: /* Teleport Self */
			if (success)
			{
				teleport_player(plev * 4);
			}
			break;
		case 5: /* Dimension Door */
			if (success)
			{
				msg_print
					("You open a dimensional gate. Choose a destination.");
				return dimension_door();
			}
			break;
		case 6: /* Trump Spying */
			if (success)
			{
				(void) set_tim_esp(p_ptr->tim_esp + randint(30) + 25);
			}
			break;
		case 7: /* Teleport Away */
			if (success)
			{
				if (!get_aim_dir(&dir))
					return FALSE;
				(void) fire_beam(GF_AWAY_ALL, dir, plev);
			}
			break;
		case 8: /* Trump Object */
			if (success)
			{
				if (!get_aim_dir(&dir))
					return FALSE;
				fetch(dir, plev * 15, TRUE);
			}
			break;
		case 9: /* Trump Animal */
		{
			bool pet = success;	/* was (randint(5) > 2) */
			int type = (pet ? SUMMON_ANIMAL_RANGER : SUMMON_ANIMAL);
			bool group = (pet ? FALSE : TRUE);

			msg_print("You concentrate on the trump of an animal...");

			if (summon_specific((pet ? -1 : 0), py, px, plev, type, group,
					FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned animal gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 10: /* Phantasmal Servant */
			if (success)
			{
				if (summon_specific(-1, py, px, (plev * 3) / 2,
						SUMMON_PHANTOM, FALSE, TRUE, TRUE))
				{
					msg_print("'Your wish, master?'");
				}
				else
				{
					no_trump = TRUE;
				}
			}
			break;
		case 11: /* Trump Monster */
		{
			bool pet = success;	/* was (randint(5) > 2) */
			int type = (pet ? SUMMON_NO_UNIQUES : 0);
			bool group = (pet ? FALSE : TRUE);

			msg_print("You concentrate on the trump of a monster...");

			if (summon_specific((pet ? -1 : 0), py, px, plev, type, group,
					FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned creature gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 12: /* Conjure Elemental */
		{
			bool pet = success;	/* was (randint(6) > 3) */
			bool group = (pet ? FALSE : TRUE);

			msg_print("You concentrate on the trump of a monster...");

			if (summon_specific((pet ? -1 : 0), py, px, plev,
					SUMMON_ELEMENTAL, group, FALSE, pet))
			{
				if (!pet)
					msg_print
						("You fail to control the elemental creature!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 13: /* Teleport Level */
			if (success)
			{
				(void) teleport_player_level();
			}
			break;
		case 14: /* Word of Recall */
			if (success)
			{
				word_of_recall();
			}
			break;
		case 15: /* Banish */
			if (success)
			{
				banish_monsters(plev * 4);
			}
			break;
		case 16: /* Joker Card */
		{
			bool pet = success;	/* was (randint(2) == 1) */
			bool group = (pet ? FALSE : TRUE);

			msg_print("You concentrate on a joker card...");

			switch (randint(4))
			{
				case 1:
					dummy = SUMMON_BIZARRE1;
					break;
				case 2:
					dummy = SUMMON_BIZARRE2;
					break;
				case 3:
					dummy = SUMMON_BIZARRE4;
					break;
				case 4:
					dummy = SUMMON_BIZARRE5;
					break;
			}

			if (summon_specific((pet ? -1 : 0), py, px, plev, dummy, group,
					FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned creature gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}
		case 17: /* Trump Spiders */
		{
			bool pet = success;	/* (randint(5) > 2) */
			bool group = (pet ? FALSE : TRUE);

			msg_print("You concentrate on the trump of a spider...");

			if (summon_specific((pet ? -1 : 0), py, px, plev,
					SUMMON_SPIDER, group, FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned spiders get angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 18: /* Trump Reptiles */
		{
			bool pet = success;	/* was (randint(5) > 2) */

			msg_print("You concentrate on the trump of a reptile...");

			if (summon_specific((pet ? -1 : 0), py, px, plev, SUMMON_HYDRA,
					TRUE, FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned reptile gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 19: /* Trump Hounds */
		{
			bool pet = success;	/* was (randint(5) > 2) */

			msg_print("You concentrate on the trump of a hound...");

			if (summon_specific((pet ? -1 : 0), py, px, plev, SUMMON_HOUND,
					TRUE, FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned hounds get angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 20: /* Trump Branding */
			if (success)
			{
				brand_weapon(4);
			}
			break;
		case 21: /* Living Trump */
			if (success)
			{
				if (randint(8) == 1)
					/* Teleport control */
					dummy = 12;
				else
					/* Random teleportation (uncontrolled) */
					dummy = 77;
				/* Gain the mutation */
				if (gain_random_mutation(dummy))
					msg_print("You have turned into a Living Trump.");
			}
			break;
		case 22: /* Death Dealing */
			if (success)
			{
				(void) dispel_living(plev * 3);
			}
			break;
		case 23: /* Trump Cyberdemon */
		{
			bool pet = success;	/* was (randint(10) > 3) */

			msg_print("You concentrate on the trump of a Cyberdemon...");

			if (summon_specific((pet ? -1 : 0), py, px, plev * 2,
					SUMMON_CYBER, FALSE, FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned Cyberdemon gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 24: /* Trump Divination */
			if (success)
			{
				(void) detect_all();
			}
			break;
		case 25: /* Trump Lore */
			if (success)
			{
				return identify_fully();
			}
			break;
		case 26: /* Trump Undead */
		{
			bool pet = success;	/* (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);

			msg_print
				("You concentrate on the trump of an undead creature...");

			if (summon_specific((pet ? -1 : 0), py, px, plev,
					SUMMON_UNDEAD, group, FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned undead creature gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 27: /* Trump Dragon */
		{
			bool pet = success;	/* was (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);

			msg_print("You concentrate on the trump of a dragon...");

			if (summon_specific((pet ? -1 : 0), py, px, plev,
					SUMMON_DRAGON, group, FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned dragon gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 28: /* Mass Trump */
		{
			no_trump = TRUE;

			msg_print("You concentrate on several trumps at once...");

			for (dummy = 0; dummy < 3 + (plev / 10); dummy++)
			{
				bool pet = success;	/* was (randint(10) > 3) */
				bool group = (pet ? FALSE : TRUE);
				int type = (pet ? SUMMON_NO_UNIQUES : 0);

				if (summon_specific((pet ? -1 : 0), py, px, plev, type,
						group, FALSE, pet))
				{
					if (!pet)
						msg_print("The summoned creatures get angry!");
					no_trump = FALSE;
				}
			}
			break;
		}
		case 29: /* Trump Demon */
		{
			bool pet = success;	/* was (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);

			msg_print("You concentrate on the trump of a demon...");

			if (summon_specific((pet ? -1 : 0), py, px, plev, SUMMON_DEMON,
					group, FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned demon gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 30: /* Trump Ancient Dragon */
		{
			bool pet = success;	/* was (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);
			int type =
				(pet ? SUMMON_HI_DRAGON_NO_UNIQUES : SUMMON_HI_DRAGON);

			msg_print
				("You concentrate on the trump of an ancient dragon...");

			if (summon_specific((pet ? -1 : 0), py, px, plev, type, group,
					FALSE, pet))
			{
				if (!pet)
					msg_print("The summoned ancient dragon gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 31: /* Trump Greater Undead */
		{
			bool pet = success;	/* was (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);
			int type =
				(pet ? SUMMON_HI_UNDEAD_NO_UNIQUES : SUMMON_HI_UNDEAD);

			msg_print
				("You concentrate on the trump of a greater undead being...");

			if (summon_specific((pet ? -1 : 0), py, px, plev, type, group,
					FALSE, pet))
			{
				if (!pet)
					msg_print
						("The summoned greater undead creature gets angry!");
			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		default:
			msg_format("You cast an unknown Trump spell: %d.", spell);
			msg_print(NULL);
	}

	if (no_trump)
	{
		msg_print("Nobody answers to your Trump call.");
	}

	return TRUE;
}


static bool cast_arcane_spell(int spell)
{
	int dir;
	int beam;
	int plev = p_ptr->lev;
	int dummy = 0;

	if (p_ptr->pclass == CLASS_MAGE)
		beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE)
		beam = plev + 10;
	else
		beam = plev / 2;

	switch (spell)
	{
		case 0: /* Zap */
			if (!get_aim_dir(&dir))
				return FALSE;

			fire_bolt_or_beam(beam - 10, GF_ELEC, dir,
				damroll(3 + ((plev - 1) / 5), 3));
			break;
		case 1: /* Wizard Lock */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) wizard_lock(dir);
			break;
		case 2: /* Detect Invisibility */
			(void) detect_monsters_invis();
			break;
		case 3: /* Detect Monsters */
			(void) detect_monsters_normal();
			break;
		case 4: /* Blink */
			teleport_player(10);
			break;
		case 5: /* Light Area */
			(void) lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
			break;
		case 6: /* Trap & Door Destruction */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) destroy_door(dir);
			break;
		case 7: /* Cure Light Wounds */
			(void) hp_player(damroll(2, 8));
			(void) set_cut(p_ptr->cut - 10);
			break;
		case 8: /* Detect Doors & Traps */
			(void) detect_traps();
			(void) detect_doors();
			(void) detect_stairs();
			break;
		case 9: /* Phlogiston */
			phlogiston();
			break;
		case 10: /* Detect Treasure */
			(void) detect_treasure();
			(void) detect_objects_gold();
			break;
		case 11: /* Detect Enchantment */
			(void) detect_objects_magic();
			break;
		case 12: /* Detect Object */
			(void) detect_objects_normal();
			break;
		case 13: /* Cure Poison */
			(void) set_poisoned(0);
			break;
		case 14: /* Resist Cold */
			(void) set_oppose_cold(p_ptr->oppose_cold + randint(20) + 20);
			break;
		case 15: /* Resist Fire */
			(void) set_oppose_fire(p_ptr->oppose_fire + randint(20) + 20);
			break;
		case 16: /* Resist Lightning */
			(void) set_oppose_elec(p_ptr->oppose_elec + randint(20) + 20);
			break;
		case 17: /* Resist Acid */
			(void) set_oppose_acid(p_ptr->oppose_acid + randint(20) + 20);
			break;
		case 18: /* Cure Medium Wounds */
			(void) hp_player(damroll(4, 8));
			(void) set_cut((p_ptr->cut / 2) - 50);
			break;
		case 19: /* Teleport */
			teleport_player(plev * 5);
			break;
		case 20: /* Stone to Mud */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) wall_to_mud(dir);
			break;
		case 21: /* Ray of Light */
			if (!get_aim_dir(&dir))
				return FALSE;

			msg_print("A line of light appears.");
			(void) lite_line(dir);
			break;
		case 22: /* Satisfy Hunger */
			(void) set_food(PY_FOOD_MAX - 1);
			break;
		case 23: /* See Invisible */
			(void) set_tim_invis(p_ptr->tim_invis + randint(24) + 24);
			break;
		case 24: /* Recharging */
			return recharge(plev * 4);
		case 25: /* Teleport Level */
			(void) teleport_player_level();
			break;
		case 26: /* Identify */
			return ident_spell();
		case 27: /* Teleport Away */
			if (!get_aim_dir(&dir))
				return FALSE;

			(void) fire_beam(GF_AWAY_ALL, dir, plev);
			break;
		case 28: /* Elemental Ball */
			if (!get_aim_dir(&dir))
				return FALSE;

			switch (randint(4))
			{
				case 1:
					dummy = GF_FIRE;
					break;
				case 2:
					dummy = GF_ELEC;
					break;
				case 3:
					dummy = GF_COLD;
					break;
				default:
					dummy = GF_ACID;
					break;
			}
			fire_ball(dummy, dir, 75 + (plev), 2);
			break;
		case 29: /* Detection */
			(void) detect_all();
			break;
		case 30: /* Word of Recall */
			word_of_recall();
			break;
		case 31: /* Clairvoyance */
			wiz_lite();
			if (!p_ptr->telepathy)
			{
				(void) set_tim_esp(p_ptr->tim_esp + randint(30) + 25);
			}
			break;
		default:
			msg_format("You cast an unknown Arcane spell: %d.", spell);
			msg_print(NULL);
	}

	return TRUE;
}


/*
 * Cast a spell
 */
void do_cmd_cast(void)
{
	int item, sval, spell, realm;
	int chance;
	int increment = 0;
	int use_realm;
	bool cast;

	const cptr prayer =
		((mp_ptr->spell_book == TV_LIFE_BOOK) ? "prayer" : "spell");

	object_type *o_ptr;

	magic_type *s_ptr;

	cptr q, s;

	/* Require spell ability */
	if (!p_ptr->realm1)
	{
		msg_print("You cannot cast spells!");
		return;
	}

	/* Require lite */
	if (p_ptr->blind || no_lite())
	{
#if 1 /* TNB */
		if (flush_failure)
			flush();
#endif /* TNB */
		msg_print("You cannot see!");
		return;
	}

	/* Not when confused */
	if (p_ptr->confused)
	{
#if 1 /* TNB */
		if (flush_failure)
			flush();
#endif /* TNB */
		msg_print("You are too confused!");
		return;
	}

	/* Restrict choices to spell books */
#if 1 /* TNB */
	item_tester_hook = item_tester_hook_cast;
#else /* TNB */
	item_tester_tval = mp_ptr->spell_book;
#endif /* TNB */

	/* Get an item */
	q = "Use which book? ";
	s = "You have no spell books!";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
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


	/* Access the item's sval */
	sval = o_ptr->sval;

	if (o_ptr->tval == REALM2_BOOK)
		increment = 32;


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();

	if (increment)
		realm = p_ptr->realm2;
	else
		realm = p_ptr->realm1;

	/* Ask for a spell */
#if 1 /* TNB */
	if (!get_spell(&spell,
		((mp_ptr->spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
		o_ptr->k_idx , TRUE, (bool) (increment ? TRUE : FALSE)))
#else /* TNB */
	if (!get_spell(&spell,
		((mp_ptr->spell_book == TV_LIFE_BOOK) ? "recite" : "cast"), sval,
		TRUE, (bool) (increment ? TRUE : FALSE)))
#endif /* TNB */
	{
		if (spell == -2)
		{
			msg_format("You don't know any %ss in that book.", prayer);
		}
		return;
	}


	/* Access the spell */
	use_realm = (increment ? p_ptr->realm2 : p_ptr->realm1);

	s_ptr = &mp_ptr->info[use_realm - 1][spell];


	/* Verify "dangerous" spells */
	if (s_ptr->smana > p_ptr->csp)
	{
#if 1 /* TNB */
		if (flush_failure)
			flush();
#endif /* TNB */

		/* Warning */
		msg_format("You do not have enough mana to %s this %s.",
			((mp_ptr->spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
			prayer);

		/* Verify */
		if (!get_check("Attempt it anyway? "))
			return;
	}


	/* Spell failure chance */
	chance = spell_chance(spell, use_realm - 1);

	/* Failed spell */
	if (rand_int(100) < chance)
	{
		if (flush_failure)
			flush();

		msg_format("You failed to get the %s off!", prayer);

		if ((randint(100) < chance) &&
			(mp_ptr->spell_book == TV_LIFE_BOOK))
			chg_virtue(V_FAITH, -1);
		else if (randint(100) < chance)
			chg_virtue(V_KNOWLEDGE, -1);


		if (realm == REALM_TRUMP)
		{
			cast_trump_spell(spell, FALSE);
		}
		else if ((o_ptr->tval == TV_CHAOS_BOOK) && (randint(100) < spell))
		{
			msg_print("You produce a chaotic effect!");
			wild_magic(spell);
		}
		else if ((o_ptr->tval == TV_DEATH_BOOK) && (randint(100) < spell))
		{
			if ((sval == 3) && (randint(2) == 1))
			{
				sanity_blast(0, TRUE);
			}
			else
			{
				msg_print("It hurts!");
				take_hit(damroll(o_ptr->sval + 1, 6),
					"a miscast Death spell");
				if ((spell > 15) && (randint(6) == 1) && !p_ptr->hold_life)
					lose_exp(spell * 250);
			}
		}
	}

	/* Process spell */
	else
	{
		if ((randint(100) < chance) && (chance < 50))
		{
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				chg_virtue(V_FAITH, 1);
			else
				chg_virtue(V_KNOWLEDGE, 1);
		}

		/* Spells.  */
		switch (realm)
		{
			case REALM_LIFE: /* * LIFE * */
				cast = cast_life_spell(spell);
				break;
			case REALM_SORCERY: /* * SORCERY * */
				cast = cast_sorcery_spell(spell);
				break;
			case REALM_NATURE:	/* * NATURE * */
				cast = cast_nature_spell(spell);
				break;
			case REALM_CHAOS: /* * CHAOS * */
				cast = cast_chaos_spell(spell);
				break;
			case REALM_DEATH: /* * DEATH * */
				cast = cast_death_spell(spell);
				break;
			case REALM_TRUMP: /* TRUMP */
				cast = cast_trump_spell(spell, TRUE);
				break;
			case REALM_ARCANE:	/* ARCANE */
				cast = cast_arcane_spell(spell);
				break;
			default:
				cast = FALSE;
				msg_format
					("You cast a spell from an unknown realm: realm %d, spell %d.",
					realm, spell);
				msg_print(NULL);
		}

		/* Canceled spells cost neither a turn nor mana */
		if (!cast)
			return;

		/* A spell was cast */
		if (!(increment ? (spell_worked2 & (1L << spell)) : (spell_worked1
					& (1L << spell))))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if (realm == p_ptr->realm1)
			{
				spell_worked1 |= (1L << spell);
			}
			else
			{
				spell_worked2 |= (1L << spell);
			}

			/* Gain experience */
			gain_exp(e * s_ptr->slevel);

			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				chg_virtue(V_FAITH, 1);
			else
				chg_virtue(V_KNOWLEDGE, 1);
		}
	}

	/* Take a turn */
	energy_use = 100;

	/* Sufficient mana */
	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
		msg_print("You faint from the effort!");

		/* Hack -- Bypass free action */
		(void) set_paralyzed(p_ptr->paralyzed + randint(5 * oops + 1));

		if (mp_ptr->spell_book == TV_LIFE_BOOK)
			chg_virtue(V_FAITH, -10);
		else
			chg_virtue(V_KNOWLEDGE, -10);

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
			msg_print("You have damaged your health!");

			/* Reduce constitution */
			(void) dec_stat(A_CON, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);
}


/*
 * Pray a prayer -- Unused in Zangband
 */
void do_cmd_pray(void)
{
	msg_print
		("Praying is not used in Zangband. Use magic spell casting instead.");
}


/*
 * Issue a pet command
 */
void do_cmd_pet(void)
{
	int i = 0;
	int num;
	int powers[36];
	cptr power_desc[36];
	bool flag, redraw;
	int ask;
	char choice;
	char out_val[160];
	int pets = 0, pet_ctr;
	bool all_pets = FALSE;
	monster_type *m_ptr;

	int mode = 0;

	byte y = 1, x = 0;
	int ctr = 0;
	char buf[160];

	num = 0;

	/* Calculate pets */
	/* Process the monsters (backwards) */
	for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
	{
		/* Access the monster */
		m_ptr = &m_list[pet_ctr];

		if (is_pet(m_ptr))
			pets++;
	}

	if (pets)
	{
		power_desc[num] = "dismiss pets";
		powers[num++] = PET_DISMISS;
	}

	power_desc[num] = "stay close";
	if (p_ptr->pet_follow_distance == PET_CLOSE_DIST)
		mode = num;
	powers[num++] = PET_STAY_CLOSE;

	power_desc[num] = "follow me";
	if (p_ptr->pet_follow_distance == PET_FOLLOW_DIST)
		mode = num;
	powers[num++] = PET_FOLLOW_ME;

	power_desc[num] = "seek and destroy";
	if (p_ptr->pet_follow_distance == PET_DESTROY_DIST)
		mode = num;
	powers[num++] = PET_SEEK_AND_DESTROY;

	power_desc[num] = "give me space";
	if (p_ptr->pet_follow_distance == PET_SPACE_DIST)
		mode = num;
	powers[num++] = PET_ALLOW_SPACE;

	power_desc[num] = "stay away";
	if (p_ptr->pet_follow_distance == PET_AWAY_DIST)
		mode = num;
	powers[num++] = PET_STAY_AWAY;

	if (p_ptr->pet_open_doors)
	{
		power_desc[num] = "pets may open doors";
	}
	else
	{
		power_desc[num] = "pets may not open doors";
	}
	powers[num++] = PET_OPEN_DOORS;

	if (p_ptr->pet_pickup_items)
	{
		power_desc[num] = "pets may pick up items";
	}
	else
	{
		power_desc[num] = "pets may not pick up items";
	}
	powers[num++] = PET_TAKE_ITEMS;

	/* Nothing chosen yet */
	flag = FALSE;

	/* Build a prompt (accept all spells) */
	if (num <= 26)
	{
		/* Build a prompt (accept all spells) */
		strnfmt(out_val, 78,
			"(Command %c-%c, *=List, ESC=exit) Select a command: ", I2A(0),
			I2A(num - 1));
	}
	else
	{
		strnfmt(out_val, 78,
			"(Command %c-%c, *=List, ESC=exit) Select a command: ", I2A(0),
			'0' + num - 27);
	}

#if 1 /* TNB */

	redraw = FALSE;

	Bind_Choose(KEYWORD_CHOOSE_CMD_PET + 1, 0, TRUE);

	while (!flag)
	{
		bool okay;

		inkey_flags = (INKEY_CMD_PET);
		okay = get_com(out_val, &choice);
		inkey_flags = 0;

		/* Cancel */
		if (!okay)
			break;

#else /* TNB */

	/* Show list */
	redraw = TRUE;

	/* Save the screen */
	Term_save();

	prt("", y++, x);

	while (ctr < num)
	{
		sprintf(buf, "%s%c) %s", (ctr == mode) ? "*" : " ", I2A(ctr),
			power_desc[ctr]);
		prt(buf, y + ctr, x);
		ctr++;
	}

	if (ctr < 17)
	{
		prt("", y + ctr, x);
	}
	else
	{
		prt("", y + 17, x);
	}

	/* Get a command from the user */
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

				angtk_eval("angband_display", "pets", "show", NULL);

#else /* TNB */

				y = 1;
				x = 0;
				ctr = 0;

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				Term_save();

				prt("", y++, x);

				while (ctr < num)
				{
					sprintf(buf, "%s%c) %s", (ctr == mode) ? "*" : " ",
						I2A(ctr), power_desc[ctr]);
					prt(buf, y + ctr, x);
					ctr++;
				}

				if (ctr < 17)
				{
					prt("", y + ctr, x);
				}
				else
				{
					prt("", y + 17, x);
				}

#endif /* TNB */
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

#if 1 /* TNB */

				angtk_eval("angband_display", "pets", "hide", NULL);
#else /* TNB */

				/* Restore the screen */
				Term_load();

#endif /* TNB */
			}

			/* Redo asking */
			continue;
		}

#if 1 /* TNB */

		/* Dismiss command called by Tcl scripts */
		if (choice == '~')
		{
			flag = TRUE;
			break;
		}

#endif /* TNB */

		if (choice == '\r' && num == 1)
		{
			choice = 'a';
		}

		if (isalpha(choice))
		{
			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if (ask)
				choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}
		else
		{
			ask = FALSE; /* Can't uppercase digits */

			i = choice - '0' + 26;
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Verify it */
		if (ask)
		{
			/* Prompt */
			strnfmt(buf, 78, "Use %s? ", power_desc[i]);

			/* Belay that order */
			if (!get_check(buf))
				continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

#if 1 /* TNB */

	Bind_Choose(KEYWORD_CHOOSE_CMD_PET + 1, 0, FALSE);

	if (redraw)
		angtk_eval("angband_display", "pets", "hide", NULL);

#else /* TNB */

	/* Restore the screen */
	if (redraw)
		Term_load();

#endif /* TNB */

	/* Abort if needed */
	if (!flag)
	{
		energy_use = 0;
		return;
	}

#if 1 /* TNB */

	/*
	 * This is a special command called by Tcl scripts. It repeatedly
	 * asks for m_list[] indexes of pets that should be dismissed.
	 * It isn't intended that the user ever see the prompt.
	 */
	if (choice == '~')
	{
		bool *dismiss;
		char pet_num[4];
		char m_name[80];
		int m_idx, n = 0;

		/* We assume this is zeroed */
		C_MAKE(dismiss, max_m_idx, bool);

		/* Ask for m_list[] indexes until done */
		while (get_string("m_list[] index? ", pet_num, 3))
		{
			m_idx = atoi(pet_num);

			/* Verify m_idx */
			if (m_idx < 1 || m_idx >= m_max)
				continue;

			/* Access the monster */
			m_ptr = &m_list[m_idx];

			/* Skip dead monsters */
			if (!m_ptr->r_idx)
				continue;

			/* Skip non-pets */
			if (!is_pet(m_ptr))
				continue;

			/* Remember this pet */
			dismiss[m_idx] = TRUE;
		}

		/* Dismiss pets (in reverse order) */
		for (m_idx = m_max - 1; m_idx >= 1; m_idx--)
		{
			if (dismiss[m_idx])
			{
				/* Remember the name of a single monster */
				if (!n)
				{
					m_ptr = &m_list[m_idx];
					monster_desc(m_name, m_ptr, 0x80);
				}

				/* Forget this monster */
				/* XXX Drop items carried? */
				delete_monster_idx(m_idx);

				/* Count dismissed pets */
				n++;
			}
		}

		/* Some pets were dismissed */
		if (n == 1)
		{
			msg_format("You have dismissed %s.", m_name);
		}
		else if (n)
		{
			msg_format("You have dismissed %d pets.", n);
		}

		C_KILL(dismiss, max_m_idx, bool);

		/* Done */
		return;
	}

#endif /* TNB */

	switch (powers[i])
	{
		case PET_DISMISS: /* Dismiss pets */
		{
			int Dismissed = 0;

			if (get_check("Dismiss all pets? "))
				all_pets = TRUE;

			/* Process the monsters (backwards) */
			for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				/* Access the monster */
				m_ptr = &m_list[pet_ctr];

				if (is_pet(m_ptr))
				{
					bool delete_this = FALSE;

					if (all_pets)
						delete_this = TRUE;
					else
					{
						char friend_name[80], check_friend[80];
						monster_desc(friend_name, m_ptr, 0x80);
						sprintf(check_friend, "Dismiss %s? ", friend_name);

						if (get_check(check_friend))
							delete_this = TRUE;
					}

					if (delete_this)
					{
						delete_monster_idx(pet_ctr);
						Dismissed++;
					}
				}
			}

			msg_format("You have dismissed %d pet%s.", Dismissed,
				(Dismissed == 1 ? "" : "s"));
			break;
		}
			/* Call pets */
		case PET_STAY_CLOSE:
		{
			p_ptr->pet_follow_distance = PET_CLOSE_DIST;
			break;
		}
			/* "Follow Me" */
		case PET_FOLLOW_ME:
		{
			p_ptr->pet_follow_distance = PET_FOLLOW_DIST;
			break;
		}
			/* "Seek and destoy" */
		case PET_SEEK_AND_DESTROY:
		{
			p_ptr->pet_follow_distance = PET_DESTROY_DIST;
			break;
		}
			/* "Give me space" */
		case PET_ALLOW_SPACE:
		{
			p_ptr->pet_follow_distance = PET_SPACE_DIST;
			break;
		}
			/* "Stay away" */
		case PET_STAY_AWAY:
		{
			p_ptr->pet_follow_distance = PET_AWAY_DIST;
			break;
		}
			/* flag - allow pets to open doors */
		case PET_OPEN_DOORS:
		{
			p_ptr->pet_open_doors = !p_ptr->pet_open_doors;
			break;
		}
			/* flag - allow pets to pickup items */
		case PET_TAKE_ITEMS:
		{
			p_ptr->pet_pickup_items = !p_ptr->pet_pickup_items;

			/* Drop objects being carried by pets */
			if (!p_ptr->pet_pickup_items)
			{
				for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
				{
					/* Access the monster */
					m_ptr = &m_list[pet_ctr];

					if (is_pet(m_ptr))
					{
						monster_drop_carried_objects(m_ptr);
					}
				}
			}

			break;
		}
	}
}
