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
	def player_outfit_hook(self, data):
		potion = variable.objects.create("potion of healing")
		player.give(potion)
		armor = variable.objects.create("soft leather armor")
		player.give(armor)
		return 1

