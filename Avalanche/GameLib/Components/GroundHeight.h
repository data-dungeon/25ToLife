/*
**
**  File:   GroundHeight.h
**  Date:   August 18, 2005
**  By:     Bryant Collard
**  Desc:   Computes and stores ground height info.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Components/GroundHeight.h $
**  $Revision: #1 $
**  $DateTime: 2005/08/19 15:38:47 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef GROUND_HEIGHT_H
#define GROUND_HEIGHT_H

#include "Components/Component.h"
#include "Interfaces/GroundHeightIF.h"

class CGroundHeight : public CActorComponent, public CGroundHeightIF
{
  public:
	// Construction
	CGroundHeight(CCompActor &i_actor);
	virtual const char* ComponentName(void) {return("GroundHeight");}

	// Get at interfaces
	virtual CInterface* GetInterface(uint i_id);

	// Interface
	virtual void Clear(void);
	virtual void Set(SCheckResults &i_results);
	virtual bool Get(float i_topHeight, float i_bottomHeight,
			SCheckResults* &o_results);
	virtual bool Get(SCheckResults* &o_results);
	virtual ECheckStatus Status(void) {return(d_results.d_status);}
	virtual bool Checked(void) {return(d_results.d_status != NOT_CHECKED);}
	virtual bool Hit(void) {return(d_results.d_status == HIT);}

  private:
	SCheckResults d_results;
	Vector3 d_lastPosition;
};

#endif // GROUND_HEIGHT_H
