#####################################################################
#
# gui/standard/__init__.py
#
# Various functions for the (Graphical) User Interface (GUI)
#
#####################################################################

from angband import io
from util.string import I2A, A2I
import string
from gui import gui_class
import variable

#####################################################################
#
# Standard GUI
#
#####################################################################
class gui_standard(gui_class):
	name = "standard"

	def __init__(self):
		import sys
		import os
		path = os.path.join(sys.path[1], "lib-tk")
		sys.path.insert(2, path)
#		from Tkinter import *
#		self.root = Tk()
#		self.root.title("ZAngband")

#		self.root.mainloop()

		from birth import gui_birth
		self.birth = gui_birth()

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

		# Sort the classes by the old order
		from util.sort import sort_by_number
		sort_by_number(classes)

		self.classes = classes

		import sys
		import os
		path = os.path.join(sys.path[1], "lib-tk")
		sys.path.insert(2, path)
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



	#####################################################################
	#
	# Print the quest info
	#
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
	#
	# Browse a book
	#
	#####################################################################
	def browse_book(self, spells):
		io.Term_save()

		list_spells(spells)

		# Wait for keypress
		io.inkey()

		io.Term_load()



	#####################################################################
	#
	# Cast a spell
	#
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
					list_spells(spells)
					listing = 1
				else:
					io.Term_load()
					listing = 0
			# Escape
			elif A2I(key) == -70:
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

#####################################################################
#
# Initialize the standard GUI
#
#####################################################################
variable.gui = gui_standard()

