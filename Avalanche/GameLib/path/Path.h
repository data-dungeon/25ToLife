///////////////////////////////////////////////////////////////////////////
//
//  Path.h
//
//  A base-class for paths.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////

#ifndef PATH_H
#define PATH_H


///////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////

#include "platform/BaseType.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

#ifndef SHIPIT  // For profiling
extern u32 g_pathProfileSlot;
#endif

#if 1	// RIP THIS OUT EVENTUALLY
// ===========================================================================
//	STRUCT: ts_SplinePath

struct ts_SplinePath
{
	char						name[64];
	int						pointCount;
	int						flags;
	Vector3Packed			pointList[1];
};

// ===========================================================================
//	STRUCT: ts_SplinePathTable
//
//	This structure is setup to mimic the ts_RecordList structure.

struct ts_SplinePathTable
{
	unsigned short			pathCount;
	unsigned short			padding;
	ts_SplinePath*			pathList[1];
};

// ===========================================================================
//	STRUCT: ts_NamedPoint

struct ts_NamedPoint
{
	char						name[64];
	Graphics4x4				matrix;
};

// ===========================================================================
//	STRUCT: ts_NamedPointTable
//
//	This structure is setup to mimic the ts_RecordList structure.

struct ts_NamedPointTable
{
	unsigned short			pointCount;
	unsigned char			padding[6];
	ts_NamedPoint			pointList[1];
};

extern ts_SplinePathTable* gbl_paths;
extern ts_NamedPointTable* gbl_points;

#endif


///////////////////////////////////////////////////////////////////////////
//  Classes
///////////////////////////////////////////////////////////////////////////

class Path
{
public:

	Path( char* name );
	Path()	{}
	virtual ~Path( void );

	const char *Name( void ) { return m_Name; };

	// Returns the length of the path.
	float Length( void );

   // Returns true if the path is a closed loop, false otherwise.
	bool ClosedLoop( void );

   // Returns the point on the path at a distance along the path.
	virtual Vector3 PointFromDistance( float distance ) = 0;

   // Returns the normalized tangent to the path at a distance along the
   // path.
	virtual Vector3 TangentFromDistance( float distance ) = 0;

   // Returns the distance along the path at the nearest point on the path
   // to an arbitrary point.
	virtual float NearestDistance( Vector3 point ) = 0;

   // Returns the distance to the path that an arbitrary point is nearest.
	virtual float NearestApproach( Vector3 point ) = 0;

	void SetCalculateIn3D(bool flag);
	
	bool bIsDisabled;  // I'm letting Game logic decide this Not Saving on Purpose
							// Call Jon M. if you make this Savable.  MtnTop would have to change.

protected:

	// Returns the squared distance from point a to point b.
	float SquaredDistance_PointToPoint( const Vector3& a, const Vector3& b ) const;

	// Return distance squared from point p to segment ab.
	//
	// If the point is beyond the line in the b->a direction, the distance
	// squared from p to a is returned.
	// If the point is beyond the line in the a->b direction, the distance
	// squared from p to b is returned.
	//
	// If the t return parameter is non-NULL, the t value along the segment is
	// returned.
	float SquaredDistance_PointToLine( const Vector3& p, const Vector3& a, const Vector3& b, float* t = NULL ) const;

protected:

	char*	m_Name;				// Name of this path.

	float	m_Length;			// Length of the path.

	u32	m_u32Flags;			// path flags, additional flag bits are added by derived classes

	// debug
	static float m_Min;
	static float m_Max;
	static float m_MinX;
	static float m_MaxX;
	static float m_MinZ;
	static float m_MaxZ;
	static Vector3 m_Trans;

	/* flag bits */

	#define PATH_OWNS_NAME		0x00000001
	#define PATH_CLOSED_LOOP	0x00000002

	/* this value should be the number of defined flag bits */

	#define PATH_FLAGS_COUNT	2

private:

	// This flag is initially set to false upon initialization
	// All Calculations were originally performed in 2D by Nate in order to get better performance
	// In the case where you want the 3D calulations made for accuracy, set the flag by calling
	// calling the accessor method SetCalculateIn3D(bool flag)  -BHY
	bool CalculateIn3D;
};

///////////////////////////////////////////////////////////////////////////

#endif	// PATH_H
