from base.p_class import player_class_class
from variable import player
import variable

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
	def player_outfit_hook(self, data):
		sword = variable.objects.create("broad sword")
		player.give(sword)
		scroll = variable.objects.create("scroll of protection from evil")
		player.give(scroll)
		return 1

