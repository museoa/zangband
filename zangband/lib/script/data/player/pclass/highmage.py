from base.p_class import player_class_class
from variable import player
import variable

class highmage_class(player_class_class):
	name = "High-Mage"
	number = 10
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				-5, 4, 0, 0, -2, 1,
				30, 36, 30, 2,  16, 20, 34, 20,
				7,  13, 9,  0,  0,  0,  15, 15,
				0, 30, 12)

	def player_outfit_hook(self, data):
		dagger = variable.objects.create("dagger")
		player.give(dagger)
		ring = variable.objects.create("ring of sustain intelligence")
		player.give(ring)
		return 1

