/* File: setting.c */

/* Purpose: the "setting" command */

/*
 * Copyright (c) 1997-2001 Tim Baker
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "tnb.h"
#include "cmdinfo-dll.h"
#include "icon.h" /* FIXME: only for allow_animation */

/*
 * These constants define the commands which can be passed to a
 * SettingProc() callback routine.
 */
#define SETTING_GET 0 /* Return the current value */
#define SETTING_DEFAULT 1 /* Return the default value */
#define SETTING_SET 2 /* Set the current value */

/*
 * Foward declarations.
 */
typedef struct SettingType SettingType;
typedef struct SettingParam SettingParam;
typedef struct SettingGroup SettingGroup;
typedef struct SettingGroup_ *SettingGroupToken;

/*
 * Each setting can have a unique callback routine.
 */
typedef int (*SettingProc)(SettingParam *param);

/*
 * One of these structures exists for every game setting.
 */
struct SettingType
{
	cptr name; /* Keyword */
	cptr desc; /* Human-readable description */
	SettingProc proc;
	void *data;
	int detail; /* Hack -- quasi-event detail. See Bind_Setting() */
};

/*
 * Contains arguments passed to a SettingProc().
 */
struct SettingParam
{
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj **objv;
	SettingType *setting;
	int cmd;
};

/*
 * A related group of settings.
 */
struct SettingGroup
{
	Tcl_Interp *interp;
	int count; /* Number of settings */
	SettingType *setting; /* Array of settings */
	Tcl_HashTable hash; /* Setting name -> setting[] index */
};


/* Setting callback for autosave_freq */
static int SettingProc_autosave_freq(SettingParam *param)
{
	Tcl_Interp *interp = param->interp;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	Tcl_Obj **objv = param->objv;

	int value;
	
	switch (param->cmd)
	{
		case SETTING_GET:
			Tcl_SetIntObj(resultPtr, autosave_freq);
			break;
		case SETTING_DEFAULT:
			break;
		case SETTING_SET:
			if (Tcl_GetIntFromObj(interp, objv[0], &value) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (value < 0 || value > 25000) value = 0;
			autosave_freq = value;
			Bind_Setting(param->setting->detail, value);
			break;
	}

	return TCL_OK;
}

/* Setting callback for misc boolean options */
static int SettingProc_bool(SettingParam *param)
{
	Tcl_Interp *interp = param->interp;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	Tcl_Obj **objv = param->objv;

	bool *var = (bool *) param->setting->data;
	int value;
	
	switch (param->cmd)
	{
		case SETTING_GET:
			Tcl_SetBooleanObj(resultPtr, (*var));
			break;
		case SETTING_DEFAULT:
			break;
		case SETTING_SET:
			if (Tcl_GetBooleanFromObj(interp, objv[0], &value) != TCL_OK)
			{
				return TCL_ERROR;
			}
			(*var) = value;

			/* XXX Hack -- don't generate <Setting> for Borg options */
			if (param->setting->detail != -1)
				Bind_Setting(param->setting->detail, value);
			break;
	}

	return TCL_OK;
}

/* Setting callback for allow_animation option */
static int SettingProc_allow_animation(SettingParam *param)
{
	int result = SettingProc_bool(param);

	if (param->cmd == SETTING_SET)
	{
		if (allow_animation)
			angtk_start_timer();
		else
			angtk_stop_timer();
	}

	return result;
}

/* Setting callback for delay_factor */
static int SettingProc_delay_factor(SettingParam *param)
{
	Tcl_Interp *interp = param->interp;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	Tcl_Obj **objv = param->objv;

	int value;
	
	switch (param->cmd)
	{
		case SETTING_GET:
			Tcl_SetIntObj(resultPtr, delay_factor);
			break;
		case SETTING_DEFAULT:
			break;
		case SETTING_SET:
			if (Tcl_GetIntFromObj(interp, objv[0], &value) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (value < 0 || value > 9) value = 9;
			delay_factor = value;
			Bind_Setting(param->setting->detail, value);
			break;
	}

	return TCL_OK;
}

/* Setting callback for hitpoint_warn */
static int SettingProc_hitpoint_warn(SettingParam *param)
{
	Tcl_Interp *interp = param->interp;
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	Tcl_Obj **objv = param->objv;

	int value;
	
	switch (param->cmd)
	{
		case SETTING_GET:
			Tcl_SetIntObj(resultPtr, hitpoint_warn);
			break;
		case SETTING_DEFAULT:
			break;
		case SETTING_SET:
			if (Tcl_GetIntFromObj(interp, objv[0], &value) != TCL_OK)
			{
				return TCL_ERROR;
			}
			if (value < 0 || value > 9) value = 9;
			hitpoint_warn = value;
			Bind_Setting(param->setting->detail, value);
			break;
	}

	return TCL_OK;
}

/*
 * Return the setting[] index for the setting with the given keyword.
 * Return an error if there is no such setting.
 */
static int Setting_FindByName(SettingGroupToken token, char *settingName,
	int *settingIndex)
{
	SettingGroup *group = (SettingGroup *) token;
	Tcl_HashEntry *hPtr;

	/* Look up the hash table entry using the tval as a key */
	if ((hPtr = Tcl_FindHashEntry(&group->hash, settingName)) == NULL)
	{
		char buf[80];
		
		/* Get the interpreter result object */
		Tcl_Obj *resultPtr = Tcl_GetObjResult(group->interp);

		/* Format the error */
		(void) sprintf(buf, "unknown setting \"%s\"", settingName);
		
		/* Append an error string */
		Tcl_SetStringObj(resultPtr, buf, -1);

		/* Failure */
		return TCL_ERROR;
	}

	/* Return the g_setting[] index for this setting */
	(*settingIndex) = (int) Tcl_GetHashValue(hPtr);

	/* Success */
	return TCL_OK;
}

static int Setting_FromObj(SettingGroupToken token, SettingType **settingPtrPtr,
	Tcl_Obj *nameObjPtr)
{
	SettingGroup *group = (SettingGroup *) token;
	char *t;
	int settingIndex;

	t = Tcl_GetString(nameObjPtr);
	if (Setting_FindByName(token, t, &settingIndex) != TCL_OK)
	{
		return TCL_ERROR;
	}
	(*settingPtrPtr) = &group->setting[settingIndex];

	return TCL_OK;
}

static int Setting_Add(SettingGroupToken token, SettingType *data)
{
	SettingGroup *group = (SettingGroup *) token;
	SettingType *settingPtr;
	int new;
	Tcl_HashEntry *hPtr;

	group->setting = Array_Insert(group->setting, &group->count,
		sizeof(SettingType), group->count);
	settingPtr = &group->setting[group->count - 1];

	settingPtr->name = data->name;
	settingPtr->desc = data->desc;
	settingPtr->proc = data->proc;
	settingPtr->data = data->data;

	/* XXX Hack */
	settingPtr->detail = group->count;
	
	/* Create a new hash table entry with key = setting keyword */
	hPtr = Tcl_CreateHashEntry(&group->hash, settingPtr->name, &new);

	/* Set the hash table entry with value = setting[] index */
	Tcl_SetHashValue(hPtr, group->count - 1);

	return TCL_OK;
}

static SettingGroupToken Setting_Init(Tcl_Interp *interp)
{
	SettingGroup *group;

	group = (SettingGroup *) Tcl_Alloc(sizeof(SettingGroup));
	group->interp = interp;
	group->count = 0;
	group->setting = Array_New(1, sizeof(SettingType));
	Tcl_InitHashTable(&group->hash, TCL_STRING_KEYS);

	return (SettingGroupToken) group;
}

int
objcmd_setting(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	CommandInfo *infoCmd = (CommandInfo *) clientData;
	int objC = objc - infoCmd->depth;
	Tcl_Obj *CONST *objV = objv + infoCmd->depth;

	SettingGroupToken token = (SettingGroupToken) infoCmd->clientData;
	static cptr cmdOptions[] = {"default", "desc", "set", NULL};
	int index;

	SettingParam param;
	SettingType *settingPtr;
	
    if (objC < 3)
    {
		Tcl_WrongNumArgs(interp, infoCmd->depth + 1, objv, (char *) "option setting ?arg ...?");
		return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objV[1], (char **) cmdOptions, (char *) "option", 0, 
		&index) != TCL_OK)
	{
		return TCL_ERROR;
    }

	switch (index)
	{
		case 0: /* default */
			break;

		case 1: /* desc */
			if (Setting_FromObj(token, &settingPtr, objV[2]) != TCL_OK)
			{
				return TCL_ERROR;
		    }
			ExtToUtf_SetResult(interp, (char *) settingPtr->desc);
			break;

		case 2: /* set */
			if (Setting_FromObj(token, &settingPtr, objV[2]) != TCL_OK)
			{
				return TCL_ERROR;
		    }
		    param.interp = interp;
		    param.objc = objC - 3;
		    param.objv = (Tcl_Obj **) objV + 3;
		    param.setting = settingPtr;
		    if (objC == 4)
			    param.cmd = SETTING_SET;
		    else
		    	param.cmd = SETTING_GET;
		    return (*settingPtr->proc)(&param);
	}

	/* Success */
	return TCL_OK;
}

SettingGroupToken g_setting; /* The global setting info. */

/*
 * Initialize all the setting/option-related stuff.
 */
void init_settings(void)
{
	SettingGroup *group;
	SettingType setting;
	int i;

	/* Allocate the game settings master */
	g_setting = Setting_Init(g_interp);

	setting.name = "delay_factor";
	setting.desc = "Base delay factor";
	setting.proc = SettingProc_delay_factor;
	setting.data = 0;
	Setting_Add(g_setting, &setting);

	setting.name = "hitpoint_warn";
	setting.desc = "Base hitpoint warning";
	setting.proc = SettingProc_hitpoint_warn;
	setting.data = 0;
	Setting_Add(g_setting, &setting);

	setting.name = "allow_animation";
	setting.desc = "Allow animation";
	setting.proc = SettingProc_allow_animation;
	setting.data = (void *) &allow_animation;
	Setting_Add(g_setting, &setting);

	setting.name = "autosave_l";
	setting.desc = "Autosave when entering new levels";
	setting.proc = SettingProc_bool;
	setting.data = (void *) &autosave_l;
	Setting_Add(g_setting, &setting);

	setting.name = "autosave_t";
	setting.desc = "Timed autosave";
	setting.proc = SettingProc_bool;
	setting.data = (void *) &autosave_t;
	Setting_Add(g_setting, &setting);

	setting.name = "autosave_freq";
	setting.desc = "Timed autosave frequency";
	setting.proc = SettingProc_autosave_freq;
	setting.data = 0;
	Setting_Add(g_setting, &setting);

	/*
	 * Create a list of setting keywords for use with bind.c stuff.
	 */
	group = (SettingGroup *) g_setting;
	C_MAKE(keyword_setting, group->count + 1, cptr);
	for (i = 0; i < group->count; i++)
	{
		keyword_setting[i] = group->setting[i].name;
	}
	keyword_setting[i] = NULL;
}

void Bind_Option(const char *name, int value)
{
	int index;

	if (Setting_FindByName(g_setting, (char *) name, &index) == TCL_OK)
	{
		SettingGroup *group = (SettingGroup *) g_setting;
		Bind_Setting(group->setting[index].detail, value);
	}
}


