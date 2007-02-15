////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// classes to handle the plugin description for the plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: PluginDesc.cpp $
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

#include "StampPch.h"

#include "Stamp.h"
#include "PluginDesc.h"

// unique ID for filter
#define STAMP_PLUGIN_ID	PluginID(0x51554d8d, 0x559c21fb)

FilterPlugin *StampPluginDesc::Create(TupperChainLogBase &log)
{
	return new StampFilter(log); 
}

PluginID StampPluginDesc::GetPluginID(void)
{
	return STAMP_PLUGIN_ID; 
}

HINSTANCE StampPluginDesc::HInstance()
{
	return hInstance;
}
