from base.p_race import player_race
from base import monster

class amberite(player_race, monster.amberite):
	name = "Amberite"
	number = 8
	def __init__(self):
		player_race.__init__(self,
				self.name,
				1,  2,  2,  2,  3,  2,
				4,  5,  5,  2, 3, 13, 15, 10,
				10,  225,
				50, 50,
				82, 5, 190, 20,
				78,  6, 180, 15,
				0,
				0x7FF)

	def get_player_flags_hook(self, args):
		from vars import player
		player.sustain_con = 1
		player.regenerate = 1
		return 1

