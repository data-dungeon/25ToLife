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
 * User: Adam Clayton Date: 3/12/02    Time: 12:55p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Moved ExportPC marking code to this filter
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_DESC_H
#define PLUGIN_DESC_H

class FilterPlugin;
class TupperChainLogBase;

class AnnotatePluginDesc : public FilterPluginDesc
{
public:
	~AnnotatePluginDesc() {}
	FilterPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
