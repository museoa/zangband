#####################################################################
#
# base/magic/arcane.py
#
# Base code for the "Arcane" realm
#
#####################################################################

from base.spell import spell
from base.realm import realm
from angband import io, spells, random

class zap(spell):
	common = 1
	name = "Zap"
	level = 1
	mana = 1
	fail = 20

	def effect(self):
		succ, dir = io.get_aim_dir()
		if not succ: return -1

		from variable import player
		beam_chance = player.p_class.get_beam_chance() - 10
		damage = random.damroll(3 + ((player.lev - 1) / 5), 3)

		spells.fire_bolt_or_beam(beam_chance, spells.GF_ELEC, dir, damage)

class detect_monsters(spell):
	common = 1
	name = "Detect Monsters"
	level = 2
	mana = 1
	fail = 33

	def effect(self):
		spells.detect_monsters_normal()

class blink(spell):
	common = 1
	name = "Blink"
	level = 2
	mana = 2
	fail = 33

	def effect(self):
		spells.teleport_player(10)

class light_area(spell):
	common = 1
	name = "Light Area"
	level = 4
	mana = 4
	fail = 40

	def effect(self):
		from variable import player
		damage = random.damroll(2, (player.lev / 2))
		radius = (player.lev / 10) + 1
		spells.lite_area(damage, radius)

class detect_doors_and_traps(spell):
	common = 1
	name = "Detect Doors & Traps"

	def effect(self):
		spells.detect_traps()
		spells.detect_doors()
		spells.detect_stairs()

class teleport(spell):
	common = 1
	name = "Teleport"

	def effect(self):
		from variable import player
		spells.teleport_player(player.lev * 5)

class stone_to_mud(spell):
	common = 1
	name = "Stone to Mud"

	def effect(self):
		succ, dir = io.get_aim_dir()
		if not succ: return -1
		spells.wall_to_mud(dir)

class satisfy_hunger(spell):
	common = 1
	name = "Satisfy Hunger"

	def effect(self):
		from angband.player import PY_FOOD_MAX
		spells.set_food(PY_FOOD_MAX - 1)

class identify(spell):
	common = 1
	name = "Identify"
	
	def effect(self):
		if not spells.ident_spell():
			return -1

class word_of_recall(spell):
	common = 1
	name = "Word of Recall"

	def effect(self):
		spells.word_of_recall()

class arcane(realm):
	name = "Arcane"

	spells = [zap, detect_monsters, blink, light_area,
	          detect_doors_and_traps, teleport, stone_to_mud,
	          satisfy_hunger, identify, word_of_recall]

	spell_distribution = [
		( 0,  0,  0,  0,  0,  0), #  0
		( 0,  4,  0,  0,  0,  0), #  1
		( 1,  8,  0,  0,  0,  0), #  2
		( 2, 12,  0,  0,  0,  0), #  3
		( 0, 16,  0,  0,  0,  0), #  4
		( 0, 16,  0,  0,  0,  0), #  5
		( 0, 16,  0,  0,  0,  0), #  6
		( 0, 16,  0,  0,  0,  0), #  7
		( 0, 16,  0,  0,  0,  0), #  8
		( 0, 16,  0,  0,  0,  0), #  9
		( 0, 16,  0,  0,  0,  0), # 10
		( 0, 16,  0,  0,  0,  0), # 11
		( 0, 16,  0,  0,  0,  0), # 12
		( 0, 16,  0,  0,  0,  0), # 13
		( 0, 16,  0,  0,  0,  0), # 14
		( 0, 16,  0,  0,  0,  0), # 15
		( 0, 16,  0,  0,  0,  0)] # 16

