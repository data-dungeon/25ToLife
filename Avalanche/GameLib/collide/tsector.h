/***************************************************************************/
// A terrain sector contains all the vertex and connectivity data for a
// sector of the terrain
//
// This construct is always loaded off disk, so there is no initialization,
// virtual functions or cleanup associated with this object
/***************************************************************************/
#ifndef TSECTOR_H
#define TSECTOR_H

#include "container/u24.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

class SectorizerTerrainSector; // For the converter
class TriangleCluster;
class CTerrainSectorIterator;

#pragma pack(1)
class TerrainSector
{
public:
	TerrainSector(void) {}
	~TerrainSector(void) {}

	// Initialize (we are loaded from disk, so we have to fixup pointers)
	inline void DiskFixup(void);

	void GetVerticalExtent(TriangleCluster* i_cluster, float &o_min,
			float &o_max) const;

#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( triangleIndex);
		ByteSwap( triangles);
#ifdef WANTTERRAINEDGES
		ByteSwap( edgeIndex);
		ByteSwap( edges);
#endif
	}
#endif

private:
	// Our list of triangles
	u24							*triangleIndex;
	uint				  			triangles;

#ifdef WANTTERRAINEDGES
	// Our list of edges
	u24							*edgeIndex;
	uint							edges;
#endif

	friend class SectorizerTerrainSector; // For the converter
	friend class CTerrainSectorIterator;

};
#pragma pack()

// inline implementations
#include "collide/tsector.hpp"

#endif
