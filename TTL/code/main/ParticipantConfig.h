/*
**
**  File:   ParticipantConfig.h
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
**      $File: //depot/TTL/code/main/ParticipantConfig.h $
**  $Revision: #3 $
**  $DateTime: 2005/07/26 12:35:11 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef PARTICIPANT_CONFIG_H
#define PARTICIPANT_CONFIG_H

#include "platform/BaseType.h"

namespace NParticipantConfig
{
	// Typedef the type used so it can be changed easily. Remember to change
	// the width in ParticipantConfig.cpp.
	typedef uint8 TConfig;

	// Referee access
	bool Invalid(TConfig i_config);
	bool ValidAssignedTeam(TConfig i_config, uint8 &o_team);
	bool IsAssignedTeam(TConfig i_config, uint8 i_team);
	uint8 AssignedTeam(TConfig i_config);
	bool Ghost(TConfig i_config);
	bool NotGhost(TConfig i_config);

	// Participant access
	bool InvalidRequestedTeam(TConfig i_config);
	bool ValidRequestedTeam(TConfig i_config, uint8 &o_team);
	bool IsRequestedTeam(TConfig i_config, uint8 i_team);
	uint8 RequestedTeam(TConfig i_config);
	bool Active(TConfig i_config);

	// Referee set
	void SetInvalid(TConfig &o_config);
	void SetAssignedTeam(TConfig &o_config, uint8 i_team);
	void SetGhost(TConfig &o_config, bool i_ghost);

	// Participant set
	void SetInvalidRequestedTeam(TConfig &o_config);
	void SetRequestedTeam(TConfig &o_config, uint8 i_team);
	void SetActive(TConfig &o_config, bool i_active);
};

#endif // PARTICIPANT_CONFIG_H
