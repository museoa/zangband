from base.p_race import player_race

class half_titan(player_race):
	name = "Half-Titan"
	number = 13
	def __init__(self):
		player_race.__init__(self,
				self.name,
				5, 1, 1, -2, 3, 1,
				-5, 5, 2, -2, 1, 8, 25, 0,
				14, 255,
				100,30,
				111, 11, 255, 86,
				99, 11, 250, 86,
				0,
				0x727)

	def get_player_flags_hook(self, args):
		from vars import player
		player.resist_chaos = 1
		return 1


