/* File: icon-dll.c */

/* Purpose: icon stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */
#include "tnb.h"
#include "icon-dll.h"
#include "plat-dll.h"


#include <limits.h>
#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif

int PixelPtrToLong(IconPtr p, int bypp);
void PixelLongToPtr(IconPtr dst, int pixel, int bypp);

static int init_ascii_data(Tcl_Interp *interp, t_icon_data *icon_data_ptr);

t_icon_data *g_icon_data; /* Array of icon types */
int g_icon_data_count = 0; /* Number of icon types */
Tcl_HashTable g_icon_table; /* Hash table for icon types */
long g_icon_length = 0; /* Length in bytes of one icon */
int g_icon_size = 0; /* Icon dimensions (16, 24 or 32) */
int g_icon_depth = 0; /* Icon depth (8, 16 or 24 bpp) */
t_ascii *g_ascii; /* Array of ascii info */
int g_ascii_count;  /* Number of elems in g_ascii[] array */
int g_pixel_size; /* Num bytes per pixel (1, 2, 3 or 4) */
int g_icon_pixels; /* Num pixels per icon (16x16, 24x24, 32x32) */
int g_icon_style; /* ICON_STYLE_xxx */

unsigned char *g_palette_rgb;

/* Hack -- Standard 16 "term" colors. User should be able to change */
IconValue g_term_palette[16] = {255, 0, 250, 17, 217, 196, 199, 101, 129,
	247, 30, 5, 35, 185, 180, 52};

/* Actual 8/16/24 pixel values for above */
unsigned long g_term_colormap[16];

#define USE_COMPRESS
#define USE_ZLIB

#ifdef USE_COMPRESS

#ifdef USE_ZLIB

#include "zlib.h"

#define BUFLEN 16384

static int CompressIconFile(Tcl_Interp *interp, char *fileName, t_icon_data *idp)
{
	gzFile out;
	long sum = 0, dataSize;
	int err, result = TCL_OK;

	out = gzopen(fileName, "wb6");
	if (out == NULL)
	{
		Tcl_AppendResult(interp, "couldn't open \"", fileName, "\"", NULL);
		return TCL_ERROR;
	}

	if (gzwrite(out, (char *) "tnbz", 4) != 4)
	{
		Tcl_AppendResult(interp, "error writing header", NULL);
		result = TCL_ERROR;
		goto cleanup;
	}

	if (gzwrite(out, &idp->depth, sizeof(int)) != sizeof(int))
	{
		Tcl_AppendResult(interp, "error writing depth", NULL);
		result = TCL_ERROR;
		goto cleanup;
	}
	if (gzwrite(out, &idp->bypp, sizeof(int)) != sizeof(int))
	{
		Tcl_AppendResult(interp, "error writing pixel-size", NULL);
		result = TCL_ERROR;
		goto cleanup;
	}
	if (gzwrite(out, &idp->width, sizeof(int)) != sizeof(int))
	{
		Tcl_AppendResult(interp, "error writing width", NULL);
		result = TCL_ERROR;
		goto cleanup;
	}
	if (gzwrite(out, &idp->height, sizeof(int)) != sizeof(int))
	{
		Tcl_AppendResult(interp, "error writing height", NULL);
		result = TCL_ERROR;
		goto cleanup;
	}
	if (gzwrite(out, &idp->icon_count, sizeof(int)) != sizeof(int))
	{
		Tcl_AppendResult(interp, "error writing count", NULL);
		result = TCL_ERROR;
		goto cleanup;
	}

	dataSize = idp->icon_count * idp->length;
	if (gzwrite(out, &dataSize, sizeof(long)) != sizeof(long))
	{
		Tcl_AppendResult(interp, "error writing file size", NULL);
		result = TCL_ERROR;
		goto cleanup;
	}

	while (1)
	{
		int len = idp->length * 16L;
		if (len > (dataSize - sum))
			len = dataSize - sum;
		if (gzwrite(out, idp->icon_data + sum, len) != len)
		{
			(void) gzerror(out, &err);
			Tcl_AppendResult(interp,
				format("gzwrite() failed: err %d", err), NULL);
			result = TCL_ERROR;
			break;
		}
		sum += len;
		if (sum >= dataSize) break;
	}

cleanup:
	if (gzclose(out) != Z_OK)
	{
		(void) gzerror(out, &err);
		Tcl_AppendResult(interp, format("gzclose() failed: err %d", err),
			NULL);
		result = TCL_ERROR;
	}

	return result;
}

static int DecompressIconFile(Tcl_Interp *interp, char *fileName, IconPtr *iconData,
	int *iconCount)
{
	gzFile in;
	char *buf = NULL;
	char header[4];
	int len;
	int err;
	long sum = 0, size;
	int depth, bypp, width, height, count;
	int length;

	in = gzopen(fileName, "rb");
	if (in == NULL)
	{
		Tcl_AppendResult(interp, "couldn't open \"", fileName, "\"",
			NULL);
		return TCL_ERROR;
	}

	len = gzread(in, header, 4);
	if ((len != 4) || strncmp(header, "tnbz", 4))
	{
		Tcl_AppendResult(interp, "error reading header", NULL);
		goto error;
	}

	len = gzread(in, &depth, sizeof(int));
	if ((len != sizeof(int)) || (depth != g_icon_depth))
	{
		Tcl_AppendResult(interp, "invalid depth", NULL);
		goto error;
	}

	len = gzread(in, &bypp, sizeof(int));
	if ((len != sizeof(int)) || (bypp != g_pixel_size))
	{
		Tcl_AppendResult(interp, "invalid pixel-size", NULL);
		goto error;
	}

	len = gzread(in, &width, sizeof(int));
	if ((len != sizeof(int)) || (width <= 0))
	{
		Tcl_AppendResult(interp, "invalid width", NULL);
		goto error;
	}

	len = gzread(in, &height, sizeof(int));
	if ((len != sizeof(int)) || (height <= 0))
	{
		Tcl_AppendResult(interp, "invalid height", NULL);
		goto error;
	}

	length = width * height * bypp;

	len = gzread(in, &count, sizeof(int));
	if ((len != sizeof(int)) || (count < 0))
	{
		Tcl_AppendResult(interp, "invalid count", NULL);
		goto error;
	}

	len = gzread(in, &size, sizeof(long));
	if ((len != sizeof(long)) || (size < 0) || (size > 10L * 1024L * 1024L)
		|| ((size / length) != count))
	{
		Tcl_AppendResult(interp, "invalid data size", NULL);
		goto error;
	}
	
	buf = Tcl_AllocDebug(size);

	while (sum < size)
	{
		len = gzread(in, buf + sum, length * 16L);
		(void) gzerror(in, &err);
		if (sum + len == size) err = 0;
		if ((len < 0) || (err < 0))
		{
			(void) gzerror(in, &err);
			Tcl_AppendResult(interp,
				format("gzread() failed: err %d", err), NULL);
			goto error;
		}
		if (len == 0)
			break;
		sum += len;
	}

	if (sum != size)
	{
		Tcl_AppendResult(interp, "file is truncated", NULL);
		goto error;
	}

	/* gzclose() fails sometimes, even though the whole file was read */
	if ((gzclose(in) != Z_OK) && 0)
	{
		Tcl_FreeDebug(buf);
		(void) gzerror(in, &err);
		Tcl_AppendResult(interp, format("gzclose() failed: err %d", err),
			NULL);

		/* Failure */
		return TCL_ERROR;
	}

	(*iconData) = (IconPtr) buf;
	(*iconCount) = size / length;

	/* Success */
	return TCL_OK;

error:
	if (buf)
		Tcl_FreeDebug(buf);
	if (gzclose(in) != Z_OK)
	{
	}

	/* Failure */
	return TCL_ERROR;
}

#endif /* USE_ZLIB */

#endif /* USE_COMPRESS */

/* RL_Encode(), but discard results (return len only) */
int RL_Len(int w, int h, int bypp, IconPtr srcbuf, int pitch, int key)
{
	int y;
	int total = 0, lastline = 0;

	for (y = 0; y < h; y++)
	{
		int x = 0;
		int blankline = 0;

		do
		{
			int opaq, trans;
			int opaqstart;
			int transstart = x;

			while ((x < w) && (PixelPtrToLong(srcbuf + x * bypp, bypp) == key))
				x++;
			opaqstart = x;
			while ((x < w) && (PixelPtrToLong(srcbuf + x * bypp, bypp) != key))
				x++;
			trans = opaqstart - transstart;
			if (trans == w)
				blankline = 1;
			opaq = x - opaqstart;

			total += 2;

			if (opaq)
			{
				total += opaq * bypp;
			}

			if (!blankline)
				lastline = total;

		} while (x < w);

		srcbuf += pitch;
	}

	total = lastline + 2;

	return total;
}

int RL_Encode(int w, int h, int bypp, IconPtr srcbuf, int pitch, int key, IconPtr rlePtr)
{
	int y;
	IconPtr dst, lastline;

	dst = rlePtr;
	lastline = dst;

	for (y = 0; y < h; y++)
	{
		int x = 0;
		int blankline = 0;

		do
		{
			int opaq, trans;
			int opaqstart;
			int transstart = x;

			while ((x < w) && (PixelPtrToLong(srcbuf + x * bypp, bypp) == key))
				x++;
			opaqstart = x;
			while ((x < w) && (PixelPtrToLong(srcbuf + x * bypp, bypp) != key))
				x++;
			trans = opaqstart - transstart;
			if (trans == w)
				blankline = 1;
			opaq = x - opaqstart;

			dst[0] = trans;
			dst[1] = opaq;
			dst += 2;

			if (opaq)
			{
				memcpy(dst, srcbuf + opaqstart * bypp, opaq * bypp);
				dst += opaq * bypp;
			}

			if (!blankline)
				lastline = dst;

		} while (x < w);

		srcbuf += pitch;
	}

	dst = lastline;
	dst[0] = 0;
	dst[1] = 0;
	dst += 2;

	return dst - rlePtr;
}

int RL_Decode(int w, int h, int bypp, IconPtr rlePtr, IconPtr dst, int pitch)
{
	int offset = 0;
	int total = 0;

	while (1)
	{
		unsigned int trans, opaq;

		trans = rlePtr[0];
		opaq = rlePtr[1];
		rlePtr += 2;

		offset += trans;

		if (opaq)
		{
			memcpy(dst + offset * bypp, rlePtr, opaq * bypp);
			rlePtr += opaq * bypp;
			offset += opaq;
		}
		else if (!offset)
			break;

		total += trans + opaq;

		if (offset == w)
		{
			offset = 0;
			dst += pitch;
			if (!--h)
				break;
		}
	}
	return total;
}

void RL_Bounds(int w, int h, int bypp, IconPtr srcbuf, int key, unsigned char *bounds)
{
	int left, top, right, bottom;
	int x, y;

	left = 255;
	right = 0;
	top = 255;
	bottom = 0;

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			if (PixelPtrToLong(srcbuf + x * bypp, bypp) != key)
			{
				if (x < left)
					left = x;
				if (x > right)
					right = x;
				if (y < top)
					top = y;
				if (y > bottom)
					bottom = y;
			}
		}
		srcbuf += w * bypp;
	}

	if (!right)
		left = 0;
	if (!bottom)
		top = 0;

	bounds[0] = left;
	bounds[1] = top;
	bounds[2] = right - left + 1;
	bounds[3] = bottom - top + 1;
}

void Icon_MakeRLEBounds(t_icon_data *iconDataPtr, int i)
{
	int left, top, right, bottom;
	int bypp = iconDataPtr->bypp;
	int x, y;
	IconPtr srcbuf = iconDataPtr->icon_data + i * iconDataPtr->length;
	unsigned char *bounds;

	left = 255;
	right = 0;
	top = 255;
	bottom = 0;

	for (y = 0; y < iconDataPtr->height; y++)
	{
		for (x = 0; x < iconDataPtr->width; x++)
		{
			if (PixelPtrToLong(srcbuf + x * bypp, bypp) != iconDataPtr->rle_pixel)
			{
				if (x < left)
					left = x;
				if (x > right)
					right = x;
				if (y < top)
					top = y;
				if (y > bottom)
					bottom = y;
			}
		}
		srcbuf += iconDataPtr->pitch;
	}

	if (!right)
		left = 0;
	if (!bottom)
		top = 0;

	bounds = iconDataPtr->rle_bounds + i * 4;
	bounds[0] = left;
	bounds[1] = top;
	bounds[2] = right - left + 1;
	bounds[3] = bottom - top + 1;
}

void Icon_MakeRLE(t_icon_data *iconDataPtr)
{
	int i, len, slop;
	long total;

	total = 0;

	/* x, y, width, height for each icon */
	iconDataPtr->rle_bounds =
		(unsigned char *) Tcl_AllocDebug(sizeof(unsigned char) *
		iconDataPtr->icon_count * 4);

	for (i = 0; i < iconDataPtr->icon_count; i++)
	{
		IconPtr iconPtr = iconDataPtr->icon_data + i * iconDataPtr->length;
		unsigned char *bounds = iconDataPtr->rle_bounds + i * 4;

		Icon_MakeRLEBounds(iconDataPtr, i);

		iconPtr += bounds[0] * iconDataPtr->bypp +
			bounds[1] * iconDataPtr->pitch;

		/* Get length of encoded data */
		len = RL_Len(bounds[2], bounds[3], iconDataPtr->bypp,
			iconPtr, iconDataPtr->pitch, iconDataPtr->rle_pixel);

		total += len;
	}

	/* RL_Encode() can write past 'len' by a number of transparent lines */
	if (iconDataPtr->bypp == 4)
		slop = iconDataPtr->height * 4;
	else
		slop = iconDataPtr->height * 2;

	iconDataPtr->rle_data = (IconPtr) Tcl_AllocDebug(total + slop);
	iconDataPtr->rle_offset =
		(long *) Tcl_AllocDebug(sizeof(long) * iconDataPtr->icon_count);
	iconDataPtr->rle_len =
		(int *) Tcl_AllocDebug(sizeof(int) * iconDataPtr->icon_count);

	total = 0;

	for (i = 0; i < iconDataPtr->icon_count; i++)
	{
		IconPtr iconPtr = iconDataPtr->icon_data + i * iconDataPtr->length;
		unsigned char *bounds = iconDataPtr->rle_bounds + i * 4;

		iconPtr += bounds[0] * iconDataPtr->bypp +
			bounds[1] * iconDataPtr->pitch;

		/* Encode */
		len = RL_Encode(bounds[2], bounds[3], iconDataPtr->bypp,
			iconPtr, iconDataPtr->pitch, iconDataPtr->rle_pixel,
			iconDataPtr->rle_data + total);
	
		iconDataPtr->rle_offset[i] = total;
		iconDataPtr->rle_len[i] = len;

		total += len;
	}

	Tcl_FreeDebug((char *) iconDataPtr->icon_data);
	iconDataPtr->icon_data = NULL;
}

static int InitPixelSize(Tcl_Interp *interp)
{
	BitmapType bitmap;

	bitmap.width = bitmap.height = 10;
	bitmap.depth = g_icon_depth;
	Bitmap_New(interp, &bitmap);
	g_pixel_size = bitmap.pixelSize;
	Bitmap_Delete(&bitmap);

	return TCL_OK;
}

RGBInfo g_rgbi;

static int CountBits(unsigned long mask)
{
	int n;

	for (n = 0; mask != 0; mask &= mask - 1)
		n++;
	return n;
}

static void InitRGBInfo(Tcl_Interp *interp)
{
	Tk_Window tkwin = Tk_MainWindow(interp);
	Visual *visual = Tk_Visual(tkwin);

	g_rgbi.red_mask = visual->red_mask;
	g_rgbi.green_mask = visual->green_mask;
	g_rgbi.blue_mask = visual->blue_mask;

#ifdef PLATFORM_WIN
	/* XXX Always 5-5-5 */
	g_rgbi.red_mask = 0x7c00;
	g_rgbi.green_mask = 0x03e0;
	g_rgbi.blue_mask = 0x001f;
#endif

	g_rgbi.red_count = CountBits(g_rgbi.red_mask);
	g_rgbi.green_count = CountBits(g_rgbi.green_mask);
	g_rgbi.blue_count = CountBits(g_rgbi.blue_mask);

	g_rgbi.red_shift = g_rgbi.red_count + g_rgbi.green_count + g_rgbi.blue_count - 8;
	g_rgbi.green_shift = g_rgbi.green_count + g_rgbi.blue_count - 8;
	g_rgbi.blue_shift = -(g_rgbi.blue_count - 8);

	g_rgbi.extra = ~(g_rgbi.red_mask | g_rgbi.green_mask | g_rgbi.blue_mask);
}

static void GetPix8(unsigned char *p, int *r, int *g, int *b)
{
	unsigned char *rgbPtr = &g_palette_rgb[*p * 3]; /* FIXME: colormap */
	*r = *rgbPtr++;
	*g = *rgbPtr++;
	*b = *rgbPtr++;
}

static void SetPix8(unsigned char *p, int r, int g, int b)
{
	/* NOTE: Not Colormap */
	*p = Palette_RGB2Index(r, g, b);
}

void GetPix16(unsigned char *p, int *r, int *g, int *b)
{
	unsigned short pix16 = *(unsigned short *) p;
	*r = (pix16 & g_rgbi.red_mask) >> g_rgbi.red_shift;
	*g = (pix16 & g_rgbi.green_mask) >> g_rgbi.green_shift;
	*b = (pix16 & g_rgbi.blue_mask) << g_rgbi.blue_shift;
}

void SetPix16(unsigned char *p, int r, int g, int b)
{
	int r2 = (r << g_rgbi.red_shift) & g_rgbi.red_mask;
	int g2 = (g << g_rgbi.green_shift) & g_rgbi.green_mask;
	int b2 = (b >> g_rgbi.blue_shift) & g_rgbi.blue_mask;

	*((unsigned short *) p) = g_rgbi.extra | r2 | g2 | b2;
}

static void GetPix24(unsigned char *p, int *r, int *g, int *b)
{
	*b = *p++;
	*g = *p++;
	*r = *p++;
}

#if 1
static void SetPix24(unsigned char *p, int r, int g, int b)
{
	*p++ = b;
	*p++ = g;
	*p++ = r;
	if (g_pixel_size == 4)
		*p++ = 0xFF;
}
#else
#define SetPix24(p,r,g,b) \
	(p)[0] = b; (p)[1] = g; (p)[2] = r;
#endif

void PixelSet_RGB(IconPtr dst, int r, int g, int b, int bypp)
{
	switch (bypp)
	{
		case 1:
			SetPix8(dst, r, g, b);
			break;
		case 2:
			SetPix16(dst, r, g, b);
			break;
		case 3:
		case 4:
			SetPix24(dst, r, g, b);
			break;
	}
}

static void PixelPtrToRGB(IconPtr src, int *r, int *g, int *b, int bypp)
{
	switch (bypp)
	{
		case 1:
			GetPix8(src, r, g, b);
			break;
		case 2:
			GetPix16(src, r, g, b);
			break;
		case 3:
		case 4:
			GetPix24(src, r, g, b);
			break;
	}
}

void PixelLongToPtr(IconPtr p, int pixel, int bypp)
{
	switch (bypp)
	{
		case 1:
			*p = (unsigned char) pixel;
			break;
		case 2:
			*((unsigned short *) p) = pixel & 0xFFFF;
			break;
		case 3:
		{
			unsigned char *p2 = (unsigned char *) &pixel;
			*p++ = *p2++;
			*p++ = *p2++;
			*p++ = *p2++;
			break;
		}
		case 4:
			*((unsigned long *) p) = pixel;
			break;
	}
}

int PixelPtrToLong(IconPtr p, int bypp)
{
	switch (bypp)
	{
		case 1:
			return *p;
		case 2:
			return *(unsigned short *) p;
		case 3:
			return p[0] + (p[1] << 8) + (p[2] << 16); /* ??? */
		case 4:
			return *(int *) p;
	}
	return 0;
}

int Image2Bits(Tcl_Interp *interp, t_icon_data *iconDataPtr,
	Tk_PhotoHandle photoH, int imageW, int imageH, XColor *xColorPtr)
{
	Tk_PhotoImageBlock photoBlock;
	int row, col, y, x;
	int i, r, g, b;
	int r2 = 0, g2 = 0, b2 = 0;
	unsigned char *srcPtr, *dstPtr;
	int dataSize;
	int numCol, numRow;
	int iconW = iconDataPtr->width;
	int pixelSize = iconDataPtr->bypp;

	/* Hack - ignore parameter */
	(void) interp;

	/* Get info about the image */
	(void) Tk_PhotoGetImage(photoH, &photoBlock);

	/* Calculate the number of icons based on image dimensions */
	numCol = (photoBlock.width / imageW);
	numRow = (photoBlock.height / imageH);
	iconDataPtr->icon_count = numCol * numRow;

	/* Allocate icon buffer */
	dataSize = iconDataPtr->icon_count * iconDataPtr->length;
	iconDataPtr->icon_data = (unsigned char *) Tcl_AllocDebug(dataSize);

	if (pixelSize == 1)
	{
		/* Clear the color hash table used by Palette_RGB2Index() */
		Palette_ResetHash();
	}

	if (xColorPtr)
	{
		r2 = ((double) xColorPtr->red / USHRT_MAX) * 255;
		g2 = ((double) xColorPtr->green / USHRT_MAX) * 255;
		b2 = ((double) xColorPtr->blue / USHRT_MAX) * 255;
	}		

	dstPtr = iconDataPtr->icon_data;
	for (row = 0; row < numRow; row++)
	{
		for (col = 0; col < numCol; col++)
		{
			srcPtr = photoBlock.pixelPtr +
				col * imageW * photoBlock.pixelSize +
				row * imageH * photoBlock.pitch;
			for (y = 0; y < imageH; y++)
			{
				for (x = 0; x < imageW; x++)
				{
					r = *(srcPtr + x * photoBlock.pixelSize + photoBlock.offset[0]);
					g = *(srcPtr + x * photoBlock.pixelSize + photoBlock.offset[1]);
					b = *(srcPtr + x * photoBlock.pixelSize + photoBlock.offset[2]);
					if (xColorPtr)
					{
						/* Transparent */
						if ((r == r2) && (g == g2) && (b == b2))
						{
							/* Exclude from mask */
							r = g = b = 0;
						}
		
						/* Opaque */
						else
						{
							/* Include in mask */
							r = g = b = 0xFF;
						}
					}
					if (imageW != iconW)
					{
						if (iconW == 24)
						{
							if ((y & 1) && (x & 1))
							{
								PixelSet_RGB(dstPtr, r, g, b, pixelSize);
								dstPtr += pixelSize;
							}
							else if (y & 1)
							{
								PixelSet_RGB(dstPtr, r, g, b, pixelSize);
								dstPtr += pixelSize;
								PixelSet_RGB(dstPtr, r, g, b, pixelSize);
								dstPtr += pixelSize;
							}
							else if (x & 1)
							{
								PixelSet_RGB(dstPtr, r, g, b, pixelSize);
								PixelSet_RGB(dstPtr + iconW * pixelSize, r, g, b, pixelSize);
								dstPtr += pixelSize;
							}
							else
							{
								PixelSet_RGB(dstPtr, r, g, b, pixelSize);
								PixelSet_RGB(dstPtr + pixelSize, r, g, b, pixelSize);
								PixelSet_RGB(dstPtr + iconW * pixelSize, r, g, b, pixelSize);
								PixelSet_RGB(dstPtr + (iconW + 1) * pixelSize, r, g, b, pixelSize);
								dstPtr += 2 * pixelSize;
							}
						}
						if (iconW == 32)
						{
							PixelSet_RGB(dstPtr, r, g, b, pixelSize);
							PixelSet_RGB(dstPtr + pixelSize, r, g, b, pixelSize);
							PixelSet_RGB(dstPtr + iconW * pixelSize, r, g, b, pixelSize);
							PixelSet_RGB(dstPtr + (iconW + 1) * pixelSize, r, g, b, pixelSize);
							dstPtr += 2 * pixelSize;
						}
					}
					else
					{
						PixelSet_RGB(dstPtr, r, g, b, pixelSize);
						dstPtr += pixelSize;
					}
				}
				srcPtr += photoBlock.pitch;
				if (imageW != iconW)
				{
					if (iconW == 24)
					{
						if (!(y & 1))
							dstPtr += iconW * pixelSize;
					}
					if (iconW == 32)
					{
						dstPtr += iconW * pixelSize;
					}
				}
			}
		}
	}

	/* This step strips off blank icons at the end */
	{
		int whiteValue = 0xFF;
		int emptyIcon = TRUE;
		int n = iconDataPtr->icon_count - 1;

		if (pixelSize == 1)
			whiteValue = PALETTE_WHITE;
		while (emptyIcon)
		{
			srcPtr = iconDataPtr->icon_data + n * iconDataPtr->length;
			for (i = 0; i < iconDataPtr->length; i++)
			{
				if (*srcPtr++ != whiteValue)
				{
					emptyIcon = FALSE;
					break;
				}
			}
			if (emptyIcon)
			{
				iconDataPtr->icon_count -= 1;
				n--;
			}
		}
	}

	return TCL_OK;
}

static int WriteIconFile(Tcl_Interp *interp, char *utfFileName, t_icon_data *idp)
{
#ifdef USE_COMPRESS

	Tcl_DString extDString;
	char *extFileName;
	int result;

	extFileName = UtfToExt_TranslateFileName(interp, utfFileName, &extDString);
	if (extFileName == NULL)
	{
		return TCL_ERROR;
	}

	result = CompressIconFile(interp, extFileName, idp);

	Tcl_DStringFree(&extDString);

	return result;

#else /* not USE_COMPRESS */

	Tcl_DString temp;
	Tcl_Channel chan;
	char *fileBuf = NULL;
	int dataSize;

	/* Translate the file name */
	fileName = Tcl_TranslateFileName(interp, fileName, &temp);
	if (fileName == NULL)
	{
		/* Note: Tcl_DStringFree() is called for us */
		goto error;
	}

	/* Open the file for writing */
	chan = Tcl_OpenFileChannel(interp, fileName, "w", 0644);
	if (chan == NULL)
	{
        Tcl_ResetResult(interp);
		goto nowrite;
	}

	(void) Tcl_SetChannelOption(interp, chan, "-translation", "binary");

	dataSize = iconCount * iconLength;
	if (Tcl_Write(chan, iconData, dataSize) < 0)
	{
		Tcl_Close(interp, chan);
		goto nowrite;
	}

	if (Tcl_Close(interp, chan) != TCL_OK)
	{
		goto error;
	}

	Tcl_DStringFree(&temp);

	return TCL_OK;

nowrite:
	Tcl_AppendResult(interp, "couldn't write file \"", fileName,
		"\": ", Tcl_PosixError(interp), NULL);

error:
	if (fileBuf != NULL)
	{
		Tcl_Free(fileBuf);
	}
	Tcl_DStringFree(&temp);
	return TCL_ERROR;

#endif /* not USE_COMPRESS */
}

/*
 * Read an icon data file. Returns the icon data and number of icons
 * read in. The icon data files are created with an external program
 * "makeicon".
 */
static int ReadIconFile(Tcl_Interp *interp, char *fileName, IconPtr *iconData,
	int *iconCount)
{
#ifdef USE_COMPRESS

	Tcl_DString extDString;
	int result;

	fileName = UtfToExt_TranslateFileName(interp, fileName, &extDString);
	if (fileName == NULL)
	{
		return TCL_ERROR;
	}

	result = DecompressIconFile(interp, fileName, iconData, iconCount);

	Tcl_DStringFree(&extDString);

	return result;

#else /* not USE_COMPRESS */

	Tcl_DString temp;
	Tcl_Channel chan;
	struct stat statBuf;
	char *fileBuf = NULL;

	/* Translate the file name */
	fileName = Tcl_TranslateFileName(interp, fileName, &temp);
	if (fileName == NULL)
	{
		/* Note: Tcl_DStringFree() is called for us */
		goto error;
	}

	/* stat() the file to get the length */
	if (stat(fileName, &statBuf) == -1)
	{
		Tcl_SetErrno(errno);
		goto noread;
	}

	if ((statBuf.st_size % ICON_LENGTH) != 0)
	{
		Tcl_AppendResult(interp, "bad size on icon file \"", fileName,
			"\"", NULL);
		goto error;
	}

	/* Open the file for reading */
	chan = Tcl_OpenFileChannel(interp, fileName, "r", 0644);
	if (chan == NULL)
	{
        Tcl_ResetResult(interp);
		goto noread;
	}

	(void) Tcl_SetChannelOption(interp, chan, "-translation", "binary");

	fileBuf = Tcl_Alloc(statBuf.st_size);
	if (Tcl_Read(chan, fileBuf, statBuf.st_size) < 0)
	{
		Tcl_Close(interp, chan);
		goto noread;
	}
	if (Tcl_Close(interp, chan) != TCL_OK)
	{
		goto error;
	}
	Tcl_DStringFree(&temp);

	(*iconData) = (IconPtr) fileBuf;
	(*iconCount) = statBuf.st_size / ICON_LENGTH;

	return TCL_OK;

noread:
	Tcl_AppendResult(interp, "couldn't read file \"", fileName,
		"\": ", Tcl_PosixError(interp), NULL);

error:
	if (fileBuf != NULL)
	{
		Tcl_Free(fileBuf);
	}
	Tcl_DStringFree(&temp);
	return TCL_ERROR;

#endif /* not USE_COMPRESS */
}

/*
 * Get colorized data for TYPE_ASCII icon.
 */
IconPtr Icon_GetAsciiData(IconSpec *specPtr, IconPtr iconPtr)
{
	int i;
	int j = specPtr->index;
	int k = specPtr->ascii;
	t_icon_data *icon_data_ptr = &g_icon_data[specPtr->type];
	PixelPtr srcPtr, dstPtr;
	int *color = g_ascii[k].color;
	int colors[2];

	srcPtr.pix8 = icon_data_ptr->icon_data + j * icon_data_ptr->length;
	dstPtr.pix8 = iconPtr;

	/* Multi-hued */
	if (g_ascii[k].mode == ASCII_ATTR_MULTI)
	{
		colors[0] = g_term_colormap[g_ascii_multi];
		colors[1] = color[1];
		color = colors;
	}

	/* Shapechanger */
	else if (g_ascii[k].mode == ASCII_SHAPECHANGER)
	{
		i = g_ascii_char;

		/* Paranoia */
		if (i >= icon_data_ptr->icon_count)
		{
			i = icon_data_ptr->icon_count - 1;
		}
		
		srcPtr.pix8 = icon_data_ptr->icon_data + i * icon_data_ptr->length;
	}

	if (icon_data_ptr->depth == 8)
	{
		for (i = 0; i < icon_data_ptr->pixels; i++)
		{
			*dstPtr.pix8++ = color[*srcPtr.pix8++];
		}
	}
	else if (icon_data_ptr->depth == 16)
	{
		for (i = 0; i < icon_data_ptr->pixels; i++)
		{
			*dstPtr.pix16++ = color[*srcPtr.pix16++];
		}
	}
	else if (icon_data_ptr->depth == 24)
	{
		/* Skip to low-order byte */
		srcPtr.pix8 += 2;

		for (i = 0; i < icon_data_ptr->pixels; i++)
		{
			unsigned char *zbgr = (unsigned char *) &color[*srcPtr.pix8];

#ifdef PLATFORM_WIN
			dstPtr.pix8[0] = zbgr[2]; /* b */
			dstPtr.pix8[1] = zbgr[1]; /* g */
			dstPtr.pix8[2] = zbgr[0]; /* r */
#endif /* */
#ifdef PLATFORM_X11
			dstPtr.pix8[0] = zbgr[0]; /* r */
			dstPtr.pix8[1] = zbgr[1]; /* g */
			dstPtr.pix8[2] = zbgr[2]; /* b */
#endif /* */

			/* Skip to low-order byte */
			srcPtr.pix8 += g_pixel_size;

			dstPtr.pix8 += g_pixel_size;
		}
	}

	return iconPtr;
}

static int g_ascii_y = 0, g_ascii_x = 0;

/*
 * Writes into the icon_data field of the given icon data struct,
 * which is assumed to be an ascii icon type. This is called when
 * creating a new ascii icon type and whenever the font for this
 * icon type is changed (via the "ascii font" command).
 */
static int init_ascii_data(Tcl_Interp *interp, t_icon_data *icon_data_ptr)
{
	BitmapType bitmap;
	int i;
	int y, x;
	char buf[2];
	PixelPtr srcPtr, dstPtr;
	Tk_FontMetrics fm;
	Tk_Window tkwin = Tk_MainWindow(interp);
	Display *display = Tk_Display(tkwin);
	int width, y2, x2;
	GC gc;
	XGCValues gcValues;
	Pixmap pixmap;

	/* Create a temporary bitmap as large as one icon */
	bitmap.width = icon_data_ptr->width;
	bitmap.height = icon_data_ptr->height;
	bitmap.depth = icon_data_ptr->depth;
	Bitmap_New(interp, &bitmap);

	pixmap = bitmap.pixmap;

	/* Graphics context for text */
	gcValues.foreground = 0x333333;
	gcValues.graphics_exposures = False;
	gcValues.font = Tk_FontId(icon_data_ptr->font);
	gc = Tk_GetGC(tkwin,
		GCForeground | GCFont | GCGraphicsExposures,
		&gcValues);

	/* Get info about the font */
	Tk_GetFontMetrics(icon_data_ptr->font, &fm);

	dstPtr.pix8 = icon_data_ptr->icon_data;

	/* Check each icon */
	for (i = 0; i < icon_data_ptr->icon_count; i++)
	{
		int ch = 32 + i; /* printable only */

		/* If a character set was specified, get the character */
		if (icon_data_ptr->char_table)
		{
			ch = icon_data_ptr->char_table[i];
		}

		/* Erase background to ones */
		for (y = 0; y < bitmap.height; y++)
		{
			PixelPtr pixelPtr;
			pixelPtr.pix8 = bitmap.pixelPtr + y * bitmap.pitch;
			for (x = 0; x < bitmap.width; x++)
			{
				switch (bitmap.depth)
				{
					case 8:
						*pixelPtr.pix8 = 1;
						break;
					case 16:
						*pixelPtr.pix16 = 1;
						break;
					case 24:
						pixelPtr.pix8[0] = 1;
						pixelPtr.pix8[1] = 1;
						pixelPtr.pix8[2] = 1;
						break;
				}
				pixelPtr.pix8 += bitmap.pixelSize;
			}
		}

		/* Only allow printable characters */
		if (isprint(ch))
		{
			/* Get a 2-byte string */
			(void) sprintf(buf, "%c", ch);

			/* Calculate the width of the character */
			width = Tk_TextWidth(icon_data_ptr->font, buf, 1);
		
			/* Calculate the position of the character in the bitmap */
			x2 = g_ascii_x + (g_icon_size - width) / 2;
			y2 = g_ascii_y + (g_icon_size - fm.linespace) / 2 + fm.ascent;

			/* Draw the character into the bitmap */
			Tk_DrawChars(display, pixmap, gc,
				icon_data_ptr->font, buf, 1, x2, y2);

			/* Flush before we copy */
			Plat_SyncDisplay(display);

			/*
			 * Convert text pixel values to zero, and copy from the bitmap
			 * to the icon data
			 */
			for (y = 0; y < bitmap.height; y++)
			{
				/* Point to the row */
				srcPtr.pix8 = bitmap.pixelPtr + y * bitmap.pitch;

				for (x = 0; x < bitmap.width; x++)
				{
					switch (bitmap.depth)
					{
						case 8:
						{
							if (srcPtr.pix8[0] != 1)
								srcPtr.pix8[0] = 0;
							dstPtr.pix8[0] = srcPtr.pix8[0];
							break;
						}
						case 16:
						{
							if (srcPtr.pix16[0] != 1)
								srcPtr.pix16[0] = 0;
							dstPtr.pix16[0] = srcPtr.pix16[0];
							break;
						}
						case 24:
						{
							if (srcPtr.pix8[0] != 1)
								srcPtr.pix8[0] = 0;
							if (srcPtr.pix8[1] != 1)
								srcPtr.pix8[1] = 0;
							if (srcPtr.pix8[2] != 1)
								srcPtr.pix8[2] = 0;
							dstPtr.pix8[0] = srcPtr.pix8[0];
							dstPtr.pix8[1] = srcPtr.pix8[1];
							dstPtr.pix8[2] = srcPtr.pix8[2];
							break;
						}
					}
					srcPtr.pix8 += bitmap.pixelSize;
					dstPtr.pix8 += icon_data_ptr->bypp;
				}
			}
		}

		/* Not printable */
		else
		{
			/* Just skip the icon (set to TYPE_DEFAULT?) */
			dstPtr.pix8 += icon_data_ptr->length;
		}
	}

	/* Done with GC */
	Tk_FreeGC(display, gc);

	/* Delete bitmap */
	Bitmap_Delete(&bitmap);

	/* Success */
	return TCL_OK;
}

/*
 * objcmd_ascii --
 *
 * Configure the "ascii" icon type.
 * Usage:
 *		ascii count
 *		ascii create forground background
 *		ascii font typeName ?fontSpec?
 *		ascii index typeName char attr -- Return index for character & attr
 *		ascii delay ?ticks?
 *		ascii inset ?y x?
 */

/* The current index into the g_term_colormap[] array */
int g_ascii_multi = 0;

/* The "frame delay" for animated ascii configurations */
int g_ascii_ticks = 0;
int g_ascii_delay = 1000;

/* The current character for shapechanging ascii configurations */
int g_ascii_char = 0;

static int objcmd_ascii(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	static cptr cmdOption[] = {"count", "create", "font", "index",
		"delay", "configure", "isascii", NULL};
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	static cptr configSwitch[] = {"-background", "-foreground", "-mode",
		NULL};
	static cptr asciiMode[] = {"normal", "attr_multi", "shapechanger",
		NULL};

	char d_char, *t;
	Tcl_Obj *CONST *objPtr;
	Tk_Font tkFont;
	t_icon_data *icon_data_ptr;
	char *typeName;
	int fg, bg, mode;
	t_ascii *ascii_ptr;
	int i;
	
	/* Hack - ignore parameter */
	(void) dummy;
	
	/* Required number of arguments */
    if (objc < 2)
    {
		Tcl_WrongNumArgs(interp, 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
    }

	/* Get requested option */
    if (Tcl_GetIndexFromObj(interp, objv[1], (char **) cmdOption, (char *) "option", 0, 
		&index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* count */
			Tcl_SetIntObj(resultPtr, g_ascii_count);
			break;

		case 1: /* create */
		{
			XColor *fgPtr, *bgPtr;
			Tk_Window tkwin = Tk_MainWindow(interp);

			/* Default */
			fg = COLORMAP_WHITE; /* Only valid on 8-bit! */
			bg = COLORMAP_BLACK; /* Only valid on 8-bit! */
			mode = ASCII_NORMAL;

			/* Point to the first option/value pair */
			objPtr = objv + 2;
			objc -= 2;

			/* Scan all option/value pairs */
			while (objc > 1)
			{
			    if (Tcl_GetIndexFromObj(interp, objPtr[0], (char **) configSwitch,
					(char *) "switch", 0, &index) != TCL_OK)
				{
					return TCL_ERROR;
			    }
				switch (index)
				{
					case 0: /* -background */
						if ((bgPtr = Tk_AllocColorFromObj(interp,
							tkwin, objPtr[1])) == NULL)
						{
							return TCL_ERROR;
						}
						bg = Plat_XColor2Pixel(bgPtr);
						Tk_FreeColor(bgPtr); /* XXX */
						break;
						
					case 1: /* -foreground */
						if ((fgPtr = Tk_AllocColorFromObj(interp,
							tkwin, objPtr[1])) == NULL)
						{
							return TCL_ERROR;
						}
						fg = Plat_XColor2Pixel(fgPtr);
						Tk_FreeColor(fgPtr); /* XXX */
						break;
						
					case 2: /* -mode */
					    if (Tcl_GetIndexFromObj(interp, objPtr[1], (char **) asciiMode,
							(char *) "mode", 0, &mode) != TCL_OK)
						{
							return TCL_ERROR;
					    }
					    break;
				}

				/* Next option/value pair */
				objPtr += 2;
				objc -= 2;
			}

			/* Append a new ascii configuration to the global array */
			g_ascii = Array_Insert(g_ascii, &g_ascii_count,
				sizeof(t_ascii), g_ascii_count);

			/* Access the ascii configuration */
			ascii_ptr = &g_ascii[g_ascii_count - 1];

			/* Set the fields */
			ascii_ptr->color[0] = fg;
			ascii_ptr->color[1] = bg;
			ascii_ptr->mode = mode;

			/* Return the index of the new ascii configuration */
			Tcl_SetIntObj(resultPtr, g_ascii_count - 1);

			break;
		}

		case 2: /* font */

			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "type");
				return TCL_ERROR;
			}

			/* Get the icon type name */
			typeName = Tcl_GetStringFromObj(objv[2], NULL);

			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &icon_data_ptr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* This icon type doesn't have a font */
			if (icon_data_ptr->font == NULL)
			{
				/* Set the error */
				Tcl_SetStringObj(resultPtr,
					format("icon type \"%s\" not ascii", typeName), -1);

				/* Failure */
				return TCL_ERROR;
			}

			/* "ascii font $type" */
			/* Return current font */
			if (objc == 3)
			{
				/* NOT IMPLEMENTED */
				break;
			}

			/* Get the name of the font */
			t = Tcl_GetStringFromObj(objv[3], NULL);

			/* Get the font */
			tkFont = Tk_GetFont(interp, Tk_MainWindow(interp), t);

			/* The font could not be created */
			if (tkFont == NULL)
			{
				return TCL_ERROR;
			}

			/* Free the old (Tk) font, if any */
			if (icon_data_ptr->font) Tk_FreeFont(icon_data_ptr->font);

			/* Remember current font */
			icon_data_ptr->font = tkFont;

			/* Set the icon_data */
			if (init_ascii_data(interp, icon_data_ptr) != TCL_OK)
			{
				return TCL_ERROR;
			}
			break;

		case 3: /* index */

			if (objc != 4)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "type char");
				return TCL_ERROR;
			}

			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &icon_data_ptr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the desired character */
			t = Tcl_GetStringFromObj(objv[3], NULL);
			d_char = t[0];

			/*
			 * The char_table for ascii icon types is used for
			 * specific ranges of characters.
			 */
			if (icon_data_ptr->char_table)
			{
				/* Check each icon */
				for (i = 0; i < icon_data_ptr->icon_count; i++)
				{
					/* This icon represents the desired character */
					if (icon_data_ptr->char_table[i] == d_char)
					{
						/* Return the icon index */
						Tcl_SetIntObj(resultPtr, i);

						/* Stop */
						break;
					}
				}

				/* The character was not found */
				if (i == icon_data_ptr->icon_count)
				{
					/* Set the error */
					Tcl_SetStringObj(resultPtr,
						format("bad character \"%c\": ", d_char), -1);

					/* Failure */
					return TCL_ERROR;
				}
			}

			/* This icon type uses all printable ascii characters */
			else if (isprint(d_char))
			{
				Tcl_SetIntObj(resultPtr, d_char - 32); /* printable only */
			}

			/* Failure */
			else
			{
				Tcl_SetIntObj(resultPtr, 0);
			}
			break;

		case 4: /* delay */

			if (objc < 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "?delay?");
				return TCL_ERROR;
			}

			/* A new value was given */
			if (objc == 3)
			{
				/* Get the new value */
				if (Tcl_GetIntFromObj(interp, objv[2], &g_ascii_delay) != TCL_OK)
				{
					return TCL_ERROR;
				}
				/* VERIFY g_ascii_delay */
			}

			/* Return the current value */
			Tcl_SetIntObj(resultPtr, g_ascii_delay);
			break;

		case 5: /* configure */

			if (objc < 4)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "index option ?value?");
				return TCL_ERROR;
			}

			/* Get the ascii configuration index */
			if (Tcl_GetIntFromObj(interp, objv[2], &index) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Verify the ascii configuration index */
			if ((index < 0) || (index >= g_ascii_count))
			{
				/* Set the error */
				Tcl_SetStringObj(resultPtr,
					format("bad ascii \"%d\": must be from 0 to %d",
					index, g_ascii_count -1), -1);


				/* Failure */
				return TCL_ERROR;
			}

			/* Access the ascii configuration */
			ascii_ptr = &g_ascii[index];

			/* "ascii configure $index $option" */
			/* Return the value of a single option */
			if (objc == 4)
			{
			    if (Tcl_GetIndexFromObj(interp, objv[3], (char **) configSwitch,
					(char *) "switch", 0, &index) != TCL_OK)
				{
					return TCL_ERROR;
			    }
			    switch (index)
			    {
					case 0: /* -background */
						Tcl_SetIntObj(resultPtr, ascii_ptr->color[1]);
						break;
						
					case 1: /* -foreground */
						Tcl_SetIntObj(resultPtr, ascii_ptr->color[0]);
						break;
						
					case 2: /* -mode */
						Tcl_SetStringObj(resultPtr,
							(char *) asciiMode[ascii_ptr->mode], -1);
					    break;
				}

				/* Done */
				break;
			}

			/* Point to the first option/value pair */
			objPtr = objv + 3;
			objc -= 3;

			/* Scan all option/value pairs */
			while (objc > 1)
			{
			    if (Tcl_GetIndexFromObj(interp, objPtr[0], (char **) configSwitch,
					(char *) "switch", 0, &index) != TCL_OK)
				{
					return TCL_ERROR;
			    }
				switch (index)
				{
					case 0: /* -background */
						if (Tcl_GetIntFromObj(interp, objPtr[1], &bg) != TCL_OK)
						{
							return TCL_ERROR;
						}
						/* VERIFY bg */
						ascii_ptr->color[1] = bg;
						break;
						
					case 1: /* -foreground */
						if (Tcl_GetIntFromObj(interp, objPtr[1], &fg) != TCL_OK)
						{
							return TCL_ERROR;
						}
						/* VERIFY fg */
						ascii_ptr->color[0] = fg;
						break;
						
					case 2: /* -mode */
					    if (Tcl_GetIndexFromObj(interp, objPtr[1], (char **) asciiMode,
							(char *) "mode", 0, &index) != TCL_OK)
						{
							return TCL_ERROR;
					    }
					    ascii_ptr->mode = index;
					    break;
				}

				/* Next option/value pair */
				objPtr += 2;
				objc -= 2;
			}
			/* VERIFY objc */
			break;

		case 6: /* isascii */

			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "type");
				return TCL_ERROR;
			}

			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &icon_data_ptr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* This icon type doesn't have a font */
			if (icon_data_ptr->font == NULL)
			{
				Tcl_SetBooleanObj(resultPtr, 0);
				break;
			}
			Tcl_SetBooleanObj(resultPtr, 1);
			break;
	}

	/* Success */
	return TCL_OK;
}

/*
 * icon photo $imageName -type $type -index $index
 * Set an existing photo image with icon data. This is similar to the
 * Widget_Photo() routine and lets us set a label widget with a picture
 * of any icon.
 */
static int objcmd_icon_photo(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tk_PhotoHandle photoH;
	Tk_PhotoImageBlock photoBlock;
	char *imageName;
	int i, transparent = 0;
	int r, g, b;
	IconSpec iconSpec;
	t_icon_data *iconDataPtr;
	unsigned char *photoData;
	unsigned char *srcPtr = NULL, *dstPtr;
	unsigned char *dataPtr, *maskPtr = NULL;
	
	/* Hack - ignore unused parameter */
	(void) dummy;

	/* Required number of arguments */
    if (objc < 3)
    {
		Tcl_WrongNumArgs(interp, 2, objv,
			(char *) "imageName -type iconType -index iconIndex");
		return TCL_ERROR;
    }

	/* Get the name of the Tk photo image. */
	imageName = Tcl_GetStringFromObj(objv[2], NULL);

	/* Lookup the photo by name. It must already exist */
	photoH = Tk_FindPhoto(interp, imageName);

	/* The photo was not found */
	if (photoH == NULL)
	{
		/* Set the error */
	    Tcl_AppendResult(interp, "image \"", imageName,
	    	"\" doesn't exist or is not a photo image",
	    	NULL);

		/* Failure */
		return TCL_ERROR;
	}

	/* Scan the arguments for icon type and index */
	if (Icon_ParseArgs(interp, objc, objv, 3, &iconSpec) != TCL_OK)
	{
		/* Failure */
		return TCL_ERROR;
	}

	iconDataPtr = &g_icon_data[iconSpec.type];
	dataPtr = (unsigned char *) Tcl_AllocDebug(iconDataPtr->length);

	/* Ascii-type icon */
	if (iconSpec.ascii != -1)
	{
		srcPtr = Icon_GetAsciiData(&iconSpec, dataPtr);
	}

	/* Dynamic transparent */
	else if (iconDataPtr->dynamic)
	{
		/* Should never be assigned */
		transparent = 1;
	}
	
	/* Transparent */
	else if (iconDataPtr->rle_data != NULL)
	{
		unsigned char *bounds = iconDataPtr->rle_bounds + iconSpec.index * 4;
		IconPtr rlePtr = iconDataPtr->rle_data +
			iconDataPtr->rle_offset[iconSpec.index];

		/* Init to transparent pixel */
		for (i = 0; i < iconDataPtr->pixels; i++)
		{
			PixelLongToPtr(dataPtr + i * iconDataPtr->bypp,
				iconDataPtr->rle_pixel, iconDataPtr->bypp);
		}
		
		/* Decode image */
		RL_Decode(bounds[2], bounds[3], iconDataPtr->bypp,
			rlePtr, dataPtr + bounds[0] * iconDataPtr->bypp + 
			bounds[1] * iconDataPtr->pitch, iconDataPtr->pitch);

		/* Create a mask, 1-byte-per-pixel */
		maskPtr = (unsigned char *) Tcl_AllocDebug(iconDataPtr->pixels);
		for (i = 0; i < iconDataPtr->pixels; i++)
		{
			int pixel = PixelPtrToLong(dataPtr + i * iconDataPtr->bypp,
				iconDataPtr->bypp);

			/* Not transparent */
			if (pixel != iconDataPtr->rle_pixel)
				maskPtr[i] = 1;

			/* Transparent */
			else
				maskPtr[i] = 0;
		}

		srcPtr = dataPtr;
		transparent = 1;
	}

	/* Simple */
	else
	{
		srcPtr = iconDataPtr->icon_data + iconSpec.index * iconDataPtr->length;
	}

	/* Convert the icon data to RGB */
	photoData = (unsigned char *) Tcl_AllocDebug(iconDataPtr->pixels * 3);
	dstPtr = photoData;
	for (i = 0; i < iconDataPtr->pixels; i++)
	{
		PixelPtrToRGB(srcPtr, &r, &g, &b, iconDataPtr->bypp);
		*dstPtr++ = r;
		*dstPtr++ = g;
		*dstPtr++ = b;
		srcPtr += iconDataPtr->bypp;
	}
	
	photoBlock.width = iconDataPtr->width;
	photoBlock.height = iconDataPtr->height;
	photoBlock.pixelSize = 3;
	photoBlock.pitch = photoBlock.width * photoBlock.pixelSize;
	photoBlock.offset[0] = 0;
	photoBlock.offset[1] = 1;
	photoBlock.offset[2] = 2;

	/*
	 * The next bit is adapted from FileReadGIF() in tkImgGIF.c
	 * from Tk 8.0.4
	 */
	if (transparent)
	{
		int start, end, y, x;
		
		/*
		 * The trick to creating a photo image with transparency
		 * is to only put image data into the masked regions.
		 */
		i = 0;
		for (y = 0; y < photoBlock.height; y++)
		{
			x = 0;
			while (x < photoBlock.width)
			{
				/* Search for first non-transparent pixel */
				while ((x < photoBlock.width) && !maskPtr[i])
				{
					x++; i++;
				}
				end = x;
				start = i;
				
				/* Search for first transparent pixel */
				while ((end < photoBlock.width) && maskPtr[i])
				{
					end++; i++;
				}
				if (end > x)
				{
					photoBlock.pixelPtr = &photoData[start * 3];
					Tk_PhotoPutBlock(photoH, &photoBlock, x,
						y, end - x, 1);
				}
				x = end;
			}
		}
	}

	/* Not transparent */
	else
	{
		photoBlock.pixelPtr = photoData;
	
		/* Add the data to the photo image */
		Tk_PhotoPutBlock(photoH, &photoBlock, 0, 0, photoBlock.width,
			photoBlock.height);
	}

	Tcl_FreeDebug((char *) photoData);
	Tcl_FreeDebug((char *) dataPtr);
	if (maskPtr)
		Tcl_FreeDebug((char *) maskPtr);

	return TCL_OK;
}

void Icon_MakeDark(t_icon_data *iconDataPtr, int index)
{
	int i, j, dark, length;
	IconPtr srcPtr, dstPtr;
	double gamma[2];

	if (iconDataPtr->dark_data == NULL)
	{
		iconDataPtr->dark_data = (IconPtr *)
			Tcl_AllocDebug(iconDataPtr->icon_count * sizeof(IconPtr));
		for (i = 0; i < iconDataPtr->icon_count; i++)
		{
			iconDataPtr->dark_data[i] = NULL;
		}
	}

	if (iconDataPtr->dynamic)
	{
		length = iconDataPtr->rle_len[index];
		srcPtr = NULL;
	}
	else if (iconDataPtr->rle_data)
	{
		length = iconDataPtr->rle_len[index];
		srcPtr = iconDataPtr->rle_data + iconDataPtr->rle_offset[index];
	}
	else
	{
		length = iconDataPtr->length;
		srcPtr = iconDataPtr->icon_data + index * iconDataPtr->length;
	}

	if (iconDataPtr->dark_data[index] == NULL)
	{
		iconDataPtr->dark_data[index] =
			(IconPtr) Tcl_AllocDebug(2 * length);
	}

	/*
	 * For dynamic icons, instead of dark'ing the whole icon,
	 * we copy each dark'd component icon (if any exist).
	 */
	if (iconDataPtr->dynamic)
	{
		IconPtr tmp;

		/* New temp buffer for 1 icon */
		tmp = (IconPtr) Tcl_AllocDebug(iconDataPtr->length);

		for (dark = 0; dark < 2; dark++)
		{
			unsigned char *bounds;

			/* Init to transparent pixel */
			for (i = 0; i < iconDataPtr->pixels; i++)
			{
				PixelLongToPtr(tmp + i * iconDataPtr->bypp,
					iconDataPtr->rle_pixel, iconDataPtr->bypp);
			}

			for (i = 0; i < 4; i++)
			{
				IconSpec iconSpec;
				t_icon_data *dataPtr;
	
				/* Get the component icon */
				iconSpec = iconDataPtr->dynamic_spec[index * 4 + i];
	
				/* Stop */
				if (iconSpec.type == -1)
					break;
					
				dataPtr = &g_icon_data[iconSpec.type];
				bounds = dataPtr->rle_bounds + iconSpec.index * 4;
	
				/* Create dark data for component if needed */
				if (dataPtr->flags[iconSpec.index] & ICON_FLAG_DARK)
					Icon_MakeDark(dataPtr, iconSpec.index);

				/* A darkened copy exists */
				if (dataPtr->dark_data && dataPtr->dark_data[iconSpec.index])
				{
					srcPtr = dataPtr->dark_data[iconSpec.index] +
						dark * dataPtr->rle_len[iconSpec.index];
				}
				else
				{
					srcPtr = dataPtr->rle_data + dataPtr->rle_offset[iconSpec.index];
				}

				RL_Decode(bounds[2], bounds[3], dataPtr->bypp, srcPtr,
					tmp + bounds[0] * dataPtr->bypp +
					bounds[1] * dataPtr->pitch,
					dataPtr->pitch);
			}

			dstPtr = iconDataPtr->dark_data[index] +
				dark * iconDataPtr->rle_len[index];
			bounds = iconDataPtr->rle_bounds + index * 4;
			
			/* Encode */
			(void) RL_Encode(bounds[2], bounds[3], iconDataPtr->bypp,
				tmp + bounds[0] * iconDataPtr->bypp +
				bounds[1] * iconDataPtr->pitch,
				iconDataPtr->pitch, iconDataPtr->rle_pixel, dstPtr);
		}

		Tcl_FreeDebug((char *) tmp);

		/* Dark data created */
		iconDataPtr->flags[index] &= ~ICON_FLAG_DARK;

		return;
	}

	gamma[0] = iconDataPtr->gamma[0][index] / 100.0;
	gamma[1] = iconDataPtr->gamma[1][index] / 100.0;
	
	for (i = 0; i < 2; i++)
	{
		IconPtr darkData = iconDataPtr->dark_data[index] + i * length;
		int bypp = iconDataPtr->bypp;
	
		/* Copy the original icon */
		memcpy(darkData, srcPtr, length);

		/* Transparent */
		if (iconDataPtr->rle_data)
		{
			IconPtr rlePtr = darkData;

			/* Apply gamma-correction */
			while (1)
			{
				int trans, opaq;
				trans = rlePtr[0];
				opaq = rlePtr[1];
				if (!trans && !opaq)
					break;
				rlePtr += 2;
				for (j = 0; j < opaq; j++)
				{
					int r, g, b;
		
					PixelPtrToRGB(rlePtr + j * bypp, &r, &g, &b, bypp);
		
					/* Gamma-correct each RGB intensity */
					r = gamma_correct(r, gamma[i]);
					g = gamma_correct(g, gamma[i]);
					b = gamma_correct(b, gamma[i]);

					PixelSet_RGB(rlePtr + j * bypp, r, g, b, bypp);
				}
				rlePtr += opaq * bypp;
			}

			continue;
		}

		/* Darken */
		for (j = 0; j < iconDataPtr->pixels; j++)
		{
			int r, g, b;

			PixelPtrToRGB(darkData + j * iconDataPtr->bypp,
				&r, &g, &b, iconDataPtr->bypp);

			/* Gamma-correct each RGB intensity */
			r = gamma_correct(r, gamma[i]);
			g = gamma_correct(g, gamma[i]);
			b = gamma_correct(b, gamma[i]);

			PixelSet_RGB(darkData + j * iconDataPtr->bypp,
				r, g, b, iconDataPtr->bypp);
		}
	}

	/* Dark data created */
	iconDataPtr->flags[index] &= ~ICON_FLAG_DARK;
}

/*
 * objcmd_icon_dark --
 * (icon) dark $type $index ?$gamma $gamma?
 */
static int objcmd_icon_dark(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	int index, g1, g2, i;
	t_icon_data *iconDataPtr;
	double gamma[2];
	char buf[64];

	/* Ignore parameter */
	(void) dummy;

	/* Required number of arguments */
    if (objc != 4 && objc != 6)
    {
		Tcl_WrongNumArgs(interp, 2, objv, (char *) "$type $index ?$gamma $gamma?");
		return TCL_ERROR;
    }

	if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (Icon_GetIndexFromObj(interp, &index, objv[3], iconDataPtr) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (objc == 4)
	{
		if (iconDataPtr->gamma[0] == NULL)
		{
			/* Empty string as result */
			return TCL_OK;
		}
		g1 = iconDataPtr->gamma[0][index];
		g2 = iconDataPtr->gamma[1][index];
		(void) sprintf(buf, "%d.%d %d.%d", g1 / 100, g1 % 100,
			g2 / 100, g2 % 100);
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		return TCL_OK;
	}

	/* Get the desired gamma value */
	if (Tcl_GetDoubleFromObj(interp, objv[4], &gamma[0]) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Get the desired gamma value */
	if (Tcl_GetDoubleFromObj(interp, objv[5], &gamma[1]) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (gamma[0] < 0.0 || gamma[0] > 2.0)
	{
		return TCL_ERROR;
	}
	if (gamma[1] < 0.0 || gamma[1] > 2.0)
	{
		return TCL_ERROR;
	}

	/* Pass 1.0 to free it */
	if (gamma[0] == 1.0)
	{
		if ((iconDataPtr->dark_data != NULL) &&
			(iconDataPtr->dark_data[index] != NULL))
		{
			Tcl_FreeDebug((char *) iconDataPtr->dark_data[index]);
			iconDataPtr->dark_data[index] = NULL;
		}
		if (iconDataPtr->gamma[0])
		{
			iconDataPtr->gamma[0][index] = 100;
			iconDataPtr->gamma[1][index] = 100;
		}
		iconDataPtr->flags[index] &= ~ICON_FLAG_DARK;
		return TCL_OK;
	}

	if (iconDataPtr->gamma[0] == NULL)
	{
		iconDataPtr->gamma[0] = (unsigned char *)
			Tcl_AllocDebug(iconDataPtr->icon_count * sizeof(unsigned char));
		iconDataPtr->gamma[1] = (unsigned char *)
			Tcl_AllocDebug(iconDataPtr->icon_count * sizeof(unsigned char));
		for (i = 0; i < iconDataPtr->icon_count; i++)
		{
			iconDataPtr->gamma[0][i] = 100;
			iconDataPtr->gamma[1][i] = 100;
		}
	}

	iconDataPtr->gamma[0][index] = gamma[0] * 100;
	iconDataPtr->gamma[1][index] = gamma[1] * 100;
	iconDataPtr->flags[index] |= ICON_FLAG_DARK;

	return TCL_OK;
}

/*
 * objcmd_icon_dynamic --
 *	(icon) dynamic blank $type $index
 *	(icon) dynamic copy $type $index -type -index -ascii
 *	(icon) dynamic count $type ?$size?
 */
static int objcmd_icon_dynamic(ClientData dummy, Tcl_Interp *interp, int objc,
								 Tcl_Obj *CONST objv[])
{
	static cptr cmdOption[] = {"blank", "copy", "count", NULL};
	enum {IDX_BLANK, IDX_COPY, IDX_COUNT} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	t_icon_data *iconDataPtr;

	/* Ignore parameter */
	(void) dummy;

	/* Required number of arguments */
    if (objc < 3)
    {
		Tcl_WrongNumArgs(interp, 1, objv, (char *) "option type ?arg ...?");
		return TCL_ERROR;
    }

	/* Get requested option */
    if (Tcl_GetIndexFromObj(interp, objv[1], (char **) cmdOption, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
	{
		return TCL_ERROR;
	}

	if (!iconDataPtr->dynamic)
	{
		return TCL_ERROR;
	}

    switch (option)
	{
		case IDX_BLANK:
		{
			IconPtr *mem;
			int i;

			if (Icon_GetIndexFromObj(interp, &index, objv[3], iconDataPtr)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}

			mem = (IconPtr *) iconDataPtr->rle_data;
			if (mem[index])
			{
				Tcl_FreeDebug((char *) mem[index]);
				mem[index] = NULL;
			}
			if (iconDataPtr->dark_data && iconDataPtr->dark_data[index])
			{
				Tcl_FreeDebug((char *) iconDataPtr->dark_data[index]);
				iconDataPtr->dark_data[index] = NULL;
			}
			for (i = 0; i < 4; i++)
				iconDataPtr->dynamic_spec[index * 4 + i].type = -1;
			break;
		}

		case IDX_COPY:
		{
			IconSpec iconSpec;
			IconPtr rlePtr, bits, *mem, tmp;
			t_icon_data *srcDataPtr;
			int i, len;
			unsigned char *rlebounds, *bounds;
			int bypp = iconDataPtr->bypp;

			if (Icon_GetIndexFromObj(interp, &index, objv[3], iconDataPtr)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Return list of components */
			if (objc == 4)
			{
				Tcl_Obj *listObjPtr = Tcl_NewListObj(0, NULL);
				for (i = 0; i < 4; i++)
				{
					char buf[64];
					iconSpec = iconDataPtr->dynamic_spec[index * 4 + i];
					if (iconSpec.type == -1)
						break;
					(void) sprintf(buf, "%s %d",
						g_icon_data[iconSpec.type].desc, iconSpec.index);
					Tcl_ListObjAppendElement(interp, listObjPtr,
						Tcl_NewStringObj(buf, -1));
				}
				Tcl_SetObjResult(interp, listObjPtr);
				break;
			}

			/* Scan the arguments for icon type and index */
			if (Icon_ParseArgs(interp, objc, objv, 4, &iconSpec) != TCL_OK)
			{
				/* Failure */
				return TCL_ERROR;
			}

			/* Get the source RLE data */
			srcDataPtr = &g_icon_data[iconSpec.type];
			rlePtr = srcDataPtr->rle_data +
				srcDataPtr->rle_offset[iconSpec.index];
			rlebounds = srcDataPtr->rle_bounds + (iconSpec.index * 4);

			/* Dest icon array */
			mem = (IconPtr *) iconDataPtr->rle_data;

			/* New temp buffer for 1 icon */
			tmp = (IconPtr) Tcl_AllocDebug(iconDataPtr->length);

			/* Init to transparent pixel */
			for (i = 0; i < iconDataPtr->pixels; i++)
			{
				PixelLongToPtr(tmp + i * iconDataPtr->bypp,
					iconDataPtr->rle_pixel, iconDataPtr->bypp);
			}

			bounds = iconDataPtr->rle_bounds + index * 4;

			/* If old data, decode it first */
			if (mem[index])
			{
				RL_Decode(bounds[2], bounds[3], iconDataPtr->bypp,
					mem[index], tmp + bounds[0] * bypp + 
					bounds[1] * iconDataPtr->pitch, iconDataPtr->pitch);
				Tcl_FreeDebug((char *) mem[index]);
				mem[index] = NULL;
			}

			/* Decode new data */
			RL_Decode(rlebounds[2], rlebounds[3], iconDataPtr->bypp,
				rlePtr, tmp + rlebounds[0] * bypp + rlebounds[1] *
				srcDataPtr->pitch, srcDataPtr->pitch);

			/* Get bounds of non-transparent pixels */
			RL_Bounds(iconDataPtr->width, iconDataPtr->height, bypp,
				tmp, iconDataPtr->rle_pixel, bounds);
			
			/* Get length of encoded data */
			len = RL_Len(bounds[2], bounds[3],
				bypp, tmp + bounds[0] * bypp + bounds[1] * iconDataPtr->pitch,
				iconDataPtr->pitch, iconDataPtr->rle_pixel);

			/* Allocate RLE buf */
			/* FIXME: Does RL_Encode() still go past end of buffer? */
			bits = (IconPtr) Tcl_AllocDebug(len);

			/* Encode */
			(void) RL_Encode(bounds[2], bounds[3],
				bypp, tmp + bounds[0] * bypp + bounds[1] * iconDataPtr->pitch,
				iconDataPtr->pitch, iconDataPtr->rle_pixel, bits);

			mem[index] = bits;
			iconDataPtr->rle_len[index] = len;

			/* Keep a list of component icons */
			for (i = 0; i < 4; i++)
			{
				if (iconDataPtr->dynamic_spec[index * 4 + i].type == -1)
				{
					iconDataPtr->dynamic_spec[index * 4 + i] = iconSpec;
					break;
				}
			}

			Tcl_FreeDebug((char *) tmp);
			
			break;
		}

		case IDX_COUNT:
		{
			IconPtr *newMem, *oldMem;
			int i, count, *newLen, *oldLen;
			unsigned char *newBounds, *oldBounds;
			IconSpec *oldSpec, *newSpec;
			short *oldFlags, *newFlags;

			if (objc == 3)
			{
				Tcl_SetIntObj(resultPtr, iconDataPtr->icon_count);
				break;
			}
			if (Tcl_GetIntFromObj(interp, objv[3], &count) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (count < 0)
			{
				return TCL_ERROR;
			}

			newMem = (IconPtr *) Tcl_AllocDebug(sizeof(IconPtr) * count);
			newBounds = (unsigned char *) Tcl_AllocDebug(sizeof(unsigned char) * count * 4);
			memset(newBounds, '\0', count * 4);
			newLen = (int *) Tcl_AllocDebug(sizeof(int) * count);
			newSpec = (IconSpec *) Tcl_AllocDebug(sizeof(IconSpec) * count * 4);
			newFlags = (short *) Tcl_AllocDebug(sizeof(short) * count);
			for (i = 0; i < count * 4; i++)
				newSpec[i].type = -1;
			for (i = 0; i < count; i++)
			{
				newMem[i] = NULL;
				newLen[i] = 0;
				newFlags[i] = 0;
			}
			oldMem = (IconPtr *) iconDataPtr->rle_data;
			oldBounds = iconDataPtr->rle_bounds;
			oldLen = iconDataPtr->rle_len;
			oldSpec = iconDataPtr->dynamic_spec;
			oldFlags = iconDataPtr->flags;
			if (oldMem)
			{
				for (i = 0; i < count; i++)
				{
					newMem[i] = oldMem[i];
					memcpy(newBounds + i * 4, oldBounds + i * 4, 4);
					newLen[i] = oldLen[i];
				}
				for ( ; i < iconDataPtr->icon_count; i++)
				{
					Tcl_FreeDebug((char *) oldMem[i]);
				}
				Tcl_FreeDebug((char *) oldMem);
				Tcl_FreeDebug((char *) oldBounds);
				Tcl_FreeDebug((char *) oldLen);

				for (i = 0; i < count * 4; i++)
					newSpec[i] = oldSpec[i];
				Tcl_FreeDebug((char *) oldSpec);

				Tcl_FreeDebug((char *) oldFlags);
			}
			iconDataPtr->icon_count = count;
			iconDataPtr->rle_data = (IconPtr) newMem;
			iconDataPtr->rle_bounds = newBounds;
			iconDataPtr->rle_len = newLen;
			iconDataPtr->dynamic_spec = newSpec;
			iconDataPtr->flags = newFlags;
			break;
		}
	}

    return TCL_OK;
}

/*
 * objcmd_makeicon --
 *
 * makeicon ?-makemask? ?-scaleup? ?--? iconSize imageFile dataFile
 */
static int objcmd_makeicon(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *CONST *objPtr;
	char buf[1024];
	int index;

	Tk_PhotoHandle photoH = NULL;
	char *imageFile = NULL, *dataFile = NULL;
	char *imageName = (char *) "MakeIconImage";
	int imageW, imageH;
	int length;
	t_icon_data iconData;
	XColor *xColorPtr = NULL;
	int result = TCL_ERROR;

	/* Hack - ignore parameter */
	(void) dummy;

	iconData.icon_data = NULL;
	iconData.depth = g_icon_depth;
	iconData.bypp = g_pixel_size;
	iconData.height = iconData.width = 0;
	imageW = imageH = 0;

	objPtr = objv + 1;
	objc -= 1;
	
	/* Scan arguments for options */
	while (objc > 0)
	{
		static cptr switches[] = {"-datafile", "-iconheight", "-iconwidth",
			"-imagefile", "-imageheight", "-imagewidth", "-transparent", NULL};
		int n = 2;

		/* Get the sub-option */
	    if (Tcl_GetIndexFromObj(interp, objPtr[0], (char **) switches, (char *) "switch",
			0, &index) != TCL_OK)
		{
			goto error;
	    }

		switch (index)
		{
			case 0: /* -datafile */
				dataFile = Tcl_GetStringFromObj(objPtr[1], NULL);
				break;

			case 1: /* -iconheight */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &iconData.height) != TCL_OK)
				{
					goto error;
				}
				break;

			case 2: /* -iconwidth */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &iconData.width) != TCL_OK)
				{
					goto error;
				}
				break;

			case 3: /* -imagefile */
				imageFile = Tcl_GetStringFromObj(objPtr[1], NULL);
				break;

			case 4: /* -imageheight */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &imageH) != TCL_OK)
				{
					goto error;
				}
				break;

			case 5: /* -imagewidth */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &imageW) != TCL_OK)
				{
					goto error;
				}
				break;

			case 6: /* -transparent */
			{
				Tk_Window tkwin = Tk_MainWindow(interp);
				if (xColorPtr)
					Tk_FreeColor(xColorPtr);
				xColorPtr = Tk_AllocColorFromObj(interp, tkwin, objPtr[1]);
				if (xColorPtr == NULL)
				{
					goto error;
				}
				break;
			}
		}

		objc -= n;
		objPtr += n;
	}

    if (objc || (dataFile == NULL) || (imageFile == NULL))
    {
		Tcl_WrongNumArgs(interp, 1, objv, NULL);
		goto error;
    }

	if ((iconData.width <= 0) || (iconData.height <= 0))
	{
		Tcl_SetStringObj(Tcl_GetObjResult(interp),
			format("invalid icon size \"%d x %d\"",
			iconData.width, iconData.height), -1);
		goto error;
	}

	iconData.length = iconData.width * iconData.height * g_pixel_size;

	if (imageW <= 0)
		imageW = iconData.width;
	if (imageH <= 0)
		imageH = iconData.height;

	/* FIXME */
	if ((imageW != iconData.width) && (imageW != 16))
	{
		Tcl_SetResult(interp, (char *) "can only scale a 16x16 image", TCL_STATIC);
		goto error;
	}

	/* Read the image file */
	length = sprintf(buf, "image create photo %s -file \"%s\"",
		imageName, imageFile);
	if (Tcl_EvalEx(interp, buf, length, TCL_EVAL_GLOBAL) != TCL_OK)
	{
		goto error;
	}

	/* Lookup the photo by name */
	photoH = Tk_FindPhoto(interp, imageName);

	/* The photo was not found */
	if (photoH == NULL)
	{
		goto error;
	}

	/* Convert 24-bit image to g_icon_depth-bit data */
	if (Image2Bits(interp, &iconData, photoH, imageW, imageH, xColorPtr) != TCL_OK)
	{
		goto error;
	}

	if (WriteIconFile(interp, dataFile, &iconData) != TCL_OK)
	{
		goto error;
	}

	result = TCL_OK;

error:
	if (xColorPtr)
		Tk_FreeColor(xColorPtr);
	if (iconData.icon_data)
		Tcl_FreeDebug((char *) iconData.icon_data);
	if (photoH)
	{
		length = sprintf(buf, "image delete %s", imageName);
		(void) Tcl_EvalEx(interp, buf, length, TCL_EVAL_GLOBAL);
	}
	return result;
}


/*
 * objcmd_icon --
 */
static int objcmd_icon(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	static cptr cmdOption[] = {"createtype", "count",
		"gettypes", "validate", "size", "ascii",
		"gamma", "photo", "makeicon", "depth", "dump",
		"rle", "height", "width", "dynamic", "duplicate",
		"transparent", "dark", "flags", "style", NULL};
	enum {IDX_CREATETYPE, IDX_COUNT,
		IDX_GETTYPES, IDX_VALIDATE, IDX_SIZE, IDX_ASCII,
		IDX_GAMMA, IDX_PHOTO, IDX_MAKEICON, IDX_DEPTH, IDX_DUMP,
		IDX_RLE, IDX_HEIGHT, IDX_WIDTH, IDX_DYNAMIC, IDX_DUPLICATE,
		IDX_TRANSPARENT, IDX_DARK, IDX_FLAGS, IDX_STYLE} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int index;

	Tcl_Obj *CONST *objPtr;
	Tcl_Obj *listObjPtr;
	char *typeName = NULL, *iconFile = NULL;
	char *fontName = NULL;
	char *charSet = NULL;
	Tcl_HashEntry *hPtr;
	int i;
	t_icon_data iconData, *iconDataPtr;
	IconSpec iconSpec = {-1, -1, -1, 0};

	/* Required number of arguments */
    if (objc < 2)
    {
		Tcl_WrongNumArgs(interp, 1, objv, (char *) "option ?arg ...?");
		return TCL_ERROR;
    }

	/* Get requested option */
    if (Tcl_GetIndexFromObj(interp, objv[1], (char **) cmdOption, (char *) "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (option)
	{
		case IDX_CREATETYPE: /* createtype */
			if (objc < 5)
			{
wrongCreateArgs:
				Tcl_WrongNumArgs(interp, 2, objv,
					(char *) "typeName -file fileName ?-maskfile fileName?");
				return TCL_ERROR;
			}

			/* Get the name of the new icon type */
			typeName = Tcl_GetStringFromObj(objv[2], NULL);

			/* Lookup the icon type by name */
			hPtr = Tcl_FindHashEntry(&g_icon_table, typeName);

			/* The icon type already exists */
			if (hPtr != NULL)
			{
				/* Set the error */
				Tcl_AppendStringsToObj(resultPtr, "icon type \"",
					typeName, "\" already exists", NULL);

				/* Failure */
				return TCL_ERROR;
			}

			/* Point to the first configuration option */
			objPtr = objv + 3;
			objc -= 3;

			iconData.dynamic = FALSE;
			iconData.icon_count = 0;
			iconData.icon_data = NULL;
			iconData.char_table = NULL;
			
			iconData.height = g_icon_size;
			iconData.width = g_icon_size;
			iconData.depth = g_icon_depth;
			iconData.bypp = g_pixel_size;

			/* Scan all option/value pairs */
			while (objc > 1)
			{
				static cptr createSwitch[] = {"-charset", "-file", "-font",
					"-height", "-width", "-dynamic", NULL};

			    if (Tcl_GetIndexFromObj(interp, objPtr[0], (char **) createSwitch,
					(char *) "switch", 0, &index) != TCL_OK)
				{
					return TCL_ERROR;
			    }

				switch (index)
				{
					case 0: /* -charset */
						charSet = Tcl_GetStringFromObj(objPtr[1], NULL);
						break;
						
					case 1: /* -file */
						if (iconFile != NULL) goto wrongCreateArgs;
						iconFile = Tcl_GetStringFromObj(objPtr[1], NULL);
						break;
						
					case 2: /* -font */
						fontName = Tcl_GetStringFromObj(objPtr[1], NULL);
						break;

					case 3: /* -height */
						if (Tcl_GetIntFromObj(interp, objPtr[1], &iconData.height) != TCL_OK)
						{
							return TCL_ERROR;
						}
						break;

					case 4: /* -width */
						if (Tcl_GetIntFromObj(interp, objPtr[1], &iconData.width) != TCL_OK)
						{
							return TCL_ERROR;
						}
						break;

					case 5: /* -dynamic */
						if (Tcl_GetBooleanFromObj(interp, objPtr[1], &iconData.dynamic) != TCL_OK)
						{
							return TCL_ERROR;
						}
						break;
				}

				/* Next option/value pair */
				objPtr += 2;
				objc -= 2;
			}

			/* Required number of arguments */
			if ((objc != 0) ||
				(!iconData.dynamic && (iconFile == NULL) && (fontName == NULL)) ||
				(iconData.height <= 0) || (iconData.width <= 0))
			{
				goto wrongCreateArgs;
			}

			/* Calculate some values */
			iconData.pitch = iconData.width * iconData.bypp;
			iconData.length = iconData.height * iconData.pitch;
			iconData.pixels = iconData.width * iconData.height;

			/* An icon file was specified */
			if (iconFile != NULL)
			{
				/* Read the data file */
				if (ReadIconFile(interp, iconFile, &iconData.icon_data,
					&iconData.icon_count) != TCL_OK)
				{
					/* Failure */
					return TCL_ERROR;
				}

				/* 256-color icons */
				if (iconData.depth == 8)
				{
					int j;

					/* Check each icon */
					for (i = 0; i < iconData.icon_count; i++)
					{
						/* Access the icon */
						IconPtr iconPtr = iconData.icon_data + i * iconData.length;

						/* Convert palette index to colormap index */
						for (j = 0; j < iconData.length; j++)
						{
							/* Convert palette index to colormap index */
							iconPtr[j] = g_palette2colormap[iconPtr[j]];
						}
					}
				}
			}

			/*
			 * A font description was given, meaning we are going to
			 * create a brand-new set of ascii type icons.
			 */
			if (fontName != NULL)
			{
				/* Get the requested font */
				iconData.font = Tk_GetFont(interp, Tk_MainWindow(interp),
					fontName);

				/* The font could not be created */
				if (iconData.font == NULL)
				{
					return TCL_ERROR;
				}

				/* A set of characters was specified */
				if (charSet != NULL)
				{
					/* The number of icons is the number of characters */
					iconData.icon_count = strlen(charSet);

					/*
					 * The char_table is used to remember
					 * which characters each icon represents.
					 */
					iconData.char_table = (int *)
						Tcl_AllocDebug(sizeof(int) * iconData.icon_count);

					/* Check each icon */
					for (i = 0; i < iconData.icon_count; i++)
					{
						/* Remember the character the i'th icon represents */
						iconData.char_table[i] = charSet[i];
					}
				}

				/* No character set specified, so use full printable set */
				else
				{
					iconData.icon_count = 126 - 32 + 1; /* printable only */
				}

				/* Allocate the icon data buffer */
				iconData.icon_data = (IconPtr) Tcl_AllocDebug(iconData.icon_count *
					iconData.length);

				/* Set the icon data using the desired font */
				if (init_ascii_data(interp, &iconData) != TCL_OK)
				{
					return TCL_ERROR;
				}
			}

			/* No font was specified */
			else
			{
				iconData.font = NULL;
			}

			iconData.desc = typeName;
			Icon_AddType(&iconData);	
			break;

		case IDX_COUNT: /* count */
			if (objc != 3)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "typeName");
				return TCL_ERROR;
			}

			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Return the number of icons */
			Tcl_SetIntObj(resultPtr, iconDataPtr->icon_count);
			break;

		case IDX_GETTYPES: /* gettypes */
			if (objc != 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, NULL);
				return TCL_ERROR;
			}

			/* Create a new Tcl list object */
			listObjPtr = Tcl_NewListObj(0, NULL);

			/* Check each icon type */
			for (i = 0; i < g_icon_data_count; i++)
			{
				/* Append icon type name as a string to the list object */
				Tcl_ListObjAppendElement(interp, listObjPtr,
					Tcl_NewStringObj(g_icon_data[i].desc, -1));
			}

			/* Return the list object */
			Tcl_SetObjResult(interp, listObjPtr);
			break;

		case IDX_VALIDATE: /* validate */
			if (objc < 6)
			{
				Tcl_WrongNumArgs(interp, 2, objv,
					(char *) "-type iconType -index iconIndex ?-ascii asciiIndex?");
				return TCL_ERROR;
			}
			if (Icon_ParseArgs(interp, objc, objv, 2, &iconSpec) != TCL_OK)
			{
				return TCL_ERROR;
			}
			break;
			
		case IDX_SIZE: /* size */
			Tcl_SetIntObj(resultPtr, g_icon_size);
			break;

		case IDX_ASCII: /* ascii */
			return objcmd_ascii(dummy, interp, objc - 1, objv + 1);

		case IDX_GAMMA: /* gamma */
		{
			double gamma;
			int i, j, first, last;
			TintTable table;

			if (objc < 4 || objc > 5)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "type gamma ?index?");
				return TCL_ERROR;
			}

			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the desired gamma value */
			if (Tcl_GetDoubleFromObj(interp, objv[3], &gamma) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the icon index */
			if (objc == 5)
			{
				if (Tcl_GetIntFromObj(interp, objv[4], &first) != TCL_OK)
				{
					return TCL_ERROR;
				}

				/* And the first shall be last */
				last = first;
			}
			else
			{
				first = 0;
				last = iconDataPtr->icon_count - 1;
			}

			/* Aaargh! */
			if (iconDataPtr->depth != 8)
				break;

			/* Initialize the 256-color gamma-corrected tint table */
			Colormap_GammaTable(gamma, table);

			/* Transparent */
			if (iconDataPtr->rle_data)
			{
				for (i = first; i <= last; i++)
				{
					IconPtr rlePtr = iconDataPtr->rle_data +
						iconDataPtr->rle_offset[i];

					/* Apply gamma-correction */
					while (1)
					{
						int trans, opaq;
						trans = rlePtr[0];
						opaq = rlePtr[1];
						if (!trans && !opaq)
							break;
						rlePtr += 2;
						for (j = 0; j < opaq; j++)
						{
							rlePtr[j] = table[rlePtr[j]];
						}
						rlePtr += opaq;
					}
				}
			}

			/* Unmasked */
			else
			{
				/* Check each icon */
				for (i = first; i <= last; i++)
				{
					/* Access the icon */
					IconPtr iconPtr = iconDataPtr->icon_data + i * iconDataPtr->length;

					/* Gamma-correct each byte */
					for (j = 0; j < iconDataPtr->length; j++)
					{
						/* Apply gamma-correction */
						iconPtr[j] = table[iconPtr[j]];
					}
				}
			}
			break;
		}

		case IDX_PHOTO: /* photo */
			return objcmd_icon_photo(dummy, interp, objc, objv);

		case IDX_MAKEICON: /* makeicon */
			return objcmd_makeicon(dummy, interp, objc - 1, objv + 1);

		case IDX_DEPTH: /* depth */
			Tcl_SetIntObj(resultPtr, g_icon_depth);
			break;

		case IDX_DUMP: /* dump */
		{
#if 0
			int y, x;
			PixelPtr p;

			if (Icon_ParseArgs(interp, objc, objv, 2, &iconSpec) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Access the icon data */
			iconDataPtr = &g_icon_data[iconSpec.type];

			if (iconDataPtr->mask_count)
			{
				p.pix8 = iconDataPtr->mask_bits +
					iconDataPtr->mask_table[iconSpec.index] *
					iconDataPtr->length;
			}
			else
			{
				p.pix8 = iconDataPtr->icon_data + iconSpec.index
					* iconDataPtr->length;
			}
			for (y = 0; y < iconDataPtr->height; y++)
			{
				for (x = 0; x < iconDataPtr->width; x++)
				{
					switch (iconDataPtr->depth)
					{
						case 8:
							Tcl_AppendResult(interp, format("%02x ", *p.pix8++), NULL);
							break;
						case 16:
							Tcl_AppendResult(interp, format("%04x ", (int) *p.pix16++), NULL);
							break;
						case 24:
							Tcl_AppendResult(interp, format("%02x%02x%02x ",
								p.pix8[0], p.pix8[1], p.pix8[2]), NULL);
							p.pix8 += 3;
							break;
					}
				}
				Tcl_AppendResult(interp, "\n", NULL);
			}
#endif
			break;
		}

		case IDX_RLE: /* rle */
		{
			XColor *xColorPtr;
			Tk_Window tkwin = Tk_MainWindow(interp);

			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the transparent color */
			xColorPtr = Tk_AllocColorFromObj(interp, tkwin, objv[3]);
			if (xColorPtr == NULL)
			{
				return TCL_ERROR;
			}
			iconDataPtr->rle_pixel = Plat_XColor2Pixel(xColorPtr); 

			/* Check size because 'dynamic' needs rle_pixel too */
			if (iconDataPtr->icon_count)
				Icon_MakeRLE(iconDataPtr);

			Tk_FreeColor(xColorPtr);
			break;
		}

		case IDX_HEIGHT: /* height */
		{
			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			Tcl_SetIntObj(resultPtr, iconDataPtr->height);
			break;
		}

		case IDX_WIDTH: /* width */
		{
			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			Tcl_SetIntObj(resultPtr, iconDataPtr->width);
			break;
		}

		case IDX_DYNAMIC: /* dynamic */
			return objcmd_icon_dynamic(dummy, interp, objc - 1, objv + 1); 

		case IDX_DUPLICATE: /* duplicate */
		{
			int index, count;

			if (objc != 5)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "type index count");
				return TCL_ERROR;
			}
		
			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (iconDataPtr->dynamic || iconDataPtr->rle_data || iconDataPtr->font)
				return TCL_ERROR;

			/* Get the icon index */
			if (Icon_GetIndexFromObj(interp, &index, objv[3], iconDataPtr)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the number of duplicates */
			if (Tcl_GetIntFromObj(interp, objv[4], &count) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (count < 0)
				return TCL_ERROR;
			if (!count)
				break;

			/* Allocate more space */
			iconDataPtr->icon_data = (unsigned char *) Tcl_Realloc((char *) iconDataPtr->icon_data,
				(iconDataPtr->icon_count + count) * iconDataPtr->length);

			/* Move following icons down */
			memcpy(
				iconDataPtr->icon_data + (index + 1 + count) * iconDataPtr->length,
				iconDataPtr->icon_data + (index + 1) * iconDataPtr->length,
				(iconDataPtr->icon_count - index - 1) * iconDataPtr->length);

			/* Copy icon */
			for (i = 0; i < count; i++)
			{
				memcpy(
					iconDataPtr->icon_data + (index + 1 + i) * iconDataPtr->length,
					iconDataPtr->icon_data + index * iconDataPtr->length,
					iconDataPtr->length);
			}
					
			/* New number of icons */
			iconDataPtr->icon_count += count;
			break;
		}

		case IDX_TRANSPARENT: /* transparent */
		{
			int trans = TRUE;

			if (objc != 4)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "type index");
				return TCL_ERROR;
			}

			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the icon index */
			if (Icon_GetIndexFromObj(interp, &index, objv[3], iconDataPtr)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Not transparent */
			if (iconDataPtr->rle_data == NULL)
				trans = FALSE;
				
			Tcl_SetBooleanObj(resultPtr, trans);
			break;
		}

		case IDX_DARK: /* dark */
			return objcmd_icon_dark(dummy, interp, objc, objv);

		case IDX_FLAGS: /* flags */
		{
			static cptr flags[] = {"left", "right", "isohack", NULL};
			int flag;
			
			if (objc < 4 || objc > 6)
			{
				Tcl_WrongNumArgs(interp, 2, objv, (char *) "type index ?flag? ?boolean?");
				return TCL_ERROR;
			}
		
			/* Lookup the icon type by name */
			if (Icon_GetTypeFromObj(interp, &iconDataPtr, objv[2]) != TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Get the icon index */
			if (Icon_GetIndexFromObj(interp, &index, objv[3], iconDataPtr)
				!= TCL_OK)
			{
				return TCL_ERROR;
			}

			/* Return value of all flags */
			if (objc == 4)
			{
				break;
			}

		    if (Tcl_GetIndexFromObj(interp, objv[4], (char **) flags, (char *) "flag", 0, 
				(int *) &flag) != TCL_OK)
			{
				return TCL_ERROR;
		    }
		    ++flag;

			/* Return value of one flag */
			if (objc == 5)
			{
				Tcl_SetBooleanObj(resultPtr,
					(iconDataPtr->flags[index] & (1L << flag)) != 0);
				break;
			}

			/* Set value of one flag */
			iconDataPtr->flags[index] |= (1L << flag);

			break;
		}

		case IDX_STYLE: /* style */
		{
			static cptr keyword_icon_style[] = {"icon", "iso", NULL};
			if (objc == 3)
			{
			    if (Tcl_GetIndexFromObj(interp, objv[2], (char **) keyword_icon_style,
			    	(char *) "style", 0, &g_icon_style) != TCL_OK)
				{
					return TCL_ERROR;
				}
				break;
			}
			Tcl_SetResult(interp, (char *) keyword_icon_style[g_icon_style],
				TCL_STATIC);
			break;
		}
	}

	/* Success */
	return TCL_OK;
}


int Icon_FindTypeByName(Tcl_Interp *interp, int *typeIndexPtr, char *typeName)
{
	Tcl_HashEntry *hPtr;

	/* Look up the icon type by name */
	hPtr = Tcl_FindHashEntry(&g_icon_table, typeName);

	/* The icon type was not found */
	if (hPtr == NULL)
	{
		Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

		/* Set the error */
		Tcl_AppendStringsToObj(resultPtr, "unknown icon type \"",
			typeName, "\"", NULL);

		/* Failure */
		return TCL_ERROR;
	}

	(*typeIndexPtr) = (int) Tcl_GetHashValue(hPtr);

	/* Success */
	return TCL_OK;
}

int Icon_GetTypeFromObj(Tcl_Interp *interp, t_icon_data **typePtrPtr,
	Tcl_Obj *objPtr)
{
	int typeIndex;

	if (Icon_FindTypeByName(interp, &typeIndex, Tcl_GetString(objPtr))
		!= TCL_OK)
	{
		return TCL_ERROR;
	}

	(*typePtrPtr) = &g_icon_data[typeIndex];

	return TCL_OK;
}

int Icon_GetIndexFromObj(Tcl_Interp *interp, int *indexPtr,
	Tcl_Obj *objPtr, t_icon_data *iconDataPtr)
{
	int index;

	if (Tcl_GetIntFromObj(interp, objPtr, &index) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if ((index < 0) || (index >= iconDataPtr->icon_count))
	{
		/* Set the error */
		Tcl_SetStringObj(Tcl_GetObjResult(interp),
			format("bad icon index \"%d\": must be from 0 to %d",
			index, iconDataPtr->icon_count - 1), -1);

		/* Failure */
		return TCL_ERROR;
	}

	(*indexPtr) = index;

	return TCL_OK;
}

int Icon_Validate(Tcl_Interp *interp, char *typeName, int index, int ascii,
	IconSpec *specPtr)
{
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	t_icon_data *iconDataPtr;
	int type;

	/* Look up the icon type by name */
	if (Icon_FindTypeByName(interp, &type, typeName) != TCL_OK)
	{
		return TCL_ERROR;
	}

	iconDataPtr = &g_icon_data[type];

	/* Font-icon must specify ascii */
	if ((iconDataPtr->font != NULL) && (ascii == -1))
	{
		Tcl_AppendStringsToObj(resultPtr, "icon type \"", typeName, "\"",
			" is ascii", NULL);
		return TCL_ERROR;
	}

	/* Non-font-icon must not specify ascii */
	if ((iconDataPtr->font == NULL) && (ascii != -1))
	{
		Tcl_AppendStringsToObj(resultPtr, "icon type \"", typeName, "\"",
			" is not ascii", NULL);
		return TCL_ERROR;
	}

	/* Verify index */
	if ((index < 0) || (index >= iconDataPtr->icon_count))
	{
		/* Set the error */
		Tcl_SetStringObj(Tcl_GetObjResult(interp),
			format("bad icon index \"%d\": must be from 0 to %d",
			index, iconDataPtr->icon_count - 1), -1);

		/* Failure */
		return TCL_ERROR;
	}

	if ((ascii != -1) && (ascii >= g_ascii_count))
	{
		/* Set the error */
		Tcl_SetStringObj(Tcl_GetObjResult(interp),
			format("bad ascii index \"%d\": "
			"must be from 0 to %d", ascii,
			g_ascii_count - 1), -1);

		/* Failure */
		return TCL_ERROR;
	}

	specPtr->type = type;
	specPtr->index = index;
	specPtr->ascii = ascii;

	/* Success */
	return TCL_OK;
}

/*
 * Scans the given object argument array and fills 'iconPtr' with the
 * corresponding type and index. Sets the interpreter result with an
 * error if the specified icon type or index is not valid.
 */
int Icon_ParseArgs(Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[],
	int offset, IconSpec *specPtr)
{
	int index;
	Tcl_Obj *CONST *objPtr;
	char *typeName = NULL;
	IconSpec iconSpec;

	objPtr = objv + offset;
	objc -= offset;

	iconSpec.type = -1;
	iconSpec.index = -1;
	iconSpec.ascii = -1;

	while (objc > 1)
	{
		static cptr assignSwitch[] = {"-ascii", "-index", "-type", NULL};

	    if (Tcl_GetIndexFromObj(interp, objPtr[0], (char **) assignSwitch,
			(char *) "switch", 0, &index) != TCL_OK)
		{
			return TCL_ERROR;
	    }
		switch (index)
		{
			case 0: /* ascii */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &iconSpec.ascii)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				break;

			case 1: /* -index */
				if (Tcl_GetIntFromObj(interp, objPtr[1], &iconSpec.index)
					!= TCL_OK)
				{
					return TCL_ERROR;
				}
				break;

			case 2: /* -type */
				typeName = Tcl_GetStringFromObj(objPtr[1], NULL);
				break;

		}
		objPtr += 2;
		objc -= 2;
	}

	/* Required number of arguments */
	if ((objc != 0) || (typeName == NULL) || (iconSpec.index == -1))
	{
		/* Set the error */
		Tcl_WrongNumArgs(interp, offset, objv,
			(char *) "-type iconType -index iconIndex ?-ascii asciiIndex?");

		/* Failure */
		return TCL_ERROR;
	}

	if (Icon_Validate(interp, typeName, iconSpec.index, iconSpec.ascii,
		specPtr) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Success */
	return TCL_OK;
}

/*
 *
 */
void Icon_AddType(t_icon_data *data)
{
	int new;
	t_icon_data iconData, *icon_data_ptr = &iconData;
	Tcl_HashEntry *hPtr;
	memset(icon_data_ptr, 0, sizeof(t_icon_data));
	icon_data_ptr->desc = string_make(data->desc);
	icon_data_ptr->icon_count = data->icon_count;
	icon_data_ptr->icon_data = data->icon_data;
	icon_data_ptr->char_table = data->char_table;
	icon_data_ptr->font = data->font;
	icon_data_ptr->dynamic = data->dynamic;
	icon_data_ptr->depth = data->depth;
	icon_data_ptr->bypp = data->bypp;
	icon_data_ptr->width = data->width;
	icon_data_ptr->height = data->height;
	icon_data_ptr->pitch = data->pitch;
	icon_data_ptr->length = data->length;
	icon_data_ptr->pixels = data->pixels;

	/* Could be dynamic */
	if (data->icon_count)
	{
		icon_data_ptr->flags = (short *) Tcl_AllocDebug(
			sizeof(short) * icon_data_ptr->icon_count);
		memset(icon_data_ptr->flags, 0,
			sizeof(short) * icon_data_ptr->icon_count);
	}

	g_icon_data = Array_Append(g_icon_data, &g_icon_data_count,
		sizeof(t_icon_data), icon_data_ptr);
	
	hPtr = Tcl_CreateHashEntry(&g_icon_table, data->desc, &new);
	Tcl_SetHashValue(hPtr, (ClientData) (g_icon_data_count - 1));
}

/*
 * Initialization.
 */
int Icon_Init(Tcl_Interp *interp, int size, int depth)
{
	/* Only initialize once */
	if (g_icon_size != 0) return TCL_OK;

#ifdef USE_TCL_STUBS
	if (Tcl_InitStubs(interp, "8.2", 0) == NULL)
	{
		return TCL_ERROR;
	}
	
	if (Tk_InitStubs(interp, "8.2", 0) == NULL)
	{
		return TCL_ERROR;
	}
#endif /* USE_TCL_STUBS */

	/* Require a known icon size */
	if ((size != 16) && (size != 24) && (size != 32))
	{
		Tcl_SetStringObj(Tcl_GetObjResult(interp),
			format("invalid icon size \"%d\": must be 16, 24 or 32", size), -1);
		return TCL_ERROR;
	}

	/* Require a known icon depth */
	if ((depth != 8) && (depth != 16) && (depth != 24))
	{
		Tcl_SetStringObj(Tcl_GetObjResult(interp),
			format("invalid icon depth \"%d\": must be 8, 16 or 24", depth), -1);
		return TCL_ERROR;
	}

	/* Remember the requested icon size */
	g_icon_size = size;

	/* Remember the requested icon depth */
	g_icon_depth = depth;

	/* Hacks-a-plenty */
	g_icon_style = ICON_STYLE_ICON;

	if (InitPixelSize(interp) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Remember number of pixels in an icon */
	g_icon_pixels = size * size;

	/* Remember the number of bytes in an icon */
	g_icon_length = g_icon_pixels * g_pixel_size;

	/***** RETURN ERROR IF PALETTE NOT INITIALIZED *****/

	g_palette_rgb = Palette_GetRGB();

	if (g_icon_depth == 16)
		InitRGBInfo(interp);

	{
		int i, paletteIndex;
		for (i = 0; i < 16; i++)
		{
			paletteIndex = g_term_palette[i];
			switch (g_icon_depth)
			{
				case 8:
					g_term_colormap[i] = g_palette2colormap[paletteIndex];
					break;
				case 16:
				{
					unsigned short pix16;
					unsigned char *rgb = &g_palette_rgb[paletteIndex * 3];
					SetPix16((unsigned char *) &pix16, rgb[0], rgb[1], rgb[2]);
					g_term_colormap[i] = pix16;
					break;
				}
				case 24:
				{
					unsigned char *rgb = &g_palette_rgb[paletteIndex * 3];
					unsigned char *pix24 = (unsigned char *) &g_term_colormap[i];
					SetPix24(pix24 + 1, rgb[0], rgb[1], rgb[2]);
					break;
				}
			}
		}
	}
	
	/*
	 * This is an array of t_icon_data types, which specify
	 * the icon data and optional mask data for each type of
	 * icon. Icon types are defined through the "icon createtype"
	 * command.
	 */
	g_icon_data = Array_New(1, sizeof(t_icon_data));
	g_icon_data_count = 0;

	/* Array of ascii configurations */
	g_ascii = Array_New(1, sizeof(t_ascii));
	g_ascii_count = 0;

	/*
	 * This hash table maps symbolic names of icon types (as defined
	 * through the "icon createtype" command) to indexes into
	 * the g_icon_data[] array above.
	 */
	Tcl_InitHashTable(&g_icon_table, TCL_STRING_KEYS);

	Tcl_CreateObjCommand(interp, (char *) "icon", objcmd_icon, NULL, NULL);
	
	return TCL_OK;
}

void Icon_Exit(Tcl_Interp *interp)
{
	int i, j;

	/* Hack - ignore parameter */
	(void) interp;

	if (g_icon_size == 0) return;

	/* Check each icon type */
	for (i = 0; i < g_icon_data_count; i++)
	{
		t_icon_data *iconDataPtr = &g_icon_data[i];

		/* This is an "ascii" icon type */
		if (iconDataPtr->font != NULL)
		{
			/* Important: free the font */
			Tk_FreeFont(g_icon_data[i].font);
		}

		/* Help the memory debugger */
		if (iconDataPtr->icon_data)
			Tcl_FreeDebug((char *) iconDataPtr->icon_data);

		/* Help the memory debugger */
		if (iconDataPtr->rle_data)
		{
			if (iconDataPtr->dynamic)
			{
				IconPtr *mem = (IconPtr *) iconDataPtr->rle_data;
				for (j = 0; j < iconDataPtr->icon_count; j++)
					if (mem[j])
						Tcl_FreeDebug((char *) mem[j]);
			}
			else
			{
				Tcl_FreeDebug((void *) iconDataPtr->rle_offset);
				Tcl_FreeDebug((void *) iconDataPtr->rle_len);
			}
			Tcl_FreeDebug((void *) iconDataPtr->rle_bounds);
		}

		/* Help the memory debugger */
		Tcl_FreeDebug((void *) iconDataPtr->flags);

		/* Help the memory debugger */
		Tcl_FreeDebug((void *) iconDataPtr->gamma[0]);
		Tcl_FreeDebug((void *) iconDataPtr->gamma[1]);

		if (iconDataPtr->dark_data)
		{
			for (j = 0; j < iconDataPtr->icon_count; j++)
				if (iconDataPtr->dark_data[j])
					Tcl_FreeDebug((char *) iconDataPtr->dark_data[j]);
		}
	}
}
