#####################################################################
#
# worlds/nexus/__init__.py
#
# Initialization of the world "Nexus"
#
#####################################################################

from variable import debug
from base.level import wilderness

class nexus_wilderness(wilderness):
	def __init__(self):
		debug.trace("nexus_wilderness.__init__(%s)" % (self))

		# Inherited
		wilderness.__init__(self)

		# Entry point
		self.entry = (4, 4)

		# Make town 1 known
		from wild.town1 import town1
		self.known_wild_classes["1"] = town1


from base.world import world

class nexus(world):
	name = "Nexus"

	directory = "nexus"

	labels = { "Town1" : (4, 4) }

	def __init__(self):
		debug.trace("nexus.__init__(%s)" % (self))

		# Inherited
		world.__init__(self)

		self.levels = nexus_wilderness()

