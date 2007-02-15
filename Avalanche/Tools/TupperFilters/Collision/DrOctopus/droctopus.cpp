////////////////////////////////////////////////////////////////////////////
//
// DrOctopus
//
// Class to handle the DrOctopus Plugin
//
////////////////////////////////////////////////////////////////////////////
#include "Pch.h"
#include "DrOctopus.h"

char *DrOctopusFilter::m_readDependencyNames[NUM_READ_DEPENDENCIES] =
{
	"Annotate_InstanceIndexes", // INSTANCE_INDEXES
	"Bounding_ObjectBounding", // OBJECT_BOUNDING
	"Bounding_NodeBounding", // NODE_BOUNDING
	"Annotate_ParentObjectReferences", // PARENT_OBJECT_REFERENCES
	"Annotate_ParentNodeReferences", // PARENT_NODE_REFERENCES
	"Annotate_PropagateVertexColors", // PROPAGATE_VERTEX_COLORS
	"Annotate_CollisionLayer", // COLLISION_LAYER
};

Revision DrOctopusFilter::m_readDependencyRevisions[NUM_READ_DEPENDENCIES] =
{
	Revision(1,0), // INSTANCE_INDEXES
	Revision(1,0), // OBJECT_BOUNDING
	Revision(1,0), // NODE_BOUNDING
	Revision(1,0), // PARENT_OBJECT_REFERENCES
	Revision(1,0), // PARENT_NODE_REFERENCES
	Revision(1,0), // PROPAGATE_VERTEX_COLORS
	Revision(1,0), // COLLISION_LAYER
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
DrOctopusFilter::DrOctopusFilter(
TupperChainLogBase &log) :
FilterPlugin(log)
{
	// Default the options
	m_options.minDepthSize = 4.0f;	
	m_options.leanTriangles = false;
	m_options.rewindTriangles = false;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
DrOctopusFilter::~DrOctopusFilter()
{
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void DrOctopusFilter::Process(
int pass,
const FilterOptions &options,
FilterDataManager &dataManager)
{
	// NULL out everything
	m_collisionTree = NULL;
	m_instanceList = NULL;
	
	memset(&m_cps, '\0', sizeof(m_cps));
	m_cp = NULL;
	m_partition = NULL;

	// Setup the log
	PluginLog::SetLog(&m_log);

	// Check the options
	CheckOptions(options);

	// Setup the stats tracking
	g_stats.Clear();

	try
	{
		// Create the collision data
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Converting World...");
		if (!BuildPartitionInfo(dataManager))
			throw(PluginException(ERROR_FATAL, "Your world is completely fuxored!!!!!"));

		// Check abort
		if (PluginLog::GetAbortFlag())
			throw(PluginException(ERROR_FATAL, "Processing Stopped By User"));

		// Process the world
		for (int p = 0; p < m_cps.partitions; p++)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Processing Partition #%d", p);

		  	// Create them a partition specific list
			m_partition[p].instanceList = new PartitionCollisionInstanceList(*m_instanceList, p);
			assert(m_partition[p].instanceList);

			// converte the data
			m_partition[p].collisionData = new PartitionOctTreeFactory(*m_partition[p].instanceList, m_options);
			assert(m_partition[p].collisionData);

			// fill out the partition
			memset(&m_cp[p], '\0', sizeof(m_cp[p]));
			m_cp[p].bounds = m_partition[p].collisionData->Bounds();
			if (m_partition[p].instanceList->Instances() == 0)
				m_cp[p].flags |= CollidePartition::FLAGS_EMPTY;

			// Update the progress bar
			g_stats.instancesProcessed += m_partition[p].instanceList->Instances();
			PluginLog::SetProgressBar(0, g_stats.totalInstances, g_stats.instancesProcessed);

			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Instanced Processed %d",m_partition[p].instanceList->Instances());

			// Check abort
			if (PluginLog::GetAbortFlag())
				throw(PluginException(ERROR_FATAL, "Processing Stopped By User"));
		}

		// Save the database
		OutputDatabase(dataManager);

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Triangles %d Degenerates %d",g_stats.triangles,g_stats.degenerates);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Max Triangles in Node %d at depth %d",g_stats.maxTrianglesInNode,g_stats.maxTrianglesInNodeDepth);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Nodes Created %d MaxDepth %d",g_stats.nodes,g_stats.maxDepth);

		// Check abort
		if (PluginLog::GetAbortFlag())
			throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"DrOctopus Filter\r\n  %s",(const char *)(e)));
	}

	delete m_collisionTree;
	delete m_instanceList;

	// clean up everything
	for (int i = 0; i < m_cps.partitions; i++)
	{
		delete m_partition[i].instanceList;
		delete m_partition[i].collisionData;
	}
	delete [] m_partition;
	
	delete [] m_cp;


}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void DrOctopusFilter::GetFileDependencies(
int pass,
const FilterOptions &options,
FilterDataManager &dataManager,
FilePathArray &filePathArray)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	TupString outputFilename(dataManager.GetOutputFilename());

	FilePath::ChangeExtension(outputFilename,".CDB");
	filePathArray.AddFilePath(outputFilename, FilePathArray::FILE_OUTPUT|FilePathArray::FILE_ACCESS_FATAL);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int DrOctopusFilter::GetTotalReadDependencies(
int pass,
const FilterOptions &options) const
{
	return NUM_READ_DEPENDENCIES;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
const char *DrOctopusFilter::GetReadDependencyKey(
int pass,
const FilterOptions &options,
int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyNames[index];
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
Revision DrOctopusFilter::GetReadDependencyRevision(
int pass,
const FilterOptions &options,
int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyRevisions[index];
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void DrOctopusFilter::CheckOptions(
const FilterOptions &options)
{
	// Minimum depth size
	m_options.minDepthSize = GetOption(options, "MinDepthSize", m_options.minDepthSize);
	m_log.Add(LogFlags::VERBOSE_LEVEL_MED,"Minimum depth size set to  %f", m_options.minDepthSize);

	m_options.leanTriangles = GetOption(options, "LeanTriangles", m_options.leanTriangles);
	m_log.Add(LogFlags::VERBOSE_LEVEL_MED,"Lean triangles set to %s", m_options.leanTriangles ? "true" : "false");

	m_options.rewindTriangles = GetOption(options, "RewindTriangles", m_options.rewindTriangles);
	m_log.Add(LogFlags::VERBOSE_LEVEL_MED,"Rewind triangles set to %s", m_options.rewindTriangles ? "true" : "false");
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
float DrOctopusFilter::GetOption(
const FilterOptions &options,
const char *optionName,
float defaultValue,
float minValue,
float maxValue)
{
	const char *option = options.GetValue(optionName);
	if (!option)
		return defaultValue;

	float value = (float)atof(option);
	if (value < minValue)
		return minValue;
	else if (value > maxValue)
		return maxValue;
	else
		return value;
} 

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
bool DrOctopusFilter::GetOption(
const FilterOptions &options,
const char *optionName,
bool defaultValue)
{
	const char *option = options.GetValue(optionName);
	if (!option)
		return defaultValue;
	
	if (strcmpi(option, "true") == 0 || strcmpi(option, "yes") == 0)
		return true;
	else
		return false;
} 

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
bool DrOctopusFilter::BuildPartitionInfo(
FilterDataManager &dataManager)
{
	// Instantiate the collision tree
	m_collisionTree = new TupperwareCollisionTree(dataManager, true);
	assert(m_collisionTree);
	if (!m_collisionTree)
		return false;

	// Build up the master instance list
	m_instanceList = new CollisionInstanceList(*m_collisionTree);
	assert(m_instanceList);
	if (!m_instanceList)
		return false;

	// What is the total instance count?
	g_stats.totalInstances = m_instanceList->TotalInstances();

	// Setup the cps struct
	m_cps.version = CPS_VERSION;
	m_cps.partitions = m_instanceList->Partitions();
	assert(m_cps.partitions > 0);
	if (m_cps.partitions <= 0)
		return false;

	// Setup the array of partitions
	m_cp = new CollidePartition[m_cps.partitions];
	assert(m_cp);
	if (!m_cp)
		return false;

	// Get the extents of everything
	m_cps.bounds.Clear();
	for (int p = 0; p < m_cps.partitions; p++)
	{
		for (int i = 0; i < m_instanceList->Instances(p); i++)
		{
			CollisionInstance *instance = m_instanceList->GetInstance(p, i);
			assert(instance);

			m_cps.bounds += instance->BoundingBox();
		}
	}

	// Make the array of partitions
	m_partition = new Partition[m_cps.partitions];
	assert(m_partition);
	if (!m_partition)
		return false;

	// Everything worked
	return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void DrOctopusFilter::OutputDatabase(
FilterDataManager &dataManager)
{
	TupString outputFile(dataManager.GetOutputFilename());
	FilePath::ChangeExtension(outputFile,".CDB");

	m_log.Add(LogFlags::VERBOSE_LEVEL_LOW, "Saving Collision Database %s", outputFile);

	// Write out all the sectorizer's output
	for (int p = 0; p < m_cps.partitions; p++)
	{
		// Make the partition filename
		TupString pathFile = outputFile;
		FilePath::AddNumberToFileName(pathFile, p);

		// Delete the old file
		remove(pathFile);

		// Write it out!
		if (m_partition[p].collisionData)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Writing Partition %d", p);
			m_partition[p].collisionData->OutputDatabase(pathFile);
		}
	}

	// Now write out the cph file
	TupString cphFile = outputFile;
	FilePath::ChangeExtension(cphFile, ".cph");
	FilePath::AddNumberToFileName(cphFile, 0);

	// Open it
	FILE *fh = fopen(cphFile, "wb");
	if (!fh)
		throw(PluginException(ERROR_FATAL, "Error creating .cph file"));

	// Write the header
	if (fwrite(&m_cps, sizeof(m_cps), 1, fh) != 1)
		throw(PluginException(ERROR_FATAL, "Error writing .cph file"));

	// Write the records
	if (fwrite(m_cp, sizeof(CollidePartition), m_cps.partitions, fh) != (size_t)m_cps.partitions)
		throw(PluginException(ERROR_FATAL, "Error writing .cph file"));

	// We are done!
	fclose(fh);
}

