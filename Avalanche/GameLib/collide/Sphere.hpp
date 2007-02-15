/*
**
**  File:   Sphere.h
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   An sphere collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Sphere.hpp $
**  $Revision: 4 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef SPHERE_HPP
#undef SPHERE_HPP

// Engine includes //
#include "Math/MathClass.h"

// Gamelib includes //
#include "collide/aabb.h"

/***************************************************************************/
/***************************************************************************/
inline void CSphereCollisionObject::SetRadius(float i_radius)
{
	d_radius = Math::Abs(i_radius);
	d_radiusSquared = d_radius * d_radius;
}

/***************************************************************************/
/***************************************************************************/
inline CSphereCollisionObject::CSphereCollisionObject() :
	CCollisionObject(SPHERE)
{
	d_center.Clear();
	d_radius = 0.0f;
	d_radiusSquared = 0.0f;
	d_centerTest = false;
}

/***************************************************************************/
/***************************************************************************/
inline CSphereCollisionObject::CSphereCollisionObject(Vector3CRef i_center,
		float i_radius) :
	CCollisionObject(SPHERE)
{
	d_center = i_center;
	SetRadius(i_radius);
	d_centerTest = false;
}

/***************************************************************************/
/***************************************************************************/
inline void CSphereCollisionObject::Set(Vector3CRef i_center, float i_radius)
{
	d_center = i_center;
	SetRadius(i_radius);
}

/***************************************************************************/
/***************************************************************************/
inline void CSphereCollisionObject::SetCenter(Vector3CRef i_center)
{
	d_center = i_center;
}

/***************************************************************************/
/***************************************************************************/
inline void CSphereCollisionObject::SetCenterTest(bool i_centerTest)
{
	d_centerTest = i_centerTest;
}

/***************************************************************************/
/***************************************************************************/
inline void CSphereCollisionObject::ComputeWorldExtents(AABB &o_worldAABB) const
{
	o_worldAABB.FromSphere(d_center, d_radius);
}

/***************************************************************************/
/***************************************************************************/
inline bool CSphereCollisionObject::TestTriangle(Vector3CRef i_normal,
		Vector3CRef i_v0,Vector3CRef i_v1, Vector3CRef i_v2) const
{
/*
 *	See if the distance from the triangle plane to the center of the sphere
 *	is greater than the radius of the sphere.
 */
	Vector3 v0InSphere = i_v0 - d_center;
	float height = Math::Abs(i_normal.Dot(v0InSphere));
	if (height > d_radius)
		return(false);
/*
 *	Now test the intersection circle against the edges of the triangle. Note:
 *	this assumes that the triangles are right-handed.
 */
	Vector3 v1InSphere = i_v1 - d_center;
	Vector3 edge = v1InSphere - v0InSphere;
	float centerDistTerm = v0InSphere.Dot(i_normal.Cross(edge));
	if (centerDistTerm > 0.0f)
	{
		float radiusSq = d_radius * d_radius;
		if (((v0InSphere.Dot(edge) >= 0.0f) &&
					(v0InSphere.LengthSquared() > radiusSq)) ||
				((v1InSphere.Dot(edge) <= 0.0f) &&
					(v1InSphere.LengthSquared() > radiusSq)) ||
				((centerDistTerm * centerDistTerm) >
					(edge.LengthSquared() * (radiusSq - height * height))))
			return(false);
		return(true);
	}

	Vector3 v2InSphere = i_v2 - d_center;
	edge = v2InSphere - v1InSphere;
	centerDistTerm = v1InSphere.Dot(i_normal.Cross(edge));
	if (centerDistTerm > 0.0f)
	{
		float radiusSq = d_radius * d_radius;
		if (((v1InSphere.Dot(edge) >= 0.0f) &&
					(v1InSphere.LengthSquared() > radiusSq)) ||
				((v2InSphere.Dot(edge) <= 0.0f) &&
					(v2InSphere.LengthSquared() > radiusSq)) ||
				((centerDistTerm * centerDistTerm) >
					(edge.LengthSquared() * (radiusSq - height * height))))
			return(false);
		return(true);
	}

	edge = v0InSphere - v2InSphere;
	centerDistTerm = v2InSphere.Dot(i_normal.Cross(edge));
	if (centerDistTerm > 0.0f)
	{
		float radiusSq = d_radius * d_radius;
		if (((v2InSphere.Dot(edge) >= 0.0f) &&
					(v2InSphere.LengthSquared() > radiusSq)) ||
				((v0InSphere.Dot(edge) <= 0.0f) &&
					(v0InSphere.LengthSquared() > radiusSq)) ||
				((centerDistTerm * centerDistTerm) >
					(edge.LengthSquared() * (radiusSq - height * height))))
			return(false);
	}

	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline void CSphereCollisionObject::Draw(DrawUtility::COLOR i_color) const
{
	DrawUtility::Sphere(d_center, i_color, d_radius);
}

#endif // SPHERE_HPP
