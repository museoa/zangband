#####################################################################
#
# base/ui.py
#
# Base for the (graphical) user interface
#
#####################################################################


#####################################################################
# Helper class for list selection
#####################################################################
class column:
	def __init__(self, header = "", width = 0,
	             attribute = "", method = None):
		self.header = header
		self.width = width
		self.attribute = attribute
		self.method = method


	def get_value(self, item):
		if self.attribute:
			return getattr(item, self.attribute)
		elif self.function:
			return apply(self.method, (item,))
		else:
			return item


#####################################################################
# Abstract base class for list selection
#####################################################################
class list_selection:
	# Available options
	select_multiple = 0
	allow_cancel = 1

	def __init__(self, list, columns):
		self.list = list
		self.selected = []
		self.columns = columns
		self.done = 0
		self.prompt = "Prompt"


	def execute(self):
		while not self.done:
			self.display()
			self.display_prompt()

		return self.selected


	def display_prompt(self):
		pass


	def display(self):
		pass


	def get_column_headers(self):
		return []


from angband import io

#####################################################################
#
# Base class for the UI
#
#####################################################################
class ui:
	name = "no UI"

	def __init__(self):
		pass

	def select_world(self, worlds):
		pass

	def birth_select_spells(self, realm):
		pass

	def get_aim_dir(self):
		return io.get_aim_dir()

	def msg_print(self, text):
		return io.msg_print(text)

	def sound(self, soundevent):
		return io.sound(soundevent)

	def clear_from(self, row):
		io.clear_from(row)

