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

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_fire = 1
		if player.level > 9:
			player.see_inv = 1
		return 1

