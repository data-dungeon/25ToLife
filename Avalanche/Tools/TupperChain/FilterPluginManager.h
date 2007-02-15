////////////////////////////////////////////////////////////////////////////
//
// FilterPluginManager
//
// Class handle the plugin filters
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: FilterPluginManager.h $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:47p
 * Updated in $/Avalanche/tools/TupperChain
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperChain
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:58p
 * Updated in $/Avalanche/tools/TupperChain
 * Added Precompiled Header
 * Added Chain manager for checking plugin dependencies
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:49p
 * Updated in $/Avalanche/tools/TupperChain
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperChain
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:19p
 * Updated in $/Avalanche/tools/TupperChain
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/10/02    Time: 2:52p
 * Updated in $/Avalanche/tools/TupperChain
 * Configuration file working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperChain
 * skeleton filter chain complete
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERCHAIN_FILTER_PLUGIN_MANAGER_H
#define TUPPERCHAIN_FILTER_PLUGIN_MANAGER_H

class FilterPlugin;
class TupperChainLogBase;

class FilterPluginManagerPlugin
{
	TupString m_filename;
	HINSTANCE m_hInstance;
	FilterPlugin *m_pFilterPlugin;
	TupperChainLogBase *m_pLog;

public:
	FilterPluginManagerPlugin(void);
	FilterPluginManagerPlugin(const char *pluginPathname,const char *filename,TupperChainLogBase &log);
	~FilterPluginManagerPlugin();
	FilterPlugin *GetFilterPlugin(void);
	HINSTANCE GetInstance(void);
	const char *GetFilename(void);
};


class FilterPluginManager
{
	// these are not defined
	FilterPluginManager &operator=(const FilterPluginManager &s); 
	FilterPluginManager(const FilterPluginManager &s);

	TupperChainLogBase &m_log;

	TupString m_pluginPathname;

	TupArray<FilterPluginManagerPlugin> m_filterPluginArray;
public:
	FilterPluginManager(TupperChainLogBase &log,const char *pluginPathname);
	~FilterPluginManager(void);

	bool FilterPluginLoaded(const char *filename);
	FilterPlugin *GetFilterPlugin(const char *filename);
};

#endif
