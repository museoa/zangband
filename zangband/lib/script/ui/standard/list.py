#####################################################################
#
# list.py
#
# Selection from a list
#
#####################################################################


from ui.list import column, list_selection



#####################################################################
# List selection with the standard terminal code.
#####################################################################
from angband import io
from util.string import I2A, A2I
import string

class angterm_list_selection(list_selection):
	def __init__(self, list, columns):
		list_selection.__init__(self, list, columns)
		self.start_x = 0
		self.start_y = 0
		self.cursor_row = 0
		self.row = 0
		self.max_row = min(len(self.list), self.row + 22)
		self.max_length = len(self.list)

	def execute(self):
		io.screen_save()
		result = list_selection.execute(self)
		io.screen_load()
		return result

	def get_prompt(self):
		return "%s (%c-%c): " % (self.prompt, I2A(0), I2A(min(len(self.list) - 1, 21)))

	def display_prompt(self):
		io.prt(self.get_prompt(), self.start_y, self.start_x)

		io.move_cursor(self.cursor_row + 2, self.start_x)

		# Get keypress
		key = io.inkey()

		self.execute_command(key)

	def display(self):
		self.max_length = min(max(self.max_length, len(self.list)), 20)

		# Recalculate the width of the list
		self.width = 3
		for column in self.columns:
			self.width = self.width + column.width + 1

		# Erase old stuff
		for row in range(0, self.max_length + 3):
			io.put_str(" " * self.width, row, self.start_x)

		self.display_header()

		for item in self.list[self.row:self.max_row]:
			self.display_item(item)

	def display_header(self):
		self.y = self.start_y + 1
		self.x = self.start_x

		io.put_str("   ", self.y, self.x)
		self.x = self.x + 3

		for column in self.columns:
			text = string.ljust(column.header, column.width + 1)
			io.c_put_str(io.TERM_YELLOW, text, self.y, self.x)
			self.x = self.x + column.width + 1

		self.y = self.y + 1

	# Show 'item' at the current row of the list
	def display_item(self, item):
		self.x = self.start_x
		color = self.get_color(item)

		text = "%c) " % (I2A(self.y - 2))
		io.c_put_str(color, text, self.y, self.x)
		self.x = self.x + 3

		for column in self.columns:
			# Get the value and convert it to a string
			value = "%s" % column.get_value(item)
			# Cut after 'column.width' characters
			text = text[:column.width]
			# Pad the text with spaces
			text = string.ljust(value, column.width + 1)
			io.c_put_str(color, text, self.y, self.x)
			self.x = self.x + column.width + 1

		self.y = self.y + 1

	def get_color(self, item):
		if self.select_multiple and item in self.selected:
			return io.TERM_L_BLUE

		return io.TERM_WHITE

	def execute_command(self, command):
		# Escape
		if ord(command) == 27:
			if self.allow_cancel:
				self.selected = []
			self.done = 1
		# List
		elif command == '*':
			pass
		elif 0 <= A2I(command) < 22:
			index = A2I(command) + self.row
			if 0 <= index < len(self.list):
				if self.list[index] in self.selected:
					self.selected.remove(self.list[index])
				else:
					self.selected.append(self.list[index])
				if not self.select_multiple:
					self.done = 1
		elif command == ' ':
			if len(self.list) > 22:
				if self.row + 22 < len(self.list):
					self.row = self.row + 22
				else:
					self.row = 0
			self.max_row = min(len(self.list), self.row + 22)
		# Cursor down
		elif command == '2':
			if self.cursor_row < len(self.list) - 1:
				self.cursor_row = self.cursor_row + 1
		# Cursor up
		elif command == '8':
			if self.cursor_row > 0:
				self.cursor_row = self.cursor_row - 1
		# Cursor left
		elif command == '4':
			if self.list and self.list[self.cursor_row] in self.selected:
				self.selected.remove(self.list[self.cursor_row])
		# Cursor right
		elif command == '6':
			if self.list and not self.list[self.cursor_row] in self.selected:
				self.selected.append(self.list[self.cursor_row])
		# Return
		elif ord(command) == 13:
			self.done = 1

	def append_item(self, item):
		self.list.append(item)
		self.max_row = self.max_row + 1

	def remove_item(self, item):
		self.list.remove(item)
		self.max_row = self.max_row - 1


#####################################################################
# List for player-race selection at birth
#####################################################################
class birth_race_list(angterm_list_selection):
	def __init__(self, list, columns, columns=None):
		angterm_list_selection.__init__(self, list, columns)
		self.prompt = "Select Race"
		if columns == None:
			self.columns = [column("", 40, attribute="name")]


#####################################################################
# List for skill selection
#####################################################################
class skill_list(angterm_list_selection):
	def __init__(self, list, columns=None):
		angterm_list_selection.__init__(self, list, columns)
		if columns == None:
			self.columns = [column("Name", 40, attribute="name"),
			                column("Lvl",   3, attribute="level"),
			                column("Mana",  4, attribute="mana"),
			                column("Fail",  4, attribute="fail")]
		self.prompt = "Select skill"



#####################################################################
# List for realm selection
#####################################################################
class realm_list(angterm_list_selection):
	def __init__(self, list, columns=None):
		angterm_list_selection.__init__(self, list, columns)
		if columns == None:
			self.columns = [column("Realm",    40, attribute="name"),
			                column("Picks",     5, attribute="level"),
			                column("Common",    9, attribute="mana"),
			                column("Uncommon",  9, attribute="fail"),
			                column("Rare",      9, attribute="fail")]
		self.prompt = "Select picks"

	def execute_command(self, command):
		from angband import io, system, commands

		# Quit
		if command == 'Q':
			system.remove_loc()
			system.quit(NULL)
		# Restart
		elif command == 'S':
			return -1
		# Help
		elif command == '?':
			commands.do_cmd_help()
		# Startup-options
		elif command == '=':
			io.screen_save()
			commands.do_cmd_options_aux(6, "Startup Options")
			io.screen_load()
		else:
			angterm_list_selection.execute_command(self, command)



#####################################################################
# List for spell selection
#####################################################################
class spell_list(angterm_list_selection):
	select_multiple = 1
	allow_cancel = 0

	def __init__(self, list, columns=None):
		angterm_list_selection.__init__(self, list, columns)
		if columns == None:
			self.columns = [column("Name", 20, attribute="name"),
			                column("Lvl",   3, attribute="level"),
			                column("Mana",  4, attribute="mana"),
			                column("Fail",  4, attribute="fail")]
		self.prompt = "Select spell"

	def execute_command(self, command):
		from angband import io, system, commands

		# Quit
		if command == 'Q':
			system.remove_loc()
			system.quit(NULL)
		# Restart
		elif command == 'S':
			return -1
		# Help
		elif command == '?':
			commands.do_cmd_help()
		# Startup-options
		elif command == '=':
			io.screen_save()
			commands.do_cmd_options_aux(6, "Startup Options")
			io.screen_load()
		else:
			angterm_list_selection.execute_command(self, command)


#####################################################################
#
#####################################################################
class double_list(spell_list):
	def __init__(self, list, columns=None):
		spell_list.__init__(self, list, columns)
		self.available = spell_list(list, columns)
		self.known = spell_list([], columns)
		self.known.start_x = 40

	def execute(self):
		spell_list.execute(self)
		return self.known.list

	def display(self):
		self.available.display()
		self.known.display()

	def execute_command(self, command):
		if 0 <= A2I(command) < 22:
			index = A2I(command) + self.row
			if 0 <= index < len(self.available.list):
				spell = self.available.list[index]
				self.known.append_item(spell)
				self.available.remove_item(spell)
		# Cursor down
		elif command == '2':
			if self.cursor_row < max(len(self.available.list), len(self.known.list)) - 1:
				self.cursor_row = self.cursor_row + 1
		# Cursor up
		elif command == '8':
			if self.cursor_row > 0:
				self.cursor_row = self.cursor_row - 1
		# Cursor left
		elif command == '4':
			if self.cursor_row < len(self.known.list):
				spell = self.known.list[self.cursor_row]
				self.available.append_item(spell)
				self.known.remove_item(spell)
		# Cursor right
		elif command == '6':
			if self.cursor_row < len(self.available.list):
				spell = self.available.list[self.cursor_row]
				self.known.append_item(spell)
				self.available.remove_item(spell)
		else:
			spell_list.execute_command(self, command)



#####################################################################
# Test
#####################################################################
class test_list(angterm_list_selection):
	def __init__(self, list, columns=None):
		def method_test(item):
			return item

		angterm_list_selection.__init__(self, list, columns)
		self.prompt = "Select Row"
		if columns == None:
			self.columns = [column("Name", 20),
			                column("Column 2", 20, method = method_test)]

def test():
	list = ["Row 1 - this is a very long entry", "Row 2", "Row 3", "Row 4", "Row 5",
	        "Row 6", "Row 7", "Row 8", "Row 9", "Row 10",
	        "Row 11", "Row 12", "Row 13", "Row 14", "Row 15",
	        "Row 16", "Row 17", "Row 18", "Row 19", "Row 20",
	        "Row 21", "Row 22", "Row 23", "Row 24", "Row 25",
	        "Row 26", "Row 27", "Row 28", "Row 29", "Row 30"]

	test = test_list(list)
	print test.execute()


# test()

