#####################################################################
#
# base/p_race.py
#
# Generic code for player-races
#
#####################################################################


import angband.prace

#####################################################################
# Base class for all player-races
#####################################################################
class player_race(angband.prace.player_race):
	version = 0

	def __init__(self, title = "", str_mod = 0, int_mod = 0, wis_mod = 0,
	             dex_mod = 0, con_mod = 0, chr_mod = 0,
	             r_dis = 0, r_dev = 0, r_sav = 0, r_stl = 0, r_srh = 0,
	             r_fos = 0, r_thn = 0, r_thb = 0, r_mhp = 0, r_exp = 0,
	             b_age = 0,	m_age = 0, m_b_ht = 0, m_m_ht = 0, m_b_wt = 0,
	             m_m_wt = 0, f_b_ht = 0, f_m_ht = 0, f_b_wt = 0, f_m_wt = 0,
	             infra = 0, choice = 0):

		# Init the C data
		angband.prace.player_race.__init__(self)

		# Set all values
		self.title = title
		self.str_mod = str_mod
		self.int_mod = int_mod
		self.wis_mod = wis_mod
		self.dex_mod = dex_mod
		self.con_mod = con_mod
		self.chr_mod = chr_mod

		self.r_dis = r_dis
		self.r_dev = r_dev
		self.r_sav = r_sav
		self.r_stl = r_stl
		self.r_srh = r_srh
		self.r_fos = r_fos
		self.r_thn = r_thn
		self.r_thb = r_thb

		self.r_mhp = r_mhp
		self.r_exp = r_exp

		self.b_age = b_age
		self.m_age = m_age
		self.m_b_ht = m_b_ht
		self.m_m_ht = m_m_ht
		self.m_b_wt = m_b_wt
		self.m_m_wt = m_m_wt
		self.f_b_ht = f_b_ht
		self.f_m_ht = f_m_ht
		self.f_b_wt = f_b_wt
		self.f_m_wt = f_m_wt

		self.infra = infra
		self.choice = choice

		from variable import events
		events.player_outfit.append(self)
		events.save_game.append(self)

	def player_outfit_hook(self, data):
		self.give_food_rations()
		self.give_torches()
		return 1

	def give_food_rations(self):
		from variable import player
		from base.object import objects
		from angband.random import rand_range
		food = objects.create("food ration")
		food.number = rand_range(2, 5)
		player.give(food)

	def give_satisfy_hunger_scrolls(self):
		from variable import player
		from base.object import objects
		from angband.random import rand_range
		scroll = objects.create("scroll of satisfy hunger")
		scroll.number = rand_range(2, 5)
		player.give(scroll)

	def give_torches(self):
		from variable import player
		from base.object import objects
		from angband.random import rand_range
		lite = objects.create("wooden torch")
		lite.number = rand_range(3, 7)
		lite.pval = rand_range(3, 7) * 500
		player.give(lite)

	# Make sure the constructor gets called when unpickling
	def __getinitargs__(self):
		return ()

	def get_player_flags_hook(self, args):
		return 1

	def save_game_hook(self):
		return ("race", self)

	# Make sure the constructor gets called when unpickling
	def __getinitargs__(self):
		return ()

