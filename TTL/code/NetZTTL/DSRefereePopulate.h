/*
**
**  File:   DSRefereePopulate.h
**  Date:   October 9, 2004
**  By:     Bryant Collard
**  Desc:   Dataset describing to help the referee manage populating.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereePopulate.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_REFEREE_POPULATE_H
#define DS_REFEREE_POPULATE_H

#include <netz.h>
#include "NetZTTL/DSRefereePopulateDDL.h"

class DSRefereePopulate : public DATASET(DSRefereePopulate)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_REFEREE_POPULATE
