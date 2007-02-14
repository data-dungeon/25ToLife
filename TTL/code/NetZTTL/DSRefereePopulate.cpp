/*
**
**  File:   DSRefereePopulate.cpp
**  Date:   October 9, 2004
**  By:     Bryant Collard
**  Desc:   Dataset describing to help the referee manage populating.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSRefereePopulate.h"

// GameLib headers
#include "GameHelper/LogUtil.h"

// Local headers
#include "NetZTTL/DOReferee.h"

/******************************************************************************/
/******************************************************************************/
void DSRefereePopulate::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
	{
		DOReferee::Ref refReferee(g_doReferee);
		if (refReferee.IsValid())
			refReferee->SetWorldExtents();

		SLogMessage logMessage;
		logMessage.d_type = SLogMessage::LOG_ACTION;
		logMessage.d_label = "Populate";
		logMessage.d_action = "Update";
		g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
				INVALID_OBJECT_HANDLE);
	}
}
