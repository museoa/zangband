from base.p_race import player_race

class gnome_class(player_race):
	name = "Gnome"
	number = 4
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-1,  2,  0,  2,  1, -2,
				10, 12, 12,  3, 6,  13, -8, 12,
				8,  135,
				50, 40,
				42,  3, 90,  6,
				39,  3, 75,  3,
				4,
				0x60F)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(gnome_class)

