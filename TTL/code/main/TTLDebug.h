/*
**
**  File:   TTLDebug.h
**  Date:   March 25, 2004
**  By:     Bryant Collard
**  Desc:   Debugging stuff from ttl loop.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/TTLDebug.h $
**  $Revision: #24 $
**  $DateTime: 2005/08/15 13:40:59 $
**    $Author: Food $
**
*/

#pragma once

#ifndef TTL_DEBUG_H
#define TTL_DEBUG_H

#include "hud/callback.h"
#include "Math/Vector.h"
#include "GameHelper/Message.h"
#include "GameHelper/Handle.h"
#include "GameHelper/ErrorStack.h"

class CTTLLoop;
class GameHud;
class DefaultControlMapper;
class SoundStream3D;
class ScreenPopupDialog;
handledef(CActor) ActorHandle;

class CTTLDebug
{
  public:
	CTTLDebug();
	~CTTLDebug();
	void Initialize(CTTLLoop* i_ttlLoop);
	void Update(void);
	void Terminate(void);

	void ActivateDebugMenu( void );
	void DeactivateDebugMenu( void );

	bool Profiling(void) {return(d_profiling);}
	void SyncTimerSpeed(void);
	bool InterceptControls(void);

  private:
	void SetupDebugMenu(void);
	static void ProcessMenuCommand(int i_command, PARAM i_param);
	void ProcessMenu(int i_command);
	bool HandleDebugMenu(void);
	void HandleInput(void);
	void CollisionDebug(void);
	void HandleTimerChanges(void);
	void ShowMemoryReport(void);

	// The associated TTL loop.
	CTTLLoop* d_ttlLoop;

	bool d_plotCollisionStack;
	bool d_profiling;
	bool d_surfaceDebug;
	int d_collisionDebug;
	bool d_collisionFrozen;
	Vector3 d_collisionDir;
	Vector3 d_collisionEyepoint;
	static float s_consoleTimerSpeed;
	static bool s_consoleQuit;
	bool d_toggleTestHud;
	bool d_spawnPointDebug;

	ActorHandle d_select;
	ErrorHandle d_errorHandle;
	ScreenPopupDialog* d_debugOutputPopup;

	int d_loadout;

	// Manage the debug menu.
	DefaultControlMapper* d_menuMapper;

	// Manage messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CSyncTimer, CTTLDebug) d_syncTimerHandler;
	REF_SEND_MESSAGE_HANDLER(CDialogCommand, CTTLDebug) d_dialogCommandHandler;
	REF_SEND_MESSAGE_HANDLER(CDialogCancel, CTTLDebug) d_dialogCancelHandler;

	// Static members
	static GameHud* s_hud;
	static bool s_showMemoryReport;

	friend void WriteDebugMessage(int line, const char *Message);
};

#endif // TTL_DEBUG_H
