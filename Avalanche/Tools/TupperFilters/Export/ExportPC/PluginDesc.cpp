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

#include "ExportPC.h"
#include "PluginDesc.h"

// unique ID for filter
#define EXPORT_PC_PLUGIN_ID	PluginID(0x33177409, 0x52951eba)

FilterPlugin *ExportPCPluginDesc::Create(TupperChainLogBase &log)
{
	return new ExportPCFilter(log); 
}

PluginID ExportPCPluginDesc::GetPluginID(void)
{
	return EXPORT_PC_PLUGIN_ID; 
}

HINSTANCE ExportPCPluginDesc::HInstance()
{
	return hInstance;
}
