//============================================================================
//=
//= InventoryListIF.cpp - Interface for basic player inventory, such as rotating and
//=								weapons, ammo, keys, etc.
//=
//============================================================================

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/InventoryListIF.h"
#include "stage/cactor.h"

uint CInventoryListIF::s_inventoryListID = INTERFACE_ID_INVALID;

//============================================================================

CInventoryListIF* CInventoryListIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CInventoryListIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

//============================================================================

CInventoryListIF* CInventoryListIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CInventoryListIF*)actor->GetInterface(GetID()));
	return(NULL);
}

//============================================================================
