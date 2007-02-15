////////////////////////////////////////////////////////////////////////////
//
// ChainManager
//
// Class handle the chain of plugin filters
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ChainManager.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/22/02    Time: 3:43p
 * Updated in $/Avalanche/tools/TupperChain
 * keeper changes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperChain
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:58p
 * Updated in $/Avalanche/tools/TupperChain
 * Added Precompiled Header
 * Added Chain manager for checking plugin dependencies
 */

////////////////////////////////////////////////////////////////////////////

#include "TupperChainPch.h"

#include "ChainManager.h"
#include "FilterPluginManager.h"
#include "ConfigManager.h"

// ChainManager::FilterDependency methods
ChainManager::FilterDependency::FilterDependency(void)
{
}

ChainManager::FilterDependency::FilterDependency(const char *key,Revision revision) : m_key(key), m_revision(revision)
{
}

const char *ChainManager::FilterDependency::GetKey(void) const
{
	return m_key;
}

Revision ChainManager::FilterDependency::GetRevision(void) const
{
	return m_revision;
}

// ChainManager::FilterDependencies methods
ChainManager::FilterDependencies::FilterDependencies(void) : m_filterName(NULL)
{
}

ChainManager::FilterDependencies::FilterDependencies(const char *filterName) : m_filterName(filterName)
{
}

void ChainManager::FilterDependencies::AddWriteDependency(const ChainManager::FilterDependency &filterDependency)
{
	m_writeDependencyArray.Add(filterDependency);
}

void ChainManager::FilterDependencies::AddReadDependency(const ChainManager::FilterDependency &filterDependency)
{
	m_readDependencyArray.Add(filterDependency);
}

int ChainManager::FilterDependencies::GetNumWriteDependencies(void) const
{
	return m_writeDependencyArray.GetSize();
}

int ChainManager::FilterDependencies::GetNumReadDependencies(void) const
{
	return m_readDependencyArray.GetSize();
}

const ChainManager::FilterDependency &ChainManager::FilterDependencies::GetWriteDependency(int index) const
{
	assert(index>=0&&index<m_writeDependencyArray.GetSize());
	return m_writeDependencyArray[index];
}

const ChainManager::FilterDependency &ChainManager::FilterDependencies::GetReadDependency(int index) const
{
	assert(index>=0&&index<m_readDependencyArray.GetSize());
	return m_readDependencyArray[index];
}

const char *ChainManager::FilterDependencies::GetFilterName(void) const
{
	return m_filterName;
}

// ChainManager::FilterDependencyManager methods
ChainManager::FilterDependencyManager::FilterDependencyManager(TupperChainLogBase &log) : m_log(log)
{
}

void ChainManager::FilterDependencyManager::AddFilter(const FilterDependencies &filterDependencies)
{
	m_dependencyArray.Add(filterDependencies);
}

void ChainManager::FilterDependencyManager::Check(void)
{
	bool error = false;

	int totalFilters = m_dependencyArray.GetSize();
	for (int filterIndex = 0;filterIndex<totalFilters;filterIndex++)
	{
		const FilterDependencies &filterDependencies = m_dependencyArray[filterIndex];
		int totalReadDependencies = filterDependencies.GetNumReadDependencies();
		for (int readDependencyIndex = 0;readDependencyIndex<totalReadDependencies;readDependencyIndex++)
		{
			const FilterDependency &readDependency = filterDependencies.GetReadDependency(readDependencyIndex);
			const char *readKey = readDependency.GetKey();
			Revision readRevision = readDependency.GetRevision();

			bool writeFound = false;

			for (int searchFilterIndex=0;searchFilterIndex<filterIndex;searchFilterIndex++)
			{
				const FilterDependencies &searchFilterDependencies = m_dependencyArray[searchFilterIndex];
				int totalWriteDependencies = searchFilterDependencies.GetNumWriteDependencies();
				for (int writeDependencyIndex = 0;writeDependencyIndex<totalWriteDependencies;writeDependencyIndex++)
				{
					const FilterDependency &writeDependency = searchFilterDependencies.GetWriteDependency(writeDependencyIndex);
					const char *writeKey = writeDependency.GetKey();
					Revision writeRevision = writeDependency.GetRevision();
					// if we have a match
					if (!strcmp(readKey,writeKey))
					{
						if (readRevision==writeRevision)
						{
							writeFound = true;
							break;
						}
						else
						{
							m_log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_RED|LogFlags::FONT_STYLE_BOLD,"Dependency Revision Conflict - Filter %s Key %s Revision %d.%d Depends on Filter %s Key %s Revision %d.%d",
								filterDependencies.GetFilterName(),readKey,readRevision.GetMajor(),readRevision.GetMinor(),
								searchFilterDependencies.GetFilterName(),writeKey,writeRevision.GetMajor(),writeRevision.GetMinor());
							error = true;
						}
					}
				}
				if (writeFound)
					break;
			}
			if (!writeFound)
			{
				m_log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_RED|LogFlags::FONT_STYLE_BOLD,"Write Dependency Not Found - Filter %s Key %s Revision %d.%d",
					filterDependencies.GetFilterName(),readKey,readRevision.GetMajor(),readRevision.GetMinor());
				error = true;
			}
		}
	}
	if (error)
	{
		throw(PluginException(ERROR_FATAL,"Dependency Check"));
	}
}


// ChainManager methods
ChainManager::ChainManager(FilterPluginManager &filterPluginManager,ConfigManager &configManager,TupperChainLogBase &log) :
	m_filterPluginManager(filterPluginManager), m_configManager(configManager), m_log(log)
{
}

ChainManager::~ChainManager()
{
}

void ChainManager::CheckDependencies(void)
{
	int totalPasses = m_configManager.GetNumPasses();
	FilterDependencyManager filterDependencyManager(m_log);

	for (int i=0;i<totalPasses;i++)
	{
		const char *filterName = m_configManager.GetFilterName(i);
		int pass = m_configManager.GetFilterPass(i);
		FilterPlugin *pFilter = m_filterPluginManager.GetFilterPlugin(filterName);
		if (!pFilter)
		{
			throw(PluginException(ERROR_FATAL,"ChainManager - Filter %s not found.",filterName));
		}
		FilterDependencies filterDependencies(filterName);

		int writeDependencies = pFilter->GetTotalWriteDependencies(pass,m_configManager.GetFilterOptions(i));
		for (int dependencyIndex = 0;dependencyIndex<writeDependencies;dependencyIndex++)
		{
			FilterDependency newDependency(pFilter->GetWriteDependencyKey(pass,m_configManager.GetFilterOptions(i),dependencyIndex),
				pFilter->GetWriteDependencyRevision(pass,m_configManager.GetFilterOptions(i),dependencyIndex));
			filterDependencies.AddWriteDependency(newDependency);
		}

		int readDependencies = pFilter->GetTotalReadDependencies(pass,m_configManager.GetFilterOptions(i));
		for (dependencyIndex = 0;dependencyIndex<readDependencies;dependencyIndex++)
		{
			FilterDependency newDependency(pFilter->GetReadDependencyKey(pass,m_configManager.GetFilterOptions(i),dependencyIndex),
				pFilter->GetReadDependencyRevision(pass,m_configManager.GetFilterOptions(i),dependencyIndex));
			filterDependencies.AddReadDependency(newDependency);
		}
		filterDependencyManager.AddFilter(filterDependencies);
	}
	filterDependencyManager.Check();
}
