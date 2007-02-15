///////////////////////////////////////////////////////////////////////////
//
//  PathCollection.h
//
//  A container for spline paths.
//
//  Travis Nixon, July 2001.
//
//  Copyright (c) 2001 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////

#ifndef PATHCOLLECTION_H_INCLUDED
#define PATHCOLLECTION_H_INCLUDED


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

///////////////////////////////////////////////////////////////////////////
//  Classes
///////////////////////////////////////////////////////////////////////////

// TYPICAL INITIALIZATION EXAMPLE:
// PathCollection::Init();
// PathCollection::Collection()->RegisterGroup( "ENV", (1<<0), Spline );
// PathCollection::Collection()->RegisterGroup( "SHORTCUT", (1<<1), Shortcut );
// PathCollection::Collection()->AddPaths( dbl_paths );

//
// This class is meant as a more flexible replacement for RacePaths.
//
// How it works in a nutshell:
//
// First of all, this is a singleton class, because that's how RacePaths was used,
// not because there only can be one pathcollection.  But since you can put as many
// groups in a collection as you want, there's really not any reason to ever have
// more than one.  If there's a demand for it, it wouldn't be that hard to change,
// I'm just going for maximum compatibility with RacePaths.
//
// Being a singleton class, you don't need to construct one.  What you do need to do
// is register groups.  This is done by calling RegisterGroup with the substring,
// a bit to use for search preferences, and a type.  If you specify a search bit
// that's already in use, the group WILL NOT BE REGISTERED.  If you specify a type
// that is incorrect for your group, then really weird shit is going to happen, so
// don't do that.
//
// Please note that because we use a bit indicator for indicating search preferences,
// this means that there are only 32 possible groups.  If you need more than 32 groups,
// well, er, fix this class so that it can have more than 32 groups.
//
// A word here on pathnames: for this collection to work, all pathnames must be in
// the format of GROUPNAME_pathname.  For example, if you have 2 groups, RAIL and
// PIPE, you might have pathnames such as RAIL_This, RAIL_That, RAIL_TheOther,
// PIPE_This, PIPE_That, and PIPE_TheOther.  Any pathnames that don't fall into
// registered groups will not be added to the collection, and will never be found
// by any of the searches.
//
// Also note that the groupname is kept as part of the pathname, so given the above
// list of names, PIPE_That would be found, but TheOther (sans group name) would not.
// Technically I suppose it would be ok to strip off the group name, since you have
// to specify which groups you want to search, but then we run into the possibility
// of naming conflict.  In our example list, which This would be found?  Of course, this
// is all set up around the premise that you'll only search one group at a time anyway,
// because searching for PIPE_This in group RAIL will never succeed, but, hey, I didn't
// write RacePaths, I'm just trying to make it as painless as possible to switch from
// a RacePaths collection to a PathCollection collection. :)

// this can't get any bigger without making some changes to combined searches,
// but if you do manage to change it, here's a good way to find all the other places
// that assume 32 max groups.
#define PATH_COLLECTION_MAXGROUPS 32

struct SplinePathList
{
	SplinePath		*path;
	SplinePathList	*next;
};

// Class PathGroup was only intended to be used in a PathCollection, which is why
// it's defined inside PathCollection.  That means you shouldn't be trying to use it,
// now go away and leave me alone.

// Originally I had PathGroup defined inside the definition of PathCollection, thereby
// enforcing the commented rule above, but for some reason MSVC chokes on it, giving
// and internal compiler error.  It worked just find with Codewarrior, though.
class PathGroup
{
public:
	PathGroup( char *name = NULL, unsigned int flag = 0, unsigned int type = -1 );
	~PathGroup();

	void AddPath( ts_SplinePath *spline, SplinePath *pMain );

	// these are all public because this class can only be created inside
	// a pathcollection.  if for some odd reason you want to use pathgroups
	// independently, it will need some rework
	char *groupname;
	unsigned int groupflag;
	unsigned int grouptype;
	SplinePathList *paths;
};

class PathCollection
{

public:
	typedef enum
	{
		Invalid = -1,
		Shortcut = 0,
		Spline,
	} PathGroupType;
	enum
	{
		// reserve one flag for searching the main path
		SearchMain = (1<<31)
	};

	PathCollection();
	virtual ~PathCollection( void );

	SplinePath *Main( void ) { return m_Main; }

	static void Init( void );
	static PathCollection *Collection( void );
	static void Shutdown( void );

	bool AddPaths( ts_SplinePathTable *paths );
	bool RegisterGroup( char *name, unsigned int flag, PathGroupType type );
	int GetGroupFlag( char *groupname );
	bool AddAutoGroupedPaths( ts_SplinePathTable *paths, PathGroupType type );

	// Returns the flag of the path
	int GetPathFlag( SplinePath *pPath );

	SplinePath *EnumPaths( SplinePath *last = NULL, int whichgroups = 0xffffffff );

	// Returns the segment path nearest the point.
	SegmentPath* NearestSegment( Vector3 point, int whichgroups = 0xffffffff );

	// Returns the spline path nearest the point.
	SplinePath* NearestSpline( Vector3 point, int whichgroups = 0xffffffff );

   // Returns a shortcut path if its entrance is available at distance,
	// NULL otherwise.
	ShortcutPath* ShortcutEntrance( float distance );

	// Returns a spline path with the name specified, or null if one with that name doesn't exist.
	SplinePath* Named( const char* name, int whichgroups = 0xffffffff);

	// Returns the entire path list related to the group (i.e. RAIL)
	SplinePathList* GetPathListbyGroup(char* name);

	void Draw( void );

private:

	SplinePath*				m_Main;				// Pointer to the main path.
	int 						ngroups;
	PathGroup*				groups[PATH_COLLECTION_MAXGROUPS];

	// date to keep track of a current EnumPaths call
	int enum_group;
	int enum_flags;
	SplinePathList *enum_list;
	SplinePath *enum_lastpath;

	// private function because we don't really want outsiders messing with pathgroups
	PathGroup *FindGroup( char *name );

};

///////////////////////////////////////////////////////////////////////////

#endif	// PATHCOLLECTION_H_INCLUDED
