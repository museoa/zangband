from angband import objects
from base.object import object_class
import variable

class potion_class(object_class):
	tval = objects.TV_POTION

class healing_class(potion_class):
	sval = objects.SV_POTION_HEALING
	name = "potion of healing"

class restore_mana_class(potion_class):
	sval = objects.SV_POTION_RESTORE_MANA
	name = "potion of restore mana"

variable.objects.announce(healing_class)
variable.objects.announce(restore_mana_class)
