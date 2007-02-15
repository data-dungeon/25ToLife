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
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 2:05p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Start of PS2 Filter
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

#include "ExportPS2.h"
#include "PluginDesc.h"

// unique ID for filter
#define EXPORT_PS2_PLUGIN_ID	PluginID(0x5539324b, 0xb1a778d)

FilterPlugin *ExportPS2PluginDesc::Create(TupperChainLogBase &log)
{
	return new ExportPS2Filter(log); 
}

PluginID ExportPS2PluginDesc::GetPluginID(void)
{
	return EXPORT_PS2_PLUGIN_ID; 
}

HINSTANCE ExportPS2PluginDesc::HInstance()
{
	return hInstance;
}
