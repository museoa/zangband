#####################################################################
#
# worlds/nexus/wild/town1.py
#
# Town 1
#
#####################################################################

from variable import events, debug
from base.building import building, quest_command

#####################################################################
# The mayors house
#####################################################################
class mayor_building(building):
	# Name of the building
	name = "Mayor"

	# Name of the owner
	owner_name = "Uldrik"

	# Race of the owner
	owner_race = "Human"

	# Entry to the building (door)
	entry = (26, 98)

	# Name of the quest-giver
	quest_giver = "Uldrik the mayor of town 1"

	# Commands of the building
	commands = [quest_command]


from base.wild import town

#####################################################################
# Town 1
#####################################################################
class town1(town):
	name = "Outpost"

	map_file = "town1.map"

	entry = (27, 98)

	labels = {"Thieves Den" : (35, 177)}

	def __init__(self):
		debug.trace("town1.__init__(%s)" % (self))

		# Inherited
		town.__init__(self)

		from base.feature import quest_entrance
		self.features = { "Mayor" : mayor_building(self) }

