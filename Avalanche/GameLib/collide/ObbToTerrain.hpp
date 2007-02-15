/*
**
**  File:   ObbToTerrain.hpp
**  Date:   February 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between an OBB and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/ObbToTerrain.hpp $
**  $Revision: 7 $
**      $Date: 6/30/03 3:35p $
**    $Author: Bcollard $
**
*/

#ifdef OBB_TO_TERRAIN_HPP
#undef OBB_TO_TERRAIN_HPP

// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"

/***************************************************************************/
/***************************************************************************/
inline CObbToTerrainIntersection::CObbToTerrainIntersection(
		OBB &i_obb, CTerrainCollisionObject &i_terrain) :
	CCollisionIntersection(OBB_TO_TERRAIN),
	d_obb(i_obb),
	d_terrain(i_terrain),
	d_iterator(NULL)
{
	d_triangleIntersection = NULL;
	d_triangleVertex = NULL;
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CObbToTerrainIntersection::CObbToTerrainIntersection(OBB &i_obb,
		CTerrainCollisionObject &i_terrain, CCollisionStack &i_stack) :
	CCollisionIntersection(OBB_TO_TERRAIN, i_stack),
	d_obb(i_obb),
	d_terrain(i_terrain),
	d_iterator(&i_stack)
{
	d_triangleIntersection = NULL;
	d_triangleVertex = NULL;
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CObbToTerrainIntersection* CObbToTerrainIntersection::New(
		OBB &i_obb, CTerrainCollisionObject &i_terrain, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CObbToTerrainIntersection* intersection =
				(CObbToTerrainIntersection*)i_stack->Allocate(
				sizeof(CObbToTerrainIntersection));

		if (intersection)
			new(intersection) CObbToTerrainIntersection(i_obb, i_terrain,
					*i_stack);

		return(intersection);
	}

	return(new CObbToTerrainIntersection(i_obb, i_terrain));
}

/***************************************************************************/
/***************************************************************************/
inline bool CObbToTerrainIntersection::IntersectionTest(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	return(d_intersects);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionStackArray<Vector3>*
		CObbToTerrainIntersection::GetTriangleIntersections(void)
{
	if ((d_validFlags & TRIANGLE_INTERSECTION) == 0)
		BuildTriangleIntersections();

	return(d_triangleIntersection);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionStackArray<Vector3>*
		CObbToTerrainIntersection::GetTriangleVertices(void)
{
	if ((d_validFlags & TRIANGLE_VERTEX) == 0)
		BuildTriangleVertices();

	return(d_triangleVertex);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CObbToTerrainIntersection::TestForIntersection(
		OBB* i_obb, CTerrainCollisionObject* i_terrain, CCollisionStack* i_stack)
{
	CObbToTerrainIntersection* intersection = CObbToTerrainIntersection::New(
			*i_obb, *i_terrain, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // OBB_TO_TERRAIN_HPP
