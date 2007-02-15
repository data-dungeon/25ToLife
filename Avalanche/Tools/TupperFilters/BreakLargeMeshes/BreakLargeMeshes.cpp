////////////////////////////////////////////////////////////////////////////
//
// BreakLargeMeshes
//
// Class to handle the BreakLargeMeshes Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BreakLargeMeshes.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 7/14/03    Time: 3:25p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * allow for turning off cutting
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * breakup meshes based on visibility regions as well as the grid
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/13/02    Time: 1:08p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/12/02    Time: 4:03p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Break Up Meshes Complete (for now)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 4:01p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Test Meshes and Copy objects
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Start
 */

////////////////////////////////////////////////////////////////////////////

#include "BreakLargeMeshesPch.h"
#include "BreakLargeMeshes.h"
#include "TestMeshes.h"
#include "BreakMeshes.h"

char *BreakLargeMeshesFilter::m_readDependencyNames[NUM_READ_DEPENDENCIES] = 
{
	"Bounding_NodeBounding", // NODE_BOUNDING
	"Bounding_NodeOBB", // NODE_OBB
};

Revision BreakLargeMeshesFilter::m_readDependencyRevisions[NUM_READ_DEPENDENCIES] = 
{
	Revision(1,0), // NODE_BOUNDING
	Revision(1,0), // NODE_OBB
};


BreakLargeMeshesFilter::BreakLargeMeshesFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int BreakLargeMeshesFilter::GetTotalPasses(void) const
{
	return 1;
}

void BreakLargeMeshesFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Building Node Attribute Tree");
	TupConfigRoot tupConfigRoot;
	TupperwareAggregate *pSceneRootAgg = dataManager.GetSceneRoot();
	TupScene tupScene(pSceneRootAgg);
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = tupScene.GetNodeAttributeTree();
	if (!pSceneNodeAttributeTreeAgg)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}
	tupConfigRoot.BuildConfigTree(pSceneNodeAttributeTreeAgg);

	TupperwareAggregate *pNodePoolAgg = tupScene.GetNodePool();
	assert(pNodePoolAgg);
	TupNodePool tupNodePool(pNodePoolAgg);

	TupperwareAggregate *pObjectPoolAgg = tupScene.GetObjectPool();
	assert(pObjectPoolAgg);
	TupObjectPool tupObjectPool(pObjectPoolAgg);

	TupperwareAggregate *pMaterialPoolAgg = tupScene.GetMaterialPool();
	assert(pMaterialPoolAgg);
	TupMaterialPool tupMaterialPool(pMaterialPoolAgg);

	TupperwareAggregate *pMapPoolAgg = tupScene.GetMapPool();
	assert(pMapPoolAgg);
	TupMapPool tupMapPool(pMapPoolAgg);

	TupperwareAggregate *pImagePoolAgg = tupScene.GetImagePool();
	assert(pImagePoolAgg);
	TupImagePool tupImagePool(pImagePoolAgg);

	TupperwareAggregate *pModifierPoolAgg = tupScene.GetModifierPool();
	assert(pModifierPoolAgg);
	TupModifierPool tupModifierPool(pModifierPoolAgg);

	int tupRootNodeRef;
	tupScene.GetRootNodeReference(tupRootNodeRef);

	TestMeshesManager testMeshesManager(tupNodePool,tupObjectPool,tupMaterialPool,tupMapPool,tupImagePool,tupModifierPool,tupRootNodeRef,tupConfigRoot,m_settings);
	BreakMeshesManager breakMeshesManager(tupNodePool,tupObjectPool,tupMaterialPool,tupMapPool,tupImagePool,tupModifierPool,tupRootNodeRef,tupConfigRoot,m_settings);

	if (m_settings.m_bCutOnVisRegion)
	{
		if (testMeshesManager.TestAndMarkNodes(TestMeshesManager::VISIBILITY_SET))
		{
			breakMeshesManager.BreakUpMarkedNodesWithVisibilitySets();
		}
	}
	if (m_settings.m_bCutOnGrid)
	{
		if (testMeshesManager.TestAndMarkNodes(TestMeshesManager::GRID))
		{
			breakMeshesManager.BreakUpMarkedNodesOnGrid();
		}
	}
	if (m_settings.m_bCutOnTextureUsage)
	{
		if (testMeshesManager.TestAndMarkNodes(TestMeshesManager::TEXTURE_USAGE))
		{
			breakMeshesManager.BreakUpMarkedNodesOnTextureUsage();
		}
	}

}

void BreakLargeMeshesFilter::CheckOptions(const FilterOptions &options)
{
	m_settings.m_sizeThreshold = 120.0f;
	m_settings.m_gridSize = 40.0f;
	m_settings.m_cutOnTextureUsageLimit = 300;
	m_settings.m_bCutOnGrid = true;
	m_settings.m_bCutOnVisRegion = true;
	m_settings.m_bCutOnTextureUsage = true;

	// grid size
	const char *pOptionGridSize = options.GetValue("GridSize");
	if (pOptionGridSize)
	{
		m_settings.m_gridSize = (float) atof(pOptionGridSize);
		if (m_settings.m_gridSize<1.0f)
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"GridSize %f is not valid.  Using default of 40.0",m_settings.m_gridSize);
			m_settings.m_gridSize = 40.0f; // set back to default
		}
		else
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"GridSize set to  %f",m_settings.m_gridSize);
		}
	}
	// size threshold
	const char *pOptionSizeThreshold = options.GetValue("SizeThreshold");
	if (pOptionSizeThreshold)
	{
		m_settings.m_sizeThreshold = (float) atof(pOptionSizeThreshold);
		if (m_settings.m_sizeThreshold<m_settings.m_gridSize)
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"SizeThreshold %f is less than the GridSize.  Setting to GridSize.",m_settings.m_sizeThreshold);
			m_settings.m_sizeThreshold = m_settings.m_gridSize; // set back gridsize
		}
		else
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"SizeThreshold set to %f",m_settings.m_sizeThreshold);
		}
	}
	const char *pCutOnTextureUsageLimit = options.GetValue("CutOnTextureUsageLimit");
	if (pCutOnTextureUsageLimit)
	{
		m_settings.m_cutOnTextureUsageLimit = atoi(pCutOnTextureUsageLimit);
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Cut On Texture Usage Limit set to %dk",m_settings.m_cutOnTextureUsageLimit);
	}

	// cut on grid
	const char *pCutOnGrid = options.GetValue("CutOnGrid");
	if (pCutOnGrid && !stricmp(pCutOnGrid,"false"))
	{
		m_settings.m_bCutOnGrid = false;
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Cut on Grid Disabled");
	}

	// cut on grid
	const char *pCutOnVisRegion = options.GetValue("CutOnVisRegion");
	if (pCutOnVisRegion && !stricmp(pCutOnVisRegion,"false"))
	{
		m_settings.m_bCutOnVisRegion = false;
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Cut on Vis Region Disabled");
	}

	// cut on texture usage
	const char *pCutOnTextureUsage = options.GetValue("CutOnTextureUsage");
	if (pCutOnTextureUsage && !stricmp(pCutOnTextureUsage,"false"))
	{
		m_settings.m_bCutOnTextureUsage = false;
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Cut on Texture Usage Disabled");
	}

}

int BreakLargeMeshesFilter::GetTotalReadDependencies(int pass,const FilterOptions &options) const
{
	return NUM_READ_DEPENDENCIES;
}

const char *BreakLargeMeshesFilter::GetReadDependencyKey(int pass,const FilterOptions &options,int index) const
{
	return m_readDependencyNames[index];
}

Revision BreakLargeMeshesFilter::GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	return m_readDependencyRevisions[index];
}
