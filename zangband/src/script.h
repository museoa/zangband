/* File: script.h */

#ifndef INCLUDED_SCRIPT_H
#define INCLUDED_SCRIPT_H

#include "angband.h"

/*
 * Initalize the scripting support
 */
extern errr script_init(void);

/*
 * Free the resources for the scripting support
 */
extern errr script_free(void);

/*
 * Display the script debug menu
 */
extern void do_cmd_script(void);

/*
 * Execute a string of scripting code
 */
extern bool script_do_string(cptr script);

/*
 * Execute a file with scripting code
 */
extern bool script_do_file(cptr filename);

/*
 * Callback for using an object
 */
extern bool use_object(object_type *o_ptr, bool *ident);

/*
 * Does the player have a certain resistance?
 */
bool player_res(u32b flag)
{
	return ((p_ptr->flags2 & flag) ? TRUE : FALSE);
}

#endif /* INCLUDED_SCRIPT_H */

