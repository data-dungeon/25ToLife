/*
**
**  File:   AnimActionIF.h
**  Date:   February 5, 2004
**  By:     Bryant Collard
**  Desc:   An interface to animation actions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/AnimActionIF.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/AnimActionIF.h"
#include "stage/cactor.h"

uint CAnimActionIF::s_animActionID = INTERFACE_ID_INVALID;

/******************************************************************************/
/******************************************************************************/
CAnimActionIF* CAnimActionIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CAnimActionIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CAnimActionIF* CAnimActionIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CAnimActionIF*)actor->GetInterface(GetID()));
	return(NULL);
}
