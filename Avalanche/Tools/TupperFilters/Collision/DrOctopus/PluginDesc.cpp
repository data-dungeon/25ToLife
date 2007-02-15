////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// classes to handle the plugin description for the plugin
//
////////////////////////////////////////////////////////////////////////////
#include "Pch.h"
#include "PluginDesc.h"

// unique ID for filter
#define DROCTOPUS_PLUGIN_ID	PluginID(0x1a4a3abc, 0x06181968)

FilterPlugin *DrOctopusPluginDesc::Create(TupperChainLogBase &log)
{
	return new DrOctopusFilter(log);
}

PluginID DrOctopusPluginDesc::GetPluginID(void)
{
	return DROCTOPUS_PLUGIN_ID;
}

HINSTANCE DrOctopusPluginDesc::HInstance()
{
	return hInstance;
}
