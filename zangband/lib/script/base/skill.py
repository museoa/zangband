"""Generic code for the player skills"""

class skill:
	"""Abstract skill base class"""
	name = "abstract skill"
	level = 0
	mana = 0
	cost = 0
	fail = 0
	info = ""

	# Active (1) or passive (0) skill
	active = 1

	# Use the skill
	def use(self):
		pass


class racial_power(skill):
	"""Base class for racial powers"""

	name = "abstract racial power"
	active = 1
	difficulty = 0
	use_stat = 0

	def get_difficulty(self):
		return self.difficulty

	def use(self):
		from vars import player, ui
		from angband.io import get_check, PR_HP, PR_MANA, PW_PLAYER, PW_SPELL
		from angband.random import randint
		from angband.spells import take_hit

		# Not enough mana - use hp
		if player.csp < self.mana:
			use_hp = 1
		else:
			use_hp = 0

		# Power is not available yet
		if player.level < self.level:
			ui.msg_print("You need to attain level %d to use this power." % (self.level))
			energy_use = 0
			return 0
		# Too confused
		elif player.confused:
			ui.msg_print("You are too confused to use this power.")
			energy_use = 0
			return 0
		# Risk death?
		elif (use_hp and (player.chp < self.cost)):
			if not get_check("Really use the power in your weakened state? "):
				energy_use = 0
				return 0

		# Else attempt to do it!

		difficulty = self.get_difficulty()

		if player.stun:
			difficulty = difficulty + player.stun
		elif player.level > self.level:
			lev_adj = ((player.level - self.level) / 3)
			if (lev_adj > 10): lev_adj = 10
			difficulty = difficulty - lev_adj

		if difficulty < 5: difficulty = 5

		# take time and pay the price
		energy_use = 100

		if use_hp:
			take_hit((self.cost / 2) + randint(self.cost / 2), "concentrating too hard")
		else:
			player.csp = player.csp - ((self.cost / 2) + randint(self.cost / 2))

		# Redraw mana and hp
		player.redraw = player.redraw & (PR_HP | PR_MANA)

		# Window stuff
		player.window = player.window & (PW_PLAYER | PW_SPELL)

		# Success?
		if randint(player.get_stat_cur(self.use_stat)) >= ((difficulty / 2) + randint(difficulty / 2)):
			self.effect()
			return

		ui.msg_print("You've failed to concentrate hard enough.")
		return 0



class skills:
	"""Container for the players skills"""

	def __init__(self):
		self.data = []

	def append(self, skill):
		"""Add a skill"""
		self.data.append(skill)

	def add_skills(self, value):
		"""Add a list of skills"""
		self.data = self.data + value

	def __len__(self):
		return len(self.data)
	def __getitem__(self, key):
		return self.data[key]
	def __setitem__(self, key, value):
		self.data[key] = value
	def __getslice__(self, low, high):
		return self.data[low:high]
	def items(self):
		return self.data.items()
	def keys(self):
		return self.data.keys()
	def values(self):
		return self.data.values()

