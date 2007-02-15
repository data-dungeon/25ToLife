/***************************************************************************/
// Oct-tree nodes
/***************************************************************************/
#ifndef OCTTREENODE_H
#define OCTTREENODE_H

#include "platform/BaseType.h"
#include "collide/cube.h"
#include "collide/terraintri.h"

//#define cHECKNODESIG
#ifdef CHECKNODESIG
#define NODESIG	0xf00dbeef
#endif

#pragma pack(1)
class OctTreeNode
{
public:
	// This can do no work run-time
	OctTreeNode() {}

	// But the tool can do it here
	OctTreeNode(uint triangles, uint childMask);

	// Each child pointer is represented by the bits
	// These are not the same os Octant::OCTANT's
	enum CHILDFLAGS
	{
		CHILDXYZ = 		0x01,
		CHILD_XYZ = 	0x02,
		CHILDX_YZ = 	0x04,
		CHILD_X_YZ = 	0x08,
		CHILDXY_Z = 	0x10,
		CHILD_XY_Z = 	0x20,
		CHILDX_Y_Z = 	0x40,
		CHILD_X_Y_Z = 	0x80
	};

	// Fixup from disk
	void DiskFixup(void *base);

	// How many triangles do we have?
	inline int Triangles() const;

	// Triangle pointer here, invalid if Triangles() == 0
	inline TerrainTriangle **TriangleList() const;

	// How many children does this node have?
	inline int Children() const;

	// Get a pointer to the compressed child list
	inline OctTreeNode **ChildList() const;

	// Get an expanded list of children
	inline void ExpandChildren(OctTreeNode *childList[Octant::OCTANTS]) const;

private:
#ifdef CHECKNODESIG
	u32				m_nodeSig;
#endif
	// The fixed data
	u16				m_childMask;
	u16				m_triangles;

	// m_triangles Triangle pointers (TerainTriangle *) here

	// child node pointers (OctTreeNodePtr *) follow based on contents of m_childMask
};
#pragma pack()

// A nice, easy to work with node that's been expanded
class ExpandedOctTreeNode
{
public:
	inline ExpandedOctTreeNode(const OctTreeNode *node);

	int					m_triangles;
	TerrainTriangle	**m_triList;

	OctTreeNode			*m_child[Octant::OCTANTS];
};

// A full blown node that includes full octant information
class FullOctTreeNode : public ExpandedOctTreeNode
{
public:
	// Setup the root
	inline FullOctTreeNode();
	inline FullOctTreeNode(const Octant &octant, const OctTreeNode *node);

	// Return a child
	inline FullOctTreeNode Child(Octant::OCTANT octant) const;

	// Make sure we can copy
	inline FullOctTreeNode &operator=(const FullOctTreeNode &n);

	Octant				m_octant;
};

// This should be as conservative as possible
#define MAX_OCTTREE_DEPTH		32

class OctTreeNodeStack
{
public:
	inline OctTreeNodeStack();
	inline OctTreeNodeStack(const Octant &rootOctant, const OctTreeNode *root);

	inline void Initialize(const Octant &rootOctant, const OctTreeNode *root);

	// What is the current depth?
	inline uint Depth();

	// What is the current entry
	inline FullOctTreeNode &Top();

	// Push down one
	inline void Push(Octant::OCTANT toChild);

	// Pop one off
	inline void Pop();

private:
	FullOctTreeNode		m_stack[MAX_OCTTREE_DEPTH];
	uint						m_top;
};

#include "collide/otnode.hpp"

#endif
