from base.p_class import player_class

class warrior(player_class):
	number = 0

	title = "Warrior"
	str_mod = 5
	int_mod = -2
	wis_mod = -2
	dex_mod = 2
	con_mod = 2
	chr_mod = -1
	c_dis = 25
	c_dev = 18
	c_sav = 18
	c_stl = 1
	c_srh = 14
	c_fos = 2
	c_thn = 70
	c_thb = 55
	x_dis = 12
	x_dev = 7
	x_sav = 10
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 45
	x_thb = 45
	c_mhp = 9
	c_exp = 0
	pet_upkeep_div = 20

	def __init__(self):
		player_class.__init__(self)

		from vars import events
		events.destroy_object.append(self)

	def get_player_flags_hook(self, args):
		from vars import player
		if player.level > 29:
			player.resist_fear = 1
		return 1

	def destroy_object_hook(self, args):
		object, number = args
		from vars import player, ui
		from angband import objects
		from angband.player import PY_MAX_EXP
		the_object = objects.object_typePtr(object)
		if ((the_object.tval in (objects.TV_LIFE_BOOK, objects.TV_SORCERY_BOOK,
		     objects.TV_NATURE_BOOK, objects.TV_CHAOS_BOOK,
		     objects.TV_DEATH_BOOK, objects.TV_TRUMP_BOOK)) and
			(the_object.sval > 1) and (player.exp < PY_MAX_EXP)):
				tester_exp = player.max_exp / 20
				if tester_exp > 10000: tester_exp = 10000
				if the_object.sval < 3: tester_exp = tester_exp / 4
				if tester_exp < 1: tester_exp = 1

				ui.msg_print("You feel more experienced.")
				player.gain_exp(tester_exp * number)
				return 1
	def sense_inventory_hook(self, args):
		from angband import random
		from vars import player
		# Good sensing
		if (0 != random.rand_int(9000 / (player.level * player.level + 40))):
			return
		# Heavy sensing
		return -1

	# Give the player the usual warrior outfit
	def player_outfit_hook(self, data):
		from vars import objects, player
		ring = objects.create("ring of fear resistance")
		player.give(ring)
		sword = objects.create("broad sword")
		player.give(sword)
		armor = objects.create("chain mail")
		player.give(armor)
		return 1

