/*
**
**  File:   DSActorInteraction.cpp
**  Date:   April 29, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the actor interaction dataset.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSActorInteraction.cpp $
**  $Revision: #1 $
**  $DateTime: 2004/05/06 17:27:25 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DSActorInteraction.h"

// Local headers
#include "Network/NetZ/DOActor.h"

/**********************************************************************/
/**********************************************************************/
void DSActorInteraction::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		DuplicatedObject* associatedDO = i_operation->GetAssociatedDO();
		if (associatedDO->IsAKindOf(DOCLASSID(DOActor)))
			((DOActor*)associatedDO)->StateChanged();
	}
}
