/*
**
**  File:   LineToObb.h
**  Date:   February 4, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToObb.h $
**  $Revision: 12 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef LINE_TO_OBB_H
#define LINE_TO_OBB_H

// Engine includes //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Line.h"
#include "collide/obb.h"

class CCollisionStack;

class CLineToObbIntersection : public CCollisionIntersection
{
  public:
	CLineToObbIntersection();
	CLineToObbIntersection(CLineCollisionObject &i_line, OBB &i_obb);
	~CLineToObbIntersection();

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CLineToObbIntersection* New(CLineCollisionObject &i_line,
			OBB &i_obb, CCollisionStack* i_stack);

	// Assignment
	CLineToObbIntersection& operator=(CLineToObbIntersection &i_source);

	// Assign new obbs.
	void Set(CLineCollisionObject* i_line, OBB* i_obb);

	CLineCollisionObject& Line(void) {return(*d_line);}
	OBB& Obb(void) {return(*d_obb);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the OBBs
	// changes state.
	void ClearTests(void);

	// Initial intersection test
	bool IntersectionTest(void);

	// Entry data
	bool Enters(void);
	uint EntryFace(void);
	float EntryFrac(void);
	Vector3CRef EntryPoint(void);
	Vector3CRef EntryNormal(void);

	// Exit data
	bool Exits(void);
	uint ExitFace(void);
	float ExitFrac(void);
	Vector3CRef ExitPoint(void);
	Vector3CRef ExitNormal(void);

	// Found data
	Vector3CRef FoundPoint(void) {return(d_foundPoint);}
	Vector3CRef FoundNormal(void) {return(d_foundNormal);}

	// Determine if a line and OBB intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(
			CLineCollisionObject* i_line, OBB* i_obb, CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Control construction on the stack.
	CLineToObbIntersection(CLineCollisionObject &i_line, OBB &i_obb,
			CCollisionStack &i_stack);

	void PerformIntersectionTest(void);

	CLineCollisionObject* d_line;
	OBB* d_obb;

	uint d_validFlags;

	// Basic intersection info
	bool d_intersects;
	bool d_enters;
	bool d_exits;
	uint d_entryFace;
	float d_entryFrac;
	uint d_exitFace;
	float d_exitFrac;

	// Entry point in world coords
	Vector3 d_entryPoint;

	// Entry normal in world coords
	Vector3 d_entryNormal;

	// Exit point in world coords
	Vector3 d_exitPoint;

	// Exit normal in world coords
	Vector3 d_exitNormal;

	// Find point
	bool d_modifiedLine;
	CCollisionIntersection* d_previousModifier;
	float d_previousPoint;
	Vector3 d_foundPoint;
	Vector3 d_foundNormal;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
	static const uint ENTRY_POINT;
	static const uint ENTRY_NORMAL;
	static const uint EXIT_POINT;
	static const uint EXIT_NORMAL;
};

#define LINE_TO_OBB_HPP
#include "collide/LineToObb.hpp"

#endif // LINE_TO_OBB_H
