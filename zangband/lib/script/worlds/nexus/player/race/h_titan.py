from base.p_race import player_race

class half_titan_class(player_race):
	name = "Half-Titan"
	number = 13
	def __init__(self):
		player_race.__init__(self,
				self.name,
				5, 1, 1, -2, 3, 1,
				-5, 5, 2, -2, 1, 8, 25, 0,
				14, 255,
				100,30,
				111, 11, 255, 86,
				99, 11, 250, 86,
				0,
				0x727)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(half_titan_class)

