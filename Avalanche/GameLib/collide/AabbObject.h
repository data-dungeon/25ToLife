/*
**
**  File:   AabbObject.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   An aabb collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibNew/collide/AabbObject.h $
**  $Revision: 4 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifndef AABB_OBJECT_H
#define AABB_OBJECT_H

#include "collide/Object.h"
#include "collide/aabb.h"
#include "Math/Vector.h"

class CAabbCollisionObject : public CCollisionObject
{
  public:
	// Constructors
	CAabbCollisionObject() : CCollisionObject(AABB_OBJECT) {}
	CAabbCollisionObject(const AABB* i_aabb) : CCollisionObject(AABB_OBJECT)
		{d_aabb = *i_aabb;}

	// Get at the aabb
	AABB &GetAABB(void) {return(d_aabb);}
	const AABB &GetConstAABB(void) const {return(d_aabb);}

	// Triangle intersections
	bool TestTriangle(Vector3CRef i_normal, Vector3CRef i_v0,
			Vector3CRef i_v1, Vector3CRef i_v2) const;
	void Draw(DrawUtility::COLOR color) const;

#ifdef GCN
	inline void EndianSwap( void)
	{
		CCollisionObject::EndianSwap();
		d_aabb.EndianSwap();
	}
#endif

  private:
	AABB d_aabb;

};

#define AABB_OBJECT_HPP
#include "collide/AabbObject.hpp"

#endif // AABBOBJECT_H
