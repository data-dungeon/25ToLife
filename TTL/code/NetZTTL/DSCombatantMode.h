/*
**
**  File:   DSCombatantMode.h
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the combatant mode dataset.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSCombatantMode.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_COMBATANT_MODE_H
#define DS_COMBATANT_MODE_H

#include <netz.h>
#include "NetZTTL/DSCombatantModeDDL.h"

class DSCombatantMode : public DATASET(DSCombatantMode)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_COMBATANT_MODE
