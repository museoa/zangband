from base.p_race import player_race

class imp(player_race):
	name = "Imp"
	number = 22
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-1, -1, -1, 1, 2, -3,
				-3, 2, -1, 1, -1, 10, 5, -5,
				10, 110,
				13,  4,
				68,  1, 150,  5,
				64,  1, 120,  5,
				3,
				0x7CB)
	def __str__(self):
		return self.name

