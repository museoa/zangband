#####################################################################
#
# base/magic/life.py
#
# Base code for the "Life" realm
#
#####################################################################

from realms import spell, realm

class detect_evil(spell):
	name = "Detect Evil"

	def effect(self):
		from angband import spells
		spells.detect_monsters_evil()

class cure_light_wounds(spell):
	name = "Cure Light Wounds"

	def effect(self):
		from angband import spells
		from variable import player
		from angband import random
		spells.hp_player(random.damroll(2, 10))
		spells.set_cut(player.cut - 10)

class bless(spell):
	name = "Bless"

	def effect(self):
		from angband import spells
		from variable import player
		from angband import random
		spells.set_blessed(player.blessed + random.randint(12) + 12)

class remove_fear(spell):
	name = "Remove Fear"

	def effect(self):
		from angband import spells
		spells.set_afraid(0)

class life(realm):
	name = "Life"

	spells = [detect_evil, cure_light_wounds, bless, remove_fear]

