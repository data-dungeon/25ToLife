//
#include "stdafx.h"
#include "HSnipePlugin.h"
#include "SnipePlugin.h"
#include "HPlugin.h"

void HSnipePlugin::SetName(const char *szName)
{
   ((SnipePlugin *)this)->SetName(szName);
}

HModelTranslatorPlugin *HSnipePlugin::AddModelTranslatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData )
{
   return (HModelTranslatorPlugin *)((SnipePlugin *)this)->AddModelTranslatorPlugin( pPluginData, (PFN_DESTROY_PLUGINDATA)pfnDestroy_PluginData );
}

HSceneTranslatorPlugin *HSnipePlugin::AddSceneTranslatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData )
{
   return (HSceneTranslatorPlugin *)((SnipePlugin *)this)->AddSceneTranslatorPlugin( pPluginData, (PFN_DESTROY_PLUGINDATA)pfnDestroy_PluginData );
}

HMotionTranslatorPlugin *HSnipePlugin::AddMotionTranslatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData )
{
   return (HMotionTranslatorPlugin *)((SnipePlugin *)this)->AddMotionTranslatorPlugin( pPluginData, (PFN_DESTROY_PLUGINDATA)pfnDestroy_PluginData );
}

HMotionSetTranslatorPlugin *HSnipePlugin::AddMotionSetTranslatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData )
{
   return (HMotionSetTranslatorPlugin *)((SnipePlugin *)this)->AddMotionSetTranslatorPlugin( pPluginData, (PFN_DESTROY_PLUGINDATA)pfnDestroy_PluginData );
}

HCommandPlugin *HSnipePlugin::AddCommandPlugin(void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   return (HCommandPlugin *)((SnipePlugin *)this)->AddCommandPlugin( pPluginData, (PFN_DESTROY_PLUGINDATA)pfnDestroy_PluginData );
}

HCommandUIPlugin *HSnipePlugin::AddCommandUIPlugin(void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   return (HCommandUIPlugin *)((SnipePlugin *)this)->AddCommandUIPlugin( pPluginData, (PFN_DESTROY_PLUGINDATA)pfnDestroy_PluginData );
}

HManipulatorPlugin *HSnipePlugin::AddManipulatorPlugin(void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   return (HManipulatorPlugin *)((SnipePlugin *)this)->AddManipulatorPlugin( pPluginData, (PFN_DESTROY_PLUGINDATA)pfnDestroy_PluginData );
}

HNavigatorPlugin *HSnipePlugin::AddNavigatorPlugin(void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   return (HNavigatorPlugin *)((SnipePlugin *)this)->AddNavigatorPlugin( pPluginData, (PFN_DESTROY_PLUGINDATA)pfnDestroy_PluginData );
}