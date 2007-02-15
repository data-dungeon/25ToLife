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
 * User: Adam Clayton Date: 8/20/02    Time: 3:11p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Duplicate Faces & Multitex Start
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 8/19/02    Time: 4:08p
 * Created in $/Avalanche/tools/TupperFilters/MultiTex
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Start
 */

#include "MultiTexPch.h"

#include "MultiTex.h"
#include "PluginDesc.h"

// unique ID for filter
#define MULTI_TEX_PLUGIN_ID	PluginID(0x27127226, 0x68031555)

FilterPlugin *MultiTexPluginDesc::Create(TupperChainLogBase &log)
{
	return new MultiTexFilter(log); 
}

PluginID MultiTexPluginDesc::GetPluginID(void)
{
	return MULTI_TEX_PLUGIN_ID; 
}

HINSTANCE MultiTexPluginDesc::HInstance()
{
	return hInstance;
}
