from base.p_magic import spellcaster


class priest(spellcaster):
	number = 2

	title = "Priest"
	str_mod = -1
	int_mod = -3
	wis_mod = 3
	dex_mod = -1
	con_mod = 0
	chr_mod = 2
	c_dis = 25
	c_dev = 30
	c_sav = 32
	c_stl = 2
	c_srh = 16
	c_fos = 8
	c_thn = 48
	c_thb = 35
	x_dis = 7
	x_dev = 10
	x_sav = 12
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 20
	x_thb = 20
	c_mhp = 2
	c_exp = 20
	pet_upkeep_div = 20

	def __init__(self):
		spellcaster.__init__(self)
		from data.player.skills.life import life
		from data.player.skills.sorcery import sorcery
		from data.player.skills.arcane import arcane
		from data.player.skills.death import death
		from data.player.skills.nature import nature
		from data.player.skills.trump import trump
		from data.player.skills.chaos import chaos

		# 8 fixed life or death picks
		self.realms["life"] = life(8, 8)
		self.realms["death"] = death(8, 8)

		# 4 free picks from the other realms
		self.realms["sorcery"] = sorcery(4)
		self.realms["nature"] = nature(4)
		self.realms["trump"] = trump(4)
		self.realms["chaos"] = chaos(4)
		self.picks = 4

		# Spell stat is Wis
		from angband.player import A_WIS
		self.spell_stat = A_WIS

		self.min_fail = 0

	# Give the priest a mace
	def player_outfit_hook(self, data):
		from vars import objects, player
		mace = objects.create("mace")
		player.give(mace)
		return 1

