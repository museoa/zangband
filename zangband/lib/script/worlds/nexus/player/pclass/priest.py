from base.p_class import player_class_class
from variable import player


class priest_class(player_class_class):
	name = "Priest"
	number = 2
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				-1, -3, 3, -1, 0, 2,
				25, 30, 32, 2,  16, 8, 48, 35,
				7,  10, 12, 0,  0,  0, 20, 20,
				2, 20, 20)
	def __str__(self):
		return self.name
	def get_player_flags_hook(self, args):
		pass
	def destroy_object_hook(self, object):
		pass
	def sense_inventory_hook(self, args):
		pass
	# Give the priest a mace
	def player_outfit_hook(self, data):
		from base.object import objects
		mace = objects.create("mace")
		player.give(mace)
		return 1


# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(priest_class)

