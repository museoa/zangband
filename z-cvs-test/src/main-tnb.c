/* File: main-tnb.c */

/* Purpose: program entry point */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include <tcl.h>
#include "angband.h"
#include "tnb.h"
#include "interp.h"
#include "tcltk.h"

#ifdef PLATFORM_WIN
#include <windows.h>
#define INVALID_FILE_NAME (DWORD)0xFFFFFFFF
#endif

static term data;
bool g_initialized = FALSE;
bool game_in_progress = FALSE;
cptr ANGBAND_DIR_ROOT;
cptr ANGBAND_DIR_TK;
cptr ANGBAND_DIR_COMMON;
Tcl_Interp *g_interp;
static char *gGameDir, *gVariantDir;

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

/* static */ errr Term_user_win(int n)
{
	return (0);
}

static errr Term_xtra_win_react(void)
{
	return 0;
}

/*
 * Process at least one event
 */
static errr Term_xtra_win_event(int v)
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
static errr Term_xtra_win_flush(void)
{
	int flags = TCL_ALL_EVENTS | TCL_DONT_WAIT;

	while (Tcl_DoOneEvent(flags)) ;

	/* Success */
	return (0);
}

static errr Term_xtra_win_clear(void)
{
	return 0;
}

/*
 * Hack -- make a noise
 */
static errr Term_xtra_win_noise(void)
{
#ifdef PLATFORM_WIN
	MessageBeep(MB_ICONASTERISK);
#endif /* PLATFORM_WIN */

	return (0);
}

/*
 * Hack -- make a sound
 */
static errr Term_xtra_win_sound(int v)
{
	return (0);
}

/*
 * Delay for "x" milliseconds
 */
static int Term_xtra_win_delay(int v)
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
/* static */ errr Term_xtra_win(int n, int v)
{
	/* Handle a subset of the legal requests */
	switch (n)
	{
		/* Make a bell sound */
		case TERM_XTRA_NOISE:
		{
			return (Term_xtra_win_noise());
		}

		/* Make a special sound */
		case TERM_XTRA_SOUND:
		{
			return (Term_xtra_win_sound(v));
		}

		/* Process random events */
		case TERM_XTRA_BORED:
		{
			return (Term_xtra_win_event(0));
		}

		/* Process an event */
		case TERM_XTRA_EVENT:
		{
			return (Term_xtra_win_event(v));
		}

		/* Flush all events */
		case TERM_XTRA_FLUSH:
		{
			return (Term_xtra_win_flush());
		}

		/* Clear the screen */
		case TERM_XTRA_CLEAR:
		{
			return (Term_xtra_win_clear());
		}

		/* React to global changes */
		case TERM_XTRA_REACT:
		{
			return (Term_xtra_win_react());
		}

		/* Delay for some milliseconds */
		case TERM_XTRA_DELAY:
		{
			return (Term_xtra_win_delay(v));
		}
	}

	/* Oops */
	return 1;
}

static errr Term_curs_win(int x, int y)
{
	return 0;
}

static errr Term_wipe_win(int x, int y, int n)
{
	return 0;
}

static errr Term_text_win(int x, int y, int n, byte a, const char *s)
{
	return 0;
}

static errr Term_pict_win(int x, int y, int n, const byte *ap, const char *cp)
{
	return 0;
}

/*
 * From z-term.c
 */
errr Term_fresh(void)
{	
	int flags = TCL_WINDOW_EVENTS | TCL_IDLE_EVENTS | TCL_DONT_WAIT;

	Bind_Generic(EVENT_TERM, KEYWORD_TERM_FRESH + 1);

	while (Tcl_DoOneEvent(flags) != 0)
		;

	return (0);
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
	t->user_hook = Term_user_win;
	t->xtra_hook = Term_xtra_win;
	t->curs_hook = Term_curs_win;
	t->wipe_hook = Term_wipe_win;
	t->text_hook = Term_text_win;
	t->pict_hook = Term_pict_win;

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
 * Check for double clicked (or dragged) savefile
 *
 * Apparently, Windows copies the entire filename into the first
 * piece of the "command line string".  Perhaps we should extract
 * the "basename" of that filename and append it to the "save" dir.
 */
static void check_for_save_file(int argc, char **argv)
{
#if 1
	int i;

	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-savefile"))
		{
			(void) strcpy(savefile, argv[i + 1]);
			break;
		}
	}
	if (i == argc)
		return;
	
#else
	char *s, *p;

	/* First arg */
	s = cmd_line;

	/* Second arg */
	p = strchr(s, ' ');

	/* Tokenize, advance */
	if (p) *p++ = '\0';

	/* No args */
	if (!*s) return;

	/* Extract filename */
	(void) strcpy(savefile, s);
#endif

	/* Okay if no such file */
	if (!check_file(savefile))
	{
/* FIXME: Try ANGBAND_DIR\save\savefile too */

		/* Forget the file */
		savefile[0] = '\0';

		/* Done */
		return;
	}

	/* Game in progress */
	game_in_progress = TRUE;

	/* Play game */
	play_game(FALSE);

	/* Bye! */
	quit(NULL);
}

/*
 * Display warning message (see "z-util.c")
 */
static void hack_plog(cptr str)
{
	/* Give a warning */
	if (str)
	{
#if 1
		angtk_eval("tk_messageBox", "-icon", "warning", "-message", str,
			"-title", "Warning", "-type", "ok", NULL);
#else
		MessageBox(NULL, str, "Warning",
			MB_ICONEXCLAMATION | MB_OK);
#endif
	}
}

/*
 * Display error message and quit (see "z-util.c")
 */
static void hack_quit(cptr str)
{
	/* Give a warning */
	if (str)
	{
#if 1
		angtk_eval("tk_messageBox", "-icon", "warning", "-message", str,
			"-title", "Warning", "-type", "ok", NULL);
#else
		MessageBox(NULL, str, "Error",
			MB_ICONEXCLAMATION | MB_OK | MB_ICONSTOP);
#endif
	}

	/* Cleanup Tcl and Tk (this exits via Tcl_Exit()) */
	TclTk_Exit(g_interp);

	/* Exit */
	exit(0);
}

/*
 * Display warning message (see "z-util.c")
 */
static void hook_plog(cptr str)
{
	/* Warning */
	if (str)
	{
#if 1
		angtk_eval("tk_messageBox", "-icon", "warning", "-message", str,
			"-title", "Warning", "-type", "ok", NULL);
#else
		int oldMode = Tcl_SetServiceMode(TCL_SERVICE_ALL);
		(void) MessageBox(NULL, str, "Warning",
			MB_ICONEXCLAMATION | MB_OK);
		(void) Tcl_SetServiceMode(oldMode);
#endif
	}
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
static void init_stuff(int argc, char **argv)
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
	
	(void) sprintf(path, "%s%s%s%s", ANGBAND_DIR_ROOT, PATH_SEP,"lib", PATH_SEP);

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

int main(int argc, char **argv)
{
	char path[1024], *t;

	/* Prepare the filepaths */
	init_stuff(argc, argv);

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
	t = Tcl_GetVar(g_interp, "tcl_patchLevel", TCL_GLOBAL_ONLY);
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

