/*
**
**  File:   DSRefereeAssignment.h
**  Date:   October 2, 2004
**  By:     Bryant Collard
**  Desc:   Dataset containing the player's team, etc. assignments.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSRefereeAssignment.h $
**  $Revision: #5 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_REFEREE_ASSIGNMENT_H
#define DS_REFEREE_ASSIGNMENT_H

#include <netz.h>
#include "NetZTTL/DSRefereeAssignmentDDL.h"

class DSRefereeAssignment : public DATASET(DSRefereeAssignment)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_REFEREE_ASSIGNMENT
