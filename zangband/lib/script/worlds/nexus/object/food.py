from angband import objects
from base.object import object_class
import variable

class food_class(object_class):
	tval = objects.TV_FOOD
	name = "food"

	def object_eat_hook(self):
		print "Eating ", self.name
		return 1

class food_ration_class(food_class):
	sval = objects.SV_FOOD_RATION
	name = "food ration"

variable.objects.announce(food_ration_class)

