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
class event_class:
	def __init__(self, eventnumber, hookname):
		self.hooks = []
		self.version = 0
		self.method = hookname
		set_callback(eventnumber, self)
#	def __len__(self):
#		return len(self.hooks)
#	def __getitem(self, key):
#		return self.hooks[key]
#	def __setitem__(self, key, value):
#		self.hooks[key] = value
#	def __delitem__(self, key):
#		del self.hooks[key]

	# Add a new hook
	def append(self, hook, priority = 0):
		import bisect
		bisect.insort(self.hooks, (priority, hook))

	# Remove a hook
	def remove(self, hook):
		for priority, a_hook in self.hooks:
			if hook == a_hook:
				try:
					self.hooks.remove((priority, hook))
				except:
					import traceback
					traceback.print_exc()
					print "in event %s while removing hook %s" % (self.method, hook)
				break
		else:
			print "%s is not in event %s" % (hook, self.method)

	# Call the event and process all hooks
	def __call__(self, args):
		try:
			end_result = None

			# Go through all hooked-in objects
			for priority, hook in self.hooks:
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
class grid_event_class(event_class):
	def __init__(self, eventnumber, hookname):
		event_class.__init__(self, eventnumber, hookname)
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
class save_event_class(event_class):
	# Override the "call" method
	def __call__(self):
		try:
			dict = {}
			dict["version"] = 1
			for priority, hook in self.hooks:
				my_hook = getattr(hook, self.method)
				try:
					tag, data = my_hook()
				except:
					import traceback
					traceback.print_exc()
					print "when calling : %s" % (my_hook)
				dict[tag] = data
			import pickle
			return pickle.dumps(dict)
		except:
			import traceback
			traceback.print_exc()
			print "when calling : %s" % (self.method)


#####################################################################
# Load Python data from the saved game and send it to all hooked objects
#####################################################################
class load_event_class(event_class):
	# Override the "call" method
	def __call__(self, args):
		try:
			dict = {}
			import pickle
			dict = pickle.loads(args)

			version = dict["version"]
			for priority, hook in self.hooks:
				my_hook = getattr(hook, self.method)
				my_hook(dict)
		except:
			import traceback
			traceback.print_exc()
			print "when calling : %s(%s)" % (self.method, args)


#####################################################################
# Storage class for the events
#####################################################################
class event_data_class:
	def __init__(self):
		# Storage for the callbacks
		self.callbacks = {}

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
		self._set_callback(GET_PLAYER_RACE_EVENT, "get_player_race")
		self._set_callback(GET_PLAYER_CLASS_EVENT, "get_player_class")
		self._set_callback(GET_PLAYER_REALMS_EVENT, "get_player_realms")
		self._set_callback(GET_PLAYER_FLAGS_EVENT, "get_player_flags")
		self._set_callback(PLAYER_OUTFIT_EVENT, "player_outfit")
		self._set_callback(SENSE_INVENTORY_EVENT, "sense_inventory")
		self._set_callback(DESTROY_OBJECT_EVENT, "destroy_object")
		self._set_callback(OBJECT_CREATE_EVENT, "object_create")
		self._set_callback(OBJECT_LOAD_EVENT, "object_load")
		self._set_callback(WILDERNESS_INIT_EVENT, "wilderness_init")
		self._set_callback(GET_WORLD_EVENT, "get_world")
		self._set_callback(CREATE_MONSTER_EVENT, "create_monster")
		self._set_callback(DELETE_MONSTER_EVENT, "delete_monster")
		self._set_callback(MONSTER_MOVE_EVENT, "monster_move")
		self._set_callback(COPY_MONSTER_EVENT, "copy_monster")

		self._set_special_callback(grid_event_class, PLAYER_SEARCH_GRID_EVENT, "player_search_grid")
		self._set_special_callback(grid_event_class, PLAYER_ENTER_GRID_EVENT, "player_enter_grid")

		# Clean the grid event lists when leaving the level/wilderness
		self.leave_level.append(self.player_search_grid)
		self.leave_level.append(self.player_enter_grid)
		self.leave_wilderness.append(self.player_search_grid)
		self.leave_wilderness.append(self.player_enter_grid)

		self._set_special_callback(load_event_class, CALLBACKS_LOAD_EVENT, "load_game")
		self._set_special_callback(save_event_class, CALLBACKS_SAVE_EVENT, "save_game")

		# Hook in for loading and saving the callbacks
		self.load_game.append(self)
		self.save_game.append(self)

	# Loading the callbacks
	def load_game_hook(self, dict):
		self.__dict__.update(dict["callbacks"])
		for i in self.callbacks.keys():
			set_callback(i, self.callbacks[i])

	# Saving the callbacks
	def save_game_hook(self):
		return ("callbacks", self.__dict__)

	# Helper-function for setting the callbacks
	def _set_callback(self, event, name):
		the_event = event_class(event, name + "_hook")
		self.callbacks[event] = the_event
		setattr(self, name, the_event)

	# Helper-function for setting special callbacks
	def _set_special_callback(self, event_class, event, name):
		the_event = apply(event_class, (event, name + "_hook"))
		self.callbacks[event] = the_event
		setattr(self, name, the_event)

