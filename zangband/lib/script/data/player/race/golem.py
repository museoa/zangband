from base.p_race import player_race

class golem(player_race):
	name = "Golem"
	number = 23
	def __init__(self):
		player_race.__init__(self,
				self.name,
				4, -5, -5, 0, 4, -4,
				-5, -5, 10, -1, -1, 8, 20, 0,
				12, 200,
				1, 100,
				66,  1, 200,  6,
				62,  1, 180,  6,
				4,
				0x001)

	def __str__(self):
		return self.name

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()
		self.give_torches()
		return 1

