////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// classes to handle the plugin description for the plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: PluginDesc.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/TriStrip
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/TriStrip
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/17/02    Time: 2:08p
 * Updated in $/Avalanche/tools/TupperFilters/TriStrip
 * Start of tristrip project
 */

#include "TriStripPch.h"

#include "TriStrip.h"
#include "PluginDesc.h"

// unique ID for filter
#define TRI_STRIP_PLUGIN_ID	PluginID(0x7909719c, 0x61ef3580)


FilterPlugin *TriStripPluginDesc::Create(TupperChainLogBase &log)
{
	return new TriStripFilter(log); 
}

PluginID TriStripPluginDesc::GetPluginID(void)
{
	return TRI_STRIP_PLUGIN_ID; 
}

HINSTANCE TriStripPluginDesc::HInstance()
{
	return hInstance;
}
