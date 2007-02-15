#include "collide/xzrect.h"

/***************************************************************************/
/***************************************************************************/

inline void Capsule::ComputeWorldExtents(XZRect &o_worldRect) const
{
	if( origin.X() < end.X() )
	{
		o_worldRect.MinX( origin.X() - radius );
		o_worldRect.MaxX( end.X() + radius );
	}
	else
	{
		o_worldRect.MinX( end.X() - radius );
		o_worldRect.MaxX( origin.X() + radius );
	}
	if( origin.Z() < end.Z() )
	{
		o_worldRect.MinZ( origin.Z() - radius );
		o_worldRect.MaxZ( end.Z() + radius );
	}
	else
	{
		o_worldRect.MinZ( end.Z() - radius );
		o_worldRect.MaxZ( origin.Z() + radius );
	}
}

/***************************************************************************/
/***************************************************************************/

inline void Capsule::ComputeWorldExtents(AABB &o_worldAABB) const
{
	Vector3 min, max;

	if( origin.X() < end.X() )
	{
		min.X( origin.X() - radius );
		max.X( end.X() + radius );
	}
	else
	{
		min.X( end.X() - radius );
		max.X( origin.X() + radius );
	}
	if( origin.Y() < end.Y() )
	{
		min.Y( origin.Y() - radius );
		max.Y( end.Y() + radius );
	}
	else
	{
		min.Y( end.Y() - radius );
		max.Y( origin.Y() + radius );
	}
	if( origin.Z() < end.Z() )
	{
		min.Z( origin.Z() - radius );
		max.Z( end.Z() + radius );
	}
	else
	{
		min.Z( end.Z() - radius );
		max.Z( origin.Z() + radius );
	}
	o_worldAABB.Set( min, max );
}
