from base.skill import racial_power
from angband.player import A_WIS

class berserk(racial_power):
	name = "Berserk"
	level = 8
	cost = 10
	fail = 0
	use_stat = A_WIS

	def get_difficulty(self):
		from variable import player
		# XXX XXX XXX
		if player.p_class.name == "Warrior":
			return 6
		else:
			return 12

	def effect(self):
		from angband.io import msg_print
		from angband.random import randint
		from variable import player
		msg_print("Raaagh!")
		player.set_afraid(0)
		player.set_shero(player.shero + 10 + randint(player.level))
		player.hp_player(30)


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

		from variable import player
		player.skills.append(berserk())

	def get_player_flags_hook(self, args):
		from variable import player
		player.resist_fear = 1
		return 1

