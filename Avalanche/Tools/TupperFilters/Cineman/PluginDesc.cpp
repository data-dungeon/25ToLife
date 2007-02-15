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
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:30p
 * Created in $/Avalanche/tools/TupperFilters/Cineman
 */

#include "CinemanPch.h"

#include "Cineman.h"
#include "PluginDesc.h"

// unique ID for filter
#define CINEMAN_PLUGIN_ID	PluginID(0x170d7567, 0x566b0c5a)

FilterPlugin *CinemanPluginDesc::Create(TupperChainLogBase &log)
{
	return new CinemanFilter(log); 
}

PluginID CinemanPluginDesc::GetPluginID(void)
{
	return CINEMAN_PLUGIN_ID; 
}

HINSTANCE CinemanPluginDesc::HInstance()
{
	return hInstance;
}
