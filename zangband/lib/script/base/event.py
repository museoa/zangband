#####################################################################
#
# base/event.py
#
# Event handling
#
#####################################################################

from angband.event import *

#####################################################################
# The main event class
#####################################################################
class event:
	def __init__(self, eventnumber, hookname):
		self.hooks = []
		self.version = 0
		self.method = hookname
		self.eventnumber = eventnumber
		set_callback(eventnumber, self)

	# Add a new hook
	def append(self, hook):
		self.hooks.append(hook)

	# Remove a hook
	def remove(self, hook):
		try:
			self.hooks.remove(hook)
		except:
			import traceback
			traceback.print_exc()
			print "in event %s while removing hook %s" % (self.method, hook)

	# Call the event and process all hooks
	def __call__(self, args):
		try:
			end_result = None

			# Go through all hooked-in objects
			for hook in self.hooks:
				my_hook = getattr(hook, self.method)
				result = my_hook(args)
				# Stop when a function returns -1
				if result:
					end_result = result

			return end_result
		except:
			import traceback
			traceback.print_exc()
			print "when calling : %s(%s)" % (self.method, args)


#####################################################################
#
#####################################################################
class grid_event(event):
	def __init__(self, eventnumber, hookname):
		event.__init__(self, eventnumber, hookname)
		# Events use a dictionary (sparse array) atm
		self.hooks = {}

	# Add a function
	def append(self, hook, y, x):
		if not self.hooks.has_key((y, x)):
			self.hooks[(y, x)] = []
		self.hooks[(y, x)].append(hook)

	# Remove a hook
	def remove(self, hook, y, x):
		try:
			self.hooks[(y, x)].remove(hook)
			if not self.hooks[(y, x)]:
				del self.hooks[(y, x)]
		except:
			import traceback
			traceback.print_exc()
			print "in event %s while removing hook %s" % (self, hook)

	# Called when the player leaves the level
	def leave_level_hook(self, level):
		self.clean_up()

	# Called when the player leaves the wilderness
	def leave_wilderness_hook(self, level):
		self.clean_up()

	def clean_up(self):
		# Free the dict
		self.hooks = {}

	# Call the grid-event
	def __call__(self, coords):
		if self.hooks.has_key(coords):
			for hook in self.hooks[coords]:
				try:
					my_hook = getattr(hook, self.method)
					result = my_hook(coords)
					if result:
						return result
				except:
					import traceback
					traceback.print_exc()
					print "when calling : %s(%s)" % (self.method, coords)


#####################################################################
# Return Python data from all hooked objects for storage in the saved game
#####################################################################
class save_event(event):
	# Override the "call" method
	def __call__(self):
		try:
			dict = {}
			dict["version"] = 1
			for hook in self.hooks:
				my_hook = getattr(hook, self.method)
				try:
					tag, data = my_hook()
					dict[tag] = data
				except:
					import traceback
					traceback.print_exc()
					print "when calling : %s" % (my_hook)
			try:
				import cPickle
				pickle = cPickle
			except ImportError:
				import pickle
			return pickle.dumps(dict)
		except:
			import traceback
			traceback.print_exc()
			print "when calling : %s" % (self.method)


#####################################################################
# Load Python data from the saved game and send it to all hooked objects
#####################################################################
class load_event(event):
	# Override the "call" method
	def __call__(self, args):
		try:
			dict = {}
			try:
				import cPickle
				pickle = cPickle	
			except ImportError:
				import pickle

			dict = pickle.loads(args)

			version = dict["version"]
			for hook in self.hooks:
				my_hook = getattr(hook, self.method)
				my_hook(dict)
		except:
			import traceback
			traceback.print_exc()
			print "when calling : %s(%s)" % (self.method, args)


#####################################################################
# Storage class for the events
#####################################################################
class event_data:
	def __init__(self):
		self.callbacks = []

		# Set the temporary callbacks
		self.load_game = load_event(LOAD_GAME_EVENT, "load_game_hook")
		self.load_game.append(self)

		self.new_game = event(NEW_GAME_EVENT, "new_game_hook")
		self.new_game.append(self)

		# Set the various events
		self._set_callback(CMD_EAT_EVENT, "cmd_eat")
		self._set_callback(PLAYER_MOVE_EVENT, "player_move")
		self._set_callback(CMD_OPEN_EVENT, "cmd_open")
		self._set_callback(CMD_SEARCH_EVENT, "cmd_search")
		self._set_callback(CMD_FEELING_EVENT, "cmd_feeling")
		self._set_callback(CMD_GO_UP_EVENT, "cmd_go_up")
		self._set_callback(KILL_MONSTER_EVENT, "kill_monster")
		self._set_callback(BUILDING_COMMAND_EVENT, "building_command")
		self._set_callback(GENERATE_LEVEL_EVENT, "generate_level")
		self._set_callback(ENTER_LEVEL_EVENT, "enter_level")
		self._set_callback(LEAVE_LEVEL_EVENT, "leave_level")
		self._set_callback(GENERATE_WILDERNESS_EVENT, "generate_wilderness")
		self._set_callback(ENTER_WILDERNESS_EVENT, "enter_wilderness")
		self._set_callback(LEAVE_WILDERNESS_EVENT, "leave_wilderness")
		self._set_callback(GET_PLAYER_FLAGS_EVENT, "get_player_flags")
		self._set_callback(PLAYER_OUTFIT_EVENT, "player_outfit")
		self._set_callback(SENSE_INVENTORY_EVENT, "sense_inventory")
		self._set_callback(DESTROY_OBJECT_EVENT, "destroy_object")
		self._set_callback(WILDERNESS_INIT_EVENT, "wilderness_init")
		self._set_callback(CREATE_MONSTER_EVENT, "create_monster")
		self._set_callback(DELETE_MONSTER_EVENT, "delete_monster")
		self._set_callback(MONSTER_MOVE_EVENT, "monster_move")
		self._set_callback(COPY_MONSTER_EVENT, "copy_monster")
		self._set_callback(PLAYER_BIRTH_EVENT, "player_birth")

		self._set_callback(USE_SKILL_EVENT, "use_skill")
		self._set_callback(PROCESS_COMMAND_EVENT, "process_command")

		self._set_callback(GET_SCRIPT_WINDOW_LINE_EVENT, "get_script_window_line")

		self._set_special_callback(grid_event, PLAYER_SEARCH_GRID_EVENT, "player_search_grid")
		self._set_special_callback(grid_event, PLAYER_ENTER_GRID_EVENT, "player_enter_grid")

		# Clean the grid event lists when leaving the level/wilderness
		self.leave_level.append(self.player_search_grid)
		self.leave_level.append(self.player_enter_grid)
		self.leave_wilderness.append(self.player_search_grid)
		self.leave_wilderness.append(self.player_enter_grid)

		self._set_special_callback(save_event, SAVE_GAME_EVENT, "save_game")

		# Hook for saving the callbacks
		self.save_game.append(self)

	# Saving the callbacks
	def save_game_hook(self):
		return ("events", self.__dict__)

	# Loading the game
	def load_game_hook(self, dict):
		# Remove old hooks
		self.remove()
		# Load the data
		self.__dict__.update(dict["events"])

	# New game
	def new_game_hook(self, args):
		# Remove now useless hooks
		self.remove()

	# Remove all hooks
	def remove(self):
		remove_callback(LOAD_GAME_EVENT)
		remove_callback(NEW_GAME_EVENT)

	# Helper-function for setting the callbacks
	def _set_callback(self, number, name):
		the_event = event(number, name + "_hook")
		self.callbacks.append(the_event)
		setattr(self, name, the_event)

	# Helper-function for setting special callbacks
	def _set_special_callback(self, event, number, name):
		the_event = apply(event, (number, name + "_hook"))
		self.callbacks.append(the_event)
		setattr(self, name, the_event)

	def __getstate__(self):
		return self.callbacks

	def __setstate__(self, data):
		self.callbacks = data
		for event in self.callbacks:
			set_callback(event.eventnumber, event)

