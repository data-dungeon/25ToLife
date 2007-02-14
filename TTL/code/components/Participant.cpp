/*
**
**  File:   Participant.cpp
**  Date:   April 16, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the participant interface.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/Participant.cpp $
**  $Revision: #15 $
**  $DateTime: 2005/08/15 13:40:59 $
**    $Author: Food $
**
*/

#include "TTLPCH.h"
#include "components/Participant.h"
#include "Components/ComponentCreator.h"
#include "main/TTLConstants.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CParticipant, Participant,
		COMPONENT_NEUTRAL_PRIORITY);

//============================================================================
// Construct
CParticipant::CParticipant( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
	d_participantId = INVALID_PARTICIPANT;
	d_requestedTeam = INVALID_TEAM;
	d_alive = false;
	d_spawnTicketIssued = false;
	d_surfaceParam = NULL;
	d_lastAlive = true;
	d_deathEnd = -MAXFLOAT;
}

//============================================================================
CParticipant::~CParticipant()
{
	if (ValidSingleton(Referee))
		g_referee.RemoveParticipant(&actor);
}

//============================================================================
void CParticipant::Initialize(void)
{
	// Set the participant category bit for anyone with this component.
	actor.Category() |= PARTICIPANT_CATEGORY;

	// Get the surface parameters.
	d_surfaceParam =
			CParam<CParticipant::SSurfaceParam>::GetParam("HitValues", "var");
	ASSERT_PTR(d_surfaceParam);

	// Delayed death.
	d_lastAlive = true;
	d_deathEnd = -MAXFLOAT;
	d_manDownHandler.Initialize("ManDown", MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_DUPLICAS,
			actor.GetMessageOwner(), this);
	d_deathEndedHandler.Initialize("DeathEnded", MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_HANDLE_IF_DUPLICA, actor.GetMessageOwner(), this);
}

//============================================================================
uint8 CParticipant::GetTeamId(void)
{
	return(g_referee.GetTeam(GetParticipantId()));
}

//============================================================================
void CParticipant::LocalMakeAvatar(uint8 i_id)
{
	d_participantId = i_id;
	d_requestedTeam = g_referee.GetTeam(i_id);
	d_alive = true;
}

//============================================================================
void CParticipant::SetLocalParticipantId(uint8 i_id)
{
	d_participantId = i_id;
}

//============================================================================
void CParticipant::LocalRequestTeam(uint8 i_team)
{
	d_requestedTeam = i_team;
	g_referee.RequestTeam(d_participantId, d_requestedTeam);
}

//============================================================================
void CParticipant::SetLocalAlive(bool i_alive)
{
	d_alive = i_alive;
	g_referee.SetActive(d_participantId, d_alive);
}

//============================================================================
void CParticipant::SetLocalSpawnTicket(bool i_ticketIssued)
{
	d_spawnTicketIssued = i_ticketIssued;
}

//============================================================================
void CParam<CParticipant::SSurfaceParam>::RegisterVariables(void)
{
	char title[64];
	for (int i = 0; i < PARTICIPANT_PARAM_SIZE; i++)
	{
		sprintf(title, "SurfaceName_%d", i);
		RegisterVariable(d_data.d_surfaceName[i], title, "",
				PARTICIPANT_NAME_LENGTH, (void*)0);

		sprintf(title, "DamageMultiplier_%d", i);
		RegisterVariable(d_data.d_damageMultiplier[i], title, 1.0f, 0.0f, 5.0f);

		int index = g_surfaceType.GetIndex(d_data.d_surfaceName[i],
				PARTICIPANT_NAME_LENGTH);
		if ((index >= 0) && (index <= INVALID_MATERIALINDEX))
			d_data.d_surfaceType[i] = (uint8)index;
		else
			d_data.d_surfaceType[i] = INVALID_MATERIALINDEX;
	}
}

//============================================================================
void CParam<CParticipant::SSurfaceParam>::CParamMessageHandler::HandleMessage(
		void* i_data, ActorHandle i_sender, bool i_posted)
{
	for (int i = 0; i < PARTICIPANT_PARAM_SIZE; i++)
	{
		int index = g_surfaceType.GetIndex(d_data->d_surfaceName[i],
				PARTICIPANT_NAME_LENGTH);
		if ((index >= 0) && (index <= INVALID_MATERIALINDEX))
			d_data->d_surfaceType[i] = (uint8)index;
		else
			d_data->d_surfaceType[i] = INVALID_MATERIALINDEX;
	}
}

//============================================================================
float CParticipant::GetDamageMultiplier(uint8 i_surfaceType)
{
	for (int i = 0; i < PARTICIPANT_PARAM_SIZE; i++)
	{
		if (i_surfaceType == d_surfaceParam->d_surfaceType[i])
			return(d_surfaceParam->d_damageMultiplier[i]);
	}
	return(1.0f);
}

//============================================================================
char * CParticipant::GetSurfaceName(uint8 i_surfaceType)
{
	for (int i = 0; i < PARTICIPANT_PARAM_SIZE; i++)
	{
		if (i_surfaceType == d_surfaceParam->d_surfaceType[i])
			return(d_surfaceParam->d_surfaceName[i]);
	}
	return(NULL);
}

#define MAX_DEATH_TIME 7.0f

//============================================================================
bool CParticipant::IsAliveOrDying(void)
{
	// Things are simple if we are alive.
	if (GetAlive())
	{
		d_lastAlive = true;
		return(true);
	}

	// We are dead but we already know it.
	if (!d_lastAlive)
		return(g_timer.GetEndSec() <= d_deathEnd);

	// We are newly dead. The only time this should hit is if it is called after
	// GetAlive() starts returning false and the "ManDown" message is received.
	d_lastAlive = false;
	d_deathEnd = g_timer.GetEndSec() + MAX_DEATH_TIME;
	return(true);
}

//============================================================================
void CParticipant::CManDown::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (&d_ref->actor == CActor::FromHandle(i_sender))
	{
		d_ref->d_lastAlive = false;
		d_ref->d_deathEnd = g_timer.GetEndSec() + MAX_DEATH_TIME;
	}
}

//============================================================================
void CParticipant::CDeathEnded::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_deathEnd = -MAXFLOAT;
}
