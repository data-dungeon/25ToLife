//***************************************************************************
// This class maintains all the information about the collision instances
// in a tupperware list
//***************************************************************************
#include "pch.h"
#include <Math/Vector.h>
#include <Math/Matrix.h>
#include "collinst.h"

//***************************************************************************
//***************************************************************************
TupperwareCollisionTree::TupperwareCollisionTree(
FilterDataManager &filterDataManager,
bool terrainMode) :
dataManager(filterDataManager)
{
	// Make sure everything is NULL
	scene = NULL;

	nodePool = NULL;
	objectPool = NULL;
	modifierPool = NULL;
	materialPool = NULL;
	mapPool = NULL;
	imagePool = NULL;

	faceCollisionManager = NULL;

	// This is all copied from Adam's stuff
	// I really have no idea what is going on
	TupperwareAggregate *sceneAgg = dataManager.GetSceneRoot();
	assert(sceneAgg);
	scene = new TupScene(sceneAgg);
	assert(scene);

	TupperwareAggregate *configAttributeTreeAgg = scene->GetNodeAttributeTree();
	if (!configAttributeTreeAgg)
		throw(PluginException(ERROR_FATAL, "Node Attribute Tree Does not Exist!"));
	nodeTreeRoot.BuildConfigTree(configAttributeTreeAgg);

	TupperwareAggregate *faceAttribTreeAgg = scene->GetFaceAttributeTree();
	if (!faceAttribTreeAgg)
		throw(PluginException(ERROR_FATAL, "Face Attribute Tree Not Found"));
	faceTreeRoot.BuildConfigTree(faceAttribTreeAgg);

	TupperwareAggregate *nodePoolAgg = scene->GetNodePool();
	assert(nodePoolAgg);
	nodePool = new TupNodePool(nodePoolAgg);
	assert(nodePool);

	TupperwareAggregate *objectPoolAgg = scene->GetObjectPool();
	assert(objectPoolAgg);
	objectPool = new TupObjectPool(objectPoolAgg);
	assert(objectPool);

	TupperwareAggregate *modifierPoolAgg = scene->GetModifierPool();
	assert(modifierPoolAgg);
	modifierPool = new TupModifierPool(modifierPoolAgg);
	assert(modifierPool);

	TupperwareAggregate *materialPoolAgg = scene->GetMaterialPool();
	assert(materialPoolAgg);
	materialPool = new TupMaterialPool(materialPoolAgg);
	assert(materialPool);

	TupperwareAggregate *mapPoolAgg = scene->GetMapPool();
	assert(mapPoolAgg);
	mapPool = new TupMapPool(mapPoolAgg);
	assert(mapPool);

	TupperwareAggregate *imagePoolAgg = scene->GetImagePool();
	assert(imagePoolAgg);
	imagePool = new TupImagePool(imagePoolAgg);
	assert(imagePool);

	// Finaly create this guy
	faceCollisionManager = new FaceCollisionManager(nodeTreeRoot, faceTreeRoot,
						 											*objectPool, *modifierPool,
																	*materialPool, *mapPool,
																	*imagePool, terrainMode);
	assert(faceCollisionManager);
}

//***************************************************************************
//***************************************************************************
TupperwareCollisionTree::~TupperwareCollisionTree(void)
{
	delete faceCollisionManager;

	delete imagePool;
	delete mapPool;
	delete materialPool;
	delete modifierPool;
	delete objectPool;
	delete nodePool;

	delete scene;
}

//***************************************************************************
//***************************************************************************
CollisionInstanceList::CollisionInstanceList(
TupperwareCollisionTree &tupCollisionTree) :
collisionTree(tupCollisionTree)
{
	// Clear everything
	instanceList = NULL;
	partitions = 0;
	totalInstances = 0;

	// How many arrays do we have?
	TupArray<int> counts;
	collisionTree.scene->GetBaseAggregate()->FindListByKey("Annotate_TotalCollisionInstances")->GetAsInt(counts);

	// That is the number of partitions we have
	partitions = counts.GetSize();
	if (partitions <= 0)
		throw(PluginException(ERROR_FATAL, "Invalid partition count %d", partitions));

	// see if we have any instances in the other visibility sets
	bool bAnyPartitionCollisions=false;
	for (int setIndex=1;setIndex<partitions;setIndex++)
	{
		if (counts[setIndex])
		{
			bAnyPartitionCollisions=true;
			break;
		}
	}
	// set total partitions to 1
	if (!bAnyPartitionCollisions)
		partitions = 1;

	// Create the list of arrays
	instanceList = new InstanceList[partitions];
	for (int p = 0; p < partitions; p++)
	{
		instanceList[p].count = 0;
		if (counts[p])
			instanceList[p].instance = new CollisionInstance *[counts[p]];
		else
			instanceList[p].instance = NULL;
	}

	int totalNodes = collisionTree.nodePool->GetNumNodes();
	int instanceCount = 0;
	for (int i = 0; i < totalNodes; i++)
	{
		// GEt the node
		TupperwareAggregate *nodeAgg = collisionTree.nodePool->GetNode(i);
		if (!nodeAgg)
			continue;

		// Is this an instance we care about?
		TupperwareScalar *instanceIndexScalar = nodeAgg->FindScalarByKey("Annotate_CollisionInstanceIndex");
		if (!instanceIndexScalar)
			continue;

		// What partition is the node in
		TupperwareScalar *partitionScalar = nodeAgg->FindScalarByKey("Annotate_CollisionPartitionIndex");
		int partition = (partitionScalar ? partitionScalar->GetAsInt() : 0);

		// Create a record for this instance
		CollisionInstance *instance = new CollisionInstance(collisionTree.nodeTreeRoot, *nodeAgg);
		assert(instance);

		// Add it to the list
		assert(instanceList[partition].count < counts[partition]);
		instanceList[partition].instance[instanceList[partition].count++] = instance;
	}

	// sum!
	for (p = 0; p < partitions; p++)
	{
		assert(counts[p] == instanceList[p].count);
		totalInstances += instanceList[p].count;
	}
}

//***************************************************************************
//***************************************************************************
CollisionInstanceList::~CollisionInstanceList(void)
{
	// delete the list of arrays
	for (int p = 0; p < partitions; p++)
	{
		for (int i = 0; i < instanceList[p].count; i++)
			delete instanceList[p].instance[i];
		delete [] instanceList[p].instance;
	}

	delete [] instanceList;
}

//***************************************************************************
// Get at the parent collisionTree info
//***************************************************************************
TupperwareCollisionTree &CollisionInstanceList::CollisionTree(void)
{
	return collisionTree;
}

//***************************************************************************
//***************************************************************************
int CollisionInstanceList::Partitions(void)
{
	return partitions;
}

//***************************************************************************
// The list of instances (by partition)
//***************************************************************************
int CollisionInstanceList::Instances(
int partition)
{
	// Sanity check
	assert(partition >= 0 && partition < partitions);

	// Give them back the thing
	return instanceList[partition].count;
}

//***************************************************************************
//***************************************************************************
int CollisionInstanceList::TotalInstances(void)
{
	return totalInstances;
}

//***************************************************************************
//***************************************************************************
CollisionInstance *CollisionInstanceList::GetInstance(
int partition,
int index)
{
	assert(partition >= 0 && partition < partitions);
	assert(index >= 0 && index < instanceList[partition].count);

	return instanceList[partition].instance[index];
}

//***************************************************************************
//***************************************************************************
PartitionCollisionInstanceList::PartitionCollisionInstanceList(
CollisionInstanceList &parent,
int withPartition) :
instanceList(parent),
partition(withPartition)
{
	// How many layers do we have?
	layers = 0;
	for (int i = 0; i < Instances(); i++)
	{
		CollisionInstance *instance = GetInstance(i);
		assert(instance);

		if (instance->Layer() + 1 > layers)
			layers = instance->Layer() + 1;
	}

	// Allocate the array
	if (layers > 0)
		layerInstances = new int[layers];
	else
		layerInstances = NULL;

	// Zero everything out
	for (int l = 0; l < layers; l++)
		layerInstances[l] = 0;

	// Figure out our layer usage
	for (i = 0; i < Instances(); i++)
	{
		CollisionInstance *instance = GetInstance(i);
		assert(instance);

		layerInstances[instance->Layer()] += 1;
	}
}

//***************************************************************************
//***************************************************************************
PartitionCollisionInstanceList::~PartitionCollisionInstanceList(void)
{
	delete [] layerInstances;
}

//***************************************************************************
// Get at the parent collisionTree info
//***************************************************************************
TupperwareCollisionTree &PartitionCollisionInstanceList::CollisionTree(void)
{
	return instanceList.CollisionTree();
}

//***************************************************************************
// What partition are we?
//***************************************************************************
int PartitionCollisionInstanceList::Partition(void)
{
	return partition;
}

//***************************************************************************
// Get at instance counts
//***************************************************************************
int PartitionCollisionInstanceList::Instances(void)
{
	return instanceList.Instances(partition);
}

//***************************************************************************
// How many instances are in a layer?
//***************************************************************************
int PartitionCollisionInstanceList::InstancesInLayer(int layer)
{
	assert(layer >= 0);
		
	if (layer >= layers)
		return 0;
	else
		return layerInstances[layer];
}

//***************************************************************************
// The list of instances (by partition)
//***************************************************************************
CollisionInstance *PartitionCollisionInstanceList::GetInstance(int index)
{
	return instanceList.GetInstance(partition, index);
}

//***************************************************************************
//***************************************************************************
CollisionInstance::CollisionInstance(
TupConfigRoot &nodeTreeRoot,
TupperwareAggregate &nodeAgg) :
agg(&nodeAgg),
node(&nodeAgg)
{
	// What layer is this instance in?
	TupperwareScalar *layerIndexScalar = agg->FindScalarByKey("Annotate_CollisionLayer");
	layer = (layerIndexScalar ? layerIndexScalar->GetAsInt() : 0);

	// Retrieve the bounding box info
	Vector3 boundingBoxMin = *((Vector3 *)agg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat());
	Vector3 boundingBoxMax = *((Vector3 *)agg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat());
	aabb.Set(boundingBoxMin, boundingBoxMax);
	rect = aabb.ToRect();
}
