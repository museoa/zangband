from base.p_magic import spellcaster

class monk(spellcaster):
	number = 8

	title = "Monk"
	str_mod = 2
	int_mod = -1
	wis_mod = 1
	dex_mod = 3
	con_mod = 2
	chr_mod = 1
	c_dis = 45
	c_dev = 32
	c_sav = 28
	c_stl = 5
	c_srh = 32
	c_fos = 24
	c_thn = 64
	c_thb = 60
	x_dis = 15
	x_dev = 11
	x_sav = 10
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 40
	x_thb = 30
	c_mhp = 6
	c_exp = 40
	pet_upkeep_div = 20

	def __init__(self):
		spellcaster.__init__(self)

		from data.player.skills.life import life
		from data.player.skills.death import death
		from data.player.skills.nature import nature

		# 7 fixed nature, life or death picks
		self.realms["life"] = life(7, 7)
		self.realms["death"] = death(7, 7)
		self.realms["nature"] = nature(7, 7)

		# Spell stat is Wis
		from angband.player import A_WIS
		self.spell_stat = A_WIS

		self.min_fail = 0

	def player_outfit_hook(self, data):
		from vars import objects, player
		potion = objects.create("potion of healing")
		player.give(potion)
		armor = objects.create("soft leather armor")
		player.give(armor)
		return 1

