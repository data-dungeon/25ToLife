/*
**
**  File:   RobberyScoringModel.cpp
**  Date:   December 27, 2004
**  By:     Bryce Thomsen
**  Desc:   A scoring model that keeps track of kills and raid objective.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/RobberyScoringModel.cpp $
**  $Revision: #50 $
**  $DateTime: 2005/08/30 17:42:01 $
**    $Author: Bryce $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "sim/RobberyScoringModel.h"

// Utility header.
#include "environ/ConfigMgr.h"

// Setup score size.
#define MAX_SCORE ((CScoringModel::TScore)~0)

/******************************************************************************/
/******************************************************************************/
SCORING_MODEL_CREATOR(CRobberyScoringModel, Robbery);

/******************************************************************************/
/******************************************************************************/
CRobberyScoringModel::CRobberyScoringModel(uint8 i_maxPlayers, uint8 i_maxParticipants) :
	CScoringModel(i_maxPlayers, i_maxParticipants),
	d_messageOwner("RobberyScoringModel")
{
	d_startReadyUpHandler.Initialize("ReadyUpStarting", MESSAGE_HANDLE_IF_MASTER, 
		g_referee.GetMessageOwner(), this);
	d_beginRoundHandler.Initialize("BeginRound", MESSAGE_HANDLE_IF_MASTER, 
		g_referee.GetMessageOwner(), this);
	d_endRoundHandler.Initialize("EndRound", MESSAGE_HANDLE_IF_MASTER,
		g_referee.GetMessageOwner(), this);
	d_killHandler.Initialize("RecordKill", MESSAGE_HANDLE_IF_MASTER |
		MESSAGE_ROUTE_TO_MASTER, g_referee.GetMessageOwner(), this);
	d_apprehendHandler.Initialize("RecordApprehend", MESSAGE_HANDLE_IF_MASTER |
		MESSAGE_ROUTE_TO_MASTER, g_referee.GetMessageOwner(), this);
	//d_objectiveTakenHandler.Initialize("ObjectiveTaken", MESSAGE_HANDLE_IF_MASTER, 
	//	g_referee.GetMessageOwner(), this);
	//d_objectiveReturnedHandler.Initialize("ObjectiveReturned", MESSAGE_HANDLE_IF_MASTER, 
	//	g_referee.GetMessageOwner(), this);
	d_objectiveCapturedHandler.Initialize("ObjectiveCaptured", MESSAGE_HANDLE_IF_MASTER, 
		g_referee.GetMessageOwner(), this);
	d_sessionMasterChangedHandler.Initialize("SessionMasterChanged",
		MESSAGE_HANDLE_IF_MASTER, g_referee.GetMessageOwner(), this);

	HasChanged( false );
}

/******************************************************************************/
/******************************************************************************/
CRobberyScoringModel::~CRobberyScoringModel()
{
	d_messageOwner.UnregisterAll();
}

/******************************************************************************/
/******************************************************************************/
CScoringModel::ETeamStatus CRobberyScoringModel::GetTeamStatus(uint8 i_teamId)
{
	const CTeamingModel& teaming = g_referee.GetTeamingModel();

	// round is still going
	if (g_referee.GetPhase() == g_referee.PLAYING)
	{
		// Test for standard team point comparison.
		{
			int us = GetTeamCount(TOTAL_SCORE, i_teamId);
			int them = 0;
			if (teaming.IsTeamInGroupAlpha( i_teamId ))
				them = GetTeamCount(TOTAL_SCORE, teaming.FindTeamInGroupBeta());
			else
				them = GetTeamCount(TOTAL_SCORE, teaming.FindTeamInGroupAlpha());

			if (us < them)
				return LOSING;
			else if (us > them)
				return WINNING;
			else
				return TIED;
		}
	}
	// End of round.
	else
	{
		// Test to see if objective was captured.
		if (IsTeamRed(i_teamId) && IsObjectiveAchieved(RED_CAPTURED_OBJ))
			return WON;
		else if (!IsTeamRed(i_teamId) && IsObjectiveAchieved(RED_CAPTURED_OBJ))
			return LOST;

		// Test for round with team spawn ticket limit in place.
		{
			uint8 themTeam;
			if (teaming.IsTeamInGroupAlpha(i_teamId))
				themTeam = teaming.FindTeamInGroupBeta();
			else
				themTeam = teaming.FindTeamInGroupAlpha();
			if ((g_referee.TeamSpawnTicketCount(i_teamId) == 0) && IsTeamEliminated(i_teamId))
			{
				if ((g_referee.TeamSpawnTicketCount(themTeam) == 0) && IsTeamEliminated(themTeam))
				{
					int usDeaths = GetTeamCount(ALL_DEATHS, i_teamId);
					int themDeaths = GetTeamCount(ALL_DEATHS, themTeam);

					// Gangster apprehensions count as deaths.
					if (IsTeamRed(i_teamId))
						usDeaths += GetTeamCount(APPREHENDS, i_teamId);
					else if (IsTeamRed(themTeam))
						themDeaths += GetTeamCount(APPREHENDS, themTeam);

					if (usDeaths > themDeaths)
						return LOST;
					else if (usDeaths < themDeaths)
						return WON;
				}
				else
					return LOST;
			}
			else if ((g_referee.TeamSpawnTicketCount(themTeam) == 0) && IsTeamEliminated(themTeam))
				return WON;
		}

		// Test to see if objective was not captured.
		{
			if (!IsTeamRed(i_teamId) && !IsObjectiveAchieved(RED_CAPTURED_OBJ))
				return WON;
			else if (IsTeamRed(i_teamId) && !IsObjectiveAchieved(RED_CAPTURED_OBJ))
				return LOST;
		}

		// We should never reach this...
		ASSERT(false);
		return LOST;
	}
}

/******************************************************************************/
/******************************************************************************/
int CRobberyScoringModel::GetTotalScore(uint8 i_id)
{
	int score = (2 * GetEnemyKills(i_id));
	score -= (2 * GetTeamKills(i_id)) + GetDeathByEnemy(i_id) + (2 * GetSuicides(i_id));
	if ((g_referee.GetTeamingModel().IsPlayerInGroupAlpha(i_id) && IsAlphaBlue()) ||
		(g_referee.GetTeamingModel().IsPlayerInGroupBeta(i_id) && !IsAlphaBlue()))
	{
		score += (4 * GetApprehensions(i_id)) - GetNonPlayerKills(i_id);
	}
	else if ((g_referee.GetTeamingModel().IsPlayerInGroupAlpha(i_id) && !IsAlphaBlue()) ||
		(g_referee.GetTeamingModel().IsPlayerInGroupBeta(i_id) && IsAlphaBlue()))
	{
		score += (5 * GetObjectives(i_id)) - GetApprehensions(i_id);
	}
	return(score);
}

/******************************************************************************/
/******************************************************************************/
void CRobberyScoringModel::CStartReadyUp::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	// Clear out our match instance data
	d_ref->ResetTeamWins();
	d_ref->ResetTeamBonus();
	d_ref->ResetObjectiveAchieved();
	d_ref->HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CRobberyScoringModel::CBeginRound::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	// Store all the active players' info
	d_ref->PopulatePlayerInfo();

	// Clear out all round instance data
	for ( int i = 0; i < ( d_ref->d_maxPlayers + NUM_SUPPLEMENTARY_ROWS ); ++i )
	{
		for ( int j = 0; j < ( d_ref->d_maxPlayers + NUM_SUPPLEMENTARY_COLUMNS ); ++j )
		{
			d_ref->Score( i, j ) = 0;
		}
	}
	d_ref->ResetForNewRound();
	d_ref->HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CRobberyScoringModel::CEndRound::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	CTeamingModel& team = g_referee.GetTeamingModel();

	// Was either team eliminated
	if (team.IsTeamAllPermanentlyDead( team.FindTeamInGroupAlpha() ))
	{
		if (team.IsAlphaBlue())
			d_ref->SetObjectiveAchieved(BLUE_ELIMINATED);
		else if (team.IsAlphaRed())
			d_ref->SetObjectiveAchieved(RED_ELIMINATED);
	}
	if (team.IsTeamAllPermanentlyDead( team.FindTeamInGroupBeta() ))
	{
		if (team.IsBetaBlue())
			d_ref->SetObjectiveAchieved(BLUE_ELIMINATED);
		else if (team.IsBetaRed())
			d_ref->SetObjectiveAchieved(RED_ELIMINATED);
	}
	// If either team won record it
	if (d_ref->GetTeamStatus( team.FindTeamInGroupAlpha() ) == d_ref->WON)
		d_ref->IncrementAlphaWins();
	else if (d_ref->GetTeamStatus( team.FindTeamInGroupBeta() ) == d_ref->WON)
		d_ref->IncrementBetaWins();
	d_ref->SetState( TABULATED );
	d_ref->HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CRobberyScoringModel::CKill::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	// Don't modify the buffer if we're not recording.
	if (!d_ref->IsRecording())
		return;

	// Get the victim and verify it's valid. Don't forget about NPCs.
	uint8 victim = (uint8)(0xFF & (uint32)i_data);
	if ((victim == INVALID_PARTICIPANT) || (victim >= d_ref->d_maxParticipants))
		return;
	else if (victim >= d_ref->d_maxPlayers)
		victim = d_ref->d_maxPlayers;

	// Get the attacker and verify it's valid.
	uint8 attacker = (uint8)(0xFF & ((uint32)i_data >> 8));
	if (attacker == INVALID_PARTICIPANT)
		attacker = victim;
	else if (attacker >= d_ref->d_maxPlayers)
		return;

	// Record and display a valid kill.
	ASSERTF(d_ref->Score(attacker, victim) < MAX_SCORE,
		("Referee: %d killing %d too many times", attacker, victim));
	if (d_ref->Score(attacker, victim) < MAX_SCORE)
	{
		d_ref->ShowKillHudMessage(victim, attacker);
		d_ref->Score(attacker, victim)++;
		d_ref->HasChanged( true );
	}
}

/******************************************************************************/
/******************************************************************************/
void CRobberyScoringModel::CApprehend::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	// Don't modify the buffer if we're not recording.
	if (!d_ref->IsRecording())
		return;

	// Get the victim and verify it's valid.
	uint8 victim = CParticipantIF::GetParticipantIdFromActor(i_sender);
	if ((victim == INVALID_PARTICIPANT) || (victim >= d_ref->d_maxPlayers))
		return;

	// Get the attacker and verify it's valid.
	uint8 attacker = (uint8)i_data;
	if ((attacker == INVALID_PARTICIPANT) || (attacker >= d_ref->d_maxPlayers))
		return;

	// Record the apprehension.
	ASSERTF(d_ref->Score(victim, d_ref->d_maxPlayers + APPREHENSIONS) < MAX_SCORE,
		("Referee: %d has been apprehended too many times", victim));
	if (d_ref->Score(victim, d_ref->d_maxPlayers + APPREHENSIONS) < MAX_SCORE)
	{
		d_ref->Score(victim, d_ref->d_maxPlayers + APPREHENSIONS)++;
		d_ref->HasChanged( true );
	}

	// Record and display the apprehender.
	ASSERTF(d_ref->Score(attacker, d_ref->d_maxPlayers + APPREHENSIONS) < MAX_SCORE,
		("Referee: %d has too many apprehensions", attacker));
	if (d_ref->Score(attacker, d_ref->d_maxPlayers + APPREHENSIONS) < MAX_SCORE)
	{
		d_ref->ShowApprehendHudMessage(victim, attacker);
		d_ref->Score(attacker, d_ref->d_maxPlayers + APPREHENSIONS)++;
		d_ref->HasChanged( true );
	}
}

///******************************************************************************/
///******************************************************************************/
//void CRobberyScoringModel::CObjectiveTaken::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
//{
//	uint8 personId = CParticipantIF::GetParticipantIdFromActor((ActorHandle)((uint32)i_data));
//	if (personId == INVALID_PARTICIPANT || personId >= d_ref->d_maxPlayers)
//		return;
//
//	d_ref->Score( personId, d_ref->d_maxPlayers + EXTRA_POINTS ) += 1;
//	d_ref->HasChanged( true );
//}
//
///******************************************************************************/
///******************************************************************************/
//void CRobberyScoringModel::CObjectiveReturned::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
//{
//	uint8 personId = CParticipantIF::GetParticipantIdFromActor((ActorHandle)((uint32)i_data));
//	if (personId == INVALID_PARTICIPANT || personId >= d_ref->d_maxPlayers)
//		return;
//
//	d_ref->Score( personId, d_ref->d_maxPlayers + EXTRA_POINTS ) += 1;
//	d_ref->HasChanged( true );
//}

/******************************************************************************/
/******************************************************************************/
void CRobberyScoringModel::CObjectiveCaptured::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	// Don't modify the buffer if we're not recording.
	if (!d_ref->IsRecording())
		return;

	uint8 personId = CParticipantIF::GetParticipantIdFromActor((ActorHandle)((uint32)i_data));
	if (personId == INVALID_PARTICIPANT || personId >= d_ref->d_maxPlayers)
		return;

	// The objective can only be achieved once per round.
	d_ref->Score( personId, d_ref->d_maxPlayers + PRI_OBJECTIVES ) = 1;
	d_ref->SetObjectiveAchieved(RED_CAPTURED_OBJ);
	d_ref->HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CRobberyScoringModel::CSessionMasterChanged::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if ((g_referee.GetPhase() == Referee::REPORTING) && !d_ref->IsTabulated())
		d_ref->d_endRoundHandler.HandleMessage(i_data, i_sender, i_posted);
}
