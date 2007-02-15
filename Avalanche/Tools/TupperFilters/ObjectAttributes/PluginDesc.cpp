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
 * User: Adam Clayton Date: 3/27/02    Time: 1:13p
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * First pass of filter
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/27/02    Time: 10:20a
 * Created in $/Avalanche/tools/TupperFilters/ObjectAttributes
 */

#include "ObjectAttributesPch.h"

#include "ObjectAttributes.h"
#include "PluginDesc.h"

// unique ID for filter
#define OBJECT_ATTRIBUTES_PLUGIN_ID	PluginID(0x4a130f8e, 0x7f46aaf)

FilterPlugin *ObjectAttributesPluginDesc::Create(TupperChainLogBase &log)
{
	return new ObjectAttributesFilter(log); 
}

PluginID ObjectAttributesPluginDesc::GetPluginID(void)
{
	return OBJECT_ATTRIBUTES_PLUGIN_ID; 
}

HINSTANCE ObjectAttributesPluginDesc::HInstance()
{
	return hInstance;
}
