from angband import objects
from base.object import object_class

class weapon(object_class):
	pass

class sword(weapon):
	tval = objects.TV_SWORD

class broad_sword(sword):
	sval = objects.SV_BROAD_SWORD
	name = "broad sword"

class short_sword(sword):
	sval = objects.SV_SHORT_SWORD
	name = "short sword"

class small_sword(sword):
	sval = objects.SV_SMALL_SWORD
	name = "small sword"

class dagger(sword):
	sval = objects.SV_DAGGER
	name = "dagger"

class hafted(weapon):
	tval = objects.TV_HAFTED

class mace(hafted):
	sval = objects.SV_MACE
	name = "mace"

import vars
vars.objects.announce(broad_sword)
vars.objects.announce(dagger)
vars.objects.announce(short_sword)
vars.objects.announce(small_sword)
vars.objects.announce(mace)

