from base.p_race import player_race

class skeleton(player_race):
	name = "Skeleton"
	number = 24
	def __init__(self):
		player_race.__init__(self,
				self.name,
				0, -2, -2, 0, 1, -4,
				-5, -5, 5, -1, -1, 8, 10, 0,
				10, 145,
				100, 35,
				72,  6, 50, 5,
				66,  4, 50, 5,
				2,
				0x70F)

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_shard = 1
		player.hold_life = 1
		player.see_inv = 1
		player.resist_pois = 1
		if player.lev > 9:
			player.resist_cold = 1
		return 1

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()
		self.give_torches()
		return 1

