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
#include "struct.h"
#include "icon.h"


/*
 * Write the user's inscription for the given object into buf.
 */
static char *object_inscription(object_type *o_ptr, char *buf)
{
	/* No inscription yet */
	buf[0] = '\0';

	/* Use the user's inscription if available */
	if (o_ptr->inscription)
	{
		(void) strcpy(buf, quark_str(o_ptr->inscription));
	}

	return buf;
}

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

	static cptr cmdOptions[] = {"assign", "blocked",
		"info", "in_bounds", "in_bounds_fully", "exists", "shape", "day",
		"wild_name",
		NULL};
	enum {IDX_ASSIGN, IDX_BLOCKED,
		IDX_INFO, IDX_IN_BOUNDS, IDX_IN_BOUNDS_FULLY, IDX_EXISTS, IDX_SHAPE,
		IDX_DAY , IDX_WILD_NAME
		} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int y, x;
	int blocked;
	char *varName;

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
		case IDX_ASSIGN: /* assign */
		{
			static cptr opt[] = {"icon1", "icon2", "icon3", "icon4", NULL};
			int plane;
			t_assign assign;
			char *t;

			if (objC < 5 || objC > 6)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv,
					(char *) "y x plane ?assign?");
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
			if (!in_bounds2(y, x))
			{
				goto bad_location;
			}
			if (Tcl_GetIndexFromObj(interp, objV[4], (char **) opt, (char *) "plane", 0, 
				&plane) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (objC == 5)
			{
				char buf[128];
				assign = g_icon_map[plane][y][x];
				(void) assign_print(buf, &assign);
				Tcl_SetResult(interp, buf, TCL_VOLATILE);
				break;
			}
			t = Tcl_GetString(objV[5]);
			if (assign_parse(interp, &assign, t) != TCL_OK)
			{
				return TCL_ERROR;
			}
			g_icon_map[plane][y][x] = assign;
			break;
		}

		case IDX_BLOCKED: /* blocked */
			if (!character_dungeon) goto not_exists;
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
	
			if (!in_bounds2(y, x))
			{
				goto bad_location;
			}
			
			blocked = !player_test_feature(y, x, 0);

			Tcl_SetBooleanObj(resultPtr, blocked);
			break;

		case IDX_INFO: /* info */
			if (!character_dungeon) goto not_exists;
			if (objC != 5)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "y x arrayName");
				return TCL_ERROR;
			}

			/* Get the y coordinate */
			if (Tcl_GetIntFromObj(interp, objV[2], &y) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the x coordinate */
			if (Tcl_GetIntFromObj(interp, objV[3], &x) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Validate coordinates */
			if (!in_bounds2(y, x))
			{
				goto bad_location;
			}

			/* Get the array variable name to dump results in */
			varName = Tcl_GetStringFromObj(objV[4], NULL);

			if (SetArrayValueLong(varName, "f_idx", area(x, y)->feat) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (SetArrayValueLong(varName, "m_idx", area(x, y)->m_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (SetArrayValueLong(varName, "o_idx", area(x, y)->o_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}
			break;

		case IDX_IN_BOUNDS: /* in_bounds */
		case IDX_IN_BOUNDS_FULLY: /* in_bounds_fully */
			if (!character_dungeon) goto not_exists;
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

			if (option == IDX_IN_BOUNDS)
				Tcl_SetBooleanObj(resultPtr, in_bounds2(y, x));
			else
				Tcl_SetBooleanObj(resultPtr, in_bounds(y, x));
			break;

		case IDX_EXISTS: /* exists */
			Tcl_SetBooleanObj(resultPtr, character_dungeon);
			break;

		case IDX_SHAPE: /* shape */
		{
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
			Tcl_SetStringObj(resultPtr, (char *) keyword_wall[g_grid[y][x].shape], -1);
			break;
		}

		case IDX_DAY: /* day */
		{
			Tcl_SetStringObj(resultPtr, (char *) ((!p_ptr->depth && g_daytime) ?
				"day" : "night"), -1);
			break;
		}
	
		case IDX_WILD_NAME: /* wild_name */
			if (!character_dungeon) goto not_exists;
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

bad_location:

	/* Set the error */
	Tcl_AppendStringsToObj(resultPtr, (char *) "location ",
		format("y=%d,x=%d", y, x), " is not in bounds", NULL);
	goto error;

not_exists:

	/* Set the error */
	Tcl_SetStringObj(resultPtr, (char *) "dungeon has not been generated yet", -1);

error:

	/* Failure */
	return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * objcmd_equipment --
 *
 *	Implements the "equipment" script command.
 * 	Syntax:
 *		equipment find SEARCHCOMMAND ...
 *		equipment info SLOT
 *		equipment memory SLOT
 *
 *--------------------------------------------------------------
 */

int
objcmd_equipment(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"inscription", NULL};
	enum {IDX_INSCRIPTION} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int i_idx;
	object_type *o_ptr;
	char buf[80], *t;

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

	if (!character_generated)
	{
		Tcl_AppendStringsToObj(resultPtr,
			(char *) "character has not been generated yet", NULL);
		return TCL_ERROR;
	}

	switch (option)
	{
		case IDX_INSCRIPTION: /* inscription */

			if (objC < 3)
			{
				/* Set the error */
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "index ?string?");

				/* Failure */
				return TCL_ERROR;
			}

			/* Get a numerical index or slot name */
			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				Tcl_ResetResult(interp);
				if (Tcl_GetIndexFromObj(interp, objV[2],
					(char **) keyword_slot, (char *) "slot", 0, &i_idx) != TCL_OK)
				{
					return TCL_ERROR;
				}
			}

			/* Verify i_idx */
			if ((i_idx < 0) || (i_idx >= EQUIP_MAX))
			{
				goto bad_index;
			}

			/* Get item info */
			o_ptr = &p_ptr->equipment[i_idx];

			/* Require a real item */
			if (!o_ptr->k_idx)
			{
				/* Set the error */
				Tcl_SetStringObj(resultPtr,
					format("equipment item %d is empty", i_idx), -1);

				/* Failure */
				return TCL_ERROR;
			}
			
			/* Set the inscription. */
			if (objC == 4)
			{
				/* Get the new inscription */
				t = Tcl_GetStringFromObj(objV[3], NULL);

				if (strlen(t))
				{
					/* Save the inscription */
					o_ptr->inscription = quark_add(t);
				}
				else
				{
					/* Clear the inscription */
					o_ptr->inscription =  0;
				}
		
				/* Combine the pack */
				p_ptr->notice |= (PN_COMBINE);
		
				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP);

				/* Done */
				break;
			}

			/* Get the current inscription, if any */
			(void) object_inscription(o_ptr, buf);

			/* Return the current inscription */
			ExtToUtf_SetResult(interp, buf);

			/* Done */
			break;
	}

	/* Success */
	return TCL_OK;

bad_index:

	/* Set the error */
	Tcl_AppendStringsToObj(resultPtr, "bad equipment index \"",
		Tcl_GetStringFromObj(objV[2], NULL),
		"\": must be between 0 and ",
		format("%d", EQUIP_MAX - 1), NULL);

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

	static cptr cmdOptions[] = {"find", "memory", "inscription",
		NULL};
	enum {IDX_FIND, IDX_MEMORY, IDX_INSCRIPTION} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	Tcl_Obj *listObjPtr;
	char buf[80], *buffer, *t;
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

		case IDX_INSCRIPTION: /* inscription */

			if (objC < 3)
			{
				/* Set the error */
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "index ?string?");

				/* Failure */
				return TCL_ERROR;
			}

			/* Get the item index */
			if (Tcl_GetIntFromObj(interp, objV[2], &i) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Verify i_idx */
			if (i <= 0 || i > o_max) goto bad_index;

			/* Get item info */
			o_ptr = &o_list[i];

			/* Illegal */
			if (!o_ptr->k_idx || (o_ptr->iy != fy) || (o_ptr->ix != fx))
			{
				goto bad_index;
			}

			/* Set the inscription. */
			if (objC == 4)
			{
				/* Get the new inscription */
				t = Tcl_GetStringFromObj(objV[3], NULL);

				if (strlen(t))
				{
					/* Save the inscription */
					o_ptr->inscription = quark_add(t);
				}
				else
				{
					/* Clear the inscription */
					o_ptr->inscription = 0;
				}
		
				/* Combine the pack */
				p_ptr->notice |= (PN_COMBINE);
		
				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP);

				/* Done */
				break;
			}

			/* Get the current inscription, if any */
			(void) object_inscription(o_ptr, buf);

			/* Return the current inscription */
			ExtToUtf_SetResult(interp, buf);

			/* Done */
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
 *		game macro_dump -- Dump a macro file
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

	static cptr cmdOptions[] = {"abort", "tkdir"
		"macro_dump", "new", "open", "process_pref_file", "quit",
		"keymap_dump", "version",
		"savefile", NULL};
	enum {IDX_ABORT, IDX_TKDIR,
		IDX_MACRO_DUMP, IDX_NEW, IDX_OPEN, IDX_PROCESS_PREF_FILE, IDX_QUIT,
		IDX_KEYMAP_DUMP, IDX_VERSION,
		IDX_SAVEFILE} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	char *t, *utfString, *extString;
	Tcl_DString utfDString, extDString;
	Tcl_Channel c;

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
		
					
		case IDX_MACRO_DUMP: /* macro_dump */
			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "filename");
				return TCL_ERROR;
			}

			/* Get the file path */
			t = Tcl_GetStringFromObj(objV[2], NULL);

			if (t[0] && t[0] != ' ')
			{
				/* Translate the file path */
				extString = UtfToExt_TranslateFileName(interp, t, &extDString);
				if (extString == NULL) return TCL_ERROR;

				/* Dump the macros */
				if (macro_dump(extString) == -1)
				{
					/* Set the error */
					Tcl_AppendStringsToObj(resultPtr,
						"error writing macro file \"", t, "\"",
						NULL);

					/* Clean up */
					Tcl_DStringFree(&extDString);

					/* Failure */
					return TCL_ERROR;
				}

				/* Clean up */
				Tcl_DStringFree(&extDString);
			}
			break;

		case IDX_NEW: /* new */
			if (!g_initialized)
			{
				Tcl_SetStringObj(resultPtr, (char *) "game is not initialized", -1);
				return TCL_ERROR;
			}
			if (game_in_progress)
			{
				Tcl_SetStringObj(resultPtr, (char *) "game is in progress", -1);
				return TCL_ERROR;
			}
			game_in_progress = 1;
			play_game(TRUE);
			quit(NULL);
			break;

		case IDX_OPEN: /* open */
			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "filename");
				return TCL_ERROR;
			}
			if (!g_initialized)
			{
				Tcl_SetStringObj(resultPtr, (char *) "game is not initialized", -1);
				return TCL_ERROR;
			}
			if (game_in_progress)
			{
				Tcl_SetStringObj(resultPtr, (char *) "game is in progress", -1);
				return TCL_ERROR;
			}

			/* Get the file path */
			t = Tcl_GetString(objV[2]);

			/* Translate the file name */
			utfString = Tcl_TranslateFileName(interp, t, &utfDString);
			if (utfString == NULL)
			{
				/* Note: Tcl_DStringFree() is called for us */
				return TCL_ERROR;
			}

			/* Test that the file is readable */
			c = Tcl_OpenFileChannel(interp, utfString, (char *) "r", 0);
			if (c == (Tcl_Channel) NULL)
			{
				/* Clean up */
				Tcl_DStringFree(&utfDString);

				/* Failure */
				return TCL_ERROR;
			}

			/* Convert UTF8 to native encoding */
			Tcl_UtfToExternalDString(NULL, utfString, -1, &extDString);
			extString = Tcl_DStringValue(&extDString);

			/* Set savefile */
			(void) strcpy(savefile, extString);

			/* Clean up */
			Tcl_DStringFree(&extDString);
			Tcl_DStringFree(&utfDString);
			Tcl_Close(NULL, c);

			/* Play the game (never returns) */
			game_in_progress = 1;
			play_game(FALSE);
			quit(NULL);
			break;

		case IDX_PROCESS_PREF_FILE: /* process_pref_file */
			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "filename");
				return TCL_ERROR;
			}

			/* Get the file path */
			t = Tcl_GetString(objV[2]);

			/* */
			if (t[0] && t[0] != ' ')
			{
				/* Translate the file path */
				extString = UtfToExt_TranslateFileName(interp, t, &extDString);
				if (extString == NULL) return TCL_ERROR;

				/* Read the preferences */
				if (process_pref_file(extString) == -1)
				{
					/* Set the error */
					Tcl_AppendStringsToObj(resultPtr,
						"error processing pref file \"", t, "\"",
						NULL);

					/* Clean up */
					Tcl_DStringFree(&extDString);

					/* Failure */
					return TCL_ERROR;
				}

				/* Clean up */
				Tcl_DStringFree(&extDString);
			}
			break;

		case IDX_QUIT: /* quit */
			if (game_in_progress && character_generated)
			{
				/* Hack -- Forget messages */
				msg_flag = FALSE;

				/* Save the game */
				do_cmd_save_game(FALSE);
			}
			quit(NULL);
			break;

		case IDX_KEYMAP_DUMP: /* keymap_dump */
			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "filename");
				return TCL_ERROR;
			}

			/* Get the file path */
			t = Tcl_GetString(objV[2]);

			/* */
			if (t[0] && t[0] != ' ')
			{
				/* Translate the file path */
				extString = UtfToExt_TranslateFileName(interp, t, &extDString);
				if (extString == NULL) return TCL_ERROR;

				/* Dump the keymap */
				if (keymap_dump(extString) == -1)
				{
					/* Set the error */
					Tcl_AppendStringsToObj(resultPtr,
						(char *) "error writing keymap file \"", t, "\"",
						NULL);

					/* Clean up */
					Tcl_DStringFree(&extDString);

					/* Failure */
					return TCL_ERROR;
				}

				/* Clean up */
				Tcl_DStringFree(&extDString);
			}
			break;

		case IDX_VERSION: /* version */
			Tcl_SetStringObj(resultPtr, format("%d.%d.%d", FAKE_VER_MAJOR,
				FAKE_VER_MINOR, FAKE_VER_PATCH), -1);
			break;

		case IDX_SAVEFILE: /* savefile */
			if (!g_initialized)
			{
				Tcl_SetStringObj(resultPtr, (char *) "game is not initialized", -1);
				return TCL_ERROR;
			}

			/* Return current savefile */
			if (objC == 2)
			{
				ExtToUtf_SetResult(interp, savefile);
				break;
			}

			/* Get the file path */
			t = Tcl_GetString(objV[2]);

			/* Translate the file path */
			extString = UtfToExt_TranslateFileName(interp, t, &extDString);
			if (extString == NULL) return TCL_ERROR;

			/* Remember the savefile */
			(void) strcpy(savefile, extString);

			/* Clean up */
			Tcl_DStringFree(&extDString);
			
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
		"tval", "flavor_color", "stat_name", "term_attr",
		NULL};
	enum {IDX_CLASS_NAME, IDX_GENDER_NAME, IDX_RACE_NAME,
		IDX_TVAL, IDX_FLAVOR_COLOR, IDX_STAT_NAME, IDX_TERM_ATTR
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
		
		case IDX_STAT_NAME: /* stat_name */
			names = (char **) keyword_stat;
			nameCount = 6;
			break;
		
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
	"INKEY_TARGET",
	"INKEY_MINDCRAFT", "INKEY_POWER", "INKEY_CMD_PET",
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

/* (array) find ?arg ...? */
int
objcmd_ARRAY_find(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	char *arrayName = (char *) infoCmd->clientData;

	return Struct_Find(interp, Struct_Lookup(interp, arrayName),
		objc, objv, infoCmd->depth + 1);
}

/* (array) info $index ?arg ...? */
int
objcmd_ARRAY_info(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	char *arrayName = (char *) infoCmd->clientData;
	StructType *typePtr = Struct_Lookup(interp, arrayName);
	int elemIndex;

	/* Get the array index */
	if (Struct_GetArrayIndexFromObj(interp, typePtr, &elemIndex, objV[1])
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	return Struct_Info(interp, typePtr, elemIndex, objc, objv,
		infoCmd->depth + 2);
}

/* (array) max */
int
objcmd_ARRAY_max(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	char *arrayName = (char *) infoCmd->clientData;

	StructType *typePtr = Struct_Lookup(interp, arrayName);
	
	/* Hack - ignore parameters */
	(void) objc;
	(void) objv;

	Tcl_SetObjResult(interp, Tcl_NewIntObj(typePtr->max));
	return TCL_OK;
}

/* (array) set $index ?$field? ?$value? */
int
objcmd_ARRAY_set(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	char *arrayName = (char *) infoCmd->clientData;

	StructType *typePtr = Struct_Lookup(interp, arrayName);
	int elemIndex;

	/* Get the array index */
	if (Struct_GetArrayIndexFromObj(interp, typePtr, &elemIndex, objV[1])
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	return Struct_Set(interp, typePtr, elemIndex, objc, objv,
		infoCmd->depth + 2);
}

/*
 *--------------------------------------------------------------
 *
 * objcmd_macro --
 *
 *	Implements the "macro" script command.
 * 	Syntax:
 *		macro action INDEX ?ACTION? -- get/set a macro's action
 *		macro command INDEX ?BOOLEAN? -- get/set a macro's "command" flag
 *		macro create KEYPRESS -- create a new macro with given keypress
 *		macro delete INDEX -- delete a macro
 *		macro keypress INDEX -- get/set a macro's trigger keypress
 *		macro max -- get number of macros
 *
 *--------------------------------------------------------------
 */

int
objcmd_macro(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"action", "command", "create", "delete",
		"keypress", "max", NULL};
	enum {IDX_ACTION, IDX_COMMAND, IDX_CREATE, IDX_DELETE,
		IDX_KEYPRESS, IDX_MAX} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int n;
	char *t, buf[80];

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
		case IDX_ACTION: /* action */
			if (Tcl_GetIntFromObj(interp, objV[2], &n) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (n < 0 || n >= macro__num) goto bad_index;
			if (objC == 4)
			{
				/* Get the action */
				t = Tcl_GetStringFromObj(objV[3], NULL);

				/* Printable --> Ascii */
				text_to_ascii(buf, t);

				/* Free the old macro action */
				string_free(macro__act[n]);

				/* Save the macro action */
				macro__act[n] = string_make(buf);
			}

			/* Ascii --> Printable */
			ascii_to_text(buf, macro__act[n]);

			/* Return action */
			Tcl_SetStringObj(resultPtr, buf, -1);
			break;

		case IDX_COMMAND: /* command */
			if (Tcl_GetIntFromObj(interp, objV[2], &n) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (n < 0 || n >= macro__num) goto bad_index;
			if (objC == 4)
			{
				int cmd_flag;

				if (Tcl_GetBooleanFromObj(interp, objV[3], &cmd_flag) != TCL_OK)
				{
					return TCL_ERROR;
				}

				/* Save the "cmd_flag" */
				macro__cmd[n] = cmd_flag;
			}

			/* Return "cmd_flag" */
			Tcl_SetBooleanObj(resultPtr, macro__cmd[n]);
			break;

		case IDX_CREATE: /* create */

			/* Get the keypress */
			t = Tcl_GetStringFromObj(objV[2], NULL);

			/* Printable --> Ascii */
			text_to_ascii(buf, t);

			/* Look for an existing macro */
			for (n = 0; n < macro__num; n++)
			{
				/* Notice macro redefinition */
				if (streq(macro__pat[n], buf))
				{
					Tcl_SetIntObj(resultPtr, n);
					return TCL_OK;
				}
			}
			
			/* Create an empty (normal) macro */
			macro_add(buf, "");

			/* Return the macro index */
			Tcl_SetIntObj(resultPtr, macro__num - 1);
			break;

		case IDX_DELETE: /* delete */
			Tcl_SetStringObj(resultPtr, (char *) "macros cannot be deleted (yet)", -1);
			return TCL_ERROR;

		case IDX_KEYPRESS: /* keypress */
			if (Tcl_GetIntFromObj(interp, objV[2], &n) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (n < 0 || n >= macro__num) goto bad_index;

			if (objC == 4)
			{
				/* Get the trigger */
				t = Tcl_GetStringFromObj(objV[3], NULL);

				/* Printable --> Ascii */
				text_to_ascii(buf, t);

				/* Look for an existing macro */
				for (n = 0; n < macro__num; n++)
				{
					/* Notice macro redefinition */
					if (streq(macro__pat[n], buf))
					{
						Tcl_SetIntObj(resultPtr, n);
						break;
					}
				}

				/* Free the old macro pattern */
				string_free(macro__pat[n]);

				/* Save the macro pattern */
				macro__pat[n] = string_make(buf);

				/* Return macro index */
				Tcl_SetIntObj(resultPtr, n);
				break;
			}

			/* Ascii --> Printable */
			ascii_to_text(buf, macro__pat[n]);

			/* Return keypress */
			Tcl_SetStringObj(resultPtr, buf, -1);
			break;

		case IDX_MAX: /* max */
			Tcl_SetIntObj(resultPtr, macro__num);
			break;
	}

	return TCL_OK;

bad_index:
	Tcl_AppendStringsToObj(resultPtr, "bad macro index \"",
		Tcl_GetStringFromObj(objV[2], NULL), "\": must be between ",
		format("%d and %d", 0, macro__num - 1), -1);
	return TCL_ERROR;
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

/*
 *--------------------------------------------------------------
 *
 * objcmd_mindcraft --
 *
 *	Implements the "mindcraft" script command. Usage:
 *	> Get a list of allowable mindcraft power indexes:
 *		mindcraft get
 *	> Get info about a mindcraft power by index:
 *		mindcraft info $index $arrayName
 *
 *--------------------------------------------------------------
 */

int
objcmd_mindcraft(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static cptr cmdOptions[] = {"get", "info", NULL};
	enum {IDX_GET, IDX_INFO} option;

	mindcraft_power *pow_ptr;
	Tcl_Obj *listObjPtr;
	int i, chance, min_fail;
	char comment[80], *s = comment, *t;

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
		case IDX_GET: /* get */
			listObjPtr = Tcl_NewListObj(0, NULL);
			for (i = 0; i < MINDCRAFT_MAX; i++)
			{
				pow_ptr = &mindcraft_powers[i];
/*				if (pow_ptr->min_lev > p_ptr->lev) break; */

				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewIntObj(i));
			}
			Tcl_SetObjResult(interp, listObjPtr);
			break;

		case IDX_INFO: /* info */
		    if (objC != 4)
		    {
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, (char *) "index arrayName");
				return TCL_ERROR;
		    }
			if (Tcl_GetIntFromObj(interp, objV[2], &i) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (i < 0 || i >= MINDCRAFT_MAX)
			{
				return TCL_ERROR;
			}
			t = Tcl_GetStringFromObj(objV[3], NULL);

			pow_ptr = &mindcraft_powers[i];

			chance = pow_ptr->fail;

			/* Reduce failure rate by "effective" level adjustment */
			chance -= 3 * (p_ptr->lev - pow_ptr->min_lev);

			/* Reduce failure rate by INT/WIS adjustment */
			chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[mp_ptr->spell_stat]] - 1);

			/* Not enough mana to cast */
			if (pow_ptr->mana_cost > p_ptr->csp)
			{
				chance += 5 * (pow_ptr->mana_cost - p_ptr->csp);
			}

			/* Extract the minimum failure rate */
			min_fail = adj_mag_fail[p_ptr->stat_ind[mp_ptr->spell_stat]];
			
			/* Minimum failure rate */
			if (chance < min_fail) chance = min_fail;

			/* Stunning makes spells harder */
			if (p_ptr->stun > 50) chance += 25;
			else if (p_ptr->stun) chance += 15;

			/* Always a 5 percent chance of working */
			if (chance > 95) chance = 95;
			
			/* Get info */
			mindcraft_info(s, i);
			if (s[0]) s++;

			SetArrayValueString(t, "char", format("%c", I2A(i)));
			ExtToUtf_SetArrayValueString(t, (char *) "name", (char *) pow_ptr->name);
			SetArrayValueLong(t, "level", pow_ptr->min_lev);
			SetArrayValueLong(t, "fail", chance);
			SetArrayValueLong(t, "mana", pow_ptr->mana_cost);
			SetArrayValueString(t, "comment", s);
			SetArrayValueLong(t, "okay", pow_ptr->min_lev <= p_ptr->lev);
			break;
	}

	return TCL_OK;
}

