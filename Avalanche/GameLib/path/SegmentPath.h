///////////////////////////////////////////////////////////////////////////
//
//  SegmentPath.h
//
//  A segmented path.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTPATH_H
#define SEGMENTPATH_H


///////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////

#include "path/Path.h"
#include "Math/Vector.h"


///////////////////////////////////////////////////////////////////////////
//  Classes
///////////////////////////////////////////////////////////////////////////

class SegmentPath : public Path
{
	friend class SplinePath;
	friend class ShortcutPath;

public:

	SegmentPath( char* name, Vector3Packed* points, int num_points, int stride = 1, bool bForceNoLoop = false );
	SegmentPath( char* name, Vector3Padded* points, int num_points, int stride = 1, bool bForceNoLoop = false );
	SegmentPath()	{}	// empty constructor for use by SplinePath
	virtual ~SegmentPath( void );

	// Returns the number of points (knots) in the path.
	int NumPoints( void )						{ return(m_NumPoints); }

	// return point array for possible point update
	Vector3 *GetPointArray()					{ return(m_Points); }

	// access to segment lengths
	float GetSegmentLength(int nIndex)		{ ASSERT((nIndex >= 0) && (nIndex < m_NumSegments)); return(m_Segments[nIndex].m_Length); }

	// Returns the distance along the path at an endpoint.
	float DistanceAtEndPoint( int endpoint );

   // Returns the point on the path at a distance along the path.
	virtual Vector3 PointFromDistance( float distance );

   // Returns the normalized tangent to the path at a distance along the
   // path.
	virtual Vector3 TangentFromDistance( float distance );

   // Returns the distance along the path at the nearest point on the path
   // to an arbitrary point.
	virtual float NearestDistance( Vector3 point );

   // Returns the distance squared to the path that an arbitrary point is nearest.
	virtual float NearestApproach( Vector3 point );

   // Returns the endpoints that a given distance is between.
	virtual void BetweenEndPoints( float distance, int& a, int& b );

	// Returns the paths average height above the ground.
	float AverageHeightAboveGround( void );

	// Returns the lowest height of the ground.
	float LowestHeightOfGround( void );

	// Build the segment list.
	void BuildSegments( void );

	// Returns a wrapped (or clamped) distance.
	float WrapDistance( float distance );

	// Returns the segment at distance along the path.
	int SegmentFromDistance( float distance );

public:	// debug

	// Draw the path (segments).
	virtual void Draw();

protected:

	// Returns the endpoint (index into m_Points) nearest an arbitrary point.
	int NearestEndPoint( Vector3 point );

protected:

	struct ts_Segment
	{
		Vector3* m_P1;
		Vector3* m_P2;

		Vector3  m_Direction;	// Normalized direction ( end - start ) / || end - start ||.

		float     m_Length;		// Length of this segment.
		float     m_Cumulative;	// Length of the path up to this segment.
	};

protected:

	int        m_NumPoints;		// Number of points in the path.
	Vector3*   m_Points;			// List of points in the path.

	int         m_NumSegments;		// Number of segments in the path.
	ts_Segment* m_Segments;			// List of the segments in the path.

	float		m_AverageHeightAboveGround;
	float		m_LowestHeightOfGround;

	/* flag bits-- start after parent class Path flag bits */

	#define SEGMENT_PATH_OWNS_POINTS		(0x00000001 << PATH_FLAGS_COUNT)
	#define SEGMENT_PATH_OWNS_SEGMENTS	(0x00000002 << PATH_FLAGS_COUNT)

	/* this value should be the number of defined flag bits, plus the number from parent class */

	#define SEGMENT_PATH_FLAGS_COUNT		(2 + PATH_FLAGS_COUNT)

private:

	struct ts_Cache
	{
		Vector3 m_Point;
		float m_DistanceToPath;
		float m_Distance;
		float m_T;
		int m_S1;
		int m_S2;
		int m_S;

		#ifdef _DEBUG
		int m_Misses;
		int m_Hits;
		#endif
	};

	ts_Cache m_Cache;					// Cache for speed

// debug
#ifdef _DEBUG
private:

	struct ts_Stats
	{
		int m_SegmentFromDistance;
		int m_SegmentFromDistance_Searches;
	};

	ts_Stats m_Stats;
#endif

};

///////////////////////////////////////////////////////////////////////////

#endif	// SEGMENTPATH_H
