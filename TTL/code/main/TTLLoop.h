/*
**
**  File:   TTLLoop.h
**  Date:   April 8, 2004
**  By:     Bryant Collard
**  Desc:   A general TTL game loop.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/TTLLoop.h $
**  $Revision: #19 $
**  $DateTime: 2005/06/29 19:35:05 $
**    $Author: Travis_Nixon $
**
*/

#pragma once

#ifndef TTL_LOOP_H
#define TTL_LOOP_H

#include "Loop/ActorLoop.h"
#include "main/TTLConstants.h"
#include "main/Players.h"
#include "EngineHelper/timer.h"
#include "GameHelper/Message.h"
#include "environ/WorldActorLoader.h"

// Local loop
#include "main/TTLDebug.h"

class CLevelScript;

class CTTLLoop : public CActorLoop
{
  public:
	CTTLLoop();
	~CTTLLoop();

	// Manage timer
	void SetTimerSpeed(CMasterTimer::ETimerSpeed i_speed, bool smooth = false);
	CLoopScenes &GetScenes(void) {return(d_scenes);}

	static bool s_noMusic;
	static WorldActorLoader& WorldLoader() { return s_worldLoader; }

  protected:
	CTTLLoop(SLoopEntry* i_stateArray, int i_loopState);

	void BeginTTLInitialize(void);
	bool TTLLoadEnvironment(int i_numPlayers);
	void EndTTLInitialize(void);
	void BeginTTLRun(void);
	void EndTTLRun(void);
	void TTLRender(void);
	void TTLUnloadEnvironment(void);
	void TTLTerminate(void);
	void TTLReset(void);

	// Manage players
	static CPlayers s_players;

	// Manage world
	static WorldActorLoader s_worldLoader;

	// Loop can receive messages
	CMessageOwner d_messageOwner;

  private:
	// Message handling
	REF_FULL_MESSAGE_HANDLER(CPause, CTTLLoop) d_pauseHandler;
	REF_BASIC_MESSAGE_HANDLER(CTimeSpeed, MESSAGE_VALUE, CTTLLoop) d_timeSpeedHandler;
	uint32																			d_timerReset;

  protected:
	// A level script
	CLevelScript* d_levelScript;

	virtual int DetermineState(int i_state);
	bool d_stateSucceeded;

	ControlState controls[MAX_PLAYERS];

	// Some debugging stuff
	virtual const char *LoopName() {return("TTLLoop");}

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
	static SLoopEntry stateArray[NUM_STATES]; // Array of state functions
	void Initialize(void);
	void Run(void);
	void Terminate(void);

	void SuspendControl( void );
	void RestoreControl( void );
};

#endif // TTL_LOOP_H
