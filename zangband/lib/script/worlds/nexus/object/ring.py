from angband import objects
from base.object import object_class
import variable

class ring_class(object_class):
	tval = objects.TV_RING

class resist_fear_class(ring_class):
	sval = objects.SV_RING_RES_FEAR
	name = "ring of fear resistance"

class sustain_int_class(ring_class):
	sval = objects.SV_RING_SUSTAIN_INT
	name = "ring of sustain intelligence"

variable.objects.announce(resist_fear_class)
variable.objects.announce(sustain_int_class)

