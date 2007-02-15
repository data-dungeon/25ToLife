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

class CombineMeshesPluginDesc : public FilterPluginDesc
{
public:
	~CombineMeshesPluginDesc() {}
	FilterPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
