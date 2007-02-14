/*
**
**  File:   PlayerAgent.cpp
**  Date:   October 4, 2004
**  By:     Bryant Collard
**  Desc:   Store information about a player.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/PlayerAgent.cpp $
**  $Revision: #11 $
**  $DateTime: 2005/08/29 19:20:27 $
**    $Author: Dan_Silver $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "main/PlayerAgent.h"

// TTL includes.
#include "main/PlayerProfile.h"
#include "main/TTLConstants.h"
#include "sim/Referee.h"

/******************************************************************************/
/******************************************************************************/
CPlayerAgent::CPlayerAgent()
{
	d_id = INVALID_PARTICIPANT;
}

/******************************************************************************/
/******************************************************************************/
CPlayerAgent::~CPlayerAgent()
{
	if (ValidSingleton(Referee))
		g_referee.RemoveParticipant(this);
}

/******************************************************************************/
/******************************************************************************/
void CPlayerAgent::Initialize(void)
{
	d_toggleReadyHandler.Initialize("PlayerReadyUpToggle",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_clearReadyHandler.Initialize("PlayerReadyUpClear",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_forceReadyHandler.Initialize("PlayerForceReadyUp",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
}

/******************************************************************************/
/******************************************************************************/
void CPlayerAgent::Terminate(void)
{
	d_messageOwner.UnregisterAll();
}

/******************************************************************************/
/******************************************************************************/
void CPlayerAgent::CToggleReady::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if ((d_ref->d_id != INVALID_PARTICIPANT) &&
			(g_referee.GetActor(d_ref->d_id) == NULL))
	{
		if (g_referee.IsReady(d_ref->d_id))
		{
			d_ref->SetReady(false);
			g_referee.ResetReadyPhaseExit();
		}
		else
			d_ref->SetReady(true);
	}
}

/******************************************************************************/
/******************************************************************************/
void CPlayerAgent::CClearReady::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if ((d_ref->d_id != INVALID_PARTICIPANT) &&
			(g_referee.GetActor(d_ref->d_id) == NULL))
		d_ref->SetReady(false);
}

/******************************************************************************/
/******************************************************************************/
void CPlayerAgent::CForceReady::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if ((d_ref->d_id != INVALID_PARTICIPANT) &&
			(g_referee.GetActor(d_ref->d_id) == NULL))
	{
		d_ref->SetReady(true);
		g_referee.ForceReadyPhaseExit();
	}
}

/******************************************************************************/
/******************************************************************************/
CPlayerAgent::EAssignment CPlayerAgent::GetAssignment(void)
{
	// Still need an id.
	if (d_id == INVALID_PARTICIPANT)
	{
		// See if participation is closed.
		if (!ValidSingleton(Referee) || g_referee.IsParticipationClosed(true))
			return(DENIED);

		// Still waiting.
		return(WAITING);
	}

	return(ASSIGNED);
}

/******************************************************************************/
/******************************************************************************/
int32 CPlayerAgent::GetStarRating(void)
{
	int32 rating = GetRating();
	int32 stars = 0;

	if( rating > 1600 )
		stars++;

	if( rating >= 1700 )
		stars++;

	if( rating >= 1800 )
		stars++;

	if( rating >= 2000 )
		stars++;

	if( rating >= 2200 )
		stars++;

	return stars;
}
