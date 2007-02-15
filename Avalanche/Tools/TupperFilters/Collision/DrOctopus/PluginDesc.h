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
 * User: Food         Date: 5/09/03    Time: 1:19a
 * Updated in $/Avalanche/tools/TupperFilters/DrOctopus
 * 
 * *****************  Version 1  *****************
 * User: Food         Date: 5/05/03    Time: 6:52p
 * Created in $/Avalanche/tools/TupperFilters/DrOctopus
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 4:23p
 * Updated in $/Avalanche/tools/TupperFilters/TerrainToola
 * Start of TerrainToola filter
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_DESC_H
#define PLUGIN_DESC_H

class FilterPlugin;
class TupperChainLogBase;

class DrOctopusPluginDesc : public FilterPluginDesc
{
public:
	~DrOctopusPluginDesc() {}

	FilterPlugin *Create(TupperChainLogBase &log);
	PluginID GetPluginID();
	HINSTANCE HInstance();
};

#endif
