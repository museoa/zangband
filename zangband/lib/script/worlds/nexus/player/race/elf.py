from base.p_race import player_race

class elf_class(player_race):
	name = "Elf"
	number = 2
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-1,  2,  2,  1, -2,  2,
				5,  6,  6,  2, 8,  12, -5, 15,
				8,  120,
				75, 75,
				60,  4, 100,  6,
				54,  4, 80,  6,
				3,
				0x75F)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(elf_class)

