from base.p_race import player_race

class zombie(player_race):
	name = "Zombie"
	number = 25
	def __init__(self):
		player_race.__init__(self,
				self.name,
				2, -6, -6, 1, 4, -5,
				-5, -5, 8, -1, -1, 5, 15, 0,
				13, 135,
				100, 30,
				72, 6, 100, 25,
				66, 4, 100, 20,
				2,
				0x001)

	def get_player_flags_hook(self, args):
		from vars import player
		player.resist_neth = 1
		player.hold_life = 1
		player.see_inv = 1
		player.resist_pois = 1
		player.slow_digest = 1
		if player.level > 4:
			player.resist_cold = 1
		return 1

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()
		self.give_torches()
		return 1

