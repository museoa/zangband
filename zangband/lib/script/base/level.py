#####################################################################
#
# base/level.py
#
# Levels, dungeon-levels, towns, and wilderness areas
#
#####################################################################

from variable import events, debug

#####################################################################
#
# Base class for a level
#
#####################################################################
class level:
	# Name of the level
	name = ""

	# Map-file of the level
	map_file = None

	# Starting position of the player
	entry = (1, 1)

	# Constructor
	def __init__(self):
		debug.trace("level.__init__(%s)" % (self))

		# Version number of the class
		self.version = 0

		# Map
		# XXX - grant access to the map of the level

		# Objects (items, monsters)
		# Contains a list of all monsters, items, traps, ... on this level
		self.objects = []

		# Scripted actions on this level
		# Keys are coordinates
		# Value is a tuple of object, function-name, arguments
		self.actions = {}

		# Features like buildings, shops, or special rooms
		# Keys are names of the features (ex. "weapon store")
		self.features = {}

	# Create the level
	def generate(self):
		debug.trace("level.generate(%s)" % (self))

	# Load a level from a map-file
	def load_map(self):
		debug.trace("level.load_map(%s)" % (self))

		# Load the map
		if self.map_file:
			from angband import cave
			cave.load_layout(self.map_file)

	# Player enters the level
	def player_enter(self):
		debug.trace("level.player_enter(%s)" % (self))

		# XXX - Init the quests
		from util.autorun import autorun_dir
		from variable import player
		autorun_dir("worlds", player.world.directory, "startup", "quests")

		# Build the features
		for feature in self.features.values():
			feature.build()

		# Create the actions
		for coords in self.actions.keys():
			object, function, data = self.actions[coords]
			self.actions[coords] = (object, function, data)

		# Place the player
		from variable import player
		if player.oldpx == 0 or player.oldpy == 0:
			player.py, player.px = self.entry

		events.player_move.append(self)

	# Player leaves the level
	def player_leave(self):
		debug.trace("level.player_leave(%s)" % (self))

		events.player_move.remove(self)

		return 1

	# Player moves to new grid
	def player_move_hook(self, coords):
		debug.trace("level.player_move(%s, %s)" % (self, coords))

		if self.actions.has_key(coords):
			action, method, args = self.actions[coords]
			function = getattr(action, method)
			return apply(function, args)


#####################################################################
#
# Base class for a dungeon-level
#
#####################################################################
class dungeon_level(level):
	# Return level feeling
	def feeling(self):
		debug.trace("dungeon_level.feeling(%s)" % (self))


#####################################################################
#
# Base class for an object
#
#####################################################################
class object:
	pass


#####################################################################
#
# Base class for an item
#
#####################################################################
class item(object):
	pass


#####################################################################
#
# Base class for an ego-item
#
#####################################################################
class ego_item(item):
	pass


#####################################################################
#
# Base class for an artifact
#
#####################################################################
class artifact(ego_item):
	pass


#####################################################################
#
# Base class for all monsters and players
#
#####################################################################
class being(object):
	pass


#####################################################################
#
# Base class for a monster
#
#####################################################################
class monster(being):
	pass


#####################################################################
#
# Base class for the player
#
#####################################################################
class player(being):
	pass


#####################################################################
#
# Base class for a player-race
#
#####################################################################
class player_race:
	pass


#####################################################################
#
# Base class for a player-class
#
#####################################################################
class player_class:
	pass


#####################################################################
#
# Base class for a player-gender
#
#####################################################################
class player_gender:
	pass


#####################################################################
#
# Base class for a spell
#
#####################################################################
class spell:
	pass


#####################################################################
#
# Base class for a trap
#
#####################################################################
class trap(object):
	# Create a trap
	def create(self, y, x):
		pass

	# Step on (spring) a trap
	def spring(self):
		pass

	# Disarm the trap
	def disarm(self):
		pass


#####################################################################
#
# Container for "level" instances.
#
# ABSTRACT!
#
#####################################################################
class levels:
	def __init__(self):
		self.labels = {}

	# Get the corresponding level
	def get_level(self, name):
		return self.levels[self.labels[name]]

	# Enter a "levels" object
	# Places the player at a default entry point
	def enter(self):
		debug.trace("levels.enter(%s)" % (self))

	def leave(self):
		debug.trace("levels.leave(%s)" % (self))


from base.wild import *

#####################################################################
#
# Container for the surface of a world.
#
#####################################################################
class wilderness(levels):
	known_wild_classes = {
		"#" : edge,
		"~" : ocean,
		"=" : shore,
		"_" : grassland }

	# Entry point for the wilderness
	entry = (0, 0)

	def __init__(self):
		debug.trace("wilderness.__init__(%s)" % (self))

		self.data = {}

		events.wilderness_init.append(self)

	# Load the map of the wilderness
	def load_map(self):
		debug.trace("wilderness.load_map(%s)" % (self))

		try:
			# Open the wilderness file
			import os.path
			from angband import system
			filename = os.path.join(system.cvar.ANGBAND_DIR_EDIT, "wild.map")
			wild_map = open(filename, 'r')
		except:
			import traceback
			traceback.print_exc()

		y1 = 0
		for line in wild_map.readlines():
			x1 = 0
			for letter in line:
				if letter != "\n":
					if self.known_wild_classes.has_key(letter):
						self.data[(y1, x1)] = self.known_wild_classes[letter]()
					else:
						# Unknown wilderness letter - place grassland
						print "Unknown wilderness type '%s'" % (letter)
						self.data[(y1, x1)] = grassland()
					x1 = x1 + 1
			y1 = y1 + 1

	# Initialize the wilderness
	def wilderness_init_hook(self, data):
		debug.trace("wilderness.wilderness_init_hook(%s, %s)" % (self, data))

		self.load_map()

		self.enter()

	# Generate a wilderness area
	def generate_wilderness_hook(self, coords):
		debug.trace("wilderness.generate_wilderness_hook(%s, %s)" % (self, coords))

		return self.data[coords].generate()

	# Enter a wilderness area
	def enter_wilderness_hook(self, coords):
		debug.trace("wilderness.enter_wilderness_hook(%s, %s)" % (self, coords))

		return self.data[coords].player_enter()

	# Leave a wilderness area
	def leave_wilderness_hook(self, coords):
		debug.trace("wilderness.leave_wilderness_hook(%s, %s)" % (self, coords))

		return self.data[coords].player_leave()

	# Enter the wilderness
	# Places the player at the entry-point
	def enter(self):
		debug.trace("wilderness.enter(%s)" % (self))

		# Hook into the wilderness events
		events.generate_wilderness.append(self)
		events.enter_wilderness.append(self)
		events.leave_wilderness.append(self)

		# XXX - Place the player
		from variable import player
		player.wilderness_y, player.wilderness_x = self.entry

	# Leave the wilderness
	def leave(self):
		debug.trace("wilderness.leave(%s)" % (self))

		# Remove the hooks
		events.generate_wilderness.remove(self)
		events.enter_wilderness.remove(self)
		events.leave_wilderness.remove(self)

	def __len__(self):
		return len(self.data)

	def __getitem__(self, key):
		return self.data[key]

	def __setitem__(self, key, value):
		self.data[key] = value

	def __delitem__(self, key):
		del self.data[key]


#####################################################################
#
# dungeon
#
#####################################################################
class dungeon(levels):
	def __init__(self):
		# Internal storage for the levels
		self.data = []

		# Entry level for the dungeon
		self.entry_level = 0

	# Enter the dungeon
	# Places the player on the entry-level
	def enter(self):
		# XXX - Place the player
		pass

	def __len__(self):
		return len(self.data)

	def __getitem__(self, key):
		return self.data[key]

	def __setitem__(self, key, value):
		self.data[key] = value

	def __delitem__(self, key):
		del self.data[key]

