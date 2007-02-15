/*
**
**  File:   GroundHeightIF.h
**  Date:   August 18, 2005
**  By:     Bryant Collard
**  Desc:   An interface used to test for ground.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/GroundHeightIF.h $
**  $Revision: #1 $
**  $DateTime: 2005/08/19 15:38:47 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef GROUND_HEIGHT_IF_H
#define GROUND_HEIGHT_IF_H

#include "Interfaces/Interface.h"
#include "platform/BaseType.h"
#include "Math/Vector.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CGroundHeightIF : public CInterface
{
  public:
	// Data definitions
	enum ECheckStatus
	{
		NOT_CHECKED,
		HIT,
		NO_HIT
	};
	struct SCheckResults
	{
		ECheckStatus d_status;
		float d_topHeight;
		float d_bottomHeight;
		Vector3 d_intersection;
		Vector3 d_normal;
		uint16 d_color;
		uint8 d_material;
		ActorHandle d_target;
	};

	static uint GetID(void) {return(CInterface::GetValidID(s_groundHeightID));}
	static CGroundHeightIF* GetInterface(CActor &i_actor);
	static CGroundHeightIF* GetInterface(CActor* i_actor);
	static CGroundHeightIF* GetInterface(ActorHandle i_handle);

	// Set the state to NOT_CHECKED.
	virtual void Clear(void) {}

	// Blindly fill in the results.
	virtual void Set(SCheckResults &i_results) {}

	// Get the check results, testing if necessary. The return value is true if
	// an intersection was found in the given range, false otherwise. If a test
	// is performed, it only checks the world.
	virtual bool Get(float i_topHeight, float i_bottomHeight,
			SCheckResults* &o_results) {return(false);}

	// Get the check results, if any. The return value is true if the check is
	// valid.
	virtual bool Get(SCheckResults* &o_results) {return(false);}

	// Simple status checks.
	virtual ECheckStatus Status(void) {return(NOT_CHECKED);}
	virtual bool Checked(void) {return(false);}
	virtual bool Hit(void) {return(false);}

  private:
	static uint s_groundHeightID;
};

#endif // GROUND_HEIGHT_IF_H
