/*
**
**  File:   DSParticipant.cpp
**  Date:   September 30, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the participant dataset.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSParticipant.cpp $
**  $Revision: #6 $
**  $DateTime: 2005/08/03 22:27:28 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSParticipant.h"

// GameLib headers
#include "GameHelper/LogUtil.h"

// Local headers
#include "main/TTLConstants.h"
#include "NetZTTL/DOReferee.h"
#include "NetZTTL/DOPlayerAgent.h"

#define MAKE_ID(i) (i & ~SPAWN_TICKET_MASK)

/******************************************************************************/
/******************************************************************************/
DSParticipant::DSParticipant()
{
	d_id = INVALID_PARTICIPANT;
	NParticipantConfig::SetInvalid(d_config);
	d_isMaster = true;
}

/******************************************************************************/
/******************************************************************************/
void DSParticipant::OperationEnd(DOOperation* i_operation)
{
	SLogMessage logMessage;
	logMessage.d_type = SLogMessage::LOG_SUBJECT;

	switch(i_operation->GetType())
	{
	 case Operation::AddToStore:
	 {
		DOReferee::Ref refReferee(g_doReferee);
		if (refReferee.IsValid())
			refReferee->SubmitParticipant(i_operation->GetAssociatedDO(),
					MAKE_ID(d_id), d_config);

		logMessage.d_label = "Add";
		logMessage.d_subject =
				i_operation->GetAssociatedDO()->GetHandle().GetValue();
		g_messageDispatcher.SendMessageToAll("LogEntry", &logMessage,
				INVALID_OBJECT_HANDLE);
		break;
	 }
	 case Operation::RemoveFromStore:
	 {
		DOReferee::Ref refReferee(g_doReferee);
		if (refReferee.IsValid())
			refReferee->WithdrawParticipant(i_operation->GetAssociatedDO(),
					MAKE_ID(d_id));

		logMessage.d_label = "Remove";
		logMessage.d_subject =
				i_operation->GetAssociatedDO()->GetHandle().GetValue();
		g_messageDispatcher.SendMessageToAll("LogEntry", &logMessage,
				INVALID_OBJECT_HANDLE);
		break;
	 }
	 case Operation::UpdateDataSet:
	 {
		DuplicatedObject* associatedDO = i_operation->GetAssociatedDO();
		if ((associatedDO != NULL) &&
				associatedDO->IsAKindOf(DOCLASSID(DOPlayerAgent)))
			((DOPlayerAgent*)associatedDO)->ParticipantChanged();

		DOReferee::Ref refReferee(g_doReferee);
		if (refReferee.IsValid())
		{
			refReferee->UpdateParticipant(associatedDO, MAKE_ID(d_id));
			refReferee->UpdateConfig(MAKE_ID(d_id), d_config);
		}

		logMessage.d_label = "Update";
		logMessage.d_subject =
				i_operation->GetAssociatedDO()->GetHandle().GetValue();
		g_messageDispatcher.SendMessageToAll("LogEntry", &logMessage,
				INVALID_OBJECT_HANDLE);
		break;
	 }
	}
}

/******************************************************************************/
/******************************************************************************/
void DSParticipant::Initialize(uint8 i_id, NParticipantConfig::TConfig i_config,
		bool i_ticketIssued)
{
	if (d_isMaster)
	{
		bool update = false;

		ASSERT(i_id == MAKE_ID(i_id));
		if (i_ticketIssued)
			i_id |= SPAWN_TICKET_MASK;

		if (d_id != i_id)
		{
			update = true;
			d_id = i_id;
		}

		if (i_config != d_config)
		{
			update = true;
			NParticipantConfig::SetAssignedTeam(d_config,
					NParticipantConfig::AssignedTeam(i_config));
			NParticipantConfig::SetGhost(d_config,
					NParticipantConfig::Ghost(i_config));
			NParticipantConfig::SetRequestedTeam(d_config,
					NParticipantConfig::RequestedTeam(i_config));
			NParticipantConfig::SetActive(d_config,
					NParticipantConfig::Active(i_config));
		}

		if (update)
			RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DSParticipant::SetId(uint8 i_id)
{
	ASSERT(i_id == MAKE_ID(i_id));
	if (d_isMaster && (MAKE_ID(d_id) != i_id))
	{
		d_id &= SPAWN_TICKET_MASK;
		d_id |= i_id;
		RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DSParticipant::RequestTeam(uint8 i_team)
{
	if (d_isMaster && !NParticipantConfig::IsRequestedTeam(d_config, i_team))
	{
		NParticipantConfig::SetRequestedTeam(d_config, i_team);
		RequestUpdate();

		DOReferee::Ref refReferee(g_doReferee);
		if (refReferee.IsValid())
			refReferee->UpdateConfig(MAKE_ID(d_id), d_config);
	}
}

/******************************************************************************/
/******************************************************************************/
void DSParticipant::SetActive(bool i_active)
{
	if (d_isMaster && (NParticipantConfig::Active(d_config) != i_active))
	{
		NParticipantConfig::SetActive(d_config, i_active);
		RequestUpdate();

		DOReferee::Ref refReferee(g_doReferee);
		if (refReferee.IsValid())
			refReferee->UpdateConfig(MAKE_ID(d_id), d_config);

		SLogMessage logMessage;
		logMessage.d_type = SLogMessage::LOG_ACTION;
		logMessage.d_label = "Participant";
		logMessage.d_action = "Ready";
		g_messageDispatcher.SendMessageToAll("LogEntry", &logMessage,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void DSParticipant::SetSpawnTicket(bool i_ticketIssued)
{
	if (d_isMaster && (((d_id & SPAWN_TICKET_MASK) != 0) != i_ticketIssued))
	{
		if (i_ticketIssued)
			d_id |= SPAWN_TICKET_MASK;
		else
			d_id &= ~SPAWN_TICKET_MASK;
		RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
uint8 DSParticipant::GetId(void) const
{
	return(MAKE_ID(d_id));
}

/******************************************************************************/
/******************************************************************************/
uint8 DSParticipant::GetRequestedTeam(void) const
{
	return(NParticipantConfig::RequestedTeam(d_config));
}

/******************************************************************************/
/******************************************************************************/
bool DSParticipant::GetActive(void) const
{
	return(NParticipantConfig::Active(d_config));
}

/******************************************************************************/
/******************************************************************************/
bool DSParticipant::GetSpawnTicket(void) const
{
	return((d_id & SPAWN_TICKET_MASK) != 0);
}
