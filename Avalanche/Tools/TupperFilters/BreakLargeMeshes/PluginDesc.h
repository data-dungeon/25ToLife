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
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Start
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 3:59p
 * Created in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/28/02    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Moved MergeXRefs from annotate filter to its own filter
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:55p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Moved ExportPC marking code to this filter
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_DESC_H
#define PLUGIN_DESC_H

class FilterPlugin;
class TupperChainLogBase;

class BreakLargeMeshesPluginDesc : public FilterPluginDesc
{
public:
	~BreakLargeMeshesPluginDesc() {}
	FilterPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
