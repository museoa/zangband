from variable import worlds
from worlds.nexus.quests.q1 import thieves_quest
worlds["Nexus"].get_level("Town1").features["Mayor"].plot.add_quest(thieves_quest())

