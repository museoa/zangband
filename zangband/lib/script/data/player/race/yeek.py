from base.p_race import player_race

class yeek(player_race):
	name = "Yeek"
	number = 15
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-2, 1, 1, 1, -2, -7,
				2, 4, 10, 3, 5, 15, -5, -5,
				7, 100,
				14, 3,
				50,  3, 90,  6,
				50,  3, 75,  3,
				2,
				0x60F)
	def __str__(self):
		return self.name

