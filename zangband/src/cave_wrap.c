/*
 * FILE : .\src\cave_wrap.c
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
#define  SWIGTYPE_cave_type_p _swig_types[0] 
#define  SWIGTYPE_vault_type_p _swig_types[1] 
#define  SWIGTYPE_errr_p _swig_types[2] 
static _swig_type_info *_swig_types[4];
/* ---- TYPES TABLE (END) ---- */

#define SWIG_init    initcavec

#define SWIG_name    "cavec"


#include "angband.h"


	errr load_layout(cptr name)
	{
		init_flags = INIT_CREATE_DUNGEON | INIT_ASSIGN;
		return process_dungeon_file(name, 0, 0, MAX_HGT, MAX_WID);
	}

	cave_type *tile(int y, int x)
	{
		return &cave[y][x];
	}
#ifdef __cplusplus
extern "C" {
#endif
static PyObject *_wrap_load_layout(PyObject *self, PyObject *args) {
    errr  *_result;
    PyObject  *_resultobj;
    char  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"s:load_layout",&_arg0)) 
        return NULL;
    _result = (errr *) malloc(sizeof(errr ));
    *(_result) = load_layout(_arg0);
    _resultobj = SWIG_NewPointerObj((void *)_result, SWIGTYPE_errr_p);
    return _resultobj;
}

static PyObject *_wrap_set_feat(PyObject *self, PyObject *args) {
    int  _arg0,_arg1,_arg2;
    PyObject  *_resultobj;
    self = self;
    if(!PyArg_ParseTuple(args,"iii:set_feat",&_arg0,&_arg1,&_arg2)) 
        return NULL;
    cave_set_feat(_arg0,_arg1,_arg2);
    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyObject *_wrap_tile(PyObject *self, PyObject *args) {
    int  _arg0,_arg1;
    cave_type  *_result;
    PyObject  *_resultobj;
    self = self;
    if(!PyArg_ParseTuple(args,"ii:tile",&_arg0,&_arg1)) 
        return NULL;
    _result = (cave_type *)tile(_arg0,_arg1);
    _resultobj = SWIG_NewPointerObj((void *) _result, SWIGTYPE_cave_type_p);
    return _resultobj;
}

static int _wrap_object_level_set(PyObject *val) {
    s16b  tval;
    tval = (s16b )PyInt_AsLong(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'object_level'(s16b )");
        return 1; 
    }
    object_level = tval;
    return 0;
}

static PyObject *_wrap_object_level_get() {
    PyObject  *pyobj;
    pyobj = PyInt_FromLong((long) object_level);
    return pyobj;
}

static int _wrap_monster_level_set(PyObject *val) {
    s16b  tval;
    tval = (s16b )PyInt_AsLong(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'monster_level'(s16b )");
        return 1; 
    }
    monster_level = tval;
    return 0;
}

static PyObject *_wrap_monster_level_get() {
    PyObject  *pyobj;
    pyobj = PyInt_FromLong((long) monster_level);
    return pyobj;
}

static int _wrap_base_level_set(PyObject *val) {
    s16b  tval;
    tval = (s16b )PyInt_AsLong(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'base_level'(s16b )");
        return 1; 
    }
    base_level = tval;
    return 0;
}

static PyObject *_wrap_base_level_get() {
    PyObject  *pyobj;
    pyobj = PyInt_FromLong((long) base_level);
    return pyobj;
}

static int _wrap_dun_level_set(PyObject *val) {
    s16b  tval;
    tval = (s16b )PyInt_AsLong(val);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,"C variable 'dun_level'(s16b )");
        return 1; 
    }
    dun_level = tval;
    return 0;
}

static PyObject *_wrap_dun_level_get() {
    PyObject  *pyobj;
    pyobj = PyInt_FromLong((long) dun_level);
    return pyobj;
}

#define cave_type_info_set(_swigobj,_swigval) (_swigobj->info = _swigval,_swigval)
static PyObject *_wrap_cave_type_info_set(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    byte  _result,_arg1;
    self = self;
    if(!PyArg_ParseTuple(args,"Ob:cave_type_info_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (byte )cave_type_info_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define cave_type_info_get(_swigobj) ((byte ) _swigobj->info)
static PyObject *_wrap_cave_type_info_get(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    byte  _result;
    self = self;
    if(!PyArg_ParseTuple(args,"O:cave_type_info_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (byte )cave_type_info_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define cave_type_feat_set(_swigobj,_swigval) (_swigobj->feat = _swigval,_swigval)
static PyObject *_wrap_cave_type_feat_set(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    byte  _result,_arg1;
    self = self;
    if(!PyArg_ParseTuple(args,"Ob:cave_type_feat_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (byte )cave_type_feat_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define cave_type_feat_get(_swigobj) ((byte ) _swigobj->feat)
static PyObject *_wrap_cave_type_feat_get(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    byte  _result;
    self = self;
    if(!PyArg_ParseTuple(args,"O:cave_type_feat_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (byte )cave_type_feat_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define cave_type_o_idx_set(_swigobj,_swigval) (_swigobj->o_idx = _swigval,_swigval)
static PyObject *_wrap_cave_type_o_idx_set(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    s16b  _result,_arg1;
    self = self;
    if(!PyArg_ParseTuple(args,"Oh:cave_type_o_idx_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (s16b )cave_type_o_idx_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("h",_result);
    return _resultobj;
}

#define cave_type_o_idx_get(_swigobj) ((s16b ) _swigobj->o_idx)
static PyObject *_wrap_cave_type_o_idx_get(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    s16b  _result;
    self = self;
    if(!PyArg_ParseTuple(args,"O:cave_type_o_idx_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (s16b )cave_type_o_idx_get(_arg0);
    _resultobj = Py_BuildValue("h",_result);
    return _resultobj;
}

#define cave_type_m_idx_set(_swigobj,_swigval) (_swigobj->m_idx = _swigval,_swigval)
static PyObject *_wrap_cave_type_m_idx_set(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    s16b  _result,_arg1;
    self = self;
    if(!PyArg_ParseTuple(args,"Oh:cave_type_m_idx_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (s16b )cave_type_m_idx_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("h",_result);
    return _resultobj;
}

#define cave_type_m_idx_get(_swigobj) ((s16b ) _swigobj->m_idx)
static PyObject *_wrap_cave_type_m_idx_get(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    s16b  _result;
    self = self;
    if(!PyArg_ParseTuple(args,"O:cave_type_m_idx_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (s16b )cave_type_m_idx_get(_arg0);
    _resultobj = Py_BuildValue("h",_result);
    return _resultobj;
}

#define cave_type_special_set(_swigobj,_swigval) (_swigobj->special = _swigval,_swigval)
static PyObject *_wrap_cave_type_special_set(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    s16b  _result,_arg1;
    self = self;
    if(!PyArg_ParseTuple(args,"Oh:cave_type_special_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (s16b )cave_type_special_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("h",_result);
    return _resultobj;
}

#define cave_type_special_get(_swigobj) ((s16b ) _swigobj->special)
static PyObject *_wrap_cave_type_special_get(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    s16b  _result;
    self = self;
    if(!PyArg_ParseTuple(args,"O:cave_type_special_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (s16b )cave_type_special_get(_arg0);
    _resultobj = Py_BuildValue("h",_result);
    return _resultobj;
}

#define cave_type_mimic_set(_swigobj,_swigval) (_swigobj->mimic = _swigval,_swigval)
static PyObject *_wrap_cave_type_mimic_set(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    byte  _result,_arg1;
    self = self;
    if(!PyArg_ParseTuple(args,"Ob:cave_type_mimic_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (byte )cave_type_mimic_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define cave_type_mimic_get(_swigobj) ((byte ) _swigobj->mimic)
static PyObject *_wrap_cave_type_mimic_get(PyObject *self, PyObject *args) {
    struct cave_type  *_arg0;
    PyObject  *_resultobj,*_argo0=0;
    byte  _result;
    self = self;
    if(!PyArg_ParseTuple(args,"O:cave_type_mimic_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_cave_type_p,1)) == -1) return NULL;
    _result = (byte )cave_type_mimic_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define vault_type_name_set(_swigobj,_swigval) (_swigobj->name = _swigval,_swigval)
static PyObject *_wrap_vault_type_name_set(PyObject *self, PyObject *args) {
    u32b  _result,_arg1;
    PyObject  *_resultobj,*_argo0=0;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"Oi:vault_type_name_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (u32b )vault_type_name_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define vault_type_name_get(_swigobj) ((u32b ) _swigobj->name)
static PyObject *_wrap_vault_type_name_get(PyObject *self, PyObject *args) {
    u32b  _result;
    PyObject  *_resultobj,*_argo0=0;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"O:vault_type_name_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (u32b )vault_type_name_get(_arg0);
    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define vault_type_text_set(_swigobj,_swigval) (_swigobj->text = _swigval,_swigval)
static PyObject *_wrap_vault_type_text_set(PyObject *self, PyObject *args) {
    u32b  _result,_arg1;
    PyObject  *_resultobj,*_argo0=0;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"Oi:vault_type_text_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (u32b )vault_type_text_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define vault_type_text_get(_swigobj) ((u32b ) _swigobj->text)
static PyObject *_wrap_vault_type_text_get(PyObject *self, PyObject *args) {
    u32b  _result;
    PyObject  *_resultobj,*_argo0=0;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"O:vault_type_text_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (u32b )vault_type_text_get(_arg0);
    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define vault_type_typ_set(_swigobj,_swigval) (_swigobj->typ = _swigval,_swigval)
static PyObject *_wrap_vault_type_typ_set(PyObject *self, PyObject *args) {
    PyObject  *_resultobj,*_argo0=0;
    byte  _result,_arg1;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"Ob:vault_type_typ_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (byte )vault_type_typ_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define vault_type_typ_get(_swigobj) ((byte ) _swigobj->typ)
static PyObject *_wrap_vault_type_typ_get(PyObject *self, PyObject *args) {
    PyObject  *_resultobj,*_argo0=0;
    byte  _result;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"O:vault_type_typ_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (byte )vault_type_typ_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define vault_type_rat_set(_swigobj,_swigval) (_swigobj->rat = _swigval,_swigval)
static PyObject *_wrap_vault_type_rat_set(PyObject *self, PyObject *args) {
    PyObject  *_resultobj,*_argo0=0;
    byte  _result,_arg1;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"Ob:vault_type_rat_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (byte )vault_type_rat_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define vault_type_rat_get(_swigobj) ((byte ) _swigobj->rat)
static PyObject *_wrap_vault_type_rat_get(PyObject *self, PyObject *args) {
    PyObject  *_resultobj,*_argo0=0;
    byte  _result;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"O:vault_type_rat_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (byte )vault_type_rat_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define vault_type_hgt_set(_swigobj,_swigval) (_swigobj->hgt = _swigval,_swigval)
static PyObject *_wrap_vault_type_hgt_set(PyObject *self, PyObject *args) {
    PyObject  *_resultobj,*_argo0=0;
    byte  _result,_arg1;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"Ob:vault_type_hgt_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (byte )vault_type_hgt_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define vault_type_hgt_get(_swigobj) ((byte ) _swigobj->hgt)
static PyObject *_wrap_vault_type_hgt_get(PyObject *self, PyObject *args) {
    PyObject  *_resultobj,*_argo0=0;
    byte  _result;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"O:vault_type_hgt_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (byte )vault_type_hgt_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define vault_type_wid_set(_swigobj,_swigval) (_swigobj->wid = _swigval,_swigval)
static PyObject *_wrap_vault_type_wid_set(PyObject *self, PyObject *args) {
    PyObject  *_resultobj,*_argo0=0;
    byte  _result,_arg1;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"Ob:vault_type_wid_set",&_argo0,&_arg1)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (byte )vault_type_wid_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define vault_type_wid_get(_swigobj) ((byte ) _swigobj->wid)
static PyObject *_wrap_vault_type_wid_get(PyObject *self, PyObject *args) {
    PyObject  *_resultobj,*_argo0=0;
    byte  _result;
    struct vault_type  *_arg0;
    self = self;
    if(!PyArg_ParseTuple(args,"O:vault_type_wid_get",&_argo0)) 
        return NULL;
    if ((SWIG_ConvertPtr(_argo0,(void **) &_arg0,SWIGTYPE_vault_type_p,1)) == -1) return NULL;
    _result = (byte )vault_type_wid_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

static PyMethodDef cavecMethods[] = {
	 { "vault_type_wid_get", _wrap_vault_type_wid_get, METH_VARARGS },
	 { "vault_type_wid_set", _wrap_vault_type_wid_set, METH_VARARGS },
	 { "vault_type_hgt_get", _wrap_vault_type_hgt_get, METH_VARARGS },
	 { "vault_type_hgt_set", _wrap_vault_type_hgt_set, METH_VARARGS },
	 { "vault_type_rat_get", _wrap_vault_type_rat_get, METH_VARARGS },
	 { "vault_type_rat_set", _wrap_vault_type_rat_set, METH_VARARGS },
	 { "vault_type_typ_get", _wrap_vault_type_typ_get, METH_VARARGS },
	 { "vault_type_typ_set", _wrap_vault_type_typ_set, METH_VARARGS },
	 { "vault_type_text_get", _wrap_vault_type_text_get, METH_VARARGS },
	 { "vault_type_text_set", _wrap_vault_type_text_set, METH_VARARGS },
	 { "vault_type_name_get", _wrap_vault_type_name_get, METH_VARARGS },
	 { "vault_type_name_set", _wrap_vault_type_name_set, METH_VARARGS },
	 { "cave_type_mimic_get", _wrap_cave_type_mimic_get, METH_VARARGS },
	 { "cave_type_mimic_set", _wrap_cave_type_mimic_set, METH_VARARGS },
	 { "cave_type_special_get", _wrap_cave_type_special_get, METH_VARARGS },
	 { "cave_type_special_set", _wrap_cave_type_special_set, METH_VARARGS },
	 { "cave_type_m_idx_get", _wrap_cave_type_m_idx_get, METH_VARARGS },
	 { "cave_type_m_idx_set", _wrap_cave_type_m_idx_set, METH_VARARGS },
	 { "cave_type_o_idx_get", _wrap_cave_type_o_idx_get, METH_VARARGS },
	 { "cave_type_o_idx_set", _wrap_cave_type_o_idx_set, METH_VARARGS },
	 { "cave_type_feat_get", _wrap_cave_type_feat_get, METH_VARARGS },
	 { "cave_type_feat_set", _wrap_cave_type_feat_set, METH_VARARGS },
	 { "cave_type_info_get", _wrap_cave_type_info_get, METH_VARARGS },
	 { "cave_type_info_set", _wrap_cave_type_info_set, METH_VARARGS },
	 { "tile", _wrap_tile, METH_VARARGS },
	 { "set_feat", _wrap_set_feat, METH_VARARGS },
	 { "load_layout", _wrap_load_layout, METH_VARARGS },
	 { NULL, NULL }
};
#ifdef __cplusplus
}
#endif
static _swig_type_info _swigt__cave_type_p[] = {{"_cave_type_p",0},{"_cave_type_p",0},{0}};
static _swig_type_info _swigt__vault_type_p[] = {{"_vault_type_p",0},{"_vault_type_p",0},{0}};
static _swig_type_info _swigt__errr_p[] = {{"_errr_p",0},{"_errr_p",0},{0}};

static _swig_type_info *_swig_types_initial[] = {
_swigt__cave_type_p, 
_swigt__vault_type_p, 
_swigt__errr_p, 
0
};

static _swig_const_info _swig_const_table[] = {
    { SWIG_PY_INT,     "BLOCK_HGT", (long) 11, 0, 0, 0},
    { SWIG_PY_INT,     "BLOCK_WID", (long) 11, 0, 0, 0},
    { SWIG_PY_INT,     "PANEL_HGT", (long) 11, 0, 0, 0},
    { SWIG_PY_INT,     "PANEL_WID", (long) 33, 0, 0, 0},
    { SWIG_PY_INT,     "SCREEN_HGT", (long) 22, 0, 0, 0},
    { SWIG_PY_INT,     "SCREEN_WID", (long) 66, 0, 0, 0},
    { SWIG_PY_INT,     "MAX_HGT", (long) 66, 0, 0, 0},
    { SWIG_PY_INT,     "MAX_WID", (long) 198, 0, 0, 0},
{0}};

static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT(void) initcavec(void) {
	 PyObject *m, *d;
	 SWIG_globals = SWIG_newvarlink();
	 m = Py_InitModule("cavec", cavecMethods);
	 d = PyModule_GetDict(m);
    {
        int i;
        for (i = 0; _swig_types_initial[i]; i++) {
            _swig_types[i] = SWIG_TypeRegister(_swig_types_initial[i]);
        }
    }
	 PyDict_SetItemString(d,"cvar", SWIG_globals);
	 SWIG_addvarlink(SWIG_globals,"object_level",_wrap_object_level_get, _wrap_object_level_set);
	 SWIG_addvarlink(SWIG_globals,"monster_level",_wrap_monster_level_get, _wrap_monster_level_set);
	 SWIG_addvarlink(SWIG_globals,"base_level",_wrap_base_level_get, _wrap_base_level_set);
	 SWIG_addvarlink(SWIG_globals,"dun_level",_wrap_dun_level_get, _wrap_dun_level_set);
    SWIG_InstallConstants(d,_swig_const_table);

}
