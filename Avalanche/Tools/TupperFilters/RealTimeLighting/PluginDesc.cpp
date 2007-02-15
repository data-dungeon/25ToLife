////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// classes to handle the plugin description for the plugin
//
////////////////////////////////////////////////////////////////////////////


#include "RealTimeLightingPch.h"
#include "RealTimeLighting.h"
#include "PluginDesc.h"

// unique ID for filter
#define MERGE_XREFS_PLUGIN_ID	PluginID(0x41a52a38, 0x68171d25)

FilterPlugin *RealTimeLightingPluginDesc::Create(TupperChainLogBase &log)
{
	return new RealTimeLightingFilter(log); 
}

PluginID RealTimeLightingPluginDesc::GetPluginID(void)
{
	return MERGE_XREFS_PLUGIN_ID; 
}

HINSTANCE RealTimeLightingPluginDesc::HInstance()
{
	return hInstance;
}
