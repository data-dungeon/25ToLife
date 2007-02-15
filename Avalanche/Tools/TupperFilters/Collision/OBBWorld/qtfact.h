////////////////////////////////////////////////////////////////////////////
//
// qtfact
//
// A class for creating a loadable quadtree of obb's
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: qtfact.h $
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * update for streaming system
 * 
 * *****************  Version 18  *****************
 * User: Food         Date: 1/14/03    Time: 4:04p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 *
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 11:33a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * new collision attribute system (using image names)
 *
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Added support for attrib tree
 *
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 */

#ifndef QTFACT_H
#define QTFACT_H

#include "collinst.h"

class OBBQuadTreeFactoryNode;

#define MAX_COLLISION_LAYERS	32

class OBBQuadTreeFactory
{
public:
	OBBQuadTreeFactory(PartitionCollisionInstanceList &instanceList,float minQuadrantSize, const Vector3 &shrinkBy, float minDimension);
	~OBBQuadTreeFactory(void);

	void OutputOBBDatabase(const char *filename);
	void OutputVisData(const char *filename);

private:
	// The main tree data
	PartitionCollisionInstanceList &m_instanceList;

	// used to split up the world per layer
	OBBQuadTreeFactoryNode	*m_pRoot[MAX_COLLISION_LAYERS];	// Our root node

	// Convert a normal file name to have a layer prefix (0 does nothing)
	// Returns a pointer to realFileName.
	char *LayerizeFilename(const char *filename, int layer, char *realFilename);
};

#endif
