/* File: borg2.c */
/* Purpose: Low level dungeon mapping skills -BEN- */

#include "angband.h"


#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg2.h"


/*
 * This file helps the Borg understand mapping the dungeon.
 *
 * Currently, this includes general routines involving dungeon grids,
 * including calculating "flow" values from place to place, determining
 * "line of sight", plus "field of view" and "torch-lit grids", setting
 * the target to a given location, and extracting the optimal direction
 * for "movement" from place to place.
 *
 * Note that the dungeon is assumed smaller than 256 by 256.
 *
 * This file also supplies the (compiled out) support for "automatic
 * room extraction".  This code will automatically group regions of
 * the dungeon into rooms, and do the "flow" navigation on those rooms
 * instead of on grids.  Often, this takes less space, and is faster,
 * howver, it is more complicated, and does not allow "specialized"
 * flow calculations that penalize grids by variable amounts.
 */

/*
 * Maximum number of slopes in a single octant
 */
#define VINFO_MAX_SLOPES 135


/*
 * Table of data used to calculate projections / los / shots.
 */
static project_type *project_data[VINFO_MAX_SLOPES];

/* Number of squares per slope */
static int slope_count[VINFO_MAX_SLOPES];

/* The min and max slopes for each square in sight */
static int p_slope_min[MAX_SIGHT + 1][MAX_SIGHT + 1];
static int p_slope_max[MAX_SIGHT + 1][MAX_SIGHT + 1];


/*
 * Maximum number of grids in a single octant
 */
#define VINFO_MAX_GRIDS 175


/*
 * Mask of bits used in a single octant
 */
#define VINFO_BITS_4 0x0000007FL
#define VINFO_BITS_3 0xFFFFFFFFL
#define VINFO_BITS_2 0xFFFFFFFFL
#define VINFO_BITS_1 0xFFFFFFFFL
#define VINFO_BITS_0 0xFFFFFFFFL


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

	u32b bits[5];

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
 * Forward declare
 */
typedef struct vinfo_hack vinfo_hack;


/*
 * Temporary data used by "vinfo_init()"
 *
 *	- Number of slopes
 *
 *	- Slope values
 *
 *  - Slope min and max for each square
 */
struct vinfo_hack
{
	int num_slopes;

	s32b slopes[VINFO_MAX_SLOPES];

	s32b slopes_min[MAX_SIGHT + 1][MAX_SIGHT + 1];
	s32b slopes_max[MAX_SIGHT + 1][MAX_SIGHT + 1];
};



/*
 * Sorting hook -- comp function -- array of s32b (see below)
 *
 * We use "u" to point to an array of s32b.
 */
static bool ang_sort_comp_hook_s32b(const vptr u, const vptr v, int a, int b)
{
	s32b *x = (s32b *)(u);

	/* Hack - ignore v */
	(void)v;

	return (x[a] <= x[b]);
}


/*
 * Sorting hook -- swap function -- array of s32b (see below)
 *
 * We use "u" to point to an array of s32b.
 */
static void ang_sort_swap_hook_s32b(const vptr u, const vptr v, int a, int b)
{
	s32b *x = (s32b *)(u);

	s32b temp;

	/* Hack - ignore v */
	(void)v;

	/* Swap */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;
}



/*
 * Save a slope
 */
static void vinfo_init_aux(vinfo_hack *hack, int x, int y, s32b m)
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
				quit_fmt("Too many slopes (%d)!", VINFO_MAX_SLOPES);
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
 * Initialize the "vinfo" and "project_data" arrays
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
static errr borg_vinfo_init(void)
{
	int i, j;
	int y, x;

	s32b m;

	vinfo_hack *hack;

	int num_grids = 0;

	int queue_head = 0;
	int queue_tail = 0;
	vinfo_type *queue[VINFO_MAX_GRIDS * 2];


	/* Make hack */
	MAKE(hack, vinfo_hack);


	/* Analyze grids */
	for (y = 0; y <= MAX_SIGHT; ++y)
	{
		for (x = y; x <= MAX_SIGHT; ++x)
		{
			/* Skip grids which are out of sight range */
			if (distance(0, 0, x, y) > MAX_SIGHT) continue;

			/* Default slope range */
			hack->slopes_min[y][x] = 999999999;
			hack->slopes_max[y][x] = 0;

			/* Clear the p_slope_min and max arrays */
			p_slope_min[x][y] = VINFO_MAX_SLOPES;
			p_slope_max[x][y] = 0;
			p_slope_min[y][x] = VINFO_MAX_SLOPES;
			p_slope_max[y][x] = 0;

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
			vinfo_init_aux(hack, x, y, m);

			/* Slope to top left corner */
			m = SCALE * (1000L * y - 500) / (1000L * x - 500);

			/* Handle "legal" slopes */
			vinfo_init_aux(hack, x, y, m);

			/* Slope to bottom right corner */
			m = SCALE * (1000L * y + 500) / (1000L * x + 500);

			/* Handle "legal" slopes */
			vinfo_init_aux(hack, x, y, m);

			/* Slope to bottom left corner */
			m = SCALE * (1000L * y + 500) / (1000L * x - 500);

			/* Handle "legal" slopes */
			vinfo_init_aux(hack, x, y, m);
		}
	}


	/* Enforce maximal efficiency */
	if (num_grids < VINFO_MAX_GRIDS)
	{
		quit_fmt("Too few grids (%d < %d)!", num_grids, VINFO_MAX_GRIDS);
	}

	/* Enforce maximal efficiency */
	if (hack->num_slopes < VINFO_MAX_SLOPES)
	{
		quit_fmt("Too few slopes (%d < %d)!",
				 hack->num_slopes, VINFO_MAX_SLOPES);
	}


	/* Sort slopes numerically */
	ang_sort_comp = ang_sort_comp_hook_s32b;

	/* Sort slopes numerically */
	ang_sort_swap = ang_sort_swap_hook_s32b;

	/* Sort the (unique) slopes */
	ang_sort(hack->slopes, NULL, hack->num_slopes);


	/* Clear the counters for each slope */
	(void)C_WIPE(slope_count, VINFO_MAX_SLOPES, int);

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
				(hack->slopes_min[y][x] < m) && (m < hack->slopes_max[y][x]))
			{
				/* We use this slope */
				slope_count[i]++;

				/* Save the bit that stands for this slope */
				vinfo[e].bits[i / 32] |= (1L << (i % 32));
			}
		}


		/* Default */
		vinfo[e].next_0 = &vinfo[0];

		/* Grid next child */
		if (distance(0, 0, x + 1, y) <= MAX_SIGHT)
		{
			if (!((queue[queue_tail - 1]->grid_x[0] == x + 1) &&
				  (queue[queue_tail - 1]->grid_y[0] == y)))
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
		if (distance(0, 0, x + 1, y + 1) <= MAX_SIGHT)
		{
			if (!((queue[queue_tail - 1]->grid_x[0] == x + 1) &&
				  (queue[queue_tail - 1]->grid_y[0] == y + 1)))
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
		vinfo[e].d = ((y > x) ? (y + x / 2) : (x + y / 2));
		vinfo[e].r = ((!y) ? x : (!x) ? y : (y == x) ? y : 0);
	}


	/* Verify maximal bits XXX XXX XXX */
	if (((vinfo[1].bits[4] | vinfo[2].bits[4]) != VINFO_BITS_4) ||
		((vinfo[1].bits[3] | vinfo[2].bits[3]) != VINFO_BITS_3) ||
		((vinfo[1].bits[2] | vinfo[2].bits[2]) != VINFO_BITS_2) ||
		((vinfo[1].bits[1] | vinfo[2].bits[1]) != VINFO_BITS_1) ||
		((vinfo[1].bits[0] | vinfo[2].bits[0]) != VINFO_BITS_0))
	{
		quit("Incorrect bit masks!");
	}

	/* Create the project_data array */
	for (i = 0; i < VINFO_MAX_SLOPES; i++)
	{
		/* Create the list of squares intersected by this slope */
		C_MAKE(project_data[i], slope_count[i], project_type);

		j = 0;

		for (y = 0; y <= MAX_SIGHT; ++y)
		{
			for (x = y; x <= MAX_SIGHT; ++x)
			{
				/* Only if in range */
				if (distance(0, 0, x, y) > MAX_SIGHT) continue;

				/* Hack - ignore the origin */
				if (!x && !y) continue;

				m = hack->slopes[i];

				/* Does this square intersect the line? */
				if ((hack->slopes_min[y][x] < m) &&
					(m < hack->slopes_max[y][x]))
				{
					/* Save the square */
					project_data[i][j].x = x;
					project_data[i][j].y = y;

					/* Add in the slopes information */
					if (p_slope_min[x][y] > i) p_slope_min[x][y] = i;
					if (p_slope_max[x][y] < i) p_slope_max[x][y] = i;

					/* Next square... */
					j++;
				}
			}
		}
	}


	/* 
	 * Add in the final information in the projection table.
	 *
	 * We need to know where to go to if the current square
	 * is blocked.
	 *
	 * This is calculated in the following way:
	 *
	 * First, we need to find the first slope that does not
	 * include the current square.
	 *
	 *  This will be the first slope that does
	 * not contain this square.  The position along that slope
	 * will be the first square that is not already scanned
	 * by the current slope.
	 *
	 * This means that we may end up scanning squares twice,
	 * but the simplification of the algorithm is worth it. 
	 */
	for (i = 0; i < VINFO_MAX_SLOPES; i++)
	{
		for (j = 0; j < slope_count[i]; j++)
		{
			/* Set default case. */
			project_data[i][j].slope = VINFO_MAX_SLOPES;
			project_data[i][j].square = 0;

			/* Find first slope without this square */
			for (x = i + 1; x < VINFO_MAX_SLOPES; x++)
			{
				bool found = FALSE;

				for (y = 0; y < slope_count[x]; y++)
				{
					if ((project_data[x][y].x == project_data[i][j].x) &&
						(project_data[x][y].y == project_data[i][j].y))
					{
						found = TRUE;
						break;
					}
				}

				/* Did we find the blocking square? */
				if (found) continue;

				/* Do we already have an answer? */
				if (project_data[i][j].slope != VINFO_MAX_SLOPES) break;

				/* We did not find the square - save the row */
				project_data[i][j].slope = x;

				/* Paranoia */
				project_data[i][j].square = 0;

				/* Find the first non-matching square */
				for (y = 0; y < slope_count[x]; y++)
				{
					if ((project_data[x][y].x != project_data[i][y].x) ||
						(project_data[x][y].y != project_data[i][y].y))
					{
						/* Not a match */
						project_data[i][j].square = y;
						break;
					}
				}
			}
		}
	}

	/* Kill hack */
	FREE(hack);

	/* Success */
	return (0);
}

/*
 * Calculate the complete field of view using a new algorithm
 *
 *
 * Normally, vision along the major axes is more likely than vision
 * along the diagonal axes, so we check the bits corresponding to
 * the lines of sight near the major axes first.
 *
 * We use the "temp_x/y" arrays (and the "CAVE_TEMP" flag) to keep track of
 * which grids were previously marked "GRID_VIEW", since only those grids
 * whose "GRID_VIEW" value changes during this routine must be redrawn.
 *
 * This function is now responsible for maintaining the "GRID_LITE"
 * flags as well as the "GRID_VIEW" flags, which is good, because
 * the only grids which normally need to be memorized and/or redrawn
 * are the ones whose "GRID_VIEW" flag changes during this routine.
 *
 * Basically, this function divides the "octagon of view" into octants of
 * grids (where grids on the main axes and diagonal axes are "shared" by
 * two octants), and processes each octant one at a time, processing each
 * octant one grid at a time, processing only those grids which "might" be
 * viewable, and setting the "GRID_VIEW" flag for each grid for which there
 * is an (unobstructed) line of sight from the center of the player grid to
 * any internal point in the grid (and collecting these "GRID_VIEW" grids
 * into the "view_g" array), and setting the "GRID_LITE" flag for the grid
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
 * is closer to the player, and in either case, the existance of a suitable
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
 * (above), which is initialised at startup.
 *
 * This has been changed to allow a more circular view, due to the more
 * advanced distance() function in Zangband.  There are now 135 lines of
 * sight and one more 32 bit flag to hold the data.
 *
 * Hack -- The queue must be able to hold more than VINFO_MAX_GRIDS grids
 * because the grids at the edge of the field of view use "grid zero" as
 * their children, and the queue must be able to hold several of these
 * special grids.  Because the actual number of required grids is bizarre,
 * we simply allocate twice as many as we would normally need.  XXX XXX XXX
 */
void borg_update_view(void)
{
	/* int py = p_ptr->py; */
	/* int px = p_ptr->px; */
	
	int py = c_y;
    int px = c_x;
	
	borg_grid *bg_ptr;
	
	/* map_block *mp_ptr; */

	byte info;

	int x, y, i, o2;

	/* Clear the old "view" grids */
	for (i = 0; i < view_n; i++)
	{
		y = borg_view_y[i];
		x = borg_view_x[i];

		if (!map_in_bounds(x, y)) continue;
		
		bg_ptr = &borg_grids[y][x];
		
		/* Clear "BORG_VIEW" flag */
		bg_ptr->info &= ~(BORG_VIEW);
	}

	/* empty the viewable list */
	borg_view_n = 0;

	/*** Step 1 -- player grid ***/

	/* Player grid */

	/* Get grid info */
	bg_ptr = &borg_grids[py][px];

	/* Assume viewable */
	bg_ptr->info |= (BORG_VIEW);

	/* Save in array */
	borg_view_y[view_n] = py;
	borg_view_x[view_n] = px;
	borg_view_n++;

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
		vinfo_type *queue[VINFO_MAX_GRIDS * 2];

		/* Slope bit vector */
		u32b bits0 = VINFO_BITS_0;
		u32b bits1 = VINFO_BITS_1;
		u32b bits2 = VINFO_BITS_2;
		u32b bits3 = VINFO_BITS_3;
		u32b bits4 = VINFO_BITS_4;

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
			if ((bits0 & (p->bits[0])) ||
				(bits1 & (p->bits[1])) ||
				(bits2 & (p->bits[2])) ||
				(bits3 & (p->bits[3])) || (bits4 & (p->bits[4])))
			{
				/* Get location */
				x = p->grid_x[o2] + px;
				y = p->grid_y[o2] + py;

				/* Is it in bounds? */
				if (!map_in_bounds(x, y))
				{
					/* Clear bits */
					bits0 &= ~(p->bits[0]);
					bits1 &= ~(p->bits[1]);
					bits2 &= ~(p->bits[2]);
					bits3 &= ~(p->bits[3]);
					bits4 &= ~(p->bits[4]);

					continue;
				}

				/* Point to the location on the map */
				bg_ptr = &borg_grids[y][x];

				/* Get current info flags for the square */
				info = bg_ptr->info;

				if (borg_cave_los_grid(bg_ptr))
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
				}
				/* Handle wall */
				else
				{
					/* Clear bits */
					bits0 &= ~(p->bits[0]);
					bits1 &= ~(p->bits[1]);
					bits2 &= ~(p->bits[2]);
					bits3 &= ~(p->bits[3]);
					bits4 &= ~(p->bits[4]);
				}
				
				/* All ready seen.  Next... */
				if (info & BORG_VIEW) continue;

				/* Mark as viewable */
				info |= (BORG_VIEW);

				/* Save cave info */
				bg_ptr->info = info;

				/* Save in array */
				borg_view_y[view_n] = y;
				borg_view_x[view_n] = x;
				borg_view_n++;
			}
		}
	}
}






/*
 * A simple, fast, integer-based line-of-sight algorithm.
 *
 * See "los()" in "cave.c" for complete documentation
 */
bool borg_los(int y1, int x1, int y2, int x2)
{

/* this routine is seriously flawed.  For the time being replace this with
 * the function borg_projectable().  It is slightly better.
 */

 if (borg_projectable(y1,x1,y2,x2)) return (TRUE);
 return (FALSE);

}

/*
 * Check the projection from (x1,y1) to (x2,y2).
 * Assume that there is no monster in the way.
 * Hack -- we refuse to assume that unknown grids are floors
 * Adapted from "projectable()" in "spells1.c".
 */
bool borg_projectable(int y1, int x1, int y2, int x2)
{
    int dist, y, x;

    borg_grid *ag;

    /* Start at the initial location */
    y = y1; x = x1;

    /* Simulate the spell/missile path */
    for (dist = 0; dist <= MAX_RANGE; dist++)
    {
        /* Get the grid */
        ag = &borg_grids[y][x];

        if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2)
        {
            /* Assume all unknow grids more than distance 10 from you */
            /* are walls--when I am wounded. This will make me more fearful */
            if ((dist > 10) && (ag->feat == FEAT_NONE)) break;
        }
        else
        {
            /* Assume all unknow grids more than distance 3 from you */
            /* are walls. */
            if ((dist > 2) && (ag->feat == FEAT_NONE)) break;
        }
        /* Never pass through walls/doors */
        if (dist && (!borg_cave_floor_grid(ag))) break;

        /* Check for arrival at "final target" */
        if ((x == x2) && (y == y2)) return (TRUE);

        /* Calculate the new location */
        borgmove2(&y, &x, y1, x1, y2, x2);
    }

    /* Assume obstruction */
    return (FALSE);
}


/*
 * Check the projection from (x1,y1) to (x2,y2).
 * Assume that there is no monster in the way.
 * Hack -- we refuse to assume that unknown grids are floors
 * Adapted from "projectable()" in "spells1.c".
 * This is used by borg_offset()
 */
bool borg_offset_projectable(int y1, int x1, int y2, int x2)
{
    int dist, y, x;

    borg_grid *ag;

    /* Start at the initial location */
    y = y1; x = x1;

    /* Simulate the spell/missile path */
    for (dist = 0; dist <= MAX_RANGE; dist++)
    {
        /* Get the grid */
        ag = &borg_grids[y][x];

        /* Assume all unknown grids are walls. */
        if ((dist) && (ag->feat == FEAT_NONE)) break;

        /* Never pass through walls/doors */
        if (dist && (!borg_cave_floor_grid(ag))) break;

        /* Check for arrival at "final target" */
        if ((x == x2) && (y == y2)) return (TRUE);

        /* Calculate the new location */
        borgmove2(&y, &x, y1, x1, y2, x2);
    }

    /* Assume obstruction */
    return (FALSE);
}


/*
 * Check the projection from (x1,y1) to (x2,y2).
 * Assume that monsters in the way will stop the projection
 * Hack -- we refuse to assume that unknown grids are floors
 * Adapted from "projectable()" in "spells1.c".
 */
bool borg_projectable_pure(int y1, int x1, int y2, int x2)
{
    int dist, y, x;
    borg_grid *ag;

    /* Start at the initial location */
    y = y1; x = x1;

    /* Simulate the spell/missile path */
    for (dist = 0; dist <= MAX_RANGE; dist++)
    {
        /* Get the grid */
        ag = &borg_grids[y][x];

        /* Hack -- assume unknown grids are walls */
        if (dist && (ag->feat == FEAT_NONE)) break;

        /* Never pass through walls/doors */
        if (dist && (!borg_cave_floor_grid(ag))) break;

        /* Check for arrival at "final target" */
        if ((x == x2) && (y == y2)) return (TRUE);

        /* Stop at monsters */
        if (ag->kill) break;

        /* Calculate the new location */
        borgmove2(&y, &x, y1, x1, y2, x2);
    }

    /* Assume obstruction */
    return (FALSE);
}


/*
 * Clear the viewable space
 */
void borg_forget_view(void)
{
    int i;

    borg_grid *ag;

    /* None to forget */
    if (!borg_view_n) return;

    /* Clear them all */
    for (i = 0; i < borg_view_n; i++)
    {
        int y = borg_view_y[i];
        int x = borg_view_x[i];

        /* Access the grid */
        ag = &borg_grids[y][x];

        /* Forget that the grid is viewable */
        ag->info &= ~BORG_VIEW;
    }

    /* None left */
    borg_view_n = 0;
}

#if 0

/*
 * This macro allows us to efficiently add a grid to the "view" array,
 * note that we are never called for illegal grids, or for grids which
 * have already been placed into the "view" array, and we are never
 * called when the "view" array is full.
 */
#define borg_cave_view_hack(A,Y,X) \
    (A)->info |= BORG_VIEW; \
    borg_view_y[borg_view_n] = (Y); \
    borg_view_x[borg_view_n] = (X); \
    borg_view_n++



/*
 * Helper function for "borg_update_view()" below
 *
 * See "update_view_aux()" in "cave.c" for complete documentation.
 */
static bool borg_update_view_aux(int y, int x, int y1, int x1, int y2, int x2)
{
    bool f1, f2, v1, v2, z1, z2, wall;

    borg_grid *ag;

    borg_grid *g1_ag;
    borg_grid *g2_ag;

    /* Access the grids */
    g1_ag = &borg_grids[y1][x1];
    g2_ag = &borg_grids[y2][x2];


    /* Check for walls */
    f1 = (borg_cave_floor_grid(g1_ag));
    f2 = (borg_cave_floor_grid(g2_ag));

    /* Totally blocked by physical walls */
    if (!f1 && !f2) return (TRUE);


    /* Check for visibility */
    v1 = (f1 && (g1_ag->info & BORG_VIEW));
    v2 = (f2 && (g2_ag->info & BORG_VIEW));

    /* Totally blocked by "unviewable neighbors" */
    if (!v1 && !v2) return (TRUE);


    /* Access the grid */
    ag = &borg_grids[y][x];


    /* Check for walls */
    wall = (!borg_cave_floor_grid(ag));


    /* Check the "ease" of visibility */
    z1 = (v1 && (g1_ag->info & BORG_XTRA));
    z2 = (v2 && (g2_ag->info & BORG_XTRA));

    /* Hack -- "easy" plus "easy" yields "easy" */
    if (z1 && z2)
    {
        ag->info |= BORG_XTRA;

        borg_cave_view_hack(ag, y, x);

        return (wall);
    }

    /* Hack -- primary "easy" yields "viewed" */
    if (z1)
    {
        borg_cave_view_hack(ag, y, x);

        return (wall);
    }


    /* Hack -- "view" plus "view" yields "view" */
    if (v1 && v2)
    {
        /* ag->info |= BORG_XTRA; */

        borg_cave_view_hack(ag, y, x);

        return (wall);
    }


    /* Mega-Hack -- the "borg_los()" function works poorly on walls */
    if (wall)
    {
        borg_cave_view_hack(ag, y, x);

        return (wall);
    }


    /* Hack -- check line of sight */
    if (borg_los(c_y, c_x, y, x))
    {
        borg_cave_view_hack(ag, y, x);

        return (wall);
    }


    /* Assume no line of sight. */
    return (TRUE);
}


/*
 * Calculate the region currently "viewable" by the player
 *
 * See "update_view()" in "cave.c" for complete documentation
 *
 * It is very important that the "player grid" be the first grid in the
 * array of "BORG_VIEW" grids, since this is assumed in several places.
 */
void borg_update_view(void)
{
    int n, m, d, k, y, x, z;

    int se, sw, ne, nw, es, en, ws, wn;

    int full, over;

    borg_grid *ag;


    /*** Initialize ***/

    /* Normal */
    /* Full radius (20) */
    full = MAX_SIGHT;

    /* Octagon factor (30) */
    over = MAX_SIGHT * 3 / 2;


    /*** Step 0 -- Begin ***/

    /* Save the old "view" grids for later */
    for (n = 0; n < borg_view_n; n++)
    {
        y = borg_view_y[n];
        x = borg_view_x[n];

        /* Access the grid */
        ag = &borg_grids[y][x];

        /* Mark the grid as not in "view" */
        ag->info &= ~(BORG_VIEW);
    }

    /* Start over with the "view" array */
    borg_view_n = 0;

    /*** Step 1 -- adjacent grids ***/

    /* Now start on the player */
    y = c_y;
    x = c_x;

	/* These locations Make Zborg crash in Wilderness */
	if (y == 1 || x == 1) return;
	if (y == 63 || x == 196) return;

    /* Access the grid */
    ag = &borg_grids[y][x];

    /* Assume the player grid is easily viewable */
    ag->info |= BORG_XTRA;

    /* Assume the player grid is viewable */
    borg_cave_view_hack(ag, y, x);


    /*** Step 2 -- Major Diagonals ***/

    /* Hack -- Limit */
    z = full * 2 / 3;

    /* Scan south-east */
    for (d = 1; d <= z; d++)
    {
		if ((y + d > AUTO_MAX_Y-1) || (x + d > AUTO_MAX_X-1)) continue;
		ag = &borg_grids[y+d][x+d];
        ag->info |= BORG_XTRA;
        borg_cave_view_hack(ag, y+d, x+d);
        if (!borg_cave_floor_grid(ag)) break;
    }

    /* Scan south-west */
    for (d = 1; d <= z; d++)
    {
		if ((y + d > AUTO_MAX_Y-1) || (x - d < 0)) continue;
        ag = &borg_grids[y+d][x-d];
        ag->info |= BORG_XTRA;
        borg_cave_view_hack(ag, y+d, x-d);
        if (!borg_cave_floor_grid(ag)) break;
    }

    /* Scan north-east */
    for (d = 1; d <= z; d++)
    {
		if ((y - d < 0) || (x + d > AUTO_MAX_X-1)) continue;
        ag = &borg_grids[y-d][x+d];
        ag->info |= BORG_XTRA;
        borg_cave_view_hack(ag, y-d, x+d);
        if (!borg_cave_floor_grid(ag)) break;
    }

    /* Scan north-west */
    for (d = 1; d <= z; d++)
    {
		if ((y - d < 0) || (x - d < 0)) continue;
        ag = &borg_grids[y-d][x-d];
        ag->info |= BORG_XTRA;
        borg_cave_view_hack(ag, y-d, x-d);
        if (!borg_cave_floor_grid(ag)) break;
    }


    /*** Step 3 -- major axes ***/

    /* Scan south */
    for (d = 1; d <= full; d++)
    {
		if ((y + d > AUTO_MAX_Y-1) || (x > AUTO_MAX_X-1)) continue;
        ag = &borg_grids[y+d][x];
        ag->info |= BORG_XTRA;
        borg_cave_view_hack(ag, y+d, x);
        if (!borg_cave_floor_grid(ag)) break;
    }

    /* Initialize the "south strips" */
    se = sw = d;

    /* Scan north */
    for (d = 1; d <= full; d++)
    {
		if ((y - d < 0) || (x > AUTO_MAX_X-1)) continue;
        ag = &borg_grids[y-d][x];
        ag->info |= BORG_XTRA;
        borg_cave_view_hack(ag, y-d, x);
        if (!borg_cave_floor_grid(ag)) break;
    }

    /* Initialize the "north strips" */
    ne = nw = d;

    /* Scan east */
    for (d = 1; d <= full; d++)
    {
		if ((y > AUTO_MAX_Y-1) || (x + d > AUTO_MAX_X-1)) continue;
        ag = &borg_grids[y][x+d];
        ag->info |= BORG_XTRA;
        borg_cave_view_hack(ag, y, x+d);
        if (!borg_cave_floor_grid(ag)) break;
    }

    /* Initialize the "east strips" */
    es = en = d;

    /* Scan west */
    for (d = 1; d <= full; d++)
    {
		if ((y > AUTO_MAX_Y-1) || (x - d < 0)) continue;
        ag = &borg_grids[y][x-d];
        ag->info |= BORG_XTRA;
        borg_cave_view_hack(ag, y, x-d);
        if (!borg_cave_floor_grid(ag)) break;
    }

    /* Initialize the "west strips" */
    ws = wn = d;


    /*** Step 4 -- Divide each "octant" into "strips" ***/

    /* Now check each "diagonal" (in parallel) */
    for (n = 1; n <= over / 2; n++)
    {
        int ypn, ymn, xpn, xmn;


        /* Acquire the "bounds" of the maximal circle */
        z = over - n - n;
        if (z > full - n) z = full - n;
        while ((z + n + (n>>1)) > full) z--;


        /* Access the four diagonal grids */
        ypn = y + n;
        ymn = y - n;
        xpn = x + n;
        xmn = x - n;


        /* South strip */
        if (ypn < AUTO_MAX_Y-1)
        {
            /* Maximum distance */
            m = MIN(z, (AUTO_MAX_Y-1) - ypn);

            /* East side */
            if ((xpn <= AUTO_MAX_X-1) && (n < se))
            {
                /* Scan */
                for (k = n, d = 1; d <= m; d++)
                {
                    /* Check grid "d" in strip "n", notice "blockage" */
                    if (borg_update_view_aux(ypn+d, xpn, ypn+d-1, xpn-1, ypn+d-1, xpn))
                    {
                        if (n + d >= se) break;
                    }

                    /* Track most distant "non-blockage" */
                    else
                    {
                        k = n + d;
                    }
                }

                /* Limit the next strip */
                se = k + 1;
            }

            /* West side */
            if ((xmn >= 0) && (n < sw))
            {
                /* Scan */
                for (k = n, d = 1; d <= m; d++)
                {
                    /* Check grid "d" in strip "n", notice "blockage" */
                    if (borg_update_view_aux(ypn+d, xmn, ypn+d-1, xmn+1, ypn+d-1, xmn))
                    {
                        if (n + d >= sw) break;
                    }

                    /* Track most distant "non-blockage" */
                    else
                    {
                        k = n + d;
                    }
                }

                /* Limit the next strip */
                sw = k + 1;
            }
        }


        /* North strip */
        if (ymn > 0)
        {
            /* Maximum distance */
            m = MIN(z, ymn);

            /* East side */
            if ((xpn <= AUTO_MAX_X-1) && (n < ne))
            {
                /* Scan */
                for (k = n, d = 1; d <= m; d++)
                {
                    /* Check grid "d" in strip "n", notice "blockage" */
                    if (borg_update_view_aux(ymn-d, xpn, ymn-d+1, xpn-1, ymn-d+1, xpn))
                    {
                        if (n + d >= ne) break;
                    }

                    /* Track most distant "non-blockage" */
                    else
                    {
                        k = n + d;
                    }
                }

                /* Limit the next strip */
                ne = k + 1;
            }

            /* West side */
            if ((xmn >= 0) && (n < nw))
            {
                /* Scan */
                for (k = n, d = 1; d <= m; d++)
                {
                    /* Check grid "d" in strip "n", notice "blockage" */
                    if (borg_update_view_aux(ymn-d, xmn, ymn-d+1, xmn+1, ymn-d+1, xmn))
                    {
                        if (n + d >= nw) break;
                    }

                    /* Track most distant "non-blockage" */
                    else
                    {
                        k = n + d;
                    }
                }

                /* Limit the next strip */
                nw = k + 1;
            }
        }


        /* East strip */
        if (xpn < AUTO_MAX_X-1)
        {
            /* Maximum distance */
            m = MIN(z, (AUTO_MAX_X-1) - xpn);

            /* South side */
            if ((ypn <= AUTO_MAX_Y-1) && (n < es))
            {
                /* Scan */
                for (k = n, d = 1; d <= m; d++)
                {
                    /* Check grid "d" in strip "n", notice "blockage" */
                    if (borg_update_view_aux(ypn, xpn+d, ypn-1, xpn+d-1, ypn, xpn+d-1))
                    {
                        if (n + d >= es) break;
                    }

                    /* Track most distant "non-blockage" */
                    else
                    {
                        k = n + d;
                    }
                }

                /* Limit the next strip */
                es = k + 1;
            }

            /* North side */
            if ((ymn >= 0) && (n < en))
            {
                /* Scan */
                for (k = n, d = 1; d <= m; d++)
                {
                    /* Check grid "d" in strip "n", notice "blockage" */
                    if (borg_update_view_aux(ymn, xpn+d, ymn+1, xpn+d-1, ymn, xpn+d-1))
                    {
                        if (n + d >= en) break;
                    }

                    /* Track most distant "non-blockage" */
                    else
                    {
                        k = n + d;
                    }
                }

                /* Limit the next strip */
                en = k + 1;
            }
        }


        /* West strip */
        if (xmn > 0)
        {
            /* Maximum distance */
            m = MIN(z, xmn);

            /* South side */
            if ((ypn <= AUTO_MAX_Y-1) && (n < ws))
            {
                /* Scan */
                for (k = n, d = 1; d <= m; d++)
                {
                    /* Check grid "d" in strip "n", notice "blockage" */
                    if (borg_update_view_aux(ypn, xmn-d, ypn-1, xmn-d+1, ypn, xmn-d+1))
                    {
                        if (n + d >= ws) break;
                    }

                    /* Track most distant "non-blockage" */
                    else
                    {
                        k = n + d;
                    }
                }

                /* Limit the next strip */
                ws = k + 1;
            }

            /* North side */
            if ((ymn >= 0) && (n < wn))
            {
                /* Scan */
                for (k = n, d = 1; d <= m; d++)
                {
                    /* Check grid "d" in strip "n", notice "blockage" */
                    if (borg_update_view_aux(ymn, xmn-d, ymn+1, xmn-d+1, ymn, xmn-d+1))
                    {
                        if (n + d >= wn) break;
                    }

                    /* Track most distant "non-blockage" */
                    else
                    {
                        k = n + d;
                    }
                }

                /* Limit the next strip */
                wn = k + 1;
            }
        }
    }


    /*** Step 5 -- Complete the algorithm ***/

    /* Update all the new grids */
    for (n = 0; n < borg_view_n; n++)
    {
        y = borg_view_y[n];
        x = borg_view_x[n];

        /* Access the grid */
        ag = &borg_grids[y][x];

        /* Clear the "BORG_XTRA" flag */
        ag->info &= ~BORG_XTRA;
    }
}


#endif /* 0 */


/*
 * Init this file.
 */
void borg_init_2(void)
{
	/* Initialise los information */
	borg_vinfo_init();
}



#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
