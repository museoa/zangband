#####################################################################
#
# base/realm.py
#
# Base code for the magic realms
#
#####################################################################


from variable import events, ui
from base.skill import skill


class realm(skill):
	active = 0

	spell_distribution = [
		( 0,  0,  0,  0,  0,  0), #  0
		( 0,  4,  0,  0,  0,  0), #  1
		( 1,  8,  0,  0,  0,  0), #  2
		( 2, 12,  0,  0,  0,  0), #  3
		( 0, 16,  0,  1,  0,  0), #  4
		( 0, 16,  1,  2,  0,  1), #  5
		( 0, 16,  2,  4,  0,  1), #  6
		( 0, 16,  2,  6,  0,  2), #  7
		( 0, 16,  3,  8,  3,  8), #  8
		( 0, 16,  3, 10,  3, 10), #  9
		( 0, 16,  4, 12,  4, 12), # 10
		( 0, 16,  4, 14,  5, 14), # 11
		( 0, 16,  5, 16,  5, 16), # 12
		( 0, 16,  6, 18,  6, 18), # 13
		( 0, 16,  7, 20,  7, 20), # 14
		( 0, 16,  8, 21,  8, 21), # 15
		( 0, 16,  9, 22,  9, 22)] # 16

	spells = []

	def __init__(self):
		self.picks = 0
		self.player_picks = 0

	def get_common(self):
		return filter(lambda x: x.common == 1, self.spells)

	def get_uncommon(self):
		return filter(lambda x: x.uncommon == 1, self.spells)

	def get_rare(self):
		return filter(lambda x: x.rare == 1, self.spells)

	######################################################################
	# Randomly selected spells
	######################################################################
	def auto_pick_spells(self):
		def pick(spells, number):
			selection = []
			import random
			for i in range(1, number):
				if len(spells):
					spell = random.choice(spells)
					selection.append(spell)
					spells.remove(spell)
			return selection
			
		c1, c2, u1, u2, r1, r2 = self.spell_distribution[self.picks]

		common = pick(self.get_common(), c2)
		uncommon = pick(self.get_uncommon(), u2)
		rare = pick(self.get_rare(), r2)

		return common + uncommon + rare


	######################################################################
	# Select the spells for this realm
	######################################################################
	def select_spells(self):
		selection = []
		spells = []
		c1, c2, u1, u2, r1, r2 = self.spell_distribution[self.picks]

		# No spells to select
		if not self.spells or (c1 + c2 + u1 + u2 + r1 + r2 < 1):
			return []

		# Player-selected spells
		if c1 + u1 + r1 > 0:
			spells = ui.birth_select_spells(self)

		# Randomly selected spells
		spells = spells + self.auto_pick_spells()

		# Add the spells
		for spell in spells:
			selection.append(spell())

		return selection

