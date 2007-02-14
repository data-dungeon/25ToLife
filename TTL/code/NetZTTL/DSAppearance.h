/*
**
**  File:   DSAppearance.h
**  Date:   December 13, 2004
**  By:     Bryant Collard
**  Desc:   Dataset describing the appearance of a player's avatar.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSAppearance.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_APPEARANCE_H
#define DS_APPEARANCE_H

#include <netz.h>
#include "NetZTTL/DSAppearanceDDL.h"

class DSAppearance : public DATASET(DSAppearance)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_APPEARANCE_H
