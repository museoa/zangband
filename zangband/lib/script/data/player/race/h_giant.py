from base.p_race import player_race

class half_giant(player_race):
	name = "Half-Giant"
	number = 12
	def __init__(self):
		player_race.__init__(self,
				self.name,
				4, -2, -2, -2, 3, -3,
				-6, -8, -6, -2, -1, 5, 25, 5,
				13, 150,
				40, 10,
				100,10, 255, 65,
				80, 10, 240, 64,
				3,
				0x011)

	def get_player_flags_hook(self, args):
		from vars import player
		player.sustain_str = 1
		player.resist_shard = 1
		return 1

