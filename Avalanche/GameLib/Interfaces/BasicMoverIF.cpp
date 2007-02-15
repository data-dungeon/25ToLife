//============================================================================
//=
//= BasicMoverIF.cpp - Interface for basic movement, such as rotating and
//=                    and translation.
//=
//============================================================================

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/BasicMoverIF.h"
#include "stage/cactor.h"

uint CBasicMoverIF::s_basicMoverID = INTERFACE_ID_INVALID;

//============================================================================

CBasicMoverIF* CBasicMoverIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CBasicMoverIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

//============================================================================

CBasicMoverIF* CBasicMoverIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CBasicMoverIF*)actor->GetInterface(GetID()));
	return(NULL);
}

//============================================================================
