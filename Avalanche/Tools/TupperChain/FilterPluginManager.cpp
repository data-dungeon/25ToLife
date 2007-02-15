////////////////////////////////////////////////////////////////////////////
//
// FilterPluginManager
//
// Class handle the plugin filters
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: FilterPluginManager.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:47p
 * Updated in $/Avalanche/tools/TupperChain
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperChain
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:58p
 * Updated in $/Avalanche/tools/TupperChain
 * Added Precompiled Header
 * Added Chain manager for checking plugin dependencies
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/11/02    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperChain
 * Memory Errors
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

#include "TupperChainPch.h"

#include "FilterPluginManager.h"

// Plugin Methods
FilterPluginManagerPlugin::FilterPluginManagerPlugin(void) : m_pLog(NULL)
{
}

FilterPluginManagerPlugin::FilterPluginManagerPlugin(const char *pluginPathname,const char *filename,TupperChainLogBase &log) : m_pLog(&log)
{
	m_filename = filename;
	m_pFilterPlugin = NULL; // start with a null filter

	TupString relativePath,fullPath,name,ext;
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,filename,pluginPathname);

	// load dll into memory
	m_hInstance = LoadLibraryEx(fullPath, NULL, 0);
	if (m_hInstance)
	{
		const char*	(*vDescription)();
		FilterPluginDesc*	(*vPluginDesc)();
		ULONG (*vVersion)();

		vDescription = (const char *(*)())GetProcAddress(m_hInstance, "LibDescription");
		vPluginDesc = (FilterPluginDesc *(*)())GetProcAddress(m_hInstance, "LibPluginDesc");
		vVersion = (ULONG(*)())GetProcAddress(m_hInstance, "LibVersion");

		if (vDescription && vPluginDesc && vVersion)
		{
			ULONG version = vVersion();
			if (version==VERSION_FILTER_CHAIN)
			{
				FilterPluginDesc *pluginDesc = vPluginDesc();
				if (pluginDesc)
				{
					m_pFilterPlugin = (FilterPlugin *)pluginDesc->Create(log);
				}
			}
		}
	}
}

FilterPluginManagerPlugin::~FilterPluginManagerPlugin()
{
}

FilterPlugin *FilterPluginManagerPlugin::GetFilterPlugin(void)
{
	return m_pFilterPlugin;
}

HINSTANCE FilterPluginManagerPlugin::GetInstance(void)
{
	return m_hInstance;
}

const char *FilterPluginManagerPlugin::GetFilename(void)
{
	return m_filename;
}

// PluginManager Methods
FilterPluginManager::FilterPluginManager(TupperChainLogBase &log, const char *pluginPathname) : 
	m_log(log), m_pluginPathname(pluginPathname)
{
}

FilterPluginManager::~FilterPluginManager(void)
{
	// free plugins
	int total = m_filterPluginArray.GetSize();
	for (int i=0;i<total;i++)
	{
		FilterPluginManagerPlugin &plugin = m_filterPluginArray[i];
		HINSTANCE hInstance = plugin.GetInstance();
		delete plugin.GetFilterPlugin();
		if (hInstance)
		{
			FreeLibrary(hInstance);
		}
	}
}

bool FilterPluginManager::FilterPluginLoaded(const char *filename)
{
	int total = m_filterPluginArray.GetSize();
	for (int i=0;i<total;i++)
	{
		FilterPluginManagerPlugin &plugin = m_filterPluginArray[i];
		const char *pluginFilename = plugin.GetFilename();
		if (!stricmp(pluginFilename,filename))
		{
			return true;
		}
	}
	return false;
}

FilterPlugin *FilterPluginManager::GetFilterPlugin(const char *filename)
{
	int total = m_filterPluginArray.GetSize();
	for (int i=0;i<total;i++)
	{
		FilterPluginManagerPlugin &plugin = m_filterPluginArray[i];
		const char *pluginFilename = plugin.GetFilename();
		if (!stricmp(pluginFilename,filename))
		{
			return plugin.GetFilterPlugin();
		}
	}
	FilterPluginManagerPlugin newPlugin(m_pluginPathname,filename,m_log);
	m_filterPluginArray.Add(newPlugin);
	return newPlugin.GetFilterPlugin();
}
		