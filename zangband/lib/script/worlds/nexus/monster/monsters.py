from base.monster import *
from variable import monsters

monsters.append(humanoid)
monsters.append(human)
monsters.append(amberite)
monsters.append(troll)
monsters.append(orc)
monsters.append(giant)
monsters.append(dragon)
monsters.append(demon)


class filthy_street_urchin(monster):
	r_idx = "1"
	name = "Filthy street urchin"
	ascii = "t,D"
	level = 0
	rarity = 2
	hp = "1d4"
	speed = 110
	aaf = 4
	ac = 2
	sleep = 40
	exp = 0
	blows = [('BEG', '', ''), ('TOUCH', 'EAT_GOLD', '')]
	flags = ['MALE', 'EVIL', 'WILD_TOWN', 'RAND_25', 'FRIENDS', 'TAKE_ITEM', 'OPEN_DOOR', 'DROP_CORPSE', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """He looks squalid and thoroughly revolting.
"""

monsters.register_race(filthy_street_urchin)


class scrawny_cat(monster):
	r_idx = "2"
	name = "Scrawny cat"
	ascii = "f,U"
	level = 0
	rarity = 3
	hp = "1d2"
	speed = 110
	aaf = 30
	ac = 3
	sleep = 10
	exp = 0
	blows = [('CLAW', 'HURT', '1d1')]
	flags = ['RAND_25', 'WILD_TOWN', 'ANIMAL', 'DROP_CORPSE', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """A skinny little furball with sharp claws and a menacing look.
"""

monsters.register_race(scrawny_cat)


class sparrow(monster):
	r_idx = "3"
	name = "Sparrow"
	ascii = "B,U"
	level = 0
	rarity = 3
	hp = "1d1"
	speed = 110
	aaf = 30
	ac = 2
	sleep = 10
	exp = 0
	blows = [('BITE', 'HURT', '1d1')]
	flags = ['RAND_25', 'CAN_FLY', 'WILD_TOWN', 'WILD_GRASS', 'ANIMAL', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """Utterly harmless, except when angry.
"""

monsters.register_race(sparrow)


class chaffinch(monster):
	r_idx = "4"
	name = "Chaffinch"
	ascii = "B,r"
	level = 0
	rarity = 3
	hp = "1d1"
	speed = 110
	aaf = 30
	ac = 2
	sleep = 10
	exp = 0
	blows = [('BITE', 'HURT', '1d1')]
	flags = ['RAND_25', 'CAN_FLY', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_SWAMP1', 'WILD_GRASS', 'ANIMAL', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """Utterly harmless, except when angry.
"""

monsters.register_race(chaffinch)


class wild_rabbit(monster):
	r_idx = "5"
	name = "Wild rabbit"
	ascii = "r,U"
	level = 0
	rarity = 3
	hp = "1d2"
	speed = 110
	aaf = 30
	ac = 2
	sleep = 10
	exp = 0
	blows = [('BITE', 'HURT', '1d1')]
	flags = ['RAND_50', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_GRASS', 'ANIMAL', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It is not a carnivore, but will defend itself if you stray too 
close.
"""

monsters.register_race(wild_rabbit)


class woodsman(monster):
	r_idx = "6"
	name = "Woodsman"
	ascii = "p,g"
	level = 0
	rarity = 1
	hp = "3d3"
	speed = 110
	aaf = 10
	ac = 5
	sleep = 255
	exp = 0
	blows = [('HIT', 'HURT', '1d6')]
	flags = ['MALE', 'WILD_FOREST1', 'WILD_FOREST2', 'RAND_25', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_GOLD', 'DROP_60', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 0
	sflags = []
	description = """He has an axe with a sharp and strong edge.
"""

monsters.register_race(woodsman)


class scruffy_little_dog(monster):
	r_idx = "7"
	name = "Scruffy little dog"
	ascii = "C,U"
	level = 0
	rarity = 3
	hp = "1d3"
	speed = 110
	aaf = 20
	ac = 3
	sleep = 5
	exp = 0
	blows = [('BITE', 'HURT', '1d1')]
	flags = ['RAND_25', 'DROP_SKELETON', 'DROP_CORPSE', 'WILD_TOWN', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A thin flea-ridden mutt, growling as you get close.
"""

monsters.register_race(scruffy_little_dog)


class farmer_maggot(monster):
	r_idx = "8"
	name = "Farmer Maggot"
	ascii = "h,w"
	level = 0
	rarity = 3
	hp = "35d10"
	speed = 110
	aaf = 40
	ac = 10
	sleep = 3
	exp = 0
	blows = [('HIT', 'HURT', '1d3')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_CORPSE', 'FRIENDLY', 'FORCE_MAXHP', 'WILD_TOWN', 'ONLY_ITEM', 'DROP_90', 'DROP_GOOD', 'DROP_GREAT', 'OPEN_DOOR', 'BASH_DOOR', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """Before you appears a broad thick-set hobbit with a round red face. 
"What's wrong with old Maggot?" asked Pippin.  "He's a good friend to 
all the Brandybucks.  Of course he's a terror to trespassers, and keeps 
ferocious dogs -- but after all, folk down here are near the border and 
have to be more on their guard." 
"""

monsters.register_race(farmer_maggot)


class blubbering_idiot(monster):
	r_idx = "9"
	name = "Blubbering idiot"
	ascii = "t,W"
	level = 0
	rarity = 1
	hp = "1d2"
	speed = 110
	aaf = 6
	ac = 1
	sleep = 0
	exp = 0
	blows = [('DROOL', '', '')]
	flags = ['MALE', 'DROP_CORPSE', 'DROP_SKELETON', 'WILD_TOWN', 'RAND_25', 'TAKE_ITEM']
	sfreq = 0
	sflags = []
	description = """He tends to blubber a lot.
"""

monsters.register_race(blubbering_idiot)


class hobo(monster):
	r_idx = "10"
	name = "Hobo"
	ascii = "t,g"
	level = 0
	rarity = 1
	hp = "1d2"
	speed = 110
	aaf = 6
	ac = 1
	sleep = 0
	exp = 0
	blows = [('DROOL', '', '')]
	flags = ['MALE', 'DROP_SKELETON', 'DROP_CORPSE', 'WILD_TOWN', 'RAND_25', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 0
	sflags = []
	description = """Ugly doesn't begin to describe him.
"""

monsters.register_race(hobo)


class raving_lunatic(monster):
	r_idx = "11"
	name = "Raving lunatic"
	ascii = "t,G"
	level = 0
	rarity = 1
	hp = "3d4"
	speed = 120
	aaf = 6
	ac = 1
	sleep = 0
	exp = 0
	blows = [('DROOL', '', '')]
	flags = ['MALE', 'DROP_CORPSE', 'DROP_SKELETON', 'WILD_TOWN', 'RAND_25', 'TAKE_ITEM']
	sfreq = 0
	sflags = []
	description = """Drooling and comical, but then, what do you expect?
"""

monsters.register_race(raving_lunatic)


class pitiful_looking_beggar(monster):
	r_idx = "12"
	name = "Pitiful looking beggar"
	ascii = "t,U"
	level = 0
	rarity = 1
	hp = "1d4"
	speed = 110
	aaf = 10
	ac = 2
	sleep = 40
	exp = 0
	blows = [('BEG', '', '')]
	flags = ['MALE', 'DROP_SKELETON', 'DROP_CORPSE', 'RAND_25', 'WILD_TOWN', 'TAKE_ITEM', 'OPEN_DOOR']
	sfreq = 0
	sflags = []
	description = """You just can't help feeling sorry for him.
"""

monsters.register_race(pitiful_looking_beggar)


class mangy_looking_leper(monster):
	r_idx = "13"
	name = "Mangy looking leper"
	ascii = "t,u"
	level = 0
	rarity = 1
	hp = "1d1"
	speed = 110
	aaf = 10
	ac = 2
	sleep = 50
	exp = 0
	blows = [('BEG', '', ''), ('TOUCH', 'DISEASE', '')]
	flags = ['MALE', 'DROP_CORPSE', 'DROP_SKELETON', 'RAND_25', 'WILD_TOWN', 'TAKE_ITEM', 'OPEN_DOOR']
	sfreq = 0
	sflags = []
	description = """You feel it isn't safe to touch him.
"""

monsters.register_race(mangy_looking_leper)


class agent_of_black_market(monster):
	r_idx = "14"
	name = "Agent of black market"
	ascii = "t,D"
	level = 0
	rarity = 1
	hp = "2d8"
	speed = 110
	aaf = 10
	ac = 10
	sleep = 99
	exp = 0
	blows = [('HIT', 'HURT', '1d6'), ('TOUCH', 'EAT_ITEM', '')]
	flags = ['MALE', 'DROP_CORPSE', 'DROP_SKELETON', 'DROP_60', 'WILD_TOWN', 'WILD_SWAMP1', 'WILD_SWAMP2', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_MOUNT1', 'WILD_MOUNT2', 'WILD_GRASS', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 0
	sflags = []
	description = """He 'finds' new wares for the Black Market. From unwary adventurers... 
"""

monsters.register_race(agent_of_black_market)


class singing_happy_drunk(monster):
	r_idx = "15"
	name = "Singing, happy drunk"
	ascii = "t,y"
	level = 0
	rarity = 1
	hp = "2d3"
	speed = 110
	aaf = 10
	ac = 3
	sleep = 0
	exp = 0
	blows = [('BEG', '', '')]
	flags = ['MALE', 'RAND_50', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_GOLD', 'DROP_60', 'WILD_TOWN', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 0
	sflags = []
	description = """He makes you glad to be sober.
"""

monsters.register_race(singing_happy_drunk)


class aimless_looking_merchant(monster):
	r_idx = "16"
	name = "Aimless looking merchant"
	ascii = "t,o"
	level = 0
	rarity = 1
	hp = "3d3"
	speed = 110
	aaf = 10
	ac = 3
	sleep = 255
	exp = 0
	blows = [('HIT', 'HURT', '1d3')]
	flags = ['MALE', 'RAND_50', 'ONLY_GOLD', 'DROP_60', 'DROP_SKELETON', 'DROP_CORPSE', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_TOWN']
	sfreq = 0
	sflags = []
	description = """The typical ponce around town, with purse jingling, and looking for more 
amulets of adornment to buy.
"""

monsters.register_race(aimless_looking_merchant)


class mean_looking_mercenary(monster):
	r_idx = "17"
	name = "Mean looking mercenary"
	ascii = "t,R"
	level = 0
	rarity = 1
	hp = "4d8"
	speed = 110
	aaf = 10
	ac = 10
	sleep = 250
	exp = 0
	blows = [('HIT', 'HURT', '1d10')]
	flags = ['MALE', 'DROP_SKELETON', 'DROP_CORPSE', 'RAND_50', 'DROP_90', 'WILD_TOWN', 'WILD_FOREST1', 'WILD_FOREST2', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 0
	sflags = []
	description = """No job is too low for him.
"""

monsters.register_race(mean_looking_mercenary)


class battle_scarred_veteran(monster):
	r_idx = "18"
	name = "Battle scarred veteran"
	ascii = "t,r"
	level = 0
	rarity = 1
	hp = "6d8"
	speed = 110
	aaf = 10
	ac = 16
	sleep = 250
	exp = 0
	blows = [('HIT', 'HURT', '2d6')]
	flags = ['MALE', 'DROP_SKELETON', 'DROP_CORPSE', 'RAND_50', 'DROP_90', 'WILD_TOWN', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 0
	sflags = []
	description = """He doesn't take to strangers kindly.
"""

monsters.register_race(battle_scarred_veteran)


class martti_ihrasaari(giant):
	r_idx = "19"
	name = "Martti Ihrasaari"
	ascii = "P,w"
	level = 0
	rarity = 4
	hp = "35d15"
	speed = 109
	aaf = 50
	ac = 15
	sleep = 4
	exp = 0
	blows = [('HIT', 'HURT', '1d3')]
	flags = ['UNIQUE', 'MALE', 'GIANT', 'CAN_SPEAK', 'FRIENDLY', 'FORCE_MAXHP', 'DROP_CORPSE', 'WILD_TOWN', 'ONLY_ITEM', 'DROP_90', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """He weighs 127 kg. He is the president of some remote country. 
"""

monsters.register_race(martti_ihrasaari)


class grey_mold(monster):
	r_idx = "20"
	name = "Grey mold"
	ascii = "m,s"
	level = 1
	rarity = 1
	hp = "1d2"
	speed = 110
	aaf = 2
	ac = 2
	sleep = 0
	exp = 3
	blows = [('HIT', 'HURT', '1d4'), ('HIT', 'HURT', '1d4')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'DROP_CORPSE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A small strange growth.
"""

monsters.register_race(grey_mold)


class large_white_snake(monster):
	r_idx = "21"
	name = "Large white snake"
	ascii = "J,w"
	level = 1
	rarity = 1
	hp = "3d6"
	speed = 100
	aaf = 4
	ac = 17
	sleep = 99
	exp = 2
	blows = [('BITE', 'HURT', '1d1'), ('CRUSH', 'HURT', '1d1')]
	flags = ['RAND_50', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about eight feet long.
"""

monsters.register_race(large_white_snake)


class blinking_dot(monster, spellcaster):
	r_idx = "22"
	name = "Blinking dot"
	ascii = ",,s"
	level = 1
	rarity = 1
	hp = "1d2"
	speed = 110
	aaf = 2
	ac = 2
	sleep = 0
	exp = 1
	blows = [('SPORE', 'CONFUSE', '1d4')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'DROP_CORPSE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['BLINK']
	description = """Is it there or is it not? 
"""

monsters.register_race(blinking_dot)


class newt(monster):
	r_idx = "23"
	name = "Newt"
	ascii = "R,y"
	level = 1
	rarity = 1
	hp = "2d6"
	speed = 110
	aaf = 8
	ac = 8
	sleep = 30
	exp = 2
	blows = [('BITE', 'HURT', '1d3'), ('BITE', 'HURT', '1d3')]
	flags = ['WEIRD_MIND', 'CAN_SWIM', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_SWAMP1', 'WILD_SWAMP2', 'WILD_WASTE1', 'WILD_SHORE', 'WILD_MOUNT1', 'WILD_MOUNT2', 'WILD_TOWN', 'ANIMAL', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A small, harmless lizard.
"""

monsters.register_race(newt)


class giant_white_centipede(monster):
	r_idx = "24"
	name = "Giant white centipede"
	ascii = "c,w"
	level = 1
	rarity = 1
	hp = "3d5"
	speed = 110
	aaf = 7
	ac = 8
	sleep = 40
	exp = 2
	blows = [('BITE', 'HURT', '1d2'), ('STING', 'HURT', '1d2')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about four feet long and carnivorous.
"""

monsters.register_race(giant_white_centipede)


class white_icky_thing(monster):
	r_idx = "25"
	name = "White icky thing"
	ascii = "i,w"
	level = 1
	rarity = 1
	hp = "3d5"
	speed = 110
	aaf = 12
	ac = 6
	sleep = 10
	exp = 1
	blows = [('TOUCH', 'HURT', '1d2')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'CAN_SWIM', 'EMPTY_MIND', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It is a smallish, slimy, icky creature.
"""

monsters.register_race(white_icky_thing)


class clear_icky_thing(monster):
	r_idx = "26"
	name = "Clear icky thing"
	ascii = "i,B"
	level = 2
	rarity = 1
	hp = "2d5"
	speed = 110
	aaf = 12
	ac = 6
	sleep = 10
	exp = 1
	blows = [('TOUCH', 'HURT', '1d2')]
	flags = ['WILD_DUNGEON_01', 'ATTR_CLEAR', 'CAN_SWIM', 'RAND_50', 'RAND_25', 'INVISIBLE', 'EMPTY_MIND', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It is a smallish, slimy, icky, blobby creature.
"""

monsters.register_race(clear_icky_thing)


class giant_white_mouse(monster):
	r_idx = "27"
	name = "Giant white mouse"
	ascii = "r,w"
	level = 1
	rarity = 1
	hp = "1d3"
	speed = 110
	aaf = 8
	ac = 4
	sleep = 20
	exp = 1
	blows = [('BITE', 'HURT', '1d2')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'WILD_GRASS', 'MULTIPLY', 'CAN_SWIM', 'ANIMAL', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It is about three feet long with large teeth.
"""

monsters.register_race(giant_white_mouse)


class large_brown_snake(monster):
	r_idx = "28"
	name = "Large brown snake"
	ascii = "J,u"
	level = 1
	rarity = 1
	hp = "4d6"
	speed = 100
	aaf = 4
	ac = 15
	sleep = 99
	exp = 3
	blows = [('BITE', 'HURT', '1d3'), ('CRUSH', 'HURT', '1d4')]
	flags = ['WILD_DUNGEON_01', 'RAND_25', 'CAN_SWIM', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about eight feet long.
"""

monsters.register_race(large_brown_snake)


class small_kobold(monster):
	r_idx = "29"
	name = "Small kobold"
	ascii = "k,G"
	level = 1
	rarity = 1
	hp = "2d7"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 10
	exp = 5
	blows = [('HIT', 'HURT', '1d5')]
	flags = ['WILD_DUNGEON_01', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It is a squat and ugly humanoid figure.
"""

monsters.register_race(small_kobold)


class kobold(monster):
	r_idx = "30"
	name = "Kobold"
	ascii = "k,g"
	level = 3
	rarity = 1
	hp = "4d6"
	speed = 110
	aaf = 20
	ac = 12
	sleep = 10
	exp = 3
	blows = [('HIT', 'HURT', '1d8')]
	flags = ['WILD_DUNGEON_01', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It is a small, dog-headed humanoid.
"""

monsters.register_race(kobold)


class white_worm_mass(monster):
	r_idx = "31"
	name = "White worm mass"
	ascii = "w,w"
	level = 1
	rarity = 1
	hp = "3d5"
	speed = 100
	aaf = 7
	ac = 3
	sleep = 10
	exp = 2
	blows = [('CRAWL', 'POISON', '1d2')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'CAN_SWIM', 'STUPID', 'WEIRD_MIND', 'MULTIPLY', 'ANIMAL', 'IM_POIS', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a large slimy mass of worms.
"""

monsters.register_race(white_worm_mass)


class floating_eye(monster):
	r_idx = "32"
	name = "Floating eye"
	ascii = "e,b"
	level = 1
	rarity = 1
	hp = "1d2"
	speed = 110
	aaf = 2
	ac = 6
	sleep = 10
	exp = 1
	blows = [('GAZE', 'PARALYZE', '')]
	flags = ['WILD_DUNGEON_01', 'NEVER_MOVE', 'CAN_FLY', 'DROP_CORPSE', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A disembodied eye, floating a few feet above the ground.
"""

monsters.register_race(floating_eye)


class rock_lizard(monster):
	r_idx = "33"
	name = "Rock lizard"
	ascii = "R,U"
	level = 1
	rarity = 1
	hp = "3d4"
	speed = 110
	aaf = 20
	ac = 5
	sleep = 15
	exp = 2
	blows = [('BITE', 'HURT', '1d1')]
	flags = ['WILD_DUNGEON_01', 'ANIMAL', 'CAN_SWIM', 'WILD_MOUNT1', 'WILD_MOUNT2', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It is a small lizard with a hardened hide.
"""

monsters.register_race(rock_lizard)


class grid_bug(monster):
	r_idx = "34"
	name = "Grid bug"
	ascii = "I,v"
	level = 1
	rarity = 1
	hp = "2d4"
	speed = 110
	aaf = 10
	ac = 4
	sleep = 10
	exp = 2
	blows = [('BITE', 'ELEC', '1d4')]
	flags = ['WILD_DUNGEON_01', 'RAND_25', 'FRIENDS', 'CAN_FLY', 'STUPID', 'WEIRD_MIND', 'ANIMAL', 'NO_FEAR', 'IM_ELEC']
	sfreq = 0
	sflags = []
	description = """A strange electric bug. 
"""

monsters.register_race(grid_bug)


class jackal(monster):
	r_idx = "35"
	name = "Jackal"
	ascii = "C,U"
	level = 1
	rarity = 1
	hp = "1d4"
	speed = 110
	aaf = 10
	ac = 5
	sleep = 10
	exp = 1
	blows = [('BITE', 'HURT', '1d1')]
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_GRASS', 'FRIENDS', 'ANIMAL', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It is a yapping snarling dog, dangerous when in a pack.
"""

monsters.register_race(jackal)


class soldier_ant(monster):
	r_idx = "36"
	name = "Soldier ant"
	ascii = "a,u"
	level = 1
	rarity = 1
	hp = "2d5"
	speed = 110
	aaf = 10
	ac = 5
	sleep = 10
	exp = 3
	blows = [('BITE', 'HURT', '1d2')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'DROP_SKELETON', 'ANIMAL', 'WILD_DUNGEON_01', 'WILD_GRASS']
	sfreq = 0
	sflags = []
	description = """A large ant with powerful mandibles.
"""

monsters.register_race(soldier_ant)


class fruit_bat(monster):
	r_idx = "37"
	name = "Fruit bat"
	ascii = "b,v"
	level = 1
	rarity = 1
	hp = "2d2"
	speed = 120
	aaf = 20
	ac = 11
	sleep = 10
	exp = 1
	blows = [('BITE', 'HURT', '1d1')]
	flags = ['ANIMAL', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_SWAMP1', 'WILD_SWAMP2', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A fast-moving pest.
"""

monsters.register_race(fruit_bat)


class insect_swarm(monster):
	r_idx = "38"
	name = "Insect swarm"
	ascii = "I,u"
	level = 1
	rarity = 1
	hp = "2d2"
	speed = 120
	aaf = 20
	ac = 8
	sleep = 10
	exp = 1
	blows = [('BITE', 'HURT', '1d2'), ('STING', 'HURT', '1d2')]
	flags = ['ANIMAL', 'WEIRD_MIND', 'CAN_FLY', 'RAND_25', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_SWAMP1', 'WILD_SWAMP2']
	sfreq = 0
	sflags = []
	description = """A lone insect may be harmless, but there's a whole swarm of 
them here!
"""

monsters.register_race(insect_swarm)


class greater_hell_beast(monster, spellcaster):
	r_idx = "39"
	name = "Greater hell-beast"
	ascii = "U,s"
	level = 1
	rarity = 6
	hp = "15d75"
	speed = 115
	aaf = 10
	ac = 75
	sleep = 99
	exp = 25
	blows = [('GAZE', '', ''), ('GAZE', '', ''), ('CRUSH', '', '')]
	flags = ['WILD_DUNGEON_01', 'EVIL', 'IM_ACID', 'IM_ELEC', 'IM_FIRE', 'IM_POIS', 'IM_COLD', 'RES_NETH', 'RES_WATE', 'RES_PLAS', 'RES_DISE', 'RES_NEXU', 'KILL_WALL', 'FORCE_MAXHP', 'CAN_SWIM', 'DROP_CORPSE']
	sfreq = 9
	sflags = ['TPORT', 'BLINK', 'TELE_AWAY']
	description = """This unholy abomination will crush you. Flee while you can! 
"""

monsters.register_race(greater_hell_beast)


class shrieker_mushroom_patch(monster, spellcaster):
	r_idx = "40"
	name = "Shrieker mushroom patch"
	ascii = ",,R"
	level = 2
	rarity = 1
	hp = "1d1"
	speed = 110
	aaf = 4
	ac = 3
	sleep = 0
	exp = 1
	blows = []
	flags = ['FORCE_SLEEP', 'NEVER_MOVE', 'NEVER_BLOW', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'DROP_CORPSE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 4
	sflags = ['SHRIEK']
	description = """Yum!  These look quite tasty.
"""

monsters.register_race(shrieker_mushroom_patch)


class blubbering_icky_thing(monster):
	r_idx = "41"
	name = "Blubbering icky thing"
	ascii = "i,g"
	level = 2
	rarity = 1
	hp = "4d7"
	speed = 110
	aaf = 14
	ac = 6
	sleep = 10
	exp = 8
	blows = [('CRAWL', 'POISON', '1d4'), ('CRAWL', 'EAT_FOOD', ''), ('DROOL', '', ''), ('DROOL', '', '')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'DROP_90', 'CAN_SWIM', 'DROP_CORPSE', 'EMPTY_MIND', 'TAKE_ITEM', 'KILL_BODY', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It is a smallish, slimy, icky, hungry creature.
"""

monsters.register_race(blubbering_icky_thing)


class metallic_green_centipede(monster):
	r_idx = "42"
	name = "Metallic green centipede"
	ascii = "c,G"
	level = 2
	rarity = 1
	hp = "3d5"
	speed = 120
	aaf = 5
	ac = 10
	sleep = 10
	exp = 3
	blows = [('CRAWL', 'HURT', '1d1')]
	flags = ['RAND_50', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_FOREST2', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about four feet long and carnivorous.
"""

monsters.register_race(metallic_green_centipede)


class novice_warrior(monster):
	r_idx = "43"
	name = "Novice warrior"
	ascii = "p,U"
	level = 2
	rarity = 1
	hp = "7d4"
	speed = 110
	aaf = 20
	ac = 13
	sleep = 5
	exp = 6
	blows = [('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '1d6')]
	flags = ['MALE', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 0
	sflags = []
	description = """He looks inexperienced but tough.
"""

monsters.register_race(novice_warrior)


class novice_rogue(monster):
	r_idx = "44"
	name = "Novice rogue"
	ascii = "p,D"
	level = 2
	rarity = 1
	hp = "6d4"
	speed = 110
	aaf = 20
	ac = 11
	sleep = 5
	exp = 6
	blows = [('HIT', 'HURT', '1d6'), ('TOUCH', 'EAT_GOLD', '')]
	flags = ['MALE', 'DROP_60', 'DROP_SKELETON', 'DROP_CORPSE', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_GRASS', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A rather shifty individual.
"""

monsters.register_race(novice_rogue)


class novice_priest(monster, spellcaster):
	r_idx = "45"
	name = "Novice priest"
	ascii = "p,B"
	level = 2
	rarity = 1
	hp = "6d4"
	speed = 110
	aaf = 20
	ac = 9
	sleep = 10
	exp = 7
	blows = [('HIT', 'HURT', '1d5')]
	flags = ['MALE', 'FORCE_SLEEP', 'GOOD', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'DROP_CORPSE', 'DROP_60', 'WILD_GRASS', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 12
	sflags = ['HEAL', 'SCARE', 'CAUSE_1']
	description = """He is tripping over his priestly robes.
"""

monsters.register_race(novice_priest)


class novice_mage(monster, spellcaster):
	r_idx = "46"
	name = "Novice mage"
	ascii = "p,R"
	level = 2
	rarity = 1
	hp = "5d4"
	speed = 110
	aaf = 20
	ac = 7
	sleep = 5
	exp = 7
	blows = [('HIT', 'HURT', '1d4')]
	flags = ['MALE', 'FORCE_SLEEP', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'DROP_CORPSE', 'DROP_60', 'WILD_GRASS', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 12
	sflags = ['BLINK', 'BLIND', 'CONF', 'MISSILE']
	description = """He is leaving behind a trail of dropped spell components.
"""

monsters.register_race(novice_mage)


class yellow_mushroom_patch(monster):
	r_idx = "47"
	name = "Yellow mushroom patch"
	ascii = ",,y"
	level = 2
	rarity = 1
	hp = "1d1"
	speed = 110
	aaf = 2
	ac = 3
	sleep = 0
	exp = 2
	blows = [('SPORE', 'TERRIFY', '1d6')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """Yum!  It looks quite tasty.
"""

monsters.register_race(yellow_mushroom_patch)


class white_jelly(monster):
	r_idx = "48"
	name = "White jelly"
	ascii = "j,w"
	level = 2
	rarity = 1
	hp = "6d8"
	speed = 120
	aaf = 2
	ac = 1
	sleep = 99
	exp = 10
	blows = [('TOUCH', 'POISON', '1d2')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """Its a large pile of white flesh.
"""

monsters.register_race(white_jelly)


class giant_black_ant(monster):
	r_idx = "49"
	name = "Giant black ant"
	ascii = "a,D"
	level = 2
	rarity = 1
	hp = "2d8"
	speed = 110
	aaf = 8
	ac = 12
	sleep = 80
	exp = 8
	blows = [('BITE', 'HURT', '1d4')]
	flags = ['RAND_25', 'WEIRD_MIND', 'DROP_SKELETON', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_GRASS', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about three feet long.
"""

monsters.register_race(giant_black_ant)


class salamander(monster):
	r_idx = "50"
	name = "Salamander"
	ascii = "R,o"
	level = 2
	rarity = 1
	hp = "3d7"
	speed = 110
	aaf = 8
	ac = 12
	sleep = 80
	exp = 10
	blows = [('BITE', 'FIRE', '1d3')]
	flags = ['RAND_25', 'CAN_SWIM', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'DROP_CORPSE', 'ANIMAL', 'IM_FIRE']
	sfreq = 0
	sflags = []
	description = """A small black and orange lizard.
"""

monsters.register_race(salamander)


class white_harpy(monster):
	r_idx = "51"
	name = "White harpy"
	ascii = "H,w"
	level = 2
	rarity = 1
	hp = "2d5"
	speed = 110
	aaf = 16
	ac = 18
	sleep = 10
	exp = 5
	blows = [('CLAW', 'HURT', '1d1'), ('CLAW', 'HURT', '1d1'), ('BITE', 'HURT', '1d2')]
	flags = ['FEMALE', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_MOUNT2', 'RAND_50', 'DROP_CORPSE', 'ANIMAL', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A flying, screeching bird with a woman's face.
"""

monsters.register_race(white_harpy)


class blue_yeek(monster):
	r_idx = "52"
	name = "Blue yeek"
	ascii = "y,b"
	level = 2
	rarity = 1
	hp = "2d6"
	speed = 110
	aaf = 18
	ac = 15
	sleep = 10
	exp = 4
	blows = [('HIT', 'HURT', '1d5')]
	flags = ['DROP_60', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'ANIMAL', 'IM_ACID']
	sfreq = 0
	sflags = []
	description = """A small humanoid figure.
"""

monsters.register_race(blue_yeek)


class grip_farmer_maggots_dog(monster):
	r_idx = "53"
	name = "Grip, Farmer Maggot's dog"
	ascii = "C,w"
	level = 2
	rarity = 2
	hp = "4d6"
	speed = 120
	aaf = 30
	ac = 17
	sleep = 0
	exp = 30
	blows = [('BITE', 'HURT', '1d4')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'RAND_25', 'DROP_CORPSE', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A rather vicious dog belonging to Farmer Maggot.  It thinks you are 
stealing mushrooms.
"""

monsters.register_race(grip_farmer_maggots_dog)


class wolf_farmer_maggots_dog(monster):
	r_idx = "54"
	name = "Wolf, Farmer Maggot's dog"
	ascii = "C,w"
	level = 2
	rarity = 2
	hp = "4d6"
	speed = 120
	aaf = 30
	ac = 17
	sleep = 0
	exp = 30
	blows = [('BITE', 'HURT', '1d4')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'RAND_25', 'DROP_CORPSE', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A rather vicious dog belonging to Farmer Maggot.  It thinks you are 
stealing mushrooms.
"""

monsters.register_race(wolf_farmer_maggots_dog)


class fang_farmer_maggots_dog(monster):
	r_idx = "55"
	name = "Fang, Farmer Maggot's dog"
	ascii = "C,w"
	level = 2
	rarity = 2
	hp = "4d6"
	speed = 120
	aaf = 30
	ac = 17
	sleep = 0
	exp = 30
	blows = [('BITE', 'HURT', '1d4')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'RAND_25', 'DROP_CORPSE', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A rather vicious dog belonging to Farmer Maggot.  It thinks you are 
stealing mushrooms.
"""

monsters.register_race(fang_farmer_maggots_dog)


class giant_green_frog(monster):
	r_idx = "56"
	name = "Giant green frog"
	ascii = "R,g"
	level = 2
	rarity = 1
	hp = "2d8"
	speed = 110
	aaf = 12
	ac = 8
	sleep = 30
	exp = 6
	blows = [('BITE', 'HURT', '1d3')]
	flags = ['RAND_25', 'WILD_SHORE', 'WILD_SWAMP1', 'WILD_SWAMP2', 'DROP_CORPSE', 'BASH_DOOR', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is as big as a wolf.
"""

monsters.register_race(giant_green_frog)


class freesia(monster):
	r_idx = "57"
	name = "Freesia"
	ascii = "f,u"
	level = 2
	rarity = 1
	hp = "4d6"
	speed = 120
	aaf = 30
	ac = 17
	sleep = 0
	exp = 32
	blows = [('CLAW', 'HURT', '1d2'), ('CLAW', 'HURT', '1d3')]
	flags = ['UNIQUE', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'DROP_SKELETON', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A striped housecat who enjoys hunting. 
"""

monsters.register_race(freesia)


class green_worm_mass(monster):
	r_idx = "58"
	name = "Green worm mass"
	ascii = "w,g"
	level = 2
	rarity = 1
	hp = "4d5"
	speed = 100
	aaf = 7
	ac = 3
	sleep = 10
	exp = 3
	blows = [('CRAWL', 'ACID', '1d3')]
	flags = ['RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'STUPID', 'WEIRD_MIND', 'MULTIPLY', 'ANIMAL', 'IM_ACID', 'CAN_SWIM', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a large slimy mass of worms.
"""

monsters.register_race(green_worm_mass)


class large_yellow_snake(monster):
	r_idx = "59"
	name = "Large yellow snake"
	ascii = "J,y"
	level = 2
	rarity = 1
	hp = "3d7"
	speed = 100
	aaf = 5
	ac = 20
	sleep = 75
	exp = 9
	blows = [('BITE', 'HURT', '1d4'), ('CRUSH', 'HURT', '1d6')]
	flags = ['RAND_25', 'CAN_SWIM', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'WILD_FOREST1', 'WILD_GRASS', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about ten feet long.
"""

monsters.register_race(large_yellow_snake)


class cave_spider(monster):
	r_idx = "60"
	name = "Cave spider"
	ascii = "S,D"
	level = 2
	rarity = 1
	hp = "2d4"
	speed = 120
	aaf = 8
	ac = 14
	sleep = 80
	exp = 7
	blows = [('BITE', 'HURT', '1d4')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'WEIRD_MIND', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """It is a black spider that moves in fits and starts.
"""

monsters.register_race(cave_spider)


class crow(monster):
	r_idx = "61"
	name = "Crow"
	ascii = "B,s"
	level = 2
	rarity = 2
	hp = "3d5"
	speed = 120
	aaf = 40
	ac = 15
	sleep = 0
	exp = 8
	blows = [('BITE', 'HURT', '1d3'), ('BITE', 'HURT', '1d3')]
	flags = ['ANIMAL', 'CAN_FLY', 'DROP_CORPSE', 'FORCE_MAXHP', 'WILD_GRASS', 'WILD_FOREST1', 'WILD_FOREST2']
	sfreq = 0
	sflags = []
	description = """It is a hooded crow, gray except for the black wings and head.
"""

monsters.register_race(crow)


class wild_cat(monster):
	r_idx = "62"
	name = "Wild cat"
	ascii = "f,U"
	level = 2
	rarity = 2
	hp = "2d6"
	speed = 120
	aaf = 40
	ac = 15
	sleep = 0
	exp = 8
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3')]
	flags = ['BASH_DOOR', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'WILD_MOUNT1']
	sfreq = 0
	sflags = []
	description = """A larger than normal feline, hissing loudly.  Its velvet claws conceal a 
fistful of needles.
"""

monsters.register_race(wild_cat)


class smeagol(monster):
	r_idx = "63"
	name = "Smeagol"
	ascii = "h,u"
	level = 3
	rarity = 2
	hp = "4d5"
	speed = 130
	aaf = 20
	ac = 24
	sleep = 5
	exp = 16
	blows = [('TOUCH', 'EAT_GOLD', '')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SWIM', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_MAXHP', 'CAN_SPEAK', 'SMART', 'RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_MOUNT1', 'ONLY_ITEM', 'DROP_90', 'DROP_GOOD', 'DROP_GREAT', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'INVISIBLE', 'EVIL']
	sfreq = 0
	sflags = []
	description = """"Down the face of a precipice, sheer and almost smooth it seemed in the pale 
moonlight, a small black shape was moving with its thin limbs splayed out. 
Maybe its soft clinging hands and toes were used, but it looked as if it was 
just creeping down on sticky pads, like some large prowling thing of 
insect-kind.  And it was coming down head first, as if it was smelling its 
way.  Now and again it lifted its head slowly, turning it right back on its 
long skinny neck, and the hobbits caught a glimpse of two small pale gleaming 
lights, its eyes that blinked at the moon for a moment and then were quickly 
lidded again." 
"""

monsters.register_race(smeagol)


class green_ooze(monster):
	r_idx = "64"
	name = "Green ooze"
	ascii = "j,g"
	level = 3
	rarity = 2
	hp = "3d3"
	speed = 120
	aaf = 8
	ac = 12
	sleep = 80
	exp = 4
	blows = [('CRAWL', 'ACID', '1d3')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'DROP_90', 'STUPID', 'EMPTY_MIND', 'IM_ACID', 'IM_POIS', 'CAN_SWIM', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It's green and it's oozing.
"""

monsters.register_race(green_ooze)


class poltergeist(monster, undead, spellcaster):
	r_idx = "65"
	name = "Poltergeist"
	ascii = "G,s"
	level = 3
	rarity = 1
	hp = "2d4"
	speed = 130
	aaf = 8
	ac = 13
	sleep = 10
	exp = 8
	blows = [('TOUCH', 'TERRIFY', '')]
	flags = ['RAND_50', 'RAND_25', 'CAN_FLY', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'TAKE_ITEM', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['BLINK']
	description = """It is a ghastly, ghostly form.
"""

monsters.register_race(poltergeist)


class yellow_jelly(monster, spellcaster):
	r_idx = "66"
	name = "Yellow jelly"
	ascii = "j,y"
	level = 3
	rarity = 1
	hp = "8d8"
	speed = 120
	aaf = 2
	ac = 1
	sleep = 99
	exp = 12
	blows = [('TOUCH', 'POISON', '1d3')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 15
	sflags = ['DRAIN_MANA']
	description = """It's a large pile of yellow flesh.
"""

monsters.register_race(yellow_jelly)


class metallic_blue_centipede(monster):
	r_idx = "67"
	name = "Metallic blue centipede"
	ascii = "c,B"
	level = 3
	rarity = 1
	hp = "3d5"
	speed = 120
	aaf = 6
	ac = 11
	sleep = 15
	exp = 7
	blows = [('CRAWL', 'HURT', '1d2')]
	flags = ['RAND_50', 'DROP_SKELETON', 'WILD_DUNGEON_01', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_FOREST1', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about four feet long and carnivorous.
"""

monsters.register_race(metallic_blue_centipede)


class raven(monster):
	r_idx = "68"
	name = "Raven"
	ascii = "B,D"
	level = 3
	rarity = 2
	hp = "3d5"
	speed = 120
	aaf = 40
	ac = 15
	sleep = 0
	exp = 8
	blows = [('BITE', 'HURT', '1d4'), ('BITE', 'HURT', '1d4')]
	flags = ['ANIMAL', 'CAN_FLY', 'DROP_CORPSE', 'FORCE_MAXHP', 'WILD_GRASS', 'WILD_FOREST1', 'WILD_FOREST2']
	sfreq = 0
	sflags = []
	description = """Larger than a crow, pitch black.
"""

monsters.register_race(raven)


class giant_white_louse(monster):
	r_idx = "69"
	name = "Giant white louse"
	ascii = "I,w"
	level = 3
	rarity = 1
	hp = "1d1"
	speed = 120
	aaf = 6
	ac = 3
	sleep = 10
	exp = 1
	blows = [('BITE', 'HURT', '1d1')]
	flags = ['RAND_50', 'RAND_25', 'MULTIPLY', 'WEIRD_MIND', 'ANIMAL', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """It is six inches long.
"""

monsters.register_race(giant_white_louse)


class piranha(monster):
	r_idx = "70"
	name = "Piranha"
	ascii = "l,B"
	level = 3
	rarity = 1
	hp = "2d5"
	speed = 120
	aaf = 20
	ac = 10
	sleep = 5
	exp = 8
	blows = [('BITE', 'HURT', '9d1')]
	flags = ['FRIENDS', 'AQUATIC', 'ANIMAL', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2']
	sfreq = 0
	sflags = []
	description = """Bloodthirsty fish who can smell your blood from a great distance.
"""

monsters.register_race(piranha)


class black_naga(monster):
	r_idx = "71"
	name = "Black naga"
	ascii = "n,D"
	level = 3
	rarity = 1
	hp = "4d9"
	speed = 110
	aaf = 16
	ac = 20
	sleep = 120
	exp = 20
	blows = [('CRUSH', 'HURT', '1d8')]
	flags = ['FEMALE', 'WILD_DUNGEON_01', 'RAND_25', 'DROP_60', 'DROP_CORPSE', 'BASH_DOOR', 'CAN_SWIM', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A large black serpent's body with a female torso.
"""

monsters.register_race(black_naga)


class spotted_mushroom_patch(monster):
	r_idx = "72"
	name = "Spotted mushroom patch"
	ascii = ",,o"
	level = 3
	rarity = 1
	hp = "1d1"
	speed = 110
	aaf = 2
	ac = 3
	sleep = 0
	exp = 3
	blows = [('SPORE', 'POISON', '2d4')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """Yum!  It looks quite tasty.
"""

monsters.register_race(spotted_mushroom_patch)


class silver_jelly(monster, spellcaster):
	r_idx = "73"
	name = "Silver jelly"
	ascii = "j,W"
	level = 3
	rarity = 2
	hp = "8d8"
	speed = 120
	aaf = 2
	ac = 1
	sleep = 99
	exp = 12
	blows = [('TOUCH', 'EAT_LITE', '1d3'), ('TOUCH', 'EAT_LITE', '1d3')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 15
	sflags = ['DRAIN_MANA']
	description = """It is a large pile of silver flesh that sucks all light from its 
surroundings.
"""

monsters.register_race(silver_jelly)


class scruffy_looking_hobbit(monster):
	r_idx = "74"
	name = "Scruffy looking hobbit"
	ascii = "h,s"
	level = 3
	rarity = 1
	hp = "2d6"
	speed = 110
	aaf = 16
	ac = 11
	sleep = 10
	exp = 4
	blows = [('HIT', 'HURT', '1d4'), ('TOUCH', 'EAT_GOLD', '')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'DROP_60', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A short little guy, in bedraggled clothes.  He appears to be looking 
for a good tavern.
"""

monsters.register_race(scruffy_looking_hobbit)


class giant_white_ant(monster):
	r_idx = "75"
	name = "Giant white ant"
	ascii = "a,w"
	level = 3
	rarity = 1
	hp = "2d7"
	speed = 110
	aaf = 8
	ac = 12
	sleep = 80
	exp = 7
	blows = [('BITE', 'HURT', '1d4')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_GRASS', 'ANIMAL', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """It is about two feet long and has sharp pincers.
"""

monsters.register_race(giant_white_ant)


class yellow_mold(monster):
	r_idx = "76"
	name = "Yellow mold"
	ascii = "m,y"
	level = 3
	rarity = 1
	hp = "6d8"
	speed = 110
	aaf = 2
	ac = 4
	sleep = 99
	exp = 9
	blows = [('HIT', 'HURT', '1d4')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a strange growth on the dungeon floor.
"""

monsters.register_race(yellow_mold)


class metallic_red_centipede(monster):
	r_idx = "77"
	name = "Metallic red centipede"
	ascii = "c,R"
	level = 3
	rarity = 1
	hp = "3d8"
	speed = 120
	aaf = 8
	ac = 13
	sleep = 20
	exp = 12
	blows = [('CRAWL', 'HURT', '1d2')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_FOREST1', 'DROP_SKELETON', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about four feet long and carnivorous.
"""

monsters.register_race(metallic_red_centipede)


class yellow_worm_mass(monster):
	r_idx = "78"
	name = "Yellow worm mass"
	ascii = "w,y"
	level = 3
	rarity = 2
	hp = "3d8"
	speed = 100
	aaf = 7
	ac = 3
	sleep = 10
	exp = 4
	blows = [('CRAWL', 'LOSE_DEX', '1d3')]
	flags = ['RAND_50', 'RAND_25', 'CAN_SWIM', 'WILD_DUNGEON_01', 'STUPID', 'WEIRD_MIND', 'MULTIPLY', 'ANIMAL', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a large slimy mass of worms.
"""

monsters.register_race(yellow_worm_mass)


class clear_worm_mass(monster):
	r_idx = "79"
	name = "Clear worm mass"
	ascii = "w,B"
	level = 3
	rarity = 2
	hp = "3d4"
	speed = 100
	aaf = 7
	ac = 3
	sleep = 10
	exp = 4
	blows = [('CRAWL', 'POISON', '1d2')]
	flags = ['ATTR_CLEAR', 'CAN_SWIM', 'WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'STUPID', 'WEIRD_MIND', 'INVISIBLE', 'MULTIPLY', 'ANIMAL', 'IM_POIS', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a disgusting mass of poisonous worms.
"""

monsters.register_race(clear_worm_mass)


class radiation_eye(monster, spellcaster):
	r_idx = "80"
	name = "Radiation eye"
	ascii = "e,R"
	level = 3
	rarity = 1
	hp = "2d7"
	speed = 110
	aaf = 2
	ac = 5
	sleep = 10
	exp = 6
	blows = [('GAZE', 'LOSE_STR', '1d6')]
	flags = ['NEVER_MOVE', 'CAN_FLY', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'HURT_LITE', 'NO_FEAR']
	sfreq = 11
	sflags = ['DRAIN_MANA']
	description = """A disembodied eye, crackling with energy.
"""

monsters.register_race(radiation_eye)


class yellow_light(monster):
	r_idx = "81"
	name = "Yellow light"
	ascii = "*,y"
	level = 4
	rarity = 1
	hp = "2d5"
	speed = 120
	aaf = 8
	ac = 10
	sleep = 30
	exp = 4
	blows = [('EXPLODE', 'BLIND', '')]
	flags = ['EMPTY_MIND', 'CAN_FLY', 'NONLIVING', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """A fast-moving bright light.
"""

monsters.register_race(yellow_light)


class cave_lizard(monster):
	r_idx = "82"
	name = "Cave lizard"
	ascii = "R,u"
	level = 4
	rarity = 1
	hp = "2d7"
	speed = 110
	aaf = 8
	ac = 12
	sleep = 80
	exp = 8
	blows = [('BITE', 'HURT', '1d5')]
	flags = ['ANIMAL', 'CAN_SWIM', 'DROP_CORPSE', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """It is an armoured lizard with a powerful bite.
"""

monsters.register_race(cave_lizard)


class novice_ranger(monster, spellcaster):
	r_idx = "83"
	name = "Novice ranger"
	ascii = "p,g"
	level = 4
	rarity = 1
	hp = "4d9"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 5
	exp = 18
	blows = [('HIT', 'HURT', '1d5'), ('HIT', 'HURT', '1d5')]
	flags = ['MALE', 'FORCE_SLEEP', 'DROP_SKELETON', 'DROP_CORPSE', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2']
	sfreq = 9
	sflags = ['ARROW_2', 'MISSILE']
	description = """An agile hunter, ready and relaxed.
"""

monsters.register_race(novice_ranger)


class blue_jelly(monster):
	r_idx = "84"
	name = "Blue jelly"
	ascii = "j,b"
	level = 4
	rarity = 1
	hp = "9d8"
	speed = 110
	aaf = 2
	ac = 1
	sleep = 99
	exp = 14
	blows = [('TOUCH', 'COLD', '1d6')]
	flags = ['NEVER_MOVE', 'COLD_BLOOD', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_COLD', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It's a large pile of pulsing blue flesh.
"""

monsters.register_race(blue_jelly)


class creeping_copper_coins(monster):
	r_idx = "85"
	name = "Creeping copper coins"
	ascii = "$,u"
	level = 4
	rarity = 2
	hp = "5d8"
	speed = 100
	aaf = 3
	ac = 20
	sleep = 10
	exp = 9
	blows = [('HIT', 'HURT', '1d4'), ('TOUCH', 'POISON', '2d4')]
	flags = ['ONLY_GOLD', 'DROP_1D2', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'BASH_DOOR', 'ANIMAL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """It is a pile of coins.
"""

monsters.register_race(creeping_copper_coins)


class giant_white_rat(monster):
	r_idx = "86"
	name = "Giant white rat"
	ascii = "r,W"
	level = 4
	rarity = 1
	hp = "2d2"
	speed = 110
	aaf = 8
	ac = 6
	sleep = 30
	exp = 1
	blows = [('BITE', 'POISON', '1d3')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'MULTIPLY', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is a very vicious rodent.
"""

monsters.register_race(giant_white_rat)


class snotling(orc):
	r_idx = "87"
	name = "Snotling"
	ascii = "o,G"
	level = 4
	rarity = 1
	hp = "4d5"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 30
	exp = 15
	blows = [('HIT', 'HURT', '1d6')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_60', 'RAND_50', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_FOREST1', 'WILD_FOREST2', 'EVIL', 'ORC', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """A pathetic breed of tiny snaga-like creatures. They are running 
wild and screaming all the time!
"""

monsters.register_race(snotling)


class swordfish(monster):
	r_idx = "88"
	name = "Swordfish"
	ascii = "l,W"
	level = 4
	rarity = 2
	hp = "4d10"
	speed = 120
	aaf = 14
	ac = 15
	sleep = 20
	exp = 15
	blows = [('STING', 'HURT', '5d1'), ('STING', 'HURT', '5d1')]
	flags = ['ANIMAL', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_SHORE']
	sfreq = 0
	sflags = []
	description = """An animal with a swordlike "beak".
"""

monsters.register_race(swordfish)


class blue_worm_mass(monster):
	r_idx = "89"
	name = "Blue worm mass"
	ascii = "w,b"
	level = 4
	rarity = 1
	hp = "4d7"
	speed = 100
	aaf = 7
	ac = 4
	sleep = 10
	exp = 5
	blows = [('CRAWL', 'COLD', '1d4')]
	flags = ['RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'STUPID', 'WEIRD_MIND', 'COLD_BLOOD', 'MULTIPLY', 'ANIMAL', 'IM_COLD', 'CAN_SWIM', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a large slimy mass of worms.
"""

monsters.register_race(blue_worm_mass)


class large_grey_snake(monster):
	r_idx = "90"
	name = "Large grey snake"
	ascii = "J,s"
	level = 4
	rarity = 1
	hp = "4d9"
	speed = 100
	aaf = 6
	ac = 22
	sleep = 50
	exp = 14
	blows = [('BITE', 'HURT', '1d5'), ('CRUSH', 'HURT', '1d8')]
	flags = ['RAND_25', 'CAN_SWIM', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is about ten feet long.
"""

monsters.register_race(large_grey_snake)


class skeleton_kobold(monster, undead):
	r_idx = "91"
	name = "Skeleton kobold"
	ascii = "s,w"
	level = 5
	rarity = 1
	hp = "4d7"
	speed = 110
	aaf = 20
	ac = 16
	sleep = 40
	exp = 12
	blows = [('HIT', 'HURT', '1d6')]
	flags = ['WILD_DUNGEON_01', 'COLD_BLOOD', 'EMPTY_MIND', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a small animated kobold skeleton.
"""

monsters.register_race(skeleton_kobold)


class ewok(monster, spellcaster):
	r_idx = "92"
	name = "Ewok"
	ascii = "h,G"
	level = 5
	rarity = 2
	hp = "2d6"
	speed = 110
	aaf = 10
	ac = 15
	sleep = 10
	exp = 20
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6')]
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST2', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'FRIENDS', 'DROP_CORPSE']
	sfreq = 8
	sflags = ['ARROW_1']
	description = """A cute, furry little animal, full of merchandising potential. 
"""

monsters.register_race(ewok)


class novice_mage(monster, spellcaster):
	r_idx = "93"
	name = "Novice mage"
	ascii = "p,R"
	level = 5
	rarity = 2
	hp = "4d5"
	speed = 110
	aaf = 20
	ac = 6
	sleep = 10
	exp = 7
	blows = [('HIT', 'HURT', '1d4')]
	flags = ['MALE', 'FORCE_SLEEP', 'FRIENDS', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 12
	sflags = ['BLINK', 'BLIND', 'CONF', 'MISSILE']
	description = """He is leaving behind a trail of dropped spell components.
"""

monsters.register_race(novice_mage)


class green_naga(monster):
	r_idx = "94"
	name = "Green naga"
	ascii = "n,g"
	level = 5
	rarity = 1
	hp = "6d9"
	speed = 110
	aaf = 18
	ac = 20
	sleep = 120
	exp = 30
	blows = [('CRUSH', 'HURT', '1d8'), ('SPIT', 'ACID', '2d6')]
	flags = ['FEMALE', 'WILD_DUNGEON_01', 'RAND_25', 'TAKE_ITEM', 'DROP_60', 'DROP_CORPSE', 'BASH_DOOR', 'CAN_SWIM', 'WILD_SHORE', 'EVIL', 'IM_ACID']
	sfreq = 0
	sflags = []
	description = """A large green serpent with a female's torso.  Her green skin glistens with 
acid.
"""

monsters.register_race(green_naga)


class giant_leech(monster):
	r_idx = "95"
	name = "Giant leech"
	ascii = "w,v"
	level = 5
	rarity = 1
	hp = "5d7"
	speed = 120
	aaf = 10
	ac = 15
	sleep = 50
	exp = 20
	blows = [('BITE', 'HURT', '3d1'), ('BITE', 'HURT', '3d1')]
	flags = ['ANIMAL', 'AQUATIC', 'WEIRD_MIND', 'RAND_25', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """Yech! The disgusting thing only wants your blood!
"""

monsters.register_race(giant_leech)


class barracuda(monster):
	r_idx = "96"
	name = "Barracuda"
	ascii = "l,b"
	level = 5
	rarity = 2
	hp = "5d10"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 20
	exp = 30
	blows = [('BITE', 'HURT', '10d1'), ('BITE', 'HURT', '1d10')]
	flags = ['AQUATIC', 'ANIMAL', 'WILD_SHORE', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """A predatory fish with razor-sharp teeth.
"""

monsters.register_race(barracuda)


class novice_paladin(monster, spellcaster):
	r_idx = "97"
	name = "Novice paladin"
	ascii = "p,w"
	level = 4
	rarity = 1
	hp = "4d9"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 5
	exp = 20
	blows = [('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '1d7')]
	flags = ['MALE', 'FORCE_SLEEP', 'WILD_DUNGEON_01', 'DROP_60', 'WILD_GRASS', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 9
	sflags = ['SCARE', 'CAUSE_1']
	description = """An adventurer both devoutly religious and skillful in combat. 
He seems to consider you an agent of the devil.
"""

monsters.register_race(novice_paladin)


class zog(monster):
	r_idx = "98"
	name = "Zog"
	ascii = "h,b"
	level = 5
	rarity = 1
	hp = "11d9"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 20
	exp = 25
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8'), ('DROOL', '', '')]
	flags = ['WILD_DUNGEON_01', 'EVIL', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_90', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """Drooling, insectoid aliens with disgusting habits.
"""

monsters.register_race(zog)


class blue_ooze(monster):
	r_idx = "99"
	name = "Blue ooze"
	ascii = "j,b"
	level = 5
	rarity = 1
	hp = "2d5"
	speed = 110
	aaf = 8
	ac = 11
	sleep = 80
	exp = 7
	blows = [('CRAWL', 'COLD', '1d4')]
	flags = ['RAND_50', 'RAND_25', 'DROP_60', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'CAN_SWIM', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It's blue and it's oozing.
"""

monsters.register_race(blue_ooze)


class green_glutton_ghost(monster, undead):
	r_idx = "100"
	name = "Green glutton ghost"
	ascii = "G,g"
	level = 5
	rarity = 1
	hp = "2d5"
	speed = 130
	aaf = 10
	ac = 12
	sleep = 10
	exp = 15
	blows = [('TOUCH', 'EAT_FOOD', '1d1')]
	flags = ['RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'CAN_FLY', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'EVIL', 'UNDEAD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """It is a very ugly green ghost with a voracious appetite.
"""

monsters.register_race(green_glutton_ghost)


class green_jelly(monster):
	r_idx = "101"
	name = "Green jelly"
	ascii = "j,g"
	level = 5
	rarity = 1
	hp = "17d8"
	speed = 120
	aaf = 2
	ac = 1
	sleep = 99
	exp = 18
	blows = [('TOUCH', 'ACID', '1d2')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_ACID', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a large pile of pulsing green flesh.
"""

monsters.register_race(green_jelly)


class large_kobold(monster):
	r_idx = "102"
	name = "Large kobold"
	ascii = "k,w"
	level = 5
	rarity = 1
	hp = "10d9"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 30
	exp = 25
	blows = [('HIT', 'HURT', '1d10')]
	flags = ['DROP_90', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It a man-sized figure with the all too recognizable face of a kobold.
"""

monsters.register_race(large_kobold)


class grey_icky_thing(monster):
	r_idx = "103"
	name = "Grey icky thing"
	ascii = "i,s"
	level = 5
	rarity = 1
	hp = "3d8"
	speed = 110
	aaf = 14
	ac = 10
	sleep = 15
	exp = 10
	blows = [('TOUCH', 'HURT', '1d5')]
	flags = ['RAND_50', 'CAN_SWIM', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'EMPTY_MIND']
	sfreq = 0
	sflags = []
	description = """It is a smallish, slimy, icky, nasty creature.
"""

monsters.register_race(grey_icky_thing)


class disenchanter_eye(monster, spellcaster):
	r_idx = "104"
	name = "Disenchanter eye"
	ascii = "e,v"
	level = 5
	rarity = 2
	hp = "5d8"
	speed = 100
	aaf = 2
	ac = 8
	sleep = 10
	exp = 20
	blows = [('GAZE', 'UN_BONUS', '')]
	flags = ['ATTR_MULTI', 'ATTR_ANY', 'RES_DISE', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'NEVER_MOVE', 'CAN_FLY', 'HURT_LITE', 'NO_FEAR']
	sfreq = 9
	sflags = ['DRAIN_MANA']
	description = """A disembodied eye, crackling with magic.
"""

monsters.register_race(disenchanter_eye)


class red_worm_mass(monster):
	r_idx = "105"
	name = "Red worm mass"
	ascii = "w,r"
	level = 5
	rarity = 1
	hp = "4d7"
	speed = 100
	aaf = 7
	ac = 8
	sleep = 10
	exp = 6
	blows = [('CRAWL', 'FIRE', '1d6')]
	flags = ['RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'MULTIPLY', 'BASH_DOOR', 'ANIMAL', 'IM_FIRE', 'CAN_SWIM', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a large slimy mass of worms.
"""

monsters.register_race(red_worm_mass)


class copperhead_snake(monster):
	r_idx = "106"
	name = "Copperhead snake"
	ascii = "J,o"
	level = 5
	rarity = 1
	hp = "3d6"
	speed = 110
	aaf = 6
	ac = 16
	sleep = 1
	exp = 15
	blows = [('BITE', 'POISON', '2d4')]
	flags = ['RAND_50', 'CAN_SWIM', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'WILD_GRASS', 'WILD_FOREST1', 'WILD_FOREST2', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It has a copper head and sharp venomous fangs.
"""

monsters.register_race(copperhead_snake)


class death_sword(monster):
	r_idx = "107"
	name = "Death sword"
	ascii = "|,W"
	level = 6
	rarity = 5
	hp = "5d7"
	speed = 130
	aaf = 20
	ac = 20
	sleep = 0
	exp = 30
	blows = [('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['NEVER_MOVE', 'NONLIVING', 'NO_FEAR', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'COLD_BLOOD', 'CHAR_MULTI', 'NO_CONF', 'NO_SLEEP', 'DROP_90', 'EVIL', 'IM_COLD', 'IM_FIRE', 'FORCE_MAXHP', 'IM_ELEC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A bloodthirsty blade lurking for prey. Beware! 
"""

monsters.register_race(death_sword)


class purple_mushroom_patch(monster):
	r_idx = "108"
	name = "Purple mushroom patch"
	ascii = ",,v"
	level = 6
	rarity = 2
	hp = "1d1"
	speed = 110
	aaf = 2
	ac = 1
	sleep = 0
	exp = 15
	blows = [('SPORE', 'LOSE_CON', '1d2'), ('SPORE', 'LOSE_CON', '1d2'), ('SPORE', 'LOSE_CON', '1d2')]
	flags = ['NEVER_MOVE', 'STUPID', 'EMPTY_MIND', 'WILD_DUNGEON_01', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """Yum!  It looks quite tasty.
"""

monsters.register_race(purple_mushroom_patch)


class novice_priest(monster, spellcaster):
	r_idx = "109"
	name = "Novice priest"
	ascii = "p,B"
	level = 6
	rarity = 2
	hp = "5d4"
	speed = 110
	aaf = 20
	ac = 9
	sleep = 5
	exp = 7
	blows = [('HIT', 'HURT', '1d5')]
	flags = ['MALE', 'GOOD', 'FORCE_SLEEP', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'DROP_CORPSE', 'FRIENDS', 'DROP_60', 'WILD_GRASS', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 12
	sflags = ['HEAL', 'SCARE', 'CAUSE_1']
	description = """He is tripping over his priestly robes.
"""

monsters.register_race(novice_priest)


class novice_warrior(monster):
	r_idx = "110"
	name = "Novice warrior"
	ascii = "p,U"
	level = 6
	rarity = 2
	hp = "7d4"
	speed = 110
	aaf = 20
	ac = 13
	sleep = 5
	exp = 6
	blows = [('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '1d6')]
	flags = ['MALE', 'FRIENDS', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_GRASS', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """He looks inexperienced but tough.
"""

monsters.register_race(novice_warrior)


class nibelung(monster):
	r_idx = "111"
	name = "Nibelung"
	ascii = "h,D"
	level = 6
	rarity = 1
	hp = "6d4"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 5
	exp = 6
	blows = [('HIT', 'HURT', '1d6'), ('TOUCH', 'EAT_GOLD', '')]
	flags = ['MALE', 'FRIENDS', 'DROP_60', 'WILD_DUNGEON_01', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'HURT_LITE', 'RES_DISE', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """Night dwarfs collecting new riches for their master, Alberich. 
"""

monsters.register_race(nibelung)


class disembodied_hand_that_strangled_people(monster, undead):
	r_idx = "112"
	name = "Disembodied hand that strangled people"
	ascii = "z,g"
	level = 6
	rarity = 2
	hp = "5d8"
	speed = 130
	aaf = 30
	ac = 20
	sleep = 20
	exp = 20
	blows = [('CRUSH', 'HURT', '1d8')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_POIS', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """Even today, nobody knows where it lurks... 
"""

monsters.register_race(disembodied_hand_that_strangled_people)


class brown_mold(monster):
	r_idx = "113"
	name = "Brown mold"
	ascii = "m,u"
	level = 6
	rarity = 1
	hp = "11d8"
	speed = 110
	aaf = 2
	ac = 12
	sleep = 99
	exp = 20
	blows = [('HIT', 'CONFUSE', '1d4')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A strange brown growth on the dungeon floor.
"""

monsters.register_race(brown_mold)


class giant_brown_bat(monster):
	r_idx = "114"
	name = "Giant brown bat"
	ascii = "b,u"
	level = 6
	rarity = 1
	hp = "2d9"
	speed = 130
	aaf = 10
	ac = 12
	sleep = 30
	exp = 10
	blows = [('BITE', 'HURT', '1d3')]
	flags = ['RAND_50', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_MOUNT2', 'WILD_FOREST1', 'WILD_FOREST2', 'ANIMAL', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It screeches as it attacks.
"""

monsters.register_race(giant_brown_bat)


class rat_thing(monster, spellcaster):
	r_idx = "115"
	name = "Rat-thing"
	ascii = "r,R"
	level = 6
	rarity = 1
	hp = "7d9"
	speed = 120
	aaf = 12
	ac = 15
	sleep = 20
	exp = 10
	blows = [('BITE', 'HURT', '3d2'), ('BITE', 'HURT', '3d2')]
	flags = ['EVIL', 'ANIMAL', 'DROP_CORPSE', 'WILD_DUNGEON_01']
	sfreq = 9
	sflags = ['SCARE']
	description = """A ratlike creature with a humanlike face. "The bones of the 
tiny paws, it is rumoured, imply prehensile characteristics 
more typical of a diminutive monkey than a of a rat; while the 
small skull with its savage yellow fangs is of the utmost 
anomalousness, appearing from certain angles like a miniature, 
monstrously degrade parody of a human skull."
"""

monsters.register_race(rat_thing)


class novice_archer(monster, spellcaster):
	r_idx = "116"
	name = "Novice archer"
	ascii = "p,G"
	level = 6
	rarity = 2
	hp = "4d9"
	speed = 120
	aaf = 20
	ac = 10
	sleep = 5
	exp = 20
	blows = [('HIT', 'HURT', '1d4'), ('HIT', 'HURT', '1d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'WILD_GRASS', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_GOLD', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 3
	sflags = ['ARROW_1']
	description = """A nasty little fellow with a bow and arrow.
"""

monsters.register_race(novice_archer)


class creeping_silver_coins(monster):
	r_idx = "117"
	name = "Creeping silver coins"
	ascii = "$,s"
	level = 6
	rarity = 2
	hp = "8d8"
	speed = 100
	aaf = 4
	ac = 18
	sleep = 10
	exp = 18
	blows = [('HIT', 'HURT', '1d6'), ('TOUCH', 'POISON', '2d6')]
	flags = ['ONLY_GOLD', 'DROP_60', 'DROP_1D2', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'BASH_DOOR', 'ANIMAL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """It is a pile of coins, crawling forward on thousands of tiny legs.
"""

monsters.register_race(creeping_silver_coins)


class snaga(orc):
	r_idx = "118"
	name = "Snaga"
	ascii = "o,G"
	level = 6
	rarity = 1
	hp = "6d9"
	speed = 110
	aaf = 20
	ac = 16
	sleep = 30
	exp = 15
	blows = [('HIT', 'HURT', '1d8')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_60', 'WILD_FOREST1', 'WILD_WASTE1', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is one of the many weaker 'slave' orcs, often mistakenly known as a 
goblin.
"""

monsters.register_race(snaga)


class rattlesnake(monster):
	r_idx = "119"
	name = "Rattlesnake"
	ascii = "J,r"
	level = 6
	rarity = 1
	hp = "4d8"
	speed = 110
	aaf = 6
	ac = 12
	sleep = 1
	exp = 20
	blows = [('BITE', 'POISON', '2d5')]
	flags = ['RAND_50', 'CAN_SWIM', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'WILD_WASTE1', 'WILD_MOUNT1', 'WILD_MOUNT2', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It is recognized by the hard-scaled end of its body that is often rattled 
to frighten its prey.
"""

monsters.register_race(rattlesnake)


class giant_slug(monster, spellcaster):
	r_idx = "120"
	name = "Giant slug"
	ascii = "w,U"
	level = 6
	rarity = 1
	hp = "9d9"
	speed = 100
	aaf = 10
	ac = 12
	sleep = 25
	exp = 25
	blows = [('BITE', 'ACID', '2d4'), ('BITE', 'ACID', '2d6')]
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST1', 'ANIMAL', 'EMPTY_MIND', 'KILL_ITEM', 'KILL_BODY', 'CAN_SWIM', 'DROP_CORPSE']
	sfreq = 10
	sflags = ['BR_ACID']
	description = """It is slowly making its way towards you, eating everything in 
its path...
"""

monsters.register_race(giant_slug)


class giant_pink_frog(monster):
	r_idx = "121"
	name = "Giant pink frog"
	ascii = "R,R"
	level = 7
	rarity = 1
	hp = "4d8"
	speed = 110
	aaf = 12
	ac = 16
	sleep = 50
	exp = 16
	blows = [('BITE', 'LOSE_STR', '2d4')]
	flags = ['RAND_50', 'WILD_SHORE', 'WILD_SWAMP1', 'WILD_SWAMP2', 'BASH_DOOR', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It looks poisonous.
"""

monsters.register_race(giant_pink_frog)


class dark_elf(monster, spellcaster):
	r_idx = "122"
	name = "Dark elf"
	ascii = "h,D"
	level = 7
	rarity = 2
	hp = "5d10"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 20
	exp = 25
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'DROP_90', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'HURT_LITE']
	sfreq = 10
	sflags = ['CONF', 'DARKNESS', 'MISSILE']
	description = """An elven figure with jet black skin and white hair, his eyes are large and 
twisted with evil.
"""

monsters.register_race(dark_elf)


class zombified_kobold(monster, undead):
	r_idx = "123"
	name = "Zombified kobold"
	ascii = "z,s"
	level = 7
	rarity = 1
	hp = "4d9"
	speed = 110
	aaf = 20
	ac = 14
	sleep = 30
	exp = 14
	blows = [('HIT', 'HURT', '1d2'), ('HIT', 'HURT', '1d2')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is an animated kobold corpse.  Flesh falls off in large chunks as it 
shambles forward. 
"""

monsters.register_race(zombified_kobold)


class crypt_creep(monster, undead, spellcaster):
	r_idx = "124"
	name = "Crypt Creep"
	ascii = "s,D"
	level = 7
	rarity = 2
	hp = "4d9"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 14
	exp = 25
	blows = [('CLAW', 'HURT', '1d2'), ('CLAW', 'HURT', '1d2'), ('BITE', 'POISON', '')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'FRIENDS', 'EVIL', 'UNDEAD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'HURT_LITE']
	sfreq = 10
	sflags = ['CAUSE_1', 'S_UNDEAD']
	description = """Frightening skeletal figures in black robes. 
"""

monsters.register_race(crypt_creep)


class rotting_corpse(monster, undead):
	r_idx = "125"
	name = "Rotting corpse"
	ascii = "z,R"
	level = 7
	rarity = 1
	hp = "6d8"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 20
	exp = 15
	blows = [('CLAW', 'POISON', '1d3'), ('CLAW', 'POISON', '1d3')]
	flags = ['OPEN_DOOR', 'BASH_DOOR', 'FRIENDS', 'WILD_DUNGEON_01', 'NO_CONF', 'NO_SLEEP', 'UNDEAD', 'EVIL', 'NO_FEAR', 'IM_POIS', 'IM_COLD', 'COLD_BLOOD', 'EMPTY_MIND']
	sfreq = 0
	sflags = []
	description = """Corpses awakened from their sleep by dark sorceries. 
"""

monsters.register_race(rotting_corpse)


class cave_orc(orc):
	r_idx = "126"
	name = "Cave orc"
	ascii = "o,U"
	level = 7
	rarity = 1
	hp = "8d9"
	speed = 110
	aaf = 20
	ac = 18
	sleep = 30
	exp = 20
	blows = [('HIT', 'HURT', '1d8')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_60', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is often found in huge numbers in deep caves.
"""

monsters.register_race(cave_orc)


class wood_spider(monster):
	r_idx = "127"
	name = "Wood spider"
	ascii = "S,U"
	level = 7
	rarity = 3
	hp = "2d7"
	speed = 120
	aaf = 8
	ac = 10
	sleep = 60
	exp = 15
	blows = [('BITE', 'HURT', '1d3'), ('STING', 'POISON', '1d4')]
	flags = ['FRIENDS', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It scuttles towards you.
"""

monsters.register_race(wood_spider)


class manes(demon):
	r_idx = "128"
	name = "Manes"
	ascii = "u,R"
	level = 7
	rarity = 2
	hp = "6d8"
	speed = 110
	aaf = 20
	ac = 18
	sleep = 30
	exp = 16
	blows = [('HIT', 'HURT', '1d8')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a minor but aggressive demon.
"""

monsters.register_race(manes)


class bloodshot_eye(monster, spellcaster):
	r_idx = "129"
	name = "Bloodshot eye"
	ascii = "e,r"
	level = 7
	rarity = 3
	hp = "3d9"
	speed = 110
	aaf = 2
	ac = 6
	sleep = 10
	exp = 15
	blows = [('GAZE', 'BLIND', '2d6')]
	flags = ['NEVER_MOVE', 'CAN_FLY', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'HURT_LITE', 'NO_FEAR']
	sfreq = 7
	sflags = ['DRAIN_MANA']
	description = """A disembodied eye, bloodshot and nasty.
"""

monsters.register_race(bloodshot_eye)


class red_naga(monster):
	r_idx = "130"
	name = "Red naga"
	ascii = "n,R"
	level = 7
	rarity = 2
	hp = "8d8"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 120
	exp = 40
	blows = [('CRUSH', 'HURT', '1d10'), ('BITE', 'LOSE_STR', '1d4')]
	flags = ['FEMALE', 'CAN_SWIM', 'WILD_DUNGEON_01', 'WILD_SHORE', 'RAND_25', 'DROP_60', 'TAKE_ITEM', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A large red snake with a woman's torso.
"""

monsters.register_race(red_naga)


class red_jelly(monster):
	r_idx = "131"
	name = "Red jelly"
	ascii = "j,R"
	level = 7
	rarity = 1
	hp = "20d8"
	speed = 110
	aaf = 2
	ac = 4
	sleep = 99
	exp = 26
	blows = [('TOUCH', 'LOSE_STR', '1d5')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a large pulsating mound of red flesh.
"""

monsters.register_race(red_jelly)


class green_icky_thing(monster):
	r_idx = "132"
	name = "Green icky thing"
	ascii = "i,G"
	level = 7
	rarity = 2
	hp = "3d9"
	speed = 110
	aaf = 14
	ac = 16
	sleep = 20
	exp = 18
	blows = [('TOUCH', 'ACID', '2d5')]
	flags = ['RAND_50', 'CAN_SWIM', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'IM_ACID']
	sfreq = 0
	sflags = []
	description = """It is a smallish, slimy, icky, acidic creature.
"""

monsters.register_race(green_icky_thing)


class lost_soul(monster, undead, spellcaster):
	r_idx = "133"
	name = "Lost soul"
	ascii = "G,u"
	level = 7
	rarity = 2
	hp = "2d6"
	speed = 110
	aaf = 12
	ac = 8
	sleep = 10
	exp = 18
	blows = [('HIT', 'HURT', '2d2'), ('TOUCH', 'LOSE_WIS', '')]
	flags = ['RAND_50', 'DROP_60', 'DROP_90', 'CAN_FLY', 'WILD_DUNGEON_01', 'INVISIBLE', 'COLD_BLOOD', 'TAKE_ITEM', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['TPORT', 'DRAIN_MANA']
	description = """It is almost insubstantial.
"""

monsters.register_race(lost_soul)


class night_lizard(monster):
	r_idx = "134"
	name = "Night lizard"
	ascii = "R,b"
	level = 7
	rarity = 2
	hp = "3d7"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 30
	exp = 35
	blows = [('BITE', 'HURT', '1d6'), ('BITE', 'HURT', '1d6')]
	flags = ['ANIMAL', 'CAN_SWIM', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It is a black lizard with overlapping scales and a powerful jaw.
"""

monsters.register_race(night_lizard)


class mughash_the_kobold_lord(monster):
	r_idx = "135"
	name = "Mughash the Kobold Lord"
	ascii = "k,b"
	level = 7
	rarity = 3
	hp = "12d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 20
	exp = 100
	blows = [('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ESCORT', 'ESCORTS', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """Strong and powerful, for a kobold.
"""

monsters.register_race(mughash_the_kobold_lord)


class skeleton_orc(orc, undead):
	r_idx = "136"
	name = "Skeleton orc"
	ascii = "s,w"
	level = 8
	rarity = 1
	hp = "7d8"
	speed = 110
	aaf = 20
	ac = 18
	sleep = 40
	exp = 26
	blows = [('HIT', 'HURT', '2d5')]
	flags = ['COLD_BLOOD', 'EMPTY_MIND', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'UNDEAD', 'WILD_DUNGEON_01', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is an animated orc skeleton.
"""

monsters.register_race(skeleton_orc)


class wormtongue_agent_of_saruman(monster, spellcaster):
	r_idx = "137"
	name = "Wormtongue, Agent of Saruman"
	ascii = "p,D"
	level = 8
	rarity = 2
	hp = "19d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 20
	exp = 150
	blows = [('HIT', 'HURT', '1d5'), ('HIT', 'HURT', '1d5'), ('TOUCH', 'EAT_GOLD', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'DROP_GREAT', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'EVIL', 'RES_TELE']
	sfreq = 5
	sflags = ['HEAL', 'SLOW', 'TRAPS', 'BO_COLD', 'BA_POIS']
	description = """He's been spying for Saruman.  He is a snivelling wretch with no morals.
"At [Theoden's] feet upon the steps sat a wizened figure of a man, with a 
pale wise face and heavy-lidded eyes...he laughed grimly, as he lifted his 
heavy lids for a moment and gazed on the strangers with dark eyes." 
"""

monsters.register_race(wormtongue_agent_of_saruman)


class robin_hood_the_outlaw(monster, spellcaster):
	r_idx = "138"
	name = "Robin Hood, the Outlaw"
	ascii = "p,G"
	level = 8
	rarity = 2
	hp = "10d12"
	speed = 120
	aaf = 20
	ac = 15
	sleep = 20
	exp = 150
	blows = [('HIT', 'HURT', '1d5'), ('HIT', 'HURT', '1d5'), ('TOUCH', 'EAT_GOLD', ''), ('TOUCH', 'EAT_ITEM', '')]
	flags = ['UNIQUE', 'MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_SPEAK', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'DROP_GREAT', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_GRASS', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 5
	sflags = ['ARROW_2', 'HEAL', 'TRAPS']
	description = """The legendary archer steals from the rich (you qualify).
"""

monsters.register_race(robin_hood_the_outlaw)


class nurgling(demon):
	r_idx = "139"
	name = "Nurgling"
	ascii = "u,U"
	level = 8
	rarity = 2
	hp = "6d10"
	speed = 110
	aaf = 20
	ac = 16
	sleep = 30
	exp = 19
	blows = [('BITE', 'DISEASE', '1d8')]
	flags = ['FRIENDS', 'FRIEND', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'IM_POIS', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a minor demon servitor of Nurgle. It looks like a hairless 
teddy bear, with twisted eyes and rotting, ghoulish skin.
"""

monsters.register_race(nurgling)


class lagduf_the_snaga(orc):
	r_idx = "140"
	name = "Lagduf, the Snaga"
	ascii = "o,u"
	level = 8
	rarity = 2
	hp = "14d10"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 30
	exp = 80
	blows = [('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d9')]
	flags = ['UNIQUE', 'MALE', 'FORCE_MAXHP', 'ESCORT', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'ORC']
	sfreq = 0
	sflags = []
	description = """A captain of a regiment of weaker orcs, Lagduf keeps his troop in order 
with displays of excessive violence.
"I've told you twice that Gorbag's swine got to the gate first, and none 
of ours got out.  Lagduf and Muzgash ran through, but they were shot." 
"""

monsters.register_race(lagduf_the_snaga)


class brown_yeek(monster):
	r_idx = "141"
	name = "Brown yeek"
	ascii = "y,u"
	level = 8
	rarity = 1
	hp = "3d7"
	speed = 110
	aaf = 18
	ac = 12
	sleep = 10
	exp = 11
	blows = [('HIT', 'HURT', '1d6')]
	flags = ['DROP_60', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'ANIMAL', 'IM_ACID']
	sfreq = 0
	sflags = []
	description = """It is a strange small humanoid.
"""

monsters.register_race(brown_yeek)


class novice_ranger(monster, spellcaster):
	r_idx = "142"
	name = "Novice ranger"
	ascii = "p,g"
	level = 8
	rarity = 1
	hp = "4d9"
	speed = 110
	aaf = 20
	ac = 8
	sleep = 5
	exp = 18
	blows = [('HIT', 'HURT', '1d5'), ('HIT', 'HURT', '1d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'FORCE_SLEEP', 'FRIENDS', 'DROP_60', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 9
	sflags = ['ARROW_2', 'MISSILE']
	description = """An agile hunter, ready and relaxed.
"""

monsters.register_race(novice_ranger)


class giant_salamander(monster, spellcaster):
	r_idx = "143"
	name = "Giant salamander"
	ascii = "R,r"
	level = 8
	rarity = 1
	hp = "4d8"
	speed = 110
	aaf = 6
	ac = 20
	sleep = 1
	exp = 50
	blows = [('BITE', 'FIRE', '3d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'RAND_25', 'ANIMAL', 'IM_FIRE', 'CAN_SWIM', 'DROP_CORPSE']
	sfreq = 9
	sflags = ['BR_FIRE']
	description = """A large black and yellow lizard.  You'd better run away!
"""

monsters.register_race(giant_salamander)


class space_monster(monster):
	r_idx = "144"
	name = "Space monster"
	ascii = ".,d"
	level = 8
	rarity = 2
	hp = "18d8"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 20
	exp = 28
	blows = [('HIT', 'TERRIFY', '1d4')]
	flags = ['WILD_DUNGEON_01', 'PASS_WALL', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'IM_ACID', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """A black hole in the fabric of reality. 
"""

monsters.register_race(space_monster)


class carnivorous_flying_monkey(monster):
	r_idx = "145"
	name = "Carnivorous flying monkey"
	ascii = "H,R"
	level = 8
	rarity = 2
	hp = "17d9"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 20
	exp = 30
	blows = [('CLAW', 'HURT', '1d10'), ('CLAW', 'HURT', '1d10'), ('BITE', 'HURT', '5d1')]
	flags = ['ANIMAL', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_MOUNT2', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It looks fantastic, yet frightening.
"""

monsters.register_race(carnivorous_flying_monkey)


class green_mold(monster):
	r_idx = "146"
	name = "Green mold"
	ascii = "m,g"
	level = 8
	rarity = 2
	hp = "16d8"
	speed = 110
	aaf = 2
	ac = 15
	sleep = 75
	exp = 28
	blows = [('HIT', 'TERRIFY', '1d4')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'STUPID', 'EMPTY_MIND', 'IM_ACID', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a strange growth on the dungeon floor.
"""

monsters.register_race(green_mold)


class novice_paladin(monster, spellcaster):
	r_idx = "147"
	name = "Novice paladin"
	ascii = "p,w"
	level = 8
	rarity = 2
	hp = "4d9"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 5
	exp = 20
	blows = [('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '1d7')]
	flags = ['MALE', 'GOOD', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 9
	sflags = ['SCARE', 'CAUSE_1']
	description = """He thinks you are an agent of the devil. 
"""

monsters.register_race(novice_paladin)


class lemure(demon):
	r_idx = "148"
	name = "Lemure"
	ascii = "u,o"
	level = 8
	rarity = 3
	hp = "9d9"
	speed = 110
	aaf = 20
	ac = 16
	sleep = 30
	exp = 16
	blows = [('HIT', 'HURT', '1d8')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is the larval form of a major demon.
"""

monsters.register_race(lemure)


class hill_orc(orc):
	r_idx = "149"
	name = "Hill orc"
	ascii = "o,s"
	level = 8
	rarity = 1
	hp = "9d9"
	speed = 110
	aaf = 20
	ac = 16
	sleep = 30
	exp = 25
	blows = [('HIT', 'HURT', '1d10')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'ORC', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is a hardy well-weathered survivor.
"""

monsters.register_race(hill_orc)


class bandit(monster):
	r_idx = "150"
	name = "Bandit"
	ascii = "p,D"
	level = 8
	rarity = 2
	hp = "6d16"
	speed = 110
	aaf = 20
	ac = 12
	sleep = 10
	exp = 26
	blows = [('HIT', 'HURT', '2d4'), ('TOUCH', 'EAT_GOLD', '')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST1', 'DROP_1D2', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL']
	sfreq = 0
	sflags = []
	description = """He is after your cash!
"""

monsters.register_race(bandit)


class hunting_hawk_of_julian(monster):
	r_idx = "151"
	name = "Hunting hawk of Julian"
	ascii = "B,u"
	level = 8
	rarity = 2
	hp = "6d8"
	speed = 120
	aaf = 30
	ac = 15
	sleep = 10
	exp = 22
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d4')]
	flags = ['ANIMAL', 'NO_FEAR', 'CAN_FLY', 'WILD_FOREST1', 'WILD_FOREST2', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """Trained to hunt and kill without fear.
"""

monsters.register_race(hunting_hawk_of_julian)


class phantom_warrior(monster):
	r_idx = "152"
	name = "Phantom warrior"
	ascii = "G,B"
	level = 8
	rarity = 1
	hp = "4d6"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 40
	exp = 15
	blows = [('HIT', 'HURT', '1d11'), ('HIT', 'HURT', '1d11')]
	flags = ['WILD_DUNGEON_01', 'PASS_WALL', 'NO_SLEEP', 'FRIENDS', 'COLD_BLOOD', 'NONLIVING', 'NO_FEAR', 'EMPTY_MIND', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """Creatures that are half real, half illusion.
"""

monsters.register_race(phantom_warrior)


class gremlin(demon):
	r_idx = "153"
	name = "Gremlin"
	ascii = "u,u"
	level = 8
	rarity = 3
	hp = "4d6"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 20
	exp = 6
	blows = [('CLAW', 'EAT_FOOD', '1d2'), ('CLAW', 'EAT_FOOD', '1d2'), ('BITE', 'EAT_FOOD', '1d3')]
	flags = ['WILD_DUNGEON_01', 'MULTIPLY', 'IM_POIS', 'HURT_LITE', 'EVIL', 'DEMON', 'OPEN_DOOR', 'TAKE_ITEM', 'CAN_SWIM']
	sfreq = 0
	sflags = []
	description = """Don't splash water on them, and don't feed them after midnight!
"""

monsters.register_race(gremlin)


class yeti(monster):
	r_idx = "154"
	name = "Yeti"
	ascii = "Y,w"
	level = 9
	rarity = 3
	hp = "8d9"
	speed = 110
	aaf = 20
	ac = 12
	sleep = 10
	exp = 30
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d4')]
	flags = ['OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_CORPSE', 'ANIMAL', 'IM_COLD']
	sfreq = 0
	sflags = []
	description = """A large white figure covered in shaggy fur.
"""

monsters.register_race(yeti)


class bloodshot_icky_thing(monster, spellcaster):
	r_idx = "155"
	name = "Bloodshot icky thing"
	ascii = "i,r"
	level = 9
	rarity = 3
	hp = "5d8"
	speed = 110
	aaf = 14
	ac = 10
	sleep = 20
	exp = 24
	blows = [('TOUCH', 'HURT', '1d4'), ('CRAWL', 'ACID', '2d4')]
	flags = ['RAND_50', 'EMPTY_MIND', 'CAN_SWIM', 'DROP_CORPSE', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_WASTE1']
	sfreq = 11
	sflags = ['DRAIN_MANA']
	description = """It is a strange, slimy, icky creature.
"""

monsters.register_race(bloodshot_icky_thing)


class giant_grey_rat(monster):
	r_idx = "156"
	name = "Giant grey rat"
	ascii = "r,s"
	level = 9
	rarity = 1
	hp = "2d2"
	speed = 110
	aaf = 8
	ac = 6
	sleep = 20
	exp = 2
	blows = [('BITE', 'POISON', '1d4')]
	flags = ['RAND_25', 'MULTIPLY', 'WILD_DUNGEON_01', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It is a rodent of unusual size.
"""

monsters.register_race(giant_grey_rat)


class black_harpy(monster):
	r_idx = "157"
	name = "Black harpy"
	ascii = "H,D"
	level = 9
	rarity = 1
	hp = "2d9"
	speed = 120
	aaf = 16
	ac = 11
	sleep = 10
	exp = 19
	blows = [('CLAW', 'HURT', '1d2'), ('CLAW', 'HURT', '1d2'), ('BITE', 'HURT', '1d3')]
	flags = ['FEMALE', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_MOUNT2', 'DROP_CORPSE', 'RAND_25', 'ANIMAL', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A woman's face on the body of a vicious black bird.
"""

monsters.register_race(black_harpy)


class skaven(monster):
	r_idx = "158"
	name = "Skaven"
	ascii = "r,G"
	level = 9
	rarity = 1
	hp = "8d9"
	speed = 110
	aaf = 15
	ac = 20
	sleep = 20
	exp = 20
	blows = [('HIT', 'HURT', '1d4'), ('HIT', 'HURT', '1d4')]
	flags = ['EVIL', 'FRIENDS', 'DROP_60', 'DROP_90', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'MALE', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'WILD_SWAMP1', 'WILD_SWAMP2']
	sfreq = 0
	sflags = []
	description = """A mutated rat-creature from the great waste, it is vaguely 
humanoid in appearance and walks on its hind legs. This race
serves chaos fervently and is greatly feared by others.
"""

monsters.register_race(skaven)


class the_wounded_bear(monster):
	r_idx = "159"
	name = "The wounded bear"
	ascii = "q,r"
	level = 9
	rarity = 1
	hp = "8d10"
	speed = 110
	aaf = 10
	ac = 18
	sleep = 10
	exp = 25
	blows = [('CLAW', 'HURT', '1d5'), ('CLAW', 'HURT', '1d5'), ('BITE', 'HURT', '1d10')]
	flags = ['BASH_DOOR', 'FORCE_MAXHP', 'FORCE_SLEEP', 'UNIQUE', 'DROP_CORPSE', 'ANIMAL', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_GRASS', 'WILD_MOUNT1']
	sfreq = 0
	sflags = []
	description = """A wounded bear, who has occasionally attacked humans.
"""

monsters.register_race(the_wounded_bear)


class portuguese_man_o_war(monster):
	r_idx = "160"
	name = "Portuguese man-o-war"
	ascii = "j,v"
	level = 9
	rarity = 2
	hp = "8d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 75
	exp = 25
	blows = [('TOUCH', 'PARALYZE', '1d6'), ('TOUCH', 'PARALYZE', '1d6')]
	flags = ['ANIMAL', 'AQUATIC', 'IM_POIS', 'WILD_SHORE', 'WILD_OCEAN']
	sfreq = 0
	sflags = []
	description = """A strange water creature, whose touch can be deadly.
"""

monsters.register_race(portuguese_man_o_war)


class rock_mole(monster):
	r_idx = "161"
	name = "Rock mole"
	ascii = "r,s"
	level = 9
	rarity = 2
	hp = "9d11"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 75
	exp = 25
	blows = [('BITE', 'HURT', '1d10'), ('BITE', 'HURT', '1d10')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'KILL_WALL', 'KILL_ITEM', 'DROP_CORPSE', 'ANIMAL', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """Despite its minuscule size, this mole creature has fangs powerful 
enough to bore through solid rock.
"""

monsters.register_race(rock_mole)


class orc_shaman(orc, spellcaster):
	r_idx = "162"
	name = "Orc shaman"
	ascii = "o,b"
	level = 9
	rarity = 1
	hp = "6d8"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 20
	exp = 30
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6')]
	flags = ['MALE', 'FORCE_SLEEP', 'DROP_90', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_SWAMP1', 'WILD_SWAMP2', 'WILD_MOUNT1', 'WILD_MOUNT2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'ORC', 'HURT_LITE']
	sfreq = 8
	sflags = ['BLINK', 'CAUSE_1', 'MISSILE']
	description = """An orc dressed in skins who gestures wildly.
"""

monsters.register_race(orc_shaman)


class baby_blue_dragon(dragon, spellcaster):
	r_idx = "163"
	name = "Baby blue dragon"
	ascii = "d,B"
	level = 9
	rarity = 2
	hp = "7d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 70
	exp = 35
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'DROP_CORPSE', 'ONLY_GOLD', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_ELEC']
	sfreq = 11
	sflags = ['BR_ELEC']
	description = """This hatchling dragon is still soft, its eyes unaccustomed to light and 
its scales a pale blue.
"""

monsters.register_race(baby_blue_dragon)


class baby_white_dragon(dragon, spellcaster):
	r_idx = "164"
	name = "Baby white dragon"
	ascii = "d,W"
	level = 9
	rarity = 2
	hp = "7d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 70
	exp = 35
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'CAN_FLY', 'DROP_CORPSE', 'ONLY_GOLD', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DRAGON', 'IM_COLD']
	sfreq = 11
	sflags = ['BR_COLD']
	description = """This hatchling dragon is still soft, its eyes unaccustomed to light and 
its scales a pale white.
"""

monsters.register_race(baby_white_dragon)


class baby_green_dragon(dragon, spellcaster):
	r_idx = "165"
	name = "Baby green dragon"
	ascii = "d,G"
	level = 9
	rarity = 2
	hp = "7d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 70
	exp = 35
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'ONLY_GOLD', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'DROP_CORPSE', 'EVIL', 'DRAGON', 'IM_POIS']
	sfreq = 11
	sflags = ['BR_POIS']
	description = """This hatchling dragon is still soft, its eyes unaccustomed to light and 
its scales a sickly green.
"""

monsters.register_race(baby_green_dragon)


class baby_black_dragon(dragon, spellcaster):
	r_idx = "166"
	name = "Baby black dragon"
	ascii = "d,s"
	level = 9
	rarity = 2
	hp = "7d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 70
	exp = 35
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'CAN_FLY', 'DROP_CORPSE', 'ONLY_GOLD', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DRAGON', 'IM_ACID']
	sfreq = 11
	sflags = ['BR_ACID']
	description = """This hatchling dragon is still soft, its eyes unaccustomed to light and 
its scales a dull black.
"""

monsters.register_race(baby_black_dragon)


class baby_red_dragon(dragon, spellcaster):
	r_idx = "167"
	name = "Baby red dragon"
	ascii = "d,R"
	level = 9
	rarity = 2
	hp = "8d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 70
	exp = 35
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'CAN_FLY', 'DROP_CORPSE', 'ONLY_GOLD', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DRAGON', 'IM_FIRE']
	sfreq = 11
	sflags = ['BR_FIRE']
	description = """This hatchling dragon is still soft, its eyes unaccustomed to light and 
its scales a pale red.
"""

monsters.register_race(baby_red_dragon)


class giant_red_ant(monster):
	r_idx = "168"
	name = "Giant red ant"
	ascii = "a,R"
	level = 9
	rarity = 2
	hp = "3d7"
	speed = 110
	aaf = 12
	ac = 17
	sleep = 60
	exp = 22
	blows = [('BITE', 'HURT', '1d4'), ('STING', 'LOSE_STR', '1d4')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'DROP_SKELETON', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is large and has venomous mandibles.
"""

monsters.register_race(giant_red_ant)


class brodda_the_easterling(monster):
	r_idx = "169"
	name = "Brodda, the Easterling"
	ascii = "p,o"
	level = 9
	rarity = 2
	hp = "15d10"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 20
	exp = 100
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12')]
	flags = ['UNIQUE', 'MALE', 'EVIL', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'CAN_SPEAK', 'WILD_WASTE1', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 0
	sflags = []
	description = """A nasty piece of work, Brodda picks on defenseless women and children.
"And Morwen was gone.  Empty stood her house, broken and cold.  It was 
more than a year since she departed to Doriath.  Brodda the Easterling 
(who had wedded Morwen's kinswoman Airin) had plundered her house, and 
taken all that was left of her goods." 
"""

monsters.register_race(brodda_the_easterling)


class bloodfang_the_wolf(monster):
	r_idx = "170"
	name = "Bloodfang the Wolf"
	ascii = "C,R"
	level = 9
	rarity = 1
	hp = "5d6"
	speed = 120
	aaf = 30
	ac = 18
	sleep = 20
	exp = 30
	blows = [('BITE', 'HURT', '1d10'), ('BITE', 'HURT', '1d10')]
	flags = ['BASH_DOOR', 'WILD_GRASS', 'WILD_FOREST1', 'DROP_CORPSE', 'ANIMAL', 'UNIQUE', 'FORCE_MAXHP']
	sfreq = 0
	sflags = []
	description = """It has been terrorizing the nearby villages.
"""

monsters.register_race(bloodfang_the_wolf)


class king_cobra(monster):
	r_idx = "171"
	name = "King cobra"
	ascii = "J,g"
	level = 9
	rarity = 2
	hp = "5d10"
	speed = 110
	aaf = 8
	ac = 15
	sleep = 1
	exp = 28
	blows = [('SPIT', 'BLIND', '1d2'), ('BITE', 'POISON', '3d4')]
	flags = ['RAND_50', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'WILD_FOREST1', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'CAN_SWIM', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It is a large snake with a hooded face.
"""

monsters.register_race(king_cobra)


class eagle(monster):
	r_idx = "172"
	name = "Eagle"
	ascii = "B,u"
	level = 9
	rarity = 2
	hp = "7d8"
	speed = 120
	aaf = 30
	ac = 20
	sleep = 10
	exp = 22
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d6')]
	flags = ['ANIMAL', 'CAN_FLY', 'WILD_WASTE1', 'WILD_MOUNT2', 'WILD_FOREST2', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A magnificent huge predatory bird.
"""

monsters.register_race(eagle)


class war_bear(monster):
	r_idx = "173"
	name = "War bear"
	ascii = "q,u"
	level = 9
	rarity = 1
	hp = "7d10"
	speed = 110
	aaf = 10
	ac = 25
	sleep = 10
	exp = 25
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d4'), ('BITE', 'HURT', '1d6')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'FRIENDS', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """Bears with tusks, trained to kill.
"""

monsters.register_race(war_bear)


class killer_bee(monster):
	r_idx = "174"
	name = "Killer bee"
	ascii = "I,y"
	level = 9
	rarity = 2
	hp = "2d3"
	speed = 120
	aaf = 12
	ac = 25
	sleep = 10
	exp = 22
	blows = [('STING', 'POISON', '1d4'), ('STING', 'LOSE_STR', '1d4')]
	flags = ['WEIRD_MIND', 'FRIENDS', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is poisonous and aggressive.
"""

monsters.register_race(killer_bee)


class giant_spider(monster):
	r_idx = "175"
	name = "Giant spider"
	ascii = "S,s"
	level = 10
	rarity = 2
	hp = "7d10"
	speed = 110
	aaf = 8
	ac = 10
	sleep = 80
	exp = 35
	blows = [('BITE', 'HURT', '1d10'), ('BITE', 'POISON', '1d6'), ('BITE', 'POISON', '1d6'), ('BITE', 'HURT', '1d10')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'WILD_WASTE1', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It is a vast black spider whose bulbous body is bloated with poison.
"""

monsters.register_race(giant_spider)


class giant_white_tick(monster):
	r_idx = "176"
	name = "Giant white tick"
	ascii = "S,w"
	level = 10
	rarity = 2
	hp = "8d8"
	speed = 100
	aaf = 12
	ac = 15
	sleep = 20
	exp = 27
	blows = [('BITE', 'POISON', '2d6')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'CAN_FLY', 'ANIMAL', 'IM_POIS', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """It is moving slowly towards you.
"""

monsters.register_race(giant_white_tick)


class the_borshin(monster):
	r_idx = "177"
	name = "The Borshin"
	ascii = "g,w"
	level = 10
	rarity = 2
	hp = "9d15"
	speed = 110
	aaf = 40
	ac = 20
	sleep = 0
	exp = 45
	blows = [('HIT', 'HURT', '2d11'), ('CRUSH', 'HURT', '2d15'), ('TOUCH', 'TERRIFY', '')]
	flags = ['WILD_DUNGEON_01', 'BASH_DOOR', 'UNIQUE', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'IM_POIS', 'IM_COLD', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """Pallid and twisted, this creature hates the very sight of you.
"It looked like something that had started out to be a man but had never 
quite made it. It had been stepped on, twisted, had holes poked into the 
sickly dough of its head-bulge. Bones showed through the transparent flesh 
of its torso and its short legs were as thick as trees, terminating in 
disk-shaped pads from which dozens of long toes hung like roots or worms. 
its arms were longer than its entire body. it was a crushed slug, a thing 
that had been frozen and thawed before it was fully baked. It was - 
'It is the Borshin', said the Lord of Bats."
"""

monsters.register_race(the_borshin)


class dark_elven_mage(monster, spellcaster):
	r_idx = "178"
	name = "Dark elven mage"
	ascii = "h,v"
	level = 10
	rarity = 1
	hp = "5d10"
	speed = 120
	aaf = 20
	ac = 10
	sleep = 20
	exp = 50
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_POIS', 'HURT_LITE']
	sfreq = 5
	sflags = ['BLIND', 'CONF', 'MISSILE', 'DARKNESS', 'BA_POIS']
	description = """A dark elven figure, dressed all in black, hurling spells at you.
"""

monsters.register_race(dark_elven_mage)


class kamikaze_yeek(monster):
	r_idx = "179"
	name = "Kamikaze yeek"
	ascii = "y,u"
	level = 10
	rarity = 1
	hp = "3d8"
	speed = 113
	aaf = 18
	ac = 15
	sleep = 10
	exp = 10
	blows = [('EXPLODE', 'HURT', '15d2')]
	flags = ['WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'ANIMAL', 'IM_ACID', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """The evil wizard Bruce has trained them to be living weapons.
"""

monsters.register_race(kamikaze_yeek)


class orfax_son_of_boldor(monster, spellcaster):
	r_idx = "180"
	name = "Orfax, Son of Boldor"
	ascii = "y,B"
	level = 10
	rarity = 3
	hp = "8d10"
	speed = 120
	aaf = 18
	ac = 16
	sleep = 10
	exp = 80
	blows = [('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d8'), ('INSULT', '', ''), ('INSULT', '', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ESCORTS', 'ONLY_ITEM', 'DROP_90', 'DROP_GOOD', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'ANIMAL', 'EVIL', 'IM_ACID']
	sfreq = 4
	sflags = ['HEAL', 'BLINK', 'TELE_TO', 'SLOW', 'CONF', 'S_MONSTER']
	description = """He's just like daddy!  He knows mighty spells, but fortunately he is a 
yeek.
"""

monsters.register_race(orfax_son_of_boldor)


class servant_of_glaaki(monster, undead, spellcaster):
	r_idx = "181"
	name = "Servant of Glaaki"
	ascii = "z,G"
	level = 10
	rarity = 1
	hp = "7d11"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 20
	exp = 25
	blows = [('CRUSH', 'HURT', '1d8'), ('CLAW', 'DISEASE', '1d3')]
	flags = ['WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'FRIENDS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'UNDEAD', 'EVIL', 'NO_FEAR', 'IM_POIS', 'IM_COLD', 'COLD_BLOOD']
	sfreq = 12
	sflags = ['CAUSE_1', 'SCARE']
	description = """"...the hand of a corpse -- bloodless and skeletal, and with 
impossibly long, cracked nails."
"""

monsters.register_race(servant_of_glaaki)


class dark_elven_warrior(monster, spellcaster):
	r_idx = "182"
	name = "Dark elven warrior"
	ascii = "h,D"
	level = 10
	rarity = 1
	hp = "7d11"
	speed = 110
	aaf = 20
	ac = 14
	sleep = 20
	exp = 50
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'HURT_LITE']
	sfreq = 12
	sflags = ['MISSILE']
	description = """A dark elven figure in armour and ready with his sword.
"""

monsters.register_race(dark_elven_warrior)


class sand_dweller(monster):
	r_idx = "183"
	name = "Sand-dweller"
	ascii = "u,U"
	level = 10
	rarity = 1
	hp = "7d9"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 20
	exp = 30
	blows = [('CLAW', 'HURT', '1d6'), ('CLAW', 'HURT', '1d6')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'DROP_SKELETON', 'OPEN_DOOR', 'BASH_DOOR', 'HURT_LITE', 'EVIL', 'DROP_60', 'DROP_90', 'MALE']
	sfreq = 0
	sflags = []
	description = """"Rough-skinned, large-eyed, large-eared, with a horrible, 
distorted resemblance to the koala bear facially, though 
his body had an appearance of emaciation."
"""

monsters.register_race(sand_dweller)


class clear_mushroom_patch(monster):
	r_idx = "184"
	name = "Clear mushroom patch"
	ascii = ",,w"
	level = 10
	rarity = 2
	hp = "1d1"
	speed = 120
	aaf = 4
	ac = 1
	sleep = 0
	exp = 3
	blows = [('SPORE', 'HURT', '1d1')]
	flags = ['ATTR_CLEAR', 'WILD_DUNGEON_01', 'NEVER_MOVE', 'INVISIBLE', 'COLD_BLOOD', 'MULTIPLY', 'STUPID', 'EMPTY_MIND', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """Yum!  It looks quite tasty.
"""

monsters.register_race(clear_mushroom_patch)


class quiver_slot(monster, spellcaster):
	r_idx = "185"
	name = "Quiver slot"
	ascii = ",,U"
	level = 10
	rarity = 2
	hp = "1d1"
	speed = 120
	aaf = 4
	ac = 1
	sleep = 0
	exp = 3
	blows = [('SPORE', 'CONFUSE', '1d1')]
	flags = ['WILD_DUNGEON_01', 'NEVER_MOVE', 'COLD_BLOOD', 'MULTIPLY', 'STUPID', 'EMPTY_MIND', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 5
	sflags = ['ARROW_1']
	description = """It looks weird.
"""

monsters.register_race(quiver_slot)


class grishnakh_the_hill_orc(orc):
	r_idx = "186"
	name = "Grishnakh, the Hill Orc"
	ascii = "o,u"
	level = 10
	rarity = 3
	hp = "16d10"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 20
	exp = 160
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d10')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'FORCE_MAXHP', 'ESCORT', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """He is a cunning and devious orc. 
"Grishnakh [was] a short crook-legged creature, very broad and with 
long arms that hung almost to the ground." 
"""

monsters.register_race(grishnakh_the_hill_orc)


class giant_piranha(monster):
	r_idx = "187"
	name = "Giant piranha"
	ascii = "l,B"
	level = 10
	rarity = 2
	hp = "5d8"
	speed = 120
	aaf = 30
	ac = 20
	sleep = 10
	exp = 40
	blows = [('BITE', 'HURT', '9d1'), ('BITE', 'HURT', '9d1')]
	flags = ['NO_SLEEP', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_FOREST2', 'FRIENDS', 'AQUATIC', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A very large and bloodthirsty fish.
"""

monsters.register_race(giant_piranha)


class owlbear(monster):
	r_idx = "188"
	name = "Owlbear"
	ascii = "H,o"
	level = 10
	rarity = 1
	hp = "12d8"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 20
	exp = 35
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('CRUSH', 'HURT', '1d10')]
	flags = ['WILD_DUNGEON_01', 'EVIL', 'ANIMAL', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A bizarre bear-creature with the claws and the face of an owl.
"""

monsters.register_race(owlbear)


class blue_horror(demon):
	r_idx = "189"
	name = "Blue horror"
	ascii = "u,B"
	level = 10
	rarity = 3
	hp = "11d9"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 20
	exp = 25
	blows = [('CLAW', 'TERRIFY', '1d8'), ('CLAW', 'TERRIFY', '1d8')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_FEAR', 'NO_CONF']
	sfreq = 0
	sflags = []
	description = """An ugly screaming little demon servant of Tzeentch.
"""

monsters.register_race(blue_horror)


class hairy_mold(monster):
	r_idx = "190"
	name = "Hairy mold"
	ascii = "m,U"
	level = 10
	rarity = 2
	hp = "11d8"
	speed = 110
	aaf = 2
	ac = 6
	sleep = 70
	exp = 32
	blows = [('HIT', 'POISON', '1d3')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a strange hairy growth on the dungeon floor.
"""

monsters.register_race(hairy_mold)


class grizzly_bear(monster):
	r_idx = "191"
	name = "Grizzly bear"
	ascii = "q,u"
	level = 10
	rarity = 1
	hp = "10d11"
	speed = 110
	aaf = 10
	ac = 25
	sleep = 10
	exp = 25
	blows = [('CLAW', 'HURT', '1d5'), ('CLAW', 'HURT', '1d5'), ('BITE', 'HURT', '1d10')]
	flags = ['BASH_DOOR', 'WILD_FOREST2', 'WILD_MOUNT1', 'WILD_MOUNT2', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A huge, beastly bear.
"""

monsters.register_race(grizzly_bear)


class disenchanter_mold(monster, spellcaster):
	r_idx = "192"
	name = "Disenchanter mold"
	ascii = "m,v"
	level = 10
	rarity = 2
	hp = "11d8"
	speed = 110
	aaf = 2
	ac = 6
	sleep = 50
	exp = 40
	blows = [('TOUCH', 'UN_BONUS', '1d6')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'RES_DISE', 'IM_POIS', 'ATTR_MULTI', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 11
	sflags = ['DRAIN_MANA']
	description = """It is a strange glowing growth on the dungeon floor.
"""

monsters.register_race(disenchanter_mold)


class pseudo_dragon(dragon, spellcaster):
	r_idx = "193"
	name = "Pseudo dragon"
	ascii = "d,v"
	level = 10
	rarity = 2
	hp = "14d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 40
	exp = 150
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'CAN_FLY', 'DROP_CORPSE', 'DROP_60', 'BASH_DOOR', 'DRAGON']
	sfreq = 11
	sflags = ['CONF', 'SCARE', 'BR_LITE', 'BR_DARK']
	description = """A small relative of the dragon that inhabits dark caves.
"""

monsters.register_race(pseudo_dragon)


class tengu(demon, spellcaster):
	r_idx = "194"
	name = "Tengu"
	ascii = "u,b"
	level = 10
	rarity = 1
	hp = "11d9"
	speed = 120
	aaf = 20
	ac = 16
	sleep = 30
	exp = 40
	blows = [('HIT', 'HURT', '1d8')]
	flags = ['OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_FEAR', 'RES_TELE', 'CAN_FLY']
	sfreq = 3
	sflags = ['BLINK', 'TELE_TO', 'TELE_AWAY', 'TPORT']
	description = """It is a fast-moving demon that blinks quickly in and out of existence; no 
other demon matches its teleporting mastery.
"""

monsters.register_race(tengu)


class creeping_gold_coins(monster):
	r_idx = "195"
	name = "Creeping gold coins"
	ascii = "$,y"
	level = 10
	rarity = 3
	hp = "12d8"
	speed = 100
	aaf = 5
	ac = 18
	sleep = 10
	exp = 32
	blows = [('HIT', 'HURT', '2d5'), ('TOUCH', 'POISON', '3d5')]
	flags = ['ONLY_GOLD', 'DROP_90', 'DROP_1D2', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'BASH_DOOR', 'ANIMAL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """It is a pile of coins, crawling forward on thousands of tiny legs.
"""

monsters.register_race(creeping_gold_coins)


class wolf(monster):
	r_idx = "196"
	name = "Wolf"
	ascii = "C,u"
	level = 10
	rarity = 1
	hp = "4d7"
	speed = 120
	aaf = 30
	ac = 15
	sleep = 20
	exp = 30
	blows = [('BITE', 'HURT', '1d6')]
	flags = ['RAND_25', 'FRIENDS', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_WASTE1', 'WILD_WASTE2', 'WILD_MOUNT1', 'WILD_MOUNT2', 'ANIMAL', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It howls and snaps at you.
"""

monsters.register_race(wolf)


class giant_fruit_fly(monster):
	r_idx = "197"
	name = "Giant fruit fly"
	ascii = "I,U"
	level = 10
	rarity = 6
	hp = "1d4"
	speed = 120
	aaf = 8
	ac = 7
	sleep = 10
	exp = 4
	blows = [('BITE', 'HURT', '1d2')]
	flags = ['RAND_50', 'RAND_25', 'CAN_FLY', 'WILD_DUNGEON_01', 'MULTIPLY', 'WEIRD_MIND', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A fast-breeding, annoying pest.
"""

monsters.register_race(giant_fruit_fly)


class panther(monster):
	r_idx = "198"
	name = "Panther"
	ascii = "f,D"
	level = 10
	rarity = 2
	hp = "7d8"
	speed = 120
	aaf = 40
	ac = 15
	sleep = 2
	exp = 25
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8')]
	flags = ['BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_GRASS', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A large black cat, stalking you with intent.  It thinks you're its next 
meal.
"""

monsters.register_race(panther)


class tax_collector(monster):
	r_idx = "199"
	name = "Tax collector"
	ascii = "p,D"
	level = 10
	rarity = 3
	hp = "6d8"
	speed = 110
	aaf = 20
	ac = 16
	sleep = 10
	exp = 35
	blows = [('TOUCH', 'EAT_GOLD', ''), ('TOUCH', 'EAT_ITEM', '')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_1D2', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """He is eyeing your purse suspiciously.
"""

monsters.register_race(tax_collector)


class hobbes_the_tiger(monster):
	r_idx = "200"
	name = "Hobbes the Tiger"
	ascii = "f,y"
	level = 10
	rarity = 2
	hp = "8d10"
	speed = 120
	aaf = 40
	ac = 15
	sleep = 0
	exp = 45
	blows = [('CLAW', 'HURT', '1d11'), ('CLAW', 'HURT', '1d11'), ('BITE', 'HURT', '1d4')]
	flags = ['WILD_DUNGEON_01', 'BASH_DOOR', 'UNIQUE', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'ANIMAL', 'MALE', 'CAN_SPEAK', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """Fast-moving, with a taste for tuna sandwiches.
"""

monsters.register_race(hobbes_the_tiger)


class shadow_creature_of_fiona(monster):
	r_idx = "201"
	name = "Shadow Creature of Fiona"
	ascii = "h,s"
	level = 10
	rarity = 2
	hp = "7d8"
	speed = 110
	aaf = 12
	ac = 10
	sleep = 16
	exp = 35
	blows = [('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '1d7')]
	flags = ['WILD_DUNGEON_01', 'BASH_DOOR', 'OPEN_DOOR', 'FRIENDS', 'DROP_60', 'IM_POIS', 'NO_SLEEP', 'NO_CONF', 'MALE', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """"There was something unusual about their appearance... For one thing, 
all had uniformly bloodshot eyes. Very, very bloodshot eyes. With them, 
though, the condition seemed normal. For another, all had an extra joint 
to each finger and thumb, and sharp, forward-curving spurs on the backs 
of their hands. All of them had prominent jaws (and) forty-four teeth, 
most of them longer than human teeth, and several looking to be much 
sharper. Their flesh was grayish and hard and shiny. There were 
undoubtedly other differences also, but those were sufficient to prove 
a point of some sort."
"""

monsters.register_race(shadow_creature_of_fiona)


class undead_mass(monster, undead):
	r_idx = "202"
	name = "Undead mass"
	ascii = "j,u"
	level = 10
	rarity = 2
	hp = "6d8"
	speed = 110
	aaf = 70
	ac = 10
	sleep = 5
	exp = 33
	blows = [('TOUCH', 'DISEASE', '1d6'), ('TOUCH', 'LOSE_CON', '1d6')]
	flags = ['WILD_DUNGEON_01', 'UNDEAD', 'EMPTY_MIND', 'NO_CONF', 'NO_SLEEP', 'IM_POIS', 'IM_COLD', 'NO_FEAR', 'HURT_LITE', 'COLD_BLOOD', 'EVIL', 'NEVER_MOVE', 'MULTIPLY']
	sfreq = 0
	sflags = []
	description = """A sickening mound of decaying flesh, bones, hands and so on. It seems to 
be growing.
"""

monsters.register_race(undead_mass)


class chaos_shapechanger(monster, spellcaster):
	r_idx = "203"
	name = "Chaos shapechanger"
	ascii = "H,v"
	level = 11
	rarity = 2
	hp = "15d9"
	speed = 110
	aaf = 10
	ac = 10
	sleep = 12
	exp = 38
	blows = [('HIT', 'HURT', '1d5'), ('HIT', 'HURT', '1d5'), ('HIT', 'CONFUSE', '1d3')]
	flags = ['WILD_DUNGEON_01', 'DROP_60', 'EVIL', 'SHAPECHANGER', 'ATTR_MULTI', 'ATTR_ANY']
	sfreq = 5
	sflags = ['BO_FIRE', 'BO_COLD', 'CONF']
	description = """A vaguely humanoid form constantly changing its appearance.
"""

monsters.register_race(chaos_shapechanger)


class baby_multi_hued_dragon(dragon, spellcaster):
	r_idx = "204"
	name = "Baby multi-hued dragon"
	ascii = "d,v"
	level = 11
	rarity = 2
	hp = "8d9"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 60
	exp = 45
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BITE', 'HURT', '1d5')]
	flags = ['ATTR_MULTI', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'ONLY_GOLD', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'DRAGON', 'CAN_FLY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS']
	sfreq = 11
	sflags = ['BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_POIS']
	description = """This hatchling dragon is still soft, its eyes unaccustomed to light and 
its scales shimmering with a hint of colour.
"""

monsters.register_race(baby_multi_hued_dragon)


class vorpal_bunny(monster, spellcaster):
	r_idx = "205"
	name = "Vorpal bunny"
	ascii = "r,w"
	level = 11
	rarity = 3
	hp = "7d10"
	speed = 120
	aaf = 40
	ac = 20
	sleep = 0
	exp = 40
	blows = [('BITE', 'HURT', '6d1'), ('BITE', 'HURT', '7d1')]
	flags = ['BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 8
	sflags = ['BLINK']
	description = """It looks very cute, except for the razor sharp teeth. It moans 
ominously as it jumps at your throat!
"""

monsters.register_race(vorpal_bunny)


class old_man_willow(monster, spellcaster):
	r_idx = "206"
	name = "Old Man Willow"
	ascii = "%,s"
	level = 11
	rarity = 5
	hp = "16d20"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 20
	exp = 150
	blows = [('TOUCH', 'PARALYZE', ''), ('TOUCH', 'PARALYZE', ''), ('CRUSH', 'HURT', '2d12')]
	flags = ['ANIMAL', 'NEVER_MOVE', 'COLD_BLOOD', 'WILD_FOREST2', 'EMPTY_MIND', 'UNIQUE', 'FORCE_MAXHP', 'FORCE_SLEEP', 'RES_WATE', 'IM_POIS', 'IM_ACID', 'DROP_90', 'DROP_GOOD', 'ONLY_ITEM']
	sfreq = 10
	sflags = ['TELE_TO']
	description = """"...a huge willow-tree, old and hoary. Enormous it looked, its 
sprawling branches going up like racing arms with may long-
lingered hands, its knotted and twisted trunk gaping in wide 
fissures that creaked faintly as the boughs moved."
"""

monsters.register_race(old_man_willow)


class hippocampus(monster):
	r_idx = "207"
	name = "Hippocampus"
	ascii = "H,B"
	level = 11
	rarity = 1
	hp = "14d9"
	speed = 110
	aaf = 12
	ac = 7
	sleep = 10
	exp = 30
	blows = [('BITE', 'HURT', '2d5'), ('BITE', 'HURT', '2d5')]
	flags = ['AQUATIC', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A truly strange hybrid of a horse and a fish.
"""

monsters.register_race(hippocampus)


class zombified_orc(orc, undead):
	r_idx = "208"
	name = "Zombified orc"
	ascii = "z,s"
	level = 11
	rarity = 1
	hp = "8d8"
	speed = 110
	aaf = 20
	ac = 12
	sleep = 25
	exp = 30
	blows = [('HIT', 'HURT', '1d4'), ('HIT', 'HURT', '1d4'), ('HIT', 'HURT', '1d4')]
	flags = ['WILD_DUNGEON_01', 'COLD_BLOOD', 'EMPTY_MIND', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a shambling orcish corpse leaving behind a trail of flesh.
"""

monsters.register_race(zombified_orc)


class hippogriff(monster):
	r_idx = "209"
	name = "Hippogriff"
	ascii = "H,U"
	level = 11
	rarity = 1
	hp = "14d9"
	speed = 110
	aaf = 12
	ac = 7
	sleep = 10
	exp = 30
	blows = [('HIT', 'HURT', '2d5'), ('BITE', 'HURT', '2d5')]
	flags = ['BASH_DOOR', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_GRASS', 'ANIMAL', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A strange hybrid of eagle, lion and horse.  It looks weird.
"""

monsters.register_race(hippogriff)


class black_mamba(monster):
	r_idx = "210"
	name = "Black mamba"
	ascii = "J,D"
	level = 12
	rarity = 3
	hp = "7d8"
	speed = 120
	aaf = 10
	ac = 14
	sleep = 1
	exp = 40
	blows = [('BITE', 'POISON', '4d4')]
	flags = ['RAND_50', 'BASH_DOOR', 'CAN_SWIM', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_SWAMP1', 'WILD_SWAMP2', 'ANIMAL', 'IM_POIS', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It has glistening black skin, a sleek body and highly venomous fangs.
"""

monsters.register_race(black_mamba)


class white_wolf(monster):
	r_idx = "211"
	name = "White wolf"
	ascii = "C,w"
	level = 12
	rarity = 1
	hp = "5d7"
	speed = 120
	aaf = 30
	ac = 14
	sleep = 20
	exp = 30
	blows = [('BITE', 'HURT', '1d3'), ('BITE', 'HURT', '1d4')]
	flags = ['RAND_25', 'FRIENDS', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'ANIMAL', 'IM_COLD', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A large and muscled wolf from the northern wastes.  Its breath is cold and 
icy and its fur coated in frost.
"""

monsters.register_race(white_wolf)


class grape_jelly(monster, spellcaster):
	r_idx = "212"
	name = "Grape jelly"
	ascii = "j,v"
	level = 12
	rarity = 3
	hp = "36d8"
	speed = 110
	aaf = 2
	ac = 1
	sleep = 99
	exp = 60
	blows = [('TOUCH', 'EXP_10', '')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 11
	sflags = ['DRAIN_MANA']
	description = """It is a pulsing mound of glowing flesh.
"""

monsters.register_race(grape_jelly)


class nether_worm_mass(monster):
	r_idx = "213"
	name = "Nether worm mass"
	ascii = "w,D"
	level = 12
	rarity = 4
	hp = "3d9"
	speed = 100
	aaf = 10
	ac = 6
	sleep = 3
	exp = 6
	blows = [('TOUCH', 'EXP_10', '')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'CAN_SWIM', 'STUPID', 'WEIRD_MIND', 'MULTIPLY', 'BASH_DOOR', 'ANIMAL', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a disgusting mass of dark worms, eating each other, the floor, 
the air, you....
"""

monsters.register_race(nether_worm_mass)


class abyss_worm_mass(monster):
	r_idx = "214"
	name = "Abyss worm mass"
	ascii = "w,D"
	level = 12
	rarity = 4
	hp = "4d7"
	speed = 100
	aaf = 10
	ac = 7
	sleep = 3
	exp = 7
	blows = [('CRAWL', 'EXP_10', '')]
	flags = ['RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'STUPID', 'WEIRD_MIND', 'MULTIPLY', 'BASH_DOOR', 'EVIL', 'CAN_SWIM', 'ANIMAL', 'HURT_LITE', 'NO_FEAR', 'KILL_WALL', 'COLD_BLOOD', 'INVISIBLE']
	sfreq = 0
	sflags = []
	description = """Even more disgusting dark worms, their essence that of unbeing.
"""

monsters.register_race(abyss_worm_mass)


class golfimbul_the_hill_orc_chief(orc):
	r_idx = "215"
	name = "Golfimbul, the Hill Orc Chief"
	ascii = "o,u"
	level = 12
	rarity = 3
	hp = "18d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 20
	exp = 230
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10')]
	flags = ['UNIQUE', 'MALE', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'ESCORT', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SPEAK', 'EVIL', 'ORC', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A leader of a band of raiding orcs, he picks on hobbits.
"""

monsters.register_race(golfimbul_the_hill_orc_chief)


class swordsman(monster):
	r_idx = "216"
	name = "Swordsman"
	ascii = "p,U"
	level = 12
	rarity = 1
	hp = "10d9"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 20
	exp = 40
	blows = [('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_1D2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 0
	sflags = []
	description = """A warrior of considerable skill.
"""

monsters.register_race(swordsman)


class skaven_shaman(monster, spellcaster):
	r_idx = "217"
	name = "Skaven shaman"
	ascii = "r,g"
	level = 9
	rarity = 1
	hp = "7d8"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 20
	exp = 36
	blows = [('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '1d7')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_SWAMP2', 'WILD_WASTE1', 'FORCE_SLEEP', 'DROP_90', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL']
	sfreq = 8
	sflags = ['BLINK', 'CAUSE_1', 'MISSILE', 'CONF', 'SCARE']
	description = """The shaman of a skaven tribe gets his powers from a mystic 
stone corrupted by chaos, called a Warp Stone.
"""

monsters.register_race(skaven_shaman)


class gazer(monster, spellcaster):
	r_idx = "218"
	name = "Gazer"
	ascii = "e,b"
	level = 12
	rarity = 1
	hp = "7d9"
	speed = 110
	aaf = 15
	ac = 9
	sleep = 40
	exp = 40
	blows = [('GAZE', 'PARALYZE', '1d2'), ('GAZE', 'CONFUSE', '1d2')]
	flags = ['CAN_FLY', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'DROP_CORPSE', 'EVIL']
	sfreq = 8
	sflags = ['HOLD', 'CONF']
	description = """A floating eye surrounded by number of smaller eyestalks. Its 
gaze seems mesmerizing.
"""

monsters.register_race(gazer)


class knight_archer(monster, spellcaster):
	r_idx = "219"
	name = "Knight archer"
	ascii = "p,U"
	level = 12
	rarity = 1
	hp = "7d11"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 20
	exp = 40
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 8
	sflags = ['ARROW_2']
	description = """A warrior trained in melee as well as missile weapons.
"""

monsters.register_race(knight_archer)


class ixitxachitl(monster):
	r_idx = "220"
	name = "Ixitxachitl"
	ascii = "l,s"
	level = 12
	rarity = 1
	hp = "9d8"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 20
	exp = 40
	blows = [('STING', 'POISON', '2d5'), ('STING', 'POISON', '2d5')]
	flags = ['ANIMAL', 'EVIL', 'AQUATIC', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_SHORE']
	sfreq = 0
	sflags = []
	description = """A devil ray of the depths.
"""

monsters.register_race(ixitxachitl)


class mine_dog(monster):
	r_idx = "221"
	name = "Mine-dog"
	ascii = "C,u"
	level = 12
	rarity = 4
	hp = "5d6"
	speed = 120
	aaf = 30
	ac = 15
	sleep = 20
	exp = 40
	blows = [('EXPLODE', 'HURT', '6d6')]
	flags = ['RAND_50', 'WILD_DUNGEON_01', 'FRIENDS', 'BASH_DOOR', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """An explosive charge has been attached to this poor animal, who 
has been trained to search for its target and detonate.
"""

monsters.register_race(mine_dog)


class hellcat(monster):
	r_idx = "222"
	name = "Hellcat"
	ascii = "f,R"
	level = 12
	rarity = 1
	hp = "7d8"
	speed = 120
	aaf = 20
	ac = 15
	sleep = 30
	exp = 40
	blows = [('CLAW', 'HURT', '1d5'), ('CLAW', 'HURT', '1d5'), ('BITE', 'HURT', '1d8')]
	flags = ['WILD_DUNGEON_01', 'ANIMAL', 'WEIRD_MIND', 'FRIENDS', 'RAND_25', 'IM_FIRE', 'EVIL']
	sfreq = 0
	sflags = []
	description = """It is as large as a tiger, its yellow eyes are pupilless.
"""

monsters.register_race(hellcat)


class moon_beast(monster, spellcaster):
	r_idx = "223"
	name = "Moon beast"
	ascii = "q,W"
	level = 12
	rarity = 1
	hp = "8d9"
	speed = 120
	aaf = 30
	ac = 15
	sleep = 20
	exp = 57
	blows = [('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3'), ('BUTT', 'HURT', '1d6')]
	flags = ['WILD_DUNGEON_01', 'DROP_1D2', 'ONLY_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'ANIMAL', 'DROP_CORPSE']
	sfreq = 6
	sflags = ['HEAL', 'BLIND', 'DARKNESS', 'CONF', 'CAUSE_2']
	description = """"Great greyish-white slippery things which could expand and 
contract at will, and whose principle shape... was that of a 
sort of toad without any eyes, but with a curious vibrating mass 
of short pink tentacles on the end of its blung, vague snout."
"""

monsters.register_race(moon_beast)


class master_yeek(monster, spellcaster):
	r_idx = "224"
	name = "Master yeek"
	ascii = "y,g"
	level = 12
	rarity = 2
	hp = "8d9"
	speed = 110
	aaf = 18
	ac = 15
	sleep = 10
	exp = 28
	blows = [('HIT', 'HURT', '1d8')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'ANIMAL', 'EVIL', 'IM_ACID', 'DROP_CORPSE']
	sfreq = 4
	sflags = ['BLINK', 'TPORT', 'BLIND', 'SLOW', 'BA_POIS', 'S_MONSTER']
	description = """A small humanoid that radiates some power.
"""

monsters.register_race(master_yeek)


class priest(monster, spellcaster):
	r_idx = "225"
	name = "Priest"
	ascii = "p,B"
	level = 12
	rarity = 1
	hp = "8d8"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 40
	exp = 36
	blows = [('HIT', 'HURT', '2d3'), ('HIT', 'HURT', '2d3')]
	flags = ['MALE', 'GOOD', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'DROP_1D2', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 3
	sflags = ['HEAL', 'SCARE', 'CAUSE_2', 'S_MONSTER']
	description = """A robed humanoid dedicated to his god.
"""

monsters.register_race(priest)


class dark_elven_priest(monster, spellcaster):
	r_idx = "226"
	name = "Dark elven priest"
	ascii = "h,b"
	level = 12
	rarity = 1
	hp = "5d11"
	speed = 120
	aaf = 20
	ac = 15
	sleep = 30
	exp = 50
	blows = [('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d10')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'ONLY_ITEM', 'DROP_1D2', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'HURT_LITE']
	sfreq = 5
	sflags = ['HEAL', 'BLIND', 'CONF', 'CAUSE_2', 'DARKNESS', 'MISSILE']
	description = """A dark elven figure, dressed all in black, chanting curses and waiting to 
deliver your soul to hell.
"""

monsters.register_race(dark_elven_priest)


class air_spirit(monster):
	r_idx = "227"
	name = "Air spirit"
	ascii = "E,B"
	level = 12
	rarity = 2
	hp = "6d8"
	speed = 130
	aaf = 12
	ac = 20
	sleep = 20
	exp = 40
	blows = [('HIT', 'HURT', '1d3')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'NONLIVING', 'EMPTY_MIND', 'INVISIBLE', 'COLD_BLOOD', 'BASH_DOOR', 'EVIL', 'IM_POIS', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A whirlwind of sentient air.
"""

monsters.register_race(air_spirit)


class skeleton_human(monster, undead):
	r_idx = "228"
	name = "Skeleton human"
	ascii = "s,w"
	level = 12
	rarity = 1
	hp = "7d8"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 30
	exp = 38
	blows = [('HIT', 'HURT', '1d8')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is an animated human skeleton.
"""

monsters.register_race(skeleton_human)


class zombified_human(monster, undead):
	r_idx = "229"
	name = "Zombified human"
	ascii = "z,s"
	level = 12
	rarity = 1
	hp = "9d8"
	speed = 110
	aaf = 20
	ac = 12
	sleep = 20
	exp = 34
	blows = [('HIT', 'HURT', '1d4'), ('HIT', 'HURT', '1d4')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a shambling human corpse dropping chunks of flesh behind it.
"""

monsters.register_race(zombified_human)


class tiger(monster):
	r_idx = "230"
	name = "Tiger"
	ascii = "f,o"
	level = 12
	rarity = 2
	hp = "9d10"
	speed = 120
	aaf = 40
	ac = 10
	sleep = 2
	exp = 40
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '1d6')]
	flags = ['BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST2', 'WILD_MOUNT2', 'ANIMAL', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """One of the largest of its species, a sleek orange and black shape creeps 
towards you, ready to pounce.
"""

monsters.register_race(tiger)


class moaning_spirit(monster, undead, spellcaster):
	r_idx = "231"
	name = "Moaning spirit"
	ascii = "G,U"
	level = 12
	rarity = 2
	hp = "3d9"
	speed = 120
	aaf = 14
	ac = 10
	sleep = 10
	exp = 44
	blows = [('WAIL', 'TERRIFY', ''), ('TOUCH', 'LOSE_DEX', '1d8')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'CAN_FLY', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['TPORT', 'SCARE']
	description = """A ghostly apparition that shrieks horribly.
"""

monsters.register_race(moaning_spirit)


class frumious_bandersnatch(monster):
	r_idx = "232"
	name = "Frumious bandersnatch"
	ascii = "c,b"
	level = 12
	rarity = 2
	hp = "10d8"
	speed = 120
	aaf = 12
	ac = 15
	sleep = 30
	exp = 40
	blows = [('BITE', 'HURT', '2d4'), ('BITE', 'HURT', '2d4'), ('STING', 'HURT', '2d4')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'ANIMAL', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """It is a vast armoured centipede with massive mandibles and a spiked tail.
"""

monsters.register_race(frumious_bandersnatch)


class spotted_jelly(monster):
	r_idx = "233"
	name = "Spotted jelly"
	ascii = "j,s"
	level = 12
	rarity = 3
	hp = "10d8"
	speed = 120
	aaf = 12
	ac = 5
	sleep = 1
	exp = 33
	blows = [('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '2d6'), ('TOUCH', 'ACID', '2d6')]
	flags = ['NEVER_MOVE', 'STUPID', 'EMPTY_MIND', 'COLD_BLOOD', 'WILD_DUNGEON_01', 'IM_ACID', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A jelly thing.
"""

monsters.register_race(spotted_jelly)


class drider(monster, spellcaster):
	r_idx = "234"
	name = "Drider"
	ascii = "S,b"
	level = 13
	rarity = 2
	hp = "7d13"
	speed = 110
	aaf = 8
	ac = 15
	sleep = 80
	exp = 55
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12'), ('BITE', 'POISON', '1d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'BASH_DOOR', 'DROP_SKELETON', 'EVIL', 'IM_POIS']
	sfreq = 8
	sflags = ['CONF', 'CAUSE_1', 'DARKNESS', 'MISSILE', 'ARROW_2']
	description = """A dark elven torso merged with the bloated form of a giant spider.
"""

monsters.register_race(drider)


class mongbat(monster):
	r_idx = "235"
	name = "Mongbat"
	ascii = "b,U"
	level = 13
	rarity = 3
	hp = "8d11"
	speed = 113
	aaf = 20
	ac = 40
	sleep = 8
	exp = 65
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d4'), ('BITE', 'POISON', '1d8')]
	flags = ['WILD_DUNGEON_01', 'WILD_WASTE2', 'ANIMAL', 'EVIL', 'FRIEND', 'FRIENDS', 'CAN_FLY', 'FORCE_MAXHP', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'WEIRD_MIND', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """Devil-bats, notoriously difficult to kill.
"""

monsters.register_race(mongbat)


class killer_brown_beetle(monster):
	r_idx = "236"
	name = "Killer brown beetle"
	ascii = "K,u"
	level = 13
	rarity = 2
	hp = "9d8"
	speed = 110
	aaf = 10
	ac = 20
	sleep = 30
	exp = 38
	blows = [('BITE', 'HURT', '3d4')]
	flags = ['WILD_DUNGEON_01', 'WEIRD_MIND', 'BASH_DOOR', 'CAN_FLY', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is a vicious insect with a tough carapace.
"""

monsters.register_race(killer_brown_beetle)


class boldor_king_of_the_yeeks(monster, spellcaster):
	r_idx = "237"
	name = "Boldor, King of the Yeeks"
	ascii = "y,v"
	level = 13
	rarity = 3
	hp = "13d10"
	speed = 120
	aaf = 18
	ac = 20
	sleep = 10
	exp = 200
	blows = [('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d8')]
	flags = ['UNIQUE', 'MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ESCORTS', 'ONLY_ITEM', 'DROP_90', 'DROP_1D2', 'DROP_GOOD', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SPEAK', 'DROP_CORPSE', 'ANIMAL', 'EVIL', 'IM_ACID']
	sfreq = 3
	sflags = ['HEAL', 'BLINK', 'TPORT', 'BLIND', 'SLOW', 'S_KIN']
	description = """A great yeek, powerful in magic and sorcery, but a yeek all the same.
"""

monsters.register_race(boldor_king_of_the_yeeks)


class ogre(giant):
	r_idx = "238"
	name = "Ogre"
	ascii = "O,U"
	level = 13
	rarity = 2
	hp = "9d9"
	speed = 110
	aaf = 20
	ac = 16
	sleep = 30
	exp = 50
	blows = [('HIT', 'HURT', '2d8')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'DROP_60', 'WILD_WASTE1', 'WILD_FOREST2', 'WILD_MOUNT2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT']
	sfreq = 0
	sflags = []
	description = """A hideous, smallish giant that is often found near or with orcs.
"""

monsters.register_race(ogre)


class creeping_mithril_coins(monster):
	r_idx = "239"
	name = "Creeping mithril coins"
	ascii = "$,B"
	level = 13
	rarity = 4
	hp = "15d8"
	speed = 110
	aaf = 5
	ac = 25
	sleep = 10
	exp = 45
	blows = [('HIT', 'HURT', '2d5'), ('TOUCH', 'POISON', '3d5')]
	flags = ['WILD_DUNGEON_01', 'ONLY_GOLD', 'DROP_90', 'DROP_2D2', 'COLD_BLOOD', 'BASH_DOOR', 'ANIMAL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """It is a pile of coins, shambling forward on thousands of tiny legs.
"""

monsters.register_race(creeping_mithril_coins)


class illusionist(monster, spellcaster):
	r_idx = "240"
	name = "Illusionist"
	ascii = "p,v"
	level = 13
	rarity = 2
	hp = "8d8"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 10
	exp = 50
	blows = [('HIT', 'HURT', '2d2')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'DROP_1D2', 'DROP_SKELETON', 'DROP_CORPSE', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 3
	sflags = ['HASTE', 'BLINK', 'TPORT', 'BLIND', 'HOLD', 'SLOW', 'CONF', 'DARKNESS']
	description = """A deceptive spell caster.
"""

monsters.register_race(illusionist)


class druid(monster, spellcaster):
	r_idx = "241"
	name = "Druid"
	ascii = "p,g"
	level = 13
	rarity = 2
	hp = "8d12"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 10
	exp = 50
	blows = [('HIT', 'HURT', '2d4'), ('HIT', 'HURT', '2d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'FORCE_SLEEP', 'DROP_1D2', 'DROP_SKELETON', 'DROP_CORPSE', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 3
	sflags = ['HASTE', 'BLINK', 'BLIND', 'HOLD', 'SLOW', 'BO_FIRE', 'BO_ELEC']
	description = """A mystic at one with nature.  Om.
"""

monsters.register_race(druid)


class pink_horror(demon, spellcaster):
	r_idx = "242"
	name = "Pink horror"
	ascii = "u,R"
	level = 13
	rarity = 3
	hp = "10d9"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 20
	exp = 64
	blows = [('CLAW', 'TERRIFY', '1d8'), ('CLAW', 'TERRIFY', '1d8'), ('BITE', 'CONFUSE', '1d6')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_FEAR', 'NO_CONF']
	sfreq = 8
	sflags = ['CONF', 'SCARE']
	description = """An ugly screaming little demon servant of Tzeentch.
"""

monsters.register_race(pink_horror)


class cloaker(monster):
	r_idx = "243"
	name = "Cloaker"
	ascii = "(,g"
	level = 13
	rarity = 5
	hp = "6d6"
	speed = 130
	aaf = 20
	ac = 20
	sleep = 0
	exp = 30
	blows = [('HIT', 'PARALYZE', '5d5'), ('HIT', 'TERRIFY', '5d5')]
	flags = ['WILD_DUNGEON_01', 'NEVER_MOVE', 'NONLIVING', 'NO_FEAR', 'STUPID', 'EMPTY_MIND', 'COLD_BLOOD', 'CHAR_MULTI', 'NO_CONF', 'NO_SLEEP', 'DROP_90', 'EVIL', 'IM_COLD', 'FORCE_MAXHP', 'IM_ELEC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It resembles a normal cloak until some poor fool ventures too close! 
"""

monsters.register_race(cloaker)


class black_orc(orc, spellcaster):
	r_idx = "244"
	name = "Black orc"
	ascii = "o,D"
	level = 13
	rarity = 2
	hp = "8d10"
	speed = 110
	aaf = 20
	ac = 36
	sleep = 20
	exp = 45
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_SKELETON', 'DROP_CORPSE', 'FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'HURT_LITE']
	sfreq = 15
	sflags = ['ARROW_1']
	description = """He is a large orc with powerful arms and deep black skin.
"""

monsters.register_race(black_orc)


class ochre_jelly(monster):
	r_idx = "245"
	name = "Ochre jelly"
	ascii = "j,y"
	level = 13
	rarity = 3
	hp = "9d8"
	speed = 120
	aaf = 12
	ac = 18
	sleep = 1
	exp = 40
	blows = [('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '2d6'), ('TOUCH', 'ACID', '2d6')]
	flags = ['WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'COLD_BLOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'IM_ACID', 'IM_POIS', 'CAN_SWIM', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A fast moving highly acidic jelly thing, that is eating away the floor it 
rests on.
"""

monsters.register_race(ochre_jelly)


class software_bug(monster):
	r_idx = "246"
	name = "Software bug"
	ascii = "I,r"
	level = 14
	rarity = 1
	hp = "1d4"
	speed = 120
	aaf = 8
	ac = 6
	sleep = 10
	exp = 4
	blows = [('BITE', 'HURT', '1d2')]
	flags = ['RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'MULTIPLY', 'WEIRD_MIND', 'BASH_DOOR', 'CAN_FLY', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """Oh no! They are everywhere!
"""

monsters.register_race(software_bug)


class lurker(monster):
	r_idx = "247"
	name = "Lurker"
	ascii = ".,w"
	level = 14
	rarity = 3
	hp = "15d10"
	speed = 110
	aaf = 30
	ac = 20
	sleep = 10
	exp = 80
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8')]
	flags = ['WILD_DUNGEON_01', 'CHAR_CLEAR', 'ATTR_CLEAR', 'NEVER_MOVE', 'FORCE_MAXHP', 'EMPTY_MIND', 'INVISIBLE', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A strange creature that merges with the dungeon floor, trapping its 
victims by enveloping them within its perfectly disguised form.
"""

monsters.register_race(lurker)


class nixie(monster):
	r_idx = "248"
	name = "Nixie"
	ascii = "h,B"
	level = 14
	rarity = 1
	hp = "13d10"
	speed = 110
	aaf = 20
	ac = 22
	sleep = 50
	exp = 60
	blows = [('HIT', 'HURT', '6d3'), ('HIT', 'HURT', '6d3')]
	flags = ['AQUATIC', 'WILD_DUNGEON_01', 'WILD_SHORE', 'MALE']
	sfreq = 0
	sflags = []
	description = """A race of fair yet belligrent sea elves.
"""

monsters.register_race(nixie)


class vlasta(monster):
	r_idx = "249"
	name = "Vlasta"
	ascii = "R,b"
	level = 14
	rarity = 3
	hp = "9d6"
	speed = 120
	aaf = 12
	ac = 10
	sleep = 12
	exp = 40
	blows = [('BITE', 'BLIND', '1d10'), ('BITE', 'BLIND', '1d10')]
	flags = ['WILD_DUNGEON_01', 'OPEN_DOOR', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """This strange creature looks like a miniature tyrannosaurus. It has 
empty, pale eyes and a sharp beak, which it aims at your eyes 
as it jumps at you! 
"""

monsters.register_race(vlasta)


class giant_white_dragon_fly(monster, spellcaster):
	r_idx = "250"
	name = "Giant white dragon fly"
	ascii = "F,w"
	level = 14
	rarity = 3
	hp = "3d9"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 50
	exp = 60
	blows = [('BITE', 'COLD', '1d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'RAND_50', 'CAN_FLY', 'WEIRD_MIND', 'BASH_DOOR', 'ANIMAL', 'IM_COLD']
	sfreq = 10
	sflags = ['BR_COLD']
	description = """It is a large fly that drips frost.
"""

monsters.register_race(giant_white_dragon_fly)


class snaga_sapper(orc):
	r_idx = "251"
	name = "Snaga sapper"
	ascii = "o,G"
	level = 14
	rarity = 1
	hp = "6d8"
	speed = 111
	aaf = 20
	ac = 16
	sleep = 30
	exp = 15
	blows = [('HIT', 'HURT', '1d8'), ('EXPLODE', 'HURT', '20d2')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_MOUNT1', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is one of the many weaker 'slave' orcs, often mistakenly known as a 
goblin. He is equipped with an explosive charge.
"""

monsters.register_race(snaga_sapper)


class blue_icky_thing(monster, spellcaster):
	r_idx = "252"
	name = "Blue icky thing"
	ascii = "i,b"
	level = 14
	rarity = 4
	hp = "7d6"
	speed = 100
	aaf = 15
	ac = 14
	sleep = 20
	exp = 20
	blows = [('CRAWL', 'POISON', '1d4'), ('CRAWL', 'EAT_FOOD', ''), ('HIT', 'HURT', '1d4'), ('HIT', 'HURT', '1d4')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'RAND_50', 'MULTIPLY', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'DROP_CORPSE', 'EVIL', 'IM_POIS']
	sfreq = 8
	sflags = ['BLIND', 'CONF', 'SCARE']
	description = """It is a strange, slimy, icky creature, with rudimentary intelligence, 
but evil cunning.  It hungers for food, and you look tasty.
"""

monsters.register_race(blue_icky_thing)


class gibbering_mouther(monster, spellcaster):
	r_idx = "253"
	name = "Gibbering mouther"
	ascii = "j,o"
	level = 14
	rarity = 4
	hp = "6d6"
	speed = 110
	aaf = 15
	ac = 14
	sleep = 20
	exp = 20
	blows = [('CRAWL', 'POISON', '1d4')]
	flags = ['WILD_DUNGEON_01', 'WILD_WASTE2', 'NEVER_MOVE', 'MULTIPLY', 'EVIL', 'CAN_SWIM', 'IM_POIS', 'EMPTY_MIND', 'NO_FEAR']
	sfreq = 7
	sflags = ['SCARE', 'CONF', 'BR_LITE']
	description = """A chaotic mass of pulsating flesh, mouths and eyes.
"""

monsters.register_race(gibbering_mouther)


class irish_wolfhound_of_flora(monster):
	r_idx = "254"
	name = "Irish wolfhound of Flora"
	ascii = "C,s"
	level = 14
	rarity = 2
	hp = "7d9"
	speed = 120
	aaf = 20
	ac = 14
	sleep = 0
	exp = 40
	blows = [('BITE', 'HURT', '1d5'), ('BITE', 'HURT', '1d5')]
	flags = ['WILD_DUNGEON_01', 'ANIMAL', 'NO_FEAR', 'FRIENDS', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """Well-trained watchdogs, obedient to death.
"""

monsters.register_race(irish_wolfhound_of_flora)


class hill_giant(giant):
	r_idx = "255"
	name = "Hill giant"
	ascii = "P,U"
	level = 14
	rarity = 1
	hp = "12d10"
	speed = 110
	aaf = 20
	ac = 22
	sleep = 50
	exp = 60
	blows = [('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6')]
	flags = ['DROP_60', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT', 'MALE']
	sfreq = 0
	sflags = []
	description = """A ten foot tall humanoid with powerful muscles.
"""

monsters.register_race(hill_giant)


class flesh_golem(monster):
	r_idx = "256"
	name = "Flesh golem"
	ascii = "g,o"
	level = 14
	rarity = 1
	hp = "9d8"
	speed = 110
	aaf = 12
	ac = 15
	sleep = 10
	exp = 50
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'BASH_DOOR', 'CAN_SWIM', 'IM_ELEC', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """A shambling humanoid monster with long scars.
"""

monsters.register_race(flesh_golem)


class warg(monster):
	r_idx = "257"
	name = "Warg"
	ascii = "C,D"
	level = 14
	rarity = 2
	hp = "6d8"
	speed = 120
	aaf = 20
	ac = 12
	sleep = 40
	exp = 40
	blows = [('BITE', 'HURT', '1d8')]
	flags = ['RAND_25', 'FRIENDS', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'WILD_MOUNT1', 'WILD_MOUNT2', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'ANIMAL', 'EVIL']
	sfreq = 0
	sflags = []
	description = """It is a large wolf with eyes full of cunning.
"""

monsters.register_race(warg)


class cheerful_leprechaun(monster, spellcaster):
	r_idx = "258"
	name = "Cheerful leprechaun"
	ascii = "h,G"
	level = 14
	rarity = 2
	hp = "2d4"
	speed = 120
	aaf = 8
	ac = 6
	sleep = 6
	exp = 23
	blows = [('TOUCH', 'EAT_GOLD', ''), ('TOUCH', 'EAT_FOOD', '')]
	flags = ['WILD_DUNGEON_01', 'MULTIPLY', 'DROP_60', 'ONLY_GOLD', 'RAND_50', 'OPEN_DOOR', 'MALE', 'GOOD']
	sfreq = 6
	sflags = ['BLINK']
	description = """A merry little gnome.
"""

monsters.register_race(cheerful_leprechaun)


class giant_flea(monster):
	r_idx = "259"
	name = "Giant flea"
	ascii = "I,s"
	level = 14
	rarity = 1
	hp = "1d2"
	speed = 120
	aaf = 6
	ac = 3
	sleep = 10
	exp = 3
	blows = [('BITE', 'HURT', '1d2')]
	flags = ['RAND_50', 'CAN_FLY', 'WILD_DUNGEON_01', 'WEIRD_MIND', 'MULTIPLY', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It makes you itch just to look at it. 
"""

monsters.register_race(giant_flea)


class ufthak_of_cirith_ungol(orc):
	r_idx = "260"
	name = "Ufthak of Cirith Ungol"
	ascii = "o,g"
	level = 14
	rarity = 3
	hp = "22d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 20
	exp = 200
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['UNIQUE', 'MALE', 'FORCE_MAXHP', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'ESCORT', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'IM_COLD', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A strong orc guarding the pass of Cirith Ungol.  He is mortally afraid of 
spiders.
"""

monsters.register_race(ufthak_of_cirith_ungol)


class clay_golem(monster):
	r_idx = "261"
	name = "Clay golem"
	ascii = "g,U"
	level = 15
	rarity = 2
	hp = "10d8"
	speed = 110
	aaf = 12
	ac = 15
	sleep = 10
	exp = 50
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a massive animated statue made out of hardened clay.
"""

monsters.register_race(clay_golem)


class black_ogre(giant):
	r_idx = "262"
	name = "Black ogre"
	ascii = "O,D"
	level = 15
	rarity = 2
	hp = "14d9"
	speed = 110
	aaf = 20
	ac = 17
	sleep = 30
	exp = 75
	blows = [('HIT', 'HURT', '2d8'), ('HIT', 'HURT', '2d8')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'DROP_CORPSE', 'FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT']
	sfreq = 0
	sflags = []
	description = """A massive orc-like figure with black skin and powerful arms.
"""

monsters.register_race(black_ogre)


class dweller_on_the_threshold(monster, spellcaster):
	r_idx = "263"
	name = "Dweller on the threshold"
	ascii = "Y,v"
	level = 15
	rarity = 5
	hp = "22d8"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 100
	blows = [('GAZE', 'PARALYZE', ''), ('CLAW', 'UN_POWER', '3d5'), ('CLAW', 'FIRE', '3d5'), ('BITE', 'UN_BONUS', '2d5')]
	flags = ['WILD_DUNGEON_01', 'NEVER_MOVE', 'DROP_60', 'EVIL', 'DROP_CORPSE', 'IM_POIS', 'IM_COLD', 'IM_FIRE', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'RES_DISE']
	sfreq = 6
	sflags = ['BO_ACID', 'CONF', 'DRAIN_MANA']
	description = """A creature like a giant purple Buddha with lots of teeth.
It cleans them with the bones of sorcerors.
"""

monsters.register_race(dweller_on_the_threshold)


class half_orc(orc):
	r_idx = "264"
	name = "Half-orc"
	ascii = "o,s"
	level = 15
	rarity = 3
	hp = "12d10"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 20
	exp = 50
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST1', 'FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'ORC']
	sfreq = 0
	sflags = []
	description = """He is a hideous deformed cross-breed with man and orc, combining man's 
strength and cunning with orcish evil.
"""

monsters.register_race(half_orc)


class dark_naga(monster, spellcaster):
	r_idx = "265"
	name = "Dark naga"
	ascii = "n,D"
	level = 15
	rarity = 2
	hp = "17d11"
	speed = 110
	aaf = 60
	ac = 32
	sleep = 60
	exp = 90
	blows = [('STING', 'HURT', '1d10'), ('BITE', 'HURT', '1d10')]
	flags = ['FEMALE', 'WILD_DUNGEON_01', 'WILD_SHORE', 'RAND_25', 'DROP_60', 'DROP_1D2', 'IM_POIS', 'IM_COLD', 'RES_WATE', 'OPEN_DOOR', 'BASH_DOOR', 'EMPTY_MIND', 'CAN_SWIM', 'DROP_CORPSE', 'EVIL']
	sfreq = 4
	sflags = ['HOLD', 'CONF', 'BO_COLD', 'HEAL', 'DARKNESS']
	description = """A giant snake-like figure with a woman's torso. Not as strong 
as other nagas, but more talented in magic.
"""

monsters.register_race(dark_naga)


class giant_octopus(monster):
	r_idx = "266"
	name = "Giant octopus"
	ascii = "l,v"
	level = 15
	rarity = 2
	hp = "22d11"
	speed = 105
	aaf = 60
	ac = 30
	sleep = 60
	exp = 60
	blows = [('SPIT', 'BLIND', '1d3'), ('CRUSH', 'HURT', '8d3'), ('CRUSH', 'HURT', '8d3'), ('CRUSH', 'HURT', '8d3')]
	flags = ['RAND_25', 'IM_COLD', 'RES_WATE', 'AQUATIC', 'ANIMAL', 'WILD_DUNGEON_01', 'WILD_SHORE']
	sfreq = 0
	sflags = []
	description = """It doesn't move very fast, but when it does - watch out!
"""

monsters.register_race(giant_octopus)


class magic_mushroom_patch(monster, spellcaster):
	r_idx = "267"
	name = "Magic mushroom patch"
	ascii = ",,B"
	level = 15
	rarity = 2
	hp = "1d1"
	speed = 130
	aaf = 40
	ac = 10
	sleep = 0
	exp = 10
	blows = []
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST2', 'FORCE_SLEEP', 'NEVER_MOVE', 'NEVER_BLOW', 'FRIENDS', 'STUPID', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 1
	sflags = ['BLINK', 'SLOW', 'SCARE', 'DARKNESS']
	description = """Yum!  It looks quite tasty.  It seems to glow with an unusual light.
"""

monsters.register_race(magic_mushroom_patch)


class plaguebearer_of_nurgle(demon, undead, spellcaster):
	r_idx = "268"
	name = "Plaguebearer of Nurgle"
	ascii = "z,o"
	level = 15
	rarity = 2
	hp = "8d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 20
	exp = 75
	blows = [('CLAW', 'DISEASE', '2d5'), ('CLAW', 'DISEASE', '2d5'), ('BUTT', 'HURT', '3d5')]
	flags = ['FORCE_MAXHP', 'DROP_60', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'IM_COLD', 'EVIL', 'DEMON', 'UNDEAD', 'IM_POIS', 'NONLIVING']
	sfreq = 8
	sflags = ['SCARE', 'S_ANT', 'CAUSE_2', 'SLOW']
	description = """An unfortunate individual, who was killed by the incurable 
disease known as Nurgle's Rot, and was transformed into a 
rotting demon zombie. It has but one eye, and a single 
pale horn in its forehead.
"""

monsters.register_race(plaguebearer_of_nurgle)


class guardian_naga(monster):
	r_idx = "269"
	name = "Guardian naga"
	ascii = "n,y"
	level = 15
	rarity = 2
	hp = "17d11"
	speed = 110
	aaf = 20
	ac = 32
	sleep = 120
	exp = 80
	blows = [('CRUSH', 'HURT', '2d8'), ('BITE', 'HURT', '1d8'), ('BITE', 'HURT', '1d8')]
	flags = ['FEMALE', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_SHORE', 'RAND_25', 'DROP_60', 'DROP_1D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A giant snake-like figure with a woman's torso.
"""

monsters.register_race(guardian_naga)


class wererat(monster, spellcaster):
	r_idx = "270"
	name = "Wererat"
	ascii = "r,D"
	level = 15
	rarity = 2
	hp = "14d8"
	speed = 110
	aaf = 10
	ac = 6
	sleep = 10
	exp = 55
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d6')]
	flags = ['FORCE_SLEEP', 'ONLY_GOLD', 'DROP_60', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'ANIMAL', 'EVIL', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2']
	sfreq = 9
	sflags = ['BLINK', 'CAUSE_2', 'BO_COLD', 'BA_POIS', 'S_KIN']
	description = """A large rat with glowing red eyes.  The wererat is a disgusting creature, 
relishing in filth and disease.
"""

monsters.register_race(wererat)


class light_hound(monster, spellcaster):
	r_idx = "271"
	name = "Light hound"
	ascii = "Z,o"
	level = 15
	rarity = 1
	hp = "4d7"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 50
	blows = [('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'DROP_CORPSE', 'FRIENDS', 'WILD_DUNGEON_01', 'BASH_DOOR', 'ANIMAL']
	sfreq = 5
	sflags = ['BR_LITE']
	description = """A brilliant canine form whose light hurts your eyes, even at this distance.
"""

monsters.register_race(light_hound)


class shadow_hound(monster, spellcaster):
	r_idx = "272"
	name = "Shadow hound"
	ascii = "Z,D"
	level = 15
	rarity = 1
	hp = "4d7"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 50
	blows = [('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'DROP_CORPSE', 'FRIENDS', 'WILD_DUNGEON_01', 'BASH_DOOR', 'HURT_LITE', 'ANIMAL']
	sfreq = 5
	sflags = ['BR_DARK']
	description = """A hole in the air in the shape of a huge hound.  No light falls upon its 
form.
"""

monsters.register_race(shadow_hound)


class flying_skull(monster, undead):
	r_idx = "273"
	name = "Flying skull"
	ascii = "s,s"
	level = 15
	rarity = 3
	hp = "7d10"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 20
	exp = 50
	blows = [('BITE', 'POISON', '1d3'), ('BITE', 'LOSE_STR', '1d4')]
	flags = ['WILD_DUNGEON_01', 'UNDEAD', 'EVIL', 'IM_POIS', 'IM_COLD', 'WEIRD_MIND', 'NO_FEAR', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP', 'DROP_60', 'BASH_DOOR', 'FRIENDS', 'COLD_BLOOD']
	sfreq = 0
	sflags = []
	description = """A skullpack animated by necromantic spells.
"""

monsters.register_race(flying_skull)


class mi_go(monster, spellcaster):
	r_idx = "274"
	name = "Mi-Go"
	ascii = "I,R"
	level = 15
	rarity = 2
	hp = "10d8"
	speed = 120
	aaf = 20
	ac = 15
	sleep = 20
	exp = 80
	blows = [('STING', 'POISON', '1d4'), ('BITE', 'LOSE_STR', '1d2')]
	flags = ['WILD_DUNGEON_01', 'IM_POIS', 'IM_COLD', 'COLD_BLOOD', 'ANIMAL', 'EVIL', 'NO_SLEEP', 'NO_CONF', 'CAN_FLY', 'DROP_SKELETON']
	sfreq = 6
	sflags = ['CONF', 'S_MONSTER', 'S_DEMON']
	description = """"They were pinkish things about five feet long; with crustaceous 
bodies bearing vast pairs of dorsal fins or membranous wings and 
several sets of articulate limbs, and with a sort of convoluted 
ellipsoid, covered with multitudes of very short antenna, where 
a head would ordinarily be..."
"""

monsters.register_race(mi_go)


class giant_tarantula(monster):
	r_idx = "275"
	name = "Giant tarantula"
	ascii = "S,G"
	level = 15
	rarity = 3
	hp = "7d15"
	speed = 120
	aaf = 8
	ac = 16
	sleep = 80
	exp = 70
	blows = [('BITE', 'POISON', '1d6'), ('BITE', 'POISON', '1d6'), ('BITE', 'POISON', '1d6')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'ANIMAL', 'IM_POIS', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """A giant spider with hairy black and red legs.
"""

monsters.register_race(giant_tarantula)


class giant_clear_centipede(monster):
	r_idx = "276"
	name = "Giant clear centipede"
	ascii = "c,w"
	level = 15
	rarity = 2
	hp = "4d7"
	speed = 110
	aaf = 12
	ac = 15
	sleep = 30
	exp = 30
	blows = [('BITE', 'HURT', '2d4'), ('STING', 'HURT', '2d4')]
	flags = ['ATTR_CLEAR', 'INVISIBLE', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'ANIMAL', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """It is about four feet long and carnivorous.
"""

monsters.register_race(giant_clear_centipede)


class mirkwood_spider(monster):
	r_idx = "277"
	name = "Mirkwood spider"
	ascii = "S,u"
	level = 15
	rarity = 2
	hp = "6d8"
	speed = 120
	aaf = 15
	ac = 13
	sleep = 80
	exp = 25
	blows = [('BITE', 'HURT', '1d8'), ('BITE', 'POISON', '1d6'), ('BITE', 'POISON', '1d6')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'WEIRD_MIND', 'BASH_DOOR', 'HURT_LITE', 'ANIMAL', 'EVIL', 'IM_POIS', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """A strong and powerful spider from Mirkwood forest.  Cunning and evil, it 
seeks to taste your juicy insides.
"""

monsters.register_race(mirkwood_spider)


class frost_giant(giant):
	r_idx = "278"
	name = "Frost giant"
	ascii = "P,w"
	level = 15
	rarity = 1
	hp = "12d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 50
	exp = 75
	blows = [('HIT', 'COLD', '3d6'), ('HIT', 'HURT', '2d8')]
	flags = ['DROP_60', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'WILD_MOUNT2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'GIANT', 'MALE', 'AURA_COLD', 'IM_COLD']
	sfreq = 0
	sflags = []
	description = """A twelve foot tall giant covered in furs.
"""

monsters.register_race(frost_giant)


class griffon(monster):
	r_idx = "279"
	name = "Griffon"
	ascii = "H,u"
	level = 15
	rarity = 1
	hp = "21d8"
	speed = 110
	aaf = 12
	ac = 8
	sleep = 10
	exp = 70
	blows = [('HIT', 'HURT', '3d4'), ('BITE', 'HURT', '2d6')]
	flags = ['BASH_DOOR', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'WILD_MOUNT1', 'WILD_MOUNT2', 'WILD_GRASS', 'ANIMAL', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """It is half lion, half eagle.  It flies menacingly towards you.
"""

monsters.register_race(griffon)


class homonculous(demon):
	r_idx = "280"
	name = "Homonculous"
	ascii = "u,U"
	level = 15
	rarity = 3
	hp = "6d8"
	speed = 110
	aaf = 20
	ac = 16
	sleep = 30
	exp = 40
	blows = [('HIT', 'PARALYZE', '1d2'), ('HIT', 'HURT', '1d10')]
	flags = ['WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'NONLIVING', 'CAN_FLY', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a small demonic spirit full of malevolence.
"""

monsters.register_race(homonculous)


class gnome_mage(monster, spellcaster):
	r_idx = "281"
	name = "Gnome mage"
	ascii = "h,R"
	level = 15
	rarity = 2
	hp = "5d8"
	speed = 110
	aaf = 20
	ac = 10
	sleep = 20
	exp = 40
	blows = [('HIT', 'HURT', '1d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'FORCE_SLEEP', 'FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'EVIL']
	sfreq = 4
	sflags = ['BLINK', 'DARKNESS', 'BO_COLD', 'S_MONSTER']
	description = """A mage of short stature.
"""

monsters.register_race(gnome_mage)


class clear_hound(monster):
	r_idx = "282"
	name = "Clear hound"
	ascii = "Z,B"
	level = 15
	rarity = 2
	hp = "7d6"
	speed = 110
	aaf = 30
	ac = 12
	sleep = 0
	exp = 50
	blows = [('BITE', 'HURT', '1d6'), ('BITE', 'HURT', '1d6'), ('BITE', 'HURT', '1d6')]
	flags = ['ATTR_CLEAR', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_SKELETON', 'DROP_CORPSE', 'INVISIBLE', 'BASH_DOOR', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A completely translucent hound.
"""

monsters.register_race(clear_hound)


class umber_hulk(monster):
	r_idx = "283"
	name = "Umber hulk"
	ascii = "X,u"
	level = 16
	rarity = 1
	hp = "14d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 10
	exp = 75
	blows = [('GAZE', 'CONFUSE', ''), ('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('BITE', 'HURT', '2d6')]
	flags = ['EMPTY_MIND', 'COLD_BLOOD', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'BASH_DOOR', 'KILL_WALL', 'DROP_SKELETON', 'ANIMAL', 'EVIL', 'IM_POIS', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """This bizarre creature has glaring eyes and large mandibles capable of 
slicing through rock.
"""

monsters.register_race(umber_hulk)


class rust_monster(monster):
	r_idx = "284"
	name = "Rust monster"
	ascii = "q,o"
	level = 16
	rarity = 2
	hp = "15d12"
	speed = 110
	aaf = 12
	ac = 28
	sleep = 10
	exp = 50
	blows = [('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10')]
	flags = ['STUPID', 'WEIRD_MIND', 'KILL_ITEM', 'WILD_DUNGEON_01', 'IM_ACID', 'IM_POIS', 'DROP_CORPSE', 'FORCE_MAXHP', 'NO_CONF']
	sfreq = 0
	sflags = []
	description = """It is a weird, small animal with two antennae popping forth from 
its forehead. It looks hungry.
"""

monsters.register_race(rust_monster)


class orc_captain(orc, spellcaster):
	r_idx = "285"
	name = "Orc captain"
	ascii = "o,g"
	level = 16
	rarity = 3
	hp = "14d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 20
	exp = 40
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_GRASS', 'WILD_MOUNT1', 'DROP_90', 'OPEN_DOOR', 'BASH_DOOR', 'ESCORT', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'ORC', 'CAN_SPEAK']
	sfreq = 15
	sflags = ['ARROW_1']
	description = """An armoured orc with an air of authority.
"""

monsters.register_race(orc_captain)


class gelatinous_cube(monster):
	r_idx = "286"
	name = "Gelatinous cube"
	ascii = "j,B"
	level = 16
	rarity = 4
	hp = "27d10"
	speed = 110
	aaf = 12
	ac = 6
	sleep = 1
	exp = 80
	blows = [('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10')]
	flags = ['FORCE_MAXHP', 'DROP_1D2', 'DROP_4D2', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'COLD_BLOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a strange, vast gelatinous structure that assumes cubic proportions 
as it lines all four walls of the corridors it patrols.  Through its 
transparent jelly structure you can see treasures it has engulfed, and a 
few corpses as well.
"""

monsters.register_race(gelatinous_cube)


class giant_green_dragon_fly(monster, spellcaster):
	r_idx = "287"
	name = "Giant green dragon fly"
	ascii = "F,G"
	level = 16
	rarity = 2
	hp = "2d8"
	speed = 110
	aaf = 12
	ac = 16
	sleep = 50
	exp = 70
	blows = [('BITE', 'POISON', '1d6')]
	flags = ['FORCE_SLEEP', 'RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'WILD_SWAMP2', 'WEIRD_MIND', 'BASH_DOOR', 'CAN_FLY', 'ANIMAL', 'IM_POIS']
	sfreq = 10
	sflags = ['BR_POIS']
	description = """A vast, foul-smelling dragonfly.
"""

monsters.register_race(giant_green_dragon_fly)


class fire_giant(giant):
	r_idx = "288"
	name = "Fire giant"
	ascii = "P,r"
	level = 16
	rarity = 2
	hp = "14d8"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 50
	exp = 54
	blows = [('HIT', 'FIRE', '3d7'), ('HIT', 'FIRE', '3d7')]
	flags = ['DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'EVIL', 'GIANT', 'MALE', 'AURA_FIRE', 'DROP_SKELETON', 'DROP_CORPSE', 'IM_FIRE']
	sfreq = 0
	sflags = []
	description = """A glowing fourteen foot tall giant.  Flames drip from its red skin.
"""

monsters.register_race(fire_giant)


class hummerhorn(monster):
	r_idx = "289"
	name = "Hummerhorn"
	ascii = "I,y"
	level = 16
	rarity = 5
	hp = "2d2"
	speed = 120
	aaf = 8
	ac = 7
	sleep = 10
	exp = 4
	blows = [('BITE', 'CONFUSE', '2d2')]
	flags = ['RAND_50', 'RAND_25', 'CAN_FLY', 'WILD_DUNGEON_01', 'MULTIPLY', 'WEIRD_MIND', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A giant buzzing wasp, its stinger drips venom.
"""

monsters.register_race(hummerhorn)


class lizardman(monster):
	r_idx = "290"
	name = "Lizardman"
	ascii = "h,R"
	level = 16
	rarity = 3
	hp = "11d10"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 20
	exp = 55
	blows = [('HIT', 'HURT', '4d4'), ('HIT', 'HURT', '4d4')]
	flags = ['MALE', 'CAN_SWIM', 'IM_ACID', 'FRIENDS', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_SHORE', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'EVIL']
	sfreq = 0
	sflags = []
	description = """Intelligent lizard beings from the depths.
"""

monsters.register_race(lizardman)


class ulfast_son_of_ulfang(monster):
	r_idx = "291"
	name = "Ulfast, Son of Ulfang"
	ascii = "p,o"
	level = 16
	rarity = 3
	hp = "25d10"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 40
	exp = 200
	blows = [('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'MALE', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_90', 'DROP_GOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A short and swarthy Easterling.
"""

monsters.register_race(ulfast_son_of_ulfang)


class hammerhead(monster):
	r_idx = "292"
	name = "Hammerhead"
	ascii = "l,b"
	level = 16
	rarity = 3
	hp = "11d10"
	speed = 115
	aaf = 20
	ac = 30
	sleep = 20
	exp = 40
	blows = [('BITE', 'HURT', '4d4'), ('BUTT', 'HURT', '4d4'), ('BITE', 'HURT', '4d4')]
	flags = ['ANIMAL', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN']
	sfreq = 0
	sflags = []
	description = """A hungry shark with a strange head.
"""

monsters.register_race(hammerhead)


class berserker(monster):
	r_idx = "293"
	name = "Berserker"
	ascii = "p,u"
	level = 16
	rarity = 3
	hp = "16d10"
	speed = 113
	aaf = 20
	ac = 32
	sleep = 20
	exp = 50
	blows = [('HIT', 'HURT', '4d4'), ('HIT', 'HURT', '4d4'), ('HIT', 'HURT', '4d4')]
	flags = ['MALE', 'NO_FEAR', 'NO_STUN', 'BASH_DOOR', 'OPEN_DOOR', 'DROP_1D2', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A warrior in a battle-frenzy; he'll stop only when he drops.
"""

monsters.register_race(berserker)


class quasit(demon, spellcaster):
	r_idx = "294"
	name = "Quasit"
	ascii = "u,W"
	level = 16
	rarity = 2
	hp = "4d8"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 20
	exp = 50
	blows = [('BITE', 'LOSE_DEX', '1d6'), ('CLAW', 'HURT', '1d3'), ('CLAW', 'HURT', '1d3')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'RAND_25', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'SMART', 'INVISIBLE', 'BASH_DOOR', 'EVIL', 'DEMON', 'IM_FIRE', 'NONLIVING']
	sfreq = 10
	sflags = ['BLINK', 'TPORT', 'TELE_TO', 'TELE_LEVEL', 'BLIND', 'CONF', 'SCARE']
	description = """The chaotic evil master's favourite pet.
"""

monsters.register_race(quasit)


class sphinx(monster, spellcaster):
	r_idx = "295"
	name = "Sphinx"
	ascii = "H,o"
	level = 17
	rarity = 2
	hp = "40d5"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 20
	exp = 80
	blows = [('CLAW', 'HURT', '2d6'), ('CLAW', 'HURT', '2d6')]
	flags = ['FORCE_SLEEP', 'ONLY_GOLD', 'DROP_1D2', 'CAN_FLY', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'ANIMAL']
	sfreq = 11
	sflags = ['SCARE', 'CONF']
	description = """It will eat you if you cannot answer its riddle.
"""

monsters.register_race(sphinx)


class imp(demon, spellcaster):
	r_idx = "296"
	name = "Imp"
	ascii = "u,r"
	level = 17
	rarity = 2
	hp = "4d8"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 20
	exp = 55
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'POISON', '3d4')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'WILD_DUNGEON_01', 'RAND_25', 'ONLY_ITEM', 'DROP_1D2', 'SMART', 'INVISIBLE', 'COLD_BLOOD', 'BASH_DOOR', 'EVIL', 'DEMON', 'IM_FIRE', 'RES_TELE']
	sfreq = 10
	sflags = ['BLINK', 'TPORT', 'TELE_TO', 'TELE_LEVEL', 'BLIND', 'CONF', 'SCARE', 'BO_FIRE']
	description = """The lawful evil master's favourite pet.
"""

monsters.register_race(imp)


class forest_troll(troll):
	r_idx = "297"
	name = "Forest troll"
	ascii = "T,G"
	level = 17
	rarity = 1
	hp = "14d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 40
	exp = 70
	blows = [('HIT', 'HURT', '1d4'), ('HIT', 'HURT', '1d4'), ('BITE', 'HURT', '1d6')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_60', 'WILD_FOREST2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is green skinned and ugly.
"""

monsters.register_race(forest_troll)


class freezing_sphere(monster):
	r_idx = "298"
	name = "Freezing sphere"
	ascii = "*,w"
	level = 17
	rarity = 1
	hp = "5d5"
	speed = 120
	aaf = 100
	ac = 15
	sleep = 0
	exp = 50
	blows = [('EXPLODE', 'COLD', '8d8')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'AURA_COLD', 'IM_COLD', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """The embodiment of cold.
"""

monsters.register_race(freezing_sphere)


class jumping_fireball(monster):
	r_idx = "299"
	name = "Jumping fireball"
	ascii = "*,r"
	level = 17
	rarity = 1
	hp = "5d5"
	speed = 120
	aaf = 100
	ac = 15
	sleep = 0
	exp = 50
	blows = [('EXPLODE', 'FIRE', '8d8')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'AURA_FIRE', 'IM_FIRE', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """The embodiment of heat.
"""

monsters.register_race(jumping_fireball)


class ball_lightning(monster):
	r_idx = "300"
	name = "Ball lightning"
	ascii = "*,B"
	level = 17
	rarity = 1
	hp = "5d5"
	speed = 120
	aaf = 100
	ac = 15
	sleep = 0
	exp = 50
	blows = [('EXPLODE', 'ELEC', '8d8')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'AURA_ELEC', 'IM_ELEC', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """A crackling ball of energy.
"""

monsters.register_race(ball_lightning)


class two_headed_hydra(monster, spellcaster):
	r_idx = "301"
	name = "2-headed hydra"
	ascii = "M,u"
	level = 17
	rarity = 2
	hp = "75d3"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 20
	exp = 80
	blows = [('BITE', 'HURT', '2d6'), ('BITE', 'HURT', '2d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_SWAMP1', 'ONLY_GOLD', 'DROP_1D2', 'CAN_SWIM', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'ANIMAL']
	sfreq = 11
	sflags = ['SCARE']
	description = """A strange reptilian hybrid with two heads, guarding its hoard.
"""

monsters.register_race(two_headed_hydra)


class swamp_thing(monster):
	r_idx = "302"
	name = "Swamp thing"
	ascii = "H,g"
	level = 17
	rarity = 2
	hp = "10d14"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 30
	exp = 80
	blows = [('CLAW', 'TERRIFY', '2d5'), ('CLAW', 'TERRIFY', '5d2')]
	flags = ['CAN_SWIM', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_SWAMP2']
	sfreq = 0
	sflags = []
	description = """A creature that was once human, but is now as green as moss.
"""

monsters.register_race(swamp_thing)


class water_spirit(monster):
	r_idx = "303"
	name = "Water spirit"
	ascii = "E,b"
	level = 17
	rarity = 1
	hp = "9d8"
	speed = 120
	aaf = 12
	ac = 15
	sleep = 40
	exp = 58
	blows = [('HIT', 'HURT', '2d4'), ('HIT', 'HURT', '2d4')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'EVIL', 'IM_POIS', 'CAN_FLY', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A whirlpool of sentient liquid.
"""

monsters.register_race(water_spirit)


class giant_red_scorpion(monster):
	r_idx = "304"
	name = "Giant red scorpion"
	ascii = "S,R"
	level = 17
	rarity = 1
	hp = "10d8"
	speed = 110
	aaf = 12
	ac = 44
	sleep = 20
	exp = 62
	blows = [('BITE', 'HURT', '2d4'), ('STING', 'LOSE_STR', '1d7')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_WASTE1', 'ANIMAL', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """It is fast and poisonous.
"""

monsters.register_race(giant_red_scorpion)


class earth_spirit(monster):
	r_idx = "305"
	name = "Earth spirit"
	ascii = "E,U"
	level = 17
	rarity = 2
	hp = "10d8"
	speed = 120
	aaf = 10
	ac = 25
	sleep = 50
	exp = 64
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'PASS_WALL', 'CAN_FLY', 'NONLIVING', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A whirling form of sentient rock.
"""

monsters.register_race(earth_spirit)


class fire_spirit(monster):
	r_idx = "306"
	name = "Fire spirit"
	ascii = "E,R"
	level = 18
	rarity = 2
	hp = "7d9"
	speed = 120
	aaf = 16
	ac = 15
	sleep = 20
	exp = 75
	blows = [('HIT', 'FIRE', '2d6'), ('HIT', 'FIRE', '2d6')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'BASH_DOOR', 'CAN_FLY', 'NONLIVING', 'EVIL', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'AURA_FIRE']
	sfreq = 0
	sflags = []
	description = """A whirlwind of sentient flame.
"""

monsters.register_race(fire_spirit)


class fire_hound(monster, spellcaster):
	r_idx = "307"
	name = "Fire hound"
	ascii = "Z,r"
	level = 18
	rarity = 1
	hp = "7d6"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 70
	blows = [('BITE', 'FIRE', '1d3'), ('BITE', 'FIRE', '1d3'), ('BITE', 'FIRE', '1d3')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'WILD_DUNGEON_01', 'BASH_DOOR', 'ANIMAL', 'IM_FIRE', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 10
	sflags = ['BR_FIRE']
	description = """Flames lick at its feet and its tongue is a blade of fire.  You can feel a 
furnace heat radiating from the creature.
"""

monsters.register_race(fire_hound)


class cold_hound(monster, spellcaster):
	r_idx = "308"
	name = "Cold hound"
	ascii = "Z,W"
	level = 18
	rarity = 1
	hp = "7d6"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 70
	blows = [('BITE', 'COLD', '1d6'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'WILD_DUNGEON_01', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'IM_COLD']
	sfreq = 10
	sflags = ['BR_COLD']
	description = """A hound as tall as a man, this creature appears to be composed of angular 
planes of ice.  Cold radiates from it and freezes your breath in the air.
"""

monsters.register_race(cold_hound)


class energy_hound(monster, spellcaster):
	r_idx = "309"
	name = "Energy hound"
	ascii = "Z,y"
	level = 18
	rarity = 1
	hp = "7d6"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 70
	blows = [('BITE', 'ELEC', '1d3'), ('BITE', 'ELEC', '1d3'), ('BITE', 'ELEC', '1d3')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'ANIMAL', 'IM_ELEC']
	sfreq = 10
	sflags = ['BR_ELEC']
	description = """Saint Elmo's Fire forms a ghostly halo around this hound, and sparks sting 
your fingers as energy builds up in the air around you.
"""

monsters.register_race(energy_hound)


class potion_mimic(monster, spellcaster):
	r_idx = "310"
	name = "Potion mimic"
	ascii = "!,w"
	level = 18
	rarity = 3
	hp = "7d10"
	speed = 110
	aaf = 25
	ac = 15
	sleep = 0
	exp = 60
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'HURT', '2d3'), ('HIT', 'HURT', '2d3')]
	flags = ['CHAR_MULTI', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'NEVER_MOVE', 'EMPTY_MIND', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_2', 'BO_COLD']
	description = """A strange creature that disguises itself as discarded objects to lure 
unsuspecting adventurers within reach of its venomous claws.
"""

monsters.register_race(potion_mimic)


class door_mimic(monster, spellcaster):
	r_idx = "311"
	name = "Door mimic"
	ascii = "+,U"
	level = 18
	rarity = 6
	hp = "7d10"
	speed = 110
	aaf = 25
	ac = 15
	sleep = 0
	exp = 70
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'CONFUSE', '2d3'), ('HIT', 'PARALYZE', '2d3')]
	flags = ['CHAR_MULTI', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'NEVER_MOVE', 'EMPTY_MIND', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_2', 'BO_COLD']
	description = """A strange creature that disguises itself as a door to lure 
unsuspecting adventurers within reach of its venomous claws.
"""

monsters.register_race(door_mimic)


class blink_dog(monster, spellcaster):
	r_idx = "312"
	name = "Blink dog"
	ascii = "C,B"
	level = 18
	rarity = 2
	hp = "6d6"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 10
	exp = 50
	blows = [('BITE', 'HURT', '1d8')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'FRIENDS', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'RES_TELE']
	sfreq = 4
	sflags = ['BLINK', 'TELE_TO']
	description = """A strange magical member of the canine race, its form seems to shimmer and 
fade in front of your very eyes.
"""

monsters.register_race(blink_dog)


class uruk(orc, spellcaster):
	r_idx = "313"
	name = "Uruk"
	ascii = "o,w"
	level = 18
	rarity = 1
	hp = "6d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 20
	exp = 68
	blows = [('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'FORCE_MAXHP', 'FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'ORC', 'IM_POIS']
	sfreq = 12
	sflags = ['ARROW_1']
	description = """He is a cunning orc of power, as tall as a man, and stronger.  It fears 
little.
"""

monsters.register_race(uruk)


class shagrat_the_orc_captain(orc):
	r_idx = "314"
	name = "Shagrat, the Orc Captain"
	ascii = "o,u"
	level = 18
	rarity = 2
	hp = "28d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 20
	exp = 400
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'ESCORT', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """He is an Uruk of power and great cunning.
"""

monsters.register_race(shagrat_the_orc_captain)


class gorbag_the_orc_captain(orc):
	r_idx = "315"
	name = "Gorbag, the Orc Captain"
	ascii = "o,u"
	level = 18
	rarity = 3
	hp = "28d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 20
	exp = 400
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'ESCORT', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A gruesomely ugly but cunning orc, his eyes regard you with hatred.  His 
powerful arms flex menacingly as he advances.
"""

monsters.register_race(gorbag_the_orc_captain)


class shambling_mound(monster, spellcaster):
	r_idx = "316"
	name = "Shambling mound"
	ascii = ",,g"
	level = 18
	rarity = 2
	hp = "15d6"
	speed = 110
	aaf = 20
	ac = 8
	sleep = 40
	exp = 75
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8')]
	flags = ['ONLY_GOLD', 'DROP_90', 'WILD_DUNGEON_01', 'WILD_SWAMP2', 'WILD_SWAMP1', 'STUPID', 'EMPTY_MIND', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 4
	sflags = ['SHRIEK']
	description = """A pile of rotting vegetation that slides towards you with a disgusting 
stench, waking all it nears.
"""

monsters.register_race(shambling_mound)


class white_shark(monster):
	r_idx = "317"
	name = "White shark"
	ascii = "l,W"
	level = 18
	rarity = 1
	hp = "7d10"
	speed = 120
	aaf = 20
	ac = 25
	sleep = 10
	exp = 68
	blows = [('BITE', 'HURT', '3d5'), ('BITE', 'HURT', '3d5')]
	flags = ['ANIMAL', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN']
	sfreq = 0
	sflags = []
	description = """Fast moving hunter of the depths, when this creature moves, 
everybody in water is in danger!
"""

monsters.register_race(white_shark)


class chaos_beastman(monster, spellcaster):
	r_idx = "318"
	name = "Chaos beastman"
	ascii = "H,u"
	level = 18
	rarity = 2
	hp = "14d8"
	speed = 110
	aaf = 20
	ac = 50
	sleep = 30
	exp = 75
	blows = [('HIT', 'CONFUSE', '3d5'), ('HIT', 'EXP_20', '3d5')]
	flags = ['DROP_1D2', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'OPEN_DOOR', 'BASH_DOOR', 'NO_CONF', 'NO_SLEEP', 'DROP_CORPSE', 'EVIL', 'IM_POIS', 'IM_FIRE', 'ATTR_ANY', 'ATTR_MULTI']
	sfreq = 8
	sflags = ['MISSILE', 'BO_FIRE', 'CONF', 'TPORT']
	description = """A truly loathsome thing, twisted by chaos, it is a mixture 
of several different kinds of creature.
"""

monsters.register_race(chaos_beastman)


class daemonette_of_slaanesh(demon, spellcaster):
	r_idx = "319"
	name = "Daemonette of Slaanesh"
	ascii = "u,R"
	level = 18
	rarity = 2
	hp = "9d8"
	speed = 120
	aaf = 20
	ac = 25
	sleep = 30
	exp = 75
	blows = [('CLAW', 'CONFUSE', '3d5'), ('CLAW', 'CONFUSE', '3d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'DROP_60', 'FEMALE', 'OPEN_DOOR', 'BASH_DOOR', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'EVIL', 'DEMON', 'IM_POIS', 'IM_FIRE']
	sfreq = 8
	sflags = ['SCARE', 'S_DEMON', 'CAUSE_2', 'CONF', 'BO_FIRE', 'BO_COLD']
	description = """It is minor female demon, vaguely human-like, but with crab-like 
pincers instead of hands. She wears a rather indecent skimpy 
leather bikini, moves quickly and casts deadly spells!
"""

monsters.register_race(daemonette_of_slaanesh)


class giant_bronze_dragon_fly(monster, spellcaster):
	r_idx = "320"
	name = "Giant bronze dragon fly"
	ascii = "F,U"
	level = 18
	rarity = 1
	hp = "2d8"
	speed = 120
	aaf = 12
	ac = 10
	sleep = 50
	exp = 70
	blows = []
	flags = ['FORCE_SLEEP', 'NEVER_BLOW', 'RAND_50', 'RAND_25', 'CAN_FLY', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['BR_CONF']
	description = """This vast gleaming bronze fly has wings which beat mesmerically fast.
"""

monsters.register_race(giant_bronze_dragon_fly)


class stone_giant(giant):
	r_idx = "321"
	name = "Stone giant"
	ascii = "P,s"
	level = 18
	rarity = 1
	hp = "20d8"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 90
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8')]
	flags = ['DROP_60', 'WILD_DUNGEON_01', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'GIANT', 'MALE', 'WILD_MOUNT2']
	sfreq = 0
	sflags = []
	description = """It is eighteen feet tall and looking at you.
"""

monsters.register_race(stone_giant)


class giant_black_dragon_fly(monster, spellcaster):
	r_idx = "322"
	name = "Giant black dragon fly"
	ascii = "F,D"
	level = 18
	rarity = 2
	hp = "2d8"
	speed = 120
	aaf = 12
	ac = 10
	sleep = 50
	exp = 68
	blows = []
	flags = ['FORCE_SLEEP', 'NEVER_BLOW', 'RAND_50', 'RAND_25', 'WEIRD_MIND', 'BASH_DOOR', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'ANIMAL', 'IM_ACID']
	sfreq = 9
	sflags = ['BR_ACID']
	description = """The size of a large bird, this fly drips caustic acid.
"""

monsters.register_race(giant_black_dragon_fly)


class stone_golem(monster):
	r_idx = "323"
	name = "Stone golem"
	ascii = "g,s"
	level = 19
	rarity = 2
	hp = "20d8"
	speed = 100
	aaf = 12
	ac = 35
	sleep = 10
	exp = 100
	blows = [('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10')]
	flags = ['WILD_DUNGEON_01', 'COLD_BLOOD', 'EMPTY_MIND', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """It is a massive animated statue.
"""

monsters.register_race(stone_golem)


class red_mold(monster):
	r_idx = "324"
	name = "Red mold"
	ascii = "m,r"
	level = 19
	rarity = 1
	hp = "11d8"
	speed = 110
	aaf = 2
	ac = 8
	sleep = 70
	exp = 64
	blows = [('TOUCH', 'FIRE', '4d4')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a strange red growth on the dungeon floor; it seems to burn with 
flame.
"""

monsters.register_race(red_mold)


class giant_gold_dragon_fly(monster, spellcaster):
	r_idx = "325"
	name = "Giant gold dragon fly"
	ascii = "F,y"
	level = 18
	rarity = 2
	hp = "2d8"
	speed = 120
	aaf = 12
	ac = 10
	sleep = 50
	exp = 78
	blows = [('BITE', 'HURT', '1d3')]
	flags = ['FORCE_SLEEP', 'RAND_50', 'RAND_25', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WEIRD_MIND', 'BASH_DOOR', 'ANIMAL', 'IM_FIRE', 'CAN_FLY']
	sfreq = 9
	sflags = ['BR_SOUN']
	description = """Large beating wings support this dazzling insect.  A loud buzzing noise 
pervades the air.
"""

monsters.register_race(giant_gold_dragon_fly)


class stunwall(monster):
	r_idx = "326"
	name = "Stunwall"
	ascii = "#,W"
	level = 18
	rarity = 5
	hp = "3d8"
	speed = 110
	aaf = 45
	ac = 15
	sleep = 0
	exp = 50
	blows = [('TOUCH', 'PARALYZE', '10d1'), ('TOUCH', 'PARALYZE', '10d1')]
	flags = ['WILD_DUNGEON_01', 'NEVER_MOVE', 'IM_COLD', 'COLD_BLOOD', 'IM_ACID', 'IM_ELEC', 'NO_FEAR', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'CHAR_MULTI', 'EMPTY_MIND', 'HURT_ROCK']
	sfreq = 0
	sflags = []
	description = """A sentient section of wall.
"""

monsters.register_race(stunwall)


class ghast(monster, undead):
	r_idx = "327"
	name = "Ghast"
	ascii = "z,U"
	level = 19
	rarity = 1
	hp = "9d10"
	speed = 110
	aaf = 40
	ac = 20
	sleep = 20
	exp = 75
	blows = [('KICK', 'HURT', '3d3'), ('KICK', 'HURT', '9d1'), ('BITE', 'HURT', '6d2')]
	flags = ['DROP_60', 'WILD_DUNGEON_01', 'NO_SLEEP', 'NO_CONF', 'UNDEAD', 'EVIL', 'IM_POIS', 'IM_COLD', 'COLD_BLOOD', 'HURT_LITE', 'CAN_SWIM', 'ELDRITCH_HORROR', 'BASH_DOOR', 'OPEN_DOOR']
	sfreq = 0
	sflags = []
	description = """A repulsive being which leaps on long hind legs like a kangaroo. 
Its face is curiously human despite the absence of a nose, a 
forehead, and "other important particulars".
"""

monsters.register_race(ghast)


class ixitxachitl_priest(monster, spellcaster):
	r_idx = "328"
	name = "Ixitxachitl priest"
	ascii = "l,s"
	level = 19
	rarity = 1
	hp = "7d10"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 20
	exp = 80
	blows = [('STING', 'POISON', '2d7'), ('STING', 'POISON', '2d7')]
	flags = ['ANIMAL', 'EVIL', 'AQUATIC', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_SHORE']
	sfreq = 6
	sflags = ['TELE_TO', 'HEAL', 'SCARE', 'CAUSE_2', 'BLIND', 'S_MONSTER']
	description = """A devil ray of the depths, with priestly magic.
"""

monsters.register_race(ixitxachitl_priest)


class huorn(monster, spellcaster):
	r_idx = "329"
	name = "Huorn"
	ascii = "%,g"
	level = 19
	rarity = 1
	hp = "35d10"
	speed = 110
	aaf = 40
	ac = 22
	sleep = 20
	exp = 75
	blows = [('CRUSH', 'HURT', '3d6'), ('CRUSH', 'HURT', '3d6'), ('CRUSH', 'HURT', '3d6'), ('CRUSH', 'HURT', '3d6')]
	flags = ['DROP_60', 'NO_SLEEP', 'NO_CONF', 'ANIMAL', 'WEIRD_MIND', 'EVIL', 'RES_WATE', 'IM_COLD', 'NEVER_MOVE', 'WILD_FOREST1', 'WILD_FOREST2']
	sfreq = 9
	sflags = ['BLINK', 'TELE_TO']
	description = """A very strong near-sentient tree, which has become hostile 
to humanoid forms of life.
"""

monsters.register_race(huorn)


class bolg_son_of_azog(orc):
	r_idx = "330"
	name = "Bolg, Son of Azog"
	ascii = "o,R"
	level = 20
	rarity = 4
	hp = "35d10"
	speed = 120
	aaf = 20
	ac = 25
	sleep = 20
	exp = 800
	blows = [('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'ESCORT', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A large and powerful orc.  He looks just like his daddy.  He is tall and 
fast, but fortunately blessed with orcish brains.
"""

monsters.register_race(bolg_son_of_azog)


class phase_spider(monster, spellcaster):
	r_idx = "331"
	name = "Phase spider"
	ascii = "S,B"
	level = 20
	rarity = 2
	hp = "4d8"
	speed = 120
	aaf = 15
	ac = 13
	sleep = 80
	exp = 60
	blows = [('BITE', 'HURT', '1d8'), ('BITE', 'POISON', '1d6'), ('BITE', 'POISON', '1d6')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'CAN_SWIM', 'ANIMAL', 'IM_POIS', 'RES_TELE']
	sfreq = 5
	sflags = ['BLINK', 'TELE_TO']
	description = """A spider that never seems quite there.  Everywhere you look it is just 
half-seen in the corner of one eye.
"""

monsters.register_race(phase_spider)


class lizard_king(monster):
	r_idx = "332"
	name = "Lizard king"
	ascii = "h,R"
	level = 20
	rarity = 3
	hp = "13d12"
	speed = 120
	aaf = 20
	ac = 20
	sleep = 20
	exp = 150
	blows = [('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('WAIL', 'TERRIFY', '')]
	flags = ['MALE', 'CAN_SWIM', 'IM_ACID', 'IM_POIS', 'WILD_SHORE', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_1D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'FORCE_MAXHP']
	sfreq = 0
	sflags = []
	description = """A lizardman leader.
"""

monsters.register_race(lizard_king)


class landmine(monster):
	r_idx = "333"
	name = "Landmine"
	ascii = ".,w"
	level = 20
	rarity = 5
	hp = "5d6"
	speed = 110
	aaf = 30
	ac = 12
	sleep = 10
	exp = 50
	blows = [('EXPLODE', 'HURT', '25d2')]
	flags = ['WILD_DUNGEON_01', 'CHAR_CLEAR', 'ATTR_CLEAR', 'NEVER_MOVE', 'FORCE_MAXHP', 'EMPTY_MIND', 'INVISIBLE', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It was left here to be used against intruders.
"""

monsters.register_race(landmine)


class wyvern(dragon):
	r_idx = "334"
	name = "Wyvern"
	ascii = "d,G"
	level = 20
	rarity = 2
	hp = "75d5"
	speed = 120
	aaf = 20
	ac = 32
	sleep = 20
	exp = 360
	blows = [('BITE', 'HURT', '2d6'), ('BITE', 'HURT', '2d6'), ('BITE', 'HURT', '2d6'), ('STING', 'POISON', '1d6')]
	flags = ['FORCE_SLEEP', 'ONLY_GOLD', 'DROP_60', 'DROP_90', 'IM_POIS', 'CAN_FLY', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'DRAGON', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_MOUNT2', 'WILD_MOUNT1', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A fast-moving and deadly dragonian animal. Beware its poisonous sting!
"""

monsters.register_race(wyvern)


class great_eagle(monster):
	r_idx = "335"
	name = "Great eagle"
	ascii = "B,u"
	level = 20
	rarity = 2
	hp = "75d5"
	speed = 120
	aaf = 20
	ac = 65
	sleep = 20
	exp = 150
	blows = [('CLAW', 'HURT', '6d3'), ('CLAW', 'HURT', '6d3'), ('BITE', 'HURT', '3d6')]
	flags = ['CAN_FLY', 'WILD_MOUNT1', 'WILD_MOUNT2', 'WILD_WASTE2', 'ANIMAL', 'GOOD', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """An agent of supernatural beings, this creature looks like a 
huge eagle.
"""

monsters.register_race(great_eagle)


class livingstone(monster):
	r_idx = "336"
	name = "Livingstone"
	ascii = "#,W"
	level = 20
	rarity = 4
	hp = "4d8"
	speed = 110
	aaf = 45
	ac = 14
	sleep = 20
	exp = 56
	blows = [('HIT', 'HURT', '2d5'), ('HIT', 'HURT', '2d5')]
	flags = ['WILD_DUNGEON_01', 'MULTIPLY', 'NEVER_MOVE', 'IM_COLD', 'COLD_BLOOD', 'IM_ACID', 'IM_ELEC', 'NO_FEAR', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'FRIENDS', 'CHAR_MULTI', 'HURT_ROCK']
	sfreq = 0
	sflags = []
	description = """A sentient section of wall.
"""

monsters.register_race(livingstone)


class earth_hound(monster, spellcaster):
	r_idx = "337"
	name = "Earth hound"
	ascii = "Z,u"
	level = 20
	rarity = 1
	hp = "10d8"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 200
	blows = [('BITE', 'HURT', '1d8'), ('BITE', 'HURT', '1d8'), ('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'ANIMAL']
	sfreq = 10
	sflags = ['BR_SHAR']
	description = """A beautiful crystalline shape does not disguise the danger this hound 
clearly presents.  Your flesh tingles as it approaches.
"""

monsters.register_race(earth_hound)


class air_hound(monster, spellcaster):
	r_idx = "338"
	name = "Air hound"
	ascii = "Z,B"
	level = 20
	rarity = 1
	hp = "10d8"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 200
	blows = [('BITE', 'HURT', '1d8'), ('BITE', 'HURT', '1d8'), ('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'FRIENDS', 'CAN_FLY', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'IM_POIS']
	sfreq = 10
	sflags = ['BR_POIS']
	description = """Swirling vapours surround this beast as it floats towards you, seemingly 
walking on air.  Noxious gases sting your throat.
"""

monsters.register_race(air_hound)


class sabre_tooth_tiger(monster):
	r_idx = "339"
	name = "Sabre-tooth tiger"
	ascii = "f,y"
	level = 20
	rarity = 2
	hp = "14d14"
	speed = 120
	aaf = 40
	ac = 25
	sleep = 0
	exp = 120
	blows = [('CLAW', 'HURT', '1d10'), ('CLAW', 'HURT', '1d10'), ('BITE', 'HURT', '1d10'), ('BITE', 'HURT', '1d10')]
	flags = ['BASH_DOOR', 'WILD_FOREST1', 'WILD_DUNGEON_01', 'WILD_GRASS', 'ANIMAL', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A fierce and dangerous cat, its huge tusks and sharp claws would lacerate 
even the strongest armour.
"""

monsters.register_race(sabre_tooth_tiger)


class water_hound(monster, spellcaster):
	r_idx = "340"
	name = "Water hound"
	ascii = "Z,b"
	level = 20
	rarity = 2
	hp = "10d8"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 200
	blows = [('BITE', 'ACID', '2d8'), ('BITE', 'ACID', '2d8'), ('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'FRIENDS', 'BASH_DOOR', 'CAN_SWIM', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'IM_ACID']
	sfreq = 10
	sflags = ['BR_ACID']
	description = """Liquid footprints follow this hound as it pads around the dungeon.  An 
acrid smell of acid rises from the dog's pelt.
"""

monsters.register_race(water_hound)


class chimera(monster, spellcaster):
	r_idx = "341"
	name = "Chimera"
	ascii = "H,y"
	level = 20
	rarity = 1
	hp = "9d8"
	speed = 110
	aaf = 12
	ac = 13
	sleep = 10
	exp = 200
	blows = [('BITE', 'HURT', '1d10'), ('BITE', 'FIRE', '1d3'), ('BITE', 'FIRE', '1d3')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'DROP_CORPSE', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'IM_FIRE']
	sfreq = 10
	sflags = ['BR_FIRE']
	description = """It is a strange concoction of lion, dragon and goat.  It looks very odd 
but very avoidable.
"""

monsters.register_race(chimera)


class quylthulg(monster, spellcaster):
	r_idx = "342"
	name = "Quylthulg"
	ascii = "Q,W"
	level = 20
	rarity = 1
	hp = "4d8"
	speed = 110
	aaf = 10
	ac = 1
	sleep = 2
	exp = 250
	blows = []
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'NEVER_MOVE', 'NEVER_BLOW', 'EMPTY_MIND', 'INVISIBLE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 4
	sflags = ['BLINK', 'S_MONSTER']
	description = """It is a strange pulsing mound of flesh.
"""

monsters.register_race(quylthulg)


class sasquatch(monster):
	r_idx = "343"
	name = "Sasquatch"
	ascii = "Y,W"
	level = 20
	rarity = 3
	hp = "14d19"
	speed = 120
	aaf = 15
	ac = 20
	sleep = 10
	exp = 180
	blows = [('CLAW', 'HURT', '1d10'), ('CLAW', 'HURT', '1d10'), ('BITE', 'HURT', '2d8')]
	flags = ['OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_WASTE2', 'ANIMAL', 'IM_COLD', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A tall shaggy, furry humanoid, it could call the yeti brother.
"""

monsters.register_race(sasquatch)


class weir(monster):
	r_idx = "344"
	name = "Weir"
	ascii = "C,W"
	level = 20
	rarity = 2
	hp = "7d12"
	speed = 110
	aaf = 15
	ac = 15
	sleep = 40
	exp = 150
	blows = [('BITE', 'HURT', '1d6'), ('BITE', 'HURT', '1d6')]
	flags = ['RAND_25', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'EVIL', 'FRIENDS', 'WILD_DUNGEON_01']
	sfreq = 0
	sflags = []
	description = """It is a tracker; half human, half beast.
"""

monsters.register_race(weir)


class whale(monster):
	r_idx = "345"
	name = "Whale"
	ascii = "l,G"
	level = 20
	rarity = 4
	hp = "22d20"
	speed = 110
	aaf = 15
	ac = 25
	sleep = 70
	exp = 175
	blows = [('CRUSH', 'HURT', '1d20'), ('CRUSH', 'HURT', '1d20')]
	flags = ['RAND_25', 'FORCE_MAXHP', 'RES_WATE', 'ANIMAL', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN']
	sfreq = 0
	sflags = []
	description = """Although it looks like a fish and lives in water, it is in fact 
a mammal. And it is huge!
"""

monsters.register_race(whale)


class electric_eel(monster):
	r_idx = "346"
	name = "Electric eel"
	ascii = "J,B"
	level = 20
	rarity = 2
	hp = "11d15"
	speed = 110
	aaf = 15
	ac = 20
	sleep = 70
	exp = 145
	blows = [('TOUCH', 'ELEC', '2d7'), ('TOUCH', 'ELEC', '7d2'), ('TOUCH', 'ELEC', '2d7')]
	flags = ['AQUATIC', 'ANIMAL', 'RAND_25', 'IM_ELEC', 'RES_WATE', 'WILD_DUNGEON_01', 'WILD_OCEAN']
	sfreq = 0
	sflags = []
	description = """This cute little serpentine creature can create a deadly voltage. 
Better watch out!
"""

monsters.register_race(electric_eel)


class werewolf(monster):
	r_idx = "347"
	name = "Werewolf"
	ascii = "C,D"
	level = 20
	rarity = 2
	hp = "14d22"
	speed = 110
	aaf = 15
	ac = 18
	sleep = 20
	exp = 150
	blows = [('BITE', 'HURT', '1d6'), ('BITE', 'HURT', '1d6'), ('BITE', 'HURT', '1d10')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'ANIMAL', 'EVIL']
	sfreq = 0
	sflags = []
	description = """It is a huge wolf with eyes that glow with manly intelligence.
"""

monsters.register_race(werewolf)


class dark_elven_lord(monster, spellcaster):
	r_idx = "348"
	name = "Dark elven lord"
	ascii = "h,D"
	level = 20
	rarity = 2
	hp = "13d15"
	speed = 120
	aaf = 20
	ac = 20
	sleep = 30
	exp = 500
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'FORCE_SLEEP', 'ONLY_ITEM', 'DROP_2D2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'HURT_LITE']
	sfreq = 5
	sflags = ['HASTE', 'BLIND', 'CONF', 'DARKNESS', 'BO_FIRE', 'BO_COLD', 'MISSILE']
	description = """A dark elven figure in armour and radiating evil power.
"""

monsters.register_race(dark_elven_lord)


class cloud_giant(giant):
	r_idx = "349"
	name = "Cloud giant"
	ascii = "P,B"
	level = 20
	rarity = 1
	hp = "25d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 50
	exp = 125
	blows = [('HIT', 'ELEC', '3d8'), ('HIT', 'ELEC', '3d8')]
	flags = ['DROP_90', 'WILD_DUNGEON_01', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'GIANT', 'IM_ELEC', 'MALE']
	sfreq = 0
	sflags = []
	description = """It is a twenty foot tall giant wreathed in clouds.
"""

monsters.register_race(cloud_giant)


class ugluk_the_uruk(orc):
	r_idx = "350"
	name = "Ugluk, the Uruk"
	ascii = "o,u"
	level = 20
	rarity = 4
	hp = "45d10"
	speed = 110
	aaf = 20
	ac = 42
	sleep = 20
	exp = 550
	blows = [('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ESCORT', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """Another of Morgoth's servants, this orc is strong and cunning.  He is ugly 
and scarred from many power struggles.
"""

monsters.register_race(ugluk_the_uruk)


class blue_dragon_bat(monster, spellcaster):
	r_idx = "351"
	name = "Blue dragon bat"
	ascii = "b,B"
	level = 21
	rarity = 1
	hp = "3d4"
	speed = 130
	aaf = 12
	ac = 13
	sleep = 50
	exp = 54
	blows = [('BITE', 'ELEC', '1d3')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'RAND_50', 'BASH_DOOR', 'CAN_FLY', 'ANIMAL', 'IM_ELEC']
	sfreq = 4
	sflags = ['BR_ELEC']
	description = """It is a glowing blue bat with a sharp tail.
"""

monsters.register_race(blue_dragon_bat)


class scroll_mimic(monster, spellcaster):
	r_idx = "352"
	name = "Scroll mimic"
	ascii = "?,w"
	level = 21
	rarity = 3
	hp = "7d14"
	speed = 110
	aaf = 30
	ac = 20
	sleep = 0
	exp = 60
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'POISON', '3d4'), ('HIT', 'HURT', '2d3'), ('HIT', 'HURT', '2d3')]
	flags = ['CHAR_MULTI', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'NEVER_MOVE', 'EMPTY_MIND', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 5
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_2', 'BO_FIRE', 'S_MONSTER']
	description = """A strange creature that disguises itself as discarded objects to lure 
unsuspecting adventurers within reach of its venomous claws.
"""

monsters.register_race(scroll_mimic)


class chest_mimic(monster, spellcaster):
	r_idx = "353"
	name = "Chest mimic"
	ascii = "&,W"
	level = 21
	rarity = 6
	hp = "7d14"
	speed = 110
	aaf = 30
	ac = 20
	sleep = 0
	exp = 70
	blows = [('GAZE', 'CONFUSE', '2d3'), ('CLAW', 'POISON', '3d4'), ('CLAW', 'POISON', '3d4'), ('SPIT', 'BLIND', '2d3')]
	flags = ['CHAR_MULTI', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'NEVER_MOVE', 'EMPTY_MIND', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 5
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_2', 'BO_POIS', 'BA_POIS', 'S_MONSTER']
	description = """A strange creature that disguises itself as a chest to lure 
unsuspecting adventurers within reach of its venomous claws.
"""

monsters.register_race(chest_mimic)


class fire_vortex(monster, spellcaster):
	r_idx = "354"
	name = "Fire vortex"
	ascii = "v,r"
	level = 21
	rarity = 1
	hp = "6d9"
	speed = 110
	aaf = 100
	ac = 12
	sleep = 0
	exp = 100
	blows = [('ENGULF', 'FIRE', '3d3')]
	flags = ['FORCE_SLEEP', 'RAND_50', 'CAN_FLY', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'AURA_FIRE', 'IM_FIRE', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_FIRE']
	description = """A whirling maelstrom of fire.
"""

monsters.register_race(fire_vortex)


class water_vortex(monster, spellcaster):
	r_idx = "355"
	name = "Water vortex"
	ascii = "v,b"
	level = 21
	rarity = 1
	hp = "6d9"
	speed = 110
	aaf = 100
	ac = 12
	sleep = 0
	exp = 100
	blows = [('ENGULF', 'ACID', '3d3')]
	flags = ['FORCE_SLEEP', 'RAND_50', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'IM_ACID', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_ACID']
	description = """A caustic spinning whirlpool of water.
"""

monsters.register_race(water_vortex)


class lugdush_the_uruk(orc):
	r_idx = "356"
	name = "Lugdush, the Uruk"
	ascii = "o,b"
	level = 21
	rarity = 3
	hp = "50d10"
	speed = 110
	aaf = 20
	ac = 45
	sleep = 20
	exp = 550
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ESCORT', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A strong and cunning orc warrior, Lugdush sneers as he insults your mother.
"""

monsters.register_race(lugdush_the_uruk)


class arch_vile(demon, spellcaster):
	r_idx = "357"
	name = "Arch-vile"
	ascii = "u,W"
	level = 21
	rarity = 1
	hp = "8d11"
	speed = 130
	aaf = 100
	ac = 15
	sleep = 0
	exp = 300
	blows = [('CLAW', 'HURT', '3d9'), ('CLAW', 'HURT', '3d9')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'EVIL', 'DEMON', 'FORCE_SLEEP', 'FORCE_MAXHP', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'COLD_BLOOD', 'IM_FIRE', 'RES_NETH', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'NO_STUN']
	sfreq = 3
	sflags = ['BA_FIRE', 'ANIM_DEAD']
	description = """A pale, corpse-like lesser demon, who moves very fast and spawns evil 
everywhere.
"""

monsters.register_race(arch_vile)


class cold_vortex(monster, spellcaster):
	r_idx = "358"
	name = "Cold vortex"
	ascii = "v,W"
	level = 21
	rarity = 1
	hp = "6d9"
	speed = 110
	aaf = 100
	ac = 12
	sleep = 0
	exp = 100
	blows = [('ENGULF', 'COLD', '3d3')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'RAND_50', 'AURA_COLD', 'COLD_BLOOD', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'IM_COLD', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_COLD']
	description = """A twisting whirlpool of frost.
"""

monsters.register_race(cold_vortex)


class energy_vortex(monster, spellcaster):
	r_idx = "359"
	name = "Energy vortex"
	ascii = "v,y"
	level = 21
	rarity = 1
	hp = "8d12"
	speed = 110
	aaf = 100
	ac = 12
	sleep = 0
	exp = 130
	blows = [('ENGULF', 'ELEC', '5d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'RAND_50', 'CAN_FLY', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'AURA_ELEC', 'IM_ELEC', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_ELEC']
	description = """A shimmering tornado of air, sparks crackle along its length.
"""

monsters.register_race(energy_vortex)


class globefish(monster, spellcaster):
	r_idx = "360"
	name = "Globefish"
	ascii = "l,w"
	level = 21
	rarity = 1
	hp = "14d10"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 30
	exp = 200
	blows = [('BITE', 'POISON', '5d5'), ('BITE', 'POISON', '5d5')]
	flags = ['EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'AQUATIC', 'IM_POIS', 'NO_STUN', 'WILD_DUNGEON_01', 'WILD_SHORE', 'FORCE_MAXHP']
	sfreq = 7
	sflags = ['BR_POIS']
	description = """This cute fish is among the most poisonous creatures there are.
"""

monsters.register_race(globefish)


class carrion(monster, undead):
	r_idx = "361"
	name = "Carrion"
	ascii = "B,W"
	level = 21
	rarity = 1
	hp = "11d12"
	speed = 113
	aaf = 20
	ac = 15
	sleep = 20
	exp = 70
	blows = [('CLAW', 'POISON', '2d3'), ('CLAW', 'POISON', '2d3'), ('BITE', 'EXP_20', '1d8')]
	flags = ['DROP_90', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ANIMAL', 'UNDEAD', 'CAN_FLY', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A mummified, undead great avian creature, who now serves as 
a steed for mighty undead creatures.
"""

monsters.register_race(carrion)


class mummified_orc(orc, undead):
	r_idx = "362"
	name = "Mummified orc"
	ascii = "z,u"
	level = 21
	rarity = 1
	hp = "12d8"
	speed = 110
	aaf = 20
	ac = 14
	sleep = 75
	exp = 56
	blows = [('HIT', 'HURT', '2d4'), ('HIT', 'HURT', '2d4')]
	flags = ['DROP_90', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is an orcish figure covered in wrappings.
"""

monsters.register_race(mummified_orc)


class killer_whale(monster):
	r_idx = "363"
	name = "Killer whale"
	ascii = "l,w"
	level = 22
	rarity = 1
	hp = "22d11"
	speed = 115
	aaf = 12
	ac = 26
	sleep = 30
	exp = 85
	blows = [('BITE', 'HURT', '8d3'), ('BITE', 'HURT', '8d3')]
	flags = ['AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """An almost beautiful, deadly beast.
"""

monsters.register_race(killer_whale)


class serpent_man(monster, spellcaster):
	r_idx = "364"
	name = "Serpent man"
	ascii = "J,g"
	level = 22
	rarity = 3
	hp = "12d10"
	speed = 120
	aaf = 20
	ac = 20
	sleep = 20
	exp = 75
	blows = [('BITE', 'POISON', '5d5'), ('BITE', 'POISON', '5d5')]
	flags = ['WILD_DUNGEON_01', 'WILD_SWAMP1', 'MALE', 'CAN_SWIM', 'IM_POIS', 'IM_ACID', 'DROP_60', 'DROP_1D2', 'FRIENDS', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 8
	sflags = ['BA_POIS', 'S_MONSTER', 'SCARE', 'HOLD', 'BO_POIS']
	description = """"They walked lithely and sinnously erect on pre-mammalian 
members, their pied and hairless bodies bending with great 
suppleness. There was a loud hissing of formulae as they 
went to and fro."
"""

monsters.register_race(serpent_man)


class vampiric_mist(monster):
	r_idx = "365"
	name = "Vampiric mist"
	ascii = "#,D"
	level = 22
	rarity = 1
	hp = "7d8"
	speed = 110
	aaf = 12
	ac = 26
	sleep = 10
	exp = 40
	blows = [('ENGULF', 'EXP_VAMP', '2d6')]
	flags = ['RAND_25', 'IM_COLD', 'IM_POIS', 'IM_ACID', 'RES_NETH', 'WILD_DUNGEON_01', 'WILD_SWAMP2', 'EVIL', 'EMPTY_MIND', 'COLD_BLOOD', 'FRIENDS']
	sfreq = 0
	sflags = []
	description = """A cloud of evil, sentient mist.
"""

monsters.register_race(vampiric_mist)


class killer_stag_beetle(monster):
	r_idx = "366"
	name = "Killer stag beetle"
	ascii = "K,s"
	level = 22
	rarity = 1
	hp = "14d8"
	speed = 110
	aaf = 12
	ac = 27
	sleep = 30
	exp = 80
	blows = [('CLAW', 'HURT', '1d12'), ('CLAW', 'HURT', '1d12')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'DROP_CORPSE', 'WEIRD_MIND', 'BASH_DOOR', 'ANIMAL', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """It is a giant beetle with vicious claws.
"""

monsters.register_race(killer_stag_beetle)


class iron_golem(monster, spellcaster):
	r_idx = "367"
	name = "Iron golem"
	ascii = "g,W"
	level = 22
	rarity = 2
	hp = "56d12"
	speed = 110
	aaf = 12
	ac = 40
	sleep = 10
	exp = 160
	blows = [('HIT', 'HURT', '1d12')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 7
	sflags = ['SLOW']
	description = """It is a massive metal statue that moves steadily towards you.
"""

monsters.register_race(iron_golem)


class auto_roller(monster):
	r_idx = "368"
	name = "Auto-roller"
	ascii = "g,s"
	level = 22
	rarity = 2
	hp = "55d12"
	speed = 120
	aaf = 10
	ac = 40
	sleep = 12
	exp = 230
	blows = [('CRUSH', 'HURT', '1d8'), ('CRUSH', 'HURT', '1d8'), ('CRUSH', 'HURT', '1d8'), ('CRUSH', 'HURT', '1d8')]
	flags = ['FORCE_SLEEP', 'RES_TELE', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """It looks like a huge spiked roller, moving on its own towards you.
"""

monsters.register_race(auto_roller)


class giant_yellow_scorpion(monster):
	r_idx = "369"
	name = "Giant yellow scorpion"
	ascii = "S,y"
	level = 22
	rarity = 1
	hp = "9d8"
	speed = 110
	aaf = 12
	ac = 19
	sleep = 20
	exp = 60
	blows = [('BITE', 'HURT', '1d8'), ('STING', 'POISON', '2d5')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'ANIMAL', 'DROP_SKELETON']
	sfreq = 0
	sflags = []
	description = """It is a giant scorpion with a sharp stinger.
"""

monsters.register_race(giant_yellow_scorpion)


class jade_monk(monster, spellcaster):
	r_idx = "370"
	name = "Jade monk"
	ascii = "p,G"
	level = 30
	rarity = 2
	hp = "41d12"
	speed = 120
	aaf = 30
	ac = 24
	sleep = 10
	exp = 800
	blows = [('KICK', 'HURT', '5d2'), ('HIT', 'HURT', '5d1'), ('KICK', 'HURT', '5d5'), ('HIT', 'HURT', '5d1')]
	flags = ['WILD_DUNGEON_01', 'MALE', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'IM_ELEC', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'DROP_CORPSE', 'DROP_SKELETON']
	sfreq = 6
	sflags = ['BO_ELEC']
	description = """A green-garbed monk, trained in martial arts.
"""

monsters.register_race(jade_monk)


class black_ooze(monster, spellcaster):
	r_idx = "371"
	name = "Black ooze"
	ascii = "j,D"
	level = 23
	rarity = 1
	hp = "4d9"
	speed = 90
	aaf = 10
	ac = 10
	sleep = 1
	exp = 7
	blows = [('TOUCH', 'ACID', '2d6')]
	flags = ['RAND_50', 'DROP_60', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'MULTIPLY', 'CAN_SWIM', 'TAKE_ITEM', 'KILL_BODY', 'OPEN_DOOR', 'BASH_DOOR', 'IM_POIS', 'NO_FEAR']
	sfreq = 11
	sflags = ['DRAIN_MANA']
	description = """It is a strangely moving puddle.
"""

monsters.register_race(black_ooze)


class hardened_warrior(monster):
	r_idx = "372"
	name = "Hardened warrior"
	ascii = "p,u"
	level = 23
	rarity = 1
	hp = "10d11"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 40
	exp = 60
	blows = [('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_1D2', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A scarred warrior who moves with confidence.
"""

monsters.register_race(hardened_warrior)


class azog_king_of_the_uruk_hai(orc):
	r_idx = "373"
	name = "Azog, King of the Uruk-Hai"
	ascii = "o,r"
	level = 23
	rarity = 5
	hp = "63d10"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 20
	exp = 1111
	blows = [('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ESCORT', 'ESCORTS', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ORC', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """He is also known as the King of Khazad-dum.  His ego is renowned to be 
bigger than his head.
"""

monsters.register_race(azog_king_of_the_uruk_hai)


class fleshhound_of_khorne(demon):
	r_idx = "374"
	name = "Fleshhound of Khorne"
	ascii = "C,R"
	level = 23
	rarity = 3
	hp = "14d20"
	speed = 120
	aaf = 15
	ac = 15
	sleep = 70
	exp = 150
	blows = [('CLAW', 'HURT', '4d1'), ('CLAW', 'HURT', '4d1'), ('BITE', 'HURT', '6d1'), ('BITE', 'HURT', '6d1')]
	flags = ['BASH_DOOR', 'DEMON', 'NO_STUN', 'NO_FEAR', 'WILD_DUNGEON_01', 'ANIMAL', 'EVIL', 'IM_FIRE', 'NO_SLEEP', 'NO_CONF', 'RES_NETH', 'RES_NEXU', 'RES_DISE', 'RES_PLAS']
	sfreq = 0
	sflags = []
	description = """A revolting canine creature, a huge demon hound with a somewhat 
reptilian head.
"""

monsters.register_race(fleshhound_of_khorne)


class dark_elven_warlock(monster, spellcaster):
	r_idx = "375"
	name = "Dark elven warlock"
	ascii = "h,v"
	level = 23
	rarity = 1
	hp = "5d10"
	speed = 120
	aaf = 20
	ac = 8
	sleep = 20
	exp = 75
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'FRIENDS', 'EVIL', 'IM_POIS', 'HURT_LITE']
	sfreq = 5
	sflags = ['CONF', 'MISSILE', 'DARKNESS', 'BO_MANA']
	description = """A dark elven mage with spells of frightening destructive power.
"""

monsters.register_race(dark_elven_warlock)


class master_rogue(monster):
	r_idx = "376"
	name = "Master rogue"
	ascii = "p,D"
	level = 23
	rarity = 2
	hp = "10d9"
	speed = 120
	aaf = 20
	ac = 18
	sleep = 40
	exp = 110
	blows = [('HIT', 'HURT', '2d8'), ('HIT', 'HURT', '2d8'), ('HIT', 'EAT_GOLD', '4d4')]
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_WASTE1', 'WILD_GRASS', 'MALE', 'DROP_SKELETON', 'DROP_CORPSE', 'DROP_2D2', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A thief of great power and shifty speed.
"""

monsters.register_race(master_rogue)


class red_dragon_bat(monster, spellcaster):
	r_idx = "377"
	name = "Red dragon bat"
	ascii = "b,r"
	level = 23
	rarity = 1
	hp = "2d9"
	speed = 130
	aaf = 12
	ac = 14
	sleep = 50
	exp = 60
	blows = [('BITE', 'FIRE', '1d3')]
	flags = ['FORCE_SLEEP', 'RAND_50', 'WILD_DUNGEON_01', 'BASH_DOOR', 'CAN_FLY', 'DROP_CORPSE', 'ANIMAL', 'IM_FIRE']
	sfreq = 4
	sflags = ['BR_FIRE']
	description = """It is a sharp-tailed bat, wreathed in fire.
"""

monsters.register_race(red_dragon_bat)


class killer_white_beetle(monster):
	r_idx = "378"
	name = "Killer white beetle"
	ascii = "K,w"
	level = 23
	rarity = 1
	hp = "14d8"
	speed = 110
	aaf = 14
	ac = 27
	sleep = 30
	exp = 85
	blows = [('BITE', 'HURT', '4d5')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'DROP_CORPSE', 'ANIMAL', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """It is looking for prey.
"""

monsters.register_race(killer_white_beetle)


class ice_skeleton(monster, undead):
	r_idx = "379"
	name = "Ice skeleton"
	ascii = "s,w"
	level = 23
	rarity = 1
	hp = "14d9"
	speed = 110
	aaf = 20
	ac = 17
	sleep = 60
	exp = 70
	blows = [('CLAW', 'COLD', '2d3'), ('CLAW', 'COLD', '2d3')]
	flags = ['ONLY_ITEM', 'DROP_90', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a skeleton covered in frost.
"""

monsters.register_race(ice_skeleton)


class angamaite_of_umbar(monster, spellcaster):
	r_idx = "380"
	name = "Angamaite of Umbar"
	ascii = "p,u"
	level = 24
	rarity = 2
	hp = "80d7"
	speed = 110
	aaf = 25
	ac = 40
	sleep = 25
	exp = 400
	blows = [('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_90', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['SLOW', 'FORGET']
	description = """A Black Numenorean who hates the men of the west.
"""

monsters.register_race(angamaite_of_umbar)


class kouko(monster, undead, spellcaster):
	r_idx = "381"
	name = "Kouko"
	ascii = "W,G"
	level = 24
	rarity = 1
	hp = "8d8"
	speed = 110
	aaf = 20
	ac = 15
	sleep = 30
	exp = 140
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('TOUCH', 'EXP_20', '')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 10
	sflags = ['SCARE', 'DRAIN_MANA']
	description = """It is a ghostly apparition with a humanoid form.
"""

monsters.register_race(kouko)


class mime_the_nibelung(monster, spellcaster):
	r_idx = "382"
	name = "Mime, the Nibelung"
	ascii = "h,u"
	level = 24
	rarity = 2
	hp = "82d7"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 10
	exp = 300
	blows = [('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6'), ('HIT', 'UN_BONUS', ''), ('TOUCH', 'EAT_GOLD', '')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'RES_DISE', 'RES_TELE']
	sfreq = 8
	sflags = ['HEAL', 'HASTE', 'BO_FIRE']
	description = """This tiny night dwarf is as greedy for gold as his brother, Alberich.
"""

monsters.register_race(mime_the_nibelung)


class hagen_son_of_alberich(monster, spellcaster):
	r_idx = "383"
	name = "Hagen, son of Alberich"
	ascii = "h,o"
	level = 24
	rarity = 2
	hp = "82d7"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 10
	exp = 300
	blows = [('HIT', 'HURT', '3d7'), ('HIT', 'HURT', '3d7'), ('HIT', 'HURT', '3d7'), ('HIT', 'UN_BONUS', '')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'DROP_CHOSEN', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 8
	sflags = ['HEAL', 'HASTE', 'BO_FIRE']
	description = """Alberich's son, born of a mortal woman won with gold.
"""

monsters.register_race(hagen_son_of_alberich)


class meneldor_the_swift(monster):
	r_idx = "384"
	name = "Meneldor the Swift"
	ascii = "B,u"
	level = 24
	rarity = 5
	hp = "80d7"
	speed = 130
	aaf = 20
	ac = 32
	sleep = 20
	exp = 360
	blows = [('CLAW', 'HURT', '7d3'), ('CLAW', 'HURT', '7d3'), ('BITE', 'HURT', '3d7')]
	flags = ['CAN_FLY', 'UNIQUE', 'FORCE_MAXHP', 'DROP_CORPSE', 'WILD_MOUNT2', 'WILD_WASTE2', 'FRIENDLY', 'ANIMAL', 'GOOD']
	sfreq = 0
	sflags = []
	description = """An agent of supernatural beings, this creature looks like a 
huge eagle.
"""

monsters.register_race(meneldor_the_swift)


class phantom_beast(monster):
	r_idx = "385"
	name = "Phantom beast"
	ascii = "G,B"
	level = 24
	rarity = 1
	hp = "8d12"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 40
	exp = 100
	blows = [('HIT', 'HURT', '44d2'), ('HIT', 'HURT', '44d2')]
	flags = ['WILD_DUNGEON_01', 'PASS_WALL', 'NO_SLEEP', 'COLD_BLOOD', 'NONLIVING', 'NO_FEAR', 'FORCE_MAXHP', 'RES_TELE', 'EMPTY_MIND', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """A creature that is half real, half illusion.
"""

monsters.register_race(phantom_beast)


class great_white_shark(monster):
	r_idx = "386"
	name = "Great white shark"
	ascii = "l,w"
	level = 24
	rarity = 2
	hp = "75d6"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 20
	exp = 250
	blows = [('BITE', 'HURT', '3d6'), ('BITE', 'HURT', '3d6'), ('BITE', 'HURT', '3d6')]
	flags = ['FORCE_SLEEP', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A very large carnivorous fish.
"""

monsters.register_race(great_white_shark)


class four_headed_hydra(monster, spellcaster):
	r_idx = "387"
	name = "4-headed hydra"
	ascii = "M,U"
	level = 24
	rarity = 2
	hp = "75d6"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 20
	exp = 350
	blows = [('BITE', 'HURT', '2d6'), ('BITE', 'HURT', '2d6'), ('BITE', 'HURT', '2d6'), ('BITE', 'HURT', '2d6')]
	flags = ['FORCE_SLEEP', 'ONLY_GOLD', 'DROP_4D2', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SHORE', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 7
	sflags = ['SCARE']
	description = """A strange reptilian hybrid with four heads, guarding its hoard.
"""

monsters.register_race(four_headed_hydra)


class lesser_hell_beast(monster, spellcaster):
	r_idx = "388"
	name = "Lesser hell-beast"
	ascii = "U,s"
	level = 24
	rarity = 4
	hp = "15d75"
	speed = 115
	aaf = 10
	ac = 50
	sleep = 50
	exp = 400
	blows = [('GAZE', 'TERRIFY', '1d4'), ('GAZE', 'TERRIFY', '1d4'), ('CRUSH', 'HURT', '44d1')]
	flags = ['WILD_DUNGEON_01', 'EVIL', 'IM_ACID', 'IM_ELEC', 'IM_FIRE', 'IM_POIS', 'IM_COLD', 'RES_NETH', 'RES_WATE', 'RES_PLAS', 'RES_DISE', 'RES_NEXU', 'KILL_WALL', 'FORCE_MAXHP', 'CAN_SWIM', 'DROP_CORPSE']
	sfreq = 9
	sflags = ['TPORT', 'BLINK', 'TELE_AWAY']
	description = """This creature just might crush you.
"""

monsters.register_race(lesser_hell_beast)


class tyrannosaurus(monster):
	r_idx = "389"
	name = "Tyrannosaurus"
	ascii = "R,g"
	level = 24
	rarity = 2
	hp = "150d3"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 20
	exp = 350
	blows = [('CLAW', 'HURT', '1d6'), ('CLAW', 'HURT', '1d6'), ('BITE', 'HURT', '3d6'), ('BITE', 'HURT', '3d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A horror from prehistory, reawakened by chaos.
"""

monsters.register_race(tyrannosaurus)


class mummified_human(monster, undead):
	r_idx = "390"
	name = "Mummified human"
	ascii = "z,U"
	level = 24
	rarity = 1
	hp = "18d8"
	speed = 110
	aaf = 20
	ac = 17
	sleep = 60
	exp = 70
	blows = [('HIT', 'HURT', '2d4'), ('HIT', 'HURT', '2d4')]
	flags = ['ONLY_ITEM', 'DROP_90', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a human form encased in mouldy wrappings.
"""

monsters.register_race(mummified_human)


class vampire_bat(monster, undead):
	r_idx = "391"
	name = "Vampire bat"
	ascii = "b,D"
	level = 24
	rarity = 2
	hp = "9d7"
	speed = 120
	aaf = 12
	ac = 20
	sleep = 50
	exp = 150
	blows = [('BITE', 'EXP_40', '1d4'), ('BITE', 'EXP_40', '1d4')]
	flags = ['WILD_DUNGEON_01', 'WILD_WASTE2', 'WILD_MOUNT2', 'RAND_50', 'COLD_BLOOD', 'REGENERATE', 'CAN_FLY', 'EVIL', 'ANIMAL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """An undead bat that flies at your neck hungrily.
"""

monsters.register_race(vampire_bat)


class sangahyando_of_umbar(monster, spellcaster):
	r_idx = "392"
	name = "Sangahyando of Umbar"
	ascii = "p,o"
	level = 24
	rarity = 2
	hp = "80d7"
	speed = 110
	aaf = 25
	ac = 40
	sleep = 25
	exp = 400
	blows = [('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_90', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['SLOW', 'FORGET']
	description = """A Black Numenorean with a blacker heart.
"""

monsters.register_race(sangahyando_of_umbar)


class it(monster, spellcaster):
	r_idx = "393"
	name = "It"
	ascii = ".,W"
	level = 24
	rarity = 3
	hp = "58d11"
	speed = 110
	aaf = 25
	ac = 40
	sleep = 25
	exp = 400
	blows = [('GAZE', 'BLIND', '8d8'), ('TOUCH', 'TERRIFY', ''), ('GAZE', 'EXP_40', ''), ('TOUCH', 'EAT_ITEM', '')]
	flags = ['WILD_DUNGEON_01', 'WILD_SWAMP2', 'WILD_WASTE2', 'ONLY_ITEM', 'DROP_90', 'DROP_1D2', 'DROP_GOOD', 'DROP_GREAT', 'DROP_CORPSE', 'CHAR_MULTI', 'CHAR_CLEAR', 'ATTR_CLEAR', 'INVISIBLE', 'COLD_BLOOD', 'NO_CONF', 'UNIQUE', 'FORCE_MAXHP', 'NO_SLEEP', 'CAN_SPEAK', 'REFLECTING', 'IM_FIRE', 'IM_ELEC', 'EMPTY_MIND', 'EVIL', 'SMART', 'RES_TELE', 'CAN_FLY']
	sfreq = 4
	sflags = ['DRAIN_MANA', 'BLINK', 'BLIND', 'SCARE', 'CONF', 'S_UNDEAD', 'S_MONSTER', 'HEAL', 'TELE_AWAY', 'DARKNESS', 'S_HYDRA', 'TRAPS', 'FORGET', 'TELE_TO', 'SHRIEK']
	description = """Nobody has ever seen It.
"""

monsters.register_race(it)


class banshee(monster, undead, spellcaster):
	r_idx = "394"
	name = "Banshee"
	ascii = "G,W"
	level = 24
	rarity = 2
	hp = "4d9"
	speed = 120
	aaf = 20
	ac = 12
	sleep = 10
	exp = 60
	blows = [('WAIL', 'TERRIFY', ''), ('TOUCH', 'EXP_20', '')]
	flags = ['FEMALE', 'WILD_DUNGEON_01', 'RAND_50', 'DROP_1D2', 'INVISIBLE', 'COLD_BLOOD', 'TAKE_ITEM', 'PASS_WALL', 'CAN_FLY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['TPORT', 'DRAIN_MANA']
	description = """It is a ghostly woman's form that wails mournfully.
"""

monsters.register_race(banshee)


class carrion_crawler(monster):
	r_idx = "395"
	name = "Carrion crawler"
	ascii = "c,g"
	level = 25
	rarity = 2
	hp = "14d12"
	speed = 110
	aaf = 15
	ac = 20
	sleep = 10
	exp = 60
	blows = [('STING', 'PARALYZE', '2d6'), ('STING', 'PARALYZE', '2d6')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WEIRD_MIND', 'BASH_DOOR', 'DROP_SKELETON', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A hideous centipede covered in slime and with glowing tentacles around its 
head.
"""

monsters.register_race(carrion_crawler)


class xiclotlan(monster):
	r_idx = "396"
	name = "Xiclotlan"
	ascii = "#,D"
	level = 25
	rarity = 2
	hp = "19d25"
	speed = 110
	aaf = 15
	ac = 30
	sleep = 10
	exp = 60
	blows = [('CRUSH', 'HURT', '6d4'), ('CRUSH', 'HURT', '6d4'), ('BITE', 'HURT', '3d2')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'BASH_DOOR', 'FORCE_MAXHP', 'IM_POIS', 'IM_ELEC']
	sfreq = 0
	sflags = []
	description = """"...a metallically grey tree... about sixteen feet high with 
very thick cylindrical branches... cylinders further 
divided into six flat circular extensions... and from the top 
of what I had taken for a trunk rose a featureless oval... an 
orifice gaping at the top."
"""

monsters.register_race(xiclotlan)


class silent_watcher(monster, spellcaster):
	r_idx = "397"
	name = "Silent watcher"
	ascii = "g,s"
	level = 25
	rarity = 5
	hp = "60d20"
	speed = 110
	aaf = 42
	ac = 40
	sleep = 10
	exp = 590
	blows = [('GAZE', 'TERRIFY', ''), ('GAZE', 'PARALYZE', ''), ('GAZE', 'LOSE_STR', '')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'NONLIVING', 'NEVER_MOVE', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'EVIL', 'HURT_ROCK', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'RES_TELE']
	sfreq = 6
	sflags = ['SHRIEK', 'S_MONSTER', 'S_MONSTERS', 'HOLD', 'CONF', 'MIND_BLAST', 'DRAIN_MANA']
	description = """A figure carved from stone, with three vulture faces. Their eyes glow 
a malevolent light...
"""

monsters.register_race(silent_watcher)


class pukelman(monster, spellcaster):
	r_idx = "398"
	name = "Pukelman"
	ascii = "g,u"
	level = 25
	rarity = 3
	hp = "46d12"
	speed = 110
	aaf = 12
	ac = 40
	sleep = 10
	exp = 600
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '3d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 4
	sflags = ['SLOW', 'CONF', 'BO_ACID']
	description = """A stumpy figure carved from stone, with glittering eyes.
"""

monsters.register_race(pukelman)


class disenchanter_beast(monster, spellcaster):
	r_idx = "399"
	name = "Disenchanter beast"
	ascii = "q,w"
	level = 25
	rarity = 2
	hp = "23d30"
	speed = 110
	aaf = 12
	ac = 30
	sleep = 12
	exp = 250
	blows = [('TOUCH', 'UN_BONUS', '1d10'), ('TOUCH', 'UN_BONUS', '1d10'), ('TOUCH', 'UN_BONUS', '1d10'), ('TOUCH', 'UN_BONUS', '1d10')]
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'STUPID', 'WEIRD_MIND', 'RES_DISE', 'DROP_CORPSE', 'IM_ACID', 'IM_POIS', 'NO_CONF']
	sfreq = 6
	sflags = ['DRAIN_MANA']
	description = """It looks like an anteater, and there is a static feeling 
crackling around its long trunk.
"""

monsters.register_race(disenchanter_beast)


class dark_elven_druid(monster, spellcaster):
	r_idx = "400"
	name = "Dark elven druid"
	ascii = "h,G"
	level = 25
	rarity = 3
	hp = "14d20"
	speed = 120
	aaf = 15
	ac = 25
	sleep = 10
	exp = 500
	blows = [('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '3d8')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'FORCE_SLEEP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['HEAL', 'CONF', 'DARKNESS', 'S_MONSTER', 'S_SPIDER', 'MISSILE']
	description = """A powerful dark elf, with mighty nature-controlling enchantments.
"""

monsters.register_race(dark_elven_druid)


class stone_troll(troll):
	r_idx = "401"
	name = "Stone troll"
	ascii = "T,D"
	level = 25
	rarity = 1
	hp = "23d7"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 50
	exp = 85
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('BITE', 'HURT', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'WILD_MOUNT2', 'FRIENDS', 'DROP_60', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'HURT_LITE', 'HURT_ROCK']
	sfreq = 0
	sflags = []
	description = """He is a giant troll with scabrous black skin.
"""

monsters.register_race(stone_troll)


class black(monster):
	r_idx = "402"
	name = "Black"
	ascii = "j,d"
	level = 25
	rarity = 3
	hp = "11d14"
	speed = 111
	aaf = 12
	ac = 32
	sleep = 30
	exp = 50
	blows = [('ENGULF', 'EXP_VAMP', '2d6'), ('ENGULF', 'EXP_VAMP', '2d6')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'IM_COLD', 'IM_POIS', 'IM_ACID', 'RES_NETH', 'EVIL', 'EMPTY_MIND', 'COLD_BLOOD', 'FRIENDS']
	sfreq = 0
	sflags = []
	description = """The eldritch blood of Yibb-Tstll is know only as "the Black"an amorphous substance, who will suck your life and deliver it to 
Yibb-Tstll.
"""

monsters.register_race(black)


class troll_priest(troll, spellcaster):
	r_idx = "403"
	name = "Troll priest"
	ascii = "T,b"
	level = 25
	rarity = 1
	hp = "21d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 30
	exp = 100
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8'), ('BITE', 'HURT', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'WILD_MOUNT2', 'WILD_WASTE2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_90', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BLINK', 'SCARE', 'CAUSE_1', 'MISSILE', 'DARKNESS']
	description = """A troll who is so bright he knows how to read.
"""

monsters.register_race(troll_priest)


class wereworm(monster):
	r_idx = "404"
	name = "Wereworm"
	ascii = "w,u"
	level = 25
	rarity = 3
	hp = "70d11"
	speed = 110
	aaf = 15
	ac = 35
	sleep = 20
	exp = 300
	blows = [('GAZE', 'EXP_20', ''), ('CRAWL', 'ACID', '2d4'), ('BITE', 'HURT', '1d10'), ('BITE', 'POISON', '1d6')]
	flags = ['BASH_DOOR', 'EVIL', 'CAN_SWIM', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'ANIMAL', 'IM_ACID', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A huge wormlike shape dripping acid, twisted by evil sorcery into a foul 
monster that breeds on death.
"""

monsters.register_race(wereworm)


class killer_crimson_beetle(monster):
	r_idx = "405"
	name = "Killer crimson beetle"
	ascii = "K,R"
	level = 25
	rarity = 2
	hp = "14d8"
	speed = 110
	aaf = 14
	ac = 25
	sleep = 30
	exp = 85
	blows = [('BITE', 'LOSE_STR', '4d4')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WEIRD_MIND', 'BASH_DOOR', 'DROP_CORPSE', 'ANIMAL', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """A giant beetle with poisonous mandibles.
"""

monsters.register_race(killer_crimson_beetle)


class vampiric_ixitxachitl(monster, spellcaster):
	r_idx = "406"
	name = "Vampiric ixitxachitl"
	ascii = "l,D"
	level = 26
	rarity = 1
	hp = "17d22"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 20
	exp = 120
	blows = [('STING', 'POISON', '2d8'), ('STING', 'EXP_VAMP', '2d8')]
	flags = ['ANIMAL', 'EVIL', 'AQUATIC', 'RES_NETH', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_SHORE']
	sfreq = 6
	sflags = ['HEAL', 'SCARE', 'CAUSE_3', 'BLIND', 'FORGET', 'HASTE']
	description = """A devil ray of the depths, with vampiric powers.
"""

monsters.register_race(vampiric_ixitxachitl)


class gnoph_keh(monster, spellcaster):
	r_idx = "407"
	name = "Gnoph-Keh"
	ascii = "q,s"
	level = 26
	rarity = 2
	hp = "15d15"
	speed = 110
	aaf = 12
	ac = 25
	sleep = 25
	exp = 95
	blows = [('CLAW', 'COLD', '2d4'), ('CLAW', 'COLD', '2d4'), ('BUTT', 'HURT', '2d9')]
	flags = ['RAND_25', 'DROP_90', 'DROP_60', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'AURA_COLD', 'IM_COLD', 'ANIMAL', 'DROP_CORPSE', 'EVIL']
	sfreq = 8
	sflags = ['BR_COLD', 'BO_ICEE', 'BO_COLD', 'BA_COLD']
	description = """A creature with a sharp hornGreenland ice, that walked sometimes on two legs, sometimes 
on four, and sometimes on six."
"""

monsters.register_race(gnoph_keh)


class giant_grey_ant(monster):
	r_idx = "408"
	name = "Giant grey ant"
	ascii = "a,s"
	level = 26
	rarity = 1
	hp = "20d5"
	speed = 110
	aaf = 10
	ac = 20
	sleep = 40
	exp = 90
	blows = [('BITE', 'HURT', '2d12')]
	flags = ['WILD_DUNGEON_01', 'WILD_MOUNT2', 'RAND_25', 'KILL_BODY', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is an ant encased in shaggy grey fur.
"""

monsters.register_race(giant_grey_ant)


class kharis_the_powerslave(monster, undead, spellcaster):
	r_idx = "409"
	name = "Kharis the Powerslave"
	ascii = "z,u"
	level = 26
	rarity = 4
	hp = "64d11"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 40
	exp = 500
	blows = [('GAZE', 'TERRIFY', ''), ('HIT', 'DISEASE', '6d6'), ('CLAW', 'LOSE_CON', ''), ('CLAW', 'LOSE_CON', '')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'UNDEAD', 'EVIL', 'ESCORTS', 'ESCORT', 'FORCE_MAXHP', 'COLD_BLOOD', 'IM_POIS', 'IM_COLD', 'NO_FEAR', 'ONLY_ITEM', 'DROP_90', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'RES_TELE']
	sfreq = 5
	sflags = ['TRAPS', 'CAUSE_3', 'DARKNESS', 'S_UNDEAD', 'SCARE', 'SLOW', 'S_KIN', 'ANIM_DEAD']
	description = """He is out to have a revenge on you who have desecrated his tomb.
"""

monsters.register_race(kharis_the_powerslave)


class gwaihir_the_windlord(monster):
	r_idx = "410"
	name = "Gwaihir the Windlord"
	ascii = "B,u"
	level = 24
	rarity = 5
	hp = "64d10"
	speed = 130
	aaf = 20
	ac = 65
	sleep = 20
	exp = 360
	blows = [('CLAW', 'HURT', '15d2'), ('CLAW', 'HURT', '15d2'), ('BITE', 'HURT', '3d10')]
	flags = ['CAN_FLY', 'UNIQUE', 'FORCE_MAXHP', 'DROP_CORPSE', 'WILD_MOUNT2', 'WILD_WASTE2', 'FRIENDLY', 'ANIMAL', 'GOOD']
	sfreq = 0
	sflags = []
	description = """An agent of supernatural beings, this creature looks like a 
huge eagle.
"""

monsters.register_race(gwaihir_the_windlord)


class giant_red_tick(monster):
	r_idx = "411"
	name = "Giant red tick"
	ascii = "S,r"
	level = 26
	rarity = 1
	hp = "18d5"
	speed = 110
	aaf = 14
	ac = 27
	sleep = 20
	exp = 90
	blows = [('BITE', 'FIRE', '3d6')]
	flags = ['RAND_25', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_MOUNT2', 'ANIMAL', 'IM_FIRE', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """It is smoking and burning with great heat.
"""

monsters.register_race(giant_red_tick)


class displacer_beast(monster):
	r_idx = "412"
	name = "Displacer beast"
	ascii = "f,b"
	level = 26
	rarity = 2
	hp = "25d7"
	speed = 110
	aaf = 35
	ac = 50
	sleep = 20
	exp = 100
	blows = [('BITE', 'HURT', '2d8'), ('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10')]
	flags = ['INVISIBLE', 'BASH_DOOR', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is a huge black panther, clubbed tentacles sprouting from its shoulders.
"""

monsters.register_race(displacer_beast)


class ulwarth_son_of_ulfang(monster):
	r_idx = "413"
	name = "Ulwarth, Son of Ulfang"
	ascii = "p,U"
	level = 26
	rarity = 4
	hp = "85d7"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 40
	exp = 500
	blows = [('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_GRASS', 'ONLY_ITEM', 'DROP_90', 'DROP_GOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A short and swarthy Easterling.
"""

monsters.register_race(ulwarth_son_of_ulfang)


class agent_of_benedict(monster, spellcaster):
	r_idx = "414"
	name = "Agent of Benedict"
	ascii = "p,W"
	level = 26
	rarity = 3
	hp = "14d8"
	speed = 110
	aaf = 14
	ac = 27
	sleep = 20
	exp = 100
	blows = [('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d9')]
	flags = ['FRIENDS', 'MALE', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'OPEN_DOOR', 'WILD_DUNGEON_01']
	sfreq = 8
	sflags = ['BLIND', 'CONF', 'TPORT']
	description = """A faithful servitor and a skilled warrior.
"""

monsters.register_race(agent_of_benedict)


class cave_ogre(giant):
	r_idx = "415"
	name = "Cave ogre"
	ascii = "O,u"
	level = 26
	rarity = 1
	hp = "31d6"
	speed = 110
	aaf = 20
	ac = 17
	sleep = 30
	exp = 42
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8')]
	flags = ['FRIENDS', 'DROP_60', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT']
	sfreq = 0
	sflags = []
	description = """A giant orc-like figure with an awesomely muscled frame.
"""

monsters.register_race(cave_ogre)


class white_wraith(monster, undead, spellcaster):
	r_idx = "416"
	name = "White wraith"
	ascii = "W,w"
	level = 26
	rarity = 1
	hp = "16d5"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 10
	exp = 175
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('TOUCH', 'EXP_20', '')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'DROP_1D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['SCARE', 'CAUSE_2', 'DARKNESS']
	description = """It is a tangible but ghostly form made of white fog.
"""

monsters.register_race(white_wraith)


class angel(monster, spellcaster):
	r_idx = "417"
	name = "Angel"
	ascii = "A,b"
	level = 26
	rarity = 6
	hp = "23d10"
	speed = 110
	aaf = 30
	ac = 30
	sleep = 255
	exp = 220
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'NO_FEAR', 'ONLY_ITEM', 'DROP_2D2', 'GOOD', 'CAN_FLY', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'IM_ACID', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'CONF', 'SCARE', 'FORGET']
	description = """A lesser angel wearing little more than a loincloth - its steely skin 
provides all the protection it needs.
"""

monsters.register_race(angel)


class ghoul(monster, undead, spellcaster):
	r_idx = "418"
	name = "Ghoul"
	ascii = "z,U"
	level = 26
	rarity = 1
	hp = "11d9"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 20
	exp = 95
	blows = [('CLAW', 'DISEASE', '1d4'), ('CLAW', 'DISEASE', '1d4'), ('BITE', 'PARALYZE', '1d5')]
	flags = ['WILD_DUNGEON_01', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'EVIL', 'UNDEAD', 'FRIENDS', 'IM_POIS', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'COLD_BLOOD', 'HURT_LITE']
	sfreq = 9
	sflags = ['SCARE', 'HOLD']
	description = """Flesh is falling off in chunks from this decaying abomination.
"""

monsters.register_race(ghoul)


class alberich_the_nibelung_king(monster, spellcaster):
	r_idx = "419"
	name = "Alberich the Nibelung King"
	ascii = "h,D"
	level = 27
	rarity = 4
	hp = "56d11"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 20
	exp = 1000
	blows = [('HIT', 'UN_BONUS', '3d12'), ('HIT', 'UN_BONUS', '3d12'), ('TOUCH', 'EAT_GOLD', ''), ('TOUCH', 'EAT_GOLD', '')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'FORCE_MAXHP', 'FORCE_SLEEP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'INVISIBLE', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'RES_DISE', 'RES_TELE']
	sfreq = 6
	sflags = ['HEAL', 'SCARE', 'BO_ACID', 'BA_ACID', 'TPORT', 'S_MONSTER']
	description = """Made invisible with his magic cap, the greedy dwarf plots for 
world domination through his riches.
"""

monsters.register_race(alberich_the_nibelung_king)


class hellblade(monster):
	r_idx = "420"
	name = "Hellblade"
	ascii = "|,v"
	level = 27
	rarity = 2
	hp = "10d13"
	speed = 120
	aaf = 20
	ac = 20
	sleep = 20
	exp = 130
	blows = [('HIT', 'EXP_20', '2d13'), ('HIT', 'EXP_20', '2d13')]
	flags = ['WILD_DUNGEON_01', 'CHAR_MULTI', 'EVIL', 'IM_POIS', 'IM_COLD', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'COLD_BLOOD', 'BASH_DOOR', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A deadly blade of chaos, moving of its own volition.
"""

monsters.register_race(hellblade)


class killer_red_beetle(monster):
	r_idx = "421"
	name = "Killer red beetle"
	ascii = "K,r"
	level = 27
	rarity = 1
	hp = "14d5"
	speed = 110
	aaf = 14
	ac = 25
	sleep = 30
	exp = 95
	blows = [('BITE', 'HURT', '3d4'), ('SPIT', 'FIRE', '4d5')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_CORPSE', 'ANIMAL', 'IM_FIRE', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """It is a giant beetle wreathed in flames.
"""

monsters.register_race(killer_red_beetle)


class beast_of_nurgle(demon):
	r_idx = "422"
	name = "Beast of Nurgle"
	ascii = "q,o"
	level = 27
	rarity = 2
	hp = "15d7"
	speed = 110
	aaf = 14
	ac = 25
	sleep = 30
	exp = 100
	blows = [('TOUCH', 'POISON', '3d3'), ('TOUCH', 'DISEASE', '3d3'), ('BITE', 'ACID', '4d5')]
	flags = ['WILD_DUNGEON_01', 'WEIRD_MIND', 'BASH_DOOR', 'IM_ACID', 'IM_POIS', 'DEMON', 'EVIL', 'IM_FIRE', 'CAN_SWIM']
	sfreq = 0
	sflags = []
	description = """It walks on four legs, but mostly resembles a slug-shaped jelly. 
It even has two disgusting antennae in its head. Yecch!
"""

monsters.register_race(beast_of_nurgle)


class creeping_adamantite_coins(monster):
	r_idx = "423"
	name = "Creeping adamantite coins"
	ascii = "$,v"
	level = 27
	rarity = 4
	hp = "14d25"
	speed = 120
	aaf = 5
	ac = 25
	sleep = 10
	exp = 45
	blows = [('BITE', 'POISON', '3d4'), ('TOUCH', 'POISON', '3d5'), ('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12')]
	flags = ['WILD_DUNGEON_01', 'ONLY_GOLD', 'DROP_90', 'DROP_2D2', 'COLD_BLOOD', 'BASH_DOOR', 'ANIMAL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """It is a pile of coins, slithering forward on thousands of tiny legs.
"""

monsters.register_race(creeping_adamantite_coins)


class algroth(troll):
	r_idx = "424"
	name = "Algroth"
	ascii = "T,s"
	level = 27
	rarity = 1
	hp = "15d12"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 40
	exp = 150
	blows = [('CLAW', 'POISON', '3d3'), ('CLAW', 'POISON', '3d3'), ('BITE', 'HURT', '1d6')]
	flags = ['FRIENDS', 'DROP_60', 'WILD_MOUNT2', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'TROLL']
	sfreq = 0
	sflags = []
	description = """A powerful troll form.  Venom drips from its needlelike claws.
"""

monsters.register_race(algroth)


class flamer_of_tzeentch(demon, spellcaster):
	r_idx = "425"
	name = "Flamer of Tzeentch"
	ascii = ",,R"
	level = 27
	rarity = 2
	hp = "45d15"
	speed = 110
	aaf = 10
	ac = 35
	sleep = 20
	exp = 500
	blows = [('ENGULF', 'FIRE', '3d6'), ('ENGULF', 'FIRE', '3d6')]
	flags = ['FORCE_SLEEP', 'BASH_DOOR', 'WILD_DUNGEON_01', 'IM_FIRE', 'IM_POIS', 'AURA_FIRE', 'DEMON', 'EVIL', 'RES_PLAS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 3
	sflags = ['BO_FIRE', 'BA_FIRE']
	description = """It looks like an inverted mushroom, with two flexible arms.
"""

monsters.register_race(flamer_of_tzeentch)


class roper(monster, spellcaster):
	r_idx = "426"
	name = "Roper"
	ascii = "#,D"
	level = 27
	rarity = 5
	hp = "70d9"
	speed = 115
	aaf = 30
	ac = 30
	sleep = 255
	exp = 235
	blows = [('CRUSH', 'PARALYZE', '3d5'), ('CRUSH', 'PARALYZE', '3d5'), ('CRUSH', 'PARALYZE', '3d5'), ('CRUSH', 'PARALYZE', '3d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'NO_FEAR', 'NEVER_MOVE', 'ONLY_GOLD', 'DROP_2D2', 'DROP_60', 'DROP_1D2', 'EVIL', 'IM_ACID', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'IM_COLD', 'IM_FIRE']
	sfreq = 5
	sflags = ['BA_FIRE', 'BA_ELEC', 'BA_POIS', 'HASTE', 'TRAPS', 'SHRIEK', 'HOLD', 'CONF']
	description = """This creature look like a pillar of rock. However, a closer 
inspection reveals a glaring eye and powerful tentacles, 
which crush its prey and feed it to the creature's hungry 
mouth.
"""

monsters.register_race(roper)


class headless(monster, spellcaster):
	r_idx = "427"
	name = "Headless"
	ascii = "H,u"
	level = 27
	rarity = 1
	hp = "19d17"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 40
	exp = 175
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8')]
	flags = ['FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_WASTE1', 'WILD_WASTE2', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL']
	sfreq = 6
	sflags = ['SCARE']
	description = """Headless humanoid abominations created by a magical mutation.
"""

monsters.register_race(headless)


class vibration_hound(monster, spellcaster):
	r_idx = "428"
	name = "Vibration hound"
	ascii = "Z,U"
	level = 27
	rarity = 3
	hp = "25d7"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 250
	blows = [('BITE', 'HURT', '2d6'), ('BITE', 'HURT', '2d6'), ('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'FRIENDS', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 5
	sflags = ['BR_SOUN']
	description = """A blurry canine form which seems to be moving as fast as the eye can 
follow.  You can feel the earth resonating beneath your feet.
"""

monsters.register_race(vibration_hound)


class nexus_hound(monster, spellcaster):
	r_idx = "429"
	name = "Nexus hound"
	ascii = "Z,v"
	level = 27
	rarity = 3
	hp = "25d7"
	speed = 110
	aaf = 30
	ac = 15
	sleep = 0
	exp = 250
	blows = [('BITE', 'HURT', '2d8'), ('BITE', 'HURT', '2d8'), ('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'RES_NEXU', 'DROP_SKELETON', 'DROP_CORPSE', 'FRIENDS', 'WILD_DUNGEON_01', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BR_NEXU']
	description = """A locus of conflicting points coalesce to form the vague shape of a huge 
hound.  Or is it just your imagination?
"""

monsters.register_race(nexus_hound)


class ogre_mage(giant, spellcaster):
	r_idx = "430"
	name = "Ogre mage"
	ascii = "O,o"
	level = 27
	rarity = 2
	hp = "21d12"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 30
	exp = 300
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8')]
	flags = ['FORCE_SLEEP', 'DROP_1D2', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_MOUNT2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'GIANT']
	sfreq = 4
	sflags = ['HEAL', 'HOLD', 'TRAPS', 'BA_COLD', 'S_MONSTER']
	description = """A hideous ogre wrapped in black sorcerous robes.
"""

monsters.register_race(ogre_mage)


class grendel(giant):
	r_idx = "431"
	name = "Grendel"
	ascii = "O,g"
	level = 27
	rarity = 2
	hp = "15d70"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 20
	exp = 1500
	blows = [('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SHORE', 'FORCE_MAXHP', 'ESCORT', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'EVIL', 'GIANT', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """An ogre renowned for acts of surpassing cruelty.
"""

monsters.register_race(grendel)


class vampire(monster, undead, spellcaster):
	r_idx = "432"
	name = "Vampire"
	ascii = "V,W"
	level = 27
	rarity = 1
	hp = "18d12"
	speed = 110
	aaf = 20
	ac = 22
	sleep = 10
	exp = 175
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('BITE', 'EXP_VAMP', '1d8')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'CAN_FLY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['TELE_TO', 'HOLD', 'SCARE', 'CAUSE_2', 'MIND_BLAST', 'FORGET', 'DARKNESS']
	description = """It is a humanoid with an aura of power.  You notice a sharp set of front 
teeth.
"""

monsters.register_race(vampire)


class gorgimera(monster, spellcaster):
	r_idx = "433"
	name = "Gorgimera"
	ascii = "H,D"
	level = 27
	rarity = 2
	hp = "25d14"
	speed = 110
	aaf = 12
	ac = 27
	sleep = 10
	exp = 200
	blows = [('BITE', 'FIRE', '1d3'), ('BITE', 'FIRE', '1d3'), ('BITE', 'HURT', '1d10'), ('GAZE', 'PARALYZE', '2d4')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'BASH_DOOR', 'CAN_FLY', 'DROP_CORPSE', 'IM_FIRE']
	sfreq = 8
	sflags = ['BR_FIRE']
	description = """It has 3 heads - gorgon, goat and dragon - all attached to a 
lion's body.
"""

monsters.register_race(gorgimera)


class shantak(monster, spellcaster):
	r_idx = "434"
	name = "Shantak"
	ascii = "B,D"
	level = 27
	rarity = 2
	hp = "25d14"
	speed = 120
	aaf = 12
	ac = 27
	sleep = 10
	exp = 280
	blows = [('BITE', 'HURT', '1d6'), ('BITE', 'HURT', '1d6'), ('BITE', 'HURT', '1d6')]
	flags = ['WILD_DUNGEON_01', 'ANIMAL', 'IM_ACID', 'EVIL', 'ELDRITCH_HORROR', 'CAN_FLY', 'DROP_CORPSE']
	sfreq = 6
	sflags = ['SCARE', 'HASTE']
	description = """A scaly bird larger than an elephants, with a horse-like head.
"""

monsters.register_race(shantak)


class colbran(monster, spellcaster):
	r_idx = "435"
	name = "Colbran"
	ascii = "g,b"
	level = 27
	rarity = 2
	hp = "56d12"
	speed = 120
	aaf = 12
	ac = 40
	sleep = 10
	exp = 900
	blows = [('HIT', 'ELEC', '3d8'), ('HIT', 'ELEC', '3d8')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'CAN_FLY', 'IM_ELEC', 'IM_POIS', 'AURA_ELEC', 'REFLECTING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 3
	sflags = ['BO_ELEC']
	description = """A man-shaped form of living lightning, sparks and shocks crackle all over 
this madly capering figure, as it leaps and whirls around and about you.
"""

monsters.register_race(colbran)


class spirit_naga(monster, spellcaster):
	r_idx = "436"
	name = "Spirit naga"
	ascii = "n,B"
	level = 28
	rarity = 2
	hp = "21d15"
	speed = 110
	aaf = 20
	ac = 37
	sleep = 120
	exp = 60
	blows = [('CRUSH', 'HURT', '2d8'), ('CRUSH', 'HURT', '2d8'), ('BITE', 'HURT', '1d8'), ('BITE', 'HURT', '1d8')]
	flags = ['FEMALE', 'FORCE_SLEEP', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_90', 'DROP_2D2', 'DROP_CORPSE', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['HEAL', 'BLIND', 'MIND_BLAST', 'DARKNESS']
	description = """A wraithly snake-like form with the torso of a beautiful woman, it is the 
most powerful of its kind.
"""

monsters.register_race(spirit_naga)


class corpser(monster):
	r_idx = "437"
	name = "Corpser"
	ascii = ",,g"
	level = 28
	rarity = 2
	hp = "23d15"
	speed = 112
	aaf = 20
	ac = 37
	sleep = 120
	exp = 65
	blows = [('CRUSH', 'HURT', '2d8'), ('CRUSH', 'HURT', '2d8'), ('CRUSH', 'HURT', '2d8'), ('CRUSH', 'HURT', '2d8')]
	flags = ['WILD_DUNGEON_01', 'WILD_GRASS', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A creature who lives underground and eats surface 
dwellers whom it catches 
with its poisonous tentacle, which is the only part of the creature tha 
is ever seen. Perhaps it is better not to see the part which remains 
underground...
"""

monsters.register_race(corpser)


class fiend_of_slaanesh(demon):
	r_idx = "438"
	name = "Fiend of Slaanesh"
	ascii = "S,R"
	level = 28
	rarity = 4
	hp = "25d14"
	speed = 120
	aaf = 12
	ac = 25
	sleep = 40
	exp = 225
	blows = [('STING', 'POISON', '8d1'), ('STING', 'LOSE_STR', '8d1'), ('WAIL', 'TERRIFY', '1d4')]
	flags = ['BASH_DOOR', 'WILD_DUNGEON_01', 'EVIL', 'DEMON']
	sfreq = 0
	sflags = []
	description = """Slaanesh's pet, a large scorpion-like creature with a human face and 
reptile skin.
"""

monsters.register_race(fiend_of_slaanesh)


class stairway_to_hell(monster, undead, spellcaster):
	r_idx = "439"
	name = "stairway to hell"
	ascii = ">,W"
	level = 28
	rarity = 5
	hp = "11d8"
	speed = 120
	aaf = 90
	ac = 20
	sleep = 20
	exp = 125
	blows = [('WAIL', 'UN_BONUS', ''), ('WAIL', 'EXP_20', ''), ('WAIL', 'EAT_GOLD', ''), ('WAIL', 'EAT_ITEM', '')]
	flags = ['WILD_DUNGEON_01', 'CHAR_MULTI', 'COLD_BLOOD', 'EVIL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING', 'UNDEAD', 'FORCE_MAXHP', 'IM_FIRE', 'IM_ELEC', 'IM_POIS', 'IM_ACID', 'EMPTY_MIND', 'NEVER_MOVE']
	sfreq = 15
	sflags = ['S_DEMON', 'SHRIEK']
	description = """Often found in graveyards.
"""

monsters.register_race(stairway_to_hell)


class five_headed_hydra(monster, spellcaster):
	r_idx = "440"
	name = "5-headed hydra"
	ascii = "M,g"
	level = 28
	rarity = 2
	hp = "70d8"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 20
	exp = 350
	blows = [('BITE', 'POISON', '4d4'), ('BITE', 'POISON', '4d4'), ('BITE', 'POISON', '4d4'), ('BITE', 'POISON', '4d4')]
	flags = ['FORCE_SLEEP', 'ONLY_GOLD', 'DROP_1D2', 'DROP_4D2', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'MOVE_BODY', 'CAN_SWIM', 'ANIMAL', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_SWAMP2']
	sfreq = 5
	sflags = ['SCARE', 'BA_POIS']
	description = """A strange reptilian hybrid with five heads dripping venom.
"""

monsters.register_race(five_headed_hydra)


class barney_the_dinosaur(monster, spellcaster):
	r_idx = "441"
	name = "Barney the Dinosaur"
	ascii = "R,v"
	level = 29
	rarity = 2
	hp = "83d9"
	speed = 120
	aaf = 10
	ac = 45
	sleep = 20
	exp = 500
	blows = [('SHOW', 'LOSE_INT', ''), ('SHOW', 'LOSE_WIS', ''), ('CHARGE', 'EAT_GOLD', ''), ('CHARGE', 'EAT_GOLD', '')]
	flags = ['WILD_DUNGEON_01', 'DROP_1D2', 'DROP_90', 'DROP_GOOD', 'DROP_GREAT', 'ONLY_ITEM', 'EVIL', 'MALE', 'CAN_SPEAK', 'SMART', 'RES_TELE', 'CAN_SWIM', 'DROP_CORPSE', 'ANIMAL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'FORCE_MAXHP', 'UNIQUE', 'FORCE_SLEEP']
	sfreq = 3
	sflags = ['SHRIEK', 'CONF', 'S_HYDRA', 'SLOW', 'BLIND', 'DRAIN_MANA', 'BA_POIS', 'BR_CHAO', 'FORGET', 'DARKNESS', 'BR_NUKE']
	description = """The lovable purple reptile is making a guest appearance here.
"""

monsters.register_race(barney_the_dinosaur)


class black_knight(monster, spellcaster):
	r_idx = "442"
	name = "Black knight"
	ascii = "p,D"
	level = 28
	rarity = 1
	hp = "22d10"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 10
	exp = 240
	blows = [('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'FORCE_SLEEP', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL']
	sfreq = 8
	sflags = ['BLIND', 'SCARE', 'CAUSE_3', 'DARKNESS']
	description = """He is a figure encased in deep black plate armour; he looks at you 
menacingly.
"""

monsters.register_race(black_knight)


class seahorse(monster, spellcaster):
	r_idx = "443"
	name = "Seahorse"
	ascii = "l,o"
	level = 28
	rarity = 2
	hp = "83d11"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 20
	exp = 360
	blows = [('BITE', 'PARALYZE', '4d5'), ('BITE', 'LOSE_DEX', '4d5'), ('BITE', 'LOSE_CON', '4d5')]
	flags = ['FORCE_SLEEP', 'AQUATIC', 'GOOD', 'ANIMAL', 'IM_COLD', 'IM_POIS', 'IM_FIRE', 'IM_ELEC', 'WILD_DUNGEON_01', 'WILD_OCEAN']
	sfreq = 5
	sflags = ['BO_WATE', 'BO_COLD', 'BO_ICEE', 'BO_MANA']
	description = """Your mind is filled with admiration as you view this wondrous, 
magical seahorse.
"""

monsters.register_race(seahorse)


class cyclops(giant, spellcaster):
	r_idx = "444"
	name = "Cyclops"
	ascii = "P,u"
	level = 28
	rarity = 2
	hp = "50d12"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 20
	exp = 350
	blows = [('HIT', 'HURT', '9d8'), ('HIT', 'HURT', '9d8')]
	flags = ['FORCE_SLEEP', 'DROP_1D2', 'TAKE_ITEM', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_MOUNT1', 'BASH_DOOR', 'OPEN_DOOR', 'MOVE_BODY', 'DROP_CORPSE', 'EVIL', 'IM_POIS', 'IM_ACID', 'GIANT']
	sfreq = 8
	sflags = ['ARROW_4']
	description = """A gigantic humanoid with but one eye.
"""

monsters.register_race(cyclops)


class clairvoyant(monster, spellcaster):
	r_idx = "445"
	name = "Clairvoyant"
	ascii = "p,B"
	level = 28
	rarity = 3
	hp = "25d7"
	speed = 120
	aaf = 100
	ac = 25
	sleep = 10
	exp = 250
	blows = [('HIT', 'CONFUSE', '5d5'), ('HIT', 'TERRIFY', '5d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'DROP_1D2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'SMART', 'EVIL']
	sfreq = 8
	sflags = ['BLIND', 'MIND_BLAST', 'HOLD', 'CAUSE_3', 'FORGET', 'S_MONSTER']
	description = """He is using his supernatural talents to bring about your 
destruction.
"""

monsters.register_race(clairvoyant)


class giant_purple_worm(monster):
	r_idx = "446"
	name = "Giant purple worm"
	ascii = "w,v"
	level = 29
	rarity = 4
	hp = "67d5"
	speed = 110
	aaf = 14
	ac = 30
	sleep = 30
	exp = 400
	blows = [('HIT', 'HURT', '1d8'), ('BITE', 'ACID', '2d8'), ('STING', 'POISON', '1d8')]
	flags = ['BASH_DOOR', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'ANIMAL', 'IM_ACID', 'IM_POIS', 'CAN_SWIM']
	sfreq = 0
	sflags = []
	description = """It is a massive worm form, many feet in length.  Its vast maw drips acid 
and poison.
"""

monsters.register_race(giant_purple_worm)


class catoblepas(monster):
	r_idx = "447"
	name = "Catoblepas"
	ascii = "q,g"
	level = 29
	rarity = 2
	hp = "30d7"
	speed = 110
	aaf = 15
	ac = 26
	sleep = 40
	exp = 400
	blows = [('GAZE', 'TERRIFY', '2d4'), ('GAZE', 'BLIND', '2d4'), ('BUTT', 'HURT', '2d6'), ('BITE', 'HURT', '2d12')]
	flags = ['ONLY_GOLD', 'DROP_2D2', 'DROP_CORPSE', 'EVIL', 'BASH_DOOR', 'CAN_SWIM', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A strange ox-like form with a huge head but a thin, weak neck, it looks 
like the creation of some deranged alchemist.
"""

monsters.register_race(catoblepas)


class lesser_wall_monster(monster):
	r_idx = "448"
	name = "Lesser wall monster"
	ascii = "#,W"
	level = 28
	rarity = 4
	hp = "9d8"
	speed = 110
	aaf = 20
	ac = 37
	sleep = 40
	exp = 600
	blows = [('HIT', 'HURT', '2d6'), ('HIT', 'HURT', '2d6'), ('HIT', 'HURT', '2d6')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'COLD_BLOOD', 'EMPTY_MIND', 'KILL_WALL', 'RAND_50', 'MULTIPLY', 'BASH_DOOR', 'IM_COLD', 'IM_ACID', 'IM_FIRE', 'IM_ELEC', 'IM_POIS', 'NONLIVING', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP', 'CHAR_MULTI']
	sfreq = 0
	sflags = []
	description = """A sentient, moving section of wall.
"""

monsters.register_race(lesser_wall_monster)


class mage(monster, spellcaster):
	r_idx = "449"
	name = "Mage"
	ascii = "p,r"
	level = 28
	rarity = 1
	hp = "15d5"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 10
	exp = 150
	blows = [('HIT', 'HURT', '2d5'), ('HIT', 'HURT', '2d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'FORCE_SLEEP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_SKELETON', 'DROP_CORPSE', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 3
	sflags = ['HASTE', 'TPORT', 'TELE_TO', 'BLIND', 'CONF', 'BO_FIRE', 'BO_COLD', 'BO_ELEC', 'S_MONSTER']
	description = """A fat mage with glasses.
"""

monsters.register_race(mage)


class mind_flayer(monster, spellcaster):
	r_idx = "450"
	name = "Mind flayer"
	ascii = "h,v"
	level = 28
	rarity = 1
	hp = "15d7"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 10
	exp = 200
	blows = [('GAZE', 'LOSE_INT', '2d6'), ('GAZE', 'LOSE_INT', '2d6')]
	flags = ['WILD_DUNGEON_01', 'WILD_WASTE1', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['BLIND', 'HOLD', 'SCARE', 'MIND_BLAST', 'BRAIN_SMASH', 'FORGET']
	description = """A humanoid form with a gruesome head, tentacular mouth, and piercing 
eyes.  Claws reach out for you and you feel a presence invade your mind.
"""

monsters.register_race(mind_flayer)


class the_ultimate_dungeon_cleaner(monster):
	r_idx = "451"
	name = "The Ultimate Dungeon Cleaner"
	ascii = "g,D"
	level = 28
	rarity = 2
	hp = "70d9"
	speed = 120
	aaf = 10
	ac = 40
	sleep = 12
	exp = 555
	blows = [('CRUSH', 'HURT', '2d10'), ('CRUSH', 'HURT', '2d10'), ('CRUSH', 'HURT', '2d10'), ('CRUSH', 'HURT', '2d10')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'KILL_BODY', 'KILL_ITEM', 'UNIQUE', 'REFLECTING', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'RES_TELE']
	sfreq = 0
	sflags = []
	description = """It looks like a huge spiked roller. It was designed to keep this dungeon 
clean, and you are the biggest spot of dirt in sight.
"""

monsters.register_race(the_ultimate_dungeon_cleaner)


class deep_one(demon):
	r_idx = "452"
	name = "Deep one"
	ascii = "u,g"
	level = 28
	rarity = 5
	hp = "26d12"
	speed = 120
	aaf = 20
	ac = 25
	sleep = 20
	exp = 150
	blows = [('CLAW', 'POISON', '2d4'), ('CLAW', 'POISON', '2d4'), ('BITE', 'HURT', '4d4')]
	flags = ['FRIENDS', 'WILD_DUNGEON_01', 'WILD_SHORE', 'DROP_CORPSE', 'RAND_25', 'DROP_60', 'DROP_90', 'CAN_SWIM', 'BASH_DOOR', 'ELDRITCH_HORROR', 'RES_TELE', 'EVIL', 'DEMON', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'RES_WATE']
	sfreq = 0
	sflags = []
	description = """"I thought their predominant color was a greyish-green, 
though they had white bellies. They were mostly shiny and 
slippery, but the ridges of their backs were scaly. Their 
forms vaguely suggested the anthropoid, while their heads were 
the heads of fish, with prodigious bulging eyes that never 
closed. At the sides of their necks were palpitating gills and 
their long paws were webbed."
"""

monsters.register_race(deep_one)


class basilisk(monster, spellcaster):
	r_idx = "453"
	name = "Basilisk"
	ascii = "R,D"
	level = 28
	rarity = 3
	hp = "14d30"
	speed = 120
	aaf = 15
	ac = 45
	sleep = 30
	exp = 350
	blows = [('GAZE', 'PARALYZE', ''), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12')]
	flags = ['ONLY_ITEM', 'DROP_1D2', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_MOUNT2', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_POIS', 'DROP_CORPSE', 'ANIMAL', 'NO_CONF', 'NO_SLEEP', 'CAN_SWIM']
	sfreq = 8
	sflags = ['BR_POIS']
	description = """An evil reptile whose eyes stare deeply at you and 
makes your soul wilt!
"""

monsters.register_race(basilisk)


class ice_troll(troll):
	r_idx = "454"
	name = "Ice troll"
	ascii = "T,w"
	level = 28
	rarity = 1
	hp = "28d7"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 50
	exp = 160
	blows = [('HIT', 'HURT', '1d5'), ('HIT', 'HURT', '1d5'), ('HIT', 'HURT', '1d5'), ('BITE', 'COLD', '3d6')]
	flags = ['MALE', 'FRIENDS', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'IM_COLD', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is a white troll with powerfully clawed hands.
"""

monsters.register_race(ice_troll)


class dhole(monster, spellcaster):
	r_idx = "455"
	name = "Dhole"
	ascii = "w,s"
	level = 29
	rarity = 4
	hp = "49d12"
	speed = 110
	aaf = 14
	ac = 64
	sleep = 25
	exp = 555
	blows = [('SPIT', 'ACID', '1d8'), ('ENGULF', 'ACID', '2d8'), ('CRUSH', 'HURT', '4d8')]
	flags = ['WILD_DUNGEON_01', 'ANIMAL', 'EVIL', 'KILL_WALL', 'IM_ACID', 'ELDRITCH_HORROR', 'CAN_SWIM', 'FORCE_MAXHP', 'DROP_CORPSE']
	sfreq = 6
	sflags = ['BR_ACID']
	description = """A gigantic worm dripping with acid. "...even as he looked, one 
reared up several hundred feet and leveled a bleached, viscous end 
at him."
"""

monsters.register_race(dhole)


class archangel(monster, spellcaster):
	r_idx = "456"
	name = "Archangel"
	ascii = "A,B"
	level = 29
	rarity = 6
	hp = "40d7"
	speed = 110
	aaf = 30
	ac = 34
	sleep = 255
	exp = 400
	blows = [('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'NO_FEAR', 'GOOD', 'ONLY_ITEM', 'DROP_2D2', 'CAN_FLY', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 3
	sflags = ['HEAL', 'HASTE', 'BLIND', 'CONF', 'SCARE']
	description = """A lesser angel protected by an aura of holiness.  Its muscular form looks 
extremely powerful next to your own frail body.
"""

monsters.register_race(archangel)


class ring_mimic(monster, spellcaster):
	r_idx = "457"
	name = "Ring mimic"
	ascii = "=,w"
	level = 29
	rarity = 4
	hp = "7d35"
	speed = 120
	aaf = 30
	ac = 30
	sleep = 100
	exp = 200
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'POISON', '3d4'), ('HIT', 'POISON', '3d4'), ('HIT', 'POISON', '3d4')]
	flags = ['CHAR_MULTI', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'NEVER_MOVE', 'EMPTY_MIND', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 4
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_2', 'FORGET', 'BO_ACID', 'BO_FIRE', 'BO_COLD', 'BO_ELEC', 'S_MONSTER']
	description = """A strange creature that disguises itself as discarded objects to lure 
unsuspecting adventurers within reach of its venomous claws.
"""

monsters.register_race(ring_mimic)


class chaos_tile(monster, spellcaster):
	r_idx = "458"
	name = "Chaos tile"
	ascii = ".,v"
	level = 29
	rarity = 6
	hp = "2d6"
	speed = 120
	aaf = 30
	ac = 30
	sleep = 100
	exp = 200
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'CONFUSE', '3d4')]
	flags = ['WILD_DUNGEON_01', 'CHAR_MULTI', 'ATTR_MULTI', 'ATTR_ANY', 'FORCE_SLEEP', 'NEVER_MOVE', 'MULTIPLY', 'EMPTY_MIND', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 4
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_2', 'BLINK', 'S_MONSTER']
	description = """It is a floor tile corrupted by chaos.
"""

monsters.register_race(chaos_tile)


class young_blue_dragon(dragon, spellcaster):
	r_idx = "459"
	name = "Young blue dragon"
	ascii = "d,b"
	level = 29
	rarity = 1
	hp = "22d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 70
	exp = 300
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d4'), ('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'DROP_1D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_ELEC']
	sfreq = 11
	sflags = ['SCARE', 'BR_ELEC']
	description = """It has a form that legends are made of.  Its still-tender scales are a 
deep blue in hue.  Sparks crackle along its length.
"""

monsters.register_race(young_blue_dragon)


class young_white_dragon(dragon, spellcaster):
	r_idx = "460"
	name = "Young white dragon"
	ascii = "d,w"
	level = 29
	rarity = 1
	hp = "22d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 70
	exp = 275
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d4'), ('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'DROP_1D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_COLD']
	sfreq = 11
	sflags = ['SCARE', 'BR_COLD']
	description = """It has a form that legends are made of.  Its still-tender scales are a 
frosty white in hue.  Icy blasts of cold air come from it as it breathes.
"""

monsters.register_race(young_white_dragon)


class young_green_dragon(dragon, spellcaster):
	r_idx = "461"
	name = "Young green dragon"
	ascii = "d,g"
	level = 29
	rarity = 1
	hp = "20d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 70
	exp = 290
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d4'), ('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'DROP_1D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_POIS']
	sfreq = 11
	sflags = ['SCARE', 'BR_POIS']
	description = """It has a form that legends are made of.  Its still-tender scales are a 
deep green in hue.  Foul gas seeps through its scales.
"""

monsters.register_race(young_green_dragon)


class young_bronze_dragon(dragon, spellcaster):
	r_idx = "462"
	name = "Young bronze dragon"
	ascii = "d,U"
	level = 29
	rarity = 3
	hp = "25d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 150
	exp = 310
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d4'), ('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'DROP_2D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 11
	sflags = ['SCARE', 'BR_CONF']
	description = """It has a form that legends are made of.  Its still-tender scales are a 
rich bronze hue, and its shape masks its true form.
"""

monsters.register_race(young_bronze_dragon)


class aklash(troll, spellcaster):
	r_idx = "463"
	name = "Aklash"
	ascii = "T,W"
	level = 29
	rarity = 4
	hp = "23d9"
	speed = 110
	aaf = 14
	ac = 32
	sleep = 25
	exp = 300
	blows = [('CLAW', 'HURT', '1d6'), ('CLAW', 'HURT', '1d6'), ('BITE', 'POISON', '1d20'), ('CRUSH', 'HURT', '2d9')]
	flags = ['WILD_DUNGEON_01', 'WILD_SWAMP2', 'TROLL', 'EVIL', 'FRIENDS', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'IM_POIS', 'IM_ACID', 'REGENERATE']
	sfreq = 7
	sflags = ['BR_POIS']
	description = """Pale, bald, fat, hairless troll creatures. Ugly beyond description. 
Not to mention how bad their breath smells...
"""

monsters.register_race(aklash)


class mithril_golem(monster):
	r_idx = "464"
	name = "Mithril golem"
	ascii = "g,B"
	level = 30
	rarity = 4
	hp = "56d15"
	speed = 110
	aaf = 12
	ac = 45
	sleep = 10
	exp = 500
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['ONLY_GOLD', 'DROP_2D2', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'REFLECTING', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """It is a massive statue of purest mithril.  It looks expensive!
"""

monsters.register_race(mithril_golem)


class skeleton_troll(troll, undead):
	r_idx = "465"
	name = "Skeleton troll"
	ascii = "s,w"
	level = 30
	rarity = 1
	hp = "20d7"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 20
	exp = 225
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('BITE', 'HURT', '3d4')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a troll skeleton animated by dark dweomers.
"""

monsters.register_race(skeleton_troll)


class skeletal_tyrannosaurus(monster, undead):
	r_idx = "466"
	name = "Skeletal tyrannosaurus"
	ascii = "R,w"
	level = 30
	rarity = 2
	hp = "50d7"
	speed = 120
	aaf = 20
	ac = 25
	sleep = 20
	exp = 225
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'LOSE_CON', '4d6'), ('GAZE', 'TERRIFY', '')]
	flags = ['WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'ANIMAL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is the skeleton of a tyrannosaurus, animated by dark dweomers.
"""

monsters.register_race(skeletal_tyrannosaurus)


class jaws(monster):
	r_idx = "467"
	name = "Jaws"
	ascii = "l,w"
	level = 30
	rarity = 2
	hp = "54d12"
	speed = 120
	aaf = 10
	ac = 40
	sleep = 12
	exp = 500
	blows = [('BITE', 'HURT', '11d2'), ('BITE', 'HURT', '22d1'), ('BITE', 'HURT', '11d2'), ('BITE', 'HURT', '22d1')]
	flags = ['FORCE_MAXHP', 'TAKE_ITEM', 'UNIQUE', 'MOVE_BODY', 'WILD_OCEAN', 'WILD_DUNGEON_01', 'BASH_DOOR', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'ANIMAL', 'AQUATIC', 'NO_CONF', 'NO_SLEEP', 'DROP_1D2']
	sfreq = 0
	sflags = []
	description = """The biggest white shark who has ever lived, it is hunting for you now! 
"""

monsters.register_race(jaws)


class thorondor(monster):
	r_idx = "468"
	name = "Thorondor"
	ascii = "B,u"
	level = 30
	rarity = 6
	hp = "64d12"
	speed = 130
	aaf = 20
	ac = 32
	sleep = 20
	exp = 555
	blows = [('CLAW', 'HURT', '16d2'), ('CLAW', 'HURT', '16d2'), ('BITE', 'HURT', '4d10')]
	flags = ['CAN_FLY', 'UNIQUE', 'FORCE_MAXHP', 'DROP_CORPSE', 'WILD_MOUNT2', 'WILD_WASTE2', 'FRIENDLY', 'ANIMAL', 'GOOD']
	sfreq = 0
	sflags = []
	description = """An agent of supernatural being, Thorondor is the lord of eagles.
"""

monsters.register_race(thorondor)


class giant_blue_ant(monster):
	r_idx = "469"
	name = "Giant blue ant"
	ascii = "a,b"
	level = 30
	rarity = 2
	hp = "5d9"
	speed = 110
	aaf = 10
	ac = 25
	sleep = 60
	exp = 80
	blows = [('BITE', 'ELEC', '5d5')]
	flags = ['RAND_25', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'ANIMAL', 'IM_ELEC']
	sfreq = 0
	sflags = []
	description = """It is a giant ant that crackles with energy.
"""

monsters.register_race(giant_blue_ant)


class grave_wight(monster, undead, spellcaster):
	r_idx = "470"
	name = "Grave wight"
	ascii = "W,b"
	level = 30
	rarity = 1
	hp = "12d7"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 30
	exp = 325
	blows = [('HIT', 'HURT', '1d7'), ('HIT', 'HURT', '1d7'), ('TOUCH', 'EXP_20', '')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'RAND_25', 'ONLY_ITEM', 'DROP_1D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'CAN_FLY', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['SCARE', 'CAUSE_3', 'DARKNESS']
	description = """It is a ghostly form with eyes that haunt you.
"""

monsters.register_race(grave_wight)


class shadow_drake(dragon, spellcaster):
	r_idx = "471"
	name = "Shadow drake"
	ascii = "d,D"
	level = 30
	rarity = 2
	hp = "35d7"
	speed = 110
	aaf = 25
	ac = 30
	sleep = 30
	exp = 700
	blows = [('BITE', 'COLD', '1d6'), ('BITE', 'COLD', '1d6'), ('BITE', 'COLD', '1d6')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'CAN_FLY', 'DROP_CORPSE', 'TAKE_ITEM', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'ANIMAL', 'EVIL', 'DRAGON', 'IM_COLD']
	sfreq = 6
	sflags = ['HASTE', 'SLOW', 'CONF', 'SCARE', 'DARKNESS']
	description = """It is a dragon-like form wrapped in shadow.  Glowing red eyes shine out in 
the dark.
"""

monsters.register_race(shadow_drake)


class manticore(monster, spellcaster):
	r_idx = "472"
	name = "Manticore"
	ascii = "H,o"
	level = 30
	rarity = 2
	hp = "25d7"
	speed = 120
	aaf = 12
	ac = 15
	sleep = 10
	exp = 300
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'BASH_DOOR', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'DROP_CORPSE', 'EVIL']
	sfreq = 5
	sflags = ['ARROW_4']
	description = """It is a winged lion's body with a human torso and a tail covered in 
vicious spikes.
"""

monsters.register_race(manticore)


class giant_army_ant(monster):
	r_idx = "473"
	name = "Giant army ant"
	ascii = "a,o"
	level = 30
	rarity = 3
	hp = "14d6"
	speed = 120
	aaf = 10
	ac = 20
	sleep = 40
	exp = 90
	blows = [('BITE', 'HURT', '2d12')]
	flags = ['RAND_25', 'KILL_BODY', 'FRIENDS', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """An armoured form moving with purpose.  Powerful on its own, flee when 
hordes of them march.
"""

monsters.register_race(giant_army_ant)


class killer_slicer_beetle(monster):
	r_idx = "474"
	name = "Killer slicer beetle"
	ascii = "K,o"
	level = 30
	rarity = 2
	hp = "22d7"
	speed = 110
	aaf = 14
	ac = 25
	sleep = 30
	exp = 200
	blows = [('BITE', 'HURT', '5d8'), ('BITE', 'HURT', '5d8')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_CORPSE', 'ANIMAL', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """It is a beetle with deadly sharp cutting mandibles and a rock-hard 
carapace.
"""

monsters.register_race(killer_slicer_beetle)


class gorgon(monster, spellcaster):
	r_idx = "475"
	name = "Gorgon"
	ascii = "H,b"
	level = 31
	rarity = 2
	hp = "30d14"
	speed = 110
	aaf = 12
	ac = 44
	sleep = 20
	exp = 275
	blows = [('BUTT', 'HURT', '3d9'), ('BUTT', 'HURT', '3d9'), ('BITE', 'POISON', '1d10'), ('KICK', 'HURT', '2d4')]
	flags = ['FORCE_SLEEP', 'ANIMAL', 'MOVE_BODY', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'BASH_DOOR', 'IM_POIS', 'IM_ACID', 'DROP_CORPSE', 'EVIL']
	sfreq = 8
	sflags = ['BR_POIS']
	description = """A bull whose skin is made of steel plates. Watch out for its 
deadly breath!
"""

monsters.register_race(gorgon)


class gug(giant):
	r_idx = "476"
	name = "Gug"
	ascii = "P,D"
	level = 31
	rarity = 2
	hp = "17d15"
	speed = 110
	aaf = 14
	ac = 30
	sleep = 30
	exp = 210
	blows = [('BITE', 'HURT', '10d4'), ('CLAW', 'HURT', '2d7'), ('CLAW', 'HURT', '2d7'), ('CLAW', 'HURT', '2d7')]
	flags = ['FORCE_SLEEP', 'DROP_1D2', 'TAKE_ITEM', 'WILD_DUNGEON_01', 'BASH_DOOR', 'OPEN_DOOR', 'DROP_CORPSE', 'EVIL', 'IM_POIS', 'IM_ACID', 'GIANT']
	sfreq = 0
	sflags = []
	description = """A hideous, beastly, four-armed unclean giantbarrel... The eyes jutted two inches from each side, shaded by 
bony protuberances overgrown of the mouth. That mouth had great 
yellow fangs and ran from the top to the tally."
"""

monsters.register_race(gug)


class ghost(monster, undead, spellcaster):
	r_idx = "477"
	name = "Ghost"
	ascii = "G,w"
	level = 31
	rarity = 1
	hp = "9d8"
	speed = 120
	aaf = 20
	ac = 20
	sleep = 10
	exp = 350
	blows = [('WAIL', 'TERRIFY', ''), ('TOUCH', 'EXP_20', ''), ('CLAW', 'LOSE_INT', '1d6'), ('CLAW', 'LOSE_WIS', '1d6')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'DROP_60', 'DROP_1D2', 'WILD_DUNGEON_01', 'INVISIBLE', 'COLD_BLOOD', 'TAKE_ITEM', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'CAN_FLY', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['BLIND', 'HOLD', 'DRAIN_MANA']
	description = """You don't believe in them.
"""

monsters.register_race(ghost)


class death_watch_beetle(monster):
	r_idx = "478"
	name = "Death watch beetle"
	ascii = "K,B"
	level = 31
	rarity = 3
	hp = "19d12"
	speed = 110
	aaf = 16
	ac = 30
	sleep = 30
	exp = 190
	blows = [('BITE', 'HURT', '5d4'), ('WAIL', 'TERRIFY', '5d6')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_FOREST2', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'ANIMAL', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """It is a giant beetle that produces a chilling sound.
"""

monsters.register_race(death_watch_beetle)


class ogre_shaman(giant, spellcaster):
	r_idx = "479"
	name = "Ogre shaman"
	ascii = "O,b"
	level = 32
	rarity = 2
	hp = "14d7"
	speed = 110
	aaf = 20
	ac = 26
	sleep = 30
	exp = 250
	blows = [('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'ONLY_ITEM', 'DROP_90', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT']
	sfreq = 5
	sflags = ['TPORT', 'HOLD', 'SCARE', 'CAUSE_2', 'TRAPS', 'BO_FIRE', 'S_MONSTER']
	description = """It is an ogre wrapped in furs and covered in grotesque body paints.
"""

monsters.register_race(ogre_shaman)


class nexus_quylthulg(monster, spellcaster):
	r_idx = "480"
	name = "Nexus quylthulg"
	ascii = "Q,v"
	level = 32
	rarity = 1
	hp = "7d12"
	speed = 110
	aaf = 10
	ac = 1
	sleep = 1
	exp = 300
	blows = []
	flags = ['FORCE_SLEEP', 'NEVER_MOVE', 'NEVER_BLOW', 'WILD_DUNGEON_01', 'INVISIBLE', 'EMPTY_MIND', 'RES_NEXU', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'RES_TELE']
	sfreq = 1
	sflags = ['BLINK', 'TELE_AWAY']
	description = """It is a very unstable, strange pulsing mound of flesh.
"""

monsters.register_race(nexus_quylthulg)


class shelob_spider_of_darkness(monster, spellcaster):
	r_idx = "481"
	name = "Shelob, Spider of Darkness"
	ascii = "S,D"
	level = 32
	rarity = 3
	hp = "12d70"
	speed = 110
	aaf = 8
	ac = 40
	sleep = 80
	exp = 1200
	blows = [('BITE', 'HURT', '2d10'), ('STING', 'POISON', '2d5'), ('STING', 'LOSE_STR', '1d4'), ('STING', 'POISON', '2d5')]
	flags = ['UNIQUE', 'FEMALE', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ESCORTS', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'DROP_GOOD', 'SMART', 'BASH_DOOR', 'ANIMAL', 'EVIL', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HEAL', 'BLIND', 'SLOW', 'CONF', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'TRAPS', 'BR_DARK', 'S_SPIDER']
	description = """Shelob is an enormous bloated spider, rumoured to have been one of the 
brood of Ungoliant the Unlight.  Her poison is legendary, as is her ego, 
which may be her downfall.  She used to guard the pass through Cirith 
Ungol, but has not been seen there for many eons.
"""

monsters.register_race(shelob_spider_of_darkness)


class giant_squid(monster, spellcaster):
	r_idx = "482"
	name = "Giant squid"
	ascii = "l,g"
	level = 32
	rarity = 3
	hp = "80d7"
	speed = 110
	aaf = 8
	ac = 40
	sleep = 80
	exp = 600
	blows = [('CRUSH', 'HURT', '8d4'), ('CRUSH', 'HURT', '8d4'), ('CRUSH', 'HURT', '8d4')]
	flags = ['IM_ACID', 'RES_WATE', 'AQUATIC', 'ANIMAL', 'IM_COLD', 'MOVE_BODY', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_OCEAN']
	sfreq = 8
	sflags = ['BR_ELEC', 'BR_ACID', 'BR_POIS']
	description = """Besides being capable of dragging whole ships underwater, 
this creature can also harm you with ranged attacks.
"""

monsters.register_race(giant_squid)


class ghoulking(monster, undead, spellcaster):
	r_idx = "483"
	name = "Ghoulking"
	ascii = "z,D"
	level = 32
	rarity = 2
	hp = "11d12"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 10
	exp = 340
	blows = [('CLAW', 'LOSE_STR', '3d4'), ('CLAW', 'DISEASE', '3d4'), ('CLAW', 'DISEASE', '3d4'), ('BITE', 'PARALYZE', '3d5')]
	flags = ['WILD_DUNGEON_01', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'DROP_1D2', 'FORCE_MAXHP', 'ESCORT', 'FORCE_SLEEP', 'EVIL', 'UNDEAD', 'IM_POIS', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'COLD_BLOOD', 'HURT_LITE']
	sfreq = 7
	sflags = ['SCARE', 'HOLD', 'DARKNESS', 'S_UNDEAD', 'ANIM_DEAD']
	description = """Flesh is falling off in chunks from this decaying abomination.
"""

monsters.register_race(ghoulking)


class doombat(monster):
	r_idx = "484"
	name = "Doombat"
	ascii = "b,b"
	level = 32
	rarity = 2
	hp = "18d24"
	speed = 120
	aaf = 16
	ac = 37
	sleep = 30
	exp = 250
	blows = [('BITE', 'FIRE', '5d4'), ('BITE', 'FIRE', '5d4'), ('BITE', 'FIRE', '5d4')]
	flags = ['WILD_DUNGEON_01', 'WILD_WASTE2', 'WILD_MOUNT2', 'WEIRD_MIND', 'BASH_DOOR', 'AURA_FIRE', 'CAN_FLY', 'DROP_CORPSE', 'IM_FIRE', 'EVIL', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is a fast moving creature of chaos, a gigantic black bat 
surrounded by flickering bright red flames.
"""

monsters.register_race(doombat)


class ninja(monster):
	r_idx = "485"
	name = "Ninja"
	ascii = "p,D"
	level = 32
	rarity = 2
	hp = "13d9"
	speed = 120
	aaf = 20
	ac = 25
	sleep = 10
	exp = 300
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'LOSE_STR', '3d4'), ('HIT', 'LOSE_STR', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST1', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A humanoid clothed in black who moves with blinding speed.
"""

monsters.register_race(ninja)


class memory_moss(monster, spellcaster):
	r_idx = "486"
	name = "Memory moss"
	ascii = ",,b"
	level = 32
	rarity = 3
	hp = "1d2"
	speed = 110
	aaf = 30
	ac = 1
	sleep = 5
	exp = 150
	blows = [('HIT', 'CONFUSE', '1d4'), ('HIT', 'CONFUSE', '1d4')]
	flags = ['FORCE_SLEEP', 'NEVER_MOVE', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'STUPID', 'EMPTY_MIND', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['FORGET']
	description = """A mass of green vegetation.  You don't remember seeing anything like it 
before.
"""

monsters.register_race(memory_moss)


class storm_giant(giant, spellcaster):
	r_idx = "487"
	name = "Storm giant"
	ascii = "P,b"
	level = 32
	rarity = 1
	hp = "38d20"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 40
	exp = 1500
	blows = [('HIT', 'ELEC', '3d8'), ('HIT', 'ELEC', '3d8'), ('HIT', 'ELEC', '3d8')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_1D2', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'AURA_ELEC', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'GIANT', 'IM_COLD', 'IM_ELEC', 'MALE']
	sfreq = 8
	sflags = ['BLINK', 'TELE_TO', 'CONF', 'SCARE', 'BO_ELEC', 'BA_ELEC']
	description = """It is a twenty-five foot tall giant wreathed in lighting.
"""

monsters.register_race(storm_giant)


class spectator(monster, spellcaster):
	r_idx = "488"
	name = "Spectator"
	ascii = "e,B"
	level = 32
	rarity = 3
	hp = "20d7"
	speed = 110
	aaf = 30
	ac = 1
	sleep = 5
	exp = 150
	blows = [('GAZE', 'PARALYZE', '1d4'), ('GAZE', 'CONFUSE', '1d4'), ('GAZE', 'UN_BONUS', '')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'STUPID', 'EMPTY_MIND', 'CAN_FLY', 'DROP_CORPSE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['FORGET', 'CAUSE_2', 'HOLD', 'SLOW']
	description = """It has two small eyestalks and a large central eye.
"""

monsters.register_race(spectator)


class bokrug(demon, spellcaster):
	r_idx = "489"
	name = "Bokrug"
	ascii = "R,g"
	level = 33
	rarity = 7
	hp = "11d68"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 2200
	blows = [('BITE', 'HURT', '5d5'), ('CRUSH', 'HURT', '2d10'), ('CRUSH', 'HURT', '2d10')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'NONLIVING', 'ESCORT', 'ESCORTS', 'ELDRITCH_HORROR', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'EVIL', 'IM_COLD', 'IM_POIS', 'DEMON']
	sfreq = 5
	sflags = ['S_UNDEAD', 'MIND_BLAST', 'CAUSE_3', 'SCARE', 'BO_WATE', 'S_KIN']
	description = """A lizard-like Great Old One worshipped by the men of Sarnath.
"""

monsters.register_race(bokrug)


class biclops(giant, spellcaster):
	r_idx = "490"
	name = "Biclops"
	ascii = "P,u"
	level = 33
	rarity = 5
	hp = "60d9"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 20
	exp = 1200
	blows = [('HIT', 'HURT', '11d8'), ('HIT', 'HURT', '11d8')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'DROP_1D2', 'TAKE_ITEM', 'DROP_CORPSE', 'BASH_DOOR', 'OPEN_DOOR', 'MOVE_BODY', 'EVIL', 'IM_POIS', 'IM_ACID', 'GIANT']
	sfreq = 8
	sflags = ['ARROW_4']
	description = """Oh, no! Aaargh! It is the most unnatural, most disgusting 
creature imaginableof nature must be exterminated!
"""

monsters.register_race(biclops)


class half_troll(troll):
	r_idx = "491"
	name = "Half-troll"
	ascii = "T,o"
	level = 33
	rarity = 2
	hp = "18d14"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 50
	exp = 300
	blows = [('CLAW', 'HURT', '1d5'), ('CLAW', 'HURT', '1d5'), ('CLAW', 'HURT', '1d5'), ('BITE', 'HURT', '2d6')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'FRIENDS', 'ONLY_ITEM', 'DROP_90', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A huge, ugly, half-human in search of plunder.
"""

monsters.register_race(half_troll)


class ivory_monk(monster, spellcaster):
	r_idx = "492"
	name = "Ivory monk"
	ascii = "p,w"
	level = 33
	rarity = 1
	hp = "40d7"
	speed = 120
	aaf = 25
	ac = 25
	sleep = 7
	exp = 900
	blows = [('KICK', 'HURT', '8d1'), ('HIT', 'HURT', '8d1'), ('KICK', 'HURT', '8d4'), ('HIT', 'HURT', '8d1')]
	flags = ['WILD_DUNGEON_01', 'MALE', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'IM_FIRE', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'DROP_CORPSE', 'DROP_SKELETON']
	sfreq = 7
	sflags = ['HEAL']
	description = """A monk in white robes.  He is skilled in martial arts.
"""

monsters.register_race(ivory_monk)


class bert_the_stone_troll(troll):
	r_idx = "493"
	name = "Bert the Stone Troll"
	ascii = "T,W"
	level = 33
	rarity = 7
	hp = "11d70"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 2000
	blows = [('HIT', 'HURT', '5d5'), ('BITE', 'HURT', '2d10'), ('BITE', 'HURT', '2d3')]
	flags = ['UNIQUE', 'MALE', 'FORCE_MAXHP', 'ESCORT', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'TROLL', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'HURT_ROCK']
	sfreq = 0
	sflags = []
	description = """Big, brawny, powerful and with a taste for hobbit.  He has friends called 
Bill and Tom.
"""

monsters.register_race(bert_the_stone_troll)


class bill_the_stone_troll(troll):
	r_idx = "494"
	name = "Bill the Stone Troll"
	ascii = "T,W"
	level = 33
	rarity = 7
	hp = "11d70"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 2000
	blows = [('HIT', 'HURT', '5d5'), ('BITE', 'HURT', '2d10'), ('BITE', 'HURT', '2d3')]
	flags = ['UNIQUE', 'MALE', 'FORCE_MAXHP', 'ESCORT', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'TROLL', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'HURT_ROCK']
	sfreq = 0
	sflags = []
	description = """Big, brawny, powerful and with a taste for hobbit.  He has friends called 
Bert and Tom.
"""

monsters.register_race(bill_the_stone_troll)


class tom_the_stone_troll(troll):
	r_idx = "495"
	name = "Tom the Stone Troll"
	ascii = "T,W"
	level = 33
	rarity = 7
	hp = "11d70"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 2000
	blows = [('HIT', 'HURT', '5d5'), ('BITE', 'HURT', '2d10'), ('BITE', 'HURT', '2d3')]
	flags = ['UNIQUE', 'MALE', 'FORCE_MAXHP', 'ESCORT', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'TROLL', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'HURT_ROCK']
	sfreq = 0
	sflags = []
	description = """Big, brawny, powerful and with a taste for hobbit.  He has friends called 
Bert and Bill.
"""

monsters.register_race(tom_the_stone_troll)


class cave_troll(troll):
	r_idx = "496"
	name = "Cave troll"
	ascii = "T,u"
	level = 33
	rarity = 1
	hp = "18d12"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 350
	blows = [('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'FRIENDS', 'DROP_60', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'IM_POIS', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is a vicious monster, feared for his ferocity.
"""

monsters.register_race(cave_troll)


class anti_paladin(monster, spellcaster):
	r_idx = "497"
	name = "Anti-paladin"
	ascii = "p,D"
	level = 33
	rarity = 2
	hp = "30d14"
	speed = 120
	aaf = 30
	ac = 25
	sleep = 30
	exp = 450
	blows = [('HIT', 'HURT', '2d6'), ('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6')]
	flags = ['WILD_DUNGEON_01', 'MALE', 'OPEN_DOOR', 'BASH_DOOR', 'TAKE_ITEM', 'DROP_1D2', 'ONLY_ITEM', 'EVIL', 'IM_POIS', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 4
	sflags = ['HOLD', 'SCARE', 'BLIND', 'CAUSE_3', 'TRAPS', 'DARKNESS', 'FORGET', 'HASTE']
	description = """An embodiment of all the cardinal vices, he beholds you scornfully.
"""

monsters.register_race(anti_paladin)


class logrus_master(monster, spellcaster):
	r_idx = "498"
	name = "Logrus master"
	ascii = "p,v"
	level = 33
	rarity = 3
	hp = "30d7"
	speed = 120
	aaf = 30
	ac = 25
	sleep = 5
	exp = 550
	blows = [('HIT', 'HURT', '10d2'), ('KICK', 'HURT', '10d2'), ('PUNCH', 'HURT', '10d2'), ('KICK', 'HURT', '10d2')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'FORCE_SLEEP', 'FORCE_MAXHP', 'EVIL', 'ONLY_ITEM', 'DROP_1D2', 'ATTR_ANY', 'DROP_SKELETON', 'DROP_CORPSE', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'IM_ACID', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['HEAL', 'S_SPIDER', 'BA_CHAO', 'S_DEMON']
	description = """An adept of chaos, feared for his skill of invoking raw Logrus.
"""

monsters.register_race(logrus_master)


class barrow_wight(monster, undead, spellcaster):
	r_idx = "499"
	name = "Barrow wight"
	ascii = "W,W"
	level = 33
	rarity = 3
	hp = "15d7"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 10
	exp = 375
	blows = [('HIT', 'HURT', '1d8'), ('HIT', 'HURT', '1d8'), ('TOUCH', 'EXP_40', '')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'DROP_60', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['HOLD', 'SCARE', 'CAUSE_2', 'DARKNESS']
	description = """It is a ghostly nightmare of an entity.
"""

monsters.register_race(barrow_wight)


class giant_skeleton_troll(troll, undead):
	r_idx = "500"
	name = "Giant skeleton troll"
	ascii = "s,b"
	level = 33
	rarity = 1
	hp = "45d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 20
	exp = 325
	blows = [('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d9'), ('BITE', 'HURT', '1d5'), ('BITE', 'HURT', '1d5')]
	flags = ['FORCE_MAXHP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is the animated form of a massive troll.
"""

monsters.register_race(giant_skeleton_troll)


class chaos_drake(dragon, spellcaster):
	r_idx = "501"
	name = "Chaos drake"
	ascii = "d,v"
	level = 33
	rarity = 3
	hp = "50d7"
	speed = 110
	aaf = 25
	ac = 50
	sleep = 30
	exp = 700
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d6')]
	flags = ['WILD_DUNGEON_01', 'WILD_WASTE2', 'ATTR_MULTI', 'ATTR_ANY', 'CAN_FLY', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'RES_DISE', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SLOW', 'CONF', 'SCARE', 'BR_DISE', 'BR_CHAO']
	description = """A dragon twisted by the forces of chaos.  It seems first ugly, then fair, 
as its form shimmers and changes in front of your eyes.
"""

monsters.register_race(chaos_drake)


class law_drake(monster, spellcaster):
	r_idx = "502"
	name = "Law drake"
	ascii = "d,W"
	level = 33
	rarity = 3
	hp = "45d7"
	speed = 110
	aaf = 25
	ac = 50
	sleep = 30
	exp = 700
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d6')]
	flags = ['WILD_DUNGEON_01', 'WILD_MOUNT2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'GOOD', 'ONLY_ITEM', 'DROP_2D2', 'CAN_FLY', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE DRAGON', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SLOW', 'CONF', 'SCARE', 'BR_SOUN', 'BR_SHAR']
	description = """This dragon is clever and cunning.  It laughs at your puny efforts to 
disturb it.
"""

monsters.register_race(law_drake)


class balance_drake(dragon, spellcaster):
	r_idx = "503"
	name = "Balance drake"
	ascii = "d,s"
	level = 33
	rarity = 3
	hp = "50d7"
	speed = 110
	aaf = 25
	ac = 45
	sleep = 30
	exp = 700
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d6')]
	flags = ['ATTR_MULTI', 'ATTR_ANY', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'RES_DISE', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'DRAGON', 'GOOD', 'CAN_FLY', 'IM_FIRE', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SLOW', 'CONF', 'SCARE', 'BR_SOUN', 'BR_SHAR', 'BR_DISE', 'BR_CHAO']
	description = """A mighty dragon, the balance drake seeks to maintain the Cosmic Balance, 
and despises your feeble efforts to destroy evil.
"""

monsters.register_race(balance_drake)


class ethereal_drake(dragon, spellcaster):
	r_idx = "504"
	name = "Ethereal drake"
	ascii = "d,o"
	level = 33
	rarity = 3
	hp = "45d7"
	speed = 110
	aaf = 25
	ac = 45
	sleep = 15
	exp = 700
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'ONLY_ITEM', 'DROP_2D2', 'CAN_FLY', 'INVISIBLE', 'PASS_WALL', 'EVIL', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SLOW', 'CONF', 'SCARE', 'BR_LITE', 'BR_DARK']
	description = """A dragon of elemental power, with control over light and dark, the 
ethereal drake's eyes glare with white hatred from the shadows.
"""

monsters.register_race(ethereal_drake)


class groo_the_wanderer(troll):
	r_idx = "505"
	name = "Groo the Wanderer"
	ascii = "p,o"
	level = 33
	rarity = 7
	hp = "13d85"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 50
	exp = 2000
	blows = [('HIT', 'HURT', '9d1'), ('HIT', 'HURT', '6d5'), ('HIT', 'HURT', '25d1'), ('HIT', 'HURT', '30d1')]
	flags = ['UNIQUE', 'MALE', 'WEIRD_MIND', 'CAN_SPEAK', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'DROP_CHOSEN', 'OPEN_DOOR', 'BASH_DOOR', 'TROLL', 'IM_COLD', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """He who laughs at Groo's brains will find there is nothing to laugh 
about... erm, nobody laughs at Groo and lives.
"""

monsters.register_race(groo_the_wanderer)


class fasolt_the_giant(giant):
	r_idx = "506"
	name = "Fasolt the Giant"
	ascii = "P,u"
	level = 33
	rarity = 7
	hp = "11d83"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 2000
	blows = [('HIT', 'HURT', '5d5'), ('BITE', 'HURT', '2d10'), ('HIT', 'EAT_GOLD', '2d2')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_CORPSE', 'ESCORT', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT', 'IM_COLD', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """Big, brawny, powerful and with a greed for gold.
"""

monsters.register_race(fasolt_the_giant)


class logrus_ghost(monster, undead, spellcaster):
	r_idx = "507"
	name = "Logrus ghost"
	ascii = "G,r"
	level = 33
	rarity = 3
	hp = "14d14"
	speed = 120
	aaf = 20
	ac = 15
	sleep = 10
	exp = 350
	blows = [('WAIL', 'TERRIFY', ''), ('TOUCH', 'EXP_40', ''), ('CLAW', 'LOSE_INT', '1d10')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_90', 'DROP_2D2', 'INVISIBLE', 'COLD_BLOOD', 'TAKE_ITEM', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP', 'ATTR_MULTI', 'ATTR_ANY']
	sfreq = 15
	sflags = ['BLIND', 'HOLD', 'DRAIN_MANA', 'FORGET']
	description = """An almost life-like creature which is nothing more than a phantom 
created by the Logrus.
"""

monsters.register_race(logrus_ghost)


class spectre(monster, undead, spellcaster):
	r_idx = "508"
	name = "Spectre"
	ascii = "G,G"
	level = 33
	rarity = 3
	hp = "14d15"
	speed = 120
	aaf = 20
	ac = 20
	sleep = 10
	exp = 350
	blows = [('WAIL', 'TERRIFY', ''), ('TOUCH', 'EXP_40', ''), ('CLAW', 'LOSE_WIS', '5d5')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_90', 'DROP_2D2', 'CAN_FLY', 'COLD_BLOOD', 'TAKE_ITEM', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['BLIND', 'HOLD', 'DRAIN_MANA', 'FORGET']
	description = """A phantasmal shrieking spirit.  Its wail drives the intense cold of pure 
evil deep within your body.
"""

monsters.register_race(spectre)


class water_troll(troll):
	r_idx = "509"
	name = "Water troll"
	ascii = "T,B"
	level = 33
	rarity = 1
	hp = "36d7"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 420
	blows = [('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '2d2'), ('HIT', 'HURT', '2d2')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_SWAMP1', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'FRIENDS', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'EVIL', 'TROLL', 'IM_COLD', 'IM_POIS', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is a troll that reeks of brine.
"""

monsters.register_race(water_troll)


class fire_elemental(monster, spellcaster):
	r_idx = "510"
	name = "Fire elemental"
	ascii = "E,r"
	level = 33
	rarity = 2
	hp = "21d8"
	speed = 110
	aaf = 12
	ac = 25
	sleep = 50
	exp = 350
	blows = [('HIT', 'FIRE', '4d6'), ('HIT', 'FIRE', '4d6')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'EMPTY_MIND', 'CAN_FLY', 'WILD_DUNGEON_01', 'KILL_ITEM', 'KILL_BODY', 'BASH_DOOR', 'POWERFUL', 'AURA_FIRE', 'EVIL', 'IM_FIRE', 'IM_POIS', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['BO_FIRE']
	description = """It is a towering inferno of flames.
"""

monsters.register_race(fire_elemental)


class cherub(monster, spellcaster):
	r_idx = "511"
	name = "Cherub"
	ascii = "A,W"
	level = 33
	rarity = 6
	hp = "45d7"
	speed = 120
	aaf = 30
	ac = 35
	sleep = 255
	exp = 400
	blows = [('HIT', 'HURT', '4d3'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '4d3'), ('HIT', 'HURT', '3d8')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_FEAR', 'GOOD', 'CAN_FLY', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'REFLECTING', 'RES_TELE', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['HEAL', 'HASTE', 'BLIND', 'SCARE', 'MIND_BLAST', 'BO_FIRE', 'S_MONSTERS']
	description = """It is an angel moving very quickly, wielding a holy war hammer and casting 
a volley of powerful spells in your direction.
"""

monsters.register_race(cherub)


class water_elemental(monster, spellcaster):
	r_idx = "512"
	name = "Water elemental"
	ascii = "E,b"
	level = 33
	rarity = 2
	hp = "18d8"
	speed = 110
	aaf = 12
	ac = 20
	sleep = 50
	exp = 325
	blows = [('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'CAN_FLY', 'KILL_BODY', 'KILL_ITEM', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_POIS', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['BO_COLD']
	description = """It is a towering tempest of water.
"""

monsters.register_race(water_elemental)


class multi_hued_hound(monster, spellcaster):
	r_idx = "513"
	name = "Multi-hued hound"
	ascii = "Z,v"
	level = 33
	rarity = 2
	hp = "30d7"
	speed = 120
	aaf = 25
	ac = 20
	sleep = 0
	exp = 600
	blows = [('CLAW', 'HURT', '3d6'), ('CLAW', 'HURT', '3d6'), ('BITE', 'HURT', '4d4'), ('BITE', 'HURT', '4d4')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'BASH_DOOR', 'ATTR_MULTI', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'NO_CONF', 'NO_SLEEP', 'IM_ELEC', 'IM_POIS', 'IM_ACID', 'IM_FIRE', 'IM_COLD']
	sfreq = 5
	sflags = ['BR_ACID', 'BR_POIS', 'BR_COLD', 'BR_FIRE', 'BR_ELEC']
	description = """Shimmering in rainbow hues, this hound is beautiful and deadly. 
"""

monsters.register_race(multi_hued_hound)


class night_stalker(monster, undead):
	r_idx = "514"
	name = "Night stalker"
	ascii = "E,w"
	level = 34
	rarity = 3
	hp = "14d12"
	speed = 130
	aaf = 20
	ac = 25
	sleep = 20
	exp = 310
	blows = [('GAZE', 'HURT', '6d6'), ('GAZE', 'HURT', '6d6')]
	flags = ['RAND_50', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'INVISIBLE', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'EVIL', 'IM_COLD', 'IM_POIS', 'UNDEAD', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """It is impossible to define its form but its violence is legendary.
"""

monsters.register_race(night_stalker)


class carrion_crawler(monster):
	r_idx = "515"
	name = "Carrion crawler"
	ascii = "c,g"
	level = 34
	rarity = 2
	hp = "14d12"
	speed = 110
	aaf = 15
	ac = 20
	sleep = 10
	exp = 100
	blows = [('STING', 'PARALYZE', '2d6'), ('STING', 'PARALYZE', '2d6')]
	flags = ['RAND_25', 'FRIENDS', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'ANIMAL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A hideous centipede covered in slime and with glowing tentacles around its 
head.
"""

monsters.register_race(carrion_crawler)


class master_thief(monster):
	r_idx = "516"
	name = "Master thief"
	ascii = "p,D"
	level = 34
	rarity = 2
	hp = "18d8"
	speed = 130
	aaf = 20
	ac = 25
	sleep = 40
	exp = 350
	blows = [('HIT', 'HURT', '2d8'), ('HIT', 'HURT', '3d4'), ('HIT', 'EAT_GOLD', '4d4'), ('HIT', 'EAT_ITEM', '4d5')]
	flags = ['MALE', 'DROP_90', 'DROP_2D2', 'DROP_SKELETON', 'DROP_CORPSE', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_GRASS', 'EVIL']
	sfreq = 0
	sflags = []
	description = """Cool and confident, fast and lithe; protect your possessions quickly!
"""

monsters.register_race(master_thief)


class jurt_the_living_trump(monster, spellcaster):
	r_idx = "517"
	name = "Jurt the Living Trump"
	ascii = "p,R"
	level = 34
	rarity = 5
	hp = "10d70"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 40
	exp = 1200
	blows = [('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'AURA_ELEC', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 6
	sflags = ['TPORT', 'BLINK', 'BA_CHAO']
	description = """A magical treatment has turned Jurt, an adept of Logrus, 
into a Living Trump.
"""

monsters.register_race(jurt_the_living_trump)


class lich(monster, undead, spellcaster):
	r_idx = "518"
	name = "Lich"
	ascii = "L,w"
	level = 34
	rarity = 3
	hp = "30d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 60
	exp = 800
	blows = [('TOUCH', 'EXP_40', ''), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'LOSE_DEX', '2d8'), ('TOUCH', 'LOSE_DEX', '2d8')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_1D2', 'WILD_DUNGEON_01', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BLINK', 'TELE_TO', 'TELE_AWAY', 'BLIND', 'HOLD', 'SLOW', 'SCARE', 'CAUSE_3', 'DRAIN_MANA', 'BRAIN_SMASH']
	description = """It is a skeletal form dressed in robes.  It radiates vastly evil power.
"""

monsters.register_race(lich)


class gas_spore(monster):
	r_idx = "519"
	name = "Gas spore"
	ascii = "e,g"
	level = 34
	rarity = 4
	hp = "25d7"
	speed = 110
	aaf = 30
	ac = 1
	sleep = 5
	exp = 100
	blows = [('EXPLODE', 'DISEASE', '40d2')]
	flags = ['FORCE_SLEEP', 'STUPID', 'EMPTY_MIND', 'CAN_FLY', 'WILD_DUNGEON_01', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """From a distance, this creature is often mistaken for the 
much more dangerous beholder.
"""

monsters.register_race(gas_spore)


class master_vampire(monster, undead, spellcaster):
	r_idx = "520"
	name = "Master vampire"
	ascii = "V,s"
	level = 34
	rarity = 3
	hp = "34d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 10
	exp = 750
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('BITE', 'EXP_VAMP', '1d7'), ('BITE', 'EXP_VAMP', '1d7')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_4D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'CAN_FLY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['TELE_TO', 'HOLD', 'CONF', 'SCARE', 'CAUSE_3', 'MIND_BLAST', 'FORGET', 'DARKNESS', 'BO_NETH']
	description = """It is a humanoid form dressed in robes.  Power emanates from its chilling 
frame.
"""

monsters.register_race(master_vampire)


class oriental_vampire(monster, undead, spellcaster):
	r_idx = "521"
	name = "Oriental vampire"
	ascii = "V,g"
	level = 34
	rarity = 3
	hp = "30d9"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 10
	exp = 750
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('BITE', 'EXP_VAMP', '1d8')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_4D2', 'CAN_FLY', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'INVISIBLE', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 6
	sflags = ['TELE_TO', 'HOLD', 'CONF', 'SCARE', 'CAUSE_3', 'MIND_BLAST', 'FORGET', 'DARKNESS', 'BO_NETH']
	description = """The oriental vampire is a mist-like creature.
"""

monsters.register_race(oriental_vampire)


class greater_mummy(monster, undead, spellcaster):
	r_idx = "522"
	name = "Greater mummy"
	ascii = "z,W"
	level = 34
	rarity = 3
	hp = "41d13"
	speed = 110
	aaf = 30
	ac = 34
	sleep = 255
	exp = 800
	blows = [('CLAW', 'LOSE_CON', '1d6'), ('CLAW', 'DISEASE', '1d6'), ('GAZE', 'EXP_VAMP', '3d4'), ('GAZE', 'TERRIFY', '1d4')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'NO_FEAR', 'EVIL', 'UNDEAD', 'COLD_BLOOD', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'RES_TELE', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['HEAL', 'HASTE', 'BLIND', 'SCARE', 'S_UNDEAD', 'ANIM_DEAD', 'BA_POIS', 'BA_NETH', 'BA_COLD', 'DRAIN_MANA', 'MIND_BLAST', 'CAUSE_3', 'DARKNESS', 'FORGET']
	description = """Once a powerful ruler, now an even more powerful undead menace.
"""

monsters.register_race(greater_mummy)


class bloodletter_of_khorne(demon):
	r_idx = "523"
	name = "Bloodletter of Khorne"
	ascii = "U,r"
	level = 34
	rarity = 1
	hp = "23d8"
	speed = 120
	aaf = 20
	ac = 20
	sleep = 30
	exp = 450
	blows = [('HIT', 'EXP_20', '13d1'), ('HIT', 'EXP_20', '13d1'), ('HIT', 'EXP_20', '13d1')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'OPEN_DOOR', 'FRIENDS', 'DROP_60', 'REGENERATE', 'ONLY_ITEM', 'DROP_CHOSEN', 'NO_FEAR', 'NONLIVING', 'EVIL', 'IM_POIS', 'IM_COLD', 'IM_FIRE', 'DEMON']
	sfreq = 0
	sflags = []
	description = """Slender, red-skinned demons twisting in nightmarish shapes. 
They are armed with hellblades, which will suck the life from 
your body.
"""

monsters.register_race(bloodletter_of_khorne)


class giant_grey_scorpion(monster):
	r_idx = "524"
	name = "Giant grey scorpion"
	ascii = "S,s"
	level = 34
	rarity = 4
	hp = "18d14"
	speed = 120
	aaf = 12
	ac = 25
	sleep = 40
	exp = 275
	blows = [('BITE', 'HURT', '1d6'), ('STING', 'POISON', '1d4')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'DROP_SKELETON', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """It is a giant grey scorpion.  It looks poisonous.
"""

monsters.register_race(giant_grey_scorpion)


class earth_elemental(monster, spellcaster):
	r_idx = "525"
	name = "Earth elemental"
	ascii = "E,u"
	level = 34
	rarity = 2
	hp = "30d10"
	speed = 100
	aaf = 10
	ac = 35
	sleep = 90
	exp = 375
	blows = [('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '4d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'NONLIVING', 'KILL_ITEM', 'KILL_BODY', 'PASS_WALL', 'POWERFUL', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 8
	sflags = ['BO_ACID']
	description = """It is a towering form composed of rock with fists of awesome power.
"""

monsters.register_race(earth_elemental)


class air_elemental(monster, spellcaster):
	r_idx = "526"
	name = "Air elemental"
	ascii = "E,B"
	level = 34
	rarity = 2
	hp = "21d5"
	speed = 120
	aaf = 12
	ac = 25
	sleep = 50
	exp = 390
	blows = [('HIT', 'HURT', '1d10'), ('HIT', 'CONFUSE', '1d4'), ('HIT', 'HURT', '1d10')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'CAN_FLY', 'NONLIVING', 'KILL_BODY', 'KILL_ITEM', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 8
	sflags = ['BO_ELEC']
	description = """It is a towering tornado of winds.
"""

monsters.register_race(air_elemental)


class doom_drake(dragon, spellcaster):
	r_idx = "527"
	name = "Doom drake"
	ascii = "d,v"
	level = 34
	rarity = 3
	hp = "30d11"
	speed = 110
	aaf = 25
	ac = 50
	sleep = 30
	exp = 450
	blows = [('CLAW', 'HURT', '1d5'), ('CLAW', 'HURT', '1d5'), ('BITE', 'HURT', '2d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_1D2', 'FRIENDS', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['BR_FIRE']
	description = """Doom drakes are trained firedrakes, always moving in pairs, looking 
for a battle.
"""

monsters.register_race(doom_drake)


class gargoyle(demon, spellcaster):
	r_idx = "528"
	name = "Gargoyle"
	ascii = "u,s"
	level = 34
	rarity = 2
	hp = "18d9"
	speed = 110
	aaf = 10
	ac = 25
	sleep = 15
	exp = 110
	blows = [('CLAW', 'HURT', '2d6'), ('CLAW', 'HURT', '2d6'), ('BITE', 'HURT', '1d6')]
	flags = ['DROP_60', 'ONLY_GOLD', 'EVIL', 'DEMON', 'FRIENDS', 'HURT_LITE', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_WASTE1', 'WILD_WASTE2', 'IM_POIS', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'HURT_ROCK', 'NONLIVING']
	sfreq = 12
	sflags = ['BR_ELEC', 'BR_FIRE']
	description = """A weird demon creature with a stone-like skin.
"""

monsters.register_race(gargoyle)


class malicious_leprechaun(monster, spellcaster):
	r_idx = "529"
	name = "Malicious leprechaun"
	ascii = "h,v"
	level = 35
	rarity = 4
	hp = "4d4"
	speed = 120
	aaf = 8
	ac = 7
	sleep = 8
	exp = 85
	blows = [('TOUCH', 'EAT_GOLD', ''), ('TOUCH', 'EAT_ITEM', '')]
	flags = ['MULTIPLY', 'INVISIBLE', 'RAND_25', 'TAKE_ITEM', 'COLD_BLOOD', 'HURT_LITE', 'EVIL', 'OPEN_DOOR', 'MALE', 'WILD_DUNGEON_01']
	sfreq = 6
	sflags = ['BLINK', 'TPORT', 'TELE_TO', 'CAUSE_1']
	description = """This little creature has a fiendish gleam in its eyes.
"""

monsters.register_race(malicious_leprechaun)


class eog_golem(monster):
	r_idx = "530"
	name = "Eog golem"
	ascii = "g,D"
	level = 35
	rarity = 4
	hp = "100d14"
	speed = 100
	aaf = 12
	ac = 50
	sleep = 10
	exp = 1200
	blows = [('HIT', 'HURT', '8d6'), ('HIT', 'HURT', '8d6'), ('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6')]
	flags = ['ONLY_GOLD', 'DROP_2D2', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'REFLECTING', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 0
	sflags = []
	description = """It is a massive deep brown statue, striding towards you with an 
all-too-familiar purpose.  Your magic surprisingly feels much less 
powerful now.
"""

monsters.register_race(eog_golem)


class little_boy(monster):
	r_idx = "531"
	name = "Little Boy"
	ascii = "{,D"
	level = 35
	rarity = 2
	hp = "9d12"
	speed = 120
	aaf = 10
	ac = 40
	sleep = 12
	exp = 200
	blows = [('EXPLODE', 'SHATTER', '100d2')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'UNIQUE', 'REFLECTING', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'RES_TELE']
	sfreq = 0
	sflags = []
	description = """A shining machine of death and destruction.
"""

monsters.register_race(little_boy)


class dagashi(monster):
	r_idx = "532"
	name = "Dagashi"
	ascii = "p,y"
	level = 35
	rarity = 4
	hp = "8d25"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 10
	exp = 500
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'LOSE_STR', '3d4'), ('HIT', 'LOSE_STR', '3d4'), ('HIT', 'POISON', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A human warrior, moving with lightning speed.
"""

monsters.register_race(dagashi)


class headless_ghost(monster, undead, spellcaster):
	r_idx = "533"
	name = "Headless ghost"
	ascii = "G,u"
	level = 35
	rarity = 3
	hp = "14d25"
	speed = 120
	aaf = 20
	ac = 15
	sleep = 10
	exp = 550
	blows = [('TOUCH', 'TERRIFY', ''), ('TOUCH', 'EXP_40', ''), ('CLAW', 'LOSE_INT', '5d5'), ('CLAW', 'LOSE_WIS', '5d5')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_90', 'DROP_2D2', 'CAN_FLY', 'COLD_BLOOD', 'TAKE_ITEM', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['BLIND', 'DRAIN_MANA', 'SCARE', 'BO_COLD', 'FORGET']
	description = """A phantasmal apparition with no head
"""

monsters.register_race(headless_ghost)


class dread(monster, undead, spellcaster):
	r_idx = "534"
	name = "Dread"
	ascii = "G,o"
	level = 35
	rarity = 2
	hp = "25d14"
	speed = 120
	aaf = 20
	ac = 15
	sleep = 10
	exp = 600
	blows = [('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6'), ('HIT', 'LOSE_STR', '3d4')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_60', 'DROP_2D2', 'TAKE_ITEM', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['BLIND', 'HOLD', 'CONF', 'DRAIN_MANA', 'BO_NETH']
	description = """It is a form that screams its presence against the eye.  Death incarnate, 
its hideous black body seems to struggle against reality as the universe 
itself struggles to banish it.
"""

monsters.register_race(dread)


class leng_spider(monster):
	r_idx = "535"
	name = "Leng spider"
	ascii = "S,v"
	level = 35
	rarity = 4
	hp = "26d14"
	speed = 120
	aaf = 12
	ac = 25
	sleep = 40
	exp = 250
	blows = [('BITE', 'POISON', '1d6'), ('STING', 'POISON', '1d6')]
	flags = ['WEIRD_MIND', 'BASH_DOOR', 'FRIENDS', 'ELDRITCH_HORROR', 'DROP_CORPSE', 'ANIMAL', 'EVIL', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2']
	sfreq = 0
	sflags = []
	description = """Bloated purple spiders with long, bristly legs.
"""

monsters.register_race(leng_spider)


class star_vampire(monster):
	r_idx = "536"
	name = "Star vampire"
	ascii = "V,r"
	level = 35
	rarity = 2
	hp = "23d25"
	speed = 120
	aaf = 20
	ac = 20
	sleep = 10
	exp = 700
	blows = [('CLAW', 'LOSE_STR', '4d4'), ('BITE', 'EXP_VAMP', '4d4'), ('CLAW', 'LOSE_STR', '4d4'), ('BITE', 'EXP_VAMP', '4d4')]
	flags = ['WILD_DUNGEON_01', 'EVIL', 'ELDRITCH_HORROR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NONLIVING', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NO_STUN']
	sfreq = 0
	sflags = []
	description = """"It was red and dripping; an immensity of pulsing, moving jelly;
a scarlet blob with myriad tentacular trunks that waved and waved. 
There were suckers on the tips of the appendages, and these were 
opening and closing with ghoulish lust... the thing was bloated and 
obscene; a headless, faceless, eyeless bulk, with the ravenous maw and 
titanic talons of a star-born monster."
"""

monsters.register_race(star_vampire)


class smoke_elemental(monster, spellcaster):
	r_idx = "537"
	name = "Smoke elemental"
	ascii = "E,D"
	level = 35
	rarity = 3
	hp = "15d7"
	speed = 120
	aaf = 10
	ac = 40
	sleep = 90
	exp = 375
	blows = [('BITE', 'HURT', '2d6'), ('BITE', 'HURT', '2d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'KILL_ITEM', 'KILL_BODY', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'CAN_FLY', 'NONLIVING']
	sfreq = 5
	sflags = ['DARKNESS', 'BO_FIRE']
	description = """It is a towering blackened form, crackling with heat.
"""

monsters.register_race(smoke_elemental)


class olog(troll):
	r_idx = "538"
	name = "Olog"
	ascii = "T,g"
	level = 35
	rarity = 1
	hp = "25d10"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 450
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12'), ('BITE', 'HURT', '2d3'), ('BITE', 'HURT', '2d3')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'OPEN_DOOR', 'FRIENDS', 'DROP_60', 'SMART', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'TROLL', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """It is a massive intelligent troll with needle sharp fangs.
"""

monsters.register_race(olog)


class halfling_slinger(monster, spellcaster):
	r_idx = "539"
	name = "Halfling slinger"
	ascii = "h,U"
	level = 35
	rarity = 1
	hp = "23d9"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 30
	exp = 330
	blows = [('HIT', 'HURT', '2d6'), ('HIT', 'HURT', '2d6')]
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_GRASS', 'FORCE_MAXHP', 'OPEN_DOOR', 'FRIENDS', 'DROP_90', 'SMART', 'EVIL', 'IM_POIS', 'IM_COLD', 'MALE', 'DROP_SKELETON', 'DROP_CORPSE']
	sfreq = 3
	sflags = ['ARROW_4']
	description = """A rebel halfling who has rejected the halfling tradition of archery.
"""

monsters.register_race(halfling_slinger)


class gravity_hound(monster, spellcaster):
	r_idx = "540"
	name = "Gravity hound"
	ascii = "Z,s"
	level = 35
	rarity = 2
	hp = "21d10"
	speed = 110
	aaf = 30
	ac = 16
	sleep = 0
	exp = 500
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'WILD_DUNGEON_01', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BR_GRAV']
	description = """Unfettered by the usual constraints of gravity, these unnatural creatures 
are walking on the walls and even the ceiling!  The earth suddenly feels 
rather less solid as you see gravity warp all round the monsters.
"""

monsters.register_race(gravity_hound)


class acidic_cytoplasm(monster):
	r_idx = "541"
	name = "Acidic cytoplasm"
	ascii = "j,G"
	level = 35
	rarity = 5
	hp = "24d10"
	speed = 120
	aaf = 12
	ac = 12
	sleep = 1
	exp = 36
	blows = [('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10')]
	flags = ['FORCE_MAXHP', 'TAKE_ITEM', 'COLD_BLOOD', 'WILD_DUNGEON_01', 'DROP_1D2', 'DROP_4D2', 'STUPID', 'EMPTY_MIND', 'OPEN_DOOR', 'BASH_DOOR', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A disgusting animated blob of destruction.  Flee its gruesome hunger!
"""

monsters.register_race(acidic_cytoplasm)


class inertia_hound(monster, spellcaster):
	r_idx = "542"
	name = "Inertia hound"
	ascii = "Z,W"
	level = 35
	rarity = 2
	hp = "21d10"
	speed = 110
	aaf = 30
	ac = 16
	sleep = 0
	exp = 500
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'DROP_SKELETON', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BR_INER']
	description = """Bizarrely, this hound seems to be hardly moving at all, yet it approaches 
you with deadly menace.  It makes you tired just to look at it.
"""

monsters.register_race(inertia_hound)


class impact_hound(monster, spellcaster):
	r_idx = "543"
	name = "Impact hound"
	ascii = "Z,u"
	level = 35
	rarity = 2
	hp = "21d10"
	speed = 110
	aaf = 30
	ac = 16
	sleep = 0
	exp = 500
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['BR_WALL']
	description = """A deep brown shape is visible before you, its canine form strikes you with 
an almost physical force.  The dungeon floor buckles as if struck by a 
powerful blow as it stalks towards you.
"""

monsters.register_race(impact_hound)


class sea_troll(troll):
	r_idx = "544"
	name = "Sea troll"
	ascii = "T,B"
	level = 35
	rarity = 1
	hp = "41d7"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 50
	exp = 440
	blows = [('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '3d2'), ('HIT', 'HURT', '3d2')]
	flags = ['MALE', 'FORCE_MAXHP', 'AQUATIC', 'FRIENDS', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_OCEAN', 'OPEN_DOOR', 'BASH_DOOR', 'RES_WATE', 'EVIL', 'TROLL', 'IM_COLD', 'IM_POIS', 'HURT_LITE']
	sfreq = 0
	sflags = []
	description = """He is a troll that reeks of brine, close relative to water trolls.
"""

monsters.register_race(sea_troll)


class ooze_elemental(monster, spellcaster):
	r_idx = "545"
	name = "Ooze elemental"
	ascii = "E,U"
	level = 35
	rarity = 3
	hp = "8d10"
	speed = 110
	aaf = 10
	ac = 40
	sleep = 90
	exp = 300
	blows = [('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10')]
	flags = ['FORCE_SLEEP', 'EMPTY_MIND', 'COLD_BLOOD', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_SWAMP1', 'KILL_ITEM', 'KILL_BODY', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_ACID', 'IM_FIRE', 'CAN_SWIM', 'NONLIVING', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 5
	sflags = ['BO_ACID', 'BA_ACID']
	description = """It is a towering mass of filth, an eyesore of ooze.
"""

monsters.register_race(ooze_elemental)


class young_black_dragon(dragon, spellcaster):
	r_idx = "546"
	name = "Young black dragon"
	ascii = "d,D"
	level = 35
	rarity = 1
	hp = "22d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 50
	exp = 620
	blows = [('CLAW', 'HURT', '1d5'), ('CLAW', 'HURT', '1d5'), ('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_SWAMP2', 'DROP_60', 'DROP_90', 'DROP_1D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_ACID']
	sfreq = 11
	sflags = ['SCARE', 'BR_ACID']
	description = """It has a form that legends are made of.  Its still-tender scales are a 
darkest black hue.  Acid drips from its body.
"""

monsters.register_race(young_black_dragon)


class mumak(monster):
	r_idx = "547"
	name = "Mumak"
	ascii = "q,s"
	level = 35
	rarity = 3
	hp = "54d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 100
	exp = 2100
	blows = [('BUTT', 'HURT', '4d6'), ('BUTT', 'HURT', '4d6'), ('CRUSH', 'HURT', '4d4')]
	flags = ['BASH_DOOR', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A massive elephantine form with eyes twisted by madness.
"""

monsters.register_race(mumak)


class giant_red_ant(monster):
	r_idx = "548"
	name = "Giant red ant"
	ascii = "a,r"
	level = 35
	rarity = 1
	hp = "12d10"
	speed = 110
	aaf = 14
	ac = 30
	sleep = 40
	exp = 350
	blows = [('BITE', 'FIRE', '3d12'), ('BITE', 'FIRE', '3d12')]
	flags = ['FORCE_MAXHP', 'KILL_BODY', 'FRIENDS', 'DROP_SKELETON', 'WEIRD_MIND', 'BASH_DOOR', 'WILD_DUNGEON_01', 'ANIMAL', 'IM_FIRE']
	sfreq = 0
	sflags = []
	description = """A giant ant covered in shaggy fur.  Its powerful jaws glow with heat.
"""

monsters.register_race(giant_red_ant)


class mature_white_dragon(dragon, spellcaster):
	r_idx = "549"
	name = "Mature white dragon"
	ascii = "d,w"
	level = 35
	rarity = 1
	hp = "35d10"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 70
	exp = 1000
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d8')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_2D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'EVIL', 'DRAGON', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 10
	sflags = ['SCARE', 'BR_COLD']
	description = """A large dragon, scales gleaming bright white.
"""

monsters.register_race(mature_white_dragon)


class xorn(monster):
	r_idx = "550"
	name = "Xorn"
	ascii = "X,o"
	level = 36
	rarity = 2
	hp = "16d7"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 10
	exp = 650
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6')]
	flags = ['FORCE_MAXHP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'KILL_ITEM', 'PASS_WALL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A huge creature of the element Earth.  Able to merge with its element, it 
has four huge arms protruding from its enormous torso.
"""

monsters.register_race(xorn)


class rogrog_the_black_troll(troll):
	r_idx = "551"
	name = "Rogrog the Black Troll"
	ascii = "T,D"
	level = 36
	rarity = 5
	hp = "15d70"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 50
	exp = 5000
	blows = [('HIT', 'HURT', '6d6'), ('BITE', 'HURT', '2d10'), ('BITE', 'HURT', '2d3'), ('SPIT', 'ACID', '3d8')]
	flags = ['UNIQUE', 'MALE', 'WILD_DUNGEON_01', 'FORCE_MAXHP', 'ESCORT', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'DROP_CORPSE', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'IM_COLD', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """A massive and cruel troll of great power, drool slides caustically down 
his muscular frame.  Despite his bulk, he strikes with stunning speed.
"""

monsters.register_race(rogrog_the_black_troll)


class mist_giant(giant):
	r_idx = "552"
	name = "Mist giant"
	ascii = "#,B"
	level = 36
	rarity = 2
	hp = "35d14"
	speed = 120
	aaf = 20
	ac = 25
	sleep = 50
	exp = 450
	blows = [('CRUSH', 'HURT', '4d8'), ('CRUSH', 'HURT', '4d8'), ('CRUSH', 'HURT', '4d8'), ('BITE', 'EXP_40', '3d9')]
	flags = ['FORCE_MAXHP', 'OPEN_DOOR', 'DROP_60', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'SMART', 'BASH_DOOR', 'EVIL', 'GIANT', 'IM_POIS', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """"Two eyes, the colour of a thin, yellow wine, were set high in the 
thing's body; though it had no separate head. A mouthing, obscene slit, 
filled with fangs lay just beneath the eyes. It had no nose or ears... 
Four appendages sprang from its upper parts and its lower body 
slithered along the ground, unsupported by any limbs... incredibly 
disgusting to behold and its amorphous body gave off a stench of death 
and decay..."
"""

monsters.register_race(mist_giant)


class pattern_ghost(monster, undead, spellcaster):
	r_idx = "553"
	name = "Pattern ghost"
	ascii = "G,D"
	level = 36
	rarity = 3
	hp = "14d25"
	speed = 120
	aaf = 30
	ac = 15
	sleep = 20
	exp = 400
	blows = [('TOUCH', 'EXP_80', ''), ('TOUCH', 'EXP_40', ''), ('CLAW', 'LOSE_INT', '1d10'), ('CLAW', 'LOSE_WIS', '1d10')]
	flags = ['FORCE_SLEEP', 'ONLY_ITEM', 'DROP_1D2', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'CAN_FLY', 'WILD_DUNGEON_01', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['FORGET', 'MIND_BLAST']
	description = """An almost life-like creature which is nothing more than a phantom 
created by the Pattern.
"""

monsters.register_race(pattern_ghost)


class grey_wraith(monster, undead, spellcaster):
	r_idx = "554"
	name = "Grey wraith"
	ascii = "W,s"
	level = 36
	rarity = 1
	hp = "12d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 10
	exp = 700
	blows = [('HIT', 'HURT', '1d10'), ('HIT', 'HURT', '1d10'), ('TOUCH', 'EXP_40', '')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'HURT_LITE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 7
	sflags = ['HOLD', 'SCARE', 'CAUSE_3', 'DARKNESS']
	description = """A tangible but ghostly form, made of grey fog.  The air around it feels 
deathly cold.
"""

monsters.register_race(grey_wraith)


class revenant(monster, undead, spellcaster):
	r_idx = "555"
	name = "Revenant"
	ascii = "W,u"
	level = 36
	rarity = 1
	hp = "83d2"
	speed = 125
	aaf = 30
	ac = 35
	sleep = 5
	exp = 800
	blows = [('HIT', 'HURT', '2d10'), ('HIT', 'HURT', '2d10')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'REGENERATE', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'UNDEAD', 'HURT_LITE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BO_FIRE']
	description = """Back from the grave, to wreak vengeance upon the living. A gaunt, tall, 
skeletal figure wearing a ruined plate mail.
"""

monsters.register_race(revenant)


class young_multi_hued_dragon(dragon, spellcaster):
	r_idx = "556"
	name = "Young multi-hued dragon"
	ascii = "d,v"
	level = 36
	rarity = 1
	hp = "24d7"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 50
	exp = 1320
	blows = [('CLAW', 'HURT', '1d9'), ('CLAW', 'HURT', '1d9'), ('BITE', 'HURT', '2d10')]
	flags = ['ATTR_MULTI', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'DROP_1D2', 'DROP_4D2', 'CAN_FLY', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'EVIL', 'DRAGON', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['SCARE', 'BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_POIS']
	description = """It has a form that legends are made of.  Beautiful scales of shimmering 
and magical colours cover it.
"""

monsters.register_race(young_multi_hued_dragon)


class raals_tome_of_destruction(monster, spellcaster):
	r_idx = "557"
	name = "Raal's Tome of Destruction"
	ascii = "?,r"
	level = 36
	rarity = 4
	hp = "38d15"
	speed = 120
	aaf = 20
	ac = 75
	sleep = 15
	exp = 1500
	blows = []
	flags = ['NEVER_MOVE', 'NEVER_BLOW', 'NONLIVING', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'DROP_90', 'DROP_GOOD', 'EVIL', 'COLD_BLOOD', 'EMPTY_MIND', 'FORCE_MAXHP', 'NO_CONF', 'NO_FEAR', 'NO_SLEEP', 'CHAR_MULTI', 'IM_ACID', 'IM_POIS', 'IM_COLD', 'IM_ELEC', 'HURT_FIRE', 'RES_NETH', 'RES_TELE']
	sfreq = 2
	sflags = ['BO_ACID', 'BR_FIRE', 'BO_MANA', 'BR_COLD', 'BR_POIS', 'BO_WATE', 'BA_POIS', 'BR_NETH']
	description = """A sentient arcane tome casting spells with malevolent intent.
"""

monsters.register_race(raals_tome_of_destruction)


class colossus(monster, spellcaster):
	r_idx = "558"
	name = "Colossus"
	ascii = "g,y"
	level = 36
	rarity = 4
	hp = "30d70"
	speed = 110
	aaf = 15
	ac = 75
	sleep = 10
	exp = 900
	blows = [('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6')]
	flags = ['FORCE_MAXHP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NONLIVING', 'REFLECTING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 8
	sflags = ['ARROW_4']
	description = """An enormous construct resembling a titan made from stone.  It strides 
purposefully towards you, swinging its slow fists with earth-shattering 
power.
"""

monsters.register_race(colossus)


class young_gold_dragon(dragon, spellcaster):
	r_idx = "559"
	name = "Young gold dragon"
	ascii = "d,y"
	level = 36
	rarity = 2
	hp = "25d7"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 150
	exp = 950
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d8')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'DROP_60', 'DROP_90', 'DROP_2D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DRAGON']
	sfreq = 11
	sflags = ['SCARE', 'BR_SOUN']
	description = """It has a form that legends are made of.  Its still-tender scales are a 
tarnished gold hue, and light is reflected from its form.
"""

monsters.register_race(young_gold_dragon)


class mature_blue_dragon(dragon, spellcaster):
	r_idx = "560"
	name = "Mature blue dragon"
	ascii = "d,b"
	level = 36
	rarity = 1
	hp = "35d10"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 70
	exp = 1200
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d10')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'DROP_60', 'DROP_90', 'DROP_2D2', 'DROP_CORPSE', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_FOREST2', 'EVIL', 'DRAGON', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['SCARE', 'BR_ELEC']
	description = """A large dragon, scales tinted deep blue.
"""

monsters.register_race(mature_blue_dragon)


class mature_green_dragon(dragon, spellcaster):
	r_idx = "561"
	name = "Mature green dragon"
	ascii = "d,g"
	level = 36
	rarity = 1
	hp = "33d10"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 70
	exp = 1100
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d4'), ('BITE', 'HURT', '1d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'DROP_60', 'DROP_90', 'DROP_2D2', 'DROP_CORPSE', 'BASH_DOOR', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_FOREST2', 'EVIL', 'DRAGON', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['SCARE', 'BR_POIS']
	description = """A large dragon, scales tinted deep green.
"""

monsters.register_race(mature_green_dragon)


class mature_bronze_dragon(dragon, spellcaster):
	r_idx = "562"
	name = "Mature bronze dragon"
	ascii = "d,U"
	level = 36
	rarity = 2
	hp = "40d10"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 150
	exp = 1300
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d10')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_1D2', 'DROP_4D2', 'CAN_FLY', 'BASH_DOOR', 'DROP_CORPSE', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['CONF', 'SCARE', 'BR_CONF']
	description = """A large dragon with scales of rich bronze.
"""

monsters.register_race(mature_bronze_dragon)


class young_red_dragon(dragon, spellcaster):
	r_idx = "563"
	name = "Young red dragon"
	ascii = "d,r"
	level = 36
	rarity = 1
	hp = "25d7"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 50
	exp = 640
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d8')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'DROP_60', 'DROP_90', 'DROP_1D2', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'DRAGON', 'IM_FIRE']
	sfreq = 11
	sflags = ['SCARE', 'BR_FIRE']
	description = """It has a form that legends are made of.  Its still-tender scales are a 
deepest red hue.  Heat radiates from its form.
"""

monsters.register_race(young_red_dragon)


class nightblade(monster):
	r_idx = "564"
	name = "Nightblade"
	ascii = "h,D"
	level = 36
	rarity = 2
	hp = "14d13"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 10
	exp = 315
	blows = [('HIT', 'POISON', '3d4'), ('HIT', 'POISON', '3d4'), ('HIT', 'LOSE_CON', '3d4')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'DROP_1D2', 'FRIENDS', 'INVISIBLE', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'HURT_LITE', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A dark elven assassin, so stealthy that he is almost impossible to see.
"""

monsters.register_race(nightblade)


class trapper(monster):
	r_idx = "565"
	name = "Trapper"
	ascii = ".,w"
	level = 36
	rarity = 3
	hp = "36d10"
	speed = 120
	aaf = 30
	ac = 35
	sleep = 10
	exp = 580
	blows = [('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8'), ('HIT', 'PARALYZE', '15d1'), ('HIT', 'PARALYZE', '15d1')]
	flags = ['CHAR_CLEAR', 'ATTR_CLEAR', 'WILD_DUNGEON_01', 'NEVER_MOVE', 'FORCE_MAXHP', 'INVISIBLE', 'EMPTY_MIND', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """This creature traps unsuspecting victims 
and paralyzes them, to be slowly digested later.
"""

monsters.register_race(trapper)


class bodak(demon, spellcaster):
	r_idx = "566"
	name = "Bodak"
	ascii = "u,D"
	level = 36
	rarity = 2
	hp = "21d10"
	speed = 110
	aaf = 10
	ac = 30
	sleep = 90
	exp = 750
	blows = [('HIT', 'FIRE', '4d6'), ('HIT', 'FIRE', '4d6'), ('GAZE', 'EXP_20', '')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'AURA_FIRE', 'NONLIVING', 'EVIL', 'DEMON', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BO_FIRE', 'BA_FIRE', 'S_DEMON']
	description = """It is a humanoid form composed of flames and hatred.
"""

monsters.register_race(bodak)


class time_bomb(monster):
	r_idx = "567"
	name = "Time bomb"
	ascii = ".,w"
	level = 36
	rarity = 5
	hp = "9d12"
	speed = 130
	aaf = 30
	ac = 20
	sleep = 0
	exp = 50
	blows = [('EXPLODE', 'TIME', '30d2')]
	flags = ['CHAR_CLEAR', 'ATTR_CLEAR', 'WILD_DUNGEON_01', 'NEVER_MOVE', 'FORCE_MAXHP', 'EMPTY_MIND', 'INVISIBLE', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It was left here to be used against intruders.
"""

monsters.register_race(time_bomb)


class mezzodaemon(demon, spellcaster):
	r_idx = "568"
	name = "Mezzodaemon"
	ascii = "u,o"
	level = 36
	rarity = 2
	hp = "30d13"
	speed = 110
	aaf = 10
	ac = 34
	sleep = 90
	exp = 750
	blows = [('CLAW', 'HURT', '5d6'), ('CLAW', 'HURT', '5d6')]
	flags = ['FORCE_SLEEP', 'PASS_WALL', 'INVISIBLE', 'WILD_DUNGEON_01', 'IM_POIS', 'IM_COLD', 'IM_ACID', 'IM_FIRE', 'NO_SLEEP', 'NO_CONF', 'NO_STUN', 'NONLIVING', 'EVIL', 'DEMON']
	sfreq = 4
	sflags = ['BLINK', 'DARKNESS', 'S_DEMON']
	description = """An ugly demon with insect-like extremities and large bulbous eyes.
"""

monsters.register_race(mezzodaemon)


class elder_thing(demon, spellcaster):
	r_idx = "569"
	name = "Elder thing"
	ascii = "u,G"
	level = 36
	rarity = 3
	hp = "26d13"
	speed = 110
	aaf = 10
	ac = 35
	sleep = 50
	exp = 800
	blows = [('CRUSH', 'HURT', '4d6'), ('CRUSH', 'HURT', '4d6'), ('CRUSH', 'HURT', '4d6'), ('TOUCH', 'LOSE_WIS', '')]
	flags = ['FORCE_SLEEP', 'OPEN_DOOR', 'BASH_DOOR', 'NONLIVING', 'WILD_DUNGEON_01', 'DEMON', 'IM_POIS', 'IM_ACID', 'NO_CONF', 'NO_SLEEP', 'RES_TELE', 'CAN_SWIM']
	sfreq = 4
	sflags = ['SCARE', 'TELE_AWAY', 'BA_NUKE', 'CAUSE_4', 'BA_POIS', 'CONF', 'S_DEMON', 'S_UNDEAD']
	description = """"...some ridged barrel-shaped objects with thin 
horizontal arms radiating spoke-like from a central ring and with 
vertical knobs or bulbs projecting from the head and base of the 
barrel. Each of these knobs was the hub of a system of five long, 
flat, triangularly tapering arms arranged around it like the arms 
of a starfish."
"""

monsters.register_race(elder_thing)


class ice_elemental(monster, spellcaster):
	r_idx = "570"
	name = "Ice elemental"
	ascii = "E,w"
	level = 36
	rarity = 2
	hp = "21d10"
	speed = 110
	aaf = 10
	ac = 30
	sleep = 90
	exp = 650
	blows = [('BITE', 'COLD', '1d3'), ('HIT', 'HURT', '4d6'), ('BITE', 'COLD', '1d3')]
	flags = ['FORCE_SLEEP', 'EMPTY_MIND', 'COLD_BLOOD', 'AURA_COLD', 'WILD_DUNGEON_01', 'KILL_ITEM', 'KILL_BODY', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_COLD', 'IM_ELEC', 'CAN_SWIM', 'IM_POIS', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 5
	sflags = ['BO_ICEE', 'BA_COLD']
	description = """It is a towering glacier of ice.
"""

monsters.register_race(ice_elemental)


class ipsissimus(monster, spellcaster):
	r_idx = "571"
	name = "Ipsissimus"
	ascii = "p,D"
	level = 36
	rarity = 2
	hp = "17d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 10
	exp = 666
	blows = [('HIT', 'HURT', '2d6'), ('HIT', 'HURT', '2d6')]
	flags = ['MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_SKELETON', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 3
	sflags = ['HASTE', 'TPORT', 'TELE_TO', 'BLIND', 'HOLD', 'SCARE', 'CAUSE_3', 'BO_NETH', 'MIND_BLAST', 'FORGET', 'S_UNDEAD', 'S_DEMON', 'ANIM_DEAD']
	description = """A gaunt figure, clothed in black robes.
"""

monsters.register_race(ipsissimus)


class the_greater_hell_magic_mushroom_were_quylthulg(demon, spellcaster):
	r_idx = "572"
	name = "The Greater hell magic mushroom were-quylthulg"
	ascii = "Q,s"
	level = 36
	rarity = 3
	hp = "19d70"
	speed = 120
	aaf = 50
	ac = 40
	sleep = 50
	exp = 2500
	blows = [('GAZE', 'EXP_VAMP', '4d8'), ('GAZE', 'EXP_VAMP', '4d8'), ('CRUSH', 'ACID', '8d8'), ('CRUSH', 'ACID', '8d8')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'FORCE_SLEEP', 'UNIQUE', 'NO_STUN', 'NO_CONF', 'NO_SLEEP', 'EVIL', 'IM_ACID', 'IM_ELEC', 'IM_FIRE', 'IM_POIS', 'IM_COLD', 'RES_NETH', 'RES_WATE', 'RES_PLAS', 'RES_DISE', 'SMART', 'RES_NEXU', 'NONLIVING', 'RES_TELE', 'KILL_WALL', 'ELDRITCH_HORROR', 'BASH_DOOR', 'DEMON', 'COLD_BLOOD', 'ANIMAL', 'CAN_SWIM', 'DROP_GOOD', 'DROP_GREAT', 'ONLY_ITEM', 'DROP_2D2']
	sfreq = 3
	sflags = ['BLINK', 'SLOW', 'SCARE', 'DARKNESS', 'HEAL', 'ANIM_DEAD', 'TPORT', 'TELE_AWAY', 'HASTE', 'S_MONSTER', 'DRAIN_MANA', 'S_UNDEAD', 'S_DEMON', 'S_DRAGON', 'S_KIN']
	description = """This unholy abomination will crush you too. Flee while you can! 
"""

monsters.register_race(the_greater_hell_magic_mushroom_were_quylthulg)


class lord_borel_of_hendrake(monster, spellcaster):
	r_idx = "573"
	name = "Lord Borel of Hendrake"
	ascii = "p,v"
	level = 36
	rarity = 2
	hp = "18d70"
	speed = 120
	aaf = 25
	ac = 50
	sleep = 10
	exp = 1200
	blows = [('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['TELE_TO', 'S_KIN']
	description = """A Lord of Chaos famous for his skill at arms.
"""

monsters.register_race(lord_borel_of_hendrake)


class chaos_spawn(monster):
	r_idx = "574"
	name = "Chaos spawn"
	ascii = "e,u"
	level = 36
	rarity = 2
	hp = "20d26"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 20
	exp = 600
	blows = [('GAZE', 'HURT', '10d10'), ('GAZE', 'UN_BONUS', '5d2'), ('GAZE', 'EXP_VAMP', '5d2'), ('GAZE', 'PARALYZE', '5d2')]
	flags = ['WILD_DUNGEON_01', 'FORCE_MAXHP', 'BASH_DOOR', 'EVIL', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """It has two eyestalks and a large central eye. Its gaze can kill.
"""

monsters.register_race(chaos_spawn)


class mummified_troll(troll, undead):
	r_idx = "575"
	name = "Mummified troll"
	ascii = "z,w"
	level = 37
	rarity = 1
	hp = "16d12"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 50
	exp = 420
	blows = [('HIT', 'HURT', '2d6'), ('HIT', 'HURT', '2d6')]
	flags = ['FORCE_MAXHP', 'DROP_60', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'TROLL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a massive figure clothed in wrappings.  You are wary of its massive 
fists.
"""

monsters.register_race(mummified_troll)


class fire_angel(dragon, spellcaster):
	r_idx = "576"
	name = "Fire angel"
	ascii = "d,y"
	level = 37
	rarity = 3
	hp = "50d7"
	speed = 120
	aaf = 10
	ac = 50
	sleep = 25
	exp = 1500
	blows = [('CLAW', 'HURT', '3d8'), ('CLAW', 'HURT', '3d8'), ('BITE', 'HURT', '5d5'), ('BITE', 'HURT', '5d5')]
	flags = ['WILD_DUNGEON_01', 'EVIL', 'DRAGON', 'SMART', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'IM_FIRE', 'DROP_4D2', 'BASH_DOOR', 'RES_PLAS', 'CAN_FLY', 'DROP_CORPSE']
	sfreq = 9
	sflags = ['BR_PLAS']
	description = """It is a fast, deadly dragonoid creature. It was bred and trained 
in the Courts of Chaos for assassination.
"""

monsters.register_race(fire_angel)


class crypt_thing(monster, undead, spellcaster):
	r_idx = "577"
	name = "Crypt thing"
	ascii = "L,B"
	level = 37
	rarity = 3
	hp = "120d5"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 60
	exp = 2000
	blows = [('TOUCH', 'EXP_40', ''), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'LOSE_DEX', '2d10'), ('TOUCH', 'LOSE_DEX', '2d10')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_1D2', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLINK', 'TELE_TO', 'TELE_AWAY', 'TELE_LEVEL', 'CAUSE_3', 'DRAIN_MANA', 'BRAIN_SMASH']
	description = """It is a skeletal form dressed in robes. 
It looks evil and devious...
"""

monsters.register_race(crypt_thing)


class chaos_butterfly(monster, spellcaster):
	r_idx = "578"
	name = "Chaos butterfly"
	ascii = "I,G"
	level = 37
	rarity = 2
	hp = "60d7"
	speed = 120
	aaf = 40
	ac = 30
	sleep = 10
	exp = 1000
	blows = [('CLAW', 'HURT', '3d7'), ('CLAW', 'HURT', '3d7'), ('CRUSH', 'HURT', '10d5')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WEIRD_MIND', 'BASH_DOOR', 'ATTR_MULTI', 'ATTR_ANY', 'NO_CONF', 'NO_SLEEP', 'EVIL']
	sfreq = 9
	sflags = ['BR_CONF', 'BR_CHAO']
	description = """"It had the appearance of a butterfly, but but a butterfly with 
wings so huge they blotted out the sun... it had a man's body, 
covered with hairs or feathers hued like a peacocks..."
"""

monsters.register_race(chaos_butterfly)


class time_elemental(monster, spellcaster):
	r_idx = "579"
	name = "Time elemental"
	ascii = "E,G"
	level = 37
	rarity = 2
	hp = "35d7"
	speed = 120
	aaf = 90
	ac = 35
	sleep = 10
	exp = 1000
	blows = [('TOUCH', 'TIME', '3d4'), ('TOUCH', 'TIME', '3d4')]
	flags = ['WILD_DUNGEON_01', 'PASS_WALL', 'IM_POIS', 'IM_FIRE', 'IM_ELEC', 'NONLIVING', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP', 'EVIL', 'EMPTY_MIND', 'KILL_ITEM', 'RAND_50']
	sfreq = 7
	sflags = ['SLOW', 'BR_TIME']
	description = """You have not seen it yet.
"""

monsters.register_race(time_elemental)


class flying_polyp(monster, spellcaster):
	r_idx = "580"
	name = "Flying polyp"
	ascii = "l,R"
	level = 37
	rarity = 2
	hp = "35d7"
	speed = 120
	aaf = 90
	ac = 35
	sleep = 10
	exp = 1000
	blows = [('CRUSH', 'PARALYZE', '8d4'), ('CRUSH', 'PARALYZE', '8d4'), ('CRUSH', 'PARALYZE', '8d4')]
	flags = ['WILD_DUNGEON_01', 'PASS_WALL', 'INVISIBLE', 'FORCE_MAXHP', 'RES_DISE', 'IM_POIS', 'IM_COLD', 'IM_ACID', 'ELDRITCH_HORROR', 'NO_CONF', 'NO_SLEEP', 'EVIL', 'CAN_FLY']
	sfreq = 7
	sflags = ['BR_WALL']
	description = """"They were only partly material and had the power of aerial motion, 
despite the absence of wings... Suggestions of monstrous plasticity 
and of temporary lapses of visibility..."
"""

monsters.register_race(flying_polyp)


class the_queen_ant(monster, spellcaster):
	r_idx = "581"
	name = "The Queen Ant"
	ascii = "a,v"
	level = 37
	rarity = 2
	hp = "9d100"
	speed = 120
	aaf = 30
	ac = 45
	sleep = 10
	exp = 1000
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d8'), ('BITE', 'HURT', '2d8')]
	flags = ['UNIQUE', 'FEMALE', 'GOOD', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ESCORT', 'ESCORTS', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'WEIRD_MIND', 'OPEN_DOOR', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['S_ANT']
	description = """She's upset because you hurt her children.
"""

monsters.register_race(the_queen_ant)


class will_o_the_wisp(monster, spellcaster):
	r_idx = "582"
	name = "Will o' the wisp"
	ascii = "E,y"
	level = 37
	rarity = 4
	hp = "12d10"
	speed = 130
	aaf = 30
	ac = 60
	sleep = 0
	exp = 500
	blows = [('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d9'), ('HIT', 'HURT', '1d9')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_50', 'SMART', 'EMPTY_MIND', 'INVISIBLE', 'PASS_WALL', 'POWERFUL', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_SWAMP1', 'WILD_SWAMP2', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 2
	sflags = ['BLINK', 'TPORT', 'CONF', 'CAUSE_2']
	description = """A strange ball of glowing light.  It disappears and reappears and seems to 
draw you to it.  You seem somehow compelled to stand still and watch its 
strange dancing motion.
"""

monsters.register_race(will_o_the_wisp)


class shan(monster, spellcaster):
	r_idx = "583"
	name = "Shan"
	ascii = "I,B"
	level = 37
	rarity = 4
	hp = "14d10"
	speed = 120
	aaf = 20
	ac = 60
	sleep = 20
	exp = 250
	blows = []
	flags = ['IM_POIS', 'IM_COLD', 'COLD_BLOOD', 'ANIMAL', 'EVIL', 'WILD_DUNGEON_01', 'NO_SLEEP', 'NO_CONF', 'CAN_FLY', 'NEVER_BLOW']
	sfreq = 2
	sflags = ['CONF', 'HOLD', 'DRAIN_MANA', 'FORGET', 'MIND_BLAST', 'SHRIEK']
	description = """"Those huge lidless eyes which stared with hate at me, the jointed 
tendrils which seemed to twist from the head in cosmic rhythms, 
the ten legs, covered with black shining tentacles and folded into 
the pallid underbelly, and the semi-circular ridged wings covered 
with triangular scales -- all this cannot convey the soul-ripping 
horror of the shape which darted at me. I saw the three mouths 
of the thing move moistly, and then it was upon me."
"""

monsters.register_race(shan)


class magma_elemental(monster, spellcaster):
	r_idx = "584"
	name = "Magma elemental"
	ascii = "E,o"
	level = 37
	rarity = 2
	hp = "21d10"
	speed = 110
	aaf = 10
	ac = 30
	sleep = 90
	exp = 950
	blows = [('HIT', 'FIRE', '3d7'), ('HIT', 'HURT', '4d6'), ('HIT', 'FIRE', '3d7')]
	flags = ['FORCE_SLEEP', 'EMPTY_MIND', 'AURA_FIRE', 'WILD_DUNGEON_01', 'KILL_ITEM', 'KILL_BODY', 'PASS_WALL', 'POWERFUL', 'EVIL', 'IM_FIRE', 'IM_ELEC', 'NONLIVING', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 7
	sflags = ['BO_PLAS', 'BA_FIRE']
	description = """It is a towering glowing form of molten hate.
"""

monsters.register_race(magma_elemental)


class black_pudding(monster):
	r_idx = "585"
	name = "Black pudding"
	ascii = "j,D"
	level = 37
	rarity = 5
	hp = "24d10"
	speed = 110
	aaf = 12
	ac = 12
	sleep = 1
	exp = 36
	blows = [('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10'), ('TOUCH', 'ACID', '1d10')]
	flags = ['FORCE_MAXHP', 'FRIENDS', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'DROP_1D2', 'STUPID', 'EMPTY_MIND', 'COLD_BLOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'CAN_SWIM']
	sfreq = 0
	sflags = []
	description = """A lump of rotting black flesh that slurrrrrrrps across the dungeon floor.
"""

monsters.register_race(black_pudding)


class killer_iridescent_beetle(monster):
	r_idx = "586"
	name = "Killer iridescent beetle"
	ascii = "K,v"
	level = 37
	rarity = 2
	hp = "15d10"
	speed = 110
	aaf = 16
	ac = 24
	sleep = 30
	exp = 850
	blows = [('CLAW', 'ELEC', '1d12'), ('CLAW', 'ELEC', '1d12'), ('GAZE', 'PARALYZE', '')]
	flags = ['ATTR_MULTI', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'FORCE_MAXHP', 'WEIRD_MIND', 'BASH_DOOR', 'AURA_ELEC', 'DROP_CORPSE', 'ANIMAL', 'IM_ELEC', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """It is a giant beetle, whose carapace shimmers with vibrant energies.
"""

monsters.register_race(killer_iridescent_beetle)


class nexus_vortex(monster, spellcaster):
	r_idx = "587"
	name = "Nexus vortex"
	ascii = "v,v"
	level = 37
	rarity = 1
	hp = "19d10"
	speed = 120
	aaf = 100
	ac = 15
	sleep = 0
	exp = 800
	blows = [('ENGULF', 'HURT', '5d5')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'RES_NEXU', 'CAN_FLY', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_NEXU']
	description = """A maelstrom of potent magical energy.
"""

monsters.register_race(nexus_vortex)


class plasma_vortex(monster, spellcaster):
	r_idx = "588"
	name = "Plasma vortex"
	ascii = "v,R"
	level = 37
	rarity = 1
	hp = "19d10"
	speed = 120
	aaf = 100
	ac = 15
	sleep = 0
	exp = 800
	blows = [('ENGULF', 'FIRE', '8d8')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'RES_PLAS', 'AURA_FIRE', 'AURA_ELEC', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'IM_FIRE', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_PLAS']
	description = """A whirlpool of intense flame, charring the stones at your feet.
"""

monsters.register_race(plasma_vortex)


class mature_red_dragon(dragon, spellcaster):
	r_idx = "589"
	name = "Mature red dragon"
	ascii = "d,r"
	level = 37
	rarity = 1
	hp = "38d10"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 30
	exp = 1400
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d10'), ('BITE', 'HURT', '2d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'DROP_1D2', 'DROP_4D2', 'CAN_FLY', 'DROP_CORPSE', 'BASH_DOOR', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['CONF', 'SCARE', 'BR_FIRE']
	description = """A large dragon, scales tinted deep red.
"""

monsters.register_race(mature_red_dragon)


class mature_gold_dragon(dragon, spellcaster):
	r_idx = "590"
	name = "Mature gold dragon"
	ascii = "d,y"
	level = 37
	rarity = 2
	hp = "40d10"
	speed = 110
	aaf = 20
	ac = 45
	sleep = 150
	exp = 1500
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d10'), ('BITE', 'HURT', '2d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_1D2', 'DROP_4D2', 'CAN_FLY', 'DROP_CORPSE', 'BASH_DOOR', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['CONF', 'SCARE', 'BR_SOUN']
	description = """A large dragon with scales of gleaming gold.
"""

monsters.register_race(mature_gold_dragon)


class crystal_drake(dragon, spellcaster):
	r_idx = "591"
	name = "Crystal drake"
	ascii = "d,B"
	level = 37
	rarity = 2
	hp = "45d10"
	speed = 120
	aaf = 25
	ac = 45
	sleep = 30
	exp = 1500
	blows = [('CLAW', 'HURT', '1d4'), ('CLAW', 'HURT', '1d4'), ('BITE', 'HURT', '2d5')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_4D2', 'REFLECTING', 'WILD_DUNGEON_01', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SLOW', 'CONF', 'SCARE', 'BLIND', 'BR_SHAR']
	description = """A dragon of strange crystalline form.  Light shines through it, dazzling 
your eyes with spectrums of colour.
"""

monsters.register_race(crystal_drake)


class mature_black_dragon(dragon, spellcaster):
	r_idx = "592"
	name = "Mature black dragon"
	ascii = "d,D"
	level = 37
	rarity = 1
	hp = "35d10"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 30
	exp = 1350
	blows = [('CLAW', 'HURT', '1d8'), ('CLAW', 'HURT', '1d8'), ('BITE', 'HURT', '2d10')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'DROP_CORPSE', 'DROP_60', 'DROP_90', 'DROP_2D2', 'WILD_DUNGEON_01', 'WILD_SWAMP2', 'BASH_DOOR', 'EVIL', 'DRAGON', 'IM_ACID', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['SCARE', 'BR_ACID']
	description = """A large dragon, with scales of deepest black.
"""

monsters.register_race(mature_black_dragon)


class mature_multi_hued_dragon(dragon, spellcaster):
	r_idx = "593"
	name = "Mature multi-hued dragon"
	ascii = "d,v"
	level = 38
	rarity = 2
	hp = "40d10"
	speed = 110
	aaf = 20
	ac = 40
	sleep = 50
	exp = 1700
	blows = [('CLAW', 'HURT', '1d10'), ('CLAW', 'HURT', '1d10'), ('BITE', 'HURT', '2d12')]
	flags = ['ATTR_MULTI', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_3D2', 'DROP_4D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DRAGON', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_POIS']
	description = """A large dragon, scales shimmering many colours.
"""

monsters.register_race(mature_multi_hued_dragon)


class sky_whale(monster, spellcaster):
	r_idx = "594"
	name = "Sky whale"
	ascii = "l,B"
	level = 38
	rarity = 6
	hp = "60d10"
	speed = 110
	aaf = 20
	ac = 37
	sleep = 30
	exp = 1750
	blows = [('CRUSH', 'HURT', '20d2'), ('CRUSH', 'HURT', '20d2'), ('CRUSH', 'HURT', '20d2')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'DROP_60', 'DROP_90', 'DROP_2D2', 'DROP_CORPSE', 'BASH_DOOR', 'SMART', 'GOOD', 'NO_CONF', 'NO_SLEEP', 'RES_NEXU', 'RES_TELE', 'RES_DISE']
	sfreq = 9
	sflags = ['BRAIN_SMASH', 'CONF', 'SCARE', 'FORGET', 'TELE_TO', 'TELE_AWAY', 'SHRIEK']
	description = """A vastly intelligent whale from the stars.
"""

monsters.register_race(sky_whale)


class father_dagon(demon, spellcaster):
	r_idx = "595"
	name = "Father Dagon"
	ascii = "u,g"
	level = 38
	rarity = 3
	hp = "19d75"
	speed = 120
	aaf = 12
	ac = 40
	sleep = 50
	exp = 3250
	blows = [('CLAW', 'POISON', '8d4'), ('CLAW', 'POISON', '8d4'), ('BITE', 'HURT', '8d8')]
	flags = ['ESCORT', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'DROP_1D2', 'DROP_GOOD', 'ONLY_ITEM', 'DROP_CORPSE', 'CAN_SWIM', 'BASH_DOOR', 'ELDRITCH_HORROR', 'RES_TELE', 'CAN_SPEAK', 'EVIL', 'DEMON', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'RES_WATE', 'UNIQUE', 'MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'POWERFUL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 7
	sflags = ['S_KIN', 'S_DEMON', 'BO_WATE', 'BA_WATE', 'BO_ACID', 'BA_ACID']
	description = """The king of the deep ones. "Vast, Polyphemous-like, and loathsome, it 
darted like a stupendous monster of nightmares..."
"""

monsters.register_race(father_dagon)


class mother_hydra(demon, spellcaster):
	r_idx = "596"
	name = "Mother Hydra"
	ascii = "u,g"
	level = 38
	rarity = 3
	hp = "19d75"
	speed = 120
	aaf = 12
	ac = 80
	sleep = 50
	exp = 3250
	blows = [('CLAW', 'POISON', '8d4'), ('CLAW', 'POISON', '8d4'), ('BITE', 'HURT', '8d8')]
	flags = ['ESCORT', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'DROP_1D2', 'DROP_GOOD', 'ONLY_ITEM', 'DROP_CORPSE', 'CAN_SWIM', 'BASH_DOOR', 'ELDRITCH_HORROR', 'RES_TELE', 'CAN_SPEAK', 'EVIL', 'DEMON', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'RES_WATE', 'UNIQUE', 'FEMALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'POWERFUL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 7
	sflags = ['S_HYDRA', 'S_DEMON', 'DARKNESS', 'BA_WATE', 'BO_ACID', 'BA_ACID']
	description = """The queen of the deep ones. "Vast, Polyphemous-like, and loathsome, it 
darted like a stupendous monster of nightmares..."
"""

monsters.register_race(mother_hydra)


class death_knight(monster, spellcaster):
	r_idx = "597"
	name = "Death knight"
	ascii = "p,s"
	level = 38
	rarity = 1
	hp = "36d10"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 10
	exp = 1111
	blows = [('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'EXP_20', '')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'RES_NETH', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_COLD']
	sfreq = 5
	sflags = ['BLIND', 'SCARE', 'CAUSE_3', 'BO_NETH', 'S_MONSTERS']
	description = """It is a humanoid form dressed in armour of an ancient form.  From beneath 
its helmet, eyes glow a baleful red and seem to pierce you like lances of 
fire.
"""

monsters.register_race(death_knight)


class mandor_master_of_the_logrus(monster, spellcaster):
	r_idx = "598"
	name = "Mandor, Master of the Logrus"
	ascii = "p,v"
	level = 38
	rarity = 5
	hp = "53d10"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 40
	exp = 1600
	blows = [('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'UN_POWER', '5d5'), ('HIT', 'UN_BONUS', '5d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'SMART', 'OPEN_DOOR', 'TAKE_ITEM', 'BASH_DOOR', 'EVIL', 'RES_TELE']
	sfreq = 2
	sflags = ['BO_FIRE', 'BO_COLD', 'HOLD', 'BO_MANA', 'S_MONSTER', 'TRAPS', 'BO_ICEE', 'HEAL', 'BO_PLAS', 'BA_CHAO']
	description = """Mandor is one of the greatest Logrus Masters, a formidable magician.
"""

monsters.register_race(mandor_master_of_the_logrus)


class time_vortex(monster, spellcaster):
	r_idx = "599"
	name = "Time vortex"
	ascii = "v,G"
	level = 38
	rarity = 4
	hp = "19d10"
	speed = 130
	aaf = 100
	ac = 15
	sleep = 0
	exp = 900
	blows = [('ENGULF', 'HURT', '5d5')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_TIME']
	description = """You haven't seen it yet.
"""

monsters.register_race(time_vortex)


class shimmering_vortex(monster, spellcaster):
	r_idx = "600"
	name = "Shimmering vortex"
	ascii = "v,v"
	level = 38
	rarity = 4
	hp = "4d12"
	speed = 140
	aaf = 100
	ac = 15
	sleep = 0
	exp = 200
	blows = []
	flags = ['ATTR_MULTI', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'NEVER_BLOW', 'RAND_50', 'RAND_25', 'CAN_FLY', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 4
	sflags = ['BR_LITE', 'SHRIEK']
	description = """A strange pillar of shining light that hurts your eyes.  Its shape changes 
constantly as it cuts through the air towards you.  It is like a beacon, 
waking monsters from their slumber.
"""

monsters.register_race(shimmering_vortex)


class ancient_blue_dragon(dragon, spellcaster):
	r_idx = "601"
	name = "Ancient blue dragon"
	ascii = "D,b"
	level = 38
	rarity = 1
	hp = "42d15"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 80
	exp = 1500
	blows = [('CLAW', 'HURT', '4d8'), ('CLAW', 'HURT', '4d8'), ('BITE', 'ELEC', '5d8')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_1D2', 'DROP_4D2', 'DROP_CORPSE', 'SMART', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_ELEC']
	description = """A huge draconic form.  Lightning crackles along its length.
"""

monsters.register_race(ancient_blue_dragon)


class ancient_bronze_dragon(dragon, spellcaster):
	r_idx = "602"
	name = "Ancient bronze dragon"
	ascii = "D,U"
	level = 38
	rarity = 2
	hp = "44d15"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 200
	exp = 1700
	blows = [('CLAW', 'HURT', '4d8'), ('CLAW', 'HURT', '4d8'), ('BITE', 'CONFUSE', '5d10')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_3D2', 'DROP_4D2', 'CAN_FLY', 'SMART', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_CONF']
	description = """A huge draconic form enveloped in a cascade of colour.
"""

monsters.register_race(ancient_bronze_dragon)


class beholder(monster, spellcaster):
	r_idx = "603"
	name = "Beholder"
	ascii = "e,g"
	level = 38
	rarity = 4
	hp = "16d70"
	speed = 120
	aaf = 30
	ac = 40
	sleep = 10
	exp = 4000
	blows = [('GAZE', 'EXP_20', '2d4'), ('GAZE', 'PARALYZE', '2d4'), ('GAZE', 'LOSE_INT', '2d6'), ('GAZE', 'UN_POWER', '2d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'CAN_FLY', 'BASH_DOOR', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'EVIL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'SLOW', 'CONF', 'SCARE', 'DRAIN_MANA', 'MIND_BLAST', 'FORGET', 'DARKNESS', 'BO_ACID', 'BO_FIRE', 'BO_COLD']
	description = """A disembodied eye, surrounded by twelve smaller eyes on stalks.
"""

monsters.register_race(beholder)


class emperor_wight(monster, undead, spellcaster):
	r_idx = "604"
	name = "Emperor wight"
	ascii = "W,y"
	level = 38
	rarity = 2
	hp = "23d10"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 10
	exp = 1600
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12'), ('TOUCH', 'EXP_80', ''), ('TOUCH', 'EXP_80', '')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_90', 'DROP_4D2', 'CAN_FLY', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['HOLD', 'SCARE', 'CAUSE_3', 'BO_NETH']
	description = """Your life force is torn from your body as this powerful unearthly being 
approaches.
"""

monsters.register_race(emperor_wight)


class seraph(monster, spellcaster):
	r_idx = "605"
	name = "Seraph"
	ascii = "A,w"
	level = 38
	rarity = 6
	hp = "50d7"
	speed = 120
	aaf = 30
	ac = 34
	sleep = 255
	exp = 1800
	blows = [('HIT', 'HURT', '4d6'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '4d6')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'NO_FEAR', 'GOOD', 'CAN_FLY', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'REFLECTING', 'RES_TELE', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 11
	sflags = ['HEAL', 'HASTE', 'TELE_AWAY', 'CONF', 'BO_MANA', 'BO_PLAS', 'S_MONSTERS', 'S_ANGEL']
	description = """It is an angel, fast and strong.  You are stunned by its extreme holiness 
and try to resist all desires to obey it.
"""

monsters.register_race(seraph)


class loge_spirit_of_fire(monster, spellcaster):
	r_idx = "606"
	name = "Loge, Spirit of Fire"
	ascii = "E,R"
	level = 38
	rarity = 3
	hp = "15d70"
	speed = 120
	aaf = 12
	ac = 25
	sleep = 50
	exp = 3000
	blows = [('HIT', 'FIRE', '6d6'), ('HIT', 'FIRE', '6d6'), ('HIT', 'FIRE', '6d6'), ('HIT', 'FIRE', '6d6')]
	flags = ['UNIQUE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_25', 'EMPTY_MIND', 'CAN_SPEAK', 'MALE', 'AURA_FIRE', 'KILL_ITEM', 'KILL_BODY', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_FIRE', 'NONLIVING', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BO_PLAS', 'BA_FIRE']
	description = """A towering fire elemental, Loge burns everything beyond recognition.
"""

monsters.register_race(loge_spirit_of_fire)


class black_wraith(monster, undead, spellcaster):
	r_idx = "607"
	name = "Black wraith"
	ascii = "W,D"
	level = 38
	rarity = 2
	hp = "30d10"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 10
	exp = 1700
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12'), ('TOUCH', 'EXP_40', ''), ('TOUCH', 'EXP_40', '')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 7
	sflags = ['BLIND', 'HOLD', 'SCARE', 'CAUSE_3', 'BO_NETH']
	description = """A figure that seems made of void, its strangely human shape is cloaked in 
shadow.  It reaches out at you.
"""

monsters.register_race(black_wraith)


class nightgaunt(demon, spellcaster):
	r_idx = "608"
	name = "Nightgaunt"
	ascii = "U,D"
	level = 38
	rarity = 2
	hp = "24d7"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 80
	exp = 1000
	blows = [('STING', 'LOSE_STR', '1d5'), ('TOUCH', 'PARALYZE', '3d4')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_60', 'ELDRITCH_HORROR', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'EVIL', 'DEMON', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 7
	sflags = ['BLIND', 'CONF', 'BO_FIRE']
	description = """"Shocking and uncouth black things with smooth, oily, whale-like 
surfaces, unpleasant horns that curved inward toward each other, 
bat wings whose beating made no sound, ugly prehensile paws, and 
barbed tails that lashed needlessly and disquietingly. And worst 
of all, they never spoke or laughed, and never smiled because they 
had no faces at all to smile with, but only to a suggestive 
blankness where a face ought to be."
"""

monsters.register_race(nightgaunt)


class baron_of_hell(demon, spellcaster):
	r_idx = "609"
	name = "Baron of hell"
	ascii = "U,U"
	level = 38
	rarity = 3
	hp = "113d12"
	speed = 110
	aaf = 10
	ac = 65
	sleep = 40
	exp = 1000
	blows = [('CLAW', 'HURT', '11d2'), ('CLAW', 'HURT', '11d2'), ('CLAW', 'HURT', '22d1')]
	flags = ['WILD_DUNGEON_01', 'IM_POIS', 'OPEN_DOOR', 'BASH_DOOR', 'MALE', 'RES_PLAS', 'NONLIVING', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP', 'EVIL', 'DEMON', 'FORCE_MAXHP', 'RES_TELE']
	sfreq = 2
	sflags = ['BO_PLAS']
	description = """A minor demon lord with a goat's head, tough to kill.
"""

monsters.register_race(baron_of_hell)


class scylla(monster, spellcaster):
	r_idx = "610"
	name = "Scylla"
	ascii = "M,G"
	level = 39
	rarity = 2
	hp = "70d15"
	speed = 125
	aaf = 20
	ac = 45
	sleep = 20
	exp = 3500
	blows = [('BITE', 'POISON', '10d3'), ('BITE', 'POISON', '10d3'), ('BITE', 'POISON', '10d3'), ('BITE', 'POISON', '10d3')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'UNIQUE', 'CAN_SWIM', 'FORCE_MAXHP', 'ONLY_GOLD', 'DROP_2D2', 'DROP_4D2', 'ESCORT', 'DROP_CORPSE', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'WILD_SHORE', 'ANIMAL', 'IM_POIS']
	sfreq = 5
	sflags = ['BR_POIS', 'BR_FIRE', 'SCARE', 'CONF']
	description = """A strange reptilian hybrid with multiple heads dripping venom.
"""

monsters.register_race(scylla)


class monastic_lich(monster, undead, spellcaster):
	r_idx = "611"
	name = "Monastic lich"
	ascii = "L,u"
	level = 39
	rarity = 2
	hp = "12d70"
	speed = 120
	aaf = 30
	ac = 40
	sleep = 30
	exp = 5000
	blows = [('KICK', 'HURT', '24d1'), ('HIT', 'HURT', '24d1'), ('CLAW', 'EXP_80', '4d2'), ('CLAW', 'LOSE_DEX', '4d2')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'RES_TELE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLINK', 'TELE_TO', 'BLIND', 'HOLD', 'CONF', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'ANIM_DEAD']
	description = """A skeletal form wrapped in priestly robes.  Before its un-death, it 
was a monk in an evil order...
"""

monsters.register_race(monastic_lich)


class nether_wraith(monster, undead, spellcaster):
	r_idx = "612"
	name = "Nether wraith"
	ascii = "W,R"
	level = 39
	rarity = 2
	hp = "29d10"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 10
	exp = 1700
	blows = [('HIT', 'HURT', '1d12'), ('HIT', 'HURT', '1d12'), ('TOUCH', 'EXP_80', ''), ('TOUCH', 'EXP_80', '')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'ONLY_ITEM', 'DROP_90', 'DROP_4D2', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['BLIND', 'SCARE', 'CAUSE_3', 'MIND_BLAST', 'DARKNESS', 'BO_NETH']
	description = """A form that hurts the eye, death permeates the air around it.  As it nears 
you, a coldness saps your soul.
"""

monsters.register_race(nether_wraith)


class fire_vampire(monster, spellcaster):
	r_idx = "613"
	name = "Fire vampire"
	ascii = "V,r"
	level = 39
	rarity = 4
	hp = "52d9"
	speed = 120
	aaf = 30
	ac = 66
	sleep = 0
	exp = 1200
	blows = [('TOUCH', 'FIRE', '1d10'), ('TOUCH', 'LOSE_WIS', '1d10'), ('TOUCH', 'FIRE', '1d10'), ('TOUCH', 'LOSE_INT', '1d10')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_25', 'CAN_FLY', 'EVIL', 'SMART', 'EMPTY_MIND', 'AURA_FIRE', 'RES_PLAS', 'KILL_ITEM', 'PASS_WALL', 'IM_ACID', 'IM_FIRE', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING', 'NO_STUN']
	sfreq = 4
	sflags = ['BLINK', 'TPORT', 'CONF', 'BA_FIRE', 'DRAIN_MANA', 'HASTE', 'BO_PLAS']
	description = """It is a living entity of fire. Fire springs up wherever this 
creature touches.
"""

monsters.register_race(fire_vampire)


class seven_headed_hydra(monster, spellcaster):
	r_idx = "614"
	name = "7-headed hydra"
	ascii = "M,G"
	level = 39
	rarity = 2
	hp = "100d7"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 20
	exp = 2000
	blows = [('BITE', 'POISON', '3d9'), ('BITE', 'POISON', '3d9'), ('BITE', 'POISON', '3d9'), ('SPIT', 'BLIND', '1d2')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_SWAMP1', 'WILD_SWAMP2', 'ONLY_GOLD', 'DROP_2D2', 'DROP_4D2', 'CAN_SWIM', 'DROP_CORPSE', 'BASH_DOOR', 'MOVE_BODY', 'ANIMAL', 'IM_POIS']
	sfreq = 5
	sflags = ['SCARE', 'BA_POIS', 'BR_POIS']
	description = """A strange reptilian hybrid with seven heads dripping venom.
"""

monsters.register_race(seven_headed_hydra)


class moire_queen_of_rebma(monster, spellcaster):
	r_idx = "615"
	name = "Moire, Queen of Rebma"
	ascii = "E,b"
	level = 39
	rarity = 3
	hp = "14d100"
	speed = 120
	aaf = 12
	ac = 40
	sleep = 50
	exp = 3250
	blows = [('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['UNIQUE', 'FEMALE', 'DROP_1D2', 'DROP_2D2', 'ONLY_ITEM', 'DROP_GOOD', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_25', 'DROP_CORPSE', 'CAN_SPEAK', 'CAN_SWIM', 'WILD_OCEAN', 'KILL_ITEM', 'KILL_BODY', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_SWAMP1', 'WILD_SWAMP2', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 4
	sflags = ['BO_ICEE', 'BO_WATE', 'BA_COLD', 'BA_WATE']
	description = """The Queen of the Sea where the Pattern is reflected is a cunning 
diplomat and a dangerous foe. "A woman sat upon the throne... 
and her hair was green, though streaked with silver, and her eyes 
were round of moons of jade and her brows rose like the wings of 
olive gulls. Her mouth was small, her chin was small; her cheeks 
were high and wide and rounded. A circlet of white gold crossed 
her brow and there was a crystal necklace about her neck. At its 
tip there flashed a sapphire between her sweet bare breasts, whose 
nipples were also bare green. She wore scaled trunks of blue and 
a silver belt, and she held a scepter of pink coral in her hand 
and had a ring upon every finger, and each ring had a stone of 
a different blue within it. She did not smile..."
"""

monsters.register_race(moire_queen_of_rebma)


class kavlax_the_many_headed(dragon, spellcaster):
	r_idx = "616"
	name = "Kavlax the Many-Headed"
	ascii = "d,v"
	level = 39
	rarity = 3
	hp = "8d100"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 30
	exp = 3000
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'ATTR_MULTI', 'CAN_SPEAK', 'ATTR_ANY', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'RES_NEXU', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_SOUN', 'BR_CONF', 'BR_SHAR', 'BR_GRAV', 'BR_NEXU']
	description = """A large dragon with a selection of heads, all shouting and arguing as they 
look for prey, but each with its own deadly breath weapon.
"""

monsters.register_race(kavlax_the_many_headed)


class ancient_white_dragon(dragon, spellcaster):
	r_idx = "617"
	name = "Ancient white dragon"
	ascii = "D,w"
	level = 39
	rarity = 1
	hp = "42d15"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 80
	exp = 2500
	blows = [('CLAW', 'HURT', '4d9'), ('CLAW', 'HURT', '4d9'), ('BITE', 'COLD', '5d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'WILD_MOUNT2', 'DROP_3D2', 'DROP_4D2', 'DROP_CORPSE', 'SMART', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_COLD']
	description = """A huge draconic form.  Frost covers it from head to tail.
"""

monsters.register_race(ancient_white_dragon)


class ancient_green_dragon(dragon, spellcaster):
	r_idx = "618"
	name = "Ancient green dragon"
	ascii = "D,g"
	level = 39
	rarity = 1
	hp = "42d15"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 80
	exp = 2400
	blows = [('CLAW', 'HURT', '4d8'), ('CLAW', 'HURT', '4d8'), ('BITE', 'HURT', '5d10')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'WILD_SWAMP2', 'DROP_3D2', 'DROP_4D2', 'CAN_FLY', 'SMART', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_POIS']
	description = """A huge draconic form enveloped in clouds of poisonous vapour.
"""

monsters.register_race(ancient_green_dragon)


class chthonian(monster, spellcaster):
	r_idx = "619"
	name = "Chthonian"
	ascii = "w,D"
	level = 39
	rarity = 2
	hp = "70d10"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 20
	exp = 2300
	blows = [('CRUSH', 'SHATTER', '11d6'), ('TOUCH', 'LOSE_CON', '1d2'), ('TOUCH', 'LOSE_CON', '1d2')]
	flags = ['WILD_DUNGEON_01', 'IM_FIRE', 'RES_PLAS', 'IM_COLD', 'IM_POIS', 'ELDRITCH_HORROR', 'RES_TELE', 'KILL_WALL', 'ONLY_GOLD', 'DROP_4D2', 'DROP_2D2', 'CAN_SWIM', 'DROP_CORPSE', 'EVIL', 'FORCE_MAXHP']
	sfreq = 5
	sflags = ['SCARE', 'CONF', 'HOLD', 'S_DEMON', 'MIND_BLAST', 'HEAL', 'HASTE', 'FORGET', 'BRAIN_SMASH']
	description = """"Flowing tentacles and a pulpy gray-black elongated sack of a body... 
no distinguishing features at all other than the reaching, groping 
tentacles. Or was there -- yes! -- a lump in the upper body of 
the thing... a container of sorts for the brain, ganglia, or 
whichever diseased organ governed this horror's loathsome life!"
"""

monsters.register_race(chthonian)


class eldrak(troll):
	r_idx = "620"
	name = "Eldrak"
	ascii = "T,r"
	level = 39
	rarity = 3
	hp = "50d10"
	speed = 110
	aaf = 20
	ac = 45
	sleep = 50
	exp = 800
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_60', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_SHORE', 'WILD_FOREST2', 'EVIL', 'TROLL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A massive troll, larger and stronger than many men together.
"""

monsters.register_race(eldrak)


class ettin(troll):
	r_idx = "621"
	name = "Ettin"
	ascii = "T,y"
	level = 39
	rarity = 3
	hp = "9d100"
	speed = 110
	aaf = 20
	ac = 55
	sleep = 30
	exp = 1000
	blows = [('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6')]
	flags = ['FORCE_MAXHP', 'ONLY_ITEM', 'DROP_1D2', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_FOREST2', 'WILD_SWAMP1', 'WILD_SWAMP2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'TROLL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A massive troll of huge strength.  Ettins are stupid but violent.
"""

monsters.register_race(ettin)


class night_mare(monster, undead):
	r_idx = "622"
	name = "Night mare"
	ascii = "q,v"
	level = 39
	rarity = 3
	hp = "9d100"
	speed = 120
	aaf = 30
	ac = 30
	sleep = 0
	exp = 2900
	blows = [('BITE', 'EXP_80', '2d6'), ('HIT', 'HURT', '3d8'), ('HIT', 'HURT', '3d8'), ('HIT', 'CONFUSE', '6d6')]
	flags = ['FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_GOLD', 'DROP_2D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A fearsome skeletal horse with glowing eyes, that watch you with little 
more than a hatred of all that lives. Flying hooves don't touch the 
ground...
"""

monsters.register_race(night_mare)


class vampire_lord(monster, undead, spellcaster):
	r_idx = "623"
	name = "Vampire lord"
	ascii = "V,b"
	level = 39
	rarity = 3
	hp = "10d100"
	speed = 120
	aaf = 20
	ac = 28
	sleep = 10
	exp = 1800
	blows = [('HIT', 'HURT', '1d6'), ('HIT', 'HURT', '1d6'), ('BITE', 'EXP_VAMP', '2d6'), ('BITE', 'EXP_VAMP', '2d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_4D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'RES_TELE', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 7
	sflags = ['BLIND', 'HOLD', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'DARKNESS', 'BO_NETH']
	description = """A foul wind chills your bones as this ghastly figure approaches.
"""

monsters.register_race(vampire_lord)


class ancient_black_dragon(dragon, spellcaster):
	r_idx = "624"
	name = "Ancient black dragon"
	ascii = "D,D"
	level = 39
	rarity = 1
	hp = "43d15"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 70
	exp = 2500
	blows = [('CLAW', 'HURT', '4d9'), ('CLAW', 'HURT', '4d9'), ('BITE', 'ACID', '5d10')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'DROP_3D2', 'DROP_4D2', 'DROP_CORPSE', 'SMART', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_ACID', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_ACID']
	description = """A huge draconic form.  Pools of acid melt the floor around it.
"""

monsters.register_race(ancient_black_dragon)


class weird_fume(monster, spellcaster):
	r_idx = "625"
	name = "Weird fume"
	ascii = "#,v"
	level = 40
	rarity = 2
	hp = "35d7"
	speed = 120
	aaf = 100
	ac = 20
	sleep = 0
	exp = 800
	blows = [('ENGULF', 'CONFUSE', '8d4'), ('ENGULF', 'CONFUSE', '8d4')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'RES_NEXU', 'AURA_ELEC', 'IM_FIRE', 'IM_ELEC', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'ATTR_MULTI', 'ATTR_ANY', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_CHAO', 'BR_NEXU', 'BR_NUKE']
	description = """A swirling spiral of mist, constantly changing its appearance.
"""

monsters.register_race(weird_fume)


class spawn_of_ubbo_sathla(monster):
	r_idx = "626"
	name = "Spawn of Ubbo-Sathla"
	ascii = "j,v"
	level = 40
	rarity = 5
	hp = "30d7"
	speed = 120
	aaf = 100
	ac = 20
	sleep = 50
	exp = 300
	blows = [('CRUSH', 'ACID', '8d4'), ('CRUSH', 'ACID', '8d4')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'ATTR_MULTI', 'ATTR_ANY', 'MULTIPLY', 'EVIL', 'RAND_25', 'RES_NEXU', 'AURA_ELEC', 'IM_FIRE', 'IM_ELEC', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'KILL_BODY', 'CAN_SWIM', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """Weird, jelly like creatures. No two look the same.
"""

monsters.register_race(spawn_of_ubbo_sathla)


class fat_man(monster):
	r_idx = "627"
	name = "Fat Man"
	ascii = "{,D"
	level = 40
	rarity = 2
	hp = "12d12"
	speed = 120
	aaf = 10
	ac = 40
	sleep = 12
	exp = 200
	blows = [('EXPLODE', 'SHATTER', '200d2')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'UNIQUE', 'REFLECTING', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'RES_TELE']
	sfreq = 0
	sflags = []
	description = """A shining machine of death and destruction.
"""

monsters.register_race(fat_man)


class malekith_the_accursed(monster, spellcaster):
	r_idx = "628"
	name = "Malekith the Accursed"
	ascii = "h,v"
	level = 40
	rarity = 2
	hp = "25d70"
	speed = 125
	aaf = 20
	ac = 35
	sleep = 10
	exp = 7500
	blows = [('HIT', 'HURT', '12d9'), ('HIT', 'HURT', '12d9')]
	flags = ['WILD_DUNGEON_01', 'MALE', 'REGENERATE', 'UNIQUE', 'CAN_SPEAK', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_90', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'NO_STUN']
	sfreq = 2
	sflags = ['HEAL', 'TELE_TO', 'BLIND', 'CONF', 'CAUSE_3', 'DARKNESS', 'FORGET', 'HOLD', 'S_MONSTER', 'S_MONSTERS', 'S_DEMON', 'TPORT', 'BA_NETH', 'MIND_BLAST', 'S_KIN']
	description = """One of the oldest and most powerful dark elves, Malekith is a master 
sorcerer devoted to evil. The left side of his face is pale, the other 
one is dark. His hair is long and white. 
"""

monsters.register_race(malekith_the_accursed)


class morgenstern_julians_steed(monster):
	r_idx = "629"
	name = "Morgenstern, Julian's steed"
	ascii = "q,W"
	level = 40
	rarity = 3
	hp = "30d70"
	speed = 130
	aaf = 20
	ac = 50
	sleep = 50
	exp = 2000
	blows = [('KICK', 'HURT', '4d4'), ('KICK', 'HURT', '5d5'), ('BITE', 'HURT', '6d6')]
	flags = ['FORCE_MAXHP', 'UNIQUE', 'ANIMAL', 'WILD_DUNGEON_01', 'REGENERATE', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """Morgenstern is tougher and faster than any other horsewas six hands higher than any other horse I had ever seen, and his 
eyes were the dead color of a Weimaraner dog's and his coat was all 
gray and his hooves looked like polished steel. He raced along like 
the wind..."
"""

monsters.register_race(morgenstern_julians_steed)


class spirit_troll(troll):
	r_idx = "630"
	name = "Spirit troll"
	ascii = "T,v"
	level = 40
	rarity = 3
	hp = "6d100"
	speed = 110
	aaf = 20
	ac = 36
	sleep = 5
	exp = 900
	blows = [('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d5'), ('HIT', 'HURT', '3d5')]
	flags = ['FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_90', 'INVISIBLE', 'PASS_WALL', 'CAN_FLY', 'EVIL', 'TROLL', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """A weird troll from the elemental planes.
"""

monsters.register_race(spirit_troll)


class war_troll(troll):
	r_idx = "631"
	name = "War troll"
	ascii = "T,g"
	level = 40
	rarity = 3
	hp = "50d7"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 50
	exp = 800
	blows = [('HIT', 'HURT', '4d4'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '6d6')]
	flags = ['FORCE_MAXHP', 'DROP_90', 'REGENERATE', 'FRIENDS', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'TROLL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A massive troll, equipped with a scimitar and heavy armour.
"""

monsters.register_race(war_troll)


class disenchanter_worm_mass(monster):
	r_idx = "632"
	name = "Disenchanter worm mass"
	ascii = "w,v"
	level = 40
	rarity = 3
	hp = "6d8"
	speed = 100
	aaf = 7
	ac = 5
	sleep = 10
	exp = 30
	blows = [('CRAWL', 'UN_BONUS', '1d4')]
	flags = ['WILD_DUNGEON_01', 'RAND_50', 'RES_DISE', 'ATTR_MULTI', 'CAN_SWIM', 'STUPID', 'WEIRD_MIND', 'MULTIPLY', 'BASH_DOOR', 'ANIMAL', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is a strange mass of squirming worms.  Magical energy crackles 
around its disgusting form.
"""

monsters.register_race(disenchanter_worm_mass)


class rotting_quylthulg(monster, spellcaster):
	r_idx = "633"
	name = "Rotting quylthulg"
	ascii = "Q,U"
	level = 40
	rarity = 1
	hp = "12d10"
	speed = 120
	aaf = 20
	ac = 1
	sleep = 0
	exp = 1500
	blows = []
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'INVISIBLE', 'EMPTY_MIND', 'ANIMAL', 'EVIL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['BLINK', 'TPORT', 'S_UNDEAD']
	description = """It is a pulsing flesh mound that reeks of death and putrefaction.
"""

monsters.register_race(rotting_quylthulg)


class lesser_titan(giant, spellcaster):
	r_idx = "634"
	name = "Lesser titan"
	ascii = "P,y"
	level = 40
	rarity = 3
	hp = "6d100"
	speed = 120
	aaf = 30
	ac = 35
	sleep = 15
	exp = 3500
	blows = [('HIT', 'CONFUSE', '6d6'), ('HIT', 'CONFUSE', '6d6'), ('HIT', 'CONFUSE', '6d6'), ('HIT', 'CONFUSE', '6d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'DROP_2D2', 'DROP_4D2', 'DROP_SKELETON', 'DROP_CORPSE', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT', 'MALE']
	sfreq = 3
	sflags = ['HEAL', 'TELE_TO', 'SCARE', 'S_MONSTERS']
	description = """It is a humanoid figure thirty feet tall that gives off an aura of power 
and hate.
"""

monsters.register_race(lesser_titan)


class nine_headed_hydra(monster, spellcaster):
	r_idx = "635"
	name = "9-headed hydra"
	ascii = "M,o"
	level = 40
	rarity = 2
	hp = "70d12"
	speed = 120
	aaf = 20
	ac = 48
	sleep = 20
	exp = 3000
	blows = [('BITE', 'FIRE', '3d6'), ('BITE', 'FIRE', '3d6'), ('BITE', 'FIRE', '3d6'), ('BITE', 'FIRE', '3d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_SWAMP1', 'WILD_SWAMP2', 'ONLY_GOLD', 'DROP_2D2', 'DROP_4D2', 'CAN_SWIM', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'DROP_CORPSE', 'ANIMAL', 'IM_FIRE']
	sfreq = 4
	sflags = ['SCARE', 'BO_FIRE', 'BR_FIRE']
	description = """A strange reptilian hybrid with nine smouldering heads.
"""

monsters.register_race(nine_headed_hydra)


class enchantress(monster, spellcaster):
	r_idx = "636"
	name = "Enchantress"
	ascii = "p,y"
	level = 40
	rarity = 4
	hp = "31d10"
	speed = 130
	aaf = 20
	ac = 25
	sleep = 10
	exp = 2100
	blows = [('HIT', 'HURT', '2d6'), ('HIT', 'HURT', '2d6'), ('HIT', 'HURT', '2d8')]
	flags = ['FEMALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST1', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'S_DRAGON']
	description = """This elusive female spellcaster has a special affinity for dragons, whom 
she rarely fights without.
"""

monsters.register_race(enchantress)


class archpriest(monster, spellcaster):
	r_idx = "637"
	name = "Archpriest"
	ascii = "p,b"
	level = 40
	rarity = 2
	hp = "34d10"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 10
	exp = 1800
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST1', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_90', 'DROP_2D2', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HEAL', 'BLIND', 'HOLD', 'CONF', 'CAUSE_3', 'ANIM_DEAD', 'S_MONSTER', 'S_UNDEAD']
	description = """An evil priest, dressed all in black.  Deadly spells hit you at an 
alarming rate as his black spiked mace rains down blow after blow on your 
pitiful frame.
"""

monsters.register_race(archpriest)


class sorcerer(monster, spellcaster):
	r_idx = "638"
	name = "Sorcerer"
	ascii = "p,o"
	level = 40
	rarity = 2
	hp = "31d10"
	speed = 130
	aaf = 20
	ac = 25
	sleep = 10
	exp = 2150
	blows = [('HIT', 'HURT', '2d8'), ('HIT', 'HURT', '2d8'), ('HIT', 'HURT', '2d8')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_WASTE1', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_90', 'DROP_4D2', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLINK', 'TELE_TO', 'BLIND', 'CONF', 'CAUSE_3', 'TRAPS', 'BO_ACID', 'BA_FIRE', 'BA_COLD', 'S_MONSTER', 'S_UNDEAD', 'S_DRAGON']
	description = """A human figure in robes, he moves with magically improved speed, and his 
hands are ablur with spell casting.
"""

monsters.register_race(sorcerer)


class xaren(monster):
	r_idx = "639"
	name = "Xaren"
	ascii = "X,y"
	level = 40
	rarity = 1
	hp = "22d10"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 10
	exp = 1200
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4')]
	flags = ['FORCE_MAXHP', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'KILL_ITEM', 'PASS_WALL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """It is a tougher relative of the Xorn.  Its hide glitters with metal ores.
"""

monsters.register_race(xaren)


class jubjub_bird(monster):
	r_idx = "640"
	name = "Jubjub bird"
	ascii = "B,u"
	level = 40
	rarity = 3
	hp = "45d13"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 10
	exp = 1000
	blows = [('CRUSH', 'HURT', '8d12'), ('CRUSH', 'HURT', '8d12'), ('HIT', 'ELEC', '12d12')]
	flags = ['BASH_DOOR', 'CAN_FLY', 'WILD_MOUNT1', 'WILD_FOREST1', 'ANIMAL', 'IM_ELEC', 'DROP_CORPSE']
	sfreq = 0
	sflags = []
	description = """A vast legendary bird, its iron talons rake the most impenetrable of 
surfaces and its screech echoes through the many winding dungeon corridors.
"""

monsters.register_race(jubjub_bird)


class minotaur(monster):
	r_idx = "641"
	name = "Minotaur"
	ascii = "H,U"
	level = 40
	rarity = 2
	hp = "60d10"
	speed = 130
	aaf = 13
	ac = 25
	sleep = 10
	exp = 2100
	blows = [('BUTT', 'HURT', '4d6'), ('BUTT', 'HURT', '4d6'), ('BUTT', 'HURT', '2d6'), ('BUTT', 'HURT', '2d6')]
	flags = ['BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'EVIL', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_MOUNT2']
	sfreq = 0
	sflags = []
	description = """It is a cross between a human and a bull.
"""

monsters.register_race(minotaur)


class jasra_brands_mistress(monster, spellcaster):
	r_idx = "642"
	name = "Jasra, Brand's Mistress"
	ascii = "n,v"
	level = 40
	rarity = 3
	hp = "24d70"
	speed = 120
	aaf = 30
	ac = 50
	sleep = 5
	exp = 9000
	blows = [('GAZE', 'EXP_80', ''), ('GAZE', 'PARALYZE', ''), ('HIT', 'HURT', '8d6'), ('HIT', 'HURT', '8d6')]
	flags = ['UNIQUE', 'FEMALE', 'CAN_SPEAK', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_SWIM', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'DROP_GOOD', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HOLD', 'SCARE', 'CAUSE_3', 'BO_FIRE', 'BO_PLAS', 'BA_ACID', 'S_HYDRA']
	description = """Brand's one-time mistress.  Her face could sink a thousand 
ships, but Brand is not too selective.
"""

monsters.register_race(jasra_brands_mistress)


class death_drake(dragon, spellcaster):
	r_idx = "643"
	name = "Death drake"
	ascii = "D,u"
	level = 40
	rarity = 2
	hp = "40d15"
	speed = 120
	aaf = 25
	ac = 45
	sleep = 80
	exp = 3500
	blows = [('CLAW', 'HURT', '4d10'), ('CLAW', 'HURT', '4d10'), ('BITE', 'EXP_80', '3d6'), ('BITE', 'EXP_80', '3d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'RES_TELE', 'INVISIBLE', 'TAKE_ITEM', 'CAN_FLY', 'PASS_WALL', 'POWERFUL', 'MOVE_BODY', 'RES_NETH', 'EVIL', 'DRAGON', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SLOW', 'CONF', 'SCARE', 'BR_NETH']
	description = """It is a dragon-like form wrapped in darkness.  You cannot make out its 
true form but you sense its evil.
"""

monsters.register_race(death_drake)


class ancient_red_dragon(dragon, spellcaster):
	r_idx = "644"
	name = "Ancient red dragon"
	ascii = "D,r"
	level = 40
	rarity = 1
	hp = "48d15"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 70
	exp = 2750
	blows = [('CLAW', 'HURT', '4d10'), ('CLAW', 'HURT', '4d10'), ('BITE', 'FIRE', '5d14')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'DROP_3D2', 'DROP_4D2', 'CAN_FLY', 'SMART', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_FIRE']
	description = """A huge draconic form.  Wisps of smoke steam from its nostrils and the 
extreme heat surrounding it makes you gasp for breath.
"""

monsters.register_race(ancient_red_dragon)


class ancient_gold_dragon(dragon, spellcaster):
	r_idx = "645"
	name = "Ancient gold dragon"
	ascii = "D,y"
	level = 40
	rarity = 2
	hp = "45d15"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 200
	exp = 4000
	blows = [('CLAW', 'HURT', '4d10'), ('CLAW', 'HURT', '4d10'), ('BITE', 'HURT', '5d14')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'DROP_3D2', 'DROP_4D2', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'SMART', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_SOUN']
	description = """A huge draconic form wreathed in a nimbus of light.
"""

monsters.register_race(ancient_gold_dragon)


class great_crystal_drake(dragon, spellcaster):
	r_idx = "646"
	name = "Great crystal drake"
	ascii = "D,B"
	level = 40
	rarity = 2
	hp = "55d15"
	speed = 120
	aaf = 25
	ac = 45
	sleep = 30
	exp = 3500
	blows = [('CLAW', 'HURT', '4d9'), ('CLAW', 'HURT', '4d9'), ('BITE', 'HURT', '5d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'REFLECTING', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'CAN_FLY', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SLOW', 'CONF', 'SCARE', 'BR_SHAR']
	description = """A huge crystalline dragon.  Its claws could cut you to shreds and its 
teeth are razor sharp.  Strange colours ripple through it as it moves in 
the light.
"""

monsters.register_race(great_crystal_drake)


class wyrd_sister(monster, spellcaster):
	r_idx = "647"
	name = "Wyrd sister"
	ascii = "p,y"
	level = 40
	rarity = 4
	hp = "38d11"
	speed = 125
	aaf = 20
	ac = 30
	sleep = 10
	exp = 1900
	blows = [('CLAW', 'HURT', '2d6'), ('CLAW', 'HURT', '2d6'), ('CLAW', 'HURT', '2d8')]
	flags = ['FEMALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_SKELETON', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'S_DEMON', 'CONF', 'SCARE', 'DARKNESS']
	description = """This old crone is rumoured to be a witch of chaos... but you don't 
really believe in witches, do you?
"""

monsters.register_race(wyrd_sister)


class clubber_demon(demon, spellcaster):
	r_idx = "648"
	name = "Clubber demon"
	ascii = "U,s"
	level = 40
	rarity = 2
	hp = "30d10"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 80
	exp = 1000
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '8d12'), ('HIT', 'HURT', '8d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'FRIENDS', 'ONLY_ITEM', 'DROP_60', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'NONLIVING', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['BLIND', 'CONF']
	description = """It is a demon swinging wildly with two clubs.
"""

monsters.register_race(clubber_demon)


class death_quasit(demon, spellcaster):
	r_idx = "649"
	name = "Death quasit"
	ascii = "u,w"
	level = 40
	rarity = 3
	hp = "27d10"
	speed = 130
	aaf = 20
	ac = 32
	sleep = 0
	exp = 1000
	blows = [('BITE', 'LOSE_DEX', '3d6'), ('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_90', 'DROP_2D2', 'DROP_4D2', 'NONLIVING', 'SMART', 'INVISIBLE', 'PASS_WALL', 'CAN_FLY', 'EVIL', 'DEMON', 'IM_FIRE', 'IM_POIS', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 10
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_3', 'FORGET', 'S_DEMON']
	description = """It is a demon of small stature, but its armoured frame moves with 
lightning speed and its powers make it a tornado of death and destruction.
"""

monsters.register_race(death_quasit)


class giganto_the_gargantuan(monster, spellcaster):
	r_idx = "650"
	name = "Giganto the Gargantuan"
	ascii = "l,s"
	level = 38
	rarity = 6
	hp = "60d25"
	speed = 110
	aaf = 20
	ac = 35
	sleep = 30
	exp = 7750
	blows = [('CRUSH', 'HURT', '33d2'), ('CRUSH', 'HURT', '33d2'), ('CRUSH', 'HURT', '33d2')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_SWIM', 'IM_FIRE', 'WILD_DUNGEON_01', 'DROP_60', 'DROP_90', 'DROP_2D2', 'UNIQUE', 'WEIRD_MIND', 'BASH_DOOR', 'SMART', 'EVIL', 'IM_COLD', 'DROP_CORPSE', 'NO_CONF', 'NO_SLEEP', 'RES_WATE', 'WILD_OCEAN']
	sfreq = 9
	sflags = ['BR_NUKE', 'BA_WATE']
	description = """A gargantuan mutant whale, who has grown legs that enable it to walk 
on dry land as well.
"""

monsters.register_race(giganto_the_gargantuan)


class strygalldwir(demon, spellcaster):
	r_idx = "651"
	name = "Strygalldwir"
	ascii = "U,W"
	level = 41
	rarity = 3
	hp = "9d100"
	speed = 120
	aaf = 90
	ac = 30
	sleep = 10
	exp = 8000
	blows = [('CLAW', 'HURT', '5d5'), ('CLAW', 'HURT', '5d5'), ('HIT', 'LOSE_STR', '4d4'), ('TOUCH', 'EXP_80', '8d1')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'MOVE_BODY', 'NONLIVING', 'OPEN_DOOR', 'BASH_DOOR', 'IM_POIS', 'IM_COLD', 'DEMON', 'EVIL']
	sfreq = 3
	sflags = ['CAUSE_3', 'HOLD', 'SCARE', 'BLIND', 'BO_ACID', 'S_DEMON', 'FORGET', 'BO_NETH', 'MIND_BLAST', 'DARKNESS']
	description = """A demon from the Courts of Chaos.  According to Corwin, 
'it was well over six feet in height, with great branches of antlers 
growing out of its forehead.  Nude, its flesh was a uniform ash-gray 
in color.  It appeared to be sexless, and it had gray, leathery wings 
extending far out behind it'.
"""

monsters.register_race(strygalldwir)


class fallen_angel(monster, spellcaster):
	r_idx = "652"
	name = "Fallen angel"
	ascii = "A,D"
	level = 41
	rarity = 6
	hp = "70d35"
	speed = 130
	aaf = 30
	ac = 70
	sleep = 255
	exp = 15000
	blows = [('GAZE', 'EXP_VAMP', '4d4'), ('GAZE', 'EXP_VAMP', '4d4'), ('HIT', 'HURT', '8d6'), ('HIT', 'HURT', '8d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'NO_FEAR', 'EVIL', 'REFLECTING', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'RES_TELE', 'RES_NETH', 'CAN_FLY']
	sfreq = 3
	sflags = ['TELE_TO', 'BLIND', 'SCARE', 'CAUSE_2', 'CAUSE_4', 'BO_MANA', 'S_DEMON', 'BO_NETH', 'INVULNER']
	description = """An angelic being who was mighty once but dared defy the Omnipotent 
to arms.
"""

monsters.register_race(fallen_angel)


class giant_headless(monster, spellcaster):
	r_idx = "653"
	name = "Giant headless"
	ascii = "H,u"
	level = 41
	rarity = 2
	hp = "30d18"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 40
	exp = 900
	blows = [('HIT', 'HURT', '4d8'), ('HIT', 'HURT', '4d8'), ('HIT', 'HURT', '4d8')]
	flags = ['FRIENDS', 'DROP_60', 'DROP_90', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'IM_POIS', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'EVIL']
	sfreq = 6
	sflags = ['SCARE', 'BA_NUKE', 'BLIND']
	description = """Giant headless humanoid abominations created by a magical mutation.
"""

monsters.register_race(giant_headless)


class judge_fire(monster, undead, spellcaster):
	r_idx = "654"
	name = "Judge Fire"
	ascii = "s,r"
	level = 41
	rarity = 3
	hp = "18d70"
	speed = 120
	aaf = 90
	ac = 35
	sleep = 10
	exp = 12000
	blows = [('HIT', 'FIRE', '5d5'), ('HIT', 'FIRE', '5d5'), ('GAZE', 'EXP_80', ''), ('WAIL', 'TERRIFY', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'POWERFUL', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'UNDEAD', 'HURT_COLD', 'IM_POIS', 'IM_FIRE', 'RES_PLAS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['CAUSE_3', 'BO_FIRE', 'BA_FIRE', 'BR_FIRE', 'BO_PLAS', 'DARKNESS', 'S_MONSTER', 'S_DEMON', 'S_UNDEAD', 'TPORT', 'BLINK', 'SCARE']
	description = """One of the Dark Judges, he has come to punish your crime of living. 
He looks like a skeleton enveloped in flames.
"""

monsters.register_race(judge_fire)


class ubbo_sathla_the_unbegotten_source(monster):
	r_idx = "655"
	name = "Ubbo-Sathla, the Unbegotten Source"
	ascii = "j,W"
	level = 41
	rarity = 3
	hp = "20d70"
	speed = 120
	aaf = 90
	ac = 40
	sleep = 10
	exp = 13500
	blows = [('CRUSH', 'ACID', '5d5'), ('HIT', 'POISON', '5d5'), ('CRUSH', 'ACID', '5d5'), ('HIT', 'POISON', '5d5')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'CAN_SPEAK', 'NO_STUN', 'NO_SLEEP', 'NO_CONF', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ELDRITCH_HORROR', 'ESCORT', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'REGENERATE', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'KILL_BODY', 'EVIL', 'HURT_FIRE', 'IM_COLD', 'IM_POIS']
	sfreq = 0
	sflags = []
	description = """"There, in the gray beginning of Earth, the formless mass that was 
Ubbo-Sathla reposed amid the slime and the vapors. Headless, 
without organs or members..."
"""

monsters.register_race(ubbo_sathla_the_unbegotten_source)


class judge_mortis(monster, undead, spellcaster):
	r_idx = "656"
	name = "Judge Mortis"
	ascii = "z,G"
	level = 41
	rarity = 3
	hp = "18d70"
	speed = 120
	aaf = 90
	ac = 35
	sleep = 10
	exp = 13000
	blows = [('HIT', 'POISON', '5d5'), ('HIT', 'DISEASE', '5d5'), ('TOUCH', 'LOSE_ALL', ''), ('TOUCH', 'EXP_80', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'UNDEAD', 'HURT_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'SCARE', 'CAUSE_3', 'BO_ACID', 'BO_NETH', 'BR_POIS', 'BR_NETH', 'BLINK', 'TPORT', 'ANIM_DEAD', 'BO_POIS', 'S_UNDEAD']
	description = """Another Dark Judge, he is a rotting humanoid with a cow's skull as 
his head.
"""

monsters.register_race(judge_mortis)


class dark_elven_sorceror(monster, spellcaster):
	r_idx = "657"
	name = "Dark elven sorceror"
	ascii = "h,v"
	level = 41
	rarity = 2
	hp = "48d10"
	speed = 130
	aaf = 20
	ac = 25
	sleep = 10
	exp = 3000
	blows = [('HIT', 'HURT', '2d8'), ('HIT', 'HURT', '2d8'), ('HIT', 'HURT', '2d8')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_90', 'DROP_4D2', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HEAL', 'BLINK', 'TELE_TO', 'BLIND', 'CONF', 'CAUSE_3', 'DARKNESS', 'BO_ACID', 'BA_FIRE', 'BA_COLD', 'ANIM_DEAD', 'S_MONSTER', 'S_UNDEAD', 'S_DEMON', 'MISSILE']
	description = """A dark elven figure, dressed in deepest black.  Power seems to crackle 
from his slender frame.
"""

monsters.register_race(dark_elven_sorceror)


class master_lich(monster, undead, spellcaster):
	r_idx = "658"
	name = "Master lich"
	ascii = "L,W"
	level = 41
	rarity = 2
	hp = "11d100"
	speed = 120
	aaf = 20
	ac = 32
	sleep = 50
	exp = 10000
	blows = [('TOUCH', 'EXP_80', ''), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'LOSE_DEX', '2d12'), ('TOUCH', 'LOSE_DEX', '2d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'RES_TELE', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLINK', 'TELE_TO', 'BLIND', 'HOLD', 'CONF', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'S_UNDEAD']
	description = """A skeletal form wrapped in robes.  Powerful magic crackles along its bony 
fingers.
"""

monsters.register_race(master_lich)


class byakhee(demon, spellcaster):
	r_idx = "659"
	name = "Byakhee"
	ascii = "U,D"
	level = 41
	rarity = 3
	hp = "45d7"
	speed = 110
	aaf = 20
	ac = 20
	sleep = 80
	exp = 1500
	blows = [('CLAW', 'LOSE_STR', '3d4'), ('BITE', 'EXP_20', '3d4')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'FRIENDS', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'ELDRITCH_HORROR', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['BO_FIRE', 'S_DEMON', 'CONF']
	description = """"There flapped rhythmically a horde of tame, trained, hybrid 
winged things... not altogether crows, nor moles, nor buzzards, 
nor ants, nor decomposed human beings, but something I cannot 
and must not recall."
"""

monsters.register_race(byakhee)


class rinaldo_son_of_brand(amberite, spellcaster):
	r_idx = "660"
	name = "Rinaldo, son of Brand"
	ascii = "p,w"
	level = 41
	rarity = 3
	hp = "16d70"
	speed = 120
	aaf = 20
	ac = 60
	sleep = 40
	exp = 7000
	blows = [('HIT', 'HURT', '8d6'), ('HIT', 'HURT', '8d6'), ('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_BONUS', '6d8')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'AMBERITE', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'IM_ACID', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 2
	sflags = ['CAUSE_3', 'BO_WATE', 'BO_MANA', 'BA_CHAO']
	description = """The madman's son is a Logrus Master, almost as great a 
menace as his father.
"""

monsters.register_race(rinaldo_son_of_brand)


class archon(monster, spellcaster):
	r_idx = "661"
	name = "Archon"
	ascii = "A,y"
	level = 41
	rarity = 6
	hp = "70d35"
	speed = 130
	aaf = 30
	ac = 70
	sleep = 255
	exp = 15000
	blows = [('GAZE', 'TERRIFY', '4d4'), ('GAZE', 'TERRIFY', '4d4'), ('HIT', 'HURT', '8d6'), ('HIT', 'HURT', '8d6')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'NO_FEAR', 'GOOD', 'REFLECTING', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'RES_TELE', 'CAN_FLY']
	sfreq = 3
	sflags = ['TELE_TO', 'BLIND', 'SCARE', 'CAUSE_2', 'CAUSE_4', 'BO_MANA', 'S_ANGEL', 'INVULNER']
	description = """Never a more heavenly being have you seen.  The very holiness of its 
presence makes you deeply respect it.  Few creatures can match the powers 
of an Archon; fewer still live to tell the tale after attacking one.
"""

monsters.register_race(archon)


class formless_spawn_of_tsathoggua(demon, spellcaster):
	r_idx = "662"
	name = "Formless spawn of Tsathoggua"
	ascii = "U,D"
	level = 41
	rarity = 2
	hp = "22d14"
	speed = 110
	aaf = 20
	ac = 25
	sleep = 80
	exp = 1850
	blows = [('HIT', 'ACID', '2d4'), ('HIT', 'ACID', '2d4'), ('CRUSH', 'HURT', '3d4'), ('BITE', 'ACID', '6d6')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ELDRITCH_HORROR', 'NONLIVING', 'ONLY_ITEM', 'DROP_90', 'REGENERATE', 'RES_TELE', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'CAN_SWIM', 'EVIL', 'DEMON', 'NO_CONF', 'NO_SLEEP', 'HURT_FIRE', 'IM_POIS']
	sfreq = 9
	sflags = ['BO_FIRE', 'BO_ACID', 'S_DEMON', 'MIND_BLAST', 'DARKNESS']
	description = """"...living things that oozed along stone channels... 
But they were not toads like Tsathoggua himself. Far worse -- 
they were amorphous lumps of viscous black slime that took 
temporary shapes for various purposes."
"""

monsters.register_race(formless_spawn_of_tsathoggua)


class hunting_horror(demon, spellcaster):
	r_idx = "663"
	name = "Hunting horror"
	ascii = "U,D"
	level = 42
	rarity = 2
	hp = "30d13"
	speed = 110
	aaf = 20
	ac = 45
	sleep = 80
	exp = 2300
	blows = [('BITE', 'LOSE_DEX', '1d3'), ('BITE', 'POISON', '1d3'), ('CRUSH', 'HURT', '9d4')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ELDRITCH_HORROR', 'ONLY_ITEM', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'CAN_FLY', 'EVIL', 'DEMON', 'HURT_LITE', 'IM_POIS', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['BLIND', 'CONF', 'S_DEMON', 'BR_DARK']
	description = """"And in the air there were great viperine creatures, 
which had curiously distorted heads, and grotesquely great 
clawed appendages, supporting themselves with ease by the aid 
of black rubbery wings of singularly monstrous dimensions."
"""

monsters.register_race(hunting_horror)


class undead_beholder(monster, undead, spellcaster):
	r_idx = "664"
	name = "Undead beholder"
	ascii = "e,s"
	level = 42
	rarity = 4
	hp = "27d70"
	speed = 120
	aaf = 30
	ac = 50
	sleep = 10
	exp = 6000
	blows = [('GAZE', 'EXP_40', ''), ('GAZE', 'PARALYZE', ''), ('GAZE', 'LOSE_INT', '2d6'), ('GAZE', 'UN_POWER', '2d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'CAN_FLY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['SLOW', 'CAUSE_4', 'DRAIN_MANA', 'MIND_BLAST', 'BRAIN_SMASH', 'FORGET', 'BO_MANA', 'ANIM_DEAD', 'S_UNDEAD']
	description = """A disembodied eye, floating in the air.  Black nether storms rage around 
its bloodshot pupil and light seems to bend as it sucks its power from the 
very air around it.  Your soul chills as it drains your vitality for its 
evil enchantments.
"""

monsters.register_race(undead_beholder)


class shadow_demon(demon, spellcaster):
	r_idx = "665"
	name = "Shadow demon"
	ascii = "G,v"
	level = 42
	rarity = 3
	hp = "7d20"
	speed = 120
	aaf = 30
	ac = 15
	sleep = 20
	exp = 425
	blows = [('TOUCH', 'EXP_80', ''), ('TOUCH', 'EXP_40', ''), ('CLAW', 'LOSE_INT', '1d10'), ('CLAW', 'LOSE_WIS', '1d10')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'POWERFUL', 'REGENERATE', 'HURT_LITE', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'FRIENDS', 'RES_NETH', 'EVIL', 'DEMON', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['BO_NETH']
	description = """A mighty spirit of darkness of vaguely humanoid form.  Razor-edged claws 
reach out to end your life as it glides towards you, seeking to suck the 
energy from your soul to feed its power.
"""

monsters.register_race(shadow_demon)


class iron_lich(monster, undead, spellcaster):
	r_idx = "666"
	name = "Iron lich"
	ascii = "L,s"
	level = 42
	rarity = 4
	hp = "28d70"
	speed = 120
	aaf = 30
	ac = 70
	sleep = 10
	exp = 9000
	blows = [('BUTT', 'COLD', '3d6'), ('BUTT', 'FIRE', '3d6'), ('BUTT', 'ELEC', '3d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'REFLECTING', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'UNDEAD', 'POWERFUL', 'SMART', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'RES_TELE', 'ONLY_ITEM', 'DROP_60', 'DROP_GOOD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BA_WATE', 'BR_FIRE', 'BO_ICEE', 'BA_ELEC', 'BA_COLD', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'S_UNDEAD']
	description = """It is a huge, twisted grey skull floating through the air. Its cold eyes 
burn with hatred towards all who live.
"""

monsters.register_race(iron_lich)


class dread(monster, undead, spellcaster):
	r_idx = "667"
	name = "Dread"
	ascii = "G,o"
	level = 42
	rarity = 1
	hp = "15d20"
	speed = 120
	aaf = 20
	ac = 22
	sleep = 10
	exp = 600
	blows = [('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6'), ('HIT', 'LOSE_STR', '3d4')]
	flags = ['FORCE_SLEEP', 'RAND_25', 'FRIENDS', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_60', 'TAKE_ITEM', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 15
	sflags = ['BLIND', 'HOLD', 'CONF', 'DRAIN_MANA', 'BO_NETH']
	description = """It is a form that screams its presence against the eye.  Death incarnate, 
its hideous black body seems to struggle against reality as the universe 
itself struggles to banish it.
"""

monsters.register_race(dread)


class greater_basilisk(monster, spellcaster):
	r_idx = "668"
	name = "Greater basilisk"
	ascii = "R,D"
	level = 42
	rarity = 2
	hp = "14d100"
	speed = 120
	aaf = 25
	ac = 50
	sleep = 15
	exp = 10000
	blows = [('GAZE', 'PARALYZE', ''), ('BITE', 'HURT', '5d12'), ('BITE', 'HURT', '5d12'), ('BITE', 'HURT', '5d12')]
	flags = ['WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_MOUNT2', 'WILD_FOREST2', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'POWERFUL', 'FORCE_MAXHP', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_POIS', 'DROP_CORPSE', 'ANIMAL', 'NO_CONF', 'NO_SLEEP', 'CAN_SWIM']
	sfreq = 8
	sflags = ['BR_POIS', 'BR_DARK', 'BR_NETH']
	description = """A large basilisk, whose shape resembles that of a great wyrm.
"""

monsters.register_race(greater_basilisk)


class charybdis(monster, spellcaster):
	r_idx = "669"
	name = "Charybdis"
	ascii = "l,r"
	level = 42
	rarity = 1
	hp = "40d70"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 70
	exp = 13000
	blows = [('ENGULF', 'HURT', '10d8'), ('ENGULF', 'HURT', '10d8'), ('ENGULF', 'HURT', '10d8')]
	flags = ['AQUATIC', 'EVIL', 'UNIQUE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'SMART', 'POWERFUL', 'MOVE_BODY', 'NEVER_MOVE', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'RES_WATE', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_STUN']
	sfreq = 5
	sflags = ['BA_WATE']
	description = """A monstrous dweller of the depths; its hungry maw has been the doom 
of innumerable sailors!
"""

monsters.register_race(charybdis)


class jack_of_shadows(monster, spellcaster):
	r_idx = "670"
	name = "Jack of Shadows"
	ascii = "p,s"
	level = 42
	rarity = 4
	hp = "30d70"
	speed = 150
	aaf = 70
	ac = 75
	sleep = 4
	exp = 10000
	blows = [('HIT', 'HURT', '5d10'), ('HIT', 'HURT', '5d10'), ('HIT', 'EAT_ITEM', '2d10'), ('HIT', 'EAT_ITEM', '2d10')]
	flags = ['WILD_DUNGEON_01', 'MALE', 'FORCE_MAXHP', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'REGENERATE', 'NO_CONF', 'NO_SLEEP', 'NO_STUN', 'NO_FEAR', 'RES_TELE', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'DROP_GREAT', 'UNIQUE', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'SMART', 'POWERFUL']
	sfreq = 3
	sflags = ['BLIND', 'HEAL', 'BA_DARK', 'HASTE', 'CONF']
	description = """Deriving his strength from the shadows, this king of thieves 
steals only for the challenge.
"""

monsters.register_race(jack_of_shadows)


class zephyr_lord(monster, undead, spellcaster):
	r_idx = "671"
	name = "Zephyr Lord"
	ascii = "W,v"
	level = 43
	rarity = 2
	hp = "60d11"
	speed = 130
	aaf = 20
	ac = 35
	sleep = 10
	exp = 3000
	blows = [('HIT', 'EXP_20', '8d2'), ('HIT', 'LOSE_STR', '8d2'), ('HIT', 'LOSE_CON', '8d2')]
	flags = ['WILD_DUNGEON_01', 'MALE', 'ATTR_MULTI', 'UNDEAD', 'EVIL', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'IM_FIRE', 'IM_COLD', 'IM_ACID', 'IM_ELEC', 'IM_POIS', 'ONLY_ITEM', 'DROP_90', 'DROP_4D2', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'NO_STUN', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['SHRIEK', 'S_HOUND']
	description = """An undead master of the vicious Zephyr hounds.
"""

monsters.register_race(zephyr_lord)


class juggernaut_of_khorne(demon):
	r_idx = "672"
	name = "Juggernaut of Khorne"
	ascii = "g,D"
	level = 43
	rarity = 3
	hp = "68d19"
	speed = 125
	aaf = 12
	ac = 45
	sleep = 10
	exp = 2500
	blows = [('BUTT', 'HURT', '6d6'), ('CRUSH', 'HURT', '8d6'), ('CRUSH', 'HURT', '8d6'), ('BUTT', 'HURT', '6d6')]
	flags = ['WILD_DUNGEON_01', 'BASH_DOOR', 'REFLECTING', 'KILL_ITEM', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'DEMON', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING', 'EVIL']
	sfreq = 0
	sflags = []
	description = """A great, vicious beast whose flesh is made of steel and whose 
blood is fire. It charges at you ignoring everything else.
"""

monsters.register_race(juggernaut_of_khorne)


class mumak(monster):
	r_idx = "673"
	name = "Mumak"
	ascii = "q,s"
	level = 43
	rarity = 2
	hp = "54d10"
	speed = 110
	aaf = 20
	ac = 30
	sleep = 100
	exp = 2100
	blows = [('BUTT', 'HURT', '4d6'), ('BUTT', 'HURT', '4d6'), ('CRUSH', 'HURT', '4d4')]
	flags = ['FRIENDS', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_GRASS', 'WILD_FOREST1', 'BASH_DOOR', 'ANIMAL']
	sfreq = 0
	sflags = []
	description = """A massive elephantine form with eyes twisted by madness.
"""

monsters.register_race(mumak)


class judge_fear(monster, undead, spellcaster):
	r_idx = "674"
	name = "Judge Fear"
	ascii = "W,D"
	level = 43
	rarity = 4
	hp = "18d70"
	speed = 120
	aaf = 90
	ac = 45
	sleep = 10
	exp = 12000
	blows = [('GAZE', 'TERRIFY', ''), ('GAZE', 'EXP_40', ''), ('GAZE', 'EXP_40', ''), ('GAZE', 'HURT', '2d20')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'INVISIBLE', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'UNDEAD', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'HOLD', 'SCARE', 'CAUSE_3', 'MIND_BLAST', 'BRAIN_SMASH', 'S_UNDEAD', 'DRAIN_MANA', 'FORGET', 'ANIM_DEAD']
	description = """A Dark Judge, reputedly so frightening that his gaze can kill.
"""

monsters.register_race(judge_fear)


class ancient_multi_hued_dragon(dragon, spellcaster):
	r_idx = "675"
	name = "Ancient multi-hued dragon"
	ascii = "D,v"
	level = 43
	rarity = 1
	hp = "100d15"
	speed = 120
	aaf = 20
	ac = 45
	sleep = 70
	exp = 13000
	blows = [('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('BITE', 'HURT', '6d12')]
	flags = ['ATTR_MULTI', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_POIS']
	description = """A huge draconic form.  Many colours ripple down its massive frame.  Few 
live to see another.
"""

monsters.register_race(ancient_multi_hued_dragon)


class ethereal_dragon(dragon, spellcaster):
	r_idx = "676"
	name = "Ethereal dragon"
	ascii = "D,o"
	level = 43
	rarity = 2
	hp = "14d100"
	speed = 120
	aaf = 25
	ac = 45
	sleep = 15
	exp = 11000
	blows = [('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('BITE', 'HURT', '6d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_GRASS', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'INVISIBLE', 'CAN_FLY', 'PASS_WALL', 'POWERFUL', 'MOVE_BODY', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BLIND', 'CONF', 'BR_LITE', 'BR_DARK', 'BR_CONF']
	description = """A huge dragon emanating from the elemental plains, the ethereal dragon is 
a master of light and dark.  Its form disappears from sight as it cloaks 
itself in unearthly shadows.
"""

monsters.register_race(ethereal_dragon)


class dark_young_of_shub_niggurath(demon, spellcaster):
	r_idx = "677"
	name = "Dark young of Shub-Niggurath"
	ascii = "#,D"
	level = 43
	rarity = 2
	hp = "9d100"
	speed = 120
	aaf = 20
	ac = 37
	sleep = 80
	exp = 5000
	blows = [('CRUSH', 'HURT', '5d6'), ('CRUSH', 'HURT', '5d6'), ('BITE', 'LOSE_STR', '1d6'), ('BITE', 'LOSE_STR', '1d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'ELDRITCH_HORROR', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'NONLIVING', 'CAN_SWIM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'HURT_LITE', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 9
	sflags = ['BLIND', 'CAUSE_2', 'S_DEMON', 'HEAL']
	description = """"Something black in the road, something that wasn't a tree. 
Something big and black and ropy, just squatting there, waiting, 
with ropy arms squirming and reaching... It came crawling up the 
hillside... and it was the black thing of my dreams... that black, 
ropy, slimy jelly tree-thing out of the woods. It crawled up and it 
flowed up on its hoofs and mouths and snaky arms."
"""

monsters.register_race(dark_young_of_shub_niggurath)


class colour_out_of_space(monster, spellcaster):
	r_idx = "678"
	name = "Colour out of space"
	ascii = ".,v"
	level = 43
	rarity = 2
	hp = "9d100"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 10
	exp = 8000
	blows = [('TOUCH', 'LOSE_ALL', '7d6'), ('TOUCH', 'LOSE_ALL', '7d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_25', 'WILD_DUNGEON_01', 'INVISIBLE', 'PASS_WALL', 'CAN_FLY', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'ATTR_MULTI', 'ATTR_ANY', 'CHAR_CLEAR', 'SMART', 'COLD_BLOOD', 'EVIL', 'IM_COLD', 'IM_FIRE', 'IM_ACID', 'IM_ELEC', 'IM_POIS', 'NO_STUN', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['DRAIN_MANA', 'BR_DISE', 'BR_NETH', 'TPORT']
	description = """"The shaft of phosphorescence from the well... was no longer shining 
out, it was pouring out; and as the shapeless stream of unplaceable 
colour left the well it seemed to flow directly into the sky."
"""

monsters.register_race(colour_out_of_space)


class quaker_master_of_earth(monster, spellcaster):
	r_idx = "679"
	name = "Quaker, Master of Earth"
	ascii = "E,u"
	level = 43
	rarity = 4
	hp = "18d70"
	speed = 110
	aaf = 10
	ac = 50
	sleep = 90
	exp = 6000
	blows = [('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6'), ('HIT', 'SHATTER', '10d10')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'EMPTY_MIND', 'COLD_BLOOD', 'KILL_WALL', 'KILL_ITEM', 'KILL_BODY', 'PASS_WALL', 'POWERFUL', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['BO_ACID', 'BA_ACID']
	description = """A towering stone elemental stands before you.  The walls and ceiling are 
reduced to rubble as Quaker advances.
"""

monsters.register_race(quaker_master_of_earth)


class death_leprechaun(monster, undead, spellcaster):
	r_idx = "680"
	name = "Death leprechaun"
	ascii = "h,D"
	level = 44
	rarity = 6
	hp = "5d5"
	speed = 120
	aaf = 8
	ac = 7
	sleep = 8
	exp = 85
	blows = [('TOUCH', 'EXP_40', '1d10'), ('TOUCH', 'EAT_GOLD', ''), ('TOUCH', 'EAT_ITEM', '')]
	flags = ['WILD_DUNGEON_01', 'MULTIPLY', 'INVISIBLE', 'RAND_25', 'TAKE_ITEM', 'COLD_BLOOD', 'SMART', 'HURT_LITE', 'EVIL', 'OPEN_DOOR', 'MALE', 'UNDEAD', 'RES_NETH']
	sfreq = 6
	sflags = ['BLINK', 'TPORT', 'TELE_TO', 'CAUSE_3', 'ANIM_DEAD']
	description = """Nasty undead little creatures.  They are chanting the name of the 
sinister wizard who created them"""

monsters.register_race(death_leprechaun)


class chaugnar_faugn_horror_from_the_hills(demon, spellcaster):
	r_idx = "681"
	name = "Chaugnar Faugn, Horror from the Hills"
	ascii = "q,D"
	level = 44
	rarity = 4
	hp = "30d70"
	speed = 110
	aaf = 10
	ac = 62
	sleep = 90
	exp = 6250
	blows = [('CRUSH', 'HURT', '10d10'), ('CRUSH', 'HURT', '10d10'), ('BITE', 'LOSE_CON', '8d2'), ('BITE', 'LOSE_CON', '8d1')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'EVIL', 'DEMON', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'ELDRITCH_HORROR', 'POWERFUL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['HOLD', 'CAUSE_4', 'CONF', 'SCARE', 'FORGET', 'BRAIN_SMASH', 'MIND_BLAST', 'CAUSE_3', 'S_DEMON', 'S_MONSTERS']
	description = """"The ears were webbed and tentacled, the trunk terminated in 
a huge flaring disk at least a foot in diameter... Its forelimbs 
were bent stiffly at the elbow, and its hands... rested palms 
upward on its lap."
"""

monsters.register_race(chaugnar_faugn_horror_from_the_hills)


class lloigor(monster, spellcaster):
	r_idx = "682"
	name = "Lloigor"
	ascii = "v,B"
	level = 44
	rarity = 2
	hp = "70d15"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 20
	exp = 6000
	blows = [('ENGULF', 'LOSE_CON', '4d10'), ('ENGULF', 'LOSE_CON', '4d10'), ('ENGULF', 'LOSE_CON', '4d10')]
	flags = ['FORCE_SLEEP', 'ELDRITCH_HORROR', 'PASS_WALL', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'INVISIBLE', 'DROP_2D2', 'DROP_4D2', 'CAN_FLY', 'EVIL', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'POWERFUL', 'NONLIVING', 'SMART', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_ACID', 'IM_POIS']
	sfreq = 4
	sflags = ['BR_WALL', 'TELE_AWAY', 'BR_GRAV', 'S_KIN', 'MIND_BLAST', 'BLIND', 'BLINK']
	description = """"Invisible ones from the stars... They sometimes took forms... 
but existed as vortices of power in their natural state."
"""

monsters.register_race(lloigor)


class utgard_loke(giant, spellcaster):
	r_idx = "683"
	name = "Utgard-Loke"
	ascii = "P,w"
	level = 44
	rarity = 3
	hp = "40d70"
	speed = 120
	aaf = 30
	ac = 62
	sleep = 15
	exp = 13500
	blows = [('HIT', 'COLD', '12d12'), ('HIT', 'COLD', '12d12'), ('HIT', 'COLD', '12d12'), ('HIT', 'COLD', '12d12')]
	flags = ['ESCORT', 'UNIQUE', 'IM_COLD', 'AURA_COLD', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'MOVE_BODY', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT', 'MALE']
	sfreq = 3
	sflags = ['HEAL', 'TELE_TO', 'S_KIN', 'BR_COLD']
	description = """A frost giant chieftain, unusually smart for a giant.
"""

monsters.register_race(utgard_loke)


class quachil_uttaus_treader_of_the_dust(demon, undead, spellcaster):
	r_idx = "684"
	name = "Quachil Uttaus, Treader of the Dust"
	ascii = "z,D"
	level = 44
	rarity = 3
	hp = "50d50"
	speed = 120
	aaf = 30
	ac = 40
	sleep = 15
	exp = 20000
	blows = [('CLAW', 'HURT', '50d1'), ('TOUCH', 'TIME', '1d50'), ('TOUCH', 'TIME', '1d50')]
	flags = ['ESCORT', 'UNIQUE', 'IM_COLD', 'ELDRITCH_HORROR', 'RES_NETH', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'KILL_BODY', 'DROP_GREAT', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'DEMON', 'REGENERATE']
	sfreq = 3
	sflags = ['BR_TIME', 'SLOW', 'HASTE', 'HEAL']
	description = """"It was a figure no larger than a young child, but sere and 
shriveled as some millenial mummy. Its hairless head, its 
unfeatured face, borne on a neck of skeleton thinness, were 
lined with a thousand reticulated wrinkles. The body was like 
that of some monstrous, withered abortion that had never drawn 
breath. The pipy arms, ending in bony claws, were outhrust as if 
ankylosed in posture of an eternal dreadful groping."
"""

monsters.register_race(quachil_uttaus_treader_of_the_dust)


class shoggoth(demon):
	r_idx = "685"
	name = "Shoggoth"
	ascii = "j,D"
	level = 44
	rarity = 2
	hp = "50d14"
	speed = 140
	aaf = 100
	ac = 40
	sleep = 20
	exp = 2500
	blows = [('CRUSH', 'ACID', '5d6'), ('CRUSH', 'ACID', '5d6'), ('CRUSH', 'ACID', '5d6'), ('CRUSH', 'HURT', '9d9')]
	flags = ['REGENERATE', 'KILL_ITEM', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'BASH_DOOR', 'EVIL', 'DEMON', 'NO_CONF', 'NO_FEAR', 'NO_SLEEP', 'KILL_BODY', 'CAN_SWIM', 'FORCE_MAXHP', 'FORCE_SLEEP', 'HURT_LITE', 'POWERFUL', 'ELDRITCH_HORROR', 'IM_ACID', 'IM_FIRE', 'RES_PLAS', 'IM_POIS', 'IM_COLD', 'IM_ELEC', 'RES_TELE']
	sfreq = 0
	sflags = []
	description = """"The nightmare, plastic column of fetid, black iridescence oozed 
tightly onward... A shapeless congerie of protoplasmic bubbles, 
faintly self-luminous and with myriads of temporary eyes forming 
and unforming as pustules of greenish light all over the 
tunnel-filling front that bore down upon us, crushing the frantic 
penguins and slithering over glistening floor that it and its 
kind had swept so evilly free of all litter. Still came that eldritch 
mocking cry -- 'Tekeli-li! Tekeli-li!'"
"""

monsters.register_race(shoggoth)


class judge_death(monster, undead, spellcaster):
	r_idx = "686"
	name = "Judge Death"
	ascii = "W,D"
	level = 44
	rarity = 3
	hp = "45d38"
	speed = 120
	aaf = 90
	ac = 45
	sleep = 10
	exp = 14000
	blows = [('CLAW', 'POISON', '10d5'), ('CLAW', 'POISON', '10d5'), ('CLAW', 'EXP_40', '10d1'), ('GAZE', 'TERRIFY', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'UNDEAD', 'IM_ELEC', 'IM_COLD', 'HURT_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'HOLD', 'SCARE', 'CAUSE_4', 'BA_FIRE', 'BA_NETH', 'ANIM_DEAD', 'S_MONSTERS', 'S_UNDEAD', 'S_HI_UNDEAD', 'DRAIN_MANA']
	description = """The most powerful Dark Judge, whose touch means death.
"""

monsters.register_race(judge_death)


class ariel_queen_of_air(monster, spellcaster):
	r_idx = "687"
	name = "Ariel, Queen of Air"
	ascii = "E,B"
	level = 44
	rarity = 4
	hp = "27d70"
	speed = 130
	aaf = 12
	ac = 25
	sleep = 50
	exp = 8000
	blows = [('HIT', 'HURT', '4d6'), ('HIT', 'CONFUSE', '1d4'), ('HIT', 'HURT', '4d6'), ('HIT', 'CONFUSE', '1d4')]
	flags = ['UNIQUE', 'FEMALE', 'AURA_ELEC', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_25', 'CAN_FLY', 'NONLIVING', 'EMPTY_MIND', 'COLD_BLOOD', 'KILL_ITEM', 'KILL_BODY', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 5
	sflags = ['BO_ELEC', 'BA_COLD', 'BA_ELEC']
	description = """A towering air elemental, Ariel, the sorceress, avoids your blows 
with her extreme speed.
"""

monsters.register_race(ariel_queen_of_air)


class eleven_headed_hydra(monster, spellcaster):
	r_idx = "688"
	name = "11-headed hydra"
	ascii = "M,R"
	level = 44
	rarity = 2
	hp = "70d18"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 20
	exp = 6000
	blows = [('BITE', 'FIRE', '3d12'), ('BITE', 'FIRE', '3d12'), ('BITE', 'FIRE', '3d12'), ('BITE', 'FIRE', '3d12')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'WILD_SHORE', 'WILD_SWAMP1', 'WILD_SWAMP2', 'ONLY_GOLD', 'DROP_2D2', 'DROP_4D2', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'ANIMAL', 'IM_FIRE', 'CAN_SWIM']
	sfreq = 4
	sflags = ['SCARE', 'BO_FIRE', 'BO_PLAS', 'BA_FIRE', 'BR_FIRE']
	description = """A strange reptilian hybrid with eleven smouldering heads.
"""

monsters.register_race(eleven_headed_hydra)


class high_priest(monster, spellcaster):
	r_idx = "689"
	name = "High priest"
	ascii = "p,w"
	level = 44
	rarity = 2
	hp = "80d7"
	speed = 120
	aaf = 20
	ac = 25
	sleep = 10
	exp = 5000
	blows = [('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d4'), ('HIT', 'HURT', '3d5')]
	flags = ['MALE', 'WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST1', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_90', 'DROP_4D2', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HEAL', 'BLIND', 'HOLD', 'CAUSE_4', 'BRAIN_SMASH', 'ANIM_DEAD', 'S_MONSTERS', 'S_UNDEAD']
	description = """A dark priest of the highest order.  Powerful and evil, beware his many 
spells.
"""

monsters.register_race(high_priest)


class dreadmaster(monster, undead, spellcaster):
	r_idx = "690"
	name = "Dreadmaster"
	ascii = "G,o"
	level = 44
	rarity = 2
	hp = "12d70"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 10
	exp = 8000
	blows = [('HIT', 'HURT', '6d6'), ('HIT', 'HURT', '6d6'), ('HIT', 'LOSE_STR', '3d4'), ('HIT', 'LOSE_STR', '3d4')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_25', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'CAN_FLY', 'SMART', 'TAKE_ITEM', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 9
	sflags = ['TELE_LEVEL', 'BLIND', 'HOLD', 'CONF', 'CAUSE_4', 'DRAIN_MANA', 'BO_NETH', 'S_UNDEAD']
	description = """It is an unlife of power almost unequaled.  An affront to existence, its 
very touch abuses and disrupts the flow of life, and its unearthly limbs, 
of purest black, crush rock and flesh with ease.
"""

monsters.register_race(dreadmaster)


class drolem(dragon, spellcaster):
	r_idx = "691"
	name = "Drolem"
	ascii = "g,g"
	level = 44
	rarity = 3
	hp = "15d100"
	speed = 120
	aaf = 25
	ac = 52
	sleep = 30
	exp = 12000
	blows = [('BITE', 'HURT', '5d8'), ('BITE', 'HURT', '5d8'), ('CLAW', 'POISON', '3d3'), ('CLAW', 'POISON', '3d3')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'WILD_DUNGEON_01', 'EMPTY_MIND', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'DRAGON', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'CAN_SWIM', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 6
	sflags = ['BLIND', 'SLOW', 'CONF', 'ARROW_3', 'BR_POIS']
	description = """A constructed dragon, the drolem has massive strength.  Powerful spells 
weaved during its creation make it a fearsome adversary.  Its eyes show 
little intelligence, but it has been instructed to destroy all it meets.
"""

monsters.register_race(drolem)


class scatha_the_worm(dragon, spellcaster):
	r_idx = "692"
	name = "Scatha the Worm"
	ascii = "D,w"
	level = 44
	rarity = 2
	hp = "20d70"
	speed = 120
	aaf = 20
	ac = 65
	sleep = 70
	exp = 17000
	blows = [('CLAW', 'HURT', '4d10'), ('CLAW', 'HURT', '4d10'), ('CLAW', 'HURT', '4d10'), ('BITE', 'HURT', '6d14')]
	flags = ['UNIQUE', 'MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_CORPSE', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'CAN_FLY', 'EVIL', 'DRAGON', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['CONF', 'CAUSE_3', 'BR_COLD']
	description = """An ancient and wise Dragon.  Scatha has grown clever over the long years.  
His scales are covered with frost, and his breath sends a shower of ice 
into the air.
"""

monsters.register_race(scatha_the_worm)


class warrior_of_the_dawn(monster):
	r_idx = "693"
	name = "Warrior of the Dawn"
	ascii = "p,R"
	level = 45
	rarity = 2
	hp = "19d25"
	speed = 120
	aaf = 20
	ac = 35
	sleep = 20
	exp = 500
	blows = [('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['IM_POIS', 'IM_FIRE', 'IM_ELEC', 'IM_ACID', 'IM_COLD', 'NO_SLEEP', 'NO_FEAR', 'FRIENDS', 'WILD_DUNGEON_01', 'MALE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 0
	sflags = []
	description = """Fierce, barbaric warriors, armed with great spiked clubs, and surrounded 
in an aura of scarlet. Whenever one of them is slain, another appears 
out of nowhere to take his place.
"""

monsters.register_race(warrior_of_the_dawn)


class lesser_black_reaver(monster, undead, spellcaster):
	r_idx = "694"
	name = "Lesser black reaver"
	ascii = "L,D"
	level = 45
	rarity = 3
	hp = "25d70"
	speed = 120
	aaf = 20
	ac = 60
	sleep = 50
	exp = 12000
	blows = [('HIT', 'UN_BONUS', '4d8'), ('HIT', 'UN_BONUS', '4d8'), ('HIT', 'LOSE_STR', '4d6'), ('HIT', 'LOSE_STR', '4d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'CAN_SWIM', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'KILL_WALL', 'NO_FEAR']
	sfreq = 3
	sflags = ['TELE_TO', 'BLIND', 'HOLD', 'CONF', 'CAUSE_3', 'DRAIN_MANA', 'MIND_BLAST', 'BA_NETH', 'ANIM_DEAD']
	description = """A humanoid form, black as night, advancing steadily and unstoppably.
"""

monsters.register_race(lesser_black_reaver)


class zoth_ommog(monster, spellcaster):
	r_idx = "695"
	name = "Zoth-Ommog"
	ascii = "R,u"
	level = 45
	rarity = 4
	hp = "25d70"
	speed = 120
	aaf = 12
	ac = 25
	sleep = 50
	exp = 18000
	blows = [('CRUSH', 'HURT', '25d3'), ('CRUSH', 'HURT', '25d3'), ('BITE', 'LOSE_DEX', '2d10'), ('BITE', 'LOSE_CON', '2d10')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_25', 'CAN_SWIM', 'ELDRITCH_HORROR', 'KILL_ITEM', 'BASH_DOOR', 'POWERFUL', 'EVIL', 'IM_ACID', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 5
	sflags = ['S_MONSTER', 'SCARE', 'CAUSE_4', 'S_HYDRA', 'S_SPIDER', 'BA_NETH', 'BA_POIS', 'BA_ACID', 'CAUSE_3', 'HOLD', 'MIND_BLAST']
	description = """"A body shaped like a road-based, truncated cone. A flat, blunt, 
wedge-shaped, vaguely reptilian head surmounts this conical torso, 
and the head is almost entirely hidden behind swirling tresses. 
This hair, or beard and mane, consists of thickly carved and 
coiling ropes, like serpents or worms... Through this repulsive 
Medusa-mane of ropy tendrils, two fierce, serpent-like eyes 
glare in a horrible intermingling of cold, inhuman mockery and 
what I can only describe as gloating menace."
"""

monsters.register_race(zoth_ommog)


class nazgul(monster, undead, spellcaster):
	r_idx = "696"
	name = "Nazgul"
	ascii = "W,D"
	level = 45
	rarity = 3
	hp = "66d50"
	speed = 120
	aaf = 90
	ac = 55
	sleep = 10
	exp = 10000
	blows = [('HIT', 'TERRIFY', '6d6'), ('HIT', 'TERRIFY', '6d6'), ('HIT', 'EXP_80', '4d6'), ('HIT', 'EXP_80', '4d6')]
	flags = ['MALE', 'FORCE_MAXHP', 'UNIQUE_7', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'RES_TELE', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'RES_NETH', 'SMART', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SCARE', 'HOLD', 'DRAIN_MANA', 'BR_NETH']
	description = """A tall black cloaked Ringwraith, radiating an aura of fear.
"""

monsters.register_race(nazgul)


class smaug_the_golden(dragon, spellcaster):
	r_idx = "697"
	name = "Smaug the Golden"
	ascii = "D,R"
	level = 45
	rarity = 2
	hp = "14d100"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 70
	exp = 19000
	blows = [('CLAW', 'HURT', '4d10'), ('CLAW', 'HURT', '4d10'), ('CLAW', 'HURT', '4d10'), ('BITE', 'HURT', '6d14')]
	flags = ['UNIQUE', 'MALE', 'REFLECTING', 'CAN_FLY', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['CONF', 'CAUSE_3', 'BR_FIRE']
	description = """Smaug is one of the Uruloki that still survive, a fire-drake of immense 
cunning and intelligence.  His speed through air is matched by few other 
dragons and his dragonfire is what legends are made of.
"""

monsters.register_race(smaug_the_golden)


class the_stormbringer(monster):
	r_idx = "698"
	name = "The Stormbringer"
	ascii = "|,D"
	level = 45
	rarity = 2
	hp = "13d93"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 20
	exp = 13333
	blows = [('WAIL', 'TERRIFY', ''), ('HIT', 'EXP_80', '64d1'), ('HIT', 'EXP_80', '64d1'), ('HIT', 'EXP_VAMP', '8d8')]
	flags = ['CHAR_MULTI', 'EVIL', 'IM_POIS', 'IM_COLD', 'IM_FIRE', 'RES_NETH', 'FORCE_SLEEP', 'UNIQUE', 'FORCE_MAXHP', 'CAN_FLY', 'COLD_BLOOD', 'BASH_DOOR', 'NONLIVING', 'WILD_DUNGEON_01', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """The mightiest of hellblades, a black runesword which thirsts for 
your soul.
"""

monsters.register_race(the_stormbringer)


class ultra_elite_paladin(monster, spellcaster):
	r_idx = "699"
	name = "Ultra-elite paladin"
	ascii = "p,w"
	level = 45
	rarity = 2
	hp = "77d7"
	speed = 120
	aaf = 20
	ac = 72
	sleep = 20
	exp = 1200
	blows = [('HIT', 'HURT', '3d12'), ('HIT', 'HURT', '3d12')]
	flags = ['WILD_DUNGEON_01', 'WILD_GRASS', 'WILD_FOREST1', 'WILD_WASTE1', 'IM_POIS', 'IM_FIRE', 'IM_ELEC', 'IM_ACID', 'IM_COLD', 'GOOD', 'RES_NETH', 'RES_NEXU', 'RES_DISE', 'RES_TELE', 'DROP_SKELETON', 'DROP_CORPSE', 'NO_SLEEP', 'NO_CONF', 'NO_FEAR', 'NO_STUN', 'DROP_1D2', 'DROP_90', 'DROP_60', 'FRIENDS', 'REFLECTING', 'MALE', 'OPEN_DOOR', 'BASH_DOOR', 'FORCE_MAXHP']
	sfreq = 12
	sflags = ['HEAL']
	description = """Fighting for a good cause, and they consider you an agent of the devil.
"""

monsters.register_race(ultra_elite_paladin)


class leprechaun_fanatic(monster, spellcaster):
	r_idx = "700"
	name = "Leprechaun fanatic"
	ascii = "h,D"
	level = 46
	rarity = 6
	hp = "5d5"
	speed = 123
	aaf = 8
	ac = 7
	sleep = 8
	exp = 80
	blows = [('EXPLODE', 'HURT', '20d3')]
	flags = ['MULTIPLY', 'RAND_25', 'TAKE_ITEM', 'SMART', 'WILD_DUNGEON_01', 'EVIL', 'OPEN_DOOR', 'MALE', 'NO_FEAR']
	sfreq = 6
	sflags = ['BLINK', 'TPORT', 'TELE_TO']
	description = """These leprechauns are not afraid to die for their cause. They are 
carrying explosives and making terrorist strikes...
"""

monsters.register_race(leprechaun_fanatic)


class dracolich(dragon, undead, spellcaster):
	r_idx = "701"
	name = "Dracolich"
	ascii = "D,B"
	level = 46
	rarity = 2
	hp = "18d100"
	speed = 120
	aaf = 25
	ac = 48
	sleep = 30
	exp = 18000
	blows = [('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('BITE', 'EXP_80', '3d6'), ('BITE', 'EXP_VAMP', '3d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'RES_TELE', 'COLD_BLOOD', 'CAN_FLY', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['CONF', 'SCARE', 'BR_COLD', 'BR_NETH']
	description = """The skeletal form of a once-great dragon, enchanted by magic most 
perilous.  Its animated form strikes with speed and drains life from its 
prey to satisfy its hunger.
"""

monsters.register_race(dracolich)


class greater_titan(giant, spellcaster):
	r_idx = "702"
	name = "Greater titan"
	ascii = "P,v"
	level = 46
	rarity = 3
	hp = "20d100"
	speed = 120
	aaf = 30
	ac = 50
	sleep = 15
	exp = 13500
	blows = [('HIT', 'CONFUSE', '12d12'), ('HIT', 'CONFUSE', '12d12'), ('HIT', 'CONFUSE', '12d12'), ('HIT', 'CONFUSE', '12d12')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'MOVE_BODY', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'GIANT', 'MALE']
	sfreq = 3
	sflags = ['HEAL', 'TELE_TO', 'S_MONSTERS']
	description = """A forty foot tall humanoid that shakes the ground as it walks.  The power 
radiating from its frame shakes your courage, its hatred inspired by your 
defiance.
"""

monsters.register_race(greater_titan)


class dracolisk(dragon, spellcaster):
	r_idx = "703"
	name = "Dracolisk"
	ascii = "D,r"
	level = 46
	rarity = 3
	hp = "18d100"
	speed = 120
	aaf = 25
	ac = 48
	sleep = 30
	exp = 14000
	blows = [('BITE', 'HURT', '5d12'), ('BITE', 'HURT', '5d12'), ('BITE', 'HURT', '8d8'), ('GAZE', 'PARALYZE', '')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'RES_NETH', 'RES_TELE', 'ANIMAL', 'EVIL', 'DRAGON', 'IM_ACID', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['HOLD', 'SCARE', 'BR_FIRE', 'BR_NETH']
	description = """A mixture of dragon and basilisk, the dracolisk stares at you with deep 
piercing eyes, its evil breath burning the ground where it stands.
"""

monsters.register_race(dracolisk)


class fastitocalon(dragon, spellcaster):
	r_idx = "704"
	name = "Fastitocalon"
	ascii = "D,g"
	level = 46
	rarity = 3
	hp = "45d70"
	speed = 120
	aaf = 25
	ac = 75
	sleep = 30
	exp = 16000
	blows = [('BITE', 'HURT', '5d8'), ('BITE', 'HURT', '5d8'), ('CRUSH', 'POISON', '3d10'), ('CRUSH', 'POISON', '3d10')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN', 'IM_FIRE', 'IM_ACID', 'IM_COLD', 'IM_POIS', 'DRAGON', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 6
	sflags = ['BR_FIRE', 'BR_ACID']
	description = """A huge aquatic dragon, its shell is as large as a small island. Some 
have called this creature a dragon-turtle.
"""

monsters.register_race(fastitocalon)


class spectral_tyrannosaurus(monster, undead, spellcaster):
	r_idx = "705"
	name = "Spectral tyrannosaurus"
	ascii = "R,G"
	level = 46
	rarity = 3
	hp = "50d50"
	speed = 120
	aaf = 25
	ac = 60
	sleep = 30
	exp = 15000
	blows = [('BITE', 'EXP_40', '2d13'), ('BITE', 'EXP_40', '2d13'), ('BITE', 'LOSE_STR', '5d8'), ('GAZE', 'TERRIFY', '')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'EVIL', 'UNDEAD', 'ANIMAL', 'RES_NEXU', 'RES_TELE', 'NO_CONF', 'NO_FEAR', 'NO_SLEEP', 'IM_POIS', 'IM_ACID', 'IM_COLD', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY']
	sfreq = 6
	sflags = ['HOLD', 'SCARE', 'BR_NEXU', 'BR_POIS', 'BR_NETH']
	description = """A deadly undead horror which looks like a skeletal tyrannosaurus 
surrounded by a sickly green glow.
"""

monsters.register_race(spectral_tyrannosaurus)


class yibb_tstll_the_patient_one(monster, spellcaster):
	r_idx = "706"
	name = "Yibb-Tstll the Patient One"
	ascii = "P,D"
	level = 46
	rarity = 2
	hp = "41d55"
	speed = 120
	aaf = 30
	ac = 50
	sleep = 20
	exp = 16000
	blows = [('TOUCH', 'LOSE_ALL', '1d166'), ('TOUCH', 'LOSE_ALL', '1d166')]
	flags = ['FORCE_SLEEP', 'ELDRITCH_HORROR', 'NEVER_MOVE', 'REGENERATE', 'WILD_DUNGEON_01', 'DROP_2D2', 'DROP_4D2', 'CAN_FLY', 'DROP_GOOD', 'ONLY_ITEM', 'POWERFUL', 'EVIL', 'CAN_SPEAK', 'UNIQUE', 'FORCE_MAXHP', 'SMART', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_ACID', 'IM_POIS']
	sfreq = 4
	sflags = ['BR_NUKE', 'S_DEMON', 'DARKNESS', 'S_UNDEAD', 'ANIM_DEAD', 'BR_NEXU', 'BR_CHAO', 'TELE_TO']
	description = """"There, about the pulsating body of the Ancient One, hugely 
winged reptilian creatures without faces cluttered and clutched 
at a multitude of blackly writhing, pendulous breasts! (Its eyes) 
moved quickly, independently -- sliding with vile viscosity over 
the whole rotten surface of Yib-Tstll's pulpy, glistening head!"
"""

monsters.register_race(yibb_tstll_the_patient_one)


class ghatanothoa(monster, spellcaster):
	r_idx = "707"
	name = "Ghatanothoa"
	ascii = "H,D"
	level = 46
	rarity = 2
	hp = "70d30"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 20
	exp = 16000
	blows = [('CLAW', 'LOSE_INT', '5d10'), ('CLAW', 'LOSE_WIS', '5d10'), ('BITE', 'CONFUSE', '5d10')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'ELDRITCH_HORROR', 'PASS_WALL', 'UNIQUE', 'FORCE_MAXHP', 'INVISIBLE', 'DROP_2D2', 'DROP_4D2', 'CAN_FLY', 'DROP_GOOD', 'ONLY_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'POWERFUL', 'EVIL', 'SMART', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_ACID', 'IM_POIS']
	sfreq = 4
	sflags = ['BR_WALL', 'TELE_AWAY', 'BR_GRAV', 'TELE_LEVEL', 'S_DEMON', 'S_UNDEAD', 'BRAIN_SMASH', 'HASTE', 'BLIND', 'BLINK', 'BR_INER', 'CAUSE_3', 'CAUSE_4']
	description = """The chief among the creatures known as Lloigor. Ghatanothoa has 
assumed a shape which is too horrible to describecould even adumbrate the loathsome, unholy, non-human, extra-galactic 
horror and hatefulness and unutterabl evil of that forbidden spawn 
of black chaos, and illimitable night."
"""

monsters.register_race(ghatanothoa)


class ent(giant):
	r_idx = "708"
	name = "Ent"
	ascii = "%,G"
	level = 46
	rarity = 3
	hp = "40d70"
	speed = 120
	aaf = 30
	ac = 60
	sleep = 15
	exp = 12500
	blows = [('CRUSH', 'HURT', '10d11'), ('CRUSH', 'HURT', '10d11'), ('CRUSH', 'HURT', '10d11'), ('CRUSH', 'HURT', '10d11')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'FRIENDLY', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'MOVE_BODY', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'GOOD', 'GIANT', 'MALE']
	sfreq = 0
	sflags = []
	description = """A treeherd; a sentient, moving tree. Its wrath is fearsome!
"""

monsters.register_race(ent)


class hru(giant):
	r_idx = "709"
	name = "Hru"
	ascii = "P,s"
	level = 56
	rarity = 3
	hp = "40d70"
	speed = 120
	aaf = 30
	ac = 75
	sleep = 15
	exp = 14500
	blows = [('CRUSH', 'SHATTER', '10d11'), ('CRUSH', 'SHATTER', '10d11'), ('CRUSH', 'SHATTER', '10d11'), ('CRUSH', 'SHATTER', '10d11')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'HURT_ROCK', 'WILD_DUNGEON_01', 'WILD_MOUNT1', 'WILD_MOUNT2', 'ONLY_GOLD', 'DROP_4D2', 'MOVE_BODY', 'KILL_WALL', 'SMART', 'KILL_ITEM', 'BASH_DOOR', 'EVIL', 'GIANT', 'MALE']
	sfreq = 0
	sflags = []
	description = """A rock giant, a being made of living stone.
"""

monsters.register_race(hru)


class itangast_the_fire_drake(dragon, spellcaster):
	r_idx = "710"
	name = "Itangast the Fire Drake"
	ascii = "D,r"
	level = 47
	rarity = 4
	hp = "22d70"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 70
	exp = 20000
	blows = [('CLAW', 'HURT', '1d10'), ('CLAW', 'HURT', '1d10'), ('BITE', 'HURT', '3d14'), ('BITE', 'HURT', '4d14')]
	flags = ['UNIQUE', 'MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['CONF', 'CAUSE_3', 'BR_FIRE']
	description = """A mighty ancient dragon, Itangast's form scorches your flesh.  Wisps of 
smoke curl up from his nostrils as he regards you with disdain.
"""

monsters.register_race(itangast_the_fire_drake)


class death_mold(monster):
	r_idx = "711"
	name = "Death mold"
	ascii = "m,D"
	level = 47
	rarity = 1
	hp = "70d20"
	speed = 140
	aaf = 100
	ac = 60
	sleep = 0
	exp = 1000
	blows = [('HIT', 'UN_BONUS', '7d7'), ('HIT', 'UN_BONUS', '7d7'), ('HIT', 'UN_BONUS', '7d7'), ('HIT', 'EXP_VAMP', '5d5')]
	flags = ['FORCE_SLEEP', 'NEVER_MOVE', 'WILD_DUNGEON_01', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """It is the epitome of all that is evil, in a mold.  Its lifeless form draws 
power from sucking the souls of those that approach it, a nimbus of pure 
evil surrounds it.  Luckily for you, it can't move.
"""

monsters.register_race(death_mold)


class fafner_the_dragon(dragon, spellcaster):
	r_idx = "712"
	name = "Fafner the Dragon"
	ascii = "D,G"
	level = 47
	rarity = 4
	hp = "25d83"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 70
	exp = 25000
	blows = [('CLAW', 'HURT', '4d10'), ('CLAW', 'HURT', '4d10'), ('BITE', 'FIRE', '14d6'), ('BITE', 'POISON', '14d6')]
	flags = ['UNIQUE', 'MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_SPEAK', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_90', 'DROP_60', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP', 'IM_POIS']
	sfreq = 3
	sflags = ['BR_FIRE', 'BR_POIS', 'SCARE', 'CAUSE_3', 'CONF']
	description = """The mighty dragon of the Norse myth, Fafner was a giant who slew his 
brother to win a treasure hoard, and then transformed himself into 
a dragon, greedily watching over his hoard.
"""

monsters.register_race(fafner_the_dragon)


class fangorn(giant):
	r_idx = "713"
	name = "Fangorn"
	ascii = "%,G"
	level = 47
	rarity = 3
	hp = "35d100"
	speed = 120
	aaf = 30
	ac = 60
	sleep = 15
	exp = 13500
	blows = [('CRUSH', 'HURT', '12d11'), ('CRUSH', 'HURT', '12d11'), ('CRUSH', 'HURT', '12d11'), ('CRUSH', 'HURT', '12d11')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'UNIQUE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'MOVE_BODY', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'GOOD', 'GIANT', 'FRIENDLY', 'MALE']
	sfreq = 0
	sflags = []
	description = """The oldest of all ents, a respected and feared ancient creature.
"""

monsters.register_race(fangorn)


class zhar_the_twin_obscenity(monster, spellcaster):
	r_idx = "714"
	name = "Zhar the Twin Obscenity"
	ascii = "R,D"
	level = 47
	rarity = 3
	hp = "35d100"
	speed = 120
	aaf = 30
	ac = 60
	sleep = 15
	exp = 15000
	blows = [('CRUSH', 'HURT', '23d13'), ('CRUSH', 'HURT', '23d13'), ('CRUSH', 'HURT', '23d13')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'UNIQUE', 'ELDRITCH_HORROR', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'KILL_BODY', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_SWIM', 'EVIL']
	sfreq = 6
	sflags = ['S_DEMON', 'SHRIEK', 'CONF', 'SCARE', 'MIND_BLAST', 'BLIND']
	description = """"The thing that crouched in the weird green dusk was a living mass 
of shuddering horror, a ghastly mountain of sensate, quivering 
flesh, whose tentacles, far-flung in the dim reaches of the 
subterranean cavern, emitted a strange humming sound, while from 
the depths of the creature's body came a weird and horrific 
undulation."
"""

monsters.register_race(zhar_the_twin_obscenity)


class glaurung_father_of_the_dragons(dragon, spellcaster):
	r_idx = "715"
	name = "Glaurung, Father of the Dragons"
	ascii = "D,r"
	level = 48
	rarity = 2
	hp = "17d100"
	speed = 120
	aaf = 20
	ac = 60
	sleep = 70
	exp = 25000
	blows = [('CLAW', 'HURT', '7d12'), ('CLAW', 'HURT', '7d12'), ('BITE', 'HURT', '8d14'), ('BITE', 'HURT', '8d14')]
	flags = ['UNIQUE', 'MALE', 'CAN_FLY', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'CAN_SPEAK', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['CONF', 'CAUSE_3', 'BR_FIRE', 'S_DRAGON']
	description = """Glaurung is the father of all dragons, and was for a long time the most 
powerful.  Nevertheless, he still has full command over his brood and can 
command them to appear whenever he so wishes.  He is the definition of 
dragonfire.
"""

monsters.register_race(glaurung_father_of_the_dragons)


class behemoth(monster, spellcaster):
	r_idx = "716"
	name = "Behemoth"
	ascii = "H,g"
	level = 49
	rarity = 3
	hp = "35d100"
	speed = 120
	aaf = 25
	ac = 90
	sleep = 30
	exp = 16000
	blows = [('BITE', 'FIRE', '5d8'), ('BITE', 'FIRE', '5d8'), ('CRUSH', 'HURT', '3d15'), ('CRUSH', 'HURT', '3d15')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_SWIM', 'ANIMAL', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'IM_FIRE', 'IM_ACID', 'IM_COLD', 'IM_POIS', 'DROP_CORPSE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 9
	sflags = ['BR_FIRE']
	description = """A great water-beast, with an almost unpenetrable skin.
"""

monsters.register_race(behemoth)


class garm_guardian_of_hel(monster, spellcaster):
	r_idx = "717"
	name = "Garm, Guardian of Hel"
	ascii = "C,u"
	level = 49
	rarity = 2
	hp = "30d70"
	speed = 120
	aaf = 20
	ac = 60
	sleep = 70
	exp = 25000
	blows = [('CLAW', 'HURT', '7d13'), ('CLAW', 'HURT', '7d13'), ('BITE', 'HURT', '8d13'), ('BITE', 'HURT', '8d13')]
	flags = ['UNIQUE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'RES_NETH', 'ANIMAL', 'EVIL', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BR_NETH', 'BR_COLD', 'BR_DARK', 'S_HOUND', 'S_UNDEAD']
	description = """Garm is a gigantic hound, whose job is to guard that none escapes 
the tortures of Hel, the Norse place of punishment for the wicked 
and cowardly dead.
"""

monsters.register_race(garm_guardian_of_hel)


class greater_wall_monster(monster):
	r_idx = "718"
	name = "Greater wall monster"
	ascii = "#,W"
	level = 50
	rarity = 4
	hp = "12d40"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 20
	exp = 900
	blows = [('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6'), ('HIT', 'HURT', '3d6')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'COLD_BLOOD', 'EMPTY_MIND', 'PASS_WALL', 'RAND_50', 'MULTIPLY', 'BASH_DOOR', 'IM_COLD', 'IM_ACID', 'IM_FIRE', 'IM_ELEC', 'IM_POIS', 'NONLIVING', 'HURT_ROCK', 'NO_CONF', 'NO_SLEEP', 'CHAR_MULTI', 'CAN_FLY']
	sfreq = 0
	sflags = []
	description = """A sentient, moving section of wall.
"""

monsters.register_race(greater_wall_monster)


class nycadaemon(demon, spellcaster):
	r_idx = "719"
	name = "Nycadaemon"
	ascii = "U,o"
	level = 50
	rarity = 3
	hp = "31d70"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 80
	exp = 10000
	blows = [('TOUCH', 'TERRIFY', ''), ('CLAW', 'HURT', '6d6'), ('CLAW', 'HURT', '6d6'), ('CLAW', 'HURT', '6d6')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'AURA_FIRE', 'NONLIVING', 'REGENERATE', 'IM_ACID', 'IM_COLD', 'IM_FIRE', 'CAN_FLY', 'NO_SLEEP', 'NO_STUN', 'NO_CONF', 'EVIL', 'DEMON', 'INVISIBLE', 'KILL_WALL', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD']
	sfreq = 4
	sflags = ['HOLD', 'BLINK', 'CONF', 'S_DEMON', 'BRAIN_SMASH', 'BR_NETH']
	description = """A loathsome, sturdy, winged, horned demon, with talons that could tear 
a stone wall down.
"""

monsters.register_race(nycadaemon)


class balrog(demon, spellcaster):
	r_idx = "720"
	name = "Balrog"
	ascii = "U,R"
	level = 50
	rarity = 3
	hp = "30d70"
	speed = 120
	aaf = 20
	ac = 40
	sleep = 80
	exp = 13000
	blows = [('HIT', 'FIRE', '2d6'), ('HIT', 'HURT', '5d6'), ('HIT', 'FIRE', '6d2'), ('HIT', 'HURT', '6d5')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'AURA_FIRE', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'NONLIVING', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'SMART', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP', 'KILL_WALL']
	sfreq = 4
	sflags = ['BLIND', 'CONF', 'BRAIN_SMASH', 'BR_FIRE', 'BO_PLAS', 'BA_NETH', 'S_HI_UNDEAD', 'S_UNDEAD', 'S_DEMON']
	description = """It is a massive humanoid demon wreathed in flames.
"""

monsters.register_race(balrog)


class goat_of_mendes(demon, spellcaster):
	r_idx = "721"
	name = "Goat of Mendes"
	ascii = "q,D"
	level = 50
	rarity = 3
	hp = "18d83"
	speed = 120
	aaf = 30
	ac = 33
	sleep = 40
	exp = 6666
	blows = [('GAZE', 'TERRIFY', ''), ('BUTT', 'HURT', '6d6'), ('BITE', 'EXP_40', ''), ('BITE', 'LOSE_CON', '')]
	flags = ['WILD_DUNGEON_01', 'EVIL', 'DEMON', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'SMART', 'NONLIVING', 'NO_CONF', 'NO_SLEEP', 'HURT_LITE', 'IM_FIRE', 'IM_COLD']
	sfreq = 4
	sflags = ['BLIND', 'CONF', 'BRAIN_SMASH', 'SCARE', 'ANIM_DEAD', 'BA_NETH', 'FORGET', 'S_UNDEAD', 'DRAIN_MANA', 'S_DEMON', 'CAUSE_4', 'BA_COLD']
	description = """It is a demonic creature from the lowest hell, vaguely resembling a 
large black he-goat.
"""

monsters.register_race(goat_of_mendes)


class nightwing(monster, undead, spellcaster):
	r_idx = "722"
	name = "Nightwing"
	ascii = "W,D"
	level = 50
	rarity = 4
	hp = "45d30"
	speed = 120
	aaf = 20
	ac = 60
	sleep = 10
	exp = 6000
	blows = [('TOUCH', 'POISON', '3d5'), ('TOUCH', 'POISON', '3d5'), ('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_BONUS', '6d8')]
	flags = ['FORCE_SLEEP', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'RES_TELE', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BLIND', 'SCARE', 'CAUSE_4', 'BRAIN_SMASH', 'BO_MANA', 'BO_NETH', 'BA_NETH', 'S_UNDEAD']
	description = """Everywhere colours seem paler and the air chiller.  At the centre of the 
cold stands a mighty figure.  Its wings envelop you in the chill of death 
as the nightwing reaches out to draw you into oblivion.  Your muscles sag 
and your mind loses all will to fight as you stand in awe of this mighty 
being.
"""

monsters.register_race(nightwing)


class maulotaur(monster, spellcaster):
	r_idx = "723"
	name = "Maulotaur"
	ascii = "H,u"
	level = 50
	rarity = 2
	hp = "188d13"
	speed = 130
	aaf = 13
	ac = 25
	sleep = 10
	exp = 4500
	blows = [('HIT', 'SHATTER', '8d8'), ('HIT', 'SHATTER', '8d8'), ('BUTT', 'HURT', '4d6'), ('BUTT', 'HURT', '4d6')]
	flags = ['ONLY_ITEM', 'DROP_60', 'DROP_GOOD', 'RES_TELE', 'WILD_DUNGEON_01', 'BASH_DOOR', 'STUPID', 'DROP_CORPSE', 'EVIL', 'IM_FIRE', 'FORCE_SLEEP', 'FORCE_MAXHP']
	sfreq = 5
	sflags = ['BO_PLAS', 'BA_FIRE']
	description = """It is a belligrent minotaur with some destructive magical arsenal, armed 
with a hammer.
"""

monsters.register_race(maulotaur)


class nether_hound(monster, spellcaster):
	r_idx = "724"
	name = "Nether hound"
	ascii = "Z,D"
	level = 51
	rarity = 2
	hp = "35d10"
	speed = 120
	aaf = 30
	ac = 40
	sleep = 0
	exp = 5000
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'RES_NETH', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'WILD_DUNGEON_01', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BR_NETH']
	description = """You feel a soul-tearing chill upon viewing this beast, a ghostly form of 
darkness in the shape of a large dog.
"""

monsters.register_race(nether_hound)


class time_hound(monster, spellcaster):
	r_idx = "725"
	name = "Time hound"
	ascii = "Z,G"
	level = 51
	rarity = 4
	hp = "35d10"
	speed = 130
	aaf = 30
	ac = 40
	sleep = 0
	exp = 5000
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['BR_TIME']
	description = """You get a terrible sense of deja vu, or is it a premonition?  All at once 
you see a little puppy and a toothless old dog.  Perhaps you should give 
up and go to bed.
"""

monsters.register_race(time_hound)


class plasma_hound(monster, spellcaster):
	r_idx = "726"
	name = "Plasma hound"
	ascii = "Z,R"
	level = 51
	rarity = 2
	hp = "35d10"
	speed = 120
	aaf = 30
	ac = 40
	sleep = 0
	exp = 5000
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'WILD_DUNGEON_01', 'FRIENDS', 'RES_PLAS', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BR_PLAS']
	description = """The very air warps as pure elemental energy stalks towards you in the 
shape of a giant hound.  Your hair stands on end and your palms itch as 
you sense trouble.
"""

monsters.register_race(plasma_hound)


class demonic_quylthulg(monster, spellcaster):
	r_idx = "727"
	name = "Demonic quylthulg"
	ascii = "Q,R"
	level = 51
	rarity = 1
	hp = "28d10"
	speed = 120
	aaf = 20
	ac = 1
	sleep = 0
	exp = 3000
	blows = []
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'INVISIBLE', 'EMPTY_MIND', 'WILD_DUNGEON_01', 'ANIMAL', 'EVIL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['BLINK', 'TPORT', 'S_DEMON']
	description = """A pile of pulsing flesh that glows with an inner hellish fire.  The world 
itself seems to cry out against it.
"""

monsters.register_race(demonic_quylthulg)


class great_storm_wyrm(dragon, spellcaster):
	r_idx = "728"
	name = "Great storm wyrm"
	ascii = "D,b"
	level = 51
	rarity = 2
	hp = "17d100"
	speed = 120
	aaf = 30
	ac = 65
	sleep = 80
	exp = 17000
	blows = [('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('BITE', 'ELEC', '5d14')]
	flags = ['WILD_DUNGEON_01', 'WILD_MOUNT2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'AURA_ELEC', 'CAN_FLY', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'DROP_CORPSE', 'EVIL', 'DRAGON', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_ELEC']
	description = """A vast dragon of power.  Storms and lightning crash around its titanic 
form.  Deep blue scales reflect the flashes and highlight the creature's 
great muscles.  It regards you with contempt.
"""

monsters.register_race(great_storm_wyrm)


class ulik_the_troll(troll):
	r_idx = "729"
	name = "Ulik the Troll"
	ascii = "T,o"
	level = 51
	rarity = 4
	hp = "35d70"
	speed = 130
	aaf = 30
	ac = 60
	sleep = 30
	exp = 18000
	blows = [('HIT', 'SHATTER', '20d12'), ('HIT', 'SHATTER', '20d12'), ('BITE', 'POISON', '6d14')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_90', 'REGENERATE', 'KILL_WALL', 'RES_TELE', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'TROLL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'IM_ELEC', 'IM_COLD', 'IM_FIRE']
	sfreq = 0
	sflags = []
	description = """Ulik is the strongest troll who has ever lived. He could challenge 
the immortals and pound them to dust with his great strength.
"""

monsters.register_race(ulik_the_troll)


class baphomet_the_minotaur_lord(monster, spellcaster):
	r_idx = "730"
	name = "Baphomet the Minotaur Lord"
	ascii = "H,D"
	level = 51
	rarity = 4
	hp = "19d100"
	speed = 130
	aaf = 30
	ac = 48
	sleep = 30
	exp = 18000
	blows = [('BUTT', 'HURT', '12d13'), ('BUTT', 'HURT', '12d13'), ('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '10d10')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'DROP_CORPSE', 'EVIL', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['SLOW', 'ARROW_4', 'BO_MANA', 'BO_PLAS', 'BA_ELEC', 'BR_WALL']
	description = """A fearsome bull-headed demon, Baphomet swings a mighty axe as he curses 
all that defy him.
"""

monsters.register_race(baphomet_the_minotaur_lord)


class hell_knight(monster, spellcaster):
	r_idx = "731"
	name = "Hell knight"
	ascii = "p,s"
	level = 52
	rarity = 1
	hp = "15d70"
	speed = 120
	aaf = 20
	ac = 65
	sleep = 10
	exp = 10000
	blows = [('HIT', 'HURT', '10d5'), ('HIT', 'HURT', '10d5'), ('HIT', 'HURT', '10d5'), ('HIT', 'EXP_80', '')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'RES_NETH', 'RES_NEXU', 'RES_PLAS', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL']
	sfreq = 5
	sflags = ['BLIND', 'SCARE', 'CAUSE_3', 'BA_NETH', 'BA_FIRE', 'BO_PLAS', 'S_MONSTERS']
	description = """It is a humanoid form dressed in armour of ancient style.  From beneath 
its helmet, eyes glow with hellfire.
"""

monsters.register_race(hell_knight)


class bull_gates(monster, spellcaster):
	r_idx = "732"
	name = "Bull Gates"
	ascii = "p,D"
	level = 52
	rarity = 3
	hp = "25d70"
	speed = 140
	aaf = 40
	ac = 45
	sleep = 0
	exp = 20000
	blows = [('CHARGE', 'EAT_GOLD', '5d5'), ('CHARGE', 'EAT_ITEM', '5d5'), ('SPIT', 'BLIND', '10d5'), ('DROOL', 'DISEASE', '8d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'DROP_GOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'IM_POIS', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP', 'EVIL', 'RES_TELE']
	sfreq = 6
	sflags = ['TRAPS']
	description = """He may not code worth a dime, but he certainly knows how to make money.
"""

monsters.register_race(bull_gates)


class santa_claus(monster, spellcaster):
	r_idx = "733"
	name = "Santa Claus"
	ascii = "h,r"
	level = 52
	rarity = 3
	hp = "25d70"
	speed = 150
	aaf = 90
	ac = 50
	sleep = 10
	exp = 45000
	blows = [('HIT', 'LOSE_CHR', '5d5'), ('TOUCH', 'LOSE_ALL', '10d1'), ('TOUCH', 'LOSE_ALL', '10d1'), ('CHARGE', 'EAT_GOLD', '')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'REFLECTING', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'NO_STUN', 'NO_SLEEP', 'NO_CONF', 'IM_ELEC', 'IM_FIRE', 'IM_POIS', 'IM_COLD', 'COLD_BLOOD', 'RES_TELE']
	sfreq = 3
	sflags = ['CAUSE_4', 'HOLD', 'DRAIN_MANA', 'SCARE', 'BLIND', 'S_UNDEAD', 'S_HI_UNDEAD', 'S_HI_DRAGON', 'S_UNIQUE', 'BA_NETH', 'FORGET', 'TRAPS', 'BRAIN_SMASH', 'TELE_AWAY']
	description = """Why would anybody want to kill Santa Claus? To get all the presents, 
of course!
"""

monsters.register_race(santa_claus)


class eihort_the_thing_in_the_labyrinth(monster, spellcaster):
	r_idx = "734"
	name = "Eihort, the Thing in the Labyrinth"
	ascii = "j,R"
	level = 53
	rarity = 3
	hp = "33d70"
	speed = 120
	aaf = 50
	ac = 45
	sleep = 10
	exp = 45000
	blows = [('BITE', 'PARALYZE', '8d1'), ('CRUSH', 'HURT', '10d10'), ('CRUSH', 'HURT', '10d10'), ('CRUSH', 'HURT', '10d10')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'CAN_SPEAK', 'ESCORT', 'REGENERATE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ELDRITCH_HORROR', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 4
	sflags = ['S_UNDEAD', 'S_DEMON', 'S_MONSTER', 'BLINK', 'BA_ACID', 'FORGET']
	description = """"Then came pale movement in the well, and something clambered 
up from the dark, a bloated blanched oval supported on myriad 
fleshless legs. Eyes formed in the gelatinous oval and stared 
at him."
"""

monsters.register_race(eihort_the_thing_in_the_labyrinth)


class the_king_in_yellow(monster, undead, spellcaster):
	r_idx = "735"
	name = "The King in Yellow"
	ascii = "L,y"
	level = 53
	rarity = 3
	hp = "32d70"
	speed = 120
	aaf = 90
	ac = 50
	sleep = 10
	exp = 50000
	blows = [('CRUSH', 'HURT', '12d12'), ('CRUSH', 'HURT', '12d12'), ('GAZE', 'LOSE_WIS', '5d10'), ('GAZE', 'TERRIFY', '5d10')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'ATTR_MULTI', 'ATTR_ANY', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ELDRITCH_HORROR', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'UNDEAD', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 2
	sflags = ['S_DEMON', 'S_UNDEAD', 'S_KIN', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'ANIM_DEAD', 'TELE_AWAY', 'TPORT', 'BRAIN_SMASH', 'CONF']
	description = """The King in Yellow is an Avatar of Hasturface, and is twice as tall as a man. He wears pointed shoes under 
his tattered, fantastically colored robes, and a streamer of silk 
appears to fall from the pointed tip of his hood. At times he appears 
to be winged; at others, haloed."
"""

monsters.register_race(the_king_in_yellow)


class great_unclean_one(demon, spellcaster):
	r_idx = "736"
	name = "Great unclean one"
	ascii = "U,g"
	level = 53
	rarity = 3
	hp = "124d40"
	speed = 120
	aaf = 30
	ac = 75
	sleep = 20
	exp = 17500
	blows = [('BITE', 'DISEASE', '10d10'), ('BITE', 'ACID', '10d10'), ('BITE', 'POISON', '10d10'), ('BITE', 'CONFUSE', '10d10')]
	flags = ['WILD_DUNGEON_01', 'DEMON', 'EVIL', 'NONLIVING', 'CAN_SWIM', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'ELDRITCH_HORROR', 'OPEN_DOOR', 'BASH_DOOR', 'SMART', 'POWERFUL', 'DROP_GOOD', 'KILL_BODY', 'IM_ACID', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'KILL_ITEM']
	sfreq = 3
	sflags = ['BR_ACID', 'BR_POIS', 'BR_NUKE', 'SCARE', 'ANIM_DEAD', 'CAUSE_3', 'CAUSE_4', 'S_DEMON', 'S_UNDEAD']
	description = """This disgusting demon resembles a shambling pile of rotting 
green flesh, with dozens of mouths drooling and leaving a 
trail of foul-smelling goo behind. Nurgle must be 
proud of himself for having created this atrocity!
"""

monsters.register_race(great_unclean_one)


class lord_of_chaos(monster, spellcaster):
	r_idx = "737"
	name = "Lord of Chaos"
	ascii = "p,v"
	level = 53
	rarity = 3
	hp = "45d41"
	speed = 130
	aaf = 30
	ac = 40
	sleep = 5
	exp = 17500
	blows = [('KICK', 'HURT', '20d2'), ('KICK', 'HURT', '10d2'), ('HIT', 'POISON', '20d1'), ('HIT', 'LOSE_ALL', '15d1')]
	flags = ['MALE', 'EVIL', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'ONLY_ITEM', 'DROP_4D2', 'ATTR_MULTI', 'SHAPECHANGER', 'ATTR_ANY', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'SMART', 'POWERFUL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HEAL', 'MIND_BLAST', 'BA_CHAO', 'S_SPIDER', 'S_HOUND', 'S_DEMON']
	description = """He is one of the few true masters of the art, being extremely skillful in 
all forms of unarmed combat and controlling the Logrus 
with disdainful ease.
"""

monsters.register_race(lord_of_chaos)


class khamul_the_easterling(monster, undead, spellcaster):
	r_idx = "738"
	name = "Khamul the Easterling"
	ascii = "W,D"
	level = 53
	rarity = 3
	hp = "35d70"
	speed = 120
	aaf = 90
	ac = 50
	sleep = 10
	exp = 50000
	blows = [('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '5d5'), ('TOUCH', 'EXP_40', ''), ('TOUCH', 'EXP_40', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'UNDEAD', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 2
	sflags = ['TELE_LEVEL', 'BLIND', 'HOLD', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'BO_MANA', 'BA_FIRE', 'BA_COLD', 'BA_NETH', 'ANIM_DEAD', 'S_UNDEAD', 'S_KIN']
	description = """A warrior-king of the East.  Khamul is a powerful opponent, his skill in 
combat awesome and his form twisted by evil cunning.
"""

monsters.register_race(khamul_the_easterling)


class hound_of_tindalos(monster, spellcaster):
	r_idx = "739"
	name = "Hound of Tindalos"
	ascii = "Z,s"
	level = 54
	rarity = 3
	hp = "45d15"
	speed = 120
	aaf = 30
	ac = 50
	sleep = 0
	exp = 8000
	blows = [('BITE', 'TIME', '2d12'), ('BITE', 'TIME', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'RES_NETH', 'WILD_DUNGEON_01', 'INVISIBLE', 'PASS_WALL', 'EVIL', 'CAN_FLY', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BLINK', 'TELE_TO', 'BR_NETH', 'BR_TIME']
	description = """"They are lean and athirst!... All the evil in the universe
was concentrated in their lean, hungry bodies. Or had they 
bodies? I saw them only for a moment, I cannot be certain."
"""

monsters.register_race(hound_of_tindalos)


class lesser_kraken(monster, spellcaster):
	r_idx = "740"
	name = "Lesser kraken"
	ascii = "l,G"
	level = 54
	rarity = 2
	hp = "112d22"
	speed = 120
	aaf = 30
	ac = 75
	sleep = 80
	exp = 20000
	blows = [('CRUSH', 'HURT', '16d12'), ('CRUSH', 'HURT', '16d12'), ('CRUSH', 'HURT', '16d12'), ('CRUSH', 'HURT', '16d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'SMART', 'RES_WATE', 'EVIL', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP', 'IM_POIS', 'IM_FIRE']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_4', 'CAUSE_3', 'BA_WATE', 'DARKNESS', 'BR_DARK', 'TELE_TO']
	description = """An enormously fearsome and powerful inhabitant of the depths. It 
resembles a gargantuan octopus and its evil is almost tangible.
"""

monsters.register_race(lesser_kraken)


class great_ice_wyrm(dragon, spellcaster):
	r_idx = "741"
	name = "Great ice wyrm"
	ascii = "D,w"
	level = 54
	rarity = 2
	hp = "30d70"
	speed = 120
	aaf = 30
	ac = 85
	sleep = 80
	exp = 20000
	blows = [('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('BITE', 'COLD', '5d14')]
	flags = ['WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'AURA_COLD', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'DROP_CORPSE', 'EVIL', 'DRAGON', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_COLD']
	description = """An immense dragon capable of awesome destruction.  You have never felt 
such extreme cold, or witnessed such an icy stare.  Begone quickly or feel 
its wrath!
"""

monsters.register_race(great_ice_wyrm)


class demilich(monster, undead, spellcaster):
	r_idx = "742"
	name = "Demilich"
	ascii = "L,U"
	level = 54
	rarity = 2
	hp = "30d70"
	speed = 120
	aaf = 20
	ac = 50
	sleep = 50
	exp = 12500
	blows = [('TOUCH', 'EXP_VAMP', '1d12'), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'LOSE_DEX', '4d12'), ('TOUCH', 'LOSE_DEX', '4d12')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'RES_TELE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_1D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLINK', 'TELE_TO', 'BLIND', 'HOLD', 'CONF', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'S_HI_UNDEAD', 'S_UNDEAD', 'FORGET', 'TPORT', 'HEAL', 'ANIM_DEAD']
	description = """A lich who is partially immaterial, on its way to a new, ethereal form.
"""

monsters.register_race(demilich)


class the_phoenix(monster, spellcaster):
	r_idx = "743"
	name = "The Phoenix"
	ascii = "B,r"
	level = 54
	rarity = 3
	hp = "36d70"
	speed = 120
	aaf = 60
	ac = 65
	sleep = 0
	exp = 40000
	blows = [('BITE', 'FIRE', '12d6'), ('BITE', 'FIRE', '12d6'), ('HIT', 'FIRE', '9d12'), ('HIT', 'FIRE', '9d12')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'RES_PLAS', 'AURA_FIRE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'GOOD', 'ANIMAL', 'IM_ACID', 'IM_FIRE', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BO_FIRE', 'BO_PLAS', 'BA_FIRE', 'BR_FIRE', 'BR_LITE', 'BR_PLAS']
	description = """A massive glowing eagle bathed in flames.  The searing heat chars your 
skin and melts your armour.
"""

monsters.register_race(the_phoenix)


class nightcrawler(monster, undead, spellcaster):
	r_idx = "744"
	name = "Nightcrawler"
	ascii = "W,D"
	level = 54
	rarity = 4
	hp = "60d60"
	speed = 120
	aaf = 20
	ac = 80
	sleep = 10
	exp = 8100
	blows = [('STING', 'LOSE_CON', '8d8'), ('STING', 'LOSE_CON', '8d8'), ('BITE', 'ACID', '10d10'), ('BITE', 'ACID', '10d10')]
	flags = ['FORCE_SLEEP', 'SMART', 'KILL_WALL', 'CAN_SWIM', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'RES_TELE', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BLIND', 'SCARE', 'BRAIN_SMASH', 'BO_MANA', 'BO_NETH', 'BA_NETH', 'BR_NETH', 'S_UNDEAD']
	description = """This intensely evil creature bears the form of a gargantuan black worm.  
Its gaping maw is a void of blackness, acid drips from its steely hide.  
It is like nothing you have ever seen before, and a terrible chill runs 
down your spine as you face it.
"""

monsters.register_race(nightcrawler)


class lord_of_change(demon, spellcaster):
	r_idx = "745"
	name = "Lord of Change"
	ascii = "B,v"
	level = 54
	rarity = 3
	hp = "38d70"
	speed = 130
	aaf = 30
	ac = 75
	sleep = 20
	exp = 17000
	blows = [('CLAW', 'CONFUSE', '10d10'), ('CLAW', 'CONFUSE', '10d10'), ('BITE', 'BLIND', '12d12')]
	flags = ['WILD_DUNGEON_01', 'DEMON', 'EVIL', 'ATTR_MULTI', 'DROP_GOOD', 'MOVE_BODY', 'NONLIVING', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'ELDRITCH_HORROR', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'SMART', 'POWERFUL', 'CAN_FLY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['MIND_BLAST', 'BA_CHAO', 'SCARE', 'BRAIN_SMASH', 'DRAIN_MANA', 'S_HOUND', 'S_DEMON', 'S_DRAGON', 'TPORT', 'HASTE', 'CONF', 'TELE_AWAY', 'FORGET']
	description = """The most powerful of Tzeentch's servitors. This demon looks like 
a huge bird-creature, with the head of a predatory bird and 
fantastically multi-coloured wings. 
"""

monsters.register_race(lord_of_change)


class keeper_of_secrets(demon, spellcaster):
	r_idx = "746"
	name = "Keeper of Secrets"
	ascii = "H,v"
	level = 54
	rarity = 3
	hp = "45d70"
	speed = 130
	aaf = 30
	ac = 75
	sleep = 20
	exp = 17000
	blows = [('HIT', 'CONFUSE', '10d10'), ('HIT', 'TERRIFY', '10d10'), ('HIT', 'BLIND', '10d10'), ('HIT', 'TERRIFY', '10d10')]
	flags = ['DEMON', 'EVIL', 'DROP_GOOD', 'NONLIVING', 'CAN_SWIM', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'ELDRITCH_HORROR', 'OPEN_DOOR', 'BASH_DOOR', 'SMART', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['SCARE', 'BRAIN_SMASH', 'DRAIN_MANA', 'BR_CONF', 'S_DEMON', 'S_UNDEAD', 'TPORT', 'HEAL', 'TELE_AWAY']
	description = """This demonic keeper of forbidden secrets looks like a hairless 
minotaur with extra arms, decorated with tattoos and nose rings. 
It is the embodiment of Slaanesh's perverted magic.
"""

monsters.register_race(keeper_of_secrets)


class shudde_mell(monster, spellcaster):
	r_idx = "747"
	name = "Shudde M'ell"
	ascii = "w,s"
	level = 54
	rarity = 2
	hp = "70d40"
	speed = 125
	aaf = 20
	ac = 45
	sleep = 20
	exp = 23000
	blows = [('CRUSH', 'SHATTER', '55d2'), ('CRUSH', 'SHATTER', '55d2'), ('TOUCH', 'LOSE_CON', '1d2'), ('TOUCH', 'LOSE_CON', '1d2')]
	flags = ['WILD_DUNGEON_01', 'IM_FIRE', 'RES_PLAS', 'IM_COLD', 'IM_POIS', 'ELDRITCH_HORROR', 'RES_TELE', 'KILL_WALL', 'ONLY_GOLD', 'DROP_4D2', 'DROP_2D2', 'CAN_SWIM', 'EVIL', 'FORCE_MAXHP', 'SMART', 'UNIQUE']
	sfreq = 5
	sflags = ['SCARE', 'CONF', 'HOLD', 'S_DEMON', 'S_KIN', 'HEAL', 'HASTE', 'FORGET', 'BRAIN_SMASH', 'BR_DARK', 'BR_ACID', 'BR_DISE']
	description = """The most powerful and most evil of all Chthonians. 
"A great gray thing a mile long chanting and exuding strange acids... 
charging through the depths of the earth at a fantastic speed, in a 
dreadful fury... melting basaltic rocks like butter under a blowtorch."
"""

monsters.register_race(shudde_mell)


class hand_druj(monster, undead, spellcaster):
	r_idx = "748"
	name = "Hand druj"
	ascii = "s,D"
	level = 55
	rarity = 4
	hp = "60d7"
	speed = 130
	aaf = 20
	ac = 55
	sleep = 10
	exp = 12000
	blows = []
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'RES_TELE', 'SMART', 'COLD_BLOOD', 'CAN_SWIM', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 1
	sflags = ['TELE_AWAY', 'BLIND', 'CONF', 'SCARE', 'CAUSE_3', 'FORGET', 'DARKNESS']
	description = """A skeletal hand floating in the air, motionless except for its flexing 
fingers.
"""

monsters.register_race(hand_druj)


class eye_druj(monster, undead, spellcaster):
	r_idx = "749"
	name = "Eye druj"
	ascii = "s,D"
	level = 55
	rarity = 4
	hp = "10d70"
	speed = 130
	aaf = 20
	ac = 45
	sleep = 10
	exp = 24000
	blows = [('GAZE', 'EXP_80', ''), ('GAZE', 'EXP_80', '')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'RES_TELE', 'SMART', 'COLD_BLOOD', 'EVIL', 'UNDEAD', 'CAN_SWIM', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 1
	sflags = ['BO_MANA', 'BO_NETH', 'BA_NETH', 'S_UNDEAD']
	description = """A bloodshot eyeball floating in the air, you'd be forgiven for assuming it 
harmless.
"""

monsters.register_race(eye_druj)


class skull_druj(monster, undead, spellcaster):
	r_idx = "750"
	name = "Skull druj"
	ascii = "s,D"
	level = 55
	rarity = 4
	hp = "14d70"
	speed = 130
	aaf = 20
	ac = 60
	sleep = 10
	exp = 25000
	blows = [('BITE', 'EXP_80', '4d4'), ('BITE', 'PARALYZE', '4d4'), ('BITE', 'LOSE_INT', '4d4'), ('BITE', 'LOSE_WIS', '4d4')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'RES_TELE', 'SMART', 'COLD_BLOOD', 'EVIL', 'UNDEAD', 'CAN_SWIM', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 1
	sflags = ['SLOW', 'CAUSE_4', 'MIND_BLAST', 'BRAIN_SMASH', 'TRAPS', 'BO_PLAS', 'BO_NETH', 'BA_WATE', 'S_UNDEAD']
	description = """A glowing skull possessed by sorcerous power.  It need not move, but 
merely blast you with mighty magic.
"""

monsters.register_race(skull_druj)


class chaos_vortex(monster, spellcaster):
	r_idx = "751"
	name = "Chaos vortex"
	ascii = "v,v"
	level = 55
	rarity = 1
	hp = "24d20"
	speed = 140
	aaf = 100
	ac = 40
	sleep = 0
	exp = 4000
	blows = []
	flags = ['ATTR_MULTI', 'ATTR_ANY', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'NEVER_BLOW', 'RAND_50', 'RAND_25', 'CAN_FLY', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING']
	sfreq = 6
	sflags = ['BR_CHAO']
	description = """Void, nothingness, spinning destructively.
"""

monsters.register_race(chaos_vortex)


class aether_vortex(monster, spellcaster):
	r_idx = "752"
	name = "Aether vortex"
	ascii = "v,B"
	level = 55
	rarity = 2
	hp = "24d20"
	speed = 130
	aaf = 100
	ac = 20
	sleep = 0
	exp = 4500
	blows = [('ENGULF', 'ELEC', '5d5'), ('ENGULF', 'FIRE', '3d3'), ('ENGULF', 'ACID', '3d3'), ('ENGULF', 'COLD', '3d3')]
	flags = ['ATTR_MULTI', 'ATTR_ANY', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'CAN_FLY', 'RAND_50', 'RAND_25', 'AURA_COLD', 'AURA_FIRE', 'AURA_ELEC', 'EMPTY_MIND', 'BASH_DOOR', 'POWERFUL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NONLIVING', 'RES_NETH', 'RES_NEXU', 'RES_PLAS']
	sfreq = 6
	sflags = ['BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_POIS', 'BR_LITE', 'BR_DARK', 'BR_SOUN', 'BR_CONF', 'BR_CHAO', 'BR_SHAR', 'BR_NETH', 'BR_WALL', 'BR_INER', 'BR_TIME', 'BR_GRAV', 'BR_PLAS', 'BR_NEXU']
	description = """An awesome vortex of pure magic, power radiates from its frame.
"""

monsters.register_race(aether_vortex)


class nidhogg_the_hel_drake(dragon, spellcaster):
	r_idx = "753"
	name = "Nidhogg the Hel-Drake"
	ascii = "D,D"
	level = 55
	rarity = 2
	hp = "29d111"
	speed = 120
	aaf = 20
	ac = 66
	sleep = 70
	exp = 25000
	blows = [('CLAW', 'LOSE_CON', '8d12'), ('CLAW', 'LOSE_CON', '8d12'), ('BITE', 'EXP_VAMP', '8d15'), ('BITE', 'EXP_VAMP', '8d15')]
	flags = ['UNIQUE', 'CAN_FLY', 'RES_NETH', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'IM_POIS', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'CAN_SPEAK', 'IM_ACID', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP', 'IM_COLD']
	sfreq = 5
	sflags = ['CAUSE_3', 'BR_NETH', 'BR_COLD', 'BR_ACID', 'BR_POIS', 'S_DRAGON', 'S_UNDEAD']
	description = """In the bowels of Hel, the dread Nidhogg, a dragon blacker than the 
night, feasts on the essences of the dead.
"""

monsters.register_race(nidhogg_the_hel_drake)


class the_lernean_hydra(monster, spellcaster):
	r_idx = "754"
	name = "The Lernean Hydra"
	ascii = "M,G"
	level = 55
	rarity = 2
	hp = "45d70"
	speed = 120
	aaf = 20
	ac = 70
	sleep = 20
	exp = 20000
	blows = [('BITE', 'POISON', '8d6'), ('BITE', 'POISON', '8d6'), ('BITE', 'FIRE', '12d6'), ('BITE', 'FIRE', '12d6')]
	flags = ['UNIQUE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'DROP_CORPSE', 'ONLY_GOLD', 'DROP_3D2', 'DROP_4D2', 'WILD_SHORE', 'WILD_SWAMP2', 'OPEN_DOOR', 'BASH_DOOR', 'KILL_BODY', 'POWERFUL', 'ANIMAL', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'CAN_SWIM']
	sfreq = 3
	sflags = ['SCARE', 'BO_FIRE', 'BO_PLAS', 'BA_FIRE', 'BA_POIS', 'BR_FIRE', 'BR_POIS', 'S_HYDRA']
	description = """A massive legendary hydra.  It has twelve powerful heads.  Its many eyes 
stare at you as clouds of smoke and poisonous vapour rise from its 
seething form.
"""

monsters.register_race(the_lernean_hydra)


class thuringwethil(monster, undead, spellcaster):
	r_idx = "755"
	name = "Thuringwethil"
	ascii = "V,D"
	level = 55
	rarity = 4
	hp = "40d70"
	speed = 130
	aaf = 20
	ac = 72
	sleep = 10
	exp = 23000
	blows = [('BITE', 'HURT', '5d8'), ('BITE', 'EXP_VAMP', '6d6'), ('HIT', 'CONFUSE', '6d6'), ('HIT', 'CONFUSE', '6d6')]
	flags = ['UNIQUE', 'FEMALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'CAN_SPEAK', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 3
	sflags = ['BLIND', 'HOLD', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'BA_NETH', 'S_KIN']
	description = """Chief messenger between Sauron and Morgoth, she is surely the most deadly 
of her vampire race.  At first she is charming to meet, but her wings and 
eyes give away her true form.
"""

monsters.register_race(thuringwethil)


class great_hell_wyrm(dragon, spellcaster):
	r_idx = "756"
	name = "Great hell wyrm"
	ascii = "D,r"
	level = 55
	rarity = 2
	hp = "54d70"
	speed = 120
	aaf = 40
	ac = 65
	sleep = 40
	exp = 23000
	blows = [('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('CLAW', 'HURT', '4d12'), ('BITE', 'HURT', '5d14')]
	flags = ['WILD_DUNGEON_01', 'WILD_MOUNT2', 'WILD_WASTE2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'AURA_FIRE', 'EVIL', 'DRAGON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_FIRE']
	description = """A vast dragon of immense power.  Fire leaps continuously from its huge 
form.  The air around it scalds you.  Its slightest glance burns you, and 
you truly realize how insignificant you are.
"""

monsters.register_race(great_hell_wyrm)


class hastur_the_unspeakable(monster, undead, spellcaster):
	r_idx = "757"
	name = "Hastur the Unspeakable"
	ascii = "H,b"
	level = 55
	rarity = 4
	hp = "55d71"
	speed = 120
	aaf = 20
	ac = 75
	sleep = 10
	exp = 23000
	blows = [('CRUSH', 'HURT', '14d8'), ('CRUSH', 'HURT', '14d8'), ('BITE', 'EXP_80', '6d6'), ('BITE', 'EXP_80', '6d6')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'ELDRITCH_HORROR', 'CAN_SWIM', 'EVIL', 'UNDEAD', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'CAN_SPEAK', 'AURA_ELEC', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'NONLIVING', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 3
	sflags = ['BLIND', 'HOLD', 'SCARE', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'BA_WATE', 'S_DEMON', 'HASTE', 'TPORT', 'TELE_AWAY', 'TELE_TO', 'HEAL', 'BR_DARK', 'BR_NETH']
	description = """Its form is partially that of a 
reptile, partially that of a gigantic octopus. It will destroy you.
"""

monsters.register_race(hastur_the_unspeakable)


class bloodthirster(demon):
	r_idx = "758"
	name = "Bloodthirster"
	ascii = "U,r"
	level = 55
	rarity = 3
	hp = "45d70"
	speed = 144
	aaf = 30
	ac = 90
	sleep = 20
	exp = 19500
	blows = [('HIT', 'HURT', '50d1'), ('HIT', 'HURT', '50d1'), ('HIT', 'HURT', '50d1')]
	flags = ['WILD_DUNGEON_01', 'DEMON', 'EVIL', 'DROP_GOOD', 'REGENERATE', 'CAN_FLY', 'NONLIVING', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_4D2', 'ELDRITCH_HORROR', 'OPEN_DOOR', 'BASH_DOOR', 'RES_NETH', 'RES_NEXU', 'RES_TELE', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """Khorne's mightiest servant, a winged hound-demon walking on 
two paws and wielding a mighty axe and a whip in the other 
two. Intelligent, bloodthirsty eyes behold you from inside 
the bloody demon armour.
"""

monsters.register_race(bloodthirster)


class draconic_quylthulg(monster, spellcaster):
	r_idx = "759"
	name = "Draconic quylthulg"
	ascii = "Q,G"
	level = 55
	rarity = 3
	hp = "72d7"
	speed = 120
	aaf = 20
	ac = 1
	sleep = 0
	exp = 5500
	blows = []
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'INVISIBLE', 'EMPTY_MIND', 'WILD_DUNGEON_01', 'ANIMAL', 'EVIL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['BLINK', 'TPORT', 'S_DRAGON']
	description = """It looks like it was once a dragon corpse, now deeply infected with 
magical bacteria that make it pulse in a foul and degrading way.
"""

monsters.register_race(draconic_quylthulg)


class nyogtha_the_thing_that_should_not_be(monster, spellcaster):
	r_idx = "760"
	name = "Nyogtha, the Thing that Should not Be"
	ascii = "j,D"
	level = 56
	rarity = 2
	hp = "55d70"
	speed = 130
	aaf = 20
	ac = 60
	sleep = 20
	exp = 20000
	blows = [('CRUSH', 'ACID', '10d6'), ('CRUSH', 'ACID', '10d6'), ('CRUSH', 'ACID', '10d6'), ('CRUSH', 'HURT', '16d16')]
	flags = ['UNIQUE', 'ELDRITCH_HORROR', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_SPEAK', 'SMART', 'CAN_SWIM', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'DROP_GOOD', 'REGENERATE', 'KILL_ITEM', 'DROP_2D2', 'DROP_90', 'DROP_60', 'BASH_DOOR', 'EVIL', 'NO_CONF', 'NO_SLEEP', 'KILL_BODY', 'HURT_LITE', 'POWERFUL', 'RES_NETH', 'NONLIVING', 'IM_ACID', 'IM_FIRE', 'RES_PLAS', 'IM_POIS', 'IM_COLD', 'IM_ELEC', 'RES_TELE']
	sfreq = 5
	sflags = ['BRAIN_SMASH', 'MIND_BLAST', 'HASTE', 'TPORT', 'S_DEMON', 'S_UNDEAD', 'S_HI_UNDEAD', 'S_KIN', 'BR_DARK', 'BR_NUKE', 'BR_ACID', 'BR_POIS']
	description = """"...a little finger of blackness crept out from beneath its edge 
a great wave of iridescent blackness, neither liquid nor solid,
a frightful gelatinous mass."
"""

monsters.register_race(nyogtha_the_thing_that_should_not_be)


class ahtu_avatar_of_nyarlathotep(demon, spellcaster):
	r_idx = "761"
	name = "Ahtu, Avatar of Nyarlathotep"
	ascii = "#,D"
	level = 56
	rarity = 3
	hp = "38d110"
	speed = 130
	aaf = 30
	ac = 60
	sleep = 15
	exp = 22500
	blows = [('CRUSH', 'HURT', '13d13'), ('CRUSH', 'FIRE', '10d10'), ('CRUSH', 'HURT', '13d13'), ('CRUSH', 'FIRE', '10d10')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'UNIQUE', 'CAN_SPEAK', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'MOVE_BODY', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DEMON', 'NONLIVING', 'ELDRITCH_HORROR']
	sfreq = 6
	sflags = ['BR_FIRE', 'S_DEMON', 'CAUSE_4', 'BR_PLAS', 'BR_NETH', 'BRAIN_SMASH', 'S_UNDEAD', 'BA_DARK']
	description = """"Higher already than the giants of the forest ringing it, the 
fifty-foot-thick column... sprouted a ring of tendrils, ruddy and 
golden and glittering overall with inclusions of quartz."
"""

monsters.register_race(ahtu_avatar_of_nyarlathotep)


class fundin_bluecloak(monster, spellcaster):
	r_idx = "762"
	name = "Fundin Bluecloak"
	ascii = "h,G"
	level = 56
	rarity = 2
	hp = "38d100"
	speed = 130
	aaf = 25
	ac = 99
	sleep = 10
	exp = 20000
	blows = [('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '8d6'), ('HIT', 'HURT', '8d6'), ('HIT', 'HURT', '8d6')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'FRIENDLY', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['HEAL', 'BLIND', 'CONF', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'BRAIN_SMASH', 'FORGET', 'S_MONSTERS']
	description = """He is one of the greatest dwarven priests to walk the earth.  Fundin has 
earned a high position in the church, and his skill with both weapon and 
spell only justify his position further.  His combination of both dwarven 
strength and priestly wisdom are a true match for any adventurer.
"""

monsters.register_race(fundin_bluecloak)


class dworkin_barimen(amberite, spellcaster):
	r_idx = "763"
	name = "Dworkin Barimen"
	ascii = "p,v"
	level = 56
	rarity = 2
	hp = "70d48"
	speed = 130
	aaf = 25
	ac = 99
	sleep = 10
	exp = 22200
	blows = [('HIT', 'CONFUSE', '4d8'), ('HIT', 'TERRIFY', '5d6'), ('HIT', 'PARALYZE', '5d6'), ('HIT', 'BLIND', '4d8')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'AMBERITE', 'DROP_CORPSE', 'DROP_CHOSEN', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SHAPECHANGER', 'CAN_SPEAK', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'DROP_GOOD', 'CAN_FLY', 'OPEN_DOOR', 'BASH_DOOR', 'ATTR_MULTI', 'RES_DISE', 'ATTR_ANY', 'RES_TELE', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['HEAL', 'BLIND', 'CONF', 'SCARE', 'CAUSE_4', 'BRAIN_SMASH', 'FORGET', 'S_MONSTERS', 'BA_CHAO', 'BLINK', 'TPORT', 'TELE_TO', 'TELE_AWAY', 'S_DEMON']
	description = """Once this now gnomish creature created the universe with the Jewel 
of Judgement - or so you have been told. There is little sanity left 
in his present form even if he still has the power. "He was a small 
man. Tiny, might be an even better word. He was around five feet tall 
and a hunchback. His hair and beard were as heavy as (Corwin's). The 
only distinguishing features in that great mass of fur were 
his long, hook nose and his almost black eyes, now squinted 
against the light... Dworkin'Oh? None of us knew that Dad had locked you up.' (Corwin) heard 
(Dworkin) weeping. 'Yes,' he said after a long time. 'He didn't 
trust me.' 'Why not?' 'I told him I'd thought of a way to 
destroy Amber. I described it to him, and he locked me in.'"
"""

monsters.register_race(dworkin_barimen)


class uriel_angel_of_fire(monster, spellcaster):
	r_idx = "764"
	name = "Uriel, Angel of Fire"
	ascii = "A,R"
	level = 56
	rarity = 3
	hp = "55d70"
	speed = 130
	aaf = 40
	ac = 80
	sleep = 10
	exp = 25000
	blows = [('HIT', 'FIRE', '9d12'), ('HIT', 'FIRE', '4d6'), ('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '10d10')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'NO_FEAR', 'GOOD', 'AURA_FIRE', 'REFLECTING', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'RES_TELE']
	sfreq = 2
	sflags = ['TELE_TO', 'BLIND', 'BO_FIRE', 'BO_MANA', 'BA_FIRE', 'BR_FIRE', 'S_ANGEL']
	description = """A creature of godly appearance, you dare not challenge Uriel's supremacy.  
Those who stood against him before are but a memory, cremated by his 
mastery of elemental fire.
"""

monsters.register_race(uriel_angel_of_fire)


class azriel_angel_of_death(monster, spellcaster):
	r_idx = "765"
	name = "Azriel, Angel of Death"
	ascii = "A,D"
	level = 57
	rarity = 3
	hp = "45d100"
	speed = 130
	aaf = 40
	ac = 85
	sleep = 10
	exp = 30000
	blows = [('TOUCH', 'EXP_VAMP', '1d12'), ('HIT', 'BLIND', '10d5'), ('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '10d10')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'RES_NETH', 'NO_FEAR', 'GOOD', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'REFLECTING', 'AURA_COLD', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'CAN_FLY', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'RES_TELE']
	sfreq = 2
	sflags = ['TELE_TO', 'BLIND', 'BO_MANA', 'BO_NETH', 'BA_NETH', 'BR_NETH', 'S_ANGEL']
	description = """Azriel commands awesome power, his visage holy enough to shrivel your 
soul.  You shriek with disbelief as his mastery of death draws you to your 
grave.  It is truly beyond all but the mightiest of warriors to stand 
against him and live.
"""

monsters.register_race(azriel_angel_of_death)


class ancalagon_the_black(dragon, spellcaster):
	r_idx = "766"
	name = "Ancalagon the Black"
	ascii = "D,D"
	level = 58
	rarity = 3
	hp = "75d70"
	speed = 120
	aaf = 20
	ac = 62
	sleep = 70
	exp = 30000
	blows = [('CLAW', 'HURT', '5d12'), ('CLAW', 'HURT', '6d12'), ('CLAW', 'HURT', '8d12'), ('BITE', 'HURT', '10d14')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_ACID', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_ACID', 'S_DRAGON', 'S_HI_DRAGON']
	description = """'Rushing Jaws' is his name, and death is his game.  No dragon of the brood 
of Glaurung can match him.
"""

monsters.register_race(ancalagon_the_black)


class daoloth_the_render_of_the_veils(monster, spellcaster):
	r_idx = "767"
	name = "Daoloth, the Render of the Veils"
	ascii = "U,s"
	level = 58
	rarity = 3
	hp = "72d70"
	speed = 120
	aaf = 20
	ac = 62
	sleep = 70
	exp = 27500
	blows = [('TOUCH', 'CONFUSE', '5d12'), ('TOUCH', 'CONFUSE', '5d12'), ('TOUCH', 'CONFUSE', '5d12'), ('TOUCH', 'CONFUSE', '5d12')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'IM_ACID', 'IM_FIRE', 'IM_POIS', 'IM_COLD', 'ELDRITCH_HORROR', 'NEVER_MOVE', 'RES_NEXU', 'REFLECTING', 'PASS_WALL', 'NO_CONF', 'NO_SLEEP', 'EVIL']
	sfreq = 3
	sflags = ['TELE_AWAY', 'S_MONSTERS', 'TELE_LEVEL', 'BR_NEXU', 'TPORT', 'BLINK']
	description = """"Not shapeless, but so complex that the eye could recognize no 
describable shape. There were hemispheres and shining metal, 
coupled by long plastic rods. The rods were of a flat gray color, 
so that he could not make out which were nearer; they merged into 
a flat mass from which protruded individual cylinders. As he looked 
at it, he had a curious feeling that eyes gleamed from between 
these rods; but wherever he glanced at the construction, he saw 
only the spaces between them."
"""

monsters.register_race(daoloth_the_render_of_the_veils)


class nightwalker(monster, undead, spellcaster):
	r_idx = "768"
	name = "Nightwalker"
	ascii = "W,D"
	level = 59
	rarity = 4
	hp = "35d65"
	speed = 130
	aaf = 20
	ac = 88
	sleep = 10
	exp = 15000
	blows = [('HIT', 'UN_BONUS', '10d10'), ('HIT', 'UN_BONUS', '10d10'), ('HIT', 'UN_BONUS', '7d7'), ('HIT', 'UN_BONUS', '7d7')]
	flags = ['FORCE_SLEEP', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'CAN_SWIM', 'WILD_DUNGEON_01', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'RES_TELE', 'EVIL', 'UNDEAD', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BLIND', 'SCARE', 'BRAIN_SMASH', 'BO_MANA', 'BO_NETH', 'BA_NETH', 'S_UNDEAD']
	description = """A huge giant garbed in black, more massive than a titan and stronger than 
a dragon.  With terrible blows, it breaks your armour from your back, 
leaving you defenseless against its evil wrath.  It can smell your fear, 
and you in turn smell the awful stench of death as this ghastly figure 
strides towards you menacingly.
"""

monsters.register_race(nightwalker)


class raphael_the_messenger(monster, spellcaster):
	r_idx = "769"
	name = "Raphael, the Messenger"
	ascii = "A,v"
	level = 59
	rarity = 3
	hp = "75d70"
	speed = 130
	aaf = 40
	ac = 90
	sleep = 10
	exp = 35000
	blows = [('HIT', 'UN_BONUS', '6d8'), ('HIT', 'FIRE', '4d6'), ('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '10d10')]
	flags = ['UNIQUE', 'MALE', 'FORCE_MAXHP', 'CAN_SPEAK', 'NO_FEAR', 'GOOD', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'SMART', 'AURA_ELEC', 'REFLECTING', 'RES_TELE', 'ESCORT', 'CAN_FLY', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS']
	sfreq = 2
	sflags = ['TELE_TO', 'BLIND', 'BO_MANA', 'S_ANGEL', 'S_KIN', 'INVULNER']
	description = """Commanding a legion of angels, Raphael will destroy you for your sins.  He 
will crush you like the pitiful insignificant being he sees you to be.  
Your very soul will be taken into judgement by his supreme authority as he 
cleanses the world of evil.
"""

monsters.register_race(raphael_the_messenger)


class artsi_the_champion_of_chaos(monster, spellcaster):
	r_idx = "770"
	name = "Artsi the Champion of Chaos"
	ascii = "h,G"
	level = 59
	rarity = 2
	hp = "83d66"
	speed = 130
	aaf = 25
	ac = 88
	sleep = 10
	exp = 20000
	blows = [('HIT', 'EXP_VAMP', '6d6'), ('HIT', 'EXP_VAMP', '8d8'), ('HIT', 'EXP_VAMP', '10d10')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'RES_NEXU', 'RES_NETH', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BA_FIRE', 'BA_CHAO', 'CONF', 'TPORT', 'S_DEMON', 'BR_CHAO', 'BA_MANA']
	description = """He is one of the greatest warriors of chaos to walk the earth. 
His bloody blade has slain thousands and tens of thousands, and still 
hungers for more. 
"""

monsters.register_race(artsi_the_champion_of_chaos)


class saruman_of_many_colours(monster, spellcaster):
	r_idx = "771"
	name = "Saruman of Many Colours"
	ascii = "p,v"
	level = 60
	rarity = 1
	hp = "38d100"
	speed = 120
	aaf = 100
	ac = 50
	sleep = 0
	exp = 35000
	blows = [('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_BONUS', '6d8'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['UNIQUE', 'MALE', 'ATTR_MULTI', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'REFLECTING', 'RES_TELE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_CHOSEN', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HEAL', 'HASTE', 'TPORT', 'TELE_AWAY', 'BLIND', 'CONF', 'SCARE', 'CAUSE_4', 'MIND_BLAST', 'FORGET', 'TRAPS', 'ANIM_DEAD', 'BO_ICEE', 'BA_ACID', 'BA_FIRE', 'BA_COLD', 'BA_WATE', 'S_UNDEAD', 'S_DEMON', 'S_DRAGON']
	description = """Originally known as the White, Saruman fell prey to Sauron's wiles. He 
searches forever for the One Ring, to become a mighty Sorcerer-King of the 
world.
"""

monsters.register_race(saruman_of_many_colours)


class gandalf_the_grey(monster, spellcaster):
	r_idx = "772"
	name = "Gandalf the Grey"
	ascii = "p,v"
	level = 60
	rarity = 7
	hp = "49d70"
	speed = 120
	aaf = 101
	ac = 50
	sleep = 0
	exp = 35000
	blows = [('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_BONUS', '6d8'), ('HIT', 'TERRIFY', '5d5'), ('HIT', 'TERRIFY', '5d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'FRIENDLY', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'REFLECTING', 'RES_TELE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'GOOD', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HEAL', 'HASTE', 'TPORT', 'TELE_AWAY', 'BLIND', 'CONF', 'SCARE', 'CAUSE_4', 'BRAIN_SMASH', 'FORGET', 'TRAPS', 'BA_FIRE', 'BO_FIRE', 'BO_PLAS', 'BO_MANA', 'S_MONSTER', 'S_ANGEL', 'S_DRAGON', 'S_KIN']
	description = """The wizard who opposed Saruman, and in the end, was the only 
one of the Istari to succeed in his task. Gandalf is very 
wise and specializes in fire magic.
"""

monsters.register_race(gandalf_the_grey)


class brand_mad_visionary_of_amber(amberite, spellcaster):
	r_idx = "773"
	name = "Brand, Mad Visionary of Amber"
	ascii = "p,v"
	level = 60
	rarity = 1
	hp = "50d70"
	speed = 120
	aaf = 100
	ac = 50
	sleep = 0
	exp = 35000
	blows = [('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_BONUS', '6d8'), ('HIT', 'CONFUSE', '5d5'), ('HIT', 'UN_POWER', '5d5')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'ATTR_MULTI', 'CAN_SPEAK', 'AMBERITE', 'ATTR_ANY', 'RES_TELE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_SKELETON', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_CHOSEN', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'CONF', 'TPORT', 'BLINK', 'BA_FIRE', 'BA_COLD', 'DRAIN_MANA', 'CAUSE_4', 'BA_ACID', 'TELE_AWAY', 'MIND_BLAST', 'TRAPS', 'HEAL', 'HASTE', 'BRAIN_SMASH', 'BA_CHAO', 'BA_DARK']
	description = """Brand sees himself as a hero, the god creator 
and absolute monarch of a future world. Unfortunately he needs 
to erase the existing world to make his new world. "...a figure both 
like Bleys and (Corwin). (Corwin's) features, though smaller, his eyes, 
Bleys' hair, beardless. He wore a riding suit of green and sat atop a white 
horse... There was a quality of both strength and weakness, questing and 
abandonment about him."
"""

monsters.register_race(brand_mad_visionary_of_amber)


class shadowlord(monster, undead, spellcaster):
	r_idx = "774"
	name = "Shadowlord"
	ascii = "G,b"
	level = 62
	rarity = 2
	hp = "30d70"
	speed = 120
	aaf = 20
	ac = 75
	sleep = 10
	exp = 22500
	blows = [('HIT', 'EXP_40', '6d6'), ('HIT', 'EXP_40', '6d6'), ('HIT', 'LOSE_STR', '4d6'), ('GAZE', 'TERRIFY', '4d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'INVISIBLE', 'COLD_BLOOD', 'TAKE_ITEM', 'PASS_WALL', 'EVIL', 'UNDEAD', 'CAN_FLY', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 3
	sflags = ['HOLD', 'DRAIN_MANA', 'BLIND', 'S_UNDEAD', 'CONF', 'SCARE', 'TELE_TO', 'TPORT', 'BRAIN_SMASH', 'ANIM_DEAD', 'BA_NETH', 'DARKNESS', 'SHRIEK']
	description = """An aura of hatred, cowardice and falsehood surrounds you as this 
cloaked figure floats towards you.
"""

monsters.register_race(shadowlord)


class greater_kraken(monster, spellcaster):
	r_idx = "775"
	name = "Greater kraken"
	ascii = "l,G"
	level = 63
	rarity = 2
	hp = "40d70"
	speed = 120
	aaf = 30
	ac = 62
	sleep = 80
	exp = 25000
	blows = [('CRUSH', 'HURT', '15d15'), ('CRUSH', 'HURT', '15d15'), ('CRUSH', 'HURT', '15d15'), ('CRUSH', 'HURT', '15d15')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN', 'ONLY_ITEM', 'DROP_3D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'SMART', 'RES_WATE', 'EVIL', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP', 'IM_POIS', 'IM_FIRE']
	sfreq = 6
	sflags = ['BLIND', 'CONF', 'SCARE', 'CAUSE_4', 'CAUSE_3', 'TELE_TO', 'TELE_AWAY', 'BA_WATE', 'DARKNESS', 'BR_DARK', 'BR_ACID', 'BR_POIS']
	description = """An enormously fearsome and powerful inhabitant of the depths. It 
resembles a gargantuan octopus and its evil is almost tangible.
"""

monsters.register_race(greater_kraken)


class archlich(monster, undead, spellcaster):
	r_idx = "776"
	name = "Archlich"
	ascii = "L,B"
	level = 64
	rarity = 2
	hp = "35d70"
	speed = 120
	aaf = 20
	ac = 60
	sleep = 50
	exp = 20000
	blows = [('TOUCH', 'EXP_VAMP', '1d16'), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'LOSE_DEX', '8d12'), ('TOUCH', 'LOSE_DEX', '8d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'RES_TELE', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLINK', 'TELE_TO', 'BLIND', 'HOLD', 'CONF', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'S_HI_UNDEAD', 'S_UNDEAD', 'FORGET', 'TPORT', 'HEAL', 'S_KIN', 'S_DEMON', 'BA_NETH', 'ANIM_DEAD']
	description = """A lich who has reached its ultimate evolutionary stageimmaterial state.
"""

monsters.register_race(archlich)


class bast_goddess_of_cats(monster, spellcaster):
	r_idx = "777"
	name = "Bast, Goddess of Cats"
	ascii = "f,o"
	level = 64
	rarity = 3
	hp = "48d70"
	speed = 130
	aaf = 100
	ac = 100
	sleep = 0
	exp = 30500
	blows = [('HIT', 'CONFUSE', '12d12'), ('TOUCH', 'LOSE_DEX', '2d12'), ('HIT', 'BLIND', '10d5'), ('HIT', 'PARALYZE', '15d1')]
	flags = ['UNIQUE', 'FEMALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ESCORTS', 'EVIL', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['TELE_TO', 'HEAL', 'S_KIN']
	description = """"Beauty -- coolness -- aloofness -- philosophical repose -- 
self-sufficiency -- untamed mastery -- what else can we find 
these things with even half the perfection and completeness 
that mark their incarnation in the peerless and softly gliding 
cat?"
"""

monsters.register_race(bast_goddess_of_cats)


class jabberwock(monster, spellcaster):
	r_idx = "778"
	name = "Jabberwock"
	ascii = "H,R"
	level = 65
	rarity = 4
	hp = "32d70"
	speed = 130
	aaf = 35
	ac = 62
	sleep = 255
	exp = 19000
	blows = [('CLAW', 'HURT', '10d10'), ('CLAW', 'HURT', '10d10'), ('BITE', 'HURT', '10d10'), ('BITE', 'HURT', '10d10')]
	flags = ['ATTR_MULTI', 'EVIL', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'ONLY_ITEM', 'DROP_60', 'DROP_90', 'BASH_DOOR', 'CAN_FLY', 'DROP_CORPSE', 'ANIMAL']
	sfreq = 5
	sflags = ['CAUSE_4', 'BR_CHAO']
	description = """"Beware the Jabberwock, my son! The jaws that bite, the claws that catch!"
"""

monsters.register_race(jabberwock)


class chaos_hound(monster, spellcaster):
	r_idx = "779"
	name = "Chaos hound"
	ascii = "Z,v"
	level = 65
	rarity = 1
	hp = "45d30"
	speed = 120
	aaf = 30
	ac = 50
	sleep = 0
	exp = 10000
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['ATTR_MULTI', 'ATTR_ANY', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'WILD_DUNGEON_01', 'FRIENDS', 'BASH_DOOR', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BR_CHAO']
	description = """A constantly changing canine form, this hound rushes towards you as if 
expecting mayhem and chaos ahead.  It appears to have an almost kamikaze 
relish for combat.  You suspect all may not be as it seems.
"""

monsters.register_race(chaos_hound)


class vlad_dracula_prince_of_darkness(monster, undead, spellcaster):
	r_idx = "780"
	name = "Vlad Dracula, Prince of Darkness"
	ascii = "V,b"
	level = 66
	rarity = 4
	hp = "50d70"
	speed = 130
	aaf = 20
	ac = 83
	sleep = 10
	exp = 33333
	blows = [('BITE', 'EXP_VAMP', '8d8'), ('BITE', 'EXP_VAMP', '8d8'), ('HIT', 'CONFUSE', '8d8'), ('HIT', 'CONFUSE', '8d8')]
	flags = ['UNIQUE', 'MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'CAN_SPEAK', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 3
	sflags = ['BLIND', 'HOLD', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'BA_NETH', 'S_KIN', 'DARKNESS', 'BA_DARK']
	description = """The most feared of all vampires, the Prince of Darkness himself.
"""

monsters.register_race(vlad_dracula_prince_of_darkness)


class ultimate_beholder(monster, spellcaster):
	r_idx = "781"
	name = "Ultimate beholder"
	ascii = "e,o"
	level = 66
	rarity = 4
	hp = "40d70"
	speed = 120
	aaf = 30
	ac = 40
	sleep = 10
	exp = 10000
	blows = [('GAZE', 'EXP_40', '2d5'), ('GAZE', 'PARALYZE', '2d5'), ('GAZE', 'LOSE_INT', '2d6'), ('GAZE', 'UN_POWER', '2d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'CAN_FLY', 'BASH_DOOR', 'FEMALE', 'SMART', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'EVIL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'CONF', 'SCARE', 'DRAIN_MANA', 'MIND_BLAST', 'FORGET', 'DARKNESS', 'BO_ACID', 'BO_FIRE', 'BO_COLD', 'S_KIN', 'BO_MANA']
	description = """A disembodied eye, surrounded by twelve smaller eyes on stalks. 
This is a beholder hive-mother, far more powerful than normal 
beholders.
"""

monsters.register_race(ultimate_beholder)


class leviathan(dragon, spellcaster):
	r_idx = "782"
	name = "Leviathan"
	ascii = "D,b"
	level = 67
	rarity = 3
	hp = "50d70"
	speed = 120
	aaf = 40
	ac = 90
	sleep = 30
	exp = 28000
	blows = [('BITE', 'FIRE', '5d12'), ('BITE', 'FIRE', '5d12'), ('CRUSH', 'HURT', '6d15'), ('CRUSH', 'HURT', '6d15')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'AQUATIC', 'WILD_DUNGEON_01', 'WILD_OCEAN', 'IM_FIRE', 'IM_ACID', 'IM_COLD', 'IM_POIS', 'EVIL', 'DRAGON', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'DROP_CORPSE']
	sfreq = 9
	sflags = ['BR_FIRE', 'BR_ACID', 'BR_DARK', 'S_DRAGON', 'S_HYDRA', 'HEAL', 'CONF', 'DARKNESS']
	description = """An enormous, evil sea-dragon.
"""

monsters.register_race(leviathan)


class great_wyrm_of_chaos(dragon, spellcaster):
	r_idx = "783"
	name = "Great Wyrm of Chaos"
	ascii = "D,v"
	level = 67
	rarity = 2
	hp = "45d70"
	speed = 120
	aaf = 40
	ac = 85
	sleep = 20
	exp = 29000
	blows = [('CLAW', 'HURT', '5d12'), ('CLAW', 'HURT', '5d12'), ('CLAW', 'HURT', '6d12'), ('BITE', 'HURT', '8d14')]
	flags = ['ATTR_MULTI', 'ATTR_ANY', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'WILD_SWAMP2', 'WILD_MOUNT2', 'WILD_FOREST2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RES_DISE', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'CAN_FLY', 'EVIL', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_CHAO', 'BR_DISE', 'S_DRAGON']
	description = """A massive dragon of changing form.  As you watch, it appears first fair 
and then foul.  Its body is twisted by chaotic forces as it strives to 
stay real.  Its very existence distorts the universe around it.
"""

monsters.register_race(great_wyrm_of_chaos)


class great_wyrm_of_law(dragon, spellcaster):
	r_idx = "784"
	name = "Great Wyrm of Law"
	ascii = "D,W"
	level = 67
	rarity = 2
	hp = "45d70"
	speed = 120
	aaf = 40
	ac = 85
	sleep = 255
	exp = 29000
	blows = [('CLAW', 'HURT', '5d12'), ('CLAW', 'HURT', '5d12'), ('CLAW', 'HURT', '6d12'), ('BITE', 'HURT', '8d14')]
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'GOOD', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'CAN_FLY', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_SOUN', 'BR_SHAR', 'S_DRAGON']
	description = """A massive dragon of powerful intellect.  It seeks to dominate the universe 
and despises all other life.  It sees all who do not obey it as mere 
insects to be crushed underfoot.
"""

monsters.register_race(great_wyrm_of_law)


class great_wyrm_of_balance(dragon, spellcaster):
	r_idx = "785"
	name = "Great Wyrm of Balance"
	ascii = "D,s"
	level = 67
	rarity = 4
	hp = "49d70"
	speed = 120
	aaf = 40
	ac = 85
	sleep = 255
	exp = 31000
	blows = [('CLAW', 'HURT', '5d12'), ('CLAW', 'HURT', '5d12'), ('CLAW', 'HURT', '6d12'), ('BITE', 'HURT', '8d14')]
	flags = ['WILD_DUNGEON_01', 'WILD_FOREST2', 'ATTR_MULTI', 'ATTR_ANY', 'GOOD', 'EVIL', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'RES_DISE', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_SOUN', 'BR_CHAO', 'BR_SHAR', 'BR_DISE', 'S_DRAGON', 'S_HI_DRAGON']
	description = """A massive dragon, it is thousands of 
years old and seeks to maintain the Cosmic Balance.  It sees you as an 
upstart troublemaker without the wisdom to control your actions. 
"""

monsters.register_race(great_wyrm_of_balance)


class shambler(monster, spellcaster):
	r_idx = "786"
	name = "Shambler"
	ascii = "E,W"
	level = 67
	rarity = 4
	hp = "50d70"
	speed = 130
	aaf = 40
	ac = 75
	sleep = 50
	exp = 22500
	blows = [('CLAW', 'HURT', '3d12'), ('CLAW', 'HURT', '3d12'), ('CRUSH', 'HURT', '8d12'), ('CRUSH', 'HURT', '8d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'CAN_SWIM', 'DROP_CORPSE', 'BASH_DOOR', 'OPEN_DOOR', 'POWERFUL', 'MOVE_BODY', 'NO_CONF', 'NO_SLEEP', 'EVIL']
	sfreq = 3
	sflags = ['BR_ELEC']
	description = """This elemental creature is power incarnate; it looks like a huge polar bear 
with a huge gaping maw instead of a head.
"""

monsters.register_race(shambler)


class hypnos_lord_of_sleep(monster, spellcaster):
	r_idx = "787"
	name = "Hypnos, Lord of Sleep"
	ascii = "P,G"
	level = 67
	rarity = 2
	hp = "51d70"
	speed = 130
	aaf = 20
	ac = 75
	sleep = 10
	exp = 36000
	blows = [('TOUCH', 'LOSE_ALL', '2d25'), ('GAZE', 'PARALYZE', '1d20'), ('GAZE', 'TERRIFY', '1d20'), ('GAZE', 'BLIND', '1d20')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'ATTR_MULTI', 'WILD_DUNGEON_01', 'RES_DISE', 'RES_NEXU', 'CAN_FLY', 'FORCE_SLEEP', 'FORCE_MAXHP', 'EVIL', 'SMART', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'NO_SLEEP', 'NO_CONF', 'NO_STUN', 'IM_POIS', 'IM_COLD', 'IM_FIRE', 'IM_ACID']
	sfreq = 3
	sflags = ['BR_NEXU', 'BR_CHAO', 'HOLD', 'SLOW', 'BR_INER', 'BA_NUKE', 'MIND_BLAST', 'BRAIN_SMASH', 'CONF', 'BLIND', 'TELE_TO', 'HEAL', 'TELE_AWAY', 'TELE_LEVEL', 'TPORT', 'S_UNDEAD', 'S_DEMON', 'DRAIN_MANA']
	description = """"Young with the youth that is outside time, and with a beauteous
bearded face, curved, smiling lips, Olympian brow; and dense locks 
waving and poppy-crowned."
"""

monsters.register_race(hypnos_lord_of_sleep)


class glaaki(demon, spellcaster):
	r_idx = "788"
	name = "Glaaki"
	ascii = "l,g"
	level = 67
	rarity = 2
	hp = "52d70"
	speed = 130
	aaf = 20
	ac = 75
	sleep = 10
	exp = 36000
	blows = [('STING', 'HURT', '20d1'), ('STING', 'DISEASE', '20d1'), ('CRUSH', 'HURT', '3d20')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'ATTR_MULTI', 'ELDRITCH_HORROR', 'WILD_DUNGEON_01', 'RES_NEXU', 'RES_WATE', 'CAN_SWIM', 'REGENERATE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'EVIL', 'SMART', 'DEMON', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'NO_SLEEP', 'NO_CONF', 'NO_STUN', 'IM_POIS', 'IM_COLD', 'IM_ACID']
	sfreq = 5
	sflags = ['BA_WATE', 'S_HYDRA', 'S_DRAGON', 'S_DEMON', 'SCARE', 'BLIND', 'CONF', 'CAUSE_4', 'BR_POIS']
	description = """"From an oval body protruded countless thin, pointed spines of 
multi-colored metal; at the more rounded end of the oval a 
circular, thick-lipped mouth formed the center of a spongy 
face, from which rose three yellow eyes on thin stalks. Around 
the underside of the body were many white pyramids, presumably 
used for locomotion. The diameter of the body must have been 
at least ten feet at its least wide..."
"""

monsters.register_race(glaaki)


class bleys_master_of_manipulation(amberite, spellcaster):
	r_idx = "789"
	name = "Bleys, Master of Manipulation"
	ascii = "p,U"
	level = 67
	rarity = 1
	hp = "50d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 50
	exp = 35000
	blows = [('HIT', 'HURT', '8d15'), ('HIT', 'HURT', '8d15'), ('TOUCH', 'EAT_ITEM', '')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'AMBERITE', 'RES_TELE', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'DROP_SKELETON', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'ONLY_ITEM', 'EVIL', 'IM_FIRE', 'IM_ELEC', 'SMART', 'REGENERATE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 3
	sflags = ['SCARE', 'BLIND', 'CONF', 'TPORT', 'BLINK', 'TELE_AWAY', 'TELE_TO', 'TELE_LEVEL', 'BA_COLD', 'BA_FIRE', 'TRAPS', 'MIND_BLAST', 'S_MONSTERS']
	description = """This cunning Amberite is the master of subtle ploys. "...a fiery bearded, 
flame-crowned man, dressed all in red and orange, mainly of silk stuff, 
and he held a sword in his right hand and a glass of wine in his left, 
and the devil himself danced behind his eyes... His chin was light, but the 
beard covered it. His sword was inlaid with an elaborate filigree of a 
golden color. He wore two huge rings on his right hand and one on his 
left"""

monsters.register_race(bleys_master_of_manipulation)


class great_wyrm_of_many_colours(dragon, spellcaster):
	r_idx = "790"
	name = "Great Wyrm of Many Colours"
	ascii = "D,v"
	level = 68
	rarity = 4
	hp = "52d70"
	speed = 120
	aaf = 40
	ac = 85
	sleep = 255
	exp = 31000
	blows = [('CLAW', 'HURT', '6d12'), ('CLAW', 'HURT', '6d12'), ('CLAW', 'HURT', '6d12'), ('BITE', 'HURT', '9d14')]
	flags = ['ATTR_MULTI', 'CAN_FLY', 'EVIL', 'WILD_DUNGEON_01', 'WILD_MOUNT2', 'FORCE_SLEEP', 'FORCE_MAXHP', 'AURA_FIRE', 'AURA_ELEC', 'RES_TELE', 'IM_FIRE', 'IM_ACID', 'IM_POIS', 'IM_COLD', 'IM_ELEC', 'AURA_COLD', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'DROP_CORPSE', 'DRAGON', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BR_POIS', 'BR_ELEC', 'BR_ACID', 'BR_FIRE', 'BR_COLD', 'CONF', 'SCARE', 'BLIND']
	description = """A huge dragon whose scales shimmer in myriad hues.
"""

monsters.register_race(great_wyrm_of_many_colours)


class fiona_the_sorceress(amberite, spellcaster):
	r_idx = "791"
	name = "Fiona the Sorceress"
	ascii = "p,y"
	level = 68
	rarity = 1
	hp = "49d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 50
	exp = 35000
	blows = [('HIT', 'POISON', '8d15'), ('HIT', 'POISON', '8d15'), ('TOUCH', 'LOSE_CHR', ''), ('TOUCH', 'LOSE_CON', '')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'FEMALE', 'CAN_SPEAK', 'AMBERITE', 'ONLY_ITEM', 'RES_TELE', 'DROP_CHOSEN', 'DROP_CORPSE', 'DROP_SKELETON', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'EVIL', 'IM_POIS', 'IM_ELEC', 'SMART', 'REGENERATE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 3
	sflags = ['SCARE', 'BLIND', 'CONF', 'TPORT', 'BLINK', 'TELE_AWAY', 'TELE_TO', 'TRAPS', 'CAUSE_4', 'BA_POIS', 'S_DEMON', 'S_MONSTERS']
	description = """She is beautiful and deadly. "...I have always been very fond of Fiona. 
She is certainly the loveliest, most civilized of (all Amberites)."
"""

monsters.register_race(fiona_the_sorceress)


class tselakus_the_dreadlord(monster, undead, spellcaster):
	r_idx = "792"
	name = "Tselakus, the Dreadlord"
	ascii = "G,R"
	level = 68
	rarity = 2
	hp = "65d70"
	speed = 130
	aaf = 20
	ac = 75
	sleep = 10
	exp = 35000
	blows = [('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '10d10'), ('HIT', 'LOSE_STR', '4d6'), ('HIT', 'LOSE_STR', '4d6')]
	flags = ['UNIQUE', 'MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_SPEAK', 'CAN_FLY', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'INVISIBLE', 'COLD_BLOOD', 'PASS_WALL', 'EVIL', 'UNDEAD', 'IM_COLD', 'AURA_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'HOLD', 'CONF', 'BA_DARK', 'BA_NETH', 'S_AMBERITES', 'S_HI_UNDEAD', 'S_KIN']
	description = """This huge affront to existence twists and tears at the fabric of space.
Darkness itself recoils from the touch of Tselakus as he leaves a trail
of death and destruction. Mighty claws rend reality as he
annihilates all in his path to your soul!
"""

monsters.register_race(tselakus_the_dreadlord)


class sky_drake(dragon, spellcaster):
	r_idx = "793"
	name = "Sky Drake"
	ascii = "D,B"
	level = 69
	rarity = 4
	hp = "60d70"
	speed = 130
	aaf = 40
	ac = 100
	sleep = 255
	exp = 31000
	blows = [('CLAW', 'HURT', '8d12'), ('CLAW', 'HURT', '8d12'), ('CLAW', 'HURT', '8d12'), ('BITE', 'ELEC', '9d15')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'AURA_ELEC', 'AURA_COLD', 'DROP_CORPSE', 'IM_ELEC', 'GOOD', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'DRAGON', 'NO_CONF', 'NO_SLEEP', 'RES_TELE']
	sfreq = 3
	sflags = ['BR_ELEC', 'BR_GRAV', 'BR_LITE', 'S_DRAGON', 'S_HI_DRAGON', 'SCARE', 'BLIND']
	description = """The mightiest elemental dragon of air, it can destroy you with ease.
"""

monsters.register_race(sky_drake)


class julian_master_of_arden_forest(amberite, spellcaster):
	r_idx = "794"
	name = "Julian, Master of Arden Forest"
	ascii = "p,g"
	level = 69
	rarity = 1
	hp = "55d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 20
	exp = 36000
	blows = [('HIT', 'HURT', '9d15'), ('HIT', 'HURT', '9d15'), ('HIT', 'LOSE_CON', '1d30')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'MALE', 'CAN_SPEAK', 'AMBERITE', 'RES_TELE', 'DROP_SKELETON', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'DROP_CORPSE', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'ONLY_ITEM', 'DROP_CHOSEN', 'EVIL', 'IM_POIS', 'IM_ELEC', 'IM_COLD', 'SMART', 'REGENERATE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 4
	sflags = ['TPORT', 'TELE_TO', 'ARROW_2', 'SHRIEK', 'SCARE', 'DARKNESS', 'S_ANT', 'S_SPIDER', 'S_HOUND', 'S_HYDRA', 'TRAPS', 'BO_WATE', 'BO_ELEC']
	description = """Julian is at home in wild woodlands. He enjoys hunting a challenging 
preypassion nor compassion. He was dressed completely in scaled white armor, 
not silver or metallic-colored, but looking as if it had been enameled."
"""

monsters.register_race(julian_master_of_arden_forest)


class tiamat_celestial_dragon_of_evil(dragon, spellcaster):
	r_idx = "795"
	name = "Tiamat, Celestial Dragon of Evil"
	ascii = "D,v"
	level = 70
	rarity = 4
	hp = "70d100"
	speed = 130
	aaf = 20
	ac = 62
	sleep = 70
	exp = 45000
	blows = [('CLAW', 'HURT', '6d12'), ('CLAW', 'HURT', '8d12'), ('CLAW', 'HURT', '8d12'), ('BITE', 'HURT', '10d14')]
	flags = ['ATTR_MULTI', 'WILD_DUNGEON_01', 'UNIQUE', 'FEMALE', 'CAN_FLY', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DRAGON', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_POIS', 'S_HI_DRAGON']
	description = """Usually found guarding the first plane of Hell, Tiamat is a formidable 
opponent, her five heads breathing death to all who stand against her.
"""

monsters.register_race(tiamat_celestial_dragon_of_evil)


class the_norsa(monster, spellcaster):
	r_idx = "796"
	name = "The Norsa"
	ascii = "H,B"
	level = 70
	rarity = 4
	hp = "70d100"
	speed = 130
	aaf = 20
	ac = 62
	sleep = 70
	exp = 47500
	blows = [('CRUSH', 'ACID', '8d12'), ('CRUSH', 'FIRE', '8d12'), ('CRUSH', 'ELEC', '8d12'), ('CRUSH', 'POISON', '10d14')]
	flags = ['ATTR_MULTI', 'ELDRITCH_HORROR', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'UNIQUE', 'CAN_SPEAK', 'AURA_FIRE', 'AURA_ELEC', 'AURA_COLD', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_POIS', 'S_HI_DRAGON', 'S_MONSTERS']
	description = """An elephantine horror with five trunks, each capable of breathing 
destructive blasts of elements. It is said that it is better to 
face the fury of a thousand raging lions than the Norsa!
"""

monsters.register_race(the_norsa)


class rhan_tegoth(monster, spellcaster):
	r_idx = "797"
	name = "Rhan-Tegoth"
	ascii = "S,b"
	level = 70
	rarity = 4
	hp = "90d70"
	speed = 130
	aaf = 20
	ac = 62
	sleep = 70
	exp = 42500
	blows = [('CLAW', 'HURT', '8d12'), ('CRUSH', 'LOSE_STR', '5d12'), ('CLAW', 'HURT', '8d12'), ('CRUSH', 'ACID', '5d12')]
	flags = ['ELDRITCH_HORROR', 'CAN_SWIM', 'EVIL', 'WILD_DUNGEON_01', 'UNIQUE', 'CAN_SPEAK', 'AURA_FIRE', 'AURA_ELEC', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'IM_ACID', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'RES_WATE']
	sfreq = 5
	sflags = ['S_DEMON', 'BR_ACID', 'S_UNDEAD', 'S_KIN', 'CONF', 'SCARE', 'MIND_BLAST', 'BR_CONF']
	description = """"An almost globular torso, with six long, sinuous limbs terminating 
in clab-like claws. From the upper end a subsidiary globe bulged 
forward bubble-like; its triangle of three staring, fishy eyes, 
its foot-long and evidently flexible proboscis, and a distended 
lateral system analogous to gills, suggested that it was a head."
"""

monsters.register_race(rhan_tegoth)


class black_reaver(monster, undead, spellcaster):
	r_idx = "798"
	name = "Black reaver"
	ascii = "L,D"
	level = 71
	rarity = 3
	hp = "35d70"
	speed = 120
	aaf = 20
	ac = 85
	sleep = 50
	exp = 23000
	blows = [('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_BONUS', '6d8'), ('HIT', 'LOSE_STR', '4d6'), ('HIT', 'LOSE_STR', '4d6')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'CAN_SWIM', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'KILL_WALL', 'NO_FEAR']
	sfreq = 3
	sflags = ['TELE_TO', 'BLIND', 'HOLD', 'CONF', 'CAUSE_3', 'CAUSE_4', 'DRAIN_MANA', 'BRAIN_SMASH', 'BA_MANA', 'BA_NETH', 'S_UNDEAD']
	description = """A humanoid form, black as night, advancing steadily and unstoppably.  
Flee!
"""

monsters.register_race(black_reaver)


class caine_the_conspirator(amberite, spellcaster):
	r_idx = "799"
	name = "Caine, the Conspirator"
	ascii = "p,D"
	level = 71
	rarity = 1
	hp = "42d100"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 20
	exp = 36500
	blows = [('HIT', 'HURT', '9d15'), ('HIT', 'HURT', '9d15'), ('INSULT', 'EAT_ITEM', '1d3'), ('INSULT', 'EAT_GOLD', '1d3')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'AMBERITE', 'RES_TELE', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'DROP_SKELETON', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'ONLY_ITEM', 'EVIL', 'IM_POIS', 'IM_ACID', 'IM_COLD', 'SMART', 'REGENERATE']
	sfreq = 4
	sflags = ['TPORT', 'TELE_TO', 'SCARE', 'DARKNESS', 'S_DRAGON', 'TRAPS', 'BO_WATE', 'BO_NETH', 'S_UNDEAD', 'S_DEMON', 'CONF', 'BO_ACID', 'FORGET', 'MIND_BLAST', 'CAUSE_4']
	description = """Caine is perhaps the least reliable Amberite, always ready to ignore 
his pacts and promises when it suits him. "...the swarthy, dark-eyed 
countenance of Caine, dressed all in satin that was black and green, 
wearing a dark three-cornered hat set at a rakish angle, and a green 
plume of feathers trailing down in the back. He was standing in the 
profile, one arm akimbo, and toes of his boots curled upwards, and he 
wore an emerald-studded dagger at his belt... He was dark."
"""

monsters.register_race(caine_the_conspirator)


class master_quylthulg(monster, spellcaster):
	r_idx = "800"
	name = "Master quylthulg"
	ascii = "Q,b"
	level = 71
	rarity = 3
	hp = "20d70"
	speed = 120
	aaf = 20
	ac = 1
	sleep = 0
	exp = 12000
	blows = []
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'WILD_DUNGEON_01', 'INVISIBLE', 'EMPTY_MIND', 'ANIMAL', 'EVIL', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['S_MONSTER', 'S_MONSTERS', 'S_UNDEAD', 'S_DRAGON', 'S_HI_UNDEAD', 'S_HI_DRAGON']
	description = """A pulsating mound of flesh, shining with silver pulses of throbbing light.
"""

monsters.register_race(master_quylthulg)


class greater_draconic_quylthulg(monster, spellcaster):
	r_idx = "801"
	name = "Greater draconic quylthulg"
	ascii = "Q,g"
	level = 71
	rarity = 3
	hp = "15d70"
	speed = 120
	aaf = 20
	ac = 1
	sleep = 0
	exp = 10500
	blows = []
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'WILD_DUNGEON_01', 'INVISIBLE', 'EMPTY_MIND', 'ANIMAL', 'EVIL', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['BLINK', 'TELE_TO', 'S_HI_DRAGON']
	description = """A massive mound of scaled flesh, throbbing and pulsating with multi-hued 
light.
"""

monsters.register_race(greater_draconic_quylthulg)


class greater_rotting_quylthulg(monster, spellcaster):
	r_idx = "802"
	name = "Greater rotting quylthulg"
	ascii = "Q,u"
	level = 71
	rarity = 3
	hp = "15d70"
	speed = 120
	aaf = 20
	ac = 1
	sleep = 0
	exp = 10500
	blows = []
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'WILD_DUNGEON_01', 'INVISIBLE', 'EMPTY_MIND', 'ANIMAL', 'EVIL', 'RES_TELE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['BLINK', 'TELE_TO', 'S_HI_UNDEAD']
	description = """A massive pile of rotting flesh.  A disgusting stench fills the air as it 
throbs and writhes.
"""

monsters.register_race(greater_rotting_quylthulg)


class null_the_living_void(monster, spellcaster):
	r_idx = "803"
	name = "Null the Living Void"
	ascii = ".,d"
	level = 72
	rarity = 2
	hp = "50d70"
	speed = 110
	aaf = 30
	ac = 50
	sleep = 20
	exp = 32500
	blows = [('TOUCH', 'LOSE_ALL', '6d16'), ('TOUCH', 'EXP_VAMP', '6d16'), ('TOUCH', 'UN_POWER', '6d16')]
	flags = ['WILD_DUNGEON_01', 'PASS_WALL', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'IM_ACID', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'EVIL', 'ELDRITCH_HORROR', 'IM_COLD', 'RES_NETH', 'NO_STUN', 'UNIQUE', 'REGENERATE', 'FORCE_MAXHP']
	sfreq = 5
	sflags = ['BR_NETH', 'BRAIN_SMASH', 'SCARE', 'S_UNDEAD', 'S_HI_UNDEAD', 'DRAIN_MANA', 'HEAL', 'ANIM_DEAD']
	description = """A black hole in the fabric of reality. It simply is not there.
"""

monsters.register_race(null_the_living_void)


class vecna_the_emperor_lich(monster, undead, spellcaster):
	r_idx = "804"
	name = "Vecna, the Emperor Lich"
	ascii = "L,y"
	level = 72
	rarity = 2
	hp = "50d70"
	speed = 130
	aaf = 20
	ac = 42
	sleep = 50
	exp = 30000
	blows = [('TOUCH', 'EXP_80', ''), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'LOSE_DEX', '2d12'), ('TOUCH', 'LOSE_DEX', '2d12')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLINK', 'TELE_TO', 'BLIND', 'HOLD', 'CONF', 'SCARE', 'CAUSE_3', 'CAUSE_4', 'BRAIN_SMASH', 'TRAPS', 'BA_MANA', 'BO_MANA', 'BA_NETH', 'S_MONSTERS', 'S_UNDEAD', 'S_KIN', 'ANIM_DEAD']
	description = """He is a highly cunning, extremely magical being, spoken of in legends.  
This ancient shadow of death wilts any living thing it passes.
"""

monsters.register_race(vecna_the_emperor_lich)


class omarax_the_eye_tyrant(monster, spellcaster):
	r_idx = "805"
	name = "Omarax the Eye Tyrant"
	ascii = "e,y"
	level = 73
	rarity = 4
	hp = "65d70"
	speed = 130
	aaf = 30
	ac = 40
	sleep = 10
	exp = 16000
	blows = [('GAZE', 'EXP_40', '2d6'), ('GAZE', 'PARALYZE', '2d6'), ('GAZE', 'UN_POWER', '2d6'), ('GAZE', 'LOSE_INT', '2d6')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'SMART', 'BASH_DOOR', 'EVIL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BLIND', 'SLOW', 'CONF', 'SCARE', 'DRAIN_MANA', 'MIND_BLAST', 'FORGET', 'DARKNESS', 'BA_DARK', 'BO_ACID', 'BO_FIRE', 'BO_COLD', 'S_KIN']
	description = """A disembodied eye, floating in the air.  His gaze seems to shred your 
soul and his spells crush your will.  He is ancient, his history steeped 
in forgotten evils, his atrocities numerous and sickening.
"""

monsters.register_race(omarax_the_eye_tyrant)


class tsathoggua_the_sleeper_of_nkai(monster, spellcaster):
	r_idx = "806"
	name = "Tsathoggua, the Sleeper of N'kai"
	ascii = "r,D"
	level = 74
	rarity = 4
	hp = "66d70"
	speed = 130
	aaf = 30
	ac = 40
	sleep = 100
	exp = 16500
	blows = [('CRUSH', 'LOSE_ALL', '5d6'), ('CRUSH', 'ACID', '5d6'), ('CRUSH', 'LOSE_ALL', '5d6'), ('CRUSH', 'ACID', '5d6')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'RES_TELE', 'ELDRITCH_HORROR', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_1D2', 'DROP_2D2', 'ONLY_ITEM', 'DROP_GOOD', 'MOVE_BODY', 'SMART', 'BASH_DOOR', 'EVIL', 'IM_POIS', 'IM_ACID', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['S_DEMON', 'S_MONSTERS', 'S_UNDEAD', 'HOLD', 'SCARE', 'MIND_BLAST', 'BR_ACID', 'BR_NETH', 'CAUSE_4']
	description = """"...the formless bulking of a couchant mass. And the mass stirred 
a little... and put forth with infinite slothfulness a huge and 
toad-shaped head. And the head opened its eyes very slowly, as if 
half awakened from slumber, so that they were visible as two slits 
of oozing phosphor in the black browless face."
"""

monsters.register_race(tsathoggua_the_sleeper_of_nkai)


class gerard_strongman_of_amber(amberite, spellcaster):
	r_idx = "807"
	name = "Gerard, Strongman of Amber"
	ascii = "p,u"
	level = 74
	rarity = 1
	hp = "66d70"
	speed = 130
	aaf = 100
	ac = 75
	sleep = 20
	exp = 36500
	blows = [('HIT', 'SHATTER', '15d15'), ('HIT', 'SHATTER', '15d15')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'AMBERITE', 'RES_TELE', 'WILD_DUNGEON_01', 'DROP_SKELETON', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'KILL_WALL', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'ONLY_ITEM', 'EVIL', 'IM_POIS', 'IM_ACID', 'IM_FIRE', 'REGENERATE', 'OPEN_DOOR', 'BASH_DOOR']
	sfreq = 4
	sflags = ['HEAL', 'TPORT', 'TELE_TO', 'SCARE', 'BLIND', 'S_MONSTERS']
	description = """Gerard may not be quite as devious as the other Amberites, but 
he is certainly stronger than the rest of them put together. "...a big, 
powerful man... (He) resembled (Corwin) quite strongly, save that 
his jaw was heavier, and (Corwin) knew he was bigger than him, though 
slower. His strength was a thing out of legend. He wore a dressing gown of 
blue and gray clasped around the middle with a wide, black belt, and he 
stood laughing. About his neck, on a heavy cord, there hung a silver 
hunting horn. He wore a fringe beard and a light mustache. In his right 
hand he held a goblet of wine."
"""

monsters.register_race(gerard_strongman_of_amber)


class ungoliant_the_unlight(monster, spellcaster):
	r_idx = "808"
	name = "Ungoliant, the Unlight"
	ascii = "S,D"
	level = 75
	rarity = 1
	hp = "130d70"
	speed = 120
	aaf = 8
	ac = 80
	sleep = 80
	exp = 35000
	blows = [('BITE', 'POISON', '3d9'), ('BITE', 'POISON', '3d9'), ('STING', 'POISON', '2d5'), ('STING', 'POISON', '2d5')]
	flags = ['UNIQUE', 'FEMALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'BASH_DOOR', 'ANIMAL', 'EVIL', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['HEAL', 'BLIND', 'SLOW', 'CONF', 'SCARE', 'DARKNESS', 'BA_DARK', 'BR_POIS', 'BR_DARK', 'S_SPIDER']
	description = """This enormous, hideous spirit of void is in the form of a spider of 
immense proportions.  She is surrounded by a cloud of Unlight as she sucks 
in all living light into her bloated body.  She is always ravenously 
hungry and would even eat herself to avoid starvation.  She is rumoured to 
have a foul and deadly breath.
"""

monsters.register_race(ungoliant_the_unlight)


class atlach_nacha_the_spider_god(monster, spellcaster):
	r_idx = "809"
	name = "Atlach-Nacha, the Spider God"
	ascii = "S,D"
	level = 75
	rarity = 1
	hp = "130d70"
	speed = 120
	aaf = 8
	ac = 80
	sleep = 80
	exp = 35000
	blows = [('BITE', 'POISON', '3d9'), ('BITE', 'LOSE_STR', '3d9'), ('STING', 'POISON', '2d5'), ('STING', 'LOSE_STR', '2d5')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'ELDRITCH_HORROR', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'BASH_DOOR', 'MOVE_BODY', 'NONLIVING', 'ANIMAL', 'EVIL', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['SCARE', 'BLIND', 'CONF', 'HOLD', 'BR_POIS', 'DARKNESS', 'BA_DARK', 'BR_DARK', 'S_SPIDER', 'S_DEMON']
	description = """"...there was a kind of face on the squat ebon body, low down amid 
the several-jointed legs. The face peered up with a weird expression 
of doubt and inquiry..."
"""

monsters.register_race(atlach_nacha_the_spider_god)


class ygolonac(monster, spellcaster):
	r_idx = "810"
	name = "Y'golonac"
	ascii = "H,R"
	level = 75
	rarity = 1
	hp = "130d70"
	speed = 120
	aaf = 8
	ac = 80
	sleep = 80
	exp = 37500
	blows = [('TOUCH', 'LOSE_INT', '1d20'), ('BITE', 'HURT', '40d1'), ('TOUCH', 'LOSE_WIS', '1d20'), ('BITE', 'HURT', '40d1')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'ELDRITCH_HORROR', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'SMART', 'BASH_DOOR', 'MOVE_BODY', 'NONLIVING', 'EVIL', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['SCARE', 'BLIND', 'CONF', 'HOLD', 'HASTE', 'DRAIN_MANA', 'CAUSE_3', 'CAUSE_4', 'DARKNESS', 'FORGET', 'S_DEMON', 'S_HOUND', 'TPORT', 'TELE_TO']
	description = """"He saw why the shadow on the frosted pane yesterday had been 
headless and he screamed... but before he could scream out his 
protest his breath was cut off, as the hands descended on his 
face and the wed ret mouths opened in their palms."
"""

monsters.register_race(ygolonac)


class aether_hound(monster, spellcaster):
	r_idx = "811"
	name = "Aether hound"
	ascii = "Z,B"
	level = 75
	rarity = 2
	hp = "45d30"
	speed = 120
	aaf = 30
	ac = 50
	sleep = 0
	exp = 10000
	blows = [('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('BITE', 'HURT', '2d12'), ('CLAW', 'HURT', '3d3')]
	flags = ['ATTR_MULTI', 'ATTR_ANY', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'CAN_FLY', 'FRIENDS', 'RES_NETH', 'RES_PLAS', 'RES_NEXU', 'RES_DISE', 'BASH_DOOR', 'AURA_FIRE', 'AURA_ELEC', 'AURA_COLD', 'DROP_CORPSE', 'DROP_SKELETON', 'ANIMAL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BR_ACID', 'BR_FIRE', 'BR_COLD', 'BR_ELEC', 'BR_POIS', 'BR_LITE', 'BR_DARK', 'BR_SOUN', 'BR_CONF', 'BR_CHAO', 'BR_SHAR', 'BR_NETH', 'BR_DISE', 'BR_WALL', 'BR_INER', 'BR_TIME', 'BR_GRAV', 'BR_PLAS', 'BR_NEXU']
	description = """A shifting, swirling form.  It seems to be all colours and sizes and 
shapes, though the dominant form is that of a huge dog.  You feel very 
uncertain all of a sudden.
"""

monsters.register_race(aether_hound)


class warp_demon(demon, spellcaster):
	r_idx = "812"
	name = "Warp demon"
	ascii = "U,o"
	level = 76
	rarity = 1
	hp = "30d70"
	speed = 120
	aaf = 30
	ac = 50
	sleep = 20
	exp = 15000
	blows = [('HIT', 'UN_BONUS', '14d11'), ('HIT', 'UN_POWER', '14d11')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'WILD_DUNGEON_01', 'DROP_GOOD', 'DROP_60', 'DROP_1D2', 'SMART', 'PASS_WALL', 'RES_NEXU', 'RES_NETH', 'NONLIVING', 'CAN_FLY', 'EVIL', 'IM_POIS', 'IM_ELEC', 'IM_COLD', 'IM_FIRE', 'DEMON']
	sfreq = 4
	sflags = ['HEAL', 'TPORT', 'S_DEMON', 'BRAIN_SMASH', 'DRAIN_MANA', 'BR_PLAS']
	description = """A huge demon who guards the cross-dimensional gateways.
"""

monsters.register_race(warp_demon)


class eric_the_usurper(amberite, spellcaster):
	r_idx = "813"
	name = "Eric the Usurper"
	ascii = "p,R"
	level = 76
	rarity = 1
	hp = "58d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 15
	exp = 35500
	blows = [('HIT', 'HURT', '10d15'), ('HIT', 'HURT', '10d15'), ('HIT', 'EAT_ITEM', '10d15')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'SMART', 'AMBERITE', 'RES_TELE', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'DROP_SKELETON', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'ONLY_ITEM', 'EVIL', 'IM_POIS', 'IM_ACID', 'IM_ELEC', 'REGENERATE']
	sfreq = 4
	sflags = ['BLIND', 'S_MONSTER', 'DRAIN_MANA', 'S_MONSTERS', 'SCARE', 'CONF', 'TPORT', 'TELE_TO', 'CAUSE_3', 'BO_ACID', 'HOLD', 'BA_FIRE', 'TRAPS', 'TELE_AWAY', 'BRAIN_SMASH', 'FORGET', 'BO_WATE', 'CAUSE_4', 'BO_NETH', 'DARKNESS', 'S_ANGEL', 'S_DEMON']
	description = """When Oberon disappeared, Eric seized the opportunity and claimed 
the throne, eliminating those who would have opposed him. "Handsome by 
anyone's standards, his hair was so dark, as to be almost blue. His beard 
curled around the mouth that always smiled, and he was dressed simply in a 
leather jacket and leggings, a plain cloak, high black boots, and he wore 
a red sword belt bearing a long silvery saber and clasped with a ruby, 
and his high cloak, collar round his head was lined with red and the 
trimmings of his sleeves matched it. His hands, thumbs hooked behind 
his belt, were terribly strong and prominent. A pair of black gloves 
jutted from his belt near his right tip."
"""

monsters.register_race(eric_the_usurper)


class yig_father_of_serpents(monster, spellcaster):
	r_idx = "814"
	name = "Yig, Father of Serpents"
	ascii = "R,g"
	level = 76
	rarity = 1
	hp = "167d30"
	speed = 130
	aaf = 100
	ac = 62
	sleep = 15
	exp = 37500
	blows = [('CLAW', 'POISON', '8d10'), ('CLAW', 'POISON', '8d10'), ('CRUSH', 'HURT', '8d15'), ('BITE', 'HURT', '100d1')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'SMART', 'RES_TELE', 'ANIMAL', 'WILD_DUNGEON_01', 'ESCORT', 'ESCORTS', 'CAN_SWIM', 'ELDRITCH_HORROR', 'OPEN_DOOR', 'BASH_DOOR', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'ONLY_ITEM', 'EVIL', 'IM_POIS', 'IM_ACID', 'IM_ELEC', 'REGENERATE']
	sfreq = 4
	sflags = ['BR_POIS', 'BR_NUKE', 'BR_ACID', 'S_HYDRA', 'S_KIN', 'S_DEMON']
	description = """"The half-human father of serpents... the snake-god of the central 
plains tribes -- presumably the primal source of the more 
southerly Quetzalcoatl or Kukulcan -- was odd, half-anthropomorphic 
devil."
"""

monsters.register_race(yig_father_of_serpents)


class unmaker(monster, spellcaster):
	r_idx = "815"
	name = "Unmaker"
	ascii = "E,v"
	level = 77
	rarity = 4
	hp = "6d50"
	speed = 120
	aaf = 60
	ac = 25
	sleep = 60
	exp = 10000
	blows = [('TOUCH', 'LOSE_ALL', '10d10'), ('TOUCH', 'UN_BONUS', '10d10'), ('TOUCH', 'UN_POWER', '10d10')]
	flags = ['KILL_WALL', 'KILL_ITEM', 'KILL_BODY', 'NO_FEAR', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'MULTIPLY', 'CAN_FLY', 'ATTR_MULTI', 'SHAPECHANGER', 'ATTR_ANY', 'DROP_60', 'DROP_GOOD', 'POWERFUL', 'AURA_ELEC', 'AURA_FIRE', 'AURA_COLD', 'BASH_DOOR', 'IM_ELEC', 'IM_FIRE', 'RES_NEXU', 'IM_COLD', 'IM_POIS', 'IM_ACID', 'RES_PLAS', 'RES_DISE', 'COLD_BLOOD', 'NONLIVING', 'RAND_50']
	sfreq = 5
	sflags = ['BR_CHAO']
	description = """Summoned from the Courts of Chaos, it is a mass of sentient Logrus, 
spreading uncontrollably and destroying everything in its path.
"""

monsters.register_race(unmaker)


class cyberdemon(demon, spellcaster):
	r_idx = "816"
	name = "Cyberdemon"
	ascii = "U,u"
	level = 77
	rarity = 4
	hp = "70d70"
	speed = 120
	aaf = 90
	ac = 45
	sleep = 90
	exp = 30000
	blows = [('HIT', 'HURT', '1d50'), ('HIT', 'HURT', '1d50'), ('HIT', 'HURT', '1d50'), ('HIT', 'HURT', '1d50')]
	flags = ['RAND_25', 'EVIL', 'DEMON', 'STUPID', 'WILD_DUNGEON_01', 'DROP_2D2', 'DROP_GOOD', 'ONLY_ITEM', 'RES_TELE', 'IM_POIS', 'IM_FIRE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NONLIVING']
	sfreq = 4
	sflags = ['ROCKET']
	description = """Reverbrant metal steps announce the arrival of this huge creature, 
half demon half machine. It has an unsurpassable firepower.
"""

monsters.register_race(cyberdemon)


class hela_queen_of_the_dead(monster, spellcaster):
	r_idx = "817"
	name = "Hela, Queen of the Dead"
	ascii = "p,g"
	level = 78
	rarity = 3
	hp = "74d70"
	speed = 130
	aaf = 60
	ac = 55
	sleep = 10
	exp = 45000
	blows = [('TOUCH', 'LOSE_ALL', '50d1'), ('TOUCH', 'EXP_VAMP', '50d1'), ('TOUCH', 'UN_BONUS', '50d1')]
	flags = ['UNIQUE', 'FEMALE', 'CAN_SPEAK', 'POWERFUL', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GREAT', 'DROP_GOOD', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'CAN_FLY', 'EVIL', 'RES_NETH', 'IM_COLD', 'IM_POIS', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['CAUSE_3', 'CAUSE_4', 'HAND_DOOM', 'TELE_TO', 'HOLD', 'S_UNDEAD', 'S_HI_UNDEAD', 'S_HI_DRAGON', 'FORGET', 'SCARE', 'BLIND', 'BA_DARK', 'BA_NETH', 'HEAL', 'ANIM_DEAD']
	description = """The Norse ruler of Hel is a merciless queen, who ever hunts for more 
souls to add to her collection of tortured spirits. She arrives in 
an ominous green robe, a certain sign of impending doom, to claim 
as her own those who die an ignoble death.
"""

monsters.register_race(hela_queen_of_the_dead)


class the_mouth_of_sauron(monster, spellcaster):
	r_idx = "818"
	name = "The Mouth of Sauron"
	ascii = "p,v"
	level = 78
	rarity = 3
	hp = "70d70"
	speed = 130
	aaf = 60
	ac = 50
	sleep = 10
	exp = 38000
	blows = [('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_BONUS', '6d8'), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'UN_POWER', '')]
	flags = ['UNIQUE', 'MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_4D2', 'DROP_GOOD', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['TELE_TO', 'HOLD', 'CAUSE_3', 'TRAPS', 'ANIM_DEAD', 'BO_PLAS', 'BA_DARK', 'BA_MANA', 'BA_FIRE', 'BA_WATE', 'BA_NETH']
	description = """The Mouth of Sauron is a mighty spell caster.  So old that even he cannot 
remember his own name, his power and evil are undeniable.  He believes 
unshakeably that he is unbeatable and laughs as he weaves his awesome 
spells.
"""

monsters.register_race(the_mouth_of_sauron)


class klingsor_evil_master_of_magic(monster, spellcaster):
	r_idx = "819"
	name = "Klingsor, Evil Master of Magic"
	ascii = "p,y"
	level = 78
	rarity = 3
	hp = "70d70"
	speed = 130
	aaf = 60
	ac = 50
	sleep = 10
	exp = 40000
	blows = [('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_POWER', '6d8'), ('TOUCH', 'BLIND', ''), ('TOUCH', 'CONFUSE', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'POWERFUL', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'DROP_CORPSE', 'DROP_SKELETON', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GREAT', 'DROP_GOOD', 'DROP_CHOSEN', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['CAUSE_3', 'TELE_TO', 'BA_FIRE', 'DRAIN_MANA', 'HOLD', 'TRAPS', 'BA_WATE', 'BO_PLAS', 'BA_NETH', 'BA_MANA', 'BA_DARK', 'S_HI_UNDEAD', 'BA_CHAO', 'HAND_DOOM', 'ANIM_DEAD']
	description = """Klingsor, whose hopeless effort to join the Knights of the Grail 
was thwarted, turned to black magic and became a deadly necromancer.
"""

monsters.register_race(klingsor_evil_master_of_magic)


class corwin_lord_of_avalon(amberite, spellcaster):
	r_idx = "820"
	name = "Corwin, Lord of Avalon"
	ascii = "p,r"
	level = 78
	rarity = 1
	hp = "59d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 15
	exp = 35500
	blows = [('HIT', 'HURT', '10d15'), ('HIT', 'HURT', '10d15'), ('HIT', 'LOSE_CON', '10d15')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'SMART', 'AMBERITE', 'RES_TELE', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'DROP_SKELETON', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'ONLY_ITEM', 'DROP_CHOSEN', 'EVIL', 'IM_COLD', 'IM_POIS', 'IM_ACID', 'IM_ELEC', 'REGENERATE']
	sfreq = 4
	sflags = ['SCARE', 'CONF', 'TPORT', 'TELE_TO', 'S_MONSTER', 'DRAIN_MANA', 'CAUSE_3', 'BO_ACID', 'BO_MANA', 'HOLD', 'BA_FIRE', 'BA_COLD', 'TRAPS', 'TELE_AWAY', 'HEAL', 'BRAIN_SMASH', 'BA_WATE', 'BA_NETH', 'FORGET', 'BO_WATE', 'BO_NETH', 'CAUSE_4', 'DARKNESS', 'S_MONSTERS', 'BO_PLAS', 'S_ANGEL']
	description = """Corwin is one of the most feared and respected Amberites; his skill 
and cunning are well known. "Green eyes, black hair, dressed in black 
and silver, yes. (Corwin) had on a cloak and it was slightly furled 
as by a wind. (He) had on black boots, like Eric's, and (he) too bore a 
blade, only (his) was heavier, though not quite as long as (Eric's). 
(He) had (his) gloves on and they were silver and scaled. The clasp 
at (his) neck was cast in the form of a silver rose."
"""

monsters.register_race(corwin_lord_of_avalon)


class the_emperor_quylthulg(monster, spellcaster):
	r_idx = "821"
	name = "The Emperor Quylthulg"
	ascii = "Q,y"
	level = 78
	rarity = 3
	hp = "38d100"
	speed = 130
	aaf = 30
	ac = 1
	sleep = 0
	exp = 20000
	blows = []
	flags = ['UNIQUE', 'RES_TELE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_4D2', 'INVISIBLE', 'ANIMAL', 'EVIL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['BRAIN_SMASH', 'S_HI_UNDEAD', 'S_HI_DRAGON']
	description = """A huge seething mass of flesh with a rudimentary intelligence, the Emperor 
Quylthulg changes colours in front of your eyes.  Pulsating first one 
colour then the next, it knows only it must bring help to protect itself.
"""

monsters.register_race(the_emperor_quylthulg)


class qlzqqlzuup_the_lord_of_flesh(monster, spellcaster):
	r_idx = "822"
	name = "Qlzqqlzuup, the Lord of Flesh"
	ascii = "Q,B"
	level = 78
	rarity = 3
	hp = "38d100"
	speed = 130
	aaf = 30
	ac = 1
	sleep = 0
	exp = 20000
	blows = []
	flags = ['UNIQUE', 'CAN_SPEAK', 'RES_TELE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NEVER_MOVE', 'NEVER_BLOW', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_4D2', 'INVISIBLE', 'ATTR_MULTI', 'ANIMAL', 'EVIL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 1
	sflags = ['S_MONSTER', 'S_MONSTERS', 'S_UNDEAD', 'S_DEMON', 'S_DRAGON', 'S_ANGEL', 'S_HYDRA', 'S_SPIDER', 'S_ANT', 'S_HOUND', 'S_UNIQUE', 'S_AMBERITES', 'S_HI_UNDEAD', 'S_HI_DRAGON', 'S_KIN']
	description = """This disgusting creature squeals and snorts as it writhes on the floor.  
It pulsates with evil.  Its intent is to overwhelm you with monster after 
monster, until it can greedily dine on your remains.
"""

monsters.register_race(qlzqqlzuup_the_lord_of_flesh)


class cthugha_the_living_flame(monster, spellcaster):
	r_idx = "823"
	name = "Cthugha, the Living Flame"
	ascii = "E,R"
	level = 78
	rarity = 3
	hp = "38d100"
	speed = 133
	aaf = 30
	ac = 50
	sleep = 20
	exp = 17500
	blows = [('TOUCH', 'BLIND', ''), ('TOUCH', 'FIRE', '12d10'), ('TOUCH', 'FIRE', '12d10'), ('TOUCH', 'FIRE', '12d10')]
	flags = ['UNIQUE', 'RES_TELE', 'AURA_FIRE', 'IM_FIRE', 'CAN_FLY', 'RES_PLAS', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NONLIVING', 'ONLY_ITEM', 'DROP_4D2', 'SMART', 'ELDRITCH_HORROR', 'EVIL', 'KILL_ITEM', 'KILL_BODY', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 4
	sflags = ['BR_FIRE', 'S_KIN']
	description = """"But even though we had shielded our eyes, it was impossible not 
to see the great amorphous shapes streaming skyward from the 
accursed place, nor the equally great being hovering like a cloud 
of living fire above the trees."
"""

monsters.register_race(cthugha_the_living_flame)


class benedict_the_ideal_warrior(amberite, spellcaster):
	r_idx = "824"
	name = "Benedict, the Ideal Warrior"
	ascii = "p,W"
	level = 79
	rarity = 1
	hp = "69d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 15
	exp = 38500
	blows = [('HIT', 'HURT', '15d15'), ('HIT', 'HURT', '15d15'), ('HIT', 'HURT', '15d15'), ('HIT', 'HURT', '15d15')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'SMART', 'AMBERITE', 'RES_TELE', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_CORPSE', 'DROP_SKELETON', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NO_CONF', 'NO_SLEEP', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_60', 'DROP_90', 'ONLY_ITEM', 'EVIL', 'IM_COLD', 'IM_POIS', 'IM_ACID', 'IM_ELEC', 'IM_FIRE', 'REGENERATE']
	sfreq = 4
	sflags = ['SCARE', 'CONF', 'TPORT', 'TELE_TO', 'DRAIN_MANA', 'TRAPS', 'BRAIN_SMASH', 'TELE_AWAY', 'CAUSE_4', 'S_MONSTERS', 'S_HI_DRAGON', 'S_DEMON', 'S_ANGEL', 'TELE_LEVEL', 'FORGET', 'HEAL']
	description = """In the field of combat Benedict rules supremehe was still superior to Corwin. "Benedict, tall and dour; thin of 
body, thin of face, wide of mind. He wore orange and yellow and brown 
and reminded me of haystacks and pumpkins and scarecrows and the 
Legend of Sleepy Hollow. He had a long, strong jaw and hazel eyes and 
brown hair that never curled. He stood beside a tan horse and leaned 
upon a lance about which was twined a rope of flowers. He seldom 
laughed."
"""

monsters.register_race(benedict_the_ideal_warrior)


class the_witch_king_of_angmar(monster, undead, spellcaster):
	r_idx = "825"
	name = "The Witch-King of Angmar"
	ascii = "W,D"
	level = 80
	rarity = 3
	hp = "60d70"
	speed = 130
	aaf = 90
	ac = 60
	sleep = 10
	exp = 42000
	blows = [('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '10d10'), ('HIT', 'EXP_80', '5d5'), ('HIT', 'EXP_80', '5d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'UNDEAD', 'IM_COLD', 'IM_POIS', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['TELE_AWAY', 'BLIND', 'HOLD', 'SCARE', 'CAUSE_3', 'BRAIN_SMASH', 'BO_MANA', 'BA_NETH', 'S_KIN', 'S_HI_UNDEAD', 'S_HI_DRAGON', 'S_MONSTERS', 'ANIM_DEAD']
	description = """The Chief of the Ringwraiths.  A fell being of devastating power.  His 
spells are lethal and his combat blows crushingly hard.  He moves at 
speed, and commands legions of evil to do his bidding.  It is said that he 
is fated never to die by the hand of mortal man.
"""

monsters.register_race(the_witch_king_of_angmar)


class cyaegha(demon, spellcaster):
	r_idx = "826"
	name = "Cyaegha"
	ascii = "e,D"
	level = 80
	rarity = 3
	hp = "64d70"
	speed = 130
	aaf = 90
	ac = 60
	sleep = 10
	exp = 44444
	blows = [('CRUSH', 'HURT', '15d15'), ('CRUSH', 'HURT', '15d15'), ('CRUSH', 'HURT', '15d15'), ('CRUSH', 'HURT', '15d15')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'RES_TELE', 'ELDRITCH_HORROR', 'CAN_FLY', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'SMART', 'ONLY_ITEM', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'EVIL', 'DEMON', 'IM_ACID', 'IM_ELEC', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['TELE_AWAY', 'BLIND', 'HOLD', 'SCARE', 'BRAIN_SMASH', 'BR_DARK', 'BA_DARK', 'BR_NETH', 'HAND_DOOM', 'FORGET', 'S_HI_UNDEAD', 'S_DEMON', 'S_MONSTERS', 'S_HYDRA']
	description = """"...it was a gigantic eye staring down at them. Around the eye, 
the sky split; deep clefts opened through which the darkness 
began to ooze, a darkness blacker than the night, which crawled 
down as a set of slimy tentacles, taking on more form, more 
definite shape... something was standing, outlined against 
the black sky, something which had tentacles of darkness 
and a green-glowing eye."
"""

monsters.register_race(cyaegha)


class pazuzu_lord_of_air(demon, spellcaster):
	r_idx = "827"
	name = "Pazuzu, Lord of Air"
	ascii = "B,w"
	level = 82
	rarity = 2
	hp = "55d70"
	speed = 140
	aaf = 40
	ac = 62
	sleep = 10
	exp = 30000
	blows = [('HIT', 'ELEC', '12d12'), ('HIT', 'ELEC', '12d12'), ('HIT', 'ELEC', '12d12'), ('HIT', 'ELEC', '12d12')]
	flags = ['UNIQUE', 'MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'DEMON', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['MIND_BLAST', 'BO_ELEC', 'BO_MANA', 'BA_ELEC']
	description = """A winged humanoid from the Planes of Hell, Pazuzu grins inhumanely at you 
as he decides your fate.
"""

monsters.register_race(pazuzu_lord_of_air)


class ithaqua_the_windwalker(demon, spellcaster):
	r_idx = "828"
	name = "Ithaqua the Windwalker"
	ascii = "Y,B"
	level = 82
	rarity = 2
	hp = "55d70"
	speed = 140
	aaf = 40
	ac = 62
	sleep = 10
	exp = 32500
	blows = [('CLAW', 'COLD', '12d12'), ('CLAW', 'COLD', '12d12'), ('CRUSH', 'HURT', '12d12'), ('CRUSH', 'HURT', '12d12')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'ESCORT', 'ELDRITCH_HORROR', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'CAN_FLY', 'AURA_COLD', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'AURA_ELEC', 'INVISIBLE', 'OPEN_DOOR', 'BASH_DOOR', 'NONLIVING', 'EVIL', 'DEMON', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BO_MANA', 'SCARE', 'BR_COLD', 'S_DEMON', 'BO_ELEC', 'BA_ELEC', 'MIND_BLAST', 'CAUSE_4', 'BA_CHAO', 'BA_WATE', 'S_HI_UNDEAD', 'S_KIN']
	description = """The Wendigo, moving so fast that you can see little except two 
glowing eyes burning with hatred. "The stars had been blotted out... 
the great cloud which had obscured the the sky looked curiously 
like the outline of a great man. And... where the top of 
the cloud must have been, where the head of the thing should have 
been, there were two gleaming stars, burning bright -- like eyes!"
"""

monsters.register_race(ithaqua_the_windwalker)


class hell_hound_of_julian(monster, spellcaster):
	r_idx = "829"
	name = "Hell hound of Julian"
	ascii = "C,r"
	level = 83
	rarity = 4
	hp = "48d7"
	speed = 120
	aaf = 25
	ac = 40
	sleep = 30
	exp = 600
	blows = [('BITE', 'FIRE', '3d12'), ('BITE', 'FIRE', '3d12'), ('BITE', 'FIRE', '3d12')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'RAND_25', 'FRIENDS', 'AURA_FIRE', 'BASH_DOOR', 'MOVE_BODY', 'ANIMAL', 'EVIL', 'IM_FIRE']
	sfreq = 5
	sflags = ['BR_FIRE']
	description = """It is a giant dog that glows with heat.  Flames pour from its nostrils.
"""

monsters.register_race(hell_hound_of_julian)


class cantoras_the_skeletal_lord(monster, undead, spellcaster):
	r_idx = "830"
	name = "Cantoras, the Skeletal Lord"
	ascii = "s,B"
	level = 84
	rarity = 2
	hp = "75d70"
	speed = 140
	aaf = 20
	ac = 60
	sleep = 80
	exp = 45000
	blows = [('GAZE', 'EXP_80', ''), ('GAZE', 'EXP_80', ''), ('TOUCH', 'POISON', '3d5'), ('TOUCH', 'POISON', '3d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 1
	sflags = ['TELE_TO', 'SLOW', 'SCARE', 'CAUSE_4', 'BRAIN_SMASH', 'BO_ICEE', 'BO_MANA', 'BA_WATE', 'BA_NETH', 'S_HI_UNDEAD', 'ANIM_DEAD']
	description = """A legion of evil undead druj animating the skeleton of a once mighty 
sorcerer.  His power is devastating and his speed unmatched in the 
underworld.  Flee his wrath!
"""

monsters.register_race(cantoras_the_skeletal_lord)


class mephistopheles_lord_of_hell(demon, spellcaster):
	r_idx = "831"
	name = "Mephistopheles, Lord of Hell"
	ascii = "U,r"
	level = 84
	rarity = 2
	hp = "30d167"
	speed = 140
	aaf = 20
	ac = 75
	sleep = 50
	exp = 42500
	blows = [('GAZE', 'EXP_VAMP', '1d15'), ('GAZE', 'TERRIFY', '1d15'), ('TOUCH', 'FIRE', '4d5'), ('TOUCH', 'UN_POWER', '4d5')]
	flags = ['MALE', 'UNIQUE', 'CAN_SPEAK', 'CAN_FLY', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'IM_FIRE', 'RES_PLAS', 'RES_NETH', 'AURA_FIRE', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'EVIL', 'DEMON', 'ESCORTS', 'IM_COLD', 'IM_POIS']
	sfreq = 3
	sflags = ['TELE_TO', 'SCARE', 'HOLD', 'BRAIN_SMASH', 'S_DEMON', 'S_HI_UNDEAD', 'S_UNDEAD', 'BR_FIRE', 'BR_NETH', 'S_CYBER', 'HAND_DOOM', 'ANIM_DEAD']
	description = """A duke of hell, in the flesh.
"""

monsters.register_race(mephistopheles_lord_of_hell)


class godzilla(monster, spellcaster):
	r_idx = "832"
	name = "Godzilla"
	ascii = "R,g"
	level = 84
	rarity = 2
	hp = "85d70"
	speed = 130
	aaf = 50
	ac = 140
	sleep = 20
	exp = 35000
	blows = [('CLAW', 'POISON', '5d10'), ('CLAW', 'POISON', '5d10'), ('BITE', 'HURT', '20d10'), ('CRUSH', 'UN_BONUS', '5d12')]
	flags = ['UNIQUE', 'CAN_SWIM', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RES_PLAS', 'RES_DISE', 'RES_TELE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'IM_FIRE', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'IM_POIS']
	sfreq = 2
	sflags = ['BR_DISE', 'BR_PLAS', 'BR_NUKE', 'BR_POIS', 'BR_ACID']
	description = """Godzilla rose from the contaminated sea. 
Fear its anger, for its devastation is unmatched!
"""

monsters.register_race(godzilla)


class abhoth_source_of_uncleanness(demon, spellcaster):
	r_idx = "833"
	name = "Abhoth, Source of Uncleanness"
	ascii = "J,G"
	level = 85
	rarity = 3
	hp = "60d105"
	speed = 130
	aaf = 50
	ac = 75
	sleep = 0
	exp = 40000
	blows = [('TOUCH', 'ACID', '11d11'), ('TOUCH', 'DISEASE', '11d11'), ('TOUCH', 'ACID', '11d11'), ('TOUCH', 'POISON', '11d11')]
	flags = ['ONLY_ITEM', 'DROP_1D2', 'CAN_SWIM', 'WILD_DUNGEON_01', 'DROP_GOOD', 'DROP_GREAT', 'CAN_SPEAK', 'EVIL', 'UNIQUE', 'NEVER_MOVE', 'FORCE_MAXHP', 'FORCE_SLEEP', 'IM_ACID', 'IM_POIS', 'IM_COLD', 'IM_ELEC', 'RES_NETH', 'RES_WATE', 'RES_NEXU', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'DEMON', 'SMART', 'ELDRITCH_HORROR', 'COLD_BLOOD', 'RES_DISE', 'NO_STUN']
	sfreq = 6
	sflags = ['S_MONSTERS', 'S_DEMON', 'S_HI_DRAGON', 'S_HI_UNDEAD', 'HEAL', 'TELE_AWAY', 'TPORT', 'CAUSE_4', 'BRAIN_SMASH', 'DRAIN_MANA', 'BR_NUKE', 'BR_POIS', 'BR_CHAO', 'BR_NEXU', 'ANIM_DEAD']
	description = """"...in the pool [there was] a grayish, horrid mass that nearly choked 
it from rim to rim. Here, it seemed, was the ultimate source of all 
miscreation and abomination. For the gray mass quobbed and quivered, 
and swelled perpetually; and from it, in manifold fission, were 
spawned the anatomies that crept away on every side through the 
grotto."
"""

monsters.register_race(abhoth_source_of_uncleanness)


class ymir_the_ice_giant(giant, spellcaster):
	r_idx = "834"
	name = "Ymir the Ice Giant"
	ascii = "P,w"
	level = 85
	rarity = 2
	hp = "86d70"
	speed = 130
	aaf = 50
	ac = 80
	sleep = 20
	exp = 32500
	blows = [('HIT', 'COLD', '5d10'), ('HIT', 'HURT', '5d10'), ('HIT', 'COLD', '5d10'), ('HIT', 'HURT', '5d10')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'RES_NETH', 'RES_WATE', 'MALE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'DROP_GOOD', 'EVIL', 'ESCORT', 'GIANT', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'TAKE_ITEM', 'RES_DISE', 'IM_COLD', 'NO_CONF', 'NO_SLEEP', 'IM_POIS', 'HURT_FIRE', 'COLD_BLOOD', 'AURA_COLD', 'DROP_CORPSE']
	sfreq = 3
	sflags = ['BR_COLD', 'BO_ICEE', 'DARKNESS', 'HEAL', 'TELE_TO', 'S_KIN', 'S_HI_UNDEAD']
	description = """Ymir is one of the oldest beings there are. He looks like a giant 
humanoid made of ice.
"""

monsters.register_race(ymir_the_ice_giant)


class loki_the_trickster(giant, spellcaster):
	r_idx = "835"
	name = "Loki the Trickster"
	ascii = "P,g"
	level = 85
	rarity = 2
	hp = "64d110"
	speed = 130
	aaf = 50
	ac = 80
	sleep = 20
	exp = 50000
	blows = [('HIT', 'BLIND', '6d11'), ('HIT', 'UN_BONUS', '6d11'), ('HIT', 'UN_POWER', '6d11')]
	flags = ['UNIQUE', 'MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'RES_NETH', 'RES_PLAS', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'EVIL', 'AURA_FIRE', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'TAKE_ITEM', 'CAN_SPEAK', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP', 'IM_POIS', 'HURT_COLD', 'GIANT', 'REGENERATE']
	sfreq = 3
	sflags = ['SHRIEK', 'HASTE', 'HEAL', 'DRAIN_MANA', 'TPORT', 'TELE_TO', 'TELE_AWAY', 'TELE_LEVEL', 'FORGET', 'S_CYBER', 'S_DEMON', 'HAND_DOOM', 'S_HI_UNDEAD', 'S_UNIQUE', 'S_HI_DRAGON', 'BA_DARK', 'BA_MANA', 'S_AMBERITES', 'ANIM_DEAD', 'INVULNER']
	description = """Loki, the god of mischief, is a nasty person. He will use every 
dirty trick in the book, and then some. In the end, his half-giant 
heritage is bound to show, as he will defect to the side of the 
giants and fight against the other gods of Asgard.
"""

monsters.register_race(loki_the_trickster)


class star_spawn_of_cthulhu(demon, spellcaster):
	r_idx = "836"
	name = "Star-spawn of Cthulhu"
	ascii = "U,G"
	level = 86
	rarity = 2
	hp = "75d70"
	speed = 130
	aaf = 90
	ac = 45
	sleep = 90
	exp = 44000
	blows = [('CLAW', 'POISON', '1d30'), ('CLAW', 'ACID', '1d30'), ('TOUCH', 'UN_POWER', '1d10'), ('CRUSH', 'UN_BONUS', '2d33')]
	flags = ['WILD_DUNGEON_01', 'RAND_25', 'KILL_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'RES_NETH', 'ELDRITCH_HORROR', 'DROP_1D2', 'DROP_2D2', 'DROP_90', 'ONLY_ITEM', 'FORCE_SLEEP', 'FORCE_MAXHP', 'EVIL', 'DEMON', 'IM_POIS', 'IM_COLD', 'IM_ACID', 'IM_ELEC', 'RES_TELE', 'NONLIVING', 'POWERFUL', 'IM_FIRE', 'CAN_SWIM', 'NO_CONF', 'DROP_GOOD']
	sfreq = 3
	sflags = ['SCARE', 'CONF', 'S_DEMON', 'S_UNDEAD', 'DRAIN_MANA', 'BR_ACID', 'BR_FIRE', 'TPORT', 'S_MONSTERS', 'BRAIN_SMASH', 'BR_NETH', 'HEAL', 'MIND_BLAST', 'BA_NUKE', 'ANIM_DEAD']
	description = """The last remnants of sanity threaten to leave your brain as you 
behold this titanic bat-winged, octopus-headed unholy abomination. 
"They all lay in stone houses in their great city of R'lyeh, 
preserved by the spells of mighty Cthulhu for a glorious 
resurrection when the stars and the earth might once more 
be ready..."
"""

monsters.register_race(star_spawn_of_cthulhu)


class surtur_the_giant_fire_demon(demon, spellcaster):
	r_idx = "837"
	name = "Surtur the Giant Fire Demon"
	ascii = "P,r"
	level = 85
	rarity = 2
	hp = "65d110"
	speed = 130
	aaf = 50
	ac = 80
	sleep = 20
	exp = 45000
	blows = [('HIT', 'FIRE', '6d11'), ('HIT', 'HURT', '6d11'), ('HIT', 'FIRE', '6d11'), ('HIT', 'HURT', '6d11')]
	flags = ['UNIQUE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'RES_NETH', 'RES_PLAS', 'MALE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'DROP_GOOD', 'EVIL', 'AURA_FIRE', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'TAKE_ITEM', 'CAN_SPEAK', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP', 'IM_POIS', 'HURT_COLD', 'GIANT', 'DEMON']
	sfreq = 5
	sflags = ['BR_FIRE', 'BR_PLAS', 'BLIND', 'TELE_TO', 'S_KIN', 'S_HI_UNDEAD', 'HAND_DOOM', 'TELE_AWAY', 'S_CYBER', 'S_DEMON']
	description = """Surtur is also one of the most ancient of all creatures. He is a demonic 
giant of fire, who is destined to set the nine worlds afire with his 
accursed sword of doom on the day of Ragnarok.
"""

monsters.register_race(surtur_the_giant_fire_demon)


class the_tarrasque(monster, spellcaster):
	r_idx = "838"
	name = "The Tarrasque"
	ascii = "R,v"
	level = 84
	rarity = 2
	hp = "85d70"
	speed = 130
	aaf = 50
	ac = 92
	sleep = 20
	exp = 35000
	blows = [('HIT', 'HURT', '10d10'), ('HIT', 'HURT', '10d10'), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'UN_POWER', '')]
	flags = ['UNIQUE', 'ATTR_MULTI', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'IM_FIRE', 'IM_COLD', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BR_FIRE', 'BR_COLD', 'BR_DISE']
	description = """The Tarrasque is a massive reptile of legend, rumoured to be unkillable 
and immune to magic.  Fear its anger, for its devastation is unmatched!
"""

monsters.register_race(the_tarrasque)


class lungorthin_the_balrog_of_white_fire(demon, spellcaster):
	r_idx = "839"
	name = "Lungorthin, the Balrog of White Fire"
	ascii = "U,w"
	level = 85
	rarity = 2
	hp = "60d100"
	speed = 130
	aaf = 20
	ac = 62
	sleep = 80
	exp = 37000
	blows = [('HIT', 'FIRE', '8d12'), ('HIT', 'FIRE', '8d12'), ('CRUSH', 'HURT', '8d12'), ('TOUCH', 'UN_POWER', '')]
	flags = ['UNIQUE', 'MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ESCORTS', 'CAN_FLY', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DEMON', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_FIRE', 'S_DEMON', 'S_HI_UNDEAD']
	description = """A massive form cloaked in flame.  Lungorthin stares balefully at you with 
eyes that smoulder red.  The dungeon floor where he stands is scorched by 
the heat of his body.
"""

monsters.register_race(lungorthin_the_balrog_of_white_fire)


class draugluin_sire_of_all_werewolves(monster, spellcaster):
	r_idx = "840"
	name = "Draugluin, Sire of All Werewolves"
	ascii = "C,u"
	level = 87
	rarity = 2
	hp = "70d70"
	speed = 130
	aaf = 80
	ac = 45
	sleep = 90
	exp = 40000
	blows = [('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3'), ('BITE', 'POISON', '2d6'), ('BITE', 'POISON', '2d6')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ESCORTS', 'DROP_CORPSE', 'RAND_25', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'ANIMAL', 'EVIL', 'IM_POIS']
	sfreq = 3
	sflags = ['SCARE', 'S_MONSTERS', 'S_HOUND']
	description = """Draugluin provides Sauron with a fearsome personal guard.  He is an 
enormous wolf inhabited with a human spirit.  He is chief of all his kind.
"""

monsters.register_race(draugluin_sire_of_all_werewolves)


class shuma_gorath(demon, spellcaster):
	r_idx = "841"
	name = "Shuma-Gorath"
	ascii = "e,G"
	level = 88
	rarity = 3
	hp = "85d70"
	speed = 130
	aaf = 50
	ac = 75
	sleep = 0
	exp = 47000
	blows = [('CRUSH', 'HURT', '12d12'), ('CRUSH', 'HURT', '12d12'), ('GAZE', 'LOSE_INT', '2d12'), ('GAZE', 'LOSE_WIS', '2d12')]
	flags = ['ONLY_ITEM', 'DROP_1D2', 'CAN_SWIM', 'WILD_DUNGEON_01', 'DROP_GOOD', 'DROP_GREAT', 'CAN_SPEAK', 'EVIL', 'UNIQUE', 'NEVER_MOVE', 'FORCE_MAXHP', 'FORCE_SLEEP', 'IM_ACID', 'IM_POIS', 'IM_COLD', 'IM_ELEC', 'RES_NETH', 'RES_WATE', 'RES_NEXU', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'DEMON', 'SMART', 'ELDRITCH_HORROR', 'COLD_BLOOD', 'RES_DISE', 'NO_STUN']
	sfreq = 2
	sflags = ['FORGET', 'HEAL', 'HAND_DOOM', 'BA_MANA', 'TPORT', 'TELE_AWAY', 'TELE_LEVEL', 'CONF', 'BLIND', 'BRAIN_SMASH', 'DRAIN_MANA', 'S_MONSTERS', 'S_KIN', 'S_CYBER', 'S_HOUND', 'S_HYDRA', 'S_DEMON', 'S_UNIQUE', 'S_HI_UNDEAD', 'S_HI_DRAGON', 'ANIM_DEAD']
	description = """Shuma-Gorath is one of the immortal lords of chaos. The true form of 
this blasphemous horror is a huge, all-seeing eye surrounded by tentacles.
"""

monsters.register_race(shuma_gorath)


class tulzscha_the_green_flame(demon, spellcaster):
	r_idx = "842"
	name = "Tulzscha, the Green Flame"
	ascii = "E,G"
	level = 89
	rarity = 4
	hp = "70d100"
	speed = 130
	aaf = 90
	ac = 50
	sleep = 50
	exp = 45000
	blows = [('HIT', 'HURT', '2d50'), ('HIT', 'HURT', '2d50'), ('HIT', 'HURT', '2d50'), ('HIT', 'HURT', '2d50')]
	flags = ['WILD_DUNGEON_01', 'RAND_25', 'EVIL', 'DEMON', 'AURA_FIRE', 'AURA_COLD', 'ELDRITCH_HORROR', 'DROP_2D2', 'DROP_4D2', 'DROP_GOOD', 'ONLY_ITEM', 'RES_TELE', 'IM_POIS', 'IM_FIRE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NONLIVING', 'IM_COLD', 'IM_ACID', 'IM_ELEC', 'RES_NEXU', 'RES_NETH', 'RES_PLAS', 'REGENERATE', 'UNIQUE']
	sfreq = 3
	sflags = ['BR_NEXU', 'BR_NETH', 'BR_COLD', 'BR_FIRE']
	description = """"A belching column of sick greenish flame... spouting volcanically 
from the depths profound and inconceivable, casting no shadows as 
healthy flame should, and coating the nitrous stone with a nasty, 
venomous verdigris. For all that seething combustion no warmth 
lay, but only the clamminess of death and corruption."
"""

monsters.register_race(tulzscha_the_green_flame)


class oremorj_the_cyberdemon_lord(demon, spellcaster):
	r_idx = "843"
	name = "Oremorj the Cyberdemon Lord"
	ascii = "U,u"
	level = 89
	rarity = 4
	hp = "90d70"
	speed = 130
	aaf = 90
	ac = 45
	sleep = 90
	exp = 50000
	blows = [('HIT', 'HURT', '2d50'), ('HIT', 'HURT', '2d50'), ('HIT', 'HURT', '2d50'), ('HIT', 'HURT', '2d50')]
	flags = ['RAND_25', 'EVIL', 'DEMON', 'WILD_DUNGEON_01', 'DROP_2D2', 'DROP_GOOD', 'ONLY_ITEM', 'RES_TELE', 'DROP_GREAT', 'UNIQUE', 'IM_POIS', 'IM_FIRE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'NONLIVING']
	sfreq = 3
	sflags = ['ROCKET', 'S_CYBER']
	description = """The mightiest of Cyberdemons, their lord and ruler.
"""

monsters.register_race(oremorj_the_cyberdemon_lord)


class kaschei_the_immortal(monster, undead, spellcaster):
	r_idx = "844"
	name = "Kaschei the Immortal"
	ascii = "L,v"
	level = 90
	rarity = 3
	hp = "60d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 0
	exp = 45000
	blows = [('HIT', 'UN_BONUS', '6d8'), ('HIT', 'UN_BONUS', '6d8'), ('HIT', 'HURT', '5d5'), ('HIT', 'HURT', '5d5')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'RES_TELE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'ESCORT', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'SMART', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'EVIL', 'UNDEAD', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['TPORT', 'BLIND', 'SCARE', 'CAUSE_4', 'BRAIN_SMASH', 'BA_MANA', 'BO_MANA', 'BA_FIRE', 'S_MONSTERS', 'S_DEMON', 'S_HI_UNDEAD', 'HAND_DOOM', 'ANIM_DEAD']
	description = """A stench of corruption and decay surrounds this sorcerer, who has clearly 
risen from the grave to continue his foul plots and schemes.
"""

monsters.register_race(kaschei_the_immortal)


class yog_sothoth_the_all_in_one(monster, spellcaster):
	r_idx = "845"
	name = "Yog-Sothoth, the All-in-One"
	ascii = "j,v"
	level = 90
	rarity = 3
	hp = "66d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 20
	exp = 45000
	blows = [('TOUCH', 'HURT', '40d5'), ('TOUCH', 'LOSE_CON', '16d2'), ('TOUCH', 'LOSE_CON', '16d2')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'CAN_SPEAK', 'SMART', 'ELDRITCH_HORROR', 'RES_TELE', 'PASS_WALL', 'FORCE_SLEEP', 'FORCE_MAXHP', 'AURA_ELEC', 'AURA_FIRE', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'ATTR_MULTI', 'ATTR_ANY', 'AURA_COLD', 'COLD_BLOOD', 'OPEN_DOOR', 'BASH_DOOR', 'NONLIVING', 'CAN_FLY', 'EVIL', 'IM_ACID', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BO_MANA', 'BRAIN_SMASH', 'BA_MANA', 'S_MONSTERS', 'S_CYBER', 'BA_CHAO', 'S_DEMON', 'S_HI_UNDEAD', 'S_HOUND', 'BR_MANA', 'BR_DISI']
	description = """"Great globes of light massing towards the opening... the breaking 
apart of the nearest globes, and the protoplasmic flesh that 
flowed blackly outward to join together and form that eldritch, 
hideous horror from outer space... whose mask was a congeries 
of iridescent globes... who froths as primal slime in nuclear 
chaos forever beyond the nethermost outposts of space and time!"
"""

monsters.register_race(yog_sothoth_the_all_in_one)


class fenris_wolf(monster, spellcaster):
	r_idx = "846"
	name = "Fenris Wolf"
	ascii = "C,D"
	level = 90
	rarity = 2
	hp = "70d70"
	speed = 134
	aaf = 80
	ac = 45
	sleep = 40
	exp = 45000
	blows = [('BITE', 'HURT', '20d6'), ('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3'), ('BITE', 'HURT', '20d6')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'DROP_CORPSE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'RAND_25', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'KILL_BODY', 'ANIMAL', 'EVIL', 'IM_POIS', 'IM_FIRE', 'IM_COLD']
	sfreq = 8
	sflags = ['BR_DARK', 'BR_POIS', 'BR_COLD']
	description = """The immensely huge wolf who would swallow the sun to satisfy its 
hunger and leave the Norse gods for the dessert.
"""

monsters.register_race(fenris_wolf)


class great_wyrm_of_power(dragon, spellcaster):
	r_idx = "847"
	name = "Great Wyrm of Power"
	ascii = "D,y"
	level = 90
	rarity = 4
	hp = "83d111"
	speed = 130
	aaf = 20
	ac = 55
	sleep = 70
	exp = 47500
	blows = [('CRUSH', 'POISON', '8d12'), ('CRUSH', 'FIRE', '8d12'), ('CRUSH', 'ELEC', '8d12'), ('CRUSH', 'HURT', '10d18')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'REFLECTING', 'WILD_DUNGEON_01', 'AURA_FIRE', 'AURA_ELEC', 'AURA_COLD', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'CAN_FLY', 'DRAGON', 'GOOD', 'DROP_CORPSE', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'RES_NETH', 'RES_NEXU', 'RES_PLAS', 'RES_DISE', 'RES_TELE']
	sfreq = 3
	sflags = ['S_HI_DRAGON', 'S_DRAGON', 'S_KIN', 'BR_NUKE', 'BR_ACID', 'BR_ELEC', 'BR_FIRE', 'BR_COLD', 'BR_POIS', 'BR_NETH', 'BR_LITE', 'BR_DARK', 'BR_CONF', 'BR_SOUN', 'BR_CHAO', 'BR_DISE', 'BR_NEXU', 'BR_TIME', 'BR_INER', 'BR_GRAV', 'BR_SHAR', 'BR_PLAS', 'BR_WALL', 'BR_MANA', 'BR_DISI']
	description = """The mightiest of all dragonkind, a great wyrm of power is seldom 
encountered in our world. It can crush stars with its might.
"""

monsters.register_race(great_wyrm_of_power)


class shub_niggurath_black_goat_of_the_woods(demon, spellcaster):
	r_idx = "848"
	name = "Shub-Niggurath, Black Goat of the Woods"
	ascii = "U,D"
	level = 91
	rarity = 3
	hp = "65d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 20
	exp = 47500
	blows = [('CRUSH', 'LOSE_WIS', '20d5'), ('CRUSH', 'LOSE_INT', '20d5'), ('BITE', 'LOSE_STR', '10d2'), ('BITE', 'LOSE_CON', '10d2')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'CAN_SPEAK', 'FEMALE', 'ELDRITCH_HORROR', 'AURA_ELEC', 'RES_TELE', 'ATTR_MULTI', 'FORCE_SLEEP', 'FORCE_MAXHP', 'PASS_WALL', 'ATTR_ANY', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'DROP_90', 'NONLIVING', 'CAN_FLY', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'DEMON', 'AURA_COLD', 'EVIL', 'IM_ACID', 'IM_COLD', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BO_MANA', 'BRAIN_SMASH', 'BA_DARK', 'S_MONSTERS', 'CAUSE_4', 'HEAL', 'BR_CHAO', 'BR_CONF', 'BR_POIS', 'BR_NUKE', 'BA_CHAO', 'S_DEMON', 'S_HI_UNDEAD', 'S_UNIQUE', 'ANIM_DEAD']
	description = """This horrendous outer god looks like a writhing cloudy mass filled 
with mouths and tentacles.
"""

monsters.register_race(shub_niggurath_black_goat_of_the_woods)


class nodens_lord_of_the_great_abyss(monster, spellcaster):
	r_idx = "849"
	name = "Nodens, Lord of the Great Abyss"
	ascii = "P,w"
	level = 92
	rarity = 3
	hp = "75d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 20
	exp = 48000
	blows = [('HIT', 'HURT', '2d66'), ('HIT', 'HURT', '2d66')]
	flags = ['UNIQUE', 'CAN_SPEAK', 'MALE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'PASS_WALL', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'DROP_90', 'DROP_GREAT', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'CAN_FLY', 'GOOD', 'IM_ACID', 'IM_COLD', 'IM_POIS', 'IM_FIRE', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP', 'RES_NEXU', 'RES_NETH', 'RES_WATE']
	sfreq = 1
	sflags = ['TELE_AWAY', 'TELE_TO', 'TELE_LEVEL', 'TPORT', 'BO_MANA', 'BA_MANA', 'S_MONSTERS', 'HEAL', 'CAUSE_4', 'HASTE', 'HAND_DOOM', 'S_ANGEL', 'S_UNIQUE']
	description = """The hoary Lord of the Great Abyss seems a wizened man, but appearances 
can be deceiving.
"""

monsters.register_race(nodens_lord_of_the_great_abyss)


class carcharoth_the_jaws_of_thirst(monster, spellcaster):
	r_idx = "850"
	name = "Carcharoth, the Jaws of Thirst"
	ascii = "C,D"
	level = 92
	rarity = 2
	hp = "75d70"
	speed = 130
	aaf = 80
	ac = 65
	sleep = 10
	exp = 40000
	blows = [('CLAW', 'HURT', '3d3'), ('CLAW', 'HURT', '3d3'), ('BITE', 'POISON', '4d4'), ('BITE', 'POISON', '4d4')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RAND_25', 'ONLY_ITEM', 'DROP_1D2', 'DROP_GOOD', 'AURA_FIRE', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'ANIMAL', 'EVIL', 'IM_FIRE', 'IM_POIS', 'NO_CONF', 'NO_SLEEP']
	sfreq = 4
	sflags = ['HEAL', 'SCARE', 'BRAIN_SMASH', 'BR_FIRE', 'S_HOUND']
	description = """The first guard of Angband, Carcharoth, also known as 'The Red Maw', is 
the largest wolf to ever walk the earth.  He is highly intelligent and a 
deadly opponent in combat.
"""

monsters.register_race(carcharoth_the_jaws_of_thirst)


class nyarlathotep_the_crawling_chaos(demon, spellcaster):
	r_idx = "851"
	name = "Nyarlathotep, the Crawling Chaos"
	ascii = "U,r"
	level = 93
	rarity = 3
	hp = "90d70"
	speed = 130
	aaf = 100
	ac = 50
	sleep = 20
	exp = 49000
	blows = [('CRUSH', 'LOSE_CON', '30d4'), ('CRUSH', 'LOSE_STR', '30d4'), ('GAZE', 'LOSE_INT', '1d50'), ('GAZE', 'LOSE_WIS', '1d50')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'CAN_SPEAK', 'ELDRITCH_HORROR', 'RES_TELE', 'NONLIVING', 'FORCE_SLEEP', 'FORCE_MAXHP', 'PASS_WALL', 'DEMON', 'RES_NEXU', 'SHAPECHANGER', 'ATTR_MULTI', 'ATTR_ANY', 'MALE', 'AURA_ELEC', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'DROP_90', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'AURA_COLD', 'EVIL', 'IM_ACID', 'IM_POIS', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['TELE_AWAY', 'TELE_TO', 'TELE_LEVEL', 'TPORT', 'BR_NEXU', 'BA_CHAO', 'BA_MANA', 'BA_FIRE', 'S_MONSTERS', 'BRAIN_SMASH', 'MIND_BLAST', 'CAUSE_4', 'HASTE', 'S_HI_UNDEAD', 'S_HI_DRAGON', 'ANIM_DEAD', 'S_ANGEL', 'S_DEMON', 'HEAL', 'S_SPIDER', 'S_HOUND', 'S_CYBER', 'HAND_DOOM']
	description = """Nyarlathothep is the messenger, the heart and the soul of the outer gods. 
He is a shapechanger capable of assuming thousands of nightmarish forms.
One of them looks like thisan antique pharaoh, gay with prismatic robes and crowned with a 
pshent that glowed with inherent light... the fascination of a 
dark god or fallen archangel, and around whose eyes there lurked 
the languid sparkle of capricious humor."
"""

monsters.register_race(nyarlathotep_the_crawling_chaos)


class azathoth_seething_nuclear_chaos(demon, spellcaster):
	r_idx = "852"
	name = "Azathoth, Seething Nuclear Chaos"
	ascii = "E,B"
	level = 93
	rarity = 3
	hp = "70d99"
	speed = 130
	aaf = 100
	ac = 75
	sleep = 100
	exp = 50000
	blows = [('CRUSH', 'HURT', '35d5'), ('CRAWL', 'ACID', '35d5'), ('CRUSH', 'HURT', '35d5'), ('CRAWL', 'ACID', '35d5')]
	flags = ['UNIQUE', 'ELDRITCH_HORROR', 'RES_TELE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'DEMON', 'AURA_FIRE', 'AURA_ELEC', 'ATTR_MULTI', 'ESCORTS', 'ESCORT', 'POWERFUL', 'ATTR_ANY', 'NONLIVING', 'KILL_ITEM', 'KILL_WALL', 'CAN_SWIM', 'AURA_COLD', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'DROP_90', 'RES_DISE', 'STUPID', 'OPEN_DOOR', 'BASH_DOOR', 'REGENERATE', 'EMPTY_MIND', 'EVIL', 'IM_ACID', 'IM_POIS', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['S_DEMON', 'BR_CHAO', 'BR_DISE', 'BR_MANA', 'BA_WATE', 'BR_DISI']
	description = """"That last amorphous blight of nethermost confusion which 
blasphemes and bubbles at the centre of all infinity -- 
the boundless daemon sultan Azathoth, whose name no lips 
dare speak aloud, and who gnaws hungrily in inconceivable, 
unlighted chambers beyond time amidst the muffled, maddening 
beating of vile drums and the thin monotonous whine of 
accursed flutes."
"""

monsters.register_race(azathoth_seething_nuclear_chaos)


class cerberus_guardian_of_hades(monster, spellcaster):
	r_idx = "853"
	name = "Cerberus, Guardian of Hades"
	ascii = "C,r"
	level = 94
	rarity = 1
	hp = "70d100"
	speed = 130
	aaf = 50
	ac = 80
	sleep = 10
	exp = 40000
	blows = [('HIT', 'FIRE', '9d12'), ('HIT', 'FIRE', '9d12'), ('HIT', 'FIRE', '9d12'), ('HIT', 'FIRE', '9d12')]
	flags = ['UNIQUE', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'AURA_FIRE', 'DROP_CORPSE', 'ONLY_ITEM', 'DROP_4D2', 'DROP_GOOD', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'ANIMAL', 'EVIL', 'IM_FIRE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BA_DARK', 'BR_FIRE', 'BR_NETH', 'S_HOUND']
	description = """A three-headed hell hound of fearsome aspect.  Flame burns merrily from its 
hide as it snarls and roars its defiance.
"""

monsters.register_race(cerberus_guardian_of_hades)


class jormungand_the_midgard_serpent(monster, spellcaster):
	r_idx = "854"
	name = "Jormungand the Midgard Serpent"
	ascii = "J,g"
	level = 94
	rarity = 1
	hp = "90d120"
	speed = 130
	aaf = 100
	ac = 100
	sleep = 0
	exp = 45000
	blows = [('CRUSH', 'HURT', '30d10'), ('CRUSH', 'HURT', '30d10'), ('BITE', 'HURT', '5d60')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'FORCE_MAXHP', 'FORCE_SLEEP', 'AQUATIC', 'WILD_OCEAN', 'MOVE_BODY', 'KILL_WALL', 'IM_FIRE', 'IM_POIS', 'IM_ACID', 'IM_COLD', 'EVIL', 'RES_WATE', 'RES_PLAS', 'RES_NEXU', 'NO_STUN', 'REGENERATE', 'DROP_CORPSE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['BA_WATE']
	description = """The Midgard Serpent is so huge that its body surrounds the world of 
mortal men. It can grind gods themselves to lifeless pulp.
"""

monsters.register_race(jormungand_the_midgard_serpent)


class the_destroyer(monster, spellcaster):
	r_idx = "855"
	name = "The Destroyer"
	ascii = "g,W"
	level = 94
	rarity = 1
	hp = "128d170"
	speed = 130
	aaf = 100
	ac = 100
	sleep = 0
	exp = 45000
	blows = [('HIT', 'HURT', '30d9'), ('HIT', 'HURT', '30d9'), ('HIT', 'HURT', '30d9'), ('HIT', 'HURT', '30d9')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'NONLIVING', 'NO_FEAR', 'FORCE_MAXHP', 'FORCE_SLEEP', 'REFLECTING', 'MOVE_BODY', 'KILL_WALL', 'IM_FIRE', 'IM_ELEC', 'IM_POIS', 'IM_ACID', 'IM_COLD', 'RES_NETH', 'RES_WATE', 'RES_PLAS', 'RES_NEXU', 'NO_STUN', 'REGENERATE', 'NO_CONF', 'NO_SLEEP', 'CAN_FLY', 'EMPTY_MIND', 'COLD_BLOOD', 'BASH_DOOR']
	sfreq = 3
	sflags = ['BR_DISI', 'BO_MANA']
	description = """The Destroyer was built by the Norse deities to be their ultimate weapon 
against the space gods who had arrived to judge the world. Unfortunately, 
the Destroyer has gone berserk and is destroying everything it sees. The 
mystical Destroyer is nearly indestructible, and it is said that when it 
uses its power of complete disintegration, the day of Ragnarok is near.
"""

monsters.register_race(the_destroyer)


class gothmog_the_high_captain_of_balrogs(demon, spellcaster):
	r_idx = "856"
	name = "Gothmog, the High Captain of Balrogs"
	ascii = "U,R"
	level = 95
	rarity = 1
	hp = "80d70"
	speed = 130
	aaf = 100
	ac = 70
	sleep = 0
	exp = 43000
	blows = [('HIT', 'FIRE', '9d12'), ('HIT', 'FIRE', '9d12'), ('CRUSH', 'HURT', '8d12'), ('TOUCH', 'UN_POWER', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'CAN_FLY', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ESCORTS', 'KILL_WALL', 'AURA_FIRE', 'NONLIVING', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'EVIL', 'DEMON', 'IM_FIRE', 'IM_ELEC', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['BLIND', 'CONF', 'SCARE', 'BR_FIRE', 'S_KIN', 'S_DEMON', 'S_HI_UNDEAD', 'S_CYBER']
	description = """Gothmog is the Chief Balrog in Morgoth's personal guard.  He is renowned 
for slaying Ecthelion the Warder of the Gates and he has never been 
defeated in combat.  With his whip of flame and awesome fiery breath he 
saved his master from Ungoliant's rage.
"""

monsters.register_race(gothmog_the_high_captain_of_balrogs)


class great_cthulhu(demon, spellcaster):
	r_idx = "857"
	name = "Great Cthulhu"
	ascii = "U,g"
	level = 96
	rarity = 2
	hp = "70d100"
	speed = 130
	aaf = 100
	ac = 70
	sleep = 100
	exp = 62500
	blows = [('CRUSH', 'HURT', '50d4'), ('CLAW', 'UN_POWER', '15d2'), ('CLAW', 'UN_BONUS', '15d2'), ('TOUCH', 'DISEASE', '100d1')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'CAN_SPEAK', 'DEMON', 'ELDRITCH_HORROR', 'NONLIVING', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ESCORT', 'ESCORTS', 'SMART', 'RES_PLAS', 'RES_NEXU', 'OPEN_DOOR', 'BASH_DOOR', 'POWERFUL', 'MOVE_BODY', 'REGENERATE', 'RES_NETH', 'ONLY_ITEM', 'DROP_2D2', 'DROP_4D2', 'DROP_1D2', 'DROP_GOOD', 'DROP_GREAT', 'RES_DISE', 'RES_TELE', 'CAN_SWIM', 'EVIL', 'IM_FIRE', 'IM_POIS', 'IM_ELEC', 'IM_ACID', 'NO_CONF', 'NO_SLEEP']
	sfreq = 3
	sflags = ['TPORT', 'SCARE', 'BLIND', 'MIND_BLAST', 'BRAIN_SMASH', 'DRAIN_MANA', 'BR_POIS', 'BR_ACID', 'BR_FIRE', 'CONF', 'DARKNESS', 'FORGET', 'S_HI_UNDEAD', 'BR_NUKE', 'BR_NETH', 'BR_CHAO', 'BR_DISE', 'BR_DARK', 'BR_PLAS', 'BR_CONF', 'BR_NEXU', 'S_DEMON', 'S_CYBER', 'S_KIN', 'BR_DISI', 'HAND_DOOM', 'ANIM_DEAD']
	description = """This creature is death incarnate. "A monster of vaguely anthropoid 
outline, but with an octopus-like head whose face was a mass of 
feelers, a scaly, rubbery-looking body, prodigious claws on hind 
fore feet, and long, narrow wings behind. This thing... was a 
somewhat bloated corpulence... It lumbered slobberingly into sight 
and gropingly squeezed its gelatinous green immensity through the 
black doorway... A mountain shambled or walked."
"""

monsters.register_race(great_cthulhu)


class sauron_the_sorcerer(monster, spellcaster):
	r_idx = "858"
	name = "Sauron, the Sorcerer"
	ascii = "p,o"
	level = 98
	rarity = 1
	hp = "105d70"
	speed = 130
	aaf = 100
	ac = 80
	sleep = 0
	exp = 50000
	blows = [('HIT', 'UN_BONUS', '10d12'), ('HIT', 'UN_BONUS', '10d12'), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'UN_POWER', '')]
	flags = ['UNIQUE', 'MALE', 'CAN_SPEAK', 'REFLECTING', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'DROP_CHOSEN', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'REGENERATE', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'RES_TELE']
	sfreq = 2
	sflags = ['TPORT', 'TELE_LEVEL', 'BLIND', 'CONF', 'SCARE', 'CAUSE_4', 'BRAIN_SMASH', 'FORGET', 'BO_ICEE', 'BO_MANA', 'BO_PLAS', 'BA_MANA', 'BA_FIRE', 'BA_WATE', 'BA_NETH', 'BA_DARK', 'S_MONSTERS', 'S_DEMON', 'S_HI_UNDEAD', 'S_HI_DRAGON', 'HAND_DOOM', 'ANIM_DEAD']
	description = """Mighty in spells and enchantments,
he created the One Ring.  His eyes glow with power and his gaze seeks to 
destroy your soul.  He has many servants, and rarely fights without them.
"""

monsters.register_race(sauron_the_sorcerer)


class the_unicorn_of_order(monster, spellcaster):
	r_idx = "859"
	name = "The Unicorn of Order"
	ascii = "q,w"
	level = 99
	rarity = 1
	hp = "200d70"
	speed = 160
	aaf = 100
	ac = 85
	sleep = 0
	exp = 65000
	blows = [('KICK', 'UN_POWER', '13d13'), ('KICK', 'UN_POWER', '12d12'), ('BUTT', 'UN_BONUS', '11d11'), ('BITE', 'TIME', '10d10')]
	flags = ['UNIQUE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'REFLECTING', 'AURA_COLD', 'AURA_ELEC', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'DROP_3D2', 'DROP_GOOD', 'DROP_GREAT', 'DROP_CHOSEN', 'RES_NETH', 'RES_WATE', 'RES_PLAS', 'RES_NEXU', 'RES_DISE', 'SMART', 'KILL_BODY', 'POWERFUL', 'REGENERATE', 'NONLIVING', 'CAN_FLY', 'GOOD', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NO_STUN', 'RES_TELE']
	sfreq = 3
	sflags = ['SHRIEK', 'BR_SOUN', 'BR_LITE', 'BR_INER', 'BR_TIME', 'BR_SHAR', 'BR_PLAS', 'BR_MANA', 'BR_DISI', 'DRAIN_MANA', 'HEAL', 'HASTE', 'TELE_AWAY', 'BLINK', 'TPORT', 'FORGET', 'S_MONSTERS', 'S_HI_DRAGON', 'S_AMBERITES', 'S_CYBER', 'S_UNIQUE']
	description = """The Unicorn of Order, who once stole an eye from the great Serpent 
of chaos, is watching the events unfold with growing distress. It 
regards you as a mortal meddling in the affairs of immortals, and 
attempts to eliminate you.
"""

monsters.register_race(the_unicorn_of_order)


class oberon_king_of_amber(amberite, spellcaster):
	r_idx = "860"
	name = "Oberon, King of Amber"
	ascii = "p,v"
	level = 99
	rarity = 1
	hp = "70d111"
	speed = 145
	aaf = 100
	ac = 82
	sleep = 0
	exp = 65000
	blows = [('HIT', 'UN_BONUS', '12d12'), ('HIT', 'UN_POWER', '12d12'), ('HIT', 'CONFUSE', '10d2'), ('HIT', 'BLIND', '3d2')]
	flags = ['UNIQUE', 'QUESTOR', 'MALE', 'CAN_SPEAK', 'DROP_CORPSE', 'WILD_DUNGEON_01', 'ATTR_MULTI', 'AMBERITE', 'RES_TELE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'FORCE_DEPTH', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'DROP_CHOSEN', 'REFLECTING', 'AURA_FIRE', 'AURA_ELEC', 'SMART', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'REGENERATE', 'EVIL', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'POWERFUL', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['TPORT', 'SCARE', 'BLIND', 'S_DEMON', 'S_ANGEL', 'S_MONSTERS', 'TELE_TO', 'CONF', 'BO_MANA', 'BA_FIRE', 'BRAIN_SMASH', 'BA_NETH', 'BO_ICEE', 'CAUSE_4', 'BA_WATE', 'BO_PLAS', 'TELE_LEVEL', 'TELE_AWAY', 'FORGET', 'DARKNESS', 'BA_MANA', 'S_HI_DRAGON', 'S_HI_UNDEAD', 'BA_CHAO', 'S_AMBERITES', 'S_CYBER', 'HAND_DOOM']
	description = """Oberon, the father or grandfather of most Amberites, is as powerful 
as ever. He is afraid that you will upset the balance and will not 
let you harm the Serpent of Chaos. "Oberon, Lord of Amber, stood before 
me in his green and his gold. High, wide and thick, his beard black and 
shot with silver, his hair the same. Green rings in gold settings and 
a blade of golden color."
"""

monsters.register_race(oberon_king_of_amber)


class morgoth_lord_of_darkness(monster, spellcaster):
	r_idx = "861"
	name = "Morgoth, Lord of Darkness"
	ascii = "P,D"
	level = 100
	rarity = 1
	hp = "140d100"
	speed = 140
	aaf = 100
	ac = 75
	sleep = 0
	exp = 60000
	blows = [('HIT', 'SHATTER', '20d10'), ('HIT', 'SHATTER', '20d10'), ('HIT', 'LOSE_ALL', '10d12'), ('TOUCH', 'UN_POWER', '')]
	flags = ['UNIQUE', 'MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'FORCE_DEPTH', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'SMART', 'KILL_WALL', 'MOVE_BODY', 'AURA_COLD', 'REGENERATE', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 3
	sflags = ['BRAIN_SMASH', 'BA_MANA', 'BO_MANA', 'BA_NETH', 'ANIM_DEAD', 'S_MONSTERS', 'S_UNIQUE', 'S_AMBERITES', 'S_HI_UNDEAD', 'S_HI_DRAGON']
	description = """He is the Master of the Pits of Angband.  His figure is like a black 
mountain crowned with Lightning.  He rages with everlasting anger, his 
body scarred by Fingolfin's eight mighty wounds.  He can never rest from 
his pain, but seeks forever to dominate all that is light and good in the 
world.  He is the origin of man's fear of darkness and created many foul 
creatures with his evil powers.  Orcs, Dragons, and Trolls are his most 
foul corruptions, causing much pain and suffering in the world to please 
him.  His disgusting visage, twisted with evil, is crowned with iron, the 
two remaining Silmarils forever burning him.  Grond, the mighty Hammer of 
the Underworld, cries defiance as he strides towards you to crush you to a 
pulp!
"""

monsters.register_race(morgoth_lord_of_darkness)


class the_serpent_of_chaos(monster, spellcaster):
	r_idx = "862"
	name = "The Serpent of Chaos"
	ascii = "J,D"
	level = 100
	rarity = 1
	hp = "140d150"
	speed = 155
	aaf = 111
	ac = 87
	sleep = 0
	exp = 66666
	blows = [('CRUSH', 'SHATTER', '22d10'), ('CRUSH', 'SHATTER', '22d10'), ('BITE', 'LOSE_ALL', '10d12'), ('TOUCH', 'UN_POWER', '')]
	flags = ['UNIQUE', 'QUESTOR', 'CAN_SPEAK', 'ATTR_MULTI', 'ATTR_ANY', 'WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'FORCE_DEPTH', 'REFLECTING', 'AURA_FIRE', 'AURA_ELEC', 'AURA_COLD', 'ONLY_ITEM', 'DROP_1D2', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'DROP_GREAT', 'DROP_CHOSEN', 'RES_NETH', 'SMART', 'KILL_WALL', 'KILL_BODY', 'POWERFUL', 'REGENERATE', 'NONLIVING', 'CAN_FLY', 'EVIL', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NO_STUN', 'RES_TELE']
	sfreq = 3
	sflags = ['S_MONSTERS', 'BR_CHAO', 'BA_CHAO', 'ROCKET', 'BRAIN_SMASH', 'S_CYBER', 'BR_NETH', 'HASTE', 'BR_MANA', 'S_HI_UNDEAD', 'S_HI_DRAGON', 'S_UNIQUE', 'S_AMBERITES', 'BR_NUKE', 'BR_POIS', 'BR_DISI', 'HAND_DOOM']
	description = """The Big Boss himself. Before the universe the Serpent was. The Unicorn 
of Order fought with Serpent and stole one of its eyes, known as the 
Jewel of Judgement. With the Jewel, Dworkin drew the Pattern and thus 
gave birth to the infinite worlds of shadow. Now the balance has been 
disrupted, the Pattern damaged, and all the shadow is being absorbed by 
the Serpent. Unless it is stopped, the word as we know it will come to 
an end, all order reverting to Primal Chaos.
"""

monsters.register_race(the_serpent_of_chaos)


class quantum_dot(monster, spellcaster):
	r_idx = "863"
	name = "Quantum dot"
	ascii = ",,v"
	level = 35
	rarity = 5
	hp = "3d2"
	speed = 110
	aaf = 20
	ac = 50
	sleep = 0
	exp = 50
	blows = [('SPORE', 'LOSE_ALL', '2d4'), ('SPORE', 'UN_BONUS', '2d4'), ('SPORE', 'CONFUSE', '2d4'), ('SPORE', 'TERRIFY', '2d4')]
	flags = ['NEVER_MOVE', 'WILD_DUNGEON_01', 'STUPID', 'WEIRD_MIND', 'ATTR_MULTI', 'ATTR_ANY', 'QUANTUM', 'ELDRITCH_HORROR', 'MULTIPLY', 'FRIENDS', 'IM_ACID', 'IM_FIRE', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'DROP_CORPSE', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR', 'NO_STUN']
	sfreq = 2
	sflags = ['BLINK', 'MIND_BLAST']
	description = """It is there and it is not. 
"""

monsters.register_race(quantum_dot)


class a_plain_gold_ring(demon, spellcaster):
	r_idx = "864"
	name = "a Plain Gold Ring"
	ascii = "=,y"
	level = 110
	rarity = 3
	hp = "160d70"
	speed = 140
	aaf = 100
	ac = 100
	sleep = 10
	exp = 5000000
	blows = [('TOUCH', 'BLIND', '5d5'), ('TOUCH', 'UN_BONUS', '10d10'), ('TOUCH', 'LOSE_ALL', '10d10'), ('TOUCH', 'EXP_80', '10d10')]
	flags = ['WILD_DUNGEON_01', 'UNIQUE', 'FORCE_SLEEP', 'NEVER_MOVE', 'ONLY_ITEM', 'DROP_90', 'DROP_GOOD', 'DROP_GREAT', 'REFLECTING', 'INVISIBLE', 'COLD_BLOOD', 'SMART', 'EMPTY_MIND', 'REGENERATE', 'POWERFUL', 'AURA_FIRE', 'EVIL', 'DEMON', 'NONLIVING', 'IM_ACID', 'IM_ELEC', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'RES_TELE', 'RES_NETH', 'RES_WATE', 'RES_PLAS', 'RES_NEXU', 'RES_DISE', 'NO_FEAR', 'NO_CONF', 'NO_STUN', 'NO_SLEEP']
	sfreq = 2
	sflags = ['BR_NETH', 'BR_TIME', 'BA_MANA', 'BO_MANA', 'HAND_DOOM', 'HEAL', 'S_DEMON', 'S_HI_UNDEAD', 'S_HI_DRAGON']
	description = """It is a ring of immense power.  You can hardly believe you've 
finally found the One Ring!
"""

monsters.register_race(a_plain_gold_ring)


class lourph(monster):
	r_idx = "865"
	name = "Lourph"
	ascii = "S,u"
	level = 79
	rarity = 7
	hp = "17d7"
	speed = 140
	aaf = 200
	ac = 55
	sleep = 0
	exp = 3333
	blows = [('BITE', 'PARALYZE', '5d5'), ('BITE', 'PARALYZE', '5d5'), ('BITE', 'POISON', '7d5'), ('BITE', 'LOSE_ALL', '8d5')]
	flags = ['FRIEND', 'FRIENDS', 'WILD_DUNGEON_01', 'COLD_BLOOD', 'EMPTY_MIND', 'REGENERATE', 'BASH_DOOR', 'KILL_BODY', 'KILL_ITEM', 'ANIMAL', 'HURT_LITE', 'IM_ACID', 'IM_ELEC', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'RES_TELE', 'RES_NETH', 'RES_DISE', 'NO_FEAR', 'NO_CONF', 'NO_STUN', 'NO_SLEEP']
	sfreq = 0
	sflags = []
	description = """Small brown spiders that lurk in the darkness deep underground.
  An individual would be nearly harmless, but their size allows many
 to crawl where only a single Mirkwood spider could stand.  Packs of
 these arachnids may scour the depths untiringly, and their hunger is
 never satisfied.
"""

monsters.register_race(lourph)


class caaws(demon, spellcaster):
	r_idx = "866"
	name = "Caaws"
	ascii = "E,D"
	level = 90
	rarity = 8
	hp = "15d173"
	speed = 120
	aaf = 100
	ac = 88
	sleep = 0
	exp = 70000
	blows = [('TOUCH', 'UN_POWER', '13d13'), ('TOUCH', 'UN_BONUS', '13d13'), ('TOUCH', 'LOSE_ALL', '13d13'), ('TOUCH', 'CONFUSE', '13d13')]
	flags = ['FORCE_MAXHP', 'NEVER_MOVE', 'STUPID', 'CAN_SWIM', 'CAN_FLY', 'NONLIVING', 'COLD_BLOOD', 'EMPTY_MIND', 'REGENERATE', 'KILL_BODY', 'AURA_COLD', 'EVIL', 'DEMON', 'IM_ACID', 'IM_ELEC', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'RES_TELE', 'RES_NETH', 'RES_WATE', 'RES_PLAS', 'RES_NEXU', 'RES_DISE', 'NO_FEAR', 'NO_CONF', 'NO_STUN', 'NO_SLEEP', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_SWAMP1']
	sfreq = 5
	sflags = ['HEAL', 'INVULNER']
	description = """A thing like a massive, jet black oyster with four groping tendrils.
  Born of nether, it need not chase its prey, content to seeth
 in the darkness until some unfortunate creature wanders too close.
"""

monsters.register_race(caaws)


class culverin(monster, spellcaster):
	r_idx = "867"
	name = "Culverin"
	ascii = "I,U"
	level = 8
	rarity = 2
	hp = "6d5"
	speed = 110
	aaf = 20
	ac = 7
	sleep = 40
	exp = 25
	blows = []
	flags = ['NEVER_BLOW', 'RAND_25', 'ANIMAL', 'WILD_FOREST1', 'WILD_DUNGEON_01']
	sfreq = 1
	sflags = ['ARROW_3']
	description = """A many legged scuttling thing, it spits pebbles with deadly force.
"""

monsters.register_race(culverin)


class behinder(monster):
	r_idx = "868"
	name = "Behinder"
	ascii = ".,d"
	level = 30
	rarity = 100
	hp = "23d20"
	speed = 125
	aaf = 10
	ac = 22
	sleep = 80
	exp = 900
	blows = [('TOUCH', 'TERRIFY', '5d5'), ('BITE', 'HURT', '8d4')]
	flags = ['CHAR_CLEAR', 'CHAR_MULTI', 'ATTR_CLEAR', 'FORCE_SLEEP', 'SMART', 'INVISIBLE', 'COLD_BLOOD', 'EMPTY_MIND', 'ELDRITCH_HORROR', 'OPEN_DOOR', 'ANIMAL', 'NO_FEAR', 'NO_STUN', 'NO_CONF', 'NO_SLEEP', 'CAN_SWIM', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'WILD_FOREST2', 'WILD_MOUNT1']
	sfreq = 0
	sflags = []
	description = """"The Behinder flung itself on his shoulders. Then I knew why
 nobody's supposed to see one. I wish I hadn't. To this day I
 can see it, plain as a fence at noon, and forever I will be
 able to see it."
"""

monsters.register_race(behinder)


class boadile(monster):
	r_idx = "869"
	name = "Boadile"
	ascii = "S,g"
	level = 25
	rarity = 40
	hp = "30d7"
	speed = 120
	aaf = 20
	ac = 15
	sleep = 30
	exp = 300
	blows = [('BITE', 'HURT', '2d5'), ('CRUSH', 'HURT', '2d8'), ('CRUSH', 'HURT', '2d8')]
	flags = ['FORCE_MAXHP', 'STUPID', 'COLD_BLOOD', 'BASH_DOOR', 'ANIMAL', 'RES_WATE', 'CAN_SWIM', 'WILD_SHORE', 'WILD_DUNGEON_01', 'WILD_OCEAN']
	sfreq = 0
	sflags = []
	description = """"...Head something like a croc's, only bigger. Around forty
 feet long. Able to roll itself into a big beachball with teeth.
  Fast on land or in water - and a hell of a lot of little legs
 on each side."
"""

monsters.register_race(boadile)


class ebony_monk(monster, spellcaster):
	r_idx = "870"
	name = "Ebony monk"
	ascii = "p,D"
	level = 36
	rarity = 3
	hp = "48d11"
	speed = 120
	aaf = 20
	ac = 28
	sleep = 5
	exp = 1100
	blows = [('KICK', 'HURT', '7d1'), ('HIT', 'EXP_10', '7d2'), ('KICK', 'HURT', '7d4'), ('HIT', 'EXP_10', '7d2')]
	flags = ['MALE', 'DROP_60', 'DROP_1D2', 'OPEN_DOOR', 'BASH_DOOR', 'IM_COLD', 'IM_POIS', 'RES_NETH', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'WILD_DUNGEON_01', 'DROP_CORPSE', 'DROP_SKELETON']
	sfreq = 9
	sflags = ['BO_NETH']
	description = """A monk in dusky robes trained in the most lethal martial arts.
"""

monsters.register_race(ebony_monk)


class unstable_worm_mass(monster):
	r_idx = "871"
	name = "Unstable worm mass"
	ascii = "w,o"
	level = 15
	rarity = 4
	hp = "3d8"
	speed = 100
	aaf = 10
	ac = 5
	sleep = 5
	exp = 7
	blows = [('EXPLODE', 'FIRE', '5d5')]
	flags = ['RAND_50', 'RAND_25', 'CAN_SWIM', 'WILD_DUNGEON_01', 'STUPID', 'WEIRD_MIND', 'MULTIPLY', 'BASH_DOOR', 'ANIMAL', 'HURT_LITE', 'NO_FEAR']
	sfreq = 0
	sflags = []
	description = """A seething mass of thick, glowing worms.
"""

monsters.register_race(unstable_worm_mass)


class auto_mobile(monster):
	r_idx = "872"
	name = "Auto-mobile"
	ascii = "g,R"
	level = 45
	rarity = 5
	hp = "80d10"
	speed = 130
	aaf = 10
	ac = 110
	sleep = 12
	exp = 1400
	blows = [('BUTT', 'HURT', '8d3'), ('CRUSH', 'HURT', '8d7')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FORCE_MAXHP', 'RES_TELE', 'KILL_BODY', 'KILL_ITEM', 'EMPTY_MIND', 'BASH_DOOR', 'IM_COLD', 'IM_ELEC', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'WILD_WASTE1', 'WILD_WASTE2']
	sfreq = 0
	sflags = []
	description = """It is a streamlined metal demon, moving under its own power to crush you.
  It rumbles hungrily.
"""

monsters.register_race(auto_mobile)


class shallow_puddle(monster):
	r_idx = "873"
	name = "Shallow puddle"
	ascii = "~,B"
	level = 11
	rarity = 6
	hp = "9d6"
	speed = 115
	aaf = 10
	ac = 17
	sleep = 0
	exp = 70
	blows = [('TOUCH', 'ACID', '2d3'), ('TOUCH', 'ACID', '2d3')]
	flags = ['NEVER_MOVE', 'COLD_BLOOD', 'WILD_DUNGEON_01', 'WILD_FOREST1', 'CAN_SWIM', 'IM_ELEC', 'IM_ACID', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'EMPTY_MIND']
	sfreq = 0
	sflags = []
	description = """It is a placid watery pool.
"""

monsters.register_race(shallow_puddle)


class deep_puddle(monster):
	r_idx = "874"
	name = "Deep puddle"
	ascii = "~,b"
	level = 23
	rarity = 6
	hp = "17d6"
	speed = 125
	aaf = 15
	ac = 40
	sleep = 0
	exp = 70
	blows = [('TOUCH', 'ACID', '4d3'), ('TOUCH', 'ACID', '3d3')]
	flags = ['NEVER_MOVE', 'COLD_BLOOD', 'WILD_DUNGEON_01', 'WILD_FOREST2', 'CAN_SWIM', 'IM_ELEC', 'IM_ACID', 'IM_POIS', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NONLIVING', 'EMPTY_MIND']
	sfreq = 0
	sflags = []
	description = """It is a large, rippling watery pool.
"""

monsters.register_race(deep_puddle)


class noxious_fume(monster):
	r_idx = "875"
	name = "Noxious fume"
	ascii = "#,y"
	level = 20
	rarity = 4
	hp = "3d3"
	speed = 90
	aaf = 20
	ac = 37
	sleep = 30
	exp = 30
	blows = [('TOUCH', 'POISON', '3d3')]
	flags = ['RAND_25', 'RAND_50', 'INVISIBLE', 'MULTIPLY', 'WILD_DUNGEON_01', 'CAN_FLY', 'CAN_SWIM', 'PASS_WALL', 'KILL_BODY', 'IM_FIRE', 'IM_COLD', 'IM_ACID', 'IM_ELEC', 'IM_POIS', 'NONLIVING', 'EMPTY_MIND', 'COLD_BLOOD', 'WILD_MOUNT2', 'WILD_WASTE1', 'WILD_WASTE2', 'WILD_SWAMP1', 'WILD_SWAMP2', 'NO_FEAR', 'NO_CONF', 'NO_SLEEP', 'NO_STUN']
	sfreq = 0
	sflags = []
	description = """It is a swirling cloud of yellowish gas.
"""

monsters.register_race(noxious_fume)


class pattern_vortex(monster, spellcaster):
	r_idx = "876"
	name = "Pattern vortex"
	ascii = "v,R"
	level = 50
	rarity = 100
	hp = "100d100"
	speed = 110
	aaf = 20
	ac = 200
	sleep = 1
	exp = 70000
	blows = [('EXPLODE', 'LOSE_ALL', '50d50')]
	flags = ['RAND_25', 'EMPTY_MIND', 'REFLECTING', 'FORCE_MAXHP', 'FORCE_SLEEP', 'REGENERATE', 'AURA_ELEC', 'BASH_DOOR', 'KILL_BODY', 'NONLIVING', 'IM_ACID', 'IM_ELEC', 'IM_FIRE', 'IM_COLD', 'IM_POIS', 'RES_TELE', 'RES_NETH', 'RES_WATE', 'RES_PLAS', 'RES_DISE', 'NO_FEAR', 'NO_STUN', 'NO_CONF', 'NO_SLEEP', 'CAN_FLY']
	sfreq = 10
	sflags = ['HOLD']
	description = """A swirling, rotating red disk.  A funnel thrusts out, seeking the
 one who disturbed the Pattern.
"""

monsters.register_race(pattern_vortex)


class storm_troll(troll, spellcaster):
	r_idx = "877"
	name = "Storm Troll"
	ascii = "T,B"
	level = 51
	rarity = 2
	hp = "40d50"
	speed = 120
	aaf = 20
	ac = 55
	sleep = 100
	exp = 3500
	blows = [('HIT', 'ELEC', '3d10'), ('BITE', 'HURT', '1d22'), ('HIT', 'COLD', '3d10'), ('BITE', 'HURT', '1d22')]
	flags = ['FORCE_SLEEP', 'FORCE_MAXHP', 'WILD_DUNGEON_01', 'WILD_WASTE1', 'WILD_WASTE2', 'DROP_3D2', 'SMART', 'TAKE_ITEM', 'OPEN_DOOR', 'BASH_DOOR', 'MOVE_BODY', 'POWERFUL', 'OPEN_DOOR', 'BASH_DOOR', 'IM_ACID', 'IM_COLD', 'IM_ELEC', 'TROLL', 'EVIL']
	sfreq = 7
	sflags = ['BA_COLD', 'BO_ICEE', 'BO_WATE', 'BA_ELEC']
	description = """The dungeon shakes as an enormous troll-form thunders into view, hurling 
ice and lightning in every direction.  It is thirty feet tall, has six 
heads, and is enraged at your impudence.
"""

monsters.register_race(storm_troll)


class bat_of_gorgoroth(monster, spellcaster):
	r_idx = "878"
	name = "Bat of Gorgoroth"
	ascii = "b,g"
	level = 28
	rarity = 3
	hp = "20d10"
	speed = 120
	aaf = 20
	ac = 30
	sleep = 30
	exp = 200
	blows = [('BITE', 'POISON', '1d10'), ('CLAW', 'HURT', '1d4')]
	flags = ['DROP_60', 'RAND_25', 'MOVE_BODY', 'CAN_FLY', 'WILD_DUNGEON_01', 'WILD_WASTE2', 'BASH_DOOR', 'MOVE_BODY', 'FRIENDS', 'ANIMAL', 'IM_POIS']
	sfreq = 8
	sflags = ['SCARE', 'BR_POIS', 'BR_DARK']
	description = """Fed with horrid meats and grown to enormous size, this slavering creature 
seeks livelier prey.
"""

monsters.register_race(bat_of_gorgoroth)


class spectral_wyrm(dragon, undead, spellcaster):
	r_idx = "879"
	name = "Spectral Wyrm"
	ascii = "D,w"
	level = 72
	rarity = 4
	hp = "60d70"
	speed = 130
	aaf = 40
	ac = 100
	sleep = 255
	exp = 35000
	blows = [('CLAW', 'HURT', '10d12'), ('CLAW', 'HURT', '10d12'), ('CLAW', 'HURT', '10d12'), ('BITE', 'UN_BONUS', '8d8')]
	flags = ['FORCE_MAXHP', 'EVIL', 'UNDEAD', 'CAN_FLY', 'WILD_DUNGEON_01', 'ONLY_ITEM', 'DROP_2D2', 'DROP_3D2', 'DROP_4D2', 'DROP_GOOD', 'POWERFUL', 'MOVE_BODY', 'PASS_WALL', 'INVISIBLE', 'DRAGON', 'NO_CONF', 'NO_SLEEP', 'RES_TELE', 'RES_NETH', 'RES_NEXU', 'RES_DISE']
	sfreq = 3
	sflags = ['BR_NEXU', 'BR_NETH', 'BR_DISE', 'S_HI_DRAGON', 'SCARE', 'BLINK', 'SHRIEK']
	description = """The tortured soul of a once powerful wyrm.
"""

monsters.register_race(spectral_wyrm)


class creeping_dust(monster, spellcaster):
	r_idx = "880"
	name = "Creeping dust"
	ascii = ".,v"
	level = 75
	rarity = 6
	hp = "2d6"
	speed = 120
	aaf = 30
	ac = 30
	sleep = 100
	exp = 800
	blows = [('HIT', 'CONFUSE', '3d4')]
	flags = ['CHAR_MULTI', 'ATTR_MULTI', 'ATTR_ANY', 'WILD_DUNGEON_01', 'RAND_50', 'RAND_25', 'FRIENDS', 'FORCE_SLEEP', 'MULTIPLY', 'HURT_ROCK', 'EMPTY_MIND', 'COLD_BLOOD', 'NO_CONF', 'NO_SLEEP', 'NO_FEAR']
	sfreq = 2
	sflags = ['BLINK', 'TPORT', 'BO_MANA', 'S_MONSTER', 'S_DEMON', 'S_HI_UNDEAD']
	description = """This is what happens to a mage when a powerful spell fails.
"""

monsters.register_race(creeping_dust)


class dark_elven_shade(monster, undead, spellcaster):
	r_idx = "881"
	name = "Dark elven shade"
	ascii = "h,w"
	level = 51
	rarity = 2
	hp = "48d12"
	speed = 130
	aaf = 20
	ac = 25
	sleep = 10
	exp = 6000
	blows = [('TOUCH', 'EXP_VAMP', '1d16'), ('TOUCH', 'UN_POWER', ''), ('TOUCH', 'LOSE_DEX', '8d8'), ('TOUCH', 'LOSE_INT', '8d8')]
	flags = ['MALE', 'FORCE_SLEEP', 'FORCE_MAXHP', 'ONLY_ITEM', 'DROP_90', 'DROP_4D2', 'WILD_DUNGEON_01', 'SMART', 'INVISIBLE', 'COLD_BLOOD', 'UNDEAD', 'EVIL', 'HURT_LITE', 'NO_CONF', 'NO_SLEEP']
	sfreq = 2
	sflags = ['HEAL', 'BLINK', 'BLIND', 'CONF', 'CAUSE_3', 'DARKNESS', 'ANIM_DEAD', 'SHRIEK', 'TELE_AWAY', 'BA_DARK', 'DRAIN_MANA', 'S_MONSTER', 'S_UNDEAD', 'S_DEMON', 'S_KIN']
	description = """A dark elven figure that glows with an inner light.
"""

monsters.register_race(dark_elven_shade)


class anti_magic_hound(monster, spellcaster):
	r_idx = "882"
	name = "Anti-magic hound"
	ascii = "Z,v"
	level = 35
	rarity = 4
	hp = "36d10"
	speed = 110
	aaf = 30
	ac = 30
	sleep = 0
	exp = 550
	blows = [('CLAW', 'HURT', '3d3'), ('BITE', 'UN_BONUS', '3d12'), ('CLAW', 'HURT', '3d3'), ('BITE', 'UN_BONUS', '3d12')]
	flags = ['WILD_DUNGEON_01', 'FORCE_SLEEP', 'FRIENDS', 'DROP_SKELETON', 'DROP_CORPSE', 'BASH_DOOR', 'RES_DISE', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 5
	sflags = ['BR_DISE']
	description = """A field of static energy surrounds this hound. Its presence seems 
to dampen all other energies...
"""

monsters.register_race(anti_magic_hound)


class mana_hound(monster, spellcaster):
	r_idx = "883"
	name = "Mana hound"
	ascii = "Z,B"
	level = 51
	rarity = 4
	hp = "60d10"
	speed = 130
	aaf = 30
	ac = 100
	sleep = 0
	exp = 4000
	blows = [('BITE', 'HURT', '25d1'), ('BITE', 'HURT', '25d1'), ('BITE', 'HURT', '25d1'), ('CLAW', 'HURT', '3d3')]
	flags = ['FORCE_SLEEP', 'FRIENDS', 'WILD_DUNGEON_01', 'OPEN_DOOR', 'BASH_DOOR', 'DROP_SKELETON', 'DROP_CORPSE', 'ANIMAL', 'NO_CONF', 'NO_SLEEP']
	sfreq = 8
	sflags = ['BR_MANA']
	description = """A powerful aura radiates from this canine creature. It feels 
like... magic?
"""

monsters.register_race(mana_hound)


class vore(monster, spellcaster):
	r_idx = "884"
	name = "Vore"
	ascii = "S,v"
	level = 38
	rarity = 2
	hp = "35d10"
	speed = 125
	aaf = 20
	ac = 30
	sleep = 40
	exp = 850
	blows = [('CLAW', 'HURT', '4d8'), ('CLAW', 'HURT', '8d4')]
	flags = ['WILD_DUNGEON_01', 'DROP_90', 'DROP_2D2', 'DROP_SKELETON', 'DROP_CORPSE', 'OPEN_DOOR', 'BASH_DOOR', 'IM_POIS', 'IM_ACID', 'IM_COLD', 'EVIL']
	sfreq = 5
	sflags = ['BO_MANA']
	description = """A gaunt, vaguely humanoid torso merged with the form of a giant 
spider, ugly as sin. It screams horribly as it charges at you.
"""

monsters.register_race(vore)


