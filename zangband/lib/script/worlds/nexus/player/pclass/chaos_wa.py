from base.p_class import player_class_class
from variable import player


class chaos_warrior_class(player_class_class):
	name = "Chaos-Warrior"
	number = 7
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				2, 1, 0, 1, 2, -2,
				20, 25, 25, 1,  14, 12, 65, 40,
				7,  11, 10, 0,  0,  0,  34, 29,
				6, 35, 20)
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
		sword = objects.create("broad sword")
		player.give(sword)
		armor = objects.create("metal scale mail")
		player.give(armor)
		return 1


# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(chaos_warrior_class)

