/* File: util-dll.c */

/* Purpose: misc stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include <tkFont.h>
#include <tkMenu.h>

#include <limits.h>
#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif

#define MAX_DELTA 195076 /* 255^2 + 255^2 + 255^2 + 1 */
#define MAX_COLOR_ENTRY 1021

/* Structure for a hash table used by rgb2index() */
typedef struct
{
	int valid; /* The hash-table entry is valid */
	unsigned long pixel; /* an RGB pixel value */
	int index; /* closest matching palette index for 'pixel' */
} t_color_entry;

typedef struct IndexedColor IndexedColor;
struct IndexedColor
{
	unsigned char rgb[256 * 3];
	t_color_entry hash[MAX_COLOR_ENTRY];
	void *platData;
};

static IndexedColor g_palette;
static int Palette_Initialized = 0;

int g_palette_black = 255;
int g_palette_white = 0;
int g_colormap_black;
int g_colormap_white;

unsigned char g_colormap2palette[256];
unsigned char g_palette2colormap[256];

/*
 * Allocate a new array
 */
void *Array_New(int count, int elem_size)
{
	char *p = Tcl_Alloc(count * elem_size);
	return (void *) p;
}

/*
 * Append an element to an array
 */
void *Array_Append(void *array_ptr, int *count, int elem_size,
	void *elem_ptr)
{
	char *ap = array_ptr;
	int n = (*count) + 1;

	ap = Tcl_Realloc(ap, n * elem_size);
	(void) memcpy(ap + (n - 1) * elem_size, elem_ptr, elem_size);
	(*count) = n;
	return ap;
}

/*
 * Insert an element in an array
 */
void *Array_Insert(void *array_ptr, int *count, int elem_size,
	int index)
{
	char *ap = array_ptr;
	int n = (*count) + 1;

	if (index < 0) index = 0;
	if (index >= n) index = n - 1;

	ap = Tcl_Realloc(ap, n * elem_size);
	if (index != n - 1)
	{
		(void) memcpy(ap + (index + 1) * elem_size,
			ap + index * elem_size,
			(n - index - 1) * elem_size);
	}
	else
	{
		memset(ap + index * elem_size, 0, elem_size);
	}
	(*count) = n;
	return ap;
}

/*
 * Delete an element from an array
 */
void *Array_Delete(void *array_ptr, int *count, int elem_size,
	int index)
{
	char *ap = array_ptr;
	int i, n = (*count) - 1;

	if (index < 0) index = 0;
	if (index > n) index = n;

	if (index != n)
	{
		(void) memcpy(ap + index * elem_size,
			ap + (index + 1) * elem_size,
			(n - index) * elem_size);
	}

	/* Zero the trailing slot to catch errors */
	for (i = 0; i < elem_size; i++)
	{
		ap[n * elem_size + i] = 0;
	}
	
	(*count) = n;
	return (void *) Tcl_Realloc(ap, n * elem_size);
}

static void IndexedColor_ResetHash(IndexedColor *idc)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		idc->hash[i].valid = 0;
	}
}

/*
 * Returns the nearest matching palette index for the given
 * RGB values.
 */
static int IndexedColor_RGB2Index(IndexedColor *idc, unsigned char r, unsigned char g, unsigned char b)
{
	int i, delta, index, sum, max;
	unsigned char *col;
	unsigned long pixel;
	t_color_entry *entry;

	/* Calculate the pixel value */
	pixel = (r << 16) | (g << 8) | b;

	/* Hash the pixel value */
	entry = &idc->hash[pixel % MAX_COLOR_ENTRY];

	/* We already calculated the palette index */
	if (entry->valid && (entry->pixel == pixel))
	{
		/* Return the palette index */
		return entry->index;
    }

	index = 0;
	max = MAX_DELTA;

	col = idc->rgb;

	/* Check each palette entry */
	for (i = 0; i < 256; i++)
	{
		/*
		 * Calculate the minimum difference between the given RGB value
		 * and each of the palette entries
		 */
		delta = r - *col++; sum = delta * delta;
		delta = g - *col++; sum += delta * delta;
		delta = b - *col++; sum += delta * delta;

		/* This palette entry is a better match than any other so far */
		if (sum < max)
		{
			/* Remember the palette index */
			index = i;

			/* Remember the minimum difference */
			max = sum;
		}
	}

	/* Remember the hash table entry info */
	entry->pixel = pixel;
	entry->index = index;
	entry->valid = 1;

	/* Return the palette index */
	return index;
}

/*
 * Calculate the 256 palette indices that the palette index 'tint'
 * looks like at the given opacity against each of the standard
 * palette colors.
 */
static void IndexedColor_TintTable(IndexedColor *idc, int tint, int opacity, TintTable table)
{
	int i;
	long v1, v2, m;
	int r1, g1, b1, r2, g2, b2;
	unsigned char *rgb = idc->rgb;

#define RGB_MAX_S 127
#define RGB_MAX_U 255

	m = opacity;

	/* Get the RGB values for the given palette index */
	r2 = rgb[tint * 3];
	g2 = rgb[tint * 3 + 1];
	b2 = rgb[tint * 3 + 2];

	/* Check each palette entry */
	for (i = 0; i < 256; ++i)
	{
		/* Get the i'th RGB values */
		r1 = rgb[i * 3];
		g1 = rgb[i * 3 + 1];
		b1 = rgb[i * 3 + 2];

		/*
		 * Simple linear equation using opacity as "slope"
		 */

		v1 = r1;
		v2 = r2;
		r1 = v1 + ( m * ( v2 - v1 ) + (RGB_MAX_S) ) / RGB_MAX_U;
		
		v1 = g1;
		v2 = g2;
		g1 = v1 + ( m * ( v2 - v1 ) + (RGB_MAX_S) ) / RGB_MAX_U;
		
		v1 = b1;
		v2 = b2;
		b1 = v1 + ( m * ( v2 - v1 ) + (RGB_MAX_S) ) / RGB_MAX_U;

		/* Now we have the RGB value, convert it into a palette index */
		table[i] = (TintValue) IndexedColor_RGB2Index(idc, r1, g1, b1);
	}
}

/*
 * Return gamma-corrected intensity. This is usually called on
 * each component of an RGB value.
 */
int gamma_correct(int value, double gamma)
{
#if 0 /* Don't use the math library... use util.c */
	double ind;
	double inverse;

	if (gamma)
	{
		inverse = 1.0 / gamma;
	}
	else
	{
		inverse = 1.0;
	}
	ind = (double) value / 256.0;
	return (int) (256 * pow(ind, inverse));
#endif /* 0 */
	/* Hack - ignore gamma value now */
	(void) gamma;

	return (value);
}

/* 
 * Writes 256 palette indices into the given tint table. Each
 * index is calculated by applying a gamma correction to each
 * RGB value of the global palette, then finding the nearest
 * palette index for the gamma-corrected RGB.
 */
static void IndexedColor_GammaTable(IndexedColor *idc, double gamma, TintTable tint)
{
	int i, r, g, b;
	unsigned char *rgb = idc->rgb;

	/* Check each palette index */
	for (i = 0; i < 256; i++)
	{
		/* Gamma-correct each RGB intensity */
		r = gamma_correct(rgb[i * 3], gamma);
		g = gamma_correct(rgb[i * 3 + 1], gamma);
		b = gamma_correct(rgb[i * 3 + 2], gamma);

		/* Find nearest palette index for RGB values */
		tint[i] = IndexedColor_RGB2Index(idc, r, g, b);
	}
}

/*
 * Returns the given value adjusted for brightness.
 * This is usually called on each component of an RGB value.
 */
static int brightness_value(int intensity, int value)
{
	double brightValue = intensity;

	/* Darken */
	if (brightValue < 0)
	{
		return (unsigned char) ((value * (255 + brightValue)) / 255);
	}
	
	/* Lighten */
	else
	{
		return (unsigned char) (value + ((255 - value) * brightValue) / 255);
	}
}

/*
 * Adjusts the given 256 palette indices for the given intensity.
 * 'Intensity' can be from -127 to +127 inclusive.
 */
static void IndexedColor_BrightnessTable(IndexedColor *idc, int intensity, TintTable table)
{
	int i, r, g, b;
	unsigned char *rgb = idc->rgb;

	for (i = 0; i < 256; i++)
	{
		/* Get the i'th palette index */
		int n = table[i];

		/* Get the n'th RGB values */
		r = rgb[n * 3 + 0];
		g = rgb[n * 3 + 1];
		b = rgb[n * 3 + 2];

		/* Adjust brightness */
		r = brightness_value(intensity, r);
		g = brightness_value(intensity, g);
		b = brightness_value(intensity, b);
		
		/* Now we have the RGB value, convert it into a palette index */
		table[i] = IndexedColor_RGB2Index(idc, r, g, b);
	}
}


#if 0
/*
 * Returns the given value adjusted for contrast.
 * This is usually called on each component of an RGB value.
 */
static int contrast_value(int intensity, int value)
{
	int v;
	double contrastValue = intensity;
	double x, y;

#define CLIPVALUE(a,x,y) ((a < x) ? x : ((a > y) ? y : a))

	/* Less contrast */
	if (contrastValue < 0)
	{
		v = (value > 127) ? (255 - value) : value;
		x = (v ? v : 1) / 127.0;
		y = (127 + contrastValue) / 127.0;
		v = (int) (127.0 * pow(x, y));
		v = CLIPVALUE(v, 0, 255);
		return (value > 127) ? (255 - v) : v;
	}
	
	/* More contrast */
	else
	{
		v = (value > 127) ? (255 - value) : value;
		x = v / 127.0;
		y = (contrastValue == 127) ? 127 : 127.0 / (127 - contrastValue);
		v = (int) (127.0 * pow(x, y));
		v = CLIPVALUE(v, 0, 255);
		return (value > 127) ? (255 - v) : v;
	}
}
#endif /* 0 */


/*
 * Adjusts the given 256 palette indices for the given contrast.
 * 'Intensity' can be from -127 to +127 inclusive.
 */
static void IndexedColor_ContrastTable(IndexedColor *idc, int intensity, TintTable table)
{
	int i, r, g, b;
	unsigned char *rgb = idc->rgb;
	
	for (i = 0; i < 256; i++)
	{
		/* Get the i'th palette index */
		int n = table[i];

		/* Get the n'th RGB values */
		r = rgb[n * 3 + 0];
		g = rgb[n * 3 + 1];
		b = rgb[n * 3 + 2];
#if 0
		/* Adjust contrast */
		r = contrast_value(intensity, r);
		g = contrast_value(intensity, g);
		b = contrast_value(intensity, b);
#endif /* 0 */
		/* Ignore unused parameter due to above */
		(void) intensity;
	
		/* Now we have the RGB value, convert it into a palette index */
		table[i] = IndexedColor_RGB2Index(idc, r, g, b);
	}
}

/*
 * Create a tint table that maps to the same palette index
 */
static void IndexedColor_One2OneTable(TintTable table)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		table[i] = i;
	}
}

/* nearest $color */
static int objcmd_palette_nearest(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
/*	int objC = objc - infoCmd->depth; */
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	XColor *xColorPtr;
	int nearest;

	/* Hack - ignore parameter */
	(void) objc;

	xColorPtr = Tk_AllocColorFromObj(interp, Tk_MainWindow(interp), objV[1]);
	if (xColorPtr == NULL)
	{
		return TCL_ERROR;
	}

	nearest = Palette_RGB2Index(
		((double) xColorPtr->red / USHRT_MAX) * 255,
		((double) xColorPtr->green / USHRT_MAX) * 255,
		((double) xColorPtr->blue / USHRT_MAX) * 255);

	Tk_FreeColor(xColorPtr);

	Tcl_SetObjResult(interp, Tcl_NewIntObj(nearest));

	return TCL_OK;
}

/* set $index ?$color? */
static int objcmd_palette_set(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;
	char buf[20];
	int i, r, g, b;

	if (Tcl_GetIntFromObj(interp, objV[1], &i) != TCL_OK)
	{
		return TCL_ERROR;
	}
	if ((i < 0) || (i >= 256))
	{
		return TCL_ERROR;
	}

	if (objC == 3)
	{
		XColor *xColorPtr = Tk_AllocColorFromObj(interp, Tk_MainWindow(interp), objV[2]);
		if (xColorPtr == NULL)
		{
			return TCL_ERROR;
		}
		g_palette.rgb[i * 3] = ((double) xColorPtr->red / USHRT_MAX) * 255;
		g_palette.rgb[i * 3 + 1] = ((double) xColorPtr->green / USHRT_MAX) * 255;
		g_palette.rgb[i * 3 + 2] = ((double) xColorPtr->blue / USHRT_MAX) * 255;
		Tk_FreeColor(xColorPtr);
		return TCL_OK;
	}

	r = g_palette.rgb[i * 3];
	g = g_palette.rgb[i * 3 + 1];
	b = g_palette.rgb[i * 3 + 2];
	(void) sprintf(buf, "#%02X%02X%02X", r, g, b);
	Tcl_SetResult(interp, buf, TCL_VOLATILE);

	return TCL_OK;
}

static CommandInit commandInit[] = {
	{0, "palette", 0, 0, NULL, NULL, (ClientData) 0},
		{1, "nearest", 2, 2, "color", objcmd_palette_nearest, (ClientData) 0},
		{1, "set", 2, 3, "?color?", objcmd_palette_set, (ClientData) 0},
	{0, NULL, 0, 0, NULL, NULL, 0}
};

int Palette_Init(Tcl_Interp *interp, char *fileName)
{
	FILE *fp;
	int count = 0, r, g, b;
	char buf[80], errorMsg[512];
	unsigned char *rgb;

	if (Palette_Initialized) return TCL_OK;

	rgb = g_palette.rgb;
	g_palette.platData = NULL;

	/* Try to open the palette file */
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		(void) sprintf(errorMsg,
			"can't open palette file \"%s\"", fileName);
		goto error;
	}

	/* Read each line until done */
	while (!feof(fp))
	{
		/* Get a line */
		if (!fgets(buf, 80, fp)) continue;

		/* Skip comments */
		if (buf[0] == '#') continue;

		/* Require RGB triplet */
		if (sscanf(buf, "%d %d %d", &r, &g, &b) != 3) continue;

		/* Write this color to the array */
		*rgb++ = r, *rgb++ = g, *rgb++ = b;

		/* Count the colors */
		count++;
	}

	/* Close the file */
	fclose(fp);

	/* Require 256 colors exactly */
	if (count != 256)
	{
		/* Fatal error */
		(void) sprintf(errorMsg,
			"expected 256 colors, got \"%d\"", count);
		goto error;
	}

	g_palette.platData = Plat_PaletteInit(g_palette.rgb);

	(void) CommandInfo_Init(interp, commandInit, NULL);

	Palette_ResetHash();

	Palette_Initialized = 1;

	Colormap_Init(interp);

	return TCL_OK;

error:
	/* Set the error */
	Tcl_SetStringObj(Tcl_GetObjResult(interp), errorMsg, -1);

	/* Failure */
	return TCL_ERROR;
}


void Palette_ResetHash(void)
{
	IndexedColor_ResetHash(&g_palette);
}

/*
 * Returns the nearest matching palette index for the given
 * RGB values.
 */
int Palette_RGB2Index(unsigned char r, unsigned char g, unsigned char b)
{
	return IndexedColor_RGB2Index(&g_palette, r, g, b);
}

/*
 * Calculate the 256 palette indices that the palette index 'tint'
 * looks like at the given opacity against each of the standard
 * palette colors.
 */
void Palette_TintTable(int tint, int opacity, TintTable table)
{
	IndexedColor_TintTable(&g_palette, tint, opacity, table);
}

/* 
 * Writes 256 palette indices into the given tint table. Each
 * index is calculated by applying a gamma correction to each
 * RGB value of the global palette, then finding the nearest
 * palette index for the gamma-corrected RGB.
 */
void Palette_GammaTable(double gamma, unsigned char *tint)
{
	IndexedColor_GammaTable(&g_palette, gamma, tint);
}

#ifdef PLATFORM_WIN

void *Palette_GetHPal(void)
{
	return g_palette.platData; /* HPALETTE */
}

#endif /* PLATFORM_WIN */

unsigned char *Palette_GetRGB(void)
{
	return g_palette.rgb;
}

void DoubleLink_Init(DoubleLinker *linker, DoubleLink *link, void *data)
{
	if (!link)
	{
		linker->count = 0;
		linker->what = "unknown";
		linker->head = NULL;
		linker->tail = NULL;
	}

	if (link)
	{
		link->isLinked = FALSE;
		link->linker = linker;
		link->prev = NULL;
		link->next = NULL;
		link->data = data;
	}
}

void DoubleLink_Link(DoubleLink *link)
{
	DoubleLinker *linker = link->linker;

	if (link->isLinked) return;

	if (linker->head == NULL)
	{
		linker->head = link;
	}
	else
	{
		link->prev = linker->tail;
		linker->tail->next = link;
	}
	link->isLinked = TRUE;
	linker->tail = link;
	++linker->count;
	
}

void DoubleLink_Unlink(DoubleLink *link)
{
	DoubleLinker *linker = link->linker;
	
	if (!link->isLinked) return;
	
	if (link->prev)
		link->prev->next = link->next;
	if (link->next)
		link->next->prev = link->prev;

	if (linker->head == link)
	{
		linker->head = link->next;
		if (!linker->head)
			linker->tail = NULL;
	}
	if (linker->tail == link)
		linker->tail = link->prev;

	link->prev = NULL;
	link->next = NULL;
	link->isLinked = FALSE;
	--linker->count;

if (!linker->count)
	if (linker->head != NULL || linker->tail != NULL)
		Tcl_Panic((char *) "linker \"%s\" count is zero, but head=%ld tail=%ld",
			linker->what, linker->head, linker->tail);
			
}

Tcl_Obj *ExtToUtf_NewStringObj(CONST char *bytes, int length)
{
	char *utfString;
	Tcl_DString utfDString;
	Tcl_Obj *objResult;

	utfString = Tcl_ExternalToUtfDString(NULL, bytes, length, &utfDString);
	objResult = Tcl_NewStringObj(utfString, Tcl_DStringLength(&utfDString));
	Tcl_DStringFree(&utfDString);
	return objResult;
}

void ExtToUtf_SetResult(Tcl_Interp *interp, char *string)
{
	char *utfString;
	Tcl_DString utfDString;

	utfString = Tcl_ExternalToUtfDString(NULL, string, -1, &utfDString);
	Tcl_SetResult(interp, utfString, TCL_VOLATILE);
	Tcl_DStringFree(&utfDString);
}

char *UtfToExt_TranslateFileName(Tcl_Interp *interp, char *utfPath, Tcl_DString *extDStringPtr)
{
	char *extString, *utfString;
	Tcl_DString utfDString;
	int utfLen;

	utfString = Tcl_TranslateFileName(interp, utfPath, &utfDString);
	if (utfString == NULL) return NULL;
	utfLen = Tcl_DStringLength(&utfDString);
	extString = Tcl_UtfToExternalDString(NULL, utfString, utfLen, extDStringPtr);
	Tcl_DStringFree(&utfDString);
	return extString;
}

static IndexedColor g_colormap;
unsigned char *g_colormap_rgb;

int Colormap_Init(Tcl_Interp *interp)
{
#ifdef PLATFORM_X11
	Tk_Window tkwin = Tk_MainWindow(interp);
	Display *display = Tk_Display(tkwin);
	Colormap colormap = Tk_Colormap(tkwin); /* DefaultColormap() */
	XColor xColor;
#endif
	int i, k, r, g, b;

	IndexedColor_ResetHash(&g_colormap);

#ifdef PLATFORM_X11
	if (Tk_Depth(tkwin) == 8)
	{
		/*
		 * Allocate each color in the colormap, to prevent the colormap
		 * entries from changing.
		 */
		for (i = 0; i < 256; i++)
		{
			xColor.pixel = i;
			XQueryColor(display, colormap, &xColor);
			(void) Tk_GetColorByValue(tkwin, &xColor);
		}
	}
#endif /* PLATFORM_X11 */

	for (i = 0; i < 256; i++)
	{
		r = g_palette.rgb[i * 3 + 0];
		g = g_palette.rgb[i * 3 + 1];
		b = g_palette.rgb[i * 3 + 2];

#ifdef PLATFORM_X11
		if (Tk_Depth(tkwin) == 8)
		{
			/* Get the XColor at this location in the colormap */
			xColor.pixel = i;
			XQueryColor(display, colormap, &xColor);
	
			/* Convert RGB values to 0-255 */
			r = ((double) xColor.red) / USHRT_MAX * 255;
			g = ((double) xColor.green) / USHRT_MAX * 255;
			b = ((double) xColor.blue) / USHRT_MAX * 255;
		}
#endif /* PLATFORM_X11 */

		/* Remember RGB values at this colormap index */
		g_colormap.rgb[i * 3 + 0] = r;
		g_colormap.rgb[i * 3 + 1] = g;
		g_colormap.rgb[i * 3 + 2] = b;

		/* Find the closest color in the palette */
		k = Palette_RGB2Index(r, g, b);

		/* Map colormap index -> palette index */
		g_colormap2palette[i] = k;
	}

	for (i = 0; i < 256; i++)
	{
		/* Get the RGB values at this palette index */
		r = g_palette.rgb[i * 3 + 0];
		g = g_palette.rgb[i * 3 + 1];
		b = g_palette.rgb[i * 3 + 2];		

		/* Find the closest color in the colormap */
		k = Colormap_RGB2Index(r, g, b);

		/* Map palette index -> colormap index */
		g_palette2colormap[i] = k;
	}

	/* Get the black and white pixels */
	g_colormap_black = PALETTE_BLACK;
	g_colormap_white = PALETTE_WHITE;

#ifdef PLATFORM_X11
	if (Tk_Depth(tkwin) == 8)
	{
		g_colormap_black = BlackPixelOfScreen(Tk_Screen(tkwin));
		g_colormap_white = WhitePixelOfScreen(Tk_Screen(tkwin));
	}
#endif /* PLATFORM_X11 */

	g_colormap_rgb = g_colormap.rgb;

	return TCL_OK;
}

unsigned char *Colormap_GetRGB(void)
{
	return g_colormap.rgb;
}

int Colormap_RGB2Index(unsigned char r, unsigned char g, unsigned char b)
{
	return IndexedColor_RGB2Index(&g_colormap, r, g, b);
}

void Colormap_GammaTable(double gamma, TintTable table)
{
	IndexedColor_GammaTable(&g_colormap, gamma, table);
}

void Colormap_TintTable(int tint, int opacity, TintTable table)
{
	IndexedColor_TintTable(&g_colormap, tint, opacity, table);
}

void Colormap_BrightnessTable(int intensity, TintTable table)
{
	IndexedColor_BrightnessTable(&g_colormap, intensity, table);
}

void Colormap_ContrastTable(int intensity, TintTable table)
{
	IndexedColor_ContrastTable(&g_colormap, intensity, table);
}

void Colormap_One2OneTable(TintTable table)
{
	IndexedColor_One2OneTable(table);
}

/*
 * Return a "standardized" string describing a font.
 */
int objcmd_fontdesc(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tk_Font tkfont;
	TkFont *fontPtr;
	char buf[1024];

	/* Hack - ignore unused parameter */
	(void) dummy;
	
    if (objc != 2)
    {
		Tcl_WrongNumArgs(interp, 1, objv, (char *) "font");
		return TCL_ERROR;
    }

	tkfont = Tk_AllocFontFromObj(interp, Tk_MainWindow(interp), objv[1]);
	if (tkfont == NULL)
	{
		return TCL_ERROR;
	}

	fontPtr = (TkFont *) tkfont;
	
	(void) sprintf(buf, "-family {%s} -size %d -weight %s -slant %s "
		"-underline %d -overstrike %d",
		fontPtr->fa.family, fontPtr->fa.size,
		(fontPtr->fa.weight == TK_FW_BOLD) ? "bold" : "normal",
		(fontPtr->fa.slant == TK_FS_ITALIC) ? "italic" : "roman",
		fontPtr->fa.underline,
		fontPtr->fa.overstrike);
		
	Tcl_SetStringObj(Tcl_GetObjResult(interp), buf, -1);

	Tk_FreeFontFromObj(Tk_MainWindow(interp), objv[1]);

	return TCL_OK;
}

/*
 * This is a colossal hack to counterattack the abysmal performance of
 * "$menu entryconfigure $index -state $state" under Tk 8.2. It allows
 * us to directly change the state of individual menu entries.
 *
 * Usage:
 *		menuentrystate $menu $index ?$state?
 */
int objcmd_menuentrystate(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	static cptr tkMenuStateStrings[] = {"active", "normal", "disabled",
		NULL};
	Tcl_HashEntry *hashEntryPtr;
	Tcl_HashTable *menuTablePtr;
	TkMenu *menuPtr;
	TkMenuEntry *mePtr;
	TkMenuReferences *menuRefPtr;
	char *pathName;
	int entryIndex, state;

	/* Hack - ignore unused parameter */
	(void) dummy;

	/* Required number of arguments */
    if ((objc != 3) && (objc != 4))
    {
		Tcl_WrongNumArgs(interp, 1, objv, (char *) "menu index ?state?");
		return TCL_ERROR;
    }

	/* Lookup the menu by name */
	pathName = Tcl_GetStringFromObj(objv[1], NULL);
	menuTablePtr = TkGetMenuHashTable(interp);
	hashEntryPtr = Tcl_FindHashEntry(menuTablePtr, pathName);
	if (hashEntryPtr == NULL)
	{
		return TCL_ERROR;
	}

	/* See how we get access to the TkMenu structure */
	menuRefPtr = (TkMenuReferences *) Tcl_GetHashValue(hashEntryPtr);
	menuPtr = menuRefPtr->menuPtr->masterMenuPtr;
	if (menuPtr == NULL)
	{
		return TCL_ERROR;
	}

	/* Get the entry index */
	if (TkGetMenuIndex(interp, menuPtr, objv[2], 0, &entryIndex) != TCL_OK)
	{
		return TCL_ERROR;	
	}

	/* Return the current value */
	if (objc == 3)
	{
		mePtr = menuPtr->entries[entryIndex];
		Tcl_SetStringObj(Tcl_GetObjResult(interp),
			(char *) tkMenuStateStrings[mePtr->state], -1);
		return TCL_OK;
	}

	/* Get the desired state */
	if (Tcl_GetIndexFromObj(interp, objv[3], (char **) tkMenuStateStrings, (char *) "state", 0, 
		&state) != TCL_OK)
	{
		return TCL_ERROR;
	}

	/* Configure the menu (in all the clone menus as well) */
	for (; menuPtr != NULL; menuPtr = menuPtr->nextInstancePtr)
	{
		mePtr = menuPtr->entries[entryIndex];
		mePtr->state = state;
		TkpConfigureMenuEntry(mePtr);
	}

	/* Success */
	return TCL_OK;
}

cptr keyword_term_color[16] = {
	"TERM_DARK",
	"TERM_WHITE",
	"TERM_SLATE",
	"TERM_ORANGE",
	"TERM_RED",
	"TERM_GREEN",
	"TERM_BLUE",
	"TERM_UMBER",
	"TERM_L_DARK",
	"TERM_L_WHITE",
	"TERM_VIOLET",
	"TERM_YELLOW",
	"TERM_L_RED",
	"TERM_L_GREEN",
	"TERM_L_BLUE",
	"TERM_L_UMBER"
};


byte g_prompt_attr = TERM_WHITE;

/*
 * Display a prompt in the "message line", but don't save it.
 */
void prompt_print(cptr str)
{
	char *attr = (char *) keyword_term_color[g_prompt_attr];

	angtk_eval("angband_prompt", "set", str, attr, NULL);
}

/*
 * Erase the "message line".
 */
void prompt_erase(void)
{
	angtk_eval("angband_prompt", "wipe", NULL);
}

/*
 * Display a formatted prompt, using "vstrnfmt()" and "prompt_print()".
 */
void prompt_format(cptr fmt, ...)
{
	va_list vp;
	
	char buf[1024];
	
	/* Begin the Varargs Stuff */
	va_start(vp, fmt);
	
	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, fmt, &vp);
	
	/* End the Varargs Stuff */
	va_end(vp);
	
	/* Display */
	prompt_print(buf);
}

void prompt_append(cptr str)
{
	char *attr = (char *) keyword_term_color[g_prompt_attr];

	angtk_eval("angband_prompt", "append", str, attr, NULL);
}

void prompt_open(cptr str)
{
	char *attr = (char *) keyword_term_color[g_prompt_attr];

	angtk_eval("angband_prompt", "open", str, attr, NULL);
}

void prompt_update(cptr str)
{
	char *attr = (char *) keyword_term_color[g_prompt_attr];

	angtk_eval("angband_prompt", "update", str, attr, NULL);
}

/*
 * Display a prompt, wait for a keypress.
 */
void any_more(cptr prompt)
{
	bool old_quick = quick_messages;

	/* Set the prompt */
	if (!prompt)
		prompt = "Hit any key to continue";

	/* Set quick_messages so any key is accepted */
	quick_messages = TRUE;

	/* Display the message, wait for a response */
	msgf(prompt);
	message_flush();

	/* Restore quick_messages */
	quick_messages = old_quick;
}

typedef struct PhotoMaster {
    Tk_ImageMaster tkMaster;	/* Tk's token for image master.  NULL means
				 * the image is being deleted. */
    Tcl_Interp *interp;		/* Interpreter associated with the
				 * application using this image. */
    Tcl_Command imageCmd;	/* Token for image command (used to delete
				 * it when the image goes away).  NULL means
				 * the image command has already been
				 * deleted. */
    int	flags;			/* Sundry flags, defined below. */
    int	width, height;		/* Dimensions of image. */
    int userWidth, userHeight;	/* User-declared image dimensions. */
    Tk_Uid palette;		/* User-specified default palette for
				 * instances of this image. */
    double gamma;		/* Display gamma value to correct for. */
    char *fileString;		/* Name of file to read into image. */
    Tcl_Obj *dataString;	/* Object to use as contents of image. */
    Tcl_Obj *format;		/* User-specified format of data in image
				 * file or string value. */
    unsigned char *pix24;	/* Local storage for 24-bit image. */
    int ditherX, ditherY;	/* Location of first incorrectly
				 * dithered pixel in image. */
    TkRegion validRegion;	/* Tk region indicating which parts of
				 * the image have valid image data. */
    struct PhotoInstance *instancePtr;
				/* First in the list of instances
				 * associated with this master. */
} PhotoMaster;


/*
 * photomask $image ?$imageMask?
 * This is the ugliest hack you (n)ever wanted to see. Tk takes a good
 * long time to load in a masked image, due to the amount of time it
 * takes to create the validRegion (ie, mask), through calls to the
 * TkUnionRectWithRegion() function. This command will quickly create
 * the validRegion by building the region by hand, using ExtCreateRegion().
 * All the white pixels (RGB=255,255,255) are left out of the mask.
 */
int
objcmd_photo_mask(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
#ifdef PLATFORM_WIN

	Tk_PhotoHandle photoH;
	Tk_PhotoImageBlock photoBlock, photoBlockDst, *photoBlockPtr;
	char *imageName;
	int y, x, end, width, height;
	unsigned char *rowPtr, *pixelPtr, *rowDstPtr, *pixelDstPtr;
	PhotoMaster *masterPtr;
	DWORD dataSize = 0;
	HRGN validRegion;
	int loop, rectCount = 0;
	RGNDATA *rgnData = NULL;
	RECT rcBound = {5000, 5000, 0, 0}, *rectPtr = NULL;

	/* Get the name of the Tk photo image. It must already exist */
	imageName = Tcl_GetStringFromObj(objv[1], NULL);

	/* Lookup the photo by name */
	photoH = Tk_FindPhoto(interp, imageName);

	/* The photo was not found */
	if (photoH == NULL)
	{
		/* Failure */
		return TCL_ERROR;
	}

	(void) Tk_PhotoGetImage(photoH, &photoBlock);
	masterPtr = (PhotoMaster *) photoH;
	photoBlockPtr = &photoBlock;

	if (objc == 3)
	{
		/* Get the name of the Tk photo image. It must already exist */
		imageName = Tcl_GetStringFromObj(objv[2], NULL);
	
		/* Lookup the photo by name */
		photoH = Tk_FindPhoto(interp, imageName);
	
		/* The photo was not found */
		if (photoH == NULL)
		{
			/* Failure */
			return TCL_ERROR;
		}
	
		(void) Tk_PhotoGetImage(photoH, &photoBlockDst);
		photoBlockPtr = &photoBlockDst;
	}

	height = photoBlockPtr->height;
	width = photoBlockPtr->width;

	/* Loop 0: count rectangles */
	/* Loop 1: create region */
	for (loop = 0; loop <= 1; loop++)
	{
		/* Adapted from tkImgGIF.c */
		rowPtr = photoBlockPtr->pixelPtr;
		rowDstPtr = photoBlock.pixelPtr;
		for (y = 0; y < height; y++)
		{
			x = 0;
			pixelPtr = rowPtr;
			pixelDstPtr = rowDstPtr;
			while (x < width)
			{
				/* Scan past transparent pixels */
				while ((x < width) && ((pixelPtr[0] + pixelPtr[1] + pixelPtr[2]) == 255 * 3))
				{
					x++;
					/* Write to the alpha channel */
					if (loop)
					{
						pixelDstPtr[0] = pixelDstPtr[1] = pixelDstPtr[2] = 0xd9;
						pixelDstPtr[3] = 0;
					}
					pixelPtr += photoBlockPtr->pixelSize;
					pixelDstPtr += 4;
				}
				end = x;
	
				/* Scan past non-transparent pixels */
				while ((end < width) && ((pixelPtr[0] + pixelPtr[1] + pixelPtr[2]) != 255 * 3))
				{
					end++;
					pixelPtr += photoBlockPtr->pixelSize;
					pixelDstPtr += 4;
				}
				if (end > x)
				{
					if (loop)
					{
						rectPtr->left = x;
						rectPtr->top = y;
						rectPtr->right = end;
						rectPtr->bottom = y + 1;
						rectPtr++;

						if (x < rcBound.left)
							rcBound.left = x;
						if (end > rcBound.right)
							rcBound.right = end;
						if (y < rcBound.top)
							rcBound.top = y;
						if (y + 1 > rcBound.bottom)
							rcBound.bottom = y + 1;
					}
					else
						rectCount++;
				}
				x = end;
			}
			rowPtr += photoBlockPtr->pitch;
			rowDstPtr += photoBlock.pitch;
		}

		if (loop == 1) break;

		/* 2413 rects for townactions.gif */
		/* 2413 * sizeof(RECT) = 38608 bytes */

		dataSize = sizeof(RGNDATA) + rectCount * sizeof(RECT);
		rgnData = (void *) HeapAlloc(GetProcessHeap(), 0, dataSize);
		rgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
		rgnData->rdh.iType = RDH_RECTANGLES;
		rgnData->rdh.nCount = rectCount;
		rgnData->rdh.nRgnSize = 0;
		rgnData->rdh.rcBound = rcBound;
		rectPtr = (RECT *) rgnData->Buffer;
	}

	if ((validRegion = ExtCreateRegion(NULL, dataSize, rgnData)) != NULL)
	{
		TkDestroyRegion(masterPtr->validRegion);
		masterPtr->validRegion = (TkRegion) validRegion;
	}

	HeapFree(GetProcessHeap(), 0, rgnData);

#endif /* PLATFORM_WIN */

#ifdef PLATFORM_X11

	Tk_PhotoHandle photoH;
	Tk_PhotoImageBlock photoBlock, photoBlockDst, *photoBlockPtr;
	char *imageName;
	int y, x, end, width, height;
	unsigned char *rowPtr, *pixelPtr, *rowDstPtr, *pixelDstPtr;
	PhotoMaster *masterPtr;
	XRectangle rect;
	TkRegion validRegion;

	/* Get the name of the Tk photo image. It must already exist */
	imageName = Tcl_GetStringFromObj(objv[1], NULL);

	/* Lookup the photo by name */
	photoH = Tk_FindPhoto(interp, imageName);

	/* The photo was not found */
	if (photoH == NULL)
	{
		/* Failure */
		return TCL_ERROR;
	}

	(void) Tk_PhotoGetImage(photoH, &photoBlock);
	masterPtr = (PhotoMaster *) photoH;
	photoBlockPtr = &photoBlock;

	if (objc == 3)
	{
		/* Get the name of the Tk photo image. It must already exist */
		imageName = Tcl_GetStringFromObj(objv[2], NULL);
	
		/* Lookup the photo by name */
		photoH = Tk_FindPhoto(interp, imageName);
	
		/* The photo was not found */
		if (photoH == NULL)
		{
			/* Failure */
			return TCL_ERROR;
		}
	
		(void) Tk_PhotoGetImage(photoH, &photoBlockDst);
		photoBlockPtr = &photoBlockDst;
	}

	validRegion = TkCreateRegion();

	height = photoBlockPtr->height;
	width = photoBlockPtr->width;

	/* Adapted from tkImgGIF.c */
	rowPtr = photoBlockPtr->pixelPtr;
	rowDstPtr = photoBlock.pixelPtr;
	for (y = 0; y < height; y++)
	{
		x = 0;
		pixelPtr = rowPtr;
		pixelDstPtr = rowDstPtr;
		while (x < width)
		{
			/* Scan past transparent pixels */
			while ((x < width) && ((pixelPtr[0] + pixelPtr[1] + pixelPtr[2]) == 255 * 3))
			{
				x++;
				/* Write to the alpha channel */
				pixelDstPtr[0] = pixelDstPtr[1] = pixelDstPtr[2] = 0xd9;
				pixelDstPtr[3] = 0;
				pixelPtr += photoBlockPtr->pixelSize;
				pixelDstPtr += 4;
			}
			end = x;

			/* Scan past non-transparent pixels */
			while ((end < width) && ((pixelPtr[0] + pixelPtr[1] + pixelPtr[2]) != 255 * 3))
			{
				end++;
				pixelPtr += photoBlockPtr->pixelSize;
				pixelDstPtr += 4;
			}
			if (end > x)
			{
				rect.x = x;
				rect.y = y;
				rect.width = end - x;
				rect.height = 1;
				TkUnionRectWithRegion(&rect, validRegion, validRegion);
			}
			x = end;
		}
		rowPtr += photoBlockPtr->pitch;
		rowDstPtr += photoBlock.pitch;
	}

	TkDestroyRegion(masterPtr->validRegion);
	masterPtr->validRegion = validRegion;
	
#endif /* PLATFORM_X11 */

	/* Hack - Ignore parameter */
	(void) dummy;

	return TCL_OK;
}


int ExtToUtf_SetArrayValueString(char *varName, char *field, char *value)
{
	Tcl_DString utfDString;
	char *utfString;

	Tcl_ExternalToUtfDString(NULL, value, -1, &utfDString);
	utfString = Tcl_DStringValue(&utfDString);
	if (Tcl_SetVar2(g_interp, varName, field, utfString, TCL_LEAVE_ERR_MSG)
		== NULL)
	{
		Tcl_DStringFree(&utfDString);
		return TCL_ERROR;
	}
	Tcl_DStringFree(&utfDString);	
	return TCL_OK;
}


