/*
**
**  File:   TagTeamingModel.h
**  Date:   November 17, 2004
**  By:     Bryce Thomsen
**  Desc:   The tag teaming model.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/TagTeamingModel.h $
**  $Revision: #5 $
**  $DateTime: 2005/07/13 17:59:16 $
**    $Author: Bryant_Collard $
**
*/

#ifndef TAG_TEAMING_MODEL_H
#define TAG_TEAMING_MODEL_H

#include "sim/TeamingModel.h"

class CTagTeamingModel : public CTeamingModel
{
  public:
	CTagTeamingModel(NParticipantConfig::TConfig* i_config, uint8* i_role,
			uint8 i_maxPlayers, uint8 i_maxParticipants, uint8 i_maxTeams);

	// Query team role
	virtual CPlayerProfile::EVariant GetTeamProfileVariant(uint8 i_id) const;

	// Query team's name
	virtual const char *GetRedTeamName(void) const;
	virtual const char *GetBlueTeamName(void) const;
};

#endif // TAG_TEAMING_MODEL_H
