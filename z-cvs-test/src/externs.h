/* CVS: Last edit by $Author$ on $Date$ */
/* File: externs.h */

/* Purpose: extern declarations (variables and functions) */

/*
 * Note that some files have their own header files
 * (z-virt.h, z-util.h, z-form.h, term.h, random.h)
 */


/*
 * Automatically generated "variable" declarations
 */

/* tables.c */
extern s16b ddd[9];
extern s16b ddx[10];
extern s16b ddy[10];
extern s16b ddx_ddd[9];
extern s16b ddy_ddd[9];
extern s16b cdd[8];
extern s16b ddx_cdd[8];
extern s16b ddy_cdd[8];
extern char hexsym[16];
extern char listsym[];
extern cptr color_char;
extern byte adj_mag_study[];
extern byte adj_mag_mana[];
extern byte adj_mag_fail[];
extern byte adj_mag_stat[];
extern byte adj_chr_gold[];
extern byte adj_int_dev[];
extern byte adj_wis_sav[];
extern byte adj_dex_dis[];
extern byte adj_int_dis[];
extern byte adj_dex_ta[];
extern byte adj_str_td[];
extern byte adj_dex_th[];
extern byte adj_str_wgt[];
extern byte adj_str_hold[];
extern byte adj_str_dig[];
extern byte adj_str_blow[];
extern byte adj_dex_blow[];
extern byte adj_dex_safe[];
extern byte adj_con_fix[];
extern byte adj_con_mhp[];
extern byte blows_table[12][12];
extern s16b arena_monsters[MAX_ARENA_MONS];
extern owner_type owners[MAX_STORES][MAX_OWNERS];
extern byte extract_energy[200];
extern s32b player_exp[PY_MAX_LEVEL];
extern player_sex sex_info[MAX_SEXES];
extern player_race race_info[MAX_RACES];
extern player_class class_info[MAX_CLASS];
extern player_magic magic_info[MAX_CLASS];
extern u32b fake_spell_flags[4];
extern byte realm_choices1[];
extern byte realm_choices2[];
extern cptr realm_names[];
extern cptr spell_names[7][32];
extern byte deadliness_conversion[151];
extern byte chest_traps[64];
extern cptr player_title[MAX_CLASS][PY_MAX_LEVEL / 5];
extern cptr color_names[16];
extern cptr stat_names[6];
extern cptr stat_names_reduced[6];
extern cptr window_flag_desc[32];
extern option_type option_info[];
extern cptr chaos_patrons[MAX_PATRON];
extern int chaos_stats[MAX_PATRON];
extern int chaos_rewards[MAX_PATRON][20];
extern martial_arts ma_blows[MAX_MA];
extern cptr game_inscriptions[];
extern cptr silly_attacks[MAX_SILLY_ATTACK];

/* variable.c */
extern cptr copyright[5];
extern byte version_major;
extern byte version_minor;
extern byte version_patch;
extern byte version_extra;
extern byte sf_major;
extern byte sf_minor;
extern byte sf_patch;
extern byte sf_extra;
extern u32b sf_version;
extern u32b sf_xtra;
extern byte z_major;
extern byte z_minor;
extern byte z_patch;
extern u32b sf_when;
extern u16b sf_lives;
extern u16b sf_saves;
extern bool arg_fiddle;
extern bool arg_wizard;
extern bool arg_sound;
extern byte arg_graphics;
extern bool arg_monochrome;
extern bool arg_force_original;
extern bool arg_force_roguelike;
extern bool character_generated;
extern bool character_dungeon;
extern bool character_loaded;
extern bool character_saved;
extern bool character_icky;
extern bool character_xtra;
extern u32b seed_flavor;
extern u32b seed_town;
extern s16b command_cmd;
extern s16b command_arg;
extern s16b command_rep;
extern s16b command_dir;
extern s16b command_see;
extern s16b command_gap;
extern s16b command_wrk;
extern s16b command_new;
extern s16b energy_use;
extern bool create_up_stair;
extern bool create_down_stair;
extern bool msg_flag;
extern bool alive;
extern bool death;
extern s16b running;
extern s16b resting;
extern s16b cur_hgt;
extern s16b cur_wid;
extern s16b dun_level;
extern s16b num_repro;
extern bool mon_fight;
extern s16b object_level;
extern s16b monster_level;
extern s16b base_level;
extern s32b turn;
extern s32b old_turn;
extern bool wizard;
extern bool use_sound;
extern bool use_graphics;
extern u16b total_winner;
extern u16b panic_save;
extern u16b noscore;
extern s16b signal_count;
extern bool inkey_base;
extern bool inkey_xtra;
extern bool inkey_scan;
extern bool inkey_flag;
extern s16b coin_type;
extern bool opening_chest;
extern bool shimmer_monsters;
extern bool shimmer_objects;
extern bool repair_monsters;
extern bool repair_objects;
extern s16b inven_nxt;
extern s16b inven_cnt;
extern s16b equip_cnt;
extern s16b o_max;
extern s16b o_cnt;
extern s16b m_max;
extern s16b m_cnt;
extern s16b hack_m_idx;
extern s16b hack_m_idx_ii;
extern int total_friends;
extern s32b total_friend_levels;
extern s32b friend_align;
extern int leaving_quest;
extern bool multi_rew;
extern char summon_kin_type;
extern bool hack_mind;
extern bool hack_mutation;
extern int artifact_bias;
extern bool show_inven_graph;
extern bool show_store_graph;
extern bool show_equip_graph;
extern bool rogue_like_commands;
extern bool quick_messages;
extern bool other_query_flag;
extern bool carry_query_flag;
extern bool always_pickup;
extern bool always_repeat;
extern bool use_old_target;
extern bool depth_in_feet;
extern bool use_color;
extern bool compress_savefile;
extern bool hilite_player;
extern bool ring_bell;
extern bool find_ignore_stairs;
extern bool find_ignore_doors;
extern bool find_cut;
extern bool find_examine;
extern bool disturb_near;
extern bool disturb_move;
extern bool disturb_panel;
extern bool disturb_state;
extern bool disturb_minor;
extern bool disturb_other;
extern bool avoid_abort;
extern bool avoid_other;
extern bool flush_disturb;
extern bool flush_failure;
extern bool flush_command;
extern bool fresh_before;
extern bool fresh_after;
extern bool fresh_message;
extern bool alert_hitpoint;
extern bool alert_failure;
extern bool view_yellow_lite;
extern bool view_bright_lite;
extern bool view_granite_lite;
extern bool view_special_lite;
extern bool skip_mutations;	/* Skip mutations screen in 'C'haracter display */
extern bool plain_descriptions;
extern bool stupid_monsters;
extern bool auto_destroy;
extern bool wear_confirm;
extern bool confirm_stairs;
extern bool disturb_pets;
extern bool view_perma_grids;
extern bool view_torch_grids;
extern bool flow_by_sound;
extern bool flow_by_smell;
extern bool track_follow;
extern bool track_target;
extern bool stack_allow_items;
extern bool stack_allow_wands;
extern bool stack_force_notes;
extern bool stack_force_costs;
extern bool view_reduce_lite;
extern bool view_reduce_view;
extern bool auto_haggle;
extern bool auto_scum;
extern bool expand_look;
extern bool expand_list;
extern bool dungeon_align;
extern bool dungeon_stair;
extern bool smart_learn;
extern bool smart_cheat;
extern bool show_labels;
extern bool show_weights;
extern bool show_choices;
extern bool show_details;
extern bool testing_stack;
extern bool testing_carry;
extern bool cheat_peek;
extern bool cheat_hear;
extern bool cheat_room;
extern bool cheat_xtra;
extern bool cheat_know;
extern bool cheat_live;
extern bool last_words;	/* Zangband options */
extern bool speak_unique;
extern bool small_levels;
extern bool always_small_levels;
extern bool empty_levels;
extern bool player_symbols;
extern bool equippy_chars;
extern bool use_command;
extern bool center_player;
extern bool avoid_center;
extern bool pillar_tunnels;
extern bool destroy_worthless;
extern bool take_notes;
extern bool auto_notes;
extern bool point_based;
extern byte hitpoint_warn;
extern byte delay_factor;
extern s16b autosave_freq;
extern byte autosave_t;
extern byte autosave_l;
extern byte feeling;
extern s16b rating;
extern bool good_item_flag;
extern bool closing_flag;
extern s16b max_panel_rows, max_panel_cols;
extern s16b panel_row, panel_col;
extern s16b panel_row_min, panel_row_max;
extern s16b panel_col_min, panel_col_max;
extern s16b panel_col_prt, panel_row_prt;
extern s16b py;
extern s16b px;
extern s16b target_who;
extern s16b target_col;
extern s16b target_row;
extern int player_uid;
extern int player_euid;
extern int player_egid;
extern char player_name[32];
extern char player_base[32];
extern char died_from[80];
extern char history[4][60];
extern char savefile[1024];
extern s16b lite_n;
extern s16b lite_y[LITE_MAX];
extern s16b lite_x[LITE_MAX];
extern s16b view_n;
extern s16b view_y[VIEW_MAX];
extern s16b view_x[VIEW_MAX];
extern s16b temp_n;
extern s16b temp_y[TEMP_MAX];
extern s16b temp_x[TEMP_MAX];
extern s16b macro__num;
extern cptr *macro__pat;
extern cptr *macro__act;
extern bool *macro__cmd;
extern char *macro__buf;
extern s16b quark__num;
extern cptr *quark__str;
extern u16b message__next;
extern u16b message__last;
extern u16b message__head;
extern u16b message__tail;
extern u16b *message__ptr;
extern char *message__buf;
extern u32b option_flag[8];
extern u32b option_mask[8];
extern u32b window_flag[8];
extern u32b window_mask[8];
extern term *angband_term[8];
extern char angband_term_name[8][16];
extern byte angband_color_table[256][4];
extern cave_type *cave[MAX_HGT];
extern object_type *o_list;
extern monster_type *m_list;
extern u16b max_towns;
extern town_type *town;
extern object_type *inventory;
extern s16b alloc_kind_size;
extern alloc_entry *alloc_kind_table;
extern s16b alloc_race_size;
extern alloc_entry *alloc_race_table;
extern byte misc_to_attr[256];
extern char misc_to_char[256];
extern byte tval_to_attr[128];
extern char tval_to_char[128];
extern cptr keymap_act[KEYMAP_MODES][256];
extern player_type *p_ptr;
extern player_sex *sp_ptr;
extern player_race *rp_ptr;
extern player_class *cp_ptr;
extern player_magic *mp_ptr;
extern u32b spell_learned1;
extern u32b spell_learned2;
extern u32b spell_worked1;
extern u32b spell_worked2;
extern u32b spell_forgotten1;
extern u32b spell_forgotten2;
extern byte spell_order[64];
extern s16b player_hp[PY_MAX_LEVEL];
extern header *v_head;
extern vault_type *v_info;
extern char *v_name;
extern char *v_text;
extern header *f_head;
extern feature_type *f_info;
extern char *f_name;
extern char *f_text;
extern header *k_head;
extern object_kind *k_info;
extern char *k_name;
extern char *k_text;
extern header *a_head;
extern artifact_type *a_info;
extern char *a_name;
extern char *a_text;
extern header *e_head;
extern ego_item_type *e_info;
extern char *e_name;
extern char *e_text;
extern header *r_head;
extern monster_race *r_info;
extern char *r_name;
extern char *r_text;
extern cptr ANGBAND_SYS;
extern cptr ANGBAND_GRAF;
extern cptr ANGBAND_DIR;
extern cptr ANGBAND_DIR_APEX;
extern cptr ANGBAND_DIR_BONE;
extern cptr ANGBAND_DIR_DATA;
extern cptr ANGBAND_DIR_EDIT;
extern cptr ANGBAND_DIR_FILE;
extern cptr ANGBAND_DIR_HELP;
extern cptr ANGBAND_DIR_INFO;
extern cptr ANGBAND_DIR_SAVE;
extern cptr ANGBAND_DIR_USER;
extern cptr ANGBAND_DIR_XTRA;
extern bool item_tester_full;
extern byte item_tester_tval;
extern bool(*item_tester_hook) (object_type * o_ptr);
extern bool(*ang_sort_comp) (vptr u, vptr v, int a, int b);
extern void (*ang_sort_swap) (vptr u, vptr v, int a, int b);
extern monster_hook_type get_mon_num_hook;
extern monster_hook_type get_mon_num2_hook;
extern bool(*get_obj_num_hook) (int k_idx);
extern bool monk_armour_aux;
extern bool monk_notify_aux;
extern wilderness_type **wilderness;
extern cptr mut1_desc[], mut2_desc[], mut3_desc[]; /* TNB */
extern building_type building[MAX_BLDG];
extern u16b max_quests;
extern u16b max_r_idx;
extern u16b max_k_idx;
extern u16b max_v_idx;
extern u16b max_f_idx;
extern u16b max_a_idx;
extern u16b max_e_idx;
extern u16b max_o_idx;
extern u16b max_m_idx;
extern s32b max_wild_x;
extern s32b max_wild_y;
extern quest_type *quest;
extern char quest_text[10][80];
extern int quest_text_line;
extern s16b gf_color[MAX_GF];
extern int init_flags;
extern int highscore_fd;
extern bool monster_terrain_sensitive;
extern int mutant_regenerate_mod;
extern bool vanilla_town;
extern bool ironman_shops;
extern bool ironman_small_levels;
extern bool ironman_downward;
extern bool ironman_autoscum;
extern bool ironman_hard_quests;
extern bool lite_town;
extern bool ironman_empty_levels;
extern bool terrain_streams;
extern bool munchkin_death;
extern bool ironman_rooms;
extern bool ironman_nightmare;
extern bool maximize_mode;
extern bool preserve_mode;
extern bool autoroller;
extern bool use_transparency;
extern bool can_save;

/* birth.c */
extern void player_birth(void);

/* cave.c */
extern int distance(int y1, int x1, int y2, int x2);
extern bool is_trap(int feat);
extern bool los(int y1, int x1, int y2, int x2);
extern bool player_can_see_bold(int y, int x);
extern bool cave_valid_bold(int y, int x);
extern bool no_lite(void);
#ifdef USE_TRANSPARENCY
extern void map_info(int y, int x, byte * ap, char *cp, byte * tap,
	char *tcp);
#else /* USE_TRANSPARENCY */
extern void map_info(int y, int x, byte * ap, char *cp);
#endif /* USE_TRANSPARENCY */
extern void move_cursor_relative(int row, int col);
extern void print_rel(char c, byte a, int y, int x);
extern void note_spot(int y, int x);
extern void display_dungeon(void);
extern void lite_spot(int y, int x);
extern void prt_map(void);
extern void display_map(int *cy, int *cx);
extern void do_cmd_view_map(void);
extern void forget_lite(void);
extern void update_lite(void);
extern void forget_view(void);
extern void update_view(void);
extern void forget_flow(void);
extern void update_flow(void);
extern void map_area(void);
extern void wiz_lite(void);
extern void wiz_dark(void);
extern void town_illuminate(bool daytime);
extern void cave_set_feat(int y, int x, int feat);
extern void mmove2(int *y, int *x, int y1, int x1, int y2, int x2);
extern bool projectable(int y1, int x1, int y2, int x2);
extern void scatter(int *yp, int *xp, int y, int x, int d, int mode);
extern void health_track(int m_idx);
extern void monster_race_track(int r_idx);
extern void object_kind_track(int k_idx);
extern void disturb(int stop_search, int flush_output);

/* cmd1.c */
extern bool test_hit_fire(int chance, int ac, int vis);
extern bool test_hit_norm(int chance, int ac, int vis);
extern s16b tot_dam_aux(object_type * o_ptr, int tdam,
	monster_type * m_ptr);
extern void search(void);
extern bool auto_pickup_okay(object_type * o_ptr);
extern void py_pickup_aux(int o_idx);
extern void carry(int pickup);
extern void py_attack(int y, int x);
extern void move_player(int dir, int do_pickup);
extern void run_step(int dir);

/* cmd2.c */
extern void do_cmd_go_up(void);
extern void do_cmd_go_down(void);
extern void do_cmd_search(void);
extern void do_cmd_toggle_search(void);
extern void do_cmd_open(void);
extern void do_cmd_close(void);
extern void do_cmd_tunnel(void);
extern void do_cmd_disarm(void);
extern void do_cmd_bash(void);
extern void do_cmd_alter(void);
extern void do_cmd_spike(void);
extern void do_cmd_walk(int pickup);
extern void do_cmd_stay(int pickup);
extern void do_cmd_run(void);
extern void do_cmd_rest(void);
extern void do_cmd_fire(void);
extern void do_cmd_fire_aux(int item, object_type * j_ptr);
extern void do_cmd_throw(void);
extern void do_cmd_throw_aux(int mult);

/* cmd3.c */
extern void do_cmd_inven(void);
extern void do_cmd_equip(void);
extern void do_cmd_wield(void);
extern void do_cmd_takeoff(void);
extern void do_cmd_drop(void);
extern void do_cmd_destroy(void);
extern void do_cmd_observe(void);
extern void do_cmd_uninscribe(void);
extern void do_cmd_inscribe(void);
extern void do_cmd_refill(void);
extern void do_cmd_target(void);
extern void do_cmd_look(void);
extern void do_cmd_locate(void);
extern bool ang_sort_comp_hook(vptr, vptr, int, int);
extern void ang_sort_swap_hook(vptr, vptr, int, int);
extern void do_cmd_query_symbol(void);
extern bool research_mon(void);

/* cmd4.c */
extern void do_cmd_redraw(void);
extern void do_cmd_message_one(void);
extern void do_cmd_messages(void);
extern void do_cmd_options_aux(int page, cptr info);
extern void do_cmd_options(void);
extern void do_cmd_pref(void);
extern void do_cmd_macros(void);
extern void do_cmd_visuals(void);
extern void do_cmd_colors(void);
extern void do_cmd_note(void);
extern void do_cmd_version(void);
extern void do_cmd_feeling(void);
extern void do_cmd_load_screen(void);
extern void do_cmd_save_screen(void);
extern void do_cmd_knowledge(void);
extern void plural_aux(char *Name);
extern void do_cmd_checkquest(void);
extern void do_cmd_time(void);

/* cmd5.c */
extern void do_cmd_browse(void);
extern void do_cmd_study(void);
extern void do_cmd_cast(void);
extern void do_cmd_pray(void);
extern void do_cmd_pet(void);

/* cmd6.c */
extern void do_cmd_eat_food(void);
extern void do_cmd_quaff_potion(void);
extern void do_cmd_read_scroll(void);
extern void do_cmd_aim_wand(void);
extern void do_cmd_use_staff(void);
extern void do_cmd_zap_rod(void);
extern void do_cmd_activate(void);
extern void do_cmd_rerate(void);
extern void ring_of_power(int dir);
extern void do_cmd_use(void);

/* dungeon.c */
extern void play_game(bool new_game);
extern bool psychometry(void);
extern void leave_level(int level);
extern void enter_level(int level);


/* files.c */
extern void safe_setuid_drop(void);
extern void safe_setuid_grab(void);
extern s16b tokenize(char *buf, s16b num, char **tokens, int mode);
extern void display_player(int mode);
extern void do_cmd_character(void);
extern errr file_character(cptr name, bool full);
extern errr process_pref_file_aux(char *buf);
extern errr process_pref_file(cptr name);
extern void print_equippy(void);
extern errr check_time_init(void);
extern errr check_load_init(void);
extern errr check_time(void);
extern errr check_load(void);
extern bool show_file(cptr name, cptr what, int line, int mode);
extern void do_cmd_help(void);
extern void process_player_name(bool sf);
extern void change_player_name(void);
extern void do_cmd_suicide(void);
extern void do_cmd_save_game(int is_autosave);
extern void do_cmd_save_and_exit(void);
extern long total_points(void);
extern void close_game(void);
extern void exit_game_panic(void);
extern void signals_ignore_tstp(void);
extern void signals_handle_tstp(void);
extern void signals_init(void);
extern errr get_rnd_line(cptr file_name, int entry, char *output);
extern void get_character_name(void);

/* generate.c */
extern void place_closed_door(int y, int x);
extern void generate_cave(void);

/* init1.c */
extern errr init_v_info_txt(FILE * fp, char *buf, bool start);
extern errr init_f_info_txt(FILE * fp, char *buf);
extern errr init_k_info_txt(FILE * fp, char *buf);
extern errr init_a_info_txt(FILE * fp, char *buf);
extern errr init_e_info_txt(FILE * fp, char *buf);
extern errr init_r_info_txt(FILE * fp, char *buf);
extern errr process_dungeon_file(cptr name, int ymin, int xmin, int ymax,
	int xmax);

/* init2.c */
extern cptr err_str[PARSE_ERROR_MAX];
extern errr init_v_info(void);
extern void init_file_paths(char *path);
extern void init_angband(void);
extern errr init_buildings(void);
#ifdef ALLOW_TEMPLATES
extern s16b error_idx;
extern s16b error_line;
extern u32b fake_name_size;
extern u32b fake_text_size;
#endif /* ALLOW_TEMPLATES */

/* load.c */
extern errr rd_savefile_new(void);

/* melee1.c */
/* melee2.c */
extern bool make_attack_normal(int m_idx);
extern bool make_attack_spell(int m_idx);
extern void process_monsters(int min_energy);
extern void curse_equipment(int chance, int heavy_chance);
extern void mon_take_hit_mon(int m_idx, int dam, bool * fear, cptr note);

/* monster1.c */
extern void screen_roff(int r_idx, int remember);
extern void display_roff(int r_idx);
extern void create_name(int type, char *name);

/* monster2.c */
extern cptr horror_desc[MAX_SAN_HORROR];
extern cptr funny_desc[MAX_SAN_FUNNY];
extern cptr funny_comments[MAX_SAN_COMMENT];
extern int get_wilderness_flag(void);
extern void sanity_blast(monster_type * m_ptr, bool necro);
extern void delete_monster_idx(int i);
extern void delete_monster(int y, int x);
extern void compact_monsters(int size);
extern void wipe_m_list(void);
extern s16b m_pop(void);
extern errr get_mon_num_prep(monster_hook_type monster_hook,
	monster_hook_type monster_hook2);
extern s16b get_mon_num(int level);
extern void monster_desc(char *desc, monster_type * m_ptr, int mode);
extern void lore_do_probe(int m_idx);
extern void lore_treasure(int m_idx, int num_item, int num_gold);
extern void update_mon(int m_idx, bool full);
extern void update_monsters(bool full);
#if 1 /* TNB */
extern void monster_disturb(int m_idx);
extern void pack_gold(object_type * o_ptr, s32b cost);
extern void monster_steal_gold(int m_idx, s32b cost);
extern s16b monster_carry(int m_idx, object_type * j_ptr);
extern void monster_swap(int y1, int x1, int y2, int x2);
#endif /* TNB */
extern bool place_monster_aux(int y, int x, int r_idx, bool slp, bool grp,
	bool friendly, bool pet);
extern bool place_monster(int y, int x, bool slp, bool grp);
extern bool alloc_horde(int y, int x);
extern bool alloc_monster(int dis, bool slp);
extern bool summon_specific(int who, int y1, int x1, int lev, int type,
	bool group, bool friendly, bool pet);
extern bool summon_named_creature(int oy, int ox, int r_idx, bool slp,
	bool group_ok, bool pet);
extern bool multiply_monster(int m_idx, bool clone, bool friendly,
	bool pet);
extern void update_smart_learn(int m_idx, int what);
extern bool place_monster_one(int y, int x, int r_idx, bool slp,
	bool friendly, bool pet);
extern bool player_place(int y, int x);
extern void monster_drop_carried_objects(monster_type * m_ptr);

/* monster3.c (currently in monster1.c) */
extern bool monster_dungeon(int r_idx);
extern bool monster_quest(int r_idx);
extern bool monster_ocean(int r_idx);
extern bool monster_shore(int r_idx);
extern bool monster_town(int r_idx);
extern bool monster_wood(int r_idx);
extern bool monster_volcano(int r_idx);
extern bool monster_mountain(int r_idx);
extern bool monster_grass(int r_idx);
extern bool monster_deep_water(int r_idx);
extern bool monster_shallow_water(int r_idx);
extern bool monster_lava(int r_idx);
extern monster_hook_type get_monster_hook(void);
extern monster_hook_type get_monster_hook2(int y, int x);
extern void set_friendly(monster_type * m_ptr);
extern void set_pet(monster_type * m_ptr);
extern void set_hostile(monster_type * m_ptr);
extern void anger_monster(monster_type * m_ptr);
extern bool monster_can_cross_terrain(byte feat, monster_race * r_ptr);
extern bool are_enemies(monster_type * m_ptr1, monster_type * m_ptr2);
extern bool monster_living(monster_race * r_ptr);

/* object1.c */
/* object2.c */
extern void flavor_init(void);
extern void reset_visuals(void);
extern void object_flags(object_type * o_ptr, u32b * f1, u32b * f2,
	u32b * f3);
extern void object_flags_known(object_type * o_ptr, u32b * f1, u32b * f2,
	u32b * f3);
extern void object_desc(char *buf, object_type * o_ptr, int pref,
	int mode);
extern void object_desc_store(char *buf, object_type * o_ptr, int pref,
	int mode);
extern bool identify_fully_aux(object_type * o_ptr);
extern char index_to_label(int i);
extern s16b label_to_inven(int c);
extern s16b label_to_equip(int c);
extern s16b wield_slot(object_type * o_ptr);
extern cptr mention_use(int i);
extern cptr describe_use(int i);
extern void inven_item_charges(int item);
extern void inven_item_describe(int item);
extern void inven_item_increase(int item, int num);
extern void inven_item_optimize(int item);
extern void floor_item_charges(int item);
extern void floor_item_describe(int item);
extern void floor_item_increase(int item, int num);
extern void floor_item_optimize(int item);
extern bool inven_carry_okay(object_type * o_ptr);
extern s16b inven_carry(object_type * o_ptr);
extern s16b inven_takeoff(int item, int amt);
extern void inven_drop(int item, int amt);
extern bool item_tester_okay(object_type * o_ptr);
extern void display_inven(void);
extern void display_equip(void);
extern void show_inven(void);
extern void show_equip(void);
extern void toggle_inven_equip(void);
extern bool get_item(int *cp, cptr pmt, cptr str, int mode);
extern void excise_object_idx(int o_idx);
extern void delete_object_idx(int o_idx);
extern void delete_object(int y, int x);
extern void compact_objects(int size);
extern void wipe_o_list(void);
extern s16b o_pop(void);
extern errr get_obj_num_prep(void);
extern s16b get_obj_num(int level);
extern void object_known(object_type * o_ptr);
extern void object_aware(object_type * o_ptr);
extern void object_tried(object_type * o_ptr);
extern s32b object_value(object_type * o_ptr);
extern s32b object_value_real(object_type * o_ptr);
extern void distribute_charges(object_type * o_ptr, object_type * q_ptr,
	int amt);
extern void reduce_charges(object_type * o_ptr, int amt);
extern bool object_similar(object_type * o_ptr, object_type * j_ptr);
extern void object_absorb(object_type * o_ptr, object_type * j_ptr);
extern s16b lookup_kind(int tval, int sval);
extern void object_wipe(object_type * o_ptr);
extern void object_prep(object_type * o_ptr, int k_idx);
extern void object_copy(object_type * o_ptr, object_type * j_ptr);
extern void apply_magic(object_type * o_ptr, int lev, bool okay, bool good,
	bool great, bool curse);
extern bool make_object(object_type * j_ptr, bool good, bool great);
extern void place_object(int y, int x, bool good, bool great);
extern bool make_gold(object_type * j_ptr);
extern void place_gold(int y, int x);
extern s16b drop_near(object_type * o_ptr, int chance, int y, int x);
extern void acquirement(int y1, int x1, int num, bool great, bool known);
extern void pick_trap(int y, int x);
extern void place_trap(int y, int x);
extern cptr item_activation(object_type * o_ptr);
extern void combine_pack(void);
extern void reorder_pack(void);
extern bool can_player_destroy_object(object_type * o_ptr);
extern void display_koff(int k_idx);

/* racial.c */
extern bool racial_aux(s16b min_level, int cost, int use_stat,
	int difficulty);
extern void do_cmd_racial_power(void);

/* save.c */
extern bool save_player(void);
extern bool load_player(void);
extern void remove_loc(void);

/* spells1.c */
extern void take_hit(int damage, cptr kb_str);
extern u16b bolt_pict(int y, int x, int ny, int nx, int typ);
extern sint project_path(coord * gp, int range, int y1, int x1, int y2,
	int x2, u16b flg);
extern int dist_to_line(int y, int x, int y1, int x1, int y2, int x2);
extern bool project(int who, int rad, int y, int x, int dam, int typ,
	u16b flg);

/* spells2.c */
extern void message_pain(int m_idx, int dam);
extern void self_knowledge(void);
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
extern bool stasis_monster(int dir); /* Like sleep, affects undead as well */
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
extern s32b flag_cost(object_type * o_ptr, int plusses);
extern void report_magics(void);
extern bool teleport_swap(int dir);
extern bool item_tester_hook_recharge(object_type * o_ptr);
extern bool project_hook(int typ, int dir, int dam, u16b flg);
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
extern void apply_nexus(monster_type * m_ptr);
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
extern bool item_tester_hook_weapon(object_type * o_ptr);
extern bool item_tester_hook_armour(object_type * o_ptr);
extern bool item_tester_hook_weapon_armour(object_type * o_ptr);
extern bool enchant(object_type * o_ptr, int n, int eflag);
extern bool enchant_spell(int num_hit, int num_dam, int num_ac);
extern bool artifact_scroll(void);
extern bool ident_spell(void);
extern bool mundane_spell(void);
extern void identify_item(object_type * o_ptr);
extern bool identify_fully(void);
extern bool recharge(int num);
extern bool bless_weapon(void);
extern bool potion_smash_effect(int who, int y, int x, int k_idx);
extern void display_spell_list(void);
extern s16b spell_chance(int spell, int realm);
extern bool spell_okay(int spell, bool known, int realm);
extern void print_spells(byte * spells, int num, int y, int x, int realm);
extern bool hates_acid(object_type * o_ptr);
extern bool hates_elec(object_type * o_ptr);
extern bool hates_fire(object_type * o_ptr);
extern bool hates_cold(object_type * o_ptr);
extern int set_acid_destroy(object_type * o_ptr);
extern int set_elec_destroy(object_type * o_ptr);
extern int set_fire_destroy(object_type * o_ptr);
extern int set_cold_destroy(object_type * o_ptr);
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

/* store.c */
extern void do_cmd_store(void);
extern void store_shuffle(int which);
extern void store_maint(int town_num, int store_num);
extern void store_init(int town_num, int store_num);
extern void move_to_black_market(object_type * o_ptr);

/* bldg.c */
extern bool get_nightmare(int r_idx);
extern void have_nightmare(int r_idx);
extern void do_cmd_bldg(void);
extern void do_cmd_quest(void);
extern void quest_discovery(int q_idx);
extern int quest_number(int level);
extern int random_quest_number(int level);

/* util.c */
extern errr path_parse(char *buf, int max, cptr file);
extern errr path_temp(char *buf, int max);
extern errr path_build(char *buf, int max, cptr path, cptr file);
extern FILE *my_fopen(cptr file, cptr mode);
extern errr my_fgets(FILE * fff, char *buf, huge n);
extern errr my_fputs(FILE * fff, cptr buf, huge n);
extern errr my_fclose(FILE * fff);
extern errr fd_kill(cptr file);
extern errr fd_move(cptr file, cptr what);
extern errr fd_copy(cptr file, cptr what);
extern int fd_make(cptr file, int mode);
extern int fd_open(cptr file, int flags);
extern errr fd_lock(int fd, int what);
extern errr fd_seek(int fd, huge n);
extern errr fd_chop(int fd, huge n);
extern errr fd_read(int fd, char *buf, huge n);
extern errr fd_write(int fd, cptr buf, huge n);
extern errr fd_close(int fd);
extern void flush(void);
extern void bell(void);
extern void move_cursor(int row, int col);
extern void text_to_ascii(char *buf, cptr str);
extern void ascii_to_text(char *buf, cptr str);
extern errr macro_add(cptr pat, cptr act);
extern sint macro_find_exact(cptr pat);
extern char inkey(void);
extern cptr quark_str(s16b num);
extern s16b quark_add(cptr str);
extern s16b message_num(void);
extern cptr message_str(int age);
extern void message_add(cptr msg);
extern void msg_print(cptr msg);
#ifndef SWIG
extern void msg_format(cptr fmt, ...);
#endif /* SWIG */
extern void screen_save(void);
extern void screen_load(void);
#if 1 /* TNB */
#define c_put_str(attr,str,row,col)
#define put_str(str,row,col)
#define c_prt(attr,str,row,col)
#define prt(str,row,col)
#define c_roff(attr,str)
#define roff(str)
#define clear_from(row)
#else /* TNB */
extern void c_put_str(byte attr, cptr str, int row, int col);
extern void put_str(cptr str, int row, int col);
extern void c_prt(byte attr, cptr str, int row, int col);
extern void prt(cptr str, int row, int col);
extern void c_roff(byte attr, cptr str);
extern void roff(cptr str);
extern void clear_from(int row);
#endif /* TNB */
extern bool askfor_aux(char *buf, int len);
extern bool get_string(cptr prompt, char *buf, int len);
extern bool get_check(cptr prompt);
extern bool get_com(cptr prompt, char *command);
extern s16b get_quantity(cptr prompt, int max);
extern void pause_line(int row);
extern void request_command(int shopping);
extern bool is_a_vowel(int ch);
extern int get_keymap_dir(char ch);

#ifdef SORT_R_INFO
extern void tag_sort(tag_type elements[], int number);
#endif /* SORT_R_INFO */

#ifdef SUPPORT_GAMMA
extern byte gamma_table[256];
extern void build_gamma_table(int gamma);
#endif /* SUPPORT_GAMMA */

/* xtra1.c */
extern void cnv_stat(int val, char *out_val);
extern s16b modify_stat_value(int value, int amount);
extern void notice_stuff(void);
extern void update_stuff(void);
extern void redraw_stuff(void);
extern void window_stuff(void);
extern void handle_stuff(void);
extern bool monk_empty_hands(void);
extern bool monk_heavy_armor(void);
extern void calc_bonuses(void);
extern int number_of_quests(void);

/* effects.c */
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


/* xtra2.c */
extern void check_experience(void);
extern void monster_death(int m_idx);
extern bool mon_take_hit(int m_idx, int dam, bool * fear, cptr note);
extern void panel_bounds(void);
extern void panel_bounds_center(void);
extern bool change_panel(int dy, int dx);
extern void verify_panel(void);
extern cptr look_mon_desc(int m_idx);
extern void ang_sort_aux(vptr u, vptr v, int p, int q);
extern void ang_sort(vptr u, vptr v, int n);
#ifdef ALLOW_EASY_OPEN
extern sint motion_dir(int y1, int x1, int y2, int x2);
#endif /* ALLOW_EASY_OPEN */
extern bool target_able(int m_idx);
extern bool target_okay(void);
extern void target_set_monster(int m_idx); /* TNB */
extern void target_set_location(int y, int x); /* TNB */
extern bool target_set(int mode);
extern bool get_aim_dir(int *dp);
extern bool get_hack_dir(int *dp);
extern bool get_rep_dir(int *dp);
extern int get_chaos_patron(void);
extern void gain_level_reward(int chosen_reward);
extern bool set_wraith_form(int v);
extern bool set_tim_esp(int v);
extern bool tgt_pt(int *x, int *y);
extern void do_poly_wounds(void);
extern int mon_damage_mod(monster_type * m_ptr, int dam, int type);

/* mspells1.c */
extern bool clean_shot(int y1, int x1, int y2, int x2, bool friend);

/* mspells2.c */
extern bool monst_spell_monst(int m_idx);

/* artifact.c */
extern bool create_artifact(object_type * o_ptr, bool a_scroll);
extern void random_resistance(object_type * q_ptr, bool is_scroll,
	int specific);
extern bool activate_random_artifact(object_type * o_ptr);
extern void random_artifact_resistance(object_type * o_ptr);
extern void create_named_art(int a_idx, int y, int x);

/* scores.c */
extern void display_scores_aux(int from, int to, int note,
	high_score * score);
extern void display_scores(int from, int to);
extern void kingly(void);
extern errr top_twenty(void);
#if 1 /* TNB */
extern int highscore_seek(int i);
extern errr highscore_read(high_score * score);
extern int highscore_write(high_score * score);
extern int highscore_where(high_score * score);
extern int highscore_add(high_score * score);
extern void predict_score(high_score * score);
#else /* TNB */
extern errr predict_score(void);
#endif /* TNB */
extern void race_legends(void);
extern void race_score(int race_num);
extern void show_highclass(int building);

/* mind.c */
extern mindcraft_power mindcraft_powers[MAX_MINDCRAFT_POWERS];
extern void mindcraft_info(char *p, int power);
extern void do_cmd_mindcraft(void);

/* mutation.c */
extern bool gain_random_mutation(int choose_mut);
extern bool lose_mutation(int choose_mut);
extern void dump_mutations(FILE * OutFile);
extern void do_cmd_knowledge_mutations(void);
extern int calc_mutant_regenerate_mod(void);
extern void mutation_power_aux(u32b power);


/*
 * Hack -- conditional (or "bizarre") externs
 */

#ifdef SET_UID
/* util.c */
extern void user_name(char *buf, int id);
#endif

#ifndef HAS_MEMSET
/* util.c */
extern char *memset(char *, int, huge);
#endif

#if 0
#ifndef HAS_STRICMP
/* util.c */
extern int stricmp(cptr a, cptr b);
#endif
#endif

#ifndef HAS_USLEEP
/* util.c */
extern int usleep(huge usecs);
#endif

#ifdef MACINTOSH
/* main-mac.c */
/* extern int main(void); */
#endif

#ifdef WINDOWS
/* main-win.c */
/* extern int FAR PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, ...); */
#endif


#ifdef ALLOW_REPEAT	/* TNB */

/* util.c */
extern void repeat_push(int what);
extern bool repeat_pull(int *what);
extern void repeat_check(void);

#endif /* ALLOW_REPEAT -- TNB */

#ifdef ALLOW_EASY_OPEN /* TNB */

/* variable.c */
extern bool easy_open;

#endif /* ALLOW_EASY_OPEN -- TNB */

#ifdef ALLOW_EASY_ALTER /* TNB */

/* variable.c */
extern bool easy_alter;

#endif /* ALLOW_EASY_ALTER -- TNB */

#ifdef ALLOW_EASY_FLOOR	/* TNB */

/* object1.c */
extern bool scan_floor(int *items, int *item_num, int y, int x, int mode);
extern void show_floor(int y, int x);
extern bool get_item_floor(int *cp, cptr pmt, cptr str, int mode);
extern void py_pickup_floor(int pickup);

/* variable.c */
extern bool easy_floor;

#endif /* ALLOW_EASY_FLOOR -- TNB */

/* obj_kind.c */
extern errr k_info_alloc(void);
extern errr k_info_free(void);
extern object_kind *k_info_add(object_kind * k_info_entry);
extern byte get_object_level(object_type * o_ptr);
extern s32b get_object_cost(object_type * o_ptr);
extern cptr get_object_name(object_type * o_ptr);
extern bool get_object_aware(object_type * o_ptr);
extern bool get_object_tried(object_type * o_ptr);
extern bool object_is_potion(object_type * o_ptr);
extern errr init_object_alloc(void);
extern void k_info_reset(void);

/* wild.c */
extern void wilderness_gen(void);
extern errr init_wilderness(void);
extern void seed_wilderness(void);
extern errr parse_line_wilderness(char *buf, int ymin, int xmin, int ymax,
	int xmax, int *y, int *x);

/* avatar.c */
extern cptr virtue[MAX_VIRTUE];
extern void get_virtues(void);
extern void chg_virtue(int virtue, int amount);
extern void dump_virtues(FILE * OutFile);

/* notes.c */
extern cptr notes_file(void);
extern void output_note(char *final_note);
extern void add_note(char *note, char code);
extern void add_note_type(int note_number);
