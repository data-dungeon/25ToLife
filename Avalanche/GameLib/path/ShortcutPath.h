///////////////////////////////////////////////////////////////////////////
//
//  ShortcutPath.h
//
//  A shortcut path.
//
//  Nate Robins, January 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////

#ifndef SHORTCUTPATH_H
#define SHORTCUTPATH_H


///////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////

#include "path/SplinePath.h"


///////////////////////////////////////////////////////////////////////////
//  Classes
///////////////////////////////////////////////////////////////////////////

class ShortcutPath : public SplinePath
{
public:

   // Create a new shortcut path.  The points array is an ordered list of
	// ( start point, control point 1, control point 2, end point ) quads.
	ShortcutPath( char* name, Vector3Packed* points, int num_points, SplinePath* main_path );
	virtual ~ShortcutPath( void );

	// Returns true if this shortcuts entrance is at the distance, false otherwise.
	bool Entrance( float distance );

	// Returns true if this shortcuts exit is at the distance, false otherwise.
	bool Exit( float distance );

	// Returns true if this distance is beyond the end points of the shortcut.
	bool BeyondEnds( float distance );

private:

	SplinePath* m_Main;			// Main path.

	int m_I1;						// Index to start point on the main path (m_MainPath->m_Points[]).
	int m_I2;						// Index to end point on the main path (m_MainPath->m_Points[]).

};

///////////////////////////////////////////////////////////////////////////

#endif	// SHORTCUTPATH_H
