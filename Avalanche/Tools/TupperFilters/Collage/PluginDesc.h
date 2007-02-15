////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// descriptor for plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: PluginDesc.h $
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

////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_DESC_H
#define PLUGIN_DESC_H

class ImageReaderPlugin;

class CollagePluginDesc : public ImageReaderPluginDesc
{
public:
	~CollagePluginDesc() {}
	ImageReaderPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
