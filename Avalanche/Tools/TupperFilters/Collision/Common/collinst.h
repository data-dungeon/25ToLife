//***************************************************************************
// This class maintains all the information about the collision instances
// in a tupperware list
//***************************************************************************
#ifndef COLLINST_H
#define COLLINST_H

// A specific collision instance
class CollisionInstance;

// All the stuff to wrap up tupperware collision stuff
class TupperwareCollisionTree
{
public:
	TupperwareCollisionTree(FilterDataManager &filterDataManager, bool terrainMode);
	~TupperwareCollisionTree(void);

	// All the tupperware goodies
	FilterDataManager		&dataManager;

	TupScene 				*scene;
	TupConfigRoot 			nodeTreeRoot;
	TupConfigRoot 			faceTreeRoot;

	TupNodePool 			*nodePool;
	TupObjectPool 			*objectPool;
	TupModifierPool 		*modifierPool;
	TupMaterialPool 		*materialPool;
	TupMapPool 				*mapPool;
	TupImagePool 			*imagePool;

	FaceCollisionManager *faceCollisionManager;

	// Some utilities
	const char *Name(void)
		{ return dataManager.GetOutputFilename(); }
};

// List of all collision instances
class CollisionInstanceList
{
public:
	CollisionInstanceList(TupperwareCollisionTree &tupCollisionTree);
	~CollisionInstanceList(void);

	// Get at the parent collision info
	TupperwareCollisionTree	&CollisionTree(void);

	// How many partitions are there?
	int					  		Partitions(void);

	// Get at instance counts
	int 							Instances(int partition);
	int 							TotalInstances(void);

	// The list of instances (by partition)
	CollisionInstance			*GetInstance(int partition, int index);

private:
	// The tupperware collision
	TupperwareCollisionTree				&collisionTree;

	// An array of lists
	struct InstanceList
	{
		CollisionInstance		**instance;
		int						count;
	};
	InstanceList							*instanceList;
	int										partitions;
	int										totalInstances;

	void										BuildInstanceLists(void);
};

// A subclass that just returns a particular partition's info
class PartitionCollisionInstanceList
{
public:
	PartitionCollisionInstanceList(CollisionInstanceList &parent, int withPartition);
	~PartitionCollisionInstanceList(void);

	// Get at the parent collision info
	TupperwareCollisionTree &CollisionTree(void);

	// What partition are we?
	int Partition(void);

	// Get at instance count
	int Instances(void);

	// How many instances are in a layer?
	int InstancesInLayer(int layer);

	// The list of instances (by partition)
	CollisionInstance *GetInstance(int index);

private:
	CollisionInstanceList	&instanceList;
	int							partition;
	int							*layerInstances;
	int							layers;
};

// The actual collision instance
class CollisionInstance
{
public:
	CollisionInstance(TupConfigRoot &nodeTreeRoot, TupperwareAggregate &nodeAgg);
	~CollisionInstance(void) {}

	// Access the node
	TupNode &Node(void)
		{ return node; }

	// Get the name
	char *Name(void)
		{ char *name; node.GetName(&name); return name; }

	// Get the object reference
	int ObjectReference(void)
		{ int objectRef; node.GetObjectReference(objectRef); return objectRef; }

	// What collision layer is this node in?
	int Layer(void) const
		{ return layer; }

	// Access bounding information
	const Vector3 &BoundingBoxMin(void) const
		{ return aabb.minV; }
	const Vector3 &BoundingBoxMax(void) const
		{ return aabb.maxV; }
	const XZRect &Rect(void) const
		{ return rect; }
	const AABB &BoundingBox(void) const
		{ return aabb; }

private:
	// The node
	TupperwareAggregate	*agg;
	TupNode					node;

	// Our properties
	int						layer;
	XZRect 					rect;
	AABB						aabb;
};

#endif
