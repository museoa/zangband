#####################################################################
#
# base/magic/life.py
#
# Base code for the "Life" realm
#
#####################################################################

from base.spell import spell
from base.realm import realm

from angband import spells, random


class detect_evil(spell):
	common = 1
	name = "Detect Evil"

	def effect(self):
		spells.detect_monsters_evil()

class cure_light_wounds(spell):
	common = 1
	name = "Cure Light Wounds"

	def effect(self):
		from variable import player
		spells.hp_player(random.damroll(2, 10))
		spells.set_cut(player.cut - 10)

class bless(spell):
	common = 1
	name = "Bless"

	def effect(self):
		from variable import player
		spells.set_blessed(player.blessed + random.randint(12) + 12)

class remove_fear(spell):
	common = 1
	name = "Remove Fear"

	def effect(self):
		spells.set_afraid(0)

class life(realm):
	name = "Life"

	spells = [detect_evil, cure_light_wounds, bless, remove_fear]

