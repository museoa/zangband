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





/*
 * Init this file.
 */
void borg_init_2(void)
{
    /* Nothing */
}



#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
