////////////////////////////////////////////////////////////////////////////
//
// qtfactn
//
// A class for creating a loadable quadtree of obb's
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: qtfactn.cpp $
 * 
 * *****************  Version 19  *****************
 * User: Bcollard     Date: 7/11/03    Time: 1:34p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Remove NEW_COLLISIONS #ifdef's.
 * 
 * *****************  Version 18  *****************
 * User: Bcollard     Date: 3/28/03    Time: 2:44p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Integrated the Phase II collision system changes but enclosed them in
 * #ifdef's.
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * hybrid configuration
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:06p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Memory Problems.
 * Simple Progress Bar Stuff
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 */

#include "Pch.h"

#include "qtfactn.h"

// How small do we want quadrants to be?
float OBBQuadTreeFactoryNode::minQuadrantSize = 0.25f;

#define LOOSEQUADTREE

/***************************************************************************/
// Create the root of the tree
/***************************************************************************/
OBBQuadTreeFactoryNode *OBBQuadTreeFactoryNode::CreateRoot(const XZRect &size)
{
	// Make it square
	XZRect realSize = size;
	realSize.MakeSquare();

#ifdef LOOSEQUADTREE
	// Inflate the size of the root node
	realSize.Inflate(realSize.Width() * 0.5f, realSize.Height() * 0.5f);
#endif

	// Create the node
	return(new OBBQuadTreeFactoryNode(realSize, 1));
}

/***************************************************************************/
// To create it you have to specify size
/***************************************************************************/
OBBQuadTreeFactoryNode::OBBQuadTreeFactoryNode(const XZRect &size,uint leafDepth)
{
	// One more leaf
	++g_stats->quadLeaves;

	rect = size;
	depth = leafDepth;

	if (depth > g_stats->maxDepth)
		++g_stats->maxDepth;

	child[QUADTREECHILD_NE] = NULL;
	child[QUADTREECHILD_NW] = NULL;
	child[QUADTREECHILD_SW] = NULL;
	child[QUADTREECHILD_SE] = NULL;
}

/***************************************************************************/
/***************************************************************************/
OBBQuadTreeFactoryNode::~OBBQuadTreeFactoryNode(void)
{
	delete child[QUADTREECHILD_NE];
	delete child[QUADTREECHILD_NW];
	delete child[QUADTREECHILD_SW];
	delete child[QUADTREECHILD_SE];
}

/***************************************************************************/
// What would the rect of the child be?
/***************************************************************************/
XZRect OBBQuadTreeFactoryNode::ChildRect(QUADTREECHILD which)
{
	XZRect childRect;

#ifdef LOOSEQUADTREE
	float childSize = rect.Width() * 0.5f;
	float centerX, centerZ;

	switch (which)
	{
		case QUADTREECHILD_NE:
			centerX = rect.CenterX() + childSize * 0.25f;
			centerZ = rect.CenterZ() + childSize * 0.25f;
			break;
		case QUADTREECHILD_NW:
			centerX = rect.CenterX() - childSize * 0.25f;
			centerZ = rect.CenterZ() + childSize * 0.25f;
			break;
		case QUADTREECHILD_SW:
			centerX = rect.CenterX() - childSize * 0.25f;
			centerZ = rect.CenterZ() - childSize * 0.25f;
			break;
		case QUADTREECHILD_SE:
			centerX = rect.CenterX() + childSize * 0.25f;
			centerZ = rect.CenterZ() - childSize * 0.25f;
			break;
	}

	float childHalf = childSize * 0.5f;
	childRect.Set(centerX - childHalf, centerZ - childHalf, centerX + childHalf, centerZ + childHalf);

#else
	float centerx = rect.CenterX();
	float centerz = rect.CenterZ();

	switch (which)
	{
		case QUADTREECHILD_NE:
			childRect.Set(centerx, centerz, rect.MaxX(), rect.MaxZ());
			break;
		case QUADTREECHILD_NW:
			childRect.Set(rect.MinX(), centerz, centerx, rect.MaxZ());
			break;
		case QUADTREECHILD_SW:
			childRect.Set(rect.MinX(), rect.MinZ(), centerx, centerz);
			break;
		case QUADTREECHILD_SE:
			childRect.Set(centerx, rect.MinZ(), rect.MaxX(), centerz);
			break;
	}
#endif

	return(childRect);
}

/***************************************************************************/
// Create a child (pass the rect if you know it, otherwise
// return NULL
/***************************************************************************/
OBBQuadTreeFactoryNode *OBBQuadTreeFactoryNode::MakeChild(QUADTREECHILD which,const XZRect *rect)
{
	// Sanity check
	if (child[which] != NULL)
		return(child[which]);

	// Did they pass a rect?
	XZRect temp;
	if (!rect)
	{
		temp = ChildRect(which);
		rect = &temp;
	}

	// Is this node going to be too small?
	if (rect->Area() < minQuadrantSize)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_YELLOW|LogFlags::FONT_STYLE_BOLD,"WARNING: Bumped leaf node size limit");
		return(NULL);
	}

	// Make the new node
	child[which] = new OBBQuadTreeFactoryNode(*rect, depth + 1);

	// return the new node
	return(child[which]);
}

/***************************************************************************/
// Add an obb instance to the list
/***************************************************************************/
bool OBBQuadTreeFactoryNode::Add(OBBStaticInstance *obb,XZRect &obbRect)
{
#if 0
	// Try all the children first
	if (AddChild(QUADTREECHILD_NE, obb, obbRect) ||
			AddChild(QUADTREECHILD_NW, obb, obbRect) ||
			AddChild(QUADTREECHILD_SW, obb, obbRect) ||
			AddChild(QUADTREECHILD_SE, obb, obbRect))
		return(true);
#else
	// Try the "best" one first
	QUADTREECHILD best = BestQuadrant(obbRect);
	if (AddChild(best, obb, obbRect))
		return(true);

	// try the others
	if (best != QUADTREECHILD_NE && AddChild(QUADTREECHILD_NE, obb, obbRect))
		return(true);
	if (best != QUADTREECHILD_NW && AddChild(QUADTREECHILD_NW, obb, obbRect))
		return(true);
	if (best != QUADTREECHILD_SW && AddChild(QUADTREECHILD_SW, obb, obbRect))
		return(true);
	if (best != QUADTREECHILD_SE && AddChild(QUADTREECHILD_SE, obb, obbRect))
		return(true);
#endif

	// Do we already have a match?
	uint i, count = obbList.Count();
	for (i = 0; i < count; i++)
	{
		OBB *newOBB = obb;
		OBB	*o = obbList.Get(i);

		if (*newOBB == *o)
		{
			++g_stats->redundantObbs;
			return(true);
		}
	}

	// Didn't fit in the children, have to add it here
	++g_stats->obbs;
	return(obbList.Add(obb));
}

/***************************************************************************/
// Write out the data
/***************************************************************************/
bool OBBQuadTreeFactoryNode::Write(FILE *fh)
{
	assert(fh);

	// Setup the node
	ObjectQuadTreeNode node;
	memset(&node, '\0', sizeof(node));
	node.rect = rect;
	node.objects = obbList.Count();
	if (node.objects)
		node.objectList = (CStaticCollisionObject *)(sizeof(ObjectQuadTreeNode));

	// setup each of the child pointers
	if (child[QUADTREECHILD_NE])
		node.child[QUADTREECHILD_NE] = (ObjectQuadTreeNode *)(sizeof(ObjectQuadTreeNode) +
													node.objects * sizeof(CStaticCollisionObject));

	if (child[QUADTREECHILD_NW])
		node.child[QUADTREECHILD_NW] = (ObjectQuadTreeNode *)(sizeof(ObjectQuadTreeNode) +
												node.objects * sizeof(CStaticCollisionObject) +
												DiskSize(QUADTREECHILD_NE));

	if (child[QUADTREECHILD_SW])
		node.child[QUADTREECHILD_SW] = (ObjectQuadTreeNode *)(sizeof(ObjectQuadTreeNode) +
												node.objects * sizeof(CStaticCollisionObject) +
												DiskSize(QUADTREECHILD_NE) +
												DiskSize(QUADTREECHILD_NW));
	if (child[QUADTREECHILD_SE])
		node.child[QUADTREECHILD_SE] = (ObjectQuadTreeNode *)(sizeof(ObjectQuadTreeNode) +
												node.objects * sizeof(CStaticCollisionObject) +
												DiskSize(QUADTREECHILD_NE) +
												DiskSize(QUADTREECHILD_NW) +
												DiskSize(QUADTREECHILD_SW));

	// Write the node
	if (fwrite(&node, sizeof(node), 1, fh) != 1)
		return(false);

	// Now write the obb table (if there are obbs)
	for (uint i = 0; i < node.objects; i++)
	{
		OBBStaticInstance *obb = obbList.Get(i);
		if (fwrite(obb, sizeof(OBBStaticInstance), 1, fh) != 1)
			return(false);
	}

	// now write the children
	if (child[QUADTREECHILD_NE] && !child[QUADTREECHILD_NE]->Write(fh))
		return(false);
	if (child[QUADTREECHILD_NW] && !child[QUADTREECHILD_NW]->Write(fh))
		return(false);
	if (child[QUADTREECHILD_SW] && !child[QUADTREECHILD_SW]->Write(fh))
		return(false);
	if (child[QUADTREECHILD_SE] && !child[QUADTREECHILD_SE]->Write(fh))
		return(false);

	// We made it!
	return(true);
}

/***************************************************************************/
// how big will we be on disk?
/***************************************************************************/
uint OBBQuadTreeFactoryNode::DiskSize(void)
{
	return(sizeof(ObjectQuadTreeNode) +
				obbList.Count() * sizeof(OBBStaticInstance) +
				DiskSize(QUADTREECHILD_NE) +
				DiskSize(QUADTREECHILD_NW) +
				DiskSize(QUADTREECHILD_SW) +
				DiskSize(QUADTREECHILD_SE));
}

/***************************************************************************/
// Vis report
/***************************************************************************/
void OBBQuadTreeFactoryNode::Report(
VisTool &vis)
{
#ifndef LOOSEQUADTREE
	// Draw our rect
	vis.Plot(rect, VisGREEN);
#else
	// Show the corrected rect
	XZRect c = rect;
	c.Inflate(c.Width() * -0.25f, c.Height() * -0.25f);
	vis.Plot(c, VisGREEN);
#endif

	// Draw our obb's
	for (int o = 0; o < obbList.Count(); o++)
	{
		OBBStaticInstance *obb = obbList.Get(o);
		vis.Plot(*obb, VisBLUE);
	}

	// Draw our children
	if (child[QUADTREECHILD_NE])
		child[QUADTREECHILD_NE]->Report(vis);
	if (child[QUADTREECHILD_NW])
		child[QUADTREECHILD_NW]->Report(vis);
	if (child[QUADTREECHILD_SW])
		child[QUADTREECHILD_SW]->Report(vis);
	if (child[QUADTREECHILD_SE])
		child[QUADTREECHILD_SE]->Report(vis);
}

/***************************************************************************/
// how big will the node will we be on disk?
/***************************************************************************/
uint OBBQuadTreeFactoryNode::DiskSize(
QUADTREECHILD which)
{
	if (child[which] == NULL)
		return(0);
	else
		return(child[which]->DiskSize());
}

/***************************************************************************/
// What is the best quadrant to try to add too...
/***************************************************************************/
QUADTREECHILD OBBQuadTreeFactoryNode::BestQuadrant(
XZRect &testRect)
{
	float myX = rect.CenterX();
	float myZ = rect.CenterZ();
	float testX = testRect.CenterX();
	float testZ = testRect.CenterZ();

	if (testX > myX)
	{
		if (testZ > myZ)
			return(QUADTREECHILD_NE);
		else
			return(QUADTREECHILD_SE);
	}
	else
	{
		if (testZ > myZ)
			return(QUADTREECHILD_NW);
		else
			return(QUADTREECHILD_SW);
	}
}

/***************************************************************************/
// Add an obb instance to the list
/***************************************************************************/
bool OBBQuadTreeFactoryNode::AddChild(
QUADTREECHILD which,
OBBStaticInstance *obb,
XZRect &obbRect)
{
	// Does the obbrect fit into the child rect?
	XZRect childRect = ChildRect(which);
	if (!childRect.Contains(obbRect))
		return(false);

	// Get the child in question
	OBBQuadTreeFactoryNode *addTo = MakeChild(which, &childRect);
	if (!addTo)
		return(false);

	// Pass it down to the child for it's test
	return(addTo->Add(obb, obbRect));
}

