# This file was created automatically by SWIG.
import cavec
class cave_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "info" :
            cavec.cave_type_info_set(self,value)
            return
        if name == "feat" :
            cavec.cave_type_feat_set(self,value)
            return
        if name == "o_idx" :
            cavec.cave_type_o_idx_set(self,value)
            return
        if name == "m_idx" :
            cavec.cave_type_m_idx_set(self,value)
            return
        if name == "special" :
            cavec.cave_type_special_set(self,value)
            return
        if name == "mimic" :
            cavec.cave_type_mimic_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "info" : 
            return cavec.cave_type_info_get(self)
        if name == "feat" : 
            return cavec.cave_type_feat_get(self)
        if name == "o_idx" : 
            return cavec.cave_type_o_idx_get(self)
        if name == "m_idx" : 
            return cavec.cave_type_m_idx_get(self)
        if name == "special" : 
            return cavec.cave_type_special_get(self)
        if name == "mimic" : 
            return cavec.cave_type_mimic_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C cave_type instance at %s>" % (self.this,)
class cave_type(cave_typePtr):
    def __init__(self,this):
        self.this = this




class vault_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "name" :
            cavec.vault_type_name_set(self,value)
            return
        if name == "text" :
            cavec.vault_type_text_set(self,value)
            return
        if name == "typ" :
            cavec.vault_type_typ_set(self,value)
            return
        if name == "rat" :
            cavec.vault_type_rat_set(self,value)
            return
        if name == "hgt" :
            cavec.vault_type_hgt_set(self,value)
            return
        if name == "wid" :
            cavec.vault_type_wid_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "name" : 
            return cavec.vault_type_name_get(self)
        if name == "text" : 
            return cavec.vault_type_text_get(self)
        if name == "typ" : 
            return cavec.vault_type_typ_get(self)
        if name == "rat" : 
            return cavec.vault_type_rat_get(self)
        if name == "hgt" : 
            return cavec.vault_type_hgt_get(self)
        if name == "wid" : 
            return cavec.vault_type_wid_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C vault_type instance at %s>" % (self.this,)
class vault_type(vault_typePtr):
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------

load_layout = cavec.load_layout

set_feat = cavec.set_feat

def tile(*_args, **_kwargs):
    val = apply(cavec.tile,_args,_kwargs)
    if val: val = cave_typePtr(val)
    return val

generate_wilderness_area = cavec.generate_wilderness_area



#-------------- VARIABLE WRAPPERS ------------------

BLOCK_HGT = cavec.BLOCK_HGT
BLOCK_WID = cavec.BLOCK_WID
PANEL_HGT = cavec.PANEL_HGT
PANEL_WID = cavec.PANEL_WID
SCREEN_HGT = cavec.SCREEN_HGT
SCREEN_WID = cavec.SCREEN_WID
MAX_HGT = cavec.MAX_HGT
MAX_WID = cavec.MAX_WID
cvar = cavec.cvar
