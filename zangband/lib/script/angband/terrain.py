# This file was created automatically by SWIG.
import terrainc
class feature_typePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,terrainc=terrainc):
        if self.thisown == 1 :
            terrainc.delete_feature_type(self)
    def __setattr__(self,name,value):
        if name == "name" :
            terrainc.feature_type_name_set(self,value)
            return
        if name == "text" :
            terrainc.feature_type_text_set(self,value)
            return
        if name == "mimic" :
            terrainc.feature_type_mimic_set(self,value)
            return
        if name == "extra" :
            terrainc.feature_type_extra_set(self,value)
            return
        if name == "unused" :
            terrainc.feature_type_unused_set(self,value)
            return
        if name == "d_attr" :
            terrainc.feature_type_d_attr_set(self,value)
            return
        if name == "d_char" :
            terrainc.feature_type_d_char_set(self,value)
            return
        if name == "x_attr" :
            terrainc.feature_type_x_attr_set(self,value)
            return
        if name == "x_char" :
            terrainc.feature_type_x_char_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "name" : 
            return terrainc.feature_type_name_get(self)
        if name == "text" : 
            return terrainc.feature_type_text_get(self)
        if name == "mimic" : 
            return terrainc.feature_type_mimic_get(self)
        if name == "extra" : 
            return terrainc.feature_type_extra_get(self)
        if name == "unused" : 
            return terrainc.feature_type_unused_get(self)
        if name == "d_attr" : 
            return terrainc.feature_type_d_attr_get(self)
        if name == "d_char" : 
            return terrainc.feature_type_d_char_get(self)
        if name == "x_attr" : 
            return terrainc.feature_type_x_attr_get(self)
        if name == "x_char" : 
            return terrainc.feature_type_x_char_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C feature_type instance at %s>" % (self.this,)
class feature_type(feature_typePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(terrainc.new_feature_type,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

