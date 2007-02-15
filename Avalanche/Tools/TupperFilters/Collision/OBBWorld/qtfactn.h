/***************************************************************************/
// A class for creating a loadable quadtree of obb's
//
// Quadtrees reside in the XZ plane.
// Quadrants have the following nomenclature:
//                | +z
//		     2  		|      1
//						|
//		------------+------------ +x
//						|
//		     3     	|      4
//						|
//
/***************************************************************************/
#ifndef QTFACTN_H
#define QTFACTN_H

#include <stdio.h>
#include <container\dynlist.h>
#include <collide\ObjectQT.h>

// The vis tool
class VisTool;

class OBBQuadTreeFactoryNode
{
public:
	// Create the root node of the quad tree
	static OBBQuadTreeFactoryNode *CreateRoot(const XZRect &size);
	~OBBQuadTreeFactoryNode(void);

	// set the minimum quadrant size
	static void SetMinQuadrantSize(float squareMeters)
		{ minQuadrantSize = squareMeters; }

	// What is our size?
	const XZRect &Rect(void) const
		{ return(rect); }

	// Return the the child
	OBBQuadTreeFactoryNode *Child(QUADTREECHILD which) const
		{ return(child[which]); }

	// What would the rect of the child be?
	XZRect ChildRect(QUADTREECHILD which);

	// Create a child (pass the rect if you know it, otherwise
	// return NULL
	OBBQuadTreeFactoryNode *MakeChild(QUADTREECHILD which, const XZRect *rect = NULL);

	// Add an obb instance to the list
	bool Add(OBBStaticInstance *obb, XZRect &obbRect);

	// How many obbs are in this node?
	inline uint OBBs(void)
		{ return(obbList.Count()); }

	// Write out the data
	bool Write(FILE *fh);

	// how big will we be on disk?
	uint DiskSize(void);

	// Vis report
	void Report(VisTool &vis);

private:
	// To create it you have to specify size
	OBBQuadTreeFactoryNode(const XZRect &size, uint leafDepth = 1);

	// Our defining rect
	XZRect 		rect;

	// our depth in the tree
	uint			depth;

	// The four children
	OBBQuadTreeFactoryNode *child[QUADTREECHILDREN];

	// Our list of OBB instances
	DynList<OBBStaticInstance>		obbList;

	// What is the best quadrant to try to add too...
	QUADTREECHILD BestQuadrant(XZRect &testRect);

	// Add the obb to this instance
	bool AddChild(QUADTREECHILD which, OBBStaticInstance *obb, XZRect &obbRect);

	// how big will the node will we be on disk?
	uint DiskSize(QUADTREECHILD which);

	// How small do we want quadrants to be?
	static float minQuadrantSize;
};

#endif
