/*
**
**  File:   DSAppearance.cpp
**  Date:   December 13, 2004
**  By:     Bryant Collard
**  Desc:   Dataset describing the appearance of a player's avatar.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSAppearance.cpp $
**  $Revision: #2 $
**  $DateTime: 2005/03/10 19:14:44 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSAppearance.h"

// Local headers
#include "NetZTTL/DOPlayerAgent.h"

/******************************************************************************/
/******************************************************************************/
void DSAppearance::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		DuplicatedObject* associatedDO = i_operation->GetAssociatedDO();
		if ((associatedDO != NULL) &&
				associatedDO->IsAKindOf(DOCLASSID(DOPlayerAgent)))
		{
			g_messageDispatcher.SendMessageToAll("PlayerChanged",
					(void*)(((DOPlayerAgent*)associatedDO)->Id()),
					INVALID_OBJECT_HANDLE);
		}
	}
}
