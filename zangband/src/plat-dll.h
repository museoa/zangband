/* File: plat-dll.h */

/* Purpose: platform-specific stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef INCLUDED_PLATDLL_H
#define INCLUDED_PLATDLL_H

extern void Plat_BitmapNew(Tcl_Interp *interp, BitmapPtr bitmapPtr);
extern void Plat_BitmapDelete(BitmapPtr bitmapPtr);
extern void *Plat_PaletteInit(unsigned char *rgb);
int Plat_XColor2Pixel(XColor *xColorPtr);
extern void Plat_SyncDisplay(Display *display);

#endif /* INCLUDED_PLATDLL_H */
