# This file was created automatically by SWIG.
import realmsc
class player_magicPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,realmsc=realmsc):
        if self.thisown == 1 :
            realmsc.delete_player_magic(self)
    def __setattr__(self,name,value):
        if name == "spell_book" :
            realmsc.player_magic_spell_book_set(self,value)
            return
        if name == "spell_xtra" :
            realmsc.player_magic_spell_xtra_set(self,value)
            return
        if name == "spell_stat" :
            realmsc.player_magic_spell_stat_set(self,value)
            return
        if name == "spell_type" :
            realmsc.player_magic_spell_type_set(self,value)
            return
        if name == "spell_first" :
            realmsc.player_magic_spell_first_set(self,value)
            return
        if name == "spell_weight" :
            realmsc.player_magic_spell_weight_set(self,value)
            return
        if name == "info" :
            realmsc.player_magic_info_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "spell_book" : 
            return realmsc.player_magic_spell_book_get(self)
        if name == "spell_xtra" : 
            return realmsc.player_magic_spell_xtra_get(self)
        if name == "spell_stat" : 
            return realmsc.player_magic_spell_stat_get(self)
        if name == "spell_type" : 
            return realmsc.player_magic_spell_type_get(self)
        if name == "spell_first" : 
            return realmsc.player_magic_spell_first_get(self)
        if name == "spell_weight" : 
            return realmsc.player_magic_spell_weight_get(self)
        if name == "info" : 
            return realmsc.player_magic_info_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C player_magic instance at %s>" % (self.this,)
class player_magic(player_magicPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(realmsc.new_player_magic,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def get_player_magic(*_args, **_kwargs):
    val = apply(realmsc.get_player_magic,_args,_kwargs)
    if val: val = player_magicPtr(val)
    return val



#-------------- VARIABLE WRAPPERS ------------------

cvar = realmsc.cvar
