#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/Cone.h"

/***************************************************************************/
/***************************************************************************/
void ConeCollisionObject::Recompute( void )
{
	if( Math::Zero( radius ) )
	{
		ASSERTS( 0, "Invalid cone radius.  If you really want a cone with a zero radius at the end, use a line query instead" );
		radius = 0.001f;
	}

	// recompute the axis and axis length based on the origin and end
	axis = end - origin;
	axisLength = axis.Length();
	if( Math::Zero( axisLength ) )
	{
		ASSERTS( 0, "Invalid cone.  An axis must be specified.  Picking one at random so the math doesn't blow up" );
		axis.Set( 0.0f, 0.0f, 1.0f );
		axisLength = 1.0f;
	}
	else
		axis = axis / axisLength;

	float h = Math::Sqrt( axisLength * axisLength + radius * radius );
	float s = radius / h;
	sinSqr = s * s;
	sinRecip = 1.0f / s;
	float c = axisLength / h;
	cosSqr = c * c;
}

#ifndef CONSUMER_BUILD
/***************************************************************************/
/***************************************************************************/
void ConeCollisionObject::Draw( DrawUtility::COLOR color )
{
	DrawUtility::Cone( origin, end, radius, color );
}

/***************************************************************************/
/***************************************************************************/
void ConeCollisionObject::QueueDraw( DrawUtility::COLOR color, float lifetime )
{
	DrawUtility::QueueCone( origin, end, radius, color, lifetime );
}
#endif