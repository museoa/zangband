from base.p_class import player_class_class
from variable import player

class warrior_mage_class(player_class_class):
	name = "Warrior-Mage"
	number = 6
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				2, 2, 0, 1, 0, 1,
				30, 30, 28, 2,  18, 16, 50, 25,
				7,  10,  9, 0,  0,  0,  20, 20,
				4, 50, 20)
	def __str__(self):
		return self.name
	def get_player_flags_hook(self, args):
		pass
	def destroy_object_hook(self, object):
		pass
	def sense_inventory_hook(self, args):
		pass
	def player_outfit_hook(self, data):
		from base.object import objects
		sword = objects.create("short sword")
		player.give(sword)
		return 1


# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(warrior_mage_class)

