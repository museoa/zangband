/* File: qebind.c */

/* Purpose: implements quasi-events */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/*
 * A general purpose module that allows a program to send event-like
 * messages to scripts, and to bind Tcl commands to those quasi-events.
 * Each event has it's own detail field and other fields, and this
 * module performs %-substitution on bound scripts just like regular
 * Tk binding model.
 *
 * To use it first call QE_BindInit() to initialize the package.
 * Then call QE_InstallEvent() for each new event you wish to define.
 * For events with details, call QE_InstallDetail() to register each
 * detail associated with a specific event type. Then create a
 * binding table, which records all binding commands defined by your
 * scripts, with QE_CreateBindingTable(). The "qebind" command is
 * called to associate Tcl commands with a given event for a particular
 * object. The objects that commands are bound to can be a Tk widget or any
 * string, just like the usual "bind" command. Bindings are not automatically
 * deleted when a widget is destroyed.
 */

#include <ctype.h>
#include <string.h>
#include <tcl.h>
#include <tk.h>
#include "qebind-dll.h"

int debug_bindings;

/*
 * Allow bindings to be deactivated.
 */
#define BIND_ACTIVE 1

#define ALLOW_INSTALL 1

typedef struct BindingTable {
	Tcl_Interp *interp;
	Tcl_HashTable patternTable; /* Key: PatternTableKey, Value: (BindValue *) */
	Tcl_HashTable objectTable; /* Key: ObjectTableKey, Value: (BindValue *) */
} BindingTable;

typedef struct BindValue {
	int type; /* Type of event (EVENT_STAT, EVENT_POSITION, etc) */
	int detail; /* Misc. other information, or 0 for none */
	ClientData object;
	char *command;
	int specific; /* For less-specific events (detail=0), this is 1
				   * if a more-specific event (detail>0) exists. */
	struct BindValue *nextValue; /* list of BindValues matching event */
#if BIND_ACTIVE
	int active; /* 1 if binding is "active", 0 otherwise */
#endif /* BIND_ACTIVE */
} BindValue;

typedef struct Pattern {
	int type; /* Type of event (EVENT_STAT, EVENT_POSITION, etc) */
	int detail; /* Misc. other information, or 0 for none */
} Pattern;

typedef struct PatternTableKey {
	int type; /* Type of event (EVENT_STAT, EVENT_POSITION, etc) */
	int detail; /* Misc. other information, or 0 for none */
} PatternTableKey;

typedef struct ObjectTableKey {
	int type; /* Type of event (EVENT_STAT, EVENT_POSITION, etc) */
	int detail; /* Misc. other information, or 0 for none */
	ClientData object; /* Object info */
} ObjectTableKey;
 
typedef struct EventInfo {
	char *name; /* Name of event (ex "Stat", "Position") */
	int type; /* Event type (ex EVENT_STAT, EVENT_POSITION) */
	QE_ExpandProc expand; /* Callback to expand % in scripts */
} EventInfo;

/* Maps event names and types to EventInfo */
static Tcl_HashTable eventTableByName, eventTableByType;

typedef struct Detail {
	Tk_Uid name; /* Name of detail */
	int code; /* Detail code */
	EventInfo *event; /* Associated event */
	struct Detail *nextDetail; /* List of Detail's with same name */
#if ALLOW_INSTALL
	char *command; /* Tcl command if qeinstall'd, else NULL */
#endif
} Detail;

static Tcl_HashTable detailTableByName, detailTableByType;

static void ExpandPercents(ClientData object, char *command,
	QE_Event *eventPtr, EventInfo *eiPtr, Tcl_DString *result);
static int ParseEventDescription(Tcl_Interp *interp, char *eventPattern,
	Pattern *patPtr, EventInfo **eventInfoPtr, Detail **detailPtr);
static BindValue *FindSequence(Tcl_Interp *interp, Tcl_HashTable *objectTablePtr,
	ClientData object, char *eventString, int create, int *created);
#if ALLOW_INSTALL
static void Expand_Install(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);
#endif

static int initialized = 0;
QE_BindingTable bindingTable;

int QE_BindInit(Tcl_Interp *interp)
{
	if (initialized) return TCL_OK;

	Tcl_InitHashTable(&eventTableByName, TCL_STRING_KEYS);
	Tcl_InitHashTable(&eventTableByType, TCL_ONE_WORD_KEYS);
	Tcl_InitHashTable(&detailTableByName, TCL_STRING_KEYS);
	Tcl_InitHashTable(&detailTableByType,
		sizeof(PatternTableKey) / sizeof(int));

	/* Create a new binding table for this interpreter */
	bindingTable = QE_CreateBindingTable(interp);

	/*
	 * Create the "qebind" command. Because the bindingTable is passed
	 * as ClientData to the command, we are only allowed one binding
	 * table per interpreter.
	 */
	Tcl_CreateCommand(interp, "qebind", QE_BindCmd,
		(ClientData) bindingTable, NULL);

	/* qeconfigure lets scripts configure bindings */
	Tcl_CreateObjCommand(interp, "qeconfigure", QE_ConfigureCmd,
		(ClientData) bindingTable, NULL);

	/* qegenerate lets scripts generate events */
	Tcl_CreateObjCommand(interp, "qegenerate", QE_GenerateCmd,
		(ClientData) bindingTable, NULL);

	/* qeinstall lets scripts add event types and details */
	Tcl_CreateObjCommand(interp, "qeinstall", QE_InstallObjCmd,
		(ClientData) bindingTable, NULL);

	initialized = 1;

	return TCL_OK;
}

void QE_InstallEvent(char *name, int type, QE_ExpandProc expand)
{
	Tcl_HashEntry *hPtr;
	EventInfo *valuePtr;
	int new;

	valuePtr = (EventInfo *) Tcl_Alloc(sizeof(EventInfo));
	valuePtr->name = name;
	valuePtr->type = type;
	valuePtr->expand = expand;

	hPtr = Tcl_CreateHashEntry(&eventTableByName, name, &new);
	Tcl_SetHashValue(hPtr, (ClientData) valuePtr);

	hPtr = Tcl_CreateHashEntry(&eventTableByType, (char *) type, &new);
	Tcl_SetHashValue(hPtr, (ClientData) valuePtr);
}

void QE_InstallDetail(char *name, int eventType, int code)
{
	Tcl_HashEntry *hPtr;
	Detail *valuePtr;
	EventInfo *eiPtr;
	PatternTableKey key;
	int new;

	/* Find the Event this detail goes with */
	hPtr = Tcl_FindHashEntry(&eventTableByType, (char *) eventType);
	if (hPtr == NULL) return;
	eiPtr = (EventInfo *) Tcl_GetHashValue(hPtr);	

	/* New Detail for detailTable */
	valuePtr = (Detail *) Tcl_Alloc(sizeof(Detail));
	valuePtr->name = Tk_GetUid(name);
	valuePtr->code = code;
	valuePtr->event = eiPtr;
#if ALLOW_INSTALL
	valuePtr->command = NULL;
#endif
	hPtr = Tcl_FindHashEntry(&detailTableByName, name);
	if (hPtr)
	{
		valuePtr->nextDetail = (Detail *) Tcl_GetHashValue(hPtr);
	}
	else
	{
		valuePtr->nextDetail = NULL;
	}

	/* Entry to find detail by name */
	hPtr = Tcl_CreateHashEntry(&detailTableByName, name, &new);
	Tcl_SetHashValue(hPtr, (ClientData) valuePtr);

	/* Entry to find detail by event type and detail code */
	key.type = eventType;
	key.detail = code;
	hPtr = Tcl_CreateHashEntry(&detailTableByType, (char *) &key, &new);
	Tcl_SetHashValue(hPtr, (ClientData) valuePtr);
}

Detail *FindDetail(int eventType, int code)
{
	PatternTableKey key;
	Tcl_HashEntry *hPtr;

	key.type = eventType;
	key.detail = code;
	hPtr = Tcl_FindHashEntry(&detailTableByType, (char *) &key);
	if (hPtr == NULL) return NULL;
	return (Detail *) Tcl_GetHashValue(hPtr);
}

QE_BindingTable QE_CreateBindingTable(Tcl_Interp *interp)
{
	BindingTable *bindPtr;

	bindPtr = (BindingTable *) Tcl_Alloc(sizeof(BindingTable));
	bindPtr->interp = interp;
	Tcl_InitHashTable(&bindPtr->patternTable,
		sizeof(PatternTableKey) / sizeof(int));
	Tcl_InitHashTable(&bindPtr->objectTable,
		sizeof(ObjectTableKey) / sizeof(int));

	return (QE_BindingTable) bindPtr;
}

int QE_CreateBinding(QE_BindingTable bindingTable,
		ClientData object, char *eventString, char *command, int append)
{
	BindingTable *bindPtr = (BindingTable *) bindingTable;
	BindValue *valuePtr;
	int new, length;
	char *cmdOld, *cmdNew;

	valuePtr = FindSequence(bindPtr->interp, &bindPtr->objectTable, object,
		eventString, 1, &new);
	if (valuePtr == NULL) return TCL_ERROR;

	/* created a new objectTable entry */
	if (new)
	{
		Tcl_HashEntry *hPtr;
		PatternTableKey key;

		key.type = valuePtr->type;
		key.detail = valuePtr->detail;
		hPtr = Tcl_CreateHashEntry(&bindPtr->patternTable, (char *) &key,
			&new);

		/*
		 * A patternTable entry exists for each different type/detail.
		 * The entry points to a BindValue which is the head of the list
		 * of BindValue's with this same type/detail, but for different
		 * objects.
		 */
		if (!new)
		{
			valuePtr->nextValue = (BindValue *) Tcl_GetHashValue(hPtr);
		}
		Tcl_SetHashValue(hPtr, (ClientData) valuePtr);
	}

	cmdOld = valuePtr->command;

	/* Append given command to any existing command */
	if (append && cmdOld)
	{
		length = strlen(cmdOld) + strlen(command) + 2;
		cmdNew = Tcl_Alloc((unsigned) length);
		(void) sprintf(cmdNew, "%s\n%s", cmdOld, command);

	/* Copy the given command */
	}
	else
	{
		cmdNew = (char *) Tcl_Alloc((unsigned) strlen(command) + 1);
		(void) strcpy(cmdNew, command);
	}

	/* Free the old command, if any */
	if (cmdOld) Tcl_Free(cmdOld);

	/* Save command associated with this binding */
	valuePtr->command = cmdNew;

	return TCL_OK;
}

int QE_DeleteBinding(QE_BindingTable bindingTable, ClientData object,
		char *eventString)
{
	Tcl_HashEntry *hPtr;
	BindingTable *bindPtr = (BindingTable *) bindingTable;
	BindValue *valuePtr, *listPtr;
	ObjectTableKey keyObj;
	PatternTableKey keyPat;

	valuePtr = FindSequence(bindPtr->interp, &bindPtr->objectTable, object,
		eventString, 0, NULL);
	if (valuePtr == NULL)
	{
		Tcl_ResetResult(bindPtr->interp);
		return TCL_OK;
	}

	/* Delete the objectTable entry */
	keyObj.type = valuePtr->type;
	keyObj.detail = valuePtr->detail;
	keyObj.object = object;
	hPtr = Tcl_FindHashEntry(&bindPtr->objectTable, (char *) &keyObj);
	if (hPtr == NULL) return TCL_ERROR; /* fatal error */
	Tcl_DeleteHashEntry(hPtr);

	/* Find the patternTable entry for this type/detail */
	keyPat.type = valuePtr->type;
	keyPat.detail = valuePtr->detail;
	hPtr = Tcl_FindHashEntry(&bindPtr->patternTable, (char *) &keyPat);
	if (hPtr == NULL) return TCL_ERROR; /* fatal error */

	/*
	 * Get the patternTable value. This is the head of a list of
	 * BindValue's that match the type/detail, but for different
	 * objects;
	 */
	listPtr = (BindValue *) Tcl_GetHashValue(hPtr);

	/* The deleted BindValue is the first */
	if (listPtr == valuePtr)
	{
		/* The deleted BindValue was the only one in the list */
		if (valuePtr->nextValue == NULL)
		{

			Tcl_DeleteHashEntry(hPtr);

		/* The next BindValue is the new head of the list */
		}
		else
		{
			Tcl_SetHashValue(hPtr, valuePtr->nextValue);
		}

	/* Look for the deleted BindValue in the list, and remove it */
	}
	else
	{
		while (1)
		{
			if (listPtr->nextValue == NULL) return TCL_ERROR; /* fatal */
			if (listPtr->nextValue == valuePtr)
			{
				listPtr->nextValue = valuePtr->nextValue;
				break;
			}
			listPtr = listPtr->nextValue;
		}
	}

	Tcl_Free((char *) valuePtr->command);
	Tcl_Free((char *) valuePtr);

	return TCL_OK;
}

char *QE_GetBinding(QE_BindingTable bindingTable, ClientData object,
		char *eventString)
{
	BindingTable *bindPtr = (BindingTable *) bindingTable;
	BindValue *valuePtr;
	
	valuePtr = FindSequence(bindPtr->interp, &bindPtr->objectTable, object,
		eventString, 0, NULL);
	if (valuePtr == NULL)
	{
		return NULL;
	}
	return valuePtr->command;
}

int QE_GetAllBindings(QE_BindingTable bindingTable, ClientData object)
{
	return TCL_OK;
}

static void ExpandPercents(ClientData object, char *command, QE_Event *eventPtr,
	EventInfo *eiPtr, Tcl_DString *result)
{
	char *string;

	Tcl_DStringSetLength(result, 0);
	
	while (1)
	{
		for (string = command; (*string != 0) && (*string != '%'); string++)
		{
		    /* Empty loop body. */
		}
		if (string != command)
		{
		    Tcl_DStringAppend(result, command, string - command);
		    command = string;
		}
		if (*command == 0)
		{
		    break;
		}

		/* Expand % here */
		(*eiPtr->expand)(command[1], object, eventPtr, result);

		command += 2;
	}
}

void BindEvent(BindingTable *bindPtr, QE_Event *eventPtr, int wantDetail,
	EventInfo *eiPtr, Detail *dPtr)
{
	Tcl_HashEntry *hPtr;
	BindValue *valuePtr;
	ObjectTableKey keyObj;
	PatternTableKey key;
	Tcl_DString command, savedResult;
	int code;

	/* Find the first BindValue for this event */
	key.type = eventPtr->type;
	key.detail = wantDetail ? eventPtr->detail : 0;
	hPtr = Tcl_FindHashEntry(&bindPtr->patternTable, (char *) &key);
	if (hPtr == NULL) return;

	Tcl_DStringInit(&command);

	/*
	 * As in Tk bindings, we expect that bindings may be invoked
	 * in the middle of Tcl commands. So we preserve the current
	 * interpreter result and restore it later.
	 */
	Tcl_DStringInit(&savedResult);
    Tcl_DStringGetResult(bindPtr->interp, &savedResult);

	for (valuePtr = (BindValue *) Tcl_GetHashValue(hPtr);
		valuePtr; valuePtr = valuePtr->nextValue)
	{
		if (wantDetail && valuePtr->detail)
		{
			keyObj.type = key.type;
			keyObj.detail = 0;
			keyObj.object = valuePtr->object;
			hPtr = Tcl_FindHashEntry(&bindPtr->objectTable, (char *) &keyObj);
			if (hPtr != NULL)
			{
				BindValue *value2Ptr;
				value2Ptr = (BindValue *) Tcl_GetHashValue(hPtr);
				value2Ptr->specific = 1;
			}
		}
		
		/*
		 * If a binding for a more-specific event exists for this object
		 * and event-type, and this is a binding for a less-specific
		 * event, then skip this binding, since the binding for the
		 * more-specific event was already invoked.
		 */
		else if (!wantDetail && valuePtr->specific)
		{
			valuePtr->specific = 0;
			continue;
		}

#if BIND_ACTIVE
		/* This binding isn't active */
		if (valuePtr->active == 0) continue;
#endif /* BIND_ACTIVE */

#if ALLOW_INSTALL
		/*
		 * Call a Tcl script to expand the percents.
		 */
		if (dPtr && (dPtr->command != NULL))
		{
			QE_ExpandProc expand = eiPtr->expand;
			ClientData data = eventPtr->clientData;
			struct {Tcl_Interp *i; Detail *d;} clientData;
			clientData.i = bindPtr->interp;
			clientData.d = dPtr;
			eiPtr->expand = Expand_Install;
			eventPtr->clientData = (ClientData) &clientData;
			ExpandPercents(valuePtr->object, valuePtr->command, eventPtr,
				eiPtr, &command);
			eiPtr->expand = expand;
			eventPtr->clientData = data;
		}
		else
#endif /* ALLOW_INSTALL */

		ExpandPercents(valuePtr->object, valuePtr->command, eventPtr, eiPtr,
			&command);

		code = Tcl_EvalEx(bindPtr->interp, Tcl_DStringValue(&command),
			Tcl_DStringLength(&command), TCL_EVAL_GLOBAL);

		if (code != TCL_OK)
		{
#if 0
			if (code == TCL_CONTINUE)
			{
				/* Nothing */
			}
			else if (code == TCL_BREAK)
			{
				break;
			}
			else
#endif
			{
				Tcl_AddErrorInfo(bindPtr->interp,
					"\n    (command bound to quasi-event)");
				Tcl_BackgroundError(bindPtr->interp);
				break;
			}
		}
	}

	/* Restore the interpreter result */
    Tcl_DStringResult(bindPtr->interp, &savedResult);
    
	Tcl_DStringFree(&command);
}

int QE_BindEvent(QE_BindingTable bindingTable, QE_Event *eventPtr)
{
	BindingTable *bindPtr = (BindingTable *) bindingTable;
	Detail *dPtr = NULL;
	EventInfo *eiPtr;
	Tcl_HashEntry *hPtr;

	/* Find the event by type */
	hPtr = Tcl_FindHashEntry(&eventTableByType, (char *) eventPtr->type);
	if (hPtr == NULL) return TCL_OK;
	eiPtr = (EventInfo *) Tcl_GetHashValue(hPtr);

	/* Find the detail */
	if (eventPtr->detail)
	{
		dPtr = FindDetail(eventPtr->type, eventPtr->detail);
		if (dPtr == NULL) return TCL_OK;
	}

	BindEvent(bindPtr, eventPtr, 1, eiPtr, dPtr);
	if (eventPtr->detail)
		BindEvent(bindPtr, eventPtr, 0, eiPtr, dPtr);

	return TCL_OK;
}

/*
 * The macro below is used to modify a "char" value (e.g. by casting
 * it to an unsigned character) so that it can be used safely with
 * macros such as isspace.
 */

#define UCHAR(c) ((unsigned char) (c))

static char *GetField(char *p, char *copy, int size)
{
	int ch = *p;

    while ((ch != '\0') && !isspace(ch) && (ch != '>')
	    && (ch != '-') && (size > 1))
	    {
		*copy = ch;
		p++;
		copy++;
		size--;
		ch = *p;
    }
    *copy = '\0';

    while ((*p == '-') || isspace(UCHAR(*p)))
    {
		p++;
    }
    return p;
}

#define FIELD_SIZE 48

static int ParseEventDescription(Tcl_Interp *interp, char *eventString,
		Pattern *patPtr, EventInfo **eventInfoPtr, Detail **detailPtr)
{
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	char *p;
	Tcl_HashEntry *hPtr;
	char field[FIELD_SIZE];
	EventInfo *eiPtr;
	Detail *dPtr;

	if (eventInfoPtr) *eventInfoPtr = NULL;
	if (detailPtr) *detailPtr = NULL;

	p = eventString;

	patPtr->type = -1;
	patPtr->detail = 0;

	/* First char must by opening < */
	if (*p != '<') return 0;
	p++;

	/* Event name (required)*/
	p = GetField(p, field, FIELD_SIZE);

	hPtr = Tcl_FindHashEntry(&eventTableByName, field);
	if (hPtr == NULL)
	{
		Tcl_AppendStringsToObj(resultPtr, "unknown event \"",
			field, "\"", NULL);
		return 0;
	}
	eiPtr = (EventInfo *) Tcl_GetHashValue(hPtr);
	patPtr->type = eiPtr->type;
	if (eventInfoPtr) *eventInfoPtr = eiPtr;

	/* Terminating > */
	if (*p == '>') return 1;

	/* Detail name (optional) */
	p = GetField(p, field, FIELD_SIZE);

	if (*field != '\0')
	{
		hPtr = Tcl_FindHashEntry(&detailTableByName, field);

		/* Find detail for the matching event */
		if (hPtr != NULL)
		{
			for (dPtr = (Detail *) Tcl_GetHashValue(hPtr);
				dPtr != NULL;
				dPtr = dPtr->nextDetail)
			{
				if (dPtr->event == eiPtr) break;
			}
			if (dPtr == NULL)
			{
				hPtr = NULL;
			}
		}
		if (hPtr == NULL)
		{
			Tcl_AppendStringsToObj(resultPtr, "unknown detail \"",
				field, "\"", NULL);
			return 0;
		}
		patPtr->detail = dPtr->code;
		if (detailPtr) *detailPtr = dPtr;
	}

	/* Terminating > */
	if (*p != '>')
	{
		Tcl_SetStringObj(resultPtr, "missing \">\" in binding", -1);
		return 0;
	}

	return 1;
}

static BindValue *FindSequence(Tcl_Interp *interp, Tcl_HashTable *objectTablePtr,
			ClientData object, char *eventString, int create, int *created)
{
	Tcl_HashEntry *hPtr;
	Pattern pats;
	ObjectTableKey key;
	BindValue *valuePtr;
	int count, new;

	if (created) (*created) = 0;

	/* Event description -> Pattern */
	count = ParseEventDescription(interp, eventString, &pats, NULL, NULL);
	if (count == 0) return NULL;

	/* type + detail + object -> BindValue */
	key.type = pats.type;
	key.detail = pats.detail;
	key.object = object;
	if (create)
	{
		hPtr = Tcl_CreateHashEntry(objectTablePtr, (char *) &key, &new);

		if (new)
		{
			valuePtr = (BindValue *) Tcl_Alloc(sizeof(BindValue));
			valuePtr->type = pats.type;
			valuePtr->detail = pats.detail;
			valuePtr->object = object;
			valuePtr->command = NULL;
			valuePtr->specific = 0;
			valuePtr->nextValue = NULL;
#if BIND_ACTIVE
			/* This binding is active */
			valuePtr->active = 1;
#endif /* BIND_ACTIVE */
			Tcl_SetHashValue(hPtr, (ClientData) valuePtr);
		}

		if (created) (*created) = new;
		return (BindValue *) Tcl_GetHashValue(hPtr);

	/* Look for existing objectTable entry */
	}
	else
	{
		hPtr = Tcl_FindHashEntry(objectTablePtr, (char *) &key);
		if (hPtr == NULL) return NULL;
		return (BindValue *) Tcl_GetHashValue(hPtr);
	}
}

void QE_ExpandDouble(double number, Tcl_DString *result)
{
	char numStorage[40];

	(void) sprintf(numStorage, "%f", number);
	QE_ExpandString(numStorage, result);
}

void QE_ExpandNumber(long number, Tcl_DString *result)
{
	char numStorage[40];

	(void) sprintf(numStorage, "%ld", number);
	QE_ExpandString(numStorage, result);
}

void QE_ExpandString(char *string, Tcl_DString *result)
{
	int length, spaceNeeded, cvtFlags;

	spaceNeeded = Tcl_ScanElement(string, &cvtFlags);
	length = Tcl_DStringLength(result);
	Tcl_DStringSetLength(result, length + spaceNeeded);
	spaceNeeded = Tcl_ConvertElement(string,
		Tcl_DStringValue(result) + length,
		cvtFlags | TCL_DONT_USE_BRACES);
	Tcl_DStringSetLength(result, length + spaceNeeded);
}

void QE_ExpandUnknown(char which, Tcl_DString *result)
{
	char string[2];

	(void) sprintf(string, "%c", which);
	QE_ExpandString(string, result);
}

int QE_BindCmd(ClientData clientData, Tcl_Interp *interp, int argc,
		char **argv)
{
	QE_BindingTable bindingTable = (QE_BindingTable) clientData;
    Tk_Window tkwin = Tk_MainWindow(interp);
	ClientData object;

	if ((argc < 2) || (argc > 4))
	{
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" window ?pattern? ?command?\"", (char *) NULL);
		return TCL_ERROR;
	}

    if (argv[1][0] == '.')
    {
		Tk_Window tkwin2;
		tkwin2 = Tk_NameToWindow(interp, argv[1], tkwin);
		if (tkwin2 == NULL)
		{
		    return TCL_ERROR;
		}
		object = (ClientData) Tk_PathName(tkwin2);
    }
    else
    {
		object = (ClientData) Tk_GetUid(argv[1]);
    }

    if (argc == 4)
    {
		int append = 0;

		if (argv[3][0] == 0)
		{
		    return QE_DeleteBinding(bindingTable, object, argv[2]);
		}
		if (argv[3][0] == '+')
		{
		    argv[3]++;
		    append = 1;
		}
		return QE_CreateBinding(bindingTable, object, argv[2], argv[3],
			append);

    }
    else if (argc == 3)
    {
		char *command;
	
		command = QE_GetBinding(bindingTable, object, argv[2]);
		if (command == NULL)
		{
		    Tcl_ResetResult(interp);
		    return TCL_OK;
		}
		Tcl_SetStringObj(Tcl_GetObjResult(interp), command, -1);

    }
    else
    {
		QE_GetAllBindings(bindingTable, object);
    }

    return TCL_OK;
}


/*
 * qegenerate -- Generate events from scripts.
 * Usage: qegenerate pattern ?field value ...?
 * Desciption: Scripts can generate "fake" quasi-events by providing
 * a quasi-event pattern and option field/value pairs.
 * TO DO: Let scripts define their own event details!
 */
 
typedef struct GenerateField {
	char which;
	char *string;
} GenerateField;

static GenerateField generateField[10];
static int generateCount;


/* Perform %-substitution using args passed to qegenerate */
static void Expand_Generate(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	Detail *dPtr;
	int i;

	for (i = 0; i < generateCount; i++)
	{
		if (which == generateField[i].which)
		{
			QE_ExpandString(generateField[i].string, result);
			return;
		}
	}

	switch (which)
	{
		case 'd': /* detail */
			dPtr = (Detail *) eventPtr->clientData;
			if (dPtr == NULL) break;
			QE_ExpandString(dPtr->name, result);
			break;

		case 'W': /* object */
			QE_ExpandString((char *) object, result);
			break;

		default: /* unknown */
			QE_ExpandUnknown(which, result);
			break;
	}
}

int
QE_GenerateCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	QE_BindingTable bindingTable = (QE_BindingTable) clientData;
	QE_Event fakeEvent;
	QE_ExpandProc expandProc;
	Tcl_HashEntry *hPtr;
	Tcl_Obj *CONST *objPtr;
	Detail *dPtr;
	EventInfo *eiPtr;
	GenerateField *fieldPtr;
	char *p, *t;
	Pattern pats;
	
    if (objc < 2)
    {
		Tcl_WrongNumArgs(interp, 1, objv, "pattern ?field value ...?");
		return TCL_ERROR;
    }

	p = Tcl_GetStringFromObj(objv[1], NULL);
	if (!ParseEventDescription(interp, p, &pats, NULL, &dPtr))
	{
		return TCL_ERROR;
	}
	fakeEvent.type = pats.type;
	fakeEvent.detail = pats.detail;
	fakeEvent.clientData = (ClientData) dPtr;
	
	objPtr = objv + 2;
	objc -= 2;

	fieldPtr = &generateField[0];
	generateCount = 0;
	
	while (objc > 1)
	{
		int length;
		t = Tcl_GetStringFromObj(objPtr[0], &length);
		if ((length != 2) || (t[0] != '-'))
		{
			char errorMsg[512];
			(void) sprintf(errorMsg, "invalid field \"%s\"", t);
			Tcl_SetResult(interp, errorMsg, TCL_VOLATILE);
			return TCL_ERROR;
		}
		fieldPtr->which = t[1];
		fieldPtr->string = Tcl_GetStringFromObj(objPtr[1], NULL);
		fieldPtr++;
		generateCount++;
		objPtr += 2;
		objc -= 2;
	}

	if (objc != 0)
	{
		Tcl_WrongNumArgs(interp, 2, objv, "pattern ?field value ...?");
		return TCL_ERROR;
	}

	/* Find the Event type */
	hPtr = Tcl_FindHashEntry(&eventTableByType, (char *) fakeEvent.type);
	eiPtr = (EventInfo *) Tcl_GetHashValue(hPtr);

	/*
	 * XXX Hack -- Swap in our own %-substitution routine. This routine
	 * uses the values the caller passed us.
	 */
	expandProc = eiPtr->expand;
	eiPtr->expand = Expand_Generate;
	if (QE_BindEvent(bindingTable, &fakeEvent) != TCL_OK)
	{
		eiPtr->expand = expandProc;
		return TCL_ERROR;
	}
	eiPtr->expand = expandProc;
	
	return TCL_OK;
}

#if BIND_ACTIVE

/* qeconfigure $win <Term-fresh> -active no */

int
QE_ConfigureCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tk_Window tkwin = Tk_MainWindow(interp);
	static char *configSwitch[] = {"-active", NULL};
	Tcl_Obj *CONST *objPtr;
	BindingTable *bindPtr = (BindingTable *) clientData;
	BindValue *valuePtr;
	char *t, *eventString;
	int index;
	ClientData object;
	
    if (objc < 3)
    {
		Tcl_WrongNumArgs(interp, 1, objv, "window pattern ?option? ?value? ?option value ...?");
		return TCL_ERROR;
    }

	t = Tcl_GetStringFromObj(objv[1], NULL);
	eventString = Tcl_GetStringFromObj(objv[2], NULL);

    if (t[0] == '.')
    {
		Tk_Window tkwin2;
		tkwin2 = Tk_NameToWindow(interp, t, tkwin);
		if (tkwin2 == NULL)
		{
		    return TCL_ERROR;
		}
		object = (ClientData) Tk_PathName(tkwin2);
    }
    else
    {
		object = (ClientData) Tk_GetUid(t);
    }

	valuePtr = FindSequence(bindPtr->interp, &bindPtr->objectTable, object,
		eventString, 0, NULL);
	if (valuePtr == NULL)
	{
		return TCL_ERROR;
	}

	objPtr = objv + 3;
	objc -= 3;

	while (objc > 1)
	{
	    if (Tcl_GetIndexFromObj(interp, objPtr[0], configSwitch,
			"option", 0, &index) != TCL_OK)
		{
			return TCL_ERROR;
	    }
		switch (index)
		{
			case 0: /* -active */
				if (Tcl_GetBooleanFromObj(interp, objPtr[1], &valuePtr->active)
					!= TCL_OK)
					{
					return TCL_ERROR;
				}
				break;
		}
		objPtr += 2;
		objc -= 2;
	}

	return TCL_OK;
}

#endif /* BIND_ACTIVE */

#if ALLOW_INSTALL

#if 0 /* comment */

qeinstall detail <Setting> show_icons 500 QEExpandCmd_Setting

proc QEExpandCmd_Setting {char object event detail} {

	switch -- $char {
		c {
			return [Setting $detail]
		}
		d {
			return $detail
		}
		W {
			return $object
		}
		default {
			return $char
		}
	}
}

#endif /* comment */

/* Perform %-substitution by calling a Tcl command */
static void Expand_Install(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result)
{
	struct {Tcl_Interp *i; Detail *d;} *clientData = eventPtr->clientData;
	Detail *dPtr = clientData->d;
	Tcl_Interp *interp = clientData->i;
	Tcl_DString command;
	Tcl_SavedResult state;

	if (dPtr->command != NULL)
	{
		Tcl_DStringInit(&command);
		Tcl_DStringAppend(&command, dPtr->command, -1);
		Tcl_DStringAppend(&command, " ", 1);
		Tcl_DStringAppend(&command, &which, 1);
		Tcl_DStringAppend(&command, " ", 1);
		Tcl_DStringAppend(&command, (char *) object, -1);
		Tcl_DStringAppend(&command, " ", 1);
		Tcl_DStringAppend(&command, dPtr->event->name, -1);
		Tcl_DStringAppend(&command, " ", 1);
		Tcl_DStringAppend(&command, dPtr->name, -1);

		Tcl_SaveResult(interp, &state);
		if (Tcl_EvalEx(interp, Tcl_DStringValue(&command),
			Tcl_DStringLength(&command), TCL_EVAL_GLOBAL) == TCL_OK)
		{
			QE_ExpandString(Tcl_GetStringFromObj(Tcl_GetObjResult(interp),
				NULL), result);
		}
		Tcl_RestoreResult(interp, &state);

		Tcl_DStringFree(&command);
	}
}

int QE_InstallObjCmd(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	static char *commandOption[] = {"detail", "event", NULL};
	int index;

	if (Tcl_GetIndexFromObj(interp, objv[1], commandOption, "option", 0,
		&index) != TCL_OK)
	{
		return TCL_ERROR;
	}
	switch (index)
	{
		case 0: /* detail */
		{
			char *eventString = Tcl_GetStringFromObj(objv[2], NULL);
			char *detailName, *command;
			int code, count, length;
			Detail *dPtr;
			EventInfo *eiPtr;
			Pattern pattern;
			Tcl_HashEntry *hPtr;

			/* Find the event type */
			count = ParseEventDescription(interp, eventString, &pattern,
				&eiPtr, NULL);
			if (count == 0) return TCL_ERROR;

			/* Get the detail name */
			detailName = Tcl_GetStringFromObj(objv[3], NULL);

			/* Verify that detailName not already defined */
			hPtr = Tcl_FindHashEntry(&detailTableByName, detailName);
			if (hPtr != NULL)
			{
				Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
					"detail \"", detailName, "\" already exists", NULL);
				return TCL_ERROR;
			}
			
			if (Tcl_GetIntFromObj(interp, objv[4], &code) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Verify that no detail with this name/code exists */
			dPtr = FindDetail(pattern.type, code);
			if (dPtr)
			{
				Tcl_SetStringObj(Tcl_GetObjResult(interp),
					"detail with that code already exists", -1);
				return TCL_ERROR;
			}

			/* Define the new detail */
			QE_InstallDetail(detailName, pattern.type, code);

			/* Get the detail we just defined */
			dPtr = FindDetail(pattern.type, code);
			if (dPtr == NULL)
				return TCL_ERROR;

			/* Set the Tcl command for this detail */
			command = Tcl_GetStringFromObj(objv[5], &length);
			dPtr->command = Tcl_Alloc(length + 1);
			(void) strcpy(dPtr->command, command);
			break;
		}
	}

	return TCL_OK;
}

#endif /* ALLOW_INSTALL */
