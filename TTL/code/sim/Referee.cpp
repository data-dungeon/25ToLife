/*
**
**  File:   Referee.cpp
**  Date:   October 5, 2004
**  By:     Bryant Collard
**  Desc:   The referee basically has three responsibilities; it controls
**          progression through a round, it manages participants, and it is
**          the installation site for game modes.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "sim/Referee.h"

// GameLib headers
#include "Network/Connection.h"
#include "GameHelper/LogUtil.h"
#include "GameHelper/ErrorStack.h"

// TTL headers
#include "sim/TeamingModel.h"
#include "sim/ScoringModel.h"
#include "main/PlayerProfile.h"
#include "main/PlayerAgent.h"
#include "environ/ConfigMgr.h"

// For voice
#include "frontend/savegame.h"
#include "Voice/NetZ/CVoiceChat.h"

#define STALE             (void*)0xFFFFFFFF

/////////////////////////////////// Referee ////////////////////////////////////

// A global for accessing this bad-boy
DeclareSingleton(Referee);

bool Referee::s_logAgents = false;

/******************************************************************************/
/******************************************************************************/
Referee::Referee(NParticipantConfig::TConfig* i_config, uint8* i_directPhaseExit,
		int32* i_phaseEndTime, int32* i_phaseLifetime, uint8* i_teamRole,
		ActorHandle* i_actor, uint8 i_maxPlayers, uint8 i_maxParticipants) :
	d_config(i_config),
	d_directPhaseExit(i_directPhaseExit),
	d_phaseEndTime(i_phaseEndTime),
	d_phaseLifetime(i_phaseLifetime),
	d_teamRole(i_teamRole),
	d_actor(i_actor),
	d_maxPlayers(i_maxPlayers),
	d_maxParticipants(i_maxParticipants),
	d_defaultTeamingModel(i_config, i_teamRole, i_maxPlayers, i_maxParticipants,
			MAX_TEAMS),
	d_defaultScoringModel(i_maxPlayers, i_maxParticipants),
	d_messageOwner("Referee")
{
	// NUM_PHASES is set in enum in Referee but the TTL.ddl file needs it so
	// NUM_REFEREE_PHASES is set in TTLConstants.h. Make sure they are in sync.
	ASSERT(NUM_PHASES == NUM_REFEREE_PHASES);

	d_localActive = true;
	d_enablePhase = false;
	d_phase = READY;
	d_currentPhaseStartTime = 0;
	d_currentPhaseEndTime = 0;
	d_returnToReady = false;
	d_allPlayersPublished = false;
	d_populateFailed = false;

	for (int i = 0; i < d_maxParticipants; i++)
		d_actor[i].MakeInvalid();

	d_localAgent = NULL;
	d_worldLoader = NULL;

	d_maxSlots = d_maxPlayers;

	d_teamingModel = &d_defaultTeamingModel;
	d_scoringModel = &d_defaultScoringModel;
	ApplyScoringModel();

	// Register message handlers.
	d_changeOptionsHandler.Initialize("GameOptionsChanged",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
			GetMessageOwner(), this);
	d_fixTeamsHandler.Initialize("FixTeams", MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_DUPLICAS,
			GetMessageOwner(), this);

	// Setup for debugging.
	g_debugOutput.RegisterCategory("Referee", "REF");
	g_debugOutput.RegisterCategory("Agent", "AGT");
}

/******************************************************************************/
/******************************************************************************/
Referee::~Referee()
{
	if (d_teamingModel != &d_defaultTeamingModel)
		delete d_teamingModel;
	if (d_scoringModel != &d_defaultScoringModel)
		delete d_scoringModel;
}

/******************************************************************************/
/******************************************************************************/
void Referee::ResetTeamingModel(void)
{
	if (d_teamingModel != &d_defaultTeamingModel)
	{
		delete d_teamingModel;
		d_teamingModel = &d_defaultTeamingModel;
	}
	ApplyTeamingModel();
}

/******************************************************************************/
/******************************************************************************/
void Referee::InstallTeamingModel(const char* i_name)
{
	if (d_teamingModel != &d_defaultTeamingModel)
		delete d_teamingModel;

	d_teamingModel = CTeamingModelCreator::Create(i_name, d_config, d_teamRole,
			d_maxPlayers, d_maxParticipants, MAX_TEAMS);

	if (d_teamingModel == NULL)
		d_teamingModel = &d_defaultTeamingModel;

	ApplyTeamingModel();
}

/******************************************************************************/
/******************************************************************************/
void Referee::LoadTeamingModel(void)
{
	const char* value = g_configmgr.GetChoiceMemberAttribute(
			"gameModes", "teamingmodel", "name");
	if ((value != NULL) && (value[0] != '\0'))
		InstallTeamingModel(value);
	else
		ResetTeamingModel();
}

/******************************************************************************/
/******************************************************************************/
void Referee::ResetScoringModel(void)
{
	if (d_scoringModel != &d_defaultScoringModel)
	{
		delete d_scoringModel;
		d_scoringModel = &d_defaultScoringModel;
		ApplyScoringModel();
	}
}

/******************************************************************************/
/******************************************************************************/
void Referee::InstallScoringModel(const char* i_name)
{
	if (d_scoringModel != &d_defaultScoringModel)
		delete d_scoringModel;
	
	d_scoringModel = CScoringModelCreator::Create(i_name, d_maxPlayers,
			d_maxParticipants);

	if (d_scoringModel == NULL)
		d_scoringModel = &d_defaultScoringModel;

	ApplyScoringModel();
}

/******************************************************************************/
/******************************************************************************/
void Referee::Update(void)
{
	UpdatePhase();
	d_modeScript.Run();
}

/******************************************************************************/
/******************************************************************************/
void Referee::ForceReadyPhaseExit(void)
{
	if (d_phase == READY)
		DirectPhaseExit(READY, CHANGE_REQUIRED);
}

/******************************************************************************/
/******************************************************************************/
void Referee::ResetReadyPhaseExit(void)
{
	DirectPhaseExit(READY, CHANGE_ALLOWED);
}

/******************************************************************************/
/******************************************************************************/
void Referee::CChangeOptions::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->ConfigureSlots();
	d_ref->LoadTeamingModel();
}

/******************************************************************************/
/******************************************************************************/
void Referee::CFixTeams::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	// Just in case the referee and the agent are out of sync when this message
	// comes in, handle it if either of them think the agent is ready.
	if (d_ref->d_localActive && (d_ref->IsReady(d_ref->d_localAgent->Id()) ||
			d_ref->d_localAgent->IsReady()))
	{
		char message[50];
		if (d_ref->d_teamingModel->GetBluePlayerCount() >
				(d_ref->d_maxPlayers / 2))
		{
			sprintf(message, "The %s team has too many players!",
					d_ref->d_teamingModel->GetBlueTeamName());
		}
		else if (d_ref->d_teamingModel->GetRedPlayerCount() >
				(d_ref->d_maxPlayers / 2))
		{
			sprintf(message, "The %s team has too many players!",
					d_ref->d_teamingModel->GetRedTeamName());
		}
		else if (d_ref->d_teamingModel->GetBluePlayerCount() == 0)
		{
			sprintf(message, "The %s team has no players!",
					d_ref->d_teamingModel->GetBlueTeamName());
		}
		else if (d_ref->d_teamingModel->GetRedPlayerCount() == 0)
		{
			sprintf(message, "The %s team has no players!",
					d_ref->d_teamingModel->GetRedTeamName());
		}
		else
		{
			ASSERT(false);
			sprintf(message, "Team has no players!");
		}
		g_errorStack.SetError("Invalid Teams", message,
				CErrorStack::ERROR_WARNING, 5000, "Ok");
		g_messageDispatcher.SendMessageToAll("PlayerReadyUpClear", NULL,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
bool Referee::ConfigureSlots(void)
{
	const char* maxSlotsString = g_configmgr.GetValue("maxPlayers");
	if (maxSlotsString == NULL)
		dbgPrint("Cannot find maxPlayers option\n");
	else
	{
		uint8 maxSlots = atoi(maxSlotsString);
		if (maxSlots == 0)
			dbgPrint("maxPlayer value bogus (%s)\n", maxSlotsString);
		else
		{
			if (!SetMaxSlots(maxSlots))
			{
				dbgPrint("Failed to set max slots to %d\n", maxSlots);
				return(false);
			}
			return(true);
		}
	}
	SetMaxSlots(d_maxPlayers);
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::SetMaxSlots(uint8 i_maxSlots)
{
	if (i_maxSlots < 1)
		return(false);

	if (i_maxSlots > d_maxPlayers)
		i_maxSlots = d_maxPlayers;

	uint8 filled, open, closed;
	CountPlayerSlots(filled, open, closed);

	uint8 usable = filled + open;
	if (i_maxSlots < usable)
	{
		uint8 numToClose = usable - i_maxSlots;
		if (numToClose > open)
			return(false);
		ClosePlayerSlots(numToClose);
	}
	else if (i_maxSlots > usable)
		OpenPlayerSlots(i_maxSlots - usable);

	d_maxSlots = i_maxSlots;
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::IsPlayer(uint8 i_id)
{
	return((i_id < d_maxPlayers) && IsParticipant(i_id));
}

/******************************************************************************/
/******************************************************************************/
bool Referee::IsNonPlayer(uint8 i_id)
{
	return((i_id >= d_maxPlayers) && IsParticipant(i_id));
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetNumPlayers(void)
{
	uint8 count = 0;
	for(uint8 i = 0; i < d_maxPlayers; i++)
	{
		if (IsParticipant(i))
			count++;
	}
	return(count);
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetNumActivePlayers(void)
{
	uint8 count = 0;
	if ((d_phase == POPULATING) || (d_phase == REPOPULATE) ||
			(d_phase == PLAYING) || (d_phase == REPORTING))
	{
		for(uint8 i = 0; i < d_maxPlayers; i++)
		{
			if (IsParticipant(i) && NParticipantConfig::NotGhost(d_config[i]))
				count++;
		}
	}
	return(count);
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetNumNonPlayers(void)
{
	uint8 count = 0;
	for(uint8 i = d_maxPlayers; i < d_maxParticipants; i++)
	{
		if (IsParticipant(i))
			count++;
	}
	return(count);
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetNumParticipants(void)
{
	uint8 count = 0;
	for(uint8 i = 0; i < d_maxParticipants; i++)
	{
		if (IsParticipant(i))
			count++;
	}
	return(count);
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetNextPlayer(uint8 i_lastId)
{
	uint8 i;
	if (i_lastId == INVALID_PARTICIPANT)
		i = 0;
	else
		i = i_lastId + 1;

	for(; i < d_maxPlayers; i++)
	{
		if (IsParticipant(i))
			return(i);
	}

	return(INVALID_PARTICIPANT);
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetNextNonPlayer(uint8 i_lastId)
{
	uint8 i;
	if (i_lastId == INVALID_PARTICIPANT)
		i = d_maxPlayers;
	else if (i_lastId < d_maxPlayers)
		return(INVALID_PARTICIPANT);
	else
		i = i_lastId + 1;

	for(; i < d_maxParticipants; i++)
	{
		if (IsParticipant(i))
			return(i);
	}

	return(INVALID_PARTICIPANT);
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetNextParticipant(uint8 i_lastId)
{
	uint8 i;
	if (i_lastId == INVALID_PARTICIPANT)
		i = 0;
	else
		i = i_lastId + 1;

	for(; i < d_maxParticipants; i++)
	{
		if (IsParticipant(i))
			return(i);
	}

	return(INVALID_PARTICIPANT);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::IsGhost(uint8 i_id)
{
	return(((d_phase == POPULATING) || (d_phase == REPOPULATE) ||
			(d_phase == PLAYING) || (d_phase == REPORTING)) &&
			NParticipantConfig::Ghost(d_config[i_id]));
}

/******************************************************************************/
/******************************************************************************/
bool Referee::IsReady(uint8 i_id)
{
	// Must be ready to have been made a non-ghost.
	return(NParticipantConfig::NotGhost(d_config[i_id]) ||
			NParticipantConfig::Active(d_config[i_id]));
}

/******************************************************************************/
/******************************************************************************/
bool Referee::IsAccepted(uint8 i_id)
{
	// Must have been accepted to have been made a non-ghost.
	if (NParticipantConfig::NotGhost(d_config[i_id]))
		return(true);

	// The active flag indicates ready status when a ghost.
	return(NParticipantConfig::Active(d_config[i_id]) &&
			NParticipantConfig::IsRequestedTeam(d_config[i_id],
			NParticipantConfig::AssignedTeam(d_config[i_id])));
}

/******************************************************************************/
/******************************************************************************/
bool Referee::IsAlive(uint8 i_id)
{
	// Only non-ghosts can be alive.
	return(NParticipantConfig::NotGhost(d_config[i_id]) &&
			NParticipantConfig::Active(d_config[i_id]));
}

/******************************************************************************/
/******************************************************************************/
bool Referee::IsActivePlayer(uint8 i_id)
{
	return(NParticipantConfig::NotGhost(d_config[i_id]));
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetTeam(uint8 i_id)
{
	if (i_id < d_maxParticipants)
		return(NParticipantConfig::AssignedTeam(d_config[i_id]));
	return(INVALID_TEAM);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::GetTeam(uint8 i_id, uint8 &o_team)
{
	if (i_id < d_maxParticipants)
	{
		o_team = NParticipantConfig::AssignedTeam(d_config[i_id]);
		return((o_team != INVALID_TEAM) &&
				(o_team == NParticipantConfig::RequestedTeam(d_config[i_id])));
	}
	o_team = INVALID_TEAM;
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::DoesTeamLimitSpawnTicketsForParticipant(uint8 i_participantId)
{
	return(DoesTeamLimitSpawnTickets(GetTeam(i_participantId)));
}

/******************************************************************************/
/******************************************************************************/
bool Referee::RequestSpawnTicketForParticipant(uint8 i_participantId)
{
	if (IsParticipant(i_participantId) && !IsGhost(i_participantId))
		return(RequestSpawnTicket(GetTeam(i_participantId)));
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::DoesTeamLimitSpawnTicketsForLocalAvatar(void)
{
	if (d_localAgent != NULL)
		return(DoesTeamLimitSpawnTickets(GetTeam(d_localAgent->Id())));
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::RequestSpawnTicketForLocalAvatar(void)
{
	if ((d_localAgent != NULL) && !IsGhost(d_localAgent->Id()))
		return(RequestSpawnTicket(GetTeam(d_localAgent->Id())));
	return(false);
}

/******************************************************************************/
/******************************************************************************/
uint8 Referee::GetLocalPlayerAgentId(void)
{
	if (d_localAgent != NULL)
		return(d_localAgent->Id());
	return(INVALID_PARTICIPANT);
}

/******************************************************************************/
/******************************************************************************/
CActor* Referee::GetActor(uint8 i_id)
{
	if (IsParticipant(i_id))
		return(CActor::FromHandle(d_actor[i_id]));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::DoAllAgentsHaveAvatars(void)
{
	for (int i = 0; i < d_maxPlayers; ++i)
	{
		if (IsParticipant(i) && !IsGhost(i) && !d_actor[i].Valid())
			return(false);
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void Referee::AssignParticipant(uint8 i_id, CPlayerAgent* i_playerAgent)
{
	d_teamingModel->AssignTeam(i_id, i_playerAgent);
}

/******************************************************************************/
/******************************************************************************/
void Referee::AssignParticipant(uint8 i_id, CActor* i_actor)
{
	d_teamingModel->AssignTeam(i_id, i_actor);
}

/******************************************************************************/
/******************************************************************************/
Referee::EPhaseChangeDirection Referee::GetPhaseChangeDirection(
		EPhase &o_nextPhase)
{
	if (d_enablePhase)
	{
		switch(d_phase)
		{
		 case READY:
		 {
			if (!d_returnToReady)
			{
				EPlayersStatus status = DeterminePlayersStatus();
				if ((((EPhaseChangeDirection)d_directPhaseExit[READY] != NO_CHANGE)
						&& (status == ALL_READY)) ||
						(((EPhaseChangeDirection)d_directPhaseExit[READY] ==
						CHANGE_REQUIRED) && (status == ALL_LOADED)))
				{
					DirectPhaseExit(READY, CHANGE_ALLOWED);
					o_nextPhase = POPULATING;
					return((EPhaseChangeDirection)d_directPhaseExit[READY]);
				}
			}
			break;
		 }

		 case SETUP:
		 {
			if (d_returnToReady)
			{
				o_nextPhase = READY;
				return(CHANGE_REQUIRED);
			}
			EPlayersStatus status = DeterminePlayersStatus();
			if (((EPhaseChangeDirection)d_directPhaseExit[SETUP] != NO_CHANGE)
					&& ((status == ALL_LOADED) || (status == ALL_READY)))
			{
				o_nextPhase = POPULATING;
				return((EPhaseChangeDirection)d_directPhaseExit[SETUP]);
			}
			break;
		 }

		 case POPULATING:
			if (d_returnToReady)
			{
				o_nextPhase = PLAYING;
				return(CHANGE_REQUIRED);
			}
			if (d_populateFailed)
			{
				o_nextPhase = REPOPULATE;
				return(CHANGE_REQUIRED);
			}
			if (((EPhaseChangeDirection)d_directPhaseExit[POPULATING] !=
					NO_CHANGE) && d_allPlayersPublished)
			{
				o_nextPhase = PLAYING;
				return((EPhaseChangeDirection)d_directPhaseExit[POPULATING]);
			}
			break;

		 case REPOPULATE:
		 {
			EPlayersStatus status = DeterminePlayersStatus();
			if ((status == ALL_LOADED) || (status == ALL_READY))
			{
				o_nextPhase = POPULATING;
				return(CHANGE_REQUIRED);
			}
			break;
		 }

		 case PLAYING:
			if (d_returnToReady)
			{
				o_nextPhase = REPORTING;
				return(CHANGE_REQUIRED);
			}
			if ((EPhaseChangeDirection)d_directPhaseExit[PLAYING] != NO_CHANGE)
			{
				o_nextPhase = REPORTING;
				return((EPhaseChangeDirection)d_directPhaseExit[PLAYING]);
			}
			break;

		 case REPORTING:
		 {
			if (d_returnToReady)
			{
				o_nextPhase = READY;
				return(CHANGE_REQUIRED);
			}
			EPlayersStatus status = DeterminePlayersStatus();
			if (((EPhaseChangeDirection)d_directPhaseExit[REPORTING] != NO_CHANGE)
					&& ((status == ALL_UNLOADED) || (status == ALL_INACTIVE)))
			{
				if (GetRoundCount() >= GetRoundsInMatch())
					o_nextPhase = READY;
				else
					o_nextPhase = SETUP;
				return((EPhaseChangeDirection)d_directPhaseExit[REPORTING]);
			}
			break;
		 }
		}
	}

	o_nextPhase = d_phase;
	return(NO_CHANGE);
}

/******************************************************************************/
/******************************************************************************/
void Referee::SetPhase(EPhase i_phase, int32 i_startTime)
{
	if (i_phase != d_phase)
	{
		// Start a match.
		if (d_phase == READY)
		{
			const char* value = g_configmgr.GetChoiceMemberAttribute(
					"gameModes", "script", "name");
			ASSERT((value != NULL) && (value[0] != '\0'));
			d_modeScript.SetScript(value);
			d_modeScript.SetMessageOwner(&d_messageOwner);
			d_modeScript.Run();
		}

		// The repopulate phase is really more of a sub-phase of the populate
		// phase. Detect if leaving repopulate.
		bool leavingRepopulate = (d_phase == REPOPULATE);

		// Cannot accept immigration if not playing.
		if (d_phase == PLAYING)
			g_session.AcceptImmigration(false);

		if (d_phase == REPORTING)
			g_world.ClearExtents();

		SLogMessage logMessage;
		d_phase = i_phase;
		d_currentPhaseStartTime = (i_startTime == -MAXINT) ?
				g_session.GetSessionTimeMS() : i_startTime;
		ComputeCurrentPhaseEndTime();
		uint8 category = g_debugOutput.Category("REF");
		switch(d_phase)
		{
		 case READY:
			d_returnToReady = false;
			DirectPhaseExit(READY, CHANGE_ALLOWED);
			d_modeScript.ClearScript();
			SetRoundsInMatch(1);
			ResetRoundCount();
			ResetScoringModel();
			AssignAlphaToBlueTeam();
			AssignBetaToRedTeam();
			g_messageDispatcher.SendMessageToAll("ReadyUpStarting", 0,
					INVALID_OBJECT_HANDLE);
			logMessage.d_action = "Ready";
			g_debugOutput.TSPrint(category, "Enter READY phase");
			break;

		 case SETUP:
			logMessage.d_action = "Setup";
			g_debugOutput.TSPrint(category, "Enter SETUP phase");
			break;

		 case POPULATING:
			// Don't treat this as a change into the populate phase if leaving
			// repopulate.
			if (!leavingRepopulate)
			{
				const char* value = g_configmgr.GetValue("teamTicketCount");
				int numTickets = (value != NULL) ? atoi(value) : 0;
				if ((numTickets >= 0xFFFF) || (numTickets < 0))
					SetUnlimitedSpawnTicketsForAll();
				else
					SetNumSpawnTicketsForAll((uint16)numTickets);

				IncrementRoundCount();

				g_messageDispatcher.SendMessageToAll("BeginPopulating", 0,
						INVALID_OBJECT_HANDLE);

				// Increase our connection timeout while populting to compensate for
				// possible reduced connection service updating.
				if (ValidSingleton(CConnection))
				{
					g_connection.ResetBPSCounters();
					g_connection.SetConnectionTimeout(true);
					g_console.Echo("*************** enabling voice chat channels from Referee::POPULATING ****************");
					g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, false);
					g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM1, false);
					g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM2, false);
				}
			}
			logMessage.d_action = "Populating";
			g_debugOutput.TSPrint(category, "Enter POPULATING phase");
			break;

		 case REPOPULATE:
			logMessage.d_action = "Repopulate";
			g_debugOutput.TSPrint(category, "Enter REPOPULATE phase");
			break;

		 case PLAYING:
			// Restore our connection timeout.
			if (ValidSingleton(CConnection))
			{
				g_connection.SetConnectionTimeout(false);
/*				g_console.Echo("*************** enabling voice chat channels from Referee::PLAYING ****************");
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM1, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM2, false);
				// enable team chat when in game
				int participantID = d_localAgent->Id();
				if( participantID != INVALID_PARTICIPANT )
				{
					bool blue = d_teamingModel->IsPlayerBlue(participantID);
					int teamID = (blue ? 1 : 2);
					g_CVoiceChat.enableChatChannel((( CVoiceChat::eChatChannels )teamID ), true);
				}*/

				// --- set voice enable
				bool VoiceEnabled = g_GlobalProfile.VoiceParams.VoiceOutput != VOICEOUTPUT_DISABLED;
				g_CVoiceChat.enable(VoiceEnabled);
			}
			g_messageDispatcher.SendMessageToAll("BeginRound", 0,
					INVALID_OBJECT_HANDLE);
			logMessage.d_action = "Playing";
			g_debugOutput.TSPrint(category, "Enter PLAYING phase");
			break;

		 case REPORTING:
			g_messageDispatcher.SendMessageToAll("EndRound", 0,
					INVALID_OBJECT_HANDLE);
			logMessage.d_action = "Reporting";
			g_debugOutput.TSPrint(category, "Enter REPORTING phase");

			if(ValidSingleton(CConnection))
			{
				g_console.Echo("*************** enabling voice chat channels from Referee::REPORTING ****************");
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM1, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM2, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, true);

				// --- set voice enable
				bool VoiceEnabled = g_GlobalProfile.VoiceParams.VoiceOutput != VOICEOUTPUT_DISABLED;
				g_CVoiceChat.enable(VoiceEnabled);

			}
			break;
		}

		logMessage.d_type = SLogMessage::LOG_ACTION;
		logMessage.d_label = "Referee";
		g_messageDispatcher.SendMessageToAll("LogEntry", &logMessage,
				INVALID_OBJECT_HANDLE);
	}
	else if ((i_startTime != -MAXINT) && (i_startTime != d_currentPhaseStartTime))
	{
		d_currentPhaseStartTime = i_startTime;
		ComputeCurrentPhaseEndTime();
	}
}

/******************************************************************************/
/******************************************************************************/
void Referee::ComputeCurrentPhaseEndTime(void)
{
	if (d_phaseEndTime[d_phase] > d_currentPhaseStartTime)
		d_currentPhaseEndTime = d_phaseEndTime[d_phase];
	else
		d_currentPhaseEndTime = d_currentPhaseStartTime + d_phaseLifetime[d_phase];
}

/******************************************************************************/
/******************************************************************************/
void Referee::SetRemainingPhaseTime(int32 i_time, bool i_exceedLifetime)
{
	if ((d_phase != READY) && (i_time >= 0))
	{
		int32 endTime = g_session.GetSessionTimeMS() + i_time;
		if (i_exceedLifetime ||
				((endTime - d_currentPhaseStartTime) < d_phaseLifetime[d_phase]))
			SetPhaseEndTime(d_phase, endTime);
	}
}

/******************************************************************************/
/******************************************************************************/
int32 Referee::GetElapsedPhaseTime(void)
{
	return(g_session.GetSessionTimeMS() - d_currentPhaseStartTime);
}

/******************************************************************************/
/******************************************************************************/
int32 Referee::GetRemainingPhaseTime(void)
{
	return(d_currentPhaseEndTime - g_session.GetSessionTimeMS());
}

/******************************************************************************/
/******************************************************************************/
int32 Referee::GetPhaseCountdown(void)
{
	int32 remainingTime = d_currentPhaseEndTime - g_session.GetSessionTimeMS();
	if (remainingTime > 0)
		return(((remainingTime - 1) / 1000) + 1);
	return(0);
}

/******************************************************************************/
/******************************************************************************/
bool Referee::IsTimedPhase(void)
{
	return((EPhaseChangeDirection)d_directPhaseExit[d_phase] != NO_CHANGE);
}

/******************************************************************************/
/******************************************************************************/
const char* Referee::GetPhaseName(EPhase i_phase)
{
	switch(i_phase)
	{
	 case READY:
		return("READY");
	 case SETUP:
		return("SETUP");
	 case POPULATING:
		return("POPULATING");
	 case REPOPULATE:
		return("REPOPULATE");
	 case PLAYING:
		return("PLAYING");
	 case REPORTING:
		return("REPORTING");
	}
	ASSERT(false);
	return("ERROR");
}

////////////////////////////// StandAloneReferee ///////////////////////////////

/******************************************************************************/
/******************************************************************************/
StandAloneReferee::StandAloneReferee() :
	Referee(d_standAloneConfig, d_standAloneDirectPhaseExit,
			d_standAlonePhaseEndTime, d_standAlonePhaseLifetime,
			d_standAloneTeamRole, d_standAloneActor,
			MAX_STANDALONE_PLAYERS, MAX_STANDALONE_PARTICIPANTS)
{
	uint8 i;
	for (i = 0; i < MAX_STANDALONE_PARTICIPANTS; i++)
	{
		d_participantKey[i] = NULL;
		NParticipantConfig::SetInvalid(d_standAloneConfig[i]);
		d_standAloneActor[i].MakeInvalid();
	}

	for (i = 0; i < NUM_PHASES; i++)
	{
		d_standAloneDirectPhaseExit[i] = (uint8)CHANGE_ALLOWED;
		d_standAlonePhaseEndTime[i] = 0;
		d_standAlonePhaseLifetime[i] = 0;
	}

	SetUnlimitedSpawnTicketsForAll();
	ResetRoundCount();
	d_roundsInMatch = 1;

	for (i = 0; i < MAX_TEAMS; i++)
	{
		d_standAloneTeamRole[i] = CTeamingModel::AMBIENT;
	}
	d_teamingModel->AssignAlphaToBlueTeam();
	d_teamingModel->AssignBetaToRedTeam();

	g_playerProfile.Initialize();
}

/******************************************************************************/
/******************************************************************************/
StandAloneReferee::~StandAloneReferee()
{
	g_playerProfile.Terminate();
	if (d_scoringModel && d_scoringModel->GetBuffer())
		memFree(d_scoringModel->GetBuffer());
}

/******************************************************************************/
/******************************************************************************/
const char* StandAloneReferee::Validate(void)
{
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool StandAloneReferee::IsParticipationClosed(bool i_player)
{
	return(FindOpenSlot((void*)NULL, d_participantKey, (void*)NULL, i_player) ==
			INVALID_PARTICIPANT);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::RegisterParticipant(CPlayerAgent* i_playerAgent)
{
	if (i_playerAgent == NULL)
		return;

	uint8 id = FindOpenSlot((void*)i_playerAgent, d_participantKey,
			(void*)NULL, true);
	if (id != INVALID_PARTICIPANT)
	{
		NParticipantConfig::SetRequestedTeam(d_standAloneConfig[id],
				i_playerAgent->GetRequestedTeam());
		NParticipantConfig::SetActive(d_standAloneConfig[id],
				i_playerAgent->IsReady());
		AssignParticipant(id, i_playerAgent);
		NParticipantConfig::SetGhost(d_standAloneConfig[id], false);
		d_participantKey[id] = (void*)i_playerAgent;
		i_playerAgent->SetId(id);
		g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
				INVALID_OBJECT_HANDLE);
		g_messageDispatcher.SendMessageToAll("PlayerAdded", (void*)id,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::RegisterParticipant(CActor* i_actor)
{
	if (i_actor == NULL)
		return;

	uint8 id = INVALID_PARTICIPANT;
	CParticipantIF* participantIF = CParticipantIF::GetInterface(i_actor);
	if (participantIF != NULL)
		id = participantIF->GetParticipantId();
	if (id < d_maxParticipants)
	{
		if (d_participantKey[id] != NULL)
		{
			ASSERT(!d_actor[id].Valid() ||
					(d_actor[id] == CActor::ToHandle(i_actor)));
			if (!d_actor[id].Valid())
			{
				d_actor[id] = CActor::ToHandle(i_actor);
				if (id < d_maxPlayers)
					g_messageDispatcher.SendMessageToAll("AvatarParticipantAdded",
							(void*)id, d_actor[id]);
			}
			return;
		}
	}

	id = FindOpenSlot((void*)i_actor, d_participantKey, (void*)NULL, false);
	if (id != INVALID_PARTICIPANT)
	{
		CParticipantIF* participantIF = CParticipantIF::GetInterface(i_actor);
		if (participantIF != NULL)
		{
			participantIF->SetParticipantId(id);
			NParticipantConfig::SetRequestedTeam(d_standAloneConfig[id],
					participantIF->GetRequestedTeamId());
			NParticipantConfig::SetActive(d_standAloneConfig[id],
					participantIF->GetAlive());
		}
		else
		{
			NParticipantConfig::SetInvalidRequestedTeam(d_standAloneConfig[id]);
			NParticipantConfig::SetActive(d_standAloneConfig[id], false);
		}
		AssignParticipant(id, i_actor);
		d_participantKey[id] = (void*)i_actor;
		d_actor[id] = CActor::ToHandle(i_actor);
		g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::RemoveParticipant(CPlayerAgent* i_playerAgent)
{
	if (i_playerAgent == NULL)
		return;

	uint8 id = i_playerAgent->Id();
	if (id < d_maxParticipants)
	{
		ASSERT((d_participantKey[id] == (void*)i_playerAgent) &&
				!d_actor[id].Valid());
		g_messageDispatcher.SendMessageToAll("PlayerRemoved", (void*)id,
				INVALID_OBJECT_HANDLE);
		d_participantKey[id] = NULL;
		NParticipantConfig::SetInvalid(d_standAloneConfig[id]);
		if (d_actor[id].Valid())
		{
			if (id < d_maxPlayers)
				g_messageDispatcher.SendMessageToAll("AvatarParticipantRemoved",
						(void*)id, d_actor[id]);
			d_actor[id].MakeInvalid();
		}
		i_playerAgent->SetId(INVALID_PARTICIPANT);
		g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::RemoveParticipant(CActor* i_actor)
{
	if (i_actor == NULL)
		return;

	uint8 id;
	CParticipantIF* participantIF = CParticipantIF::GetInterface(i_actor);
	if ((participantIF != NULL) &&
			((id = participantIF->GetParticipantId()) < d_maxParticipants))
	{
		ASSERT(d_actor[id] == CActor::ToHandle(i_actor));
		if (d_participantKey[id] == (void*)i_actor)
		{
			d_participantKey[id] = NULL;
			NParticipantConfig::SetInvalid(d_standAloneConfig[id]);
			g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
					INVALID_OBJECT_HANDLE);
		}
		if (d_actor[id].Valid())
		{
			if (id < d_maxPlayers)
				g_messageDispatcher.SendMessageToAll("AvatarParticipantRemoved",
						(void*)id, d_actor[id]);
			d_actor[id].MakeInvalid();
		}
		participantIF->SetParticipantId(INVALID_PARTICIPANT);
	}
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::RequestTeam(uint8 i_id, uint8 i_requestedTeam)
{
	if ((i_id < d_maxParticipants) &&
			!NParticipantConfig::Invalid(d_standAloneConfig[i_id]))
	{
		if (i_requestedTeam != NParticipantConfig::RequestedTeam(
				d_standAloneConfig[i_id]))
		{
			NParticipantConfig::SetRequestedTeam(d_standAloneConfig[i_id],
					i_requestedTeam);
			d_teamingModel->ProcessTeamChanges();
			g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
					INVALID_OBJECT_HANDLE);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetActive(uint8 i_id, bool i_active)
{
	if ((i_id < d_maxParticipants) &&
			!NParticipantConfig::Invalid(d_standAloneConfig[i_id]))
	{
		NParticipantConfig::SetActive(d_standAloneConfig[i_id], i_active);
		g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::DirectPhaseExit(EPhase i_phase,
		EPhaseChangeDirection i_direction)
{
	d_standAloneDirectPhaseExit[i_phase] = (uint8)i_direction;
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetPhaseEndTime(EPhase i_phase, int32 i_time)
{
	d_standAlonePhaseEndTime[i_phase] = i_time;
	ComputeCurrentPhaseEndTime();
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetPhaseLifetime(EPhase i_phase, int32 i_time)
{
	d_standAlonePhaseLifetime[i_phase] = i_time;
	ComputeCurrentPhaseEndTime();
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::UpdatePhase(void)
{
	d_allPlayersPublished = (d_localAgent->GetMode() == CPlayerAgent::PUBLISHED);

	EPhase nextPhase;
	EPhaseChangeDirection direction = GetPhaseChangeDirection(nextPhase);
	if ((direction == CHANGE_REQUIRED) || ((direction == CHANGE_ALLOWED) &&
			(g_session.GetSessionTimeMS() > d_currentPhaseEndTime)))
		SetPhase(nextPhase);
}

/******************************************************************************/
/******************************************************************************/
Referee::EPlayersStatus StandAloneReferee::DeterminePlayersStatus(void)
{
	switch (d_localAgent->GetMode())
	{
	 case CPlayerAgent::INACTIVE:
		return(ALL_INACTIVE);

	 case CPlayerAgent::UNLOADED:
		return(ALL_UNLOADED);

	 case CPlayerAgent::LOADED:
	 {
		uint8 id = d_localAgent->Id();
		if ((id < d_maxPlayers) && d_localAgent->IsReady() &&
				NParticipantConfig::IsRequestedTeam(d_standAloneConfig[id],
				NParticipantConfig::AssignedTeam(d_standAloneConfig[id])))
			return(ALL_READY);
		return(ALL_LOADED);
	 }
	}

	return(OTHER);
}

/******************************************************************************/
/******************************************************************************/
bool StandAloneReferee::IsParticipant(uint8 i_id)
{
	return((i_id < d_maxParticipants) && (d_participantKey[i_id] != NULL));
}

/******************************************************************************/
/******************************************************************************/
uint8 StandAloneReferee::GetNumStartPlayers(void)
{
	return(1);
}

/******************************************************************************/
/******************************************************************************/
uint16 StandAloneReferee::ParticipantSpawnTicketCount(uint8 i_participantId)
{
	uint8 teamId = GetTeam(i_participantId);
	if (teamId < MAX_TEAMS)
		return(d_spawnTickets[teamId]);
	return(0);
}

/******************************************************************************/
/******************************************************************************/
uint16 StandAloneReferee::TeamSpawnTicketCount(uint8 i_teamId)
{
	if (i_teamId < MAX_TEAMS)
		return(d_spawnTickets[i_teamId]);
	return(0);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetUnlimitedSpawnTickets(uint8 i_teamId)
{
	if (i_teamId < MAX_TEAMS)
		d_spawnTickets[i_teamId] = 0xFFFF;
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetUnlimitedSpawnTicketsForAll(void)
{
	for (uint8 i = 0; i < MAX_TEAMS; i++)
		d_spawnTickets[i] = 0xFFFF;
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetNumSpawnTickets(uint8 i_teamId,
		uint16 d_numSpawnTickets)
{
	if (i_teamId < MAX_TEAMS)
		d_spawnTickets[i_teamId] = d_numSpawnTickets;
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetNumSpawnTicketsForAll(uint16 d_numSpawnTickets)
{
	for (uint8 i = 0; i < MAX_TEAMS; i++)
		d_spawnTickets[i] = d_numSpawnTickets;
}

/******************************************************************************/
/******************************************************************************/
bool StandAloneReferee::DoesTeamLimitSpawnTickets(uint8 i_teamId)
{
	if (i_teamId < MAX_TEAMS)
		return(d_spawnTickets[i_teamId] != 0xFFFF);
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool StandAloneReferee::RequestSpawnTicket(uint8 i_teamId)
{
	if (i_teamId < MAX_TEAMS)
	{
		if (d_spawnTickets[i_teamId] > 0)
		{
			if (d_spawnTickets[i_teamId] != 0xFFFF)
				d_spawnTickets[i_teamId]--;
			return(true);
		}
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetRoundsInMatch(uint8 i_roundsInMatch)
{
	d_roundsInMatch = i_roundsInMatch;
}

/******************************************************************************/
/******************************************************************************/
uint8 StandAloneReferee::GetRoundsInMatch(void)
{
	return(d_roundsInMatch);
}

/******************************************************************************/
/******************************************************************************/
uint8 StandAloneReferee::GetRoundCount(void)
{
	return(d_roundCount);
}

/******************************************************************************/
/******************************************************************************/
bool StandAloneReferee::IsRoundOpen(void)
{
	return((d_phase == POPULATING) || (d_phase == REPOPULATE) ||
			(d_phase == PLAYING));
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::ResetRoundCount(void)
{
	d_roundCount = 0;
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::IncrementRoundCount(void)
{
	d_roundCount++;
}

/******************************************************************************/
/******************************************************************************/
CPlayerAgent* StandAloneReferee::CreateLocalPlayerAgent(void)
{
	g_playerProfile.SetReady(true);
	d_localAgent = &g_playerProfile;
	return(d_localAgent);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::DestroyLocalPlayerAgent(void)
{
	d_localAgent = NULL;
}

/******************************************************************************/
/******************************************************************************/
CPlayerAgent* StandAloneReferee::GetPlayerAgent(uint8 i_id)
{
	if (i_id < d_maxPlayers)
		return((CPlayerAgent*)d_participantKey[i_id]);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool StandAloneReferee::CanLoadWorldActors(int &o_start, int &o_end)
{
	o_start = 0;
	o_end = MAXINT;
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::InstallScoringModel(const char* i_name)
{
	if (d_scoringModel->GetBuffer() != NULL)
		memFree(d_scoringModel->GetBuffer());

	Referee::InstallScoringModel(i_name);

	if (d_scoringModel->GetBufferSize() > 0)
	{
		d_scoringModel->SetBuffer(memAlloc(d_scoringModel->GetBufferSize(), 16));
		d_scoringModel->InitializeBuffer();
	}
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetAlphasRole(CTeamingModel::ERole i_eRole)
{
	d_teamingModel->SetAlphasRole(i_eRole);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::SetBetasRole(CTeamingModel::ERole i_eRole)
{
	d_teamingModel->SetBetasRole(i_eRole);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::AssignAlphaToBlueTeam()
{
	d_teamingModel->AssignAlphaToBlueTeam();
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::AssignBetaToBlueTeam()
{
	d_teamingModel->AssignBetaToBlueTeam();
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::AssignAlphaToRedTeam()
{
	d_teamingModel->AssignAlphaToRedTeam();
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::AssignBetaToRedTeam()
{
	d_teamingModel->AssignBetaToRedTeam();
}

/******************************************************************************/
/******************************************************************************/
int StandAloneReferee::GetPlayerAgentCount(void)
{
	return(1);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::CountPlayerSlots(uint8 &o_filled, uint8 &o_open,
		uint8 &o_closed)
{
	Referee::CountPlayerSlots(d_participantKey, (void*)NULL, STALE, o_filled,
			o_open, o_closed);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::ClosePlayerSlots(uint8 i_numToClose)
{
	Referee::ClosePlayerSlots(i_numToClose, d_participantKey, (void*)NULL, STALE);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::OpenPlayerSlots(uint8 i_numToOpen)
{
	Referee::OpenPlayerSlots(i_numToOpen, d_participantKey, (void*)NULL, STALE);
}

/******************************************************************************/
/******************************************************************************/
void StandAloneReferee::ApplyTeamingModel(void)
{
	d_teamingModel->ValidateAssignments();
}
