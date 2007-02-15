/***************************************************************************/
// Triangle cluster
/***************************************************************************/
#ifndef TCLUSTER_H
#define TCLUSTER_H

#include "collide/tsectgeo.h"

// forwards
class SectorizerTriangleCluster;

// Define this for edges
//#define WANTTERRAINEDGES

#define SUSPECT_RIGHT_HANDED 0x01
#define SUSPECT_UNIT_NORMAL  0x02
#define SUSPECT_CORNER1      0x04
#define SUSPECT_MULTI1       0x08
#define SUSPECT_CORNER2      0x10
#define SUSPECT_MULTI2       0x20
#define SUSPECT_CORNER3      0x40
#define SUSPECT_MULTI3       0x80

#define SUSPECT_CORNER       0x1
#define SUSPECT_MULTI        0x2
#define NEIGHBOR1_SHIFT        2
#define NEIGHBOR2_SHIFT        4
#define NEIGHBOR3_SHIFT        6

#pragma pack(1)
class TriangleCluster
{
public:
	// These can do nothing
	TriangleCluster(void) {}
	~TriangleCluster(void) {}

	// Initialize (we are loaded from disk, so we have to fixup pointers)
	void DiskFixup(void);

	// access our data
	inline TerrainTriangle *TriangleArray(void)
		{ return(triangle); }
	inline TerrainTriangle &Triangle(uint index)
		{ return(triangle[index]); }
	inline uint Triangles(void)
		{ return(triangles); }

	inline Vector3Packed *VertexArray(void)
		{ return(vertex); }
	inline Vector3Packed &Vertex(uint index)
		{ return(vertex[index]); }
	inline uint Vertices(void)
		{ return(vertices); }

#ifdef WANTTERRAINEDGES
	inline TerrainEdge *EdgeArray(void)
		{ return(edge); }
	inline TerrainEdge &Edge(uint index)
		{ return(edge[index]); }
	inline uint Edges(void)
		{ return(edges); }
#endif

	// Reset the test counter in all the triangles to 0
	void ResetTestID(void);

#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( vertex);
		ByteSwap( vertices);
		ByteSwap( triangle);
		ByteSwap( triangles);
#ifdef WANTTERRAINEDGES
		ByteSwap( edge);
		ByteSwap( edges);
#endif
	}
#endif

	struct SSuspectTriangle
	{
		TerrainTriangle* d_triangle;
		Vector3Packed* d_vertexTable;
		u32 d_reason;
	};
	void Validate(SSuspectTriangle* o_suspectList, u32 &io_numSuspects,
			u32 i_maxSuspects);
	u32 TestNeighbor(u32 i_tester, u32 i_v1, u32 i_v2);

private:
	// Our data
	Vector3Packed		*vertex;
	u32					vertices;
	TerrainTriangle	*triangle;
	u32					triangles;
#ifdef WANTTERRAINEDGES
	TerrainEdge			*edge;
	u32					edges;
#endif

	// our friend builds us
	friend class SectorizerTriangleCluster;
};
#pragma pack()

#endif
