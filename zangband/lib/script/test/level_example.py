#####################################################################
#
# level_example.py
#
#####################################################################

# Base-class for this level
from base.level import dungeon_level

# Import used actions
from base.action import fireball

class example_level(dungeon_level):
	map = "example.map"
	player_pos = (1, 1)

	actions = {
	           (1,2) : (fireball, {"source" : (1, 1), "target" : (1, 2), "radius" : 5, "damage" : 1})
	          }

