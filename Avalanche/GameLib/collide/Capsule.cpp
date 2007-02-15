#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/Capsule.h"

/***************************************************************************/
/***************************************************************************/
DirCos3x3 Capsule::Orientation( void )
{
	if( !orientationValid )
	{
		// determine an orientation matrix, where Y is along the capsules, axis, and
		// x-z are arbitrary vectors lying in the plane 
		Vector3 x, z;

		// figure out a vector to use as a reference for a cross product.  any vector not pointing directly
		// the same direction as the cylinder's axis will work.
		// if x is greater than .1, then we have at least some x component, so straight y is safe.
		// if x is less than .1, then we have some y OR some z component, so straight x is safe.
		if( axis.x() > 0.1f )
			x.Set( 0.0f, 1.0f, 0.0f );
		else
			x.Set( 1.0f, 0.0f, 0.0f );

		z = x.Cross( axis );
		z.SafeNormalize();
		x = axis.Cross( z );
		x.SafeNormalize();
		orientation.SetRow0( x );
		orientation.SetRow1( axis );
		orientation.SetRow2( z );
	}
	return orientation;
}

/***************************************************************************/
/***************************************************************************/
void Capsule::Recompute( void )
{
	// recompute the axis and axis length based on the origin and end
	axis = end - origin;
	axisLength = axis.Length();
	if( !Math::Zero( axisLength ) )
		axis = axis / axisLength;

	center = (end + origin) * 0.5f;
	sphericalRadius = (axisLength * 0.5f) + (radius * 2.0f);

	orientationValid = false;
}

#ifndef CONSUMER_BUILD
/***************************************************************************/
/***************************************************************************/
void Capsule::Draw( DrawUtility::COLOR color )
{
	DrawUtility::Lozenge( origin, end, radius, color );
	DrawUtility::Line( origin, end, DrawUtility::YELLOW );
}

/***************************************************************************/
/***************************************************************************/
void Capsule::QueueDraw( DrawUtility::COLOR color, float lifetime )
{
	DrawUtility::QueueLozenge( origin, end, radius, color, lifetime );
}
#endif

/***************************************************************************/
/***************************************************************************/
Vector3 Capsule::SurfaceNormalAtPoint( Vector3CRef p )
{
	// the normal is the vector from the closest point along the capsule's axis,
	// to the specified point.  If that point is off one of the ends, then use
	// that endpoint instead

	Vector3 projected = origin + (axis * ((p-origin).Dot(axis)));
	float len = projected.LengthSquared();
	if( len < 0.0f ) // off the end from the origin
		projected = origin;
	else if( len > axisLength ) // off the end from the End
		projected = end;

	Vector3 normal = p - projected;
	if( !(normal.SafeNormalize()) ) // point was exactly on the axis
	{
		normal = axis;
		normal.SafeNormalize();
	}
	return normal;
}

/***************************************************************************/
/***************************************************************************/
bool Capsule::PointInsideCapsule( Vector3CRef p )
{
	float s = (p - origin).Dot( axis );
	float d;

	if( s < 0.0f )
	{
		// distance to the origin
		d = (p - origin).LengthSquared();
	}
	else if( s > axisLength )
	{
		// distance to the end sphere
		d = (p - end).LengthSquared();
	}
	else
	{
		// distance to the cylinder axis
		d = ( p - (origin + s * axis) ).LengthSquared();
	}

	if( d > (radius * radius) )
		return false;
	
	return true;
}
