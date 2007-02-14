/*
**
**  File:   TTLSessionMgr.cpp
**  Date:   October 17, 2004
**  By:     Bryant Collard
**  Desc:   TTL specific session management.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/TTLSessionMgr.cpp $
**  $Revision: #9 $
**  $DateTime: 2005/07/21 15:36:14 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "main/TTLSessionMgr.h"

// TTL headers
#include "sim/Referee.h"
#include "environ/ConfigMgr.h"

#define INVALIDATE_DELAY  500

/******************************************************************************/
/******************************************************************************/
bool CTTLSessionMgr::Open(bool i_lanSession)
{
	if (CSessionMgr::Open(i_lanSession))
	{
		g_session.SetConfig(g_configmgr.GetOptions());
		d_changeConfigHandler.Initialize("GameOptionsChanged",
				MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
		return(true);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void CTTLSessionMgr::Close(void)
{
	CSessionMgr::Close();
	d_messageOwner.UnregisterAll();
}

/******************************************************************************/
/******************************************************************************/
void CTTLSessionMgr::CChangeConfig::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	g_session.SetConfig(g_configmgr.GetOptions());
}

/******************************************************************************/
/******************************************************************************/
bool CTTLSessionMgr::CreateSessionSingletons(void)
{
	if (CSessionMgr::CreateSessionSingletons())
	{
		// Make sure there is a referee singleton.
		if (!ValidSingleton(Referee))
		{
			d_referee = new StandAloneReferee;
			SetSingleton(Referee, d_referee);
		}
#ifndef CONSUMER_BUILD
		d_errorHandle = ERROR_HANDLE_INVALID;
		d_invalidateTime = MAXUINT;
#endif

		return(true);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void CTTLSessionMgr::UpdateSessionSingletons(void)
{
	CSessionMgr::UpdateSessionSingletons();

	// Update the referee
	g_referee.Update();

#ifndef CONSUMER_BUILD
	// Make sure the session is valid.
	const char* validateMsg = g_referee.Validate();
	if (validateMsg != NULL)
	{
		uint32 currTime = g_timer.GetOSTime();
		if (d_invalidateTime == MAXUINT)
			d_invalidateTime = currTime + INVALIDATE_DELAY;

		if ((currTime >= d_invalidateTime) &&
				(d_errorHandle == ERROR_HANDLE_INVALID))
		{
			d_errorHandle = g_errorStack.SetError("Session Inconsistent",
					validateMsg, CErrorStack::ERROR_WARNING, ERROR_INFINITE_LIFETIME);
		}
	}
	else if ((d_invalidateTime != MAXUINT) ||
			(d_errorHandle != ERROR_HANDLE_INVALID))
	{
		g_errorStack.ClearError(d_errorHandle);
		d_errorHandle = ERROR_HANDLE_INVALID;
		d_invalidateTime = MAXUINT;
	}
#endif
}

/******************************************************************************/
/******************************************************************************/
void CTTLSessionMgr::DestroySessionSingletons(void)
{
	// Clean up the referee
	delete d_referee;
	d_referee = NULL;
	ClearSingleton(Referee);
#ifndef CONSUMER_BUILD
	g_errorStack.ClearError(d_errorHandle);
#endif

	CSessionMgr::DestroySessionSingletons();
}
