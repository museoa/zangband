/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3u-20000922-2313 (Alpha 5)
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

#define  SWIGTYPE_p_player_class swig_types[0] 
#define  SWIGTYPE_p_short swig_types[1] 
static swig_type_info *swig_types[3];

/* -------- TYPES TABLE (END) -------- */


/*-----------------------------------------------
              @(target):= pclassc.so
  ------------------------------------------------*/
#define SWIG_init    initpclassc

#define SWIG_name    "pclassc"


#include "angband.h"

#ifdef __cplusplus
extern "C" {
#endif
static PyObject *_wrap_player_class_title_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    cptr arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Os:player_class_title_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    {
        arg0->title = string_make(arg1);
    }
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_title_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    cptr result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_title_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (cptr ) (arg0->title);
    resultobj = PyString_FromString(result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_adj_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    short *result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_adj_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (short *)(short *) (arg0->c_adj);
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_short);
    return resultobj;
}


static PyObject *_wrap_player_class_c_dis_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_dis_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_dis = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_dis_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_dis_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_dis);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_dev_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_dev_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_dev = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_dev_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_dev_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_dev);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_sav_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_sav_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_sav = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_sav_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_sav_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_sav);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_stl_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_stl_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_stl = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_stl_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_stl_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_stl);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_srh_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_srh_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_srh = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_srh_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_srh_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_srh);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_fos_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_fos_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_fos = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_fos_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_fos_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_fos);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_thn_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_thn_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_thn = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_thn_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_thn_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_thn);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_thb_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_thb_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_thb = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_thb_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_thb_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_thb);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_x_dis_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_x_dis_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->x_dis = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_x_dis_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_x_dis_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->x_dis);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_x_dev_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_x_dev_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->x_dev = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_x_dev_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_x_dev_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->x_dev);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_x_sav_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_x_sav_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->x_sav = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_x_sav_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_x_sav_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->x_sav);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_x_stl_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_x_stl_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->x_stl = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_x_stl_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_x_stl_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->x_stl);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_x_srh_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_x_srh_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->x_srh = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_x_srh_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_x_srh_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->x_srh);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_x_fos_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_x_fos_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->x_fos = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_x_fos_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_x_fos_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->x_fos);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_x_thn_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_x_thn_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->x_thn = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_x_thn_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_x_thn_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->x_thn);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_x_thb_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_x_thb_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->x_thb = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_x_thb_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_x_thb_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->x_thb);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_mhp_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_mhp_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_mhp = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_mhp_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_mhp_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_mhp);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_c_exp_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    s16b arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oh:player_class_c_exp_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->c_exp = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_c_exp_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_c_exp_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b ) (arg0->c_exp);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_player_class_pet_upkeep_div_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    byte arg1 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Ob:player_class_pet_upkeep_div_set",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    arg0->pet_upkeep_div = arg1;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_player_class_pet_upkeep_div_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    byte result ;
    
    if(!PyArg_ParseTuple(args,"O:player_class_pet_upkeep_div_get",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (byte ) (arg0->pet_upkeep_div);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


struct player_class * new_player_class() {
    {
        cp_ptr = (player_class*) malloc(sizeof(player_class));
        return cp_ptr;
    }
}


static PyObject *_wrap_new_player_class(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *result ;
    
    if(!PyArg_ParseTuple(args,":new_player_class")) return NULL;
    result = (struct player_class *)new_player_class();
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_player_class);
    return resultobj;
}


void  delete_player_class(struct player_class *self) {
    {
        free(self);
    }
}


static PyObject *_wrap_delete_player_class(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"O:delete_player_class",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    delete_player_class(arg0);
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


s16b  player_class_get_c_adj(struct player_class *self,int i) {
    {
        return self->c_adj[i];
    }
}


static PyObject *_wrap_player_class_get_c_adj(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    int arg1 ;
    PyObject * argo0 =0 ;
    s16b result ;
    
    if(!PyArg_ParseTuple(args,"Oi:player_class_get_c_adj",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    result = (s16b )player_class_get_c_adj(arg0,arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


void  player_class_set_c_adj(struct player_class *self,int i,s16b val) {
    {
        self->c_adj[i] = val;
    }
}


static PyObject *_wrap_player_class_set_c_adj(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    struct player_class *arg0 ;
    int arg1 ;
    s16b arg2 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"Oih:player_class_set_c_adj",&argo0,&arg1,&arg2)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_player_class,1)) == -1) return NULL;
    player_class_set_c_adj(arg0,arg1,arg2);
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyMethodDef pclasscMethods[] = {
	 { "player_class_title_set", _wrap_player_class_title_set, METH_VARARGS },
	 { "player_class_title_get", _wrap_player_class_title_get, METH_VARARGS },
	 { "player_class_c_adj_get", _wrap_player_class_c_adj_get, METH_VARARGS },
	 { "player_class_c_dis_set", _wrap_player_class_c_dis_set, METH_VARARGS },
	 { "player_class_c_dis_get", _wrap_player_class_c_dis_get, METH_VARARGS },
	 { "player_class_c_dev_set", _wrap_player_class_c_dev_set, METH_VARARGS },
	 { "player_class_c_dev_get", _wrap_player_class_c_dev_get, METH_VARARGS },
	 { "player_class_c_sav_set", _wrap_player_class_c_sav_set, METH_VARARGS },
	 { "player_class_c_sav_get", _wrap_player_class_c_sav_get, METH_VARARGS },
	 { "player_class_c_stl_set", _wrap_player_class_c_stl_set, METH_VARARGS },
	 { "player_class_c_stl_get", _wrap_player_class_c_stl_get, METH_VARARGS },
	 { "player_class_c_srh_set", _wrap_player_class_c_srh_set, METH_VARARGS },
	 { "player_class_c_srh_get", _wrap_player_class_c_srh_get, METH_VARARGS },
	 { "player_class_c_fos_set", _wrap_player_class_c_fos_set, METH_VARARGS },
	 { "player_class_c_fos_get", _wrap_player_class_c_fos_get, METH_VARARGS },
	 { "player_class_c_thn_set", _wrap_player_class_c_thn_set, METH_VARARGS },
	 { "player_class_c_thn_get", _wrap_player_class_c_thn_get, METH_VARARGS },
	 { "player_class_c_thb_set", _wrap_player_class_c_thb_set, METH_VARARGS },
	 { "player_class_c_thb_get", _wrap_player_class_c_thb_get, METH_VARARGS },
	 { "player_class_x_dis_set", _wrap_player_class_x_dis_set, METH_VARARGS },
	 { "player_class_x_dis_get", _wrap_player_class_x_dis_get, METH_VARARGS },
	 { "player_class_x_dev_set", _wrap_player_class_x_dev_set, METH_VARARGS },
	 { "player_class_x_dev_get", _wrap_player_class_x_dev_get, METH_VARARGS },
	 { "player_class_x_sav_set", _wrap_player_class_x_sav_set, METH_VARARGS },
	 { "player_class_x_sav_get", _wrap_player_class_x_sav_get, METH_VARARGS },
	 { "player_class_x_stl_set", _wrap_player_class_x_stl_set, METH_VARARGS },
	 { "player_class_x_stl_get", _wrap_player_class_x_stl_get, METH_VARARGS },
	 { "player_class_x_srh_set", _wrap_player_class_x_srh_set, METH_VARARGS },
	 { "player_class_x_srh_get", _wrap_player_class_x_srh_get, METH_VARARGS },
	 { "player_class_x_fos_set", _wrap_player_class_x_fos_set, METH_VARARGS },
	 { "player_class_x_fos_get", _wrap_player_class_x_fos_get, METH_VARARGS },
	 { "player_class_x_thn_set", _wrap_player_class_x_thn_set, METH_VARARGS },
	 { "player_class_x_thn_get", _wrap_player_class_x_thn_get, METH_VARARGS },
	 { "player_class_x_thb_set", _wrap_player_class_x_thb_set, METH_VARARGS },
	 { "player_class_x_thb_get", _wrap_player_class_x_thb_get, METH_VARARGS },
	 { "player_class_c_mhp_set", _wrap_player_class_c_mhp_set, METH_VARARGS },
	 { "player_class_c_mhp_get", _wrap_player_class_c_mhp_get, METH_VARARGS },
	 { "player_class_c_exp_set", _wrap_player_class_c_exp_set, METH_VARARGS },
	 { "player_class_c_exp_get", _wrap_player_class_c_exp_get, METH_VARARGS },
	 { "player_class_pet_upkeep_div_set", _wrap_player_class_pet_upkeep_div_set, METH_VARARGS },
	 { "player_class_pet_upkeep_div_get", _wrap_player_class_pet_upkeep_div_get, METH_VARARGS },
	 { "new_player_class", _wrap_new_player_class, METH_VARARGS },
	 { "delete_player_class", _wrap_delete_player_class, METH_VARARGS },
	 { "player_class_get_c_adj", _wrap_player_class_get_c_adj, METH_VARARGS },
	 { "player_class_set_c_adj", _wrap_player_class_set_c_adj, METH_VARARGS },
	 { NULL, NULL }
};

#ifdef __cplusplus
}
#endif

/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (BEGIN) -------- */

static swig_type_info _swigt__p_player_class[] = {{"_p_player_class", 0, "struct player_class *"},{"_p_player_class"},{0}};
static swig_type_info _swigt__p_short[] = {{"_p_short", 0, "short *"},{"_p_short"},{0}};

static swig_type_info *swig_types_initial[] = {
_swigt__p_player_class, 
_swigt__p_short, 
0
};


/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (END) -------- */

static swig_const_info swig_const_table[] = {
{0}};

static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT(void) initpclassc(void) {
    PyObject *m, *d;
    int i;
    SWIG_globals = SWIG_newvarlink();
    m = Py_InitModule("pclassc", pclasscMethods);
    d = PyModule_GetDict(m);
    for (i = 0; swig_types_initial[i]; i++) {
        swig_types[i] = SWIG_TypeRegister(swig_types_initial[i]);
    }
    SWIG_InstallConstants(d,swig_const_table);
}

