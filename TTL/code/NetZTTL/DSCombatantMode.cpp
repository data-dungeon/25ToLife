/*
**
**  File:   DSCombatantMode.cpp
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the combatant mode dataset.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSCombatantMode.cpp $
**  $Revision: #1 $
**  $DateTime: 2004/05/06 17:27:25 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSCombatantMode.h"

// Local headers
#include "NetZTTL/DOCombatant.h"

/**********************************************************************/
/**********************************************************************/
void DSCombatantMode::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		DuplicatedObject* associatedDO = i_operation->GetAssociatedDO();
		if (associatedDO->IsAKindOf(DOCLASSID(DOCombatant)))
			((DOCombatant*)associatedDO)->StateChanged();
	}
}
