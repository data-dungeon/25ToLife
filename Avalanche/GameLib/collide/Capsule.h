#ifndef __CAPSULE_H_INCLUDED__
#define __CAPSULE_H_INCLUDED__

#include "collide/Object.h"
#include "collide/aabb.h"
#include "EngineHelper/drawutil.h"


class Capsule : public CCollisionObject
{
public:
	Capsule() : CCollisionObject( CAPSULE ) {};

	Vector3CRef Origin( void ) { return origin; }
	Vector3CRef End( void ) { return end; }
	Vector3CRef Axis( void ) { return axis; }
	float AxisLength( void ) { return axisLength; }
	DirCos3x3 Orientation( void );

	float Radius( void ) { return radius; }

	inline void SetExtents( Vector3CRef i_origin, Vector3CRef i_end, float i_radius ) { origin = i_origin; end = i_end; radius = i_radius; Recompute(); }
	void Recompute( void );

	inline void ComputeWorldExtents(XZRect &o_worldRect) const;
	inline void ComputeWorldExtents(AABB &o_worldAABB) const;

	Vector3 SurfaceNormalAtPoint( Vector3CRef p );
	bool PointInsideCapsule( Vector3CRef p );

	float SphericalRadius( void ) { return sphericalRadius; }
	Vector3CRef Center( void ) { return center; }

	// Debugging
#ifndef CONSUMER_BUILD
	void QueueDraw(DrawUtility::COLOR color = DrawUtility::RED, float lifetime = 0.0f);
	void Draw(DrawUtility::COLOR color = DrawUtility::RED);
#endif

private:

	Vector3 origin;
	Vector3 end;
	Vector3 axis;
	float axisLength;
	float radius;
	DirCos3x3 orientation;
	bool orientationValid;
	
	// these are used for quick rejection tests.  The center and totalRadius define
	// a sphere that totally encompasses the capsule
	Vector3 center;
	float sphericalRadius;
};

// Include the implementation
#include "collide/Capsule.hpp"

#endif