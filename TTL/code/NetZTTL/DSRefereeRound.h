/*
**
**  File:   DSRefereeRound.h
**  Date:   December 1, 2004
**  By:     Bryant Collard
**  Desc:   Dataset containing the round id.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeRound.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_REFEREE_ROUND_H
#define DS_REFEREE_ROUND_H

#include <netz.h>
#include "NetZTTL/DSRefereeRoundDDL.h"

class DSRefereeRound : public DATASET(DSRefereeRound)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_REFEREE_ROUND
