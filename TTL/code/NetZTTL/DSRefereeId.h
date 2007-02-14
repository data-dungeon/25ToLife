/*
**
**  File:   DSRefereeId.h
**  Date:   October 2, 2004
**  By:     Bryant Collard
**  Desc:   Dataset containing the player's id assignments.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeId.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_REFEREE_ID_H
#define DS_REFEREE_ID_H

#include <netz.h>
#include "NetZTTL/DSRefereeIdDDL.h"

class DSRefereeId : public DATASET(DSRefereeId)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_REFEREE_ID
