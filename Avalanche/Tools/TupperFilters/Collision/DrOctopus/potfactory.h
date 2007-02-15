////////////////////////////////////////////////////////////////////////////
// Creates the array of OctTrees for the partition
////////////////////////////////////////////////////////////////////////////
#ifndef POTFACTORY_H
#define POTFACTORY_H

class FactoryOctTree;

class PartitionOctTreeFactory
{
public:
	PartitionOctTreeFactory(PartitionCollisionInstanceList &instanceList, const DrOctopusOptions &options);
	~PartitionOctTreeFactory();

	// What is our total bounds
	AABB Bounds()
		{ return m_totalAABB; }

	// Write the database
	void OutputDatabase(const char *filename);

private:
	// Our instance list and partition
	PartitionCollisionInstanceList	&m_instanceList;
	
	// The options
	const DrOctopusOptions				&m_options;

	// extent data
	AABB										m_aabb[MAX_COLLIDE_LAYERS];
	AABB										m_totalAABB;

	// Build the triangle soup
	TriangleSoup							m_triangleSoup[MAX_COLLIDE_LAYERS];
	void 										BuildSoups();
	void 										ProcessInstance(TupNode &tupNode, TupTriMesh &tupTriMesh,
																		TupperwareAggregate *vertexColorMapAgg,
																		TupArray<FaceCollisionManager::Attributes> &faceAttributeArray,
																		TupArray<int> &faceAttributeIndices,
																		TriangleSoup &soup,
																		AABB &layerAABB,
																		AABB &totalAABB);

	// The oct trees
	FactoryOctTree							*m_tree[MAX_COLLIDE_LAYERS];
	void										ProcessTrees(const DrOctopusOptions &options);
};

#endif
