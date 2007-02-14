/*
**
**  File:   DSRefereeRound.cpp
**  Date:   December 1, 2004
**  By:     Bryant Collard
**  Desc:   Dataset containing the round id.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeRound.cpp $
**  $Revision: #2 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DSRefereeRound.h"

// TTL headers
#include "main/Lobby.h"

/******************************************************************************/
/******************************************************************************/
void DSRefereeRound::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::UpdateDataSet)
		g_lobby.SetRoundId(d_roundId);
}
