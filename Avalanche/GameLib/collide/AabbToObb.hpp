/*
**
**  File:   AabbToObb.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/becCollide/AabbToObb.hpp $
**  $Revision: 1 $
**      $Date: 3/13/03 9:43a $
**    $Author: Bcollard $
**
*/

#ifdef AABB_TO_OBB_HPP
#undef AABB_TO_OBB_HPP

// System includes //
#include <new.h>

// Engine includes //
#include "Layers/Assert/Assert.h"

// Gamelib includes //
#include "collide/Stack.h"

/***************************************************************************/
/***************************************************************************/
inline CAabbToObbIntersection::CAabbToObbIntersection(
		CAabbCollisionObject &i_aabb, OBB &i_obb) :
	CCollisionIntersection(AABB_TO_OBB),
	d_aabb(&i_aabb),
	d_obb(&i_obb)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CAabbToObbIntersection::CAabbToObbIntersection() :
	CCollisionIntersection(AABB_TO_OBB),
	d_aabb(NULL),
	d_obb(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CAabbToObbIntersection::CAabbToObbIntersection(
		CAabbCollisionObject &i_aabb, OBB &i_obb, CCollisionStack &i_stack) :
	CCollisionIntersection(AABB_TO_OBB, i_stack),
	d_aabb(&i_aabb),
	d_obb(&i_obb)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CAabbToObbIntersection* CAabbToObbIntersection::New(
		CAabbCollisionObject &i_aabb, OBB &i_obb, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CAabbToObbIntersection* intersection =
				(CAabbToObbIntersection*)i_stack->Allocate(
				sizeof(CAabbToObbIntersection));

		if (intersection)
			new(intersection) CAabbToObbIntersection(i_aabb, i_obb, *i_stack);

		return(intersection);
	}

	return(new CAabbToObbIntersection(i_aabb, i_obb));
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CAabbToObbIntersection::TestForIntersection(
		CAabbCollisionObject* i_aabb, OBB* i_obb, CCollisionStack* i_stack)
{
	CAabbToObbIntersection* intersection = CAabbToObbIntersection::New(*i_aabb,
			*i_obb, i_stack);
	if (intersection)
	{
		if (intersection->SeparatingAxisTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline void CAabbToObbIntersection::Set(CAabbCollisionObject* i_aabb,
		OBB* i_obb)
{
	ASSERT((i_aabb != NULL) && (i_obb != NULL));
	d_aabb = i_aabb;
	d_obb = i_obb;
	ClearTests();
}

#endif // AABB_TO_OBB_HPP
