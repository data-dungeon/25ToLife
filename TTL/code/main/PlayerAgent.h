/*
**
**  File:   PlayerAgent.h
**  Date:   October 4, 2004
**  By:     Bryant Collard
**  Desc:   Store information about a player.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#pragma once

#ifndef PLAYER_AGENT_H
#define PLAYER_AGENT_H

#include "platform/BaseType.h"

class CPlayerProfile;
struct ts_TagParams;

class CPlayerAgent
{
  public:
	virtual ~CPlayerAgent();

	virtual void Initialize(void);
	void Terminate(void);

	// Operational mode of the player agent.
	enum EMode
	{
		INACTIVE,
		UNLOADED,
		LOADED,
		PUBLISHED
	};
	virtual void SetMode(EMode i_mode) = 0;
	virtual EMode GetMode(void) = 0;

	// Status in lobby just to let other players know what you are doing.
	virtual void SetLobbyStatus(uint8 i_status) = 0;
	virtual uint8 GetLobbyStatus(void) = 0;

	// Get the id assignment from the referee.
	enum EAssignment
	{
		WAITING,
		ASSIGNED,
		DENIED
	};
	EAssignment GetAssignment(void);
	uint8 Id(void) {return(d_id);}
	virtual void SetId(uint8 i_id) = 0;

	// Request a new team.
	virtual void RequestTeam(uint8 i_team) = 0;
	virtual uint8 GetRequestedTeam(void) = 0;

	// Ready status.
	virtual void SetReady(bool i_ready) = 0;
	virtual bool IsReady(void) = 0;

	// Player data
	virtual const char* GetAccountName(void) = 0;
	virtual const char* GetDisplayName(void) = 0;
	virtual byte* GetAccountId(void) = 0;
	virtual void GetTagParams(ts_TagParams &o_tagParams) = 0;
	virtual const char* GetCurrentCharacter(void) = 0;
	virtual const char* GetCurrentWeaponLoadout(void) = 0;
	virtual void SetRating(int32 i_rating) = 0;
	virtual int32 GetRating(void) = 0;
	virtual void SetRank(int32 i_rank) = 0;
	virtual int32 GetRank(void) = 0;
	virtual int32 GetStarRating(void);
	virtual bool IsSessionHost(void) = 0;

	// Level data
	virtual void ClearLevel(void) = 0;
	virtual void SetLevel(uint32 i_crc, int32 i_distributedActorCount,
			Vector3CRef i_min, Vector3CRef i_max, float i_buffer) = 0;

  protected:
	CPlayerAgent();

	uint8 d_id;

	// Messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CToggleReady, CPlayerAgent) d_toggleReadyHandler;
	REF_SEND_MESSAGE_HANDLER(CClearReady, CPlayerAgent) d_clearReadyHandler;
	REF_SEND_MESSAGE_HANDLER(CForceReady, CPlayerAgent) d_forceReadyHandler;
};

#endif // PLAYER_AGENT_H
