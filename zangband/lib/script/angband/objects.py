# This file was created automatically by SWIG.
import objectsc
class object_kindPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "name" :
            objectsc.object_kind_name_set(self.this,value)
            return
        if name == "text" :
            objectsc.object_kind_text_set(self.this,value)
            return
        if name == "tval" :
            objectsc.object_kind_tval_set(self.this,value)
            return
        if name == "sval" :
            objectsc.object_kind_sval_set(self.this,value)
            return
        if name == "pval" :
            objectsc.object_kind_pval_set(self.this,value)
            return
        if name == "to_h" :
            objectsc.object_kind_to_h_set(self.this,value)
            return
        if name == "to_d" :
            objectsc.object_kind_to_d_set(self.this,value)
            return
        if name == "to_a" :
            objectsc.object_kind_to_a_set(self.this,value)
            return
        if name == "ac" :
            objectsc.object_kind_ac_set(self.this,value)
            return
        if name == "dd" :
            objectsc.object_kind_dd_set(self.this,value)
            return
        if name == "ds" :
            objectsc.object_kind_ds_set(self.this,value)
            return
        if name == "weight" :
            objectsc.object_kind_weight_set(self.this,value)
            return
        if name == "cost" :
            objectsc.object_kind_cost_set(self.this,value)
            return
        if name == "flags1" :
            objectsc.object_kind_flags1_set(self.this,value)
            return
        if name == "flags2" :
            objectsc.object_kind_flags2_set(self.this,value)
            return
        if name == "flags3" :
            objectsc.object_kind_flags3_set(self.this,value)
            return
        if name == "locale" :
            objectsc.object_kind_locale_set(self.this,value)
            return
        if name == "chance" :
            objectsc.object_kind_chance_set(self.this,value)
            return
        if name == "level" :
            objectsc.object_kind_level_set(self.this,value)
            return
        if name == "extra" :
            objectsc.object_kind_extra_set(self.this,value)
            return
        if name == "d_attr" :
            objectsc.object_kind_d_attr_set(self.this,value)
            return
        if name == "d_char" :
            objectsc.object_kind_d_char_set(self.this,value)
            return
        if name == "x_attr" :
            objectsc.object_kind_x_attr_set(self.this,value)
            return
        if name == "x_char" :
            objectsc.object_kind_x_char_set(self.this,value)
            return
        if name == "flavor" :
            objectsc.object_kind_flavor_set(self.this,value)
            return
        if name == "easy_know" :
            objectsc.object_kind_easy_know_set(self.this,value)
            return
        if name == "aware" :
            objectsc.object_kind_aware_set(self.this,value)
            return
        if name == "tried" :
            objectsc.object_kind_tried_set(self.this,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "name" : 
            return objectsc.object_kind_name_get(self.this)
        if name == "text" : 
            return objectsc.object_kind_text_get(self.this)
        if name == "tval" : 
            return objectsc.object_kind_tval_get(self.this)
        if name == "sval" : 
            return objectsc.object_kind_sval_get(self.this)
        if name == "pval" : 
            return objectsc.object_kind_pval_get(self.this)
        if name == "to_h" : 
            return objectsc.object_kind_to_h_get(self.this)
        if name == "to_d" : 
            return objectsc.object_kind_to_d_get(self.this)
        if name == "to_a" : 
            return objectsc.object_kind_to_a_get(self.this)
        if name == "ac" : 
            return objectsc.object_kind_ac_get(self.this)
        if name == "dd" : 
            return objectsc.object_kind_dd_get(self.this)
        if name == "ds" : 
            return objectsc.object_kind_ds_get(self.this)
        if name == "weight" : 
            return objectsc.object_kind_weight_get(self.this)
        if name == "cost" : 
            return objectsc.object_kind_cost_get(self.this)
        if name == "flags1" : 
            return objectsc.object_kind_flags1_get(self.this)
        if name == "flags2" : 
            return objectsc.object_kind_flags2_get(self.this)
        if name == "flags3" : 
            return objectsc.object_kind_flags3_get(self.this)
        if name == "locale" : 
            return objectsc.object_kind_locale_get(self.this)
        if name == "chance" : 
            return objectsc.object_kind_chance_get(self.this)
        if name == "level" : 
            return objectsc.object_kind_level_get(self.this)
        if name == "extra" : 
            return objectsc.object_kind_extra_get(self.this)
        if name == "d_attr" : 
            return objectsc.object_kind_d_attr_get(self.this)
        if name == "d_char" : 
            return objectsc.object_kind_d_char_get(self.this)
        if name == "x_attr" : 
            return objectsc.object_kind_x_attr_get(self.this)
        if name == "x_char" : 
            return objectsc.object_kind_x_char_get(self.this)
        if name == "flavor" : 
            return objectsc.object_kind_flavor_get(self.this)
        if name == "easy_know" : 
            return objectsc.object_kind_easy_know_get(self.this)
        if name == "aware" : 
            return objectsc.object_kind_aware_get(self.this)
        if name == "tried" : 
            return objectsc.object_kind_tried_get(self.this)
        raise AttributeError,name
    def __repr__(self):
        return "<C object_kind instance>"
class object_kind(object_kindPtr):
    def __init__(self,this):
        self.this = this




class object_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            objectsc.delete_object_type(self.this)
    def object_wipe(self):
        val = objectsc.object_type_object_wipe(self.this)
        return val
    def object_copy(self,arg0):
        val = objectsc.object_type_object_copy(self.this,arg0.this)
        return val
    def object_prep(self,arg0):
        val = objectsc.object_type_object_prep(self.this,arg0)
        return val
    def drop_near(self,arg0,arg1,arg2):
        val = objectsc.object_type_drop_near(self.this,arg0,arg1,arg2)
        return val
    def activation(self):
        val = objectsc.object_type_activation(self.this)
        return val
    def identify_fully_aux(self):
        val = objectsc.object_type_identify_fully_aux(self.this)
        return val
    def object_known(self):
        val = objectsc.object_type_object_known(self.this)
        return val
    def object_aware(self):
        val = objectsc.object_type_object_aware(self.this)
        return val
    def set_python(self,arg0):
        val = objectsc.object_type_set_python(self.this,arg0)
        return val
    def __setattr__(self,name,value):
        if name == "k_idx" :
            objectsc.object_type_k_idx_set(self.this,value)
            return
        if name == "iy" :
            objectsc.object_type_iy_set(self.this,value)
            return
        if name == "ix" :
            objectsc.object_type_ix_set(self.this,value)
            return
        if name == "tval" :
            objectsc.object_type_tval_set(self.this,value)
            return
        if name == "sval" :
            objectsc.object_type_sval_set(self.this,value)
            return
        if name == "pval" :
            objectsc.object_type_pval_set(self.this,value)
            return
        if name == "discount" :
            objectsc.object_type_discount_set(self.this,value)
            return
        if name == "number" :
            objectsc.object_type_number_set(self.this,value)
            return
        if name == "weight" :
            objectsc.object_type_weight_set(self.this,value)
            return
        if name == "name1" :
            objectsc.object_type_name1_set(self.this,value)
            return
        if name == "name2" :
            objectsc.object_type_name2_set(self.this,value)
            return
        if name == "xtra1" :
            objectsc.object_type_xtra1_set(self.this,value)
            return
        if name == "xtra2" :
            objectsc.object_type_xtra2_set(self.this,value)
            return
        if name == "to_h" :
            objectsc.object_type_to_h_set(self.this,value)
            return
        if name == "to_d" :
            objectsc.object_type_to_d_set(self.this,value)
            return
        if name == "to_a" :
            objectsc.object_type_to_a_set(self.this,value)
            return
        if name == "ac" :
            objectsc.object_type_ac_set(self.this,value)
            return
        if name == "dd" :
            objectsc.object_type_dd_set(self.this,value)
            return
        if name == "ds" :
            objectsc.object_type_ds_set(self.this,value)
            return
        if name == "timeout" :
            objectsc.object_type_timeout_set(self.this,value)
            return
        if name == "ident" :
            objectsc.object_type_ident_set(self.this,value)
            return
        if name == "marked" :
            objectsc.object_type_marked_set(self.this,value)
            return
<<<<<<< objects.py
        if name == "inscription" :
            objectsc.object_type_inscription_set(self,value)
=======
        if name == "inscription" :
            objectsc.object_type_inscription_set(self.this,value)
>>>>>>> 1.2
            return
        if name == "art_name" :
            objectsc.object_type_art_name_set(self.this,value)
            return
        if name == "art_flags1" :
            objectsc.object_type_art_flags1_set(self.this,value)
            return
        if name == "art_flags2" :
            objectsc.object_type_art_flags2_set(self.this,value)
            return
        if name == "art_flags3" :
            objectsc.object_type_art_flags3_set(self.this,value)
            return
        if name == "next_o_idx" :
            objectsc.object_type_next_o_idx_set(self.this,value)
            return
        if name == "held_m_idx" :
            objectsc.object_type_held_m_idx_set(self.this,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "k_idx" : 
            return objectsc.object_type_k_idx_get(self.this)
        if name == "iy" : 
            return objectsc.object_type_iy_get(self.this)
        if name == "ix" : 
            return objectsc.object_type_ix_get(self.this)
        if name == "tval" : 
            return objectsc.object_type_tval_get(self.this)
        if name == "sval" : 
            return objectsc.object_type_sval_get(self.this)
        if name == "pval" : 
            return objectsc.object_type_pval_get(self.this)
        if name == "discount" : 
            return objectsc.object_type_discount_get(self.this)
        if name == "number" : 
            return objectsc.object_type_number_get(self.this)
        if name == "weight" : 
            return objectsc.object_type_weight_get(self.this)
        if name == "name1" : 
            return objectsc.object_type_name1_get(self.this)
        if name == "name2" : 
            return objectsc.object_type_name2_get(self.this)
        if name == "xtra1" : 
            return objectsc.object_type_xtra1_get(self.this)
        if name == "xtra2" : 
            return objectsc.object_type_xtra2_get(self.this)
        if name == "to_h" : 
            return objectsc.object_type_to_h_get(self.this)
        if name == "to_d" : 
            return objectsc.object_type_to_d_get(self.this)
        if name == "to_a" : 
            return objectsc.object_type_to_a_get(self.this)
        if name == "ac" : 
            return objectsc.object_type_ac_get(self.this)
        if name == "dd" : 
            return objectsc.object_type_dd_get(self.this)
        if name == "ds" : 
            return objectsc.object_type_ds_get(self.this)
        if name == "timeout" : 
            return objectsc.object_type_timeout_get(self.this)
        if name == "ident" : 
            return objectsc.object_type_ident_get(self.this)
        if name == "marked" : 
<<<<<<< objects.py
            return objectsc.object_type_marked_get(self)
        if name == "inscription" : 
            return objectsc.object_type_inscription_get(self)
=======
            return objectsc.object_type_marked_get(self.this)
        if name == "inscription" : 
            return objectsc.object_type_inscription_get(self.this)
>>>>>>> 1.2
        if name == "art_name" : 
            return objectsc.object_type_art_name_get(self.this)
        if name == "art_flags1" : 
            return objectsc.object_type_art_flags1_get(self.this)
        if name == "art_flags2" : 
            return objectsc.object_type_art_flags2_get(self.this)
        if name == "art_flags3" : 
            return objectsc.object_type_art_flags3_get(self.this)
        if name == "next_o_idx" : 
            return objectsc.object_type_next_o_idx_get(self.this)
        if name == "held_m_idx" : 
            return objectsc.object_type_held_m_idx_get(self.this)
        raise AttributeError,name
    def __repr__(self):
        return "<C object_type instance>"
class object_type(object_typePtr):
    def __init__(self) :
        self.this = objectsc.new_object_type()
        self.thisown = 1




class artifact_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "name" :
            objectsc.artifact_type_name_set(self.this,value)
            return
        if name == "text" :
            objectsc.artifact_type_text_set(self.this,value)
            return
        if name == "tval" :
            objectsc.artifact_type_tval_set(self.this,value)
            return
        if name == "sval" :
            objectsc.artifact_type_sval_set(self.this,value)
            return
        if name == "pval" :
            objectsc.artifact_type_pval_set(self.this,value)
            return
        if name == "to_h" :
            objectsc.artifact_type_to_h_set(self.this,value)
            return
        if name == "to_d" :
            objectsc.artifact_type_to_d_set(self.this,value)
            return
        if name == "to_a" :
            objectsc.artifact_type_to_a_set(self.this,value)
            return
        if name == "ac" :
            objectsc.artifact_type_ac_set(self.this,value)
            return
        if name == "dd" :
            objectsc.artifact_type_dd_set(self.this,value)
            return
        if name == "ds" :
            objectsc.artifact_type_ds_set(self.this,value)
            return
        if name == "weight" :
            objectsc.artifact_type_weight_set(self.this,value)
            return
        if name == "cost" :
            objectsc.artifact_type_cost_set(self.this,value)
            return
        if name == "flags1" :
            objectsc.artifact_type_flags1_set(self.this,value)
            return
        if name == "flags2" :
            objectsc.artifact_type_flags2_set(self.this,value)
            return
        if name == "flags3" :
            objectsc.artifact_type_flags3_set(self.this,value)
            return
        if name == "level" :
            objectsc.artifact_type_level_set(self.this,value)
            return
        if name == "rarity" :
            objectsc.artifact_type_rarity_set(self.this,value)
            return
        if name == "cur_num" :
            objectsc.artifact_type_cur_num_set(self.this,value)
            return
        if name == "max_num" :
            objectsc.artifact_type_max_num_set(self.this,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "name" : 
            return objectsc.artifact_type_name_get(self.this)
        if name == "text" : 
            return objectsc.artifact_type_text_get(self.this)
        if name == "tval" : 
            return objectsc.artifact_type_tval_get(self.this)
        if name == "sval" : 
            return objectsc.artifact_type_sval_get(self.this)
        if name == "pval" : 
            return objectsc.artifact_type_pval_get(self.this)
        if name == "to_h" : 
            return objectsc.artifact_type_to_h_get(self.this)
        if name == "to_d" : 
            return objectsc.artifact_type_to_d_get(self.this)
        if name == "to_a" : 
            return objectsc.artifact_type_to_a_get(self.this)
        if name == "ac" : 
            return objectsc.artifact_type_ac_get(self.this)
        if name == "dd" : 
            return objectsc.artifact_type_dd_get(self.this)
        if name == "ds" : 
            return objectsc.artifact_type_ds_get(self.this)
        if name == "weight" : 
            return objectsc.artifact_type_weight_get(self.this)
        if name == "cost" : 
            return objectsc.artifact_type_cost_get(self.this)
        if name == "flags1" : 
            return objectsc.artifact_type_flags1_get(self.this)
        if name == "flags2" : 
            return objectsc.artifact_type_flags2_get(self.this)
        if name == "flags3" : 
            return objectsc.artifact_type_flags3_get(self.this)
        if name == "level" : 
            return objectsc.artifact_type_level_get(self.this)
        if name == "rarity" : 
            return objectsc.artifact_type_rarity_get(self.this)
        if name == "cur_num" : 
            return objectsc.artifact_type_cur_num_get(self.this)
        if name == "max_num" : 
            return objectsc.artifact_type_max_num_get(self.this)
        raise AttributeError,name
    def __repr__(self):
        return "<C artifact_type instance>"
class artifact_type(artifact_typePtr):
    def __init__(self,this):
        self.this = this




class ego_item_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "name" :
            objectsc.ego_item_type_name_set(self.this,value)
            return
        if name == "text" :
            objectsc.ego_item_type_text_set(self.this,value)
            return
        if name == "slot" :
            objectsc.ego_item_type_slot_set(self.this,value)
            return
        if name == "rating" :
            objectsc.ego_item_type_rating_set(self.this,value)
            return
        if name == "level" :
            objectsc.ego_item_type_level_set(self.this,value)
            return
        if name == "rarity" :
            objectsc.ego_item_type_rarity_set(self.this,value)
            return
        if name == "max_to_h" :
            objectsc.ego_item_type_max_to_h_set(self.this,value)
            return
        if name == "max_to_d" :
            objectsc.ego_item_type_max_to_d_set(self.this,value)
            return
        if name == "max_to_a" :
            objectsc.ego_item_type_max_to_a_set(self.this,value)
            return
        if name == "max_pval" :
            objectsc.ego_item_type_max_pval_set(self.this,value)
            return
        if name == "cost" :
            objectsc.ego_item_type_cost_set(self.this,value)
            return
        if name == "flags1" :
            objectsc.ego_item_type_flags1_set(self.this,value)
            return
        if name == "flags2" :
            objectsc.ego_item_type_flags2_set(self.this,value)
            return
        if name == "flags3" :
            objectsc.ego_item_type_flags3_set(self.this,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "name" : 
            return objectsc.ego_item_type_name_get(self.this)
        if name == "text" : 
            return objectsc.ego_item_type_text_get(self.this)
        if name == "slot" : 
            return objectsc.ego_item_type_slot_get(self.this)
        if name == "rating" : 
            return objectsc.ego_item_type_rating_get(self.this)
        if name == "level" : 
            return objectsc.ego_item_type_level_get(self.this)
        if name == "rarity" : 
            return objectsc.ego_item_type_rarity_get(self.this)
        if name == "max_to_h" : 
            return objectsc.ego_item_type_max_to_h_get(self.this)
        if name == "max_to_d" : 
            return objectsc.ego_item_type_max_to_d_get(self.this)
        if name == "max_to_a" : 
            return objectsc.ego_item_type_max_to_a_get(self.this)
        if name == "max_pval" : 
            return objectsc.ego_item_type_max_pval_get(self.this)
        if name == "cost" : 
            return objectsc.ego_item_type_cost_get(self.this)
        if name == "flags1" : 
            return objectsc.ego_item_type_flags1_get(self.this)
        if name == "flags2" : 
            return objectsc.ego_item_type_flags2_get(self.this)
        if name == "flags3" : 
            return objectsc.ego_item_type_flags3_get(self.this)
        raise AttributeError,name
    def __repr__(self):
        return "<C ego_item_type instance>"
class ego_item_type(ego_item_typePtr):
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------

lookup_kind = objectsc.lookup_kind



#-------------- VARIABLE WRAPPERS ------------------

TV_SKELETON = objectsc.TV_SKELETON
TV_BOTTLE = objectsc.TV_BOTTLE
TV_JUNK = objectsc.TV_JUNK
TV_SPIKE = objectsc.TV_SPIKE
TV_CHEST = objectsc.TV_CHEST
TV_SHOT = objectsc.TV_SHOT
TV_ARROW = objectsc.TV_ARROW
TV_BOLT = objectsc.TV_BOLT
TV_BOW = objectsc.TV_BOW
TV_DIGGING = objectsc.TV_DIGGING
TV_HAFTED = objectsc.TV_HAFTED
TV_POLEARM = objectsc.TV_POLEARM
TV_SWORD = objectsc.TV_SWORD
TV_BOOTS = objectsc.TV_BOOTS
TV_GLOVES = objectsc.TV_GLOVES
TV_HELM = objectsc.TV_HELM
TV_CROWN = objectsc.TV_CROWN
TV_SHIELD = objectsc.TV_SHIELD
TV_CLOAK = objectsc.TV_CLOAK
TV_SOFT_ARMOR = objectsc.TV_SOFT_ARMOR
TV_HARD_ARMOR = objectsc.TV_HARD_ARMOR
TV_DRAG_ARMOR = objectsc.TV_DRAG_ARMOR
TV_LITE = objectsc.TV_LITE
TV_AMULET = objectsc.TV_AMULET
TV_RING = objectsc.TV_RING
TV_STAFF = objectsc.TV_STAFF
TV_WAND = objectsc.TV_WAND
TV_ROD = objectsc.TV_ROD
TV_SCROLL = objectsc.TV_SCROLL
TV_POTION = objectsc.TV_POTION
TV_FLASK = objectsc.TV_FLASK
TV_FOOD = objectsc.TV_FOOD
TV_LIFE_BOOK = objectsc.TV_LIFE_BOOK
TV_SORCERY_BOOK = objectsc.TV_SORCERY_BOOK
TV_NATURE_BOOK = objectsc.TV_NATURE_BOOK
TV_CHAOS_BOOK = objectsc.TV_CHAOS_BOOK
TV_DEATH_BOOK = objectsc.TV_DEATH_BOOK
TV_TRUMP_BOOK = objectsc.TV_TRUMP_BOOK
TV_ARCANE_BOOK = objectsc.TV_ARCANE_BOOK
TV_GOLD = objectsc.TV_GOLD
SV_AMMO_LIGHT = objectsc.SV_AMMO_LIGHT
SV_AMMO_NORMAL = objectsc.SV_AMMO_NORMAL
SV_AMMO_HEAVY = objectsc.SV_AMMO_HEAVY
SV_SLING = objectsc.SV_SLING
SV_SHORT_BOW = objectsc.SV_SHORT_BOW
SV_LONG_BOW = objectsc.SV_LONG_BOW
SV_LIGHT_XBOW = objectsc.SV_LIGHT_XBOW
SV_HEAVY_XBOW = objectsc.SV_HEAVY_XBOW
SV_SHOVEL = objectsc.SV_SHOVEL
SV_GNOMISH_SHOVEL = objectsc.SV_GNOMISH_SHOVEL
SV_DWARVEN_SHOVEL = objectsc.SV_DWARVEN_SHOVEL
SV_PICK = objectsc.SV_PICK
SV_ORCISH_PICK = objectsc.SV_ORCISH_PICK
SV_DWARVEN_PICK = objectsc.SV_DWARVEN_PICK
SV_MATTOCK = objectsc.SV_MATTOCK
SV_CLUB = objectsc.SV_CLUB
SV_WHIP = objectsc.SV_WHIP
SV_QUARTERSTAFF = objectsc.SV_QUARTERSTAFF
SV_NUNCHAKU = objectsc.SV_NUNCHAKU
SV_MACE = objectsc.SV_MACE
SV_BALL_AND_CHAIN = objectsc.SV_BALL_AND_CHAIN
SV_JO_STAFF = objectsc.SV_JO_STAFF
SV_WAR_HAMMER = objectsc.SV_WAR_HAMMER
SV_LUCERN_HAMMER = objectsc.SV_LUCERN_HAMMER
SV_THREE_PIECE_ROD = objectsc.SV_THREE_PIECE_ROD
SV_MORNING_STAR = objectsc.SV_MORNING_STAR
SV_FLAIL = objectsc.SV_FLAIL
SV_BO_STAFF = objectsc.SV_BO_STAFF
SV_LEAD_FILLED_MACE = objectsc.SV_LEAD_FILLED_MACE
SV_TETSUBO = objectsc.SV_TETSUBO
SV_TWO_HANDED_FLAIL = objectsc.SV_TWO_HANDED_FLAIL
SV_GREAT_HAMMER = objectsc.SV_GREAT_HAMMER
SV_MACE_OF_DISRUPTION = objectsc.SV_MACE_OF_DISRUPTION
SV_GROND = objectsc.SV_GROND
SV_HATCHET = objectsc.SV_HATCHET
SV_SPEAR = objectsc.SV_SPEAR
SV_SICKLE = objectsc.SV_SICKLE
SV_AWL_PIKE = objectsc.SV_AWL_PIKE
SV_TRIDENT = objectsc.SV_TRIDENT
SV_FAUCHARD = objectsc.SV_FAUCHARD
SV_BROAD_SPEAR = objectsc.SV_BROAD_SPEAR
SV_PIKE = objectsc.SV_PIKE
SV_NAGINATA = objectsc.SV_NAGINATA
SV_BEAKED_AXE = objectsc.SV_BEAKED_AXE
SV_BROAD_AXE = objectsc.SV_BROAD_AXE
SV_GLAIVE = objectsc.SV_GLAIVE
SV_LAJATANG = objectsc.SV_LAJATANG
SV_HALBERD = objectsc.SV_HALBERD
SV_GUISARME = objectsc.SV_GUISARME
SV_SCYTHE = objectsc.SV_SCYTHE
SV_LANCE = objectsc.SV_LANCE
SV_BATTLE_AXE = objectsc.SV_BATTLE_AXE
SV_GREAT_AXE = objectsc.SV_GREAT_AXE
SV_TRIFURCATE_SPEAR = objectsc.SV_TRIFURCATE_SPEAR
SV_LOCHABER_AXE = objectsc.SV_LOCHABER_AXE
SV_HEAVY_LANCE = objectsc.SV_HEAVY_LANCE
SV_SCYTHE_OF_SLICING = objectsc.SV_SCYTHE_OF_SLICING
SV_BROKEN_DAGGER = objectsc.SV_BROKEN_DAGGER
SV_BROKEN_SWORD = objectsc.SV_BROKEN_SWORD
SV_DAGGER = objectsc.SV_DAGGER
SV_MAIN_GAUCHE = objectsc.SV_MAIN_GAUCHE
SV_TANTO = objectsc.SV_TANTO
SV_RAPIER = objectsc.SV_RAPIER
SV_SMALL_SWORD = objectsc.SV_SMALL_SWORD
SV_BASILLARD = objectsc.SV_BASILLARD
SV_SHORT_SWORD = objectsc.SV_SHORT_SWORD
SV_SABRE = objectsc.SV_SABRE
SV_CUTLASS = objectsc.SV_CUTLASS
SV_WAKIZASHI = objectsc.SV_WAKIZASHI
SV_KHOPESH = objectsc.SV_KHOPESH
SV_TULWAR = objectsc.SV_TULWAR
SV_BROAD_SWORD = objectsc.SV_BROAD_SWORD
SV_LONG_SWORD = objectsc.SV_LONG_SWORD
SV_SCIMITAR = objectsc.SV_SCIMITAR
SV_NINJATO = objectsc.SV_NINJATO
SV_KATANA = objectsc.SV_KATANA
SV_BASTARD_SWORD = objectsc.SV_BASTARD_SWORD
SV_GREAT_SCIMITAR = objectsc.SV_GREAT_SCIMITAR
SV_CLAYMORE = objectsc.SV_CLAYMORE
SV_ESPADON = objectsc.SV_ESPADON
SV_TWO_HANDED_SWORD = objectsc.SV_TWO_HANDED_SWORD
SV_FLAMBERGE = objectsc.SV_FLAMBERGE
SV_NO_DACHI = objectsc.SV_NO_DACHI
SV_EXECUTIONERS_SWORD = objectsc.SV_EXECUTIONERS_SWORD
SV_ZWEIHANDER = objectsc.SV_ZWEIHANDER
SV_BLADE_OF_CHAOS = objectsc.SV_BLADE_OF_CHAOS
SV_SMALL_LEATHER_SHIELD = objectsc.SV_SMALL_LEATHER_SHIELD
SV_SMALL_METAL_SHIELD = objectsc.SV_SMALL_METAL_SHIELD
SV_LARGE_LEATHER_SHIELD = objectsc.SV_LARGE_LEATHER_SHIELD
SV_LARGE_METAL_SHIELD = objectsc.SV_LARGE_METAL_SHIELD
SV_DRAGON_SHIELD = objectsc.SV_DRAGON_SHIELD
SV_SHIELD_OF_DEFLECTION = objectsc.SV_SHIELD_OF_DEFLECTION
SV_HARD_LEATHER_CAP = objectsc.SV_HARD_LEATHER_CAP
SV_METAL_CAP = objectsc.SV_METAL_CAP
SV_JINGASA = objectsc.SV_JINGASA
SV_IRON_HELM = objectsc.SV_IRON_HELM
SV_STEEL_HELM = objectsc.SV_STEEL_HELM
SV_DRAGON_HELM = objectsc.SV_DRAGON_HELM
SV_KABUTO = objectsc.SV_KABUTO
SV_IRON_CROWN = objectsc.SV_IRON_CROWN
SV_GOLDEN_CROWN = objectsc.SV_GOLDEN_CROWN
SV_JEWELED_CROWN = objectsc.SV_JEWELED_CROWN
SV_MORGOTH = objectsc.SV_MORGOTH
SV_PAIR_OF_SOFT_LEATHER_BOOTS = objectsc.SV_PAIR_OF_SOFT_LEATHER_BOOTS
SV_PAIR_OF_HARD_LEATHER_BOOTS = objectsc.SV_PAIR_OF_HARD_LEATHER_BOOTS
SV_PAIR_OF_METAL_SHOD_BOOTS = objectsc.SV_PAIR_OF_METAL_SHOD_BOOTS
SV_CLOAK = objectsc.SV_CLOAK
SV_ELVEN_CLOAK = objectsc.SV_ELVEN_CLOAK
SV_FUR_CLOAK = objectsc.SV_FUR_CLOAK
SV_SHADOW_CLOAK = objectsc.SV_SHADOW_CLOAK
SV_SET_OF_LEATHER_GLOVES = objectsc.SV_SET_OF_LEATHER_GLOVES
SV_SET_OF_GAUNTLETS = objectsc.SV_SET_OF_GAUNTLETS
SV_SET_OF_CESTI = objectsc.SV_SET_OF_CESTI
SV_FILTHY_RAG = objectsc.SV_FILTHY_RAG
SV_ROBE = objectsc.SV_ROBE
SV_PAPER_ARMOR = objectsc.SV_PAPER_ARMOR
SV_SOFT_LEATHER_ARMOR = objectsc.SV_SOFT_LEATHER_ARMOR
SV_SOFT_STUDDED_LEATHER = objectsc.SV_SOFT_STUDDED_LEATHER
SV_HARD_LEATHER_ARMOR = objectsc.SV_HARD_LEATHER_ARMOR
SV_HARD_STUDDED_LEATHER = objectsc.SV_HARD_STUDDED_LEATHER
SV_RHINO_HIDE_ARMOR = objectsc.SV_RHINO_HIDE_ARMOR
SV_CORD_ARMOR = objectsc.SV_CORD_ARMOR
SV_PADDED_ARMOR = objectsc.SV_PADDED_ARMOR
SV_LEATHER_SCALE_MAIL = objectsc.SV_LEATHER_SCALE_MAIL
SV_LEATHER_JACK = objectsc.SV_LEATHER_JACK
SV_STONE_AND_HIDE_ARMOR = objectsc.SV_STONE_AND_HIDE_ARMOR
SV_RUSTY_CHAIN_MAIL = objectsc.SV_RUSTY_CHAIN_MAIL
SV_RING_MAIL = objectsc.SV_RING_MAIL
SV_METAL_SCALE_MAIL = objectsc.SV_METAL_SCALE_MAIL
SV_CHAIN_MAIL = objectsc.SV_CHAIN_MAIL
SV_DOUBLE_RING_MAIL = objectsc.SV_DOUBLE_RING_MAIL
SV_AUGMENTED_CHAIN_MAIL = objectsc.SV_AUGMENTED_CHAIN_MAIL
SV_DOUBLE_CHAIN_MAIL = objectsc.SV_DOUBLE_CHAIN_MAIL
SV_BAR_CHAIN_MAIL = objectsc.SV_BAR_CHAIN_MAIL
SV_METAL_BRIGANDINE_ARMOUR = objectsc.SV_METAL_BRIGANDINE_ARMOUR
SV_SPLINT_MAIL = objectsc.SV_SPLINT_MAIL
SV_DO_MARU = objectsc.SV_DO_MARU
SV_PARTIAL_PLATE_ARMOUR = objectsc.SV_PARTIAL_PLATE_ARMOUR
SV_METAL_LAMELLAR_ARMOUR = objectsc.SV_METAL_LAMELLAR_ARMOUR
SV_HARAMAKIDO = objectsc.SV_HARAMAKIDO
SV_FULL_PLATE_ARMOUR = objectsc.SV_FULL_PLATE_ARMOUR
SV_O_YOROI = objectsc.SV_O_YOROI
SV_RIBBED_PLATE_ARMOUR = objectsc.SV_RIBBED_PLATE_ARMOUR
SV_MITHRIL_CHAIN_MAIL = objectsc.SV_MITHRIL_CHAIN_MAIL
SV_MITHRIL_PLATE_MAIL = objectsc.SV_MITHRIL_PLATE_MAIL
SV_ADAMANTITE_PLATE_MAIL = objectsc.SV_ADAMANTITE_PLATE_MAIL
SV_DRAGON_BLACK = objectsc.SV_DRAGON_BLACK
SV_DRAGON_BLUE = objectsc.SV_DRAGON_BLUE
SV_DRAGON_WHITE = objectsc.SV_DRAGON_WHITE
SV_DRAGON_RED = objectsc.SV_DRAGON_RED
SV_DRAGON_GREEN = objectsc.SV_DRAGON_GREEN
SV_DRAGON_MULTIHUED = objectsc.SV_DRAGON_MULTIHUED
SV_DRAGON_SHINING = objectsc.SV_DRAGON_SHINING
SV_DRAGON_LAW = objectsc.SV_DRAGON_LAW
SV_DRAGON_BRONZE = objectsc.SV_DRAGON_BRONZE
SV_DRAGON_GOLD = objectsc.SV_DRAGON_GOLD
SV_DRAGON_CHAOS = objectsc.SV_DRAGON_CHAOS
SV_DRAGON_BALANCE = objectsc.SV_DRAGON_BALANCE
SV_DRAGON_POWER = objectsc.SV_DRAGON_POWER
SV_LITE_TORCH = objectsc.SV_LITE_TORCH
SV_LITE_LANTERN = objectsc.SV_LITE_LANTERN
SV_LITE_GALADRIEL = objectsc.SV_LITE_GALADRIEL
SV_LITE_ELENDIL = objectsc.SV_LITE_ELENDIL
SV_LITE_THRAIN = objectsc.SV_LITE_THRAIN
SV_AMULET_DOOM = objectsc.SV_AMULET_DOOM
SV_AMULET_TELEPORT = objectsc.SV_AMULET_TELEPORT
SV_AMULET_ADORNMENT = objectsc.SV_AMULET_ADORNMENT
SV_AMULET_SLOW_DIGEST = objectsc.SV_AMULET_SLOW_DIGEST
SV_AMULET_RESIST_ACID = objectsc.SV_AMULET_RESIST_ACID
SV_AMULET_SEARCHING = objectsc.SV_AMULET_SEARCHING
SV_AMULET_WISDOM = objectsc.SV_AMULET_WISDOM
SV_AMULET_CHARISMA = objectsc.SV_AMULET_CHARISMA
SV_AMULET_THE_MAGI = objectsc.SV_AMULET_THE_MAGI
SV_AMULET_REFLECTION = objectsc.SV_AMULET_REFLECTION
SV_AMULET_CARLAMMAS = objectsc.SV_AMULET_CARLAMMAS
SV_AMULET_INGWE = objectsc.SV_AMULET_INGWE
SV_AMULET_DWARVES = objectsc.SV_AMULET_DWARVES
SV_AMULET_NO_MAGIC = objectsc.SV_AMULET_NO_MAGIC
SV_AMULET_NO_TELE = objectsc.SV_AMULET_NO_TELE
SV_AMULET_RESISTANCE = objectsc.SV_AMULET_RESISTANCE
SV_RING_WOE = objectsc.SV_RING_WOE
SV_RING_AGGRAVATION = objectsc.SV_RING_AGGRAVATION
SV_RING_WEAKNESS = objectsc.SV_RING_WEAKNESS
SV_RING_STUPIDITY = objectsc.SV_RING_STUPIDITY
SV_RING_TELEPORTATION = objectsc.SV_RING_TELEPORTATION
SV_RING_SLOW_DIGESTION = objectsc.SV_RING_SLOW_DIGESTION
SV_RING_FEATHER_FALL = objectsc.SV_RING_FEATHER_FALL
SV_RING_RESIST_FIRE = objectsc.SV_RING_RESIST_FIRE
SV_RING_RESIST_COLD = objectsc.SV_RING_RESIST_COLD
SV_RING_SUSTAIN_STR = objectsc.SV_RING_SUSTAIN_STR
SV_RING_SUSTAIN_INT = objectsc.SV_RING_SUSTAIN_INT
SV_RING_SUSTAIN_WIS = objectsc.SV_RING_SUSTAIN_WIS
SV_RING_SUSTAIN_DEX = objectsc.SV_RING_SUSTAIN_DEX
SV_RING_SUSTAIN_CON = objectsc.SV_RING_SUSTAIN_CON
SV_RING_SUSTAIN_CHR = objectsc.SV_RING_SUSTAIN_CHR
SV_RING_PROTECTION = objectsc.SV_RING_PROTECTION
SV_RING_ACID = objectsc.SV_RING_ACID
SV_RING_FLAMES = objectsc.SV_RING_FLAMES
SV_RING_ICE = objectsc.SV_RING_ICE
SV_RING_RESIST_POIS = objectsc.SV_RING_RESIST_POIS
SV_RING_FREE_ACTION = objectsc.SV_RING_FREE_ACTION
SV_RING_SEE_INVIS = objectsc.SV_RING_SEE_INVIS
SV_RING_SEARCHING = objectsc.SV_RING_SEARCHING
SV_RING_STR = objectsc.SV_RING_STR
SV_RING_INT = objectsc.SV_RING_INT
SV_RING_DEX = objectsc.SV_RING_DEX
SV_RING_CON = objectsc.SV_RING_CON
SV_RING_ACCURACY = objectsc.SV_RING_ACCURACY
SV_RING_DAMAGE = objectsc.SV_RING_DAMAGE
SV_RING_SLAYING = objectsc.SV_RING_SLAYING
SV_RING_SPEED = objectsc.SV_RING_SPEED
SV_RING_BARAHIR = objectsc.SV_RING_BARAHIR
SV_RING_TULKAS = objectsc.SV_RING_TULKAS
SV_RING_NARYA = objectsc.SV_RING_NARYA
SV_RING_NENYA = objectsc.SV_RING_NENYA
SV_RING_VILYA = objectsc.SV_RING_VILYA
SV_RING_POWER = objectsc.SV_RING_POWER
SV_RING_RES_FEAR = objectsc.SV_RING_RES_FEAR
SV_RING_RES_LD = objectsc.SV_RING_RES_LD
SV_RING_RES_NETHER = objectsc.SV_RING_RES_NETHER
SV_RING_RES_NEXUS = objectsc.SV_RING_RES_NEXUS
SV_RING_RES_SOUND = objectsc.SV_RING_RES_SOUND
SV_RING_RES_CONFUSION = objectsc.SV_RING_RES_CONFUSION
SV_RING_RES_SHARDS = objectsc.SV_RING_RES_SHARDS
SV_RING_RES_DISENCHANT = objectsc.SV_RING_RES_DISENCHANT
SV_RING_RES_CHAOS = objectsc.SV_RING_RES_CHAOS
SV_RING_RES_BLINDNESS = objectsc.SV_RING_RES_BLINDNESS
SV_RING_LORDLY = objectsc.SV_RING_LORDLY
SV_RING_ATTACKS = objectsc.SV_RING_ATTACKS
SV_STAFF_DARKNESS = objectsc.SV_STAFF_DARKNESS
SV_STAFF_SLOWNESS = objectsc.SV_STAFF_SLOWNESS
SV_STAFF_HASTE_MONSTERS = objectsc.SV_STAFF_HASTE_MONSTERS
SV_STAFF_SUMMONING = objectsc.SV_STAFF_SUMMONING
SV_STAFF_TELEPORTATION = objectsc.SV_STAFF_TELEPORTATION
SV_STAFF_IDENTIFY = objectsc.SV_STAFF_IDENTIFY
SV_STAFF_REMOVE_CURSE = objectsc.SV_STAFF_REMOVE_CURSE
SV_STAFF_STARLITE = objectsc.SV_STAFF_STARLITE
SV_STAFF_LITE = objectsc.SV_STAFF_LITE
SV_STAFF_MAPPING = objectsc.SV_STAFF_MAPPING
SV_STAFF_DETECT_GOLD = objectsc.SV_STAFF_DETECT_GOLD
SV_STAFF_DETECT_ITEM = objectsc.SV_STAFF_DETECT_ITEM
SV_STAFF_DETECT_TRAP = objectsc.SV_STAFF_DETECT_TRAP
SV_STAFF_DETECT_DOOR = objectsc.SV_STAFF_DETECT_DOOR
SV_STAFF_DETECT_INVIS = objectsc.SV_STAFF_DETECT_INVIS
SV_STAFF_DETECT_EVIL = objectsc.SV_STAFF_DETECT_EVIL
SV_STAFF_CURE_LIGHT = objectsc.SV_STAFF_CURE_LIGHT
SV_STAFF_CURING = objectsc.SV_STAFF_CURING
SV_STAFF_HEALING = objectsc.SV_STAFF_HEALING
SV_STAFF_THE_MAGI = objectsc.SV_STAFF_THE_MAGI
SV_STAFF_SLEEP_MONSTERS = objectsc.SV_STAFF_SLEEP_MONSTERS
SV_STAFF_SLOW_MONSTERS = objectsc.SV_STAFF_SLOW_MONSTERS
SV_STAFF_SPEED = objectsc.SV_STAFF_SPEED
SV_STAFF_PROBING = objectsc.SV_STAFF_PROBING
SV_STAFF_DISPEL_EVIL = objectsc.SV_STAFF_DISPEL_EVIL
SV_STAFF_POWER = objectsc.SV_STAFF_POWER
SV_STAFF_HOLINESS = objectsc.SV_STAFF_HOLINESS
SV_STAFF_GENOCIDE = objectsc.SV_STAFF_GENOCIDE
SV_STAFF_EARTHQUAKES = objectsc.SV_STAFF_EARTHQUAKES
SV_STAFF_DESTRUCTION = objectsc.SV_STAFF_DESTRUCTION
SV_WAND_HEAL_MONSTER = objectsc.SV_WAND_HEAL_MONSTER
SV_WAND_HASTE_MONSTER = objectsc.SV_WAND_HASTE_MONSTER
SV_WAND_CLONE_MONSTER = objectsc.SV_WAND_CLONE_MONSTER
SV_WAND_TELEPORT_AWAY = objectsc.SV_WAND_TELEPORT_AWAY
SV_WAND_DISARMING = objectsc.SV_WAND_DISARMING
SV_WAND_TRAP_DOOR_DEST = objectsc.SV_WAND_TRAP_DOOR_DEST
SV_WAND_STONE_TO_MUD = objectsc.SV_WAND_STONE_TO_MUD
SV_WAND_LITE = objectsc.SV_WAND_LITE
SV_WAND_SLEEP_MONSTER = objectsc.SV_WAND_SLEEP_MONSTER
SV_WAND_SLOW_MONSTER = objectsc.SV_WAND_SLOW_MONSTER
SV_WAND_CONFUSE_MONSTER = objectsc.SV_WAND_CONFUSE_MONSTER
SV_WAND_FEAR_MONSTER = objectsc.SV_WAND_FEAR_MONSTER
SV_WAND_DRAIN_LIFE = objectsc.SV_WAND_DRAIN_LIFE
SV_WAND_POLYMORPH = objectsc.SV_WAND_POLYMORPH
SV_WAND_STINKING_CLOUD = objectsc.SV_WAND_STINKING_CLOUD
SV_WAND_MAGIC_MISSILE = objectsc.SV_WAND_MAGIC_MISSILE
SV_WAND_ACID_BOLT = objectsc.SV_WAND_ACID_BOLT
SV_WAND_CHARM_MONSTER = objectsc.SV_WAND_CHARM_MONSTER
SV_WAND_FIRE_BOLT = objectsc.SV_WAND_FIRE_BOLT
SV_WAND_COLD_BOLT = objectsc.SV_WAND_COLD_BOLT
SV_WAND_ACID_BALL = objectsc.SV_WAND_ACID_BALL
SV_WAND_ELEC_BALL = objectsc.SV_WAND_ELEC_BALL
SV_WAND_FIRE_BALL = objectsc.SV_WAND_FIRE_BALL
SV_WAND_COLD_BALL = objectsc.SV_WAND_COLD_BALL
SV_WAND_WONDER = objectsc.SV_WAND_WONDER
SV_WAND_ANNIHILATION = objectsc.SV_WAND_ANNIHILATION
SV_WAND_DRAGON_FIRE = objectsc.SV_WAND_DRAGON_FIRE
SV_WAND_DRAGON_COLD = objectsc.SV_WAND_DRAGON_COLD
SV_WAND_DRAGON_BREATH = objectsc.SV_WAND_DRAGON_BREATH
SV_WAND_ROCKETS = objectsc.SV_WAND_ROCKETS
SV_ROD_DETECT_TRAP = objectsc.SV_ROD_DETECT_TRAP
SV_ROD_DETECT_DOOR = objectsc.SV_ROD_DETECT_DOOR
SV_ROD_IDENTIFY = objectsc.SV_ROD_IDENTIFY
SV_ROD_RECALL = objectsc.SV_ROD_RECALL
SV_ROD_ILLUMINATION = objectsc.SV_ROD_ILLUMINATION
SV_ROD_MAPPING = objectsc.SV_ROD_MAPPING
SV_ROD_DETECTION = objectsc.SV_ROD_DETECTION
SV_ROD_PROBING = objectsc.SV_ROD_PROBING
SV_ROD_CURING = objectsc.SV_ROD_CURING
SV_ROD_HEALING = objectsc.SV_ROD_HEALING
SV_ROD_RESTORATION = objectsc.SV_ROD_RESTORATION
SV_ROD_SPEED = objectsc.SV_ROD_SPEED
SV_ROD_PESTICIDE = objectsc.SV_ROD_PESTICIDE
SV_ROD_TELEPORT_AWAY = objectsc.SV_ROD_TELEPORT_AWAY
SV_ROD_DISARMING = objectsc.SV_ROD_DISARMING
SV_ROD_LITE = objectsc.SV_ROD_LITE
SV_ROD_SLEEP_MONSTER = objectsc.SV_ROD_SLEEP_MONSTER
SV_ROD_SLOW_MONSTER = objectsc.SV_ROD_SLOW_MONSTER
SV_ROD_DRAIN_LIFE = objectsc.SV_ROD_DRAIN_LIFE
SV_ROD_POLYMORPH = objectsc.SV_ROD_POLYMORPH
SV_ROD_ACID_BOLT = objectsc.SV_ROD_ACID_BOLT
SV_ROD_ELEC_BOLT = objectsc.SV_ROD_ELEC_BOLT
SV_ROD_FIRE_BOLT = objectsc.SV_ROD_FIRE_BOLT
SV_ROD_COLD_BOLT = objectsc.SV_ROD_COLD_BOLT
SV_ROD_ACID_BALL = objectsc.SV_ROD_ACID_BALL
SV_ROD_ELEC_BALL = objectsc.SV_ROD_ELEC_BALL
SV_ROD_FIRE_BALL = objectsc.SV_ROD_FIRE_BALL
SV_ROD_COLD_BALL = objectsc.SV_ROD_COLD_BALL
SV_ROD_HAVOC = objectsc.SV_ROD_HAVOC
SV_SCROLL_DARKNESS = objectsc.SV_SCROLL_DARKNESS
SV_SCROLL_AGGRAVATE_MONSTER = objectsc.SV_SCROLL_AGGRAVATE_MONSTER
SV_SCROLL_CURSE_ARMOR = objectsc.SV_SCROLL_CURSE_ARMOR
SV_SCROLL_CURSE_WEAPON = objectsc.SV_SCROLL_CURSE_WEAPON
SV_SCROLL_SUMMON_MONSTER = objectsc.SV_SCROLL_SUMMON_MONSTER
SV_SCROLL_SUMMON_UNDEAD = objectsc.SV_SCROLL_SUMMON_UNDEAD
SV_SCROLL_TRAP_CREATION = objectsc.SV_SCROLL_TRAP_CREATION
SV_SCROLL_PHASE_DOOR = objectsc.SV_SCROLL_PHASE_DOOR
SV_SCROLL_TELEPORT = objectsc.SV_SCROLL_TELEPORT
SV_SCROLL_TELEPORT_LEVEL = objectsc.SV_SCROLL_TELEPORT_LEVEL
SV_SCROLL_WORD_OF_RECALL = objectsc.SV_SCROLL_WORD_OF_RECALL
SV_SCROLL_IDENTIFY = objectsc.SV_SCROLL_IDENTIFY
SV_SCROLL_STAR_IDENTIFY = objectsc.SV_SCROLL_STAR_IDENTIFY
SV_SCROLL_REMOVE_CURSE = objectsc.SV_SCROLL_REMOVE_CURSE
SV_SCROLL_STAR_REMOVE_CURSE = objectsc.SV_SCROLL_STAR_REMOVE_CURSE
SV_SCROLL_ENCHANT_ARMOR = objectsc.SV_SCROLL_ENCHANT_ARMOR
SV_SCROLL_ENCHANT_WEAPON_TO_HIT = objectsc.SV_SCROLL_ENCHANT_WEAPON_TO_HIT
SV_SCROLL_ENCHANT_WEAPON_TO_DAM = objectsc.SV_SCROLL_ENCHANT_WEAPON_TO_DAM
SV_SCROLL_STAR_ENCHANT_ARMOR = objectsc.SV_SCROLL_STAR_ENCHANT_ARMOR
SV_SCROLL_STAR_ENCHANT_WEAPON = objectsc.SV_SCROLL_STAR_ENCHANT_WEAPON
SV_SCROLL_RECHARGING = objectsc.SV_SCROLL_RECHARGING
SV_SCROLL_LIGHT = objectsc.SV_SCROLL_LIGHT
SV_SCROLL_MAPPING = objectsc.SV_SCROLL_MAPPING
SV_SCROLL_DETECT_GOLD = objectsc.SV_SCROLL_DETECT_GOLD
SV_SCROLL_DETECT_ITEM = objectsc.SV_SCROLL_DETECT_ITEM
SV_SCROLL_DETECT_TRAP = objectsc.SV_SCROLL_DETECT_TRAP
SV_SCROLL_DETECT_DOOR = objectsc.SV_SCROLL_DETECT_DOOR
SV_SCROLL_DETECT_INVIS = objectsc.SV_SCROLL_DETECT_INVIS
SV_SCROLL_SATISFY_HUNGER = objectsc.SV_SCROLL_SATISFY_HUNGER
SV_SCROLL_BLESSING = objectsc.SV_SCROLL_BLESSING
SV_SCROLL_HOLY_CHANT = objectsc.SV_SCROLL_HOLY_CHANT
SV_SCROLL_HOLY_PRAYER = objectsc.SV_SCROLL_HOLY_PRAYER
SV_SCROLL_MONSTER_CONFUSION = objectsc.SV_SCROLL_MONSTER_CONFUSION
SV_SCROLL_PROTECTION_FROM_EVIL = objectsc.SV_SCROLL_PROTECTION_FROM_EVIL
SV_SCROLL_RUNE_OF_PROTECTION = objectsc.SV_SCROLL_RUNE_OF_PROTECTION
SV_SCROLL_TRAP_DOOR_DESTRUCTION = objectsc.SV_SCROLL_TRAP_DOOR_DESTRUCTION
SV_SCROLL_STAR_DESTRUCTION = objectsc.SV_SCROLL_STAR_DESTRUCTION
SV_SCROLL_DISPEL_UNDEAD = objectsc.SV_SCROLL_DISPEL_UNDEAD
SV_SCROLL_GENOCIDE = objectsc.SV_SCROLL_GENOCIDE
SV_SCROLL_MASS_GENOCIDE = objectsc.SV_SCROLL_MASS_GENOCIDE
SV_SCROLL_ACQUIREMENT = objectsc.SV_SCROLL_ACQUIREMENT
SV_SCROLL_STAR_ACQUIREMENT = objectsc.SV_SCROLL_STAR_ACQUIREMENT
SV_SCROLL_FIRE = objectsc.SV_SCROLL_FIRE
SV_SCROLL_ICE = objectsc.SV_SCROLL_ICE
SV_SCROLL_CHAOS = objectsc.SV_SCROLL_CHAOS
SV_SCROLL_RUMOR = objectsc.SV_SCROLL_RUMOR
SV_SCROLL_ARTIFACT = objectsc.SV_SCROLL_ARTIFACT
SV_POTION_WATER = objectsc.SV_POTION_WATER
SV_POTION_APPLE_JUICE = objectsc.SV_POTION_APPLE_JUICE
SV_POTION_SLIME_MOLD = objectsc.SV_POTION_SLIME_MOLD
SV_POTION_SLOWNESS = objectsc.SV_POTION_SLOWNESS
SV_POTION_SALT_WATER = objectsc.SV_POTION_SALT_WATER
SV_POTION_POISON = objectsc.SV_POTION_POISON
SV_POTION_BLINDNESS = objectsc.SV_POTION_BLINDNESS
SV_POTION_CONFUSION = objectsc.SV_POTION_CONFUSION
SV_POTION_SLEEP = objectsc.SV_POTION_SLEEP
SV_POTION_LOSE_MEMORIES = objectsc.SV_POTION_LOSE_MEMORIES
SV_POTION_RUINATION = objectsc.SV_POTION_RUINATION
SV_POTION_DEC_STR = objectsc.SV_POTION_DEC_STR
SV_POTION_DEC_INT = objectsc.SV_POTION_DEC_INT
SV_POTION_DEC_WIS = objectsc.SV_POTION_DEC_WIS
SV_POTION_DEC_DEX = objectsc.SV_POTION_DEC_DEX
SV_POTION_DEC_CON = objectsc.SV_POTION_DEC_CON
SV_POTION_DEC_CHR = objectsc.SV_POTION_DEC_CHR
SV_POTION_DETONATIONS = objectsc.SV_POTION_DETONATIONS
SV_POTION_DEATH = objectsc.SV_POTION_DEATH
SV_POTION_INFRAVISION = objectsc.SV_POTION_INFRAVISION
SV_POTION_DETECT_INVIS = objectsc.SV_POTION_DETECT_INVIS
SV_POTION_SLOW_POISON = objectsc.SV_POTION_SLOW_POISON
SV_POTION_CURE_POISON = objectsc.SV_POTION_CURE_POISON
SV_POTION_BOLDNESS = objectsc.SV_POTION_BOLDNESS
SV_POTION_SPEED = objectsc.SV_POTION_SPEED
SV_POTION_RESIST_HEAT = objectsc.SV_POTION_RESIST_HEAT
SV_POTION_RESIST_COLD = objectsc.SV_POTION_RESIST_COLD
SV_POTION_HEROISM = objectsc.SV_POTION_HEROISM
SV_POTION_BESERK_STRENGTH = objectsc.SV_POTION_BESERK_STRENGTH
SV_POTION_CURE_LIGHT = objectsc.SV_POTION_CURE_LIGHT
SV_POTION_CURE_SERIOUS = objectsc.SV_POTION_CURE_SERIOUS
SV_POTION_CURE_CRITICAL = objectsc.SV_POTION_CURE_CRITICAL
SV_POTION_HEALING = objectsc.SV_POTION_HEALING
SV_POTION_STAR_HEALING = objectsc.SV_POTION_STAR_HEALING
SV_POTION_LIFE = objectsc.SV_POTION_LIFE
SV_POTION_RESTORE_MANA = objectsc.SV_POTION_RESTORE_MANA
SV_POTION_RESTORE_EXP = objectsc.SV_POTION_RESTORE_EXP
SV_POTION_RES_STR = objectsc.SV_POTION_RES_STR
SV_POTION_RES_INT = objectsc.SV_POTION_RES_INT
SV_POTION_RES_WIS = objectsc.SV_POTION_RES_WIS
SV_POTION_RES_DEX = objectsc.SV_POTION_RES_DEX
SV_POTION_RES_CON = objectsc.SV_POTION_RES_CON
SV_POTION_RES_CHR = objectsc.SV_POTION_RES_CHR
SV_POTION_INC_STR = objectsc.SV_POTION_INC_STR
SV_POTION_INC_INT = objectsc.SV_POTION_INC_INT
SV_POTION_INC_WIS = objectsc.SV_POTION_INC_WIS
SV_POTION_INC_DEX = objectsc.SV_POTION_INC_DEX
SV_POTION_INC_CON = objectsc.SV_POTION_INC_CON
SV_POTION_INC_CHR = objectsc.SV_POTION_INC_CHR
SV_POTION_AUGMENTATION = objectsc.SV_POTION_AUGMENTATION
SV_POTION_ENLIGHTENMENT = objectsc.SV_POTION_ENLIGHTENMENT
SV_POTION_STAR_ENLIGHTENMENT = objectsc.SV_POTION_STAR_ENLIGHTENMENT
SV_POTION_SELF_KNOWLEDGE = objectsc.SV_POTION_SELF_KNOWLEDGE
SV_POTION_EXPERIENCE = objectsc.SV_POTION_EXPERIENCE
SV_POTION_RESISTANCE = objectsc.SV_POTION_RESISTANCE
SV_POTION_CURING = objectsc.SV_POTION_CURING
SV_POTION_INVULNERABILITY = objectsc.SV_POTION_INVULNERABILITY
SV_POTION_NEW_LIFE = objectsc.SV_POTION_NEW_LIFE
SV_FOOD_POISON = objectsc.SV_FOOD_POISON
SV_FOOD_BLINDNESS = objectsc.SV_FOOD_BLINDNESS
SV_FOOD_PARANOIA = objectsc.SV_FOOD_PARANOIA
SV_FOOD_CONFUSION = objectsc.SV_FOOD_CONFUSION
SV_FOOD_HALLUCINATION = objectsc.SV_FOOD_HALLUCINATION
SV_FOOD_PARALYSIS = objectsc.SV_FOOD_PARALYSIS
SV_FOOD_WEAKNESS = objectsc.SV_FOOD_WEAKNESS
SV_FOOD_SICKNESS = objectsc.SV_FOOD_SICKNESS
SV_FOOD_STUPIDITY = objectsc.SV_FOOD_STUPIDITY
SV_FOOD_NAIVETY = objectsc.SV_FOOD_NAIVETY
SV_FOOD_UNHEALTH = objectsc.SV_FOOD_UNHEALTH
SV_FOOD_DISEASE = objectsc.SV_FOOD_DISEASE
SV_FOOD_CURE_POISON = objectsc.SV_FOOD_CURE_POISON
SV_FOOD_CURE_BLINDNESS = objectsc.SV_FOOD_CURE_BLINDNESS
SV_FOOD_CURE_PARANOIA = objectsc.SV_FOOD_CURE_PARANOIA
SV_FOOD_CURE_CONFUSION = objectsc.SV_FOOD_CURE_CONFUSION
SV_FOOD_CURE_SERIOUS = objectsc.SV_FOOD_CURE_SERIOUS
SV_FOOD_RESTORE_STR = objectsc.SV_FOOD_RESTORE_STR
SV_FOOD_RESTORE_CON = objectsc.SV_FOOD_RESTORE_CON
SV_FOOD_RESTORING = objectsc.SV_FOOD_RESTORING
SV_FOOD_BISCUIT = objectsc.SV_FOOD_BISCUIT
SV_FOOD_JERKY = objectsc.SV_FOOD_JERKY
SV_FOOD_RATION = objectsc.SV_FOOD_RATION
SV_FOOD_SLIME_MOLD = objectsc.SV_FOOD_SLIME_MOLD
SV_FOOD_WAYBREAD = objectsc.SV_FOOD_WAYBREAD
SV_FOOD_PINT_OF_ALE = objectsc.SV_FOOD_PINT_OF_ALE
SV_FOOD_PINT_OF_WINE = objectsc.SV_FOOD_PINT_OF_WINE
SV_FOOD_MIN_FOOD = objectsc.SV_FOOD_MIN_FOOD
SV_ROD_MIN_DIRECTION = objectsc.SV_ROD_MIN_DIRECTION
SV_CHEST_MIN_LARGE = objectsc.SV_CHEST_MIN_LARGE
SV_BOOK_MIN_GOOD = objectsc.SV_BOOK_MIN_GOOD
OBJ_GOLD_LIST = objectsc.OBJ_GOLD_LIST
MAX_GOLD = objectsc.MAX_GOLD
IDENT_SENSE = objectsc.IDENT_SENSE
IDENT_FIXED = objectsc.IDENT_FIXED
IDENT_EMPTY = objectsc.IDENT_EMPTY
IDENT_KNOWN = objectsc.IDENT_KNOWN
IDENT_STOREB = objectsc.IDENT_STOREB
IDENT_MENTAL = objectsc.IDENT_MENTAL
IDENT_CURSED = objectsc.IDENT_CURSED
IDENT_BROKEN = objectsc.IDENT_BROKEN
