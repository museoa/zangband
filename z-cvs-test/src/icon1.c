/* File: icon1.c */

/* Purpose: icon stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include <tcl.h>
#include "angband.h"
#include "tnb.h"
#include "interp.h"
#include "cmdinfo-dll.h"
#include "util-dll.h"
#include "icon.h"

#include <sys/stat.h>

extern int AngbandTk_CmdChooseFont _ANSI_ARGS_((ClientData clientData,
	 Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));

unsigned char *g_palette_rgb;
t_assign_group g_assign[ASSIGN_MAX];
t_assign g_assign_none;
t_grid *g_grid[MAX_HGT] = {0};
int g_grid_xtra_init = 0;
t_flavor *g_flavor = NULL; /* Array of flavor types */
int g_flavor_count = 0; /* Number of flavors */
Tcl_HashTable g_flavor_table; /* Map flavor name to g_flavor[] index */
t_sprite *g_sprite; /* Array of sprites */
int g_sprite_count;  /* Number of sprites */
t_alternate *g_alternate; /* Array of alternate icon info */
int g_alternate_count;  /* Number of elems in g_alternate[] array */
t_effect *g_effect; /* Array of effect icon info */
int *g_feat_lite = NULL;
int *g_background = NULL;
byte *g_feat_flag = NULL;
t_darken g_darken[3];
TintTable g_yellow;
t_assign *g_icon_map[ICON_LAYER_MAX][MAX_HGT];
t_assign *g_assignshape[GRID_SHAPE_MAX] = {NULL};
bool g_icon_map_changed = FALSE;
int g_torchlite; /* Use 4 levels of torch lite */
int *g_image_monster, *g_image_object;
bool g_daytime; /* Day or night */

cptr keyword_wall[] = { "not", "single", "ns", "we", "corner_nw",
	"corner_ne", "corner_sw", "corner_se", "tri_n", "tri_s",
	"tri_w", "tri_e", "quad", NULL };

/*
 * Calculate the light radius for this floor grid
 */
void angtk_view_floor(int y, int x, int info, int torch)
{
	/* Special lighting effects */
	if (view_special_lite)
	{
		/* Handle "blind" */
		if (p_ptr->blind)
		{
			/* Use "dark gray" */
			g_grid[y][x].dark = GRID_LITE_DARK;
		}

		/* Handle "seen" grids */
		else if (info & (CAVE_VIEW))
		{
			/* Only lit by "torch" lite */
			if (view_yellow_lite && torch)
			{
				/* Use "yellow" */
				g_grid[y][x].dark = GRID_LITE_TORCH;
			}

			else
			{
				g_grid[y][x].dark = GRID_LITE_NORMAL;
			}
		}

		/* Handle "dark" grids */
		else if (!(info & (CAVE_GLOW)))
		{
			/* Use "dark gray" */
			g_grid[y][x].dark = GRID_LITE_DARK;
		}

		/* Handle "view_bright_lite" */
		else if (view_bright_lite)
		{
			/* Use "gray" */
			g_grid[y][x].dark = GRID_LITE_DARK;
		}
	}
	else
	{
		g_grid[y][x].dark = GRID_LITE_TORCH;
	}
}

/*
 * Calculate the light radius for this wall grid
 */
void angtk_view_wall(int y, int x, int info, int torch)
{
	/* Special lighting effects (walls only) */
	if (view_granite_lite)
	{
		/* Handle "blind" */
		if (p_ptr->blind)
		{
			/* Use "dark gray" */
			g_grid[y][x].dark = GRID_LITE_DARK;
		}

		/* Handle "seen" grids */
		else if (info & (CAVE_VIEW))
		{
			if (view_yellow_lite && torch)
			{
				g_grid[y][x].dark = GRID_LITE_TORCH;
			}

			else
			{
				g_grid[y][x].dark = GRID_LITE_NORMAL;
			}
		}

		/* Handle "view_bright_lite" */
		else if (view_bright_lite)
		{
			/* Use "gray" */
			g_grid[y][x].dark = GRID_LITE_DARK;
		}
	}
	else
	{
		g_grid[y][x].dark = GRID_LITE_TORCH;
	}
}

/*
 * This routine fills the given t_grid struct with the indices of
 * any known feature, object or monster at the given cave location.
 * This routine does not consider light radius.
 */
void get_grid_info(int y, int x, t_grid *gridPtr)
{
	byte feat;
	byte info;

	s16b this_o_idx, next_o_idx;

	s16b m_idx;

	/*
	 * Hallucination: The original game assigns a random image to
	 * an object or monster, and sometimes (1 in 256) to a feature.
	 */

	gridPtr->f_idx = 0;
	gridPtr->o_idx = 0;
	gridPtr->m_idx = 0;

	/* Feature */
	feat = cave[y][x].feat;

	/* Apply "mimic" field */
	if (cave[y][x].mimic)
		feat = cave[y][x].mimic;

	/* Apply mimic field */
	feat = f_info[feat].mimic;

	/* Monster/Player */
	m_idx = cave[y][x].m_idx;

	if ((y == py) && (x == px))
		m_idx = -1;

	/* Handle "player" */
	if (m_idx < 0)
	{
		/* Remember the character index */
		gridPtr->m_idx = m_idx;

		/* Remember the feature index (for masked icon) */
		gridPtr->f_idx = feat;

		/* Done */
		return;
	}

	/* Cave flags */
	info = cave[y][x].info;

	/* Boring grids (floors, etc) */
	if (g_feat_flag[feat] & FEAT_FLAG_BORING)
	{
		/* Memorized (or seen) floor */
		if ((info & (CAVE_MARK)) ||
		    (((info & (CAVE_LITE)) ||
		      ((info & (CAVE_GLOW)) &&
		       (info & (CAVE_VIEW)))) &&
		     !p_ptr->blind))
		{
			/* Remember the feature index */
			gridPtr->f_idx = feat;
		}

		/* Unknown */
		else
		{
			/* Nothing */
		}
	}

	/* Interesting grids (non-floors) */
	else
	{
		/* Memorized grids */
		if (info & (CAVE_MARK))
		{
			/* Remember the feature index */
			gridPtr->f_idx = feat;
		}

		/* Unknown */
		else
		{
			/* Nothing */
		}
	}

	/* Objects */
	for (this_o_idx = cave[y][x].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorized objects */
		if (!o_ptr->marked) continue;
				
		/* Remember the top-most object index */
		gridPtr->o_idx = this_o_idx;

		/* Stop */
		break;
	}

	/* Monsters */
	if (m_idx > 0)
	{
		/* Visible monster */
		if (m_list[m_idx].ml)
		{
			/* Remember the monster index */
			gridPtr->m_idx = m_idx;
		}
	}
}

/*
 * This is the routine that determines the actual icon(s) used to
 * represent the given cave location. We use the global g_grid[] array
 * to first determine what monster/object/feature is visible at the
 * given location. Then we read the assignment for the specific monster/
 * object/feature from the g_assign[] array.
 */
void get_display_info(int y, int x, t_display *displayPtr)
{
	int m_idx, o_idx, f_idx;

	/* Access the global cave memory */
	t_grid *gridPtr = &g_grid[y][x];

	/* Get the darkness factor */
	int dark = gridPtr->dark;

	/* Determine if there is daylight in the town */
	int daytime = !dun_level && g_daytime;

	int layer;

	t_assign assign;
	IconSpec iconSpec;
	
	m_idx = gridPtr->m_idx;
	o_idx = gridPtr->o_idx;
	f_idx = gridPtr->f_idx;

	/* The grid is completely uninteresting */
	if (!m_idx && !o_idx && !f_idx)
	{
		/* All other values are uninitialized */
		displayPtr->blank = TRUE;
		
		/* Done */
		return;
	}

	/* */
	displayPtr->blank = FALSE;
	displayPtr->tint = NULL;
	displayPtr->anim = FALSE;

	if (m_idx || o_idx)
	{
		/* Character */
		if (m_idx == -1)
		{
			int k = 0;
	
			/*
			 * Currently only one icon is assigned to the character. We
			 * could allow different icons to be used depending on the
			 * state of the character (badly wounded, invincible, etc).
			 */
		
			assign = g_assign[ASSIGN_CHARACTER].assign[k];
		}
	
		/* Monster */
		else if (m_idx > 0)
		{
			/* Get the monster race */
			int r_idx = m_list[m_idx].r_idx;
	
			/* XXX Hack -- Hallucination */
			if (p_ptr->image)
			{
				/* Get a random monster race */
				r_idx = g_image_monster[r_idx];
			}
	
			/* Get the icon assigned to the monster race */
			assign = g_assign[ASSIGN_MONSTER].assign[r_idx];
		}
	
		/* Object */
		else if (o_idx)
		{	
			/* Get the object kind */
			int k_idx = o_list[o_idx].k_idx;
	
			/* Get the icon assigned to the object kind */
			assign = g_assign[ASSIGN_OBJECT].assign[k_idx];
		}

		/*
		 * Now we have the assignment for the character, monster, or object.
		 * The assignment may be TYPE_ALTERNATE, TYPE_FLAVOR, or
		 * TYPE_SPRITE, which we must resolve into a "real" icon type and
		 * index (for example, the current frame of a sprite).
		 *
		 * XXX TYPE_ALTERNATE is currently used only for objects and
		 * features, but feature assignments must already be resolved
		 * in set_grid_assign(). That's why TYPE_ALTERNATE is only
		 * checked for objects (see above).
		 */
	
		switch (assign.assignType)
		{
			/*
			 * TYPE_ALTERNATE assignments use one of two icons,
			 * depending on some property of the object.
			 */
			case ASSIGN_TYPE_ALTERNATE:
			{
				/* Access the alternate */
				t_alternate *alternatePtr = &g_alternate[assign.alternate.index];
		
				/* Default to the first frame */
				int index = 0;
				
				switch (alternatePtr->reason)
				{
					case REASON_NONE:
						break;
		
					case REASON_NUMBER:
						if (o_list[o_idx].number == 1) ++index;
						break;
					
					case REASON_IDENT:
						if (object_known_p(&o_list[o_idx])) ++index;
						break;
				}
		
				/* Get the type and index of the frame */
				iconSpec = alternatePtr->icon[index];
				break;
			}
	
			/* Resolve flavor */
			case ASSIGN_TYPE_FLAVOR:
			{
				/* Access the flavor */
				t_flavor *flavor = &g_flavor[assign.flavor.group];
		
				/* Get the flavor index */
				int index = flavor->sorted[assign.flavor.index];
		
				/* Get the icon */
				iconSpec = flavor->icon[index];
				break;
			}
	
			/* Resolve icon */
			case ASSIGN_TYPE_ICON:
			{
				iconSpec.type = assign.icon.type;
				iconSpec.index = assign.icon.index;
				iconSpec.ascii = assign.icon.ascii;
		
				/* XXX Hack -- Multi-hued ascii icons are animated */
				if (iconSpec.ascii != -1)
				{
					if (g_ascii[iconSpec.ascii].mode != ASCII_NORMAL)
					{
						/* This grid is animated */
						displayPtr->anim = TRUE;
					}
				}
				break;
			}
	
			/* Resolve sprite */
			case ASSIGN_TYPE_SPRITE:
			{
				/* Access the sprite */
				t_sprite *spritePtr = &g_sprite[assign.sprite.index];
			
				/* Get the current frame */
				iconSpec = spritePtr->icon[spritePtr->frame];
			
				/* This grid is animated */
				displayPtr->anim = TRUE;
			}
		}
	}

	/* No character, monster or object */
	else
	{
		iconSpec.type = ICON_TYPE_NONE;
	}

	/* Set the foreground icon */
	iconSpec.dark = 0;
	displayPtr->fg = iconSpec;
	
	/* Don't bother calculating background if foreground is not masked */
	if (g_icon_data[iconSpec.type].rle_data == NULL)
	{
		/* Other layers uninitialized */
		displayPtr->bg[ICON_LAYER_1].type = ICON_TYPE_NONE;

		/* Done */
		return;
	}

	/* The feature is not known */
	if (f_idx == 0)
	{
		/* Other layers uninitialized */
		displayPtr->bg[ICON_LAYER_1].type = ICON_TYPE_BLANK;

		/* Done */
		return;
	}

	if (g_icon_map_changed)
	{
		int y, x;
		
		for (y = 0; y < MAX_HGT; y++)
		{
			for (x = 0; x < MAX_WID; x++)
			{
				set_grid_assign(y, x);
			}
		}
		g_icon_map_changed = FALSE;
	}

	/* Option: Use 4 levels of lighting. */
	if (g_torchlite)
	{
		/* Grid is lit by the character's light source */
		if (dark == GRID_LITE_TORCH)
		{
			/* Calculate distance from py,px */
			dark = MAX(ABS(x - px), ABS(y - py)) - 1;

			/* We may have dark == -1 at py,px */
			if (dark < 0) dark = 0;

			/* Light radius > 3 not allowed */
			else if (dark > 3) dark = 3;
		}

		/* Grid isn't lit by character's light source */
		else
		{
			/* Maximum darkness */
			dark = 3;
		}
	}

	/* In the Town */
	if (daytime)
	{
		/* Use bright light */
		dark = 0;
	}

	/*
	 * Get the icon from the global icon map. The g_icon_map[]
	 * array allows us to use different icons for the same
	 * feature index. For example, doors may be vertical or
	 * horizontal.
	 */
	for (layer = 0; layer < ICON_LAYER_MAX; layer++)
	{
		assign = g_icon_map[layer][y][x];

		/* Resolve sprite */
		if (assign.assignType == ASSIGN_TYPE_SPRITE)
		{
			/* Access the sprite */
			t_sprite *spritePtr = &g_sprite[assign.sprite.index];
		
			/* Get the type and index of the current frame */
			iconSpec = spritePtr->icon[spritePtr->frame];
		
			/* This grid is animated */
			displayPtr->anim = TRUE;
		}

		/* Must be an icon */
		else
		{
			iconSpec.type = assign.icon.type;
			iconSpec.index = assign.icon.index;
			iconSpec.ascii = assign.icon.ascii;

			/* Only layer 1 is required */
			if (iconSpec.type == ICON_TYPE_NONE)
			{
				displayPtr->bg[layer] = iconSpec;
				break;
			}

			/* XXX Hack -- Multi-hued ascii icons are animated */
			if (iconSpec.ascii != -1)
			{
				if (g_ascii[iconSpec.ascii].mode != ASCII_NORMAL)
				{
					/* This grid is animated */
					displayPtr->anim = TRUE;
				}
			}
		}
#if 0
		/*
		 * Note that TYPE_ALTERNATE assignments must already have
		 * been resolved in set_grid_assign() or this will bomb.
		 * And TYPE_SPRITE assignments will bomb if lighting is
		 * FT_LIGHT_ICON and not FT_LIGHT_TINT.
		 */

		if (dark)
		{
			/* Examine the lighting mode for this feature */
			switch (g_feat_lite[f_idx])
			{
				/* Use icon series for lighting */
				case FT_LIGHT_ICON:
					
					/* Paranoia: only icons use light */
					if (assign.assignType == ASSIGN_TYPE_ICON)
					{
						if (iconSpec.type > ICON_TYPE_DEFAULT)
							iconSpec.index += dark;
					}
					break;

				/* Use tint table for lighting (slow) */
				case FT_LIGHT_TINT:
					if (g_icon_depth == 8)
						displayPtr->tint = g_darken[dark-1].table;
					break;
			}
		}
#endif

		/* A darkened copy of the icon exists, or will exist */
		if ((g_icon_data[iconSpec.type].dark_data &&
			g_icon_data[iconSpec.type].dark_data[iconSpec.index]) ||
			(g_icon_data[iconSpec.type].flags[iconSpec.index] & ICON_FLAG_DARK))
		{
			iconSpec.dark = dark;
		}
		else
		{
			iconSpec.dark = 0;
		}

		displayPtr->bg[layer] = iconSpec;
	}
}

bool is_wall(int y, int x)
{
	int f_idx = cave[y][x].feat;

	/* Apply "mimic" field */
	if (cave[y][x].mimic)
		f_idx = cave[y][x].mimic;

	/* Apply mimic field */
	f_idx = f_info[f_idx].mimic;

	if ((f_idx == FEAT_SECRET) ||
		((f_idx >= FEAT_MAGMA) && (f_idx <= FEAT_PERM_SOLID)))
	{
		return TRUE;
	}
	return FALSE;
}

bool is_door(int y, int x)
{
	int f_idx = cave[y][x].feat;

	/* Apply "mimic" field */
	if (cave[y][x].mimic)
		f_idx = cave[y][x].mimic;

	/* Apply mimic field */
	f_idx = f_info[f_idx].mimic;

	if (f_idx == FEAT_OPEN || f_idx == FEAT_BROKEN)
		return TRUE;
	if (f_idx >= FEAT_DOOR_HEAD && f_idx <= FEAT_DOOR_TAIL)
		return TRUE;
	return FALSE;	
}

int wall_shape(int y, int x, bool force)
{
	int i, j;
	bool wall[3][3];
	int n = 0, nswe = 0;
	int col0n = 0, col2n = 0;
	int row0n = 0, row2n = 0;
	int shape;

	if (!in_bounds2(y, x))
		return GRID_SHAPE_NOT;

	/* Require knowledge unless forced */
	if (!force && !(cave[y][x].info & CAVE_MARK))
		return GRID_SHAPE_NOT;

	/* Require wall or secret door */
	if (!(g_grid[y][x].xtra & GRID_XTRA_WALL))
		return GRID_SHAPE_NOT;

	for (j = 0; j < 3; j++)
	{
		int yy = y - 1 + j;

		for (i = 0; i < 3; i++)
		{
			int xx = x - 1 + i;
			bool known;

			if ((i == 1) && (j == 1))
				continue;

			if (!in_bounds2(yy, xx))
			{
				wall[j][i] = FALSE;
				continue;
			}

			known = force || ((cave[yy][xx].info & CAVE_MARK) != 0);

			wall[j][i] = known &&
				(g_grid[yy][xx].xtra & (GRID_XTRA_WALL | GRID_XTRA_DOOR));

			if (wall[j][i])
			{
				++n;
				if (!i)
					++col0n;
				else if (i == 1)
					++nswe;
				else if (i == 2)
					++col2n;

				if (!j)
					++row0n;
				else if (j == 1)
					++nswe;
				else if (j == 2)
					++row2n;
			}
		}
	}

	/* Surrounded */
	if (n == 8)
	{
		return GRID_SHAPE_SINGLE;
	}

	/* Single, or no wall to N, S, W, or E */
	if (!n || (!nswe))
	{
		/* A solitary wall */
		if (!force)
		{
			/* Get the actual shape */
			int shape = wall_shape(y, x, TRUE);

			/* Not actually single */
			if (shape != GRID_SHAPE_SINGLE)
			{
				switch (shape)
				{
					case GRID_SHAPE_NS:
					case GRID_SHAPE_WE:
						return shape;
					case GRID_SHAPE_CORNER_NW:
					case GRID_SHAPE_CORNER_NE:
					case GRID_SHAPE_CORNER_SW:
					case GRID_SHAPE_CORNER_SE:
						return shape;
					case GRID_SHAPE_TRI_N:
					case GRID_SHAPE_TRI_S:
						return GRID_SHAPE_WE;
					case GRID_SHAPE_TRI_W:
					case GRID_SHAPE_TRI_E:
						return GRID_SHAPE_NS;
					case GRID_SHAPE_QUAD:
						return GRID_SHAPE_NS; /* FIXME: corner instead? */
				}
			}
		}

		return GRID_SHAPE_SINGLE;
	}

#if 1

	shape = GRID_SHAPE_NOT;

	if (nswe == 4)
	{
		shape = GRID_SHAPE_QUAD;

		if (n < 6) return shape;

		if (n == 6)
		{
			if (row0n == 3)
				shape = GRID_SHAPE_TRI_S;
			else if (row2n == 3)
				shape = GRID_SHAPE_TRI_N;
			else if (col0n == 3)
				shape = GRID_SHAPE_TRI_E;
			else if (col2n == 3)
				shape = GRID_SHAPE_TRI_W;

			return shape;
		}

		/* 7 grids, so must be corner */
		if (!wall[0][0])
			shape = GRID_SHAPE_CORNER_SE;
		else if (!wall[2][0])
			shape = GRID_SHAPE_CORNER_NE;
		else if (!wall[0][2])
			shape = GRID_SHAPE_CORNER_SW;
		else
			shape = GRID_SHAPE_CORNER_NW;

		return shape;
	}

	if (nswe == 3)
	{
		/* Walls to N and S */
		if (wall[0][1] && wall[2][1])
		{
			if (col0n == 3 || col2n == 3)
				shape = GRID_SHAPE_NS;
			else if (wall[1][0])
				shape = GRID_SHAPE_TRI_W;
			else
				shape = GRID_SHAPE_TRI_E;
		}

		/* Walls to W and E */
		else
		{
			if (row0n == 3 || row2n == 3)
				shape = GRID_SHAPE_WE;
			else if (wall[0][1])
				shape = GRID_SHAPE_TRI_N;
			else
				shape = GRID_SHAPE_TRI_S;
		}

		return shape;
	}

	if (nswe == 2)
	{
		if (wall[0][1] && wall[2][1])
			shape = GRID_SHAPE_NS;
		if (wall[1][0] && wall[1][2])
			shape = GRID_SHAPE_WE;

		if (wall[0][1] && wall[1][0])
			shape = GRID_SHAPE_CORNER_SE;
		if (wall[0][1] && wall[1][2])
			shape = GRID_SHAPE_CORNER_SW;
		if (wall[2][1] && wall[1][0])
			shape = GRID_SHAPE_CORNER_NE;
		if (wall[2][1] && wall[1][2])
			shape = GRID_SHAPE_CORNER_NW;

		return shape;
	}

	if (nswe == 1)
	{
		if (wall[0][1] || wall[2][1])
			shape = GRID_SHAPE_NS;
		else
			shape = GRID_SHAPE_WE;

		return shape;
	}

	/* Should never happen */
	return shape;

#else /* not 1 */

	/* One surrounding grid missing */
	if (n == 7)
	{
		/* Not missing N, S, W or E */
		if (nswe == 4)
		{
			/* Corner */
			if (!wall[0][0])
				return GRID_SHAPE_CORNER_SE;
			if (!wall[2][0])
				return GRID_SHAPE_CORNER_NE;
			if (!wall[0][2])
				return GRID_SHAPE_CORNER_SW;
			return GRID_SHAPE_CORNER_NW;
		}

		/* Flat */
		if (!wall[0][1] || !wall[2][1])
			return GRID_SHAPE_WE;

		return GRID_SHAPE_NS;
	}

	/* Two missing grids */
	if (n == 6)
	{
		/* 2 corners missing, so TRI or QUAD */
		if (nswe == 4)
		{
			if (row0n == 3)
				return GRID_SHAPE_TRI_S;
			if (row2n == 3)
				return GRID_SHAPE_TRI_N;
			if (col0n == 3)
				return GRID_SHAPE_TRI_E;
			if (col2n == 3)
				return GRID_SHAPE_TRI_W;

			/* Diagonally oposite corners missing */
			return GRID_SHAPE_QUAD;
		}

		if (nswe == 2)
		{
			if (wall[0][1])
				return GRID_SHAPE_NS;
			return GRID_SHAPE_WE;
		}

		/* 2 in same row/col missing, so NS or WE */
		if (row0n == 1 || row2n == 1)
			return GRID_SHAPE_WE;
		if (col0n == 1 || col2n == 1)
			return GRID_SHAPE_NS;

		/* 1 in different row/col missing, so TRI */
		if (row0n == 3 || row2n == 3)
			return wall[1][0] ? GRID_SHAPE_TRI_W : GRID_SHAPE_TRI_E;
		return wall[0][1] ? GRID_SHAPE_TRI_N : GRID_SHAPE_TRI_S;
	}

	if (n == 5)
	{
		/* Three corners missing */
		if (nswe == 4)
			return GRID_SHAPE_QUAD;

		if (!row0n || !row2n)
			return GRID_SHAPE_WE;
		if (!col0n || !col2n)
			return GRID_SHAPE_NS;
	}

	if (nswe == 4)
	{
		return GRID_SHAPE_QUAD;
	}

	if (nswe == 3)
	{
		if (wall[0][1] && wall[2][1])
			return wall[1][0] ? GRID_SHAPE_TRI_W : GRID_SHAPE_TRI_E;
		return wall[0][1] ? GRID_SHAPE_TRI_N : GRID_SHAPE_TRI_S;
	}

	if (n == 3)
	{
		/* Sticks out from wall */
		if (row0n == 3 || row2n == 3)
			return GRID_SHAPE_NS;
		if (col0n == 3 || col2n == 3)
			return GRID_SHAPE_WE;
	}

	if (nswe == 2)
	{
		if (wall[0][1] && wall[2][1])
			return GRID_SHAPE_NS;
		if (wall[1][0] && wall[1][2])
			return GRID_SHAPE_WE;

		if (wall[0][1] && wall[1][0])
			return GRID_SHAPE_CORNER_SE;
		if (wall[0][1] && wall[1][2])
			return GRID_SHAPE_CORNER_SW;
		if (wall[2][1] && wall[1][0])
			return GRID_SHAPE_CORNER_NE;
		/* if (wall[2][1] && wall[1][2]) */
			return GRID_SHAPE_CORNER_NW;
	}

	if (nswe == 1)
	{
		if (wall[0][1] || wall[2][1])
			return GRID_SHAPE_NS;
		return GRID_SHAPE_WE;
	}

	/* Only 2 touching, neither are N, S, W or E */
	if (n == 2)
	{
		if (wall[0][1] || wall[2][1])
			return GRID_SHAPE_NS;
		return GRID_SHAPE_WE;
	}

	/* Only 1 touching */
	if (n == 1)
	{
		if (wall[0][1] || wall[2][1])
			return GRID_SHAPE_NS;
		return GRID_SHAPE_WE;
	}

	return GRID_SHAPE_SINGLE;

#endif /* not 1 */
}

/*
 * Called when knowledge about many grids changes.
 */
void angtk_cave_changed(void)
{
	int y, x;

	/* Do not use g_cave_hgt/_wid */

	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			get_grid_info(y, x, &g_grid[y][x]);
			g_grid[y][x].shape = wall_shape(y, x, FALSE);
			set_grid_assign(y, x);
			map_symbol_set(y, x);
		}
	}
}

/*
 * Updates knowledge about a grid, including shape.
 * Called when a dungeon feature changes.
 */
void angtk_feat_changed(int y, int x)
{
	int d, shape;

	/* Forget walls and doors */
	g_grid[y][x].xtra &= ~(GRID_XTRA_DOOR | GRID_XTRA_WALL);

	/* Forget shape */
	g_grid[y][x].shape = 0;

	/* A wall (or secret door) */
	if (is_wall(y, x))
	{
		/* Note wall */
		g_grid[y][x].xtra |= GRID_XTRA_WALL;

		/* Get shape */
		g_grid[y][x].shape = wall_shape(y, x, FALSE);
	}

	/* A door */
	else if (is_door(y, x))
	{
		/* Note door */
		g_grid[y][x].xtra |= GRID_XTRA_DOOR;
	}

	set_grid_assign(y, x);

	for (d = 0; d < 8; d++)
	{
		int yy = y + ddy_ddd[d];
		int xx = x + ddx_ddd[d];
		
		if (in_bounds2(yy, xx) && (g_grid[yy][xx].xtra & GRID_XTRA_WALL))
		{
			shape = wall_shape(yy, xx, FALSE);
			if (shape != g_grid[yy][xx].shape)
			{
				g_grid[yy][xx].shape = shape;
				set_grid_assign(yy, xx);
			}
		}
	}
}

/*
 * Sets the shape for the given grid and surrounding grids.
 * Called when a grid becomes known.
 */
void angtk_feat_known(int y, int x)
{
	int d, shape;

	if (!(g_grid[y][x].xtra & (GRID_XTRA_WALL | GRID_XTRA_DOOR)))
		return;

	if (g_grid[y][x].xtra & GRID_XTRA_WALL)
	{
		shape = wall_shape(y, x, FALSE);
		if (shape != g_grid[y][x].shape)
		{
			g_grid[y][x].shape = shape;
			set_grid_assign(y, x);

			for (d = 0; d < 8; d++)
			{
				int yy = y + ddy_ddd[d];
				int xx = x + ddx_ddd[d];
				
				if (in_bounds2(yy, xx) && (g_grid[yy][xx].xtra & GRID_XTRA_WALL))
				{
					shape = wall_shape(yy, xx, FALSE);
					if (shape != g_grid[yy][xx].shape)
					{
						g_grid[yy][xx].shape = shape;
						set_grid_assign(yy, xx);
					}
				}
			}
		}
	}
}

/*
 * This routine determines the icon to use for the given cave
 * location. It is called after the dungeon is created or loaded
 * from the savefile, and whenever a feature changes. It handles
 * the possible TYPE_ALTERNATE assignments used to display doors.
 * It handles any special vault icons as well.
 */
void set_grid_assign(int y, int x)
{
	int feat = cave[y][x].feat;
	t_assign assign, assignArray[ICON_LAYER_MAX];
	IconSpec iconSpec;
	int layer, shape;

	/* The dungeon isn't ready yet */
	if (!character_dungeon) return;

	/* Paranoia */
	if (g_icon_map[ICON_LAYER_1][0] == NULL) return;

	/* Apply "mimic" field */
	if (cave[y][x].mimic)
		feat = cave[y][x].mimic;

	/* Apply mimic field */
	feat = f_info[feat].mimic;

g_grid[y][x].xtra &= ~0x0001;

	/* Get the assignment for this feature */
	assign = g_assign[ASSIGN_FEATURE].assign[feat];

	/* Get the shape of this grid */
	shape = g_grid[y][x].shape;

	/* If a door, set shape now */
	if (is_door(y, x)) /* XTRA_DOOR */
	{
		if (g_grid[y][x].xtra & GRID_XTRA_ISVERT)
			shape = GRID_SHAPE_NS;
		else
			shape = GRID_SHAPE_WE;
	}

	/* A valid shape */
	if ((shape > 0) && (shape < GRID_SHAPE_MAX))
	{
		t_assign assign2;

		/* Get the assignment for this shape */
		assign2 = g_assignshape[shape][feat];

		/* This is not a TYPE_DEFAULT icon */
		if ((assign2.assignType != ASSIGN_TYPE_ICON) ||
			(assign2.icon.type != ICON_TYPE_DEFAULT))
		{
			/* Use the shape icon */
			assign = assign2;
		}

		/*
		 * XXX Hack -- Remember if there is a second shape assignment.
		 * The second assignment uses an "unknown" floor.
		 */
		assign2 = g_assignshape[shape][max_f_idx + feat];
		if ((assign2.assignType != ASSIGN_TYPE_ICON) ||
			(assign2.icon.type != ICON_TYPE_DEFAULT))
		{
			/* Use the shape icon */
			g_grid[y][x].xtra |= 0x0001;
		}
	}

	if (assign.assignType == ASSIGN_TYPE_ALTERNATE)
	{
		/* This is a door */
		if ((feat == FEAT_OPEN) || (feat == FEAT_BROKEN) ||
			((feat >= FEAT_DOOR_HEAD) && (feat <= FEAT_DOOR_TAIL)))
		{
			/* The reason must be REASON_FEATURE */
			t_alternate *alternatePtr = &g_alternate[assign.alternate.index];
			int vert = (g_grid[y][x].xtra & GRID_XTRA_ISVERT) != 0;

			/* Index 0 is horizontal door, 1 is vertical door */
			iconSpec = alternatePtr->icon[vert];

			assign.assignType = ASSIGN_TYPE_ICON;
			assign.icon.type = iconSpec.type;
			assign.icon.index = iconSpec.index;
			assign.icon.ascii = iconSpec.ascii;
		}
	
		/* This is a granite wall, or a magma/quartz stream */
		/* Note: INNER/OUTER/SOLID granite mimics EXTRA */
		else if ((feat == FEAT_WALL_EXTRA) || (feat == FEAT_MAGMA) ||
			(feat == FEAT_QUARTZ))
		{
			/* The reason must be REASON_FEATURE */
			t_alternate *alternatePtr = &g_alternate[assign.alternate.index];
	
			/* Index 0 is normal granite wall, 1 is pillar */
			iconSpec = alternatePtr->icon[0];

			assign.assignType = ASSIGN_TYPE_ICON;
			assign.icon.type = iconSpec.type;
			assign.icon.index = iconSpec.index;
			assign.icon.ascii = iconSpec.ascii;
		}
	}

	/* Remember the icon in the global icon map */
	g_icon_map[ICON_LAYER_1][y][x] = assign;

	layer = ICON_LAYER_2;

	if (feat != g_background[feat])
	{
		/* Swap foreground & background */
		g_icon_map[ICON_LAYER_2][y][x] = assign;

		feat = g_background[feat];
		assign = g_assign[ASSIGN_FEATURE].assign[feat];

		if (assign.assignType == ASSIGN_TYPE_ALTERNATE)
		{
			/* This is a door */
			if ((feat == FEAT_OPEN) || (feat == FEAT_BROKEN) ||
				((feat >= FEAT_DOOR_HEAD) && (feat <= FEAT_DOOR_TAIL)))
			{
				/* The reason must be REASON_FEATURE */
				t_alternate *alternatePtr = &g_alternate[assign.alternate.index];
				int vert = (g_grid[y][x].xtra & GRID_XTRA_ISVERT) != 0;
	
				/* Index 0 is horizontal door, 1 is vertical door */
				iconSpec = alternatePtr->icon[vert];

				assign.assignType = ASSIGN_TYPE_ICON;
				assign.icon.type = iconSpec.type;
				assign.icon.index = iconSpec.index;
				assign.icon.ascii = iconSpec.ascii;
			}
		
			/* This is an "inner" granite wall, or a magma/quartz stream */
			else if ((feat == FEAT_WALL_INNER) || (feat == FEAT_MAGMA) ||
				(feat == FEAT_QUARTZ))
			{
				/* The reason must be REASON_FEATURE */
				t_alternate *alternatePtr = &g_alternate[assign.alternate.index];
		
				/* Index 0 is normal granite wall, 1 is pillar */
				iconSpec = alternatePtr->icon[0];

				assign.assignType = ASSIGN_TYPE_ICON;
				assign.icon.type = iconSpec.type;
				assign.icon.index = iconSpec.index;
				assign.icon.ascii = iconSpec.ascii;
			}
		}

		/* Swap foreground & background */
		g_icon_map[ICON_LAYER_1][y][x] = assign;

		/* Wipe layers 3 & 4 */
		layer = ICON_LAYER_3;
	}

	/* Wipe the remaining layers */
	for (; layer < ICON_LAYER_MAX; layer++)
	{
		g_icon_map[layer][y][x] = g_assign_none;
	}
}

/*
 * Determine the icon type/index of a real icon type from the given
 * icon type/index. This routine is used to get the actual frame of
 * a sprite for example. Usually the given icon type/index is returned.
 */
void FinalIcon(IconSpec *iconOut, t_assign *assignPtr, int hack, object_type *o_ptr)
{
	switch (assignPtr->assignType)
	{
		case ASSIGN_TYPE_ALTERNATE:
		{
			t_alternate *alternatePtr = &g_alternate[assignPtr->alternate.index];
			int index = 1;

			/* An object */
			if (o_ptr)
			{
				switch (alternatePtr->reason)
				{
					case REASON_NUMBER:
						if (o_ptr->number != 1) index = 0;
						break;
					
					case REASON_IDENT:
						if (!object_known_p(o_ptr)) index = 0;
						break;
				}
			}

			/* XXX Hack -- Vault wants first icon */
			else if (hack == 1)
			{
				index = 0;
			}
			(*iconOut) = alternatePtr->icon[index];
#if 0
			iconOut->type = alternatePtr->icon[1].type;
			iconOut->index = alternatePtr->icon[1].index;
			iconOut->ascii = alternatePtr->icon[1].ascii;
#endif
			break;
		}

		case ASSIGN_TYPE_FLAVOR:
		{
			t_flavor *flavorPtr = &g_flavor[assignPtr->flavor.group];
			int index = flavorPtr->sorted[assignPtr->flavor.index];
			(*iconOut) = flavorPtr->icon[index];
#if 0
			iconOut->type = flavorPtr->icon[index].type;
			iconOut->index = flavorPtr->icon[index].index;
			iconOut->ascii = flavorPtr->icon[index].ascii;
#endif
			break;
		}

		case ASSIGN_TYPE_ICON:
			iconOut->type = assignPtr->icon.type;
			iconOut->index = assignPtr->icon.index;
			iconOut->ascii = assignPtr->icon.ascii;
			break;

		case ASSIGN_TYPE_SPRITE:
		{
			t_sprite *spritePtr = &g_sprite[assignPtr->sprite.index];
			(*iconOut) = spritePtr->icon[spritePtr->frame];
#if 0
			iconOut->type = spritePtr->icon[spritePtr->frame].type;
			iconOut->index = spritePtr->icon[spritePtr->frame].index;
			iconOut->ascii = spritePtr->icon[spritePtr->frame].ascii;
#endif
			break;
		}
	}
}

int read_dark_file(char *fileName)
{
	FILE *fp;
	int count, table_count, index[16], i;
	char buf[80];

	if ((fp = fopen(fileName, "r")) == NULL)
	{
		return TCL_ERROR;
	}

	count = 0;
	table_count = 0;
	
	/* Parse the file */
	while (!feof(fp))
	{
		/* Read a line */
		if (!fgets(buf, 80, fp)) continue;

		/* Skip comments */
		if (buf[0] == '#') continue;

		/* Scan 16 values */
		if (sscanf(buf, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",
			&index[0], &index[1], &index[2], &index[3], &index[4],
			&index[5], &index[6], &index[7], &index[8], &index[9],
			&index[10], &index[11], &index[12], &index[13], &index[14],
			&index[15]) != 16) continue;

		/* Remember 16 values */
		for (i = 0; i < 16; i++)
		{
			g_darken[table_count].table[count++] = index[i];
		}
		
		/* Next table */
		if (count == 256)
		{
			table_count++;
			count = 0;
		}

		/* Error */
		if (table_count > 3) break;
	}

	/* Close the file */
	fclose(fp);

	if (table_count != 3) return TCL_ERROR;
	
	/* Success */
	return TCL_OK;
}

void init_palette(void)
{
	char path[1024], path2[1024];
	int i;

	/*
	 * The colors in this 256-color palette are indexed by each byte
	 * of icon data.
	 */
	path_build(path2, 1024, ANGBAND_DIR_TK, "config");
	path_build(path, 1024, path2, "palette_256");

	if (Palette_Init(g_interp, path) != TCL_OK)
		quit(Tcl_GetStringResult(g_interp));

	g_palette_rgb = Palette_GetRGB();

	/*
	 * Tint table for light effect.
	 */
	for (i = 0; i < 3; i++)
	{
		g_darken[i].brightness = 0;
		g_darken[i].contrast = 0;
		g_darken[i].gamma = 1.0;
		Colormap_One2OneTable(g_darken[i].table);
	}
	path_build(path2, 1024, ANGBAND_DIR_TK, "config");
	path_build(path, 1024, path2, "dark");
	if (read_dark_file(path) != TCL_OK)
	{
		quit_fmt("error reading \"%s\"", path);
	}
}

/* 
 * Is the door horizontal or vertical?
 * Return 0 for horizontal, 1 for vertical
 */
bool door_vertical(int y, int x)
{
	bool wall_left, wall_right, wall_above, wall_below;
	bool door_right, door_left;
	int f_left, f_right;
	
	wall_left = !cave_floor_bold(y, x-1);
	wall_right = !cave_floor_bold(y, x+1);
	
	wall_above = !cave_floor_bold(y-1, x);
	wall_below = !cave_floor_bold(y+1, x);
	
	/* Walls on both horizontal sides */
	if (wall_left && wall_right) return (0);
	
	/* Walls on both vertical sides */
	if (wall_above && wall_below) return (1);
	
	/* Check for doors on either horizontal side */
	f_left = cave[y][x - 1].feat;
	f_right = cave[y][x + 1].feat;
	
	/* Note we also check secret doors */
	door_left = (f_left == FEAT_OPEN) || (f_left == FEAT_BROKEN) ||
		(f_left >= FEAT_DOOR_HEAD && f_left <= FEAT_SECRET);
	door_right = (f_right == FEAT_OPEN) || (f_right == FEAT_BROKEN) ||
		(f_right >= FEAT_DOOR_HEAD && f_right <= FEAT_SECRET);

	/* Doors on left and right */
	if (door_left && door_right) return (0);
	
	/* Door on one side and wall on the other side */
	if ((door_left && wall_right) || (door_right && wall_left)) return (0);
	
	/* Default to vertical door (handles stacked vertical doors too!) */
	return (1);
}

IconPtr SetIconBitsRLE(IconPtr bg, IconPtr fg, TintTable t, IconPtr b)
{
	IconPtr dst = b;
	int i;

	/* Tint */
	if (t)
	{
		for (i = 0; i < ICON_LENGTH; i++)
		{
			*dst++ = *(t + *bg++);
		}

		/* Return the address of the buffer we wrote into */
		return b;
	}

	/*
	 * In the simplest case (non-tinted), just return
	 * the given address of the icon data.
	 */
	return bg;
}

/*
 * bg -- background bits
 * fg -- foreground bits, or NULL
 * mk -- mask bits for fg, or also NULL
 * t  -- tint table, or NULL
 * b  -- result bits
 *
 * The tint could be applied to the fg + bg, or to the bg only. Since
 * tint tables are currently only used for lighting purposes, we tint
 * the background but not the foreground.
 *
 * This is not quite right. We should be able to tint the foreground
 * and/or the background separately. For example, if Rubble is masked,
 * the foreground should be tinted, and the background may or may not
 * be tinted (depending on whether the floor is tinted).
 */
IconPtr SetIconBits(IconPtr bg, IconPtr fg, IconPtr mk, TintTable t, IconPtr b)
{
	int i;

	/* Masked */
	if (fg)
	{
		/* Tint */
		if (t)
		{
			for (i = 0; i < ICON_LENGTH; i++)
			{
#if 0 /* pixel = tint(fg + bg) */

				*b++ = *(t + ((*bg++ & *mk++) | *fg++));
				
#else /* pixel = fg + tint(bg) */

				*b++ = (*(t + *bg++) & *mk++) | *fg++;
#endif
			}
		}

		/* No tint */
		else
		{
			for (i = 0; i < ICON_LENGTH; i++)
			{
				*b++ = (*bg++ & *mk++) | *fg++;
			}
		}
	}

	/* Not masked */
	else
	{
		/* Tint */
		if (t)
		{
			for (i = 0; i < ICON_LENGTH; i++)
			{
				*b++ = *(t + *bg++);
			}
		}

		/* No tint */
		else
		{
			/*
			 * In the simplest case (non-masked, non-tinted), just return
			 * the given address of the icon data.
			 */
			return bg;
		}
	}

	/* Return the address of the buffer we wrote into */
	return b - ICON_LENGTH;
}

/* Return the number of milliseconds */
unsigned long Milliseconds(void)
{
#ifdef PLATFORM_WIN
	return GetTickCount();
#endif
#ifdef PLATFORM_X11
	return TclpGetClicks() / 1000;
#endif
}

int update_sprites(void)
{
	static unsigned long last_update = 0;
	static unsigned long last_call = 0;
	unsigned long tick_count, ticks;
	int i, update = 0;

	/* Get the system tick count */
	tick_count = Milliseconds();

	/* Don't update twice the same tick (not bloody likely) */
	if (tick_count == last_call) return 0;

	/* Remember the tick count */
	last_call = tick_count;

	/* Calculate the elapsed ticks */
	ticks = MIN(tick_count - last_update, 1000);

	/* No time has passed (yeah, right) */
	if (ticks == 0) return 0;

	/* Remember the tick count */
	last_update = tick_count;

	/* Check each sprite */
	for (i = 0; i < g_sprite_count; ++i)
	{
		/* Access the sprite */
		t_sprite *sprite = &g_sprite[i];

		/* Get the current frame */
		int frame = sprite->frame;

		/* Get the number of frames */
		int count = sprite->count;

		/* -1 backwards, 0 always forwards, 1 forwards */
		int reverse = sprite->reverse;

		/* Paranoia: require 2 frames */
		if (count < 2) continue;

		/* Another tick(s) added */
		sprite->ticks += ticks;

		/* The frame delay has been exceeded */
		if (sprite->ticks >= sprite->speed)
		{
			/* Reset tick counter */
			sprite->ticks = 0;
			
			/* This sprite always goes from first to last frame */
			if (reverse == 0)
			{
				/* Increase frame count. */
				++frame;
				
				/* Wrap around if needed */
				if (frame >= count) frame = 0;
			}
		
			/* Cycle forwards */
			else if (reverse > 0)
			{
				/* Increase frame count. */
				++frame;

				if (frame >= count)
				{
					/* Requires minimum 2 icons */
					frame = count - 2;
					
					/* Cycle in reverse now */
					reverse = -1;
				}
			}
		
			/* Cycle backwards */
			else
			{
				/* Note unsigned values */
				if (frame == 0)
				{
					/* Requires minimum 2 icons */
					frame = 1;
					
					/* Cycle forwards now */
					reverse = +1;
				}
				
				else
				{
					/* Decrease frame count. */
					--frame;
				}
			}
			
			/* Remember the current frame */
			sprite->frame = frame;

			/* Remember reverse status */
			sprite->reverse = reverse;
			
			/* Note it needs redrawing */
			sprite->changed = TRUE;

			/* A sprite changed */
			update = 1;
		}
		
		/* The sprite is unchanged */
		else
		{
			/* No redrawing needed */
			sprite->changed = FALSE;
		}
	}

	/* Increment the ascii-type frame delay */
	g_ascii_ticks += ticks;

	/* The ascii-type frame delay was exceeded */
	if (g_ascii_ticks >= g_ascii_delay)
	{
		/* ASCII_ATTR_MULTI uses one of 16 standard colors */
		g_ascii_multi = (g_ascii_multi + 1) % 16;

		/* ASCII_SHAPECHANGER uses random character */
		/* XXX This should be a valid monster/object char only */
		g_ascii_char = rand_int(126 - 32 + 1); /* printable only */

		/* Clear the tick counter */
		g_ascii_ticks = 0;

		/* We need to redraw */
		return TRUE;
	}

	return update;
}

/*
 * Return TRUE if the given icon type/index can be "animated"
 */
bool is_sprite(t_assign *assignPtr)
{
	/* Sprites are animated */
	if (assignPtr->assignType == ASSIGN_TYPE_SPRITE) return TRUE;

	/* */
	if (assignPtr->assignType != ASSIGN_TYPE_ICON) return FALSE;

	/* XXX Hack -- Multi-hued ascii icons are animated */
	if (assignPtr->icon.ascii != -1)
	{
		if (g_ascii[assignPtr->icon.ascii].mode != ASCII_NORMAL)
		{
			return TRUE;
		}
	}

	/* Nope */
	return FALSE;
}

/*
 * Recalculate the indices used for monsters and objects during
 * hallucination.
 */
void angtk_image_reset(void)
{
	int i;

	/* Randomize monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		int r_idx;
		monster_race *r_ptr;

		/* Infinite loop */
		while (1)
		{
			/* Pick a random non-unique */
			r_idx = randint(max_r_idx - 1);

			/* Access the monster race */
			r_ptr = &r_info[r_idx];

			/* Require real race */
			if (!r_ptr->name) continue;

			/* Skip uniques */
			if (r_ptr->flags1 & RF1_UNIQUE) continue;

			/* Stop */
			break;
		}

		/* Remember the monster race */
		g_image_monster[i] = r_idx;
	}

	/* Randomize objects */
	for (i = 1; i < max_k_idx; i++)
	{
		int k_idx;
		object_kind *k_ptr;

		/* Infinite loop */
		while (1)
		{
			/* Pick a random object kind */
			k_idx = randint(max_k_idx - 1);

			/* Access the object kind */
			k_ptr = &k_info[k_idx];

			/* Require real kind */
			if (!k_ptr->name) continue;

			/* Skip artifacts */
			if (k_ptr->flags3 & (TR3_INSTA_ART)) continue;
			
			/* Stop */
			break;
		}

		/* Remember the object kind */
		g_image_object[i] = k_idx;
	}
}

/* Option: Animation */
bool allow_animation = TRUE;

/* Debugging: Calculate average time between animation updates */
#define TIMER_STATSxxx

#ifdef TIMER_STATS

static unsigned long _sum = 0, _count = 0, _last = 0;

/*
 * Debug command. You should find a cave location with some animation
 * going on, call "timer reset", wait a few seconds, then call
 * "timer average" to get the averate time between updates.
 *
 * Usage:
 *    timer average -- current delay between animation updates in ms
 *    timer reset   -- zero the timer statistics
 */
static int
objcmd_timer(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	static char *cmdOption[] = {"average", "reset", NULL};
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;
	
	/* Required number of arguments */
    if (objc < 2)
    {
		Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
		return TCL_ERROR;
    }

	/* Get requested option */
    if (Tcl_GetIndexFromObj(interp, objv[1], cmdOption, "option", 0, 
		&index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* average */
			if (_count == 0) _count = 1;
			Tcl_SetIntObj(resultPtr, _sum / _count);
			break;

		case 1: /* reset */
			_last = Milliseconds();
			_sum = _count = 0;
			break;
	}

	/* Success */
	return TCL_OK;
}

#endif /* TIMER_STATS */

/*
 * Animation timer (3 implementations):
 *
 * The first method uses Tcl_CreateTimerHandler() to repeatedly reschedule
 * our timer callback. The average delay between calls is about 57ms on my
 * machine.
 *
 * The second method calls the Win32 SetTimer() call with a custom window
 * class. The average delay is the same as the first method, unless there
 * is rapid mouse motion, in which case it drops to about 22ms.
 *
 * The third method uses SetTimer() without a custom window class. But
 * I'm not sure if it executes asynchronously, so I won't use it.
 *
 * Note that the last two implementations may no longer compile properly.
 */

/* Number of milliseconds between timer callback */
#define TIMER_TICKS 10

static Tcl_TimerToken g_timer_token = NULL;

/*
 * Tcl_CreateTimerHandler() callback
 */
static void AngbandTimerProc(ClientData clientData)
{
	/* Reschedule the animation timer */
	g_timer_token = Tcl_CreateTimerHandler(TIMER_TICKS, AngbandTimerProc, 0);

	/* No animation while repeating a command */
	if (running || command_rep || resting) return;

#ifdef TIMER_STATS

	if (_last == 0)
		_last = Milliseconds();
	else
	{
		_sum += Milliseconds() - _last;
		_last = Milliseconds();
		_count++;
	}
	
#endif /* TIMER_STATS */

	/* Perform animation */
	angtk_idle();
}

/*
 * One-time initialization for the animation timer
 */
static void init_timer(void)
{
#ifdef TIMER_STATS
	Tcl_CreateObjCommand(g_interp, "timer", objcmd_timer, NULL, NULL);
#endif /* TIMER_STATS */
}

/*
 * One-time cleanup for the animation timer
 */
static void free_timer(void)
{
	/* The timer was started */
	if (g_timer_token != NULL)
	{
		/* Cancel the timer */
		Tcl_DeleteTimerHandler(g_timer_token);

		/* Forget the timer */
		g_timer_token = NULL;
	}
}

/*
 * Start the animation timer
 */
void angtk_start_timer(void)
{
	/* The timer is already sheduled */
	if (g_timer_token != NULL) return;

	/* Shedule the timer */
	g_timer_token = Tcl_CreateTimerHandler(TIMER_TICKS, AngbandTimerProc, 0);
}

/*
 * Stop the animation timer
 */
void angtk_stop_timer(void)
{
	/* The timer is scheduled */
	if (g_timer_token != NULL)
	{
		/* Cancel the timer */
		Tcl_DeleteTimerHandler(g_timer_token);

		/* Forget the timer */
		g_timer_token = NULL;
	}

	/*
	 * XXX Hack -- When animation is disabled, use TERM_VIOLET for
	 * all multi-hued ascii type icons.
	 */
	g_ascii_multi = TERM_VIOLET;
}

void angtk_flavor_swap(int n, int a, int b)
{
	int tmp;

	tmp = g_flavor[n].sorted[a];
	g_flavor[n].sorted[a] = g_flavor[n].sorted[b];
	g_flavor[n].sorted[b] = tmp;
}

static int init_flavor_aux(int n, char *desc, int tval, int count,
	byte *color)
{
	Tcl_HashEntry *hPtr;
	t_flavor flavor;
	int i, dummy;
	IconSpec defaultIcon = {ICON_TYPE_DEFAULT, 0, -1};

	flavor.desc = (char *) string_make(desc);
	flavor.tval = tval;
	flavor.count = count;
	C_MAKE(flavor.sorted, count, int);
	C_MAKE(flavor.icon, count, IconSpec);
	C_MAKE(flavor.color, count, byte);

	for (i = 0; i < count; i++)
	{
		flavor.sorted[i] = i;
		flavor.color[i] = color[i];
		flavor.icon[i] = defaultIcon;
	}

	hPtr = Tcl_CreateHashEntry(&g_flavor_table, desc, &dummy);
	Tcl_SetHashValue(hPtr, (ClientData) n);
	g_flavor[n++] = flavor;

	return n;
}

void angtk_flavor_init(int *max, byte **attr)
{
	int n = 0;

	C_MAKE(g_flavor, FLAVOR_MAX, t_flavor);
	g_flavor_count = FLAVOR_MAX;

	/*
	 * This table maps a flavor name to an index in the g_flavor[] array.
	 */
	Tcl_InitHashTable(&g_flavor_table, TCL_STRING_KEYS);

	/* Note: indexes are hard-coded by flavor_init() */
	n = init_flavor_aux(n, "amulet", TV_AMULET, max[n], attr[n]);
	n = init_flavor_aux(n, "mushroom", TV_FOOD,  max[n], attr[n]);
	n = init_flavor_aux(n, "potion", TV_POTION,  max[n], attr[n]);
	n = init_flavor_aux(n, "ring", TV_RING,  max[n], attr[n]);
	n = init_flavor_aux(n, "rod", TV_ROD,  max[n], attr[n]);
	n = init_flavor_aux(n, "staff", TV_STAFF,  max[n], attr[n]);
	(void) init_flavor_aux(n, "wand", TV_WAND,  max[n], attr[n]);
}

/*
 * Initialize the icon environment. This should be called once with
 * the desired dimensions of the icons to use (16x16, 24x24 or 32x32).
 */
void init_icons(int size, int depth)
{
	int i, n, y, x, y2, x2;
	t_assign assign;
	t_icon_data icon_data, *icon_data_ptr = &icon_data;
	IconSpec iconDefault = {ICON_TYPE_DEFAULT, 0, -1};
	unsigned char *rgb = Colormap_GetRGB();

	/* Initialize the Icon library */
	if (Icon_Init(g_interp, size, depth) != TCL_OK)
	{
		quit(Tcl_GetStringFromObj(Tcl_GetObjResult(g_interp), NULL));
	}

	/*
	 * The TYPE_NONE/"none" type icon is a single masked icon with an empty
	 * mask. It is suitable for equipment displays when no item is present
	 * in a slot.
	 */
	icon_data_ptr->desc = "none";
	icon_data_ptr->icon_count = 1;
	icon_data_ptr->icon_data = (IconPtr) Tcl_AllocDebug(ICON_LENGTH);
	icon_data_ptr->char_table = NULL;
	icon_data_ptr->font = NULL;
	for (i = 0; i < ICON_LENGTH; i++)
	{
		icon_data_ptr->icon_data[i] = 0x00;
	}

	icon_data_ptr->dynamic = FALSE;
	icon_data_ptr->depth = g_icon_depth;
	icon_data_ptr->bypp = g_pixel_size;
	icon_data_ptr->width = g_icon_size;
	icon_data_ptr->height = g_icon_size;
	icon_data_ptr->pitch = g_icon_size * g_pixel_size;
	icon_data_ptr->length = g_icon_size * g_icon_size * g_pixel_size;
	icon_data_ptr->pixels = g_icon_size * g_icon_size;

	Icon_AddType(icon_data_ptr);

	g_icon_data[ICON_TYPE_NONE].rle_pixel = 0;
	Icon_MakeRLE(&g_icon_data[ICON_TYPE_NONE]);

	/*
	 * The TYPE_BLANK/"blank" icon type is a single black unmasked icon
	 */
	icon_data_ptr->desc = "blank";
	icon_data_ptr->icon_count = 1;
	icon_data_ptr->icon_data = (IconPtr) Tcl_AllocDebug(ICON_LENGTH);
	icon_data_ptr->char_table = NULL;
	icon_data_ptr->font = NULL;
	for (i = 0; i < ICON_LENGTH; i++)
	{
		if (g_icon_depth != 8)
			icon_data_ptr->icon_data[i] = 0; /* Black (RGB 0,0,0) */
		else
			icon_data_ptr->icon_data[i] = COLORMAP_BLACK;
	}

	icon_data_ptr->dynamic = FALSE;
	icon_data_ptr->depth = g_icon_depth;
	icon_data_ptr->bypp = g_pixel_size;
	icon_data_ptr->width = g_icon_size;
	icon_data_ptr->height = g_icon_size;
	icon_data_ptr->pitch = g_icon_size * g_pixel_size;
	icon_data_ptr->length = g_icon_size * g_icon_size * g_pixel_size;
	icon_data_ptr->pixels = g_icon_size * g_icon_size;

	Icon_AddType(icon_data_ptr);

	/*
	 * The TYPE_DEFAULT/"default" icon type is a single multicolored
	 * unmasked icon. If we see it, it probably means we forgot to
	 * assign an icon to something.
	 */
	icon_data_ptr->desc = "default";
	icon_data_ptr->icon_count = 1;
	icon_data_ptr->icon_data = (IconPtr) Tcl_AllocDebug(ICON_LENGTH);
	icon_data_ptr->char_table = NULL;
	icon_data_ptr->font = NULL;
	n = 0, y2 = 0;
	for (y = 0; y < 16; y++)
	{
		int dy = 0;
		if (g_icon_size == 24)
		{
			if (!(y & 1)) dy++;
		}
		if (g_icon_size == 32) dy++;
		x2 = 0;
		for (x = 0; x < 16; x++)
		{
			int dx = 0;
			if (g_icon_size == 24)
			{
				if (!(x & 1)) dx++;
			}
			if (g_icon_size == 32) dx++;
			PixelSet_RGB(icon_data_ptr->icon_data + (x2 * g_pixel_size) + (y2 * g_icon_size * g_pixel_size),
				rgb[0], rgb[1], rgb[2], g_pixel_size);
			PixelSet_RGB(icon_data_ptr->icon_data + ((x2 + dx) * g_pixel_size) + (y2 * g_icon_size * g_pixel_size),
				rgb[0], rgb[1], rgb[2], g_pixel_size);
			PixelSet_RGB(icon_data_ptr->icon_data + (x2 * g_pixel_size) + ((y2 + dy) * g_icon_size * g_pixel_size),
				rgb[0], rgb[1], rgb[2], g_pixel_size);
			PixelSet_RGB(icon_data_ptr->icon_data + ((x2 + dx) * g_pixel_size) + ((y2 + dy) * g_icon_size * g_pixel_size),
				rgb[0], rgb[1], rgb[2], g_pixel_size);
			rgb += 3;
#if 0
			icon_data_ptr->icon_data[x2 + y2 * g_icon_size] = n;
			icon_data_ptr->icon_data[(x2 + dx) + y2 * g_icon_size] = n;
			icon_data_ptr->icon_data[x2 + (y2 + dy) * g_icon_size] = n;
			icon_data_ptr->icon_data[(x2 + dx) + (y2 + dy) * g_icon_size] = n;
#endif
			n++;
			x2 += dx ? 2 : 1;
		}
		y2 += dy ? 2 : 1;
	}

	icon_data_ptr->dynamic = FALSE;
	icon_data_ptr->depth = g_icon_depth;
	icon_data_ptr->bypp = g_pixel_size;
	icon_data_ptr->width = g_icon_size;
	icon_data_ptr->height = g_icon_size;
	icon_data_ptr->pitch = g_icon_size * g_pixel_size;
	icon_data_ptr->length = g_icon_size * g_icon_size * g_pixel_size;
	icon_data_ptr->pixels = g_icon_size * g_icon_size;

	Icon_AddType(icon_data_ptr);

	/* Allocate array of t_assign for each monster */
	g_assign[ASSIGN_MONSTER].count = max_r_idx;
	C_MAKE(g_assign[ASSIGN_MONSTER].assign, max_r_idx, t_assign);

	/* Allocate array of t_assign for each object */
	g_assign[ASSIGN_OBJECT].count = max_k_idx;
	C_MAKE(g_assign[ASSIGN_OBJECT].assign, max_k_idx, t_assign);

	/* Allocate array of t_assign for the character */
	n = 1;
	g_assign[ASSIGN_CHARACTER].count = n;
	C_MAKE(g_assign[ASSIGN_CHARACTER].assign, n, t_assign);

	/* Allocate array of t_assign for each feature */
	g_assign[ASSIGN_FEATURE].count = max_f_idx;
	C_MAKE(g_assign[ASSIGN_FEATURE].assign, max_f_idx, t_assign);

	assign.assignType = ASSIGN_TYPE_ICON;
	assign.icon.type = ICON_TYPE_DEFAULT;
	assign.icon.index = 0;
	assign.icon.ascii = -1;

	/* Set default icon for the character */
	for (i = 0; i < g_assign[ASSIGN_CHARACTER].count; i++)
	{
		g_assign[ASSIGN_CHARACTER].assign[i] = assign;
	}

	/* Set default icon for each monster */
	for (i = 0; i < g_assign[ASSIGN_MONSTER].count; i++)
	{
		g_assign[ASSIGN_MONSTER].assign[i] = assign;
	}
	g_assign[ASSIGN_MONSTER].assign[0].icon.type = ICON_TYPE_NONE;

	/* Set default icon for each object */
	for (i = 0; i < g_assign[ASSIGN_OBJECT].count; i++)
	{
		g_assign[ASSIGN_OBJECT].assign[i] = assign;
	}
	g_assign[ASSIGN_OBJECT].assign[0].icon.type = ICON_TYPE_NONE;

	/*
	 * This is an array of t_icon types, one for every grid in
	 * the cave! The icons are only those assigned to features,
	 * never to monsters, objects, or the character. The icons
	 * are not calculated for visibility or light radius. This
	 * array allows different icons to be assigned to the same
	 * feature type. For example, doors are horizontal or vertical,
	 * and have different icons, and the town has a varied
	 * set of icons.
	 */
	for (i = 0; i < MAX_HGT; i++)
	{
		int layer;
		for (layer = 0; layer < ICON_LAYER_MAX; layer++)
			C_MAKE(g_icon_map[layer][i], MAX_WID, t_assign);
	}

	/*
	 * A feature can use lighting in one of three ways:
	 * (1) FT_LIGHT_NONE means ignore lighting
	 * (2) FT_LIGHT_ICON means use a sequence of icons
	 * (3) FT_LIGHT_TINT means use the g_darken[] tint table (slow)
	 */
	C_MAKE(g_feat_lite, max_f_idx, int);

	/*
	 * When a feature is masked, or a masked icon is drawn on
	 * a feature, we may use the icon assigned to a different feature
	 * as the background.
	 */
	C_MAKE(g_background, max_f_idx, int);

	/* Set default icon for each feature */
	for (i = 0; i < g_assign[ASSIGN_FEATURE].count; i++)
	{
		g_assign[ASSIGN_FEATURE].assign[i] = assign;
		g_background[i] = i;
		g_feat_lite[i] = FT_LIGHT_NONE;
	}
	g_assign[ASSIGN_FEATURE].assign[FEAT_NONE].icon.type = ICON_TYPE_NONE;

	for (i = 0; i < GRID_SHAPE_MAX; i++)
	{
		int j;
		C_MAKE(g_assignshape[i], max_f_idx * 2, t_assign);
		for (j = 0; j < max_f_idx * 2; j++)
		{
			g_assignshape[i][j] = assign;
		}
	}

	/* Array of alternates */
	g_alternate = Array_New(1, sizeof(t_alternate));
	g_alternate_count = 0;

	/* Array of sprites */
	g_sprite = Array_New(1, sizeof(t_sprite));
	g_sprite_count = 0;

	/* Array of effect info */
	g_effect = Array_New(EFFECT_MAX, sizeof(t_effect));

	C_MAKE(g_effect[EFFECT_SPELL_BALL].icon, EFFECT_SPELL_MAX, IconSpec);
	C_MAKE(g_effect[EFFECT_SPELL_BOLT].icon, EFFECT_SPELL_MAX, IconSpec);
	C_MAKE(g_effect[EFFECT_AMMO].icon, EFFECT_AMMO_MAX, IconSpec);

	g_effect[EFFECT_SPELL_BALL].name = (char **) keyword_effect_spell;
	g_effect[EFFECT_SPELL_BOLT].name = (char **) keyword_effect_spell;
	g_effect[EFFECT_AMMO].name = (char **) keyword_effect_ammo;

	/* Set default icon for each effect */
	for (i = 0; i < EFFECT_SPELL_MAX; i++)
	{
		/* Default ball icon */
		g_effect[EFFECT_SPELL_BALL].icon[i] = iconDefault;

		/* Default bolt icon (first of 4) */
		g_effect[EFFECT_SPELL_BOLT].icon[i] = iconDefault;
	}

	/* Set default icon for each effect */
	for (i = 0; i < EFFECT_AMMO_MAX; i++)
	{
		g_effect[EFFECT_AMMO].icon[i] = iconDefault;
	}

	/* Clear the color hash table */
	Palette_ResetHash();

	/* Initialize a tint table for drawing torch light (NOT USED) */
	Palette_TintTable(5, 127, g_yellow);

	/* Add some new commands to the global interpreter */
	{
		extern CommandInit assignCmdInit[];
		(void) CommandInfo_Init(g_interp, assignCmdInit, NULL);
	}

#ifdef PLATFORM_WIN
	Tcl_CreateObjCommand(g_interp, "tk_chooseFont", AngbandTk_CmdChooseFont,
		NULL, NULL);
#endif /* PLATFORM_WIN */

	g_assign_none.assignType = ASSIGN_TYPE_ICON;
	g_assign_none.icon.type = ICON_TYPE_NONE;
	g_assign_none.icon.index = 0;
	g_assign_none.icon.ascii = -1;
	
	if (init_widget(g_interp) != TCL_OK)
		quit(Tcl_GetStringFromObj(Tcl_GetObjResult(g_interp), NULL));

	if (CanvasWidget_Init(g_interp) != TCL_OK)
		quit(Tcl_GetStringFromObj(Tcl_GetObjResult(g_interp), NULL));

	/* Hack -- indices for hallucination */
	C_MAKE(g_image_monster, max_r_idx, int);
	C_MAKE(g_image_object, max_k_idx, int);

	/* Randomize the hallucination indices */
	angtk_image_reset();
	
	/* Initialize the animation timer */
	init_timer();

	/* Now we can safely use lite_spot() */
	angtk_lite_spot = angtk_lite_spot_real;
}

/*
 * Clean up the icon environment
 */
void free_icons(void)
{
	/* Make sure Tk fonts are freed or else */
	Icon_Exit(g_interp);

	/* Free the animation timer */
	free_timer();
}
