from base.p_class import player_class_class
from variable import player
import variable

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
	# Give the rogue a dagger and a soft leather armor
	def player_outfit_hook(self, data):
		dagger = variable.objects.create("dagger")
		player.give(dagger)
		armor = variable.objects.create("soft leather armor")
		player.give(armor)
		return 1

