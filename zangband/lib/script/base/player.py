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

		self.skills = []
		events.use_skill.append(self)

	def give(self, object):
		debug.trace("player_data_class.give(%s, %s)" % (self, object))

		object.object_aware()
		object.object_known()
		self.carry(object.this)

	def use_skill_hook(self, args):
		from angband import io
		from util.string import I2A, A2I
		import string

		skills = filter(lambda x: x.active, self.skills)

		if not skills:
			io.msg_print("You have no useable skills.")
			io.msg_print("")
			return

		done = 0
		listing = 0

		while not done:
			# Print the prompt
			io.prt("(%s %c-%c, *=List, ESC=exit) %s which %s? " % ("Skills", I2A(0), I2A(len(skills) - 1), "Use", "skill"), 0, 0)

			# Wait for keypress
			key = io.inkey()

			# Clear the prompt
			io.prt("", 0, 0)

			# Show list
			if key in '* ?':
				if not listing:
					io.Term_save()

					y = 1
					x = 20

					# Heading
					io.prt("", y, x)
					io.put_str("Name", y, x + 5)
					io.put_str("Lv Mana Fail Info", y, x + 35)

					# Skill list
					i = 0
					for skill in skills:
						if skill:
							line = "  %c) %-30s%2d %4d %3d%%%s" % (I2A(i), skill.name, skill.level, skill.mana, skill.fail, skill.info)

							io.prt(line, y + i + 1, x)
							i = i + 1

					# Empty footer
					io.prt("", y + i + 1, x)

					listing = 1
				else:
					io.Term_load()
					listing = 0
			# Escape
			elif A2I(key) == -70:
				return
			# Select skill
			elif key in string.letters:
				key = string.lower(key)
				index = A2I(key) - A2I('a')
				if index >= len(skills):
					io.bell()
					io.msg_print("You may not use that skill.")
					io.msg_print("")
				else:
					done = 1

		if listing:
			io.Term_load()

		skills[index].use()


	def load_game_hook(self, dict):
		self.__dict__ = dict["player"]

	def save_game_hook(self):
		return ("player", self.__dict__)

