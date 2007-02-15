/*
**
**  File:   BaseLoop.h
**  Date:   April 7, 2004
**  By:     Bryant Collard
**  Desc:   A gameloop that handles base functionality.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/BaseLoop.h $
**  $Revision: #8 $
**  $DateTime: 2005/04/26 17:49:42 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef BASE_LOOP_H
#define BASE_LOOP_H

#include "Loop/gameloop.h"
#include "Loop/LoopScenes.h"

class CEnvironment;

class CBaseLoop : public GameLoop
{
  public:
	// Public construct/destruct
	CBaseLoop();
	virtual ~CBaseLoop();

	// Set an environment.
	void SetEnvironment(CEnvironment* i_environment)
		{d_environment = i_environment;}

  protected:
	// Constructor for derived classes to pass their loop array up.
	CBaseLoop(SLoopEntry* i_stateArray, int i_loopState);

	// Functions that do the actual work. To be called by loop functions.
	void BeginBaseInitialize(void);
	bool BaseLoadEnvironment(int i_numScenes)
		{return(BaseLoadEnvironment(d_environment, i_numScenes));}
	bool BaseLoadEnvironment(CEnvironment* i_environment, int i_numScenes);
	void EndBaseInitialize(void);
	void BeginBaseRun(void);
	void EndBaseRun(void);
	void BaseRender(void);
	void BaseUnloadEnvironment(void);
	void BaseTerminate(void);
	void BaseReset(void);

	// A loop array is implemented here so this class can be used as a game loop
	// in its own right.
	virtual int DetermineState(int i_state);

	// Some debugging stuff
	virtual const char *LoopName() {return("BaseLoop");}

	// General scene management plus a standard set of scenes.
	static CLoopScenes d_scenes;

	// Can use to store an environment.
	static CEnvironment* d_environment;

	// Used by the locally implemented loop functions.
	bool d_initSucceeded;

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
	static SLoopEntry stateArray[NUM_STATES];
	void Initialize(void);
	void Run(void);
	void Terminate(void);
};

#endif // BASE_LOOP_H
