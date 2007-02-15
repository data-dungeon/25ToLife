////////////////////////////////////////////////////////////////////////////
//
// DrOctopus
//
// Class to handle the DrOctopus Plugin
//
////////////////////////////////////////////////////////////////////////////
#ifndef DROCTOPUS_H
#define DROCTOPUS_H

class TupperwareAggregate;
class PartitionOctTreeFactory;

struct DrOctopusOptions
{
	float					minDepthSize;
	bool					leanTriangles;
	bool					rewindTriangles;
};

class DrOctopusFilter : public FilterPlugin
{
public:
	DrOctopusFilter(TupperChainLogBase &log);
	~DrOctopusFilter();

	// How many total passes do we have?
	int GetTotalPasses(void) const
		{ return 1; }

	// The actual call that does all the work
	void Process(int pass, const FilterOptions &options, FilterDataManager &dataManager);

	// Dependency calls
	void GetFileDependencies(int pass, const FilterOptions &options, FilterDataManager &dataManager, FilePathArray &filePathArray);

	int GetTotalReadDependencies(int pass, const FilterOptions &options) const;
	const char *GetReadDependencyKey(int pass, const FilterOptions &options, int index) const;
	Revision GetReadDependencyRevision(int pass, const FilterOptions &options, int index) const;

private:
	// Our read dependencies
	enum ReadDependencies
	{
		INSTANCE_INDEXES,
		OBJECT_BOUNDING,
		NODE_BOUNDING,
		PARENT_OBJECT_REFERENCES,
		PARENT_NODE_REFERENCES,
		PROPAGATE_VERTEX_COLORS,
		COLLISION_LAYER,
		NUM_READ_DEPENDENCIES // last entry
	};
	static char 		*m_readDependencyNames[NUM_READ_DEPENDENCIES];
	static Revision 	m_readDependencyRevisions[NUM_READ_DEPENDENCIES];

	// Options
	DrOctopusOptions			m_options;
	void CheckOptions(const FilterOptions &options);
	static float GetOption(const FilterOptions &options,
									const char *optionName, float defaultValue, 
									float minValue = 0.0f, float maxValue = 100000.0f);
	static bool GetOption(const FilterOptions &options,
									const char *optionName, bool defaultValue);

	// The tupperware stuff
	TupperwareCollisionTree	*m_collisionTree;
	CollisionInstanceList	*m_instanceList;

	// What we create
	CollidePartitionSystem	m_cps;
	CollidePartition			*m_cp;

	// The array of partition lists
	struct Partition
	{
		PartitionCollisionInstanceList	*instanceList;
		PartitionOctTreeFactory				*collisionData;
	};
	Partition									*m_partition;
	bool BuildPartitionInfo(FilterDataManager &dataManager);

	// Output the database
 	void OutputDatabase(FilterDataManager &dataManager);
};

#endif
