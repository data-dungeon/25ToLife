/*
**
**  File:   DSRefereeId.cpp
**  Date:   October 2, 2004
**  By:     Bryant Collard
**  Desc:   Dataset containing the player's id assignments.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeId.cpp $
**  $Revision: #2 $
**  $DateTime: 2005/03/10 19:14:44 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSRefereeId.h"

// GameLib headers
#include "GameHelper/LogUtil.h"

// TTL headers
#include "NetZTTL/DOReferee.h"

/******************************************************************************/
/******************************************************************************/
void DSRefereeId::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		ASSERT(i_operation->GetAssociatedDO()->IsAKindOf(DOCLASSID(DOReferee)));
		((DOReferee*)i_operation->GetAssociatedDO())->UpdateIds();

		SLogMessage logMessage;
		logMessage.d_type = SLogMessage::LOG_ACTION;
		logMessage.d_label = "Referee";
		logMessage.d_action = "Id";
		g_messageDispatcher.SendMessageToAll("LogEntry", &logMessage,
				INVALID_OBJECT_HANDLE);
	}
}
