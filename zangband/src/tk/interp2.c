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

	static cptr cmdOptions[] = {"wild_name", NULL};
	enum {IDX_WILD_NAME} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

    if (objC < 2)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "option ?arg ...?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], cmdOptions, "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (option)
	{
	
		case IDX_WILD_NAME: /* wild_name */
			if (!character_dungeon)
			{
				/* Set the error */
				Tcl_SetStringObj(resultPtr, "dungeon has not been generated yet", -1);
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
					Tcl_SetStringObj(resultPtr, "Wilderness", -1);
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
	char *buffer;
	int i;
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
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp, objV[1], cmdOptions, "option", 0, 
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
				static cptr cmdOptions[] = {"-limit", "-tester", NULL};

				/* Get the sub-option */
				if (Tcl_GetIndexFromObj(interp, objV[i], cmdOptions, "option",
					0, &index) != TCL_OK)
				{
					return TCL_ERROR;
				}

				switch (index)
				{
					case 0: /* Limit */
					{
						if (Tcl_GetIntFromObj(interp, objV[i+1], &match_limit)
							!= TCL_OK)
						{
							return TCL_ERROR;
						}
						request_limit = 1;
						i += 2;
						break;
					}

					case 1: /* Tester */
					{
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

			C_MAKE(buffer, 5 * 1024L, char);
			length = angtk_describe_object(o_ptr, buffer, FALSE);
			Tcl_SetObjResult(interp, ExtToUtf_NewStringObj(buffer, length));
			FREE(buffer);
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
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp, objV[1], cmdOptions, "option", 0, 
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
				if (Tcl_GetIndexFromObj(interp, objV[2], abortSwitch,
					"switch", 0, &index) != TCL_OK)
				{
					return TCL_ERROR;
				}
				confirm = 0;
			}
			if (confirm && game_in_progress && character_generated)
			{
				int result;

				result = Tcl_EvalEx(g_interp,
					"tk_messageBox -icon warning -type okcancel -message \"Your character will not be saved!\" -title \"Quit Without Saving\"",
					-1, TCL_EVAL_GLOBAL);
				if (result == TCL_OK)
				{
					cptr s = Tcl_GetStringResult(g_interp);
					if (!strcmp(s, "cancel")) break;
				}
			}
			quit(NULL);
			break;
		}
		
		case IDX_TKDIR: /* Tk directory for game .tcl files */
		{
		 	/* Return the current directory path */
			ExtToUtf_SetResult(interp, ANGBAND_DIR_TK);
			break;
		}
		
		case IDX_VERSION: /* version */
			Tcl_SetStringObj(resultPtr, format("%d.%d.%d", FAKE_VER_MAJOR,
				FAKE_VER_MINOR, FAKE_VER_PATCH), -1);
			break;
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
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "option ?arg ...?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], cmdOptions, "option", 0, 
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
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "option ?arg ...?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], cmdOption, "option", 0, 
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
			Tcl_SetStringObj(resultPtr, keyword_term_color[attr], -1);
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
			ExtToUtf_SetResult(interp, message_str(i));
			break;
	}

	return TCL_OK;
}


