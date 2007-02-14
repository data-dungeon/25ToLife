/*
**
**  File:   DevLobby.h
**  Date:   March 31, 2004
**  By:     Bryant Collard
**  Desc:   A simple lobby used during development.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/DevLobby.h $
**  $Revision: #10 $
**  $DateTime: 2005/06/06 20:50:48 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DEV_LOBBY_H
#define DEV_LOBBY_H

#include "Loop/gameloop.h"
#include "GameHelper/Message.h"
#include "main/Lobby.h"

#define MAX_DEV_NAME_LEN  128

class CDevLobby : public GameLoop
{
  public:
	enum ENextLoop
	{
		NO_NEXT,
		INITIAL_NEXT,
		SINGLEPLAYER_NEXT,
		MULTIPLAYER_NEXT
	};

	CDevLobby();

	bool ParseOverrides(void);
	void SyncLevel(void);
	bool Active(void) {return(d_active);}

	// Where do we go from here?
	ENextLoop GetNextLoop(void) {return(d_nextLoop);}

	void Launch(void);

  protected:
	virtual int DetermineState(int i_state);

	// Some debugging stuff
	virtual const char *LoopName() {return("DevLobby");}

  private:
	// Loop states.
	enum ELoopStates
	{
		UNINITIALIZED = UNDEFINED_STATE,
		INITIALIZE = 0,
		RUN,
		LAUNCH,
		TERMINATE,
		NUM_STATES
	};
	static SLoopEntry s_stateArray[NUM_STATES];

	// Loop functions.
	void Initialize(void);
	void Terminate(void);

	// Find a session. Move to NetZSession?
	bool FindSession(void);

	// State flags.
	ENextLoop d_nextLoop;
	bool d_launch;
	bool d_active;

	// Overrides
	int d_character;
	int d_level;
	char d_sessionName[MAX_DEV_NAME_LEN];
	char d_wanHostIP[MAX_DEV_NAME_LEN];
	char d_modeName[MAX_DEV_NAME_LEN];

	// Messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(Launch, CDevLobby) d_launchHandler;
	REF_SEND_MESSAGE_HANDLER(Backup, CDevLobby) d_backupHandler;
	REF_SEND_MESSAGE_HANDLER(Invite, CDevLobby) d_inviteHandler;

	// Lobby
	CLanLobby d_lobby;
};

#endif // DEV_LOBBY_H
