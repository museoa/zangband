from base.p_race import player_race

class vampire(player_race):
	name = "Vampire"
	number = 26
	def __init__(self):
		player_race.__init__(self,
				self.name,
				3, 3, -1, -1, 1, 2,
				4, 10, 10, 4, 1, 8, 5, 0,
				11, 200,
				100, 30,
				72,  6, 180, 25,
				66,  4, 150, 20,
				5,
				0x7FF)

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_dark = 1
		player.hold_life = 1
		player.resist_neth = 1
		player.resist_cold = 1
		player.resist_pois = 1
		player.lite = 1
		return 1

	def player_outfit_hook(self, data):
		self.give_satisfy_hunger_scrolls()

		from default.objects import objects
		from angband.random import rand_range
		scroll = objects.create("scroll of darkness")
		scroll.number = rand_range(2, 5)
		player.give(scroll)
		return 1

