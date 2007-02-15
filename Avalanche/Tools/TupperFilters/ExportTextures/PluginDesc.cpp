////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// classes to handle the plugin description for the plugin
//
////////////////////////////////////////////////////////////////////////////

#include "ExportTexturesPch.h"

#include "ExportTextures.h"
#include "PluginDesc.h"

// unique ID for filter
#define EXPORT_TEXTURES_PLUGIN_ID	PluginID(0x11c34b2e, 0x6b85c65)

FilterPlugin *ExportTexturesPluginDesc::Create(TupperChainLogBase &log)
{
	return new ExportTexturesFilter(log); 
}

PluginID ExportTexturesPluginDesc::GetPluginID(void)
{
	return EXPORT_TEXTURES_PLUGIN_ID; 
}

HINSTANCE ExportTexturesPluginDesc::HInstance()
{
	return hInstance;
}
