#####################################################################
#
# base/p_magic.py
#
# Generic code for player-magic
#
#####################################################################

from variable import events, debug, player


#####################################################################
#
# Generic class for the standard spellcasters
#
#####################################################################
class spellcaster_class:
	def __init__(self):
		self.realms = []
		events.get_player_realms.append(self)

	def get_player_realms_hook(self, args):
		# XXX This section should be moved to the gui-package
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

		row = 15

		# Print the header
		io.Term_putstr( 5, row, -1, io.TERM_WHITE, "Realm")
		io.Term_putstr(30, row, -1, io.TERM_WHITE, "Picks")
		io.Term_putstr(40, row, -1, io.TERM_WHITE, "Common")
		io.Term_putstr(50, row, -1, io.TERM_WHITE, "Uncommon")
		io.Term_putstr(60, row, -1, io.TERM_WHITE, "Rare")

		for realm in self.realms:
			row = row + 1
			io.Term_putstr(5, row, -1, io.TERM_YELLOW, realm.name)


		io.inkey()

		if 0:
			# Init some temporary helper variables
			n = 0
			dict = {}

			# Print all available realms and remember the letter for the selection
			for realm in self.realms:
				if n < 26:
					letter = "%c" % I2A(n)
				else:
					letter = "%d" % (n - 26)
				io.put_str("%c) %s" % (letter, realm.name), 19 + (n/3), 2 + 20 * (n%3))
				dict[letter] = realm
				n = n + 1

			# Wait for player input
			while 1:
				# Print prompt
				io.put_str("Choose a realm (%c-%c), or * for random: " % (I2A(0), I2A(n-1)), 18, 2)

				# Get a key
				c = io.inkey()

				# Quit
				if c == 'Q':
					remove_loc()
					quit(NULL)
				# Restart
				elif c == 'S':
					return -1
				# Random selection
				elif c == '*':
					from angband.random import randint
					selected = self.realms[randint(len(self.realms)) - 1]
					break
				# Help
				elif c == '?':
					commands.do_cmd_help()
				# Startup-options
				elif c == '=':
					io.screen_save()
					commands.do_cmd_options_aux(6, "Startup Options")
					io.screen_load()
				# Class selection
				elif c in dict.keys():
					selected = dict[c]
					break
				# Error
				else:
					io.bell()

			self.magic[selected.name] = apply(selected, ())

			io.put_str("Magic       :", 6, 1)
			io.c_put_str(io.TERM_L_BLUE, selected.name, 6, 15);

			io.clear_from(15)
	
		return 1


#####################################################################
#
# Class for the mage spellcaster
#
#####################################################################
class spellcaster_mage_class(spellcaster_class):
	def __init__(self):
		spellcaster_class.__init__(self)
		from magic.life import life
		self.realms = [life]
		# Spell stat is Int
		from angband import player
		self.spell_stat = player.A_INT
		self.min_fail = 0

