/***************************************************************************/
// BSP geometry pool
/***************************************************************************/
#ifndef BSPPOOL_H
#define BSPPOOL_H

#include "collide/bsptri.h"

#pragma pack(1)
class BSPGeometryPool
{
public:
	// These can do nothing
	BSPGeometryPool() {}
	~BSPGeometryPool() {}

	// Initialize (we are loaded from disk, so we have to fixup pointers)
	void DiskFixup();

	// access our data
	inline BSPTriangle *TriangleArray()
		{ return(triangle); }
	inline BSPTriangle &Triangle(uint index)
		{ return(triangle[index]); }
	inline uint Triangles()
		{ return(triangles); }

	inline Vector3Packed *VertexArray()
		{ return(vertex); }
	inline Vector3Packed &Vertex(uint index)
		{ return(vertex[index]); }
	inline uint Vertices()
		{ return(vertices); }

	// Reset the test counter in all the triangles to 0
	void ResetTestID();

#ifdef GCN
	inline void EndianSwap()
	{
		ByteSwap(vertex);
		ByteSwap(vertices);
		ByteSwap(triangle);
		ByteSwap(triangles);
	}
#endif

private:
	// Our data
	Vector3Packed		*vertex;
	u32					vertices;
	BSPTriangle			*triangle;
	u32					triangles;
};
#pragma pack()

#endif
