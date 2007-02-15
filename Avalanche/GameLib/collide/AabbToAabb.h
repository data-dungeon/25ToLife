/*
**
**  File:   AabbToAabb.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between AABBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbToAabb.h $
**  $Revision: 6 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef AABB_TO_AABB_H
#define AABB_TO_AABB_H

#include "collide/Intersection.h"
#include "collide/AabbObject.h"

class CCollisionStack;

class CAabbToAabbIntersection : public CCollisionIntersection
{
  public:
	CAabbToAabbIntersection();
	CAabbToAabbIntersection(CAabbCollisionObject &i_aabbA,
			CAabbCollisionObject &i_aabbB);

	// Allocate a new intersection object off the stack.
	static CAabbToAabbIntersection* New(CAabbCollisionObject &i_aabbA,
			CAabbCollisionObject &i_aabbB, CCollisionStack* i_stack);

	// Assign new aabbs.
	void SetAabbs(CAabbCollisionObject* i_aabbA, CAabbCollisionObject* i_aabbB);

	CAabbCollisionObject &AabbA(void) {return(*d_aabbA);}
	CAabbCollisionObject &AabbB(void) {return(*d_aabbB);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the AABBs
	// changes state.
	void ClearTests(void) {d_validFlags = 0;}

	// The separating axis test.  This is reasonably fast if they objects don't
	// collide.  If this test passes the boxes are touching.
	bool SeparatingAxisTest(void);

	// Determine if two AABBs intersect and allocate an intersection object
	// if they do.
	static CCollisionIntersection* TestForIntersection(
			CAabbCollisionObject* i_aabbA, CAabbCollisionObject* i_aabbB,
			CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Control construction on the stack.
	CAabbToAabbIntersection(CAabbCollisionObject &i_aabbA,
			CAabbCollisionObject &i_aabbB, CCollisionStack &i_stack);

	CAabbCollisionObject* d_aabbA;
	CAabbCollisionObject* d_aabbB;

	uint d_validFlags;

	// Separating axis test
	bool d_noSeparatingPlane;

	// Valid data masks.
	static const uint SEPARATING_AXIS_TEST;
};

#define AABB_TO_AABB_HPP
#include "collide/AabbToAabb.hpp"

#endif AABB_TO_AABB_H
