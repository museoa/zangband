from base.p_class import player_class_class
from variable import player


class ranger_class(player_class_class):
	name = "Ranger"
	number = 4
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				2, 2, 0, 1, 1, 1,
				30, 32, 28, 3,  24, 16, 56, 72,
				8,  10, 10, 0,  0,  0,  30, 45,
				4, 30, 20)
	def __str__(self):
		return self.name
	def get_player_flags_hook(self, args):
		pass
	def destroy_object_hook(self, object):
		pass
	def sense_inventory_hook(self, args):
		pass
	# Give the ranger a dagger
	def player_outfit_hook(self, data):
		from base.object import objects
		dagger = objects.create("dagger")
		player.give(dagger)
		return 1



# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(ranger_class)

