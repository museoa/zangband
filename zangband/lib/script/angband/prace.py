# This file was created automatically by SWIG.
import pracec
class player_racePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,pracec=pracec):
        if self.thisown == 1 :
            pracec.delete_player_race(self)
    def __setattr__(self,name,value):
        if name == "title" :
            pracec.player_race_title_set(self,value)
            return
        if name == "r_adj" :
            pracec.player_race_r_adj_set(self,value)
            return
        if name == "r_dis" :
            pracec.player_race_r_dis_set(self,value)
            return
        if name == "r_dev" :
            pracec.player_race_r_dev_set(self,value)
            return
        if name == "r_sav" :
            pracec.player_race_r_sav_set(self,value)
            return
        if name == "r_stl" :
            pracec.player_race_r_stl_set(self,value)
            return
        if name == "r_srh" :
            pracec.player_race_r_srh_set(self,value)
            return
        if name == "r_fos" :
            pracec.player_race_r_fos_set(self,value)
            return
        if name == "r_thn" :
            pracec.player_race_r_thn_set(self,value)
            return
        if name == "r_thb" :
            pracec.player_race_r_thb_set(self,value)
            return
        if name == "r_mhp" :
            pracec.player_race_r_mhp_set(self,value)
            return
        if name == "r_exp" :
            pracec.player_race_r_exp_set(self,value)
            return
        if name == "b_age" :
            pracec.player_race_b_age_set(self,value)
            return
        if name == "m_age" :
            pracec.player_race_m_age_set(self,value)
            return
        if name == "m_b_ht" :
            pracec.player_race_m_b_ht_set(self,value)
            return
        if name == "m_m_ht" :
            pracec.player_race_m_m_ht_set(self,value)
            return
        if name == "m_b_wt" :
            pracec.player_race_m_b_wt_set(self,value)
            return
        if name == "m_m_wt" :
            pracec.player_race_m_m_wt_set(self,value)
            return
        if name == "f_b_ht" :
            pracec.player_race_f_b_ht_set(self,value)
            return
        if name == "f_m_ht" :
            pracec.player_race_f_m_ht_set(self,value)
            return
        if name == "f_b_wt" :
            pracec.player_race_f_b_wt_set(self,value)
            return
        if name == "f_m_wt" :
            pracec.player_race_f_m_wt_set(self,value)
            return
        if name == "infra" :
            pracec.player_race_infra_set(self,value)
            return
        if name == "choice" :
            pracec.player_race_choice_set(self,value)
            return
        if name == "str_mod" :
            pracec.player_race_str_mod_set(self,value)
            return
        if name == "int_mod" :
            pracec.player_race_int_mod_set(self,value)
            return
        if name == "wis_mod" :
            pracec.player_race_wis_mod_set(self,value)
            return
        if name == "dex_mod" :
            pracec.player_race_dex_mod_set(self,value)
            return
        if name == "con_mod" :
            pracec.player_race_con_mod_set(self,value)
            return
        if name == "chr_mod" :
            pracec.player_race_chr_mod_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "title" : 
            return pracec.player_race_title_get(self)
        if name == "r_adj" : 
            return pracec.player_race_r_adj_get(self)
        if name == "r_dis" : 
            return pracec.player_race_r_dis_get(self)
        if name == "r_dev" : 
            return pracec.player_race_r_dev_get(self)
        if name == "r_sav" : 
            return pracec.player_race_r_sav_get(self)
        if name == "r_stl" : 
            return pracec.player_race_r_stl_get(self)
        if name == "r_srh" : 
            return pracec.player_race_r_srh_get(self)
        if name == "r_fos" : 
            return pracec.player_race_r_fos_get(self)
        if name == "r_thn" : 
            return pracec.player_race_r_thn_get(self)
        if name == "r_thb" : 
            return pracec.player_race_r_thb_get(self)
        if name == "r_mhp" : 
            return pracec.player_race_r_mhp_get(self)
        if name == "r_exp" : 
            return pracec.player_race_r_exp_get(self)
        if name == "b_age" : 
            return pracec.player_race_b_age_get(self)
        if name == "m_age" : 
            return pracec.player_race_m_age_get(self)
        if name == "m_b_ht" : 
            return pracec.player_race_m_b_ht_get(self)
        if name == "m_m_ht" : 
            return pracec.player_race_m_m_ht_get(self)
        if name == "m_b_wt" : 
            return pracec.player_race_m_b_wt_get(self)
        if name == "m_m_wt" : 
            return pracec.player_race_m_m_wt_get(self)
        if name == "f_b_ht" : 
            return pracec.player_race_f_b_ht_get(self)
        if name == "f_m_ht" : 
            return pracec.player_race_f_m_ht_get(self)
        if name == "f_b_wt" : 
            return pracec.player_race_f_b_wt_get(self)
        if name == "f_m_wt" : 
            return pracec.player_race_f_m_wt_get(self)
        if name == "infra" : 
            return pracec.player_race_infra_get(self)
        if name == "choice" : 
            return pracec.player_race_choice_get(self)
        if name == "str_mod" : 
            return pracec.player_race_str_mod_get(self)
        if name == "int_mod" : 
            return pracec.player_race_int_mod_get(self)
        if name == "wis_mod" : 
            return pracec.player_race_wis_mod_get(self)
        if name == "dex_mod" : 
            return pracec.player_race_dex_mod_get(self)
        if name == "con_mod" : 
            return pracec.player_race_con_mod_get(self)
        if name == "chr_mod" : 
            return pracec.player_race_chr_mod_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_race instance at %s>" % (self.this,)
class player_race(player_racePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(pracec.new_player_race,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

