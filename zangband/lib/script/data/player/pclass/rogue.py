from base.p_magic import spellcaster

class rogue(spellcaster):
	number = 3

	title = "Rogue"
	str_mod = 2
	int_mod = 1
	wis_mod = -2
	dex_mod = 3
	con_mod = 1
	chr_mod = -1
	c_dis = 45
	c_dev = 32
	c_sav = 28
	c_stl = 5
	c_srh = 32
	c_fos = 24
	c_thn = 60
	c_thb = 66
	x_dis = 15
	x_dev = 10
	x_sav = 10
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 40
	x_thb = 30
	c_mhp = 6
	c_exp = 25
	pet_upkeep_div = 20

	def __init__(self):
		spellcaster.__init__(self)
		from data.player.skills.sorcery import sorcery
		from data.player.skills.arcane import arcane
		from data.player.skills.death import death
		from data.player.skills.trump import trump

		# 7 fixed picks from either trump, arcane, sorcery, or death
		self.realms["trump"] = trump(7, 0, 7)
		self.realms["arcane"] = arcane(7, 0, 7)
		self.realms["sorcery"] = sorcery(7, 0, 7)
		self.realms["death"] = death(7, 0, 7)
		self.picks = 7

		# Spell stat is Int
		from angband.player import A_INT
		self.spell_stat = A_INT

		self.min_fail = 0

	# Give the rogue a dagger and a soft leather armor
	def player_outfit_hook(self, data):
		from vars import objects, player
		dagger = objects.create("dagger")
		player.give(dagger)
		armor = objects.create("soft leather armor")
		player.give(armor)
		return 1

