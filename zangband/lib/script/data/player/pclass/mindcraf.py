from base.p_class import player_class

class mindcrafter(player_class):
	number = 9

	title = "Mindcrafter"
	str_mod = -1
	int_mod = 0
	wis_mod = 3
	dex_mod = -1
	con_mod = -1
	chr_mod = 2
	c_dis = 30
	c_dev = 30
	c_sav = 30
	c_stl = 3
	c_srh = 22
	c_fos = 16
	c_thn = 50
	c_thb = 40
	x_dis = 10
	x_dev = 10
	x_sav = 10
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 20
	x_thb = 30
	c_mhp = 2
	c_exp = 25
	pet_upkeep_div = 20

	def player_outfit_hook(self, data):
		from vars import objects, player
		potion = objects.create("potion of restore mana")
		player.give(potion)
		armor = objects.create("soft leather armor")
		player.give(armor)
		sword = objects.create("small sword")
		player.give(sword)
		return 1

