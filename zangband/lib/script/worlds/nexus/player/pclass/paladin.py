from base.p_class import player_class_class
from variable import player


class paladin_class(player_class_class):
	name = "Paladin"
	number = 5
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				3, -3, 1, 0, 2, 2,
				20, 24, 26, 1,  12, 2, 68, 40,
				7,  10, 11, 0,  0,  0,  35, 30,
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
		scroll = objects.create("scroll of protection from evil")
		player.give(scroll)
		return 1


# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(paladin_class)

