/*
**
**  File:   SphereToObb.hpp
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/SphereToObb.hpp $
**  $Revision: 1 $
**      $Date: 5/05/03 3:12p $
**    $Author: Bcollard $
**
*/

#ifdef SPHERE_TO_OBB_HPP
#undef SPHERE_TO_OBB_HPP

// System includes //
#include <new.h>

// Engine includes //
#include "Layers/Memory.h"
#include "Layers/Assert/Assert.h"

// Gamelib includes //
#include "collide/Stack.h"

/***************************************************************************/
/***************************************************************************/
inline CSphereToObbIntersection::CSphereToObbIntersection(
		CSphereCollisionObject &i_sphere, OBB &i_obb) :
	CCollisionIntersection(SPHERE_TO_OBB),
	d_sphere(&i_sphere),
	d_obb(&i_obb)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CSphereToObbIntersection::CSphereToObbIntersection() :
	CCollisionIntersection(SPHERE_TO_OBB),
	d_sphere(NULL),
	d_obb(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CSphereToObbIntersection::CSphereToObbIntersection(
		CSphereCollisionObject &i_sphere, OBB &i_obb, CCollisionStack &i_stack) :
	CCollisionIntersection(SPHERE_TO_OBB, i_stack),
	d_sphere(&i_sphere),
	d_obb(&i_obb)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
inline CSphereToObbIntersection* CSphereToObbIntersection::New(
		CSphereCollisionObject &i_sphere, OBB &i_obb, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CSphereToObbIntersection* intersection =
				(CSphereToObbIntersection*)i_stack->Allocate(
				sizeof(CSphereToObbIntersection));

		if (intersection)
			new(intersection) CSphereToObbIntersection(i_sphere, i_obb, *i_stack);

		return(intersection);
	}

	return(new CSphereToObbIntersection(i_sphere, i_obb));
}

/***************************************************************************/
/***************************************************************************/
inline CSphereToObbIntersection& CSphereToObbIntersection::operator=(
		CSphereToObbIntersection &i_source)
{
	d_sphere = i_source.d_sphere;
	d_obb = i_source.d_obb;
	d_validFlags = i_source.d_validFlags;

	if ((d_validFlags & INCLUDE_RADIUS_TEST) != 0)
	{
		d_obbCenterIsInSphere = i_source.d_obbCenterIsInSphere;
		d_sphereFromObb = i_source.d_sphereFromObb;
		d_centerDistanceSquared = i_source.d_centerDistanceSquared;
	}

	if ((d_validFlags & EXCLUDE_RADIUS_TEST) != 0)
		d_spheresIntersect = i_source.d_spheresIntersect;

	if ((d_validFlags & SEPARATING_AXIS_TEST) != 0)
	{
		d_intersects = i_source.d_intersects;
		d_centerInside = i_source.d_centerInside;
		d_closestPoint = i_source.d_closestPoint;
	}

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline void CSphereToObbIntersection::Set(CSphereCollisionObject* i_sphere,
		OBB* i_obb)
{
	ASSERT((i_sphere != NULL) && (i_obb != NULL));
	d_sphere = i_sphere;
	d_obb = i_obb;
	ClearTests();
}

/***************************************************************************/
// Return true if the collision could occur. While fast, a false result does
// not guarantee an non-intersection.
/***************************************************************************/
inline bool CSphereToObbIntersection::IncludeRadiusTest(void)
{
	ASSERT((d_sphere != NULL) && (d_obb != NULL));
	if ((d_validFlags & INCLUDE_RADIUS_TEST) == 0)
	{
		d_sphereFromObb = d_sphere->Center() - *(d_obb->Center());
		d_centerDistanceSquared = d_sphereFromObb.LengthSquared();
		d_obbCenterIsInSphere =
				(d_centerDistanceSquared < d_sphere->RadiusSquared());
		d_validFlags |= INCLUDE_RADIUS_TEST;
	}
	return(d_obbCenterIsInSphere);
}

/***************************************************************************/
// Return true if the collision could occur. While fast, a true result does
// not guarantee an intersection.
/***************************************************************************/
inline bool CSphereToObbIntersection::ExcludeRadiusTest(void)
{
	if ((d_validFlags & EXCLUDE_RADIUS_TEST) == 0)
	{
		d_spheresIntersect = IncludeRadiusTest();
		if (!d_spheresIntersect)
		{
			float radiusSum = d_sphere->Radius() + d_obb->Radius();
			d_spheresIntersect =
					(d_centerDistanceSquared < (radiusSum * radiusSum));
		}
		d_validFlags |= EXCLUDE_RADIUS_TEST;
	}
	return(d_spheresIntersect);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CSphereToObbIntersection::TestForIntersection(
		CSphereCollisionObject* i_sphere, OBB* i_obb, CCollisionStack* i_stack)
{
	ASSERT((i_sphere != NULL) && (i_obb != NULL));
	CSphereToObbIntersection* intersection = CSphereToObbIntersection::New(
			*i_sphere, *i_obb, i_stack);
	if (intersection)
	{
		if (intersection->IncludeRadiusTest() || (!i_sphere->CenterTest() &&
				intersection->ExcludeRadiusTest() &&
				intersection->SeparatingAxisTest()))
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline const Vector3 &CSphereToObbIntersection::ClosestPoint(void)
{
	SeparatingAxisTest();
	return(d_closestPoint);
}

/***************************************************************************/
/***************************************************************************/
inline bool CSphereToObbIntersection::CenterInside(void)
{
	SeparatingAxisTest();
	return(d_centerInside);
}

#endif // SPHERE_TO_OBB_HPP
