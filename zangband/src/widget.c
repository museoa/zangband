/* File: widget.c */

/* Purpose: Widget back-end stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "tnb.h"
#include "interp.h"
#include "util-dll.h"
#include "plat-dll.h"
#include "icon.h"
#include "widget.h"

static void DrawIconSpec(int y, int x, IconSpec iconSpec, BitmapPtr bitmapPtr)
{
	int pitch = bitmapPtr->pitch;
	int bypp = bitmapPtr->pixelSize;
	IconPtr srcPtr, dstPtr;
	int y2;
	int length;
	t_icon_data *iconDataPtr;

	/* Ignore NONE icon */
	if (iconSpec.type == ICON_TYPE_NONE)
		return;

	/* Special handling of BLANK */
	if (iconSpec.type == ICON_TYPE_BLANK)
	{
		/* Access the "blank" icon data */
		iconDataPtr = &g_icon_data[ICON_TYPE_BLANK];
		length = iconDataPtr->width * bypp;
		srcPtr = iconDataPtr->icon_data;
		
		/* Get the address of where to write the data in the bitmap */
		dstPtr = bitmapPtr->pixelPtr +
			x * bypp +
			y * pitch;
	
		/* Write the icon data */
		for (y2 = 0; y2 < g_icon_size; y2++)
		{
			memcpy(dstPtr, srcPtr, length);
			srcPtr += length;
			dstPtr += pitch;
		}

		/* Done */
		return;
	}

	/* Sanity check icon type */
	if ((iconSpec.type) < 0 || (iconSpec.type >= g_icon_data_count))
	{	
		/* Use "default" icon */
		iconSpec.type = ICON_TYPE_DEFAULT;
		iconSpec.index = 0;
		iconSpec.ascii = -1;
		iconSpec.dark = 0;
	}

	/* Access the icon type */
	iconDataPtr = &g_icon_data[iconSpec.type];

	/* Sanity check icon index */
	if ((iconSpec.index < 0) || (iconSpec.index >= iconDataPtr->icon_count))
	{
		/* Use "default" icon */
		iconSpec.type = ICON_TYPE_DEFAULT;
		iconSpec.index = 0;
		iconSpec.ascii = -1;
		iconSpec.dark = 0;

		/* Access the DEFAULT icon type */
		iconDataPtr = &g_icon_data[iconSpec.type];
	}

	if (iconSpec.ascii != -1)
	{
		if (iconSpec.ascii >= g_ascii_count)
		{
			/* Use "default" icon */
			iconSpec.type = ICON_TYPE_DEFAULT;
			iconSpec.index = 0;
			iconSpec.ascii = -1;
			iconSpec.dark = 0;

			/* Access the DEFAULT icon type */
			iconDataPtr = &g_icon_data[iconSpec.type];
		}
		else
		{
			IconData iconData;
/*			iconDataPtr = &g_icon_data[ICON_TYPE_BLANK];*/
			length = iconDataPtr->width * bypp;
			srcPtr = Icon_GetAsciiData(&iconSpec, iconData);
			dstPtr = bitmapPtr->pixelPtr +
				x * bypp +
				y * pitch;
			for (y2 = 0; y2 < iconDataPtr->height; y2++)
			{
				memcpy(dstPtr, srcPtr, length);
				srcPtr += length;
				dstPtr += pitch;
			}
			return;
		}
	}

	/* Verify darkness */
	if (iconSpec.dark)
	{
		if ((iconSpec.dark < 0) || (iconSpec.dark > 2))
		{
			/* Use "default" icon */
			iconSpec.type = ICON_TYPE_DEFAULT;
			iconSpec.index = 0;
			iconSpec.ascii = -1;
			iconSpec.dark = 0;

			/* Access the DEFAULT icon type */
			iconDataPtr = &g_icon_data[iconSpec.type];
		}
	}

	/* Create dark_data if needed */
	if (iconSpec.dark && (iconDataPtr->flags[iconSpec.index] & ICON_FLAG_DARK))
	{
		Icon_MakeDark(iconDataPtr, iconSpec.index);
	}

	/* Transparent */
	if (iconDataPtr->rle_data)
	{
		unsigned char *bounds = iconDataPtr->rle_bounds + iconSpec.index * 4;
		int w = bounds[2];
		int h = bounds[3];
		int col = 0;
		IconPtr rlePtr;

if (iconSpec.dark)
{
	rlePtr = iconDataPtr->dark_data[iconSpec.index] + (iconSpec.dark - 1) * iconDataPtr->rle_len[iconSpec.index];
}
else
		if (iconDataPtr->dynamic)
			rlePtr = ((IconPtr *) iconDataPtr->rle_data)[iconSpec.index];
		else
			rlePtr = iconDataPtr->rle_data + iconDataPtr->rle_offset[iconSpec.index];

		dstPtr = bitmapPtr->pixelPtr +
			x * bypp +
			y * pitch
			+ bounds[0] * bypp
			+ bounds[1] * pitch;

		while (1)
		{
			unsigned int trans, opaq;
		
			trans = rlePtr[0];
			opaq = rlePtr[1];
			rlePtr += 2;
		
			col += trans;
		
			if (opaq)
			{
				memcpy(dstPtr + col * bypp, rlePtr, opaq * bypp);
				rlePtr += opaq * bypp;
				col += opaq;
			}
			else if (!col)
				break;
		
			if (col == w)
			{
				if (!--h)
					break;
				col = 0;
				dstPtr += pitch;
			}
		}

		/* Done */
		return;
	}

if (iconSpec.dark)
{
	srcPtr = iconDataPtr->dark_data[iconSpec.index] + (iconSpec.dark - 1) * iconDataPtr->length;
}
else
	/* FIXME: tint */
	srcPtr = iconDataPtr->icon_data + iconSpec.index * iconDataPtr->length;

	/* Get the address of where to write the data in the bitmap */
	dstPtr = bitmapPtr->pixelPtr +
		x * bypp +
		y * pitch;

	length = iconDataPtr->width * bypp;

	/* Write the icon data */
	for (y2 = 0; y2 < iconDataPtr->height; y2++)
	{
		memcpy(dstPtr, srcPtr, length);
		srcPtr += length;
		dstPtr += pitch;
	}
}

/* Widget.whatToDrawProc() */
static void widget_wtd(Widget *widgetPtr, int y, int x, t_display *wtd)
{
	/* Hack - ignore parameter */
	(void) widgetPtr;

	/* If this is a valid cave location, get the display info. */
	if (in_bounds2(x, y))
		get_display_info(y, x, wtd);

	/* This isn't a valid cave location, so draw a "blank" icon */
	else
		wtd->blank = TRUE;
}

/*
 * Redraw everything.
 */
static void widget_draw_all(Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;
	int tile, layer;
	int rc = widgetPtr->rc;
	int cc = widgetPtr->cc;
	DoubleLink *link;
	int y, x, yp, xp;
	t_display wtd;
	IconSpec iconSpec;
	BitmapPtr bitmapPtr = &widgetPtr->bitmap;

	/* Paranoia: make sure the bitmap exists */
	if (widgetPtr->bitmap.pixelPtr == NULL) return;

	/* Drawing is disabled */
	if (widgetPtr->flags & WIDGET_NO_UPDATE) return;

	widgetPtr->animCnt = 0;

	for (tile = 0; tile < cc * rc; tile++)
	{
		/* This tile does not need to be redrawn */
		widgetPtr->info[tile] &= ~(WIDGET_INFO_DIRTY | WIDGET_INFO_ANIM);

		y = widgetPtr->y_min + tile / cc;
		x = widgetPtr->x_min + tile % cc;

		(*exPtr->whatToDrawProc)(widgetPtr, y, x, &wtd);

		yp = tile / cc * widgetPtr->gheight;
		xp = tile % cc * widgetPtr->gwidth;

		/* Just "erase" this spot */
		if (wtd.blank)
		{
			iconSpec.type = ICON_TYPE_BLANK;
			DrawIconSpec(yp, xp, iconSpec, bitmapPtr);
			continue;
		}

		if (wtd.anim)
		{
			widgetPtr->info[tile] |= WIDGET_INFO_ANIM;
			widgetPtr->anim[widgetPtr->animCnt++] = tile;
		}

		/*
		 * Draw 1-4 background layers.
		 */
		for (layer = 0; layer < ICON_LAYER_MAX; layer++)
		{
			iconSpec = wtd.bg[layer];
	
			/* Stop at NONE icon */
			if (iconSpec.type == ICON_TYPE_NONE)
				break;
	
			/* Draw background icon */
			DrawIconSpec(yp, xp, iconSpec, bitmapPtr);
	
			/* Stop at BLANK icon */
			if (iconSpec.type == ICON_TYPE_BLANK)
				break;
		}
	
		/* Draw foreground icon */
		if (wtd.fg.type != ICON_TYPE_NONE)
			DrawIconSpec(yp, xp, wtd.fg, bitmapPtr);
	
		/* Draw effect icon */
		if (exPtr->effect[tile].type != ICON_TYPE_NONE)
			DrawIconSpec(yp, xp, exPtr->effect[tile], bitmapPtr);
	}

	/* There are no invalid tiles */
	widgetPtr->invalidCnt = 0;

	/* Now draw all of the items for this Widget */
	for (link = widgetPtr->linkerItemVis.head; link; link = link->next)
	{
		WidgetItem *itemPtr = DoubleLink_Data(link, WidgetItem);

		(*itemPtr->typePtr->displayProc)(widgetPtr->interp, widgetPtr,
			itemPtr);

		/* Invalidate the grids covered by the item */
/*		Widget_InvalidateArea(widgetPtr, itemPtr->minY, itemPtr->minX,
			itemPtr->maxY, itemPtr->maxX);
*/	}

	/* Set dirty bounds to entire window */
	widgetPtr->dx = widgetPtr->bx;
	widgetPtr->dy = widgetPtr->by;
	widgetPtr->dw = widgetPtr->width;
	widgetPtr->dh = widgetPtr->height;
}

/*
 * Redraws only those grids that were specifically marked as invalid
 * Any affected widget items are also redrawn.
 */
static void widget_draw_invalid(Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;
	int i, layer;
	int cc = widgetPtr->cc;
	int y, x, yp, xp;
	DoubleLink *link;
	t_display wtd;
	IconSpec iconSpec;
	BitmapPtr bitmapPtr = &widgetPtr->bitmap;
	short *pinfo = widgetPtr->info;
	int dl, dt, dr, db;
	int by, bx;

	/* Paranoia: make sure the bitmap exists */
	if (bitmapPtr->pixelPtr == NULL) return;

	/* Drawing is disabled */
	if (widgetPtr->flags & WIDGET_NO_UPDATE) return;

	/* Keep track of dirty area */
	dl = bitmapPtr->width;
	dt = bitmapPtr->height;
	dr = 0;
	db = 0;

/*	widgetPtr->animCnt = 0; */

	for (i = 0; i < widgetPtr->invalidCnt; i++)
	{
		int tile = widgetPtr->invalid[i];

		/* This tile does not need to be redrawn */
		pinfo[tile] &= ~WIDGET_INFO_DIRTY;

		/* Cave coords */
		y = widgetPtr->y_min + tile / cc;
		x = widgetPtr->x_min + tile % cc;

		(*exPtr->whatToDrawProc)(widgetPtr, y, x, &wtd);

		/* Bitmap coords */
		yp = (tile / cc) * widgetPtr->gheight;
		xp = (tile % cc) * widgetPtr->gwidth;

		/* Dirty bounds */
		if (xp < dl)
			dl = xp;
		if (yp < dt)
			dt = yp;
		if (xp + widgetPtr->gwidth - 1 > dr)
			dr = xp + widgetPtr->gwidth - 1;
		if (yp + widgetPtr->gheight - 1 > db)
			db = yp + widgetPtr->gheight - 1;

		/* Just "erase" this spot */
		if (wtd.blank)
		{
			iconSpec.type = ICON_TYPE_BLANK;
			DrawIconSpec(yp, xp, iconSpec, bitmapPtr);
			continue;
		}

		if (wtd.anim && !(pinfo[tile] & WIDGET_INFO_ANIM))
		{
			pinfo[tile] |= WIDGET_INFO_ANIM;
			widgetPtr->anim[widgetPtr->animCnt++] = tile;
		}

		/*
		 * Draw 1-4 background layers.
		 */
		for (layer = 0; layer < ICON_LAYER_MAX; layer++)
		{
			iconSpec = wtd.bg[layer];
	
			/* Stop at NONE icon */
			if (iconSpec.type == ICON_TYPE_NONE)
				break;
	
			/* Draw background icon */
			DrawIconSpec(yp, xp, iconSpec, bitmapPtr);
	
			/* Stop at BLANK icon */
			if (iconSpec.type == ICON_TYPE_BLANK)
				break;
		}
	
		/* Draw foreground icon */
		if (wtd.fg.type != ICON_TYPE_NONE)
			DrawIconSpec(yp, xp, wtd.fg, bitmapPtr);
	
		/* Draw effect icon */
		if (exPtr->effect[tile].type != ICON_TYPE_NONE)
			DrawIconSpec(yp, xp, exPtr->effect[tile], bitmapPtr);
	}

	widgetPtr->invalidCnt = 0;

	/* Redraw any items inside the dirty area */
	by = widgetPtr->by;
	bx = widgetPtr->bx;
	for (link = widgetPtr->linkerItemVis.head; link; link = link->next)
	{
		WidgetItem *itemPtr = DoubleLink_Data(link, WidgetItem);

		if ((bx + itemPtr->x2 > dl) && (bx + itemPtr->x1 < dr) &&
			(by + itemPtr->y2 > dt) && (by + itemPtr->y1 < db))
		{
			(*itemPtr->typePtr->displayProc)(widgetPtr->interp, widgetPtr,
				itemPtr);
		}
				
		/* Invalidate the grids covered by the item */
/*		Widget_InvalidateArea(widgetPtr, itemPtr->minY, itemPtr->minX,
			itemPtr->maxY, itemPtr->maxX);
*/	}

	widgetPtr->dx = dl;
	widgetPtr->dy = dt;
	widgetPtr->dw = dr - dl + 1;
	widgetPtr->dh = db - dt + 1;
}

static int widget_configure(Tcl_Interp *interp, Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;

	/* Hack - ignore unused parameter */
	(void) interp;

	/* Valid micro-map sizes: 4, 6, 8 */
	if (widgetPtr->style == WIDGET_STYLE_MAP)
	{
		widgetPtr->drawAllProc = map_draw_all;
		widgetPtr->drawInvalidProc = map_draw_invalid;
		widgetPtr->hitTestProc = NULL;
		exPtr->whatToDrawProc = NULL;
		exPtr->symbolProc = map_symbol_proc;
	}

	/* Make this Widget draw icons */
	if (widgetPtr->style == WIDGET_STYLE_ICON)
	{
		widgetPtr->drawAllProc = widget_draw_all;
		widgetPtr->drawInvalidProc = widget_draw_invalid;
		widgetPtr->hitTestProc = NULL;
		exPtr->whatToDrawProc = widget_wtd;
		exPtr->symbolProc = NULL;
	}

	return TCL_OK;
}

static void widget_changed(Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;
	int i;

	if ((exPtr->effect != NULL) &&
		((widgetPtr->tc != widgetPtr->oldTileCnt)))
	{
		if (exPtr->effect)
		{
			Tcl_FreeDebug((char *) exPtr->effect);
			exPtr->effect = NULL;
		}
	}

	/*  */
	if ((widgetPtr->style != WIDGET_STYLE_MAP) && (exPtr->effect == NULL))
	{
		exPtr->effect = (IconSpec *) Tcl_AllocDebug(sizeof(IconSpec) * widgetPtr->tc);

		for (i = 0; i < widgetPtr->tc; i++)
		{
			exPtr->effect[i].type = ICON_TYPE_NONE;
		}
	}
}

static void widget_destroy(Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;

	if (exPtr->effect)
		Tcl_FreeDebug((char *) exPtr->effect);
}

/*
 * Allocate storage for a new Widget.
 */
static int widget_create(Tcl_Interp *interp, Widget **ptr)
{
	ExWidget *exPtr = (ExWidget *) Tcl_AllocDebug(sizeof(ExWidget));
	Widget *widgetPtr = (Widget *) exPtr;

	/* Hack - ignore unused parameter */
	(void) interp;

	widgetPtr->centerProc = NULL;
	widgetPtr->configureProc = widget_configure;
	widgetPtr->changedProc = widget_changed;
	widgetPtr->destroyProc = widget_destroy;
	widgetPtr->drawInvalidProc = NULL;
	widgetPtr->hitTestProc = NULL;
	widgetPtr->wipeProc = NULL;
	widgetPtr->invalidateProc = NULL;
	widgetPtr->invalidateAreaProc = NULL;
	exPtr->whatToDrawProc = NULL;
	exPtr->symbolProc = NULL;
	exPtr->effect = NULL;

	(*ptr) = widgetPtr;

	return TCL_OK;
}

/*
 * One-time initialization.
 */
int init_widget(Tcl_Interp *interp)
{
	if (Widget_Init(interp, widget_create) != TCL_OK)
		return TCL_ERROR;
	return TCL_OK;
}

void angtk_widget_lock(bool lock)
{
	DoubleLink *link;
	Widget *widgetPtr;

	if (lock)
	{
		for (link = WidgetList.head; link; link = link->next)
		{
			widgetPtr = DoubleLink_Data(link, Widget);
			widgetPtr->flags |= WIDGET_NO_UPDATE;
		}
	}
	else
	{
		for (link = WidgetList.head; link; link = link->next)
		{
			widgetPtr = DoubleLink_Data(link, Widget);
			if (!widgetPtr->noUpdate)
			{
				widgetPtr->flags &= ~WIDGET_NO_UPDATE;
			}
		}
	}
}

/*
 * Now for spell and missile effects in a Widget. The basic idea is to
 * prepare for drawing, perform the drawing (offscreen), display
 * the changed grids onscreen, then repair the affected grids offscreen.
 *
 * Specifically, we call angtk_effect_prep(), then another routine to
 * draw any number of spell/missile grids offscreen, and finally
 * angtk_effect_fresh() to display the changes onscreen. This allows us
 * to draw each radius of a spell explosion in turn.
 *
 * Because a Widget may display an arbitrary area of the cave, and because
 * a particular Widget may not be mapped (ie, visible onscreen), we keep
 * track of which Widgets were really drawn into and require redisplaying.
 */

/*
 * Return EFFECT_SPELL_XXX constant for given GF_XXX constant. For each
 * EFFECT_SPELL_XXX we have a bolt icon assignment and a ball icon assignment.
 * The bolt assignment is assumed to be the first of four icons.
 */
static byte effect_index(int type)
{
	/* Analyze */
	switch (type)
	{
		case GF_MISSILE:	return (EFFECT_SPELL_MISSILE);
		case GF_ACID:		return (EFFECT_SPELL_ACID);
		case GF_ELEC:		return (EFFECT_SPELL_ELEC);
		case GF_FIRE:		return (EFFECT_SPELL_FIRE);
		case GF_COLD:		return (EFFECT_SPELL_COLD);
		case GF_POIS:		return (EFFECT_SPELL_POIS);
		case GF_HOLY_FIRE:	return (EFFECT_SPELL_HOLY_FIRE);
		case GF_HELL_FIRE:	return (EFFECT_SPELL_HELL_FIRE);
		case GF_MANA:		return (EFFECT_SPELL_MANA);
		case GF_ARROW:		return (EFFECT_SPELL_ARROW);
		case GF_WATER:		return (EFFECT_SPELL_WATER);
		case GF_NETHER:		return (EFFECT_SPELL_NETHER);
		case GF_CHAOS:		return (EFFECT_SPELL_CHAOS);
		case GF_DISENCHANT:	return (EFFECT_SPELL_DISENCHANT);
		case GF_NEXUS:		return (EFFECT_SPELL_NEXUS);
		case GF_CONFUSION:	return (EFFECT_SPELL_CONFUSION);
		case GF_SOUND:		return (EFFECT_SPELL_SOUND);
		case GF_SHARDS:		return (EFFECT_SPELL_SHARD);
		case GF_FORCE:		return (EFFECT_SPELL_FORCE);
		case GF_INERTIA:	return (EFFECT_SPELL_INERTIA);
		case GF_GRAVITY:	return (EFFECT_SPELL_GRAVITY);
		case GF_TIME:		return (EFFECT_SPELL_TIME);
		case GF_LITE_WEAK:	return (EFFECT_SPELL_LITE_WEAK);
		case GF_LITE:		return (EFFECT_SPELL_LITE);
		case GF_DARK_WEAK:	return (EFFECT_SPELL_DARK_WEAK);
		case GF_DARK:		return (EFFECT_SPELL_DARK);
		case GF_PLASMA:		return (EFFECT_SPELL_PLASMA);
		case GF_METEOR:		return (EFFECT_SPELL_METEOR);
		case GF_ICE:		return (EFFECT_SPELL_ICE);
		case GF_ROCKET:		return (EFFECT_SPELL_ROCKET);
		case GF_DEATH_RAY:	return (EFFECT_SPELL_DEATH_RAY);
		case GF_NUKE:		return (EFFECT_SPELL_NUKE);
		case GF_DISINTEGRATE:	return (EFFECT_SPELL_DISINTEGRATE);
		case GF_PSI:
		case GF_PSI_DRAIN:
		case GF_TELEKINESIS:
		case GF_DOMINATION:		return (EFFECT_SPELL_PSI);
	}

	/* Standard "color" */
	return (EFFECT_SPELL_FORCE);
}

static bool angtk_effect_aux(int y, int x, IconSpec *iconSpecPtr)
{
	Widget *widgetPtr;
	ExWidget *exPtr;
	DoubleLink *link;
	int row, col;
	bool drawn = FALSE;

	/* Paranoia */
	iconSpecPtr->dark = 0;

	/* Check each Widget */
	for (link = WidgetListMap.head; link; link = link->next)
	{
		widgetPtr = DoubleLink_Data(link, Widget);
		exPtr = (ExWidget *) widgetPtr;

		/* Don't draw in micro-map */
		if (widgetPtr->style == WIDGET_STYLE_MAP) continue;

		/* Drawing is disabled */
		if (widgetPtr->flags & WIDGET_NO_UPDATE) continue;

		/* Don't draw out of bounds */
		if (!Widget_CaveToView(widgetPtr, y, x, &row, &col))
			continue;
			
		/* Set effect icon */
		exPtr->effect[col + row * widgetPtr->cc] = *iconSpecPtr;

		/* Mark the grid as invalid */
		Widget_Invalidate(widgetPtr, row, col);

		/* Redraw later */
		widgetPtr->flags |= WIDGET_DRAW_INVALID;
		Widget_EventuallyRedraw(widgetPtr);

		/* Something was drawn */
		drawn = TRUE;
	}

	return drawn;
}

void angtk_effect_clear(int y, int x)
{
	Widget *widgetPtr;
	ExWidget *exPtr;
	DoubleLink *link;
	int row, col;

	/* Check each Widget */
	for (link = WidgetListMap.head; link; link = link->next)
	{
		widgetPtr = DoubleLink_Data(link, Widget);
		exPtr = (ExWidget *) widgetPtr;

		/* Don't draw in micro-map */
		if (widgetPtr->style == WIDGET_STYLE_MAP) continue;

		/* Drawing is disabled */
		if (widgetPtr->flags & WIDGET_NO_UPDATE) continue;

		/* Don't draw out of bounds */
		if (!Widget_CaveToView(widgetPtr, y, x, &row, &col))
			continue;

		/* No effect was drawn here */
		if (exPtr->effect[col + row * widgetPtr->cc].type == ICON_TYPE_NONE)
			continue;

		/* Clear effect icon */
		exPtr->effect[col + row * widgetPtr->cc].type = ICON_TYPE_NONE;

		/* Mark the grid as invalid */
		Widget_Invalidate(widgetPtr, row, col);

		widgetPtr->flags |= WIDGET_DRAW_INVALID;
		Widget_EventuallyRedraw(widgetPtr);
	}
}

/*
 * Draws the icon assigned to a particular spell type (GF_XXX constant).
 */
bool angtk_effect_spell(int y, int x, int typ, int bolt)
{
	int effect = effect_index(typ);
	IconSpec iconSpec;

	if (bolt)
	{
		iconSpec = g_effect[EFFECT_SPELL_BOLT].icon[effect];

		iconSpec.index += bolt - 1;
	}
	else
	{
		iconSpec = g_effect[EFFECT_SPELL_BALL].icon[effect];
	}

	return angtk_effect_aux(y, x, &iconSpec);
}

/*
 * Draws the icon assigned to a fired ammunition
 */
bool angtk_effect_ammo(int y, int x, object_type *o_ptr, int dir)
{
	IconSpec iconSpec;

	/* Eliminate '5' */
	if (dir >= 5) dir -= 1;

	switch (k_info[o_ptr->k_idx].tval)
	{
		case TV_ARROW:
			iconSpec = g_effect[EFFECT_AMMO].icon[EFFECT_AMMO_ARROW];
			iconSpec.index += dir - 1;
			break;

		case TV_BOLT:
			iconSpec = g_effect[EFFECT_AMMO].icon[EFFECT_AMMO_BOLT];
			iconSpec.index += dir - 1;
			break;

		/* Sling ammo */
		default:
			return angtk_effect_object(y, x, o_ptr);
	}

	/* Nothing is assigned */
	if (iconSpec.type == ICON_TYPE_DEFAULT)
	{
		return angtk_effect_object(y, x, o_ptr);
	}

	return angtk_effect_aux(y, x, &iconSpec);
}

/*
 * Draws the icon assigned to thrown object
 */
bool angtk_effect_object(int y, int x, object_type *o_ptr)
{
	t_assign assign = g_assign[ASSIGN_OBJECT].assign[o_ptr->k_idx];
	IconSpec iconSpec;

	FinalIcon(&iconSpec, &assign, 0, o_ptr);
	return angtk_effect_aux(y, x, &iconSpec);
}

/*
 * Performs actions at idle time
 */
void angtk_idle(void)
{
	DoubleLink *link;
	int i;

	/* Calculate the current frame for sprites */
	if (update_sprites() == 0) return;

	for (link = WidgetListMap.head; link; link = link->next)
	{
		Widget *widgetPtr = DoubleLink_Data(link, Widget);

		/* This Widget doesn't have any animated grids */
		if (!widgetPtr->animCnt) continue;

		/*
		 * It is possible that some grids which don't display sprites
		 * are in the array, but that's okay for now.
		 */
		for (i = 0; i < widgetPtr->animCnt; i++)
		{
			int tile = widgetPtr->anim[i];
			if (!(widgetPtr->info[tile] & WIDGET_INFO_DIRTY))
			{
				widgetPtr->invalid[widgetPtr->invalidCnt++] = tile;
				widgetPtr->info[tile] |= WIDGET_INFO_DIRTY;
			}
		}

		/* A redraw was scheduled */
		if (widgetPtr->flags & WIDGET_REDRAW)
		{
			/* Cancel the scheduled redraw */
			Tcl_CancelIdleCall(Widget_Display, (ClientData) widgetPtr);
		}

		/* Redraw invalidated grids (the sprites) */
		widgetPtr->flags |= WIDGET_DRAW_INVALID;

		/* Update grids, and copy to screen */
		Widget_Display((ClientData) widgetPtr);
	}

	/* Redisplay sprites in all Widget-type Canvas items */
	CanvasWidget_Idle();
}

/*
 * This is a dummy lite_spot() routine that may get called before
 * the icons have been initialized.
 */
static void angtk_lite_spot_dummy(int y, int x)
{
	/* Ignore parameters and do nothing */
	(void) x;
	(void) y;
}

void (*angtk_lite_spot)(int y, int x) = angtk_lite_spot_dummy;

/*
 * This is called whenever the game thinks a grid needs to be redrawn,
 * via lite_spot(). If the grid actually changed then a redraw is
 * scheduled.
 */
void angtk_lite_spot_real(int y, int x)
{
	DoubleLink *link;
	int row, col;

	/* Get knowledge about location */
	get_grid_info(y, x, &g_grid[y][x]);

	/* Update the global array of map symbols */
	map_symbol_set(y, x);

	/* Check each Widget */
	for (link = WidgetListMap.head; link; link = link->next)
	{
		Widget *widgetPtr = DoubleLink_Data(link, Widget);

		/* Drawing is disabled */
		if (widgetPtr->flags & WIDGET_NO_UPDATE)
			continue;

		/* A full redraw is already pending */
		if (widgetPtr->flags & WIDGET_WIPE)
			continue;

		/* Cave location isn't visible */
		if (!Widget_CaveToView(widgetPtr, y, x, &row, &col))
			continue;

		/* */
		Widget_Invalidate(widgetPtr, row, col);

		/* Redraw invalid grids later */
		widgetPtr->flags |= WIDGET_DRAW_INVALID;
		Widget_EventuallyRedraw(widgetPtr);
	}
}
