#####################################################################
#
# worlds/nexus/quests/slime.py
#
# "Slime mold" quest
#
#####################################################################

from variable import events, debug
from base.quest import quest
from angband.io import msg_print


# The class for the "slime mold" quest
class slime_mold_quest(quest):
	level = 1

	# Unique id
	id = "nexus.town1.slimemold quest"

	def __init__(self):
		quest.__init__(self)
		self.version = 1
		self.slimemolds = 0

	def assign(self, quest_giver):
		# Inherited
		quest.assign(self, quest_giver)

		# We add ourself to the eat event
		events.cmd_eat.append(self)

	def cmd_eat_hook(self, sval):
		debug.trace("slime_mold_quest.cmd_eat_hook(%s, %s)" % (self, sval))
		if sval == 36: # sval of the slime molds
			self.slimemolds = self.slimemolds + 1
			if self.slimemolds >= 5:
				msg_print("Slime mold quest finished!")
				self.complete()
				events.cmd_eat.remove(self)

	def get_description(self):
		debug.trace("slime_mold_quest.get_description(%s)" % (self))

		text = ["Slime molds (Danger level %d)" % (self.level)]

		if self.status == "taken": 
			text = text + ["You look hungry.  Go eat some slime molds!"]
		elif self.status == "finished":
			text = text + ["So the tales are true, you eat *anything*!"]
		elif self.status == "failed":
			text = text + ["How did you do *that*?"]

		return text

