/***************************************************************************/
// Triangle cluster
/***************************************************************************/
#include "collide/CollidePCH.h"

/***************************************************************************/
// Initialize (we are loaded from disk, so we have to fixup pointers)
/***************************************************************************/
void BSPGeometryPool::DiskFixup(void)
{
	uint baseAddr = (uint)this;

#ifdef GCN
	EndianSwap();
#endif

	// None of these should be NULL (they all point to the end of the structure
	// if there is no data
	ASSERT(vertex != NULL);
	ASSERT(triangle != NULL);

	vertex = (Vector3Packed *)(baseAddr + (uint)vertex);
	triangle = (BSPTriangle *)(baseAddr + (uint)triangle);

#ifdef GCN
	u32 i;
	for(i = 0; i < vertices; i++)
		vertex[i].EndianSwap();
	for(i = 0; i < triangles; i++)
		triangle[i].EndianSwap();
#endif
}

