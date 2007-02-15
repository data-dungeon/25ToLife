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
 * User: Adam Clayton Date: 11/15/04   Time: 3:23p
 * Created in $/Avalanche/Tools/TupperFilters/MeshPath
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/24/03    Time: 9:41a
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 * Added Stamp Filter
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/24/03    Time: 9:29a
 * Created in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_DESC_H
#define PLUGIN_DESC_H

class FilterPlugin;
class TupperChainLogBase;

class MeshPathPluginDesc : public FilterPluginDesc
{
public:
	~MeshPathPluginDesc() {}
	FilterPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
