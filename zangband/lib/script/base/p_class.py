#####################################################################
#
# base/p_class.py
#
# Generic code for player-classes
#
#####################################################################


from variable import events, player
from angband.pclass import player_class


#####################################################################
#
# Base class for all player-classes
#
#####################################################################
class player_class_class(player_class):
	realm1 = []
	realm2 = []

	def __init__(self, title = "", str_mod = 0, int_mod = 0, wis_mod = 0,
	             dex_mod = 0, con_mod = 0, chr_mod = 0,
	             c_dis = 0, c_dev = 0, c_sav = 0, c_stl = 0, c_srh = 0,
	             c_fos = 0, c_thn = 0, c_thb = 0, x_dis = 0, x_dev = 0,
	             x_sav = 0, x_stl = 0, x_srh = 0, x_fos = 0, x_thn = 0,
	             x_thb = 0, c_mhp = 0, c_exp = 0, pet_upkeep_div = 0):

		# Init the C data
		player_class.__init__(self)

		# Set all values
		self.title = title
		self.str_mod = str_mod
		self.int_mod = int_mod
		self.wis_mod = wis_mod
		self.dex_mod = dex_mod
		self.con_mod = con_mod
		self.chr_mod = chr_mod

		self.c_dis = c_dis
		self.c_dev = c_dev
		self.c_sav = c_sav
		self.c_stl = c_stl
		self.c_srh = c_srh
		self.c_fos = c_fos
		self.c_thn = c_thn
		self.c_thb = c_thb

		self.x_dis = x_dis
		self.x_dev = x_dev
		self.x_sav = x_sav
		self.x_stl = x_stl
		self.x_srh = x_srh
		self.x_fos = x_fos
		self.x_thn = x_thn
		self.x_thb = x_thb

		self.c_mhp = c_mhp
		self.c_exp = c_exp
		self.pet_upkeep_div = pet_upkeep_div

		events.sense_inventory.append(self)
		events.player_outfit.append(self)

	# Make sure the constructor gets called when unpickling
	def __getinitargs__(self):
		return ()

	def get_player_flags_hook(self, args):
		return 1

	def sense_inventory_hook(self, args):
		pass

	def player_outfit_hook(self, data):
		return 1


#####################################################################
#
# Player-class selection
#
#####################################################################
class player_class_data_class:
	def __init__(self):
		self.classes = []

	def append(self, p_class):
		self.classes.append(p_class)

	def get_player_class_hook(self, args):
		# Load all available player-classes
		from util.autorun import autorun_dir
		autorun_dir(player.world.directory, "player", "pclass")

		# Select the class
		from variable import gui
		selected = gui.birth.select_class(self.classes)

		# Restart character generation
		if not selected:
			return -1

		# Initialize the player-class
		player.p_class = apply(selected, ())
		player.pclass = selected.number

		# XXX !!! MEGA-HACK !!!
		import realmsc
		realmsc.cvar.mp_ptr = realmsc.get_player_magic(player.pclass)
		
		return 1

