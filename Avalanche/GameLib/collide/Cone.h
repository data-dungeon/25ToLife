#ifndef __CONE_H_INCLUDED__
#define __CONE_H_INCLUDED__

#include "collide/Object.h"
#include "collide/aabb.h"
#include "EngineHelper/drawutil.h"


class ConeCollisionObject : public CCollisionObject
{
public:
	ConeCollisionObject() : CCollisionObject( CONE ) {};

	Vector3CRef Origin( void ) { return origin; }
	Vector3CRef End( void ) { return end; }
	Vector3CRef Axis( void ) { return axis; }
	float AxisLength( void ) { return axisLength; }

	float Radius( void ) { return radius; }

	// the radius you pass in here is the radius of the cone at the end.
	inline void SetExtents( Vector3CRef i_origin, Vector3CRef i_end, float i_radius ) { origin = i_origin; end = i_end; radius = i_radius; Recompute(); }
	void Recompute( void );

	// Debugging
#ifndef CONSUMER_BUILD
	void QueueDraw(DrawUtility::COLOR color = DrawUtility::RED, float lifetime = 0.0f);
	void Draw(DrawUtility::COLOR color = DrawUtility::RED);
#endif

	float SinReciprocal( void ) { return sinRecip; }
	float CosSquared( void ) { return cosSqr; }
	float SinSquared( void ) { return sinSqr; }

private:

	Vector3 origin;
	Vector3 end;
	Vector3 axis;
	float axisLength;
	float radius;

	// precomputed values that are used in the intersection tests
	float sinRecip;
	float cosSqr;
	float sinSqr;
};

#endif