%module event

%{

#include "event.h"


/* Array of the Python objects */
static PyObject *python_callbacks[MAX_EVENT];


void eat_callback(int sval)
{
	PyObject *func, *arglist;
	PyObject *result;

	/* Get the Python object */
	func = python_callbacks[CMD_EAT_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", sval);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	/* Void */
	return;
}


void kill_monster_callback(int r_idx)
{
	PyObject *func, *arglist;
	PyObject *result;

	/* Get the Python object */
	func = python_callbacks[KILL_MONSTER_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", r_idx);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	/* Void */
	return;
}


bool building_command_callback(int number, int action)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[BUILDING_COMMAND_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", number, action);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool player_move_callback(int y, int x)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[PLAYER_MOVE_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", y, x);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool cmd_open_callback(int y, int x)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[CMD_OPEN_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", y, x);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool cmd_search_callback(int y, int x)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[CMD_SEARCH_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", y, x);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool player_search_grid_callback(y, x)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[PLAYER_SEARCH_GRID_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", y, x);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool cmd_feeling_callback(int feeling)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[CMD_FEELING_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", feeling);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool use_skill_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[USE_SKILL_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool process_command_callback(char command)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[PROCESS_COMMAND_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(c)", command);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool cmd_go_up_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[CMD_GO_UP_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


long player_birth_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	long res = 0;

	/* Get the Python object */
	func = python_callbacks[PLAYER_BIRTH_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result))
		{
			res = PyInt_AsLong(result);
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


cptr get_script_window_line(int line)
{
	PyObject *func, *arglist;
	PyObject *result;
	cptr res = "";

	/* Get the Python object */
	func = python_callbacks[GET_SCRIPT_WINDOW_LINE_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", line);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		if (result && PyString_Check(result))
		{
			res = string_make(PyString_AsString(result));
		}

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


void new_game_callback(void)
{
	PyObject *func;
	PyObject *result;
	PyObject *arglist;

	/* Get the Python object */
	func = python_callbacks[NEW_GAME_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");
		
		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the argument */
		Py_XDECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}
}


void play_game_callback(void)
{
	PyObject *func;
	PyObject *result;
	PyObject *arglist;

	/* Get the Python object */
	func = python_callbacks[PLAY_GAME_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");
		
		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the argument */
		Py_XDECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}
}


cptr save_game_callback(void)
{
	PyObject *func;
	PyObject *result;
	cptr res = NULL;

	/* Get the Python object */
	func = python_callbacks[SAVE_GAME_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, NULL);

		if (result && PyString_Check(result))
		{
			res = string_make(PyString_AsString(result));
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


void load_game_callback(char *data)
{
	PyObject *func, *arglist;
	PyObject *result;

	/* Get the Python object */
	func = python_callbacks[LOAD_GAME_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(s)", data);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	/* Void */
	return;
}


void leave_level_callback(int level)
{
	PyObject *func, *arglist;
	PyObject *result;

	/* Get the Python object */
	func = python_callbacks[LEAVE_LEVEL_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", level);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	/* Void */
	return;
}


void enter_level_callback(int level)
{
	PyObject *func, *arglist;
	PyObject *result;

	/* Get the Python object */
	func = python_callbacks[ENTER_LEVEL_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", level);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	/* Void */
	return;
}


bool player_enter_grid_callback(int y, int x)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[PLAYER_ENTER_GRID_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", y, x);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool generate_level_callback(int level)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[GENERATE_LEVEL_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", level);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool generate_wilderness_callback(int y, int x)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[GENERATE_WILDERNESS_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", y, x);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool enter_wilderness_callback(int y, int x)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[ENTER_WILDERNESS_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", y, x);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool leave_wilderness_callback(int y, int x)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[LEAVE_WILDERNESS_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", y, x);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool monster_move_callback(int *mm, int m_idx)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;
	int i;

	/* Get the Python object */
	func = python_callbacks[MONSTER_MOVE_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", m_idx);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyList_Check(result))
		{
			for (i = 0; i < PyList_Size(result); i++)
			{
				mm[i] = PyInt_AsLong(PyList_GetItem(result,i));
			}

			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


void create_monster_callback(int m_idx)
{
	PyObject *func, *arglist;
	PyObject *result;

	/* Get the Python object */
	func = python_callbacks[CREATE_MONSTER_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", m_idx);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	/* void */
	return;
}


void delete_monster_callback(int m_idx)
{
	PyObject *func, *arglist;
	PyObject *result;

	/* Get the Python object */
	func = python_callbacks[DELETE_MONSTER_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", m_idx);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	/* void */
	return;
}


void copy_monster_callback(int i1, int i2)
{
	PyObject *func, *arglist;
	PyObject *result;

	/* Get the Python object */
	func = python_callbacks[COPY_MONSTER_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((ii))", i1, i2);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		/* Free the result */
		Py_XDECREF(result);
	}

	/* void */
	return;
}


char inkey_borg_callback(bool inkey_base, bool inkey_xtra,
                         bool inkey_flag, bool inkey_scan)
{
	PyObject *func, *arglist;
	PyObject *result;
	char res = 0;

	/* Get the Python object */
	func = python_callbacks[INKEY_BORG_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("((iiii))", inkey_base, inkey_xtra,
		                                  inkey_flag, inkey_scan);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result))
		{
			res = (char)PyInt_AsLong(result);
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


char inkey_callback(char key)
{
	PyObject *func, *arglist;
	PyObject *result;
	char res = 0;

	/* Get the Python object */
	func = python_callbacks[INKEY_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(i)", key);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result))
		{
			res = (char)PyInt_AsLong(result);
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool get_player_flags_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[GET_PLAYER_FLAGS_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


long sense_inventory_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	long res = 0;

	/* Get the Python object */
	func = python_callbacks[SENSE_INVENTORY_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result))
		{
			res = PyInt_AsLong(result);
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool destroy_object_callback(object_type *o_ptr, int number)
{
	PyObject *func, *arglist;
	PyObject *result;
	char _ptemp[128];
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[DESTROY_OBJECT_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		SWIG_MakePtr(_ptemp, o_ptr, SWIGTYPE_p_object_type);
		arglist = Py_BuildValue("((si))", _ptemp, number);

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool player_outfit_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[PLAYER_OUTFIT_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result))
		{
			if (PyInt_AsLong(result))
				res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool wilderness_init_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[WILDERNESS_INIT_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool free_object_kind_list_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[FREE_OBJECT_KIND_LIST_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


bool init_object_kind_list_callback(void)
{
	PyObject *func, *arglist;
	PyObject *result;
	bool res = FALSE;

	/* Get the Python object */
	func = python_callbacks[INIT_OBJECT_KIND_LIST_EVENT];

	/* Callback installed */
	if (func)
	{
		/* Build the argument-list */
		arglist = Py_BuildValue("(())");

		/* Call the object with the correct arguments */
		result = PyEval_CallObject(func, arglist);

		/* Free the arguments */
		Py_DECREF(arglist);

		if (result && PyInt_Check(result) && PyInt_AsLong(result))
		{
			res = TRUE;
		}

		/* Free the result */
		Py_XDECREF(result);
	}

	return res;
}


void set_callback(int event, PyObject *PyFunc)
{
	/* Dispose of previous callback */
	Py_XDECREF(python_callbacks[event]);

	/* Add a reference to new callback */
	Py_XINCREF(PyFunc);

	/* Remember new callback */
	python_callbacks[event] = PyFunc;
}

PyObject *get_callback(int event)
{
	/* Add a reference to new callback */
	Py_XINCREF(python_callbacks[event]);

	/* Remember new callback */
	return python_callbacks[event];
}

void remove_callback(int event)
{
	/* Dispose old callback */
	Py_XDECREF(python_callbacks[event]);

	python_callbacks[event] = NULL;
}

%}

%typemap(python,in) PyObject *PyFunc
{
	if (!PyCallable_Check($source))
	{
		PyErr_SetString(PyExc_TypeError, "Need a callable object!");
		return NULL;
	}
	$target = $source;
}

%typemap(python,out) PyObject *get_callback
{
	$target = $source;
}

typedef const object_type *object_type_ptr;

%include "event.h"

void set_callback(int event, PyObject *PyFunc);

PyObject *get_callback(int event);

void remove_callback(int event);
