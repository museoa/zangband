#####################################################################
#
# base/birth.py
#
# Handling of the player birth
#
#####################################################################


from variable import events, debug

#####################################################################
#
#####################################################################
class player_race_birth:
	version = 0
	races = []

	def __init__(self):
		events.get_player_race.append(self)

	# Make sure the constructor gets called when unpickling
	def __getinitargs__(self):
		return ()

	def get_player_race_hook(self, args):
		from variable import player

		# Load all available player-races
		self.races = player.world.get_player_races()

		# Select the race
		from variable import ui
		selected = ui.birth.select_race(self.races)

		# Restart character generation
		if not selected:
			return -1

		player.race = apply(selected, ())
		player.prace = selected.number

		# Remove the events since we no longer need them
		events.get_player_race.remove(self)

		return 1


#####################################################################
#
#####################################################################
class player_class_birth:
	version = 0
	classes = []

	def __init__(self):
		events.get_player_class.append(self)

	# Make sure the constructor gets called when unpickling
	def __getinitargs__(self):
		return ()

	def get_player_class_hook(self, args):
		from variable import player

		# Load all available player-classes
		self.classes = player.world.get_player_classes()

		# Select the class
		from variable import ui
		selected = ui.birth.select_class(self.classes)

		# Restart character generation
		if not selected:
			return -1

		player.p_class = apply(selected, ())
		player.pclass = selected.number

		# XXX !!! MEGA-HACK !!!
		import realmsc
		realmsc.cvar.mp_ptr = realmsc.get_player_magic(player.pclass)

		# Remove the event since we no longer need it
		events.get_player_class.remove(self)

		return 1


#####################################################################
#
#####################################################################
class player_sex_birth:
	pass

