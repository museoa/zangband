######################################################################
#
# r_info.py
#
# Parser for r_info.txt
#
######################################################################


from string import split, strip, atoi, joinfields, splitfields, lower

class monster_class:
	color_name = { "d": "xxx",
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

	abilities_name = { "INVISIBLE" : " is invisible.  ",
			           "COLD_BLOOD" : " is cold blooded.  ",
			           "EMPTY_MIND" : " is not detected by telepathy.  ",
			           "WEIRD_MIND" : " is rarely detected by telepathy.  ",
			           "MULTIPLY" : " breeds explosively.  ",
			           "REGENERATE" : " regenerates quickly.  " }

	def __init__(self):
		self.name = ""
		self.class_name = ""
		self.base_class = "monster"
		self.class_extend = []
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
		self.sex = "It"
		self.sfreq = 0
		self.blows = []


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

				# Create a useable class name
				monster.class_name = lower(joinfields(split(monster.name), '_'))
				monster.class_name = joinfields(splitfields(monster.class_name, ','), '')
				monster.class_name = joinfields(splitfields(monster.class_name, "2-"), 'two_')
				monster.class_name = joinfields(splitfields(monster.class_name, "4-"), 'four_')
				monster.class_name = joinfields(splitfields(monster.class_name, "5-"), 'five_')
				monster.class_name = joinfields(splitfields(monster.class_name, "7-"), 'seven_')
				monster.class_name = joinfields(splitfields(monster.class_name, "9-"), 'nine_')
				monster.class_name = joinfields(splitfields(monster.class_name, "11-"), 'eleven_')
				monster.class_name = joinfields(splitfields(monster.class_name, '-'), '_')
				monster.class_name = joinfields(splitfields(monster.class_name, "'"), '')

			if result[0] == 'G':
				monster.letter = strip(result[1])
				monster.color  = strip(result[2])#color[strip(result[2])]
			if result[0] == 'I':
				monster.speed = atoi(strip(result[1]))
				monster.hp  = strip(result[2])
				monster.aaf = atoi(strip(result[3]))
				monster.ac  = atoi(strip(result[4]))
				monster.sleep = atoi(strip(result[5]))
			if result[0] == 'W':
				monster.level  = atoi(strip(result[1]))
				monster.rarity = atoi(strip(result[2]))
				monster.extra  = atoi(strip(result[3]))
				monster.exp    = atoi(strip(result[4]))

			if result[0] == 'B':
				while len(result) < 4:
					result.append("")

				blow = (strip(result[1]), strip(result[2]), strip(result[3]))
				monster.blows.append(blow)

			if result[0] == 'F':
				flags = split(result[1], "|")
				for flag in flags:
					flag = strip(flag)
					monster.flags.append(flag)
					if (flags == "MALE"):
						monster.sex == "He"
					if (flags == "FEMALE"):
						monster.sex == "She"
			if result[0] == 'S':
				flags = split(result[1], "|")
				for flag in flags:
					flag = strip(flag)
					if (flag[:5] == "1_IN_"):
						monster.sfreq = atoi(flag[5:10])
					monster.sflags.append(flag)
			if result[0] == 'D':
				monster.description = monster.description + result[1]
				for i in xrange(2, len(result)):
					monster.description = monster.description + ':' + result[i]

	r_info.close()

	for key in monsters.keys():
		monster = monsters[key]

		if monster.flags.count("DEMON"):
			monster.base_class = "demon"
		if monster.flags.count("AMBERITE"):
			monster.base_class = "amberite"
		if monster.flags.count("TROLL"):
			monster.base_class = "troll"
		if monster.flags.count("ORC"):
			monster.base_class = "orc"
		if monster.flags.count("GIANT"):
			monster.base_class = "giant"
		if monster.flags.count("DRAGON"):
			monster.base_class = "dragon"

		if monster.flags.count("UNDEAD"):
			monster.class_extend.append("undead")
		if monster.sfreq > 0:
			monster.class_extend.append("spellcaster")

	# Remove the player from the list
	del(monsters[0])

	# Return the result
	return monsters

