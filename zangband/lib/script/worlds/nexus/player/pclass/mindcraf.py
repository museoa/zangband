from base.p_class import player_class_class
from variable import player


class mindcrafter_class(player_class_class):
	name = "Mindcrafter"
	number = 9
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				-1, 0, 3, -1, -1, 2,
				30, 30, 30, 3,  22, 16, 50, 40,
				10, 10, 10, 0,   0,  0, 20, 30,
				2, 25, 20)
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
		potion = objects.create("potion of restore mana")
		player.give(potion)
		armor = objects.create("soft leather armor")
		player.give(armor)
		sword = objects.create("small sword")
		player.give(sword)
		return 1


# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(mindcrafter_class)

