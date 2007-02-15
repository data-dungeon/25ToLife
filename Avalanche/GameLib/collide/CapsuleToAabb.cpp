#include "collide/CollidePCH.h"
#include "CapsuleToAabb.h"

// Valid data masks.
const uint CCapsuleToAabbIntersection::SEPARATING_AXIS_TEST = 0x01;
const uint CCapsuleToAabbIntersection::RADIUS_TEST = 0x02;

CCapsuleToAabbIntersection::CCapsuleToAabbIntersection(Capsule &i_capsule, CAabbCollisionObject &i_aabb) :
	CCollisionIntersection( CAPSULE_TO_AABB ),
	d_capsule( &i_capsule ),
	d_aabb( &i_aabb )
{
	ClearTests();
}

CCapsuleToAabbIntersection::CCapsuleToAabbIntersection(Capsule &i_capsule, CAabbCollisionObject &i_aabb, CCollisionStack &i_stack) :
	CCollisionIntersection( CAPSULE_TO_AABB, i_stack ),
	d_capsule( &i_capsule ),
	d_aabb( &i_aabb )
{
	ClearTests();
}

CCapsuleToAabbIntersection* CCapsuleToAabbIntersection::New(
	Capsule &i_capsule, CAabbCollisionObject &i_aabb, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CCapsuleToAabbIntersection* intersection =
			(CCapsuleToAabbIntersection*)i_stack->Allocate(
			sizeof(CCapsuleToAabbIntersection));

		if (intersection)
			new(intersection) CCapsuleToAabbIntersection(i_capsule, i_aabb, *i_stack);

		return(intersection);
	}

	return(new CCapsuleToAabbIntersection(i_capsule, i_aabb));
}

CCollisionIntersection* CCapsuleToAabbIntersection::TestForIntersection(
	Capsule *i_capsule, CAabbCollisionObject* i_aabb, CCollisionStack* i_stack)
{
	CCapsuleToAabbIntersection* intersection = CCapsuleToAabbIntersection::New( *i_capsule,
		*i_aabb, i_stack);
	if (intersection)
	{
		if (intersection->SeparatingAxisTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

bool CCapsuleToAabbIntersection::RadiusTest( void )
{
	// use the aabb's maximum radius, compared to the capsule's maximum radius, to determine
	// if they could possibly be colliding
	if( !(d_validFlags & RADIUS_TEST) )
	{
		d_validFlags |= RADIUS_TEST;

		float aabbRadius = d_aabb->GetConstAABB().Size().Length() * 0.5f;
		float rsum = d_capsule->SphericalRadius() + aabbRadius;
		Vector3 c = d_capsule->Center() - d_aabb->GetConstAABB().Center();
		if( c.LengthSquared() > (rsum * rsum) )
			d_radiusIntersection = false;
		else
			d_radiusIntersection = true;

	}
	return d_radiusIntersection;
}

bool CCapsuleToAabbIntersection::SeparatingAxisTest( void )
{
	if( !(d_validFlags & SEPARATING_AXIS_TEST) )
	{
		if( !(d_validFlags & RADIUS_TEST) )
			RadiusTest();

		if( d_radiusIntersection )
		{
			d_validFlags |= SEPARATING_AXIS_TEST;
			Vector3CRef o = d_capsule->Origin();
			Vector3CRef e = d_capsule->End();
			float r = d_capsule->Radius();
			Vector3 min = d_aabb->GetConstAABB().GetMinV();
			Vector3 max = d_aabb->GetConstAABB().GetMaxV();

			if( o.x() + r < min.x() && e.x() + r < min.x() )
				d_intersects = false;
			else if( o.x() - r > max.x() && e.x() - r > max.x() )
				d_intersects = false;
			else if( o.y() + r < min.y() && e.y() + r < min.y() )
				d_intersects = false;
			else if( o.y() - r > max.y() && e.y() - r > max.y() )
				d_intersects = false;
			else if( o.z() + r < min.z() && e.z() + r < min.z() )
				d_intersects = false;
			else if( o.z() - r > max.z() && e.z() - r > max.z() )
				d_intersects = false;
			else
				d_intersects = true;
		}
		else
			d_intersects = false;
	}

	return d_intersects;
}