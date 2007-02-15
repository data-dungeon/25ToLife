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

#include "MergeXRefsPch.h"

#include "MergeXRefs.h"
#include "PluginDesc.h"

// unique ID for filter
#define MERGE_XREFS_PLUGIN_ID	PluginID(0x4b05f45, 0x4a543e70)

FilterPlugin *MergeXRefsPluginDesc::Create(TupperChainLogBase &log)
{
	return new MergeXRefsFilter(log); 
}

PluginID MergeXRefsPluginDesc::GetPluginID(void)
{
	return MERGE_XREFS_PLUGIN_ID; 
}

HINSTANCE MergeXRefsPluginDesc::HInstance()
{
	return hInstance;
}
