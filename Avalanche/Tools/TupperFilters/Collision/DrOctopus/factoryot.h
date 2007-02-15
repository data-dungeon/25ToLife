//***************************************************************************
// Class to build an octtree
//***************************************************************************
#ifndef FACTORYOCTTREE_H
#define FACTORYOCTTREE_H

#include <stdio.h>
#include <collide\cube.h>

class	TriangleSoup;
class FactoryOctTreeNode;
class OctTreeFile;
class OctTreeNode;

#define NODE_BADOFFSET		0xf00dbeef

// The root
class FactoryOctTree
{
public:
	// Constructor does all the work
	FactoryOctTree(TriangleSoup &triangleSoup, const AABB &worldSize, const DrOctopusOptions &options);
	~FactoryOctTree();

	// Output the database
	int WriteDatabase(FILE *fh);

private:
	// Our basic info
	AABB									m_aabb;
	const DrOctopusOptions			&m_options;

	// The root
	FactoryOctTreeNode				*m_root;

	// How many triangles do we have?
	int									m_triangles;

	// The fat terrain triangle list
	TerrainTriangleFat				*m_fatTriList;

	// The lean terrain triangle list
	TriangleSoup::LeanTriangles 	*m_leanList;
};

// The actual node
class FactoryOctTreeNode
{
public:
	// All the work is in the constructor, I love tools! (okay, I'm kidding)
	FactoryOctTreeNode(const Octant &rootOctant);
	FactoryOctTreeNode(const Octant &octant,
								Octant::OCTANT octantID,
								FactoryOctTreeNode *parent = NULL,
								uint depth = 0);
	~FactoryOctTreeNode();

	// What is this node's octant
	Octant GetOctant();

	// Process a triangle
	void ProcessTriangle(TerrainTriangleBase *tri,
								const Vector3 &a,
								const Vector3 &b,
								const Vector3 &c,
								const Vector3 &normal,
								uint minLeafDepth,
								uint maxLeafDepth,
								const DrOctopusOptions &options);

	// Prune all the empty nodes from the tree
	void Prune(void);

	// How deep is this tree?
	void MaxDepth(uint &maxDepth);

	// How many nodes at given depth
	void CountsAtDepth(uint testDepth,uint &nodeCount,uint &triCount);

	// What is the lowest leaf?
	void MinimumLeafDepth(uint &leafDepth);

	// How big are the nodes
	void DiskSize(int &nodeSize);

	// Write out the node and return the size
	void Write(OctTreeFile &file, uint forDepth);

	// Fixup
	void WriteFixups(OctTreeFile &file);

	// Validate the write
	void ValidateWrite(int &errors);

	// Weee
	int DiskOffset();

	// How many triangles are in this node?
	int TotalTriangles();

private:
	// our parent
	FactoryOctTreeNode				*m_parent;

	// our bounding volume
	Octant								m_octant;
	Octant::OCTANT						m_octantID;

	// Our depth
	uint									m_depth;

	// The triangles that lie this octant
	DynList<TerrainTriangleBase>	m_triList;

	// Our chiuldren
	FactoryOctTreeNode				*m_child[Octant::OCTANTS];

	// Stuff for disk writing
	int									m_nodeOffset;
	bool									m_nodeWritten;
	bool									m_nodeFixedup;

	// How many children are in use?  If childmask is non-null
	// The bitmask for used children is filled out
	int									UsedChildren(u16 *childMask = NULL);

	// Is this node empty?
	bool Empty();
};

// Used for writing stuff out
class OctTreeFile
{
public:
	OctTreeFile(FILE *fh);
	~OctTreeFile();

	// Get the total size
	int TotalSize();

	// Write the header
	bool Write(OctTreeHeader &hdr);

	// Write the triangles
	bool Write(TerrainTriangleFat *tri, int triangles);

	// Write the triangles
	bool Write(TriangleSoup::LeanTriangles *leanList, int triangles);

	// Start the tree
	void StartTree(int treeSize);

	// Get the disk offset of an object
	OctTreeNode *NodeOffset(FactoryOctTreeNode *node);

	// Get the offset of a triangle
	TerrainTriangleBase *TriangleOffset(TerrainTriangleBase *tri);

	// Write a bit of data
	bool Write(void *data, int size);

	// Get the current offset
	int GetOffset();

	// Go to an offset
	void Goto(int offset);

private:
	FILE		*m_fh;

	int		m_baseSeek;

	int		m_hdrSize;

	int		m_vertexSize;

	uint		m_triangleAddr;
	int		m_triangleOffset;
	int		m_triangleSize;

	int		m_treeOffset;
	int		m_treeSize;
};


#endif
