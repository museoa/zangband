#####################################################################
#
# base/player.py
#
# Generic code for the player
#
#####################################################################

from angband.player import *
from variable import events, debug


#####################################################################
# Base class for the player
#####################################################################
class player_data_class(player_typePtr):
	def __init__(self):
		player_typePtr.__init__(self, cvar.p_ptr)

		debug.trace("player_data_class.__init__(%s)" % (self))

		# Init the quests
		import base.quest
		self.quests = base.quest.quests_data()

		# Loading/saving
		events.load_game.append(self)
		events.save_game.append(self)

		# Create the support for the player race and class
		import birth
		birth.player_sex_birth()
		birth.player_race_birth()
		birth.player_class_birth()

		import base.skill
		self.skills = base.skill.skills()

	def give(self, object):
		debug.trace("player_data_class.give(%s, %s)" % (self, object))

		object.object_aware()
		object.object_known()
		self.carry(object.this)

	def load_game_hook(self, dict):
		self.__dict__ = dict["player"]

	def save_game_hook(self):
		return ("player", self.__dict__)

