#####################################################################
#
# base/quest.py
#
# Random ideas:
# - player wants a quest from the quest-giver
# - quest-giver has list of available quest-classes
# - quest-giver maps the list with the "available()" function
# - quest-giver selects a quest-class from the list
# - quest-giver shows the player the quest description
# - quest-giver asks the player if he wants the quest
# - quest-giver creates instance of the quest-class
# - quest creates an entry, adds hooks, ...
# - quest-giver assigns the quest instance to the player
# - quest-giver adds the instance to his list of running quests
# - quest-giver removes the quest-class from his list of available
#   quest-classes or turns an "availability" flag off
# - by changing the quest-giver of a quest at runtime you can create
#   quests that are rewarded by somebody else than the original
#   quest-giver
# 
# - quest-generators (create random quests)?
#   - should probably not be removed from the list of available quests
# 
#####################################################################


#####################################################################
# The base class for all quests
#####################################################################
class quest:
	# Unique ID of the quest
	id = 0

	# Name of the quest
	name = ""

	# Status of the quest
	status = None

	# Description
	description = """Place quest description here."""

	# Minimum player-level required for the quest
	min_level = 0

	# Quest-giver
	quest_giver = ""

	# XXX
	level = 0
	version = 0

	# Constructor
	def __init__(self):
		pass

	# Fail to complete the quest
	def fail(self):
		self.status = "failed"

	# Finish the quest
	def finish(self):
		self.status = "finished"

	# Assign the quest to the player
	def assign(self, quest_giver):
		# Note: Class-variable is transformed to normal member data
		self.status = "taken"

		# Set the quest-giver
		self.quest_giver = quest_giver

		# Assign the quest to the player
		from vars import player
		player.quests[self.id] = self

	# Enter a quest
	def enter(self):
		from vars import player, events
		# First add ourself to the event for level generation
		events.generate_level.append(self)
		player.leaving = 1
		player.inside_quest = 1
		player.oldpx = player.px
		player.oldpy = player.py
		from angband.event import *
		return 1

	# Complete a quest
	def complete(self):
		self.status = "completed"

	# Reward a quest
	def reward(self):
		self.status = "finished"

	# Do something with the quest
	def execute(self):
		# Quest completed? -> reward
		if self.status == "completed":
			self.reward()

	# Returns a list of strings containing the description/status message
	# Ex. ["Kill 15 hill orcs. 13 are already dead."]
	def get_description(self):
		return [""]

	# Availability check
	# - checks if the player is fit for the quest (level, race, class, ...)
	# - can prevent boring quests from random quest-generators
	# (Ex. one random "kill the monster" quest already active, don't start
	# another one right now)
	# returns true if available, false otherwise
	def available(self):
		from vars import player

		# Is the quest already assigned to the player?
		if player.quests.has_key(self.id):
			return 0
		# Has the player a high enough level?
		elif player.level < self.min_level:
			return 0
		else:
			return 1

	def unfinished(self):
		if self.status == "finished":
			return 0
		else:
			return 1

	# Comparision for sorting
	def __cmp__(self, other):
		if self.level < other.level:
			return -1
		elif self.level > other.level:
			return 1
		else:
			return 0


#####################################################################
# A quest that contains other sub-quests
#####################################################################
class plot(quest):
	# Constructor
	def __init__(self):
		# Inherited
		quest.__init__(self)

		# Sub-quests of the plot
		self.sub_quests = []

	def execute(self):
		# Sort the quests
		self.sub_quests.sort()

		# Check the list of quests for the next quest
		for quest in self.sub_quests:
			if quest.available():
				return quest

	def add_quest(self, new_quest):
		self.sub_quests.append(new_quest)


#####################################################################
# Storage for quests
#####################################################################
class quests_data:
	def __init__(self):
		# Dictionary of players quests
		self.quests = {}

	# Behave like a dictionary
	def __len__(self):
		return len(self.quests)
	def __getitem__(self, key):
		return self.quests[key]
	def __setitem__(self, key, value):
		self.quests[key] = value
	def __delitem__(self, key):
		del self.quests[key]
	def has_key(self, key):
		if self.quests.has_key(key):
			return 1
		else:
			return 0
	def keys(self):
		return self.quests.keys()
	def values(self):
		return self.quests.values()

