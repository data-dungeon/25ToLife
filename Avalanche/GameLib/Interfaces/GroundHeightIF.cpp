/*
**
**  File:   GroundHeightIF.cpp
**  Date:   August 18, 2005
**  By:     Bryant Collard
**  Desc:   An interface used to test for ground.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/GroundHeightIF.cpp $
**  $Revision: #1 $
**  $DateTime: 2005/08/19 15:38:47 $
**    $Author: Bryant_Collard $
**
*/

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/GroundHeightIF.h"
#include "stage/cactor.h"

uint CGroundHeightIF::s_groundHeightID = INTERFACE_ID_INVALID;

/******************************************************************************/
/******************************************************************************/
CGroundHeightIF* CGroundHeightIF::GetInterface(CActor &i_actor)
{
	return((CGroundHeightIF*)i_actor.GetInterface(GetID()));
}

/******************************************************************************/
/******************************************************************************/
CGroundHeightIF* CGroundHeightIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CGroundHeightIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CGroundHeightIF* CGroundHeightIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CGroundHeightIF*)actor->GetInterface(GetID()));
	return(NULL);
}
