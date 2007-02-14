/*
**
**  File:   DOPlayerAgent.h
**  Date:   April 29, 2004
**  By:     Shon C. Love
**  Desc:   Implementation of the player agent distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#pragma once

#ifndef DO_PLAYER_AGENT_H
#define DO_PLAYER_AGENT_H

#include <netz.h>
#include "NetZTTL/DOPlayerAgentDDL.h"
#include "NetZTTL/DSParticipant.h"
#include "main/PlayerAgent.h"

class DOPlayerAgent : public DOCLASS(DOPlayerAgent), public CPlayerAgent
{
  public:
	virtual void InitDO();
	virtual void OperationBegin(DOOperation* i_operation);
	virtual void OperationEnd(DOOperation* i_operation);
	virtual void Update(void);
	void ParticipantChanged(void);
	virtual void Initialize(void);
	virtual void SetId(uint8 i_id);
	virtual void SetMode(EMode i_mode);
	virtual EMode GetMode(void);
	virtual void SetLobbyStatus(uint8 i_status);
	virtual uint8 GetLobbyStatus(void);

	// Override DOTalker
	virtual Vector3 GetLocation(void) const;
	virtual Vector3 GetDirection(void) const;
	virtual Vector3 GetVelocity(void) const;
	virtual bool IsAlive(void) const;

	// Set configuration.
	virtual void RequestTeam(uint8 i_team);
	virtual uint8 GetRequestedTeam(void);
	virtual void SetReady(bool i_ready);
	virtual bool IsReady(void);

	// Player data
	virtual const char* GetAccountName(void);
	virtual const char* GetDisplayName(void);
	virtual byte* GetAccountId(void);
	virtual void GetTagParams(ts_TagParams &o_tagParams);
	virtual const char* GetCurrentCharacter(void);
	virtual const char* GetCurrentWeaponLoadout(void);
	virtual void SetRating(int32 i_rating);
	virtual int32 GetRating(void);
	virtual void SetRank(int32 i_rank);
	virtual int32 GetRank(void);
	virtual bool IsSessionHost(void);

	// Level data
	virtual void ClearLevel(void);
	virtual void SetLevel(uint32 i_crc, int32 i_distributedActorCount,
			Vector3CRef i_min, Vector3CRef i_max, float i_buffer);
	uint32 GetCRC(void);
	int32 GetDistributedActorCount(void);
	void SetWorldExtents(void);

  protected:
	// Appearance
	bool UpdateAppearance(void);
	REF_SEND_MESSAGE_HANDLER(CPlayerChange, DOPlayerAgent) d_playerChangeHandler;
	REF_SEND_MESSAGE_HANDLER(CAssignmentChange, DOPlayerAgent)
			d_assignmentChangeHandler;
	REF_SEND_MESSAGE_HANDLER(CTeamRoleChange, DOPlayerAgent)
			d_teamRoleChangeHandler;
};

#endif // DO_PLAYER_AGENT_H
