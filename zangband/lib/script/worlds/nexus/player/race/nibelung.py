from base.p_race import player_race

class nibelung_class(player_race):
	name = "Nibelung"
	number = 18
	def __init__(self):
		player_race.__init__(self,
				self.name,
				1, -1, 2, 0, 2, -4,
				3, 5, 10, 1, 5, 10, 9, 0,
				11, 135,
				40, 12,
				43,  3, 92,  6,
				40,  3, 78,  3,
				5,
				0x40F)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(nibelung_class)

