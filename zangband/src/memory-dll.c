#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>

void db_dump_allocs(void);
int db_heap_error(void* vp,int head);

#define _DB_BIGPAD_

#define FROMPTR(ptr) (dbst*)(((char*)ptr)-sizeof(dbst)+sizeof(int)-HMEMPAD)
#define DBSZ(siz) (sizeof(dbst)-1+siz+HMEMPAD+TMEMPAD)
#define DB_MAGIC 0xbeafdead
#define DB_MEMFILL (unsigned char)0x253
#ifdef _DB_BIGPAD_
#define _db_padbig "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#else
#define _db_padbig
#endif
static char _db_padhead[]=_db_padbig "PaDhEaD\201";
static char _db_padtail[]="\201PaDtAiL" _db_padbig;
#define HMEMPAD (sizeof(_db_padhead)-1)
#define TMEMPAD (sizeof(_db_padtail)-1)

typedef struct dbst {
  int magic;
  struct dbst* next, *prev;
  unsigned int siz;
  int line;
  unsigned int mark:31;
  unsigned int dead:1;
  char *file;
  char d[1];
}  dbst;

int db_mark=0;
int db_fatal=0;
int db_interval=1;
int db_verbose=0;
int db_quiet=0;
int db_nofree=0;
int db_freebad=0;
int db_writes=1;
int db_checkstart=0;  /* Start all heap checks from this mark */

static dbst *heap=0;
static dbst *freeheap=0;
static int db_is_init=0;
static int db_zero=1;
static int db_atexit=1;
static int hcnt=0;
static int db_heap=0;

static int objcmd_dbcheck(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	db_dump_allocs();
	return TCL_OK;
}

static void _db_init(void) {
    char *cp,  *e=getenv("DB_ALLOC");
    db_is_init=1;
    if (e) {
	while (e && isalpha(*e)) {
	  if (*(cp=(e+1)) != '=')  {
	     exit(-1);
	  }
	  db_heap=1;
	  switch (e[0]) {
	    case 'd': break;
	    case 'z': db_zero=atoi(cp+1); break;
	    case 'x': db_atexit=atoi(cp+1); break;
	    case 'f': db_fatal=atoi(cp+1); break;
	    case 'i': db_interval=atoi(cp+1); break;
	    case 'v': db_verbose=atoi(cp+1); break;
	    case 'q': db_quiet=atoi(cp+1); break;
	    case 'n': db_nofree=atoi(cp+1); break;
	    case 'b': db_freebad=atoi(cp+1); break;
	    default:
	    case 'w':
	    	if (e[0]=='w' && db_nofree) { db_writes=atoi(cp+1); break; }
		exit(-1);
	  }
	  if ((e=strchr(cp,','))) e++;
	}
    }
   if (db_heap && db_atexit)
      atexit(db_dump_allocs);

{
	extern Tcl_Interp *TclTk_Interp(void);
	Tcl_Interp *interp = TclTk_Interp();
	Tcl_CreateObjCommand(interp, "dbcheck", objcmd_dbcheck, NULL, NULL);
}
}

static void _db_heap_check_v(void *vp) {
    dbst *v=vp;
    if (v->dead) {
      return;
    }
    if (memcmp(v->d,_db_padhead,HMEMPAD))
       if (db_heap_error(v,1) && db_fatal) abort();
    if (memcmp(v->d+HMEMPAD+v->siz,_db_padtail,TMEMPAD))
       if (db_heap_error(v,0) && db_fatal) abort();
}


void db_heap_check(int mark) {
    dbst* v;
    if (!db_is_init) _db_init();
    for (v=heap; v; v=v->next)
	if (v->mark >= mark)
	  _db_heap_check_v(v);
}

void db_dump_since(int mark) {
    int hdr=0;
    dbst* v;
    for (v=heap; v; v=v->next)
	if (v->mark>=mark) {
          if (!hdr) {
	     hdr=1;
	  }
	  _db_heap_check_v(v);
       }
}

/* Dump all allocations */
void db_dump_allocs(void) {
   db_dump_since(db_checkstart);
}

static char * _db_mempad(dbst *v, unsigned siz) {
   v->magic=DB_MAGIC;
   memcpy(v->d,_db_padhead,HMEMPAD);
   memcpy(v->d+HMEMPAD+siz,_db_padtail,TMEMPAD);
   v->siz=siz;
   if (db_interval && (hcnt++ >= db_interval)) {
     db_heap_check(db_checkstart);
     hcnt=0;
   }
   return v->d+HMEMPAD;
}

#define DBSETUP v=DBSetup(siz,file,line);

dbst* DBSetup(int siz, char* file, int line) {
  dbst *v=(dbst*)Tcl_Alloc(DBSZ(siz));
  memset(v, 0, sizeof(dbst)); /* TNB */
  v->file=file;
  v->line=line;
  v->mark=++db_mark;
  if (!heap) return heap=v;
  v->next=heap; 
  heap->prev=v;
  return heap=v;
}

void *_db_malloc(size_t siz, char *file, int line) {
   dbst *v;
   if (!db_is_init) _db_init();
   if (!db_heap) return Tcl_Alloc(siz);
   DBSETUP
   /* printf("%x\n", v); */
   return _db_mempad(v,siz);
}

void *_db_calloc(size_t sz, size_t cnt, char *file, int line) {
   char *p;
   unsigned int siz;
   dbst *v;
   if (!db_is_init) _db_init();
   if (!db_heap) return Tcl_Alloc(sz * cnt);
   siz=sz*cnt;
   DBSETUP
   p=_db_mempad(v,siz);
   memset(p,0,siz);
   return p;
}

void *_db_realloc(void *ptr, size_t siz, char *file, int line) {
   dbst *nv, *pv, *v,*ov;
   if (!db_is_init) _db_init();
   if (!db_heap) return Tcl_Realloc(ptr, siz);
   ov=v=FROMPTR(ptr);
   if (v->magic!=DB_MAGIC) {
 	if (db_heap_error(v,3))
	    abort();
	if (db_freebad) return Tcl_Realloc(ptr, siz);
	return ptr;
   }
   if (siz<v->siz) return ptr;
   pv=v->prev; nv=v->next;
   v->line=line;
   v->file=file;
   v=(dbst *)Tcl_Realloc((char *) v,siz+HMEMPAD+TMEMPAD+sizeof(dbst));
   v->siz = siz; /* TNB */
   memcpy(v->d+HMEMPAD+siz,_db_padtail,TMEMPAD);
   if (ov==v) return ptr;
   if (ov==heap) heap=v;
   if (pv) pv->next=v;
   if (nv) nv->prev=v;
   return v->d+HMEMPAD;
}

char *_db_strdup(const char *str, char *file, int line) {
   char *p;
   unsigned int siz;
   dbst *v;
   if (!db_is_init) _db_init();
   siz=strlen(str)+1;
   if (!db_heap) {
      p=Tcl_Alloc(siz);
   } else {
     DBSETUP
     p=_db_mempad(v,siz);
   }
   strcpy(p,str);
   return p;
}

void _db_free(void *ptr, char *file, int line) {
   dbst *v;
   if (!db_is_init) _db_init();
   if (!db_heap) { if (ptr) Tcl_Free(ptr); return; }
   if (!ptr) {
      if (db_zero)
        db_heap_error(0,2);
      return;
   }
   v=FROMPTR(ptr);
   if (v->magic!=DB_MAGIC) {
	if (db_heap_error(v,3))
	    abort();
	if ((!db_nofree) && db_freebad) Tcl_Free(ptr);
	return;
   }
   if (!v->dead)
      _db_heap_check_v(v);
   v->dead=1;
   if (v==heap) heap=v->next;
   else {
     if (!v->prev) {
        Tcl_Panic("_db_free: v->prev is NULL");
     }
     if ((v->prev->next=v->next))
        v->next->prev=v->prev;
   }
   if (!db_nofree) Tcl_Free((char *) v);
   else {  /* Save on freeheap to check for later writes. */
     memset(v->d,DB_MEMFILL,v->siz+HMEMPAD+TMEMPAD);
     if (!freeheap) {
        v->next=0;
	freeheap=v;
     } else {
	v->next=freeheap;
	freeheap=v;
     }
   }
}


int db_heap_error(void* vp,int head) {
    dbst *v=vp;
    int rc=db_fatal;
    
    return rc;
}

