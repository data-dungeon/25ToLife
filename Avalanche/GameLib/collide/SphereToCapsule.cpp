#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/SphereToCapsule.h"
#include "geomUtil/LineToLine.h"

const uint CSphereToCapsuleIntersection::INTERSECTION_TEST		= 0x01;
const uint CSphereToCapsuleIntersection::CLOSEST_POINT			= 0x02;

/***************************************************************************/
/***************************************************************************/
CSphereToCapsuleIntersection::CSphereToCapsuleIntersection(
	CSphereCollisionObject &i_sphere, Capsule &i_capsule) :
		CCollisionIntersection(SPHERE_TO_CAPSULE),
		d_sphere(&i_sphere),
		d_capsule(&i_capsule)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CSphereToCapsuleIntersection::CSphereToCapsuleIntersection() :
		CCollisionIntersection(SPHERE_TO_CAPSULE),
		d_sphere(NULL),
		d_capsule(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CSphereToCapsuleIntersection::CSphereToCapsuleIntersection(
	CSphereCollisionObject &i_sphere, Capsule &i_capsule, CCollisionStack &i_stack) :
		CCollisionIntersection(SPHERE_TO_CAPSULE, i_stack),
		d_sphere(&i_sphere),
		d_capsule(&i_capsule)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CSphereToCapsuleIntersection* CSphereToCapsuleIntersection::New(
	CSphereCollisionObject &i_sphere, Capsule &i_capsule, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CSphereToCapsuleIntersection* intersection =
			(CSphereToCapsuleIntersection*)i_stack->Allocate(
			sizeof(CSphereToCapsuleIntersection));

		if (intersection)
			new(intersection) CSphereToCapsuleIntersection(i_sphere, i_capsule, *i_stack);

		return(intersection);
	}

	return(new CSphereToCapsuleIntersection(i_sphere, i_capsule));
}

/***************************************************************************/
/***************************************************************************/
CSphereToCapsuleIntersection& CSphereToCapsuleIntersection::operator=(
	CSphereToCapsuleIntersection &i_source)
{
	d_sphere = i_source.d_sphere;
	d_capsule = i_source.d_capsule;
	d_validFlags = i_source.d_validFlags;

	if ((d_validFlags & INTERSECTION_TEST) != 0)
	{
		d_intersects = i_source.d_intersects;
		d_closestPointOnCapsuleAxis = i_source.d_closestPointOnCapsuleAxis;
	}
	if( d_validFlags & CLOSEST_POINT )
	{
		d_closestPoint = i_source.d_closestPoint;
	}

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
void CSphereToCapsuleIntersection::Set(CSphereCollisionObject* i_sphere,
		Capsule* i_capsule)
{
	ASSERT((i_sphere != NULL) && (i_capsule != NULL));
	d_sphere = i_sphere;
	d_capsule = i_capsule;
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCollisionIntersection* CSphereToCapsuleIntersection::TestForIntersection(
	CSphereCollisionObject* i_sphere, Capsule* i_capsule, CCollisionStack* i_stack)
{
	ASSERT((i_sphere != NULL) && (i_capsule != NULL));
	CSphereToCapsuleIntersection* intersection = CSphereToCapsuleIntersection::New(
		*i_sphere, *i_capsule, i_stack);
	if (intersection)
	{
		if( intersection->IntersectionTest() )
			return( intersection );

		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

Vector3CRef CSphereToCapsuleIntersection::ClosestPoint(void)
{
	if( !(d_validFlags & INTERSECTION_TEST) )
		IntersectionTest();

	if( d_intersects )
	{
		if( !(d_validFlags & CLOSEST_POINT) )
		{
			d_validFlags |= CLOSEST_POINT;

			if( d_capsule->PointInsideCapsule( d_sphere->Center() ) )
			{
				d_closestPoint = d_sphere->Center();
			}
			else
			{
				Vector3 toSphere = d_sphere->Center() - d_closestPointOnCapsuleAxis;
				toSphere.SafeNormalize();
				d_closestPoint = d_closestPointOnCapsuleAxis + toSphere * d_capsule->Radius();
			}
		}
	}
	return d_closestPoint;
}

bool CSphereToCapsuleIntersection::IntersectionTest( void )
{
	if( !(d_validFlags & INTERSECTION_TEST) )
	{
		float radiusSum = d_sphere->Radius() + d_capsule->Radius();
		Vector3 p;
		float t, d2;
		
		d_validFlags |= INTERSECTION_TEST;
		d2 = PointToSegmentDistanceSquared( d_capsule->Origin(), d_capsule->Axis(), 0, d_capsule->AxisLength(),
			d_sphere->Center(), t, p );
		
		if( d2 > (radiusSum * radiusSum) )
			d_intersects = false;
		else
		{
			d_intersects = true;
			d_closestPointOnCapsuleAxis = p;
		}
	}	
	return d_intersects;
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CSphereToCapsuleIntersection::GetName(void)
{
	return("SphereToCapsule");
}
#endif
