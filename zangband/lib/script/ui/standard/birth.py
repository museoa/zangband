"""UI functions for character generation"""

from angband import io


#####################################################################
class ui_birth:

	#####################################################################
	def selection(self, choices, prompt):
		from angband import commands
		import string
		from util.string import I2A, A2I

		# Init some temporary helper variables
		n = 0
		dict = {}

		# Print all available choices and remember the letter for the selection
		for choice in choices:
			if n < 26:
				letter = "%c" % I2A(n)
			else:
				letter = "%d" % (n - 26)
#			io.put_str("%c) %s" % (letter, choice.name), 19 + (n / 3), 2 + 20 * (n % 3))
			io.put_str("%c) %s" % (letter, choice.name), 18 + (n / 5), 2 + 15 * (n % 5))
			dict[letter] = choice
			n = n + 1

		# Wait for player input
		while 1:
			# Print prompt
			io.put_str(prompt % (I2A(0), I2A(n-1)), 17, 2)

			# Get a key
			c = io.inkey()

			# Quit
			if c == 'Q':
				from angband import system
				system.remove_loc()
				system.quit("")
			# Restart
			elif c == 'S':
				return
			# Random selection
			elif c == '*':
				from angband.random import randint
				selected = choices[randint(len(choices)) - 1]
				break
			# Help
			elif c == '?':
				commands.do_cmd_help()
			# Startup-options
			elif c == '=':
				io.screen_save()
				commands.do_cmd_options_aux(6, "Startup Options")
				io.screen_load()
			# Selection
			elif c in dict.keys():
				selected = dict[c]
				break
			# Error
			else:
				io.bell()

		# Return the selected choice
		return selected



	def birth_select_class(self, classes):
		"""birth_select_class()
		Selection of the player class from the list of available classes
		args: classes : list of available player-classes
		result: None -> restart of character generation
		        class -> selected player class"""

		# Prompt
		io.Term_putstr(5, 15, -1, io.TERM_WHITE,
			"Your 'class' determines various intrinsic abilities and bonuses.")
		io.Term_putstr(5, 16, -1, io.TERM_WHITE,
			"Any entries in parentheses should only be used by advanced players.")

		selected = self.selection(classes, "Choose a class (%c-%c), or * for random: ")

		if selected:
			# Display the class name
			io.c_put_str(io.TERM_L_BLUE, selected.name, 5, 15)
			io.clear_from(15)

		# Return the selected class
		return selected


	#####################################################################
	#
	# birth_select_race()
	#
	# Selection of the player race from the list of available races
	#
	# args: races : list of available player-races
	# result: None -> restart of character generation
	#         race -> selected player race
	#
	#####################################################################
	def birth_select_race(self, races):
		# Prompt
		io.Term_putstr(5, 15, -1, io.TERM_WHITE,
			"Your 'race' determines various intrinsic factors and bonuses.")

		selected = self.selection(races, "Choose a race (%c-%c), or * for random: ")

		if selected:
			# Display the race name
			io.c_put_str(io.TERM_L_BLUE, selected.name, 4, 15)
			io.clear_from(15)

		# Return the selected race
		return selected

	#####################################################################
	#
	# birth_select_gender()
	#
	# Selection of the player gender from the list of available genders
	#
	# args: genders : list of available player-genders
	# result: None -> restart of character generation
	#         race -> selected player gender
	#
	#####################################################################
	def birth_select_gender(self, genders):
		selected = self.selection(genders, "Choose a sex (%c-%c), or * for random: ")

		if selected:
			# Display the gender name
			io.c_put_str(io.TERM_L_BLUE, selected.name, 3, 15)
			io.clear_from(15)

		# Return the selected gender
		return selected


	#####################################################################
	#
	# birth_select_world()
	#
	# Selection of the world from the list of available worlds
	#
	# args: worlds : list of available worlds
	# result: either None (restart of character generation) or the
	#         selected world
	#
	#####################################################################
	def birth_select_world(self, worlds):
		self.clear_from(1)

		selected = self.selection(worlds, "Choose a world (%c-%c), or * for random: ")

		# Return the selected class
		return selected

