/* File: widget.c */

/* Purpose: Widget back-end stuff */

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
#include "util-dll.h"
#include "plat-dll.h"
#include "icon.h"
#include "widget.h"

extern void map_draw_all(Widget *widgetPtr);
extern void map_draw_invalid(Widget *widgetPtr);
extern int map_symbol_proc(Widget *widgetPtr, int y, int x);


/*
 * Invert a grid in the bitmap of a Widget
 */
void Widget_InvertSpot(Widget *widgetPtr, int row, int col, t_display *wtd)
{
	BitmapPtr bitmapPtr = &widgetPtr->bitmap;
	int bypp = bitmapPtr->pixelSize;
	int pitch = bitmapPtr->pitch;
	IconPtr dstPtr;
	t_icon_data *iconDataPtr;
	int xp, yp, x, y;

	if (widgetPtr->style == WIDGET_STYLE_ICON)
	{
		yp = row * widgetPtr->gheight;
		xp = col * widgetPtr->gwidth;
	}
	else
	{
		yp = widgetPtr->yp[row * widgetPtr->cc + col];
		xp = widgetPtr->xp[row * widgetPtr->cc + col] + 10;
	}

	/* The icon is transparent */
	if (!wtd->blank && (wtd->fg.type != ICON_TYPE_NONE) &&
		g_icon_data[wtd->fg.type].rle_data)
	{
		IconPtr rlePtr;
		int col = 0;
		int h, w;
		unsigned char *bounds;

		/* Access the icon info */
		iconDataPtr = &g_icon_data[wtd->fg.type];

		rlePtr = iconDataPtr->rle_data + iconDataPtr->rle_offset[wtd->fg.index];
		bounds = iconDataPtr->rle_bounds + wtd->fg.index * 4;
		w = bounds[2];
		h = bounds[3];
		if (widgetPtr->style == WIDGET_STYLE_ICON)
		{
			xp += bounds[0];
			yp += bounds[1];
		}

		dstPtr = bitmapPtr->pixelPtr + xp * bypp + yp * pitch;

		while (1)
		{
			unsigned int trans, opaq;
		
			trans = rlePtr[0];
			opaq = rlePtr[1];
			rlePtr += 2;
			col += trans;
			
			if (opaq)
			{
				for (x = 0; x < opaq * bypp; x++)
					*(dstPtr + col * bypp + x) = ~*(dstPtr + col * bypp + x);
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
	}
	else
	{
		/* Get the address of the top-left corner */
		dstPtr = bitmapPtr->pixelPtr + xp * bypp + yp * pitch;

		for (y = 0; y < g_icon_size; y++)
		{
			for (x = 0; x < g_icon_size * bypp; x++)
			{
				*(dstPtr + x) = ~*(dstPtr + x);
			}
			dstPtr += pitch;
		}
	}

	XCopyArea(widgetPtr->display,
		widgetPtr->bitmap.pixmap, /* source drawable */
		Tk_WindowId(widgetPtr->tkwin), /* dest drawable */
		widgetPtr->copyGC, /* graphics context */
		xp, yp, /* source top-left */
		(unsigned int) widgetPtr->gwidth, /* width */
		(unsigned int) widgetPtr->gheight, /* height */
		xp - widgetPtr->bx, yp - widgetPtr->by /* dest top-left */
	);
}

/*
 * Visually "flash" the given location.
 */
void angtk_invert_spot(int y, int x)
{
	int row, col;
	DoubleLink *link;
	t_display wtd;
	Display *display = None;

	/* Check each mapped Widget */
	for (link = WidgetListMap.head; link; link = link->next)
	{
		Widget *widgetPtr = DoubleLink_Data(link, Widget);

		/* Skip micro-map Widgets */
		if (widgetPtr->style == WIDGET_STYLE_MAP)
			continue;

		/* Drawing is disabled */
		if (widgetPtr->flags & WIDGET_NO_UPDATE)
			continue;

		if (!Widget_CaveToView(widgetPtr, y, x, &row, &col))
			continue;

		(*((ExWidget *) widgetPtr)->whatToDrawProc)(widgetPtr, y, x, &wtd);
		
		/* Invert pixels, copy to screen */
		Widget_InvertSpot(widgetPtr, row, col, &wtd);

		/* Invalidate the grid, so it will be redrawn later */
		Widget_Invalidate(widgetPtr, row, col);

		/* Redraw later */
		widgetPtr->flags |= WIDGET_DRAW_INVALID;
		Widget_EventuallyRedraw(widgetPtr);

		display = widgetPtr->display;
	}

	if (display != None)
		Plat_SyncDisplay(display);

	/* Delay */
	Term_xtra(TERM_XTRA_DELAY, delay_factor * delay_factor * delay_factor);
}

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
void widget_wtd(Widget *widgetPtr, int y, int x, t_display *wtd)
{
	/* If this is a valid cave location, get the display info. */
	if (in_bounds2(y, x))
		get_display_info(y, x, wtd);

	/* This isn't a valid cave location, so draw a "blank" icon */
	else
		wtd->blank = TRUE;
}

/*
 * Redraw everything.
 */
void widget_draw_all(Widget *widgetPtr)
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
void widget_draw_invalid(Widget *widgetPtr)
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

int widget_configure(Tcl_Interp *interp, Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;

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

void widget_changed(Widget *widgetPtr)
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
		exPtr->effect = Tcl_AllocDebug(sizeof(IconSpec) * widgetPtr->tc);

		for (i = 0; i < widgetPtr->tc; i++)
		{
			exPtr->effect[i].type = ICON_TYPE_NONE;
		}
	}
}

void widget_destroy(Widget *widgetPtr)
{
	ExWidget *exPtr = (ExWidget *) widgetPtr;

	if (exPtr->effect)
		Tcl_FreeDebug((char *) exPtr->effect);
}

/*
 * Allocate storage for a new Widget.
 */
int widget_create(Tcl_Interp *interp, Widget **ptr)
{
	ExWidget *exPtr = (ExWidget *) Tcl_AllocDebug(sizeof(ExWidget));
	Widget *widgetPtr = (Widget *) exPtr;

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

bool angtk_effect_aux(int y, int x, IconSpec *iconSpecPtr)
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
void angtk_lite_spot_dummy(int y, int x)
{
	/* Nothing */
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

/*
 * Visual effect: Draw a circle centered at y,x of radius r.
 * Used for detection spells, earthquake, etc.
 */
static void Widget_DetectRadius(Widget *widgetPtr, GC gc, int y, int x, int r)
{
	int ymin = widgetPtr->y_min;
	int xmin = widgetPtr->x_min;
	int x2, y2;

	x2 = widgetPtr->bx + (x - r - xmin) * widgetPtr->gwidth + widgetPtr->gwidth / 2;
	y2 = widgetPtr->by + (y - r - ymin) * widgetPtr->gheight + widgetPtr->gheight / 2;

	XDrawArc(widgetPtr->display, widgetPtr->bitmap.pixmap, gc, x2, y2,
		r * 2 * widgetPtr->gwidth, r * 2 * widgetPtr->gheight, 0, 360 * 64);
}

/*
 * Visual effect: Draw a circle centered at y,x of radius r.
 * Used for detection spells, earthquake, etc.
 */
void angtk_detect_radius(int y, int x, int r)
{
	static s32b s_detect_turn = 0L;
	Tk_Window tkwin = Tk_MainWindow(g_interp);
	XGCValues gcValues;
	XColor *color;
	GC gc;
	DoubleLink *link;
	int dist;

	/* Hack -- Avoid multiple flashes per spell */
	if (turn == s_detect_turn) return;
	s_detect_turn = turn;

	/* Get gray pixel value */
	color = Tk_GetColor(NULL, tkwin, Tk_GetUid("gray"));
	if (color == NULL)
	{
		return;
	}

	/* Get a graphics context for drawing */
	gcValues.foreground = color->pixel;
	gcValues.line_width = 2;
	gc = Tk_GetGC(tkwin, GCForeground | GCLineWidth, &gcValues);

	/*
	 * Draw "radius = dist * dist", and always draw "radius = r".
	 */
	dist = 2;
	while (1)
	{
		int r2 = dist * dist;
		unsigned long msStart;

		if (r2 > r)
		{
			r2 = r;
		}

		msStart = Milliseconds();

		/* Draw the effect */
		for (link = WidgetListMap.head; link; link = link->next)
		{
			Widget *widgetPtr = DoubleLink_Data(link, Widget);
			
			if (widgetPtr->flags & WIDGET_NO_UPDATE) continue;
			if (widgetPtr->style != WIDGET_STYLE_MAP) continue;

			Widget_DetectRadius(widgetPtr, gc, y, x, r2);
			Widget_Display((ClientData) widgetPtr);
		}

		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, 30 - (Milliseconds() - msStart));

		/* Repair offscreen pixmap */
		for (link = WidgetListMap.head; link; link = link->next)
		{
			Widget *widgetPtr = DoubleLink_Data(link, Widget);

			if (widgetPtr->flags & WIDGET_NO_UPDATE) continue;
			if (widgetPtr->style != WIDGET_STYLE_MAP) continue;

			Widget_DrawAll(widgetPtr);

			if (r2 == r)
				Widget_Display((ClientData) widgetPtr);
		}

		/* Stop */
		if (r2 == r) break;

		++dist;
	}

	/* Cleanup */
	Tk_FreeGC(Tk_Display(tkwin), gc);
	Tk_FreeColor(color);
}

/*
 * A big silly routine for a big silly visual effect. When arg=1, every
 * icon-drawing mapped Widget is randomly recentered and redisplayed
 * a number of times. When arg=2, every icon-drawing mapped widget is
 * totally erased to white and a display is scheduled. When arg=3 every
 * icon-drawing mapped widget is wiped and a redraw is scheduled (to
 * erase the white when arg=2).
 */
void angtk_destroy_area(int arg)
{
	int i, cy, cx;
	int offset_y[16] = {0, -1, 1, -1, 1, 1, 0, -1, 0, -1, 1, -1, 1, 1, 0, -1};
	int offset_x[16] = {-1, 1, 0, 0, 1, -1, 1, -1, -1, 1, 0, 0, 1, -1, 1, -1};
	unsigned long msStart, msEnd;
	DoubleLink *link;

	/*
	 * Mega-Hack -- Update everything. I noticed that the smarty-button
	 * popups were not withdrawn before the effect took place.
	 */
	Term_fresh();

	/* Check each Widget */
	for (link = WidgetListMap.head; link; link = link->next)
	{
		Widget *widgetPtr = DoubleLink_Data(link, Widget);

		/* Skip non-icon-drawing Widgets */
		if (widgetPtr->style == WIDGET_STYLE_MAP) continue;

		/* Step 1: Shake n' bake */
		if (arg == 1)
		{
			/* Drawing is disabled */
			if (widgetPtr->flags & WIDGET_NO_UPDATE) continue;

			cy = widgetPtr->y;
			cx = widgetPtr->x;
			for (i = 0; i < 16; i++)
			{
				msStart = Milliseconds();
				Widget_Center(widgetPtr, cy + offset_y[i], cx + offset_x[i]);
				Widget_Display((ClientData) widgetPtr);
				msEnd = Milliseconds();
				Term_xtra(TERM_XTRA_DELAY, 70 - (msEnd - msStart));
			}
			Widget_Center(widgetPtr, cy, cx);
		}

		/* Step 2: Blast of light */
		else if (arg == 2)
		{
			XRectangle rect;
			XGCValues gcValues;
			GC gc;

			rect.x = widgetPtr->bx;
			rect.y = widgetPtr->by;
			rect.width = widgetPtr->width;
			rect.height = widgetPtr->height;

			gcValues.foreground = WhitePixelOfScreen(Tk_Screen(widgetPtr->tkwin));
			gc = Tk_GetGC(widgetPtr->tkwin, GCForeground, &gcValues);
	
		    XFillRectangles(widgetPtr->display,
		    	widgetPtr->bitmap.pixmap, gc,
				&rect, 1);
	
			Tk_FreeGC(widgetPtr->display, gc);

			/*
			 * Eventually redisplay the widget, but do NOT update the grids.
			 * That is, the redisplay should show us the white field we
			 * painted above. NOTE: angtk_widget_lock() is called for us.
			 */
			widgetPtr->flags &= ~(WIDGET_DRAW_INVALID | WIDGET_WIPE);
			Widget_EventuallyRedraw(widgetPtr);
		}

		/* Step 3: Redraw later */
		else if (arg == 3)
		{
			/* Drawing is disabled */
			if (widgetPtr->flags & WIDGET_NO_UPDATE) continue;

			Widget_Wipe(widgetPtr);
		}

		/*
		 * XXX Mega-Hack -- Only actually draw into the first suitable
		 * Widget in the list. If we had two or more suitable Widgets
		 * we would need to rewrite the loops to make it work.
		 */
		break;
	}
}

/*
 * do_cmd_locate() helper
 */
void angtk_locate(int dir)
{
	DoubleLink *link;
	int y, x;

	/* Check each Widget */
	for (link = WidgetListMap.head; link; link = link->next)
	{
		Widget *widgetPtr = DoubleLink_Data(link, Widget);
		int rc = widgetPtr->rc;
		int cc = widgetPtr->cc;

		/* Skip non-icon-drawing Widgets */
		if (widgetPtr->style == WIDGET_STYLE_MAP) continue;

		if (dir == -1)
		{
			Widget_Center(widgetPtr, py, px);
			break;
		}

		y = widgetPtr->y;
		x = widgetPtr->x;

		if (widgetPtr->style == WIDGET_STYLE_ICON)
		{
			if (ddy[dir] < 0)
			{
				if (widgetPtr->y_min > -1)
				{
					y = widgetPtr->y_min;
					if (y - rc / 2 < -1)
						y = rc / 2 - 1;
				}
			}
			if (ddy[dir] > 0)
			{
				if (widgetPtr->y_max - 1 < g_cave_hgt)
				{
					y = widgetPtr->y_max - 1;
					if (y - rc / 2 + rc > g_cave_hgt)
						y = g_cave_hgt - rc + rc / 2 + 1;
				}
			}
			if (ddx[dir] < 0)
			{
				if (widgetPtr->x_min > -1)
				{
					x = widgetPtr->x_min;
					if (x - cc / 2 < -1)
						x = cc / 2 - 1;
				}
			}
			if (ddx[dir] > 0)
			{
				if (widgetPtr->x_max - 1 < g_cave_wid)
				{
					x = widgetPtr->x_max - 1;
					if (x - cc / 2 + cc > g_cave_wid)
						x = g_cave_wid - cc + cc / 2 + 1;
				}
			}
		}

		Widget_Center(widgetPtr, y, x);

		/* Stop after first Widget */
		break;
	}
}
