from variable import worlds
from worlds.nexus.quests.slime import slime_mold_quest
worlds["Nexus"].get_level("Town1").features["Mayor"].plot.add_quest(slime_mold_quest())

