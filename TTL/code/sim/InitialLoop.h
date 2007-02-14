/********************************************************************
	$File: //depot/TTL/code/sim/InitialLoop.h $
	$Revision: #11 $
	$DateTime: 2005/05/03 16:18:03 $
	$Author: Bryant_Collard $
	purpose: Implement the initial game loop.
*********************************************************************/

#pragma once

#ifndef _INITIAL_LOOP_H
#define _INITIAL_LOOP_H

#include "Loop/gameloop.h"

class CInitialLoop : public GameLoop
{
  public:
	enum ENextLoop
	{
		NO_NEXT,
		SYSTEMLINK_NEXT,
		ONLINE_NEXT,
		SINGLEPLAYER_NEXT,
		DEVELOPER_NEXT,
	};

	CInitialLoop();
	ENextLoop GetNextLoop(void) {return(d_nextLoop);}

	static bool s_skipFluff;
	static bool s_preloadProfile;

  protected:
	virtual int DetermineState(int i_state);

	// Some debugging stuff
	virtual const char *LoopName() {return("InitialLoop");}

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

	static SLoopEntry s_stateArray[NUM_STATES]; // Array of state functions
	void Initialize(void);
	void Terminate(void);

	// Determine netxt loop.
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CNextLoopHandler, CInitialLoop) d_nextLoopHandler;
	REF_SEND_MESSAGE_HANDLER(CAcceptInvitationHandler, CInitialLoop)
		d_acceptInvitationHandler;
	ENextLoop d_nextLoop;

	static bool s_titleHasBeenShown;
};

#endif // _INITIAL_LOOP_H
