# This file was created automatically by SWIG.
import realmsc
import new
class player_magic:
    def __init__(self,*args):
        self.this = apply(realmsc.new_player_magic,args)
        self.thisown = 1

    def __del__(self,realmsc=realmsc):
        if self.thisown == 1 :
            realmsc.delete_player_magic(self)
    __setmethods__ = {
        "spell_book" : realmsc.player_magic_spell_book_set,
        "spell_xtra" : realmsc.player_magic_spell_xtra_set,
        "spell_stat" : realmsc.player_magic_spell_stat_set,
        "spell_type" : realmsc.player_magic_spell_type_set,
        "spell_first" : realmsc.player_magic_spell_first_set,
        "spell_weight" : realmsc.player_magic_spell_weight_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = player_magic.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "spell_book" : realmsc.player_magic_spell_book_get,
        "spell_xtra" : realmsc.player_magic_spell_xtra_get,
        "spell_stat" : realmsc.player_magic_spell_stat_get,
        "spell_type" : realmsc.player_magic_spell_type_get,
        "spell_first" : realmsc.player_magic_spell_first_get,
        "spell_weight" : realmsc.player_magic_spell_weight_get,
        "info" : realmsc.player_magic_info_get,
    }
    def __getattr__(self,name):
        method = player_magic.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_magic instance at %s>" % (self.this,)
class player_magicPtr(player_magic):
    def __init__(self,this):
        self.this = this
        self.thisown = 0





#-------------- FUNCTION WRAPPERS ------------------

def get_player_magic(*args, **kwargs):
    val = apply(realmsc.get_player_magic,args,kwargs)
    if val: val = player_magicPtr(val)
    return val



#-------------- VARIABLE WRAPPERS ------------------

cvar = realmsc.cvar
