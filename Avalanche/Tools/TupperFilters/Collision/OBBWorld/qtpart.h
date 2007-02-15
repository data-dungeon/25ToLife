////////////////////////////////////////////////////////////////////////////
// The quad tree partition factory generates a quad tree for each
// partition.
////////////////////////////////////////////////////////////////////////////
#ifndef QTPART_H
#define QTPART_H

#include "collinst.h"
#include "qtfact.h"

class OBBQuadTreePartitionFactory
{
public:
	OBBQuadTreePartitionFactory(FilterDataManager &filterDataManager,
											float minQuadrantSize,
											const Vector3 &shrinkBy,
											float minDimension);
	~OBBQuadTreePartitionFactory(void);

	void OutputOBBDatabase(const char *filename);
	void OutputVisData(const char *filename);

	int TotalInstances(void)
		{ return(instanceList.TotalInstances()); }

private:
	// The tupperware stuff
	TupperwareCollisionTree	collisionTree;
	CollisionInstanceList	instanceList;

	// The array of partition lists
	PartitionCollisionInstanceList **partitionList;

	// The array of obb tree factories
	OBBQuadTreeFactory		**qtFact;

	// Make a partition specific filename
	char *MakeFileName(const char *filename, int partition, char *realFilename);
};

#endif
