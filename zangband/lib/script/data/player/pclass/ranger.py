from base.p_magic import spellcaster

class ranger(spellcaster):
	number = 4

	title = "Ranger"
	str_mod = 2
	int_mod = 2
	wis_mod = 0
	dex_mod = 1
	con_mod = 1
	chr_mod = 1
	c_dis = 30
	c_dev = 32
	c_sav = 28
	c_stl = 3
	c_srh = 24
	c_fos = 16
	c_thn = 56
	c_thb = 72
	x_dis = 8
	x_dev = 10
	x_sav = 10
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 30
	x_thb = 45
	c_mhp = 4
	c_exp = 30
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

		# 4 fixed Nature picks
		self.realms["nature"] = nature(4, 4)

		# 4 free picks from the other realms
		self.realms["sorcery"] = sorcery(4)
		self.realms["life"] = life(4)
		self.realms["death"] = death(4)
		self.realms["trump"] = trump(4)
		self.realms["chaos"] = chaos(4)
		self.picks = 4

		# Spell stat is Int
		from angband.player import A_INT
		self.spell_stat = A_INT

		self.min_fail = 0

	# Give the ranger a dagger
	def player_outfit_hook(self, data):
		from vars import objects, player
		dagger = objects.create("dagger")
		player.give(dagger)
		return 1

