/***************************************************************************/
// Header for a CDB chunk
/***************************************************************************/
#ifndef CDBHDR_H
#define CDBHDR_H

#include "collide/cube.h"

#define OTH_VERSION					0x00010000
#define OTH_VERTEXARRAY				0x10000000

#define MAX_COLLIDE_LAYERS			32

#pragma pack(1)
class CDBHeader
{
public:
	u32				layerSize[MAX_COLLIDE_LAYERS]; 	// Zero means no layer present

#ifdef GCN
	void EndianSwap()
	{
		for (int i = 0; i < MAX_COLLIDE_LAYERS; i++)
			ByteSwap(layerSize[i]);
	}
#endif
};
#pragma pack()

#pragma pack(1)
class OctTreeHeader
{
public:
	u32				version;
 	AABB				bounds;
	Octant			rootOctant;
	u32				vertices;
	u32				triangles;
	u32				leafDepth;

	char				pad[8];

#ifdef GCN
	void EndianSwap()
	{
		ByteSwap(version);
		bounds.EndianSwap();
		rootOctant.EndianSwap();
		ByteSwap(triangles);
		ByteSwap(leafDepth);
		ByteSwap(vertices);
	}
#endif
};
#pragma pack()

#endif
