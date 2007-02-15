//***************************************************************************
// Class to build a BSP tree
//***************************************************************************
#include "Pch.h"

#include <collide/otnode.h>

// How much do we inflate an octant
#define OCTANT_INFLATE		0.005f

//***************************************************************************
// Constructor does all the work
//***************************************************************************
FactoryOctTree::FactoryOctTree(
TriangleSoup &triangleSoup,
const AABB &worldSize,
const DrOctopusOptions &options) :
m_options(options)
{
	// Save off our size
	m_aabb = worldSize;

	// Make a root
	m_root = new FactoryOctTreeNode(Octant::Root(m_aabb));
	assert(m_root);

	// calculate the number of iterations that can be done
	// try to find the closest depth to the minDepthSize
	uint minLeafDepth = 1;
	while (1)
	{
		float mySize = m_root->GetOctant().SizeAtDepth(minLeafDepth);
		float childSize = m_root->GetOctant().SizeAtDepth(minLeafDepth+1);
		float midSize = (mySize+childSize)/2.0f;
		if (midSize<=options.minDepthSize)
		{
			break;
		}
		minLeafDepth++;
	}
	uint maxLeafDepth = minLeafDepth+2; // go 2 more levels deep for now

	// Create the triangle list
	m_triangles = triangleSoup.Triangles();

	// Make the appropriate triangle list
	if (!options.leanTriangles)
	{
		m_fatTriList = triangleSoup.ConvertForDiskFat();
		assert(m_fatTriList);
	}
	else
	{
		m_leanList = triangleSoup.ConvertForDiskLean();
		assert(m_leanList);
	}

	// Process all the triangles
	for (int t = 0; t < m_triangles; t++)
	{
		if (!m_options.leanTriangles)
		{
			TerrainTriangleFat *tri = &m_fatTriList[t];
			m_root->ProcessTriangle(tri, tri->A(), tri->B(), tri->C(), tri->FaceNormal(), minLeafDepth, maxLeafDepth, m_options);
		}
		else
		{
			TerrainTriangleLean *tri = &m_leanList->triangle[t];
			m_root->ProcessTriangle(tri, tri->A(), tri->B(), tri->C(), tri->FaceNormal(), minLeafDepth, maxLeafDepth, m_options);
		}
	}

	// Prune
	m_root->Prune();

	// We are done!
}

//***************************************************************************
//***************************************************************************
FactoryOctTree::~FactoryOctTree()
{
	delete m_root;

	if (!m_options.leanTriangles)
		delete [] m_fatTriList;
	else
		TriangleSoup::CleanUp(m_leanList);
}

//***************************************************************************
// Output the database
//***************************************************************************
int FactoryOctTree::WriteDatabase(
FILE *fh)
{
	// Doh!
	assert(m_root);
	assert(fh);

	// Create the file thingie
	OctTreeFile file(fh);

	// Setup a header
	OctTreeHeader hdr;
	memset(&hdr, '\0', sizeof(hdr));
	hdr.version = OTH_VERSION;
	if (m_options.leanTriangles)
		hdr.version |= OTH_VERTEXARRAY;
	hdr.bounds = m_aabb;
	hdr.rootOctant = m_root->GetOctant();
	hdr.triangles = m_triangles;
	if (m_options.leanTriangles)
		hdr.vertices = m_leanList->vertices;

	// Leaf depth?
	uint leafDepth = 0;
	m_root->MinimumLeafDepth(leafDepth);
	hdr.leafDepth = leafDepth;

	// Write the header
	if (!file.Write(hdr))
		throw(PluginException(ERROR_FATAL,"OutputCollisionDatabase - Cannot write oct tree header"));

	// Write triangles
	if (!m_options.leanTriangles)
	{
		if (!file.Write(m_fatTriList, m_triangles))
			throw(PluginException(ERROR_FATAL,"OutputCollisionDatabase - Cannot write triangle list"));
	}
	else
	{
		if (!file.Write(m_leanList, m_triangles))
			throw(PluginException(ERROR_FATAL,"OutputCollisionDatabase - Cannot write triangle list"));
	}

	// Compute the depth of the tree
	uint maxDepth = 0;
	m_root->MaxDepth(maxDepth);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Depth  Node Count  Triangle Count  World Octant Size");

	for (uint depthIndex = 0; depthIndex <= maxDepth; depthIndex++)
	{
		uint nodeCount = 0;
		uint triCount = 0;
		m_root->CountsAtDepth(depthIndex,nodeCount,triCount);
		float octantSize = m_root->GetOctant().SizeAtDepth(depthIndex);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH," %2d   %6d          %8d            %8.2f",depthIndex,nodeCount,triCount,octantSize);
	}

	// How big is the node part of this tree going to be?
	int treeSize = 0;
	m_root->DiskSize(treeSize);
	file.StartTree(treeSize);

	// Write out in depth order
	for (uint depth = 0; depth <= maxDepth; depth++)
		m_root->Write(file, depth);

	// Now write the fixups
	m_root->WriteFixups(file);

	int errors = 0;
	m_root->ValidateWrite(errors);
	if (errors != 0)
		throw(PluginException(ERROR_FATAL,"OutputCollisionDatabase - Database errors!!!"));

	// Give them back the total size
	return file.TotalSize();
}

//***************************************************************************
//***************************************************************************
FactoryOctTreeNode::FactoryOctTreeNode(
const Octant &rootOctant) :
m_triList(0, 16, false)
{
	// Save everything
	m_parent = NULL;
	m_octant = rootOctant;
	m_octantID = Octant::OCTANTS;
	m_depth = 0;

	// Null out all the pointers
	for (int o = 0; o < Octant::OCTANTS; o++)
		m_child[o] = NULL;

	// Initialize this to something bad
	m_nodeOffset = NODE_BADOFFSET;
	m_nodeWritten = false;
	m_nodeFixedup = false;
}

//***************************************************************************
//***************************************************************************
FactoryOctTreeNode::FactoryOctTreeNode(
const Octant &octant,
Octant::OCTANT octantID,
FactoryOctTreeNode *parent,
uint depth) :
m_triList(0, 16, false)
{
	// Save everything
	m_parent = parent;
	m_octant = octant;
	m_octantID = octantID;
	m_depth = depth;

	// Update stats
	if (m_depth > g_stats.maxDepth)
		g_stats.maxDepth = m_depth;
	++g_stats.nodes;
	if (g_stats.smallestNode < 0.0f || m_octant.Size() < g_stats.smallestNode)
		g_stats.smallestNode = m_octant.Size();

	// Null out all the pointers
	for (int o = 0; o < Octant::OCTANTS; o++)
		m_child[o] = NULL;

	// Initialize this to something bad
	m_nodeOffset = NODE_BADOFFSET;
	m_nodeWritten = false;
	m_nodeFixedup = false;
}

//***************************************************************************
//***************************************************************************
FactoryOctTreeNode::~FactoryOctTreeNode()
{
	// Woo hoo clear out our children!
	for (int o = 0; o < Octant::OCTANTS; o++)
		delete m_child[o];
}

//***************************************************************************
// What is this node's octant
//***************************************************************************
Octant FactoryOctTreeNode::GetOctant()
{
	return m_octant;
}

//***************************************************************************
// Process a triangle
//***************************************************************************
void FactoryOctTreeNode::ProcessTriangle(
TerrainTriangleBase *tri,
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
const Vector3 &normal,
uint minLeafDepth,
uint maxLeafDepth,
const DrOctopusOptions &options)
{
	// Some asserts
	assert(tri);

	// above the depth size, just feed this to our children
	if (m_depth < minLeafDepth)
	{
		for (int o = 0; o < Octant::OCTANTS; o++)
		{
			// What is this child's octant
			Octant childOctant = m_octant.Child((Octant::OCTANT)o);

			// Does this triangle touch the child octant?
			if (childOctant.Touches(normal, a, b, c, OCTANT_INFLATE))
			{
				// Create the child
				if (!m_child[o])
					m_child[o] = new FactoryOctTreeNode(childOctant, (Octant::OCTANT)o, this, m_depth + 1);
				m_child[o]->ProcessTriangle(tri, a, b, c, normal, minLeafDepth, maxLeafDepth, options);
			}
		}
	}
	else // The triangle is allowed to be in this leaf
	{
		// What is the best octant for this triangle
		const Vector3 centroid = (a + b + c) / 3.0f;
		Octant::OCTANT bestOctant = m_octant.BestChild(centroid);

		// What is the best child's octant
		Octant childOctant = m_octant.Child(bestOctant);

		// Does it fit entirely?
		if (m_depth+1 > maxLeafDepth || !childOctant.Contains(a, b, c))
		{
			// Just put it here
			m_triList.Add(tri);

			// Update the stats
			if (m_triList.Count() > (int)g_stats.maxTrianglesInNode)
			{
				g_stats.maxTrianglesInNode = m_triList.Count();
				g_stats.maxTrianglesInNodeDepth = m_depth;
			}

			// This should never happen
			if (m_depth == 0)
				g_stats.rootTriangles = m_triList.Count();
		}
		else
		{
			// Stick it in!
			if (!m_child[bestOctant])
				m_child[bestOctant] = new FactoryOctTreeNode(childOctant, bestOctant, this, m_depth + 1);
			m_child[bestOctant]->ProcessTriangle(tri, a, b, c, normal, minLeafDepth, maxLeafDepth, options);
		}
	}
}

//***************************************************************************
// Prune all the empty nodes from the tree
//***************************************************************************
void FactoryOctTreeNode::Prune(void)
{
	// First call on all my children
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o])
			m_child[o]->Prune();
	}

	// Are we empty?
	for (o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o] && m_child[o]->Empty())
		{
			delete m_child[o];
			m_child[o] = NULL;
			++g_stats.pruned;
		}
	}
}

//***************************************************************************
// How deep is this tree?
//***************************************************************************
void FactoryOctTreeNode::MaxDepth(
uint &maxDepth)
{
	if (m_depth > maxDepth)
		maxDepth = m_depth;

	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o])
			m_child[o]->MaxDepth(maxDepth);
	}
}

//***************************************************************************
// How deep is this tree?
//***************************************************************************
void FactoryOctTreeNode::CountsAtDepth(
uint testDepth,
uint &nodeCount,
uint &triCount)
{
	if (m_depth == testDepth)
	{
		nodeCount++;
		triCount+= m_triList.Count();
	}
	else if (m_depth < testDepth)
	{
		for (int o = 0; o < Octant::OCTANTS; o++)
		{
			if (m_child[o])
				m_child[o]->CountsAtDepth(testDepth,nodeCount,triCount);
		}
	}
}


//***************************************************************************
// What is the lowest leaf?
//***************************************************************************
void FactoryOctTreeNode::MinimumLeafDepth(
uint &leafDepth)
{
	if ((leafDepth == 0 || m_depth < leafDepth) && m_triList.Count() > 0)
		leafDepth = m_depth;

	// Call the children
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o])
			m_child[o]->MinimumLeafDepth(leafDepth);
	}
}

//***************************************************************************
// How big are the nodes?
//***************************************************************************
void FactoryOctTreeNode::DiskSize(
int &size)
{
	// What is our size?
	size += sizeof(OctTreeNode) +
		 		m_triList.Count() * sizeof(TerrainTriangleBase *) +
				UsedChildren() * sizeof(OctTreeNode *);

	// Call the children
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o])
			m_child[o]->DiskSize(size);
	}
}

//***************************************************************************
// Write out the node and return it's disk offset
//***************************************************************************
void FactoryOctTreeNode::Write(
OctTreeFile &file,
uint forDepth)
{
	// Are we at the right level?
	if (forDepth < m_depth)
		return;
	else if (forDepth > m_depth)
	{
		// Call on our children
		for (int o = 0; o < Octant::OCTANTS; o++)
		{
			if (m_child[o])
				m_child[o]->Write(file, forDepth);
		}
		return;
	}

	// We are writing at our depth

	// Save the position of the node
	m_nodeOffset = file.GetOffset();

	// How many used children do we have?
	u16 childMask;
	int usedChildren = UsedChildren(&childMask);

	// We should have triangles or children
	assert(usedChildren > 0 || m_triList.Count() > 0);

	// Create the node
#if 0
	OctTreeNode *parent = file.NodeOffset(m_parent);
#endif
	OctTreeNode node(m_triList.Count(), childMask);

	// Write the node
	if (!file.Write(&node, sizeof(node)))
		throw(PluginException(ERROR_FATAL, "NodeWrite - Cannot write data to file!"));

	// We are down
	m_nodeWritten = true;

	// Compute the size of this node
	int nodeSize = sizeof(OctTreeNode) +
						m_triList.Count() * sizeof(TerrainTriangleBase *) +
						usedChildren * sizeof(OctTreeNode *);

	// Do we have a triangle pointer?
	if (m_triList.Count() > 0)
	{
		// Make an offset table
		int triangles = m_triList.Count();
		TerrainTriangleBase **triangleOffset = new TerrainTriangleBase *[triangles];
		assert(triangleOffset);

		for (int t = 0; t < triangles; t++)
			triangleOffset[t] = file.TriangleOffset(m_triList.Get(t));

		// Write the pointer to the triangle data
		if (!file.Write(triangleOffset, sizeof(TerrainTriangleBase *) * triangles))
			throw(PluginException(ERROR_FATAL, "NodeWrite - Cannot write data to file!"));

		// Clean up the converted data
		delete [] triangleOffset;
	}

	// Seek to the end of the node
	file.Goto(m_nodeOffset + nodeSize);
}

//***************************************************************************
// Fixup
//***************************************************************************
void FactoryOctTreeNode::WriteFixups(
OctTreeFile &file)
{
	// Build up the used children pointers
	size_t children = 0;
	OctTreeNode *childOffset[Octant::OCTANTS];
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o])
			childOffset[children++] = file.NodeOffset(m_child[o]);
	}
	assert(children == (size_t)UsedChildren());

	// Nothing to do here if we have no children
	m_nodeFixedup = true;
	if (children == 0)
		return;

	// Go to where the fixup should be
	file.Goto(m_nodeOffset + sizeof(OctTreeNode) + m_triList.Count() * sizeof(TerrainTriangleBase *));

	// Fixup our table
	if (!file.Write(childOffset, sizeof(OctTreeNode *) * children))
		throw(PluginException(ERROR_FATAL, "NodeWrite - Cannot write data to file!"));

	// Recurse through all our children
	for (o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o])
			m_child[o]->WriteFixups(file);
	}
}

//***************************************************************************
// Validate the write
//***************************************************************************
void FactoryOctTreeNode::ValidateWrite(
int &errors)
{
	assert(m_nodeWritten);
	assert(m_nodeFixedup);
	assert(m_nodeOffset != NODE_BADOFFSET);

	if (!m_nodeWritten)
		++errors;
	if (!m_nodeFixedup)
		++errors;
	if (m_nodeOffset == NODE_BADOFFSET)
		++errors;

	// Recurse through all our children
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o])
			m_child[o]->ValidateWrite(errors);
	}
}

//***************************************************************************
// Weee
//***************************************************************************
int FactoryOctTreeNode::DiskOffset()
{
	assert(m_nodeOffset != NODE_BADOFFSET);

	return m_nodeOffset;
}

//***************************************************************************
// How many triangles are in this node?
//***************************************************************************
int FactoryOctTreeNode::TotalTriangles()
{
	int count = m_triList.Count();
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (m_child[o])
			count += m_child[o]->TotalTriangles();
	}

	return count;
}

//***************************************************************************
//***************************************************************************
int FactoryOctTreeNode::UsedChildren(
u16 *childMask)
{
	// How many used children do we have?
	int usedChildren = 0;
	if (childMask)
		*childMask = 0;
	uint childBit = 1;
	for (int o = 0; o < Octant::OCTANTS; o++, childBit <<= 1)
	{
		if (m_child[o])
		{
			if (childMask)
				*childMask |= childBit;
			++usedChildren;
		}
	}

	return usedChildren;
}

//***************************************************************************
// Is this node empty?
//***************************************************************************
bool FactoryOctTreeNode::Empty()
{
	return (m_triList.Count() == 0 && UsedChildren() == 0);
}

//***************************************************************************
//***************************************************************************
OctTreeFile::OctTreeFile(
FILE *fh)
{
	m_fh = fh;

	m_baseSeek = ftell(m_fh);
}

//***************************************************************************
//***************************************************************************
OctTreeFile::~OctTreeFile()
{
	// Make sure we are at the end of the file
	fseek(m_fh, m_baseSeek + TotalSize(), SEEK_SET);
}

//***************************************************************************
// Get the total size
//***************************************************************************
int OctTreeFile::TotalSize()
{
	return m_hdrSize + m_vertexSize + m_triangleSize + m_treeSize;
}

//***************************************************************************
// Write the header
//***************************************************************************
bool OctTreeFile::Write(
OctTreeHeader &hdr)
{
	m_hdrSize = sizeof(hdr);
	return (fwrite(&hdr, m_hdrSize, 1, m_fh) == 1);
}

//***************************************************************************
// Write the triangles
//***************************************************************************
bool OctTreeFile::Write(
TerrainTriangleFat *tri,
int triangles)
{
	m_vertexSize = 0;
	m_triangleOffset = GetOffset();
	m_triangleAddr = (uint)tri;
	m_triangleSize = sizeof(TerrainTriangleFat) * triangles;

	return (fwrite(tri, m_triangleSize, 1, m_fh) == 1);
}

//***************************************************************************
// Write the triangles
//***************************************************************************
bool OctTreeFile::Write(
TriangleSoup::LeanTriangles *leanList,
int triangles)
{
	// Write the vertex table
	m_vertexSize = sizeof(Vector3Packed) * leanList->vertices;
	if (fwrite(leanList->vertex, m_vertexSize, 1, m_fh) != 1)
		return false;

	// Now do the triangles
	m_triangleOffset = GetOffset();
	m_triangleAddr = (uint)leanList->triangle;
	m_triangleSize = sizeof(TerrainTriangleLean) * triangles;

	return (fwrite(leanList->triangle, m_triangleSize, 1, m_fh) == 1);
}

//***************************************************************************
// Start the tree
//***************************************************************************
void OctTreeFile::StartTree(
int treeSize)
{
	m_treeOffset = GetOffset();
	m_treeSize = treeSize;
}

//***************************************************************************
// Get the offset of a node
//***************************************************************************
OctTreeNode *OctTreeFile::NodeOffset(
FactoryOctTreeNode *node)
{
	if (!node)
		return 0;
	else
	{
		int offset = node->DiskOffset();

		assert(offset >= m_treeOffset && offset < (m_treeOffset + m_treeSize));

		return (OctTreeNode *)offset;
	}
}

//***************************************************************************
// Get the offset of a triangle
//***************************************************************************
TerrainTriangleBase *OctTreeFile::TriangleOffset(
TerrainTriangleBase *tri)
{
	assert(tri);

	if (!tri)
		return 0;
	else
	{
		int offset = ((uint)tri - m_triangleAddr) + (int)m_triangleOffset;

		assert(offset >= m_triangleOffset && offset < m_treeOffset);

		return (TerrainTriangleBase *)offset;
	}
}

//***************************************************************************
// Write a bit of data
//***************************************************************************
bool OctTreeFile::Write(
void *data,
int size)
{
	return (fwrite(data, size, 1, m_fh) == 1);
}

//***************************************************************************
// Get the current offset
//***************************************************************************
int OctTreeFile::GetOffset()
{
	return ftell(m_fh) - m_baseSeek;
}

//***************************************************************************
// Go to an offset
//***************************************************************************
void OctTreeFile::Goto(
int offset)
{
	fseek(m_fh, offset + m_baseSeek, SEEK_SET);
}



