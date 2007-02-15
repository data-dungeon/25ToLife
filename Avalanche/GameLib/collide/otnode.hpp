/***************************************************************************/
// Oct-tree nodes
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
inline int OctTreeNode::Triangles() const
{
	return m_triangles;
}

/***************************************************************************/
// Triangle pointer here, invalid if Triangles() == 0
/***************************************************************************/
inline TerrainTriangle **OctTreeNode::TriangleList() const
{
	// Get at the pointer
	return (TerrainTriangle **)(this + 1);
}

/***************************************************************************/
// Get a pointer to the compressed child list
/***************************************************************************/
inline OctTreeNode **OctTreeNode::ChildList() const
{
	return (OctTreeNode **)((char *)(this + 1) + sizeof(TerrainTriangle *) * m_triangles);
}

/***************************************************************************/
// How many children does this node have?
/***************************************************************************/
inline int OctTreeNode::Children() const
{
	// Assume none for now (as you can tell, this isn't fast)
	int children = 0;

	// Count the bits in the m_childMask field
	uint16 test = m_childMask;
	for (int o = 0; o < Octant::OCTANTS; o++, test >>= 1)
	{
		if ((test & 1) != 0)
			++children;
	}

	return children;
}

/***************************************************************************/
// Get an expanded list of children
/***************************************************************************/
inline void OctTreeNode::ExpandChildren(
OctTreeNode *childList[Octant::OCTANTS]) const
{
	uint16 test = m_childMask;
	OctTreeNode **ptr = ChildList();
	for (int o = 0; o < Octant::OCTANTS; o++, test >>= 1)
	{
		if ((test & 1) == 0)
			childList[o] = NULL;
		else
		{
			childList[o] = *ptr++;
			ASSERT((uint)this < (uint)childList[o]);
		}
	}
}

/***************************************************************************/
/***************************************************************************/
inline ExpandedOctTreeNode::ExpandedOctTreeNode(
const OctTreeNode *node)
{
	if (node)
	{
		// Get the triangle list pointer
		m_triangles = node->Triangles();
		m_triList = node->TriangleList();

		// Unwind the pointers for chidlren
		node->ExpandChildren(m_child);
	}
	else
	{
		// Nothing to see here
		m_triangles = 0;

		for (int o = 0; o < Octant::OCTANTS; o++)
			m_child[o] = NULL;
	}
}

/***************************************************************************/
/***************************************************************************/
inline FullOctTreeNode::FullOctTreeNode() :
ExpandedOctTreeNode(NULL)
{
}

/***************************************************************************/
/***************************************************************************/
inline FullOctTreeNode::FullOctTreeNode(
const Octant &octant,
const OctTreeNode *node) :
ExpandedOctTreeNode(node),
m_octant(octant)
{
}

/***************************************************************************/
/***************************************************************************/
inline FullOctTreeNode FullOctTreeNode::Child(
Octant::OCTANT octant) const
{
	FullOctTreeNode child(m_octant.Child(octant), m_child[octant]);

	return child;
}

/***************************************************************************/
// Make sure we can copy
/***************************************************************************/
inline FullOctTreeNode &FullOctTreeNode::operator=(
const FullOctTreeNode &n)
{
	m_triangles = n.m_triangles;
	m_triList = n.m_triList;

	for (int o = 0; o < Octant::OCTANTS; o++)
		m_child[o] = n.m_child[o];

	m_octant = n.m_octant;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline OctTreeNodeStack::OctTreeNodeStack()
{
	m_top = 0;
}

/***************************************************************************/
/***************************************************************************/
inline void OctTreeNodeStack::Initialize(
const Octant &rootOctant,
const OctTreeNode *root)
{
	m_top = 0;
	m_stack[0] = FullOctTreeNode(rootOctant, root);
}

/***************************************************************************/
/***************************************************************************/
inline OctTreeNodeStack::OctTreeNodeStack(
const Octant &rootOctant,
const OctTreeNode *root)
{
	Initialize(rootOctant, root);
}

/***************************************************************************/
// What is the current depth?
/***************************************************************************/
inline uint OctTreeNodeStack::Depth()
{
	return m_top;
}

/***************************************************************************/
// What is the current entry
/***************************************************************************/
inline FullOctTreeNode &OctTreeNodeStack::Top()
{
	return m_stack[m_top];
}

/***************************************************************************/
// Push down one
/***************************************************************************/
inline void OctTreeNodeStack::Push(
Octant::OCTANT toChild)
{
	ASSERT(m_top < MAX_OCTTREE_DEPTH);

	m_stack[m_top + 1] = m_stack[m_top].Child(toChild);
	++m_top;
}

/***************************************************************************/
// Pop one off
/***************************************************************************/
inline void OctTreeNodeStack::Pop()
{
	ASSERT(m_top > 0);
	--m_top;
}
