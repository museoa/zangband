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

int PointInIcon(int x, int y, IconSpec *iconSpecPtr);
int HitTestTile(Widget *widgetPtr, int x, int y, int col, int row,
	int xc, int yc);

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

	return 0;
}

extern int *g_image_monster, *g_image_object;

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

