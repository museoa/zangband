from base.p_race import player_race

class draconian(player_race):
	name = "Draconian"
	number = 20
	def __init__(self):
		player_race.__init__(self,
				self.name,
				2, 1, 1, 1, 2, -3,
				-2, 5, 3, 0, 1, 10, 5, 5,
				11, 250,
				75, 33,
				76,  1, 160,  5,
				72,  1, 130,  5,
				2,
				0x757)

	def get_player_flags_hook(self, args):
		from vars import player
		player.ffall = 1
		if player.level > 4:
			player.resist_fire = 1
		if player.level > 9:
			player.resist_cold = 1
		if player.level > 14:
			player.resist_acid = 1
		if player.level > 19:
			player.resist_elec = 1
		if player.level > 34:
			player.resist_pois = 1
		return 1

