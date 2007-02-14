#include "TTLPCH.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "stage/cactor.h"

uint CParticipantIF::s_participantID = INTERFACE_ID_INVALID;

/******************************************************************************/
/******************************************************************************/
CParticipantIF* CParticipantIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CParticipantIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CParticipantIF* CParticipantIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CParticipantIF*)actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
uint8 CParticipantIF::GetParticipantIdFromActor(CActor* i_actor)
{
	if (i_actor != NULL)
	{
		CParticipantIF* participant =
				(CParticipantIF*)i_actor->GetInterface(GetID());
		if (participant != NULL)
			return(participant->GetParticipantId());
	}
	return(INVALID_PARTICIPANT);
}

/******************************************************************************/
/******************************************************************************/
uint8 CParticipantIF::GetParticipantIdFromActor(ActorHandle i_actor)
{
	CActor* actor = CActor::FromHandle(i_actor);
	if (actor != NULL)
	{
		CParticipantIF* participant =
				(CParticipantIF*)actor->GetInterface(GetID());
		if (participant != NULL)
			return(participant->GetParticipantId());
	}
	return(INVALID_PARTICIPANT);
}

