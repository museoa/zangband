%module terrain

%{

#include "angband.h"

%}


typedef const char *cptr;
typedef unsigned char byte;
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;


/*
 * Information about terrain "features"
 */
struct feature_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte d_attr;		/* Default feature attribute */
	char d_char;		/* Default feature character */


	byte x_attr;		/* Desired feature attribute */
	char x_char;		/* Desired feature character */

	%addmethods
	{
		feature_type(void)
		{
			return (feature_type*) malloc(sizeof(feature_type));
		}

		~feature_type(void)
		{
			free(self);
		}
	}
};

