/*
**
**  File:   DSRefereeAssignment.h
**  Date:   October 2, 2004
**  By:     Bryant Collard
**  Desc:   Dataset containing the player's team, etc. assignments.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeAssignment.cpp $
**  $Revision: #4 $
**  $DateTime: 2005/07/27 12:18:24 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSRefereeAssignment.h"

// GameLib headers
#include "GameHelper/LogUtil.h"

// TTL headers
#include "NetZTTL/DOReferee.h"

/******************************************************************************/
/******************************************************************************/
void DSRefereeAssignment::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		g_messageDispatcher.SendMessageToAll("RefereeAssignmentChanged", NULL,
				INVALID_OBJECT_HANDLE);

		SLogMessage logMessage;
		logMessage.d_type = SLogMessage::LOG_ACTION;
		logMessage.d_label = "Referee";
		logMessage.d_action = "Assign";
		g_messageDispatcher.SendMessageToAll("LogEntry", &logMessage,
				INVALID_OBJECT_HANDLE);
	}
}
