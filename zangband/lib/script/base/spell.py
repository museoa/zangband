#####################################################################
#
# base/spell.py
#
# Base code for the magic spells
#
#####################################################################


from base.skill import skill

# Stat Table (INT/WIS) -- Various things
adj_mag_stat = [
	0,       # 3
	0,       # 4
	0,       # 5
	0,       # 6
	0,       # 7
	1,       # 8
	1,       # 9
	1,       # 10
	1,       # 11
	1,       # 12
	1,       # 13
	1,       # 14
	2,       # 15
	2,       # 16
	2,       # 17
	3,       # 18/00-18/09
	3,       # 18/10-18/19
	3,       # 18/20-18/29
	3,       # 18/30-18/39
	3,       # 18/40-18/49
	4,       # 18/50-18/59
	4,       # 18/60-18/69
	5,       # 18/70-18/79
	6,       # 18/80-18/89
	7,       # 18/90-18/99
	8,       # 18/100-18/109
	9,       # 18/110-18/119
	10,      # 18/120-18/129
	11,      # 18/130-18/139
	12,      # 18/140-18/149
	13,      # 18/150-18/159
	14,      # 18/160-18/169
	15,      # 18/170-18/179
	16,      # 18/180-18/189
	17,      # 18/190-18/199
	18,      # 18/200-18/209
	19,      # 18/210-18/219
	20       # 18/220
]


# Stat Table (INT/WIS) -- Minimum failure rate (percentage)
adj_mag_fail = [
	99,     # 3
	99,     # 4
	99,     # 5
	99,     # 6
	99,     # 7
	50,     # 8
	30,     # 9
	20,     # 10
	15,     # 11
	12,     # 12
	11,     # 13
	10,     # 14
	9,      # 15
	8,      # 16
	7,      # 17
	6,      # 18/00-18/09
	6,      # 18/10-18/19
	5,      # 18/20-18/29
	5,      # 18/30-18/39
	5,      # 18/40-18/49
	4,      # 18/50-18/59
	4,      # 18/60-18/69
	4,      # 18/70-18/79
	4,      # 18/80-18/89
	3,      # 18/90-18/99
	3,      # 18/100-18/109
	2,      # 18/110-18/119
	2,      # 18/120-18/129
	2,      # 18/130-18/139
	2,      # 18/140-18/149
	1,      # 18/150-18/159
	1,      # 18/160-18/169
	1,      # 18/170-18/179
	1,      # 18/180-18/189
	1,      # 18/190-18/199
	0,      # 18/200-18/209
	0,      # 18/210-18/219
	0,      # 18/220
]


class spell(skill):
	active = 1
	common = 0
	uncommon = 0
	rare = 0

	# Spells use 1 mana by default
	mana = 1

	def use(self):
		self.cast()

	def effect(self):
		pass

	# Cast the spell
	# Returns 1 on successful cast and 0 when failing
	def cast(self):
		from vars import player

		if self.mana > player.csp:
			# XXX
			from vars import ui
			ui.msg_print("You don't have enough mana to cast this spell.")
			return

		# Extract the base spell failure rate
		chance = self.fail

		# Reduce failure rate by "effective" level adjustment
		chance = chance - (3 * (player.level - self.level))

		# Reduce failure rate by INT/WIS adjustment
		stat = player.get_stat_ind(player.p_class.spell_stat)
		chance = chance - 3 * (adj_mag_stat[stat] - 1)

		# Not enough mana to cast
		if self.mana > player.csp:
			chance = chance + 5 * (self.mana - player.csp)

		# Extract the minimum failure rate
		minfail = adj_mag_fail[player.get_stat_ind(player.p_class.spell_stat)]

		# Non mage/priest characters never get too good
		if minfail < player.p_class.min_fail:
			minfail = player.p_class.min_fail

		# Priest prayer penalty for "edged" weapons
		# XXX

		# Minimum failure rate
		if chance < minfail: chance = minfail

		# Stunning makes spells harder
		if player.stun > 50:
			chance = chance + 25
		elif player.stun:
			chance = chance + 15

		# Always a 5 percent chance of working
		if chance > 95: chance = 95

		# Failed spell ?
		from angband.random import rand_int
		if rand_int(100) < chance:
			# Reduce mana (but not below zero)
			player.csp = player.csp - self.mana
			if player.csp < 0: player.csp = 0

			from vars import ui
			from angband import io
			ui.msg_print("You failed to get the spell off!")
			ui.sound(io.SOUND_FAIL)
			return

		if self.effect() == -1:
			# Canceled spell
			pass
		else:
			# Reduce mana (but not below zero)
			player.csp = player.csp - self.mana
			if player.csp < 0: player.csp = 0

