/* File: iomod.c */

/* Purpose: Python interface to the input/output functions */

/*
 * Copyright (c) 2001 Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

#ifdef USE_SCRIPT

#include <Python.h>

#include "script.h"


/*
 * Print a message
 */
static PyObject* io_msg_print(PyObject *self, PyObject *args)
{
	char *buf;

	/* Parse arguments */
	if (!PyArg_ParseTuple(args, "s", &buf))
		return NULL;

	/* Print message */
	msg_print(buf);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}


/*
 * Make a (relevant?) sound
 */
static PyObject* io_sound(PyObject *self, PyObject *args)
{
	int val;

	/* Parse arguments */
	if (!PyArg_ParseTuple(args, "i", &val))
		return NULL;

	/* Make a sound */
	sound(val);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}


/*
 * Flush the screen, make a noise
 */
static PyObject* io_bell(PyObject *self, PyObject *args)
{
	/* Parse arguments */
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	/* Make a bell noice */
	bell(NULL);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* io_get_aim_dir(PyObject *self, PyObject *args)
{
	int dir;
	bool result;

	/* Parse arguments */
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	/* Get a direction */
	result = get_aim_dir(&dir);

	/* Check for abortion */
	if (!result) dir = -1;

	/* Return direction */
	return Py_BuildValue("i", dir);
}


/*
 * Clear to bottom of screen
 */
static PyObject* io_clear_from(PyObject *self, PyObject *args)
{
	int row;

	/* Parse arguments */
	if (!PyArg_ParseTuple(args, "i", &row))
		return NULL;

	/* Clear to bottom of screen */
	clear_from(row);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}


/*
 * Get a line of input
 */
static PyObject* io_get_line(PyObject *self, PyObject *args)
{
	static char get_line_tmp[1024];

	/* Parse arguments */
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	if (get_string("", get_line_tmp, 1024))
	{
		p_ptr->window |= (PW_SCRIPT);

		/* Return line */
		return Py_BuildValue("s", get_line_tmp);
	}
	else
	{
		/* Return nothing */
		Py_INCREF(Py_None);
		return Py_None;
	}
}


static PyObject* io_put_str(PyObject *self, PyObject *args)
{
	cptr str;
	int row, col;


	/* Parse arguments */
	if (!PyArg_ParseTuple(args, "sii", &str, &row, &col))
		return NULL;

	/* Print */
	put_str(str, row, col);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* io_inkey(PyObject *self, PyObject *args)
{
	char c;

	/* Parse arguments */
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	/* Get a key */
	c = inkey();

	return Py_BuildValue("c", c);
}


static PyObject* io_Term_putstr(PyObject *self, PyObject *args)
{
	int x, y, n;
	byte a;
	cptr s;

	/* Parse arguments */
	if (!PyArg_ParseTuple(args, "iiiis", &x, &y, &n, &a, &s))
		return NULL;

	Term_putstr(x, y, n, a, s);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* io_c_put_str(PyObject *self, PyObject *args)
{
	cptr str;
	int row, col;
	int attr;


	/* Parse arguments */
	if (!PyArg_ParseTuple(args, "isii", &attr, &str, &row, &col))
		return NULL;

	/* Print */
	c_put_str((byte)attr, str, row, col);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* io_move_cursor(PyObject *self, PyObject *args)
{
	int row, col;

	/* Parse arguments */
	if (!PyArg_ParseTuple(args, "ii", &row, &col))
		return NULL;

	move_cursor(row, col);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* io_screen_save(PyObject *self, PyObject *args)
{
	/* Parse arguments */
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	screen_save();

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* io_screen_load(PyObject *self, PyObject *args)
{
	/* Parse arguments */
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	screen_load();

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* io_prt(PyObject *self, PyObject *args)
{
	cptr str;
	int row, col;


	/* Parse arguments */
	if (!PyArg_ParseTuple(args, "sii", &str, &row, &col))
		return NULL;

	/* Print */
	prt(str, row, col);

	/* Return nothing */
	Py_INCREF(Py_None);
	return Py_None;
}


/*
 * Method table for all functions provided by the module
 */
static PyMethodDef ioMethods[] =
{
	{ "msg_print", io_msg_print, METH_VARARGS },
	{ "sound", io_sound, METH_VARARGS },
	{ "bell", io_bell, METH_VARARGS },
	{ "get_aim_dir", io_get_aim_dir, METH_VARARGS },
	{ "clear_from", io_clear_from, METH_VARARGS },
	{ "get_line", io_get_line, METH_VARARGS },
	{ "put_str", io_put_str, METH_VARARGS },
	{ "inkey", io_inkey, METH_VARARGS },
	{ "Term_putstr", io_Term_putstr, METH_VARARGS },
	{ "c_put_str", io_c_put_str, METH_VARARGS },
	{ "move_cursor", io_move_cursor, METH_VARARGS },
	{ "screen_save", io_screen_save, METH_VARARGS },
	{ "screen_load", io_screen_load, METH_VARARGS },
	{ "prt", io_prt, METH_VARARGS },
	{ NULL, NULL }
};


/*
 * Helper function.  Add an integer to a dictionary.
 */
static void add_constant(PyObject *dict, char *name, int value)
{
	PyObject *temp;

	/* Form an object from the value */
	temp = Py_BuildValue("i", value);

	/* Set item */
	PyDict_SetItemString(dict, name, temp);

	/* Delete object */
	Py_DECREF(temp);
}


/*
 * Initialize the I/O module
 */
void script_init_io(void)
{
	PyObject *m, *d;

	/* Tell the interpreter to insert the module */
	m = Py_InitModule("angband_io", ioMethods);

	/* Add some symbolic constants to the module */
	d = PyModule_GetDict(m);

	add_constant(d, "TERM_DARK", TERM_DARK);
	add_constant(d, "TERM_WHITE", TERM_WHITE);
	add_constant(d, "TERM_SLATE", TERM_SLATE);
	add_constant(d, "TERM_ORANGE", TERM_ORANGE);
	add_constant(d, "TERM_RED", TERM_RED);
	add_constant(d, "TERM_GREEN", TERM_GREEN);
	add_constant(d, "TERM_BLUE", TERM_BLUE);
	add_constant(d, "TERM_UMBER", TERM_UMBER);
	add_constant(d, "TERM_L_DARK", TERM_L_DARK);
	add_constant(d, "TERM_L_WHITE", TERM_L_WHITE);
	add_constant(d, "TERM_VIOLET", TERM_VIOLET);
	add_constant(d, "TERM_YELLOW", TERM_YELLOW);
	add_constant(d, "TERM_L_RED", TERM_L_RED);
	add_constant(d, "TERM_L_GREEN", TERM_L_GREEN);
	add_constant(d, "TERM_L_BLUE", TERM_L_BLUE);
	add_constant(d, "TERM_L_UMBER", TERM_L_UMBER);
}

#endif /* USE_SCRIPT */
