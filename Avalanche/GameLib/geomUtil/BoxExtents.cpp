/*
**
**  File:   BoxExtents.cpp
**  Date:   July 16, 2004
**  By:     Bryant Collard
**  Desc:   A utility that finds the extreme corners of a box.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Node.h $
**  $Revision: #2 $
**      $Date: 2005/03/29 $
**    $Author: Rob $
**
*/

// Pre-compiled header
#include "geomUtil/GeomUtilPCH.h"

// My header (always first)
#include "geomUtil/BoxExtents.h"

/******************************************************************************/
/******************************************************************************/
CBoxExtents::CBoxExtents()
{
	Initialize();
}

/******************************************************************************/
// Initialize member variables. This sets the reference to target transformation
// to identity and resets the min and max values.
/******************************************************************************/
void CBoxExtents::Initialize(void)
{
	d_refToTarget.Identity();
	Reset();
}

/******************************************************************************/
// Reset the min and max values but leave the reference to target transformation
// alone.
/******************************************************************************/
void CBoxExtents::Reset(void)
{
	d_minExtent.Set(MAXFLOAT, MAXFLOAT, MAXFLOAT);
	d_maxExtent.Set(-MAXFLOAT, -MAXFLOAT, -MAXFLOAT);
}

/******************************************************************************/
// Information about the box of interest is expressed in some reference
// system. It may be desirable to find the resulting extents in some other
// target system. Specify a transformation to that target system.
/******************************************************************************/
void CBoxExtents::SetRefToTarget(const Graphics4x4 &i_refToTarget)
{
	d_refToTarget = i_refToTarget;
}

/******************************************************************************/
// Information about the box of interest is expressed in some reference
// system. It may be desirable to find the resulting extents in some other
// target system. Specify a transformation to that target system.
/******************************************************************************/
void CBoxExtents::SetRefToTarget(Vector3CRef i_refInTarget,
		const Matrix3x3 &i_refToTarget)
{
	d_refToTarget = i_refToTarget;
	d_refToTarget.PokeTranslation(i_refInTarget);
}

/******************************************************************************/
// Information about the box of interest is expressed in some reference
// system. It may be desirable to find the resulting extents in some other
// target system. Specify a transformation to that target system.
/******************************************************************************/
void CBoxExtents::SetTargetToRef(const Graphics4x4 &i_targetToRef)
{
	i_targetToRef.Invert(d_refToTarget);
}

/******************************************************************************/
// Information about the box of interest is expressed in some reference
// system. It may be desirable to find the resulting extents in some other
// target system. Specify a transformation to that target system.
/******************************************************************************/
void CBoxExtents::SetTargetToRef(Vector3CRef i_targetInRef,
		const Matrix3x3 &i_targetToRef)
{
	Graphics4x4 targetToRef(i_targetToRef, i_targetInRef);
	targetToRef.Invert(d_refToTarget);
}

/******************************************************************************/
// Information about the box of interest is expressed in some reference
// system. It may be desirable to find the resulting extents in some other
// target system. Specify a transformation to that target system.
/******************************************************************************/
void CBoxExtents::SetTargetToRef(const Cartesian4x4 &i_targetToRef)
{
	i_targetToRef.Invert(d_refToTarget);
}

/******************************************************************************/
// Information about the box of interest is expressed in some reference
// system. It may be desirable to find the resulting extents in some other
// target system. Specify a transformation to that target system.
/******************************************************************************/
void CBoxExtents::SetTargetToRef(Vector3CRef i_targetInRef,
		const DirCos3x3 &i_targetToRef)
{
	Cartesian4x4 targetToRef(i_targetToRef, i_targetInRef);
	targetToRef.Invert(d_refToTarget);
}

/******************************************************************************/
// Describe a box and find its extents in a target system.
/******************************************************************************/
void CBoxExtents::FindExtents(Vector3CRef i_halfSize,
		const Graphics4x4 &i_boxToRef, Vector3 &o_minExtent,
		Vector3 &o_maxExtent, const Graphics4x4 &i_refToTarget)
{
	Graphics4x4 norm;
	norm.SetScale(i_halfSize.X(), i_halfSize.Y(), i_halfSize.Z());
	norm *= i_boxToRef * i_refToTarget;

	Vector3 absExtent(
			Math::Abs(norm.e00) + Math::Abs(norm.e10) + Math::Abs(norm.e20),
			Math::Abs(norm.e01) + Math::Abs(norm.e11) + Math::Abs(norm.e21),
			Math::Abs(norm.e02) + Math::Abs(norm.e12) + Math::Abs(norm.e22));
	Vector3 pos = norm.GetTranslation();
	
	o_minExtent = pos - absExtent;
	o_maxExtent = pos + absExtent;
}

/******************************************************************************/
// Describe a box and find its extents in a target system.
/******************************************************************************/
void CBoxExtents::FindExtents(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
		const Matrix3x3 &i_boxToRef, Vector3 &o_minExtent, Vector3 &o_maxExtent,
		const Graphics4x4 &i_refToTarget)
{
	Matrix3x3 norm(i_halfSize);
	norm *= i_boxToRef;
	norm.TimesEqualsOrientation(i_refToTarget);

	Vector3 absExtent(
			Math::Abs(norm.e00) + Math::Abs(norm.e10) + Math::Abs(norm.e20),
			Math::Abs(norm.e01) + Math::Abs(norm.e11) + Math::Abs(norm.e21),
			Math::Abs(norm.e02) + Math::Abs(norm.e12) + Math::Abs(norm.e22));
	Vector3 pos = i_boxInRef * i_refToTarget;
	
	o_minExtent = pos - absExtent;
	o_maxExtent = pos + absExtent;
}

/******************************************************************************/
// Describe a box and find its extents in a reference system.
/******************************************************************************/
void CBoxExtents::FindExtents(Vector3CRef i_halfSize,
		const Graphics4x4 &i_boxToRef, Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	Graphics4x4 norm;
	norm.SetScale(i_halfSize.X(), i_halfSize.Y(), i_halfSize.Z());
	norm *= i_boxToRef;

	Vector3 absExtent(
			Math::Abs(norm.e00) + Math::Abs(norm.e10) + Math::Abs(norm.e20),
			Math::Abs(norm.e01) + Math::Abs(norm.e11) + Math::Abs(norm.e21),
			Math::Abs(norm.e02) + Math::Abs(norm.e12) + Math::Abs(norm.e22));
	Vector3 pos = norm.GetTranslation();
	
	o_minExtent = pos - absExtent;
	o_maxExtent = pos + absExtent;
}

/******************************************************************************/
// Describe a box and find its extents in a reference system.
/******************************************************************************/
void CBoxExtents::FindExtents(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
		const Matrix3x3 &i_boxToRef, Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	Matrix3x3 norm(i_halfSize);
	norm *= i_boxToRef;

	Vector3 absExtent(
			Math::Abs(norm.e00) + Math::Abs(norm.e10) + Math::Abs(norm.e20),
			Math::Abs(norm.e01) + Math::Abs(norm.e11) + Math::Abs(norm.e21),
			Math::Abs(norm.e02) + Math::Abs(norm.e12) + Math::Abs(norm.e22));
	
	o_minExtent = i_boxInRef - absExtent;
	o_maxExtent = i_boxInRef + absExtent;
}

/******************************************************************************/
// Describe a box and find its extents in the target system.
/******************************************************************************/
void CBoxExtents::FindExtentsInTarget(Vector3CRef i_halfSize,
		const Graphics4x4 &i_boxToRef, Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	FindExtents(i_halfSize, i_boxToRef, o_minExtent, o_maxExtent, d_refToTarget);
}

/******************************************************************************/
// Describe a box and find its extents in the target system.
/******************************************************************************/
void CBoxExtents::FindExtentsInTarget(Vector3CRef i_halfSize,
		Vector3CRef i_boxInRef, const Matrix3x3 &i_boxToRef,
		Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	FindExtents(i_halfSize, i_boxInRef, i_boxToRef, o_minExtent, o_maxExtent,
			d_refToTarget);
}

/******************************************************************************/
// Describe a box and find its extents in the reference system.
/******************************************************************************/
void CBoxExtents::FindExtentsInRef(Vector3CRef i_halfSize,
		const Graphics4x4 &i_boxToRef, Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	FindExtents(i_halfSize, i_boxToRef, o_minExtent, o_maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the reference system.
/******************************************************************************/
void CBoxExtents::FindExtentsInRef(Vector3CRef i_halfSize,
		Vector3CRef i_boxInRef, const Matrix3x3 &i_boxToRef,
		Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	FindExtents(i_halfSize, i_boxInRef, i_boxToRef, o_minExtent, o_maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the target system, return the results
// of the test and compare them to tests since the last reset to find the
// collective extents. Note that all tests must be performed in the same
// coordnate system.
/******************************************************************************/
void CBoxExtents::TestExtentsInTarget(Vector3CRef i_halfSize,
		const Graphics4x4 &i_boxToRef, Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	FindExtents(i_halfSize, i_boxToRef, o_minExtent, o_maxExtent, d_refToTarget);
	TestExtents(o_minExtent, o_maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the target system, return the results
// of the test and compare them to tests since the last reset to find the
// collective extents. Note that all tests must be performed in the same
// coordnate system.
/******************************************************************************/
void CBoxExtents::TestExtentsInTarget(Vector3CRef i_halfSize,
		Vector3CRef i_boxInRef, const Matrix3x3 &i_boxToRef,
		Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	FindExtents(i_halfSize, i_boxInRef, i_boxToRef, o_minExtent, o_maxExtent,
			d_refToTarget);
	TestExtents(o_minExtent, o_maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the target system and compare them to
// tests since the last reset to find the collective extents. Note that all
// tests must be performed in the same coordnate system.
/******************************************************************************/
void CBoxExtents::TestExtentsInTarget(Vector3CRef i_halfSize,
		const Graphics4x4 &i_boxToRef)
{
	Vector3 minExtent, maxExtent;
	FindExtents(i_halfSize, i_boxToRef, minExtent, maxExtent, d_refToTarget);
	TestExtents(minExtent, maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the target system and compare them to
// tests since the last reset to find the collective extents. Note that all
// tests must be performed in the same coordnate system.
/******************************************************************************/
void CBoxExtents::TestExtentsInTarget(Vector3CRef i_halfSize,
		Vector3CRef i_boxInRef, const Matrix3x3 &i_boxToRef)
{
	Vector3 minExtent, maxExtent;
	FindExtents(i_halfSize, i_boxInRef, i_boxToRef, minExtent, maxExtent,
			d_refToTarget);
	TestExtents(minExtent, maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the reference system, return the
// results of the test and compare them to tests since the last reset to find
// the collective extents. Note that all tests must be performed in the same
// coordnate system.
/******************************************************************************/
void CBoxExtents::TestExtentsInRef(Vector3CRef i_halfSize,
		const Graphics4x4 &i_boxToRef, Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	FindExtents(i_halfSize, i_boxToRef, o_minExtent, o_maxExtent);
	TestExtents(o_minExtent, o_maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the reference system, return the
// results of the test and compare them to tests since the last reset to find
// the collective extents. Note that all tests must be performed in the same
// coordnate system.
/******************************************************************************/
void CBoxExtents::TestExtentsInRef(Vector3CRef i_halfSize,
		Vector3CRef i_boxInRef, const Matrix3x3 &i_boxToRef,
		Vector3 &o_minExtent, Vector3 &o_maxExtent)
{
	FindExtents(i_halfSize, i_boxInRef, i_boxToRef, o_minExtent, o_maxExtent);
	TestExtents(o_minExtent, o_maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the reference system and compare them
// to tests since the last reset to find the collective extents. Note that all
// tests must be performed in the same coordnate system.
/******************************************************************************/
void CBoxExtents::TestExtentsInRef(Vector3CRef i_halfSize,
		const Graphics4x4 &i_boxToRef)
{
	Vector3 minExtent, maxExtent;
	FindExtents(i_halfSize, i_boxToRef, minExtent, maxExtent);
	TestExtents(minExtent, maxExtent);
}

/******************************************************************************/
// Describe a box and find its extents in the reference system and compare them
// to tests since the last reset to find the collective extents. Note that all
// tests must be performed in the same coordnate system.
/******************************************************************************/
void CBoxExtents::TestExtentsInRef(Vector3CRef i_halfSize,
		Vector3CRef i_boxInRef, const Matrix3x3 &i_boxToRef)
{
	Vector3 minExtent, maxExtent;
	FindExtents(i_halfSize, i_boxInRef, i_boxToRef, minExtent, maxExtent);
	TestExtents(minExtent, maxExtent);
}

/******************************************************************************/
// Actually perform the extent test.
/******************************************************************************/
void CBoxExtents::TestExtents(Vector3CRef i_min, Vector3CRef i_max)
{
	d_minExtent = d_minExtent.Min(i_min);
	d_maxExtent = d_maxExtent.Max(i_max);
}
