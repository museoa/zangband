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
class player_data_class(player_type):
	def __init__(self):
		player_type.__init__(self, cvar.p_ptr)
		self.thisown = 0

		debug.trace("player_data_class.__init__(%s)" % (self))

		# Init the quests
		import base.quest
		self.quests = base.quest.quests_data()

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

