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

	# Save the object
	def object_save_hook(self):
		return pickle.dumps(self)

	# Delete the object
	def object_delete_hook(self):
		from vars import objects
		objects.data.remove(self)

	# Create a copy of the object
	def object_copy_hook(self, j_ptr):
		# Create a (deep) copy of ourself
		import copy
		target = copy.deepcopy(self)
		# Mega-Hack ! - Redirect the base object
		target.this = j_ptr

	def get_object_name_hook(self):
		return self.name


#####################################################################
# Object storage class
#####################################################################
class object_data_class:
	def __init__(self):
		self.version = 0
		self.data = []
		self.classes = {}
		from vars import events
		# We want to know when Angband creates a new object
		events.object_create.append(self)
		# And when it loads an object
		events.object_load.append(self)

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

	# Called when Angband creates a new object
	def object_create_hook(self, o_ptr):
		# Get tval and sval of the new object
		the_object = object_typePtr(o_ptr)
		tval = the_object.tval
		sval = the_object.sval

		# Create the corresponding Python object
		if self.classes.has_key((tval, sval)):
			# Init a specific item type
			the_object.__class__ = self.classes[(tval, sval)]
		elif self.classes.has_key(tval):
			# Init an item sub-type
			the_object.__class__ = self.classes[tval]
		else:
			# Init a generic item
			# (disabled atm because generic items are boring)
			the_object = None

		# Store the object
		self.data.append(the_object)

		# Return the object
		return the_object

	def object_load_hook(self, data):
		the_object = pickle.loads(data)
		self.data.append(the_object)
		return the_object

