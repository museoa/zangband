%module random

%{

#include "angband.h"

%}


typedef char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;

/**** Available Variables ****/


extern bool Rand_quick;
extern u32b Rand_value;
extern u16b Rand_place;
extern u32b Rand_state[RAND_DEG];


/**** Available Functions ****/


extern void Rand_state_init(u32b seed);
extern s32b Rand_mod(s32b m);
extern s32b Rand_div(u32b m);
extern s16b randnor(int mean, int stand);
extern s16b damroll(int num, int sides);
extern s16b maxroll(int num, int sides);

%{
	s32b rand_int_temp(u32b m)
	{
		return rand_int(m);
	}
%}

%name(rand_int) extern s32b rand_int_temp(u32b m);

/*
 * Generates a random long integer X where O<=X<M.
 * The integer X falls along a uniform distribution.
 * For example, if M is 100, you get "percentile dice"
 */
#define rand_int(M) \
	(Rand_div(M))

%{
	/*
	 * Generates a random long integer X where A<=X<=B
	 * The integer X falls along a uniform distribution.
	 * Note: rand_range(0,N-1) == rand_int(N)
	 */
	s32b rand_range_temp(u32b min, u32b max)
	{
		return rand_range(min, max);
	}
%}

%name(rand_range) extern s32b rand_range_temp(u32b min, u32b max);


/*
 * Generate a random long integer X where A-D<=X<=A+D
 * The integer X falls along a uniform distribution.
 * Note: rand_spread(A,D) == rand_range(A-D,A+D)
 */
#define rand_spread(A,D) \
	((A) + (rand_int(1+(D)+(D))) - (D))

%{
	s32b randint_temp(u32b m)
	{
		return randint(m);
	}
%}

%name(randint) extern s32b randint_temp(u32b m);

/*
 * Generate a random long integer X where 1<=X<=M
 * Also, "correctly" handle the case of M<=1
 */
#define randint(M) \
	(rand_int(M) + 1)


/*
 * Evaluate to TRUE "P" percent of the time
 */
#define magik(P) \
	(rand_int(100) < (P))


#define one_in_(X) \
	(rand_int(X) == 0)
