from base.p_race import player_race

class half_ogre(player_race):
	name = "Half-Ogre"
	number = 11
	def __init__(self):
		player_race.__init__(self,
				self.name,
				3, -1, -1, -1, 3, -3,
				-3, -5, -5, -2, -1, 5, 20, 0,
				12,  130,
				40, 10,
				92, 10, 255, 60,
				80,  8, 235, 60,
				3,
				0x407)

	def get_player_flags_hook(self, args):
		from vars import player
		player.resist_dark = 1
		player.sustain_str = 1
		return 1

