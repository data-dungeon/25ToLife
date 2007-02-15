/*
**
**  File:   Line.hpp
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A line collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Line.hpp $
**  $Revision: 7 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef LINE_HPP
#undef LINE_HPP

#include "collide/xzrect.h"

#define LINE_BOUNDARY_NONE 0x00
#define LINE_BOUNDARY_MIN  0x01
#define LINE_BOUNDARY_MAX  0x02

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetOriginBasis(Vector3CRef i_origin,
		Vector3CRef i_basis)
{
	ASSERT(!d_findModeActive);
	d_origin = i_origin;
	d_basis = i_basis;
	d_normalizedBasisLength = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetOriginEnd(Vector3CRef i_origin,
		Vector3CRef i_end)
{
	ASSERT(!d_findModeActive);

	d_origin = i_origin;
	d_basis = i_end - i_origin;
	d_normalizedBasisLength = d_basis.Length();
	if( Math::Zero( d_normalizedBasisLength ) )
		d_basis.Set( 0.0f, 0.0f, 1.0f );
	else
		d_basis = d_basis / d_normalizedBasisLength;

	SetMaxBoundary( d_normalizedBasisLength );
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetBasisEnd(Vector3CRef i_basis,
		Vector3CRef i_end)
{
	ASSERT(!d_findModeActive);
	d_basis = i_basis;
	d_origin = i_end - i_basis;
	d_normalizedBasisLength = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetDetectionAbility(uint i_detectionAbility)
{
	d_configuration &= ~DETECT_MASK;
	d_configuration |= (i_detectionAbility & DETECT_MASK);
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetBoundary(uint i_condition, float i_min,
		float i_max)
{
	ASSERT(!d_findModeActive);
	d_configuration &= ~BOUNDARY_MASK;
	d_configuration |= (i_condition & BOUNDARY_MASK);
	d_min = i_min;
	d_max = i_max;
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetBoundaryCondition(uint i_condition)
{
	ASSERT(!d_findModeActive);
	d_configuration &= ~BOUNDARY_MASK;
	d_configuration |= (i_condition & BOUNDARY_MASK);
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::BitwiseSetBoundaryCondition(uint i_condition)
{
	ASSERT(!d_findModeActive);
	d_configuration |= (i_condition & BOUNDARY_MASK);
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::BitwiseClearBoundaryCondition(
		uint i_condition)
{
	ASSERT(!d_findModeActive);
	d_configuration &= ~(i_condition & BOUNDARY_MASK);
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetMinBoundary(float i_min)
{
	ASSERT(!d_findModeActive);
	d_min = i_min;
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetMaxBoundary(float i_max)
{
	ASSERT(!d_findModeActive);
	d_max = i_max;
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetFindMode(uint i_findMode)
{
	ASSERT(!d_findModeActive);
	d_configuration &= ~FIND_MASK;
	d_configuration |= (i_findMode & FIND_MASK);
}

/***************************************************************************/
/***************************************************************************/
inline CLineCollisionObject::CLineCollisionObject() :
	CCollisionObject(LINE)
{
	d_origin.Clear();
	d_basis.Set(1.0f, 0.0f, 0.0f);
	d_configuration = 0x0;
	d_findModeActive = false;
	SetDetectionAbility(DETECT_BOTH_SIDES);
	SetBoundary(BOUNDARY_MIN_MAX, 0.0f, 1.0f);
	SetFindMode(FIND_ALL);
}

/***************************************************************************/
/***************************************************************************/
inline CLineCollisionObject::	CLineCollisionObject(Vector3CRef i_origin,
		Vector3CRef i_basis, uint i_detectionAbility, uint i_boundary,
		float i_min, float i_max, uint i_findMode) :
	CCollisionObject(LINE)
{
	d_findModeActive = false;
	SetOriginBasis(i_origin, i_basis);
	d_configuration = 0x0;
	SetDetectionAbility(i_detectionAbility);
	SetBoundary(i_boundary, i_min, i_max);
	SetFindMode(i_findMode);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3& CLineCollisionObject::Origin(void)
{
	return(d_origin);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3& CLineCollisionObject::Basis(void)
{
	return(d_basis);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CLineCollisionObject::ConstOrigin(void) const
{
	return(d_origin);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CLineCollisionObject::ConstBasis(void) const
{
	return(d_basis);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3 CLineCollisionObject::End(void) const
{
	Vector3 end = d_origin + d_basis;
	return(end);
}

/***************************************************************************/
/***************************************************************************/
inline uint CLineCollisionObject::DetectionAbility(void) const
{
	return(d_configuration & DETECT_MASK);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::DetectFrontSide(void) const
{
	return((d_configuration & DETECT_FRONT_SIDE) != 0);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::DetectBackSide(void) const
{
	return((d_configuration & DETECT_BACK_SIDE) != 0);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::DetectBothSides(void) const
{
	return((d_configuration & DETECT_MASK) == DETECT_BOTH_SIDES);
}

/***************************************************************************/
/***************************************************************************/
inline uint CLineCollisionObject::BoundaryCondition(void) const
{
	return(d_configuration & BOUNDARY_MASK);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::TestMinBoundary(void) const
{
	return((d_configuration & BOUNDARY_MIN) != 0);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::TestMaxBoundary(void) const
{
	return((d_configuration & BOUNDARY_MAX) != 0);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::TestBothBoundaries(void) const
{
	return((d_configuration & BOUNDARY_MASK) == BOUNDARY_MIN_MAX);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineCollisionObject::MinBoundary(void) const
{
	return(d_min);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineCollisionObject::MaxBoundary(void) const
{
	return(d_max);
}

/***************************************************************************/
/***************************************************************************/
inline uint CLineCollisionObject::FindMode(void) const
{
	return(d_configuration & FIND_MASK);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineCollisionObject::FoundPoint(void) const
{
	if( Math::Zero( d_normalizedBasisLength ) )
		return 0.0f;

	return(d_base / d_normalizedBasisLength);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CLineCollisionObject::Modifier(void) const
{
	return(d_modifier);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::FindPoint(void) const
{
	return(((d_configuration & FIND_MASK) == FIND_CLOSEST) ||
			((d_configuration & FIND_MASK) == FIND_FURTHEST));
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::IsOn(float i_value) const
{
	switch (d_configuration & BOUNDARY_MASK)
	{
	 case LINE_BOUNDARY_NONE:
		return(false);
	 case LINE_BOUNDARY_MIN:
		 return(i_value >= d_min);
	 case LINE_BOUNDARY_MAX:
		 return(i_value <= d_max);
	}
	return((i_value >= d_min) && (i_value <= d_max));
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::IsOn(float i_rangeMin, float i_rangeMax) const
{
	if (i_rangeMin > i_rangeMax)
	{
		float swapTemp = i_rangeMin;
		i_rangeMin = i_rangeMax;
		i_rangeMax = swapTemp;
	}

	switch (d_configuration & BOUNDARY_MASK)
	{
	 case LINE_BOUNDARY_NONE:
		return(false);
	 case LINE_BOUNDARY_MIN:
		 return(i_rangeMax >= d_min);
	 case LINE_BOUNDARY_MAX:
		 return(i_rangeMin <= d_max);
	}
	return((i_rangeMax >= d_min) && (i_rangeMin <= d_max) && (d_min <= d_max));
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::InitializeFindMode(void)
{
	ASSERT(!d_findModeActive);
	d_findModeActive = true;
	d_modifier = NULL;
	switch (d_configuration & FIND_MASK)
	{
	 case LINE_FIND_CLOSEST:
		d_base = d_max;
		break;
	 case LINE_FIND_FURTHEST:
		d_base = d_min;
		break;
	}
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::TerminateFindMode(void)
{
	ASSERT(d_findModeActive);
	d_findModeActive = false;
	switch (d_configuration & FIND_MASK)
	{
	 case LINE_FIND_CLOSEST:
	 {
		float point = d_max;
		d_max = d_base;
		d_base = point;
		break;
	 }
	 case LINE_FIND_FURTHEST:
	 {
		float point = d_min;
		d_min = d_base;
		d_base = point;
		break;
	 }
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::SetClosestPoint(float i_point)
{
	ASSERT(d_findModeActive &&
			((d_configuration & FIND_MASK) == LINE_FIND_CLOSEST));

	if (i_point < d_max)
	{
		d_max = i_point;
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::SetFurthestPoint(float i_point)
{
	ASSERT(d_findModeActive &&
			((d_configuration & FIND_MASK) == LINE_FIND_FURTHEST));

	if (i_point > d_min)
	{
		d_min = i_point;
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::SetAnyPoint(float i_point)
{
	ASSERT(d_findModeActive && (d_modifier == NULL) &&
			((d_configuration & FIND_MASK) == LINE_FIND_ANY));

	d_base = i_point;
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CLineCollisionObject::SetModifier(
		CCollisionIntersection* i_newModifier)
{
	ASSERT(d_findModeActive);

	CCollisionIntersection* previousModifier = d_modifier;
	d_modifier = i_newModifier;
	return(previousModifier);
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::RestoreFindState(
		CCollisionIntersection* i_restoringModifier,
		CCollisionIntersection* i_previousModifier, float i_previousPoint)
{
	if (d_findModeActive && (i_restoringModifier == d_modifier))
	{
		d_modifier = i_previousModifier;
		switch (d_configuration & FIND_MASK)
		{
		 case LINE_FIND_CLOSEST:
			d_max = i_previousPoint;
			break;
		 case LINE_FIND_FURTHEST:
			d_min = i_previousPoint;
			break;
		 case LINE_FIND_ANY:
			d_base = i_previousPoint;
			break;
		}
	}
}

/***************************************************************************/
// Find world extents that contain the min and max boundary points.
// This ignores the boundary condition setting and if the min value is
// greater than the max value.
/***************************************************************************/
inline void CLineCollisionObject::ComputeWorldExtents(XZRect &o_xzRect) const
{
	float minX = d_origin.x() + d_min * d_basis.x();
	float maxX = d_origin.x() + d_max * d_basis.x();
	if (minX <= maxX)
	{
		o_xzRect.MinX(minX);
		o_xzRect.MaxX(maxX);
	}
	else
	{
		o_xzRect.MinX(maxX);
		o_xzRect.MaxX(minX);
	}

	float minZ = d_origin.z() + d_min * d_basis.z();
	float maxZ = d_origin.z() + d_max * d_basis.z();
	if (minZ <= maxZ)
	{
		o_xzRect.MinZ(minZ);
		o_xzRect.MaxZ(maxZ);
	}
	else
	{
		o_xzRect.MinZ(maxZ);
		o_xzRect.MaxZ(minZ);
	}
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::ComputePoint(float i_frac,
		Vector3 &o_point) const
{
	o_point = d_origin + i_frac * d_basis;
}

/***************************************************************************/
/***************************************************************************/
inline Vector3 CLineCollisionObject::ComputePoint(float i_frac) const
{
	Vector3 point = d_origin + i_frac * d_basis;
	return(point);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::DetectPlane(Vector3CRef i_normal,
		Vector3CRef i_point, float &o_frac) const
{
/*
 *	Compute the component of the basis in the normal direction.
 */
	float normalComp = i_normal.Dot(d_basis);
/*
 *	See if the line can detect this side of the plane.
 */
	if ((!DetectFrontSide() && (normalComp <= 0.0f)) ||
			(!DetectBackSide() && (normalComp >= 0.0f)))
		return(false);
/*
 *	Compute the distance from the origin to the plane.
 */
	o_frac = i_normal.Dot(i_point - d_origin);
/*
 *	See if the basis is parallel to the plane.
 */
	if (Math::DivByZero(o_frac, normalComp))
		return(false);
/*
 *	Compute the fractional distance along the line and return success.
 */
	o_frac /= normalComp;
/*
 *	Check boundaries.
 */
	return(IsOn(o_frac));
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineCollisionObject::IntersectPlane(Vector3CRef i_normal,
		Vector3CRef i_point, float &o_frac, Vector3 &o_intersection) const
{
	if (!DetectPlane(i_normal, i_point, o_frac))
		return(false);

	o_intersection = d_origin + o_frac * d_basis;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline void CLineCollisionObject::Draw(DrawUtility::COLOR i_color)
{
	DrawUtility::Line(d_origin + d_min * d_basis, d_origin + d_max * d_basis,
			i_color);
}

#endif // LINE_HPP
