/*
**
**  File:   AabbToObb.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbToObb.cpp $
**  $Revision: 3 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/AabbToObb.h"

#if 0

// Engine includes //
#include "Math/MathClass.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

#endif

// Valid data masks.
const uint CAabbToObbIntersection::SEPARATING_AXIS_TEST = 0x01;

/***************************************************************************/
// If the collision isn't trivally rejected, do the separating
// axis test.  This is reasonably fast if they objects don't
// collide.  If this test passes the boxes are touching.
/***************************************************************************/
bool CAabbToObbIntersection::SeparatingAxisTest(void)
{
	ASSERT((d_aabb != NULL) && (d_obb != NULL));

	if ((d_validFlags & SEPARATING_AXIS_TEST) != 0)
		return(d_noSeparatingPlane);
	d_validFlags |= SEPARATING_AXIS_TEST;
	d_noSeparatingPlane = false;

	Vector3 aabbExtents = 0.5f * (d_aabb->GetConstAABB().maxV -
			d_aabb->GetConstAABB().minV);
	Vector3CRef obbExtents = d_obb->GetConstAABB().maxV;
	const DirCos3x3 &c = *(d_obb->Matrix());

	// compute difference of box centers, D = C1-C0
	Vector3 d = *(d_obb->Center()) - 0.5f * (d_aabb->GetConstAABB().maxV +
			d_aabb->GetConstAABB().minV);

	Matrix3x3 absC;		// |c_{ij}|

	// AABB x-axis
	absC.e00 = Math::Abs(c.e00);
	absC.e10 = Math::Abs(c.e10);
	absC.e20 = Math::Abs(c.e20);
	if ((aabbExtents.x() + obbExtents.x() * absC.e00 + obbExtents.y() * absC.e10 +
			obbExtents.z() * absC.e20) < Math::Abs(d.x()))
		return(false);

	// AABB y-axis
	absC.e01 = Math::Abs(c.e01);
	absC.e11 = Math::Abs(c.e11);
	absC.e21 = Math::Abs(c.e21);
	if ((aabbExtents.y() + obbExtents.x() * absC.e01 + obbExtents.y() * absC.e11 +
			obbExtents.z() * absC.e21) < Math::Abs(d.y()))
		return(false);

	// AABB z-axis
	absC.e02 = Math::Abs(c.e02);
	absC.e12 = Math::Abs(c.e12);
	absC.e22 = Math::Abs(c.e22);
	if ((aabbExtents.z() + obbExtents.x() * absC.e02 + obbExtents.y() * absC.e12 +
			obbExtents.z() * absC.e22) < Math::Abs(d.z()))
		return(false);

	// OBB x-axis
	if ((aabbExtents.x() * absC.e00 + aabbExtents.y() * absC.e01 +
			aabbExtents.z() * absC.e02 + obbExtents.x()) <
			Math::Abs(d.x() * c.e00 + d.y() * c.e01 + d.z() * c.e02))
		return(false);

	// OBB y-axis
	if ((aabbExtents.x() * absC.e10 + aabbExtents.y() * absC.e11 +
			aabbExtents.z() * absC.e12 + obbExtents.y()) <
			Math::Abs(d.x() * c.e10 + d.y() * c.e11 + d.z() * c.e12))
		return(false);

	// OBB z-axis
	if ((aabbExtents.x() * absC.e20 + aabbExtents.y() * absC.e21 +
			aabbExtents.z() * absC.e22 + obbExtents.z()) <
			Math::Abs(d.x() * c.e20 + d.y() * c.e21 + d.z() * c.e22))
		return(false);

	// AABB x-axis cross OBB x-axis
	if ((aabbExtents.y() * absC.e02 + aabbExtents.z() * absC.e01 +
			obbExtents.y() * absC.e20 + obbExtents.z() * absC.e10) <
			Math::Abs(d.z() * c.e01 - d.y() * c.e02))
		return(false);

	// AABB x-axis cross OBB y-axis
	if ((aabbExtents.y() * absC.e12 + aabbExtents.z() * absC.e11 +
			obbExtents.x() * absC.e20 + obbExtents.z() * absC.e00) <
			Math::Abs(d.z() * c.e11 - d.y() * c.e12))
		return(false);

	// AABB x-axis cross OBB z-axis
	if ((aabbExtents.y() * absC.e22 + aabbExtents.z() * absC.e21 +
			obbExtents.x() * absC.e10 + obbExtents.y() * absC.e00) <
			Math::Abs(d.z() * c.e21 - d.y() * c.e22))
		return(false);

	// AABB y-axis cross OBB x-axis
	if ((aabbExtents.x() * absC.e02 + aabbExtents.z() * absC.e00 +
			obbExtents.y() * absC.e21 + obbExtents.z() * absC.e11) <
			Math::Abs(d.x() * c.e02 - d.z() * c.e00))
		return(false);

	// AABB y-axis cross OBB y-axis
	if ((aabbExtents.x() * absC.e12 + aabbExtents.z() * absC.e10 +
			obbExtents.x() * absC.e21 + obbExtents.z() * absC.e01) <
			Math::Abs(d.x() * c.e12 - d.z() * c.e10))
		return(false);

	// AABB y-axis cross OBB z-axis
	if ((aabbExtents.x() * absC.e22 + aabbExtents.z() * absC.e20 +
			obbExtents.x() * absC.e11 + obbExtents.y() * absC.e01) <
			Math::Abs(d.x() * c.e22 - d.z() * c.e20))
		return(false);

	// AABB z-axis cross OBB x-axis
	if ((aabbExtents.x() * absC.e01 + aabbExtents.y() * absC.e00 +
			obbExtents.y() * absC.e22 + obbExtents.z() * absC.e12) <
			Math::Abs(d.y() * c.e00 - d.x() * c.e01))
		return(false);

	// AABB z-axis cross OBB y-axis
	if ((aabbExtents.x() * absC.e11 + aabbExtents.y() * absC.e10 +
			obbExtents.x() * absC.e22 + obbExtents.z() * absC.e02) <
			Math::Abs(d.y() * c.e10 - d.x() * c.e11))
		return(false);

	// AABB z-axis cross OBB z-axis
	if ((aabbExtents.x() * absC.e21 + aabbExtents.y() * absC.e20 +
			obbExtents.x() * absC.e12 + obbExtents.y() * absC.e02) <
			Math::Abs(d.y() * c.e20 - d.x() * c.e21))
		return(false);

	d_noSeparatingPlane = true;

	return(true);
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CAabbToObbIntersection::GetName(void)
{
	return("AabbToObb");
}
#endif
