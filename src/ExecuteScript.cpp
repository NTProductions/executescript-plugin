/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/


/*	
	Reveal Project.cpp

	Demonstrates usage of Keyframer API, specifically the 
	KeyFrameSuite. 
	

	version		notes							engineer		date
	
	1.0			First implementation			bbb				10/15/00
	2.0			Added AEGP_SuiteHandler usage	bbb				11/6/00
	2.5			Added ADM Palette handling		bbb				1/4/01
	3.0			Re-write; updated for 5.5		bbb				8/21/01
	3.1			Removed ADM (now exercised in	bbb				3/31/02
				Mangler sample), cleaned up
				functionality and names.
	3.1.1		Fixed a bug setting temporal	zal				10/15/14
				ease when calling
				AEGP_Get/SetKeyframeTemporalEase
	
*/

#include "ExecuteScript.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

static AEGP_Command			S_dummy_cmd =	0L;
static AEGP_Command			Execute_cmd =	0L;
static AEGP_PluginID		S_my_id =	0L;
static A_long				S_idle_count		=	0L;
static SPBasicSuite			*sP					=	NULL;

static	A_Err	IdleHook(
	AEGP_GlobalRefcon	plugin_refconP,	
	AEGP_IdleRefcon		refconP,		
	A_long				*max_sleepPL)
{
	A_Err					err			= A_Err_NONE;
	S_idle_count++;
	return err;
}

std::string globalChar;
std::string globalFullPath;
std::string globalName;

int globalOpenedCounter = 0;

static A_Err
UpdateMenuHook(
	AEGP_GlobalRefcon		plugin_refconPV,	/* >> */
	AEGP_UpdateMenuRefcon	refconPV,			/* >> */
	AEGP_WindowType			active_window)		/* >> */
{
	A_Err 				err 			=	A_Err_NONE,
						err2			=	A_Err_NONE;
		
	// variable setup
	AEGP_SuiteHandler	suites(sP);

	ERR2(suites.CommandSuite1()->AEGP_EnableCommand(Execute_cmd));
		
	return err;
}

static A_Err
CommandHook(
	AEGP_GlobalRefcon	plugin_refconPV,		/* >> */
	AEGP_CommandRefcon	refconPV,				/* >> */
	AEGP_Command		command,				/* >> */
	AEGP_HookPriority	hook_priority,			/* >> */
	A_Boolean			already_handledB,		/* >> */
	A_Boolean* handledPB)				/* << */
{
	A_Err			err = A_Err_NONE;


	AEGP_SuiteHandler	suites(sP);


	int counter = 0;
	if (Execute_cmd == command && *handledPB == FALSE) {

		//suites.UtilitySuite6()->AEGP_StartUndoGroup("Script Undo Group");

		std::string s = R"(app.project.items.addComp("Test Comp",1920,1080,1,10,30).openInViewer();)";


		suites.UtilitySuite6()->AEGP_ExecuteScript(S_my_id, s.c_str(), TRUE, NULL, NULL);
		*handledPB = TRUE;

		//suites.UtilitySuite6()->AEGP_EndUndoGroup();
	}

		return err;
}

A_Err
EntryPointFunc(
	struct SPBasicSuite		*pica_basicP,		/* >> */
	A_long				 	major_versionL,		/* >> */		
	A_long					minor_versionL,		/* >> */		
	AEGP_PluginID			aegp_plugin_id,		/* >> */
	AEGP_GlobalRefcon		*global_refconP)	/* << */
{
	S_my_id = aegp_plugin_id;
	A_Err 					err 				= A_Err_NONE, 
		err2 				= A_Err_NONE;
	
	sP = pica_basicP;
	
	AEGP_SuiteHandler suites(pica_basicP);
	
	
	err = suites.CommandSuite1()->AEGP_GetUniqueCommand(&Execute_cmd);
	
	if (!err && Execute_cmd) {
		ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(Execute_cmd, "Execute Script", AEGP_Menu_FILE, AEGP_MENU_INSERT_AT_BOTTOM));
	} 
	ERR(suites.RegisterSuite5()->AEGP_RegisterCommandHook(	S_my_id, 
															AEGP_HP_BeforeAE, 
															AEGP_Command_ALL,
															CommandHook, 
															0));
	
	ERR(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(S_my_id, UpdateMenuHook, 0));
	
	ERR(suites.RegisterSuite5()->AEGP_RegisterIdleHook(S_my_id, IdleHook, 0));
	
	if (err){ // not !err, err!
		ERR2(suites.UtilitySuite3()->AEGP_ReportInfo(S_my_id, "ExecuteScript : Could not register command hook."));
	}
	return err;
}
				   
