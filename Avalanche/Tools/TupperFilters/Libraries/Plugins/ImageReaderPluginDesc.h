////////////////////////////////////////////////////////////////////////////
//
// ImageReaderPluginDesc
//
// Base class for all image reader plugin descriptors
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageReaderPluginDesc.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_IMAGE_PLUGINS_IMAGE_PLUGIN_DESC_H
#define TUPPERFILTERS_LIB_IMAGE_PLUGINS_IMAGE_PLUGIN_DESC_H

#ifndef TUPPERFILTERS_LIB_PLUGINS_PLUGIN_ID_H
#include "TupperFilters/Libraries/Plugins/PluginID.h"
#endif

class ImageReaderPlugin;
class TupperChainLogBase;

class ImageReaderPluginDesc
{
protected:
	HINSTANCE hInstance;

public:
	virtual ~ImageReaderPluginDesc() {}
	virtual ImageReaderPlugin *Create(TupperChainLogBase &log)=0;
	virtual PluginID GetPluginID()=0;
	virtual HINSTANCE HInstance()=0;
};

#endif
