////////////////////////////////////////////////////////////////////////////
//
// ChainManager
//
// Class handle the issues between plugin filters
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ChainManager.h $
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

#ifndef TUPPERCHAIN_CHAIN_MANAGER_H
#define TUPPERCHAIN_CHAIN_MANAGER_H

class TupperChainLogBase;
class FilterPluginManager;
class ConfigManager;

class ChainManager
{
public:
	class FilterDependency
	{
		TupString m_key;
		Revision m_revision;
	public:
		FilterDependency(void);
		FilterDependency(const char *key,Revision revision);
		const char *GetKey(void) const;
		Revision GetRevision(void) const;
	};

	class FilterDependencies
	{
		TupArray<FilterDependency> m_writeDependencyArray;
		TupArray<FilterDependency> m_readDependencyArray;
		const char *m_filterName;
	public:
		FilterDependencies(void);
		FilterDependencies(const char *filterName);
		void AddWriteDependency(const FilterDependency &filterDependency);
		void AddReadDependency(const FilterDependency &filterDependency);
		int GetNumWriteDependencies(void) const;
		int GetNumReadDependencies(void) const;
		const FilterDependency &GetWriteDependency(int index) const;
		const FilterDependency &GetReadDependency(int index) const;
		const char *GetFilterName(void) const;
	};

	class FilterDependencyManager
	{
	   	TupperChainLogBase &m_log;
		TupArray<FilterDependencies> m_dependencyArray;
	public:
		FilterDependencyManager(TupperChainLogBase &log);
		void AddFilter(const FilterDependencies &filterDependencies);
		void Check(void);
	};

private:
	FilterPluginManager &m_filterPluginManager;
	ConfigManager &m_configManager;
	TupperChainLogBase &m_log;

public:
	ChainManager(FilterPluginManager &filterPluginManager,ConfigManager &configManager,TupperChainLogBase &log);
	~ChainManager();
	void CheckDependencies(void);
};

#endif
