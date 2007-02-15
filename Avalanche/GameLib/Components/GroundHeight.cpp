/*
**
**  File:   GroundHeight.cpp
**  Date:   August 18, 2005
**  By:     Bryant Collard
**  Desc:   Computes and stores ground height info.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Components/GroundHeight.cpp $
**  $Revision: #1 $
**  $DateTime: 2005/08/19 15:38:47 $
**    $Author: Bryant_Collard $
**
*/

#include "Components/ComponentsPCH.h"
#include "Components/GroundHeight.h"
#include "Components/ComponentCreator.h"
#include "CollideHelper/LineEntity.h"
#include "CollideHelper/LineQuery.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CGroundHeight, GroundHeight, COMPONENT_CONTROLLER_PRIORITY);

#define INVALIDATE_THRESHOLD_SQ 0.0025f

/******************************************************************************/
/******************************************************************************/
CGroundHeight::CGroundHeight(CCompActor &i_actor) : CActorComponent(i_actor)
{
	Clear();
}

/******************************************************************************/
/******************************************************************************/
CInterface* CGroundHeight::GetInterface(uint i_id)
{
	if (i_id == CGroundHeightIF::GetID())
		return((CInterface*)this);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CGroundHeight::Clear(void)
{
	d_results.d_status = NOT_CHECKED;
}

/******************************************************************************/
/******************************************************************************/
void CGroundHeight::Set(SCheckResults &i_results)
{
	d_results = i_results;
	if (d_results.d_status != NOT_CHECKED)
		d_lastPosition = actor.GetBodyInWorld();
}

/******************************************************************************/
/******************************************************************************/
bool CGroundHeight::Get(float i_topHeight, float i_bottomHeight,
		SCheckResults* &o_results)
{
	if (d_results.d_status == NOT_CHECKED)
	{
		d_results.d_topHeight = i_topHeight;
		d_results.d_bottomHeight = i_bottomHeight;
	}
	else
	{
		bool expand = false;
		if (i_topHeight > d_results.d_topHeight)
		{
			d_results.d_topHeight = i_topHeight;
			expand = true;
		}
		if ((d_results.d_status == NO_HIT) &&
				(i_bottomHeight < d_results.d_bottomHeight))
		{
			d_results.d_bottomHeight = i_bottomHeight;
			expand = true;
		}
		if ((expand && (d_results.d_status == NO_HIT)) ||
				((actor.GetBodyInWorld() - d_lastPosition).LengthSquared() >
				INVALIDATE_THRESHOLD_SQ))
			d_results.d_status = NOT_CHECKED;
	}

	if (d_results.d_status == NOT_CHECKED)
	{
		CLineEntity line;
		d_lastPosition = actor.GetBodyInWorld();
		line.Line().SetOriginBasis(d_lastPosition, Vector3(0.0f, -1.0f, 0.0f));
		line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
		line.Line().SetBoundary(CLineCollisionObject::BOUNDARY_MIN_MAX,
				-d_results.d_topHeight, -d_results.d_bottomHeight);
		line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		line.SetPositiveInclude(WORLD_CATEGORY);

		CLineQuery query;
		g_collisionSystem.ResetStack();
		if (query.Test(line))
		{
			d_results.d_status = HIT;
			d_results.d_bottomHeight = -query.d;
			d_results.d_intersection = query.intersection;
			d_results.d_normal = query.normal;
			d_results.d_color = query.color;
			d_results.d_material = query.material;
		}
		else
		{
			d_results.d_status = NO_HIT;
			d_results.d_color = 0xffff;
			d_results.d_material = INVALID_MATERIALINDEX;
		}
		d_results.d_target.MakeInvalid();
	}

	o_results = &d_results;
	return((d_results.d_status == HIT) &&
			(d_results.d_bottomHeight <= i_topHeight) &&
			(d_results.d_bottomHeight >= i_bottomHeight));
}

/******************************************************************************/
/******************************************************************************/
bool CGroundHeight::Get(SCheckResults* &o_results)
{
	o_results = &d_results;
	return(d_results.d_status != NOT_CHECKED);
}
