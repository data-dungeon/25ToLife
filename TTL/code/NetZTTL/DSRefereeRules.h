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
**      $File: //depot/TTL/code/NetZTTL/DSRefereeRules.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_REFEREE_RULES_H
#define DS_REFEREE_RULES_H

#include <netz.h>
#include "NetZTTL/DSRefereeRulesDDL.h"

class DSRefereeRules : public DATASET(DSRefereeRules)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_REFEREE_RULES
