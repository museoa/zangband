/* File: icon1.c */

/* Purpose: icon stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include "icon.h"

unsigned char *g_palette_rgb;
t_assign_group g_assign[ASSIGN_MAX];
t_assign g_assign_none;
t_grid *g_grid[MAX_HGT] = {0};
int g_grid_xtra_init = 0;
t_flavor *g_flavor = NULL; /* Array of flavor types */
int g_flavor_count = 0; /* Number of flavors */
Tcl_HashTable g_flavor_table; /* Map flavor name to g_flavor[] index */
t_alternate *g_alternate; /* Array of alternate icon info */
int g_alternate_count;  /* Number of elems in g_alternate[] array */
t_effect *g_effect; /* Array of effect icon info */
int *g_feat_lite = NULL;
int *g_background = NULL;

t_assign *g_icon_map[ICON_LAYER_MAX][MAX_HGT];
bool g_icon_map_changed = FALSE;
int *g_image_monster, *g_image_object;
bool g_daytime; /* Day or night */


/*
 * This routine fills the given t_grid struct with the indices of
 * any known feature, object or monster at the given cave location.
 * This routine does not consider light radius.
 */
void get_grid_info(int y, int x, t_grid *gridPtr)
{
	byte feat;

	object_type *o_ptr;

	s16b m_idx;

	/*
	 * Hallucination: The original game assigns a random image to
	 * an object or monster, and sometimes (1 in 256) to a feature.
	 */

	gridPtr->f_idx = 0;
	gridPtr->o_ptr = NULL;
	gridPtr->m_idx = 0;

	/* Feature */
	feat = area(x, y)->feat;

	/* Monster/Player */
	m_idx = area(x, y)->m_idx;

	if ((y == p_ptr->py) && (x == p_ptr->px)) m_idx = -1;

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

	/* Remember the feature index */
	gridPtr->f_idx = parea(x, y)->feat;

	/* Objects */
	OBJ_ITT_START (area(x, y)->o_idx, o_ptr)
	{
		/* Memorized objects */
		if (!o_ptr->info & OB_SEEN) continue;
				
		/* Remember the top-most object */
		gridPtr->o_ptr = o_ptr;

		/* Stop */
		break;
	}
	OBJ_ITT_END;

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
	int m_idx, f_idx;
	
	object_type *o_ptr;

	/* Access the global cave memory */
	t_grid *gridPtr = &g_grid[y][x];

	int layer;

	t_assign assign;
	IconSpec iconSpec;
	
	m_idx = gridPtr->m_idx;
	o_ptr = gridPtr->o_ptr;
	f_idx = gridPtr->f_idx;

	/* The grid is completely uninteresting */
	if (!m_idx && !o_ptr && !f_idx)
	{
		/* All other values are uninitialized */
		displayPtr->blank = TRUE;
		
		/* Done */
		return;
	}

	/* */
	displayPtr->blank = FALSE;
	displayPtr->tint = NULL;

	if (m_idx || o_ptr)
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
		else if (o_ptr)
		{	
			/* Get the icon assigned to the object kind */
			assign = g_assign[ASSIGN_OBJECT].assign[o_ptr->k_idx];
		}

		/*
		 * Now we have the assignment for the character, monster, or object.
		 * The assignment may be TYPE_ALTERNATE, or TYPE_FLAVOR, which we
		 * must resolve into a "real" icon type and
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
						if (o_ptr->number == 1) ++index;
						break;
					
					case REASON_IDENT:
						if (object_known_p(o_ptr)) ++index;
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
		
				break;
			}
		}
	}

	/* No character, monster or object */
	else
	{
		iconSpec.type = ICON_TYPE_NONE;
	}

	/* Set the foreground icon */
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

	/*
	 * Get the icon from the global icon map. The g_icon_map[]
	 * array allows us to use different icons for the same
	 * feature index. For example, doors may be vertical or
	 * horizontal.
	 */
	for (layer = 0; layer < ICON_LAYER_MAX; layer++)
	{
		assign = g_icon_map[layer][y][x];

		iconSpec.type = assign.icon.type;
		iconSpec.index = assign.icon.index;
		iconSpec.ascii = assign.icon.ascii;

		/* Only layer 1 is required */
		if (iconSpec.type == ICON_TYPE_NONE)
		{
			displayPtr->bg[layer] = iconSpec;
			break;
		}

		displayPtr->bg[layer] = iconSpec;
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
	int feat = area(x, y)->feat;
	t_assign assign;
	IconSpec iconSpec;
	int layer;

	/* The dungeon isn't ready yet */
	if (!character_dungeon) return;

	/* Paranoia */
	if (g_icon_map[ICON_LAYER_1][0] == NULL) return;

	/* Get the assignment for this feature */
	assign = g_assign[ASSIGN_FEATURE].assign[feat];

	if (assign.assignType == ASSIGN_TYPE_ALTERNATE)
	{
		/* This is a door */
		if ((feat == FEAT_OPEN) || (feat == FEAT_BROKEN) || (feat == FEAT_CLOSED))
		{
			/* The reason must be REASON_FEATURE */
			t_alternate *alternatePtr = &g_alternate[assign.alternate.index];

			/* Index 0 is horizontal door, 1 is vertical door */
			iconSpec = alternatePtr->icon[0];

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
			if ((feat == FEAT_OPEN) || (feat == FEAT_BROKEN) || (feat == FEAT_CLOSED))
			{
				/* The reason must be REASON_FEATURE */
				t_alternate *alternatePtr = &g_alternate[assign.alternate.index];
	
				/* Index 0 is horizontal door, 1 is vertical door */
				iconSpec = alternatePtr->icon[0];

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
	}
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
				*b++ = (*(t + *bg++) & *mk++) | *fg++;
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


/*
 * Recalculate the indices used for monsters and objects during
 * hallucination.
 */
void angtk_image_reset(void)
{
	int i;

	/* Randomize monsters */
	for (i = 1; i < z_info->r_max; i++)
	{
		int r_idx;
		monster_race *r_ptr;

		/* Infinite loop */
		while (1)
		{
			/* Pick a random non-unique */
			r_idx = randint1(z_info->r_max - 1);

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
	for (i = 1; i < z_info->k_max; i++)
	{
		int k_idx;
		object_kind *k_ptr;

		/* Infinite loop */
		while (1)
		{
			/* Pick a random object kind */
			k_idx = randint1(z_info->k_max - 1);

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

void angtk_flavor_swap(int n, int a, int b)
{
	int tmp;

	tmp = g_flavor[n].sorted[a];
	g_flavor[n].sorted[a] = g_flavor[n].sorted[b];
	g_flavor[n].sorted[b] = tmp;
}

static int init_flavor_aux(int n, cptr desc, int tval, int count,
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
 * Clean up the icon environment
 */
void free_icons(void)
{
	/* Make sure Tk fonts are freed or else */
	Icon_Exit(g_interp);
}
