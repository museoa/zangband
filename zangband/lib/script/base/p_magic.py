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
		self.realms = {}
		self.picks = 0
		events.get_player_realms.append(self)

	def check_picks(self):
		return 1

	def assign_realms(self):
		# Clean up the realms
		for realm in self.realms.keys():
			if self.realms[realm].player_picks == 0:
				del self.realms[realm]
			else:
				self.realms[realm].picks = self.realms[realm].player_picks

		# XXX
		if 0:
			# Let the player select his spells
			for realm in self.realms.values():
				c1, c2, u1, u2, r1, r2 = realm.spell_distribution[realm.picks]

				if 0 < c1 < 16:
					for i in range(0, c1):
						pass
		else:
			for realm in self.realms.values():
				for spell in realm.spells:
					player.skills.append(spell())

		# Remove the event
		events.get_player_realms.remove(self)

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

		while 1:
			letters = {}

			row = 15

			# Print the header
			io.Term_putstr( 5, row, -1, io.TERM_WHITE, "Realm")
			io.Term_putstr(30, row, -1, io.TERM_WHITE, "Picks")
			io.Term_putstr(40, row, -1, io.TERM_WHITE, "Common")
			io.Term_putstr(50, row, -1, io.TERM_WHITE, "Uncommon")
			io.Term_putstr(60, row, -1, io.TERM_WHITE, "Rare")

			for realm in self.realms.values():
				letter = "%c" % (I2A(row - 15))
				letters[letter] = realm
				row = row + 1
				c1, c2, u1, u2, r1, r2 = realm.spell_distribution[realm.player_picks]
				io.Term_putstr( 1, row, -1, io.TERM_YELLOW, "%c/%c" % (letter, string.upper(letter)))
				io.Term_putstr( 5, row, -1, io.TERM_YELLOW, realm.name)
				io.Term_putstr(30, row, -1, io.TERM_YELLOW, "%2d" % (realm.player_picks))
				io.Term_putstr(40, row, -1, io.TERM_YELLOW, "%2d+%2d" % (c1, c2))
				io.Term_putstr(50, row, -1, io.TERM_YELLOW, "%2d+%2d" % (u1, u2))
				io.Term_putstr(60, row, -1, io.TERM_YELLOW, "%2d+%2d" % (r1, r2))

			c = io.inkey()

			if letters.has_key(c):
				realm = letters[c]
				if self.picks > 0 and realm.player_picks < 16:
					realm.player_picks = realm.player_picks + 1
					self.picks = self.picks - 1
			elif letters.has_key(string.lower(c)):
				realm = letters[string.lower(c)]
				if realm.player_picks > realm.picks:
					realm.player_picks = realm.player_picks - 1
					self.picks = self.picks + 1
			# Quit
			elif c == 'Q':
				remove_loc()
				quit(NULL)
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
			# Return/Enter
			elif ord(c) == 13:
				if self.check_picks():
					self.assign_realms()
					break
				else:
					io.bell()
			# Error
			else:
				io.bell()

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
		from magic.sorcery import sorcery
		from magic.arcane import arcane
		from magic.death import death
		from magic.nature import nature
		from magic.trump import trump
		from magic.chaos import chaos

		self.realms["arcane"] = arcane()
		self.realms["arcane"].picks = 4
		self.realms["arcane"].player_picks = 4
		self.realms["sorcery"] = sorcery()
		self.realms["life"] = life()
		self.realms["death"] = death()
		self.realms["nature"] = nature()
		self.realms["trump"] = trump()
		self.realms["chaos"] = chaos()

		self.picks = 12

		# Spell stat is Int
		from angband import player
		self.spell_stat = player.A_INT
		self.min_fail = 0

