#####################################################################
#
# ui/birth.py
#
# UI functions for character generation
#
#####################################################################

from angband import io


class ui_birth:
	#####################################################################
	#
	# selection()
	#
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
			io.put_str("%c) %s" % (letter, choice.name), 19 + (n/3), 2 + 20 * (n%3))
			dict[letter] = choice
			n = n + 1

		# Wait for player input
		while 1:
			# Print prompt
			io.put_str(prompt % (I2A(0), I2A(n-1)), 18, 2)

			# Get a key
			c = io.inkey()

			# Quit
			if c == 'Q':
				remove_loc()
				quit(NULL)
			# Restart
			elif c == 'S':
				return
			# Random selection
			elif c == '*':
				from angband.random import randint
				selected = choices[randint(len(choice)) - 1]
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



	#####################################################################
	#
	# select_class()
	#
	# Selection of the player class from the list of available classes
	#
	# args: classes : list of available player-classes
	# result: None -> restart of character generation
	#         class -> selected player class
	#
	#####################################################################
	def select_class(self, classes):
		# Sort the classes by the old order
		from util.sort import sort_by_number
		sort_by_number(classes)

		self.classes = classes

		from Tkinter import *
		self.root = Tk()
		self.root.title("Choose a class")
		message1 = Message(self.root,
			text="""Your 'class' determines various intrinsic abilities and bonuses. Any entries in parentheses should only be used by advanced players.""")
		message1.grid(row=0, columnspan=3, sticky=N+E+W)

		self.listbox = Listbox(self.root)

		for p_class in classes:
			self.listbox.insert("end", p_class.name)

		self.listbox.activate(1)
		self.listbox.focus_set()
		self.listbox.bind("<Double-Button-1>", self.select_class_ok)
		self.listbox.grid(row=2, columnspan=3, sticky=E+W)

		ok_button = Button(self.root, text="Ok", command=self.select_class_ok)
		ok_button.grid(row=3, sticky=S)

		restart_button = Button(self.root, text="Restart", command=self.select_class_restart)
		restart_button.grid(row=3, column=1, sticky=S)

		random_button = Button(self.root, text="Random", command=self.select_class_random)
		random_button.grid(row=3, column=2, sticky=S)

		self.selected = None

		self.root.mainloop()

		if self.selected:
			# Display the class name
			io.c_put_str(io.TERM_L_BLUE, self.selected.name, 5, 15)
			io.clear_from(15)

		# Return the selected class
		return self.selected

	def select_class_ok(self, e=None):
		items = self.listbox.curselection()
		try:
			import string
			items = map(string.atoi, items)
			self.root.destroy()
			self.selected = self.classes[items[0]]
		except ValueError:
			pass

	def select_class_restart(self, e=None):
		self.root.destroy()
		self.selected = None

	def select_class_random(self, e=None):
		self.root.destroy()
		from angband.random import randint
		self.selected = self.classes[randint(len(self.classes)) - 1]


	#####################################################################
	#
	# select_race()
	#
	# Selection of the player race from the list of available raceses
	#
	# args: races : list of available player-races
	# result: None -> restart of character generation
	#         race -> selected player race
	#
	#####################################################################
	def select_race(self, races):
		# Prompt
		io.Term_putstr(5, 15, -1, io.TERM_WHITE,
			"Your 'race' determines various intrinsic factors and bonuses.")

		# Sort the races by the old order
		from util.sort import sort_by_number
		sort_by_number(races)

		selected = self.selection(races, "Choose a race (%c-%c), or * for random: ")

		if selected:
			# Display the race name
			io.c_put_str(io.TERM_L_BLUE, selected.name, 4, 15)
			io.clear_from(15)

		# Return the selected race
		return selected


	#####################################################################
	#
	# select_world()
	#
	# Selection of the world from the list of available worlds
	#
	# args: worlds : list of available worlds
	# result: either None (restart of character generation) or the
	#         selected world
	#
	#####################################################################
	def select_world(self, worlds):
	#	# Prompt
	#	io.Term_putstr(5, 15, -1, io.TERM_WHITE,
	#		"Your 'class' determines various intrinsic abilities and bonuses.")
	#	io.Term_putstr(5, 16, -1, io.TERM_WHITE,
	#		"Any entries in parentheses should only be used by advanced players.")

	#	# Sort the classes by the old order
	#	from util.sort import sort_by_number
	#	sort_by_number(classes)

		selected = self.selection(worlds, "Choose a world (%c-%c), or * for random: ")

	#	# Display the class name
	#	io.c_put_str(io.TERM_L_BLUE, selected.name, 5, 15)
	#	io.clear_from(15)

		# Return the selected class
		return selected

