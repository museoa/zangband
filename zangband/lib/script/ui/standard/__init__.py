#####################################################################
#
# ui/standard.py
#
# Various functions for the (Graphical) User Interface (UI)
#
#####################################################################

from angband import io
from util.string import I2A, A2I
import string
from base.ui import ui
from birth import ui_birth

#####################################################################
#
# Standard UI
#
#####################################################################
class ui_standard(ui, ui_birth):
	name = "standard"

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
	def display_spell_list(self, spells):
		from list import spell_list
		list = spell_list(spells)
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

	#####################################################################
	# Select realms
	#####################################################################
	def birth_select_realms(self, realms, picks):
		from angband import io
		from angband import commands
		import string
		from util.string import I2A, A2I

#		io.Term_putstr(5, 15, -1, io.TERM_WHITE,
#			"The realm of magic will determine which spells you can learn.")
#		io.Term_putstr(5, 16, -1, io.TERM_WHITE,
#			"Life and Sorcery are protective, Chaos and Death are destructive.")
#		io.Term_putstr(5, 17, -1, io.TERM_WHITE,
#			"Nature has both defensive and offensive spells.")

		# Current cursor position
		current_index = 0

		# Assign realms to letters
		letters = {}
		i = 0
		for realm in realms.values():
			letter = "%c" % (I2A(i))
			letters[letter] = realm
			i = i + 1

		# First row
		row = 15

		while 1:
			# Print the header
			io.Term_putstr( 5, row, -1, io.TERM_WHITE, "Realm")
			io.Term_putstr(30, row, -1, io.TERM_WHITE, "Picks")
			io.Term_putstr(40, row, -1, io.TERM_WHITE, "Common")
			io.Term_putstr(50, row, -1, io.TERM_WHITE, "Uncommon")
			io.Term_putstr(60, row, -1, io.TERM_WHITE, "Rare")

			# Print the table
			i = 0
			for realm in realms.values():
				letter = "%c" % (I2A(i))			
				if i == current_index:
					color = io.TERM_YELLOW
				else:
					color = io.TERM_WHITE
				c1, c2, u1, u2, r1, r2 = realm.spell_distribution[realm.picks]
				io.Term_putstr( 1, row + 1 + i, -1, color, "%c/%c" % (letter, string.upper(letter)))
				io.Term_putstr( 5, row + 1 + i, -1, color, realm.name)
				io.Term_putstr(30, row + 1 + i, -1, color, "%2d" % (realm.picks))
				io.Term_putstr(40, row + 1 + i, -1, color, "%2d+%2d" % (c1, c2))
				io.Term_putstr(50, row + 1 + i, -1, color, "%2d+%2d" % (u1, u2))
				io.Term_putstr(60, row + 1 + i, -1, color, "%2d+%2d" % (r1, r2))
				i = i + 1

			io.move_cursor(row + 1 + current_index, 1)

			c = io.inkey()

			if letters.has_key(c):
				realm = letters[c]
				if picks > 0 and realm.picks < realm.max_picks:
					realm.picks = realm.picks + realm.pick_step
					picks = picks - realm.pick_step
			elif letters.has_key(string.lower(c)):
				realm = letters[string.lower(c)]
				if realm.picks > realm.fixed_picks:
					realm.picks = realm.picks - realm.pick_step
					picks = picks + realm.pick_step
			elif c == '2':
				if current_index < len(realms) - 1:
					current_index = current_index + 1
			elif c == '8':
				if current_index > 0:
					current_index = current_index - 1
			elif c == '6':
				realm = letters["%c" % (I2A(current_index))]
				if picks > 0 and realm.picks < realm.max_picks:
					realm.picks = realm.picks + realm.pick_step
					picks = picks - realm.pick_step
			elif c == '4':
				realm = letters["%c" % (I2A(current_index))]
				if realm.picks > realm.fixed_picks:
					realm.picks = realm.picks - realm.pick_step
					picks = picks + realm.pick_step
			# Quit
			elif c == 'Q':
				from angband import system
				system.remove_loc()
				system.quit("")
			# Restart
			elif c == 'S':
				return -1
			# Help
			elif c == '?':
				commands.do_cmd_help()
			# Startup-options
			elif c == '=':
				io.screen_save()
				commands.do_cmd_options_aux(6, "Startup Options")
				io.screen_load()
			# Escape or Return
			elif ord(c) in (27, 13):
				break
			# Error
			else:
				io.bell()

		io.clear_from(row)

		return realms

