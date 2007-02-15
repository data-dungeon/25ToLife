/*
**
**  File:   LineToTriangleList.cpp
**  Date:   August 3, 2004
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and triangle lists.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/LineToTriangleList.cpp $
**  $Revision: #2 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

// Pre-compiled header
#include "collide/CollidePCH.h"

// My header (always first)
#include "collide/LineToTriangleList.h"

/******************************************************************************/
/******************************************************************************/
bool CLineToTriangleListIntersection::IntersectionTest(void)
{
	switch (d_line.FindMode())
	{
	 case LINE_FIND_ALL:
	 {
		ASSERT(d_pointList == NULL);
		d_pointList = CCollisionStackArray<CLineToTriangleListPoint>::New(d_stack);
		if (d_pointList == NULL)
			return(false);

		d_pointList->InitArray();
		CLineToTriangleListPoint* point = d_pointList->ExpandArray();
		if (point == NULL)
			return(false);

		int numTriangles = d_triangleList.NumTriangles();
		for (int i = 0; i < numTriangles; i++)
		{
			point->d_triangle = d_triangleList.UnsafeGetTriangle(i);
			if (d_line.IntersectPlane(point->d_triangle->FaceNormal(),
					point->d_triangle->A(), point->d_frac,
					point->d_intersection) && point->d_triangle->PointInTriangle(
					point->d_intersection))
			{
				if ((point = d_pointList->ExpandArray()) == NULL)
					return(d_pointList->NumEntries() > 0);
			}
		}

		d_pointList->ContractArray();
		return(d_pointList->NumEntries() > 0);
	 }
	 case LINE_FIND_CLOSEST:
	 {
		ASSERT(d_foundPoint.d_triangle == NULL);
		d_previousPoint = d_line.MaxBoundary();
		CLineToTriangleListPoint testPoint;
		int numTriangles = d_triangleList.NumTriangles();
		for(int i = 0; i < numTriangles; i++)
		{
			testPoint.d_triangle = d_triangleList.UnsafeGetTriangle(i);
			if (d_line.IntersectPlane(testPoint.d_triangle->FaceNormal(),
					testPoint.d_triangle->A(), testPoint.d_frac,
					testPoint.d_intersection) &&
					testPoint.d_triangle->PointInTriangle(testPoint.d_intersection)
					&& d_line.SetClosestPoint(testPoint.d_frac))
				d_foundPoint = testPoint;
		}
		break;
	 }
	 case LINE_FIND_FURTHEST:
	 {
		ASSERT(d_foundPoint.d_triangle == NULL);
		d_previousPoint = d_line.MinBoundary();
		CLineToTriangleListPoint testPoint;
		int numTriangles = d_triangleList.NumTriangles();
		for(int i = 0; i < numTriangles; i++)
		{
			testPoint.d_triangle = d_triangleList.UnsafeGetTriangle(i);
			if (d_line.IntersectPlane(testPoint.d_triangle->FaceNormal(),
					testPoint.d_triangle->A(), testPoint.d_frac,
					testPoint.d_intersection) &&
					testPoint.d_triangle->PointInTriangle(testPoint.d_intersection)
					&& d_line.SetFurthestPoint(testPoint.d_frac))
				d_foundPoint = testPoint;
		}
		break;
	 }
	 case LINE_FIND_ANY:
		ASSERT(d_foundPoint.d_triangle == NULL);
		if (d_line.Modifier() == NULL)
		{
			CLineToTriangleListPoint testPoint;
			int numTriangles = d_triangleList.NumTriangles();
			for(int i = 0; i < numTriangles; i++)
			{
				testPoint.d_triangle = d_triangleList.UnsafeGetTriangle(i);
				if (d_line.IntersectPlane(testPoint.d_triangle->FaceNormal(),
						testPoint.d_triangle->A(), testPoint.d_frac,
						testPoint.d_intersection) &&
						testPoint.d_triangle->PointInTriangle(
						testPoint.d_intersection))
				{
					d_line.SetAnyPoint(testPoint.d_frac);
					d_foundPoint = testPoint;
					break;
				}
			}
		}
		break;
	}

	if (d_foundPoint.d_triangle == NULL)
		return(false);

	d_previousModifier = d_line.SetModifier(this);
	return(true);
}

#ifdef DEBUG_STACK
/******************************************************************************/
/******************************************************************************/
const char* CLineToTriangleListIntersection::GetName(void)
{
	return("LineToTriangleList");
}
#endif
