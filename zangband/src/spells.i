%module spells

%{

#include "angband.h"

%}


typedef const char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;


/* Detection spells */
extern bool detect_traps(void);
extern bool detect_doors(void);
extern bool detect_stairs(void);
extern bool detect_treasure(void);
extern bool detect_objects_gold(void);
extern bool detect_objects_normal(void);
extern bool detect_objects_magic(void);
extern bool detect_monsters_normal(void);
extern bool detect_monsters_invis(void);
extern bool detect_monsters_evil(void);
extern bool detect_monsters_xxx(u32b match_flag);
extern bool detect_monsters_string(cptr);
extern bool detect_monsters_nonliving(void);
extern bool detect_all(void);


/* Effects */
extern bool set_blind(int v);
extern bool set_confused(int v);
extern bool set_poisoned(int v);
extern bool set_afraid(int v);
extern bool set_paralyzed(int v);
extern bool set_image(int v);
extern bool set_fast(int v);
extern bool set_slow(int v);
extern bool set_shield(int v);
extern bool set_blessed(int v);
extern bool set_hero(int v);
extern bool set_shero(int v);
extern bool set_protevil(int v);
extern bool set_invuln(int v);
extern bool set_tim_invis(int v);
extern bool set_tim_infra(int v);
extern bool set_oppose_acid(int v);
extern bool set_oppose_elec(int v);
extern bool set_oppose_fire(int v);
extern bool set_oppose_cold(int v);
extern bool set_oppose_pois(int v);
extern bool set_stun(int v);
extern bool set_cut(int v);
extern bool set_food(int v);
extern bool inc_stat(int stat);
extern bool dec_stat(int stat, int amount, int permanent);
extern bool res_stat(int stat);
extern bool hp_player(int num);
extern bool do_dec_stat(int stat);
extern bool do_res_stat(int stat);
extern bool do_inc_stat(int stat);
extern bool restore_level(void);
extern bool lose_all_info(void);
extern void gain_exp(s32b amount);
extern void lose_exp(s32b amount);
extern void do_poly_self(void);


/* spells1.c */
extern void take_hit(int damage, cptr kb_str);
extern u16b bolt_pict(int y, int x, int ny, int nx, int typ);
extern sint project_path(coord *gp, int range, int y1, int x1, int y2, int x2, int flg);
extern int dist_to_line(int y, int x, int y1, int x1, int y2, int x2);
extern bool project(int who, int rad, int y, int x, int dam, int typ, int flg);

/* spells2.c */
extern void message_pain(int m_idx, int dam);
extern void self_knowledge(void);
extern bool wall_stone(void);
extern bool speed_monsters(void);
extern bool slow_monsters(void);
extern bool sleep_monsters(void);
extern void aggravate_monsters(int who);
extern bool genocide(int player_cast);
extern bool mass_genocide(int player_cast);
extern bool probing(void);
extern bool banish_evil(int dist);
extern bool dispel_evil(int dam);
extern bool dispel_good(int dam);
extern bool dispel_undead(int dam);
extern bool dispel_monsters(int dam);
extern bool dispel_living(int dam);
extern bool dispel_demons(int dam);
extern bool turn_undead(void);
extern bool destroy_area(int y1, int x1, int r, int full);
extern bool earthquake(int cy, int cx, int r);
extern void lite_room(int y1, int x1);
extern void unlite_room(int y1, int x1);
extern bool lite_area(int dam, int rad);
extern bool unlite_area(int dam, int rad);
extern bool fire_ball(int typ, int dir, int dam, int rad);
extern bool fire_bolt(int typ, int dir, int dam);
extern void call_chaos(void);
extern bool fire_beam(int typ, int dir, int dam);
extern bool fire_bolt_or_beam(int prob, int typ, int dir, int dam);
extern bool lite_line(int dir);
extern bool drain_life(int dir, int dam);
extern bool death_ray(int dir, int plev);
extern bool wall_to_mud(int dir);
extern bool destroy_door(int dir);
extern bool disarm_trap(int dir);
extern bool wizard_lock(int dir);
extern bool heal_monster(int dir);
extern bool speed_monster(int dir);
extern bool slow_monster(int dir);
extern bool sleep_monster(int dir);
extern bool stasis_monster(int dir);    /* Like sleep, affects undead as well */
extern bool confuse_monster(int dir, int plev);
extern bool stun_monster(int dir, int plev);
extern bool fear_monster(int dir, int plev);
extern bool poly_monster(int dir);
extern bool clone_monster(int dir);
extern bool teleport_monster(int dir);
extern bool door_creation(void);
extern bool trap_creation(void);
extern bool glyph_creation(void);
extern bool destroy_doors_touch(void);
extern bool sleep_monsters_touch(void);
extern bool activate_ty_curse(bool stop_ty, int *count);
extern int activate_hi_summon(void);
extern int summon_cyber(int who, int y, int x);
extern void wall_breaker(void);
extern bool confuse_monsters(int dam);
extern bool charm_monsters(int dam);
extern bool charm_animals(int dam);
extern bool stun_monsters(int dam);
extern bool stasis_monsters(int dam);
extern bool banish_monsters(int dist);
extern bool turn_monsters(int dam);
extern bool turn_evil(int dam);
extern bool deathray_monsters(void);
extern bool charm_monster(int dir, int plev);
extern bool control_one_undead(int dir, int plev);
extern bool charm_animal(int dir, int plev);
extern bool mindblast_monsters(int dam);
extern void get_table_name(char *out_string);
extern s32b flag_cost(object_type *o_ptr, int plusses);
extern void report_magics(void);
extern bool teleport_swap(int dir);
extern bool item_tester_hook_recharge(object_type *o_ptr);
extern bool project_hook(int typ, int dir, int dam, int flg);
extern bool project_hack(int typ, int dam);

/* spells3.c */
extern bool teleport_away(int m_idx, int dis);
extern void teleport_to_player(int m_idx);
extern void teleport_player(int dis);
extern void teleport_player_to(int ny, int nx);
extern void teleport_player_level(void);
extern void recall_player(int turns);
extern void word_of_recall(void);
extern bool apply_disenchant(int mode);
extern void mutate_player(void);
extern void apply_nexus(monster_type *m_ptr);
extern void phlogiston(void);
extern void brand_weapon(int brand_type);
extern void call_the_(void);
extern void fetch(int dir, int wgt, bool require_los);
extern void alter_reality(void);
extern bool warding_glyph(void);
extern bool explosive_rune(void);
extern void identify_pack(void);
extern bool remove_curse(void);
extern bool remove_all_curse(void);
extern bool alchemy(void);
extern void stair_creation(void);
extern bool item_tester_hook_weapon(object_type *o_ptr);
extern bool item_tester_hook_armour(object_type *o_ptr);
extern bool item_tester_hook_weapon_armour(object_type *o_ptr);
extern bool enchant(object_type *o_ptr, int n, int eflag);
extern bool enchant_spell(int num_hit, int num_dam, int num_ac);
extern bool artifact_scroll(void);
extern bool ident_spell(void);
extern bool mundane_spell(void);
extern void identify_item(object_type *o_ptr);
extern bool identify_fully(void);
extern bool recharge(int num);
extern bool bless_weapon(void);
extern bool potion_smash_effect(int who, int y, int x, int k_idx);
extern void display_spell_list(void);
extern s16b spell_chance(int spell, int realm);
extern bool spell_okay(int spell, bool known, int realm);
extern void print_spells(byte *spells, int num, int y, int x, int realm);
extern bool hates_acid(object_type *o_ptr);
extern bool hates_elec(object_type *o_ptr);
extern bool hates_fire(object_type *o_ptr);
extern bool hates_cold(object_type *o_ptr);
extern int set_acid_destroy(object_type *o_ptr);
extern int set_elec_destroy(object_type *o_ptr);
extern int set_fire_destroy(object_type *o_ptr);
extern int set_cold_destroy(object_type *o_ptr);
extern int inven_damage(inven_func typ, int perc);
extern void acid_dam(int dam, cptr kb_str);
extern void elec_dam(int dam, cptr kb_str);
extern void fire_dam(int dam, cptr kb_str);
extern void cold_dam(int dam, cptr kb_str);
extern bool rustproof(void);
extern bool curse_armor(void);
extern bool curse_weapon(void);
extern bool brand_bolts(void);
extern bool polymorph_monster(int y, int x);
extern bool dimension_door(void);


/*
 * Legal restrictions for "summon_specific()"
 */
#define SUMMON_ANT                  11
#define SUMMON_SPIDER               12
#define SUMMON_HOUND                13
#define SUMMON_HYDRA                14
#define SUMMON_ANGEL                15
#define SUMMON_DEMON                16
#define SUMMON_UNDEAD               17
#define SUMMON_DRAGON               18
#define SUMMON_HI_UNDEAD            21
#define SUMMON_HI_DRAGON            22
#define SUMMON_AMBERITES            31
#define SUMMON_UNIQUE               32
#define SUMMON_BIZARRE1             33
#define SUMMON_BIZARRE2             34
#define SUMMON_BIZARRE3             35
#define SUMMON_BIZARRE4             36
#define SUMMON_BIZARRE5             37
#define SUMMON_BIZARRE6             38
#define SUMMON_CYBER                39
#define SUMMON_KIN                  40
#define SUMMON_DAWN                 41
#define SUMMON_ANIMAL               42
#define SUMMON_ANIMAL_RANGER        43
#define SUMMON_HI_UNDEAD_NO_UNIQUES 44
#define SUMMON_HI_DRAGON_NO_UNIQUES 45
#define SUMMON_NO_UNIQUES           46
#define SUMMON_PHANTOM              47
#define SUMMON_ELEMENTAL            48
#define SUMMON_BLUE_HORROR          49
#define SUMMON_GHB						50



/*
 * Spell types used by project(), and related functions.
 */
#define GF_ELEC         1
#define GF_POIS         2
#define GF_ACID         3
#define GF_COLD         4
#define GF_FIRE         5
#define GF_MISSILE      10
#define GF_ARROW        11
#define GF_PLASMA       12
/* XXX */
#define GF_WATER        14
#define GF_LITE         15
#define GF_DARK         16
#define GF_LITE_WEAK    17
#define GF_DARK_WEAK    18
#define GF_SHARDS       20
#define GF_SOUND        21
#define GF_CONFUSION    22
#define GF_FORCE        23
#define GF_INERTIA      24
#define GF_MANA         26
#define GF_METEOR       27
#define GF_ICE          28
#define GF_CHAOS        30
#define GF_NETHER       31
#define GF_DISENCHANT   32
#define GF_NEXUS        33
#define GF_TIME         34
#define GF_GRAVITY      35
#define GF_KILL_WALL    40
#define GF_KILL_DOOR    41
#define GF_KILL_TRAP    42
#define GF_MAKE_WALL    45
#define GF_MAKE_DOOR    46
#define GF_MAKE_TRAP    47
#define GF_OLD_CLONE    51
#define GF_OLD_POLY             52
#define GF_OLD_HEAL             53
#define GF_OLD_SPEED    54
#define GF_OLD_SLOW             55
#define GF_OLD_CONF             56
#define GF_OLD_SLEEP    57
#define GF_OLD_DRAIN    58
#define GF_AWAY_UNDEAD  61
#define GF_AWAY_EVIL    62
#define GF_AWAY_ALL     63
#define GF_TURN_UNDEAD  64
#define GF_TURN_EVIL    65
#define GF_TURN_ALL     66
#define GF_DISP_UNDEAD  67
#define GF_DISP_EVIL    68
#define GF_DISP_ALL 69
#define GF_DISP_DEMON   70      /* New types for Zangband begin here... */
#define GF_DISP_LIVING  71
#define GF_ROCKET       72
#define GF_NUKE         73
#define GF_MAKE_GLYPH   74
#define GF_STASIS       75
#define GF_STONE_WALL   76
#define GF_DEATH_RAY    77
#define GF_STUN         78
#define GF_HOLY_FIRE    79
#define GF_HELL_FIRE    80
#define GF_DISINTEGRATE 81
#define GF_CHARM        82
#define GF_CONTROL_UNDEAD   83
#define GF_CONTROL_ANIMAL   84
#define GF_PSI         85
#define GF_PSI_DRAIN   86
#define GF_TELEKINESIS  87
#define GF_JAM_DOOR     88
#define GF_DOMINATION   89
#define GF_DISP_GOOD    90

#define MAX_GF				91
