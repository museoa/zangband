#####################################################################
#
# base/world.py
#
# Initialization of the worlds
#
#####################################################################

from variable import events, debug


#####################################################################
#
# Base class for a world.
# Contains a "wilderness" as well as the available monsters/items
# ("object").
#
# ABSTRACT CLASS!
#
#####################################################################
class world:
	# Name of the world
	name = ""

	# Directory that stores the world definition
	# (sub-directory of "script/world")
	directory = ""

	# Entry-points into the world
	# Each world must at least define a "default" entry-point
	entry = {}

	# Labels
	labels = {}

	# Constructor
	def __init__(self):
		debug.trace("world.__init__(%s)" % (self))

		# Levels of the world
		self.levels = {}

		# Run the auto-start scripts
		from util.autorun import autorun_dir
		autorun_dir("worlds", self.directory, "startup")

	# Get the corresponding level
	def get_level(self, name):
		return self.levels[self.labels[name]]



#####################################################################
#
# World selection at character birth
#
#####################################################################
class world_data:
	def __init__(self):
		debug.trace("world_data.__init__(%s)" % (self))

		# Registered world-classes
		self.data = []

		# Initialized worlds
		self.worlds = {}

		# Register the birth event
		events.get_world.append(self)

	# Register a new worlds
	def register(self, world):
		debug.trace("world_data.register(%s, %s)" % (self, world))

		self.data.append(world)

	def get_world_hook(self, args):
		debug.trace("world_data.get_world_hook(%s, %s)" % (self, args))

		# Select the world
		from variable import ui
		selected = ui.birth.select_world(self.data)

		# Restart character generation
		if not selected:
			return -1

		# Initialize the world
		world = apply(selected, ())

		self.worlds[world.name] = world

		from variable import player
		player.world = world

		return 1

	# Pretend to be a dictionary
	def __len__(self):
		return len(self.worlds)

	def __getitem__(self, key):
		return self.worlds[key]

	def __setitem__(self, key, value):
		self.worlds[key] = value

	def __delitem__(self, key):
		del self.worlds[key]

