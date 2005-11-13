/* File z-form.h */

/*
 * Copyright (c) 2005 Steven Fuerst
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

#ifndef INCLUDED_Z_FILE_H
#define INCLUDED_Z_FILE_H

#include "h-basic.h"

/*
 * Several systems have no "O_BINARY" flag
 */
#ifndef O_BINARY
# define O_BINARY 0
#endif /* O_BINARY */

/*
 * This file provides file i/o routines.
 *
 * If SET_UID and SECURE are set then a child process will be launched,
 * connected to the main process by pipes.  The main process drops all
 * permissions, and can therefore access secure files only through the
 * child.  This limits the code that must be bug-free to a small part
 * of the game.
 */

/*
 * Helper macro so call path_build() with correct buffer size.
 */
#define path_make(B, P, F) \
	do \
	{ \
		assert(sizeof(B) > sizeof(void*)); \
		path_build((B), sizeof(B), (P), (F)); \
	} \
	while (FALSE)



extern errr path_parse(char *buf, int max, cptr file);

extern void safe_setuid_drop(void);
extern void safe_setuid_grab(void);
extern void init_secure(void);

extern errr fd_kill(cptr file);
extern errr fd_move(cptr file, cptr what);
extern int fd_make(cptr file, int mode);
extern int fd_open(cptr file, int flags);
extern errr fd_lock(int fd, int what);
extern errr fd_seek(int fd, huge n);
extern errr fd_read(int fd, char *buf, huge n);
extern errr fd_write(int fd, cptr buf, huge n);
extern errr fd_close(int fd);

extern FILE *my_fopen_temp(char *buf, int max);
extern void path_build(char *buf, int max, cptr path, cptr file);
extern FILE *my_fopen(cptr file, cptr mode);
extern void my_fclose(FILE *fff);
extern errr my_fgets(FILE *fff, char *buf, huge n);
extern errr my_raw_fgets(FILE *fff, char *buf, huge n);
extern errr my_putc(char data, int fd);
extern errr my_getc(char *data, int fd);

#endif
