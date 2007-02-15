/*
**
**  File:   LineToAabb.cpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and AABBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToAabb.cpp $
**  $Revision: 5 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

// My header (always first) //
#include "collide/LineToAabb.h"

// Valid data masks.
const uint CLineToAabbIntersection::INTERSECTION_TEST = 0x01;
const uint CLineToAabbIntersection::ENTRY_POINT       = 0x02;
const uint CLineToAabbIntersection::EXIT_POINT        = 0x04;

/***************************************************************************/
/***************************************************************************/
void CLineToAabbIntersection::PerformIntersectionTest(void)
{
	d_validFlags |= INTERSECTION_TEST;

	if (d_aabb.GetConstAABB().IntersectLine(d_line.Origin(), d_line.Basis(),
			d_entryFace, d_entryFrac, d_exitFace, d_exitFrac) &&
			d_line.IsOn(d_entryFrac, d_exitFrac))
	{
		d_intersects = true;
		d_enters = (d_line.DetectFrontSide() && d_line.IsOn(d_entryFrac));
		d_exits = (d_line.DetectBackSide() && d_line.IsOn(d_exitFrac));

		switch (d_line.FindMode())
		{
		 case LINE_FIND_ALL:
			return;
		 case LINE_FIND_CLOSEST:
			d_previousPoint = d_line.MaxBoundary();
			if (d_enters)
			{
				if (!d_line.SetClosestPoint(d_entryFrac))
					return;
				d_line.ComputePoint(d_entryFrac, d_foundPoint);
				AABB::GetFaceNormal(d_entryFace, d_foundNormal);
			}
			else if (d_exits && d_line.SetClosestPoint(d_exitFrac))
			{
				d_line.ComputePoint(d_exitFrac, d_foundPoint);
				AABB::GetFaceNormal(d_exitFace, d_foundNormal);
			}
			else if (d_line.DetectBothSides() &&
					d_line.SetClosestPoint(d_line.MinBoundary()))
			{
				d_line.ComputePoint(d_line.MinBoundary(), d_foundPoint);
				d_foundNormal = d_line.Basis().Normalized().Negated();
			}
			else
				return;
			d_modifiedLine = true;
			d_previousModifier = d_line.SetModifier(this);
			return;
		 case LINE_FIND_FURTHEST:
			d_previousPoint = d_line.MinBoundary();
			if (d_exits)
			{
				if (!d_line.SetFurthestPoint(d_exitFrac))
					return;
				d_line.ComputePoint(d_exitFrac, d_foundPoint);
				AABB::GetFaceNormal(d_exitFace, d_foundNormal);
			}
			else if (d_enters && d_line.SetFurthestPoint(d_entryFrac))
			{
				d_line.ComputePoint(d_entryFrac, d_foundPoint);
				AABB::GetFaceNormal(d_entryFace, d_foundNormal);
			}
			else if (d_line.DetectBothSides() &&
					d_line.SetFurthestPoint(d_line.MaxBoundary()))
			{
				d_line.ComputePoint(d_line.MaxBoundary(), d_foundPoint);
				d_foundNormal = d_line.Basis().Normalized();
			}
			else
				return;
			d_modifiedLine = true;
			d_previousModifier = d_line.SetModifier(this);
			return;
		 case LINE_FIND_ANY:
			if (d_line.Modifier() == NULL)
			{
				d_modifiedLine = true;
				d_previousModifier = d_line.SetModifier(this);
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
const char* CLineToAabbIntersection::GetName(void)
{
	return("LineToAabb");
}
#endif
