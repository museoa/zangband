from base.p_class import player_class_class
from base.p_magic import spellcaster_class
from variable import player
import variable

#####################################################################
# Class for the mage spellcaster
#####################################################################
class spellcaster_mage_class(spellcaster_class):
	def __init__(self):
		spellcaster_class.__init__(self)
		from data.player.skills.life import life
		from data.player.skills.sorcery import sorcery
		from data.player.skills.arcane import arcane
		from data.player.skills.death import death
		from data.player.skills.nature import nature
		from data.player.skills.trump import trump
		from data.player.skills.chaos import chaos

		# Mages get 4 Arcane picks
		self.realms["arcane"] = arcane()
		self.realms["arcane"].picks = 4
		self.realms["arcane"].player_picks = 4

		# 12 free picks from the other realms
		self.realms["sorcery"] = sorcery()
		self.realms["life"] = life()
		self.realms["death"] = death()
		self.realms["nature"] = nature()
		self.realms["trump"] = trump()
		self.realms["chaos"] = chaos()
		self.picks = 12

		# Spell stat is Int
		from angband.player import A_INT
		self.spell_stat = A_INT

		self.min_fail = 0


class mage_class(spellcaster_mage_class, player_class_class):
	name = "Mage"
	number = 1
	realm1 = ["Life", "Sorcery", "Nature", "Chaos",
			  "Death", "Trump", "Arcane"]
	realm2 = ["Life", "Sorcery", "Nature", "Chaos",
			  "Death", "Trump", "Arcane"]

	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				-5, 3, 0, 1, -2, 1,
				30, 36, 30, 2,  16, 20, 34, 20,
				7,  13, 9,  0,  0,  0,  15, 15,
				0, 30, 15)
		spellcaster_mage_class.__init__(self)
	def __str__(self):
		return self.name
	def get_player_flags_hook(self, args):
		pass
	def destroy_object_hook(self, object):
		pass
	def sense_inventory_hook(self, args):
		pass

	# Give the player a dagger
	def player_outfit_hook(self, data):
		dagger = variable.objects.create("dagger")
		player.give(dagger)
		return 1

	def get_beam_chance(self):
		from variable import player
		return player.level

