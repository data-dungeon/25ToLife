/***************************************************************************/
// The main terrain collision object
/***************************************************************************/
#ifndef TCOLLIDE_H
#define TCOLLIDE_H

#include "collide/xzrect.h"
#include "container/sparseptr.h"
#include "collide/tcluster.h"
#include "collide/tsector.h"
#include "Layers/LayersStruct.h"

// The current verison
#define TERRAINCOLLIDE_CURRENTVERSION	0x00050005

// Forwards
class TerrainSector;
class CTerrainCollideRectIterator;
class CTerrainCollideLineIterator;
class CTerrainSectorInfo;

// The sectorizer is the converter
class SectorizerTerrainCollide;

// Not implemented
class TrianglePerimeterDatabase;
class CLoopSubdiv;

#pragma pack(1)
class TerrainCollide
{
public:
	// We can't do work here, we are born from the disk
	TerrainCollide(void) {}
	~TerrainCollide(void) {}

	// Intialize/terminate
	void Initialize(t_Handle dblHandle);
	void Terminate(void) {}

	// A simple query for the size of the terrain
	const XZRect &Size(void)
		{ return(worldExtents); }

	void Validate(TriangleCluster::SSuspectTriangle* o_suspectList,
			u32 &io_numSuspects, u32 i_maxSuspects)
		{cluster->Validate(o_suspectList, io_numSuspects, i_maxSuspects);}

#ifdef GCN
	void EndianSwap(void);
#endif

private:
	/*** structure must match disk image! if you add anything you must update tool chain! ***/

	// Header information from the file
	u32 											version			PRAGMAPACK;
	char											name[32]			PRAGMAPACK;

	// Some important data
	u32											sectorsX			PRAGMAPACK;
	u32											sectorsZ			PRAGMAPACK;
	float											sectorSize		PRAGMAPACK;
	float											invSectorSize	PRAGMAPACK;
	XZRect										worldExtents	PRAGMAPACK;

	// Our test counter
	u32											testCounter		PRAGMAPACK;
	inline u32 IncrementTest(void);

	// The cluster(s)
	TriangleCluster							*cluster			PRAGMAPACK;

	// The array of sectors
	SparsePointerMatrix<TerrainSector>	*sector			PRAGMAPACK;

	// This is NULL (we load it)
	TrianglePerimeterDatabase 				*perimeterData	PRAGMAPACK;

	// Object for evaluating subdivision surfaces
	CLoopSubdiv									*subdiv			PRAGMAPACK;

	/*** structure must match disk image! if you add anything you must update tool chain! ***/

	// Return the terrain sector that this point is in....
	// Returns NULL if this is off the map.
	inline TerrainSector *PointToSector(float x, float z);

	// Resolve this xsector/zsector to a TerrainSector (DOES NOT DO CLIPPING)
	inline TerrainSector *ResolveSector(int xsector, int zsector);

	// Resolve the sector with clipping
	inline TerrainSector *ResolveSectorClipped(int xsector, int zsector);

	// Convert this x/z coordinate pair into an xsector/zsector
	inline void PointToSector(float x, float z, int &sectorx, int &sectorz);

	// Is this sector off the map?
	inline bool SectorClipped(int sectorx, int sectorz);

	// aligned disk size of this record

	inline uint DiskSize(void) { return((sizeof(TerrainCollide) + 31) & ~31); }

	// Sector list data is here

	// for the converter
	friend class SectorizerTerrainCollide;
	friend class CTerrainCollideRectIterator;
	friend class CTerrainCollideLineIterator;
	friend class CTerrainSectorInfo;
};
#pragma pack()

/////////////////////////// CTerrainSectorIterator ///////////////////////////

class CTerrainSectorIterator
{
  public:
	void Setup(TriangleCluster* i_cluster, uint32 i_testID);
	bool Initialize(TerrainSector &i_sector);
	TerrainTriangle* GetNextTriangle(void);

  private:
	TerrainSector* d_sector;
	TriangleCluster* d_cluster;
	uint32 d_testID;
	int d_index;
};

//////////////////////// CTerrainCollideRectIterator ////////////////////////

class CTerrainCollideRectIterator
{
  public:
	XZRect &Rect(void);
	Vector3Packed* VertexTable(void);
	TriangleCluster* Cluster(void);
	bool Initialize(TerrainCollide &i_collide);
	TerrainTriangle* GetNextTriangle(void);

  private:
	TerrainCollide* d_collide;
	XZRect d_rect;
	int d_startX;
	int d_endX;
	int d_endZ;
	int d_sectorX;
	int d_sectorZ;
	CTerrainSectorIterator d_iterator;
};

//////////////////////// CTerrainCollideLineIterator ////////////////////////

class CTerrainCollideLineIterator
{
  public:
	void Setup(Vector3CRef i_start, Vector3CRef i_end);
	Vector3Packed* VertexTable(void);
	TriangleCluster* Cluster(void);
	bool InitializeSector(TerrainCollide &i_collide);
	bool Initialize(TerrainCollide &i_collide);
	TerrainTriangle* GetNextTriangle(void);
	bool GetNextSector(CTerrainSectorInfo &o_sectorInfo);
	void FoundEnd(void);

  private:
	bool IncrementSector(void);

	TerrainCollide* d_collide;
	bool d_foundEnd;
	Vector3 d_start;
	Vector3 d_end;
	int d_sectorX;
	int d_sectorZ;
	int d_endX;
	int d_endZ;
	enum
	{
		POS_X_POS_Z,
		POS_Z_POS_X,
		NEG_Z_POS_X,
		POS_X_NEG_Z,
		NEG_X_NEG_Z,
		NEG_Z_NEG_X,
		POS_Z_NEG_X,
		NEG_X_POS_Z
	} d_direction;
	int* d_inside;
	int* d_outside;
	int d_insideInc;
	int d_outsideInc;
	float d_progress;
	float d_progressDelta;
	float d_progressBackup;
	CTerrainSectorIterator d_iterator;
};

///////////////////////////// CTerrainSectorInfo /////////////////////////////

class CTerrainSectorInfo
{
  public:
	void Draw(void);

	TerrainCollide* d_collide;
	TerrainSector* d_sector;
	int d_sectorX;
	int d_sectorZ;
};

// Include our inlines!
#include "collide/tcollide.hpp"

#endif
