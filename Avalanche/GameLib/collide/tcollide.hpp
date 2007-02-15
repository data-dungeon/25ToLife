/***************************************************************************/
/***************************************************************************/

#include "geomUtil/clipseg.h"

/***************************************************************************/
/***************************************************************************/
inline u32 TerrainCollide::IncrementTest(void)
{
	if (++testCounter == 0)
	{
		cluster->ResetTestID();
		testCounter = 1;
	}

	return(testCounter);
}

/***************************************************************************/
// Convert this x/z coordinate pair into an xsector/zsector
/***************************************************************************/
inline void TerrainCollide::PointToSector(
float x,
float z,
int &sectorx,
int &sectorz)
{
	// Convert into local coord's
	x -= worldExtents.MinX();
	z -= worldExtents.MinZ();

	// Convert
	if (x >= 0.0f)
		sectorx = (int)(x * invSectorSize);
	else
		sectorx = -1 - (int)(-x * invSectorSize);

	if (z >= 0.0f)
		sectorz = (int)(z * invSectorSize);
	else
		sectorz = -1 - (int)(-z * invSectorSize);
}

/***************************************************************************/
// Is this sector off the map?
/***************************************************************************/
inline bool TerrainCollide::SectorClipped(
int sectorx,
int sectorz)
{
	return(sectorx < 0 || sectorx >= (int)sectorsX ||
				sectorz < 0 || sectorz >= (int)sectorsZ);
}

/***************************************************************************/
// Resolve this xsector/zsector to a TerrainSector
/***************************************************************************/
inline TerrainSector *TerrainCollide::ResolveSector(
int xsector,
int zsector)
{
	ASSERT(!SectorClipped(xsector, zsector));
	return(sector->Get(xsector, zsector));
}

/***************************************************************************/
// Return the terrain sector that this point is in....
// Returns NULL if this is off the map.
/***************************************************************************/
inline TerrainSector *TerrainCollide::PointToSector(
float x,
float z)
{
	int xsector, zsector;
	PointToSector(x, z, xsector, zsector);
	if (SectorClipped(xsector, zsector))
		return(NULL);
	else
		return(ResolveSector(xsector, zsector));
}

/***************************************************************************/
// Resolve the sector with clipping
/***************************************************************************/
inline TerrainSector *TerrainCollide::ResolveSectorClipped(
int xsector,
int zsector)
{
	if (SectorClipped(xsector, zsector))
		return(NULL);
	else
		return(sector->Get(xsector, zsector));
}

/////////////////////////// CTerrainSectorIterator //////////////////////////

/***************************************************************************/
/***************************************************************************/
inline void CTerrainSectorIterator::Setup(TriangleCluster* i_cluster,
		uint32 i_testID)
{
	d_cluster = i_cluster;
	d_testID = i_testID;
}

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainSectorIterator::Initialize(TerrainSector &i_sector)
{
	d_sector = &i_sector;
	d_index = -1;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle* CTerrainSectorIterator::GetNextTriangle(void)
{
	for (d_index++; d_index < (int)d_sector->triangles; d_index++)
	{
		TerrainTriangle* triangle =
				&(d_cluster->Triangle(d_sector->triangleIndex[d_index]));
		if (!triangle->MatchTestID(d_testID))
		{
			triangle->SetTestID(d_testID);

			return(triangle);
		}
	}
	return(NULL);
}

//////////////////////// CTerrainCollideRectIterator ////////////////////////

/***************************************************************************/
/***************************************************************************/
inline XZRect &CTerrainCollideRectIterator::Rect(void)
{
	return(d_rect);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3Packed* CTerrainCollideRectIterator::VertexTable(void)
{
	return(d_collide->cluster->VertexArray());
}

/***************************************************************************/
/***************************************************************************/
inline TriangleCluster* CTerrainCollideRectIterator::Cluster(void)
{
	return(d_collide->cluster);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainCollideRectIterator::Initialize(TerrainCollide &i_collide)
{
	d_collide = &i_collide;

	// Make sure this is a good test
	if (!d_collide->worldExtents.Intersects(d_rect))
		return(false);

	// Get a new test ID.
	d_iterator.Setup(d_collide->cluster, d_collide->IncrementTest());

	// Setup the sector bounds
	d_collide->PointToSector(d_rect.MinX(), d_rect.MinZ(), d_startX, d_sectorZ);
	if (d_startX < 0)
		d_startX = 0;
	if (d_sectorZ < 0)
		d_sectorZ = 0;

	d_collide->PointToSector(d_rect.MaxX(), d_rect.MaxZ(), d_endX, d_endZ);
	if (d_endX >= (int)d_collide->sectorsX)
		d_endX = (int)d_collide->sectorsX - 1;
	if (d_endZ >= (int)d_collide->sectorsZ)
		d_endZ = (int)d_collide->sectorsZ - 1;

	// Find a sector
	for (; d_sectorZ <= d_endZ; d_sectorZ++)
	{
		for (d_sectorX = d_startX; d_sectorX <= d_endX; d_sectorX++)
		{
			TerrainSector* sector = d_collide->ResolveSector(d_sectorX, d_sectorZ);
			if ((sector != NULL) && d_iterator.Initialize(*sector))
				return(true);
		}
	}

	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle* CTerrainCollideRectIterator::GetNextTriangle(void)
{
	TerrainTriangle* triangle = d_iterator.GetNextTriangle();

	if (triangle != NULL)
		return(triangle);

	for (d_sectorX++; d_sectorX <= d_endX; d_sectorX++)
	{
		TerrainSector* sector = d_collide->ResolveSector(d_sectorX, d_sectorZ);
		if ((sector != NULL) && d_iterator.Initialize(*sector) &&
				((triangle = d_iterator.GetNextTriangle()) != NULL))
			return(triangle);
	}

	for (d_sectorZ++; d_sectorZ <= d_endZ; d_sectorZ++)
	{
		for (d_sectorX = d_startX; d_sectorX <= d_endX; d_sectorX++)
		{
			TerrainSector* sector = d_collide->ResolveSector(d_sectorX, d_sectorZ);
			if ((sector != NULL) && d_iterator.Initialize(*sector) &&
					((triangle = d_iterator.GetNextTriangle()) != NULL))
				return(triangle);
		}
	}

	return(NULL);
}

//////////////////////// CTerrainCollideLineIterator ////////////////////////

/***************************************************************************/
/***************************************************************************/
inline Vector3Packed* CTerrainCollideLineIterator::VertexTable(void)
{
	return(d_collide->cluster->VertexArray());
}

/***************************************************************************/
/***************************************************************************/
inline TriangleCluster* CTerrainCollideLineIterator::Cluster(void)
{
	return(d_collide->cluster);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainCollideLineIterator::IncrementSector(void)
{
	d_progress += d_progressDelta;
	if (d_progress > 1.0f)
	{
		d_progress -= d_progressBackup;
		*d_outside += d_outsideInc;
	}
	else
		*d_inside += d_insideInc;

	return((d_sectorX != d_endX) && (d_sectorZ != d_endZ));
}

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainCollideLineIterator::Initialize(TerrainCollide &i_collide)
{
	if (InitializeSector(i_collide))
	{
		// Get a new test ID.
		d_iterator.Setup(d_collide->cluster, d_collide->IncrementTest());
		d_foundEnd = false;

		// Look for a sector that will work.
		while (IncrementSector())
		{
			TerrainSector* sector = d_collide->ResolveSector(d_sectorX, d_sectorZ);
			if ((sector != NULL) && d_iterator.Initialize(*sector))
				return(true);
		}
	}

	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle* CTerrainCollideLineIterator::GetNextTriangle(void)
{
	TerrainTriangle* triangle = d_iterator.GetNextTriangle();

	if (triangle != NULL)
		return(triangle);

	if (d_foundEnd)
		return(NULL);

	// Look for a sector that will work.
	while (IncrementSector())
	{
		TerrainSector* sector = d_collide->ResolveSector(d_sectorX, d_sectorZ);
		if ((sector != NULL) && d_iterator.Initialize(*sector) &&
				((triangle = d_iterator.GetNextTriangle()) != NULL))
			return(triangle);
	}

	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainCollideLineIterator::GetNextSector(
		CTerrainSectorInfo &o_sectorInfo)
{
	// Look for a sector that will work.
	while (IncrementSector())
	{
		TerrainSector* sector = d_collide->ResolveSector(d_sectorX, d_sectorZ);
		if (sector != NULL)
		{
			o_sectorInfo.d_collide = d_collide;
			o_sectorInfo.d_sector = sector;
			o_sectorInfo.d_sectorX = d_sectorX;
			o_sectorInfo.d_sectorZ = d_sectorZ;
			return(true);
		}
	}

	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline void CTerrainCollideLineIterator::FoundEnd(void)
{
	d_foundEnd = true;
}
