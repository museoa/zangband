from base.p_race import player_race

class high_elf(player_race):
	name = "High-Elf"
	number = 9
	def __init__(self):
		player_race.__init__(self,
				self.name,
				1,  3,  2,  3,  1,  5,
				4,  20, 20,  4,  3, 14, 10, 25,
				10,  200,
				100, 30,
				90, 10, 190, 20,
				82, 10, 180, 15,
				4,
				0x75F)

	def get_player_flags_hook(self, args):
		from vars import player
		player.resist_lite = 1
		player.see_inv = 1
		return 1

