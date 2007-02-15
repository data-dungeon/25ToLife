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
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/13/02    Time: 10:31a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Starting point for Gamecube Exporter
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Start of Tupperchain filter for the PC 
 * Currently got as far to output the SOM data (should not work yet)
 */

#include "ExportPch.h"

#include "ExportGCN.h"
#include "PluginDesc.h"

// unique ID for filter
#define EXPORT_GCN_PLUGIN_ID	PluginID(0x24d70fd3, 0x691466a2)

FilterPlugin *ExportGCNPluginDesc::Create(TupperChainLogBase &log)
{
	return new ExportGCNFilter(log); 
}

PluginID ExportGCNPluginDesc::GetPluginID(void)
{
	return EXPORT_GCN_PLUGIN_ID; 
}

HINSTANCE ExportGCNPluginDesc::HInstance()
{
	return hInstance;
}
