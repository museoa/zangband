from base.p_race import player_race

class nibelung(player_race):
	name = "Nibelung"
	number = 18
	def __init__(self):
		player_race.__init__(self,
				self.name,
				1, -1, 2, 0, 2, -4,
				3, 5, 10, 1, 5, 10, 9, 0,
				11, 135,
				40, 12,
				43,  3, 92,  6,
				40,  3, 78,  3,
				5,
				0x40F)

	def get_player_flags_hook(self, args):
		from vars import player
		player.resist_disen = 1
		player.resist_dark = 1
		return 1

