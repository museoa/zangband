from base.p_race import player_race

class kobold_class(player_race):
	name = "Kobold"
	number = 17
	def __init__(self):
		player_race.__init__(self,
				self.name,
				1, -1, 0, 1, 0, -4,
				-2, -3, -2, -1, 1, 8, 10, -8,
				9, 125,
				11,  3,
				60,  1, 130,  5,
				55,  1, 100,  5,
				3,
				0x009)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(kobold_class)

