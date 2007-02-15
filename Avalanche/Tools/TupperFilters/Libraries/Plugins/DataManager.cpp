////////////////////////////////////////////////////////////////////////////
//
// DataManager
//
// Class to handle the data that is passed from filter to filter
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DataManager.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:46p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 6/04/02    Time: 3:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Xref paths
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 6/03/02    Time: 3:30p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added work path
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 5/23/02    Time: 4:18p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added more support for other scenes
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added output filenames
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/10/02    Time: 2:52p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Configuration file working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * skeleton filter chain complete
 */

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "DataManager.h"
#include "FilePath.h"

// TupTreeEntry methods
TupTreeEntry::TupTreeEntry(void) : m_aggregate(NULL)
{
}

TupTreeEntry::TupTreeEntry(const char *filename)
{
	int result;
	m_filename = filename;
	result = TupperwareLoadSaveManager::Read(m_filename,&m_aggregate);
	if (!result)
	{
		m_aggregate = NULL;
	}
}

TupTreeEntry::TupTreeEntry(TupperwareAggregate *pInitialDataAgg, const char *filename)
{
	int result;
	m_filename = filename;
	if (pInitialDataAgg)
	{
      m_aggregate = new TupperwareAggregate;
      m_aggregate->CopyChildren(pInitialDataAgg); // start with a copy of the data
	}
	else
	{
		result = TupperwareLoadSaveManager::Read(m_filename,&m_aggregate);
		if (!result)
		{
			m_aggregate = NULL;
		}
	}
}


TupTreeEntry::TupTreeEntry(const TupTreeEntry &s)
{
	m_filename = s.m_filename;
	m_aggregate = s.m_aggregate;
}

TupTreeEntry &TupTreeEntry::operator=(const TupTreeEntry &s)
{
	m_filename = s.m_filename;
	m_aggregate = s.m_aggregate;
	return (*this);

}

const char *TupTreeEntry::GetFilename(void) const
{
	return m_filename;
}

TupperwareAggregate *TupTreeEntry::GetAggregate(void)
{
	return m_aggregate;
}

// FilterDataManager methods
FilterDataManager::FilterDataManager(const char *inputFilename,const char *outputFilename,const char *workDirectory,const char *pluginDirectory,const char *configDirectory,ImageManager &imageManager) : 
	m_inputFilename(inputFilename), m_outputFilename(outputFilename), m_workDirectory(workDirectory), m_pluginDirectory(pluginDirectory), m_configDirectory(configDirectory),m_imageManager(imageManager)
{
	TupTreeEntry sceneEntry(inputFilename);
	m_tupTreeArray.Add(sceneEntry);
}

FilterDataManager::FilterDataManager(TupperwareAggregate *pInitialDataAgg, const char *inputFilename,const char *outputFilename,const char *workDirectory,const char *pluginDirectory,const char *configDirectory,ImageManager &imageManager) : 
m_inputFilename(inputFilename), m_outputFilename(outputFilename), m_workDirectory(workDirectory), m_pluginDirectory(pluginDirectory), m_configDirectory(configDirectory),m_imageManager(imageManager)
{
	TupTreeEntry sceneEntry(pInitialDataAgg,inputFilename);
	m_tupTreeArray.Add(sceneEntry);
}

FilterDataManager::~FilterDataManager()
{
	int total = m_tupTreeArray.GetSize();
	for (int i=0;i<total;i++)
	{
		TupperwareAggregate *agg = m_tupTreeArray[i].GetAggregate();
		if (agg)
		{
			delete agg; // kill each tree
		}
	}
}

TupperwareAggregate *FilterDataManager::GetSceneRoot(void)
{
	assert(m_tupTreeArray.GetSize());
	return m_tupTreeArray[0].GetAggregate();
}

const char *FilterDataManager::GetSceneFilename(void) const
{
	assert(m_tupTreeArray.GetSize());
	return m_tupTreeArray[0].GetFilename();
}

const char *FilterDataManager::GetInputFilename(void) const
{
	return m_inputFilename;
}

const char *FilterDataManager::GetOutputFilename(void) const
{
	return m_outputFilename;
}

const char *FilterDataManager::GetWorkDirectory(void) const
{
	return m_workDirectory;
}

const char *FilterDataManager::GetPluginDirectory(void) const
{
	return m_pluginDirectory;
}

const char *FilterDataManager::GetConfigDirectory(void) const
{
	return m_configDirectory;
}


TupperwareAggregate *FilterDataManager::GetOtherSceneRoot(const char *filename)
{
	TupString relativePath,fullPath,name,ext;
	// get full path of filename
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,filename,m_inputFilename);

	int total = m_tupTreeArray.GetSize();
	for (int i=1;i<total;i++)
	{
		const char *otherFilename = m_tupTreeArray[i].GetFilename();
		if (!stricmp(fullPath,otherFilename))
		{
			return m_tupTreeArray[i].GetAggregate();
		}
	}
	// if it is not there then load it...
	TupTreeEntry otherSceneEntry(fullPath);
	m_tupTreeArray.Add(otherSceneEntry);
	return otherSceneEntry.GetAggregate();
}

// return an index (starting at 0) for the other scene
int FilterDataManager::GetOtherSceneIndex(const char *filename) 
{
	TupString relativePath,fullPath,name,ext;
	// get full path of filename
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,filename,m_inputFilename);

	int total = m_tupTreeArray.GetSize();
	for (int i=1;i<total;i++)
	{
		const char *otherFilename = m_tupTreeArray[i].GetFilename();
		if (!stricmp(fullPath,otherFilename))
		{
			return i-1;
		}
	}
	// if it is not there then load it...
	TupTreeEntry otherSceneEntry(fullPath);
	m_tupTreeArray.Add(otherSceneEntry);
	return m_tupTreeArray.GetSize()-2; // -1 for total -1 for starting at other scenes
}

int FilterDataManager::GetNumOtherScenes(void) const
{
	assert(m_tupTreeArray.GetSize());
	return m_tupTreeArray.GetSize()-1;
}

TupperwareAggregate *FilterDataManager::GetOtherSceneRoot(int index)
{
	int total = m_tupTreeArray.GetSize();
	assert(index+1>0 && index+1<total);
	return m_tupTreeArray[index+1].GetAggregate();
}

const char *FilterDataManager::GetOtherSceneFilename(int index) const
{
	int total = m_tupTreeArray.GetSize();
	assert(index+1>0 && index+1<total);
	return m_tupTreeArray[index+1].GetFilename();
}

ImageManager &FilterDataManager::GetImageManager(void)
{
	return m_imageManager;
}

