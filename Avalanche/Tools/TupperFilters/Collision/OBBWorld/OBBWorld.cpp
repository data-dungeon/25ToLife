////////////////////////////////////////////////////////////////////////////
//
// OBBWorld
//
// Class to handle the OBBWorld Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: OBBWorld.cpp $
 * 
 * *****************  Version 15  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:26p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * new dependencies
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * update for streaming system
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 1/14/03    Time: 11:37a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * added collisionlayer dependency
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * File Dependency checks
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Added support for attrib tree
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/27/02    Time: 1:13p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Dependencies
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * hybrid configuration
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/22/02    Time: 2:39p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Surface Types file loading
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Abort checks within system
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/14/02    Time: 9:49a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 */

#include "Pch.h"
#include "OBBWorld.h"
#include "qtpart.h"

OBBWorldFilter::OBBWorldFilter(TupperChainLogBase &log) : FilterPlugin(log),
	m_saveVisFile(false),
	m_sectorSize(0.0f),
	m_minQuadrantSize(0.0f),
	m_minDimension(0.0f)
{
	m_shrinkBy.Set(1.0f,1.0f,1.0f);
}

int OBBWorldFilter::GetTotalPasses(void) const
{
	return 1;
}

char *OBBWorldFilter::m_readDependencyNames[NUM_READ_DEPENDENCIES] =
{
	"Annotate_InstanceIndexes", // INSTANCE_INDEXES
	"Bounding_ObjectBounding", // OBJECT_BOUNDING
	"Bounding_NodeBounding", // NODE_BOUNDING
	"Annotate_ParentObjectReferences", // PARENT_OBJECT_REFERENCES
	"Annotate_ParentNodeReferences", // PARENT_NODE_REFERENCES
	"Annotate_PropagateVertexColors", // PROPAGATE_VERTEX_COLORS
	"Annotate_CollisionLayer", // COLLISION_LAYER
};

Revision OBBWorldFilter::m_readDependencyRevisions[NUM_READ_DEPENDENCIES] =
{
	Revision(1,0), // INSTANCE_INDEXES
	Revision(1,0), // OBJECT_BOUNDING
	Revision(1,0), // NODE_BOUNDING
	Revision(1,0), // PARENT_OBJECT_REFERENCES
	Revision(1,0), // PARENT_NODE_REFERENCES
	Revision(1,0), // PROPAGATE_VERTEX_COLORS
	Revision(1,0), // COLLISION_LAYER
};

int OBBWorldFilter::GetTotalReadDependencies(int pass,const FilterOptions &options) const
{
	return NUM_READ_DEPENDENCIES;
}

const char *OBBWorldFilter::GetReadDependencyKey(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyNames[index];
}

Revision OBBWorldFilter::GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyRevisions[index];
}


void OBBWorldFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	TupString outputFilename(dataManager.GetOutputFilename());

	// Create the stats dlg
	g_stats = &m_stats;
	m_stats.Clear();
	m_stats.minQuadrantSize = m_minQuadrantSize;
	m_stats.shrink = (1.0f - m_shrinkBy.x( ) ) * 100.0f;
	m_stats.minDimension = m_minDimension;

	try
	{
		// Create the collision data
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Converting World...");
		OBBQuadTreePartitionFactory converter(dataManager,m_minQuadrantSize,m_shrinkBy,m_minDimension);
		m_stats.totalInstances = converter.TotalInstances();

		if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

		FilePath::ChangeExtension(outputFilename,".ODB");
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Saving OBB Database %s",outputFilename);
		converter.OutputOBBDatabase(outputFilename);
		if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

		if (m_saveVisFile)
		{
			FilePath::ChangeExtension(outputFilename,".PPM");
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Saving Vis File %s",outputFilename);
			converter.OutputVisData(outputFilename);
			if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
		}
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"OBBWorld Filter\r\n  %s",(const char *)(e)));
	}
}


void OBBWorldFilter::CheckOptions(const FilterOptions &options)
{
	// save vis file
	const char *pOptionSaveVisFile = options.GetValue("SaveVisFile");
	if (pOptionSaveVisFile && !stricmp(pOptionSaveVisFile,"true"))
		m_saveVisFile = true;
	else
		m_saveVisFile = false;

	// MinDimension
	const char *pOptionMinDimension = options.GetValue("MinDimension");
	if (pOptionMinDimension)
	{
		m_minDimension = (float)atof(pOptionMinDimension);
		if (m_minDimension<0.0f)
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"MinDimension %f is not valid.  Using default of 0.0",m_minDimension);
			m_minDimension = 0.0f; // set back to default
		}
		else
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"MinDimension set to  %f",m_minDimension);
		}
	}
	else
	{
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"MinDimension option not set defaulting to %f",m_minDimension);
	}

	// MinQuadrantSize
	const char *pOptionMinQuadrantSize = options.GetValue("MinQuadrantSize");
	if (pOptionMinQuadrantSize)
	{
		m_minQuadrantSize = (float)atof(pOptionMinQuadrantSize);
		if (m_minQuadrantSize<0.0f)
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"MinQuadrantSize %f is not valid.  Using default of 0.0",m_minQuadrantSize);
			m_minQuadrantSize = 0.0f; // set back to default
		}
		else
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"MinQuadrantSize set to  %f",m_minQuadrantSize);
		}
	}
	else
	{
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"MinQuadrantSize option not set defaulting to %f",m_minQuadrantSize);
	}
	// Shrinkage
	const char *pOptionShrinkage = options.GetValue("Shrinkage");
	if (pOptionShrinkage)
	{
		float shrinkage = 1.0f - ((float)atof(pOptionShrinkage)/100.0f);
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Shrinkage xyz set to  %f",shrinkage);
		m_shrinkBy.Set(shrinkage,shrinkage,shrinkage);
	}
	// ShrinkageX
	const char *pOptionShrinkageX = options.GetValue("ShrinkageX");
	if (pOptionShrinkageX)
	{
		float shrinkageX = 1.0f - ((float)atof(pOptionShrinkageX)/100.0f);
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Shrinkage x set to  %f",shrinkageX);
		m_shrinkBy.x( shrinkageX );
	}
	// ShrinkageY
	const char *pOptionShrinkageY = options.GetValue("ShrinkageY");
	if (pOptionShrinkageY)
	{
		float shrinkageY = 1.0f - ((float)atof(pOptionShrinkageY)/100.0f);
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Shrinkage y set to  %f",shrinkageY);
		m_shrinkBy.y( shrinkageY );
	}
	// ShrinkageZ
	const char *pOptionShrinkageZ = options.GetValue("ShrinkageZ");
	if (pOptionShrinkageZ)
	{
		float shrinkageZ = 1.0f - ((float)atof(pOptionShrinkageZ)/100.0f);
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Shrinkage z set to  %f",shrinkageZ);
		m_shrinkBy.z( shrinkageZ );
	}
}

void OBBWorldFilter::GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	TupString outputFilename(dataManager.GetOutputFilename());

	FilePath::ChangeExtension(outputFilename,".ODB");
	filePathArray.AddFilePath(outputFilename,FilePathArray::FILE_OUTPUT|FilePathArray::FILE_ACCESS_FATAL);

	if (m_saveVisFile)
	{
		FilePath::ChangeExtension(outputFilename,".PPM");
		filePathArray.AddFilePath(outputFilename,FilePathArray::FILE_OUTPUT|FilePathArray::FILE_ACCESS_FATAL);
	}
}
