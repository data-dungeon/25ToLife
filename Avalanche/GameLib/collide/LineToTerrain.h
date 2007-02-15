/*
**
**  File:   LineToTerrain.h
**  Date:   February 19, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToTerrain.h $
**  $Revision: 12 $
**      $Date: 7/25/03 11:42a $
**    $Author: Bcollard $
**
*/

#ifndef LINE_TO_TERRAIN_H
#define LINE_TO_TERRAIN_H

// Engine includes //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Line.h"
#include "collide/Terrain.h"

class TerrainTriangle;
class CLineToTerrainIntersection;
template<class T> class CCollisionStackArray;

class CLineToTerrainPoint
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

	friend class CLineToTerrainIntersection;
};

class CLineToTerrainIntersection : public CCollisionIntersection
{
  public:
	CLineToTerrainIntersection(CLineCollisionObject &i_line,
			CTerrainCollisionObject &i_terrain);
	~CLineToTerrainIntersection();

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CLineToTerrainIntersection* New(CLineCollisionObject &i_line,
			CTerrainCollisionObject &i_terrain, CCollisionStack* i_stack);

	// Find intersections.
	bool IntersectionTest(void);
	bool IntersectionTestAll(void);
	bool IntersectionTestNearest(void);
	bool IntersectionTestFarthest(void);
	bool IntersectionTestAny(void);

	// Get at intersections.
	CLineToTerrainPoint* GetIntersection(int i_index);
	CLineToTerrainPoint* GetFoundPoint(void);

	// Determine if a line and terrain intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(
			CLineCollisionObject* i_line, CTerrainCollisionObject* i_terrain,
			CCollisionStack* i_stack);

	void DrawTerrain(void);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Prevent illegal construction.
	CLineToTerrainIntersection();

	// Control construction on the stack.
	CLineToTerrainIntersection(CLineCollisionObject &i_line,
			CTerrainCollisionObject &i_terrain, CCollisionStack &i_stack);

	// Triangle search.
	bool InitializeFindClosest(void);
	bool InitializeFindFurthest(void);
	bool InitializeFindAny(void);
	bool GetNextTriangle(void);

	// Content.
	CLineCollisionObject &d_line;
	CTerrainCollisionObject &d_terrain;

	// Intersection results.
	CLineToTerrainPoint d_testPoint;
	CLineToTerrainPoint d_foundPoint;
	CCollisionStackArray<CLineToTerrainPoint>* d_pointList;

	// Find triangles.
	CTerrainLineIterator d_iterator;
	CCollisionIntersection* d_previousModifier;
	float d_previousPoint;
};

#define LINE_TO_TERRAIN_HPP
#include "collide/LineToTerrain.hpp"

#endif // LINE_TO_TERRAIN_H
