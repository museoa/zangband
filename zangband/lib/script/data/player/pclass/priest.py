from base.p_class import player_class_class
from variable import player
import variable

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

	# Give the priest a mace
	def player_outfit_hook(self, data):
		mace = variable.objects.create("mace")
		player.give(mace)
		return 1

