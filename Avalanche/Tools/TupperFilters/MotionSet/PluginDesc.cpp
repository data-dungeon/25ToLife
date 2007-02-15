////////////////////////////////////////////////////////////////////////////
//
// PluginDesc
//
// classes to handle the plugin description for the plugin
//
////////////////////////////////////////////////////////////////////////////

#include "MotionSetPch.h"

#include "MotionSet.h"
#include "PluginDesc.h"

// unique ID for filter
#define MOTIONSET_PLUGIN_ID	PluginID(0x23ac43ef, 0x73f22573)

FilterPlugin *MotionSetPluginDesc::Create(TupperChainLogBase &log)
{
	return new MotionSetFilter(log); 
}

PluginID MotionSetPluginDesc::GetPluginID(void)
{
	return MOTIONSET_PLUGIN_ID; 
}

HINSTANCE MotionSetPluginDesc::HInstance()
{
	return hInstance;
}
