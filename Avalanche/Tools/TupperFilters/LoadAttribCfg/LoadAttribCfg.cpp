////////////////////////////////////////////////////////////////////////////
//
// LoadAttribCfg
//
// Class to handle the Load Attrib Cfg Plugin
//
////////////////////////////////////////////////////////////////////////////

#include "LoadAttribCfgPch.h"
#include "LoadAttribCfg.h"

LoadAttribCfgFilter::LoadAttribCfgFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
	m_nodeConfigFile = "ObjAttr.cfg";
	m_faceConfigFile = "FaceAttr.cfg";
	m_mapConfigFile = "MapAttr.cfg";
}

int LoadAttribCfgFilter::GetTotalPasses(void) const
{
	return 1;
}

void LoadAttribCfgFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	TupperwareAggregate *pSceneAgg = dataManager.GetSceneRoot();
	TupScene tupScene(pSceneAgg);

	PluginLog::SetLog(&m_log);
	CheckOptions(options);

	TupString relativePath,fullPath,name,ext;

	// node attributes
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,m_nodeConfigFile,dataManager.GetConfigDirectory());
	TupperwareAggregate *pNodeAttributeTreeAgg;
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Loading Object Attribute Config File %s.",(const char *)fullPath);
	if (TupperwareLoadSaveManager::Read(fullPath,&pNodeAttributeTreeAgg))
	{
		// add to scene
		tupScene.AddNodeAttributeTree(pNodeAttributeTreeAgg);
		delete pNodeAttributeTreeAgg;
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Error Loading %s.",(const char *)fullPath);
	}

	// face attributes
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,m_faceConfigFile,dataManager.GetConfigDirectory());
	TupperwareAggregate *pFaceAttributeTreeAgg;
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Loading Face Attribute Config File %s.",(const char *)fullPath);
	if (TupperwareLoadSaveManager::Read(fullPath,&pFaceAttributeTreeAgg))
	{
		// add to scene
		tupScene.AddFaceAttributeTree(pFaceAttributeTreeAgg);
		delete pFaceAttributeTreeAgg;
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Error Loading %s.",(const char *)fullPath);
	}

	// map attributes
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,m_mapConfigFile,dataManager.GetConfigDirectory());
	TupperwareAggregate *pMapAttributeTreeAgg;
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Loading Map Attribute Config File %s.",(const char *)fullPath);
	if (TupperwareLoadSaveManager::Read(fullPath,&pMapAttributeTreeAgg))
	{
		// add to scene
		tupScene.AddMapAttributeTree(pMapAttributeTreeAgg);
		delete pMapAttributeTreeAgg;
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Error Loading %s.",(const char *)fullPath);
	}

}

void LoadAttribCfgFilter::CheckOptions(const FilterOptions &options)
{
	const char *pValue;
	pValue = options.GetValue("NodeConfigFile");
	if (pValue)
	{
		m_nodeConfigFile = pValue;
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"NodeConfigFile not specified.");
	}

	pValue = options.GetValue("FaceConfigFile");
	if (pValue)
	{
		m_faceConfigFile = pValue;
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FaceConfigFile not specified.");
	}

	pValue = options.GetValue("MapConfigFile");
	if (pValue)
	{
		m_mapConfigFile = pValue;
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"MapConfigFile not specified.");
	}
}



