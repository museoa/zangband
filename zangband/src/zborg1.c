/* File: borg1.c */
/* Purpose: Low level stuff for the Borg -BEN- */
#include "angband.h"


#ifdef ALLOW_BORG

#include "zborg1.h"

/*
 * using 2 rings of Light Res.
 * does not know anti-teleport.
 * if a borg has no light and no money to buy fuel, sell some
 *    in order to raise cash.
 * Zborg.txt needs support for quest numbers on respawn
 *    and verify the quest numbers on from respawn.
 *    I dont know if the same random Quests are generated
 *	  for each respawned borg, if they are reset, or even
 *    marked as done.  Might need to watch it.
 * Inscription problem with long art names
 * learning - of prayers.
 *
 * Mimic Doors.
 *
 * mindcrafters need to be able to sell non ps-id items if they
 *   are low level and do not have ID or ps-id.
 * getting to a wilderness grid that has a town.
 *   using waypoints?
 * Using dimdoor to get accross lava and water.
 * Mutations can be activated.
 */

/* Needs code:
 *    _QUESTITEM
 * TR3_NO_TELE
 * TR3_NO_MAGIC
 * TR3_TY_CURSE
 */

/* Things that would be nice for the borg to know but would
 * require a bit of programming and may not be worth the time.
 *
 * - Aquatic animals stay in aquatic realms.
 * - Wraithform
 * - Summoning
 * - Fixed Quests
 */

/*
 * This file contains various low level variables and routines.
 */

/* dynamic borg stuff */
bool borg_plays_risky;
bool borg_scums_uniques;

/* HACK... this should really be a parm into borg_prepared */
/*         I am just being lazy */
bool borg_slow_return = FALSE;

int *borg_has;
int *borg_skill;
int size_class;
int size_depth;
int size_obj;

/*
 * Some variables
 */

bool borg_active;	/* Actually active */
bool borg_resurrect = FALSE;	/* continous play mode */

bool borg_cancel;	/* Being cancelled */

bool borg_stop_king = TRUE;
bool borg_dont_react = FALSE;
int successful_target = 0;

/*
 * Various silly flags
 */

bool borg_flag_save = TRUE;	/* Save savefile at each level */
bool borg_flag_dump = FALSE;	/* Save savefile at each death */
bool borg_save = FALSE;	/* do a save next level */
bool borg_confirm_target = FALSE;	/* emergency spell use */

/*
 * Use a simple internal random number generator
 */


u32b borg_rand_local;	/* Save personal setting */


/*
 * Hack -- Time variables
 */

s32b borg_t = 0L;	/* Current "time" */
s32b need_see_inviso = 0;	/* cast this when required */
s32b borg_see_inv = 0;
bool vault_on_level;	/* Borg will search for a vault */
bool unique_on_level;
bool scaryguy_on_level;	/* flee from certain guys */

bool breeder_level = FALSE;	/* Borg will shut door */
s16b old_depth = 128;
s16b borg_no_retreat = 0;

/*
 * Hack -- Other time variables
 */

s32b when_call_lite = 0;	/* When we last did call light */
s32b when_wizard_lite = 0;	/* When we last did wizard light */

s32b when_detect_traps = 0;	/* When we last detected traps */
s32b when_detect_doors = 0;	/* When we last detected doors */
s32b when_detect_walls = 0;	/* When we last detected walls */
s32b when_detect_evil = 0;	/* When we last detected monsters or evil */

bool my_need_alter;	/* incase i hit a wall or door */
bool my_no_alter;	/*  */
bool borg_attempting_refresh = FALSE;	/* for the goi spell */

/*
 * Some information
 */

s16b goal;	/* Goal type */

bool goal_rising;	/* Currently returning to town */

bool goal_leaving;	/* Currently leaving the level */

bool goal_fleeing;	/* Currently fleeing the level */

bool goal_ignoring;	/* Currently ignoring monsters */

int goal_recalling;	/* Currently waiting for recall, guessing the turns left */

bool goal_less;	/* return to, but dont use, the next up stairs */

s16b borg_times_twitch;	/* how often twitchy on this level */
s16b borg_escapes;	/* how often teleported on this level */

bool stair_less;	/* Use the next "up" staircase */
bool stair_more;	/* Use the next "down" staircase */

s32b borg_began;	/* When this level began */
s32b borg_time_town;	/* how long it has been since I was in town */

s16b avoidance = 0;	/* Current danger thresh-hold */

bool borg_failure;	/* Notice failure */

bool borg_simulate;	/* Simulation flag */
bool borg_attacking;	/* Simulation flag */
bool borg_offsetting;	/* offset ball attacks */

bool borg_completed;	/* Completed the level */

bool borg_needs_searching;	/* borg will search with each step */
bool borg_full_damage;	/* make danger = full possible damage. */

/* defence flags */
bool borg_prot_from_evil;
bool borg_speed;
bool borg_bless;
bool borg_hero;
bool borg_berserk;
bool my_oppose_fire;
bool my_oppose_cold;
bool my_oppose_acid;
bool my_oppose_pois;
bool my_oppose_elec;
s16b borg_goi;
s16b borg_inviso;
bool borg_esp;

s16b borg_game_ratio;	/* the ratio of borg time to game time */

bool borg_shield;
bool borg_on_glyph;	/* borg is standing on a glyph of warding */
bool borg_create_door;	/* borg is going to create doors */
bool borg_sleep_spell;
bool borg_sleep_spell_ii;
bool borg_slow_spell;	/* borg is about to cast the spell */
bool borg_confuse_spell;
bool borg_fear_mon_spell;

/*
 * Current shopping information
 */

s16b goal_shop = -1;	/* Next shop to visit */



/* Current "shops" */
borg_shop *borg_shops;

s16b track_shop_num;
s16b track_shop_size;

/*
 * Hack -- current shop index
 */
s16b shop_num = -1;



/*
 * Location variables
 */

int c_x;	/* Current location (X) */
int c_y;	/* Current location (Y) */

int g_x;	/* Goal location (X) */
int g_y;	/* Goal location (Y) */

int dim_door_y;	/* Safe landing zone for DDoor */
int dim_door_x;

/* BIG HACK! Assume only 50 cursed artifacts */
int bad_obj_x[50];	/* Dropped cursed artifact at location (X) */
int bad_obj_y[50];	/* Dropped cursed artifact at location (Y) */

/*
 * Some estimated state variables
 */

s16b my_stat_max[6];	/* Current "maximal" stat values */
s16b my_stat_cur[6];	/* Current "natural" stat values */
s16b my_stat_use[6];	/* Current "resulting" stat values */
s16b my_stat_ind[6];	/* Current "additions" to stat values */
bool my_need_stat_check[6];	/* do I need to check my stats? */

s16b my_stat_add[6];	/* additions to stats  This will allow upgrading of */
					  /* equipment to allow a ring of int +4 to be traded */
					  /* for a ring of int +6 even if maximized to allow a */
					  /* later swap to be better. */

s16b home_stat_add[6];

bool borg_wearing_cursed;

int my_ammo_tval;	/* Ammo -- "tval" */
s16b my_ammo_power;	/* Average power */
s16b my_ammo_range;	/* Shooting range */

s16b my_need_enchant_to_a;	/* Need some enchantment */
s16b my_need_enchant_to_h;	/* Need some enchantment */
s16b my_need_enchant_to_d;	/* Need some enchantment */


/*
 * Various "amounts" (for the player)
 */

s16b amt_phase;
s16b amt_food_hical;
s16b amt_food_lowcal;

s16b amt_slow_poison;
s16b amt_cure_confusion;
s16b amt_cure_blind;

s16b amt_book[8][4];	/* [realm][sval] */

s16b amt_add_stat[6];
s16b amt_fix_stat[7];	/* #7 is to fix all stats */
s16b amt_fix_exp;

s16b amt_cool_staff;	/* holiness - power staff */

s16b amt_enchant_to_a;
s16b amt_enchant_to_d;
s16b amt_enchant_to_h;
s16b amt_brand_weapon;	/* apw brand bolts */
s16b amt_enchant_weapon;
s16b amt_enchant_armor;
s16b amt_digger;


/*
 * Hack -- extra state variables
 */

int borg_feeling = 0;	/* Current level "feeling" */


/*
 * State variables extracted from the screen
 */

s32b borg_exp;	/* Current experience */

s32b borg_gold;	/* Current gold */

int borg_stat[6];	/* Current stat values */

int borg_book[8][4];	/* Current book slots, [realm][sval] */


/*
 * Constant state variables
 */

int borg_race;	/* Player race */
int borg_class;	/* Player class */


/*
 * Hack -- access the class/race records
 */

player_race *rb_ptr;	/* Player race info */
player_class *cb_ptr;	/* Player class info */

player_magic *pmb_ptr;	/* Player magic info */



/*
 * Number of turns to step for (zero means forever)
 */
u16b borg_step = 0;	/* Step count (if any) */

/*
 * Status message search string
 */
char borg_match[128] = "";

/*
 * Log file
 */
FILE *borg_fff = NULL;	/* Log file */


/*
 * Track "stairs up"
 */
s16b track_less_num;
s16b track_less_size;
int *track_less_x;
int *track_less_y;


/*
 * Track "stairs down"
 */
s16b track_more_num;
s16b track_more_size;
int *track_more_x;
int *track_more_y;

/*
 * Track glyphs
 */
s16b track_glyph_num;
s16b track_glyph_size;
int *track_glyph_x;
int *track_glyph_y;

/*
 * Track Steps
 */
s16b track_step_num;
s16b track_step_size;
int *track_step_x;
int *track_step_y;

/*
 * Track closed doors
 */
s16b track_door_num;
s16b track_door_size;
int *track_door_x;
int *track_door_y;

/*
 * The object list.  This list is used to "track" objects.
 */
s16b borg_takes_cnt;
s16b borg_takes_nxt;
borg_take *borg_takes;


/*
 * The monster list.  This list is used to "track" monsters.
 */
s16b borg_kills_cnt;
s16b borg_kills_nxt;
borg_kill *borg_kills;

/*
 * Hack -- count racial appearances per level
 */
s16b *borg_race_count;


/*
 * Maintain a set of grids marked as "BORG_GLOW"
 */

s16b borg_glow_n = 0;

s16b borg_glow_x[AUTO_LITE_MAX];
s16b borg_glow_y[AUTO_LITE_MAX];


/*
 * Maintain a set of grids marked as "BORG_VIEW"
 */

s16b borg_view_n = 0;

s16b borg_view_x[AUTO_VIEW_MAX];
s16b borg_view_y[AUTO_VIEW_MAX];


/*
 * Maintain a temporary set of grids
 */

s16b borg_temp_n = 0;

s16b borg_temp_x[AUTO_TEMP_MAX];
s16b borg_temp_y[AUTO_TEMP_MAX];

/*
 * Maintain a circular queue of grids
 */

s16b borg_flow_n = 0;

s16b borg_flow_x[AUTO_FLOW_MAX];
s16b borg_flow_y[AUTO_FLOW_MAX];


/*
 * Hack -- use "flow" array as a queue
 */

int flow_head = 0;
int flow_tail = 0;



/*
 * Strategy flags -- examine the world
 */
bool borg_do_frame = TRUE;	/* Acquire "frame" info */

bool borg_do_spell = TRUE;	/* Acquire "spell" info */


/*
 * Strategy flags -- run certain functions
 */

bool borg_do_crush_junk = FALSE;

bool borg_do_crush_hole = FALSE;

bool borg_do_crush_slow = FALSE;

/* am I fighting a unique? */
int borg_fighting_unique;
bool borg_fighting_evil_unique;



/*
 * Query the "attr/char" at a given location on the screen
 * We return "zero" if the given location was legal
 *
 * XXX XXX XXX We assume the given location is legal
 */
errr borg_what_char(int x, int y, byte *a, char *c)
{
	/* Direct access XXX XXX XXX */
	(*a) = (Term->scr->a[y][x]);
	(*c) = (Term->scr->c[y][x]);

	/* Success */
	return (0);
}


/*
 * Query the "attr/chars" at a given location on the screen
 *
 * Note that "a" points to a single "attr", and "s" to an array
 * of "chars", into which the attribute and text at the given
 * location are stored.
 *
 * We will not grab more than "ABS(n)" characters for the string.
 * If "n" is "positive", we will grab exactly "n" chars, or fail.
 * If "n" is "negative", we will grab until the attribute changes.
 *
 * We automatically convert all "blanks" and "invisible text" into
 * spaces, and we ignore the attribute of such characters.
 *
 * We do not strip final spaces, so this function will very often
 * read characters all the way to the end of the line.
 *
 * We succeed only if a string of some form existed, and all of
 * the non-space characters in the string have the same attribute,
 * and the string was long enough.
 *
 * XXX XXX XXX We assume the given location is legal
 */
errr borg_what_text(int x, int y, int n, byte *a, char *s)
{
	int i;

	byte t_a;
	char t_c;

	byte *aa;
	char *cc;

	/* Current attribute */
	byte d_a = 0;

	/* Max length to scan for */
	int m = ABS(n);

	/* Hack -- Do not run off the screen */
	if (x + m > 80) m = 80 - x;

	/* Direct access XXX XXX XXX */
	aa = &(Term->scr->a[y][x]);
	cc = &(Term->scr->c[y][x]);

	/* Grab the string */
	for (i = 0; i < m; i++)
	{
		/* Access */
		t_a = *aa++;
		t_c = *cc++;

		/* Handle spaces */
		if ((t_c == ' ') || !t_a)
		{
			/* Save space */
			s[i] = ' ';
		}

		/* Handle real text */
		else
		{
			/* Attribute ready */
			if (d_a)
			{
				/* Verify the "attribute" (or stop) */
				if (t_a != d_a) break;
			}

			/* Acquire attribute */
			else
			{
				/* Save it */
				d_a = t_a;
			}

			/* Save char */
			s[i] = t_c;
		}
	}

	/* Terminate the string */
	s[i] = '\0';

	/* Save the attribute */
	(*a) = d_a;

	/* Too short */
	if ((n > 0) && (i != n)) return (1);

	/* Success */
	return (0);
}


/*
 * Log a message to a file
 */
void borg_info(cptr what)
{
	/* Dump a log file message */
	if (borg_fff) fprintf(borg_fff, "%s\n", what);
}



/*
 * Memorize a message, Log it, Search it, and Display it in pieces
 */
void borg_note(cptr what)
{
	int j, n, i, k;

	int w, h, x, y;


	term *old = Term;

	/* Memorize it */
	message_add(what, MSG_GENERIC);


	/* Log the message */
	borg_info(what);


	/* Mega-Hack -- Check against the search string */
	if (borg_match[0] && strstr(what, borg_match))
	{
		/* Clean cancel */
		borg_cancel = TRUE;
	}

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		if (!angband_term[j]) continue;

		/* Check flag */
		if (!(window_flag[j] & PW_BORG_1)) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Access size */
		Term_get_size(&w, &h);

		/* Access cursor */
		Term_locate(&x, &y);

		/* Erase current line */
		Term_erase(0, y, 255);


		/* Total length */
		n = strlen(what);

		/* Too long */
		if (n > w - 2)
		{
			char buf[1024];

			/* Split */
			while (n > w - 2)
			{
				/* Default */
				k = w - 2;

				/* Find a split point */
				for (i = w / 2; i < w - 2; i++)
				{
					/* Pre-emptive split point */
					if (isspace(what[i])) k = i;
				}

				/* Copy over the split message */
				for (i = 0; i < k; i++)
				{
					/* Copy */
					buf[i] = what[i];
				}

				/* Indicate split */
				buf[i++] = '\\';

				/* Terminate */
				buf[i] = '\0';

				/* Show message */
				Term_addstr(-1, TERM_WHITE, buf);

				/* Advance (wrap) */
				if (++y >= h) y = 0;

				/* Erase next line */
				Term_erase(0, y, 255);

				/* Advance */
				what += k;

				/* Reduce */
				n -= k;
			}

			/* Show message tail */
			Term_addstr(-1, TERM_WHITE, what);

			/* Advance (wrap) */
			if (++y >= h) y = 0;

			/* Erase next line */
			Term_erase(0, y, 255);
		}

		/* Normal */
		else
		{
			/* Show message */
			Term_addstr(-1, TERM_WHITE, what);

			/* Advance (wrap) */
			if (++y >= h) y = 0;

			/* Erase next line */
			Term_erase(0, y, 255);
		}


		/* Flush output */
		Term_fresh();

		/* Use correct window */
		Term_activate(old);
	}
}


/*
 * borg_note() , but with formatting
 */
void borg_note_fmt(cptr fmt, ...)
{
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Display */
	borg_note(buf);
}


/*
 * Abort the Borg, noting the reason
 */
void borg_oops(cptr what)
{
	char buf[1024];

	/* Stop processing */
	borg_active = FALSE;
	
	/* Format the string */
	(void)strnfmt(buf, 1024, "# Aborting (%s).", what);

	/* Give a warning */
	borg_note(buf);

	/* Forget borg keys */
	borg_flush();
}

/*
 * borg_oops() , but with formatting
 */
void borg_oops_fmt(cptr fmt, ...)
{
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Display */
	borg_oops(buf);
}


/*
 * A Queue of keypresses to be sent
 */
static char *borg_key_queue;
static s16b borg_key_head;
static s16b borg_key_tail;


/*
 * Add a keypress to the "queue" (fake event)
 */
errr borg_keypress(char k)
{
	char buf[10];

	/* Hack -- Refuse to enqueue "nul" */
	if (!k) return (-1);

	/* Format the string */
	(void)strnfmt(buf, 10, "& Key <%c>", k);

	/* Hack -- note the keypress */
	if (borg_fff) borg_info(buf);

	/* Store the char, advance the queue */
	borg_key_queue[borg_key_head++] = k;

	/* Circular queue, handle wrap */
	if (borg_key_head == KEY_SIZE) borg_key_head = 0;

	/* Hack -- Catch overflow (forget oldest) */
	if (borg_key_head == borg_key_tail) borg_oops("overflow");

	/* Hack -- Overflow may induce circular queue */
	if (borg_key_tail == KEY_SIZE) borg_key_tail = 0;

	/* Success */
	return (0);
}


/*
 * Add a keypress to the "queue" (fake event)
 */
errr borg_keypresses(cptr str)
{
	cptr s;

	/* Enqueue them */
	for (s = str; *s; s++) borg_keypress(*s);

	/* Success */
	return (0);
}


/*
 * Get the next Borg keypress
 */
char borg_inkey(bool take)
{
	int i;

	/* Nothing ready */
	if (borg_key_head == borg_key_tail)
		return (0);

	/* Extract the keypress */
	i = borg_key_queue[borg_key_tail];

	/* Do not advance */
	if (!take) return (i);

	/* Advance the queue */
	borg_key_tail++;

	/* Circular queue requires wrap-around */
	if (borg_key_tail == KEY_SIZE) borg_key_tail = 0;

	/* Return the key */
	return (i);
}



/*
 * Get the next Borg keypress
 */
void borg_flush(void)
{
	/* Simply forget old keys */
	borg_key_tail = borg_key_head;
}






/*
 * Hack -- take a note later
 */
bool borg_tell(cptr what)
{
	cptr s;

	/* Hack -- self note */
	borg_keypress(':');
	for (s = what; *s; s++) borg_keypress(*s);
	borg_keypress('\n');

	/* Success */
	return (TRUE);
}



/*
 * Attempt to change the borg's name
 */
bool borg_change_name(cptr str)
{
	cptr s;

	/* Cancel everything */
	borg_keypress(ESCAPE);
	borg_keypress(ESCAPE);

	/* Character description */
	borg_keypress('C');

	/* Change the name */
	borg_keypress('c');

	/* Enter the new name */
	for (s = str; *s; s++) borg_keypress(*s);

	/* End the name */
	borg_keypress('\r');

	/* Cancel everything */
	borg_keypress(ESCAPE);
	borg_keypress(ESCAPE);

	/* Success */
	return (TRUE);
}


/*
 * Attempt to dump a character description file
 */
bool borg_dump_character(cptr str)
{
	cptr s;

	/* Cancel everything */
	borg_keypress(ESCAPE);
	borg_keypress(ESCAPE);

	/* Character description */
	borg_keypress('C');

	/* Dump character file */
	borg_keypress('f');

	/* Enter the new name */
	for (s = str; *s; s++) borg_keypress(*s);

	/* End the file name */
	borg_keypress('\r');

	/* Cancel everything */
	borg_keypress(ESCAPE);
	borg_keypress(ESCAPE);

	/* Success */
	return (TRUE);
}




/*
 * Attempt to save the game
 */
bool borg_save_game(void)
{
	/* Cancel everything */
	borg_keypress(ESCAPE);
	borg_keypress(ESCAPE);

	/* Save the game */
	borg_keypress('^');
	borg_keypress('S');

	/* Cancel everything */
	borg_keypress(ESCAPE);
	borg_keypress(ESCAPE);

	/* Success */
	return (TRUE);
}




/*
 * Update the Borg based on the current "frame"
 *
 * Assumes the Borg is actually in the dungeon.
 */
void borg_update_frame(void)
{
	int i;

	s32b len = 10L * TOWN_DAWN;
	s32b tick = turn % len + len / 4;

	int hour = (24 * tick / len) % 24;
	int min = (1440 * tick / len) % 60;

	borg_skill[BI_HRTIME] = hour;
	borg_skill[BI_MNTIME] = min;

	/* Assume level is fine */
	borg_skill[BI_ISFIXLEV] = FALSE;

	/* Note "Lev" vs "LEV" */
	if (p_ptr->lev < p_ptr->max_lev) borg_skill[BI_ISFIXLEV] = TRUE;

	/* Extract "LEVEL xxxxxx" */
	borg_skill[BI_CLEVEL] = borg_skill[BI_CLEVEL] = p_ptr->lev;

	/* cheat the max clevel */
	borg_skill[BI_MAXCLEVEL] = p_ptr->max_lev;

	/* Note "Winner" */
	borg_skill[BI_KING] = p_ptr->total_winner;

	/* Assume experience is fine */
	borg_skill[BI_ISFIXEXP] = FALSE;

	/* Note "Exp" vs "EXP" and am I lower than level 50 */
	if ((p_ptr->exp < p_ptr->max_exp) &&
		(borg_skill[BI_CLEVEL] != 50)) borg_skill[BI_ISFIXEXP] = TRUE;

	/* Extract "EXP xxxxxxxx" */
	borg_exp = p_ptr->exp;


	/* Extract "AU xxxxxxxxx" */
	borg_gold = p_ptr->au;


	/* Extract "Fast (+x)" or "Slow (-x)" */
	borg_skill[BI_SPEED] = p_ptr->pspeed;

	/* Check my float for decrementing variables */
	if (borg_skill[BI_SPEED] > 110)
	{
		borg_game_ratio = 100000 / (((borg_skill[BI_SPEED] - 110) * 10) + 100);
	}
	else
	{
		borg_game_ratio = 1000;
	}


	/* if hasting, it doesn't count as 'borg_speed'.  The speed */
	/* gained from hasting is counted seperately. */
	if (borg_speed)
		borg_skill[BI_SPEED] -= 10;

	/* Extract "Cur AC xxxxx" */
	borg_skill[BI_ARMOR] = p_ptr->dis_ac + p_ptr->dis_to_a;

	/* Extract "Cur HP xxxxx" */
	borg_skill[BI_CURHP] = p_ptr->chp;

	/* Extract "Max HP xxxxx" */
	borg_skill[BI_MAXHP] = p_ptr->mhp;

	/* Extract "Cur SP xxxxx" (or zero) */
	borg_skill[BI_CURSP] = p_ptr->csp;

	/* Extract "Max SP xxxxx" (or zero) */
	borg_skill[BI_MAXSP] = p_ptr->msp;

	/* Clear all the "state flags" */
	borg_skill[BI_ISWEAK] = borg_skill[BI_ISHUNGRY] = borg_skill[BI_ISFULL] =
		borg_skill[BI_ISGORGED] = FALSE;
	borg_skill[BI_ISBLIND] = borg_skill[BI_ISCONFUSED] =
		borg_skill[BI_ISAFRAID] = borg_skill[BI_ISPOISONED] = FALSE;
	borg_skill[BI_ISCUT] = borg_skill[BI_ISSTUN] = borg_skill[BI_ISHEAVYSTUN] =
		borg_skill[BI_ISIMAGE] = borg_skill[BI_ISSTUDY] = FALSE;
	borg_skill[BI_ISSEARCHING] = FALSE;

	/* Check for "Weak" */
	if (p_ptr->food < PY_FOOD_WEAK) borg_skill[BI_ISWEAK] =
			borg_skill[BI_ISHUNGRY] = TRUE;

	/* Check for "Hungry" */
	else if (p_ptr->food < PY_FOOD_ALERT) borg_skill[BI_ISHUNGRY] = TRUE;

	/* Check for "Normal" */
	else if (p_ptr->food < PY_FOOD_FULL) /* Nothing */ ;

	/* Check for "Full" */
	else if (p_ptr->food < PY_FOOD_MAX) borg_skill[BI_ISFULL] = TRUE;

	/* Check for "Gorged" */
	else
		borg_skill[BI_ISGORGED] = borg_skill[BI_ISFULL] = TRUE;

	/* Check for "Blind" */
	if (p_ptr->blind) borg_skill[BI_ISBLIND] = TRUE;

	/* Check for "Confused" */
	if (p_ptr->confused) borg_skill[BI_ISCONFUSED] = TRUE;

	/* Check for "Afraid" */
	if (p_ptr->afraid) borg_skill[BI_ISAFRAID] = TRUE;

	/* Check for "Poisoned" */
	if (p_ptr->poisoned) borg_skill[BI_ISPOISONED] = TRUE;

	/* Check for any text */
	if (p_ptr->cut) borg_skill[BI_ISCUT] = TRUE;

	/* Check for Stun */
	if (p_ptr->stun && (p_ptr->stun <= 50)) borg_skill[BI_ISSTUN] = TRUE;

	/* Check for Heavy Stun */
	if (p_ptr->stun > 50) borg_skill[BI_ISHEAVYSTUN] = TRUE;

	/* XXX XXX XXX Parse "State" */
	if (p_ptr->searching) borg_skill[BI_ISSEARCHING] = TRUE;

	/* Check for "Study" */
	if (p_ptr->new_spells) borg_skill[BI_ISSTUDY] = TRUE;

	/* Parse stats */
	for (i = 0; i < 6; i++)
	{
		borg_skill[BI_ISFIXSTR + i] =
			p_ptr->stat_cur[A_STR + i] < p_ptr->stat_max[A_STR + i];
		borg_skill[BI_CSTR + i] = p_ptr->stat_cur[A_STR + i];
		borg_stat[i] = p_ptr->stat_cur[i];

	}

	/* Hack -- Access max depth */
	borg_skill[BI_CDEPTH] = p_ptr->depth;

	/* Hack -- Access max depth */
	borg_skill[BI_MAXDEPTH] = p_ptr->max_depth;

	/* Hack -- Realms */
	borg_skill[BI_REALM1] = p_ptr->realm1;
	borg_skill[BI_REALM2] = p_ptr->realm2;

	/* Hack -- Mana increases */
	switch (borg_class)
	{
		case CLASS_PRIEST:
		case CLASS_PALADIN:
		case CLASS_MONK:
		case CLASS_MINDCRAFTER:
		{
			borg_skill[BI_WISMANA] = 1;
			break;
		}

		case CLASS_MAGE:
		case CLASS_ROGUE:
		case CLASS_RANGER:
		case CLASS_WARRIOR_MAGE:
		case CLASS_CHAOS_WARRIOR:
		case CLASS_HIGH_MAGE:
		{
			borg_skill[BI_INTMANA] = 1;
			break;
		}

		default:
		{
			borg_skill[BI_WISMANA] = 0;
			borg_skill[BI_INTMANA] = 0;
		}
	}
}


/*
 * Initialize this file
 */
void borg_init_1(void)
{
	/* Allocate the "keypress queue" */
	C_MAKE(borg_key_queue, KEY_SIZE, char);

	/* Prepare a local random number seed */
	if (!borg_rand_local) borg_rand_local = randint0(0x10000000);

	/*** Special "tracking" arrays ***/

	/* Track "up" stairs */
	track_less_num = 0;
	track_less_size = 16;
	C_MAKE(track_less_x, track_less_size, int);
	C_MAKE(track_less_y, track_less_size, int);

	/* Track "down" stairs */
	track_more_num = 0;
	track_more_size = 16;
	C_MAKE(track_more_x, track_more_size, int);
	C_MAKE(track_more_y, track_more_size, int);

	/* Track glyphs */
	track_glyph_num = 0;
	track_glyph_size = 256;
	C_MAKE(track_glyph_x, track_glyph_size, int);
	C_MAKE(track_glyph_y, track_glyph_size, int);

	/* Track Steps */
	track_step_num = 0;
	track_step_size = 256;
	C_MAKE(track_step_x, track_step_size, int);
	C_MAKE(track_step_y, track_step_size, int);

	/* Track closed doors */
	track_door_num = 0;
	track_door_size = 256;
	C_MAKE(track_door_x, track_door_size, int);
	C_MAKE(track_door_y, track_door_size, int);

	/*** Object tracking ***/

	/* No objects yet */
	borg_takes_cnt = 0;
	borg_takes_nxt = 1;

	/* Array of objects */
	C_MAKE(borg_takes, BORG_TAKES_MAX, borg_take);

	/*** Monster tracking ***/

	/* No monsters yet */
	borg_kills_cnt = 0;
	borg_kills_nxt = 1;

	/* Array of monsters */
	C_MAKE(borg_kills, BORG_KILLS_MAX, borg_kill);

	/*** Special counters ***/

	/* Count racial appearances */
	C_MAKE(borg_race_count, z_info->r_max, s16b);
}



#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
