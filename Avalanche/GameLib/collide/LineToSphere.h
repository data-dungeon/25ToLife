/*
**
**  File:   LineToSphere.h
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and spheres.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToSphere.h $
**  $Revision: 4 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef LINE_TO_SPHERE_H
#define LINE_TO_SPHERE_H

// Engine includes //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Line.h"
#include "collide/Sphere.h"

class CCollisionStack;

class CLineToSphereIntersection : public CCollisionIntersection
{
  public:
	CLineToSphereIntersection();
	CLineToSphereIntersection(CLineCollisionObject &i_line,
			CSphereCollisionObject &i_sphere);
	~CLineToSphereIntersection();

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CLineToSphereIntersection* New(CLineCollisionObject &i_line,
			CSphereCollisionObject &i_sphere, CCollisionStack* i_stack);

	// Assignment
	CLineToSphereIntersection& operator=(CLineToSphereIntersection &i_source);

	// Assign new spheres.
	void Set(CLineCollisionObject* i_line, CSphereCollisionObject* i_sphere);

	CLineCollisionObject& Line(void) {return(*d_line);}
	CSphereCollisionObject& Sphere(void) {return(*d_sphere);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the spheres
	// changes state.
	void ClearTests(void);

	// Initial intersection test
	bool IntersectionTest(void);

	// Entry data
	bool Enters(void);
	float EntryFrac(void);
	const Vector3 &EntryPoint(void);
	const Vector3 &EntryNormal(void);

	// Exit data
	bool Exits(void);
	float ExitFrac(void);
	const Vector3 &ExitPoint(void);
	const Vector3 &ExitNormal(void);

	// Found data
	const Vector3 &FoundPoint(void) {return(d_foundPoint);}
	const Vector3 &FoundNormal(void) {return(d_foundNormal);}

	// Determine if a line and a sphere intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(
			CLineCollisionObject* i_line, CSphereCollisionObject* i_sphere,
			CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Control construction on the stack.
	CLineToSphereIntersection(CLineCollisionObject &i_line,
			CSphereCollisionObject &i_sphere, CCollisionStack &i_stack);

	void PerformIntersectionTest(void);

	CLineCollisionObject* d_line;
	CSphereCollisionObject* d_sphere;

	uint d_validFlags;

	// Basic intersection info
	bool d_intersects;
	bool d_enters;
	bool d_exits;
	float d_entryFrac;
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

#define LINE_TO_SPHERE_HPP
#include "collide/LineToSphere.hpp"

#endif // LINE_TO_SPHERE_H
