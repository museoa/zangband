/* File: describe.c */

/* Purpose: object recall */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"

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

#define TR0_STAT_MASK	\
        (TR0_STR | TR0_INT | TR0_WIS | TR0_DEX | \
         TR0_CON | TR0_CHR)

#define TR1_SUST_MASK	\
        (TR1_SUST_STR | TR1_SUST_INT | TR1_SUST_WIS | TR1_SUST_DEX | \
         TR1_SUST_CON | TR1_SUST_CHR)

/*
 * Write object memory to a buffer
 */
long angtk_describe_object(object_type *o_ptr, char *buf, bool in_store)
{
	cptr vp[128];
	int vn;

	bool known = FALSE;
	bool k = FALSE;
	bool hack;
	
	/* Set the global. See roll_off(). */
	s_buffer = buf;

	/* Null-terminate it anyhow */
	s_buffer[0] = '\0';

	/* See if the object is "known" */
	if (object_known_p(o_ptr)) known = TRUE;

	/* Mega-Hack -- describe activation */
	if (TEST_FLAG(o_ptr->flags, 2, TR2_ACTIVATE))
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

	if (TR_FLAG(o_ptr->flags, 2, TY_CURSE)) vp[vn++] = "carries an ancient foul curse";

	if (cursed_p(o_ptr))
	{
		if (TR_FLAG(o_ptr->flags, 2, PERMA_CURSE))		vp[vn++] = "is permanently cursed";
		else if (TR_FLAG(o_ptr->flags, 2, HEAVY_CURSE)) 	vp[vn++] = "is heavily cursed";
		else if (known || (o_ptr->info & OB_SENSE)) vp[vn++] = "is cursed";
	}
	
	if (TR_FLAG(o_ptr->flags, 2, DRAIN_EXP))			vp[vn++] = "drains experience";
	if (TR_FLAG(o_ptr->flags, 2, TELEPORT)) 			vp[vn++] = "teleports randomly";
	if (TR_FLAG(o_ptr->flags, 2, AGGRAVATE))			vp[vn++] = "aggravates monsters";
	if (TR_FLAG(o_ptr->flags, 2, NO_MAGIC))    vp[vn++] = "disrupts magic";
	if (TR_FLAG(o_ptr->flags, 2, NO_TELE))     vp[vn++] = "inhibits teleportation";
	
	if (vn)
	{
		roll_off("Unfortunately it ", k);
		roll_em(vp, vn, NULL, NULL);
		
		k = TRUE;
		hack = TRUE;
	}

	if ((TR_FLAG(o_ptr->flags, 0, STAT_MASK)) && o_ptr->pval)
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
		
		if (TR_FLAG(o_ptr->flags, 0, STR)) vp[vn++] = "Strength";
		if (TR_FLAG(o_ptr->flags, 0, INT)) vp[vn++] = "Intelligence";
		if (TR_FLAG(o_ptr->flags, 0, WIS)) vp[vn++] = "Wisdom";
		if (TR_FLAG(o_ptr->flags, 0, DEX)) vp[vn++] = "Dexterity";
		if (TR_FLAG(o_ptr->flags, 0, CON)) vp[vn++] = "Constitution";
		if (TR_FLAG(o_ptr->flags, 0, CHR)) vp[vn++] = "Charisma";

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
	
		if (TR_FLAG(o_ptr->flags, 0, STEALTH))	vp[vn++] = "Stealth";
		if (TR_FLAG(o_ptr->flags, 0, SEARCH)) 	vp[vn++] = "Searching";
		if (TR_FLAG(o_ptr->flags, 0, INFRA)) 	vp[vn++] = "Infra-vision";
		if (TR_FLAG(o_ptr->flags, 0, BLOWS)) 	vp[vn++] = "number of attacks";
		if (TR_FLAG(o_ptr->flags, 0, SPEED)) 	vp[vn++] = "speed";
	
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
	
	if (TR_FLAG(o_ptr->flags, 0, BRAND_ACID)) 	vp[vn++] = "acid";
	if (TR_FLAG(o_ptr->flags, 0, BRAND_ELEC))	vp[vn++] = "electricity";
	if (TR_FLAG(o_ptr->flags, 0, BRAND_FIRE)) 	vp[vn++] = "fire";
	if (TR_FLAG(o_ptr->flags, 0, BRAND_COLD))	vp[vn++] = "frost";
	if (TR_FLAG(o_ptr->flags, 0, BRAND_POIS))	vp[vn++] = "poison";

	if (vn)
	{
		roll_off("Does extra damage from ", k);
		roll_em(vp, vn, NULL, NULL);

		k = TRUE;
		hack = TRUE;
	}

	if (TR_FLAG(o_ptr->flags, 1, THROW))
	{
		if (k) roll_off("  ", FALSE);
		roll_off("It is perfectly balanced for throwing.", FALSE);

		k = TRUE;
		hack = TRUE;
	}

	/* Misc nastiness */
	vn = 0;

	if (TR_FLAG(o_ptr->flags, 0, CHAOTIC)) vp[vn++] = "produces chaotic effects";
	if (TR_FLAG(o_ptr->flags, 0, VAMPIRIC)) vp[vn++] = "drains life from your foes";
	if (TR_FLAG(o_ptr->flags, 0, IMPACT)) vp[vn++] = "causes earthquakes";
	if (TR_FLAG(o_ptr->flags, 0, VORPAL)) vp[vn++] = "cuts with supernatural sharpness";

	if (vn)
	{
		roll_off("It ", k);
		roll_em(vp, vn, NULL, NULL);

		k = TRUE;
		hack = TRUE;
	}

	/* What does it slay? */
	vn = 0;
	
	if (TEST_FLAG(o_ptr->flags, 0, TR0_KILL_DRAGON | TR0_SLAY_DRAGON)) 	vp[vn++] = "dragons";
	if (TR_FLAG(o_ptr->flags, 0, SLAY_ORC))   	vp[vn++] = "orcs";
	if (TR_FLAG(o_ptr->flags, 0, SLAY_TROLL)) 	vp[vn++] = "trolls";
	if (TR_FLAG(o_ptr->flags, 0, SLAY_GIANT)) 	vp[vn++] = "giants";
	if (TR_FLAG(o_ptr->flags, 0, SLAY_DEMON)) 	vp[vn++] = "demons";
	if (TR_FLAG(o_ptr->flags, 0, SLAY_UNDEAD)) 	vp[vn++] = "undead";
	if (TR_FLAG(o_ptr->flags, 0, SLAY_ANIMAL)) 	vp[vn++] = "animals";
	if (TR_FLAG(o_ptr->flags, 0, SLAY_EVIL)) 	vp[vn++] = "evil";

	if (vn)
	{
		roll_off("Especially deadly against ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* Sustain stats? */
	if (TR_FLAG(o_ptr->flags, 1, SUST_MASK))
	{
		roll_off("Sustains ", k);
				
		vn = 0;
		
		if (TR_FLAG(o_ptr->flags, 1, SUST_STR)) vp[vn++] = "Strength";
		if (TR_FLAG(o_ptr->flags, 1, SUST_INT)) vp[vn++] = "Intelligence";
		if (TR_FLAG(o_ptr->flags, 1, SUST_WIS)) vp[vn++] = "Wisdom";
		if (TR_FLAG(o_ptr->flags, 1, SUST_DEX)) vp[vn++] = "Dexterity";
		if (TR_FLAG(o_ptr->flags, 1, SUST_CON)) vp[vn++] = "Constitution";
		if (TR_FLAG(o_ptr->flags, 1, SUST_CHR)) vp[vn++] = "Charisma";

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
	
	if (TR_FLAG(o_ptr->flags, 1, IM_ACID)) 		vp[vn++] = "acid";
	if (TR_FLAG(o_ptr->flags, 1, IM_ELEC)) 		vp[vn++] = "electricity";
	if (TR_FLAG(o_ptr->flags, 1, IM_FIRE)) 		vp[vn++] = "fire";
	if (TR_FLAG(o_ptr->flags, 1, IM_COLD)) 		vp[vn++] = "cold";
	if (TR_FLAG(o_ptr->flags, 1, RES_BLIND)) 	vp[vn++] = "blindness";
	if (TR_FLAG(o_ptr->flags, 1, FREE_ACT))		vp[vn++] = "paralysis";
	if (TR_FLAG(o_ptr->flags, 1, RES_FEAR))		vp[vn++] = "fear";

	if (vn)
	{
		roll_off("Provides immunity to ", k);
		k = TRUE;

		roll_em(vp, vn, NULL, NULL);
	}

	/* Resistances */
	vn = 0;

	if (TR_FLAG(o_ptr->flags, 1, RES_ACID)) 		vp[vn++] = "acid";
	if (TR_FLAG(o_ptr->flags, 1, RES_ELEC)) 		vp[vn++] = "electricity";
	if (TR_FLAG(o_ptr->flags, 1, RES_FIRE)) 		vp[vn++] = "fire";
	if (TR_FLAG(o_ptr->flags, 1, RES_COLD)) 		vp[vn++] = "cold";
	if (TR_FLAG(o_ptr->flags, 1, RES_POIS)) 		vp[vn++] = "poison";

	if (TR_FLAG(o_ptr->flags, 1, RES_LITE)) 		vp[vn++] = "light";
	if (TR_FLAG(o_ptr->flags, 1, RES_DARK))		vp[vn++] = "dark";
	
	if (TR_FLAG(o_ptr->flags, 1, RES_CONF)) 		vp[vn++] = "confusion";
	if (TR_FLAG(o_ptr->flags, 1, RES_SOUND)) 	vp[vn++] = "sound";
	if (TR_FLAG(o_ptr->flags, 1, RES_SHARDS)) 	vp[vn++] = "shards";

	if (TR_FLAG(o_ptr->flags, 1, RES_NETHER)) 	vp[vn++] = "nether";
	if (TR_FLAG(o_ptr->flags, 1, RES_NEXUS)) 	vp[vn++] = "nexus";
	if (TR_FLAG(o_ptr->flags, 1, RES_CHAOS)) 	vp[vn++] = "chaos";
	if (TR_FLAG(o_ptr->flags, 1, RES_DISEN))		vp[vn++] = "disenchantment";
	if (TR_FLAG(o_ptr->flags, 1, HOLD_LIFE))		vp[vn++] = "life draining";

	if (vn)
	{
		roll_off("Provides resistance to ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* What's cool? */
	vn = 0;
	
	if (TR_FLAG(o_ptr->flags, 2, FEATHER)) 		vp[vn++] = "feather falling";
	if (TR_FLAG(o_ptr->flags, 2, SEE_INVIS)) 	vp[vn++] = "see invisible";
	if (TR_FLAG(o_ptr->flags, 2, TELEPATHY)) 	vp[vn++] = "telepathy";
	if (TR_FLAG(o_ptr->flags, 2, SLOW_DIGEST)) 	vp[vn++] = "slow digestion";
	if (TR_FLAG(o_ptr->flags, 2, REGEN)) 		vp[vn++] = "regeneration";

	if (vn)
	{
		roll_off("Enables ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* Cool too */
	vn = 0;

	if (TR_FLAG(o_ptr->flags, 1, REFLECT)) vp[vn++] = "reflects missiles";
	if (TR_FLAG(o_ptr->flags, 2, SH_FIRE)) vp[vn++] = "radiates fire";
	if (TR_FLAG(o_ptr->flags, 2, SH_ELEC)) vp[vn++] = "radiates electricity";

	if (vn)
	{
		roll_off("It ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* Powerful bows */
	vn = 0;
	
	if (TR_FLAG(o_ptr->flags, 2, XTRA_MIGHT)) 	vp[vn++] = "with extra might";
	if (TR_FLAG(o_ptr->flags, 2, XTRA_SHOTS)) 	vp[vn++] = "excessively fast";

	if (vn)
	{
		roll_off("Fires missiles ", k);
		roll_em(vp, vn, NULL, NULL);
		k = TRUE;
	}

	/* Misc */
	if (TR_FLAG(o_ptr->flags, 2, BLESSED))
	{
		roll_off("It has been blessed by the gods.", k);
		k = TRUE;
	}

	/* Hack -- describe lite's */
	if (o_ptr->tval == TV_LITE)
	{
		if (TR_FLAG(o_ptr->flags, 2, INSTA_ART))
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
	if (TR_FLAG(o_ptr->flags, 2, LITE))
	{
		roll_off("It provides light (radius 1) forever.", k);
		k = TRUE;
	}

	if (TR_FLAG(o_ptr->flags, 0, TUNNEL))
	{
		roll_off("It is ", k);
		roll_off((o_ptr->pval >= 0) ? "an effective" : "a useless", FALSE);
		roll_off(" digging tool.", FALSE);
		k = TRUE;
	}

	/* What does the object resist? */
	vn = 0;
	
	if (TR_FLAG(o_ptr->flags, 2, IGNORE_ACID)) 	vp[vn++] = "acid";
	if (TR_FLAG(o_ptr->flags, 2, IGNORE_ELEC)) 	vp[vn++] = "electricity";
	if (TR_FLAG(o_ptr->flags, 2, IGNORE_FIRE)) 	vp[vn++] = "fire";
	if (TR_FLAG(o_ptr->flags, 2, IGNORE_COLD)) 	vp[vn++] = "cold";

	if (vn)
	{
		roll_off("It cannot be harmed by ", k);
		roll_em(vp, vn, " or ", NULL);
		k = TRUE;
	}
	
	/* Permanently identified? */
	if (o_ptr->info & OB_MENTAL)
	{
		roll_off("It is permanently identified.", k);
		k = TRUE;
	}

	/* Store bought */
	if (!in_store && (o_ptr->info & OB_STOREB))
	{
		roll_off("It was purchased.", k);
		k = TRUE;
	}

	return s_buffer - buf;
}


/*
 * Set a field of a Tcl array variable
 */
int SetArrayValueChar(cptr varName, cptr field, char value)
{
	char string[20];

	strnfmt(string, 20, "%c", value);
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
int SetArrayValueLong(cptr varName, cptr field, long value)
{
	char string[20];

	strnfmt(string, 20, "%ld", value);
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
int SetArrayValueString(cptr varName, cptr field, cptr value)
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
static int DumpObjectInfo(object_type *o_ptr, char *varName)
{
	int known;
	cptr note;

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
			(TEST_FLAG(o_ptr->flags, 2, TR2_ACTIVATE)) != 0) != TCL_OK)
		{
			return TCL_ERROR;
		}
		if (SetArrayValueLong(varName, "artifact", (TR_FLAG(o_ptr->flags, 2, INSTA_ART)) ? 1 : 0)
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
	if (o_ptr->inscription)
	{
		note = quark_str(o_ptr->inscription);
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
int objcmd_inveninfo(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	int i_idx;
	char *varName;
	object_type *o_ptr;
	
	/* Hack - ignore parameter */
	(void) objc;

    if (Tcl_GetIntFromObj(interp, objV[1], &i_idx) != TCL_OK)
    {
		return TCL_ERROR;
    }

	/*** Verify index, existing object ***/

	/* Get the array variable name to dump results in */
	varName = Tcl_GetStringFromObj(objV[2], NULL);

	/* Grab the item */
	o_ptr = get_list_item(p_ptr->inventory, i_idx);

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
	
	/* Hack - ignore parameter */
	(void) objc;

	/* Get a numerical index or slot name */
    if (Tcl_GetIntFromObj(interp, objV[1], &i_idx) != TCL_OK)
    {
		Tcl_ResetResult(interp);
		if (Tcl_GetIndexFromObj(interp, objV[1], keyword_slot,
			"slot", 0, &i_idx) != TCL_OK)
		{
			return TCL_ERROR;
		}
    }

	/*** Verify index, existing object ***/

	/* Get the array variable name to dump results in */
	varName = Tcl_GetStringFromObj(objV[2], NULL);

	/* Grab the item */
	o_ptr = &p_ptr->equipment[i_idx];

	return DumpObjectInfo(o_ptr, varName);
}
