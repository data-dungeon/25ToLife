#include "TTLPCH.h"
#include "InterfacesTTL/HealthIF.h"
#include "stage/cactor.h"

uint HealthIF::s_healthID = INTERFACE_ID_INVALID;

/******************************************************************************/
/******************************************************************************/
HealthIF* HealthIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((HealthIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
HealthIF* HealthIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((HealthIF*)actor->GetInterface(GetID()));
	return(NULL);
}
