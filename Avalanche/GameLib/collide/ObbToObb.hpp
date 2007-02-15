/*
**
**  File:   ObbToObb.hpp
**  Date:   February 4, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/ObbToObb.hpp $
**  $Revision: 9 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef OBB_TO_OBB_HPP
#undef OBB_TO_OBB_HPP

// System includes //
#include <new.h>

// Engine includes //
#include "Layers/Memory.h"
#include "Layers/Assert/Assert.h"

// Gamelib includes //
#include "collide/Stack.h"

/***************************************************************************/
/***************************************************************************/
inline CObbToObbIntersection::CObbToObbIntersection(OBB &i_obbA, OBB &i_obbB) :
	CCollisionIntersection(OBB_TO_OBB),
	d_obbA(&i_obbA),
	d_obbB(&i_obbB)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CObbToObbIntersection::CObbToObbIntersection() :
	CCollisionIntersection(OBB_TO_OBB),
	d_obbA(NULL),
	d_obbB(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CObbToObbIntersection::CObbToObbIntersection(OBB &i_obbA, OBB &i_obbB,
		CCollisionStack &i_stack) :
	CCollisionIntersection(OBB_TO_OBB, i_stack),
	d_obbA(&i_obbA),
	d_obbB(&i_obbB)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CObbToObbIntersection* CObbToObbIntersection::New(OBB &i_obbA,
		OBB &i_obbB, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CObbToObbIntersection* intersection =
				(CObbToObbIntersection*)i_stack->Allocate(
				sizeof(CObbToObbIntersection));

		if (intersection)
			new(intersection) CObbToObbIntersection(i_obbA, i_obbB, *i_stack);

		return(intersection);
	}

	return(new CObbToObbIntersection(i_obbA, i_obbB));
}

/***************************************************************************/
/***************************************************************************/
inline CObbToObbIntersection& CObbToObbIntersection::operator=(
		CObbToObbIntersection &i_source)
{
	d_obbA = i_source.d_obbA;
	d_obbB = i_source.d_obbB;
	d_validFlags = i_source.d_validFlags;

	if ((d_validFlags & RADIUS_TEST) != 0)
		d_spheresIntersect = i_source.d_spheresIntersect;

	if ((d_validFlags & SEPARATING_AXIS_TEST) != 0)
		d_noSeparatingPlane = i_source.d_noSeparatingPlane;

	if ((d_validFlags & CLOSEST_SEPARATING_AXIS) != 0)
	{
		memcpy(d_axisOverlap, i_source.d_axisOverlap,
				NUM_SEPARATING_AXIS * sizeof(float));
		d_closestSeparatingAxis = i_source.d_closestSeparatingAxis;
	}

	if ((d_validFlags & TRANSFORM) != 0)
		d_aToB = i_source.d_aToB;

	if ((d_validFlags & INTERSECTION_POINTS) != 0)
	{
		for (int i = 0; i < d_numPoints; i++)
		{
			d_faceID[i] = i_source.d_faceID[i];
			d_point[i] = i_source.d_point[i];
		}
	}

	if ((d_validFlags & INTERSECTION_NORMAL_SUM) != 0)
		d_normalSum = i_source.d_normalSum;

	if ((d_validFlags & INTERSECTION_PLANE) != 0)
	{
		d_planeResult = i_source.d_planeResult;
		d_planeLocation = i_source.d_planeLocation;
		d_planeNormal = i_source.d_planeNormal;
	}

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline void CObbToObbIntersection::SetObbs(OBB* i_obbA, OBB* i_obbB)
{
	ASSERT((i_obbA != NULL) && (i_obbB != NULL));
	d_obbA = i_obbA;
	d_obbB = i_obbB;
	ClearTests();
}

/***************************************************************************/
// Return true if the collision can occur.
// This is very fast and very inaccurate
/***************************************************************************/
inline bool CObbToObbIntersection::RadiusTest(void)
{
	ASSERT((d_obbA != NULL) && (d_obbB != NULL));
	if ((d_validFlags & RADIUS_TEST) == 0)
	{
		Vector3 d = *(d_obbA->Center()) - *(d_obbB->Center());
		float radiusSum = d_obbA->Radius() + d_obbB->Radius();
		d_spheresIntersect = (d.LengthSquared() < (radiusSum * radiusSum));
		d_validFlags |= RADIUS_TEST;
	}
	return(d_spheresIntersect);
}

/***************************************************************************/
/***************************************************************************/
inline void CObbToObbIntersection::CompareSeparatingAxes(int i_testAxis,
		float i_axisComp1, float i_axisComp2, int &io_closestAxis,
		float &io_closestLengthSq, float &io_closestOverlapSq) const
{
	float lengthSq = i_axisComp1 * i_axisComp1 + i_axisComp2 * i_axisComp2;
	float overlapSq = d_axisOverlap[i_testAxis] * d_axisOverlap[i_testAxis];
	if ((overlapSq * io_closestLengthSq) < (io_closestOverlapSq * lengthSq))
	{
		io_closestAxis = i_testAxis;
		io_closestLengthSq = lengthSq;
		io_closestOverlapSq = overlapSq;
	}
}

/***************************************************************************/
// Get the transform from OBB A to OBB B.
/***************************************************************************/
inline Cartesian4x4& CObbToObbIntersection::ObbAToObbB(void)
{
	ASSERT((d_obbA != NULL) && (d_obbB != NULL));
	if ((d_validFlags & TRANSFORM) == 0)
	{
		d_aToB = (*(d_obbA->Matrix())) / (*(d_obbB->Matrix()));
		d_validFlags |= TRANSFORM;
	}
	return(d_aToB);
}

/***************************************************************************/
/***************************************************************************/
inline void CObbToObbIntersection::PerformEdgeIntersections(void)
{
	ASSERT((d_obbA != NULL) && (d_obbB != NULL));
/*
 *	Reset the list.
 */
	d_numPoints = 0;
/*
 *	Make sure the transform from OBB A to OBB B has been computed.
 */
	ObbAToObbB();
/*
 *	Intersect edges of OBB A with faces of OBB B.
 */
	FindEdgeObbIntersections(d_aToB, d_obbA->GetConstAABB().maxV, *d_obbB, 0x8);
/*
 *	Intersect edges of OBB B with faces of OBB A.
 */
	Cartesian4x4 bToA;
	d_aToB.Invert(bToA);
	FindEdgeObbIntersections(bToA, d_obbB->GetConstAABB().maxV, *d_obbA, 0x0);
/*
 *	Mark this as done.
 */
	d_validFlags |= INTERSECTION_POINTS;
}

/***************************************************************************/
/***************************************************************************/
inline int CObbToObbIntersection::EdgeIntersections(void)
{
/*
 *	See if this has already been done.
 */
	if ((d_validFlags & INTERSECTION_POINTS) == 0)
		PerformEdgeIntersections();
	return(d_numPoints);
}

/***************************************************************************/
/***************************************************************************/
inline const Vector3* CObbToObbIntersection::EdgeIntersection(int i_index)
{
	if (((d_validFlags & INTERSECTION_POINTS) == 0) || (i_index < 0) ||
			(i_index >= d_numPoints))
		return(NULL);
	return(&d_point[i_index]);
}

/***************************************************************************/
/***************************************************************************/
inline const Vector3* CObbToObbIntersection::EdgeIntersection(int i_index,
		uint16 i_selectedFaceMask)
{
	if (((d_validFlags & INTERSECTION_POINTS) != 0) && (i_index >= 0) &&
			(i_index < d_numPoints))
	{
		if (((1 << d_faceID[i_index]) & i_selectedFaceMask) != 0)
			return(&d_point[i_index]);
		uint16 edgeField = EDGE_MAP[d_edgeID[i_index]];
		if ((d_faceID[i_index] & 0x8) == 0)
			edgeField <<= 8;
		if ((edgeField & i_selectedFaceMask) != 0)
			return(&d_point[i_index]);
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CObbToObbIntersection::EdgeIntersectionNormalSum(void)
{
	if ((d_validFlags & INTERSECTION_NORMAL_SUM) == 0)
	{
		if ((d_validFlags & INTERSECTION_POINTS) == 0)
			PerformEdgeIntersections();
		PerformEdgeIntersectionNormalSum();
	}
	return(d_normalSum);
}

/***************************************************************************/
/***************************************************************************/
inline int CObbToObbIntersection::AddEdgeIntersections(CPlaneFit &io_planeFit)
{
	if ((d_validFlags & INTERSECTION_POINTS) == 0)
		PerformEdgeIntersections();
	for (int i = 0; i < d_numPoints; i++)
		io_planeFit.AddTransformPoint(d_point[i]);
	return(d_numPoints);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CObbToObbIntersection::TestForIntersection(
		OBB* i_obbA, OBB* i_obbB, CCollisionStack* i_stack)
{
	CObbToObbIntersection* intersection = CObbToObbIntersection::New(*i_obbA,
			*i_obbB, i_stack);
	if (intersection)
	{
		if (intersection->RadiusTest() && intersection->SeparatingAxisTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // OBB_TO_OBB_HPP
