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


/*
 * Player racial info
 */
struct player_race
{
	cptr title;			/* Type of race */

	s16b r_adj[A_MAX];		/* Racial stat bonuses */

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

		s16b get_r_adj(int i)
		{
			return self->r_adj[i];
		}

		void set_r_adj(int i, s16b val)
		{
			self->r_adj[i] = val;
		}
	}
};


/*
 * Player sex info
 */

typedef struct player_sex player_sex;

struct player_sex
{
	cptr title;			/* Type of sex */

	cptr winner;		/* Name of winner */

	%addmethods
	{
		player_sex(void)
		{
			sp_ptr = (player_sex*)malloc(sizeof(player_sex));
			return sp_ptr;
		}

		~player_sex(void)
		{
			free(self);
		}
	}
};
