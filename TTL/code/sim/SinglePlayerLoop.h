/********************************************************************
	$File: //depot/TTL/code/sim/SinglePlayerLoop.h $
	$Revision: #7 $
	$DateTime: 2005/05/03 16:18:03 $
	$Author: Bryant_Collard $
	purpose: Implement the initial game loop.
*********************************************************************/

#pragma once

#ifndef _SINGLEPLAYER_LOOP_H
#define _SINGLEPLAYER_LOOP_H

#include "Loop/gameloop.h"
#include "GameHelper/Message.h"

class CSinglePlayerLoop : public GameLoop
{
  public:
	enum ENextLoop
	{
		NO_NEXT,
		INITIAL_NEXT,
		SIM_NEXT
	};

	CSinglePlayerLoop();
	ENextLoop GetNextLoop() {return(d_nextLoop);}

  protected:
	virtual int DetermineState(int i_state);

	// Some debugging stuff
	virtual const char *LoopName() {return("SinglePlayerLoop");}

  private:
	// LoopStates
	enum ELoopStates
	{
		UNINITIALIZED = UNDEFINED_STATE,
		INITIALIZE = 0,
		RUN,
		TERMINATE,
		NUM_STATES
	};
	static SLoopEntry s_stateArray[NUM_STATES];

	// Loop Functions
	void Initialize(void);
	void Terminate(void);

	// State
	ENextLoop d_nextLoop;

	// Messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CLaunch, CSinglePlayerLoop) d_launchHandler;
	REF_SEND_MESSAGE_HANDLER(CBackup, CSinglePlayerLoop) d_backupHandler;
	REF_SEND_MESSAGE_HANDLER(CAcceptInvitation, CSinglePlayerLoop)
		d_acceptInvitationHandler;
};

#endif // _SINGLEPLAYER_LOOP_H
