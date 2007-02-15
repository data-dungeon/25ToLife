//***************************************************************************
// Covers a rectangular XZ region with arbitrarily shaped "zones".  You can
// then query a point in XZ region and see what zones touch the point.
//
// Of course, the implementation doesn't really matter...but this is using
// either a quad-tree or MRM to make sure your queries are very fast.
//***************************************************************************
#ifndef ZONEMAP
#define ZONEMAP

#include "container/dynlist.h"
#include "CollideHelper/zone.h"

// Forwards
class ZoneGrid;

class ZoneMap
{
public:
	ZoneMap(const XZRect &mapSize, float gridSize = 2.0f, int maxRowsColumns = 64);
	~ZoneMap();

	// Add a zone (ZoneMap will delete it when dead)
	bool Add(Zone *newZone);

	// Bake all the added zones, once baked you can no longer add
	bool Bake();

	// What are the total zones in this map (call after bake)
	int TotalZones();

	// Just access an indexed zoned
	Zone *ZoneMap::GetZone(int index);

	// What is the largest number of zones that overlap (call after bake)
	int MaxOverlappingZones();

	// What zones touch this point
	// You pass in the array and max count in zones
	// it returns the number of entries it filled in in zones
	// We don't want per-frame mem allocations going on
	int Touches(Vector3CRef p, Zone **touchList, int maxZones, bool inOrder = false);

	// Debugging draw call..draws all touched zones
	// Active/inactive in different colors
	void Draw(const Vector3 *p);

private:
	// What size are we?
	XZRect									size;
	float										grid, invGrid;
	int										columns, rows;

	// The array of zones
	DynList<Zone>							zone;
	int										maxOverlappingZones;

	// The grid of information
	bool										baked;
	Zone										**zonePool;
	ZoneGrid									*zoneGridPool;

	ZoneGrid									**zoneGrid;
	ZoneGrid									*globalGrid;

	inline int Index(int x, int z);
	bool Gridize(Vector3CRef p, int &gridX, int &gridZ);
	void GridizeClipped(float x, float z, int &gridX, int &gridZ);
	XZRect GridRect(int x, int z);
	void RectToGrid(const XZRect &rect, int &minx, int &minz, int &maxx, int &maxz);
	bool GlobalZone(const XZRect &rect);

	// Compare two zone ID's for qsort
	static int CDECL ZoneIDCompare(const void *elem1, const void *elem2);
};

class ZoneGrid
{
public:
	ZoneGrid() {}
	~ZoneGrid() {}

	// Setup the memory
	void Initialize(Zone **zoneMemory, int maxZones);

	// How many zones are there?
	inline int Zones()
		{ return zones; }

	// Get at the list of zone's in this grid
	inline Zone *GetZone(int index)
		{ return zone[index]; }

	// Add a zone
	void Add(Zone *newZone);

private:
	Zone				**zone;
	int				zones;
};

#endif
