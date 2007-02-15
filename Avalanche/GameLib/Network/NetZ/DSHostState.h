/*
**
**  File:   DSHostState.h
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the host state dataset.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSHostState.h $
**  $Revision: #4 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_HOST_STATE_H
#define DS_HOST_STATE_H

#include <netz.h>
#include "Network/NetZ/DSHostStateDDL.h"

class DSHostState : public DATASET(DSHostState)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_HOST_STATE
