import variable
from base.p_class import player_class_class
from variable import player

class warrior_mage_class(player_class_class):
	name = "Warrior-Mage"
	number = 6
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				2, 2, 0, 1, 0, 1,
				30, 30, 28, 2,  18, 16, 50, 25,
				7,  10,  9, 0,  0,  0,  20, 20,
				4, 50, 20)
	def player_outfit_hook(self, data):
		sword = variable.objects.create("short sword")
		player.give(sword)
		return 1

