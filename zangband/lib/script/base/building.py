#####################################################################
#
# base/building.py
#
# Building code
#
#####################################################################

from variable import events, debug

from base.feature import room

#####################################################################
# Base class for buildings
#####################################################################
class building(room):
	# Name of the building
	name = "Empty building"

	# Name of the owner
	owner_name = "Nobody"

	# Race of the owner
	owner_race = "None"

	# Entry to the building (door)
	entry = (1, 1)

	commands = []

	def __init__(self, level):
		debug.trace("building.__init__(%s, %s)" % (self, level))

		room.__init__(self, level)

		self.data = []

		for command in self.commands:
			self.data.append(command(self))

		# Redirect the player_enter event
		self.level.actions[self.entry] = (self, "player_enter", ())

	# Enter the building
	def player_enter(self):
		debug.trace("building.player_enter(%s)" % (self))

		from angband import io
		io.screen_save()
		io.Term_clear()
#		try:
		io.prt(("%s (%s) %35s" % (self.owner_name, self.owner_race, self.name)), 2, 1)
		# Print available commands
		i = 0
		for command in self.data:
			cost = command.cost()
			if not cost:
				cost = ""
			io.prt((" %c) %s %s" % (command.letter, command.name, cost)), 19+(i/2), 35*(i%2))
			i = i + 1
		io.prt(" ESC) Exit building", 23, 0)
		# Request command
		letter = io.inkey()
		# Execute the command
		for command in self.data:
			if command.letter == letter:
				command.execute(self)
#		finally:
		# Leave the building
		self.leave()
		return 1

	# Leave the building
	def leave(self):
		debug.trace("building.leave(%s)" % (self))

		from angband import io
		io.screen_load()


#####################################################################
# Base class for a command in a building
#####################################################################
class building_command:
	# Name of the command
	name = ""

	# Letter
	letter = ""

	# Price of the command
	price = 0

	# Create the command
	def __init__(self, building):
		debug.trace("building_command.__init__(%s, %s)" % (self, building))

	# Cost of executing the command
	def cost(self):
		return self.price

#####################################################################
# Command for quest assignment
#####################################################################
class quest_command(building_command):
	name = "Get quest"
	letter = "q"

	# Create the command
	def __init__(self, building):
		debug.trace("quest_command.__init__(%s, %s)" % (self, building))

		building_command.__init__(self, building)

		# Create the plot for the building
		import quest
		building.plot = quest.plot()

	# Execute the command
	def execute(self, building):
		debug.trace("quest_command.__init__(%s, %s)" % (self, building))

		from variable import gui

		# Check the players quests for a quest from this building
		from variable import player
		for quest in player.quests.values():
			if quest.quest_giver == building.quest_giver:
				if quest.unfinished():
					quest.execute()
					gui.print_quest_info(quest.get_description())
					return

		# Get a new quest
		quest = building.plot.execute()

		if quest:
			# Assign the quest to the player
			quest.assign(building.quest_giver)

			# Print the description
			gui.print_quest_info(quest.get_description())
		else:
			gui.print_quest_info(["I don't have a quest for you at the moment."])

