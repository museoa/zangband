"""Initialization of the worlds"""


class world:
	"""Base class for a world.
	Contains a "wilderness" as well as the available monsters/items
	("object").
	ABSTRACT CLASS!"""

	# Name of the world
	name = ""

	# Directory that stores the world definition
	# (sub-directory of "script/world")
	directory = ""

	# Entry-points into the world
	# Each world must at least define a "default" entry-point
	entry = {}

	# Labels
	labels = {}

	def __init__(self):
		# Levels of the world
		self.levels = {}
		
	def initialize(self):
		# Run the auto-start scripts
		from util.autorun import autorun_dir
		autorun_dir("worlds", self.directory, "startup")

	# Get the corresponding level
	def get_level(self, name):
		return self.levels[self.labels[name]]

	def get_player_race(self):
		from vars import ui

		# Select the race
		selected = ui.birth_select_race(self.races)

		# Restart character generation
		if not selected: return

		return apply(selected, ())

	def get_player_gender(self):
		from vars import ui

		# Select the gender
		selected = ui.birth_select_gender(self.genders)

		# Restart character generation
		if not selected: return

		return apply(selected, ())

	def get_player_class(self):
		from vars import ui

		# Select the class
		selected = ui.birth_select_class(self.classes)

		# Restart character generation
		if not selected: return

		player_class = apply(selected, ())
		if not player_class: return
		if not player_class.birth(): return
		return player_class


class world_data:
	"""World selection at character birth"""

	def __init__(self):
		# Registered world-classes
		self.data = []

		# Initialized worlds
		self.worlds = {}

	def register(self, world):
		"""Register a new worlds"""

		self.data.append(world)

	def get_world(self):
		# Select the world
		from vars import ui
		selected = ui.birth_select_world(self.data)

		# Abort
		if not selected: return

		# Create the world
		world = apply(selected, ())

		# Store the world for fast access
		import vars
		vars.world = world

		# Initialize the world data
		world.initialize()

		# Store the initialized worlds to form a persistent universe
		self.worlds[world.name] = vars.world

		return world

	# Pretend to be a dictionary
	def __len__(self):
		return len(self.worlds)

	def __getitem__(self, key):
		return self.worlds[key]

	def __setitem__(self, key, value):
		self.worlds[key] = value

	def __delitem__(self, key):
		del self.worlds[key]

