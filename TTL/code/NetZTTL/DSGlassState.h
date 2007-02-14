/*
**
**  File:   DSGlassState.h
**  Date:   March 4, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the glass state dataset.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSGlassState.h $
**  $Revision: #4 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_GLASS_STATE_H
#define DS_GLASS_STATE_H

#include <netz.h>
#include "NetZTTL/DSGlassStateDDL.h"

class DSGlassState : public DATASET(DSGlassState)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_GLASS_STATE
