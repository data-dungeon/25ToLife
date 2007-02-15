///////////////////////////////////////////////////////////////////////////
//
//  SplinePath.h
//
//  A spline path.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////

#ifndef SPLINEPATH_H
#define SPLINEPATH_H


///////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////

#include "path/SegmentPath.h"


///////////////////////////////////////////////////////////////////////////
//  Classes
///////////////////////////////////////////////////////////////////////////


class SplinePath : public SegmentPath
{
	
public:
	
   // Create a new spline path.  The points array is an ordered list of 
	// ( start point, control point 1, control point 2, end point ) quads.
	SplinePath( char* name, Vector3Packed* points, int num_points, bool bForceNoLoop = false );
	SplinePath( char* name, Vector3Padded* points, int num_points, bool bForceNoLoop = false );
	SplinePath(SplinePath *pSource);
	virtual ~SplinePath( void );

	// re-calculate segment lengths as splines instead of straight lines
	void RecalculateLength();

	// Returns the number of control points in the path.
	int NumControlPoints( void )						{ return(m_NumControlPoints); }

	// return control point array for possible point update
	Vector3 *GetControlPointArray()					{ return(m_ControlPoints); }

   // Returns the point on the path at a distance along the path. 
	virtual Vector3 PointFromDistance( float distance );
	
   // Returns the normalized tangent to the path at a distance along the 
   // path. 
	virtual Vector3 TangentFromDistance( float distance );

	// Return the point given a parameter t which goes [0.0f 1.0f]
	Vector3 PointFromT(float t)
		{ return(PointFromDistance(t * Length())); }
	Vector3 TangentFromT(float t)
		{ return(TangentFromDistance(t * Length())); }

	// randomize the path
	void Randomize();

   // Returns the distance along the path at the nearest point on the path 
   // to an arbitrary point.
	//virtual float NearestDistance( Vector3 point );

   // Returns the distance to the path that an arbitrary point is nearest.
	//virtual float NearestApproach( Vector3 point );
	
public:	// debug

	// Draw the path (arcLength parameterization).
	virtual void Draw( float arcLength = 0.25f );

	void DrawSegments(Vector3 MyPoint);
	
protected:
	
	int			m_NumControlPoints;	// Number of points in the path.
	Vector3		*m_ControlPoints;		// List of control points in the path.

	/* can construct a spline path from an existing one */

	SplinePath	*m_pSource;

	/* flag bits-- start after parent class SegmentPath flag bits */

	#define SPLINE_PATH_OWNS_CONTROL_POINTS	(0x00000001 << SEGMENT_PATH_FLAGS_COUNT)

	/* this value should be the number of defined flag bits, plus the number from parent class */

	#define SPLINE_PATH_FLAGS_COUNT				(1 + SEGMENT_PATH_FLAGS_COUNT)

private:

	typedef struct
	{
		float m_Distance;
		float m_T;
		Vector3* m_P0;
		Vector3* m_P1;
		Vector3* m_P2;
		Vector3* m_P3;

		#ifdef _DEBUG
		int m_Misses;
		int m_Hits;
		#endif

	} ts_Cache;

	ts_Cache m_Cache;					// Cache for speed

// operations

public:

	/* overload = operator to make copying easy-- can't just do a memcpy */

	SplinePath &operator=( SplinePath & );
};

///////////////////////////////////////////////////////////////////////////

#endif	// SPLINEPATH_H
