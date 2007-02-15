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
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 7/29/03    Time: 2:58p
 * Created in $/Avalanche/tools/TupperFilters/ConvertObjects
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ConvertObjects
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/ConvertObjects
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/ConvertObjects
 * skeleton filter chain complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/08/02    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/ConvertObjects
 * Filter chain files created
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_DESC_H
#define PLUGIN_DESC_H

class FilterPlugin;
class TupperChainLogBase;

class ConvertObjectsPluginDesc : public FilterPluginDesc
{
public:
	~ConvertObjectsPluginDesc() {}
	FilterPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
