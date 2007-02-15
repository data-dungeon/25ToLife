/*
**
**  File:   AabbToTerrain.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between an AABB and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbToTerrain.hpp $
**  $Revision: 6 $
**      $Date: 7/23/03 2:27p $
**    $Author: Bcollard $
**
*/

#ifdef AABB_TO_TERRAIN_HPP
#undef AABB_TO_TERRAIN_HPP

// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"

/***************************************************************************/
/***************************************************************************/
inline CAabbToTerrainIntersection::CAabbToTerrainIntersection(
		CAabbCollisionObject &i_aabb, CTerrainCollisionObject &i_terrain) :
	CCollisionIntersection(AABB_TO_TERRAIN),
	d_aabb(i_aabb),
	d_terrain(i_terrain),
	d_iterator(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CAabbToTerrainIntersection::CAabbToTerrainIntersection(
		CAabbCollisionObject &i_aabb, CTerrainCollisionObject &i_terrain,
		CCollisionStack &i_stack) :
	CCollisionIntersection(AABB_TO_TERRAIN, i_stack),
	d_aabb(i_aabb),
	d_terrain(i_terrain),
	d_iterator(&i_stack)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CAabbToTerrainIntersection* CAabbToTerrainIntersection::New(
		CAabbCollisionObject &i_aabb, CTerrainCollisionObject &i_terrain,
		CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CAabbToTerrainIntersection* intersection =
				(CAabbToTerrainIntersection*)i_stack->Allocate(
				sizeof(CAabbToTerrainIntersection));

		if (intersection)
			new(intersection) CAabbToTerrainIntersection(i_aabb, i_terrain,
					*i_stack);

		return(intersection);
	}

	return(new CAabbToTerrainIntersection(i_aabb, i_terrain));
}

/***************************************************************************/
/***************************************************************************/
inline bool CAabbToTerrainIntersection::IntersectionTest(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	return(d_intersects);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CAabbToTerrainIntersection::TestForIntersection(
		CAabbCollisionObject* i_aabb, CTerrainCollisionObject* i_terrain,
		CCollisionStack* i_stack)
{
	CAabbToTerrainIntersection* intersection = CAabbToTerrainIntersection::New(
			*i_aabb, *i_terrain, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // AABB_TO_TERRAIN_HPP
