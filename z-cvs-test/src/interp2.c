/* File: interp2.c */

/* Purpose: more tcl commands */

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
#include "cmdinfo-dll.h"
#include "struct-dll.h"
#include "util-dll.h"
#include "icon.h"


/*
 * Write the user's inscription for the given object into buf.
 */
static char *object_inscription(object_type *o_ptr, char *buf)
{
	/* No inscription yet */
	buf[0] = '\0';

	/* Use the user's inscription if available */
	if (get_user_inscription(o_ptr))
	{
		(void) strcpy(buf, quark_str(get_user_inscription(o_ptr)));
	}

	return buf;
}

/* This is an information block set in building.c */
_buildingdata g_buildingdata = {0};

/* From bldg.c */
static bool is_owner(building_type *bldg)
{
	if (bldg->member_class[p_ptr->pclass] == BUILDING_OWNER)
	{
		return (TRUE);	
	}

	if (bldg->member_race[p_ptr->prace] == BUILDING_OWNER)
	{
		return (TRUE);	
	}

	if ((bldg->member_realm[p_ptr->realm1] == BUILDING_OWNER) ||
		(bldg->member_realm[p_ptr->realm2] == BUILDING_OWNER))
	{
		return (TRUE);
	}

	return (FALSE);
}

/* From bldg.c */
static bool is_member(building_type *bldg)
{
	if (bldg->member_class[p_ptr->pclass])
	{
		return (TRUE);	
	}

	if (bldg->member_race[p_ptr->prace])
	{
		return (TRUE);	
	}

	if ((bldg->member_realm[p_ptr->realm1]) || (bldg->member_realm[p_ptr->realm2]))
	{
		return (TRUE);
	}
	
	return (FALSE);
}

/*
 *--------------------------------------------------------------
 *
 * objcmd_building --
 *
 *	Implements the "building" script command.
 * 	Syntax:
 *		building ownername -- name of owner
 *		building ownerrace -- race of owner
 *		building buildingename -- building name
 *		building action -- List of action chars/text
 *
 *--------------------------------------------------------------
 */

int
objcmd_building(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static char *cmdOptions[] = {"ownername", "ownerrace", "buildingname",
		"actions", "index", NULL};
	enum {IDX_OWNERNAME, IDX_OWNERRACE, IDX_BUILDINGNAME,
		IDX_ACTIONS, IDX_INDEX} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	building_type *bldg;
	char buff[20];
	int i;
	Tcl_Obj *listObjPtr;

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

	if (!g_buildingdata.inside)
	{
		Tcl_SetStringObj(resultPtr, "character isn't inside a building", -1);
		return TCL_ERROR;
	}

	bldg = g_buildingdata.bldg;

	switch (option)
	{
		case IDX_OWNERNAME: /* ownername */
			ExtToUtf_SetResult(interp, (char *) bldg->owner_name);
			break;

		case IDX_OWNERRACE: /* ownerrace */
			ExtToUtf_SetResult(interp, (char *) bldg->owner_race);
			break;

		case IDX_BUILDINGNAME: /* buildingname */
			ExtToUtf_SetResult(interp, (char *) bldg->name);
			break;

		case IDX_ACTIONS: /* actions */
		{
			Tcl_Obj *elemObjv[8];			
			int elemObjc;

			listObjPtr = Tcl_NewListObj(0, NULL);
			for (i = 0; i < 6; i++)
			{
				if (!bldg->letters[i]) continue;
				if (bldg->action_restr[i] == 0)
				{
					if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
						(!is_owner(bldg) && (bldg->other_costs[i] == 0)))
					{
						buff[0] = '\0';
					}
					else if (is_owner(bldg))
					{
						sprintf(buff, " (%dgp)", bldg->member_costs[i]);
					}
					else
					{
						sprintf(buff, " (%dgp)", bldg->other_costs[i]);
					}
				}
				else if (bldg->action_restr[i] == 1)
				{
					if (!is_member(bldg))
					{
						strcpy(buff, " (closed)");
					}
					else if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
						(is_member(bldg) && (bldg->other_costs[i] == 0)))
					{
						buff[0] = '\0';
					}
					else if (is_owner(bldg))
					{
						sprintf(buff, " (%dgp)", bldg->member_costs[i]);
					}
					else
					{
						sprintf(buff, " (%dgp)", bldg->other_costs[i]);
					}
				}
				else
				{
					if (!is_owner(bldg))
					{
						strcpy(buff, " (closed)");
					}
					else if (bldg->member_costs[i] != 0)
					{
						sprintf(buff, " (%dgp)", bldg->member_costs[i]);
					}
					else
					{
						buff[0] = '\0';
					}
				}

				/* Reset */
				elemObjc = 0;

				/* char */
				elemObjv[elemObjc++] = Tcl_NewStringObj("char", -1);
				elemObjv[elemObjc++] = Tcl_NewStringObj(&bldg->letters[i], 1);

				/* label */
				elemObjv[elemObjc++] = Tcl_NewStringObj("label", -1);
				elemObjv[elemObjc++] = ExtToUtf_NewStringObj(bldg->act_names[i], -1);

				/* info */
				elemObjv[elemObjc++] = Tcl_NewStringObj("info", -1);
				elemObjv[elemObjc++] = ExtToUtf_NewStringObj(buff, -1);

				/* action */
				elemObjv[elemObjc++] = Tcl_NewStringObj("action", -1);
				elemObjv[elemObjc++] = Tcl_NewIntObj(bldg->actions[i]);

				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewListObj(elemObjc, elemObjv));
			}
			Tcl_SetObjResult(interp, listObjPtr);
			break;
		}

		case IDX_INDEX: /* index */
			Tcl_SetIntObj(resultPtr, bldg - building);
			break;
	}

	return TCL_OK;
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

	static char *cmdOptions[] = {"assign", "blocked", "examine", "height", "width",
		"info", "in_bounds", "in_bounds_fully", "exists", "shape", "day",
		"wild_name",
		NULL};
	enum {IDX_ASSIGN, IDX_BLOCKED, IDX_EXAMINE, IDX_HEIGHT, IDX_WIDTH,
		IDX_INFO, IDX_IN_BOUNDS, IDX_IN_BOUNDS_FULLY, IDX_EXISTS, IDX_SHAPE,
		IDX_DAY , IDX_WILD_NAME
		} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int feat, y, x;
	int blocked;
	char desc[160], *varName;

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
		case IDX_ASSIGN: /* assign */
		{
			static char *opt[] = {"icon1", "icon2", "icon3", "icon4", NULL};
			int plane;
			t_assign assign;
			char *t;

			if (objC < 5 || objC > 6)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv,
					"y x plane ?assign?");
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
			if (!in_bounds_test(y, x))
			{
				goto bad_location;
			}
			if (Tcl_GetIndexFromObj(interp, objV[4], opt, "plane", 0, 
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
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "y x");
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
	
			if (!in_bounds_test(y, x))
			{
				goto bad_location;
			}
			
			blocked = !player_test_feature(y, x, 0);

			Tcl_SetBooleanObj(resultPtr, blocked);
			break;

		case IDX_EXAMINE: /* examine */
			if (!character_dungeon) goto not_exists;

			/* Required number of arguments */
			if (objC != 4)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "y x");
				return TCL_ERROR;
			}

			/* Get the y coordinate */
			if (Tcl_GetIntFromObj(interp, objV[2], &y) != TCL_OK)
			{
				y = -1;
			}
	
			/* Get the x coordinate */
			if (Tcl_GetIntFromObj(interp, objV[3], &x) != TCL_OK)
			{
				x = -1;
			}

			/* Get a string describing what is seen. */
			angtk_examine(y, x, desc);

			/* Set the result */
			ExtToUtf_SetResult(interp, desc);
			break;

		case IDX_HEIGHT: /* height */
			if (!character_dungeon) goto not_exists;
			Tcl_SetIntObj(resultPtr, g_cave_hgt);
			break;

		case IDX_WIDTH: /* width */
			if (!character_dungeon) goto not_exists;
			Tcl_SetIntObj(resultPtr, g_cave_wid);
			break;

		case IDX_INFO: /* info */
			if (!character_dungeon) goto not_exists;
			if (objC != 5)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "y x arrayName");
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
			if (!in_bounds_test(y, x))
			{
				goto bad_location;
			}

			/* Get the array variable name to dump results in */
			varName = Tcl_GetStringFromObj(objV[4], NULL);

			/* Get the feature index */
			feat = cave_feat(y, x);
			feat = f_info[feat].mimic;

			if (SetArrayValueLong(varName, "special", cave[y][x].special) != TCL_OK)
			{
				return TCL_ERROR;
			}
			/* XXX Hack -- Get the "mimic" index */
			if (cave[y][x].mimic)
			{
				feat = cave[y][x].mimic;
			}
			
			if (SetArrayValueLong(varName, "f_idx", feat) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (SetArrayValueLong(varName, "m_idx", cave_m_idx(y, x)) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (SetArrayValueLong(varName, "o_idx", cave_o_idx(y, x)) != TCL_OK)
			{
				return TCL_ERROR;
			}
			break;

		case IDX_IN_BOUNDS: /* in_bounds */
		case IDX_IN_BOUNDS_FULLY: /* in_bounds_fully */
			if (!character_dungeon) goto not_exists;
			if (objC != 4)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "y x");
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
				Tcl_SetBooleanObj(resultPtr, in_bounds_test(y, x));
			else
				Tcl_SetBooleanObj(resultPtr, in_bounds_fully_test(y, x));
			break;

		case IDX_EXISTS: /* exists */
			Tcl_SetBooleanObj(resultPtr, character_dungeon);
			break;

		case IDX_SHAPE: /* shape */
		{
			if (objC != 4)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "y x");
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
			Tcl_SetStringObj(resultPtr, (!p_ptr_depth && g_daytime) ?
				"day" : "night", -1);
			break;
		}
	
		case IDX_WILD_NAME: /* wild_name */
			if (!character_dungeon) goto not_exists;
			if (!dun_level)
			{
				if (p_ptr->town_num)
				{
					ExtToUtf_SetResult(interp, town[p_ptr->town_num].name);
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

bad_location:

	/* Set the error */
	Tcl_AppendStringsToObj(resultPtr, "location ",
		format("y=%d,x=%d", y, x), " is not in bounds", NULL);
	goto error;

not_exists:

	/* Set the error */
	Tcl_SetStringObj(resultPtr, "dungeon has not been generated yet", -1);

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

	static char *cmdOptions[] = {"find", "info", "memory", "flags",
		"inscription", NULL};
	enum {IDX_FIND, IDX_INFO, IDX_MEMORY, IDX_FLAGS,
		IDX_INSCRIPTION} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	int i, i_idx, tval;
	object_type *o_ptr;
	char buf[80], *buffer, *varName, *t;
	long length;
	Tcl_Obj *listObjPtr;

	/* Default to finding all matches */
	int request_limit = 0, match_limit = 0, cnt = 0;

	/* Default to ignoring activate */
	int request_activate = 0, match_activate = 0;

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

	if (!character_generated)
	{
		Tcl_AppendStringsToObj(resultPtr,
			"character has not been generated yet", NULL);
		return TCL_ERROR;
	}

	switch (option)
	{
		case IDX_FIND: /* find */

			/* Scan arguments for options */
			for (i = 2; i < objC; )
			{
				static char *cmdOptions[] = {"-limit", "-tester", "-tval",
					"-activate", NULL};

				/* Get the sub-option */
				if (Tcl_GetIndexFromObj(interp, objV[i], cmdOptions, "option",
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

					case 3: /* Activate */
						if (Tcl_GetBooleanFromObj(interp, objV[i+1],
							&match_activate) != TCL_OK)
						{
							return TCL_ERROR;
						}
						request_activate = 1;
						i += 2;
						break;
				}
			}
	
			/* Return a list of inventory[] indexes */
			listObjPtr = Tcl_NewListObj(0, NULL);

			/* Scan inventory list */
			for (i_idx = INVEN_WIELD; i_idx < INVEN_TOTAL; i_idx++)
			{
				u32b f1, f2, f3;

				/* Get the n'th item */
				o_ptr = &inventory[i_idx];

				/* Extract the flags */
				object_flags(o_ptr, &f1, &f2, &f3);
	
				if (request_tester && match_tester)
				{
					if (!item_tester_okay(o_ptr)) continue;
				}
				if (request_tval)
				{
					for (i = 0; i < tval_cnt; i++)
					{
						if (match_tval[0] == o_ptr->tval) break;
					}
					if (i == tval_cnt) continue;
				}
				if (request_activate)
				{
					bool activate = object_known_p(o_ptr) &&
						((f3 & TR3_ACTIVATE) != 0);
					if (activate != match_activate) continue;
				}
	
				/* Found a match */
				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewIntObj(i_idx - INVEN_WIELD));
	
				/* Return x matches */
				if (request_limit && (++cnt >= match_limit)) break;
			}
			Tcl_SetObjResult(interp, listObjPtr);
			break;

		case IDX_INFO: /* info */

			if (objC != 4)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "slot arrayName");
				return TCL_ERROR;
			}

			/* Get a numerical index or slot name */
			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				Tcl_ResetResult(interp);
				if (Tcl_GetIndexFromObj(interp, objV[2],
					(char **) keyword_slot, "slot", 0, &i_idx) != TCL_OK)
				{
					return TCL_ERROR;
				}
			}
			if ((i_idx < 0) || (i_idx >= (INVEN_TOTAL - INVEN_WIELD)))
			{
				goto bad_index;
			}
	
			/* Get object info */
			o_ptr = &inventory[INVEN_WIELD + i_idx];

			/* Get the array variable name to dump results in */
			varName = Tcl_GetStringFromObj(objV[3], NULL);

			if (dump_object_info(varName, o_ptr, INVEN_WIELD + i_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}
			break;

		case IDX_MEMORY: /* memory */

			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "slot");
				return TCL_ERROR;
			}
	
			/* Get a numerical index or slot name */
			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				Tcl_ResetResult(interp);
				if (Tcl_GetIndexFromObj(interp, objV[2],
					(char **) keyword_slot, "slot", 0, &i_idx) != TCL_OK)
				{
					return TCL_ERROR;
				}
			}
			if ((i_idx < 0) || (i_idx >= (INVEN_TOTAL - INVEN_WIELD)))
			{
				goto bad_index;
			}
	
			/* Get object info */
			o_ptr = &inventory[INVEN_WIELD + i_idx];
	
			buffer = Tcl_Alloc(5 * 1024L);
			length = angtk_describe_object(o_ptr, buffer, FALSE);
			Tcl_SetObjResult(interp, ExtToUtf_NewStringObj(buffer, length));
			Tcl_Free(buffer);
			break;

		case IDX_FLAGS: /* flags */

			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "slot");
				return TCL_ERROR;
			}

			/* Get a numerical index or slot name */
			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				Tcl_ResetResult(interp);
				if (Tcl_GetIndexFromObj(interp, objV[2],
					(char **) keyword_slot, "slot", 0, &i_idx) != TCL_OK)
				{
					return TCL_ERROR;
				}
			}
			if ((i_idx < 0) || (i_idx >= (INVEN_TOTAL - INVEN_WIELD)))
			{
				goto bad_index;
			}
	
			/* Get object info */
			o_ptr = &inventory[INVEN_WIELD + i_idx];

			listObjPtr = dump_object_flags(interp, o_ptr);
			if (listObjPtr == NULL)
			{
				return TCL_ERROR;
			}
			Tcl_SetObjResult(interp, listObjPtr);
			break;

		case IDX_INSCRIPTION: /* inscription */

			if (objC < 3)
			{
				/* Set the error */
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index ?string?");

				/* Failure */
				return TCL_ERROR;
			}

			/* Get a numerical index or slot name */
			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				Tcl_ResetResult(interp);
				if (Tcl_GetIndexFromObj(interp, objV[2],
					(char **) keyword_slot, "slot", 0, &i_idx) != TCL_OK)
				{
					return TCL_ERROR;
				}
			}

			/* Verify i_idx */
			if ((i_idx < 0) || (i_idx >= (INVEN_TOTAL - INVEN_WIELD)))
			{
				goto bad_index;
			}

			/* Get item info */
			o_ptr = &inventory[INVEN_WIELD + i_idx];

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
					set_user_inscription(o_ptr, quark_add(t));
				}
				else
				{
					/* Clear the inscription */
					set_user_inscription(o_ptr, 0);
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
		format("%d", INVEN_TOTAL - INVEN_WIELD - 1), NULL);

	/* Failure */
	return TCL_ERROR;
}

#ifdef ALLOW_EASY_FLOOR

int floor_y = -1, floor_x = -1;

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

	static char *cmdOptions[] = {"find", "info", "memory", "inscription",
		NULL};
	enum {IDX_FIND, IDX_INFO, IDX_MEMORY, IDX_INSCRIPTION} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	Tcl_Obj *listObjPtr;
	char buf[80], *buffer, *t, *varName;
	int this_o_idx, next_o_idx;
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
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp, objV[1], cmdOptions, "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* XXX Hack -- Determine the location to display */
	fy = floor_y;
	fx = floor_x;
	if (fy == -1)
	{
		fy = p_ptr_py;
		fx = p_ptr_px;
	}

	switch (option)
	{
		case IDX_FIND: /* find */
		{
			extern bool item_tester_hook_cast(const object_type *o_ptr);
			extern bool item_tester_hook_study(const object_type *o_ptr);

			bool (*old_tester_hook)(const object_type *) = item_tester_hook;
			bool (*temp_tester_hook)(const object_type *) = NULL;
			
			/* Scan arguments for options */
			for (i = 2; i < objC; )
			{
				static char *cmdOptions[] = {"-hook", "-limit", "-tester",
					"-tval", NULL};

				/* Get the sub-option */
				if (Tcl_GetIndexFromObj(interp, objV[i], cmdOptions, "option",
					0, &index) != TCL_OK)
				{
					return TCL_ERROR;
				}

				switch (index)
				{
					case 0: /* hook */
						t = Tcl_GetStringFromObj(objV[i+1], NULL);
						if (streq(t, "cast"))
							temp_tester_hook = item_tester_hook_cast;
						else if (streq(t, "study"))
							temp_tester_hook = item_tester_hook_study;
						else
						{
							Tcl_SetResult(interp, format("unknown hook \"%s\"",
								t), TCL_VOLATILE);
							return TCL_ERROR;
						}
						request_tester = 1;
						match_tester = 1;
						i += 2;
						break;

					case 1: /* Limit */
						if (Tcl_GetIntFromObj(interp, objV[i+1], &match_limit)
							!= TCL_OK)
						{
							return TCL_ERROR;
						}
						request_limit = 1;
						i += 2;
						break;

					case 2: /* Tester */
						if (Tcl_GetBooleanFromObj(interp, objV[i+1],
							&match_tester) != TCL_OK)
						{
							return TCL_ERROR;
						}
						request_tester = 1;
						i += 2;
						break;
		
					case 3: /* Tval */
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
			for (this_o_idx = cave_o_idx(fy, fx); this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire object */
				o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;
	
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
					Tcl_NewIntObj(this_o_idx));
	
				/* Return x matches */
				if (request_limit && (++cnt >= match_limit)) break;
			}

			/* XXX Hack -- Restore the hook */
			item_tester_hook = old_tester_hook;

			/* Return a list of o_list[] indexes */
			Tcl_SetObjResult(interp, listObjPtr);
			break;
		}

		case IDX_INFO: /* info */
	
			if (objC != 4)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index arrayName");
				return TCL_ERROR;
			}
			if (Tcl_GetIntFromObj(interp, objV[2], &i) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (i <= 0 || i > o_max) goto bad_index;
			
			/* Get the array variable name to dump results in */
			varName = Tcl_GetStringFromObj(objV[3], NULL);

			/* Get item info */
			o_ptr = &o_list[i];

			/* Illegal */
			if (!o_ptr->k_idx || (o_ptr->iy != fy) || (o_ptr->ix != fx))
			{
				goto bad_index;
			}
			
			/* Floor items always get 'a' for char */
			if (dump_object_info(varName, o_ptr, 0) != TCL_OK)
			{
				return TCL_ERROR;
			}
			break;

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
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index ?string?");

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
					set_user_inscription(o_ptr, quark_add(t));
				}
				else
				{
					/* Clear the inscription */
					set_user_inscription(o_ptr, 0);
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

#endif /* ALLOW_EASY_FLOOR */

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
 *		game variant -- Which variant is this?
 *
 *--------------------------------------------------------------
 */

extern errr macro_dump(cptr fname); /* see cmd4.c */
extern errr keymap_dump(cptr fname); /* see cmd4.c */

/* List of directory keywords */
char *keyword_path[] = {
	"ANGBAND_DIR_ROOT",
	"ANGBAND_DIR_USER",
	"ANGBAND_DIR_TK",
	"ANGBAND_DIR_COMMON_TK",
	NULL
};

/* Can the above directories be changed? */
static bool s_edit_path[] = {
	FALSE,
	FALSE,
	FALSE,
	TRUE,
	TRUE
};

extern bool check_dir(cptr s);

int
objcmd_game(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static char *cmdOptions[] = {"abort", "directory",
		"macro_dump", "new", "open", "process_pref_file", "quit",
		"keymap_dump", "savefile_info", "version", "variant",
		"savefile", NULL};
	enum {IDX_ABORT, IDX_DIRECTORY,
		IDX_MACRO_DUMP, IDX_NEW, IDX_OPEN, IDX_PROCESS_PREF_FILE, IDX_QUIT,
		IDX_KEYMAP_DUMP, IDX_SAVEFILE_INFO, IDX_VERSION, IDX_VARIANT,
		IDX_SAVEFILE} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	char *t, *utfString, *extString, *varName;
	cptr *angband_path[10];
	Tcl_DString utfDString, extDString;
	Tcl_Channel c;

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
				static char *abortSwitch[] = {"-noask", NULL};
				if (Tcl_GetIndexFromObj(interp, objV[2], abortSwitch,
					"switch", 0, &index) != TCL_OK)
				{
					return TCL_ERROR;
				}
				confirm = 0;
			}
			if (confirm && game_in_progress && character_generated)
			{
#if 1 /* def PLATFORM_X11 */
				int result;

				result = Tcl_EvalEx(g_interp,
					"tk_messageBox -icon warning -type okcancel -message \"Your character will not be saved!\" -title \"Quit Without Saving\"",
					-1, TCL_EVAL_GLOBAL);
				if (result == TCL_OK)
				{
					char *s = Tcl_GetStringResult(g_interp);
					if (!strcmp(s, "cancel")) break;
				}
#endif /* PLATFORM_X11 */

#ifdef PLATFORM_WINxx
				int oldMode;
				int winResult;

				oldMode = Tcl_SetServiceMode(TCL_SERVICE_ALL);
				winResult = MessageBox(NULL,
					"Your character will not be saved!", "Quit Without Saving",
					MB_ICONEXCLAMATION | MB_OKCANCEL);
				(void) Tcl_SetServiceMode(oldMode);
				
				if (winResult == IDCANCEL) break;
#endif /* PLATFORM_WIN */
			}
			quit(NULL);
			break;
		}
		
		case IDX_DIRECTORY: /* directory */
			if (objC < 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "dirname ?path?");
				return TCL_ERROR;
			}
			if (Tcl_GetIndexFromObj(interp, objV[2], keyword_path, "dirname",
				0, &index) != TCL_OK)
			{
				return TCL_ERROR;
			}
			
			angband_path[0] = &ANGBAND_DIR_ROOT;
			angband_path[1] = &ANGBAND_DIR_USER;
			angband_path[2] = &ANGBAND_DIR_TK;
			angband_path[3] = &ANGBAND_DIR_COMMON;
			angband_path[4] = &ANGBAND_DIR_COMMON_TK;
			
			if (objC == 4)
			{
				/* Only the sound directory can be changed */
				if (!s_edit_path[index])
				{
					/* Set the error */
					Tcl_SetStringObj(resultPtr,
						format("can't change directory \"%s\"",
						keyword_path[index]), -1);

					/* Failure */
					return TCL_ERROR; 
				}

				/* Get the new directory path */
				t = Tcl_GetString(objV[3]);

				/* Translate the directory path */
				extString = UtfToExt_TranslateFileName(interp, t, &extDString);
				if (extString == NULL) return TCL_ERROR;

				/* Check that the directory exists */
				if (!check_dir(extString))
				{
					/* Set the error */
					Tcl_SetStringObj(resultPtr,
						format("can't access directory \"%s\"", t), -1);

					/* Clean up */
					Tcl_DStringFree(&extDString);

					/* Failure */
					return TCL_ERROR;
				}

				/* Free the old directory path */
				string_free(*angband_path[index]);

				/* Save the new directory path */
				*angband_path[index] = string_make(extString);

				/* Clean up */
				Tcl_DStringFree(&extDString);

				/* Done */
				break;
			}

			/* Return the current directory path */
			ExtToUtf_SetResult(interp, (char *) *angband_path[index]);
			break;
			
		case IDX_MACRO_DUMP: /* macro_dump */
			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "filename");
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
				Tcl_SetStringObj(resultPtr, "game is not initialized", -1);
				return TCL_ERROR;
			}
			if (game_in_progress)
			{
				Tcl_SetStringObj(resultPtr, "game is in progress", -1);
				return TCL_ERROR;
			}
			game_in_progress = 1;
			play_game(TRUE);
			quit(NULL);
			break;

		case IDX_OPEN: /* open */
			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "filename");
				return TCL_ERROR;
			}
			if (!g_initialized)
			{
				Tcl_SetStringObj(resultPtr, "game is not initialized", -1);
				return TCL_ERROR;
			}
			if (game_in_progress)
			{
				Tcl_SetStringObj(resultPtr, "game is in progress", -1);
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
			c = Tcl_OpenFileChannel(interp, utfString, "r", 0);
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
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "filename");
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
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "filename");
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
						"error writing keymap file \"", t, "\"",
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

		case IDX_SAVEFILE_INFO: /* savefile_info */
			if (objC != 4)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "fileName varName");
				return TCL_ERROR;
			}
			if (!g_initialized)
			{
				Tcl_SetStringObj(resultPtr, "game is not initialized", -1);
				return TCL_ERROR;
			}

			/* Get the file path */
			t = Tcl_GetString(objV[2]);

			/* Get the file path */
			varName = Tcl_GetString(objV[3]);

			/* Translate the file path */
			extString = UtfToExt_TranslateFileName(interp, t, &extDString);
			if (extString == NULL) return TCL_ERROR;

			/* Scan the savefile */
			if (angtk_savefile_info(extString, varName))
			{
				/* Set the error */
				Tcl_SetStringObj(resultPtr, "error parsing savefile", -1);

				/* Clean up */
				Tcl_DStringFree(&extDString);

				/* Failure */
				return TCL_ERROR;
			}

			/* Clean up */
			Tcl_DStringFree(&extDString);
			break;

		case IDX_VERSION: /* version */
			Tcl_SetStringObj(resultPtr, format("%d.%d.%d", FAKE_VER_MAJOR,
				FAKE_VER_MINOR, FAKE_VER_PATCH), -1);
			break;

		case IDX_VARIANT: /* variant */
			/* Try char *g_variant = "ANGBANDTK" */
			Tcl_SetStringObj(resultPtr, "ZANGBANDTK", -1);
			break;

		case IDX_SAVEFILE: /* savefile */
			if (!g_initialized)
			{
				Tcl_SetStringObj(resultPtr, "game is not initialized", -1);
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
 * objcmd_highscore --
 *
 *	Implements the "highscore" script command.
 * 	Syntax:
 *		highscore find -- search for high scores
 *		highscore info -- get info about a highscore
 *		highscore predict -- get predicted character score (if any)
 *
 *--------------------------------------------------------------
 */

bool (*highscore_tester_hook)(high_score *the_score) = NULL;
int highscore_tester_param = 0;
bool highscore_predict = FALSE;

static bool highscore_tester_class(high_score *the_score)
{
	return (TRUE);
}

static bool highscore_tester_race(high_score * the_score)
{
	int p_r;
	int race_num = highscore_tester_param;

	p_r = atoi(the_score->p_r);
	if (p_r == race_num)
		return (TRUE);
	return (FALSE);
}

/*
 * show_highclass - selectively list highscores based on class
 * -KMW-
 */
void show_highclass(int building)
{
	char *title = "Unknown building number!";

	highscore_tester_hook = highscore_tester_class;
	highscore_tester_param = building;
	highscore_predict = FALSE;

	switch (building)
	{
		case 0:
			break; /* Library */
		case 1:
			title = "Busts of Greatest Kings";
			break;
		case 2:
			title = "Plaque - Greatest Arena Champions";
			break;
		case 3:
			break; /* Gambling House */
		case 4:
			break; /* Inn */
		case 5:
			break; /* Beastmaster */
		case 6:
			break; /* Weaponsmaster */
		case 7:
			title = "Plaque - Greatest Fighters";
			break;
		case 8:
			title = "Spires of the Greatest Magic-Users";
			break;
		case 9:
			title = "Busts of Greatest Priests";
			break;
		case 10:
			title = "Wall Inscriptions - Greatest Thieves";
			break;
		case 11:
			title = "Plaque - Greatest Rangers";
			break;
		case 12:
			title = "Shrine to the Greatest Paladins";
			break;
		case 13:
			title = "Spires of the Greatest Illusionists";
			break;
	}

	/* Now, list the active player if they qualify */
	if ((building == 1) && (p_ptr->lev >= PY_MAX_LEVEL))
	{
		highscore_predict = TRUE;
	}
	else if ((building == 2) && (p_ptr->arena_number > MAX_ARENA_MONS))
	{
		highscore_predict = TRUE;
	}
	else if ((building != 1) && (building != 2))
	{
		if (p_ptr->pclass == (building - 10))
		{
			highscore_predict = TRUE;
		}
	}

	angtk_eval("angband_display", "highscore", "show", title, NULL);

	any_more(NULL);

	angtk_eval("angband_display", "highscore", "hide", NULL);

	highscore_tester_hook = NULL;
	highscore_tester_param = 0;
	highscore_predict = FALSE;
}

/*
 * Race Legends
 * -KMW- 
 */
void race_score(int race_num)
{
	char out_val[80];

	highscore_tester_hook = highscore_tester_race;
	highscore_tester_param = race_num;
	highscore_predict = FALSE;

	/* add player if qualified */
	if (p_ptr->prace == race_num)
	{
		highscore_predict = TRUE;
	}

	(void) sprintf(out_val, "The Greatest of all the %s",
		race_info[race_num].title);
	angtk_eval("angband_display", "highscore", "show", out_val, NULL);

	any_more(NULL);

	highscore_tester_hook = NULL;
	highscore_tester_param = 0;
}

/*
 * Race Legends
 * -KMW-
 */
void race_legends(void)
{
	int i;

	for (i = 0; i < MAX_P_IDX; i++)
	{
		race_score(i);
	}

	angtk_eval("angband_display", "highscore", "hide", NULL);
}

bool highscore_tester_okay(high_score *the_score)
{
	/* Check the hook */
	if (highscore_tester_hook)
	{
		if (!(*highscore_tester_hook)(the_score)) return (FALSE);
	}

	/* Assume okay */
	return (TRUE);
}

int highscore_delete(int slot)
{
	char pathBak[1024], pathRaw[1024], pathTmp[1024];
	high_score score;
	int fd, i, result = TCL_OK;
	int mode = 0644;

	path_build(pathBak, 1024, ANGBAND_DIR_APEX, "scores.bak");
	path_build(pathRaw, 1024, ANGBAND_DIR_APEX, "scores.raw");
	path_build(pathTmp, 1024, ANGBAND_DIR_APEX, "scores.tmp");

	/* Create scores.tmp */
	fd_kill(pathTmp);
	FILE_TYPE(FILE_TYPE_DATA);
	fd = fd_make(pathTmp, mode);
	if (fd < 0) return TCL_ERROR;
	
	for (i = 0; i < MAX_HISCORES; i++)
	{
		/* Read the current score. Stop if we reach the end.*/
		if (highscore_read(&score)) break;

		/* Skip the score to be deleted */
		if (i == slot) continue;

		/* Write the score to the temp file */
		if (fd_write(fd, (char *) &score, sizeof(score)))
		{
			result = TCL_ERROR;
			break;
		}
	}

	/* Close scores.tmp */
	fd_close(fd);

	if (result == TCL_OK)
	{
		/* Close scores.raw */
		fd_close(highscore_fd);
		highscore_fd = -1;

		/* scores.raw --> scores.bak */
		fd_kill(pathBak);
		rename(pathRaw, pathBak);
	
		/* scores.tmp --> scores.raw */
		rename(pathTmp, pathRaw);
	}

	/* Delete scores.tmp */
	fd_kill(pathTmp);

	return result;
}

int HighScoreToArray(char *arrayName, high_score *the_score)
{
	char *when;

	(void) ExtToUtf_SetArrayValueString(arrayName, "what", the_score->what);
	(void) SetArrayValueLong(arrayName, "pts", atol(the_score->pts));
	(void) SetArrayValueLong(arrayName, "gold", atol(the_score->gold));
	(void) SetArrayValueLong(arrayName, "turns", atol(the_score->turns));
	when = the_score->day;

	(void) ExtToUtf_SetArrayValueString(arrayName, "day", when);
	(void) ExtToUtf_SetArrayValueString(arrayName, "who", the_score->who);
	(void) SetArrayValueLong(arrayName, "uid", atoi(the_score->uid));
	(void) ExtToUtf_SetArrayValueString(arrayName, "sex", the_score->sex);
	(void) SetArrayValueLong(arrayName, "p_r", atoi(the_score->p_r));
	(void) SetArrayValueLong(arrayName, "p_c", atoi(the_score->p_c));
	(void) SetArrayValueLong(arrayName, "cur_lev", atoi(the_score->cur_lev));
	(void) SetArrayValueLong(arrayName, "cur_dun", atoi(the_score->cur_dun));
	(void) SetArrayValueLong(arrayName, "max_lev", atoi(the_score->max_lev));
	(void) SetArrayValueLong(arrayName, "max_dun", atoi(the_score->max_dun));
	(void) ExtToUtf_SetArrayValueString(arrayName, "how", the_score->how);

	/* Extra human-readable names */
	(void) SetArrayValueString(arrayName, "race",
		(char *) keyword_race[atoi(the_score->p_r)]);
	(void) SetArrayValueString(arrayName, "class",
		(char *) keyword_class[atoi(the_score->p_c)]);

	return TCL_OK;
}

int
objcmd_highscore(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static char *cmdOptions[] = {"delete", "find", "info", "predict", NULL};
	enum {OPT_DELETE, OPT_FIND, OPT_INFO, OPT_PREDICT} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	char path[1024], *t;
	high_score the_score;
	int i, old_fd = highscore_fd;
	int request_class = 0, match_class;
	int request_race = 0, match_race;
	int request_tester = 0, match_tester;
	Tcl_Obj *listObjPtr;

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

	/* Open the highscore file, unless it is already open */
	path_build(path, 1024, ANGBAND_DIR_APEX, "scores.raw");
	if (highscore_fd == -1)
	{
		highscore_fd = fd_open(path, O_RDONLY);
		if (highscore_fd < 0)
		{
			Tcl_SetStringObj(resultPtr, "score file unavailable", -1);
			return TCL_ERROR;
		}
	}

	if (highscore_seek(0)) goto noread;

	switch (option)
	{
		case OPT_DELETE: /* delete */
			if (objC != 3)
			{
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index");
				return TCL_ERROR;
		    }
			if (Tcl_GetIntFromObj(interp, objV[2], &index) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (highscore_delete(index) != TCL_OK) goto close_err;
			if (old_fd != -1)
				highscore_fd = fd_open(path, O_RDONLY);
			break;

		case OPT_FIND: /* find */

			/* Scan arguments for options */
			for (i = 2; i < objC; )
			{
				static char *cmdOptions[] = {"-class", "-race", "-tester", NULL};

				/* Get the sub-option */
			    if (Tcl_GetIndexFromObj(interp, objV[i], cmdOptions, "option",
					0, &index) != TCL_OK)
				{
					goto close_err;
				}

				switch (index)
				{
					case 0: /* class */
						if (Tcl_GetIntFromObj(interp, objV[i+1], &match_class)
							!= TCL_OK)
						{
							goto close_err;
						}
						if (match_class < 0 || match_class >= MAX_CLASS) {
							Tcl_SetStringObj(resultPtr,
								format("bad class \"%d\": must be between 0 and %d",
									match_class, MAX_CLASS - 1), -1);
							goto close_err;
						}
						request_class = 1;
						i += 2;
						break;

					case 1: /* race */
						if (Tcl_GetIntFromObj(interp, objV[i+1], &match_race)
							!= TCL_OK)
						{
							goto close_err;
						}
						if (match_race < 0 || match_race >= MAX_P_IDX)
						{
							Tcl_SetStringObj(resultPtr,
								format("bad race \"%d\": must be between 0 and %d",
									match_race, MAX_P_IDX - 1), -1);
							goto close_err;
						}
						request_race = 1;
						i += 2;
						break;

					case 2: /* tester */
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

			/* Return a list of highscore indexes */
			listObjPtr = Tcl_NewListObj(0, NULL);
	
			/* Scan highscore list */
			for (i = 0; i < MAX_HISCORES; i++)
			{
				if (highscore_read(&the_score)) break;
	
				if (request_tester && match_tester)
				{
					if (!highscore_tester_okay(&the_score)) continue;
				}
				if (request_class)
				{
					if (atoi(the_score.p_c) != match_class) continue;
				}
				if (request_race)
				{
					if (atoi(the_score.p_r) != match_race) continue;
				}
	
				/* Found a match */
				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewIntObj(i));
			}
			Tcl_SetObjResult(interp, listObjPtr);
			break;

		case OPT_INFO: /* info */
		    if (objC != 4)
		    {
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index arrayName");
				return TCL_ERROR;
		    }
			if (Tcl_GetIntFromObj(interp, objV[2], &index) != TCL_OK)
			{
				return TCL_ERROR;
			}
			t = Tcl_GetStringFromObj(objV[3], NULL);

			if (highscore_seek(index)) goto noread;
			if (highscore_read(&the_score)) goto noread;

			(void) HighScoreToArray(t, &the_score);
			break;

		case OPT_PREDICT: /* predict */
		    if (objC < 3 || objC > 4)
		    {
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "?-force? arrayName");
				return TCL_ERROR;
		    }
			t = Tcl_GetStringFromObj(objV[2], NULL);
			if (objC == 4) /* -force */
			{
				t = Tcl_GetStringFromObj(objV[3], NULL);
			}
			if (highscore_predict || (objC == 4))
			{
				predict_score(&the_score);
				(void) HighScoreToArray(t, &the_score);
			}
			Tcl_SetBooleanObj(resultPtr, highscore_predict);
			break;
	}

	/* Close the file if it wasn't already open */
	if (old_fd == -1)
	{
		(void) fd_close(highscore_fd);
		highscore_fd = -1;
	}
	return TCL_OK;

noread:
	Tcl_SetStringObj(resultPtr, "error reading score file", -1);

close_err:
	if (highscore_fd != -1)
	{
		if (old_fd == -1)
		{
			(void) fd_close(highscore_fd);
			highscore_fd = -1;
		}
	}

	return TCL_ERROR;
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

	static char *cmdOptions[] = {"class_name", "gender_name", "race_name",
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
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "option ?index?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], cmdOptions, "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	if (option == IDX_FLAVOR_COLOR) /* flavor_color */
	{
		extern cptr keyword_term_color[];
		static char *flavorName[] = {"amulet", "mushroom", "potion",
			"ring", "rod", "staff", "wand", NULL};
		if (objC != 3)
		{
			Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "flavor");
			return TCL_ERROR;
		}
	    if (Tcl_GetIndexFromObj(interp, objV[2], flavorName, "flavor", 0, 
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
			nameCount = MAX_P_IDX;
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

	if (g_icon_size)
	{
		Tcl_SetResult(interp, "icons were already initialized", TCL_VOLATILE);
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
	Tcl_SetResult(interp, (char *) inkey_to_str[inkey_flags], TCL_VOLATILE);
	return TCL_OK;
}

/* (inkey) other */
int
objcmd_inkey_other(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	if (inkey_flags == INKEY_ITEM)
	{
#ifdef ALLOW_EASY_FLOOR 

		if (easy_floor)
		{
			Tcl_SetResult(interp,
				(p_ptr_command_wrk == (USE_EQUIP)) ? "equipment" : 
				(p_ptr_command_wrk == (USE_INVEN)) ? "inventory" :
				"floor", TCL_VOLATILE);
		}
		else
		
#endif /* ALLOW_EASY_FLOOR */

		Tcl_SetResult(interp,
			p_ptr_command_wrk ? "equipment" : "inventory", TCL_VOLATILE);
	}
	else if (inkey_flags == INKEY_SPELL)
	{
		Tcl_SetObjResult(interp, Tcl_NewIntObj(inkey_book));
	}

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

	static char *cmdOptions[] = {"count", "find", "info", "memory",
		"total_weight", "weight_limit", "inscription", "worthless",
		NULL};
	enum {IDX_COUNT, IDX_FIND, IDX_INFO, IDX_MEMORY,
		IDX_TOTAL_WEIGHT, IDX_WEIGHT_LIMIT, IDX_INSCRIPTION, IDX_WORTHLESS
	} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	Tcl_Obj *listObjPtr;
	int i_idx, k_idx, i, z = 0, tval;
	char buf[80], *buffer, *varName, *t;
	object_type *o_ptr;
	long length;

	/* Default to no restriction on k_idx */
	int request_k_idx = 0, match_k_idx[10], k_idx_cnt = 0;

	/* Default to finding all matches */
	int request_limit = 0, match_limit = 0, cnt = 0;

	/* Default to ignoring store_will_buy() */
	int request_store = 0, match_store = 0;

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

	/* Find the "final" slot */
	for (i_idx = 0; i_idx < INVEN_PACK; i_idx++)
	{
		/* Get the n'th item */
		o_ptr = &inventory[i_idx];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Track */
		z = i_idx + 1;
	}

	switch (option)
	{
		case IDX_COUNT: /* count */
			Tcl_SetIntObj(resultPtr, z);
			break;

		case IDX_FIND: /* find */
		{
			extern bool item_tester_hook_cast(const object_type *o_ptr);
			extern bool item_tester_hook_study(const object_type *o_ptr);

			bool (*old_tester_hook)(const object_type *) = item_tester_hook;
			bool (*temp_tester_hook)(const object_type *) = NULL;

			/* Scan arguments for options */
			for (i = 2; i < objC; )
			{
				static char *cmdOptions[] = {"-hook", "-k_idx", "-limit",
					"-tester", "-tval", "-store_will_buy", NULL};

				/* Get the sub-option */
			    if (Tcl_GetIndexFromObj(interp, objV[i], cmdOptions, "option",
					0, &index) != TCL_OK)
				{
					return TCL_ERROR;
			    }

				switch (index)
				{
					case 0: /* hook */
						t = Tcl_GetStringFromObj(objV[i+1], NULL);
						if (streq(t, "cast"))
							temp_tester_hook = item_tester_hook_cast;
						else if (streq(t, "study"))
							temp_tester_hook = item_tester_hook_study;
						else
						{
							Tcl_SetResult(interp, format("unknown hook \"%s\"",
								t), TCL_VOLATILE);
							return TCL_ERROR;
						}
						request_tester = 1;
						match_tester = 1;
						i += 2;
						break;

					case 1: /* -k_idx */
						if (Tcl_GetIntFromObj(interp, objV[i+1], &k_idx)
							!= TCL_OK)
						{
							return TCL_ERROR;
						}
						match_k_idx[k_idx_cnt++] = k_idx;
						request_k_idx = 1;
						i += 2;
						break;

					case 2: /* -limit */
						if (Tcl_GetIntFromObj(interp, objV[i+1], &match_limit)
							!= TCL_OK)
						{
							return TCL_ERROR;
						}
						request_limit = 1;
						i += 2;
						break;

					case 3: /* -tester */
						if (Tcl_GetBooleanFromObj(interp, objV[i+1],
							&match_tester) != TCL_OK)
						{
							return TCL_ERROR;
						}
						request_tester = 1;
						i += 2;
						break;

					case 4: /* -tval */
						t = Tcl_GetStringFromObj(objV[i+1], NULL);
						if (angtk_tval_const(&tval, t) != TCL_OK)
						{
							return TCL_ERROR;
						}
						match_tval[tval_cnt++] = tval;
						request_tval = 1;
						i += 2;
						break;

					case 5: /* -store_will_buy */
						if (!storedata.shopping)
						{
							Tcl_SetStringObj(resultPtr,
								"character isn't shopping", -1);
							return TCL_ERROR;
						}
						if (Tcl_GetBooleanFromObj(interp, objV[i+1],
							&match_store) != TCL_OK)
						{
							return TCL_ERROR;
						}
						request_store = 1;
						i += 2;
						break;
				}
			}

			if (temp_tester_hook) item_tester_hook = temp_tester_hook;

			/* Return a list of inventory[] indexes */
			listObjPtr = Tcl_NewListObj(0, NULL);

			/* Scan inventory list */
			for (i_idx = 0; i_idx < z; i_idx++)
			{
				/* Get the n'th item */
				o_ptr = &inventory[i_idx];

				/* Return items by k_info[] index */
				if (request_k_idx)
				{
					for (i = 0; i < k_idx_cnt; i++)
					{
						if (match_k_idx[0] == o_ptr->k_idx) break;
					}
					if (i == k_idx_cnt) continue;
				}

				/* Return "okay" items */
				if (request_tester && match_tester)
				{
					if (!item_tester_okay(o_ptr)) continue;
				}

				/* Return items the store will buy */
				if (request_store && match_store)
				{
					if (!store_will_buy(o_ptr)) continue;
				}

				/* Return items by tval */
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
					Tcl_NewIntObj(i_idx));
	
				/* Return x matches */
				if (request_limit && (++cnt >= match_limit)) break;
			}

			/* XXX Hack -- Restore the hook */
			item_tester_hook = old_tester_hook;

			/* Return the list of indexes */
			Tcl_SetObjResult(interp, listObjPtr);
			break;
		}

		case IDX_INFO: /* info */
	
		    if (objC != 4)
		    {
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index arrayName");
				return TCL_ERROR;
		    }
			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if ((i_idx < 0) || (i_idx >= z))
			{
				goto bad_index;
			}

			/* Get the array variable name to dump results in */
			varName = Tcl_GetStringFromObj(objV[3], NULL);

			/* Get item info */
			o_ptr = &inventory[i_idx];

			if (dump_object_info(varName, o_ptr, i_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}
			break;

		case IDX_MEMORY: /* memory */

			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if ((i_idx < 0) || (i_idx >= z))
			{
				goto bad_index;
			}

			/* Get item info */
			o_ptr = &inventory[i_idx];

			buffer = Tcl_Alloc(5 * 1024L);
			length = angtk_describe_object(o_ptr, buffer, FALSE);
			Tcl_SetObjResult(interp, ExtToUtf_NewStringObj(buffer, length));
			Tcl_Free(buffer);
			break;

		case IDX_TOTAL_WEIGHT: /* total_weight */
			Tcl_SetIntObj(resultPtr, p_ptr->total_weight);
			break;

		case IDX_WEIGHT_LIMIT: /* weight_limit */

			/* Max carrying capacity in 10ths of pounds */
			i = adj_str_wgt[p_ptr->stat_ind[A_STR]] * 100;
			Tcl_SetIntObj(resultPtr, i);
			break;

		case IDX_INSCRIPTION: /* inscription */

		    if (objC < 3)
		    {
		    	/* Set the error */
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index ?string?");

				/* Failure */
				return TCL_ERROR;
		    }

			/* Get the item index */
			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Verify i_idx */
			if ((i_idx < 0) || (i_idx >= z))
			{
				goto bad_index;
			}

			/* Get item info */
			o_ptr = &inventory[i_idx];

			/* Set the inscription. */
			if (objC == 4)
			{
				/* Get the new inscription */
				t = Tcl_GetStringFromObj(objV[3], NULL);

				if (strlen(t))
				{
					/* Save the inscription */
					set_user_inscription(o_ptr, quark_add(t));
				}
				else
				{
					/* Clear the inscription */
					set_user_inscription(o_ptr, 0);
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

		case IDX_WORTHLESS: /* worthless */

		    if (objC != 3)
		    {
		    	/* Set the error */
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index");

				/* Failure */
				return TCL_ERROR;
		    }

			/* Get the item index */
			if (Tcl_GetIntFromObj(interp, objV[2], &i_idx) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Verify i_idx */
			if ((i_idx < 0) || (i_idx >= z))
			{
				goto bad_index;
			}

			/* Get item info */
			o_ptr = &inventory[i_idx];

			/* Is it worthless? */
			Tcl_SetBooleanObj(resultPtr, object_value(o_ptr) < 1);
			break;
	}

	return TCL_OK;

bad_index:
	Tcl_AppendStringsToObj(resultPtr, "bad inventory index \"",
		Tcl_GetStringFromObj(objV[2], NULL),
		"\": must be between 0 and ",
		format("%d", z - 1), NULL);
	return TCL_ERROR;
}

/* keycount */
int
objcmd_keycount(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	Tcl_SetObjResult(interp,
		Tcl_NewBooleanObj(Term->key_head != Term->key_tail));
	return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * objcmd_keymap --
 *
 *	Implements the "keymap" script command.
 * 	Syntax:
 *		keymap action KEY ?ACTION? -- get/set a keymap's action
 *		keymap find STRING -- find keymap with underlying action
 *
 *--------------------------------------------------------------
 */

int
objcmd_keymap(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	/* Success */
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

	static char *cmdOptions[] = {"action", "command", "create", "delete",
		"keypress", "max", NULL};
	enum {IDX_ACTION, IDX_COMMAND, IDX_CREATE, IDX_DELETE,
		IDX_KEYPRESS, IDX_MAX} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int i, n;
	char *t, buf[80];

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
			Tcl_SetStringObj(resultPtr, "macros cannot be deleted (yet)", -1);
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

	static char *cmdOption[] = {"color", "count", "get", NULL};
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

	static char *cmdOptions[] = {"get", "info", NULL};
	enum {IDX_GET, IDX_INFO} option;

	mindcraft_power *pow_ptr;
	Tcl_Obj *listObjPtr;
	int i, chance, min_fail;
	char comment[80], *s = comment, *t;

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
		case IDX_GET: /* get */
			listObjPtr = Tcl_NewListObj(0, NULL);
			for (i = 0; i < MAX_MINDCRAFT_POWERS; i++)
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
				Tcl_WrongNumArgs(interp, infoCmd->depth + 2, objv, "index arrayName");
				return TCL_ERROR;
		    }
			if (Tcl_GetIntFromObj(interp, objV[2], &i) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (i < 0 || i >= MAX_MINDCRAFT_POWERS)
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
			ExtToUtf_SetArrayValueString(t, "name", (char *) pow_ptr->name);
			SetArrayValueLong(t, "level", pow_ptr->min_lev);
			SetArrayValueLong(t, "fail", chance);
			SetArrayValueLong(t, "mana", pow_ptr->mana_cost);
			SetArrayValueString(t, "comment", s);
			SetArrayValueLong(t, "okay", pow_ptr->min_lev <= p_ptr->lev);
			break;
	}

	return TCL_OK;
}

