/***************************************************************************/
// Oct-tree nodes
/***************************************************************************/
#include "collide/CollidePCH.h"
#include "collide/otnode.h"
#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

/***************************************************************************/
// But the tool can do it here
/***************************************************************************/
OctTreeNode::OctTreeNode(
uint triangles,
uint childMask)
{
#ifdef CHECKNODESIG
	m_nodeSig = NODESIG;
#endif
	m_triangles = triangles;
	m_childMask = childMask;
}

/***************************************************************************/
// Fixup from disk
/***************************************************************************/
void OctTreeNode::DiskFixup(
void *base)
{
#ifdef GCN
	// Handle the byte swapping
	ByteSwap(m_triangles);
	ByteSwap(m_childMask);
#endif

#ifdef CHECKNODESIG
#ifdef GCN
	ByteSwap(m_nodeSig);
#endif
	ASSERT(m_nodeSig == NODESIG);
#endif

	// Some asserts
	ASSERT((m_childMask & 0xff00) == 0);

	// Handle the triangle list
	TerrainTriangle **tri = TriangleList();
	for (int t = 0; t < m_triangles; t++)
	{
#ifdef GCN
		// swap it
		ByteSwap( tri[t]);
#endif

		// SEtup the triangle pointer
		tri[t] = (TerrainTriangle *)((unsigned char *)base + (int)tri[t]);
	}

	// do the children
	int children = Children();
	OctTreeNode **childList = ChildList();
	for (int c = 0; c < children; c++)
	{
#ifdef GCN
		// swap it
		ByteSwap( childList[c]);
#endif

		// Setup the child pointer
		childList[c] = (OctTreeNode *)((unsigned char *)base + (int)childList[c]);
	}

	OctTreeNode *child[Octant::OCTANTS];
	ExpandChildren(child);
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (child[o])
			child[o]->DiskFixup(base);
	}
}



