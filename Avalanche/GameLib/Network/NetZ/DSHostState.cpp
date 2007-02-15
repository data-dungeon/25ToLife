/*
**
**  File:   DSHostState.cpp
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the host distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSHostState.cpp $
**  $Revision: #3 $
**  $DateTime: 2004/09/13 15:22:31 $
**    $Author: Rob $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DSHostState.h"

// Local headers
#include "Network/NetZ/DOHost.h"

/**********************************************************************/
/**********************************************************************/
void DSHostState::OperationEnd(DOOperation* i_operation)
{
	switch ( i_operation->GetType() )
	{
		case Operation::UpdateDataSet:
		case Operation::AddToStore:
		{
			DuplicatedObject* associatedDO = i_operation->GetAssociatedDO();
			if (associatedDO->IsAKindOf(DOCLASSID(DOHost)) &&
				 associatedDO->IsADuplica())
			{
				((DOHost*)associatedDO)->StateChanged();
			}
		}
		break;
	}
}
