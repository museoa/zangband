from base.p_race import player_race

class golem(player_race):
	name = "Golem"
	number = 23
	def __init__(self):
		player_race.__init__(self,
				self.name,
				4, -5, -5, 0, 4, -4,
				-5, -5, 10, -1, -1, 8, 20, 0,
				12, 200,
				1, 100,
				66,  1, 200,  6,
				62,  1, 180,  6,
				4,
				0x001)

	def get_player_flags_hook(self, args):
		from vars import player
		player.slow_digest = 1
		player.free_act = 1
		player.see_inv = 1
		player.resist_pois = 1
		if player.level > 34:
			player.hold_life = 1
		return 1

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()
		self.give_torches()
		return 1

