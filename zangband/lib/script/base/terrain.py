#####################################################################
#
# base/terrain.py
#
# Terrain features
#
#####################################################################

from variable import events, debug
from angband.terrain import feature_type

#####################################################################
#
# Base class for terrain
#
#####################################################################
class terrain(feature_type):
	index = 0
	name = "nothing"
	char = " "
	color = "w"

class nothing(terrain):
	index = 0
	name = "nothing"
	char = " "
	color = "w"


#####################################################################
#
# Terrain Feature Indexes (see "lib/edit/f_info.txt")
#
#####################################################################

# Nothing
FEAT_NONE             = 0x00

# Various
FEAT_FLOOR            = 0x01
FEAT_INVIS            = 0x02
FEAT_GLYPH            = 0x03
FEAT_OPEN             = 0x04
FEAT_BROKEN           = 0x05
FEAT_LESS             = 0x06
FEAT_MORE             = 0x07

# Quest features
FEAT_QUEST_ENTER      = 0x08
FEAT_QUEST_EXIT       = 0x09
FEAT_QUEST_DOWN       = 0x0A
FEAT_QUEST_UP         = 0x0B

# Feature 0x0C - 0x0F unused

# Traps
FEAT_TRAP_HEAD        = 0x10
FEAT_TRAP_TRAPDOOR    = 0x10
FEAT_TRAP_PIT         = 0x11
FEAT_TRAP_SPIKED_PIT  = 0x12
FEAT_TRAP_POISON_PIT  = 0x13
FEAT_TRAP_SUMMON      = 0x14
FEAT_TRAP_TELEPORT    = 0x15
FEAT_TRAP_FIRE        = 0x16
FEAT_TRAP_ACID        = 0x17
FEAT_TRAP_SLOW        = 0x18
FEAT_TRAP_LOSE_STR    = 0x19
FEAT_TRAP_LOSE_DEX    = 0x1A
FEAT_TRAP_LOSE_CON    = 0x1B
FEAT_TRAP_BLIND       = 0x1C
FEAT_TRAP_CONFUSE     = 0x1D
FEAT_TRAP_POISON      = 0x1E
FEAT_TRAP_SLEEP       = 0x1F
FEAT_TRAP_TAIL        = 0x1F

# Doors
FEAT_DOOR_HEAD        = 0x20
FEAT_DOOR_TAIL        = 0x2F

# Extra
FEAT_SECRET           = 0x30
FEAT_RUBBLE           = 0x31

# Seams
FEAT_MAGMA            = 0x32
FEAT_QUARTZ           = 0x33
FEAT_MAGMA_H          = 0x34
FEAT_QUARTZ_H         = 0x35
FEAT_MAGMA_K          = 0x36
FEAT_QUARTZ_K         = 0x37

# Walls
FEAT_WALL_EXTRA       = 0x38
FEAT_WALL_INNER       = 0x39
FEAT_WALL_OUTER       = 0x3A
FEAT_WALL_SOLID       = 0x3B
FEAT_PERM_EXTRA       = 0x3C
FEAT_PERM_INNER       = 0x3D
FEAT_PERM_OUTER       = 0x3E
FEAT_PERM_SOLID       = 0x3F

# Glyph
FEAT_MINOR_GLYPH      = 0x40

# Pattern
FEAT_PATTERN_START    = 0x41
FEAT_PATTERN_1        = 0x42
FEAT_PATTERN_2        = 0x43
FEAT_PATTERN_3        = 0x44
FEAT_PATTERN_4        = 0x45
FEAT_PATTERN_END      = 0x46
FEAT_PATTERN_OLD      = 0x47
FEAT_PATTERN_XTRA1    = 0x48
FEAT_PATTERN_XTRA2    = 0x49

# Shops
FEAT_SHOP_HEAD        = 0x4A
FEAT_SHOP_TAIL        = 0x52

# Terrains
FEAT_DEEP_WATER       = 0x53
FEAT_SHAL_WATER       = 0x54
FEAT_DEEP_LAVA        = 0x55
FEAT_SHAL_LAVA        = 0x56
FEAT_DARK_PIT         = 0x57
FEAT_DIRT             = 0x58
FEAT_GRASS            = 0x59

# Feature 0x5A - 0x5F unused

# Terrain
FEAT_TREES            = 0x60
FEAT_MOUNTAIN         = 0x61

# Feature 0x62 - 0x7F unused

# Buildings
FEAT_BLDG_HEAD        = 0x80
FEAT_BLDG_TAIL        = 0x9F



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

