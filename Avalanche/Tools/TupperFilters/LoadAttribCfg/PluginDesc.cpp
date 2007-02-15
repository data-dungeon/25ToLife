////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// classes to handle the plugin description for the plugin
//
////////////////////////////////////////////////////////////////////////////

#include "LoadAttribCfgPch.h"

#include "LoadAttribCfg.h"
#include "PluginDesc.h"

// unique ID for filter
#define LOAD_ATTRIB_CFG_PLUGIN_ID	PluginID(0x57236b9c, 0x5a18715d)

FilterPlugin *LoadAttribCfgPluginDesc::Create(TupperChainLogBase &log)
{
	return new LoadAttribCfgFilter(log); 
}

PluginID LoadAttribCfgPluginDesc::GetPluginID(void)
{
	return LOAD_ATTRIB_CFG_PLUGIN_ID; 
}

HINSTANCE LoadAttribCfgPluginDesc::HInstance()
{
	return hInstance;
}
