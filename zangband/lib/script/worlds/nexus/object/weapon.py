from angband import objects
from base.object import object_class

class weapon_class(object_class):
	pass

class sword_class(weapon_class):
	tval = objects.TV_SWORD

class broad_sword_class(sword_class):
	sval = objects.SV_BROAD_SWORD
	name = "broad sword"

class short_sword_class(sword_class):
	sval = objects.SV_SHORT_SWORD
	name = "short sword"

class small_sword_class(sword_class):
	sval = objects.SV_SMALL_SWORD
	name = "small sword"

class dagger_class(sword_class):
	sval = objects.SV_DAGGER
	name = "dagger"

class hafted_class(weapon_class):
	tval = objects.TV_HAFTED

class mace_class(hafted_class):
	sval = objects.SV_MACE
	name = "mace"


from base.object import objects
objects.announce(broad_sword_class)
objects.announce(dagger_class)
objects.announce(short_sword_class)
objects.announce(small_sword_class)
objects.announce(mace_class)

