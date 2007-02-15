/*
**
**  File:   AabbToTerrain.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between an AABB and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbToTerrain.h $
**  $Revision: 13 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef AABB_TO_TERRAIN_H
#define AABB_TO_TERRAIN_H

#include "Math/Vector.h"
#include "collide/Intersection.h"
#include "collide/AabbObject.h"
#include "collide/Terrain.h"
#include "collide/TriangleList.h"

class CCollisionStack;
class TerrainTriangle;

class CAabbToTerrainIntersection : public CCollisionIntersection
{
  public:
	CAabbToTerrainIntersection(CAabbCollisionObject &i_aabb,
			CTerrainCollisionObject &i_terrain);
	~CAabbToTerrainIntersection();

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CAabbToTerrainIntersection* New(CAabbCollisionObject &i_aabb,
			CTerrainCollisionObject &i_terrain, CCollisionStack* i_stack);

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if the AABB changes
	// state.
	void ClearTests(void) {d_validFlags = 0;}

	// See if the AABB touches the terrain at all.
	bool IntersectionTest(void);

	// Get the triangle list
	CTriangleListCollisionObject &GetTriangleList(void) {return(d_triangleList);}

	// Draw the terrain touched by the intersection.
	void DrawTerrain(void);

	// Determine if an AABB and terrain intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(
			CAabbCollisionObject* i_aabb, CTerrainCollisionObject* i_terrain,
			CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
	virtual void AppendDescription(char* io_msg);
#endif

  private:
	// Prevent construction without an AABB and terrain.
	CAabbToTerrainIntersection();

	// Control construction on the stack.
	CAabbToTerrainIntersection(CAabbCollisionObject &i_aabb,
			CTerrainCollisionObject &i_terrain, CCollisionStack &i_stack);

	void PerformIntersectionTest(void);

	// Participants.
	CAabbCollisionObject &d_aabb;
	CTerrainCollisionObject &d_terrain;

	// Valid data flags.
	uint d_validFlags;

	// Intersection test.
	bool d_intersects;

	// Triangle list.
	CTriangleListCollisionObject d_triangleList;
	CTerrainAABBIterator d_iterator;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
};

#define AABB_TO_TERRAIN_HPP
#include "collide/AabbToTerrain.hpp"

#endif AABB_TO_TERRAIN_H
