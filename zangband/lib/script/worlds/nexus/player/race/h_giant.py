from base.p_race import player_race

class half_giant_class(player_race):
	name = "Half-Giant"
	number = 12
	def __init__(self):
		player_race.__init__(self,
				self.name,
				4, -2, -2, -2, 3, -3,
				-6, -8, -6, -2, -1, 5, 25, 5,
				13, 150,
				40, 10,
				100,10, 255, 65,
				80, 10, 240, 64,
				3,
				0x011)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(half_giant_class)

