from base.p_race import player_race

class klackon(player_race):
	name = "Klackon"
	number = 16
	def __init__(self):
		player_race.__init__(self,
				self.name,
				2, -1, -1, 1, 2, -2,
				10, 5, 5, 0, -1, 10, 5, 5,
				12, 135,
				20, 3,
				60,  3, 80,  4,
				54,  3, 70,  4,
				2,
				0x011)

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_conf = 1
		player.resist_acid = 1
		player.pspeed = player.pspeed + player.lev / 10
		return 1

