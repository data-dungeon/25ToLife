/*
**
**  File:   SetupLoop.h
**  Date:   September 21, 2004
**  By:     Bryant Collard
**  Desc:   Meta loop used to setup sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/SetupLoop.h $
**  $Revision: #8 $
**  $DateTime: 2005/08/03 19:20:26 $
**    $Author: Dwain_Skinner $
**
*/

#pragma once

#ifndef SETUP_LOOP_H
#define SETUP_LOOP_H

#include "main/TTLLoop.h"
#include "sim/InitialLoop.h"
#include "sim/SinglePlayerLoop.h"
#include "sim/MultiPlayerLoop.h"
#include "sim/DevLobby.h"
#include "sim/OnlineSetupLoop.h"
#include "sim/CustomPlayerLoop.h"
#include "main/TTLSessionMgr.h"

class GeneralCamera;

// Parameters for camera.
struct SSetupLoopParams
{
	Vector3 d_cameraPosition;
	float d_cameraHeading;
	float d_cameraPitch;
	GeneralCamera* d_camera;
};

class CSetupLoop : public CTTLLoop
{
  public:
	enum ETopLevelLoop
	{
		ENTER_LOOP,
		SINGLEPLAYER_LOOP,
		SYSTEMLINK_LOOP,
		ONLINE_LOOP,
		EXIT_LOOP
	};

	CSetupLoop();
	virtual void RequestExit(void);
	virtual void ClearRequestExit(void);

	ETopLevelLoop GetLoopType(void) {return(d_topLevelLoop);}

  protected:
	virtual int DetermineState(int i_state);

	// Some debugging stuff
	virtual const char *LoopName() {return("SetupLoop");}

  private:
	// The main loop states and associated functions.
	enum ELoopStates
	{
		UNINITIALIZED = UNDEFINED_STATE,
		MAIN_MENU = 0,
		CUSTOM_PLAYER,
		SINGLEPLAYER_LOBBY,
		LAN_LOBBY,
		ONLINE_LOBBY,
		DEV_LOBBY,
		NUM_LOOPS,
		INITIALIZE,
		TERMINATE,
		NUM_STATES
	};
	static SLoopEntry s_stateArray[NUM_STATES]; // Array of state functions

	void Initialize(void);
	void Run(void);
	void Terminate(void);

#ifdef DIRECTX_PC
	void RunFramePC( void );
#endif // DIRECTX_PC

	// Selected session.
	ETopLevelLoop d_topLevelLoop;

	// Subloops.
	GameLoop* d_subLoop[NUM_LOOPS];
	CInitialLoop d_mainMenu;
	CCustomPlayerLoop d_customPlayer;
	CSinglePlayerLoop d_singlePlayerLobby;
	CMultiPlayerLoop d_lanLobby;
#if defined( _XBOX ) || defined( PS2 ) || defined( DIRECTX_PC )
	COnlineSetupLoop d_onlineLobby;
#endif
	CDevLobby d_devLobby;

	// Is a subloop active?
	bool d_subLoopActive;

	// The session manager.
	CTTLSessionMgr d_sessionMgr;

	// A debug object.
#ifndef CONSUMER_BUILD
	CTTLDebug d_ttlDebug;
#endif

	// Parameters
	SSetupLoopParams* d_params;
};

#endif // SETUP_LOOP_H
