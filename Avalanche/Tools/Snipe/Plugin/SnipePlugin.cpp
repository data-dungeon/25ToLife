//
#include "stdafx.h"
#include "SnipePlugin.h"
#include "ModelTranslatorPlugin.h"
#include "SceneTranslatorPlugin.h"
#include "MotionTranslatorPlugin.h"
#include "MotionSetTranslatorPlugin.h"
#include "CommandPlugin.h"
#include "CommandUIPlugin.h"
#include "ManipulatorPlugin.h"
#include "NavigatorPlugin.h"

SnipePlugin::~SnipePlugin()
{
   for (int i=0; i < NUM_PLUGINTYPES; i++ )
      for (POSITION pos = m_pluginlistarray[i].GetHeadPosition(); pos; )
         delete m_pluginlistarray[i].GetNext(pos);

   FreeLibrary(m_hinstance);
}

ModelTranslatorPlugin *SnipePlugin::AddModelTranslatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   ModelTranslatorPlugin *pPlugin = SNEW ModelTranslatorPlugin(this, pPluginData, pfnDestroy_PluginData);
   m_pluginlistarray[MODELTRANSLATOR_PLUGIN].AddTail(pPlugin);
   return pPlugin;
}

SceneTranslatorPlugin *SnipePlugin::AddSceneTranslatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   SceneTranslatorPlugin *pPlugin = SNEW SceneTranslatorPlugin(this, pPluginData, pfnDestroy_PluginData);
   m_pluginlistarray[SCENETRANSLATOR_PLUGIN].AddTail(pPlugin);
   return pPlugin;
}

MotionTranslatorPlugin *SnipePlugin::AddMotionTranslatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   MotionTranslatorPlugin *pPlugin = SNEW MotionTranslatorPlugin(this, pPluginData, pfnDestroy_PluginData);
   m_pluginlistarray[MOTIONTRANSLATOR_PLUGIN].AddTail(pPlugin);
   return pPlugin;
}

MotionSetTranslatorPlugin *SnipePlugin::AddMotionSetTranslatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   MotionSetTranslatorPlugin *pPlugin = SNEW MotionSetTranslatorPlugin(this, pPluginData, pfnDestroy_PluginData);
   m_pluginlistarray[MOTIONSETTRANSLATOR_PLUGIN].AddTail(pPlugin);
   return pPlugin;
}

CommandPlugin *SnipePlugin::AddCommandPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   CommandPlugin *pPlugin = SNEW CommandPlugin(this, pPluginData, pfnDestroy_PluginData);
   m_pluginlistarray[COMMAND_PLUGIN].AddTail(pPlugin);
   return pPlugin;
}

CommandUIPlugin *SnipePlugin::AddCommandUIPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   CommandUIPlugin *pPlugin = SNEW CommandUIPlugin(this, pPluginData, pfnDestroy_PluginData);
   m_pluginlistarray[COMMANDUI_PLUGIN].AddTail(pPlugin);
   return pPlugin;
}

ManipulatorPlugin *SnipePlugin::AddManipulatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   ManipulatorPlugin *pPlugin = SNEW ManipulatorPlugin(this, pPluginData, pfnDestroy_PluginData);
   m_pluginlistarray[MANIPULATOR_PLUGIN].AddTail(pPlugin);
   return pPlugin;
}

NavigatorPlugin *SnipePlugin::AddNavigatorPlugin(void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData)
{
   NavigatorPlugin *pPlugin = SNEW NavigatorPlugin(this, pPluginData, pfnDestroy_PluginData);
   m_pluginlistarray[NAVIGATOR_PLUGIN].AddTail(pPlugin);
   return pPlugin;
}
