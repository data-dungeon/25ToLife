/*
**
**  File:   DevLobby.cpp
**  Date:   March 31, 2004
**  By:     Bryant Collard
**  Desc:   A simple lobby used during development.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/DevLobby.cpp $
**  $Revision: #27 $
**  $DateTime: 2005/08/24 17:01:26 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header
#include "sim/DevLobby.h"

// Non-precompiled headers.
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "main/PlayerDB.h"
#include "main/TTLLoop.h"
#include "sim/InitialLoop.h"
#include "Network/SessionSpec.h"
#include "environ/ConfigMgr.h"
#include "GameHelper/LogUtil.h"

#define SEARCH_TIMEOUT_MS 500

// Initialize function array.
SLoopEntry CDevLobby::s_stateArray[CDevLobby::NUM_STATES] =
{
	"INITIALIZE", (LoopFunc)&CDevLobby::Initialize,
	"RUN",         NULL,
	"LAUNCH",     (LoopFunc)&CDevLobby::Launch,
	"TERMINATE",  (LoopFunc)&CDevLobby::Terminate
};

/***************************************************************************/
/***************************************************************************/
CDevLobby::CDevLobby() :
	GameLoop(s_stateArray, UNINITIALIZED),
	d_messageOwner("DevLobby")
{
	d_nextLoop = NO_NEXT;
	d_launch = false;
	d_active = false;
	d_character = -1;
	d_level = -1;
	d_sessionName[0] = '\0';
	d_wanHostIP[0] = '\0';
	d_modeName[0] = '\0';
}

/***************************************************************************/
/***************************************************************************/
int CDevLobby::DetermineState(int i_state)
{
	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
		if (d_requestExit)
			return(UNINITIALIZED);
		d_nextLoop = NO_NEXT;
		d_launch = false;
		return(INITIALIZE);
	 case INITIALIZE:
		if (d_modeName[0] == '\0')
			strcpy(d_modeName, "dev");
		return(RUN);
	 case RUN:
		if (d_requestExit)
			return(TERMINATE);
		if (d_launch)
			return(LAUNCH);
		if (d_nextLoop != NO_NEXT)
			return(TERMINATE);
		return(RUN);
	 case LAUNCH:
		if (d_requestExit)
			return(TERMINATE);
		RenderMgr__FadeOut(FADE_TO_BLACK_FAST);
		return(WaitForTask(&RenderMgr__IsFadedOut, NULL, TERMINATE));
	 case TERMINATE:
		break;
	 default:
		if (d_requestExit)
			return(TERMINATE);
		return(i_state);
	}
	return(UNINITIALIZED);
}

/***************************************************************************/
/***************************************************************************/
void CDevLobby::Initialize(void)
{
	// If this point is reached, d_level is -1 if no sessions have been run
	// or the level used in the last session. Set the environment so the menu
	// displays that level.
	if (d_level >= 0)
		g_gameEnvironment.Set(d_level);

	CLobby::Stop();

	d_launchHandler.Initialize("SetupLaunch", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_backupHandler.Initialize("SetupBackup", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_inviteHandler.Initialize("AcceptInvitation", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);

	// Make sure there is no fade.
	if (RenderMgr__IsFadedOut() || RenderMgr__IsFadingOut())
		RenderMgr__FadeIn(FADE_TO_BLACK_INSTANT);

	g_FrontEnd.ClearScreenHistory();
	g_FrontEnd.GotoScreen("SCREEN_ID_DEVMENU");

	d_active = false;
}

/***************************************************************************/
/***************************************************************************/
void CDevLobby::Launch(void)
{
	if (d_lobby.SessionSearch().SpecifyHostedSession(d_wanHostIP, g_sessionSpec,
			SEARCH_TIMEOUT_MS))
		d_nextLoop = MULTIPLAYER_NEXT;
	else if (d_sessionName[0] != '\0')
	{
		if (!d_lobby.SessionSearch().SpecifyNamedSession(d_sessionName,
				g_sessionSpec, SEARCH_TIMEOUT_MS))
			g_sessionSpec.SetName(d_sessionName);
		d_nextLoop = MULTIPLAYER_NEXT;
		CLobby::Start(d_lobby);
	}
	else
		d_nextLoop = SINGLEPLAYER_NEXT;

	if (d_modeName[0] != '\0')
	{
		g_configmgr.Initialize();
		g_configmgr.SetOptionByName("gameModes", d_modeName);
		g_configmgr.AcceptChanges(false);
	}

	d_active = true;
}

/***************************************************************************/
/***************************************************************************/
void CDevLobby::Terminate(void)
{
	d_messageOwner.UnregisterAll();
}

/***************************************************************************/
/***************************************************************************/
void CDevLobby::Launch::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->d_launch = true;
}

/***************************************************************************/
/***************************************************************************/
void CDevLobby::Backup::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->d_nextLoop = INITIAL_NEXT;
}

/***************************************************************************/
/***************************************************************************/
void CDevLobby::Invite::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->d_nextLoop = INITIAL_NEXT;
}

/***************************************************************************/
/***************************************************************************/
bool CDevLobby::ParseOverrides(void)
{
	// Make sure things are clear.
	d_character = -1;
	d_level = -1;
	d_sessionName[0] = '\0';
	d_wanHostIP[0] = '\0';
	d_modeName[0] = '\0';

	// Process the 'quicklaunch.var' file.
	char *context = "quicklaunch";
	char accountName[MAX_DEV_NAME_LEN] = "\0";
	char levelName[MAX_DEV_NAME_LEN] = "\0";
	char mpLevelName[MAX_DEV_NAME_LEN] = "\0";
	char spLevelName[MAX_DEV_NAME_LEN] = "\0";
	char dbLevelName[MAX_DEV_NAME_LEN] = "\0";
	char characterName[MAX_DEV_NAME_LEN] = "\0";

#if 0
	g_console.BindScript(context, "quicklaunch");
	g_console.CreateVar(context, "account", accountName, MAX_DEV_NAME_LEN);
	g_console.CreateVar(context, "level", levelName, MAX_DEV_NAME_LEN);
	g_console.CreateVar(context, "levelmp", mpLevelName, MAX_DEV_NAME_LEN);
	g_console.CreateVar(context, "levelsp", spLevelName, MAX_DEV_NAME_LEN);
	g_console.CreateVar(context, "leveldb", dbLevelName, MAX_DEV_NAME_LEN);
	g_console.CreateVar(context, "sessionname", d_sessionName, MAX_DEV_NAME_LEN);
	g_console.CreateVar(context, "WANHostIP", d_wanHostIP, MAX_DEV_NAME_LEN);
	g_console.CreateVar(context, "character", characterName, MAX_DEV_NAME_LEN);
	if (g_console.ExecuteContext(context))
		g_console.Notify("QuickLaunch (quicklaunch.var) configuration detected.\n");
	g_console.DeleteContext(context);
#endif

	// Parse the account name.
	if (accountName[0] != '\0')
		g_playerProfile.SetAccountName(accountName);

	// Parse the level names.
	if (levelName[0] != '\0')
	{
		int level = g_gameEnvironment.ParseWorldName(levelName);
		if (level < 0)
		{
			g_console.Notify("Unrecognized level specified in "
					"quicklaunch.var: %s\n", levelName);
		}
		else
		{
			d_level = level;
			if (d_modeName[0] == '\0')
				strcpy(d_modeName, "dev");
		}
	}
	if (mpLevelName[0] != '\0')
	{
		int level = g_gameEnvironment.ParseWorldName(mpLevelName,
				CGameEnv::TYPE_MULTIPLAYER);
		if (level < 0)
		{
			g_console.Notify("Unrecognized multi-player level specified in "
					"quicklaunch.var: %s\n", mpLevelName);
		}
		else
		{
			if (d_level >= 0)
				g_console.Notify("Multiple levels specified in quicklaunch.var.\n");
			d_level = level;
			if (d_modeName[0] == '\0')
				strcpy(d_modeName, "war");
		}
	}
	if (spLevelName[0] != '\0')
	{
		int level = g_gameEnvironment.ParseWorldName(spLevelName,
				CGameEnv::TYPE_SINGLEPLAYER);
		if (level < 0)
		{
			g_console.Notify("Unrecognized single-player level specified in "
					"quicklaunch.var: %s\n", spLevelName);
		}
		else
		{
			if (d_level >= 0)
				g_console.Notify("Multiple levels specified in quicklaunch.var.\n");
			d_level = level;
			if (d_modeName[0] == '\0')
				strcpy(d_modeName, "sp");
		}
	}
	if (dbLevelName[0] != '\0')
	{
		int level = g_gameEnvironment.ParseWorldName(dbLevelName,
				CGameEnv::TYPE_DEBUG);
		if (level < 0)
		{
			g_console.Notify("Unrecognized debug level specified in "
					"quicklaunch.var: %s\n", dbLevelName);
		}
		else
		{
			if (d_level >= 0)
				g_console.Notify("Multiple levels specified in quicklaunch.var.\n");
			d_level = level;
			if (d_modeName[0] == '\0')
				strcpy(d_modeName, "dev");
		}
	}

	// Parse the character name.
	if (characterName[0] != '\0')
	{
		d_character = PlayerDB::CharacterIndexFromName(characterName);
		if (d_character < 0)
		{
			g_console.Notify("Unrecognized character specified in "
					"quicklaunch.var: %s\n", characterName);
		}
	}

	// Process the IP address.
	if(d_wanHostIP[0] != '\0')
	{
		char *p = d_wanHostIP;
		while(*p)
		{
			// The console reverses forward slashes to back slashes.
			if(*p == '\\')
				*p = '/';
			p++;
		}
		// Now attach the port which gets lost for some reason.
		strcpy(p, ";port=9103");
	}

#ifndef CDROM
	//////////////////////////////////////////////////////////////////////////
	// PROCESSING THE COMMAND LINE - OVERRIDE THE .VAR CONFIG FILE
	//////////////////////////////////////////////////////////////////////////

	GameApp *app = GameApp::Application();

	// check out the command line
	if (app->CommandLine())
	{
		char *arg;
		const char *line = app->CommandLine();

		// Skip blanks
		while (*line == ' ')
			++line;

		// Parse account
		if ((arg = strstr(line, "-account=")) != 0)
		{
			char accountString[MAX_DEV_NAME_LEN];
			arg += strlen("-account=");
			if (arg[0] == '\"')
			{
				strncpy(accountString, arg + 1, sizeof(accountString));
				strtok(accountString, "\"");
			}
			else
			{
				strncpy(accountString, arg, sizeof(accountString));
				strtok(accountString, " ,\t\n\"\'");
			}

			if (accountString[0] != '\0')
			{
				if ((g_playerProfile.GetAccountName() != NULL) &&
						(*g_playerProfile.GetAccountName() != '\0'))
				{
					g_console.Notify("Overridding account specified in "
							"quicklaunch.ver with account specified on command "
							"line.\n");
				}
				g_playerProfile.SetAccountName(accountString);
				CInitialLoop::s_preloadProfile =
						CSaveGame::LoadImmediately(accountString);

				// Skip fluff when an account is set even it if fails to load.
				CInitialLoop::s_skipFluff = true;
			}
		}

		// Parse level names.
		int cmdLevel = -1;
		if ((arg = strstr(line, "-level=")) != 0)
		{
			arg += strlen("-level=");
			int level = g_gameEnvironment.ParseWorldName(arg);
			if (level < 0)
			{
				g_console.Notify("Unrecognized level specified on "
						"command line: %s\n", arg);
			}
			else
			{
				cmdLevel = level;
				if (d_modeName[0] == '\0')
					strcpy(d_modeName, "dev");
			}
		}
		if ((arg = strstr(line, "-levelmp=")) != 0)
		{
			arg += strlen("-levelmp=");
			int level = g_gameEnvironment.ParseWorldName(arg,
					CGameEnv::TYPE_MULTIPLAYER);
			if (level < 0)
			{
				g_console.Notify("Unrecognized multi-player level specified on "
						"command line: %s\n", arg);
			}
			else
			{
				if (cmdLevel >= 0)
					g_console.Notify("Multiple levels specified on command line\n");
				cmdLevel = level;
				if (d_modeName[0] == '\0')
					strcpy(d_modeName, "war");
			}
		}
		if ((arg = strstr(line, "-levelsp=")) != 0)
		{
			arg += strlen("-levelsp=");
			int level = g_gameEnvironment.ParseWorldName(arg,
					CGameEnv::TYPE_SINGLEPLAYER);
			if (level < 0)
			{
				g_console.Notify("Unrecognized single-player level specified on "
						"command line: %s\n", arg);
			}
			else
			{
				if (cmdLevel >= 0)
					g_console.Notify("Multiple levels specified on command line\n");
				cmdLevel = level;
				if (d_modeName[0] == '\0')
					strcpy(d_modeName, "sp");
			}
		}
		if ((arg = strstr(line, "-leveldb=")) != 0)
		{
			arg += strlen("-levelsp=");
			int level = g_gameEnvironment.ParseWorldName(arg,
					CGameEnv::TYPE_DEBUG);
			if (level < 0)
			{
				g_console.Notify("Unrecognized debug level specified on "
						"command line: %s\n", arg);
			}
			else
			{
				if (cmdLevel >= 0)
					g_console.Notify("Multiple levels specified on command line\n");
				cmdLevel = level;
				if (d_modeName[0] == '\0')
					strcpy(d_modeName, "dev");
			}
		}
		if (cmdLevel >= 0)
		{
			if (d_level >= 0)
				g_console.Notify("Overridding level specified in quicklaunch.ver "
						"with level specified on command line.\n");
			d_level = cmdLevel;
		}

		// Parse character
		if ((arg = strstr(line, "-character=")) != 0)
		{
			char characterString[MAX_DEV_NAME_LEN];
			arg += strlen("-character=");
			if (arg[0] == '\"')
			{
				strncpy(characterString, arg + 1, sizeof(characterString));
				strtok(characterString, "\"");
			}
			else
			{
				strncpy(characterString, arg, sizeof(characterString));
				strtok(characterString, " ,\t\n\"\'");
			}

			if (characterString[0] != '\0')
			{
				int character = PlayerDB::CharacterIndexFromName(characterString);
				if (character < 0)
				{
					g_console.Notify("Unrecognized character specified on "
							"command line: %s\n", characterString);
				}
				else
				{
					if (d_character >= 0)
					{
						g_console.Notify("Overridding character specified in "
								"quicklaunch.ver with level specified on command "
								"line.\n");
					}
					d_character = character;
				}
			}
		}

		// grab the session name from the command line
		if ((arg = strstr(line, "-sessionname=")) != 0)
		{
			char sessionString[MAX_DEV_NAME_LEN];
			strncpy(sessionString, arg + strlen("-sessionname="),
					sizeof(sessionString));
			char* name = strtok(sessionString, " ,\t\n\"\'");
			if (name[0] != '\0')
			{
				if (d_sessionName[0] != '\0')
				{
					g_console.Notify("Overridding session name specified in "
							"quicklaunch.ver with session name specified on command "
							"line.\n");
				}
				strncpy(d_sessionName, name, sizeof(d_sessionName));
			}
		}

		// select the game mode
		if ((arg = strstr(line, "-mode=")) != 0)
		{
			char modeString[MAX_DEV_NAME_LEN];
			strncpy(modeString, arg + strlen("-mode="), sizeof(modeString));
			char* mode = strtok(modeString, " ,\t\n\"\'");
			if (mode[0] != '\0')
			{
				if (!strncmp(mode, "mode_", strlen("mode_")))
					mode += strlen("mode_");
				if (mode[0] != '\0')
					strncpy(d_modeName, mode, sizeof(d_modeName));
			}
		}

		// set a log path
		if ((arg = strstr(line, "-logpath=")) != 0)
		{
			char logString[256];
			strncpy(logString, arg + strlen("-logpath="), sizeof(logString));
			g_logUtil.SetPath(strtok(logString, " ,\t\n\"\'"));
			g_logUtil.Enable(true);
		}

		// set a log path
		if ((arg = strstr(line, "-logdir=")) != 0)
		{
			char logString[256];
			strncpy(logString, arg + strlen("-logdir="), sizeof(logString));
			g_logUtil.SetDirectory(strtok(logString, " ,\t\n\"\'"));
			g_logUtil.Enable(true);
		}

		// set a full log path
		if ((arg = strstr(line, "-log=")) != 0)
		{
			char logString[256];
			strncpy(logString, arg + strlen("-log="), sizeof(logString));
			g_logUtil.SetPath(NULL);
			g_logUtil.SetDirectory(strtok(logString, " ,\t\n\"\'"));
			g_logUtil.Enable(true);
		}

		// disable use of the log path
		if ((arg = strstr(line, "-disablelogpath")) != 0)
			g_logUtil.Enable(false);

		// turn on agent logging
		if ((arg = strstr(line, "-logagents")) != 0)
			Referee::EnableAgentsLog(true);

		// turn on lock logging
		if ((arg = strstr(line, "-loglocks")) != 0)
			CSession::EnableLockLog(true);

		// turn on messsage logging
		if ((arg = strstr(line, "-logmessage")) != 0)
			g_messageDispatcher.EnableLog(true);

		// filter message logging
		if ((arg = strstr(line, "-messagelogfilter=")) != 0)
		{
			int filter;
			char filterString[10];
			strncpy(filterString, arg + strlen("-messagelogfilter="),
					sizeof(filterString));
			filterString[9] = '\0';
			if ((strlen(filterString) > 2) && (filterString[0] == '0') &&
					((filterString[1] == 'x') || (filterString[1] == 'X')))
				sscanf(filterString + 2, "%x", &filter);
			else
				sscanf(filterString, "%d", &filter);
			ASSERT((filter >= 0x00) && (filter <= 0xFF));
			if ((filter >= 0x00) && (filter <= 0xFF))
				g_messageDispatcher.SetLogFilter((uint8)filter);
		}

		// turn on debug output logging
		if ((arg = strstr(line, "-logoutput")) != 0)
			g_debugOutput.EnableLog(true);

		// turn off music
		if (strstr(line, "-nomusic") != 0)
			CTTLLoop::s_noMusic = true;

		// turn off fluff
		if (strstr(line, "-skipfluff") != 0)
			CInitialLoop::s_skipFluff = true;

		// turn off any AI players
		if (strstr(line, "-noai") != 0)
			WorldActorLoader::DisableCombatants();

		if (strstr(line, "-noactors") != 0)
			WorldActorLoader::DisableActors();
	}
#endif //CDROM

	if (d_sessionName[0] != '\0')
		d_active = true;

	if (d_character >= 0)
	{
		d_active = true;
		g_playerProfile.SetAllCharacters(d_character);
	}

	if (d_level >= 0)
	{
		d_active = true;
		g_gameEnvironment.Set(d_level);
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CDevLobby::SyncLevel(void)
{
	d_level = g_gameEnvironment.Get();
}
