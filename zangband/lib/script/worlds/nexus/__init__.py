"""Initialization of the world 'Nexus'"""

from base.world import world
from base.utils import proxy

class nexus(world):
	# Name of the world
	name = "Nexus"

	# Subdirectory name
	directory = "nexus"

	races = [proxy("Human", "data.player.race.human", "human"),
	         proxy("Half Elf", "data.player.race.half_elf", "half_elf"),
	         proxy("Elf", "data.player.race.elf", "elf"),
	         proxy("Hobbit", "data.player.race.hobbit", "hobbit"),
	         proxy("Gnome", "data.player.race.gnome", "gnome"),
	         proxy("Dwarf", "data.player.race.dwarf", "dwarf"),
	         proxy("Half Orc", "data.player.race.half_orc", "half_orc"),
	         proxy("Half Troll", "data.player.race.h_troll", "half_troll"),
	         proxy("Amberite", "data.player.race.amberite", "amberite"),
	         proxy("High Elf", "data.player.race.high_elf", "high_elf"),
	         proxy("Barbarian", "data.player.race.barbaria", "barbarian"),
	         proxy("Half Ogre", "data.player.race.h_ogre", "half_ogre"),
	         proxy("Half Giant", "data.player.race.h_giant", "half_giant"),
	         proxy("Half Titan", "data.player.race.h_titan", "half_titan"),
	         proxy("Cyclops", "data.player.race.cyclops", "cyclops"),
	         proxy("Yeek", "data.player.race.yeek", "yeek"),
	         proxy("Klackon", "data.player.race.klackon", "klackon"),
	         proxy("Kobold", "data.player.race.kobold", "kobold"),
	         proxy("Nibelung", "data.player.race.nibelung", "nibelung"),
	         proxy("Dark Elf", "data.player.race.dark_elf", "dark_elf"),
	         proxy("Draconian", "data.player.race.draconia", "draconian"),
	         proxy("Mindflayer", "data.player.race.mindflay", "mindflayer"),
	         proxy("Imp", "data.player.race.imp", "imp"),
	         proxy("Golem", "data.player.race.golem", "golem"),
	         proxy("Skeleton", "data.player.race.skeleton", "skeleton"),
	         proxy("Zombie", "data.player.race.zombie", "zombie"),
	         proxy("Vampire", "data.player.race.vampire", "vampire"),
	         proxy("Spectre", "data.player.race.spectre", "spectre"),
	         proxy("Sprite", "data.player.race.sprite", "sprite"),
	         proxy("Beastman", "data.player.race.beastman", "beastman")]

	genders = [proxy("Female", "data.player.gender.female", "female"),
	           proxy("Male", "data.player.gender.male", "male")]

	classes = [proxy("Warrior", "data.player.pclass.warrior", "warrior"),
	           proxy("Mage", "data.player.pclass.mage", "mage_class"),
	           proxy("Priest", "data.player.pclass.priest", "priest"),
	           proxy("Rogue", "data.player.pclass.rogue", "rogue"),
	           proxy("Ranger", "data.player.pclass.ranger", "ranger"),
	           proxy("Paladin", "data.player.pclass.paladin", "paladin"),
	           proxy("Warrior Mage", "data.player.pclass.war_mage", "warrior_mage"),
	           proxy("Chaos Warrior", "data.player.pclass.chaos_wa", "chaos_warrior"),
	           proxy("Monk", "data.player.pclass.monk", "monk"),
	           proxy("Mindcrafter", "data.player.pclass.mindcraf", "mindcrafter"),
	           proxy("High Mage", "data.player.pclass.highmage", "highmage")]

