/*
**
**  File:   DSRefereeMatch.h
**  Date:   July 19, 2005
**  By:     Bryant Collard
**  Desc:   Dataset containing information about the current match.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeMatch.h $
**  $Revision: #2 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_REFEREE_MATCH_H
#define DS_REFEREE_MATCH_H

#include <netz.h>
#include "NetZTTL/DSRefereeMatchDDL.h"

class DSRefereeMatch : public DATASET(DSRefereeMatch)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_REFEREE_MATCH
