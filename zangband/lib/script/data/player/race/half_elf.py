from base.p_race import player_race

class half_elf(player_race):
	name = "Half-Elf"
	number = 1
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-1,  1,  1,  1, -1,  1,
				2,  3,  3,  1, 6,  11, -1,  5,
				9,  110,
				24, 16,
				66,  6, 130, 15,
				62,  6, 100, 10,
				2,
                0x7FF)
	def __str__(self):
		return self.name

