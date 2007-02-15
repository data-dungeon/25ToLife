/*
**
**  File:   LineToTriangleList.h
**  Date:   August 3, 2004
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and triangle lists.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/LineToTriangleList.h $
**  $Revision: #2 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef LINE_TO_TRIANGLE_LIST_H
#define LINE_TO_TRIANGLE_LIST_H

// Engine includes //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Line.h"
#include "collide/TriangleList.h"

class TerrainTriangle;
class CLineToTerrainIntersection;
template<class T> class CCollisionStackArray;

class CLineToTriangleListPoint
{
  public:
	Vector3 &Intersection(void) {return(d_intersection);}
	float Frac(void) {return(d_frac);}
	Vector3CRef Normal(void) {return(d_triangle->FaceNormal());}
	uint16 Color(void) {return(d_triangle->Color());}
	uint8 Material(void) {return(d_triangle->Material());}

  private:
	TerrainTriangle* d_triangle;
	Vector3 d_intersection;
	float d_frac;

	friend class CLineToTriangleListIntersection;
};

class CLineToTriangleListIntersection : public CCollisionIntersection
{
  public:
	CLineToTriangleListIntersection(CLineCollisionObject &i_line,
			CTriangleListCollisionObject &i_triangleList);
	~CLineToTriangleListIntersection();

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CLineToTriangleListIntersection* New(CLineCollisionObject &i_line,
			CTriangleListCollisionObject &i_triangleList, CCollisionStack* i_stack);

	// Find intersections.
	bool IntersectionTest(void);

	// Get at intersections.
	CLineToTriangleListPoint* GetIntersection(int i_index);
	CLineToTriangleListPoint* GetFoundPoint(void);

	// Determine if a line and triangle list intersect and allocate an
	// intersection object if they do.
	static CCollisionIntersection* TestForIntersection(
			CLineCollisionObject* i_line,
			CTriangleListCollisionObject* i_triangleList, CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Prevent illegal construction.
	CLineToTriangleListIntersection();

	// Control construction on the stack.
	CLineToTriangleListIntersection(CLineCollisionObject &i_line,
			CTriangleListCollisionObject &i_triangleList, CCollisionStack &i_stack);

	// Content.
	CLineCollisionObject &d_line;
	CTriangleListCollisionObject &d_triangleList;

	// Intersection results.
	CLineToTriangleListPoint d_foundPoint;
	CCollisionStackArray<CLineToTriangleListPoint>* d_pointList;

	// Find triangles.
	CCollisionIntersection* d_previousModifier;
	float d_previousPoint;
};

#define LINE_TO_TRIANGLE_LIST_HPP
#include "collide/LineToTriangleList.hpp"

#endif // LINE_TO_TRIANGLE_LIST_H
