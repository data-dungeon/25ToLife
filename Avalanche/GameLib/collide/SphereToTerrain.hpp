/*
**
**  File:   SphereToTerrain.hpp
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between a sphere and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/SphereToTerrain.hpp $
**  $Revision: 3 $
**      $Date: 6/13/03 12:15p $
**    $Author: Bcollard $
**
*/

#ifdef SPHERE_TO_TERRAIN_HPP
#undef SPHERE_TO_TERRAIN_HPP

// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"

/***************************************************************************/
/***************************************************************************/
inline CSphereToTerrainIntersection::CSphereToTerrainIntersection(
		CSphereCollisionObject &i_sphere, CTerrainCollisionObject &i_terrain) :
	CCollisionIntersection(SPHERE_TO_TERRAIN),
	d_sphere(i_sphere),
	d_terrain(i_terrain),
	d_iterator(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CSphereToTerrainIntersection::CSphereToTerrainIntersection(
		CSphereCollisionObject &i_sphere, CTerrainCollisionObject &i_terrain,
		CCollisionStack &i_stack) :
	CCollisionIntersection(SPHERE_TO_TERRAIN, i_stack),
	d_sphere(i_sphere),
	d_terrain(i_terrain),
	d_iterator(&i_stack)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CSphereToTerrainIntersection* CSphereToTerrainIntersection::New(
		CSphereCollisionObject &i_sphere, CTerrainCollisionObject &i_terrain,
		CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CSphereToTerrainIntersection* intersection =
				(CSphereToTerrainIntersection*)i_stack->Allocate(
				sizeof(CSphereToTerrainIntersection));

		if (intersection)
			new(intersection) CSphereToTerrainIntersection(i_sphere, i_terrain,
					*i_stack);

		return(intersection);
	}

	return(new CSphereToTerrainIntersection(i_sphere, i_terrain));
}

/***************************************************************************/
/***************************************************************************/
inline bool CSphereToTerrainIntersection::IntersectionTest(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	return(d_intersects);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection*
		CSphereToTerrainIntersection::TestForIntersection(
		CSphereCollisionObject* i_sphere, CTerrainCollisionObject* i_terrain,
		CCollisionStack* i_stack)
{
	CSphereToTerrainIntersection* intersection =
			CSphereToTerrainIntersection::New(*i_sphere, *i_terrain, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // SPHERE_TO_TERRAIN_HPP
