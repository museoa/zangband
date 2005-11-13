/* File: fileio.c */

/*
 * Copyright (c) 1997 Ben Harrison, 2005 Steven Fuerst
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */


/*
 * Purpose: File io, with support for security via a child process
 *  if SECURE is set.
 *
 * We do this by marshalling all file i/o which requires group 'games'
 * to the child.  The child then does the work, and returns the result
 * to the main game process.  This means that the child process can be
 * audited relatively easily for security holes.  This allows the game
 * to run with gtk in multi-user setups in a secure fashion.
 */

#include "z-config.h"
#include "z-virt.h"
#include "z-util.h"
#include "z-file.h"
#include "z-form.h"  /* For format() */
#include "autoconf.h" /* For HAS_MKSTEMP */

/* Uncomment to add debugging */
/* #define TRACE_FILEIO */

#ifdef TRACE_FILEIO
#define DEBUG_CHILD(a) plog("CHILD: "), plog(a)
#define DEBUG_GAME(a) plog("GAME: "), plog(a)
#else /* TRACE_FILEIO */
#define DEBUG_CHILD(a)
#define DEBUG_GAME(a)
#endif /* TRACE_FILEIO */

/*
 * Most of the "file" routines for "ACORN" should be in "main-acn.c"
 *
 * Many of them can be rewritten now that only "fd_open()" and "fd_make()"
 * and "my_fopen()" should ever create files.
 *
 * Note that there are two versions of all the my_blah() functions here
 *   which ones are compiled in depends on whether SECURE is turned on.
 *   This should be cleaned up by using static dummy functions.
 */


/*
 * Code Warrior is a little weird about some functions
 */
#ifdef BEN_HACK
extern int open(const char *, int, ...);
extern int close(int);
extern int read(int, void *, unsigned int);
extern int write(int, const void *, unsigned int);
extern long lseek(int, long, int);
#endif /* BEN_HACK */


/*
 * The Macintosh is a little bit brain-dead sometimes
 */
#ifdef MACINTOSH
# define open(N,F,M) \
((M), open((char*)(N),F))
# define write(F,B,S) \
write(F,(char*)(B),S)
#endif /* MACINTOSH */


#ifdef SET_UID

/*
 * Extract a "parsed" path from an initial filename
 * Normally, we simply copy the filename into the buffer
 * But leading tilde symbols must be handled in a special way
 * Replace "~user/" by the home directory of the user named "user"
 * Replace "~/" by the home directory of the current user
 */
errr path_parse(char *buf, int max, cptr file)
{
	cptr u, s;
	struct passwd *pw;
	char user[128];

	/* Assume no result */
	buf[0] = '\0';

	/* No file? */
	if (!file) return (-1);

	/* File needs no parsing */
	if (file[0] != '~')
	{
		strncpy(buf, file, max - 1);

		/* Terminate */
		buf[max - 1] = '\0';
				
		return (0);
	}

	/* Point at the user */
	u = file + 1;

	/* Look for non-user portion of the file */
	s = strstr(u, PATH_SEP);

	/* Hack -- no long user names */
	if (s && (s >= u + sizeof(user))) return (1);

	/* Extract a user name */
	if (s)
	{
		int i;
		for (i = 0; u < s; ++i) user[i] = *u++;
		user[i] = '\0';
		u = user;
	}

	/* Look up the "current" user */
	if (u[0] == '\0') u = getlogin();

	/* Look up a user (or "current" user) */
	if (u) pw = getpwnam(u);
	else
		pw = getpwuid(getuid());

	/* Nothing found? */
	if (!pw) return (1);

	/* Make use of the info */
	(void)strncpy(buf, pw->pw_dir, max - 1);

	/* Append the rest of the filename, if any */
	if (s) (void)strncat(buf, s, max - 1);
	
	/* Terminate */
	buf[max - 1] = '\0';

	/* Success */
	return (0);
}

#else /* SET_UID */



/*
 * Extract a "parsed" path from an initial filename
 *
 * This requires no special processing on simple machines,
 * except for verifying the size of the filename.
 */
errr path_parse(char *buf, int max, cptr file)
{
	/* Accept the filename */
	(void)strnfmt(buf, max, "%s", file);

	/* Success */
	return (0);
}

#endif /* !SET_UID */

/* Flag stating whether we need secure access or not */
static bool group_games = FALSE;

/* Forward declare */
errr fd_write_raw(int fd, cptr buf, huge n, bool secure);

#if defined SECURE && defined SET_UID && defined HANDLE_SIGNALS

/* Used to terminate child process */
#include <signal.h>


void safe_setuid_drop(void)
{
	group_games = FALSE;
}

void safe_setuid_grab(void)
{
	group_games = TRUE;
}

/*
 * Operations to marshall over the pipes
 */
#define OP_OPEN		1
#define OP_CLOSE	2
#define OP_MOVE		3
#define OP_KILL		4
#define OP_MAKE		5
#define OP_SEEK		6
#define OP_LOCK		7
#define OP_READ		8
#define OP_WRITE	9

#define FD_MAX 10

/* The pipes themselves */
static int pipe_in[2];
static int pipe_out[2];
static int pipe_err[2];
static pid_t child = 0;


/*
 * Since we cannot pass the fd's over the pipe in any meaningful
 * way, we make fake file descriptors.  The child uses the below
 * arrays to convert from a fake file descriptor x, to a real file
 * descriptor fd_list[x].
 *
 * The game process uses the same arrays to convert from a fake file
 * descriptor y, to either a real file descriptor fd_list[y], if
 * fd_secure[y] is false, i.e. a non-secure file.  Or, if
 * fd_secure[y] is true, to the fake file descriptor of the child
 * fd_list[y] = x.  Note that since they are different processes,
 * the data is not shared - the two processes just 'happen' to use
 * arrays of the same name for this purpose.
 *
 * To increase speed (we don't really want to marshall one byte at
 * a time over the pipe), the file i/o is buffered.  This introduces
 * lots of lovely complexity.  We are basically reimplenting the
 * buffered file i/o part of libc here in a dodgy way. -SF-
 */

static int fd_list[FD_MAX];
static bool fd_secure[FD_MAX];

/* For buffered io */
static byte *fd_buf[FD_MAX];
static int fd_size[FD_MAX];
static int fd_offset[FD_MAX];

/*
 * Type of buffer (used in game process only)
 *  TRUE = writing to child
 *  FALSE = reading from child
 */
static bool fd_buf_type[FD_MAX];

/*
 * Save a file descriptor in the table
 */
static int child_create_fd(int fd)
{
	int i;
	
	for (i = 0; i < FD_MAX; i++)
	{
		/* Skip allocated entries */
		if (fd_list[i]) continue;
		
		fd_list[i] = fd;
		
		DEBUG_CHILD(format("fd %d, entry %d", fd, i));
		
		/* Success */
		return (i);
	}
	
	DEBUG_CHILD("fd entry allocation failed!");
	
	/* Failure */
	exit (0);
}


/*
 * Remove a file descriptor from the table
 */
static void child_remove_fd(int i)
{
	DEBUG_CHILD(format("Removing entry %d", i));

	/* Delete it */
	fd_list[i] = 0;
}

/*
 * Read size bytes from the file descriptor
 */
static errr my_read(int fd, char *data, int size)
{
	int n;
	int orig_size = size;
	
	while ((n = read(fd, data, size)) != size)
	{
		/* Interrupted? */
		if (n < 0)
		{
			/* Failure */
			return (n);
		}
		
		/* End of file! */
		if (!n)
		{
			/* Say number of bytes we got + 1 */
			return (orig_size - size + 1);
		}
		
		/* Move along, and try again */
		size -= n;
		data += n;
	}
	
	/* Success */
	return (0);
}

/*
 * Write size bytes to the file descriptor
 */ 
static errr my_write(int fd, cptr data, int size)
{
	int n;
	
	while ((n = write(fd, data, size)) != size)
	{
		/* Interrupted? */
		if (n == -1)
		{
			/* Failure */
			return (1);
		}
		
		/* Move along, and try again */
		size -= n;
		data += n;
	}
	
	/* Success */
	return (0);
}



static bool read_int(int *data)
{
	return (my_read(pipe_out[0], (char *) data, sizeof(int)));
}


static bool read_fd(int *fd)
{
	if (!read_int(fd))
	{
		/* Out of bounds? */
		if ((*fd < 0) || (*fd >= FD_MAX)) return (1);
		
		/* Make sure we have allocated it */
		if (fd_list[*fd] < 0) return (1);
		
		/* Success */
		return (0);
	}
	
	/* Failure */
	return (1);
}


static bool write_int(int data)
{
	return (my_write(pipe_in[1], (char *) &data, sizeof(int)));
}


/* Hack - so don't have a dependancy on angband.h */
extern bool validate_path(cptr buf);

static bool read_path(char *buf)
{
	int size;
	
	/* Get size of string */
	if (!read_int(&size) && (size <= 1023))
	{
		/* Read it */
		if (!my_read(pipe_out[0], buf, size))
		{
			DEBUG_CHILD(format("Validating path %s", buf));
			
			return (validate_path(buf));
		}
	}
	
	/* Failure */
	return (1);
}

/*
 * Attempt to send one page of data at a time over the pipes
 * Decrease this if io over the pipes doesn't work (deadlocks)?
 */
#define IO_BUF ((int)(4096 - sizeof(int) * 2))

/*
 * Child main loop
 */
static void child_loop(void)
{
	char buf[1024];
	char what[1024];
	int op;
	int fd;
	errr err;
	
	char iobuf[IO_BUF];
	
	while (TRUE)
	{
		if (read_int(&op))
		{
			/* Fatal error */
			exit(0);
		}
		
		/* Assume failure */
		err = -1;
	
		switch (op)
		{
			case OP_OPEN:
			{
				int flags;
				
				/* Get input */
				if (!read_int(&flags) && !read_path(buf))
				{
					/* Open the file - assuming O_BINARY */
					fd = open(buf, flags | O_BINARY, 0);
					
					if (fd >= 0)
					{
						/* Save it for later */
						fd = child_create_fd(fd);
					
						/* Send it to the game */
						if (!write_int(fd))
						{
							/* Success */
							err = 0;
							DEBUG_CHILD("Opened file");
						}
					}
					else
					{
						DEBUG_CHILD("Open failure");
					
						/* Failure */
						write_int(-1);
					}
				}
				
				break;
			}
			
			case OP_CLOSE:
			{
				/* Read file descriptor */
				if (!read_fd(&fd))
				{
					/* Close file */
					close(fd_list[fd]);
				
					/* Remove fd from table */
					child_remove_fd(fd);
					
					/* Success */
					err = 0;
					DEBUG_CHILD("Closed file");
				}
			
				break;
			}
			
			case OP_MOVE:
			{
				/* Read in two paths */
				if (!read_path(buf) && !read_path(what))
				{
					/* Rename */
					(void) rename(buf, what);
					
					DEBUG_CHILD(format("Moved %s to %s", buf, what));
					
					/* Success */
					err = 0;
				}

				break;
			}
			
			case OP_KILL:
			{
				/* Read in path */
				if (!read_path(buf))
				{
					(void) remove(buf);
					
					DEBUG_CHILD(format("Deleted %s", buf));
					
					/* Success */
					err = 0;
				}
			
				break;
			}
			
			case OP_MAKE:
			{
				int mode;
				
				/* Get mode + path */
				if (!read_int(&mode) && !read_path(buf))
				{
					/* Open the file - assuming O_BINARY */
					fd = open(buf, O_CREAT | O_EXCL | O_WRONLY | O_BINARY, mode);
					
					if (fd >= 0)
					{
						/* Save it for later */
						fd = child_create_fd(fd);
					
						/* Send it to the game */
						if (!write_int(fd))
						{
							DEBUG_CHILD(format("Created %s", buf));
						
							/* Success */
							err = 0;
						}
					}
				}
				break;
			}
			
			case OP_SEEK:
			{
				int n, p;

				/* Get fd + offset */
				if (!read_fd(&fd) && !read_int(&n))
				{
					p = lseek(fd_list[fd], n, SEEK_SET);
				
					if (p == n)
					{
						/* Success */
						err = 0;
						
						DEBUG_CHILD(format("Seek in fd %d to %d", fd, n)); 
					}
				}
			
				break;
			}
			
			case OP_LOCK:
			{
				int what;
				
				/* Get fd + lock state */
				if (!read_fd(&fd) && !read_int(&what))
				{
#ifdef USG

#if defined(F_ULOCK) && defined(F_LOCK)
					/* Un-Lock */
					if (what == F_UNLCK)
					{
						/* Unlock it, Ignore errors */
						lockf(fd_list[fd], F_ULOCK, 0);
						err = 0;
					}

					/* Lock */
					else
					{
						/* Lock the score file */
						if (!lockf(fd_list[fd], F_LOCK, 0)) err = 0;
					}
#endif

#else

#if defined(LOCK_UN) && defined(LOCK_EX)
					/* Un-Lock */
					if (what == F_UNLCK)
					{
						/* Unlock it, Ignore errors */
						(void)flock(fd_list[fd], LOCK_UN);
						err = 0;
					}

					/* Lock */
					else
					{
						/* Lock the score file */
						if (!flock(fd_list[fd], LOCK_EX)) err = 0;
					}
#endif

#endif
					if (!err)
					{
						DEBUG_CHILD(format("Locked %d", fd));
					}
				}
				
				break;
			}
			
			case OP_READ:
			{
				int size;
				
				/* Get fd + size */
				if (!read_fd(&fd) && !read_int(&size))
				{
					if ((size <= IO_BUF) && (size > 0))
					{
						err = my_read(fd_list[fd], iobuf, size);
						
						if (!err)
						{
							err = my_write(pipe_in[1], iobuf, size);
							if (err) err = -1;
						}
						else
						{
							if (err < 0)
							{
								DEBUG_CHILD("read fail 1");
							}
							else
							{
								/* Partial read */
								size = err - 1;
								err = my_write(pipe_in[1], iobuf, size);
								
								/* Say how much we have actually read */
								if (!err) err = size + 1;
							}
						}
					}
					else
					{
						DEBUG_CHILD("read fail 2");
					}
				}
				else
				{
					DEBUG_CHILD("read fail 3");
				}
			
				break;
			}
			
			case OP_WRITE:
			{
				int size;
				
				/* Assume success */
			
				/* Get fd + size */
				if (!read_fd(&fd) && !read_int(&size))
				{
					if ((size < IO_BUF) && (size > 0))
					{
						if (!my_read(pipe_out[0], iobuf, size) &&
							!my_write(fd_list[fd], iobuf, size))
						{
							/* Success */
							err = 0;
						}
						else
						{
							DEBUG_CHILD("write fail 1");
						}
					}
					else
					{
						DEBUG_CHILD("write fail 2");
					}
				}
				else
				{
					DEBUG_CHILD("write fail 3");
				}
				
				break;
			}
			
			default:
			{
				/* Something really bad happened, we are out of sync, so die */
				exit(0);
			}
		}
		
		/* Return error code */
		if (!my_write(pipe_err[1], (char *) &err, sizeof(int)))
		{
			if (err)
			{
				/* This is actually expected on a partial read... */
				DEBUG_CHILD("ERROR!");
			}
		}
		else
		{
			/* Ouch - can't talk to main program, so die */
			exit(0);
		}
	}
}


/* Parent functions */

/*
 * Save a file descriptor in the table
 */
static int create_fd(int fd)
{
	int i;
	
	/* Paranoia */
	if (fd < 0) return (fd);
	
	for (i = 0; i < FD_MAX; i++)
	{
		/* Skip allocated entries */
		if (fd_list[i]) continue;
		
		fd_list[i] = fd;
		
		/* Save security status */
		fd_secure[i] = group_games;
		
		/* Create buffer */
		C_MAKE(fd_buf[i], IO_BUF + 1, byte);
		
		fd_size[i] = 0;
		fd_offset[i] = 0;
		
		/* Default to reading */
		fd_buf_type[i] = FALSE;
		
		/* Success */
		return (i);
	}
	
	/* Failure */
	return (-1);
}

/*
 * Remove a file descriptor from the table
 */
static void remove_fd(int i)
{
	/* Clear write buffer */
	if (fd_size[i] && fd_buf_type[i])
	{
		fd_write_raw(fd_list[i], (cptr) fd_buf[i], fd_size[i], fd_secure[i]);
	}

	/* Delete it */
	fd_list[i] = 0;
	fd_secure[i] = FALSE;
	
	FREE(fd_buf[i]);
	fd_buf[i] = NULL;
	
	/* These lines aren't strictly needed */
	fd_buf_type[i] = FALSE;
	fd_size[i] = 0;
	fd_offset[i] = 0;
}

static bool secure_fd(int fd)
{
	return (fd_secure[fd]);
}

static int fd_lookup(int fd)
{
	return (fd_list[fd]);
}

static errr send_int(int data)
{
	return (my_write(pipe_out[1], (char *) &data, sizeof(int)));
}

static errr send_fd(int fd)
{
	return (send_int(fd_list[fd]));
}

/*
 * Send data to child
 */
static errr send_data(cptr buf, int size)
{
	if (!send_int(size)) return (my_write(pipe_out[1], buf, size));
	
	/* Failure */
	return(1);
}


static errr send_path(cptr buf)
{
	int size = strlen(buf);
	
	return (send_data(buf, size + 1));
}

/*
 * Receive data from child
 */
static errr receive_data(char *buf, int size)
{
	return (my_read(pipe_in[0], buf, size));
}


static errr receive_int(int *data)
{
	return (my_read(pipe_in[0], (char *) data, sizeof(int)));
}

static errr receive_err(void)
{
	errr err;
	
	if (my_read(pipe_err[0], (char *) &err, sizeof(err)))
	{
		/* Failure */
		return (1);
	}
	
	/* Return status */
	return (err);
}


static void secure_fd_kill(cptr buf)
{
	if (!send_int(OP_KILL) && !send_path(buf))
	{
		if (!receive_err())
		{
			/* Success */
			DEBUG_GAME("Killed file");
		}
	}
	else
	{
		/* Need to clear error status */
		receive_err();
	}
}

static void secure_fd_move(cptr buf, cptr aux)
{
	if (!send_int(OP_MOVE) && !send_path(buf) && !send_path(aux))
	{
		if (!receive_err())
		{
			/* Success */
			DEBUG_GAME("Moved file");
		}
	}
	else
	{
		/* Need to clear error status */
		receive_err();
	}
}

static int secure_fd_make(cptr buf, int mode)
{
	int fd;
	
	if (!send_int(OP_MAKE) && !send_int(mode), !send_path(buf))
	{
		if (!receive_err() && !receive_int(&fd))
		{
			DEBUG_GAME("Created file");
			
			/* Success - convert to fake fd in table */
			return (create_fd(fd));
		}
		else
		{
			/* Failure */
			return (-1);
		}
	}
	else
	{
		/* Need to clear error status */
		receive_err();
		
		/* Failure */
		return (-1);
	}
}

static int secure_fd_open(cptr buf, int flags)
{
	int fd;
	
	if (!send_int(OP_OPEN) && !send_int(flags) && !send_path(buf))
	{
		if (!receive_int(&fd) && (fd != -1))
		{
			/* Need to clear error status */
			receive_err();
		
			DEBUG_GAME("Opened file");
		
			/* Success */
			return (create_fd(fd));
		}
		else
		{
			/* Need to clear error status */
			receive_err();
		
			/* Failure */
			return (-1);
		}
	}
	else
	{
		/* Need to clear error status */
		receive_err();
		
		/* Failure */
		return (-1);
	}
}

static errr secure_fd_lock(int fd, int what)
{
	if (!send_int(OP_LOCK) && !send_fd(fd) && !send_int(what))
	{
		if (receive_err())
		{
			/* Failure */
			return (1);
		}
	}
	else
	{
		/* Need to clear error status */
		receive_err();
		
		/* Failure */
		return (1);
	}
	
	DEBUG_GAME("Locked file");

	/* Success */
	return (0);
}

static errr secure_fd_seek(int fd, int n)
{
	if (!send_int(OP_SEEK) && !send_fd(fd) && !send_int(n))
	{
		if (receive_err())
		{
			/* Failure */
			return (1);
		}
	}
	else
	{
		/* Need to clear error status */
		receive_err();
		
		/* Failure */
		return (1);
	}
	
	DEBUG_GAME("Seeked in file");

	/* Success */
	return (0);
}

/*
 * Read data from the child
 * This will read a maximum of IO_BUF size byte stream
 */
static errr child_read(int fd, char *buf, huge n)
{
	errr err;
	huge amount;
	
	/* Paranoia */
	if (n > IO_BUF) return (-1);
	
	/* Is this write-buffered? */
	if (fd_buf_type[fd])
	{
		/* Can't read from a write-buffered file. */
		return (-1);
	}
	
	amount = fd_size[fd] - fd_offset[fd];
	
	/* Do we have some already buffered data? */
	if (amount)
	{
		/* Copy at most n bytes */
		if (amount > n) amount = n;
		
		/* Copy them */
		C_COPY(buf, fd_buf[fd] + fd_offset[fd], amount, byte);
		
		fd_offset[fd] += amount;
		
		return (amount);
	}
	
	/* Buffer is empty - reset things */
	fd_offset[fd] = 0;
	fd_size[fd] = 0;
	
	/* Try to read IOBUF bytes */
	if (!send_int(OP_READ) && !send_fd(fd_lookup(fd)) && !send_int(IO_BUF))
	{
		/* Get error result */
		err = receive_err();
		
		/* Did the child get it from the file? */
		if (!err)
		{
			/* Read the data from child */
			if (!receive_data((char *) fd_buf[fd], IO_BUF))
			{
				/* Copy to output buffer */
				C_COPY(buf, fd_buf[fd], n, byte);
		
				fd_offset[fd] += n;
				fd_size[fd] = IO_BUF;
			
				/* Read n bytes */
				return (n);
			}
			else
			{
				DEBUG_GAME("Er1");
				
				/* Some error */
				return (-1);
			}
		}
		
		if (err < 0)
		{
			DEBUG_GAME("child_read: read error");
		}
		
		/* Read less than requested */
		if (err > 0)
		{
			DEBUG_GAME(format("Less than wanted: %d", err-1));

			/* Get amount actually read */
			err--;
			
			/* Read the data from child */
			if (!receive_data((char *) fd_buf[fd], err))
			{
				fd_size[fd] = err;
				
				/* Reading beyond EOF */
				if (n > (huge) err)
				{
					DEBUG_GAME("Beyond EOF?");
					return (-1);
				}
				
				/* Copy to output buffer */
				C_COPY(buf, fd_buf[fd], n, byte);
		
				fd_offset[fd] += n;
			
				/* Read n bytes */
				return (n);
			}
			else
			{
				DEBUG_GAME("Er2");
				
				/* Some error */
				return (-1);
			}
		}
		
		DEBUG_GAME("Er3");
		
		/* Failure */
		return (-1);
	}
	else
	{
		DEBUG_GAME("read error");
		
		/* Need to clear error status */
		receive_err();
			
		/* Failure */
		return (-1);
	}
}

static errr secure_fd_read(int fd, char *buf, huge n)
{
	int amount;

	/* Read pieces */
	while (n)
	{
		/* Read a piece, maximum size IO_BUF at a time */
		amount = child_read(fd, buf, MIN(n, IO_BUF));

		/* Read error? */
    	if (amount < 0) return (1);

    	/* Shorten the task */
    	buf += amount;

    	/* Shorten the task */
    	n -= amount;
		
		/* Done? */
		if (!n) return (0);
	}

	/* End of file */
	return (1);
}

/*
 * Write data to the child
 * This will send a maximum of IO_BUF size byte stream
 */
static errr child_write(int fd, cptr buf, huge n)
{
	if (n > IO_BUF) return (1);
	
	if (!send_int(OP_WRITE) && !send_fd(fd))
	{
		if (!send_data(buf, n) && !receive_err())
		{
			/* Success */
			return (0);
		}
	}

	/* Need to clear error status */
	receive_err();
			
	/* Failure */
	return (1);
}

static errr secure_fd_write(int fd, cptr buf, huge n)
{
	/* Write pieces */
	while (n >= IO_BUF)
	{
		/* Write a piece */
		if (child_write(fd, buf, IO_BUF)) return (1);			

		/* Shorten the task */
		buf += IO_BUF;

		/* Shorten the task */
		n -= IO_BUF;
	}
	
	/* Write the final piece */
	return (child_write(fd, buf, n));
}

static void secure_fd_close(int fd)
{
	if (!send_int(OP_CLOSE) && !send_fd(fd))
	{
		/* Success */
		DEBUG_GAME("Closed file");
	}
	
	/* Need to clear error status */
	receive_err();
}

static void slay_child(void)
{
	if (child)
	{
		/* Kill the child */
		kill(child, SIGTERM);
	
		child = 0;
		DEBUG_GAME("Killed Child!");
	}
}


void init_secure(void)
{
	int i;
	int fd;
	
	int user_gid;
	
	/*
	 * Make sure fds 0, 1 and 2 are fine 
	 * Otherwise game can be tricked into writing to stderr which
	 * could be redirected anywhere... which is bad.
	 */
	while (TRUE)
	{
    	fd = open("/dev/null", O_RDWR);
    	if (fd < 0) exit(0);
    	if (fd > 2)
		{
    	    close(fd);
    	    break;
    	}
	}
	
	/* XXX XXX XXX */
#if 0

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

#endif /* 0 */

	
	/* No game file descriptors yet*/
	for (i = 0; i < FD_MAX; i++)
	{
		fd_list[i] = 0;
	}
	
	/* Create the pipes used to send data to secure child */
	if (!pipe(pipe_in) && !pipe(pipe_out) && !pipe(pipe_err))
	{
		child = fork();
		
		if (child < 0)
		{
			quit("Fork failure!");
		}
		
		/* If we die - we need to clean up the child process */
		quit_init = slay_child;
		
		if (!child)
		{
			/* We are in the child */
		
			/* Close unused file descriptors */
			close(pipe_in[0]);
			close(pipe_out[1]);
			close(pipe_err[0]);
			
			/* Jump to childs main loop */
			child_loop();
		}
		else
		{
			/* We are in the parent */
		
			/* Close unused file descriptors */
			close(pipe_in[1]);
			close(pipe_out[0]);
			close(pipe_err[1]);
			
			/* Drop permissions totally */
			user_gid = getgid();
			if (setregid(user_gid, user_gid) != 0)
			{
				quit("setregid(): Failed to set permissions correctly!");
			}
		}
	}
	else
	{
		quit("Cannot create pipes!");
	}
}


#else /* ! SECURE */

static int create_fd(int fd)
{
	/* Just use incoming value */
	return (fd);
}

static void remove_fd(int i)
{
	(void) i;
}

static bool secure_fd(int fd)
{
	(void) fd;
	
	return (FALSE);
}

static int fd_lookup(int fd)
{
	return (fd);
}

static void secure_fd_kill(cptr buf)
{
	(void) buf;
}

static void secure_fd_move(cptr buf, cptr aux)
{
	(void) buf, aux;
}

static int secure_fd_make(cptr buf, int mode)
{
	(void) buf;
	(void) mode;
	
	return (-1);
}

static int secure_fd_open(cptr buf, int flags)
{
	(void) buf;
	(void) flags;
	
	return (-1);
}

static errr secure_fd_lock(int fd, int what)
{
	(void) fd;
	(void) what;
	
	return (1);
}

static errr secure_fd_seek(int fd, int n)
{
	(void) fd;
	(void) n;
	
	return (1);
}

static errr secure_fd_read(int fd, char *buf, huge n)
{
	(void) fd;
	(void) buf;
	(void) n;
	
	return (1);
}

static errr secure_fd_write(int fd, cptr buf, huge n)
{
	(void) fd;
	(void) buf;
	(void) n;
	
	return (1);
}

static void secure_fd_close(int fd)
{
	(void) fd;
}

void init_secure(void)
{

}


/*
 * Hack -- drop permissions
 */
void safe_setuid_drop(void)
{

#ifdef SAFE_SETUID

#ifdef HAVE_SETEGID

	if (setegid(getgid()) != 0)
	{
		quit("setegid(): cannot set permissions correctly!");
	}

#else  /* HAVE_SETEGID */

#ifdef SAFE_SETUID_POSIX

	if (setgid(getgid()) != 0)
	{
		quit("setgid(): cannot set permissions correctly!");
	}

#else  /* SAFE_SETUID_POSIX */

	if (setregid(getegid(), getgid()) != 0)
	{
		quit("setregid(): cannot set permissions correctly!");
	}

#endif /* SAFE_SETUID_POSIX */

#endif /* HAVE_SETEGID */

#endif /* SAFE_SETUID */

}


/*
 * Hack -- grab permissions
 */
void safe_setuid_grab(void)
{

#ifdef SAFE_SETUID

#ifdef HAVE_SETEGID

	if (setegid(player_egid) != 0)
	{
		quit("setegid(): cannot set permissions correctly!");
	}

#else  /* HAVE_SETEGID */

#ifdef SAFE_SETUID_POSIX

	if (setgid(player_egid) != 0)
	{
		quit("setgid(): cannot set permissions correctly!");
	}

#else  /* SAFE_SETUID_POSIX */

	if (setregid(getegid(), getgid()) != 0)
	{
		quit("setregid(): cannot set permissions correctly!");
	}

#endif /* SAFE_SETUID_POSIX */

#endif /* HAVE_SETEGID */

#endif /* SAFE_SETUID */
}

#endif /* SECURE */


/*
 * Hack -- attempt to delete a file
 */
errr fd_kill(cptr file)
{
	char buf[1024];

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (-1);
	
	if (group_games)
	{
		secure_fd_kill(buf);
	}
	else
	{
		/* Remove */
		(void)remove(buf);
	}
	
	/* Assume success XXX XXX XXX */
	return (0);
}


/*
 * Hack -- attempt to move a file
 */
errr fd_move(cptr file, cptr what)
{
	char buf[1024];
	char aux[1024];

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (-1);

	/* Hack -- Try to parse the path */
	if (path_parse(aux, 1024, what)) return (-1);
	
	if (group_games)
	{
		secure_fd_move(buf, aux);
	}
	else
	{
		/* Rename */
		(void)rename(buf, aux);
	}
	
	/* Assume success XXX XXX XXX */
	return (0);
}


/*
 * Hack -- attempt to open a file descriptor (create file)
 *
 * This function should fail if the file already exists
 *
 * Note that we assume that the file should be "binary"
 */
int fd_make(cptr file, int mode)
{
	char buf[1024];
	
	int fd;

	/* Hack -- Try to parse the path */
	if (path_parse(buf, sizeof(buf), file)) return (-1);

#if defined(MACINTOSH)

	/* Create the file, fail if exists, write-only, binary */
	fd = open(buf, O_CREAT | O_EXCL | O_WRONLY | O_BINARY);

#else

	if (group_games)
	{
		fd = secure_fd_make(buf, mode);
	}
	else
	{
		/* Create the file, fail if exists, write-only, binary */
		fd = open(buf, O_CREAT | O_EXCL | O_WRONLY | O_BINARY, mode);
		fd = create_fd(fd);
	}
	
#endif

#if defined(MAC_MPW) || defined(MACH_O_CARBON)

	/* Set file creator and type */
	if (fd >= 0) fsetfileinfo(buf, _fcreator, _ftype);
	
#endif
 
	return (fd);
}


/*
 * Hack -- attempt to open a file descriptor (existing file)
 *
 * Note that we assume that the file should be "binary"
 */
int fd_open(cptr file, int flags)
{
	char buf[1024];
	
	int fd;

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (-1);
	
	if (group_games)
	{
		fd = secure_fd_open(buf, flags);
	}
	else
	{
		/* Attempt to open the file */
		fd = open(buf, flags | O_BINARY, 0);
		fd = create_fd(fd);
	}
	
	return (fd);
}


/*
 * Hack -- attempt to lock a file descriptor
 *
 * Legal lock types -- F_UNLCK, F_RDLCK, F_WRLCK
 */
errr fd_lock(int fd, int what)
{
	/* XXX XXX */
	what = what ? what : 0;

	/* Verify the fd */
	if (fd < 0) return (-1);
	
	if (secure_fd(fd))
	{
		return (secure_fd_lock(fd_lookup(fd), what));
	}
	else
	{
		/* lookup fd */
		fd = fd_lookup(fd);

#ifdef SET_UID

# ifdef USG

#  if defined(F_ULOCK) && defined(F_LOCK)

		/* Un-Lock */
		if (what == F_UNLCK)
		{
			/* Unlock it, Ignore errors */
			lockf(fd, F_ULOCK, 0);
		}

		/* Lock */
		else
		{
			/* Lock the score file */
			if (lockf(fd, F_LOCK, 0) != 0) return (1);
		}

#  endif

# else

#  if defined(LOCK_UN) && defined(LOCK_EX)

		/* Un-Lock */
		if (what == F_UNLCK)
		{
			/* Unlock it, Ignore errors */
			(void)flock(fd, LOCK_UN);
		}

		/* Lock */
		else
		{
			/* Lock the score file */
			if (flock(fd, LOCK_EX) != 0) return (1);
		}

#  endif

# endif

#endif
	}

	/* Success */
	return (0);
}


/*
 * Hack -- attempt to seek on a file descriptor
 */
errr fd_seek(int fd, huge n)
{
	huge p;

	/* Verify fd */
	if (fd < 0) return (-1);
	
	if (secure_fd(fd))
	{
		return (secure_fd_seek(fd_lookup(fd), n));
	}
	else
	{
		/* lookup fd */
		fd = fd_lookup(fd);
	
		/* Seek to the given position */
		p = lseek(fd, n, SEEK_SET);

		/* Failure */
		if (p != n) return (1);
	}
	
	/* Success */
	return (0);
}


/*
 * Hack -- attempt to read data from a file descriptor
 */
errr fd_read(int fd, char *buf, huge n)
{
	/* Verify the fd */
	if (fd < 0) return (-1);

	if (secure_fd(fd))
	{
		return (secure_fd_read(fd, buf, n));
	}
	else
	{
		/* lookup fd */
		fd = fd_lookup(fd);
	
		/* Read pieces */
		while (n >= 16384)
		{
			/* Read a piece */
			if (read(fd, buf, 16384) != 16384) return (1);

			/* Shorten the task */
			buf += 16384;

			/* Shorten the task */
			n -= 16384;
		}

		/* Read the final piece */
		if (read(fd, buf, n) != (int)n) return (1);
	}
	
	/* Success */
	return (0);
}


/*
 * Hack -- Attempt to write data to a file descriptor
 */
errr fd_write_raw(int fd, cptr buf, huge n, bool secure)
{	
	if (secure)
	{
		return (secure_fd_write(fd, buf, n));
	}
	else
	{
		/* Write pieces */
		while (n >= 16384)
		{
			/* Write a piece */
			if (write(fd, buf, 16384) != 16384) return (1);

			/* Shorten the task */
			buf += 16384;

			/* Shorten the task */
			n -= 16384;
		}
		
		/* Write the final piece */
		if (write(fd, buf, n) != (int)n) return (1);
	}
	
	/* Success */
	return (0);
}


/*
 * Hack -- Attempt to write data to a file descriptor
 */
errr fd_write(int fd, cptr buf, huge n)
{
	int size;
	int i;
	bool secure;
	errr err;

	/* Verify the fd */
	if (fd < 0) return (-1);

/* Perhaps non-secure should buffer as well? XXX */
#ifdef SECURE	
	size = fd_size[fd];
	
	/* Is the buffer a read buffer? */
	if (!fd_buf_type[fd])
	{
		/* Paranoia - we can't write into a read-buffer! */
	 	if (fd_offset[fd]) return (1);
		
		/* The buffer is empty - change it to a write buffer */
		fd_buf_type[fd] = TRUE;
	}
	
	/* See if we should buffer it */
	if (n + size < IO_BUF)
	{
		/* Buffering */
		C_COPY(fd_buf[fd] + size, buf, n, byte);
		
		fd_size[fd] += n;
		
		/* Success */
		return (0);
	}
	
	/* Status of file */
	secure = secure_fd(fd);
	i = fd_lookup(fd);
	
	if (size)
	{
		/* Otherwise we need to write out the current buffer */
		err = fd_write_raw(i, (cptr) fd_buf[fd], size, secure);
		fd_size[fd] = 0;

		/* Paranoia */		
		if (err) return (err);
	}
	
	/*
	 * A better algorithm would be to buffer this as well,
	 * but I'm lazy, so if the input will overflow the buffer
	 * (even by one byte), we dump it directly to fd_write_raw(),
	 * rather than writing IO_BUF bytes and saving the rest.
	 */

#else /* SECURE */

	(void) size;
	(void) err;

	i = fd;
	secure = FALSE;

#endif /* SECURE */
	
	/* Actually do it */
	return (fd_write_raw(i, buf, n, secure));
}


/*
 * Hack -- attempt to close a file descriptor
 */
errr fd_close(int fd)
{
	/* Verify the fd */
	if (fd < 0) return (-1);

	/*
	 * Note - we need to check this before remove_fd()
	 * because that will clear the secure flag in the array.
	 */
	if (secure_fd(fd))
	{
		/* Remove fake fd from table */
		remove_fd(fd);
	
		/* Secure close */
		secure_fd_close(fd_lookup(fd));
	}
	else
	{
		/* Remove fake fd from table */
		remove_fd(fd);
	
		/* Close */
		(void)close(fd_lookup(fd));
	}

	/* XXX XXX XXX */
	return (0);
}


/*
 * The concept of the "file" routines below (and elsewhere) is that all
 * file handling should be done using as few routines as possible, since
 * every machine is slightly different, but these routines always have the
 * same semantics.
 *
 * In fact, perhaps we should use the "path_parse()" routine below to convert
 * from "canonical" filenames (optional leading tilde's, internal wildcards,
 * slash as the path seperator, etc) to "system" filenames (no special symbols,
 * system-specific path seperator, etc).  This would allow the program itself
 * to assume that all filenames are "Unix" filenames, and explicitly "extract"
 * such filenames if needed (by "path_parse()", or perhaps "path_canon()").
 *
 * Note that "path_temp" should probably return a "canonical" filename.
 *
 * Note that "my_fopen()" and "my_open()" and "my_make()" and "my_kill()"
 * and "my_move()" and "my_copy()" should all take "canonical" filenames.
 *
 * Note that "canonical" filenames use a leading "slash" to indicate an absolute
 * path, and a leading "tilde" to indicate a special directory, and default to a
 * relative path, but MSDOS uses a leading "drivename plus colon" to indicate the
 * use of a "special drive", and then the rest of the path is parsed "normally",
 * and MACINTOSH uses a leading colon to indicate a relative path, and an embedded
 * colon to indicate a "drive plus absolute path", and finally defaults to a file
 * in the current working directory, which may or may not be defined.
 *
 * We should probably parse a leading "~~/" as referring to "ANGBAND_DIR". (?)
 */


#ifdef ACORN


/*
 * Most of the "file" routines for "ACORN" should be in "main-acn.c"
 */


#else  /* ACORN */

#ifdef HAVE_MKSTEMP

FILE *my_fopen_temp(char *buf, int max)
{
	int fd;

	/* Prepare the buffer for mkstemp */
	strncpy(buf, "/tmp/anXXXXXX", max);

	/* Secure creation of a temporary file */
	fd = mkstemp(buf);

	/* Check the file-descriptor */
	if (fd < 0) return (NULL);

	/* Return a file stream */
	return (fdopen(fd, "w"));
}

#else  /* HAVE_MKSTEMP */

/*
 * Hack -- acquire a "temporary" file name if possible
 *
 * This filename is always in "system-specific" form.
 */
static errr path_temp(char *buf, int max)
{
	cptr s;

	/*
	 * Temp file
	 *
	 * If the following line gives you a compile-time warning,
	 * then turn on the HAVE_MKSTEMP if you have mkstemp().
	 */
	s = tmpnam(NULL);

	/* Oops */
	if (!s) return (-1);

	/* Format to length */
	(void)strnfmt(buf, max, "%s", s);

	/* Success */
	return (0);
}

FILE *my_fopen_temp(char *buf, int max)
{
	/* Generate a temporary filename */
	if (path_temp(buf, max)) return (NULL);

	/* Open the file */
	return (my_fopen(buf, "w"));
}

#endif /* HAVE_MKSTEMP */


/*
 * Create a new path by appending a file (or directory) to a path.
 *
 * This requires no special processing on simple machines, except
 * for verifying the size of the filename, but note the ability to
 * bypass the given "path" with certain special file-names.
 *
 * Note that the "file" may actually be a "sub-path", including
 * a path and a file.
 *
 * Note that this function yields a path which must be "parsed"
 * using the "parse" function above.
 */
void path_build(char *buf, int max, cptr path, cptr file)
{
	/* Special file */
	if (file[0] == '~')
	{
		/* Use the file itself */
		(void)strnfmt(buf, max, "%s", file);
	}

	/* Absolute file, on "normal" systems */
	else if (prefix(file, PATH_SEP) && !streq(PATH_SEP, ""))
	{
		/* Use the file itself */
		(void)strnfmt(buf, max, "%s", file);
	}

	/* No path given */
	else if (!path[0])
	{
		/* Use the file itself */
		(void)strnfmt(buf, max, "%s", file);
	}

	/* Path and File */
	else
	{
		/* Build the new path */
		(void)strnfmt(buf, max, "%s%s%s", path, PATH_SEP, file);
	}
}


/*
 * Hack -- replacement for "fopen()"
 */
FILE *my_fopen(cptr file, cptr mode)
{
	char buf[1024];
	FILE *fff;

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (NULL);
	
	/* Attempt to fopen the file anyway */
	fff = fopen(buf, mode);
  	 
#if defined(MAC_MPW) || defined(MACH_O_CARBON)
  	 
  	/* Set file creator and type */
  	if (fff && strchr(mode, 'w')) fsetfileinfo(buf, _fcreator, _ftype);
  	 
#endif
  	 
  	 /* Return open file or NULL */
  	 return (fff);
}


/*
 * Hack -- replacement for "fclose()"
 */
void my_fclose(FILE *fff)
{
	/* Require a file */
	if (!fff) return;

	/* Close, check for error */
	(void)fclose(fff);
}


#endif /* ACORN */


/*
 * Hack -- replacement for "fgets()"
 *
 * Read a string, without a newline, to a file
 *
 * Process tabs, strip internal non-printables if told.
 */
static errr my_fgets_aux(FILE *fff, char *buf, huge n, bool strip)
{
	huge i = 0;

	char *s;

	char tmp[1024];

	/* Read a line */
	if (fgets(tmp, 1024, fff))
	{
		/* Convert weirdness */
		for (s = tmp; *s; s++)
		{
			/* Handle newline */
			if (*s == '\n')
			{
				/* Terminate */
				buf[i] = '\0';

				/* Success */
				return (0);
			}

			/* Handle tabs */
			else if (*s == '\t')
			{
				/* Hack -- require room */
				if (i + 8 >= n) break;

				/* Append a space */
				buf[i++] = ' ';

				/* Append some more spaces */
				while (!(i % 8)) buf[i++] = ' ';
			}
			
			/* Strip non-printables if asked */
			else if(!strip || isprint(*s))
			{
				/* Copy */
				buf[i++] = *s;

				/* Check length */
				if (i >= n) break;
			}
		}
	}

	/* Nothing */
	buf[0] = '\0';

	/* Failure */
	return (1);
}


/*
 * Hack -- replacement for "fgets()"
 *
 * Read a string, without a newline, to a file
 *
 * Process tabs, strip internal non-printables
 */
errr my_fgets(FILE *fff, char *buf, huge n)
{
	return (my_fgets_aux(fff, buf, n, TRUE));
}

/*
 * Hack -- replacement for "fgets()"
 *
 * Read a string, without a newline, to a file
 *
 * Process tabs, do not strip internal non-printables
 */
errr my_raw_fgets(FILE *fff, char *buf, huge n)
{
	return (my_fgets_aux(fff, buf, n, FALSE));
}


/*
 * Wrapper around write call to put one byte at a time
 */
errr my_putc(char data, int fd)
{
	return (fd_write(fd, &data, 1));
}


/*
 * Wrapper around read to get one byte at a time
 */
errr my_getc(char *data, int fd)
{
	return (fd_read(fd, data, 1));
}

