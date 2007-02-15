/*
**
**  File:   SessionTime.h
**  Date:   May 1, 2004
**  By:     Bryant Collard
**  Desc:   Maps between network time, session time and game time. Network
**          time is the time provided by the SessionClock and is of type Time.
**          Session time is of type int32 and is the number of milliseconds
**          since a start time agreed upon by all stations in a session. Game
**          time is of type float and is the local game clock.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/SessionTime.h $
**  $Revision: #21 $
**  $DateTime: 2005/08/24 17:01:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef SESSION_TIME_H
#define SESSION_TIME_H

#ifndef CDROM
#define DEBUG_SESSION_TIME
#endif

#include <netz.h>
#include "Network/NetZ/AvalancheDefines.h"
#include "EngineHelper/Singleton.h"
#include "Math/Vector.h"
#include "mathUtil/Kalman.h"

#ifdef DEBUG_SESSION_TIME
#include "GameHelper/LogUtil.h"
#endif

class DOWorld;
class DOActor;

class CSessionTime
{
  public:
	CSessionTime();

	// Manage session time.
	void StartSessionTime(void);
	void StopSessionTime(void);
	void Update(void);

	// Get session time.
	int32 GetSessionTime(void);
	int32 GetSessionFrameStartTime(void);
	int32 GetSessionFrameEndTime(void);

	// Convert between session and network time scales.
	Time ConvertTimeSessionToNet(int32 i_sessionTime);
	int32 ConvertTimeNetToSession(Time i_netTime);

	// Convert between game (local host) and network time scales.
	Time ConvertTimeGameToNet(float i_gameTime);
	float ConvertTimeNetToGame(Time i_netTime);

	// Convert between game (local host) and session time scales.
	int32 ConvertTimeGameToSession(float i_gameTime);
	float ConvertTimeSessionToGame(int32 i_sessionTime);

	// Convert rates between session and game.
	float ConvertRateGameToSessionSec(float i_gameRate);
	void ConvertRateGameToSessionSec(Vector3CRef i_gameRate,
			Vector3 &o_sessionRate);
	float ConvertRateSessionSecToGame(float i_sessionRate);
	void ConvertRateSessionSecToGame(Vector3CRef i_sessionRate,
			Vector3 &o_gameRate);

#ifdef LATENCY_COMPENSATION
	// Convert between host and network time scales.
	Time ConvertTimeHostToNet(DOActor* i_hostActor, float i_hostTime);
	float ConvertTimeNetToHost(DOActor* i_hostActor, Time i_netTime);

	// Convert between host and session time scales.
	int32 ConvertTimeHostToSession(DOActor* i_hostActor, float i_hostTime);
	float ConvertTimeSessionToHost(DOActor* i_hostActor, int32 i_sessionTime);

	// Convert rates from host to game.
	float ConvertRateHostToGame(DOActor* i_hostActor, float i_hostRate);
	void ConvertRateHostToGame(DOActor* i_hostActor, Vector3CRef i_hostRate,
			Vector3 &o_gameRate);

	// Latency debugging support.
	void GetTimeProperties(DOActor* i_hostActor, float &o_hostTimePerSessionTime,
			float &o_hostVsSessionIntercept);
#endif

	// Debugging support.
	void ToggleDebug(void);
	void SetDebug(bool i_active);

  private:
	void Reset(void);
	float InitOsVsSession(void);
	float UpdateOsVsSession(void);
	void Adjust(float i_adjustTime);
	int32 ConvertTimeHostToSession(float i_hostTime, float i_hostPerSession,
			float i_hostVsSessionIntercept);
	float ConvertTimeSessionToHost(int32 i_sessionTime, float i_hostPerSession,
			float i_hostVsSessionIntercept);

	DOWorld* d_world;
	float d_osPerSession;
	float d_osVsSessionIntercept;
	float d_gamePerSession;
	float d_gameVsSessionIntercept;
	float d_currOsPerSession;
	float d_currOsVsSessionIntercept;
	float d_lastAdjustTime;

	CKalmanDerivative d_fitFilter;

#ifdef DEBUG_SESSION_TIME
	void LogFit(float i_osTime, float i_sessionTime);
	void LogAdjustStart(float i_time, int32 i_sessionEnd,
			float i_computedGameEnd);
	void LogAdjustEnd(float i_gamePerOs);
	CLogFile d_logFit;
	CLogFile d_logAdjust;
	static int s_logCount;
#endif
};

DefineSingleton(CSessionTime)
#define g_sessionTime GetSingleton(CSessionTime)

#endif // SESSION_TIME
