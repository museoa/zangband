# This file was created automatically by SWIG.
import terrainc
import new
class feature_type:
    def __init__(self,*args):
        self.this = apply(terrainc.new_feature_type,args)
        self.thisown = 1

    def __del__(self,terrainc=terrainc):
        if self.thisown == 1 :
            terrainc.delete_feature_type(self)
    __setmethods__ = {
        "name" : terrainc.feature_type_name_set,
        "text" : terrainc.feature_type_text_set,
        "mimic" : terrainc.feature_type_mimic_set,
        "extra" : terrainc.feature_type_extra_set,
        "unused" : terrainc.feature_type_unused_set,
        "d_attr" : terrainc.feature_type_d_attr_set,
        "d_char" : terrainc.feature_type_d_char_set,
        "x_attr" : terrainc.feature_type_x_attr_set,
        "x_char" : terrainc.feature_type_x_char_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = feature_type.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "name" : terrainc.feature_type_name_get,
        "text" : terrainc.feature_type_text_get,
        "mimic" : terrainc.feature_type_mimic_get,
        "extra" : terrainc.feature_type_extra_get,
        "unused" : terrainc.feature_type_unused_get,
        "d_attr" : terrainc.feature_type_d_attr_get,
        "d_char" : terrainc.feature_type_d_char_get,
        "x_attr" : terrainc.feature_type_x_attr_get,
        "x_char" : terrainc.feature_type_x_char_get,
    }
    def __getattr__(self,name):
        method = feature_type.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C feature_type instance at %s>" % (self.this,)
class feature_typePtr(feature_type):
    def __init__(self,this):
        self.this = this
        self.thisown = 0




#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

