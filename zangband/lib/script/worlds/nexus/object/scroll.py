from angband import objects
from base.object import object_class

class scroll_class(object_class):
	tval = objects.TV_SCROLL

class darkness_class(scroll_class):
	sval = objects.SV_SCROLL_DARKNESS
	name = "scroll of darkness"

class satisfy_hunger_class(scroll_class):
	sval = objects.SV_SCROLL_SATISFY_HUNGER
	name = "scroll of satisfy hunger"

class protection_from_evil_class(scroll_class):
	sval = objects.SV_SCROLL_PROTECTION_FROM_EVIL
	name = "scroll of protection from evil"

from base.object import objects
objects.announce(satisfy_hunger_class)
objects.announce(darkness_class)
objects.announce(protection_from_evil_class)

