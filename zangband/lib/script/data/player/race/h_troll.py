from base.p_race import player_race

class half_troll(player_race):
	name = "Half-Troll"
	number = 7
	def __init__(self):
		player_race.__init__(self,
				self.name,
				4, -4, -2, -4,  3, -6,
				-5, -8, -8, -2,  -1, 5, 20, -10,
				12,  137,
				20, 10,
				96, 10, 250, 50,
				84,  8, 225, 40,
				3,
				0x005)
	def __str__(self):
		return self.name

