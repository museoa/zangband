from base.p_race import player_race

class dwarf(player_race):
	name = "Dwarf"
	number = 5
	def __init__(self):
		player_race.__init__(self,
				self.name,
				2, -2,  2, -2,  2, -3,
				2,  9,  10,  -1,  7,  10, 15,  0,
				11,  125,
				35, 15,
				48,  3, 150, 10,
				46,  3, 120, 10,
				5,
				0x005)

	def get_player_flags_hook(self, args):
		from vars import player
		player.resist_blind = 1
		return 1

