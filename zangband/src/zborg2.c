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
	map_block *mb_ptr;

	byte info;

	int x, y, i, o2;

	/* Clear the old "view" grids */
	for (i = 0; i < view_n; i++)
	{
		y = borg_view_y[i];
		x = borg_view_x[i];

		if (!map_in_bounds(x, y)) continue;

		mb_ptr = map_loc(x, y);

		/* Clear "BORG_VIEW" flag */
		mb_ptr->info &= ~(BORG_MAP_VIEW);
	}

	/* empty the viewable list */
	borg_view_n = 0;

	/*** Step 1 -- player grid ***/

	/* Player grid */

	/* Get grid info */
	mb_ptr = map_loc(c_x, c_y);

	/* Assume viewable */
	mb_ptr->info |= (BORG_MAP_VIEW);

	/* Save in array */
	borg_view_y[view_n] = c_y;
	borg_view_x[view_n] = c_x;
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
				x = p->grid_x[o2] + c_x;
				y = p->grid_y[o2] + c_y;

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
				mb_ptr = map_loc(x, y);

				/* Get current info flags for the square */
				info = mb_ptr->info;

				if (borg_cave_los_grid(mb_ptr))
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
				if (info & BORG_MAP_VIEW) continue;

				/* Mark as viewable */
				info |= (BORG_MAP_VIEW);

				/* Save cave info */
				mb_ptr->info = info;

				/* Save in array */
				borg_view_y[view_n] = y;
				borg_view_x[view_n] = x;
				borg_view_n++;
			}
		}
	}
}


/*
 * Clear the viewable space
 */
void borg_forget_view(void)
{
	int i;

	map_block *mb_ptr;

	/* None to forget */
	if (!borg_view_n) return;

	/* Clear them all */
	for (i = 0; i < borg_view_n; i++)
	{
		int y = borg_view_y[i];
		int x = borg_view_x[i];

		/* Bounds checking */
		if (!map_in_bounds(x, y)) continue;

		/* Access the grid */
		mb_ptr = map_loc(x, y);

		/* Forget that the grid is viewable */
		mb_ptr->info &= ~BORG_MAP_VIEW;
	}

	/* None left */
	borg_view_n = 0;
}


/*
 * This is a version of the los function that uses a
 * tester function to determine whether or not to stop.
 *
 * Use this function instead of cut+pasting los() everywhere
 * with only tiny changes made to it.
 *
 * This works by following a "ray" that is one of those used
 * in the update_view() routine.
 *
 * We pick the minimal sloped ray that passes through the
 * required square.  We then follow that ray, looking at each
 * grid along it.  If the grid passes c_hook() then we keep
 * going.  If the grid does not, then we go to the minimal
 * slope that does not pass through this blocking grid.
 * We go to the first unchecked square along that ray - and
 * then continue following it.
 *
 * If the new ray does not pass through the target square, then
 * its slope will be greater than the maximal slope of the
 * target.
 *
 * This routine will over-check some squares in a worst-case
 * scenario - but it is fairly efficient.  Most of the required
 * information has been pre-calculated in the code that also
 * works out the data used by update_view()
 *
 * Unlike the old los() routine, this will give exactly the
 * same results as testing cave_view_grid() after using
 * update_view().
 */
static bool los_general(int x1, int y1, int x2, int y2, map_hook_type mb_hook)
{
	int i, j, temp, dist;

	int x, y;

	int dx, dy, ax, ay, sx, sy;

	map_block *mb_ptr;

	dist = distance(x1, y1, x2, y2);

	/* If (x1,y1) == (x2, y2) we know we can see ourselves */
	if (dist == 0) return (TRUE);

	/* We only work for points that are less than MAX_SIGHT appart. */
	if (dist > MAX_SIGHT) return (FALSE);

	/* Extract the offset */
	dy = y2 - y1;
	dx = x2 - x1;

	/* Extract the absolute offset */
	ay = ABS(dy);
	ax = ABS(dx);

	/*
	 * Start at the first square in the list.
	 * This is a square adjacent to (x1,y1)
	 */
	j = 0;

	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;

	/* Hack - we need to stick to one octant */
	if (ay < ax)
	{
		/* Look up the slope to use */
		i = p_slope_min[ax][ay];

		while (i <= p_slope_max[ax][ay])
		{
			x = x1 + sx * project_data[i][j].x;
			y = y1 + sy * project_data[i][j].y;

			/* Done? */
			if ((x == x2) && (y == y2)) return (TRUE);

			/* Stop if out of bounds */
			if (!map_in_bounds(x, y)) return (FALSE);

			mb_ptr = map_loc(x, y);

			if ((*mb_hook) (mb_ptr))
			{
				/* Blocked: go to the best position we have not looked at yet */
				temp = project_data[i][j].slope;
				j = project_data[i][j].square;
				i = temp;
			}
			else
			{
				/* Advance along ray */
				j++;
			}
		}
	}
	else
	{
		/* Look up the slope to use */
		i = p_slope_min[ay][ax];

		while (i <= p_slope_max[ay][ax])
		{
			/* Note that the data offsets have x,y swapped */
			x = x1 + sx * project_data[i][j].y;
			y = y1 + sy * project_data[i][j].x;

			/* Done? */
			if ((x == x2) && (y == y2)) return (TRUE);

			/* Stop if out of bounds */
			if (!map_in_bounds(x, y)) return (FALSE);

			mb_ptr = map_loc(x, y);

			if ((*mb_hook) (mb_ptr))
			{
				/* Blocked: go to the best position we have not looked at yet */
				temp = project_data[i][j].slope;
				j = project_data[i][j].square;
				i = temp;
			}
			else
			{
				/* Advance along ray */
				j++;
			}
		}
	}

	/* No path */
	return (FALSE);
}


/* Slope and square used by mmove2 */
static int mmove_slope;
static int mmove_sq;

/* Direction to move in */
static int mmove_dx;
static int mmove_dy;


/*
 * Calculate the slope and square information used by
 * a following mmove2
 */
void borg_mmove_init(int x1, int y1, int x2, int y2)
{
	int temp;

	int xx, yy;

	int dx, dy, ax, ay, sx, sy, dist;

	map_block *mb_ptr;
	
	bool is_projectable;

	/* Clear slope and square */
	mmove_slope = 0;
	mmove_sq = 0;

	/* Clear direction */
	mmove_dx = 0;
	mmove_dy = 0;

	/* Paranoia - degenerate case */
	if ((x1 == x2) && (y1 == y2)) return;

	/* Extract the offset */
	dy = y2 - y1;
	dx = x2 - x1;

	/*
	 * We only work for points that are less than MAX_SIGHT appart.
	 * Note that MAX_RANGE < MAX_SIGHT
	 */
	dist = distance(x1, y1, x2, y2);

	if (dist > MAX_SIGHT)
	{
		/* Rescale */
		dx = (dx * MAX_SIGHT) / dist;
		dy = (dy * MAX_SIGHT) / dist;
	}

	/* Save direction */
	mmove_dx = dx;
	mmove_dy = dy;

	/* Extract the absolute offset */
	ay = ABS(dy);
	ax = ABS(dx);

	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;
	
	
	/* Is the square projectable from here? */
	is_projectable = projectable(x1, y1, x2, y2);

	/*
	 * Start at the first square in the list.
	 * This is a square adjacent to (x1,y1)
	 */

	/* Hack - we need to stick to one octant */
	if (ay < ax)
	{
		/* Look up the slope to use */
		mmove_slope = p_slope_min[ax][ay];

		while (mmove_slope <= p_slope_max[ax][ay])
		{
			xx = x1 + sx * project_data[mmove_slope][mmove_sq].x;
			yy = y1 + sy * project_data[mmove_slope][mmove_sq].y;

			/* Done? */
			if ((xx == x1 + dx) && (yy == y1 + dy)) break;
			
			/* Paranoia */
			if (!map_in_bounds(xx, yy)) break;

			mb_ptr = map_loc(xx, yy);
			
			/* Do we want to stop early? */
			if (!is_projectable && mb_ptr->monster) break;

			/* Is the square not occupied by a monster, and passable? */
			if (!borg_cave_los_grid(mb_ptr) || mb_ptr->monster)
			{
				/* Advance to the best position we have not looked at yet */
				temp = project_data[mmove_slope][mmove_sq].slope;
				mmove_sq = project_data[mmove_slope][mmove_sq].square;
				mmove_slope = temp;
			}
			else
			{
				/* Advance along ray */
				(mmove_sq)++;
			}
		}

		/* No match? */
		if (mmove_slope > p_slope_max[ax][ay])
		{
			mmove_slope = (p_slope_min[ax][ay] + p_slope_max[ax][ay]) / 2;
		}
	}
	else
	{
		/* Look up the slope to use */
		mmove_slope = p_slope_min[ay][ax];

		while (mmove_slope <= p_slope_max[ay][ax])
		{
			/* Note that the data offsets have x,y swapped */
			xx = x1 + sx * project_data[mmove_slope][mmove_sq].y;
			yy = y1 + sy * project_data[mmove_slope][mmove_sq].x;

			/* Done? */
			if ((xx == x1 + dx) && (yy == y1 + dy)) break;

			/* Paranoia */
			if (!map_in_bounds(xx, yy)) break;

			mb_ptr = map_loc(xx, yy);
			
			/* Do we want to stop early? */
			if (!is_projectable && mb_ptr->monster) break;

			/* Is the square not occupied by a monster, and passable? */
			if (!cave_los_grid(mb_ptr) || mb_ptr->monster)
			{
				/* Advance to the best position we have not looked at yet */
				temp = project_data[mmove_slope][mmove_sq].slope;
				mmove_sq = project_data[mmove_slope][mmove_sq].square;
				mmove_slope = temp;
			}
			else
			{
				/* Advance along ray */
				(mmove_sq)++;
			}
		}

		/* No match? */
		if (mmove_slope > p_slope_max[ay][ax])
		{
			mmove_slope = (p_slope_min[ay][ax] + p_slope_max[ay][ax]) / 2;
		}
	}


	/*
	 * Reset to start.
	 *
	 * Square zero is the the first square along the path.
	 * It is not the starting square
	 */
	mmove_sq = 0;
}

/*
 * Calculate incremental motion
 *
 * The current position is updated.
 *
 * (x,y) encodes the current location.
 *
 * This routine is very similar to los() except that we can use it
 * to return partial results.
 */
void borg_mmove(int *x, int *y, int x1, int y1)
{
	int ax, ay, sx, sy;

	/* Extract the absolute offset */
	ay = ABS(mmove_dy);
	ax = ABS(mmove_dx);

	/* Extract some signs */
	sx = (mmove_dx < 0) ? -1 : 1;
	sy = (mmove_dy < 0) ? -1 : 1;

	/* Paranoia - square number is too large */
	if (mmove_sq >= slope_count[mmove_slope])
	{
		mmove_sq = slope_count[mmove_slope] - 1;
	}

	if (ay < ax)
	{
		/* Work out square to return */
		*x = x1 + sx * project_data[mmove_slope][mmove_sq].x;
		*y = y1 + sy * project_data[mmove_slope][mmove_sq].y;
	}
	else
	{
		/* Work out square to return */
		*x = x1 + sx * project_data[mmove_slope][mmove_sq].y;
		*y = y1 + sy * project_data[mmove_slope][mmove_sq].x;
	}

	/* Next square, next time. */
	mmove_sq++;
}


/*
 * Calculate "incremental motion". Used by project() and shoot().
 * Assumes that (*y,*x) lies on the path from (y1,x1) to (y2,x2).
 */
/* changing this to be more like project_path */
/* note that this is much slower but much more accurate */
void borgmove2(int *py, int *px, int y1, int x1, int y2, int x2)
{
	int dy, dx;
	int sy, sx;
	int y, x;

	/* Scale factors */
	int full, half;

	/* Fractions */
	int frac;

	/* Slope */
	int m;

	/* Distance */
	int k = 0;

	/* Extract the distance travelled */
	/* Analyze "dy" */
	if (y2 < y1)
	{
		dy = (y1 - y2);
		sy = -1;
	}
	else
	{
		dy = (y2 - y1);
		sy = 1;
	}

	/* Analyze "dx" */
	if (x2 < x1)
	{
		dx = (x1 - x2);
		sx = -1;
	}
	else
	{
		dx = (x2 - x1);
		sx = 1;
	}

	/* Paranoia -- Hack -- no motion */
	if (!dy && !dx) return;

	/* Number of "units" in one "half" grid */
	half = (dy * dx);

	/* Number of "units" in one "full" grid */
	full = half << 1;

	/* First step is fixed */
	if (*px == x1 && *py == y1)
	{
		if (dy > dx)
		{
			*py += sy;
			return;
		}
		else if (dx > dy)
		{
			*px += sx;
			return;
		}
		else
		{
			*px += sx;
			*py += sy;
			return;
		}
	}

	/* Move mostly vertically */
	if (dy > dx)
	{
		k = dy;

		/* Start at tile edge */
		frac = dx * dx;

		/* Let m = ((dx/dy) * full) = (dx * dx * 2) = (frac * 2) */
		m = frac << 1;

		/* Start */
		y = y1 + sy;
		x = x1;

		/* Create the projection path */
		while (1)
		{
			if (x == *px && y == *py)
				k = 1;

			/* Slant */
			if (m)
			{
				/* Advance (X) part 1 */
				frac += m;

				/* Horizontal change */
				if (frac >= half)
				{
					/* Advance (X) part 2 */
					x += sx;

					/* Advance (X) part 3 */
					frac -= full;
				}
			}

			/* Advance (Y) */
			y += sy;

			/* Track distance */
			k--;

			if (!k)
			{
				*px = x;
				*py = y;
				return;
			}
		}
	}
	/* Move mostly horizontally */
	else if (dx > dy)
	{
		/* Start at tile edge */
		frac = dy * dy;

		/* Let m = ((dy/dx) * full) = (dy * dy * 2) = (frac * 2) */
		m = frac << 1;

		/* Start */
		y = y1;
		x = x1 + sx;
		k = dx;

		/* Create the projection path */
		while (1)
		{
			if (x == *px && y == *py)
				k = 1;

			/* Slant */
			if (m)
			{
				/* Advance (Y) part 1 */
				frac += m;

				/* Vertical change */
				if (frac >= half)
				{
					/* Advance (Y) part 2 */
					y += sy;

					/* Advance (Y) part 3 */
					frac -= full;
				}
			}

			/* Advance (X) */
			x += sx;

			/* Track distance */
			k--;

			if (!k)
			{
				*px = x;
				*py = y;
				return;
			}
		}
	}
	/* Diagonal */
	else
	{
		/* Start */
		k = dy;
		y = y1 + sy;
		x = x1 + sx;

		/* Create the projection path */
		while (1)
		{
			if (x == *px && y == *py)
				k = 1;

			/* Advance (Y) */
			y += sy;

			/* Advance (X) */
			x += sx;

			/* Track distance */
			k--;

			if (!k)
			{
				*px = x;
				*py = y;
				return;
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

	if (borg_projectable(y1, x1, y2, x2)) return (TRUE);
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

	map_block *mb_ptr;

	/* Start at the initial location */
	y = y1;
	x = x1;

	/* Simulate the spell/missile path */
	for (dist = 0; dist <= MAX_RANGE; dist++)
	{
		/* Bounds checking */
		if (!map_in_bounds(x, y)) break;

		/* Get the grid */
		mb_ptr = map_loc(x, y);

		if (borg_skill[BI_CURHP] < borg_skill[BI_MAXHP] / 2)
		{
			/*
			 * Assume all unknown grids more than distance 10 from you
			 * are walls--when I am wounded. This will make me more fearful
			 */
			if ((dist > 10) && !mb_ptr->feat) break;
		}
		else
		{
			/*
			 * Assume all unknown grids more than distance 3 from you
			 * are walls.
			 */
			if ((dist > 2) && !mb_ptr->feat) break;
		}
		/* Never pass through walls/doors */
		if (dist && borg_cave_wall_grid(mb_ptr)) break;

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

	map_block *mb_ptr;

	/* Start at the initial location */
	y = y1;
	x = x1;

	/* Simulate the spell/missile path */
	for (dist = 0; dist <= MAX_RANGE; dist++)
	{
		/* Bounds checking */
		if (!map_in_bounds(x, y)) break;

		/* Get the grid */
		mb_ptr = map_loc(x, y);

		/* Assume all unknown grids are walls. */
		if ((dist) && !mb_ptr->feat) break;

		/* Never pass through walls/doors */
		if (dist && borg_cave_wall_grid(mb_ptr)) break;

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

	map_block *mb_ptr;

	/* Start at the initial location */
	y = y1;
	x = x1;

	/* Simulate the spell/missile path */
	for (dist = 0; dist <= MAX_RANGE; dist++)
	{
		/* Bounds checking */
		if (!map_in_bounds(x, y)) break;

		/* Get the grid */
		mb_ptr = map_loc(x, y);

		/* Hack -- assume unknown grids are walls */
		if (dist && !mb_ptr->feat) break;

		/* Never pass through walls/doors */
		if (dist && borg_cave_wall_grid(mb_ptr)) break;

		/* Check for arrival at "final target" */
		if ((x == x2) && (y == y2)) return (TRUE);

		/* Stop at monsters */
		if (mb_ptr->monster) break;

		/* Calculate the new location */
		borgmove2(&y, &x, y1, x1, y2, x2);
	}

	/* Assume obstruction */
	return (FALSE);
}


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
