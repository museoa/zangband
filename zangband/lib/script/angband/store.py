# This file was created automatically by SWIG.
import storec
class owner_type:
    def __init__(self,this):
        self.this = this

    __setmethods__ = {
        "owner_name" : storec.owner_type_owner_name_set,
        "max_cost" : storec.owner_type_max_cost_set,
        "max_inflate" : storec.owner_type_max_inflate_set,
        "min_inflate" : storec.owner_type_min_inflate_set,
        "haggle_per" : storec.owner_type_haggle_per_set,
        "insult_max" : storec.owner_type_insult_max_set,
        "owner_race" : storec.owner_type_owner_race_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = owner_type.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "owner_name" : storec.owner_type_owner_name_get,
        "max_cost" : storec.owner_type_max_cost_get,
        "max_inflate" : storec.owner_type_max_inflate_get,
        "min_inflate" : storec.owner_type_min_inflate_get,
        "haggle_per" : storec.owner_type_haggle_per_get,
        "insult_max" : storec.owner_type_insult_max_get,
        "owner_race" : storec.owner_type_owner_race_get,
    }
    def __getattr__(self,name):
        method = owner_type.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C owner_type instance at %s>" % (self.this,)
class owner_typePtr(owner_type):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = owner_type



class store_type:
    def __init__(self,this):
        self.this = this

    __setmethods__ = {
        "type" : storec.store_type_type_set,
        "owner" : storec.store_type_owner_set,
        "extra" : storec.store_type_extra_set,
        "insult_cur" : storec.store_type_insult_cur_set,
        "good_buy" : storec.store_type_good_buy_set,
        "bad_buy" : storec.store_type_bad_buy_set,
        "store_open" : storec.store_type_store_open_set,
        "last_visit" : storec.store_type_last_visit_set,
        "table_num" : storec.store_type_table_num_set,
        "table_size" : storec.store_type_table_size_set,
        "table" : storec.store_type_table_set,
        "stock_num" : storec.store_type_stock_num_set,
        "stock_size" : storec.store_type_stock_size_set,
        "stock" : storec.store_type_stock_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = store_type.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "type" : storec.store_type_type_get,
        "owner" : storec.store_type_owner_get,
        "extra" : storec.store_type_extra_get,
        "insult_cur" : storec.store_type_insult_cur_get,
        "good_buy" : storec.store_type_good_buy_get,
        "bad_buy" : storec.store_type_bad_buy_get,
        "store_open" : storec.store_type_store_open_get,
        "last_visit" : storec.store_type_last_visit_get,
        "table_num" : storec.store_type_table_num_get,
        "table_size" : storec.store_type_table_size_get,
        "table" : storec.store_type_table_get,
        "stock_num" : storec.store_type_stock_num_get,
        "stock_size" : storec.store_type_stock_size_get,
        "stock" : storec.store_type_stock_get,
    }
    def __getattr__(self,name):
        method = store_type.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C store_type instance at %s>" % (self.this,)
class store_typePtr(store_type):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = store_type





#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

