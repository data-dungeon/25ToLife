#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/ConeToCapsule.h"
#include "geomUtil/LineToLine.h"

const uint CConeToCapsuleIntersection::INTERSECTION_TEST		= 0x01;

/***************************************************************************/
/***************************************************************************/
CConeToCapsuleIntersection::CConeToCapsuleIntersection(
	ConeCollisionObject &i_cone, Capsule &i_capsule) :
CCollisionIntersection(CONE_TO_CAPSULE),
d_cone(&i_cone),
d_capsule(&i_capsule)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CConeToCapsuleIntersection::CConeToCapsuleIntersection() :
CCollisionIntersection(CONE_TO_CAPSULE),
d_cone(NULL),
d_capsule(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CConeToCapsuleIntersection::CConeToCapsuleIntersection(
	ConeCollisionObject &i_cone, Capsule &i_capsule, CCollisionStack &i_stack) :
CCollisionIntersection(CONE_TO_CAPSULE, i_stack),
d_cone(&i_cone),
d_capsule(&i_capsule)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CConeToCapsuleIntersection* CConeToCapsuleIntersection::New(
	ConeCollisionObject &i_cone, Capsule &i_capsule, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CConeToCapsuleIntersection* intersection =
			(CConeToCapsuleIntersection*)i_stack->Allocate(
			sizeof(CConeToCapsuleIntersection));

		if (intersection)
			new(intersection) CConeToCapsuleIntersection(i_cone, i_capsule, *i_stack);

		return(intersection);
	}

	return(new CConeToCapsuleIntersection(i_cone, i_capsule));
}

/***************************************************************************/
/***************************************************************************/
CConeToCapsuleIntersection& CConeToCapsuleIntersection::operator=(
	CConeToCapsuleIntersection &i_source)
{
	d_cone = i_source.d_cone;
	d_capsule = i_source.d_capsule;
	d_validFlags = i_source.d_validFlags;

	if ((d_validFlags & INTERSECTION_TEST) != 0)
	{
		d_intersects = i_source.d_intersects;
	}

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
void CConeToCapsuleIntersection::Set(ConeCollisionObject* i_cone, Capsule* i_capsule)
{
	ASSERT((i_cone != NULL) && (i_capsule != NULL));
	d_cone = i_cone;
	d_capsule = i_capsule;
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCollisionIntersection* CConeToCapsuleIntersection::TestForIntersection(
	ConeCollisionObject* i_cone, Capsule* i_capsule, CCollisionStack* i_stack)
{
	ASSERT((i_cone != NULL) && (i_capsule != NULL));
	CConeToCapsuleIntersection* intersection = CConeToCapsuleIntersection::New(
		*i_cone, *i_capsule, i_stack);
	if (intersection)
	{
		if( intersection->IntersectionTest() )
			return( intersection );

		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

bool CConeToCapsuleIntersection::IntersectionTest( void )
{
	if( !(d_validFlags & INTERSECTION_TEST) )
	{
		d_intersects = false;

		// this test IS NOT PERFECT!  We want something fast, easy, and mildly accurate.
		// It will definitely return true for any capsule whose center is inside the cone,
		// but when the center is out of the cone, things are a little iffy.  In addition,
		// YOU WILL GET FALSE POSITIVES, because we're only comparing the capsule's spherical
		// radius (a sphere entirely enclosing the capsule), not the actual capsule.

		// first rejection: a sphere entirely enclosing the cone, against a sphere entirely enclosing the capsule
		float capR = d_capsule->SphericalRadius();
		Vector3 capC = d_capsule->Center();
		float rSum = d_cone->AxisLength() + capR;
		if( (capC - d_cone->End()).LengthSquared() < rSum * rSum )
		{
			// now compare the sphere entirely enclosing the capsule against the cone
			// algorithm from http://www.geometrictools.com/Intersection.html
			Vector3 U = d_cone->Origin() - (capR * d_cone->SinReciprocal()) * d_cone->Axis();
			Vector3 D = capC - U;
			float dsqr = D.LengthSquared();
			float e = d_cone->Axis().Dot( D );
			if( e > 0 && e * e >= dsqr * d_cone->CosSquared() )
			{
				D = capC - d_cone->Origin();
				dsqr = D.LengthSquared();
				e = -(d_cone->Axis().Dot( D ));
				if( e > 0 && e * e >= dsqr * d_cone->SinSquared() )
					d_intersects = dsqr <= capR * capR;
				else
				{
					// past the max axis?
					if( (capC - d_cone->Origin()).Dot( d_cone->Axis() ) < (d_cone->AxisLength() + capR) )
						d_intersects = true;
				}
			}
		}
	}	
	return d_intersects;
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CConeToCapsuleIntersection::GetName(void)
{
	return("ConeToCapsule");
}
#endif
