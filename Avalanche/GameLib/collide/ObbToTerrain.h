/*
**
**  File:   ObbToTerrain.h
**  Date:   February 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between an OBB and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/ObbToTerrain.h $
**  $Revision: 20 $
**      $Date: 8/01/03 5:28p $
**    $Author: Bcollard $
**
*/

#ifndef OBB_TO_TERRAIN_H
#define OBB_TO_TERRAIN_H

// Engine include //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/obb.h"
#include "collide/Terrain.h"
#include "collide/TriangleList.h"
#include "geomUtil/planefit.h"

class TerrainTriangle;
class CCollisionStack;
template<class T> class CCollisionStackArray;

class CObbToTerrainIntersection : public CCollisionIntersection
{
  public:
	CObbToTerrainIntersection(OBB &i_obb, CTerrainCollisionObject &i_terrain);
	~CObbToTerrainIntersection();

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CObbToTerrainIntersection* New(OBB &i_obb,
			CTerrainCollisionObject &i_terrain, CCollisionStack* i_stack);

	OBB &Obb(void) {return(d_obb);}
	CTerrainCollisionObject &Terrain(void) {return(d_terrain);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if the OBB changes
	// state.
	void ClearTests(void) {d_validFlags = 0;}

	// See if the OBB touches the terrain at all.
	bool IntersectionTest(void);

	// Get the triangle list
	CTriangleListCollisionObject &GetTriangleList(void) {return(d_triangleList);}

	// Get the sum of the triangle normals.
	Vector3CRef TriangleNormalSum(void);

	// Get the sum of the triangle normals that point in a general direction.
	// To get expected results, i_direction should be a unit vector or
	// i_scaledCosAngle should be the cosine of the allowed angle between the
	// normal and i_direction scaled by the magnitude of i_direction.
	Vector3 TriangleNormalSum(Vector3CRef i_direction, float i_scaledCosAngle);

	// Add the OBB edge/triangle intersections to a plane fit object.
	void AddObbEdgeIntersections(CPlaneFit &io_planeFit);

	// Fit a plane to the edge/face intersection points. If there are no
	// intersection points, position the plane at the OBB center.
	void ObbEdgeIntersectionPlaneFit(Vector3 &o_loc, Vector3 &o_normal);

	// Serve a list of OBB edge/triangle intersections.
	struct SObbEdgeTerrainState
	{
		int d_entry;
		OBB::SObbPlaneIntersections d_intersections;
		enum
		{
			PLANE_YZ,
			PLANE_XZ,
			PLANE_XY
		} d_plane;
		int d_path;
	};
	bool InitObbEdgeIntersectionTest(SObbEdgeTerrainState &i_state);
	bool GetNextObbEdgeIntersection(SObbEdgeTerrainState &io_state,
			TerrainTriangle* &o_triangle, Vector3 &o_intersection);

	// Get at the triangle edge/obb intersections.
	CCollisionStackArray<Vector3>* GetTriangleIntersections(void);

	// Get at the triangle (whose normals point in a general direction) edge/obb
	// intersections. To get expected results, i_direction should be a unit
	// vector or i_scaledCosAngle should be the cosine of the allowed angle
	// between the normal and i_direction scaled by the magnitude of
	// i_direction. Note that this routine constructs the list on the stack.
	CCollisionStackArray<Vector3>* GetTriangleIntersections(
			Vector3CRef i_direction, float i_scaledCosAngle);

	// Get at the terrain vertices in the obb.
	CCollisionStackArray<Vector3>* GetTriangleVertices(void);

	// Determine if an OBB and terrain intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(OBB* i_obb,
			CTerrainCollisionObject* i_terrain, CCollisionStack* i_stack);

	// Draw the terrain touched by the intersection.
	void DrawTerrain(void);

	// Draw stuff to debug the intersection.
	void DrawDebug(void);
	void DrawDebugTerrain(void);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
	virtual void AppendDescription(char* io_msg);
#endif

  private:
	// Prevent construction without an OBB and terrain.
	CObbToTerrainIntersection();

	// Control construction on the stack.
	CObbToTerrainIntersection(OBB &i_obb, CTerrainCollisionObject &i_terrain,
			CCollisionStack &i_stack);

	void PerformIntersectionTest(void);
	void PerformTriangleNormalSum(void);
	void PerformAddObbEdgeIntersections(CPlaneFit &io_planeFit);
	void BuildTriangleIntersections(void);
	bool TestAndAddTriangleIntersections(int i_edge,
			CCollisionStackArray<Vector3>* io_triangleIntersection);
	void BuildTriangleVertices(void);

	// Participants.
	OBB &d_obb;
	CTerrainCollisionObject &d_terrain;

	// Valid data flags.
	uint d_validFlags;

	// Intersection test.
	bool d_intersects;

	// Triangle list.
	CTriangleListCollisionObject d_triangleList;
	CTerrainOBBIterator d_iterator;

	// Sum of the normals of the triangles.
	Vector3 d_normalSum;

	// Intersection plane derived from the OBB edge/triangle intersection points
	CPlaneFit::EPlaneFitResult d_planeResult;
	Vector3 d_planeLocation;
	Vector3 d_planeNormal;

	// Triangle edge/obb intersections
	CCollisionStackArray<Vector3>* d_triangleIntersection;

	// Contained triangle verticies
	CCollisionStackArray<Vector3>* d_triangleVertex;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
	static const uint TRIANGLE_NORMAL_SUM;
	static const uint INTERSECTION_PLANE;
	static const uint TRIANGLE_INTERSECTION;
	static const uint TRIANGLE_VERTEX;
};

#define OBB_TO_TERRAIN_HPP
#include "collide/ObbToTerrain.hpp"

#endif OBB_TO_TERRAIN_H
