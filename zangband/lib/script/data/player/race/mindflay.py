from base.p_race import player_race

class mindflayer(player_race):
	name = "Mindflayer"
	number = 21
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-3, 4, 4, 0, -2, -5,
				10, 25, 15, 2, 5, 12, -10, -5,
				9, 140,
				100, 25,
				68,  6, 142, 15,
				63,  6, 112, 10,
				4,
				0x746)
	def __str__(self):
		return self.name

