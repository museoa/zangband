/* File: util-dll.h */

/* Purpose: misc stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef _INCLUDE_UTILDLL_H_
#define _INCLUDE_UTILDLL_H_

#include <tk.h>

#if !defined(PLATFORM_MAC) && !defined(PLATFORM_WIN) && !defined(PLATFORM_X11)
#error "you must define one of PLATFORM_MAC, PLATFORM_WIN or PLATFORM_X11"
#endif /* */

#if !defined(MIN)
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

int g_palette_white, g_palette_black;
#define PALETTE_WHITE g_palette_white
#define PALETTE_BLACK g_palette_black

int g_colormap_white, g_colormap_black;
#define COLORMAP_WHITE g_colormap_white
#define COLORMAP_BLACK g_colormap_black

unsigned char g_palette2colormap[256];
unsigned char g_colormap2palette[256];

typedef unsigned char TintValue, TintTable[256], *TintPtr;
int Palette_Init(Tcl_Interp *interp, char *fileName);
#ifdef PLATFORM_WIN
/* HPALETTE */ void *Palette_GetHPal(void);
#endif /* PLATFORM_WIN */
unsigned char *Palette_GetRGB(void);
void Palette_ResetHash(void);
int Palette_RGB2Index(unsigned char r, unsigned char g, unsigned char b);
void Palette_GammaTable(double gamma, TintTable table);
void Palette_TintTable(int tint, int opacity, TintTable table);

int Colormap_Init(Tcl_Interp *interp);
unsigned char *Colormap_GetRGB(void);
int Colormap_RGB2Index(unsigned char r, unsigned char g, unsigned char b);
void Colormap_GammaTable(double gamma, TintTable table);
void Colormap_TintTable(int tint, int opacity, TintTable table);
void Colormap_BrightnessTable(int intensity, TintTable table);
void Colormap_ContrastTable(int intensity, TintTable table);
void Colormap_One2OneTable(TintTable table);

typedef struct RGBInfo RGBInfo;
struct RGBInfo
{
	int red_count, green_count, blue_count;
	int red_mask, green_mask, blue_mask;
	int red_shift, green_shift, blue_shift;
	int extra;
};
RGBInfo g_rgbi;

void GetPix16(unsigned char *p, int *r, int *g, int *b);
void SetPix16(unsigned char *p, int r, int g, int b);

void *Array_New(int count, int elem_size);
void *Array_Append(void *array_ptr, int *count, int elem_size,
	void *elem_ptr);
void *Array_Insert(void *array_ptr, int *count, int elem_size,
	int index);
void *Array_Delete(void *array_ptr, int *count, int elem_size,
	int index);

#define TCL_FREE_HEADER \
	int _tcl_free[2]; /* [0]magic [1]size */

#define TCL_FREE_MAGIC 1234

#define MEMORY_DEBUG
#ifdef MEMORY_DEBUG
void db_dump_allocs(void);
int db_heap_error(void* vp,int head);

void *_db_malloc(size_t siz, char *file, int line);
void *_db_realloc(void *ptr, size_t siz, char *file, int line);
void _db_free(void *ptr, char *file, int line);
#define Tcl_AllocDebug(siz) _db_malloc(siz,__FILE__,__LINE__)
#define Tcl_ReallocDebug(str,siz) _db_realloc(str,siz,__FILE__,__LINE__)
#define Tcl_FreeDebug(str) _db_free(str,__FILE__,__LINE__)
#else /* not MEMORY_DEBUG */
char *Tcl_AllocDebug(int size);
void Tcl_ReallocDebug(char *ptr);
void Tcl_FreeDebug(char *ptr);
#endif /* not MEMORY_DEBUG */

typedef struct BitmapType {
	unsigned char *pixelPtr; /* Address of top-left pixel */
	int width; /* Width in pixels */
	int height; /* Height in pixels */
	int depth; /* 8, 16 or 24 */
	int pitch; /* Address difference between vertically adjacent pixels */
	int pixelSize; /* Address difference between horizontally adjacent pixels */
    Pixmap pixmap;
    void *platData; /* Platform-specific info */
} BitmapType, *BitmapPtr;

void Bitmap_New(Tcl_Interp *interp, BitmapPtr bitmapPtr);
void Bitmap_Delete(BitmapPtr bitmapPtr);

#ifdef PLATFORM_WINxxx
HFONT TkToWinFont(Tk_Font tkFont);
#endif /* PLATFORM_WIN */

typedef struct DoubleLink DoubleLink;
typedef struct DoubleLinker DoubleLinker;

struct DoubleLink
{
	int isLinked;
	DoubleLinker *linker;
	DoubleLink *prev;
	DoubleLink *next;
	void *data;
};

struct DoubleLinker
{
	int count;
	char *what;
	DoubleLink *head;
	DoubleLink *tail;
};

#define DoubleLink_Data(link,type) ((type *) link->data)
void DoubleLink_Init(DoubleLinker *linker, DoubleLink *link, void *data);
void DoubleLink_Link(DoubleLink *link);
void DoubleLink_Unlink(DoubleLink *link);
/* void DoubleLink_Move(DoubleLink *old, DoubleLink *new); */

Tcl_Obj *ExtToUtf_NewStringObj(CONST char *bytes, int length);
void ExtToUtf_SetResult(Tcl_Interp *interp, char *string);
char *UtfToExt_TranslateFileName(Tcl_Interp *interp, char *utfPath,
	Tcl_DString *extDStringPtr);

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif /* _INCLUDE_UTILDLL_H_ */

