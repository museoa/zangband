#####################################################################
#
# base/feature.py
#
# Dungeon/town features like rooms, vaults, buildings, and shops
#
#####################################################################

from variable import events, debug


#####################################################################
#
# Base class for a feature
#
#####################################################################
class feature:
	def __init__(self, level):
		debug.trace("feature.__init__(%s, %s)" % (self, level))

		self.level = level

	# Build the feature on this level
	def build(self):
		debug.trace("feature.build(%s)" % (self))

	def player_enter(self):
		debug.trace("feature.player_enter(%s)" % (self))

	def player_leave(self):
		debug.trace("feature.player_leave(%s)" % (self))


#####################################################################
#
# Base class for a room
#
#####################################################################
class room(feature):
	pass


#####################################################################
#
# Base class for a vault
#
#####################################################################
class vault(room):
	pass


#####################################################################
#
# Base class for a pit
#
#####################################################################
class pit(room):
	pass


#####################################################################
#
# Base class for a shop
#
#####################################################################
class shop(room):
	pass

#####################################################################
#
# Entry to a quest
#
#####################################################################
class quest_entrance(feature):
	def __init__(self, level, label, quest):
		debug.trace("quest_entrance.__init__(%s, %s, %s, %s)" % (self, level, label, quest))

		# Add the feature to the correct place
		from variable import worlds
		self.level = level
		self.level.features[label] = self

		self.quest = quest
		self.label = label

	# Build the entrance
	def build(self):
		debug.trace("quest_entrance.build(%s)" % (self))

		entry = self.level.labels[self.label]

		# Add a player_enter event
		self.level.actions[entry] = (self, "player_enter", ())

		from angband.cave import set_feat
		y, x = entry
		set_feat(y, x, 8)

	# Enter a quest
	def player_enter(self):
		debug.trace("quest_entrance.player_enter(%s)" % (self))

		from variable import player
		# First add ourself to the event for level generation
		events.generate_level.append(self.quest)
		player.leaving = 1
		player.inside_quest = 1
		player.oldpx = player.px
		player.oldpy = player.py
		from angband.event import *
		return 1

