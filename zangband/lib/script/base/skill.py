#####################################################################
#
# base/skill.py
#
# Generic code for the player skills
#
#####################################################################

from variable import events, debug


#####################################################################
# Base class for the skills
#####################################################################
class skill:
	name = "abstract skill"
	level = 0
	mana = 0
	fail = 0
	info = ""

	active = 1

	# Use the skill
	def use(self):
		return 1

