/*
**
**  File:   PlayerProfile.cpp
**  Date:   October 9, 2004
**  By:     Bryant Collard
**  Desc:   Represent the player.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "main/PlayerProfile.h"

// GameLib headers
#include "GameHelper/LogUtil.h"

// TTL headers
#include "main/PlayerAgent.h"
#include "main/PlayerDB.h"
#include "frontend/savegame.h"

CPlayerProfile g_playerProfile;

static byte l_accountId[ACCOUNT_ID_SIZE];

/******************************************************************************/
/******************************************************************************/
CPlayerProfile::CPlayerProfile()
{
	// Agent
	d_mode = INACTIVE;
	d_lobbyStatus = 0;
	d_requestedTeam = INVALID_TEAM;
	d_ready = false;

	// Description
	//d_accountName[0] = '\0';
	//d_accountName[MAX_PROFILE_NAME_LENGTH - 1] = '\0';
	SetAccountName( "Playa" );
	for (int i = 0; i < NUM_VARIANTS; i++)
	{
		d_character[i] = 0;
		d_weaponLoad[i][0] = '\0';
		d_weaponLoad[i][MAX_WEAPON_PROFILE_LENGTH - 1] = '\0';
	}
	d_rating = -1;
	d_rank = -1;
	d_messageOwner.SetOwnerName("PlayerProfile");

	// Clear the account ID stub (it never changes).
	memset(l_accountId, 0, ACCOUNT_ID_SIZE);
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetId(uint8 i_id)
{
	if (d_id != i_id)
	{
		ASSERT((d_id == INVALID_PARTICIPANT) || (i_id == INVALID_PARTICIPANT));
		d_id = i_id;
	}
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::RequestTeam(uint8 i_team)
{
	d_requestedTeam = i_team;
	if (ValidSingleton(Referee))
		g_referee.RequestTeam(d_id, i_team);
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetAccountName(const char* i_name)
{
	const int n = MAX_PROFILE_NAME_LENGTH-1;
	strncpy(d_accountName, i_name, n);
	d_accountName[n] = 0;
	g_logUtil.SetPrefix(d_accountName);
	d_messageOwner.SetOwnerName("PlayerProfile", d_accountName);
}


/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetDisplayName(const char* i_name)
{
	const int n = MAX_PROFILE_DISPLAY_NAME_LENGTH-1;
	strncpy(d_displayName, i_name, n);
	d_displayName[n] = 0;
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetCharacter(EVariant i_variant, uint i_character)
{
	ASSERT((i_variant != INVALID) && (i_variant != NUM_VARIANTS));
	if ((i_variant != INVALID) && (i_variant != NUM_VARIANTS))
	{
		d_character[i_variant] = i_character;
		g_messageDispatcher.SendMessageToAll("PlayerChanged", (void*)d_id,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetCurrentCharacter(uint i_character)
{
	EVariant variant = g_referee.GetTeamingModel().GetPlayerProfileVariant(d_id);
	ASSERT((variant != INVALID) && (variant != NUM_VARIANTS));
	if ((variant != INVALID) && (variant != NUM_VARIANTS))
	{
		d_character[variant] = i_character;
		g_messageDispatcher.SendMessageToAll("PlayerChanged", (void*)d_id,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetAllCharacters(uint i_character)
{
	for (int i = 0; i < NUM_VARIANTS; i++)
		d_character[i] = i_character;
	g_messageDispatcher.SendMessageToAll("PlayerChanged", (void*)d_id,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetWeaponLoadout(EVariant i_variant, const char* i_load)
{
	ASSERT((i_variant != INVALID) && (i_variant < NUM_VARIANTS));
	if ((i_variant != INVALID) && (i_variant < NUM_VARIANTS))
	{
		strncpy(d_weaponLoad[i_variant], i_load, MAX_WEAPON_PROFILE_LENGTH - 1);
		g_messageDispatcher.SendMessageToAll("PlayerChanged", (void*)d_id,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetAllWeaponLoadouts(const char* i_load)
{
	for (int i = 0; i < NUM_VARIANTS; i++)
		strncpy(d_weaponLoad[i], i_load, MAX_WEAPON_PROFILE_LENGTH - 1);
	g_messageDispatcher.SendMessageToAll("PlayerChanged", (void*)d_id,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
const char* CPlayerProfile::GetDisplayName(void)
{
	if( strlen(d_displayName) == 0 )
		return GetAccountName();

	return d_displayName;
}

/******************************************************************************/
/******************************************************************************/
byte* CPlayerProfile::GetAccountId(void)
{
	return(l_accountId);
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::GetTagParams(ts_TagParams &o_tagParams)
{
	g_GlobalProfile.GetTagParams(o_tagParams);
}

/******************************************************************************/
/******************************************************************************/
const char* CPlayerProfile::GetCurrentCharacter(void)
{
	EVariant variant = g_referee.GetTeamingModel().GetPlayerProfileVariant(d_id);
	ASSERT(variant != NUM_VARIANTS);
	if (variant == INVALID)
		return(PlayerDB::CharacterInitString(d_character[0]));
	if (variant != NUM_VARIANTS)
		return(PlayerDB::CharacterInitString(d_character[variant]));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
const char* CPlayerProfile::GetCurrentWeaponLoadout(void)
{
	EVariant variant = g_referee.GetTeamingModel().GetPlayerProfileVariant(d_id);
	ASSERT(variant != NUM_VARIANTS);
	if (variant == INVALID)
		return(d_weaponLoad[0]);
	if (variant != NUM_VARIANTS)
		return(d_weaponLoad[variant]);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
uint CPlayerProfile::GetCharacter(EVariant i_variant)
{
	if ((i_variant != INVALID) && (i_variant != NUM_VARIANTS))
		return(d_character[i_variant]);

	ASSERT(false);
	return(0);
}

/******************************************************************************/
/******************************************************************************/
const char* CPlayerProfile::GetWeaponLoadout(EVariant i_variant)
{
	if ((i_variant != INVALID) && (i_variant != NUM_VARIANTS))
		return(d_weaponLoad[i_variant]);

	ASSERT(false);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::ClearLevel(void)
{
	if (ValidSingleton(CWorld))
		g_world.ClearExtents();
}

/******************************************************************************/
/******************************************************************************/
void CPlayerProfile::SetLevel(uint32 i_crc, int32 i_distributedActorCount,
		Vector3CRef i_min, Vector3CRef i_max, float i_buffer)
{
	if (ValidSingleton(CWorld))
		g_world.SetExtents(i_min, i_max, i_buffer);
}
