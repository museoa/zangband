from base.p_race import player_race

class sprite(player_race):
	name = "Sprite"
	number = 28
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-4, 3, 3, 3, -2, 2,
				10, 10, 10, 4, 10, 10, -12, 0,
				7, 175,
				50, 25,
				32,  2, 75,  2,
				29,  2, 65,  2,
				4,
				0x65E)
	def __str__(self):
		return self.name

