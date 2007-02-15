///////////////////////////////////////////////////////////////////////////
//
//  PathCollection.cpp
//
//  A container for SplinePaths.
//
//  Travis Nixon, July 2001.
//
//  Copyright (c) 2001 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////


/******************* includes ****************************/

#include "path/PathPCH.h"
#include "cmdcon/cmdcon.h"





///////////////////////////////////////////////////////////////////////////
//  Globals
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//  Statics
///////////////////////////////////////////////////////////////////////////

static bool dbg_DrawSegments = false;
static bool dbg_DrawSplines = false;
static float dbg_ArcLength = 0.25f;
static bool dbg_DrawGroup[32] =
{
	true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,
	true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true
};

///////////////////////////////////////////////////////////////////////////
//  Private Functions
///////////////////////////////////////////////////////////////////////////

// Case insensitive substring compare (where substring means up to the
// shorter of the two strings). Return the same results as every other
// string compare in the world.  0 means they match, while negative or positive
// indicates a (case insensitive) sort order
static int StringCompare( const char* s0, const char* s1 )
{
	ASSERT( s0 );
	ASSERT( s1 );

	while( *s0 && *s1 )
	{
		if ( tolower( *s0 ) != tolower( *s1 ) )
			return *s1 - *s0;
		s0++;
		s1++;
	}

	return 0;
}

static int BitToIndex( unsigned int bit )
{
	int shift = 0;

	while( !(bit & 1) )
	{
		shift++;
		bit >>= 1;
	}
	return shift;
}

////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//  Implementation
///////////////////////////////////////////////////////////////////////////

PathCollection *ThePaths;

///////////////////////////////////////////////////////////////////////////

PathCollection::PathCollection()
{
	int i;

	ngroups = 0;
	for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
		groups[i] = NULL;

	m_Main = NULL;
	// reserve a group for main
	RegisterGroup( "CIRCUIT", (1<<31), Spline );

	enum_group = -1;
	enum_flags = 0;
	enum_list = NULL;
	enum_lastpath = NULL;
}

///////////////////////////////////////////////////////////////////////////

PathCollection::~PathCollection()
{
	// er, this is a singleton class that statically constructs itself, so this
	// better only happen at exit, or we're doing something wrong

	// clean up groups anyway, just for fun and safety in case somebody changes
	// this so that it can be constructed dynamically
	int i;

	for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
		delete groups[i];
	
	delete m_Main;
}

///////////////////////////////////////////////////////////////////////////

void PathCollection::Init( void )
{
	ASSERT( ThePaths == NULL && "Only one PathCollection is allowed" );
	ThePaths = new PathCollection;
}

PathCollection *PathCollection::Collection( void )
{
	return ThePaths;
}

void PathCollection::Shutdown( void )
{
	ASSERT( ThePaths && "Init() must be called before Shutdown()" );
	delete ThePaths;
	ThePaths = NULL;
}

bool PathCollection::RegisterGroup( char *name, unsigned int flag, PathGroupType type )
{
	int i, groupslot = -1;

	// is this flag already used?
	for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
	{
		if( groups[i] && (groups[i]->groupflag & flag) )
		{
			// you probably forgot that the flag is a bit, not an index
			ASSERT( 0 && "Path group registered with a flag that conflicts with other flags." );
			return false;
		}
	}
	// valid type?
	if( type == Invalid )
	{
		ASSERT( 0 && "Invalid path group type." );
		return false;
	}
	// do we have a spot for a new group?
	for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
	{
		if( groups[i] == NULL )
		{
			groupslot = i;
			break;
		}
	}
	if( groupslot == -1 )
	{
		ASSERT( 0 && "Too many Path Groups. Unable to register a new one" );
		return false;
	}

	// go ahead and create the group
	groups[groupslot] = new PathGroup( name, flag, type );
	return true;
}

PathGroup *PathCollection::FindGroup( char *name )
{
	int i;

	for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
	{
		if( groups[i] && (StringCompare( name, groups[i]->groupname ) == 0) )
			return groups[i];
	}
	return NULL;
}

int PathCollection::GetGroupFlag( char *name )
{
	PathGroup *group = FindGroup( name );
	if( group )
		return group->groupflag;

	return 0;
}

SplinePath *PathCollection::EnumPaths( SplinePath *last, int whichgroups )
{
	int i;

	// EnumPaths keeps track of the search state so we don't have to go through every
	// group's list to figure out where we were.  All the state information is basically
	// so we know when we need to start a new search rather than continue the last one

	if( last == NULL || last != enum_lastpath || whichgroups != enum_flags )
	{
		enum_group = 0;
		enum_flags = whichgroups;

		// find the first group matching this flag that has a path
		enum_list = NULL;
		for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
		{
			if( groups[i] && (groups[i]->groupflag & whichgroups) && groups[i]->paths )
			{
				enum_list = groups[i]->paths;
				break;
			}
		}
	}

	// enum_list is pointing at the path we should return now, but we need to figure
	// out which one will be next
	// if enum_list is null, that means we've completed our search.
	enum_lastpath = NULL;
	if( enum_list )
	{
		enum_lastpath = enum_list->path;
		if( enum_list->next )
		{
			enum_list = enum_list->next;
		}
		else
		{
			// move on to the next group and find the next item that will be returned
			// in the enumeration
			enum_list = NULL;
			enum_group++;
			for( i = enum_group; i < PATH_COLLECTION_MAXGROUPS; i++ )
			{
				if( groups[i] && (groups[i]->groupflag & whichgroups) && groups[i]->paths )
				{
					enum_list = groups[i]->paths;
					break;
				}
			}
		}
	}

	return enum_lastpath;
}

///////////////////////////////////////////////////////////////////////////

bool PathCollection::AddPaths( ts_SplinePathTable *paths )
{
	int i;
	PathGroup *pGroup;
	bool success = true;

	if ( paths == NULL )
	{
		conPuts( "No paths found!\n" );
		return false;
	}

	for( i = 0; i < paths->pathCount; i++ )
	{
		ts_SplinePath* p = paths->pathList[i];

		pGroup = FindGroup( p->name );
		if( pGroup )
		{
			pGroup->AddPath( p, m_Main );
			// was this the main path?
			if( pGroup->groupflag == SearchMain )
			{
				// Make sure that we create only one main circuit!
				ASSERT( m_Main == NULL && "Only one Circuit path is allowed" );
				m_Main = new SplinePath( p->name, &p->pointList[0], p->pointCount );
			}
		}
		else
		{
#if 0
			ASSERT( 0 && "Path trying to use an unregistered group name" );
#else
			char msgStr[100];
			sprintf(msgStr, "No path group registered for %s\n", p->name);
			conPuts(msgStr);
#endif
			success = false;
		}
	}

	char dbgstr[32];
	for( i = 0; i < 32; i++ )
	{
		sprintf( dbgstr, "Path%d", i );
		g_console.CreateVar( "paths", dbgstr, dbg_DrawGroup[i] );
	}
	g_console.CreateVar( "paths", "arcLength", dbg_ArcLength );
	g_console.CreateVar( "paths", "drawSplines", dbg_DrawSplines);
	g_console.CreateVar( "paths", "drawSegments", dbg_DrawSegments);
	
	return success;
}

///////////////////////////////////////////////////////////////////////////

bool PathCollection::AddAutoGroupedPaths( ts_SplinePathTable *paths, PathGroupType type )
{
	int i;
	PathGroup *pGroup;
	bool success = true;
	unsigned int flag = 0x1;

	if ( paths == NULL )
	{
		conPuts( "No paths found!\n" );
		return false;
	}

	for( int c = 0; c < paths->pathCount; c++ )
	{
		ts_SplinePath* p = paths->pathList[c];

		pGroup = FindGroup( p->name );
		if( pGroup == NULL )
		{
			// Find an unused group
			int groupslot;
			for( groupslot = 0; groupslot < PATH_COLLECTION_MAXGROUPS; groupslot++ )
			{
				if( groups[groupslot] == NULL )
					break;
			}

			// Find an unused flag
			if( groupslot < PATH_COLLECTION_MAXGROUPS )
			{
				while( flag != 0 )
				{
					for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
					{
						if( groups[i] && (groups[i]->groupflag & flag) )
							break;
					}
					if( i >= PATH_COLLECTION_MAXGROUPS )
						break;
					flag <<= 1;
				}
			}

			// An unused flag and group was found
			if( ( flag != 0 ) && ( groupslot < PATH_COLLECTION_MAXGROUPS ) )
			{
				// Extract a group name
				char* groupEnd = p->name;
				while( ( *groupEnd != '\0' ) && ( *groupEnd != '_' ) )
					groupEnd++;
				char groupChar = *groupEnd;
				*groupEnd = '\0';

				// go ahead and create the group
				pGroup = new PathGroup( p->name, flag, type );
				groups[groupslot] = pGroup;

				// Restore the path name
				*groupEnd = groupChar;

				// Move to the next flag
				flag <<= 1;
			}
		}

		if( pGroup )
		{
			pGroup->AddPath( p, m_Main );
			// was this the main path?
			if( pGroup->groupflag == SearchMain )
			{
				// Make sure that we create only one main circuit!
				ASSERT( m_Main == NULL && "Only one Circuit path is allowed" );
				m_Main = new SplinePath( p->name, &p->pointList[0], p->pointCount );
			}
		}
		else
		{
			char msgStr[100];
			sprintf(msgStr, "Cannot find or create path group for %s\n", p->name);
			conPuts(msgStr);
		}
	}

	char dbgstr[32];
	for( i = 0; i < 32; i++ )
	{
		sprintf( dbgstr, "Path%d", i );
		g_console.CreateVar( "paths", dbgstr, dbg_DrawGroup[i] );
	}
	g_console.CreateVar( "paths", "arcLength", dbg_ArcLength );
	g_console.CreateVar( "paths", "drawSplines", dbg_DrawSplines);
	g_console.CreateVar( "paths", "drawSegments", dbg_DrawSegments);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////

SplinePath* PathCollection::Named( const char* name, int whichgroups )
{
	int i;

	for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
	{
		if( groups[i] && (whichgroups & groups[i]->groupflag) )
		{
			SplinePathList *list = groups[i]->paths;
			while( list )
			{
				// NOTE: Stringcompare is actually only a substring match,
				// so RAIL_Straight will find the first of RAIL_StraightLeft or
				// RAIL_StraightRight that it finds.

				// SCL - this call needs to do an EXACT match of the path name
				// otherwise, env_path_nameA matches env_path_nameAA,
				// sometimes . . .
				if( stricmp( name, list->path->Name() ) == 0 )
					return list->path;
				list = list->next;
			}
		}
	}

	return NULL;
}

int PathCollection::GetPathFlag( SplinePath *pPath )
{
	int i;

	if(pPath)
	{
		for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
		{
			if(groups[i])
			{
				SplinePathList *list = groups[i]->paths;
				while (list)
				{
					if( pPath ==  list->path )
						return groups[i]->groupflag;

					list = list->next;
				}
			}
		}
	}

	return 0;
}

SplinePathList* PathCollection::GetPathListbyGroup(char* name)
{
	int i;

	for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
	{
		if(groups[i])
		{
			SplinePathList *list = groups[i]->paths;
			if (list)
			{
			if( StringCompare( name, list->path->Name() ) == 0 )
				return list;
			}
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////

// I removed NearestSegment because it didn't make sense in the context
// of how everything was stored.  None of the splines are stored as
// SegmentPaths, they are stored as either SplinePaths or ShortcutPaths, which
// are derived from segmentpath.  If this collection is ever changed so that
// it can hold SegmentPaths, the group list type will need to be changed
// to segmentpath as well.
SplinePath* PathCollection::NearestSpline( Vector3 point, int whichgroups )
{
	// NOTE: RacePaths started with m_Main as the nearest path, but since
	// you can specify to not search main, I thought that was probably a bug,
	// so I removed it.
	SplinePath* nearest_path = NULL;
	float min_distance_to_path = 1e38f;
	float distance_to_path;
	int i;

	// Sanity check.
	// TN - um, is this really safe considering that point is a float vector?
	// leaving it for now
	ASSERT( point.x() < 1e5f );
	ASSERT( point.y() < 1e5f );
	ASSERT( point.z() < 1e5f );

	for( i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
	{
		if( groups[i] && ( whichgroups & groups[i]->groupflag ) )
		{
			SplinePathList *list = groups[i]->paths;
			while( list )
			{
				if(list->path->bIsDisabled == FALSE)
				{
					distance_to_path = list->path->NearestApproach( point );
					if( distance_to_path < min_distance_to_path )
					{
						nearest_path = list->path;
						min_distance_to_path = distance_to_path;
					}
				}
				list = list->next;
			}
		}
	}
	return nearest_path;
}

///////////////////////////////////////////////////////////////////////////

ShortcutPath* PathCollection::ShortcutEntrance( float distance )
{
	// search all groups that have type Shortcut
	for( int i = 0; i < PATH_COLLECTION_MAXGROUPS; i++ )
	{
		if( groups[i] && groups[i]->grouptype == Shortcut )
		{
			SplinePathList *list = groups[i]->paths;
			while( list )
			{
				ShortcutPath *p = (ShortcutPath *)list->path;
				if( p->Entrance( distance ) )
					return p;
				list = list->next;
			}
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////

PathGroup::PathGroup( char *name, unsigned int flag, unsigned int type )
{
	groupname = new char[strlen(name) + 1];
	strcpy( groupname, name );
	groupflag = flag;
	grouptype = type;

	paths = NULL;
}

///////////////////////////////////////////////////////////////////////////

PathGroup::~PathGroup()
{
	delete[] groupname;

	SplinePathList *list, *next;

	// clean up path list
	list = paths;
	while( list )
	{
		next = list->next;
		delete list->path;
		delete list;
		list = next;
	}
}

///////////////////////////////////////////////////////////////////////////

void PathGroup::AddPath( ts_SplinePath *spline, SplinePath *pMain )
{
	SplinePath *pSP = NULL;
	SplinePathList *list;

	switch( grouptype )
	{
		case PathCollection::Shortcut:
			ASSERT( pMain && "main circuit must appear before any shortcuts" );
			pSP = new ShortcutPath( spline->name, &spline->pointList[0], spline->pointCount, pMain );
			break;
		case PathCollection::Spline:
			pSP = new SplinePath( spline->name, &spline->pointList[0], spline->pointCount );
			break;
		default:
			ASSERT( 0 && "Invalid Group type" );
			break;
	}

	ASSERT( pSP && "Failed to create path" );
	list = new SplinePathList;
	list->path = pSP;
	list->next = paths;
	paths = list;
}

///////////////////////////////////////////////////////////////////////////

// if you only want to draw paths with specific names, add the names to this array BEFORE the NULL
char *PathsToDraw[] = 
{
	NULL
};

void PathCollection::Draw( void )
{
	if (!dbg_DrawSegments && !dbg_DrawSplines)
	{
		return;
	}

	for(int i = 0; i < PATH_COLLECTION_MAXGROUPS; i++)
	{
		if(groups[i] && dbg_DrawGroup[i])
		{
			SplinePathList *list = groups[i]->paths;
			while (list)
			{
				ShortcutPath *scut = NULL;
				bool draw = true;

				if(groups[i]->grouptype == Shortcut)
				{
					scut = (ShortcutPath *)list->path;
				}

				if( PathsToDraw[0] )
				{
					draw = false;
					for( int j = 0; j < (sizeof( PathsToDraw ) / sizeof( PathsToDraw[0] )); j++ )
					{
						if( PathsToDraw[j] && strcmpi( PathsToDraw[j], list->path->Name() ) == 0 )
						{
							draw = true;
							break;
						}
					}
				}
				
				if( draw )
				{
					if(dbg_DrawSegments)
					{
						list->path->SegmentPath::Draw();
					}
					if(dbg_DrawSplines)
					{
						list->path->SplinePath::Draw(dbg_ArcLength);
					}
				}

				list = list->next;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////
