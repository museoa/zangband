#####################################################################
#
# base/skill.py
#
# Generic code for the player skills
#
#####################################################################

from variable import events, debug, ui


#####################################################################
# Base class for the skills
#####################################################################
class skill:
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


#####################################################################
# Base class for racial powers
#####################################################################
class racial_power(skill):
	name = "abstract racial power"
	active = 1
	difficulty = 0
	use_stat = 0

	def get_difficulty(self):
		return self.difficulty

	def use(self):
		from variable import player
		from angband.io import msg_print, get_check, PR_HP, PR_MANA, PW_PLAYER, PW_SPELL
		from angband.random import randint
		from variable import ui
		from angband.spells import take_hit

		# Not enough mana - use hp
		if player.csp < self.mana:
			use_hp = 1
		else:
			use_hp = 0

		# Power is not available yet
		if player.level < self.level:
			msg_print("You need to attain level %d to use this power." % (self.level))
			energy_use = 0
			return 0
		# Too confused
		elif player.confused:
			msg_print("You are too confused to use this power.")
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
		if randint(player.stat_cur_get(self.use_stat)) >= ((difficulty / 2) + randint(difficulty / 2)):
			self.effect()
			return

		msg_print("You've failed to concentrate hard enough.")
		return 0



#####################################################################
#
# Class for the players skills
#
#####################################################################
class skills:
	#################################################################
	# Constructor
	#################################################################
	def __init__(self):
		self.data = []

		events.use_skill.append(self)

	#################################################################
	# Add a skill
	#################################################################
	def append(self, skill):
		self.data.append(skill)

	#################################################################
	# Add a list of skills
	#################################################################
	def add_skills(self, value):
		self.data = self.data + value

	def __len__(self):
		return len(self.data)
	def __getitem__(self, key):
		return self.data[key]
	def __setitem__(self, key, value):
		self.data[key] = value
	def items(self):
		return self.data.items()
	def keys(self):
		return self.data.keys()
	def values(self):
		return self.data.values()

	#################################################################
	# Use a skill
	#################################################################
	def use_skill_hook(self, args):
		skill = ui.select_skill(self.data)
		if skill:
			return skill[0].use()

