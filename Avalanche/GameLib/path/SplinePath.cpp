///////////////////////////////////////////////////////////////////////////
//
//  SplinePath.cpp
//
//  A spline path.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "path/PathPCH.h"


#include "math/fastrand.h"
#include "Layers/Timing.h"


///////////////////////////////////////////////////////////////////////////
//  Implementation
///////////////////////////////////////////////////////////////////////////

SplinePath::SplinePath( char* name, Vector3Packed* points, int num_points, bool bForceNoLoop /*= false*/ ) :
SegmentPath( name, points, num_points, 3, bForceNoLoop )
{
	// Sanity check
	ASSERT( ( num_points - 1 ) % 3 == 0 );

	// Create the control points.
	m_NumControlPoints = ( ( num_points - 1 ) / 3 ) * 2;
	m_ControlPoints = new Vector3[ m_NumControlPoints ];
	ASSERT( m_ControlPoints );
	m_u32Flags |= SPLINE_PATH_OWNS_CONTROL_POINTS;

	for ( int i = 0; i < m_NumControlPoints; i += 2 )
	{
		m_ControlPoints[ i + 0 ] = points[ ( ( i / 2 ) * 3 ) + 1 ];
		m_ControlPoints[ i + 1 ] = points[ ( ( i / 2 ) * 3 ) + 2 ];

		// Sanity checks
		ASSERT( ( ( i / 2 ) * 3 ) + 1 < num_points );
		ASSERT( ( ( i / 2 ) * 3 ) + 2 < num_points );
	}

	/* creating from scratch */

	m_pSource = NULL;

	/* clear cache */

	m_Cache.m_Distance = MAXFLOAT;
	m_Cache.m_T = 0;
	m_Cache.m_P0 = NULL;
	m_Cache.m_P1 = NULL;
	m_Cache.m_P2 = NULL;
	m_Cache.m_P3 = NULL;

	#ifdef _DEBUG
	m_Cache.m_Misses = 0;
	m_Cache.m_Hits = 0;
	#endif
}

SplinePath::SplinePath( char* name, Vector3Padded* points, int num_points, bool bForceNoLoop /*= false*/ ) :
SegmentPath( name, points, num_points, 3, bForceNoLoop )
{
	// Sanity check
	ASSERT( ( num_points - 1 ) % 3 == 0 );

	// Create the control points.
	m_NumControlPoints = ( ( num_points - 1 ) / 3 ) * 2;
	m_ControlPoints = new Vector3[ m_NumControlPoints ];
	ASSERT( m_ControlPoints );
	m_u32Flags |= SPLINE_PATH_OWNS_CONTROL_POINTS;

	for ( int i = 0; i < m_NumControlPoints; i += 2 )
	{
		m_ControlPoints[ i + 0 ] = points[ ( ( i / 2 ) * 3 ) + 1 ];
		m_ControlPoints[ i + 1 ] = points[ ( ( i / 2 ) * 3 ) + 2 ];

		// Sanity checks
		ASSERT( ( ( i / 2 ) * 3 ) + 1 < num_points );
		ASSERT( ( ( i / 2 ) * 3 ) + 2 < num_points );
	}

	/* creating from scratch */

	m_pSource = NULL;

	/* clear cache */

	m_Cache.m_Distance = MAXFLOAT;
	m_Cache.m_T = 0;
	m_Cache.m_P0 = NULL;
	m_Cache.m_P1 = NULL;
	m_Cache.m_P2 = NULL;
	m_Cache.m_P3 = NULL;

	#ifdef _DEBUG
	m_Cache.m_Misses = 0;
	m_Cache.m_Hits = 0;
	#endif
}

///////////////////////////////////////////////////////////////////////////

/* create a new spline path from an existing one, typically in preparation to randomize the path */

SplinePath::SplinePath(SplinePath *pSource)
{
	*this = *pSource;
	m_pSource = pSource;

	/* clear cache */

	m_Cache.m_Distance = MAXFLOAT;
	m_Cache.m_T = 0;
	m_Cache.m_P0 = NULL;
	m_Cache.m_P1 = NULL;
	m_Cache.m_P2 = NULL;
	m_Cache.m_P3 = NULL;
}

///////////////////////////////////////////////////////////////////////////

SplinePath::~SplinePath( void )
{
	if (m_u32Flags & SPLINE_PATH_OWNS_CONTROL_POINTS)
		delete [] m_ControlPoints;
}

///////////////////////////////////////////////////////////////////////////

/* re-calculate segment lengths and path overall length from spline lengths instead of segment lengths */
/*** hack! maybe we should get rid of SegmentPath, as suggested by Nate? SegmentPath was implemented as  ***/
/*** an LOD for path followers when the are off-screen, but performance benefit is minimal, and the      ***/
/*** spline vs segment distinction adds some (maybe) unnecessary complexity/inaccuracy. this routine     ***/
/*** was written because i had a one-segment path that started and ended in the same place-- SegmentPath ***/
/*** calculates its length as 0, of course                                                               ***/

void SplinePath::RecalculateLength()
{
	m_Length = 0.0f;
	for (int i = 0; i < m_NumSegments; i++)
	{
		/* nothing yet */

		m_Segments[i].m_Length = 0.0f;

		/* points and control points that make up this segment */

		Vector3* p0 = &m_Points[i];
		Vector3* p1 = &m_ControlPoints[(i * 2) + 0];
		Vector3* p2 = &m_ControlPoints[(i * 2) + 1];
		Vector3* p3 = &m_Points[i + 1];

		/* sample spline to get rough length */

#define SAMPLE_POINTS 10
		Vector3 vLastPos = *p0;	// first endpoint
		float t = 1.0f / (float) SAMPLE_POINTS;
		for (int j = 0; j < SAMPLE_POINTS - 1; j++, t += 1.0f / (float) SAMPLE_POINTS)
		{
			/* helpful multipliers */

			float s = 1.0f - t;
			float ss = s * s;
			float tt = t * t;

			/* uses a Bernstein basis function. 18 multiplies, 9 adds (+ 1 subtract and 2 multiplies above). */

			float a = s * ss;
			float b = 3.0f * t * ss;
			float c = 3.0f * tt * s;
			float d = t * tt;

			/* calculate point on spline */

			Vector3 vPos = *p0 * a + *p1 * b + *p2 * c + *p3 * d;

			/* update length of this segment */

			m_Segments[i].m_Length += (vPos - vLastPos).LengthSquared();
			vLastPos = vPos;
		}

		/* one more for second endpoint */

		m_Segments[i].m_Length += (*p3 - vLastPos).LengthSquared();

		/* cumulative length */

		m_Segments[i].m_Cumulative = m_Length;

		/* and update total length */

		m_Length += m_Segments[i].m_Length;
	}
}

///////////////////////////////////////////////////////////////////////////

Vector3 SplinePath::PointFromDistance( float distance )
{
	Vector3 p;
#ifndef SHIPIT
    Profiler__Start(g_pathProfileSlot);
#endif

	distance = WrapDistance( distance );

	// Find the segment that the distance is on.
	int i = SegmentFromDistance( distance );
	
	// Compute a t along the segment.
	float t = ( distance - m_Segments[ i ].m_Cumulative ) / m_Segments[ i ].m_Length;
	float s = 1.0f - t;
	float ss = s * s;
	float tt = t * t;
	
	// Sanity check.
	ASSERT( t > -0.001f && t < 1.001f );
	
	// Compute the point on the spline.
	
	// Uses a Bernstein basis function.
	// 18 multiplies, 9 adds (+ 1 subtract and 2 multiplies above).
	float a = s * ss;
	float b = 3 * t * ss;
	float c = 3 * tt * s;
	float d = t * tt;
	
	// Sanity check.
	ASSERT( i * 2 + 1 < m_NumControlPoints );

	Vector3* p0 = &( m_Points[ i ] );
	Vector3* p1 = &( m_ControlPoints[ i * 2 + 0 ] );
	Vector3* p2 = &( m_ControlPoints[ i * 2 + 1 ] );
	Vector3* p3 = &( m_Points[ i + 1 ] );

	// check for wrapping closed loops
	if ((ClosedLoop()) && (i == m_NumPoints - 1))
		p3 = &( m_Points[ 0 ] );
#ifdef _DEBUG
	else
		ASSERT(i < m_NumPoints - 1);	// for non-closed paths, segment count should be (point count - 1)
#endif //_DEBUG

	p = *p0 * a + *p1 * b + *p2 * c + *p3 * d;

	// Cache.
	m_Cache.m_Distance = distance;
	m_Cache.m_T = t;
	m_Cache.m_P0 = p0;
	m_Cache.m_P1 = p1;
	m_Cache.m_P2 = p2;
	m_Cache.m_P3 = p3;
	
#ifndef SHIPIT
    Profiler__Stop(g_pathProfileSlot);
#endif
	return p;
}

///////////////////////////////////////////////////////////////////////////

Vector3 SplinePath::TangentFromDistance( float distance )
{
	if ( m_Cache.m_Distance != distance )
	{
		PointFromDistance( distance );

		#ifdef _DEBUG
		m_Cache.m_Misses++;
		#endif
	}
	#ifdef _DEBUG
	else
	{
		m_Cache.m_Hits++;
	}
	#endif
#ifndef SHIPIT
    Profiler__Start(g_pathProfileSlot);
#endif

	float t = m_Cache.m_T;
	float s = 1.0f - t;
	float ss = s * s;
	float tt = t * t;

	// Uses derivative of Bernstein basis function.
	// 16 multiplies, 9 adds, 3 subtracts (+1 subtract and 2 multiplies above).
	float u = 3 * ss;
	float v = 6 * t * s;
	float w = 3 * tt;

	float a = -u;
	float b = u - v;
	float c = v - w;
	float d = w;

	Vector3* p0 = m_Cache.m_P0;
	Vector3* p1 = m_Cache.m_P1;
	Vector3* p2 = m_Cache.m_P2;
	Vector3* p3 = m_Cache.m_P3;

	Vector3 p = *p0 * a + *p1 * b + *p2 * c + *p3 * d;

	// Eek!  The tangent must be normalized.
	p.SafeNormalize();

#ifndef SHIPIT
    Profiler__Stop(g_pathProfileSlot);
#endif
	return p;
}

///////////////////////////////////////////////////////////////////////////

/* randomize a spline path. this is so multiple path followers can use the same source path but follow it slightly
differently. this routine randomizes the point and control point buffers. however, it leaves the segment buffer alone
(and indeed, just points to the source path's segment buffer). this has the effect of making the path the same length
no matter the randomization, because the segment buffer contains pre-calculated segment lengths */

void SplinePath::Randomize()
{
	/* the intent is to make a copy of an original spline path and then randomize the copy-- it doesn't really make */
	/* sense to randomize the original, i don't think */

	ASSERT(m_pSource);	// should always be randomizing a copy of a path? ok to ignore this

	/* point to source data */

	Vector3 *pvOrigPoints;
	Vector3 *pvOrigControlPoints;
	if (m_pSource)
	{
		pvOrigPoints = m_pSource->GetPointArray();
		pvOrigControlPoints = m_pSource->GetControlPointArray();
	}
	else
	{
		/* no parent source, this must be an original spline. since we just modify in place, ok to point */
		/* to current buffers */

		pvOrigPoints = GetPointArray();
		pvOrigControlPoints = GetControlPointArray();
	}

	/* malloc new point and control point buffers if necessary */

	if ((m_u32Flags & SEGMENT_PATH_OWNS_POINTS) == 0)
	{
		m_Points = new Vector3[NumPoints()];
		m_u32Flags |= SEGMENT_PATH_OWNS_POINTS;
	}
	if ((m_u32Flags & SPLINE_PATH_OWNS_CONTROL_POINTS) == 0)
	{
		m_ControlPoints = new Vector3[NumControlPoints()];
		m_u32Flags |= SPLINE_PATH_OWNS_CONTROL_POINTS;
	}

	/* randomly initialize points. each point owns two control points, and they should be adjusted the same */
	/* amount as the point to maintain first degree continuity. each point owns the control points immediately */
	/* behind and ahead of it. first and last point own one control point each */

	for (int i = 0; i < NumPoints(); i++)
	{
		/* watch out for short segments. this is written for closed loops, but there's no harm for non-looped */
		/* paths-- really, for a non-loop path, we only need to check one segment for the first point, but */
		/* the only penalty is a possible reduction in the amount of randomness if the last segment happens */
		/* to be short-- not worth worrying about */

		int nCurrentSegment = i;
		int nPrevSegment = i - 1;
		if (nCurrentSegment == NumPoints() - 1)
			nCurrentSegment = 0;	// last point is duplicate of first point, use first point's segment
		if (nPrevSegment < 0)
			nPrevSegment = NumPoints() - 2;	// last segment is duplicate of first, we want the one before that
		float fRandomRange = Math::Min(GetSegmentLength(nCurrentSegment), GetSegmentLength(nPrevSegment));

		/* control points are typically at about 1/4 and 3/4 of distance between points. we don't want the */
		/* randomization to swap the positions of points or control points-- we'll get loops. also don't */
		/* make too big-- path followers may go too far off path */

		fRandomRange *= 0.125f;
		fRandomRange = Math::Min(fRandomRange, 1.0f);	// max of 2 x 2 x 2 meter cube around original point

		/* randomize */

		Vector3 vAdjust;
		vAdjust.x( g_random.InRange(-fRandomRange, fRandomRange) );
		vAdjust.y( g_random.InRange(-fRandomRange, fRandomRange) );
		vAdjust.z( g_random.InRange(-fRandomRange, fRandomRange) );

		/* adjusted point */

		m_Points[i] = pvOrigPoints[i] + vAdjust;

		/* adjusted control points */

		if (i == 0)
		{
			m_ControlPoints[0] = pvOrigControlPoints[0] + vAdjust;
			if (ClosedLoop())
				m_ControlPoints[NumControlPoints() - 1] = pvOrigControlPoints[NumControlPoints() - 1] + vAdjust;
		}
		else if ((ClosedLoop() == false) && (i == NumPoints() - 1))
		{
			m_ControlPoints[NumControlPoints() - 1] = pvOrigControlPoints[NumControlPoints() - 1] + vAdjust;
		}
		else
		{
			int j;
			j = (i * 2) - 1;	// preceeding control point, e.g. 1 -> 1, 2 -> 3
			m_ControlPoints[j] = pvOrigControlPoints[j] + vAdjust;
			j = (i * 2);		// succeeding control point, e.g. 1 -> 2, 2 -> 4
			m_ControlPoints[j] = pvOrigControlPoints[j] + vAdjust;
			ASSERT(j < NumControlPoints());	// don't overflow buffer
		}
	}
}

///////////////////////////////////////////////////////////////////////////

void SplinePath::Draw( float arcLength )
{
	for ( int i = 0; i < m_NumPoints; i++ )
	{
		Vector3 p0 = m_Points[ i + 0 ];
		Vector3 p1;
		if ( i == m_NumPoints - 1 )
		{
			if ( ClosedLoop() )
			{
				p1 = m_Points[ 0 ];
			}
			else
			{
				p1 = p0;
			}
		}
		else
		{
			p1 = m_Points[ i + 1 ];
		}
		
		// draw the points on the curve.
		DrawUtility::Point( p0, DrawUtility::CYAN, 0.025f );
		
		// if it is the first point, draw it in green...
		if ( i == 0 )
		{
			DrawUtility::Point( p0, DrawUtility::GREEN, 0.025f );
		}
		
		// ...draw the last point in red.
		if ( i == m_NumPoints - 1 )
		{
			DrawUtility::Point( p1, DrawUtility::RED, 0.025f );
		}
		
		// draw the control points.
		Vector3 c0 = m_ControlPoints[ i * 2 + 0 ];
		Vector3 c1 = m_ControlPoints[ i * 2 + 1 ];
		DrawUtility::Point( c0, DrawUtility::BLUE, 0.025f );
		DrawUtility::Point( c1, DrawUtility::BLUE, 0.025f );
	}
	
	// draw an arclength parameterization of the curve.
	const int numArcs = ( int )Math::Ceil( Length() / arcLength );
	for ( int j = 0; j < numArcs; j++ )
	{
		Vector3 p0 = PointFromDistance( ( j + 0 ) * arcLength );
		Vector3 p1 = PointFromDistance( ( j + 1 ) * arcLength );

		if ( ( j + 1 ) * arcLength > Length() )
		{
			if ( ClosedLoop() )
			{
				p1 = PointFromDistance( 0 );
			}
			else
			{
				p1 = PointFromDistance( Length() );
			}
		}

		DrawUtility::Line( p0, p1, DrawUtility::YELLOW );
	}
}

///////////////////////////////////////////////////////////////////////////

void SplinePath::DrawSegments(Vector3 MyPoint )
{
	int i;

	RenderState__Default();
	TextureState__Default();
	
	RenderState__Request( RS_CULL, RS_CULL_NONE );
	RenderState__Request( RS_LIGHTING, RS_FALSE );
	
	for(i=0;i<this->m_NumSegments;i++)
	{
		DrawUtility::Line(*m_Segments[i].m_P1, *m_Segments[i].m_P2, DrawUtility::GREEN);
	}

}

///////////////////////////////////////////////////////////////////////////

/* copy one SplinePath to another */

SplinePath &SplinePath::operator=( SplinePath &Source )
{
	/* memcpy */

	memcpy(this, &Source, sizeof(SplinePath));

	/* new copy does not own malloc'd buffers */

	m_u32Flags &= ~(PATH_OWNS_NAME | SEGMENT_PATH_OWNS_SEGMENTS | SEGMENT_PATH_OWNS_POINTS | SPLINE_PATH_OWNS_CONTROL_POINTS);

	/* done */

	return *this;
}

