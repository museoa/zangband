# This file was created automatically by SWIG.
import monsterc
class monster_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,monsterc=monsterc):
        if self.thisown == 1 :
            monsterc.delete_monster_type(self)
    def __setattr__(self,name,value):
        if name == "r_idx" :
            monsterc.monster_type_r_idx_set(self,value)
            return
        if name == "fy" :
            monsterc.monster_type_fy_set(self,value)
            return
        if name == "fx" :
            monsterc.monster_type_fx_set(self,value)
            return
        if name == "hp" :
            monsterc.monster_type_hp_set(self,value)
            return
        if name == "maxhp" :
            monsterc.monster_type_maxhp_set(self,value)
            return
        if name == "csleep" :
            monsterc.monster_type_csleep_set(self,value)
            return
        if name == "mspeed" :
            monsterc.monster_type_mspeed_set(self,value)
            return
        if name == "energy" :
            monsterc.monster_type_energy_set(self,value)
            return
        if name == "stunned" :
            monsterc.monster_type_stunned_set(self,value)
            return
        if name == "confused" :
            monsterc.monster_type_confused_set(self,value)
            return
        if name == "monfear" :
            monsterc.monster_type_monfear_set(self,value)
            return
        if name == "cdis" :
            monsterc.monster_type_cdis_set(self,value)
            return
        if name == "mflag" :
            monsterc.monster_type_mflag_set(self,value)
            return
        if name == "ml" :
            monsterc.monster_type_ml_set(self,value)
            return
        if name == "hold_o_idx" :
            monsterc.monster_type_hold_o_idx_set(self,value)
            return
        if name == "smart" :
            monsterc.monster_type_smart_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "r_idx" : 
            return monsterc.monster_type_r_idx_get(self)
        if name == "fy" : 
            return monsterc.monster_type_fy_get(self)
        if name == "fx" : 
            return monsterc.monster_type_fx_get(self)
        if name == "hp" : 
            return monsterc.monster_type_hp_get(self)
        if name == "maxhp" : 
            return monsterc.monster_type_maxhp_get(self)
        if name == "csleep" : 
            return monsterc.monster_type_csleep_get(self)
        if name == "mspeed" : 
            return monsterc.monster_type_mspeed_get(self)
        if name == "energy" : 
            return monsterc.monster_type_energy_get(self)
        if name == "stunned" : 
            return monsterc.monster_type_stunned_get(self)
        if name == "confused" : 
            return monsterc.monster_type_confused_get(self)
        if name == "monfear" : 
            return monsterc.monster_type_monfear_get(self)
        if name == "cdis" : 
            return monsterc.monster_type_cdis_get(self)
        if name == "mflag" : 
            return monsterc.monster_type_mflag_get(self)
        if name == "ml" : 
            return monsterc.monster_type_ml_get(self)
        if name == "hold_o_idx" : 
            return monsterc.monster_type_hold_o_idx_get(self)
        if name == "smart" : 
            return monsterc.monster_type_smart_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C monster_type instance at %s>" % (self.this,)
class monster_type(monster_typePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(monsterc.new_monster_type,_args,_kwargs)
        self.thisown = 1




class monster_blowPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "method" :
            monsterc.monster_blow_method_set(self,value)
            return
        if name == "effect" :
            monsterc.monster_blow_effect_set(self,value)
            return
        if name == "d_dice" :
            monsterc.monster_blow_d_dice_set(self,value)
            return
        if name == "d_side" :
            monsterc.monster_blow_d_side_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "method" : 
            return monsterc.monster_blow_method_get(self)
        if name == "effect" : 
            return monsterc.monster_blow_effect_get(self)
        if name == "d_dice" : 
            return monsterc.monster_blow_d_dice_get(self)
        if name == "d_side" : 
            return monsterc.monster_blow_d_side_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C monster_blow instance at %s>" % (self.this,)
class monster_blow(monster_blowPtr):
    def __init__(self,this):
        self.this = this




class monster_racePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "name" :
            monsterc.monster_race_name_set(self,value)
            return
        if name == "text" :
            monsterc.monster_race_text_set(self,value)
            return
        if name == "hdice" :
            monsterc.monster_race_hdice_set(self,value)
            return
        if name == "hside" :
            monsterc.monster_race_hside_set(self,value)
            return
        if name == "ac" :
            monsterc.monster_race_ac_set(self,value)
            return
        if name == "sleep" :
            monsterc.monster_race_sleep_set(self,value)
            return
        if name == "aaf" :
            monsterc.monster_race_aaf_set(self,value)
            return
        if name == "speed" :
            monsterc.monster_race_speed_set(self,value)
            return
        if name == "mexp" :
            monsterc.monster_race_mexp_set(self,value)
            return
        if name == "extra" :
            monsterc.monster_race_extra_set(self,value)
            return
        if name == "freq_inate" :
            monsterc.monster_race_freq_inate_set(self,value)
            return
        if name == "freq_spell" :
            monsterc.monster_race_freq_spell_set(self,value)
            return
        if name == "flags1" :
            monsterc.monster_race_flags1_set(self,value)
            return
        if name == "flags2" :
            monsterc.monster_race_flags2_set(self,value)
            return
        if name == "flags3" :
            monsterc.monster_race_flags3_set(self,value)
            return
        if name == "flags4" :
            monsterc.monster_race_flags4_set(self,value)
            return
        if name == "flags5" :
            monsterc.monster_race_flags5_set(self,value)
            return
        if name == "flags6" :
            monsterc.monster_race_flags6_set(self,value)
            return
        if name == "flags7" :
            monsterc.monster_race_flags7_set(self,value)
            return
        if name == "flags8" :
            monsterc.monster_race_flags8_set(self,value)
            return
        if name == "flags9" :
            monsterc.monster_race_flags9_set(self,value)
            return
        if name == "blow" :
            monsterc.monster_race_blow_set(self,value.this)
            return
        if name == "level" :
            monsterc.monster_race_level_set(self,value)
            return
        if name == "rarity" :
            monsterc.monster_race_rarity_set(self,value)
            return
        if name == "d_attr" :
            monsterc.monster_race_d_attr_set(self,value)
            return
        if name == "d_char" :
            monsterc.monster_race_d_char_set(self,value)
            return
        if name == "x_attr" :
            monsterc.monster_race_x_attr_set(self,value)
            return
        if name == "x_char" :
            monsterc.monster_race_x_char_set(self,value)
            return
        if name == "max_num" :
            monsterc.monster_race_max_num_set(self,value)
            return
        if name == "cur_num" :
            monsterc.monster_race_cur_num_set(self,value)
            return
        if name == "r_sights" :
            monsterc.monster_race_r_sights_set(self,value)
            return
        if name == "r_deaths" :
            monsterc.monster_race_r_deaths_set(self,value)
            return
        if name == "r_pkills" :
            monsterc.monster_race_r_pkills_set(self,value)
            return
        if name == "r_tkills" :
            monsterc.monster_race_r_tkills_set(self,value)
            return
        if name == "r_wake" :
            monsterc.monster_race_r_wake_set(self,value)
            return
        if name == "r_ignore" :
            monsterc.monster_race_r_ignore_set(self,value)
            return
        if name == "r_xtra1" :
            monsterc.monster_race_r_xtra1_set(self,value)
            return
        if name == "r_xtra2" :
            monsterc.monster_race_r_xtra2_set(self,value)
            return
        if name == "r_drop_gold" :
            monsterc.monster_race_r_drop_gold_set(self,value)
            return
        if name == "r_drop_item" :
            monsterc.monster_race_r_drop_item_set(self,value)
            return
        if name == "r_cast_inate" :
            monsterc.monster_race_r_cast_inate_set(self,value)
            return
        if name == "r_cast_spell" :
            monsterc.monster_race_r_cast_spell_set(self,value)
            return
        if name == "r_blows" :
            monsterc.monster_race_r_blows_set(self,value)
            return
        if name == "r_flags1" :
            monsterc.monster_race_r_flags1_set(self,value)
            return
        if name == "r_flags2" :
            monsterc.monster_race_r_flags2_set(self,value)
            return
        if name == "r_flags3" :
            monsterc.monster_race_r_flags3_set(self,value)
            return
        if name == "r_flags4" :
            monsterc.monster_race_r_flags4_set(self,value)
            return
        if name == "r_flags5" :
            monsterc.monster_race_r_flags5_set(self,value)
            return
        if name == "r_flags6" :
            monsterc.monster_race_r_flags6_set(self,value)
            return
        if name == "r_flags7" :
            monsterc.monster_race_r_flags7_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "name" : 
            return monsterc.monster_race_name_get(self)
        if name == "text" : 
            return monsterc.monster_race_text_get(self)
        if name == "hdice" : 
            return monsterc.monster_race_hdice_get(self)
        if name == "hside" : 
            return monsterc.monster_race_hside_get(self)
        if name == "ac" : 
            return monsterc.monster_race_ac_get(self)
        if name == "sleep" : 
            return monsterc.monster_race_sleep_get(self)
        if name == "aaf" : 
            return monsterc.monster_race_aaf_get(self)
        if name == "speed" : 
            return monsterc.monster_race_speed_get(self)
        if name == "mexp" : 
            return monsterc.monster_race_mexp_get(self)
        if name == "extra" : 
            return monsterc.monster_race_extra_get(self)
        if name == "freq_inate" : 
            return monsterc.monster_race_freq_inate_get(self)
        if name == "freq_spell" : 
            return monsterc.monster_race_freq_spell_get(self)
        if name == "flags1" : 
            return monsterc.monster_race_flags1_get(self)
        if name == "flags2" : 
            return monsterc.monster_race_flags2_get(self)
        if name == "flags3" : 
            return monsterc.monster_race_flags3_get(self)
        if name == "flags4" : 
            return monsterc.monster_race_flags4_get(self)
        if name == "flags5" : 
            return monsterc.monster_race_flags5_get(self)
        if name == "flags6" : 
            return monsterc.monster_race_flags6_get(self)
        if name == "flags7" : 
            return monsterc.monster_race_flags7_get(self)
        if name == "flags8" : 
            return monsterc.monster_race_flags8_get(self)
        if name == "flags9" : 
            return monsterc.monster_race_flags9_get(self)
        if name == "blow" : 
            return monster_blowPtr(monsterc.monster_race_blow_get(self))
        if name == "level" : 
            return monsterc.monster_race_level_get(self)
        if name == "rarity" : 
            return monsterc.monster_race_rarity_get(self)
        if name == "d_attr" : 
            return monsterc.monster_race_d_attr_get(self)
        if name == "d_char" : 
            return monsterc.monster_race_d_char_get(self)
        if name == "x_attr" : 
            return monsterc.monster_race_x_attr_get(self)
        if name == "x_char" : 
            return monsterc.monster_race_x_char_get(self)
        if name == "max_num" : 
            return monsterc.monster_race_max_num_get(self)
        if name == "cur_num" : 
            return monsterc.monster_race_cur_num_get(self)
        if name == "r_sights" : 
            return monsterc.monster_race_r_sights_get(self)
        if name == "r_deaths" : 
            return monsterc.monster_race_r_deaths_get(self)
        if name == "r_pkills" : 
            return monsterc.monster_race_r_pkills_get(self)
        if name == "r_tkills" : 
            return monsterc.monster_race_r_tkills_get(self)
        if name == "r_wake" : 
            return monsterc.monster_race_r_wake_get(self)
        if name == "r_ignore" : 
            return monsterc.monster_race_r_ignore_get(self)
        if name == "r_xtra1" : 
            return monsterc.monster_race_r_xtra1_get(self)
        if name == "r_xtra2" : 
            return monsterc.monster_race_r_xtra2_get(self)
        if name == "r_drop_gold" : 
            return monsterc.monster_race_r_drop_gold_get(self)
        if name == "r_drop_item" : 
            return monsterc.monster_race_r_drop_item_get(self)
        if name == "r_cast_inate" : 
            return monsterc.monster_race_r_cast_inate_get(self)
        if name == "r_cast_spell" : 
            return monsterc.monster_race_r_cast_spell_get(self)
        if name == "r_blows" : 
            return monsterc.monster_race_r_blows_get(self)
        if name == "r_flags1" : 
            return monsterc.monster_race_r_flags1_get(self)
        if name == "r_flags2" : 
            return monsterc.monster_race_r_flags2_get(self)
        if name == "r_flags3" : 
            return monsterc.monster_race_r_flags3_get(self)
        if name == "r_flags4" : 
            return monsterc.monster_race_r_flags4_get(self)
        if name == "r_flags5" : 
            return monsterc.monster_race_r_flags5_get(self)
        if name == "r_flags6" : 
            return monsterc.monster_race_r_flags6_get(self)
        if name == "r_flags7" : 
            return monsterc.monster_race_r_flags7_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C monster_race instance at %s>" % (self.this,)
class monster_race(monster_racePtr):
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------

def get_monster(*_args, **_kwargs):
    val = apply(monsterc.get_monster,_args,_kwargs)
    if val: val = monster_typePtr(val)
    return val

get_wilderness_flag = monsterc.get_wilderness_flag

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

monster_wood = monsterc.monster_wood

monster_volcano = monsterc.monster_volcano

monster_mountain = monsterc.monster_mountain

monster_grass = monsterc.monster_grass

monster_deep_water = monsterc.monster_deep_water

monster_shallow_water = monsterc.monster_shallow_water

monster_lava = monsterc.monster_lava

get_monster_hook = monsterc.get_monster_hook

get_monster_hook2 = monsterc.get_monster_hook2

is_friendly = monsterc.is_friendly

set_friendly = monsterc.set_friendly

is_pet = monsterc.is_pet

set_pet = monsterc.set_pet

is_hostile = monsterc.is_hostile

set_hostile = monsterc.set_hostile

monster_can_cross_terrain = monsterc.monster_can_cross_terrain

are_enemies = monsterc.are_enemies

monster_living = monsterc.monster_living



#-------------- VARIABLE WRAPPERS ------------------

