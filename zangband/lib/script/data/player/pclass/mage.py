from base.p_magic import spellcaster


class mage(spellcaster):
	"""Class for the mage spellcaster"""

	number = 1
	realm1 = ["Life", "Sorcery", "Nature", "Chaos",
			  "Death", "Trump", "Arcane"]
	realm2 = ["Life", "Sorcery", "Nature", "Chaos",
			  "Death", "Trump", "Arcane"]

	title = "Mage"
	str_mod = -5
	int_mod = 3
	wis_mod = 0
	dex_mod = 1
	con_mod = -2
	chr_mod = 1
	c_dis = 30
	c_dev = 36
	c_sav = 30
	c_stl = 2
	c_srh = 16
	c_fos = 20
	c_thn = 34
	c_thb = 20
	x_dis = 7
	x_dev = 13
	x_sav = 9
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 15
	x_thb = 15
	c_mhp = 0
	c_exp = 30
	pet_upkeep_div = 15

	def __init__(self):
		spellcaster.__init__(self)

		from data.player.skills.life import life
		from data.player.skills.sorcery import sorcery
		from data.player.skills.arcane import arcane
		from data.player.skills.death import death
		from data.player.skills.nature import nature
		from data.player.skills.trump import trump
		from data.player.skills.chaos import chaos

		# 4 fixed Arcane picks
		self.realms["arcane"] = arcane(4, 4)

		# 12 free picks from the other realms
		self.realms["sorcery"] = sorcery(12)
		self.realms["life"] = life(12)
		self.realms["death"] = death(12)
		self.realms["nature"] = nature(12)
		self.realms["trump"] = trump(12)
		self.realms["chaos"] = chaos(12)
		self.picks = 12

		# Spell stat is Int
		from angband.player import A_INT
		self.spell_stat = A_INT

		self.min_fail = 0

	# Give the player a dagger
	def player_outfit_hook(self, data):
		from vars import objects, player
		dagger = objects.create("dagger")
		player.give(dagger)
		return 1

	def get_beam_chance(self):
		from vars import player
		return player.level

