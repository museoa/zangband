%module commands

%{

#include "angband.h"

%}

typedef char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;

typedef struct cave_type cave_type;

struct cave_type
{
	byte info;		/* Hack -- cave flags */

	byte feat;		/* Hack -- feature type */

	s16b o_idx;		/* Object in this grid */

	s16b m_idx;		/* Monster in this grid */

	s16b fld_idx;		/* Field in this grid */

	byte cost;		/* Hack -- cost of flowing */
	byte when;		/* Hack -- when cost was computed */
};

/* Forward declare */
typedef struct field_type field_type;

/*
 * A function pointer to an action.  The function takes two values:
 * 1) a pointer to the index of the field that is undergoing the action.
 * 2) a pointer to a structure cast to void that contains the
 *	information the action needs to complete its job.
 */
typedef void (*field_action_type)(s16b *field_ptr, vptr);



/*
 * The thaumaturgical list of fields.
 *
 * (Equivalent to monster races, or object kinds.
 *  They had to be called something. ;-) )
 *
 * Eventually most of this, and the following struct
 * will be wrapped inside a python object.  Only things
 * that need to be accessed quickly will be left as is.
 */

typedef struct field_thaum field_thaum;
struct field_thaum
{
	byte f_attr;			/* attribute */
	char f_char;			/* character */
	
	byte d_attr;			/* Default attribute */
	char d_char;			/* Default char */

	byte priority;			/* LOS priority higher = more visible */

	byte type;			/* Type of field */

	s16b count_init;		/* Counter for timed effects */

	field_action_type action[FIELD_ACTION_MAX]; /* Function indexs for the actions */

	/* Storage space for the actions to interact with. */
	byte data_init[8];

	u16b info;			/* Information flags */

	char *name;			/* The name of the field */
};


/*
 * The field structure.
 *
 * Fields will be used to create a variety of effects from
 * the ability to place traps on _all_ terrains (not just 
 * dungeon floor), to the nightmare mode automatic corpse raising.
 *
 * The new building / store code will use this structure.
 *
 */
struct field_type
{
	byte f_attr;			/* attribute */
	char f_char;			/* character */

	s16b t_idx;			/* field type index */

	s16b fy;			/* Y location on map */
	s16b fx;			/* X location on map */

	s16b next_f_idx;		/* Pointer to next field in list */

	u16b info;			/* quick access flags */

	/* Storage space for the actions to interact with. */
	byte data[8];

	field_action_type action[FIELD_ACTION_MAX]; /* Function pointers for the actions */

	s16b counter;			/* Counter for timed effects */
	
	byte priority;			/* LOS priority higher = more visible */

#ifdef USE_SCRIPT
	PyObject *python;
#endif /* USE_SCRIPT */
};


extern void do_cmd_view_map(void);
extern void do_cmd_go_up(void);
extern void do_cmd_go_down(void);
extern void do_cmd_search(void);
extern void do_cmd_toggle_search(void);
extern void do_cmd_open(void);
extern void do_cmd_close(void);
extern void do_cmd_tunnel(void);
extern void do_cmd_disarm(void);
extern void do_cmd_alter(void);
extern void do_cmd_spike(void);
extern void do_cmd_walk(int pickup);
extern void do_cmd_stay(int pickup);
extern void do_cmd_run(void);
extern void do_cmd_rest(void);
extern void do_cmd_fire(void);
extern void do_cmd_throw(void);
extern void do_cmd_throw_aux(int mult);
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
extern void do_cmd_query_symbol(void);
extern void do_cmd_redraw(void);
extern void do_cmd_message_one(void);
extern void do_cmd_messages(void);
extern void do_cmd_options(byte flags);
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
extern void do_cmd_checkquest(void);
extern void do_cmd_time(void);
extern void do_cmd_browse(void);
extern void do_cmd_study(void);
extern void do_cmd_cast(void);
extern void do_cmd_pray(void);
extern void do_cmd_pet(void);
extern void do_cmd_eat_food(void);
extern void do_cmd_quaff_potion(void);
extern void do_cmd_read_scroll(void);
extern void do_cmd_aim_wand(void);
extern void do_cmd_use_staff(void);
extern void do_cmd_zap_rod(void);
extern void do_cmd_activate(void);
extern void do_cmd_rerate(void);
extern void do_cmd_help(void);
extern void do_cmd_suicide(void);
extern void do_cmd_save_game(int is_autosave);
extern void do_cmd_save_and_exit(void);
extern void do_cmd_racial_power(void);
extern void do_cmd_store(field_type *f_ptr);
extern void do_cmd_mindcraft(void);
extern void do_cmd_knowledge_mutations(void);
extern bool do_cmd_disarm_aux(cave_type *c_ptr, int dir);

#define OPT_FLAG_BIRTH			0x01
#define OPT_FLAG_SERVER			0x02
#define OPT_FLAG_PLAYER			0x04
