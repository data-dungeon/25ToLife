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
 * User: Adam Clayton Date: 3/12/02    Time: 12:55p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Moved ExportPC marking code to this filter
 */

#include "AnnotatePch.h"

#include "Annotate.h"
#include "PluginDesc.h"

// unique ID for filter
#define ANNOTATE_PLUGIN_ID	PluginID(0x6c873d89, 0x7171799e)

FilterPlugin *AnnotatePluginDesc::Create(TupperChainLogBase &log)
{
	return new AnnotateFilter(log); 
}

PluginID AnnotatePluginDesc::GetPluginID(void)
{
	return ANNOTATE_PLUGIN_ID; 
}

HINSTANCE AnnotatePluginDesc::HInstance()
{
	return hInstance;
}
