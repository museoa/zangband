from base.p_race import player_race

class barbarian(player_race):
	name = "Barbarian"
	number = 10
	def __init__(self):
		player_race.__init__(self,
				self.name,
				3, -2,  -1,  1,  2, -2,
				-2, -10, 2,  -1,  1, 7, 12, 10,
				11, 120,
				14, 8,
				82, 5, 200, 20,
				78,  6, 190, 15,
				0,
				0x09D)
	def __str__(self):
		return self.name

