from base.p_race import player_race

class half_troll(player_race):
	name = "Half-Troll"
	number = 7
	def __init__(self):
		player_race.__init__(self,
				self.name,
				4, -4, -2, -4,  3, -6,
				-5, -8, -8, -2,  -1, 5, 20, -10,
				12,  137,
				20, 10,
				96, 10, 250, 50,
				84,  8, 225, 40,
				3,
				0x005)

	def get_player_flags_hook(self, args):
		from variable import player
		player.sustain_str = 1

		if player.lev > 14:
			# High level trolls heal fast...
			player.regenerate = 1

			# Let's not make Regeneration
			# a disadvantage for the poor warriors who can
			# never learn a spell that satisfies hunger (actually
			# neither can rogues, but half-trolls are not
			# supposed to play rogues)
			if player.p_class.name == "Warrior":
				player.slow_digest = 1

		return 1

