"""Generic code for the player"""

from angband.player import *


class player(player_type):
	"""Base class for the player"""

	def __init__(self):
		player_type.__init__(self, cvar.p_ptr)
		self.thisown = 0

		# Init the quests
		import base.quest
		self.quests = base.quest.quests_data()

		# Init the skills
		import base.skill
		self.skills = base.skill.skills()

		# Init the events
		from vars import events
		events.player_birth.append(self)
		events.load_game.append(self)
		events.save_game.append(self)

	# New game
	def player_birth_hook(self, data):
		# Get the world
		from vars import worlds
		world = worlds.get_world()
		if not world: return -1

		# Get the player race
		self.race = world.get_player_race()
		if not self.race: return -1
		self.prace = self.race.number

		# Get the player gender
		self.sex = world.get_player_gender()
		if not self.sex: return -1
		self.psex = self.sex.number

		# Get the player class
		self.p_class = world.get_player_class()
		if not self.p_class: return -1
		self.pclass = self.p_class.number

		# XXX !!! MEGA-HACK !!!
		import realmsc
		realmsc.cvar.mp_ptr = realmsc.get_player_magic(self.pclass)

	# Saving the game
	def save_game_hook(self):
		return ("player", self.__dict__)

	# Loading the game
	def load_game_hook(self, dict):
		# Load the data
		self.__dict__.update(dict["player"])

	def give(self, object):
		object.object_aware()
		object.object_known()
		self.carry(object.this)

