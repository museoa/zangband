"""Generic code for player-races"""

import angband.prace

#####################################################################
class player_race(angband.prace.player_race):
	"""Base class for all player races"""

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
		from base.player import A_STR, A_INT, A_WIS, A_DEX, A_CON, A_CHR
		self.set_r_adj(A_STR, str_mod)
		self.set_r_adj(A_INT, int_mod)
		self.set_r_adj(A_WIS, wis_mod)
		self.set_r_adj(A_DEX, dex_mod)
		self.set_r_adj(A_CON, con_mod)
		self.set_r_adj(A_CHR, chr_mod)

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

		from vars import events
		events.player_outfit.append(self)
		events.get_player_flags.append(self)

	def player_outfit_hook(self, data):
		self.give_food_rations()
		self.give_torches()
		return 1

	def give_food_rations(self):
		import vars
		from angband.random import rand_range
		food = vars.objects.create("food ration")
		food.number = rand_range(2, 5)
		vars.player.give(food)

	def give_satisfy_hunger_scrolls(self):
		import vars
		from angband.random import rand_range
		scroll = vars.objects.create("scroll of satisfy hunger")
		scroll.number = rand_range(2, 5)
		vars.player.give(scroll)

	def give_torches(self):
		import vars
		from angband.random import rand_range
		lite = vars.objects.create("wooden torch")
		lite.number = rand_range(3, 7)
		lite.pval = rand_range(3, 7) * 500
		vars.player.give(lite)

	def get_player_flags_hook(self, args):
		return 1

	def __str__(self):
		return self.name

	# Make sure the constructor gets called when unpickling
	def __getinitargs__(self):
		return ()

