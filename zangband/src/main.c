/* File: main.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

#include "angband.h"


/*
 * Some machines have a "main()" function in their "main-xxx.c" file,
 * all the others use this file for their "main()" function.
 */


#if !defined(MACINTOSH) && !defined(WINDOWS) && !defined(ACORN)

#ifdef USE_SCRIPT

#include "Python.h"

#endif /* USE_SCRIPT */


/*
 * A hook for "quit()".
 *
 * Close down, then fall back into "quit()".
 */
static void quit_hook(cptr s)
{
	int j;

	/* Ignore s */
	(void) s;
	
	/* Scan windows */
	for (j = 8 - 1; j >= 0; j--)
	{
		/* Unused */
		if (!angband_term[j]) continue;

		/* Nuke it */
		term_nuke(angband_term[j]);
	}
}



/*
 * Set the stack size (for the Amiga)
 */
#ifdef AMIGA
# include <dos.h>
__near long __stack = 32768L;
#endif


/*
 * Set the stack size and overlay buffer (see main-286.c")
 */
#ifdef USE_286
# include <dos.h>
extern unsigned _stklen = 32768U;
extern unsigned _ovrbuffer = 0x1500;
#endif

/*
 * Initialize and verify the file paths, and the score file.
 *
 * Use the ANGBAND_PATH environment var if possible, else use
 * DEFAULT_PATH, and in either case, branch off appropriately.
 *
 * First, we'll look for the ANGBAND_PATH environment variable,
 * and then look for the files in there.  If that doesn't work,
 * we'll try the DEFAULT_PATH constant.  So be sure that one of
 * these two things works...
 *
 * We must ensure that the path ends with "PATH_SEP" if needed,
 * since the "init_file_paths()" function will simply append the
 * relevant "sub-directory names" to the given path.
 *
 * Note that the "path" must be "Angband:" for the Amiga, and it
 * is ignored for "VM/ESA", so I just combined the two.
 *
 * Make sure that the path doesn't overflow the buffer.  We have
 * to leave enough space for the path separator, directory, and
 * filenames.
 */
static void init_stuff(void)
{
	char path[1024];

#if defined(AMIGA) || defined(VM)

	/* Hack -- prepare "path" */
	strcpy(path, "Angband:");

#else /* AMIGA / VM */

	cptr tail;

	/* Get the environment variable */
	tail = getenv("ANGBAND_PATH");

	/* Use the angband_path, or a default */
	strncpy(path, tail ? tail : DEFAULT_PATH, 511);

	/* Make sure it's terminated */
	path[511] = '\0';

	/* Hack -- Add a path separator (only if needed) */
	if (!suffix(path, PATH_SEP)) strcat(path, PATH_SEP);

#endif /* AMIGA / VM */

	/* Initialize */
	init_file_paths(path);
}



/*
 * Handle a "-d<what>=<path>" option
 *
 * The "<what>" can be any string starting with the same letter as the
 * name of a subdirectory of the "lib" folder (i.e. "i" or "info").
 *
 * The "<path>" can be any legal path for the given system, and should
 * not end in any special path separator (i.e. "/tmp" or "~/.ang-info").
 */
static void change_path(cptr info)
{
	cptr s;

	/* Find equal sign */
	s = strchr(info, '=');

	/* Verify equal sign */
	if (!s) quit_fmt("Try '-d<what>=<path>' not '-d%s'", info);

	/* Analyze */
	switch (tolower(info[0]))
	{
		case 'a':
		{
			string_free(ANGBAND_DIR_APEX);
			ANGBAND_DIR_APEX = string_make(s+1);
			break;
		}

		case 'f':
		{
			string_free(ANGBAND_DIR_FILE);
			ANGBAND_DIR_FILE = string_make(s+1);
			break;
		}

		case 'h':
		{
			string_free(ANGBAND_DIR_HELP);
			ANGBAND_DIR_HELP = string_make(s+1);
			break;
		}

		case 'i':
		{
			string_free(ANGBAND_DIR_INFO);
			ANGBAND_DIR_INFO = string_make(s+1);
			break;
		}

		case 'u':
		{
			string_free(ANGBAND_DIR_USER);
			ANGBAND_DIR_USER = string_make(s+1);
			break;
		}

		case 'x':
		{
			string_free(ANGBAND_DIR_XTRA);
			ANGBAND_DIR_XTRA = string_make(s+1);
			break;
		}

#ifdef VERIFY_SAVEFILE

		case 'b':
		case 'd':
		case 'e':
		case 's':
		{
			quit_fmt("Restricted option '-d%s'", info);
		}

#else /* VERIFY_SAVEFILE */

		case 'b':
		{
			string_free(ANGBAND_DIR_BONE);
			ANGBAND_DIR_BONE = string_make(s+1);
			break;
		}

		case 'd':
		{
			string_free(ANGBAND_DIR_DATA);
			ANGBAND_DIR_DATA = string_make(s+1);
			break;
		}

		case 'e':
		{
			string_free(ANGBAND_DIR_EDIT);
			ANGBAND_DIR_EDIT = string_make(s+1);
			break;
		}

		case 's':
		{
			string_free(ANGBAND_DIR_SAVE);
			ANGBAND_DIR_SAVE = string_make(s+1);
			break;
		}

		case 'z':
		{
			string_free(ANGBAND_DIR_SCRIPT);
			ANGBAND_DIR_SCRIPT = string_make(s+1);
			break;
		}

#endif /* VERIFY_SAVEFILE */

		default:
		{
			quit_fmt("Bad semantics in '-d%s'", info);
		}
	}
}

/*
 * The default message to print when we get bad input.
 */
static void game_usage(void)
{
	/* Dump usage information */
	puts("Usage: angband [options] [-- subopts]");
	puts("  -n       Start a new character");
	puts("  -f       Request fiddle mode");
	puts("  -w       Request wizard mode");
	puts("  -v       Request sound mode");
	puts("  -g       Request graphics mode");
	puts("  -o       Request original keyset");
	puts("  -r       Request rogue-like keyset");
	puts("  -M       Request monochrome mode");
	puts("  -s<num>  Show <num> high scores");
	puts("  -u<who>  Use your <who> savefile");
	puts("  -d<def>  Define a 'lib' dir sub-path");
	
#ifdef USE_XAW
	puts("  -mxaw    To use XAW");
	puts("  --       Sub options");
	puts("  -- -d    Set display name");
	puts("  -- -s    Turn off smoothscaling graphics");
	puts("  -- -n#   Number of terms to use");
#endif /* USE_XAW */
	
#ifdef USE_X11
	puts("  -mx11    To use X11");
	puts("  --       Sub options");
	puts("  -- -d    Set display name");
	puts("  -- -s    Turn off smoothscaling graphics");
	puts("  -- -n#   Number of terms to use");
#endif /* USE_X11 */
	
#ifdef USE_XPJ
	puts("  -mxpj    To use XPJ");
	puts("  --       Sub options");
	puts("  -- -d    Set display name");
	puts("  -- -s    Turn off smoothscaling graphics");
	puts("  -- -n#   Number of terms to use");
#endif /* USE_XPJ */
	
#ifdef USE_GCU
	puts("  -mgcu    To use GCU (GNU Curses)");
#endif /* USE_GCU */

#ifdef USE_CAP
	puts("  -mcap    To use CAP (\"Termcap\" calls)");
#endif /* USE_CAP */

#ifdef USE_DOS
	puts("  -mdos    To use DOS (Graphics)");
#endif /* USE_DOS */

#ifdef USE_IBM
	puts("  -mibm    To use IBM (BIOS text mode)");
#endif /* USE_IBM */

#ifdef USE_SLA
	puts("  -msla    To use SLA (SLANG)");
#endif /* USE_SLA */

#ifdef USE_LSL
	puts("  -mlsl    To use LSL (Linux-SVGALIB)");
#endif /* USE_LSL */

#ifdef USE_AMI
	puts("  -mami    To use AMI (Amiga)");
#endif /* USE_AMI */

#ifdef USE_VME
	puts("  -mvme    To use VME (VAX/ESA)");
#endif /* USE_VME */

#ifdef USE_GTK
	puts("  -mgtk    To use GTK toolkit");
	puts("  --       Sub options");
	/* puts("  -- -d    Set display name"); */
	/* puts("  -- -s    Turn off smoothscaling graphics"); */
	puts("  -- -n#   Number of terms to use");
#endif /* USE_VME */
				
	/* Actually abort the process */
	quit(NULL);
}


/*
 * Simple "main" function for multiple platforms.
 *
 * Note the special "--" option which terminates the processing of
 * standard options.  All non-standard options (if any) are passed
 * directly to the "init_xxx()" function.
 */
int main(int argc, char *argv[])
{
	int i;

	bool done = FALSE;

	bool new_game = FALSE;

	int show_score = 0;

	cptr mstr = NULL;

	bool args = TRUE;


	/* Save the "program name" XXX XXX XXX */
	argv0 = argv[0];

#ifdef USE_286
	/* Attempt to use XMS (or EMS) memory for swap space */
	if (_OvrInitExt(0L, 0L))
	{
		_OvrInitEms(0, 0, 64);
	}
#endif


#ifdef SET_UID

	/* Default permissions on files */
	(void)umask(022);

# ifdef SECURE
	/* Authenticate */
	Authenticate();
# endif

#endif


	/* Get the file paths */
	init_stuff();


#ifdef SET_UID

	/* Get the user id (?) */
	player_uid = getuid();

#ifdef VMS
	/* Mega-Hack -- Factor group id */
	player_uid += (getgid() * 1000);
#endif

# ifdef SAFE_SETUID

#  ifdef _POSIX_SAVED_IDS

	/* Save some info for later */
	player_euid = geteuid();
	player_egid = getegid();

#  endif

#  if 0	/* XXX XXX XXX */

	/* Redundant setting necessary in case root is running the game */
	/* If not root or game not setuid the following two calls do nothing */

	if (setgid(getegid()) != 0)
	{
		quit("setgid(): cannot set permissions correctly!");
	}

	if (setuid(geteuid()) != 0)
	{
		quit("setuid(): cannot set permissions correctly!");
	}

#  endif

# endif

#endif


#ifdef SET_UID

	/* Initialize the "time" checker */
	if (check_time_init() || check_time())
	{
		quit("The gates to Angband are closed (bad time).");
	}

	/* Initialize the "load" checker */
	if (check_load_init() || check_load())
	{
		quit("The gates to Angband are closed (bad load).");
	}

	/* Acquire the "user name" as a default player name */
#ifdef ANGBAND_2_8_1
	user_name(player_name, player_uid);
#else /* ANGBAND_2_8_1 */
	user_name(op_ptr->full_name, player_uid);
#endif /* ANGBAND_2_8_1 */

#endif /* SET_UID */


	/* Process the command line arguments */
	for (i = 1; args && (i < argc); i++)
	{
		/* Require proper options */
		if (argv[i][0] != '-') game_usage();

		/* Analyze option */
		switch (argv[i][1])
		{
			case 'N':
			case 'n':
			{
				new_game = TRUE;
				break;
			}

			case 'F':
			case 'f':
			{
				arg_fiddle = TRUE;
				break;
			}

			case 'W':
			case 'w':
			{
				arg_wizard = TRUE;
				break;
			}

			case 'V':
			case 'v':
			{
				arg_sound = TRUE;
				break;
			}

			case 'G':
			case 'g':
			{
				/* HACK - Graphics mode switches on the original tiles */
				arg_graphics = GRAPHICS_ORIGINAL;
				break;
			}

			case 'R':
			case 'r':
			{
				arg_force_roguelike = TRUE;
				break;
			}

			case 'O':
			case 'o':
			{
				arg_force_original = TRUE;
				break;
			}

			case 'S':
			case 's':
			{
				show_score = atoi(&argv[i][2]);
				if (show_score <= 0) show_score = 10;
				break;
			}

			case 'u':
			case 'U':
			{
				if (!argv[i][2]) game_usage();
#ifdef ANGBAND_2_8_1
				strncpy(player_name, &argv[i][2], 32);
				player_name[31] = '\0';
#else /* ANGBAND_2_8_1 */
				strncpy(op_ptr->full_name, &argv[i][2], 32);
				op_ptr->full_name[31] = '\0';
#endif /* ANGBAND_2_8_1 */
				break;
			}

			case 'm':
			{
				if (!argv[i][2]) game_usage();
				mstr = &argv[i][2];
				break;
			}

			case 'M':
			{
				arg_monochrome = TRUE;
				break;
			}

			case 'd':
			case 'D':
			{
				change_path(&argv[i][2]);
				break;
			}

			case '-':
			{
				argv[i] = argv[0];
				argc = argc - i;
				argv = argv + i;
				args = FALSE;
				break;
			}

			default:
			{
				 /* Default usage-help */
				 game_usage();
			}
		}
	}

	/* Hack -- Forget standard args */
	if (args)
	{
		argc = 1;
		argv[1] = NULL;
	}


	/* Process the player name */
	process_player_name(TRUE);



	/* Install "quit" hook */
	quit_aux = quit_hook;


	/* 
	 * Drop privs (so X11 will work correctly)
	 * unless we are running the Linux-SVGALib version.
	 *
	 * (In which case we initialize after safe_setuid_grab()
	 * is called.)
	 */
 
 	safe_setuid_drop();

#ifdef USE_XAW
	/* Attempt to use the "main-xaw.c" support */
	if (!done && (!mstr || (streq(mstr, "xaw"))))
	{
		if (0 == init_xaw(argc, argv))
		{
			ANGBAND_SYS = "xaw";
			done = TRUE;
		}
	}
#endif


#ifdef USE_X11
	/* Attempt to use the "main-x11.c" support */
	if (!done && (!mstr || (streq(mstr, "x11"))))
	{
		if (0 == init_x11(argc, argv))
		{
			ANGBAND_SYS = "x11";
			done = TRUE;
		}
	}
#endif


#ifdef USE_XPJ
	/* Attempt to use the "main-xpj.c" support */
	if (!done && (!mstr || (streq(mstr, "xpj"))))
	{
		if (0 == init_xpj(argc, argv))
		{
			ANGBAND_SYS = "xpj";
			done = TRUE;
		}
	}
#endif


#ifdef USE_GTK
	/* Attempt to use the "main-gtk.c" support */
	if (!done && (!mstr || (streq(mstr, "gtk"))))
	{
		if (0 == init_gtk(argc, argv))
		{
			/* Mega-hack */
			ANGBAND_SYS = "gtk";
			done = TRUE;
		}
	}
#endif


#ifdef USE_GCU
	/* Attempt to use the "main-gcu.c" support */
	if (!done && (!mstr || (streq(mstr, "gcu"))))
	{
		if (0 == init_gcu())
		{
			ANGBAND_SYS = "gcu";
			done = TRUE;
		}
	}
#endif

#ifdef USE_CAP
	/* Attempt to use the "main-cap.c" support */
	if (!done && (!mstr || (streq(mstr, "cap"))))
	{
		if (0 == init_cap(argc, argv))
		{
			ANGBAND_SYS = "cap";
			done = TRUE;
		}
	}
#endif


#ifdef USE_DOS
	/* Attempt to use the "main-dos.c" support */
	if (!done && (!mstr || (streq(mstr, "dos"))))
	{
		if (0 == init_dos())
		{
			ANGBAND_SYS = "dos";
			done = TRUE;
		}
	}
#endif

#ifdef USE_IBM
	/* Attempt to use the "main-ibm.c" support */
	if (!done && (!mstr || (streq(mstr, "ibm"))))
	{
		if (0 == init_ibm())
		{
			ANGBAND_SYS = "ibm";
			done = TRUE;
		}
	}
#endif


#ifdef USE_EMX
	/* Attempt to use the "main-emx.c" support */
	if (!done && (!mstr || (streq(mstr, "emx"))))
	{
		if (0 == init_emx())
		{
			ANGBAND_SYS = "emx";
			done = TRUE;
		}
	}
#endif


#ifdef USE_SLA
	/* Attempt to use the "main-sla.c" support */
	if (!done && (!mstr || (streq(mstr, "sla"))))
	{
		if (0 == init_sla())
		{
			ANGBAND_SYS = "sla";
			done = TRUE;
		}
	}
#endif


#ifdef USE_AMI
	/* Attempt to use the "main-ami.c" support */
	if (!done && (!mstr || (streq(mstr, "ami"))))
	{
		if (0 == init_ami())
		{
			ANGBAND_SYS = "ami";
			done = TRUE;
		}
	}
#endif


#ifdef USE_VME
	/* Attempt to use the "main-vme.c" support */
	if (!done && (!mstr || (streq(mstr, "vme"))))
	{
		if (0 == init_vme())
		{
			ANGBAND_SYS = "vme";
			done = TRUE;
		}
	}
#endif

	/* Grab privs (dropped above for X11) */
 	safe_setuid_grab();

#ifdef USE_LSL
	/* Attempt to use the "main-lsl.c" support */
	if (!done && (!mstr || (streq(mstr, "lsl"))))
	{
		if (0 == init_lsl())
		{
			ANGBAND_SYS = "lsl";
			done = TRUE;
		}
	}
#endif




	/* Make sure we have a display! */
	if (!done) quit("Unable to prepare any 'display module'!");


	/* Hack -- If requested, display scores and quit */
	if (show_score > 0) display_scores(0, show_score);


	/* Catch nasty signals */
	signals_init();

	/* Initialize */
	init_angband();

	/* Wait for response */
	pause_line(23);

	/* Play the game */
	play_game(new_game);

	/* Quit */
	quit(NULL);

	/* Exit */
	return (0);
}

#endif



