from angband import objects
from base.object import object_class
import variable


class lite_class(object_class):
	tval = objects.TV_LITE

class torch_class(lite_class):
	sval = objects.SV_LITE_TORCH
	name = "wooden torch"

variable.objects.announce(torch_class)

