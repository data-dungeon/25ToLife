/*
**
**  File:   DSGroundFollower.h
**  Date:   April 30, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of a ground follower dataset.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSGroundFollower.h $
**  $Revision: #11 $
**  $DateTime: 2005/08/15 17:04:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_GROUND_FOLLOWER_H
#define DS_GROUND_FOLLOWER_H

#include <netz.h>
#include "Network/NetZ/DSGroundFollowerDDL.h"
#include "Network/NetZ/DRModel.h"

class DRGroundFollowerModel;

class DSGroundFollower : public DATASET(DSGroundFollower)
{
  public:
	DSGroundFollower() {d_isActive = true;}

	void SetIsActive(bool i_isActive) {d_isActive = i_isActive;}
	bool GetIsActive(void) const {return d_isActive;}

	// Macros to make dead reckoning accessible.
	DeclareDRMembers(DSGroundFollower, DRGroundFollowerModel,
			DRGroundFollowerModelPolicy)
	DROperationBegin

  private:
	bool d_isActive;
};

#endif // DS_GROUND_FOLLOWER
