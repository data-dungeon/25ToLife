/*
**
**  File:   LineToSphere.cpp
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and spheres.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToSphere.cpp $
**  $Revision: 2 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

// My header (always first) //
#include "collide/LineToSphere.h"

// Valid data masks.
const uint CLineToSphereIntersection::INTERSECTION_TEST = 0x01;
const uint CLineToSphereIntersection::ENTRY_POINT       = 0x02;
const uint CLineToSphereIntersection::ENTRY_NORMAL      = 0x04;
const uint CLineToSphereIntersection::EXIT_POINT        = 0x08;
const uint CLineToSphereIntersection::EXIT_NORMAL       = 0x10;

/***************************************************************************/
/***************************************************************************/
void CLineToSphereIntersection::PerformIntersectionTest(void)
{
	ASSERT((d_line != NULL) && (d_sphere != NULL));
	d_validFlags |= INTERSECTION_TEST;

	Vector3 start = d_line->Origin() - d_sphere->Center();
	float basisSq = d_line->Basis().LengthSquared();
	float startDotBasis = start.Dot(d_line->Basis());
	float radiusTerm = startDotBasis * startDotBasis + d_sphere->Radius() *
			d_sphere->Radius() - basisSq * start.Dot(start);
	if (radiusTerm < 0.0f)
	{
		d_intersects = false;
		d_enters = false;
		d_exits = false;
		return;
	}

	radiusTerm = Math::Sqrt(radiusTerm);
	d_entryFrac = radiusTerm - startDotBasis;
	d_exitFrac = -radiusTerm - startDotBasis;

	if (Math::DivByZero(d_entryFrac, basisSq))
	{
		d_intersects = false;
		d_enters = false;
		d_exits = false;
		return;
	}
	d_entryFrac /= basisSq;

	if (Math::DivByZero(d_exitFrac, basisSq))
	{
		d_intersects = false;
		d_enters = false;
		d_exits = false;
		return;
	}
	d_exitFrac /= basisSq;
	
	if (d_line->IsOn(d_entryFrac, d_exitFrac))
	{
		d_intersects = true;
		d_enters = (d_line->DetectFrontSide() && d_line->IsOn(d_entryFrac));
		d_exits = (d_line->DetectBackSide() && d_line->IsOn(d_exitFrac));

		switch (d_line->FindMode())
		{
		 case LINE_FIND_ALL:
			return;
		 case LINE_FIND_CLOSEST:
			d_previousPoint = d_line->MaxBoundary();
			if (d_enters)
			{
				if (!d_line->SetClosestPoint(d_entryFrac))
					return;
				d_line->ComputePoint(d_entryFrac, d_foundPoint);
			}
			else if (d_exits && d_line->SetClosestPoint(d_exitFrac))
			{
				d_line->ComputePoint(d_exitFrac, d_foundPoint);
			}
			else if (d_line->DetectBothSides() &&
					d_line->SetClosestPoint(d_line->MinBoundary()))
			{
				d_line->ComputePoint(d_line->MinBoundary(), d_foundPoint);
			}
			else
				return;
			d_foundNormal = (d_foundPoint - d_sphere->Center()) /
					d_sphere->Radius();
			d_modifiedLine = true;
			d_previousModifier = d_line->SetModifier(this);
			return;
		 case LINE_FIND_FURTHEST:
			d_previousPoint = d_line->MinBoundary();
			if (d_exits)
			{
				if (!d_line->SetFurthestPoint(d_exitFrac))
					return;
				d_line->ComputePoint(d_exitFrac, d_foundPoint);
			}
			else if (d_enters && d_line->SetFurthestPoint(d_entryFrac))
			{
				d_line->ComputePoint(d_entryFrac, d_foundPoint);
			}
			else if (d_line->DetectBothSides() &&
					d_line->SetFurthestPoint(d_line->MaxBoundary()))
			{
				d_line->ComputePoint(d_line->MaxBoundary(), d_foundPoint);
			}
			else
				return;
			d_foundNormal = (d_foundPoint - d_sphere->Center()) /
					d_sphere->Radius();
			d_modifiedLine = true;
			d_previousModifier = d_line->SetModifier(this);
			return;
		 case LINE_FIND_ANY:
			if (d_line->Modifier() == NULL)
			{
				d_modifiedLine = true;
				d_previousModifier = d_line->SetModifier(this);
			}
			return;
		}
	}

	d_intersects = false;
	d_enters = false;
	d_exits = false;
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CLineToSphereIntersection::GetName(void)
{
	return("LineToSphere");
}
#endif
