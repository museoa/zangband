/* File: widget-iso.c */

/* Purpose: Isometric stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#define HAVE_LIMITS_H
#define HAVE_UNISTD_H
#include <tkInt.h> /* This is after to avoid Unix warning about NBBY */
#include "tnb.h"
#include "util-dll.h"
#include "icon.h"
#include "widget.h"

void iso_wtd(Widget *widgetPtr, int y, int x, t_display *wtd);

int PointToTile(Widget *widgetPtr, int x, int y, int *colPtr, int *rowPtr,
	int *xcPtr, int *ycPtr);
int PointInIcon(int x, int y, IconSpec *iconSpecPtr);
int HitTestTile(Widget *widgetPtr, int x, int y, int col, int row,
	int xc, int yc);

#ifdef PIXAR

struct Pixar
{
	int xfb, xlr, y;
	int dfb, dlr, dy;
}
thePixar = {0, -16, +16, -1, 1, -1};
void Pixar_Click(Widget *widgetPtr)
{
	thePixar.xfb += thePixar.dfb;
	if (thePixar.xfb < -16)
	{
		thePixar.xfb = -15;
		thePixar.dfb = 1;
	}
	else if (thePixar.xfb > 16)
	{
		thePixar.xfb = 15;
		thePixar.dfb = -1;
	}

	thePixar.xlr += thePixar.dlr;
	if (thePixar.xlr < -16)
	{
		thePixar.xlr = -15;
		thePixar.dlr = 1;
	}
	else if (thePixar.xlr > 16)
	{
		thePixar.xlr = 15;
		thePixar.dlr = -1;
	}

	thePixar.y += thePixar.dy;
	if (thePixar.y < -16)
	{
		thePixar.y = -15;
		thePixar.dy = 1;
	}
	else if (thePixar.y > 16)
	{
		thePixar.y = 15;
		thePixar.dy = -1;
	}
}

void Pixar_Draw(Widget *widgetPtr, int x, int y)
{
	BitmapType *bmp = &widgetPtr->bitmap;
	int bypp = bmp->pixelSize;
	IconPtr dst;
	int i, j;
	char pixel[4];

	/* Yellow */
	PixelSet_RGB((IconPtr) pixel, 255, 255, 0, bypp);

	x += 16 + thePixar.xlr;
	y += 16 + thePixar.y;
	dst = bmp->pixelPtr + y * bmp->pitch + x * bypp;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			memcpy(dst + j * bmp->pixelSize, pixel, bypp);
		}
		dst += bmp->pitch;
	}
}

#endif /* PIXAR */

static void WindowToBitmap(Widget *widgetPtr, int *y, int *x)
{
	*y += widgetPtr->by;
	*x += widgetPtr->bx;
}

int HitTestTile(Widget *widgetPtr, int x, int y, int col, int row, int xc, int yc)
{
	t_display wtd;
	int yp, xp, y1, x1;
	int layer;

	/* Sanity check */
	if (col < 0 || col >= widgetPtr->cc)
		return -1;
	if (row < 0 || row >= widgetPtr->rc)
		return -1;

	/* Hit nothing when out-of-bounds */
	if (!in_bounds_test(yc, xc))
		return -1;

	(*((ExWidget *) widgetPtr)->whatToDrawProc)(widgetPtr, yc, xc, &wtd);

	/* Nothing is drawn */
	if (wtd.blank)
		return -1;

	xp = widgetPtr->xp[row * widgetPtr->cc + col];
	yp = widgetPtr->yp[row * widgetPtr->cc + col];

	WindowToBitmap(widgetPtr, &y, &x);

	/* Zero-based coords within icon */
	x1 = x - xp;
	y1 = y - yp;
	
	/* Test mon/obj/pla */
	if (wtd.fg.type != ICON_TYPE_NONE)
	{
		/* Handle left/right icons */
		for (layer = ICON_LAYER_4; layer >= ICON_LAYER_1; layer--)
		{
			t_icon_data *iconDataPtr;
			int index;
			
			if (wtd.bg[layer].type == ICON_TYPE_NONE)
				continue;

			if (wtd.bg[layer].type == ICON_TYPE_BLANK)
				break;

			iconDataPtr = &g_icon_data[wtd.bg[layer].type];
			index = wtd.bg[layer].index;
			if ((iconDataPtr->flags[index] & ICON_FLAG_LEFT) && (x1 < ISO_WID / 2))
				if (PointInIcon(x1, y1, &wtd.bg[layer]))
					return layer + 1;
			if ((iconDataPtr->flags[index] & ICON_FLAG_RIGHT) && (x1 >= ISO_WID / 2))
				if (PointInIcon(x1, y1, &wtd.bg[layer]))
					return layer + 1;
			
			break;
		}

		if (PointInIcon(x1, y1, &wtd.fg))
			return 0;
	}

	/* Test feat */
	for (layer = ICON_LAYER_4; layer >= ICON_LAYER_1; layer--)
	{
		if (wtd.bg[layer].type == ICON_TYPE_NONE)
			continue;

		if (PointInIcon(x1, y1, &wtd.bg[layer]))
			return layer + 1;
	}

	return -1;
}

int iso_hittest(Widget *widgetPtr, int x, int y, int col, int row, int *xc, int *yc)
{
	int layer, yc1, xc1;

	/* Test tile in front */
	xc1 = *xc + 1;
	yc1 = *yc + 1;
	layer = HitTestTile(widgetPtr, x, y, col, row + 2, xc1, yc1);
	if (layer != -1)
	{
		*xc = xc1;
		*yc = yc1;
		return layer;
	}

	/* Test SW tile */
	xc1 = *xc;
	yc1 = *yc + 1;
	if (row & 1)
	{
		layer = HitTestTile(widgetPtr, x, y, col, row + 1, xc1, yc1);
	}
	else
	{
		layer = HitTestTile(widgetPtr, x, y, col - 1, row + 1, xc1, yc1);
	}
	if (layer != -1)
	{
		*xc = xc1;
		*yc = yc1;
		return layer;
	}

	/* Test SE tile */
	xc1 = *xc + 1;
	yc1 = *yc;
	if (row & 1)
	{
		layer = HitTestTile(widgetPtr, x, y, col + 1, row + 1, xc1, yc1);
	}
	else
	{
		layer = HitTestTile(widgetPtr, x, y, col, row + 1, xc1, yc1);
	}
	if (layer != -1)
	{
		*xc = xc1;
		*yc = yc1;
		return layer;
	}

	/* Test hit tile */
	layer = HitTestTile(widgetPtr, x, y, col, row, *xc, *yc);

	return layer;
}

int blit(int x, int y, IconSpec iconSpec, BitmapType *bitmapPtr,
	int clip, int x1, int y1, int w1, int h1, int high)
{
	int col = 0;
	t_icon_data *iconDataPtr;
	IconPtr srcPtr, dstPtr;
	IconPtr rlePtr;
	int bypp = bitmapPtr->pixelSize;
	int pitch = bitmapPtr->pitch;
	int i, j, w, h;
	unsigned char *bounds;
	IconValue highPixel[4];

	/* Yellow */
	if (high)
		PixelSet_RGB((IconPtr) highPixel, 255, 255, 0, bypp);

	/* Ignore NONE icon */
	if (iconSpec.type == ICON_TYPE_NONE)
		return 0;

	/* Special handling of BLANK */
	if (iconSpec.type == ICON_TYPE_BLANK)
	{
		t_assign assign = g_assign[ASSIGN_FEATURE].assign[0];
		if (assign.assignType == ASSIGN_TYPE_ICON)
		{
			iconSpec.type = assign.icon.type;
			iconSpec.index = assign.icon.index;
			iconSpec.ascii = assign.icon.ascii;
			iconSpec.dark = 0;
		}
		else
		{
			iconSpec.type = ICON_TYPE_DEFAULT;
			iconSpec.index = 0;
			iconSpec.ascii = -1;
			iconSpec.dark = 0;
		}
	}

	/* Sanity check icon type */
	if ((iconSpec.type < 0) || (iconSpec.type >= g_icon_data_count))
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

		/* Access the icon type */
		iconDataPtr = &g_icon_data[iconSpec.type];
	}

	/* Sanity check draw location */
	if (x < 0 || x + iconDataPtr->width >= bitmapPtr->width) return 2;
	if (y < 0 || y + iconDataPtr->height >= bitmapPtr->height) return 3;

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
			x += 10;
			y += ISO_HGT - 32 - ISO_BOTTOM;
			h = iconDataPtr->height;
			w = iconDataPtr->width;
			srcPtr = Icon_GetAsciiData(&iconSpec, iconData);
			if (clip)
			{
				if (x + w <= x1 || x >= x1 + w1)
					return 0;
				if (y + h <= y1 || y >= y1 + h1)
					return 0;
				if (x < x1)
				{
					srcPtr += (x1 - x) * bypp;
					w -= x1 - x;
					x = x1;
				}
				if (x + w >= x1 + w1)
				{
					w -= (x + w) - (x1 + w1);
				}
				if (y < y1)
				{
					srcPtr += (y1 - y) * iconDataPtr->pitch;
					h -= y1 - y;
					y = y1;
				}
				if (y + h >= y1 + h1)
				{
					h -= (y + h) - (y1 + h1);
				}
			}
			dstPtr = bitmapPtr->pixelPtr + x * bypp + y * pitch;
			while (h--)
			{
				memcpy(dstPtr, srcPtr, w * bypp);
				if (high)
				{
					if (y & 1)
						j = (x & 1) != 0;
					else
						j = (x & 1) == 0;
					for (i = j; i < w; i += 2)
						memcpy(dstPtr + i * bypp, highPixel, bypp);
				}
				srcPtr += iconDataPtr->pitch;
				dstPtr += pitch;
				++y;
			}
			return 0;
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

	/* Not transparent */
	if (!iconDataPtr->rle_data)
	{
		if (iconDataPtr->width == 32)
		{
			x += 10;
			y += ISO_HGT - 32 - ISO_BOTTOM;
		}
		if (iconSpec.dark)
			srcPtr = iconDataPtr->dark_data[iconSpec.index] + (iconSpec.dark - 1) * iconDataPtr->length;
		else
			srcPtr = iconDataPtr->icon_data + iconDataPtr->length * iconSpec.index;
		h = iconDataPtr->height;
		w = iconDataPtr->width;
		if (clip)
		{
			if (x + w <= x1 || x >= x1 + w1)
				return 0;
			if (y + h <= y1 || y >= y1 + h1)
				return 0;
			if (x < x1)
			{
				srcPtr += (x1 - x) * bypp;
				w -= x1 - x;
				x = x1;
			}
			if (x + w >= x1 + w1)
			{
				w -= (x + w) - (x1 + w1);
			}
			if (y < y1)
			{
				srcPtr += (y1 - y) * iconDataPtr->pitch;
				h -= y1 - y;
				y = y1;
			}
			if (y + h >= y1 + h1)
			{
				h -= (y + h) - (y1 + h1);
			}
		}
		dstPtr = bitmapPtr->pixelPtr + x * bypp + y * pitch;
		while (h--)
		{
			memcpy(dstPtr, srcPtr, w * bypp);
			if (high)
			{
				if (y & 1)
					j = (x & 1) != 0;
				else
					j = (x & 1) == 0;
				for (i = j; i < w; i += 2)
					memcpy(dstPtr + i * bypp, highPixel, bypp);
			}
			srcPtr += iconDataPtr->pitch;
			dstPtr += pitch;
			++y;
		}
		return 0;
	}

	if (iconSpec.dark)
	{
		rlePtr = iconDataPtr->dark_data[iconSpec.index] + (iconSpec.dark - 1) * iconDataPtr->rle_len[iconSpec.index];
	}
	else if (iconDataPtr->dynamic)
		rlePtr = ((IconPtr *) iconDataPtr->rle_data)[iconSpec.index];
	else
		rlePtr = iconDataPtr->rle_data + iconDataPtr->rle_offset[iconSpec.index];
	bounds = iconDataPtr->rle_bounds + iconSpec.index * 4;

	x += bounds[0];
	y += bounds[1];
	w = bounds[2];
	h = bounds[3];

	if (iconDataPtr->height == 32)
	{
		x += 10;
		y -= bounds[1]; /* undo above */
		y += ISO_HGT - h - ISO_BOTTOM;
	}

	if (clip)
	{
		if (x + w <= x1 || x >= x1 + w1) return 0;
		if (y + h <= y1 || y >= y1 + h1) return 0;
	}

	dstPtr = bitmapPtr->pixelPtr + x * bypp + y * pitch;

#if 1
	if (clip)
	{
		/* Skip rows */
		while (y < y1)
		{
			unsigned int trans, opaq;

			trans = rlePtr[0];
			opaq = rlePtr[1];
			rlePtr += 2;
			col += trans;
			if (opaq)
			{
				rlePtr += opaq * bypp;
				col += opaq;
			}
			if (col == w)
			{
				++y;
				--h;
				col = 0;
				dstPtr += pitch;
			}
		}
		col = 0;
		while (y < y1 + h1)
		{
			unsigned int trans, opaq;
		
			trans = rlePtr[0];
			opaq = rlePtr[1];
			rlePtr += 2;
		
			col += trans;
		
			if (opaq)
			{
				for (i = 0; i < opaq; i++)
					if ((x + col + i >= x1) && (x + col + i < x1 + w1))
						memcpy(dstPtr + (col + i) * bypp, rlePtr + i * bypp, bypp);
				if (high)
				{
					if (y & 1)
						j = (col & 1) != 0;
					else
						j = (col & 1) == 0;
					for (i = j; i < opaq; i += 2)
						if ((x + col + i >= x1) && (x + col + i < x1 + w1))
							memcpy(dstPtr + (col + i) * bypp, highPixel, bypp);
				}
				rlePtr += opaq * bypp;
				col += opaq;
			}
			else if (!col)
				break;
		
			if (col == w)
			{
				if (!--h)
					break;
				++y;
				col = 0;
				dstPtr += pitch;
			}
		}
		return 0;
	}

	/* Not clipped */
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
			if (high)
			{
				if (y & 1)
					j = (col & 1) != 0;
				else
					j = (col & 1) == 0;
				for (i = j; i < opaq; i += 2)
					memcpy(dstPtr + (col + i) * bypp, highPixel, bypp);
			}
			rlePtr += opaq * bypp;
			col += opaq;
		}
		else if (!col)
			break;
	
		if (col == w)
		{
			if (!--h)
				break;
			++y;
			col = 0;
			dstPtr += pitch;
		}
	}

#else

	while (1)
	{
		unsigned int trans, opaq;
	
		trans = rlePtr[0];
		opaq = rlePtr[1];
		rlePtr += 2;
	
		col += trans;
	
		if (opaq)
		{
			if (clip)
			{
				if (y >= y1 && y < y1 + h1)
				{
					for (i = 0; i < opaq; i++)
						if ((x + col + i >= x1) && (x + col + i < x1 + w1))
							memcpy(dstPtr + (col + i) * bypp, rlePtr + i * bypp, bypp);
					if (high)
					{
						if (y & 1)
							j = (col & 1) != 0;
						else
							j = (col & 1) == 0;
						for (i = j; i < opaq; i += 2)
							if ((x + col + i >= x1) && (x + col + i < x1 + w1))
								memcpy(dstPtr + (col + i) * bypp, highPixel, bypp);
					}
				}
			}
			else
			{
				memcpy(dstPtr + col * bypp, rlePtr, opaq * bypp);
				if (high)
				{
					if (y & 1)
						j = (col & 1) != 0;
					else
						j = (col & 1) == 0;
					for (i = j; i < opaq; i += 2)
						memcpy(dstPtr + (col + i) * bypp, highPixel, bypp);
				}
			}
			rlePtr += opaq * bypp;
			col += opaq;
		}
		else if (!col)
			break;
	
		if (col == w)
		{
			if (!--h)
				break;
			++y;
			if (clip && (y >= y1 + h1))
				break;
			col = 0;
			dstPtr += pitch;
		}
	}
#endif /* 0 */

	return 0;
}

extern int *g_image_monster, *g_image_object;

#if 1
static bool is_unknown_floor(int y, int x, int hack)
{
	int feat;

	/* Require valid location */
	if (!in_bounds_test(y, x))
		return FALSE;

	/* Outer wall always has unknown floor */
	if (hack /* (g_grid[y][x].xtra & 0x0001) */ &&
		((y == g_cave_hgt - 1) || (x == g_cave_wid - 1)))
		return TRUE;

	feat = g_grid[y][x].f_idx;

	/* Unknown feat */
	if (!feat)
		return TRUE;

	/* This is a (known or seen) floor */
	if (feat == FEAT_FLOOR)
		return FALSE;

	/* No floor under this known/seen feat */
	if (!hack && (g_background[feat] != FEAT_FLOOR))
		return FALSE;

	/* Torch-lit */
	if (!g_grid[y][x].dark)
		return FALSE;

	/* Perma-lit */
	if (cave[y][x].info & CAVE_GLOW)
		return FALSE;

	/* Unlit floor */
	return TRUE;
}

#endif

void iso_wtd(Widget *widgetPtr, int y, int x, t_display *wtd)
{
	int m_idx, o_idx, f_idx;
	int layer;
	t_grid *gridPtr;
	int dark, daytime;
	t_assign assign;
	IconSpec iconSpec;

	if (!in_bounds_test(y, x))
	{
		wtd->blank = TRUE;
		return;
	}

	/* Access the global cave memory */
	gridPtr = &g_grid[y][x];

	/* Get the darkness factor */
	dark = gridPtr->dark;

	/* Determine if there is daylight in the town */
	daytime = !dun_level && g_daytime;
	
	m_idx = gridPtr->m_idx;
	o_idx = gridPtr->o_idx;
	f_idx = gridPtr->f_idx;

	/* The grid is completely uninteresting */
	if (!f_idx && !m_idx && !o_idx)
	{
		wtd->blank = TRUE;

		/* Done */
		return;
	}

	wtd->anim = FALSE;
	wtd->blank = FALSE;
	wtd->fg.type = ICON_TYPE_NONE;
	for (layer = 0; layer < ICON_LAYER_MAX; layer++)
	{
		wtd->bg[layer].type = ICON_TYPE_NONE;
	}

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
#ifdef ALLOW_PILE_IMAGE
	
			/* This a pile of objects */
			if (easy_floor && (gridPtr->xtra & GRID_XTRA_PILE))
			{
				/* Get the icon assigned to object-kind zero */
				assign = g_assign[ASSIGN_OBJECT].assign[0];
			}
	
			/* Not a pile */
			else
			{
				/* Get the object kind */
				int k_idx = o_list[o_idx].k_idx;
	
				/* XXX Hack -- Hallucination */
				if (p_ptr->image)
				{
					/* Get a random object kind */
					k_idx = g_image_object[k_idx];
				}
		
				/* Get the icon assigned to the object kind */
				assign = g_assign[ASSIGN_OBJECT].assign[k_idx];
			}
			
#else /* not ALLOW_PILE_IMAGE */
	
			/* Get the object kind */
			int k_idx = o_list[o_idx].k_idx;
	
			/* Get the icon assigned to the object kind */
			assign = g_assign[ASSIGN_OBJECT].assign[k_idx];
	
#endif /* not ALLOW_PILE_IMAGE */
		}

		/*
		 * Now we have the assignment for the character, monster, object or
		 * feature. For a feature, the icon index may have been adjusted
		 * for lighting. The assignment may be TYPE_ALTERNATE, TYPE_FLAVOR, or
		 * TYPE_SPRITE, which we must resolve into a "real" icon type and
		 * index (for example, the current frame of a sprite).
		 *
		 * XXX TYPE_ALTERNATE is currently used only for objects and
		 * features, but feature assignments must already be resolved
		 * in set_grid_assign(). That's why TYPE_ALTERNATE is only
		 * checked for objects (see below).
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
						wtd->anim = TRUE;
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
				wtd->anim = TRUE;
			}
		}
	
		/* */
		iconSpec.dark = 0;
		wtd->fg = iconSpec;
	}

	if (!f_idx)
	{
		wtd->bg[ICON_LAYER_1].type = ICON_TYPE_BLANK;
		return;
	}

	/* Delayed update of g_icon_map[] */
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

#if 0
if (!daytime && !dark && (g_grid[y][x].xtra & GRID_XTRA_WALL) &&
	(g_grid[y][x].shape != GRID_SHAPE_SINGLE))
{
	if (py < y && px < x)
		dark = 2;
}
#endif

	/*
	 * Get the assignment from the global assign map. The g_icon_map[]
	 * array allows us to use different assignments for the same
	 * feature index. For example, doors may be vertical or
	 * horizontal, and some walls are actually pillars.
	 */
	for (layer = 0; layer < ICON_LAYER_MAX; layer++)
	{
		assign = g_icon_map[layer][y][x];

#if 1
		if (!layer)
		{
			if ((g_grid[y][x].xtra & 0x0001) &&
				(is_unknown_floor(y, x, TRUE) ||
				is_unknown_floor(y+1, x, FALSE) ||
				is_unknown_floor(y, x+1, FALSE)))
			{
				assign = g_assignshape[g_grid[y][x].shape][max_f_idx + f_idx];
			}
			else if (is_unknown_floor(y, x, FALSE))
			{
				assign = g_assign[ASSIGN_FEATURE].assign[FEAT_NONE];
			}
		}
#else
		/*
		 * XXX Hack -- There is a second assignment for this feature
		 * with an "unknown" floor.
		 */
		if (!layer && (g_grid[y][x].xtra & 0x0001) &&
			(((y == g_cave_hgt - 1) || !g_grid[y+1][x].f_idx) ||
			((x == g_cave_wid - 1) || !g_grid[y][x+1].f_idx)))
		{
			assign = g_assignshape[g_grid[y][x].shape][max_f_idx + f_idx];
		}

		/*
		 * XXX Hack -- If floor is not lit, use "unknown" floor.
		 */
		else if (!layer && (f_idx != FEAT_FLOOR) &&
			(g_background[f_idx] == FEAT_FLOOR) &&
			(g_grid[y][x].dark && !(cave[y][x].info & CAVE_GLOW)))
		{
			assign = g_assign[ASSIGN_FEATURE].assign[FEAT_NONE];
		}
#endif

		/* Resolve sprite */
		if (assign.assignType == ASSIGN_TYPE_SPRITE)
		{
			/* Access the sprite */
			t_sprite *spritePtr = &g_sprite[assign.sprite.index];
		
			/* Get the type and index of the current frame */
			iconSpec = spritePtr->icon[spritePtr->frame];
		
			/* This grid is animated */
			wtd->anim = TRUE;
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
				wtd->bg[layer] = iconSpec;
				break;
			}

			/* XXX Hack -- Multi-hued ascii icons are animated */
			if (iconSpec.ascii != -1)
			{
				if (g_ascii[iconSpec.ascii].mode != ASCII_NORMAL)
				{
					/* This grid is animated */
					wtd->anim = TRUE;
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
						if (assign.icon.type > ICON_TYPE_DEFAULT)
							iconSpec.index += dark;
					}
					break;

				/* Use tint table for lighting (slow) */
				case FT_LIGHT_TINT:
					if (g_icon_depth == 8)
						wtd->tint[1] = g_darken[dark-1].table;
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
			iconSpec.dark = 0;
		wtd->bg[layer] = iconSpec;
	}
}

int PointInIcon(int x, int y, IconSpec *iconSpecPtr)
{
	t_icon_data *iconDataPtr;
	IconPtr rlePtr;
	int w, h, row = 0;
	int col = 0;
	int bypp;
	unsigned char *bounds;

	if ((iconSpecPtr->type < 0) || (iconSpecPtr->type >= g_icon_data_count))
		return 0;

	iconDataPtr = &g_icon_data[iconSpecPtr->type];

	if ((iconSpecPtr->index < 0) || (iconSpecPtr->index >= iconDataPtr->icon_count))
		return 0;

	w = iconDataPtr->width;
	h = iconDataPtr->height;

	if (iconDataPtr->rle_data)
	{
		bounds = iconDataPtr->rle_bounds + iconSpecPtr->index * 4;
		x -= bounds[0];
		y -= bounds[1];
		w = bounds[2];
		h = bounds[3];

		if (iconDataPtr->width == 32)
		{
			y += bounds[1];
		}
	}

	if (iconDataPtr->width == 32)
	{
		x -= 10;
		y -= ISO_HGT - h - ISO_BOTTOM;
	}

	if ((x < 0) || (x >= w))
		return 0;
	if ((y < 0) || (y >= h))
		return 0;

	if (!iconDataPtr->rle_data)
		return 1;

	bypp = iconDataPtr->bypp;

	if (iconDataPtr->dynamic)
		rlePtr = ((IconPtr *) iconDataPtr->rle_data)[iconSpecPtr->index];
	else
		rlePtr = iconDataPtr->rle_data +
			iconDataPtr->rle_offset[iconSpecPtr->index];

	while (1)
	{
		unsigned int trans, opaq;
	
		trans = rlePtr[0];
		opaq = rlePtr[1];
		rlePtr += 2;

		if (y == row)
		{
			if (x < col + trans)
				return 0;
			if (x < col + trans + opaq)
				return 1;
		}
		
		col += trans;
	
		if (opaq)
		{
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
			++row;
		}
	}

	return 0;
}

void iso_draw_all(Widget *widgetPtr)
{
	t_display wtd;
	int tile, y, x, yp, xp, yt, xt;
	int rc = widgetPtr->rc;
	int cc = widgetPtr->cc;
	short *pinfo = widgetPtr->info;
	int *pyo = widgetPtr->yo;
	int *pxo = widgetPtr->xo;
	int *pyp = widgetPtr->yp;
	int *pxp = widgetPtr->xp;
	IconSpec *peffect = ((ExWidget *) widgetPtr)->effect;
	IconSpec iconSpec, darkIconSpec;
IconSpec bfSpec;
int bfHigh;
	BitmapType *bitmapPtr = &widgetPtr->bitmap;
	void (*wtdProc)(Widget *, int, int, t_display *) =
		((ExWidget *) widgetPtr)->whatToDrawProc;
	int layer, high;
	DoubleLink *link;


	if (widgetPtr->flags & WIDGET_NO_UPDATE)
		return;

#if 0
	/* Paint to catch errors */
	IconPtr dst = bitmapPtr->pixelPtr;
	for (y = 0; y < bitmapPtr->height; y++)
	{
		for (x = 0; x < bitmapPtr->width; x++)
			dst[x] = 10;
		dst += bitmapPtr->pitch;
	}
#endif

#ifdef PIXAR
	Pixar_Click(widgetPtr);
#endif

	widgetPtr->animCnt = 0;

	/* Get cave coords of top-left tile */
	yt = widgetPtr->y0;
	xt = widgetPtr->x0;

	/* Darkness */
	darkIconSpec.type = ICON_TYPE_BLANK;

	/* Check every tile */
	for (tile = 0; tile < cc * rc; tile++)
	{
		if (pinfo[tile] & WIDGET_INFO_IGNORE)
			continue;

		pinfo[tile] &= ~(WIDGET_INFO_DIRTY | WIDGET_INFO_ANIM);

		/* Calculate cave coords */
		y = yt + pyo[tile];
		x = xt + pxo[tile];

		/* Get pixel offsets to draw tile */
		yp = pyp[tile];
		xp = pxp[tile];

		(*wtdProc)(widgetPtr, y, x, &wtd);

		/* Unknown grid */
		if (wtd.blank)
		{
			/* Darkness */
			high = FALSE;
			if (widgetPtr->hit == 1 && widgetPtr->hitx == x && widgetPtr->hity == y)
				high = TRUE;
			blit(xp, yp, darkIconSpec, bitmapPtr, FALSE, 0, 0, 0, 0, high);

			/* Next cave location */
			continue;
		}

		if (wtd.anim)
		{
			pinfo[tile] |= WIDGET_INFO_ANIM;
			widgetPtr->anim[widgetPtr->animCnt++] = tile;
		}

		bfSpec.type = ICON_TYPE_NONE;
		bfHigh = FALSE;

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
			high = FALSE;
			if ((widgetPtr->hit == (layer + 1)) && (widgetPtr->hitx == x) && (widgetPtr->hity == y))
				high = TRUE;
			blit(xp, yp, iconSpec, bitmapPtr, FALSE, 0, 0, 0, 0, high);

			/* Stop at BLANK icon */
			if (iconSpec.type == ICON_TYPE_BLANK)
				break;

			/* This icon has 2 halves */
			if (g_icon_data[iconSpec.type].flags[iconSpec.index] & (ICON_FLAG_LEFT | ICON_FLAG_RIGHT))
			{
				bfSpec = iconSpec;
				bfHigh = high;
			}
		}

		/* Draw foreground icon */
		if (wtd.fg.type != ICON_TYPE_NONE)
		{
#ifdef PIXAR
			if (i == widgetPtr->centerTile && thePixar.xfb <= 0)
				Pixar_Draw(widgetPtr, xp + 10, yp + 7);
#endif
			high = FALSE;
			if (widgetPtr->hit == 0 && widgetPtr->hitx == x && widgetPtr->hity == y)
				high = TRUE;
			blit(xp, yp, wtd.fg, bitmapPtr, FALSE, 0, 0, 0, 0, high);
#ifdef PIXAR
			if (i == widgetPtr->centerTile && thePixar.xfb > 0)
				Pixar_Draw(widgetPtr, xp + 10, yp + 7);
#endif
		}

		/* Draw effect */
		iconSpec = peffect[tile];
		if (iconSpec.type != ICON_TYPE_NONE)
		{
			blit(xp, yp, iconSpec, bitmapPtr, FALSE, 0, 0, 0, 0, FALSE);
		}

		/* This icon has 2 halves */
		if (bfSpec.type != ICON_TYPE_NONE)
		{
			if ((wtd.fg.type != ICON_TYPE_NONE) || (iconSpec.type != ICON_TYPE_NONE))
			{
				if (g_icon_data[bfSpec.type].flags[bfSpec.index] & ICON_FLAG_LEFT)
					blit(xp, yp, bfSpec, bitmapPtr, TRUE,
						xp, yp, ISO_WID / 2, ISO_HGT, bfHigh);
				else if (g_icon_data[bfSpec.type].flags[bfSpec.index] & ICON_FLAG_RIGHT)
					blit(xp, yp, bfSpec, bitmapPtr, TRUE,
						xp + ISO_WID / 2, yp, ISO_WID / 2, ISO_HGT, bfHigh);
			}
		}
	}

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

#if 0
	/* horizontal line 1/2 height */
	y = widgetPtr->by + widgetPtr->height / 2 - 1;
	dst = widgetPtr->bitmap.pixelPtr + y * widgetPtr->bitmap.pitch;
	for (x = widgetPtr->bx; x < widgetPtr->bx + widgetPtr->width; x++)
		dst[x] = 10;

	/* vertical line 1/2 width */
	x = widgetPtr->bx + widgetPtr->width / 2 - 1;
	dst = widgetPtr->bitmap.pixelPtr + x * widgetPtr->bitmap.pixelSize;
	for (y = widgetPtr->by; y < widgetPtr->by + widgetPtr->height; y++)
		*(dst + y * widgetPtr->bitmap.pitch) = 10;

	/* vertical line -1 */
	x = widgetPtr->bx - 1;
	dst = widgetPtr->bitmap.pixelPtr + x * widgetPtr->bitmap.pixelSize;
	for (y = widgetPtr->by; y < widgetPtr->by + widgetPtr->height; y++)
		*(dst + y * widgetPtr->bitmap.pitch) = 14;
#endif
}

int sectrect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2, int b[4])
{
	int y, x, x3, y3, x4, y4;

	x3 = x1 + w1;
	y3 = y1 + h1;
	x4 = x2 + w2;
	y4 = y2 + h2;

	b[0] = (x1 < x2) ? x2 : x1;
	b[1] = (y1 < y2) ? y2 : y1;
	x = (x3 < x4) ? x3 : x4;
	y = (y3 < y4) ? y3 : y4;
	b[2] = x - b[0];
	b[3] = y - b[1];

	return (b[2] > 0 && b[3] > 0);
}

void DrawClipped(Widget *widgetPtr, int x, int y, int w, int h,
	int tile, int yc, int xc)
{
	t_display wtd;
	int xp, yp;
	int rc = widgetPtr->rc;
	int cc = widgetPtr->cc;
	IconSpec iconSpec, darkIconSpec;
IconSpec bfSpec;
	BitmapPtr bitmapPtr = &widgetPtr->bitmap;
	void (*wtdProc)(Widget *, int, int, t_display *) =
		((ExWidget *) widgetPtr)->whatToDrawProc;
	int layer;

	if (tile < 0 || tile >= rc * cc)
		return;
	if (widgetPtr->info[tile] & WIDGET_INFO_IGNORE)
		return;

	/* Darkness */
	darkIconSpec.type = ICON_TYPE_BLANK;

	/* Tile is no longer dirty */
	widgetPtr->info[tile] &= ~WIDGET_INFO_DIRTY;

	/* Get pixel offsets to draw tile */
	yp = widgetPtr->yp[tile];
	xp = widgetPtr->xp[tile];

if ((yp + ISO_HGT <= y) || (yp >= y + h)) return;
if ((xp + ISO_WID <= x) || (xp >= x + w)) return;

	(*wtdProc)(widgetPtr, yc, xc, &wtd);

	/* Unknown grid */
	if (wtd.blank)
	{
		/* Darkness */
		blit(xp, yp, darkIconSpec, bitmapPtr, TRUE, x, y, w, h, FALSE);

		/* Next cave location */
		return;
	}

	if (wtd.anim && !(widgetPtr->info[tile] & WIDGET_INFO_ANIM))
	{
		widgetPtr->info[tile] |= WIDGET_INFO_ANIM;
		widgetPtr->anim[widgetPtr->animCnt++] = tile;
	}

bfSpec.type = ICON_TYPE_NONE;

	/*
	 * Draw 1-4 background layers.
	 */
	for (layer = 0; layer < ICON_LAYER_MAX; layer++)
	{
		iconSpec = wtd.bg[layer];

		/* Stop at NONE icon */
		if (iconSpec.type == ICON_TYPE_NONE)
			break;

		/* FIXME: tint */

		/* Draw background icon */
		blit(xp, yp, iconSpec, bitmapPtr, TRUE, x, y, w, h, FALSE);

		/* Stop at BLANK icon */
		if (iconSpec.type == ICON_TYPE_BLANK)
			break;

if (g_icon_data[iconSpec.type].flags[iconSpec.index] & (ICON_FLAG_LEFT | ICON_FLAG_RIGHT))
	bfSpec = iconSpec;
	}

	/* Draw foreground icon */
	if (wtd.fg.type != ICON_TYPE_NONE)
	{
		blit(xp, yp, wtd.fg, bitmapPtr, TRUE, x, y, w, h, FALSE);
	}

	/* Draw effect */
	iconSpec = ((ExWidget *) widgetPtr)->effect[tile];
	if (iconSpec.type != ICON_TYPE_NONE)
	{
		blit(xp, yp, iconSpec, bitmapPtr, TRUE, x, y, w, h, FALSE);
	}

if (bfSpec.type != ICON_TYPE_NONE)
{
	if ((wtd.fg.type != ICON_TYPE_NONE) || (iconSpec.type != ICON_TYPE_NONE))
	{
		if (g_icon_data[bfSpec.type].flags[bfSpec.index] & ICON_FLAG_LEFT)
		{
			int b[4];
			if (sectrect(x, y, w, h, xp, yp, ISO_WID / 2, ISO_HGT, b))
				blit(xp, yp, bfSpec, bitmapPtr, TRUE, b[0], b[1], b[2], b[3], FALSE);
		}
		else if (g_icon_data[bfSpec.type].flags[bfSpec.index] & ICON_FLAG_RIGHT)
		{
			int b[4];
			if (sectrect(x, y, w, h, xp + ISO_WID / 2, yp, ISO_WID / 2, ISO_HGT, b))
				blit(xp, yp, bfSpec, bitmapPtr, TRUE,  b[0], b[1], b[2], b[3], FALSE);
		}
	}
}
}

void iso_draw_invalid(Widget *widgetPtr)
{
	int tile, i, yp, xp, yt, xt;
	int rc = widgetPtr->rc;
	int cc = widgetPtr->cc;
	int *pyo = widgetPtr->yo;
	int *pxo = widgetPtr->xo;
	int *pyp = widgetPtr->yp;
	int *pxp = widgetPtr->xp;
	int *pinvalid = widgetPtr->invalid;
	int dl, dt, dr, db;
	int by, bx;
	int row, col;
	DoubleLink *link;

	if (widgetPtr->flags & WIDGET_NO_UPDATE)
		return;

	/* Keep track of dirty area */
	dl = widgetPtr->bitmap.width;
	dt = widgetPtr->bitmap.height;
	dr = 0;
	db = 0;
			
	/* Clear the "animation" flag */
	/* Don't clear, because only some grids drawn */
/*	widgetPtr->animCnt = 0 */

	/* Get cave coords of top-left tile */
	yt = widgetPtr->y0;
	xt = widgetPtr->x0;
	
	for (i = 0; i < widgetPtr->invalidCnt; i++)
	{
		tile = pinvalid[i];

		/* Pixel offsets in bitmap */
		xp = pxp[tile];
		yp = pyp[tile];

#if 1
	if (debug_widgets & DEBUG_WIDGET_DRAW)
	{
		/* Paint to catch errors */
		IconPtr dst = widgetPtr->bitmap.pixelPtr + yp * widgetPtr->bitmap.pitch +
			xp * widgetPtr->bitmap.pixelSize;
		int y2, x2;
		for (y2 = 0; y2 < ISO_HGT; y2++)
		{
			for (x2 = 0; x2 < ISO_WID * widgetPtr->bitmap.pixelSize; x2++)
				dst[x2] = 10;
			dst += widgetPtr->bitmap.pitch;
		}
	}
#endif

		/* Dirty bounds */
		if (xp < dl)
			dl = xp;
		if (yp < dt)
			dt = yp;
		if (xp + ISO_WID - 1 > dr)
			dr = xp + ISO_WID - 1;
		if (yp + ISO_HGT - 1 > db)
			db = yp + ISO_HGT - 1;

		for (row = (tile / cc) - 3; row <= (tile / cc) + 3; row++)
		{
			if (row < 0 || row >= rc) continue;
			for (col = (tile % cc) - 1; col <= (tile % cc) + 1; col++)
			{
				int tile2 = row * cc + col;
				if (col < 0 || col >= cc) continue;
				DrawClipped(widgetPtr, xp, yp, ISO_WID, ISO_HGT,
					tile2, yt + pyo[tile2], xt + pxo[tile2]);
			}
		}
	}

	widgetPtr->invalidCnt = 0;

	/* Now draw all of the items for this Widget */
	by = widgetPtr->by;
	bx = widgetPtr->bx;
	for (link = widgetPtr->linkerItemVis.head; 1 && link; link = link->next)
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

