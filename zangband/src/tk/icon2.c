/* File: icon2.c */

/* Purpose: more icon stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include "icon.h"
#include "widget.h"

static char *AssignToString_Alternate(char *buf, t_assign *assign)
{
	(void) sprintf(buf, "alternate %d", assign->alternate.index);
	return buf;
}

static int StringToAssign_Alternate(Tcl_Interp *interp, t_assign *assignPtr, char *desc)
{
	char option[64];
	int index;

	if (sscanf(desc, "%s %d", option, &index) != 2)
	{
		Tcl_SetResult(interp, format("malformed assignment \"%s\"",
			desc), TCL_VOLATILE);
		return TCL_ERROR;
	}

	if ((index < 0) || (index >= g_alternate_count))
	{
		/* Set the error */
		Tcl_SetResult(interp, format("bad alternate \"%d\": "
			"must be from 0 to %d", index, g_alternate_count - 1),
			TCL_VOLATILE);

		/* Failure */
		return TCL_ERROR;
	}

	assignPtr->assignType = ASSIGN_TYPE_ALTERNATE;
	assignPtr->alternate.index = index;

	return TCL_OK;
}

static char *AssignToString_Flavor(char *buf, t_assign *assignPtr)
{
	(void) sprintf(buf, "flavor %s %d",
		g_flavor[assignPtr->flavor.group].desc,
		assignPtr->flavor.index);
	return buf;
}

static int StringToAssign_Flavor(Tcl_Interp *interp, t_assign *assignPtr, char *desc)
{
	char option[64], flavorName[64];
	int group, index;
	Tcl_HashEntry *hPtr;

	if (sscanf(desc, "%s %s %d", option, flavorName, &index) != 3)
	{
		Tcl_SetResult(interp, format("malformed assignment \"%s\"",
			desc), TCL_VOLATILE);
		return TCL_ERROR;
	}

	/* Lookup the flavor by name */
	hPtr = Tcl_FindHashEntry(&g_flavor_table, flavorName);

	/* The flavor was not found */
	if (hPtr == NULL)
	{
		/* Set the error */
		Tcl_SetResult(interp, format("unknown flavor \"%s\"", flavorName),
			TCL_VOLATILE);

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the g_flavor[] index */
	group = (int) Tcl_GetHashValue(hPtr);

	/* Verify the flavor index */
	if ((index < 0) || (index >= g_flavor[group].count))
	{
		/* Set the error */
		Tcl_SetResult(interp,
			format("bad flavor index \"%d\": must be from 0 to %d",
			index, g_flavor[group].count - 1), TCL_VOLATILE);

		/* Failure */
		return TCL_ERROR;
	}

	assignPtr->assignType = ASSIGN_TYPE_FLAVOR;
	assignPtr->flavor.group = group;
	assignPtr->flavor.index = index;

	return TCL_OK;
}

static char *AssignToString_Icon(char *buf, t_assign *assign)
{
	if (assign->icon.ascii == -1)
	{
		(void) sprintf(buf, "icon %s %d",
			g_icon_data[assign->icon.type].desc,
			assign->icon.index);
	}
	else
	{
		(void) sprintf(buf,"icon %s %d %d",
			g_icon_data[assign->icon.type].desc,
			assign->icon.index, assign->icon.ascii);
	}

	return buf;
}

static int StringToAssign_Icon(Tcl_Interp *interp, t_assign *assignPtr, char *desc)
{
	char option[64], typeName[64];
	IconSpec iconSpec;

	iconSpec.ascii = -1;
	if (sscanf(desc, "%s %s %d %d", option, typeName, &iconSpec.index,
		&iconSpec.ascii) < 3)
	{
		Tcl_SetResult(interp, format("malformed assignment \"%s\"",
			desc), TCL_VOLATILE);
		return TCL_ERROR;
	}

	if (Icon_Validate(interp, typeName, iconSpec.index, iconSpec.ascii,
		&iconSpec) != TCL_OK)
	{
		return TCL_ERROR;
	}

	assignPtr->assignType = ASSIGN_TYPE_ICON;
	assignPtr->icon.type = iconSpec.type;
	assignPtr->icon.index = iconSpec.index;
	assignPtr->icon.ascii = iconSpec.ascii;

	return TCL_OK;
}


cptr keyword_assign_type[] = {"alternate", "flavor", "icon", NULL};

/* char* -> t_assign */
typedef char *(*AssignToStringProc)(char *buf, t_assign *assign);
AssignToStringProc gAssignToStringProc[ASSIGN_TYPE_MAX] = {
	AssignToString_Alternate,
	AssignToString_Flavor,
	AssignToString_Icon

};

/* t_assign -> char* */
typedef int (*StringToAssignProc)(Tcl_Interp *interp, t_assign *assignPtr, char *desc);
StringToAssignProc gStringToAssignProc[ASSIGN_TYPE_MAX] = {
	StringToAssign_Alternate,
	StringToAssign_Flavor,
	StringToAssign_Icon
};

int assign_parse(Tcl_Interp *interp, t_assign *assignPtr, char *desc)
{
	char option[64];
	Tcl_Obj *objPtr;
	int assignType;

	/* Ex. "icon dungeon 10" or "icon ascii 12 10" */
	if (sscanf(desc, "%s", option) != 1)
	{
		Tcl_SetResult(interp, format("malformed assignment \"%s\"",
			desc), TCL_VOLATILE);
		return TCL_ERROR;
	}

	objPtr = Tcl_NewStringObj(option, -1);
	if (Tcl_GetIndexFromObj(interp, objPtr, (char **) keyword_assign_type,
		(char *) "option", 0, &assignType) != TCL_OK)
	{
		Tcl_DecrRefCount(objPtr);
		return TCL_ERROR;
	}
	Tcl_DecrRefCount(objPtr);

	return (*gStringToAssignProc[assignType])(interp, assignPtr, desc);
}

char *assign_print(char *buf, t_assign *assignPtr)
{
	return (*gAssignToStringProc[assignPtr->assignType])(buf, assignPtr);
}

char *assign_print2(char *buf, int assignType, int assignIndex)
{
	t_assign *assignPtr = &g_assign[assignType].assign[assignIndex];
	return assign_print(buf, assignPtr);
}

char *assign_print_object(char *buf, object_type *o_ptr)
{
	t_assign assign;
	get_object_assign(&assign, o_ptr);
	return assign_print(buf, &assign);
}

/*
 * Get the assignment for the given object. Handle "empty" objects and
 * resolve alternate assignments.
 */
void get_object_assign(t_assign *assignPtr, object_type *o_ptr)
{
	t_assign assign;

	if (o_ptr->k_idx)
	{
		assign = g_assign[ASSIGN_OBJECT].assign[o_ptr->k_idx];
		if (assign.assignType == ASSIGN_TYPE_ALTERNATE)
		{
			t_alternate *alternatePtr = &g_alternate[assign.alternate.index];
			int index = 0;
			switch (alternatePtr->reason)
			{
				case REASON_NUMBER:
					if (o_ptr->number == 1) ++index;
					break;
				
				case REASON_IDENT:
					if (object_known_p(o_ptr)) ++index;
					break;
			}					
			assign.assignType = ASSIGN_TYPE_ICON;
			assign.icon.type = alternatePtr->icon[index].type;
			assign.icon.index = alternatePtr->icon[index].index;
			assign.icon.ascii = alternatePtr->icon[index].ascii;
		}
	}
	else
	{
		/*
		 * Use ICON_TYPE_NONE icon. This is needed because the "pile" icon is
		 * assigned to object zero.
		 */
		assign.assignType = ASSIGN_TYPE_ICON;
		assign.icon.type = ICON_TYPE_NONE;
		assign.icon.index = 0;
		assign.icon.ascii = -1;
	}

	(*assignPtr) = assign;
}

/*
 * Set the interpreter result with an error if the given index is
 * outside the g_alternate[] array bounds.
 */
static int ValidateAlternate(Tcl_Interp *interp, int alternateIndex)
{
	/* Verify the alternate index */
	if ((alternateIndex < 0) || (alternateIndex >= g_alternate_count))
	{
		/* Get the interpreter result object */
		Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

		/* Set the error */
		Tcl_SetStringObj(resultPtr, format("bad alternate \"%d\": "
			"must be from 0 to %d", alternateIndex,
			g_alternate_count - 1), -1);

		/* Failure */
		return TCL_ERROR;
	}

	/* Sucess */
	return TCL_OK;
}

/*
 * Set the interpreter result with an error if the given index specifies
 * an invalid frame in the given alternate. The alternate must be valid.
 */
static int ValidateFrameAlternate(Tcl_Interp *interp, int alternateIndex,
	int frameIndex)
{
	/* Access the alternate */
	t_alternate *alternatePtr = &g_alternate[alternateIndex];

	/* Verify the frame index */
	if ((frameIndex < 0) || (frameIndex >= alternatePtr->count))
	{
		/* Get the interpreter result object */
		Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

		/* Set the error */
		Tcl_SetStringObj(resultPtr,
			format("bad frame \"%d\" on alternate \"%d\": "
			"must be from 0 to %d", frameIndex, alternateIndex,
			alternatePtr->count - 1), -1);

		/* Failure */
		return TCL_ERROR;
	}

	/* Success */
	return TCL_OK;
}

cptr keyword_alternate_reason[] = {"none", "number", "ident", "feature", NULL};

static int AlternateFromObj(Tcl_Interp *interp, Tcl_Obj *objPtr,
	t_alternate **alternatePtrPtr, int *alternateIndexPtr)
{
	int alternateIndex;

	/* Get the alternate index */
	if (Tcl_GetIntFromObj(interp, objPtr, &alternateIndex) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Verify the alternate index */
	if (ValidateAlternate(interp, alternateIndex) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Access the alternate */
	(*alternatePtrPtr) = &g_alternate[alternateIndex];

	if (alternateIndexPtr)
		(*alternateIndexPtr) = alternateIndex;

	/* Success */
	return TCL_OK;
}

static int AlternateFrameFromObj(Tcl_Interp *interp, Tcl_Obj *objPtr,
	int alternateIndex, int *frameIndexPtr)
{
	int frameIndex;

	/* Get the frame index */
	if (Tcl_GetIntFromObj(interp, objPtr, &frameIndex) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Verify the frame index */
	if (ValidateFrameAlternate(interp, alternateIndex, frameIndex) != TCL_OK)
	{
		return TCL_ERROR;
	}

	(*frameIndexPtr) = frameIndex;

	/* Success */
	return TCL_OK;
}

/*
 * (alternate) assign alternateIndex frameIndex -type iconType -index iconIndex
 */
static int objcmd_alternate_assign(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	t_alternate *alternate_ptr;
	int alternateIndex, frameIndex;
	IconSpec iconSpec;

	/* Get the alternate */
	if (AlternateFromObj(interp, objV[1], &alternate_ptr, &alternateIndex)
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Get the frame index */
	if (AlternateFrameFromObj(interp, objV[2], alternateIndex, &frameIndex)
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Scan the arguments for icon type and index */
	if (Icon_ParseArgs(interp, objc, objv, infoCmd->depth + 3, &iconSpec)
		!= TCL_OK)
	{
		/* Failure */
		return TCL_ERROR;
	}

	/* Set the frame */
	alternate_ptr->icon[frameIndex] = iconSpec;

	if (alternate_ptr->reason == REASON_FEATURE)
	{
		g_icon_map_changed = TRUE;
	}

	/* Success */
	return TCL_OK;
}

/*
 * (alternate) create reason
 */
static int objcmd_alternate_create(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int index;
	t_alternate *alternate_ptr;

	/* Hack - ignore parameter */
	(void) objc;

    if (Tcl_GetIndexFromObj(interp, objV[1],
    	(char **) keyword_alternate_reason,
		(char *) "reason", 0, &index) != TCL_OK)
	{
		return TCL_ERROR;
    }

    /* Insert a new alternate into the global array */
	g_alternate = Array_Insert(g_alternate, &g_alternate_count,
		sizeof(t_alternate), g_alternate_count);

	/* Access the alternate */
	alternate_ptr = &g_alternate[g_alternate_count - 1];

	/* Set the fields, allocate empty frames array */
	alternate_ptr->reason = index;
	alternate_ptr->count = 0;
	alternate_ptr->icon = Array_New(1, sizeof(IconSpec));

	/* Return the index of the new alternate */
	Tcl_SetIntObj(resultPtr, g_alternate_count - 1);

	/* Success */
	return TCL_OK;
}

/*
 * (alternate) delete alternateIndex ?frameIndex?
 */
static int objcmd_alternate_delete(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	t_alternate *alternate_ptr;
	int alternateIndex, frameIndex;

	/* Get the alternate */
	if (AlternateFromObj(interp, objV[1], &alternate_ptr, &alternateIndex)
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	/* A frame index argument was given */
	if (objC == 3)
	{
		/* Get the frame index */
		if (AlternateFrameFromObj(interp, objV[2], alternateIndex, &frameIndex)
			!= TCL_OK)
		{
			return TCL_ERROR;
		}

		/* Delete the frame */
		alternate_ptr->icon = Array_Delete(alternate_ptr->icon,
			&alternate_ptr->count, sizeof(IconSpec), frameIndex);

		/* Done */
		return TCL_OK;
	}

	/* Free the frame array */
	Tcl_Free((char *) alternate_ptr->icon);

	/* Delete the alternate from the global array */
	g_alternate = Array_Delete(g_alternate,
		&g_alternate_count, sizeof(t_alternate), alternateIndex);

	/***
	 *** Now we must go through all the possible assignments and
	 *** update any references to the deleted alternate or any of the
	 *** moved alternates. NOT IMPLEMENTED
	 ***/

	/* Success */
	return TCL_OK;
}

/*
 * (alternate) get alternateIndex ?frameIndex?
 */
static int objcmd_alternate_get(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	t_alternate *alternate_ptr;
	int alternateIndex, frameIndex;
	Tcl_Obj *listObjPtr;
	IconSpec iconSpec;

	/* Get the alternate */
	if (AlternateFromObj(interp, objV[1], &alternate_ptr, &alternateIndex)
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	/* A frame index argument was given */
	if (objC == 3)
	{
		/* Get the frame index */
		if (AlternateFrameFromObj(interp, objV[2], alternateIndex, &frameIndex)
			!= TCL_OK)
		{
			return TCL_ERROR;
		}

		/* Create a new Tcl list object */
		listObjPtr = Tcl_NewListObj(0, NULL);

		/* Get the icon for the frame */
		iconSpec = alternate_ptr->icon[frameIndex];
		
		/* Append icon description to the list */
		Tcl_ListObjAppendElement(interp, listObjPtr,
			Tcl_NewStringObj(format("%s %d",
				g_icon_data[iconSpec.type].desc, iconSpec.index), -1));

		/* Return the list object */
		Tcl_SetObjResult(interp, listObjPtr);

		/* Done */
		return TCL_OK;
	}

	/* Create a new Tcl list object */
	listObjPtr = Tcl_NewListObj(0, NULL);

	/* Check each frame */
	for (frameIndex = 0; frameIndex < alternate_ptr->count; frameIndex++)
	{
		/* Get the icon for the frame */
		iconSpec = alternate_ptr->icon[frameIndex];
		
		/* Append icon description to the list */
		Tcl_ListObjAppendElement(interp, listObjPtr,
			Tcl_NewStringObj(format("%s %d",
				g_icon_data[iconSpec.type].desc, iconSpec.index), -1));
	}

	/* Return the list object */
	Tcl_SetObjResult(interp, listObjPtr);

	/* Success */
	return TCL_OK;
}

/*
 * (alternate) count ?alternateIndex?
 */
static int objcmd_alternate_count(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	t_alternate *alternate_ptr;
	int alternateIndex;

	/* An alternate index was given */
	if (objC == 2)
	{
		/* Get the alternate */
		if (AlternateFromObj(interp, objV[1], &alternate_ptr, &alternateIndex)
			!= TCL_OK)
		{
			return TCL_ERROR;
		}

		/* Return the number of frames in the alternate */
		Tcl_SetIntObj(resultPtr, alternate_ptr->count);

		/* Done */
		return TCL_OK;
	}

	/* Return total number of alternates */
	Tcl_SetIntObj(resultPtr, g_alternate_count);

	/* Success */
	return TCL_OK;
}

/*
 * (alternate) configure alternateIndex ?option? ?value? ?option value ...?
 */
static int objcmd_alternate_configure(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	static cptr configSwitch[] = {"-reason", NULL};
	t_alternate *alternate_ptr;
	int alternateIndex, index;
	Tcl_Obj *CONST *objPtr;

	/* Get the alternate */
	if (AlternateFromObj(interp, objV[1], &alternate_ptr, &alternateIndex)
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	/* "alternate configure $alternateIndex" */
	/* Return all configuration options */
	if (objC == 2)
	{
		/* NOT IMPLEMENTED */
		return TCL_OK;
	}

	/* "alternate configure $alternateIndex $option" */
	/* Return the value of a single option */
	if (objC == 3)
	{
	    if (Tcl_GetIndexFromObj(interp, objV[2], (char **) configSwitch,
			(char *) "switch", 0, &index) != TCL_OK)
		{
			return TCL_ERROR;
	    }

		switch (index)
		{
			case 0: /* -reason */
				Tcl_SetStringObj(resultPtr,
					(char *) keyword_alternate_reason[alternate_ptr->reason],
					-1);
				break;
		}

		/* Done */
		return TCL_OK;
	}

	/* Access the first option/value argument */
	objPtr = objV + 2;
	objC -= 2;

	/* Require even number of arguments */
	if (objC & 1)
	{
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv,
			(char *) "alternateIndex option value ?option value ...?");
		return TCL_ERROR;
	}

	/* Scan all option/value pairs */
	while (objC > 1)
	{
	    if (Tcl_GetIndexFromObj(interp, objPtr[0], (char **) configSwitch,
			(char *) "switch", 0, &index) != TCL_OK)
		{
			return TCL_ERROR;
	    }

		switch (index)
		{
			case 0: /* -reason */
			    if (Tcl_GetIndexFromObj(interp, objPtr[1],
			    	(char **) keyword_alternate_reason,
					(char *) "reason", 0, &alternate_ptr->reason) != TCL_OK)
				{
					return TCL_ERROR;
			    }
				break;
		}

		/* Next option/value pair */
		objPtr += 2;
		objC -= 2;
	}

	/* Success */
	return TCL_OK;
}

/*
 * (alternate) insert alternateIndex frameIndex ?-type iconType -index iconIndex?
 */
static int objcmd_alternate_insert(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	t_alternate *alternate_ptr;
	int alternateIndex, frameIndex;
	IconSpec iconSpec;

	/* Get the alternate */
	if (AlternateFromObj(interp, objV[1], &alternate_ptr, &alternateIndex)
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Get the desired frame index */
	if (Tcl_GetIntFromObj(interp, objV[2], &frameIndex) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Scan the arguments for icon type and index */
	if (Icon_ParseArgs(interp, objc, objv, infoCmd->depth + 3, &iconSpec)
		!= TCL_OK)
	{
		/* Failure */
		return TCL_ERROR;
	}

	/* Append */
	if (frameIndex > alternate_ptr->count)
	{
		frameIndex = alternate_ptr->count;
	}

	/* Prepend */
	else if (frameIndex < 0)
	{
		frameIndex = 0;
	}

	/* Insert a frame */
	alternate_ptr->icon = Array_Insert(alternate_ptr->icon,
		&alternate_ptr->count, sizeof(IconSpec), frameIndex);

	/* Set the frame */
	alternate_ptr->icon[frameIndex] = iconSpec;

	/* Success */
	return TCL_OK;
}

/* (assign) types */
static int objcmd_assign_types(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	int i;
	Tcl_Obj *listObjPtr;

	/* Hack - ignore parameters */
	(void) objc;
	(void) objv;
	(void) clientData;

	listObjPtr = Tcl_NewListObj(0, NULL);
	
	for (i = 0; i < ASSIGN_TYPE_MAX; i++)
	{
		Tcl_ListObjAppendElement(interp, listObjPtr,
			Tcl_NewStringObj(keyword_assign_type[i], -1));
	}

	Tcl_SetObjResult(interp, listObjPtr);

	return TCL_OK;
}


/* (assign) toicon $assign */
static int objcmd_assign_toicon(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	char buf[128], *t;
	IconSpec iconSpec;
	t_assign assign;

	/* Hack - ignore parameter */
	(void) objc;

	t = Tcl_GetString(objV[1]);
	if (assign_parse(interp, &assign, t) != TCL_OK)
	{
		return TCL_ERROR;
	}

	FinalIcon(&iconSpec, &assign, 0, NULL);
	(void) AssignToString_Icon(buf, &assign);
	Tcl_SetResult(interp, buf + 5, TCL_VOLATILE);

	return TCL_OK;
}

/* (assign) validate $assign */
static int objcmd_assign_validate(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	char *t;
	t_assign assign;

	/* Hack - ignore parameter */
	(void) objc;

	t = Tcl_GetString(objV[1]);
    return assign_parse(interp, &assign, t);
}


/* Indexes are EFFECT_SPELL_XXX constants */
cptr keyword_effect_spell[] = {
"arrow",
"missile",
"mana",
"light_weak",
"dark_weak",
"water",
"plasma",
"meteor",
"ice",
"gravity",
"inertia",
"force",
"time",
"acid",
"electricity",
"fire",
"cold",
"poison",
"light",
"dark",
"confusion",
"sound",
"shard",
"nexus",
"nether",
"chaos",
"disenchant",
"rocket",
"nuke",
"death_ray",
"holy_fire",
"hell_fire",
"disintegrate",
"psi",
"psi_drain",
"telekenesis",
"domination",
NULL
};

/* Indexes are EFFECT_AMMO_XXX constants */
cptr keyword_effect_ammo[] = {
"arrow",
"bolt",
NULL
};

cptr keyword_effect_group[] = {"ball", "bolt", "ammo", NULL};

/*
 * (effect) assign $group $effect ?-type iconType -index iconIndex?
 */
static int objcmd_effect_assign(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	t_effect *effect_ptr;
	int effectIndex, effectType;
	IconSpec iconSpec;

	/* Get the effect type */
	if (Tcl_GetIndexFromObj(interp, objV[1], (char **) keyword_effect_group,
		(char *) "type", 0, &effectType) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Access the effect */
	effect_ptr = &g_effect[effectType];

	/* Get the effect keyword */
	if (Tcl_GetIndexFromObj(interp, objV[2], effect_ptr->name,
		(char *) "effect", 0, &effectIndex) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* */
	if (objC > 3)
	{
		/* Scan the arguments for icon type and index */
		if (Icon_ParseArgs(interp, objc, objv, infoCmd->depth + 3, &iconSpec)
			!= TCL_OK)
		{
			return TCL_ERROR;
		}

		/* Set the icon */
		effect_ptr->icon[effectIndex] = iconSpec;

		/* Success */
		return TCL_OK;;
	}

	/* Get the icon */
	iconSpec = effect_ptr->icon[effectIndex];

	/* Return an icon description */
	Tcl_SetStringObj(resultPtr, format("%s %d",
		g_icon_data[iconSpec.type].desc, iconSpec.index), -1);

	/* Success */
	return TCL_OK;;
}

/*
 * (effect) groups
 */
static int objcmd_effect_groups(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
/*	CommandInfo *infoCmd = (CommandInfo *) clientData; */
/*	int objC = objc - infoCmd->depth; */
/*	Tcl_Obj *CONST *objV = objv + infoCmd->depth; */

	int index;
	Tcl_Obj *listObjPtr;

	/* Hack - ignore parameters */
	(void) objc;
	(void) objv;
	(void) clientData;

	/* Create a new Tcl list object */
	listObjPtr = Tcl_NewListObj(0, NULL);

	/* Check each effect keyword */
	for (index = 0; index < EFFECT_MAX; index++)
	{
		/* Append effect keyword as a string object to the list*/
		Tcl_ListObjAppendElement(interp, listObjPtr,
			Tcl_NewStringObj((char *) keyword_effect_group[index], -1));
	}

	/* Return the list */
	Tcl_SetObjResult(interp, listObjPtr);

	/* Success */
	return TCL_OK;;
}

/*
 * (effect) names $group
 */
static int objcmd_effect_names(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	t_effect *effect_ptr;
	int effectType, index;
	Tcl_Obj *listObjPtr;

	/* Hack - ignore parameter */
	(void) objc;

	/* Get the effect type */
	if (Tcl_GetIndexFromObj(interp, objV[1], (char **) keyword_effect_group,
		(char *) "type", 0, &effectType) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Access the effect */
	effect_ptr = &g_effect[effectType];

	/* Create a new Tcl list object */
	listObjPtr = Tcl_NewListObj(0, NULL);

	/* Check each effect keyword */
	for (index = 0; effect_ptr->name[index]; index++)
	{
		/* Append effect keyword as a string object to the list*/
		Tcl_ListObjAppendElement(interp, listObjPtr,
			Tcl_NewStringObj(effect_ptr->name[index], -1));
	}

	/* Return the list */
	Tcl_SetObjResult(interp, listObjPtr);

	/* Success */
	return TCL_OK;;
}

/* (flavor) assign $group $index ?-type $type -index $index -ascii $ascii? */
static int
objcmd_flavor_assign(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	char *flavorName;
	int flavor, flavorIndex;
	IconSpec iconSpec;
	Tcl_HashEntry *hPtr;

	/* Get the specified flavor name */
	flavorName = Tcl_GetStringFromObj(objV[1], NULL);

	/* Lookup the flavor by name */
	hPtr = Tcl_FindHashEntry(&g_flavor_table, flavorName);

	/* The flavor was not found */
	if (hPtr == NULL)
	{
		/* Set the error */
		Tcl_SetResult(interp, format("unknown flavor \"%s\"", flavorName),
			TCL_VOLATILE);

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the g_flavor[] index */
	flavor = (int) Tcl_GetHashValue(hPtr);

	/* Get the desired flavor index */
	if (Tcl_GetIntFromObj(interp, objV[2], &flavorIndex) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Verify the flavor index */
	if ((flavorIndex < 0) || (flavorIndex >= g_flavor[flavor].count))
	{
		/* Set the error */
		Tcl_SetResult(interp, format("bad flavor index \"%d\": must be from 0 to %d",
			flavorIndex, g_flavor[flavor].count - 1), TCL_VOLATILE);

		/* Failure */
		return TCL_ERROR;
	}

	/* Return icon assigned to that flavor index */
	if (objC == 3)
	{
		iconSpec = g_flavor[flavor].icon[flavorIndex];
		if (iconSpec.ascii == -1)
		{
			Tcl_SetResult(interp, format("%s %d",
				g_icon_data[iconSpec.type].desc, iconSpec.index),
				TCL_VOLATILE);
		}
		else
		{
			Tcl_SetResult(interp, format("%s %d %d",
				g_icon_data[iconSpec.type].desc, iconSpec.index,
				iconSpec.ascii), TCL_VOLATILE);
		}

		/* Success */
		return TCL_OK;
	}

	if (Icon_ParseArgs(interp, objc, objv, infoCmd->depth + 3, &iconSpec)
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Finally, assign the icon */
	g_flavor[flavor].icon[flavorIndex] = iconSpec;

	/* Success */
	return TCL_OK;
}

/* (flavor) count $group */
static int
objcmd_flavor_count(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	char *flavorName;
	int flavor;
	Tcl_HashEntry *hPtr;

	/* Hack - ignore parameter */
	(void) objc;

	/* Get the specified flavor name */
	flavorName = Tcl_GetStringFromObj(objV[1], NULL);

	/* Lookup the flavor by name */
	hPtr = Tcl_FindHashEntry(&g_flavor_table, flavorName);

	/* The flavor was not found */
	if (hPtr == NULL)
	{
		/* Set the error */
		Tcl_SetResult(interp, format("unknown flavor \"%s\"", flavorName),
			TCL_VOLATILE);

		/* Failure */
		return TCL_ERROR;
	}

	/* Get the g_flavor[] index */
	flavor = (int) Tcl_GetHashValue(hPtr);

	Tcl_SetObjResult(interp, Tcl_NewIntObj(g_flavor[flavor].count));

	return TCL_OK;
}

/*
 * This command allows us to adjust the tint tables used to darken
 * feature icons according to light radius (see get_display_info()).
 * To specify that a feature should use a tint table, call the "feature
 * configure" command with the "-light tint" option. Using tint tables
 * slows that game down somewhat, so it is usually desireable to
 * use icons for lighting, via "feature configure -light icon".
 *
 * The tint tables adjusted by this routine are in fact initialized
 * using the tk/config/dark file. The values in that file were created
 * on a Macintosh using a somewhat complicated procedure.
 */
static int
objcmd_lighting(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	int index;
	Tcl_Obj *CONST *objPtr;

	int brightness, contrast, radius;
	int do_brightness = 0, do_contrast = 0, do_gamma = 0;
	double gamma;
	t_darken *dark_ptr;

	/* The radius argument must be given */
	radius = -1;
	
	/* Point to the first option/value pair */
	objPtr = objV + 1;
	objC -= 1;

	/* Scan all option/value pairs */
	while (objC > 1)
	{
		static cptr lightOption[] = {"-brightness", "-contrast",
			"-gamma", "-radius", NULL};
			
	    if (Tcl_GetIndexFromObj(interp, objPtr[0], (char **) lightOption,
			(char *) "switch", 0, &index) != TCL_OK)
		{
			return TCL_ERROR;
	    }
	    
		switch (index)
		{
			case 0: /* -brightness */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &brightness)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				if ((brightness < -127) || (brightness > 127))
				{
					/* Set the error */
					Tcl_SetStringObj(resultPtr, format("bad brightness \"%d\":"
						" must be from -127 to +127", brightness), -1);

					/* Failure */
					return TCL_ERROR;
				}
				do_brightness = 1;
				break;
				
			case 1: /* -contrast */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &contrast)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				if ((contrast < -127) || (contrast > 127))
				{
					/* Set the error */
					Tcl_SetStringObj(resultPtr, format("bad contrast \"%d\":"
						" must be from -127 to +127", contrast), -1);

					/* Failure */
					return TCL_ERROR;
				}
				do_contrast = 1;
				break;
				
			case 2: /* -gamma */
				if (Tcl_GetDoubleFromObj(interp, objPtr[1], &gamma)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				if ((gamma < 0.0) || (gamma > 2.0))
				{
					/* Set the error */
					Tcl_SetStringObj(resultPtr, format("bad gamma \"%f\":"
						" must be from 0.0 to 2.0", gamma), -1);

					/* Failure */
					return TCL_ERROR;
				}
				do_gamma = 1;
				break;
				
			case 3: /* -radius */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &radius)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				if ((radius < 1) || (radius > 3))
				{
					/* Set the error */
					Tcl_SetStringObj(resultPtr, format("bad radius \"%d\":"
						" must be from 1 to 3", radius), -1);

					/* Failure */
					return TCL_ERROR;
				}
				break;
		}

		/* Next option/value pair */
		objPtr += 2;
		objC -= 2;
	}

	/* Required number of arguments */
	if ((objC != 0) || (radius == -1))
	{
		goto wrongNumArgs;
	}

	/* Access the lighting info */
	dark_ptr = &g_darken[radius - 1];

	/* Return current values */
	if (!do_brightness && !do_contrast && !do_gamma)
	{
		/* Set result */
		Tcl_SetStringObj(resultPtr, format("%d %d %f", dark_ptr->brightness,
			dark_ptr->contrast, dark_ptr->gamma), -1);

		/* Success */
		return TCL_OK;
	}
	
	/* Reset the table */
	Colormap_One2OneTable(dark_ptr->table);

	/* Do gamma */
	if (!do_gamma) gamma = dark_ptr->gamma;
	if (gamma != 1.0) Colormap_GammaTable(gamma, dark_ptr->table);
	dark_ptr->gamma = gamma;

	/* Do brightness */
	if (!do_brightness) brightness = dark_ptr->brightness;
	if (brightness) Colormap_BrightnessTable(brightness, dark_ptr->table);
	dark_ptr->brightness = brightness;

	/* Do contrast */
	if (!do_contrast) contrast = dark_ptr->contrast;
	if (contrast) Colormap_ContrastTable(contrast, dark_ptr->table);
	dark_ptr->contrast = contrast;

	/* Success */	
	return TCL_OK;

wrongNumArgs:

	/* Set the error */
	Tcl_WrongNumArgs(interp, 1, objv,
		(char *) "-radius radius ?-brightness b -contrast c -gamma g?");

	/* Failure */
	return TCL_ERROR;
}


CommandInit assignCmdInit[] = {
	{0, "alternate", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "assign", 3, 0, "alternateIndex frameIndex ?args ...?", objcmd_alternate_assign, (ClientData) 0},
		{1, "configure", 2, 0, "alternateIndex ?option? ?value? ?option value ...?", objcmd_alternate_configure, (ClientData) 0},
		{1, "count", 1, 2, "?alternateIndex?", objcmd_alternate_count, (ClientData) 0},
		{1, "create", 2, 2, "reason", objcmd_alternate_create, (ClientData) 0},
		{1, "delete", 2, 3, "alternateIndex ?frameIndex?", objcmd_alternate_delete, (ClientData) 0},
		{1, "get", 2, 3, "alternateIndex ?frameIndex?", objcmd_alternate_get, (ClientData) 0},
		{1, "insert", 3, 0, "alternateIndex frameIndex ?args ...?", objcmd_alternate_insert, (ClientData) 0},
	{0, "assign", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "types", 1, 1, NULL, objcmd_assign_types, (ClientData) 0},
		{1, "toicon", 2, 2, "assign", objcmd_assign_toicon, (ClientData) 0},
		{1, "validate", 2, 2, "assign", objcmd_assign_validate, (ClientData) 0},
	{0, "effect", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "assign", 3, 0, "group effect ?args ...?", objcmd_effect_assign, (ClientData) 0},
		{1, "groups", 1, 1, NULL, objcmd_effect_groups, (ClientData) 0},
		{1, "names", 2, 2, "group", objcmd_effect_names, (ClientData) 0},
	{0, "feature", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "lighting", 3, 0, "-radius r ?option? ?value? ?option value ...?", objcmd_lighting, (ClientData) 0},
	{0, "flavor", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "assign", 3, 0, "flavorName flavorIndex ?args ...?", objcmd_flavor_assign, (ClientData) 0},
		{1, "count", 2, 2, "flavorName", objcmd_flavor_count, (ClientData) 0},
	{0, NULL, 0, 0, NULL, NULL, (ClientData) 0}
};


/*
 * Initialize the icon environment. This should be called once with
 * the desired dimensions of the icons to use (16x16, 24x24 or 32x32).
 */
void init_icons(int size, int depth)
{
	int i, n, y, x, y2, x2;
	t_assign assign;
	t_icon_data icon_data, *icon_data_ptr = &icon_data;
	IconSpec iconDefault = {ICON_TYPE_DEFAULT, 0, -1, 0};
	unsigned char *rgb = Colormap_GetRGB();

	/* Initialize the Icon library */
	if (Icon_Init(g_interp, size, depth) != TCL_OK)
	{
		quit(Tcl_GetStringFromObj(Tcl_GetObjResult(g_interp), NULL));
	}

	/*
	 * The TYPE_NONE/"none" type icon is a single masked icon with an empty
	 * mask. It is suitable for equipment displays when no item is present
	 * in a slot.
	 */
	icon_data_ptr->desc = "none";
	icon_data_ptr->icon_count = 1;
	icon_data_ptr->icon_data = (IconPtr) Tcl_Alloc(ICON_LENGTH);
	icon_data_ptr->char_table = NULL;
	icon_data_ptr->font = NULL;
	for (i = 0; i < ICON_LENGTH; i++)
	{
		icon_data_ptr->icon_data[i] = 0x00;
	}

	icon_data_ptr->dynamic = FALSE;
	icon_data_ptr->depth = g_icon_depth;
	icon_data_ptr->bypp = g_pixel_size;
	icon_data_ptr->width = g_icon_size;
	icon_data_ptr->height = g_icon_size;
	icon_data_ptr->pitch = g_icon_size * g_pixel_size;
	icon_data_ptr->length = g_icon_size * g_icon_size * g_pixel_size;
	icon_data_ptr->pixels = g_icon_size * g_icon_size;

	Icon_AddType(icon_data_ptr);

	g_icon_data[ICON_TYPE_NONE].rle_pixel = 0;
	Icon_MakeRLE(&g_icon_data[ICON_TYPE_NONE]);

	/*
	 * The TYPE_BLANK/"blank" icon type is a single black unmasked icon
	 */
	icon_data_ptr->desc = "blank";
	icon_data_ptr->icon_count = 1;
	icon_data_ptr->icon_data = (IconPtr) Tcl_Alloc(ICON_LENGTH);
	icon_data_ptr->char_table = NULL;
	icon_data_ptr->font = NULL;
	for (i = 0; i < ICON_LENGTH; i++)
	{
		if (g_icon_depth != 8)
			icon_data_ptr->icon_data[i] = 0; /* Black (RGB 0,0,0) */
		else
			icon_data_ptr->icon_data[i] = COLORMAP_BLACK;
	}

	icon_data_ptr->dynamic = FALSE;
	icon_data_ptr->depth = g_icon_depth;
	icon_data_ptr->bypp = g_pixel_size;
	icon_data_ptr->width = g_icon_size;
	icon_data_ptr->height = g_icon_size;
	icon_data_ptr->pitch = g_icon_size * g_pixel_size;
	icon_data_ptr->length = g_icon_size * g_icon_size * g_pixel_size;
	icon_data_ptr->pixels = g_icon_size * g_icon_size;

	Icon_AddType(icon_data_ptr);

	/*
	 * The TYPE_DEFAULT/"default" icon type is a single multicolored
	 * unmasked icon. If we see it, it probably means we forgot to
	 * assign an icon to something.
	 */
	icon_data_ptr->desc = "default";
	icon_data_ptr->icon_count = 1;
	icon_data_ptr->icon_data = (IconPtr) Tcl_Alloc(ICON_LENGTH);
	icon_data_ptr->char_table = NULL;
	icon_data_ptr->font = NULL;
	n = 0, y2 = 0;
	for (y = 0; y < 16; y++)
	{
		int dy = 0;
		if (g_icon_size == 24)
		{
			if (!(y & 1)) dy++;
		}
		if (g_icon_size == 32) dy++;
		x2 = 0;
		for (x = 0; x < 16; x++)
		{
			int dx = 0;
			if (g_icon_size == 24)
			{
				if (!(x & 1)) dx++;
			}
			if (g_icon_size == 32) dx++;
			PixelSet_RGB(icon_data_ptr->icon_data + (x2 * g_pixel_size) + (y2 * g_icon_size * g_pixel_size),
				rgb[0], rgb[1], rgb[2], g_pixel_size);
			PixelSet_RGB(icon_data_ptr->icon_data + ((x2 + dx) * g_pixel_size) + (y2 * g_icon_size * g_pixel_size),
				rgb[0], rgb[1], rgb[2], g_pixel_size);
			PixelSet_RGB(icon_data_ptr->icon_data + (x2 * g_pixel_size) + ((y2 + dy) * g_icon_size * g_pixel_size),
				rgb[0], rgb[1], rgb[2], g_pixel_size);
			PixelSet_RGB(icon_data_ptr->icon_data + ((x2 + dx) * g_pixel_size) + ((y2 + dy) * g_icon_size * g_pixel_size),
				rgb[0], rgb[1], rgb[2], g_pixel_size);
			rgb += 3;
#if 0
			icon_data_ptr->icon_data[x2 + y2 * g_icon_size] = n;
			icon_data_ptr->icon_data[(x2 + dx) + y2 * g_icon_size] = n;
			icon_data_ptr->icon_data[x2 + (y2 + dy) * g_icon_size] = n;
			icon_data_ptr->icon_data[(x2 + dx) + (y2 + dy) * g_icon_size] = n;
#endif
			n++;
			x2 += dx ? 2 : 1;
		}
		y2 += dy ? 2 : 1;
	}

	icon_data_ptr->dynamic = FALSE;
	icon_data_ptr->depth = g_icon_depth;
	icon_data_ptr->bypp = g_pixel_size;
	icon_data_ptr->width = g_icon_size;
	icon_data_ptr->height = g_icon_size;
	icon_data_ptr->pitch = g_icon_size * g_pixel_size;
	icon_data_ptr->length = g_icon_size * g_icon_size * g_pixel_size;
	icon_data_ptr->pixels = g_icon_size * g_icon_size;

	Icon_AddType(icon_data_ptr);

	/* Allocate array of t_assign for each monster */
	g_assign[ASSIGN_MONSTER].count = z_info->r_max;
	C_MAKE(g_assign[ASSIGN_MONSTER].assign, z_info->r_max, t_assign);

	/* Allocate array of t_assign for each object */
	g_assign[ASSIGN_OBJECT].count = z_info->k_max;
	C_MAKE(g_assign[ASSIGN_OBJECT].assign, z_info->k_max, t_assign);

	/* Allocate array of t_assign for the character */
	n = 1;
	g_assign[ASSIGN_CHARACTER].count = n;
	C_MAKE(g_assign[ASSIGN_CHARACTER].assign, n, t_assign);

	/* Allocate array of t_assign for each feature */
	g_assign[ASSIGN_FEATURE].count = z_info->f_max;
	C_MAKE(g_assign[ASSIGN_FEATURE].assign, z_info->f_max, t_assign);

	assign.assignType = ASSIGN_TYPE_ICON;
	assign.icon.type = ICON_TYPE_DEFAULT;
	assign.icon.index = 0;
	assign.icon.ascii = -1;

	/* Set default icon for the character */
	for (i = 0; i < g_assign[ASSIGN_CHARACTER].count; i++)
	{
		g_assign[ASSIGN_CHARACTER].assign[i] = assign;
	}

	/* Set default icon for each monster */
	for (i = 0; i < g_assign[ASSIGN_MONSTER].count; i++)
	{
		g_assign[ASSIGN_MONSTER].assign[i] = assign;
	}
	g_assign[ASSIGN_MONSTER].assign[0].icon.type = ICON_TYPE_NONE;

	/* Set default icon for each object */
	for (i = 0; i < g_assign[ASSIGN_OBJECT].count; i++)
	{
		g_assign[ASSIGN_OBJECT].assign[i] = assign;
	}
	g_assign[ASSIGN_OBJECT].assign[0].icon.type = ICON_TYPE_NONE;

	/*
	 * This is an array of t_icon types, one for every grid in
	 * the cave! The icons are only those assigned to features,
	 * never to monsters, objects, or the character. The icons
	 * are not calculated for visibility or light radius. This
	 * array allows different icons to be assigned to the same
	 * feature type. For example, doors are horizontal or vertical,
	 * and have different icons, and the town has a varied
	 * set of icons.
	 */
	for (i = 0; i < MAX_HGT; i++)
	{
		int layer;
		for (layer = 0; layer < ICON_LAYER_MAX; layer++)
			C_MAKE(g_icon_map[layer][i], MAX_WID, t_assign);
	}

	/*
	 * A feature can use lighting in one of three ways:
	 * (1) FT_LIGHT_NONE means ignore lighting
	 * (2) FT_LIGHT_ICON means use a sequence of icons
	 * (3) FT_LIGHT_TINT means use the g_darken[] tint table (slow)
	 */
	C_MAKE(g_feat_lite, z_info->f_max, int);

	/*
	 * When a feature is masked, or a masked icon is drawn on
	 * a feature, we may use the icon assigned to a different feature
	 * as the background.
	 */
	C_MAKE(g_background, z_info->f_max, int);

	/* Set default icon for each feature */
	for (i = 0; i < g_assign[ASSIGN_FEATURE].count; i++)
	{
		g_assign[ASSIGN_FEATURE].assign[i] = assign;
		g_background[i] = i;
		g_feat_lite[i] = FT_LIGHT_NONE;
	}
	g_assign[ASSIGN_FEATURE].assign[FEAT_NONE].icon.type = ICON_TYPE_NONE;

	/* Array of alternates */
	g_alternate = Array_New(1, sizeof(t_alternate));
	g_alternate_count = 0;

	/* Array of effect info */
	g_effect = Array_New(EFFECT_MAX, sizeof(t_effect));

	C_MAKE(g_effect[EFFECT_SPELL_BALL].icon, EFFECT_SPELL_MAX, IconSpec);
	C_MAKE(g_effect[EFFECT_SPELL_BOLT].icon, EFFECT_SPELL_MAX, IconSpec);
	C_MAKE(g_effect[EFFECT_AMMO].icon, EFFECT_AMMO_MAX, IconSpec);

	g_effect[EFFECT_SPELL_BALL].name = (char **) keyword_effect_spell;
	g_effect[EFFECT_SPELL_BOLT].name = (char **) keyword_effect_spell;
	g_effect[EFFECT_AMMO].name = (char **) keyword_effect_ammo;

	/* Set default icon for each effect */
	for (i = 0; i < EFFECT_SPELL_MAX; i++)
	{
		/* Default ball icon */
		g_effect[EFFECT_SPELL_BALL].icon[i] = iconDefault;

		/* Default bolt icon (first of 4) */
		g_effect[EFFECT_SPELL_BOLT].icon[i] = iconDefault;
	}

	/* Set default icon for each effect */
	for (i = 0; i < EFFECT_AMMO_MAX; i++)
	{
		g_effect[EFFECT_AMMO].icon[i] = iconDefault;
	}

	/* Clear the color hash table */
	Palette_ResetHash();

	/* Add some new commands to the global interpreter */
	CommandInfo_Init(g_interp, assignCmdInit, NULL);

	g_assign_none.assignType = ASSIGN_TYPE_ICON;
	g_assign_none.icon.type = ICON_TYPE_NONE;
	g_assign_none.icon.index = 0;
	g_assign_none.icon.ascii = -1;
	
	if (init_widget(g_interp) != TCL_OK)
		quit(Tcl_GetStringFromObj(Tcl_GetObjResult(g_interp), NULL));

	if (CanvasWidget_Init(g_interp) != TCL_OK)
		quit(Tcl_GetStringFromObj(Tcl_GetObjResult(g_interp), NULL));

	/* Hack -- indices for hallucination */
	C_MAKE(g_image_monster, z_info->r_max, int);
	C_MAKE(g_image_object, z_info->k_max, int);

	/* Randomize the hallucination indices */
	angtk_image_reset();
	
	/* Now we can safely use lite_spot() */
	angtk_lite_spot = angtk_lite_spot_real;
}
