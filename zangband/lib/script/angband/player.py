# This file was created automatically by SWIG.
import playerc
class player_type:
    def __init__(self,this):
        self.this = this

    def stat_cur_get(*args):
        val = apply(playerc.player_type_stat_cur_get,args)
        return val
    def inventory(*args):
        val = apply(playerc.player_type_inventory,args)
        return val
    def inven_item_increase(*args):
        val = apply(playerc.player_type_inven_item_increase,args)
        return val
    def inven_item_optimize(*args):
        val = apply(playerc.player_type_inven_item_optimize,args)
        return val
    def carry(*args):
        val = apply(playerc.player_type_carry,args)
        return val
    def place(*args):
        val = apply(playerc.player_type_place,args)
        return val
    def teleport(*args):
        val = apply(playerc.player_type_teleport,args)
        return val
    def teleport_to(*args):
        val = apply(playerc.player_type_teleport_to,args)
        return val
    def teleport_level(*args):
        val = apply(playerc.player_type_teleport_level,args)
        return val
    def recall(*args):
        val = apply(playerc.player_type_recall,args)
        return val
    def word_of_recall(*args):
        val = apply(playerc.player_type_word_of_recall,args)
        return val
    def disenchant(*args):
        val = apply(playerc.player_type_disenchant,args)
        return val
    def mutate_player(*args):
        val = apply(playerc.player_type_mutate_player,args)
        return val
    def nexus(*args):
        val = apply(playerc.player_type_nexus,args)
        return val
    def phlogiston(*args):
        val = apply(playerc.player_type_phlogiston,args)
        return val
    def brand_weapon(*args):
        val = apply(playerc.player_type_brand_weapon,args)
        return val
    def call_the_void(*args):
        val = apply(playerc.player_type_call_the_void,args)
        return val
    def fetch(*args):
        val = apply(playerc.player_type_fetch,args)
        return val
    def alter_reality(*args):
        val = apply(playerc.player_type_alter_reality,args)
        return val
    def set_afraid(*args):
        val = apply(playerc.player_type_set_afraid,args)
        return val
    def set_shero(*args):
        val = apply(playerc.player_type_set_shero,args)
        return val
    def hp_player(*args):
        val = apply(playerc.player_type_hp_player,args)
        return val
    def set_blind(*args):
        val = apply(playerc.player_type_set_blind,args)
        return val
    def set_confused(*args):
        val = apply(playerc.player_type_set_confused,args)
        return val
    def set_poisoned(*args):
        val = apply(playerc.player_type_set_poisoned,args)
        return val
    def set_paralyzed(*args):
        val = apply(playerc.player_type_set_paralyzed,args)
        return val
    def set_image(*args):
        val = apply(playerc.player_type_set_image,args)
        return val
    def set_fast(*args):
        val = apply(playerc.player_type_set_fast,args)
        return val
    def set_slow(*args):
        val = apply(playerc.player_type_set_slow,args)
        return val
    def set_shield(*args):
        val = apply(playerc.player_type_set_shield,args)
        return val
    def set_blessed(*args):
        val = apply(playerc.player_type_set_blessed,args)
        return val
    def set_hero(*args):
        val = apply(playerc.player_type_set_hero,args)
        return val
    def set_protevil(*args):
        val = apply(playerc.player_type_set_protevil,args)
        return val
    def set_invuln(*args):
        val = apply(playerc.player_type_set_invuln,args)
        return val
    def set_tim_invis(*args):
        val = apply(playerc.player_type_set_tim_invis,args)
        return val
    def set_tim_infra(*args):
        val = apply(playerc.player_type_set_tim_infra,args)
        return val
    def set_oppose_acid(*args):
        val = apply(playerc.player_type_set_oppose_acid,args)
        return val
    def set_oppose_elec(*args):
        val = apply(playerc.player_type_set_oppose_elec,args)
        return val
    def set_oppose_fire(*args):
        val = apply(playerc.player_type_set_oppose_fire,args)
        return val
    def set_oppose_cold(*args):
        val = apply(playerc.player_type_set_oppose_cold,args)
        return val
    def set_oppose_pois(*args):
        val = apply(playerc.player_type_set_oppose_pois,args)
        return val
    def set_stun(*args):
        val = apply(playerc.player_type_set_stun,args)
        return val
    def set_cut(*args):
        val = apply(playerc.player_type_set_cut,args)
        return val
    def set_food(*args):
        val = apply(playerc.player_type_set_food,args)
        return val
    def inc_stat(*args):
        val = apply(playerc.player_type_inc_stat,args)
        return val
    def dec_stat(*args):
        val = apply(playerc.player_type_dec_stat,args)
        return val
    def res_stat(*args):
        val = apply(playerc.player_type_res_stat,args)
        return val
    def do_dec_stat(*args):
        val = apply(playerc.player_type_do_dec_stat,args)
        return val
    def do_res_stat(*args):
        val = apply(playerc.player_type_do_res_stat,args)
        return val
    def do_inc_stat(*args):
        val = apply(playerc.player_type_do_inc_stat,args)
        return val
    def restore_level(*args):
        val = apply(playerc.player_type_restore_level,args)
        return val
    def lose_all_info(*args):
        val = apply(playerc.player_type_lose_all_info,args)
        return val
    def gain_exp(*args):
        val = apply(playerc.player_type_gain_exp,args)
        return val
    def lose_exp(*args):
        val = apply(playerc.player_type_lose_exp,args)
        return val
    def do_poly_self(*args):
        val = apply(playerc.player_type_do_poly_self,args)
        return val
    __setmethods__ = {
        "px" : playerc.player_type_px_set,
        "py" : playerc.player_type_py_set,
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
        "max_depth" : playerc.player_type_max_depth_set,
        "depth" : playerc.player_type_depth_set,
        "max_lev" : playerc.player_type_max_lev_set,
        "lev" : playerc.player_type_lev_set,
        "max_exp" : playerc.player_type_max_exp_set,
        "exp" : playerc.player_type_exp_set,
        "exp_frac" : playerc.player_type_exp_frac_set,
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
        "spell_learned1" : playerc.player_type_spell_learned1_set,
        "spell_learned2" : playerc.player_type_spell_learned2_set,
        "spell_worked1" : playerc.player_type_spell_worked1_set,
        "spell_worked2" : playerc.player_type_spell_worked2_set,
        "spell_forgotten1" : playerc.player_type_spell_forgotten1_set,
        "spell_forgotten2" : playerc.player_type_spell_forgotten2_set,
        "total_winner" : playerc.player_type_total_winner_set,
        "panic_save" : playerc.player_type_panic_save_set,
        "noscore" : playerc.player_type_noscore_set,
        "is_dead" : playerc.player_type_is_dead_set,
        "wizard" : playerc.player_type_wizard_set,
        "playing" : playerc.player_type_playing_set,
        "leaving" : playerc.player_type_leaving_set,
        "leaving_dungeon" : playerc.player_type_leaving_dungeon_set,
        "exit_bldg" : playerc.player_type_exit_bldg_set,
        "leftbldg" : playerc.player_type_leftbldg_set,
        "create_up_stair" : playerc.player_type_create_up_stair_set,
        "create_down_stair" : playerc.player_type_create_down_stair_set,
        "align" : playerc.player_type_align_set,
        "detectx" : playerc.player_type_detectx_set,
        "detecty" : playerc.player_type_detecty_set,
        "detected" : playerc.player_type_detected_set,
        "total_weight" : playerc.player_type_total_weight_set,
        "inven_cnt" : playerc.player_type_inven_cnt_set,
        "equip_cnt" : playerc.player_type_equip_cnt_set,
        "target_set" : playerc.player_type_target_set_set,
        "target_who" : playerc.player_type_target_who_set,
        "target_row" : playerc.player_type_target_row_set,
        "target_col" : playerc.player_type_target_col_set,
        "health_who" : playerc.player_type_health_who_set,
        "monster_race_idx" : playerc.player_type_monster_race_idx_set,
        "object_kind_idx" : playerc.player_type_object_kind_idx_set,
        "energy_use" : playerc.player_type_energy_use_set,
        "resting" : playerc.player_type_resting_set,
        "running" : playerc.player_type_running_set,
        "run_cur_dir" : playerc.player_type_run_cur_dir_set,
        "run_old_dir" : playerc.player_type_run_old_dir_set,
        "run_unused" : playerc.player_type_run_unused_set,
        "run_open_area" : playerc.player_type_run_open_area_set,
        "run_break_right" : playerc.player_type_run_break_right_set,
        "run_break_left" : playerc.player_type_run_break_left_set,
        "command_cmd" : playerc.player_type_command_cmd_set,
        "command_arg" : playerc.player_type_command_arg_set,
        "command_rep" : playerc.player_type_command_rep_set,
        "command_dir" : playerc.player_type_command_dir_set,
        "command_see" : playerc.player_type_command_see_set,
        "command_wrk" : playerc.player_type_command_wrk_set,
        "command_new" : playerc.player_type_command_new_set,
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
        "resist_fear" : playerc.player_type_resist_fear_set,
        "resist_lite" : playerc.player_type_resist_lite_set,
        "resist_dark" : playerc.player_type_resist_dark_set,
        "resist_blind" : playerc.player_type_resist_blind_set,
        "resist_confu" : playerc.player_type_resist_confu_set,
        "resist_sound" : playerc.player_type_resist_sound_set,
        "resist_shard" : playerc.player_type_resist_shard_set,
        "resist_nexus" : playerc.player_type_resist_nexus_set,
        "resist_nethr" : playerc.player_type_resist_nethr_set,
        "resist_chaos" : playerc.player_type_resist_chaos_set,
        "resist_disen" : playerc.player_type_resist_disen_set,
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
        "slow_digest" : playerc.player_type_slow_digest_set,
        "ffall" : playerc.player_type_ffall_set,
        "lite" : playerc.player_type_lite_set,
        "regenerate" : playerc.player_type_regenerate_set,
        "telepathy" : playerc.player_type_telepathy_set,
        "see_inv" : playerc.player_type_see_inv_set,
        "free_act" : playerc.player_type_free_act_set,
        "hold_life" : playerc.player_type_hold_life_set,
        "impact" : playerc.player_type_impact_set,
        "aggravate" : playerc.player_type_aggravate_set,
        "teleport" : playerc.player_type_teleport_set,
        "exp_drain" : playerc.player_type_exp_drain_set,
        "bless_blade" : playerc.player_type_bless_blade_set,
        "xtra_might" : playerc.player_type_xtra_might_set,
        "pass_wall" : playerc.player_type_pass_wall_set,
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
        "noise" : playerc.player_type_noise_set,
        "num_blow" : playerc.player_type_num_blow_set,
        "num_fire" : playerc.player_type_num_fire_set,
        "tval_xtra" : playerc.player_type_tval_xtra_set,
        "ammo_mult" : playerc.player_type_ammo_mult_set,
        "ammo_tval" : playerc.player_type_ammo_tval_set,
        "pspeed" : playerc.player_type_pspeed_set,
        "pet_follow_distance" : playerc.player_type_pet_follow_distance_set,
        "pet_open_doors" : playerc.player_type_pet_open_doors_set,
        "pet_pickup_items" : playerc.player_type_pet_pickup_items_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = player_type.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "px" : playerc.player_type_px_get,
        "py" : playerc.player_type_py_get,
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
        "max_depth" : playerc.player_type_max_depth_get,
        "depth" : playerc.player_type_depth_get,
        "max_lev" : playerc.player_type_max_lev_get,
        "lev" : playerc.player_type_lev_get,
        "max_exp" : playerc.player_type_max_exp_get,
        "exp" : playerc.player_type_exp_get,
        "exp_frac" : playerc.player_type_exp_frac_get,
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
        "virtues" : playerc.player_type_virtues_get,
        "vir_types" : playerc.player_type_vir_types_get,
        "word_recall" : playerc.player_type_word_recall_get,
        "energy" : playerc.player_type_energy_get,
        "food" : playerc.player_type_food_get,
        "confusing" : playerc.player_type_confusing_get,
        "searching" : playerc.player_type_searching_get,
        "spell_learned1" : playerc.player_type_spell_learned1_get,
        "spell_learned2" : playerc.player_type_spell_learned2_get,
        "spell_worked1" : playerc.player_type_spell_worked1_get,
        "spell_worked2" : playerc.player_type_spell_worked2_get,
        "spell_forgotten1" : playerc.player_type_spell_forgotten1_get,
        "spell_forgotten2" : playerc.player_type_spell_forgotten2_get,
        "spell_order" : playerc.player_type_spell_order_get,
        "player_hp" : playerc.player_type_player_hp_get,
        "died_from" : playerc.player_type_died_from_get,
        "history" : playerc.player_type_history_get,
        "total_winner" : playerc.player_type_total_winner_get,
        "panic_save" : playerc.player_type_panic_save_get,
        "noscore" : playerc.player_type_noscore_get,
        "is_dead" : playerc.player_type_is_dead_get,
        "wizard" : playerc.player_type_wizard_get,
        "playing" : playerc.player_type_playing_get,
        "leaving" : playerc.player_type_leaving_get,
        "leaving_dungeon" : playerc.player_type_leaving_dungeon_get,
        "exit_bldg" : playerc.player_type_exit_bldg_get,
        "leftbldg" : playerc.player_type_leftbldg_get,
        "create_up_stair" : playerc.player_type_create_up_stair_get,
        "create_down_stair" : playerc.player_type_create_down_stair_get,
        "align" : playerc.player_type_align_get,
        "detectx" : playerc.player_type_detectx_get,
        "detecty" : playerc.player_type_detecty_get,
        "detected" : playerc.player_type_detected_get,
        "total_weight" : playerc.player_type_total_weight_get,
        "inven_cnt" : playerc.player_type_inven_cnt_get,
        "equip_cnt" : playerc.player_type_equip_cnt_get,
        "target_set" : playerc.player_type_target_set_get,
        "target_who" : playerc.player_type_target_who_get,
        "target_row" : playerc.player_type_target_row_get,
        "target_col" : playerc.player_type_target_col_get,
        "health_who" : playerc.player_type_health_who_get,
        "monster_race_idx" : playerc.player_type_monster_race_idx_get,
        "object_kind_idx" : playerc.player_type_object_kind_idx_get,
        "energy_use" : playerc.player_type_energy_use_get,
        "resting" : playerc.player_type_resting_get,
        "running" : playerc.player_type_running_get,
        "run_cur_dir" : playerc.player_type_run_cur_dir_get,
        "run_old_dir" : playerc.player_type_run_old_dir_get,
        "run_unused" : playerc.player_type_run_unused_get,
        "run_open_area" : playerc.player_type_run_open_area_get,
        "run_break_right" : playerc.player_type_run_break_right_get,
        "run_break_left" : playerc.player_type_run_break_left_get,
        "command_cmd" : playerc.player_type_command_cmd_get,
        "command_arg" : playerc.player_type_command_arg_get,
        "command_rep" : playerc.player_type_command_rep_get,
        "command_dir" : playerc.player_type_command_dir_get,
        "command_see" : playerc.player_type_command_see_get,
        "command_wrk" : playerc.player_type_command_wrk_get,
        "command_new" : playerc.player_type_command_new_get,
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
        "resist_fear" : playerc.player_type_resist_fear_get,
        "resist_lite" : playerc.player_type_resist_lite_get,
        "resist_dark" : playerc.player_type_resist_dark_get,
        "resist_blind" : playerc.player_type_resist_blind_get,
        "resist_confu" : playerc.player_type_resist_confu_get,
        "resist_sound" : playerc.player_type_resist_sound_get,
        "resist_shard" : playerc.player_type_resist_shard_get,
        "resist_nexus" : playerc.player_type_resist_nexus_get,
        "resist_nethr" : playerc.player_type_resist_nethr_get,
        "resist_chaos" : playerc.player_type_resist_chaos_get,
        "resist_disen" : playerc.player_type_resist_disen_get,
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
        "slow_digest" : playerc.player_type_slow_digest_get,
        "ffall" : playerc.player_type_ffall_get,
        "lite" : playerc.player_type_lite_get,
        "regenerate" : playerc.player_type_regenerate_get,
        "telepathy" : playerc.player_type_telepathy_get,
        "see_inv" : playerc.player_type_see_inv_get,
        "free_act" : playerc.player_type_free_act_get,
        "hold_life" : playerc.player_type_hold_life_get,
        "impact" : playerc.player_type_impact_get,
        "aggravate" : playerc.player_type_aggravate_get,
        "teleport" : playerc.player_type_teleport_get,
        "exp_drain" : playerc.player_type_exp_drain_get,
        "bless_blade" : playerc.player_type_bless_blade_get,
        "xtra_might" : playerc.player_type_xtra_might_get,
        "pass_wall" : playerc.player_type_pass_wall_get,
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
        "noise" : playerc.player_type_noise_get,
        "num_blow" : playerc.player_type_num_blow_get,
        "num_fire" : playerc.player_type_num_fire_get,
        "tval_xtra" : playerc.player_type_tval_xtra_get,
        "ammo_mult" : playerc.player_type_ammo_mult_get,
        "ammo_tval" : playerc.player_type_ammo_tval_get,
        "pspeed" : playerc.player_type_pspeed_get,
        "pet_follow_distance" : playerc.player_type_pet_follow_distance_get,
        "pet_open_doors" : playerc.player_type_pet_open_doors_get,
        "pet_pickup_items" : playerc.player_type_pet_pickup_items_get,
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
        self.__class__ = player_type





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
