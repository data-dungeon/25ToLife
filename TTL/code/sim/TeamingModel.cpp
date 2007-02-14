/*
**
**  File:   TeamingModel.cpp
**  Date:   October 5, 2004
**  By:     Bryant Collard
**  Desc:   The base class of a module that installs in the referee and that
**          manages team assignments.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/TeamingModel.cpp $
**  $Revision: #26 $
**  $DateTime: 2005/08/24 22:38:06 $
**    $Author: Brian_Green $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "sim/TeamingModel.h"

// TTL headers
#include "main/ParticipantConfig.h"
#include "components/Ai/AiBehavior.h"

// Declare teaming model creator head. For this stuff to work, the compiler must
// initialize it to null before it is referenced for the first time (possibly
// before this translation unit is processed). ANSI C specifies that any global
// variable declared with the static storage class specifier and of pointer type
// that is not explicitly initialized is assigned a null pointer when program
// execution begins.
CTeamingModelCreator* CTeamingModelCreator::s_head;

/******************************************************************************/
/******************************************************************************/
CTeamingModel* CTeamingModelCreator::Create(const char* i_name,
		NParticipantConfig::TConfig* &i_config, uint8* &i_role,
		uint8 i_maxPlayers, uint8 i_maxParticipants, uint8 i_maxTeams)
{
	CTeamingModelCreator* creator =
			(CTeamingModelCreator*)CCreator::Find(i_name, s_head);
	if (creator != NULL)
	{
		CTeamingModel* model = creator->Create(i_config, i_role, i_maxPlayers,
				i_maxParticipants, i_maxTeams);
		return(model);
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CTeamingModel::CTeamingModel(NParticipantConfig::TConfig* i_config,
		uint8* i_role, uint8 i_maxPlayers, uint8 i_maxParticipants,
		uint8 i_maxTeams) :
	d_config(i_config),
	d_role(i_role),
	d_maxPlayers(i_maxPlayers),
	d_maxParticipants(i_maxParticipants),
	d_maxTeams(i_maxTeams)
{
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::ValidateAssignments(void)
{
	uint8 i, team;
	int alphaCount = 0;
	int betaCount = 0;
	bool invalid = false;
	for (i = 0; i < d_maxPlayers; i++)
	{
		if (NParticipantConfig::ValidRequestedTeam(d_config[i], team))
		{
			if (IsTeamInGroupAlpha( team ))
				alphaCount++;
			else if (IsTeamInGroupBeta( team ))
				betaCount++;
			else if (team != AMBIENT)
				invalid = true;
		}
	}
	if (invalid)
	{
		for (i = 0; i < d_maxPlayers; i++)
		{
			if (NParticipantConfig::ValidRequestedTeam(d_config[i], team))
			{
				if (!IsTeamInGroupAlpha( team ) && !IsTeamInGroupBeta( team )
					&& (team != AMBIENT))
				{
					if (betaCount < alphaCount)
					{
						team = FindTeamInGroupBeta();
						betaCount++;
					}
					else
					{
						team = FindTeamInGroupAlpha();
						alphaCount++;
					}
					NParticipantConfig::SetRequestedTeam(d_config[i], team);
					NParticipantConfig::SetAssignedTeam(d_config[i], team);
				}
			}
		}
	}

	int friendlyCount = 0;
	int enemyCount = 0;
	invalid = false;
	for (i = d_maxPlayers; i < d_maxParticipants; i++)
	{
		if (NParticipantConfig::ValidRequestedTeam(d_config[i], team))
		{
			if (team == ENEMY || team == ENEMY2)
				enemyCount++;
			else if (team == FRIEND)
				friendlyCount++;
			else if (team != AMBIENT)
				invalid = true;
		}
	}
	if (invalid)
	{
		for (i = d_maxPlayers; i < d_maxParticipants; i++)
		{
			if (NParticipantConfig::ValidRequestedTeam(d_config[i], team))
			{
				if ((team != ENEMY) && (team != ENEMY2) && (team != FRIEND)
					&& (team != AMBIENT))
				{
					if (enemyCount < friendlyCount)
					{
						team = ENEMY;
						enemyCount++;
					}
					else
					{
						team = FRIEND;
						friendlyCount++;
					}
					NParticipantConfig::SetRequestedTeam(d_config[i], team);
					NParticipantConfig::SetAssignedTeam(d_config[i], team);
				}
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::AssignTeam(uint8 i_id, CPlayerAgent* i_playerAgent)
{
	if (i_id < d_maxParticipants)
	{
		uint8 team;
		if (!NParticipantConfig::ValidRequestedTeam(d_config[i_id], team))
		{
			if (i_id < d_maxPlayers)
			{
				int alphaCount = 0;
				int betaCount = 0;
				for (uint8 i = 0; i < d_maxPlayers; i++)
				{
					if ((i == i_id) ||
						!NParticipantConfig::ValidAssignedTeam(d_config[i], team))
						continue;
					if (IsTeamInGroupAlpha( team ))
						alphaCount++;
					else if (IsTeamInGroupBeta( team ))
						betaCount++;
				}
				if (betaCount < alphaCount)
					team = FindTeamInGroupBeta();
				else
					team = FindTeamInGroupAlpha();
			}
			else
				team = AMBIENT;
			NParticipantConfig::SetRequestedTeam(d_config[i_id], team);
		}
		NParticipantConfig::SetAssignedTeam(d_config[i_id], team);
	}

	//if (NParticipantConfig::InvalidRequestedTeam(d_config[i_id]))
	//{
	//	NParticipantConfig::SetAssignedTeam(d_config[i_id], 0);
	//	NParticipantConfig::SetRequestedTeam(d_config[i_id], 0);
	//}
	//else
	//{
	//	NParticipantConfig::SetAssignedTeam(d_config[i_id],
	//			NParticipantConfig::RequestedTeam(d_config[i_id]));
	//}
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::AssignTeam(uint8 i_id, CActor* i_actor)
{
	if (i_id < d_maxParticipants)
	{
		uint8 team;
		if (!NParticipantConfig::ValidRequestedTeam(d_config[i_id], team))
		{
			team = AMBIENT;
			if (i_actor != NULL)
			{
				int request = i_actor->Parm().GetInt("npc_team");
				switch(request)
				{
				case AiBehavior::OBJNPCTEAM_ENEMY:
					team = ENEMY;
					break;
				case AiBehavior::OBJNPCTEAM_FRIENDLY:
					team = FRIEND;
					break;
				case AiBehavior::OBJNPCTEAM_NEUTRAL:
					team = AMBIENT;
					break;
				case AiBehavior::OBJNPCTEAM_ENEMY2:
					team = ENEMY2;
					break;
				case AiBehavior::OBJNPCTEAM_AMBIENT:
					team = AMBIENT;
					break;
				}
			}
			NParticipantConfig::SetRequestedTeam(d_config[i_id], team);
		}
		NParticipantConfig::SetAssignedTeam(d_config[i_id], team);
	}

	//AssignTeam(i_id, (CPlayerAgent*)NULL);
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::ProcessTeamChanges(void)
{
	uint8 i, assignedTeam, requestedTeam;
	for (i = 0; i < d_maxPlayers; i++)
	{
		if (NParticipantConfig::ValidAssignedTeam(d_config[i], assignedTeam))
		{
			if (!NParticipantConfig::ValidRequestedTeam(d_config[i], requestedTeam))
				NParticipantConfig::SetRequestedTeam(d_config[i], assignedTeam);
			else if ((requestedTeam != assignedTeam) &&
				TeamHasOpenSlot( requestedTeam ) &&
				((IsTeamInGroupAlpha( requestedTeam )) ||
				(IsTeamInGroupBeta( requestedTeam )) ||
				(requestedTeam == AMBIENT)))
			{
				NParticipantConfig::SetAssignedTeam(d_config[i], requestedTeam);
			}
		}
	}

	for (; i < d_maxParticipants; i++)
	{
		if (NParticipantConfig::ValidAssignedTeam(d_config[i], assignedTeam))
		{
			if (!NParticipantConfig::ValidRequestedTeam(d_config[i], requestedTeam))
				NParticipantConfig::SetRequestedTeam(d_config[i], assignedTeam);
			else if ((requestedTeam != assignedTeam) &&
				((requestedTeam == ENEMY) ||
				(requestedTeam == ENEMY2) ||
				(requestedTeam == FRIEND) ||
				(requestedTeam == AMBIENT)))
				NParticipantConfig::SetAssignedTeam(d_config[i], requestedTeam);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::ValidTeams()
{
	int bluePlayers = GetBluePlayerCount();
	int redPlayers = GetRedPlayerCount();

	// Check that both teams have between 1 and MaxPlayers/2 players.
	if ( bluePlayers > 0 && redPlayers > 0 &&
			bluePlayers <= (d_maxPlayers / 2) &&
			redPlayers <= (d_maxPlayers / 2) )
	{
		return( true );
	}
#if defined( _DEBUG ) || !defined( CDROM )
	// Single player is valid for dev purposes.
	else if ( (bluePlayers + redPlayers) == 1 )
	{
		return( true );
	}
#endif //_DEBUG
	else
	{
		return( false );
	}
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::TeamHasOpenSlot(uint8 i_id)
{
	// We need to know how many players are on a team regardless of their state.
	int teamCount = 0;
	for (uint8 i = 0; i < d_maxPlayers; i++)
		if (g_referee.GetTeam( i ) == i_id)
			teamCount++;

	// Single player game or team has less then half the max players.
	if ((d_maxPlayers == 1) || (teamCount < (d_maxPlayers / 2)))
	{
		return( true );
	}
	else
	{
		return( false );
	}
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::GetTeams(uint8 i_idA, uint8 i_idB, uint8 &o_teamA,
		uint8 &o_teamB, bool i_includeGhosts) const
{
	return((i_idA < d_maxParticipants) && (i_idB < d_maxParticipants) &&
			NParticipantConfig::ValidAssignedTeam(d_config[i_idA], o_teamA) &&
			NParticipantConfig::ValidAssignedTeam(d_config[i_idB], o_teamB) &&
			(i_includeGhosts || (!g_referee.IsGhost(i_idA) &&
			!g_referee.IsGhost(i_idB))));
}

/******************************************************************************/
/******************************************************************************/
uint8 CTeamingModel::FindTeamInGroupAlpha() const
{
	// We currently only have one team in this group.
	return 1;
}

/******************************************************************************/
/******************************************************************************/
uint8 CTeamingModel::FindTeamInGroupBeta() const
{
	// We currently only have one team in this group.
	return 0;
}

/******************************************************************************/
/******************************************************************************/
uint8 CTeamingModel::FindTeamInSameGroup(uint8 i_id) const
{
	// this function isn't needed in TTL, but in the future...
	return INVALID_TEAM;
}

/******************************************************************************/
/******************************************************************************/
uint8 CTeamingModel::FindTeamInNewGroup(uint8 i_id) const
{
	uint8 team;
	if ((i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team))
	{
		if (IsTeamInGroupAlpha( team ))
			return FindTeamInGroupBeta();
		else if (IsTeamInGroupBeta( team ))
			return FindTeamInGroupAlpha();
	}
	return INVALID_TEAM;
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::RequestTeamInSameGroup(uint8 i_id)
{
	if( IsPlayerInGroupAlpha( i_id ) )
		g_referee.RequestTeam( i_id, FindTeamInGroupAlpha() );
	else if( IsPlayerInGroupBeta( i_id ) )
		g_referee.RequestTeam( i_id, FindTeamInGroupBeta() );
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::RequestTeamInNewGroup(uint8 i_id)
{
	if( IsPlayerInGroupAlpha( i_id ) )
		g_referee.RequestTeam( i_id, FindTeamInGroupBeta() );
	else if( IsPlayerInGroupBeta( i_id ) )
		g_referee.RequestTeam( i_id, FindTeamInGroupAlpha() );
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsPlayerInGroupAlpha(uint8 i_id) const
{
	uint8 team;
	if ((i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team))
	{
		return (IsTeamInGroupAlpha( team ));
	}
	return false;
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsPlayerInGroupBeta(uint8 i_id) const
{
	uint8 team;
	if ((i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team))
	{
		return (IsTeamInGroupBeta( team ));
	}
	return false;
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsTeamInGroupAlpha(uint8 i_id) const
{
	// we can deal with multiple teams in a single group, but for now this suffices
	return (i_id == 1);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsTeamInGroupBeta(uint8 i_id) const
{
	// we can deal with multiple teams in a single group, but for now this suffices
	return (i_id == 0);
}

/******************************************************************************/
/******************************************************************************/
CTeamingModel::ERole CTeamingModel::GetPlayerRole(uint8 i_id) const
{
	uint8 team;
	if ((i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team))
	{
		return (ERole)d_role[team];
	}
	return INVALID_ROLE;
}

/******************************************************************************/
/******************************************************************************/
CTeamingModel::ERole CTeamingModel::GetTeamRole(uint8 i_id) const
{
	if (i_id < d_maxTeams)
	{
		return (ERole)d_role[i_id];
	}
	return INVALID_ROLE;
}

/******************************************************************************/
/******************************************************************************/
CTeamingModel::ERole CTeamingModel::GetAlphasRole() const
{
	for (uint8 i = 0; i < d_maxTeams; i++)
	{
		if (IsTeamInGroupAlpha(i))
			return((ERole)d_role[i]);
	}
	return INVALID_ROLE;
}

/******************************************************************************/
/******************************************************************************/
CTeamingModel::ERole CTeamingModel::GetBetasRole() const
{
	for (uint8 i = 0; i < d_maxTeams; i++)
	{
		if (IsTeamInGroupBeta(i))
			return((ERole)d_role[i]);
	}
	return INVALID_ROLE;
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsAlphaBlue() const
{
	return (GetAlphasRole() == BLUE);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsBetaBlue() const
{
	return (GetBetasRole() == BLUE);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsAlphaRed() const
{
	return (GetAlphasRole() == RED);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsBetaRed() const
{
	return (GetBetasRole() == RED);
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::SetAlphasRole(ERole i_eRole)
{
	for (uint8 i = 0; i < d_maxTeams; i++)
	{
		if (IsTeamInGroupAlpha(i))
			d_role[i] = (uint8)i_eRole;
	}
	g_messageDispatcher.SendMessageToAll("TeamRoleChanged", NULL,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::SetBetasRole(ERole i_eRole)
{
	for (uint8 i = 0; i < d_maxTeams; i++)
	{
		if (IsTeamInGroupBeta(i))
			d_role[i] = (uint8)i_eRole;
	}
	g_messageDispatcher.SendMessageToAll("TeamRoleChanged", NULL,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::AssignAlphaToBlueTeam()
{
	SetAlphasRole( BLUE );
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::AssignBetaToBlueTeam()
{
	SetBetasRole( BLUE );
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::AssignAlphaToRedTeam()
{
	SetAlphasRole( RED );
}

/******************************************************************************/
/******************************************************************************/
void CTeamingModel::AssignBetaToRedTeam()
{
	SetBetasRole( RED );
}

/******************************************************************************/
/******************************************************************************/
CPlayerProfile::EVariant CTeamingModel::GetPlayerProfileVariant(uint8 i_id) const
{
	uint8 team;
	if ((i_id < d_maxParticipants) &&
			NParticipantConfig::ValidAssignedTeam(d_config[i_id], team))
	{
		return GetTeamProfileVariant( team );
	}
	return(CPlayerProfile::INVALID);
}

/******************************************************************************/
/******************************************************************************/
CPlayerProfile::EVariant CTeamingModel::GetTeamProfileVariant(uint8 i_id) const
{
	return(CPlayerProfile::COP);
}

/******************************************************************************/
/******************************************************************************/
const char *CTeamingModel::GetTeamName(uint8 i_teamId) const
{
	if (IsTeamBlue( i_teamId ))
		return GetBlueTeamName();
	else if (IsTeamRed( i_teamId ))
		return GetRedTeamName();
	return("Unknown");
}

/******************************************************************************/
/******************************************************************************/
const char *CTeamingModel::GetBlueTeamName(void) const
{
	return("Blue");
}

/******************************************************************************/
/******************************************************************************/
const char *CTeamingModel::GetRedTeamName(void) const
{
	return("Red");
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsPlayerBlue(uint8 i_id) const
{
	uint8 team;
	return((i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team) &&
		(IsTeamBlue( team )));
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsPlayerRed(uint8 i_id) const
{
	uint8 team;
	return((i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team) &&
		(IsTeamRed( team )));
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsTeamBlue(uint8 i_id) const
{
	if (IsTeamInGroupAlpha( i_id ) && IsAlphaBlue())
		return (true);
	else if (IsTeamInGroupBeta( i_id ) && IsBetaBlue())
		return (true);
	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsTeamRed(uint8 i_id) const
{
	if (IsTeamInGroupAlpha( i_id ) && IsAlphaRed())
		return (true);
	else if (IsTeamInGroupBeta( i_id ) && IsBetaRed())
		return (true);
	return (false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsAmbient(uint8 i_id) const
{
	uint8 team;
	return((i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team) &&
		(team == AMBIENT));
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsPlayerFriend(uint8 i_id) const
{
	uint8 team;
	return((i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team) &&
		(team == FRIEND));
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsPlayerEnemy(uint8 i_id) const
{
	uint8 team;
	if(i_id < d_maxParticipants)
	{
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team);
		if(team == ENEMY || team == ENEMY2)
			return true;
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsPlayerNeutral(uint8 i_id) const
{
	uint8 team;
	return((i_id >= d_maxParticipants) && (i_id < d_maxParticipants) &&
		NParticipantConfig::ValidAssignedTeam(d_config[i_id], team) &&
		(team == AMBIENT));
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::AreFriendTeams(uint8 i_teamA, uint8 i_teamB) const
{
	if(i_teamA == i_teamB)
		return true;

	//if one team is nuetral, they are not friends
	if(AreNeutralTeams(i_teamA,i_teamB))
		return false;

	return( !AreEnemyTeams(i_teamA, i_teamB) );
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::AreEnemyTeams(uint8 i_teamA, uint8 i_teamB) const
{
	if(i_teamA == i_teamB)
		return(false);

	if(i_teamA == AMBIENT || i_teamB == AMBIENT)
		return(false);

	switch(i_teamA)
	{
	case RED:
		switch(i_teamB)
		{
		case BLUE:
		case ENEMY:
		case ENEMY2:
			return(true);
		case FRIEND:
			return(false);
		}
		break;

	case BLUE:
		switch(i_teamB)
		{
		case RED:
		case ENEMY:
		case ENEMY2:
			return(true);
		case FRIEND:
			return(false);
		}
		break;

	case ENEMY:
		switch(i_teamB)
		{
		case BLUE:
		case RED:
		case ENEMY2:
		case FRIEND:
			return(true);
		}
		break;

	case ENEMY2:
		switch(i_teamB)
		{
		case BLUE:
		case RED:
		case ENEMY:
		case FRIEND:
			return(true);
		}
		break;

	case FRIEND:
		switch(i_teamB)
		{
		case BLUE:
		case RED:
			return(false);
		case ENEMY:
		case ENEMY2:
			return(true);
		}
		break;
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::AreNeutralTeams(uint8 i_teamA, uint8 i_teamB) const
{
	return( (i_teamA == AMBIENT) || (i_teamB == AMBIENT) );
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsTeamAllAlive(uint8 i_team) const
{
	for (uint8 i = 0; i < d_maxParticipants; i++)
	{
		if (NParticipantConfig::IsAssignedTeam(d_config[i], i_team) &&
			!g_referee.IsGhost(i) && !NParticipantConfig::Active(d_config[i]))
			return(false);
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsBlueAllAlive(void) const
{
	if (IsAlphaBlue())
		return( IsTeamAllAlive(FindTeamInGroupAlpha()) );
	else if (IsBetaBlue())
		return( IsTeamAllAlive(FindTeamInGroupBeta()) );
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsRedAllAlive(void) const
{
	if (IsAlphaRed())
		return( IsTeamAllAlive(FindTeamInGroupAlpha()) );
	else if (IsBetaRed())
		return( IsTeamAllAlive(FindTeamInGroupBeta()) );
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsTeamAllDead(uint8 i_team) const
{
	for (uint8 i = 0; i < d_maxParticipants; i++)
	{
		if (NParticipantConfig::IsAssignedTeam(d_config[i], i_team) &&
			!g_referee.IsGhost(i) && NParticipantConfig::Active(d_config[i]))
			return(false);
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsBlueAllDead(void) const
{
	if (IsAlphaBlue())
		return( IsTeamAllDead(FindTeamInGroupAlpha()) );
	else if (IsBetaBlue())
		return( IsTeamAllDead(FindTeamInGroupBeta()) );
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsRedAllDead(void) const
{
	if (IsAlphaRed())
		return( IsTeamAllDead(FindTeamInGroupAlpha()) );
	else if (IsBetaRed())
		return( IsTeamAllDead(FindTeamInGroupBeta()) );
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsTeamAllPermanentlyDead(uint8 i_team) const
{
	for (uint8 i = 0; i < d_maxParticipants; i++)
	{
		if (NParticipantConfig::IsAssignedTeam(d_config[i], i_team))
		{
			CParticipantIF* participantIF =
					CParticipantIF::GetInterface(g_referee.GetActor(i));
			if ((participantIF != NULL) && (participantIF->GetAlive() ||
						 participantIF->GetSpawnTicket()))
				return(false);
		}
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsBlueAllPermanentlyDead(void) const
{
	if (IsAlphaBlue())
		return( IsTeamAllPermanentlyDead(FindTeamInGroupAlpha()) );
	else if (IsBetaBlue())
		return( IsTeamAllPermanentlyDead(FindTeamInGroupBeta()) );
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CTeamingModel::IsRedAllPermanentlyDead(void) const
{
	if (IsAlphaRed())
		return( IsTeamAllPermanentlyDead(FindTeamInGroupAlpha()) );
	else if (IsBetaRed())
		return( IsTeamAllPermanentlyDead(FindTeamInGroupBeta()) );
	return(false);
}

/******************************************************************************/
/******************************************************************************/
int CTeamingModel::GetTeamPlayerCount(uint8 i_team) const
{
	int iCount = 0;
	for (uint8 i = 0; i < d_maxPlayers; i++)
		if ( (g_referee.GetTeam( i ) == i_team) && g_referee.IsReady( i ) )
			iCount++;
	return( iCount );
}

/******************************************************************************/
/******************************************************************************/
int CTeamingModel::GetBluePlayerCount() const
{
	if (IsAlphaBlue())
		return( GetTeamPlayerCount(FindTeamInGroupAlpha()) );
	else if (IsBetaBlue())
		return( GetTeamPlayerCount(FindTeamInGroupBeta()) );
	return(0);
}

/******************************************************************************/
/******************************************************************************/
int CTeamingModel::GetRedPlayerCount() const
{
	if (IsAlphaRed())
		return( GetTeamPlayerCount(FindTeamInGroupAlpha()) );
	else if (IsBetaRed())
		return( GetTeamPlayerCount(FindTeamInGroupBeta()) );
	return(0);
}

/******************************************************************************/
/******************************************************************************/
int CTeamingModel::GetTeamAliveCount(uint8 i_team) const
{
	int iCount = 0;
	for (uint8 i = 0; i < d_maxPlayers; i++)
		if ((g_referee.GetTeam(i) == i_team) && 
			!g_referee.IsGhost(i) && NParticipantConfig::Active(d_config[i]))
			iCount++;
	return( iCount );
}

/******************************************************************************/
/******************************************************************************/
int CTeamingModel::GetBlueAliveCount(void) const
{
	if (IsAlphaBlue())
		return( GetTeamAliveCount(FindTeamInGroupAlpha()) );
	else if (IsBetaBlue())
		return( GetTeamAliveCount(FindTeamInGroupBeta()) );
	return(0);
}

/******************************************************************************/
/******************************************************************************/
int CTeamingModel::GetRedAliveCount(void) const
{
	if (IsAlphaRed())
		return( GetTeamAliveCount(FindTeamInGroupAlpha()) );
	else if (IsBetaRed())
		return( GetTeamAliveCount(FindTeamInGroupBeta()) );
	return(0);
}

/******************************************************************************/
/******************************************************************************/
#define DEFINE_GET_NEXT(Relation, Function)                                    \
uint8 CTeamingModel::GetNext##Relation(uint8 i_idQuery, uint8 i_idLast,        \
		bool i_includeQuery, bool i_wrap, bool i_includeGhosts) const            \
{                                                                              \
	uint8 queryTeam;                                                            \
	if ((i_idQuery < d_maxParticipants) &&                                      \
			NParticipantConfig::ValidAssignedTeam(d_config[i_idQuery], queryTeam))\
	{                                                                           \
		uint8 i;                                                                 \
		if (i_idLast == INVALID_PARTICIPANT)                                     \
		{                                                                        \
			i = 0;                                                                \
			i_wrap = false;                                                       \
		}                                                                        \
		else                                                                     \
			i = i_idLast + 1;                                                     \
                                                                               \
		for (; i < d_maxParticipants; i++)                                       \
		{                                                                        \
			uint8 team;                                                           \
			if ((i_includeQuery || (i != i_idQuery)) &&                           \
					NParticipantConfig::ValidAssignedTeam(d_config[i], team) &&     \
					(i_includeGhosts || !g_referee.IsGhost(i)) &&                   \
					Function(queryTeam, team))                                      \
				return(i);                                                         \
		}                                                                        \
                                                                               \
		if (i_wrap)                                                              \
		{                                                                        \
			for (i = 0; i < i_idLast; i++)                                        \
			{                                                                     \
				uint8 team;                                                        \
				if ((i_includeQuery || (i != i_idQuery)) &&                        \
						NParticipantConfig::ValidAssignedTeam(d_config[i], team) &&  \
						(i_includeGhosts || !g_referee.IsGhost(i)) &&                \
						Function(queryTeam, team))                                   \
					return(i);                                                      \
			}                                                                     \
		}                                                                        \
	}                                                                           \
	return(INVALID_PARTICIPANT);                                                \
}                                                                              \

/******************************************************************************/
/******************************************************************************/
#define DEFINE_GET_PREV(Relation, Function)                                    \
uint8 CTeamingModel::GetPrev##Relation(uint8 i_idQuery, uint8 i_idLast,        \
		bool i_includeQuery, bool i_wrap, bool i_includeGhosts) const            \
{                                                                              \
	uint8 queryTeam;                                                            \
	if ((i_idQuery < d_maxParticipants) &&                                      \
			NParticipantConfig::ValidAssignedTeam(d_config[i_idQuery], queryTeam))\
	{                                                                           \
		uint8 i;                                                                 \
		if (i_idLast == INVALID_PARTICIPANT)                                     \
		{                                                                        \
			i = d_maxParticipants;                                                \
			i_wrap = false;                                                       \
		}                                                                        \
		else                                                                     \
			i = i_idLast;                                                         \
                                                                               \
		while (i > 0)                                                            \
		{                                                                        \
			i--;                                                                  \
			uint8 team;                                                           \
			if ((i_includeQuery || (i != i_idQuery)) &&                           \
					NParticipantConfig::ValidAssignedTeam(d_config[i], team) &&     \
					(i_includeGhosts || !g_referee.IsGhost(i)) &&                   \
					Function(queryTeam, team))                                      \
				return(i);                                                         \
		}                                                                        \
                                                                               \
		if (i_wrap)                                                              \
		{                                                                        \
			for (i = d_maxParticipants - 1; i > i_idLast; i--)                    \
			{                                                                     \
				uint8 team;                                                        \
				if ((i_includeQuery || (i != i_idQuery)) &&                        \
						NParticipantConfig::ValidAssignedTeam(d_config[i], team) &&  \
						(i_includeGhosts || !g_referee.IsGhost(i)) &&                \
						Function(queryTeam, team))                                   \
					return(i);                                                      \
			}                                                                     \
		}                                                                        \
	}                                                                           \
	return(INVALID_PARTICIPANT);                                                \
}                                                                              \

/******************************************************************************/
/******************************************************************************/
DEFINE_GET_NEXT(Teammate, AreTeammateTeams)
DEFINE_GET_NEXT(Friend, AreFriendTeams)
DEFINE_GET_NEXT(Enemy, AreEnemyTeams)
DEFINE_GET_NEXT(Neutral, AreNeutralTeams)
DEFINE_GET_NEXT(NonTeammate, !AreTeammateTeams)
DEFINE_GET_NEXT(NonFriend, !AreFriendTeams)
DEFINE_GET_NEXT(NonEnemy, !AreEnemyTeams)
DEFINE_GET_NEXT(NonNeutral, !AreNeutralTeams)
DEFINE_GET_PREV(Teammate, AreTeammateTeams)
DEFINE_GET_PREV(Friend, AreFriendTeams)
DEFINE_GET_PREV(Enemy, AreEnemyTeams)
DEFINE_GET_PREV(Neutral, AreNeutralTeams)
DEFINE_GET_PREV(NonTeammate, !AreTeammateTeams)
DEFINE_GET_PREV(NonFriend, !AreFriendTeams)
DEFINE_GET_PREV(NonEnemy, !AreEnemyTeams)
DEFINE_GET_PREV(NonNeutral, !AreNeutralTeams)
