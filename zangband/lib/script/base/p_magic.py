"""Generic code for player-magic"""

from base.p_class import player_class

class spellcaster(player_class):
	"""Generic class for the standard spellcasters"""

	def __init__(self):
		player_class.__init__(self)

		self.realms = {}
		self.picks = 0
		from vars import events
		events.process_command.append(self)

	def birth(self):
		if not player_class.birth(self): return

		return self.get_player_realms()

	def process_command_hook(self, command):
		if command == 'm':
			from vars import player, ui
			skill = ui.select_skill(player.skills)
			if skill:
				skill[0].use()
			return 1

	def check_picks(self):
		return 1

	def assign_realms(self):
		from vars import events, player

		# Clean up the realms
		for realm in self.realms.keys():
			if self.realms[realm].picks == 0:
				del self.realms[realm]

		# Let the player select his spells
		for realm in self.realms.values():
			player.skills.add_skills(realm.select_spells())

	def get_player_realms(self):
		if self.picks > 0:
			from vars import ui
			result = ui.birth_select_realms(self.realms, self.picks)
			if not result: return
			self.realms = result
			if self.check_picks():
				self.assign_realms()
		return 1

