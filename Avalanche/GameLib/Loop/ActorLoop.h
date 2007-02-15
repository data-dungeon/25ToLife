/*
**
**  File:   ActorLoop.h
**  Date:   April 7, 2004
**  By:     Bryant Collard
**  Desc:   A gameloop that adds actor to the base functionality.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/ActorLoop.h $
**  $Revision: #5 $
**  $DateTime: 2005/04/26 17:49:42 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef ACTOR_LOOP_H
#define ACTOR_LOOP_H

#include "Loop/BaseLoop.h"

class CEnvironment;

class CActorLoop : public CBaseLoop
{
  public:
	CActorLoop();
	~CActorLoop();

  protected:
	CActorLoop(SLoopEntry* i_stateArray, int i_loopState);

	void BeginActorInitialize(void);
	bool ActorLoadEnvironment(int i_numScenes)
		{return(ActorLoadEnvironment(d_environment, i_numScenes));}
	bool ActorLoadEnvironment(CEnvironment* i_environment, int i_numScenes);
	void EndActorInitialize(void);
	void BeginActorRun(void);
	void EndActorRun(void);
	void ActorRender(void);
	void ActorUnloadEnvironment(void);
	void ActorTerminate(void);
	void ActorReset(void);

	virtual int DetermineState(int i_state);

	// Some debugging stuff
	virtual const char *LoopName() {return("ActorLoop");}

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
};

#endif // ACTOR_LOOP_H
