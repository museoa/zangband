from string import split, strip, atoi, joinfields, splitfields, lower

import r_info

header = """from base.monster import *
from variable import monsters

monsters.append(humanoid)
monsters.append(human)
monsters.append(amberite)
monsters.append(troll)
monsters.append(orc)
monsters.append(giant)
monsters.append(dragon)
monsters.append(demon)


"""


monsters = r_info.parse()

monster_list = monsters.keys()
monster_list.sort()

outfile = open("monsters.py", 'w')

outfile.write(header)

length = 0

for key in monster_list:
	monster = monsters[key]

	class_name = monster.get_class_name()

	outfile.write('class %s(%s' % (class_name, monster.get_base_class()))

	for the_class in monster.get_class_extend():
		outfile.write(', %s' % the_class)

	outfile.write('):\n')

	outfile.write('\tname = "%s"\n' % monster.name)
	outfile.write('\tascii = "%s,%s"\n' % (monster.letter, monster.color))
	outfile.write('\tlevel = %d\n' % monster.level)
	outfile.write('\trarity = %d\n' % monster.rarity)
	outfile.write('\thp = "%s"\n' % monster.hp)
	outfile.write('\tspeed = %d\n' % monster.speed)
	outfile.write('\taaf = %d\n' % monster.aaf)
	outfile.write('\tac = %d\n' % monster.ac)
	outfile.write('\tsleep = %d\n' % monster.sleep)
	outfile.write('\texp = %d\n' % monster.exp)
	outfile.write('\tblows = %s\n' % monster.blows)
	outfile.write('\tflags = %s\n' % monster.flags)
	outfile.write('\tsfreq = %s\n' % monster.sfreq)
	outfile.write('\tsflags = %s\n' % monster.sflags)
	outfile.write('\tdescription = """%s"""\n' % monster.description)
	outfile.write('\n')

	outfile.write('monsters.register_race(%s)\n' % class_name)
	outfile.write('\n\n')

outfile.close()

