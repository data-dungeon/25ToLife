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
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 4:25p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Added Collage Image Reader
 * Uses Paintlib (16 extensions)
 * Can read file and add width and height to tup
 * More to come...
 */

#include "CollagePch.h"

#include "Collage.h"
#include "PluginDesc.h"

// unique ID for filter
#define COLLAGE_PLUGIN_ID	PluginID(0xd37bb9, 0x8a375c4)

ImageReaderPlugin *CollagePluginDesc::Create(TupperChainLogBase &log)
{
	return new CollageImageReader(log); 
}

PluginID CollagePluginDesc::GetPluginID(void)
{
	return COLLAGE_PLUGIN_ID; 
}

HINSTANCE CollagePluginDesc::HInstance()
{
	return hInstance;
}
