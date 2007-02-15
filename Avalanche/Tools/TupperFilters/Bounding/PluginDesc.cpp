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
 * User: Adam Clayton Date: 4/04/03    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/Bounding
 * new bounding filter for use in annotate and break large meshes
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/31/03    Time: 3:13p
 * Created in $/Avalanche/tools/TupperFilters/Bounding
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

#include "BoundingPch.h"

#include "BoundingFilter.h"
#include "PluginDesc.h"

// unique ID for filter
#define BOUNDING_PLUGIN_ID	PluginID(0x57ff54fb, 0x4c003bf2)

FilterPlugin *BoundingPluginDesc::Create(TupperChainLogBase &log)
{
	return new BoundingFilter(log); 
}

PluginID BoundingPluginDesc::GetPluginID(void)
{
	return BOUNDING_PLUGIN_ID; 
}

HINSTANCE BoundingPluginDesc::HInstance()
{
	return hInstance;
}
