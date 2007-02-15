#pragma once

#include "Plugin.h"
#include "Interface/MainInterface.h"

class ModelTranslatorPlugin;
class CommandUIPlugin;
class CommandPlugin;
class ManipulatorPlugin;
class NavigatorPlugin;
class SceneTranslatorPlugin;
class MotionTranslatorPlugin;
class MotionSetTranslatorPlugin;

class SnipePlugin
{
private:
   HINSTANCE m_hinstance;
   String m_strFileName;
   String m_strName;

   PluginList m_pluginlistarray[NUM_PLUGINTYPES]; 

public:
   SnipePlugin(HINSTANCE hinstance, const String &strFileName) :
    m_hinstance(hinstance),
    m_strFileName(strFileName)
   {
   }
   ~SnipePlugin();

   void SetName(const String &strName) { m_strName = strName; }
   String GetName() const { return m_strName; }
   String GetFileName() const { return m_strFileName; }
   HINSTANCE GetHInstance() const { return m_hinstance; }

   PluginList *GetPluginList(UINT nIndex) { ASSERT(nIndex < NUM_PLUGINTYPES); return &m_pluginlistarray[ nIndex ]; } 
   ModelTranslatorPlugin *AddModelTranslatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   SceneTranslatorPlugin *AddSceneTranslatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   MotionTranslatorPlugin *AddMotionTranslatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   MotionSetTranslatorPlugin *AddMotionSetTranslatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );
   CommandUIPlugin *AddCommandUIPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData);
   CommandPlugin *AddCommandPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData);
   ManipulatorPlugin *AddManipulatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData);
   NavigatorPlugin *AddNavigatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData);
};