%module realms

%{

#include "angband.h"

%}


typedef char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;


%{
	player_magic* get_player_magic(int class)
	{
		return &magic_info[class];
	}
%}


extern player_magic *mp_ptr;
extern u32b spell_learned1;
extern u32b spell_learned2;
extern u32b spell_worked1;
extern u32b spell_worked2;
extern u32b spell_forgotten1;
extern u32b spell_forgotten2;
extern byte spell_order[64];

extern player_magic magic_info[MAX_CLASS];


/*
 * Information about the player's "magic"
 *
 * Note that a player with a "spell_book" of "zero" is illiterate.
 */

struct player_magic
{
	int spell_book;		/* Tval of spell books (if any) */
	int spell_xtra;		/* Something for later */

	int spell_stat;		/* Stat for spells (if any)  */
	int spell_type;		/* Spell type (mage/priest) */

	int spell_first;		/* Level of first spell */
	int spell_weight;		/* Weight that hurts spells */

    magic_type info[MAX_REALM][32];    /* The available spells */

	%addmethods
	{
		player_magic(void)
		{
			mp_ptr = (player_magic*) malloc(sizeof(player_magic));
			return mp_ptr;
		}

		~player_magic(void)
		{
			free(self);
		}
	}
};

extern player_magic* get_player_magic(int class);
