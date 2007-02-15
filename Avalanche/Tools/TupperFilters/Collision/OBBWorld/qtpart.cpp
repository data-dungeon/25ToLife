////////////////////////////////////////////////////////////////////////////
// The quad tree partition factory generates a quad tree for each
// partition.
////////////////////////////////////////////////////////////////////////////
#include "Pch.h"
#include "qtpart.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
OBBQuadTreePartitionFactory::OBBQuadTreePartitionFactory(
FilterDataManager &dataManager,
float minQuadrantSize,
const Vector3 &shrinkBy,
float minDimension) :
collisionTree(dataManager, false),
instanceList(collisionTree)
{
	// Create the array of factories
	qtFact = new OBBQuadTreeFactory *[instanceList.Partitions()];
	assert(qtFact);

	// Create the array of partition lists
	partitionList = new PartitionCollisionInstanceList *[instanceList.Partitions()];
	assert(partitionList);

	// Do the processing
	for (int p = 0; p < instanceList.Partitions(); p++)
	{
	  	// Create them a partition specific list
		partitionList[p] = new PartitionCollisionInstanceList(instanceList, p);
		assert(partitionList[p]);

		// Convert the data
		qtFact[p] = new OBBQuadTreeFactory(*partitionList[p],
														minQuadrantSize, shrinkBy, minDimension);
		assert(qtFact[p]);
	}
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
OBBQuadTreePartitionFactory::~OBBQuadTreePartitionFactory(void)
{
	// Get rid of the factories
	for (int p = 0; p < instanceList.Partitions(); p++)
	{
		delete qtFact[p];
		delete partitionList[p];
	}

	delete [] qtFact;
	delete [] partitionList;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void OBBQuadTreePartitionFactory::OutputOBBDatabase(
const char *filename)
{
	// Write out all the output
	for (int p = 0; p < instanceList.Partitions(); p++)
	{
		// Make the partition filename
		char pathFile[256];
		MakeFileName(filename, p, pathFile);

		// Delete the old file
		remove(pathFile);

		// write it out
		if (qtFact[p])
			qtFact[p]->OutputOBBDatabase(pathFile);
	}
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void OBBQuadTreePartitionFactory::OutputVisData(
const char *filename)
{
	// Write out all the output
	for (int p = 0; p < instanceList.Partitions(); p++)
	{
		// Make the partition filename
		char pathFile[256];
		MakeFileName(filename, p, pathFile);

		// Delete the old file
		remove(pathFile);

		// write it out
		if (qtFact[p])
			qtFact[p]->OutputVisData(pathFile);
	}
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
char *OBBQuadTreePartitionFactory::MakeFileName(
const char *filename,
int partition,
char *realFilename)
{
	TupString path = filename;
	FilePath::AddNumberToFileName(path, partition);
	strcpy(realFilename, path);
	return(realFilename);
}
