from base.p_race import player_race

class beastman(player_race):
	name = "Beastman"
	number = 29
	def __init__(self):
		player_race.__init__(self,
				self.name,
				2, -2, -1, -1, 2, -4,
				-5, -2, -1, -1, -1, 5, 12, 5,
				11, 140,
				14, 6,
				65,  6, 150, 20,
				61,  6, 120, 15,
				0,
				0x7CF)
	def __str__(self):
		return self.name

