////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// classes to handle the plugin description for the plugin
//
////////////////////////////////////////////////////////////////////////////

#include "MeshPathPch.h"

#include "MeshPath.h"
#include "PluginDesc.h"

// unique ID for filter
#define MESH_PATH_PLUGIN_ID	PluginID(0x6e940141, 0x6d1e04c8)

FilterPlugin *MeshPathPluginDesc::Create(TupperChainLogBase &log)
{
	return new MeshPathFilter(log); 
}

PluginID MeshPathPluginDesc::GetPluginID(void)
{
	return MESH_PATH_PLUGIN_ID; 
}

HINSTANCE MeshPathPluginDesc::HInstance()
{
	return hInstance;
}
