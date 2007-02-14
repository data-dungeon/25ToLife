/*
**
**  File:   DSParticipant.h
**  Date:   September 30, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the participant dataset.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSParticipant.h $
**  $Revision: #6 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_PARTICIPANT_H
#define DS_PARTICIPANT_H

#include <netz.h>
#include "NetZTTL/DSParticipantDDL.h"
#include "InterfacesTTL/ParticipantIF.h"

class DSParticipant : public DATASET(DSParticipant), public CNetParticipant
{
  public:
	DSParticipant();
	virtual ~DSParticipant() {}
	void OperationEnd(DOOperation* i_operation);
	virtual void Initialize(uint8 i_id, NParticipantConfig::TConfig i_config,
			bool i_ticketIssued);
	virtual void SetId(uint8 i_id);
	virtual void RequestTeam(uint8 i_team);
	virtual void SetActive(bool i_active);
	virtual void SetSpawnTicket(bool i_ticketIssued);
	virtual uint8 GetId(void) const;
	virtual uint8 GetRequestedTeam(void) const;
	virtual bool GetActive(void) const;
	virtual bool GetSpawnTicket(void) const;
	void SetIsMaster(bool i_isMaster) {d_isMaster = i_isMaster;}

  private:
	bool d_isMaster;
};

#endif // DS_PARTICIPANT
