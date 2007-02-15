/*
**
**  File:   ObbToObb.h
**  Date:   February 4, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/ObbToObb.h $
**  $Revision: 13 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef OBB_TO_OBB_H
#define OBB_TO_OBB_H

// Engine includes //
#include "Math/Matrix.h"
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/obb.h"
#include "geomUtil/planefit.h"

#include "EngineHelper/drawutil.h"

class CCollisionStack;

// I think I can at least justify this number. Each edge of each OBB could
// conceivably hit two faces. However, when an edge of one OBB penetrates a
// face, it effectively prevents at least one of the edges of the other OBB
// from penetrating any faces at all. So there could be no more than two
// times the number of edges in one OBB.
#define MAX_EDGE_FACE_INTERSECTIONS 48

class CObbToObbIntersection : public CCollisionIntersection
{
  public:
	CObbToObbIntersection();
	CObbToObbIntersection(OBB &i_obbA, OBB &i_obbB);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CObbToObbIntersection* New(OBB &i_obbA, OBB &i_obbB,
			CCollisionStack* i_stack);

	// Assignment
	CObbToObbIntersection& operator=(CObbToObbIntersection &i_source);

	// Assign new obbs.
	void SetObbs(OBB* i_obbA, OBB* i_obbB);

	OBB &ObbA(void) {return(*d_obbA);}
	OBB &ObbB(void) {return(*d_obbB);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the OBBs
	// changes state.
	void ClearTests(void) {d_validFlags = 0;}

	// Return true if the collision could occur.
	// This is very fast, but very inaccurate
	bool RadiusTest(void);

	// The separating axis test.  This is reasonably fast if they objects don't
	// collide.  If this test passes the boxes are touching.
	bool SeparatingAxisTest(void);

	// Get the vector from A towards B that would move the OBBs to the
	// closest separating plane. Returns false if the OBBs are not intersecting.
	bool ClosestSeparatingAxis(Vector3 &o_axis);

	// Get the transform from OBB A to OBB B.
	Cartesian4x4 &ObbAToObbB(void);

	// Select faces of an OBB whose normals point in some general direction. If
	// i_selectAFaces is true, faces on OBB A are selected; otherwise, faces on
	// OBB B are selected. Faces whose normals point in the general direction of
	// i_direction (expressed in world coordinates) are selected. To get expected
	// results, i_direction should be a unit vector or i_scaledCosAngle should be
	// the cosine of the allowed angle between the normal and i_direction scaled
	// by the magnitude of i_direction. Multiple selections can be combined by
	// performing a bitwise OR of selection values. Selections can be inverted by
	// performing a one's complement on a selection value.
	uint16 SelectFaces(bool i_selectAFaces, Vector3CRef i_direction,
			float i_scaledCosAngle);

	// Selects all of the faces of an OBB. If i_selectAFaces is true, faces of
	// OBB A are selected; otherwise, faces of OBB B are selected. Multiple
	// selections can be combined by performing a bitwise OR of selection
	// values. Selections can be inverted by performing a one's complement on a
	// selection value.
	uint16 SelectAllFaces(bool i_selectAFaces);

	// Find the intersections of the edges of each box with the faces of the
	// other. Returns the number of intersections.
	int EdgeIntersections(void);

	// Get a specific intersection
	const Vector3* EdgeIntersection(int i_index);

	// Get a specific intersection if selected. If not selected, return NULL.
	const Vector3* EdgeIntersection(int i_index, uint16 i_selectedFaceMask);

	// Get the sum of the edge/face intersection normals.
	Vector3CRef EdgeIntersectionNormalSum(void);

	// Get the sum of the selected edge/face intersection normals.
	Vector3 EdgeIntersectionNormalSum(uint16 i_selectedFaceMask);

	// Add the edge/face intersections to a plane fit object.
	int AddEdgeIntersections(CPlaneFit &io_planeFit);

	// Fit a plane to the edge/face intersection points. If there are no
	// intersection points, position the plane between the two OBB centers.
	void EdgeIntersectionPlaneFit(Vector3 &o_loc, Vector3 &o_normal);

	// Determine if two OBBs intersect and allocate an intersection object
	// if they do.
	static CCollisionIntersection* TestForIntersection(OBB* i_obbA, OBB* i_obbB,
			CCollisionStack* i_stack);

	// Draw points.
	void DrawPoints(DrawUtility::COLOR i_color = DrawUtility::BLUE);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

  private:
	// Control construction on the stack.
	CObbToObbIntersection(OBB &i_obbA, OBB &i_obbB, CCollisionStack &i_stack);

	enum ESeparatingAxis
	{
		A0,
		A1,
		A2,
		B0,
		B1,
		B2,
		A0XB0,
		A0XB1,
		A0XB2,
		A1XB0,
		A1XB1,
		A1XB2,
		A2XB0,
		A2XB1,
		A2XB2,
		NUM_SEPARATING_AXIS
	};

	void CompareSeparatingAxes(int i_testAxis, float i_axisComp1,
			float i_axisComp2, int &io_closestAxis, float &io_closestLengthSq,
			float &io_closestOverlapSq) const;

	void PerformEdgeIntersections(void);
	void FindEdgeObbIntersections(const Cartesian4x4 &i_edgeToFace,
			Vector3CRef i_edgeSize, const OBB &i_faceOBB, uint8 i_obbFlag);
	void FindEdgeFaceIntersections(Vector3CRef i_center, Vector3CRef i_halfEdge,
			const OBB &i_faceObb, uint8 i_obbFlag, uint8 i_edgeID);

	void PerformEdgeIntersectionNormalSum(void);

	OBB* d_obbA;
	OBB* d_obbB;

	uint d_validFlags;

	// Radius test
	bool d_spheresIntersect;

	// Separating axis test
	bool d_noSeparatingPlane;

	// Closest separating axis
	float d_axisOverlap[NUM_SEPARATING_AXIS];
	Vector3 d_closestSeparatingAxis;

	// Transform from OBB A to OBB B
	Cartesian4x4 d_aToB;

	// Edge/Face intersection points
	int d_numPoints;

	// The LSB (bit 1) is unset for the negative direction and set for the
	// positive direction. Bits 2 and 3 contain the index (0, 1, 2) of the axis
	// the face is perpendicular to. Bit 4 is unset for OBB A and set for OBB
	// B. The face normal is just the appropriate row of the OBB matrix.
	uint8 d_faceID[MAX_EDGE_FACE_INTERSECTIONS];

	// The edge id as defined for OBBs. The MSB of the corresponding face id
	// indicates OBB with the intersected face; therefore, the edge is in the
	// other OBB.
	uint8 d_edgeID[MAX_EDGE_FACE_INTERSECTIONS];

	// Since intersections only occur on faces, the intersection location
	// could be stored in two floats and then the face ID could be used to
	// form a 3D location. That seems like unnecessary extra overhead.
	Vector3 d_point[MAX_EDGE_FACE_INTERSECTIONS];

	// Sum of the normals of the edge/face intersection points
	Vector3 d_normalSum;

	// Intersection plane derived from the edge/face intersection points
	CPlaneFit::EPlaneFitResult d_planeResult;
	Vector3 d_planeLocation;
	Vector3 d_planeNormal;

	// Valid data masks.
	static const uint RADIUS_TEST;
	static const uint SEPARATING_AXIS_TEST;
	static const uint CLOSEST_SEPARATING_AXIS;
	static const uint TRANSFORM;
	static const uint INTERSECTION_POINTS;
	static const uint INTERSECTION_NORMAL_SUM;
	static const uint INTERSECTION_PLANE;

	// Map edge ids to face masks.
	static const uint16 EDGE_MAP[12];
};

#define OBB_TO_OBB_HPP
#include "collide/ObbToObb.hpp"

#endif OBB_TO_OBB_H
