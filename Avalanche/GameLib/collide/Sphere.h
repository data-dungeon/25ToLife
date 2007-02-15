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
**      $File: //depot/Avalanche/GameLib/collide/Sphere.h $
**  $Revision: #8 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef SPHERE_H
#define SPHERE_H

#include "collide/Object.h"
#include "Math/Vector.h"
#include "Math/MathClass.h"
#include "EngineHelper/drawutil.h"

class AABB;

class CSphereCollisionObject : public CCollisionObject
{
  public:
	// Constructors
	CSphereCollisionObject();
	CSphereCollisionObject(Vector3CRef i_center, float i_radius);

	// Sets
	void Set(Vector3CRef i_center, float i_radius);
	void SetCenter(Vector3CRef i_center);
	void SetRadius(float i_radius);
	void SetCenterTest(bool i_centerTest);

	// Gets
	Vector3 &Center(void) {return(d_center);}
	Vector3CRef ConstCenter(void) const {return(d_center);}
	float Radius(void) const {return(d_radius);}
	float RadiusSquared(void) const {return(d_radiusSquared);}
	bool CenterTest(void) const {return(d_centerTest);}

	// Find world extents.
	void ComputeWorldExtents(AABB &o_worldAABB) const;

	// Triangle intersections
	bool TestTriangle(Vector3CRef i_normal, Vector3CRef i_v0,
			Vector3CRef i_v1, Vector3CRef i_v2) const;

	void Draw(DrawUtility::COLOR i_color = DrawUtility::RED) const;

#ifdef GCN
	inline void EndianSwap( void)
	{
		CCollisionObject::EndianSwap();
		d_center.EndianSwap();
		ByteSwap(d_radius);
		ByteSwap(d_radiusSquared);
	}
#endif

  private:
	Vector3 d_center;
	float d_radius;
	float d_radiusSquared;
	bool d_centerTest;
};

#define SPHERE_HPP
#include "collide/Sphere.hpp"

#endif // SPHERE_H
