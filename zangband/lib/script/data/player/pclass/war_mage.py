from base.p_magic import spellcaster

class warrior_mage(spellcaster):
	number = 6

	title = "Warrior-Mage"
	str_mod = 2
	int_mod = 2
	wis_mod = 0
	dex_mod = 1
	con_mod = 0
	chr_mod = 1
	c_dis = 30
	c_dev = 30
	c_sav = 28
	c_stl = 2
	c_srh = 18
	c_fos = 16
	c_thn = 50
	c_thb = 25
	x_dis = 7
	x_dev = 10
	x_sav = 9
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 20
	x_thb = 20
	c_mhp = 4
	c_exp = 50
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

		# 2 fixed Arcane picks
		self.realms["arcane"] = arcane(2, 2)

		# 8 free picks from the other realms
		self.realms["sorcery"] = sorcery(8)
		self.realms["life"] = life(8)
		self.realms["death"] = death(8)
		self.realms["nature"] = nature(8)
		self.realms["trump"] = trump(8)
		self.realms["chaos"] = chaos(8)
		self.picks = 8

		# Spell stat is Int
		from angband.player import A_INT
		self.spell_stat = A_INT

		self.min_fail = 0

	def player_outfit_hook(self, data):
		from vars import objects, player
		sword = objects.create("short sword")
		player.give(sword)
		return 1

