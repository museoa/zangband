from base.p_race import player_race

class spectre(player_race):
	name = "Spectre"
	number = 27
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-5, 4, 4, 2, -3, -6,
				10, 25, 20, 5, 5, 14, -15, -5,
				7, 180,
				100, 30,
				72, 6, 100, 25,
				66, 4, 100, 20,
				5,
				0x74E)

	def __str__(self):
		return self.name

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()
		self.give_torches()
		return 1

