//***************************************************************************
// Covers a rectangular XZ region with arbitrarily shaped "zones".  You can
// then query a point in XZ region and see what zones touch the point.
//***************************************************************************
#include "CollideHelper/CollideHelperPCH.h"

//***************************************************************************
//***************************************************************************
ZoneMap::ZoneMap(
const XZRect &mapSize,
float gridSize,
int maxRowsColumns) :
zone(8, 8, true)
{
	// save off the world-size and grid
	size = mapSize;
	size.Normalize();

	grid = gridSize;
	ASSERT(grid > 0.0f);
	invGrid = 1.0f / grid;

	columns = (int)(size.Width() * invGrid) + 1;
	rows = (int)(size.Height() * invGrid) + 1;

	// Clamp these
	if (columns > maxRowsColumns || rows > maxRowsColumns)
	{
		const float invMax = 1.0f / (float)maxRowsColumns;
		grid = Math::Max(size.Width() * invMax, size.Height() * invMax);
		invGrid = 1.0f / grid;

		columns = (int)(size.Width() * invGrid) + 1;
		rows = (int)(size.Height() * invGrid) + 1;
	}

	// Not baked yet
	baked = false;

	// Make the grid
	zonePool = NULL;
	zoneGridPool = NULL;
	zoneGrid = NULL;
	globalGrid = NULL;

	// Dunno this yet
	maxOverlappingZones = 0;
}

//***************************************************************************
//***************************************************************************
ZoneMap::~ZoneMap()
{
	// Kill the master array
	delete [] zoneGrid;

	// Delete the pools
	delete [] zonePool;
	delete [] zoneGridPool;
}

//***************************************************************************
// Add a zone (ZoneMap will delete it when dead)
//***************************************************************************
bool ZoneMap::Add(
Zone *newZone)
{
	// Can't add if we are baked
	ASSERT(!baked);
	if (baked)
		return false;

	// Make sure we have data
	if (!newZone)
		return false;

	// stick it in the master list
	zone.Add(newZone);
	return true;
}

//***************************************************************************
// Bake all the added zones, once baked you can no longer add
//***************************************************************************
bool ZoneMap::Bake()
{
	// First we want to count the total number of Zone*'s needed for the
	int globalZonesNeeded = 0;
	int *zonesNeeded = new int [rows * columns];
	ASSERT(zonesNeeded);
	if (!zonesNeeded)
		return false;
	int i;
	for (i = 0; i < rows * columns; i++)
		zonesNeeded[i] = 0;
	int totalZonesNeeded = 0;
	int totalZoneGridsNeeded = 0;

	// Create the array of counts
	for (i = 0; i < zone.Count(); i++)
	{
		// Get it's extents
		XZRect zoneRect = zone[i]->Extents();

		// If it's huge, just add it to the global grid
		if (GlobalZone(zoneRect))
		{
			++globalZonesNeeded;
			++totalZonesNeeded;
		}
		else
		{
			// Map into the grid
			int minx, minz, maxx, maxz;
			RectToGrid(zoneRect, minx, minz, maxx, maxz);

			// Walk each zonegrid it touches
			for (int z = minz; z <= maxz; z++)
			{
				for (int x = minx; x <= maxx; x++)
				{
					// Build the rect for this grid
					XZRect gridRect = GridRect(x, z);

					// Add it to the grid
					if (zone[i]->Intersects(gridRect))
					{
						// What is the index of this grid
						int index = Index(x, z);

						// New grid?
						if (zonesNeeded[index] == 0)
							++totalZoneGridsNeeded;

						// Increment the counts
						++zonesNeeded[index];
						++totalZonesNeeded;
					}
				}
			}
		}
	}

	if (totalZonesNeeded == 0)
		return true;

	// Keep track of memory useage
	int memoryUsed = 0;

	// Create the array of zone *'s
	zonePool = new Zone *[totalZonesNeeded];
	memoryUsed += sizeof(Zone *) * totalZonesNeeded;
	ASSERT(zonePool);
	if (!zonePool)
	{
		delete [] zonesNeeded;
		return false;
	}
	int usedZonePool = 0;

	// Now create the zone grids
	zoneGridPool = new ZoneGrid[totalZoneGridsNeeded + 1];
	memoryUsed += sizeof(ZoneGrid) * (totalZoneGridsNeeded + 1);
	ASSERT(zoneGridPool);
	if (!zoneGridPool)
	{
		delete [] zonesNeeded;
		return false;
	}
	int usedZoneGridPool = 0;

	// Make the grid
	zoneGrid = new ZoneGrid *[rows * columns];
	memoryUsed += sizeof(ZoneGrid *) * (rows * columns);
	ASSERT(zoneGrid);
	if (!zoneGrid)
	{
		delete [] zonesNeeded;
		return false;
	}

	// Set them all to NULL
	int z, index = 0;
	for (z = 0; z < rows; z++)
		for (int x = 0; x < columns; x++, index++)
			zoneGrid[index] = NULL;

	// Create the global grid
	globalGrid = &zoneGridPool[usedZoneGridPool++];
	globalGrid->Initialize(&zonePool[usedZonePool], globalZonesNeeded);
	usedZonePool += globalZonesNeeded;

	// Now add everyone
	for (i = 0; i < zone.Count(); i++)
	{
		// Get it's extents
		XZRect zoneRect = zone[i]->Extents();

		// If it's huge, just add it to the global grid
		if (GlobalZone(zoneRect))
		{
			// Just put it in the global space
			globalGrid->Add(zone[i]);
			++maxOverlappingZones;
		}
		else
		{
			// Map into the grid
			int minx, minz, maxx, maxz;
			RectToGrid(zoneRect, minx, minz, maxx, maxz);

			// Walk each zonegrid it touches
			for (int z = minz; z <= maxz; z++)
			{
				for (int x = minx; x <= maxx; x++)
				{
					// Build the rect for this grid
					XZRect gridRect = GridRect(x, z);

					// Add it to the grid
					if (zone[i]->Intersects(gridRect))
					{
						// What is the index of this grid
						int index = Index(x, z);

						// Is it there?
						if (!zoneGrid[index])
						{
							zoneGrid[index] = &zoneGridPool[usedZoneGridPool++];
							zoneGrid[index]->Initialize(&zonePool[usedZonePool], zonesNeeded[index]);
							usedZonePool += zonesNeeded[index];
						}

						// Tell the grid about it
						zoneGrid[index]->Add(zone[i]);

						// Update the max overlapping zones?
						int totalZones = zoneGrid[index]->Zones() + globalGrid->Zones();
						if (totalZones > maxOverlappingZones)
							maxOverlappingZones = totalZones;
					}
				}
			}
		}
	}

	// Done with the counts
	delete [] zonesNeeded;

	TRACE("ZoneGrid %dx%d used %d bytes.\n", rows, columns, memoryUsed);

#ifdef _DEBUG
	// Verify order
	for (z = 0; z < rows; z++)
	{
		for (int x = 0; x < columns; x++)
		{
			// What is the index of this grid
			int index = Index(x, z);

			// Is it there?
			if (zoneGrid[index])
			{
				for (int test = 1; test < zoneGrid[index]->Zones(); test++)
				{
					ASSERT(zoneGrid[index]->GetZone(test));
					ASSERT(zoneGrid[index]->GetZone(test - 1));
					ASSERT(zoneGrid[index]->GetZone(test - 1)->ID() < zoneGrid[index]->GetZone(test)->ID());

				}
			}
		}
	}

	// Is it there?
	if (globalGrid)
	{
		for (int test = 1; test < globalGrid->Zones(); test++)
		{
			ASSERT(globalGrid->GetZone(test));
			ASSERT(globalGrid->GetZone(test - 1));
			ASSERT(globalGrid->GetZone(test - 1)->ID() < globalGrid->GetZone(test)->ID());
		}
	}
#endif

	// Done!
	baked = true;
	return true;
}

//***************************************************************************
// What are the total zones in this map
//***************************************************************************
int ZoneMap::TotalZones()
{
	return zone.Count();
}

//***************************************************************************
//***************************************************************************
Zone *ZoneMap::GetZone(int index)
{
	if (index < 0 || index >= zone.Count())
		return NULL;

	return zone[index];
}

//***************************************************************************
// What is the largest number of zones that overlap
//***************************************************************************
int ZoneMap::MaxOverlappingZones()
{
	return maxOverlappingZones;
}

//***************************************************************************
// What zones touch this point
// You pass in the array and max count in zones
// it returns the number of entries it filled in in zones
// We don't want per-frame mem allocations going on
//***************************************************************************
int ZoneMap::Touches(
Vector3CRef p,
Zone **touchList,
int maxZones,
bool inOrder)
{
	// Duh
	if (!baked)
		Bake();

	// Do we have to do any work?
	int x, z;
	if (!Gridize(p, x, z))
		return 0;

	// Init the count
	int zones = 0;

	// First copy in the global stuff
	int i;
	for (i = 0; i < globalGrid->Zones(); i++)
	{
		// Are we outta here?
		if (zones == maxZones)
			break;

		// Alias the zone we are working with
		Zone *zone = globalGrid->GetZone(i);

		// Is this zone applicable?
		if (zone->Contains(p))
			touchList[zones++] = zone;
	}

	// Are we in a specific cell?
	ZoneGrid *inGrid = zoneGrid[Index(x, z)];
	if (!inGrid)
		return zones;

	// Copy the list
	for (i = 0; i < inGrid->Zones(); i++)
	{
		// Are we outta here?
		if (zones == maxZones)
			break;

		// Alias the zone we are working with
		Zone *zone = inGrid->GetZone(i);

		// Is this zone applicable?
		if (zone->Contains(p))
			touchList[zones++] = zone;
	}

	// Gotta sort because we merged two lists
	if (inOrder)
		qsort(touchList, zones, sizeof(Zone *), ZoneIDCompare);

	return zones;
}

//***************************************************************************
//***************************************************************************
int CDECL ZoneMap::ZoneIDCompare(
const void *elem1,
const void *elem2)
{
	Zone *zone1 = *((Zone **)elem1);
	Zone *zone2 = *((Zone **)elem2);
	ASSERT(zone1 && zone2);

	return (zone1->ID() - zone2->ID());
}

//***************************************************************************
// Debugging draw call..draws all touched zones
// Active/inactive in different colors
//***************************************************************************
void ZoneMap::Draw(
const Vector3 *p)
{
	if (!p)
	{
		// Draw all
		for (int z = 0; z < zone.Count(); z++)
			zone[z]->Draw();
	}
	else
	{
	  // Convert into row and column
		int x, z;
		if	(!Gridize(*p, x, z))
			return;

		// Get the ZoneGrid
		ZoneGrid *inGrid = zoneGrid[Index(x, z)];
		if (inGrid)
		{
			// Draw everything
			for (int i = 0; i < inGrid->Zones(); i++)
				inGrid->GetZone(i)->Draw();
		}
	}
}

//***************************************************************************
//***************************************************************************
inline int ZoneMap::Index(
int x,
int z)
{
	ASSERT(z >= 0 && z < rows);
	ASSERT(x >= 0 && x < columns);

	return z * columns + x;
}

//***************************************************************************
//***************************************************************************
bool ZoneMap::Gridize(
Vector3CRef p,
int &x,
int &z)
{
	// Convert into grid space
	float gx = p.x() - size.MinX();
	float gz = p.z() - size.MinZ();

	// figure out the column
	x = (int)(gx * invGrid);
	if (x < 0 || x >= columns)
		return false;

	// Figure out the row
	z = (int)(gz * invGrid);
	if (z < 0 || z >= rows)
		return false;

	// on the map
	return true;
}

//***************************************************************************
//***************************************************************************
void ZoneMap::GridizeClipped(
float x,
float z,
int &gridx,
int &gridz)
{
	// Convert into grid space
	float gx = x - size.MinX();
	float gz = z - size.MinZ();

	// figure out the gridx
	gridx = (int)(gx * invGrid);
	if (gridx < 0)
		gridx = 0;
	else if (gridx >= columns)
		gridx = columns - 1;

	// Figure out the row
	gridz = (int)(gz * invGrid);
	if (gridz < 0)
		gridz = 0;
	else if (gridz >= rows)
		gridz = rows - 1;
}

//***************************************************************************
//***************************************************************************
XZRect ZoneMap::GridRect(
int x,
int z)
{
	float minx = size.MinX() + (float)x * grid;
	float minz = size.MinZ() + (float)z * grid;
	float maxx = minx + grid;
	float maxz = minz + grid;

	return XZRect(minx, minz, maxx, maxz);
}

//***************************************************************************
//***************************************************************************
void ZoneMap::RectToGrid(
const XZRect &rect,
int &minx,
int &minz,
int &maxx,
int &maxz)
{
	GridizeClipped(rect.MinX(), rect.MinZ(), minx, minz);
	GridizeClipped(rect.MaxX(), rect.MaxZ(), maxx, maxz);
}

//***************************************************************************
// Is this zone large enough to go into the global grid?
//***************************************************************************
bool ZoneMap::GlobalZone(
const XZRect &rect)
{
	return (rect.Area() >= size.Area() * 0.5f || !rect.Intersects(size));
}

//***************************************************************************
//***************************************************************************
void ZoneGrid::Initialize(
Zone **zoneMemory,
int maxZones)
{
	zone = zoneMemory;
	zones = maxZones;

	for (int i = 0; i < zones; i++)
		zone[i] = NULL;
}

//***************************************************************************
// Add a zone
//***************************************************************************
inline void ZoneGrid::Add(
Zone *newZone)
{
	ASSERT(zone);

	for (int i = 0; i < zones; i++)
	{
		if (!zone[i])
		{
			zone[i] = newZone;
			return;
		}
	}
}

