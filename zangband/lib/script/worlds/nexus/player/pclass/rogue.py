from base.p_class import player_class_class
from variable import player


class rogue_class(player_class_class):
	name = "Rogue"
	number = 3
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				2, 1, -2, 3, 1, -1,
				45, 32, 28, 5, 32, 24, 60, 66,
				15, 10, 10, 0,  0,  0, 40, 30,
				6, 25, 20)
	def __str__(self):
		return self.name
	def get_player_flags_hook(self, args):
		pass
	def destroy_object_hook(self, object):
		pass
	def sense_inventory_hook(self, args):
		pass
	# Give the rogue a dagger and a soft leather armor
	def player_outfit_hook(self, data):
		from base.object import objects
		dagger = objects.create("dagger")
		player.give(dagger)
		armor = objects.create("soft leather armor")
		player.give(armor)
		return 1


# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(rogue_class)

