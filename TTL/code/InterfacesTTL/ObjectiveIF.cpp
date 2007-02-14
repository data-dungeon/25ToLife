/*
**
**  File:   ObjectiveIF.cpp
**  Date:   July 14, 2005
**  By:     Bryant Collard
**  Desc:   Interfaces to get at objective attributes.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/InterfacesTTL/ObjectiveIF.cpp $
**  $Revision: #1 $
**  $DateTime: 2005/07/16 10:40:25 $
**    $Author: Bryant_Collard $
**
*/

// Pre-compiled header
#include "TTLPCH.h"

// My header
#include "InterfacesTTL/ObjectiveIF.h"

uint CObjectiveIF::s_objectiveID = INTERFACE_ID_INVALID;

/******************************************************************************/
/******************************************************************************/
CObjectiveIF* CObjectiveIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CObjectiveIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CObjectiveIF* CObjectiveIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CObjectiveIF*)actor->GetInterface(GetID()));
	return(NULL);
}
