from base.p_race import player_race

class dark_elf(player_race):
	name = "Dark-Elf"
	number = 19
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-1, 3, 2, 2, -2, 1,
				5, 15, 20, 3, 8, 12, -5, 10,
				9, 150,
				75, 75,
				60,  4, 100,  6,
				54,  4, 80,  6,
				5,
				0x7DF)

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_dark = 1
		if player.lev > 19:
			player.see_inv = 1
		return 1

