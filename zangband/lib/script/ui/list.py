#####################################################################
#
# list.py
#
# Selection from a list
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

