# Make the classes known

from data.player.pclass import chaos_wa
from data.player.pclass import highmage
from data.player.pclass import mage
from data.player.pclass import mindcraf
from data.player.pclass import monk
from data.player.pclass import paladin
from data.player.pclass import priest
from data.player.pclass import ranger
from data.player.pclass import rogue
from data.player.pclass import war_mage
from data.player.pclass import warrior

from base.birth import player_class_birth

player_class_birth.classes.append(chaos_wa.chaos_warrior_class)
player_class_birth.classes.append(highmage.highmage_class)
player_class_birth.classes.append(mage.mage_class)
player_class_birth.classes.append(mindcraf.mindcrafter_class)
player_class_birth.classes.append(monk.monk_class)
player_class_birth.classes.append(paladin.paladin_class)
player_class_birth.classes.append(priest.priest_class)
player_class_birth.classes.append(ranger.ranger_class)
player_class_birth.classes.append(rogue.rogue_class)
player_class_birth.classes.append(war_mage.warrior_mage_class)
player_class_birth.classes.append(warrior.warrior_class)

