%module pclass

%{

#include "angband.h"

%}


typedef char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;

%typemap(memberin) cptr title
{
	$target = string_make($source);
}

%typemap(memberout) cptr title
{
	$target = $source;
}


/*
 * Player class info
 */

struct player_class
{
	cptr title;			/* Type of class */

	s16b c_adj[6];		/* Class stat modifier */

	s16b c_dis;			/* class disarming */
	s16b c_dev;			/* class magic devices */
	s16b c_sav;			/* class saving throws */
	s16b c_stl;			/* class stealth */
	s16b c_srh;			/* class searching ability */
	s16b c_fos;			/* class searching frequency */
	s16b c_thn;			/* class to hit (normal) */
	s16b c_thb;			/* class to hit (bows) */

	s16b x_dis;			/* extra disarming */
	s16b x_dev;			/* extra magic devices */
	s16b x_sav;			/* extra saving throws */
	s16b x_stl;			/* extra stealth */
	s16b x_srh;			/* extra searching ability */
	s16b x_fos;			/* extra searching frequency */
	s16b x_thn;			/* extra to hit (normal) */
	s16b x_thb;			/* extra to hit (bows) */

	s16b c_mhp;			/* Class hit-dice adjustment */
	s16b c_exp;			/* Class experience factor */

	byte pet_upkeep_div; /* Pet upkeep divider */

	%addmethods
	{
		player_class(void)
		{
			cp_ptr = (player_class*) malloc(sizeof(player_class));
			return cp_ptr;
		}

		~player_class(void)
		{
			free(self);
		}

		s16b get_c_adj(int i)
		{
			return self->c_adj[i];
		}

		void set_c_adj(int i, s16b val)
		{
			self->c_adj[i] = val;
		}
	}
};
