/* File: struct.c */

/* Purpose: reading/writing game arrays */

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

static Tcl_Obj *struct_get_ARRAY_tval(Tcl_Interp *interp, t_field *info,
	unsigned char *elem, int elemIndex)
{
	cptr cstr;
	byte tval = *(byte *)(elem + info->fOffset);

	/* Hack - ignore unused parameters */
	(void) interp;
	(void) elemIndex;

	(void) angtk_tval_string(&cstr, tval);
	return Tcl_NewStringObj(cstr, -1);
}

#define NAME_TEXT_TYPE FLD_INT32U


static t_field struct_monster_type[] = {

{ FIELD_DESC(r_idx, monster_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(fy, monster_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(fx, monster_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(hp, monster_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(maxhp, monster_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(csleep, monster_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(mspeed, monster_type), FLD_INT8U, EDIT_YES, 0, 199, NULL},
{ FIELD_DESC(energy, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX, NULL},
{ FIELD_DESC(stunned, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX, NULL},
{ FIELD_DESC(confused, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX, NULL},
{ FIELD_DESC(monfear, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX, NULL},
{ FIELD_DESC(invulner, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX, NULL},
{ FIELD_DESC(cdis, monster_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(mflag, monster_type), FLD_FLAGS8, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(ml, monster_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(hold_o_idx, monster_type), FLD_INT16, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(smart, monster_type), FLD_FLAGS32, EDIT_YES, 0, 0, NULL},


{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};


static t_field struct_object_type[] = {

{ FIELD_DESC(k_idx, object_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(iy, object_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(ix, object_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(tval, object_type), FLD_INT8U, EDIT_CALLBACK, 0, 0, (struct_return) struct_get_ARRAY_tval },
{ FIELD_DESC(sval, object_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(pval, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX, NULL},
{ FIELD_DESC(discount, object_type), FLD_INT8U, EDIT_YES, 0, 100, NULL},
{ FIELD_DESC(number, object_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX, NULL },
{ FIELD_DESC(weight, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX, NULL },
{ FIELD_DESC(to_h, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX, NULL },
{ FIELD_DESC(to_d, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX, NULL },
{ FIELD_DESC(to_a, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX, NULL },
{ FIELD_DESC(ac, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX, NULL },
{ FIELD_DESC(dd, object_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX, NULL },
{ FIELD_DESC(ds, object_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX, NULL },
{ FIELD_DESC(timeout, object_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL },
{ FIELD_DESC(info, object_type), FLD_FLAGS8, EDIT_YES, 0, 0, NULL },
{ FIELD_DESC(inscription, object_type), FLD_INT16U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(feeling, object_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(next_o_idx, object_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};


/* option ?arg ...? */
static int struct_info_object_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, object_type *o_ptr,
	int o_idx)
{
	static cptr infoOption[] = {"icon", "memory", "name", "desc", "known",
		"char", "attr", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	Tcl_Obj *resultObjPtr = NULL;
	int index;

	char buf[128], *buffer, o_name[256];
	object_type fake = (*o_ptr);
	
	/* Hack - Ignore unused parameters */
	(void) objc;
	(void) o_idx;
	(void) typePtr;

    if (Tcl_GetIndexFromObj(interp, objV[0], (char **) infoOption, (char *) "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
	}
	switch (index)
	{
		case 0: /* icon */
			resultObjPtr = Tcl_NewStringObj(assign_print_object(buf, o_ptr), -1);
			break;

		case 1: /* memory */
			buffer = Tcl_Alloc(4 * 1024L);
			(void) angtk_describe_object(&fake, buffer, FALSE);
			resultObjPtr = ExtToUtf_NewStringObj(buffer, -1);
			Tcl_Free(buffer);
			break;

		case 2: /* name */
			object_desc_store(o_name, o_ptr, TRUE, 3, 256);
			resultObjPtr = ExtToUtf_NewStringObj(o_name, -1);
			break;

		case 3: /* desc */
			object_desc(o_name, o_ptr, TRUE, 3, 256);
			resultObjPtr = ExtToUtf_NewStringObj(o_name, -1);
			break;

		case 4: /* known */
			resultObjPtr = Tcl_NewBooleanObj(object_known_p(o_ptr));
			break;

		case 5: /* char */
			buf[0] = object_char(o_ptr);
			resultObjPtr = Tcl_NewStringObj(buf, 1);
			break;

		case 6: /* attr */
			resultObjPtr = Tcl_NewIntObj(object_attr(o_ptr));
			break;
	}

	Tcl_SetObjResult(interp, resultObjPtr);

	return TCL_OK;
}

/* option ?arg ...? */
static int struct_info_monster_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, monster_type *m_ptr,
	int m_idx)
{
	static cptr infoOption[] = {"icon", "memory", "name", "look_mon_desc",
		"monster_desc", "friend", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	Tcl_Obj *resultObjPtr = NULL;
	int index;

	char buf[128], *buffer, m_name[80];
	long length;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Hack - ignore unused parameters */
	(void) objc;
	(void) typePtr;

    if (Tcl_GetIndexFromObj(interp, objV[0], (char **) infoOption, (char *) "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
	}
	switch (index)
	{
		case 0: /* icon */
			resultObjPtr = Tcl_NewStringObj(assign_print2(buf, ASSIGN_MONSTER,
				m_ptr->r_idx), -1);
			break;

		case 1: /* memory */
			buffer = Tcl_Alloc(4 * 1024L);
			length = angtk_roff(m_ptr->r_idx, buffer);
			resultObjPtr = ExtToUtf_NewStringObj(buffer, length);
			Tcl_Free(buffer);
			break;

		case 2: /* name */
			resultObjPtr = ExtToUtf_NewStringObj(r_name + r_ptr->name, -1);
			break;

		case 3: /* look_mon_desc */
		{
			resultObjPtr = ExtToUtf_NewStringObj((char *) look_mon_desc(m_idx), -1);
			break;
		}

		case 4: /* monster_desc */
			monster_desc(m_name, m_ptr, 0x08, 80);
			resultObjPtr = ExtToUtf_NewStringObj(m_name, -1);
			break;

		case 5: /* friend */
			resultObjPtr = Tcl_NewBooleanObj(!is_hostile(m_ptr));
			break;
	}

	Tcl_SetObjResult(interp, resultObjPtr);

	return TCL_OK;
}

void init_struct(void)
{
	StructType type;

	/* Initialize the struct stuff */
	if (Struct_Init(g_interp) != TCL_OK)
		quit_fmt("Struct_Init() failed: %s", Tcl_GetStringResult(g_interp));

	type.name = "monster_type";
	type.elem = (unsigned char *) m_list;
	type.elem_size = sizeof(monster_type);
	type.max = z_info->m_max;
	type.info = struct_monster_type;
	type.infoProc = (StructInfoProc) struct_info_monster_type;
	type.findProc = NULL;
	type.matchProc = (StructFindMatchProc) NULL;
	Struct_AddType(g_interp, &type);

	type.name = "object_type";
	type.elem = (unsigned char *) o_list;
	type.elem_size = sizeof(object_type);
	type.max = z_info->o_max;
	type.info = struct_object_type;
	type.infoProc = (StructInfoProc) struct_info_object_type;
	type.findProc = NULL /* struct_find_object_type */;
	type.matchProc = NULL /* (StructFindMatchProc) struct_match_object_type */;
	Struct_AddType(g_interp, &type);
}
