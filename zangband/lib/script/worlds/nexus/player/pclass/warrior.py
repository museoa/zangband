from variable import events, player
from base.p_class import player_class_class
from angband import random

class warrior_class(player_class_class):
	name = "Warrior"
	number = 0
	def __init__(self):
		player_class_class.__init__(self,
				self.name,
				5, -2, -2, 2, 2, -1,
				25, 18, 18, 1, 14, 2, 70, 55,
				12, 7,  10, 0,  0,  0,  45, 45,
				9,  0, 20)
		events.destroy_object.append(self)
	def __str__(self):
		return self.name
	def get_player_flags_hook(self, args):
		if player.level > 29:
			player.resist_fear = 1
		return 1
	def destroy_object_hook(self, args):
		object, number = args
		from angband import objects
		from angband.player import PY_MAX_EXP
		the_object = objects.object_typePtr(object)
		if ((the_object.tval in (objects.TV_LIFE_BOOK, objects.TV_SORCERY_BOOK,
		     objects.TV_NATURE_BOOK, objects.TV_CHAOS_BOOK,
		     objects.TV_DEATH_BOOK, objects.TV_TRUMP_BOOK)) and
			(the_object.sval > 1) and (player.exp < PY_MAX_EXP)):
				tester_exp = player.max_exp / 20
				if tester_exp > 10000: tester_exp = 10000
				if the_object.sval < 3: tester_exp = tester_exp / 4
				if tester_exp < 1: tester_exp = 1

				from angband.io import msg_print
				msg_print("You feel more experienced.")
				player.gain_exp(tester_exp * number)
				return 1
	def sense_inventory_hook(self, args):
		# Good sensing
		if (0 != random.rand_int(9000 / (player.level * player.level + 40))):
			return
		# Heavy sensing
		return -1

	# Give the player the usual warrior outfit
	def player_outfit_hook(self, data):
		from base.object import objects
		ring = objects.create("ring of fear resistance")
		player.give(ring)
		sword = objects.create("broad sword")
		player.give(sword)
		armor = objects.create("chain mail")
		player.give(armor)
		return 1


# Make our class known
from base.birth import player_class_birth
player_class_birth.classes.append(warrior_class)

