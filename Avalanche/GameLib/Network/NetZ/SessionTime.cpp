/*
**
**  File:   SessionTime.cpp
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
**      $File: //depot/Avalanche/GameLib/Network/NetZ/SessionTime.cpp $
**  $Revision: #22 $
**  $DateTime: 2005/08/24 17:01:26 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/SessionTime.h"

// Engine headers
#include "Math/MathClass.h"
#include "Math/Vector.h"

// Gamelib headers
#include "EngineHelper/timer.h"
#include "Network/NetZ/DOWorld.h"
#include "Network/NetZ/DOHost.h"
#include "Network/NetZ/DOActor.h"
#include "cmdcon/cmdcon.h"

DeclareSingleton(CSessionTime);

#define MAX_TIME_ERROR   0.005f
#define MIN_ADJUST_TIME  500.0f
#define FIT_VARIANCE     0.1f

#ifdef DEBUG_SESSION_TIME
int CSessionTime::s_logCount = 0;
#endif

/******************************************************************************/
/******************************************************************************/
CSessionTime::CSessionTime()
{
	Reset();
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::Reset(void)
{
	d_world = NULL;
	d_osPerSession = 1.0f;
	d_osVsSessionIntercept = 0.0f;
	d_gamePerSession = 0.001f;
	d_gameVsSessionIntercept = 0.0f;
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::InitOsVsSession(void)
{
	// Get the current session and OS times. Do this as close together as
	// possible so they act as a single sample.
	float currOsTime = (float)g_timer.GetOSTime();
	float currSessionTime =
			(float)(int32)(SessionClock::GetTime() - d_world->GetStartTime());

	// Initialize the filter.
	d_fitFilter.SetMeasurementVariance(FIT_VARIANCE);
	d_fitFilter.SetModelCovariance(0.0f, 0.0f, 0.0f);
	d_fitFilter.InitializeErrorCovariance(1.0e-9f, 1.0e-9f, 1.0e-9f);
	d_fitFilter.Initialize(currSessionTime, currOsTime, 1.0f);

	// Set the properties.
	d_currOsPerSession = d_fitFilter.Derivative();
	d_currOsVsSessionIntercept = currOsTime -
			d_currOsPerSession * currSessionTime;

	// Log and return.
#ifdef DEBUG_SESSION_TIME
	LogFit(currOsTime, currSessionTime);
#endif
	return(currOsTime);
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::UpdateOsVsSession(void)
{
	// Get the current session and OS times. Do this as close together as
	// possible so they act as a single sample.
	float currOsTime = (float)g_timer.GetOSTime();
	float currSessionTime =
			(float)(int32)(SessionClock::GetTime() - d_world->GetStartTime());

	// Update the filter.
	d_fitFilter.Update(currSessionTime, currOsTime);

	// Set the properties.
	d_currOsPerSession = d_fitFilter.Derivative();
	d_currOsVsSessionIntercept = currOsTime -
			d_currOsPerSession * currSessionTime;

	// Log and return.
#ifdef DEBUG_SESSION_TIME
	LogFit(currOsTime, currSessionTime);
#endif
	return(currOsTime);
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::Adjust(float i_adjustTime)
{
	// Update the OS versus session time properties with the current best
	// estimate (assumed constant for now).
	d_osPerSession = d_currOsPerSession;
	d_osVsSessionIntercept = d_currOsVsSessionIntercept;

	// Find the rate game time passes relative to the OS time. Note that game
	// time is measured in seconds and OS time is measured in milliseconds.
	float gamePerOs = 0.001f * g_timer.GetSpeedFactor();

	// Compute the game versus session properties.
	d_gamePerSession = gamePerOs * d_osPerSession;
	d_gameVsSessionIntercept = g_timer.GetEndSec() + gamePerOs *
			(d_osVsSessionIntercept - (float)g_timer.GetOSEndMS());

#ifdef LATENCY_COMPENSATION
	// Update the host.
	if (DOHost::GetLocal() != NULL)
	{
		DOHost::GetLocal()->SetTimeProperties(d_gamePerSession,
				d_gameVsSessionIntercept);
	}
#endif

	// Record the time.
	d_lastAdjustTime = i_adjustTime;
#ifndef CONSUMER_BUILD
	g_console.Echo("Adjust session time: Slope = %f, Intercept = %f\n",
			d_gamePerSession, d_gameVsSessionIntercept);
#endif

#ifdef DEBUG_SESSION_TIME
	LogAdjustEnd(gamePerOs);
#endif
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::StartSessionTime(void)
{
	DOWorld::Ref refWorld(g_doWorld);
	if (refWorld.IsValid())
	{
		// Set up the world.
		d_world = *refWorld;
		d_world->StartSessionClock();

		float currOsTime = InitOsVsSession();
		Adjust(currOsTime);
	}
	else
		StopSessionTime();
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::StopSessionTime(void)
{
	Reset();
#ifdef LATENCY_COMPENSATION
	if (DOHost::GetLocal() != NULL)
	{
		DOHost::GetLocal()->SetTimeProperties(d_gamePerSession,
				d_gameVsSessionIntercept);
	}
#endif
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::Update(void)
{
	// Make sure the session time is started.
	if (d_world == NULL)
		return;

	// Find the best current estimate of the OS versus session properties.
	float currOsTime = UpdateOsVsSession();

	// If enough time has passed since the last adjustment, see if the time
	// properties need to be adjusted.
	if ((currOsTime - d_lastAdjustTime) > MIN_ADJUST_TIME)
	{
		// The OS time at the end of the current frame is known. The game time at
		// the end of the frame is known as well and is provided by the game
		// timer.  Using the best current estimate of the OS versus session
		// properties, find the best estimate of the session time at the end of
		// the current frame given the OS time at the end of the current frame.
		int32 sessionEndTime = (int32)(((float)g_timer.GetOSEndMS() -
				d_currOsVsSessionIntercept) / d_currOsPerSession);

		// Find the game time at the end of the current frame from the session
		// time found above using the current properties.
		float computedEndSec = ConvertTimeSessionToHost(sessionEndTime,
				d_gamePerSession, d_gameVsSessionIntercept);

		// Compare this computed end time against the known end time. If the error
		// exceeds the maximum allowed, adjust the properties.
		if (!Math::Equal(g_timer.GetEndSec(), computedEndSec, MAX_TIME_ERROR))
		{
#ifdef DEBUG_SESSION_TIME
			LogAdjustStart(currOsTime, sessionEndTime, computedEndSec);
#endif
			Adjust(currOsTime);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
int32 CSessionTime::GetSessionTime(void)
{
	if (d_world != NULL)
		return((int32)(SessionClock::GetTime() - d_world->GetStartTime()));
	return(g_timer.GetStartMS());
}

/******************************************************************************/
/******************************************************************************/
int32 CSessionTime::GetSessionFrameStartTime(void)
{
	return((int32)(((float)g_timer.GetOSStartMS() - d_osVsSessionIntercept) /
			d_osPerSession));
}

/******************************************************************************/
/******************************************************************************/
int32 CSessionTime::GetSessionFrameEndTime(void)
{
	return((int32)(((float)g_timer.GetOSEndMS() - d_osVsSessionIntercept) /
			d_osPerSession));
}

/******************************************************************************/
/******************************************************************************/
Time CSessionTime::ConvertTimeSessionToNet(int32 i_sessionTime)
{
	ASSERT_PTR(d_world);
	return((Time)(i_sessionTime + d_world->GetStartTime()));
}

/******************************************************************************/
/******************************************************************************/
int32 CSessionTime::ConvertTimeNetToSession(Time i_netTime)
{
	ASSERT_PTR(d_world);
	return((int32)(i_netTime - d_world->GetStartTime()));
}

/******************************************************************************/
/******************************************************************************/
int32 CSessionTime::ConvertTimeHostToSession(float i_hostTime,
		float i_hostPerSession, float i_hostVsSessionIntercept)
{
	float deltaHostTime = i_hostTime - i_hostVsSessionIntercept;
	if (Math::DivByZero(deltaHostTime, i_hostPerSession))
		return(GetSessionTime());
	return((int32)(deltaHostTime / i_hostPerSession));
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::ConvertTimeSessionToHost(int32 i_sessionTime,
		float i_hostPerSession, float i_hostVsSessionIntercept)
{
	return(i_hostPerSession * (float)i_sessionTime + i_hostVsSessionIntercept);
}

/******************************************************************************/
/******************************************************************************/
Time CSessionTime::ConvertTimeGameToNet(float i_gameTime)
{
	ASSERT_PTR(d_world);
	return((Time)(ConvertTimeHostToSession(i_gameTime, d_gamePerSession,
			d_gameVsSessionIntercept) + d_world->GetStartTime()));
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::ConvertTimeNetToGame(Time i_netTime)
{
	ASSERT_PTR(d_world);
	return(ConvertTimeSessionToHost((int32)(i_netTime - d_world->GetStartTime()),
			d_gamePerSession, d_gameVsSessionIntercept));
}

/******************************************************************************/
/******************************************************************************/
int32 CSessionTime::ConvertTimeGameToSession(float i_gameTime)
{
	return(ConvertTimeHostToSession(i_gameTime, d_gamePerSession,
			d_gameVsSessionIntercept));
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::ConvertTimeSessionToGame(int32 i_sessionTime)
{
	return(ConvertTimeSessionToHost(i_sessionTime, d_gamePerSession,
			d_gameVsSessionIntercept));
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::ConvertRateGameToSessionSec(float i_gameRate)
{
	return(1000.0f * d_gamePerSession * i_gameRate);
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::ConvertRateGameToSessionSec(Vector3CRef i_gameRate,
		Vector3 &o_sessionRate)
{
	o_sessionRate = 1000.0f * d_gamePerSession * i_gameRate;
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::ConvertRateSessionSecToGame(float i_sessionRate)
{
	if (Math::DivByZero(i_sessionRate, d_gamePerSession))
		return(i_sessionRate);
	return(i_sessionRate / (1000.0f * d_gamePerSession));
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::ConvertRateSessionSecToGame(Vector3CRef i_sessionRate,
		Vector3 &o_gameRate)
{
	if (Math::DivByZero(1.0f, d_gamePerSession))
		o_gameRate = i_sessionRate;
	else
	{
		float sessionPerGame = 0.001f / d_gamePerSession;
		o_gameRate = sessionPerGame * i_sessionRate;
	}
}

#ifdef LATENCY_COMPENSATION

/******************************************************************************/
/******************************************************************************/
Time CSessionTime::ConvertTimeHostToNet(DOActor* i_hostActor, float i_hostTime)
{
	ASSERT_PTR(d_world);

	DOHost *host = (i_hostActor != NULL) ? i_hostActor->GetHost() : NULL;
	if (host == NULL)
		return((Time)(ConvertTimeHostToSession(i_hostTime, d_gamePerSession,
				d_gameVsSessionIntercept) + d_world->GetStartTime()));

	return((Time)(ConvertTimeHostToSession(i_hostTime,
			host->GetHostTimePerSessionTime(), host->GetHostVsSessionIntercept()) +
			d_world->GetStartTime()));
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::ConvertTimeNetToHost(DOActor* i_hostActor, Time i_netTime)
{
	ASSERT_PTR(d_world);

	DOHost *host = (i_hostActor != NULL) ? i_hostActor->GetHost() : NULL;
	if (host == NULL)
		return(ConvertTimeSessionToHost((int32)(i_netTime -
				d_world->GetStartTime()), d_gamePerSession,
				d_gameVsSessionIntercept));

	return(ConvertTimeSessionToHost((int32)(i_netTime - d_world->GetStartTime()),
			host->GetHostTimePerSessionTime(), host->GetHostVsSessionIntercept()));
}

/******************************************************************************/
/******************************************************************************/
int32 CSessionTime::ConvertTimeHostToSession(DOActor* i_hostActor,
		float i_hostTime)
{
	DOHost *host = (i_hostActor != NULL) ? i_hostActor->GetHost() : NULL;
	if (host == NULL)
		return(ConvertTimeHostToSession(i_hostTime, d_gamePerSession,
				d_gameVsSessionIntercept));

	return(ConvertTimeHostToSession(i_hostTime,
			host->GetHostTimePerSessionTime(), host->GetHostVsSessionIntercept()));
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::ConvertTimeSessionToHost(DOActor* i_hostActor,
		int32 i_sessionTime)
{
	DOHost *host = (i_hostActor != NULL) ? i_hostActor->GetHost() : NULL;
	if (host == NULL)
		return(ConvertTimeSessionToHost(i_sessionTime, d_gamePerSession,
				d_gameVsSessionIntercept));

	return(ConvertTimeSessionToHost(i_sessionTime,
			host->GetHostTimePerSessionTime(), host->GetHostVsSessionIntercept()));
}

/******************************************************************************/
/******************************************************************************/
float CSessionTime::ConvertRateHostToGame(DOActor* i_hostActor,
		float i_hostRate)
{
	DOHost *host = (i_hostActor != NULL) ? i_hostActor->GetHost() : NULL;
	if (host == NULL)
		return(i_hostRate);

	float sessionRate = i_hostRate * host->GetHostTimePerSessionTime();
	if (Math::DivByZero(sessionRate, d_gamePerSession))
		return(sessionRate * 1000.0f);

	return(sessionRate / d_gamePerSession);
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::ConvertRateHostToGame(DOActor* i_hostActor,
		Vector3CRef i_hostRate, Vector3 &o_gameRate)
{
	DOHost *host = (i_hostActor != NULL) ? i_hostActor->GetHost() : NULL;
	if (host == NULL)
		o_gameRate = i_hostRate;
	else
	{
		float hostPerGame = host->GetHostTimePerSessionTime();
		if (Math::DivByZero(hostPerGame, d_gamePerSession))
			hostPerGame *= 1000.0f;
		else
			hostPerGame /= d_gamePerSession;
		o_gameRate = hostPerGame * i_hostRate;
	}
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::GetTimeProperties(DOActor* i_hostActor,
		float &o_hostTimePerSessionTime, float &o_hostVsSessionIntercept)
{
	DOHost *host = (i_hostActor != NULL) ? i_hostActor->GetHost() : NULL;
	if (host == NULL)
	{
		o_hostTimePerSessionTime = d_gamePerSession;
		o_hostVsSessionIntercept = d_gameVsSessionIntercept;
	}
	else
	{
		o_hostTimePerSessionTime = host->GetHostTimePerSessionTime();
		o_hostVsSessionIntercept = host->GetHostVsSessionIntercept();
	}
}

#endif // LATENCY_COMPENSATION

/******************************************************************************/
/******************************************************************************/
void CSessionTime::ToggleDebug(void)
{
#ifdef DEBUG_SESSION_TIME
	bool active = d_logFit.IsOpen() || d_logAdjust.IsOpen();
	SetDebug(!active);
#endif
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::SetDebug(bool i_active)
{
#ifdef DEBUG_SESSION_TIME
	if (i_active)
	{
		int lastCount = s_logCount;
		if (!d_logFit.IsOpen())
		{
			s_logCount = lastCount + 1;
			char rootName[20];
			sprintf(rootName, "TimeFit%d.txt", s_logCount);
			const char* name = g_logUtil.Enabled() ?
					g_logUtil.MakeFullName(rootName) : rootName;
			if (!d_logFit.Open(name, 9))
				return;

			d_logFit.SetField(0, "SessTime");
			d_logFit.SetField(1, "OsTime");
			d_logFit.SetField(2, "EstOsTime");
			d_logFit.SetField(3, "Error");
			d_logFit.SetField(4, "Slope");
			d_logFit.SetField(5, "Incpt");
			d_logFit.SetField(6, "OsVar");
			d_logFit.SetField(7, "SlopeVar");
			d_logFit.SetField(8, "Covar");

			d_logFit.Print();
		}

		if (!d_logAdjust.IsOpen())
		{
			s_logCount = lastCount + 1;
			char rootName[20];
			sprintf(rootName, "TimeUpdate%d.txt", s_logCount);
			const char* name = g_logUtil.Enabled() ?
					g_logUtil.MakeFullName(rootName) : rootName;
			if (!d_logAdjust.Open(name, 14))
				return;

			d_logAdjust.SetField(0, "OsTime");
			d_logAdjust.SetField(1, "OsEnd");
			d_logAdjust.SetField(2, "SessionEnd");
			d_logAdjust.SetField(3, "GameEnd");
			d_logAdjust.SetField(4, "CompGameEnd");
			d_logAdjust.SetField(5, "InitOsSlope");
			d_logAdjust.SetField(6, "InitOsIncpt");
			d_logAdjust.SetField(7, "InitGameSlope");
			d_logAdjust.SetField(8, "InitGameIncpt");
			d_logAdjust.SetField(9, "GamePerOs");
			d_logAdjust.SetField(10, "FinalOsSlope");
			d_logAdjust.SetField(11, "FinalOsIncpt");
			d_logAdjust.SetField(12, "FinalGameSlope");
			d_logAdjust.SetField(13, "FinalGameIncpt");

			d_logAdjust.Print();
		}
	}
	else
	{
		d_logFit.Reclose();
		d_logAdjust.Reclose();
	}
#endif
}

#ifdef DEBUG_SESSION_TIME

/******************************************************************************/
/******************************************************************************/
void CSessionTime::LogFit(float i_osTime, float i_sessionTime)
{
	if (d_logFit.IsOpen())
	{
		float osVar, slopeVar, covar;
		d_fitFilter.GetErrorCovariance(osVar, slopeVar, covar);

		d_logFit.SetField(0, i_sessionTime);
		d_logFit.SetField(1, i_osTime);
		d_logFit.SetField(2, d_fitFilter.LastValue());
		d_logFit.SetField(3, i_osTime - d_fitFilter.LastValue());
		d_logFit.SetField(4, d_currOsPerSession);
		d_logFit.SetField(5, d_currOsVsSessionIntercept);
		d_logFit.SetField(6, osVar);
		d_logFit.SetField(7, slopeVar);
		d_logFit.SetField(8, covar);

		d_logFit.Print();
	}
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::LogAdjustStart(float i_time, int32 i_sessionEnd,
		float i_computedGameEnd)
{
	if (d_logAdjust.IsOpen())
	{
		d_logAdjust.SetField(0, i_time);
		d_logAdjust.SetField(1, (int)g_timer.GetOSEndMS());
		d_logAdjust.SetField(2, i_sessionEnd);
		d_logAdjust.SetField(3, g_timer.GetEndSec());
		d_logAdjust.SetField(4, i_computedGameEnd);
		d_logAdjust.SetField(5, d_osPerSession);
		d_logAdjust.SetField(6, d_osVsSessionIntercept);
		d_logAdjust.SetField(7, d_gamePerSession);
		d_logAdjust.SetField(8, d_gameVsSessionIntercept);
	}
}

/******************************************************************************/
/******************************************************************************/
void CSessionTime::LogAdjustEnd(float i_gamePerOs)
{
	if (d_logAdjust.IsOpen())
	{
		d_logAdjust.SetField(9, i_gamePerOs);
		d_logAdjust.SetField(10, d_osPerSession);
		d_logAdjust.SetField(11, d_osVsSessionIntercept);
		d_logAdjust.SetField(12, d_gamePerSession);
		d_logAdjust.SetField(13, d_gameVsSessionIntercept);

		d_logAdjust.Print();
	}
}

#endif // DEBUG_SESSION_TIME
