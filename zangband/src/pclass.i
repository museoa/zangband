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

%typemap(memberin) s16b str_mod
{
	obj->c_adj[A_STR] = $source;
}

%typemap(memberout) s16b str_mod
{
	$target = obj->c_adj[A_STR];
}

%typemap(memberin) s16b int_mod
{
	obj->c_adj[A_INT] = $source;
}

%typemap(memberout) s16b int_mod
{
	$target = obj->c_adj[A_INT];
}

%typemap(memberin) s16b wis_mod
{
	obj->c_adj[A_WIS] = $source;
}

%typemap(memberout) s16b wis_mod
{
	$target = obj->c_adj[A_WIS];
}

%typemap(memberin) s16b dex_mod
{
	obj->c_adj[A_DEX] = $source;
}

%typemap(memberout) s16b dex_mod
{
	$target = obj->c_adj[A_DEX];
}

%typemap(memberin) s16b con_mod
{
	obj->c_adj[A_CON] = $source;
}

%typemap(memberout) s16b con_mod
{
	$target = obj->c_adj[A_CON];
}

%typemap(memberin) s16b chr_mod
{
	obj->c_adj[A_CHR] = $source;
}

%typemap(memberout) s16b chr_mod
{
	$target = obj->c_adj[A_CHR];
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

	/* "Virtual" members */
	s16b str_mod;
	s16b int_mod;
	s16b wis_mod;
	s16b dex_mod;
	s16b con_mod;
	s16b chr_mod;

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
	}
};
