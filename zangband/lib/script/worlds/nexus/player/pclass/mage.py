from base.p_class import player_class_class
from base.p_magic import spellcaster_mage_class
from variable import player


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
		from base.object import objects
		dagger = objects.create("dagger")
		player.give(dagger)
		return 1

# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(mage_class)

