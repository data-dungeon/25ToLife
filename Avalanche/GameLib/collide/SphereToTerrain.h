/*
**
**  File:   SphereToTerrain.h
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between a sphere and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/SphereToTerrain.h $
**  $Revision: 6 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef SPHERE_TO_TERRAIN_H
#define SPHERE_TO_TERRAIN_H

// Engine include //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Sphere.h"
#include "collide/Terrain.h"
#include "collide/TriangleList.h"
#include "geomUtil/planefit.h"

class TerrainTriangle;
class CCollisionStack;

class CSphereToTerrainIntersection : public CCollisionIntersection
{
  public:
	CSphereToTerrainIntersection(CSphereCollisionObject &i_sphere,
			CTerrainCollisionObject &i_terrain);
	~CSphereToTerrainIntersection();

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CSphereToTerrainIntersection* New(CSphereCollisionObject &i_sphere,
			CTerrainCollisionObject &i_terrain, CCollisionStack* i_stack);

	CSphereCollisionObject &Sphere(void) {return(d_sphere);}
	CTerrainCollisionObject &Terrain(void) {return(d_terrain);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if the sphere changes
	// state.
	void ClearTests(void) {d_validFlags = 0;}

	// See if the sphere touches the terrain at all.
	bool IntersectionTest(void);

	// Get the triangle list
	CTriangleListCollisionObject &GetTriangleList(void) {return(d_triangleList);}

	// Determine if an sphere and terrain intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(CSphereCollisionObject* i_sphere,
			CTerrainCollisionObject* i_terrain, CCollisionStack* i_stack);

	// Draw the terrain touched by the intersection.
	void DrawTerrain(void);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
	virtual void AppendDescription(char* io_msg);
#endif

  private:
	// Prevent construction without an sphere and terrain.
	CSphereToTerrainIntersection();

	// Control construction on the stack.
	CSphereToTerrainIntersection(CSphereCollisionObject &i_sphere,
			CTerrainCollisionObject &i_terrain, CCollisionStack &i_stack);

	void PerformIntersectionTest(void);

	// Participants.
	CSphereCollisionObject &d_sphere;
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

#define SPHERE_TO_TERRAIN_HPP
#include "collide/SphereToTerrain.hpp"

#endif SPHERE_TO_TERRAIN_H
