from base.p_class import player_class_class
from variable import player
import variable

class monk_class(player_class_class):
	name = "Monk"
	number = 8
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				2, -1, 1, 3, 2, 1,
				45, 32, 28, 5, 32, 24, 64, 60,
				15, 11, 10, 0,  0,  0, 40, 30,
				6, 40, 20)
	def __str__(self):
		return self.name
	def get_player_flags_hook(self, args):
		pass
	def destroy_object_hook(self, object):
		pass
	def sense_inventory_hook(self, args):
		pass
	def player_outfit_hook(self, data):
		potion = variable.objects.create("potion of healing")
		player.give(potion)
		armor = variable.objects.create("soft leather armor")
		player.give(armor)
		return 1

