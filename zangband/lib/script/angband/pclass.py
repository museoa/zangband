# This file was created automatically by SWIG.
import pclassc
class player_classPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,pclassc=pclassc):
        if self.thisown == 1 :
            pclassc.delete_player_class(self)
    def __setattr__(self,name,value):
        if name == "title" :
            pclassc.player_class_title_set(self,value)
            return
        if name == "c_adj" :
            pclassc.player_class_c_adj_set(self,value)
            return
        if name == "c_dis" :
            pclassc.player_class_c_dis_set(self,value)
            return
        if name == "c_dev" :
            pclassc.player_class_c_dev_set(self,value)
            return
        if name == "c_sav" :
            pclassc.player_class_c_sav_set(self,value)
            return
        if name == "c_stl" :
            pclassc.player_class_c_stl_set(self,value)
            return
        if name == "c_srh" :
            pclassc.player_class_c_srh_set(self,value)
            return
        if name == "c_fos" :
            pclassc.player_class_c_fos_set(self,value)
            return
        if name == "c_thn" :
            pclassc.player_class_c_thn_set(self,value)
            return
        if name == "c_thb" :
            pclassc.player_class_c_thb_set(self,value)
            return
        if name == "x_dis" :
            pclassc.player_class_x_dis_set(self,value)
            return
        if name == "x_dev" :
            pclassc.player_class_x_dev_set(self,value)
            return
        if name == "x_sav" :
            pclassc.player_class_x_sav_set(self,value)
            return
        if name == "x_stl" :
            pclassc.player_class_x_stl_set(self,value)
            return
        if name == "x_srh" :
            pclassc.player_class_x_srh_set(self,value)
            return
        if name == "x_fos" :
            pclassc.player_class_x_fos_set(self,value)
            return
        if name == "x_thn" :
            pclassc.player_class_x_thn_set(self,value)
            return
        if name == "x_thb" :
            pclassc.player_class_x_thb_set(self,value)
            return
        if name == "c_mhp" :
            pclassc.player_class_c_mhp_set(self,value)
            return
        if name == "c_exp" :
            pclassc.player_class_c_exp_set(self,value)
            return
        if name == "pet_upkeep_div" :
            pclassc.player_class_pet_upkeep_div_set(self,value)
            return
        if name == "str_mod" :
            pclassc.player_class_str_mod_set(self,value)
            return
        if name == "int_mod" :
            pclassc.player_class_int_mod_set(self,value)
            return
        if name == "wis_mod" :
            pclassc.player_class_wis_mod_set(self,value)
            return
        if name == "dex_mod" :
            pclassc.player_class_dex_mod_set(self,value)
            return
        if name == "con_mod" :
            pclassc.player_class_con_mod_set(self,value)
            return
        if name == "chr_mod" :
            pclassc.player_class_chr_mod_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "title" : 
            return pclassc.player_class_title_get(self)
        if name == "c_adj" : 
            return pclassc.player_class_c_adj_get(self)
        if name == "c_dis" : 
            return pclassc.player_class_c_dis_get(self)
        if name == "c_dev" : 
            return pclassc.player_class_c_dev_get(self)
        if name == "c_sav" : 
            return pclassc.player_class_c_sav_get(self)
        if name == "c_stl" : 
            return pclassc.player_class_c_stl_get(self)
        if name == "c_srh" : 
            return pclassc.player_class_c_srh_get(self)
        if name == "c_fos" : 
            return pclassc.player_class_c_fos_get(self)
        if name == "c_thn" : 
            return pclassc.player_class_c_thn_get(self)
        if name == "c_thb" : 
            return pclassc.player_class_c_thb_get(self)
        if name == "x_dis" : 
            return pclassc.player_class_x_dis_get(self)
        if name == "x_dev" : 
            return pclassc.player_class_x_dev_get(self)
        if name == "x_sav" : 
            return pclassc.player_class_x_sav_get(self)
        if name == "x_stl" : 
            return pclassc.player_class_x_stl_get(self)
        if name == "x_srh" : 
            return pclassc.player_class_x_srh_get(self)
        if name == "x_fos" : 
            return pclassc.player_class_x_fos_get(self)
        if name == "x_thn" : 
            return pclassc.player_class_x_thn_get(self)
        if name == "x_thb" : 
            return pclassc.player_class_x_thb_get(self)
        if name == "c_mhp" : 
            return pclassc.player_class_c_mhp_get(self)
        if name == "c_exp" : 
            return pclassc.player_class_c_exp_get(self)
        if name == "pet_upkeep_div" : 
            return pclassc.player_class_pet_upkeep_div_get(self)
        if name == "str_mod" : 
            return pclassc.player_class_str_mod_get(self)
        if name == "int_mod" : 
            return pclassc.player_class_int_mod_get(self)
        if name == "wis_mod" : 
            return pclassc.player_class_wis_mod_get(self)
        if name == "dex_mod" : 
            return pclassc.player_class_dex_mod_get(self)
        if name == "con_mod" : 
            return pclassc.player_class_con_mod_get(self)
        if name == "chr_mod" : 
            return pclassc.player_class_chr_mod_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_class instance at %s>" % (self.this,)
class player_class(player_classPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(pclassc.new_player_class,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

