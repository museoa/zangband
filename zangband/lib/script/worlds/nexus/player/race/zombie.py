from base.p_race import player_race

class zombie_class(player_race):
	name = "Zombie"
	number = 25
	def __init__(self):
		player_race.__init__(self,
				self.name,
				2, -6, -6, 1, 4, -5,
				-5, -5, 8, -1, -1, 5, 15, 0,
				13, 135,
				100, 30,
				72, 6, 100, 25,
				66, 4, 100, 20,
				2,
				0x001)

	def __str__(self):
		return self.name

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()
		self.give_torches()
		return 1

# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(zombie_class)

