#include "collide/CollidePCH.h"

#include "collide/LineToCapsule.h"

// Gamelib includes //
#include "collide/Stack.h"
#include "Layers/Assert/Assert.h"
#include "geomUtil/LineToLine.h"

// Valid data masks.
const uint CLineToCapsuleIntersection::INTERSECTION_TEST = 0x01;
const uint CLineToCapsuleIntersection::ENTRY_POINT       = 0x02;
const uint CLineToCapsuleIntersection::ENTRY_NORMAL      = 0x04;
const uint CLineToCapsuleIntersection::EXIT_POINT        = 0x08;
const uint CLineToCapsuleIntersection::EXIT_NORMAL       = 0x10;

/***************************************************************************/
/***************************************************************************/
CLineToCapsuleIntersection::CLineToCapsuleIntersection() :
	CCollisionIntersection(LINE_TO_CAPSULE),
	d_line(NULL),
	d_capsule(NULL)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
CLineToCapsuleIntersection::CLineToCapsuleIntersection(
		CLineCollisionObject &i_line, Capsule &i_capsule) :
	CCollisionIntersection(LINE_TO_CAPSULE),
	d_line(&i_line),
	d_capsule(&i_capsule)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
CLineToCapsuleIntersection::CLineToCapsuleIntersection(
		CLineCollisionObject &i_line, Capsule &i_capsule,
		CCollisionStack &i_stack) :
	CCollisionIntersection(LINE_TO_CAPSULE, i_stack),
	d_line(&i_line),
	d_capsule(&i_capsule)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
CLineToCapsuleIntersection::~CLineToCapsuleIntersection()
{
	if (d_modifiedLine)
		d_line->RestoreFindState(this, d_previousModifier, d_previousPoint);
}

/***************************************************************************/
/***************************************************************************/
CLineToCapsuleIntersection* CLineToCapsuleIntersection::New(
		CLineCollisionObject &i_line, Capsule &i_capsule,
		CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CLineToCapsuleIntersection* intersection =
			(CLineToCapsuleIntersection*)i_stack->Allocate(
			sizeof(CLineToCapsuleIntersection));

		if (intersection)
			new(intersection) CLineToCapsuleIntersection(i_line, i_capsule,
			*i_stack);

		return(intersection);
	}

	return(new CLineToCapsuleIntersection(i_line, i_capsule));
}

/***************************************************************************/
/***************************************************************************/
CLineToCapsuleIntersection& CLineToCapsuleIntersection::operator=(
		CLineToCapsuleIntersection &i_source)
{
	d_line = i_source.d_line;
	d_capsule = i_source.d_capsule;
	d_validFlags = i_source.d_validFlags;

	if ((d_validFlags & INTERSECTION_TEST) != 0)
	{
		d_intersects = i_source.d_intersects;
		d_enters = i_source.d_enters;
		d_exits = i_source.d_exits;
		d_entryFrac = i_source.d_entryFrac;
		d_exitFrac = i_source.d_exitFrac;
	}

	if ((d_validFlags & ENTRY_POINT) != 0)
		d_entryPoint = i_source.d_entryPoint;

	if ((d_validFlags & ENTRY_NORMAL) != 0)
		d_entryNormal = i_source.d_entryNormal;

	if ((d_validFlags & EXIT_POINT) != 0)
		d_exitPoint = i_source.d_exitPoint;

	if ((d_validFlags & EXIT_NORMAL) != 0)
		d_exitNormal = i_source.d_exitNormal;

	d_modifiedLine = false;

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
void CLineToCapsuleIntersection::ClearTests(void)
{
	if (d_modifiedLine)
		d_line->RestoreFindState(this, d_previousModifier, d_previousPoint);

	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
void CLineToCapsuleIntersection::Set(CLineCollisionObject* i_line,
										   Capsule* i_capsule)
{
	ASSERT((i_line != NULL) && (i_capsule != NULL));
	ClearTests();
	d_line = i_line;
	d_capsule = i_capsule;
}

/***************************************************************************/
/***************************************************************************/
bool CLineToCapsuleIntersection::IntersectionTest(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	return(d_intersects);
}

/***************************************************************************/
/***************************************************************************/
bool CLineToCapsuleIntersection::Enters(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_enters);
}

/***************************************************************************/
/***************************************************************************/
float CLineToCapsuleIntersection::EntryFrac(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_entryFrac);
}

/***************************************************************************/
/***************************************************************************/
const Vector3& CLineToCapsuleIntersection::EntryPoint(void)
{
	if ((d_validFlags & ENTRY_POINT) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_intersects)
			d_line->ComputePoint(d_entryFrac, d_entryPoint);
		else
			d_entryPoint.Clear();

		d_validFlags |= ENTRY_POINT;
	}

	return(d_entryPoint);
}

/***************************************************************************/
/***************************************************************************/
const Vector3& CLineToCapsuleIntersection::EntryNormal(void)
{
	if ((d_validFlags & ENTRY_NORMAL) == 0)
	{
		d_validFlags |= ENTRY_NORMAL;

		if ((d_validFlags & ENTRY_POINT) == 0)
		{
			d_validFlags |= ENTRY_POINT;

			if ((d_validFlags & INTERSECTION_TEST) == 0)
				PerformIntersectionTest();

			if (d_intersects)
			{
				d_line->ComputePoint(d_entryFrac, d_entryPoint);
			}
			else
			{
				d_entryPoint.Clear();
				d_entryNormal.Set(0.0f, 1.0f, 0.0f);
				return(d_entryNormal);
			}
		}
		else if (!d_intersects)
		{
			d_entryNormal.Set(0.0f, 1.0f, 0.0f);
			return(d_entryNormal);
		}

		d_entryNormal = d_capsule->SurfaceNormalAtPoint( d_entryPoint );
	}

	return(d_entryNormal);
}

/***************************************************************************/
/***************************************************************************/
bool CLineToCapsuleIntersection::Exits(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exits);
}

/***************************************************************************/
/***************************************************************************/
float CLineToCapsuleIntersection::ExitFrac(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exitFrac);
}

/***************************************************************************/
/***************************************************************************/
const Vector3& CLineToCapsuleIntersection::ExitPoint(void)
{
	if ((d_validFlags & EXIT_POINT) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_intersects)
			d_line->ComputePoint(d_exitFrac, d_exitPoint);
		else
			d_exitPoint.Clear();

		d_validFlags |= EXIT_POINT;
	}

	return(d_exitPoint);
}

/***************************************************************************/
/***************************************************************************/
const Vector3& CLineToCapsuleIntersection::ExitNormal(void)
{
	if ((d_validFlags & EXIT_NORMAL) == 0)
	{
		d_validFlags |= EXIT_NORMAL;

		if ((d_validFlags & EXIT_POINT) == 0)
		{
			d_validFlags |= EXIT_POINT;

			if ((d_validFlags & INTERSECTION_TEST) == 0)
				PerformIntersectionTest();

			if (d_intersects)
			{
				d_line->ComputePoint(d_exitFrac, d_exitPoint);
			}
			else
			{
				d_exitPoint.Clear();
				d_exitNormal.Set(0.0f, 1.0f, 0.0f);
				return(d_exitNormal);
			}
		}
		else if (!d_intersects)
		{
			d_exitNormal.Set(0.0f, 1.0f, 0.0f);
			return(d_exitNormal);
		}

		d_exitNormal = d_capsule->SurfaceNormalAtPoint( d_exitPoint );
	}

	return(d_exitNormal);
}

/***************************************************************************/
/***************************************************************************/
CCollisionIntersection* CLineToCapsuleIntersection::TestForIntersection(
	CLineCollisionObject* i_line, Capsule* i_capsule,
	CCollisionStack* i_stack)
{
	CLineToCapsuleIntersection* intersection = CLineToCapsuleIntersection::New(
		*i_line, *i_capsule, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

bool CLineToCapsuleIntersection::TestCapsuleSphere( Vector3CRef center, float radiusSquared, 
		Vector3CRef lineOrigin, Vector3CRef lineDirection, 
		float &entry, float &exit )
{
	Vector3 L;
	float s, LSquared, mSquared, q;

	L = center - lineOrigin;
	s = L.Dot( lineDirection );
	LSquared = L.LengthSquared();
	if( s < 0 && LSquared > radiusSquared )
		return false;
	mSquared = LSquared - ( s * s );
	if( mSquared > radiusSquared )
		return false;
	q = Math::Sqrt( radiusSquared - mSquared );
	entry = (s - q);
	exit = (s + q);
	return true;
}

/***************************************************************************/
/***************************************************************************/
void CLineToCapsuleIntersection::PerformIntersectionTest(void)
{
	ASSERT((d_line != NULL) && (d_capsule != NULL));
	d_validFlags |= INTERSECTION_TEST;

	// first of all, see if they could possibly intersect at all.  
	// This requires a point-to-line distance check, which is cheap, but doesn't necessarily
	// mean the line intersects the capsule.  It's just a quick rejection test

	if( PointToSegmentDistanceSquared( d_line->Origin(), d_line->Basis(), 
			d_line->MinBoundary(), d_line->MaxBoundary(), d_capsule->Center() ) >
			(d_capsule->SphericalRadius() * d_capsule->SphericalRadius()) )
	{
		d_intersects = false;
		d_enters = false;
		d_exits = false;
		return;
	}

	// another quick rejection.  if this one passes, then we're definitely intersecting,
	// but this test doesn't particularly help us determine where the intersection points
	// are.
	if( SegmentToSegmentDistanceSquared( d_line->Origin(), d_line->Basis(),
		d_line->MinBoundary(), d_line->MaxBoundary(), 
		d_capsule->Origin(), d_capsule->Axis(), 0.0f, d_capsule->AxisLength() ) >
		(d_capsule->Radius() * d_capsule->Radius()) )
	{
		d_intersects = false;
		d_enters = false;
		d_exits = false;
		return;
	}

	// check to see if both of the line's points are inside the capsule.  If so,
	// we have an intersection but no entry or exit.
	if( d_capsule->PointInsideCapsule( d_line->Origin() ) &&
		d_capsule->PointInsideCapsule( d_line->End() ) )
	{
		d_intersects = true;
		d_enters = false;
		d_exits = false;
		return;
	}

	// P0 and D0 are the start and normalized direction of the capsule's axis
	// P1 and D1 are the start and normalized direction of the line
	// a, b, and c are coefficients of the quadratic equation

	Vector3 P0 = d_capsule->Origin();
	Vector3 D0 = d_capsule->Axis(); // capsule stores its axis as a direction and length already
	Vector3 P1 = d_line->Origin();
	Vector3 D1 = d_line->Basis();

	float rSquared = d_capsule->Radius();
	rSquared = rSquared * rSquared;

	float lineLength;
	float basisLength = D1.Length();

	// if the line has zero length, there is no intersection, since we already checked
	// to see if both ends were inside the capsule
	if( Math::Zero( basisLength ) )
	{
		d_intersects = false;
		d_enters = false;
		d_exits = false;
		return;
	}
	D1 = D1 / basisLength;
	lineLength = basisLength * d_line->MaxBoundary() - d_line->MinBoundary();

	bool foundEntry = false;
	bool foundExit = false;
	bool validEntry = true;
	bool validExit = true;
	Vector3 entryPoint, exitPoint;

	// if the capsule's axis length is zero, then it's really just a sphere.  Since at this point, we
	// already know we're intersecting, just run entry/exit points on the origin sphere
	if( Math::Zero( d_capsule->AxisLength() ) )
	{
		float entry, exit;
		bool mustBeTrue = TestCapsuleSphere( d_capsule->End(), rSquared, P1, D1, entry, exit );
		ASSERT( mustBeTrue && "Something majorly horked in line-to-capsule on a capsule with zero length" );

		foundEntry = true;
		foundExit = true;
		d_entryFrac = entry / basisLength;
		d_exitFrac = exit / basisLength;
		validEntry = true;
		validExit = true;
	}
	else
	{
		// transform the line into the capsule's coordinate system
		Vector3 P1InCapsule = P1 - P0;
		P1InCapsule = P1InCapsule / d_capsule->Orientation();
		Vector3 D1InCapsule = D1 / d_capsule->Orientation();

		// now we can use the equation of our cylinder (x2 + z2 = r2)
		// to determine where the intersection points are

		float a, b, c;
		a = (D1InCapsule.x() * D1InCapsule.x()) + (D1InCapsule.z() * D1InCapsule.z());
		b = 2 * ( (D1InCapsule.x() * P1InCapsule.x()) + (D1InCapsule.z() * P1InCapsule.z()) );
		c = (P1InCapsule.x() * P1InCapsule.x()) + (P1InCapsule.z() * P1InCapsule.z()) - rSquared;

		float root = (b * b) - (4 * a * c);
		if( root < 0.0f || Math::Zero( a ) ) // imaginary root, no intersection
		{
			d_intersects = false;
			d_enters = false;
			d_exits = false;
			return;
		}
		root = Math::Sqrt( root );
		
		d_entryFrac = ((-b - root) / (2 * a)) / basisLength;
		d_exitFrac = ((-b + root) / (2 * a)) / basisLength;

		Vector3 entryPoint, exitPoint;
		entryPoint = d_line->Origin() + d_line->Basis() * d_entryFrac;
		exitPoint = d_line->Origin() + d_line->Basis() * d_exitFrac;
		
		// if the intersection points are off the end of the cylinder, test against the
		// spherical end caps instead
		float sEntry = (entryPoint - P0).Dot( D0 );
		float sExit = (exitPoint - P0).Dot( D0 );

		if( sEntry < 0.0f ) // entry (and possibly exit) off the origin end
		{
			float entry, exit;
			if( TestCapsuleSphere( P0, rSquared, P1, D1, entry, exit ) )
			{
				foundEntry = true;
				d_entryFrac = entry / basisLength;
				// is the exit going out of the same sphere?
				if( sExit < 0.0f )
				{
					foundExit = true;
					d_exitFrac = exit / basisLength;
				}
			}
			else
				validEntry = false;
		}
		else if( sEntry > d_capsule->AxisLength() ) // entry (and possibly exit)
		{
			float entry, exit;
			if( TestCapsuleSphere( d_capsule->End(), rSquared, P1, D1, entry, exit ) )
			{
				foundEntry = true;
				d_entryFrac = entry / basisLength;
				// is the exit going out of the same sphere?
				if( sExit > d_capsule->AxisLength() )
				{
					foundExit = true;
					d_exitFrac = exit / basisLength;
				}
			}
			else
				validEntry = false;
		}
		if( !foundExit ) // if we already found the exit point, we don't need to do it again
		{
			if( sExit < 0.0f )
			{
				float entry, exit;
				if( TestCapsuleSphere( P0, rSquared, P1, D1, entry, exit ) )
				{
					foundExit = true;
					d_exitFrac = exit / basisLength;
				}
				else
					validExit = false;
			}
			else if( sExit > d_capsule->AxisLength() )
			{
				float entry, exit;
				if( TestCapsuleSphere( d_capsule->End(), rSquared, P1, D1, entry, exit ) )
				{
					foundExit = true;
					d_exitFrac = exit / basisLength;
				}
				else
					validExit = false;
			}
		}
	}
	entryPoint = d_line->Origin() + d_line->Basis() * d_entryFrac;
	exitPoint = d_line->Origin() + d_line->Basis() * d_exitFrac;

	if ( validEntry && validExit && d_line->IsOn(d_entryFrac, d_exitFrac))
	{
		d_intersects = true;

		if( d_line->IsOn( d_entryFrac ) )
			d_enters = d_line->DetectFrontSide();
		else
			d_enters = false;
		if( d_line->IsOn( d_exitFrac ) )
			d_exits = d_line->DetectBackSide();
		else
			d_exits = false;

		switch (d_line->FindMode())
		{
		case LINE_FIND_ALL:
			return;
		case LINE_FIND_CLOSEST:
			d_previousPoint = d_line->MaxBoundary();
			if (d_enters)
			{
				if (!d_line->SetClosestPoint(d_entryFrac))
					return;
				d_foundPoint = entryPoint;
			}
			else if (d_exits && d_line->SetClosestPoint(d_exitFrac))
			{
				d_foundPoint = exitPoint;
			}
			else if (d_line->DetectBothSides() &&
				d_line->SetClosestPoint(d_line->MinBoundary()))
			{
				d_line->ComputePoint(d_line->MinBoundary(), d_foundPoint);
			}
			else
				return;
			d_foundNormal = d_capsule->SurfaceNormalAtPoint( d_foundPoint );
			d_modifiedLine = true;
			d_previousModifier = d_line->SetModifier(this);
			return;
		case LINE_FIND_FURTHEST:
			d_previousPoint = d_line->MinBoundary();
			if (d_exits)
			{
				if (!d_line->SetFurthestPoint(d_exitFrac))
					return;
				d_foundPoint = exitPoint;
			}
			else if (d_enters && d_line->SetFurthestPoint(d_entryFrac))
			{
				d_foundPoint = entryPoint;
			}
			else if (d_line->DetectBothSides() &&
				d_line->SetFurthestPoint(d_line->MaxBoundary()))
			{
				d_line->ComputePoint(d_line->MaxBoundary(), d_foundPoint);
			}
			else
				return;
			d_foundNormal = d_capsule->SurfaceNormalAtPoint( d_foundPoint );
			d_modifiedLine = true;
			d_previousModifier = d_line->SetModifier(this);
			return;
		case LINE_FIND_ANY:
			if (d_line->Modifier() == NULL)
			{
				d_modifiedLine = true;
				d_previousModifier = d_line->SetModifier(this);
			}
			return;
		}
	}

	d_intersects = false;
	d_enters = false;
	d_exits = false;
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CLineToCapsuleIntersection::GetName(void)
{
	return("LineToCapsule");
}
#endif
