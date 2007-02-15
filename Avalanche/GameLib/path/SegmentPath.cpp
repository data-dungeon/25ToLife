///////////////////////////////////////////////////////////////////////////
//
//  SegmentPath.cpp
//
//  A segmented path.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////


/******************* includes ****************************/

#include "path/PathPCH.h"



#include "Layers/Timing.h"
#include "collide/collsys.h"
#include "CollideHelper/XZEntity.h"

///////////////////////////////////////////////////////////////////////////
//  Implementation
///////////////////////////////////////////////////////////////////////////

SegmentPath::SegmentPath( char* name, Vector3Padded* points, int num_points, int stride, bool bForceNoLoop /*= false*/ ) :
Path( name )
{
	int i;

	// Sanity check.
	ASSERT( ( ( num_points - 1 ) % stride ) == 0 );
	
	// Determine if the path is a closed loop.
	if (bForceNoLoop)
		m_u32Flags &= ~PATH_CLOSED_LOOP;
	else
	{
		Vector3Padded* p0 = &( points[ 0 ] );
		Vector3Padded* p1 = &( points[ num_points - 1 ] );

		// If the endpoints are within one meter of each other, call it 
		// close enough. 
		int ax = ( int )( p0->x() + 0.5f );
		int ay = ( int )( p0->y() + 0.5f );
		int az = ( int )( p0->z() + 0.5f );
		int bx = ( int )( p1->x() + 0.5f );
		int by = ( int )( p1->y() + 0.5f );
		int bz = ( int )( p1->z() + 0.5f );

		if ( ax == bx && ay == by && az == bz )
		{
			m_u32Flags |= PATH_CLOSED_LOOP;
		}
		else
		{
			m_u32Flags &= ~PATH_CLOSED_LOOP;
		}
	}

	// Create the end points.
	m_NumSegments = ( num_points - 1 ) / stride;
	if ( ClosedLoop() )
	{
		m_NumPoints = m_NumSegments;
	}
	else
	{
		m_NumPoints = m_NumSegments + 1;
	}

	m_Points = new Vector3[ m_NumPoints ];
	ASSERT( m_Points );
	m_u32Flags |= SEGMENT_PATH_OWNS_POINTS;
	for ( i = 0; i < m_NumPoints; i++ )
	{
		m_Points[ i ] = points[ i * stride ];
	}
	
	// Create the segments.
	m_Segments = new ts_Segment[ m_NumSegments ];
	ASSERT( m_Segments );
	m_u32Flags |= SEGMENT_PATH_OWNS_SEGMENTS;
	
	BuildSegments();
	
	m_AverageHeightAboveGround = 1e38f;
	m_LowestHeightOfGround = 1e38f;

	m_Cache.m_Point.x( 0.0f );
	m_Cache.m_Point.y( 0.0f );
	m_Cache.m_Point.z( 0.0f );
	m_Cache.m_DistanceToPath = 0;
	m_Cache.m_Distance = 0;
	m_Cache.m_S1 = 0;
	m_Cache.m_S2 = 0;
	m_Cache.m_S = 0;
	m_Cache.m_T = 0;

	// debug
	{
		// Determine the square bounds of the path.
		Vector3 m_MinPt = m_Points[ 0 ];
		Vector3 m_MaxPt = m_Points[ 1 ];
		for ( i = 1; i < m_NumPoints; i++ )
		{
			Vector3* p = &( m_Points[ i ] );

			if ( p->x() < m_MinPt.x() )
			{
				m_MinPt.x( p->x() );
			}
			else if ( p->x() > m_MaxPt.x() )
			{
				m_MaxPt.x( p->x() );
			}
			if ( p->y() < m_MinPt.y() )
			{
				m_MinPt.y( p->y() );
			}
			else if ( p->y() > m_MaxPt.y() )
			{
				m_MaxPt.y( p->y() );
			}
			if ( p->z() < m_MinPt.z() )
			{
				m_MinPt.z( p->z() );
			}
			else if ( p->z() > m_MaxPt.z() )
			{
				m_MaxPt.z( p->z() );
			}
		}

		if ( m_MinPt.x() < m_MinX )
		{
			m_MinX = m_MinPt.x();
		}
		if ( m_MaxPt.x() > m_MaxX )
		{
			m_MaxX = m_MaxPt.x();
		}
		if ( m_MinPt.z() < m_MinZ )
		{
			m_MinZ = m_MinPt.z();
		}
		if ( m_MaxPt.z() > m_MaxZ )
		{
			m_MaxZ = m_MaxPt.z();
		}
		
		if ( ( m_MaxX - m_MinX ) > ( m_MaxZ - m_MinZ ) )
		{
			m_Min = m_MinX;
			m_Max = m_MaxX;
			
			m_Trans.x( 0.0f );
			m_Trans.z( ( m_Max + m_Min ) / 2.0f - ( m_MaxZ + m_MinZ ) / 2.0f );
		}
		else
		{
			m_Min = m_MinZ;
			m_Max = m_MaxZ;
			
			m_Trans.z( 0.0f );
			m_Trans.x( ( m_Max + m_Min ) / 2.0f - ( m_MaxX + m_MinX ) / 2.0f );
		}

//		m_Min -= ( m_Max - m_Min ) * 0.01f;
//		m_Max += ( m_Max - m_Min ) * 0.01f;

		#ifdef _DEBUG
		m_Stats.m_SegmentFromDistance = 0;
		m_Stats.m_SegmentFromDistance_Searches = 0;
		#endif

		#ifdef _DEBUG
		m_Cache.m_Misses = 0;
		m_Cache.m_Hits = 0;
		#endif
	}
}

SegmentPath::SegmentPath( char* name, Vector3Packed* points, int num_points, int stride, bool bForceNoLoop /*= false*/ ) :
Path( name )
{
	int i;

	// Sanity check.
	ASSERT( ( ( num_points - 1 ) % stride ) == 0 );
	
	// Determine if the path is a closed loop.
	if (bForceNoLoop)
		m_u32Flags &= ~PATH_CLOSED_LOOP;
	else
	{
		Vector3Packed* p0 = &( points[ 0 ] );
		Vector3Packed* p1 = &( points[ num_points - 1 ] );

		// If the endpoints are within one meter of each other, call it 
		// close enough. 
		int ax = ( int )( p0->x() + 0.5f );
		int ay = ( int )( p0->y() + 0.5f );
		int az = ( int )( p0->z() + 0.5f );
		int bx = ( int )( p1->x() + 0.5f );
		int by = ( int )( p1->y() + 0.5f );
		int bz = ( int )( p1->z() + 0.5f );

		if ( ax == bx && ay == by && az == bz )
		{
			m_u32Flags |= PATH_CLOSED_LOOP;
		}
		else
		{
			m_u32Flags &= ~PATH_CLOSED_LOOP;
		}
	}

	// Create the end points.
	m_NumSegments = ( num_points - 1 ) / stride;
	if ( ClosedLoop() )
	{
		m_NumPoints = m_NumSegments;
	}
	else
	{
		m_NumPoints = m_NumSegments + 1;
	}

	m_Points = new Vector3[ m_NumPoints ];
	ASSERT( m_Points );
	m_u32Flags |= SEGMENT_PATH_OWNS_POINTS;
	for ( i = 0; i < m_NumPoints; i++ )
	{
		m_Points[ i ] = points[ i * stride ];
	}
	
	// Create the segments.
	m_Segments = new ts_Segment[ m_NumSegments ];
	ASSERT( m_Segments );
	m_u32Flags |= SEGMENT_PATH_OWNS_SEGMENTS;
	
	BuildSegments();
	
	m_AverageHeightAboveGround = 1e38f;
	m_LowestHeightOfGround = 1e38f;

	m_Cache.m_Point.x( 0.0f );
	m_Cache.m_Point.y( 0.0f );
	m_Cache.m_Point.z( 0.0f );
	m_Cache.m_DistanceToPath = 0;
	m_Cache.m_Distance = 0;
	m_Cache.m_S1 = 0;
	m_Cache.m_S2 = 0;
	m_Cache.m_S = 0;
	m_Cache.m_T = 0;

	// debug
	{
		// Determine the square bounds of the path.
		Vector3 m_MinPt = m_Points[ 0 ];
		Vector3 m_MaxPt = m_Points[ 1 ];
		for ( i = 1; i < m_NumPoints; i++ )
		{
			Vector3* p = &( m_Points[ i ] );

			if ( p->x() < m_MinPt.x() )
			{
				m_MinPt.x( p->x() );
			}
			else if ( p->x() > m_MaxPt.x() )
			{
				m_MaxPt.x( p->x() );
			}
			if ( p->y() < m_MinPt.y() )
			{
				m_MinPt.y( p->y() );
			}
			else if ( p->y() > m_MaxPt.y() )
			{
				m_MaxPt.y( p->y() );
			}
			if ( p->z() < m_MinPt.z() )
			{
				m_MinPt.z( p->z() );
			}
			else if ( p->z() > m_MaxPt.z() )
			{
				m_MaxPt.z( p->z() );
			}
		}

		if ( m_MinPt.x() < m_MinX )
		{
			m_MinX = m_MinPt.x();
		}
		if ( m_MaxPt.x() > m_MaxX )
		{
			m_MaxX = m_MaxPt.x();
		}
		if ( m_MinPt.z() < m_MinZ )
		{
			m_MinZ = m_MinPt.z();
		}
		if ( m_MaxPt.z() > m_MaxZ )
		{
			m_MaxZ = m_MaxPt.z();
		}
		
		if ( ( m_MaxX - m_MinX ) > ( m_MaxZ - m_MinZ ) )
		{
			m_Min = m_MinX;
			m_Max = m_MaxX;
			
			m_Trans.x( 0.0f );
			m_Trans.z( ( m_Max + m_Min ) / 2.0f - ( m_MaxZ + m_MinZ ) / 2.0f );
		}
		else
		{
			m_Min = m_MinZ;
			m_Max = m_MaxZ;
			
			m_Trans.z( 0.0f );
			m_Trans.x( ( m_Max + m_Min ) / 2.0f - ( m_MaxX + m_MinX ) / 2.0f );
		}

//		m_Min -= ( m_Max - m_Min ) * 0.01f;
//		m_Max += ( m_Max - m_Min ) * 0.01f;

		#ifdef _DEBUG
		m_Stats.m_SegmentFromDistance = 0;
		m_Stats.m_SegmentFromDistance_Searches = 0;
		#endif

		#ifdef _DEBUG
		m_Cache.m_Misses = 0;
		m_Cache.m_Hits = 0;
		#endif
	}
}

///////////////////////////////////////////////////////////////////////////

SegmentPath::~SegmentPath( void )
{
	if (m_u32Flags & SEGMENT_PATH_OWNS_POINTS)
		delete [] m_Points;
	if (m_u32Flags & SEGMENT_PATH_OWNS_SEGMENTS)
		delete [] m_Segments;
}

///////////////////////////////////////////////////////////////////////////

float SegmentPath::DistanceAtEndPoint( int endpoint )
{
	ASSERT( endpoint >= 0 && endpoint <= m_NumSegments );

	return m_Segments[ endpoint ].m_Cumulative;
}

///////////////////////////////////////////////////////////////////////////

Vector3 SegmentPath::PointFromDistance( float distance )
{
	Vector3 p;

#ifndef SHIPIT
    Profiler__Start(g_pathProfileSlot);
#endif
	// Find the segment that the distance is on.
	if ( m_Cache.m_Distance != distance )
	{
		int s = SegmentFromDistance( distance );
		
		m_Cache.m_Distance = distance;
		m_Cache.m_S = s;
		
		// Compute distance along the segment.
		float d = distance - m_Segments[ s ].m_Cumulative;
		
		// Compute the point on the segment.
		Vector3& start     = *m_Segments[ s ].m_P1;
		Vector3& direction = m_Segments[ s ].m_Direction;
		
		p = start + ( direction * d );

		m_Cache.m_Point = p;

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
    Profiler__Stop(g_pathProfileSlot);
#endif

	return m_Cache.m_Point;
}

///////////////////////////////////////////////////////////////////////////

Vector3 SegmentPath::TangentFromDistance( float distance )
{
#ifndef SHIPIT
    Profiler__Start(g_pathProfileSlot);
#endif
	distance = WrapDistance( distance );

	if ( m_Cache.m_Distance != distance )
	{
		int s = SegmentFromDistance( distance );

		m_Cache.m_Distance = distance;
		m_Cache.m_S = s;

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
    Profiler__Stop(g_pathProfileSlot);
#endif

	return m_Segments[ m_Cache.m_S ].m_Direction;
}

///////////////////////////////////////////////////////////////////////////

float SegmentPath::NearestDistance( Vector3 point )
{
	int   s, s1, s2;
	float t, t1, t2;
	float d, d1, d2;

	if ( point.x() < -1e6f || point.x() > 1e6f ||
		  point.y() < -1e6f || point.y() > 1e6f ||
		  point.z() < -1e6f || point.y() > 1e6f )
	{
		TRACE( "WARNING: Bike in boonies (%5g, %5g, %5g), warping to (%5g, %5g, %5g)\n", point.x(), point.y(), point.z(), m_Points[ 0 ].x(), m_Points[ 0 ].y(), m_Points[ 0 ].z() );

		point = m_Points[ 0 ];
	}

#ifndef SHIPIT
    Profiler__Start(g_pathProfileSlot);
#endif
	if ( m_Cache.m_Point.x() != point.x() ||
		  m_Cache.m_Point.y() != point.y() ||
		  m_Cache.m_Point.z() != point.z() )
	{
		// Find the nearest endpoint.
		int p = NearestEndPoint( point );

		// Determine which two segments (indices) share the point.
		s2 = p;

		if ( ClosedLoop() )
		{
			// Sanity check.
			ASSERT( s2 >= 0 && s2 < m_NumSegments );

			if ( s2 == 0 )
			{
				s1 = m_NumSegments - 1;
			}
			else
			{
				s1 = s2 - 1;
			}
		}
		else
		{
			// Sanity check.
			ASSERT( s2 >= 0 && s2 <= m_NumSegments );

			if ( s2 == 0 )
			{
				s1 = 0;
			}
			else if ( s2 == m_NumSegments )
			{
				s2 = m_NumSegments - 1;
				s1 = m_NumSegments - 1;
			}
			else
			{
				s1 = s2 - 1;
			}
		}

		// Determine distance to each segment that shares the point (and get a 
		// t [0,1] along the segment), and store the nearer one. 
		{
			Vector3* s1p1 = m_Segments[ s1 ].m_P1;
			Vector3* s1p2 = m_Segments[ s1 ].m_P2;
			Vector3* s2p1 = m_Segments[ s2 ].m_P1;
			Vector3* s2p2 = m_Segments[ s2 ].m_P2;

			d1 = SquaredDistance_PointToLine( point, *s1p1, *s1p2, &t1 );
			d2 = SquaredDistance_PointToLine( point, *s2p1, *s2p2, &t2 );

			if ( d1 < d2 )
			{
				t = t1;
				s = s1;
				d = d1;
			}
			else
			{
				t = t2;
				s = s2;
				d = d2;
			}
		}

		// Cache.
		m_Cache.m_Point = point;
		m_Cache.m_DistanceToPath = d;

		// Convert segment t into a distance along the path.
		ts_Segment* segment = &( m_Segments[ s ] );
		d = segment->m_Cumulative + ( t * segment->m_Length );

		// Cache.
		m_Cache.m_Distance = d;
		m_Cache.m_T = t;
		m_Cache.m_S1 = s1;
		m_Cache.m_S2 = s2;

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

//	if ( point_on_path )
//	{
//		Vector3* a = s->start;
//		Vector3* b = s->end;
//		Vector3 v = *b - *a;
//
//		v.x( a->x() + v.x() * min_t);
//		v.y( a->y() + v.y() * min_t);
//		v.z( a->z() + v.z() * min_t);
//
//		*point_on_path = v;
//	}

#ifndef SHIPIT
    Profiler__Stop(g_pathProfileSlot);
#endif

	return m_Cache.m_Distance;
}

///////////////////////////////////////////////////////////////////////////

float SegmentPath::NearestApproach( Vector3 point )
{
	if ( m_Cache.m_Point != point)
	{
		NearestDistance( point );

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

	return m_Cache.m_DistanceToPath;
}

////////////////////////////////////////////////////////////////////////////

void SegmentPath::BetweenEndPoints( float distance, int& a, int& b )
{
	int s = SegmentFromDistance( distance );

	a = s;

	if ( a < m_NumPoints - 1 )
	{
		b = s + 1;
	}
	else
	{
		b = s;
	}
}

////////////////////////////////////////////////////////////////////////////

float SegmentPath::AverageHeightAboveGround( void )
{
	if ( m_AverageHeightAboveGround > 1e37f )
	{
		// Compute average height above ground.
		float y = 0;
		int missedPoints = 0;
		for ( int i = 0; i < m_NumPoints; i++ )
		{
			Vector3* p = &( m_Points[ i ] );

			bool hit;
			CXZEntity entity;
			entity.SetPositiveInclude(WORLD_CATEGORY);
			entity.FindFloor(p->x(), p->y() + 1.0f, p->z());

			g_collisionSystem.ResetStack();
			hit = (g_collisionSystem.Test(&entity) != NULL);
			
			// If we didn't get a hit the first time, try from higher up (we could be below the ground).
			if ( ! hit )
			{
				entity.FindHeight();
				hit = (g_collisionSystem.Test(&entity) != NULL);
				
				// We better have a hit now!
				if ( !hit )
				{
					missedPoints++;
				}
			}

			if ( ( hit ) && ( entity.YVal() < p->y() ) )
			{
				y += ( p->y() - entity.YVal() );
			}
		}

		ASSERT( m_NumPoints > missedPoints );

		m_AverageHeightAboveGround = y / ( m_NumPoints - missedPoints );
	}

	return m_AverageHeightAboveGround;
}

////////////////////////////////////////////////////////////////////////////

float SegmentPath::LowestHeightOfGround( void )
{
	if ( m_LowestHeightOfGround > 1e37f )
	{
		// Compute lowest point on the ground.
		float y = 0;
		int missedPoints = 0;
		for ( int i = 0; i < m_NumPoints; i++ )
		{
			Vector3* p = &( m_Points[ i ] );

			bool hit;
			CXZEntity entity;
			entity.SetPositiveInclude(WORLD_CATEGORY);
			entity.FindFloor(p->x(), p->y() + 1.0f, p->z());

			g_collisionSystem.ResetStack();
			hit = (g_collisionSystem.Test(&entity) != NULL);
			
			// If we didn't get a hit the first time, try from higher up (we could be below the ground).
			if ( ! hit )
			{
				entity.FindHeight();
				hit = (g_collisionSystem.Test(&entity) != NULL);
				
				// We better have a hit now!
				if ( !hit )
				{
					missedPoints++;
				}
			}

			if ( hit )
			{
				float y = entity.YVal();

				if ( y < m_LowestHeightOfGround )
				{
					m_LowestHeightOfGround = y;
				}
			}
		}
	}

	return m_LowestHeightOfGround;
}


///////////////////////////////////////////////////////////////////////////
// Private
///////////////////////////////////////////////////////////////////////////

float SegmentPath::WrapDistance( float distance )
{
	//ASSERT( distance >= 0.0f );

	if ( distance < -1e6f || distance > 1e6f )
	{
		TRACE( "WARNING: Bike in boonies (d = %g), warping back to (d = 0)\n", distance );

		distance = 0;
	}

	// Wrap or clamp the distance depending on path type.

	// If a path is a shortcut, clamp the distance to the ends of the shortcut.
	if ( m_Segments[ 0 ].m_Cumulative != 0 )
	{
		float start = m_Segments[ 0 ].m_Cumulative;
		float end   = m_Segments[ m_NumSegments - 1 ].m_Cumulative + m_Segments[ m_NumSegments - 1 ].m_Length;

		if ( distance < start )
		{
			distance = start;
		}
		else if ( distance > end )
		{
			distance = end;
		}
	}
	else
	{
		if ( ClosedLoop() )
		{
			while ( distance < 0 )
			{
				distance += m_Length;

				#ifdef _DEBUG
				{
					static int n = 0;

					n++;
					if ( n > 1 )
					{
//						conPuts( "*** SegmentPath: distance is _WAY_ out of range.\n" );
					}
				}
				#endif
			}

			while ( distance > m_Length )
			{
				distance -= m_Length;

				#ifdef _DEBUG
				{
					static int n = 0;

					n++;
					if ( n > 3 )
					{
//						conPuts( "*** SegmentPath: distance is _WAY_ out of range.\n" );
					}
				}
				#endif
			}
		}
		else
		{
			if ( distance < 0 )
			{
				distance = 0;
				//conPuts( "SegmentPath: clamping distance on open loop path.\n" );
			}
			else
			if ( distance > m_Length )
			{
				distance = m_Length;
				//conPuts( "SegmentPath: clamping distance on open loop path.\n" );
			}
		}
	}

	return distance;
}

///////////////////////////////////////////////////////////////////////////

int SegmentPath::SegmentFromDistance( float distance )
{
	float d;
	
	distance = WrapDistance( distance );

	#ifdef _DEBUG
	m_Stats.m_SegmentFromDistance++;
	#endif
	
	// Any path that has a first segment with a cumulative distance > 0 is 
	// attached to the main path at that distance (i.e., a shortcut).  So, 
	// to calculate the correct segment, a distance that is relative to the 
	// path start must be used.
	d = distance - m_Segments[ 0 ].m_Cumulative;
	
	// (Sort-of) smart search. Assume that all the segments are the same 
	// length. If this is the case, then distance * m_NumSegments / m_Length
	// should give us the right segment. So, do a linear search from this 
	// first good guess. 
	int s = ( int )( ( d * ( m_NumSegments - 1 ) ) / m_Length );
	
	// Sanity check.
	ASSERT( s >= 0 && s < m_NumSegments );
	
	if ( distance < m_Segments[ s ].m_Cumulative )
	{
      // Search toward the beginning of the list.
      // If the search has arrived at the first segment, it must be the 
      // one, since the distance should be bounded by [0,m_Length].
		while ( s > 0 && distance < m_Segments[ s ].m_Cumulative )
		{
			s--;

			#ifdef _DEBUG
			m_Stats.m_SegmentFromDistance_Searches++;
			#endif
		}
	}
	else
	{
		// Search toward the end of the list.
      // If the search has arrived at the last segment, it must be the 
      // one, since the distance should be bounded by [0,m_Length].
		while ( s < m_NumSegments - 1 && distance > m_Segments[ s + 1 ].m_Cumulative )
		{
			s++;

			#ifdef _DEBUG
			m_Stats.m_SegmentFromDistance_Searches++;
			#endif
		}
	}
	
	// Sanity check.
	ASSERT( s >= 0 && s < m_NumSegments );

	return s;
}

///////////////////////////////////////////////////////////////////////////

int SegmentPath::NearestEndPoint( Vector3 point )
{
	int i;
	int nearest = 0;
	float min_distance = 1e38f;

	// Sanity checks.
	ASSERT( point.x() < 1e5f );
	ASSERT( point.y() < 1e5f );
	ASSERT( point.z() < 1e5f );

	for ( i = 0; i < m_NumPoints; i++ )
	{
		float d;
		
		d = SquaredDistance_PointToPoint( m_Points[ i ], point );

		if ( d < min_distance )
		{
			nearest = i;
			min_distance = d;
		}
	}
	
	// Sanity check.
	ASSERT( nearest >= 0 && nearest < m_NumPoints );

	return nearest;
}

///////////////////////////////////////////////////////////////////////////

void SegmentPath::BuildSegments( void )
{
	int i;
	
	// Attach segment points.
	if ( ClosedLoop() )
	{
		// The number of points is equal to the number of segments.
		for ( i = 0; i < m_NumSegments - 1; i++ )
		{
			m_Segments[ i ].m_P1 = &( m_Points[ i ] );
			m_Segments[ i ].m_P2 = &( m_Points[ i + 1 ] );
		}
		
		// Close the loop.
		m_Segments[ m_NumSegments - 1 ].m_P1 = &( m_Points[ m_NumPoints - 1 ] );
		m_Segments[ m_NumSegments - 1 ].m_P2 = &( m_Points[ 0 ] );
	}
	else
	{
		// The number of points is 1 greater than the number of segments.
		for ( i = 0; i < m_NumSegments; i++ )
		{
			m_Segments[ i ].m_P1 = &( m_Points[ i ] );
			m_Segments[ i ].m_P2 = &( m_Points[ i + 1 ] );
		}
	}
	
	m_Length = 0;
	for ( i = 0; i < m_NumSegments; i++ )
	{
		Vector3* p0 = m_Segments[ i ].m_P1;
		Vector3* p1 = m_Segments[ i ].m_P2;
		Vector3  v = *p1 - *p0;
		
		m_Segments[ i ].m_Length = v.Length();
		
		m_Length += m_Segments[ i ].m_Length;
		
		float l = m_Segments[ i ].m_Length;
		
		if ( l != 0 )
		{
			l = 1.0f / l;
			v *= l;
		}
		
		m_Segments[ i ].m_Direction = v;
	}
	
	// Compute cumulative lengths for each segment.
	float cumulative = 0;
	for ( i = 0; i < m_NumSegments; i++ )
	{
		m_Segments[ i ].m_Cumulative = cumulative;
		
		cumulative += m_Segments[ i ].m_Length;
	}
}

///////////////////////////////////////////////////////////////////////////

void SegmentPath::Draw()
{
	for ( int i = 0; i < m_NumPoints; i++ )
	{
		// draw a line between the points on the curve.
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
		DrawUtility::Line( p0, p1, DrawUtility::CYAN );
		
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
	}
}

///////////////////////////////////////////////////////////////////////////
