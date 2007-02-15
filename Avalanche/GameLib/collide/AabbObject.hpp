/*
**
**  File:   AabbObject.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   An aabb collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbObject.hpp $
**  $Revision: 4 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef AABB_OBJECT_HPP
#undef AABB_OBJECT_HPP

// Engine includes //
#include "Math/MathClass.h"

/***************************************************************************/
/***************************************************************************/
inline bool CAabbCollisionObject::TestTriangle(Vector3CRef i_normal,
		Vector3CRef i_v0,Vector3CRef i_v1, Vector3CRef i_v2) const
{
	return d_aabb.TriangleTouches(i_normal, i_v0, i_v1, i_v2);
}

inline void CAabbCollisionObject::Draw(
			   DrawUtility::COLOR color) const
{
	Vector3 min = GetConstAABB().GetMinV();
	Vector3 max = GetConstAABB().GetMaxV();

	DrawUtility::Line( Vector3( min.x(), min.y(), min.z() ), Vector3( max.x(), min.y(), min.z() ), color );
	DrawUtility::Line( Vector3( min.x(), min.y(), min.z() ), Vector3( min.x(), max.y(), min.z() ), color );
	DrawUtility::Line( Vector3( min.x(), min.y(), min.z() ), Vector3( min.x(), min.y(), max.z() ), color );
	DrawUtility::Line( Vector3( min.x(), max.y(), min.z() ), Vector3( max.x(), max.y(), min.z() ), color );
	DrawUtility::Line( Vector3( min.x(), max.y(), min.z() ), Vector3( min.x(), max.y(), max.z() ), color );
	DrawUtility::Line( Vector3( min.x(), min.y(), max.z() ), Vector3( max.x(), min.y(), max.z() ), color );
	DrawUtility::Line( Vector3( min.x(), min.y(), max.z() ), Vector3( min.x(), max.y(), max.z() ), color );
	DrawUtility::Line( Vector3( max.x(), min.y(), min.z() ), Vector3( max.x(), max.y(), min.z() ), color );
	DrawUtility::Line( Vector3( max.x(), min.y(), min.z() ), Vector3( max.x(), min.y(), max.z() ), color );
	DrawUtility::Line( Vector3( max.x(), max.y(), max.z() ), Vector3( min.x(), max.y(), max.z() ), color );
	DrawUtility::Line( Vector3( max.x(), max.y(), max.z() ), Vector3( max.x(), min.y(), max.z() ), color );
	DrawUtility::Line( Vector3( max.x(), max.y(), max.z() ), Vector3( max.x(), max.y(), min.z() ), color );
}

#endif // AABB_OBJECT_HPP
