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
cptr ANGBAND_DIR_XTRA_HELP;
cptr ANGBAND_DIR_ROOT;
cptr ANGBAND_DIR_TK;
cptr ANGBAND_DIR_COMMON;
cptr ANGBAND_DIR_COMMON_TK;
Tcl_Interp *g_interp;

#ifdef PLATFORM_X11

/*
 * Check for existance of a file
 */
static bool check_file(cptr s)
{
	struct stat statBuf;

	if (stat(s, &statBuf)) return (FALSE);
	if (S_ISDIR(statBuf.st_mode)) return (FALSE);
	return (TRUE);
}

/*
 * Check for existance of a directory
 * EXPORTED!!!
 */
bool check_dir(cptr s)
{
	struct stat statBuf;

	if (stat(s, &statBuf)) return (FALSE);
	if (!S_ISDIR(statBuf.st_mode)) return (FALSE);
	return (TRUE);
}

#endif /* PLATFORM_X11 */

/*
 * Validate a file
 */
void validate_file(cptr s, cptr fmt)
{
	/* Verify or fail */
	if (!check_file(s))
	{
		if (!fmt) fmt = "Could not find a required file:\n%s";
		quit_fmt(fmt, s);
	}
}

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

			Bind_Generic(EVENT_TERM, KEYWORD_TERM_FRESH + 1);

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


#ifdef PRIVATE_USER_PATH

/*
 * Create an ".angband/" directory in the users home directory.
 *
 * ToDo: Add error handling.
 * ToDo: Only create the directories when actually writing files.
 */
static void create_user_dir(void)
{
	char dirpath[1024];
	char subdirpath[1024];


	/* Get an absolute path from the filename */
	path_parse(dirpath, 1024, PRIVATE_USER_PATH);

	/* Create the ~/.angband/ directory */
	mkdir(dirpath, 0700);

	/* Build the path to the variant-specific sub-directory */
	path_build(subdirpath, 1024, dirpath, VERSION_NAME);

	/* Create the directory */
	mkdir(subdirpath, 0700);
}

#endif /* PRIVATE_USER_PATH */


#ifdef PLATFORM_X11

/*
 * Display error message and quit (see "z-util.c")
 */
static void hook_quit(cptr str)
{
	/* Give a warning */
	if (str)
	{
		fputs(str, stderr);
	}

	free_icons();

	/* Cleanup Tcl and Tk (this exits via Tcl_Exit()) */
	TclTk_Exit(g_interp);

	exit(0);
}

/* /home/tnb/AngbandTk/./angband --> /home/tnb/AngbandTk/angband */
/* /home/./tnb/foo/../bar --> /home/tnb/bar */
static char *clean_path(char *inp, char *outp)
{
	char buf[1024];
	char *elem[64], *elem2[64];
	int elemc, elem2c;
	int i;

	(void) strcpy(buf, inp);

	/* Split path into elements */
	elemc = 0;
	for (i = 0; buf[i]; i++)
	{
		if (buf[i] == '/')
		{
			elem[elemc++] = buf + i + 1;
			buf[i] = '\0';
		}
	}

	/* Handle . and .. */
	elem2c = 0;
	for (i = 0; i < elemc; i++)
	{
		if (!strcmp(elem[i], ".")) continue;
		if (!strcmp(elem[i], ".."))
		{
			elem2c--;
			continue;
		}
		elem2[elem2c++] = elem[i];
	}			

	/* Join path */
	outp[0] = '\0';
	for (i = 0; i < elem2c; i++)
	{
		strcat(outp, "/");
		strcat(outp, elem2[i]);
	}

	return outp;
}

/*
 * Init some stuff
 */
static void init_stuff(char **argv)
{
	char path[1024];

	char *p;

	/* On Linux, full pathname isn't given when starting from Bash */
	if (argv[0][0] != '/')
	{
		(void) getcwd(path, 1024);
		strcat(path, "/");

		/* Note: This may give us "/home/tnb/bin/../AngbandTk/angband" */
		strcat(path, argv[0]);
	}
	else
	{
		strcpy(path, argv[0]);
	}

	/* Eliminate . and .. from path */
	clean_path(path, path);

	/* Strip off application name */
	p = strrchr(path, '/');
	*p = '\0';

	/* Save the application directory */
	ANGBAND_DIR_ROOT = string_make(path);
	
	(void) sprintf(path, "%s%s%s", ANGBAND_DIR_ROOT, PATH_SEP, "tk");
	ANGBAND_DIR_COMMON_TK = string_make(path);

	/* Append "lib" directory to pathname */
	(void) strcpy(p, "/lib/");

	/* Validate the path */
	validate_dir(path);

	/* No spaces in path are allowed... */
	if (strchr(path, ' ') != NULL)
	{
		quit("Please install AngbandTk in a directory with no spaces in the name.");
	}

	/* Init the file paths */
	init_file_paths(path);

#ifdef PRIVATE_USER_PATH

	/* Create a directory for the users files. */
	create_user_dir();

#endif /* PRIVATE_USER_PATH */

	/* Hack -- Validate the paths */
	validate_dir(ANGBAND_DIR_APEX);
	validate_dir(ANGBAND_DIR_BONE);
	validate_dir(ANGBAND_DIR_DATA);
	validate_dir(ANGBAND_DIR_EDIT);
	validate_dir(ANGBAND_DIR_FILE);
	validate_dir(ANGBAND_DIR_HELP);
	validate_dir(ANGBAND_DIR_INFO);
	validate_dir(ANGBAND_DIR_SAVE);
	validate_dir(ANGBAND_DIR_USER);
	validate_dir(ANGBAND_DIR_XTRA);

	/* Build the filename */
	path_build(path, 1024, ANGBAND_DIR_FILE, "news.txt");

	/* Hack -- Validate the "news.txt" file */
	validate_file(path, NULL);

	/* Build the "help" path */
	path_build(path, 1024, ANGBAND_DIR_XTRA, "help");

	/* Allocate the path */
	ANGBAND_DIR_XTRA_HELP = string_make(path);

	/* Validate the "help" directory */
	/* validate_dir(ANGBAND_DIR_XTRA_HELP); */


	/* Build the "tk" path */
	path_build(path, 1024, ANGBAND_DIR_ROOT, "tk");

	/* Allocate the path */
	ANGBAND_DIR_TK = string_make(path);
	ANGBAND_DIR_COMMON = string_make(path);

	/* Validate the "tk" directory */
	validate_dir(ANGBAND_DIR_TK);

	/* Use graphics */
	use_graphics = 1;
}

int init_tnb(int argc, char **argv)
{
	char *t;

	/* Prepare the filepaths */
	init_stuff(argv);

#ifdef SET_UID

	/* Get the user id (?) */
	player_uid = getuid();

#ifdef SAFE_SETUID
#ifdef _POSIX_SAVED_IDS

	/* Save some info for later */
	player_euid = geteuid();
	player_egid = getegid();
	
#endif /* _POSIX_SAVED_IDS */
#endif /* SAFE_SETUID */
#endif /* SET_UID */

	/* Prepare the windows */
	init_windows();

	/* Activate hooks */
	quit_aux = hook_quit;
	core_aux = hook_quit;

	/* Set the system suffix */
	ANGBAND_SYS = "tk-x11";

	/* Initialize Tcl and Tk. */
	g_interp = TclTk_Init(argc, argv);

	/* Sanity: Require same Tcl version as common.dll */
	t = Tcl_GetVar(g_interp, (char *) "tcl_patchLevel", TCL_GLOBAL_ONLY);
	if (!t || strcmp(t, TCL_PATCH_LEVEL))
	{
		quit_fmt("The game was compiled with Tcl version %s, "
			"but common.dll was compiled with Tcl version %s"
			TCL_PATCH_LEVEL, t ? t : "UNKNOWN");
	}

	/* Initialize */
	angtk_init();

	/* Initialize */
	init_angband();

	/* We are now initialized */
	g_initialized = TRUE;

	/* Program is intialized */
	angtk_angband_initialized();

	/* XXX Did the user pass a savefile name via argv[]? */

	/* Loop forever (never returns) */
	while (1) Tcl_DoOneEvent(0);

	/* Paranoia */
	quit(NULL);

	/* Paranoia */
	return (0);
}

#endif /* PLATFORM_X11 */

#endif /* USE_TNB */
