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
#include "struct-dll.h"
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

static Tcl_Obj *struct_get_object_kind_attr(Tcl_Interp *interp, t_field *info,
	object_kind *k_ptr, int k_idx)
{
	/* Hack - ignore unused parameters */
	(void) interp;
	(void) info;
	(void) k_ptr;

	return Tcl_NewIntObj(kind_attr(k_idx));
}

static Tcl_Obj *struct_get_player_type_stat(Tcl_Interp *interp, t_field *info,
	player_type *p_ptr, int p_idx)
{
	s16b *data = (s16b *) ((byte *) p_ptr + info->fOffset);
	int i, n = 0;
	char buf[128];

	/* Hack - ignore unused parameters */
	(void) interp;
	(void) p_idx;

	for (i = 0; i < 6; i++)
	{
		if (data[i] > 18)
		{
			n += sprintf(buf + n, "18/%d", data[i] - 18);
		}
		else
		{
			n += sprintf(buf + n, "%d", data[i]);
		}
		if (i < 5)
		{
			n += sprintf(buf + n, " ");
		}
	}

	return Tcl_NewStringObj(buf, n);
}

#define NAME_TEXT_TYPE FLD_INT32U

t_field struct_artifact_type[] = {
{FIELD_DESC(name, artifact_type), NAME_TEXT_TYPE, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(text, artifact_type), NAME_TEXT_TYPE, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(tval, artifact_type), FLD_INT8U, EDIT_CALLBACK, 0, 0, (struct_return) struct_get_ARRAY_tval},
{FIELD_DESC(sval, artifact_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(pval, artifact_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(to_h, artifact_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(to_d, artifact_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(to_a, artifact_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(ac, artifact_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(dd, artifact_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(ds, artifact_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(weight, artifact_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(cost, artifact_type), FLD_INT32, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(flags1, artifact_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(flags2, artifact_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(flags3, artifact_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(level, artifact_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(rarity, artifact_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(cur_num, artifact_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(max_num, artifact_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{NULL, 0, 0, 0, 0, 0, 0, NULL}
};

static t_field struct_feature_type[] = {
{FIELD_DESC(name, feature_type), NAME_TEXT_TYPE, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(text, feature_type), NAME_TEXT_TYPE, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(d_attr, feature_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(d_char, feature_type), FLD_CHAR, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(x_attr, feature_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(x_char, feature_type), FLD_CHAR, EDIT_NO, 0, 0, NULL},
{NULL, 0, 0, 0, 0, 0, 0, NULL}
};

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

t_field struct_object_kind[] = {
{FIELD_DESC(name, object_kind), NAME_TEXT_TYPE, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(text, object_kind), NAME_TEXT_TYPE, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(tval, object_kind), FLD_INT8U, EDIT_CALLBACK, 0, 0, (struct_return) struct_get_ARRAY_tval},
{FIELD_DESC(sval, object_kind), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(pval, object_kind), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(to_h, object_kind), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(to_d, object_kind), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(to_a, object_kind), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(ac, object_kind), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(dd, object_kind), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(ds, object_kind), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(weight, object_kind), FLD_INT16, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(cost, object_kind), FLD_INT32, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(flags1, object_kind), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(flags2, object_kind), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(flags3, object_kind), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
/* byte locale[4] */
/* byte chance[4] */
{FIELD_DESC(level, object_kind), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(extra, object_kind), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(d_attr, object_kind), FLD_INT8U, EDIT_CALLBACK, 0, 0, (struct_return) struct_get_object_kind_attr},
{FIELD_DESC(d_char, object_kind), FLD_CHAR, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(x_attr, object_kind), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(x_char, object_kind), FLD_CHAR, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(flavor, object_kind), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(easy_know, object_kind), FLD_BOOL, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(aware, object_kind), FLD_BOOL, EDIT_NO, 0, 0, NULL},
{FIELD_DESC(tried, object_kind), FLD_BOOL, EDIT_NO, 0, 0, NULL},
{NULL, 0, 0, 0, 0, 0, 0, NULL}
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


static t_field struct_player_type[] =
{
{ FIELD_DESC(psex, player_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(prace, player_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(pclass, player_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(realm1, player_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(realm2, player_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(hitdie, player_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(expfact, player_type), FLD_INT16U, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(age, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(ht, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(wt, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(sc, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(au, player_type), FLD_INT32, EDIT_YES, 0, LONG_MAX, NULL},
{ FIELD_DESC(max_exp, player_type), FLD_INT32, EDIT_YES, 0, PY_MAX_EXP, NULL},
{ FIELD_DESC(exp, player_type), FLD_INT32, EDIT_YES, 0, PY_MAX_EXP, NULL},
{ FIELD_DESC(exp_frac, player_type), FLD_INT16U, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(lev, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(place_num, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(mhp, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(chp, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(chp_frac, player_type), FLD_INT16U, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(msp, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(csp, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX, NULL},
{ FIELD_DESC(csp_frac, player_type), FLD_INT16U, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(max_lev, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(max_depth, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(stat_max, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_max, player_type), 0, (struct_return) struct_get_player_type_stat},
{ FIELD_DESC(stat_cur, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_cur, player_type), 0, (struct_return) struct_get_player_type_stat},

{ FIELD_DESC(fast, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(slow, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(blind, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(paralyzed, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(confused, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(afraid, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(image, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(poisoned, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(cut, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(stun, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},

{ FIELD_DESC(protevil, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(invuln, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(hero, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(shero, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(shield, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(blessed, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(tim_invis, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(tim_infra, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},

{ FIELD_DESC(oppose_acid, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(oppose_elec, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(oppose_fire, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(oppose_cold, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(oppose_pois, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},

{ FIELD_DESC(tim_esp, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},
{ FIELD_DESC(wraith_form, player_type), FLD_INT16, EDIT_YES, 0, 10000, NULL},

{ FIELD_DESC(resist_magic, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(chaos_patron, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(muta1, player_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(muta2, player_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(muta3, player_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(word_recall, player_type), FLD_INT16, EDIT_YES, 0, 1000, NULL},

{ FIELD_DESC(energy, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(food, player_type), FLD_INT16, EDIT_YES, 0, PY_FOOD_MAX, NULL},

{ FIELD_DESC(total_weight, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(confusing, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(searching, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(health_who, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(monster_race_idx, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(object_kind_idx, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(new_spells, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},


{ FIELD_DESC(cumber_armor, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(cumber_glove, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(heavy_wield, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(heavy_shoot, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(icky_wield, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(cur_lite, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(notice, player_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(update, player_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(redraw, player_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(window, player_type), FLD_FLAGS32, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(stat_use, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_use, player_type), 0, (struct_return) struct_get_player_type_stat},
{ FIELD_DESC(stat_top, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_top, player_type), 0, (struct_return) struct_get_player_type_stat},

	/*** Extracted fields ***/

{ FIELD_DESC(stat_add, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_add, player_type), 0, (struct_return) struct_get_player_type_stat},
{ FIELD_DESC(stat_ind, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_ind, player_type), 0, (struct_return) struct_get_player_type_stat},

{ FIELD_DESC(immune_acid, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(immune_elec, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(immune_fire, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(immune_cold, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(resist_acid, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_elec, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_fire, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_cold, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_pois, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(resist_confu, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_sound, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_lite, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_dark, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_chaos, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_disen, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_shard, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_nexus, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_blind, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_nethr, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(resist_fear, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(reflect, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(sh_fire, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(sh_elec, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(anti_magic, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(anti_tele, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(sustain_str, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(sustain_int, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(sustain_wis, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(sustain_dex, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(sustain_con, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(sustain_chr, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(aggravate, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(teleport, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(exp_drain, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(ffall, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(lite, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(free_act, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(see_inv, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(regenerate, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(hold_life, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(telepathy, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(slow_digest, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(bless_blade, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(xtra_might, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(impact, player_type), FLD_BOOL, EDIT_YES, 0, 0, NULL},

{ FIELD_DESC(dis_to_h, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(dis_to_d, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(dis_to_a, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(dis_ac, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(to_h, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(to_d, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(to_a, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(ac, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(see_infra, player_type), FLD_INT16, EDIT_YES, 0, MAX_SIGHT, NULL},

{ FIELD_DESC(skill_dis, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_dev, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_sav, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_stl, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_sns, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_fos, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_thn, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_thb, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_tht, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(skill_dig, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(num_blow, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(num_fire, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(pspeed, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},


{ FIELD_DESC(pet_follow_distance, player_type), FLD_INT16, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(pet_open_doors, player_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},
{ FIELD_DESC(pet_pickup_items, player_type), FLD_INT8U, EDIT_NO, 0, 0, NULL},

{ FIELD_DESC(leaving, player_type), FLD_BOOL, EDIT_NO, 0, 0, NULL},

{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};

struct t_match_artifact_type {
	int request_known;
	int match_known;
	int request_tval;
	int match_tval[10];
} match_artifact_type;

static int struct_find_artifact_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int *objIndex)
{
	static cptr findOption[] = {"-known", "-tval", NULL};
	int i, index;

	struct t_match_artifact_type *data = &match_artifact_type;
	static cptr boolOptions[] = {"no", "yes", "ignore", NULL};
	char *t;
	int tval;

	/* Hack - ignore unused parameter */
	(void) typePtr;

	/* Hack -- Initialize */
	if (!objc && !objv)
	{
		data->request_known = 0;
		data->request_tval = 0;
		return TCL_OK;
	}

	i = (*objIndex);

	/* Get the sub-option */
    if (Tcl_GetIndexFromObj(interp, objv[i], (char **) findOption, (char *) "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* known */
		    if (Tcl_GetIndexFromObj(interp, objv[i+1],
				(char **) boolOptions, (char *) "quasi-boolean", 0, &data->match_known)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}
			data->request_known = (data->match_known != 2);
			i += 2;
			break;

		case 1: /* tval */
			t = Tcl_GetStringFromObj(objv[i+1], NULL);
			if (angtk_tval_const(&tval, t) != TCL_OK)
			{
				return TCL_ERROR;
			}
			data->match_tval[data->request_tval++] = tval;
			i += 2;
			break;
	}

	(*objIndex) = i;

	return TCL_OK;
}

static int struct_match_artifact_type(Tcl_Interp *interp, StructType *typePtr,
	artifact_type *a_ptr, int a_idx)
{
	struct t_match_artifact_type *data = &match_artifact_type;
	int i;

	/* Hack - ignore unused parameters */
	(void) interp;
	(void) typePtr;
	(void) a_idx;

	/* Skip "empty" artifacts */
	if (a_ptr->name == 0) return 1;

	if (data->request_known)
	{
		if ((a_ptr->cur_num != 0) != data->match_known)
			return 1;
	}
	if (data->request_tval)
	{
		for (i = 0; i < data->request_tval; i++)
		{
			if (data->match_tval[i] == a_ptr->tval)
				break;
		}
		if (i == data->request_tval)
			return 1;
	}

	/* A match was found */
	return 0;
}

static int struct_info_artifact_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, artifact_type *a_ptr,
	int a_idx)
{
	static cptr infoOption[] = {"exists", "icon", "k_idx",
	 "name", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	int index;

	/* Hack - ignore unused parameters */
	(void) objc;
	(void) a_idx;
	(void) typePtr;

    if (Tcl_GetIndexFromObj(interp, objV[0], (char **) infoOption, (char *) "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
	}

	switch (index)
	{
		case 0: /* exists */
			Tcl_SetObjResult(interp, Tcl_NewBooleanObj(a_ptr->name != 0));
			break;

		case 1: /* icon */
		{
			char buf[128];
			int k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);
			if (!k_idx)
			{
				Tcl_SetResult(interp, (char *) "lookup_kind() failed", TCL_VOLATILE);
				return TCL_ERROR;
			}
			Tcl_SetResult(interp, assign_print2(buf, ASSIGN_OBJECT,
				k_idx), TCL_VOLATILE);
			break;
		}

		case 2: /* k_idx */
		{
			int k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);
			if (!k_idx)
			{
				Tcl_SetResult(interp, (char *) "lookup_kind() failed", TCL_VOLATILE);
				return TCL_ERROR;
			}
			Tcl_SetObjResult(interp, Tcl_NewIntObj(k_idx));
			break;
		}

		case 3: /* name */
			ExtToUtf_SetResult(interp, a_name + a_ptr->name);
			break;
	}

	return TCL_OK;
}

struct t_match_feature_type {
	int request_name;
	char match_name[160];
	int exact;
} match_feature_type;

static int struct_find_feature_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int *objIndex)
{
	static cptr findOption[] = {"-name", "-nameexact", NULL};
	int i, index;

	struct t_match_feature_type *data = &match_feature_type;

	/* Hack - ignore unused parameter */
	(void) typePtr;

	/* Hack -- Initialize */
	if (!objc && !objv)
	{
		data->request_name = 0;
		data->exact = FALSE;
		return TCL_OK;
	}

	i = (*objIndex);

	/* Get the sub-option */
    if (Tcl_GetIndexFromObj(interp, objv[i], (char **) findOption, (char *) "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* name */
		case 1: /* nameexact */
		{
			char *t = Tcl_GetStringFromObj(objv[i+1], NULL);

			/* FIXME: 160 chars max */
			(void) strcpy(data->match_name + data->request_name, t);
			data->request_name += strlen(t) + 1;
			data->match_name[data->request_name] = '\0';
			i += 2;

			if (index == 1)
				data->exact = TRUE;
			break;
		}
	}

	(*objIndex) = i;

	return TCL_OK;
}

static int struct_match_feature_type(Tcl_Interp *interp, StructType *typePtr,
	feature_type *f_ptr, int f_idx)
{
	struct t_match_feature_type *data = &match_feature_type;

	/* Hack - ignore unused parameter */
	(void) interp;
	(void) typePtr;
	(void) f_idx;

	/* Skip "empty" features */
	if (f_ptr->name == 0) return 1;

	if (data->request_name)
	{
		char *t = data->match_name;
		while (*t)
		{
			if (data->exact)
			{
				if (streq(f_name + f_ptr->name, t))
					return 0;
			}
			else
			{
				if (strstr(f_name + f_ptr->name, t) != NULL)
					return 0;
			}
			t += strlen(t) + 1;
		}

		/* No match */
		return 1;
	}

	/* A match was found */
	return 0;
}

static int struct_info_feature_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, feature_type *f_ptr,
	int f_idx)
{
	static cptr infoOption[] = {"name", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	int index;

	/* Hack - ignore unused parameters */
	(void) objc;
	(void) typePtr;
	(void) f_idx;

    if (Tcl_GetIndexFromObj(interp, objV[0], (char **) infoOption, (char *) "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
	}
	switch (index)
	{
		case 0: /* name */
			Tcl_SetObjResult(interp,
				ExtToUtf_NewStringObj(f_name + f_ptr->name, -1));
			break;
	}

	return TCL_OK;
}

struct t_match_object_kind {
	int request_known;
	int match_known;
	int request_tval;
	int match_tval[10];
} match_object_kind;

static int struct_find_object_kind(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int *objIndex)
{
	static cptr findOption[] = {"-known", "-tval", NULL};
	int i, index;

	struct t_match_object_kind *data = &match_object_kind;
	static cptr boolOptions[] = {"no", "yes", "ignore", NULL};
	char *t;
	int tval;

	/* Hack - ignore unused parameter */
	(void) typePtr;

	/* Hack -- Initialize */
	if (!objc && !objv)
	{
		data->request_known = 0;
		data->request_tval = 0;
		return TCL_OK;
	}

	i = (*objIndex);

	/* Get the sub-option */
    if (Tcl_GetIndexFromObj(interp, objv[i], (char **) findOption, (char *) "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* known */
		    if (Tcl_GetIndexFromObj(interp, objv[i+1],
				(char **) boolOptions, (char *) "quasi-boolean", 0, &data->match_known)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}
			data->request_known = (data->match_known != 2);
			i += 2;
			break;

		case 1: /* tval */
			t = Tcl_GetStringFromObj(objv[i+1], NULL);
			if (angtk_tval_const(&tval, t) != TCL_OK)
			{
				return TCL_ERROR;
			}
			data->match_tval[data->request_tval++] = tval;
			i += 2;
			break;
	}

	(*objIndex) = i;

	return TCL_OK;
}

static int struct_match_object_kind(Tcl_Interp *interp, StructType *typePtr,
	object_kind *k_ptr, int k_idx)
{
	struct t_match_object_kind *data = &match_object_kind;
	int i;

	/* Hack - ignore unused parameters */
	(void) interp;
	(void) typePtr;
	(void) k_idx;

	/* Skip "empty" object kinds */
	if (k_ptr->name == 0) return 1;

	if (data->request_known)
	{
		/* Hack -- skip insta-artifacts */
		if (k_ptr->flags3 & (TR3_INSTA_ART))
			return 1;
		if ((k_ptr->flavor && k_ptr->aware) != data->match_known)
			return 1;
	}
	if (data->request_tval)
	{
		for (i = 0; i < data->request_tval; i++)
		{
			if (data->match_tval[i] == k_ptr->tval)
				break;
		}
		if (i == data->request_tval)
			return 1;
	}

	/* A match was found */
	return 0;
}

static int struct_info_object_kind(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, object_kind *k_ptr,
	int k_idx)
{
	static cptr infoOption[] = {"exists", "icon", "insta_art", "memory",
		"name", "object_desc", "object_desc_flavor", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	int index;
	char buf[128];

	/* Hack - ignore parameters */
	(void) objc;
	(void) typePtr;

    if (Tcl_GetIndexFromObj(interp, objV[0], (char **) infoOption, (char *) "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
	}
	switch (index)
	{
		case 0: /* exists */
			Tcl_SetObjResult(interp, Tcl_NewBooleanObj(k_ptr->name != 0));
			break;

		case 1: /* icon */
		{
			object_type *o_ptr;

			/* Create fake object */
			o_ptr = object_prep(k_idx);

			/* Resolve alternate assignments */
			Tcl_SetResult(interp, assign_print_object(buf, o_ptr),
				TCL_VOLATILE);
			break;
		}

		case 2: /* insta_art */
			Tcl_SetObjResult(interp,
				Tcl_NewBooleanObj((k_ptr->flags3 & TR3_INSTA_ART) != 0));
			break;

		case 3: /* memory */
		{
			char *buffer;
			long length;
			object_type *o_ptr;

			/* Create fake object */
			o_ptr = object_prep(k_idx);

			buffer = Tcl_Alloc(5 * 1024L);
			length = angtk_describe_object(o_ptr, buffer, FALSE);
			Tcl_SetObjResult(interp, ExtToUtf_NewStringObj(buffer, length));
			Tcl_Free(buffer);
			break;
		}

		case 4: /* name */
			Tcl_SetObjResult(interp,
				ExtToUtf_NewStringObj(k_name + k_ptr->name, -1));
			break;

		case 5: /* object_desc */
		{
			char o_name[256];
			object_type *o_ptr;

			/*
			 * ZAngband relies on the IDENT_STOREB flag
			 * to disable flavors.
			 */
			bool old_plain = plain_descriptions;

			plain_descriptions = TRUE;

			/* Create fake object */
			o_ptr = object_prep(k_idx);

			/* Describe the object */
			object_desc_store(o_name, o_ptr, FALSE, 0, 256);

			plain_descriptions = old_plain;

			ExtToUtf_SetResult(interp, o_name);
			break;
		}

		case 6: /* object_desc_flavor */
		{
			char o_name[256];
			object_type *o_ptr;

			/* Save the "aware" flag */
			bool hack_aware = k_ptr->aware;

			/* Create fake object */
			o_ptr = object_prep(k_idx);
		
			/* Set the "known" flag */
			o_ptr->info |= (OB_KNOWN);
		
			/* Force "aware" for description */
			k_ptr->aware = TRUE;

			/* Describe the object */
			object_desc(o_name, o_ptr, FALSE, 0, 256);

			/* Restore "aware" flag */
			k_ptr->aware = hack_aware;

			ExtToUtf_SetResult(interp, o_name);
			break;
		}
	}

	return TCL_OK;
}

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
			monster_desc(m_name, m_ptr, 0x08);
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

	type.name = "artifact_type";
	type.elem = (unsigned char *) a_info;
	type.elem_size = sizeof(artifact_type);
	type.max = z_info->a_max;
	type.info = struct_artifact_type;
	type.infoProc = (StructInfoProc) struct_info_artifact_type;
	type.findProc = struct_find_artifact_type;
	type.matchProc = (StructFindMatchProc) struct_match_artifact_type;
	Struct_AddType(g_interp, &type);

	type.name = "feature_type";
	type.elem = (unsigned char *) f_info;
	type.elem_size = sizeof(feature_type);
	type.max = z_info->f_max;
	type.info = struct_feature_type;
	type.infoProc = (StructInfoProc) struct_info_feature_type;
	type.findProc = struct_find_feature_type;
	type.matchProc = (StructFindMatchProc) struct_match_feature_type;
	Struct_AddType(g_interp, &type);

	type.name = "monster_type";
	type.elem = (unsigned char *) m_list;
	type.elem_size = sizeof(monster_type);
	type.max = z_info->m_max;
	type.info = struct_monster_type;
	type.infoProc = (StructInfoProc) struct_info_monster_type;
	type.findProc = NULL;
	type.matchProc = (StructFindMatchProc) NULL;
	Struct_AddType(g_interp, &type);

	type.name = "object_kind";
	type.elem = (unsigned char *) k_info;
	type.elem_size = sizeof(object_kind);
	type.max = z_info->k_max;
	type.info = struct_object_kind;
	type.infoProc = (StructInfoProc) struct_info_object_kind;
	type.findProc = struct_find_object_kind;
	type.matchProc = (StructFindMatchProc) struct_match_object_kind;
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

	type.name = "player_type";
	type.elem = (unsigned char *) p_ptr;
	type.elem_size = sizeof(player_type);
	type.max = 1;
	type.info = struct_player_type;
	type.infoProc = NULL;
	type.findProc = NULL;
	type.matchProc = NULL;
	Struct_AddType(g_interp, &type);
}
