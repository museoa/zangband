%module spells

%{

#include "angband.h"

%}


typedef const char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;


/* Detection spells */
extern bool detect_traps(void);
extern bool detect_doors(void);
extern bool detect_stairs(void);
extern bool detect_treasure(void);
extern bool detect_objects_gold(void);
extern bool detect_objects_normal(void);
extern bool detect_objects_magic(void);
extern bool detect_monsters_normal(void);
extern bool detect_monsters_invis(void);
extern bool detect_monsters_evil(void);
extern bool detect_monsters_xxx(u32b match_flag);
extern bool detect_monsters_string(cptr);
extern bool detect_monsters_nonliving(void);
extern bool detect_all(void);
