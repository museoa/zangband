#####################################################################
#
# base/skill.py
#
# Generic code for the player skills
#
#####################################################################

from variable import events, debug, ui


#####################################################################
# Base class for the skills
#####################################################################
class skill:
	name = "abstract skill"
	level = 0
	mana = 0
	fail = 0
	info = ""

	# Active (1) or passive (0) skill
	active = 1

	# Use the skill
	def use(self):
		pass



#####################################################################
#
# Class for the players skills
#
#####################################################################
class skills:
	#################################################################
	# Constructor
	#################################################################
	def __init__(self):
		self.data = []

		events.use_skill.append(self)

	#################################################################
	# Add a skill
	#################################################################
	def append(self, skill):
		self.data.append(skill)

	#################################################################
	# Add a list of skills
	#################################################################
	def add_skills(self, value):
		self.data = self.data + value

	def __len__(self):
		return len(self.data)
	def __getitem__(self, key):
		return self.data[key]
	def __setitem__(self, key, value):
		self.data[key] = value
	def items(self):
		return self.data.items()
	def keys(self):
		return self.data.keys()
	def values(self):
		return self.data.values()

	#################################################################
	# Use a skill
	#################################################################
	def use_skill_hook(self, args):
		skill = ui.select_skill(self.data)
		if skill:
			return skill[0].use()

