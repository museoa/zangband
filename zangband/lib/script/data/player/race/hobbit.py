from base.p_race import player_race

class hobbit(player_race):
	name = "Hobbit"
	number = 3
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-2,  2,  1,  3,  2,  1,
				15, 18, 18, 5, 12,  15, -10, 20,
				7,  110,
				21, 12,
				36,  3, 60,  3,
				33,  3, 50,  3,
				4,
				0x40B)

	def get_player_flags_hook(self, args):
		from vars import player
		player.sustain_dex = 1
		return 1

