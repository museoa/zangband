/*
 * FILE : terrain_wrap.c
 * 
 * This file was automatically generated by :
 * Simplified Wrapper and Interface Generator (SWIG)
 * Version 1.1 (Build 883)
 * 
 * Portions Copyright (c) 1995-1998
 * The University of Utah and The Regents of the University of California.
 * Permission is granted to distribute this file in any manner provided
 * this notice remains intact.
 * 
 * Do not make changes to this file--changes will be lost!
 *
 */


#define SWIGCODE
/* Implementation : PYTHON */

#define SWIGPYTHON
/***********************************************************************
 * $Header$
 * swig_lib/python/python.cfg
 *
 * Contains variable linking and pointer type-checking code.
 ************************************************************************/

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "Python.h"

/* Definitions for Windows/Unix exporting */
#if defined(_WIN32) || defined(__WIN32__)
#   if defined(_MSC_VER)
#	define SWIGEXPORT(a) __declspec(dllexport) a
#   else
#	if defined(__BORLANDC__)
#	    define SWIGEXPORT(a) a _export
#	else
#	    define SWIGEXPORT(a) a
#	endif
#   endif
#else
#   define SWIGEXPORT(a) a
#endif

#ifdef SWIG_GLOBAL
#define SWIGSTATICRUNTIME(a) SWIGEXPORT(a)
#else
#define SWIGSTATICRUNTIME(a) static a
#endif

typedef struct {
  char  *name;
  PyObject *(*get_attr)(void);
  int (*set_attr)(PyObject *);
} swig_globalvar;

typedef struct swig_varlinkobject {
  PyObject_HEAD
  swig_globalvar **vars;
  int      nvars;
  int      maxvars;
} swig_varlinkobject;

/* ----------------------------------------------------------------------
   swig_varlink_repr()

   Function for python repr method
   ---------------------------------------------------------------------- */

static PyObject *
swig_varlink_repr(swig_varlinkobject *v)
{
  v = v;
  return PyString_FromString("<Global variables>");
}

/* ---------------------------------------------------------------------
   swig_varlink_print()

   Print out all of the global variable names
   --------------------------------------------------------------------- */

static int
swig_varlink_print(swig_varlinkobject *v, FILE *fp, int flags)
{

  int i = 0;
  flags = flags;
  fprintf(fp,"Global variables { ");
  while (v->vars[i]) {
    fprintf(fp,"%s", v->vars[i]->name);
    i++;
    if (v->vars[i]) fprintf(fp,", ");
  }
  fprintf(fp," }\n");
  return 0;
}

/* --------------------------------------------------------------------
   swig_varlink_getattr
 
   This function gets the value of a variable and returns it as a
   PyObject.   In our case, we'll be looking at the datatype and
   converting into a number or string
   -------------------------------------------------------------------- */

static PyObject *
swig_varlink_getattr(swig_varlinkobject *v, char *n)
{
  int i = 0;
  char temp[128];

  while (v->vars[i]) {
    if (strcmp(v->vars[i]->name,n) == 0) {
      return (*v->vars[i]->get_attr)();
    }
    i++;
  }
  sprintf(temp,"C global variable %s not found.", n);
  PyErr_SetString(PyExc_NameError,temp);
  return NULL;
}

/* -------------------------------------------------------------------
   swig_varlink_setattr()

   This function sets the value of a variable.
   ------------------------------------------------------------------- */

static int
swig_varlink_setattr(swig_varlinkobject *v, char *n, PyObject *p)
{
  char temp[128];
  int i = 0;
  while (v->vars[i]) {
    if (strcmp(v->vars[i]->name,n) == 0) {
      return (*v->vars[i]->set_attr)(p);
    }
    i++;
  }
  sprintf(temp,"C global variable %s not found.", n);
  PyErr_SetString(PyExc_NameError,temp);
  return 1;
}

statichere PyTypeObject varlinktype = {
/*  PyObject_HEAD_INIT(&PyType_Type)  Note : This doesn't work on some machines */
  PyObject_HEAD_INIT(0)              
  0,
  "varlink",                          /* Type name    */
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
SWIG_newvarlink(void)
{
  swig_varlinkobject *result = 0;
  result = PyMem_NEW(swig_varlinkobject,1);
  varlinktype.ob_type = &PyType_Type;    /* Patch varlinktype into a PyType */
  result->ob_type = &varlinktype;
  /*  _Py_NewReference(result);  Does not seem to be necessary */
  result->nvars = 0;
  result->maxvars = 64;
  result->vars = (swig_globalvar **) malloc(64*sizeof(swig_globalvar *));
  result->vars[0] = 0;
  result->ob_refcnt = 0;
  Py_XINCREF((PyObject *) result);
  return ((PyObject*) result);
}

SWIGSTATICRUNTIME(void)
SWIG_addvarlink(PyObject *p, char *name,
	   PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p))
{
  swig_varlinkobject *v;
  v= (swig_varlinkobject *) p;
	
  if (v->nvars >= v->maxvars -1) {
    v->maxvars = 2*v->maxvars;
    v->vars = (swig_globalvar **) realloc(v->vars,v->maxvars*sizeof(swig_globalvar *));
    if (v->vars == NULL) {
      fprintf(stderr,"SWIG : Fatal error in initializing Python module.\n");
      exit(1);
    }
  }
  v->vars[v->nvars] = (swig_globalvar *) malloc(sizeof(swig_globalvar));
  v->vars[v->nvars]->name = (char *) malloc(strlen(name)+1);
  strcpy(v->vars[v->nvars]->name,name);
  v->vars[v->nvars]->get_attr = get_attr;
  v->vars[v->nvars]->set_attr = set_attr;
  v->nvars++;
  v->vars[v->nvars] = 0;
}

/* -----------------------------------------------------------------------------
 * Pointer type-checking
 * ----------------------------------------------------------------------------- */

/* SWIG pointer structure */
typedef struct SwigPtrType {
  char               *name;               /* Datatype name                  */
  int                 len;                /* Length (used for optimization) */
  void               *(*cast)(void *);    /* Pointer casting function       */
  struct SwigPtrType *next;               /* Linked list pointer            */
} SwigPtrType;

/* Pointer cache structure */
typedef struct {
  int                 stat;               /* Status (valid) bit             */
  SwigPtrType        *tp;                 /* Pointer to type structure      */
  char                name[256];          /* Given datatype name            */
  char                mapped[256];        /* Equivalent name                */
} SwigCacheType;

static int SwigPtrMax  = 64;           /* Max entries that can be currently held */
static int SwigPtrN    = 0;            /* Current number of entries              */
static int SwigPtrSort = 0;            /* Status flag indicating sort            */
static int SwigStart[256];             /* Starting positions of types            */
static SwigPtrType *SwigPtrTable = 0;  /* Table containing pointer equivalences  */

/* Cached values */
#define SWIG_CACHESIZE  8
#define SWIG_CACHEMASK  0x7
static SwigCacheType SwigCache[SWIG_CACHESIZE];  
static int SwigCacheIndex = 0;
static int SwigLastCache = 0;

/* Sort comparison function */
static int swigsort(const void *data1, const void *data2) {
	SwigPtrType *d1 = (SwigPtrType *) data1;
	SwigPtrType *d2 = (SwigPtrType *) data2;
	return strcmp(d1->name,d2->name);
}

/* Register a new datatype with the type-checker */
SWIGSTATICRUNTIME(void) 
SWIG_RegisterMapping(char *origtype, char *newtype, void *(*cast)(void *)) {
  int i;
  SwigPtrType *t = 0,*t1;

  /* Allocate the pointer table if necessary */
  if (!SwigPtrTable) {     
    SwigPtrTable = (SwigPtrType *) malloc(SwigPtrMax*sizeof(SwigPtrType));
  }

  /* Grow the table */
  if (SwigPtrN >= SwigPtrMax) {
    SwigPtrMax = 2*SwigPtrMax;
    SwigPtrTable = (SwigPtrType *) realloc((char *) SwigPtrTable,SwigPtrMax*sizeof(SwigPtrType));
  }
  for (i = 0; i < SwigPtrN; i++) {
    if (strcmp(SwigPtrTable[i].name,origtype) == 0) {
      t = &SwigPtrTable[i];
      break;
    }
  }
  if (!t) {
    t = &SwigPtrTable[SwigPtrN++];
    t->name = origtype;
    t->len = strlen(t->name);
    t->cast = 0;
    t->next = 0;
  }

  /* Check for existing entries */
  while (t->next) {
    if ((strcmp(t->name,newtype) == 0)) {
      if (cast) t->cast = cast;
      return;
    }
    t = t->next;
  }
  t1 = (SwigPtrType *) malloc(sizeof(SwigPtrType));
  t1->name = newtype;
  t1->len = strlen(t1->name);
  t1->cast = cast;
  t1->next = 0;            
  t->next = t1;           
  SwigPtrSort = 0;
}

/* Make a pointer value string */
SWIGSTATICRUNTIME(void) 
SWIG_MakePtr(char *c, const void *ptr, char *type) {
  static char hex[17] = "0123456789abcdef";
  unsigned long p, s;
  char result[24], *r; 
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
    strcpy (c, type);
  } else {
    strcpy (c, "NULL");
  }
}

/* Function for getting a pointer value */
SWIGSTATICRUNTIME(char *) 
SWIG_GetPtr(char *c, void **ptr, char *t)
{
  unsigned long p;
  char temp_type[256], *name;
  int  i, len, start, end;
  SwigPtrType *sp,*tp;
  SwigCacheType *cache;
  register int d;

  p = 0;
  /* Pointer values must start with leading underscore */
  if (*c != '_') {
    *ptr = (void *) 0;
    if (strcmp(c,"NULL") == 0) return (char *) 0;
    else c;
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
  if ((!t) || (strcmp(t,c)==0)) return (char *) 0;

  if (!SwigPtrSort) {
    qsort((void *) SwigPtrTable, SwigPtrN, sizeof(SwigPtrType), swigsort); 
    for (i = 0; i < 256; i++) SwigStart[i] = SwigPtrN;
    for (i = SwigPtrN-1; i >= 0; i--) SwigStart[(int) (SwigPtrTable[i].name[1])] = i;
    for (i = 255; i >= 1; i--) {
      if (SwigStart[i-1] > SwigStart[i])
	SwigStart[i-1] = SwigStart[i];
    }
    SwigPtrSort = 1;
    for (i = 0; i < SWIG_CACHESIZE; i++) SwigCache[i].stat = 0;
  }
  /* First check cache for matches.  Uses last cache value as starting point */
  cache = &SwigCache[SwigLastCache];
  for (i = 0; i < SWIG_CACHESIZE; i++) {
    if (cache->stat && (strcmp(t,cache->name) == 0) && (strcmp(c,cache->mapped) == 0)) {
      cache->stat++;
      if (cache->tp->cast) *ptr = (*(cache->tp->cast))(*ptr);
      return (char *) 0;
    }
    SwigLastCache = (SwigLastCache+1) & SWIG_CACHEMASK;
    if (!SwigLastCache) cache = SwigCache;
    else cache++;
  }
  /* Type mismatch.  Look through type-mapping table */
  start = SwigStart[(int) t[1]];
  end = SwigStart[(int) t[1]+1];
  sp = &SwigPtrTable[start];

  /* Try to find a match */
  while (start <= end) {
    if (strncmp(t,sp->name,sp->len) == 0) {
      name = sp->name;
      len = sp->len;
      tp = sp->next;
      /* Try to find entry for our given datatype */
      while(tp) {
	if (tp->len >= 255) {
	  return c;
	}
	strcpy(temp_type,tp->name);
	strncat(temp_type,t+len,255-tp->len);
	if (strcmp(c,temp_type) == 0) {
	  strcpy(SwigCache[SwigCacheIndex].mapped,c);
	  strcpy(SwigCache[SwigCacheIndex].name,t);
	  SwigCache[SwigCacheIndex].stat = 1;
	  SwigCache[SwigCacheIndex].tp = tp;
	  SwigCacheIndex = SwigCacheIndex & SWIG_CACHEMASK;
	  /* Get pointer value */
	  *ptr = (void *) p;
	  if (tp->cast) *ptr = (*(tp->cast))(*ptr);
	  return (char *) 0;
	}
	tp = tp->next;
      }
    }
    sp++;
    start++;
  }
  return c;
} 

/* New object-based GetPointer function. This uses the Python abstract
 * object interface to automatically dereference the 'this' attribute
 * of shadow objects. */

SWIGSTATICRUNTIME(char *)
SWIG_GetPtrObj(PyObject *obj, void **ptr, char *type) {
  PyObject *sobj = obj;
  char     *str;
  if (!PyString_Check(obj)) {
    sobj = PyObject_GetAttrString(obj,"this");
    if (!sobj) return "";
  }
  str = PyString_AsString(sobj);
  return SWIG_GetPtr(str,ptr,type);
}

#ifdef __cplusplus
}
#endif


#define SWIG_init    initterrain

#define SWIG_name    "terrain"


#include "angband.h"

#ifdef __cplusplus
extern "C" {
#endif
#define feature_type_name_set(_swigobj,_swigval) (_swigobj->name = _swigval,_swigval)
static PyObject *_wrap_feature_type_name_set(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    u32b  _result;
    struct feature_type * _arg0;
    u32b  _arg1;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"Oi:feature_type_name_set",&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_name_set. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (u32b )feature_type_name_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define feature_type_name_get(_swigobj) ((u32b ) _swigobj->name)
static PyObject *_wrap_feature_type_name_get(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    u32b  _result;
    struct feature_type * _arg0;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"O:feature_type_name_get",&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_name_get. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (u32b )feature_type_name_get(_arg0);
    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define feature_type_text_set(_swigobj,_swigval) (_swigobj->text = _swigval,_swigval)
static PyObject *_wrap_feature_type_text_set(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    u32b  _result;
    struct feature_type * _arg0;
    u32b  _arg1;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"Oi:feature_type_text_set",&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_text_set. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (u32b )feature_type_text_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define feature_type_text_get(_swigobj) ((u32b ) _swigobj->text)
static PyObject *_wrap_feature_type_text_get(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    u32b  _result;
    struct feature_type * _arg0;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"O:feature_type_text_get",&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_text_get. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (u32b )feature_type_text_get(_arg0);
    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define feature_type_d_attr_set(_swigobj,_swigval) (_swigobj->d_attr = _swigval,_swigval)
static PyObject *_wrap_feature_type_d_attr_set(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    byte  _result;
    struct feature_type * _arg0;
    byte  _arg1;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"Ob:feature_type_d_attr_set",&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_d_attr_set. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (byte )feature_type_d_attr_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define feature_type_d_attr_get(_swigobj) ((byte ) _swigobj->d_attr)
static PyObject *_wrap_feature_type_d_attr_get(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    byte  _result;
    struct feature_type * _arg0;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"O:feature_type_d_attr_get",&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_d_attr_get. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (byte )feature_type_d_attr_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define feature_type_d_char_set(_swigobj,_swigval) (_swigobj->d_char = _swigval,_swigval)
static PyObject *_wrap_feature_type_d_char_set(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    char  _result;
    struct feature_type * _arg0;
    char  _arg1;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"Oc:feature_type_d_char_set",&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_d_char_set. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (char )feature_type_d_char_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("c",_result);
    return _resultobj;
}

#define feature_type_d_char_get(_swigobj) ((char ) _swigobj->d_char)
static PyObject *_wrap_feature_type_d_char_get(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    char  _result;
    struct feature_type * _arg0;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"O:feature_type_d_char_get",&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_d_char_get. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (char )feature_type_d_char_get(_arg0);
    _resultobj = Py_BuildValue("c",_result);
    return _resultobj;
}

#define feature_type_x_attr_set(_swigobj,_swigval) (_swigobj->x_attr = _swigval,_swigval)
static PyObject *_wrap_feature_type_x_attr_set(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    byte  _result;
    struct feature_type * _arg0;
    byte  _arg1;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"Ob:feature_type_x_attr_set",&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_x_attr_set. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (byte )feature_type_x_attr_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define feature_type_x_attr_get(_swigobj) ((byte ) _swigobj->x_attr)
static PyObject *_wrap_feature_type_x_attr_get(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    byte  _result;
    struct feature_type * _arg0;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"O:feature_type_x_attr_get",&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_x_attr_get. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (byte )feature_type_x_attr_get(_arg0);
    _resultobj = Py_BuildValue("b",_result);
    return _resultobj;
}

#define feature_type_x_char_set(_swigobj,_swigval) (_swigobj->x_char = _swigval,_swigval)
static PyObject *_wrap_feature_type_x_char_set(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    char  _result;
    struct feature_type * _arg0;
    char  _arg1;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"Oc:feature_type_x_char_set",&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_x_char_set. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (char )feature_type_x_char_set(_arg0,_arg1);
    _resultobj = Py_BuildValue("c",_result);
    return _resultobj;
}

#define feature_type_x_char_get(_swigobj) ((char ) _swigobj->x_char)
static PyObject *_wrap_feature_type_x_char_get(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    char  _result;
    struct feature_type * _arg0;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"O:feature_type_x_char_get",&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of feature_type_x_char_get. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    _result = (char )feature_type_x_char_get(_arg0);
    _resultobj = Py_BuildValue("c",_result);
    return _resultobj;
}

static struct feature_type *new_feature_type() {
			return (feature_type*) malloc(sizeof(feature_type));
		}

static PyObject *_wrap_new_feature_type(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    struct feature_type * _result;
    char _ptemp[128];

    self = self;
    if(!PyArg_ParseTuple(args,":new_feature_type")) 
        return NULL;
    _result = (struct feature_type *)new_feature_type();
    if (_result) {
        SWIG_MakePtr(_ptemp, (char *) _result,"_struct_feature_type_p");
        _resultobj = Py_BuildValue("s",_ptemp);
    } else {
        Py_INCREF(Py_None);
        _resultobj = Py_None;
    }
    return _resultobj;
}

static void delete_feature_type(struct feature_type *self) {
			free(self);
		}
static PyObject *_wrap_delete_feature_type(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    struct feature_type * _arg0;
    PyObject * _argo0 = 0;

    self = self;
    if(!PyArg_ParseTuple(args,"O:delete_feature_type",&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_struct_feature_type_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of delete_feature_type. Expected _struct_feature_type_p.");
        return NULL;
        }
    }
    delete_feature_type(_arg0);
    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyMethodDef terrainMethods[] = {
	 { "delete_feature_type", _wrap_delete_feature_type, METH_VARARGS },
	 { "new_feature_type", _wrap_new_feature_type, METH_VARARGS },
	 { "feature_type_x_char_get", _wrap_feature_type_x_char_get, METH_VARARGS },
	 { "feature_type_x_char_set", _wrap_feature_type_x_char_set, METH_VARARGS },
	 { "feature_type_x_attr_get", _wrap_feature_type_x_attr_get, METH_VARARGS },
	 { "feature_type_x_attr_set", _wrap_feature_type_x_attr_set, METH_VARARGS },
	 { "feature_type_d_char_get", _wrap_feature_type_d_char_get, METH_VARARGS },
	 { "feature_type_d_char_set", _wrap_feature_type_d_char_set, METH_VARARGS },
	 { "feature_type_d_attr_get", _wrap_feature_type_d_attr_get, METH_VARARGS },
	 { "feature_type_d_attr_set", _wrap_feature_type_d_attr_set, METH_VARARGS },
	 { "feature_type_text_get", _wrap_feature_type_text_get, METH_VARARGS },
	 { "feature_type_text_set", _wrap_feature_type_text_set, METH_VARARGS },
	 { "feature_type_name_get", _wrap_feature_type_name_get, METH_VARARGS },
	 { "feature_type_name_set", _wrap_feature_type_name_set, METH_VARARGS },
	 { NULL, NULL }
};
#ifdef __cplusplus
}
#endif
/*
 * This table is used by the pointer type-checker
 */
static struct { char *n1; char *n2; void *(*pcnv)(void *); } _swig_mapping[] = {
    { "_signed_long","_long",0},
    { "_byte","_unsigned_char",0},
    { "_long","_unsigned_long",0},
    { "_long","_signed_long",0},
    { "_struct_feature_type","_feature_type",0},
    { "_feature_type","_struct_feature_type",0},
    { "_cptr","_char_p",0},
    { "_char_p","_cptr",0},
    { "_unsigned_long","_long",0},
    { "_signed_int","_s32b",0},
    { "_signed_int","_int",0},
    { "_s16b","_signed_short",0},
    { "_s16b","_short",0},
    { "_unsigned_short","_u16b",0},
    { "_unsigned_short","_short",0},
    { "_s32b","_signed_int",0},
    { "_s32b","_int",0},
    { "_signed_short","_s16b",0},
    { "_signed_short","_short",0},
    { "_unsigned_char","_byte",0},
    { "_unsigned_int","_u32b",0},
    { "_unsigned_int","_int",0},
    { "_short","_u16b",0},
    { "_short","_s16b",0},
    { "_short","_unsigned_short",0},
    { "_short","_signed_short",0},
    { "_int","_u32b",0},
    { "_int","_s32b",0},
    { "_int","_unsigned_int",0},
    { "_int","_signed_int",0},
    { "_u16b","_unsigned_short",0},
    { "_u16b","_short",0},
    { "_u32b","_unsigned_int",0},
    { "_u32b","_int",0},
{0,0,0}};

static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT(void) initterrain() {
	 PyObject *m, *d;
	 SWIG_globals = SWIG_newvarlink();
	 m = Py_InitModule("terrain", terrainMethods);
	 d = PyModule_GetDict(m);
{
   int i;
   for (i = 0; _swig_mapping[i].n1; i++)
        SWIG_RegisterMapping(_swig_mapping[i].n1,_swig_mapping[i].n2,_swig_mapping[i].pcnv);
}
}
