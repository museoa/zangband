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
 * Execute one of the scripts attached to an object
 */
extern bool apply_object_trigger(int trigger_id, object_type *o_ptr, bool *ident, 
		cptr var1, int val1, cptr var2, int val2, cptr var3, int val3);
extern char *apply_object_trigger_str(int trigger_id, object_type *o_ptr);

/*
 * Callback for using an object
 */
extern bool use_object(object_type *o_ptr, bool *ident);

#endif /* INCLUDED_SCRIPT_H */

