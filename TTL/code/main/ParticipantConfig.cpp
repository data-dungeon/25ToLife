/*
**
**  File:   ParticipantConfig.cpp
**  Date:   October 4, 2004
**  By:     Bryant Collard
**  Desc:   Team assignments and other configuration information is encoded in a
**          single value. A pariticipant contains its own configuration and the
**          refereee contains an array of configurations, one entry for each
**          participant. The configuration value is split into two halfs. The
**          LSBs are controlled by the referee and the MSBs are controlled by
**          the participant. So, the referee will copy the MSBs of a
**          participant's configuration value into its corresponding
**          configuration entry and set the LSBs. On the other hand, a
**          participant will copy the LSBs of its entry in the referee's
**          configuration array and set the MSBs.
**
**          The MSB of the participant half indicates whether or not a player is
**          ready during the ready-up phase and whether or not a participant is
**          alive during gameplay. The remaining bits express the team the
**          participant wants to be on.
**
**          The MSB of the referee half is set if the participant is in ghost
**          mode. The remaining bits are used for the team assigned by the
**          referee.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/ParticipantConfig.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/07/26 12:35:11 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "main/ParticipantConfig.h"

#define IS_VALID(c) ((c & TEAM_MASK) != TEAM_MASK)

/******************************************************************************/
/******************************************************************************/
namespace NParticipantConfig
{
	// Some constants used to slice and dice the configuration.
	static const uint    WIDTH                 = 8;
	static const uint    SHIFT                 = WIDTH >> 1;
	static const TConfig REFEREE_HALF_MASK     = ((TConfig)~0x0) >> SHIFT;
	static const TConfig PARTICIPANT_HALF_MASK = REFEREE_HALF_MASK << SHIFT;
	static const TConfig TEAM_MASK             = REFEREE_HALF_MASK >> 1;
	static const TConfig PARTICIPANT_ACTIVE    = 1 << (WIDTH - 1);
	static const TConfig PARTICIPANT_GHOST     = 1 << (SHIFT - 1);
};

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::Invalid(TConfig i_config)
{
	return(!IS_VALID(i_config));
}

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::ValidAssignedTeam(TConfig i_config, uint8 &o_team)
{
	o_team = i_config & TEAM_MASK;
	return(o_team != TEAM_MASK);
}

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::IsAssignedTeam(TConfig i_config, uint8 i_team)
{
	uint8 team = i_config & TEAM_MASK;
	return((team != TEAM_MASK) && (team  == (i_team & TEAM_MASK)));
}

/******************************************************************************/
/******************************************************************************/
uint8 NParticipantConfig::AssignedTeam(TConfig i_config)
{
	uint8 team = i_config & TEAM_MASK;
	return(team == TEAM_MASK ? INVALID_TEAM : team);
}

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::Ghost(TConfig i_config)
{
	return(IS_VALID(i_config) && ((i_config & PARTICIPANT_GHOST) != 0x0));
}

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::NotGhost(TConfig i_config)
{
	return(IS_VALID(i_config) && ((i_config & PARTICIPANT_GHOST) == 0x0));
}

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::InvalidRequestedTeam(TConfig i_config)
{
	return(!IS_VALID(i_config) ||
			(((i_config >> SHIFT) & TEAM_MASK) == TEAM_MASK));
}

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::ValidRequestedTeam(TConfig i_config, uint8 &o_team)
{
	o_team = (i_config >> SHIFT) & TEAM_MASK;
	return(IS_VALID(i_config) && (o_team != TEAM_MASK));
}

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::IsRequestedTeam(TConfig i_config, uint8 i_team)
{
	uint8 team = (i_config >> SHIFT) & TEAM_MASK;
	return(IS_VALID(i_config) && (team != TEAM_MASK) &&
			((i_team & TEAM_MASK) == team));
}

/******************************************************************************/
/******************************************************************************/
uint8 NParticipantConfig::RequestedTeam(TConfig i_config)
{
	uint8 team = (i_config >> SHIFT) & TEAM_MASK;
	return((!IS_VALID(i_config) || (team == TEAM_MASK)) ? INVALID_TEAM : team);
}

/******************************************************************************/
/******************************************************************************/
bool NParticipantConfig::Active(TConfig i_config)
{
	return(IS_VALID(i_config) && ((i_config & PARTICIPANT_ACTIVE) != 0x0));
}

/******************************************************************************/
/******************************************************************************/
void NParticipantConfig::SetInvalid(TConfig &o_config)
{
	o_config = TEAM_MASK;
}

/******************************************************************************/
/******************************************************************************/
void NParticipantConfig::SetAssignedTeam(TConfig &o_config, uint8 i_team)
{
	if ((i_team & TEAM_MASK) == TEAM_MASK)
		o_config = TEAM_MASK;
	else
	{
		o_config &= ~TEAM_MASK;
		o_config |= (i_team & TEAM_MASK);
	}
}

/******************************************************************************/
/******************************************************************************/
void NParticipantConfig::SetGhost(TConfig &o_config, bool i_ghost)
{
	if (i_ghost)
		o_config |= PARTICIPANT_GHOST;
	else
		o_config &= ~PARTICIPANT_GHOST;
}

/******************************************************************************/
/******************************************************************************/
void NParticipantConfig::SetInvalidRequestedTeam(TConfig &o_config)
{
	o_config |= TEAM_MASK << SHIFT;
}

/******************************************************************************/
/******************************************************************************/
void NParticipantConfig::SetRequestedTeam(TConfig &o_config, uint8 i_team)
{
	uint8 team = i_team & TEAM_MASK;
	if (team == TEAM_MASK)
		o_config |= TEAM_MASK << SHIFT;
	else
	{
		o_config &= ~(TEAM_MASK << SHIFT);
		o_config |= team << SHIFT;
	}
}

/******************************************************************************/
/******************************************************************************/
void NParticipantConfig::SetActive(TConfig &o_config, bool i_active)
{
	if (i_active)
		o_config |= PARTICIPANT_ACTIVE;
	else
		o_config &= ~PARTICIPANT_ACTIVE;
}
