from base.p_magic import spellcaster

class chaos_warrior(spellcaster):
	number = 7

	title = "Chaos-Warrior"
	str_mod = 2
	int_mod = 1
	wis_mod = 0
	dex_mod = 1
	con_mod = 2
	chr_mod = -2
	c_dis = 20
	c_dev = 25
	c_sav = 25
	c_stl = 1
	c_srh = 14
	c_fos = 12
	c_thn = 65
	c_thb = 40
	x_dis = 7
	x_dev = 11
	x_sav = 10
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 34
	x_thb = 29
	c_mhp = 6
	c_exp = 35
	pet_upkeep_div = 20

	def __init__(self):
		spellcaster.__init__(self)
		from data.player.skills.chaos import chaos

		# 7 fixed chaos picks
		self.realms["chaos"] = chaos(7, 7)

		# Spell stat is Int
		from angband.player import A_INT
		self.spell_stat = A_INT

		self.min_fail = 0

	def player_outfit_hook(self, data):
		from vars import objects, player
		sword = objects.create("broad sword")
		player.give(sword)
		armor = objects.create("metal scale mail")
		player.give(armor)
		return 1

