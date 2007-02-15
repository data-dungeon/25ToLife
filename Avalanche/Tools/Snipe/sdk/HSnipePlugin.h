//
#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

#include "HPlugin.h"

class HModelTranslatorPlugin;
class HCommandPlugin;
class HCommandUIPlugin;
class HManipulatorPlugin;
class HNavigatorPlugin;
class HSceneTranslatorPlugin;
class HMotionTranslatorPlugin;
class HMotionSetTranslatorPlugin;

typedef void (* SDK_PFN_DESTROY_PLUGINDATA)( void *pPluginData );

class SNIPEEXPORT HSnipePlugin
{
public:
   void SetName(const char *szName);

   //Entry Types
   HModelTranslatorPlugin *AddModelTranslatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   HSceneTranslatorPlugin *AddSceneTranslatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   HMotionTranslatorPlugin *AddMotionTranslatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   HMotionSetTranslatorPlugin *AddMotionSetTranslatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );

   HCommandPlugin *AddCommandPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   HCommandUIPlugin *AddCommandUIPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   HManipulatorPlugin *AddManipulatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   HNavigatorPlugin *AddNavigatorPlugin( void *pPluginData, SDK_PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
};