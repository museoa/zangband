from angband import objects
from base.object import object_class

class spell_book(object_class):
	spells = []

	def object_browse_hook(self):
		try:
			from vars import ui
			ui.display_spell_list(self.spells)
			return 1
		except:
			from traceback import print_exc
			print_exc()

	def object_cast_hook(self):
		from vars import ui
		try:
			# Count known spells
			i = 0
			for spell in self.spells:
				if spell: i = i + 1

			# No known spells
			if not i:
				ui.msg_print("You don't know any spells in that book.")
				return 1

			spell = ui.cast_spell(self.spells)
			if spell:
				spell.cast()
			return 1
		except:
			from traceback import print_exc
			print_exc()



from data.player.skills import life
from data.player.skills import sorcery
from data.player.skills import arcane

class life_book(spell_book):
	tval = objects.TV_LIFE_BOOK
	realm = life.life

class life_book_1(life_book):
	sval = 0
	name = "book of common prayer"
	spells = [life.detect_evil,
	          life.cure_light_wounds,
	          life.bless,
	          life.remove_fear]

class sorcery_book(spell_book):
	tval = objects.TV_SORCERY_BOOK
	realm = sorcery.sorcery

class arcane_book(spell_book):
	tval = objects.TV_ARCANE_BOOK
	realm = arcane.arcane

class arcane_book_1(arcane_book):
	sval = 0
	name = "Cantrips for Beginners"
	spells = [ arcane.zap,
	           arcane.detect_monsters,
	           arcane.blink,
	           arcane.light_area ]

class arcane_book_2(arcane_book):
	sval = 1
	name = "Minor Arcana"
	spells = [ arcane.detect_doors_and_traps ]

import vars
vars.objects.announce(life_book)
vars.objects.announce(life_book_1)
vars.objects.announce(sorcery_book)
vars.objects.announce(arcane_book)
vars.objects.announce(arcane_book_1)
vars.objects.announce(arcane_book_2)

