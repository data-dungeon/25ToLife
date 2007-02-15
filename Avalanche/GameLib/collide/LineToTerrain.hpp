/*
**
**  File:   LineToTerrain.hpp
**  Date:   February 19, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibnew/collide/LineToTerrain.hpp $
**  $Revision: 11 $
**      $Date: 8/11/03 11:49a $
**    $Author: Food $
**
*/

#ifdef LINE_TO_TERRAIN_HPP
#undef LINE_TO_TERRAIN_HPP

// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"
#include "collide/StackArray.h"

/***************************************************************************/
/***************************************************************************/
inline CLineToTerrainIntersection::CLineToTerrainIntersection(
		CLineCollisionObject &i_line, CTerrainCollisionObject &i_terrain) :
	CCollisionIntersection(LINE_TO_TERRAIN),
	d_line(i_line),
	d_terrain(i_terrain),
	d_iterator(NULL)
{
	d_foundPoint.d_triangle = NULL;
	d_pointList = NULL;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToTerrainIntersection::CLineToTerrainIntersection(
		CLineCollisionObject &i_line, CTerrainCollisionObject &i_terrain,
		CCollisionStack &i_stack) :
	CCollisionIntersection(LINE_TO_TERRAIN, i_stack),
	d_line(i_line),
	d_terrain(i_terrain),
	d_iterator(&i_stack)
{
	d_foundPoint.d_triangle = NULL;
	d_pointList = NULL;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToTerrainIntersection::~CLineToTerrainIntersection()
{
	if (d_foundPoint.d_triangle != NULL)
		d_line.RestoreFindState(this, d_previousModifier, d_previousPoint);
	CCollisionStackItem::Delete(d_pointList);
}

/***************************************************************************/
/***************************************************************************/
inline CLineToTerrainIntersection* CLineToTerrainIntersection::New(
		CLineCollisionObject &i_line, CTerrainCollisionObject &i_terrain,
		CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CLineToTerrainIntersection* intersection =
				(CLineToTerrainIntersection*)i_stack->Allocate(
				sizeof(CLineToTerrainIntersection));

		if (intersection)
		{
			new(intersection) CLineToTerrainIntersection(i_line, i_terrain,
					*i_stack);
		}

		return(intersection);
	}
	return(new CLineToTerrainIntersection(i_line, i_terrain));
}

/***************************************************************************/
/***************************************************************************/
inline CLineToTerrainPoint* CLineToTerrainIntersection::GetIntersection(
		int i_index)
{
	if ((d_pointList == NULL) || (i_index < 0) ||
			(i_index >= d_pointList->NumEntries()))
		return(NULL);

	return(d_pointList->EntryPtr(i_index));
}

/***************************************************************************/
/***************************************************************************/
inline CLineToTerrainPoint* CLineToTerrainIntersection::GetFoundPoint(void)
{
	if (d_foundPoint.d_triangle == NULL)
		return(NULL);

	return(&d_foundPoint);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CLineToTerrainIntersection::TestForIntersection(
		CLineCollisionObject* i_line, CTerrainCollisionObject* i_terrain,
		CCollisionStack* i_stack)
{
	CLineToTerrainIntersection* intersection =
			(CLineToTerrainIntersection*)CLineToTerrainIntersection::New(
			*i_line, *i_terrain, i_stack);
	if (intersection)
	{
		bool hit = intersection->IntersectionTest();
		if (hit)
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToTerrainIntersection::InitializeFindClosest(void)
{
	ASSERT(d_foundPoint.d_triangle == NULL);
	d_iterator.Setup(d_line.ComputePoint(d_line.MinBoundary()),
			d_line.ComputePoint(d_line.MaxBoundary()));
	if (d_iterator.Initialize(d_terrain))
	{
		d_previousPoint = d_line.MaxBoundary();
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToTerrainIntersection::InitializeFindFurthest(void)
{
	ASSERT(d_foundPoint.d_triangle == NULL);
	d_iterator.Setup(d_line.ComputePoint(d_line.MaxBoundary()),
			d_line.ComputePoint(d_line.MinBoundary()));
	if (d_iterator.Initialize(d_terrain))
	{
		d_previousPoint = d_line.MinBoundary();
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToTerrainIntersection::InitializeFindAny(void)
{
	ASSERT(d_foundPoint.d_triangle == NULL);
	if (d_line.Modifier() == NULL)
	{
		d_iterator.Setup(d_line.ComputePoint(d_line.MinBoundary()),
				d_line.ComputePoint(d_line.MaxBoundary()));
		if (d_iterator.Initialize(d_terrain))
			return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToTerrainIntersection::GetNextTriangle(void)
{
	while((d_testPoint.d_triangle = d_iterator.GetNextTriangle()) != NULL)
	{
		if (d_line.IntersectPlane(d_testPoint.d_triangle->FaceNormal(),
				d_testPoint.d_triangle->A(), d_testPoint.d_frac,
				d_testPoint.d_intersection) &&
				d_testPoint.d_triangle->PointInTriangle(d_testPoint.d_intersection))
		{
			d_iterator.FoundEnd(d_testPoint.d_intersection);
			return(true);
		}
	}
	return(false);
}

#endif // LINE_TO_TERRAIN_HPP
