from base.p_race import player_race

class dark_elf_class(player_race):
	name = "Dark-Elf"
	number = 19
	def __init__(self):
		player_race.__init__(self,
				self.name,
				-1, 3, 2, 2, -2, 1,
				5, 15, 20, 3, 8, 12, -5, 10,
				9, 150,
				75, 75,
				60,  4, 100,  6,
				54,  4, 80,  6,
				5,
				0x7DF)
	def __str__(self):
		return self.name


# Make our race known
from base.birth import player_race_birth
player_race_birth.races.append(dark_elf_class)

