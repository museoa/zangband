#####################################################################
#
# base/wild.py
#
# Generic code for the wilderness
#
#####################################################################

from variable import events, debug
from angband import cave
from base.level import level

#####################################################################
#
# Base class for a wilderness area
#
#####################################################################
class wilderness_grid(level):
	# Wilderness areas can have a name
	name = "Wilderness"


#####################################################################
# Wilderness area for the "Edge of the world"
#####################################################################
class edge(wilderness_grid):
	def generate(self):
		debug.trace("edge.generate(%s)" % (self))

		# Generate an area filled with permanent walls
		cave.generate_wilderness_area(0, 1, 0, 0)

		# Call the base-class
		wilderness_grid.generate(self)

		return 1


#####################################################################
# Wilderness area for the ocean
#####################################################################
class ocean(wilderness_grid):
	def generate(self):
		debug.trace("ocean.generate(%s)" % (self))

		# Generate an area filled with water
		cave.generate_wilderness_area(2, 1, 0, 0)

		# Call the base-class
		wilderness_grid.generate(self)

		return 1


#####################################################################
# Wilderness area for the shore
#####################################################################
class shore(wilderness_grid):
	def generate(self):
		debug.trace("shore.generate(%s)" % (self))

		# Generate an area filled with (shallow) water
		cave.generate_wilderness_area(3, 1, 0, 0)

		# Call the base-class
		wilderness_grid.generate(self)

		return 1


#####################################################################
# Wilderness area for grassland
#####################################################################
class grassland(wilderness_grid):
	def generate(self):
		debug.trace("grassland.generate(%s)" % (self))

		# Generate an area filled with grass
		cave.generate_wilderness_area(6, 1, 0, 0)

		# Call the base-class
		wilderness_grid.generate(self)

		return 1


#####################################################################
# Base class for a town (in the wilderness)
#####################################################################
class town(wilderness_grid):
	def generate(self):
		debug.trace("town.generate(%s)" % (self))

		# Load the map of the town
		self.load_map()

		# Call the base-class
		wilderness_grid.generate(self)

		return 1

	# XXX
	def quest_enter(self, name):
		from quest import quests
		from variable import player
		# First add ourself to the event for level generation
		events.generate_level.append(quests.quests[name])
		player.leaving = 1
		player.inside_quest = 1
		player.oldpx = player.px
		player.oldpy = player.py
		return 1

