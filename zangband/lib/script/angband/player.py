# This file was created automatically by SWIG.
import playerc
import new
class player_type:
    def __init__(self,this):
        self.this = this

    __setmethods__ = {
        "oldpy" : playerc.player_type_oldpy_set,
        "oldpx" : playerc.player_type_oldpx_set,
        "psex" : playerc.player_type_psex_set,
        "prace" : playerc.player_type_prace_set,
        "pclass" : playerc.player_type_pclass_set,
        "realm1" : playerc.player_type_realm1_set,
        "realm2" : playerc.player_type_realm2_set,
        "oops" : playerc.player_type_oops_set,
        "hitdie" : playerc.player_type_hitdie_set,
        "expfact" : playerc.player_type_expfact_set,
        "age" : playerc.player_type_age_set,
        "ht" : playerc.player_type_ht_set,
        "wt" : playerc.player_type_wt_set,
        "sc" : playerc.player_type_sc_set,
        "au" : playerc.player_type_au_set,
        "max_exp" : playerc.player_type_max_exp_set,
        "exp" : playerc.player_type_exp_set,
        "exp_frac" : playerc.player_type_exp_frac_set,
        "level" : playerc.player_type_level_set,
        "town_num" : playerc.player_type_town_num_set,
        "arena_number" : playerc.player_type_arena_number_set,
        "inside_arena" : playerc.player_type_inside_arena_set,
        "inside_quest" : playerc.player_type_inside_quest_set,
        "wilderness_x" : playerc.player_type_wilderness_x_set,
        "wilderness_y" : playerc.player_type_wilderness_y_set,
        "mhp" : playerc.player_type_mhp_set,
        "chp" : playerc.player_type_chp_set,
        "chp_frac" : playerc.player_type_chp_frac_set,
        "msp" : playerc.player_type_msp_set,
        "csp" : playerc.player_type_csp_set,
        "csp_frac" : playerc.player_type_csp_frac_set,
        "max_plv" : playerc.player_type_max_plv_set,
        "max_dlv" : playerc.player_type_max_dlv_set,
        "fast" : playerc.player_type_fast_set,
        "slow" : playerc.player_type_slow_set,
        "blind" : playerc.player_type_blind_set,
        "paralyzed" : playerc.player_type_paralyzed_set,
        "confused" : playerc.player_type_confused_set,
        "afraid" : playerc.player_type_afraid_set,
        "image" : playerc.player_type_image_set,
        "poisoned" : playerc.player_type_poisoned_set,
        "cut" : playerc.player_type_cut_set,
        "stun" : playerc.player_type_stun_set,
        "protevil" : playerc.player_type_protevil_set,
        "invuln" : playerc.player_type_invuln_set,
        "hero" : playerc.player_type_hero_set,
        "shero" : playerc.player_type_shero_set,
        "shield" : playerc.player_type_shield_set,
        "blessed" : playerc.player_type_blessed_set,
        "tim_invis" : playerc.player_type_tim_invis_set,
        "tim_infra" : playerc.player_type_tim_infra_set,
        "oppose_acid" : playerc.player_type_oppose_acid_set,
        "oppose_elec" : playerc.player_type_oppose_elec_set,
        "oppose_fire" : playerc.player_type_oppose_fire_set,
        "oppose_cold" : playerc.player_type_oppose_cold_set,
        "oppose_pois" : playerc.player_type_oppose_pois_set,
        "tim_esp" : playerc.player_type_tim_esp_set,
        "wraith_form" : playerc.player_type_wraith_form_set,
        "resist_magic" : playerc.player_type_resist_magic_set,
        "tim_xtra1" : playerc.player_type_tim_xtra1_set,
        "tim_xtra2" : playerc.player_type_tim_xtra2_set,
        "tim_xtra3" : playerc.player_type_tim_xtra3_set,
        "tim_xtra4" : playerc.player_type_tim_xtra4_set,
        "tim_xtra5" : playerc.player_type_tim_xtra5_set,
        "tim_xtra6" : playerc.player_type_tim_xtra6_set,
        "tim_xtra7" : playerc.player_type_tim_xtra7_set,
        "tim_xtra8" : playerc.player_type_tim_xtra8_set,
        "chaos_patron" : playerc.player_type_chaos_patron_set,
        "muta1" : playerc.player_type_muta1_set,
        "muta2" : playerc.player_type_muta2_set,
        "muta3" : playerc.player_type_muta3_set,
        "word_recall" : playerc.player_type_word_recall_set,
        "energy" : playerc.player_type_energy_set,
        "food" : playerc.player_type_food_set,
        "confusing" : playerc.player_type_confusing_set,
        "searching" : playerc.player_type_searching_set,
        "new_spells" : playerc.player_type_new_spells_set,
        "old_spells" : playerc.player_type_old_spells_set,
        "old_cumber_armor" : playerc.player_type_old_cumber_armor_set,
        "old_cumber_glove" : playerc.player_type_old_cumber_glove_set,
        "old_heavy_wield" : playerc.player_type_old_heavy_wield_set,
        "old_heavy_shoot" : playerc.player_type_old_heavy_shoot_set,
        "old_icky_wield" : playerc.player_type_old_icky_wield_set,
        "old_lite" : playerc.player_type_old_lite_set,
        "old_view" : playerc.player_type_old_view_set,
        "old_food_aux" : playerc.player_type_old_food_aux_set,
        "cumber_armor" : playerc.player_type_cumber_armor_set,
        "cumber_glove" : playerc.player_type_cumber_glove_set,
        "heavy_wield" : playerc.player_type_heavy_wield_set,
        "heavy_shoot" : playerc.player_type_heavy_shoot_set,
        "icky_wield" : playerc.player_type_icky_wield_set,
        "cur_lite" : playerc.player_type_cur_lite_set,
        "notice" : playerc.player_type_notice_set,
        "update" : playerc.player_type_update_set,
        "redraw" : playerc.player_type_redraw_set,
        "window" : playerc.player_type_window_set,
        "immune_acid" : playerc.player_type_immune_acid_set,
        "immune_elec" : playerc.player_type_immune_elec_set,
        "immune_fire" : playerc.player_type_immune_fire_set,
        "immune_cold" : playerc.player_type_immune_cold_set,
        "resist_acid" : playerc.player_type_resist_acid_set,
        "resist_elec" : playerc.player_type_resist_elec_set,
        "resist_fire" : playerc.player_type_resist_fire_set,
        "resist_cold" : playerc.player_type_resist_cold_set,
        "resist_pois" : playerc.player_type_resist_pois_set,
        "resist_conf" : playerc.player_type_resist_conf_set,
        "resist_sound" : playerc.player_type_resist_sound_set,
        "resist_lite" : playerc.player_type_resist_lite_set,
        "resist_dark" : playerc.player_type_resist_dark_set,
        "resist_chaos" : playerc.player_type_resist_chaos_set,
        "resist_disen" : playerc.player_type_resist_disen_set,
        "resist_shard" : playerc.player_type_resist_shard_set,
        "resist_nexus" : playerc.player_type_resist_nexus_set,
        "resist_blind" : playerc.player_type_resist_blind_set,
        "resist_neth" : playerc.player_type_resist_neth_set,
        "resist_fear" : playerc.player_type_resist_fear_set,
        "reflect" : playerc.player_type_reflect_set,
        "sh_fire" : playerc.player_type_sh_fire_set,
        "sh_elec" : playerc.player_type_sh_elec_set,
        "anti_magic" : playerc.player_type_anti_magic_set,
        "anti_tele" : playerc.player_type_anti_tele_set,
        "sustain_str" : playerc.player_type_sustain_str_set,
        "sustain_int" : playerc.player_type_sustain_int_set,
        "sustain_wis" : playerc.player_type_sustain_wis_set,
        "sustain_dex" : playerc.player_type_sustain_dex_set,
        "sustain_con" : playerc.player_type_sustain_con_set,
        "sustain_chr" : playerc.player_type_sustain_chr_set,
        "aggravate" : playerc.player_type_aggravate_set,
        "teleport" : playerc.player_type_teleport_set,
        "exp_drain" : playerc.player_type_exp_drain_set,
        "ffall" : playerc.player_type_ffall_set,
        "lite" : playerc.player_type_lite_set,
        "free_act" : playerc.player_type_free_act_set,
        "see_inv" : playerc.player_type_see_inv_set,
        "regenerate" : playerc.player_type_regenerate_set,
        "hold_life" : playerc.player_type_hold_life_set,
        "telepathy" : playerc.player_type_telepathy_set,
        "slow_digest" : playerc.player_type_slow_digest_set,
        "bless_blade" : playerc.player_type_bless_blade_set,
        "xtra_might" : playerc.player_type_xtra_might_set,
        "impact" : playerc.player_type_impact_set,
        "dis_to_h" : playerc.player_type_dis_to_h_set,
        "dis_to_d" : playerc.player_type_dis_to_d_set,
        "dis_to_a" : playerc.player_type_dis_to_a_set,
        "dis_ac" : playerc.player_type_dis_ac_set,
        "to_h" : playerc.player_type_to_h_set,
        "to_d" : playerc.player_type_to_d_set,
        "to_a" : playerc.player_type_to_a_set,
        "ac" : playerc.player_type_ac_set,
        "see_infra" : playerc.player_type_see_infra_set,
        "skill_dis" : playerc.player_type_skill_dis_set,
        "skill_dev" : playerc.player_type_skill_dev_set,
        "skill_sav" : playerc.player_type_skill_sav_set,
        "skill_stl" : playerc.player_type_skill_stl_set,
        "skill_srh" : playerc.player_type_skill_srh_set,
        "skill_fos" : playerc.player_type_skill_fos_set,
        "skill_thn" : playerc.player_type_skill_thn_set,
        "skill_thb" : playerc.player_type_skill_thb_set,
        "skill_tht" : playerc.player_type_skill_tht_set,
        "skill_dig" : playerc.player_type_skill_dig_set,
        "num_blow" : playerc.player_type_num_blow_set,
        "num_fire" : playerc.player_type_num_fire_set,
        "tval_xtra" : playerc.player_type_tval_xtra_set,
        "tval_ammo" : playerc.player_type_tval_ammo_set,
        "pspeed" : playerc.player_type_pspeed_set,
        "pet_follow_distance" : playerc.player_type_pet_follow_distance_set,
        "pet_open_doors" : playerc.player_type_pet_open_doors_set,
        "pet_pickup_items" : playerc.player_type_pet_pickup_items_set,
        "exit_bldg" : playerc.player_type_exit_bldg_set,
        "leftbldg" : playerc.player_type_leftbldg_set,
        "leaving" : playerc.player_type_leaving_set,
        "name" : playerc.player_type_name_set,
        "dun_level" : playerc.player_type_dun_level_set,
        "py" : playerc.player_type_py_set,
        "px" : playerc.player_type_px_set,
        "player_base" : playerc.player_type_player_base_set,
        "died_from" : playerc.player_type_died_from_set,
        "history" : playerc.player_type_history_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = player_type.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "oldpy" : playerc.player_type_oldpy_get,
        "oldpx" : playerc.player_type_oldpx_get,
        "psex" : playerc.player_type_psex_get,
        "prace" : playerc.player_type_prace_get,
        "pclass" : playerc.player_type_pclass_get,
        "realm1" : playerc.player_type_realm1_get,
        "realm2" : playerc.player_type_realm2_get,
        "oops" : playerc.player_type_oops_get,
        "hitdie" : playerc.player_type_hitdie_get,
        "expfact" : playerc.player_type_expfact_get,
        "age" : playerc.player_type_age_get,
        "ht" : playerc.player_type_ht_get,
        "wt" : playerc.player_type_wt_get,
        "sc" : playerc.player_type_sc_get,
        "au" : playerc.player_type_au_get,
        "max_exp" : playerc.player_type_max_exp_get,
        "exp" : playerc.player_type_exp_get,
        "exp_frac" : playerc.player_type_exp_frac_get,
        "level" : playerc.player_type_level_get,
        "town_num" : playerc.player_type_town_num_get,
        "arena_number" : playerc.player_type_arena_number_get,
        "inside_arena" : playerc.player_type_inside_arena_get,
        "inside_quest" : playerc.player_type_inside_quest_get,
        "rewards" : playerc.player_type_rewards_get,
        "wilderness_x" : playerc.player_type_wilderness_x_get,
        "wilderness_y" : playerc.player_type_wilderness_y_get,
        "mhp" : playerc.player_type_mhp_get,
        "chp" : playerc.player_type_chp_get,
        "chp_frac" : playerc.player_type_chp_frac_get,
        "msp" : playerc.player_type_msp_get,
        "csp" : playerc.player_type_csp_get,
        "csp_frac" : playerc.player_type_csp_frac_get,
        "max_plv" : playerc.player_type_max_plv_get,
        "max_dlv" : playerc.player_type_max_dlv_get,
        "stat_max" : playerc.player_type_stat_max_get,
        "fast" : playerc.player_type_fast_get,
        "slow" : playerc.player_type_slow_get,
        "blind" : playerc.player_type_blind_get,
        "paralyzed" : playerc.player_type_paralyzed_get,
        "confused" : playerc.player_type_confused_get,
        "afraid" : playerc.player_type_afraid_get,
        "image" : playerc.player_type_image_get,
        "poisoned" : playerc.player_type_poisoned_get,
        "cut" : playerc.player_type_cut_get,
        "stun" : playerc.player_type_stun_get,
        "protevil" : playerc.player_type_protevil_get,
        "invuln" : playerc.player_type_invuln_get,
        "hero" : playerc.player_type_hero_get,
        "shero" : playerc.player_type_shero_get,
        "shield" : playerc.player_type_shield_get,
        "blessed" : playerc.player_type_blessed_get,
        "tim_invis" : playerc.player_type_tim_invis_get,
        "tim_infra" : playerc.player_type_tim_infra_get,
        "oppose_acid" : playerc.player_type_oppose_acid_get,
        "oppose_elec" : playerc.player_type_oppose_elec_get,
        "oppose_fire" : playerc.player_type_oppose_fire_get,
        "oppose_cold" : playerc.player_type_oppose_cold_get,
        "oppose_pois" : playerc.player_type_oppose_pois_get,
        "tim_esp" : playerc.player_type_tim_esp_get,
        "wraith_form" : playerc.player_type_wraith_form_get,
        "resist_magic" : playerc.player_type_resist_magic_get,
        "tim_xtra1" : playerc.player_type_tim_xtra1_get,
        "tim_xtra2" : playerc.player_type_tim_xtra2_get,
        "tim_xtra3" : playerc.player_type_tim_xtra3_get,
        "tim_xtra4" : playerc.player_type_tim_xtra4_get,
        "tim_xtra5" : playerc.player_type_tim_xtra5_get,
        "tim_xtra6" : playerc.player_type_tim_xtra6_get,
        "tim_xtra7" : playerc.player_type_tim_xtra7_get,
        "tim_xtra8" : playerc.player_type_tim_xtra8_get,
        "chaos_patron" : playerc.player_type_chaos_patron_get,
        "muta1" : playerc.player_type_muta1_get,
        "muta2" : playerc.player_type_muta2_get,
        "muta3" : playerc.player_type_muta3_get,
        "word_recall" : playerc.player_type_word_recall_get,
        "energy" : playerc.player_type_energy_get,
        "food" : playerc.player_type_food_get,
        "confusing" : playerc.player_type_confusing_get,
        "searching" : playerc.player_type_searching_get,
        "new_spells" : playerc.player_type_new_spells_get,
        "old_spells" : playerc.player_type_old_spells_get,
        "old_cumber_armor" : playerc.player_type_old_cumber_armor_get,
        "old_cumber_glove" : playerc.player_type_old_cumber_glove_get,
        "old_heavy_wield" : playerc.player_type_old_heavy_wield_get,
        "old_heavy_shoot" : playerc.player_type_old_heavy_shoot_get,
        "old_icky_wield" : playerc.player_type_old_icky_wield_get,
        "old_lite" : playerc.player_type_old_lite_get,
        "old_view" : playerc.player_type_old_view_get,
        "old_food_aux" : playerc.player_type_old_food_aux_get,
        "cumber_armor" : playerc.player_type_cumber_armor_get,
        "cumber_glove" : playerc.player_type_cumber_glove_get,
        "heavy_wield" : playerc.player_type_heavy_wield_get,
        "heavy_shoot" : playerc.player_type_heavy_shoot_get,
        "icky_wield" : playerc.player_type_icky_wield_get,
        "cur_lite" : playerc.player_type_cur_lite_get,
        "notice" : playerc.player_type_notice_get,
        "update" : playerc.player_type_update_get,
        "redraw" : playerc.player_type_redraw_get,
        "window" : playerc.player_type_window_get,
        "stat_use" : playerc.player_type_stat_use_get,
        "stat_top" : playerc.player_type_stat_top_get,
        "stat_add" : playerc.player_type_stat_add_get,
        "stat_ind" : playerc.player_type_stat_ind_get,
        "immune_acid" : playerc.player_type_immune_acid_get,
        "immune_elec" : playerc.player_type_immune_elec_get,
        "immune_fire" : playerc.player_type_immune_fire_get,
        "immune_cold" : playerc.player_type_immune_cold_get,
        "resist_acid" : playerc.player_type_resist_acid_get,
        "resist_elec" : playerc.player_type_resist_elec_get,
        "resist_fire" : playerc.player_type_resist_fire_get,
        "resist_cold" : playerc.player_type_resist_cold_get,
        "resist_pois" : playerc.player_type_resist_pois_get,
        "resist_conf" : playerc.player_type_resist_conf_get,
        "resist_sound" : playerc.player_type_resist_sound_get,
        "resist_lite" : playerc.player_type_resist_lite_get,
        "resist_dark" : playerc.player_type_resist_dark_get,
        "resist_chaos" : playerc.player_type_resist_chaos_get,
        "resist_disen" : playerc.player_type_resist_disen_get,
        "resist_shard" : playerc.player_type_resist_shard_get,
        "resist_nexus" : playerc.player_type_resist_nexus_get,
        "resist_blind" : playerc.player_type_resist_blind_get,
        "resist_neth" : playerc.player_type_resist_neth_get,
        "resist_fear" : playerc.player_type_resist_fear_get,
        "reflect" : playerc.player_type_reflect_get,
        "sh_fire" : playerc.player_type_sh_fire_get,
        "sh_elec" : playerc.player_type_sh_elec_get,
        "anti_magic" : playerc.player_type_anti_magic_get,
        "anti_tele" : playerc.player_type_anti_tele_get,
        "sustain_str" : playerc.player_type_sustain_str_get,
        "sustain_int" : playerc.player_type_sustain_int_get,
        "sustain_wis" : playerc.player_type_sustain_wis_get,
        "sustain_dex" : playerc.player_type_sustain_dex_get,
        "sustain_con" : playerc.player_type_sustain_con_get,
        "sustain_chr" : playerc.player_type_sustain_chr_get,
        "aggravate" : playerc.player_type_aggravate_get,
        "teleport" : playerc.player_type_teleport_get,
        "exp_drain" : playerc.player_type_exp_drain_get,
        "ffall" : playerc.player_type_ffall_get,
        "lite" : playerc.player_type_lite_get,
        "free_act" : playerc.player_type_free_act_get,
        "see_inv" : playerc.player_type_see_inv_get,
        "regenerate" : playerc.player_type_regenerate_get,
        "hold_life" : playerc.player_type_hold_life_get,
        "telepathy" : playerc.player_type_telepathy_get,
        "slow_digest" : playerc.player_type_slow_digest_get,
        "bless_blade" : playerc.player_type_bless_blade_get,
        "xtra_might" : playerc.player_type_xtra_might_get,
        "impact" : playerc.player_type_impact_get,
        "dis_to_h" : playerc.player_type_dis_to_h_get,
        "dis_to_d" : playerc.player_type_dis_to_d_get,
        "dis_to_a" : playerc.player_type_dis_to_a_get,
        "dis_ac" : playerc.player_type_dis_ac_get,
        "to_h" : playerc.player_type_to_h_get,
        "to_d" : playerc.player_type_to_d_get,
        "to_a" : playerc.player_type_to_a_get,
        "ac" : playerc.player_type_ac_get,
        "see_infra" : playerc.player_type_see_infra_get,
        "skill_dis" : playerc.player_type_skill_dis_get,
        "skill_dev" : playerc.player_type_skill_dev_get,
        "skill_sav" : playerc.player_type_skill_sav_get,
        "skill_stl" : playerc.player_type_skill_stl_get,
        "skill_srh" : playerc.player_type_skill_srh_get,
        "skill_fos" : playerc.player_type_skill_fos_get,
        "skill_thn" : playerc.player_type_skill_thn_get,
        "skill_thb" : playerc.player_type_skill_thb_get,
        "skill_tht" : playerc.player_type_skill_tht_get,
        "skill_dig" : playerc.player_type_skill_dig_get,
        "num_blow" : playerc.player_type_num_blow_get,
        "num_fire" : playerc.player_type_num_fire_get,
        "tval_xtra" : playerc.player_type_tval_xtra_get,
        "tval_ammo" : playerc.player_type_tval_ammo_get,
        "pspeed" : playerc.player_type_pspeed_get,
        "pet_follow_distance" : playerc.player_type_pet_follow_distance_get,
        "pet_open_doors" : playerc.player_type_pet_open_doors_get,
        "pet_pickup_items" : playerc.player_type_pet_pickup_items_get,
        "exit_bldg" : playerc.player_type_exit_bldg_get,
        "leftbldg" : playerc.player_type_leftbldg_get,
        "leaving" : playerc.player_type_leaving_get,
        "name" : playerc.player_type_name_get,
        "dun_level" : playerc.player_type_dun_level_get,
        "py" : playerc.player_type_py_get,
        "px" : playerc.player_type_px_get,
        "player_base" : playerc.player_type_player_base_get,
        "died_from" : playerc.player_type_died_from_get,
        "history" : playerc.player_type_history_get,
    }
    def __getattr__(self,name):
        method = player_type.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_type instance at %s>" % (self.this,)
class player_typePtr(player_type):
    def __init__(self,this):
        self.this = this
        self.thisown = 0


player_type.stat_cur_get = new.instancemethod(playerc.player_type_stat_cur_get, None, player_type)
player_type.inventory = new.instancemethod(playerc.player_type_inventory, None, player_type)
player_type.inven_item_increase = new.instancemethod(playerc.player_type_inven_item_increase, None, player_type)
player_type.inven_item_optimize = new.instancemethod(playerc.player_type_inven_item_optimize, None, player_type)
player_type.carry = new.instancemethod(playerc.player_type_carry, None, player_type)
player_type.place = new.instancemethod(playerc.player_type_place, None, player_type)
player_type.teleport_to = new.instancemethod(playerc.player_type_teleport_to, None, player_type)
player_type.teleport_level = new.instancemethod(playerc.player_type_teleport_level, None, player_type)
player_type.recall = new.instancemethod(playerc.player_type_recall, None, player_type)
player_type.word_of_recall = new.instancemethod(playerc.player_type_word_of_recall, None, player_type)
player_type.disenchant = new.instancemethod(playerc.player_type_disenchant, None, player_type)
player_type.mutate_player = new.instancemethod(playerc.player_type_mutate_player, None, player_type)
player_type.nexus = new.instancemethod(playerc.player_type_nexus, None, player_type)
player_type.phlogiston = new.instancemethod(playerc.player_type_phlogiston, None, player_type)
player_type.brand_weapon = new.instancemethod(playerc.player_type_brand_weapon, None, player_type)
player_type.call_the_void = new.instancemethod(playerc.player_type_call_the_void, None, player_type)
player_type.fetch = new.instancemethod(playerc.player_type_fetch, None, player_type)
player_type.alter_reality = new.instancemethod(playerc.player_type_alter_reality, None, player_type)
player_type.set_afraid = new.instancemethod(playerc.player_type_set_afraid, None, player_type)
player_type.set_shero = new.instancemethod(playerc.player_type_set_shero, None, player_type)
player_type.hp_player = new.instancemethod(playerc.player_type_hp_player, None, player_type)
player_type.gain_exp = new.instancemethod(playerc.player_type_gain_exp, None, player_type)



#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

cvar = playerc.cvar
INVEN_PACK = playerc.INVEN_PACK
INVEN_WIELD = playerc.INVEN_WIELD
INVEN_BOW = playerc.INVEN_BOW
INVEN_LEFT = playerc.INVEN_LEFT
INVEN_RIGHT = playerc.INVEN_RIGHT
INVEN_NECK = playerc.INVEN_NECK
INVEN_LITE = playerc.INVEN_LITE
INVEN_BODY = playerc.INVEN_BODY
INVEN_OUTER = playerc.INVEN_OUTER
INVEN_ARM = playerc.INVEN_ARM
INVEN_HEAD = playerc.INVEN_HEAD
INVEN_HANDS = playerc.INVEN_HANDS
INVEN_FEET = playerc.INVEN_FEET
INVEN_TOTAL = playerc.INVEN_TOTAL
PY_MAX_EXP = playerc.PY_MAX_EXP
PY_MAX_GOLD = playerc.PY_MAX_GOLD
PY_MAX_LEVEL = playerc.PY_MAX_LEVEL
PY_FOOD_MAX = playerc.PY_FOOD_MAX
PY_FOOD_FULL = playerc.PY_FOOD_FULL
PY_FOOD_ALERT = playerc.PY_FOOD_ALERT
PY_FOOD_WEAK = playerc.PY_FOOD_WEAK
PY_FOOD_FAINT = playerc.PY_FOOD_FAINT
PY_FOOD_STARVE = playerc.PY_FOOD_STARVE
PY_REGEN_NORMAL = playerc.PY_REGEN_NORMAL
PY_REGEN_WEAK = playerc.PY_REGEN_WEAK
PY_REGEN_FAINT = playerc.PY_REGEN_FAINT
PY_REGEN_HPBASE = playerc.PY_REGEN_HPBASE
PY_REGEN_MNBASE = playerc.PY_REGEN_MNBASE
A_STR = playerc.A_STR
A_INT = playerc.A_INT
A_WIS = playerc.A_WIS
A_DEX = playerc.A_DEX
A_CON = playerc.A_CON
A_CHR = playerc.A_CHR
SEX_FEMALE = playerc.SEX_FEMALE
SEX_MALE = playerc.SEX_MALE
