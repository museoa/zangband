#####################################################################
#
# worlds/nexus/quests/q1.py
#
# "Thieves hideout" quest
#
#####################################################################

from variable import events, debug
from angband.io import msg_print
from base.quest import kill_all_monsters_quest

#################### thieves_quest ####################
# The class for the "thieves hideout" quest
class thieves_quest(kill_all_monsters_quest):
	name = "Thieves quest"
	level = 5
	start_pos = (2, 2)

	# Unique id
	id = "nexus.town1.thieves quest"

	# Init the quest
	def __init__(self):
		debug.trace("thieves_quest.__init__(%s)" % (self))

		# Inherited
		kill_all_monsters_quest.__init__(self)
		self.killed = 0

	# Assign the quest
	def assign(self, quest_giver):
		debug.trace("thieves_quest.assign(%s, %s)" % (self, quest_giver))

		# Inherited
		kill_all_monsters_quest.assign(self, quest_giver)

		self.build_entrance()

	# Build the quest entrance
	def build_entrance(self):
		from base.feature import quest_entrance
		from variable import worlds
		level = worlds["Nexus"].get_level("Town1")
		quest_entrance(level, "Thieves Den", self).build()

	def generate_level_hook(self, level):
		debug.trace("thieves_quest.generate_level_hook(%s, %s)" % (self, level))

		kill_all_monsters_quest.generate_level_hook(self, level)
		from angband import objects
		from variable import player
		from angband import cave
		from angband.player import INVEN_TOTAL
		player.dun_level = 5
		pos = 0

		# Load the map
		cave.load_layout("thieves.map")
		msg_print("You feel a vicious blow on your head.")

		# Take away all items and store them in another room
		for i in range(INVEN_TOTAL-1, -1, -1):
			an_object = objects.object_typePtr(player.inventory(i))
			if an_object.k_idx:
				new_object = objects.object_type()
				an_object.object_copy(new_object)
				player.inven_item_increase(i, -99)
				player.inven_item_optimize(i)
				new_object.marked = 0
				pos = pos + 1
				new_object.drop_near(-1, 1 + (pos / 13), 19 + (pos % 13))

		# Take away the gold too
		player.au = 0

		# Prepare the searching for the exit
		self.search_counter = 0
		events.player_search_grid.append(self, 1, 6)
		return 1	

	def player_move_hook(self, coords):
		debug.trace("thieves_quest.player_move_hook(%s, %s)" % (self, coords))

		if coords == (1, 7):
			# "Another cell"
			msg_print("Looks like another prison cell.")
			return 0
		elif coords in ((14, 19), (15, 19), (16, 19)):
			# The main hall has an alarm system
			from variable import player
			from base import terrain
			player.aggravate = 1
			msg_print("An alarm sounds.")

			# Close the door behind the player
			terrain.close_door((15, 20))
			msg_print("The door behind you closes.")

			# Open the chambers of the thieves
			terrain.open_door((12, 6), (12, 6), (18, 6), (12, 10), (18, 10),
			                  (12, 14), (18, 14), (12, 18), (18, 18))
			return 0
		elif coords == (4, 3):
			msg_print("There is a strong magical rune blocking the doorway.")
			return 1
		elif coords == (3, 3):
			msg_print("You see light shining through a small window in the cell door.")
			return 0

	# Print a message after entering the quest
	def cmd_feeling_hook(self, feeling):
		debug.trace("thieves_quest.cmd_feeling_hook(%s, %s)" % (self, feeling))

		kill_all_monsters_quest.cmd_feeling_hook(self, feeling)
		msg_print("You wake up in a prison cell.")
		msg_print("All your possessions have been stolen!")
		return 1

	# The cell door is magically locked
	def cmd_open_hook(self, coords):
		debug.trace("thieves_quest.cmd_open_hook(%s, %s)" % (self, coords))

		kill_all_monsters_quest.cmd_open_hook(self, coords)

		if coords == (4, 3):
			msg_print("The door seems to be magically locked.")
			return 1
		return 0

	# You can find an escape
	def player_search_grid_hook(self, coords):
		debug.trace("thieves_quest.player_search_grid_hook(%s, %s)" % (self, coords))

		kill_all_monsters_quest.player_search_grid_hook(self, coords)
		self.search_counter = self.search_counter + 1
		if self.search_counter == 5:
			msg_print("One of the stones in the wall is loose.")
			msg_print("You have found an escape tunnel!")
			from angband.cave import set_feat
			set_feat(1, 6, 1)
			# Remove the hook since we no longer need it
			events.player_search_grid.remove(self, 1, 6)
			return 1

	# Clean up when leaving the quest
	def cmd_go_up_hook(self, arg):
		debug.trace("thieves_quest.cmd_go_up_hook(%s, %s)" % (self, arg))

		kill_all_monsters_quest.cmd_go_up_hook(self, arg)
		msg_print("The entrance collapses behind you!")

		# Remove the entrance to the hideout
		from variable import worlds
		level = worlds["Nexus"].get_level("Town1")
		del level.features["Thieves Den"]

		# The quest is over
		if self.killed >= 10:
			self.complete()
		else:
			self.fail()

	def kill_monster_hook(self, r_idx):
		debug.trace("thieves_quest.kill_monster_hook(%s, %s)" % (self, r_idx))

		kill_all_monsters_quest.kill_monster_hook(self, r_idx)
		self.killed = self.killed + 1
		if self.killed >= 10:
			msg_print("You have killed all thieves.")
			self.complete()

	def reward(self):
		debug.trace("thieves_quest.reward(%s)" % (self))

		kill_all_monsters_quest.reward(self)

		if self.killed >= 10:
			# Give the player a reward
			from angband import objects
			reward = objects.object_type()
			reward.object_prep(42) # Longsword
			reward.to_h = 5
			reward.to_d = 3
			from variable import player
			player.carry(reward.this)
		else:
			# No reward for the lousy work
			pass

	def get_description(self):
		debug.trace("thieves_quest.get_description(%s)" % (self))

		text = ["Thieves Hideout (Danger level %d)" % (self.level)]

		if self.status == "taken": 
			text = text + ["There are thieves robbing my people!",
			               "They live in a small burrow outside the city walls",
			               "robbing people as they pass on the roads. They even",
			               "sneak inside the town to mug and pillage! Your task",
			               "is to go to the burrow and kill these ruffians."]
		elif self.status == "finished":
			text = text + ["Well done!  Accept this small gift from me."]
		elif self.status == "failed":
			text = text + ["I'm glad you escaped from the thieves hideout.",
		    	           "Too bad that some of them survived."]

		return text

