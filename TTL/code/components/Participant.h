/*
**
**  File:   Participant.h
**  Date:   April 16, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the participant interface.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/Participant.h $
**  $Revision: #14 $
**  $DateTime: 2005/08/15 13:40:59 $
**    $Author: Food $
**
*/

#pragma once

#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include "Components/Component.h"
#include "InterfacesTTL/ParticipantIF.h"

#define PARTICIPANT_PARAM_SIZE   6
#define PARTICIPANT_NAME_LENGTH  32

// the referee owns the team id for any given participant
#include "sim/Referee.h"

class CParticipant : public CActorComponent, public CParticipantIF
{
  public:
	CParticipant( CCompActor &i_actor );
	~CParticipant();

	// Override CActorComponent
	virtual const char *ComponentName(void) {return("Participant");}
	virtual CInterface* GetInterface(uint i_id)
		{return((i_id == CParticipantIF::GetID()) ? this : NULL);}
	virtual void Initialize(void);

	// Override CParticipantIF
	virtual uint8 GetTeamId(void);

	// Map surface types to damage, etc. There are a number of ways this could
	// be made more general or flexible with extra effort. Since the extra power
	// may not ever be used, a fairly simple approach is taken and the extra
	// power can be added later if needed.
	virtual float GetDamageMultiplier(uint8 i_surfaceType);
	virtual char *GetSurfaceName(uint8 i_surfaceType);
	struct SSurfaceParam
	{
		uint8 d_surfaceType[PARTICIPANT_PARAM_SIZE];
		char d_surfaceName[PARTICIPANT_PARAM_SIZE][PARTICIPANT_NAME_LENGTH];
		float d_damageMultiplier[PARTICIPANT_PARAM_SIZE];
	};

	// Allow death animation to play before becomnig really dead.
	virtual bool IsAliveOrDying(void);

  protected:
	virtual void LocalMakeAvatar(uint8 i_id);
	virtual void SetLocalParticipantId(uint8 i_id);
	virtual void LocalRequestTeam(uint8 i_team);
	virtual void SetLocalAlive(bool i_alive);
	virtual void SetLocalSpawnTicket(bool i_ticketIssued);
	virtual uint8 GetLocalParticipantId(void) {return(d_participantId);}
	virtual uint8 GetLocalRequestedTeamId(void) {return(d_requestedTeam);}
	virtual bool GetLocalAlive(void) {return(d_alive);}
	virtual bool GetLocalSpawnTicket(void) {return(d_spawnTicketIssued);}

	uint8 d_participantId;
	uint8 d_requestedTeam;
	bool d_alive;
	bool d_spawnTicketIssued;
	SSurfaceParam* d_surfaceParam;

	bool d_lastAlive;
	float d_deathEnd;
	REF_SEND_MESSAGE_HANDLER(CManDown, CParticipant) d_manDownHandler;
	REF_SEND_MESSAGE_HANDLER(CDeathEnded, CParticipant) d_deathEndedHandler;
};

#endif // PARTICIPANT_H
