######################################################################
#
# r_info.py
#
# Parser for r_info.txt
#
######################################################################


from string import split, strip, atoi, joinfields, splitfields, lower, replace

class monster_class:
	color_name = { "d": "Black",
	               "w": "White",
	               "s": "Slate",
	               "o": "Orange",
	               "r": "Red",
	               "g": "Green",
	               "b": "Blue",
	               "u": "Umber",
	               "D": "L.Dark",
	               "W": "L.Slate",
	               "v": "Violet",
	               "y": "Yellow",
	               "R": "L.Red",
	               "G": "L.Green",
	               "B": "L.Blue",
	               "U": "L.Umber" }

	color_rgb =  { "d": "000000",
	               "w": "FFFFFF",
	               "s": "808080",
	               "o": "FF8000",
	               "r": "C00000",
	               "g": "008040",
	               "b": "0040FF",
	               "u": "804000",
	               "D": "606060",
	               "W": "C0C0C0",
	               "v": "FF00FF",
	               "y": "FFFF00",
	               "R": "FF0000",
	               "G": "00FF00",
	               "B": "00FFFF",
	               "U": "C08040" }

	type_name = { "ELDRITCH_HORROR" : "sanity-blasting",
				  "ANIMAL" : "natural",
				  "EVIL" : "evil",
				  "GOOD" : "good",
				  "UNDEAD" : "undead",
				  "DRAGON" : "dragon",
				  "DEMON" : "demon",
				  "GIANT" : "giant",
				  "TROLL" : "troll",
				  "ORC" : "orc",
				  "AMBERITE" : "Amberite" }

	abilities_name = { "INVISIBLE" : " is invisible.  ",
			           "COLD_BLOOD" : " is cold blooded.  ",
			           "EMPTY_MIND" : " is not detected by telepathy.  ",
			           "WEIRD_MIND" : " is rarely detected by telepathy.  ",
			           "MULTIPLY" : " breeds explosively.  ",
			           "REGENERATE" : " regenerates quickly.  " }

	blow_effect_name = {"" : "",
						"HURT":     "attack",
						"POISON":   "poison",
						"UN_BONUS": "disenchant",
						"UN_POWER": "drain charges",
						"EAT_GOLD": "steal gold",
						"EAT_ITEM": "steal items",
						"EAT_FOOD": "eat your food",
						"EAT_LITE": "absorb light",
						"ACID":     "shoot acid",
						"ELEC":     "electrocute",
						"FIRE":     "burn",
						"COLD":     "freeze",
						"BLIND":    "blind",
						"CONFUSE":  "confuse",
						"TERRIFY":  "terrify",
						"PARALYZE": "paralyze",
						"LOSE_STR": "reduce strength",
						"LOSE_INT": "reduce intelligence",
						"LOSE_WIS": "reduce wisdom",
						"LOSE_DEX": "reduce dexterity",
						"LOSE_CON": "reduce constitution",
						"LOSE_CHR": "reduce charisma",
						"LOSE_ALL": "reduce all stats",
						"SHATTER":  "shatter",
						"EXP_10":   "lower experience (by 10d6+)",
						"EXP_20":   "lower experience (by 20d6+)",
						"EXP_40":   "lower experience (by 40d6+)",
						"EXP_80":   "lower experience (by 80d6+)",
						"DISEASE":  "disease",
						"TIME":     "time" }

	blow_method_name = {"HIT": "hit",
						"TOUCH": "touch",
						"PUNCH": "punch",
						"KICK": "kick",
						"CLAW": "claw",
						"BITE": "bite",
						"STING": "sting",
						"BUTT": "butt",
						"CRUSH": "crush",
						"ENGULF": "engulf",
						"CHARGE": "charge",
						"CRAWL": "crawl on you",
						"DROOL": "drool on you",
						"SPIT": "spit",
						"EXPLODE": "explode",
						"GAZE": "gaze",
						"WAIL": "wail",
						"SPORE": "release spores",
						"BEG": "beg",
						"INSULT": "insult",
						"MOAN": "moan",
						"SHOW": "sing"}

	spell_names = {	"BA_ACID" : "produce acid balls",
					"BA_ELEC" : "produce lightning balls",
					"BA_FIRE" : "produce fire balls",
					"BA_COLD" : "produce frost balls",
					"BA_POIS" : "produce poison balls",
					"BA_NETH" : "produce nether balls",
					"BA_WATE" : "produce water balls",
					"BA_MANA" : "produce mana storms",
					"BA_DARK" : "produce darkness storms",
					"DRAIN_MANA" : "drain mana",
					"MIND_BLAST" : "cause mind blasting",
					"BRAIN_SMASH" : "cause brain smashing",
					"CAUSE_1" : "cause light wounds",
					"CAUSE_2" : "cause serious wounds",
					"CAUSE_3" : "cause critical wounds",
					"CAUSE_4" : "cause mortal wounds",
					"BO_ACID" : "produce acid bolts",
					"BO_ELEC" : "produce lightning bolts",
					"BO_FIRE" : "produce fire bolts",
					"BO_COLD" : "produce frost bolts",
					"BO_POIS" : "produce poison bolts",
					"BO_NETH" : "produce nether bolts",
					"BO_WATE" : "produce water bolts",
					"BO_MANA" : "produce mana bolts",
					"BO_PLAS" : "produce plasma bolts",
					"BO_ICEE" : "produce ice bolts",
					"MISSILE" : "produce magic missiles",
					"SCARE" : "terrify",
					"BLIND" : "blind",
					"CONF" : "confuse",
					"SLOW" : "slow",
					"HOLD" : "paralyze",
					"HASTE" : "haste-self",
					"HEAL" : "heal-self",
					"BLINK" : "blink-self",
					"TPORT" : "teleport-self",
					"TELE_TO" : "teleport to",
					"TELE_AWAY" : "teleport away",
					"TELE_LEVEL" : "teleport level",
					"DARKNESS" : "create darkness",
					"TRAPS" : "create traps",
					"FORGET" : "cause amnesia",
					"S_KIN" : "summon similar monsters",
					"S_HI_DEMON" : "summon greater demons",
					"S_MONSTER" : "summon a monster",
					"S_MONSTERS" : "summon monsters",
					"S_ANT" : "summon ants",
					"S_SPIDER" : "summon spiders",
					"S_HOUND" : "summon hounds",
					"S_HYDRA" : "summon hydras",
					"S_ANGEL" : "summon an angel",
					"S_DEMON" : "summon a demon",
					"S_UNDEAD" : "summon an undead",
					"S_DRAGON" : "summon a dragon",
					"S_HI_UNDEAD" : "summon greater undead",
					"S_HI_DRAGON" : "summon ancient dragons",
					"S_WRAITH" : "summon ring wraiths",
					"S_CYBER" : "summon Cyberdemons",
					"S_AMBERITES" : "summon Lords of Amber",
					"S_UNIQUE" : "summon unique monsters" }

	breath_name = { "BR_ACID": "acid",
		"BR_ELEC": "lightning",
		"BR_FIRE": "fire",
		"BR_COLD": "frost",
		"BR_POIS": "poison",
		"BR_NETH": "nether",
		"BR_LITE": "light",
		"BR_DARK": "darkness",
		"BR_CONF": "confusion",
		"BR_SOUN": "sound",
		"BR_CHAO": "chaos",
		"BR_DISE": "disenchantment",
		"BR_NEXU": "nexus",
		"BR_TIME": "time",
		"BR_INER": "inertia",
		"BR_GRAV": "gravity",
		"BR_SHAR": "shards",
		"BR_PLAS": "plasma",
		"BR_WALL": "force",
		"BR_MANA": "mana",
		"BR_NUKE": "toxic waste",
		"BR_DISI": "disintegration" }


	inate_attacks_name = { "SHRIEK" : "shriek for help",
						   "ROCKET" : "shoot a rocket",
						   "ARROW_1" : "fire arrows",
						   "ARROW_2" : "fire arrows",
						   "ARROW_3" : "fire missiles",
						   "ARROW_4" : "fire missiles" }


	def __init__(self):
		self.name = ""
		self.letter = ""
		self.color = ""
		self.speed = ""
		self.hp = ""
		self.aaf = 0
		self.ac = 0
		self.sleep = 0
		self.level = 0
		self.rarity = 0
		self.extra = 0
		self.exp = 0
		self.description = ""
		self.flags = []
		self.sflags = []
		self.sfreq = 0
		self.blows = []

	# Create a useable class name
	def get_class_name(self):
		class_name = replace(lower(self.name), ' ', '_')
		class_name = replace(class_name, ',', '')
		class_name = replace(class_name, "'", '')
		class_name = replace(class_name, '2-', 'two_')
		class_name = replace(class_name, '4-', 'four_')
		class_name = replace(class_name, '5-', 'five_')
		class_name = replace(class_name, '7-', 'seven_')
		class_name = replace(class_name, '9-', 'nine_')
		class_name = replace(class_name, '11-', 'eleven_')
		class_name = replace(class_name, '-', '_')
		return class_name

	def get_base_class(self):
		base_class = "monster"
		if "DEMON" in self.flags:
			base_class = "demon"
		elif "AMBERITE" in self.flags:
			base_class = "amberite"
		elif "TROLL" in self.flags:
			base_class = "troll"
		elif "ORC" in self.flags:
			base_class = "orc"
		elif "GIANT" in self.flags:
			base_class = "giant"
		elif "DRAGON" in self.flags:
			base_class = "dragon"
		return base_class

	def get_class_extend(self):
		class_extend = []
		if "UNDEAD" in self.flags:
			class_extend.append("undead")
		if self.sfreq > 0:
			class_extend.append("spellcaster")
		return class_extend


######################################################################
def parse(path="r_info.txt"):
	"""Parse a r_info.txt file and return a dictionary of
monster_class instances with r_idx as key."""

	r_info = open(path, 'r')

	monsters = {}
	number = 0

	for line in r_info.readlines():
		# Skip empty lines and comments
		if (line[0] != '\n') and (line[0] != '#'):
			result = split(line, ':')
			if result[0] == 'N':
				number = atoi(strip(result[1]))
				monsters[number] = monster_class()
				monster = monsters[number]
				monster.name = strip(result[2])
			elif result[0] == 'G':
				monster.letter = strip(result[1])
				monster.color  = strip(result[2])
			elif result[0] == 'I':
				monster.speed = atoi(strip(result[1]))
				monster.hp  = strip(result[2])
				monster.aaf = atoi(strip(result[3]))
				monster.ac  = atoi(strip(result[4]))
				monster.sleep = atoi(strip(result[5]))
			elif result[0] == 'W':
				monster.level  = atoi(strip(result[1]))
				monster.rarity = atoi(strip(result[2]))
				monster.extra  = atoi(strip(result[3]))
				monster.exp    = atoi(strip(result[4]))
			elif result[0] == 'B':
				while len(result) < 4:
					result.append("")
				blow = (strip(result[1]), strip(result[2]), strip(result[3]))
				monster.blows.append(blow)
			elif result[0] == 'F':
				flags = split(result[1], "|")
				for flag in flags:
					flag = strip(flag)
					if flag:
						monster.flags.append(flag)
			elif result[0] == 'S':
				flags = split(result[1], "|")
				for flag in flags:
					flag = strip(flag)
					if flag:
						if (flag[:5] == "1_IN_"):
							monster.sfreq = atoi(flag[5:10])
						else:
							monster.sflags.append(flag)
			elif result[0] == 'D':
				monster.description = monster.description + result[1]

	r_info.close()

	# Remove the player from the list
	del(monsters[0])

	# Return the result
	return monsters

