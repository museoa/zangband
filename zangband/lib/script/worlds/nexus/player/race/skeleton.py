from base.p_race import player_race

class skeleton_class(player_race):
	name = "Skeleton"
	number = 24
	def __init__(self):
		player_race.__init__(self,
				self.name,
				0, -2, -2, 0, 1, -4,
				-5, -5, 5, -1, -1, 8, 10, 0,
				10, 145,
				100, 35,
				72,  6, 50, 5,
				66,  4, 50, 5,
				2,
				0x70F)

	def __str__(self):
		return self.name

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()
		self.give_torches()
		return 1


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(skeleton_class)

