/* File: describe.c */

/* Purpose: object recall */
;
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

/*
 * Like strcpy() but returns the length of the string copied
 */
int strcpy_len(char *s1, const char *s2)
{
	int len = 0;
	while ((*s1++ = *s2++)) ++len;
	return len;
}

/* Global pointer into output buffer */
static char *s_buffer;

/*
 * Write a string to the output buffer, plus optional padding
 */
static void roll_off(cptr str, bool pad)
{
	if (pad) s_buffer += strcpy_len(s_buffer, "  ");
	s_buffer += strcpy_len(s_buffer, str);
}

/*
 * Remove characters from the output buffer
 */
static void roll_back(int count)
{
	s_buffer -= count;
}

/*
 * Write array of strings to output buffer
 */
static void roll_em(cptr *vp, int vn, cptr joiner, cptr finish)
{
	int		i;
	
	for (i = 0; i < vn; ++i)
	{
		roll_off(vp[i], FALSE);
		
		if (i < vn - 2)
			roll_off(", ", FALSE);
		else if (i == vn - 2)
			roll_off(joiner ? joiner : " and ", FALSE);
		else
			roll_off(finish ? finish : ".", FALSE);
	}
}

#define TR1_STAT_MASK	\
        (TR1_STR | TR1_INT | TR1_WIS | TR1_DEX | \
         TR1_CON | TR1_CHR)

#define TR2_SUST_MASK	\
        (TR2_SUST_STR | TR2_SUST_INT | TR2_SUST_WIS | TR2_SUST_DEX | \
         TR2_SUST_CON | TR2_SUST_CHR)

/*
 * Write object memory to a buffer
 */
long angtk_describe_object(object_type *o_ptr, char *buf, bool in_store)
{
    u32b f1, f2, f3;

	cptr vp[128];
	int vn;

	bool known = FALSE;
	bool k = FALSE;
	bool hack;
	
	/* Set the global. See roll_off(). */
	s_buffer = buf;

	/* Null-terminate it anyhow */
	s_buffer[0] = '\0';

    /* Extract the (known) flags */
    object_flags_known(o_ptr, &f1, &f2, &f3);

	/* See if the object is "known" */
	if (object_known_p(o_ptr)) known = TRUE;

	/* Mega-Hack -- describe activation */
	if (f3 & (TR3_ACTIVATE))
	{
		roll_off("It can be activated for ", k);
		roll_off(item_activation(o_ptr), FALSE);
		roll_off(" if it is being worn.", FALSE);

		k = TRUE;
	}

	/* Figurines, a hack */
	if (o_ptr->tval == TV_FIGURINE)
	{
		roll_off("It will transform into a pet when thrown.", k);
		k = TRUE;
	}

	/* Bad things */
	vn = 0;
	
	hack = FALSE;

	if (f3 & TR3_TY_CURSE) vp[vn++] = "carries an ancient foul curse";

	if (cursed_p(o_ptr))
	{
		if (f3 & TR3_PERMA_CURSE)		vp[vn++] = "is permanently cursed";
		else if (f3 & TR3_HEAVY_CURSE) 	vp[vn++] = "is heavily cursed";
		else if (known || (o_ptr->ident & (IDENT_SENSE))) vp[vn++] = "is cursed";
	}
	
	if (f3 & TR3_DRAIN_EXP)			vp[vn++] = "drains experience";
	if (f3 & TR3_TELEPORT) 			vp[vn++] = "teleports randomly";
	if (f3 & TR3_AGGRAVATE)			vp[vn++] = "aggravates monsters";
	if (f3 & TR3_NO_MAGIC)    vp[vn++] = "disrupts magic";
	if (f3 & TR3_NO_TELE)     vp[vn++] = "inhibits teleportation";
	
	if (vn)
	{
		roll_off("Unfortunately it ", k);
		roll_em(vp, vn, NULL, NULL);
		
		k = TRUE;
		hack = TRUE;
	}

	if ((f1 & TR1_STAT_MASK) && o_ptr->pval)
	{
		if (hack && (o_ptr->pval < 0))
		{
			roll_back(1);
			roll_off(",", FALSE);
		}
		else
		{
			if (k) roll_off("  ", FALSE);
			k = TRUE;
		}
		
		roll_off((o_ptr->pval > 0) ? "Increases " : (hack ? " and it decreases " : "Decreases "), FALSE);
		
		vn = 0;
		
		if (f1 & TR1_STR) vp[vn++] = "Strength";
		if (f1 & TR1_INT) vp[vn++] = "Intelligence";
		if (f1 & TR1_WIS) vp[vn++] = "Wisdom";
		if (f1 & TR1_DEX) vp[vn++] = "Dexterity";
		if (f1 & TR1_CON) vp[vn++] = "Constitution";
		if (f1 & TR1_CHR) vp[vn++] = "Charisma";

		if (vn == 6)
		{
			roll_off("all stats.", FALSE);
		}
		else
		{
			roll_em(vp, vn, NULL, NULL);
		}
	}

	/* Effects on skills */
	if (o_ptr->pval)
	{
		vn = 0;
	
		if (f1 & TR1_STEALTH)	vp[vn++] = "Stealth";
		if (f1 & TR1_SEARCH) 	vp[vn++] = "Searching";
		if (f1 & TR1_INFRA) 	vp[vn++] = "Infra-vision";
		if (f1 & TR1_BLOWS) 	vp[vn++] = "number of attacks";
		if (f1 & TR1_SPEED) 	vp[vn++] = "speed";
	
		if (vn)
		{
			roll_off(o_ptr->pval > 0 ? "Improves " : "Hinders ", k);
			roll_em(vp, vn, NULL, NULL);
			k = TRUE;
		}
	}

	/* Criticals */
	vn = 0;
	
	hack = FALSE;
	
	if (f1 & TR1_BRAND_ACID) 	vp[vn++] = "acid";
	if (f1 & TR1_BRAND_ELEC)	vp[vn++] = "electricity";
	if (f1 & TR1_BRAND_FIRE) 	vp[vn++] = "fire";
	if (f1 & TR1_BRAND_COLD)	vp[vn++] = "frost";
	if (f1 & TR1_BRAND_POIS)	vp[vn++] = "poison";

	if (vn)
	{
		roll_off("Does extra damage from ", k);
		roll_em(vp, vn, NULL, NULL);

		k = TRUE;
		hack = TRUE;
	}

	if (f2 & TR2_THROW)
	{
		if (k) roll_off("  ", FALSE);
		roll_off("It is perfectly balanced for throwing.", FALSE);

		k = TRUE;
		hack = TRUE;
	}

	/* Misc nastiness */
	vn = 0;

	if (f1 & TR1_CHAOTIC) vp[vn++] = "produces chaotic effects";
	if (f1 & TR1_VAMPIRIC) vp[vn++] = "drains life from your foes";
	if (f1 & TR1_IMPACT) vp[vn++] = "causes earthquakes";
	if (f1 & TR1_VORPAL) vp[vn++] = "cuts with supernatural sharpness";

	if (vn)
	{
		roll_off("It ", k);
		roll_em(vp, vn, NULL, NULL);

		k = TRUE;
		hack = TRUE;
	}

	/* What does it slay? */
	vn = 0;
	
	if (f1 & (TR1_KILL_DRAGON | TR1_SLAY_DRAGON)) 	vp[vn++] = "dragons";
	if (f1 & TR1_SLAY_ORC)   	vp[vn++] = "orcs";
	if (f1 & TR1_SLAY_TROLL) 	vp[vn++] = "trolls";
	if (f1 & TR1_SLAY_GIANT) 	vp[vn++] = "giants";
	if (f1 & TR1_SLAY_DEMON) 	vp[vn++] = "demons";
	if (f1 & TR1_SLAY_UNDEAD) 	vp[vn++] = "undead";
	if (f1 & TR1_SLAY_ANIMAL) 	vp[vn++] = "animals";
	if (f1 & TR1_SLAY_EVIL) 	vp[vn++] = "evil";

	if (vn)
	{
		roll_off("Especially deadly against ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* Sustain stats? */
	if (f2 & TR2_SUST_MASK)
	{
		roll_off("Sustains ", k);
				
		vn = 0;
		
		if (f2 & TR2_SUST_STR) vp[vn++] = "Strength";
		if (f2 & TR2_SUST_INT) vp[vn++] = "Intelligence";
		if (f2 & TR2_SUST_WIS) vp[vn++] = "Wisdom";
		if (f2 & TR2_SUST_DEX) vp[vn++] = "Dexterity";
		if (f2 & TR2_SUST_CON) vp[vn++] = "Constitution";
		if (f2 & TR2_SUST_CHR) vp[vn++] = "Charisma";

		if (vn == 6)
		{
			roll_off("all stats.", FALSE);
		}
		else
		{
			roll_em(vp, vn, NULL, NULL);
		}

		k = TRUE;
	}

	/* Immunities */
	vn = 0;
	
	if (f2 & TR2_IM_ACID) 		vp[vn++] = "acid";
	if (f2 & TR2_IM_ELEC) 		vp[vn++] = "electricity";
	if (f2 & TR2_IM_FIRE) 		vp[vn++] = "fire";
	if (f2 & TR2_IM_COLD) 		vp[vn++] = "cold";
	if (f2 & TR2_RES_BLIND) 	vp[vn++] = "blindness";
	if (f2 & TR2_FREE_ACT)		vp[vn++] = "paralysis";
	if (f2 & TR2_RES_FEAR)		vp[vn++] = "fear";

	if (vn)
	{
		roll_off("Provides immunity to ", k);
		k = TRUE;

		roll_em(vp, vn, NULL, NULL);
	}

	/* Resistances */
	vn = 0;

	if (f2 & TR2_RES_ACID) 		vp[vn++] = "acid";
	if (f2 & TR2_RES_ELEC) 		vp[vn++] = "electricity";
	if (f2 & TR2_RES_FIRE) 		vp[vn++] = "fire";
	if (f2 & TR2_RES_COLD) 		vp[vn++] = "cold";
	if (f2 & TR2_RES_POIS) 		vp[vn++] = "poison";

	if (f2 & TR2_RES_LITE) 		vp[vn++] = "light";
	if (f2 & TR2_RES_DARK)		vp[vn++] = "dark";
	
	if (f2 & TR2_RES_CONF) 		vp[vn++] = "confusion";
	if (f2 & TR2_RES_SOUND) 	vp[vn++] = "sound";
	if (f2 & TR2_RES_SHARDS) 	vp[vn++] = "shards";

	if (f2 & TR2_RES_NETHER) 	vp[vn++] = "nether";
	if (f2 & TR2_RES_NEXUS) 	vp[vn++] = "nexus";
	if (f2 & TR2_RES_CHAOS) 	vp[vn++] = "chaos";
	if (f2 & TR2_RES_DISEN)		vp[vn++] = "disenchantment";
	if (f2 & TR2_HOLD_LIFE)		vp[vn++] = "life draining";

	if (vn)
	{
		roll_off("Provides resistance to ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* What's cool? */
	vn = 0;
	
	if (f3 & TR3_FEATHER) 		vp[vn++] = "feather falling";
	if (f3 & TR3_SEE_INVIS) 	vp[vn++] = "see invisible";
	if (f3 & TR3_TELEPATHY) 	vp[vn++] = "telepathy";
	if (f3 & TR3_SLOW_DIGEST) 	vp[vn++] = "slow digestion";
	if (f3 & TR3_REGEN) 		vp[vn++] = "regeneration";

	if (vn)
	{
		roll_off("Enables ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* Cool too */
	vn = 0;

	if (f2 & TR2_REFLECT) vp[vn++] = "reflects missiles";
	if (f3 & TR3_SH_FIRE) vp[vn++] = "radiates fire";
	if (f3 & TR3_SH_ELEC) vp[vn++] = "radiates electricity";

	if (vn)
	{
		roll_off("It ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}
#
	/* Powerful bows */
	vn = 0;
	
	if (f3 & TR3_XTRA_MIGHT) 	vp[vn++] = "with extra might";
	if (f3 & TR3_XTRA_SHOTS) 	vp[vn++] = "excessively fast";

	if (vn)
	{
		roll_off("Fires missiles ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* Misc */
	if (f3 & TR3_BLESSED)
	{
		roll_off("It has been blessed by the gods.", k);
		k = TRUE;
	}

	/* Hack -- describe lite's */
	if (o_ptr->tval == TV_LITE)
	{
		if (artifact_p(o_ptr))
		{
			roll_off("It provides light (radius 3) forever.", k);
		}
		else if (o_ptr->sval == SV_LITE_LANTERN)
		{
			roll_off("It provides light (radius 2) when fueled.", k);
		}
		else
		{
			roll_off("It provides light (radius 1) when fueled.", k);
		}

		k = TRUE;
	}

	/* Permanent light source */
	if (f3 & TR3_LITE)
	{
		roll_off("It provides light (radius 1) forever.", k);
		k = TRUE;
	}

	if (f1 & TR1_TUNNEL)
	{
		roll_off("It is ", k);
		roll_off((o_ptr->pval >= 0) ? "an effective" : "a useless", FALSE);
		roll_off(" digging tool.", FALSE);
		k = TRUE;
	}

	/* What does the object resist? */
	vn = 0;
	
	if (f3 & TR3_IGNORE_ACID) 	vp[vn++] = "acid";
	if (f3 & TR3_IGNORE_ELEC) 	vp[vn++] = "electricity";
	if (f3 & TR3_IGNORE_FIRE) 	vp[vn++] = "fire";
	if (f3 & TR3_IGNORE_COLD) 	vp[vn++] = "cold";

	if (vn)
	{
		roll_off("It cannot be harmed by ", k);
		roll_em(vp, vn, " or ", NULL);
		k = TRUE;
	}
	
	/* Permanently identified? */
	if (o_ptr->ident & IDENT_MENTAL)
	{
		roll_off("It is permanently identified.", k);
		k = TRUE;
	}

	/* Store bought */
	if (!in_store && (o_ptr->ident & IDENT_STOREB))
	{
		roll_off("It was purchased.", k);
		k = TRUE;
	}

	return s_buffer - buf;
}


/*
 * Hack -- Create a "forged" artifact
 * Taken from wizard1.c.
 */
bool make_fake_artifact(object_type *o_ptr, int name1)
{
	int i;

	artifact_type *a_ptr = &a_info[name1];


	/* Ignore "empty" artifacts */
	if (!a_ptr->name) return FALSE;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	if (!i) return (FALSE);

	/* Create the artifact */
	object_prep(o_ptr, i);

	/* Save the name */
	o_ptr->name1 = name1;

	/* Extract the fields */
	o_ptr->pval = a_ptr->pval;
	o_ptr->ac = a_ptr->ac;
	o_ptr->dd = a_ptr->dd;
	o_ptr->ds = a_ptr->ds;
	o_ptr->to_a = a_ptr->to_a;
	o_ptr->to_h = a_ptr->to_h;
	o_ptr->to_d = a_ptr->to_d;
	o_ptr->weight = a_ptr->weight;

	/* Hack -- acquire "cursed" flag */
	if (a_ptr->flags3 & TR3_CURSED)
		o_ptr->ident |= (IDENT_CURSED);

	/* Success */
	return (TRUE);
}

/*
 * Set a field of a Tcl array variable
 */
int SetArrayValueChar(char *varName, char *field, char value)
{
	char string[20];

	(void) sprintf(string, "%c", value);
	if (Tcl_SetVar2(g_interp, varName, field, string, TCL_LEAVE_ERR_MSG)
		== NULL)
	{
		return TCL_ERROR;
	}
	return TCL_OK;
}

/*
 * Set a field of a Tcl array variable
 */
int SetArrayValueLong(char *varName, char *field, long value)
{
	char string[20];

	(void) sprintf(string, "%ld", value);
	if (Tcl_SetVar2(g_interp, varName, field, string, TCL_LEAVE_ERR_MSG)
		== NULL)
	{
		return TCL_ERROR;
	}
	return TCL_OK;
}

/*
 * Set a field of a Tcl array variable
 */
int SetArrayValueString(char *varName, char *field, char *value)
{
	if (Tcl_SetVar2(g_interp, varName, field, value, TCL_LEAVE_ERR_MSG)
		== NULL)
	{
		return TCL_ERROR;
	}
	return TCL_OK;
}

/*
 * Dump object info into a Tcl array variable
 */
int DumpObjectInfo(object_type *o_ptr, char *varName)
{
	int known;
	u32b f1, f2, f3;
	char *note;

	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3);

	/* Known? */
	known = object_known_p(o_ptr) ? 1 : 0;

	if (SetArrayValueLong(varName, "known", known) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Some fields are returned only if the object is "known" */
	if (known)
	{
		if (SetArrayValueLong(varName, "activate",
			(f3 & (TR3_ACTIVATE)) != 0) != TCL_OK)
		{
			return TCL_ERROR;
		}
		if (SetArrayValueLong(varName, "artifact", artifact_p(o_ptr) ? 1 : 0)
			!= TCL_OK)
		{
			return TCL_ERROR;
		}
		if (SetArrayValueLong(varName, "pval", o_ptr->pval) != TCL_OK)
		{
			return TCL_ERROR;
		}
		if (SetArrayValueLong(varName, "to_a", o_ptr->to_a) != TCL_OK)
		{
			return TCL_ERROR;
		}
		if (SetArrayValueLong(varName, "to_d", o_ptr->to_d) != TCL_OK)
		{
			return TCL_ERROR;
		}
		if (SetArrayValueLong(varName, "to_h", o_ptr->to_h) != TCL_OK)
		{
			return TCL_ERROR;
		}
		if (SetArrayValueLong(varName, "timeout", o_ptr->timeout) != TCL_OK)
		{
			return TCL_ERROR;
		}
	}

	if (SetArrayValueLong(varName, "ac", o_ptr->ac) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if (SetArrayValueLong(varName, "dd", o_ptr->dd) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if (SetArrayValueLong(varName, "ds", o_ptr->ds) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if (SetArrayValueLong(varName, "k_idx", o_ptr->k_idx) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if (SetArrayValueLong(varName, "number", o_ptr->number) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if (SetArrayValueLong(varName, "sval", o_ptr->sval) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if (SetArrayValueLong(varName, "tval", o_ptr->tval) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if (SetArrayValueLong(varName, "weight", o_ptr->weight) != TCL_OK)
	{
		return TCL_ERROR;
	}

	note = "";
	if (get_user_inscription(o_ptr))
	{
		note = (char *) quark_str(get_user_inscription(o_ptr));
	}
	if (ExtToUtf_SetArrayValueString(varName, "note", note) != TCL_OK)
	{
		return TCL_ERROR;
	}

	return TCL_OK;
}

/*
 * Get verbose info for an inventory item. This should be combined
 * with the "angband inventory" command. The main difference is it
 * returns more info.
 */
int
objcmd_inveninfo(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	int i_idx;
	char *varName;
	object_type *o_ptr;

    if (Tcl_GetIntFromObj(interp, objV[1], &i_idx) != TCL_OK)
    {
		return TCL_ERROR;
    }

	/*** Verify index, existing object ***/

	/* Get the array variable name to dump results in */
	varName = Tcl_GetStringFromObj(objV[2], NULL);

	/* Grab the item */
	o_ptr = &inventory[i_idx];

	return DumpObjectInfo(o_ptr, varName);
}

/* Textual name of each equipment slot */
cptr keyword_slot[] = {
	"INVEN_WIELD", "INVEN_BOW", "INVEN_LEFT", "INVEN_RIGHT", "INVEN_NECK",
	"INVEN_LITE", "INVEN_BODY", "INVEN_OUTER", "INVEN_ARM", "INVEN_HEAD",
	"INVEN_HANDS", "INVEN_FEET",
	NULL
};

/*
 * Get verbose info for an equipment item. This should be combined
 * with the "angband equipment" command. The main difference is it
 * returns more info.
 */
int
objcmd_equipinfo(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	int i_idx;
	char *varName;
	object_type *o_ptr;

	/* Get a numerical index or slot name */
    if (Tcl_GetIntFromObj(interp, objV[1], &i_idx) != TCL_OK)
    {
		Tcl_ResetResult(interp);
		if (Tcl_GetIndexFromObj(interp, objV[1], (char **) keyword_slot,
			"slot", 0, &i_idx) != TCL_OK)
		{
			return TCL_ERROR;
		}
    }

	/*** Verify index, existing object ***/

	i_idx += INVEN_WIELD;

	/* Get the array variable name to dump results in */
	varName = Tcl_GetStringFromObj(objV[2], NULL);

	/* Grab the item */
	o_ptr = &inventory[i_idx];

	return DumpObjectInfo(o_ptr, varName);
}

/*
 * Returns a Tcl list of object flag keywords
 */
Tcl_Obj *dump_object_flags(Tcl_Interp *interp, object_type *o_ptr)
{
	int i, j, num = 0;
	u32b f[4];
	Tcl_Obj *listObjPtr;
	cptr *k_info_flags[4], info[96];

	listObjPtr = Tcl_NewListObj(0, NULL);

	/* Return empty list for empty objects */
	if (!o_ptr->k_idx) return listObjPtr;

	/* Known flags */
	object_flags_known(o_ptr, &f[1], &f[2], &f[3]);

	/* Hack -- Artifact lights give permanent light */
	if (artifact_p(o_ptr) && (o_ptr->tval == TV_LITE))
	{
		f[3] |= TR3_LITE;
	}

	/*
	 * Hack -- Some items seem to get a "temporary" curse, but
	 * the "cursed" flag is not set. So if it is marked cursed and
	 * known/sensed then I add the "cursed" flag.
	 */
	if (cursed_p(o_ptr) && (object_known_p(o_ptr) ||
		(o_ptr->ident & (IDENT_SENSE))))
	{
		f[3] |= TR3_CURSED;
	}

	k_info_flags[1] = k_info_flags1;
	k_info_flags[2] = k_info_flags2;
	k_info_flags[3] = k_info_flags3;

	for (i = 1; i < 4; i++)
	{
		for (j = 0; j < 32; j++)
		{
			if (f[i] & (1L << j))
			{
				info[num++] = k_info_flags[i][j];
			}
		}
	}

	for (i = 0; i < num; i++)
	{
		Tcl_ListObjAppendElement(interp, listObjPtr,
			Tcl_NewStringObj((char *) info[i], -1));
	}

	return listObjPtr;
}

/*
 * Returns a Tcl list of object flag keywords for the character
 */
Tcl_Obj *dump_player_flags(Tcl_Interp *interp)
{
	int i, j, num = 0;
	u32b f[4];
	Tcl_Obj *listObjPtr;
	cptr *k_info_flags[4], info[96];

	listObjPtr = Tcl_NewListObj(0, NULL);

	/* Clear */
	f[1] = f[2] = f[3] = 0L;

	player_flags(&f[1], &f[2], &f[3]);

	k_info_flags[1] = k_info_flags1;
	k_info_flags[2] = k_info_flags2;
	k_info_flags[3] = k_info_flags3;

	for (i = 1; i < 4; i++)
	{
		for (j = 0; j < 32; j++)
		{
			if (f[i] & (1L << j))
			{
				info[num++] = k_info_flags[i][j];
			}
		}
	}

	for (i = 0; i < num; i++)
	{
		Tcl_ListObjAppendElement(interp, listObjPtr,
			Tcl_NewStringObj((char *) info[i], -1));
	}

	return listObjPtr;
}
