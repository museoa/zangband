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


class life(realm):
	name = "Life"

	spells = [detect_evil]

