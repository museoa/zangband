/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3u-20001122-1458 (Alpha 5)
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

#define SWIGPYTHON
/***********************************************************************
 * common.swg
 *
 *     This file contains generic SWIG runtime support for pointer
 *     type checking as well as a few commonly used macros to control
 *     external linkage.
 *
 * Author : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (c) 1999-2000, The University of Chicago
 * 
 * This file may be freely redistributed without license or fee provided
 * this copyright message remains intact.
 ************************************************************************/

#include <string.h>

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
#define SWIGRUNTIME(a) SWIGEXPORT(a)
#else
#define SWIGRUNTIME(a) static a
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct swig_type_info {
  char  *name;                 
  void *(*converter)(void *);
  char  *str;
  struct swig_type_info  *next;
  struct swig_type_info  *prev;
} swig_type_info;

#ifdef SWIG_NOINCLUDE
SWIGEXPORT(swig_type_info *) SWIG_TypeRegister(swig_type_info *);
SWIGEXPORT(swig_type_info *) SWIG_TypeCheck(char *c, swig_type_info *);
SWIGEXPORT(void *) SWIG_TypeCast(swig_type_info *, void *);
#else

static swig_type_info *swig_type_list = 0;

/* Register a type mapping with the type-checking */
SWIGRUNTIME(swig_type_info *)
SWIG_TypeRegister(swig_type_info *ti)
{
  swig_type_info *tc, *head, *ret, *next;
  /* Check to see if this type has already been registered */
  tc = swig_type_list;
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
  ti->prev = swig_type_list;
  swig_type_list = ti;

  /* Build linked lists */
 l1:
  ret = head;
  tc = ti + 1;
  /* Patch up the rest of the links */
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
SWIGRUNTIME(swig_type_info *) 
SWIG_TypeCheck(char *c, swig_type_info *ty)
{
  swig_type_info *s;
  if (!ty) return 0;        /* Void pointer */
  s = ty->next;             /* First element always just a name */
  while (s) {
    if (strcmp(s->name,c) == 0) {
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

/* Cast a pointer (needed for C++ inheritance */
SWIGRUNTIME(void *) 
SWIG_TypeCast(swig_type_info *ty, void *ptr) 
{
  if ((!ty) || (!ty->converter)) return ptr;
  return (*ty->converter)(ptr);
}

/* Search for a swig_type_info structure */
SWIGRUNTIME(void *)
SWIG_TypeQuery(const char *name) {
  swig_type_info *ty = swig_type_list;
  while (ty) {
    if (ty->str && (strcmp(name,ty->str) == 0)) return ty;
    if (ty->name && (strcmp(name,ty->name) == 0)) return ty;
    ty = ty->prev;
  }
  return 0;
}

#endif

#ifdef __cplusplus
}
#endif



/***********************************************************************
 * python.swg
 *
 *     This file contains the runtime support for Python modules
 *     and includes code for managing global variables and pointer
 *     type checking.
 *
 * Author : David Beazley (beazley@cs.uchicago.edu)
 ************************************************************************/

#include <stdlib.h>
#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWIG_PY_INT     1
#define SWIG_PY_FLOAT   2
#define SWIG_PY_STRING  3
#define SWIG_PY_POINTER 4

/* Constant information structure */
typedef struct swig_const_info {
    int type;
    char *name;
    long lvalue;
    double dvalue;
    void   *pvalue;
    swig_type_info **ptype;
} swig_const_info;

#ifdef SWIG_NOINCLUDE

SWIGEXPORT(PyObject *)        SWIG_newvarlink();
SWIGEXPORT(void)              SWIG_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
SWIGEXPORT(int)               SWIG_ConvertPtr(PyObject *, void **, swig_type_info *, int);
SWIGEXPORT(void)              SWIG_MakePtr(char *c, void *, swig_type_info *);
SWIGEXPORT(PyObject *)        SWIG_NewPointerObj(void *, swig_type_info *);
SWIGEXPORT(void)              SWIG_InstallConstants(PyObject *d, swig_const_info constants[]);

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
SWIGRUNTIME(PyObject *)
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

SWIGRUNTIME(void)
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
/* Convert a pointer value */
SWIGRUNTIME(int)
SWIG_ConvertPtr(PyObject *obj, void **ptr, swig_type_info *ty, int flags) {
  unsigned long p;
  register int d;
  swig_type_info *tc;
  char  *c;
  static PyObject *SWIG_this = 0;
  int    newref = 0;

  if (!obj || (obj == Py_None)) {
    *ptr = 0;
    return 0;
  }
#ifdef SWIG_COBJECT_TYPES
  if (!(PyCObject_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_InternFromString("this");
    obj = PyObject_GetAttr(obj,SWIG_this);
    newref = 1;
    if (!obj) goto type_error;
    if (!PyCObject_Check(obj)) {
      Py_DECREF(obj);
      goto type_error;
    }
  } 
  *ptr = PyCObject_AsVoidPtr(obj);
  c = (char *) PyCObject_GetDesc(obj);
  if (newref) Py_DECREF(obj);
  goto cobject;
#else
  if (!(PyString_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_InternFromString("this");
    obj = PyObject_GetAttr(obj,SWIG_this);
    newref = 1;
    if (!obj) goto type_error;
    if (!PyString_Check(obj)) {
      Py_DECREF(obj);
      goto type_error;
    }
  } 
  c = PyString_AsString(obj);
  p = 0;
  /* Pointer values must start with leading underscore */
  if (*c != '_') {
    *ptr = (void *) 0;
    if (strcmp(c,"NULL") == 0) {
      if (newref) Py_DECREF(obj);
      return 0;
    } else {
      if (newref) Py_DECREF(obj);
      goto type_error;
    }
  }
  c++;
  /* Extract hex value from pointer */
  while ((d = *c)) {
    if ((d >= '0') && (d <= '9'))
      p = (p << 4) + (d - '0');
    else if ((d >= 'a') && (d <= 'f'))
      p = (p << 4) + (d - ('a'-10));
    else
      break; 
    c++;
  }
  *ptr = (void *) p;
  if (newref) Py_DECREF(obj);
#endif

#ifdef SWIG_COBJECT_TYPES
cobject:
#endif

  if (ty) {
    tc = SWIG_TypeCheck(c,ty);
    if (!tc) goto type_error;
    *ptr = SWIG_TypeCast(tc,(void*)p);
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
SWIGRUNTIME(void) 
SWIG_MakePtr(char *c, void *ptr, swig_type_info *ty) {
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
SWIGRUNTIME(PyObject *)
SWIG_NewPointerObj(void *ptr, swig_type_info *type) {
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
SWIGRUNTIME(void)
SWIG_InstallConstants(PyObject *d, swig_const_info constants[]) {
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



/* -------- TYPES TABLE (BEGIN) -------- */

#define  SWIGTYPE_a_63__u32b swig_types[0] 
static swig_type_info *swig_types[2];

/* -------- TYPES TABLE (END) -------- */


/*-----------------------------------------------
              @(target):= randomc.so
  ------------------------------------------------*/
#define SWIG_init    initrandomc

#define SWIG_name    "randomc"


#include "angband.h"


	s32b rand_int_temp(u32b m)
	{
		return randint0(m);
	}
extern s32b rand_int_temp(u32b );

	s32b randint_temp(u32b m)
	{
		return randint1(m);
	}
extern s32b randint_temp(u32b );

	/*
	 * Generates a random long integer X where A<=X<=B
	 * The integer X falls along a uniform distribution.
	 * Note: rand_range(0,N-1) == rand_int(N)
	 */
	s32b rand_range_temp(u32b min, u32b max)
	{
		return rand_range(min, max);
	}
extern s32b rand_range_temp(u32b ,u32b );
extern bool Rand_quick;
extern u32b Rand_value;
extern u16b Rand_place;
extern u32b Rand_state[63];
extern byte quick_rand_place;
extern void Rand_state_init(u32b );
extern s32b Rand_mod(s32b );
extern s32b Rand_div(u32b );
extern s16b randnor(int ,int );
extern s16b damroll(int ,int );
extern s16b maxroll(int ,int );
extern bool quick_rand();
extern void quick_rand_add();
#ifdef __cplusplus
extern "C" {
#endif
static PyObject *_wrap_rand_int(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    u32b arg0 ;
    s32b result ;
    
    if(!PyArg_ParseTuple(args,"i:rand_int",&arg0)) return NULL;
    result = (s32b )rand_int_temp(arg0);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_randint(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    u32b arg0 ;
    s32b result ;
    
    if(!PyArg_ParseTuple(args,"i:randint",&arg0)) return NULL;
    result = (s32b )randint_temp(arg0);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_rand_range(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    u32b arg0 ;
    u32b arg1 ;
    s32b result ;
    
    if(!PyArg_ParseTuple(args,"ii:rand_range",&arg0,&arg1)) return NULL;
    result = (s32b )rand_range_temp(arg0,arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static int _wrap_Rand_quick_set(PyObject *val) {
    bool  tval ;
    
    tval = (bool ) PyInt_AsLong(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'Rand_quick'(bool )");
        return 1; 
    }
    Rand_quick = tval;
    return 0;
}


static PyObject *_wrap_Rand_quick_get() {
    PyObject *pyobj;
    
    pyobj = PyInt_FromLong((long) Rand_quick);
    return pyobj;
}


static int _wrap_Rand_value_set(PyObject *val) {
    u32b  tval ;
    
    tval = (u32b ) PyInt_AsLong(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'Rand_value'(u32b )");
        return 1; 
    }
    Rand_value = tval;
    return 0;
}


static PyObject *_wrap_Rand_value_get() {
    PyObject *pyobj;
    
    pyobj = PyInt_FromLong((long) Rand_value);
    return pyobj;
}


static int _wrap_Rand_place_set(PyObject *val) {
    u16b  tval ;
    
    tval = (u16b ) PyInt_AsLong(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'Rand_place'(u16b )");
        return 1; 
    }
    Rand_place = tval;
    return 0;
}


static PyObject *_wrap_Rand_place_get() {
    PyObject *pyobj;
    
    pyobj = PyInt_FromLong((long) Rand_place);
    return pyobj;
}


static int _wrap_Rand_state_set(PyObject *val) {
    PyErr_SetString(PyExc_TypeError,"Variable Rand_state is read-only.");
    return 1;
    return 0;
}


static PyObject *_wrap_Rand_state_get() {
    PyObject *pyobj;
    
    pyobj = SWIG_NewPointerObj((void *)Rand_state, SWIGTYPE_a_63__u32b);
    return pyobj;
}


static int _wrap_quick_rand_place_set(PyObject *val) {
    byte  tval ;
    
    tval = (byte ) PyInt_AsLong(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'quick_rand_place'(byte )");
        return 1; 
    }
    quick_rand_place = tval;
    return 0;
}


static PyObject *_wrap_quick_rand_place_get() {
    PyObject *pyobj;
    
    pyobj = PyInt_FromLong((long) quick_rand_place);
    return pyobj;
}


static PyObject *_wrap_Rand_state_init(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    u32b arg0 ;
    
    if(!PyArg_ParseTuple(args,"i:Rand_state_init",&arg0)) return NULL;
    Rand_state_init(arg0);
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_Rand_mod(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    s32b arg0 ;
    s32b result ;
    
    if(!PyArg_ParseTuple(args,"i:Rand_mod",&arg0)) return NULL;
    result = (s32b )Rand_mod(arg0);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Rand_div(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    u32b arg0 ;
    s32b result ;
    
    if(!PyArg_ParseTuple(args,"i:Rand_div",&arg0)) return NULL;
    result = (s32b )Rand_div(arg0);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_randnor(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    int arg0 ;
    int arg1 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"ii:randnor",&arg0,&arg1)) return NULL;
    result = (s16b )randnor(arg0,arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_damroll(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    int arg0 ;
    int arg1 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"ii:damroll",&arg0,&arg1)) return NULL;
    result = (s16b )damroll(arg0,arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_maxroll(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    int arg0 ;
    int arg1 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"ii:maxroll",&arg0,&arg1)) return NULL;
    result = (s16b )maxroll(arg0,arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_quick_rand(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    bool result ;
    
    if(!PyArg_ParseTuple(args,":quick_rand")) return NULL;
    result = (bool )quick_rand();
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_quick_rand_add(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    
    if(!PyArg_ParseTuple(args,":quick_rand_add")) return NULL;
    quick_rand_add();
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyMethodDef randomcMethods[] = {
	 { "rand_int", _wrap_rand_int, METH_VARARGS },
	 { "randint", _wrap_randint, METH_VARARGS },
	 { "rand_range", _wrap_rand_range, METH_VARARGS },
	 { "Rand_state_init", _wrap_Rand_state_init, METH_VARARGS },
	 { "Rand_mod", _wrap_Rand_mod, METH_VARARGS },
	 { "Rand_div", _wrap_Rand_div, METH_VARARGS },
	 { "randnor", _wrap_randnor, METH_VARARGS },
	 { "damroll", _wrap_damroll, METH_VARARGS },
	 { "maxroll", _wrap_maxroll, METH_VARARGS },
	 { "quick_rand", _wrap_quick_rand, METH_VARARGS },
	 { "quick_rand_add", _wrap_quick_rand_add, METH_VARARGS },
	 { NULL, NULL }
};

#ifdef __cplusplus
}
#endif

/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (BEGIN) -------- */

static swig_type_info _swigt__a_63__u32b[] = {{"_a_63__u32b", 0, "u32b [63]"},{"_a_63__u32b"},{0}};

static swig_type_info *swig_types_initial[] = {
_swigt__a_63__u32b, 
0
};


/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (END) -------- */

static swig_const_info swig_const_table[] = {
    { SWIG_PY_INT,     "RAND_DEG", (long) 63, 0, 0, 0},
{0}};

static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT(void) initrandomc(void) {
    PyObject *m, *d;
    int i;
    SWIG_globals = SWIG_newvarlink();
    m = Py_InitModule("randomc", randomcMethods);
    d = PyModule_GetDict(m);
    for (i = 0; swig_types_initial[i]; i++) {
        swig_types[i] = SWIG_TypeRegister(swig_types_initial[i]);
    }
    PyDict_SetItemString(d,"cvar", SWIG_globals);
    SWIG_addvarlink(SWIG_globals,"Rand_quick",_wrap_Rand_quick_get, _wrap_Rand_quick_set);
    SWIG_addvarlink(SWIG_globals,"Rand_value",_wrap_Rand_value_get, _wrap_Rand_value_set);
    SWIG_addvarlink(SWIG_globals,"Rand_place",_wrap_Rand_place_get, _wrap_Rand_place_set);
    SWIG_addvarlink(SWIG_globals,"Rand_state",_wrap_Rand_state_get, _wrap_Rand_state_set);
    SWIG_addvarlink(SWIG_globals,"quick_rand_place",_wrap_quick_rand_place_get, _wrap_quick_rand_place_set);
    SWIG_InstallConstants(d,swig_const_table);
}

