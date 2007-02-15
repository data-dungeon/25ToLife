/*
**
**  File:   LineToTerrain.cpp
**  Date:   February 19, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToTerrain.cpp $
**  $Revision: 12 $
**      $Date: 7/25/03 11:42a $
**    $Author: Bcollard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/LineToTerrain.h"

#if 0

// Gamelib includes //
#include "collide/StackArray.h"

#endif

/***************************************************************************/
/***************************************************************************/
bool CLineToTerrainIntersection::IntersectionTest(void)
{
	switch (d_line.FindMode())
	{
	 case LINE_FIND_ALL:
	 {
		ASSERT(d_pointList == NULL);
		d_iterator.Setup(d_line.ComputePoint(d_line.MinBoundary()),
				d_line.ComputePoint(d_line.MaxBoundary()));
		if (!d_iterator.Initialize(d_terrain) || ((d_pointList =
				CCollisionStackArray<CLineToTerrainPoint>::New(d_stack)) == NULL))
		{
			d_iterator.Terminate();
			ASSERT(!d_stack->IsBottomLocked());
			return(false);
		}

		d_pointList->InitArray();
		CLineToTerrainPoint* point = d_pointList->ExpandArray();
		if (point == NULL)
		{
			d_iterator.Terminate();
			ASSERT(!d_stack->IsBottomLocked());
			return(false);
		}

		while((point->d_triangle = d_iterator.GetNextTriangle()) != NULL)
		{
			if (d_line.IntersectPlane(point->d_triangle->FaceNormal(),
					point->d_triangle->A(), point->d_frac,
					point->d_intersection) && point->d_triangle->PointInTriangle(
					point->d_intersection))
			{
				if ((point = d_pointList->ExpandArray()) == NULL)
				{
					d_iterator.Terminate();
					ASSERT(!d_stack->IsBottomLocked());
					return(d_pointList->NumEntries() > 0);
				}
			}
		}

		d_pointList->ContractArray();
		ASSERT(!d_stack->IsBottomLocked());
		return(d_pointList->NumEntries() > 0);
	 }
	 case LINE_FIND_CLOSEST:
		if (!InitializeFindClosest())
		{
			ASSERT(!d_stack->IsBottomLocked());
			return(false);
		}

		while(GetNextTriangle())
		{
			if (d_line.SetClosestPoint(d_testPoint.d_frac))
				d_foundPoint = d_testPoint;
		}
		break;
	 case LINE_FIND_FURTHEST:
		if (!InitializeFindFurthest())
		{
			ASSERT(!d_stack->IsBottomLocked());
			return(false);
		}

		while(GetNextTriangle())
		{
			if (d_line.SetFurthestPoint(d_testPoint.d_frac))
				d_foundPoint = d_testPoint;
		}
		break;
	 case LINE_FIND_ANY:
		if (!InitializeFindAny())
		{
			ASSERT(!d_stack->IsBottomLocked());
			return(false);
		}

		while(GetNextTriangle())
		{
			d_line.SetAnyPoint(d_testPoint.d_frac);
			d_foundPoint = d_testPoint;
			d_iterator.Terminate();
			break;
		}
		break;
	}

	ASSERT(!d_stack->IsBottomLocked());

	if (d_foundPoint.d_triangle == NULL)
		return(false);

	d_previousModifier = d_line.SetModifier(this);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
void CLineToTerrainIntersection::DrawTerrain(void)
{
	d_iterator.Setup(d_line.ComputePoint(d_line.MinBoundary()),
			d_line.ComputePoint(d_line.MaxBoundary()));

	if (d_iterator.Initialize(d_terrain))
	{
		CTerrainCollisionObject::SDrawState drawState;
		d_terrain.InitializeDraw(d_stack, drawState);
		TerrainTriangle* triangle;
		while((triangle = d_iterator.GetNextTriangle()) != NULL)
		{
			float frac;
			Vector3 intersection;
			if (d_line.IntersectPlane(triangle->FaceNormal(),
					triangle->A(), frac, intersection) &&
					triangle->PointInTriangle(intersection))
			{
				if (!d_terrain.DrawTriangle(triangle, drawState))
				{
					ASSERT(!d_stack->IsBottomLocked());
					return;
				}
			}
		}
		d_terrain.TerminateDraw(drawState);
	}

#if 0
	if (d_iterator.InitializeSector(d_terrain))
	{
		CTerrainSectorInfo sector;
		while(d_iterator.GetNextSector(sector))
			sector.Draw();
	}
#endif
	ASSERT(!d_stack->IsBottomLocked());
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CLineToTerrainIntersection::GetName(void)
{
	return("LineToTerrain");
}
#endif
