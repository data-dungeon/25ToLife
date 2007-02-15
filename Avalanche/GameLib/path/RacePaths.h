///////////////////////////////////////////////////////////////////////////
//
//  RacePaths.h
//
//  A container for RoadRash paths.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////

#ifndef RACEPATHS_H
#define RACEPATHS_H


#YOU SHOULDN'T BE INCLUDING THIS FILE ANYMORE.  PLEASE SWITCH TO PATHCOLLECTION

///////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////

#include "path/Path.h"
#include "path/SegmentPath.h"
#include "path/SplinePath.h"
#include "path/ShortcutPath.h"


////////////////////////////////////////////////////////////////////////////
// Externs
////////////////////////////////////////////////////////////////////////////

class RacePaths;
extern RacePaths* g_Paths;


///////////////////////////////////////////////////////////////////////////
//  Classes
///////////////////////////////////////////////////////////////////////////

class RacePaths
{

public:

	enum
	{
		SearchMain		 = ( 1 << 0 ),
		SearchShortcuts = ( 1 << 1 ),
		SearchEnvs      = ( 1 << 2 ),
	};

public:

	RacePaths( ts_SplinePathTable* paths );
	virtual ~RacePaths( void );

	// Returns true if there is at least a main circuit path.
	bool Exist( void );

	// Returns the main circuit path.
	SplinePath* Main( void );

	// Returns the segment path nearest the point.
	SegmentPath* NearestSegment( Vector3 point, int flags = SearchMain|SearchShortcuts);

	// Returns the spline path nearest the point.
	SplinePath* NearestSpline( Vector3 point, int flags = SearchMain|SearchShortcuts);

   // Returns a shortcut path if its entrance is available at distance,
	// NULL otherwise.
	ShortcutPath* ShortcutEntrance( float distance );

	// Returns a spline path with the name specified, or null if one with that name doesn't exist.
	SplinePath* Named( char* name, int flags = SearchEnvs );

public:	// debug

	void Draw( void );

private:

	ShortcutPath** m_Shortcuts;		// Array of pointers to shortcut paths.
	int				m_NumShortcuts;	// Number of shortcuts.

	SplinePath**	m_Envs;				// Array of pointers to environment paths.
	int				m_NumEnvs;			// Number of envs.

	SplinePath*		m_Main;				// Pointer to the main path.
};

///////////////////////////////////////////////////////////////////////////

#endif	// RACEPATHS_H