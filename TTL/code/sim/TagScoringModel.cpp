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
**      $File: //depot/TTL/code/sim/TagScoringModel.cpp $
**  $Revision: #42 $
**  $DateTime: 2005/08/11 17:18:35 $
**    $Author: Bryce $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "sim/TagScoringModel.h"

// Utility header.
#include "environ/ConfigMgr.h"
#include "environ/PlayerSpawnMgr.h"
#include "sim/TeamingModel.h"

// Setup score size.
#define MAX_SCORE ((CScoringModel::TScore)~0)
#define MAX_BONUS ((uint16)~0)

/******************************************************************************/
/******************************************************************************/
SCORING_MODEL_CREATOR(CTagScoringModel, Tag);

/******************************************************************************/
/******************************************************************************/
CTagScoringModel::CTagScoringModel(uint8 i_maxPlayers, uint8 i_maxParticipants) :
	CScoringModel(i_maxPlayers, i_maxParticipants),
	d_messageOwner("TagScoringModel")
{
	d_startReadyUpHandler.Initialize("ReadyUpStarting", MESSAGE_HANDLE_IF_MASTER, 
		g_referee.GetMessageOwner(), this);
	d_beginRoundHandler.Initialize("BeginRound", MESSAGE_HANDLE_IF_MASTER, 
		g_referee.GetMessageOwner(), this);
	d_endRoundHandler.Initialize("EndRound", MESSAGE_HANDLE_IF_MASTER,
		g_referee.GetMessageOwner(), this);
	d_killHandler.Initialize("RecordKill", MESSAGE_HANDLE_IF_MASTER |
		MESSAGE_ROUTE_TO_MASTER, g_referee.GetMessageOwner(), this);
	d_tagsHandler.Initialize("RecordTagOwnage", MESSAGE_HANDLE_IF_MASTER |
		MESSAGE_ROUTE_TO_MASTER, g_referee.GetMessageOwner(), this);
	d_teamBonusHandler.Initialize("TagTeamBonus", MESSAGE_HANDLE_IF_MASTER,
		g_referee.GetMessageOwner(), this);
	d_sessionMasterChangedHandler.Initialize("SessionMasterChanged",
		MESSAGE_HANDLE_IF_MASTER, g_referee.GetMessageOwner(), this);

	HasChanged( false );
}

/******************************************************************************/
/******************************************************************************/
CTagScoringModel::~CTagScoringModel()
{
	d_messageOwner.UnregisterAll();
}

/******************************************************************************/
/******************************************************************************/
int CTagScoringModel::GetTotalScore(uint8 i_id)
{
	int score = (2 * GetEnemyKills(i_id)) + (2 * GetObjectives(i_id));
	score -= (2 * GetTeamKills(i_id)) + GetDeathByEnemy(i_id) + (2 * GetSuicides(i_id));
	return(score);
}

/******************************************************************************/
/******************************************************************************/
void CTagScoringModel::CStartReadyUp::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	// Clear out our match instance data
	d_ref->ResetTeamWins();
	d_ref->ResetTeamBonus();
	d_ref->ResetObjectiveAchieved();
	d_ref->HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CTagScoringModel::CBeginRound::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
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
void CTagScoringModel::CEndRound::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
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
void CTagScoringModel::CKill::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
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
void CTagScoringModel::CTag::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	// Don't modify the buffer if we're not recording.
	if (!d_ref->IsRecording())
		return;

	// Get the tagger and verify it's valid.
	uint8 tagger = (uint8)i_data;
	if (tagger >= d_ref->d_maxPlayers)
		return;

	// Record and display the tagging.
	ASSERTF(d_ref->Score(tagger, d_ref->d_maxPlayers + PRI_OBJECTIVES) < MAX_SCORE,
			("Referee: %d is tagging too many times", tagger));
	if (d_ref->Score(tagger, d_ref->d_maxPlayers + PRI_OBJECTIVES) < MAX_SCORE)
	{
		d_ref->ShowTagHudMessage(tagger);
		d_ref->Score(tagger, d_ref->d_maxPlayers + PRI_OBJECTIVES) += 1;
		d_ref->HasChanged( true );
	}
}

/******************************************************************************/
/******************************************************************************/
void CTagScoringModel::CTeamBonus::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	// Don't modify the buffer if we're not recording or the player spawn manager is invalid.
	if (!d_ref->IsRecording() || !ValidSingleton(PlayerSpawnMgr))
		return;

	int blueTeamBonus = g_PlayerSpawnMgr.HowManyRoleSpawnPoints( CTeamingModel::BLUE );
	int redTeamBonus = g_PlayerSpawnMgr.HowManyRoleSpawnPoints( CTeamingModel::RED );

	// Give a team bonus point for each spawn point controlled
	ASSERT(d_ref->GetAlphaBonus() < MAX_BONUS);
	if (d_ref->GetAlphaBonus() < MAX_BONUS)
	{
		if (d_ref->IsAlphaBlue())
			d_ref->IncrementAlphaBonus( blueTeamBonus );
		else
			d_ref->IncrementAlphaBonus( redTeamBonus );
	}
	ASSERT(d_ref->GetBetaBonus() < MAX_BONUS);
	if (d_ref->GetBetaBonus() < MAX_BONUS)
	{
		if (!d_ref->IsAlphaBlue())
			d_ref->IncrementBetaBonus( blueTeamBonus );
		else
			d_ref->IncrementBetaBonus( redTeamBonus );
	}
}

/******************************************************************************/
/******************************************************************************/
void CTagScoringModel::CSessionMasterChanged::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if ((g_referee.GetPhase() == Referee::REPORTING) && !d_ref->IsTabulated())
		d_ref->d_endRoundHandler.HandleMessage(i_data, i_sender, i_posted);
}
