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


char *AssignToString_Icon(char *buf, t_assign_icon *assign)
{
	if (assign->ascii == -1)
	{
		(void) sprintf(buf, "icon %s %d",
			g_icon_data[assign->type].desc,
			assign->index);
	}
	else
	{
		(void) sprintf(buf,"icon %s %d %d",
			g_icon_data[assign->type].desc,
			assign->index, assign->ascii);
	}

	return buf;
}

static int StringToAssign_Icon(Tcl_Interp *interp, t_assign_icon *assignPtr, cptr desc)
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

	assignPtr->type = iconSpec.type;
	assignPtr->index = iconSpec.index;
	assignPtr->ascii = iconSpec.ascii;

	return TCL_OK;
}


cptr keyword_assign_type[] = {"icon", NULL};

int assign_parse(Tcl_Interp *interp, t_assign_icon *assignPtr, cptr desc)
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
	if (Tcl_GetIndexFromObj(interp, objPtr, keyword_assign_type,
		"option", 0, &assignType) != TCL_OK)
	{
		Tcl_DecrRefCount(objPtr);
		return TCL_ERROR;
	}
	Tcl_DecrRefCount(objPtr);

	return StringToAssign_Icon(interp, assignPtr, desc);
}

char *assign_print2(char *buf, int assignType)
{
	t_assign_icon *assignPtr = &g_assign[assignType].assign[0];
	return AssignToString_Icon(buf, assignPtr);
}

char *assign_print_object(char *buf, object_type *o_ptr)
{
	t_assign_icon assign;
	get_object_assign(&assign, o_ptr);
	return AssignToString_Icon(buf, &assign);
}

/*
 * Get the assignment for the given object. Handle "empty" objects and
 * resolve alternate assignments.
 */
void get_object_assign(t_assign_icon *assignPtr, object_type *o_ptr)
{
	t_assign_icon assign;

	if (o_ptr->k_idx)
	{
		assign = g_assign[ASSIGN_OBJECT].assign[o_ptr->k_idx];
	}
	else
	{
		/*
		 * Use ICON_TYPE_NONE icon. This is needed because the "pile" icon is
		 * assigned to object zero.
		 */
		assign.type = ICON_TYPE_NONE;
		assign.index = 0;
		assign.ascii = -1;
	}

	(*assignPtr) = assign;
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
	t_assign_icon assign;

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
	t_assign_icon assign;

	/* Hack - ignore parameter */
	(void) objc;

	t = Tcl_GetString(objV[1]);
    return assign_parse(interp, &assign, t);
}


CommandInit assignCmdInit[] = {
	{0, "assign", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "types", 1, 1, NULL, objcmd_assign_types, (ClientData) 0},
		{1, "toicon", 2, 2, "assign", objcmd_assign_toicon, (ClientData) 0},
		{1, "validate", 2, 2, "assign", objcmd_assign_validate, (ClientData) 0},
	{0, NULL, 0, 0, NULL, NULL, (ClientData) 0}
};


/*
 * Initialize the icon environment. This should be called once with
 * the desired dimensions of the icons to use (16x16, 24x24 or 32x32).
 */
void init_icons(int size, int depth)
{
	int i, n, y, x, y2, x2;
	t_assign_icon assign;
	t_icon_data icon_data, *icon_data_ptr = &icon_data;
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
	C_MAKE(icon_data_ptr->icon_data, ICON_LENGTH, byte);
	icon_data_ptr->char_table = NULL;
	icon_data_ptr->font = NULL;
	for (i = 0; i < ICON_LENGTH; i++)
	{
		icon_data_ptr->icon_data[i] = 0x00;
	}

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
	C_MAKE(icon_data_ptr->icon_data, ICON_LENGTH, byte);
	icon_data_ptr->char_table = NULL;
	icon_data_ptr->font = NULL;
	for (i = 0; i < ICON_LENGTH; i++)
	{
		if (g_icon_depth != 8)
			icon_data_ptr->icon_data[i] = 0; /* Black (RGB 0,0,0) */
		else
			icon_data_ptr->icon_data[i] = COLORMAP_BLACK;
	}

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
	C_MAKE(icon_data_ptr->icon_data, ICON_LENGTH, byte);
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

			n++;
			x2 += dx ? 2 : 1;
		}
		y2 += dy ? 2 : 1;
	}

	icon_data_ptr->depth = g_icon_depth;
	icon_data_ptr->bypp = g_pixel_size;
	icon_data_ptr->width = g_icon_size;
	icon_data_ptr->height = g_icon_size;
	icon_data_ptr->pitch = g_icon_size * g_pixel_size;
	icon_data_ptr->length = g_icon_size * g_icon_size * g_pixel_size;
	icon_data_ptr->pixels = g_icon_size * g_icon_size;

	Icon_AddType(icon_data_ptr);

	/* Allocate array of t_assign_icon for each monster */
	g_assign[ASSIGN_MONSTER].count = z_info->r_max;
	C_MAKE(g_assign[ASSIGN_MONSTER].assign, z_info->r_max, t_assign_icon);

	/* Allocate array of t_assign_icon for each object */
	g_assign[ASSIGN_OBJECT].count = z_info->k_max;
	C_MAKE(g_assign[ASSIGN_OBJECT].assign, z_info->k_max, t_assign_icon);

	/* Allocate array of t_assign_icon for the character */
	n = 1;
	g_assign[ASSIGN_CHARACTER].count = n;
	C_MAKE(g_assign[ASSIGN_CHARACTER].assign, n, t_assign_icon);

	/* Allocate array of t_assign_icon for each feature */
	g_assign[ASSIGN_FEATURE].count = z_info->f_max;
	C_MAKE(g_assign[ASSIGN_FEATURE].assign, z_info->f_max, t_assign_icon);

	assign.type = ICON_TYPE_DEFAULT;
	assign.index = 0;
	assign.ascii = -1;

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
	g_assign[ASSIGN_MONSTER].assign[0].type = ICON_TYPE_NONE;

	/* Set default icon for each object */
	for (i = 0; i < g_assign[ASSIGN_OBJECT].count; i++)
	{
		g_assign[ASSIGN_OBJECT].assign[i] = assign;
	}
	g_assign[ASSIGN_OBJECT].assign[0].type = ICON_TYPE_NONE;

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
			C_MAKE(g_icon_map[layer][i], MAX_WID, t_assign_icon);
	}

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
	}
	g_assign[ASSIGN_FEATURE].assign[FEAT_NONE].type = ICON_TYPE_NONE;

	/* Clear the color hash table */
	Palette_ResetHash();

	/* Add some new commands to the global interpreter */
	CommandInfo_Init(g_interp, assignCmdInit, NULL);

	g_assign_none.type = ICON_TYPE_NONE;
	g_assign_none.index = 0;
	g_assign_none.ascii = -1;
	
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
