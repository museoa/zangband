from base.p_race import player_race

class spectre(player_race):
	name = "Spectre"
	number = 27
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-5, 4, 4, 2, -3, -6,
				10, 25, 20, 5, 5, 14, -15, -5,
				7, 180,
				100, 30,
				72, 6, 100, 25,
				66, 4, 100, 20,
				5,
				0x74E)

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_neth = 1
		player.hold_life = 1
		player.see_inv = 1
		player.resist_pois = 1
		player.slow_digest = 1
		player.resist_cold = 1
		player.pass_wall = 1
		if player.lev > 34:
			player.telepathy = 1
		return 1

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()
		self.give_torches()
		return 1

