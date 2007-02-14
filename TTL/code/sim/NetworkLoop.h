/*
**
**  File:   NetworkLoop.h
**  Date:   September 29, 2004
**  By:     Bryant Collard
**  Desc:   Gameplay loop for multi-player sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/NetworkLoop.h $
**  $Revision: #27 $
**  $DateTime: 2005/08/27 18:42:45 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef NETWORK_LOOP_H
#define NETWORK_LOOP_H

#include "main/TTLLoop.h"
#include "Network/Session.h"
#include "main/TTLSessionMgr.h"

class CPlayerAgent;
class ControllerInputDriver;

class CNetworkLoop : public CTTLLoop, public CSessionLockRequest
{
  public:
	CNetworkLoop();
	virtual void RequestExit(void);

  protected:
	virtual int DetermineState(int i_state);
	virtual const char *LoopName() {return("NetworkLoop");}

  private:
	// The main loop states and associated functions.
	enum ELoopStates
	{
		UNINITIALIZED = UNDEFINED_STATE,
		OPEN_SESSION = 0,
		WAIT_TO_OPEN_MATCH,
		WAIT_FOR_IDENTITY,
		WAIT_FOR_READY_OR_SETUP,
		ENTER_READY,
		WAIT_FOR_POPULATE,
		WAIT_FOR_REPOPULATE,
		ENTER_JOIN,
		WAIT_TO_JOIN,
		WAIT_FOR_DISTRIBUTE,
		WAIT_FOR_LOCK_RESOLVE,
		PUBLISH,
		WAIT_TO_RUN,
		ENTER_RUN,
		RUN,
		EXIT_RUN,
		UNPUBLISH,
		WAIT_FOR_EMIGRATION,
		ENTER_GHOST_READY,
		JOIN_GHOST_READY,
		WAIT_FOR_GHOST_READY,
		LEAVE_GHOST_READY,
		WAIT_FOR_GHOST_REPORT,
		START_GHOST,
		WAIT_TO_RUN_GHOST,
		ENTER_RUN_GHOST,
		RUN_GHOST,
		STOP_GHOST,
		LEAVE_ROUND,
		LEAVE_SESSION,
		DISABLE_SESSION,
		WAIT_TO_CLOSE_SESSION,
		WAIT_TO_CLOSE_MATCH,
		CLOSE_SESSION,
		RESET,
		NUM_STATES
	};

	static SLoopEntry s_stateArray[NUM_STATES]; // Array of state functions
	void Wait(void);
	void RenderWait(void);
	void OpenSession(void);
	void EnterReady(void);
	void Publish(void);
	void EnterRun(void);
	void Run(void);
	void ExitRun(void);
	void Unpublish(void);
	void EnterRunGhost(void);
	void StartGhost(void);
	void StopGhost(void);
	void DisableSession(void);
	void CloseSession(void);
	void Reset(void);
	bool ProcessPreload(void);

	// Loading
	bool Load(void);
	void Unload(void);
	bool d_loaded;

	// The session manager.
	CTTLSessionMgr d_sessionMgr;

	// The local agent.
	CPlayerAgent* d_localAgent;

	// The controller.
	bool InterceptControls(void);
	ControllerInputDriver* d_controller;

	// A debug object.
#ifndef CONSUMER_BUILD
	CTTLDebug d_ttlDebug;
#endif

	// Session locks
	virtual void LockEstablished(void);
	enum
	{
		LOCK_INACTIVE,
		LOCK_READY,
		LOCK_CLOSE_SESSION,
		LOCK_ESTABLISHED
	} d_lockState;

	// Message handling
	REF_SEND_MESSAGE_HANDLER(CQuit, CNetworkLoop) d_quitHandler;
	REF_SEND_MESSAGE_HANDLER(CAcceptInvitation, CNetworkLoop)
		d_acceptInvitationHandler;

	// Service network while idle.
	static void ServiceConnection(void);
	static void PopulateWorld(void);

	// Tasks to wait for.
	bool IsDesiredScreenReady(void);
	bool ReadyToReset(void);

	// Display screens.
	enum
	{
		NO_SCREEN,
		SESSION_SCREEN,
		READY_SCREEN,
		LEAVING_SCREEN,
		REPORTING_SCREEN
	} d_currentScreen;
	void GoToSessionScreen(void);
	void GoToReadyScreen(void);
	void GoToLeavingScreen(void);
	void GoToReportScreen(void);
};

#endif // NETWORK_LOOP_H
