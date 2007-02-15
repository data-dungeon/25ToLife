/*
**
**  File:   SphereToObb.cpp
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/SphereToObb.cpp $
**  $Revision: 3 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/SphereToObb.h"

#if 0

// Engine includes //
#include "Math/MathClass.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

#endif

// Valid data masks.
const uint CSphereToObbIntersection::INCLUDE_RADIUS_TEST     = 0x01;
const uint CSphereToObbIntersection::EXCLUDE_RADIUS_TEST     = 0x02;
const uint CSphereToObbIntersection::SEPARATING_AXIS_TEST    = 0x04;

/***************************************************************************/
// If the collision isn't trivally rejected, do the separating
// axis test.  This is reasonably fast if they objects don't
// collide.  If this test passes the objects are touching.
/***************************************************************************/
bool CSphereToObbIntersection::SeparatingAxisTest(void)
{
	if ((d_validFlags & SEPARATING_AXIS_TEST) != 0)
		return(d_intersects);
	d_validFlags |= SEPARATING_AXIS_TEST;

	IncludeRadiusTest();
	Vector3 sphereInObb = d_sphereFromObb / *(d_obb->Matrix());
	Vector3 absSphereInObb = sphereInObb.Abs();

	const Vector3 &extent = d_obb->GetConstAABB().maxV;

	d_centerInside = false;
	if (absSphereInObb.x() > extent.x())
	{
		if (absSphereInObb.y() > extent.y())
		{
/*
 *			Closest to a corner.
 */
			if (absSphereInObb.z() > extent.z())
				d_closestPoint = extent;
/*
 *			Closest to a z-edge.
 */
			else
				d_closestPoint.Set(extent.x(), extent.y(), absSphereInObb.z());
		}
		else
		{
/*
 *			Closest to a y-edge.
 */
			if (absSphereInObb.z() > extent.z())
				d_closestPoint.Set(extent.x(), absSphereInObb.y(), extent.z());
/*
 *			Closest to a x-face.
 */
			else
				d_closestPoint.Set(extent.x(), absSphereInObb.y(), absSphereInObb.z());
		}
	}
	else
	{
		if (absSphereInObb.y() > extent.y())
		{
/*
 *			Closest to a x-edge.
 */
			if (absSphereInObb.z() > extent.z())
				d_closestPoint.Set(absSphereInObb.x(), extent.y(), extent.z());
/*
 *			Closest to a y-face.
 */
			else
				d_closestPoint.Set(absSphereInObb.x(), extent.y(), absSphereInObb.z());
		}
		else
		{
/*
 *			Closest to a z-face.
 */
			if (absSphereInObb.z() > extent.z())
				d_closestPoint.Set(absSphereInObb.x(), absSphereInObb.y(), extent.z());
/*
 *			Inside the obb.
 */
			else
			{
				d_closestPoint = absSphereInObb;
				d_centerInside = true;
			}
		}
	}

	d_intersects = ((absSphereInObb - d_closestPoint).LengthSquared() <=
			d_sphere->RadiusSquared());

	if (d_intersects)
	{
		if (sphereInObb.x() < 0.0f)
			d_closestPoint.x( -d_closestPoint.x());
		if (sphereInObb.y() < 0.0f)
			d_closestPoint.y( -d_closestPoint.y());
		if (sphereInObb.z() < 0.0f)
			d_closestPoint.z( -d_closestPoint.z());
		d_closestPoint = d_closestPoint * *(d_obb->Matrix()) + *(d_obb->Center());
	}

	return(d_intersects);
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CSphereToObbIntersection::GetName(void)
{
	return("SphereToObb");
}
#endif
