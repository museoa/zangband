/* File: plat.c */

/* Purpose: platform-specific stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifdef PLATFORM_WIN
#include <windows.h>
#include <tkWinInt.h>
#include "cmdinfo-dll.h"
#endif

#ifdef PLATFORM_X11
#include <tcl.h>
#endif

#ifdef PLATFORM_WIN

/*
 * This command puts up a font-selection dialog, but is NOT USED.
 */
int AngbandTk_CmdChooseFont(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tk_Window parent = Tk_MainWindow(interp);
	CHOOSEFONT cf;
	LOGFONT lf;
	BOOL result;
	int i, index, oldMode;
	char *t;
	
    /* Initialize members of the CHOOSEFONT structure. */ 
 
    cf.lStructSize = sizeof(CHOOSEFONT); 
    cf.hwndOwner = NULL; 
    cf.hDC = NULL; 
    cf.lpLogFont = &lf; 
    cf.iPointSize = 0; 
    cf.Flags = CF_SCREENFONTS; 
    cf.rgbColors = RGB(0,0,0); 
    cf.lCustData = 0L; 
    cf.lpfnHook = NULL; 
    cf.lpTemplateName = NULL; 

    cf.hInstance = NULL; 
    cf.lpszStyle = NULL; 
    cf.nFontType = SCREEN_FONTTYPE; 
    cf.nSizeMin = 0; 
    cf.nSizeMax = 0; 

	for (i = 1; i < objc; i++)
	{
		static char *option[] = {"-fixedpitch", "-parent", "-style", NULL};
		
	    if (Tcl_GetIndexFromObj(interp, objv[i], option, "option", 0, 
			&index) != TCL_OK)
		{
			return TCL_ERROR;
	    }

		switch (index)
		{
			case 0: /* -fixedpitch */
				cf.Flags |= CF_FIXEDPITCHONLY;
				break;

			case 1: /* -parent */
				t = Tcl_GetStringFromObj(objv[i+1], NULL);
				parent = Tk_NameToWindow(interp, t, Tk_MainWindow(interp));
				if (parent == NULL)
				{
					return TCL_ERROR;
				}
				i++;
				break;

			case 2: /* -style */
				lf.lfUnderline = 0;
				lf.lfStrikeOut = 0;
				cf.Flags |= CF_EFFECTS;
				break;
		}
	}

	if (Tk_WindowId(parent) == None)
	{
		Tk_MakeWindowExist(parent);
	}
	cf.hwndOwner = Tk_GetHWND(Tk_WindowId(parent));
	
    oldMode = Tcl_SetServiceMode(TCL_SERVICE_ALL);
	result = ChooseFont(&cf);
    (void) Tcl_SetServiceMode(oldMode);

    Tcl_ResetResult(interp);

 	if (result)
	{
		char buf[1024];
		(void) sprintf(buf, "-family {%s} -size %d -weight %s -slant %s "
			"-underline %d -overstrike %d",
			lf.lfFaceName, cf.iPointSize / 10,
			(lf.lfWeight > FW_MEDIUM) ? "bold" : "normal",
			(lf.lfItalic != 0) ? "italic" : "roman",
			(lf.lfUnderline != 0) ? 1 : 0,
			(lf.lfStrikeOut != 0) ? 1 : 0);
		Tcl_SetStringObj(Tcl_GetObjResult(interp), buf, -1);
	}

	return TCL_OK;
}

void *Plat_XWindowToHWND(Window xwin)
{
	return Tk_GetHWND(xwin);
}

/*
 * From tkWinColor.c. I want to know the system colors so I can set
 * a "disabled" toolbar button image to the actual RGB values. Canvas
 * items and widgets can use SytemButtonFace etc as valid color names,
 * but "$imageName put" does not accept them.
 */
typedef struct {
    char *name;
    int index;
} SystemColorEntry;

static SystemColorEntry sysColors[] = {
    {"ButtonFace",		COLOR_BTNFACE},
    {"ButtonHighlight",	COLOR_BTNHIGHLIGHT},
    {"ButtonShadow",	COLOR_BTNSHADOW},
    {"ButtonText",		COLOR_BTNTEXT},
    {NULL,				0}
};

#endif /* PLATFORM_WIN */

/*
 *--------------------------------------------------------------
 *
 * objcmd_system --
 *
 *--------------------------------------------------------------
 */
int
objcmd_system(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
#ifdef PLATFORM_X11

	return TCL_OK;
	
#endif /* PLATFORM_X11 */

#ifdef PLATFORM_WIN

	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	static char *cmdOptions[] = {"color", "mouse", "workarea", "osversion",
#if (TK_MINOR_VERSION == 3) && (TK_RELEASE_SERIAL < 3)
 		"windowicon",
#endif /* version < 8.3.3 */
 		NULL};
	enum {IDX_COLOR, IDX_MOUSE, IDX_WORKAREA, IDX_OSVERSION
#if (TK_MINOR_VERSION == 3) && (TK_RELEASE_SERIAL < 3)
		IDX_WINDOWICON
#endif /* version < 8.3.3 */
	} option;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);

	char *t, buf[128];
	int i;
	RECT rect;
	
	/* Required number of arguments */
    if (objC < 2)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, "option ?arg ...?");
		return TCL_ERROR;
    }

	/* Get requested option */
    if (Tcl_GetIndexFromObj(interp, objV[1], cmdOptions, "option", 0, 
		(int *) &option) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (option)
	{
		case IDX_COLOR: /* color */
			t = Tcl_GetStringFromObj(objV[2], NULL);
			for (i = 0; sysColors[i].name; i++)
			{
				if (!strcmp(t + 6, sysColors[i].name))
				{
					DWORD color = GetSysColor(sysColors[i].index);
				    int red = GetRValue(color);
				    int green = GetGValue(color);
				    int blue = GetBValue(color);
				    (void) sprintf(buf, "%d %d %d", red, green, blue);
					Tcl_SetStringObj(Tcl_GetObjResult(interp), buf, -1);
					return TCL_OK;
				}
			}
			Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
				"unknown color name \"%s\"", t, NULL);
			return TCL_ERROR;

		case IDX_MOUSE: /* mouse */
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				Tcl_AppendStringsToObj(resultPtr, "button1", NULL);
			}
			if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
			{
				Tcl_AppendStringsToObj(resultPtr, " button2", NULL);
			}
			if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
			{
				Tcl_AppendStringsToObj(resultPtr, " button3", NULL);
			}
			break;

		case IDX_WORKAREA:
			if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0))
			{
				return TCL_ERROR;
			}
			sprintf(buf, "%ld %ld %ld %ld",
				rect.left, rect.top, rect.right, rect.bottom);
			Tcl_SetStringObj(resultPtr, buf, -1);
			break;

#if (TK_MINOR_VERSION == 3) && (TK_RELEASE_SERIAL < 3)

		/*
		 * Set the window icon to the ANGBAND icon. This was taken from
		 * the "winico" Tcl extension Copyright(c) 1999 Brueckner&Jarosch.
		 */
		case IDX_WINDOWICON:
		{
			char *t = Tcl_GetString(objV[2]);
			HICON hIcon;
			char cmdBuf[1024];
			HWND hWnd;

			hIcon = LoadIcon(GetModuleHandle(NULL), "ANGBAND");
			if (hIcon == NULL)
			{
				Tcl_SetResult(interp, "can't load icon", TCL_STATIC);
				return TCL_ERROR;
			}
			(void) sprintf(cmdBuf, "wm frame %s", t);
			if (Tcl_Eval(interp, cmdBuf) != TCL_OK)
				return TCL_ERROR;
			(void) sscanf(Tcl_GetStringResult(interp), "0x%x",
				(unsigned int *) &hWnd);
			Tcl_ResetResult(interp);
			(void) SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);
			(void) SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) hIcon);
			break;
		}

#endif /* version < 8.3.3 */

		/* Same as tcl_platform(os), but detects 95/98/ME/NT/2000 */
		case IDX_OSVERSION:
		{
#ifdef __LCC__
			OSVERSIONINFO osInfo;
#else
			OSVERSIONINFOA osInfo;
#endif
			char *osStr = "???";

			osInfo.dwOSVersionInfoSize = sizeof(osInfo);
			(void) GetVersionExA(&osInfo);

			switch (osInfo.dwPlatformId)
			{
				case VER_PLATFORM_WIN32_NT:
					if (osInfo.dwMajorVersion >= 5)
						osStr = "Windows 2000";
					else
						osStr = "Windows NT";
					break;
				case VER_PLATFORM_WIN32_WINDOWS:
					if (osInfo.dwMajorVersion >= 5)
						osStr = "Windows ME";
					else if ((osInfo.dwMajorVersion == 4) &&
						(osInfo.dwMinorVersion > 0))
						osStr = "Windows 98";
					else
						osStr = "Windows 95";
					break;
				case VER_PLATFORM_WIN32s:
					osStr = "Win32s";
					break;
			}
			Tcl_SetStringObj(resultPtr, osStr, -1);
			break;
		}
	}

	return TCL_OK;

#endif /* PLATFORM_WIN */
}

/* Return the number of milliseconds */
unsigned long Milliseconds(void)
{
#ifdef PLATFORM_WIN
	return GetTickCount();
#endif
#ifdef PLATFORM_X11
	return TclpGetClicks() / 1000;
#endif
}

