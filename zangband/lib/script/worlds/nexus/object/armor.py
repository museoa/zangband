from angband import objects
from base.object import object_class

class armor_class(object_class):
	pass

class hard_armor_class(armor_class):
	tval = objects.TV_HARD_ARMOR

class chain_mail_class(hard_armor_class):
	sval = objects.SV_CHAIN_MAIL
	name = "chain mail"

class metal_scale_mail_class(hard_armor_class):
	sval = objects.SV_METAL_SCALE_MAIL
	name = "metal scale mail"

class soft_armor_class(armor_class):
	tval = objects.TV_SOFT_ARMOR

class soft_leather_armor_class(soft_armor_class):
	sval = objects.SV_SOFT_LEATHER_ARMOR
	name = "soft leather armor"

from base.object import objects
objects.announce(chain_mail_class)
objects.announce(metal_scale_mail_class)
objects.announce(soft_leather_armor_class)

