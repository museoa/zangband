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
		pass

class slot:
	def __init__(self, name = "empty slot"):
		self.name = name
		self.skill = skill()
	def assign(self, skill):
		self.skill = skill
		self.name = skill.name
	def use(self):
		self.skill.use()

class page:
	def __init__(self, name = "empty page"):
		self.name = name
		self.data = {}
		for i in range(0, 20):
			self.data[i] = slot()

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
		self.slots = {}

		# Create the skill-slots/pages
		for i in range(0, 20):
			self.slots[i] = page()

		events.use_skill.append(self)

	#################################################################
	# Add a skill
	#################################################################
	def append(self, skill):
		self.data.append(skill)

	#################################################################
	# Use a skill
	#################################################################
	def use_skill_hook(self, args):
		slot = self.select_slot()
		if slot:
			return slot.use()

	#################################################################
	# Select a skill slot
	#################################################################
	def select_slot(self):
		from angband import io
		from util.string import I2A, A2I
		import string

		slots = self.slots
		heading = "Skill"
		page = 1
		listing = 0
		selected_slot = None

		while 1:
			# Print the prompt
			if page:
				upper_task = "Rename page"
			else:
				upper_task = "Assign skill"

			io.prt("%s (a-t=Use, A-T=%s, *=List, ESC=exit): " % (heading, upper_task), 0, 0)

			if listing:
				# Show list
				y = 1
				x = 0

				# Heading
				io.prt("", y, x)
				io.put_str("Name", y, x + 5)
				io.put_str("Lv Mana Fail Info", y, x + 35)

				# Skill list
				skills = slots.items()
				skills.sort()
				for key, value in skills:
					line = "  %c) %-30s" % (I2A(key), value.name)
					y = y + 1
					io.prt(line, y, x)

				# Empty footer
				io.prt("", y + 1, x)

			# Get keypress
			key = io.inkey()

			# Escape
			if A2I(key) == -70:
				break
			# List
			elif key == '*':
				if listing == 0:
					io.screen_save()
					listing = 1
				else:
					io.screen_load()
					listing = 0
			# Use skill
			elif key in "abcdefghijklmnopqrst":
				index = A2I(key) - A2I('a')
				if page:
					slots = slots[index]
					heading = slots.name
					page = 0
				else:
					selected_slot = slots[index]
					break
			# Assign skill
			elif key in "ABCDEFGHIJKLMNOPQRST":
				index = A2I(string.lower(key)) - A2I('a')
				if page:
					buffer = "%-30s" % (slots[index].name)
					if io.get_string("Rename: ", buffer, 30):
						name = string.split(buffer, '\0')
						name = name[0]
						slots[index].name = name
					slots = slots[index]
					heading = slots.name
					page = 0
				else:
					slot = slots[index]
					skill = self.select_skill()
					if skill:
						slot.assign(skill)				
					break

		if listing:
			# Restore the screen
			io.screen_load()

		io.prt("", 0, 0)

		return selected_slot


	#################################################################
	# Select a skill
	#################################################################
	def select_skill(self):
		from angband import io
		from util.string import I2A, A2I
		import string

		listing = 0
		selected_skill = None

		while 1:
			# Print the prompt
			io.prt("Select skill (a-t=Select, *=List, ESC=exit): ", 0, 0)

			if listing:
				y = 1
				x = 0

				# Heading
				io.prt("", y, x)
				io.put_str("Name", y, x)
				io.put_str("Lv Mana Fail Info", y, x + 35)

				# Skill list
				i = 0
				for skill in self.data:
					line = "%-30s%2d %4d %3d%%%s" % (skill.name, skill.level, skill.mana, skill.fail, skill.info)

					io.Term_putstr(x, y + i + 1, -1, io.TERM_WHITE, line)
					i = i + 1

				# Empty footer
				io.prt("", y + i + 1, x)


			# Get keypress
			key = io.inkey()

			# Escape
			if A2I(key) == -70:
				break
			# List
			elif key == '*':
				if listing == 0:
					io.screen_save()
					listing = 1
				else:
					io.screen_load()
					listing = 0
			# Lower case -> Use skill
			elif key in "abcdefghijklmnopqrst":
				index = A2I(key) - A2I('a')
				if index < len(self.data):
					selected_skill = self.data[index]
				break

		if listing:
			# Restore the screen
			io.screen_load()

		io.prt("", 0, 0)

		return selected_skill

