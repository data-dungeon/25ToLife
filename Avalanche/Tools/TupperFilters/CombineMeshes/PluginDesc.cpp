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
 * Updated in $/Avalanche/tools/TupperFilters/CombineMeshes
 * Duplicate Faces & CombineMeshes Start
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 8/19/02    Time: 4:08p
 * Created in $/Avalanche/tools/TupperFilters/CombineMeshes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/CombineMeshes
 * Start
 */

#include "CombineMeshesPch.h"

#include "CombineMeshes.h"
#include "PluginDesc.h"

// unique ID for filter
#define COMBINE_MESHES_PLUGIN_ID	PluginID(0x77df035c, 0x460970c8)

FilterPlugin *CombineMeshesPluginDesc::Create(TupperChainLogBase &log)
{
	return new CombineMeshesFilter(log); 
}

PluginID CombineMeshesPluginDesc::GetPluginID(void)
{
	return COMBINE_MESHES_PLUGIN_ID; 
}

HINSTANCE CombineMeshesPluginDesc::HInstance()
{
	return hInstance;
}
