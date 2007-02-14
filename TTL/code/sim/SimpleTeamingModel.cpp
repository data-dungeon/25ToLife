/*
**
**  File:   SimpleTeamingModel.cpp
**  Date:   October 5, 2004
**  By:     Bryant Collard
**  Desc:   The simple teaming model.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/SimpleTeamingModel.cpp $
**  $Revision: #13 $
**  $DateTime: 2005/07/13 17:59:16 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "sim/SimpleTeamingModel.h"

/******************************************************************************/
/******************************************************************************/
TEAMING_MODEL_CREATOR(CSimpleTeamingModel, Simple);

/******************************************************************************/
/******************************************************************************/
CSimpleTeamingModel::CSimpleTeamingModel(
		NParticipantConfig::TConfig* i_config, uint8* i_role, uint8 i_maxPlayers,
		uint8 i_maxParticipants, uint8 i_maxTeams) :
	CTeamingModel(i_config, i_role, i_maxPlayers, i_maxParticipants, i_maxTeams)
{
}
/******************************************************************************/
/******************************************************************************/
CPlayerProfile::EVariant CSimpleTeamingModel::GetTeamProfileVariant(uint8 i_id) const
{
	// the profile variant is special since it needs the role of the team in the 
	// next match after the old match is done, but before the next match begins.
	if ( (IsTeamInGroupAlpha( i_id ) && IsAlphaBlue())
		|| (IsTeamInGroupBeta( i_id ) && IsBetaBlue()) )
		return(CPlayerProfile::COP);
	else if ( (IsTeamInGroupAlpha( i_id ) && IsAlphaRed()) 
		|| (IsTeamInGroupBeta( i_id ) && IsBetaRed()) )
		return(CPlayerProfile::GANG);
	return(CPlayerProfile::INVALID);
}

/******************************************************************************/
/******************************************************************************/
const char *CSimpleTeamingModel::GetBlueTeamName(void) const
{
	return("Law Enforcement");
}

/******************************************************************************/
/******************************************************************************/
const char *CSimpleTeamingModel::GetRedTeamName(void) const
{
	return("Criminals");
}

