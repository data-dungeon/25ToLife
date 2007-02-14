/********************************************************************
	$File: //depot/TTL/code/sim/MultiPlayerLoop.h $
	$Revision: #11 $
	$DateTime: 2005/05/03 16:18:03 $
	$Author: Bryant_Collard $
	purpose: Implement the initial game loop.
*********************************************************************/

#pragma once

#ifndef _MULTIPLAYER_LOOP_H
#define _MULTIPLAYER_LOOP_H

#include "Loop/gameloop.h"
#include "GameHelper/Message.h"
#include "main/Lobby.h"

class CMultiPlayerLoop : public GameLoop
{
  public:
	enum ENextLoop
	{
		NO_NEXT,
		INITIAL_NEXT,
		CUSTOMPLAYER_NEXT,
		SIM_NEXT
	};

	CMultiPlayerLoop();
	ENextLoop GetNextLoop() {return(d_nextLoop);}

  protected:
	virtual int DetermineState(int i_state);

	// Some debugging stuff
	virtual const char *LoopName() {return("MultiPlayerLoop");}

  private:
	// The main loop states and associated functions.
	enum ELoopStates
	{
		UNINITIALIZED = UNDEFINED_STATE,
		INITIALIZE = 0,
		RUN,
		TERMINATE,
		NUM_STATES
	};
	static SLoopEntry s_stateArray[NUM_STATES];

	// Loop functions
	void Initialize(void);
	void Terminate(void);

	// State
	ENextLoop d_nextLoop;

	// Messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CBackup, CMultiPlayerLoop) d_backupHandler;
	REF_SEND_MESSAGE_HANDLER(CSettings, CMultiPlayerLoop) d_settingsHandler;
	REF_SEND_MESSAGE_HANDLER(CCreate, CMultiPlayerLoop) d_createHandler;
	REF_SEND_MESSAGE_HANDLER(CLaunch, CMultiPlayerLoop) d_launchHandler;
	REF_SEND_MESSAGE_HANDLER(CCancelSearch, CMultiPlayerLoop)
		d_cancelSearchHandler;
	REF_SEND_MESSAGE_HANDLER(CAcceptInvitation, CMultiPlayerLoop)
		d_acceptInvitationHandler;

	// Lobby
	CLanLobby d_lobby;
};

#endif // _MULTIPLAYER_LOOP_H
