#####################################################################
# Initialize the debugger
#####################################################################
import util.debug
debug = util.debug.debug()

#####################################################################
# Init the events
#####################################################################
import base.event
events = base.event.event_data_class()

#####################################################################
# Init the UI
#####################################################################
import ui.standard
ui = ui.standard.ui_standard()

#####################################################################
# Init the monsters
#####################################################################
import base.monster
monsters = base.monster.monster_data()

#####################################################################
# Init the object storage
#####################################################################
import base.object
objects = base.object.object_data_class()

#####################################################################
# Init the player
#####################################################################
import base.player
player = base.player.player_data_class()

#####################################################################
# Create the worlds
#####################################################################
import base.world
worlds = base.world.world_data()

#####################################################################
# What to save
#####################################################################
def get_save_data():
	return { "debug" : debug,
	         "events" : events,
	         "ui" : ui,
	         "monsters" : monsters,
	         "objects" : objects,
	         "player" : player,
	         "worlds" : worlds }

def set_save_data(dict):
	global debug
	debug = dict["debug"]
	global events
	events = dict["events"]
	global ui
	ui = dict["ui"]
	global monsters
	monsters = dict["monsters"]
	global objects
	objects = dict["objects"]
	global player
	player = dict["player"]
	global worlds
	worlds = dict["worlds"]

