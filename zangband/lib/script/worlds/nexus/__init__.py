#####################################################################
#
# Initialization of the world "Nexus"
#
#####################################################################

from base.world import world

class nexus(world):
	# Name of the world
	name = "Nexus"

	# Subdirectory name
	directory = "nexus"

	# Available player races
	def get_player_races(self):
		from data.player.race import amberite
		from data.player.race import barbaria
		from data.player.race import beastman
		from data.player.race import cyclops
		from data.player.race import dark_elf
		from data.player.race import draconia
		from data.player.race import dwarf
		from data.player.race import elf
		from data.player.race import gnome
		from data.player.race import golem
		from data.player.race import h_giant
		from data.player.race import h_ogre
		from data.player.race import h_titan
		from data.player.race import h_troll
		from data.player.race import half_elf
		from data.player.race import half_orc
		from data.player.race import high_elf
		from data.player.race import hobbit
		from data.player.race import human
		from data.player.race import imp

		return [amberite.amberite,
		        barbaria.barbarian,
		        beastman.beastman,
		        cyclops.cyclops,
		        dark_elf.dark_elf,
		        draconia.draconian,
		        dwarf.dwarf,
		        elf.elf,
		        gnome.gnome,
		        golem.golem,
		        h_giant.half_giant,
		        h_ogre.half_ogre,
		        h_titan.half_titan,
		        h_troll.half_troll,
		        half_elf.half_elf,
		        half_orc.half_orc,
		        high_elf.high_elf,
		        hobbit.hobbit,
		        human.human,
		        imp.imp]

	# Available player classes
	def get_player_classes(self):
		from data.player.pclass import chaos_wa
		from data.player.pclass import highmage
		from data.player.pclass import mage
		from data.player.pclass import mindcraf
		from data.player.pclass import monk
		from data.player.pclass import paladin
		from data.player.pclass import priest
		from data.player.pclass import ranger
		from data.player.pclass import rogue
		from data.player.pclass import war_mage
		from data.player.pclass import warrior

		return [chaos_wa.chaos_warrior_class,
		        highmage.highmage_class,
		        mage.mage_class,
		        mindcraf.mindcrafter_class,
		        monk.monk_class,
		        paladin.paladin_class,
		        priest.priest_class,
		        ranger.ranger_class,
		        rogue.rogue_class,
		        war_mage.warrior_mage_class,
		        warrior.warrior_class]

