from base.p_race import player_race

class gnome(player_race):
	name = "Gnome"
	number = 4
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-1,  2,  0,  2,  1, -2,
				10, 12, 12,  3, 6,  13, -8, 12,
				8,  135,
				50, 40,
				42,  3, 90,  6,
				39,  3, 75,  3,
				4,
				0x60F)

	def get_player_flags_hook(self, args):
		from variable import player
		player.free_act = 1
		return 1

