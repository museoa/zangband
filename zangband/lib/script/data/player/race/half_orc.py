from base.p_race import player_race

class half_orc(player_race):
	name = "Half-Orc"
	number = 6
	def __init__(self):
		player_race.__init__(self,
				self.name,
				2, -1,  0,  0,  1, -4,
				-3, -3, -3,  -1,  0, 7, 12, -5,
				10,  110,
				11,  4,
				66,  1, 150,  5,
				62,  1, 120,  5,
				3,
				0x18D)

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_dark = 1
		return 1

