#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/CapsuleToCapsule.h"
#include "geomUtil/LineToLine.h"

const uint CCapsuleToCapsuleIntersection::INTERSECTION_TEST     = 0x01;

/***************************************************************************/
/***************************************************************************/
CCapsuleToCapsuleIntersection::CCapsuleToCapsuleIntersection(Capsule &i_capsuleA, Capsule &i_capsuleB) :
	CCollisionIntersection(CAPSULE_TO_CAPSULE),
	d_capsuleA(&i_capsuleA),
	d_capsuleB(&i_capsuleB)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToCapsuleIntersection::CCapsuleToCapsuleIntersection() :
	CCollisionIntersection(CAPSULE_TO_CAPSULE),
	d_capsuleA(NULL),
	d_capsuleB(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToCapsuleIntersection::CCapsuleToCapsuleIntersection(Capsule &i_capsuleA, Capsule &i_capsuleB,
		CCollisionStack &i_stack) :
	CCollisionIntersection(CAPSULE_TO_CAPSULE, i_stack),
	d_capsuleA(&i_capsuleA),
	d_capsuleB(&i_capsuleB)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToCapsuleIntersection* CCapsuleToCapsuleIntersection::New(Capsule &i_capsuleA,
		Capsule &i_capsuleB, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CCapsuleToCapsuleIntersection* intersection =
			(CCapsuleToCapsuleIntersection*)i_stack->Allocate(
			sizeof(CCapsuleToCapsuleIntersection));

		if (intersection)
			new(intersection) CCapsuleToCapsuleIntersection(i_capsuleA, i_capsuleB, *i_stack);

		return(intersection);
	}

	return(new CCapsuleToCapsuleIntersection(i_capsuleA, i_capsuleB));
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToCapsuleIntersection& CCapsuleToCapsuleIntersection::operator=(
	CCapsuleToCapsuleIntersection &i_source)
{
	d_capsuleA = i_source.d_capsuleA;
	d_capsuleB = i_source.d_capsuleB;
	d_validFlags = i_source.d_validFlags;

	if( d_validFlags & INTERSECTION_TEST )
	{
		d_intersects = i_source.d_intersects;
		d_closestPointOnA = i_source.d_closestPointOnA;
		d_closestPointOnB = i_source.d_closestPointOnB;
	}

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
void CCapsuleToCapsuleIntersection::SetCapsules(Capsule* i_capsuleA, Capsule* i_capsuleB)
{
	ASSERT((i_capsuleA != NULL) && (i_capsuleB != NULL));
	d_capsuleA = i_capsuleA;
	d_capsuleB = i_capsuleB;
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCollisionIntersection* CCapsuleToCapsuleIntersection::TestForIntersection(
	Capsule* i_capsuleA, Capsule* i_capsuleB, CCollisionStack* i_stack)
{
	CCapsuleToCapsuleIntersection* intersection = CCapsuleToCapsuleIntersection::New(*i_capsuleA,
		*i_capsuleB, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
bool CCapsuleToCapsuleIntersection::IntersectionTest( void )
{
	if( !(d_validFlags & INTERSECTION_TEST) )
	{
		d_validFlags |= INTERSECTION_TEST;

		float d, t1, t2, rsum;

		d = SegmentToSegmentDistanceSquared( d_capsuleA->Origin(), d_capsuleA->Axis(), 0, d_capsuleA->AxisLength(),
			d_capsuleB->Origin(), d_capsuleB->Axis(), 0, d_capsuleB->AxisLength(), 
			t1, t2, d_closestPointOnA, d_closestPointOnB );
		
		rsum = d_capsuleA->Radius() + d_capsuleB->Radius();
		if( d > (rsum * rsum) )
			d_intersects = false;
		else
		{
			d_intersects = true;
			// temp run again so we can trace
			d = SegmentToSegmentDistanceSquared( d_capsuleA->Origin(), d_capsuleA->Axis(), 0, d_capsuleA->AxisLength(),
				d_capsuleB->Origin(), d_capsuleB->Axis(), 0, d_capsuleB->AxisLength(), 
				t1, t2, d_closestPointOnA, d_closestPointOnB );
		}
	}
	return d_intersects;
}

/***************************************************************************/
/***************************************************************************/
Vector3CRef CCapsuleToCapsuleIntersection::ClosestPointOnA( void )
{
	if( !(d_validFlags & INTERSECTION_TEST) )
		IntersectionTest();
	return d_closestPointOnA;
}

/***************************************************************************/
/***************************************************************************/
Vector3CRef CCapsuleToCapsuleIntersection::ClosestPointOnB( void )
{
	if( !(d_validFlags & INTERSECTION_TEST) )
		IntersectionTest();
	return d_closestPointOnB;
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CCapsuleToCapsuleIntersection::GetName(void)
{
	return("CapsuleToCapsule");
}
#endif
