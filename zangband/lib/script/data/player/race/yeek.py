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

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_acid = 1
		if player.level > 19:
			player.immune_acid = 1
		return 1

