# This file was created automatically by SWIG.
import monsterc
class monster_type:
    def __init__(self,*args):
        self.this = apply(monsterc.new_monster_type,args)
        self.thisown = 1

    def __del__(self,monsterc=monsterc):
        if self.thisown == 1 :
            monsterc.delete_monster_type(self)
    __setmethods__ = {
        "r_idx" : monsterc.monster_type_r_idx_set,
        "fy" : monsterc.monster_type_fy_set,
        "fx" : monsterc.monster_type_fx_set,
        "hp" : monsterc.monster_type_hp_set,
        "maxhp" : monsterc.monster_type_maxhp_set,
        "csleep" : monsterc.monster_type_csleep_set,
        "mspeed" : monsterc.monster_type_mspeed_set,
        "energy" : monsterc.monster_type_energy_set,
        "stunned" : monsterc.monster_type_stunned_set,
        "confused" : monsterc.monster_type_confused_set,
        "monfear" : monsterc.monster_type_monfear_set,
        "cdis" : monsterc.monster_type_cdis_set,
        "mflag" : monsterc.monster_type_mflag_set,
        "ml" : monsterc.monster_type_ml_set,
        "hold_o_idx" : monsterc.monster_type_hold_o_idx_set,
        "smart" : monsterc.monster_type_smart_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = monster_type.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "r_idx" : monsterc.monster_type_r_idx_get,
        "fy" : monsterc.monster_type_fy_get,
        "fx" : monsterc.monster_type_fx_get,
        "hp" : monsterc.monster_type_hp_get,
        "maxhp" : monsterc.monster_type_maxhp_get,
        "csleep" : monsterc.monster_type_csleep_get,
        "mspeed" : monsterc.monster_type_mspeed_get,
        "energy" : monsterc.monster_type_energy_get,
        "stunned" : monsterc.monster_type_stunned_get,
        "confused" : monsterc.monster_type_confused_get,
        "monfear" : monsterc.monster_type_monfear_get,
        "cdis" : monsterc.monster_type_cdis_get,
        "mflag" : monsterc.monster_type_mflag_get,
        "ml" : monsterc.monster_type_ml_get,
        "hold_o_idx" : monsterc.monster_type_hold_o_idx_get,
        "smart" : monsterc.monster_type_smart_get,
    }
    def __getattr__(self,name):
        method = monster_type.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C monster_type instance at %s>" % (self.this,)
class monster_typePtr(monster_type):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = monster_type



class monster_blow:
    def __init__(self,this):
        self.this = this

    __setmethods__ = {
        "method" : monsterc.monster_blow_method_set,
        "effect" : monsterc.monster_blow_effect_set,
        "d_dice" : monsterc.monster_blow_d_dice_set,
        "d_side" : monsterc.monster_blow_d_side_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = monster_blow.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "method" : monsterc.monster_blow_method_get,
        "effect" : monsterc.monster_blow_effect_get,
        "d_dice" : monsterc.monster_blow_d_dice_get,
        "d_side" : monsterc.monster_blow_d_side_get,
    }
    def __getattr__(self,name):
        method = monster_blow.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C monster_blow instance at %s>" % (self.this,)
class monster_blowPtr(monster_blow):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = monster_blow



class monster_race:
    def __init__(self,this):
        self.this = this

    __setmethods__ = {
        "name" : monsterc.monster_race_name_set,
        "text" : monsterc.monster_race_text_set,
        "hdice" : monsterc.monster_race_hdice_set,
        "hside" : monsterc.monster_race_hside_set,
        "ac" : monsterc.monster_race_ac_set,
        "sleep" : monsterc.monster_race_sleep_set,
        "aaf" : monsterc.monster_race_aaf_set,
        "speed" : monsterc.monster_race_speed_set,
        "mexp" : monsterc.monster_race_mexp_set,
        "extra" : monsterc.monster_race_extra_set,
        "freq_inate" : monsterc.monster_race_freq_inate_set,
        "freq_spell" : monsterc.monster_race_freq_spell_set,
        "flags1" : monsterc.monster_race_flags1_set,
        "flags2" : monsterc.monster_race_flags2_set,
        "flags3" : monsterc.monster_race_flags3_set,
        "flags4" : monsterc.monster_race_flags4_set,
        "flags5" : monsterc.monster_race_flags5_set,
        "flags6" : monsterc.monster_race_flags6_set,
        "flags7" : monsterc.monster_race_flags7_set,
        "flags8" : monsterc.monster_race_flags8_set,
        "flags9" : monsterc.monster_race_flags9_set,
        "level" : monsterc.monster_race_level_set,
        "rarity" : monsterc.monster_race_rarity_set,
        "d_attr" : monsterc.monster_race_d_attr_set,
        "d_char" : monsterc.monster_race_d_char_set,
        "x_attr" : monsterc.monster_race_x_attr_set,
        "x_char" : monsterc.monster_race_x_char_set,
        "max_num" : monsterc.monster_race_max_num_set,
        "cur_num" : monsterc.monster_race_cur_num_set,
        "r_sights" : monsterc.monster_race_r_sights_set,
        "r_deaths" : monsterc.monster_race_r_deaths_set,
        "r_pkills" : monsterc.monster_race_r_pkills_set,
        "r_tkills" : monsterc.monster_race_r_tkills_set,
        "r_wake" : monsterc.monster_race_r_wake_set,
        "r_ignore" : monsterc.monster_race_r_ignore_set,
        "r_xtra1" : monsterc.monster_race_r_xtra1_set,
        "r_xtra2" : monsterc.monster_race_r_xtra2_set,
        "r_drop_gold" : monsterc.monster_race_r_drop_gold_set,
        "r_drop_item" : monsterc.monster_race_r_drop_item_set,
        "r_cast_inate" : monsterc.monster_race_r_cast_inate_set,
        "r_cast_spell" : monsterc.monster_race_r_cast_spell_set,
        "r_flags1" : monsterc.monster_race_r_flags1_set,
        "r_flags2" : monsterc.monster_race_r_flags2_set,
        "r_flags3" : monsterc.monster_race_r_flags3_set,
        "r_flags4" : monsterc.monster_race_r_flags4_set,
        "r_flags5" : monsterc.monster_race_r_flags5_set,
        "r_flags6" : monsterc.monster_race_r_flags6_set,
        "r_flags7" : monsterc.monster_race_r_flags7_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = monster_race.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "name" : monsterc.monster_race_name_get,
        "text" : monsterc.monster_race_text_get,
        "hdice" : monsterc.monster_race_hdice_get,
        "hside" : monsterc.monster_race_hside_get,
        "ac" : monsterc.monster_race_ac_get,
        "sleep" : monsterc.monster_race_sleep_get,
        "aaf" : monsterc.monster_race_aaf_get,
        "speed" : monsterc.monster_race_speed_get,
        "mexp" : monsterc.monster_race_mexp_get,
        "extra" : monsterc.monster_race_extra_get,
        "freq_inate" : monsterc.monster_race_freq_inate_get,
        "freq_spell" : monsterc.monster_race_freq_spell_get,
        "flags1" : monsterc.monster_race_flags1_get,
        "flags2" : monsterc.monster_race_flags2_get,
        "flags3" : monsterc.monster_race_flags3_get,
        "flags4" : monsterc.monster_race_flags4_get,
        "flags5" : monsterc.monster_race_flags5_get,
        "flags6" : monsterc.monster_race_flags6_get,
        "flags7" : monsterc.monster_race_flags7_get,
        "flags8" : monsterc.monster_race_flags8_get,
        "flags9" : monsterc.monster_race_flags9_get,
        "blow" : lambda x : monster_blowPtr(monsterc.monster_race_blow_get(x)),
        "level" : monsterc.monster_race_level_get,
        "rarity" : monsterc.monster_race_rarity_get,
        "d_attr" : monsterc.monster_race_d_attr_get,
        "d_char" : monsterc.monster_race_d_char_get,
        "x_attr" : monsterc.monster_race_x_attr_get,
        "x_char" : monsterc.monster_race_x_char_get,
        "max_num" : monsterc.monster_race_max_num_get,
        "cur_num" : monsterc.monster_race_cur_num_get,
        "r_sights" : monsterc.monster_race_r_sights_get,
        "r_deaths" : monsterc.monster_race_r_deaths_get,
        "r_pkills" : monsterc.monster_race_r_pkills_get,
        "r_tkills" : monsterc.monster_race_r_tkills_get,
        "r_wake" : monsterc.monster_race_r_wake_get,
        "r_ignore" : monsterc.monster_race_r_ignore_get,
        "r_xtra1" : monsterc.monster_race_r_xtra1_get,
        "r_xtra2" : monsterc.monster_race_r_xtra2_get,
        "r_drop_gold" : monsterc.monster_race_r_drop_gold_get,
        "r_drop_item" : monsterc.monster_race_r_drop_item_get,
        "r_cast_inate" : monsterc.monster_race_r_cast_inate_get,
        "r_cast_spell" : monsterc.monster_race_r_cast_spell_get,
        "r_blows" : monsterc.monster_race_r_blows_get,
        "r_flags1" : monsterc.monster_race_r_flags1_get,
        "r_flags2" : monsterc.monster_race_r_flags2_get,
        "r_flags3" : monsterc.monster_race_r_flags3_get,
        "r_flags4" : monsterc.monster_race_r_flags4_get,
        "r_flags5" : monsterc.monster_race_r_flags5_get,
        "r_flags6" : monsterc.monster_race_r_flags6_get,
        "r_flags7" : monsterc.monster_race_r_flags7_get,
    }
    def __getattr__(self,name):
        method = monster_race.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C monster_race instance at %s>" % (self.this,)
class monster_racePtr(monster_race):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = monster_race





#-------------- FUNCTION WRAPPERS ------------------

def get_monster(*args, **kwargs):
    val = apply(monsterc.get_monster,args,kwargs)
    if val: val = monster_typePtr(val)
    return val

sanity_blast = monsterc.sanity_blast

delete_monster_idx = monsterc.delete_monster_idx

delete_monster = monsterc.delete_monster

compact_monsters = monsterc.compact_monsters

wipe_m_list = monsterc.wipe_m_list

m_pop = monsterc.m_pop

get_mon_num_prep = monsterc.get_mon_num_prep

get_mon_num = monsterc.get_mon_num

monster_desc = monsterc.monster_desc

lore_do_probe = monsterc.lore_do_probe

lore_treasure = monsterc.lore_treasure

update_mon = monsterc.update_mon

update_monsters = monsterc.update_monsters

place_monster_aux = monsterc.place_monster_aux

place_monster = monsterc.place_monster

alloc_horde = monsterc.alloc_horde

alloc_monster = monsterc.alloc_monster

summon_specific = monsterc.summon_specific

multiply_monster = monsterc.multiply_monster

update_smart_learn = monsterc.update_smart_learn

place_monster_one = monsterc.place_monster_one

player_place = monsterc.player_place

monster_drop_carried_objects = monsterc.monster_drop_carried_objects

monster_dungeon = monsterc.monster_dungeon

monster_quest = monsterc.monster_quest

monster_ocean = monsterc.monster_ocean

monster_shore = monsterc.monster_shore

monster_town = monsterc.monster_town

monster_grass = monsterc.monster_grass

get_monster_hook = monsterc.get_monster_hook

get_monster_hook2 = monsterc.get_monster_hook2

set_friendly = monsterc.set_friendly

set_pet = monsterc.set_pet

set_hostile = monsterc.set_hostile

monster_can_cross_terrain = monsterc.monster_can_cross_terrain

are_enemies = monsterc.are_enemies

monster_living = monsterc.monster_living



#-------------- VARIABLE WRAPPERS ------------------

