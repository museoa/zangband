/* File: bind.c */

/* Purpose: quasi-event binding code */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include <tcl.h>
#include "angband.h"
#include "tnb.h"
#include "interp.h"
#include "util-dll.h"
#include "qebind-dll.h"
#include "icon.h"

static void ExpandAssign(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandChoose(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandCursor(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandDungeon(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandGeneric(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandKeymap(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandPosition(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandPy(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandSetting(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandStatus(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandTarget(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
static void ExpandTrack(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);

cptr keyword_assign[] = {"monster", "object", "character",
		"feature", "shopkeeper", "artifact", NULL};

cptr keyword_choose[] = {"item", "spell",
	"power", "mindcraft", "cmd_pet", NULL};

cptr keyword_cursor[] = {"show", "hide", NULL};
cptr keyword_dungeon[] = {"enter", "leave", "generate", NULL};

cptr keyword_inkey[] = {"cmd", "dir", "disturb", "item", "item_store",
	"more", "spell", "target",
	"mindcraft", "power", "cmd_pet", NULL};

cptr keyword_py[] = {"hitpoints", "mana", "food", "depth", "exp",
	"level", "armor_class", "name", "title", "gold",
	"race", NULL};

cptr keyword_stat[] = {"strength", "intelligence", "wisdom", "dexterity",
	"constitution", "charisma", NULL};
cptr keyword_status[] = {"cut", "stun", "hunger", "blind", "confused",
	"afraid", "poisoned", "state", "speed", "study", "winner",
	NULL};

cptr *keyword_setting = NULL;
cptr keyword_target[] = {"set", "unset", "visibility", NULL};
cptr keyword_term[] = {"fresh", "inkey", NULL};
cptr keyword_track[] = {"health", "race", "object", "inventory", "equipment",
	"grid", "message", NULL};

typedef struct EventInfo {
	cptr name; /* Name of event (ex "Stat", "Position") */
	int type; /* Event type (ex Stat, Position) */
	QE_ExpandProc expand; /* Callback to expand % in scripts */
	cptr *detail; /* Array of detail names, or NULL */
} EventInfo;

/*
 * Table to initialize event types. Elements must be in order of
 * EVENT_XXX constants.
 */
static EventInfo eventArray[] = {
	{"Position", EVENT_POSITION, ExpandPosition, NULL},
	{"Stat", EVENT_STAT, ExpandGeneric, keyword_stat},
	{"Target", EVENT_TARGET, ExpandTarget, keyword_target},
	{"Status", EVENT_STATUS, ExpandStatus, keyword_status},
	{"Inkey", EVENT_INKEY, ExpandGeneric, keyword_inkey},
	{"Cursor", EVENT_CURSOR, ExpandCursor, keyword_cursor},
	{"Assign", EVENT_ASSIGN, ExpandAssign, keyword_assign},
	{"Term", EVENT_TERM, ExpandGeneric, keyword_term},
	{"Choose", EVENT_CHOOSE, ExpandChoose, keyword_choose},
	{"Track", EVENT_TRACK, ExpandTrack, keyword_track},
	{"Py", EVENT_PY, ExpandPy, keyword_py},
	{"Setting", EVENT_SETTING, ExpandSetting, NULL},
	{"Dungeon", EVENT_DUNGEON, ExpandDungeon, keyword_dungeon},
	{"Keymap", EVENT_KEYMAP, ExpandKeymap, NULL},
	{NULL, 0, NULL, NULL}
};

/*
 * Wrapper around QE_ExpandString().
 */
static void ExtToUtf_ExpandString(char *extString, Tcl_DString *result)
{
	char *utfString;
	Tcl_DString utfDString;

	utfString = Tcl_ExternalToUtfDString(NULL, extString, -1, &utfDString);
	QE_ExpandString(utfString, result);
	Tcl_DStringFree(&utfDString);
}

/*
 * %-substitution for EVENT_ASSIGN
 */
static void ExpandAssign(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	struct {int toIndex; t_assign *assignPtr;} *clientData = eventPtr->clientData;
	char buf[128];

	switch (which)
	{
		case 'I': /* -toindex */
			QE_ExpandNumber(clientData->toIndex, result);
			break;

		case 'a': /* -assign */
			QE_ExpandString(assign_print(buf, clientData->assignPtr),
				result);
			break;
	
		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * %-substitution for EVENT_CHOOSE
 */
static void ExpandChoose(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	struct {int other, show;} *clientData = eventPtr->clientData;

	switch (which)
	{
		case 'o': /* other */
			if (eventPtr->detail == KEYWORD_CHOOSE_ITEM + 1)
			{
				static cptr s[] = {"inventory", "equipment", "floor", NULL};
				QE_ExpandString(s[clientData->other], result);
			}
			else
				QE_ExpandNumber(clientData->other, result);
			break;

		case 's': /* show/hide */
			QE_ExpandNumber(clientData->show, result);
			break;
		
		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * %-substitution for EVENT_CURSOR
 */
static void ExpandCursor(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	struct {int y, x;} *clientData = eventPtr->clientData;

	switch (which)
	{
		case 'y': /* y */
			QE_ExpandNumber(clientData->y, result);
			break;

		case 'x': /* x */
			QE_ExpandNumber(clientData->x, result);
			break;
		
		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * %-substitution for EVENT_DUNGEON
 */
static void ExpandDungeon(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	switch (which)
	{
		case 'c': /* depth */
			QE_ExpandNumber(p_ptr->depth, result);
			break;
		
		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * %-substitution for any event
 */
static void ExpandGeneric(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	char *string;
	EventInfo *eiPtr = &eventArray[eventPtr->type - 1];

	switch (which)
	{
		case 'd': /* detail */
			string = (char *) eiPtr->detail[eventPtr->detail - 1];
			QE_ExpandString(string, result);
			break;

		case 'W': /* object */
			QE_ExpandString((char *) object, result);
			break;

		default:
			QE_ExpandUnknown(which, result);
			break;
	}
}

/*
 * %-substitution for EVENT_KEYMAP
 */
static void ExpandKeymap(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	/* Hack - ignore parameter */
	(void) object;

	switch (which)
	{
		case 'c': /* ch */
			QE_ExpandString((char *) eventPtr->clientData, result);
			break;

		default:
			QE_ExpandUnknown(which, result);
			break;
	}
}

/*
 * %-substitution for EVENT_POSITION
 */
static void ExpandPosition(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	struct {int who, y1, x1, y2, x2;} *clientData = eventPtr->clientData;
	int number;

	switch (which)
	{
		case 'y':
			number = clientData->y2;
			QE_ExpandNumber(number, result);
			break;

		case 'x':
			number = clientData->x2;
			QE_ExpandNumber(number, result);
			break;

		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * %-substitution for EVENT_PY
 */
static void ExpandPy(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	double number = 0;
	long expadv;

	switch (eventPtr->detail)
	{
		case KEYWORD_PY_HP + 1:
			switch (which)
			{
				case 'c': /* chp */
					QE_ExpandNumber(p_ptr->chp, result);
					return;
		
				case 'm': /* mhp */
					QE_ExpandNumber(p_ptr->mhp, result);
					return;

				case 'f': /* chp/mhp */
					if (p_ptr->mhp) number = (double) p_ptr->chp / p_ptr->mhp;
					QE_ExpandDouble(number, result);
					return;
			}
			break;

		case KEYWORD_PY_SP + 1:
			switch (which)
			{
				case 'c': /* csp */
					QE_ExpandNumber(p_ptr->csp, result);
					return;
		
				case 'm': /* msp */
					QE_ExpandNumber(p_ptr->msp, result);
					return;

				case 'f': /* csp/msp */
					if (p_ptr->msp) number = (double) p_ptr->csp / p_ptr->msp;
					QE_ExpandDouble(number, result);
					return;
			}
			break;

		case KEYWORD_PY_FOOD + 1:
			switch (which)
			{
				case 'c': /* food */
					QE_ExpandNumber(p_ptr->food, result);
					return;
		
				case 'm': /* PY_FOOD_MAX */
					QE_ExpandNumber(PY_FOOD_MAX, result);
					return;

				case 'f': /* food/PY_FOOD_MAX */
					QE_ExpandDouble((double) p_ptr->food / PY_FOOD_MAX, result);
					return;
			}
			break;

		case KEYWORD_PY_DEPTH + 1:
			switch (which)
			{
				case 'c': /* current */
					QE_ExpandNumber(p_ptr->depth, result);
					return;
		
				case 'm': /* max_depth */
					QE_ExpandNumber(p_ptr->max_depth, result);
					return;
			}
			break;

		case KEYWORD_PY_EXP + 1:
			switch (which)
			{
				case 'a': /* advance */
					if (p_ptr->lev >= PY_MAX_LEVEL)
					{
						expadv = 999999999;
					}
					else
					{
						expadv = (s32b)(player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100L);
					}
					QE_ExpandNumber(expadv, result);
					return;
		
				case 'c': /* exp */
					QE_ExpandNumber(p_ptr->exp, result);
					return;

				case 'm': /* max_exp */
					QE_ExpandNumber(p_ptr->max_exp, result);
					return;
			}
			break;

		case KEYWORD_PY_LEVEL + 1:
			switch (which)
			{
				case 'c': /* lev */
					QE_ExpandNumber(p_ptr->lev, result);
					return;

				case 'm': /* max_lev */
					QE_ExpandNumber(p_ptr->max_depth, result);
					return;
			}
			break;

		case KEYWORD_PY_AC + 1:
			switch (which)
			{
				case 'c': /* dis_ac */
					QE_ExpandNumber(p_ptr->dis_ac, result);
					return;

				case 't': /* dis_to_a */
					QE_ExpandNumber(p_ptr->dis_to_a, result);
					return;
			}
			break;

		case KEYWORD_PY_NAME + 1:
			switch (which)
			{
				case 'c': /* name */
					ExtToUtf_ExpandString(player_name, result);
					return;
			}
			break;

		case KEYWORD_PY_TITLE + 1:
			switch (which)
			{
				case 'c': /* title */
					ExtToUtf_ExpandString((char *) player_title[p_ptr->pclass][(p_ptr->lev-1)/5], result);
					return;
			}
			break;

		case KEYWORD_PY_GOLD + 1:
			switch (which)
			{
				case 'c': /* gold */
					QE_ExpandNumber(p_ptr->au, result);
					return;
			}
			break;

		/* The character's race may change in ZAngband! */
		case KEYWORD_PY_RACE + 1:
			switch (which)
			{
				case 'c': /* race */
					ExtToUtf_ExpandString((char *) rp_ptr->title, result);
					return;
			}
			break;
	}

	ExpandGeneric(which, object, eventPtr, result);
}

/*
 * %-substitution for EVENT_SETTING
 */
static void ExpandSetting(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	switch (which)
	{
		case 'c': /* value */
			QE_ExpandNumber((int) eventPtr->clientData, result);
			break;

		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * %-substitution for EVENT_STATUS
 */
static void ExpandStatus(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	struct {char *format; int value;} *clientData = eventPtr->clientData;

	switch (which)
	{
		case 'f': /* format */
			QE_ExpandString(clientData->format, result);
			break;

		case 'v': /* value */
			QE_ExpandNumber(clientData->value, result);
			break;

		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * %-substitution for EVENT_TARGET
 */
static void ExpandTarget(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	switch (which)
	{
		case 'w': /* who */
			QE_ExpandNumber(p_ptr->target_who, result);
			break;

		case 'r': /* r_idx */
			if (p_ptr->target_who > 0)
			{
				monster_type *m_ptr = &m_list[p_ptr->target_who];
				QE_ExpandNumber(m_ptr->r_idx, result);
			}
			else
			{
				QE_ExpandNumber(0, result);
			}
			break;

		case 'y': /* y */
			QE_ExpandNumber(p_ptr->target_row, result);
			break;

		case 'x': /* x */
			QE_ExpandNumber(p_ptr->target_col, result);
			break;

		case 'v': /* visibility */
			QE_ExpandNumber(target_vis, result);
			break;
		
		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * %-substitution for EVENT_TRACK
 */
static void ExpandTrack(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	struct {int who; int y; int x;} *clientData = eventPtr->clientData;
	monster_type *m_ptr;

	switch (which) {

		case 'f': /* friend */
			m_ptr = &m_list[clientData->who];
			QE_ExpandNumber((is_friendly(m_ptr) || is_pet(m_ptr)), result);
			break;

		case 'w': /* who */
			QE_ExpandNumber(clientData->who, result);
			break;

		case 'x': /* x */
			QE_ExpandNumber(clientData->x, result);
			break;

		case 'y': /* y */
			QE_ExpandNumber(clientData->y, result);
			break;

		default:
			ExpandGeneric(which, object, eventPtr, result);
			break;
	}
}

/*
 * Generate an EVENT_ASSIGN quasi-event
 */
void Bind_Assign(int to, int toIndex, t_assign *assignPtr)
{
	QE_Event event;
	struct {int toIndex; t_assign *assignPtr;} clientData;

	clientData.toIndex = toIndex;
	clientData.assignPtr = assignPtr;

	event.type = EVENT_ASSIGN;
	event.detail = to;
	event.clientData = (ClientData) &clientData;

	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Generate an EVENT_CHOOSE quasi-event
 */
void Bind_Choose(int detail, int other, int show)
{
	QE_Event event;
	struct {int other, show;} clientData;

	clientData.other = other;
	clientData.show = show;

	event.type = EVENT_CHOOSE;
	event.detail = detail;
	event.clientData = (ClientData) &clientData;

	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Generate an EVENT_CURSOR quasi-event
 */
void Bind_Cursor(int detail, int y, int x)
{
	QE_Event event;
	struct {int y, x;} clientData;

	clientData.y = y;
	clientData.x = x;

	event.type = EVENT_CURSOR;
	event.detail = detail;
	event.clientData = (ClientData) &clientData;

	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Generate a quasi-event
 */
void Bind_Generic(int eventType, int eventDetail)
{
	QE_Event event;

	event.type = eventType;
	event.detail = eventDetail;
	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Generate an EVENT_KEYMAP quasi-event
 */
void Bind_Keymap(int ch)
{
	QE_Event event;
	char string[3];

	if (iscntrl(ch))
	{
		(void) sprintf(string, "^%c", ch + 64);
	}
	else
	{
		(void) sprintf(string, "%c", ch);
	}
	
	event.type = EVENT_KEYMAP;
	event.detail = 0;
	event.clientData = (ClientData) string;
	
	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Generate an EVENT_POSITION quasi-event
 */
void Bind_Position(int who, int y1, int x1, int y2, int x2)
{
	QE_Event event;
	struct {int who, y1, x1, y2, x2;} clientData;

	clientData.who = who;
	clientData.y1 = y1;
	clientData.x1 = x1;
	clientData.y2 = y2;
	clientData.x2 = x2;

	event.type = EVENT_POSITION;
	event.detail = 0;
	event.clientData = (ClientData) &clientData;

	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Generate an EVENT_SETTING quasi-event
 */
void Bind_Setting(int detail, int value)
{
	QE_Event event;

	event.type = EVENT_SETTING;
	event.detail = detail;
	event.clientData = (ClientData) value;
	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Generate an EVENT_STATUS quasi-event
 */
void Bind_Status(int detail)
{
	QE_Event event;
	struct {cptr format; int value;} clientData;

	clientData.format = player_status(detail - 1, &clientData.value);

	event.type = EVENT_STATUS;
	event.detail = detail;
	event.clientData = (ClientData) &clientData;
	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Generate an EVENT_TRACK quasi-event
 */
void Bind_Track(int detail, int who, int y, int x)
{
	QE_Event event;
	struct {int who; int y; int x;} clientData;

	/* .who is m_list[] for Track-health, r_info[] for Track-race */
	clientData.who = who;
	clientData.y = y;
	clientData.x = x;

	event.type = EVENT_TRACK;
	event.detail = detail;
	event.clientData = (ClientData) &clientData;

	(void) QE_BindEvent(bindingTable, &event);
}

/*
 * Initialize the quasi-event package and define events
 */
void init_bindings(void)
{
	EventInfo *eiPtr;
	int i;

	/* One-time package initialization */
	QE_BindInit(g_interp);

	/* Hack -- Detail names determined at run-time */
	eventArray[EVENT_SETTING - 1].detail = keyword_setting;

	/* Define our event types */
	for (eiPtr = eventArray; eiPtr->name; eiPtr++)
	{
		/* Define the event type */
		QE_InstallEvent(eiPtr->name, eiPtr->type, eiPtr->expand);

		/* Check each detail */
		for (i = 0; eiPtr->detail && eiPtr->detail[i]; i++)
		{
			/* Define a detail for this event type */
			QE_InstallDetail((char *) eiPtr->detail[i], eiPtr->type, i + 1);
		}
	}
}

