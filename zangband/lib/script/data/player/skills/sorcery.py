#####################################################################
#
# base/magic/sorcery.py
#
# Base code for the "Sorcery" realm
#
#####################################################################

from base.spell import spell
from base.realm import realm
from angband import spells, random

class wizard_lock(spell):
	common = 1
	name = "Wizard Lock"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		from vars import ui
		succ, dir = ui.get_aim_dir()
		if not succ: return -1
		spells.wizard_lock(dir)

class phlogiston(spell):
	common = 1
	name = "Phlogiston"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		spells.phlogiston()

class confuse_monster(spell):
	common = 1
	name = "Confuse Monster"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		from vars import ui
		succ, dir = ui.get_aim_dir()
		if not succ: return -1

		from vars import player
		spells.confuse_monster(dir, (player.level * 3) / 2)

class sleep_monster(spell):
	common = 1
	name = "Sleep Monster"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		from vars import ui
		succ, dir = ui.get_aim_dir()
		if not succ: return -1

		spells.sleep_monster(dir)

class magic_mapping(spell):
	common = 1
	name = "Magic Mapping"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		from angband import cave
		cave.map_area()

class recharging(spell):
	common = 1
	name = "Recharging"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		from vars import player
		spells.recharge(player.level * 4)

class slow_monster(spell):
	common = 1
	name = "Slow Monster"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		from vars import ui
		succ, dir = ui.get_aim_dir()
		if not succ: return -1

		spells.slow_monster(dir)

class charm_monster(spell):
	common = 1
	name = "Charm Monster"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		from vars import ui
		succ, dir = ui.get_aim_dir()
		if not succ: return -1

		from vars import player
		spells.charm_monster(dir, player.level)

class teleport_level(spell):
	common = 1
	name = "Teleport Level"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		spells.teleport_player_level()

class sorcery(realm):
	name = "Sorcery"

	spells = [wizard_lock, phlogiston, confuse_monster, sleep_monster,
	          magic_mapping, recharging, slow_monster, charm_monster,
	          teleport_level]

