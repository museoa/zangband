/* File: main-tnb.c */

/* Purpose: program entry point */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#ifdef USE_TNB

#include "tk/tnb.h"

cptr help_tnb[] =
{
	"To use tk/tcl port",
	NULL
};

#ifdef PLATFORM_WIN
#include <windows.h>
#endif

static term data;
bool g_initialized = FALSE;
bool game_in_progress = FALSE;
cptr ANGBAND_DIR_TK;
Tcl_Interp *g_interp;

#ifdef PLATFORM_X11

/*
 * Check for existance of a directory
 */
static bool check_dir(cptr s)
{
	struct stat statBuf;

	if (stat(s, &statBuf)) return (FALSE);
	if (!S_ISDIR(statBuf.st_mode)) return (FALSE);
	return (TRUE);
}

#endif /* PLATFORM_X11 */

/*
 * Validate a directory
 */
static void validate_dir(cptr s)
{
	/* Verify or fail */
	if (!check_dir(s))
	{
		quit_fmt("Cannot find required directory:\n%s", s);
	}
}


int
objcmd_term_attr(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	
	byte attr;
	
	int x, y;

	/* Required number of arguments */
    if (objC != 3)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "x y");
		return TCL_ERROR;
    }

	if (Tcl_GetIntFromObj(interp, objV[1], &x) != TCL_OK)
	{
		return TCL_ERROR;
	}
	
	if (Tcl_GetIntFromObj(interp, objV[2], &y) != TCL_OK)
	{
		return TCL_ERROR;
	}
	
	/* Paranoia */
	if ((x >= 80) || (x < 0)) return TCL_ERROR;
	if ((y >= 24) || (y < 0)) return TCL_ERROR;
	
	/* Get the term information */
	attr = data.scr->a[y][x];
	
	/* Store into the result */
	Tcl_SetIntObj(resultPtr, attr);

	return TCL_OK;
}


int
objcmd_term_char(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	
	char c;
	
	static char hack_string[2] = "1\0";
	
	int x, y;
	
	/* Required number of arguments */
    if (objC != 3)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "x y");
		return TCL_ERROR;
    }
	
	if (Tcl_GetIntFromObj(interp, objV[1], &x) != TCL_OK)
	{
		return TCL_ERROR;
	}
	
	if (Tcl_GetIntFromObj(interp, objV[2], &y) != TCL_OK)
	{
		return TCL_ERROR;
	}
	
	/* Paranoia */
	if ((x >= 80) || (x < 0)) return TCL_ERROR;
	if ((y >= 25) || (y < 0)) return TCL_ERROR;
	
	/* Get the term information */
	c = data.scr->c[y][x];
	
	/* Store into the result */
	hack_string[0] = c;
	Tcl_SetStringObj(resultPtr, hack_string, -1);

	return TCL_OK;
}

static errr Term_user_tnb(int n)
{
	/* Hack - ignore parameters for now */
	(void) n;

	return (0);
}

static errr Term_xtra_tnb_react(void)
{
	return 0;
}

/*
 * Process at least one event
 */
static errr Term_xtra_tnb_event(int v)
{
	int flags;

	/* Wait for an event */
	if (v) {

		/* Block */
		flags = TCL_ALL_EVENTS;

	/* Check for an event */
	} else {

		/* Check */
		flags = TCL_ALL_EVENTS | TCL_DONT_WAIT;
	}

	(void) Tcl_DoOneEvent(flags);

	/* Success */
	return 0;
}


/*
 * Process all pending events
 */
static errr Term_xtra_tnb_flush(void)
{
	int flags = TCL_ALL_EVENTS | TCL_DONT_WAIT;
	
	/* Hack - redraw everything */
	angtk_eval("NSTerm::Redraw", NULL);

	while (Tcl_DoOneEvent(flags)) ;

	/* Success */
	return (0);
}

static errr Term_xtra_tnb_clear(void)
{
	return 0;
}

/*
 * Hack -- make a noise
 */
static errr Term_xtra_tnb_noise(void)
{
#ifdef PLATFORM_WIN
	MessageBeep(MB_ICONASTERISK);
#endif /* PLATFORM_WIN */

	return (0);
}

/*
 * Hack -- make a sound
 */
static errr Term_xtra_tnb_sound(int v)
{
	/* Hack - ignore parameters for now */
	(void) v;

	return (0);
}

/*
 * Delay for "x" milliseconds
 */
static int Term_xtra_tnb_delay(int v)
{
	if (v <= 0)
		return (0);

#ifdef PLATFORM_WIN

	/* Sleep */
	Sleep(v);

#endif /* PLATFORM_WIN */

#ifdef PLATFORM_X11

	usleep(1000 * v);

#endif /* PLATFORM_X11 */

	/* Success */
	return (0);
}

/*
 * Do a "special thing"
 */
static errr Term_xtra_tnb(int n, int v)
{
	/* Handle a subset of the legal requests */
	switch (n)
	{
		/* Make a bell sound */
		case TERM_XTRA_NOISE:
		{
			return (Term_xtra_tnb_noise());
		}

		/* Make a special sound */
		case TERM_XTRA_SOUND:
		{
			return (Term_xtra_tnb_sound(v));
		}

		/* Process random events */
		case TERM_XTRA_BORED:
		{
			return (Term_xtra_tnb_event(0));
		}

		/* Process an event */
		case TERM_XTRA_EVENT:
		{
			return (Term_xtra_tnb_event(v));
		}

		/* Flush all events */
		case TERM_XTRA_FLUSH:
		{
			return (Term_xtra_tnb_flush());
		}

		/* Clear the screen */
		case TERM_XTRA_CLEAR:
		{
			return (Term_xtra_tnb_clear());
		}

		/* React to global changes */
		case TERM_XTRA_REACT:
		{
			return (Term_xtra_tnb_react());
		}

		/* Delay for some milliseconds */
		case TERM_XTRA_DELAY:
		{
			return (Term_xtra_tnb_delay(v));
		}
		
		/* Flush the output XXX XXX */
		case TERM_XTRA_FRESH:
		{
			int flags = TCL_WINDOW_EVENTS | TCL_IDLE_EVENTS | TCL_DONT_WAIT;

			while (Tcl_DoOneEvent(flags) != 0)
				;

			return (0);
		}
	}

	/* Oops */
	return 1;
}

static errr Term_curs_tnb(int x, int y)
{
	/* Hack - ignore parameters for now */
	(void) x;
	(void) y;

	return 0;
}

static errr Term_wipe_tnb(int x, int y, int n)
{
	/* Hack - ignore parameters for now */
	(void) x;
	(void) y;
	(void) n;

	return 0;
}

static errr Term_text_tnb(int x, int y, int n, byte a, const char *s)
{
	/* Hack - ignore parameters for now */
	(void) x;
	(void) y;
	(void) n;
	(void) a;
	(void) s;

	return 0;
}

static errr Term_pict_tnb(int x, int y, int n, const byte *ap, const char *cp, const byte *tap, const char *tcp)
{
	/* Hack - ignore parameters for now */
	(void) x;
	(void) y;
	(void) n;
	(void) ap;
	(void) cp;
	(void) tap;
	(void) tcp;

	return 0;
}


static void term_data_link(term *t)
{
	/* Initialize the term */
	term_init(t, 80, 24, 1024);

	/* Use a "software" cursor */
	t->soft_cursor = TRUE;

	/* Use "Term_pict" for "graphic" data */
	t->higher_pict = TRUE;

	/* Erase with "white space" */
	t->attr_blank = TERM_WHITE;
	t->char_blank = ' ';

	/* Prepare the template hooks */
	t->user_hook = Term_user_tnb;
	t->xtra_hook = Term_xtra_tnb;
	t->curs_hook = Term_curs_tnb;
	t->wipe_hook = Term_wipe_tnb;
	t->text_hook = Term_text_tnb;
	t->pict_hook = Term_pict_tnb;

	/* Remember where we came from */
	t->data = NULL;
}

static void init_windows(void)
{
	int i;

	term *t = &data;

	/* Main window */
	term_data_link(t);
	angband_term[0] = t;

	/* No extra Term's required */
	for (i = 1; i < 8; i++)
	{
		angband_term[i] = NULL;
	}

	Term_activate(t);
}


/*
 * Display error message and quit (see "z-util.c")
 */
static void hook_quit(cptr str)
{
	(void) str;

	free_icons();

	/* Cleanup Tcl and Tk (this exits via Tcl_Exit()) */
	/* TclTk_Exit(g_interp); */
	
	cleanup_angband();
}

#ifdef PLATFORM_X11

/*
 * Init the tk port
 */
int init_tnb(int argc, char **argv)
{
	ANGBAND_DIR_TK = DEFAULT_TK_PATH;

	/* Validate the "tk" directory */
	validate_dir(ANGBAND_DIR_TK);

	/* Use graphics */
	use_graphics = 1;

	/* Prepare the windows */
	init_windows();

	/* Activate hooks */
	quit_aux = hook_quit;
	core_aux = hook_quit;

	/* Set the system suffix */
	ANGBAND_SYS = "tk-x11";

	/* Initialize Tcl and Tk. */
	g_interp = TclTk_Init(argc, argv);
	
	/* Paranoia */
	if (!g_interp) return(1);

	/* Initialize */
	angtk_init();
	
	/* Initialize */
	init_angband();

	/* We are now initialized */
	g_initialized = TRUE;

	/* Program is intialized */
	angtk_angband_initialized();

#if 0
	while (TRUE)
	{
	while (Tcl_DoOneEvent(TRUE) != 0)
				;
	}
#endif /* 0 */
	
	/* Paranoia */
	return (0);
}

#endif /* PLATFORM_X11 */

#endif /* USE_TNB */
