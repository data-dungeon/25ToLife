///////////////////////////////////////////////////////////////////////////
//
//  RacePaths.cpp
//
//  A container for RoadRash paths.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#ifdef PS2
#include <ctype.h>
#endif
#ifdef GCN
#include <ctype.h>
#endif


#include "Layers/Debug.h"

#include "cmdcon/cmdcon.h"

#include "path/RacePaths.h"


///////////////////////////////////////////////////////////////////////////
//  Globals
///////////////////////////////////////////////////////////////////////////

RacePaths* g_Paths;

ts_NamedPointTable* gbl_points = 0;


///////////////////////////////////////////////////////////////////////////
//  Statics
///////////////////////////////////////////////////////////////////////////

static bool dbg_Draw3D = false;
bool dbg_Draw2D = false;
static bool dbg_DrawMain = true;
static bool dbg_DrawShortcuts = true;
static bool dbg_DrawEnvs = true;
static bool dbg_DrawArcLength = false;
bool dbg_DrawRacers = true;


///////////////////////////////////////////////////////////////////////////
//  Private Functions
///////////////////////////////////////////////////////////////////////////

// Case insensitive substring compare (where substring means up to the
// shorter of the two strings). Return 1 (true) if the substrings match, 0
// (false) if they don't.
static int StringCompare( char* s0, char* s1 )
{
	int i;

	ASSERT( s0 );
	ASSERT( s1 );

	int l0 = strlen( s0 );
	int l1 = strlen( s1 );
	int len = l0 < l1 ? l0 : l1;

	for ( i = 0; i < len; i++ )
	{
		if ( tolower( *s0 ) != tolower( *s1 ) )
		{
			return 0;
		}

		s0++;
		s1++;
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////

// Return 1 (true) if the path number is a shortcut path, or 0 (false) if
// it isn't.
static int IsShortcut( char* name )
{
	return StringCompare( "SHORTCUT", name );
}

////////////////////////////////////////////////////////////////////////////

// Return 1 (true) if the path number is an environmental path, or 0
// (false) if it isn't.
static int IsEnv( char* name )
{
	return StringCompare( "ENV", name );
}

////////////////////////////////////////////////////////////////////////////

// Return 1 (true) if the path number is a circuit path, or 0 (false) if it
// isn't.
static int IsMain( char* name )
{
	return StringCompare( "CIRCUIT", name );
}


///////////////////////////////////////////////////////////////////////////
//  Implementation
///////////////////////////////////////////////////////////////////////////

RacePaths::RacePaths( ts_SplinePathTable* paths )
{
	int i;

	m_Main = NULL;
	m_Shortcuts = NULL;
	m_Envs = NULL;

	//ASSERT( paths != NULL );	// There aren't any paths in the dbl file!
	if ( paths == NULL )
	{
		conPuts( "No paths found in DBL file!\n" );
		return;
	}

	// Count the number of each path type.
	m_NumShortcuts = 0;
	m_NumEnvs = 0;
	int circuits = 0;
	for ( i = 0; i < paths->pathCount; i++ )
	{
		ts_SplinePath* p = paths->pathList[ i ];

		if ( IsShortcut( p->name ) )
		{
			m_NumShortcuts++;
		}
		else if ( IsEnv( p->name ) )
		{
			m_NumEnvs++;
		}
		else if ( IsMain( p->name ) )
		{
			// Make sure that we create only one main circuit!
			ASSERT( m_Main == NULL );			// If this fires, there are two paths named "circuit" in the dbl.
			m_Main = new SplinePath( p->name, p->pointList, p->pointCount );
		}
		else
		{
			// no more.  these ASSERT assumptions are way too Road Rash specific
			// to be in GameLib.  But hey, so is this whole class, which is why I'm
			// writing PathCollection.  TN
			//ASSERT( 0 );	// Invalid path type (the path is probably named improperly).
		}
	}

	// Sanity check.
	// TN - An insane sanity check, since not all games will have a race course.
	// or did somebody change GameLib to RaceLib while I wasn't looking?
	//ASSERT( m_Main );	// There is no circuit path (none named "circuit" in the dbl).

#if 0	// N8: !!! - until the art is up to speed, we've got to take this into account.
if ( _RacePaths.num_circuits == 0 )
{
	delete [] _RacePaths.all[ 0 ]->Name;

	_RacePaths.all[ 0 ]->Name = new char[ strlen( TOKEN_CIRCUIT ) + 1 ];
	ASSERT( _RacePaths.all[ 0 ]->Name );
	memcpy( _RacePaths.all[ 0 ]->Name, TOKEN_CIRCUIT, strlen( TOKEN_CIRCUIT ) + 1 );

	_RacePaths.num_circuits = 1;
}
#endif

	// Allocate memory for each path type.
	if ( m_NumShortcuts > 0 )
	{
		m_Shortcuts = new ShortcutPath*[ m_NumShortcuts ];
		ASSERT( m_Shortcuts );
	}

	if ( m_NumEnvs > 0 )
	{
		m_Envs = new SplinePath*[ m_NumEnvs ];
		ASSERT( m_Envs );
	}

	// Categorize each path.
	m_NumShortcuts = 0;
	m_NumEnvs		= 0;
	for ( i = 0; i < paths->pathCount; i++ )
	{
		ts_SplinePath* p = paths->pathList[ i ];

		if ( IsShortcut( p->name ) )
		{
			ASSERT( m_Main );	// The main circuit must be the first path in the dlb chunk!
			m_Shortcuts[ m_NumShortcuts ] = new ShortcutPath( p->name, p->pointList, p->pointCount, m_Main );
			m_NumShortcuts++;
		}
		else if ( IsEnv( p->name ) )
		{
			m_Envs[ m_NumEnvs ] = new SplinePath( p->name, p->pointList, p->pointCount );
			m_NumEnvs++;
		}
		else if ( IsMain( p->name ) )
		{
			// already created it above.
		}
		else
		{
			// no more.  these ASSERT assumptions are way too Road Rash specific
			// to be in GameLib.  But hey, so is this whole class, which is why I'm
			// writing PathCollection.  TN
			//ASSERT( 0 );	// Invalid path type (the path is probably named improperly).
		}
	}

	{
    	char* context = "paths";

		g_console.CreateVar( context, "2D", dbg_Draw2D );
		g_console.CreateVar( context, "3D", dbg_Draw3D );
		g_console.CreateVar( context, "main", dbg_DrawMain );
		g_console.CreateVar( context, "shortcuts", dbg_DrawShortcuts );
		g_console.CreateVar( context, "envs", dbg_DrawEnvs );
		g_console.CreateVar( context, "arclength", dbg_DrawArcLength );
		g_console.CreateVar( context, "racers", dbg_DrawRacers );
	}
}

///////////////////////////////////////////////////////////////////////////

RacePaths::~RacePaths( void )
{
	if ( m_Shortcuts )
	{
		for ( int i = 0; i < m_NumShortcuts; i++ )
		{
			delete m_Shortcuts[ i ];
		}

		delete [] m_Shortcuts;
	}

	if ( m_Envs )
	{
		for ( int i = 0; i < m_NumEnvs; i++ )
		{
			delete m_Envs[ i ];
		}

		delete [] m_Envs;
	}

	if ( m_Main )
	{
		delete m_Main;
	}
}

///////////////////////////////////////////////////////////////////////////

bool RacePaths::Exist( void )
{
	if ( m_Main )
	{
		return true;
	}
	else
	{
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////

SplinePath* RacePaths::Main( void )
{
	// Sanity check.
	ASSERT( m_Main );

	return m_Main;
}

///////////////////////////////////////////////////////////////////////////

SplinePath* RacePaths::Named( char* name, int flags )
{
	if ( flags & SearchMain )
	{
		if ( StringCompare( name, m_Main->m_Name ) )
		{
			return m_Main;
		}
	}

	if ( flags & SearchShortcuts )
	{
		for ( int i = 0; i < m_NumShortcuts; i++ )
		{
			if ( StringCompare( name, m_Shortcuts[ i ]->m_Name ) )
			{
				return m_Shortcuts[ i ];
			}
		}
	}

	if ( flags & SearchEnvs )
	{
		for ( int i = 0; i < m_NumEnvs; i++ )
		{
			if ( StringCompare( name, m_Envs[ i ]->m_Name ) )
			{
				return m_Envs[ i ];
			}
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////

SegmentPath* RacePaths::NearestSegment( Vector3 point, int flags )
{
	SplinePath* nearest_path = m_Main;
	float min_distance_to_path = 1e38f;
	float distance_to_path;

	// Sanity check.
	ASSERT( point.X < 1e5f );
	ASSERT( point.Y < 1e5f );
	ASSERT( point.Z < 1e5f );

	if ( flags & SearchMain )
	{
		distance_to_path = m_Main->NearestApproach( point );

		if ( distance_to_path < min_distance_to_path )
		{
			nearest_path = m_Main;
			min_distance_to_path = distance_to_path;
		}
	}

	if ( flags & SearchShortcuts )
	{
		for ( int i = 0; i < m_NumShortcuts; i++ )
		{
			distance_to_path = m_Shortcuts[ i ]->NearestApproach( point );

			if ( distance_to_path < min_distance_to_path )
			{
				nearest_path = m_Shortcuts[ i ];
				min_distance_to_path = distance_to_path;
			}
		}
	}

	if ( flags & SearchEnvs )
	{
		for ( int i = 0; i < m_NumEnvs; i++ )
		{
			distance_to_path = m_Envs[ i ]->NearestApproach( point );

			if ( distance_to_path < min_distance_to_path )
			{
				nearest_path = m_Envs[ i ];
				min_distance_to_path = distance_to_path;
			}
		}
	}

	return nearest_path;
}

///////////////////////////////////////////////////////////////////////////

SplinePath* RacePaths::NearestSpline( Vector3 point, int flags )
{
	return (SplinePath*)NearestSegment( point, flags );
}

///////////////////////////////////////////////////////////////////////////

ShortcutPath* RacePaths::ShortcutEntrance( float distance )
{
	for ( int i = 0; i < m_NumShortcuts; i++ )
	{
		ShortcutPath* p = m_Shortcuts[ i ];

		if ( p->Entrance( distance ) )
		{
			return p;
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////

void RacePaths::Draw( void )
{
	int i;

	if ( dbg_Draw3D )
	{
		if ( dbg_DrawMain )
		{
			if ( dbg_DrawArcLength )
			{
				m_Main->SplinePath::Draw3D( 0.0f, 0.0f, 0.0f );
			}
			else
			{
				m_Main->SegmentPath::Draw3D( 0.0f, 0.0f, 0.0f );
			}
		}

		if ( dbg_DrawShortcuts )
		{
			for ( i = 0; i < m_NumShortcuts; i++ )
			{
				if ( dbg_DrawArcLength )
				{
					m_Shortcuts[ i ]->SplinePath::Draw3D( 0.0f, 0.0f, 1.0f );
				}
				else
				{
					m_Shortcuts[ i ]->SegmentPath::Draw3D( 0.0f, 0.0f, 1.0f );
				}
			}
		}

		if ( dbg_DrawEnvs )
		{
			for ( i = 0; i < m_NumEnvs; i++ )
			{
				if ( dbg_DrawArcLength )
				{
					m_Envs[ i ]->SplinePath::Draw3D( 0.0f, 1.0f, 0.0f );
				}
				else
				{
					m_Envs[ i ]->SegmentPath::Draw3D( 0.0f, 1.0f, 0.0f );
				}
			}
		}
	}

	if ( dbg_Draw2D )
	{
		if ( dbg_DrawMain )
		{
			if ( dbg_DrawArcLength )
			{
				m_Main->SplinePath::Draw2D( 0.0f, 0.0f, 0.0f );
			}
			else
			{
				m_Main->SegmentPath::Draw2D( 0.0f, 0.0f, 0.0f );
			}
		}

		if ( dbg_DrawShortcuts )
		{
			for ( i = 0; i < m_NumShortcuts; i++ )
			{
				if ( dbg_DrawArcLength )
				{
					m_Shortcuts[ i ]->Draw2D( 0.0f, 0.0f, 1.0f, 1.0f, true );
				}
				else
				{
					m_Shortcuts[ i ]->Draw2D( 0.0f, 0.0f, 1.0f, 1.0f, false );
				}
			}
		}

		if ( dbg_DrawEnvs )
		{
			for ( i = 0; i < m_NumEnvs; i++ )
			{
				if ( dbg_DrawArcLength )
				{
					m_Envs[ i ]->SplinePath::Draw2D( 0.0f, 1.0f, 0.0f );
				}
				else
				{
					m_Envs[ i ]->SegmentPath::Draw2D( 0.0f, 1.0f, 0.0f );
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////