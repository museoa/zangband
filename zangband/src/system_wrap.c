/*
 * FILE : .\src\system_wrap.c
 * 
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3u-20000417-1444 (Alpha 2)
 * 
 * Portions Copyright (c) 1995-1999
 * The University of Utah, The Regents of the University of California, and
 * The University of Chicago.  Permission is granted to use and distribute this
 * file in any manner provided this notice remains intact.
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 *
 */


#define SWIGCODE
/* Implementation : PYTHON */

#define SWIGPYTHON
/***********************************************************************
 * python.swg
 *
 *     This file contains the runtime support for Python modules
 *     and includes code for managing global variables and pointer
 *     type checking.
 *
 * Author : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (c) 1999-2000, The University of Chicago
 * 
 * This file may be freely redistributed without license or fee provided
 * this copyright message remains intact.
 ************************************************************************/

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "Python.h"

#if defined(_WIN32) || defined(__WIN32__)
#       if defined(_MSC_VER)
#               if defined(STATIC_LINKED)
#                       define SWIGEXPORT(a) a
#               else
#                       define SWIGEXPORT(a) __declspec(dllexport) a
#               endif
#       else
#               if defined(__BORLANDC__)
#                       define SWIGEXPORT(a) a _export
#               else
#                       define SWIGEXPORT(a) a
#       endif
#endif
#else
#       define SWIGEXPORT(a) a
#endif

#ifdef SWIG_GLOBAL
#define SWIGSTATICRUNTIME(a) SWIGEXPORT(a)
#else
#define SWIGSTATICRUNTIME(a) static a
#endif

/* Type information structure */

typedef struct _swig_type_info {
  char  *name;                 
  void *(*converter)(void *);
  struct _swig_type_info  *next;
  struct _swig_type_info  *prev;
} _swig_type_info;

/* Constant information structure */
typedef struct _swig_const_info {
    int type;
    char *name;
    long lvalue;
    double dvalue;
    void   *pvalue;
    _swig_type_info **ptype;
} _swig_const_info;

#define SWIG_PY_INT     1
#define SWIG_PY_FLOAT   2
#define SWIG_PY_STRING  3
#define SWIG_PY_POINTER 4

#ifdef SWIG_NOINCLUDE

SWIGEXPORT(PyObject *)        SWIG_newvarlink();
SWIGEXPORT(void)              SWIG_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
SWIGEXPORT(_swig_type_info *) SWIG_TypeRegister(_swig_type_info *);
SWIGEXPORT(_swig_type_info *) SWIG_TypeCheck(char *, _swig_type_info *);
SWIGEXPORT(int)               SWIG_ConvertPtr(PyObject *, void **, _swig_type_info *, int);
SWIGEXPORT(void)              SWIG_MakePtr(char *c, void *, _swig_type_info *);
SWIGEXPORT(PyObject *)        SWIG_NewPointerObj(void *, _swig_type_info *);
SWIGEXPORT(void)              SWIG_InstallConstants(PyObject *d, _swig_const_info constants[]);

/* External declarations when using runtime libraries */

#else

/* -----------------------------------------------------------------------------
 * global variable support code.
 * ----------------------------------------------------------------------------- */

typedef struct swig_globalvar {   
  char       *name;                  /* Name of global variable */
  PyObject *(*get_attr)(void);       /* Return the current value */
  int       (*set_attr)(PyObject *); /* Set the value */
  struct swig_globalvar *next;
} swig_globalvar;

typedef struct swig_varlinkobject {
  PyObject_HEAD
  swig_globalvar *vars;
} swig_varlinkobject;

static PyObject *
swig_varlink_repr(swig_varlinkobject *v) {
  v = v;
  return PyString_FromString("<Global variables>");
}

static int
swig_varlink_print(swig_varlinkobject *v, FILE *fp, int flags) {
  swig_globalvar  *var;
  flags = flags;
  fprintf(fp,"Global variables { ");
  for (var = v->vars; var; var=var->next) {
    fprintf(fp,"%s", var->name);
    if (var->next) fprintf(fp,", ");
  }
  fprintf(fp," }\n");
  return 0;
}

static PyObject *
swig_varlink_getattr(swig_varlinkobject *v, char *n) {
  swig_globalvar *var = v->vars;
  while (var) {
    if (strcmp(var->name,n) == 0) {
      return (*var->get_attr)();
    }
    var = var->next;
  }
  PyErr_SetString(PyExc_NameError,"Unknown C global variable");
  return NULL;
}

static int
swig_varlink_setattr(swig_varlinkobject *v, char *n, PyObject *p) {
  swig_globalvar *var = v->vars;
  while (var) {
    if (strcmp(var->name,n) == 0) {
      return (*var->set_attr)(p);
    }
    var = var->next;
  }
  PyErr_SetString(PyExc_NameError,"Unknown C global variable");
  return 1;
}

statichere PyTypeObject varlinktype = {
  PyObject_HEAD_INIT(0)              
  0,
  "swigvarlink",                      /* Type name    */
  sizeof(swig_varlinkobject),         /* Basic size   */
  0,                                  /* Itemsize     */
  0,                                  /* Deallocator  */ 
  (printfunc) swig_varlink_print,     /* Print        */
  (getattrfunc) swig_varlink_getattr, /* get attr     */
  (setattrfunc) swig_varlink_setattr, /* Set attr     */
  0,                                  /* tp_compare   */
  (reprfunc) swig_varlink_repr,       /* tp_repr      */    
  0,                                  /* tp_as_number */
  0,                                  /* tp_as_mapping*/
  0,                                  /* tp_hash      */
};

/* Create a variable linking object for use later */
SWIGSTATICRUNTIME(PyObject *)
SWIG_newvarlink(void) {
  swig_varlinkobject *result = 0;
  result = PyMem_NEW(swig_varlinkobject,1);
  varlinktype.ob_type = &PyType_Type;    /* Patch varlinktype into a PyType */
  result->ob_type = &varlinktype;
  result->vars = 0;
  result->ob_refcnt = 0;
  Py_XINCREF((PyObject *) result);
  return ((PyObject*) result);
}

SWIGSTATICRUNTIME(void)
SWIG_addvarlink(PyObject *p, char *name,
	   PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p)) {
  swig_varlinkobject *v;
  swig_globalvar *gv;
  v= (swig_varlinkobject *) p;
  gv = (swig_globalvar *) malloc(sizeof(swig_globalvar));
  gv->name = (char *) malloc(strlen(name)+1);
  strcpy(gv->name,name);
  gv->get_attr = get_attr;
  gv->set_attr = set_attr;
  gv->next = v->vars;
  v->vars = gv;
}

/* -----------------------------------------------------------------------------
 * Pointer type-checking
 * ----------------------------------------------------------------------------- */

static _swig_type_info *swig_types = 0;

/* Register type mappings with the type-checker */
SWIGSTATICRUNTIME(_swig_type_info *)
SWIG_TypeRegister(_swig_type_info *ti) {
  _swig_type_info *tc, *head, *ret, *next;
  /* Check to see if this type has already been registered */
  tc = swig_types;
  while (tc) {
    if (strcmp(tc->name, ti->name) == 0) {
      /* Already exists in the table.  Just add additional types to the list */
      head = tc;
      next = tc->next;
      goto l1;
    }
    tc = tc->prev;
  }
  head = ti;
  next = 0;

  /* Place in list */
  ti->prev = swig_types;
  swig_types = ti;

  /* Build linked lists */
 l1:
  ret = head;
  tc = ti + 1;
  while (tc->name) {
    head->next = tc;
    tc->prev = head;
    head = tc;
    tc++;
  }
  head->next = next;
  return ret;
}

/* Check the typename */
SWIGSTATICRUNTIME(_swig_type_info *) 
SWIG_TypeCheck(char *c, _swig_type_info *ty) {
  _swig_type_info *s;
  char *sn;
  if (!ty) return 0;        /* Void pointer */
  s = ty->next;             /* First element is always just the name */
  while (s) {
    sn = s->name;
    if ((c == sn) || ((*c == *sn) && (strcmp(sn,c) == 0))) {
      if (s == ty->next) return s;
      /* Move s to the top of the linked list */
      s->prev->next = s->next;
      if (s->next) {
	s->next->prev = s->prev;
      }
      /* Insert s as second element in the list */
      s->next = ty->next;
      if (ty->next) ty->next->prev = s;
      ty->next = s;
      return s;
    }
    s = s->next;
  }
  return 0;
}

/* Convert a pointer value */
SWIGSTATICRUNTIME(int)
SWIG_ConvertPtr(PyObject *obj, void **ptr, _swig_type_info *ty, int flags) {
  unsigned long p;
  register int d;
  _swig_type_info *tc;
  char  *c;
  static PyObject *SWIG_this = 0;

  if (!obj || (obj == Py_None)) {
    *ptr = 0;
    return 0;
  }
#ifdef SWIG_COBJECT_TYPES
  if (!(PyCObject_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_InternFromString("this");
    obj = PyObject_GetAttr(obj,SWIG_this);
    if ((!obj) || !(PyCObject_Check(obj))) goto type_error;
  } 
  *ptr = PyCObject_AsVoidPtr(obj);
  c = (char *) PyCObject_GetDesc(obj);
  goto cobject;
#else
  if (!(PyString_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_InternFromString("this");
    obj = PyObject_GetAttr(obj,SWIG_this);
    if ((!obj) || !(PyString_Check(obj))) goto type_error;
  } 
  c = PyString_AsString(obj);
  p = 0;
  /* Pointer values must start with leading underscore */
  if (*c != '_') {
    *ptr = (void *) 0;
    if (strcmp(c,"NULL") == 0) return 0;
  }
  c++;
  /* Extract hex value from pointer */
  while (d = *c) {
    if ((d >= '0') && (d <= '9'))
      p = (p << 4) + (d - '0');
    else if ((d >= 'a') && (d <= 'f'))
      p = (p << 4) + (d - ('a'-10));
    else
      break; 
    c++;
  }
  *ptr = (void *) p;
#endif

#ifdef SWIG_COBJECT_TYPES
cobject:
#endif

  if (ty) {
    tc = SWIG_TypeCheck(c,ty);
    if (!tc) goto type_error;
    if (tc->converter) {
      *ptr = (*tc->converter)((void *) p);
    }
  }
  return 0;

type_error:
  if (flags) {
    if (ty) {
      char *temp = (char *) malloc(64+strlen(ty->name));
      sprintf(temp,"Type error. Expected %s", ty->name);
      PyErr_SetString(PyExc_TypeError, temp);
      free((char *) temp);
    } else {
      PyErr_SetString(PyExc_TypeError,"Expected a pointer");
    }
  }
  return -1;
}

/* Take a pointer and convert it to a string */
SWIGSTATICRUNTIME(void) 
SWIG_MakePtr(char *c, void *ptr, _swig_type_info *ty) {
  static char hex[17] = "0123456789abcdef";
  unsigned long p, s;
  char result[32], *r; 
  r = result;
  p = (unsigned long) ptr;
  if (p > 0) {
    while (p > 0) {
      s = p & 0xf;
      *(r++) = hex[s];
      p = p >> 4;
    }
    *r = '_';
    while (r >= result)
      *(c++) = *(r--);
    strcpy (c, ty->name);
  } else {
    strcpy (c, "NULL");
  }
}

/* Create a new pointer object */
SWIGSTATICRUNTIME(PyObject *)
SWIG_NewPointerObj(void *ptr, _swig_type_info *type) {
  char result[512];
  PyObject *robj;
  if (!ptr) {
    Py_INCREF(Py_None);
    return Py_None;
  }
#ifdef SWIG_COBJECT_TYPES
  robj = PyCObject_FromVoidPtrAndDesc((void *) ptr, type->name, NULL);
#else
  SWIG_MakePtr(result,ptr,type);
  robj = PyString_FromString(result);
#endif
  return robj;
}

/* Install Constants */
SWIGSTATICRUNTIME(void)
SWIG_InstallConstants(PyObject *d, _swig_const_info constants[]) {
  int i;
  PyObject *obj;
  for (i = 0; constants[i].type; i++) {
    switch(constants[i].type) {
    case SWIG_PY_INT:
      obj = PyInt_FromLong(constants[i].lvalue);
      break;
    case SWIG_PY_FLOAT:
      obj = PyFloat_FromDouble(constants[i].dvalue);
      break;
    case SWIG_PY_STRING:
      obj = PyString_FromString((char *) constants[i].pvalue);
      break;
    case SWIG_PY_POINTER:
      obj = SWIG_NewPointerObj(constants[i].pvalue, *(constants[i]).ptype);
      break;
    default:
      obj = 0;
      break;
    }
    if (obj) {
      PyDict_SetItemString(d,constants[i].name,obj);
      Py_DECREF(obj);
    }
  }
}

#endif

#ifdef __cplusplus
}
#endif


/* ---- TYPES TABLE (BEGIN) ---- */
static _swig_type_info *_swig_types[1];
/* ---- TYPES TABLE (END) ---- */

#define SWIG_init    initsystemc

#define SWIG_name    "systemc"


#include "angband.h"

extern void remove_loc();
extern void plog(cptr );
extern void quit(cptr );
extern void core(cptr );
#ifdef __cplusplus
extern "C" {
#endif
static int _wrap_ANGBAND_SYS_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_SYS'(cptr )");
        return 1; 
    }
    if (ANGBAND_SYS) free(ANGBAND_SYS);
    ANGBAND_SYS = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_SYS,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_SYS_get() {
    PyObject  *pyobj;
    if (ANGBAND_SYS)
        pyobj = PyString_FromString(ANGBAND_SYS);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_GRAF_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_GRAF'(cptr )");
        return 1; 
    }
    if (ANGBAND_GRAF) free(ANGBAND_GRAF);
    ANGBAND_GRAF = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_GRAF,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_GRAF_get() {
    PyObject  *pyobj;
    if (ANGBAND_GRAF)
        pyobj = PyString_FromString(ANGBAND_GRAF);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR) free(ANGBAND_DIR);
    ANGBAND_DIR = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR)
        pyobj = PyString_FromString(ANGBAND_DIR);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_APEX_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_APEX'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_APEX) free(ANGBAND_DIR_APEX);
    ANGBAND_DIR_APEX = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_APEX,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_APEX_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_APEX)
        pyobj = PyString_FromString(ANGBAND_DIR_APEX);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_BONE_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_BONE'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_BONE) free(ANGBAND_DIR_BONE);
    ANGBAND_DIR_BONE = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_BONE,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_BONE_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_BONE)
        pyobj = PyString_FromString(ANGBAND_DIR_BONE);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_DATA_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_DATA'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_DATA) free(ANGBAND_DIR_DATA);
    ANGBAND_DIR_DATA = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_DATA,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_DATA_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_DATA)
        pyobj = PyString_FromString(ANGBAND_DIR_DATA);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_EDIT_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_EDIT'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_EDIT) free(ANGBAND_DIR_EDIT);
    ANGBAND_DIR_EDIT = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_EDIT,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_EDIT_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_EDIT)
        pyobj = PyString_FromString(ANGBAND_DIR_EDIT);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_SCRIPT_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_SCRIPT'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_SCRIPT) free(ANGBAND_DIR_SCRIPT);
    ANGBAND_DIR_SCRIPT = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_SCRIPT,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_SCRIPT_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_SCRIPT)
        pyobj = PyString_FromString(ANGBAND_DIR_SCRIPT);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_FILE_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_FILE'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_FILE) free(ANGBAND_DIR_FILE);
    ANGBAND_DIR_FILE = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_FILE,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_FILE_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_FILE)
        pyobj = PyString_FromString(ANGBAND_DIR_FILE);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_HELP_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_HELP'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_HELP) free(ANGBAND_DIR_HELP);
    ANGBAND_DIR_HELP = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_HELP,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_HELP_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_HELP)
        pyobj = PyString_FromString(ANGBAND_DIR_HELP);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_INFO_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_INFO'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_INFO) free(ANGBAND_DIR_INFO);
    ANGBAND_DIR_INFO = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_INFO,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_INFO_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_INFO)
        pyobj = PyString_FromString(ANGBAND_DIR_INFO);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_SAVE_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_SAVE'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_SAVE) free(ANGBAND_DIR_SAVE);
    ANGBAND_DIR_SAVE = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_SAVE,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_SAVE_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_SAVE)
        pyobj = PyString_FromString(ANGBAND_DIR_SAVE);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_USER_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_USER'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_USER) free(ANGBAND_DIR_USER);
    ANGBAND_DIR_USER = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_USER,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_USER_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_USER)
        pyobj = PyString_FromString(ANGBAND_DIR_USER);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static int _wrap_ANGBAND_DIR_XTRA_set(PyObject *val) {
    char  *tval;
    tval = (char *) PyString_AsString(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'ANGBAND_DIR_XTRA'(cptr )");
        return 1; 
    }
    if (ANGBAND_DIR_XTRA) free(ANGBAND_DIR_XTRA);
    ANGBAND_DIR_XTRA = (char *) malloc(strlen(tval)+1);
    strcpy((char *)ANGBAND_DIR_XTRA,tval);
    return 0;
}

static PyObject *_wrap_ANGBAND_DIR_XTRA_get() {
    PyObject  *pyobj;
    if (ANGBAND_DIR_XTRA)
        pyobj = PyString_FromString(ANGBAND_DIR_XTRA);
    else pyobj = PyString_FromString("(NULL)");
    return pyobj;
}

static PyObject *_wrap_remove_loc(PyObject *self, PyObject *args) {
    PyObject  *_resultobj;
    self = self;
    if(!PyArg_ParseTuple(args,":remove_loc")) 
        return NULL;
    remove_loc();
    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyObject *_wrap_plog(PyObject *self, PyObject *args) {
    cptr  _arg0;
    PyObject  *_resultobj;
    self = self;
    if(!PyArg_ParseTuple(args,"s:plog",&_arg0)) 
        return NULL;
    plog(_arg0);
    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyObject *_wrap_quit(PyObject *self, PyObject *args) {
    cptr  _arg0;
    PyObject  *_resultobj;
    self = self;
    if(!PyArg_ParseTuple(args,"s:quit",&_arg0)) 
        return NULL;
    quit(_arg0);
    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyObject *_wrap_core(PyObject *self, PyObject *args) {
    cptr  _arg0;
    PyObject  *_resultobj;
    self = self;
    if(!PyArg_ParseTuple(args,"s:core",&_arg0)) 
        return NULL;
    core(_arg0);
    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyMethodDef systemcMethods[] = {
	 { "core", _wrap_core, METH_VARARGS },
	 { "quit", _wrap_quit, METH_VARARGS },
	 { "plog", _wrap_plog, METH_VARARGS },
	 { "remove_loc", _wrap_remove_loc, METH_VARARGS },
	 { NULL, NULL }
};
#ifdef __cplusplus
}
#endif

static _swig_type_info *_swig_types_initial[] = {
0
};

static _swig_const_info _swig_const_table[] = {
{0}};

static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT(void) initsystemc(void) {
	 PyObject *m, *d;
	 SWIG_globals = SWIG_newvarlink();
	 m = Py_InitModule("systemc", systemcMethods);
	 d = PyModule_GetDict(m);
    {
        int i;
        for (i = 0; _swig_types_initial[i]; i++) {
            _swig_types[i] = SWIG_TypeRegister(_swig_types_initial[i]);
        }
    }
	 PyDict_SetItemString(d,"cvar", SWIG_globals);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_SYS",_wrap_ANGBAND_SYS_get, _wrap_ANGBAND_SYS_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_GRAF",_wrap_ANGBAND_GRAF_get, _wrap_ANGBAND_GRAF_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR",_wrap_ANGBAND_DIR_get, _wrap_ANGBAND_DIR_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_APEX",_wrap_ANGBAND_DIR_APEX_get, _wrap_ANGBAND_DIR_APEX_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_BONE",_wrap_ANGBAND_DIR_BONE_get, _wrap_ANGBAND_DIR_BONE_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_DATA",_wrap_ANGBAND_DIR_DATA_get, _wrap_ANGBAND_DIR_DATA_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_EDIT",_wrap_ANGBAND_DIR_EDIT_get, _wrap_ANGBAND_DIR_EDIT_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_SCRIPT",_wrap_ANGBAND_DIR_SCRIPT_get, _wrap_ANGBAND_DIR_SCRIPT_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_FILE",_wrap_ANGBAND_DIR_FILE_get, _wrap_ANGBAND_DIR_FILE_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_HELP",_wrap_ANGBAND_DIR_HELP_get, _wrap_ANGBAND_DIR_HELP_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_INFO",_wrap_ANGBAND_DIR_INFO_get, _wrap_ANGBAND_DIR_INFO_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_SAVE",_wrap_ANGBAND_DIR_SAVE_get, _wrap_ANGBAND_DIR_SAVE_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_USER",_wrap_ANGBAND_DIR_USER_get, _wrap_ANGBAND_DIR_USER_set);
	 SWIG_addvarlink(SWIG_globals,"ANGBAND_DIR_XTRA",_wrap_ANGBAND_DIR_XTRA_get, _wrap_ANGBAND_DIR_XTRA_set);
    SWIG_InstallConstants(d,_swig_const_table);

}
