#####################################################################
#
# base/terrain.py
#
# Terrain features
#
#####################################################################

from variable import events, debug
from angband.terrain import *

#####################################################################
#
# Base class for terrain
#
#####################################################################
class terrain(feature_type):
	pass


#####################################################################
class nothing(terrain):
	index = 0
	name = "nothing"
	char = " "
	color = "w"


#####################################################################
# Close a door (or several doors at once)
#####################################################################
def close_door(*coords):
	for y, x in coords:
		from angband import cave
		tile = cave.tile(y, x)

		if tile.feat == FEAT_OPEN:
			# Close the door
			cave.set_feat(y, x, FEAT_DOOR_HEAD + 0x00)

			# Update some things
#			p_ptr->update |= (PU_VIEW | PU_LITE | PU_MONSTERS)

			# Sound
			from angband import io
			io.sound(io.SOUND_SHUTDOOR)


#####################################################################
# Open a door (or several doors at once)
#####################################################################
def open_door(*coords):
	for y, x in coords:
		from angband import cave
		tile = cave.tile(y, x)

		if FEAT_DOOR_HEAD <= tile.feat <= FEAT_DOOR_TAIL:
			# Open the door
			cave.set_feat(y, x, FEAT_OPEN)

			# Update some things
#			p_ptr->update |= (PU_VIEW | PU_LITE | PU_MONSTERS)

			# Sound
			from angband import io
			io.sound(io.SOUND_OPENDOOR)

