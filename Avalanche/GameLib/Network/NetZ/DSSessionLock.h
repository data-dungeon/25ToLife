/*
**
**  File:   DSSessionLock.h
**  Date:   August 31, 2004
**  By:     Bryant Collard
**  Desc:   A mechanism to "lock" a session. A session lock is required when
**          a host wants to perform some coordinated action such as emmigrating
**          NPCs or leaving a session.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSSessionLock.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_SESSION_LOCK_H
#define DS_SESSION_LOCK_H

#include <netz.h>
#include "Network/NetZ/DSSessionLockDDL.h"

class DSSessionLock : public DATASET(DSSessionLock)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_SESSION_LOCK
