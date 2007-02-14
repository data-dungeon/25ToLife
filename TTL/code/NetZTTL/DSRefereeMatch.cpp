/*
**
**  File:   DSRefereeMatch.cpp
**  Date:   July 19, 2005
**  By:     Bryant Collard
**  Desc:   Dataset containing information about the current match.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeMatch.cpp $
**  $Revision: #1 $
**  $DateTime: 2005/07/19 14:28:45 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSRefereeMatch.h"

// TTL headers.
#include "environ/ConfigMgr.h"
#include "NetZTTL/DOReferee.h"

/******************************************************************************/
/******************************************************************************/
void DSRefereeMatch::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		ASSERT(i_operation->GetAssociatedDO()->IsAKindOf(DOCLASSID(DOReferee)));
		((DOReferee*)i_operation->GetAssociatedDO())->ComputeCurrentPhaseEndTime();
	}
}
