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
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * skeleton filter chain complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/08/02    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Filter chain files created
 */

#include "MakeYUpPch.h"

#include "MakeYup.h"
#include "PluginDesc.h"

// unique ID for filter
#define MAKE_Y_UP_PLUGIN_ID	PluginID(0x32bd4032, 0x1c090acf)

FilterPlugin *MakeYUpPluginDesc::Create(TupperChainLogBase &log)
{
	return new MakeYUpFilter(log); 
}

PluginID MakeYUpPluginDesc::GetPluginID(void)
{
	return MAKE_Y_UP_PLUGIN_ID; 
}

HINSTANCE MakeYUpPluginDesc::HInstance()
{
	return hInstance;
}
