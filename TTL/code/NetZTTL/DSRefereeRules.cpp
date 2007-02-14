/*
**
**  File:   DSRefereeRules.h
**  Date:   October 2, 2004
**  By:     Bryant Collard
**  Desc:   Dataset describing the current rules.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeRules.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/07/13 17:59:16 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSRefereeRules.h"

// TTL headers.
#include "environ/ConfigMgr.h"

/******************************************************************************/
/******************************************************************************/
void DSRefereeRules::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		g_configmgr.SetOptions(d_desc.CStr());
		g_messageDispatcher.SendMessageToAll("TeamRoleChanged", NULL,
				INVALID_OBJECT_HANDLE);
	}
}
