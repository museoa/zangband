# This file was created automatically by SWIG.
import playerc
class s16b_array_6Ptr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "data" :
            playerc.s16b_array_6_data_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "data" : 
            return playerc.s16b_array_6_data_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C s16b_array_6 instance at %s>" % (self.this,)
class s16b_array_6(s16b_array_6Ptr):
    def __init__(self,this):
        self.this = this




class player_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def inventory(self, *_args, **_kwargs):
        val = apply(playerc.player_type_inventory,(self,) + _args, _kwargs)
        return val
    def inven_item_increase(self, *_args, **_kwargs):
        val = apply(playerc.player_type_inven_item_increase,(self,) + _args, _kwargs)
        return val
    def inven_item_optimize(self, *_args, **_kwargs):
        val = apply(playerc.player_type_inven_item_optimize,(self,) + _args, _kwargs)
        return val
    def carry(self, *_args, **_kwargs):
        val = apply(playerc.player_type_carry,(self,) + _args, _kwargs)
        return val
    def place(self, *_args, **_kwargs):
        val = apply(playerc.player_type_place,(self,) + _args, _kwargs)
        return val
    def teleport_to(self, *_args, **_kwargs):
        val = apply(playerc.player_type_teleport_to,(self,) + _args, _kwargs)
        return val
    def teleport_level(self, *_args, **_kwargs):
        val = apply(playerc.player_type_teleport_level,(self,) + _args, _kwargs)
        return val
    def recall(self, *_args, **_kwargs):
        val = apply(playerc.player_type_recall,(self,) + _args, _kwargs)
        return val
    def word_of_recall(self, *_args, **_kwargs):
        val = apply(playerc.player_type_word_of_recall,(self,) + _args, _kwargs)
        return val
    def disenchant(self, *_args, **_kwargs):
        val = apply(playerc.player_type_disenchant,(self,) + _args, _kwargs)
        return val
    def mutate_player(self, *_args, **_kwargs):
        val = apply(playerc.player_type_mutate_player,(self,) + _args, _kwargs)
        return val
    def nexus(self, *_args, **_kwargs):
        val = apply(playerc.player_type_nexus,(self,) + _args, _kwargs)
        return val
    def phlogiston(self, *_args, **_kwargs):
        val = apply(playerc.player_type_phlogiston,(self,) + _args, _kwargs)
        return val
    def brand_weapon(self, *_args, **_kwargs):
        val = apply(playerc.player_type_brand_weapon,(self,) + _args, _kwargs)
        return val
    def call_the_void(self, *_args, **_kwargs):
        val = apply(playerc.player_type_call_the_void,(self,) + _args, _kwargs)
        return val
    def fetch(self, *_args, **_kwargs):
        val = apply(playerc.player_type_fetch,(self,) + _args, _kwargs)
        return val
    def alter_reality(self, *_args, **_kwargs):
        val = apply(playerc.player_type_alter_reality,(self,) + _args, _kwargs)
        return val
    def gain_exp(self, *_args, **_kwargs):
        val = apply(playerc.player_type_gain_exp,(self,) + _args, _kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "oldpy" :
            playerc.player_type_oldpy_set(self,value)
            return
        if name == "oldpx" :
            playerc.player_type_oldpx_set(self,value)
            return
        if name == "psex" :
            playerc.player_type_psex_set(self,value)
            return
        if name == "prace" :
            playerc.player_type_prace_set(self,value)
            return
        if name == "pclass" :
            playerc.player_type_pclass_set(self,value)
            return
        if name == "realm1" :
            playerc.player_type_realm1_set(self,value)
            return
        if name == "realm2" :
            playerc.player_type_realm2_set(self,value)
            return
        if name == "oops" :
            playerc.player_type_oops_set(self,value)
            return
        if name == "hitdie" :
            playerc.player_type_hitdie_set(self,value)
            return
        if name == "expfact" :
            playerc.player_type_expfact_set(self,value)
            return
        if name == "age" :
            playerc.player_type_age_set(self,value)
            return
        if name == "ht" :
            playerc.player_type_ht_set(self,value)
            return
        if name == "wt" :
            playerc.player_type_wt_set(self,value)
            return
        if name == "sc" :
            playerc.player_type_sc_set(self,value)
            return
        if name == "au" :
            playerc.player_type_au_set(self,value)
            return
        if name == "max_exp" :
            playerc.player_type_max_exp_set(self,value)
            return
        if name == "exp" :
            playerc.player_type_exp_set(self,value)
            return
        if name == "exp_frac" :
            playerc.player_type_exp_frac_set(self,value)
            return
        if name == "level" :
            playerc.player_type_level_set(self,value)
            return
        if name == "town_num" :
            playerc.player_type_town_num_set(self,value)
            return
        if name == "arena_number" :
            playerc.player_type_arena_number_set(self,value)
            return
        if name == "inside_arena" :
            playerc.player_type_inside_arena_set(self,value)
            return
        if name == "inside_quest" :
            playerc.player_type_inside_quest_set(self,value)
            return
        if name == "rewards" :
            playerc.player_type_rewards_set(self,value)
            return
        if name == "wilderness_x" :
            playerc.player_type_wilderness_x_set(self,value)
            return
        if name == "wilderness_y" :
            playerc.player_type_wilderness_y_set(self,value)
            return
        if name == "mhp" :
            playerc.player_type_mhp_set(self,value)
            return
        if name == "chp" :
            playerc.player_type_chp_set(self,value)
            return
        if name == "chp_frac" :
            playerc.player_type_chp_frac_set(self,value)
            return
        if name == "msp" :
            playerc.player_type_msp_set(self,value)
            return
        if name == "csp" :
            playerc.player_type_csp_set(self,value)
            return
        if name == "csp_frac" :
            playerc.player_type_csp_frac_set(self,value)
            return
        if name == "max_plv" :
            playerc.player_type_max_plv_set(self,value)
            return
        if name == "max_dlv" :
            playerc.player_type_max_dlv_set(self,value)
            return
        if name == "stat_max" :
            playerc.player_type_stat_max_set(self,value)
            return
        if name == "stat_cur" :
            playerc.player_type_stat_cur_set(self,value)
            return
        if name == "fast" :
            playerc.player_type_fast_set(self,value)
            return
        if name == "slow" :
            playerc.player_type_slow_set(self,value)
            return
        if name == "blind" :
            playerc.player_type_blind_set(self,value)
            return
        if name == "paralyzed" :
            playerc.player_type_paralyzed_set(self,value)
            return
        if name == "confused" :
            playerc.player_type_confused_set(self,value)
            return
        if name == "afraid" :
            playerc.player_type_afraid_set(self,value)
            return
        if name == "image" :
            playerc.player_type_image_set(self,value)
            return
        if name == "poisoned" :
            playerc.player_type_poisoned_set(self,value)
            return
        if name == "cut" :
            playerc.player_type_cut_set(self,value)
            return
        if name == "stun" :
            playerc.player_type_stun_set(self,value)
            return
        if name == "protevil" :
            playerc.player_type_protevil_set(self,value)
            return
        if name == "invuln" :
            playerc.player_type_invuln_set(self,value)
            return
        if name == "hero" :
            playerc.player_type_hero_set(self,value)
            return
        if name == "shero" :
            playerc.player_type_shero_set(self,value)
            return
        if name == "shield" :
            playerc.player_type_shield_set(self,value)
            return
        if name == "blessed" :
            playerc.player_type_blessed_set(self,value)
            return
        if name == "tim_invis" :
            playerc.player_type_tim_invis_set(self,value)
            return
        if name == "tim_infra" :
            playerc.player_type_tim_infra_set(self,value)
            return
        if name == "oppose_acid" :
            playerc.player_type_oppose_acid_set(self,value)
            return
        if name == "oppose_elec" :
            playerc.player_type_oppose_elec_set(self,value)
            return
        if name == "oppose_fire" :
            playerc.player_type_oppose_fire_set(self,value)
            return
        if name == "oppose_cold" :
            playerc.player_type_oppose_cold_set(self,value)
            return
        if name == "oppose_pois" :
            playerc.player_type_oppose_pois_set(self,value)
            return
        if name == "tim_esp" :
            playerc.player_type_tim_esp_set(self,value)
            return
        if name == "wraith_form" :
            playerc.player_type_wraith_form_set(self,value)
            return
        if name == "resist_magic" :
            playerc.player_type_resist_magic_set(self,value)
            return
        if name == "tim_xtra1" :
            playerc.player_type_tim_xtra1_set(self,value)
            return
        if name == "tim_xtra2" :
            playerc.player_type_tim_xtra2_set(self,value)
            return
        if name == "tim_xtra3" :
            playerc.player_type_tim_xtra3_set(self,value)
            return
        if name == "tim_xtra4" :
            playerc.player_type_tim_xtra4_set(self,value)
            return
        if name == "tim_xtra5" :
            playerc.player_type_tim_xtra5_set(self,value)
            return
        if name == "tim_xtra6" :
            playerc.player_type_tim_xtra6_set(self,value)
            return
        if name == "tim_xtra7" :
            playerc.player_type_tim_xtra7_set(self,value)
            return
        if name == "tim_xtra8" :
            playerc.player_type_tim_xtra8_set(self,value)
            return
        if name == "chaos_patron" :
            playerc.player_type_chaos_patron_set(self,value)
            return
        if name == "muta1" :
            playerc.player_type_muta1_set(self,value)
            return
        if name == "muta2" :
            playerc.player_type_muta2_set(self,value)
            return
        if name == "muta3" :
            playerc.player_type_muta3_set(self,value)
            return
        if name == "word_recall" :
            playerc.player_type_word_recall_set(self,value)
            return
        if name == "energy" :
            playerc.player_type_energy_set(self,value)
            return
        if name == "food" :
            playerc.player_type_food_set(self,value)
            return
        if name == "confusing" :
            playerc.player_type_confusing_set(self,value)
            return
        if name == "searching" :
            playerc.player_type_searching_set(self,value)
            return
        if name == "new_spells" :
            playerc.player_type_new_spells_set(self,value)
            return
        if name == "old_spells" :
            playerc.player_type_old_spells_set(self,value)
            return
        if name == "old_cumber_armor" :
            playerc.player_type_old_cumber_armor_set(self,value)
            return
        if name == "old_cumber_glove" :
            playerc.player_type_old_cumber_glove_set(self,value)
            return
        if name == "old_heavy_wield" :
            playerc.player_type_old_heavy_wield_set(self,value)
            return
        if name == "old_heavy_shoot" :
            playerc.player_type_old_heavy_shoot_set(self,value)
            return
        if name == "old_icky_wield" :
            playerc.player_type_old_icky_wield_set(self,value)
            return
        if name == "old_lite" :
            playerc.player_type_old_lite_set(self,value)
            return
        if name == "old_view" :
            playerc.player_type_old_view_set(self,value)
            return
        if name == "old_food_aux" :
            playerc.player_type_old_food_aux_set(self,value)
            return
        if name == "cumber_armor" :
            playerc.player_type_cumber_armor_set(self,value)
            return
        if name == "cumber_glove" :
            playerc.player_type_cumber_glove_set(self,value)
            return
        if name == "heavy_wield" :
            playerc.player_type_heavy_wield_set(self,value)
            return
        if name == "heavy_shoot" :
            playerc.player_type_heavy_shoot_set(self,value)
            return
        if name == "icky_wield" :
            playerc.player_type_icky_wield_set(self,value)
            return
        if name == "cur_lite" :
            playerc.player_type_cur_lite_set(self,value)
            return
        if name == "notice" :
            playerc.player_type_notice_set(self,value)
            return
        if name == "update" :
            playerc.player_type_update_set(self,value)
            return
        if name == "redraw" :
            playerc.player_type_redraw_set(self,value)
            return
        if name == "window" :
            playerc.player_type_window_set(self,value)
            return
        if name == "stat_use" :
            playerc.player_type_stat_use_set(self,value)
            return
        if name == "stat_top" :
            playerc.player_type_stat_top_set(self,value)
            return
        if name == "stat_add" :
            playerc.player_type_stat_add_set(self,value)
            return
        if name == "stat_ind" :
            playerc.player_type_stat_ind_set(self,value)
            return
        if name == "immune_acid" :
            playerc.player_type_immune_acid_set(self,value)
            return
        if name == "immune_elec" :
            playerc.player_type_immune_elec_set(self,value)
            return
        if name == "immune_fire" :
            playerc.player_type_immune_fire_set(self,value)
            return
        if name == "immune_cold" :
            playerc.player_type_immune_cold_set(self,value)
            return
        if name == "resist_acid" :
            playerc.player_type_resist_acid_set(self,value)
            return
        if name == "resist_elec" :
            playerc.player_type_resist_elec_set(self,value)
            return
        if name == "resist_fire" :
            playerc.player_type_resist_fire_set(self,value)
            return
        if name == "resist_cold" :
            playerc.player_type_resist_cold_set(self,value)
            return
        if name == "resist_pois" :
            playerc.player_type_resist_pois_set(self,value)
            return
        if name == "resist_conf" :
            playerc.player_type_resist_conf_set(self,value)
            return
        if name == "resist_sound" :
            playerc.player_type_resist_sound_set(self,value)
            return
        if name == "resist_lite" :
            playerc.player_type_resist_lite_set(self,value)
            return
        if name == "resist_dark" :
            playerc.player_type_resist_dark_set(self,value)
            return
        if name == "resist_chaos" :
            playerc.player_type_resist_chaos_set(self,value)
            return
        if name == "resist_disen" :
            playerc.player_type_resist_disen_set(self,value)
            return
        if name == "resist_shard" :
            playerc.player_type_resist_shard_set(self,value)
            return
        if name == "resist_nexus" :
            playerc.player_type_resist_nexus_set(self,value)
            return
        if name == "resist_blind" :
            playerc.player_type_resist_blind_set(self,value)
            return
        if name == "resist_neth" :
            playerc.player_type_resist_neth_set(self,value)
            return
        if name == "resist_fear" :
            playerc.player_type_resist_fear_set(self,value)
            return
        if name == "reflect" :
            playerc.player_type_reflect_set(self,value)
            return
        if name == "sh_fire" :
            playerc.player_type_sh_fire_set(self,value)
            return
        if name == "sh_elec" :
            playerc.player_type_sh_elec_set(self,value)
            return
        if name == "anti_magic" :
            playerc.player_type_anti_magic_set(self,value)
            return
        if name == "anti_tele" :
            playerc.player_type_anti_tele_set(self,value)
            return
        if name == "sustain_str" :
            playerc.player_type_sustain_str_set(self,value)
            return
        if name == "sustain_int" :
            playerc.player_type_sustain_int_set(self,value)
            return
        if name == "sustain_wis" :
            playerc.player_type_sustain_wis_set(self,value)
            return
        if name == "sustain_dex" :
            playerc.player_type_sustain_dex_set(self,value)
            return
        if name == "sustain_con" :
            playerc.player_type_sustain_con_set(self,value)
            return
        if name == "sustain_chr" :
            playerc.player_type_sustain_chr_set(self,value)
            return
        if name == "aggravate" :
            playerc.player_type_aggravate_set(self,value)
            return
        if name == "teleport" :
            playerc.player_type_teleport_set(self,value)
            return
        if name == "exp_drain" :
            playerc.player_type_exp_drain_set(self,value)
            return
        if name == "ffall" :
            playerc.player_type_ffall_set(self,value)
            return
        if name == "lite" :
            playerc.player_type_lite_set(self,value)
            return
        if name == "free_act" :
            playerc.player_type_free_act_set(self,value)
            return
        if name == "see_inv" :
            playerc.player_type_see_inv_set(self,value)
            return
        if name == "regenerate" :
            playerc.player_type_regenerate_set(self,value)
            return
        if name == "hold_life" :
            playerc.player_type_hold_life_set(self,value)
            return
        if name == "telepathy" :
            playerc.player_type_telepathy_set(self,value)
            return
        if name == "slow_digest" :
            playerc.player_type_slow_digest_set(self,value)
            return
        if name == "bless_blade" :
            playerc.player_type_bless_blade_set(self,value)
            return
        if name == "xtra_might" :
            playerc.player_type_xtra_might_set(self,value)
            return
        if name == "impact" :
            playerc.player_type_impact_set(self,value)
            return
        if name == "dis_to_h" :
            playerc.player_type_dis_to_h_set(self,value)
            return
        if name == "dis_to_d" :
            playerc.player_type_dis_to_d_set(self,value)
            return
        if name == "dis_to_a" :
            playerc.player_type_dis_to_a_set(self,value)
            return
        if name == "dis_ac" :
            playerc.player_type_dis_ac_set(self,value)
            return
        if name == "to_h" :
            playerc.player_type_to_h_set(self,value)
            return
        if name == "to_d" :
            playerc.player_type_to_d_set(self,value)
            return
        if name == "to_a" :
            playerc.player_type_to_a_set(self,value)
            return
        if name == "ac" :
            playerc.player_type_ac_set(self,value)
            return
        if name == "see_infra" :
            playerc.player_type_see_infra_set(self,value)
            return
        if name == "skill_dis" :
            playerc.player_type_skill_dis_set(self,value)
            return
        if name == "skill_dev" :
            playerc.player_type_skill_dev_set(self,value)
            return
        if name == "skill_sav" :
            playerc.player_type_skill_sav_set(self,value)
            return
        if name == "skill_stl" :
            playerc.player_type_skill_stl_set(self,value)
            return
        if name == "skill_srh" :
            playerc.player_type_skill_srh_set(self,value)
            return
        if name == "skill_fos" :
            playerc.player_type_skill_fos_set(self,value)
            return
        if name == "skill_thn" :
            playerc.player_type_skill_thn_set(self,value)
            return
        if name == "skill_thb" :
            playerc.player_type_skill_thb_set(self,value)
            return
        if name == "skill_tht" :
            playerc.player_type_skill_tht_set(self,value)
            return
        if name == "skill_dig" :
            playerc.player_type_skill_dig_set(self,value)
            return
        if name == "num_blow" :
            playerc.player_type_num_blow_set(self,value)
            return
        if name == "num_fire" :
            playerc.player_type_num_fire_set(self,value)
            return
        if name == "tval_xtra" :
            playerc.player_type_tval_xtra_set(self,value)
            return
        if name == "tval_ammo" :
            playerc.player_type_tval_ammo_set(self,value)
            return
        if name == "pspeed" :
            playerc.player_type_pspeed_set(self,value)
            return
        if name == "pet_follow_distance" :
            playerc.player_type_pet_follow_distance_set(self,value)
            return
        if name == "pet_open_doors" :
            playerc.player_type_pet_open_doors_set(self,value)
            return
        if name == "pet_pickup_items" :
            playerc.player_type_pet_pickup_items_set(self,value)
            return
        if name == "exit_bldg" :
            playerc.player_type_exit_bldg_set(self,value)
            return
        if name == "leftbldg" :
            playerc.player_type_leftbldg_set(self,value)
            return
        if name == "leaving" :
            playerc.player_type_leaving_set(self,value)
            return
        if name == "name" :
            playerc.player_type_name_set(self,value)
            return
        if name == "dun_level" :
            playerc.player_type_dun_level_set(self,value)
            return
        if name == "py" :
            playerc.player_type_py_set(self,value)
            return
        if name == "px" :
            playerc.player_type_px_set(self,value)
            return
        if name == "player_base" :
            playerc.player_type_player_base_set(self,value)
            return
        if name == "died_from" :
            playerc.player_type_died_from_set(self,value)
            return
        if name == "history" :
            playerc.player_type_history_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "oldpy" : 
            return playerc.player_type_oldpy_get(self)
        if name == "oldpx" : 
            return playerc.player_type_oldpx_get(self)
        if name == "psex" : 
            return playerc.player_type_psex_get(self)
        if name == "prace" : 
            return playerc.player_type_prace_get(self)
        if name == "pclass" : 
            return playerc.player_type_pclass_get(self)
        if name == "realm1" : 
            return playerc.player_type_realm1_get(self)
        if name == "realm2" : 
            return playerc.player_type_realm2_get(self)
        if name == "oops" : 
            return playerc.player_type_oops_get(self)
        if name == "hitdie" : 
            return playerc.player_type_hitdie_get(self)
        if name == "expfact" : 
            return playerc.player_type_expfact_get(self)
        if name == "age" : 
            return playerc.player_type_age_get(self)
        if name == "ht" : 
            return playerc.player_type_ht_get(self)
        if name == "wt" : 
            return playerc.player_type_wt_get(self)
        if name == "sc" : 
            return playerc.player_type_sc_get(self)
        if name == "au" : 
            return playerc.player_type_au_get(self)
        if name == "max_exp" : 
            return playerc.player_type_max_exp_get(self)
        if name == "exp" : 
            return playerc.player_type_exp_get(self)
        if name == "exp_frac" : 
            return playerc.player_type_exp_frac_get(self)
        if name == "level" : 
            return playerc.player_type_level_get(self)
        if name == "town_num" : 
            return playerc.player_type_town_num_get(self)
        if name == "arena_number" : 
            return playerc.player_type_arena_number_get(self)
        if name == "inside_arena" : 
            return playerc.player_type_inside_arena_get(self)
        if name == "inside_quest" : 
            return playerc.player_type_inside_quest_get(self)
        if name == "rewards" : 
            return playerc.player_type_rewards_get(self)
        if name == "wilderness_x" : 
            return playerc.player_type_wilderness_x_get(self)
        if name == "wilderness_y" : 
            return playerc.player_type_wilderness_y_get(self)
        if name == "mhp" : 
            return playerc.player_type_mhp_get(self)
        if name == "chp" : 
            return playerc.player_type_chp_get(self)
        if name == "chp_frac" : 
            return playerc.player_type_chp_frac_get(self)
        if name == "msp" : 
            return playerc.player_type_msp_get(self)
        if name == "csp" : 
            return playerc.player_type_csp_get(self)
        if name == "csp_frac" : 
            return playerc.player_type_csp_frac_get(self)
        if name == "max_plv" : 
            return playerc.player_type_max_plv_get(self)
        if name == "max_dlv" : 
            return playerc.player_type_max_dlv_get(self)
        if name == "stat_max" : 
            return playerc.player_type_stat_max_get(self)
        if name == "stat_cur" : 
            return playerc.player_type_stat_cur_get(self)
        if name == "fast" : 
            return playerc.player_type_fast_get(self)
        if name == "slow" : 
            return playerc.player_type_slow_get(self)
        if name == "blind" : 
            return playerc.player_type_blind_get(self)
        if name == "paralyzed" : 
            return playerc.player_type_paralyzed_get(self)
        if name == "confused" : 
            return playerc.player_type_confused_get(self)
        if name == "afraid" : 
            return playerc.player_type_afraid_get(self)
        if name == "image" : 
            return playerc.player_type_image_get(self)
        if name == "poisoned" : 
            return playerc.player_type_poisoned_get(self)
        if name == "cut" : 
            return playerc.player_type_cut_get(self)
        if name == "stun" : 
            return playerc.player_type_stun_get(self)
        if name == "protevil" : 
            return playerc.player_type_protevil_get(self)
        if name == "invuln" : 
            return playerc.player_type_invuln_get(self)
        if name == "hero" : 
            return playerc.player_type_hero_get(self)
        if name == "shero" : 
            return playerc.player_type_shero_get(self)
        if name == "shield" : 
            return playerc.player_type_shield_get(self)
        if name == "blessed" : 
            return playerc.player_type_blessed_get(self)
        if name == "tim_invis" : 
            return playerc.player_type_tim_invis_get(self)
        if name == "tim_infra" : 
            return playerc.player_type_tim_infra_get(self)
        if name == "oppose_acid" : 
            return playerc.player_type_oppose_acid_get(self)
        if name == "oppose_elec" : 
            return playerc.player_type_oppose_elec_get(self)
        if name == "oppose_fire" : 
            return playerc.player_type_oppose_fire_get(self)
        if name == "oppose_cold" : 
            return playerc.player_type_oppose_cold_get(self)
        if name == "oppose_pois" : 
            return playerc.player_type_oppose_pois_get(self)
        if name == "tim_esp" : 
            return playerc.player_type_tim_esp_get(self)
        if name == "wraith_form" : 
            return playerc.player_type_wraith_form_get(self)
        if name == "resist_magic" : 
            return playerc.player_type_resist_magic_get(self)
        if name == "tim_xtra1" : 
            return playerc.player_type_tim_xtra1_get(self)
        if name == "tim_xtra2" : 
            return playerc.player_type_tim_xtra2_get(self)
        if name == "tim_xtra3" : 
            return playerc.player_type_tim_xtra3_get(self)
        if name == "tim_xtra4" : 
            return playerc.player_type_tim_xtra4_get(self)
        if name == "tim_xtra5" : 
            return playerc.player_type_tim_xtra5_get(self)
        if name == "tim_xtra6" : 
            return playerc.player_type_tim_xtra6_get(self)
        if name == "tim_xtra7" : 
            return playerc.player_type_tim_xtra7_get(self)
        if name == "tim_xtra8" : 
            return playerc.player_type_tim_xtra8_get(self)
        if name == "chaos_patron" : 
            return playerc.player_type_chaos_patron_get(self)
        if name == "muta1" : 
            return playerc.player_type_muta1_get(self)
        if name == "muta2" : 
            return playerc.player_type_muta2_get(self)
        if name == "muta3" : 
            return playerc.player_type_muta3_get(self)
        if name == "word_recall" : 
            return playerc.player_type_word_recall_get(self)
        if name == "energy" : 
            return playerc.player_type_energy_get(self)
        if name == "food" : 
            return playerc.player_type_food_get(self)
        if name == "confusing" : 
            return playerc.player_type_confusing_get(self)
        if name == "searching" : 
            return playerc.player_type_searching_get(self)
        if name == "new_spells" : 
            return playerc.player_type_new_spells_get(self)
        if name == "old_spells" : 
            return playerc.player_type_old_spells_get(self)
        if name == "old_cumber_armor" : 
            return playerc.player_type_old_cumber_armor_get(self)
        if name == "old_cumber_glove" : 
            return playerc.player_type_old_cumber_glove_get(self)
        if name == "old_heavy_wield" : 
            return playerc.player_type_old_heavy_wield_get(self)
        if name == "old_heavy_shoot" : 
            return playerc.player_type_old_heavy_shoot_get(self)
        if name == "old_icky_wield" : 
            return playerc.player_type_old_icky_wield_get(self)
        if name == "old_lite" : 
            return playerc.player_type_old_lite_get(self)
        if name == "old_view" : 
            return playerc.player_type_old_view_get(self)
        if name == "old_food_aux" : 
            return playerc.player_type_old_food_aux_get(self)
        if name == "cumber_armor" : 
            return playerc.player_type_cumber_armor_get(self)
        if name == "cumber_glove" : 
            return playerc.player_type_cumber_glove_get(self)
        if name == "heavy_wield" : 
            return playerc.player_type_heavy_wield_get(self)
        if name == "heavy_shoot" : 
            return playerc.player_type_heavy_shoot_get(self)
        if name == "icky_wield" : 
            return playerc.player_type_icky_wield_get(self)
        if name == "cur_lite" : 
            return playerc.player_type_cur_lite_get(self)
        if name == "notice" : 
            return playerc.player_type_notice_get(self)
        if name == "update" : 
            return playerc.player_type_update_get(self)
        if name == "redraw" : 
            return playerc.player_type_redraw_get(self)
        if name == "window" : 
            return playerc.player_type_window_get(self)
        if name == "stat_use" : 
            return playerc.player_type_stat_use_get(self)
        if name == "stat_top" : 
            return playerc.player_type_stat_top_get(self)
        if name == "stat_add" : 
            return playerc.player_type_stat_add_get(self)
        if name == "stat_ind" : 
            return playerc.player_type_stat_ind_get(self)
        if name == "immune_acid" : 
            return playerc.player_type_immune_acid_get(self)
        if name == "immune_elec" : 
            return playerc.player_type_immune_elec_get(self)
        if name == "immune_fire" : 
            return playerc.player_type_immune_fire_get(self)
        if name == "immune_cold" : 
            return playerc.player_type_immune_cold_get(self)
        if name == "resist_acid" : 
            return playerc.player_type_resist_acid_get(self)
        if name == "resist_elec" : 
            return playerc.player_type_resist_elec_get(self)
        if name == "resist_fire" : 
            return playerc.player_type_resist_fire_get(self)
        if name == "resist_cold" : 
            return playerc.player_type_resist_cold_get(self)
        if name == "resist_pois" : 
            return playerc.player_type_resist_pois_get(self)
        if name == "resist_conf" : 
            return playerc.player_type_resist_conf_get(self)
        if name == "resist_sound" : 
            return playerc.player_type_resist_sound_get(self)
        if name == "resist_lite" : 
            return playerc.player_type_resist_lite_get(self)
        if name == "resist_dark" : 
            return playerc.player_type_resist_dark_get(self)
        if name == "resist_chaos" : 
            return playerc.player_type_resist_chaos_get(self)
        if name == "resist_disen" : 
            return playerc.player_type_resist_disen_get(self)
        if name == "resist_shard" : 
            return playerc.player_type_resist_shard_get(self)
        if name == "resist_nexus" : 
            return playerc.player_type_resist_nexus_get(self)
        if name == "resist_blind" : 
            return playerc.player_type_resist_blind_get(self)
        if name == "resist_neth" : 
            return playerc.player_type_resist_neth_get(self)
        if name == "resist_fear" : 
            return playerc.player_type_resist_fear_get(self)
        if name == "reflect" : 
            return playerc.player_type_reflect_get(self)
        if name == "sh_fire" : 
            return playerc.player_type_sh_fire_get(self)
        if name == "sh_elec" : 
            return playerc.player_type_sh_elec_get(self)
        if name == "anti_magic" : 
            return playerc.player_type_anti_magic_get(self)
        if name == "anti_tele" : 
            return playerc.player_type_anti_tele_get(self)
        if name == "sustain_str" : 
            return playerc.player_type_sustain_str_get(self)
        if name == "sustain_int" : 
            return playerc.player_type_sustain_int_get(self)
        if name == "sustain_wis" : 
            return playerc.player_type_sustain_wis_get(self)
        if name == "sustain_dex" : 
            return playerc.player_type_sustain_dex_get(self)
        if name == "sustain_con" : 
            return playerc.player_type_sustain_con_get(self)
        if name == "sustain_chr" : 
            return playerc.player_type_sustain_chr_get(self)
        if name == "aggravate" : 
            return playerc.player_type_aggravate_get(self)
        if name == "teleport" : 
            return playerc.player_type_teleport_get(self)
        if name == "exp_drain" : 
            return playerc.player_type_exp_drain_get(self)
        if name == "ffall" : 
            return playerc.player_type_ffall_get(self)
        if name == "lite" : 
            return playerc.player_type_lite_get(self)
        if name == "free_act" : 
            return playerc.player_type_free_act_get(self)
        if name == "see_inv" : 
            return playerc.player_type_see_inv_get(self)
        if name == "regenerate" : 
            return playerc.player_type_regenerate_get(self)
        if name == "hold_life" : 
            return playerc.player_type_hold_life_get(self)
        if name == "telepathy" : 
            return playerc.player_type_telepathy_get(self)
        if name == "slow_digest" : 
            return playerc.player_type_slow_digest_get(self)
        if name == "bless_blade" : 
            return playerc.player_type_bless_blade_get(self)
        if name == "xtra_might" : 
            return playerc.player_type_xtra_might_get(self)
        if name == "impact" : 
            return playerc.player_type_impact_get(self)
        if name == "dis_to_h" : 
            return playerc.player_type_dis_to_h_get(self)
        if name == "dis_to_d" : 
            return playerc.player_type_dis_to_d_get(self)
        if name == "dis_to_a" : 
            return playerc.player_type_dis_to_a_get(self)
        if name == "dis_ac" : 
            return playerc.player_type_dis_ac_get(self)
        if name == "to_h" : 
            return playerc.player_type_to_h_get(self)
        if name == "to_d" : 
            return playerc.player_type_to_d_get(self)
        if name == "to_a" : 
            return playerc.player_type_to_a_get(self)
        if name == "ac" : 
            return playerc.player_type_ac_get(self)
        if name == "see_infra" : 
            return playerc.player_type_see_infra_get(self)
        if name == "skill_dis" : 
            return playerc.player_type_skill_dis_get(self)
        if name == "skill_dev" : 
            return playerc.player_type_skill_dev_get(self)
        if name == "skill_sav" : 
            return playerc.player_type_skill_sav_get(self)
        if name == "skill_stl" : 
            return playerc.player_type_skill_stl_get(self)
        if name == "skill_srh" : 
            return playerc.player_type_skill_srh_get(self)
        if name == "skill_fos" : 
            return playerc.player_type_skill_fos_get(self)
        if name == "skill_thn" : 
            return playerc.player_type_skill_thn_get(self)
        if name == "skill_thb" : 
            return playerc.player_type_skill_thb_get(self)
        if name == "skill_tht" : 
            return playerc.player_type_skill_tht_get(self)
        if name == "skill_dig" : 
            return playerc.player_type_skill_dig_get(self)
        if name == "num_blow" : 
            return playerc.player_type_num_blow_get(self)
        if name == "num_fire" : 
            return playerc.player_type_num_fire_get(self)
        if name == "tval_xtra" : 
            return playerc.player_type_tval_xtra_get(self)
        if name == "tval_ammo" : 
            return playerc.player_type_tval_ammo_get(self)
        if name == "pspeed" : 
            return playerc.player_type_pspeed_get(self)
        if name == "pet_follow_distance" : 
            return playerc.player_type_pet_follow_distance_get(self)
        if name == "pet_open_doors" : 
            return playerc.player_type_pet_open_doors_get(self)
        if name == "pet_pickup_items" : 
            return playerc.player_type_pet_pickup_items_get(self)
        if name == "exit_bldg" : 
            return playerc.player_type_exit_bldg_get(self)
        if name == "leftbldg" : 
            return playerc.player_type_leftbldg_get(self)
        if name == "leaving" : 
            return playerc.player_type_leaving_get(self)
        if name == "name" : 
            return playerc.player_type_name_get(self)
        if name == "dun_level" : 
            return playerc.player_type_dun_level_get(self)
        if name == "py" : 
            return playerc.player_type_py_get(self)
        if name == "px" : 
            return playerc.player_type_px_get(self)
        if name == "player_base" : 
            return playerc.player_type_player_base_get(self)
        if name == "died_from" : 
            return playerc.player_type_died_from_get(self)
        if name == "history" : 
            return playerc.player_type_history_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_type instance at %s>" % (self.this,)
class player_type(player_typePtr):
    def __init__(self,this):
        self.this = this






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
