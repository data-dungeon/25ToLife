/*
**
**  File:   StandAloneLoop.h
**  Date:   September 29, 2004
**  By:     Bryant Collard
**  Desc:   Gameplay loop for multi-player sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/StandAloneLoop.h $
**  $Revision: #13 $
**  $DateTime: 2005/08/03 19:20:26 $
**    $Author: Dwain_Skinner $
**
*/

#pragma once

#ifndef STANDALONE_LOOP_H
#define STANDALONE_LOOP_H

#include "main/TTLLoop.h"
#include "Network/Session.h"
#include "main/TTLSessionMgr.h"


enum {
	STANDALONE_IDLE,
	STANDALONE_AUTOSAVE,
	STANDALONE_AUTOSAVE_RESULT,
	STANDALONE_AUTOSAVE_DONE,
};

class CPlayerAgent;

class CStandAloneLoop : public CTTLLoop
{
  public:
	CStandAloneLoop();

	bool SwitchingLevels()
		{ return d_switchingLevels; }

#ifdef DIRECTX_PC
	void SetCheckpoint();
#endif

  protected:
	virtual int DetermineState(int i_state);
	virtual const char *LoopName() {return("StandAloneLoop");}

  private:
	// The main loop states and associated functions.
	enum ELoopStates
	{
		UNINITIALIZED = UNDEFINED_STATE,
		INITIALIZE = 0,
		WAIT_TO_PUBLISH,
		PUBLISH,
		WAIT_TO_RUN,
		RUN,
		REPORTING,
		UNPUBLISH,
		TERMINATE,
		NUM_STATES
	};
	static SLoopEntry s_stateArray[NUM_STATES]; // Array of state functions
	void Initialize(void);
	void Publish(void);
	void Wait(void);
	void Run(void);
	void Reporting(void);
	void Unpublish(void);
	void Terminate(void);
	void MemCardStateEngine();

	// Loading
	bool Load(void);
	void Unload(void);
	bool d_loaded;

	// The session manager.
	CTTLSessionMgr d_sessionMgr;

	// The local agent.
	CPlayerAgent* d_localAgent;

	// A debug object.
	bool InterceptControls(void);

#ifndef CONSUMER_BUILD
	CTTLDebug d_ttlDebug;
#endif

	// Are we switching levels?
	bool d_switchingLevels;

	// The end of round movie
	const char *d_endofLevelFMV;

	// Nicer switch level stuff
	uint			d_switchLevelTime;
	int			d_nextLevel;
	void HandleSwitchLevel();

	// Message handling
	REF_SEND_MESSAGE_HANDLER(CQuit, CStandAloneLoop) d_quitHandler;
	REF_BASIC_MESSAGE_HANDLER(CSwitchLevel, MESSAGE_STRING, CStandAloneLoop)	m_switchLevelHandler;
	REF_SEND_MESSAGE_HANDLER(CAcceptInvitation, CStandAloneLoop) d_acceptInvitationHandler;
};

#endif // STANDALONE_LOOP_H
