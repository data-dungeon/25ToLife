/*
**
**  File:   KillsScoringModel.cpp
**  Date:   October 14, 2004
**  By:     Bryant Collard
**  Desc:   A scoring model that keeps track of kills.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/KillsScoringModel.cpp $
**  $Revision: #41 $
**  $DateTime: 2005/08/11 17:18:35 $
**    $Author: Bryce $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "sim/KillsScoringModel.h"

// Utility header.
#include "environ/ConfigMgr.h"

// Setup score size.
#define MAX_SCORE ((CKillsScoringModel::TScore)~0)

/******************************************************************************/
/******************************************************************************/
SCORING_MODEL_CREATOR(CKillsScoringModel, Kills);

/******************************************************************************/
/******************************************************************************/
CKillsScoringModel::CKillsScoringModel(uint8 i_maxPlayers, uint8 i_maxParticipants) :
	CScoringModel(i_maxPlayers, i_maxParticipants),
	d_messageOwner("KillsScoringModel")
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
	d_sessionMasterChangedHandler.Initialize("SessionMasterChanged",
		MESSAGE_HANDLE_IF_MASTER, g_referee.GetMessageOwner(), this);

	HasChanged( false );
}

/******************************************************************************/
/******************************************************************************/
CKillsScoringModel::~CKillsScoringModel()
{
	d_messageOwner.UnregisterAll();
}

/******************************************************************************/
/******************************************************************************/
void CKillsScoringModel::CStartReadyUp::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	// Clear out our match instance data
	d_ref->ResetTeamWins();
	d_ref->ResetTeamBonus();
	d_ref->ResetObjectiveAchieved();
	d_ref->HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CKillsScoringModel::CBeginRound::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
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
void CKillsScoringModel::CEndRound::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
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
void CKillsScoringModel::CKill::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
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
void CKillsScoringModel::CApprehend::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
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

/******************************************************************************/
/******************************************************************************/
void CKillsScoringModel::CSessionMasterChanged::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if ((g_referee.GetPhase() == Referee::REPORTING) && !d_ref->IsTabulated())
		d_ref->d_endRoundHandler.HandleMessage(i_data, i_sender, i_posted);
}
