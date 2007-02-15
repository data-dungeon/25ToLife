/***************************************************************************/
// The main terrain collision object
//
// This class is read in from disk, so the constructor can do no work.
/***************************************************************************/

/******************* includes ****************************/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/tcollide.h"

// Gamelib includes //
#include "collide/tsector.h"

/***************************************************************************/
// Load in a full TerrainCollide and all the TerrainSectors from disk
/***************************************************************************/
void TerrainCollide::Initialize(
t_Handle dblHandle)
{
#ifdef GCN
	EndianSwap();
#endif

	// This would be bad
	ASSERT(version == TERRAINCOLLIDE_CURRENTVERSION);

	// Fixup all the pointers
	uint baseAddr = (uint)this;

	// Do the cluster
	cluster = (TriangleCluster *)((uint)cluster + baseAddr);
	cluster->DiskFixup();

	// Fixup the sector array
	sector = (SparsePointerMatrix<TerrainSector> *)((uint)sector + baseAddr);
	sector->DiskFixup(baseAddr);

	// Initialize all the other sectors
	for (uint z = 0; z < sectorsZ; z++)
  		for (uint x = 0; x < sectorsX; x++)
		{
			TerrainSector *s = sector->Get(x, z);
			if (s)
				s->DiskFixup();
		}

	perimeterData = NULL;
	subdiv = NULL;
}

#ifdef GCN
/***************************************************************************/
/***************************************************************************/
void TerrainCollide::EndianSwap( void)
{
	ByteSwap(version);
	ByteSwap(sectorsX);
	ByteSwap(sectorsZ);
	ByteSwap(sectorSize);
	ByteSwap(invSectorSize);
	worldExtents.EndianSwap();
	ByteSwap(testCounter);
	ByteSwap(cluster);
	ByteSwap(sector);
	ByteSwap(perimeterData);
	ByteSwap(subdiv);
}
#endif

/***************************************************************************/
/***************************************************************************/
void CTerrainSectorInfo::Draw(void)
{
	Vector3 minCorner, maxCorner;

	minCorner.x( d_collide->worldExtents.MinX() +
			(float)d_sectorX * d_collide->sectorSize);
	minCorner.z( d_collide->worldExtents.MinZ() +
			(float)d_sectorZ * d_collide->sectorSize);
	maxCorner.x( minCorner.x() + d_collide->sectorSize);
	maxCorner.z( minCorner.z() + d_collide->sectorSize);
	float miny, maxy; 

	d_sector->GetVerticalExtent(d_collide->cluster, miny, maxy);

	minCorner.y(miny);
	maxCorner.y(maxy);

	DrawUtility::Box(minCorner, maxCorner);
}

//////////////////////// CTerrainCollideLineIterator ////////////////////////

/***************************************************************************/
/***************************************************************************/
void CTerrainCollideLineIterator::Setup(Vector3CRef i_start,
		Vector3CRef i_end)
{
	d_start = i_start;
	d_end = i_end;
	float deltaX = d_end.x() - d_start.x();
	float deltaZ = d_end.z() - d_start.z();

	float outsideDelta;
	if (deltaX > 0.0f)
	{
		if (deltaZ > deltaX)
		{
			d_direction = POS_Z_POS_X;
			d_inside = &d_sectorZ;
			d_outside = &d_sectorX;
			d_insideInc = 1;
			d_outsideInc = 1;
			d_progressDelta = deltaX;
			outsideDelta = deltaZ;
		}
		else if (deltaZ > 0.0f)
		{
			d_direction = POS_X_POS_Z;
			d_inside = &d_sectorX;
			d_outside = &d_sectorZ;
			d_insideInc = 1;
			d_outsideInc = 1;
			d_progressDelta = deltaZ;
			outsideDelta = deltaX;
		}
		else if (deltaZ > -deltaX)
		{
			d_direction = POS_X_NEG_Z;
			d_inside = &d_sectorX;
			d_outside = &d_sectorZ;
			d_insideInc = 1;
			d_outsideInc = -1;
			d_progressDelta = -deltaZ;
			outsideDelta = deltaX;
		}
		else
		{
			d_direction = NEG_Z_POS_X;
			d_inside = &d_sectorZ;
			d_outside = &d_sectorX;
			d_insideInc = -1;
			d_outsideInc = 1;
			d_progressDelta = -deltaX;
			outsideDelta = deltaZ;
		}
	}
	else
	{
		if (deltaZ < deltaX)
		{
			d_direction = NEG_Z_NEG_X;
			d_inside = &d_sectorZ;
			d_outside = &d_sectorX;
			d_insideInc = -1;
			d_outsideInc = -1;
			d_progressDelta = deltaX;
			outsideDelta = deltaZ;
		}
		else if (deltaZ < 0.0f)
		{
			d_direction = NEG_X_NEG_Z;
			d_inside = &d_sectorX;
			d_outside = &d_sectorZ;
			d_insideInc = -1;
			d_outsideInc = -1;
			d_progressDelta = deltaZ;
			outsideDelta = deltaX;
		}
		else if (deltaZ < -deltaX)
		{
			d_direction = NEG_X_POS_Z;
			d_inside = &d_sectorX;
			d_outside = &d_sectorZ;
			d_insideInc = -1;
			d_outsideInc = 1;
			d_progressDelta = -deltaZ;
			outsideDelta = deltaX;
		}
		else
		{
			d_direction = POS_Z_NEG_X;
			d_inside = &d_sectorZ;
			d_outside = &d_sectorX;
			d_insideInc = 1;
			d_outsideInc = -1;
			d_progressDelta = -deltaX;
			outsideDelta = deltaZ;
		}
	}
	if (Math::DivByZero(d_progressDelta, outsideDelta))
		d_progressDelta = 0.0f;
	else
		d_progressDelta /= outsideDelta;
	d_progressBackup = 1.0f + d_progressDelta;
}

/***************************************************************************/
/***************************************************************************/
bool CTerrainCollideLineIterator::InitializeSector(TerrainCollide &i_collide)
{
	d_collide = &i_collide;

	// Clip the line and exit if the line does not cross the region.
	Vector3 clippedStart, clippedEnd;
	ClipSegment clipper(
			d_collide->worldExtents.MinX(), d_collide->worldExtents.MinZ(),
			d_collide->worldExtents.MaxX(), d_collide->worldExtents.MaxZ());
	if (!clipper.Clip(d_start, d_end, clippedStart, clippedEnd))
		return(false);

	// Find the sector containing the start point and the position of the
	// start point in the sector.
	float relX = (clippedStart.x() - d_collide->worldExtents.MinX()) *
			d_collide->invSectorSize;
	float relZ = (clippedStart.z() - d_collide->worldExtents.MinZ()) *
			d_collide->invSectorSize;
	d_sectorX = (int)relX;
	d_sectorZ = (int)relZ;
	relX -= (float)d_sectorX;
	relZ -= (float)d_sectorZ;

	// Decrement the inside index so the initial increment will hit the first
	// sector.
	*d_inside -= d_insideInc;

	// Find the sector containing the end point.
	d_endX = (int)((clippedEnd.x() - d_collide->worldExtents.MinX()) *
			d_collide->invSectorSize);
	d_endZ = (int)((clippedEnd.z() - d_collide->worldExtents.MinZ()) *
			d_collide->invSectorSize);

	// Initialize the progress term. Subtract a delta term so the initial
	// increment will hit the first sector. Push the ends out so we can detect
	// when the line has gone past the end.
	switch(d_direction)
	{
	 case POS_X_POS_Z:
	 {
		d_progress = relZ - (relX + 1.0f) * d_progressDelta;
		d_endX++;
		d_endZ++;
		break;
	 }
	 case POS_Z_POS_X:
	 {
		d_progress = relX - (relZ + 1.0f) * d_progressDelta;
		d_endX++;
		d_endZ++;
		break;
	 }
	 case NEG_Z_POS_X:
	 {
		d_progress = relX + (relZ - 2.0f) * d_progressDelta;
		d_endX++;
		d_endZ--;
		break;
	 }
	 case POS_X_NEG_Z:
	 {
		d_progress = 1.0f - relZ - (relX + 1.0f) * d_progressDelta;
		d_endX++;
		d_endZ--;
		break;
	 }
	 case NEG_X_NEG_Z:
	 {
		d_progress = 1.0f - relZ + (relX - 2.0f) * d_progressDelta;
		d_endX--;
		d_endZ--;
		break;
	 }
	 case NEG_Z_NEG_X:
	 {
		d_progress = 1.0f - relX + (relZ - 2.0f) * d_progressDelta;
		d_endX--;
		d_endZ--;
		break;
	 }
	 case POS_Z_NEG_X:
	 {
		d_progress = 1.0f - relX - (relZ + 1.0f) * d_progressDelta;
		d_endX--;
		d_endZ++;
		break;
	 }
	 case NEG_X_POS_Z:
	 {
		d_progress = relZ + (relX - 2.0f) * d_progressDelta;
		d_endX--;
		d_endZ++;
		break;
	 }
	}

	return(true);
}
