from base.p_race import player_race

class cyclops_class(player_race):
	name = "Cyclops"
	number = 14
	def __init__(self):
		player_race.__init__(self,
				self.name,
				4, -3, -3, -3, 4, -6,
				-4, -5, -5, -2, -2, 5, 20, 12,
				13, 130,
				50, 24,
				92, 10, 255, 60,
				80,  8, 235, 60,
				1,
				0x005)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(cyclops_class)

