///////////////////////////////////////////////////////////////////////////
//
//  ShortcutPath.cpp
//
//  A shortcut path.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////


/******************* includes ****************************/

#include "path/PathPCH.h"





///////////////////////////////////////////////////////////////////////////
//  Implementation
///////////////////////////////////////////////////////////////////////////

ShortcutPath::ShortcutPath( char* name, Vector3Packed* points, int num_points, SplinePath* main_path ) :
SplinePath( name, points, num_points )
{
	m_Main = main_path;

	// Find the start/end points on the main path.
	// The endpoint index doubles as a segment index.
	m_I1 = m_Main->NearestEndPoint( m_Points[ 0 ] );
	m_I2 = m_Main->NearestEndPoint( m_Points[ m_NumPoints - 1 ] );

   // The shortcuts should be specified in the direction of the main path,
	// if they aren't, the data must be changed.
	//ASSERT( m_I1 < m_I2 );
	#ifdef _DEBUG
	if ( m_I1 > m_I2 )
	{
		char s[ 256 ];
		sprintf( s, "*** ShortcutPath: shortcut (%s) is reversed!\n", m_Name );
		conPuts( s );
	}
	#endif

	// The shortcuts must have the first two and last two points on the main path.
	//ASSERT( m_Main->NearestEndPoint( m_Points[ 1 ] ) == m_I1 + 1 );
	//ASSERT( m_Main->NearestEndPoint( m_Points[ m_NumPoints - 2 ] ) == m_I2 - 1 );

	// Sanity check.
	ASSERT( m_I1 < m_Main->m_NumSegments );
	ASSERT( m_I2 < m_Main->m_NumSegments );

	// Compute the lengths of the shortcuts. These need to be computed so 
	// that they reflect the distance traveled along the main path. So, 
	// take the distance at the point on the main path where the shortcut 
	// ends and pro-rate it back to the shortcut beginning.
   float start  = m_Main->m_Segments[ m_I1 ].m_Cumulative;
	float finish = m_Main->m_Segments[ m_I2 ].m_Cumulative;
	float main_length = finish - start;

	int i;
	for ( i = 0; i < m_NumSegments; i++ )
	{
		ts_Segment* s = &( m_Segments[ i ] );

		s->m_Length = ( s->m_Length / m_Length ) * main_length;
	}

	// Compute the cumulative length of each segment.
	float cumulative = start;
	for ( i = 0; i < m_NumSegments; i++ )
	{
		ts_Segment* s = &( m_Segments[ i ] );

		s->m_Cumulative = cumulative;

		cumulative += s->m_Length;
	}

	// Patch up the last segments length.
	m_Segments[ m_NumSegments - 1 ].m_Length = finish - m_Segments[ m_NumSegments - 1 ].m_Cumulative;

	m_Length = main_length;
}

///////////////////////////////////////////////////////////////////////////

ShortcutPath::~ShortcutPath( void )
{
}

///////////////////////////////////////////////////////////////////////////

bool ShortcutPath::Entrance( float distance )
{
	int s = m_Main->SegmentFromDistance( distance );

	if ( s == m_I1 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////

bool ShortcutPath::Exit( float distance )
{
	int s = m_Main->SegmentFromDistance( distance );

	if ( s == m_I2 - 1 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////

bool ShortcutPath::BeyondEnds( float distance )
{
	int s = m_Main->SegmentFromDistance( distance );

	// This will return true if we're anywhere before the start of the 
	// shortcut, or anywhere after the exit of the shortcut.
	if ( s < m_I1 || s > m_I2 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////
