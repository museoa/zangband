/* File: interp2.c */

/* Purpose: more tcl commands */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include "icon.h"

/*
 *--------------------------------------------------------------
 *
 * objcmd_cave --
 *
 *	Implements the "cave" script command.
 * 	Syntax:
 *		cave blocked y x -- can player move there
 *		cave examine y x -- describe what's there
 *		cave height -- height of cave
 *		cave width -- width of cave
 *		cave info -- get info about a grid
 *		cave wild_name -- get name of wilderness area
 *
 *--------------------------------------------------------------
 */

int
objcmd_cave(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"blocked", "wild_name", NULL};
	enum {IDX_BLOCKED, IDX_WILD_NAME} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int y, x;
	int blocked;

    if (objC < 2)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (option)
	{
		case IDX_BLOCKED: /* blocked */
			if (!character_dungeon) goto error;
			if (objC != 4)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "y x");
				return TCL_ERROR;
			}
			if (Tcl_GetIntFromObj(interp, objV[2], &y) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (Tcl_GetIntFromObj(interp, objV[3], &x) != TCL_OK)
			{
				return TCL_ERROR;
			}
	
			if (!in_bounds2(x, y))
			{
				/* Set the error */
				Tcl_AppendStringsToObj(resultPtr, (char *) "location ",
					format("y=%d,x=%d", y, x), " is not in bounds", NULL);
			
				goto error;
			}
			
			blocked = !player_test_feature(y, x, 0);

			Tcl_SetBooleanObj(resultPtr, blocked);
			break;
	
		case IDX_WILD_NAME: /* wild_name */
			if (!character_dungeon)
			{
				/* Set the error */
				Tcl_SetStringObj(resultPtr, (char *) "dungeon has not been generated yet", -1);
				goto error;
			}
			if (!p_ptr->depth)
			{
				if (p_ptr->place_num)
				{
					ExtToUtf_SetResult(interp, place[p_ptr->place_num].name);
				}
				else
				{
					Tcl_SetStringObj(resultPtr, (char *) "Wilderness", -1);
				}
			}
			break;
	}

	/* Success */
	return TCL_OK;
	
error:

	/* Failure */
	return TCL_ERROR;
}


/*
 *--------------------------------------------------------------
 *
 * objcmd_floor --
 *
 *	Implements the "floor" script command.
 * 	Syntax:
 *
 *		floor find SEARCHCOMMAND ?arg arg ...?
 * 			Return list of indexes of matching objects
 *		floor info INDEX arrayName
 *		floor memory INDEX
 *
 *--------------------------------------------------------------
 */

int
objcmd_floor(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"find", "memory",
		NULL};
	enum {IDX_FIND, IDX_MEMORY} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	Tcl_Obj *listObjPtr;
	char *buffer, *t;
	int i, tval;
	long length;
	object_type *o_ptr;
	int fy, fx;

	/* Default to finding all matches */
	int request_limit = 0, match_limit = 0, cnt = 0;

	/* Default to ignoring item_tester_okay() hook */
	int request_tester = 0, match_tester = 0;

	/* Default to no restriction on tval */
	int request_tval = 0, match_tval[10], tval_cnt = 0;

	if (objC < 2)
	{
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* XXX Hack -- Determine the location to display */
	fy = p_ptr->py;
	fx = p_ptr->px;

	switch (option)
	{
		case IDX_FIND: /* find */
		{
			bool (*old_tester_hook)(const object_type *) = item_tester_hook;
			bool (*temp_tester_hook)(const object_type *) = NULL;
			
			/* Scan arguments for options */
			for (i = 2; i < objC; )
			{
				static cptr cmdOptions[] = {"-limit", "-tester",
					"-tval", NULL};

				/* Get the sub-option */
				if (Tcl_GetIndexFromObj(interp, objV[i], (char **) cmdOptions, (char *) "option",
					0, &index) != TCL_OK)
				{
					return TCL_ERROR;
				}

				switch (index)
				{
					case 0: /* Limit */
						if (Tcl_GetIntFromObj(interp, objV[i+1], &match_limit)
							!= TCL_OK)
						{
							return TCL_ERROR;
						}
						request_limit = 1;
						i += 2;
						break;

					case 1: /* Tester */
						if (Tcl_GetBooleanFromObj(interp, objV[i+1],
							&match_tester) != TCL_OK)
						{
							return TCL_ERROR;
						}
						request_tester = 1;
						i += 2;
						break;
		
					case 2: /* Tval */
						t = Tcl_GetStringFromObj(objV[i+1], NULL);
						if (angtk_tval_const(&tval, t) != TCL_OK)
						{
							return TCL_ERROR;
						}
						match_tval[tval_cnt++] = tval;
						request_tval = 1;
						i += 2;
						break;
				}
			}

			if (temp_tester_hook) item_tester_hook = temp_tester_hook;

			/* Return a list of o_list[] indexes */
			listObjPtr = Tcl_NewListObj(0, NULL);

			/* Scan all objects in the grid */
			OBJ_ITT_START (area(fx, fy)->o_idx, o_ptr)
			{
				if (request_tester && match_tester)
				{
					/* Accept TV_GOLD if no tester */
					if ((o_ptr->tval == TV_GOLD) && !item_tester_hook &&
						!item_tester_tval)
					{
						/* Nothing */
					}
					else if (!item_tester_okay(o_ptr)) 
					{
						continue;
					}
				}
				if (request_tval)
				{
					for (i = 0; i < tval_cnt; i++)
					{
						if (match_tval[0] == o_ptr->tval) break;
					}
					if (i == tval_cnt) continue;
				}
	
				/* Found a match */
				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewIntObj(_this_o_idx));
	
				/* Return x matches */
				if (request_limit && (++cnt >= match_limit)) break;
			}
			OBJ_ITT_END;

			/* XXX Hack -- Restore the hook */
			item_tester_hook = old_tester_hook;

			/* Return a list of o_list[] indexes */
			Tcl_SetObjResult(interp, listObjPtr);
			break;
		}

		case IDX_MEMORY: /* memory */

			if (Tcl_GetIntFromObj(interp, objV[2], &i) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (i <= 0 || i > o_max) goto bad_index;

			/* Get item info */
			o_ptr = &o_list[i];

			/* Illegal */
			if (!o_ptr->k_idx || (o_ptr->iy != fy) || (o_ptr->ix != fx))
			{
				goto bad_index;
			}

			buffer = Tcl_Alloc(5 * 1024L);
			length = angtk_describe_object(o_ptr, buffer, FALSE);
			Tcl_SetObjResult(interp, ExtToUtf_NewStringObj(buffer, length));
			Tcl_Free(buffer);
			break;
	}

	return TCL_OK;

bad_index:
	Tcl_SetStringObj(resultPtr, format("bad floor index \"%d\"", i), -1);
	return TCL_ERROR;
}


/*
 *--------------------------------------------------------------
 *
 * objcmd_game --
 *
 *	Implements the "game" script command.
 * 	Syntax:
 *		game abort ?confirm? -- Quit without saving
 *		game directory -- Get a directory pathname
 *		game keymap_dump -- Dump a keymap file
 *		game new -- Start a new game
 *		game open -- Open a save file
 *		game process_pref_file -- Process a preference file
 *		game quit -- Quit with save
 *
 *--------------------------------------------------------------
 */


/* List of directory keywords */
cptr keyword_path[] = {
	"ANGBAND_DIR_ROOT",
	"ANGBAND_DIR_USER",
	"ANGBAND_DIR_TK",
	NULL
};


int
objcmd_game(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"abort", "tkdir" "version", NULL};
	enum {IDX_ABORT, IDX_TKDIR, IDX_VERSION} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	if (objC < 2)
	{
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
	}

	switch (option)
	{
		case IDX_ABORT: /* abort */
		{
			int confirm = 1;
			if (objC == 3)
			{
				static cptr abortSwitch[] = {"-noask", NULL};
				if (Tcl_GetIndexFromObj(interp, objV[2], (char **) abortSwitch,
					(char *) "switch", 0, &index) != TCL_OK)
				{
					return TCL_ERROR;
				}
				confirm = 0;
			}
			if (confirm && game_in_progress && character_generated)
			{
				int result;

				result = Tcl_EvalEx(g_interp,
					(char *) "tk_messageBox -icon warning -type okcancel -message \"Your character will not be saved!\" -title \"Quit Without Saving\"",
					-1, TCL_EVAL_GLOBAL);
				if (result == TCL_OK)
				{
					char *s = Tcl_GetStringResult(g_interp);
					if (!strcmp(s, "cancel")) break;
				}
			}
			quit(NULL);
			break;
		}
		
		case IDX_TKDIR: /* Tk directory for game .tcl files */
		{
		 	/* Return the current directory path */
			ExtToUtf_SetResult(interp, (char *) ANGBAND_DIR_TK);
			break;
		}
		
		case IDX_VERSION: /* version */
			Tcl_SetStringObj(resultPtr, format("%d.%d.%d", FAKE_VER_MAJOR,
				FAKE_VER_MINOR, FAKE_VER_PATCH), -1);
			break;
	}

	return TCL_OK;
}


/*
 *--------------------------------------------------------------
 *
 * objcmd_info --
 *
 *	Implements the "info" script command.
 * 	Syntax:
 *		info class_name -- get list of class names
 *		info gender_name -- get list of gender names
 *		info race_name -- get list of race names
 *		info tval -- get list of tval names
 *
 *--------------------------------------------------------------
 */

int
objcmd_info(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"class_name", "gender_name", "race_name",
		"tval", "flavor_color", "term_attr",
		NULL};
	enum {IDX_CLASS_NAME, IDX_GENDER_NAME, IDX_RACE_NAME,
		IDX_TVAL, IDX_FLAVOR_COLOR, IDX_TERM_ATTR
		} option;
	int index;

	char **names = NULL;
	int i, subIndex = -1, nameCount = 0;
	Tcl_Obj *listObjPtr;

    if (objC < 2)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?index?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	if (option == IDX_FLAVOR_COLOR) /* flavor_color */
	{
		static cptr flavorName[] = {"amulet", "mushroom", "potion",
			"ring", "rod", "staff", "wand", NULL};
		if (objC != 3)
		{
			Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "flavor");
			return TCL_ERROR;
		}
	    if (Tcl_GetIndexFromObj(interp, objV[2], (char **) flavorName, (char *) "flavor", 0, 
			&index) != TCL_OK)
		{
			return TCL_ERROR;
	    }
		listObjPtr = Tcl_NewListObj(0, NULL);
		for (i = 0; i < g_flavor[index].count; i++)
		{
			int c = g_flavor[index].color[i];
			Tcl_ListObjAppendElement(interp, listObjPtr,
				Tcl_NewStringObj((char *) keyword_term_color[c], -1));
		}
		Tcl_SetObjResult(interp, listObjPtr);
		return TCL_OK;
	}

	if (objC == 3)
	{
		if (Tcl_GetIntFromObj(interp, objV[2], &subIndex) != TCL_OK)
		{
			return TCL_ERROR;
		}
		if (subIndex < 0) subIndex = -1;
	}
	
	switch (option)
	{
		case IDX_CLASS_NAME: /* class_name */
			names = (char **) keyword_class;
			nameCount = MAX_CLASS;
			break;

		case IDX_GENDER_NAME: /* gender_name */
			names = (char **) keyword_gender;
			nameCount = MAX_SEXES;
			break;

		case IDX_RACE_NAME: /* race_name */
			names = (char **) keyword_race;
			nameCount = MAX_RACES;
			break;

		case IDX_TVAL: /* tval */
			listObjPtr = Tcl_NewListObj(0, NULL);
			for (i = 0; g_tval[i].key; i++)
			{
				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewStringObj(g_tval[i].key, -1));
			}
			Tcl_SetObjResult(interp, listObjPtr);
			return TCL_OK;
				
		case IDX_TERM_ATTR: /* term_attr */
			names = (char **) keyword_term_color;
			nameCount = 16;
			break;

		/* Prevent compiler warnings */
		case IDX_FLAVOR_COLOR:
			break;
	}

	if (subIndex != -1)
	{
		if ((subIndex < 0) || (subIndex >= nameCount))
		{
			return TCL_ERROR;
		}
		Tcl_SetStringObj(Tcl_GetObjResult(interp), names[subIndex], -1);
	}
	else
	{
		listObjPtr = Tcl_NewListObj(0, NULL);
		for (i = 0; i < nameCount; i++)
		{
			Tcl_ListObjAppendElement(interp, listObjPtr,
				Tcl_NewStringObj(names[i], -1));
		}
		Tcl_SetObjResult(interp, listObjPtr);
	}
	
	return TCL_OK;
}

/* init_icons $size $depth */
int
objcmd_init_icons(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	int size, depth;
	
	/* Hack - ignore parameter */
	(void) objc;

	if (g_icon_size)
	{
		Tcl_SetResult(interp, (char *) "icons were already initialized", TCL_VOLATILE);
		return TCL_ERROR;
	}

	/* Get the size */
	if (Tcl_GetIntFromObj(interp, objV[1], &size) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Get the depth */
	if (Tcl_GetIntFromObj(interp, objV[2], &depth) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Initialize (quit on failure) */
	init_icons(size, depth);

	return TCL_OK;
}

/* Strings returned by "inkey_flags" command, indexed by INKEY_XXX defines. */
cptr inkey_to_str[] = {"", "INKEY_CMD", "INKEY_DIR", "INKEY_DISTURB",
	"INKEY_ITEM", "INKEY_ITEM_STORE", "INKEY_MORE", "INKEY_SPELL",
	"INKEY_TARGET", "INKEY_POWER", "INKEY_CMD_PET",
	NULL};

/* (inkey) flags */
int
objcmd_inkey_flags(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	/* Hack - ignore parameters */
	(void) objc;
	(void) objv;
	(void) clientData;

	Tcl_SetResult(interp, (char *) inkey_to_str[inkey_flags], TCL_VOLATILE);
	return TCL_OK;
}


/*
 *--------------------------------------------------------------
 *
 * objcmd_inventory --
 *
 *	Implements the "inventory" script command.
 * 	Syntax:
 *
 *		inventory count
 *			Return number of inventory items carried
 *
 *		inventory find SEARCHCOMMAND ?arg arg ...?
 * 			Return list of indexes of matching objects
 *
 *		inventory info INDEX VARNAME
 *			Return info about specific object
 *
 *		inventory memory INDEX
 *			Return memory about about specific object
 *
 *		inventory total_weight
 *			Return total weight carried
 *
 *		inventory weight_limit
 *			Return carrying capacity in 10ths of pounds
 *
 *--------------------------------------------------------------
 */

int
objcmd_inventory(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {
		"total_weight", "weight_limit",
		NULL};
	enum {
		IDX_TOTAL_WEIGHT, IDX_WEIGHT_LIMIT
	} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int i;

    if (objC < 2)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (option)
	{
		case IDX_TOTAL_WEIGHT: /* total_weight */
			Tcl_SetIntObj(resultPtr, p_ptr->total_weight);
			break;

		case IDX_WEIGHT_LIMIT: /* weight_limit */

			/* Max carrying capacity in 10ths of pounds */
			i = adj_str_wgt[p_ptr->stat_ind[A_STR]] * 100;
			Tcl_SetIntObj(resultPtr, i);
			break;
	}

	return TCL_OK;
}

/* keycount */
int
objcmd_keycount(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	/* Hack - ignore parameters */
	(void) objc;
	(void) objv;
	(void) clientData;

	Tcl_SetObjResult(interp,
		Tcl_NewBooleanObj(Term->key_head != Term->key_tail));
	return TCL_OK;
}

/* keypress $string */
int
objcmd_keypress(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	char *t;
	int i;

	/* Hack - ignore parameters */
	(void) objc;
	(void) interp;

	t = Tcl_GetStringFromObj(objV[1], NULL);
	for (i = 0; t[i]; i++)
	{
		Term_keypress(t[i]);
	}
	return TCL_OK;
}


/*
 *--------------------------------------------------------------
 *
 * objcmd_message --
 *
 *	Implements the "message" script command.
 * 	Syntax:
 *		message color -- Return color for message $index
 *		message count -- Return number of saved messages
 *		message get $index -- Return most-recent number of messages
 *		message sound $index -- Return sound for message $index
 *
 *--------------------------------------------------------------
 */

int
objcmd_message(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOption[] = {"color", "count", "get", NULL};
	enum {IDX_COLOR, IDX_COUNT, IDX_GET} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int i, k;
	byte attr;

    if (objC < 2)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOption, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	if (!character_generated)
	{
		Tcl_AppendStringsToObj(resultPtr,
			"character has not been generated yet", NULL);
		return TCL_ERROR;
	}

	switch (option)
	{
		case IDX_COLOR: /* color */
			if (Tcl_GetIntFromObj(interp, objV[2], &i) != TCL_OK)
			{
				return TCL_ERROR;
			}
			k = message_num();
			if (i < 0 || i >= k)
			{
				Tcl_SetStringObj(resultPtr, format("invalid message index \"%d\": "
					"must be from 0 to %d", i, k - 1), -1);
				return TCL_ERROR;
			}
			attr = TERM_WHITE;
			Tcl_SetStringObj(resultPtr, (char *) keyword_term_color[attr], -1);
			break;
		
		case IDX_COUNT: /* count */
			Tcl_SetIntObj(resultPtr, message_num());
			break;

		case IDX_GET: /* get */
			if (Tcl_GetIntFromObj(interp, objV[2], &i) != TCL_OK)
			{
				return TCL_ERROR;
			}
			k = message_num();
			if (i < 0 || i >= k)
			{
				Tcl_SetStringObj(resultPtr, format("invalid message index \"%d\": "
					"must be from 0 to %d", i, k - 1), -1);
				return TCL_ERROR;
			}
			ExtToUtf_SetResult(interp, (char *) message_str(i));
			break;
	}

	return TCL_OK;
}


