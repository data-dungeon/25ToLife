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
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Start
 */

#include "BreakLargeMeshesPch.h"

#include "BreakLargeMeshes.h"
#include "PluginDesc.h"

// unique ID for filter
#define MERGE_XREFS_PLUGIN_ID	PluginID(0x36556728, 0x11db2973)

FilterPlugin *BreakLargeMeshesPluginDesc::Create(TupperChainLogBase &log)
{
	return new BreakLargeMeshesFilter(log); 
}

PluginID BreakLargeMeshesPluginDesc::GetPluginID(void)
{
	return MERGE_XREFS_PLUGIN_ID; 
}

HINSTANCE BreakLargeMeshesPluginDesc::HInstance()
{
	return hInstance;
}
