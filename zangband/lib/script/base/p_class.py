"""Generic code for player-classes"""

import angband.pclass


class player_class(angband.pclass.player_class):
	"""Base class for all player-classes"""

	realm1 = []
	realm2 = []

	title = ""
	str_mod = 0
	int_mod = 0
	wis_mod = 0
	dex_mod = 0
	con_mod = 0
	chr_mod = 0
	c_dis = 0
	c_dev = 0
	c_sav = 0
	c_stl = 0
	c_srh = 0
	c_fos = 0
	c_thn = 0
	c_thb = 0
	x_dis = 0
	x_dev = 0
	x_sav = 0
	x_stl = 0
	x_srh = 0
	x_fos = 0
	x_thn = 0
	x_thb = 0
	c_mhp = 0
	c_exp = 0
	pet_upkeep_div = 0

	def __init__(self):
		# Init the C data
		angband.pclass.player_class.__init__(self)

		# Set all values
		self.title = self.title
		from base.player import A_STR, A_INT, A_WIS, A_DEX, A_CON, A_CHR
		self.set_c_adj(A_STR, self.str_mod)
		self.set_c_adj(A_INT, self.int_mod)
		self.set_c_adj(A_WIS, self.wis_mod)
		self.set_c_adj(A_DEX, self.dex_mod)
		self.set_c_adj(A_CON, self.con_mod)
		self.set_c_adj(A_CHR, self.chr_mod)

		self.c_dis = self.c_dis
		self.c_dev = self.c_dev
		self.c_sav = self.c_sav
		self.c_stl = self.c_stl
		self.c_srh = self.c_srh
		self.c_fos = self.c_fos
		self.c_thn = self.c_thn
		self.c_thb = self.c_thb

		self.x_dis = self.x_dis
		self.x_dev = self.x_dev
		self.x_sav = self.x_sav
		self.x_stl = self.x_stl
		self.x_srh = self.x_srh
		self.x_fos = self.x_fos
		self.x_thn = self.x_thn
		self.x_thb = self.x_thb

		self.c_mhp = self.c_mhp
		self.c_exp = self.c_exp
		self.pet_upkeep_div = self.pet_upkeep_div

		from vars import events
		events.sense_inventory.append(self)
		events.get_player_flags.append(self)

	def birth(self):
		from vars import events
		events.player_outfit.append(self)
		return 1

	def get_beam_chance(self):
		from vars import player
		return player.level / 2

	# Make sure the constructor gets called when unpickling
	def __getinitargs__(self):
		return ()

	def get_player_flags_hook(self, args):
		return 1

	def sense_inventory_hook(self, args):
		pass

	def player_outfit_hook(self, data):
		return 1

	def __str__(self):
		return self.title

