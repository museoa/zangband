/* File: borg9.c */

/* Purpose: Highest level functions for the Borg -BEN- */
#include "angband.h"

#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg2.h"
#include "zborg3.h"
#include "zborg4.h"
#include "zborg5.h"
#include "zborg6.h"
#include "zborg7.h"
#include "zborg8.h"
#include "zborg9.h"

bool borg_cheat_death;

/*
 * This file implements the "APWBorg", an "Automatic Angband Player".
 *
 * This version of the "APWBorg" is designed for use with ZAngband 2.7.x.
 *
 * Use of the "APWBorg" requires re-compilation with ALLOW_BORG defined,
 * and with the various "zborg*.c" files linked into the executable.
 *
 * Note that you can only use the Borg if your character has been marked
 * as a "Borg User".  You can do this, if necessary, by responding "y"
 * when asked if you really want to use the Borg.  This will (normally)
 * result in your character being inelligible for the high score list.
 *
 * The "do_cmd_borg()" function, called when the user hits "^Z", allows
 * the user to interact with the Borg.  You do so by typing "Borg Commands",
 * including 'z' to activate (or re-activate), 'K' to show monsters, 'T' to
 * show objects, 'd' to toggle "demo mode", 'f' to open/shut the "log file",
 * 'i' to display internal flags, etc.  See "do_cmd_borg()" for more info.
 *
 * The first time you enter a Borg command, the Borg is initialized.  This
 * consists of three major steps, and requires at least 400K of free memory,
 * if the memory is not available, the game may abort.
 *
 * (1) The various "borg" modules are initialized.
 *
 * (2) Some important "state" information is extracted, including the level
 *     and race/class of the player, and some more initialization is done.
 *
 * (3) Some "historical" information (killed uniques, maximum dungeon depth)
 *     is "stolen" from the game.
 *
 * When the Borg is "activated", it uses the "Term_inkey_hook" to steal
 * control from the user.  Later, if it detects any input from the real user,
 * it gracefully relinquishes control by clearing the "Term_inkey_hook" after
 * any pending key-sequences are complete.
 *
 * The Borg will abort if it detects any "errors", or if it detects any
 * "situations" such as "death", or if it detects any "panic" situations,
 * such as "imminent death", if the appropriate flags are set.
 *
 * The Borg is only supposed to "know" what is visible on the screen,
 * which it learns by using the "term.c" screen access function "Term_what()",
 * the cursor location function "Term_locate()", and the cursor visibility
 * extraction function "Term_get_cursor()".
 *
 * The Borg is only supposed to "send" keypresses when the "Term_inkey()"
 * function asks for a keypress, which is accomplished by using a special
 * function hook in the "z-term.c" file, which allows the Borg to "steal"
 * control from the "Term_inkey()" and "Term_flush()" functions.  This
 * allows the Borg to pretend to be a normal user.
 *
 * Note that if properly designed, the Borg could be run as an external
 * process, which would actually examine the screen (or pseudo-terminal),
 * and send keypresses directly to the keyboard (or pseudo-terminal).  Thus
 * it should never access any "game variables", unless it is able to extract
 * those variables for itself by code duplication or complex interactions,
 * or, in certain situations, if those variables are not actually "required".
 *
 * Currently, the Ben Borg is a few steps away from being able to be run as
 * an external process, primarily in the "low level" details, such as knowing
 * when the game is ready for a keypress.  Also, the Ben Borg assumes that a
 * character has already been rolled, and maintains no state between saves,
 * which is partially offset by "cheating" to "acquire" the maximum dungeon
 * depth, without which equipment analysis will be faulty.
 *
 * The "theory" behind the Borg is that is should be able to run as a
 * separate process, playing Angband in a window just like a human, that
 * is, examining the screen for information, and sending keypresses to
 * the game.  The current Borg does not actually do this, because it would
 * be very slow and would not run except on Unix machines, but as far as
 * possible, I have attempted to make sure that the Borg *could* run that
 * way.  This involves "cheating" as little as possible, where "cheating"
 * means accessing information not available to a normal Angband player.
 * And whenever possible, this "cheating" should be optional, that is,
 * there should be software options to disable the cheating, and, if
 * necessary, to replace it with "complex" parsing of the screen.
 *
 * Thus, the Borg COULD be written as a separate process which runs Angband
 * in a pseudo-terminal and "examines" the "screen" and sends keypresses
 * directly (as with a terminal emulator), although it would then have
 * to explicitly "wait" to make sure that the game was completely done
 * sending information.
 *
 * The Borg is thus allowed to examine the screen directly (by efficient
 * direct access of the "Term->scr->a" and "Term->scr->c" arrays, which
 * could be replaced by calls to "Term_grab()"), and to access the cursor
 * location (via "Term_locate()") and visibility (via "Term_get_cursor()"),
 * and, as mentioned above, the Borg is allowed to send keypresses directly
 * to the game, and only when needed, using the "Term_inkey_hook" hook, and
 * uses the same hook to know when it should discard all pending keypresses.
 *
 * The Borg should not know when the game is ready for a keypress, and
 * should really do something nasty such as "pause" between turns for
 * some amount of time to ensure that the game is really waiting for
 * a keypress.
 *
 * Various other "cheats" (mostly optional) are described where they are
 * used, primarily in this file.
 *
 * Note that any "user input" will be ignored, and will cancel the Borg,
 * after the Borg has completed any key-sequences currently in progress.
 *
 * Note that the "borg_t" parameter bears a close resemblance to the number of
 * "player turns" that have gone by.  Except that occasionally, the Borg will
 * do something that he *thinks* will take time but which actually does not
 * (for example, attempting to open a hallucinatory door), and that sometimes,
 * the Borg performs a "repeated" command (rest, open, tunnel, or search),
 * which may actually take longer than a single turn.  This has the effect
 * that the "borg_t" variable is slightly lacking in "precision".
 *
 * Note that as of 2.7.9, the Borg can play any class, that is, he can make
 * "effective" use of at least a few spells/prayers, and is not "broken"
 * by low strength, blind-ness, hallucination, etc.  This does not, however,
 * mean that he plays all classes equally well, especially since he is so
 * dependant on a high strength for so many things.  The "demo" mode is useful
 * for many classes (especially Mage) since it allows the Borg to "die" a few
 * times, without being penalized.
 *
 * The Borg assumes that the "maximize" flag is off, and that the
 * "preserve" flag is on, since he cannot actually set those flags.
 * If the "maximize" flag is on, the Borg may not work correctly.
 * If the "preserve" flag is off, the Borg may miss artifacts.
 */


/*
 * We currently handle:
 *   Level changing (intentionally or accidentally)
 *   Embedded objects (gold) that must be extracted
 *   Ignore embedded objects if too "weak" to extract
 *   Traps (disarm), Doors (open/etc), Rubble (tunnel)
 *   Stores (enter via movement, exit via escape)
 *   Stores (limited commands, and different commands)
 *   Always deal with objects one at a time, not in piles
 *   Discard junk before trying to pick up more stuff
 *   Use "identify" to obtain knowledge and/or money
 *   Rely on "sensing" objects as much as possible
 *   Do not sell junk or worthless items to any shop
 *   Do not attempt to buy something without the cash
 *   Use the non-optimal stairs if stuck on a level
 *   Use "flow" code for all tasks for consistency
 *   Cancel all goals when major world changes occur
 *   Use the "danger" code to avoid potential death
 *   Use the "danger" code to avoid inconvenience
 *   Try to avoid danger (both actively and passively)
 *   Handle "Mace of Disruption", "Scythe of Slicing", etc
 *   Learn spells, and use them when appropriate
 *   Remember that studying prayers is slightly random
 *   Do not try to read scrolls when blind or confused
 *   Do not study/use spells/prayers when blind/confused
 *   Use spells/prayers at least once for the experience
 *   Attempt to heal when "confused", "blind", etc
 *   Attempt to fix "fear", "poison", "cuts", etc
 *   Analyze potential equipment in proper context
 *   Priests should avoid edged weapons (spell failure)
 *   Mages should avoid most gloves (lose mana)
 *   Non-warriors should avoid heavy armor (lose mana)
 *   Keep "best" ring on "tight" right finger in stores
 *   Remove items which do not contribute to total fitness
 *   Wear/Remove/Sell/Buy items in most optimal order
 *   Pursue optimal combination of available equipment
 *   Notice "failure" when using rods/staffs/artifacts
 *   Notice "failure" when attempting spells/prayers
 *   Attempt to correctly track terrain, objects, monsters
 *   Take account of "clear" and "multi-hued" monsters
 *   Take account of "flavored" (randomly colored) objects
 *   Handle similar objects/monsters (mushrooms, coins)
 *   Multi-hued/Clear monsters, and flavored objects
 *   Keep in mind that some monsters can move (quickly)
 *   Do not fire at monsters that may not actually exist
 *   Assume everything is an object until proven otherwise
 *   Parse messages to correct incorrect assumptions
 *   Search for secret doors after exploring the level
 *   React intelligently to changes in the wall structure
 *   Do not recalculate "flow" information unless required
 *   Collect monsters/objects/terrain not currently in view
 *   Keep in mind that word of recall is a delayed action
 *   Keep track of charging items (rods and artifacts)
 *   Be very careful not to access illegal locations!
 *   Do not rest next to dangerous (or immobile) monsters
 *   Recall into dungeon if prepared for resulting depth
 *   Do not attempt to destroy cursed ("terrible") artifacts
 *   Attempted destruction will yield "terrible" inscription
 *   Use "maximum" level and depth to prevent "thrashing"
 *   Use "maximum" hp's and sp's when checking "potentials"
 *   Attempt to recognize large groups of "disguised" monsters
 *   Beware of firing at a monster which is no longer present
 *   Stockpile items in the Home, and use those stockpiles
 *   Discounted spell-books (low level ones are ignored)
 *   Take items out of the home to sell them when no longer needed
 *   Trappers and Mimics (now treated as invisible monsters)
 *   Invisible monsters (induce "fear" of nearby regions)
 *   Fleeing monsters are "followed" down corridors and such
 *
 * We ignore:
 *   Long object descriptions may have clipped inscriptions
 *
 * We need to handle:
 *   Technically a room can have no exits, requiring digging
 *   Conserve memory space (each grid byte costs about 15K)
 *   Conserve computation time (especially with the flow code)
 *
 * We need to handle "loading" saved games:
 *   The "max_depth" value is lost if loaded in the town
 *   If we track "dead uniques" then this information is lost
 *   The "map" info, "flow" info, "tracking" info, etc is lost
 *   The contents of the shops (and the home) are lost
 *   We may be asked to "resume" a non-Borg character (icky)
 */


/*
 * Currently, the Borg "cheats" in a few situations...
 *
 * Cheats that are significant, and possibly unavoidable:
 *   Knowledge of when we are being asked for a keypress.
 *   Note that this could be avoided by LONG timeouts/delays
 *
 * Cheats "required" by implementation, but not signifant:
 *   Direct access to the "screen image" (parsing screen)
 *   Direct access to the "keypress queue" (sending keys)
 *   Direct access to the "cursor visibility" (game state)
 *
 * Cheats that could be avoided by simple (ugly) code:
 *   Direct modification of the "current options"
 *
 * Cheats that could be avoided by duplicating code:
 *   Use of the tables in "tables.c"
 *   Use of the arrays initialized in "init.c"
 *
 * Cheats that the Borg would love:
 *   Immunity to hallucination, blindness, confusion
 *   Unique attr/char codes for every monster and object
 *   Removal of the "mimic" and "trapper" monsters
 *   Removal of the "mushroom" and "gold" monsters
 */


/*
 * Stat advantages:
 *   High STR (attacks, to-dam, digging, weight limit)
 *   High DEX (attacks, to-hit, armor class)
 *   High CON (hitpoints, recovery)
 *   High WIS (better prayers, saving throws)
 *   High INT (better spells, disarming, device usage)
 *   High CHR (better item costs)
 *
 * Class advantages:
 *   Warrior (good fighting, sensing)
 *   Mage (good spells)
 *   Priest (good prayers, fighting)
 *   Ranger (some spells, fighting)
 *   Rogue (some spells, fighting, sensing)
 *   Paladin (prayers, fighting, sensing)
 *
 * Race advantages:
 *   Gnome (free action)
 *   Dwarf (resist blindness)
 *   High elf (see invisible)
 *   Non-human (infravision)
 */




/*
 * Some variables
 */

static bool initialized;	/* Hack -- Initialized */



/*
 * Mega-Hack -- extract some "hidden" variables
 *
 * XXX XXX XXX This step would not be necessary if more info
 * was available on the screen.
 *
 */
static void borg_hidden(void)
{
	int i;

	/* Clear the stat modifiers */
	for (i = 0; i < A_MAX; i++) my_stat_add[i] = 0;

	/* Scan the usable inventory */
	for (i = 0; i < equip_num; i++)
	{
		list_item *l_ptr = &equipment[i];

		/* Skip empty items */
		if (!l_ptr->k_idx) continue;

		/* Affect stats */
		if (l_ptr->kn_flags1 & TR1_STR) my_stat_add[A_STR] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_INT) my_stat_add[A_INT] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_WIS) my_stat_add[A_WIS] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_DEX) my_stat_add[A_DEX] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_CON) my_stat_add[A_CON] += l_ptr->pval;
		if (l_ptr->kn_flags1 & TR1_CHR) my_stat_add[A_CHR] += l_ptr->pval;
	}

	/* Mega-Hack -- Guess at "my_stat_cur[]" */
	for (i = 0; i < A_MAX; i++)
	{
		if (!my_need_stat_check[i]) continue;

		/* Hack - get current internal stat value */
		my_stat_cur[i] = p_ptr->stat_cur[i];

		/* Max stat is the max that the cur stat ever is. */
		if (my_stat_cur[i] > my_stat_max[i])
		{
			my_stat_max[i] = my_stat_cur[i];
		}
	}
}


/*
 * Think about the world and perform an action
 *
 * Check inventory/equipment/spells/panel once per "turn"
 *
 * Process "store" and other modes when necessary
 *
 * Technically, we should attempt to parse all the messages that
 * indicate that it is necessary to re-parse the books, and only
 * set the appropriate flags at that point.  This would not only
 * reduce the potential screen flashing, but would also optimize
 * the code a lot.  For paranoia, we could always select items
 * and spells using capital letters.
 */
static bool borg_think(void)
{
	int i, ii;

	byte t_a;

	char buf[128];
	static char svSavefile[1024];
	static bool justSaved = FALSE;


	/*** Process inventory/equipment ***/

	/* save now */
	if (borg_save && borg_save_game())
	{
		/* Log */
		borg_note("# Auto Save!");

		borg_save = FALSE;

		return (TRUE);
	}
	if (justSaved)
	{
		memcpy(savefile, svSavefile, sizeof(savefile));
		borg_save_game();
		justSaved = FALSE;
		return (TRUE);
	}

	/*** Find books ***/

	/* Only if needed */
	if (borg_do_spell)
	{
		/* Assume no books */
		for (ii = 0; ii < MAX_REALM; ii++)
		{
			for (i = 0; i < 4; i++) borg_book[ii][i] = -1;
		}

		/* Scan the pack */
		for (ii = 1; ii < MAX_REALM; ii++)
		{
			/* skip non my realms */
			if (ii != borg_skill[BI_REALM1] &&
				ii != borg_skill[BI_REALM2]) continue;

			for (i = 0; i < inven_num; i++)
			{
				list_item *l_ptr = &inventory[i];

				/* Skip wrong-realm books */
				if (l_ptr->tval != REALM1_BOOK &&
					l_ptr->tval != REALM2_BOOK) continue;

				/* Note book locations */
				borg_book[l_ptr->tval - TV_LIFE_BOOK +
						  1][k_info[l_ptr->k_idx].sval] = i;
			}
		}
	}

	/*** Process books ***/

	/* Hack -- Warriors never browse */
	if (borg_class == CLASS_WARRIOR) borg_do_spell = FALSE;

	/* Hack -- Blind or Confused prevents browsing */
	if (borg_skill[BI_ISBLIND] ||
		borg_skill[BI_ISCONFUSED]) borg_do_spell = FALSE;

	/* XXX XXX XXX Dark */

	/* Cheat */
	if (borg_do_spell)
	{
		/* Only do it once for each realm */
		borg_do_spell = FALSE;

		/* Cheat that realm */
		borg_cheat_spell(borg_skill[BI_REALM1]);
		borg_cheat_spell(borg_skill[BI_REALM2]);


		/* Done */
		return (FALSE);
	}

	/* If king, maybe retire. */
	if (borg_skill[BI_KING])
	{
		/* Prepare to retire */
		if (borg_stop_king)
		{
			borg_oops("retire");
		}
	}

	/*** Handle stores ***/

	/* Hack -- Check for being in a store */
	if ((0 == borg_what_text(3, 5, 16, &t_a, buf)) &&
		(streq(buf, "Item Description")))
	{
		/* Cheat the current gold (unless in home) */
		borg_gold = p_ptr->au;

		/* Hack -- allow user abort */
		if (borg_cancel) return (TRUE);

		/* Think until done */
		return (borg_think_store());
	}

	/*** Analyze the Frame ***/

	/* Analyze the frame */
	if (borg_do_frame)
	{
		/* Only once */
		borg_do_frame = FALSE;

		/* Analyze the "frame" */
		borg_update_frame();
	}

	/*** Re-activate Tests ***/

	/* Check frame again later */
	borg_do_frame = TRUE;

	/* Check spells again later */
	borg_do_spell = TRUE;


	/*** Analyze status ***/

	/* Track best level */
	if (borg_skill[BI_CLEVEL] > borg_skill[BI_MAXCLEVEL])
	{
		borg_skill[BI_MAXCLEVEL] = borg_skill[BI_CLEVEL];
	}
	if (borg_skill[BI_CDEPTH] > borg_skill[BI_MAXDEPTH])
	{
		borg_skill[BI_MAXDEPTH] = borg_skill[BI_CDEPTH];
	}

	/*** Think about it ***/

	/* Increment the clock */
	borg_t++;

	/* Examine the screen */
	borg_update();

	/* Extract some "hidden" variables */
	borg_hidden();

	/* Hack -- allow user abort */
	if (borg_cancel) return (TRUE);

	/* Do something */
	return (borg_think_dungeon());
}



/*
 * Hack -- methods of hurting a monster (order not important).
 *
 * See "message_pain()" for details.
 */
static cptr suffix_pain[] =
{
	" is unharmed." " barely notices.",
	" flinches.",
	" squelches.",
	" quivers in pain.",
	" writhes about.",
	" writhes in agony.",
	" jerks limply.",

	" roars thunderously.",
	" rumbles.",
	" grunts",
	" hesitates",
	" crumples",

	" hisses.",
	" rears up in anger.",
	" hisses furiously.",
	" roars.",
	" mewls in pain.",
	" mewls pitifully.",

	" chitters.",
	" scuttles about.",
	" twitters.",
	" twitches.",
	" chirps.",
	" squawks.",
	" chatters.",
	" jeers.",
	" flutters about.",
	" squeaks.",

	" snarls with pain.",
	" roars with pain.",
	" gasps.",
	" snarls feebly.",
	" rattles.",
	" stumbles.",
	" staggers.",
	" clatters.",
	" groans.",
	" moans.",
	" hesitates.",
	" grunts.",
	" wails.",
	" howls.",
	" moans softly.",
	" sighs.",
	" shrieks in pain.",
	" shrieks in agony.",

	" spawns!",
	" looks healthier.",
	" starts moving faster.",
	" starts moving slower.",

	" is unaffected!",
	" is immune.",
	" resists a lot.",
	" resists.",
	" resists somewhat.",

	" shrugs off the attack.",
	" snarls with pain.",
	" yelps in pain.",
	" howls in pain.",
	" howls in agony.",
	/* xxx */
	" yelps feebly.",

	" ignores the attack.",
	" grunts with pain.",
	" squeals in pain.",
	" shrieks in pain.",
	" shrieks in agony.",
	/* xxx */
	" cries out feebly.",

	/* xxx */
	/* xxx */
	" cries out in pain.",
	" screams in pain.",
	" screams in agony.",
	/* xxx */
	" cringes from the light!",
	" loses some skin!",

	" is hit hard.",

	NULL
};


/*
 * Hack -- methods of killing a monster (order not important).
 *
 * See "mon_take_hit()" for details.
 */
static cptr prefix_kill[] =
{
	"You have killed ",
	"You have slain ",
	"You have destroyed ",
	NULL
};


/*
 * Hack -- methods of monster death (order not important).
 *
 * See "project_m()", "do_cmd_fire()", "mon_take_hit()" for details.
 */
static cptr suffix_died[] =
{
	" dies.",
	" dies from the Quivering Palm.",
	" is destroyed.",
	" is killed.",
	" dissolves!",
	" shrivels away in the light!",
	" collapses, a mindless husk.",
	NULL
};
static cptr suffix_blink[] =
{
	" disappears!",	/* from teleport other */
	" changes!",	/* from polymorph spell */
	" teleports away.",	/* RF6_TPORT */
	" blinks away.",	/* RF6_BLINK */
	NULL
};

/*
 * Hack -- methods of hitting the player (order not important).
 *
 * The "insult", "moan", and "begs you for money" messages are ignored.
 *
 * See "make_attack_normal()" for details.
 */
static cptr suffix_hit_by[] =
{
	" hits you.",
	" touches you.",
	" punches you.",
	" kicks you.",
	" claws you.",
	" bites you.",
	" stings you.",
	" butts you.",
	" crushes you.",
	" engulfs you.",
	" charges you.",
	" crawls on you.",
	" drools on you.",
	" spits on you.",
	" gazes at you.",
	" wails at you.",
	" releases spores at you.",
	NULL
};


/*
 * Hack -- methods of casting spells at the player (order important).
 *
 * See "make_attack_spell()" for details.
 */
/* AJG These had gotten out of synch with where they are used. */
static cptr suffix_spell[] =
{
	" makes a high pitched shriek.",	/* 0 RF4_SHRIEK */
	" tries to cast a spell, but fails.",	/* 1 RF4_FAILS */
	" does something.",	/* 2 RF4_XXX3X4 */
	" does something.",	/* 3 RF4_XXX4X4 */
	" fires an arrow.",	/* 4 RF4_ARROW_1 */
	" fires an arrow!",	/* 5 RF4_ARROW_2 */
	" fires a missile.",	/* 6 RF4_ARROW_3 */
	" fires a missile!",	/* 7 RF4_ARROW_4 */
	" breathes acid.",	/* 8 RF4_BR_ACID */
	" breathes lightning.",	/* 9 RF4_BR_ELEC */
	" breathes fire.",	/*10 RF4_BR_FIRE */
	" breathes frost.",	/*11 RF4_BR_COLD */
	" breathes gas.",	/*12 RF4_BR_POIS */
	" breathes nether.",	/*13 RF4_BR_NETH */
	" breathes light.",	/*14 RF4_BR_LITE */
	" breathes darkness.",	/*15 RF4_BR_DARK */
	" breathes confusion.",	/*16 RF4_BR_CONF */
	" breathes sound.",	/*17 RF4_BR_SOUN */
	" breathes chaos.",	/*18 RF4_BR_CHAO */
	" breathes disenchantment.",	/*19 RF4_BR_DISE */
	" breathes nexus.",	/*20 RF4_BR_NEXU */
	" breathes time.",	/*21 RF4_BR_TIME */
	" breathes inertia.",	/*22 RF4_BR_INER */
	" breathes gravity.",	/*23 RF4_BR_GRAV */
	" breathes shards.",	/*24 RF4_BR_SHAR */
	" breathes plasma.",	/*25 RF4_BR_PLAS */
	" breathes force.",	/*26 RF4_BR_WALL */
	" does something.",	/*27 RF4_BR_MANA */
	" does something.",	/*28 RF4_XXX5X4 */
	" does something.",	/*29 RF4_XXX6X4 */
	" does something.",	/*30 RF4_XXX7X4 */
	" does something.",	/*31 RF4_XXX8X4 */
	" casts an acid ball.",	/*32 RF5_BA_ACID */
	" casts a lightning ball.",	/*33 RF5_BA_ELEC */
	" casts a fire ball.",	/*34 RF5_BA_FIRE */
	" casts a frost ball.",	/*35 RF5_BA_COLD */
	" casts a stinking cloud.",	/*36 RF5_BA_POIS */
	" casts a nether ball.",	/*37 RF5_BA_NETH */
	" gestures fluidly.",	/*38 RF5_BA_WATE */
	" invokes a mana storm.",	/*39 RF5_BA_MANA */
	" invokes a darkness storm.",	/*40 RF5_BA_DARK */
	" draws psychic energy from you!",	/*41 RF5_DRAIN_MANA */
	" gazes deep into your eyes.",	/*42 RF5_MIND_BLAST */
	" looks deep into your eyes.",	/*43 RF5_BRAIN_SMASH */
	" points at you and curses.",	/*44 RF5_CAUSE_1 */
	" points at you and curses horribly.",	/*45 RF5_CAUSE_2 */
	" points at you, incanting terribly!",	/*46 RF5_CAUSE_3 */
	" points at you, screaming the word DIE!",	/*47 RF5_CAUSE_4 */
	" casts a acid bolt.",	/*48 RF5_BO_ACID */
	" casts a lightning bolt.",	/*49 RF5_BO_ELEC */
	" casts a fire bolt.",	/*50 RF5_BO_FIRE */
	" casts a frost bolt.",	/*51 RF5_BO_COLD */
	" does something.",	/*52 RF5_BO_POIS */
	" casts a nether bolt.",	/*53 RF5_BO_NETH */
	" casts a water bolt.",	/*54 RF5_BO_WATE */
	" casts a mana bolt.",	/*55 RF5_BO_MANA */
	" casts a plasma bolt.",	/*56 RF5_BO_PLAS */
	" casts an ice bolt.",	/*57 RF5_BO_ICEE */
	" casts a magic missile.",	/*58 RF5_MISSILE */
	" casts a fearful illusion.",	/*59 RF5_SCARE */
	" casts a spell, burning your eyes!",	/*60 RF5_BLIND */
	" creates a mesmerising illusion.",	/*61 RF5_CONF */
	" drains power from your muscles!",	/*62 RF5_SLOW */
	" stares deep into your eyes!",	/*63 RF5_HOLD */
	" concentrates on XXX body.",	/*64 RF6_HASTE */
	" does something.",	/*65 RF6_XXX1X6 */
	" concentrates on XXX wounds.",	/*66 RF6_HEAL */
	" does something.",	/*67 RF6_XXX2X6 */
	" does something.",	/*68 RF6_XXX3X6 */
	" does something.",	/*69 RF6_XXX4X6 */
	" commands you to return.",	/*70 RF6_TELE_TO */
	" teleports you away.",	/*71 RF6_TELE_AWAY */
	" gestures at your feet.",	/*72 RF6_TELE_LEVEL */
	" does something.",	/*73 RF6_XXX5 */
	" gestures in shadow.",	/*74 RF6_DARKNESS */
	" casts a spell and cackles evilly.",	/*75 RF6_TRAPS */
	" tries to blank your mind.",	/*76 RF6_FORGET */
	" does something.",	/*77 RF6_XXX6X6 */
	" does something.",	/*78 RF6_XXX7X6 */
	" does something.",	/*79 RF6_XXX8X6 */
	" magically summons help!",	/*80 RF6_S_MONSTER */
	" magically summons monsters!",	/*81 RF6_S_MONSTERS */
	" magically summons ants.",	/*82 RF6_S_ANT */
	" magically summons spiders.",	/*83 RF6_S_SPIDER */
	" magically summons hounds.",	/*84 RF6_S_HOUND */
	" magically summons hydras.",	/*85 RF6_S_HYDRA */
	" magically summons an angel!",	/*86 RF6_S_ANGEL */
	" magically summons a hellish adversary!",	/*87 RF6_S_DEMON */
	" magically summons an undead adversary!",	/*88 RF6_S_UNDEAD */
	" magically summons a dragon!",	/*89 RF6_S_DRAGON */
	" magically summons greater undead!",	/*90 RF6_S_HI_UNDEAD */
	" magically summons ancient dragons!",	/*91 RF6_S_HI_DRAGON */
	" magically summons mighty undead opponents!",	/*92 RF6_S_WRAITH */
	" magically summons special opponents!",	/*93 RF6_S_UNIQUE */

	NULL
};



#if 0
/* XXX XXX XXX */
msg_format("%^s looks healthier.", m_name);
msg_format("%^s looks REALLY healthy!", m_name);
#endif



/*
 * Hack -- Spontaneous level feelings (order important).
 *
 * See "do_cmd_feeling()" for details.
 */
static cptr prefix_feeling[] =
{
	"Looks like any other level",
	"You feel there is something special",
	"You have a superb feeling",
	"You have an excellent feeling",
	"You have a very good feeling",
	"You have a good feeling",
	"You feel strangely lucky",
	"You feel your luck is turning",
	"You like the look of this place",
	"This level can't be all bad",
	"What a boring place",
	NULL
};



/*
 * Hack -- Parse a message from the world
 *
 * Note that detecting "death" is EXTREMELY important, to prevent
 * all sorts of errors arising from attempting to parse the "tomb"
 * screen, and to allow the user to "observe" the "cause" of death.
 *
 * Note that detecting "failure" is EXTREMELY important, to prevent
 * bizarre situations after failing to use a staff of perceptions,
 * which would otherwise go ahead and send the "item index" which
 * might be a legal command (such as "a" for "aim").  This method
 * is necessary because the Borg cannot parse "prompts", and must
 * assume the success of the prompt-inducing command, unless told
 * otherwise by a failure message.  Also, we need to detect failure
 * because some commands, such as detection spells, need to induce
 * furthur processing if they succeed, but messages are only given
 * if the command fails.
 *
 * Note that certain other messages may contain useful information,
 * and so they are "analyzed" and sent to "borg_react()", which just
 * queues the messages for later analysis in the proper context.
 *
 * Along with the actual message, we send a special formatted buffer,
 * containing a leading "opcode", which may contain extra information,
 * such as the index of a spell, and an "argument" (for example, the
 * capitalized name of a monster), with a "colon" to separate them.
 *
 * XXX XXX XXX Several message strings take a "possessive" of the form
 * "his" or "her" or "its".  These strings are all represented by the
 * encoded form "XXX" in the various match strings.  Unfortunately,
 * the encode form is never decoded, so the Borg currently ignores
 * messages about several spells (heal self and haste self).
 *
 * XXX XXX XXX We notice a few "terrain feature" messages here so
 * we can acquire knowledge about wall types and door types.
 */
static void borg_parse_aux(cptr msg, int len)
{
	int i, tmp;

	char who[256];
	char buf[256];

	/* Log (if needed) */
	if (borg_fff) borg_info(format("& Msg <%s>", msg));


	/* Hack -- Notice death */
	if (prefix(msg, "You die."))
	{
		/* Abort (unless cheating) */
		if (!(p_ptr->wizard || cheat_live))
		{
			/* Abort */
			borg_oops("death");

			/* Abort right now! */
			borg_active = FALSE;

			/* Noise XXX XXX XXX */
			Term_xtra(TERM_XTRA_NOISE, 1);
		}

		/* Done */
		return;
	}


	/* Hack -- Notice "failure" */
	if (prefix(msg, "You failed "))
	{
		/* Hack -- store the keypress */
		borg_note("# Normal failure.");

		/* Set the failure flag */
		borg_failure = TRUE;

		/* Flush our key-buffer */
		borg_flush();

		/* If we were casting a targetted spell and failed */
		/* it does not mean we can't target that location */
		successful_target = 0;
		
		return;
	}


	/* Ignore teleport trap */
	if (prefix(msg, "You hit a teleport")) return;

	/* Ignore arrow traps */
	if (prefix(msg, "An arrow ")) return;

	/* Ignore dart traps */
	if (prefix(msg, "A small dart ")) return;

	if (prefix(msg, "The cave "))
	{
		borg_react(msg, "QUAKE");
		return;
	}

	/* need to check stat */
	if (prefix(msg, "You feel very") ||
		prefix(msg, "You feel less") || prefix(msg, "Wow!  You feel very"))
	{
		/* need to check str */
		if (prefix(msg, "You feel very weak"))
		{
			my_need_stat_check[0] = TRUE;
		}
		if (prefix(msg, "You feel less weak"))
		{
			my_need_stat_check[0] = TRUE;
		}
		if (prefix(msg, "Wow!  You feel very strong"))
		{
			my_need_stat_check[0] = TRUE;
		}

		/* need to check int */
		if (prefix(msg, "You feel very stupid"))
		{
			my_need_stat_check[1] = TRUE;
		}
		if (prefix(msg, "You feel less stupid"))
		{
			my_need_stat_check[1] = TRUE;
		}
		if (prefix(msg, "Wow!  You feel very smart"))
		{
			my_need_stat_check[1] = TRUE;
		}

		/* need to check wis */
		if (prefix(msg, "You feel very naive"))
		{
			my_need_stat_check[2] = TRUE;
		}
		if (prefix(msg, "You feel less naive"))
		{
			my_need_stat_check[2] = TRUE;
		}
		if (prefix(msg, "Wow!  You feel very wise"))
		{
			my_need_stat_check[2] = TRUE;
		}

		/* need to check dex */
		if (prefix(msg, "You feel very clumsy"))
		{
			my_need_stat_check[3] = TRUE;
		}
		if (prefix(msg, "You feel less clumsy"))
		{
			my_need_stat_check[3] = TRUE;
		}
		if (prefix(msg, "Wow!  You feel very dextrous"))
		{
			my_need_stat_check[3] = TRUE;
		}

		/* need to check con */
		if (prefix(msg, "You feel very sickly"))
		{
			my_need_stat_check[4] = TRUE;
		}
		if (prefix(msg, "You feel less sickly"))
		{
			my_need_stat_check[4] = TRUE;
		}
		if (prefix(msg, "Wow!  You feel very healthy"))
		{
			my_need_stat_check[4] = TRUE;
		}

		/* need to check cha */
		if (prefix(msg, "You feel very ugly"))
		{
			my_need_stat_check[5] = TRUE;
		}
		if (prefix(msg, "You feel less ugly"))
		{
			my_need_stat_check[5] = TRUE;
		}
		if (prefix(msg, "Wow!  You feel very cute"))
		{
			my_need_stat_check[5] = TRUE;
		}
	}

	/* time attacks, just do all stats. */
	if (prefix(msg, "You're not as"))
	{
		my_need_stat_check[0] = TRUE;
		my_need_stat_check[1] = TRUE;
		my_need_stat_check[2] = TRUE;
		my_need_stat_check[3] = TRUE;
		my_need_stat_check[4] = TRUE;
		my_need_stat_check[5] = TRUE;
	}

	/* Nexus attacks, need to check everything! */
	if (prefix(msg, "Your body starts to scramble..."))
	{
		my_need_stat_check[0] = TRUE;
		my_need_stat_check[1] = TRUE;
		my_need_stat_check[2] = TRUE;
		my_need_stat_check[3] = TRUE;
		my_need_stat_check[4] = TRUE;
		my_need_stat_check[5] = TRUE;

		/* max stats may have lowered */
		my_stat_max[0] = 0;
		my_stat_max[1] = 0;
		my_stat_max[2] = 0;
		my_stat_max[3] = 0;
		my_stat_max[4] = 0;
		my_stat_max[5] = 0;

	}

	if (streq(msg, "You have been knocked out."))
	{
		borg_note("Ignoring Messages While KO'd");
		borg_dont_react = TRUE;
	}

	if (streq(msg, "You are paralyzed"))
	{
		borg_note("Ignoring Messages While Paralyzed");
		borg_dont_react = TRUE;
	}


	/* Hit somebody */
	if (prefix(msg, "You hit "))
	{
		tmp = strlen("You hit ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You strike "))
	{
		tmp = strlen("You strike ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You hack at "))
	{
		tmp = strlen("You hack at ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You bash "))
	{
		tmp = strlen("You bash ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You slash "))
	{
		tmp = strlen("You slash ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You pound "))
	{
		tmp = strlen("You pound ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You score "))
	{
		tmp = strlen("You score ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You batter "))
	{
		tmp = strlen("You batter ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You gouge "))
	{
		tmp = strlen("You gouge ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* Hit somebody */
	if (prefix(msg, "You bludgeon "))
	{
		tmp = strlen("You bludgeon ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}
	/* Hit somebody */
	if (prefix(msg, "You *smite* "))
	{
		tmp = strlen("You *smite* ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* Hit somebody */
	if (prefix(msg, "You bite "))
	{
		tmp = strlen("You bite ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* Hit somebody */
	if (prefix(msg, "You claw "))
	{
		tmp = strlen("You claw ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "HIT:%^s", who);
		borg_react(msg, buf);
		return;
	}


	/* Miss somebody */
	if (prefix(msg, "You miss "))
	{
		tmp = strlen("You miss ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "MISS:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* Miss somebody (because of fear) */
	if (prefix(msg, "You are too afraid to attack "))
	{
		tmp = strlen("You are too afraid to attack ");
		strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
		strnfmt(buf, 256, "MISS:%^s", who);
		borg_react(msg, buf);
		return;
	}


	/* "It screams in pain." (etc) */
	for (i = 0; suffix_pain[i]; i++)
	{
		/* "It screams in pain." (etc) */
		if (suffix(msg, suffix_pain[i]))
		{
			tmp = strlen(suffix_pain[i]);
			strnfmt(who, 1 + len - tmp, "%s", msg);
			strnfmt(buf, 256, "PAIN:%^s", who);
			borg_react(msg, buf);
			return;
		}
	}


	/* "You have killed it." (etc) */
	for (i = 0; prefix_kill[i]; i++)
	{
		/* "You have killed it." (etc) */
		if (prefix(msg, prefix_kill[i]))
		{
			tmp = strlen(prefix_kill[i]);
			strnfmt(who, 1 + len - (tmp + 1), "%s", msg + tmp);
			strnfmt(buf, 256, "KILL:%^s", who);
			borg_react(msg, buf);
			return;
		}
	}


	/* "It dies." (etc) */
	for (i = 0; suffix_died[i]; i++)
	{
		/* "It dies." (etc) */
		if (suffix(msg, suffix_died[i]))
		{
			tmp = strlen(suffix_died[i]);
			strnfmt(who, 1 + len - tmp, "%s", msg);
			strnfmt(buf, 256, "DIED:%^s", who);
			borg_react(msg, buf);
			return;
		}
	}

	/* "It blinks or telports." (etc) */
	for (i = 0; suffix_blink[i]; i++)
	{
		/* "It teleports." (etc) */
		if (suffix(msg, suffix_blink[i]))
		{
			tmp = strlen(suffix_blink[i]);
			strnfmt(who, 1 + len - tmp, "%s", msg);
			strnfmt(buf, 256, "BLINK:%^s", who);
			borg_react(msg, buf);
			return;
		}
	}

	/* "It misses you." */
	if (suffix(msg, " misses you."))
	{
		tmp = strlen(" misses you.");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "MISS_BY:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* "It is repelled.." */
	/* treat as a miss */
	if (suffix(msg, " is repelled."))
	{
		tmp = strlen(" is repelled.");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "MISS_BY:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* Ignore talking monsters */
	if (strstr(msg, " says,")) return;

	/* "It hits you." (etc) */
	for (i = 0; suffix_hit_by[i]; i++)
	{
		/* "It hits you." (etc) */
		if (suffix(msg, suffix_hit_by[i]))
		{
			tmp = strlen(suffix_hit_by[i]);
			strnfmt(who, 1 + len - tmp, "%s", msg);
			strnfmt(buf, 256, "HIT_BY:%^s", who);
			borg_react(msg, buf);

			/* If I was hit, then I am not on a glyph */
			if (track_glyph_num)
			{
				/* erase them all and
				 * allow the borg to scan the screen and rebuild the array.
				 * He won't see the one under him though.  So a special check
				 * must be made.
				 */
				/* byte feat = mb_ptr->feat; */

				/* Remove the entire array */
				for (i = 0; i < track_glyph_num; i++)
				{
					/* Stop if we already new about this glyph */
					track_glyph_x[i] = 0;
					track_glyph_y[i] = 0;
				}
				track_glyph_num = 0;
			}

			return;
		}
	}


	/* "It casts a spell." (etc) */
	for (i = 0; suffix_spell[i]; i++)
	{
		/* "It casts a spell." (etc) */
		if (suffix(msg, suffix_spell[i]))
		{
			tmp = strlen(suffix_spell[i]);
			strnfmt(who, 1 + len - tmp, "%s", msg);
			strnfmt(buf, 256, "SPELL_%03d:%^s", i, who);
			borg_react(msg, buf);
			return;
		}
	}

#if 0
	/* State -- Paralyzed */
	if (suffix(msg, " is paralyzed!"))
	{
		tmp = strlen(" is paralyzed!");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE_INVLUN:%^s", who);
		borg_react(msg, buf);
		return;
	}
#endif

	/* State -- Asleep */
	if (suffix(msg, " falls asleep!"))
	{
		tmp = strlen(" falls asleep!");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE_SLEEP:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* State -- confused */
	if (suffix(msg, " looks confused."))
	{
		tmp = strlen(" looks confused.");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE_CONFUSED:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* State -- confused */
	if (suffix(msg, " looks more confused."))
	{
		tmp = strlen(" looks more confused.");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE_CONFUSED:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* State -- Not Asleep */
	if (suffix(msg, " wakes up."))
	{
		tmp = strlen(" wakes up.");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE_AWAKE:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* State -- Afraid */
	if (suffix(msg, " flees in terror!"))
	{
		tmp = strlen(" flees in terror!");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE__FEAR:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* State -- Not Afraid */
	if (suffix(msg, " recovers his courage."))
	{
		tmp = strlen(" recovers his courage.");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE__BOLD:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* State -- Not Afraid */
	if (suffix(msg, " recovers her courage."))
	{
		tmp = strlen(" recovers her courage.");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE__BOLD:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* State -- Not Afraid */
	if (suffix(msg, " recovers its courage."))
	{
		tmp = strlen(" recovers its courage.");
		strnfmt(who, 1 + len - tmp, "%s", msg);
		strnfmt(buf, 256, "STATE__BOLD:%^s", who);
		borg_react(msg, buf);
		return;
	}

	/* Feature XXX XXX XXX */
	if (streq(msg, "The door appears to be broken."))
	{
		/* Clear goals */
		goal = 0;
		return;
	}

	/* Feature XXX XXX XXX */
	if (streq(msg, "The door appears to be stuck."))
	{
		/* Clear goals */
		goal = 0;

		return;
	}



	/* Feature XXX XXX XXX */
	if (streq(msg, "This seems to be permanent rock."))
	{

		/* Clear goals */
		goal = 0;

		return;
	}

	/* Feature XXX XXX XXX */
	if (streq(msg, "You tunnel into the granite wall."))
	{
		/* Clear goals */
		goal = 0;

		return;
	}

	/* Feature Invisible Walls */
	if (streq(msg, "You bump into something."))
	{
		/* Clear goals */
		goal = 0;

		return;
	}

	/* Feature XXX XXX XXX */
	if (streq(msg, "You tunnel into the quartz vein."))
	{
		/* Clear goals */
		goal = 0;

		return;
	}

	/* Feature XXX XXX XXX */
	if (streq(msg, "You tunnel into the magma vein."))
	{
		/* Clear goals */
		goal = 0;

		return;
	}

	/* Word of Recall -- Ignition */
	if (prefix(msg, "The air about you becomes "))
	{
		/* Initiate recall */
		/* Guess how long it will take to lift off */
		goal_recalling = 15000 + 5000;	/* Guess. game turns x 1000 ( 15+rand(20)) */
		return;
	}

	/* Word of Recall -- Lift off */
	if (prefix(msg, "You feel yourself yanked "))
	{
		/* Recall complete */
		goal_recalling = 0;
		return;
	}

	/* Word of Recall -- Cancelled */
	if (prefix(msg, "A tension leaves "))
	{
		/* Hack -- Oops */
		goal_recalling = 0;
		return;
	}

	/* Wearing Cursed Item */
	if ((prefix(msg, "Oops! It feels deathly cold!")) ||
		(suffix(msg, " appears to be cursed.")) ||
		(suffix(msg, " seems to be cursed.")))
	{
		/* Hack -- Oops */
		borg_wearing_cursed = TRUE;
		return;
	}

	/* protect from evil */
	if (prefix(msg, "You feel safe from evil!"))
	{
		borg_prot_from_evil = TRUE;
		return;
	}
	if (prefix(msg, "You no longer feel safe from evil."))
	{
		borg_prot_from_evil = FALSE;
		return;
	}
	/* haste self */
	if (prefix(msg, "You feel yourself moving faster!"))
	{
		borg_speed = TRUE;
		return;
	}
	if (prefix(msg, "You feel yourself slow down."))
	{
		borg_speed = FALSE;
		return;
	}
	/* Bless */
	if (prefix(msg, "You feel righteous!"))
	{
		borg_bless = TRUE;
		return;
	}
	if (prefix(msg, "The prayer has expired."))
	{
		borg_bless = FALSE;
		return;
	}

	/* hero */
	if (prefix(msg, "You feel like a hero!"))
	{
		borg_hero = TRUE;
		return;
	}
	if (prefix(msg, "The heroism wears off."))
	{
		borg_hero = FALSE;
		return;
	}
	/* berserk */
	if (prefix(msg, "You feel like a killing machine!"))
	{
		borg_berserk = TRUE;
		return;
	}
	if (prefix(msg, "You feel less Berserk."))
	{
		borg_berserk = FALSE;
		return;
	}

	/* check for wall blocking but not when confused */
	if ((prefix(msg, "There is a wall ") && (!borg_skill[BI_ISCONFUSED])))
	{
		my_need_alter = TRUE;
		goal = 0;
		return;
	}


	/* check for closed door but not when confused */
	if ((prefix(msg, "There is a closed door blocking your way.") &&
		 (!borg_skill[BI_ISCONFUSED])))
	{
		my_need_alter = TRUE;
		goal = 0;
		return;
	}

	/* check for mis-alter command.  Sometime induced by never_move guys */
	if (streq(msg, "You spin around.") && !borg_skill[BI_ISCONFUSED])
	{

		/* Examine all the monsters */
		for (i = 1; i < borg_kills_nxt; i++)
		{
			borg_kill *kill = &borg_kills[i];

			/* Skip dead monsters */
			if (!kill->r_idx) continue;

			/* Distance components */
			if ((c_y == kill->y && c_x == kill->x) ||
				(g_y == kill->y && g_x == kill->x))
			{
				/* Hack -- kill em */
				borg_delete_kill(i, "oops, spinning");
			}
		}

		my_no_alter = TRUE;
		goal = 0;
		return;
	}

	/* Feature XXX XXX XXX */
	if (prefix(msg, "You see nothing there "))
	{
		my_no_alter = TRUE;
		/* Clear goals */
		goal = 0;
		return;

	}
	/* Tunneling not understood correctly */
	if (prefix(msg, "You cannot tunnel through air."))
	{
		my_no_alter = TRUE;

		/* Clear goals */
		goal = 0;
		return;

	}

	/* Hack to protect against clock overflows and errors */
	if (prefix(msg, "Illegal "))
	{
		borg_oops_fmt("# Borg problem msg: %s", msg);

		/* Hack -- Oops */
		borg_keypress(ESCAPE);
		borg_keypress(ESCAPE);
		return;
	}

	/* resist acid */
	if (prefix(msg, "You feel resistant to acid!"))
	{
		my_oppose_acid = TRUE;
		return;
	}
	if (prefix(msg, "You feel less resistant to acid."))
	{
		my_oppose_acid = FALSE;
		return;
	}
	/* resist electricity */
	if (prefix(msg, "You feel resistant to electricity!"))
	{
		my_oppose_elec = TRUE;
		return;
	}
	if (prefix(msg, "You feel less resistant to electricity."))
	{
		my_oppose_elec = FALSE;
		return;
	}
	/* resist fire */
	if (prefix(msg, "You feel resistant to fire!"))
	{
		my_oppose_fire = TRUE;
		return;
	}
	if (prefix(msg, "You feel less resistant to fire."))
	{
		my_oppose_fire = FALSE;
		return;
	}
	/* resist cold */
	if (prefix(msg, "You feel resistant to cold!"))
	{
		my_oppose_cold = TRUE;
		return;
	}
	if (prefix(msg, "You feel less resistant to cold."))
	{
		my_oppose_cold = FALSE;
		return;
	}
	/* resist poison */
	if (prefix(msg, "You feel resistant to poison!"))
	{
		my_oppose_pois = TRUE;
		return;
	}
	if (prefix(msg, "You feel less resistant to poison."))
	{
		my_oppose_pois = FALSE;
		return;
	}
	/* GOI! */
	if (prefix(msg, "You feel invulnerable!"))
	{
		borg_goi = 12000;		/* keep track of how long it has left (a guess) */
		return;
	}
	if (prefix(msg, "You feel vulnerable once more."))
	{
		borg_goi = 0;
		return;
	}
	/* Telepathy */
	if (prefix(msg, "You feel your consciousness expand!"))
	{
		borg_esp = TRUE;
		return;
	}
	if (prefix(msg, "Your consciousness contracts again."))
	{
		borg_esp = FALSE;
		return;
	}

	/* Invisible */

	/* Shield */
	if (prefix(msg, "A mystic shield forms around your body!"))
	{
		borg_shield = TRUE;
		return;
	}
	if (prefix(msg, "Your mystic shield crumbles away."))
	{
		borg_shield = FALSE;
		return;
	}

	/* Glyph of Warding (the spell no longer gives a report) */
	/* Sadly  Rune of Protection has no message */
	if (prefix(msg, "You inscribe a mystic symbol on the ground!"))
	{
		/* Check for an existing glyph */
		for (i = 0; i < track_glyph_num; i++)
		{
			/* Stop if we already new about this glyph */
			if ((track_glyph_x[i] == c_x) && (track_glyph_y[i] == c_y)) break;
		}

		/* Track the newly discovered glyph */
		if ((i == track_glyph_num) && (track_glyph_size))
		{
			borg_note("# Noting the creation of a glyph.");
			track_glyph_x[i] = c_x;
			track_glyph_y[i] = c_y;
			track_glyph_num++;
		}
		return;
	}
	if (prefix(msg, "The rune of protection is broken!"))
	{
		/* we won't know which is broken so erase them all and
		 * allow the borg to scan the screen and rebuild the array.
		 * He won't see the one under him though.  So a special check
		 * must be made.
		 */
		/* byte feat = mb_ptr->feat; */

		/* Remove the entire array */
		for (i = 0; i < track_glyph_num; i++)
		{
			/* Stop if we already new about this glyph */
			track_glyph_x[i] = 0;
			track_glyph_y[i] = 0;
			track_glyph_num = 0;
		}

		/* no known glyphs */
		track_glyph_num = 0;
	}
	/* failed glyph spell message */
	if (prefix(msg, "The object resists the spell"))
	{

		/* Forget the newly created-though-failed  glyph */
		track_glyph_x[track_glyph_num] = 0;
		track_glyph_y[track_glyph_num] = 0;
		track_glyph_num--;

		return;
	}

	/* Removed rubble.  Important when out of lite */
	if (prefix(msg, "You have removed the "))
	{
		int x, y;
		/* remove rubbles from array */
		for (y = c_y - 1; y < c_y + 1; y++)
		{
			for (x = c_x - 1; x < c_x + 1; x++)
			{
				/* replace all rubble with broken doors, the borg ignores
				 * broken doors.  This routine is only needed if the borg
				 * is out of lite and searching in the dark.
				 */
				if (borg_skill[BI_CUR_LITE]) continue;
			}
		}
		return;
	}

	/* need to kill monsters when WoD is used */
	if (prefix(msg, "There is a searing blast of light!"))
	{
		/* Examine all the monsters */
		for (i = 1; i < borg_kills_nxt; i++)
		{
			borg_kill *kill = &borg_kills[i];

			int x9 = kill->x;
			int y9 = kill->y;
			int ax, ay, d;

			/* Skip dead monsters */
			if (!kill->r_idx) continue;

			/* Distance components */
			ax = (x9 > c_x) ? (x9 - c_x) : (c_x - x9);
			ay = (y9 > c_y) ? (y9 - c_y) : (c_y - y9);

			/* Distance */
			d = MAX(ax, ay);

			/* Minimal distance */
			if (d > 12) continue;

			/* Hack -- kill em */
			borg_delete_kill(i, "*destruction*");
		}

		return;
	}

	/* Be aware and concerned of busted doors */
	if (prefix(msg, "You hear a door burst open!"))
	{
		/* on level 1 and 2 be concerned.  Could be Grip or Fang */
		if (borg_skill[BI_CDEPTH] <= 3 &&
			borg_skill[BI_CLEVEL] <= 5) scaryguy_on_level = TRUE;
	}

	/* Recognize Drownin */
	if (prefix(msg, "You are drowning"))
	{
		borg_note("# Help! I can't swim");

		return;
	}

	/* Recognize Drownin */
	if (prefix(msg, "The lava burns you"))
	{
		borg_note("# Help! I'm burning");

		return;
	}

	/* Feelings about the level */
	for (i = 0; prefix_feeling[i]; i++)
	{
		/* "You feel..." (etc) */
		if (prefix(msg, prefix_feeling[i]))
		{
			strnfmt(buf, 256, "FEELING:%d", i);
			borg_react(msg, buf);
			return;
		}
	}
}



/*
 * Parse a message, piece of a message, or set of messages.
 *
 * We must handle long messages which are "split" into multiple
 * pieces, and also multiple messages which may be "combined"
 * into a single set of messages.
 */
static void borg_parse(cptr msg)
{
	static char len = 0;
	static char buf[1024];


	/* Flush messages */
	if (len && (!msg || (msg[0] != ' ')))
	{
		int i, j;

		/* Split out punctuation */
		for (j = i = 0; i < len - 1; i++)
		{
			/* Check for punctuation */
			if ((buf[i] == '.') ||
				(buf[i] == '!') || (buf[i] == '?') || (buf[i] == '"'))
			{
				/* Require space */
				if (buf[i + 1] == ' ')
				{
					/* Terminate */
					buf[i + 1] = '\0';

					/* Parse fragment */
					borg_parse_aux(buf + j, (i + 1) - j);

					/* Restore */
					buf[i + 1] = ' ';

					/* Advance past spaces */
					for (j = i + 2; buf[j] == ' '; j++) /* loop */ ;
				}
			}
		}

		/* Parse tail */
		borg_parse_aux(buf + j, len - j);

		/* Forget */
		len = 0;
	}


	/* No message */
	if (!msg)
	{
		/* Start over */
		len = 0;
	}

	/* Continued message */
	else if (msg[0] == ' ')
	{
		/* Collect, verify, and grow */
		len += strnfmt(buf + len, 1024 - len, "%s", msg + 1);
	}

	/* New message */
	else
	{
		/* Collect, verify, and grow */
		len = strnfmt(buf, 1024, "%s", msg);
	}
}


/*
 * Initialize zborg.txt
 */

static void init_borg_txt_file(void)
{
	/*
	 * Array of borg variables is stored as
	 * 0 to k_max = items in inventory
	 * k_max  to end of array = Other skills/possessions
	 */
	size_obj = z_info->k_max + BI_MAX;

	C_MAKE(borg_has, size_obj, int);

	/* Make a shortcut pointers into the array */
	borg_skill = &borg_has[z_info->k_max];

	/* Make sure we know who and what we are */
	borg_class = p_ptr->pclass;

	/* Use default values */
	borg_scums_uniques = TRUE;
	borg_stop_king = TRUE;

	/* Success */
	return;
}

static void borg_log_death(void)
{
	char buf[1024];
	FILE *borg_log_file;
	time_t death_time;

	path_build(buf, 1024, ANGBAND_DIR_USER, "borg-log.txt");

	/* Hack -- drop permissions */
	safe_setuid_drop();

	/* Append to the file */
	borg_log_file = my_fopen(buf, "a");

	/* Hack -- grab permissions */
	safe_setuid_grab();

	/* Failure */
	if (!borg_log_file) return;

	/* Get time of death */
	(void)time(&death_time);

	/* Save the date */
	strftime(buf, 80, "%Y/%m/%d %H:%M\n", localtime(&death_time));

	fprintf(borg_log_file, buf);

	fprintf(borg_log_file, "%s the %s %s, Level %d/%d\n", player_name,
			race_info[p_ptr->prace].title,
			class_info[p_ptr->pclass].title, p_ptr->lev, p_ptr->max_lev);

	fprintf(borg_log_file, "Exp: %lu  Gold: %lu  Turn: %lu\n",
			(long) /* total_points() */ 0, (long)p_ptr->au, (long)turn);
	fprintf(borg_log_file, "Killed on level: %d (max. %d) by %s\n",
			p_ptr->depth, p_ptr->max_depth, p_ptr->died_from);

	fprintf(borg_log_file, "----------\n\n");

	my_fclose(borg_log_file);
}


static void borg_log_death_data(void)
{
	char buf[1024];
	FILE *borg_log_file;
	time_t death_time;

	path_build(buf, 1024, ANGBAND_DIR_USER, "zborg.dat");

	/* Hack -- drop permissions */
	safe_setuid_drop();

	/* Append to the file */
	borg_log_file = my_fopen(buf, "a");

	/* Hack -- grab permissions */
	safe_setuid_grab();

	/* Failure */
	if (!borg_log_file) return;

	/* Get time of death */
	(void)time(&death_time);

	/* dump stuff for easy import to database */
	fprintf(borg_log_file, "%s, %s, %d, %d, %s\n",
			race_info[p_ptr->prace].title, class_info[p_ptr->pclass].title,
			p_ptr->lev, p_ptr->depth, p_ptr->died_from);

	my_fclose(borg_log_file);
}

/*
 * Mega-Hack -- special "inkey_hack" hook.  XXX XXX XXX
 *
 * A special function hook (see "util.c") which allows the Borg to take
 * control of the "inkey()" function, and substitute in fake keypresses.
 */
extern char (*inkey_hack) (int flush_first);

/*
 * This function lets the Borg "steal" control from the user.
 *
 * The "util.c" file provides a special "inkey_hack" hook which we use
 * to steal control of the keyboard, using the special function below.
 *
 * Since this function bypasses the code in "inkey()" which "refreshes"
 * the screen whenever the game has to wait for a keypress, the screen
 * will only get refreshed when (1) an option such as "fresh_before"
 * induces regular screen refreshing or (2) various explicit calls to
 * "Term_fresh" are made, such as in the "project()" function.  This
 * has the interesting side effect that the screen is never refreshed
 * while the Borg is browsing stores, checking his inventory/equipment,
 * browsing spell books, checking the current panel, or examining an
 * object, which reduces the "screen flicker" considerably.  :-)
 *
 * The only way that the Borg can be stopped once it is started, unless
 * it dies or encounters an error, is to press any key.  This function
 * checks for real user input on a regular basic, and if any is found,
 * it is flushed, and after completing any actions in progress, this
 * function hook is removed, and control is returned to the user.
 *
 * We handle "broken" messages, in which long messages are "broken" into
 * pieces, and all but the first message are "indented" by one space, by
 * collecting all the pieces into a complete message and then parsing the
 * message once it is known to be complete.
 *
 * This function hook automatically removes itself when it realizes that
 * it should no longer be active.  Note that this may take place after
 * the game has asked for the next keypress, but the various "keypress"
 * routines should be able to handle this.
 */
static char borg_inkey_hack(int flush_first)
{
	char ch;

	int y = 0;
	int x = 80;

	byte t_a;

	char buf[128];

	bool borg_prompt;	/* ajg  For now we can just use this locally.
						   in the 283 borg he uses this to optimize knowing if
						   we are waiting at a prompt for info */

	bool borg_rand_quick;	/* Save system setting */
	u32b borg_rand_value;	/* Save system setting */

	/* Refresh the screen */
	Term_fresh();

	/* Deactivate */
	if (!borg_active)
	{
		/* Message */
		borg_note("# Removing keypress hook");

		/* Remove hook */
		inkey_hack = NULL;

		/* Flush keys */
		borg_flush();

		/* Flush */
		flush();

		/* Done */
		return (0);
	}


	/* Mega-Hack -- flush keys */
	if (flush_first)
	{
		/* Only flush if needed */
		if (borg_inkey(FALSE) != 0)
		{
			/* Message */
			borg_note("# Flushing keypress buffer");

			/* Flush keys */
			borg_flush();
		}
	}

	/* Locate the cursor */
	(void)Term_locate(&x, &y);


	/* Assume no prompt/message is available */
	borg_prompt = FALSE;

	/*
	 * Mega-Hack -- check for possible prompts/messages
	 * If the first four characters on the message line all
	 * have the same attribute (or are all spaces), and they
	 * are not all spaces (ascii value 0x20)...
	 */
	if ((0 == borg_what_text(0, 0, 4, &t_a, buf)) &&
		(t_a != TERM_DARK) && (*((u32b *)(buf)) != 0x20202020))
	{
		/* Assume a prompt/message is available */
		borg_prompt = TRUE;
	}


	/*
	 * Mega-Hack -- Catch "Die? [y/n]" messages
	 * If there is text on the first line...
	 * And the game does not want a command...
	 * And the cursor is on the top line...
	 * And the text acquired above is "Die?" 
	 */
	if (borg_prompt && !p_ptr->inkey_flag &&
		(y == 0) && (x >= 4) && streq(buf, "Die?") && borg_cheat_death)
	{
		/* Flush messages */
		borg_parse(NULL);

		/* flush the buffer */
		borg_flush();

		/* Take note */
		borg_note("# Cheating death...");

		/* Log the death */
		borg_log_death();
		borg_log_death_data();

		/* Cheat death */
		return ('n');
	}

	/* Mega-Hack -- Handle death */
	if (p_ptr->is_dead)
	{
		/* Log death */
		borg_log_death();
		borg_log_death_data();

		/* flush the buffer */
		borg_flush();

		/* Oops  */
		borg_oops("player died");

		/* Useless keypress */
		return (KTRL('C'));
	}


	/* 
	 * Mega-Hack -- Catch "-more-" messages
	 * If there is text on the first line...
	 * And the game does not want a command...
	 * And the cursor is on the top line...
	 * And there is text before the cursor...
	 * And that text is "-more-"
	 */
	if (borg_prompt && !p_ptr->inkey_flag &&
		(y == 0) && (x >= 7) &&
		(0 == borg_what_text(x - 7, y, 7, &t_a, buf)) &&
		(streq(buf, " -more-")))
	{
		/* Get the message */
		if (0 == borg_what_text(0, 0, x - 7, &t_a, buf))
		{
			/* Parse it */
			borg_parse(buf);
		}
		/* Clear the message */
		return (KTRL('M'));
	}

	/*
	 * Mega-Hack -- catch normal messages
	 * If there is text on the first line...
	 * And the game wants a command
	 */
	if (borg_prompt && p_ptr->inkey_flag)
	{
		/* Get the message(s) */
		if (0 == borg_what_text(0, 0, -80, &t_a, buf))
		{
			int k = strlen(buf);

			/* Strip trailing spaces */
			while ((k > 0) && (buf[k - 1] == ' ')) k--;

			/* Terminate */
			buf[k] = '\0';

			/* Parse it */
			borg_parse(buf);
		}

		/* Clear the message */
		return (KTRL('M'));
	}
	/* Flush messages */
	borg_parse(NULL);
	borg_dont_react = FALSE;

	/* Check for key */
	ch = borg_inkey(TRUE);

	/* Use the key */
	if (ch) return (ch);

	/* Hack - check to see if we are doing a repeated action */
	if (p_ptr->running || p_ptr->command_rep || p_ptr->resting)
	{
		return (0);
	}


	/* Check for user abort */
	(void)Term_inkey(&ch, FALSE, FALSE);

	/* User Abort */
	if (ch != 0)
	{
		/* Oops */
		borg_oops("user abort");

		/* Hack -- Escape */
		return (ESCAPE);
	}

	/* Save the system random info */
	borg_rand_quick = Rand_quick;
	borg_rand_value = Rand_value;

	/* Use the local random info */
	Rand_quick = TRUE;
	Rand_value = borg_rand_local;


	/* Think */
	while (!borg_think()) /* loop */ ;

	/* DVE- Update the status screen */
	borg_status();

	/* Save the local random info */
	borg_rand_local = Rand_value;

	/* Restore the system random info */
	Rand_quick = borg_rand_quick;
	Rand_value = borg_rand_value;

	/* Hack -- allow stepping to induce a clean cancel */
	if (borg_step && (!--borg_step)) borg_cancel = TRUE;


	/* Check for key */
	ch = borg_inkey(TRUE);

	/* Use the key */
	if (ch) return (ch);

	/* Oops */
	borg_oops("normal abort - out of keys???");

	/* Hack -- Escape */
	return (ESCAPE);
}

/*
 * Output a long int in binary format.
 */
static void borg_prt_binary(u32b flags, int col, int row)
{
	int i;
	u32b bitmask;

	/* Scan the flags */
	for (i = bitmask = 1; i <= 32; i++, bitmask *= 2)
	{
		/* Dump set bits */
		if (flags & bitmask)
		{
			Term_putch(col++, row, TERM_BLUE, '*');
		}

		/* Dump unset bits */
		else
		{
			Term_putch(col++, row, TERM_WHITE, '-');
		}
	}
}

/* this will display the values which the borg believes an
 * item has.  Select the item by inven # prior to hitting
 * the ^zo.
 */
static void borg_display_item(list_item *l_ptr)
{
	int i, j = 13;

	/* Clear the screen */
	for (i = 1; i <= 23; i++) prt("", j - 2, i);

	/* Describe fully */
	if (l_ptr->o_name) prt(l_ptr->o_name, j, 2);

	prt(format("k_idx = %-5d    tval = %-5d ",
			   l_ptr->k_idx, l_ptr->tval), j, 4);

	prt(format("number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
			   l_ptr->number, l_ptr->weight, l_ptr->ac, l_ptr->dd, l_ptr->ds),
		j, 5);

	prt(format("pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
			   l_ptr->pval, l_ptr->to_a, l_ptr->to_h, l_ptr->to_d), j, 6);

	if (l_ptr->xtra_name)
	{
		prt(format("xtra_name = %s", l_ptr->xtra_name), j, 7);
	}

	prt(format("info = %d  timeout = %-d", l_ptr->info, l_ptr->timeout), j, 8);


	prt("+------------FLAGS1------------+", j, 10);
	prt("AFFECT........SLAY........BRAND.", j, 11);
	prt("              cvae      xsqpaefc", j, 12);
	prt("siwdcc  ssidsahanvudotgddhuoclio", j, 13);
	prt("tnieoh  trnipttmiinmrrnrrraiierl", j, 14);
	prt("rtsxna..lcfgdkcpmldncltggpksdced", j, 15);
	borg_prt_binary(l_ptr->kn_flags1, j, 16);

	prt("+------------FLAGS2------------+", j, 17);
	prt("SUST...IMMUN..RESIST............", j, 18);
	prt("        aefctrpsaefcpfldbc sn   ", j, 19);
	prt("siwdcc  clioheatcliooeialoshtncd", j, 20);
	prt("tnieoh  ierlrfraierliatrnnnrhehi", j, 21);
	prt("rtsxna..dcedwlatdcedsrekdfddrxss", j, 22);
	borg_prt_binary(l_ptr->kn_flags2, j, 23);

	prt("+------------FLAGS3------------+", j + 32, 10);
	prt("fe      ehsi  st    iiiiadta  hp", j + 32, 11);
	prt("il   n taihnf ee    ggggcregb vr", j + 32, 12);
	prt("re  nowysdose eld   nnnntalrl ym", j + 32, 13);
	prt("ec  omrcyewta ieirmsrrrriieaeccc", j + 32, 14);
	prt("aa  taauktmatlnpgeihaefcvnpvsuuu", j + 32, 15);
	prt("uu  egirnyoahivaeggoclioaeoasrrr", j + 32, 16);
	prt("rr  litsopdretitsehtierltxrtesss", j + 32, 17);
	prt("aa  echewestreshtntsdcedeptedeee", j + 32, 18);
	borg_prt_binary(l_ptr->kn_flags3, j + 32, 19);
}


/*
 * Initialize the Borg
 */
void borg_init_9(void)
{
	/*** Hack -- initialize borg.ini options ***/

	/* Message */
	prt("Initializing the Borg... (zborg.txt)", 0, 0);

	/* Hack -- flush it */
	Term_fresh();

	init_borg_txt_file();

	/*** Hack -- initialize game options ***/

	/* Message */
	prt("Initializing the Borg... (options)", 0, 0);

	/* Hack -- flush it */
	Term_fresh();

	/* We use the original keypress codes */
	rogue_like_commands = FALSE;

	/* We pick up items when we step on them */
	always_pickup = TRUE;

	/* We specify targets by hand */
	use_old_target = FALSE;

	/* We must pick items up without verification */
	carry_query_flag = FALSE;

	/* We repeat by hand */
	always_repeat = FALSE;

	/* We need space */
	show_labels = FALSE;
	show_weights = FALSE;

	/* We need the dungeon level */
	depth_in_feet = FALSE;

	/* Allow items to stack */
	stack_force_costs = TRUE;

	/* Hack - we don't understand this */
	auto_destroy = FALSE;

	/* Do not confirm actions */
	confirm_wear = FALSE;
	confirm_stairs = FALSE;


	/* Zangband Commands */

	/* The borg doesn't understand the easy options */
	easy_floor = FALSE;
	easy_disarm = FALSE;
	easy_open = FALSE;

	/* The borg doesn't understand speaking uniques */
	speak_unique = FALSE;

	/* Hack -- notice "command" mode */
	hilite_player = FALSE;

	/*** Various ***/

	/* Message */
	prt("Initializing the Borg... (various)", 0, 0);

	/* Hack -- flush it */
	Term_fresh();


	/*** Cheat / Panic ***/

	/* more cheating */
	borg_cheat_death = FALSE;

	/* set the continous play mode if the game cheat death is on */
	if (cheat_live) borg_cheat_death = TRUE;

	/* Initialise player position */
	map_get_player(&c_x, &c_y);

	/*** Initialize ***/

	/* Initialize */
	borg_init_1();
	borg_init_2();
	borg_init_3();
	borg_init_4();
	borg_init_5();
	borg_init_6();
	borg_init_7();
	borg_init_8();

	/* Hack - initialise the hooks into the overhead map code */

	/* Initialise the overhead map code */
	init_overhead_map();

	/* Save the borg hooks into the overhead map */
	old_info_hook = set_callback((callback_type) borg_map_info, CALL_MAP_INFO);
	old_erase_hook = set_callback((callback_type) borg_map_erase,
								  CALL_MAP_ERASE);
	
	/* Save old player movement hook */
	old_move_hook = set_callback((callback_type) borg_player_move,
								  CALL_PLAYER_MOVE);

	/* Save the borg hooks for object lists */
	old_list_hook = set_callback((callback_type) borg_list_info,
								  CALL_OBJECT_LIST);

	/*** Redraw ***/

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Redraw everything */
	do_cmd_redraw();


	/*** Hack -- Extract race ***/

	/* Insert the player Race--cheat */
	borg_race = p_ptr->prace;

	/* Extract the race pointer */
	rb_ptr = &race_info[borg_race];


	/*** Hack -- Extract class ***/
	borg_class = p_ptr->pclass;

	/* Extract the class pointer */
	cb_ptr = &class_info[borg_class];

	/* Extract the magic pointer */
	pmb_ptr = &magic_info[borg_class];


	/*** Hack -- react to race and class ***/

	/* Notice the new race and class */
	prepare_race_class_info();


	/*** All done ***/

	/* Done initialization */
	prt("Initializing the Borg... done.", 0, 0);

	/* Clear line */
	prt("", 0, 0);

	/* Reset the clock */
	borg_t = 1000;

	/* Official message */
	borg_note("# Ready...");

	/* Now it is ready */
	initialized = TRUE;
}


static void borg_display_map_info(byte data, byte type)
{
	int x, y;

	char c = ' ';
	byte a = TERM_DARK;

	int wid, hgt;

	map_block *mb_ptr;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Remove map offset */
	wid -= COL_MAP + 1;
	hgt -= ROW_MAP + 1;

	for (x = panel_col_min; x < panel_col_max; x++)
	{
		for (y = panel_row_min; y < panel_row_max; y++)
		{
			a = TERM_DARK;
			c = ' ';

			if (map_in_bounds(x, y))
			{
				mb_ptr = map_loc(x, y);

				switch (type)
				{
					case BORG_SHOW_FEAT:
					{
						if (mb_ptr->feat == data)
						{
							a = TERM_WHITE;
							c = '*';
						}

						break;
					}

					case BORG_SHOW_INFO:
					{
						if (mb_ptr->info & data)
						{
							a = TERM_WHITE;
							c = '*';
						}

						break;
					}

					case BORG_SHOW_FLAG:
					{
						if (mb_ptr->flags & data)
						{
							a = TERM_WHITE;
							c = '*';
						}

						break;
					}

					case BORG_SHOW_FLOW:
					{
						if (mb_ptr->flow == data)
						{
							a = TERM_WHITE;
							c = '*';
						}

						break;
					}

					case BORG_SHOW_AVOID:
					{
						/* Obtain danger */
						int p = borg_danger(x, y, 1, TRUE);

						/* Skip non-avoidances */
						if (p <= avoidance / 3) break;

						if (p <= avoidance)
						{
							a = TERM_YELLOW;
						}
						else
						{
							a = TERM_RED;
						}

						break;
					}

					case BORG_SHOW_FEAR:
					{
						/* Obtain fear */
						int p = mb_ptr->fear;

						/* Skip non-avoidances */
						if (p <= avoidance / 10) break;

						if (p <= avoidance / 4)
						{
							a = TERM_YELLOW;
						}
						else
						{
							a = TERM_RED;
						}

						break;
					}

					case BORG_SHOW_STEP:
					{
						int i;

						/* Check for an existing step */
						for (i = 0; i < track_step_num; i++)
						{
							/* Stop if we already new about this glyph */
							if ((track_step_x[i] == x) &&
								(track_step_y[i] == y))
							{
								a = TERM_WHITE;
								c = '*';

								break;
							}
						}
						break;
					}
				}
			}


			if (c != ' ')
			{
				/* Hack -- Queue it */
				Term_queue_char(x - panel_col_prt, y - panel_row_prt, a, c,
								a, c);
			}
		}
	}
}


/* DVE's function for displaying the status of various info */
/* Display what the borg is thinking DvE*/
void borg_status(void)
{
	int j;

#if 0
	quest_type *q_ptr;
	monster_race *r_ptr;
	int q_num;
	char name[80];
#endif /* 0 */

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* Unused */
		if (!angband_term[j]) continue;

		/* Check for borg status term */
		if (window_flag[j] & (PW_BORG_2))
		{
			byte attr;

			/* Activate */
			Term_activate(angband_term[j]);

			/* Display what resists the borg (thinks he) has */
			Term_putstr(5, 0, -1, TERM_WHITE, "RESISTS");

			/* Basic four */
			attr = TERM_SLATE;
			if (borg_skill[BI_RACID]) attr = TERM_BLUE;
			if (my_oppose_acid) attr = TERM_GREEN;
			if (borg_skill[BI_IACID]) attr = TERM_WHITE;
			Term_putstr(1, 1, -1, attr, "Acid");

			attr = TERM_SLATE;
			if (borg_skill[BI_RELEC]) attr = TERM_BLUE;
			if (my_oppose_elec) attr = TERM_GREEN;
			if (borg_skill[BI_IELEC]) attr = TERM_WHITE;
			Term_putstr(1, 2, -1, attr, "Elec");

			attr = TERM_SLATE;
			if (borg_skill[BI_RFIRE]) attr = TERM_BLUE;
			if (my_oppose_fire) attr = TERM_GREEN;
			if (borg_skill[BI_IFIRE]) attr = TERM_WHITE;
			Term_putstr(1, 3, -1, attr, "Fire");

			attr = TERM_SLATE;
			if (borg_skill[BI_RCOLD]) attr = TERM_BLUE;
			if (my_oppose_cold) attr = TERM_GREEN;
			if (borg_skill[BI_ICOLD]) attr = TERM_WHITE;
			Term_putstr(1, 4, -1, attr, "Cold");

			/* High resists */
			attr = TERM_SLATE;
			if (borg_skill[BI_RPOIS]) attr = TERM_BLUE;
			if (my_oppose_pois) attr = TERM_GREEN;
			Term_putstr(1, 5, -1, attr, "Pois");

			if (borg_skill[BI_RFEAR]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(1, 6, -1, attr, "Fear");

			if (borg_skill[BI_RLITE]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(1, 7, -1, attr, "Lite");

			if (borg_skill[BI_RDARK]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(1, 8, -1, attr, "Dark");

			if (borg_skill[BI_RBLIND]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(6, 1, -1, attr, "Blind");

			if (borg_skill[BI_RCONF]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(6, 2, -1, attr, "Confu");

			if (borg_skill[BI_RSND]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(6, 3, -1, attr, "Sound");

			if (borg_skill[BI_RSHRD]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(6, 4, -1, attr, "Shard");

			if (borg_skill[BI_RNXUS]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(6, 5, -1, attr, "Nexus");

			if (borg_skill[BI_RNTHR]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(6, 6, -1, attr, "Nethr");

			if (borg_skill[BI_RKAOS]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(6, 7, -1, attr, "Chaos");

			if (borg_skill[BI_RDIS]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(6, 8, -1, attr, "Disen");

			/* Other abilities */
			if (borg_skill[BI_SDIG]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(12, 1, -1, attr, "S.Dig");

			if (borg_skill[BI_FEATH]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(12, 2, -1, attr, "Feath");

			if (borg_skill[BI_LITE]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(12, 3, -1, attr, "PLite");

			if (borg_skill[BI_REG]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(12, 4, -1, attr, "Regen");

			if (borg_skill[BI_ESP]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(12, 5, -1, attr, "Telep");

			if (borg_skill[BI_SINV]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(12, 6, -1, attr, "Invis");

			if (borg_skill[BI_FRACT]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(12, 7, -1, attr, "FrAct");

			if (borg_skill[BI_HLIFE]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(12, 8, -1, attr, "HLife");

			/* Display the slays */
			Term_putstr(5, 10, -1, TERM_WHITE, "Weapon Slays:");

			if (borg_skill[BI_WS_ANIMAL]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(1, 11, -1, attr, "Animal");

			if (borg_skill[BI_WS_EVIL]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(8, 11, -1, attr, "Evil");

			if (borg_skill[BI_WS_UNDEAD]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(15, 11, -1, attr, "Undead");

			if (borg_skill[BI_WS_DEMON]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(22, 11, -1, attr, "Demon");

			if (borg_skill[BI_WS_ORC]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(1, 12, -1, attr, "Orc");

			if (borg_skill[BI_WS_TROLL]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(8, 12, -1, attr, "Troll");

			if (borg_skill[BI_WS_GIANT]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(15, 12, -1, attr, "Giant");

			if (borg_skill[BI_WS_DRAGON]) attr = TERM_BLUE;
			if (borg_skill[BI_WK_DRAGON]) attr = TERM_GREEN;
			else
				attr = TERM_SLATE;
			Term_putstr(22, 12, -1, attr, "Dragon");

			if (borg_skill[BI_WB_ACID]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(1, 13, -1, attr, "Acid");

			if (borg_skill[BI_WB_COLD]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(8, 13, -1, attr, "Cold");

			if (borg_skill[BI_WB_ELEC]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(15, 13, -1, attr, "Elec");

			if (borg_skill[BI_WB_FIRE]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(22, 13, -1, attr, "Fire");


			/* Display the Concerns */
			Term_putstr(36, 10, -1, TERM_WHITE, "Concerns:");

			if (borg_wearing_cursed) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(29, 11, -1, attr, "Cursed");

			if (borg_skill[BI_ISWEAK]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(36, 11, -1, attr, "Weak");

			if (borg_skill[BI_ISPOISONED]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(43, 11, -1, attr, "Poison");

			if (borg_skill[BI_ISCUT]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(29, 12, -1, attr, "Cut");

			if (borg_skill[BI_ISSTUN]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(36, 12, -1, attr, "Stun");

			if (borg_skill[BI_ISCONFUSED]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(43, 12, -1, attr, "Confused");

			if (borg_skill[BI_ISFIXEXP]) attr = TERM_BLUE;
			else
				attr = TERM_SLATE;
			Term_putstr(43, 13, -1, attr, "Exp Drain");

			/* Display the Time */
			Term_putstr(60, 10, -1, TERM_WHITE, "Time:");

			Term_putstr(54, 11, -1, TERM_SLATE, "This Level         ");
			Term_putstr(65, 11, -1, TERM_WHITE,
						format("%d", borg_t - borg_began));

			Term_putstr(54, 12, -1, TERM_SLATE, "Since Town         ");
			Term_putstr(65, 12, -1, TERM_WHITE,
						format("%d", borg_time_town + (borg_t - borg_began)));


			/* Sustains */
			Term_putstr(19, 0, -1, TERM_WHITE, "Sustains");

			if (borg_skill[BI_SSTR]) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(21, 1, -1, attr, "STR");

			if (borg_skill[BI_SINT]) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(21, 2, -1, attr, "INT");

			if (borg_skill[BI_SWIS]) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(21, 3, -1, attr, "WIS");

			if (borg_skill[BI_SDEX]) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(21, 4, -1, attr, "DEX");

			if (borg_skill[BI_SCON]) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(21, 5, -1, attr, "CON");

			if (borg_skill[BI_SCHR]) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(21, 6, -1, attr, "CHR");


			/* Temporary effects */
			Term_putstr(28, 0, -1, TERM_WHITE, "Temp Effects");

			if (borg_prot_from_evil) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(28, 1, -1, attr, "Prot. Evil");

			if (borg_goi) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(28, 2, -1, attr, "Invulnerable");

			if (borg_hero) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(28, 3, -1, attr, "Heroism");

			if (borg_berserk) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(28, 4, -1, attr, "Berserk");

			if (borg_shield) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(28, 5, -1, attr, "Shielded");

			if (borg_bless) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(28, 6, -1, attr, "Blessed");

			if (borg_speed) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(28, 7, -1, attr, "Fast");

			if (borg_inviso) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(28, 8, -1, attr, "Invisible");

			/* Temporary effects */
			Term_putstr(42, 0, -1, TERM_WHITE, "Level Information");

			if (vault_on_level) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(42, 1, -1, attr, "Vault on level");

			if (unique_on_level) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(42, 2, -1, attr, "Unique on level");
			if (unique_on_level) Term_putstr(58, 2, -1, attr, format("(%s)",
																	 r_name +
																	 r_info[(int)unique_on_level].name));
			else
				Term_putstr(58, 2, -1, attr,
							"                                   ");
			if (breeder_level) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(42, 4, -1, attr,
						"Breeder level (close the door, will ye)");

			/* level preparedness */
			attr = TERM_SLATE;
			Term_putstr(42, 6, -1, attr, "Reason for not diving:");
			attr = TERM_WHITE;
			Term_putstr(64, 6, -1, attr,
						format("%s                              ",
							   borg_prepared(borg_skill[BI_MAXDEPTH] + 1)));

			if (goal_fleeing) attr = TERM_WHITE;
			else
				attr = TERM_SLATE;
			Term_putstr(42, 7, -1, attr, "Fleeing Level");

			attr = TERM_SLATE;
			Term_putstr(42, 8, -1, attr, "Maximal Depth:");
			attr = TERM_WHITE;
			Term_putstr(56, 8, -1, attr,
						format("%d    ", borg_skill[BI_MAXDEPTH]));

			/* Fresh */
			Term_fresh();

			/* Restore */
			Term_activate(old);
		}
	}
}


/* keep a log of certain battles, used on Questor uniques */
void borg_log_battle(bool keep_log)
{
	/* char buf[80]; */

	(void)keep_log;

/* for now the log is deactivated */
	return;
#if 0
	/* Close the log file */
	if (!keep_log)
	{
		borg_info("Closing Battle Log");
		borg_info("**************");
		my_fclose(borg_fff);
		borg_fff = NULL;		/* needed on unix! */
		return;
	}

	/* Hack -- drop permissions */
	safe_setuid_drop();

	/* Default  */
	strcpy(buf, "battle.log");

	/* Open a new file */
	borg_fff = my_fopen(buf, "a");
	/* Failure */
	if (!borg_fff) msg_print("Cannot open that file.");

	/* Note start of log */
	borg_info("**************");
	borg_info("Opening Battle Log");

	/* Hack -- grab permissions */
	safe_setuid_grab();
	return;
#endif /* 0 */
}


/*
 * Hack -- interact with the "Ben Borg".
 */
void do_cmd_borg(void)
{
	char cmd;

	/* Get a "Borg command", or abort */
	if (!get_com("Borg command: ", &cmd)) return;

	/* Simple help */
	if (cmd == '?')
	{
		int i = 2;

		/* Save the screen */
		Term_save();

		/* Clear the screen */
		Term_clear();

		i++;
		Term_putstr(2, i, -1, TERM_WHITE, "Command 'z' activates the Borg.");
		Term_putstr(42, i++, -1, TERM_WHITE, "Command 'u' updates the Borg.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command 'x' steps the Borg.");
		Term_putstr(42, i, -1, TERM_WHITE,
					"Command 'f' modifies the normal flags.");
		Term_putstr(2, i++, -1, TERM_WHITE,
					"Command 'c' modifies the cheat flags.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'l' activates a log file.");
		Term_putstr(2, i++, -1, TERM_WHITE,
					"Command 's' activates search mode.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'i' displays grid info.");
		Term_putstr(2, i++, -1, TERM_WHITE,
					"Command 'g' displays grid feature.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'a' displays avoidances.");
		Term_putstr(2, i++, -1, TERM_WHITE,
					"Command 'k' displays monster info.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 't' displays object info.");
		Term_putstr(2, i++, -1, TERM_WHITE,
					"Command '%' displays current flow.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command '#' displays danger grid.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command '_' Regional Fear info.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'p' Borg Power.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command '1' change max depth.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command '2' level prep info.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command 'e' Examine Equip Item.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command '!' Time.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command '@' Borg LOS.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'w' My Swap Weapon.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command 'q' Auto stop on level.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'v' Version stamp.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command 'd' Dump spell info.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'h' Borg_Has function.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command '$' Reload Borg.txt.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'y' Last 75 steps.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command '^' Flow Pathway.");
		Term_putstr(42, i, -1, TERM_WHITE, "Command 'o' Examine Inven Item.");
		Term_putstr(2, i++, -1, TERM_WHITE, "Command 'R' Respawn Borg.");


		/* Prompt for key */
		msg_print("Commands: ");
		msg_print(NULL);

		/* Restore the screen */
		Term_load();

		/* Done */
		return;
	}

	/* Hack -- force initialization */
	if (!initialized) borg_init_9();

	switch (cmd)
	{
		case '$':
		{
			/* Command: Nothing */

			/*** Hack -- initialize borg.ini options ***/

			/* Message */
			borg_note("Reloading the Borg rules... (zborg.txt)");

			KILL(borg_has);

			init_borg_txt_file();
			borg_note("# Ready...");
			break;
		}

		case 'z':
		case 'Z':
		{
			/* Command: Activate */

			/* Activate */
			borg_active = TRUE;

			/* Reset cancel */
			borg_cancel = FALSE;

			/* Step forever */
			borg_step = 0;

			/*** Redraw ***/

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD);

			/* Redraw everything */
			do_cmd_redraw();

			/* need to check all stats */
			my_need_stat_check[0] = TRUE;
			my_need_stat_check[1] = TRUE;
			my_need_stat_check[2] = TRUE;
			my_need_stat_check[3] = TRUE;
			my_need_stat_check[4] = TRUE;
			my_need_stat_check[5] = TRUE;

			/* Allowable Cheat -- Obtain "recall" flag */
			goal_recalling = p_ptr->word_recall * 1000;

			/* Allowable Cheat -- Obtain "prot_from_evil" flag */
			borg_prot_from_evil = (p_ptr->protevil ? TRUE : FALSE);

			/* Allowable Cheat -- Obtain "speed" flag */
			borg_speed = (p_ptr->fast ? TRUE : FALSE);

			/* Allowable Cheat -- Obtain "goi" flag */
			borg_goi = (p_ptr->invuln ? 9000 : 0);
			borg_inviso = (p_ptr->tim_invis ? 9000 : 0);

			/* Allowable Cheat -- Obtain "resist" flags */
			my_oppose_acid = (p_ptr->oppose_acid ? TRUE : FALSE);
			my_oppose_elec = (p_ptr->oppose_elec ? TRUE : FALSE);
			my_oppose_fire = (p_ptr->oppose_fire ? TRUE : FALSE);
			my_oppose_cold = (p_ptr->oppose_cold ? TRUE : FALSE);
			my_oppose_pois = (p_ptr->oppose_pois ? TRUE : FALSE);
			borg_bless = (p_ptr->blessed ? TRUE : FALSE);
			borg_shield = (p_ptr->shield ? TRUE : FALSE);
			borg_hero = (p_ptr->hero ? TRUE : FALSE);
			borg_berserk = (p_ptr->shero ? TRUE : FALSE);

			/* Message */
			borg_note("# Installing keypress hook");

			/* Activate the key stealer */
			inkey_hack = borg_inkey_hack;

			break;
		}

		case 'u':
		case 'U':
		{
			/* Command: Update */

			/* Activate */
			borg_active = TRUE;

			/* Immediate cancel */
			borg_cancel = TRUE;

			/* Step forever */
			borg_step = 0;

			/* Allowable Cheat -- Obtain "recall" flag */
			goal_recalling = p_ptr->word_recall * 1000;

			/* Allowable Cheat -- Obtain "prot_from_evil" flag */
			borg_prot_from_evil = (p_ptr->protevil ? TRUE : FALSE);
			/* Allowable Cheat -- Obtain "speed" flag */
			borg_speed = (p_ptr->fast ? TRUE : FALSE);
			/* Allowable Cheat -- Obtain "goi" flag */
			borg_goi = (p_ptr->invuln ? 9000 : 0);
			borg_inviso = (p_ptr->tim_invis ? 9000 : 0);
			/* Allowable Cheat -- Obtain "resist" flags */
			my_oppose_acid = (p_ptr->oppose_acid ? TRUE : FALSE);
			my_oppose_elec = (p_ptr->oppose_elec ? TRUE : FALSE);
			my_oppose_fire = (p_ptr->oppose_fire ? TRUE : FALSE);
			my_oppose_cold = (p_ptr->oppose_cold ? TRUE : FALSE);
			my_oppose_pois = (p_ptr->oppose_pois ? TRUE : FALSE);
			borg_bless = (p_ptr->blessed ? TRUE : FALSE);
			borg_shield = (p_ptr->shield ? TRUE : FALSE);
			borg_hero = (p_ptr->hero ? TRUE : FALSE);
			borg_berserk = (p_ptr->shero ? TRUE : FALSE);

			/* Message */
			borg_note("# Installing keypress hook");

			/* Activate the key stealer */
			inkey_hack = borg_inkey_hack;

			break;
		}

		case 'x':
		case 'X':
		{
			/* Command: Step */

			/* Activate */
			borg_active = TRUE;

			/* Reset cancel */
			borg_cancel = FALSE;

			/* Step N times */
			borg_step = 1;

			/*** Redraw ***/

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD);

			/* Redraw everything */
			do_cmd_redraw();

			/* need to check all stats */
			my_need_stat_check[0] = TRUE;
			my_need_stat_check[1] = TRUE;
			my_need_stat_check[2] = TRUE;
			my_need_stat_check[3] = TRUE;
			my_need_stat_check[4] = TRUE;
			my_need_stat_check[5] = TRUE;

			/* Allowable Cheat -- Obtain "recall" flag */
			goal_recalling = p_ptr->word_recall * 1000;
			/* Allowable Cheat -- Obtain "prot_from_evil" flag */
			borg_prot_from_evil = (p_ptr->protevil ? TRUE : FALSE);
			/* Allowable Cheat -- Obtain "speed" flag */
			borg_speed = (p_ptr->fast ? TRUE : FALSE);
			/* Allowable Cheat -- Obtain "goi" flag */
			borg_goi = (p_ptr->invuln ? 9000 : 0);
			borg_inviso = (p_ptr->tim_invis ? 9000 : 0);
			/* Allowable Cheat -- Obtain "resist" flags */
			my_oppose_acid = (p_ptr->oppose_acid ? TRUE : FALSE);
			my_oppose_elec = (p_ptr->oppose_elec ? TRUE : FALSE);
			my_oppose_fire = (p_ptr->oppose_fire ? TRUE : FALSE);
			my_oppose_cold = (p_ptr->oppose_cold ? TRUE : FALSE);
			my_oppose_pois = (p_ptr->oppose_pois ? TRUE : FALSE);
			borg_bless = (p_ptr->blessed ? TRUE : FALSE);
			borg_shield = (p_ptr->shield ? TRUE : FALSE);
			borg_hero = (p_ptr->hero ? TRUE : FALSE);
			borg_berserk = (p_ptr->shero ? TRUE : FALSE);

			/* Message */
			borg_note("# Installing keypress hook");

			/* Activate the key stealer */
			inkey_hack = borg_inkey_hack;

			break;
		}

		case 'f':
		case 'F':
		{
			/* Command: toggle "flags" */

			/* Get a "Borg command", or abort */
			if (!get_com("Borg command: Toggle Flag: (m/s) ", &cmd)) return;

			switch (cmd)
			{
				case 'm':
				case 'M':
				{
					/* Give borg thought messages in window */
					break;
				}

				case 'd':
				case 'D':
				{
					/* Dump savefile at each death */
					borg_flag_dump = !borg_flag_dump;
					msg_format("Borg -- borg_flag_dump is now %d.",
							   borg_flag_dump);
					break;
				}
			}
			break;
		}

		case 'c':
		case 'C':
		{
			/* Command: toggle "cheat" flags */

			/* Get a "Borg command", or abort */
			if (!get_com("Borg command: Toggle Cheat: (d/i/e/s/p)", &cmd))
				return;

			switch (cmd)
			{
				case 'd':
				case 'D':
				{
					borg_cheat_death = !borg_cheat_death;
					msg_format("Borg -- borg_cheat_death is now %d.",
							   borg_cheat_death);
					break;
				}
			}
			break;

		}

		case 'l':
		case 'L':
		{
			/* Start a new log file */

			char buf[80];

			/* Close the log file */
			if (borg_fff) my_fclose(borg_fff);

			/* Hack -- drop permissions */
			safe_setuid_drop();

			/* Default  */
			strcpy(buf, "borg.log");

			/* XXX XXX XXX Get the name and open the log file */
			if (get_string("Borg Log File: ", buf, 70))
			{
				/* Open a new file */
				borg_fff = my_fopen(buf, "w");

				/* Failure */
				if (!borg_fff) msg_print("Cannot open that file.");
			}

			/* Hack -- grab permissions */
			safe_setuid_grab();
			break;
		}

		case 's':
		case 'S':
		{
			/* Activate a search string */

			/* Get the new search string (or cancel the matching) */
			if (!get_string("Borg Match String: ", borg_match, 70))
			{
				/* Cancel it */
				strcpy(borg_match, "");

				/* Message */
				msg_print("Borg Match String de-activated.");
			}
			break;
		}

		case 'g':
		case 'G':
		{
			/* Command: check Grid "feature" flags */

			u16b feat = 0;

			/* Get a "Borg command", or abort */
			if (!get_com("Borg command: Show grids: ", &cmd)) return;

			/* Extract a flag */
			switch (cmd)
			{
				case '.':
				{
					feat = FEAT_FLOOR;
					break;
				}
				case ',':
				{
					feat = FEAT_OPEN;
					break;
				}
				case 'x':
				{
					feat = FEAT_BROKEN;
					break;
				}
				case '<':
				{
					feat = FEAT_LESS;
					break;
				}
				case '>':
				{
					feat = FEAT_MORE;
					break;
				}
				case '+':
				{
					feat = FEAT_CLOSED;
					break;
				}
				case 's':
				{
					feat = FEAT_SECRET;
					break;
				}
				case ':':
				{
					feat = FEAT_RUBBLE;
					break;
				}
				case 't':
				{
					feat = FEAT_TREES;
					break;
				}
				case 'l':
				{
					feat = FEAT_SHAL_LAVA;
					break;
				}
				case 'L':
				{
					feat = FEAT_DEEP_LAVA;
					break;
				}
				case 'a':
				{
					feat = FEAT_SHAL_WATER;
					break;
				}
				case 'A':
				{
					feat = FEAT_DEEP_WATER;
					break;
				}

				case 'w':
				{
					feat = FEAT_WALL_EXTRA;
					break;
				}
				case 'p':
				{
					feat = FEAT_PERM_EXTRA;
					break;
				}

				default:
				{
					feat = FEAT_NONE;
					break;
				}
			}

			/* Show it */
			borg_display_map_info(feat, BORG_SHOW_FEAT);

			/* Get keypress */
			msg_print("Press any key.");
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case 'i':
		case 'I':
		{
			/* Command: check "info" flags */
			u16b mask;

			/* Get a "Borg command", or abort */
			if (!get_com("Borg command: Show grids: ", &cmd)) return;

			/* Extract a flag */
			switch (cmd)
			{
				case '0':
				{
					mask = 1 << 0;
					break;
				}
				case '1':
				{
					mask = 1 << 1;
					break;
				}
				case '2':
				{
					mask = 1 << 2;
					break;
				}
				case '3':
				{
					mask = 1 << 3;
					break;
				}
				case '4':
				{
					mask = 1 << 4;
					break;
				}
				case '5':
				{
					mask = 1 << 5;
					break;
				}
				case '6':
				{
					mask = 1 << 6;
					break;
				}
				case '7':
				{
					mask = 1 << 7;
					break;
				}

				default:
				{
					mask = 0x00;
					break;
				}
			}

			/* Show it */
			borg_display_map_info(mask, BORG_SHOW_FLAG);

			/* Get keypress */
			msg_print("Press any key.");
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case 'a':
		case 'A':
		{
			/* Command: check "avoidances" */

			/* Show it */
			borg_display_map_info(0, BORG_SHOW_AVOID);

			/* Get keypress */
			msg_format("(%d,%d) Avoidance value %d.", c_x, c_y, avoidance);
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case 'y':
		{
			/* Command: check previous steps */

			/* Show it */
			borg_display_map_info(0, BORG_SHOW_STEP);

			/* Get keypress */
			msg_format("(%d) Steps noted", track_step_num);
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case 'k':
		case 'K':
		{
			/* Command: show "monsters" */
			int i, n = 0;

			/* Scan the monsters */
			for (i = 1; i < borg_kills_nxt; i++)
			{
				borg_kill *kill = &borg_kills[i];

				/* Still alive */
				if (kill->r_idx)
				{
					int x = kill->x;
					int y = kill->y;

					/* Display */
					print_rel('*', TERM_RED, x, y);

					/* Count */
					n++;
				}
			}

			/* Get keypress */
			msg_format("There are %d known monsters.", n);
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case 't':
		case 'T':
		{
			/* Command: show "objects" */
			int i, n = 0;

			/* Scan the objects */
			for (i = 1; i < borg_takes_nxt; i++)
			{
				borg_take *take = &borg_takes[i];

				/* Still alive */
				if (take->k_idx)
				{
					int x = take->x;
					int y = take->y;

					/* Display */
					print_rel('*', TERM_RED, x, y);

					/* Count */
					n++;
				}
			}

			/* Get keypress */
			msg_format("There are %d known objects.", n);
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case '8':
		{
			/* Command: debug -- show shops */
			int i, n = 0;

			for (i = 0; i < track_shop_num; i++)
			{
				/* Print */
				print_rel('*', TERM_RED, borg_shops[i].x, borg_shops[i].y);

				/* Count */
				n++;
			}

			/* Get keypress */
			msg_format("There are %d known shops.", n);
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case '%':
		{
			/* Command: debug -- current flow */
			int i;

			/* Flow */
			for (i = 0; i < 250; i++)
			{
				/* Show it */
				borg_display_map_info(i, BORG_SHOW_FLOW);

				/* Get keypress */
				msg_format("Flow depth %d.", i);
				msg_print(NULL);

				if (!get_check("Continue?")) break;

				/* Redraw map */
				prt_map();
			}
			break;
		}

		case '^':
		{
			/* Display the intended path to the flow */
			int x, y;
			int o;
			int false_y, false_x;

			false_y = c_y;
			false_x = c_x;

			/* Continue */
			for (o = 0; o < 250; o++)
			{
				int b_n = 0;

				int i, b_i = -1;

				int c, b_c;

				map_block *mb_ptr = map_loc(c_x, c_y);


				/* Flow cost of current grid */
				b_c = mb_ptr->flow * 10;

				/* Prevent loops */
				b_c = b_c - 5;

				/* Look around */
				for (i = 0; i < 8; i++)
				{
					/* Grid in that direction */
					x = false_x + ddx_ddd[i];
					y = false_y + ddy_ddd[i];

					/* Bounds checking */
					if (!map_in_bounds(x, y)) continue;

					/* Access the grid */
					mb_ptr = map_loc(x, y);

					/* Flow cost at that grid */
					c = mb_ptr->flow * 10;

					/* Never backtrack */
					if (c > b_c) continue;

					/* Notice new best value */
					if (c < b_c) b_n = 0;

					/* Apply the randomizer to equivalent values */
					if ((++b_n >= 2) && (randint0(b_n))) continue;

					/* Track it */
					b_i = i;
					b_c = c;
				}

				/* Try it */
				if (b_i >= 0)
				{
					/* Access the location */
					x = false_x + ddx_ddd[b_i];
					y = false_y + ddy_ddd[b_i];

					/* Display */
					print_rel('*', TERM_RED, x, y);

					/* Simulate motion */
					false_y = y;
					false_x = x;
				}

			}
			msg_print("Probable Flow Path");
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case '#':
		{
			/* Command: debug -- danger of grid */
			int n = 0;

			/* Turns */
			n = (p_ptr->command_arg ? p_ptr->command_arg : 1);

			/* Danger of grid */
			msg_format("Danger(%d,%d,%d) is %d",
					   p_ptr->target_col, p_ptr->target_row, n,
					   borg_danger(p_ptr->target_col, p_ptr->target_row, n,
								   TRUE));
			break;
		}

		case '_':
		{
			/* Command:  Regional Fear Info */

			/* Show it */
			borg_display_map_info(0, BORG_SHOW_FEAR);

			/* Get keypress */
			msg_format("(%d,%d) Regional Fear.", c_x, c_y);
			msg_print(NULL);

			/* Redraw map */
			prt_map();
			break;
		}

		case 'p':
		case 'P':
		{
			/* Command: debug -- Power */
			s32b p;

			/* Examine the screen */
			borg_update_frame();

			/* Examine the screen */
			borg_update();

			/* Extract some "hidden" variables */
			borg_hidden();

			/* Evaluate */
			p = borg_power();

			/* Report it */
			msg_format("Current Borg Power %ld", p);
			msg_format("Current Home Power %ld", borg_power_home());

			break;
		}

		case '!':
		{
			/* Command: Show time */
			s32b time = borg_t - borg_began;

			msg_format("time: (%d) ", (int)time);
			time = (borg_time_town + (borg_t - borg_began));

			msg_format("; from town (%d)", (int)time);
			msg_format("; need inviso (%d)", (int)need_see_inviso);
			break;
		}

		case '1':
		{
			/* APW command: debug -- change max depth */
			int new_borg_skill[BI_MAXDEPTH];

			/* Get the new max depth */
			new_borg_skill[BI_MAXDEPTH] =
				get_quantity("Enter new Max Depth: ", MAX_DEPTH - 1);

			/* Allow user abort */
			if (new_borg_skill[BI_MAXDEPTH] >= 0)
			{
				p_ptr->max_depth = new_borg_skill[BI_MAXDEPTH];
				borg_skill[BI_MAXDEPTH] = new_borg_skill[BI_MAXDEPTH];
			}

			break;
		}

		case 'q':
		{
			char cmd;

			get_com("Stop when Morgoth Dies? (y or n)? ", &cmd);

			if (cmd == 'n' || cmd == 'N') borg_stop_king = FALSE;

			break;
		}

		case '2':
		{
			/* Command: APW HACK debug -- preparation for level */

			int i = 0;

			/* Examine the screen */
			borg_update_frame();
			borg_update();

			/* Extract some "hidden" variables */
			borg_hidden();

			/* Examine the inventory */
			borg_notice();
			borg_notice_home();

			/* Dump prep codes */
			for (i = 1; i <= 127; i++)
			{
				/* Dump fear code */
				if ((cptr)NULL != borg_prepared(i)) break;
			}

			msg_format("Max Level: %d  Prep'd For: %d  Reason: %s",
					   borg_skill[BI_MAXDEPTH], i - 1, borg_prepared(i));

			break;
		}

		case 'o':
		case 'O':
		{
			/* Command: Display all known info on item */
			int n = p_ptr->command_arg - 1;

			/* Paranoia */
			if (n < 0) n = 0;
			if (n >= inven_num) break;

			/* Save the screen */
			Term_save();

			/* Display the special screen */
			borg_display_item(&inventory[n]);

			/* pause for study */
			msg_format("Borg believes: ");
			msg_print(NULL);

			/* Restore the screen */
			Term_load();


			break;
		}

		case 'e':
		case 'E':
		{
			/* Command: Display all known info on item */
			int n = p_ptr->command_arg - 1;

			/* Paranoia */
			if (n < 0) n = 0;
			if (n >= equip_num) break;

			/* Save the screen */
			Term_save();

			/* Display the special screen */
			borg_display_item(&equipment[n]);

			/* pause for study */
			msg_format("Borg believes: ");
			msg_print(NULL);

			/* Restore the screen */
			Term_load();

			break;
		}


		case 'd':
		case 'D':
		{
			/* Dump realms */
			int ii = 1, k;

			/* Save the screen */
			Term_save();

			/* Dump the first realm spells */
			for (k = 1; k < MAX_REALM; k++)
			{
				int i = 0, j;

				/* skip wrong realms */
				if (k != borg_skill[BI_REALM1] &&
					k != borg_skill[BI_REALM2]) continue;

				ii++;

				/* Books */
				for (i = 0; i < 4; i++)
				{

					/* Clear the screen */
					Term_clear();

					Term_putstr(1, ii++, -1, TERM_WHITE,
								format("[ Realm 1 ]:(%s)",
									   &borg_magics[k][0][0].realm_name));
					for (j = 0; j < 8; j++)
					{
						borg_magic *as =
							&borg_magics[borg_skill[BI_REALM1]][i][j];
						cptr legal = NULL;

						if (as->level < 99)
						{
							legal =
								(borg_spell_legal(borg_skill[BI_REALM1], i, j) ?
								 "legal" : "Not Legal ");
						}
						Term_putstr(1, ii++, -1, TERM_WHITE,
									format("%s, %s, attempted %d times",
										   as->name, legal, as->times));
					}
					get_com("Exam spell books.  Press any key for next book.",
							&cmd);
				}
			}

			/* Restore the screen */
			Term_load();

			/* Done */
			return;
		}

		default:
		{
			/* Oops */

			/* Message */
			msg_print("That is not a legal Borg command.");
			break;
		}
	}
}

#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
