/*
**
**  File:   Line.h
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A line collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Line.h $
**  $Revision: 6 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifndef LINE_H
#define LINE_H

#include "collide/Object.h"
#include "Math/Vector.h"
#include "EngineHelper/drawutil.h"

class XZRect;
class CCollisionIntersection;

#define LINE_FIND_ALL      0x00
#define LINE_FIND_CLOSEST  0x10
#define LINE_FIND_FURTHEST 0x20
#define LINE_FIND_ANY      0x30

class CLineCollisionObject : public CCollisionObject
{
  public:

	static const uint BOUNDARY_NONE;
	static const uint BOUNDARY_MIN;
	static const uint BOUNDARY_MAX;
	static const uint BOUNDARY_MIN_MAX;

	static const uint DETECT_NEITHER_SIDE;
	static const uint DETECT_FRONT_SIDE;
	static const uint DETECT_BACK_SIDE;
	static const uint DETECT_BOTH_SIDES;

	static const uint FIND_ALL;
	static const uint FIND_CLOSEST;
	static const uint FIND_FURTHEST;
	static const uint FIND_ANY;

	// Constructors
	CLineCollisionObject();
	CLineCollisionObject(Vector3CRef i_origin, Vector3CRef i_basis,
			uint i_detectionAbility = DETECT_BOTH_SIDES,
			uint i_boundary = BOUNDARY_MIN_MAX, float i_min = 0.0f,
			float i_max = 1.0f, uint i_findMode = FIND_ALL);

	// Set definition properties.
	void SetOriginBasis(Vector3CRef i_origin, Vector3CRef i_basis);
	void SetOriginEnd(Vector3CRef i_origin, Vector3CRef i_end);
	void SetBasisEnd(Vector3CRef i_basis, Vector3CRef i_end);

	// Set detection ability properties.
	void SetDetectionAbility(uint i_detectionAbility);

	// Set boundary properties.
	void SetBoundary(uint i_condition, float i_min = 0.0f, float i_max = 1.0f);
	void SetBoundaryCondition(uint i_condition);
	void BitwiseSetBoundaryCondition(uint i_condition);
	void BitwiseClearBoundaryCondition(uint i_condition);
	void SetMinBoundary(float i_min);
	void SetMaxBoundary(float i_max);

	// Set find mode properties.
	void SetFindMode(uint i_findMode);

	// Get definition properties.
	Vector3& Origin(void);
	Vector3& Basis(void);
	Vector3CRef ConstOrigin(void) const;
	Vector3CRef ConstBasis(void) const;
	Vector3 End(void) const;

	// Get detection ability properties.
	uint DetectionAbility(void) const;
	bool DetectFrontSide(void) const;
	bool DetectBackSide(void) const;
	bool DetectBothSides(void) const;

	// Get boundary properties.
	uint BoundaryCondition(void) const;
	bool TestMinBoundary(void) const;
	bool TestMaxBoundary(void) const;
	bool TestBothBoundaries(void) const;
	float MinBoundary(void) const;
	float MaxBoundary(void) const;

	// Get find mode properties.
	uint FindMode(void) const;
	float FoundPoint(void) const;
	CCollisionIntersection* Modifier(void) const;
	bool FindPoint(void) const;

	// Evaluate points and ranges.
	bool IsOn(float i_value) const;
	bool IsOn(float i_rangeMin, float i_rangeMax) const;

	// Find mode actions.
	void InitializeFindMode(void);
	void TerminateFindMode(void);
	bool SetClosestPoint(float i_point);
	bool SetFurthestPoint(float i_point);
	void SetAnyPoint(float i_point);
	CCollisionIntersection* SetModifier(CCollisionIntersection* i_newModifier);
	void RestoreFindState(CCollisionIntersection* i_restoringModifier,
			CCollisionIntersection* i_previousModifier, float i_previousPoint);

	// Find world extents that contain the min and max boundary points.
	// This ignores the boundary condition setting and if the min value is
	// greater than the max value.
	void ComputeWorldExtents(XZRect &o_xzRect) const;

	// Compute points.
	void ComputePoint(float i_frac, Vector3 &o_point) const;
	Vector3 ComputePoint(float i_frac) const;

	// Plane detection.
	bool DetectPlane(Vector3CRef i_normal, Vector3CRef i_point,
			float &o_frac) const;

	// Plane intersection.
	bool IntersectPlane(Vector3CRef i_normal, Vector3CRef i_point,
			float &o_frac, Vector3 &o_intersection) const;

	// Debugging
	void Draw(DrawUtility::COLOR i_color);

  private:
	// Definition
	Vector3 d_origin;
	Vector3 d_basis;

	// Configuration flags
	uint d_configuration;

	// Boundary values
	float d_min;
	float d_max;

	// Find mode support
	float d_base;
	CCollisionIntersection* d_modifier;
	bool d_findModeActive;

	float d_normalizedBasisLength;

	// Configuration masks
	static const uint BOUNDARY_MASK;
	static const uint DETECT_MASK;
	static const uint FIND_MASK;
};

#define LINE_HPP
#include "collide/Line.hpp"

#endif // LINE_H
