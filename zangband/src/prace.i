%module prace

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
	obj->r_adj[A_STR] = $source;
}

%typemap(memberout) s16b str_mod
{
	$target = obj->r_adj[A_STR];
}

%typemap(memberin) s16b int_mod
{
	obj->r_adj[A_INT] = $source;
}

%typemap(memberout) s16b int_mod
{
	$target = obj->r_adj[A_INT];
}

%typemap(memberin) s16b wis_mod
{
	obj->r_adj[A_WIS] = $source;
}

%typemap(memberout) s16b wis_mod
{
	$target = obj->r_adj[A_WIS];
}

%typemap(memberin) s16b dex_mod
{
	obj->r_adj[A_DEX] = $source;
}

%typemap(memberout) s16b dex_mod
{
	$target = obj->r_adj[A_DEX];
}

%typemap(memberin) s16b con_mod
{
	obj->r_adj[A_CON] = $source;
}

%typemap(memberout) s16b con_mod
{
	$target = obj->r_adj[A_CON];
}

%typemap(memberin) s16b chr_mod
{
	obj->r_adj[A_CHR] = $source;
}

%typemap(memberout) s16b chr_mod
{
	$target = obj->r_adj[A_CHR];
}


/*
 * Player racial info
 */
struct player_race
{
	cptr title;			/* Type of race */

	s16b r_adj[6];		/* Racial stat bonuses */

	s16b r_dis;			/* disarming */
	s16b r_dev;			/* magic devices */
	s16b r_sav;			/* saving throw */
	s16b r_stl;			/* stealth */
	s16b r_srh;			/* search ability */
	s16b r_fos;			/* search frequency */
	s16b r_thn;			/* combat (normal) */
	s16b r_thb;			/* combat (shooting) */

	byte r_mhp;			/* Race hit-dice modifier */
	byte r_exp;			/* Race experience factor */

	byte b_age;			/* base age */
	byte m_age;			/* mod age */

	byte m_b_ht;		/* base height (males) */
	byte m_m_ht;		/* mod height (males) */
	byte m_b_wt;		/* base weight (males) */
	byte m_m_wt;		/* mod weight (males) */

	byte f_b_ht;		/* base height (females) */
	byte f_m_ht;		/* mod height (females)	  */
	byte f_b_wt;		/* base weight (females) */
	byte f_m_wt;		/* mod weight (females) */

	byte infra;			/* Infra-vision	range */

    u16b choice;        /* Legal class choices */

	/* "Virtual" members */
	s16b str_mod;
	s16b int_mod;
	s16b wis_mod;
	s16b dex_mod;
	s16b con_mod;
	s16b chr_mod;

	%addmethods
	{
		player_race(void)
		{
			rp_ptr = (player_race*) malloc(sizeof(player_race));
			return rp_ptr;
		}

		~player_race(void)
		{
			free(self);
		}
	}
};
