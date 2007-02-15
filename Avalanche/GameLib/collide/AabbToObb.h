/*
**
**  File:   AabbToObb.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between an AABB and OBB.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbToObb.h $
**  $Revision: 5 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef AABB_TO_OBB_H
#define AABB_TO_OBB_H

#include "collide/Intersection.h"
#include "collide/AabbObject.h"
#include "collide/obb.h"

class CCollisionStack;

class CAabbToObbIntersection : public CCollisionIntersection
{
  public:
	CAabbToObbIntersection();
	CAabbToObbIntersection(CAabbCollisionObject &i_aabb, OBB &i_obb);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CAabbToObbIntersection* New(CAabbCollisionObject &i_aabb, OBB &i_obb,
			CCollisionStack* i_stack);

	// Determine if two OBBs intersect and allocate an intersection object
	// if they do.
	static CCollisionIntersection* TestForIntersection(
			CAabbCollisionObject* i_aabb, OBB* i_obb, CCollisionStack* i_stack);

	// Assignment
	CAabbToObbIntersection& operator=(CAabbToObbIntersection &i_source);
	void Set(CAabbCollisionObject* i_aabb, OBB* i_obb);

	CAabbCollisionObject &GetAabb(void) {return(*d_aabb);}
	OBB &GetObb(void) {return(*d_obb);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the OBBs
	// changes state.
	void ClearTests(void) {d_validFlags = 0;}

	// The separating axis test.  This is reasonably fast if they objects don't
	// collide.  If this test passes the boxes are touching.
	bool SeparatingAxisTest(void);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Control construction on the stack.
	CAabbToObbIntersection(CAabbCollisionObject &i_aabb, OBB &i_obb,
			CCollisionStack &i_stack);

	CAabbCollisionObject* d_aabb;
	OBB* d_obb;

	uint d_validFlags;

	// Separating axis test
	bool d_noSeparatingPlane;

	// Valid data masks.
	static const uint SEPARATING_AXIS_TEST;
};

#define AABB_TO_OBB_HPP
#include "collide/AabbToObb.hpp"

#endif // AABB_TO_OBB_H
