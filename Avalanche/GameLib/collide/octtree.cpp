/***************************************************************************/
/***************************************************************************/
#include "collide/CollidePCH.h"
#include "collide/octtree.h"

// How far to we advance the ray (in meters) to the next cube
#define EXIT_EPSILON		0.005f

#ifdef OCTREELINEDEBUG
int													OctTreeLineIterator::s_trace = 0;
Octant 												OctTreeLineIterator::s_debugLeaf[OLI_MAXDEBUGOCTANTS];
int													OctTreeLineIterator::s_debugLeafColor[OLI_MAXDEBUGOCTANTS];
int													OctTreeLineIterator::s_debugLeaves;
Vector3  											OctTreeLineIterator::s_debugLine[OLI_MAXDEBUGLINES][2];
int													OctTreeLineIterator::s_debugLines;
TerrainTriangleList								OctTreeLineIterator::s_debugTriList[OLI_MAXDEBUGTRILISTS];
int 													OctTreeLineIterator::s_debugTriLists;
Vector3  											OctTreeLineIterator::s_debugPoint[OLI_MAXDEBUGPOINTS];
int													OctTreeLineIterator::s_debugPoints;
#endif

/***************************************************************************/
/***************************************************************************/
bool OctTreeAABBIterator::Initialize(
TerrainCollideLayer &layer,
CCollisionStack *stack)
{
	// Increment the test ID
	layer.NewTestID();

	// init the array
	m_triList.Initialize(stack);

	// Walk the tree and build the list...
	const Octant rootOctant = layer.RootOctant();
	if (rootOctant.Touches(m_bounds))
		Walk(rootOctant, layer.Root(), m_bounds);

	if (m_triList.Lists() == 0)
	{
		m_triList.Terminate();
		return false;
	}
	return true;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeAABBIterator::Walk(
const Octant &octant,
const OctTreeNode *node,
const AABB &aabb)
{
	// Expand the node
	ExpandedOctTreeNode expandedNode(node);

	// Add the list of triangles from this node
	if (expandedNode.m_triangles)
		m_triList.Add(expandedNode.m_triList, expandedNode.m_triangles);

	// Call all the chilren
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (expandedNode.m_child[o])
		{
			Octant childOctant = octant.Child((Octant::OCTANT)o);
			if (childOctant.Touches(aabb))
			{
				// Walk the child
				Walk(childOctant, expandedNode.m_child[o], aabb);

#if 0
				// This is dubious...may not be faster
				if (childOctant.Contains(aabb))
					return;
#endif
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
bool OctTreeOBBIterator::Initialize(
TerrainCollideLayer &layer,
CCollisionStack *stack)
{
	// Increment the test ID
	layer.NewTestID();

	// init the array
	m_triList.Initialize(stack);

	// Walk the tree and build the list...
	const Octant rootOctant = layer.RootOctant();
	if (rootOctant.Touches(m_aabb))
	{
		if (!m_obb)
			WalkAABB(rootOctant, layer.Root());
		else
			WalkOBB(rootOctant, layer.Root());
	}

	if (m_triList.Lists() == 0)
	{
		m_triList.Terminate();
		return false;
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeOBBIterator::WalkAABB(
const Octant &octant,
const OctTreeNode *node)
{
	// Expand the node
	ExpandedOctTreeNode expandedNode(node);

	// Add the list of triangles from this node
	if (expandedNode.m_triangles)
		m_triList.Add(expandedNode.m_triList, expandedNode.m_triangles);

	// Call all the chilren
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (expandedNode.m_child[o])
		{
			Octant childOctant = octant.Child((Octant::OCTANT)o);
			if (childOctant.Touches(m_aabb))
			{
				// Walk the child
				WalkAABB(childOctant, expandedNode.m_child[o]);

				// This is dubious...may not be faster
				if (childOctant.Contains(m_aabb))
					return;
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void OctTreeOBBIterator::WalkOBB(
const Octant &octant,
const OctTreeNode *node)
{
	// Expand the node
	ExpandedOctTreeNode expandedNode(node);

	// Add the list of triangles from this node
	if (expandedNode.m_triangles)
		m_triList.Add(expandedNode.m_triList, expandedNode.m_triangles);

	// Call all the chilren
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (expandedNode.m_child[o])
		{
			Octant childOctant = octant.Child((Octant::OCTANT)o);
			if (childOctant.Touches(m_aabb) && childOctant.Touches(*m_obb))
			{
				// Walk the child
				WalkOBB(childOctant, expandedNode.m_child[o]);

#if 0
				// This is dubious...may not be faster
				if (childOctant.Contains(m_corner, 8))
					return;
#endif
			}
		}
	}
}

 /***************************************************************************/
/***************************************************************************/
bool OctTreeLineIterator::Initialize(
TerrainCollideLayer &layer,
CCollisionStack *stack)
{
	// Save off the tree
	m_rootOctant = layer.RootOctant();
	m_root = layer.Root();
	m_leafDepth = layer.LeafDepth();

	// Save off the stack
	m_stack = stack;

	// They are retarded sometimes
	if ((m_end - m_start).LengthSquared() < Math::Epsilon)
		return false;

	// Clip the segment
	if (!m_rootOctant.Clip(m_start, m_end, m_clipStart, m_clipEnd))
		return false;
	ASSERT(m_rootOctant.Inside(m_clipStart));
	ASSERT(m_rootOctant.Inside(m_clipEnd));

	// Setup the semisegment
	m_semiSeg.SetEndPoints(m_clipStart, m_clipEnd);

	// Setup our current "voxel"
	m_leafSize = m_rootOctant.SizeAtDepth(m_leafDepth);
	m_currentCenter = m_rootOctant.CenterAtDepth(m_leafDepth, m_clipStart);

#ifdef OCTREELINEDEBUG
	DebugAddLine(m_clipStart, m_clipEnd);
	m_onLeaf = 0;
#endif

	// Don't stop
	m_haveEndPoint = false;
	m_doneWalk = false;
	m_leafStack.Initialize(m_rootOctant, m_root);

	// Setup the test ID
	layer.NewTestID();

	// Start the walk
	if (NextLeaf() == 0)
	{
		m_triList.Terminate();
		return false;
	}
	return true;
}

/***************************************************************************/
/***************************************************************************/
TerrainTriangle *OctTreeLineIterator::GetNextTriangle(
u32 testID)
{
	// Do we have a triagle?
	TerrainTriangle *tri = m_triList.GetNextTriangle(testID);
	while (!tri)
	{
		// Did they stop the walk or are we done?
		if (HaveValidEnd() || m_doneWalk)
			return NULL;

		// Try to walk to the next leaf
		NextLeaf();
		tri = m_triList.GetNextTriangle(testID);
	}

	return tri;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::FoundEnd(
const Vector3 &intersection)
{
	// Only stop if this intersection was inside the current leaf
	m_haveEndPoint = true;
	m_endPoint = intersection;

#ifdef OCTREELINEDEBUG
	DebugAddPoint(m_endPoint);
#endif
}

/***************************************************************************/
// Specialied walk
/***************************************************************************/
int OctTreeLineIterator::NextLeaf()
{
	// Reset the iteration
	m_triList.Initialize(m_stack);

	// Try to fill up with the next leaf that has tri's
	while (!m_doneWalk && m_triList.Lists() == 0)
	{
		// Get the left octant where this point starts
		FullOctTreeNode *leaf = LeafNode(m_currentCenter);

		// We have walked outside the tree for some reason
		if (!leaf)
		{
			m_doneWalk = true;
			return 0;
		}

#ifdef OCTREELINEDEBUG
		// Debugging!!!
		int color;
		if (m_onLeaf == 0)
			color	= DrawUtility::GREEN;
		else if (leaf->m_octant.Inside(m_clipEnd))
			color	= DrawUtility::RED;
		else
			color	= DrawUtility::CYAN;
		DebugAddLeaf(leaf->m_octant, color);
#endif

		// Now we have the start add everything there
		AddAllTriangles(*leaf);

		// Where does the ray exit the cube?
		Cube::Face exit = leaf->m_octant.RayFaceExit(m_clipStart, m_clipEnd);
		UpdateCenter(exit);

		// Are we done?
		m_doneWalk = leaf->m_octant.Inside(m_clipEnd);

#ifdef OCTREELINEDEBUG
		++m_onLeaf;
#endif
	}

	return m_triList.Lists();
}

/***************************************************************************/
// Drill down until we find the leaf with the point in it
/***************************************************************************/
FullOctTreeNode *OctTreeLineIterator::LeafNode(
Vector3CRef point)
{
	// Start by walking up
	while (!m_leafStack.Top().m_octant.Inside(point))
	{
		// Break at the root
		if (m_leafStack.Depth() == 0)
			break;

		// Keep walking up
 		m_leafStack.Pop();
	}

	// The point is outside the world
	if (!m_leafStack.Top().m_octant.Inside(point))
		return NULL;

	// Make sure we aren't too deep
	while (m_leafStack.Depth() < m_leafDepth)
	{
		// What is the best child?
		Octant::OCTANT best = m_leafStack.Top().m_octant.BestChild(point);

		// Give them back the child
		m_leafStack.Push(best);
	}

	// Give them back the current leaf
	return &m_leafStack.Top();
}

/***************************************************************************/
// Add all the triangles in this node and all the children
/***************************************************************************/
/***************************************************************************/
// The fast way, but more tri's
/***************************************************************************/
void OctTreeLineIterator::AddAllTriangles(
const ExpandedOctTreeNode &node)
{
	// Add the list of triangles from this node
	if (node.m_triangles)
	{
		m_triList.Add(node.m_triList, node.m_triangles);
#ifdef OCTREELINEDEBUG
		DebugAddTriangleList(node.m_triList, node.m_triangles);
#endif
	}

	// Call all the children
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (node.m_child[o])
		{
			ExpandedOctTreeNode child(node.m_child[o]);
			AddAllTriangles(child);
		}
	}
}

#ifdef OCTREELINEDEBUG
#include "EngineHelper/drawutil.h"

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::Draw(
bool showOctants,
bool showTris)
{
	// Draw the lines
	int l;
	for (l = 0; l < s_debugLines; l++)
		DrawUtility::Line(s_debugLine[l][0], s_debugLine[l][1], DrawUtility::WHITE);

	// Draw the octants
	if (showOctants)
	{
		for (l = 0; l < s_debugLeaves; l++)
			DrawUtility::Box(s_debugLeaf[l].Min(), s_debugLeaf[l].Max(), NULL, NULL, (DrawUtility::COLOR)s_debugLeafColor[l]);
	}

	// Show the triangles
	if (showTris)
	{
		for (l = 0; l < s_debugTriLists; l++)
		{
			TerrainTriangleList &list = s_debugTriList[l];
			for (int t = 0; t < list.count; t++)
			{
				TerrainTriangle &tri = *list.tri[t];

				DrawUtility::Line(tri.A(), tri.B(), DrawUtility::RED);
				DrawUtility::Line(tri.B(), tri.C(), DrawUtility::RED);
				DrawUtility::Line(tri.C(), tri.A(), DrawUtility::RED);
			}
		}
	}

	for (l = 0; l < s_debugPoints; l++)
		DrawUtility::Point(s_debugPoint[l]);
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::ClearTrace()
{
	s_debugLeaves = 0;
	s_debugLines = 0;
	s_debugTriLists = 0;
	s_debugPoints = 0;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::StartTrace()
{
	++s_trace;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::EndTrace()
{
	--s_trace;
	ASSERT(s_trace >= 0);
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::DebugAddLine(
const Vector3 &start,
const Vector3 &end)
{
	if (s_trace == 0)
		return;

	// Doh
	//ASSERT(s_debugLines < OLI_MAXDEBUGLINES);
	if (s_debugLines == OLI_MAXDEBUGLINES)
		return;

	// Add it in
	s_debugLine[s_debugLines][0] = start;
	s_debugLine[s_debugLines][1] = end;
	++s_debugLines;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::DebugAddLeaf(
const Octant &leaf,
int color)
{
	if (s_trace == 0)
		return;

	// Doh
	//ASSERT(s_debugLeaves < OLI_MAXDEBUGOCTANTS);
	if (s_debugLeaves == OLI_MAXDEBUGOCTANTS)
		return;

	// Add it in
	s_debugLeaf[s_debugLeaves] = leaf;
	s_debugLeafColor[s_debugLeaves] = color;
	++s_debugLeaves;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::DebugAddTriangleList(
TerrainTriangle **triList,
int triangles)
{
	if (s_trace == 0)
		return;

	// Doh
	//ASSERT(s_debugTriLists < OLI_MAXDEBUGTRILISTS);
	if (s_debugTriLists  == OLI_MAXDEBUGTRILISTS)
		return;

	// Be anal
	ASSERT(triList != NULL);
	ASSERT(triangles > 0);
	for (int t = 0; t < triangles; t++)
	{
		ASSERT(triList[t] != NULL);
	}

	// Add it in
	s_debugTriList[s_debugTriLists].tri = triList;
	s_debugTriList[s_debugTriLists].count = triangles;
	++s_debugTriLists;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineIterator::DebugAddPoint(
const Vector3 &p)
{
	if (s_trace == 0)
		return;

	// Doh
	//ASSERT(s_debugLines < OLI_MAXDEBUGLINES);
	if (s_debugLines == OLI_MAXDEBUGPOINTS)
		return;

	// Add it in
	s_debugPoint[s_debugPoints++] = p;
}

#endif

/***************************************************************************/
/***************************************************************************/
bool OctTreeLineExactIterator::Initialize(
TerrainCollideLayer &layer,
CCollisionStack *stack)
{
	// Save off the tree
	m_rootOctant = layer.RootOctant();
	m_root = layer.Root();
	m_leafDepth = layer.LeafDepth();

	// Save off the stack
	m_stack = stack;

	// They are retarded sometimes
	if ((m_end - m_start).LengthSquared() < Math::Epsilon)
		return false;

	// Clip the segment
	if (!m_rootOctant.Clip(m_start, m_end, m_clipStart, m_clipEnd))
		return false;
	ASSERT(m_rootOctant.Inside(m_clipStart));
	ASSERT(m_rootOctant.Inside(m_clipEnd));

	// Setup the semisegment
	m_semiSeg.SetEndPoints(m_clipStart, m_clipEnd);

	// Setup our current "voxel"
	m_leafSize = m_rootOctant.SizeAtDepth(m_leafDepth);
	m_currentCenter = m_rootOctant.CenterAtDepth(m_leafDepth, m_clipStart);

	// Don't stop
	m_haveEndPoint = false;
	m_doneWalk = false;
	m_leafStack.Initialize(m_rootOctant, m_root);

	// Setup the test ID
	layer.NewTestID();

	// Start the walk
	if (NextLeaf() == 0)
	{
		m_triList.Terminate();
		return false;
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
TerrainTriangle *OctTreeLineExactIterator::GetNextTriangle(
u32 testID)
{
	// Do we have a triagle?
	TerrainTriangle *tri = m_triList.GetNextTriangle(testID);
	while (!tri)
	{
		// Did they stop the walk or are we done?
		if (HaveValidEnd() || m_doneWalk)
			return NULL;

		// Try to walk to the next leaf
		NextLeaf();
		tri = m_triList.GetNextTriangle(testID);
	}

	return tri;
}

/***************************************************************************/
/***************************************************************************/
void OctTreeLineExactIterator::FoundEnd(
const Vector3 &intersection)
{
	// Only stop if this intersection was inside the current leaf
	m_haveEndPoint = true;
	m_endPoint = intersection;
}

/***************************************************************************/
// Specialied walk
/***************************************************************************/
int OctTreeLineExactIterator::NextLeaf()
{
	// Reset the iteration
	m_triList.Initialize(m_stack);

	// Try to fill up with the next leaf that has tri's
	while (!m_doneWalk && m_triList.Lists() == 0)
	{
		// Get the left octant where this point starts
		FullOctTreeNode *leaf = LeafNode(m_currentCenter);

		// We have walked outside the tree for some reason
		if (!leaf)
		{
			m_doneWalk = true;
			return 0;
		}

		// Now we have the start add everything there
		AddAllTriangles(*leaf);

		// Where does the ray exit the cube?
		Cube::Face exit = leaf->m_octant.RayFaceExit(m_clipStart, m_clipEnd);
		UpdateCenter(exit);

		// Are we done?
		m_doneWalk = leaf->m_octant.Inside(m_clipEnd);
	}

	return m_triList.Lists();
}

/***************************************************************************/
// Drill down until we find the leaf with the point in it
/***************************************************************************/
FullOctTreeNode *OctTreeLineExactIterator::LeafNode(
Vector3CRef point)
{
	// Start by walking up
	while (!m_leafStack.Top().m_octant.Inside(point))
	{
		// Break at the root
		if (m_leafStack.Depth() == 0)
			break;

		// Keep walking up
 		m_leafStack.Pop();
	}

	// The point is outside the world
	if (!m_leafStack.Top().m_octant.Inside(point))
		return NULL;

	// Make sure we aren't too deep
	while (m_leafStack.Depth() < m_leafDepth)
	{
		// What is the best child?
		Octant::OCTANT best = m_leafStack.Top().m_octant.BestChild(point);

		// Give them back the child
		m_leafStack.Push(best);
	}

	// Give them back the current leaf
	return &m_leafStack.Top();
}

/***************************************************************************/
// Add all the triangles in this node and all the children
/***************************************************************************/
void OctTreeLineExactIterator::AddAllTriangles(
const FullOctTreeNode &node)
{
	// Add the list of triangles from this node
	if (node.m_triangles)
		m_triList.Add(node.m_triList, node.m_triangles);

	// Call all the children
	for (int o = 0; o < Octant::OCTANTS; o++)
	{
		if (node.m_child[o])
		{
			// Expand out the child
			FullOctTreeNode child = node.Child((Octant::OCTANT)o);

			// This could be dubious
			if (child.m_octant.Touches(m_semiSeg))
				AddAllTriangles(child);
		}
	}
}

