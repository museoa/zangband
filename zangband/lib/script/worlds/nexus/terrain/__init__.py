from variable import events

class terrains_class:
	def __init__(self, name):
		self.version = 0
		self.name = name

class terrain_data_class:
	def __init__(self):
		self.terrains = {}

terrains = terrain_data_class()

