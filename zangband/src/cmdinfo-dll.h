/* File: cmdinfo-dll.h */

/* Purpose: command stuff */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef INCLUDED_CMDINFO_H
#define INCLUDE_CMDINFO_H

typedef struct CommandInfo CommandInfo;
typedef struct CommandInit CommandInit;
typedef struct SubCommandInfo SubCommandInfo;

struct SubCommandInfo {
	int alloc;
	int count;
	char **name;
	CommandInfo **info;
};

struct CommandInfo {
	int depth;
	cptr name;
	int minArgs, maxArgs;
	cptr errorMsg;
	Tcl_ObjCmdProc *proc;
	ClientData clientData;
	SubCommandInfo subCmd;
};

struct CommandInit {
	int depth;
	cptr name;
	int minArgs, maxArgs;
	cptr errorMsg;
	Tcl_ObjCmdProc *proc;
	ClientData clientData;
};

void CommandInfo_Add(CommandInfo *infoCmd, CommandInfo *infoSubCmd);
CommandInfo *CommandInfo_GetInfo(Tcl_Interp *interp, char *names[]);
extern int CommandInfo_InitAux(Tcl_Interp *interp, CommandInit *init,
	int index, CommandInfo *parent);
int CommandInfo_Init(Tcl_Interp *interp, CommandInit *init,
	CommandInfo *parent);
CommandInfo *CommandInfo_New(CommandInit *init);
extern int CommandInfo_ObjCmd(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

#endif /* INCLUDE_CMDINFO_H */
