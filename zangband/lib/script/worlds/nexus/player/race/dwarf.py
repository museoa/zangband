from base.p_race import player_race

class dwarf_class(player_race):
	name = "Dwarf"
	number = 5
	def __init__(self):
		player_race.__init__(self,
				self.name,
				2, -2,  2, -2,  2, -3,
				2,  9,  10,  -1,  7,  10, 15,  0,
				11,  125,
				35, 15,
				48,  3, 150, 10,
				46,  3, 120, 10,
				5,
				0x005)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(dwarf_class)

