from base.p_race import player_race

class hobbit_class(player_race):
	name = "Hobbit"
	number = 3
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-2,  2,  1,  3,  2,  1,
				15, 18, 18, 5, 12,  15, -10, 20,
				7,  110,
				21, 12,
				36,  3, 60,  3,
				33,  3, 50,  3,
				4,
				0x40B)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(hobbit_class)

