#####################################################################
#
# base/object.py
#
# Base code for all objects
#
#####################################################################

from angband.objects import object_type, object_typePtr

try:
	import cPickle
	pickle = cPickle
except ImportError:
	import pickle


#####################################################################
# Object base class
#####################################################################
class object_class(object_type):
	def __init__(self, o_ptr=None):
		if o_ptr:
			object_typePtr.__init__(self, o_ptr)
		else:
			object_type.__init__(self)


#####################################################################
# Object storage class
#####################################################################
class object_data_class:
	def __init__(self):
		self.version = 0
		self.classes = {}

	def announce(self, object_class):
		if hasattr(object_class, "sval"):
			self.classes[object_class.tval, object_class.sval] = object_class
		else:
			self.classes[object_class.tval] = object_class

	def create(self, name):
		for object in self.classes.values():
			if hasattr(object, "name") and (object.name == name):
				from angband.objects import lookup_kind
				the_object = object()
				the_object.object_prep(lookup_kind(object.tval, object.sval))
				return the_object
		raise "unknown object", name
