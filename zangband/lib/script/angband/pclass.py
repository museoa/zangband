# This file was created automatically by SWIG.
import pclassc
import new
class player_class:
    def __init__(self,*args):
        self.this = apply(pclassc.new_player_class,args)
        self.thisown = 1

    def __del__(self,pclassc=pclassc):
        if self.thisown == 1 :
            pclassc.delete_player_class(self)
    __setmethods__ = {
        "title" : pclassc.player_class_title_set,
        "c_dis" : pclassc.player_class_c_dis_set,
        "c_dev" : pclassc.player_class_c_dev_set,
        "c_sav" : pclassc.player_class_c_sav_set,
        "c_stl" : pclassc.player_class_c_stl_set,
        "c_srh" : pclassc.player_class_c_srh_set,
        "c_fos" : pclassc.player_class_c_fos_set,
        "c_thn" : pclassc.player_class_c_thn_set,
        "c_thb" : pclassc.player_class_c_thb_set,
        "x_dis" : pclassc.player_class_x_dis_set,
        "x_dev" : pclassc.player_class_x_dev_set,
        "x_sav" : pclassc.player_class_x_sav_set,
        "x_stl" : pclassc.player_class_x_stl_set,
        "x_srh" : pclassc.player_class_x_srh_set,
        "x_fos" : pclassc.player_class_x_fos_set,
        "x_thn" : pclassc.player_class_x_thn_set,
        "x_thb" : pclassc.player_class_x_thb_set,
        "c_mhp" : pclassc.player_class_c_mhp_set,
        "c_exp" : pclassc.player_class_c_exp_set,
        "pet_upkeep_div" : pclassc.player_class_pet_upkeep_div_set,
        "str_mod" : pclassc.player_class_str_mod_set,
        "int_mod" : pclassc.player_class_int_mod_set,
        "wis_mod" : pclassc.player_class_wis_mod_set,
        "dex_mod" : pclassc.player_class_dex_mod_set,
        "con_mod" : pclassc.player_class_con_mod_set,
        "chr_mod" : pclassc.player_class_chr_mod_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = player_class.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "title" : pclassc.player_class_title_get,
        "c_adj" : pclassc.player_class_c_adj_get,
        "c_dis" : pclassc.player_class_c_dis_get,
        "c_dev" : pclassc.player_class_c_dev_get,
        "c_sav" : pclassc.player_class_c_sav_get,
        "c_stl" : pclassc.player_class_c_stl_get,
        "c_srh" : pclassc.player_class_c_srh_get,
        "c_fos" : pclassc.player_class_c_fos_get,
        "c_thn" : pclassc.player_class_c_thn_get,
        "c_thb" : pclassc.player_class_c_thb_get,
        "x_dis" : pclassc.player_class_x_dis_get,
        "x_dev" : pclassc.player_class_x_dev_get,
        "x_sav" : pclassc.player_class_x_sav_get,
        "x_stl" : pclassc.player_class_x_stl_get,
        "x_srh" : pclassc.player_class_x_srh_get,
        "x_fos" : pclassc.player_class_x_fos_get,
        "x_thn" : pclassc.player_class_x_thn_get,
        "x_thb" : pclassc.player_class_x_thb_get,
        "c_mhp" : pclassc.player_class_c_mhp_get,
        "c_exp" : pclassc.player_class_c_exp_get,
        "pet_upkeep_div" : pclassc.player_class_pet_upkeep_div_get,
        "str_mod" : pclassc.player_class_str_mod_get,
        "int_mod" : pclassc.player_class_int_mod_get,
        "wis_mod" : pclassc.player_class_wis_mod_get,
        "dex_mod" : pclassc.player_class_dex_mod_get,
        "con_mod" : pclassc.player_class_con_mod_get,
        "chr_mod" : pclassc.player_class_chr_mod_get,
    }
    def __getattr__(self,name):
        method = player_class.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_class instance at %s>" % (self.this,)
class player_classPtr(player_class):
    def __init__(self,this):
        self.this = this
        self.thisown = 0





#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

