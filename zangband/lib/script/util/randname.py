""" Functions for generating random names. """

from angband import random

#
# Random Name Generator
#
# Based on Javascript by Michael Hensley
# http://www.geocities.com/timessquare/castle/6274/
#
# Ported to Python code by Mark Howson
#
# XXX Needs to weed out some of the weaker names, and some names sound quite odd.
#
_syllable_table = {
	"dwarf" :
	[
	["B", "D", "F", "G", "Gl", "H", "K", "L", "M", "N", "R", "S", "T", "Th", "V" ],
	["a", "e", "i", "o", "oi", "u"],
	["bur", "fur", "gan", "gnus", "gnar", "li", "lin", "lir", "mli", "nar", "nus", "rin", "ran", "sin", "sil", "sur"]
	],
	"elf" :
	[
	["Al", "An", "Bal", "Bel", "Cal", "Cel", "El", "Elr", "Elv", "Eow", "Eär", "F", "Fal", "Fel", "Fin", "G",
	 "Gal", "Gel", "Gl", "Is", "Lan", "Leg", "Lóm", "N", "Nal", "Nel",  "S", "Sal", "Sel", "T", "Tal", "Tel",
	 "Thr", "Tin"],
	["a", "adrie", "ara", "e", "ebri", "ele", "ere", "i", "io", "ithra", "ilma", "il-Ga", "ili", "o", "orfi", "u", "y"],
	["l", "las", "lad", "ldor", "ldur", "linde", "lith", "mir", "n", "nd", "ndel", "ndil", "ndir", "nduil",
	 "ng", "mbor", "r", "rith", "ril", "riand", "rion", "s", "thien", "viel", "wen", "wyn"]
	],
	"gnome" :
	[
	["Aar", "An", "Ar", "As", "C", "H", "Han", "Har", "Hel", "Iir", "J", "Jan", "Jar", "K", "L", "M",
	 "Mar", "N", "Nik", "Os", "Ol", "P", "R", "S", "Sam", "San", "T", "Ter", "Tom", "Ul", "V", "W", "Y"],
	["a", "aa",  "ai", "e", "ei", "i", "o", "uo", "u", "uu"],
	["ron", "re", "la", "ki", "kseli", "ksi", "ku", "ja", "ta", "na", "namari", "neli", "nika", "nikki",
	 "nu", "nukka", "ka", "ko", "li", "kki", "rik", "po", "to", "pekka", "rjaana", "rjatta", "rjukka",
	 "la", "lla", "lli", "mo", "nni"]
	],
	"hobbit" :
	[
	["B", "Ber", "Br", "D", "Der", "Dr", "F", "Fr", "G", "H", "L", "Ler", "M", "Mer", "N", "P",	 
	 "Pr", "Per", "R", "S", "T", "W"],
	["a", "e", "i", "ia", "o", "oi", "u"],
	["bo", "ck", "decan", "degar", "do", "doc", "go", "grin", "lba", "lbo", "lda", "ldo", "lla",
	 "ll", "lo", "m", "mwise", "nac", "noc", "nwise", "p", "ppin", "pper", "tho", "to"]
	],
	"human" :
	[
	["Ab", "Ac", "Ad", "Af", "Agr", "Ast", "As", "Al", "Adw", "Adr", "Ar", "B", "Br", "C", "Cr",
	 "Ch", "Cad", "D", "Dr", "Dw", "Ed", "Eth", "Et", "Er", "El", "Eow", "F", "Fr", "G", "Gr",
	 "Gw", "Gal", "Gl", "H", "Ha", "Ib", "Jer", "K", "Ka", "Ked", "L", "Loth", "Lar", "Leg", "M",
	 "Mir", "N", "Nyd", "Ol", "Oc", "On", "P", "Pr", "R", "Rh", "S", "Sev", "T", "Tr", "Th", "V",
	 "Y", "Z", "W", "Wic"],
	["a", "ae", "au", "ao", "are", "ale", "ali", "ay", "ardo", "e", "ei", "ea", "eri", "era",
	 "ela", "eli", "enda", "erra", "i", "ia", "ie", "ire", "ira", "ila", "ili", "ira", "igo",
	 "o", "oa", "oi", "oe", "ore", "u", "y"],
	["a", "and", "b", "bwyn", "baen", "bard", "c", "ctred", "cred", "ch", "can", "d", "dan",
	 "don", "der", "dric", "dfrid", "dus", "f", "g", "gord", "gan", "l", "li", "lgrin", "lin",
	 "lith", "lath", "loth", "ld", "ldric", "ldan", "m", "mas", "mos", "mar", "mond", "n",
	 "nydd", "nidd", "nnon", "nwan", "nyth", "nad", "nn", "nnor", "nd", "p", "r", "ron", "rd",
	 "s", "sh", "seth", "sean", "t", "th", "tha", "tlan", "trem", "tram", "v", "vudd", "w", "wan",
	 "win", "wyn", "wyr", "wyr", "wyth"]
	],
	"orc" :
	[
	["B", "Er", "G", "Gr", "H", "P", "Pr", "R", "V", "Vr", "T", "Tr", "M", "Dr"],
	["a", "i", "o", "oo", "u", "ui"],
	["dash", "dish", "dush", "gar", "gor", "gdush", "lo", "gdish", "k", "lg", "nak", "rag",
	 "rbag", "rg", "rk", "ng", "nk", "rt", "ol", "urk", "shnak", "mog", "mak", "rak"]
	]
}

def _race_helper(s):
	""" Return a random entry in the list 's'. """
	return s[random.rand_int(len(s))]

def create_monster_name(race):
	""" Make a random name.
	<P>
	Create and return a random name for a monster (or, even a player), in the style of "race", where race
	is the style of name to create ; one of "dwarf", "elf", "gnome", "human", "hobbit", "orc". """
	try:
		str = ''
		for i in range(3):
			str = str + _race_helper(_syllable_table[race][i])
		return str
	except:
		return None

def test():
	print create_monster_name("dwarf")
	print create_monster_name("elf")
	print create_monster_name("gnome")
	print create_monster_name("human")
	print create_monster_name("hobbit")
	print create_monster_name("orc")

