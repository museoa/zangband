#####################################################################
#
# base/monster.py
#
# Base code for all monsters
#
#####################################################################

from variable import events, debug
from angband.monster import monster_type

#####################################################################
# Base class for monsters
#####################################################################
class monster(monster_type):
	version = 0
	current_num = 0 # Number of current monsters of this type

	subclasses = []

	def __init__(self, m_ptr=None):
		monster_type.__init__(self, m_ptr)
		if m_ptr:
			self.thisown = 0

		debug.trace("monster.__init__(%s, %s)" % (self, m_ptr))

	def move(self):
		pass

	def kill(self):
		pass

	def regenerate(self):
		if self.hp < self.max_hp:
			# Base regeneration or minimal regeneration rate
			frac = max(self.max_hp / 100, 1)

		# Some monsters regenerate quickly
		if "REGENERATE" in self.flags:
			frac = frac * 2

		# Regenerate
		self.hp = self.hp + frac

		# Do not over-regenerate
		if self.hp > self.max_hp:
			self.hp = self.max_hp


class humanoid(monster):
	pass

class human(humanoid):
	pass

class amberite(humanoid):
	pass

class troll(humanoid):
	pass

class orc(humanoid):
	pass

class giant(humanoid):
	pass

class dragon(monster):
	pass

class demon(monster):
	pass


class undead(monster):
	pass

class spellcaster(monster):
	pass


class factory:
	def __init__(self):
		self.__classes = []

	def select(self):
		import random
		return random.choice(self.__classes)

	def filter(self, func):
		return filter(func, self.subclasses)

	def append(self, value):
		self.__classes.append(value)

	def __len__(self):
		return len(self.__classes)
	def __getitem__(self, key):
		return self.__classes[key]
	def __setitem__(self, key, value):
		self.__classes[key] = value
	def __delitem__(self, key):
		del self.__classes[key]


#####################################################################
# Storage for the monsters
#####################################################################
class monster_data(factory):
	def __init__(self):
		debug.trace("monster_data.__init__(%s)" % (self))

		# Inherited
		factory.__init__(self)

		self.monsters = {}
		self.races = {}

		events.monster_move.append(self)
		events.kill_monster.append(self)
		events.create_monster.append(self)
		events.delete_monster.append(self)
		events.copy_monster.append(self)

	def create_monster_hook(self, index):
		debug.trace("monster_data.create_monster_hook(%s, %s)" % (self, index))

		from angband.monster import get_monster
		c_monster = get_monster(index)
		monster_class = self.races[c_monster.r_idx]
		if self.monsters.has_key(index):
			print "Create: Monster %d already exists" % (index)
		self.monsters[index] = monster_class(c_monster)

	def delete_monster_hook(self, index):
		debug.trace("monster_data.delete_monster_hook(%s, %s)" % (self, index))

		del self.monsters[index]

	def copy_monster_hook(self, args):
		i1, i2 = args
		debug.trace("monster_data.copy_monster_hook(%s, %s, %s)" % (self, i1, i2))

		if not self.monsters.has_key(i1):
			print "Copy: Monster %d doesn't exist" % (i1)
		if self.monsters.has_key(i2):
			print "Copy: Monster %d already exists" % (i2)
		self.monsters[i2] = self.monsters[i1]
		del self.monsters[i1]

	def monster_move_hook(self, index):
		debug.trace("monster_data.monster_move_hook(%s, %s)" % (self, index))
		return self.monsters[index].move()

	def kill_monster_hook(self, index):
		debug.trace("monster_data.kill_monster_hook(%s, %s)" % (self, index))
		return self.monsters[index].kill()

	# XXX
	def load_game_hook(self, dict):
		self.monsters = dict["monsters"]

	# XXX
	def save_game_hook(self):
		return ("monsters", self.monsters)

	def register_race(self, monster_class):
		self.races[monster_class.r_idx] = monster_class
		self.append(monster_class)

