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
 * User: Adam Clayton Date: 3/14/02    Time: 9:49a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 */

#include "Pch.h"

#include "OBBWorld.h"
#include "PluginDesc.h"

// unique ID for filter
#define OBB_WORLD_PLUGIN_ID	PluginID(0x9612156, 0x3b0d1192)

FilterPlugin *OBBWorldPluginDesc::Create(TupperChainLogBase &log)
{
	return new OBBWorldFilter(log); 
}

PluginID OBBWorldPluginDesc::GetPluginID(void)
{
	return OBB_WORLD_PLUGIN_ID; 
}

HINSTANCE OBBWorldPluginDesc::HInstance()
{
	return hInstance;
}
