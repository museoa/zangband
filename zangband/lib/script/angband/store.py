# This file was created automatically by SWIG.
import storec
class owner_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "owner_name" :
            storec.owner_type_owner_name_set(self,value)
            return
        if name == "max_cost" :
            storec.owner_type_max_cost_set(self,value)
            return
        if name == "max_inflate" :
            storec.owner_type_max_inflate_set(self,value)
            return
        if name == "min_inflate" :
            storec.owner_type_min_inflate_set(self,value)
            return
        if name == "haggle_per" :
            storec.owner_type_haggle_per_set(self,value)
            return
        if name == "insult_max" :
            storec.owner_type_insult_max_set(self,value)
            return
        if name == "owner_race" :
            storec.owner_type_owner_race_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "owner_name" : 
            return storec.owner_type_owner_name_get(self)
        if name == "max_cost" : 
            return storec.owner_type_max_cost_get(self)
        if name == "max_inflate" : 
            return storec.owner_type_max_inflate_get(self)
        if name == "min_inflate" : 
            return storec.owner_type_min_inflate_get(self)
        if name == "haggle_per" : 
            return storec.owner_type_haggle_per_get(self)
        if name == "insult_max" : 
            return storec.owner_type_insult_max_get(self)
        if name == "owner_race" : 
            return storec.owner_type_owner_race_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C owner_type instance at %s>" % (self.this,)
class owner_type(owner_typePtr):
    def __init__(self,this):
        self.this = this




class store_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "type" :
            storec.store_type_type_set(self,value)
            return
        if name == "owner" :
            storec.store_type_owner_set(self,value)
            return
        if name == "extra" :
            storec.store_type_extra_set(self,value)
            return
        if name == "insult_cur" :
            storec.store_type_insult_cur_set(self,value)
            return
        if name == "good_buy" :
            storec.store_type_good_buy_set(self,value)
            return
        if name == "bad_buy" :
            storec.store_type_bad_buy_set(self,value)
            return
        if name == "store_open" :
            storec.store_type_store_open_set(self,value)
            return
        if name == "last_visit" :
            storec.store_type_last_visit_set(self,value)
            return
        if name == "table_num" :
            storec.store_type_table_num_set(self,value)
            return
        if name == "table_size" :
            storec.store_type_table_size_set(self,value)
            return
        if name == "table" :
            storec.store_type_table_set(self,value)
            return
        if name == "stock_num" :
            storec.store_type_stock_num_set(self,value)
            return
        if name == "stock_size" :
            storec.store_type_stock_size_set(self,value)
            return
        if name == "stock" :
            storec.store_type_stock_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "type" : 
            return storec.store_type_type_get(self)
        if name == "owner" : 
            return storec.store_type_owner_get(self)
        if name == "extra" : 
            return storec.store_type_extra_get(self)
        if name == "insult_cur" : 
            return storec.store_type_insult_cur_get(self)
        if name == "good_buy" : 
            return storec.store_type_good_buy_get(self)
        if name == "bad_buy" : 
            return storec.store_type_bad_buy_get(self)
        if name == "store_open" : 
            return storec.store_type_store_open_get(self)
        if name == "last_visit" : 
            return storec.store_type_last_visit_get(self)
        if name == "table_num" : 
            return storec.store_type_table_num_get(self)
        if name == "table_size" : 
            return storec.store_type_table_size_get(self)
        if name == "table" : 
            return storec.store_type_table_get(self)
        if name == "stock_num" : 
            return storec.store_type_stock_num_get(self)
        if name == "stock_size" : 
            return storec.store_type_stock_size_get(self)
        if name == "stock" : 
            return storec.store_type_stock_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C store_type instance at %s>" % (self.this,)
class store_type(store_typePtr):
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

