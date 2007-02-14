/*
**
**  File:   TagTeamingModel.cpp
**  Date:   November 17, 2004
**  By:     Bryce Thomsen
**  Desc:   The tag teaming model.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/TagTeamingModel.cpp $
**  $Revision: #13 $
**  $DateTime: 2005/07/13 17:59:16 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "sim/TagTeamingModel.h"

/******************************************************************************/
/******************************************************************************/
TEAMING_MODEL_CREATOR(CTagTeamingModel, Tag);

/******************************************************************************/
/******************************************************************************/
CTagTeamingModel::CTagTeamingModel(
		NParticipantConfig::TConfig* i_config, uint8* i_role, uint8 i_maxPlayers,
		uint8 i_maxParticipants, uint8 i_maxTeams) :
	CTeamingModel(i_config, i_role, i_maxPlayers, i_maxParticipants, i_maxTeams)
{
}

/******************************************************************************/
/******************************************************************************/
CPlayerProfile::EVariant CTagTeamingModel::GetTeamProfileVariant(uint8 i_id) const
{
	// the profile variant is special since it needs the role of the team in the 
	// next match after the old match is done, but before the next match begins.
	if ( (IsTeamInGroupAlpha( i_id ) && IsAlphaBlue())
		|| (IsTeamInGroupBeta( i_id ) && IsBetaBlue()) )
		return(CPlayerProfile::GANG);
	else if ( (IsTeamInGroupAlpha( i_id ) && IsAlphaRed()) 
		|| (IsTeamInGroupBeta( i_id ) && IsBetaRed()) )
		return(CPlayerProfile::GANG);
	return(CPlayerProfile::INVALID);
}

/******************************************************************************/
/******************************************************************************/
const char *CTagTeamingModel::GetBlueTeamName(void) const
{
	return("Blue");
}

/******************************************************************************/
/******************************************************************************/
const char *CTagTeamingModel::GetRedTeamName(void) const
{
	return("Red");
}

