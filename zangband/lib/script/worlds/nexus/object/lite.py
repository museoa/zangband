from angband import objects
from base.object import object_class


class lite_class(object_class):
	tval = objects.TV_LITE

class torch_class(lite_class):
	sval = objects.SV_LITE_TORCH
	name = "wooden torch"

from base.object import objects
objects.announce(torch_class)

