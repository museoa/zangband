/* File: qebind.c */

/* Purpose: public interface to quasi-event package */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#ifndef INCLUDED_QEBIND_H
#define INCLUDED_QEBIND_H

typedef struct QE_Event {
	int type;
	int detail;
	ClientData clientData;
} QE_Event;

typedef void (*QE_ExpandProc)(char which, ClientData object, QE_Event *eventPtr,
	Tcl_DString *result);

typedef struct QE_BindingTable_ *QE_BindingTable;
QE_BindingTable bindingTable;
int debug_bindings;

int QE_BindInit(Tcl_Interp *interp);
extern QE_BindingTable QE_CreateBindingTable(Tcl_Interp *interp);
void QE_InstallEvent(char *name, int type, QE_ExpandProc expand);
void QE_InstallDetail(char *name, int eventType, int code);
extern void QE_FindDetail(int eventType, int code);
extern int QE_CreateBinding(QE_BindingTable bindingTable,
	ClientData object, char *eventString, char *command, int append);
extern int QE_DeleteBinding(QE_BindingTable bindingTable,
	ClientData object, char *eventString);
extern char *QE_GetBinding(QE_BindingTable bindingTable,
	ClientData object, char *eventString);
extern int QE_GetAllBindings(QE_BindingTable bindingTable,
	ClientData object);
int QE_BindEvent(QE_BindingTable bindingTable, QE_Event *eventPtr);
void QE_ExpandDouble(double number, Tcl_DString *result);
void QE_ExpandNumber(long number, Tcl_DString *result);
void QE_ExpandString(char *string, Tcl_DString *result);
void QE_ExpandUnknown(char which, Tcl_DString *result);
extern int QE_BindCmd(ClientData clientData, Tcl_Interp *interp, int argc,
	char **argv);
extern int QE_ConfigureCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));
extern int QE_GenerateCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));
extern int QE_InstallObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));

#endif /* INCLUDED_QEBIND_H */
