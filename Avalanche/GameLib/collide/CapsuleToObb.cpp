#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/CapsuleToObb.h"
#include "geomUtil/LineToLine.h"

const uint CCapsuleToObbIntersection::RADIUS_TEST				= 0x01;
const uint CCapsuleToObbIntersection::SEPARATING_AXIS_TEST		= 0x02;
const uint CCapsuleToObbIntersection::CLOSEST_FEATURE_TEST		= 0x04;
const uint CCapsuleToObbIntersection::CLOSEST_AXIS_TEST			= 0x08;
const uint CCapsuleToObbIntersection::FIRST_POINT_OF_CONTACT	= 0x10;

/***************************************************************************/
/***************************************************************************/
CCapsuleToObbIntersection::CCapsuleToObbIntersection(Capsule &i_capsule, OBB &i_obb) :
	CCollisionIntersection(CAPSULE_TO_OBB),
	d_capsule(&i_capsule),
	d_obb(&i_obb)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToObbIntersection::CCapsuleToObbIntersection() :
	CCollisionIntersection(CAPSULE_TO_OBB),
	d_capsule(NULL),
	d_obb(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToObbIntersection::CCapsuleToObbIntersection(Capsule &i_capsule, OBB &i_obb,
		CCollisionStack &i_stack) :
	CCollisionIntersection(CAPSULE_TO_OBB, i_stack),
	d_capsule(&i_capsule),
	d_obb(&i_obb)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToObbIntersection* CCapsuleToObbIntersection::New(Capsule &i_capsule,
		OBB &i_obb, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CCapsuleToObbIntersection* intersection =
			(CCapsuleToObbIntersection*)i_stack->Allocate(
			sizeof(CCapsuleToObbIntersection));

		if (intersection)
			new(intersection) CCapsuleToObbIntersection(i_capsule, i_obb, *i_stack);

		return(intersection);
	}

	return(new CCapsuleToObbIntersection(i_capsule, i_obb));
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToObbIntersection& CCapsuleToObbIntersection::operator=(
	CCapsuleToObbIntersection &i_source)
{
	d_capsule = i_source.d_capsule;
	d_obb = i_source.d_obb;
	d_validFlags = i_source.d_validFlags;

	/*
	if( d_validFlags & CLOSEST_FEATURE_TEST )
	{
		d_capsuleOriginInOBB = i_source.d_capsuleOriginInOBB;
		d_capsuleAxisInOBB = i_source.d_capsuleAxisInOBB;
		d_capsuleEndInOBB = i_source.d_capsuleEndInOBB;
		d_resolutionNormal = i_source.d_resolutionNormal;
		d_resolutionDepth = i_source.d_resolutionDepth;
		d_intersects = i_source.d_intersects;
	}
	*/
	if( d_validFlags & RADIUS_TEST )
	{
		d_radiusIntersection = i_source.d_radiusIntersection;
	}
	if( d_validFlags & FIRST_POINT_OF_CONTACT )
	{
		d_firstPoint = i_source.d_firstPoint;
		d_firstNormal = i_source.d_firstNormal;
		d_firstDepth = i_source.d_firstDepth;
	}

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
void CCapsuleToObbIntersection::Set(Capsule* i_capsule, OBB* i_obb)
{
	ASSERT((i_capsule != NULL) && (i_obb != NULL));
	d_capsule = i_capsule;
	d_obb = i_obb;
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCollisionIntersection* CCapsuleToObbIntersection::TestForIntersection(
	Capsule* i_capsule, OBB* i_obb, CCollisionStack* i_stack)
{
	CCapsuleToObbIntersection* intersection = CCapsuleToObbIntersection::New(*i_capsule,
		*i_obb, i_stack);
	if (intersection)
	{
		if (intersection->ClosestFeatureTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
bool CCapsuleToObbIntersection::RadiusTest( void )
{
	if( !(d_validFlags & RADIUS_TEST) )
	{
		d_validFlags |= RADIUS_TEST;

		float rsum = d_capsule->SphericalRadius() + d_obb->Radius();
		Vector3 c = d_capsule->Center() - *(d_obb->Center());
		if( c.LengthSquared() > (rsum * rsum) )
			d_radiusIntersection = false;
		else
			d_radiusIntersection = true;
	}
	return d_radiusIntersection;
}

bool CCapsuleToObbIntersection::SeparatingAxisTest( void )
{
	if( !(d_validFlags & SEPARATING_AXIS_TEST) )
	{
		d_validFlags |= SEPARATING_AXIS_TEST;
		d_intersects = false;

		if( !(d_validFlags & RADIUS_TEST) )
			RadiusTest();

		if( d_radiusIntersection )
		{
			// separating axes between an obb and a capsule:
			// 1) 3 cross products of box axes with capsule main axis direction (to test
			//	edges to capsule cylinder),
			// 2) 8 directions of lines perpendicular to capsule main axis, passing through
			//	each of the box vertices (to test vertices to capsule cylinder, actually
			//	catches the faces agains capsule cylinder as a side effect),
			// 3) 2 "Arvo's axes", one for each capsule sphere (this catches capsule caps
			//	against anything on the cube).

			Vector3CRef aextent = d_obb->GetConstAABB().maxV;
			Vector3 ax, ay, az, b, d;
			float halfAxis, cr;
			ax = d_obb->Matrix()->GetRow0(); ay = d_obb->Matrix()->GetRow1(); az = d_obb->Matrix()->GetRow2();
			b = d_capsule->Axis();
			d = *(d_obb->Center()) - d_capsule->Center();
			halfAxis = d_capsule->AxisLength() * 0.5f;
			cr = d_capsule->Radius();
			
			float r0, r1, r;		// interval radii and distance between centers
			
			float axDotd, ayDotd, azDotd, axDotb, ayDotb, azDotb;
			float AbsaxDotb, AbsayDotb, AbsazDotb;

			// box X cross capsule
			azDotd = az.Dot( d );
			azDotb = az.Dot( b );
			ayDotd = ay.Dot( d );
			ayDotb = ay.Dot( b );
			AbsayDotb = Math::Abs( ayDotb );
			AbsazDotb = Math::Abs( azDotb );
			r = Math::Abs( azDotd * ayDotb - ayDotd * azDotb );
			r0 = aextent.y() * AbsazDotb + aextent.z() * AbsayDotb;
			r1 = cr;
			d_axisOverlap[A0XB] = r0 + r1 - r;
			if( d_axisOverlap[A0XB] <= 0.0f )
				return false;

			// box Y cross capsule
			axDotd = ax.Dot( d );
			axDotb = ax.Dot( b );
			AbsaxDotb = Math::Abs( axDotb );
			r = Math::Abs( axDotd * azDotb - azDotd * axDotb );
			r0 = aextent.x() * AbsazDotb + aextent.z() * AbsaxDotb;
			r1 = cr;
			d_axisOverlap[A1XB] = r0 + r1 - r;
			if( d_axisOverlap[A1XB] <= 0.0f )
				return false;

			// box Z cross capsule
			r = Math::Abs( ayDotd * axDotb - axDotd * ayDotb );
			r0 = aextent.x() * AbsayDotb + aextent.y() * AbsaxDotb;
			r1 = cr;
			d_axisOverlap[A2XB] = r0 + r1 - r;
			if( d_axisOverlap[A2XB] <= 0.0f )
				return false;

			// these next 8 axes are perpendicular to the capsule's axis, in the direction
			// of each point on the box.

			// NOTE: the projections here are along a different vector than the cross tests,
			// the the amount of axis overlap we're storing isn't directly relative to previous
			// axis overlaps.
			// To compensate for this, if we try to determine which separating axis requires
			// the least amount of translation to make the 2 objects not collide,
			// we need to be sure to relate the depths to the length of the axis they're
			// projected along.  (the difference between the centers for the cross tests,
			// and the point from the capsule's axis to each box point for these 8)
			
			Vector3 a2p;
			Vector3 boxP;

			// P1 - maxX, maxY, maxZ
			// P2 - maxX, maxY, minZ
			// P3 - maxX, minY, maxZ
			// P4 - maxX, minY, minZ
			// P5 - minX, maxY, maxZ
			// P6 - minX, maxY, minZ
			// P7 - minX, minY, maxZ
			// P8 - minX, minY, minZ


			d_intersects = true;
		}
	}
	
	return d_intersects;
}

/***************************************************************************/
/***************************************************************************/
bool CCapsuleToObbIntersection::DirectionalFeatureTest( Vector3CRef dir, Vector3 &o_normal, float &o_depth )
{
	return false;
#if 0
	int i;
	bool found = false;
	float thisDepth;
	Vector3 point;
	Vector3 e1;
	Vector3 e2;
	Vector3 normal;
	float e1End;
	float e2End;
	Vector3 min = d_obb->GetConstAABB().GetMinV();
	Vector3 max = d_obb->GetConstAABB().GetMaxV();

	o_depth = MAXFLOAT;
	for( i = 0; i < 6; i++ )
	{
		switch( i )
		{
		case 0:
			//	Face				Vertices    	Edges       		Normal
			// ---- 				--------  		-----             ------
			//   0				0, 2, 6, 4		-3, 10, -5. -8		  -x
			normal.Set( -1.0f, 0.0f, 0.0f );
			point.Set( min.x(), max.y(), min.z() );
			e1.Set( 0.0f, -1.0f, 0.0f );
			e2.Set( 0.0f, 0.0f, 1.0f );
			e1End = max.y() - min.y();
			e2End = max.z() - min.z();
			break;
		case 1:
			//	Face				Vertices    	Edges       		Normal
			// ---- 				--------  		-----             ------
			//   1				1, 5, 7, 3		9, -7, -11, -1		  +x
			normal.Set( 1.0f, 0.0f, 0.0f );
			point.Set( max.x(), max.y(), min.z() );
			e1.Set( 0.0f, 0.0f, 1.0f );
			e2.Set( 0.0f, -1.0f, 0.0f );
			e1End = max.z() - min.z();
			e2End = max.y() - min.y();
			break;
		case 2:
			//	Face				Vertices    	Edges       		Normal
			// ---- 				--------  		-----             ------
			//   2				0, 1, 3. 2		0, 1, 2, 3			  -y
			normal.Set( 0.0f, -1.0f, 0.0f );
			point.Set( min.x(), min.y(), min.z() );
			e1.Set( 1.0f, 0.0f, 0.0f );
			e2.Set( 0.0f, 0.0f, 1.0f );
			e1End = max.x() - min.x();
			e2End = max.z() - min.z();
			break;
		case 3:
			//	Face				Vertices    	Edges       		Normal
			// ---- 				--------  		-----             ------
			//   3				4, 6, 7, 5		5, 6, 7, 4			  +y
			normal.Set( 0.0f, 1.0f, 0.0f );
			point.Set( min.x(), max.y(), min.z() );
			e1.Set( 0.0f, 0.0f, 1.0f );
			e2.Set( 1.0f, 0.0f, 0.0f );
			e1End = max.z() - min.z();
			e2End = max.x() - min.x();
			break;
		case 4:
			//	Face				Vertices    	Edges       		Normal
			// ---- 				--------  		-----             ------
			//   4				0, 4, 5, 1		8, -4, -9, -0		  -z
			normal.Set( 0.0f, 0.0f, -1.0f );
			point.Set( min.x(), max.y(), min.z() );
			e1.Set( 1.0f, 0.0f, 0.0f );
			e2.Set( 0.0f, -1.0f, 0.0f );
			e1End = max.x() - min.x();
			e2End = max.y() - min.y();
			break;
		case 5:
			//	Face				Vertices    	Edges       		Normal
			// ---- 				--------  		-----             ------
			//   5				2, 3, 7, 6		-2, 11, -6, -10	  +z
			normal.Set( 0.0f, 0.0f, 1.0f );
			point.Set( min.x(), max.y(), max.z() );
			e1.Set( 0.0f, -1.0f, 0.0f );
			e2.Set( 1.0f, 0.0f, 0.0f );
			e1End = max.y() - min.y();
			e2End = max.x() - min.x();
			break;
		}
		SegmentToRectangleInfo info;

		DirCos3x3 matrix;
		Vector3 pw, nw, e1w, e2w;
		matrix = *(d_obb->Matrix());
		pw = (point * matrix) + *(d_obb->Center());
		nw = (normal) * matrix;
		e1w = (e1) * matrix;
		e2w = (e2) * matrix;
		
		DrawUtility::Line( pw, pw + nw, DrawUtility::GREEN );
		DrawUtility::Line( pw, pw + e1w, DrawUtility::BLUE );
		DrawUtility::Line( pw, pw + e2w, DrawUtility::BLUE );

		thisDepth = CapsuleToRectangleMaxPenetration( d_capsuleOriginInOBB, d_capsuleAxisInOBB,
			d_capsule->Radius(), 0.0f, d_capsule->AxisLength(), normal, point, 
			e1, 0.0f, e1End, e2, 0.0f, e2End, info );

		if( thisDepth < o_depth )
		{
			o_depth = thisDepth;
			o_normal = normal;
			if( o_depth > 0.0f )
				found = true;
		}
	}
	if( found && o_depth >= 0.0f )
		o_normal = o_normal * (*(d_obb->Matrix()));
	else
		o_depth = 0.0f;

	return found;
#endif
}

/***************************************************************************/
/***************************************************************************/
bool CCapsuleToObbIntersection::ClosestFeatureTest( void )
{
	if( !(d_validFlags & CLOSEST_FEATURE_TEST) )
	{
		d_validFlags |= CLOSEST_FEATURE_TEST;

		d_intersects = false;

		if( !(d_validFlags & RADIUS_TEST) )
			RadiusTest();

		if( d_radiusIntersection )
		{	
			float dSquared;
			Vector3 pointOnLine, pointOnBox;

			float halfAxis = d_capsule->AxisLength() * 0.5f;
			dSquared = SegmentToOBBDistanceSquared( d_capsule->Center(), d_capsule->Axis(),
				-halfAxis, halfAxis, *d_obb, pointOnLine, pointOnBox );

			float rSquared = d_capsule->Radius();
			rSquared *= rSquared;
			// recheck length, since SegmentToOBBDistance is returning incorrect distances in
			// some cases, although it still seems to be returning the correct points.
			// Talk to the idiot who typed in that indecipherable bullshit, probably making
			// a very important, but hard to track down, typo somewhere.
			if( (pointOnBox - pointOnLine).LengthSquared() < rSquared )
			{
				d_intersects = true;
				d_closestPointOnObb = pointOnBox;
				d_closestPointOnCapsule = pointOnLine;
			}
		}
	}
	else
		d_intersects = false;

	return d_intersects;
}

bool CCapsuleToObbIntersection::GetResolutionNormal( Vector3 &o_normal, float &o_depth )
{
	if( d_validFlags & CLOSEST_FEATURE_TEST )
	{
		o_normal = d_closestPointOnCapsule - d_closestPointOnObb;
		o_depth = o_normal.Length();
		if( Math::Zero( o_depth ) )
		{
			// which face is the pointOnObb on?
			float xP, yP, zP;
			float absx, absy, absz;
			Vector3 pMinusC = d_closestPointOnObb - *(d_obb->Center());
			xP = pMinusC.Dot( d_obb->Matrix()->GetRow0() ) / d_obb->GetConstAABB().GetMaxV().x();
			yP = pMinusC.Dot( d_obb->Matrix()->GetRow1() ) / d_obb->GetConstAABB().GetMaxV().y();
			zP = pMinusC.Dot( d_obb->Matrix()->GetRow2() ) / d_obb->GetConstAABB().GetMaxV().z();

			absx = Math::Abs( xP );
			absy = Math::Abs( yP );
			absz = Math::Abs( zP );

			if( absx > absy )
			{
				if( absx > absz )
				{
					o_normal = d_obb->Matrix()->GetRow0();
					if( xP < 0.0f )
						o_normal = -o_normal;
				}
				else
				{
					o_normal = d_obb->Matrix()->GetRow2();
					if( zP < 0.0f )
						o_normal = -o_normal;
				}
			}
			else
			{
				if( absy > absz )
				{
					o_normal = d_obb->Matrix()->GetRow1();
					if( yP < 0.0f )
						o_normal = -o_normal;
				}
				else
				{
					o_normal = d_obb->Matrix()->GetRow2();
					if( zP < 0.0f )
						o_normal = -o_normal;
				}
			}
		}
		else
			o_normal = o_normal / o_depth;

		o_depth = d_capsule->Radius() - o_depth;

		return d_intersects;
	}

	return false;
#if 0
	if( !(d_validFlags & CLOSEST_AXIS_TEST) )
	{
		d_validFlags |= CLOSEST_AXIS_TEST;

		if( !(d_validFlags & SEPARATING_AXIS_TEST) )
			SeparatingAxisTest();

		if( d_intersects )
		{
			int i;
			d_closestAxisDepth = MAXFLOAT;

			for( i = 0; i < NUM_SEPARATING_AXIS; i++ )
			{
				if( d_axisOverlap[i] < d_closestAxisDepth )
				{
					d_closestAxisDepth = d_axisOverlap[i];
					d_closestAxis = i;
				}
			}
			switch( d_closestAxis )
			{
				case A0:
					d_closestAxisNormal = d_obb->Matrix()->GetRow0();
					break;
				case A1:
					d_closestAxisNormal = d_obb->Matrix()->GetRow1();
					break;
				case A2:
					d_closestAxisNormal = d_obb->Matrix()->GetRow2();
					break;
				case B:
					d_closestAxisNormal = d_capsule->Axis();
					break;
				case A0XB:
					d_closestAxisNormal = d_obb->Matrix()->GetRow0().Cross( d_capsule->Axis() );
					break;
				case A1XB:
					d_closestAxisNormal = d_obb->Matrix()->GetRow1().Cross( d_capsule->Axis() );
					break;
				case A2XB:
					d_closestAxisNormal = d_obb->Matrix()->GetRow2().Cross( d_capsule->Axis() );
					break;
				case D:
					{
						Vector3 d = *(d_obb->Center()) - d_capsule->Center();
						
						d_closestAxisNormal = d - (d_capsule->Axis() * d.Dot(d_capsule->Axis()));
						break;
					}
			}
			d_closestAxisNormal.SafeNormalize();
			if( d_closestAxisNormal.Dot( *(d_obb->Center()) - d_capsule->Center() ) > 0.0f )
				d_closestAxisNormal = -d_closestAxisNormal;
		}
	}
	if( d_intersects )
	{
		o_normal = d_closestAxisNormal;
		o_depth = d_closestAxisDepth;
	}
	return d_intersects;
#endif
}

bool CCapsuleToObbIntersection::GetFirstPointOfContact( Vector3CRef movementDirection, Vector3 &o_point, 
		Vector3 &o_normal, float &o_depth )
{
	
	if( !(d_validFlags & CLOSEST_FEATURE_TEST) )
		ClosestFeatureTest();

	if( d_intersects && !(d_validFlags & FIRST_POINT_OF_CONTACT) )
	{
		d_validFlags |= FIRST_POINT_OF_CONTACT;
		d_foundFirstPoint = false;
		if( DirectionalFeatureTest( -movementDirection, d_firstNormal, d_firstDepth ) )
		{
			d_foundFirstPoint = true;
			o_normal = d_firstNormal;
			o_depth = d_firstDepth;
			o_point = d_firstPoint;
		}
	}

	return d_foundFirstPoint;
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CCapsuleToObbIntersection::GetName(void)
{
	return("CapsuleToObb");
}
#endif
