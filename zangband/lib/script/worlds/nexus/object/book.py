from angband import objects
from base.object import object_class
from variable import player

class spell_book_class(object_class):
	def object_browse_hook(self):
		try:
			# Create a list of spells in the book
			list = self.create_spell_list()

			from variable import gui
			gui.browse_book(list)
			return 1
		except:
			from traceback import print_exc
			print_exc()

	def object_cast_hook(self):
		try:
			# Create a list of spells in the book
			list = self.create_spell_list()

			# Count known spells
			i = 0
			for spell in list:
				if spell:
					i = i + 1

			# No known spells
			if not i:
				io.msg_format("You don't know any %ss in that book.", prayer)
				return 1

			from variable import gui
			spell = gui.cast_spell(list)
			if spell:
				spell.cast()
			return 1
		except:
			from traceback import print_exc
			print_exc()

	def create_spell_list(self):
		# Create a list of spells in the book
		list = []
		for spell in self.spells:
			p_spell = player.p_class.magic[self.realm].spells.get(spell, None)
			if p_spell:
				list.append(p_spell)
			else:
				list.append(None)
		return list



class life_book_class(spell_book_class):
	tval = objects.TV_LIFE_BOOK
	realm = "Life"

class life_book_1_class(life_book_class):
	sval = 0
	name = "book of common prayer"
	spells = ["Detect Evil",
	          "Cure Light Wounds"]

from base.object import objects
objects.announce(life_book_class)
objects.announce(life_book_1_class)

