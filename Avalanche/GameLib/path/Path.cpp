///////////////////////////////////////////////////////////////////////////
//
//  Path.cpp
//
//  A base-class for paths.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////


/******************* includes ****************************/

#include "path/PathPCH.h"





///////////////////////////////////////////////////////////////////////////
//  Globals
///////////////////////////////////////////////////////////////////////////

#ifndef SHIPIT  // For profiling
#include "Layers/Timing.h"
u32 g_pathProfileSlot = PROFILE_INVALID_SLOT;
#endif

ts_SplinePathTable* gbl_paths = NULL;


///////////////////////////////////////////////////////////////////////////
//  Defines
///////////////////////////////////////////////////////////////////////////


// All checks initially set to 2D, now set to 3D checks by setting appropriate
// flag by calling the method SetCalculateIn3D(bool flag);  -BHY

//#define DISTANCES_IN_2D <-- Obsolete  -BHY


///////////////////////////////////////////////////////////////////////////
//  Statics
///////////////////////////////////////////////////////////////////////////

float Path::m_MaxX = -1e38f;
float Path::m_MinX =  1e38f;
float Path::m_MaxZ = -1e38f;
float Path::m_MinZ =  1e38f;
float Path::m_Max = -1e38f;
float Path::m_Min =  1e38f;
Vector3 Path::m_Trans( 0.0f, 0.0f, 0.0f );


///////////////////////////////////////////////////////////////////////////
//  Implementation
///////////////////////////////////////////////////////////////////////////

Path::Path( char* name )
{
	m_Name = new char[ strlen( name ) + 1 ];
	ASSERT( m_Name );
	strcpy( m_Name, name );

	m_Length = 0;

	m_u32Flags = PATH_OWNS_NAME;
	bIsDisabled = FALSE;

	CalculateIn3D = false;
}

///////////////////////////////////////////////////////////////////////////

Path::~Path( void )
{
	if (( m_Name ) && (m_u32Flags & PATH_OWNS_NAME))
	{
		delete [] m_Name;
	}
}

///////////////////////////////////////////////////////////////////////////

float Path::Length( void )
{
	return m_Length;
}

///////////////////////////////////////////////////////////////////////////

bool Path::ClosedLoop( void )
{
	return((m_u32Flags & PATH_CLOSED_LOOP) != 0);
}

///////////////////////////////////////////////////////////////////////////

void Path::SetCalculateIn3D(bool flag)
{
	CalculateIn3D=flag;
}


///////////////////////////////////////////////////////////////////////////
//  Private
///////////////////////////////////////////////////////////////////////////

float Path::SquaredDistance_PointToPoint( const Vector3& a, const Vector3& b ) const
{
	Vector3 ab( b.x() - a.x(), b.y() - a.y(), b.z() - a.z() );

//	#ifdef DISTANCES_IN_2D
	if(!CalculateIn3D)
	{
		ab.y( 0.0f );
	}
//	#endif

	return ( ab.x() * ab.x() ) + ( ab.y() * ab.y() ) + ( ab.z() * ab.z() );
}

///////////////////////////////////////////////////////////////////////////

float Path::SquaredDistance_PointToLine( const Vector3& p, const Vector3& a, const Vector3& b, float* t ) const
{
	Vector3 ab = b - a;
	Vector3 ap = p - a;

//	#ifdef DISTANCES_IN_2D
	if(!CalculateIn3D)
	{
		ab.y( 0.0f );
		ap.y( 0.0f );
	}
//	#endif

	float ap_dot_ab = ap.Dot( ab );

	float ab_sqr_len = ab.LengthSquared();

	float s;

	if ( Math::Equal( ab_sqr_len, 0.0f ) )
	{
		s = 0.0f;
	}
	else
	{
		s = ap_dot_ab / ab_sqr_len;
	}

	if ( s < 0.0f )
	{
		// The point is beyond the line in the b->a direction.
		// Return distance squared from p to a.

		if ( t )
		{
			*t = s;
		}

		return ap.LengthSquared();
	}
	else if ( s > 1.0f )
	{
		// The point is beyond the line in the a->b direction.
		// Return distance squared from p to b.

		Vector3 bp = p - b;

//	    #ifdef DISTANCES_IN_2D
		if(!CalculateIn3D)
		{
			bp.y( 0.0f );
		}
//	    #endif

		if ( t )
		{
			*t = s;
		}


		return bp.LengthSquared();
	}
	else
	{
      // The point is not beyond either end of the line.
      // Return the distance squared from p to c, where c is the closest
      // point to p on ab.

		Vector3 c = a + ( ab * s );

		Vector3 cp = p - c;

//	    #ifdef DISTANCES_IN_2D
		if(!CalculateIn3D)
		{
			cp.y( 0.0f );
		}
//	    #endif

		if ( t )
		{
			*t = s;
		}
		
		return cp.LengthSquared();
	}
}

///////////////////////////////////////////////////////////////////////////
