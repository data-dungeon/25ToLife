/*
**
**  File:   DOReferee.cpp
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the referee distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DOReferee.h"

// GameLib headers
#include "Network/NetZ/DOActor.h"
#include "Network/NetZ/SessionTime.h"
#include "GameHelper/LogUtil.h"
#include "Network/NetZ/NetZSession.h"
#include "Voice/NetZ/CVoiceChat.h"
#include "GameHelper/ErrorStack.h"

// TTL headers.
#include "main/Lobby.h"
#include "NetZTTL/DOPlayerAgent.h"
#include "sim/TeamingModel.h"
#include "sim/ScoringModel.h"
#include "environ/ConfigMgr.h"
#include "environ/WorldActorLoader.h"

// Invalid player and non-player pointers.
#define STALE             (void*)0xFFFFFFFF
#define PTR_VALID(p)      (((p) != NULL) && ((p) != STALE))

// Round status.
#define ROUND_CLOSED  0
#define ROUND_OPENED  1
#define ROUND_STARTED 2

// Don't let the referee change its mind about the next phase when it is close
// to the change time.
#define CHANGE_TIME_BUFFER 500
/******************************************************************************/
/******************************************************************************/
static void LogMessage(char* i_label, char* i_action, const char* i_name)
{
	SLogMessage logMessage;
	logMessage.d_type = SLogMessage::LOG_ACTION;
	logMessage.d_label = i_label;
	logMessage.d_action = i_action;
	g_messageDispatcher.SendMessageToAll(i_name, &logMessage,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
DOReferee::DOReferee() :
	Referee(d_assignment.d_config, d_match.d_directPhaseExit,
			(int32*)d_match.d_phaseEndTime, (int32*)d_match.d_phaseLifetime,
			d_rules.d_teamRole, d_networkedActor,
			MAX_PLAYERS, MAX_PARTICIPANTS)
{
	uint8 i;
	for (i = 0; i < MAX_PLAYERS; i++)
		d_player[i] = NULL;
	for (i = 0; i < (MAX_PARTICIPANTS - MAX_PLAYERS); i++)
		d_nonPlayer[i] = NULL;
	d_localDOAgent = NULL;

	d_nextPhase.d_value = (uint8)d_phase;
	d_nextPhase.d_changeTime = 0;
	d_nextPhase.d_assignmentId = 0;
	for (i = 0; i < MAX_PARTICIPANTS; i++)
	{
		d_id.d_handle[i] = INVALID_DOHANDLE;
		NParticipantConfig::SetInvalid(d_assignment.d_config[i]);
	}
	d_assignment.d_assignmentId = 0;
	d_populate.d_crc = 0;
	d_populate.d_numPlayers = 0;
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		d_populate.d_start[i] = -1;
		d_populate.d_end[i] = -1;
	}
	d_round.d_status = ROUND_CLOSED;
	d_rules.d_desc = g_configmgr.GetOptions();
	d_score.SetSize(0);
	d_score.SetCallback(d_callback.Set(d_scoringModel,
			&CScoringModel::DataHasChanged));

	for (i = 0; i < NUM_PHASES; i++)
	{
		d_match.d_directPhaseExit[i] = (uint8)CHANGE_ALLOWED;
		d_match.d_phaseEndTime[i] = 0;
		d_match.d_phaseLifetime[i] = 0;
	}
	d_match.d_roundsInMatch = 1;
	d_match.d_roundCount = 0;

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		d_scoringPlayerInfo.d_name[i] = "";
		d_scoringPlayerInfo.d_team[i] = 0xFF;
	}
	d_scoringPlayerInfo.d_isAlphaBlue = true;

	d_session.d_alphaWins = 0;
	d_session.d_betaWins = 0;

	for (i = 0; i < MAX_TEAMS; i++)
	{
		d_teams.d_spawnTickets[i] = 0xFFFF;
		d_rules.d_teamRole[i] = CTeamingModel::AMBIENT;
	}
	d_teamingModel->AssignAlphaToBlueTeam();
	d_teamingModel->AssignBetaToRedTeam();

	// Register message handlers.
	d_changeConfigHandler.Initialize("GameOptionsChanged",
			MESSAGE_HANDLE_IF_MASTER, GetMessageOwner(), this);
	d_openRoundHandler.Initialize("RoundOpened", MESSAGE_HANDLE_IF_MASTER,
			GetMessageOwner(), this);
	d_startRoundHandler.Initialize("RoundStarted", MESSAGE_HANDLE_IF_MASTER,
			GetMessageOwner(), this);
	d_endRoundHandler.Initialize("RoundEnded", MESSAGE_HANDLE_IF_MASTER,
			GetMessageOwner(), this);
	d_assignActorHandler.Initialize("AssignActor", MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_HANDLE_IF_DUPLICA, GetMessageOwner(), this);
	d_requestSpawnTicketHandler.Initialize("RequestSpawnTicket",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER, GetMessageOwner(),
			this);

	// Debugging
	d_logHandler.Initialize("LogEntry",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
			GetMessageOwner(), this);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::InitDO(void)
{
	if (IsADuplicationMaster())
	{
		g_lobby.GetRoundId(d_round.d_roundId);
		g_configmgr.SetOptionByName("readyUp", (d_phase == READY) ? "1" : "0");
		char count[4];
		sprintf(count, "%d", GetNumPlayers());
		g_configmgr.SetOptionByName("numPlayers", count);
		g_configmgr.AcceptChanges(true);
	}
	else
	{
		g_lobby.SetRoundId(d_round.d_roundId);
		g_configmgr.SetOptions(d_rules.d_desc.CStr());
		ComputeCurrentPhaseEndTime();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::CChangeConfig::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (d_ref->IsADuplicationMaster())
	{
		d_ref->d_rules.d_desc = g_configmgr.GetOptions();
		d_ref->d_rules.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::COpenRound::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (d_ref->IsADuplicationMaster())
	{
		g_lobby.GetRoundId(d_ref->d_round.d_roundId);
		d_ref->d_round.d_status = ROUND_OPENED;
		d_ref->d_round.RequestUpdate();
		LogMessage("Populate", "OpenRound", "LogLock");
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::CStartRound::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (d_ref->IsADuplicationMaster())
	{
		d_ref->d_round.d_status = ROUND_STARTED;
		d_ref->d_round.RequestUpdate();
		LogMessage("Populate", "StartRound", "LogLock");
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::CEndRound::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (d_ref->IsADuplicationMaster())
	{
		g_lobby.GetRoundId(d_ref->d_round.d_roundId);
		d_ref->d_round.d_status = ROUND_CLOSED;
		d_ref->d_round.RequestUpdate();
		LogMessage("Populate", "EndRound", "LogLock");
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::CAssignActor::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (i_data != NULL)
	{
		DOActor* doActor = (DOActor*)i_data;
		CActor* actor = doActor->GetActor();
		if (actor != NULL)
		{
			CParticipantIF* participantIF =
					(CParticipantIF*)actor->GetInterface(CParticipantIF::GetID());
			if (participantIF != NULL)
			{
				uint8 id = participantIF->GetParticipantId();
				if (id < d_ref->d_maxPlayers)
					d_ref->AddAvatar(doActor, id);
				else if (id < d_ref->d_maxParticipants)
					d_ref->AddActor(doActor, id);
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::CRequestSpawnTicket::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	uint8 teamId = (uint8)i_data;
	if (d_ref->IsADuplicationMaster() && (teamId < MAX_TEAMS) &&
			(d_ref->d_teams.d_spawnTickets[teamId] > 0) &&
			(d_ref->d_teams.d_spawnTickets[teamId] != 0xFFFF))
	{
		d_ref->d_teams.d_spawnTickets[teamId]--;
		d_ref->d_teams.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::OperationEnd(DOOperation* i_operation)
{
	switch(i_operation->GetType())
	{
	 case Operation::ChangeMasterStation:
	 {
		// Update the message owner's master.
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		GetMessageOwner().SetMaster(IsADuplicationMaster());

		// Find the player agent that has the same master station as the
		// referee. The agent's account ID is used to identify the new session
		// master.
		DOHandle masterStation = GetMasterStation();
		DOPlayerAgent::SelectionIterator agent;
		agent.GotoStart();
		while(!agent.EndReached())
		{
			if (agent->GetMasterStation() == masterStation)
			{
				g_messageDispatcher.SendMessageToAll("SessionMasterChanged",
						agent->GetAccountId(), INVALID_OBJECT_HANDLE);
				break;
			}
			agent++;
		}
		ASSERT(!agent.EndReached());

		if (ChangeMasterStationOperation::DynamicCast(i_operation)->IsAPromotion())
			ReconcileParticipants();

		break;
	 }

	 case Operation::AddToStore:
		ASSERT(!ValidSingleton(Referee));
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		SetSingleton(Referee, this);
		GetMessageOwner().SetMaster(IsADuplicationMaster());
		GetMessageOwner().SetSessionID(GetHandleValue());
		break;

	 case Operation::RemoveFromStore:
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		GetMessageOwner().SetSessionID(INVALID_DOHANDLE_VALUE);
		break;
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::Update(void)
{
	Referee::Update();
	if (d_localDOAgent != NULL)
		d_localDOAgent->Update();
	if (IsADuplicationMaster())
	{
		if (d_scoringModel->HasDataChanged())
			d_score.RequestUpdate();
		DuplicatedObject::Update();
	}
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::ValidateParticipant(DuplicatedObject* i_do, uint8 i_id,
		bool &o_isAgent)
{
	if (i_do != NULL)
	{
		if (i_do->IsAKindOf(DOCLASSID(DOPlayerAgent)))
		{
			if ((i_id == INVALID_PARTICIPANT) || (i_id < d_maxPlayers))
			{
				o_isAgent = true;
				return(true);
			}
		}
		else if (i_do->IsAKindOf(DOCLASSID(DOActor)) &&
				((i_id == INVALID_PARTICIPANT) || (i_id < d_maxParticipants)))
		{
			o_isAgent = false;
			return(true);
		}
	}
	ASSERT(false);
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SubmitParticipant(DuplicatedObject* i_do, uint8 i_id,
		NParticipantConfig::TConfig i_config)
{
	// Validate the arguments and identify the type of participant.
	bool isAgent;
	if (!ValidateParticipant(i_do, i_id, isAgent))
		return;

	// If this is the master referee, make appropriate assignments.
	if (IsADuplicationMaster())
	{
		// If the object has not been assigned an id, try to assign one.
		if (i_id == INVALID_PARTICIPANT)
		{
			i_id = FindOpenSlot(i_do->GetHandle(), d_id.d_handle,
					(DOHandle)INVALID_DOHANDLE, isAgent);
			if (i_id != INVALID_PARTICIPANT)
			{
				if (isAgent)
				{
					g_debugOutput.TSPrint(g_debugOutput.Category("NET"),
							"Assigning agent %s[0x%x] to slot %d",
							((DOPlayerAgent*)i_do)->GetAccountName(),
							i_do->GetHandle().GetValue(), i_id);
				}

				d_id.d_handle[i_id] = i_do->GetHandle();
				d_id.RequestUpdate();

				d_assignment.d_config[i_id] = i_config;
				if (isAgent)
				{
					AssignParticipant(i_id, (CPlayerAgent*)i_do);
					NParticipantConfig::SetGhost(d_assignment.d_config[i_id], true);
				}
				else
					AssignParticipant(i_id, ((DOActor*)i_do)->GetActor());
				d_assignment.RequestUpdate();
			}
		}

		// Since the referee is a well-known object and this is the master
		// referee it must be created before any participants (none of which are
		// well-known). The only way for a participant to be submitted to the
		// master referee with an id already assigned is for it to be a player's
		// avatar (and there can only be one of those). If it is an avatar,
		// copy the avatar's alive status over the agent's ready status.
		else if (!isAgent && (i_id < d_maxPlayers) && !d_actor[i_id].Valid())
		{
			bool alive = NParticipantConfig::Active(i_config);
			if (alive != NParticipantConfig::Active(d_assignment.d_config[i_id]))
			{
				NParticipantConfig::SetActive(d_assignment.d_config[i_id], alive);
				d_assignment.RequestUpdate();
			}

			// The agent's and avatar's configuration really should match now.
			ASSERT(i_config == d_assignment.d_config[i_id]);
		}

		// Danger, Will Robinson!
		else
		{
			ASSERT(false);
			return;
		}
	}

	// A participant can be added to a station only after the master referee has
	// assigned it an id. If it is discovered that an id has been assigned at
	// a later time, the participant will be added then.
	if (i_id != INVALID_PARTICIPANT)
	{
		if (isAgent)
			AddAgent((DOPlayerAgent*)i_do, i_id);
		else if (i_id < d_maxPlayers)
			AddAvatar((DOActor*)i_do, i_id);
		else
			AddActor((DOActor*)i_do, i_id);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::UpdateParticipant(DuplicatedObject* i_do, uint8 i_id)
{
	// Validate the arguments and identify the type of participant.
	bool isAgent;
	if (!ValidateParticipant(i_do, i_id, isAgent))
		return;

	// A participant can be added to a station only after the master referee has
	// assigned it an id. If it is discovered that an id has been assigned at
	// a later time, the participant will be added then.
	if (i_id != INVALID_PARTICIPANT)
	{
		if (isAgent)
			AddAgent((DOPlayerAgent*)i_do, i_id);
		else if (i_id < d_maxPlayers)
			AddAvatar((DOActor*)i_do, i_id);
		else
			AddActor((DOActor*)i_do, i_id);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::WithdrawParticipant(DuplicatedObject* i_do, uint8 i_id)
{
	// Validate the arguments and identify the type of participant.
	bool isAgent;
	if (!ValidateParticipant(i_do, i_id, isAgent) ||
			(i_id == INVALID_PARTICIPANT))
		return;

	if (IsADuplicationMaster())
	{
		if (isAgent || (i_id >= d_maxPlayers))
		{
			ASSERT(d_id.d_handle[i_id] == i_do->GetHandle());
			d_id.d_handle[i_id] = INVALID_DOHANDLE;
			d_id.RequestUpdate();

			NParticipantConfig::SetInvalid(d_assignment.d_config[i_id]);
			d_assignment.RequestUpdate();
		}
	}

	if (isAgent)
		RemoveAgent(i_id);
	else if (i_id < d_maxPlayers)
		RemoveAvatar(i_id);
	else
		RemoveActor(i_id);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::UpdateIds(void)
{
	uint8 i;
	for (i = 0; i < d_maxPlayers; i++)
	{
		if ((d_id.d_handle[i] != INVALID_DOHANDLE) &&
				(d_id.d_handle[i] != GetHandle()))
		{
			ASSERT(d_id.d_handle[i].IsAKindOf(DOCLASSID(DOPlayerAgent)));
			DOPlayerAgent::Ref refAgent(d_id.d_handle[i]);
			if (refAgent.IsValid())
			{
				AddAgent(*refAgent, i);
				continue;
			}
		}
		RemoveAgent(i);
	}

	for (i = d_maxPlayers; i < d_maxParticipants; i++)
	{
		if (d_id.d_handle[i] != INVALID_DOHANDLE)
		{
			ASSERT(d_id.d_handle[i].IsAKindOf(DOCLASSID(DOActor)));
			DOActor::Ref refActor(d_id.d_handle[i]);
			if (refActor.IsValid())
			{
				AddActor(*refActor, i);
				continue;
			}
		}
		RemoveActor(i);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::AddAgent(DOPlayerAgent* i_do, uint8 i_id)
{
	if ((d_id.d_handle[i_id] != INVALID_DOHANDLE) &&
			(d_id.d_handle[i_id] != GetHandle()))
	{
#ifdef _DEBUG
		ASSERT(d_id.d_handle[i_id] == i_do->GetHandle());
		DOPlayerAgent::Ref refAgent(d_id.d_handle[i_id]);
		ASSERT(refAgent.IsValid());
		ASSERT((i_do->Id() == INVALID_PARTICIPANT) || (i_do->Id() == i_id));
		ASSERT(!PTR_VALID(d_player[i_id]) || (d_player[i_id] == i_do));
#endif

		// Make sure the id is set. If the agent is a duplica on this station,
		// this call will have no effect but the corresponding call on its master
		// station will eventually work its way here.
		i_do->SetId(i_id);
		if ((i_do->Id() == i_id) && !PTR_VALID(d_player[i_id]))
		{
			d_player[i_id] = i_do;
			g_messageDispatcher.SendMessageToAll("PlayerAdded", (void*)i_id,
					INVALID_OBJECT_HANDLE);
			g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
					INVALID_OBJECT_HANDLE);

			if (IsADuplicationMaster())
			{
				char count[4];
				sprintf(count, "%d", GetNumPlayers());
				g_configmgr.SetOptionByName("numPlayers", count);
				g_configmgr.AcceptChanges(true);
			}

			g_debugOutput.TSPrint(g_debugOutput.Category("NET"),
					"Adding agent %s[0x%x] in slot %d", i_do->GetAccountName(),
					i_do->GetHandle().GetValue(), i_id);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::RemoveAgent(uint8 i_id)
{
	if (PTR_VALID(d_player[i_id]))
	{
		g_messageDispatcher.SendMessageToAll("PlayerRemoved", (void*)i_id,
				INVALID_OBJECT_HANDLE);

		d_player[i_id]->SetId(INVALID_PARTICIPANT);
		if ((d_id.d_handle[i_id] != INVALID_DOHANDLE) &&
				(d_id.d_handle[i_id] != GetHandle()))
			d_player[i_id] = (DOPlayerAgent*)STALE;
		else 
			d_player[i_id] = NULL;

		if (IsADuplicationMaster())
		{
			char count[4];
			sprintf(count, "%d", GetNumPlayers());
			g_configmgr.SetOptionByName("numPlayers", count);
			g_configmgr.AcceptChanges(true);
		}
	}
	else if ((d_id.d_handle[i_id] == INVALID_DOHANDLE) ||
			(d_id.d_handle[i_id] == GetHandle()))
		d_player[i_id] = NULL;
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::AddAvatar(DOActor* i_do, uint8 i_id)
{
	CActor* actor = ((DOActor*)i_do)->GetActor();
	if (actor != NULL)
	{
#ifdef _DEBUG
		CParticipantIF* participantIF =
				(CParticipantIF*)actor->GetInterface(CParticipantIF::GetID());
		ASSERT((participantIF != NULL) &&
				(participantIF->GetParticipantId() == i_id));
		ASSERT(!d_actor[i_id].Valid() ||
				(d_actor[i_id] == CActor::ToHandle(actor)));
#endif
		if (i_do->IsADuplica())
		{
			actor->GetMessageOwner().SetOwnerName("RemoteAvatar",
					PTR_VALID(d_player[i_id]) ? d_player[i_id]->GetAccountName() :
					NULL);
		}
		if (!d_actor[i_id].Valid())
		{
			d_actor[i_id] = CActor::ToHandle(actor);
			g_messageDispatcher.SendMessageToAll("AvatarParticipantAdded",
					(void*)i_id, d_actor[i_id]);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::RemoveAvatar(uint8 i_id)
{
	if (d_actor[i_id].Valid())
	{
		g_messageDispatcher.SendMessageToAll("AvatarParticipantRemoved",
				(void*)i_id, d_actor[i_id]);
		d_actor[i_id].MakeInvalid();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::AddActor(DOActor* i_do, uint8 i_id)
{
	CParticipantIF* participantIF = NULL;
	CActor* actor = ((DOActor*)i_do)->GetActor();
	if (actor != NULL)
	{
		participantIF =
				(CParticipantIF*)actor->GetInterface(CParticipantIF::GetID());
	}
	
	if ((d_id.d_handle[i_id] != INVALID_DOHANDLE) && (participantIF != NULL))
	{
		uint8 nonPlayerIndex = i_id - d_maxPlayers;
#ifdef _DEBUG
		ASSERT(d_id.d_handle[i_id] == i_do->GetHandle());
		DOActor::Ref refActor(d_id.d_handle[i_id]);
		ASSERT(refActor.IsValid());
		ASSERT((participantIF->GetParticipantId() == INVALID_PARTICIPANT) ||
				(participantIF->GetParticipantId() == i_id));
		ASSERT(!PTR_VALID(d_nonPlayer[nonPlayerIndex]) ||
				(d_nonPlayer[nonPlayerIndex] == i_do));
		ASSERT(!d_actor[i_id].Valid() ||
				(d_actor[i_id] == CActor::ToHandle(actor)));
#endif

		// Make sure the id is set. If the actor is a duplica on this station,
		// this call will have no effect but the corresponding call on its master
		// station will eventually work its way here.
		participantIF->SetParticipantId(i_id);
		if ((participantIF->GetParticipantId() == i_id) &&
				!PTR_VALID(d_nonPlayer[nonPlayerIndex]))
		{
			d_nonPlayer[nonPlayerIndex] = i_do;
			d_actor[i_id] = CActor::ToHandle(actor);
			g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
					INVALID_OBJECT_HANDLE);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::RemoveActor(uint8 i_id)
{
	uint8 nonPlayerIndex = i_id - d_maxPlayers;
	if (PTR_VALID(d_nonPlayer[nonPlayerIndex]))
	{
		CActor* actor = d_nonPlayer[nonPlayerIndex]->GetActor();
		if (actor != NULL)
		{
			CParticipantIF* participantIF =
					(CParticipantIF*)actor->GetInterface(CParticipantIF::GetID());
			if (participantIF != NULL)
				participantIF->SetParticipantId(INVALID_PARTICIPANT);
		}
		if (d_id.d_handle[i_id] != INVALID_DOHANDLE)
			d_nonPlayer[nonPlayerIndex] = (DOActor*)STALE;
		else 
			d_nonPlayer[nonPlayerIndex] = NULL;
		d_actor[i_id].MakeInvalid();
	}
	else if (d_id.d_handle[i_id] == INVALID_DOHANDLE)
		d_nonPlayer[nonPlayerIndex] = NULL;
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::ReconcileParticipants(void)
{
	if (IsADuplicationMaster())
	{
		uint8 i;
		bool update = false;

		// Get rid of any invalid agents and avatars.
		for (i = 0; i < d_maxPlayers; i++)
		{
			if ((d_id.d_handle[i] != (DOHandle)INVALID_DOHANDLE) &&
				(d_id.d_handle[i] != GetHandle()))
			{
				DOPlayerAgent::Ref refAgent(d_id.d_handle[i]);
				if (!refAgent.IsValid())
				{
					g_debugOutput.Print(g_debugOutput.Category("REF"),
							"Removing invalid agent to reconcile (slot = %d, "
							"handle = 0x%x)", i, d_id.d_handle[i].GetValue());
					d_id.d_handle[i] = (DOHandle)INVALID_DOHANDLE;
					NParticipantConfig::SetInvalid(d_assignment.d_config[i]);
					RemoveAgent(i);
					RemoveAvatar(i);
					update = true;
				}
			}
		}

		// Get rid of any invalid actors.
		for (i = d_maxPlayers; i < d_maxParticipants; i++)
		{
			if ((d_id.d_handle[i] != (DOHandle)INVALID_DOHANDLE) &&
				(d_id.d_handle[i] != GetHandle()))
			{
				DOActor::Ref refActor(d_id.d_handle[i]);
				if (!refActor.IsValid())
				{
					g_debugOutput.Print(g_debugOutput.Category("REF"),
							"Removing invalid actor to reconcile (slot = %d, "
							"handle = 0x%x)", i, d_id.d_handle[i].GetValue());
					RemoveActor(i);
					d_id.d_handle[i] = (DOHandle)INVALID_DOHANDLE;
					NParticipantConfig::SetInvalid(d_assignment.d_config[i]);
					update = true;
				}
			}
		}

		// Make sure all known agents are accounted for.
		DOPlayerAgent::SelectionIterator agent;
		agent.GotoStart();
		while(!agent.EndReached())
		{
			// Get the agent's id and handle.
			uint8 id = agent->Id();
			DOHandle handle = agent->GetHandle();

			// If the agent hasn't been assigned an id, try to assign one.
			if (id == INVALID_PARTICIPANT)
			{
				id = FindOpenSlot(handle, d_id.d_handle, (DOHandle)INVALID_DOHANDLE,
						true);
			}

			// Really should have a valid id now.
			if (id == INVALID_PARTICIPANT)
			{
				g_debugOutput.Print(g_debugOutput.Category("REF"),
						"Cannot assign a slot to agent 0x%x", handle.GetValue());
				ASSERT(false);
			}
			else if (id >= d_maxPlayers)
			{
				g_debugOutput.Print(g_debugOutput.Category("REF"),
						"Agent 0x%x has bogus id %d", handle.GetValue(), id);
				ASSERT(false);
			}

			// See if the agent is not properly accounted for.
			else if (d_id.d_handle[id] != handle)
			{
				// Force a closed slot open.
				if (d_id.d_handle[id] == GetHandle())
				{
					g_debugOutput.Print(g_debugOutput.Category("REF"),
							"Forcing a closed slot open (%d)", id);
					ASSERT(false);
					d_id.d_handle[id] = (DOHandle)INVALID_DOHANDLE;
				}

				// Add the agent to an open slot.
				if (d_id.d_handle[id] == (DOHandle)INVALID_DOHANDLE)
				{
					g_debugOutput.Print(g_debugOutput.Category("REF"),
							"Adding agent to reconcile (slot = %d, handle = 0x%x)",
							id, handle.GetValue());
					d_id.d_handle[id] = handle;
//					d_assignment.d_config[id] = agent->GetConfig();
					AssignParticipant(id, *agent);
					NParticipantConfig::SetGhost(d_assignment.d_config[id], true);
					AddAgent(*agent, id);
					update = true;
				}

				// The slot is currently filled with a valid agent.
				else
				{
					g_debugOutput.Print(g_debugOutput.Category("REF"),
							"Handle array very confused.");
					ASSERT(false);
				}
			}
			agent++;
		}

		DOActor::SelectionIterator actor;
		actor.GotoStart();
		while(!actor.EndReached())
		{
			CParticipantIF* participantIF =
					CParticipantIF::GetInterface(actor->GetActor());
			if (participantIF != NULL)
			{
				// Get the actor's id and handle.
				uint8 id = participantIF->GetParticipantId();
				DOHandle handle = actor->GetHandle();

				// If the actor hasn't been assigned an id, try to assign one.
				if (id == INVALID_PARTICIPANT)
				{
					id = FindOpenSlot(handle, d_id.d_handle,
							(DOHandle)INVALID_DOHANDLE, false);
					if (id != INVALID_PARTICIPANT)
					{
						g_debugOutput.Print(g_debugOutput.Category("REF"),
								"Adding actor to reconcile (slot = %d, handle = 0x%x)",
								id, handle.GetValue());
						d_id.d_handle[id] = handle;
//						d_assignment.d_config[id] = i_config;
						AssignParticipant(id, actor->GetActor());
						AddActor(*actor, id);
						update = true;
					}
					else
					{
						g_debugOutput.Print(g_debugOutput.Category("REF"),
								"Cannot assign a slot to actor 0x%x", handle.GetValue());
						ASSERT(false);
					}
				}

				// Really should have a valid id now.
				else if (id >= d_maxParticipants)
				{
					g_debugOutput.Print(g_debugOutput.Category("REF"),
							"Actor 0x%x has bogus id %d", handle.GetValue(), id);
					ASSERT(false);
				}

				// Is an avatar.
				else if (id < d_maxPlayers)
				{
					if (!d_actor[id].Valid())
					{
						g_debugOutput.Print(g_debugOutput.Category("REF"),
								"Adding avatar to reconcile (slot = %d, handle = 0x%x)",
								id, handle.GetValue());
//						d_assignment.d_config[id] = foo;
						AddAvatar(*actor, id);
						update = true;
					}
				}

				// See if the non-player actor is not properly accounted for.
				else if (d_id.d_handle[id] != handle)
				{
					// Add the actor to an open slot.
					if (d_id.d_handle[id] == (DOHandle)INVALID_DOHANDLE)
					{
						g_debugOutput.Print(g_debugOutput.Category("REF"),
								"Adding non-player to reconcile (slot = %d, "
								"handle = 0x%x)", id, handle.GetValue());
						d_id.d_handle[id] = handle;
						AddActor(*actor, id);
						update = true;
					}

					// The slot is currently filled with a valid actor.
					else
					{
						g_debugOutput.Print(g_debugOutput.Category("REF"),
								"Handle array very confused.");
						ASSERT(false);
					}
				}
			}
			actor++;
		}

		// Propogate changes.
		if (update)
		{
			d_id.RequestUpdate();
			d_assignment.RequestUpdate();
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::UpdateConfig(uint8 i_id, NParticipantConfig::TConfig i_config)
{
	RequestTeam(i_id, NParticipantConfig::RequestedTeam(i_config));
	SetActive(i_id, NParticipantConfig::Active(i_config));
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::IsParticipationClosed(bool i_player)
{
	return(FindOpenSlot((DOHandle)INVALID_DOHANDLE, d_id.d_handle,
			(DOHandle)INVALID_DOHANDLE, i_player) == INVALID_PARTICIPANT);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::RequestTeam(uint8 i_id, uint8 i_requestedTeam)
{
	if (IsADuplicationMaster() && (i_id < d_maxParticipants))
	{
		if (!NParticipantConfig::IsRequestedTeam(d_assignment.d_config[i_id],
				i_requestedTeam))
		{
			NParticipantConfig::SetRequestedTeam(d_assignment.d_config[i_id],
					i_requestedTeam);
			d_teamingModel->ProcessTeamChanges();
			d_assignment.RequestUpdate();
			g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
					INVALID_OBJECT_HANDLE);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetActive(uint8 i_id, bool i_active)
{
	if (IsADuplicationMaster() && (i_id < d_maxParticipants) &&
			(i_active != NParticipantConfig::Active(d_assignment.d_config[i_id])))
	{
		NParticipantConfig::SetActive(d_assignment.d_config[i_id], i_active);
		d_assignment.RequestUpdate();
		g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged",
				NULL, INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::IsParticipant(uint8 i_id)
{
	if (i_id < d_maxPlayers)
		return(PTR_VALID(d_player[i_id]));

	return((i_id < d_maxParticipants) &&
			PTR_VALID(d_nonPlayer[i_id - d_maxPlayers]));
}

/******************************************************************************/
/******************************************************************************/
uint8 DOReferee::GetNumStartPlayers(void)
{
	return(d_populate.d_numPlayers);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetUnlimitedSpawnTickets(uint8 i_teamId)
{
	if (IsADuplicationMaster() && (i_teamId < MAX_TEAMS))
	{
		d_teams.d_spawnTickets[i_teamId] = 0xFFFF;
		d_teams.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetUnlimitedSpawnTicketsForAll(void)
{
	if (IsADuplicationMaster())
	{
		for (uint8 i = 0; i < MAX_TEAMS; i++)
			d_teams.d_spawnTickets[i] = 0xFFFF;
		d_teams.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetNumSpawnTickets(uint8 i_teamId, uint16 d_numSpawnTickets)
{
	if (IsADuplicationMaster() && (i_teamId < MAX_TEAMS))
	{
		d_teams.d_spawnTickets[i_teamId] = d_numSpawnTickets;
		d_teams.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetNumSpawnTicketsForAll(uint16 d_numSpawnTickets)
{
	if (IsADuplicationMaster())
	{
		for (uint8 i = 0; i < MAX_TEAMS; i++)
			d_teams.d_spawnTickets[i] = d_numSpawnTickets;
		d_teams.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::DoesTeamLimitSpawnTickets(uint8 i_teamId)
{
	if (i_teamId < MAX_TEAMS)
		return(d_teams.d_spawnTickets[i_teamId] != 0xFFFF);
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::RequestSpawnTicket(uint8 i_teamId)
{
	if (i_teamId < MAX_TEAMS)
	{
		if (d_teams.d_spawnTickets[i_teamId] > 0)
		{
			g_messageDispatcher.SendMessageToOwner("RequestSpawnTicket",
					(void*)i_teamId, INVALID_OBJECT_HANDLE, GetMessageOwner());
			return(true);
		}
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
uint16 DOReferee::ParticipantSpawnTicketCount(uint8 i_participantId)
{
	uint8 teamId = GetTeam(i_participantId);
	if (teamId < MAX_TEAMS)
		return(d_teams.d_spawnTickets[teamId]);
	return(0);
}

/******************************************************************************/
/******************************************************************************/
uint16 DOReferee::TeamSpawnTicketCount(uint8 i_teamId)
{
	if (i_teamId < MAX_TEAMS)
		return(d_teams.d_spawnTickets[i_teamId]);
	return(0);
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::IsParticipantTalking(uint8 i_id)
{
	if ((i_id < d_maxPlayers) && PTR_VALID(d_player[i_id]))
	{
		DOPlayerAgent *pAgent = d_player[i_id];
		return( g_CVoiceChat.isPlayerTalking(pAgent));
	}
	return( false );
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::IsParticipantHeadsetConnected(uint8 i_id)
{
	if ((i_id < d_maxPlayers) && PTR_VALID(d_player[i_id]))
	{
		DOPlayerAgent *pAgent = d_player[i_id];
		return(((DOTalker *)pAgent)->d_props.d_headset);
	}
	return( false );
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::IsParticipantVoiceReDirected(uint8 i_id)
{
	if ((i_id < d_maxPlayers) && PTR_VALID(d_player[i_id]))
	{
		DOPlayerAgent *pAgent = d_player[i_id];
		return(((DOTalker *)pAgent)->d_props.d_voiceToSpeakers);
	}
	return( false );
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetRoundsInMatch(uint8 i_roundsInMatch)
{
	if (IsADuplicationMaster())
	{
		d_match.d_roundsInMatch = i_roundsInMatch;
		d_match.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
uint8 DOReferee::GetRoundsInMatch(void)
{
	return(d_match.d_roundsInMatch);
}

/******************************************************************************/
/******************************************************************************/
uint8 DOReferee::GetRoundCount(void)
{
	return(d_match.d_roundCount);
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::IsRoundOpen(void)
{
	return(d_round.d_status != ROUND_CLOSED);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::ResetRoundCount(void)
{
	if (IsADuplicationMaster())
	{
		d_match.d_roundCount = 0;
		d_match.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::IncrementRoundCount(void)
{
	if (IsADuplicationMaster())
	{
		d_match.d_roundCount++;
		d_match.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
CPlayerAgent* DOReferee::CreateLocalPlayerAgent(void)
{
	if (d_localAgent == NULL)
	{
		d_localDOAgent = DOPlayerAgent::Create();
		ASSERT_PTR(d_localDOAgent);
		if (d_localDOAgent != NULL)
		{
			d_localAgent = (CPlayerAgent*)d_localDOAgent;
			d_localDOAgent->Initialize();
			d_localDOAgent->Publish();
		}
	}
	return(d_localAgent);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::DestroyLocalPlayerAgent(void)
{
	if (d_localDOAgent != NULL)
	{
		d_localDOAgent->DeleteMainRef();
		d_localDOAgent = NULL;
		d_localAgent = NULL;
	}
}

/******************************************************************************/
/******************************************************************************/
CPlayerAgent* DOReferee::GetPlayerAgent(uint8 i_id)
{
	if ((i_id < d_maxPlayers) && PTR_VALID(d_player[i_id]))
		return((CPlayerAgent*)d_player[i_id]);

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::ApplyTeamingModel(void)
{
	if (IsADuplicationMaster())
		d_teamingModel->ValidateAssignments();
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::ApplyScoringModel(void)
{
	d_score.SetCallback(d_callback.Set(d_scoringModel,
			&CScoringModel::DataHasChanged));
	if (IsADuplicationMaster())
	{
		d_score.SetSize(d_scoringModel->GetBufferSize());
		d_scoringModel->SetBuffer(d_score.GetBuffer());
		d_scoringModel->InitializeBuffer();
		d_score.RequestUpdate();
		d_scoringModel->SetPlayerInfo(&d_scoringPlayerInfo);
		d_scoringPlayerInfo.RequestUpdate();
	}
	else
	{
		d_scoringModel->SetBuffer(d_score.GetBuffer());
		d_scoringModel->DataHasChanged();
		d_scoringModel->SetPlayerInfo(&d_scoringPlayerInfo);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::UpdateScoringModelBuffer(void)
{
	if (d_scoringModel->GetBuffer() == NULL)
		d_scoringModel->SetBuffer(d_score.GetBuffer());
	d_scoringModel->DataHasChanged();
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetAlphasRole(CTeamingModel::ERole i_eRole)
{
	if (IsADuplicationMaster())
	{
		d_teamingModel->SetAlphasRole(i_eRole);
		d_rules.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetBetasRole(CTeamingModel::ERole i_eRole)
{
	if (IsADuplicationMaster())
	{
		d_teamingModel->SetBetasRole(i_eRole);
		d_rules.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::AssignAlphaToBlueTeam()
{
	if (IsADuplicationMaster())
	{
		d_teamingModel->AssignAlphaToBlueTeam();
		d_rules.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::AssignBetaToBlueTeam()
{
	if (IsADuplicationMaster())
	{
		d_teamingModel->AssignBetaToBlueTeam();
		d_rules.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::AssignAlphaToRedTeam()
{
	if (IsADuplicationMaster())
	{
		d_teamingModel->AssignAlphaToRedTeam();
		d_rules.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::AssignBetaToRedTeam()
{
	if (IsADuplicationMaster())
	{
		d_teamingModel->AssignBetaToRedTeam();
		d_rules.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
int DOReferee::GetSessionTeamWins(uint8 i_teamId)
{
	if (d_teamingModel->IsTeamInGroupAlpha(i_teamId))
		return(d_session.d_alphaWins);
	else if (d_teamingModel->IsTeamInGroupBeta(i_teamId))
		return(d_session.d_betaWins);
	return(0);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::IncrementSessionAlphaWins()
{
	if (IsADuplicationMaster())
	{
		d_session.d_alphaWins++;
		d_session.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::IncrementSessionBetaWins()
{
	if (IsADuplicationMaster())
	{
		d_session.d_betaWins++;
		d_session.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::DirectPhaseExit(EPhase i_phase,
		EPhaseChangeDirection i_direction)
{
	if (IsADuplicationMaster())
	{
		d_match.d_directPhaseExit[i_phase] = (uint8)i_direction;
		d_match.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetPhaseEndTime(EPhase i_phase, int32 i_time)
{
	if (IsADuplicationMaster())
	{
		d_match.d_phaseEndTime[i_phase] = i_time;
		d_match.RequestUpdate();
		ComputeCurrentPhaseEndTime();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetPhaseLifetime(EPhase i_phase, int32 i_time)
{
	if (IsADuplicationMaster())
	{
		d_match.d_phaseLifetime[i_phase] = i_time;
		d_match.RequestUpdate();
		ComputeCurrentPhaseEndTime();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::UpdatePhase(void)
{
	if (IsADuplicationMaster())
	{
		// Get the current time and assume there will be no phase change.
		int32 currentTime = g_session.GetSessionTimeMS();
		int32 changeTime = d_currentPhaseStartTime;
		EPhase nextPhase = d_phase;

		// Manage the populating phase. Only change phase if it says it's OK.
		if (ManagePopulate())
		{
			// Orphan sessions want to exit as soon as possible.
			if (g_lobby.IsOrphanSession())
				d_returnToReady = true;

			// If the current phase is the same as the next phase propagated to the
			// duplicas, then a phase change has yet to be approved. If that is the
			// case or, if there is some time left before the phase was to end, the
			// need for a change needs to be determined.
			EPhaseChangeDirection direction;
			if ((d_phase == (EPhase)d_nextPhase.d_value) ||
					(currentTime < (d_nextPhase.d_changeTime - CHANGE_TIME_BUFFER)))
			{
				// Get the desired next phase and the strength of that desire.
				direction = GetPhaseChangeDirection(nextPhase);

				// Don't try to leave or bypass the ready phase if the teams are not
				// valid.
				if (((d_phase == READY) || (nextPhase == SETUP)) &&
						((direction != NO_CHANGE) && !d_teamingModel->ValidTeams()))
				{
					// Tell the user if actually trying to change the phase.
					if ((direction == CHANGE_REQUIRED) ||
							((direction == CHANGE_ALLOWED) &&
							(currentTime > d_currentPhaseEndTime)))
					{
						g_messageDispatcher.SendMessageToOwner("FixTeams", NULL,
								INVALID_OBJECT_HANDLE, &GetMessageOwner());
					}

					// Want to be in the ready phase.
					nextPhase = READY;

					// Don't indicate a change if there won't be one.
					if (d_phase == READY)
						direction = NO_CHANGE;
				}
			}

			// If the current phase is not the same as the next phase propagated to
			// the duplicas, then a phase change was approved in a previous frame.
			// Don't allow the referee to back track if that is the case and it is
			// close to the phase end time.
			else
			{
				nextPhase = (EPhase)d_nextPhase.d_value;
				direction = CHANGE_ALLOWED;
			}

			// If a change is allowed but the time has not expired, the rest of the
			// stations will be told what that phase is and when it should start.
			// This way, phases can start in unison.
			if ((direction == CHANGE_ALLOWED) &&
					(currentTime <= d_currentPhaseEndTime))
				changeTime = d_currentPhaseEndTime;

			// A change should occur if it is required or if it is allowed and the
			// current phase's time has expired.
			else if (direction != NO_CHANGE)
			{
				// Take care of business in phases being left.
				switch(d_phase)
				{
				 case READY:
					g_configmgr.SetOptionByName("readyUp", "0");
					g_configmgr.AcceptChanges(true);
					break;
				 case SETUP:
					break;
				 case POPULATING:
					LogMessage("Populate", "Leave", "LogLock");
					break;
				 case REPOPULATE:
					break;
				 case PLAYING:
					break;
				 case REPORTING:
					ClearPlayers();
					break;
				}

				// Set the new phase and note the new start time.
				SetPhase(nextPhase);
				changeTime = d_currentPhaseStartTime;

				// Take care of business in phases being left.
				switch(d_phase)
				{
				 case READY:
					g_configmgr.SetOptionByName("readyUp", "1");
					g_configmgr.AcceptChanges(true);
					break;
				 case SETUP:
					break;
				 case POPULATING:
					IdentifyPlayers();
					LogMessage("Referee", "PopulatePrep", "LogEntry");
					LogMessage("Populate", "Enter", "LogLock");
					break;
				 case REPOPULATE:
					break;
				 case PLAYING:
					break;
				 case REPORTING:
					break;
				}
			}
		}

		// Update duplicas if there is a change.
		if (((EPhase)d_nextPhase.d_value != nextPhase) ||
				((d_nextPhase.d_changeTime != changeTime) &&
				((d_nextPhase.d_changeTime > currentTime) ||
				(changeTime > currentTime))) ||
				(d_nextPhase.d_assignmentId != d_assignment.d_assignmentId))
		{
			uint8 category = g_debugOutput.Category("REF");
			g_debugOutput.TSPrint(category, "Sending phase update (%s -> %s)",
					GetPhaseName((EPhase)d_nextPhase.d_value),
					GetPhaseName(nextPhase));
			if ((d_nextPhase.d_changeTime != changeTime) &&
					((d_nextPhase.d_changeTime > currentTime) ||
					(changeTime > currentTime)))
				g_debugOutput.TSPrint(category,
						"  Time: Old change = %d, New Change = %d, Current = %d",
						d_nextPhase.d_changeTime, changeTime, currentTime);
			if (d_nextPhase.d_assignmentId != d_assignment.d_assignmentId)
				g_debugOutput.TSPrint(category, "  Assignment id (%d -> %d)",
						d_nextPhase.d_assignmentId, d_assignment.d_assignmentId);

			d_nextPhase.d_value = (uint8)nextPhase;
			d_nextPhase.d_changeTime = changeTime;
			d_nextPhase.d_assignmentId = d_assignment.d_assignmentId;
			d_nextPhase.RequestUpdate();

			char message[40];
			sprintf(message, "%s (%d)", GetPhaseName(nextPhase), changeTime);
			LogMessage("Send", message, "LogEntry");
		}
	}

	// The phase value is the current phase.
	else if (g_session.GetSessionTimeMS() > d_nextPhase.d_changeTime)
		SetPhase((EPhase)d_nextPhase.d_value, d_nextPhase.d_changeTime);

	// The phase value is the next phase.
	else
	{
		switch((EPhase)d_nextPhase.d_value)
		{
		 case READY:
			SetPhase(REPORTING);
			break;

		 case SETUP:
			SetPhase(REPORTING);
			break;

		 case POPULATING:
			if (d_match.d_roundCount <= 1)
				SetPhase(READY);
			else
				SetPhase(SETUP);
			break;

		 case REPOPULATE:
			SetPhase(POPULATING);
			break;

		 case PLAYING:
			SetPhase(POPULATING);
			break;

		 case REPORTING:
			SetPhase(PLAYING);
			break;
		}
		d_currentPhaseEndTime = d_nextPhase.d_changeTime;
	}
}

/******************************************************************************/
/******************************************************************************/
Referee::EPlayersStatus DOReferee::DeterminePlayersStatus(void)
{
	bool foundUnloaded = false;
	bool foundLoaded = false;
	bool allReady = true;

	DOPlayerAgent::SelectionIterator player;
	player.GotoStart();
	while(!player.EndReached())
	{
		switch (player->GetMode())
		{
		 case CPlayerAgent::INACTIVE:
			break;

		 case CPlayerAgent::UNLOADED:
			if (foundLoaded)
				return(OTHER);
			foundUnloaded = true;
			break;

		 case CPlayerAgent::LOADED:
		 {
			if (foundUnloaded)
				return(OTHER);
			foundLoaded = true;
			uint8 id = player->Id();
			ASSERT(id < d_maxPlayers);
			if (id < d_maxPlayers)
			{
				allReady = allReady &&
						(NParticipantConfig::Active(d_assignment.d_config[id]) &&
						NParticipantConfig::IsRequestedTeam(d_assignment.d_config[id],
						NParticipantConfig::AssignedTeam(d_assignment.d_config[id])));
			}
			break;
		 }

		 default:
			return(OTHER);
		}
		player++;
	}

	if (foundUnloaded)
		return(ALL_UNLOADED);
	if (!foundLoaded)
		return(ALL_INACTIVE);
	if (allReady)
		return(ALL_READY);
	return(ALL_LOADED);
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::ManagePopulate(void)
{
	if (d_phase == POPULATING)
	{
		// Don't change phase if the lobby is busy.
		if (g_lobby.IsRequestPending())
			return(false);

		// Count the number of players and see if they are all published.
		DOPlayerAgent* levelAgent = NULL;
		bool inconsistentCRC = false;
		d_allPlayersPublished = (d_populate.d_crc != 0);
		uint8 numPlayers = 0;
		DOPlayerAgent::SelectionIterator player;
		player.GotoStart();
		while(!player.EndReached())
		{
			uint8 id = player->Id();
			if (NParticipantConfig::NotGhost(d_assignment.d_config[id]))
			{
				numPlayers++;
				d_allPlayersPublished = d_allPlayersPublished &&
						(player->GetMode() == CPlayerAgent::PUBLISHED);
			}

			uint32 playerCRC = player->GetCRC();
			if (playerCRC != 0)
			{
				if (levelAgent == NULL)
					levelAgent = *player;
				else if (playerCRC != levelAgent->GetCRC())
					inconsistentCRC = true;
			}

			player++;
		}
		d_populateFailed = d_allPlayersPublished && (inconsistentCRC ||
				(numPlayers != d_populate.d_numPlayers));

		// Make sure a round is open and started.
		if (d_round.d_status == ROUND_CLOSED)
		{
			LogMessage("Populate", "ReqOpen", "LogLock");
			g_lobby.OpenRound();
		}
		else if ((d_round.d_status == ROUND_OPENED) && d_allPlayersPublished &&
				!d_populateFailed)
		{
			LogMessage("Populate", "ReqStart", "LogLock");
			g_lobby.StartRound();
		}

		// Entering populate and opening a round kicks off a process that results
		// in the world getting loaded somewhere. Once that is complete, assign
		// world actors to players who will create and publish them.
		if ((d_populate.d_crc == 0) && (levelAgent != NULL))
		{
			// Set some of the populate dataset.
			d_populate.d_crc = levelAgent->GetCRC();
			d_populate.d_numPlayers = numPlayers;

			// Assign a range of actors to each players.
			uint8 category = g_debugOutput.Category("REF");
			g_debugOutput.TSPrint(category, "Configuring populate");
			int start = 0;
			player.GotoStart();
			while(!player.EndReached())
			{
				uint8 id = player->Id();
				if (NParticipantConfig::NotGhost(d_assignment.d_config[id]))
				{
					d_populate.d_start[id] = start;
					start += (levelAgent->GetDistributedActorCount() - start) /
							numPlayers;
					d_populate.d_end[id] = start - 1;
					numPlayers--;
					g_debugOutput.TSPrint(category, "  Agent %s (0x%x, %d) - "
							"Start: %d, End: %d", player->GetAccountName(),
							player->GetHandleValue(), id, d_populate.d_start[id],
							d_populate.d_end[id]);
				}
				player++;
			}
			d_populate.RequestUpdate();
			SetWorldExtents();

			LogMessage("Populate", "Assign", "LogLock");
		}
	}
	else if (d_populate.d_crc != 0)
	{
		// Reset the populate dataset.
		d_populate.d_crc = 0;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			d_populate.d_start[i] = -1;
			d_populate.d_end[i] = -1;
		}
		d_populate.RequestUpdate();

		g_debugOutput.TSPrint(g_debugOutput.Category("REF"), "End populate");
		LogMessage("Populate", "Unassign", "LogLock");
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::IdentifyPlayers(void)
{
	uint8 category = g_debugOutput.Category("REF");
	g_debugOutput.TSPrint(category, "Identifying ready players");

	DOPlayerAgent::SelectionIterator player;
	player.GotoStart();
	while(!player.EndReached())
	{
		uint8 id = player->Id();
		NParticipantConfig::SetGhost(d_assignment.d_config[id],
				(!NParticipantConfig::Active(d_assignment.d_config[id]) ||
				!NParticipantConfig::IsRequestedTeam(d_assignment.d_config[id],
				NParticipantConfig::AssignedTeam(d_assignment.d_config[id]))));
		g_debugOutput.TSPrint(category, "  Agent %s (0x%x, %d) is %s",
				player->GetAccountName(), player->GetHandleValue(), id,
				NParticipantConfig::Ghost(d_assignment.d_config[id]) ?
				"a ghost" : "ready");
		player++;
	}
	if (d_assignment.d_assignmentId < 0xFF)
		d_assignment.d_assignmentId++;
	else
		d_assignment.d_assignmentId = 1;
	d_assignment.RequestUpdate();
	g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::ClearPlayers(void)
{
	DOPlayerAgent::SelectionIterator player;
	player.GotoStart();
	while(!player.EndReached())
	{
		uint8 id = player->Id();
		if (NParticipantConfig::NotGhost(d_assignment.d_config[id]))
		{
			NParticipantConfig::SetGhost(d_assignment.d_config[id], true);
			NParticipantConfig::SetActive(d_assignment.d_config[id], true);
		}
		player++;
	}
	if (d_assignment.d_assignmentId < 0xFF)
		d_assignment.d_assignmentId++;
	else
		d_assignment.d_assignmentId = 1;
	d_assignment.RequestUpdate();
	g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::CanLoadWorldActors(int &o_start, int &o_end)
{
	if ((d_localAgent != NULL) && (d_populate.d_crc != 0) && g_world.IsSet())
	{
		if ((d_worldLoader == NULL) ||
				(d_worldLoader->GetCRC() != d_populate.d_crc))
		{
			g_errorStack.SetError("Session", "Wrong level loaded",
					CErrorStack::ERROR_FATAL, 5000);
			return(false);
		}
		uint8 id = d_localAgent->Id();
		if (id < MAX_PLAYERS)
		{
			o_start = d_populate.d_start[id];
			o_end = d_populate.d_end[id];
			return(true);
		}
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::SetWorldExtents(void)
{
	if (d_populate.d_crc != 0)
	{
		DOPlayerAgent::SelectionIterator player;
		player.GotoStart();
		while(!player.EndReached())
		{
			if (player->GetCRC() == d_populate.d_crc)
			{
				player->SetWorldExtents();
				return;
			}
			player++;
		}
		g_errorStack.SetError("Session", "Inconsistent level loaded",
				CErrorStack::ERROR_FATAL, 5000);
	}
}

/******************************************************************************/
/******************************************************************************/
bool DOReferee::IsConsistent(void)
{
	return(d_nextPhase.d_assignmentId == d_assignment.d_assignmentId);
}

/******************************************************************************/
/******************************************************************************/
CMessageOwner* DOReferee::MessageOwner(void)
{
	return(&GetMessageOwner());
}

/******************************************************************************/
/******************************************************************************/
int DOReferee::GetPlayerAgentCount(void)
{
	int iCount = 0;
	for (int i = 0; i < d_maxPlayers; ++i)
	{
		if ((d_id.d_handle[i] != INVALID_DOHANDLE) &&
				(d_id.d_handle[i] != GetHandle()))
			++iCount;
	}
	return(iCount);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::CountPlayerSlots(uint8 &o_filled, uint8 &o_open, uint8 &o_closed)
{
	// Use the referee's handle to indicate a closed slot since it is a handle
	// that no participant will ever have.
	Referee::CountPlayerSlots(d_id.d_handle, (DOHandle)INVALID_DOHANDLE,
			GetHandle(), o_filled, o_open, o_closed);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::ClosePlayerSlots(uint8 i_numToClose)
{
	if (IsADuplicationMaster())
	{
		Referee::ClosePlayerSlots(i_numToClose, d_id.d_handle,
				(DOHandle)INVALID_DOHANDLE, GetHandle());
		d_id.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::OpenPlayerSlots(uint8 i_numToOpen)
{
	if (IsADuplicationMaster())
	{
		Referee::OpenPlayerSlots(i_numToOpen, d_id.d_handle,
				(DOHandle)INVALID_DOHANDLE, GetHandle());
		d_id.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
#define CONSTANT_MASK  0xfff0000f
#define CONSTANT_VALUE 0x06800001
#define CONSTANT_SHIFT 4
static const char* ParseHandle(uint32 i_handle)
{
	static char result[11];
	if (i_handle == INVALID_DOHANDLE_VALUE)
		strcpy(result, "");
	else if ((i_handle & CONSTANT_MASK) == CONSTANT_VALUE)
	{
		uint value = (i_handle & ~CONSTANT_MASK) >> CONSTANT_SHIFT;
		sprintf(result, "%u", value);
	}
	else
		sprintf(result, "%u", i_handle);
	return(result);
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::CLog::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (Referee::s_logAgents && !d_ref->d_logFile.IsInvalid())
	{
		if (!d_ref->d_logFile.IsOpen())
		{
			char rootName[] = "Agents.txt";
			const char* name = g_logUtil.Enabled() ?
					g_logUtil.MakeFullName(rootName) : rootName;
			if (!d_ref->d_logFile.Open(name, 3 + (3 * d_ref->d_maxPlayers)))
				return;

			char header[10];
			d_ref->d_logFile.SetField(0, "Time");
			d_ref->d_logFile.SetField(1, "Label");
			d_ref->d_logFile.SetField(2, "Subject");
			for (int i = 0; i < d_ref->d_maxPlayers; i++)
			{
				int j = 3 + (i * 3);
				sprintf(header, "Handle%d", i);
				d_ref->d_logFile.SetField(j, header);

				j++;
				sprintf(header, "Player%d", i);
				d_ref->d_logFile.SetField(j, header);

				j++;
				sprintf(header, "Ready%d", i);
				d_ref->d_logFile.SetField(j, header);
			}
			d_ref->d_logFile.Print();
		}

		SLogMessage* logMessage = (SLogMessage*)i_data;
		d_ref->d_logFile.SetField(0, (int)(ValidSingleton(CSessionTime) ?
				g_session.GetSessionTimeMS() : 0));
		d_ref->d_logFile.SetField(1, logMessage->d_label);
		switch(logMessage->d_type)
		{
		 case SLogMessage::LOG_SUBJECT:
			d_ref->d_logFile.SetField(2, ParseHandle(logMessage->d_subject));
			break;
		 case SLogMessage::LOG_ACTION:
			d_ref->d_logFile.SetField(2, logMessage->d_action);
			break;
		}

		for (int i = 0; i < d_ref->d_maxPlayers; i++)
		{
			if ((d_ref->d_id.d_handle[i] != INVALID_DOHANDLE) &&
					(d_ref->d_id.d_handle[i] != d_ref->GetHandle()))
			{
				DuplicatedObject::Ref ref(d_ref->d_id.d_handle[i]);

				int j = 3 + (3 * i);
				d_ref->d_logFile.SetField(j++,
						(const char*)(!ref.IsValid() ? "INVALID" :
						ParseHandle(d_ref->d_id.d_handle[i].GetValue())));
				d_ref->d_logFile.SetField(j++,
						(const char*)((d_ref->d_player[i] == NULL) ? "NULL" :
						((d_ref->d_player[i] == STALE) ?
						"STALE" : d_ref->d_player[i]->GetAccountName())));
				d_ref->d_logFile.SetField(j, (const char*)(
						NParticipantConfig::Active(d_ref->d_assignment.d_config[i]) ?
						"*" : ""));
			}
		}
		d_ref->d_logFile.Print();
	}
}

/******************************************************************************/
/******************************************************************************/
template<int LENGTH>
class CValidateString
{
  public:
	CValidateString() {d_string[0] = '\0';}
	void Clear(void) {d_string[0] = '\0';}
	void AddLine(const char* i_line);
	const char* Get(void) {return((d_string[0] == '\0') ? NULL : d_string);}

  private:
	char d_string[LENGTH];
};

/******************************************************************************/
/******************************************************************************/
template<int LENGTH>
void CValidateString<LENGTH>::AddLine(const char* i_line)
{
	if ((i_line != NULL) && (i_line[0] != '\0'))
	{
		int length = strlen(d_string) + strlen(i_line);
		if (d_string[0] != '\0')
			length++;
		if (length < LENGTH)
		{
			if (d_string[0] != '\0')
				strcat(d_string, "\n");
			strcat(d_string, i_line);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
const char* DOReferee::Validate(void)
{
	// Set up the validate string.
	static CValidateString<256> validateString;
	validateString.Clear();

	// Validate each known agent.
	DOPlayerAgent::SelectionIterator agent;
	agent.GotoStart();
	while(!agent.EndReached())
	{
		// When an agent is created and published, its id is INVALID_PARTICIPANT.
		// Not much can be done to validate it until it is assigned an id.
		uint8 id = agent->Id();
		if (id < d_maxPlayers)
		{
			// When the referee master detects it, the agent is added to the handle
			// array. When the agent sees its handle in the array, it changes its
			// id to be its index into the array. All stations should see a new
			// entry in the handle array and they certainly should see the change
			// in the array before they see the change in the agent's id. Make sure
			// the agent is in its assigned slot.
			DOHandle handle = agent->GetHandle();
			if (d_id.d_handle[id] != handle)
			{
				char msg[80];
				sprintf(msg, "%s[0x%x] not in slot %d (0x%x is)",
						agent->GetAccountName(), handle.GetValue(), id,
						d_id.d_handle[id].GetValue());
				validateString.AddLine(msg);
			}

			// Make sure each agent only appears once in the handle array.
			for (uint8 i = 0; i < d_maxPlayers; i++)
			{
				if ((d_id.d_handle[i] == handle) && (i != id))
				{
					char msg[80];
					sprintf(msg, "%s[0x%x] assigned slot %d and in slot %d",
							agent->GetAccountName(), handle.GetValue(), id,
							i);
					validateString.AddLine(msg);
				}
			}
		}

		// Check for bogus data while we are here.
		else if (id != INVALID_PARTICIPANT)
		{
			char msg[80];
			sprintf(msg, "%s[0x%x] has bogus id (%d)", agent->GetAccountName(),
					agent->GetHandle().GetValue(), id);
			validateString.AddLine(msg);
		}
		agent++;
	}

	// Make sure there is an agent for each valid entry in the handle array.
	for (uint8 i = 0; i < d_maxPlayers; i++)
	{
		if ((d_id.d_handle[i] != (DOHandle)INVALID_DOHANDLE) &&
				(d_id.d_handle[i] != GetHandle()))
		{
			DOPlayerAgent::Ref refAgent(d_id.d_handle[i]);
			if (!refAgent.IsValid())
			{
				char msg[80];
				sprintf(msg, "Invalid agent[0x%x] in slot %d",
						d_id.d_handle[i].GetValue(), i);
				validateString.AddLine(msg);
			}
		}
	}

	// If any problems are found, dump unassigned agents for good measure.
	if (validateString.Get() != NULL)
	{
		agent.GotoStart();
		while(!agent.EndReached())
		{
			if (agent->Id() == INVALID_PARTICIPANT)
			{
				char msg[80];
				sprintf(msg, "%s[0x%x] is unassigned", agent->GetAccountName(),
						agent->GetHandle().GetValue());
				validateString.AddLine(msg);
			}
			agent++;
		}
	}

	return(validateString.Get());
}

/******************************************************************************/
/******************************************************************************/
void DOReferee::DumpAgentList(void)
{
	DOPlayerAgent::SelectionIterator agent;
	agent.GotoStart();
	while(!agent.EndReached())
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("REF"),
				"Name: %s, Id: %d, Handle: 0x%x\n", agent->GetAccountName(),
				agent->Id(), agent->GetHandle().GetValue());
		agent++;
	}
}
