from base.p_race import player_race

class human(player_race):
	name = "Human"
	number = 0
	def __init__(self):
		player_race.__init__(self,
				self.name,
				0,  0,  0,  0,  0,  0,
				0,  0,  0,  0,  0,  10,  0,  0,
				10,  100,
				14,  6,
				72,  6, 180, 25,
				66,  4, 150, 20,
				0,
				0x7FF)
	def __str__(self):
		return self.name

