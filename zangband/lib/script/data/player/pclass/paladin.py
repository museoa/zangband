from base.p_magic import spellcaster

class paladin(spellcaster):
	number = 5

	title = "Paladin"
	str_mod = 3
	int_mod = -3
	wis_mod = 1
	dex_mod = 0
	con_mod = 2
	chr_mod = 2
	c_dis = 20
	c_dev = 24
	c_sav = 26
	c_stl = 1
	c_srh = 12
	c_fos = 2
	c_thn = 68
	c_thb = 40
	x_dis = 7
	x_dev = 10
	x_sav = 11
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 35
	x_thb = 30
	c_mhp = 6
	c_exp = 35
	pet_upkeep_div = 20

	def __init__(self):
		spellcaster.__init__(self)
		from data.player.skills.life import life
		from data.player.skills.death import death

		# 7 fixed picks in either life or death
		self.realms["life"] = life(7, 0, 7)
		self.realms["death"] = death(7, 0, 7)
		self.picks = 7

		# Spell stat is Wis
		from angband.player import A_WIS
		self.spell_stat = A_WIS

		self.min_fail = 0

	def player_outfit_hook(self, data):
		from vars import objects, player
		sword = objects.create("broad sword")
		player.give(sword)
		scroll = objects.create("scroll of protection from evil")
		player.give(scroll)
		return 1

