/*
**
**  File:   AabbToAabb.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between AABBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbToAabb.hpp $
**  $Revision: 3 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef AABB_TO_AABB_HPP
#undef AABB_TO_AABB_HPP

// System includes //
#include <new.h>

// Engine includes //
#include "Layers/Assert/Assert.h"

// Gamelib includes //
#include "collide/Stack.h"

/***************************************************************************/
/***************************************************************************/
inline CAabbToAabbIntersection::CAabbToAabbIntersection(
		CAabbCollisionObject &i_aabbA, CAabbCollisionObject &i_aabbB) :
	CCollisionIntersection(AABB_TO_AABB),
	d_aabbA(&i_aabbA),
	d_aabbB(&i_aabbB)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CAabbToAabbIntersection::CAabbToAabbIntersection() :
	CCollisionIntersection(AABB_TO_AABB),
	d_aabbA(NULL),
	d_aabbB(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CAabbToAabbIntersection::CAabbToAabbIntersection(
		CAabbCollisionObject &i_aabbA, CAabbCollisionObject &i_aabbB,
		CCollisionStack &i_stack) :
	CCollisionIntersection(AABB_TO_AABB, i_stack),
	d_aabbA(&i_aabbA),
	d_aabbB(&i_aabbB)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CAabbToAabbIntersection* CAabbToAabbIntersection::New(
		CAabbCollisionObject &i_aabbA, CAabbCollisionObject &i_aabbB,
		CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CAabbToAabbIntersection* intersection =
				(CAabbToAabbIntersection*)i_stack->Allocate(
				sizeof(CAabbToAabbIntersection));

		if (intersection)
			new(intersection) CAabbToAabbIntersection(i_aabbA, i_aabbB, *i_stack);

		return(intersection);
	}

	return(new CAabbToAabbIntersection(i_aabbA, i_aabbB));
}

/***************************************************************************/
/***************************************************************************/
inline void CAabbToAabbIntersection::SetAabbs(CAabbCollisionObject* i_aabbA,
		CAabbCollisionObject* i_aabbB)
{
	ASSERT((i_aabbA != NULL) && (i_aabbB != NULL));
	d_aabbA = i_aabbA;
	d_aabbB = i_aabbB;
	ClearTests();
}

/***************************************************************************/
// If the collision isn't trivally rejected, do the separating
// axis test.  This is reasonably fast if they objects don't
// collide.  If this test passes the boxes are touching.
/***************************************************************************/
inline bool CAabbToAabbIntersection::SeparatingAxisTest(void)
{
	ASSERT((d_aabbA != NULL) && (d_aabbB != NULL));

	if ((d_validFlags & SEPARATING_AXIS_TEST) != 0)
		return(d_noSeparatingPlane);
	d_validFlags |= SEPARATING_AXIS_TEST;
	d_noSeparatingPlane = false;

	d_noSeparatingPlane = (
			(d_aabbA->GetConstAABB().minV.x() <= d_aabbB->GetConstAABB().maxV.x()) &&
			(d_aabbA->GetConstAABB().maxV.x() >= d_aabbB->GetConstAABB().minV.x()) &&
			(d_aabbA->GetConstAABB().minV.y() <= d_aabbB->GetConstAABB().maxV.y()) &&
			(d_aabbA->GetConstAABB().maxV.y() >= d_aabbB->GetConstAABB().minV.y()) &&
			(d_aabbA->GetConstAABB().minV.z() <= d_aabbB->GetConstAABB().maxV.z()) &&
			(d_aabbA->GetConstAABB().maxV.z() >= d_aabbB->GetConstAABB().minV.z()));

	return(d_noSeparatingPlane);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CAabbToAabbIntersection::TestForIntersection(
		CAabbCollisionObject* i_aabbA, CAabbCollisionObject* i_aabbB,
		CCollisionStack* i_stack)
{
	CAabbToAabbIntersection* intersection = CAabbToAabbIntersection::New(
			*i_aabbA, *i_aabbB, i_stack);
	if (intersection)
	{
		if (intersection->SeparatingAxisTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // AABB_TO_AABB_HPP
