#####################################################################
#
# ui/standard/__init__.py
#
# Various functions for the (Graphical) User Interface (UI)
#
#####################################################################

from angband import io
from util.string import I2A, A2I
import string
from ui import ui_class

#####################################################################
#
# Standard UI
#
#####################################################################
class ui_standard(ui_class):
	name = "standard"

	def __init__(self):
		from birth import ui_birth
		self.birth = ui_birth()

	#####################################################################
	# Select spells at player birth
	#####################################################################
	def birth_select_spells(self, realm):
		io.clear_from(0)

		from list import double_list
		list = double_list(realm.spells)
		return list.execute()

	#####################################################################
	# Select a skill to use
	#####################################################################
	def select_skill(self, skills):
		from list import skill_list
		list = skill_list(skills)
		return list.execute()
	

	#####################################################################
	# Print the quest info
	#####################################################################
	def print_quest_info(self, text):
		y = 6
		for line in text:
			io.put_str(line, y, 0)
			y = y + 1
		io.inkey()



	def list_spells(self, spells):
		y = 1
		x = 20

		# Heading
		io.prt("", y, x)
		io.put_str("Name", y, x + 5)
		io.put_str("Lv Mana Fail Info", y, x + 35)

		# Spell list
		i = 0
		for spell in spells:
			if spell:
				line = "  %c) %-30s%2d %4d %3d%%%s" % (I2A(i), spell.name, spell.level, spell.mana, spell.fail, spell.info)
			else:
				line = "  %c) %-30s" % (I2A(i), "(illegible)")

			io.prt(line, y + i + 1, x)
			i = i + 1

		# Empty footer
		io.prt("", y + i + 1, x)



	#####################################################################
	# Browse a book
	#####################################################################
	def browse_book(self, spells):
		io.Term_save()

		self.list_spells(spells)

		# Wait for keypress
		io.inkey()

		io.Term_load()



	#####################################################################
	# Cast a spell
	#####################################################################
	def cast_spell(self, spells):
		done = 0
		listing = 0

		while not done:
			# Print the prompt
			io.prt("(%s %c-%c, *=List, ESC=exit) %s which %s? " % ("Spells", I2A(0), I2A(1), "Cast", "spell"), 0, 0)

			# Wait for keypress
			key = io.inkey()

			# Clear the prompt
			io.prt("", 0, 0)

			# Show list
			if key in '* ?':
				if not listing:
					io.Term_save()
					self.list_spells(spells)
					listing = 1
				else:
					io.Term_load()
					listing = 0
			# Escape
			elif ord(key) == 27:
				return
			# Select spell
			elif key in string.letters:
				key = string.lower(key)
				index = A2I(key) - A2I('a')
				if index >= len(spells):
					io.bell()
					io.msg_print("You may not cast that spell.")
					io.msg_print("")
				else:
					done = 1

		if listing:
			io.Term_load()

		return spells[index]

