/*
**
**  File:   SimpleTeamingModel.h
**  Date:   October 5, 2004
**  By:     Bryant Collard
**  Desc:   The simple teaming model.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/SimpleTeamingModel.h $
**  $Revision: #6 $
**  $DateTime: 2005/07/13 17:59:16 $
**    $Author: Bryant_Collard $
**
*/

#ifndef SIMPLE_TEAMING_MODEL_H
#define SIMPLE_TEAMING_MODEL_H

#include "sim/TeamingModel.h"

class CSimpleTeamingModel : public CTeamingModel
{
  public:
	CSimpleTeamingModel(NParticipantConfig::TConfig* i_config, uint8* i_role,
			uint8 i_maxPlayers, uint8 i_maxParticipants, uint8 i_maxTeams);

	// Query team role
	virtual CPlayerProfile::EVariant GetTeamProfileVariant(uint8 i_id) const;

	// Query team's name
	virtual const char *GetRedTeamName(void) const;
	virtual const char *GetBlueTeamName(void) const;
};

#endif // SIMPLE_TEAMING_MODEL_H
