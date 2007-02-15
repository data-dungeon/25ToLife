#include "geomUtil/GeomUtilPCH.h"

#include "collide/obb.h"
#include "geomUtil/LineToLine.h"
#include "EngineHelper/drawutil.h"

float PointToSegmentDistanceSquared( Vector3CRef origin, Vector3CRef direction, float start, float end,
	Vector3CRef point, float &o_t, Vector3 &o_p )
{
	o_t = PointToLine_TOnly( origin, direction, point );
	if( o_t < start )
		o_t = start;
	else if( o_t > end )
		o_t = end;

	o_p = origin + direction * o_t;
	return (point - o_p).LengthSquared();
}

// for those of us who don't give a shit about when or where
float PointToSegmentDistanceSquared( Vector3CRef origin, Vector3CRef direction, float start, float end,
	Vector3CRef point )
{
	float t;
	Vector3 p;

	return PointToSegmentDistanceSquared( origin, direction, start, end, point, t, p );
}

float PointToLineDistanceSquared( Vector3CRef origin, Vector3CRef direction, Vector3CRef point,
	float &o_t, Vector3 &o_p )
{
	o_t = PointToLine_TOnly( origin, direction, point );
	o_p = origin + o_t * direction;
	return (point - o_p).LengthSquared();
}

float PointToLine_TOnly( Vector3CRef origin, Vector3CRef direction, Vector3CRef point )
{
	float lenSq = direction.LengthSquared();
	if( Math::Zero( lenSq ) )
		return 0.0f;

	return (point - origin).Dot( direction ) / lenSq;
}

void LineToLine_TOnly( Vector3CRef origin1, Vector3CRef d1,
	Vector3CRef origin2, Vector3CRef d2, float &o_t1, float &o_t2 )
{
	float D1DotD2 = d1.Dot( d2 );
	float denom = D1DotD2 * D1DotD2 - 1.0f;

	if( Math::Zero( denom ) )
	{
		// lines are parallel
		o_t1 = 0.0f;
		o_t2 = PointToLine_TOnly( origin2, d2, origin1 );
		return;
	}

	Vector3CRef O2MinusO1 = origin2 - origin1;
	float O2MinusO1DotD1 = O2MinusO1.Dot( d1 );
	float O2MinusO1DotD2 = O2MinusO1.Dot( d2 );

	o_t1 = ( ( D1DotD2 * O2MinusO1DotD2 ) - O2MinusO1DotD1 ) / denom;
	o_t2 = ( O2MinusO1DotD2 - ( D1DotD2 * O2MinusO1DotD1 ) ) / denom;
}
		
float LineToLineDistanceSquared( Vector3CRef origin1, Vector3CRef direction1,
	Vector3CRef origin2, Vector3CRef direction2, float &o_t1, float &o_t2,
	Vector3 &o_p1, Vector3 &o_p2)
{
	LineToLine_TOnly( origin1, direction1, origin2, direction2, o_t1, o_t2 );
	o_p1 = origin1 + o_t1 * direction1;
	o_p2 = origin2 + o_t2 * direction2;

	return ( o_p2 - o_p1 ).LengthSquared();
}

//#define DEBUG_SEG2SEG_DRAW

float SegmentToSegmentDistanceSquared( Vector3CRef origin1, Vector3CRef direction1, float start1, float end1,
	Vector3CRef origin2, Vector3CRef direction2, float start2, float end2,
	float &o_t1, float &o_t2, Vector3 &o_p1, Vector3 &o_p2 )
{
	float t;
	if( end1 < start1 )
	{
		t = end1;
		end1 = start1;
		start1 = t;
	}
	if( end2 < start2 )
	{
		t = end2;
		end2 = start2;
		start2 = t;
	}
	LineToLine_TOnly( origin1, direction1, origin2, direction2, o_t1, o_t2 );

	float ratio = direction1.Dot( direction2 );

	if( Math::Zero( ratio ) )
	{
		// lines are perpendicular.  all we need to do is clamp both ends on each
		if( o_t1 < start1 )
			o_t1 = start1;
		else if( o_t1 > end1 )
			o_t1 = end1;
		if( o_t2 < start2 )
			o_t2 = start2;
		else if( o_t2 > end2 )
			o_t2 = end2;
	}
	else
	{
		#ifdef DEBUG_SEG2SEG_DRAW
			DrawUtility::Point( origin1 + o_t1 * direction1, DrawUtility::MAGENTA, 0.1f );
			DrawUtility::Point( origin2 + o_t2 * direction2, DrawUtility::CYAN, 0.1f );
		#endif

		// now that we have the closest point along the two infinite lines, figure out where
		// the closest point lies on the first segment.
		float off, clampStart = 0.0f, clampEnd = 0.0f, offStart = 0.0f, offEnd = 0.0f;
		if( o_t1 < start1 )
		{
			// closest point is on the end
			clampStart = (start1 - o_t1);
			o_t1 = start1;
			o_t2 = PointToLine_TOnly( origin2, direction2, origin1 );
		}
		else if( o_t1 > end1 )
		{
			clampEnd = (end1 - o_t1);
			o_t1 = end1;
			o_t2 = PointToLine_TOnly( origin2, direction2, origin1 + direction1 * end1 );
		}
		else
		{
			// closest point lies inside segment 1.  We already know the relevant times, so
			// we don't have anything to do here.
		}

		#ifdef DEBUG_SEG2SEG_DRAW
			DrawUtility::Point( origin1 + o_t1 * direction1, DrawUtility::RED, 0.1f );
			DrawUtility::Point( origin2 + o_t2 * direction2, DrawUtility::GREEN, 0.1f );
		#endif

		// now clamp to the second segment
		off = 0.0f;
		if( o_t2 < start2 )
		{
			off = start2 - o_t2;
			o_t2 = start2;
		}
		else if( o_t2 > end2 )
		{
			off = end2 - o_t2;
			o_t2 = end2;
		}

		// apply that offset back to the first segment, and reclamp

		// clamping here is a little bizzare, since when we clamp to the end of
		// one capsule, we want to push the end of the other one an equivalent amount.
		// but, after capping one end, we don't want to let another bump push it back out,
		// or push further along the axis than the original clamp did
		if( !Math::Zero( off ) )
		{
			float tmp = off * ratio;
			if( tmp > 0.0f )
			{
				tmp = (off + clampStart * ratio) * ratio;
				if( tmp > clampStart )
				{
					o_t1 += tmp - clampStart;
					if( o_t1 > end1 )
						o_t1 = end1;
				}
			}
			else
			{
				tmp = (off + clampEnd * ratio) * ratio;
				if( tmp < clampEnd )
				{
					o_t1 += tmp - clampEnd;
					if( o_t1 < start1 )
						o_t1 = start1;
				}
			}
		}
	}

	o_p1 = origin1 + o_t1 * direction1;
	o_p2 = origin2 + o_t2 * direction2;

	#ifdef DEBUG_SEG2SEG_DRAW
		DrawUtility::Point( o_p1, DrawUtility::WHITE, 0.1f );
		DrawUtility::Point( o_p2, DrawUtility::YELLOW, 0.1f );
		DrawUtility::Line( o_p1, o_p2, DrawUtility::CYAN );
	#endif

	return ( o_p2 - o_p1 ).LengthSquared();
}

// for those of us who don't care about the extra shit
float SegmentToSegmentDistanceSquared( Vector3CRef origin1, Vector3CRef direction1, float start1, float end1,
	Vector3CRef origin2, Vector3CRef direction2, float start2, float end2 )
{
	Vector3 p1, p2;
	float t1, t2;
	return SegmentToSegmentDistanceSquared( origin1, direction1, start1, end1, origin2, direction2, start2, end2,
			t1, t2, p1, p2 );
}

bool LineToPlaneIntersection( Vector3CRef LOrigin, Vector3CRef LDirection, 
	Vector3CRef PNormal, Vector3CRef POrigin, float &o_t, float &o_LNRatio )
{
	o_LNRatio = LDirection.Dot( PNormal );
	float LMinusPDotNorm = (LOrigin - POrigin).Dot( PNormal );

	if( Math::Zero( o_LNRatio, 1e-4f ) ) // plane and line are parallel
	{
		o_t = 0.0f;
		// does the line lie in the plane?
		if( Math::Zero( LMinusPDotNorm ) )
			return true;
		else
			return false;
	}

	o_t = - LMinusPDotNorm / o_LNRatio;
	return true;
}

// This looks a lot nastier than it really is.  As far as I can tell, this algorithm, along
// with my line-to-line distance algorithm, is less math and MUCH less code than every other
// segment-to-triangle algorithm I've seen.  line-to-triangle does require a couple square roots,
// but according to those in the know, a square root on the ps2 is faster than a dot product.

// As a point of comparison, most segment-to-triangle alorithms require math equivalent to
// 18-ish dot products before they can even determine which region the solution lies in,
// enough branches to determine which of 21 regions the solution lies in, and then additional
// math (of varying amounts) for each region.  In addion, the case where the line is parallel
// to the plane of the triangle is special-cased, and requires finding the distance from the line
// to each of the 3 edges of the triangle, and using the minimum of those 3 distances.  If you really
// want to see how ugly it can get, take a look at http://www.magic-software.com/Distance.html at the
// line-triangle distance code.

// As with the line-to-line, the trick is to find the intersection point between the infinite
// features, and then clamp to the float extents of each feature, making sure we adjust the
// closest point on the other while we clamp.

// It works because if the intersection point did lie on both features, it would be the closest point,
// so we can work backwards from that point as long as we're careful to move corresponding amounts 
// on each feature while we clamp, and as long as we're careful not to undo any movement that we 
// did by clamping to the first, when clamping to the second.

// If you really need to know how this works, talk to Travis.  It makes much more sense on a whiteboard.

//#define DEBUG_SEG2TRI_DRAW

// e0 and e1 are 2 adjacent edges of the triangle, both of which start at TOrigin.
// We also assume that e0 and e1 are the full length of the edge, rather than just being the edge direction
float SegmentToTriangleDistanceSquared( Vector3CRef LOrigin, Vector3CRef LDirection, float LStart, float LEnd,
	Vector3CRef TNormal, Vector3CRef TOrigin, Vector3CRef E0, Vector3CRef E1, Vector3 &o_LP, Vector3 &o_TP )
{
	// get the intersection of the line versus the plane that the triangle is in
	float LNRatio;
	float s, t, d;

	// yes, I know this sucks, but we really have to get the length of these guys.
	// it might be possible to make this all work with non-normalized edge vectors,
	// but my brain nearly implodes when trying to think about all of this even 
	// with normalized edges, which simplifies a lot of things. :(
	Vector3 E0Dir, E1Dir;
	float E0Len, E1Len;

	E0Len = E0.Length();
	if( Math::Zero( E0Len ) )
		E0Dir.Clear();
	else
		E0Dir = E0 / E0Len;

	E1Len = E1.Length();
	if( Math::Zero( E1Len ) )
		E1Dir.Clear();
	else
		E1Dir = E1 / E1Len;

	float E0DotE1 = E0Dir.Dot( E1Dir );
	float LDotE0 = LDirection.Dot( E0Dir );
	float LDotE1 = LDirection.Dot( E1Dir );

	// figure out the closest point from the segment to the plane of the triangle.
	bool intersects = LineToPlaneIntersection( LOrigin, LDirection, TNormal, TOrigin, d, LNRatio );
	Vector3 closestPointOnPlane;
	float clampStart = 0.0f, clampEnd = 0.0f;
	if( d < LStart )
	{
		// the start is closest.  project it down to the plane to find
		// the point on the plane that is closest
		closestPointOnPlane = LOrigin;
		closestPointOnPlane -= TNormal * (closestPointOnPlane - TOrigin).Dot( TNormal );
		clampStart = LStart - d;
		clampStart = 0.0f;
		d = LStart;
	}
	else if( d > LEnd || !intersects )
	{
		// the end is closest.  project it down to the plane to find
		// the point on the plane that is closest
		closestPointOnPlane = LOrigin + LDirection * LEnd;
		closestPointOnPlane -= TNormal * (closestPointOnPlane - TOrigin).Dot( TNormal );
		clampEnd = LStart - d;
		clampEnd = 0.0f;
		d = LEnd;
	}
	else
	{
		// the intersection point is closest
		closestPointOnPlane = LOrigin + LDirection * d;
	}

	// figure out where we are along e0 and e1.
	Vector3 iMinusT = closestPointOnPlane - TOrigin;

	Vector3 denom = E0Dir * E0DotE1 - E1Dir;
	Vector3 num = E0Dir * iMinusT.Dot( E0Dir ) - iMinusT;

	t = Math::Sqrt( num.LengthSquared() / denom.LengthSquared() );
	if( num.Dot( denom ) < 0.0f )
		t = -t;
	s = E0Dir.Dot( iMinusT ) - t * E0DotE1;

	#ifdef DEBUG_SEG2TRI_DRAW
		DrawUtility::Line( TOrigin, TOrigin + E0, DrawUtility::RED );
		DrawUtility::Line( TOrigin, TOrigin + E1, DrawUtility::BLUE );
		DrawUtility::Point( closestPointOnPlane, DrawUtility::WHITE, 0.1f );
	#endif

	// now, d is the point along the line of the intersection between the line and the plane,
	// and s,t are the points along edge 0 and edge 1 of that intersection
	
	// we're already clamped to the line, so we don't need to do that.  Just clamp to the triangle
	float off, sOff, tOff;

	sOff = 0.0f;
	tOff = 0.0f;

	if( s * E1Len + t * E0Len > E0Len * E1Len )
	{
		if( t < 0.0f ) // region 6
		{
			sOff = E0Len - s;
			tOff = -t;
		}
		else if( s < 0.0f ) // region 2
		{
			sOff = -s;
			tOff = E1Len - t;
		}
		else // region 1
		{
			// I know there's a way to reduce the math here, given what
			// we've already figured out, but I'll be damned if I can determine what it is.
			// so, this is a copy of region 1 code from a normal point-to-triangle distance
			// formula.  :(

			float s0, t0;
			float c, d, b, e;
			c = E1.LengthSquared();
			d = -E0.Dot( iMinusT );
			b = E0.Dot( E1 );
			e = -E1.Dot( iMinusT );
	
			float num = c + e - b - d;
			if( num <= 0 )
			{
				s0 = 0;
			}
			else
			{
				float a = E0.LengthSquared();
				float denom = a - 2 * b + c;
				s0 = (num >= denom ? 1 : num / denom);
			}
			t0 = 1 - s0;
			s0 *= E0Len;
			t0 *= E1Len;
			
			sOff = s0 - s;
			tOff = t0 - t;
		}
	}
	else
	{
		if( t < 0.0f )
		{
			if( s < 0.0f ) // region 4
			{
				sOff = -s;
				tOff = -t;
			}
			else // region 5
			{
				tOff = -t;
				sOff = -tOff * E0DotE1;
				if( s + sOff > E0Len )
					sOff = E0Len - s;
				else if( s + sOff < 0.0f )
					sOff = -s;
			}
		}
		else if( s < 0.0f ) // region 3
		{
			sOff = -s;
			tOff = -sOff * E0DotE1;
			if( t + tOff > E1Len )
				tOff = E1Len - t;
			else if( t + tOff < 0.0f )
				tOff = -t;
		}
		else // region 0, already in
		{
		}
	}

	s += sOff;
	t += tOff;

	// apply these offsets back to the line, and clamp
	// see segmenttosegment for an explanation of the bizarre clamping behavior
	off = sOff * LDotE0 + tOff * LDotE1;
	if( !Math::Zero( off ) )
	{
		if( off > 0.0f )
		{
			off = (sOff + clampStart * LDotE0) * LDotE0;
			off += (tOff + clampStart * LDotE1) * LDotE1;
			if( off > clampStart )
			{
				off = off - clampStart;
				if( d + off > LEnd )
					off = LEnd - d;
				d += off;
			}
		}
		else
		{
			off = (sOff + clampEnd * LDotE0) * LDotE0;
			off += (tOff + clampEnd * LDotE1) * LDotE1;
			if( off < clampEnd )
			{
				off = off - clampEnd;
				if( d + off < LStart )
					off = LStart - d;
				d += off;
			}
		}
	}

	// holy shit I will be so amazed if this actually works.
	o_LP = LOrigin + LDirection * d;
	o_TP = TOrigin + E0Dir * s + E1Dir * t;

	#ifdef DEBUG_SEG2TRI_DRAW
		DrawUtility::Point( o_LP, DrawUtility::MAGENTA, 0.1f );
		DrawUtility::Point( o_TP, DrawUtility::CYAN, 0.1f );
		DrawUtility::Line( o_LP, o_TP, DrawUtility::CYAN );
	#endif

	return (o_LP - o_TP).LengthSquared();
}

float PointToOBBDistanceSquared_PointInOBB( Vector3CRef pPrime, const OBB &obb, Vector3 &qPrime )
{
	Vector3 max = obb.GetConstAABB().maxV;

	// Project pPrime onto box
	float distanceSquared = 0.0f;
	float d;

	if( pPrime.x() < -max.x() )
	{
		d = pPrime.x() + max.x();
		distanceSquared += d * d;
		qPrime.x( -max.x() );
	}
	else if( pPrime.x() > max.x() )
	{
		d = pPrime.x() - max.x();
		distanceSquared += d * d;
		qPrime.x( max.x() );
	}
	else
		qPrime.x( pPrime.x() );

	if( pPrime.y() < -max.y() )
	{
		d = pPrime.y() + max.y();
		distanceSquared += d * d;
		qPrime.y( -max.y() );
	}
	else if( pPrime.y() > max.y() )
	{
		d = pPrime.y() - max.y();
		distanceSquared += d * d;
		qPrime.y( max.y() );
	}
	else
		qPrime.y( pPrime.y() );

	if( pPrime.z() < -max.z() )
	{
		d = pPrime.z() + max.z();
		distanceSquared += d * d;
		qPrime.z( -max.z() );
	}
	else if( pPrime.z() > max.z() )
	{
		d = pPrime.z() - max.z();
		distanceSquared += d * d;
		qPrime.z( max.z() );
	}
	else
		qPrime.z( pPrime.z() );
	
	return distanceSquared;
}

float PointToOBBDistanceSquared( Vector3CRef point, const OBB &obb, Vector3 &o_BP )
{
	// From "Geometric Tools For Computer Graphics" (Schneider & Eberly)
	// Section 10.4.2, Point to Oriented Bounding Box

	Vector3 ax = obb.Matrix()->GetRow0();
	Vector3 ay = obb.Matrix()->GetRow1();
	Vector3 az = obb.Matrix()->GetRow2();

	// Transform p into box's coordinate frame
	Vector3 offset = point - *(obb.Center());
	Vector3 pPrime( offset.Dot( ax ), offset.Dot( ay ), offset.Dot( az ) );
	Vector3 qPrime;

	float distanceSquared = PointToOBBDistanceSquared_PointInOBB( pPrime, obb, qPrime );

	o_BP.x( qPrime.x() * ax.x() + qPrime.y() * ay.x() + qPrime.z() * az.x() );
	o_BP.y( qPrime.x() * ax.y() + qPrime.y() * ay.y() + qPrime.z() * az.y() );
	o_BP.z( qPrime.x() * ax.z() + qPrime.y() * ay.z() + qPrime.z() * az.z() );

	o_BP += *(obb.Center());

	return distanceSquared;
}

float FaceX( Vector3CRef pPrime, Vector3CRef dPrime, Vector3CRef ptMinusExtents, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	qPrime = pPrime;
	float distanceSquared = 0.0f;
	Vector3 max = obb.GetConstAABB().maxV;
	Vector3 ptPlusExtents = pPrime + max;
	float inverse, lSqr, tmp, delta, ft;

	if( dPrime.x() * ptPlusExtents.y() >= dPrime.y() * ptMinusExtents.x() )
	{
		// region 0, 5, or 4
		if( dPrime.x() * ptPlusExtents.z() >= dPrime.z() * ptMinusExtents.x() )
		{
			// region 0 - line intersects face
			qPrime.x( max.x() );
			inverse = 1.0f / dPrime.x();
			qPrime.y( qPrime.y() - dPrime.y() * ptMinusExtents.x() * inverse );
			qPrime.z( qPrime.z() - dPrime.z() * ptMinusExtents.x() * inverse );
			o_t = -ptMinusExtents.x() * inverse;
		}
		else
		{
			// region 4 or 5
			lSqr = dPrime.x() * dPrime.x() + dPrime.z() * dPrime.z();
			tmp = lSqr * ptPlusExtents.y() - dPrime.y() * 
				(dPrime.x() * ptMinusExtents.x() + dPrime.z() * ptPlusExtents.z());
			if( tmp <= 2.0f * lSqr * max.y() )
			{
				// region 4 - this code is duplicated lower, be sure to change both spots
				ft = tmp / lSqr;
				lSqr += dPrime.y() * dPrime.y();
				tmp = ptPlusExtents.y() - ft;
				delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * tmp + dPrime.z() * ptPlusExtents.z();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() + tmp * tmp +
					ptPlusExtents.z() * ptPlusExtents.z() + delta * o_t;

				qPrime.x( max.x() );
				qPrime.y( ft - max.y() );
				qPrime.z( -max.z() );
			}
			else
			{
				// region 5 - this code is duplicated lower, be sure to change both spots
				lSqr += dPrime.y() * dPrime.y();
				delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptMinusExtents.y() +
					dPrime.z() * ptPlusExtents.z();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() +
					ptMinusExtents.y() * ptMinusExtents.y() + ptPlusExtents.z() * ptPlusExtents.z() +
					delta * o_t;

				qPrime.x( max.x() );
				qPrime.y( max.y() );
				qPrime.z( -max.z());
			}
		}
	}
	else
	{
		if( dPrime.x() * ptPlusExtents.z() >= dPrime.z() * ptMinusExtents.x() )
		{
			// region 1 or 2
			lSqr = dPrime.x() * dPrime.x() + dPrime.y() * dPrime.y();
			tmp = lSqr * ptPlusExtents.z() - dPrime.z() * 
				(dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptPlusExtents.y());
			if( tmp <= 2.0f * lSqr * max.z() )
			{
				// region 2 - this code is duplicated lower, be sure to change both spots
				ft = tmp / lSqr;
				lSqr += dPrime.z() * dPrime.z();
				tmp = ptPlusExtents.z() - ft;
				delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptPlusExtents.y() + dPrime.z() * tmp;
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() + 
					ptPlusExtents.y() * ptPlusExtents.y() + tmp * tmp + delta * o_t;

				qPrime.x( max.x() );
				qPrime.y( -max.y() );
				qPrime.z( ft - max.z() );
			}
			else
			{
				// region 1 - this code is duplicated lower, be sure to change both spots
				lSqr += dPrime.z() * dPrime.z();
				delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptMinusExtents.y() +
					dPrime.z() * ptPlusExtents.z();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() +
					ptPlusExtents.y() * ptPlusExtents.y() + ptMinusExtents.z() * ptMinusExtents.z() +
					delta * o_t;

				qPrime.x( max.x() );
				qPrime.y( -max.y() );
				qPrime.z( max.z() );
			}
		}
		else
		{
			lSqr = dPrime.x() * dPrime.x() + dPrime.z() * dPrime.z();
			tmp = lSqr * ptPlusExtents.y() - dPrime.y() * 
				(dPrime.x() * ptMinusExtents.x() + dPrime.z() * ptPlusExtents.z());
			if( tmp >= 0.0f )
			{
				// region 4 or 5
				if( tmp <= 2.0f * lSqr * max.y() )
				{
					// region 4.  same as code above for region 4
					ft = tmp / lSqr;
					lSqr += dPrime.y() * dPrime.y();
					tmp = ptPlusExtents.y() - ft;
					delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * tmp + dPrime.z() * ptPlusExtents.z();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() + tmp * tmp +
						ptPlusExtents.z() * ptPlusExtents.z() + delta * o_t;

					qPrime.x( max.x() );
					qPrime.y( ft - max.y() );
					qPrime.z( -max.z() );
				}
				else
				{
					// region 5.  same as code above for region 5
					lSqr += dPrime.y() * dPrime.y();
					delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptMinusExtents.y() +
						dPrime.z() * ptPlusExtents.z();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() +
						ptMinusExtents.y() * ptMinusExtents.y() + ptPlusExtents.z() * ptPlusExtents.z() +
						delta * o_t;

					qPrime.x( max.x() );
					qPrime.y( max.y() );
					qPrime.z( -max.z());
				}
				return distanceSquared;
			}
			lSqr = dPrime.x() * dPrime.x() + dPrime.y() * dPrime.y();
			tmp = lSqr * ptPlusExtents.z() - dPrime.z() *
				(dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptPlusExtents.y());
			if( tmp >= 0.0f )
			{
				// region 1 or 2
				if( tmp <= 2.0f * lSqr * max.z() )
				{
					// region 2.  same as code above for region 2
					ft = tmp / lSqr;
					tmp = ptPlusExtents.z() - (tmp / lSqr);
					lSqr += dPrime.z() * dPrime.z();
					delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptPlusExtents.y() + dPrime.z() * tmp;
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() + 
						ptPlusExtents.y() * ptPlusExtents.y() + tmp * tmp + delta * o_t;

					qPrime.x( max.x() );
					qPrime.y( -max.y() );
					qPrime.z( ft - max.z() );
				}
				else
				{
					// region 1.  same as code above for region 1
					lSqr += dPrime.z() * dPrime.z();
					delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptMinusExtents.y() +
						dPrime.z() * ptPlusExtents.z();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() +
						ptPlusExtents.y() * ptPlusExtents.y() + ptMinusExtents.z() * ptMinusExtents.z() +
						delta * o_t;

					qPrime.x( max.x() );
					qPrime.y( -max.y() );
					qPrime.z( max.z() );
				}
				return distanceSquared;
			}
			
			// region 3
			lSqr += dPrime.y() * dPrime.y();
			delta = dPrime.x() * ptMinusExtents.x() + dPrime.y() * ptPlusExtents.y() +
				dPrime.z() * ptPlusExtents.z();
			o_t = -delta / lSqr;
			distanceSquared += ptMinusExtents.x() * ptMinusExtents.x() +
				ptPlusExtents.y() * ptPlusExtents.y() + ptPlusExtents.z() * ptPlusExtents.z() +
				delta * o_t;
			qPrime.x( max.x() );
			qPrime.y( -max.y() );
			qPrime.z( -max.z() );
		}
	}
	return distanceSquared;
}

float FaceY( Vector3CRef pPrime, Vector3CRef dPrime, Vector3CRef ptMinusExtents, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	qPrime = pPrime;
	float distanceSquared = 0.0f;
	Vector3 max = obb.GetConstAABB().maxV;
	Vector3 ptPlusExtents = pPrime + max;
	float inverse, lSqr, tmp, delta, ft;

	if( dPrime.y() * ptPlusExtents.z() >= dPrime.z() * ptMinusExtents.y() )
	{
		// region 0, 5, or 4
		if( dPrime.y() * ptPlusExtents.x() >= dPrime.x() * ptMinusExtents.y() )
		{
			// region 0 - line intersects face
			qPrime.y( max.y() );
			inverse = 1.0f / dPrime.y();
			qPrime.z( qPrime.z() - dPrime.z() * ptMinusExtents.y() * inverse );
			qPrime.x( qPrime.x() - dPrime.x() * ptMinusExtents.y() * inverse );
			o_t = -ptMinusExtents.y() * inverse;
		}
		else
		{
			// region 4 or 5
			lSqr = dPrime.y() * dPrime.y() + dPrime.x() * dPrime.x();
			tmp = lSqr * ptPlusExtents.z() - dPrime.z() * 
				(dPrime.y() * ptMinusExtents.y() + dPrime.x() * ptPlusExtents.x());
			if( tmp <= 2.0f * lSqr * max.z() )
			{
				// region 4 - this code is duplicated lower, be sure to change both spots
				ft = tmp / lSqr;
				tmp = ptPlusExtents.z() - (tmp / lSqr);
				lSqr += dPrime.z() * dPrime.z();
				delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * tmp + dPrime.x() * ptPlusExtents.x();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() + tmp * tmp +
					ptPlusExtents.x() * ptPlusExtents.x() + delta * o_t;

				qPrime.y( max.y() );
				qPrime.z( ft - max.z() );
				qPrime.x( -max.x() );
			}
			else
			{
				// region 5 - this code is duplicated lower, be sure to change both spots
				lSqr += dPrime.z() * dPrime.z();
				delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptMinusExtents.z() +
					dPrime.x() * ptPlusExtents.x();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() +
					ptMinusExtents.z() * ptMinusExtents.z() + ptPlusExtents.x() * ptPlusExtents.x() +
					delta * o_t;

				qPrime.y( max.y() );
				qPrime.z( max.z() );
				qPrime.x( -max.x());
			}
		}
	}
	else
	{
		if( dPrime.y() * ptPlusExtents.x() >= dPrime.x() * ptMinusExtents.y() )
		{
			// region 1 or 2
			lSqr = dPrime.y() * dPrime.y() + dPrime.z() * dPrime.z();
			tmp = lSqr * ptPlusExtents.x() - dPrime.x() * 
				(dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptPlusExtents.z());
			if( tmp <= 2.0f * lSqr * max.x() )
			{
				// region 2 - this code is duplicated lower, be sure to change both spots
				ft = tmp / lSqr;
				tmp = ptPlusExtents.x() - (tmp / lSqr);
				lSqr += dPrime.x() * dPrime.x();
				delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptPlusExtents.z() + dPrime.x() * tmp;
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() + 
					ptPlusExtents.z() * ptPlusExtents.z() + tmp * tmp + delta * o_t;

				qPrime.y( max.y() );
				qPrime.z( -max.z() );
				qPrime.x( ft - max.x() );
			}
			else
			{
				// region 1 - this code is duplicated lower, be sure to change both spots
				lSqr += dPrime.x() * dPrime.x();
				delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptMinusExtents.z() +
					dPrime.x() * ptPlusExtents.x();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() +
					ptPlusExtents.z() * ptPlusExtents.z() + ptMinusExtents.x() * ptMinusExtents.x() +
					delta * o_t;

				qPrime.y( max.y() );
				qPrime.z( -max.z() );
				qPrime.x( max.x() );
			}
		}
		else
		{
			lSqr = dPrime.y() * dPrime.y() + dPrime.x() * dPrime.x();
			tmp = lSqr * ptPlusExtents.z() - dPrime.z() * 
				(dPrime.y() * ptMinusExtents.y() + dPrime.x() * ptPlusExtents.x());
			if( tmp >= 0.0f )
			{
				// region 4 or 5
				if( tmp <= 2.0f * lSqr * max.z() )
				{
					// region 4.  same as code above for region 4
					ft = tmp / lSqr;
					tmp = ptPlusExtents.z() - (tmp / lSqr);
					lSqr += dPrime.z() * dPrime.z();
					delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * tmp + dPrime.x() * ptPlusExtents.x();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() + tmp * tmp +
						ptPlusExtents.x() * ptPlusExtents.x() + delta * o_t;

					qPrime.y( max.y() );
					qPrime.z( ft - max.z() );
					qPrime.x( -max.x() );
				}
				else
				{
					// region 5.  same as code above for region 5
					lSqr += dPrime.z() * dPrime.z();
					delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptMinusExtents.z() +
						dPrime.x() * ptPlusExtents.x();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() +
						ptMinusExtents.z() * ptMinusExtents.z() + ptPlusExtents.x() * ptPlusExtents.x() +
						delta * o_t;

					qPrime.y( max.y() );
					qPrime.z( max.z() );
					qPrime.x( -max.x());
				}
				return distanceSquared;
			}
			lSqr = dPrime.y() * dPrime.y() + dPrime.z() * dPrime.z();
			tmp = lSqr * ptPlusExtents.x() - dPrime.x() *
				(dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptPlusExtents.z());
			if( tmp >= 0.0f )
			{
				// region 1 or 2
				if( tmp <= 2.0f * lSqr * max.x() )
				{
					// region 2.  same as code above for region 2
					ft = tmp / lSqr;
					tmp = ptPlusExtents.x() - (tmp / lSqr);
					lSqr += dPrime.x() * dPrime.x();
					delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptPlusExtents.z() + dPrime.x() * tmp;
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() + 
						ptPlusExtents.z() * ptPlusExtents.z() + tmp * tmp + delta * o_t;

					qPrime.y( max.y() );
					qPrime.z( -max.z() );
					qPrime.x( ft - max.x() );
				}
				else
				{
					// region 1.  same as code above for region 1
					lSqr += dPrime.x() * dPrime.x();
					delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptMinusExtents.z() +
						dPrime.x() * ptPlusExtents.x();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() +
						ptPlusExtents.z() * ptPlusExtents.z() + ptMinusExtents.x() * ptMinusExtents.x() +
						delta * o_t;

					qPrime.y( max.y() );
					qPrime.z( -max.z() );
					qPrime.x( max.x() );
				}
				return distanceSquared;
			}

			// region 3
			lSqr += dPrime.z() * dPrime.z();
			delta = dPrime.y() * ptMinusExtents.y() + dPrime.z() * ptPlusExtents.z() +
				dPrime.x() * ptPlusExtents.x();
			o_t = -delta / lSqr;
			distanceSquared += ptMinusExtents.y() * ptMinusExtents.y() +
				ptPlusExtents.z() * ptPlusExtents.z() + ptPlusExtents.x() * ptPlusExtents.x() +
				delta * o_t;
			qPrime.y( max.y() );
			qPrime.z( -max.z() );
			qPrime.x( -max.x() );
		}
	}
	return distanceSquared;
}

float FaceZ( Vector3CRef pPrime, Vector3CRef dPrime, Vector3CRef ptMinusExtents, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	qPrime = pPrime;
	float distanceSquared = 0.0f;
	Vector3 max = obb.GetConstAABB().maxV;
	Vector3 ptPlusExtents = pPrime + max;
	float inverse, lSqr, tmp, delta, ft;

	if( dPrime.z() * ptPlusExtents.x() >= dPrime.x() * ptMinusExtents.z() )
	{
		// region 0, 5, or 4
		if( dPrime.z() * ptPlusExtents.y() >= dPrime.y() * ptMinusExtents.z() )
		{
			// region 0 - line intersects face
			qPrime.z( max.z() );
			inverse = 1.0f / dPrime.z();
			qPrime.x( qPrime.x() - dPrime.x() * ptMinusExtents.z() * inverse );
			qPrime.y( qPrime.y() - dPrime.y() * ptMinusExtents.z() * inverse );
			o_t = -ptMinusExtents.z() * inverse;
		}
		else
		{
			// region 4 or 5
			lSqr = dPrime.z() * dPrime.z() + dPrime.y() * dPrime.y();
			tmp = lSqr * ptPlusExtents.x() - dPrime.x() * 
				(dPrime.z() * ptMinusExtents.z() + dPrime.y() * ptPlusExtents.y());
			if( tmp <= 2.0f * lSqr * max.x() )
			{
				// region 4 - this code is duplicated lower, be sure to change both spots
				ft = tmp / lSqr;
				tmp = ptPlusExtents.x() - (tmp / lSqr);
				lSqr += dPrime.x() * dPrime.x();
				delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * tmp + dPrime.y() * ptPlusExtents.y();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() + tmp * tmp +
					ptPlusExtents.y() * ptPlusExtents.y() + delta * o_t;

				qPrime.z( max.z() );
				qPrime.x( ft - max.x() );
				qPrime.y( -max.y() );
			}
			else
			{
				// region 5 - this code is duplicated lower, be sure to change both spots
				lSqr += dPrime.x() * dPrime.x();
				delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptMinusExtents.x() +
					dPrime.y() * ptPlusExtents.y();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() +
					ptMinusExtents.x() * ptMinusExtents.x() + ptPlusExtents.y() * ptPlusExtents.y() +
					delta * o_t;

				qPrime.z( max.z() );
				qPrime.x( max.x() );
				qPrime.y( -max.y());
			}
		}
	}
	else
	{
		if( dPrime.z() * ptPlusExtents.y() >= dPrime.y() * ptMinusExtents.z() )
		{
			// region 1 or 2
			lSqr = dPrime.z() * dPrime.z() + dPrime.x() * dPrime.x();
			tmp = lSqr * ptPlusExtents.y() - dPrime.y() * 
				(dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptPlusExtents.x());
			if( tmp <= 2.0f * lSqr * max.y() )
			{
				// region 2 - this code is duplicated lower, be sure to change both spots
				ft = tmp / lSqr;
				tmp = ptPlusExtents.y() - (tmp / lSqr);
				lSqr += dPrime.y() * dPrime.y();
				delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptPlusExtents.x() + dPrime.y() * tmp;
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() + 
					ptPlusExtents.x() * ptPlusExtents.x() + tmp * tmp + delta * o_t;

				qPrime.z( max.z() );
				qPrime.x( -max.x() );
				qPrime.y( ft - max.y() );
			}
			else
			{
				// region 1 - this code is duplicated lower, be sure to change both spots
				lSqr += dPrime.y() * dPrime.y();
				delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptMinusExtents.x() +
					dPrime.y() * ptPlusExtents.y();
				o_t = -delta / lSqr;
				distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() +
					ptPlusExtents.x() * ptPlusExtents.x() + ptMinusExtents.y() * ptMinusExtents.y() +
					delta * o_t;

				qPrime.z( max.z() );
				qPrime.x( -max.x() );
				qPrime.y( max.y() );
			}
		}
		else
		{
			lSqr = dPrime.z() * dPrime.z() + dPrime.y() * dPrime.y();
			tmp = lSqr * ptPlusExtents.x() - dPrime.x() * 
				(dPrime.z() * ptMinusExtents.z() + dPrime.y() * ptPlusExtents.y());
			if( tmp >= 0.0f )
			{
				// region 4 or 5
				if( tmp <= 2.0f * lSqr * max.x() )
				{
					// region 4.  same as code above for region 4
					ft = tmp / lSqr;
					tmp = ptPlusExtents.x() - (tmp / lSqr);
					lSqr += dPrime.x() * dPrime.x();
					delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * tmp + dPrime.y() * ptPlusExtents.y();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() + tmp * tmp +
						ptPlusExtents.y() * ptPlusExtents.y() + delta * o_t;

					qPrime.z( max.z() );
					qPrime.x( ft - max.x() );
					qPrime.y( -max.y() );
				}
				else
				{
					// region 5.  same as code above for region 5
					lSqr += dPrime.x() * dPrime.x();
					delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptMinusExtents.x() +
						dPrime.y() * ptPlusExtents.y();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() +
						ptMinusExtents.x() * ptMinusExtents.x() + ptPlusExtents.y() * ptPlusExtents.y() +
						delta * o_t;

					qPrime.z( max.z() );
					qPrime.x( max.x() );
					qPrime.y( -max.y());
				}
				return distanceSquared;
			}
			lSqr = dPrime.z() * dPrime.z() + dPrime.x() * dPrime.x();
			tmp = lSqr * ptPlusExtents.y() - dPrime.y() *
				(dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptPlusExtents.x());
			if( tmp >= 0.0f )
			{
				// region 1 or 2
				if( tmp <= 2.0f * lSqr * max.y() )
				{
					// region 2.  same as code above for region 2
					ft = tmp / lSqr;
					tmp = ptPlusExtents.y() - (tmp / lSqr);
					lSqr += dPrime.y() * dPrime.y();
					delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptPlusExtents.x() + dPrime.y() * tmp;
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() + 
						ptPlusExtents.x() * ptPlusExtents.x() + tmp * tmp + delta * o_t;

					qPrime.z( max.z() );
					qPrime.x( -max.x() );
					qPrime.y( ft - max.y() );
				}
				else
				{
					// region 1.  same as code above for region 1
					lSqr += dPrime.y() * dPrime.y();
					delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptMinusExtents.x() +
						dPrime.y() * ptPlusExtents.y();
					o_t = -delta / lSqr;
					distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() +
						ptPlusExtents.x() * ptPlusExtents.x() + ptMinusExtents.y() * ptMinusExtents.y() +
						delta * o_t;

					qPrime.z( max.z() );
					qPrime.x( -max.x() );
					qPrime.y( max.y() );
				}
				return distanceSquared;
			}

			// region 3
			lSqr += dPrime.x() * dPrime.x();
			delta = dPrime.z() * ptMinusExtents.z() + dPrime.x() * ptPlusExtents.x() +
				dPrime.y() * ptPlusExtents.y();
			o_t = -delta / lSqr;
			distanceSquared += ptMinusExtents.z() * ptMinusExtents.z() +
				ptPlusExtents.x() * ptPlusExtents.x() + ptPlusExtents.y() * ptPlusExtents.y() +
				delta * o_t;
			qPrime.z( max.z() );
			qPrime.x( -max.x() );
			qPrime.y( -max.y() );
		}
	}
	return distanceSquared;
}

float NoZeroComponents( Vector3CRef pPrime, Vector3CRef dPrime, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	float distanceSquared = 0.0f;
	Vector3 max = obb.GetConstAABB().maxV;
	Vector3 ptMinusExtents = pPrime - max;
	float dyEx = dPrime.y() * ptMinusExtents.x();
	float dxEy = dPrime.x() * ptMinusExtents.y();

	if( dyEx >= dxEy )
	{
		float dzEx = dPrime.z() * ptMinusExtents.x();
		float dxEz = dPrime.x() * ptMinusExtents.z();

		if( dzEx >= dxEz )
			distanceSquared = FaceX( pPrime, dPrime, ptMinusExtents, obb, o_t, qPrime );
		else
			distanceSquared = FaceZ( pPrime, dPrime, ptMinusExtents, obb, o_t, qPrime );
	}
	else
	{
		float dzEy = dPrime.z() * ptMinusExtents.y();
		float dyEz = dPrime.y() * ptMinusExtents.z();

		if( dzEy >= dyEz )
			distanceSquared = FaceY( pPrime, dPrime, ptMinusExtents, obb, o_t, qPrime );
		else
			distanceSquared = FaceZ( pPrime, dPrime, ptMinusExtents, obb, o_t, qPrime );
	}

	return distanceSquared;
}

float OneZeroComponent_X( Vector3CRef pPrime, Vector3CRef dPrime, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	Vector3 max = obb.GetConstAABB().maxV;
	float fPmE0 = pPrime.y() - max.y();
	float fPmE1 = pPrime.z() - max.z();
	float fProd0 = dPrime.z() * fPmE0;
	float fProd1 = dPrime.y() * fPmE1;
	float fDelta, fInvLSqr, fInv;
	float rfSqrDistance = 0.0f;

	qPrime = pPrime;

	if ( fProd0 >= fProd1 )
	{
		// line intersects P[i0] = e[i0]
		qPrime.y( max.y() );

		float fPpE1 = pPrime.z() + max.z();
		fDelta = fProd0 - dPrime.y() * fPpE1;
		if ( fDelta >= (float)0.0 )
		{
			fInvLSqr = ((float)1.0)/( dPrime.y()*dPrime.y()+dPrime.z()*dPrime.z());
			rfSqrDistance += fDelta*fDelta*fInvLSqr;
			qPrime.z( -max.z() );
			o_t = -(dPrime.y() * fPmE0 + dPrime.z() * fPpE1)*fInvLSqr;
		}
		else
		{
			fInv = ((float)1.0)/dPrime.y();
			qPrime.z( qPrime.z() - fProd0 * fInv );
			o_t = -fPmE0 * fInv;
		}
	}
	else
	{
		// line intersects P[i1] = e[i1]
		qPrime.z( max.z() );

		float fPpE0 = pPrime.y() + max.y();
		fDelta = fProd1 - dPrime.z() * fPpE0;
		if ( fDelta >= (float)0.0 )
		{
			fInvLSqr = ((float)1.0)/( dPrime.y()*dPrime.y()+dPrime.z()*dPrime.z());
			rfSqrDistance += fDelta*fDelta*fInvLSqr;
			qPrime.y( -max.y() );
			o_t = -(dPrime.y() * fPpE0 + dPrime.z() * fPmE1)*fInvLSqr;
		}
		else
		{
			fInv = ((float)1.0)/dPrime.z();
			qPrime.y( qPrime.y() - fProd1 * fInv );
			o_t = -fPmE1 * fInv;
		}
	}

	if ( pPrime.x() < -max.x() )
	{
		fDelta = pPrime.x() + max.x();
		rfSqrDistance += fDelta*fDelta;
		qPrime.x( -max.x() );
	}
	else if ( pPrime.x() > max.x() )
	{
		fDelta = pPrime.x() - max.x();
		rfSqrDistance += fDelta*fDelta;
		qPrime.x( max.x() );
	}
	return rfSqrDistance;
#if 0
	Vector3 max = obb.GetConstAABB().maxV;
	float fPmE0 = pPrime.AXIS0() - max.AXIS0();
	float fPmE1 = pPrime.AXIS1() - max.AXIS1();
	float fProd0 = dPrime.AXIS1() * fPmE0;
	float fProd1 = dPrime.AXIS0() * fPmE1;
	float fDelta, fInvLSqr, fInv;
	float rfSqrDistance = 0.0f;
	
	qPrime = pPrime;

	if ( fProd0 >= fProd1 )
	{
		// line intersects P[i0] = e[i0]
		qPrime.AXIS0( max.AXIS0() );

		float fPpE1 = pPrime.AXIS1() + max.AXIS1();
		fDelta = fProd0 - dPrime.AXIS0() * fPpE1;
		if ( fDelta >= (float)0.0 )
		{
			fInvLSqr = ((float)1.0)/( dPrime.AXIS0()*dPrime.AXIS0()+dPrime.AXIS1()*dPrime.AXIS1());
			rfSqrDistance += fDelta*fDelta*fInvLSqr;
			qPrime.AXIS1( -max.AXIS1() );
			o_t = -(dPrime.AXIS0() * fPmE0 + dPrime.AXIS1() * fPpE1)*fInvLSqr;
		}
		else
		{
			fInv = ((float)1.0)/dPrime.AXIS0();
			qPrime.AXIS1( qPrime.AXIS1() - fProd0 * fInv );
			o_t = -fPmE0 * fInv;
		}
	}
	else
	{
		// line intersects P[i1] = e[i1]
		qPrime.AXIS1( max.AXIS1() );

		float fPpE0 = pPrime.AXIS0() + max.AXIS0();
		fDelta = fProd1 - dPrime.AXIS1() * fPpE0;
		if ( fDelta >= (float)0.0 )
		{
			fInvLSqr = ((float)1.0)/( dPrime.AXIS0()*dPrime.AXIS0()+dPrime.AXIS1()*dPrime.AXIS1());
			rfSqrDistance += fDelta*fDelta*fInvLSqr;
			qPrime.AXIS0( -max.AXIS0() );
			o_t = -(dPrime.AXIS0() * fPpE0 + dPrime.AXIS1() * fPmE1)*fInvLSqr;
		}
		else
		{
			fInv = ((float)1.0)/dPrime.AXIS1();
			qPrime.AXIS0( qPrime.AXIS0() - fProd1 * fInv );
			o_t = -fPmE1 * fInv;
		}
	}

	if ( pPrime.AXIS2() < -max.AXIS2() )
	{
		fDelta = pPrime.AXIS2() + max.AXIS2();
		rfSqrDistance += fDelta*fDelta;
		qPrime.AXIS2( -max.AXIS2() );
	}
	else if ( pPrime.AXIS2() > max.AXIS2() )
	{
		fDelta = pPrime.AXIS2() - max.AXIS2();
		rfSqrDistance += fDelta*fDelta;
		qPrime.AXIS2( max.AXIS2() );
	}
	return rfSqrDistance;
#endif
#if 0
	float distanceSquared = 0.0f;
	Vector3 max = obb.GetConstAABB().maxV;
	Vector3 ptMinusExtents = pPrime - max;
	float prod0 = dPrime.z() * ptMinusExtents.y();
	float prod1 = dPrime.y() * ptMinusExtents.z();
	float delta, tmp, invLSquared;
	qPrime.x( pPrime.x() );

	if( prod0 >= prod1 )
	{
		// line intersects y-axis of OBB
		qPrime.y( max.y() );
		tmp = pPrime.z() + max.z();
		delta = prod0 - dPrime.y() * tmp;
		if( delta >= 0.0f )
		{
			// there is no intersection, so compute distance
			invLSquared = 1 / ( dPrime.y() * dPrime.y() + dPrime.z() * dPrime.z() );
			distanceSquared += (delta * delta) * invLSquared;
			qPrime.z( -max.z() );
			o_t = -( dPrime.y() * ptMinusExtents.y() * dPrime.z() + tmp ) * invLSquared;
		}
		else
		{
			// Line intersects box.  distance is zero
			invLSquared = 1 / dPrime.y();
			qPrime.z( pPrime.z() - (prod0 * invLSquared) );
			o_t = -ptMinusExtents.y() * invLSquared;
		}
	}
	else
	{
		// line intersects z-axis of OBB
		qPrime.z( max.z() );
		tmp = pPrime.y() + max.y();
		delta = prod1 - dPrime.z() * tmp;
		if( delta >= 0.0f )
		{
			// there is no intersection, so compute distance
			invLSquared = 1 / ( dPrime.y() * dPrime.y() + dPrime.z() * dPrime.z() );
			distanceSquared += (delta * delta) * invLSquared;
			qPrime.y( -max.y() );
			o_t = -( dPrime.z() * ptMinusExtents.z() * dPrime.y() + tmp ) * invLSquared;
		}
		else
		{
			// Line intersects box.  distance is zero
			invLSquared = 1 / dPrime.z();
			qPrime.y( pPrime.y() - (prod1 * invLSquared) );
			o_t = -ptMinusExtents.z() * invLSquared;
		}
	}

	// Now, consider the x-direction
	if( pPrime.x() < -(max.x()) )
	{
		delta = pPrime.x() + max.x();
		distanceSquared += delta * delta;
		qPrime.x( -max.x() );
	}
	else if( pPrime.x() > max.x() )
	{
		delta = pPrime.x() - max.x();
		distanceSquared += delta * delta;
		qPrime.x( max.x() );
	}

	return distanceSquared;
#endif
}

float OneZeroComponent_Y( Vector3CRef pPrime, Vector3CRef dPrime, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	Vector3 max = obb.GetConstAABB().maxV;
	float fPmE0 = pPrime.x() - max.x();
	float fPmE1 = pPrime.z() - max.z();
	float fProd0 = dPrime.z() * fPmE0;
	float fProd1 = dPrime.x() * fPmE1;
	float fDelta, fInvLSqr, fInv;
	float rfSqrDistance = 0.0f;

	qPrime = pPrime;

	if ( fProd0 >= fProd1 )
	{
		// line intersects P[i0] = e[i0]
		qPrime.x( max.x() );

		float fPpE1 = pPrime.z() + max.z();
		fDelta = fProd0 - dPrime.x() * fPpE1;
		if ( fDelta >= (float)0.0 )
		{
			fInvLSqr = ((float)1.0)/( dPrime.x()*dPrime.x()+dPrime.z()*dPrime.z());
			rfSqrDistance += fDelta*fDelta*fInvLSqr;
			qPrime.z( -max.z() );
			o_t = -(dPrime.x() * fPmE0 + dPrime.z() * fPpE1)*fInvLSqr;
		}
		else
		{
			fInv = ((float)1.0)/dPrime.x();
			qPrime.z( qPrime.z() - fProd0 * fInv );
			o_t = -fPmE0 * fInv;
		}
	}
	else
	{
		// line intersects P[i1] = e[i1]
		qPrime.z( max.z() );

		float fPpE0 = pPrime.x() + max.x();
		fDelta = fProd1 - dPrime.z() * fPpE0;
		if ( fDelta >= (float)0.0 )
		{
			fInvLSqr = ((float)1.0)/( dPrime.x()*dPrime.x()+dPrime.z()*dPrime.z());
			rfSqrDistance += fDelta*fDelta*fInvLSqr;
			qPrime.x( -max.x() );
			o_t = -(dPrime.x() * fPpE0 + dPrime.z() * fPmE1)*fInvLSqr;
		}
		else
		{
			fInv = ((float)1.0)/dPrime.z();
			qPrime.x( qPrime.x() - fProd1 * fInv );
			o_t = -fPmE1 * fInv;
		}
	}

	if ( pPrime.y() < -max.y() )
	{
		fDelta = pPrime.y() + max.y();
		rfSqrDistance += fDelta*fDelta;
		qPrime.y( -max.y() );
	}
	else if ( pPrime.y() > max.y() )
	{
		fDelta = pPrime.y() - max.y();
		rfSqrDistance += fDelta*fDelta;
		qPrime.y( max.y() );
	}
	return rfSqrDistance;
}

float OneZeroComponent_Z( Vector3CRef pPrime, Vector3CRef dPrime, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	Vector3 max = obb.GetConstAABB().maxV;
	float fPmE0 = pPrime.x() - max.x();
	float fPmE1 = pPrime.y() - max.y();
	float fProd0 = dPrime.y() * fPmE0;
	float fProd1 = dPrime.x() * fPmE1;
	float fDelta, fInvLSqr, fInv;
	float rfSqrDistance = 0.0f;

	qPrime = pPrime;

	if ( fProd0 >= fProd1 )
	{
		// line intersects P[i0] = e[i0]
		qPrime.x( max.x() );

		float fPpE1 = pPrime.y() + max.y();
		fDelta = fProd0 - dPrime.x() * fPpE1;
		if ( fDelta >= (float)0.0 )
		{
			fInvLSqr = ((float)1.0)/( dPrime.x()*dPrime.x()+dPrime.y()*dPrime.y());
			rfSqrDistance += fDelta*fDelta*fInvLSqr;
			qPrime.y( -max.y() );
			o_t = -(dPrime.x() * fPmE0 + dPrime.y() * fPpE1)*fInvLSqr;
		}
		else
		{
			fInv = ((float)1.0)/dPrime.x();
			qPrime.y( qPrime.y() - fProd0 * fInv );
			o_t = -fPmE0 * fInv;
		}
	}
	else
	{
		// line intersects P[i1] = e[i1]
		qPrime.y( max.y() );

		float fPpE0 = pPrime.x() + max.x();
		fDelta = fProd1 - dPrime.y() * fPpE0;
		if ( fDelta >= (float)0.0 )
		{
			fInvLSqr = ((float)1.0)/( dPrime.x()*dPrime.x()+dPrime.y()*dPrime.y());
			rfSqrDistance += fDelta*fDelta*fInvLSqr;
			qPrime.x( -max.x() );
			o_t = -(dPrime.x() * fPpE0 + dPrime.y() * fPmE1)*fInvLSqr;
		}
		else
		{
			fInv = ((float)1.0)/dPrime.y();
			qPrime.x( qPrime.x() - fProd1 * fInv );
			o_t = -fPmE1 * fInv;
		}
	}

	if ( pPrime.z() < -max.z() )
	{
		fDelta = pPrime.z() + max.z();
		rfSqrDistance += fDelta*fDelta;
		qPrime.z( -max.z() );
	}
	else if ( pPrime.z() > max.z() )
	{
		fDelta = pPrime.z() - max.z();
		rfSqrDistance += fDelta*fDelta;
		qPrime.z( max.z() );
	}
	return rfSqrDistance;
}

float TwoZeroComponents_YZ( Vector3CRef pPrime, Vector3CRef dPrime, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	float distanceSquared = 0.0f, delta;
	Vector3 max = obb.GetConstAABB().maxV;

	// Parameter of closest point on the line
	o_t = ( max.x() - pPrime.x() ) / dPrime.x();

	// Closest point on the box
	qPrime.x( max.x() );
	qPrime.y( pPrime.y() );
	qPrime.z( pPrime.z() );

	// Compute distance squared and Y and Z components of box's closest point
	if( pPrime.y() < -max.y() )
	{
		delta = pPrime.y() + max.y();
		distanceSquared += delta * delta;
		qPrime.y( -max.y() );
	}
	else if( pPrime.y() > max.y() )
	{
		delta = pPrime.y() - max.y();
		distanceSquared += delta * delta;
		qPrime.y( max.y() );
	}
	if( pPrime.z() < -max.z() )
	{
		delta = pPrime.z() + max.z();
		distanceSquared += delta * delta;
		qPrime.z( -max.z() );
	}
	else if( pPrime.z() > max.z() )
	{
		delta = pPrime.z() - max.z();
		distanceSquared += delta * delta;
		qPrime.z( max.z() );
	}

	return distanceSquared;
}

float TwoZeroComponents_XZ( Vector3CRef pPrime, Vector3CRef dPrime, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	float distanceSquared = 0.0f, delta;
	Vector3 max = obb.GetConstAABB().maxV;

	// Parameter of closest point on the line
	o_t = ( max.y() - pPrime.y() ) / dPrime.y();

	// Closest point on the box
	qPrime.x( pPrime.x() );
	qPrime.y( max.y() );
	qPrime.z( pPrime.z() );

	// Compute distance squared and Y and Z components of box's closest point
	if( pPrime.x() < -max.x() )
	{
		delta = pPrime.x() + max.x();
		distanceSquared += delta * delta;
		qPrime.x( -max.x() );
	}
	else if( pPrime.x() > max.x() )
	{
		delta = pPrime.x() - max.x();
		distanceSquared += delta * delta;
		qPrime.x( max.x() );
	}
	if( pPrime.z() < -max.z() )
	{
		delta = pPrime.z() + max.z();
		distanceSquared += delta * delta;
		qPrime.z( -max.z() );
	}
	else if( pPrime.z() > max.z() )
	{
		delta = pPrime.z() - max.z();
		distanceSquared += delta * delta;
		qPrime.z( max.z() );
	}

	return distanceSquared;
}

float TwoZeroComponents_XY( Vector3CRef pPrime, Vector3CRef dPrime, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	float distanceSquared = 0.0f, delta;
	Vector3 max = obb.GetConstAABB().maxV;

	// Parameter of closest point on the line
	o_t = ( max.z() - pPrime.z() ) / dPrime.z();

	// Closest point on the box
	qPrime.x( pPrime.x() );
	qPrime.y( pPrime.y() );
	qPrime.z( max.z() );

	// Compute distance squared and Y and Z components of box's closest point
	if( pPrime.x() < -max.x() )
	{
		delta = pPrime.x() + max.x();
		distanceSquared += delta * delta;
		qPrime.x( -max.x() );
	}
	else if( pPrime.x() > max.x() )
	{
		delta = pPrime.x() - max.x();
		distanceSquared += delta * delta;
		qPrime.x( max.x() );
	}
	if( pPrime.y() < -max.y() )
	{
		delta = pPrime.y() + max.y();
		distanceSquared += delta * delta;
		qPrime.y( -max.y() );
	}
	else if( pPrime.y() > max.y() )
	{
		delta = pPrime.y() - max.y();
		distanceSquared += delta * delta;
		qPrime.y( max.y() );
	}

	return distanceSquared;
}

float ThreeZeroComponents( Vector3CRef pPrime, Vector3CRef dPrime, const OBB &obb, float &o_t, Vector3 &qPrime )
{
	o_t = 0.0f;
	return PointToOBBDistanceSquared_PointInOBB( pPrime, obb, qPrime );
}

//#define DEBUG_SEG2OBB_DRAW
float SegmentToOBBDistanceSquared( Vector3CRef LOrigin, Vector3CRef LDirection,
	float LStart, float LEnd, const OBB &obb, Vector3 &o_LP, Vector3 &o_BP )
{
	// From "Geometric Tools For Computer Graphics" (Schneider & Eberly)
	// Section 10.9.4, Linear Component to Oriented Bounding Box

	Vector3 ax = obb.Matrix()->GetRow0();
	Vector3 ay = obb.Matrix()->GetRow1();
	Vector3 az = obb.Matrix()->GetRow2();
	Vector3 max = obb.GetConstAABB().maxV;

	// Transform p into box's coordinate frame
	Vector3 offset = LOrigin - *(obb.Center());
	Vector3 pPrime( offset.Dot( ax ), offset.Dot( ay ), offset.Dot( az ) );
	Vector3 dPrime( LDirection.Dot( ax ), LDirection.Dot( ay ), LDirection.Dot( az ) );
	Vector3 qPrime;
	float distanceSquared;
	float t;

	bool flipX = false, flipY = false, flipZ = false;
    
	if( dPrime.x() < 0.0f )
	{
		dPrime.x( -dPrime.x() );
		pPrime.x( -pPrime.x() );
		flipX = true;
	}
	if( dPrime.y() < 0.0f )
	{
		dPrime.y( -dPrime.y() );
		pPrime.y( -pPrime.y() );
		flipY = true;
	}
	if( dPrime.z() < 0.0f )
	{
		dPrime.z( -dPrime.z() );
		pPrime.z( -pPrime.z() );
		flipZ = true;
	}
	// how many components of dPrime are zero?
	if( Math::Zero( dPrime.x() ) )
	{
		if( Math::Zero( dPrime.y() ) )
		{
			if( Math::Zero( dPrime.z() ) )
				distanceSquared = ThreeZeroComponents( pPrime, dPrime, obb, t, qPrime );
			else
				distanceSquared = TwoZeroComponents_XY( pPrime, dPrime, obb, t, qPrime );
		}
		else if( Math::Zero( dPrime.z() ) )
		{
			distanceSquared = TwoZeroComponents_XZ( pPrime, dPrime, obb, t, qPrime );
		}
		else
			distanceSquared = OneZeroComponent_X( pPrime, dPrime, obb, t, qPrime );
	}
	else if( Math::Zero( dPrime.y() ) )
	{
		if( Math::Zero( dPrime.z() ) )
			distanceSquared = TwoZeroComponents_YZ( pPrime, dPrime, obb, t, qPrime );
		else
			distanceSquared = OneZeroComponent_Y( pPrime, dPrime, obb, t, qPrime );
	}
	else if( Math::Zero( dPrime.z() ) )
		distanceSquared = OneZeroComponent_Z( pPrime, dPrime, obb, t, qPrime );
	else
		distanceSquared = NoZeroComponents( pPrime, dPrime, obb, t, qPrime );

	// make sure the closest point on the line is really on the line
	if( t < LStart )
	{
		o_LP = LOrigin + LStart * LDirection;
		distanceSquared = PointToOBBDistanceSquared( o_LP, obb, o_BP );
		t = LStart;
	}
	else if( t > LEnd )
	{
		o_LP = LOrigin + LEnd * LDirection;
		distanceSquared = PointToOBBDistanceSquared( o_LP, obb, o_BP );
		t = LEnd;
	}
	else
	{
		if( flipX )
			qPrime.x( -qPrime.x() );
		if( flipY )
			qPrime.y( -qPrime.y() );
		if( flipZ )
			qPrime.z( -qPrime.z() );

		o_BP.x( qPrime.x() * ax.x() + qPrime.y() * ay.x() + qPrime.z() * az.x() );
		o_BP.y( qPrime.x() * ax.y() + qPrime.y() * ay.y() + qPrime.z() * az.y() );
		o_BP.z( qPrime.x() * ax.z() + qPrime.y() * ay.z() + qPrime.z() * az.z() );

		o_BP += *(obb.Center());

		o_LP = LOrigin + t * LDirection;
	}

	#ifdef DEBUG_SEG2OBB_DRAW
		DrawUtility::Point( o_LP, DrawUtility::MAGENTA, 0.1f );
		DrawUtility::Point( o_BP, DrawUtility::CYAN, 0.1f );
	#endif

	return distanceSquared;
}

void SegmentToRectangleDistanceSquared( Vector3CRef LOrigin, Vector3CRef LDirection,
	float LStart, float LEnd, Vector3CRef RNormal, Vector3CRef ROrigin, Vector3CRef RDirection1, 
	float RStart1, float REnd1, Vector3CRef RDirection2, float RStart2, float REnd2, 
	SegmentToRectangleInfo &o_info )
{
	float LNRatio, RLRatio1, RLRatio2;

	LineToPlaneIntersection( LOrigin, LDirection, RNormal, ROrigin, o_info.LT, LNRatio );
	RLRatio1 = RDirection1.Dot( LDirection );
	RLRatio2 = RDirection2.Dot( LDirection );

	Vector3 intersection = LOrigin + LDirection * o_info.LT;
	Vector3 iMinusR = intersection - ROrigin;
	
	o_info.RT1 = iMinusR.Dot( RDirection1 );
	o_info.RT2 = iMinusR.Dot( RDirection2 );

	// right now, LT, RT1, RT2 specifies the intersection point of the infinite line
	// against the infinite plane.  First, bring the point onto the segment, making sure
	// to drag the times along the rectangle axes by the correct ratio
	float off;
	if( o_info.LT < LStart )
	{
		off = LStart - o_info.LT;
		o_info.RT1 += off * RLRatio1;
		o_info.RT2 += off * RLRatio2;
	}
	if( o_info.LT > LEnd )
	{
		off = LEnd - o_info.LT;
		o_info.RT1 += off * RLRatio1;
		o_info.RT2 += off * RLRatio2;
	}
	// we may have a solution right now
	if( o_info.RT1 >= RStart1 && o_info.RT1 <= REnd1 &&
		o_info.RT2 >= RStart2 && o_info.RT2 <= REnd2 )
	{
		o_info.PointOnLine = LOrigin + LDirection * o_info.LT;

	}
}

// evil ugly nasty function:
// what this does, is takes a rectangle and a line, and figures out how far you would need
// to move that line along the rectangle's normal so that the line no longer passed "behind"
// the rectangle at any point.  If you really need more details, talk to Travis, and hope he
// remembers what the fuck he did.

// the line definition is the same as all the preceding functions.  an origin, a normalized direction,
// and start and end times

// the rectangle definition is similar.  A point, a normal, the 2 axes defining the side of the rectangle,
// and start/end times along the edges.
float CapsuleToRectangleMaxPenetration( Vector3CRef LOrigin, Vector3CRef LDirection, float radius,
	float LStart, float LEnd, Vector3CRef RNormal, Vector3CRef ROrigin, Vector3CRef RDirection1, 
	float RStart1, float REnd1, Vector3CRef RDirection2, float RStart2, float REnd2, 
	SegmentToRectangleInfo &o_info )
{
	return 0.0f;
#if 0
	// first, determine the intersection point between the infinite plane and the infinite line
	float LNRatio = LDirection.Dot( RNormal );
	float R1LRatio = RDirection1.Dot( LDirection );
	float R2LRatio = RDirection2.Dot( LDirection );

	Vector3 intersection;

	if( Math::Zero( LNRatio ) )
	{
		// line and plane are parallel
		intersection = LOrigin;
	}
	else
	{
		intersection = LOrigin - ( LDirection * ( (LOrigin - ROrigin).Dot( RNormal ) / LNRatio ) );
	}
	Vector3 iMinusR = intersection - ROrigin;
	o_info.LT = (intersection - LOrigin).Dot( LDirection );
	o_info.RT1 = iMinusR.Dot( RDirection1 );
	o_info.RT2 = iMinusR.Dot( RDirection2 );

	// is the line entirely in front 
	if( (LNRatio >= 0.0f && LStart > o_info.LT) ||
		(LNRatio < 0.0f && LEnd < o_info.LT) )
	if( o_info.RT1 > RStart1 && o_info.RT1 < REnd1 && o_info.RT2 > RStart2 && o_info.RT2 < REnd2 )
	{
		// intersection point falls inside the rectangle.  trace the line back to the nearest edge,
		// or to the end of the line, whichever comes first.
		float dToLineBack = MAXFLOAT, dToEdge1 = MAXFLOAT, dToEdge2 = MAXFLOAT;
		float lineStepDirection = MAXFLOAT;

		// considering 3 points: the end of the line that's furthest "behind" the plane,
		// and the furthest back point where the line becomes perpendicular to each edge.
		// The first of these that we reach when stepping backwards along the line is our
		// maximum penetration point.
		bool found = false;
		float first = MAXFLOAT;
		float test;
		if( LNRatio >= 0.0f ) // line goes the same direction as the plane normal.  Step backwards on it
		{
			dToLineBack = LStart - o_info.LT;
			lineStepDirection = -1.0f;
		}
		else // opposite direction from plane normal.  step forward along the line to get to the "back"
		{
			dToLineBack = LEnd - o_info.LT;
			lineStepDirection = 1.0f;
		}
		if( !Math::Zero( R1LRatio ) )
		{
			if( R1LRatio > 0.0f ) 
				e1Step = lineStepDirection;
			else
				e1Step = -lineStepDirection;
			if( e1Step > 0.0f )
				dToEdge1 = REnd1 - o_info.RT1;
			else
				dToEdge1 = RStart1 - o_info.RT1;
			dToEdge1 = dToEdge1 / R1LRatio;
		}
		if( !Math::Zero( R2LRatio ) )
		{
			if( R2LRatio > 0.0f ) 
				e2Step = lineStepDirection;
			else
				e2Step = -lineStepDirection;
			if( e2Step > 0.0f )
				dToEdge2 = REnd2 - o_info.RT2;
			else
				dToEdge2 = RStart2 - o_info.RT2;
			dToEdge2 = dToEdge2 / R2LRatio;
		}
		// now, which of these is the smallest, but still puts us behind the plane?
		
		test = dToLineBack * lineStepDirection;
		if( test > biggest ) { biggest = test; found = true; }
		test = dToEdge1 * e1Step;
		if( test > biggest ) { biggest = test; found = true; }
		test = dToEdge2 * e2Step;
		if( test > biggest ) { biggest = test; found = true; }

		if( found )
		{
			float off = biggest * lineStepDirection;
			o_info.LT += off;
			if( o_info.LT < LStart )
			{
				off += LStart - o_info.LT;
				o_info.LT = LStart;
			}
			if( o_info.LT > LEnd )
			{
				off += LEnd - o_info.LT;
				o_info.LT = LEnd;
			}
			o_info.RT1 += off * R1LRatio;
			o_info.RT2 += off * R2LRatio;

			// if that operation took us out of the rect, then we missed
			if( o_info.RT1 > RStart1 && o_info.RT1 < REnd1 && o_info.RT2 > RStart2 && o_info.RT2 < REnd2 )
			{
				o_info.PointOnLine = LOrigin + LDirection * o_info.LT;
				return (ROrigin - o_info.PointOnLine).Dot( RNormal );
			}
			else
			{
				return MAXFLOAT;
			}
		}
	}
	else
	{
	}

	return MAXFLOAT;
#endif
#if 0
	// now we need to test 6 points, and see which one has the maximum penetration.
	// each end of the line, and the intersection between the line and each edge.

	// but, if advancing along the line to each point takes us off the line, or outside of the
	// rectangle, then that point does not qualify.
	bool found = false;
	float foundDepth = 0.0f;
	float foundLT;
	float thisdepth;
	float thisLT;
	float thisRT1;
	float thisRT2;
	float off;

	// Line origin
	thisLT = LStart;
	off = thisLT - o_info.LT;
	thisRT1 = o_info.RT1 + off * R1LRatio;
	thisRT2 = o_info.RT2 + off * R2LRatio;
	if( thisRT1 > RStart1 && thisRT1 < REnd1 && thisRT2 > RStart2 && thisRT2 < REnd2 )
	{
		thisdepth = (ROrigin - (LOrigin + thisLT * LDirection)).Dot( RNormal );
		if( thisdepth > foundDepth ) { found = true; foundLT = thisLT; foundDepth = thisdepth; }
	}

	// Line end
	thisLT = LEnd;
	off = thisLT - o_info.LT;
	thisRT1 = o_info.RT1 + off * R1LRatio;
	thisRT2 = o_info.RT2 + off * R2LRatio;

	if( thisRT1 > RStart1 && thisRT1 < REnd1 && thisRT2 > RStart2 && thisRT2 < REnd2 )
	{
		thisdepth = (ROrigin - (LOrigin + thisLT * LDirection)).Dot( RNormal );
		if( thisdepth > foundDepth ) { found = true; foundLT = thisLT; foundDepth = thisdepth; }
	}

	// Edge 1
	if( !Math::Zero( R1LRatio ) )
	{
		// min
		thisRT1 = RStart1;
		off = thisRT1 - o_info.RT1;
		thisLT = o_info.LT + off / R1LRatio;
		thisRT2 = o_info.RT2 + off * R2LRatio;
		if( thisRT1 > RStart1 && thisRT1 < REnd1 && thisRT2 > RStart2 && thisRT2 < REnd2 )
		{
			thisdepth = (ROrigin - (LOrigin + thisLT * LDirection)).Dot( RNormal );
			if( thisdepth > foundDepth ) { found = true; foundLT = thisLT; foundDepth = thisdepth; }
		}
		// max
		thisRT1 = REnd1;
		off = thisRT1 - o_info.RT1;
		thisLT = o_info.LT + off / R1LRatio;
		thisRT2 = o_info.RT2 + off * R2LRatio;
		if( thisRT1 > RStart1 && thisRT1 < REnd1 && thisRT2 > RStart2 && thisRT2 < REnd2 )
		{
			thisdepth = (ROrigin - (LOrigin + thisLT * LDirection)).Dot( RNormal );
			if( thisdepth > foundDepth ) { found = true; foundLT = thisLT; foundDepth = thisdepth; }
		}
	}
	// Edge 2
	if( !Math::Zero( R2LRatio ) )
	{
		// min
		thisRT2 = RStart2;
		off = thisRT2 - o_info.RT2;
		thisLT = o_info.LT + off / R2LRatio;
		thisRT1 = o_info.RT1 + off * R1LRatio;
		if( thisRT1 > RStart1 && thisRT1 < REnd1 && thisRT2 > RStart2 && thisRT2 < REnd2 )
		{
			thisdepth = (ROrigin - (LOrigin + thisLT * LDirection)).Dot( RNormal );
			if( thisdepth > foundDepth ) { found = true; foundLT = thisLT; foundDepth = thisdepth; }
		}
		// max
		thisRT2 = REnd2;
		off = thisRT2 - o_info.RT2;
		thisLT = o_info.LT + off / R2LRatio;
		thisRT1 = o_info.RT1 + off * R1LRatio;
		if( thisRT1 > RStart1 && thisRT1 < REnd1 && thisRT2 > RStart2 && thisRT2 < REnd2 )
		{
			thisdepth = (ROrigin - (LOrigin + thisLT * LDirection)).Dot( RNormal );
			if( thisdepth > foundDepth ) { found = true; foundLT = thisLT; foundDepth = thisdepth; }
		}
	}

	if( found )
	{
		float off = o_info.LT - foundLT;
		o_info.LT = foundLT;
		o_info.RT1 += off * R1LRatio;
		o_info.RT2 += off * R2LRatio;
		o_info.PointOnLine = LOrigin + LDirection * o_info.LT;
	}

	return foundDepth;
#endif
}
