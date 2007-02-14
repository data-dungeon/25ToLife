/*
**
**  File:   DSGlassState.cpp
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the glass state dataset.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSGlassState.cpp $
**  $Revision: #2 $
**  $DateTime: 2004/04/01 19:13:23 $
**    $Author: Dwain_Skinner $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSGlassState.h"

// Local headers
#include "NetZTTL/DOGlass.h"

/**********************************************************************/
/**********************************************************************/
void DSGlassState::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		DuplicatedObject* associatedDO = i_operation->GetAssociatedDO();
		if (associatedDO->IsAKindOf(DOCLASSID(DOGlass)))
			((DOGlass*)associatedDO)->StateChanged();
	}
}
