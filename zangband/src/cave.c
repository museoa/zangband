/* CVS: Last edit by $Author$ on $Date$ */
/* File: cave.c */

/* Purpose: low level dungeon routines -BEN- */


#include "angband.h"


/*
 * Support for Adam Bolt's tileset, lighting and transparency effects
 * by Robert Ruehlmann (rr9@angband.org)
 */


/*
 * Distance between two points via Newton-Raphson technique
 */
int distance(int y1, int x1, int y2, int x2)
{
	int dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
	int dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);

	/* Squared distance */
	int target = (dy * dy) + (dx * dx);

	/* Approximate distance: hypot(dy,dx) = max(dy,dx) + min(dy,dx) / 2 */
	int d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));

	int err;

	/* Simple case */
	if (!dy || !dx) return d;

	while (1)
	{
		/* Approximate error */
		err = (target / d - d) / 2;

		/* No error - we are done */
		if (!err) break;

		/* Adjust distance */
		d += err;
	}

	return d;
}


/*
 * Return TRUE if the given feature is a trap
 */
bool is_trap(int feat)
{
	switch (feat)
	{
		case FEAT_TRAP_TRAPDOOR:
		case FEAT_TRAP_PIT:
		case FEAT_TRAP_SPIKED_PIT:
		case FEAT_TRAP_POISON_PIT:
		case FEAT_TRAP_TY_CURSE:
		case FEAT_TRAP_TELEPORT:
		case FEAT_TRAP_FIRE:
		case FEAT_TRAP_ACID:
		case FEAT_TRAP_SLOW:
		case FEAT_TRAP_LOSE_STR:
		case FEAT_TRAP_LOSE_DEX:
		case FEAT_TRAP_LOSE_CON:
		case FEAT_TRAP_BLIND:
		case FEAT_TRAP_CONFUSE:
		case FEAT_TRAP_POISON:
		case FEAT_TRAP_SLEEP:
		case FEAT_TRAP_TRAPS:
		{
			/* A trap */
			return (TRUE);
		}
		default:
		{
			/* Not a trap */
			return (FALSE);
		}
	}
}


/*
 * A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
 * 4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu.
 *
 * Returns TRUE if a line of sight can be traced from (x1,y1) to (x2,y2).
 *
 * The LOS begins at the center of the tile (x1,y1) and ends at the center of
 * the tile (x2,y2).  If los() is to return TRUE, all of the tiles this line
 * passes through must be floor tiles, except for (x1,y1) and (x2,y2).
 *
 * We assume that the "mathematical corner" of a non-floor tile does not
 * block line of sight.
 *
 * Because this function uses (short) ints for all calculations, overflow may
 * occur if dx and dy exceed 90.
 *
 * Once all the degenerate cases are eliminated, the values "qx", "qy", and
 * "m" are multiplied by a scale factor "f1 = abs(dx * dy * 2)", so that
 * we can use integer arithmetic.
 *
 * We travel from start to finish along the longer axis, starting at the border
 * between the first and second tiles, where the y offset = .5 * slope, taking
 * into account the scale factor.  See below.
 *
 * Also note that this function and the "move towards target" code do NOT
 * share the same properties.  Thus, you can see someone, target them, and
 * then fire a bolt at them, but the bolt may hit a wall, not them.  However,
 * by clever choice of target locations, you can sometimes throw a "curve".
 *
 * Note that "line of sight" is not "reflexive" in all cases.
 *
 * Use the "projectable()" routine to test "spell/missile line of sight".
 *
 * Use the "update_view()" function to determine player line-of-sight.
 */
bool los(int y1, int x1, int y2, int x2)
{
	/* Delta */
	int dx, dy;

	/* Absolute */
	int ax, ay;

	/* Signs */
	int sx, sy;

	/* Fractions */
	int qx, qy;

	/* Scanners */
	int tx, ty;

	/* Scale factors */
	int f1, f2;

	/* Slope, or 1/Slope, of LOS */
	int m;

	cave_type *c_ptr;

	/* Extract the offset */
	dy = y2 - y1;
	dx = x2 - x1;

	/* Extract the absolute offset */
	ay = ABS(dy);
	ax = ABS(dx);


	/* Handle adjacent (or identical) grids */
	if ((ax < 2) && (ay < 2)) return (TRUE);


	/* Paranoia -- require "safe" origin */
	/* if (!in_bounds(y1, x1)) return (FALSE); */


	/* Directly South/North */
	if (!dx)
	{
		/* South -- check for walls */
		if (dy > 0)
		{
			for (ty = y1 + 1; ty < y2; ty++)
			{
				c_ptr = area(ty, x1);
				if (!cave_floor_grid(c_ptr)) return (FALSE);
			}
		}

		/* North -- check for walls */
		else
		{
			for (ty = y1 - 1; ty > y2; ty--)
			{
				c_ptr = area(ty, x1);
				if (!cave_floor_grid(c_ptr)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}

	/* Directly East/West */
	if (!dy)
	{
		/* East -- check for walls */
		if (dx > 0)
		{
			for (tx = x1 + 1; tx < x2; tx++)
			{
				c_ptr = area(y1, tx);
				if (!cave_floor_grid(c_ptr)) return (FALSE);
			}
		}

		/* West -- check for walls */
		else
		{
			for (tx = x1 - 1; tx > x2; tx--)
			{
				c_ptr = area(y1, tx);
				if (!cave_floor_grid(c_ptr)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}


	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;


	/* Vertical "knights" */
	if (ax == 1)
	{
		if (ay == 2)
		{
			c_ptr = area(y1 + sy, x1);
			if (cave_floor_grid(c_ptr)) return (TRUE);
		}
	}

	/* Horizontal "knights" */
	else if (ay == 1)
	{
		if (ax == 2)
		{
			c_ptr = area(y1, x1 + sx);
			if (cave_floor_grid(c_ptr)) return (TRUE);
		}
	}


	/* Calculate scale factor div 2 */
	f2 = (ax * ay);

	/* Calculate scale factor */
	f1 = f2 << 1;


	/* Travel horizontally */
	if (ax >= ay)
	{
		/* Let m = dy / dx * 2 * (dy * dx) = 2 * dy * dy */
		m = ay * ay << 1;

		tx = x1 + sx;

		/* Consider the special case where slope == 1. */
		if (ax == ay)
		{
			ty = y1 + sy;
			qy = -f2;
		}
		else
		{
			ty = y1;
			qy = f2;
		}

		/* Note (below) the case (qy == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (x2 - tx)
		{
			c_ptr = area(ty, tx);
			if (!cave_floor_grid(c_ptr)) return (FALSE);

			qy += m;

			if (qy < f2)
			{
				tx += sx;
			}
			else if (qy > f2)
			{
				ty += sy;
				c_ptr = area(ty, tx);
				if (!cave_floor_grid(c_ptr)) return (FALSE);
				qy -= f1;
				tx += sx;
			}
			else
			{
				ty += sy;
				qy -= f1;
				tx += sx;
			}
		}
	}

	/* Travel vertically */
	else
	{
		/* Let m = dx / dy * 2 * (dx * dy) = 2 * dx * dx */
		m = ax * ax << 1;

		ty = y1 + sy;

		if (ax == ay)
		{
			tx = x1 + sx;
			qx = -f2;
		}
		else
		{
			tx = x1;
			qx = f2;
		}

		/* Note (below) the case (qx == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (y2 - ty)
		{
			c_ptr = area(ty, tx);
			if (!cave_floor_grid(c_ptr)) return (FALSE);

			qx += m;

			if (qx < f2)
			{
				ty += sy;
			}
			else if (qx > f2)
			{
				tx += sx;
				c_ptr = area(ty, tx);
				if (!cave_floor_grid(c_ptr)) return (FALSE);
				qx -= f1;
				ty += sy;
			}
			else
			{
				tx += sx;
				qx -= f1;
				ty += sy;
			}
		}
	}

	/* Assume los */
	return (TRUE);
}






/*
 * Can the player "see" the given grid in detail?
 *
 * He must have vision, illumination, and line of sight.
 *
 * Note -- "CAVE_LITE" is only set if the "torch" has "los()".
 * So, given "CAVE_LITE", we know that the grid is "fully visible".
 *
 * Note that "CAVE_GLOW" makes little sense for a wall, since it would mean
 * that a wall is visible from any direction.  That would be odd.  Except
 * under wizard light, which might make sense.  Thus, for walls, we require
 * not only that they be "CAVE_GLOW", but also, that they be adjacent to a
 * grid which is not only "CAVE_GLOW", but which is a non-wall, and which is
 * in line of sight of the player.
 *
 * This extra check is expensive, but it provides a more "correct" semantics.
 *
 * Note that "glowing walls" are only considered to be "illuminated" if the
 * grid which is next to the wall in the direction of the player is also a
 * "glowing" grid.  This prevents the player from being able to "see" the
 * walls of illuminated rooms from a corridor outside the room.
 */
bool player_can_see_bold(int y, int x)
{
	int xx, yy;

	cave_type *c_ptr;

	/* Blind players see nothing */
	if (p_ptr->blind) return (FALSE);

	/* Access the cave grid */
	c_ptr = area(y, x);

	/* Note that "torch-lite" yields "illumination" */
	if (c_ptr->info & (CAVE_LITE)) return (TRUE);

	/* Require line of sight to the grid */
	if (!player_has_los_grid(c_ptr)) return (FALSE);

	/* Require "perma-lite" of the grid */
	if (!(c_ptr->info & (CAVE_GLOW))) return (FALSE);

	/* Floors are simple */
	if (cave_floor_grid(c_ptr)) return (TRUE);

	/* Hack -- move towards player */
	yy = (y < py) ? (y + 1) : (y > py) ? (y - 1) : y;
	xx = (x < px) ? (x + 1) : (x > px) ? (x - 1) : x;

	/* Check for "local" illumination */
	if (area(yy,xx)->info & (CAVE_GLOW))
	{
		/* Assume the wall is really illuminated */
		return (TRUE);
	}

	/* Assume not visible */
	return (FALSE);
}



/*
 * Returns true if the player's grid is dark
 */
bool no_lite(void)
{
	return (!player_can_see_bold(py, px));
}




/*
 * Determine if a given location may be "destroyed"
 *
 * Used by destruction spells, and for placing stairs, etc.
 */
bool cave_valid_grid(cave_type *c_ptr)
{
	s16b this_o_idx, next_o_idx = 0;


	/* Forbid perma-grids */
	if (cave_perma_grid(c_ptr)) return (FALSE);

	/* Check objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Forbid artifact grids */
		if ((o_ptr->art_name) || artifact_p(o_ptr)) return (FALSE);
	}

	/* Accept */
	return (TRUE);
}




/*
 * Hack -- Legal monster codes
 */
static cptr image_monster_hack = \
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


/*
 * Mega-Hack -- Hallucinatory monster
 */
static void image_monster(byte *ap, char *cp)
{
	int n = strlen(image_monster_hack);

	/* Random symbol from set above */
	if (use_graphics)
	{
		(*cp) = r_info[randint(max_r_idx-1)].x_char;
		(*ap) = r_info[randint(max_r_idx-1)].x_attr;
	}
	else
	/* Text mode */
	{
		(*cp) = (image_monster_hack[rand_int(n)]);

		/* Random color */
		(*ap) = randint(15);
	}
}



/*
 * Hack -- Legal object codes
 */
static cptr image_object_hack = \
"?/|\\\"!$()_-=[]{},~";


/*
 * Mega-Hack -- Hallucinatory object
 */
static void image_object(byte *ap, char *cp)
{
	int n = strlen(image_object_hack);

	if (use_graphics)
	{
		(*cp) = k_info[randint(max_k_idx-1)].x_char;
		(*ap) = k_info[randint(max_k_idx-1)].x_attr;
	}
	else
	{
		(*cp) = (image_object_hack[rand_int(n)]);

		/* Random color */
		(*ap) = randint(15);
	}
}



/*
 * Hack -- Random hallucination
 */
static void image_random(byte *ap, char *cp)
{
	/* Normally, assume monsters */
	if (rand_int(100) < 75)
	{
		image_monster(ap, cp);
	}

	/* Otherwise, assume objects */
	else
	{
		image_object(ap, cp);
	}
}

/*
 * Not using graphical tiles for this feature?
 */
#define is_ascii_graphics(C , A) \
    (!(((C) & 0x80) && ((A) & 0x80)))

/*
 * The 16x16 tile of the terrain supports lighting
 */
static bool feat_supports_lighting(byte feat)
{
	if ((feat == FEAT_OPEN) ||
	(feat == FEAT_BROKEN) ||
	((feat >= FEAT_DOOR_HEAD) && (feat <= FEAT_DOOR_TAIL)) ||
	((feat >= FEAT_PATTERN_START) && (feat <= FEAT_PATTERN_XTRA2)) ||
	((feat >= FEAT_SHOP_HEAD) && (feat <= FEAT_SHOP_TAIL)))
		return FALSE;

	return TRUE;
}

/*
 * This array lists the effects of "brightness" on various "base" colours.
 *
 * This is used to do dynamic lighting effects in ascii :-)
 * At the moment, only the various "floor" tiles are affected.
 *
 * The layout of the array is [x][0] = light and [x][1] = dark.
 */

static byte lighting_colours[16][2] =
{
	/* TERM_DARK */
	{TERM_L_DARK, TERM_DARK},

	/* TERM_WHITE */
	{TERM_YELLOW, TERM_SLATE},

	/* TERM_SLATE */
	{TERM_WHITE, TERM_L_DARK},

	/* TERM_ORANGE */
	{TERM_YELLOW, TERM_UMBER},

	/* TERM_RED */
	{TERM_RED, TERM_RED},

	/* TERM_GREEN */
	{TERM_L_GREEN, TERM_GREEN},

	/* TERM_BLUE */
	{TERM_BLUE, TERM_BLUE},

	/* TERM_UMBER */
	{TERM_L_UMBER, TERM_RED},

	/* TERM_L_DARK */
	{TERM_SLATE, TERM_L_DARK},

	/* TERM_L_WHITE */
	{TERM_WHITE, TERM_SLATE},

	/* TERM_VIOLET */
	{TERM_L_RED, TERM_BLUE},

	/* TERM_YELLOW */
	{TERM_YELLOW, TERM_ORANGE},

	/* TERM_L_RED */
	{TERM_L_RED, TERM_L_RED},

	/* TERM_L_GREEN */
	{TERM_YELLOW, TERM_GREEN},

	/* TERM_L_BLUE */
	{TERM_L_BLUE, TERM_L_BLUE},

	/* TERM_L_UMBER */
	{TERM_YELLOW, TERM_UMBER}
};


/*
 * Extract the attr/char to display at the given (legal) map location
 *
 * Basically, we "paint" the chosen attr/char in several passes, starting
 * with any known "terrain features" (defaulting to darkness), then adding
 * any known "objects", and finally, adding any known "monsters".  This
 * is not the fastest method but since most of the calls to this function
 * are made for grids with no monsters or objects, it is fast enough.
 *
 * Note that this function, if used on the grid containing the "player",
 * will return the attr/char of the grid underneath the player, and not
 * the actual player attr/char itself, allowing a lot of optimization
 * in various "display" functions.
 *
 * Note that the "zero" entry in the feature/object/monster arrays are
 * used to provide "special" attr/char codes, with "monster zero" being
 * used for the player attr/char, "object zero" being used for the "stack"
 * attr/char, and "feature zero" being used for the "nothing" attr/char,
 * though this function makes use of only "feature zero".
 *
 * Note that monsters can have some "special" flags, including "ATTR_MULTI",
 * which means their color changes, and "ATTR_CLEAR", which means they take
 * the color of whatever is under them, and "CHAR_CLEAR", which means that
 * they take the symbol of whatever is under them.  Technically, the flag
 * "CHAR_MULTI" is supposed to indicate that a monster looks strange when
 * examined, but this flag is currently ignored.  All of these flags are
 * ignored if the "avoid_other" option is set, since checking for these
 * conditions is expensive and annoying on some systems.
 *
 * Currently, we do nothing with multi-hued objects, because there are
 * not any.  If there were, they would have to set "shimmer_objects"
 * when they were created, and then new "shimmer" code in "dungeon.c"
 * would have to be created handle the "shimmer" effect, and the code
 * in "cave.c" would have to be updated to create the shimmer effect.
 *
 * Note the effects of hallucination.  Objects always appear as random
 * "objects", monsters as random "monsters", and normal grids occasionally
 * appear as random "monsters" or "objects", but note that these random
 * "monsters" and "objects" are really just "colored ascii symbols".
 *
 * Note that "floors" and "invisible traps" (and "zero" features) are
 * drawn as "floors" using a special check for optimization purposes,
 * and these are the only features which get drawn using the special
 * lighting effects activated by "view_special_lite".
 *
 * Note the use of the "mimic" field in the "terrain feature" processing,
 * which allows any feature to "pretend" to be another feature.  This is
 * used to "hide" secret doors, and to make all "doors" appear the same,
 * and all "walls" appear the same, and "hidden" treasure stay hidden.
 * It is possible to use this field to make a feature "look" like a floor,
 * but the "special lighting effects" for floors will not be used.
 *
 * Note the use of the new "terrain feature" information.  Note that the
 * assumption that all interesting "objects" and "terrain features" are
 * memorized allows extremely optimized processing below.  Note the use
 * of separate flags on objects to mark them as memorized allows a grid
 * to have memorized "terrain" without granting knowledge of any object
 * which may appear in that grid.
 *
 * Note the efficient code used to determine if a "floor" grid is
 * "memorized" or "viewable" by the player, where the test for the
 * grid being "viewable" is based on the facts that (1) the grid
 * must be "lit" (torch-lit or perma-lit), (2) the grid must be in
 * line of sight, and (3) the player must not be blind, and uses the
 * assumption that all torch-lit grids are in line of sight.
 *
 * Note that floors (and invisible traps) are the only grids which are
 * not memorized when seen, so only these grids need to check to see if
 * the grid is "viewable" to the player (if it is not memorized).  Since
 * most non-memorized grids are in fact walls, this induces *massive*
 * efficiency, at the cost of *forcing* the memorization of non-floor
 * grids when they are first seen.  Note that "invisible traps" are
 * always treated exactly like "floors", which prevents "cheating".
 *
 * Note the "special lighting effects" which can be activated for floor
 * grids using the "view_special_lite" option (for "white" floor grids),
 * causing certain grids to be displayed using special colors.  If the
 * player is "blind", we will use "dark gray", else if the grid is lit
 * by the torch, and the "view_yellow_lite" option is set, we will use
 * "yellow", else if the grid is "dark", we will use "dark gray", else
 * if the grid is not "viewable", and the "view_bright_lite" option is
 * set, and the we will use "slate" (gray).  We will use "white" for all
 * other cases, in particular, for illuminated viewable floor grids.
 *
 * Note the "special lighting effects" which can be activated for wall
 * grids using the "view_granite_lite" option (for "white" wall grids),
 * causing certain grids to be displayed using special colors.  If the
 * player is "blind", we will use "dark gray", else if the grid is lit
 * by the torch, and the "view_yellow_lite" option is set, we will use
 * "yellow", else if the "view_bright_lite" option is set, and the grid
 * is not "viewable", or is "dark", or is glowing, but not when viewed
 * from the player's current location, we will use "slate" (gray).  We
 * will use "white" for all other cases, in particular, for correctly
 * illuminated viewable wall grids.
 *
 * Note that, when "view_granite_lite" is set, we use an inline version
 * of the "player_can_see_bold()" function to check the "viewability" of
 * grids when the "view_bright_lite" option is set, and we do NOT use
 * any special colors for "dark" wall grids, since this would allow the
 * player to notice the walls of illuminated rooms from a hallway that
 * happened to run beside the room.  The alternative, by the way, would
 * be to prevent the generation of hallways next to rooms, but this
 * would still allow problems when digging towards a room.
 *
 * Note that bizarre things must be done when the "attr" and/or "char"
 * codes have the "high-bit" set, since these values are used to encode
 * various "special" pictures in some versions, and certain situations,
 * such as "multi-hued" or "clear" monsters, cause the attr/char codes
 * to be "scrambled" in various ways.
 */
#ifdef USE_TRANSPARENCY
void map_info(int y, int x, byte *ap, char *cp, byte *tap, char *tcp)
#else /* USE_TRANSPARENCY */
void map_info(int y, int x, byte *ap, char *cp)
#endif /* USE_TRANSPARENCY */
{
	cave_type *c_ptr;

	feature_type *f_ptr;

	s16b this_o_idx, next_o_idx = 0;

	byte feat;
	byte info;

	byte a;
	byte c;

	/* Get the cave */
	c_ptr = area(y,x);

	/* Feature code */
	feat = c_ptr->feat;
	info = c_ptr->info;

	/* Apply mimic field */
	feat = f_info[feat].mimic;

	/* Hack - Non LOS blocking terrains */
	if (cave_floor_grid(c_ptr))
	{
		/* Memorized (or visible) floor */
		if   ((info & CAVE_MARK) ||
		    (((info & CAVE_LITE) ||
		     ((info & CAVE_GLOW) &&
		      (info & CAVE_VIEW))) &&
		     !p_ptr->blind))
		{
			/* Access floor */
			f_ptr = &f_info[feat];

			/* Normal char */
			c = f_ptr->x_char;

			/* Normal attr */
			a = f_ptr->x_attr;

			/* Special lighting effects */
			if (view_special_lite && (!use_transparency || feat_supports_lighting(feat) || is_ascii_graphics(c,a)))
			{
				/* Handle "blind" */
				if (p_ptr->blind)
				{
					if (is_ascii_graphics(c,a))
					{
						/* Use darkened colour */
						a = lighting_colours[a][1];
					}
					else if (use_transparency && feat_supports_lighting(feat))
					{
						/* Use a dark tile */
						c++;
					}
				}

				/* Handle "torch-lit" grids */
				else if (info & CAVE_LITE)
				{
					/* Torch lite */
					if (view_yellow_lite)
					{
						if (is_ascii_graphics(c,a))
						{
							/* Use lightened colour */
							a = lighting_colours[a][0];
						}
						else if (use_transparency &&
							 feat_supports_lighting(feat))
						{
							/* Use a brightly lit tile */
							c += 2;
						}
					}
				}

				/* Handle "dark" grids */
				else if (!(info & CAVE_GLOW))
				{
					if (is_ascii_graphics(c,a))
					{
						/* Use darkened colour */
						a = lighting_colours[a][1];
					}
					else if (use_transparency && feat_supports_lighting(feat))
					{
						/* Use a dark tile */
						c++;
					}
				}

				/* Handle "out-of-sight" grids */
				else if (!(info & CAVE_VIEW))
				{
					/* Special flag */
					if (view_bright_lite)
					{
						if (is_ascii_graphics(c,a))
						{
							/* Use darkened colour */
							a = lighting_colours[a][1];
						}
						else if (use_transparency && feat_supports_lighting(feat))
						{
							/* Use a dark tile */
							c++;
						}
					}
				}
			}
		}

		/* Unknown */
		else
		{
			/* Access darkness */
			f_ptr = &f_info[FEAT_NONE];

			/* Normal attr */
			a = f_ptr->x_attr;

			/* Normal char */
			c = f_ptr->x_char;
		}
	}

	/* Non floors */
	else
	{
		/* Memorized grids */
		if ((info & CAVE_MARK) && view_granite_lite)
		{
			/* Access feature */
			f_ptr = &f_info[feat];

			/* Normal char */
			c = f_ptr->x_char;

			/* Normal attr */
			a = f_ptr->x_attr;

			/* Handle "blind" */
			if (p_ptr->blind)
			{
				if (is_ascii_graphics(c,a))
				{
					/* Use darkened colour */
					a = lighting_colours[a][1];
				}
				else if (use_transparency && feat_supports_lighting(feat))
				{
					/* Use a dark tile */
					c++;
				}
			}

			/* Handle "torch-lit" grids */
			else if (info & CAVE_LITE)
			{
				/* Torch lite */
				if (view_yellow_lite && (!use_transparency || feat_supports_lighting(feat) || is_ascii_graphics(c,a)))
				{
					if (is_ascii_graphics(c,a))
					{
						/* Use lightened colour */
						a = lighting_colours[a][0];
					}
					else if (use_transparency && feat_supports_lighting(feat))
					{
						/* Use a brightly lit tile */
						c += 2;
					}
				}
			}

			/* Handle "view_bright_lite" */
			else if (view_bright_lite && (!use_transparency || feat_supports_lighting(feat) || is_ascii_graphics(c,a)))

			{
				/* Not viewable */
				if (!(info & CAVE_VIEW))
				{
					if (is_ascii_graphics(c,a))
					{
						/* Use darkened colour */
						a = lighting_colours[a][1];
					}
					else if (use_transparency && feat_supports_lighting(feat))
					{
						/* Use a dark tile */
						c++;
					}
				}

				/* Not glowing */
				else if (!(info & CAVE_GLOW))
				{
					if (is_ascii_graphics(c,a))
					{
						/* Use darkened colour */
						a = lighting_colours[a][1];
					}
				}
			}
		}

		/* "Simple Lighting" */
		else
		{
			/* Access feature */
			f_ptr = &f_info[feat];

			/* Handle "blind" */
			if (!(info & CAVE_MARK))
			{
				/* Access darkness */
				f_ptr = &f_info[FEAT_NONE];
			}

			/* Normal attr */
			a = f_ptr->x_attr;

			/* Normal char */
			c = f_ptr->x_char;
		}
	}

	/* Hack -- rare random hallucination, except on outer dungeon walls */
	if (p_ptr->image && (feat < FEAT_PERM_SOLID) && !rand_int(256))
	{
		/* Hallucinate */
		image_random(ap, cp);
	}

#ifdef USE_TRANSPARENCY
	/* Save the terrain info for the transparency effects */
	(*tap) = a;
	(*tcp) = c;
#endif /* USE_TRANSPARENCY */

	/* Save the info */
	(*ap) = a;
	(*cp) = c;

	/* Objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorized objects */
		if (o_ptr->marked)
		{
			/* Normal char */
			(*cp) = object_char(o_ptr);

			/* Normal attr */
			(*ap) = object_attr(o_ptr);

			/* Hack -- hallucination */
			if (p_ptr->image) image_object(ap, cp);

			/* Done */
			break;
		}
	}


	/* Handle monsters */
	if (c_ptr->m_idx)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];

		/* Visible monster */
		if (m_ptr->ml)
		{
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Desired attr */
			a = r_ptr->x_attr;

			/* Desired char */
			c = r_ptr->x_char;

			/* Ignore weird codes */
			if (avoid_other)
			{
				/* Use char */
				(*cp) = c;

				/* Use attr */
				(*ap) = a;
			}

			/* Mimics' colors vary */
			else if (strchr("\"!=", c) && !(r_ptr->flags1 & RF1_UNIQUE))
			{
				/* Use char */
				(*cp) = c;

				/* Use semi-random attr */
				(*ap) = c_ptr->m_idx % 15 + 1;
			}

			/* Special attr/char codes */
			else if ((a & 0x80) && (c & 0x80))
			{
				/* Use char */
				(*cp) = c;

				/* Use attr */
				(*ap) = a;
			}

			/* Multi-hued monster */
			else if (r_ptr->flags1 & (RF1_ATTR_MULTI))
			{
				/* Is it a shapechanger? */
				if (r_ptr->flags2 & (RF2_SHAPECHANGER))
				{
					if (use_graphics)
					{
						(*cp) = r_info[randint(max_r_idx-1)].x_char;
						(*ap) = r_info[randint(max_r_idx-1)].x_attr;
					}
					else
					{
						(*cp) = (randint(25) == 1 ?
							image_object_hack[randint(strlen(image_object_hack))] :
							image_monster_hack[randint(strlen(image_monster_hack))]);
					}
				}
				else
					(*cp) = c;

				/* Multi-hued attr */
				if (r_ptr->flags2 & RF2_ATTR_ANY)
					(*ap) = randint(15);
				else switch (randint(7))
				{
					case 1:
						(*ap) = TERM_RED;
						break;
					case 2:
						(*ap) = TERM_L_RED;
						break;
					case 3:
						(*ap) = TERM_WHITE;
						break;
					case 4:
						(*ap) = TERM_L_GREEN;
						break;
					case 5:
						(*ap) = TERM_BLUE;
						break;
					case 6:
						(*ap) = TERM_L_DARK;
						break;
					case 7:
						(*ap) = TERM_GREEN;
						break;
				}
			}

			/* Normal monster (not "clear" in any way) */
			else if (!(r_ptr->flags1 & (RF1_ATTR_CLEAR | RF1_CHAR_CLEAR)))
			{
				/* Use char */
				(*cp) = c;

				/* Use attr */
				(*ap) = a;
			}

			/* Hack -- Bizarre grid under monster */
			else if ((*ap & 0x80) || (*cp & 0x80))
			{
				/* Use char */
				(*cp) = c;

				/* Use attr */
				(*ap) = a;
			}

			/* Normal */
			else
			{
				/* Normal (non-clear char) monster */
				if (!(r_ptr->flags1 & (RF1_CHAR_CLEAR)))
				{
					/* Normal char */
					(*cp) = c;
				}

				/* Normal (non-clear attr) monster */
				else if (!(r_ptr->flags1 & (RF1_ATTR_CLEAR)))
				{
					/* Normal attr */
					(*ap) = a;
				}
			}

			/* Hack -- hallucination */
			if (p_ptr->image)
			{
				/* Hallucinatory monster */
				image_monster(ap, cp);
			}
		}
	}

	/* Handle "player" */
	if ((y == py) && (x == px))
	{
		monster_race *r_ptr = &r_info[0];

		/* Get the "player" attr */
		a = r_ptr->x_attr;

		/* Get the "player" char */
		c = r_ptr->x_char;

#ifdef VARIABLE_PLAYER_GRAPH

		if (!streq(ANGBAND_GRAF, "new"))
		{
			if (!streq(ANGBAND_SYS,"ibm"))
			{
				if (use_graphics)
				{
					a = BMP_FIRST_PC_CLASS + p_ptr->pclass;
					c = BMP_FIRST_PC_RACE  + p_ptr->prace;
				}
			}
			else
			{
				if (use_graphics)
				{
					if (p_ptr->psex == SEX_FEMALE) c = (char)242;
					switch (p_ptr->pclass)
					{
						case CLASS_PALADIN:
							if (p_ptr->lev < 20)
								a = TERM_L_WHITE;
							else
								a = TERM_WHITE;
							c = 253;
							break;
						case CLASS_WARRIOR_MAGE:
							if (p_ptr->lev < 20)
								a = TERM_L_RED;
							else
								a = TERM_VIOLET;
							break;
						case CLASS_CHAOS_WARRIOR:
							a = rand_int(14) + 1;
							break;
						case CLASS_MAGE:
						case CLASS_HIGH_MAGE:
							if (p_ptr->lev < 20)
								a = TERM_L_RED;
							else
								a = TERM_RED;
							c = 248;
							break;
						case CLASS_PRIEST:
							if (p_ptr->lev < 20)
								a = TERM_L_BLUE;
							else
								a = TERM_BLUE;
							c = 248;
							break;
						case CLASS_RANGER:
							if (p_ptr->lev < 20)
								a = TERM_L_GREEN;
							else
								a = TERM_GREEN;
							break;
						case CLASS_ROGUE:
							if (p_ptr->lev < 20)
								a = TERM_SLATE;
							else
								a = TERM_L_DARK;
							break;
						case CLASS_WARRIOR:
							if (p_ptr->lev < 20)
								a = TERM_L_UMBER;
							else
								a = TERM_UMBER;
							break;
						case CLASS_MONK:
						case CLASS_MINDCRAFTER:
							if (p_ptr->lev < 20)
								a = TERM_L_UMBER;
							else
								a = TERM_UMBER;
							c = 248;
							break;
						default: /* Unknown */
							a = TERM_WHITE;
					}

					switch (p_ptr->prace)
					{
						case RACE_GNOME:
						case RACE_HOBBIT:
							c = 144;
							break;
						case RACE_DWARF:
							c = 236;
							break;
						case RACE_HALF_ORC:
							c = 243;
							break;
						case RACE_HALF_TROLL:
							c = 184;
							break;
						case RACE_ELF:
						case RACE_HALF_ELF:
						case RACE_HIGH_ELF:
							c = 223;
							break;
						case RACE_HALF_OGRE:
							c = 168;
							break;
						case RACE_HALF_GIANT:
						case RACE_HALF_TITAN:
						case RACE_CYCLOPS:
							c = 145;
							break;
						case RACE_YEEK:
							c = 209;
							break;
						case RACE_KLACKON:
							c = 229;
							break;
						case RACE_KOBOLD:
							c = 204;
							break;
						case RACE_NIBELUNG:
							c = 144;
							break;
						case RACE_DARK_ELF:
							c = 223;
							break;
						case RACE_DRACONIAN:
							if (p_ptr->lev < 20)
								c = 240;
							else if (p_ptr->lev < 40)
								c = 22;
							else
								c = 137;
							break;
						case RACE_MIND_FLAYER:
							c = 236;
							break;
						case RACE_IMP:
							c = 142;
							break;
						case RACE_GOLEM:
							c = 6;
							break;
						case RACE_SKELETON:
							if (p_ptr->pclass == CLASS_MAGE ||
								p_ptr->pclass == CLASS_PRIEST ||
								p_ptr->pclass == CLASS_HIGH_MAGE ||
								p_ptr->pclass == CLASS_MONK ||
								p_ptr->pclass == CLASS_MINDCRAFTER)
								c = 159;
							else
								c = 181;
							break;
						case RACE_ZOMBIE:
							c = 221;
							break;
						case RACE_VAMPIRE:
							c = 217;
							break;
						case RACE_SPECTRE:
							c = 241;
							break;
						case RACE_SPRITE:
							c = 244;
							break;
						case RACE_BEASTMAN:
							c = 154;
							break;
					}
				}
			}
		}

		

#endif /* VARIABLE_PLAYER_GRAPH */
		/* Save the info */
		(*ap) = a;
		(*cp) = c;
	}
}



/*
 * Moves the cursor to a given MAP (y,x) location
 */
void move_cursor_relative(int row, int col)
{
	/* Real co-ords convert to screen positions */
	row -= panel_row_prt;
	col -= panel_col_prt;

	/* Go there */
	Term_gotoxy(col, row);
}



/*
 * Place an attr/char pair at the given map coordinate, if legal.
 */
void print_rel(char c, byte a, int y, int x)
{
	/* Only do "legal" locations */
	if (panel_contains(y, x))
	{
		/* Hack -- fake monochrome */
		if (!use_graphics || streq(ANGBAND_SYS, "ibm"))
		{
			if (p_ptr->invuln || !use_color) a = TERM_WHITE;
			else if (p_ptr->wraith_form) a = TERM_L_DARK;
		}

		/* Draw the char using the attr */
		Term_draw(x-panel_col_prt, y-panel_row_prt, a, c);
	}
}





/*
 * Memorize interesting viewable object/features in the given grid
 *
 * This function should only be called on "legal" grids.
 *
 * This function will memorize the object and/or feature in the given
 * grid, if they are (1) viewable and (2) interesting.  Note that all
 * objects are interesting, all terrain features except floors (and
 * invisible traps) are interesting, and floors (and invisible traps)
 * are interesting sometimes (depending on various options involving
 * the illumination of floor grids).
 *
 * The automatic memorization of all objects and non-floor terrain
 * features as soon as they are displayed allows incredible amounts
 * of optimization in various places, especially "map_info()".
 *
 * Note that the memorization of objects is completely separate from
 * the memorization of terrain features, preventing annoying floor
 * memorization when a detected object is picked up from a dark floor,
 * and object memorization when an object is dropped into a floor grid
 * which is memorized but out-of-sight.
 *
 * This function should be called every time the "memorization" of
 * a grid (or the object in a grid) is called into question, such
 * as when an object is created in a grid, when a terrain feature
 * "changes" from "floor" to "non-floor", when any grid becomes
 * "illuminated" or "viewable", and when a "floor" grid becomes
 * "torch-lit".
 *
 * Note the relatively efficient use of this function by the various
 * "update_view()" and "update_lite()" calls, to allow objects and
 * terrain features to be memorized (and drawn) whenever they become
 * viewable or illuminated in any way, but not when they "maintain"
 * or "lose" their previous viewability or illumination.
 *
 * Note the butchered "internal" version of "player_can_see_bold()",
 * optimized primarily for the most common cases, that is, for the
 * non-marked floor grids.
 */
void note_spot(int y, int x)
{
	cave_type *c_ptr = area(y,x);

	s16b this_o_idx, next_o_idx = 0;


	/* Blind players see nothing */
	if (p_ptr->blind) return;

	/* Analyze non-torch-lit grids */
	if (!(c_ptr->info & (CAVE_LITE)))
	{
		/* Require line of sight to the grid */
		if (!(c_ptr->info & (CAVE_VIEW))) return;

		/* Require "perma-lite" of the grid */
		if (!(c_ptr->info & (CAVE_GLOW))) return;
	}


	/* Hack -- memorize objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorize objects */
		o_ptr->marked = TRUE;
	}


	/* Hack -- memorize grids */
	if (!(c_ptr->info & (CAVE_MARK)))
	{
		/* Handle floor grids first */
		if ((c_ptr->feat <= FEAT_INVIS) || (c_ptr->feat == FEAT_WALL_INVIS))
		{
			/* Option -- memorize all torch-lit floors */
			if (view_torch_grids && (c_ptr->info & (CAVE_LITE)))
			{
				/* Memorize */
				c_ptr->info |= (CAVE_MARK);
			}

			/* Option -- memorize all perma-lit floors */
			else if (view_perma_grids && (c_ptr->info & (CAVE_GLOW)))
			{
				/* Memorize */
				c_ptr->info |= (CAVE_MARK);
			}
		}

		/* Memorize normal grids */
		else if (cave_floor_grid(c_ptr))
		{
			/* Memorize */
			c_ptr->info |= (CAVE_MARK);
		}

		/* Memorize torch-lit walls */
		else if (c_ptr->info & (CAVE_LITE))
		{
			/* Memorize */
			c_ptr->info |= (CAVE_MARK);
		}

		/* Memorize certain non-torch-lit wall grids */
		else
		{
			int yy, xx;

			/* Hack -- move one grid towards player */
			yy = (y < py) ? (y + 1) : (y > py) ? (y - 1) : y;
			xx = (x < px) ? (x + 1) : (x > px) ? (x - 1) : x;

			/* Check for "local" illumination */
			if (area(yy,xx)->info & (CAVE_GLOW))
			{
				/* Memorize */
				c_ptr->info |= (CAVE_MARK);
			}
		}
	}
}

/*
 * This function is nearly identical to the above one - but since it
 * is used in lighting / darkening the wilderness - it doesn't have to
 * deal with walls next to lit rooms.
 *
 * This simplifies the code.
 *
 * Also note that FEAT_FLOOR is not common in the wilderness - this means
 * that the optimisations in note_spot() probably are not useful.
 */

void note_wild_spot(cave_type *c_ptr)
{
	s16b this_o_idx, next_o_idx = 0;


	/* Blind players see nothing */
	if (p_ptr->blind) return;

	/* Analyze non-torch-lit grids */
	if (!(c_ptr->info & (CAVE_LITE)))
	{
		/* Require line of sight to the grid */
		if (!(c_ptr->info & (CAVE_VIEW))) return;

		/* Require "perma-lite" of the grid */
		if (!(c_ptr->info & (CAVE_GLOW))) return;
	}


	/* Hack -- memorize objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorize objects */
		o_ptr->marked = TRUE;
	}


	/* Hack -- memorize grids */
	if (!(c_ptr->info & (CAVE_MARK)))
	{
		/* Handle floor grids first */
		if ((c_ptr->feat <= FEAT_INVIS) || (c_ptr->feat == FEAT_WALL_INVIS))
		{
			/* Option -- memorize all torch-lit floors */
			if (view_torch_grids && (c_ptr->info & (CAVE_LITE)))
			{
				/* Memorize */
				c_ptr->info |= (CAVE_MARK);
			}

			/* Option -- memorize all perma-lit floors */
			else if (view_perma_grids && (c_ptr->info & (CAVE_GLOW)))
			{
				/* Memorize */
				c_ptr->info |= (CAVE_MARK);
			}
		}

		/* Memorize normal grids */
		else if (cave_floor_grid(c_ptr))
		{
			/* Memorize */
			c_ptr->info |= (CAVE_MARK);
		}

		/* Memorize torch-lit walls */
		else if (c_ptr->info & (CAVE_LITE))
		{
			/* Memorize */
			c_ptr->info |= (CAVE_MARK);
		}
	}
}



void display_dungeon(void)
{
	int x, y;
	byte a;
	char c;

#ifdef USE_TRANSPARENCY
	byte ta;
	char tc;
#endif /* USE_TRANSPARENCY */

	for (x = px - Term->wid / 2 + 1; x <= px + Term->wid / 2; x++)
	{
		for (y = py - Term->hgt / 2 + 1; y <= py + Term->hgt / 2; y++)
		{
			if (in_bounds2(y, x))
			{

#ifdef USE_TRANSPARENCY
				/* Examine the grid */
				map_info(y, x, &a, &c, &ta, &tc);
#else /* USE_TRANSPARENCY */
				/* Examine the grid */
				map_info(y, x, &a, &c);
#endif /* USE_TRANSPARENCY */

				/* Hack -- fake monochrome */
				if (!use_graphics || streq(ANGBAND_SYS, "ibm"))
				{
					if (p_ptr->invuln || !use_color) a = TERM_WHITE;
					else if (p_ptr->wraith_form) a = TERM_L_DARK;
				}

#ifdef USE_TRANSPARENCY
				/* Hack -- Queue it */
				Term_queue_char(x - px + Term->wid / 2 - 1, y - py + Term->hgt / 2 - 1, a, c, ta, tc);
#else /* USE_TRANSPARENCY */
				/* Hack -- Queue it */
				Term_queue_char(x - px + Term->wid / 2 - 1, y - py + Term->hgt / 2 - 1, a, c);
#endif /* USE_TRANSPARENCY */

			}
			else
			{
				/* Clear out-of-bound tiles */

				/* Access darkness */
				feature_type *f_ptr = &f_info[FEAT_NONE];

				/* Normal attr */
				a = f_ptr->x_attr;

				/* Normal char */
				c = f_ptr->x_char;

#ifdef USE_TRANSPARENCY
				/* Hack -- Queue it */
				Term_queue_char(x - px + Term->wid / 2 - 1, y - py + Term->hgt / 2 - 1, a, c, ta, tc);
#else /* USE_TRANSPARENCY */
				/* Hack -- Queue it */
				Term_queue_char(x - px + Term->wid / 2 - 1, y - py + Term->hgt / 2 - 1, a, c);
#endif /* USE_TRANSPARENCY */
			}
		}
	}
}


/*
 * Redraw (on the screen) a given MAP location
 *
 * This function should only be called on "legal" grids
 */
void lite_spot(int y, int x)
{
	/* Redraw if on screen */
	if (panel_contains(y, x))
	{
		byte a;
		char c;

#ifdef USE_TRANSPARENCY
		byte ta;
		char tc;

		/* Examine the grid */
		map_info(y, x, &a, &c, &ta, &tc);
#else /* USE_TRANSPARENCY */
		/* Examine the grid */
		map_info(y, x, &a, &c);
#endif /* USE_TRANSPARENCY */

		/* Hack -- fake monochrome */
		if (!use_graphics || streq(ANGBAND_SYS, "ibm"))
		{
			if (p_ptr->invuln || !use_color) a = TERM_WHITE;
			else if (p_ptr->wraith_form) a = TERM_L_DARK;
		}

#ifdef USE_TRANSPARENCY
		/* Hack -- Queue it */
		Term_queue_char(x-panel_col_prt, y-panel_row_prt, a, c, ta, tc);
#else /* USE_TRANSPARENCY */
		/* Hack -- Queue it */
		Term_queue_char(x-panel_col_prt, y-panel_row_prt, a, c);
#endif /* USE_TRANSPARENCY */
	}
}


/*
 * Prints the map of the dungeon
 *
 * Note that, for efficiency, we contain an "optimized" version
 * of both "lite_spot()" and "print_rel()", and that we use the
 * "lite_spot()" function to display the player grid, if needed.
 */
void prt_map(void)
{
	int     x, y;
	int     v, n;
	bool    fake_monochrome = (!use_graphics || streq(ANGBAND_SYS, "ibm"));

	/* map bounds */
	s16b	xmin, xmax, ymin, ymax;
	
	byte *pa;
	char *pc;
	
#ifdef USE_TRANSPARENCY
	byte *pta;
	char *ptc;
	
#endif /* USE_TRANSPARENCY */	
	
	/* Access the cursor state */
	(void)Term_get_cursor(&v);

	/* Hide the cursor */
	(void)Term_set_cursor(0);
	
	
	/* Get bounds */
	if (dun_level)
	{
		/* Dungeon */
		xmin = (0 < panel_col_min) ? panel_col_min : 0;
		xmax = (cur_wid - 1 > panel_col_max) ? panel_col_max : cur_wid - 1;
		ymin = (0 < panel_row_min) ? panel_row_min : 0;
		ymax = (cur_hgt - 1 > panel_row_max) ? panel_row_max : cur_hgt - 1;
	}
	else
	{
		/* Wilderness */
		xmin = (wild_grid.x_min < panel_col_min) ? panel_col_min : wild_grid.x_min;
		xmax = (wild_grid.x_max - 1> panel_col_max) ? panel_col_max : wild_grid.x_max - 1;
		ymin = (wild_grid.y_min < panel_row_min) ? panel_row_min : wild_grid.y_min;
		ymax = (wild_grid.y_max - 1 > panel_row_max) ? panel_row_max : wild_grid.y_max - 1;
	}
	
	
	
#ifdef USE_TRANSPARENCY	
	/* Bottom section of screen */
	for (y = 1; y < ymin - panel_row_prt; y++)
	{
		for (x = 13; x < map_wid + 13; x++)
		{
			/* Efficiency -- Redraw that grid of the map */
			Term_queue_char(x, y , TERM_WHITE, ' ', TERM_WHITE, ' ');
		}
	}
	
	/* Top section of screen */
	for (y = ymax - panel_row_prt; y < map_hgt; y++)
	{
		for (x = 13; x < map_wid + 13; x++)
		{
			
			/* Efficiency -- Redraw that grid of the map */
			Term_queue_char(x, y, TERM_WHITE, ' ', TERM_WHITE, ' ');
		}	
	}
	
	/* Left section of screen */
	for (x = 13; x < xmin - panel_col_prt; x++)
	{
		for (y = ymin - panel_row_prt; y < ymax - panel_row_prt; y++)
		{
			/* Efficiency -- Redraw that grid of the map */
			Term_queue_char(x, y, TERM_WHITE, ' ', TERM_WHITE, ' ');
		}
	}

	/* Right section of screen */
	for (x = xmax - panel_col_prt; x < map_wid + 13; x++)
	{
		for (y = ymin - panel_row_prt; y < ymax - panel_row_prt; y++)
		{
			/* Efficiency -- Redraw that grid of the map */
			Term_queue_char(x, y, TERM_WHITE, ' ', TERM_WHITE, ' ');
		}
	}
	
#else /* USE_TRANSPARENCY */
	/* Bottom section of screen */
	for (y = 1; y < ymin - panel_row_prt; y++)
	{
		for (x = 13; x < map_wid + 13; x++)
		{
			/* Efficiency -- Redraw that grid of the map */
			Term_queue_char(x, y , TERM_WHITE, ' ');
		}
	}
	
	/* Top section of screen */
	for (y = ymax - panel_row_prt; y < map_hgt; y++)
	{
		for (x = 13; x < map_wid + 13; x++)
		{
			
			/* Efficiency -- Redraw that grid of the map */
			Term_queue_char(x, y, TERM_WHITE, ' ');
		}	
	}
	
	/* Left section of screen */
	for (x = 13; x < xmin - panel_col_prt; x++)
	{
		for (y = ymin - panel_row_prt; y < ymax - panel_row_prt; y++)
		{
			/* Efficiency -- Redraw that grid of the map */
			Term_queue_char(x, y, TERM_WHITE, ' ');
		}
	}

	/* Right section of screen */
	for (x = xmax - panel_col_prt; x < map_wid + 13; x++)
	{
		for (y = ymin - panel_row_prt; y < ymax - panel_row_prt; y++)
		{
			/* Efficiency -- Redraw that grid of the map */
			Term_queue_char(x, y, TERM_WHITE, ' ');
		}
	}
#endif /* USE_TRANSPARENCY */

	/* Pointers to current position in the string */
	pa = mp_a;
	pc = mp_c;
		
#ifdef USE_TRANSPARENCY
	pta = mp_ta;
	ptc = mp_tc;
#endif /* USE_TRANSPARENCY */	
	
	
	/* Dump the map */
	for (y = ymin; y <= ymax; y++)
	{		
		/* No characters yet */
		n = 0;
		
		/* Scan the columns of row "y" */
		for (x = xmin; x <= xmax; x++)
		{
			byte a;
			char c;

#ifdef USE_TRANSPARENCY
			byte ta;
			char tc;

			/* Determine what is there */
			map_info(y, x, &a, &c, &ta, &tc);

			/* Hack -- fake monochrome */
			if (fake_monochrome)
			{
				if (p_ptr->invuln || !use_color) a = TERM_WHITE;
				else if (p_ptr->wraith_form) a = TERM_L_DARK;
			}

			/* Queue visible. */
			*pa++ = a;
			*pc++ = c;
			*pta++ = ta;
			*ptc++ = tc;
						
#else /* USE_TRANSPARENCY */

			/* Determine what is there */
			map_info(y, x, &a, &c);

			/* Hack -- fake monochrome */
			if (fake_monochrome)
			{
				if (p_ptr->invuln || !use_color) a = TERM_WHITE;
				else if (p_ptr->wraith_form) a = TERM_L_DARK;
			}

			/* Queue visible. */
			*pa++ = a;
			*pc++ = c;

#endif /* USE_TRANSPARENCY */
		}
		
		
#ifdef USE_TRANSPARENCY		

		/* Point to start of line */
		pa = mp_a;
		pc = mp_c;
		pta = mp_ta;
		ptc = mp_tc;
			
		/* Efficiency -- Redraw that row of the map */
		Term_queue_line(xmin - panel_col_prt, y - panel_row_prt, xmax - xmin + 1
			, pa, pc, pta, ptc);
		
#else /* USE_TRANSPARENCY */

		/* Point to start of line */
		pa = mp_a;
		pc = mp_c;
			
		/* Efficiency -- Redraw that row of the map */
		Term_queue_line(xmin - panel_col_prt, y - panel_row_prt, xmax - xmin + 1
			, pa, pc);
		
#endif /* USE_TRANSPARENCY */
	}

	/* Restore the cursor */
	(void)Term_set_cursor(v);
}





/*
 * Hack -- priority array (see below)
 *
 * Note that all "walls" always look like "secret doors" (see "map_info()").
 */
static byte priority_table[][2] =
{
	/* Dark */
	{ FEAT_NONE, 2 },

	/* Floors */
	{ FEAT_FLOOR, 5 },

	/* Walls */
	{ FEAT_SECRET, 10 },

	/* Quartz */
	{ FEAT_QUARTZ, 11 },

	/* Magma */
	{ FEAT_MAGMA, 12 },

	/* Rubble */
	{ FEAT_RUBBLE, 13 },

	/* Open doors */
	{ FEAT_OPEN, 15 },
	{ FEAT_BROKEN, 15 },

	/* Closed doors */
	{ FEAT_DOOR_HEAD + 0x00, 17 },

	/* Hidden gold */
	{ FEAT_QUARTZ_K, 19 },
	{ FEAT_MAGMA_K, 19 },

	/* water, lava, & trees */
	{ FEAT_DEEP_WATER, 20 },
	{ FEAT_SHAL_WATER, 20 },
	{ FEAT_DEEP_LAVA, 20 },
	{ FEAT_SHAL_LAVA, 20 },
	{ FEAT_DIRT, 6 },
	{ FEAT_GRASS, 6 },
	{ FEAT_DARK_PIT, 20 },
	{ FEAT_TREES, 6 },
	{ FEAT_MOUNTAIN, 20 },

	/* Stairs */
	{ FEAT_LESS, 25 },
	{ FEAT_MORE, 25 },

	/* End */
	{ 0, 0 }
};


/*
 * Hack -- a priority function (see below)
 */
static byte priority(byte a, char c)
{
	int i, p0, p1;

	feature_type *f_ptr;

	/* Scan the table */
	for (i = 0; TRUE; i++)
	{
		/* Priority level */
		p1 = priority_table[i][1];

		/* End of table */
		if (!p1) break;

		/* Feature index */
		p0 = priority_table[i][0];

		/* Access the feature */
		f_ptr = &f_info[p0];

		/* Check character and attribute, accept matches */
		if ((f_ptr->x_char == c) && (f_ptr->x_attr == a)) return (p1);
	}

	/* Default */
	return (20);
}

/*
 * Tunnels are important.  (Whist bare floor is not.)
 */
static int priority_tunnel(int y, int x)
{
	int i, count = 0;

	/* Count number of floors around square */
	for (i = 1; i < 10; i++)
	{
		if (!in_bounds2(y + ddy[i], x + ddx[i])) continue;

		if (cave_floor_grid(&cave[y + ddy[i]][x + ddx[i]]))
			count++;
	}

	/* Three or less floor squares - Important */
	if (count < 4) return (19);

	/* Not important. */
	return (0);
}

/*
 * Display a "small-scale" map of the dungeon in the active Term
 *
 * Note that the "map_info()" function must return fully colorized
 * data or this function will not work correctly.
 *
 * Note that this function must "disable" the special lighting
 * effects so that the "priority" function will work.
 *
 * Note the use of a specialized "priority" function to allow this
 * function to work with any graphic attr/char mappings, and the
 * attempts to optimize this function where possible.
 *
 * cx and cy are offsets from the position of the player.  This
 * allows the map to be shifted around - but only works in the
 * wilderness.  cx and cy return the position of the player on the
 * possibly shifted map.
 */
void display_map(int *cy, int *cx)
{
	int i, j, x, y;

	byte ta;
	char tc;

	byte tp;

	u16b w_type, town;

	byte **ma;
	char **mc;

	byte **mp;

	bool old_view_special_lite;
	bool old_view_granite_lite;

	bool fake_monochrome = (!use_graphics || streq(ANGBAND_SYS, "ibm"));

	int yrat = cur_hgt / map_hgt;
	int xrat = cur_wid / map_wid;

	/* Take care of rounding */
	if (cur_hgt % map_hgt) yrat++;
	if (cur_wid % map_wid) xrat++;

	/* Save lighting effects */
	old_view_special_lite = view_special_lite;
	old_view_granite_lite = view_granite_lite;

	/* Disable lighting effects */
	view_special_lite = FALSE;
	view_granite_lite = FALSE;

	/* Allocate the maps */
	C_MAKE(ma, (map_hgt + 2), byte_ptr);
	C_MAKE(mc, (map_hgt + 2), char_ptr);
	C_MAKE(mp, (map_hgt + 2), byte_ptr);

	/* Allocate and wipe each line map */
	for (i = 0; i < (map_hgt + 2); i++)
	{
		/* Allocate one row each array */
		C_MAKE(ma[i], (map_wid + 2), byte);
		C_MAKE(mc[i], (map_wid + 2), char);
		C_MAKE(mp[i], (map_wid + 2), byte);
	}

	/* Clear the chars and attributes */
	for (y = 0; y < map_hgt + 2; ++y)
	{
		for (x = 0; x < map_wid + 2; ++x)
		{
			/* Nothing here */
			ma[y][x] = TERM_WHITE;
			mc[y][x] = ' ';

			/* No priority */
			mp[y][x] = 0;
		}
	}

	if (!dun_level)
	{
		/* Plot wilderness */

		/* work out coords of player in wilderness */
		x = px / 16 + *cx;
		y = py / 16 + *cy;

		/* recenter */
		x = x - map_wid / 2;
		if (x < 0) x = 0;

		y = y - map_hgt / 2;
		if (y < 0) y = 0;

		/* Player location in wilderness */
		(*cy) += py / 16 - y + 1 + ROW_MAP;
		(*cx) += px / 16 - x + 1 + COL_MAP;

		/* Fill in the map */
		for (i = 0; i < map_wid; ++i)
		{
			for (j = 0; j < map_hgt; ++j)
			{
				/* Only draw blocks inside map */
				if (((x + i + 1) >= max_wild) || ((y + j + 1) >= max_wild)) continue;

				/* Only draw blocks that have been seen */
				if (!(wild[j + y][i + x].done.info & WILD_INFO_SEEN)) continue;

				w_type = wild[j + y][i + x].done.wild;

				/* Get attr / char pair for wilderness block type */

				/* Add in effects of sea / roads */
				if (w_type >= WILD_SEA)
				{
					ma[j + 1][i + 1] = TERM_BLUE;
					mc[j + 1][i + 1] = '~';
				}
				else if (wild[j + y][i + x].done.info &
					 (WILD_INFO_ROAD | WILD_INFO_TRACK))
				{
					ma[j + 1][i + 1] = TERM_UMBER;
					mc[j + 1][i + 1] = '+';
				}
				else
				{
					ma[j + 1][i + 1] = wild_gen_data[w_type].w_attr;
					mc[j + 1][i + 1] = wild_gen_data[w_type].w_char;
				}

				/* Hack - draw towns/specials */
				/* Eventually will get attr,char from town data structure. */

				town = wild[j + y][i + x].done.town;

				/* If there is a town... */
				if (town)
				{
					/* Hack make a char /attr */
					ma[j + 1][i + 1] = TERM_WHITE;
					mc[j + 1][i + 1] = '0' + town % 10;

				}

				/* Finally show position of player */
				if ((i + x == px / 16) && (j + y == py / 16))
				{
					ma[j + 1][i + 1] = TERM_WHITE;
					mc[j + 1][i + 1] = '@';
				}
			}
		}
	}
	else
	{
		/* Player location in dungeon */
		(*cy) = py / yrat + 1 + ROW_MAP;
		(*cx) = px / xrat + 1 + COL_MAP;

		/* Fill in the map of dungeon */
		for (i = 0; i < cur_wid; ++i)
		{
			for (j = 0; j < cur_hgt; ++j)
			{
				/* Location */
				x = i / xrat + 1;
				y = j / yrat + 1;

				/* Priority zero */
				tp = 0;

				if (cave_floor_grid(&cave[j][i]))
				{
					/* Corridors are important */
					tp = priority_tunnel(j, i);
				}

				/* Extract the current attr/char at that map location */
#ifdef USE_TRANSPARENCY
				map_info(j, i, &ta, &tc, &ta, &tc);
#else /* USE_TRANSPARENCY */
				map_info(j, i, &ta, &tc);
#endif /* USE_TRANSPARENCY */

				/* Extract the priority of that attr/char */
				tp += priority(ta, tc);

				/* Save "best" */
				if (mp[y][x] < tp)
				{
					/* Save the char */
					mc[y][x] = tc;

					/* Save the attr */
					ma[y][x] = ta;

					/* Save priority */
					mp[y][x] = tp;
				}
			}
		}
	}

	/* Corners */
	i = map_wid + 1;
	j = map_hgt + 1;

	/* Draw the corners */
	mc[0][0] = mc[0][i] = mc[j][0] = mc[j][i] = '+';

	/* Draw the horizontal edges */
	for (i = 1; i <= map_wid; i++) mc[0][i] = mc[j][i] = '-';

	/* Draw the vertical edges */
	for (j = 1; j <= map_hgt; j++) mc[j][0] = mc[j][i] = '|';


	/* Display each map line in order */
	for (j = 0; j < map_hgt + 2; ++j)
	{
		/* Start a new line */
		Term_gotoxy(COL_MAP, j);

		/* Display the line */
		for (i = 0; i < map_wid + 2; ++i)
		{
			ta = ma[j][i];
			tc = mc[j][i];

			/* Hack -- fake monochrome */
			if (fake_monochrome)
			{
				if (p_ptr->invuln || !use_color) ta = TERM_WHITE;
				else if (p_ptr->wraith_form) ta = TERM_L_DARK;
			}

			/* Add the character */
			Term_addch(ta, tc);
		}
	}


	/* Restore lighting effects */
	view_special_lite = old_view_special_lite;
	view_granite_lite = old_view_granite_lite;



	/* Free each line map */
	for (i = 0; i < (map_hgt + 2); i++)
	{
		/* Free one row each array */
		C_FREE(ma[i], (map_wid + 2), byte);
		C_FREE(mc[i], (map_wid + 2), char);
		C_FREE(mp[i], (map_wid + 2), byte);
	}

	/* Free the maps */
	C_FREE(ma, (map_hgt + 2), byte_ptr);
	C_FREE(mc, (map_hgt + 2), char_ptr);
	C_FREE(mp, (map_hgt + 2), byte_ptr);
}


/*
 * Display a "small-scale" map of the dungeon for the player
 *
 * Currently, the "player" is displayed on the map.  XXX XXX XXX
 */
void do_cmd_view_map(void)
{
	int cy, cx;

	/* No overhead map in vanilla town mode. */
	if (!dun_level && vanilla_town) return;

	/* Save the screen */
	screen_save();

	/* Note */
	prt("Please wait...", 0, 0);

	/* Flush */
	Term_fresh();

	/* Clear the screen */
	Term_clear();

	if (dun_level)
	{
		/* In the dungeon - All we have to do is display the map */

		/* No offset from player */
		cx = 0;
		cy = 0;

		/* Display the map */
		display_map(&cy, &cx);

		/* Wait for it */
		put_str("Hit any key to continue", 23, 35);

		/* Hilite the player */
		move_cursor(cy, cx);

		/* Get any key */
		inkey();
	}
	else
	{
		/* Offset from player */
		int x, y;

		/* Direction */
		int d;

		/* No offset yet */
		x = 0;
		y = 0;

		/* In the wilderness - Display the map + move it around */

		while (TRUE)
		{
			/* Reset offset of map */
			cx = x;
			cy = y;

			display_map(&cy, &cx);

			/* Wait for it */
			put_str("Move around, or hit any other key to continue.", map_hgt + 1, (14 + map_wid) / 2 + 15);

			/* Hilite the player */
			move_cursor(cy, cx);

			/* Get a response */
			d = get_keymap_dir(inkey());

			/* Done if not a direction */
			if (!d) break;

			x += ddx[d];
			y += ddy[d];

			/* Bounds checking */
			if (x + px / 16 < 0) x = -px / 16;
			if (y + py / 16 < 0) y = -py / 16;
			if (x + px / 16 > max_wild - 2) x = max_wild - px / 16 - 2;
			if (y + py / 16 > max_wild - 2) y = max_wild - py / 16 - 2;
		}
	}

	/* Restore the screen */
	screen_load();
}





/*
 * Some comments on the cave grid flags.  -BEN-
 *
 *
 * One of the major bottlenecks in previous versions of Angband was in
 * the calculation of "line of sight" from the player to various grids,
 * such as monsters.  This was such a nasty bottleneck that a lot of
 * silly things were done to reduce the dependancy on "line of sight",
 * for example, you could not "see" any grids in a lit room until you
 * actually entered the room, and there were all kinds of bizarre grid
 * flags to enable this behavior.  This is also why the "call light"
 * spells always lit an entire room.
 *
 * The code below provides functions to calculate the "field of view"
 * for the player, which, once calculated, provides extremely fast
 * calculation of "line of sight from the player", and to calculate
 * the "field of torch lite", which, again, once calculated, provides
 * extremely fast calculation of "which grids are lit by the player's
 * lite source".  In addition to marking grids as "GRID_VIEW" and/or
 * "GRID_LITE", as appropriate, these functions maintain an array for
 * each of these two flags, each array containing the locations of all
 * of the grids marked with the appropriate flag, which can be used to
 * very quickly scan through all of the grids in a given set.
 *
 * To allow more "semantically valid" field of view semantics, whenever
 * the field of view (or the set of torch lit grids) changes, all of the
 * grids in the field of view (or the set of torch lit grids) are "drawn"
 * so that changes in the world will become apparent as soon as possible.
 * This has been optimized so that only grids which actually "change" are
 * redrawn, using the "temp" array and the "GRID_TEMP" flag to keep track
 * of the grids which are entering or leaving the relevent set of grids.
 *
 * These new methods are so efficient that the old nasty code was removed.
 *
 * Note that there is no reason to "update" the "viewable space" unless
 * the player "moves", or walls/doors are created/destroyed, and there
 * is no reason to "update" the "torch lit grids" unless the field of
 * view changes, or the "light radius" changes.  This means that when
 * the player is resting, or digging, or doing anything that does not
 * involve movement or changing the state of the dungeon, there is no
 * need to update the "view" or the "lite" regions, which is nice.
 *
 * Note that the calls to the nasty "los()" function have been reduced
 * to a bare minimum by the use of the new "field of view" calculations.
 *
 * I wouldn't be surprised if slight modifications to the "update_view()"
 * function would allow us to determine "reverse line-of-sight" as well
 * as "normal line-of-sight", which would allow monsters to use a more
 * "correct" calculation to determine if they can "see" the player.  For
 * now, monsters simply "cheat" somewhat and assume that if the player
 * has "line of sight" to the monster, then the monster can "pretend"
 * that it has "line of sight" to the player.
 *
 *
 * This set of grids is the complete set of all grids which are lit by
 * the players light source, which allows the "player_can_see_bold()"
 * function to work very quickly.
 *
 * Note that every "CAVE_LITE" grid is also a "CAVE_VIEW" grid, and in
 * fact, the player (unless blind) can always "see" all grids which are
 * marked as "CAVE_LITE", unless they are "off screen".
 *
 *
 * The "update_view()" function maintains the "CAVE_VIEW" flag for each
 * grid and maintains an array of all "CAVE_VIEW" grids.
 *
 *
 * The current "update_view()" algorithm uses the "CAVE_XTRA" flag as a
 * temporary internal flag to mark those grids which are not only in view,
 * but which are also "easily" in line of sight of the player.  This flag
 * is always cleared when we are done.
 *
 *
 * The current "update_view()" algorithm uses the
 * "CAVE_TEMP" flag, and the array of grids which are marked as "CAVE_TEMP",
 * to keep track of which grids were previously marked as "CAVE_VIEW", which
 * allows us to optimize the "screen updates".
 *
 * The "CAVE_TEMP" flag, and the array of "CAVE_TEMP" grids, is also used
 * for various other purposes, such as spreading lite or darkness during
 * "lite_room()" / "unlite_room()", and for calculating monster flow.
 *
 *
 * Any grid can be marked as "CAVE_GLOW" which means that the grid itself is
 * in some way permanently lit.  However, for the player to "see" anything
 * in the grid, as determined by "player_can_see()", the player must not be
 * blind, the grid must be marked as "CAVE_VIEW", and, in addition, "wall"
 * grids, even if marked as "perma lit", are only illuminated if they touch
 * a grid which is not a wall and is marked both "CAVE_GLOW" and "CAVE_VIEW".
 *
 *
 * To simplify various things, a grid may be marked as "CAVE_MARK", meaning
 * that even if the player cannot "see" the grid, he "knows" the terrain in
 * that grid.  This is used to "remember" walls/doors/stairs/floors when they
 * are "seen" or "detected", and also to "memorize" floors, after "wiz_lite()",
 * or when one of the "memorize floor grids" options induces memorization.
 *
 * Objects are "memorized" in a different way, using a special "marked" flag
 * on the object itself, which is set when an object is observed or detected.
 *
 *
 * A grid may be marked as "CAVE_ROOM" which means that it is part of a "room".
 * This is used only in dungeon generation.  Perhaps this flag can be used in
 * other code if required.
 *
 *
 * A grid may be marked as "CAVE_ICKY" which means it is part of a "vault",
 * and should be unavailable for "teleportation" destinations.
 *
 *
 * The "view_perma_grids" allows the player to "memorize" every perma-lit grid
 * which is observed, and the "view_torch_grids" allows the player to memorize
 * every torch-lit grid.  The player will always memorize important walls,
 * doors, stairs, and other terrain features, as well as any "detected" grids.
 *
 * Note that the new "update_view()" method allows, among other things, a room
 * to be "partially" seen as the player approaches it, with a growing cone of
 * floor appearing as the player gets closer to the door.  Also, by not turning
 * on the "memorize perma-lit grids" option, the player will only "see" those
 * floor grids which are actually in line of sight.
 *
 * And my favorite "plus" is that you can now use a special option to draw the
 * "floors" in the "viewable region" brightly (actually, to draw the *other*
 * grids dimly), providing a "pretty" effect as the player runs around, and
 * to efficiently display the "torch lite" in a special color.
 *
 *
 * Some comments on the "update_view()" algorithm...
 *
 * The algorithm is very fast, since it spreads "obvious" grids very quickly,
 * and only has to call "los()" on the borderline cases.  The major axes/diags
 * even terminate early when they hit walls.  I need to find a quick way
 * to "terminate" the other scans.
 *
 * Note that in the worst case (a big empty area with say 5% scattered walls),
 * each of the 1500 or so nearby grids is checked once, most of them getting
 * an "instant" rating, and only a small portion requiring a call to "los()".
 *
 * The only time that the algorithm appears to be "noticeably" too slow is
 * when running, and this is usually only important in town, since the town
 * provides about the worst scenario possible, with large open regions and
 * a few scattered obstructions.  There is a special "efficiency" option to
 * allow the player to reduce his field of view in town, if needed.
 *
 * In the "best" case (say, a normal stretch of corridor), the algorithm
 * makes one check for each viewable grid, and makes no calls to "los()".
 * So running in corridors is very fast, and if a lot of monsters are
 * nearby, it is much faster than the old methods.
 *
 * Note that resting, most normal commands, and several forms of running,
 * plus all commands executed near large groups of monsters, are strictly
 * more efficient with "update_view()" that with the old "compute los() on
 * demand" method, primarily because once the "field of view" has been
 * calculated, it does not have to be recalculated until the player moves
 * (or a wall or door is created or destroyed).
 *
 * Note that we no longer have to do as many "los()" checks, since once the
 * "view" region has been built, very few things cause it to be "changed"
 * (player movement, and the opening/closing of doors, changes in wall status).
 * Note that door/wall changes are only relevant when the door/wall itself is
 * in the "view" region.
 *
 * The algorithm seems to only call "los()" from zero to ten times, usually
 * only when coming down a corridor into a room, or standing in a room, just
 * misaligned with a corridor.  So if, say, there are five "nearby" monsters,
 * we will be reducing the calls to "los()".
 *
 * I am thinking in terms of an algorithm that "walks" from the central point
 * out to the maximal "distance", at each point, determining the "view" code
 * (above).  For each grid not on a major axis or diagonal, the "view" code
 * depends on the "cave_floor_grid()" and "view" of exactly two other grids
 * (the one along the nearest diagonal, and the one next to that one, see
 * "update_view_aux()"...).
 *
 * We "memorize" the viewable space array, so that at the cost of under 3000
 * bytes, we reduce the time taken by "forget_view()" to one assignment for
 * each grid actually in the "viewable space".  And for another 3000 bytes,
 * we prevent "erase + redraw" ineffiencies via the "seen" set.  These bytes
 * are also used by other routines, thus reducing the cost to almost nothing.
 *
 * A similar thing is done for "forget_lite()" in which case the savings are
 * much less, but save us from doing bizarre maintenance checking.
 *
 * In the worst "normal" case (in the middle of the town), the reachable space
 * actually reaches to more than half of the largest possible "circle" of view,
 * or about 800 grids, and in the worse case (in the middle of a dungeon level
 * where all the walls have been removed), the reachable space actually reaches
 * the theoretical maximum size of just under 1500 grids.
 *
 * Each grid G examines the "state" of two (?) other (adjacent) grids, G1 & G2.
 * If G1 is lite, G is lite.  Else if G2 is lite, G is half.  Else if G1 and G2
 * are both half, G is half.  Else G is dark.  It only takes 2 (or 4) bits to
 * "name" a grid, so (for MAX_RAD of 20) we could use 1600 bytes, and scan the
 * entire possible space (including initialization) in one step per grid.  If
 * we do the "clearing" as a separate step (and use an array of "view" grids),
 * then the clearing will take as many steps as grids that were viewed, and the
 * algorithm will be able to "stop" scanning at various points.
 * Oh, and outside of the "torch radius", only "lite" grids need to be scanned.
 */




/*
 * Clear the viewable space
 */
void forget_view(void)
{
	int i;

	cave_type *c_ptr;

	/* None to forget */
	if (!view_n) return;

	/* Clear them all */
	for (i = 0; i < view_n; i++)
	{
		int y = view_y[i];
		int x = view_x[i];

		/* Access the grid */
		c_ptr = area(y,x);

		/* Forget that the grid is viewable or lit */
		c_ptr->info &= ~(CAVE_VIEW | CAVE_LITE);
		
		/* Only lite the spot if is on the panel (can change due to resizing */
		if (!panel_contains(y, x)) continue;

		/* Update the screen */
		lite_spot(y, x);
	}

	/* None left */
	view_n = 0;
}

/*
 * New update lite/ view routines.
 */


/*
 * Maximum number of grids in a single octant
 */
#define VINFO_MAX_GRIDS 161


/*
 * Maximum number of slopes in a single octant
 */
#define VINFO_MAX_SLOPES 126


/*
 * Mask of bits used in a single octant
 */
#define VINFO_BITS_3 0x3FFFFFFF
#define VINFO_BITS_2 0xFFFFFFFF
#define VINFO_BITS_1 0xFFFFFFFF
#define VINFO_BITS_0 0xFFFFFFFF


/*
 * Forward declare
 */
typedef struct vinfo_type vinfo_type;


/*
 * The 'vinfo_type' structure
 */
struct vinfo_type
{
	s16b grid_x[8];
	s16b grid_y[8];

	u32b bits_3;
	u32b bits_2;
	u32b bits_1;
	u32b bits_0;

	vinfo_type *next_0;
	vinfo_type *next_1;

	byte y;
	byte x;
	byte d;
	byte r;
};



/*
 * The array of "vinfo" objects, initialized by "vinfo_init()"
 */
static vinfo_type vinfo[VINFO_MAX_GRIDS];




/*
 * Slope scale factor
 */
#define SCALE 100000L


/*
 * The actual slopes (for reference)
 */

/* Bit :     Slope   Grids */
/* --- :     -----   ----- */
/*   0 :      2439      21 */
/*   1 :      2564      21 */
/*   2 :      2702      21 */
/*   3 :      2857      21 */
/*   4 :      3030      21 */
/*   5 :      3225      21 */
/*   6 :      3448      21 */
/*   7 :      3703      21 */
/*   8 :      4000      21 */
/*   9 :      4347      21 */
/*  10 :      4761      21 */
/*  11 :      5263      21 */
/*  12 :      5882      21 */
/*  13 :      6666      21 */
/*  14 :      7317      22 */
/*  15 :      7692      20 */
/*  16 :      8108      21 */
/*  17 :      8571      21 */
/*  18 :      9090      20 */
/*  19 :      9677      21 */
/*  20 :     10344      21 */
/*  21 :     11111      20 */
/*  22 :     12000      21 */
/*  23 :     12820      22 */
/*  24 :     13043      22 */
/*  25 :     13513      22 */
/*  26 :     14285      20 */
/*  27 :     15151      22 */
/*  28 :     15789      22 */
/*  29 :     16129      22 */
/*  30 :     17241      22 */
/*  31 :     17647      22 */
/*  32 :     17948      23 */
/*  33 :     18518      22 */
/*  34 :     18918      22 */
/*  35 :     20000      19 */
/*  36 :     21212      22 */
/*  37 :     21739      22 */
/*  38 :     22580      22 */
/*  39 :     23076      22 */
/*  40 :     23809      22 */
/*  41 :     24137      22 */
/*  42 :     24324      23 */
/*  43 :     25714      23 */
/*  44 :     25925      23 */
/*  45 :     26315      23 */
/*  46 :     27272      22 */
/*  47 :     28000      23 */
/*  48 :     29032      23 */
/*  49 :     29411      23 */
/*  50 :     29729      24 */
/*  51 :     30434      23 */
/*  52 :     31034      23 */
/*  53 :     31428      23 */
/*  54 :     33333      18 */
/*  55 :     35483      23 */
/*  56 :     36000      23 */
/*  57 :     36842      23 */
/*  58 :     37142      24 */
/*  59 :     37931      24 */
/*  60 :     38461      24 */
/*  61 :     39130      24 */
/*  62 :     39393      24 */
/*  63 :     40740      24 */
/*  64 :     41176      24 */
/*  65 :     41935      24 */
/*  66 :     42857      23 */
/*  67 :     44000      24 */
/*  68 :     44827      24 */
/*  69 :     45454      23 */
/*  70 :     46666      24 */
/*  71 :     47368      24 */
/*  72 :     47826      24 */
/*  73 :     48148      24 */
/*  74 :     48387      24 */
/*  75 :     51515      25 */
/*  76 :     51724      25 */
/*  77 :     52000      25 */
/*  78 :     52380      25 */
/*  79 :     52941      25 */
/*  80 :     53846      25 */
/*  81 :     54838      25 */
/*  82 :     55555      24 */
/*  83 :     56521      25 */
/*  84 :     57575      26 */
/*  85 :     57894      25 */
/*  86 :     58620      25 */
/*  87 :     60000      23 */
/*  88 :     61290      25 */
/*  89 :     61904      25 */
/*  90 :     62962      25 */
/*  91 :     63636      25 */
/*  92 :     64705      25 */
/*  93 :     65217      25 */
/*  94 :     65517      25 */
/*  95 :     67741      26 */
/*  96 :     68000      26 */
/*  97 :     68421      26 */
/*  98 :     69230      26 */
/*  99 :     70370      26 */
/* 100 :     71428      25 */
/* 101 :     72413      26 */
/* 102 :     73333      26 */
/* 103 :     73913      26 */
/* 104 :     74193      27 */
/* 105 :     76000      26 */
/* 106 :     76470      26 */
/* 107 :     77777      25 */
/* 108 :     78947      26 */
/* 109 :     79310      26 */
/* 110 :     80952      26 */
/* 111 :     81818      26 */
/* 112 :     82608      26 */
/* 113 :     84000      26 */
/* 114 :     84615      26 */
/* 115 :     85185      26 */
/* 116 :     86206      27 */
/* 117 :     86666      27 */
/* 118 :     88235      27 */
/* 119 :     89473      27 */
/* 120 :     90476      27 */
/* 121 :     91304      27 */
/* 122 :     92000      27 */
/* 123 :     92592      27 */
/* 124 :     93103      28 */
/* 125 :    100000      13 */



/*
 * Forward declare
 */
typedef struct vinfo_hack vinfo_hack;


/*
 * Temporary data used by "vinfo_init()"
 *
 *	- Number of grids
 *
 *	- Number of slopes
 *
 *	- Slope values
 *
 *	- Slope range per grid
 */
struct vinfo_hack {

	int num_slopes;

	long slopes[VINFO_MAX_SLOPES];

	long slopes_min[MAX_SIGHT+1][MAX_SIGHT+1];
	long slopes_max[MAX_SIGHT+1][MAX_SIGHT+1];
};



/*
 * Sorting hook -- comp function -- array of long's (see below)
 *
 * We use "u" to point to an array of long integers.
 */
static bool ang_sort_comp_hook_longs(vptr u, vptr v, int a, int b)
{
	long *x = (long*)(u);

	return (x[a] <= x[b]);
}


/*
 * Sorting hook -- comp function -- array of long's (see below)
 *
 * We use "u" to point to an array of long integers.
 */
static void ang_sort_swap_hook_longs(vptr u, vptr v, int a, int b)
{
	long *x = (long*)(u);

	long temp;

	/* Swap */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;
}



/*
 * Save a slope
 */
static void vinfo_init_aux(vinfo_hack *hack, int y, int x, long m)
{
	int i;

	/* Handle "legal" slopes */
	if ((m > 0) && (m <= SCALE))
	{
		/* Look for that slope */
		for (i = 0; i < hack->num_slopes; i++)
		{
			if (hack->slopes[i] == m) break;
		}

		/* New slope */
		if (i == hack->num_slopes)
		{
			/* Paranoia */
			if (hack->num_slopes >= VINFO_MAX_SLOPES)
			{
				quit_fmt("Too many slopes (%d)!",
			         	VINFO_MAX_SLOPES);
			}

			/* Save the slope, and advance */
			hack->slopes[hack->num_slopes++] = m;
		}
	}

	/* Track slope range */
	if (hack->slopes_min[y][x] > m) hack->slopes_min[y][x] = m;
	if (hack->slopes_max[y][x] < m) hack->slopes_max[y][x] = m;
}


/*
 * Initialize the "vinfo" array
 *
 * Full Octagon (radius 20), Grids=1149
 *
 * Quadrant (south east), Grids=308, Slopes=251
 *
 * Octant (east then south), Grids=161, Slopes=126
 *
 * This function assumes that VINFO_MAX_GRIDS and VINFO_MAX_SLOPES
 * have the correct values, which can be derived by setting them to
 * a number which is too high, running this function, and using the
 * error messages to obtain the correct values.
 */
errr vinfo_init(void)
{
	int i;
	int y, x;

	long m;

	vinfo_hack *hack;

	int num_grids = 0;

	int queue_head = 0;
	int queue_tail = 0;
	vinfo_type *queue[VINFO_MAX_GRIDS*2];


	/* Make hack */
	MAKE(hack, vinfo_hack);


	/* Analyze grids */
	for (y = 0; y <= MAX_SIGHT; ++y)
	{
		for (x = y; x <= MAX_SIGHT; ++x)
		{
			/* Skip grids which are out of sight range */
			if (((y > x) ? (y + x/2) : (x + y/2)) > MAX_SIGHT) continue;

			/* Default slope range */
			hack->slopes_min[y][x] = 999999999;
			hack->slopes_max[y][x] = 0;

			/* Paranoia */
			if (num_grids >= VINFO_MAX_GRIDS)
			{
				quit_fmt("Too many grids (%d >= %d)!",
				         num_grids, VINFO_MAX_GRIDS);
			}

			/* Count grids */
			num_grids++;

			/* Slope to the top right corner */
			m = SCALE * (1000L * y - 500) / (1000L * x + 500);

			/* Handle "legal" slopes */
			vinfo_init_aux(hack, y, x, m);

			/* Slope to top left corner */
			m = SCALE * (1000L * y - 500) / (1000L * x - 500);

			/* Handle "legal" slopes */
			vinfo_init_aux(hack, y, x, m);

			/* Slope to bottom right corner */
			m = SCALE * (1000L * y + 500) / (1000L * x + 500);

			/* Handle "legal" slopes */
			vinfo_init_aux(hack, y, x, m);

			/* Slope to bottom left corner */
			m = SCALE * (1000L * y + 500) / (1000L * x - 500);

			/* Handle "legal" slopes */
			vinfo_init_aux(hack, y, x, m);
		}
	}


	/* Enforce maximal efficiency */
	if (num_grids < VINFO_MAX_GRIDS)
	{
		quit_fmt("Too few grids (%d < %d)!",
		         num_grids, VINFO_MAX_GRIDS);
	}

	/* Enforce maximal efficiency */
	if (hack->num_slopes < VINFO_MAX_SLOPES)
	{
		quit_fmt("Too few slopes (%d < %d)!",
		         hack->num_slopes, VINFO_MAX_SLOPES);
	}


	/* Sort slopes numerically */
	ang_sort_comp = ang_sort_comp_hook_longs;

	/* Sort slopes numerically */
	ang_sort_swap = ang_sort_swap_hook_longs;

	/* Sort the (unique) slopes */
	ang_sort(hack->slopes, NULL, hack->num_slopes);



	/* Enqueue player grid */
	queue[queue_tail++] = &vinfo[0];

	/* Process queue */
	while (queue_head < queue_tail)
	{
		int e;

		vinfo_type *p;


		/* Index */
		e = queue_head;

		/* Dequeue next grid */
		p = queue[queue_head++];

		/* Location */
		y = vinfo[e].grid_y[0];
		x = vinfo[e].grid_x[0];


		/* Compute grid offsets */
		vinfo[e].grid_x[0] = x;
		vinfo[e].grid_x[1] = y;
		vinfo[e].grid_x[2] = -y;
		vinfo[e].grid_x[3] = -x;
		vinfo[e].grid_x[4] = -x;
		vinfo[e].grid_x[5] = -y;
		vinfo[e].grid_x[6] = y;
		vinfo[e].grid_x[7] = x;
		
		vinfo[e].grid_y[0] = y;
		vinfo[e].grid_y[1] = x;
		vinfo[e].grid_y[2] = x;
		vinfo[e].grid_y[3] = y;
		vinfo[e].grid_y[4] = -y;
		vinfo[e].grid_y[5] = -x;
		vinfo[e].grid_y[6] = -x;
		vinfo[e].grid_y[7] = -y;


		/* Analyze slopes */
		for (i = 0; i < hack->num_slopes; ++i)
		{
			m = hack->slopes[i];

			/* Memorize intersection slopes (for non-player-grids) */
			if ((e > 0) &&
			    (hack->slopes_min[y][x] < m) &&
			    (m < hack->slopes_max[y][x]))
			{
				switch (i / 32)
				{
					case 3: vinfo[e].bits_3 |= (1L << (i % 32)); break;
					case 2: vinfo[e].bits_2 |= (1L << (i % 32)); break;
					case 1: vinfo[e].bits_1 |= (1L << (i % 32)); break;
					case 0: vinfo[e].bits_0 |= (1L << (i % 32)); break;
				}
			}
		}


		/* Default */
		vinfo[e].next_0 = &vinfo[0];

		/* Grid next child */
		if (((y > x+1) ? (y + (x+1)/2) : (x+1 + y/2)) <= MAX_SIGHT)
		{
			if (!((queue[queue_tail-1]->grid_x[0] == x + 1) &&
			 		(queue[queue_tail-1]->grid_y[0] == y)))
			{
				vinfo[queue_tail].grid_x[0] = x + 1;
				vinfo[queue_tail].grid_y[0] = y;
				queue[queue_tail] = &vinfo[queue_tail];
				queue_tail++;
			}

			vinfo[e].next_0 = &vinfo[queue_tail - 1];
		}


		/* Default */
		vinfo[e].next_1 = &vinfo[0];

		/* Grid diag child */
		if (((y > x) ? (y+1 + (x+1)/2) : (x+1 + (y+1)/2)) <= MAX_SIGHT)
		{
			if (!((queue[queue_tail-1]->grid_x[0] == x + 1) &&
			 		(queue[queue_tail-1]->grid_y[0] == y + 1)))
			{
				vinfo[queue_tail].grid_x[0] = x + 1;
				vinfo[queue_tail].grid_y[0] = y + 1;
				queue[queue_tail] = &vinfo[queue_tail];
				queue_tail++;
			}

			vinfo[e].next_1 = &vinfo[queue_tail - 1];
		}


		/* Hack -- main diagonal has special children */
		if (y == x) vinfo[e].next_0 = vinfo[e].next_1;


		/* Extra values */
		vinfo[e].y = y;
		vinfo[e].x = x;
		vinfo[e].d = ((y > x) ? (y + x/2) : (x + y/2));
		vinfo[e].r = ((!y) ? x : (!x) ? y : (y == x) ? y : 0);
	}


	/* Verify maximal bits XXX XXX XXX */
	if (((vinfo[1].bits_3 | vinfo[2].bits_3) != VINFO_BITS_3) ||
	    ((vinfo[1].bits_2 | vinfo[2].bits_2) != VINFO_BITS_2) ||
	    ((vinfo[1].bits_1 | vinfo[2].bits_1) != VINFO_BITS_1) ||
	    ((vinfo[1].bits_0 | vinfo[2].bits_0) != VINFO_BITS_0))
	{
		quit("Incorrect bit masks!");
	}


	/* Kill hack */
	KILL(hack, vinfo_hack);


	/* Success */
	return (0);
}

/*
 * Calculate the complete field of view using a new algorithm
 *
 * If "view_g" and "temp_g" were global pointers to arrays of grids, as
 * opposed to actual arrays of grids, then we could be more efficient by
 * using "pointer swapping".
 *
 *
 * Normally, vision along the major axes is more likely than vision
 * along the diagonal axes, so we check the bits corresponding to
 * the lines of sight near the major axes first.
 *
 * We use the "temp_g" array (and the "CAVE_TEMP" flag) to keep track of
 * which grids were previously marked "CAVE_VIEW", since only those grids
 * whose "CAVE_VIEW" value changes during this routine must be redrawn.
 *
 * This function is now responsible for maintaining the "CAVE_LITE"
 * flags as well as the "CAVE_VIEW" flags, which is good, because
 * the only grids which normally need to be memorized and/or redrawn
 * are the ones whose "CAVE_VIEW" flag changes during this routine.
 *
 * Basically, this function divides the "octagon of view" into octants of
 * grids (where grids on the main axes and diagonal axes are "shared" by
 * two octants), and processes each octant one at a time, processing each
 * octant one grid at a time, processing only those grids which "might" be
 * viewable, and setting the "CAVE_VIEW" flag for each grid for which there
 * is an (unobstructed) line of sight from the center of the player grid to
 * any internal point in the grid (and collecting these "CAVE_VIEW" grids
 * into the "view_g" array), and setting the "CAVE_LITE" flag for the grid
 * if, in addition, the grid is "illuminated" in some way (by a torch).
 *
 * This function relies on a theorem (suggested and proven by Mat Hostetter)
 * which states that in each octant of a field of view, a given grid will
 * be "intersected" by one or more unobstructed "lines of sight" from the
 * center of the player grid if and only if it is "intersected" by at least
 * one such unobstructed "line of sight" which passes directly through some
 * corner of some grid in the octant which is not shared by any other octant.
 * The proof is based on the fact that there are at least three significant
 * lines of sight involving any non-shared grid in any octant, one which
 * intersects the grid and passes though the corner of the grid closest to
 * the player, and two which "brush" the grid, passing through the "outer"
 * corners of the grid, and that any line of sight which intersects a grid
 * without passing through the corner of a grid in the octant can be "slid"
 * slowly towards the corner of the grid closest to the player, until it
 * either reaches it or until it brushes the corner of another grid which
 * is closer to the player, and in either case, the existanc of a suitable
 * line of sight is thus demonstrated.
 *
 * It turns out that in each octant of the radius 20 "octagon of view",
 * there are 161 grids (with 128 not shared by any other octant), and there
 * are exactly 126 distinct "lines of sight" passing from the center of the
 * player grid through any corner of any non-shared grid in the octant.  To
 * determine if a grid is "viewable" by the player, therefore, you need to
 * simply show that one of these 126 lines of sight intersects the grid but
 * does not intersect any wall grid closer to the player.  So we simply use
 * a bit vector with 126 bits to represent the set of interesting lines of
 * sight which have not yet been obstructed by wall grids, and then we scan
 * all the grids in the octant, moving outwards from the player grid.  For
 * each grid, if any of the lines of sight which intersect that grid have not
 * yet been obstructed, then the grid is viewable.  Furthermore, if the grid
 * is a wall grid, then all of the lines of sight which intersect the grid
 * should be marked as obstructed for future reference.  Also, we only need
 * to check those grids for whom at least one of the "parents" was a viewable
 * non-wall grid, where the parents include the two grids touching the grid
 * but closer to the player grid (one adjacent, and one diagonal).  For the
 * bit vector, we simply use 4 32-bit integers.  All of the static values
 * which are needed by this function are stored in the large "vinfo" array
 * (above), which is machine generated by another program.  XXX XXX XXX
 *
 * Hack -- The queue must be able to hold more than VINFO_MAX_GRIDS grids
 * because the grids at the edge of the field of view use "grid zero" as
 * their children, and the queue must be able to hold several of these
 * special grids.  Because the actual number of required grids is bizarre,
 * we simply allocate twice as many as we would normally need.  XXX XXX XXX
 */
void update_view(void)
{
	cave_type *c_ptr;
	
	byte info;
	
	int x, y, i, o2;
	
	s16b this_o_idx, next_o_idx = 0;
	
	/* Light radius */
	s16b radius = p_ptr->cur_lite;
	
	
	/*** Save the old "lite" grids for later ***/	
	
	/* Save the old "view" grids for later */
	for (i = 0; i < view_n; i++)
	{
		y = view_y[i];
		x = view_x[i];

		if (!in_bounds2(y,x)) continue;

		c_ptr = area(y, x);
		info = c_ptr->info;
		
		/* Save "CAVE_VIEW" grids */
		if (info & (CAVE_VIEW))
		{
			/* Set "CAVE_TEMP" flag */
			info |= (CAVE_TEMP);

			/* Save grid for later */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}

		/* Clear "CAVE_VIEW" and "CAVE_LITE" flags */
		info &= ~(CAVE_VIEW | CAVE_LITE);
		
		/* Save the "CAVE_MARK" flag */
		if (info & CAVE_MARK)
		{
			info |= CAVE_XTRA;
		}

		/* Save cave info */
		c_ptr->info = info;
	}
	
	/* empty the viewable list */
	view_n = 0;
	
	/*** Step 1 -- player grid ***/

	/* Player grid */

	/* Get grid info */
	info = area(py, px)->info;

	/* Assume viewable */
	info |= (CAVE_VIEW);

	/* Torch-lit grid */
	if (0 < radius)
	{
		/* Mark as "CAVE_LITE" and "CAVE_VIEW" */
		info |= (CAVE_LITE | CAVE_VIEW);
	}

	/* Perma-lit grid */
	else if (info & (CAVE_GLOW))
	{
		/* Mark as "CAVE_VIEW" */
		info |= (CAVE_VIEW);
	}

	/* Save cave info */
	area(py, px)->info = info;
	
	/* Redraw player*/
	/*lite_spot(py, px);*/

	/* Save in array */
	
	view_y[view_n] = py;
	view_x[view_n] = px;
	view_n++;	

	/*** Step 2 -- octants ***/

	/* Scan each octant */
	for (o2 = 0; o2 < 8; o2 += 1)
	{
		vinfo_type *p;

		/* Last added */
		vinfo_type *last = &vinfo[0];

		/* Grid queue */
		int queue_head = 0;
		int queue_tail = 0;
		vinfo_type *queue[VINFO_MAX_GRIDS*2];

		/* Slope bit vector */
		u32b bits0 = VINFO_BITS_0;
		u32b bits1 = VINFO_BITS_1;
		u32b bits2 = VINFO_BITS_2;
		u32b bits3 = VINFO_BITS_3;

		/* Reset queue */
		queue_head = queue_tail = 0;

		/* Initial grids */
		queue[queue_tail++] = &vinfo[1];
		queue[queue_tail++] = &vinfo[2];

		/* Process queue */
		while (queue_head < queue_tail)
		{
			/* Dequeue next grid */
			p = queue[queue_head++];

			/* Check bits */
			if ((bits0 & (p->bits_0)) ||
			    (bits1 & (p->bits_1)) ||
			    (bits2 & (p->bits_2)) ||
			    (bits3 & (p->bits_3)))
			{
				/* Get location */
				x = p->grid_x[o2] + px;
				y = p->grid_y[o2] + py;
			
				/* Is it in bounds? */
				if (!in_bounds2(y, x))
				{
					/* Clear bits */
					bits0 &= ~(p->bits_0);
					bits1 &= ~(p->bits_1);
					bits2 &= ~(p->bits_2);
					bits3 &= ~(p->bits_3);
				
					continue;
				} 			
			
				/* Point to the location on the map */
				c_ptr = area(y, x);
			
				/* Get current info flags for the square */
				info = c_ptr->info;
			
				if (cave_los_grid(c_ptr))
				{
					/* Floor or semi-blocking terrain like trees */
				
					/* Enqueue child */
					if (last != p->next_0)
					{
						queue[queue_tail++] = last = p->next_0;
					}

					/* Enqueue child */
					if (last != p->next_1)
					{
						queue[queue_tail++] = last = p->next_1;
					}

					if (info & CAVE_VIEW) continue;					
					
					/* Mark as "viewable" */
					info |= (CAVE_VIEW);

					/* Torch-lit grids */
					if (p->d <= radius)
					{
						/* Mark as "CAVE_LITE" */
						info |= (CAVE_LITE);
					}
					
					/* Floor grids */
					if ((c_ptr->feat <= FEAT_INVIS) ||
						(c_ptr->feat == FEAT_WALL_INVIS))
					{
						if ((info & (CAVE_LITE)) && view_torch_grids)	
						{
							/* Memorize */
							info |= (CAVE_MARK);
						}
	
						/* Option -- memorize all perma-lit floors */
						else if (view_perma_grids && (info & (CAVE_GLOW)))
						{
							/* Memorize */
							info |= (CAVE_MARK);
						}
					}
					else
					{
						/* Memorize */
						info |= (CAVE_MARK);
					}

					/* Save cave info */
					c_ptr->info = info;

					/* Save in array */
					view_y[view_n] = y;
    				view_x[view_n] = x;
    				view_n++;
				}				
				/* Handle wall */
				else
				{
					/* Clear bits */
					bits0 &= ~(p->bits_0);
					bits1 &= ~(p->bits_1);
					bits2 &= ~(p->bits_2);
					bits3 &= ~(p->bits_3);

					/* All ready seen.  Next... */
					if (info & CAVE_VIEW) continue;
					
					/* Mark as viewable */
					info |= (CAVE_VIEW);

					/* Torch-lit grids */
					if (p->d <= radius)
					{
						/* Mark as "CAVE_LITE", "CAVE_MARK" */
						info |= (CAVE_LITE | CAVE_MARK);
					}

					/* Perma-lit grids */
					else
					{
						int yy, xx;

						/* Hack -- move one grid towards player */
						yy = (y < py) ? (y + 1) : (y > py) ? (y - 1) : y;
						xx = (x < px) ? (x + 1) : (x > px) ? (x - 1) : x;

						/* Check for "local" illumination */
						if (area(yy, xx)->info & (CAVE_GLOW))
						{
							/* Memorize */
							info |= (CAVE_MARK);
						}
					}

					/* Save cave info */
					c_ptr->info = info;

					/* Save in array */
					view_y[view_n] = y;
					view_x[view_n] = x;
					view_n++;
				}				
			}
		}
	}
	
	/*** Step 3 -- Complete the algorithm ***/

	/* Process "new" grids */
	for (i = 0; i < view_n; i++)
	{
		/* Grid */
		x = view_x[i];
		y = view_y[i];
			
		c_ptr = area(y, x);

		/* Get grid info */
		info = c_ptr->info;
		
		/* Handle blindness */
		if ((p_ptr->blind) && !(info & CAVE_XTRA))
		{
			/* Grid cannot be "CAVE_MARK" (wasn't before) */
			c_ptr->info &= ~(CAVE_MARK);
			
			/* Don't do anything else */
			continue;
		}

		/*
		 * We know we have LOS, but is it visible?
		 */
		if (info & (CAVE_LITE | CAVE_GLOW))
		{
			/* Show the objects */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				object_type *o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Memorize objects */
				o_ptr->marked = TRUE;
			}
			
			/* Must note the new information on the screen */
			if (!(info & CAVE_TEMP))
			{
				/* Redraw */
				lite_spot(y, x);
			}
		}
	}

	/* Process "old" grids */
	for (i = 0; i < temp_n; i++)
	{
		/* Grid */
		x = temp_x[i];
		y = temp_y[i];
		
		c_ptr = area(y, x);

		/* Get grid info */
		info = c_ptr->info;

		/* Clear "CAVE_TEMP" and "CAVE_XTRA" flags */
		info &= ~(CAVE_TEMP | CAVE_XTRA);

		/* Save cave info */
		c_ptr->info = info;

		/* Was "CAVE_VIEW", is now not "CAVE_VIEW" */
		if (!(info & (CAVE_VIEW)))
		{
			/* Redraw */
			lite_spot(y, x);
		}
	}
	
	/* None left */
	temp_n = 0;
}



/*
 * Hack -- provide some "speed" for the "flow" code
 * This entry is the "current index" for the "when" field
 * Note that a "when" value of "zero" means "not used".
 *
 * Note that the "cost" indexes from 1 to 127 are for
 * "old" data, and from 128 to 255 are for "new" data.
 *
 * This means that as long as the player does not "teleport",
 * then any monster up to 128 + MONSTER_FLOW_DEPTH will be
 * able to track down the player, and in general, will be
 * able to track down either the player or a position recently
 * occupied by the player.
 */
static int flow_n = 0;


/*
 * Hack -- forget the "flow" information
 */
void forget_flow(void)
{

#ifdef MONSTER_FLOW

	int x, y;

	/* Nothing to forget */
	if (!flow_n) return;

	if (!dun_level)
	{

		/* Check the entire dungeon */
		for (y = wild_grid.y_min; y < wild_grid.y_max - 1; y++)
		{
			for (x = wild_grid.x_min; x < wild_grid.x_max - 1; x++)
			{
				/* Forget the old data */
				area(y, x)->cost = 0;
				area(y, x)->when = 0;
			}
		}
	}
	else
	{
		/* Check the entire dungeon */
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				/* Forget the old data */
				area(y, x)->cost = 0;
				area(y, x)->when = 0;
			}
		}
	}

	/* Start over */
	flow_n = 0;

#endif /* MONSTER_FLOW */

}


#ifdef MONSTER_FLOW

/*
 * Hack -- Allow us to treat the "seen" array as a queue
 */
static int flow_head = 0;
static int flow_tail = 0;

/*
 * Hack - speed up the update_flow algorithm by only doing
 * it everytime the player moves out of LOS of the last
 * "way-point".
 */
static u16b flow_x = 0;
static u16b flow_y = 0;


/*
 * Take note of a reachable grid.  Assume grid is legal.
 */
static void update_flow_aux(int y, int x, int n)
{
	cave_type *c_ptr;

	int old_head = flow_head;

	/* Get the grid */
	c_ptr = area(y,x);

	/* Ignore "pre-stamped" entries */
	if (c_ptr->when == flow_n) return;

	/* Ignore "walls" and "rubble" */
	if (!cave_floor_grid(c_ptr)) return;

	/* Save the time-stamp */
	c_ptr->when = flow_n;

	/* Save the flow cost */
	c_ptr->cost = n;

	/* Hack -- limit flow depth */
	if (n == MONSTER_FLOW_DEPTH) return;

	/* Enqueue that entry */
	temp_y[flow_head] = y;
	temp_x[flow_head] = x;
	
	/* Advance the queue */
	if (++flow_head == TEMP_MAX) flow_head = 0;

	/* Hack -- notice overflow by forgetting new entry */
	if (flow_head == flow_tail) flow_head = old_head;
}

#endif /* MONSTER_FLOW */


/*
 * Hack -- fill in the "cost" field of every grid that the player
 * can "reach" with the number of steps needed to reach that grid.
 * This also yields the "distance" of the player from every grid.
 *
 * In addition, mark the "when" of the grids that can reach
 * the player with the incremented value of "flow_n".
 *
 * Hack -- use the "seen" array as a "circular queue".
 *
 * We do not need a priority queue because the cost from grid
 * to grid is always "one" and we process them in order.
 */
void update_flow(void)
{

#ifdef MONSTER_FLOW

	int x, y, d, w;
	cave_type *c_ptr;

	/* Hack -- disabled */
	if (!flow_by_sound) return;

	/* Paranoia -- make sure the array is empty */
	if (temp_n) return;

	/* The last way-point is on the map */
	if (in_bounds2(flow_y, flow_x))
	{
		/* The way point is in sight - do not update.  (Speedup) */
		if (area(flow_y, flow_x)->info & CAVE_VIEW) return;
	}
	
	/* Save player position */
	flow_y = py;
	flow_x = px;	
	
	/* Cycle the old entries (once per 128 updates) */
	if (flow_n == 255)
	{
		if (!dun_level)
		{
			/* Rotate the time-stamps */
			for (y = wild_grid.y_min; y < wild_grid.y_max; y++)
			{
				for (x = wild_grid.x_min; x < wild_grid.x_max; x++)
				{
					c_ptr = area(y, x);
					w = c_ptr->when;
					c_ptr->when = (w > 128) ? (w - 128) : 0;
				}
			}
		}
		else
		{
			/* Rotate the time-stamps */
			for (y = 0; y < cur_hgt; y++)
			{
				for (x = 0; x < cur_wid; x++)
				{
					c_ptr = area(y, x);
					w = c_ptr->when;
					c_ptr->when = (w > 128) ? (w - 128) : 0;
				}
			}
		}

		/* Restart */
		flow_n = 127;
	}

	/* Start a new flow (never use "zero") */
	flow_n++;


	/* Reset the "queue" */
	flow_head = flow_tail = 0;

	/* Add the player's grid to the queue */
	update_flow_aux(py, px, 0);

	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		y = temp_y[flow_tail];
		x = temp_x[flow_tail];

		/* Forget that entry */
		if (++flow_tail == TEMP_MAX) flow_tail = 0;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			/* Add that child if "legal" */
			if (!in_bounds2(y + ddy_ddd[d], x + ddx_ddd[d])) continue;

			update_flow_aux(y + ddy_ddd[d], x + ddx_ddd[d], area(y, x)->cost + 1);
		}
	}

	/* Forget the flow info */
	flow_head = flow_tail = 0;

#endif /* MONSTER_FLOW */

}



/*
 * Hack -- map a region ala "magic mapping"
 */
void map_area(void)
{
	int             i, x, y, y1, y2, x1, x2;

	cave_type       *c_ptr;


	/* Pick an area to map */
	y1 = py - MAX_DETECT - randint(10);
	y2 = py + MAX_DETECT + randint(10);
	x1 = px - MAX_DETECT - randint(20);
	x2 = px + MAX_DETECT + randint(20);

	if (!dun_level)
	{
		/* Speed -- shrink to fit legal bounds */
		if (y1 < wild_grid.y_min + 1) y1 = wild_grid.y_min + 1;
		if (y2 > wild_grid.y_max - 2) y2 = wild_grid.y_max - 2;
		if (x1 < wild_grid.x_min + 1) x1 = wild_grid.x_min + 1;
		if (x2 > wild_grid.x_max - 2) x2 = wild_grid.x_max - 2;
	}
	else
	{
		/* Speed -- shrink to fit legal bounds */
		if (y1 < 1) y1 = 1;
		if (y2 > cur_hgt - 2) y2 = cur_hgt - 2;
		if (x1 < 1) x1 = 1;
		if (x2 > cur_wid - 2) x2 = cur_wid - 2;
	}

	/* Scan that area */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			c_ptr = area(y,x);

			/* All non-walls are "checked" */
			if ((c_ptr->feat < FEAT_SECRET) ||
			    (c_ptr->feat == FEAT_RUBBLE) ||
				 ((c_ptr->feat >= FEAT_MINOR_GLYPH) &&
				  (c_ptr->feat != FEAT_TRAP_TRAPS) &&
				  (c_ptr->feat != FEAT_WALL_INVIS)))
			{
				/* Memorize normal features */
				if (c_ptr->feat > FEAT_INVIS)
				{
					/* Memorize the object */
					c_ptr->info |= (CAVE_MARK);
				}

				/* Memorize known walls */
				for (i = 0; i < 8; i++)
				{
					c_ptr = area(y + ddy_ddd[i],x + ddx_ddd[i]);

					/* Memorize walls (etc) */
					if ((c_ptr->feat >= FEAT_SECRET) && (c_ptr->feat != FEAT_WALL_INVIS))
					{
						/* Memorize the walls */
						c_ptr->info |= (CAVE_MARK);
					}
				}
			}
		}
	}

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}



/*
 * Light up the dungeon using "clairvoyance"
 *
 * This function "illuminates" every grid in the dungeon, memorizes all
 * "objects", memorizes all grids as with magic mapping, and, under the
 * standard option settings (view_perma_grids but not view_torch_grids)
 * memorizes all floor grids too.
 *
 * Note that if "view_perma_grids" is not set, we do not memorize floor
 * grids, since this would defeat the purpose of "view_perma_grids", not
 * that anyone seems to play without this option.
 *
 * Note that if "view_torch_grids" is set, we do not memorize floor grids,
 * since this would prevent the use of "view_torch_grids" as a method to
 * keep track of what grids have been observed directly.
 */
void wiz_lite(void)
{
	int i, y, x;

	chg_virtue(V_KNOWLEDGE, 1);
	chg_virtue(V_ENLIGHTEN, 1);

	/* Memorize objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Memorize */
		o_ptr->marked = TRUE;
	}

	/* Detect monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Repair visibility later */
		repair_monsters = TRUE;

		/* Hack -- Detect monster */
		m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

		/* Update the monster */
		update_mon(i, FALSE);
	}

	if (!dun_level)
	{
		/* Scan all normal grids */
		for (y = wild_grid.y_min; y < wild_grid.y_max; y++)
		{
			for (x = wild_grid.x_min; x < wild_grid.x_max; x++)
			{
				cave_type *c_ptr = area(y,x);

				/* Memorize normal features */
				if ((c_ptr->feat > FEAT_INVIS) && (c_ptr->feat != FEAT_WALL_INVIS))
				{
					/* Memorize the grid */
					c_ptr->info |= (CAVE_MARK);
				}
			}
		}
	}
	else
	{
		/* Scan all normal grids */
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				cave_type *c_ptr = area(y,x);

				/* Memorize normal features */
				if ((c_ptr->feat > FEAT_INVIS) && (c_ptr->feat != FEAT_WALL_INVIS))
				{
					/* Memorize the grid */
					c_ptr->info |= (CAVE_MARK);
				}
			}
		}
	}

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}


/*
 * Forget the dungeon map (ala "Thinking of Maud...").
 */
void wiz_dark(void)
{
	int i, y, x;


	if (!dun_level)
	{
		/* Forget every grid */
		for (y = wild_grid.y_min; y < wild_grid.y_max; y++)
		{
			for (x = wild_grid.x_min; x < wild_grid.x_max; x++)
			{
				cave_type *c_ptr = area(y, x);

				/* Process the grid */
				c_ptr->info &= ~(CAVE_MARK);
			}
		}

	}
	else
	{
		/* Forget every grid */
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				cave_type *c_ptr = area(y, x);

				/* Process the grid */
				c_ptr->info &= ~(CAVE_MARK);
			}
		}
	}
	/* Forget all objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Forget the object */
		o_ptr->marked = FALSE;
	}

	/* Mega-Hack -- Forget the view */
	p_ptr->update |= (PU_UN_VIEW);

	/* Update the view */
	p_ptr->update |= (PU_VIEW);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}





/*
 * Change the "feat" flag for a grid, and notice/redraw the grid
 */
void cave_set_feat(int y, int x, int feat)
{
	cave_type *c_ptr = area(y,x);

	/* Change the feature */
	c_ptr->feat = feat;

	/* Notice */
	note_spot(y, x);

	/* Redraw */
	lite_spot(y, x);
}



/*
 * Calculate "incremental motion". Used by project() and shoot().
 * Assumes that (*y,*x) lies on the path from (y1,x1) to (y2,x2).
 */
void mmove2(int *y, int *x, int y1, int x1, int y2, int x2)
{
	int dy, dx, dist, shift;

	/* Extract the distance travelled */
	dy = (*y < y1) ? y1 - *y : *y - y1;
	dx = (*x < x1) ? x1 - *x : *x - x1;

	/* Number of steps */
	dist = (dy > dx) ? dy : dx;

	/* We are calculating the next location */
	dist++;


	/* Calculate the total distance along each axis */
	dy = (y2 < y1) ? (y1 - y2) : (y2 - y1);
	dx = (x2 < x1) ? (x1 - x2) : (x2 - x1);

	/* Paranoia -- Hack -- no motion */
	if (!dy && !dx) return;


	/* Move mostly vertically */
	if (dy > dx)
	{
		/* Extract a shift factor */
		shift = (dist * dx + (dy - 1) / 2) / dy;

		/* Sometimes move along the minor axis */
		(*x) = (x2 < x1) ? (x1 - shift) : (x1 + shift);

		/* Always move along major axis */
		(*y) = (y2 < y1) ? (y1 - dist) : (y1 + dist);
	}

	/* Move mostly horizontally */
	else
	{
		/* Extract a shift factor */
		shift = (dist * dy + (dx - 1) / 2) / dx;

		/* Sometimes move along the minor axis */
		(*y) = (y2 < y1) ? (y1 - shift) : (y1 + shift);

		/* Always move along major axis */
		(*x) = (x2 < x1) ? (x1 - dist) : (x1 + dist);
	}
}



/*
 * Determine if a bolt spell cast from (y1,x1) to (y2,x2) will arrive
 * at the final destination, assuming no monster gets in the way.
 *
 * This is slightly (but significantly) different from "los(y1,x1,y2,x2)".
 */
bool projectable(int y1, int x1, int y2, int x2)
{
	int y, x;

	int grid_n = 0;
	coord grid_g[512];

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, 0);

	/* No grid is ever projectable from itself */
	if (!grid_n) return (FALSE);

	/* Final grid */
	y = grid_g[grid_n - 1].y;
	x = grid_g[grid_n - 1].x;

	/* May not end in an unrequested grid */
	if ((y != y2) || (x != x2)) return (FALSE);

	/* Assume okay */
	return (TRUE);
}


/*
 * Standard "find me a location" function
 *
 * Obtains a legal location within the given distance of the initial
 * location, and with "los()" from the source to destination location.
 *
 * This function is often called from inside a loop which searches for
 * locations while increasing the "d" distance.
 *
 * Currently the "m" parameter is unused.
 */
void scatter(int *yp, int *xp, int y, int x, int d, int m)
{
	int nx, ny;

	/* Unused */
	m = m;

	/* Pick a location */
	while (TRUE)
	{
		/* Pick a new location */
		ny = rand_spread(y, d);
		nx = rand_spread(x, d);

		/* Ignore annoying locations */
		if (!in_bounds(ny, nx)) continue;

		/* Ignore "excessively distant" locations */
		if ((d > 1) && (distance(y, x, ny, nx) > d)) continue;

		/* Require "line of sight" */
		if (los(y, x, ny, nx)) break;
	}

	/* Save the location */
	(*yp) = ny;
	(*xp) = nx;
}




/*
 * Track a new monster
 */
void health_track(int m_idx)
{
	/* Track a new guy */
	p_ptr->health_who = m_idx;

	/* Redraw (later) */
	p_ptr->redraw |= (PR_HEALTH);
}



/*
 * Hack -- track the given monster race
 */
void monster_race_track(int r_idx)
{
	/* Save this monster ID */
	p_ptr->monster_race_idx = r_idx;

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER);
}



/*
 * Hack -- track the given object kind
 */
void object_kind_track(int k_idx)
{
	/* Save this monster ID */
	p_ptr->object_kind_idx = k_idx;

	/* Window stuff */
	p_ptr->window |= (PW_OBJECT);
}



/*
 * Something has happened to disturb the player.
 *
 * The first arg indicates a major disturbance, which affects search.
 *
 * The second arg is currently unused, but could induce output flush.
 *
 * All disturbance cancels repeated commands, resting, and running.
 */
void disturb(int stop_search, int unused_flag)
{
	/* Unused */
	unused_flag = unused_flag;

	/* Cancel auto-commands */
	/* command_new = 0; */

	/* Cancel repeated commands */
	if (command_rep)
	{
		/* Cancel */
		command_rep = 0;

		/* Redraw the state (later) */
		p_ptr->redraw |= (PR_STATE);
	}

	/* Cancel Resting */
	if (resting)
	{
		/* Cancel */
		resting = 0;

		/* Redraw the state (later) */
		p_ptr->redraw |= (PR_STATE);
	}

	/* Cancel running */
	if (running)
	{
		/* Cancel */
		running = 0;

		/* Check for new panel if appropriate */
		if (center_player && avoid_center) verify_panel();

		/* Calculate torch radius */
		p_ptr->update |= (PU_TORCH);
	}

	/* Cancel searching if requested */
	if (stop_search && p_ptr->searching)
	{
		/* Cancel */
		p_ptr->searching = FALSE;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);
	}

	/* Flush the input if requested */
	if (flush_disturb) flush();
}
