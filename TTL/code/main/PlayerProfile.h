/*
**
**  File:   PlayerProfile.h
**  Date:   October 9, 2004
**  By:     Bryant Collard
**  Desc:   Represent the player.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#pragma once

#ifndef PLAYER_PROFILE_H
#define PLAYER_PROFILE_H

#include "platform/BaseType.h"
#include "main/PlayerAgent.h"

#define MAX_PROFILE_NAME_LENGTH 16
#define MAX_WEAPON_PROFILE_LENGTH  128
#define MAX_PROFILE_DISPLAY_NAME_LENGTH	(MAX_PROFILE_NAME_LENGTH+8)

class CPlayerProfile : public CPlayerAgent
{
  public:
	// Types
	enum EVariant
	{
		INVALID = -1,
		COP,
		GANG,
		NUM_VARIANTS
	};

	// Construct
	CPlayerProfile();

	// Override CPlayerAgent
	virtual void SetMode(EMode i_mode) {d_mode = i_mode;}
	virtual EMode GetMode(void) {return(d_mode);}
	virtual void SetLobbyStatus(uint8 i_status) {d_lobbyStatus = i_status;}
	virtual uint8 GetLobbyStatus(void) {return(d_lobbyStatus);}
	virtual void SetId(uint8 i_id);
	virtual void RequestTeam(uint8 i_team);
	virtual uint8 GetRequestedTeam(void) {return(d_requestedTeam);}
	virtual void SetReady(bool i_ready) {d_ready = i_ready;}
	virtual bool IsReady(void) {return(d_ready);}

	// Describe player
	void SetAccountName(const char* i_name);
	void SetDisplayName(const char* i_name);
	void SetCharacter(EVariant i_variant, uint i_character);
	void SetCurrentCharacter(uint i_character);
	void SetAllCharacters(uint i_character);
	void SetWeaponLoadout(EVariant i_variant, const char* i_load);
	void SetAllWeaponLoadouts(const char* i_load);

	// Get player description
	virtual const char* GetAccountName(void) {return(d_accountName);}
	virtual const char* GetDisplayName(void);
	virtual byte* GetAccountId(void);
	virtual void GetTagParams(ts_TagParams &o_tagParams);
	virtual const char* GetCurrentCharacter(void);
	virtual const char* GetCurrentWeaponLoadout(void);
	uint GetCharacter(EVariant i_variant);
	uint GetAnyCharacter(void) {return(d_character[0]);}
	const char* GetWeaponLoadout(EVariant i_variant);
	virtual void SetRating(int32 i_rating) {d_rating = i_rating;}
	virtual int32 GetRating(void) {return(d_rating);}
	virtual void SetRank(int32 i_rank) {d_rank = i_rank;}
	virtual int32 GetRank(void) {return(d_rank);}
	virtual bool IsSessionHost(void) {return(true);}

	// Level data
	virtual void ClearLevel(void);
	virtual void SetLevel(uint32 i_crc, int32 i_distributedActorCount,
			Vector3CRef i_min, Vector3CRef i_max, float i_buffer);

  private:
	EMode d_mode;
	uint8 d_lobbyStatus;
	uint8 d_requestedTeam;
	bool d_ready;

	// Profile data
	char d_accountName[MAX_PROFILE_NAME_LENGTH];
	char d_displayName[MAX_PROFILE_DISPLAY_NAME_LENGTH];
	uint d_character[NUM_VARIANTS];
	char d_weaponLoad[NUM_VARIANTS][MAX_WEAPON_PROFILE_LENGTH];
	int32 d_rating;
	int32 d_rank;
};

extern CPlayerProfile g_playerProfile;

#endif // PLAYER_PROFILE_H
