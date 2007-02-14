/*
**
**  File:   DOPlayerAgent.cpp
**  Date:   April 29, 2004
**  By:     Shon C. Love
**  Desc:   Implementation of the player distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DOPlayerAgent.h"

// GameLib headers
#include "Voice/NetZ/CVoiceChat.h"
#include "GameHelper/LogUtil.h"

// TTL headers
#include "main/Lobby.h"
#include "main/PlayerProfile.h"
#include "effects/customtag.h"

#define ENABLE_VOICE_CHAT

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::Initialize(void)
{
	d_messageOwner.SetOwnerName("LocalAgent", g_playerProfile.GetAccountName());
	CPlayerAgent::Initialize();
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::InitDO()
{
	if (IsADuplicationMaster())
	{
		ts_TagParams tagParams;
		g_playerProfile.GetTagParams(tagParams);

		d_participant.SetIsMaster(true);
		g_lobby.GetLocalAccountId(d_profile.d_accountId);
		d_profile.d_accountName = g_playerProfile.GetAccountName();
		d_profile.d_displayName = g_playerProfile.GetDisplayName();
		ASSERT(sizeof(ts_TagParams) <= TAG_PARAMS_SIZE);
		memcpy(d_profile.d_tagParams, &tagParams, sizeof(ts_TagParams));
		d_agent.d_mode = INACTIVE;
		d_agent.d_lobbyStatus = 0;
#ifdef ENABLE_VOICE_CHAT
		g_CVoiceChat.getVoiceMaskParms(&d_props.d_pitchWarp, &d_props.d_pitchOffset, &d_props.d_spectrumWarp,
												 &d_props.d_spectrumOffset, &d_props.d_spectrumLedge, &d_props.d_robot,
												 &d_props.d_whisper, &d_props.d_environ );
		d_props.d_headset = false;
		d_props.d_voiceToSpeakers = false;
		d_props.d_id = GetHandle();
		d_props.d_userFlags = 0;
#endif
		d_appearance.d_character = g_playerProfile.GetCurrentCharacter();
		d_appearance.d_loadout = g_playerProfile.GetCurrentWeaponLoadout();
		d_stats.d_rating = g_playerProfile.GetRating();
		d_stats.d_rank = g_playerProfile.GetRank();
		d_playerChangeHandler.Initialize("PlayerChanged",
				MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
		d_assignmentChangeHandler.Initialize("RefereeAssignmentChanged",
				MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
		d_teamRoleChangeHandler.Initialize("TeamRoleChanged",
				MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
		d_level.d_crc = 0;
		d_level.d_distributedActorCount = 0;
		d_level.d_centerX = 0.0f;
		d_level.d_centerY = 0.0f;
		d_level.d_centerZ = 0.0f;
		d_level.d_extentX = 0.0f;
		d_level.d_extentY = 0.0f;
		d_level.d_extentZ = 0.0f;
	}
	else
	{
		d_messageOwner.SetOwnerName("RemoteAgent", d_profile.d_accountName.CStr());
		d_participant.SetIsMaster(false);
	}
	ParticipantChanged();
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::CPlayerChange::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if ((uint8)i_data == d_ref->d_id)
		d_ref->UpdateAppearance();
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::CAssignmentChange::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if (d_ref->UpdateAppearance())
	{
		g_messageDispatcher.SendMessageToAll("PlayerChanged",
				(void*)(d_ref->d_id), INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::CTeamRoleChange::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if (d_ref->UpdateAppearance())
	{
		g_messageDispatcher.SendMessageToAll("PlayerChanged",
				(void*)(d_ref->d_id), INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::OperationBegin(DOOperation* i_operation)
{
#ifdef ENABLE_VOICE_CHAT
	if (IsADuplicationMaster() &&
			(i_operation->GetType() == Operation::RemoveFromStore))
	{
		g_CVoiceChat.enable(false);
		g_CVoiceChat.tearDown();
	}
#endif
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::OperationEnd(DOOperation* i_operation)
{
#ifdef ENABLE_VOICE_CHAT
	if (IsADuplicationMaster() &&
			(i_operation->GetType() == Operation::AddToStore) &&
			g_CVoiceChat.create(this, g_session))
	{
//		g_CVoiceChat.enable(false);
//		g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, true);
//		g_CVoiceChat.enable(true);
	}
#endif
	switch(i_operation->GetType())
	{
	 case Operation::AddToStore:
		g_debugOutput.TSPrint(g_debugOutput.Category("AGT"),
				"Add %s agent %s (0x%x)", IsADuplicationMaster() ?
				"MASTER" : "DUPLICA", GetAccountName(), GetHandleValue());
		break;
	 case Operation::RemoveFromStore:
		g_debugOutput.TSPrint(g_debugOutput.Category("AGT"),
				"Remove %s agent %s (0x%x)", IsADuplicationMaster() ?
				"MASTER" : "DUPLICA", GetAccountName(), GetHandleValue());
		break;
	}
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::Update(void)
{
	if (IsADuplicationMaster())
	{
#ifdef ENABLE_VOICE_CHAT
		g_CVoiceChat.update();
#endif
		DuplicatedObject::Update();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::ParticipantChanged(void)
{
	if (d_id != d_participant.GetId())
	{
		ASSERT((d_id == INVALID_PARTICIPANT) ||
				(d_participant.GetId() == INVALID_PARTICIPANT));
		d_id = d_participant.GetId();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::SetId(uint8 i_id)
{
	if (IsADuplicationMaster() && (i_id != d_participant.GetId()))
	{
		NParticipantConfig::TConfig config;
		NParticipantConfig::SetAssignedTeam(config, g_referee.GetTeam(i_id));
		NParticipantConfig::SetGhost(config, g_referee.IsGhost(i_id));
		NParticipantConfig::SetRequestedTeam(config, g_referee.GetTeam(i_id));
		NParticipantConfig::SetActive(config, false);
		d_participant.Initialize(i_id, config, false);
		ParticipantChanged();

		g_playerProfile.SetId(d_participant.GetId());
		UpdateAppearance();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::SetMode(EMode i_mode)
{
	if (IsADuplicationMaster() && ((EMode)d_agent.d_mode != i_mode))
	{
		d_agent.d_mode = i_mode;
		d_agent.RequestUpdate();

		SLogMessage logMessage;
		logMessage.d_type = SLogMessage::LOG_ACTION;
		logMessage.d_label = "Agent";
		switch(i_mode)
		{
		 case INACTIVE:
			g_debugOutput.TSPrint("Player mode = INACTIVE");
			logMessage.d_action = "Inactive";
			break;
		 case UNLOADED:
			g_debugOutput.TSPrint("Player mode = UNLOADED");
			logMessage.d_action = "Unloaded";
			break;
		 case LOADED:
			g_debugOutput.TSPrint("Player mode = LOADED");
			logMessage.d_action = "Loaded";
			break;
		 case PUBLISHED:
			g_debugOutput.TSPrint("Player mode = PUBLISHED");
			logMessage.d_action = "Published";
			break;
		}
		g_messageDispatcher.SendMessageToAll("LogEntry", &logMessage,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
CPlayerAgent::EMode DOPlayerAgent::GetMode(void)
{
	return((EMode)d_agent.d_mode);
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::SetLobbyStatus(uint8 i_status)
{
	if (IsADuplicationMaster() && (d_agent.d_lobbyStatus != i_status))
	{
		d_agent.d_lobbyStatus = i_status;
		d_agent.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
uint8 DOPlayerAgent::GetLobbyStatus(void)
{
	return(d_agent.d_lobbyStatus);
}

/******************************************************************************/
/******************************************************************************/
Vector3 DOPlayerAgent::GetLocation(void) const
{
	CActor* avatar = g_referee.GetActor(d_participant.GetId());
	if (avatar != NULL)
		return(avatar->GetBodyInWorld());
	return(Vector3(0.0f, 0.0f, 0.0f));
}

/******************************************************************************/
/******************************************************************************/
Vector3 DOPlayerAgent::GetDirection(void) const
{
	CActor* avatar = g_referee.GetActor(d_participant.GetId());
	if (avatar != NULL)
		return(avatar->GetBodyToWorld().GetRow2());
	return(Vector3(0.0f, 0.0f, 1.0f));
}

/******************************************************************************/
/******************************************************************************/
Vector3 DOPlayerAgent::GetVelocity(void) const
{
	CActor* avatar = g_referee.GetActor(d_participant.GetId());
	if (avatar != NULL)
		return(avatar->GetVelocityInWorld());
	return(Vector3(0.0f, 0.0f, 0.0f));
}

/******************************************************************************/
/******************************************************************************/
bool DOPlayerAgent::IsAlive(void) const
{
	return(g_referee.IsAlive(d_participant.GetId()));
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::RequestTeam(uint8 i_team)
{
	d_participant.RequestTeam(i_team);
}

/******************************************************************************/
/******************************************************************************/
uint8 DOPlayerAgent::GetRequestedTeam(void)
{
	return(NParticipantConfig::RequestedTeam(d_participant.d_config));
}

/******************************************************************************/
/******************************************************************************/
bool DOPlayerAgent::IsReady(void)
{
	return(NParticipantConfig::Active(d_participant.d_config));
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::SetReady(bool i_ready)
{
	if (IsADuplicationMaster())
		d_participant.SetActive(i_ready);
}

/******************************************************************************/
/******************************************************************************/
const char* DOPlayerAgent::GetAccountName(void)
{
	return(d_profile.d_accountName.CStr());
}

/******************************************************************************/
/******************************************************************************/
const char* DOPlayerAgent::GetDisplayName(void)
{
	return(d_profile.d_displayName.CStr());
}

/******************************************************************************/
/******************************************************************************/
byte* DOPlayerAgent::GetAccountId(void)
{
	return(d_profile.d_accountId);
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::GetTagParams(ts_TagParams &o_tagParams)
{
	ASSERT(sizeof(ts_TagParams) <= TAG_PARAMS_SIZE);
	memcpy(&o_tagParams, d_profile.d_tagParams, sizeof(ts_TagParams));
}

/******************************************************************************/
/******************************************************************************/
const char* DOPlayerAgent::GetCurrentCharacter(void)
{
	// Get directly from the player profile if this is the master just in case
	// changes to the profile have not been propogated to the DOPlayerAgent yet.
	if (IsADuplicationMaster())
		return(g_playerProfile.GetCurrentCharacter());
	return(d_appearance.d_character.CStr());
}

/******************************************************************************/
/******************************************************************************/
const char* DOPlayerAgent::GetCurrentWeaponLoadout(void)
{
	// Get directly from the player profile if this is the master just in case
	// changes to the profile have not been propogated to the DOPlayerAgent yet.
	if (IsADuplicationMaster())
		return(g_playerProfile.GetCurrentWeaponLoadout());
	return(d_appearance.d_loadout.CStr());
}

/******************************************************************************/
/******************************************************************************/
bool DOPlayerAgent::UpdateAppearance(void)
{
	if ((d_appearance.d_character != g_playerProfile.GetCurrentCharacter()) ||
			(d_appearance.d_loadout != g_playerProfile.GetCurrentWeaponLoadout()))
	{
		d_appearance.d_character = g_playerProfile.GetCurrentCharacter();
		d_appearance.d_loadout = g_playerProfile.GetCurrentWeaponLoadout();
		d_appearance.RequestUpdate();
		return(true);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::SetRating(int32 i_rating)
{
	if (IsADuplicationMaster() && (i_rating != d_stats.d_rating))
	{
		g_playerProfile.SetRating(i_rating);
		d_stats.d_rating = i_rating;
		d_stats.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
int32 DOPlayerAgent::GetRating(void)
{
	return(d_stats.d_rating);
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::SetRank(int32 i_rank)
{
	if (IsADuplicationMaster() && (i_rank != d_stats.d_rank))
	{
		g_playerProfile.SetRank(i_rank);
		d_stats.d_rank = i_rank;
		d_stats.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
int32 DOPlayerAgent::GetRank(void)
{
	return(d_stats.d_rank);
}

/******************************************************************************/
/******************************************************************************/
bool DOPlayerAgent::IsSessionHost(void)
{
	return(GetMasterID() == Session::GetInstance()->GetMasterID());
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::ClearLevel(void)
{
	if (IsADuplicationMaster())
	{
		d_level.d_crc = 0;
		d_level.d_distributedActorCount = 0;
		d_level.d_centerX = 0.0f;
		d_level.d_centerY = 0.0f;
		d_level.d_centerZ = 0.0f;
		d_level.d_extentX = 0.0f;
		d_level.d_extentY = 0.0f;
		d_level.d_extentZ = 0.0f;

		d_level.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::SetLevel(uint32 i_crc, int32 i_distributedActorCount,
		Vector3CRef i_min, Vector3CRef i_max, float i_buffer)
{
	if (IsADuplicationMaster())
	{
		Vector3 center;
		Vector3 extent;
		CWorld::ComputeExtents(i_min, i_max, i_buffer, center, extent);

		d_level.d_crc = i_crc;
		d_level.d_distributedActorCount = i_distributedActorCount;
		d_level.d_centerX = center.X();
		d_level.d_centerY = center.Y();
		d_level.d_centerZ = center.Z();
		d_level.d_extentX = extent.X();
		d_level.d_extentY = extent.Y();
		d_level.d_extentZ = extent.Z();

		d_level.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
uint32 DOPlayerAgent::GetCRC(void)
{
	return(d_level.d_crc);
}

/******************************************************************************/
/******************************************************************************/
int32 DOPlayerAgent::GetDistributedActorCount(void)
{
	return(d_level.d_distributedActorCount);
}

/******************************************************************************/
/******************************************************************************/
void DOPlayerAgent::SetWorldExtents(void)
{
	g_world.SetExtents(
			Vector3(d_level.d_centerX, d_level.d_centerY, d_level.d_centerZ),
			Vector3(d_level.d_extentX, d_level.d_extentY, d_level.d_extentZ));
}
