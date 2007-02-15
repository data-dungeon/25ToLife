/*
**
**  File:   LineToAabb.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and AABBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToAabb.h $
**  $Revision: 9 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef LINE_TO_AABB_H
#define LINE_TO_AABB_H

// Engine includes //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Line.h"
#include "collide/AabbObject.h"

class CCollisionStack;

class CLineToAabbIntersection : public CCollisionIntersection
{
  public:
	CLineToAabbIntersection(CLineCollisionObject &i_line,
			CAabbCollisionObject &i_aabb);
	~CLineToAabbIntersection();

	// Allocate a new intersection object off the stack.
	static CLineToAabbIntersection* New(CLineCollisionObject &i_line,
			CAabbCollisionObject &i_aabb, CCollisionStack* i_stack);

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the AABBs
	// changes state.
	void ClearTests(void);

	// Initial intersection test
	bool IntersectionTest(void);

	// Entry data
	bool Enters(void);
	uint EntryFace(void);
	float EntryFrac(void);
	Vector3CRef EntryPoint(void);

	// Exit data
	bool Exits(void);
	uint ExitFace(void);
	float ExitFrac(void);
	Vector3CRef ExitPoint(void);

	// Found data
	Vector3CRef FoundPoint(void) {return(d_foundPoint);}
	Vector3CRef FoundNormal(void) {return(d_foundNormal);}

	// Determine if a line and AABB intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(
			CLineCollisionObject* i_line, CAabbCollisionObject* i_aabb,
			CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Prevent illegal construction.
	CLineToAabbIntersection();

	// Control construction on the stack.
	CLineToAabbIntersection(CLineCollisionObject &i_line,
			CAabbCollisionObject &i_aabb, CCollisionStack &i_stack);

	void PerformIntersectionTest(void);

	CLineCollisionObject &d_line;
	CAabbCollisionObject &d_aabb;

	uint d_validFlags;

	// Basic intersection info
	bool d_intersects;
	bool d_enters;
	bool d_exits;
	uint d_entryFace;
	float d_entryFrac;
	uint d_exitFace;
	float d_exitFrac;

	// Entry point
	Vector3 d_entryPoint;

	// Exit point
	Vector3 d_exitPoint;

	// Find point
	bool d_modifiedLine;
	CCollisionIntersection* d_previousModifier;
	float d_previousPoint;
	Vector3 d_foundPoint;
	Vector3 d_foundNormal;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
	static const uint ENTRY_POINT;
	static const uint EXIT_POINT;
};

#define LINE_TO_AABB_HPP
#include "collide/LineToAabb.hpp"

#endif // LINE_TO_AABB_H
