/* File: interp1.c */

/* Purpose: general script commands */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include "struct.h"
#include "icon.h"


/* d_attr characters */
cptr g_attr_str = "dwsorgbuDWvyRGBU";

/* Last known target state */
int target_state = 0;

/* Visibility of target */
bool target_vis = FALSE;

/* TRUE if current command is repeated */
bool command_repeating = FALSE;

/* NULL-terminated lists */
cptr *keyword_gender;
cptr *keyword_race;
cptr *keyword_class;

int inkey_flags = 0;
int inkey_book;
int exit_skip_save = FALSE;

/* Macro used to initialize the g_tval array below */
#define TVAL_DESC(tval) {#tval, tval}

/*
 * The g_tval array associates a textual string for each TV_XXX constant
 * with the integer value of the constant. It is used to initialize the
 * g_hash_tval hash table.
 */
t_tval g_tval[] = {
	TVAL_DESC(TV_ANY),
	TVAL_DESC(TV_SKELETON),
	TVAL_DESC(TV_BOTTLE),
	TVAL_DESC(TV_JUNK),
	TVAL_DESC(TV_SPIKE),
	TVAL_DESC(TV_CHEST),
	TVAL_DESC(TV_FIGURINE),
	TVAL_DESC(TV_STATUE),
	TVAL_DESC(TV_SHOT),
	TVAL_DESC(TV_ARROW),
	TVAL_DESC(TV_BOLT),
	TVAL_DESC(TV_BOW),
	TVAL_DESC(TV_DIGGING),
	TVAL_DESC(TV_HAFTED),
	TVAL_DESC(TV_POLEARM),
	TVAL_DESC(TV_SWORD),
	TVAL_DESC(TV_BOOTS),
	TVAL_DESC(TV_GLOVES),
	TVAL_DESC(TV_HELM),
	TVAL_DESC(TV_CROWN),
	TVAL_DESC(TV_SHIELD),
	TVAL_DESC(TV_CLOAK),
	TVAL_DESC(TV_SOFT_ARMOR),
	TVAL_DESC(TV_HARD_ARMOR),
	TVAL_DESC(TV_DRAG_ARMOR),
	TVAL_DESC(TV_LITE),
	TVAL_DESC(TV_AMULET),
	TVAL_DESC(TV_RING),
	TVAL_DESC(TV_STAFF),
	TVAL_DESC(TV_WAND),
	TVAL_DESC(TV_ROD),
	TVAL_DESC(TV_SCROLL),
	TVAL_DESC(TV_POTION),
	TVAL_DESC(TV_FLASK),
	TVAL_DESC(TV_FOOD),
	TVAL_DESC(TV_LIFE_BOOK),
	TVAL_DESC(TV_SORCERY_BOOK),
	TVAL_DESC(TV_NATURE_BOOK),
	TVAL_DESC(TV_CHAOS_BOOK),
	TVAL_DESC(TV_DEATH_BOOK),
	TVAL_DESC(TV_TRUMP_BOOK),
	TVAL_DESC(TV_ARCANE_BOOK),
	TVAL_DESC(TV_GOLD),
	{NULL, 0}
};

/*
 * The g_tval_str hash table is used to look up the integer value 
 * of a TV_XXX constant given the textual string.
 */
Tcl_HashTable *g_tval_str = NULL;

/*
 * The g_tval_const hash table is used to look up the textual string 
 * of a TV_XXX constant given the integer value. You could use a 101-element
 * array, but there are a lot of holes in TV_XXX constants.
 */
Tcl_HashTable *g_tval_const = NULL;

/*
 * Initialize the g_tval_by_XXX hash tables
 */
static void init_tval_hash(void)
{
	int i, new;
	Tcl_HashEntry *hPtr;

	/* Allocate the g_tval_str hash table */
	g_tval_str = (Tcl_HashTable *) Tcl_Alloc(sizeof(Tcl_HashTable));

	/* Initialize the g_tval_str hash table. */
	Tcl_InitHashTable(g_tval_str, TCL_STRING_KEYS);

	/* Allocate the g_tval_const hash table */
	g_tval_const = (Tcl_HashTable *) Tcl_Alloc(sizeof(Tcl_HashTable));

	/* Initialize the g_tval_const hash table. */
	Tcl_InitHashTable(g_tval_const, TCL_ONE_WORD_KEYS);

	/* Iterate over each key/value */
	for (i = 0; g_tval[i].key; i++)
	{
		/* Create a new hash table entry with key = TV_XXX string */
		hPtr = Tcl_CreateHashEntry(g_tval_str, g_tval[i].key, &new);

		/* Set the hash table entry with value = TV_XXX integer value */
		Tcl_SetHashValue(hPtr, g_tval[i].value);

		/* Create a new hash table entry with key = TV_XXX constant */
		hPtr = Tcl_CreateHashEntry(g_tval_const,
			(char *) g_tval[i].value, &new);

		/* Set the hash table entry with value = TV_XXX integer value */
		Tcl_SetHashValue(hPtr, g_tval[i].key);
	}
}

/*
 * Convert a TV_XXX string into the corresponding integer value.
 * Return an error if the tval is unknown.
 */
int angtk_tval_const(int *tval, cptr str)
{
	Tcl_HashEntry *hPtr;

	if ((hPtr = Tcl_FindHashEntry(g_tval_str, str)) == NULL)
	{
		/* Get the interpreter result object */
		Tcl_Obj *resultPtr = Tcl_GetObjResult(g_interp);
		
		/* Append an error string */
		Tcl_AppendStringsToObj(resultPtr, "unknown tval \"", str,
			"\"", NULL);

		/* Failure */
		return TCL_ERROR;
	}

	/* Return the integer value of the TV_XXX constant */
	(*tval) = (int) Tcl_GetHashValue(hPtr);

	/* Success */
	return TCL_OK;
}

/*
 * Convert a TV_XXX constant into the corresponding string.
 * Return an error if the tval is unknown.
 */
int angtk_tval_string(cptr *str, int tval)
{
	Tcl_HashEntry *hPtr;

	/* Look up the hash table entry using the tval as a key */
	if ((hPtr = Tcl_FindHashEntry(g_tval_const, (char *) tval)) == NULL)
	{
		char buf[80];
		
		/* Get the interpreter result object */
		Tcl_Obj *resultPtr = Tcl_GetObjResult(g_interp);

		/* Format the error */
		(void) sprintf(buf, "unknown tval \"%d\"", tval);
		
		/* Append an error string */
		Tcl_AppendStringsToObj(resultPtr, buf, NULL);

		/* Failure */
		return TCL_ERROR;
	}

	/* Return the textual name for this TV_XXX constant */
	(*str) = (cptr) Tcl_GetHashValue(hPtr);

	/* Success */
	return TCL_OK;
}

/*
 * Return a Tcl list of m_list[] indexes of pets
 */
static Tcl_Obj *DumpPets(void)
{
	int i;
	Tcl_Obj *listObjPtr;

	/* Create a new Tcl list object */
	listObjPtr = Tcl_NewListObj(0, NULL);

	/* Process the monsters */
	for (i = 1; i < m_max; i++)
	{
		/* Access the monster */
		monster_type *m_ptr = &m_list[i];

		/* Ignore "dead" monsters */
		if (!m_ptr->r_idx) continue;

		/* Append m_list[] index of friendly monster */
		if (is_pet(m_ptr))
		{
			Tcl_ListObjAppendElement(g_interp, listObjPtr,
				Tcl_NewIntObj(i));
		}
	}

	return listObjPtr;
}

static int s_status_value;

/*
 * Prints status of hunger
 */
static cptr state_hunger(void)
{
	/* Fainting / Starving */
	if (p_ptr->food < PY_FOOD_FAINT)
	{
		return "Weak";
	}

	/* Weak */
	else if (p_ptr->food < PY_FOOD_WEAK)
	{
		return "Weak";
	}

	/* Hungry */
	else if (p_ptr->food < PY_FOOD_ALERT)
	{
		return "Hungry";
	}

	/* Normal */
	else if (p_ptr->food < PY_FOOD_FULL)
	{
		return "";
	}

	/* Full */
	else if (p_ptr->food < PY_FOOD_MAX)
	{
		return "Full";
	}

	/* Gorged */
	else
	{
		return "Gorged";
	}
}

/*
 * Prints Blind status
 */
static cptr state_blind(void)
{
	if (p_ptr->blind)
	{
		return "Blind";
	}
	else
	{
		return "";
	}
}

/*
 * Prints Confusion status
 */
static cptr state_confused(void)
{
	if (p_ptr->confused)
	{
		return "Confused";
	}
	else
	{
		return "";
	}
}

/*
 * Prints Fear status
 */
static cptr state_afraid(void)
{
	if (p_ptr->afraid)
	{
		return "Afraid";
	}
	else
	{
		return "";
	}
}

/*
 * Prints Poisoned status
 */
static cptr state_poisoned(void)
{
	if (p_ptr->poisoned)
	{
		return "Poisoned";
	}
	else
	{
		return "";
	}
}

static int trunc_num(int n)
{
	/* Only 4 digits are allowed */
	if (n > 9999) n = 9999;
	
	/* Extensive */
	if (n >= 1000)
	{
		return (n / 100) * 100;
	}

	/* Long */
	else if (n >= 100)
	{
		return (n / 10) * 10;
	}

	/* Medium */
	else if (n >= 10)
	{
		return (n / 5) * 5;
	}

	/* Short */
	return n;
}

/*
 * Prints Searching, Resting, Paralysis, or 'count' status
 * Display is always exactly 10 characters wide (see below)
 *
 * This function was a major bottleneck when resting, so a lot of
 * the text formatting code was optimized in place below.
 */
static cptr state_state(void)
{
	/* Paralysis */
	if (p_ptr->paralyzed)
	{
		return "Paralyzed!";
	}

	/* Resting */
	else if (p_ptr->resting)
	{
		int n = p_ptr->resting;

		/* Rest until healed */
		if (n == -1)
		{
			return "Rest *****";
		}

		/* Rest until done */
		else if (n == -2)
		{
			return "Rest &&&&&";
		}
		else
		{
			s_status_value = trunc_num(n);
			return "Rest %d";
		}
	}

	/* Repeating */
	else if (p_ptr->command_rep)
	{
		int n = p_ptr->command_rep;

		s_status_value = trunc_num(n);

		if (n > 999)
		{
			return "Rep. %d";
		}
		else
		{
			return "Repeat %d";
		}
	}

	/* Searching */
	else if (p_ptr->searching)
	{
		return "Searching";
	}

	/* Nothing interesting */
	else
	{
		return "";
	}
}

/*
 * Prints the speed of a character.			-CJS-
 */
static cptr state_speed(void)
{
	int n = p_ptr->pspeed;

	/* Hack -- Visually "undo" the Search Mode Slowdown */
	if (p_ptr->searching) n += 10;

	/* Fast */
	if (n > 110)
	{
		s_status_value = n - 110;
		return "Fast (%+d)";
	}

	/* Slow */
	else if (n < 110)
	{
		s_status_value = -(110 - n);
		return "Slow (%+d)";
	}

	/* Normal */
	return "";
}

static cptr state_study(void)
{
	if (p_ptr->new_spells)
	{
		return "Study";
	}
	else
	{
		return "";
	}
}

static cptr state_cut(void)
{
	int c = p_ptr->cut;

	if (c > 1000)
	{
		return "Mortal wound";
	}
	else if (c > 200)
	{
		return "Deep gash";
	}
	else if (c > 100)
	{
		return "Severe cut";
	}
	else if (c > 50)
	{
		return "Nasty cut";
	}
	else if (c > 25)
	{
		return "Bad cut";
	}
	else if (c > 10)
	{
		return "Light cut";
	}
	else if (c)
	{
		return "Graze";
	}
	else
	{
		return "";
	}
}

static cptr state_stun(void)
{
	int s = p_ptr->stun;

	if (s > 100)
	{
		return "Knocked out";
	}
	else if (s > 50)
	{
		return "Heavy stun";
	}
	else if (s)
	{
		return "Stun";
	}
	else
	{
		return "";
	}
}

static cptr state_winner(void)
{
	/* Wizard */
	if (p_ptr->wizard)
	{
		return "Wizard";
	}

	/* Winner */
	else if (p_ptr->total_winner || (p_ptr->lev > PY_MAX_LEVEL))
	{
		return "Winner";
	}

	/* Normal */
	else
	{
		return "";
	}
}

cptr player_status(int status, int *value)
{
	cptr format;

	typedef cptr (status_proc)(void);
	static status_proc *status_info[] = {
		state_cut,
		state_stun,
		state_hunger,
		state_blind,
		state_confused,
		state_afraid,
		state_poisoned,
		state_state,
		state_speed,
		state_study,
		state_winner
	};

	s_status_value = 0;

	format = (*status_info[status])();
	(*value) = s_status_value;
	return format;
}

void blows_per_round(int *_blows, int *_muta_att)
{
	int muta_att = 0;

	if (p_ptr->muta2 & MUT2_HORNS)
		muta_att++;
	if (p_ptr->muta2 & MUT2_SCOR_TAIL)
		muta_att++;
	if (p_ptr->muta2 & MUT2_BEAK)
		muta_att++;
	if (p_ptr->muta2 & MUT2_TRUNK)
		muta_att++;
	if (p_ptr->muta2 & MUT2_TENTACLES)
		muta_att++;

	(*_blows) = p_ptr->num_blow;
	(*_muta_att) = muta_att;
}

void shots_per_round(int *_shots, int *_shots_frac)
{
	int energy_fire = 100;
	int shots, shots_frac;
	object_type *o_ptr = &p_ptr->equipment[EQUIP_BOW];

	if (o_ptr->k_idx)
	{
		switch (o_ptr->sval)
		{
			case SV_SLING:
			{
				energy_fire = 50;
				break;
			}
			case SV_SHORT_BOW:
			{
				energy_fire = 100;
				break;
			}
			case SV_LONG_BOW:
			{
				energy_fire = 100;
				break;
			}
			case SV_LIGHT_XBOW:
			{
				energy_fire = 120;
				break;
			}
			case SV_HEAVY_XBOW:
			{
				if (p_ptr->stat_use[A_DEX] >= 16)
				{
					energy_fire = 150;
				}
				else
				{
					/* players with low dex will take longer to load */
					energy_fire = 200;
				}
			}
			break;
		}
	}

	shots = p_ptr->num_fire * 100;
	shots_frac = (shots * 100 / energy_fire) % 100;
	shots = shots / energy_fire;

	(*_shots) = shots;
	(*_shots_frac) = shots_frac;
}

/*
 *--------------------------------------------------------------
 *
 * objcmd_player --
 *
 *	Implements the "player" script command.
 *
 *--------------------------------------------------------------
 */

int
objcmd_player(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"ability", "age", "armor_class",
		"blows_per_round", "icon", "died_from",
		"exp", "food", "gold", "height", "history", "hitpoints",
		"infravision", "level", "mana", "max_depth", "name", "position",
		"sex", "shots_per_round", "social_class", "spell_book",
		"status", "title", "to_dam", "to_hit", "weight",
		"total_weight", "preserve", "base_name",
		"is_dead", "turn", "max_level", "disturb", "new_spells",
		"command_rep", "running", "prayer_or_spell", "health_who",
		"monster_race_idx", "life_rating",
		"pets", "realm1", "realm2", "patron",
		NULL};
	enum {IDX_ABILITY, IDX_AGE, IDX_ARMOR_CLASS,
		IDX_BLOWS_PER_ROUND, IDX_ICON, IDX_DIED_FROM,
		IDX_EXP, IDX_FOOD, IDX_GOLD, IDX_HEIGHT, IDX_HISTORY, IDX_HITPOINTS,
		IDX_INFRAVISION, IDX_LEVEL, IDX_MANA, IDX_MAX_DEPTH, IDX_NAME, IDX_POSITION,
		IDX_SEX, IDX_SHOTS_PER_ROUND, IDX_SOCIAL_CLASS, IDX_SPELL_BOOK,
		IDX_STATUS, IDX_TITLE, IDX_TO_DAM, IDX_TO_HIT, IDX_WEIGHT,
		IDX_TOTAL_WEIGHT, IDX_PRESERVE, IDX_BASE_NAME,
		IDX_IS_DEAD, IDX_TURN, IDX_MAX_LEVEL, IDX_DISTURB, IDX_NEW_SPELLS,
		IDX_COMMAND_REP, IDX_RUNNING, IDX_PRAYER_OR_SPELL, IDX_HEALTH_WHO,
		IDX_MONSTER_RACE_IDX, IDX_LIFE_RATING,
		IDX_PETS, IDX_REALM1, IDX_REALM2, IDX_PATRON
		} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	cptr cstr;
	object_type *o_ptr;
	int i, tmp;
	long expadv;
	double pct;
	char buf[512];
	cptr t, p;
	Tcl_Obj *listObjPtr;

	static cptr abilityOptions[] = {"fighting", "bows_throw", "saving_throw",
		"stealth", "perception", "searching", "disarming", "magic_device",
		NULL};

	static struct {int rating; int max;} ability[] = {
		{0, 10}, /* fighting */
		{0, 10}, /* bows_throw */
		{0, 6}, /* saving_throw */
		{0, 1}, /* stealth */
		{0, 6}, /* perception */
		{0, 6}, /* searching */
		{0, 8}, /* disarming */
		{0, 6} /* magic_device */
	};

	/* Required number of arguments */
    if (objC < 2)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
    }

	/* Get requested option */
    if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (option)
	{
		case IDX_ABILITY: /* ability */

		    if (objC != 3)
		    {
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "ability");
				return TCL_ERROR;
		    }

		    if (Tcl_GetIndexFromObj(interp, objV[2], (char **) abilityOptions, (char *) "ability", 0, 
				&index) != TCL_OK)
			{
				return TCL_ERROR;
		    }
	
			/* Fighting Skill (with current weapon) */
			o_ptr = &p_ptr->equipment[EQUIP_WIELD];
			tmp = p_ptr->to_h + o_ptr->to_h;
			ability[0].rating = p_ptr->skill_thn + (tmp * BTH_PLUS_ADJ);
	
			/* Shooting Skill (with current bow and normal missile) */
			o_ptr = &p_ptr->equipment[EQUIP_BOW];
			tmp = p_ptr->to_h + o_ptr->to_h;
			ability[1].rating = p_ptr->skill_thb + (tmp * BTH_PLUS_ADJ);
	
			ability[2].rating = p_ptr->skill_sav;
			ability[3].rating = p_ptr->skill_stl;
			ability[4].rating = p_ptr->skill_fos;
			ability[5].rating = p_ptr->skill_sns;
			ability[6].rating = p_ptr->skill_dis;
			ability[7].rating = p_ptr->skill_dev;
			 
			Tcl_SetStringObj(resultPtr, format("%d %d",
				ability[index].rating, ability[index].max), -1);
			break;

		case IDX_AGE: /* age */
			Tcl_SetIntObj(resultPtr, p_ptr->age);
			break;

		case IDX_ARMOR_CLASS: /* armor_class */
			Tcl_SetStringObj(resultPtr,
				format("%d %d", p_ptr->dis_ac, p_ptr->dis_to_a), -1);
			break;

		case IDX_BLOWS_PER_ROUND: /* blows_per_round */
		{
			int blows, muta_att;
			blows_per_round(&blows, &muta_att);
			Tcl_SetStringObj(resultPtr,
				format(muta_att ? "%d+%d" : "%d", blows, muta_att), -1);
			break;
		}

		case IDX_ICON: /* icon */
		{
			char buf[128];
			
			index = 0;
			Tcl_SetResult(interp, assign_print2(buf, ASSIGN_CHARACTER, index),
				TCL_VOLATILE);
			break;
		}

		case IDX_DIED_FROM: /* died_from */
			if (!p_ptr->is_dead)
			{
				Tcl_SetStringObj(resultPtr, (char *) "character is not dead", -1);
				return TCL_ERROR;
			}
			ExtToUtf_SetResult(interp, p_ptr->died_from);
			break;

		case IDX_EXP: /* exp */
			if (p_ptr->lev >= PY_MAX_LEVEL) expadv = 999999999;
			else expadv = (s32b)(player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100L);
			Tcl_SetStringObj(resultPtr, format("%ld %ld %ld", p_ptr->exp,
				p_ptr->max_exp, expadv), -1);
			break;

		case IDX_FOOD: /* food */
			tmp = MIN(p_ptr->food,PY_FOOD_MAX);
			Tcl_SetStringObj(resultPtr,
				format("%d %d %f", p_ptr->food, PY_FOOD_MAX,
				(double) tmp / PY_FOOD_MAX), -1);
			break;

		case IDX_GOLD: /* gold */
			Tcl_SetStringObj(resultPtr, format("%ld", p_ptr->au), -1);
			break;

		case IDX_HEIGHT: /* height */
			Tcl_SetIntObj(resultPtr, p_ptr->ht);
			break;

		case IDX_HISTORY: /* history */
			buf[0] = '\0';
			for (i = 0; i < 4; i++)
			{
				(void) strcat(buf, format("%s\n", p_ptr->history[i]));
			}
			i = strlen(buf) - 1;
			while (buf[i] == '\n') buf[i--] = '\0';
			ExtToUtf_SetResult(interp, buf);
			break;

		case IDX_HITPOINTS: /* hitpoints */
			Tcl_SetStringObj(resultPtr,
				format("%d %d %f", p_ptr->chp, p_ptr->mhp,
				(double) p_ptr->chp / p_ptr->mhp), -1);
			break;

		case IDX_INFRAVISION: /* infravision */
			Tcl_SetIntObj(resultPtr, p_ptr->see_infra * 10);
			break;

		case IDX_LEVEL: /* level */
			Tcl_SetIntObj(resultPtr, p_ptr->lev);
			break;

		case IDX_MANA: /* mana */
			if (p_ptr->msp) pct = (double) p_ptr->csp / p_ptr->msp;
			else pct = 0;
			Tcl_SetStringObj(resultPtr,
				format("%d %d %f", p_ptr->csp, p_ptr->msp, pct), -1);
			break;

	    case IDX_MAX_DEPTH: /* max_depth */
			Tcl_SetIntObj(resultPtr, p_ptr->max_depth);
			break; 

 		case IDX_NAME: /* name */
			if (objC == 3)
			{
				t = Tcl_GetStringFromObj(objV[2], NULL);
				for (p = t; *p; p++)
					if (iscntrl((unsigned char) p[0])) break;
				if (*p || (p - t > 15))
				{
					Tcl_AppendStringsToObj(resultPtr,
						"invalid character name \"", t, "\"", NULL);
					return TCL_ERROR;
				}
				(void) strcpy(player_name, t);
				process_player_name(FALSE);
				Bind_Generic(EVENT_PY, KEYWORD_PY_NAME + 1);
			}
			ExtToUtf_SetResult(interp, (char *) player_name);
			break;

		case IDX_POSITION: /* position */
			Tcl_SetStringObj(resultPtr, format("%d %d", p_ptr->py, p_ptr->px),
				-1);
			break;

		case IDX_SEX: /* sex */
			Tcl_SetStringObj(resultPtr, (char *) sp_ptr->title, -1);
			break;

		case IDX_SHOTS_PER_ROUND: /* shots_per_round */
		{
			int shots, shots_frac;
			shots_per_round(&shots, &shots_frac);
			Tcl_SetStringObj(resultPtr,
				format("%d.%d", shots, shots_frac), -1);
			break;
		}

		case IDX_SOCIAL_CLASS: /* social_class */
			Tcl_SetIntObj(resultPtr, p_ptr->sc);
			break;

		case IDX_SPELL_BOOK: /* spell_book */
			listObjPtr = Tcl_NewListObj(0, NULL);
			if (p_ptr->realm1)
			{
				(void) angtk_tval_string(&cstr, p_ptr->realm1 + TV_LIFE_BOOK - 1);
				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewStringObj((char *) cstr, -1));
			}
			if (p_ptr->realm2)
			{
				(void) angtk_tval_string(&cstr, p_ptr->realm2 + TV_LIFE_BOOK - 1);
				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewStringObj((char *) cstr, -1));
			}
			Tcl_SetObjResult(interp, listObjPtr);
			break;

		case IDX_STATUS: /* status */
		    if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "status");
				return TCL_ERROR;
		    }
		    if (Tcl_GetIndexFromObj(interp, objV[2], (char **) keyword_status,
				(char *) "status", 0, &index) != TCL_OK)
			{
				return TCL_ERROR;
			}
			t = player_status(index, &i);
			Tcl_SetStringObj(resultPtr, format("{%s} %d", t, i), -1);
			break;

		case IDX_TITLE: /* title */
			ExtToUtf_SetResult(interp,
				(char *) player_title[p_ptr->pclass][(p_ptr->lev-1)/5]);
			break;

		case IDX_TO_DAM: /* to_dam */
			Tcl_SetIntObj(resultPtr, p_ptr->dis_to_d);
			break;

		case IDX_TO_HIT: /* to_hit */
			Tcl_SetIntObj(resultPtr, p_ptr->dis_to_h);
			break;

		case IDX_WEIGHT: /* weight */
			Tcl_SetIntObj(resultPtr, p_ptr->wt);
			break;

		case IDX_TOTAL_WEIGHT: /* total_weight */
			Tcl_SetIntObj(resultPtr, p_ptr->total_weight);
			break;

		case IDX_PRESERVE: /* preserve */
			Tcl_SetIntObj(resultPtr, preserve_mode);
			break;

		case IDX_BASE_NAME: /* base_name */
			ExtToUtf_SetResult(interp, (char *) player_base);
			break;

		case IDX_IS_DEAD: /* is_dead */
			Tcl_SetBooleanObj(resultPtr, p_ptr->is_dead);
			break;

		case IDX_TURN: /* turn */
			Tcl_SetLongObj(resultPtr, turn);
			break;

		case IDX_MAX_LEVEL: /* max_level */
			Tcl_SetIntObj(resultPtr, p_ptr->max_lev);
			break;

		case IDX_DISTURB: /* disturb */
			/* When is this allowed? */
			if (inkey_flags == 0)
			{
				disturb(FALSE);
			}
			break;

		case IDX_NEW_SPELLS: /* new_spells */
			if (!p_ptr->realm1)
			{
				Tcl_SetStringObj(resultPtr, (char *) "character cannot read books", -1);
				return TCL_ERROR;
			}
			Tcl_SetIntObj(resultPtr, p_ptr->new_spells);
			break;

		case IDX_COMMAND_REP: /* command_rep */
			Tcl_SetIntObj(resultPtr, p_ptr->command_rep);
			break;

		case IDX_RUNNING: /* running */
			Tcl_SetIntObj(resultPtr, p_ptr->running);
			break;

		case IDX_PRAYER_OR_SPELL: /* prayer_or_spell */
			if (!p_ptr->realm1)
			{
				Tcl_SetStringObj(resultPtr, (char *) "character cannot read books", -1);
				return TCL_ERROR;
			}
			switch (mp_ptr->spell_book)
			{
				case TV_LIFE_BOOK: t = "prayer"; break;
				default: t = "spell"; break;
			}
			if (t == NULL)
			{
				quit_fmt("unhandled mp_ptr->spell_book %d",
					mp_ptr->spell_book);
			}
			Tcl_SetStringObj(resultPtr, (char *) t, -1);
			break;

		case IDX_HEALTH_WHO: /* health_who */
			/*
			 * Should I call health_track() to set PW_HEALTH?
			 * Should I call handle_stuff() to update the display?
			 */
			if (objC == 3)
			{
				int m_idx;
				if (Tcl_GetIntFromObj(interp, objV[2], &m_idx) != TCL_OK)
				{
					return TCL_ERROR;
				}
				if ((m_idx < 0) || (m_idx >= m_max))
				{
					Tcl_SetStringObj(resultPtr,
						format("bad m_list index \"%d\": must be between 0 and %d",
						m_idx, (int) m_max - 1), -1);
				}
				p_ptr->health_who = m_idx;
				break;
			}
			Tcl_SetIntObj(resultPtr, p_ptr->health_who);
			break;

		case IDX_MONSTER_RACE_IDX: /* monster_race_idx */
			/*
			 * Should I call monster_race_track() to set PW_MONSTER?
			 * Should I call handle_stuff() to update the display?
			 */
			if (objC == 3)
			{
				int r_idx;
				if (Tcl_GetIntFromObj(interp, objV[2], &r_idx) != TCL_OK)
				{
					return TCL_ERROR;
				}
				if (!((r_idx >= 0) && (r_idx < z_info->r_max)))
				{
					Tcl_SetStringObj(resultPtr,
						format("bad r_info index \"%d\": must be between 0 and %d",
						r_idx, (int) z_info->r_max - 1), -1);
					return TCL_ERROR;
				}
				p_ptr->monster_race_idx = r_idx;
				break;
			}
			Tcl_SetIntObj(resultPtr, p_ptr->monster_race_idx);
			break;

		case IDX_LIFE_RATING: /* life_rating */
			i = (int) (((long) p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) / 
				(2 * p_ptr->hitdie + ((PY_MAX_LEVEL - 1) *
				(p_ptr->hitdie + 1))));
			Tcl_SetIntObj(resultPtr, i);
			break;
			
		case IDX_PETS: /* pets */
			Tcl_SetObjResult(interp, DumpPets());
			break;

		case IDX_REALM1: /* realm1 */
			Tcl_SetStringObj(resultPtr,
				(char *) realm_names[p_ptr->realm1], -1);
			break;

		case IDX_REALM2: /* realm2 */
			Tcl_SetStringObj(resultPtr,
				(char *) realm_names[p_ptr->realm2], -1);
			break;

		case IDX_PATRON: /* patron */
			ExtToUtf_SetResult(interp,
				(char *) chaos_patrons[p_ptr->chaos_patron]);
			break;

	}

	return TCL_OK;
}


/*
 * An array of human-readable descriptions of each power, indexed
 * by POWER_XXX constants. These are similar to spell names.
 */
cptr power_desc[] = {

/* Racial power */
"Detect Doors & Traps",
"Create Food",
"Teleport",
"Remove Fear",
"Berserk",
"Shadow Shifting",
"Pattern Mindwalking",
"Berserk",
"Explosive Rune",
"Stone To Mud",
"Probing",
"Throw Boulder",
"Scare Monster",
"Spit Acid",
"Poison Dart",
"Detect Doors & Traps",
"Magic Missile",
"Breath Weapon",
"Mind Blast",
"Fire Bolt/Ball",
"Stone Skin",
"Restore Life",
"Restore Life",
"Drain Life",
"Scare Monster",
"Sleeping Dust",

/* Mutation power */
"Spit Acid",
"Fire Breath",
"Hypnotic Gaze",
"Telekinesis",
"Teleport",
"Mind Blast",
"Emit Radiation",
"Vampiric Drain",
"Smell Metal",
"Smell Monsters",
"Blink",
"Eat Rock",
"Swap Position",
"Shriek",
"Illuminate",
"Detect Curse",
"Berserk",
"Polymorph",
"Midas Touch",
"Grow Mold",
"Resist Elements",
"Earthquake",
"Eat Magic",
"Weigh Magic",
"Sterilize",
"Panic Hit",
"Dazzle",
"Laser Eye",
"Recall",
"Banish Evil",
"Cold Touch",
"Throw Object"
};

/* Constants for racial and mutation powers */
enum {

/* Racial power */
POWER_DWARF,
POWER_HOBBIT,
POWER_GNOME,
POWER_HALF_ORC,
POWER_HALF_TROLL,
POWER_AMBERITE_A,
POWER_AMBERITE_B,
POWER_BARBARIAN,
POWER_HALF_OGRE,
POWER_HALF_GIANT,
POWER_HALF_TITAN,
POWER_CYCLOPS,
POWER_YEEK,
POWER_KLACKON,
POWER_KOBOLD,
POWER_NIBELUNG,
POWER_DARK_ELF,
POWER_DRACONIAN,
POWER_MIND_FLAYER,
POWER_IMP,
POWER_GOLEM,
POWER_SKELETON,
POWER_ZOMBIE,
POWER_VAMPIRE,
POWER_SPECTRE,
POWER_SPRITE,

/* Mutation power */
POWER_SPIT_ACID,
POWER_BR_FIRE,
POWER_HYPN_GAZE,
POWER_TELEKINES,
POWER_VTELEPORT,
POWER_MIND_BLST,
POWER_RADIATION,
POWER_VAMPIRISM,
POWER_SMELL_MET,
POWER_SMELL_MON,
POWER_BLINK,
POWER_EAT_ROCK,
POWER_SWAP_POS,
POWER_SHRIEK,
POWER_ILLUMINE,
POWER_DET_CURSE,
POWER_BERSERK,
POWER_POLYMORPH,
POWER_MIDAS_TCH,
POWER_GROW_MOLD,
POWER_RESIST,
POWER_EARTHQUAKE,
POWER_EAT_MAGIC,
POWER_WEIGH_MAG,
POWER_STERILITY,
POWER_PANIC_HIT,
POWER_DAZZLE,
POWER_LASER_EYE,
POWER_RECALL,
POWER_BANISH,
POWER_COLD_TOUCH,
POWER_LAUNCHER,

/* Max number of power's */
MAX_POWER
};


/*
 * Get a list of POWER_XXX indexes of character powers.
 */
static int get_powers(int *power)
{
	int num = 0;

	/* Racial power */
	switch (p_ptr->prace)
	{
		case RACE_DWARF:
			power[num++] = POWER_DWARF;
			break;

		case RACE_HOBBIT:
			power[num++] = POWER_HOBBIT;
			break;

		case RACE_GNOME:
			power[num++] = POWER_GNOME;
			break;

		case RACE_HALF_ORC:
			power[num++] = POWER_HALF_ORC;
			break;

		case RACE_HALF_TROLL:
			power[num++] = POWER_HALF_TROLL;
			break;

		case RACE_AMBERITE:
			power[num++] = POWER_AMBERITE_A;
			power[num++] = POWER_AMBERITE_B;
			break;

		case RACE_BARBARIAN:
			power[num++] = POWER_BARBARIAN;
			break;

		case RACE_HALF_OGRE:
			power[num++] = POWER_HALF_OGRE;
			break;

		case RACE_HALF_GIANT:
			power[num++] = POWER_HALF_GIANT;
			break;

		case RACE_HALF_TITAN:
			power[num++] = POWER_HALF_TITAN;
			break;

		case RACE_CYCLOPS:
			power[num++] = POWER_CYCLOPS;
			break;

		case RACE_YEEK:
			power[num++] = POWER_YEEK;
			break;

		case RACE_KLACKON:
			power[num++] = POWER_KLACKON;
			break;

		case RACE_KOBOLD:
			power[num++] = POWER_KOBOLD;
			break;

		case RACE_NIBELUNG:
			power[num++] = POWER_NIBELUNG;
			break;

		case RACE_DARK_ELF:
			power[num++] = POWER_DARK_ELF;
			break;

		case RACE_DRACONIAN:
			power[num++] = POWER_DRACONIAN;
			break;

		case RACE_MIND_FLAYER:
			power[num++] = POWER_MIND_FLAYER;
			break;

		case RACE_IMP:
			power[num++] = POWER_IMP;
			break;

		case RACE_GOLEM:
			power[num++] = POWER_GOLEM;
			break;

		case RACE_SKELETON:
			power[num++] = POWER_SKELETON;
			break;

		case RACE_ZOMBIE:
			power[num++] = POWER_ZOMBIE;
			break;

		case RACE_VAMPIRE:
			power[num++] = POWER_VAMPIRE;
			break;

		case RACE_SPECTRE:
			power[num++] = POWER_SPECTRE;
			break;

		case RACE_SPRITE:
			power[num++] = POWER_SPRITE;
			break;
	}

	/* Mutation power */
	if (p_ptr->muta1 & MUT1_SPIT_ACID)
	{
		power[num++] = POWER_SPIT_ACID;
	}
	if (p_ptr->muta1 & MUT1_BR_FIRE)
	{
		power[num++] = POWER_BR_FIRE;
	}
	if (p_ptr->muta1 & MUT1_HYPN_GAZE)
	{
		power[num++] = POWER_HYPN_GAZE;
	}
	if (p_ptr->muta1 & MUT1_TELEKINES)
	{
		power[num++] = POWER_TELEKINES;
	}
	if (p_ptr->muta1 & MUT1_VTELEPORT)
	{
		power[num++] = POWER_VTELEPORT;
	}
	if (p_ptr->muta1 & MUT1_MIND_BLST)
	{
		power[num++] = POWER_MIND_BLST;
	}
	if (p_ptr->muta1 & MUT1_RADIATION)
	{
		power[num++] = POWER_RADIATION;
	}
	if (p_ptr->muta1 & MUT1_VAMPIRISM)
	{
		power[num++] = POWER_VAMPIRISM;
	}
	if (p_ptr->muta1 & MUT1_SMELL_MET)
	{
		power[num++] = POWER_SMELL_MET;
	}
	if (p_ptr->muta1 & MUT1_SMELL_MON)
	{
		power[num++] = POWER_SMELL_MON;
	}
	if (p_ptr->muta1 & MUT1_BLINK)
	{
		power[num++] = POWER_BLINK;
	}
	if (p_ptr->muta1 & MUT1_EAT_ROCK)
	{
		power[num++] = POWER_EAT_ROCK;
	}
	if (p_ptr->muta1 & MUT1_SWAP_POS)
	{
		power[num++] = POWER_SWAP_POS;
	}
	if (p_ptr->muta1 & MUT1_SHRIEK)
	{
		power[num++] = POWER_SHRIEK;
	}
	if (p_ptr->muta1 & MUT1_ILLUMINE)
	{
		power[num++] = POWER_ILLUMINE;
	}
	if (p_ptr->muta1 & MUT1_DET_CURSE)
	{
		power[num++] = POWER_DET_CURSE;
	}
	if (p_ptr->muta1 & MUT1_BERSERK)
	{
		power[num++] = POWER_BERSERK;
	}
	if (p_ptr->muta1 & MUT1_POLYMORPH)
	{
		power[num++] = POWER_POLYMORPH;
	}
	if (p_ptr->muta1 & MUT1_MIDAS_TCH)
	{
		power[num++] = POWER_MIDAS_TCH;
	}
	if (p_ptr->muta1 & MUT1_GROW_MOLD)
	{
		power[num++] = POWER_GROW_MOLD;
	}
	if (p_ptr->muta1 & MUT1_RESIST)
	{
		power[num++] = POWER_RESIST;
	}
	if (p_ptr->muta1 & MUT1_EARTHQUAKE)
	{
		power[num++] = POWER_EARTHQUAKE;
	}
	if (p_ptr->muta1 & MUT1_EAT_MAGIC)
	{
		power[num++] = POWER_EAT_MAGIC;
	}
	if (p_ptr->muta1 & MUT1_WEIGH_MAG)
	{
		power[num++] = POWER_WEIGH_MAG;
	}
	if (p_ptr->muta1 & MUT1_STERILITY)
	{
		power[num++] = POWER_STERILITY;
	}
	if (p_ptr->muta1 & MUT1_PANIC_HIT)
	{
		power[num++] = POWER_PANIC_HIT;
	}
	if (p_ptr->muta1 & MUT1_DAZZLE)
	{
		power[num++] = POWER_DAZZLE;
	}
	if (p_ptr->muta1 & MUT1_LASER_EYE)
	{
		power[num++] = POWER_LASER_EYE;
	}
	if (p_ptr->muta1 & MUT1_RECALL)
	{
		power[num++] = POWER_RECALL;
	}
	if (p_ptr->muta1 & MUT1_BANISH)
	{
		power[num++] = POWER_BANISH;
	}
	if (p_ptr->muta1 & MUT1_COLD_TOUCH)
	{
		power[num++] = POWER_COLD_TOUCH;
	}
	if (p_ptr->muta1 & MUT1_LAUNCHER)
	{
		power[num++] = POWER_LAUNCHER;
	}

	/* Return number of powers */
	return num;
}

/*
 *--------------------------------------------------------------
 *
 * objcmd_power --
 *
 *	Implements the "power" script command.
 * 	Syntax:
 *		power get
 * 			Return list of racial/mutation power names
 *
 *		power info $power $arrayName
 * 			Set array with info about a power:
 * 				$arrayName(chance) - how hard to use
 * 				$arrayName(cost) - mana cost per use
 *				$arrayName(name) - name of this power
 * 				$arrayName(stat) - stat used
 * 				$arrayName(level) - min level to use
 * 				$arrayName(type) "racial"/"mutation"
 *
 *--------------------------------------------------------------
 */

int
objcmd_power(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"get", NULL};
	enum {IDX_GET} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	Tcl_Obj *listObjPtr;
	int i, num, power[MAX_POWER];

	/* Required number of arguments */
    if (objC < 2)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
    }

	/* Get requested option */
    if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	if (!character_generated)
	{
		Tcl_AppendStringsToObj(resultPtr,
			(char *) "character has not been generated yet", NULL);
		return TCL_ERROR;
	}

	switch (option)
	{
		case IDX_GET: /* get */
			num = get_powers(power);
			if (num)
			{
				listObjPtr = Tcl_NewListObj(0, NULL);
				for (i = 0; i < num; i++)
				{
					Tcl_ListObjAppendElement(interp, listObjPtr,
						Tcl_NewIntObj(power[i]));
				}
				Tcl_SetObjResult(interp, listObjPtr);
			}
			break;
	}

	return TCL_OK;
}

/*
 * Return an error if the given object doesn't contain a valid
 * spell book k_info[] index for the character.
 */
static int GetBookFromObj(Tcl_Interp *interp, Tcl_Obj *objPtr, int *bookNum)
{
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int k_idx, tval;
	cptr cstr;
	
    /* Get the book number */
	if (Tcl_GetIntFromObj(interp, objPtr, &k_idx) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Verify the book number */
	if ((k_idx <= 0) || (k_idx >= z_info->k_max))
	{
		/* Set the error */
		Tcl_SetStringObj(resultPtr,
			format("invalid object index \"%d\"", k_idx), -1);

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the tval */
	tval = k_info[k_idx].tval;

	/* Verify the tval */
	if ((tval < TV_LIFE_BOOK) ||
		(tval > TV_ARCANE_BOOK))
	{
		/* Set the error */
		Tcl_SetStringObj(resultPtr,
			format("object \"%d\" is not a book", k_idx), -1);

		/* Failure */
		return TCL_ERROR;
	}

	/* Verify the tval */
	if ((tval != p_ptr->realm1 + (TV_LIFE_BOOK - 1)) &&
		(tval != p_ptr->realm2 + (TV_LIFE_BOOK - 1)))
	{
		/* Get the string for the tval */
		(void) angtk_tval_string(&cstr, tval);

		/* Set the error */
		Tcl_SetStringObj(resultPtr,
			format("character can't read \"%s\" books", cstr), -1);

		/* Failure */
		return TCL_ERROR;
	}

	/* Return the book number */
	(*bookNum) = k_idx;
	
	/* Success */
	return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * objcmd_spell --
 *
 *	Implements the "spell" script command.
 * 	Syntax:
 *		spell find $k_idx ?-tester $boolean? ?-known $boolean?
 *		spell info $k_idx $spell $arrayName
 *			arrayName(char)
 *			arrayName(name)
 *			arrayName(level)
 *			arrayName(mana)
 *			arrayName(chance)
 *			arrayName(known)
 *			arrayName(info)
 *
 *--------------------------------------------------------------
 */

int
objcmd_spell(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"find", "info", "memory", NULL};
	enum {IDX_FIND, IDX_INFO, IDX_MEMORY} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;
	
	Tcl_Obj *listObjPtr;
    int k_idx, realm, sval, tval, spell, i = -1, first;
	char *t;
	u32b forgotten, learned, worked;

	/* Default to ignoring known */
	int request_known = 0, match_known = 0;

	/* Default to ignoring spell_okay() */
	int request_tester = 0, match_tester = 0;

	/* Required number of arguments */
	if (objC < 2)
	{
		/* Set the error */
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?arg ...?");

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the requested option */
    if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
    {
		return TCL_ERROR;
    }

	/* The character doesn't exist yet */
	if (!character_generated)
	{
		/* Set the error */
		Tcl_AppendStringsToObj(resultPtr,
			"character has not been generated yet", NULL);

		/* Failure */
		return TCL_ERROR;
	}

	/* The character is not a magic user */
	if (!p_ptr->realm1 && !p_ptr->realm2)
	{
		/* Set the error */
		Tcl_AppendStringsToObj(resultPtr,
			"character cannot read books", NULL);

		/* Failure */
		return TCL_ERROR;
	}

	switch (option)
	{
		case IDX_FIND: /* find */

			/* Required number of arguments */
		    if (objC < 3)
		    {
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "bookNum");
				return TCL_ERROR;
		    }

		    /* Get the book number */
			if (GetBookFromObj(interp, objV[2], &k_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the tval */
			tval = k_info[k_idx].tval;
			
			/* Get the sval */
			sval = k_info[k_idx].sval;
		
			/* Get the realm */
			if (tval == p_ptr->realm2 + (TV_LIFE_BOOK - 1))
			{
				realm = p_ptr->realm2;
				forgotten = p_ptr->spell_forgotten2;
				learned = p_ptr->spell_learned2;
				worked = p_ptr->spell_worked2;
			}
			else
			{
				realm = p_ptr->realm1;
				forgotten = p_ptr->spell_forgotten1;
				learned = p_ptr->spell_learned1;
				worked = p_ptr->spell_worked1;
			}
			
			/* Scan arguments for options */
			for (i = 3; i < objC; )
			{
				static cptr cmdOptions[] = {"-known", "-tester", NULL};
				
				/* Get the sub-option */
				if (Tcl_GetIndexFromObj(interp, objV[i], (char **) cmdOptions, (char *) "option",
					0, &index) != TCL_OK)
				{
					return TCL_ERROR;
				}

				switch (index)
				{
					case 0: /* -known */
						if (Tcl_GetBooleanFromObj(interp, objV[i+1],
							&match_known) != TCL_OK)
						{
							return TCL_ERROR;
						}
						request_known = 1;
						i += 2;
						break;

					case 1: /* -tester */
						if (Tcl_GetBooleanFromObj(interp, objV[i+1],
							&match_tester) != TCL_OK)
						{
							return TCL_ERROR;
						}
						request_tester = 1;
						i += 2;
						break;
				}
			}

			/*
			 * If we are not casting or studying a spell, then there is
			 * no need to check spell_okay().
			 */
			if ((p_ptr->command_cmd != 'G') && (p_ptr->command_cmd != 'm'))
			{
				request_tester = 0;
			}
			
			/*
			 * The list of indexes returned by this command are
			 * book-relative. That is, the first spell in a book
			 * is index 0, the next is 1, and so on, except for those
			 * spells which are "illegible".
			 */
			i = -1;

			/* Return spell list */
			listObjPtr = Tcl_NewListObj(0, NULL);
		
			/* Extract spells */
			for (spell = 0; spell < 32; spell++)
			{
				/* Check for this spell */
				if (fake_spell_flags[sval] & (1L << spell))
				{
					magic_type *s_ptr;

					/* Zero-based spell index in this book */
					++i;
		
					/* Access the spell */
					s_ptr = &mp_ptr->info[realm-1][spell];
				
					/* Skip illegible spells */
					if (s_ptr->slevel >= 99) continue;

					/* Return only known spells */
					if (request_known)
					{
						bool known = (learned & (1L << spell)) != 0;
						
						if (known != match_known)
						{
							continue;
						}
					}

					/* Return only "okay" spells */
					if (request_tester && match_tester)
					{
						bool known = p_ptr->command_cmd != 'G';
						if (!spell_okay(spell, known, realm - 1))
						{
							continue;
						}
					}

					/* Append this spellindex to the list */
					Tcl_ListObjAppendElement(interp, listObjPtr,
						Tcl_NewIntObj(i));
				}
			}

			/* Return the list of spell indexes */
			Tcl_SetObjResult(interp, listObjPtr);
			break;

		case IDX_INFO: /* info */

			/* Required number of arguments */
		    if (objC < 5)
		    {
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "bookNum spellIndex arrayName");
				return TCL_ERROR;
		    }

		    /* Get the book number */
			if (GetBookFromObj(interp, objV[2], &k_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}

		    /* Get the spell index */
			if (Tcl_GetIntFromObj(interp, objV[3], &spell) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Verify the spell index */
			if ((spell < 0) || (spell >= 32))
			{
				/* REPORT */
				return TCL_ERROR;
			}

			/* Get the variable name */
			t = Tcl_GetStringFromObj(objV[4], NULL);
			
			/* Get the tval */
			tval = k_info[k_idx].tval;

			/* Get the sval */
			sval = k_info[k_idx].sval;
		
			/* Get the realm */
			if (tval == p_ptr->realm2 + (TV_LIFE_BOOK - 1))
			{
				realm = p_ptr->realm2;
				forgotten = p_ptr->spell_forgotten2;
				learned = p_ptr->spell_learned2;
				worked = p_ptr->spell_worked2;
			}
			else
			{
				realm = p_ptr->realm1;
				forgotten = p_ptr->spell_forgotten1;
				learned = p_ptr->spell_learned1;
				worked = p_ptr->spell_worked1;
			}

			/* Find the first spell in the book */
			first = -1;
			for (i = 0; i < 32; i++)
			{
				if (fake_spell_flags[sval] & (1L << i))
				{
					first = i;
					break;
				}
			}

			if (first == -1)
			{
				/* REPORT */
				return TCL_ERROR;
			}
			
			/* Get the spell index */
			spell += first;
			
			/* Check for this spell */
			if (fake_spell_flags[sval] & (1L << spell))
			{
				magic_type *s_ptr;
				char info[80];
				cptr comment;
				
				/* Access the spell */
				s_ptr = &mp_ptr->info[realm-1][spell];
			
				/* Skip illegible spells */
				if (s_ptr->slevel >= 99)
				{
					/* REPORT */
					return TCL_ERROR;
				}

				if (SetArrayValueChar(t, "char", (char) I2A(spell - first))
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				if (ExtToUtf_SetArrayValueString(t, (char *) "name",
					(char *) spell_names[realm-1][spell]) != TCL_OK)
				{
					return TCL_ERROR;
				}
				if (SetArrayValueLong(t, "level", s_ptr->slevel) != TCL_OK)
				{
					return TCL_ERROR;
				}
				if (SetArrayValueLong(t, "mana", s_ptr->smana) != TCL_OK)
				{
					return TCL_ERROR;
				}
				if (SetArrayValueLong(t, "chance",
					spell_chance(spell, realm - 1)) != TCL_OK)
				{
					return TCL_ERROR;
				}
				if (SetArrayValueLong(t, "known",
					(learned & (1L << spell)) != 0) != TCL_OK)
				{
					return TCL_ERROR;
				}

				spell_info(info, spell, realm-1);
				if (!info[0]) (void) strcpy(info, " ");
				comment = info;
	
				/* Describe the status of the spell */
				if (forgotten & (1L << spell))
				{
					comment = " forgotten";
				}
				else if (!(learned & (1L << spell)))
				{
					comment = " unknown";
				}
				else if (!(worked & (1L << spell)))
				{
					comment = " untried";
				}
				if (ExtToUtf_SetArrayValueString(t, (char *) "info", (char *) comment + 1)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
			}			
			break;

		case IDX_MEMORY: /* memory */
			/* Only OAngband 0.4.0 has spell descriptions */
			Tcl_ResetResult(interp);
			break;
	}

	/* Success */
	return TCL_OK;
}

/*
 * Initializes lists of keywords for various things.
 */
static void init_keyword_misc(void)
{
	int i;
	
	/* A list of gender names */
	C_MAKE(keyword_gender, MAX_SEXES + 1, cptr);
	for (i = 0; i < MAX_SEXES; i++)
	{
		keyword_gender[i] = (char *) sex_info[i].title;
	}
	keyword_gender[MAX_SEXES] = NULL;

	/* A list of class names */
	C_MAKE(keyword_class, MAX_CLASS + 1, cptr);
	for (i = 0; i < MAX_CLASS; i++)
	{
		keyword_class[i] = (char *) class_info[i].title;
	}
	keyword_class[MAX_CLASS] = NULL;

	/* A list of race names */
	C_MAKE(keyword_race, MAX_RACES + 1, cptr);
	for (i = 0; i < MAX_RACES; i++)
	{
		keyword_race[i] = (char *) race_info[i].title;
	}
	keyword_race[MAX_RACES] = NULL;
}


/*
 *--------------------------------------------------------------
 *
 * angtk_eval --
 *
 *	Eval() a command with arguments.
 *
 *--------------------------------------------------------------
 */

void angtk_eval(cptr command, ...)
{
	cptr s = command;
	va_list vp;
	int objc = 0;
	Tcl_Obj *objv[40];
	int i, result;

	/* Start processing variable argument list */
	va_start(vp, command);

	/* Process each string argument */
	while (s)
	{
		/* Append a new string object to the command object */
		/* XXX Some args are already ASCII, safe to translate? */
		objv[objc++] = ExtToUtf_NewStringObj(s, -1);
		Tcl_IncrRefCount(objv[objc - 1]);

		/* Get the next string argument */
		s = va_arg(vp, cptr);
	}

	/* Finish processing variable argument list */
	va_end(vp);

	result = Tcl_EvalObjv(g_interp, objc, objv, TCL_EVAL_GLOBAL);

	for (i = 0; i < objc; i++)
	{
		Tcl_DecrRefCount(objv[i]);
	}

    if (result == TCL_ERROR)
    {
    	/* Report the error */
		Tcl_AddErrorInfo(g_interp,
			"\n    (inside angtk_eval)");
		Tcl_BackgroundError(g_interp);
    }
}

/*
 *--------------------------------------------------------------
 *
 * angtk_display_info --
 *
 *	Calls "angband_display info TITLE LIST" where LIST is a given
 *	array of strings, and TITLE is displayed in the window.
 *	Used for "Self Knowledge" and others.
 *
 *--------------------------------------------------------------
 */

static Tcl_Obj *s_info_list = NULL;

void angtk_display_info_init(void)
{
	s_info_list = Tcl_NewListObj(0, NULL);
}

void angtk_display_info_append(cptr s)
{
	Tcl_ListObjAppendElement(g_interp, s_info_list,
		ExtToUtf_NewStringObj(s, -1));
}

void angtk_display_info_done(cptr title)
{
	angtk_display_info_aux((char *) title, s_info_list);
}

void angtk_display_info_aux(char *title, Tcl_Obj *listObjPtr)
{
	int i, objc = 0, result;
	Tcl_Obj *objv[10];

	objv[objc++] = Tcl_NewStringObj("angband_display", -1);
	objv[objc++] = Tcl_NewStringObj("info", -1);
	objv[objc++] = Tcl_NewStringObj("show", -1);
	objv[objc++] = ExtToUtf_NewStringObj(title, -1);
	objv[objc++] = listObjPtr;

	for (i = 0; i < objc; i++)
		Tcl_IncrRefCount(objv[i]);

	result = Tcl_EvalObjv(g_interp, objc, objv, TCL_EVAL_GLOBAL);

	for (i = 0; i < objc; i++)
		Tcl_DecrRefCount(objv[i]);

    if (result == TCL_ERROR)
    {
    	/* Report the error */
		Tcl_AddErrorInfo(g_interp,
			"\n    (inside angtk_display_info_aux)");
		Tcl_BackgroundError(g_interp);
    }
}

void angtk_display_info(char *title, char **info, int count)
{
	int i, objc = 0;
	Tcl_Obj *objv[256];

	/* Paranoia */
	if (count > 256)
	{
		plog("angtk_display_info(): too many strings");
		count = 256;
	}

	/* Check each string */
	for (i = 0; i < count; i++)
	{
		/* Create a new string object */
		objv[objc++] = ExtToUtf_NewStringObj(info[i], -1);
	}

	/* Pass the title and list of strings to Tcl */
	angtk_display_info_aux(title, Tcl_NewListObj(objc, objv));
}

static void HandleError(void)
{
	char path[1024];
	char *errorInfo;
	FILE *fp;

	/* Dump the stack to errors.txt */
	path_build(path, 1024, ANGBAND_DIR_TK, "errors.txt");
	fp = fopen(path, "a");
	if (fp != NULL)
	{
		errorInfo = Tcl_GetVar(g_interp, (char *) "errorInfo", TCL_GLOBAL_ONLY);
		fprintf(fp, "***** (inside HandleError)\n\n%s\n\n", errorInfo);
		fclose(fp);
	}

	/* Display a message and quit */
	quit_fmt("The following error occurred:\n\n%s\n\n"
			 "Please examine the errors.txt file to see what happened.",
				Tcl_GetStringResult(g_interp));
}

/*
 * Initialize stuff after the character has been generated.
 */
static void angtk_character_generated(void)
{
	char path[1024];

	/* Source a file to create the interface */
	path_build(path, 1024, ANGBAND_DIR_TK, "init-other.tcl");
	if (angtk_eval_file(path) == TCL_ERROR)
	{
		HandleError();
	}

	/* The icon environment must be initialized by a script. */
	if (g_icon_size == 0)
	{
		quit_fmt("Fatal error:\nIcons were not initialized.\n"
			"You must call \"angband init_icons\"");
	}

	/*
	 * Assign icons to each grid. You have to do this *after* sourcing
	 * the startup script, because that's where icon types are defined
	 * and where icon assignments are made.
	 */
	g_icon_map_changed = TRUE;
}


static CommandInit commandInit[] = {
	{0, "angband", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "a_info", 0, 0, NULL, NULL, (ClientData) 0},
			{2, "find", 1, 0, NULL, objcmd_ARRAY_find, (ClientData) "artifact_type"},
			{2, "info", 3, 0, "a_idx option ?args ...?", objcmd_ARRAY_info, (ClientData) "artifact_type"},
			{2, "max", 1, 1, NULL, objcmd_ARRAY_max, (ClientData) "artifact_type"},
			{2, "set", 2, 4, "a_idx ?field? ?value?", objcmd_ARRAY_set, (ClientData) "artifact_type"},
		{1, "f_info", 0, 0, NULL, NULL, (ClientData) 0},
			{2, "find", 1, 0, NULL, objcmd_ARRAY_find, (ClientData) "feature_type"},
			{2, "info", 3, 0, "f_idx option ?args ...?", objcmd_ARRAY_info, (ClientData) "feature_type"},
			{2, "max", 1, 1, NULL, objcmd_ARRAY_max, (ClientData) "feature_type"},
			{2, "set", 2, 4, "f_idx ?field? ?value?", objcmd_ARRAY_set, (ClientData) "feature_type"},
		{1, "k_info", 0, 0, NULL, NULL, (ClientData) 0},
			{2, "find", 1, 0, NULL, objcmd_ARRAY_find, (ClientData) "object_kind"},
			{2, "info", 3, 0, "k_idx option ?args ...?", objcmd_ARRAY_info, (ClientData) "object_kind"},
			{2, "max", 1, 1, NULL, objcmd_ARRAY_max, (ClientData) "object_kind"},
			{2, "set", 2, 4, "k_idx ?field? ?value?", objcmd_ARRAY_set, (ClientData) "object_kind"},
		{1, "r_info", 0, 0, NULL, NULL, (ClientData) 0},
			{2, "find", 1, 0, NULL, objcmd_ARRAY_find, (ClientData) "monster_race"},
			{2, "info", 3, 0, "r_idx option ?args ...?", objcmd_ARRAY_info, (ClientData) "monster_race"},
			{2, "max", 1, 1, NULL, objcmd_ARRAY_max, (ClientData) "monster_race"},
			{2, "set", 2, 4, "r_idx ?field? ?value?", objcmd_ARRAY_set, (ClientData) "monster_race"},
		{1, "m_list", 0, 0, NULL, NULL, (ClientData) 0},
			{2, "find", 1, 0, NULL, objcmd_ARRAY_find, (ClientData) "monster_type"},
			{2, "info", 3, 0, "m_idx option ?args ...?", objcmd_ARRAY_info, (ClientData) "monster_type"},
			{2, "max", 1, 1, NULL, objcmd_ARRAY_max, (ClientData) "monster_type"},
			{2, "set", 2, 4, "m_idx ?field? ?value?", objcmd_ARRAY_set, (ClientData) "monster_type"},
		{1, "o_list", 0, 0, NULL, NULL, (ClientData) 0},
			{2, "find", 1, 0, NULL, objcmd_ARRAY_find, (ClientData) "object_type"},
			{2, "info", 3, 0, "o_idx option ?args ...?", objcmd_ARRAY_info, (ClientData) "object_type"},
			{2, "max", 1, 1, NULL, objcmd_ARRAY_max, (ClientData) "object_type"},
			{2, "set", 2, 4, "o_idx ?field? ?value?", objcmd_ARRAY_set, (ClientData) "object_type"},
		{1, "cave", 0, 0, NULL, objcmd_cave, (ClientData) 0},
		{1, "equipment", 0, 0, NULL, objcmd_equipment, (ClientData) 0},
		{1, "game", 0, 0, NULL, objcmd_game, (ClientData) 0},
		{1, "info", 0, 0, NULL, objcmd_info, (ClientData) 0},
		{1, "inkey_flags", 1, 1, NULL, objcmd_inkey_flags, (ClientData) 0},
		{1, "inventory", 0, 0, NULL, objcmd_inventory, (ClientData) 0},
		{1, "keypress", 2, 2, "string", objcmd_keypress, (ClientData) 0},
		{1, "macro", 0, 0, NULL, objcmd_macro, (ClientData) 0},
		{1, "message", 0, 0, NULL, objcmd_message, (ClientData) 0},
		{1, "player", 0, 0, NULL, objcmd_player, (ClientData) 0},
		{1, "setting", 0, 0, NULL, objcmd_setting, (ClientData) 0},
		{1, "spell", 0, 0, NULL, objcmd_spell, (ClientData) 0},
		{1, "equipinfo", 3, 3, "slot arrayName", objcmd_equipinfo, (ClientData) 0},
		{1, "inveninfo", 3, 3, "slot arrayName", objcmd_inveninfo, (ClientData) 0},
		{1, "init_icons", 3, 3, "size depth", objcmd_init_icons, (ClientData) 0},
		{1, "floor", 0, 0, NULL, objcmd_floor, (ClientData) 0},
		{1, "keycount", 0, 0, NULL, objcmd_keycount, (ClientData) 0},
		{1, "mindcraft", 0, 0, NULL, objcmd_mindcraft, (ClientData) 0},
		{1, "power", 0, 0, NULL, objcmd_power, (ClientData) 0},
	{0, "menuentrystate", 0, 0, NULL, objcmd_menuentrystate, (ClientData) 0},
	{0, "photoget", 0, 0, NULL, objcmd_photo_get, (ClientData) 0},
	{0, "photomask", 2, 3, "imageDst ?imageMask?", objcmd_photo_mask, (ClientData) 0},
	{0, "fontdesc", 2, 2, "font", objcmd_fontdesc, (ClientData) 0},
	{0, "term_window", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "char", 3, 3, "x y", objcmd_term_char, (ClientData) 0},
		{1, "attr", 3, 3, "x y", objcmd_term_attr, (ClientData) 0},
	{0, NULL, 0, 0, NULL, NULL, (ClientData) 0}
};

/*
 * Initialize stuff after Tcl/Tk but before a game is started.
 */
void angtk_init(void)
{
	char path[1024];
	int i;

	/* Hash table for object kind "tval" */
	init_tval_hash();

	/* Settings */
	init_settings();

	for (i = 0; commandInit[i].name; ++i)
	{
		if (streq(commandInit[i].name, "setting"))
		{
			commandInit[i].clientData = g_setting;
			break;
		}
	}
	
	/* Tcl commands */
	CommandInfo_Init(g_interp, commandInit, NULL);

	/* Cool quasi-event bindings */
	init_bindings();

	init_const(g_interp);

	/* Standard color palette */
	init_palette();
#if 0	
	/* Testing */
	path_build(path, 1024, ANGBAND_DIR_TK, "term.tcl");
	if (angtk_eval_file(path) == TCL_ERROR)
	{
		HandleError();
	}
#endif /* 0 */
	
	/* Source the "startup script" */
	path_build(path, 1024, ANGBAND_DIR_TK, "init-startup.tcl");
	if (angtk_eval_file(path) == TCL_ERROR)
	{
		HandleError();
	}
}

/*
 * Initialize stuff after init_angband().
 */
void angtk_angband_initialized(void)
{
	int i;

	/* Ack! Angband has dynamic p_info array */
	/* Names of gender, race and class */
	init_keyword_misc();

	/* These are required for savefile loading (see lite_spot()) */
	for (i = 0; i < MAX_HGT; i++)
	{
		/* Info about what feature/monster/object is known. */
		C_MAKE(g_grid[i], MAX_WID, t_grid);
	}

	init_map();

	init_struct();

	/* Program is intialized */
	if (Tcl_EvalEx(g_interp, (char *) "angband_initialized", -1, TCL_EVAL_GLOBAL) != TCL_OK)
	{
		HandleError();
	}
	
	/* Hack - look at more scripts */
	angtk_character_generated();
}

/*
 * Tcl_Eval() a file, assuming the given filename is not UTF-8.
 */
int angtk_eval_file(cptr extFileName)
{
	cptr utfFileName;
	Tcl_DString dString;
	int result;

	utfFileName = Tcl_ExternalToUtfDString(NULL, extFileName, -1, &dString);
	result = Tcl_EvalFile(g_interp, (char *) utfFileName);
	Tcl_DStringFree(&dString);
	return result;
}


/*
 * Do stuff after a dungeon is generated.
 */
void angtk_cave_generated(void)
{
	g_icon_map_changed = TRUE;

	Bind_Generic(EVENT_DUNGEON, KEYWORD_DUNGEON_GEN + 1);
}


/*
 * Return TRUE if the character should be allowed to attempt to move
 * onto the given cave location. If "message" is TRUE and the character
 * cannot move onto the location, then a message is displayed and the
 * location may be marked if it wasn't already.
 *
 * Valid bits for 'mode' are:
 *    0x01 - Display message
 *    0x02 - Apply damage; When character is at y,x, apply damage due to
 *           being unprotected on this grid. (NOT USED)
 */
bool player_test_feature(int y, int x, int mode)
{
	cptr msg = NULL;
/*	int wt; */
	bool known = FALSE;
	bool message = FALSE;
	bool note = FALSE;
	bool pass_wall = FALSE;
	int feat = area(x, y)->feat;
	
	/* Hack -- walking into unseen features obtains knowledge XXX XXX */
	/* if ((cave[y][x].info & (CAVE_MARK))) known = TRUE; */
	known = TRUE;

	/* Player can not walk through "walls" unless in Shadow Form */
	if ((p_ptr->wraith_form) || (p_ptr->pass_wall))
		pass_wall = TRUE;
	
	/* Print a message if asked */
	if (mode & 0x01) message = TRUE;

	/* Examine the feature */
	switch (feat)
	{
		case FEAT_CLOSED:
			if (pass_wall) return (TRUE);
			if (!known)
			{
				msg = "You feel a closed door blocking your way";
				note = TRUE;
			}
			else
				msg = "There is a door in the way!";
#ifdef ALLOW_EASY_ALTER

			if (known && easy_alter)
			{
				/* Jammed door */
				if (feat >= FEAT_DOOR_HEAD + 0x08)
				{
					msg = "The door appears to be stuck.";
				}
				else
					return (TRUE);
			}

#endif /* ALLOW_EASY_ALTER */
			break;

		case FEAT_RUBBLE:
			if (pass_wall) return (TRUE);
			if (!known)
			{
				msg = "You feel some rubble blocking your way.";
				note = TRUE;
			}
			else
				msg = "There is a pile of rubble in the way!";
			break;

		case FEAT_SECRET:
		case FEAT_MAGMA:
		case FEAT_QUARTZ:
		case FEAT_MAGMA_K:
		case FEAT_QUARTZ_K:
		case FEAT_WALL_EXTRA:
		case FEAT_WALL_INNER:
		case FEAT_WALL_OUTER:
		case FEAT_WALL_SOLID:
			if (pass_wall) return (TRUE);
			if (!known)
			{
				msg = "You feel a wall blocking your way.";
				note = TRUE;
			}
			else
				msg = "There is a wall in the way!";
			break;

		case FEAT_PERM_EXTRA:
		case FEAT_PERM_INNER:
		case FEAT_PERM_OUTER:
		case FEAT_PERM_SOLID:
			if (!known)
			{
				msg = "You feel a wall blocking your way.";
				note = TRUE;
			}
			else
				msg = "There is a wall in the way!";
			break;

		case FEAT_TREES:
			if ((p_ptr->pclass != CLASS_RANGER))
			{
				if (message) p_ptr->energy += 10;
			}
			return (TRUE);
			break;

		case FEAT_MOUNTAIN:
			if (!known)
			{
				msg = "You feel a mountain blocking your way.";
				note = TRUE;
			}
			else
				msg = "You can't climb the mountains!";
			break;

		case FEAT_DEEP_WATER:
#if 1
			/* The character may drown later */
			return (TRUE);
#else
			if (p_ptr->ffall) return (TRUE);
			wt = (adj_str_wgt[p_ptr->stat_ind[A_STR]] * 100) / 2;
			if (p_ptr->total_weight < wt) return (TRUE);
			if (!known)
			{
				msg = "You feel deep water blocking your way.";
				note = TRUE;
			}
			else
				msg = "You can't swim with that much weight.";
#endif
			break;

		case FEAT_DEEP_LAVA:
#if 1
			/* The character may burn later */
			return (TRUE);
#else
			if (!known)
			{
				msg = "You feel the heat of deep lava blocking your way.";
				note = TRUE;
			}
			else
				msg = "There is a river of lava in the way!";
#endif
			break;

		case FEAT_SHAL_LAVA:
#if 1
			/* The character may sizzle later */
			return (TRUE);
#else
			if (p_ptr->ffall) return (TRUE);
			if ((p_ptr->resist_fire) ||
				(p_ptr->immune_fire) || 
			    (p_ptr->oppose_fire))
			{
				return (TRUE);
			}
			if (!known)
			{
				msg = "You feel the heat of shallow lava blocking your way.";
				note = TRUE;
			}
			else
				msg = "The heat of the lava is too intense to cross!";
#endif
			break;
			
		default:
			return (TRUE);
			break;
	}

	/* The grid is unknown, and no message is requested */
	if (!known && !message) return (TRUE);

	/* A message was requested */
	if (message)
	{
		/* Print a message */
		if (msg) msgf(msg);

		/* XXX Hack -- No energy is used */
		if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
			p_ptr->energy = 0;
	}

	/* Character cannot move here */
	return (FALSE);
}

