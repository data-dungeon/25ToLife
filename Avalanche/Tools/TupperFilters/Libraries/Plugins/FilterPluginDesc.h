////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// Base class for all plugin descriptors
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: FilterPluginDesc.h $
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
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * skeleton filter chain complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/08/02    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Filter chain files created
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_FILTER_PLUGIN_DESC_H
#define TUPPERFILTERS_LIB_PLUGINS_FILTER_PLUGIN_DESC_H

#ifndef TUPPERFILTERS_LIB_PLUGINS_PLUGIN_ID_H
#include "PluginID.h"
#endif

class FilterPlugin;
class TupperChainLogBase;

class FilterPluginDesc
{
protected:
	HINSTANCE hInstance;

public:
	virtual ~FilterPluginDesc() {}
	virtual FilterPlugin *Create(TupperChainLogBase &log)=0;
	virtual PluginID GetPluginID()=0;
	virtual HINSTANCE HInstance()=0;
};

#endif
