/* File: struct.c */

/* Purpose: reading/writing game arrays */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include <limits.h>
#include <tcl.h>
#include "angband.h"
#include "tnb.h"
#include "interp.h"
#include "struct-dll.h"
#include "util-dll.h"
#include "icon.h"

static Tcl_Obj *struct_get_ARRAY_tval(Tcl_Interp *interp, t_field *info,
	unsigned char *elem, int elemIndex)
{
	cptr cstr;
	byte tval = *(byte *)(elem + info->fOffset);

	(void) angtk_tval_string(&cstr, tval);
	return Tcl_NewStringObj(cstr, -1);
}

static Tcl_Obj *struct_get_object_kind_attr(Tcl_Interp *interp, t_field *info,
	object_kind *k_ptr, int k_idx)
{
	return Tcl_NewIntObj(kind_attr(k_idx));
}

static Tcl_Obj *struct_get_player_type_stat(Tcl_Interp *interp, t_field *info,
	player_type *p_ptr, int p_idx)
{
	s16b *data = (s16b *) ((byte *) p_ptr + info->fOffset);
	int i, n = 0;
	char buf[128];

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
{FIELD_DESC(name, artifact_type), NAME_TEXT_TYPE, EDIT_NO},
{FIELD_DESC(text, artifact_type), NAME_TEXT_TYPE, EDIT_NO},
{FIELD_DESC(tval, artifact_type), FLD_INT8U, EDIT_CALLBACK, 0, 0, struct_get_ARRAY_tval},
{FIELD_DESC(sval, artifact_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(pval, artifact_type), FLD_INT16, EDIT_NO},
{FIELD_DESC(to_h, artifact_type), FLD_INT16, EDIT_NO},
{FIELD_DESC(to_d, artifact_type), FLD_INT16, EDIT_NO},
{FIELD_DESC(to_a, artifact_type), FLD_INT16, EDIT_NO},
{FIELD_DESC(ac, artifact_type), FLD_INT16, EDIT_NO},
{FIELD_DESC(dd, artifact_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(ds, artifact_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(weight, artifact_type), FLD_INT16, EDIT_NO},
{FIELD_DESC(cost, artifact_type), FLD_INT32, EDIT_NO},
{FIELD_DESC(flags1, artifact_type), FLD_FLAGS32, EDIT_NO},
{FIELD_DESC(flags2, artifact_type), FLD_FLAGS32, EDIT_NO},
{FIELD_DESC(flags3, artifact_type), FLD_FLAGS32, EDIT_NO},
{FIELD_DESC(level, artifact_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(rarity, artifact_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(cur_num, artifact_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(max_num, artifact_type), FLD_INT8U, EDIT_NO},
{NULL, 0, 0, 0, 0, 0, 0, NULL}
};

static t_field struct_feature_type[] = {
{FIELD_DESC(name, feature_type), NAME_TEXT_TYPE, EDIT_NO},
{FIELD_DESC(text, feature_type), NAME_TEXT_TYPE, EDIT_NO},
{FIELD_DESC(mimic, feature_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(extra, feature_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(unused, feature_type), FLD_INT16, EDIT_NO},
{FIELD_DESC(d_attr, feature_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(d_char, feature_type), FLD_CHAR, EDIT_NO},
{FIELD_DESC(x_attr, feature_type), FLD_INT8U, EDIT_NO},
{FIELD_DESC(x_char, feature_type), FLD_CHAR, EDIT_NO},
{NULL, 0, 0, 0, 0, 0, 0, NULL}
};

static t_field struct_monster_race[] = {

{ FIELD_DESC(name, monster_race), NAME_TEXT_TYPE, EDIT_NO },
{ FIELD_DESC(text, monster_race), NAME_TEXT_TYPE, EDIT_NO },

{ FIELD_DESC(hdice, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(hside, monster_race), FLD_INT8U, EDIT_NO },

{ FIELD_DESC(ac, monster_race), FLD_INT16, EDIT_NO },

{ FIELD_DESC(sleep, monster_race), FLD_INT16, EDIT_NO },
{ FIELD_DESC(aaf, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(speed, monster_race), FLD_INT8U, EDIT_NO },

{ FIELD_DESC(mexp, monster_race), FLD_INT32, EDIT_NO },
{ FIELD_DESC(extra, monster_race), FLD_INT16, EDIT_NO },

{ FIELD_DESC(freq_inate, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(freq_spell, monster_race), FLD_INT8U, EDIT_NO },

{ FIELD_DESC(flags1, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags1 },
{ FIELD_DESC(flags2, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags2 },
{ FIELD_DESC(flags3, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags3 },
{ FIELD_DESC(flags4, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags4 },
{ FIELD_DESC(flags5, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags5 },
{ FIELD_DESC(flags6, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags6 },
{ FIELD_DESC(flags7, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags7 },
{ FIELD_DESC(flags8, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags8 },
{ FIELD_DESC(flags9, monster_race), FLD_FLAGS32, EDIT_NO, 0, 0, r_info_flags9 },

{ FIELD_DESC(blow, monster_race), FLD_CALLBACK, EDIT_NO },

{ FIELD_DESC(level, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(rarity, monster_race), FLD_INT8U, EDIT_NO },

{ FIELD_DESC(d_attr, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(d_char, monster_race), FLD_CHAR, EDIT_NO },

{ FIELD_DESC(x_attr, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(x_char, monster_race), FLD_CHAR, EDIT_NO },

{ FIELD_DESC(max_num, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(cur_num, monster_race), FLD_INT8U, EDIT_NO },

{ FIELD_DESC(r_sights, monster_race), FLD_INT16, EDIT_NO },
{ FIELD_DESC(r_deaths, monster_race), FLD_INT16, EDIT_NO },

{ FIELD_DESC(r_pkills, monster_race), FLD_INT16, EDIT_NO },
{ FIELD_DESC(r_tkills, monster_race), FLD_INT16, EDIT_NO },

{ FIELD_DESC(r_wake, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(r_ignore, monster_race), FLD_INT8U, EDIT_NO },

{ FIELD_DESC(r_drop_gold, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(r_drop_item, monster_race), FLD_INT8U, EDIT_NO },

{ FIELD_DESC(r_cast_inate, monster_race), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(r_cast_spell, monster_race), FLD_INT8U, EDIT_NO },

{ FIELD_DESC(r_blows, monster_race), FLD_CALLBACK, EDIT_NO }, /* BUG BUG BUG */

{ FIELD_DESC(r_flags1, monster_race), FLD_FLAGS32, EDIT_NO },
{ FIELD_DESC(r_flags2, monster_race), FLD_FLAGS32, EDIT_NO },
{ FIELD_DESC(r_flags3, monster_race), FLD_FLAGS32, EDIT_NO },
{ FIELD_DESC(r_flags4, monster_race), FLD_FLAGS32, EDIT_NO },
{ FIELD_DESC(r_flags5, monster_race), FLD_FLAGS32, EDIT_NO },
{ FIELD_DESC(r_flags6, monster_race), FLD_FLAGS32, EDIT_NO },
{ FIELD_DESC(r_flags7, monster_race), FLD_FLAGS32, EDIT_NO },

{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};


static t_field struct_monster_type[] = {

{ FIELD_DESC(r_idx, monster_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(fy, monster_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(fx, monster_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(hp, monster_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(maxhp, monster_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(csleep, monster_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(mspeed, monster_type), FLD_INT8U, EDIT_YES, 0, 199},
{ FIELD_DESC(energy, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX},
{ FIELD_DESC(stunned, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX},
{ FIELD_DESC(confused, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX},
{ FIELD_DESC(monfear, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX},
{ FIELD_DESC(invulner, monster_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX},
{ FIELD_DESC(cdis, monster_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(mflag, monster_type), FLD_FLAGS8, EDIT_YES, 0, 0, NULL},
{ FIELD_DESC(ml, monster_type), FLD_BOOL, EDIT_YES, 0, 0,},
{ FIELD_DESC(hold_o_idx, monster_type), FLD_INT16, EDIT_YES, 0, 0},

#ifdef WDT_TRACK_OPTIONS

{ FIELD_DESC(ty, monster_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(tx, monster_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(t_dur, monster_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(t_bit, monster_type), FLD_FLAGS8, EDIT_NO},

#endif

#ifdef DRS_SMART_OPTIONS

{ FIELD_DESC(smart, monster_type), FLD_FLAGS32, EDIT_YES, 0, 0, NULL},

#endif


{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};

t_field struct_object_kind[] = {
{FIELD_DESC(name, object_kind), NAME_TEXT_TYPE, EDIT_NO},
{FIELD_DESC(text, object_kind), NAME_TEXT_TYPE, EDIT_NO},
{FIELD_DESC(tval, object_kind), FLD_INT8U, EDIT_CALLBACK, 0, 0, struct_get_ARRAY_tval},
{FIELD_DESC(sval, object_kind), FLD_INT8U, EDIT_NO},
{FIELD_DESC(pval, object_kind), FLD_INT16, EDIT_NO},
{FIELD_DESC(to_h, object_kind), FLD_INT16, EDIT_NO},
{FIELD_DESC(to_d, object_kind), FLD_INT16, EDIT_NO},
{FIELD_DESC(to_a, object_kind), FLD_INT16, EDIT_NO},
{FIELD_DESC(ac, object_kind), FLD_INT16, EDIT_NO},
{FIELD_DESC(dd, object_kind), FLD_INT8U, EDIT_NO},
{FIELD_DESC(ds, object_kind), FLD_INT8U, EDIT_NO},
{FIELD_DESC(weight, object_kind), FLD_INT16, EDIT_NO},
{FIELD_DESC(cost, object_kind), FLD_INT32, EDIT_NO},
{FIELD_DESC(flags1, object_kind), FLD_FLAGS32, EDIT_NO},
{FIELD_DESC(flags2, object_kind), FLD_FLAGS32, EDIT_NO},
{FIELD_DESC(flags3, object_kind), FLD_FLAGS32, EDIT_NO},
/* byte locale[4] */
/* byte chance[4] */
{FIELD_DESC(level, object_kind), FLD_INT8U, EDIT_NO},
{FIELD_DESC(extra, object_kind), FLD_INT8U, EDIT_NO},
{FIELD_DESC(d_attr, object_kind), FLD_INT8U, EDIT_CALLBACK, 0, 0, struct_get_object_kind_attr},
{FIELD_DESC(d_char, object_kind), FLD_CHAR, EDIT_NO},
{FIELD_DESC(x_attr, object_kind), FLD_INT8U, EDIT_NO},
{FIELD_DESC(x_char, object_kind), FLD_CHAR, EDIT_NO},
{FIELD_DESC(flavor, object_kind), FLD_INT8U, EDIT_NO},
{FIELD_DESC(easy_know, object_kind), FLD_BOOL, EDIT_NO},
{FIELD_DESC(aware, object_kind), FLD_BOOL, EDIT_NO},
{FIELD_DESC(tried, object_kind), FLD_BOOL, EDIT_NO},
{NULL, 0, 0, 0, 0, 0, 0, NULL}
};

static t_field struct_object_type[] = {

{ FIELD_DESC(k_idx, object_type), FLD_INT16, EDIT_NO },
{ FIELD_DESC(iy, object_type), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(ix, object_type), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(tval, object_type), FLD_INT8U, EDIT_CALLBACK, 0, 0, struct_get_ARRAY_tval },
{ FIELD_DESC(sval, object_type), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(pval, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX },
{ FIELD_DESC(discount, object_type), FLD_INT8U, EDIT_YES, 0, 100 },
{ FIELD_DESC(number, object_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX },
{ FIELD_DESC(weight, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX },
{ FIELD_DESC(name1, object_type), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(name2, object_type), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(xtra1, object_type), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(xtra2, object_type), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(to_h, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX },
{ FIELD_DESC(to_d, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX },
{ FIELD_DESC(to_a, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX },
{ FIELD_DESC(ac, object_type), FLD_INT16, EDIT_YES, SHRT_MIN, SHRT_MAX },
{ FIELD_DESC(dd, object_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX },
{ FIELD_DESC(ds, object_type), FLD_INT8U, EDIT_YES, 0, UCHAR_MAX },
{ FIELD_DESC(timeout, object_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX },
{ FIELD_DESC(ident, object_type), FLD_FLAGS8, EDIT_YES, 0, 0, NULL },
{ FIELD_DESC(marked, object_type), FLD_BOOL, EDIT_YES },
{ FIELD_DESC(inscription, object_type), FLD_INT16U, EDIT_NO },
{ FIELD_DESC(feeling, object_type), FLD_INT8U, EDIT_NO },
{ FIELD_DESC(next_o_idx, object_type), FLD_INT16, EDIT_NO },
{ FIELD_DESC(held_m_idx, object_type), FLD_INT16, EDIT_NO },
{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};

#if 0

static t_field player_stat_info[] = {

{ "Strength", 0, 2, FLD_INT16, EDIT_YES, 3, 118 },
{ "Intelligence", 2, 2, FLD_INT16, EDIT_YES, 3, 118 },
{ "Wisdom", 4, 2, FLD_INT16, EDIT_YES, 3, 118 },
{ "Dexterity", 6, 2, FLD_INT16, EDIT_YES, 3, 118 },
{ "Constitution", 8, 2, FLD_INT16, EDIT_YES, 3, 118 },
{ "Charisma", 10, 2, FLD_INT16, EDIT_YES, 3, 118 },

{ NULL, 0, 0, 0, 0, 0, 0, NULL }

};

#endif

static t_field struct_player_type[] =
{
{ FIELD_DESC(oldpy, player_type), FLD_INT16, EDIT_YES, 1, DUNGEON_HGT - 1},
{ FIELD_DESC(oldpx, player_type), FLD_INT16, EDIT_YES, 1, DUNGEON_WID - 1},

{ FIELD_DESC(psex, player_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(prace, player_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(pclass, player_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(realm1, player_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(realm2, player_type), FLD_INT8U, EDIT_NO},

{ FIELD_DESC(hitdie, player_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(expfact, player_type), FLD_INT16U, EDIT_NO},

{ FIELD_DESC(age, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(ht, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(wt, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(sc, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(au, player_type), FLD_INT32, EDIT_YES, 0, LONG_MAX},
{ FIELD_DESC(max_exp, player_type), FLD_INT32, EDIT_YES, 0, PY_MAX_EXP},
{ FIELD_DESC(exp, player_type), FLD_INT32, EDIT_YES, 0, PY_MAX_EXP},
{ FIELD_DESC(exp_frac, player_type), FLD_INT16U, EDIT_NO},

{ FIELD_DESC(lev, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(town_num, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(arena_number, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(inside_arena, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(inside_quest, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(wilderness_x, player_type), FLD_INT32, EDIT_YES, 0, 9},
{ FIELD_DESC(wilderness_y, player_type), FLD_INT32, EDIT_YES, 0, 9},

{ FIELD_DESC(mhp, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(chp, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(chp_frac, player_type), FLD_INT16U, EDIT_NO},

{ FIELD_DESC(msp, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(csp, player_type), FLD_INT16, EDIT_YES, 0, SHRT_MAX},
{ FIELD_DESC(csp_frac, player_type), FLD_INT16U, EDIT_NO},

{ FIELD_DESC(max_plv, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(max_dlv, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(stat_max, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_max, player_type), 0, struct_get_player_type_stat},
{ FIELD_DESC(stat_cur, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_cur, player_type), 0, struct_get_player_type_stat},

{ FIELD_DESC(fast, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(slow, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(blind, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(paralyzed, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(confused, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(afraid, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(image, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(poisoned, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(cut, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(stun, player_type), FLD_INT16, EDIT_YES, 0, 10000},

{ FIELD_DESC(protevil, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(invuln, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(hero, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(shero, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(shield, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(blessed, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(tim_invis, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(tim_infra, player_type), FLD_INT16, EDIT_YES, 0, 10000},

{ FIELD_DESC(oppose_acid, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(oppose_elec, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(oppose_fire, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(oppose_cold, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(oppose_pois, player_type), FLD_INT16, EDIT_YES, 0, 10000},

{ FIELD_DESC(tim_esp, player_type), FLD_INT16, EDIT_YES, 0, 10000},
{ FIELD_DESC(wraith_form, player_type), FLD_INT16, EDIT_YES, 0, 10000},

{ FIELD_DESC(resist_magic, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(chaos_patron, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(muta1, player_type), FLD_FLAGS32, EDIT_NO},
{ FIELD_DESC(muta2, player_type), FLD_FLAGS32, EDIT_NO},
{ FIELD_DESC(muta3, player_type), FLD_FLAGS32, EDIT_NO},

{ FIELD_DESC(word_recall, player_type), FLD_INT16, EDIT_YES, 0, 1000},

{ FIELD_DESC(energy, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(food, player_type), FLD_INT16, EDIT_YES, 0, PY_FOOD_MAX},

{ FIELD_DESC(total_weight, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(confusing, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(searching, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(health_who, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(monster_race_idx, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(object_kind_idx, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(new_spells, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(old_spells, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(old_cumber_armor, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(old_cumber_glove, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(old_heavy_wield, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(old_heavy_shoot, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(old_icky_wield, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(old_lite, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(old_view, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(old_food_aux, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(cumber_armor, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(cumber_glove, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(heavy_wield, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(heavy_shoot, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(icky_wield, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(cur_lite, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(notice, player_type), FLD_FLAGS32, EDIT_NO},
{ FIELD_DESC(update, player_type), FLD_FLAGS32, EDIT_NO},
{ FIELD_DESC(redraw, player_type), FLD_FLAGS32, EDIT_NO},
{ FIELD_DESC(window, player_type), FLD_FLAGS32, EDIT_NO},

{ FIELD_DESC(stat_use, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_use, player_type), 0, struct_get_player_type_stat},
{ FIELD_DESC(stat_top, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_top, player_type), 0, struct_get_player_type_stat},

	/*** Extracted fields ***/

{ FIELD_DESC(stat_add, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_add, player_type), 0, struct_get_player_type_stat},
{ FIELD_DESC(stat_ind, player_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(stat_ind, player_type), 0, struct_get_player_type_stat},

{ FIELD_DESC(immune_acid, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(immune_elec, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(immune_fire, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(immune_cold, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(resist_acid, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_elec, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_fire, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_cold, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_pois, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(resist_conf, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_sound, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_lite, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_dark, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_chaos, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_disen, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_shard, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_nexus, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_blind, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_neth, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(resist_fear, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(reflect, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(sh_fire, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(sh_elec, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(anti_magic, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(anti_tele, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(sustain_str, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(sustain_int, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(sustain_wis, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(sustain_dex, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(sustain_con, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(sustain_chr, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(aggravate, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(teleport, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(exp_drain, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(ffall, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(lite, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(free_act, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(see_inv, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(regenerate, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(hold_life, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(telepathy, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(slow_digest, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(bless_blade, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(xtra_might, player_type), FLD_BOOL, EDIT_YES},
{ FIELD_DESC(impact, player_type), FLD_BOOL, EDIT_YES},

{ FIELD_DESC(dis_to_h, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(dis_to_d, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(dis_to_a, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(dis_ac, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(to_h, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(to_d, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(to_a, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(ac, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(see_infra, player_type), FLD_INT16, 0, MAX_SIGHT},

{ FIELD_DESC(skill_dis, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_dev, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_sav, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_stl, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_srh, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_fos, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_thn, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_thb, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_tht, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(skill_dig, player_type), FLD_INT16, EDIT_NO},

{ FIELD_DESC(num_blow, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(num_fire, player_type), FLD_INT16, EDIT_NO},


{ FIELD_DESC(tval_xtra, player_type), FLD_INT8U, EDIT_NO},

{ FIELD_DESC(tval_ammo, player_type), FLD_INT8U, EDIT_NO},

{ FIELD_DESC(pspeed, player_type), FLD_INT16, EDIT_NO},


{ FIELD_DESC(pet_follow_distance, player_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(pet_open_doors, player_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(pet_pickup_items, player_type), FLD_INT8U, EDIT_NO},

{ FIELD_DESC(exit_bldg, player_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(leftbldg, player_type), FLD_INT8U, EDIT_NO},
{ FIELD_DESC(leaving, player_type), FLD_BOOL, EDIT_NO},
{ FIELD_DESC(leaving_dungeon, player_type), FLD_BOOL, EDIT_NO},

{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};

static t_field struct_player_class[] =
{
{ FIELD_DESC(title, player_class), FLD_STRINGPTR, EDIT_NO},

{ FIELD_DESC(c_adj, player_class), FLD_CALLBACK, EDIT_NO, FIELD_SIZE(c_adj, player_class),},

{ FIELD_DESC(c_dis, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(c_dev, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(c_sav, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(c_stl, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(c_srh, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(c_fos, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(c_thn, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(c_thb, player_class), FLD_INT16, EDIT_NO},

{ FIELD_DESC(x_dis, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(x_dev, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(x_sav, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(x_stl, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(x_srh, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(x_fos, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(x_thn, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(x_thb, player_class), FLD_INT16, EDIT_NO},

{ FIELD_DESC(c_mhp, player_class), FLD_INT16, EDIT_NO},
{ FIELD_DESC(c_exp, player_class), FLD_INT16, EDIT_NO},

{ FIELD_DESC(pet_upkeep_div, player_class), FLD_INT8U, EDIT_NO},

{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};


#define QUEST_TYPE_NONE 0

cptr keyword_quest_type_type[] = {
	STRINGIFY2(QUEST_TYPE_NONE),
	STRINGIFY2(QUEST_TYPE_KILL_LEVEL),
	STRINGIFY2(QUEST_TYPE_KILL_ANY_LEVEL),
	STRINGIFY2(QUEST_TYPE_FIND_ARTIFACT),
	STRINGIFY2(QUEST_TYPE_FIND_EXIT),
	STRINGIFY2(QUEST_TYPE_KILL_NUMBER),
	STRINGIFY2(QUEST_TYPE_KILL_ALL),
	STRINGIFY2(QUEST_TYPE_RANDOM),
	NULL
};

cptr keyword_quest_type_status[] = {
	STRINGIFY2(QUEST_STATUS_UNTAKEN),
	STRINGIFY2(QUEST_STATUS_TAKEN),
	STRINGIFY2(QUEST_STATUS_COMPLETED),
	STRINGIFY2(QUEST_STATUS_REWARDED),
	STRINGIFY2(QUEST_STATUS_FINISHED),
	STRINGIFY2(QUEST_STATUS_FAILED),
	STRINGIFY2(QUEST_STATUS_FAILED_DONE),
	NULL
};



static t_field struct_building_type[] = {
{ FIELD_DESC(name, building_type), FLD_STRING, EDIT_NO},
{ FIELD_DESC(owner_name, building_type), FLD_STRING, EDIT_NO},
{ FIELD_DESC(owner_race, building_type), FLD_STRING, EDIT_NO},
{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};

#define q_list quest

static Tcl_Obj *struct_get_quest_type_name(Tcl_Interp *interp, t_field *info,
	quest_type *q_ptr, int q_idx)
{
	/*
	 * We want to know the name of the quest, but it may not have been
	 * initialized. So we must parse the q_info.txt file. But only if
	 * the name is empty.
	 */
	if (!q_ptr->name[0])
	{
		int i, old_quest;
		
		/* Clear the text */
		for (i = 0; i < 10; i++)
		{
			quest_text[i][0] = '\0';
		}

		quest_text_line = 0;

		/* Set the quest number temporarily */
		old_quest = p_ptr->inside_quest;
		p_ptr->inside_quest = q_idx;

		/* Get the quest text */
		init_flags = INIT_SHOW_TEXT;
		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		/* Reset the old quest number */
		p_ptr->inside_quest = old_quest;
	}

	return ExtToUtf_NewStringObj(q_ptr->name, -1);
}

cptr keyword_quest_type_flags[] = {
	STRINGIFY2(QUEST_FLAG_SILENT),
	STRINGIFY2(QUEST_FLAG_PRESET),
	NULL
};

/* Note: t_field.max is calculated below for all FLD_INDEX16 fields */
 
static t_field struct_quest_type[] = {

{ FIELD_DESC(status, quest_type), FLD_INDEX16, EDIT_NO, 0, -1, keyword_quest_type_status},
{ FIELD_DESC(type, quest_type), FLD_INDEX16, EDIT_NO, 0, -1, keyword_quest_type_type},
{ FIELD_DESC(name, quest_type), FLD_CALLBACK, EDIT_CALLBACK, FIELD_SIZE(name, quest_type), 0, struct_get_quest_type_name},
{ FIELD_DESC(level, quest_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(r_idx, quest_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(cur_num, quest_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(max_num, quest_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(k_idx, quest_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(num_mon, quest_type), FLD_INT16, EDIT_NO},
{ FIELD_DESC(flags, quest_type), FLD_FLAGS8, EDIT_NO, 0, 0, keyword_quest_type_flags},

{ NULL, 0, 0, 0, 0, 0, 0, NULL }
};

struct t_match_artifact_type {
	int request_known;
	int match_known;
	int request_tval;
	int match_tval[10];
} match_artifact_type;

int struct_find_artifact_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int *objIndex)
{
	static char *findOption[] = {"-known", "-tval", NULL};
	int i, index;

	struct t_match_artifact_type *data = &match_artifact_type;
	static char *boolOptions[] = {"no", "yes", "ignore", NULL};
	char *t;
	int tval;

	/* Hack -- Initialize */
	if (!objc && !objv)
	{
		data->request_known = 0;
		data->request_tval = 0;
		return TCL_OK;
	}

	i = (*objIndex);

	/* Get the sub-option */
    if (Tcl_GetIndexFromObj(interp, objv[i], findOption, "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* known */
		    if (Tcl_GetIndexFromObj(interp, objv[i+1],
				boolOptions, "quasi-boolean", 0, &data->match_known)
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

int struct_match_artifact_type(Tcl_Interp *interp, StructType *typePtr,
	artifact_type *a_ptr, int a_idx)
{
	struct t_match_artifact_type *data = &match_artifact_type;
	int i;

	/* Skip "empty" artifacts */
	if (a_ptr->name == 0) return 1;

	if (data->request_known)
	{
		if ((ART_CURNUM(a_ptr) != 0) != data->match_known)
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

int struct_info_artifact_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, artifact_type *a_ptr,
	int a_idx)
{
	static char *infoOption[] = {"exists", "icon", "k_idx", "location",
		"memory", "name", "object_desc", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	int index;
	object_type object_type_body, *o_ptr = &object_type_body;

    if (Tcl_GetIndexFromObj(interp, objV[0], infoOption, "option",
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
				Tcl_SetResult(interp, "lookup_kind() failed", TCL_VOLATILE);
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
				Tcl_SetResult(interp, "lookup_kind() failed", TCL_VOLATILE);
				return TCL_ERROR;
			}
			Tcl_SetObjResult(interp, Tcl_NewIntObj(k_idx));
			break;
		}

		case 3: /* location */
			index = angtk_find_artifact(a_idx, o_ptr);
			if (index == -1)
			{
				return TCL_ERROR;
			}
			Tcl_SetResult(interp, (char *) keyword_artifact_location[index],
				TCL_VOLATILE);
			break;

		case 4: /* memory */
		{
			char *buffer;
			long length;

			index = angtk_find_artifact(a_idx, o_ptr);
			if (index == -1)
			{
				return TCL_ERROR;
			}
			buffer = Tcl_Alloc(5 * 1024L);
			length = angtk_describe_object(o_ptr, buffer, FALSE);
			Tcl_SetObjResult(interp, ExtToUtf_NewStringObj(buffer, length));
			Tcl_Free(buffer);
			break;
		}

		case 5: /* name */
			ExtToUtf_SetResult(interp, a_name + a_ptr->name);
			break;

		case 6: /* object_desc */
		{
			char o_name[O_NAME_MAX];

			index = angtk_find_artifact(a_idx, o_ptr);
			if (index == -1)
			{
				return TCL_ERROR;
			}
			object_desc(o_name, o_ptr, TRUE, 3);
			ExtToUtf_SetResult(interp, o_name);
			break;
		}
	}

	return TCL_OK;
}

struct t_match_feature_type {
	int request_name;
	char match_name[160];
	int exact;
} match_feature_type;

int struct_find_feature_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int *objIndex)
{
	static char *findOption[] = {"-name", "-nameexact", NULL};
	int i, index;

	struct t_match_feature_type *data = &match_feature_type;

	/* Hack -- Initialize */
	if (!objc && !objv)
	{
		data->request_name = 0;
		data->exact = FALSE;
		return TCL_OK;
	}

	i = (*objIndex);

	/* Get the sub-option */
    if (Tcl_GetIndexFromObj(interp, objv[i], findOption, "option",
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

int struct_match_feature_type(Tcl_Interp *interp, StructType *typePtr,
	feature_type *f_ptr, int f_idx)
{
	struct t_match_feature_type *data = &match_feature_type;

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

int struct_info_feature_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, feature_type *f_ptr,
	int f_idx)
{
	static char *infoOption[] = {"name", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	int index;

    if (Tcl_GetIndexFromObj(interp, objV[0], infoOption, "option",
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

int struct_find_object_kind(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int *objIndex)
{
	static char *findOption[] = {"-known", "-tval", NULL};
	int i, index;

	struct t_match_object_kind *data = &match_object_kind;
	static char *boolOptions[] = {"no", "yes", "ignore", NULL};
	char *t;
	int tval;

	/* Hack -- Initialize */
	if (!objc && !objv)
	{
		data->request_known = 0;
		data->request_tval = 0;
		return TCL_OK;
	}

	i = (*objIndex);

	/* Get the sub-option */
    if (Tcl_GetIndexFromObj(interp, objv[i], findOption, "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* known */
		    if (Tcl_GetIndexFromObj(interp, objv[i+1],
				boolOptions, "quasi-boolean", 0, &data->match_known)
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

int struct_match_object_kind(Tcl_Interp *interp, StructType *typePtr,
	object_kind *k_ptr, int k_idx)
{
	struct t_match_object_kind *data = &match_object_kind;
	int i;

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

int struct_info_object_kind(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, object_kind *k_ptr,
	int k_idx)
{
	static char *infoOption[] = {"exists", "icon", "insta_art", "memory",
		"name", "object_desc", "object_desc_flavor", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	int index;
	char buf[128];

    if (Tcl_GetIndexFromObj(interp, objV[0], infoOption, "option",
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
			object_type object_type_body;

			/* Get local object */
			o_ptr = &object_type_body;

			/* Create fake object */
			object_prep(o_ptr, k_idx);

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
			object_type object_type_body;

			/* Get local object */
			o_ptr = &object_type_body;

			/* Create fake object */
			object_prep(o_ptr, k_idx);

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
			char o_name[O_NAME_MAX];
			object_type *o_ptr;
			object_type object_type_body;

			/*
			 * ZAngband relies on the IDENT_STOREB flag
			 * to disable flavors.
			 */
			bool old_plain = plain_descriptions;

			plain_descriptions = TRUE;

			/* Get local object */
			o_ptr = &object_type_body;

			/* Create fake object */
			object_prep(o_ptr, k_idx);

			/* Describe the object */
			object_desc_store(o_name, o_ptr, FALSE, 0);

			plain_descriptions = old_plain;

			ExtToUtf_SetResult(interp, o_name);
			break;
		}

		case 6: /* object_desc_flavor */
		{
			char o_name[O_NAME_MAX];
			object_type *o_ptr;
			object_type object_type_body;

			/* Save the "aware" flag */
			bool hack_aware = k_ptr->aware;

			/* Get local object */
			o_ptr = &object_type_body;

			/* Create fake object */
			object_prep(o_ptr, k_idx);
		
			/* Set the "known" flag */
			o_ptr->ident |= (IDENT_KNOWN);
		
			/* Force "aware" for description */
			k_ptr->aware = TRUE;

			/* Describe the object */
			object_desc(o_name, o_ptr, FALSE, 0);

			/* Restore "aware" flag */
			k_ptr->aware = hack_aware;

			ExtToUtf_SetResult(interp, o_name);
			break;
		}
	}

	return TCL_OK;
}

/* option ?arg ...? */
int struct_info_object_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, object_type *o_ptr,
	int o_idx)
{
	static char *infoOption[] = {"icon", "memory", "name", "desc", "known",
		"char", "attr", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	Tcl_Obj *resultObjPtr = NULL;
	int index;

	char buf[128], *buffer, o_name[O_NAME_MAX];
	object_type fake = (*o_ptr);

    if (Tcl_GetIndexFromObj(interp, objV[0], infoOption, "option",
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
			object_desc_store(o_name, o_ptr, TRUE, 3);
			resultObjPtr = ExtToUtf_NewStringObj(o_name, -1);
			break;

		case 3: /* desc */
			object_desc(o_name, o_ptr, TRUE, 3);
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

/* Data for custom search routine */
struct t_match_monster_race {
	int request_d_char;
	char match_d_char[80];

	int request_unique;
	int match_unique;
	
	int request_known;
	int match_known;
	
	int request_name;
	char match_name[80];
	
	int request_flags, request_not_flags;
	u32b match_flags[10];
	u32b match_not_flags[10];
} match_monster_race = {0};

extern char **r_info_flags[10];
extern int r_info_flags_max;

int struct_find_monster_race(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int *objIndex)
{
	static char *findOption[] = {"-d_char", "-known", "-name", "-unique",
		"-flag", "-notflag", NULL};
	static char *boolOption[] = {"no", "yes", "ignore", NULL};
	char *t;
	int i, j, index;

	struct t_match_monster_race *data = &match_monster_race;

	/* Hack -- Initialize */
	if (!objc && !objv)
	{
		data->request_d_char = 0;
		data->request_unique = 0;
		data->match_unique = 0;
		data->request_known = 0;
		data->match_known = 0;
		data->request_name = 0;
		data->request_flags = 0;
		data->request_not_flags = 0;
		
		for (i = 0; i < 10; i++)
		{
			data->match_flags[i] = 0;
			data->match_not_flags[i] = 0;
		}

		return TCL_OK;
	}

	i = (*objIndex);

	/* Get the sub-option */
    if (Tcl_GetIndexFromObj(interp, objv[i], findOption, "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* d_char */
			(void) strcpy(data->match_d_char,
				Tcl_GetStringFromObj(objv[i+1], NULL));
			data->request_d_char = 1;
			i += 2;
			break;

		case 1: /* known */
		    if (Tcl_GetIndexFromObj(interp, objv[i+1],
				boolOption, "quasi-boolean", 0, &data->match_known)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}
			data->request_known = (data->match_known != 2);
			i += 2;
			break;

		case 2: /* name */
			(void) strcpy(data->match_name,
				Tcl_GetStringFromObj(objv[i+1], NULL));
			data->request_name = 1;
			i += 2;
			break;

		case 3: /* unique */
		    if (Tcl_GetIndexFromObj(interp, objv[i+1],
				boolOption, "quasi-boolean", 0, &data->match_unique)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}
			data->request_unique = (data->match_unique != 2);
			i += 2;
			break;

		case 4: /* flag */
			t = Tcl_GetStringFromObj(objv[i+1], NULL);
			for (j = 1; j <= r_info_flags_max; j++)
			{
				int k;
				for (k = 0; k < 32; k++)
				{
					if (!strcmp(t, r_info_flags[j][k]))
					{
						data->match_flags[j] |= (1L << k);
						break;
					}
				}
				if (k < 32) break;
			}
			if (j > r_info_flags_max)
			{
				Tcl_SetStringObj(Tcl_GetObjResult(interp),
					format("unknown flag \"%s\"", t), -1);
				return TCL_ERROR;
			}
			data->request_flags = 1;
			i += 2;
			break;

		case 5: /* notflag */
			t = Tcl_GetStringFromObj(objv[i+1], NULL);
			for (j = 1; j <= r_info_flags_max; j++)
			{
				int k;
				for (k = 0; k < 32; k++)
				{
					if (!strcmp(t, r_info_flags[j][k]))
					{
						data->match_not_flags[j] |= (1L << k);
						break;
					}
				}
				if (k < 32) break;
			}
			if (j > r_info_flags_max)
			{
				Tcl_SetStringObj(Tcl_GetObjResult(interp),
					format("unknown flag \"%s\"", t), -1);
				return TCL_ERROR;
			}
			data->request_not_flags = 1;
			i += 2;
			break;
	}

	(*objIndex) = i;

	return TCL_OK;
}

int struct_match_monster_race(Tcl_Interp *interp, StructType *typePtr,
	monster_race *r_ptr, int r_idx)
{
	struct t_match_monster_race *data = &match_monster_race;
	DECLARE_LORE(l_ptr) = AT_LORE(r_idx);
	u32b flags[10];
	int i;

	/* Never match non-existent races */
	if (r_ptr->name == 0) return 1;

	if (data->request_flags || data->request_not_flags)
	{
		flags[1] = r_ptr->flags1; 
		flags[2] = r_ptr->flags2; 
		flags[3] = r_ptr->flags3; 
		flags[4] = r_ptr->flags4; 
		flags[5] = r_ptr->flags5; 
		flags[6] = r_ptr->flags6; 
	}
	
	if (data->request_d_char)
	{
		if (strchr(data->match_d_char, r_ptr->d_char) == NULL)
			return 1;
	}
	if (data->request_known)
	{
		if ((cheat_know || l_ptr->LF(sights)) != data->match_known)
			return 1;
	}
	if (data->request_name)
	{
		if (strstr(r_name + r_ptr->name, data->match_name) == NULL)
			return 1;
	}
	if (data->request_unique)
	{
		if (((r_ptr->flags1 & (RF1_UNIQUE)) != 0) != data->match_unique)
			return 1;
	}
	if (data->request_flags)
	{
		/*
		 * If any of the requested flags are not set, then
		 * do not match this monster race.
		 */
		for (i = 1; i <= r_info_flags_max; i++)
		{
			if (!data->match_flags[i])
				continue;
			if ((flags[i] & data->match_flags[i]) != data->match_flags[i])
				return 1;
		}
	}
	if (data->request_not_flags)
	{
		/*
		 * If any of the requested flags are set, then
		 * do not match this monster race.
		 */
		for (i = 1; i <= r_info_flags_max; i++)
		{
			if (!data->match_not_flags[i])
				continue;
			if (flags[i] & data->match_not_flags[i])
				return 1;
		}
	}

	/* A match was found */
	return 0;
}

int struct_info_monster_race(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, monster_race *r_ptr,
	int r_idx)
{
	static char *infoOption[] = {"exists", "icon", "memory", "name", "unique",
		"plural", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	int index;

    if (Tcl_GetIndexFromObj(interp, objV[0], infoOption, "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
	}
	switch (index)
	{
		case 0: /* exists */
			Tcl_SetObjResult(interp, Tcl_NewBooleanObj(r_ptr->name != 0));
			break;

		case 1: /* icon */
		{
			char buf[128];
			Tcl_SetResult(interp, assign_print2(buf, ASSIGN_MONSTER, r_idx),
				TCL_VOLATILE);
			break;
		}

		case 2: /* memory */
		{
			char *buffer;
			long length;

			buffer = Tcl_Alloc(5 * 1025L);
			length = angtk_roff(r_idx, buffer);
			Tcl_SetObjResult(interp, ExtToUtf_NewStringObj(buffer, length));
			Tcl_Free(buffer);
			break;
		}

		case 3: /* name */
			ExtToUtf_SetResult(interp, r_name + r_ptr->name);
			break;

		case 4: /* unique */
			Tcl_SetObjResult(interp,
				Tcl_NewBooleanObj((r_ptr->flags1 & (RF1_UNIQUE)) != 0));
			break;

		case 5: /* plural */
		{
			char name[160];
			(void) strcpy(name, r_name + r_ptr->name);
			plural_aux(name);
			ExtToUtf_SetResult(interp, name);
			break;
		}
	}

	return TCL_OK;
}

/* option ?arg ...? */
int struct_info_monster_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, monster_type *m_ptr,
	int m_idx)
{
	static char *infoOption[] = {"icon", "memory", "name", "look_mon_desc",
		"monster_desc", "friend", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	Tcl_Obj *resultObjPtr = NULL;
	int index;

	char buf[128], *buffer, m_name[80];
	long length;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

    if (Tcl_GetIndexFromObj(interp, objV[0], infoOption, "option",
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

/* option ?arg ...? */
int struct_info_quest_type(Tcl_Interp *interp, StructType *typePtr,
	int objc, Tcl_Obj *CONST objv[], int objOffset, quest_type *q_ptr,
	int q_idx)
{
	static char *infoOption[] = {"text", NULL};
/*	int objC = objc - objOffset; */
	Tcl_Obj *CONST *objV = objv + objOffset;
	Tcl_Obj *resultObjPtr = NULL;
	int index;

	char buf[1024];
	int i, old_quest;

    if (Tcl_GetIndexFromObj(interp, objV[0], infoOption, "option",
		0, &index) != TCL_OK)
	{
		return TCL_ERROR;
	}
	switch (index)
	{
		case 0: /* text */
		{
			char *s = buf;
			int len = 0;

			/* Clear the text */
			for (i = 0; i < 10; i++)
			{
				quest_text[i][0] = '\0';
			}

			quest_text_line = 0;

			/* Set the quest number temporary */
			old_quest = p_ptr->inside_quest;
			p_ptr->inside_quest = q_idx;

			/* Get the quest text */
			init_flags = INIT_SHOW_TEXT;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);

			/* Reset the old quest number */
			p_ptr->inside_quest = old_quest;

			buf[0] = '\0';
			for (i = 0; i < 10; i++)
			{
				char *t = quest_text[i];
				if (strlen(t))
				{
					if (len &&
						(s[len - 1] != ' ') &&
						(s[len - 1] != '-') &&
						(s[len - 1] != '\n') &&
						(t[0] != ' '))
					{
						if ((s[len - 1] == '.') || (s[len - 1] == '!'))
							len += strcpy_len(s + len, " ");
#ifndef JP
						len += strcpy_len(s + len, " ");
#endif
					}
					len += strcpy_len(s + len, t);
					while (len && (s[len - 1] == ' ')) len--;
				}
				else
				{
					len += strcpy_len(s + len, "\n\n");
				}
			}
		
			/* Strip out trailing blank lines */
			i = len - 1;
			while (buf[i] == '\n') buf[i--] = '\0';

			resultObjPtr = ExtToUtf_NewStringObj(buf, i + 1);
			break;
		}
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
	type.max = MAX_A_IDX;
	type.info = struct_artifact_type;
	type.infoProc = (StructInfoProc) struct_info_artifact_type;
	type.findProc = struct_find_artifact_type;
	type.matchProc = (StructFindMatchProc) struct_match_artifact_type;
	Struct_AddType(g_interp, &type);

	type.name = "feature_type";
	type.elem = (unsigned char *) f_info;
	type.elem_size = sizeof(feature_type);
	type.max = MAX_F_IDX;
	type.info = struct_feature_type;
	type.infoProc = (StructInfoProc) struct_info_feature_type;
	type.findProc = struct_find_feature_type;
	type.matchProc = (StructFindMatchProc) struct_match_feature_type;
	Struct_AddType(g_interp, &type);

	type.name = "monster_race";
	type.elem = (unsigned char *) r_info;
	type.elem_size = sizeof(monster_race);
	type.max = MAX_VALID_R_IDX;
	type.info = struct_monster_race;
	type.infoProc = (StructInfoProc) struct_info_monster_race;
	type.findProc = struct_find_monster_race;
	type.matchProc = (StructFindMatchProc) struct_match_monster_race;
	Struct_AddType(g_interp, &type);

	type.name = "monster_type";
	type.elem = (unsigned char *) m_list;
	type.elem_size = sizeof(monster_type);
	type.max = MAX_M_IDX;
	type.info = struct_monster_type;
	type.infoProc = (StructInfoProc) struct_info_monster_type;
	type.findProc = NULL;
	type.matchProc = (StructFindMatchProc) NULL;
	Struct_AddType(g_interp, &type);

	type.name = "object_kind";
	type.elem = (unsigned char *) k_info;
	type.elem_size = sizeof(object_kind);
	type.max = MAX_K_IDX;
	type.info = struct_object_kind;
	type.infoProc = (StructInfoProc) struct_info_object_kind;
	type.findProc = struct_find_object_kind;
	type.matchProc = (StructFindMatchProc) struct_match_object_kind;
	Struct_AddType(g_interp, &type);

	type.name = "object_type";
	type.elem = (unsigned char *) o_list;
	type.elem_size = sizeof(object_type);
	type.max = MAX_O_IDX;
	type.info = struct_object_type;
	type.infoProc = (StructInfoProc) struct_info_object_type;
	type.findProc = NULL /* struct_find_object_type */;
	type.matchProc = NULL /* (StructFindMatchProc) struct_match_object_type */;
	Struct_AddType(g_interp, &type);

	type.name = "player_class";
	type.elem = (unsigned char *) class_info;
	type.elem_size = sizeof(player_class);
	type.max = MAX_CLASS;
	type.info = struct_player_class;
	type.infoProc = NULL;
	type.findProc = NULL;
	type.matchProc = NULL;
	Struct_AddType(g_interp, &type);

#if 0
	type.name = "player_race";
	type.elem = (unsigned char *) race_info;
	type.elem_size = sizeof(player_race);
	type.max = MAX_P_IDX;
	type.info = struct_player_race;
	type.infoProc = NULL;
	type.findProc = NULL;
	type.matchProc = NULL;
	Struct_AddType(g_interp, &type);
#endif

	type.name = "player_type";
	type.elem = (unsigned char *) p_ptr;
	type.elem_size = sizeof(player_type);
	type.max = 1;
	type.info = struct_player_type;
	type.infoProc = NULL;
	type.findProc = NULL;
	type.matchProc = NULL;
	Struct_AddType(g_interp, &type);

	type.name = "building_type";
	type.elem = (unsigned char *) building;
	type.elem_size = sizeof(building_type);
	type.max = MAX_BLDG;
	type.info = struct_building_type;
	type.infoProc = NULL;
	type.findProc = NULL;
	type.matchProc = NULL;
	Struct_AddType(g_interp, &type);

	type.name = "quest_type";
	type.elem = (unsigned char *) q_list;
	type.elem_size = sizeof(quest_type);
	type.max = MAX_QUESTS;
	type.info = struct_quest_type;
	type.infoProc = (StructInfoProc) struct_info_quest_type;
	type.findProc = NULL;
	type.matchProc = NULL;
	Struct_AddType(g_interp, &type);

	type.name = "inventory";
	type.elem = (unsigned char *) inventory;
	type.elem_size = sizeof(object_type);
	type.max = INVEN_TOTAL;
	type.info = struct_object_type;
	type.infoProc = (StructInfoProc) struct_info_object_type;
	type.findProc = NULL;
	type.matchProc = NULL;
	Struct_AddType(g_interp, &type);
}
