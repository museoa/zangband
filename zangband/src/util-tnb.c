/* File util-tnb.c */

/* Purpose: generic utilities */

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
	(void)vstrnfmt(buf, 1024, fmt, vp);
	
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
	msg_print(prompt);
	msg_print(NULL);

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
 * photoget $imageName $x $y
 * Like "$imageName get $x $y" but returns empty list on transparency.
 */
int
objcmd_photo_get(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tk_PhotoHandle photoH;
	char *imageName;
	int y, x;
	PhotoMaster *masterPtr;
	unsigned char *pixelPtr;

	/* Hack - ignore unused parameter */
	(void) dummy;

	/* Required number of arguments */
    if (objc != 4)
    {
		Tcl_WrongNumArgs(interp, 1, objv, (char *) "imageName x y");
		return TCL_ERROR;
    }

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

	masterPtr = (PhotoMaster *) photoH;

	if (Tcl_GetIntFromObj(interp, objv[2], &x) != TCL_OK ||
		Tcl_GetIntFromObj(interp, objv[3], &y) != TCL_OK)
		return TCL_ERROR;

	if ((x < 0) || (x >= masterPtr->width)
		|| (y < 0) || (y >= masterPtr->height))
	{
	    Tcl_AppendResult(interp, "coordinates out of range", NULL);
	    return TCL_ERROR;
	}

	if (TkRectInRegion(masterPtr->validRegion, x, y, 1, 1))
	{
		pixelPtr = masterPtr->pix24 + (y * masterPtr->width + x) * 3;
		Tcl_SetStringObj(Tcl_GetObjResult(interp), format("%d %d %d",
			pixelPtr[0], pixelPtr[1], pixelPtr[2]), -1);
	}
	
	return TCL_OK;
}

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

