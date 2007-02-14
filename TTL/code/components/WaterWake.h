/*
**
**  File:   WaterWake.h
**  Date:   June 8, 2005
**  By:     Bryant Collard
**  Desc:   Generate wakes when dragging through water.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/WaterWake.h $
**  $Revision: #1 $
**  $DateTime: 2005/06/08 21:24:46 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef WATER_WAKE_H
#define WATER_WAKE_H

#include "Components/Component.h"

class CWaterWake : public CActorComponent
{
  public:
	CWaterWake(CCompActor &i_actor);
	~CWaterWake();

	void Create(const char* i_name);
	void Destroy(void);

	// Override CActorComponent
	virtual const char *ComponentName(void) {return("WaterWake");}
	virtual void BeginUpdate(void);
	virtual void EndUpdate(float i_deltaSec);
	virtual void CollisionAccept(CStageEntity* i_entity,
			CCollisionEvent* i_event);

  private:
	enum ELocation
	{
		LOCATE_LEFT,
		LOCATE_CENTER,
		LOCATE_RIGHT,
		NUM_LOCATIONS
	};

	void Activate(int i_index);
	void Deactivate(int i_index);

	t_Handle d_emitter[NUM_LOCATIONS];
	Vector3 d_pos[NUM_LOCATIONS];
	int d_count[NUM_LOCATIONS];
	bool d_active[NUM_LOCATIONS];
};

#endif // WATER_WAKE_H
