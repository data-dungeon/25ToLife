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
 * User: Adam Clayton Date: 5/16/03    Time: 1:30p
 * Created in $/Avalanche/tools/TupperFilters/Cineman
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_DESC_H
#define PLUGIN_DESC_H

class FilterPlugin;
class TupperChainLogBase;

class CinemanPluginDesc : public FilterPluginDesc
{
public:
	~CinemanPluginDesc() {}
	FilterPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
