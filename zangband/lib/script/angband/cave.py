# This file was created automatically by SWIG.
import cavec
import new
class cave_type:
    def __init__(self,this):
        self.this = this

    __setmethods__ = {
        "info" : cavec.cave_type_info_set,
        "feat" : cavec.cave_type_feat_set,
        "o_idx" : cavec.cave_type_o_idx_set,
        "m_idx" : cavec.cave_type_m_idx_set,
        "special" : cavec.cave_type_special_set,
        "mimic" : cavec.cave_type_mimic_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = cave_type.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "info" : cavec.cave_type_info_get,
        "feat" : cavec.cave_type_feat_get,
        "o_idx" : cavec.cave_type_o_idx_get,
        "m_idx" : cavec.cave_type_m_idx_get,
        "special" : cavec.cave_type_special_get,
        "mimic" : cavec.cave_type_mimic_get,
    }
    def __getattr__(self,name):
        method = cave_type.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C cave_type instance at %s>" % (self.this,)
class cave_typePtr(cave_type):
    def __init__(self,this):
        self.this = this
        self.thisown = 0



class vault_type:
    def __init__(self,this):
        self.this = this

    __setmethods__ = {
        "name" : cavec.vault_type_name_set,
        "text" : cavec.vault_type_text_set,
        "typ" : cavec.vault_type_typ_set,
        "rat" : cavec.vault_type_rat_set,
        "hgt" : cavec.vault_type_hgt_set,
        "wid" : cavec.vault_type_wid_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = vault_type.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "name" : cavec.vault_type_name_get,
        "text" : cavec.vault_type_text_get,
        "typ" : cavec.vault_type_typ_get,
        "rat" : cavec.vault_type_rat_get,
        "hgt" : cavec.vault_type_hgt_get,
        "wid" : cavec.vault_type_wid_get,
    }
    def __getattr__(self,name):
        method = vault_type.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C vault_type instance at %s>" % (self.this,)
class vault_typePtr(vault_type):
    def __init__(self,this):
        self.this = this
        self.thisown = 0





#-------------- FUNCTION WRAPPERS ------------------

load_layout = cavec.load_layout

set_feat = cavec.set_feat

def tile(*args, **kwargs):
    val = apply(cavec.tile,args,kwargs)
    if val: val = cave_typePtr(val)
    return val



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
