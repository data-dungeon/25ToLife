/*
**
**  File:   ScoringModel.cpp
**  Date:   October 5, 2004
**  By:     Bryant Collard
**  Desc:   The base class of a module that installs in the referee and that
**          manages scoring.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/ScoringModel.cpp $
**  $Revision: #45 $
**  $DateTime: 2005/08/30 17:42:01 $
**    $Author: Bryce $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "sim/ScoringModel.h"

// Utility header.
#include "environ/ConfigMgr.h"

// Setup score size.
#define MAX_SCORE ((CScoringModel::TScore)~0)

// Declare scoring model creator head. For this stuff to work, the compiler must
// initialize it to null before it is referenced for the first time (possibly
// before this translation unit is processed). ANSI C specifies that any global
// variable declared with the static storage class specifier and of pointer type
// that is not explicitly initialized is assigned a null pointer when program
// execution begins.
CScoringModelCreator* CScoringModelCreator::s_head;

/******************************************************************************/
/******************************************************************************/
CScoringModel* CScoringModelCreator::Create(const char* i_name,
		uint8 i_maxPlayers, uint8 i_maxParticipants)
{
	CScoringModelCreator* creator =
			(CScoringModelCreator*)CCreator::Find(i_name, s_head);
	if (creator != NULL)
		return(creator->Create(i_maxPlayers, i_maxParticipants));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CScoringModel::CScoringModel(uint8 i_maxPlayers, uint8 i_maxParticipants) :
	d_maxPlayers(i_maxPlayers), d_maxParticipants(i_maxParticipants)
{
	d_buffer = NULL;
	d_localMatchScore.d_alphaWins = 0;
	d_localMatchScore.d_betaWins = 0;
	d_localMatchScore.d_alphaBonus = 0;
	d_localMatchScore.d_betaBonus = 0;
	d_localMatchScore.d_objAchieved = 0;
	d_localMatchScore.d_state = RECORDING;

	d_hasChanged = false;
	d_timeLastUpdated = 0.0f;
	d_isAlphaTeamLeading = false;
	d_isBetaTeamLeading = false;

	d_pPlayerInfo = NULL;
}

/******************************************************************************/
/******************************************************************************/
CScoringModel::~CScoringModel()
{
}

/******************************************************************************/
/******************************************************************************/
uint CScoringModel::GetBufferSize(void)
{
	// Data is stored in a 2-dimensional table. The rows store kills. The 
	// first d_maxPlayers rows store kills for the players. The columns 
	// store deaths and bonus points. The first d_maxPlayers columns store 
	// deaths for the players.
	// The d_maxPlayers + 0 column stores npc kills.
	// The d_maxPlayers + 1 column stores number of apprehends/apprehensions.
	// The d_maxPlayers + 2 column stores number of objectives obtained.
	// The d_maxPlayers + 3 column stores bonus points. 
	// The d_maxPlayers + 4 column stores number of rounds won by each player.
	// The d_maxPlayers + 5 column stores number of rounds lost by each player.
	return((d_maxPlayers + NUM_SUPPLEMENTARY_ROWS) *
			(d_maxPlayers + NUM_SUPPLEMENTARY_COLUMNS) * sizeof(TScore) +
			sizeof(SMatchScore));
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetBuffer(void* i_buffer)
{
	if (i_buffer != d_buffer)
	{
		if (d_buffer != NULL)
			d_localMatchScore = MatchScore();
		d_buffer = i_buffer;
		if (d_buffer != NULL)
			MatchScore() = d_localMatchScore;
	}
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::InitializeBuffer(void)
{
	for (int i = 0; i < (d_maxPlayers + NUM_SUPPLEMENTARY_ROWS); i++)
	{
		for (int j = 0; j < (d_maxPlayers + NUM_SUPPLEMENTARY_COLUMNS); j++)
		{
			Score(i, j) = 0;
		}
	}
	MatchScore().d_alphaWins = 0;
	MatchScore().d_betaWins = 0;
	MatchScore().d_alphaBonus = 0;
	MatchScore().d_betaBonus = 0;
	MatchScore().d_objAchieved = 0;
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::HasDataChanged(void)
{
	if (d_hasChanged)
	{
		// Test for the lead changing hands.
		{
			const CTeamingModel &teaming = g_referee.GetTeamingModel();
			char szTalkText[ 32 ];

			// Alpha gained the lead.
			if ( !d_isAlphaTeamLeading && ( GetTeamStatus( teaming.FindTeamInGroupAlpha() ) == WINNING ) )
			{
				sprintf( szTalkText, "%s team now leads", IsAlphaBlue() ? 
						teaming.GetBlueTeamName() : teaming.GetRedTeamName() );
				g_messageDispatcher.SendMessageToSession( "TextChat", szTalkText, INVALID_OBJECT_HANDLE );
				d_isAlphaTeamLeading = true;
				d_isBetaTeamLeading = false;
			}

			// Beta gained the lead.
			if ( !d_isBetaTeamLeading && ( GetTeamStatus( teaming.FindTeamInGroupBeta() ) == WINNING ) )
			{
				sprintf( szTalkText, "%s team now leads", IsAlphaBlue() ? 
						teaming.GetRedTeamName() : teaming.GetBlueTeamName() );
				g_messageDispatcher.SendMessageToSession( "TextChat", szTalkText, INVALID_OBJECT_HANDLE );
				d_isAlphaTeamLeading = false;
				d_isBetaTeamLeading = true;
			}
		}

		// Only allow updates to be sent within a certain time of each other.
		if ( g_session.GetSessionTimeSec() > d_timeLastUpdated + MIN_UPDATE_TIME_DELAY )
		{
			d_timeLastUpdated = g_session.GetSessionTimeSec();
			HasChanged( false );
			return(true);
		}
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::DataHasChanged(void)
{
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetPlayerInfo(DSScoringPlayerInfo* i_pPlayerInfo)
{
	if (i_pPlayerInfo != d_pPlayerInfo)
	{
		d_pPlayerInfo = i_pPlayerInfo;
	}
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::AddToPlayerCount(EScoringType i_eType, 
	uint8 i_playerId, uint8 i_targetId, int i_count)
{
	// Don't modify the buffer if we're not recording.
	if (!IsRecording())
		return;

	// Parameter validity test.
	if ((i_playerId >= d_maxPlayers) || (i_targetId >= d_maxParticipants) || (i_count == 0))
		return;

	switch (i_eType)
	{
		case ALL_KILLS:
		case ENEMY_KILLS:
		case TEAM_KILLS:
		case NPC_KILLS:
			Score(i_playerId, i_targetId) += (TScore)i_count;
			break;
		case ALL_DEATHS:
		case DEATHS:
		case TEAM_KILLED:
			Score(i_targetId, i_playerId) += (TScore)i_count;
			break;
		case SUICIDES:
			Score(i_playerId, i_playerId) += (TScore)i_count;
			break;
		case APPREHENDS:
			Score(i_playerId, d_maxPlayers + APPREHENSIONS) += (TScore)i_count;
			break;
		case OBJECTIVES:
			Score(i_playerId, d_maxPlayers + PRI_OBJECTIVES) += (TScore)i_count;
		case TOTAL_SCORE:
			break;
		default:
			break;
	}
	HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::AddToTeamCount(EScoringType i_eType, uint8 i_teamId, uint8 i_targetId, int i_count)
{
	// Don't modify the buffer if we're not recording.
	if (!IsRecording())
		return;

	if (i_count == 0)
		return;

	for (int i = 0; i < d_maxPlayers; i++)
	{
		if (GetPlayerTeam(i) == i_teamId)
		{
			switch (i_eType)
			{
				case ALL_KILLS:
				case ENEMY_KILLS:
				case TEAM_KILLS:
				case NPC_KILLS:
					Score(i, i_targetId) += (TScore)i_count;
					break;
				case ALL_DEATHS:
				case DEATHS:
				case TEAM_KILLED:
					Score(i_targetId, i) += (TScore)i_count;
					break;
				case SUICIDES:
					Score(i, i) += (TScore)i_count;
					break;
				case APPREHENDS:
					Score(i, d_maxPlayers + APPREHENSIONS) += (TScore)i_count;
					break;
				case OBJECTIVES:
					Score(i, d_maxPlayers + PRI_OBJECTIVES) += (TScore)i_count;
				case TOTAL_SCORE:
					break;
				default:
					break;
			}
		}
	}
	// Also add the win to the overall team count, not just the individual.
	if (i_eType == ROUNDS_WON)
	{
		g_referee.GetTeamingModel().IsTeamInGroupAlpha(i_teamId) ? IncrementAlphaWins() : IncrementBetaWins();
	}
	HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetPlayerCount(EScoringType i_eType, uint8 i_playerId)
{
	if (i_playerId >= d_maxPlayers)
		return 0;

	switch (i_eType)
	{
		case ALL_KILLS:
			return GetAllKills( i_playerId );
			break;
		case ENEMY_KILLS:
			return GetEnemyKills( i_playerId );
			break;
		case TEAM_KILLS:
			return GetTeamKills( i_playerId );
			break;
		case NPC_KILLS:
			return GetNonPlayerKills( i_playerId );
			break;
		case ALL_DEATHS:
			return GetAllDeaths( i_playerId );
			break;
		case DEATHS:
			return GetDeathByEnemy( i_playerId );
			break;
		case TEAM_KILLED:
			return GetDeathByTeammates( i_playerId );
			break;
		case SUICIDES:
			return GetSuicides( i_playerId );
			break;
		case APPREHENDS:
			return GetApprehensions( i_playerId );
			break;
		case OBJECTIVES:
			return GetObjectives( i_playerId );
			break;
		case TOTAL_SCORE:
			return GetTotalScore( i_playerId );
			break;
		case ROUNDS_WON:
			return (g_referee.GetTeamingModel().IsPlayerInGroupAlpha(i_playerId) ? GetAlphaWins() : GetBetaWins());
			break;
		case ROUNDS_LOST:
			return (g_referee.GetTeamingModel().IsPlayerInGroupAlpha(i_playerId) ? GetBetaWins() : GetAlphaWins());
			break;
		default:
			return 0;
			break;
	}
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetTeamCount(EScoringType i_eType, uint8 i_teamId)
{
	int total = 0;

	// Look for everything else.
	for (int i = 0; i < d_maxPlayers; i++)
	{
		if (GetPlayerTeam(i) == i_teamId)
		{
			switch (i_eType)
			{
				case ALL_KILLS:
					total += GetAllKills( i );
					break;
				case ENEMY_KILLS:
					total += GetEnemyKills( i );
					break;
				case TEAM_KILLS:
					total += GetTeamKills( i );
					break;
				case NPC_KILLS:
					total += GetNonPlayerKills( i );
					break;
				case ALL_DEATHS:
					total += GetAllDeaths( i );
					break;
				case DEATHS:
					total += GetDeathByEnemy( i );
					break;
				case TEAM_KILLED:
					total += GetDeathByTeammates( i );
					break;
				case SUICIDES:
					total += GetSuicides( i );
					break;
				case APPREHENDS:
					total += GetApprehensions( i );
					break;
				case OBJECTIVES:
					total += GetObjectives( i );
					break;
				case TOTAL_SCORE:
					total += GetTotalScore( i );
					break;
				case ROUNDS_WON:
					return (g_referee.GetTeamingModel().IsTeamInGroupAlpha(i_teamId) ? GetAlphaWins() : GetBetaWins());
					break;
				case ROUNDS_LOST:
					return (g_referee.GetTeamingModel().IsTeamInGroupAlpha(i_teamId) ? GetBetaWins() : GetAlphaWins());
					break;
				default:
					return total;
					break;
			}
		}
	}
	// Team bonuses only apply to the whole team, not individuals
	if (i_eType == TOTAL_SCORE)
	{
		if (g_referee.GetTeamingModel().IsTeamInGroupAlpha(i_teamId))
			total += GetAlphaBonus();
		else if (g_referee.GetTeamingModel().IsTeamInGroupBeta(i_teamId))
			total += GetBetaBonus();
	}
	return total;
}

/******************************************************************************/
/******************************************************************************/
uint8 CScoringModel::GetBestPlayer(EScoringType i_eType, int &o_total)
{
	int best = -256;
	int total = 0;
	uint8 player = INVALID_PLAYER;

	for (int i = 0; i < d_maxPlayers; i++)
	{
		total = GetPlayerCount(i_eType, i);

		if (best < total)
		{
			best = total;
			player = i;
		}
	}
	o_total = best;
	return player;
}

/******************************************************************************/
/******************************************************************************/
uint8 CScoringModel::GetBestTeam(EScoringType i_eType, int &o_total)
{
	int best = -256;
	int total = 0;
	int team = INVALID_TEAM;

	// Search through everything else.
	for (int t = 0; t < g_referee.GetTeamingModel().NUM_GROUPS; t++)
	{
		total = GetTeamCount(i_eType, t);

		if (best < total)
		{
			best = total;
			team = t;
		}
	}
	o_total = best;
	return team;
}

/******************************************************************************/
/******************************************************************************/
uint8 CScoringModel::GetBestPlayerOnTeam(EScoringType i_eType, uint8 i_teamId, int &o_total)
{
	int best = -256;
	int total = 0;
	int player = INVALID_PLAYER;

	for (int i = 0; i < d_maxPlayers; i++)
	{
		if (GetPlayerTeam(i) == i_teamId)
		{
			total = GetPlayerCount(i_eType, i);

			if (best < total)
			{
				best = total;
				player = i;
			}
		}
	}
	o_total = best;
	return player;
}

/******************************************************************************/
/******************************************************************************/
uint8 CScoringModel::GetNextBestPlayerOnTeam(EScoringType i_eType, uint8 i_playerId, int &o_total)
{
	if (i_playerId == INVALID_PLAYER)
		return (INVALID_PLAYER);

	int team = GetPlayerTeam(i_playerId);
	int oldBest = GetPlayerCount( i_eType, i_playerId );
	int newBest = -256;
	int total = 0;
	int player = INVALID_PLAYER;

	for (int i = 0; i < d_maxPlayers; i++)
	{
		if ((GetPlayerTeam(i) == team) && (i != i_playerId))
		{
			total = GetPlayerCount(i_eType, i);

			// the new total is greater than the best one found so far, but less than
			// the old best. or, they are equal and the player is further down the list.
			if ((newBest < total) && ((total < oldBest) || ((total == oldBest) && (i > i_playerId))))
			{
				newBest = total;
				player = i;
			}
		}
	}
	o_total = newBest;
	return player;
}

/******************************************************************************/
// Get overall MVP
uint8 CScoringModel::GetOverallMVP()
{
	uint8 bestPlayer = INVALID_PLAYER;

	// Find the MVP
	for (int i = 0; i < d_maxPlayers; i++)
	{
		if (GetPlayerTeam(i) == INVALID_SCORING_TEAM)
			continue;

		// Have we found a best player yet?
		if (bestPlayer != INVALID_PLAYER)
		{
			// Compare players using total score
			if (GetTotalScore(i) > GetTotalScore(bestPlayer))
			{
				bestPlayer = i;
			}
			else if (GetTotalScore(i) == GetTotalScore(bestPlayer))
			{
				// Compare players using enemy kills
				if (GetEnemyKills(i) > GetEnemyKills(bestPlayer))
				{
					bestPlayer = i;
				}
				else if (GetEnemyKills(i) == GetEnemyKills(bestPlayer))
				{
					// Compare players using deaths and suicides
					if (GetAllDeaths(i) < GetAllDeaths(bestPlayer))
					{
						bestPlayer = i;
					}
					else if (GetAllDeaths(i) == GetAllDeaths(bestPlayer))
					{
						// Compare players using winning team
						if (GetTeamStatus(GetPlayerTeam(i)) == WON 
							&& GetTeamStatus(GetPlayerTeam(bestPlayer)) == LOST)
						{
							bestPlayer = i;
						}
					}
				}
			}
		}
		else
		{
			bestPlayer = i;
		}
	}

	return bestPlayer;
}

/******************************************************************************/
// Get team MVP
uint8 CScoringModel::GetTeamMVP(uint8 i_teamId)
{
	uint8 bestPlayer = INVALID_PLAYER;

	// Find the MVP
	for (int i = 0; i < d_maxPlayers; i++)
	{
		// Player must be valid and on the specified team
		if (GetPlayerTeam(i) != i_teamId)
			continue;

		// Have we found a best player yet?
		if (bestPlayer != INVALID_PLAYER)
		{
			// Compare players using total score
			if (GetTotalScore(i) > GetTotalScore(bestPlayer))
			{
				bestPlayer = i;
			}
			else if (GetTotalScore(i) == GetTotalScore(bestPlayer))
			{
				// Compare players using enemy kills
				if (GetEnemyKills(i) > GetEnemyKills(bestPlayer))
				{
					bestPlayer = i;
				}
				else if (GetEnemyKills(i) == GetEnemyKills(bestPlayer))
				{
					// Compare players using deaths and suicides
					if (GetAllDeaths(i) < GetAllDeaths(bestPlayer))
					{
						bestPlayer = i;
					}
				}
			}
		}
		else
		{
			bestPlayer = i;
		}
	}

	return bestPlayer;
}

/******************************************************************************/
/******************************************************************************/
CScoringModel::ETeamStatus CScoringModel::GetTeamStatus(uint8 i_teamId)
{
	const CTeamingModel& teaming = g_referee.GetTeamingModel();

	// Round is still going.
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
					// If both teams are gangsters then apprehensions are unused.
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

		// Test for standard team point comparison.
		{
			int us = GetTeamCount(TOTAL_SCORE, i_teamId);
			int them = 0;
			if (teaming.IsTeamInGroupAlpha( i_teamId ))
				them = GetTeamCount(TOTAL_SCORE, teaming.FindTeamInGroupBeta());
			else
				them = GetTeamCount(TOTAL_SCORE, teaming.FindTeamInGroupAlpha());

			if (us < them)
				return LOST;
			else if (us > them)
				return WON;
			else
				return TIED;
		}
	}
}

/******************************************************************************/
/******************************************************************************/
CScoringModel::ETeamStatus CScoringModel::GetBlueTeamStatus()
{
	uint8 team = INVALID_TEAM;
	if (IsAlphaBlue())
		team = g_referee.GetTeamingModel().FindTeamInGroupAlpha();
	else if (IsBetaBlue())
		team = g_referee.GetTeamingModel().FindTeamInGroupBeta();

	return( GetTeamStatus( team ) );
}

/******************************************************************************/
/******************************************************************************/
CScoringModel::ETeamStatus CScoringModel::GetRedTeamStatus()
{
	uint8 team = INVALID_TEAM;
	if (IsAlphaRed())
		team = g_referee.GetTeamingModel().FindTeamInGroupAlpha();
	else if (IsBetaRed())
		team = g_referee.GetTeamingModel().FindTeamInGroupBeta();

	return( GetTeamStatus( team ) );
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetAllKills(uint8 i_id)
{
	int total = 0;
	for (int j = 0; j <= (d_maxPlayers + NPC_DEATHS); j++)
		total += (int)Score(i_id, j);
	return(total);
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetEnemyKills(uint8 i_id)
{
	int kills = 0;
	for (int i = 0; i < d_maxPlayers; i++)
		if ((i != i_id) && AreEnemies(i, i_id))
			kills += (int)Score(i_id, i);
	return (kills);
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetTeamKills(uint8 i_id)
{
	int kills = 0;
	for (int i = 0; i < d_maxPlayers; i++)
		if ((i != i_id) && AreTeammates(i, i_id))
			kills += (int)Score(i_id, i);
	return (kills);
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetNonPlayerKills(uint8 i_id)
{
	return((int)Score(i_id, d_maxPlayers));
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetAllDeaths(uint8 i_id)
{
	int total = 0;
	for (int i = 0; i < d_maxPlayers; i++)
		total += (int)Score(i, i_id);
	return(total);
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetDeathByEnemy(uint8 i_id)
{
	int total = 0;
	for (int i = 0; i < d_maxPlayers; i++)
		if ((i != i_id) && AreEnemies(i, i_id))
			total += (int)Score(i, i_id);
	return(total);
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetDeathByTeammates(uint8 i_id)
{
	int total = 0;
	for (int i = 0; i < d_maxPlayers; i++)
		if ((i != i_id) && AreTeammates(i, i_id))
			total += (int)Score(i, i_id);
	return(total);
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetSuicides(uint8 i_id)
{
	return((int)Score(i_id, i_id));
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetApprehensions(uint8 i_id)
{
	return((int)Score(i_id, d_maxPlayers + APPREHENSIONS));
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetObjectives(uint8 i_id)
{
	return((int)Score(i_id, d_maxPlayers + PRI_OBJECTIVES));
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetTotalScore(uint8 i_id)
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
		score += -GetApprehensions(i_id);
	}
	return(score);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::ShowKillHudMessage(uint8 i_victim, uint8 i_attacker)
{
	// Don't show in sp mode
	if (g_configmgr.TestOptionValue("gameModes", "sp"))
		return;

	char szTalkText[ 128 ];
	CPlayerAgent *pVictim = g_referee.GetPlayerAgent( i_victim );
	if (pVictim == NULL)
		return;
	CPlayerAgent *pAttacker = g_referee.GetPlayerAgent( i_attacker );
	if ((pAttacker == NULL) || (pVictim == pAttacker))
		sprintf( szTalkText, "%s died", pVictim->GetDisplayName() );
	else if (AreTeammates( i_victim, i_attacker ))
		sprintf( szTalkText, "%s team-killed %s", pAttacker->GetDisplayName(), pVictim->GetDisplayName() );
	else
		sprintf( szTalkText, "%s killed %s", pAttacker->GetDisplayName(), pVictim->GetDisplayName() );
	g_messageDispatcher.SendMessageToSession( "TextChat", szTalkText, INVALID_OBJECT_HANDLE );
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::ShowApprehendHudMessage(uint8 i_victim, uint8 i_attacker)
{
	// Don't show in sp mode
	if (g_configmgr.TestOptionValue("gameModes", "sp"))
		return;

	CPlayerAgent *pVictim = g_referee.GetPlayerAgent( i_victim );
	if (pVictim == NULL)
		return;
	char szTalkText[ 128 ];
	CPlayerAgent *pAttacker = g_referee.GetPlayerAgent( i_attacker );
	if (pAttacker == NULL)
		sprintf( szTalkText, "%s died", pVictim->GetDisplayName() );
	else
		sprintf( szTalkText, "%s apprehended %s", pAttacker->GetDisplayName(), pVictim->GetDisplayName() );
	g_messageDispatcher.SendMessageToSession( "TextChat", szTalkText, INVALID_OBJECT_HANDLE );
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::ShowTagHudMessage(uint8 i_player)
{
	// Only show in tag mode
	if (!g_configmgr.TestOptionValue("gameModes", "tag"))
		return;

	char szTalkText[ 128 ];
	const CTeamingModel& teaming = g_referee.GetTeamingModel();
	const char* teamName;
	if (IsPlayerBlue( i_player ))
		teamName = teaming.GetBlueTeamName();
	else
		teamName = teaming.GetRedTeamName();
	sprintf( szTalkText, "%s team captured a spawn point", teamName );
	g_messageDispatcher.SendMessageToSession( "TextChat", szTalkText, INVALID_OBJECT_HANDLE );
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetAlphaWins(uint8 i_count)
{
	MatchScore().d_alphaWins = i_count;
	HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::IncrementAlphaWins()
{
	SetAlphaWins(GetAlphaWins() + 1);
	g_referee.IncrementSessionAlphaWins();
}

/******************************************************************************/
/******************************************************************************/
uint8 CScoringModel::GetAlphaWins(void)
{
	return(MatchScore().d_alphaWins);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetBetaWins(uint8 i_count)
{
	MatchScore().d_betaWins = i_count;
	HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::IncrementBetaWins()
{
	SetBetaWins(GetBetaWins() + 1);
	g_referee.IncrementSessionBetaWins();
}

/******************************************************************************/
/******************************************************************************/
uint8 CScoringModel::GetBetaWins(void)
{
	return(MatchScore().d_betaWins);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::IncrementTeamBonus(uint8 i_teamId, uint16 i_count)
{
	if (g_referee.GetTeamingModel().IsTeamInGroupAlpha(i_teamId))
		IncrementAlphaBonus(i_count);
	else if (g_referee.GetTeamingModel().IsTeamInGroupBeta(i_teamId))
		IncrementBetaBonus(i_count);
}

/******************************************************************************/
/******************************************************************************/
uint16 CScoringModel::GetTeamBonus(uint8 i_teamId)
{
	if (g_referee.GetTeamingModel().IsTeamInGroupAlpha(i_teamId))
		return(GetAlphaBonus());
	else if (g_referee.GetTeamingModel().IsTeamInGroupBeta(i_teamId))
		return(GetBetaBonus());
	return(0);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetAlphaBonus(uint16 i_count)
{
	MatchScore().d_alphaBonus = i_count;
	HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
uint16 CScoringModel::GetAlphaBonus()
{
	return(MatchScore().d_alphaBonus);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetBetaBonus(uint16 i_count)
{
	MatchScore().d_betaBonus = i_count;
	HasChanged( true );
}

/******************************************************************************/
/******************************************************************************/
uint16 CScoringModel::GetBetaBonus()
{
	return(MatchScore().d_betaBonus);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetObjectiveAchieved(uint8 i_obj)
{
	MatchScore().d_objAchieved |= i_obj;
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsObjectiveAchieved(uint8 i_obj)
{
	return(MatchScore().d_objAchieved & i_obj ? true : false);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::ResetObjectiveAchieved()
{
	MatchScore().d_objAchieved = 0;
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsTeamEliminated(uint8 i_teamId)
{
	if (IsTeamBlue( i_teamId ))
		return( IsObjectiveAchieved( BLUE_ELIMINATED ) );
	else if (IsTeamRed( i_teamId ))
		return( IsObjectiveAchieved( RED_ELIMINATED ) );
	else
		return( false );
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetState(uint8 i_state)
{
	MatchScore().d_state = i_state;
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsRecording()
{
	return( MatchScore().d_state == RECORDING );
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsTabulated()
{
	return( MatchScore().d_state == TABULATED );
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::ResetForNewRound()
{
	ResetTeamBonus();
	ResetObjectiveAchieved();
	SetState( RECORDING );
	d_timeLastUpdated = 0.0f;
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::PopulatePlayerInfo()
{
	if (d_pPlayerInfo == NULL)
		return;

	for (int i = 0; i < d_maxPlayers; i++)
	{
		if (g_referee.IsActivePlayer(i))
		{
			SetPlayerName(i, g_referee.GetPlayerAgent(i)->GetDisplayName());
			SetPlayerTeam(i, g_referee.GetTeam(i));
		}
		else
		{
			SetPlayerName(i, "");
			SetPlayerTeam(i, INVALID_SCORING_TEAM);
		}
	}
	d_pPlayerInfo->d_isAlphaBlue = g_referee.GetTeamingModel().IsAlphaBlue() ? 1 : 0;
	d_pPlayerInfo->RequestUpdate();
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetPlayerInfoCount()
{
	if (d_pPlayerInfo == NULL)
		return 0;

	int count = 0;
	for (int i = 0; i < d_maxPlayers; i++)
		if (GetPlayerTeam(i) != INVALID_SCORING_TEAM)
			count++;
	return (count);
}

/******************************************************************************/
/******************************************************************************/
int CScoringModel::GetTeamPlayerCount(uint8 i_teamId)
{
	if (d_pPlayerInfo == NULL)
		return 0;

	int count = 0;
	for (int i = 0; i < d_maxPlayers; i++)
		if (GetPlayerTeam(i) == i_teamId)
			count++;
	return (count);
}

/******************************************************************************/
/******************************************************************************/
const char* CScoringModel::GetPlayerName(uint8 i_playerId)
{
	if (d_pPlayerInfo && (i_playerId < d_maxPlayers) && ((d_pPlayerInfo->d_name[i_playerId].CStr())[0] != '\0'))
		return (d_pPlayerInfo->d_name[i_playerId].CStr());

	return ("");
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetPlayerName(uint8 i_playerId, const char* i_name)
{
	if (d_pPlayerInfo)
		d_pPlayerInfo->d_name[i_playerId] = i_name;
}

/******************************************************************************/
/******************************************************************************/
uint8 CScoringModel::GetPlayerTeam(uint8 i_playerId)
{
	if (d_pPlayerInfo && (i_playerId < d_maxPlayers))
		return (d_pPlayerInfo->d_team[i_playerId]);

	return (INVALID_SCORING_TEAM);
}

/******************************************************************************/
/******************************************************************************/
void CScoringModel::SetPlayerTeam(uint8 i_playerId, uint8 i_teamId)
{
	if (d_pPlayerInfo)
		d_pPlayerInfo->d_team[i_playerId] = i_teamId;
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsPlayerBlue(uint8 i_playerId)
{
	if (d_pPlayerInfo && (i_playerId < d_maxPlayers) &&
			((IsAlphaBlue() && g_referee.GetTeamingModel().IsPlayerInGroupAlpha(i_playerId)) || 
			(IsBetaBlue() && g_referee.GetTeamingModel().IsPlayerInGroupBeta(i_playerId))))
		return (true);

	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsPlayerRed(uint8 i_playerId)
{
	if (d_pPlayerInfo && (i_playerId < d_maxPlayers) &&
			((IsAlphaRed() && g_referee.GetTeamingModel().IsPlayerInGroupAlpha(i_playerId)) || 
			(IsBetaRed() && g_referee.GetTeamingModel().IsPlayerInGroupBeta(i_playerId))))
		return (true);

	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsTeamBlue(uint8 i_teamId)
{
	if (d_pPlayerInfo && 
			((IsAlphaBlue() && i_teamId == g_referee.GetTeamingModel().FindTeamInGroupAlpha()) ||
			(IsBetaBlue() && i_teamId == g_referee.GetTeamingModel().FindTeamInGroupBeta())))
		return (true);

	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsTeamRed(uint8 i_teamId)
{
	if (d_pPlayerInfo && 
			((IsAlphaRed() && i_teamId == g_referee.GetTeamingModel().FindTeamInGroupAlpha()) ||
			(IsBetaRed() && i_teamId == g_referee.GetTeamingModel().FindTeamInGroupBeta())))
		return (true);

	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsAlphaBlue()
{
	if (d_pPlayerInfo)
		return(d_pPlayerInfo->d_isAlphaBlue ? true : false);

	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsAlphaRed()
{
	if (d_pPlayerInfo)
		return(d_pPlayerInfo->d_isAlphaBlue ? false : true);

	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsBetaBlue()
{
	if (d_pPlayerInfo)
		return(d_pPlayerInfo->d_isAlphaBlue ? false : true);

	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::IsBetaRed()
{
	if (d_pPlayerInfo)
		return(d_pPlayerInfo->d_isAlphaBlue ? true : false);

	return (false);
}

/******************************************************************************/
/******************************************************************************/
CPlayerProfile::EVariant CScoringModel::GetPlayerProfileVariant(uint8 i_playerId)
{
	if (IsPlayerBlue(i_playerId) && !g_configmgr.TestOptionValue("gameModes", "tag"))
		return (CPlayerProfile::COP);
	else
		return (CPlayerProfile::GANG);
}

/******************************************************************************/
/******************************************************************************/
CPlayerProfile::EVariant CScoringModel::GetTeamProfileVariant(uint8 i_teamId)
{
	if (IsTeamBlue(i_teamId) && !g_configmgr.TestOptionValue("gameModes", "tag"))
		return (CPlayerProfile::COP);
	else
		return (CPlayerProfile::GANG);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::AreEnemies(uint8 i_alphaId, uint8 i_betaId)
{
	if ( (GetPlayerTeam(i_alphaId) != GetPlayerTeam(i_betaId)) 
		&& ( (GetPlayerTeam(i_alphaId) != INVALID_SCORING_TEAM) 
			|| (GetPlayerTeam(i_betaId) != INVALID_SCORING_TEAM) ) )
		return (true);
	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CScoringModel::AreTeammates(uint8 i_alphaId, uint8 i_betaId)
{
	if ( (GetPlayerTeam(i_alphaId) == GetPlayerTeam(i_betaId)) 
		&& (GetPlayerTeam(i_alphaId) != INVALID_SCORING_TEAM) )
		return (true);
	return (false);
}
