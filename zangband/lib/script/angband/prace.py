# This file was created automatically by SWIG.
import pracec
class player_race:
    def __init__(self,*args):
        self.this = apply(pracec.new_player_race,args)
        self.thisown = 1

    def __del__(self,pracec=pracec):
        if self.thisown == 1 :
            pracec.delete_player_race(self)
    def get_r_adj(*args):
        val = apply(pracec.player_race_get_r_adj,args)
        return val
    def set_r_adj(*args):
        val = apply(pracec.player_race_set_r_adj,args)
        return val
    __setmethods__ = {
        "title" : pracec.player_race_title_set,
        "r_dis" : pracec.player_race_r_dis_set,
        "r_dev" : pracec.player_race_r_dev_set,
        "r_sav" : pracec.player_race_r_sav_set,
        "r_stl" : pracec.player_race_r_stl_set,
        "r_srh" : pracec.player_race_r_srh_set,
        "r_fos" : pracec.player_race_r_fos_set,
        "r_thn" : pracec.player_race_r_thn_set,
        "r_thb" : pracec.player_race_r_thb_set,
        "r_mhp" : pracec.player_race_r_mhp_set,
        "r_exp" : pracec.player_race_r_exp_set,
        "b_age" : pracec.player_race_b_age_set,
        "m_age" : pracec.player_race_m_age_set,
        "m_b_ht" : pracec.player_race_m_b_ht_set,
        "m_m_ht" : pracec.player_race_m_m_ht_set,
        "m_b_wt" : pracec.player_race_m_b_wt_set,
        "m_m_wt" : pracec.player_race_m_m_wt_set,
        "f_b_ht" : pracec.player_race_f_b_ht_set,
        "f_m_ht" : pracec.player_race_f_m_ht_set,
        "f_b_wt" : pracec.player_race_f_b_wt_set,
        "f_m_wt" : pracec.player_race_f_m_wt_set,
        "infra" : pracec.player_race_infra_set,
        "choice" : pracec.player_race_choice_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = player_race.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "title" : pracec.player_race_title_get,
        "r_adj" : pracec.player_race_r_adj_get,
        "r_dis" : pracec.player_race_r_dis_get,
        "r_dev" : pracec.player_race_r_dev_get,
        "r_sav" : pracec.player_race_r_sav_get,
        "r_stl" : pracec.player_race_r_stl_get,
        "r_srh" : pracec.player_race_r_srh_get,
        "r_fos" : pracec.player_race_r_fos_get,
        "r_thn" : pracec.player_race_r_thn_get,
        "r_thb" : pracec.player_race_r_thb_get,
        "r_mhp" : pracec.player_race_r_mhp_get,
        "r_exp" : pracec.player_race_r_exp_get,
        "b_age" : pracec.player_race_b_age_get,
        "m_age" : pracec.player_race_m_age_get,
        "m_b_ht" : pracec.player_race_m_b_ht_get,
        "m_m_ht" : pracec.player_race_m_m_ht_get,
        "m_b_wt" : pracec.player_race_m_b_wt_get,
        "m_m_wt" : pracec.player_race_m_m_wt_get,
        "f_b_ht" : pracec.player_race_f_b_ht_get,
        "f_m_ht" : pracec.player_race_f_m_ht_get,
        "f_b_wt" : pracec.player_race_f_b_wt_get,
        "f_m_wt" : pracec.player_race_f_m_wt_get,
        "infra" : pracec.player_race_infra_get,
        "choice" : pracec.player_race_choice_get,
    }
    def __getattr__(self,name):
        method = player_race.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_race instance at %s>" % (self.this,)
class player_racePtr(player_race):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = player_race



class player_sex:
    def __init__(self,*args):
        self.this = apply(pracec.new_player_sex,args)
        self.thisown = 1

    def __del__(self,pracec=pracec):
        if self.thisown == 1 :
            pracec.delete_player_sex(self)
    __setmethods__ = {
        "title" : pracec.player_sex_title_set,
        "winner" : pracec.player_sex_winner_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = player_sex.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "title" : pracec.player_sex_title_get,
        "winner" : pracec.player_sex_winner_get,
    }
    def __getattr__(self,name):
        method = player_sex.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_sex instance at %s>" % (self.this,)
class player_sexPtr(player_sex):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = player_sex





#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

