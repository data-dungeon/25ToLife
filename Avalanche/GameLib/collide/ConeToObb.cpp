#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/ConeToObb.h"
#include "geomUtil/LineToLine.h"

const uint CConeToObbIntersection::INTERSECTION_TEST		= 0x01;

/***************************************************************************/
/***************************************************************************/
CConeToObbIntersection::CConeToObbIntersection(
	ConeCollisionObject &i_cone, OBB &i_obb) :
CCollisionIntersection(CONE_TO_OBB),
d_cone(&i_cone),
d_obb(&i_obb)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CConeToObbIntersection::CConeToObbIntersection() :
CCollisionIntersection(CONE_TO_OBB),
d_cone(NULL),
d_obb(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CConeToObbIntersection::CConeToObbIntersection(
	ConeCollisionObject &i_cone, OBB &i_obb, CCollisionStack &i_stack) :
CCollisionIntersection(CONE_TO_OBB, i_stack),
d_cone(&i_cone),
d_obb(&i_obb)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CConeToObbIntersection* CConeToObbIntersection::New(
	ConeCollisionObject &i_cone, OBB &i_obb, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CConeToObbIntersection* intersection =
			(CConeToObbIntersection*)i_stack->Allocate(
			sizeof(CConeToObbIntersection));

		if (intersection)
			new(intersection) CConeToObbIntersection(i_cone, i_obb, *i_stack);

		return(intersection);
	}

	return(new CConeToObbIntersection(i_cone, i_obb));
}

/***************************************************************************/
/***************************************************************************/
CConeToObbIntersection& CConeToObbIntersection::operator=(
	CConeToObbIntersection &i_source)
{
	d_cone = i_source.d_cone;
	d_obb = i_source.d_obb;
	d_validFlags = i_source.d_validFlags;

	if ((d_validFlags & INTERSECTION_TEST) != 0)
	{
		d_intersects = i_source.d_intersects;
	}

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
void CConeToObbIntersection::Set(ConeCollisionObject* i_cone, OBB* i_obb)
{
	ASSERT((i_cone != NULL) && (i_obb != NULL));
	d_cone = i_cone;
	d_obb = i_obb;
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCollisionIntersection* CConeToObbIntersection::TestForIntersection(
	ConeCollisionObject* i_cone, OBB* i_obb, CCollisionStack* i_stack)
{
	ASSERT((i_cone != NULL) && (i_obb != NULL));
	CConeToObbIntersection* intersection = CConeToObbIntersection::New(
		*i_cone, *i_obb, i_stack);
	if (intersection)
	{
		if( intersection->IntersectionTest() )
			return( intersection );

		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

bool CConeToObbIntersection::IntersectionTest( void )
{
	if( !(d_validFlags & INTERSECTION_TEST) )
	{
		d_intersects = false;

		// this test IS NOT PERFECT!  We want something fast, easy, and mildly accurate.
		// It will definitely return true for any obb whose center is inside the cone,
		// but when the center is out of the cone, things are a little iffy.  In addition,
		// YOU WILL GET FALSE POSITIVES, because we're only comparing the obb's spherical
		// radius (a sphere entirely enclosing the obb), not the actual obb.

		// first rejection: a sphere entirely enclosing the cone, against a sphere entirely enclosing the obb
		float obbR = d_obb->Radius();
		Vector3 obbC = *(d_obb->Center());
		float rSum = d_cone->AxisLength() + obbR;
		if( (obbC - d_cone->End()).LengthSquared() < rSum * rSum )
		{
			// now compare the sphere entirely enclosing the obb against the cone
			// algorithm from http://www.geometrictools.com/Intersection.html
			Vector3 U = d_cone->Origin() - (obbR * d_cone->SinReciprocal()) * d_cone->Axis();
			Vector3 D = obbC - U;
			float dsqr = D.LengthSquared();
			float e = d_cone->Axis().Dot( D );
			if( e > 0 && e * e >= dsqr * d_cone->CosSquared() )
			{
				D = obbC - d_cone->Origin();
				dsqr = D.LengthSquared();
				e = -(d_cone->Axis().Dot( D ));
				if( e > 0 && e * e >= dsqr * d_cone->SinSquared() )
					d_intersects = dsqr <= obbR * obbR;
				else
				{
					// past the max axis?
					if( (obbC - d_cone->Origin()).Dot( d_cone->Axis() ) < (d_cone->AxisLength() + obbR) )
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
const char* CConeToObbIntersection::GetName(void)
{
	return("ConeToOBB");
}
#endif
