/* File: ui.c */

/* Purpose: Angband user interface -SF- */

#include "angband.h"


/*
 * Generic "get choice from menu" function
 */
int get_player_choice(cptr *choices, int num, int col, int wid,
                             cptr helpfile, void (*hook) (cptr))
{
	int top = 0, cur = 0;
	/* int bot = 13; */
	int i, dir;
	char c;
	char buf[80];
	bool done = FALSE;
	int hgt;


	/* Autoselect if able */
	if (num == 1) done = TRUE;

	/* Clear */
	for (i = TABLE_ROW; i < Term->hgt; i++)
	{
		/* Clear */
		Term_erase(col, i, Term->wid - wid);
	}

	/* Choose */
	while (TRUE)
	{
		/*
		 * Note to Melkor: What happens when the screen is resized?
		 * There is no 'redraw' hook at this point... 
		 * (That is why the original code restricted itself to what
		 * would fit in the smallest possible screen.) -SF-
		 */
		hgt = Term->hgt - TABLE_ROW - 1;

		/* Redraw the list */
		for (i = 0; ((i + top < num) && (i <= hgt)); i++)
		{
			if (i + top < 26)
			{
				sprintf(buf, "%c) %s", I2A(i + top), choices[i + top]);
			}
			else
			{
				/* ToDo: Fix the ASCII dependency */
				sprintf(buf, "%c) %s", 'A' + (i + top - 26), choices[i + top]);
			}

			/* Clear */
			Term_erase(col, i + TABLE_ROW, wid);

			/* Display */
			if (i == (cur - top))
			{
				/* Highlight the current selection */
				Term_putstr(col, i + TABLE_ROW, wid, TERM_L_BLUE, buf);
			}
			else
			{
				Term_putstr(col, i + TABLE_ROW, wid, TERM_WHITE, buf);
			}
		}

		if (done) return (cur);

		/* Display auxiliary information if any is available. */
		if (hook) hook(choices[cur]);

		/* Move the cursor */
		put_str("", col, TABLE_ROW + cur - top);

		c = inkey();

		if (c == KTRL('X'))
		{
			remove_loc();
			quit(NULL);
		}
		if (c == ESCAPE)
		{
			/* Mega Hack - go back. */
			return (INVALID_CHOICE);
		}
		if (c == '*')
		{
			/* Select at random */
			cur = randint0(num);

			/* Move it onto the screen */
			if ((cur < top) || (cur > top + hgt))
			{
				top = cur;
			}

			/* Done */
			done = TRUE;
		}
		else if (c == '?')
		{
			screen_save();
			(void)show_file(helpfile, NULL, 0, 0);
			screen_load();
		}
		else if (c == '=')
		{
			screen_save();
			do_cmd_options(OPT_FLAG_BIRTH | OPT_FLAG_SERVER | OPT_FLAG_PLAYER);
			screen_load();
		}
		else if ((c == '\n') || (c == '\r'))
		{
			/* Done */
			return (cur);
		}
		else if (isdigit(c))
		{
			/* Get a direction from the key */
			dir = get_keymap_dir(c);

			/* Going up? */
			if (dir == 8)
			{
				if (cur != 0)
				{
					/* Move selection */
					cur--;
				}

				if ((top > 0) && ((cur - top) < 4))
				{
					/* Scroll up */
					top--;
				}
			}

			/* Going down? */
			if (dir == 2)
			{
				if (cur != (num - 1))
				{
					/* Move selection */
					cur++;
				}

				if ((top + hgt < (num - 1)) && ((top + hgt - cur) < 4))
				{
					/* Scroll down */
					top++;
				}
			}
		}
		else if (isalpha(c))
		{
			int choice;

			if (islower(c))
			{
				choice = A2I(c);
			}
			else
			{
				choice = c - 'A' + 26;
			}

			/* Validate input */
			if ((choice > -1) && (choice < num))
			{
				cur = choice;

				/* Move it onto the screen */
				if ((cur < top) || (cur > top + hgt))
				{
					top = cur;
				}

				/* Done */
				done = TRUE;
			}
			else
			{
				bell("Illegal birth choice!");
			}
		}

		/* Invalid input */
		bell("Illegal birth choice!");
	}

	return (INVALID_CHOICE);
}


/*
 * Sorting hook -- comp function -- strings (see below)
 *
 * We use "u" to point to an array of strings.
 */
static bool ang_sort_comp_hook_string(const vptr u, const vptr v, int a, int b)
{
	cptr *x = (cptr *)(u);

	/* Hack - ignore v */
	(void)v;

	return (strcmp(x[a], x[b]) <= 0);
}


/*
 * Sorting hook -- swap function -- array of strings (see below)
 *
 * We use "u" to point to an array of strings.
 */
static void ang_sort_swap_hook_string(const vptr u, const vptr v, int a, int b)
{
	cptr *x = (cptr *)(u);

	cptr temp;

	/* Hack - ignore v */
	(void)v;

	/* Swap */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;
}


/*
 * Present a sorted list to the player, and get a selection
 */
int get_player_sort_choice(cptr *choices, int num, int col, int wid,
                                  cptr helpfile, void (*hook) (cptr))
{
	int i;
	int choice;
	cptr *strings;

	C_MAKE(strings, num, cptr);

	/* Initialise the sorted string array */
	for (i = 0; i < num; i++)
	{
		strings[i] = choices[i];
	}

	/* Sort the strings */
	ang_sort_comp = ang_sort_comp_hook_string;
	ang_sort_swap = ang_sort_swap_hook_string;

	/* Sort the (unique) slopes */
	ang_sort((void *)strings, NULL, num);

	/* Get the choice */
	choice = get_player_choice(strings, num, col, wid, helpfile, hook);

	/* Invert the choice */
	for (i = 0; i < num; i++)
	{
		/* Does the string match the one we selected? */
		if (choices[i] == strings[choice])
		{
			/* Save the choice + exit */
			choice = i;
			break;
		}
	}

	/* Free the strings */
	FREE((void *)strings);

	/* Return the value from the list */
	return (choice);
}

/*
 * Flush the screen, make a noise
 */
void bell(cptr reason)
{
	/* Mega-Hack -- Flush the output */
	Term_fresh();

	/* Hack -- memorize the reason if possible */
	if (character_generated && reason) message_add(reason, MSG_BELL);

	/* Make a bell noise (if allowed) */
	if (ring_bell) Term_xtra(TERM_XTRA_NOISE, 0);

	/* Flush the input (later!) */
	flush();
}


/*
 * Hack -- Make a (relevant?) sound
 */
void sound(int val)
{
	/* No sound */
	if (!use_sound) return;

	/* Make a sound (if allowed) */
	Term_xtra(TERM_XTRA_SOUND, val);
}

/*
 * Convert a "color letter" into an "actual" color
 * The colors are: dwsorgbuDWvyRGBU, as shown below
 */
int color_char_to_attr(char c)
{
	switch (c)
	{
		case 'd': return (TERM_DARK);
		case 'w': return (TERM_WHITE);
		case 's': return (TERM_SLATE);
		case 'o': return (TERM_ORANGE);
		case 'r': return (TERM_RED);
		case 'g': return (TERM_GREEN);
		case 'b': return (TERM_BLUE);
		case 'u': return (TERM_UMBER);

		case 'D': return (TERM_L_DARK);
		case 'W': return (TERM_L_WHITE);
		case 'v': return (TERM_VIOLET);
		case 'y': return (TERM_YELLOW);
		case 'R': return (TERM_L_RED);
		case 'G': return (TERM_L_GREEN);
		case 'B': return (TERM_L_BLUE);
		case 'U': return (TERM_L_UMBER);
	}

	return (-1);
}




/*
 * Hack -- prevent "accidents" in "screen_save()" or "screen_load()"
 */
static int screen_depth = 0;


/*
 * Save the screen, and increase the "icky" depth.
 *
 * This function must match exactly one call to "screen_load()".
 */
void screen_save(void)
{
	/* Hack -- Flush messages */
	message_flush();

	/* Save the screen (if legal) */
	if (screen_depth++ == 0) Term_save();

	/* Increase "icky" depth */
	character_icky++;
}


/*
 * Load the screen, and decrease the "icky" depth.
 *
 * This function must match exactly one call to "screen_save()".
 */
void screen_load(void)
{
	/* Hack -- Flush messages */
	message_flush();

	/* Load the screen (if legal) */
	if (--screen_depth == 0) Term_load();

	/* Decrease "icky" depth */
	character_icky--;
}

/*
 * Display a string on the screen using an attribute.
 *
 * At the given location, using the given attribute, if allowed,
 * add the given string.  Do not clear the line.
 */
void c_put_str(byte attr, cptr str, int col, int row)
{
	/* Hack -- fake monochrome */
	if (!use_color || ironman_moria) attr = TERM_WHITE;

	/* Position cursor, Dump the attr/text */
	Term_putstr(col, row, -1, attr, str);
}


/*
 * As above, but in "white"
 */
void put_str(cptr str, int col, int row)
{
	/* Spawn */
	Term_putstr(col, row, -1, TERM_WHITE, str);
}


/*
 * Put a string with control characters at a given location
 */
void put_cstr(int col, int row, cptr str)
{
	cptr c = str;
	
	/* Default to white */
	byte a = TERM_WHITE;
	
	while (*c)
	{
		/* Does this character match the escape code? */
		if (*c == 1)
		{
			/* Scan the next character */
			c++;
			
			/* Is it a colour specifier? */
			if (*c && (*c < TERM_L_UMBER + 1))
			{
				/* Save the new colour */
				a = *c - 1;
				c++;
				
				/* Paranoia: no more in string? */
				if (!(*c)) return;
				
				/* Hack -- fake monochrome */
				if (!use_color || ironman_moria) a = TERM_WHITE;
			}
		}
		
		/* Display the character */
		Term_putch(col, row, a, *c);
		
		/* Next position */
		col++;
		c++;
	}
}

/*
 * Put a string with formatting information at a given location
 */
void put_fstr(int col, int row, cptr str, ...)
{
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, str);

	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, str, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Display */
	put_cstr(col, row, buf);
}

/*
 * Display a string on the screen using an attribute, and clear
 * to the end of the line.
 */
void c_prt(byte attr, cptr str, int col, int row)
{
	/* Hack -- fake monochrome */
	if (!use_color || ironman_moria) attr = TERM_WHITE;

	/* Clear line, position cursor */
	Term_erase(col, row, 255);

	/* Dump the attr/text */
	Term_addstr(-1, attr, str);
}


/*
 * As above, but in "white"
 */
void prt(cptr str, int col, int row)
{
	/* Spawn */
	c_prt(TERM_WHITE, str, col, row);
}


/*
 * Print some (colored) text to the screen at the current cursor position,
 * automatically "wrapping" existing text (at spaces) when necessary to
 * avoid placing any text into the last column, and clearing every line
 * before placing any text in that line.  Also, allow "newline" to force
 * a "wrap" to the next line.  Advance the cursor as needed so sequential
 * calls to this function will work correctly.
 *
 * Once this function has been called, the cursor should not be moved
 * until all the related "c_roff()" calls to the window are complete.
 *
 * This function will correctly handle any width up to the maximum legal
 * value of 256, though it works best for a standard 80 character width.
 */
void c_roff(byte a, cptr str)
{
	int x, y;

	int w, h;

	cptr s;


	/* Hack -- fake monochrome */
	if (!use_color || ironman_moria) a = TERM_WHITE;


	/* Obtain the size */
	(void)Term_get_size(&w, &h);

	/* Obtain the cursor */
	(void)Term_locate(&x, &y);

	/* Process the string */
	for (s = str; *s; s++)
	{
		char ch;

		/* Force wrap */
		if (*s == '\n')
		{
			/* Wrap */
			x = 0;
			y++;

			/* Clear line, move cursor */
			Term_erase(x, y, 255);

			continue;
		}

		/* Clean up the char */
		ch = (isprint(*s) ? *s : ' ');

		/* Wrap words as needed */
		if ((x >= w - 1) && (ch != ' '))
		{
			int i, n = 0;

			byte av[256];
			char cv[256];

			/* Wrap word */
			if (x < w)
			{
				/* Scan existing text */
				for (i = w - 2; i >= 0; i--)
				{
					/* Grab existing attr/char */
					(void)Term_what(i, y, &av[i], &cv[i]);

					/* Break on space */
					if (cv[i] == ' ') break;

					/* Track current word */
					n = i;
				}
			}

			/* Special case */
			if (n == 0) n = w;

			/* Clear line */
			Term_erase(n, y, 255);

			/* Wrap */
			x = 0;
			y++;

			/* Clear line, move cursor */
			Term_erase(x, y, 255);

			/* Wrap the word (if any) */
			for (i = n; i < w - 1; i++)
			{
				/* Dump */
				Term_addch(av[i], cv[i]);

				/* Advance (no wrap) */
				if (++x > w) x = w;
			}
		}

		/* Dump */
		Term_addch(a, ch);

		/* Advance */
		if (++x > w) x = w;
	}
}

/*
 * As above, but in "white"
 */
void roff(cptr str)
{
	/* Spawn */
	c_roff(TERM_WHITE, str);
}


/*
 * Clear part of the screen
 */
void clear_from(int row)
{
	int y;

	/* Erase requested rows */
	for (y = row; y < Term->hgt; y++)
	{
		/* Erase part of the screen */
		Term_erase(0, y, 255);
	}
}




/*
 * Get some input at the cursor location.
 * Assume the buffer is initialized to a default string.
 * Note that this string is often "empty" (see below).
 * The default buffer is displayed in yellow until cleared.
 * Pressing RETURN right away accepts the default entry.
 * Normal chars clear the default and append the char.
 * Backspace clears the default or deletes the final char.
 * ESCAPE clears the buffer and the window and returns FALSE.
 * RETURN accepts the current buffer contents and returns TRUE.
 *
 * Note that 'len' refers to the size of the buffer.  The maximum length
 * of the input is 'len-1'.
 */
bool askfor_aux(char *buf, int len)
{
	int y, x;

	int i = 0;

	int k = 0;

	bool done = FALSE;

	/* Locate the cursor */
	(void)Term_locate(&x, &y);


	/* Paranoia -- check len */
	if (len < 1) len = 1;

	/* Paranoia -- check column */
	if ((x < 0) || (x >= 80)) x = 0;

	/* Restrict the length */
	if (x + len > 80) len = 80 - x;


	/* Paranoia -- Clip the default entry */
	buf[len - 1] = '\0';


	/* Display the default answer */
	Term_erase(x, y, len);

	/* Fake monochrome */
	if (!use_color || ironman_moria)
	{
		Term_putstr(x, y, -1, TERM_WHITE, buf);
	}
	else
	{
		Term_putstr(x, y, -1, TERM_YELLOW, buf);
	}

	/* Process input */
	while (!done)
	{
		/* Place cursor */
		Term_gotoxy(x + k, y);

		/* Get a key */
		i = inkey();

		/* Analyze the key */
		switch (i)
		{
			case ESCAPE:
				k = 0;
				done = TRUE;
				break;

			case '\n':
			case '\r':
				k = strlen(buf);
				done = TRUE;
				break;

			case 0x7F:
			case '\010':
				if (k > 0) k--;
				break;

			default:
				if ((k < len - 1) && (isprint(i)))
				{
					buf[k++] = i;
				}
				else
				{
					bell("Illegal edit key!");
				}
				break;
		}

		/* Terminate */
		buf[k] = '\0';

		/* Update the entry */
		Term_erase(x, y, len);
		Term_putstr(x, y, -1, TERM_WHITE, buf);
	}

	/* Aborted */
	if (i == ESCAPE) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Get a string from the user
 *
 * The "prompt" should take the form "Prompt: "
 *
 * Note that the initial contents of the string is used as
 * the default response, so be sure to "clear" it if needed.
 *
 * We clear the input, and return FALSE, on "ESCAPE".
 */
bool get_string(cptr prompt, char *buf, int len)
{
	bool res;

	/* Paranoia XXX XXX XXX */
	message_flush();

	/* Display prompt */
	prt(prompt, 0, 0);

	/* Ask the user for a string */
	res = askfor_aux(buf, len);

	/* Clear prompt */
	prt("", 0, 0);

	/* Result */
	return (res);
}


/*
 * Verify something with the user
 *
 * The "prompt" should take the form "Query? "
 *
 * Note that "[y/n]" is appended to the prompt.
 */
bool get_check(cptr prompt)
{
	int i;

	char buf[80];

	/* Do not skip */
	p_ptr->skip_more = FALSE;

	/* Paranoia XXX XXX XXX */
	message_flush();

	/* Hack -- Build a "useful" prompt */
	(void)strnfmt(buf, 78, "%.70s[y/n] ", prompt);

	/* Prompt for it */
	prt(buf, 0, 0);

	/* Get an acceptable answer */
	while (TRUE)
	{
		i = inkey();
		if (quick_messages) break;
		if (i == ESCAPE) break;
		if (strchr("YyNn", i)) break;
		bell("Illegal response to a 'yes/no' question!");
	}

	/* Erase the prompt */
	prt("", 0, 0);

	/* Normal negation */
	if ((i != 'Y') && (i != 'y')) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Prompts for a keypress
 *
 * The "prompt" should take the form "Command: "
 *
 * Returns TRUE unless the character is "Escape"
 */
bool get_com(cptr prompt, char *command)
{
	/* Paranoia XXX XXX XXX */
	message_flush();

	/* Display a prompt */
	prt(prompt, 0, 0);

	/* Get a key */
	*command = inkey();

	/* Clear the prompt */
	prt("", 0, 0);

	/* Handle "cancel" */
	if (*command == ESCAPE) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Request a "quantity" from the user
 *
 * Hack -- allow "command_arg" to specify a quantity
 */
s16b get_quantity(cptr prompt, int max)
{
	int amt;

	char tmp[80];

	char buf[80];


	/* Use "command_arg" */
	if (p_ptr->command_arg)
	{
		/* Extract a number */
		amt = p_ptr->command_arg;

		/* Clear "command_arg" */
		p_ptr->command_arg = 0;

		/* Enforce the maximum */
		if (amt > max) amt = max;

		/* Use it */
		return (amt);
	}

	/* Get the item index */
	if ((max != 1) && repeat_pull(&amt))
	{
		/* Enforce the maximum */
		if (amt > max) amt = max;

		/* Enforce the minimum */
		if (amt < 0) amt = 0;

		/* Use it */
		return (amt);
	}

	/* Build a prompt if needed */
	if (!prompt)
	{
		/* Build a prompt */
		sprintf(tmp, "Quantity (1-%d): ", max);

		/* Use that prompt */
		prompt = tmp;
	}


	/* Default to one */
	amt = 1;

	/* Build the default */
	sprintf(buf, "%d", amt);

	/* Ask for a quantity */
	if (!get_string(prompt, buf, 7)) return (0);

	/* Extract a number */
	amt = atoi(buf);

	/* A letter means "all" */
	if (isalpha(buf[0])) amt = max;

	/* Enforce the maximum */
	if (amt > max) amt = max;

	/* Enforce the minimum */
	if (amt < 0) amt = 0;

	if (amt) repeat_push(amt);

	/* Return the result */
	return (amt);
}


/*
 * Pause for user response XXX XXX XXX
 */
void pause_line(int row)
{
	int i;
	prt("", 0, row);
	put_str("[Press any key to continue]", 23, row);
	i = inkey();
	prt("", 0, row);
}


/*
 * GH
 * Called from cmd4.c and a few other places. Just extracts
 * a direction from the keymap for ch (the last direction,
 * in fact) byte or char here? I'm thinking that keymaps should
 * generally only apply to single keys, which makes it no more
 * than 128, so a char should suffice... but keymap_act is 256...
 */
int get_keymap_dir(char ch)
{
	cptr act, s;
	int d = 0;

	if (isdigit(ch))
	{
		d = D2I(ch);
	}
	else
	{
		if (rogue_like_commands)
		{
			act = keymap_act[KEYMAP_MODE_ROGUE][(byte)ch];
		}
		else
		{
			act = keymap_act[KEYMAP_MODE_ORIG][(byte)ch];
		}

		if (act)
		{
			/* Convert to a direction */
			for (s = act; *s; ++s)
			{
				/* Use any digits in keymap */
				if (isdigit(*s)) d = D2I(*s);
			}
		}
	}
	return d;
}

