/*
**
**  File:   DSPathFollower.h
**  Date:   October 15, 2004
**  By:     Bryce Thomsen
**  Desc:   Implementation of a path follower dataset.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSPathFollower.h $
**  $Revision: #8 $
**  $DateTime: 2005/08/15 17:04:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_PATH_FOLLOWER_H
#define DS_PATH_FOLLOWER_H

#include <netz.h>
#include "Network/NetZ/DSPathFollowerDDL.h"
#include "Network/NetZ/DRModel.h"

class DRPathFollowerModel;

class DSPathFollower : public DATASET(DSPathFollower)
{
  public:
	DSPathFollower() {d_pathFollower = NULL;}

	void SetPathFollowerObject(CPathFollower& i_pathFollower) 
			{ d_pathFollower = &i_pathFollower; }
	CPathFollower* GetPathFollowerObject(void) const { return(d_pathFollower); }
	CPathFollower* GetPathFollowerObject(void) { return (d_pathFollower); }

	void SetIsActive(bool i_isActive) { d_isActive = i_isActive; }
	bool IsActive() const { return (d_isActive); }

	// Macros to make dead reckoning accessible.
	DeclareDRMembers(DSPathFollower, DRPathFollowerModel,
			DRPathFollowerModelPolicy)
	DROperationBegin

  private:
	CPathFollower* d_pathFollower;
	bool d_isActive;
};

#endif // DS_PATH_FOLLOWER
