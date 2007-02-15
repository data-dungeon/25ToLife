/*
**
**  File:   DSSessionLock.cpp
**  Date:   August 31, 2004
**  By:     Bryant Collard
**  Desc:   A mechanism to "lock" a session. A session lock is required when
**          a host wants to perform some coordinated action such as emmigrating
**          NPCs or leaving a session.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSSessionLock.cpp $
**  $Revision: #4 $
**  $DateTime: 2005/08/29 20:13:34 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DSSessionLock.h"

// GameLib headers
#include "GameHelper/LogUtil.h"
#include "GameHelper/DebugOutput.h"

// Local headers
#include "Network/Session.h"
#include "Network/NetZ/DOHost.h"

/******************************************************************************/
/******************************************************************************/
void DSSessionLock::OperationEnd(DOOperation* i_operation)
{
	switch(i_operation->GetType())
	{
	 case Operation::AddToStore:
	 case Operation::RemoveFromStore:
	 case Operation::UpdateDataSet:
	 {
		SLogMessage logMessage;
		logMessage.d_type = SLogMessage::LOG_SUBJECT;
		switch(i_operation->GetType())
		{
		 case Operation::AddToStore:
			logMessage.d_label = "Add";
			break;
		 case Operation::RemoveFromStore:
			logMessage.d_label = "Remove";
			break;
		 case Operation::UpdateDataSet:
			logMessage.d_label = "Update";
			break;
		}
		logMessage.d_subject =
				i_operation->GetAssociatedDO()->GetHandle().GetValue();
		g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
				INVALID_OBJECT_HANDLE);
#if 0
		if (!i_operation->GetAssociatedDO()->IsAKindOf(DOCLASSID(DOHost)))
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Lock not associated with DOHost");
		else if (d_grantee == INVALID_DOHANDLE)
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Grantor = %s, Grantee = NONE, ReqId = %d, GrantId = %d",
						((DOHost*)i_operation->GetAssociatedDO())->GetName(),
						d_ids & 0x0F, (d_ids & 0xF0) >> 4);
		else if (!d_grantee.IsAKindOf(DOCLASSID(DOHost)))
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Lock grantee not a DOHost");
		else
		{
			DOHost::Ref granteeRef = d_grantee;
			if (!granteeRef.IsValid())
				g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
						"Lock grantee not valid");
			else
				g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
						"Grantor = %s, Grantee = %s, ReqId = %d, GrantId = %d",
						((DOHost*)i_operation->GetAssociatedDO())->GetName(),
						granteeRef->GetName(), d_ids & 0x0F, (d_ids & 0xF0) >> 4);
		}
#endif
		g_session.UpdateLock();
	 }
	}
}
