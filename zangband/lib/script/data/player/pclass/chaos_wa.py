from base.p_class import player_class_class
from variable import player
import variable

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
	def player_outfit_hook(self, data):
		sword = variable.objects.create("broad sword")
		player.give(sword)
		armor = variable.objects.create("metal scale mail")
		player.give(armor)
		return 1

