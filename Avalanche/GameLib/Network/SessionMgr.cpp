/*
**
**  File:   SessionMgr.h
**  Date:   October 17, 2004
**  By:     Bryant Collard
**  Desc:   Manages session components.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/SessionMgr.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/07/21 15:36:14 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/SessionMgr.h"

// Gamelib includes
#include "Network/Session.h"
#include "Environ/World.h"

/******************************************************************************/
/******************************************************************************/
CSessionMgr::CSessionMgr(void)
{
	d_initialized = false;
	d_session = NULL;
	d_world = NULL;
}

/******************************************************************************/
/******************************************************************************/
CSessionMgr::~CSessionMgr(void)
{
	Close();
}

/******************************************************************************/
/******************************************************************************/
bool CSessionMgr::Open(bool i_lanSession)
{
	if (!d_initialized)
	{
		if (ValidSingleton(CSession))
			g_session.SetLanSession(i_lanSession);
		if (!CreateSessionSingletons())
			return(false);
		d_initialized = true;
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CSessionMgr::Update(void)
{
	if (d_initialized)
		UpdateSessionSingletons();
}

/******************************************************************************/
/******************************************************************************/
void CSessionMgr::Close(void)
{
	if (d_initialized)
	{
		DestroySessionSingletons();
		d_initialized = false;
	}
}

/******************************************************************************/
/******************************************************************************/
bool CSessionMgr::CreateSessionSingletons(void)
{
	if (!ValidSingleton(CSession))
	{
		d_session = new CSession;
		SetSingleton(CSession, d_session);
	}

	if (!g_session.Open())
	{
		DeleteSingleton(CSession);
		d_session = NULL;
		return(false);
	}

	if (!ValidSingleton(CWorld))
	{
		d_world = new CStandAloneWorld;
		SetSingleton(CWorld, d_world);
	}
	g_world.Initialize();

	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CSessionMgr::UpdateSessionSingletons(void)
{
	g_session.Update();
	g_world.Update();
}

/******************************************************************************/
/******************************************************************************/
void CSessionMgr::DestroySessionSingletons(void)
{
	g_session.Close();
	DeleteSingleton(CSession);
	d_session = NULL;

	delete d_world;
	d_world = NULL;
	ClearSingleton(CWorld);
}
