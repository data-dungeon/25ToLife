/*
**
**  File:   SphereToObb.h
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between spheres OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/SphereToObb.h $
**  $Revision: 4 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef SPHERE_TO_OBB_H
#define SPHERE_TO_OBB_H

// Engine includes //
#include "Math/Matrix.h"
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Sphere.h"
#include "collide/obb.h"
#include "geomUtil/planefit.h"

#include "EngineHelper/drawutil.h"

class CCollisionStack;

class CSphereToObbIntersection : public CCollisionIntersection
{
  public:
	CSphereToObbIntersection();
	CSphereToObbIntersection(CSphereCollisionObject &i_sphere, OBB &i_obb);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CSphereToObbIntersection* New(CSphereCollisionObject &i_sphere,
			OBB &i_obb, CCollisionStack* i_stack);

	// Assignment
	CSphereToObbIntersection& operator=(CSphereToObbIntersection &i_source);

	// Assign new obbs.
	void Set(CSphereCollisionObject* i_sphere, OBB* i_obb);

	// Access
	CSphereCollisionObject &Sphere(void) {return(*d_sphere);}
	OBB &Obb(void) {return(*d_obb);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the objects
	// change state.
	void ClearTests(void) {d_validFlags = 0;}

	// Return true if the collision could occur. While fast, a false result does
	// not guarantee an non-intersection.
	bool IncludeRadiusTest(void);

	// Return true if the collision could occur. While fast, a true result does
	// not guarantee an intersection.
	bool ExcludeRadiusTest(void);

	// The separating axis test.  This is reasonably fast if the objects don't
	// collide.  If this test passes the objects are touching.
	bool SeparatingAxisTest(void);

	const Vector3 &ClosestPoint(void);
	bool CenterInside(void);

	// Determine if a sphere and OBB intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(
			CSphereCollisionObject* i_sphere, OBB* i_obb,
			CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Control construction on the stack.
	CSphereToObbIntersection(CSphereCollisionObject &i_sphere, OBB &i_obb,
			CCollisionStack &i_stack);

	CSphereCollisionObject* d_sphere;
	OBB* d_obb;

	uint d_validFlags;

	// Include radius test
	bool d_obbCenterIsInSphere;

	// Exclude radius test
	bool d_spheresIntersect;

	// Separating axis test
	bool d_intersects;

	// Relative centers
	Vector3 d_sphereFromObb;
	float d_centerDistanceSquared;

	// Point closest to the center.
	bool d_centerInside;
	Vector3 d_closestPoint;

	// Valid data masks.
	static const uint INCLUDE_RADIUS_TEST;
	static const uint EXCLUDE_RADIUS_TEST;
	static const uint SEPARATING_AXIS_TEST;
};

#define SPHERE_TO_OBB_HPP
#include "collide/SphereToObb.hpp"

#endif SPHERE_TO_OBB_H
