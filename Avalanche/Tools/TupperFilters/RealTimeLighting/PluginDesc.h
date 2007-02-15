////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// descriptor for plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_DESC_H
#define PLUGIN_DESC_H

class FilterPlugin;
class TupperChainLogBase;

class RealTimeLightingPluginDesc : public FilterPluginDesc
{
public:
	~RealTimeLightingPluginDesc() {}
	FilterPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
